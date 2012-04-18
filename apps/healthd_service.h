#ifndef HEALTHD_SERVICE_H_
#define HEALTHD_SERVICE_H_

#include <stdint.h>

void hdp_types_configure(uint16_t hdp_data_types[]);
void healthd_idle_add(void*, void*);
#endif
