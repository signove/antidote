/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file blood_pressure_monitor.c
 * \brief Blood Pressure Monitor Specialization Implementation
 *
 * Copyright (C) 2010 Signove Tecnologia Corporation.
 * All rights reserved.
 * Contact: Signove Tecnologia Corporation (contact@signove.com)
 *
 * $LICENSE_TEXT:BEGIN$
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation and appearing
 * in the file LICENSE included in the packaging of this file; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 * $LICENSE_TEXT:END$
 *
 * \date Jun 24, 2010
 * \author Jose Martins, Fabricio Silva
 */

#include <stdio.h>
#include "src/specializations/blood_pressure_monitor.h"
#include "src/util/bytelib.h"
#include "src/dim/mds.h"
#include "src/dim/nomenclature.h"
#include "src/util/dateutil.h"
#include "communication/parser/encoder_ASN1.h"
#include "communication/parser/struct_cleaner.h"


/**
 * @defgroup DeviceSpecializations Device Specializations
 * @brief IEEE 11073-20601 Device Specializations. For more
 * information about device specializations at IEEE library
 * , see documentation on \ref SpecializationDescription "IEEE device specializations"
 * @page SpecializationDescription IEEE Device Specialization
 *
 *
 * The ISO/IEEE 11073-104zz device specializations define mandatory
 * objects and attributes that shall exist within an agents configuration.
 * Furthermore, each of the specializations defines mandatory elements
 * (e.g., including mandatory actions and methods) of the service and
 * communication models, which have to be supported by an agent following
 * that specialization.
 *
 * \par Procedure to add a new specialization
 *
 * The manager is the entity responsible for the management of all
 * specializations and it is the start point to add any new standard
 * specialization. The steps to add a new specialization are described
 * below:
 *
 * 1 – Create a new configuration (one configuration for each
 * specialization). A standard configuration is represented in
 * the system by a struct named StdConfiguration. This struct
 * has the following attributes that must be defined:
 *
 *		\b dev_config_id – The hexadecimal constant for the device.
 *
 *		\b configure_action – Returns the configuration of the device.
 *
 *		\b ds_to_string_function – Process the MDS received by manager
 * from device and returns a string.
 *
 * 2 – Register the new configuration.
 * In the manager_init() function of manager.c file the developer
 * has to add one or more calls to the function std_configurations_register_conf
 * (struct StdConfiguration *config). This will register the new
 * configuration within the manager.
 *
 * For example, the following code registers the standard configuration of
 * the <em>Pulse Oximeter Monitor</em> specialization (the specialization
 * provides two different standard configurations).
 *
 * \code
 *
 * std_configurations_register_conf(pulse_oximeter_create_std_config_ID0190());
 * std_configurations_register_conf(pulse_oximeter_create_std_config_ID0191());
 *
 * \endcode
 *
 */


