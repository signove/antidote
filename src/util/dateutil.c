/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file dateutil.c
 * \brief Date utilities implementation.
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

#include "dateutil.h"
#include <stdio.h>

/**
 * \addtogroup Utility
 * @{
 */

/**
 *  Converts from BCD to numeric representation.
 *
 *  \param field representation
 *
 *  \return the numeric representation
 */
int date_util_convert_bcd_to_number(intu8 field)
{
	return 10 * (field >> 4) + (field & 0x000F);
}

/**
 *  Converts a given value into BCD (binary-coded decimal) representation.
 *
 *  \param number the value to convert to BCD representation
 *
 *  \return the given value converted to BCD representation
 */
intu8 date_util_convert_number_to_bcd(int number)
{
	int aux_result = 1;
	int aux = number;

	while (aux >= 10) {
		aux_result *= 10;
		aux = aux / 10;
	}

	aux = number;
	intu8 result = 0;

	while (aux >= 10) {
		result += aux / aux_result;
		aux = aux % aux_result;
		aux_result = aux_result / 10;
		result = result << 4;
	}

	result += aux / aux_result;

	return result;
}

/**
 *  Creates an AbsoluteTime struct based on given date fields.
 *
 *  \param year the year
 *  \param month the month
 *  \param day the day
 *  \param hour the hour
 *  \param minute the minute
 *  \param second the second
 *  \param sec_fractions the second fractions
 *
 *  \return an AbsoluteTime struct created from given date fields.
 */
AbsoluteTime date_util_create_absolute_time(int year, int month, int day,
		int hour, int minute, int second, int sec_fractions)
{
	AbsoluteTime result;
	result.century  = date_util_convert_number_to_bcd(year / 100);
	result.year = date_util_convert_number_to_bcd(year % 100);
	result.month = date_util_convert_number_to_bcd(month);
	result.day = date_util_convert_number_to_bcd(day);
	result.hour = date_util_convert_number_to_bcd(hour);
	result.minute = date_util_convert_number_to_bcd(minute);
	result.second = date_util_convert_number_to_bcd(second);
	result.sec_fractions = date_util_convert_number_to_bcd(sec_fractions);
	return result;
}

/**
 *  Tests the AbsoluteTime structs for equality. Returns a negative number if time_1 is less than time_2,
 *  returns zero if the two AbsoluteTime structs are equal, and returns a positive number is time_1 is
 *  greater than time_2.
 *
 *  \param time_1 the first AbsoluteTime struct to be compared.
 *  \param time_2 the second AbsoluteTime struct to be compared.
 *
 *  \return Returns a negative number if time_1 is less than time_2, returns zero if the
 *  two AbsoluteTime structs are equal, and returns a positive number is time_1 is greater than time_2.
 */
int date_util_compare_absolute_time(AbsoluteTime time_1, AbsoluteTime time_2)
{
	int century_1 = date_util_convert_bcd_to_number(time_1.century);
	int century_2 = date_util_convert_bcd_to_number(time_2.century);

	int year_1 = date_util_convert_bcd_to_number(time_1.year);
	int year_2 = date_util_convert_bcd_to_number(time_2.year);

	int abs_year_1 = century_1 * 100 + year_1;
	int abs_year_2 = century_2 * 100 + year_2;

	int month_1 = date_util_convert_bcd_to_number(time_1.month);
	int month_2 = date_util_convert_bcd_to_number(time_2.month);

	int day_1 = date_util_convert_bcd_to_number(time_1.day);
	int day_2 = date_util_convert_bcd_to_number(time_2.day);

	int hour_1 = date_util_convert_bcd_to_number(time_1.hour);
	int hour_2 = date_util_convert_bcd_to_number(time_2.hour);

	int minute_1 = date_util_convert_bcd_to_number(time_1.minute);
	int minute_2 = date_util_convert_bcd_to_number(time_2.minute);

	int second_1 = date_util_convert_bcd_to_number(time_1.second);
	int second_2 = date_util_convert_bcd_to_number(time_2.second);

	int second_fract_1 = date_util_convert_bcd_to_number(time_1.sec_fractions);
	int second_fract_2 = date_util_convert_bcd_to_number(time_2.sec_fractions);

	if (abs_year_1 < abs_year_2)
		return -1;
	else if (abs_year_1 > abs_year_2)
		return 1;

	if (month_1 < month_2)
		return -1;
	else if (month_1 > month_2)
		return 1;

	if (day_1 < day_2)
		return -1;
	else if (day_1 > day_2)
		return 1;

	if (hour_1 < hour_2)
		return -1;
	else if (hour_1 > hour_2)
		return 1;

	if (minute_1 < minute_2)
		return -1;
	else if (minute_1 > minute_2)
		return 1;

	if (second_1 < second_2)
		return -1;
	else if (second_1 > second_2)
		return 1;

	if (second_fract_1 < second_fract_2)
		return -1;
	else if (second_fract_1 > second_fract_2)
		return 1;

	return 0;
}

/*! @} */
