/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file blood_pressure_monitor.h
 * \brief Blood Pressure Monitor Specialization
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
 * \author Jose Martins
 */

#ifndef BLOOD_PRESSURE_MONITOR_H_
#define BLOOD_PRESSURE_MONITOR_H_

#include <communication/stdconfigurations.h>

/* Blood pressure monitor */
#define MDC_PULS_RATE_NON_INV 18474 /* */
#define MDC_PRESS_BLD_NONINV 18948 /* NIBP */
#define MDC_PRESS_BLD_NONINV_SYS 18949 /* */
#define MDC_PRESS_BLD_NONINV_DIA 18950 /* */
#define MDC_PRESS_BLD_NONINV_MEAN 18951 /* */

struct StdConfiguration *blood_pressure_monitor_create_std_config_ID02BC();

/**
 * Event report data, used in Agent role
 */
struct blood_pressure_event_report_data {
	BasicNuObsValue systolic;
	BasicNuObsValue diastolic;
	BasicNuObsValue mean;
	BasicNuObsValue pulse_rate;
	int century;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int sec_fractions;
};
#endif /* BLOOD_PRESSURE_MONITOR_H_ */
