#include <string.h>
#include <src/util/linkedlist.h>
#include <src/util/log.h>
#include <src/communication/context_manager.h>
#include <src/communication/communication.h>
#include <src/communication/parser/struct_cleaner.h>
#include <src/communication/association.h>
#include <src/communication/configuring.h>
#include <src/dim/mds.h>
#include "trans.h"

/**
 * \addtogroup Transcoding
 * \brief Transcoding layer implementation for non-11073 devices
 * @{
 */

/**
 * List of loaded transcoding plugins
 */
static LinkedList *_plugins = NULL;

/**
 * Pointer to 'dummy' communication plugin that represents transcoded devices.
 * The only task that this plugin actually does it timer servicing.
 */
CommunicationPlugin *trans_comm_plugin = 0;

/**
 * Internal structure representing a known transcoded device.
 * It relates a low-level address with a context.
 */
typedef struct TransDevice
{
	/**
	 * Low level address. Actual format is defined by transcoding plug-in.
	 * At this level, it is essentially opaque. The plug-in is responsible
	 * by generating unique lladdrs across all plug-ins.
	 */
	char *lladdr;

	/**
	 * Related stack Context id of the device.
	 * The "plugin id" inside this is always the same:
	 * the dummy communication plug-in at communication/plugin/trans.
	 */
	ContextId context;

	/**
	 * Origin transcoding plug-in for this device.
	 * The transcoding plug-in where the device is really brought to life.
	 */
	TransPlugin *plugin;
} TransDevice;

/**
 * Initial Context Id for a transcoded device
 */
#define INITIAL_TRANS_CONTEXT 0x324

/**
 * Context ID generator for all transcoded devices
 */
static unsigned long long int new_context = INITIAL_TRANS_CONTEXT;

/**
 * List of known transcoded devices
 */
static LinkedList *_devices = NULL;

// TODO multithreading protection

/**
 * Returns the list of loaded transcodings plugins
 * @return list of transcoding plugins
 */
static LinkedList *plugins()
{
	if (! _plugins) {
		_plugins = llist_new();
	}
	return _plugins;
}

/**
 * Returns the list of loaded transcodings plugins
 * @return list of transcoding plugins
 */
static LinkedList *devices()
{
	if (! _devices) {
		_devices = llist_new();
	}
	return _devices;
}

static int search_by_addr(void *parg, void *pelement)
{
	char *lladdr = parg;
	TransDevice *element = pelement;

	if (strcmp(element->lladdr, lladdr) == 0) {
		return 1;
	}

	return 0;
}

/**
 * Returns a transcoded device struct, search by low-level addr
 * @param lladdr low-level address as string
 * @return transcoded device or NULL if not found
 */
static TransDevice *get_device_by_addr(char *lladdr)
{
	TransDevice *dev = llist_search_first(devices(), lladdr,
						search_by_addr);
	return dev;
}

/**
 * \cond Undocumented
 */
static int search_by_context(void *parg, void *pelement)
{
	TransDevice *element = pelement;
	ContextId context = *((ContextId*) parg);

	if ((element->context.plugin == context.plugin) &&
				element->context.connid == context.connid) {
		return 1;
	}

	return 0;
}
/**
 * \endcond
 */

/**
 * Find device by context id
 *
 * @param id Context id
 * @return TransDevice struct or NULL if not found
 */
static TransDevice *get_device_by_context(ContextId id)
{
	TransDevice *dev = llist_search_first(devices(), &id,
						search_by_context);
	return dev;
}

/*
static char *get_addr_by_context(ContextId id)
{
	TransDevice *dev = get_device_by_context(id);
	if (dev) {
		return dev->lladdr;
	}
	return NULL;
}
*/

/**
 * Get a context ID for a transcoded device
 * Implicitly generates a new context if device is new
 *
 * @param lladdr Low-level address, format is opaque
 * @param plugin The related transcoding plug-in for the device
 * @return Context ID
 */
ContextId trans_context_get(char *lladdr, TransPlugin *plugin)
{
	TransDevice *dev = get_device_by_addr(lladdr);
	if (dev) {
		return dev->context;
	} else if (! trans_comm_plugin) {
		ERROR("Transcoding comm plugin not loaded");
	} else if (plugin) {
		dev = malloc(sizeof(TransDevice));
		ContextId c = {communication_plugin_id(trans_comm_plugin),
				new_context++};
		new_context++;
		dev->context = c;
		dev->lladdr = strdup(lladdr);
		dev->plugin = plugin;
		llist_add(devices(), dev);
		return dev->context;
	} else {
		ERROR("Trans context w/ unknown plugin");
	}
	ContextId c = {0, 0};
	return c;
}

/**
 * Destroy a transcoded device structure (TransDevice)
 * Used in stack finalization phase
 *
 * @param device TransDevice structure
 */
static void destroy_device(TransDevice *dev)
{
	free(dev->lladdr);
	dev->lladdr = 0;
	dev->plugin = 0;
	free(dev);
}

/**
 * Register a transcoding plug-in at this engine
 *
 * @param plugin the plug-in descriptor
 */
