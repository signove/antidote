/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file epi_cfg_scanner.c
 * \brief Episodic Cfg Scanner implementation.
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
#include "epi_cfg_scanner.h"
#include "mds.h"

/**
 * \defgroup EpiCfgScanner EpiCfgScanner
 * \ingroup Scanner
 * \brief EpiCfgScanner Object Class
 *
 * The EpiCfgScanner class represents a class that can be instantiated.
 *
 * EpiCfgScanner objects are used to send reports containing episodic
 * data, that is, data not having a fixed period between each data value.
 * A report is sent whenever one of the observed attributes changes value;
 * however, two consecutive event reports shall not have a time interval
 * less than the value of the Min-Reporting-Interval attribute.
 *
 * @{
 */

/**
 * Returns one instance of the EpiCfgScanner structure.
 * The attributes must be defined through direct assignments, including
 * conditional and optional attributes.
 *
 * \param scanner the scanner of EpiCfgScanner instance.
 *
 * \return instance of the EpiCfgScanner structure.
 */
struct EpiCfgScanner *epi_cfg_scanner_instance(struct CfgScanner *scanner)
{
	struct EpiCfgScanner *result = calloc(1, sizeof(struct EpiCfgScanner));
	result->dim.id = epi_cfg_scanner_get_nomenclature_code();
	result->scanner = *scanner;
	result->min_reporting_interval = 0;
	return result;
}

/**
 * Deallocates a pointer to a EpiCfgScanner struct.
 *
 * \param self the EpiCfgScanner pointer to be deallocated.
 */
void epi_cfg_scanner_destroy(struct EpiCfgScanner *self)
{
	if (self != NULL) {
		cfg_scanner_destroy(&self->scanner);
	}
}

/**
 * Reports summary data about any objects and attributes that the scanner
 * monitors. The event is triggered whenever data values change and the
 * variable message format (type/length/value) is used when reporting
 * data that changed.
 *
 * \param ctx
 * \param self an instance of the EpiCfgScanner structure.
 * \param report_info the data container
 */
void epi_cfg_scanner_event_report_unbuf_scan_report_var(Context *ctx, struct EpiCfgScanner *self,
		ScanReportInfoVar *report_info)
{
	mds_event_report_dynamic_data_update_var(ctx, report_info);
}

/**
 * This event style is used whenever data values change and the
 * fixed message format of each object is used to report data that changed.
 *
 * \param ctx
 * \param self an instance of the EpiCfgScanner structure.
 * \param report_info the data container
 */
void epi_cfg_scanner_event_report_unbuf_scan_report_fixed(Context *ctx, struct EpiCfgScanner *self,
		ScanReportInfoFixed *report_info)
{
	mds_event_report_dynamic_data_update_fixed(ctx, report_info);
}

/**
 * This style is used when the scanner object is used to send
 * the data in its most compact format. The Handle-Attr-Val-Map attribute
 * describes the objects and attributes that are included and the format of
 * the message.
 *
 * \param ctx
 * \param self an instance of the EpiCfgScanner structure.
 * \param report_info the data container
 */
void epi_cfg_scanner_event_report_unbuf_scan_report_grouped(Context *ctx, struct EpiCfgScanner *self,
		ScanReportInfoGrouped *report_info)
{
	cfg_scanner_event_report_scan_report_grouped(ctx, &self->scanner, report_info);
}

/**
 * Same as epi_cfg_scanner_event_report_unbuf_scan_report_var,
 * but allows inclusion of data from multiple persons.
 *
 * \param ctx current context.
 * \param self an instance of the EpiCfgScanner structure.
 * \param report_info the data container
 */
void epi_cfg_scanner_event_report_unbuf_scan_report_mp_var(Context *ctx, struct EpiCfgScanner *self,
		ScanReportInfoMPVar *report_info)
{
	mds_event_report_dynamic_data_update_mp_var(ctx, report_info);
}

/**
 * Same as epi_cfg_scanner_event_report_unbuf_scan_report_fixed,
 * but allows inclusion of data from multiple persons.
 *
 *
 * \param ctx
 * \param self an instance of the EpiCfgScanner structure.
 * \param report_info the data container
 */
void epi_cfg_scanner_event_report_unbuf_scan_report_mp_fixed(Context *ctx, struct EpiCfgScanner *self,
		ScanReportInfoMPFixed *report_info)
{
	mds_event_report_dynamic_data_update_mp_fixed(ctx, report_info);
}

/**
 * Same as epi_cfg_scanner_event_report_unbuf_scan_report_grouped,
 * but allows inclusion of data from multiple persons.
 *
 * \param ctx current context.
 * \param self an instance of the EpiCfgScanner structure.
 * \param report_info the data container
 */
void epi_cfg_scanner_event_report_unbuf_scan_report_mp_grouped(Context *ctx, struct EpiCfgScanner *self,
		ScanReportInfoMPGrouped *report_info)
{
	cfg_scanner_event_report_scan_report_mp_grouped(ctx, &self->scanner, report_info);
}

/**
 * Agents that have scanner derived objects shall support the
 * SET service for the Operational-State attribute of the scanner objects.
 *
 * \param ctx current context.
 * \param self the scanner to have operation state set.
 * \param new_operational_state the new operational state.
 * \param callback callback function of the request
 *
 * \return a pointer to a request just done.
 */
Request *epi_cfg_scanner_set_operational_state(Context *ctx, struct EpiCfgScanner *self,
		OperationalState new_operational_state, service_request_callback callback)
{
	return cfg_scanner_set_operational_state(ctx, &self->scanner, new_operational_state, callback);
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
void epi_cfg_scanner_set_operational_state_response(
	struct EpiCfgScanner *self,
	OperationalState new_operational_state)
{
	cfg_scanner_set_operational_state_response(&self->scanner,
			new_operational_state);
}

/**
 * Returns the code used to identify the EpiCfgScanner structure.
 *
 * \return the code that identify the EpiCfgScanner structure.
 */
int epi_cfg_scanner_get_nomenclature_code()
{
	return MDC_MOC_SCAN_CFG_EPI;
}

/** @} */

