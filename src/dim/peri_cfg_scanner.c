/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * \file peri_cfg_scanner.c
 * \brief PeriCfgScanner definition
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
 * \date Jun 09, 2010
 * \author Jose Martins
 */

#include <stdlib.h>
#include "peri_cfg_scanner.h"
#include "mds.h"
#include "dimutil.h"
#include "src/manager_p.h"
#include "src/api/api_definitions.h"
#include "src/api/data_encoder.h"

/**
 * \defgroup PeriCfgScanner PeriCfgScanner
 * \ingroup Scanner
 * \brief PeriCfgScanner Object Class
 *
 * The PeriCfgScanner class represents a class that can be instantiated.
 *
 * PeriCfgScanner objects are used to send reports containing Periodic
 * data, that is, data sampled during fixed periods. It buffers any
 * data value changes to be sent as part of a periodic report.
 * Event reports shall be sent with a time interval equal to the
 * Reporting-Interval attribute value.
 *
 * The number of observations for each metric object is dependent
 * on the metric object’s update interval and the scanner’s Reporting-Interval.
 *
 * Example: A periodic configurable scanner is set up to “scan” two
 * metric objects with a Reporting-Interval of 1 s. The two objects update
 * their corresponding observed value periodically with an interval of 1 s and ½ s,
 * respectively. The periodic configurable scanner then issues event reports
 * every second containing one observation scan of metric object #1 and
 * two observation scans of metric object #2.
 *
 * @{
 */

/**
 * Returns a new instance of an PeriCfgScanner object.
 *
 * \return a pointer to an PeriCfgScanner object.
 */
static struct PeriCfgScanner *peri_new_instance()
{
	struct PeriCfgScanner *result = calloc(1, sizeof(struct PeriCfgScanner));
	result->dim.id = peri_cfg_scanner_get_nomenclature_code();
	return result;
}

/**
 * Deallocates a pointer to a PeriCfgScanner struct.
 *
 * \param self the PeriCfgScanner pointer to be deallocated.
 */
void peri_cfg_scanner_destroy(struct PeriCfgScanner *self)
{
	if (self != NULL) {
		cfg_scanner_destroy(&self->scanner);
	}
}

/**
 * This function return one instance of the PeriCfgScanner structure.
 * The attributes must be defined through direct assignments, including
 * conditional and optional attributes.
 *
 * \param scanner a scanner.
 *
 * \return instance of the PeriCfgScanner structure.
 */
struct PeriCfgScanner *peri_cfg_scanner_instance(struct CfgScanner *scanner)
{
	struct PeriCfgScanner *result = peri_new_instance();
	result->reporting_interval = 0;
	result->scanner = *scanner;
	return result;
}

/**
 * Reports summary data about any objects and attributes that the scanner
 * monitors. The event is triggered whenever data values change and the
 * variable message format (type/length/value) is used when reporting
 * data that changed.
 *
 * \param ctx
 * \param self an instance of the PeriCfgScanner structure.
 * \param report_info the data container
 */
void peri_cfg_scanner_event_report_buf_scan_report_var(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoVar *report_info)
{
	int info_size = report_info->obs_scan_var.count;

	int i;

	for (i = 0; i < info_size; ++i) {
		DataList *data_list = data_list_new(1);

		if (data_list != NULL) {
			dimutil_update_mds_from_obs_scan(ctx->mds, &report_info->obs_scan_var.value[i],
							 &data_list->values[0]);
			manager_notify_evt_measurement_data_updated(ctx, data_list);
		} else {
			// TODO Error Condition
		}
	}
}

/**
 * This event style is used whenever data values change and the
 * fixed message format of each object is used to report data that changed.
 *
 * \param ctx
 * \param self an instance of the PeriCfgScanner structure.
 * \param report_info the data container
 */
void peri_cfg_scanner_event_report_buf_scan_report_fixed(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoFixed *report_info)
{
	int info_size = report_info->obs_scan_fixed.count;

	int i;

	for (i = 0; i < info_size; ++i) {
		DataList *data_list = data_list_new(1);

		if (data_list != NULL) {
			dimutil_update_mds_from_obs_scan_fixed(ctx->mds, &report_info->obs_scan_fixed.value[i], &data_list->values[0]);
			manager_notify_evt_measurement_data_updated(ctx, data_list);
		} else {
			// TODO Error Condition
		}
	}
}

/**
 * This style is used when the scanner object is used to send
 * the data in its most compact format. The Handle-Attr-Val-Map attribute
 * describes the objects and attributes that are included and the format of
 * the message.
 *
 * \param ctx
 * \param self an instance of the PeriCfgScanner structure.
 * \param report_info the data container
 */
void peri_cfg_scanner_event_report_buf_scan_report_grouped(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoGrouped *report_info)
{
	HandleAttrValMap *attr_map = &self->scanner.scanner.scan_handle_attr_val_map;

	int i;

	for (i = 0; i < report_info->obs_scan_grouped.count; i++) {
		ObservationScanGrouped *data = &report_info->obs_scan_grouped.value[i];
		ByteStreamReader *stream = byte_stream_reader_instance(data->value, data->length);

		int j;

		for (j = 0; j < attr_map->count; j++) {
			DataList *data_list = data_list_new(1);

			if (data_list != NULL) {
				dimutil_update_mds_from_grouped_observations(ctx->mds, stream, &attr_map->value[j], &data_list->values[0]);
				manager_notify_evt_measurement_data_updated(ctx, data_list);
			} else {
				// TODO Error Condition
			}
		}

		free(stream);
	}
}

