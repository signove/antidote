/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file text_encoder.c
 * \brief Implementation of text_encode.h header.
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
 * \author Fabricio Silva
 * \date Jul 2, 2010
 */

#include "text_encoder.h"
#include "api_definitions.h"
#include "src/asn1/phd_types.h"
#include "src/util/strbuff.h"
#include "src/util/dateutil.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

#define MAX_FLOAT_STR 100
#define MAX_INT_STR 20

/**
 *
 * \addtogroup TextEncoder Text Encoder
 * \ingroup API
 *
 * \brief Encodes types of IEEE layer into text representation
 *
 * @{
 */

/**
 * Converts an int to string representation.
 *
 * @param value the integer to be converted into string.
 * @return the string representation.
 */
char *int2str(int value)
{
	char *str = calloc(MAX_INT_STR, sizeof(char));
	sprintf(str, "%d", value);
	return str;
}

/**
 * Converts an int8 to string representation.
 *
 * @param value the integer to be converted into string.
 * @return the string representation.
 */
char *int8_2str(int8 value)
{
	return int2str(value);
}

/**
 * Converts an intu8 to string representation.
 *
 * @param value the integer to be converted into string.
 * @return the string representation.
 */
char *intu8_2str(intu8 value)
{
	char *str = calloc(MAX_INT_STR, sizeof(char));
	sprintf(str, "%u", value);
	return str;
}


/**
 * Converts an int16 to string representation.
 *
 * @param value the integer to be converted into string.
 * @return the string representation.
 */
char *int16_2str(int16 value)
{
	return int2str(value);
}

/**
 * Converts an intu16 to string representation.
 *
 * @param value the integer to be converted into string.
 * @return the string representation.
 */
char *intu16_2str(intu16 value)
{
	char *str = calloc(MAX_INT_STR, sizeof(char));
	sprintf(str, "%u", value);
	return str;
}

/**
 * Converts an int32 to string representation.
 *
 * @param value the integer to be converted into string.
 * @return the string representation.
 */
char *int32_2str(int32 value)
{
	return int2str(value);
}

/**
 * Converts an intu32 to string representation.
 *
 * @param value the integer to be converted into string.
 * @return the string representation.
 */
char *intu32_2str(intu32 value)
{
	char *str = calloc(MAX_INT_STR, sizeof(char));
	sprintf(str, "%u", value);
	return str;
}

/**
 * Converts an intu16 list to string representation.
 *
 * @param list the intu16 list to be converted into string.
 * @param size the number of elements to be converted to string representation.
 * @return the string representation.
 */
char *intu16list_2str(intu16 *list, int size)
{
	char *result = calloc(MAX_INT_STR * (size+1), sizeof(char));

	if (size > 0) {
		int i = 0;

		for (i = 0; i < size-1; i++) {
			sprintf(result, "%s%d,", result, list[i]);
		}

		sprintf(result, "%s%d", result, list[size-1]);
	}

	return result;
}

/**
 * Converts a float to string representation.
 *
 * @param value the float to be converted into string.
 * @return the string representation.
 */
char *float2str(float value)
{
	char *str = calloc(MAX_FLOAT_STR, sizeof(char));
	sprintf(str, "%f", value);
	return str;
}

/**
 * Converts an octet_string to string representation.
 *
 * @param str the octet_string to be converted into string.
 * @return the string representation.
 */
char *octet_string2str(octet_string *str)
{
	char *result = calloc(str->length + 1, sizeof(char));

	int i = 0;

	for (i = 0; i < str->length; i++) {
		sprintf(result, "%s%c", result, str->value[i]);
	}

	result[str->length] = '\0';

	return result;
}

/**
 * Converts an octet_string to hex string representation.
 *
 * @param str the octet_string to be converted into hex string.
 * @return the hex string representation.
 */
char *octet_string2hex(octet_string *str)
{
	int size = str->length*2;
	char *result = calloc(size+1, sizeof(char));
	int i = 0;

	for (i = 0; i < str->length; i++) {
		sprintf(result, "%s%.2X", result, str->value[i]);
	}

	result[size] = '\0';

	return result;
}

/**
 * Converts an HighResRelativeTime to hex string representation.
 *
 * @param time the HighResRelativeTime to be converted into hex string.
 * @return the hex string representation.
 */
char *high_res_relative_time2hex(HighResRelativeTime *time)
{
	int time_length = 8;
	int size = time_length*2;
	char *result = calloc(size+1, sizeof(char));
	int i = 0;

	for (i = 0; i < time_length; i++) {
		sprintf(result, "%s%.2X", result, time->value[i]);
	}

	result[size] = '\0';

	return result;
}


/**
 * Converts a BCD time to string representation.
 *
 * @param value the BCD time to be converted into string.
 * @return the string representation.
 */
char *bcdtime2number(intu8 value)
{
	return intu8_2str(date_util_convert_bcd_to_number(value));
}


/** @} */
