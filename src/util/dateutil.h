/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file dateutil.h
 * \brief Date utilities definitions.
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
 * \date Jun 15, 2010
 * \author Jose Martins
 */

#ifndef DATAUTIL_H_
#define DATAUTIL_H_

#include "src/asn1/phd_types.h"

int date_util_compare_absolute_time(AbsoluteTime time1, AbsoluteTime time2);

AbsoluteTime date_util_create_absolute_time(int year, int month, int day,
		int hour, int minute, int second, int sec_fractions);

intu8 date_util_convert_number_to_bcd(int value);
int date_util_convert_bcd_to_number(intu8 field);

#endif /* DATAUTIL_H_ */
