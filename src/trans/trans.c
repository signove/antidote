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

static LinkedList *_plugins = NULL;

extern CommunicationPlugin *trans_comm_plugin;

typedef struct TransDevice
{
	char *lladdr;
	ContextId context;
	TransPlugin *plugin;
} TransDevice;

static unsigned long long int new_context = 991;

static LinkedList *_devices = NULL;

static LinkedList *plugins()
{
	if (! _plugins) {
		_plugins = llist_new();
	}
	return _plugins;
}

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

static TransDevice *get_device_by_addr(char *lladdr)
{
	TransDevice *dev = llist_search_first(devices(), lladdr,
						search_by_addr);
	return dev;
}

static int search_by_context(void *parg, void *pelement)
{
	TransDevice *element = element;
	ContextId context = *((ContextId*) parg);

	if ((element->context.plugin == context.plugin) &&
				element->context.connid == context.connid) {
		return 1;
	}

	return 0;
}

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

void trans_register_plugin(TransPlugin *plugin)
{
	llist_add(plugins(), plugin);
	plugin->init();
}

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

	// following call deletes config_report (if necessary)
	configuring_perform_configuration_in(ctx, config, NULL, 1);
	// del_phdassociationinformation(&assoc_info);

	int i;
	for (i = 0; i < spec.count; i++) {
		AVA_Type *attribute = &spec.value[i];
		mds_set_attribute(ctx->mds, attribute);
	}

	del_attributelist(&spec);

	return 1;
}

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
	// del_scanreportinfofixed(&report);
	return 1;
}

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

void trans_force_disconnect(ContextId id)
{
	TransDevice *dev = get_device_by_context(id);
	if (! dev) {
		DEBUG("Transcoded dev: unknown context for forced disconnection");
	}
	dev->plugin->force_disconnect(dev->lladdr);
	communication_transport_disconnect_indication(id);
}