void trans_register_plugin(TransPlugin *plugin)
{
	llist_add(plugins(), plugin);
	plugin->init();
}

/**
 * Unregister a transcoding plug-in 
 * Used in stack finalization phase
 * @param plugin the plug-in descriptor
 */
static void unregister_plugin(TransPlugin *plugin)
{
	plugin->finalize();
	free(plugin);
}

/**
 * Finalization clean-up: clear all plugins and devices
 */ 
void trans_finalize()
{
	if (_plugins) {
		llist_destroy(_plugins, (llist_handle_element) unregister_plugin);
		_plugins = NULL;
	}

	if (_devices) {
		llist_destroy(_devices, (llist_handle_element) destroy_device);
		_devices = NULL;
	}
	
	new_context = INITIAL_TRANS_CONTEXT;
}

/**
 * Called by any transcoding plug-in when a device connects
 *
 * @param plugin the transcoding plugin
 * @param lladdr low-level address of device (opaque at this level)
 * @param spec MDS attributes, at the very least must contain specialization
 * @param assoc_info association information in 11073 format
 * @param config device configuration in 11073 format
 * @return 0 if error, >0 if ok
 */
int trans_connected(TransPlugin *plugin,
			char *lladdr,
			AttributeList spec,
			PhdAssociationInformation assoc_info,
			ConfigReport config)
{
	// create context, if any
	ContextId context = trans_context_get(lladdr, plugin);

	if (!context.plugin) {
		DEBUG("Transcoding: context not found for %s", lladdr);
		return 0;
	}

	if (plugin->conn_cb)
		plugin->conn_cb(context, lladdr);

	communication_transport_connect_indication(context);

	Context *ctx = context_get(context);
	if (!ctx) {
		DEBUG("Transcoding: context struct not found");
		return 0;
	}

	association_accept_data_protocol_20601_in(ctx, assoc_info, 1);
	del_phdassociationinformation(&assoc_info);

	// following call deletes config_report (if necessary)
	configuring_perform_configuration_in(ctx, config, NULL, 1);

	int i;
	for (i = 0; i < spec.count; i++) {
		AVA_Type *attribute = &spec.value[i];
		mds_set_attribute(ctx->mds, attribute);
	}

	del_attributelist(&spec);

	return 1;
}

/**
 * Called by any transcoding plug-in when measurement data is received
 *
 * @param plugin the transcoding plugin
 * @param lladdr low-level address of device (opaque at this level)
 * @param report Measurement data in 11073 format
 * @return 0 if error, >0 if ok
 */
int trans_event_report_fixed(TransPlugin *plugin,
				char *lladdr,
				ScanReportInfoFixed report)
{
	// passing plugin = NULL indirectly blocks creation
	ContextId context = trans_context_get(lladdr, NULL);

	if (!context.plugin) {
		DEBUG("Transcoded %s no context for evt report", lladdr);
		return 0;
	}
	Context *ctx = context_get(context);
	if (!ctx) {
		DEBUG("Transcoding: context struct not found");
		return 0;
	}

	mds_event_report_dynamic_data_update_fixed(ctx, &report);
	del_scanreportinfofixed(&report);
	return 1;
}

/**
 * Called by any transcoding plug-in when measurement data is received
 *
 * @param plugin the transcoding plugin
 * @param lladdr low-level address of device (opaque at this level)
 * @param report Measurement data in 11073 format
 * @return 0 if error, >0 if ok
 */
int trans_event_report_var(TransPlugin *plugin,
			char *lladdr,
			ScanReportInfoVar report)
{
	// passing plugin = NULL indirectly blocks creation
	ContextId context = trans_context_get(lladdr, NULL);

	if (!context.plugin) {
		DEBUG("Transcoded %s no context for evt report (var)", lladdr);
		return 0;
	}
	Context *ctx = context_get(context);
	if (!ctx) {
		DEBUG("Transcoding: context struct not found");
		return 0;
	}

	mds_event_report_dynamic_data_update_var(ctx, &report);
	del_scanreportinfovar(&report);
	return 1;
}

/**
 * Called by any transcoding plug-in when a device disconnects
 *
 * @param plugin the transcoding plugin
 * @param lladdr low-level address of device (opaque at this level)
 * @return 0 if error, >0 if ok
 */
int trans_disconnected(TransPlugin *plugin, char *lladdr)
{
	// passing plugin = NULL indirectly blocks creation
	ContextId context = trans_context_get(lladdr, NULL);

	if (!context.plugin) {
		DEBUG("Transcoded %s no context for disconnection", lladdr);
		return 0;
	}

	communication_transport_disconnect_indication(context);

	if (plugin->disconn_cb)
		plugin->disconn_cb(context, lladdr);

	return 1;
}

/**
 * Called by 11073 stack when a connection must be brought down
 *
 * @param id the Context id of device
 */
void trans_force_disconnect(ContextId id)
{
	TransDevice *dev = get_device_by_context(id);
	if (! dev) {
		DEBUG("Transcoded dev: unknown context for forced disconnection");
	}
	dev->plugin->force_disconnect(dev->lladdr);
	communication_transport_disconnect_indication(id);
}

/** @} */
