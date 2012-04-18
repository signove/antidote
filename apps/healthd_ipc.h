#ifndef HEALTHD_IPC_
#define HEALTHD_IPC_

typedef struct {
	void (*call_agent_measurementdata)(ContextId, char *);
	void (*call_agent_connected)(ContextId, const char *);
	void (*call_agent_disconnected)(ContextId, const char *);
	void (*call_agent_associated)(ContextId, char *);
	void (*call_agent_disassociated)(ContextId);
	void (*call_agent_segmentinfo)(ContextId, unsigned int, char *);
	void (*call_agent_segmentdataresponse)(ContextId, unsigned int, unsigned int, unsigned int);
	void (*call_agent_segmentdata)(ContextId, unsigned int, unsigned int, char *);
	void (*call_agent_segmentcleared)(ContextId, unsigned int, unsigned int, unsigned int);
	void (*call_agent_pmstoredata)(ContextId, unsigned int, char *);
	void (*call_agent_deviceattributes)(ContextId, char *xml);
	void (*start)();
	void (*stop)();
} healthd_ipc;

#endif
