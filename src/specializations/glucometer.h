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

#ifndef GLUCOMETER_H_
#define GLUCOMETER_H_

#include <communication/stdconfigurations.h>

/* Glucometer nomenclature */

#define MDC_CONC_GLU_CAPILLARY_WHOLEBLOOD 29112
#define MDC_DIM_MILLI_G_PER_DL 2130 /* mg dL-1 */

struct StdConfiguration *glucometer_create_std_config_ID06A4();

/**
 * Event report data, used in Agent role
 */
struct glucometer_event_report_data {
	FLOAT_Type capillary_whole_blood;
	int century;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int sec_fractions;
};

#endif /* GLUCOMETER_H_ */