/**
 * \defgroup BloodPressure Blood Pressure Monitor
 * \ingroup DeviceSpecializations
 * \brief Blood Pressure Monitor IEEE specialization
 *
 * A blood pressure monitor is a device that measures blood pressure
 * [i.e., systolic, diastolic, and mean arterial pressure (MAP)] and,
 * optionally, pulse noninvasively. Blood pressure monitor devices
 * considered in this standard typically inflate a cuff to occlude an
 * artery and then to measure the reaction of the artery while the
 * pressure is released with the results being converted into systolic,
 * diastolic, and MAP values. Optionally, pulse rate may be determined
 * at the same time.
 *
 * Blood pressure monitor devices may use a variety of techniques for
 * measuring blood pressure and pulse rate. One typical method is the
 * oscillometric method where oscillations in cuff pressure are analyzed
 * to obtain blood pressure values. Another technique is the automated
 * auscultatory method where the device uses a microphone to detect
 * Korotkoff sounds during cuff deflation.
 *
 * Auscultatory devices measure the systolic and diastolic values
 * and estimates the MAP. In home monitors, the oscillometric method
 * is typically used, allowing the measurement to be done electronically.
 * On the oscillometric method, small pressure changes (oscillations)
 * occur in the cuff as a result of blood pressure pulses during the
 * inflation or deflation of the cuff and are detected. These oscillations,
 * which first increase and then decrease, are stored together with
 * the corresponding cuff pressure values in the automated sphygmomanometer.
 * With these stored values, the systolic, diastolic, and mean blood
 * pressure values can be mathematically derived using an appropriate algorithm.
 *
 * The blood pressure receives two different measurements: 1) Systolic, diastolic, MAP
 * and 2) Pulse rate.
 *
 * The measures are read from a Numeric object and data are saved at
 * numeric->compound_basic_nu_observed_value->value field. The following example reads
 * systolic, diastolic and MAP data, and simply prints them.
 *
 * \code
 *
 * struct MDS_object* obj1 = mds_get_object_by_handle(HANDLE_MEASUREMENT_1);
 *
 *	if (obj1 != NULL && obj1->choice == MDS_OBJ_METRIC) {
 *
 *		if (obj1->u.metric.choice == METRIC_NUMERIC){
 *			struct Numeric* numeric = &obj1->u.metric.u.numeric;
 *
 *			if (numeric->compound_basic_nu_observed_value.count > 0){
 *				unsigned char *unit_label = mds_get_unit_code_string(numeric->metric.unit_code);
 *
 *				sprintf(str, "%s Pressure systolic (%s): %.2f,", str, unit_label,
 *							  numeric->compound_basic_nu_observed_value.value[0]);
 *				sprintf(str, "%s diastolic: %.2f,",str,
 *							  numeric->compound_basic_nu_observed_value.value[1]);
 *				sprintf(str, "%s mean: %.2f\n", str,
 *							  numeric->compound_basic_nu_observed_value.value[2]);
 *
 *				sprintf(str, "%s Measurement time: %2.X/%.2X/%.2X %.2X:%.2X:%.2X\n", str,
 *							numeric->metric.absolute_time_stamp.day,
 *							numeric->metric.absolute_time_stamp.month,
 *							numeric->metric.absolute_time_stamp.year,
 *							numeric->metric.absolute_time_stamp.hour,
 *						numeric->metric.absolute_time_stamp.minute,
 *							numeric->metric.absolute_time_stamp.second);
 *			}
 *		}
 *	}
 *
 * \endcode
 *
 * @{
 */

/** Measurement 1: Systolic, diastolic, MAP */
#define HANDLE_MEASUREMENT_1 1

/** Measurement 2: Pulse rate */
#define HANDLE_MEASUREMENT_2 2

/**
 *  Returns the standard configuration for <em>Blood Pressure Monitor</em> specialization (02BC).
 *  For more information about <em>Blood Pressure Monitor</em> specialization, see IEEE 11073-10407
 *  Standard (Section 6.4, page 7).
 *
 *  \return an StdConfiguration struct that represents the standard configuration (a StdConfiguration instance)
 *  for <em>Blood Pressure Monitor</em> specialization.
 */
