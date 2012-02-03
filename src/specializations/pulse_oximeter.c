/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file pulse_oximeter.c
 * \brief Pulse Oximeter Monitor Specialization implementation
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
 * \date Jul 08, 2010
 * \author Diego Bezerra
 */

#include <stdio.h>
#include "pulse_oximeter.h"
#include "src/util/bytelib.h"
#include "src/util/dateutil.h"
#include "communication/parser/encoder_ASN1.h"
#include "src/dim/nomenclature.h"
#include "src/dim/mds.h"

/**
 * \defgroup PulseOximeter Pulse Oximeter
 * \ingroup DeviceSpecializations
 * \brief Pulse Oximeter Monitor IEEE specialization
 *
 * A pulse oximeter, also called an oximeter, provides a noninvasive
 * estimate of functional oxygen of arterial haemoglobin (SpO2) from
 * a light signal interacting with tissue, by using the time-dependent
 * changes in tissue optical properties that occur with pulsatile blood
 * flow.
 *
 * Applying the Beer-Lambert law of light absorption through such
 * an arterial network, the fraction of oxygenation of arterial
 * haemoglobin can be estimated. This estimate, normally expressed as a
 * percentage by multiplying that fraction by 100, is known as SpO2.
 * Occasionally, this estimate may be referenced as %SpO2.
 *
 * Pulse oximeter systems with applicability in the personal health
 * space may take on a variety of configurations and sensor compositions,
 * and their configurations have suitability in different personal
 * health application spaces. Pulse oximeter equipment comprises a pulse
 * oximeter monitor, a pulse oximeter probe, and a probe cable extender,
 * if provided.
 *
 * Some oximeters are all-in-one assemblies, where the optical
 * probe, processing, and display components are in a single package.
 * Other oximeters may consist of separate sensor and processing/display
 * components. Still others may place the sensor and signal processing in one
 * component, and send that information into an external component for display
 * and storage. In addition, other configurations may add storage capability
 * into the system. This implies that different information models may be best
 * suited for each particular device configuration
 *
 * The pulse oximeter receives two different measures: 1) heart beat and 2) estimate
 * of functional oxygen of arterial haemoglobin (SpO2).
 *
 * The measures are read from a Numeric object and data are saved at
 * numeric->compound_basic_nu_observed_value->value field. The following example reads
 * estimate of functional oxygen of arterial haemoglobin and simply prints it.
 *
 * \code
 *
 * struct MDS_object* obj1 = mds_get_object_by_handle(HANDLE_MEASUREMENT_1);
 *
 * if (obj1 != NULL && obj1->choice == MDS_OBJ_METRIC) {
 *		if (obj1->u.metric.choice == METRIC_NUMERIC){
 *			struct Numeric* numeric = &obj1->u.metric.u.numeric;
 *			if (numeric->basic_nu_observed_value > 0){
 *				unsigned char *unit_label = mds_get_unit_code_string(numeric->metric.unit_code);
 *				sprintf(str, "%s SpO2 (%s): %.2f\n", str, unit_label,
 *							numeric->basic_nu_observed_value);
 *			}
 *		}
 *	}
 *
 * \endcode
 *
 *
 * @{
 */

/** Measurement 1: functional oxygen of arterial haemoglobin (SpO2) */
#define HANDLE_MEASUREMENT_1 1

/** Measurement 2: Heart Rate */
#define HANDLE_MEASUREMENT_2 10

/**
 *  Creates the standard configuration for <em>Pulse Oximeter</em> specialization (0190).
 *  For more information about <em>Pulse Oximeter Monitor</em> specialization, see IEEE 11073-10404
 *  Standard (Section 5.7, page 8).
 *
 *  \return an StdConfiguration struct that represents the standard configuration (a StdConfiguration instance)
 *  for <em>Pulse Oximeter Monitor</em> specialization.
 */
