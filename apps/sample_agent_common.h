#ifndef SAMPLE_AGENT_COMMON_H_
#define SAMPLE_AGENT_COMMON_H_

struct mds_system_data;
extern intu8 AGENT_SYSTEM_ID_VALUE[];

void *oximeter_event_report_cb();
void *blood_pressure_event_report_cb();
void *weightscale_event_report_cb();
void *glucometer_event_report_cb();
struct mds_system_data *mds_data_cb();

#endif