static ConfigObjectList *blood_pressure_monitor_get_config_ID02BC()
{
	ConfigObjectList *std_object_list = malloc(sizeof(ConfigObjectList));
	std_object_list->count = 2;
	std_object_list->length = 108;
	std_object_list->value = malloc(sizeof(ConfigObject)
					* std_object_list->count);
	std_object_list->value[0].obj_class = MDC_MOC_VMO_METRIC_NU;
	std_object_list->value[0].obj_handle = HANDLE_MEASUREMENT_1;

	AttributeList *attr_list1 = malloc(sizeof(AttributeList));
	attr_list1->count = 6;
	attr_list1->length = 56;
	attr_list1->value = malloc(attr_list1->count * sizeof(AVA_Type));

	attr_list1->value[0].attribute_id = MDC_ATTR_ID_TYPE;
	attr_list1->value[0].attribute_value.length = 4;
	ByteStreamWriter *bsw = byte_stream_writer_instance(4);
	write_intu16(bsw, MDC_PART_SCADA);
	write_intu16(bsw, MDC_PRESS_BLD_NONINV);
	attr_list1->value[0].attribute_value.value = bsw->buffer;

	attr_list1->value[1].attribute_id = MDC_ATTR_METRIC_SPEC_SMALL;
	attr_list1->value[1].attribute_value.length = 2;
	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, 0xF040); // 0xF0 0x40 intermittent, stored data, upd & msmt aperiodic, agent init, measured
	attr_list1->value[1].attribute_value.value = bsw->buffer;

	attr_list1->value[2].attribute_id = MDC_ATTR_METRIC_STRUCT_SMALL;
	attr_list1->value[2].attribute_value.length = 2;
	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, 0x0303);
	attr_list1->value[2].attribute_value.value = bsw->buffer;

	attr_list1->value[3].attribute_id = MDC_ATTR_ID_PHYSIO_LIST;
	attr_list1->value[3].attribute_value.length = 10;
	free(bsw);
	bsw = byte_stream_writer_instance(10);
	write_intu16(bsw, 0x0003); // MetricIdList.count = 3
	write_intu16(bsw, 0x0006); // MetricIdList.length = 6
	write_intu16(bsw, MDC_PRESS_BLD_NONINV_SYS);
	write_intu16(bsw, MDC_PRESS_BLD_NONINV_DIA);
	write_intu16(bsw, MDC_PRESS_BLD_NONINV_MEAN);
	attr_list1->value[3].attribute_value.value = bsw->buffer;

	attr_list1->value[4].attribute_id = MDC_ATTR_UNIT_CODE;
	attr_list1->value[4].attribute_value.length = 2;
	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, MDC_DIM_MMHG);
	attr_list1->value[4].attribute_value.value = bsw->buffer;

	attr_list1->value[5].attribute_id = MDC_ATTR_ATTRIBUTE_VAL_MAP;
	attr_list1->value[5].attribute_value.length = 12;
	free(bsw);
	bsw = byte_stream_writer_instance(12);
	write_intu16(bsw, 0x0002); // AttrValMap.count = 2
	write_intu16(bsw, 0x0008); // AttrValMap.length = 8
	write_intu16(bsw, MDC_ATTR_NU_CMPD_VAL_OBS_BASIC);
	write_intu16(bsw, 0x000A); // value length 10
	write_intu16(bsw, MDC_ATTR_TIME_STAMP_ABS);
	write_intu16(bsw, 0x0008); // value length 8
	attr_list1->value[5].attribute_value.value = bsw->buffer;

	std_object_list->value[1].obj_class = MDC_MOC_VMO_METRIC_NU;
	std_object_list->value[1].obj_handle = HANDLE_MEASUREMENT_2;
	AttributeList *attr_list2 = malloc(sizeof(AttributeList));
	attr_list2->count = 4;
	attr_list2->length = 36;
	attr_list2->value = malloc(attr_list2->count * sizeof(AVA_Type));

	attr_list2->value[0].attribute_id = MDC_ATTR_ID_TYPE;
	attr_list2->value[0].attribute_value.length = 4;
	free(bsw);
	bsw = byte_stream_writer_instance(4);
	write_intu16(bsw, MDC_PART_SCADA);
	write_intu16(bsw, MDC_PULS_RATE_NON_INV);
	attr_list2->value[0].attribute_value.value = bsw->buffer;

	attr_list2->value[1].attribute_id = MDC_ATTR_METRIC_SPEC_SMALL;
	attr_list2->value[1].attribute_value.length = 2;
	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, 0xF040); // 0xF0 0x40 intermittent, stored data, upd & msmt aperiodic, agent init, measured
	attr_list2->value[1].attribute_value.value = bsw->buffer;

	attr_list2->value[2].attribute_id = MDC_ATTR_UNIT_CODE;
	attr_list2->value[2].attribute_value.length = 2;
	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, MDC_DIM_BEAT_PER_MIN);
	attr_list2->value[2].attribute_value.value = bsw->buffer;

	attr_list2->value[3].attribute_id = MDC_ATTR_ATTRIBUTE_VAL_MAP;
	attr_list2->value[3].attribute_value.length = 12;
	free(bsw);
	bsw = byte_stream_writer_instance(12);
	write_intu16(bsw, 0x0002); // AttrValMap.count = 2
	write_intu16(bsw, 0x0008); // AttrValMap.length = 8
	write_intu16(bsw, MDC_ATTR_NU_VAL_OBS_BASIC);
	write_intu16(bsw, 0x0002); // value length 2
	write_intu16(bsw, MDC_ATTR_TIME_STAMP_ABS);
	write_intu16(bsw, 0x0008); // value length 8
	attr_list2->value[3].attribute_value.value = bsw->buffer;

	std_object_list->value[0].attributes = *attr_list1;
	std_object_list->value[1].attributes = *attr_list2;

	free(attr_list1);
	free(attr_list2);
	free(bsw);

	return std_object_list;
}

 /**
  * Populates an event report APDU.
  */