static ConfigObjectList *pulse_oximeter_get_config_ID0190()
{
	ConfigObjectList *std_object_list = malloc(sizeof(ConfigObjectList));
	std_object_list->count = 2;
	std_object_list->length = 80;
	std_object_list->value = malloc(sizeof(ConfigObject)*2);
	std_object_list->value[0].obj_class = MDC_MOC_VMO_METRIC_NU;

	std_object_list->value[0].obj_handle = 1;
	AttributeList *attr_list1 = malloc(sizeof(AttributeList));
	attr_list1->count = 4;
	attr_list1->length = 32;
	attr_list1->value = malloc(attr_list1->count * sizeof(AVA_Type));

	attr_list1->value[0].attribute_id = MDC_ATTR_ID_TYPE;
	attr_list1->value[0].attribute_value.length = 4;

	ByteStreamWriter *bsw = byte_stream_writer_instance(4);
	write_intu16(bsw, MDC_PART_SCADA);
	write_intu16(bsw, MDC_PULS_OXIM_SAT_O2);
	attr_list1->value[0].attribute_value.value = bsw->buffer;

	attr_list1->value[1].attribute_id = MDC_ATTR_METRIC_SPEC_SMALL;
	attr_list1->value[1].attribute_value.length = 2;

	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, 0x4040); // 0x40 0x40 avail-stored-data, acc-agent-init, measured
	attr_list1->value[1].attribute_value.value = bsw->buffer;

	attr_list1->value[2].attribute_id = MDC_ATTR_UNIT_CODE;
	attr_list1->value[2].attribute_value.length = 2;

	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, MDC_DIM_PERCENT);
	attr_list1->value[2].attribute_value.value = bsw->buffer;

	attr_list1->value[3].attribute_id = MDC_ATTR_ATTRIBUTE_VAL_MAP;
	attr_list1->value[3].attribute_value.length = 8;
	free(bsw);
	bsw = byte_stream_writer_instance(8);
	write_intu16(bsw, 1); // AttrValMap.count = 1
	write_intu16(bsw, 4); // AttrValMap.length = 4
	write_intu16(bsw, MDC_ATTR_NU_VAL_OBS_BASIC);
	write_intu16(bsw, 2); // value length = 2
	attr_list1->value[3].attribute_value.value = bsw->buffer;

	std_object_list->value[1].obj_class = MDC_MOC_VMO_METRIC_NU;
	// TODO verify obj_handle
	std_object_list->value[1].obj_handle = 10;
	AttributeList *attr_list2 = malloc(sizeof(AttributeList));
	attr_list2->count = 4;
	attr_list2->length = 32;
	attr_list2->value = malloc(attr_list2->count * sizeof(AVA_Type));

	attr_list2->value[0].attribute_id = MDC_ATTR_ID_TYPE;
	attr_list2->value[0].attribute_value.length = 4;
	free(bsw);
	bsw = byte_stream_writer_instance(4);
	write_intu16(bsw, MDC_PART_SCADA);
	write_intu16(bsw, MDC_PULS_OXIM_PULS_RATE);
	attr_list2->value[0].attribute_value.value = bsw->buffer;

	attr_list2->value[1].attribute_id = MDC_ATTR_METRIC_SPEC_SMALL;
	attr_list2->value[1].attribute_value.length = 2;
	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, 0x4040); // 0x40 0x40 avail-stored-data, acc-agent-init, measured
	attr_list2->value[1].attribute_value.value = bsw->buffer;

	attr_list2->value[2].attribute_id = MDC_ATTR_UNIT_CODE;
	attr_list2->value[2].attribute_value.length = 2;
	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, MDC_DIM_BEAT_PER_MIN);
	attr_list2->value[2].attribute_value.value = bsw->buffer;

	attr_list2->value[3].attribute_id = MDC_ATTR_ATTRIBUTE_VAL_MAP;
	attr_list2->value[3].attribute_value.length = 8;
	free(bsw);
	bsw = byte_stream_writer_instance(8);
	write_intu16(bsw, 1); // AttrValMap.count = 1
	write_intu16(bsw, 4); // AttrValMap.length = 4
	write_intu16(bsw, MDC_ATTR_NU_VAL_OBS_BASIC);
	write_intu16(bsw, 2); // value length = 2
	attr_list2->value[3].attribute_value.value = bsw->buffer;
	free(bsw);

	std_object_list->value[0].attributes = *attr_list1;
	std_object_list->value[1].attributes = *attr_list2;

	free(attr_list1);
	free(attr_list2);

	return std_object_list;
}

/**
 * Populates an event report APDU. 
 */

