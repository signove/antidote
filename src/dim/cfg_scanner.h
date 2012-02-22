/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file cfg_scanner.h
 * \brief Cfg Scanner definitions.
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

#ifndef CFGSCANNER_H_
#define CFGSCANNER_H_

#include <dim/scanner.h>

/**
 * \brief The CfgScanner is a specialization of structure Scanner and define
 * the attributes that are common for compose CfgScanners more specialized.
 *
 * The CfgScanner defines the communication behavior of a configurable Scanner
 * structure.
 */
struct CfgScanner {

	/**
	 * The DIM structure.
	 */
	struct DIM dim;

	/**
	 * The Scanner structure.
	 */
	struct Scanner scanner;

	/**
	 * This attribute defines whether event reports are sent
	 * confirmed or unconfirmed.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	ConfirmMode confirm_mode;


	/**
	 * This informational timeout attribute defines the minimum
	 * time that the agent shall wait for a Response message from
	 * the manager after issuing a Confirmed Event Report
	 * invoke message before timing out and transitioning to the
	 * Unassociated state.
	 *
	 * Qualifier: Conditional
	 *
	 */
	RelativeTime confirm_timeout;

	/**
	 * This attribute defines informative data provided by
	 * the agent that may help a manager optimize its
	 * configuration.
	 *
	 * Qualifier: Optional
	 *
	 */
	intu16 transmit_window;
};

struct CfgScanner *cfg_scanner_instance(struct Scanner *scanner,
					ConfirmMode confirm_mode);

void cfg_scanner_destroy(struct CfgScanner *self);

int cfg_scanner_get_nomenclature_code();

Request *cfg_scanner_set_operational_state(Context *ctx, struct CfgScanner *self,
		OperationalState new_operational_state, service_request_callback callback);

void cfg_scanner_set_operational_state_response(struct CfgScanner *self,
		OperationalState new_operational_state);

void cfg_scanner_event_report_scan_report_grouped(Context *ctx, struct CfgScanner *self,
		ScanReportInfoGrouped *report_info);

void cfg_scanner_event_report_scan_report_mp_grouped(Context *ctx, struct CfgScanner *self,
		ScanReportInfoMPGrouped *report_info);

#endif /* CFGSCANNER_H_ */
