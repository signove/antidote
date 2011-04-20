/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file cfg_scanner.c
 * \brief Cfg Scanner implementation.
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
#include <stdio.h>
#include "cfg_scanner.h"
#include "dimutil.h"
#include "mds.h"
#include "src/api/api_definitions.h"
#include "src/api/data_encoder.h"
#include "src/api/text_encoder.h"
#include "src/util/bytelib.h"
#include "src/manager_p.h"

/**
 * \defgroup CfgScanner CfgScanner
 * \ingroup Scanner
 * \brief CfgScanner Object Class
 *
 * The CfgScanner class is an abstract class defining attributes,
 * methods, events, and services that are common for its subclasses.
 * In particular, it defines the communication behavior of a
 * configurable scanner object.
 *
 * @{
 */

/**
 * Returns a valid pointer to an CfgScanner struct.
 *
 * \return a valid pointer to an CfgScanner.
 */
static struct CfgScanner *cfg_scanner_new_instance()
{
	struct CfgScanner *result = calloc(1, sizeof(struct CfgScanner));
	result->dim.id = cfg_scanner_get_nomenclature_code();
	return result;
}

/**
 * Deallocates a pointer to a CfgScanner struct.
 *
 * \param self the CfgScanner pointer to be deallocated.
 */
void cfg_scanner_destroy(struct CfgScanner *self)
{
	if (self != NULL) {
		scanner_destroy(&self->scanner);
	}
}

/**
 * Return one instance of the CfgScanner structure.
 * The attributes must be defined through direct assignments, including
 * conditional and optional attributes.
 *
 * \param scanner the Scanner struct.
 * \param confirm_mode This attribute defines whether event reports are
 * sent confirmed or unconfirmed.
 *
 * \return instance of the CfgScanner structure.
 */
struct CfgScanner *cfg_scanner_instance(struct Scanner *scanner,
					ConfirmMode confirm_mode) {
	struct CfgScanner *result = cfg_scanner_new_instance();
	result->confirm_mode = confirm_mode;
	result->scanner = *scanner;
	result->confirm_timeout = 0;
	result->transmit_window = 0;
	return result;
}

/**
 * Returns the code used to identify the CfgScanner structure.
 *
 * \return the code that identify the CfgScanner structure.
 */
int cfg_scanner_get_nomenclature_code()
{
	return MDC_MOC_SCAN_CFG;
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
Request *cfg_scanner_set_operational_state(Context *ctx, struct CfgScanner *self,
		OperationalState new_operational_state, service_request_callback callback)
{
	return scanner_set_operational_state(ctx, &self->scanner, new_operational_state, callback);
}

/**
 * Agents that have scanner derived objects shall support the
 * SET service for the Operational-State attribute of the scanner objects.
 *
 * In this case, the attribute should any be modified after receiving the agent
 * response.
 *
 * \param self the scanner to have operation state set.
 * \param new_operational_state the new operational state.
 */
void cfg_scanner_set_operational_state_response(struct CfgScanner *self, OperationalState new_operational_state)
{
	scanner_set_operational_state_response(&self->scanner, new_operational_state);
}


/**
 * This style is used when the scanner object is used to send
 * the data in its most compact format. The Handle-Attr-Val-Map attribute
 * describes the objects and attributes that are included and the format of
 * the message.
 *
 * \param ctx the current context.
 * \param self an instance of the CfgScanner structure.
 * \param report_info the data container
 */
void cfg_scanner_event_report_scan_report_grouped(Context *ctx, struct CfgScanner *self,
		ScanReportInfoGrouped *report_info)
{
	HandleAttrValMap *attr_map = &self->scanner.scan_handle_attr_val_map;
	int i;

	for (i = 0; i < report_info->obs_scan_grouped.count; i++) {
		ObservationScanGrouped *data = &report_info->obs_scan_grouped.value[i];
		ByteStreamReader *stream = byte_stream_reader_instance(data->value, data->length);
		DataList *data_list = data_list_new(attr_map->count);

		if (data_list != NULL) {
			int j;

			for (j = 0; j < attr_map->count; j++) {
				dimutil_update_mds_from_grouped_observations(ctx->mds, stream, &attr_map->value[j], &data_list->values[j]);
			}
		}

		manager_notify_evt_measurement_data_updated(ctx, data_list);
		free(stream);
	}
}

/**
 * Same as cfg_scanner_event_report_scan_report_grouped,
 * but allows inclusion of data from multiple persons.
 *
 *
 * \param ctx
 * \param self an instance of the CfgScanner structure.
 * \param report_info the data container
 */
void cfg_scanner_event_report_scan_report_mp_grouped(Context *ctx, struct CfgScanner *self,
		ScanReportInfoMPGrouped *report_info)
{
	HandleAttrValMap *attr_map = &self->scanner.scan_handle_attr_val_map;
	int info_grouped_list_size = report_info->scan_per_grouped.count;
	int i;

	for (i = 0; i < info_grouped_list_size; ++i) {
		ObservationScanGrouped *data = &report_info->scan_per_grouped.value[i].obs_scan_grouped;
		ByteStreamReader *stream = byte_stream_reader_instance(data->value, data->length);
		DataList *data_list = data_list_new(attr_map->count);

		if (data_list != NULL) {
			int j;

			for (j = 0; j < attr_map->count; j++) {
				data_meta_set_personal_id(&data_list->values[j],
							  report_info->scan_per_grouped.value[i].person_id);

				dimutil_update_mds_from_grouped_observations(ctx->mds, stream, &attr_map->value[j], &data_list->values[j]);
			}

			manager_notify_evt_measurement_data_updated(ctx, data_list);
		} else {
			// TODO Error Condition
		}

		free(stream);
	}
}

/** @} */
