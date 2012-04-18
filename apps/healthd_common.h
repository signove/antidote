#ifndef HEALTHD_COMMON_
#define HEALTHD_COMMON_

#include "src/communication/context_manager.h"
#include "src/api/api_definitions.h"

void new_data_received(Context *ctx, DataList *list);
void segment_data_received(Context *ctx, int handle, int instnumber, DataList *list);
void device_associated(Context *ctx, DataList *list);
int device_connected(Context *ctx, const char *low_addr);
int device_disconnected(Context *ctx, const char *low_addr);
void device_disassociated(Context *ctx);
void device_reqmdsattr(ContextId ctx);
void device_getconfig(ContextId ctx, char** xml_out);
void device_reqmeasurement(ContextId ctx);
void device_set_time(ContextId ctx, unsigned long long time);
void device_reqactivationscanner(ContextId ctx, int handle);
void device_reqdeactivationscanner(ContextId ctx, int handle);
void device_releaseassoc(ContextId ctx);
void device_abortassoc(ContextId ctx);
void device_get_pmstore(ContextId ctx, int handle, int* ret);
void device_get_segminfo(ContextId ctx, int handle, int* ret);
void device_get_segmdata(ContextId ctx, int handle, int instnumber,
				int* ret);
void device_clearsegmdata(ContextId ctx, int handle, int instnumber,
				int *ret);
void device_clearallsegmdata(ContextId ctx, int handle, int *ret);

#endif
