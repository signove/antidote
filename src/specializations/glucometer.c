/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file glucometer.c
 * \brief Glucometer Standard Specialization implementation
 *
 * Copyright (C) 2012 Signove Tecnologia Corporation.
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
 * \date Feb 29, 2012
 * \author Jose Luis do Nascimento
 */

#include <stdio.h>
#include "glucometer.h"
#include "src/util/bytelib.h"
#include "src/util/dateutil.h"
#include "communication/parser/encoder_ASN1.h"
#include "src/dim/nomenclature.h"
#include "src/dim/mds.h"

/**
 * \defgroup Glucometer Glucometer Device Specialization Implementation
 * \ingroup DeviceSpecializations
 * \brief IEEE specialization for Glucometer
 *
 * @{
 */

/**
 *  Creates the standard configuration for <em>Glucometer</em> specialization (06A4).
 *  For more information about <em>Glucometer</em> specialization, see IEEE 11073-10417
 *  Standard (Section 5.7, page 8).
 *
 *  \return an StdConfiguration struct that represents the standard configuration (a StdConfiguration instance)
 *  for <em>Glucometer</em> specialization.
 */
static ConfigObjectList *glucometer_get_config_ID06A4()
{
	ConfigObjectList *std_object_list = malloc(sizeof(ConfigObjectList));
	std_object_list->count = 1;
	std_object_list->length = 44;
	std_object_list->value = malloc(sizeof(ConfigObject));
	std_object_list->value[0].obj_class = MDC_MOC_VMO_METRIC_NU;

	std_object_list->value[0].obj_handle = 1;
	AttributeList *attr_list1 = malloc(sizeof(AttributeList));
	attr_list1->count = 4;
	attr_list1->length = 36;
	attr_list1->value = malloc(attr_list1->count * sizeof(AVA_Type));

	attr_list1->value[0].attribute_id = MDC_ATTR_ID_TYPE;
	attr_list1->value[0].attribute_value.length = 4;

	ByteStreamWriter *bsw = byte_stream_writer_instance(4);
	write_intu16(bsw, MDC_PART_SCADA);
	write_intu16(bsw, MDC_CONC_GLU_CAPILLARY_WHOLEBLOOD);
	attr_list1->value[0].attribute_value.value = bsw->buffer;

	attr_list1->value[1].attribute_id = MDC_ATTR_METRIC_SPEC_SMALL;
	attr_list1->value[1].attribute_value.length = 2;

	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, 0xF040); // 0xF0 0x40 intermittent, stored data, upd & msmt aperiodic, agent init, measured
	attr_list1->value[1].attribute_value.value = bsw->buffer;

	attr_list1->value[2].attribute_id = MDC_ATTR_UNIT_CODE;
	attr_list1->value[2].attribute_value.length = 2;

	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, MDC_DIM_MILLI_G_PER_DL);
	attr_list1->value[2].attribute_value.value = bsw->buffer;

	attr_list1->value[3].attribute_id = MDC_ATTR_ATTRIBUTE_VAL_MAP;
	attr_list1->value[3].attribute_value.length = 12;
	free(bsw);
	bsw = byte_stream_writer_instance(12);
	write_intu16(bsw, 2); // AttrValMap.count = 2
	write_intu16(bsw, 8); // AttrValMap.length = 8
	write_intu16(bsw, MDC_ATTR_NU_VAL_OBS_BASIC);
	write_intu16(bsw, 2); // value length = 2
	write_intu16(bsw, MDC_ATTR_TIME_STAMP_ABS);
	write_intu16(bsw, 8); // value length = 8
	attr_list1->value[3].attribute_value.value = bsw->buffer;

	free(bsw);

	std_object_list->value[0].attributes = *attr_list1;

	free(attr_list1);

	return std_object_list;
}

/**
 * Populates an event report APDU. 
 */

static DATA_apdu *glucometer_populate_event_report(void *edata)
{
	DATA_apdu *data;
	EventReportArgumentSimple evt;
	ScanReportInfoFixed scan;
	ObservationScanFixedList scan_fixed;
	ObservationScanFixed measure[1];
	AbsoluteTime nu_time;
	FLOAT_Type nu_capillary_whole_blood;
	struct glucometer_event_report_data *evtdata;

	data = calloc(sizeof(DATA_apdu), 1);
	evtdata = (struct glucometer_event_report_data*) edata;

	nu_time = date_util_create_absolute_time(evtdata->century * 100 + evtdata->year,
						evtdata->month,
						evtdata->day,
						evtdata->hour,
						evtdata->minute,
						evtdata->second,
						evtdata->sec_fractions);

	nu_capillary_whole_blood = evtdata->capillary_whole_blood;

	// will be filled afterwards by service_* function
	data->invoke_id = 0xffff;

	data->message.choice = ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN;
	data->message.length = 34;

	evt.obj_handle = 0;
	evt.event_time = 0xFFFFFFFF;
	evt.event_type = MDC_NOTI_SCAN_REPORT_FIXED;
	evt.event_info.length = 24;

	scan.data_req_id = 0xF000;
	scan.scan_report_no = 0;

	scan_fixed.count = 1;
	scan_fixed.length = 16;
	scan_fixed.value = measure;

	measure[0].obj_handle = 1;
	measure[0].obs_val_data.length = 10;
	ByteStreamWriter *writer0 = byte_stream_writer_instance(measure[0].obs_val_data.length);

	encode_basicnuobsvalue(writer0, &nu_capillary_whole_blood);
	encode_absolutetime(writer0, &nu_time);
	
	measure[0].obs_val_data.value = writer0->buffer;


	scan.obs_scan_fixed = scan_fixed;

	ByteStreamWriter *scan_writer = byte_stream_writer_instance(evt.event_info.length);

	encode_scanreportinfofixed(scan_writer, &scan);

	del_byte_stream_writer(writer0, 1);


	evt.event_info.value = scan_writer->buffer;
	data->message.u.roiv_cmipEventReport = evt;

	del_byte_stream_writer(scan_writer, 0);

	return data;
}


/**
 *  Creates the first standard configuration for <em>Glucometer</em> specialization (0x06A4).
 *  For more information about <em>Glucometer</em> specialization, see IEEE 11073-10417
 *  Standard.
 *
 *  \return an StdConfiguration struct that represents the first standard configuration (a StdConfiguration instance)
 *  for <em>Glucometer</em> specialization just created.
 */
struct StdConfiguration *glucometer_create_std_config_ID06A4()
{
	struct StdConfiguration *result = malloc(sizeof(struct StdConfiguration));
	result->dev_config_id = 0x06A4;
	result->configure_action = &glucometer_get_config_ID06A4;
	result->event_report = &glucometer_populate_event_report;
	return result;
}

/** @} */
