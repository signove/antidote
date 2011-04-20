/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file epi_cfg_scanner.h
 * \brief Episodic Cfg Scanner definition.
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

#ifndef EPICFGSCANNER_H_
#define EPICFGSCANNER_H_

#include "cfg_scanner.h"

/**
 * \brief The EpiCfgScanner is a specialization of structure CfgScanner
 * and is used to send reports containing episodic data, that is, data
 * not having a fixed period between each data value. A report is sent
 * whenever one of the observed attributes changes value; however, two
 * consecutive event reports shall not have a time interval less than
 * the value of the Min-Reporting-Interval attribute.
 */
struct EpiCfgScanner {

	/**
	 * The DIM structure reference.
	 */
	struct DIM dim;

	/**
	 * The CfgScanner structure reference.
	 *
	 */
	struct CfgScanner scanner;

	/**
	 * This attribute provides an estimate of the expected
	 * minimum time between any two successive event reports
	 *
	 * Qualifier: Optional
	 *
	 */
	RelativeTime min_reporting_interval;
};

struct EpiCfgScanner *epi_cfg_scanner_instance(struct CfgScanner *scanner);

void epi_cfg_scanner_destroy(struct EpiCfgScanner *self);

void epi_cfg_scanner_event_report_unbuf_scan_report_var(Context *ctx,
		struct EpiCfgScanner *self, ScanReportInfoVar *report_info);

void epi_cfg_scanner_event_report_unbuf_scan_report_fixed(Context *ctx,
		struct EpiCfgScanner *self, ScanReportInfoFixed *report_info);

void epi_cfg_scanner_event_report_unbuf_scan_report_grouped(Context *ctx,
		struct EpiCfgScanner *self, ScanReportInfoGrouped *report_info);

void epi_cfg_scanner_event_report_unbuf_scan_report_mp_var(Context *ctx,
		struct EpiCfgScanner *self, ScanReportInfoMPVar *report_info);

void epi_cfg_scanner_event_report_unbuf_scan_report_mp_fixed(Context *ctx,
		struct EpiCfgScanner *self, ScanReportInfoMPFixed *report_info);

void epi_cfg_scanner_event_report_unbuf_scan_report_mp_grouped(Context *ctx,
		struct EpiCfgScanner *self, ScanReportInfoMPGrouped *report_info);


Request *epi_cfg_scanner_set_operational_state(Context *ctx, struct EpiCfgScanner *self,
		OperationalState new_operational_state, service_request_callback callback);

void epi_cfg_scanner_set_operational_state_response(struct EpiCfgScanner *self,
		OperationalState new_operational_state);


int epi_cfg_scanner_get_nomenclature_code();

#endif /* EPICFGSCANNER_H_ */