static DATA_apdu *blood_pressure_populate_event_report(void *edata)
{
	DATA_apdu *data;
	EventReportArgumentSimple evt;
	ScanReportInfoFixed scan;
	ObservationScanFixedList scan_fixed;
	ObservationScanFixed measure[2];
	AbsoluteTime nu_time;
	BasicNuObsValueCmp nu_pressure;
	BasicNuObsValue nu_pulse_rate;
	struct blood_pressure_event_report_data *evtdata;

	data = calloc(sizeof(DATA_apdu), 1);
	evtdata = (struct blood_pressure_event_report_data*) edata;

	nu_time = date_util_create_absolute_time(evtdata->century * 100 + evtdata->year,
						evtdata->month,
						evtdata->day,
						evtdata->hour,
						evtdata->minute,
						evtdata->second,
						evtdata->sec_fractions);

	nu_pressure.count = 0x03;
	nu_pressure.length = 0x06;
	nu_pressure.value = calloc(sizeof(BasicNuObsValue), 3);
	nu_pressure.value[0] = evtdata->systolic;
	nu_pressure.value[1] = evtdata->diastolic;
	nu_pressure.value[2] = evtdata->mean;
	nu_pulse_rate = evtdata->pulse_rate;

	// will be filled afterwards by service_* function
	data->invoke_id = 0xffff;

	data->message.choice = ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN;
	data->message.length = 54;

	evt.obj_handle = 0;
	evt.event_time = 0xFFFFFFFF;
	evt.event_type = MDC_NOTI_SCAN_REPORT_FIXED;
	evt.event_info.length = 44;

	scan.data_req_id = 0xF000;
	scan.scan_report_no = 0;

	scan_fixed.count = 2;
	scan_fixed.length = 36;
	scan_fixed.value = measure;

	measure[0].obj_handle = 1;
	measure[0].obs_val_data.length = 18;
	ByteStreamWriter *writer0 = byte_stream_writer_instance(
			measure[0].obs_val_data.length);

	encode_basicnuobsvaluecmp(writer0, &nu_pressure);
	encode_absolutetime(writer0, &nu_time);

	measure[1].obj_handle = 2;
	measure[1].obs_val_data.length = 10;
	ByteStreamWriter *writer1 = byte_stream_writer_instance(
			measure[1].obs_val_data.length);

	encode_basicnuobsvalue(writer1, &nu_pulse_rate);
	encode_absolutetime(writer1, &nu_time);

	measure[0].obs_val_data.value = writer0->buffer;
	measure[1].obs_val_data.value = writer1->buffer;

	scan.obs_scan_fixed = scan_fixed;

	ByteStreamWriter *scan_writer = byte_stream_writer_instance(
			evt.event_info.length);

	encode_scanreportinfofixed(scan_writer, &scan);

	del_byte_stream_writer(writer0, 1);
	del_byte_stream_writer(writer1, 1);
	del_basicnuobsvaluecmp(&nu_pressure);

	evt.event_info.value = scan_writer->buffer;
	data->message.u.roiv_cmipEventReport = evt;

	del_byte_stream_writer(scan_writer, 0);

	return data;
}

/**
 *  Creates the standard configuration for <em>Blood Pressure Monitor</em> specialization (02BC).
 *  For more information about <em>Blood Pressure Monitor</em> specialization, see IEEE 11073-10407
 *  Standard (Section 6.4, page 7).
 *
 *  \return an StdConfiguration struct that represents the standard configuration (a StdConfiguration instance)
 *  for <em>Blood Pressure Monitor</em> specialization just created.
 */
struct StdConfiguration *blood_pressure_monitor_create_std_config_ID02BC()
{
	struct StdConfiguration *result = calloc(1,
					  sizeof(struct StdConfiguration));
	result->dev_config_id = 0x02BC;
	result->configure_action = &blood_pressure_monitor_get_config_ID02BC;
	result->event_report = &blood_pressure_populate_event_report;
	return result;
}

/** @} */
