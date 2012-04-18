#ifndef HEALTHD_SERVICE_H_
#define HEALTHD_SERVICE_H_

#include <stdint.h>

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
void device_get_segmdata(ContextId ctx, int handle, int instnumber, int* ret);
void device_clearsegmdata(ContextId ctx, int handle, int instnumber, int *ret);
void device_clearallsegmdata(ContextId ctx, int handle, int *ret);

void hdp_types_configure(uint16_t hdp_data_types[]);
#endif