static DATA_apdu *pulse_oximeter_populate_event_report(void *edata)
{
	DATA_apdu *data;
	EventReportArgumentSimple evt;
	ScanReportInfoFixed scan;
	ObservationScanFixedList scan_fixed;
	ObservationScanFixed measure[2];
	AbsoluteTime nu_time;
	BasicNuObsValue nu_oximetry;
	BasicNuObsValue nu_beats;
	struct oximeter_event_report_data *evtdata;

	data = calloc(sizeof(DATA_apdu), 1);
	evtdata = (struct oximeter_event_report_data*) edata;

	nu_time = date_util_create_absolute_time(evtdata->century * 100 + evtdata->year,
						evtdata->month,
						evtdata->day,
						evtdata->hour,
						evtdata->minute,
						evtdata->second,
						evtdata->sec_fractions);

	nu_oximetry = evtdata->oximetry;
	nu_beats = evtdata->beats;

	// will be filled afterwards by service_* function
	data->invoke_id = 0xffff;

	// data->message.choice = ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN;
	data->message.choice = ROIV_CMIP_EVENT_REPORT_CHOSEN;
	data->message.length = 46;

	evt.obj_handle = 0;
	evt.event_time = 0xFFFFFFFF;
	evt.event_type = MDC_NOTI_SCAN_REPORT_FIXED;
	evt.event_info.length = 36;

	scan.data_req_id = 0xF000;
	scan.scan_report_no = 0;

	scan_fixed.count = 2;
	scan_fixed.length = 28;
	scan_fixed.value = measure;

	measure[0].obj_handle = 1;
	measure[0].obs_val_data.length = 10;
	ByteStreamWriter *writer0 = byte_stream_writer_instance(measure[0].obs_val_data.length);

	encode_basicnuobsvalue(writer0, &nu_oximetry);
	encode_absolutetime(writer0, &nu_time);

	measure[1].obj_handle = 10;
	measure[1].obs_val_data.length = 10;
	ByteStreamWriter *writer1 = byte_stream_writer_instance(measure[1].obs_val_data.length);

	encode_basicnuobsvalue(writer1, &nu_beats);
	encode_absolutetime(writer1, &nu_time);
	
	measure[0].obs_val_data.value = writer0->buffer;
	measure[1].obs_val_data.value = writer1->buffer;

	scan.obs_scan_fixed = scan_fixed;

	ByteStreamWriter *scan_writer = byte_stream_writer_instance(evt.event_info.length);

	encode_scanreportinfofixed(scan_writer, &scan);

	del_byte_stream_writer(writer0, 1);
	del_byte_stream_writer(writer1, 1);

	evt.event_info.value = scan_writer->buffer;
	data->message.u.roiv_cmipEventReport = evt;

	del_byte_stream_writer(scan_writer, 0);

	return data;
}


/**
 *  Creates the first standard configuration for <em>Pulse Oximeter Monitor</em> specialization (0190).
 *  For more information about <em>Pulse Oximeter Monitor</em> specialization, see IEEE 11073-10404
 *  Standard (Section 5.7, page 8).
 *
 *  \return an StdConfiguration struct that represents the first standard configuration (a StdConfiguration instance)
 *  for <em>Pulse Oximeter Monitor</em> specialization just created.
 */
struct StdConfiguration *pulse_oximeter_create_std_config_ID0190()
{
	struct StdConfiguration *result = malloc(sizeof(struct StdConfiguration));
	result->dev_config_id = 0x0190;
	result->configure_action = &pulse_oximeter_get_config_ID0190;
	result->event_report = &pulse_oximeter_populate_event_report;
	return result;
}

/**
 *  Creates the second standard configuration for <em>Pulse Oximeter Monitor</em> specialization (0190).
 *  For more information about <em>Pulse Oximeter Monitor</em> specialization, see IEEE 11073-10404
 *  Standard (Section 5.7, page 8).
 *
 *  \return an StdConfiguration struct that represents the second standard configuration (a StdConfiguration instance)
 *  for <em>Pulse Oximeter Monitor</em> specialization just created.
 */
struct StdConfiguration *pulse_oximeter_create_std_config_ID0191()
{
	struct StdConfiguration *result = malloc(sizeof(struct StdConfiguration));
	result->dev_config_id = 0x0191;
	result->configure_action = &pulse_oximeter_get_config_ID0190;
	result->event_report = &pulse_oximeter_populate_event_report;
	return result;
}

/** @} */
