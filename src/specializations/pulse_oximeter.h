/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file pulse_oximeter.h
 * \brief Pulse Oximeter Monitor Specialization
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

#ifndef PULSE_OXIMETER_H_
#define PULSE_OXIMETER_H_

#include <communication/stdconfigurations.h>

/* Pulse oximeter nomenclature */
#define MDC_PULS_OXIM_SAT_O2 19384
#define MDC_PULS_OXIM_PULS_RATE 18458
#define MDC_DIM_BEAT_PER_MIN 2720

struct StdConfiguration *pulse_oximeter_create_std_config_ID0190();

struct StdConfiguration *pulse_oximeter_create_std_config_ID0191();

/**
 * Event report data, used in Agent role
 */
struct oximeter_event_report_data {
	BasicNuObsValue beats;
	BasicNuObsValue oximetry;
	int century;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int sec_fractions;
};

#endif /* PULSE_OXIMETER_H_ */
