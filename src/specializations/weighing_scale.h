/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file weighing_scale.h
 * \brief Weighting Scale Specialization
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
 * \date Jun 14, 2010
 * \author Diego Bezerra
 */

#ifndef BODY_COMPOSITION_MONITOR_H_
#define BODY_COMPOSITION_MONITOR_H_

#include <communication/stdconfigurations.h>

/** /brief Body composition monitor
 */
#define MDC_LEN_BODY_ACTUAL 57668
#define MDC_RATIO_MASS_BODY_LEN_SQ 57680

struct StdConfiguration *weighting_scale_create_std_config_ID05DC();

/**
 * Event report data, used in Agent role
 */
struct weightscale_event_report_data {
	FLOAT_Type weight;
	FLOAT_Type bmi;
	int century;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int sec_fractions;
};

#endif /* BODY_COMPOSITION_MONITOR_H_ */
