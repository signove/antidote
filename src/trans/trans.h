#ifndef _SRC_COMMUNICATION_TRANS_TRANS_H
#define _SRC_COMMUNICATION_TRANS_TRANS_H

#include <src/asn1/phd_types.h>
#include <src/communication/context.h>

/**
 * \defgroup Transcoding Transcoding
 * \brief Transcoding layer API for non-11073 devices
 * @{
 */

/**
 * Function called by transcoding plugin when agent connects
 */
typedef int (*agent_connected_cb)(ContextId id, const char *lladdr);
/**
 * Function called by transcoding plugin when agent disconnects
 */
typedef int (*agent_disconnected_cb)(ContextId id, const char *lladdr);

/**
 * Transcoding plugin structure
 */
typedef struct TransPlugin 
{
	/**
	 * Plug-in initialization function (called by engine)
	 */
	int (*init)();

	/**
	 * Plug-in finalization function (called by engine)
	 */
	int (*finalize)();

	/**
	 * Connection forced closing function (called by engine)
	 */
	void (*force_disconnect)(const char *);

	/**
	 * Agent connected callback
	 */
	agent_connected_cb conn_cb;

	/**
	 * Agent disconnected callback
	 */
	agent_disconnected_cb disconn_cb;
} TransPlugin;

/* Methods called by plugins */
void trans_register_plugin(TransPlugin *plugin);

void trans_finalize();

int trans_connected(TransPlugin *plugin,
			char *lladdr,
			AttributeList spec,
			PhdAssociationInformation assoc_info,
			ConfigReport config);

int trans_event_report_fixed(TransPlugin *plugin,
			char *lladdr,
			ScanReportInfoFixed report);

int trans_event_report_var(TransPlugin *plugin,
			char *lladdr,
			ScanReportInfoVar report);

int trans_disconnected(TransPlugin *plugin, char *lladdr);

/* Called by dummy trans communication plug-in */
void trans_force_disconnect(ContextId id);

/** @} */

#endif