/**
 * Same as peri_cfg_scanner_event_report_buf_scan_report_var,
 * but allows inclusion of data from multiple persons.
 *
 * \param ctx
 * \param self an instance of the PeriCfgScanner structure.
 * \param report_info the data container
 */
void peri_cfg_scanner_event_report_buf_scan_report_mp_var(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoMPVar *report_info)
{
	int info_mp_list_size = report_info->scan_per_var.count;
	int i;

	for (i = 0; i < info_mp_list_size; ++i) {
		int info_size = report_info->scan_per_var.value[i].obs_scan_var.count;

		int j;

		for (j = 0; j < info_size; ++j) {
			DataList *data_list = data_list_new(1);

			if (data_list != NULL) {
				data_meta_set_personal_id(&data_list->values[0],
							  report_info->scan_per_var.value[i].person_id);

				dimutil_update_mds_from_obs_scan(ctx->mds,
								 &report_info->scan_per_var.value[i].obs_scan_var.value[j],
								 &data_list->values[0]);
				manager_notify_evt_measurement_data_updated(ctx, data_list);
			} else {
				// TODO Error Condition
			}
		}
	}
}

/**
 * Same as peri_cfg_scanner_event_report_buf_scan_report_fixed,
 * but allows inclusion of data from multiple persons.
 *
 * \param ctx
 * \param self an instance of the PeriCfgScanner structure.
 * \param report_info the data container
 */
void peri_cfg_scanner_event_report_buf_scan_report_mp_fixed(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoMPFixed *report_info)
{

	int info_fixed_list_size = report_info->scan_per_fixed.count;
	int i;

	for (i = 0; i < info_fixed_list_size; ++i) {
		int info_size = report_info->scan_per_fixed.value[i].obs_scan_fix.count;

		int j;

		for (j = 0; j < info_size; ++j) {
			DataList *data_list = data_list_new(1);

			if (data_list != NULL) {
				data_meta_set_personal_id(&data_list->values[0],
							  report_info->scan_per_fixed.value[i].person_id);

				dimutil_update_mds_from_obs_scan_fixed(ctx->mds,
								       &report_info->scan_per_fixed.value[i].obs_scan_fix.value[j],
								       &data_list->values[0]);
				manager_notify_evt_measurement_data_updated(ctx, data_list);
			} else {
				// TODO Error Condition
			}
		}
	}
}

/**
 * Same as peri_cfg_scanner_event_report_buf_scan_report_grouped,
 * but allows inclusion of data from multiple persons.
 *
 * \param ctx
 * \param self an instance of the PeriCfgScanner structure.
 * \param report_info the data container
 */
void peri_cfg_scanner_event_report_buf_scan_report_mp_grouped(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoMPGrouped *report_info)
{
	HandleAttrValMap *attr_map = &self->scanner.scanner.scan_handle_attr_val_map;
	int info_grouped_list_size = report_info->scan_per_grouped.count;
	int i;

	for (i = 0; i < info_grouped_list_size; ++i) {
		ObservationScanGrouped *data = &report_info->scan_per_grouped.value[i].obs_scan_grouped;
		ByteStreamReader *stream = byte_stream_reader_instance(data->value, data->length);

		int j;

		for (j = 0; j < attr_map->count; j++) {
			DataList *data_list = data_list_new(attr_map->count);

			if (data_list != NULL) {
				data_meta_set_personal_id(&data_list->values[0],
							  report_info->scan_per_grouped.value[i].person_id);

				dimutil_update_mds_from_grouped_observations(ctx->mds, stream, &attr_map->value[j], &data_list->values[0]);
				manager_notify_evt_measurement_data_updated(ctx, data_list);
			} else {
				// TODO Error Condition
			}
		}

		free(stream);
	}
}

/**
 * Agents that have scanner derived objects shall support the
 * SET service for the Operational-State attribute of the scanner objects.
 *
 * \param ctx the current context.
 * \param self the scanner to have operation state set.
 * \param new_operational_state the new operational state.
 * \param callback callback function of the request
 *
 * \return a pointer to a request just done.
 */
Request *peri_cfg_scanner_set_operational_state(Context *ctx, struct PeriCfgScanner *self,
		OperationalState new_operational_state, service_request_callback callback)
{
	return cfg_scanner_set_operational_state(ctx, &self->scanner, new_operational_state, callback);
}

/**
 * Agents that have scanner derived objects shall support the
 * SET service for the Operational-State attribute of the scanner objects.
 *
 * In this case, the attribute should any be modified after receiving the agent
 * response.This is a instance of the PeriCfgScanner structure.
 *
 * \param self the scanner to have operation state set.
 * \param new_operational_state the new operational state.
 */
void peri_cfg_scanner_set_operational_state_response(
	struct PeriCfgScanner *self,
	OperationalState new_operational_state)
{
	cfg_scanner_set_operational_state_response(&self->scanner,
			new_operational_state);
}

/**
 * Returns the code used to identify the PeriCfgScanner structure.
 *
 * \return the code that identify the PeriCfgScanner structure.
 */
int peri_cfg_scanner_get_nomenclature_code()
{
	return MDC_MOC_SCAN_CFG_PERI;
}

/** @} */
