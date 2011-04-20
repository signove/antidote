/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file peri_cfg_scanner.h
 * \brief PeriCfgScanner definition
 **
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

#ifndef PERICFGSCANNER_H_
#define PERICFGSCANNER_H_

#include "cfg_scanner.h"

/**
 * \brief The EpiCfgScanner is a specialization of structure CfgScanner
 * and is used to send reports containing Periodic data, that is, data
 * sampled during fixed periods. It buffers any data value changes to be
 * sent as part of a periodic report. Event reports shall be sent with a
 * time interval equal to the Reporting-Interval attribute value.
 */
struct PeriCfgScanner {

	/**
	 * The DIM structure reference.
	 */
	struct DIM dim;

	/**
	 * The CfgScanner structure reference.
	 */
	struct CfgScanner scanner;

	/**
	 * Reporting period of the event reports.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	RelativeTime reporting_interval;
};


struct PeriCfgScanner *peri_cfg_scanner_instance(struct CfgScanner *scanner);

void peri_cfg_scanner_destroy(struct PeriCfgScanner *self);

void peri_cfg_scanner_event_report_buf_scan_report_var(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoVar *report_info);

void peri_cfg_scanner_event_report_buf_scan_report_fixed(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoFixed *report_info);

void peri_cfg_scanner_event_report_buf_scan_report_grouped(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoGrouped *report_info);

void peri_cfg_scanner_event_report_buf_scan_report_mp_var(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoMPVar *report_info);

void peri_cfg_scanner_event_report_buf_scan_report_mp_fixed(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoMPFixed *report_info);

void peri_cfg_scanner_event_report_buf_scan_report_mp_grouped(Context *ctx,
		struct PeriCfgScanner *self,
		ScanReportInfoMPGrouped *report_info);


Request *peri_cfg_scanner_set_operational_state(Context *ctx, struct PeriCfgScanner *self,
		OperationalState new_operational_state, service_request_callback callback);

void peri_cfg_scanner_set_operational_state_response(
	struct PeriCfgScanner *self,
	OperationalState new_operational_state);


int peri_cfg_scanner_get_nomenclature_code();

#endif /* PERICFGSCANNER_H_ */
