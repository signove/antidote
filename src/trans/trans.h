#ifndef _SRC_COMMUNICATION_TRANS_TRANS_H
#define _SRC_COMMUNICATION_TRANS_TRANS_H

#include <src/asn1/phd_types.h>
#include <src/communication/context.h>

typedef int (*agent_connected_cb)(ContextId id, const char *lladdr);
typedef int (*agent_disconnected_cb)(ContextId id, const char *lladdr);

typedef struct TransPlugin 
{
	int (*init)();
	int (*finalize)();
	void (*force_disconnect)(char *);
	agent_connected_cb conn_cb;
	agent_disconnected_cb disconn_cb;
} TransPlugin;

/* Methods called by plugins */
void trans_register_plugin(TransPlugin *plugin);

int trans_connected(TransPlugin *plugin,
			char *lladdr,
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

#endif
