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
	 * Set-Time action ported to transcoding
	 */
	int (*set_time)(const char *, int, SetTimeInvoke);

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

void trans_set_time_response(char *lladdr, int invoke_id, int ok);

/* Methods called by stack */
int trans_set_time(ContextId id, int invoke_id, SetTimeInvoke time);

/* Called by dummy trans communication plug-in */
void trans_force_disconnect(ContextId id);

/** @} */

#endif
