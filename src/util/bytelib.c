/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file bytelib.c
 * \brief Byte manipulation module implementation.
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
 * \author Walter Guerra, Mateus Lima
 * \date Jun 14, 2010
 */

/**
 * \addtogroup ByteLib
 *
 * \ingroup Parser
 *
 * @{
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#ifndef WIN32
        #include <arpa/inet.h>
#else
        #include<float.h>
        #include<WinSock2.h>
#endif
#include "src/util/log.h"
#include "bytelib.h"

typedef enum {
	MDER_POSITIVE_INFINITY = 0x007FFFFE,
	MDER_NaN = 0x007FFFFF,
	MDER_NRes = 0x00800000,
	MDER_RESERVED_VALUE = 0x00800001,
	MDER_NEGATIVE_INFINITY = 0x00800002
} ReservedFloatValues;
static const int32 FIRST_RESERVED_VALUE = MDER_POSITIVE_INFINITY;

// (2 ** 23 - 3)
#define MDER_FLOAT_MANTISSA_MAX 0x007FFFFD
// 2 ** 7 - 1
#define MDER_FLOAT_EXPONENT_MAX 127
#define MDER_FLOAT_EXPONENT_MIN -128
// (2 ** 23 - 3) * 10 ** 127
#define MDER_FLOAT_MAX 8.388604999999999e+133
// -(2 ** 23 - 3) * 10 ** 127
#define MDER_FLOAT_MIN (-MDER_FLOAT_MAX)
// 10 ** -128
#define MDER_FLOAT_EPSILON 1e-128
// 10 ** upper(23 * log(2) / log(10))
// precision for a number 1.0000xxx
#define MDER_FLOAT_PRECISION 10000000

typedef enum {
	MDER_S_POSITIVE_INFINITY = 0x07FE,
	MDER_S_NaN = 0x07FF,
	MDER_S_NRes = 0x0800,
	MDER_S_RESERVED_VALUE = 0x0801,
	MDER_S_NEGATIVE_INFINITY = 0x0802
} ReservedSFloatValues;
static const intu32 FIRST_S_RESERVED_VALUE = MDER_S_POSITIVE_INFINITY;

// (2 ** 11 - 3)
#define MDER_SFLOAT_MANTISSA_MAX 0x07FD
// 2 ** 3 - 1
#define MDER_SFLOAT_EXPONENT_MAX 7
#define MDER_SFLOAT_EXPONENT_MIN -8
// (2 ** 11 - 3) * 10 ** 7
#define MDER_SFLOAT_MAX 20450000000.0
// -(2 ** 11 - 3) * 10 ** 7
#define MDER_SFLOAT_MIN (-MDER_SFLOAT_MAX)
// 10 ** -8
#define MDER_SFLOAT_EPSILON 1e-8
// 10 ** upper(11 * log(2) / log(10))
#define MDER_SFLOAT_PRECISION 10000

#ifdef WIN32
int round(double number)
{
    return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
}

#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
#define NAN _FPCLASS_SNAN
#define INFINITY _FPCLASS_PINF

#endif

static const double reserved_float_values[5] = {INFINITY, NAN, NAN, NAN, -INFINITY};

/**
 * Bytelib constructor.
 *
 * @param *buffer Input data array
 * @param size Input data array size
 * @return ByteStreamReader A ByteStreamReader pointer.
 */
ByteStreamReader *byte_stream_reader_instance(intu8 *buffer, intu32 size)
{
	if (buffer == NULL || size == 0) {
		return NULL;
	}

	ByteStreamReader *stream = (ByteStreamReader *) malloc(sizeof(ByteStreamReader));
	stream->buffer_cur = buffer;
	stream->buffer = buffer;
	stream->unread_bytes = size;

	return stream;
}

/**
 * Consumes an intu8 from data.
 *
 * @param stream The current ByteStreamReader.
 * @param error A reference to a boolean to hold the error code.
 * @return intu8 Consumed from stream.
 */
intu8 read_intu8(ByteStreamReader *stream, int *error)
{
	intu8 ret = 0;

	if (stream && stream->unread_bytes > 0) {
		ret = *(stream->buffer_cur);
		stream->buffer_cur++;
		stream->unread_bytes--;
	} else {
		if (error) {
			*error = 1;
		}

		ERROR("read_intu8")
		;
	}

	return ret;
}

/**
 * Consumes a number of intu8's from data.
 *
 * @param stream The current ByteStreamReader.
 * @param buf The target buffer where bytes are going to
 * @param len The exact number of bytes that are to be consumed
 * @param error A reference to a boolean to hold the error code.
 */
void read_intu8_many(ByteStreamReader *stream, intu8 *buf, int len, int *error)
{
	if (stream && stream->unread_bytes >= (unsigned) len) {
		memcpy(buf, stream->buffer_cur, len);
		stream->buffer_cur += len;
		stream->unread_bytes -= len;
	} else {
		if (error) {
			*error = 1;
		}

		ERROR("read_intu8_many")
		;
	}
}

/**
 * Consumes an intu16 from data, rearranging it to the proper endianism.
 *
 * @param stream The current ByteStreamReader.
 * @param error A reference to a boolean to hold the error code.
 * @return intu16 Consumed from stream rearranged.
 */
intu16 read_intu16(ByteStreamReader *stream, int *error)
{
	intu16 ret = 0;

	if (stream && stream->unread_bytes > 1) {
		ret = ntohs(*((uint16_t *) stream->buffer_cur));

		stream->buffer_cur += 2;
		stream->unread_bytes -= 2;
	} else {
		if (error) {
			*error = 1;
		}

		ERROR("read_intu16");
	}

	return ret;
}

/**
 * Consumes an intu32 from data, rearranging it to the proper endianism.
 *
 * @param stream The current ByteStreamReader.
 * @param error A reference to a boolean to hold the error code.
 * @return intu32 Consumed from stream rearranged.
 */
intu32 read_intu32(ByteStreamReader *stream, int *error)
{
	intu32 ret = 0;

	if (stream && stream->unread_bytes > 3) {
		ret = ntohl(*((uint32_t *) stream->buffer_cur));
		stream->buffer_cur += 4;
		stream->unread_bytes -= 4;
	} else {
		if (error) {
			*error = 1;
		}

		ERROR("read_intu32");
	}

	return ret;
}

/**
 * Consumes an intu32 from data, and calculates the float as described in MDER Annex F.6.
 *
 * @param stream The current ByteStreamReader.
 * @param error Error feedback
 * @return float Converted float from stream.
 */
FLOAT_Type read_float(ByteStreamReader *stream, int *error)
{
	intu32 int_data = read_intu32(stream, error);
	if (*error)
		return 0;

	int32 mantissa = int_data & 0xFFFFFF;
	int8 expoent = int_data >> 24;
	double output = 0;

	if (mantissa >= FIRST_RESERVED_VALUE &&
					mantissa <= MDER_NEGATIVE_INFINITY) {
		output = reserved_float_values[mantissa - FIRST_RESERVED_VALUE];
	} else {
		if (mantissa >= 0x800000) {
			mantissa = -((0xFFFFFF + 1) - mantissa);
		}
		output = (mantissa * pow(10.0f, expoent));
	}

	return output;
}

/* round number n to d decimal points */
inline double fround(double n, int d)
{
	return floor(n * pow(10.0f, d) + 0.5f) / pow(10.0f, d);
}

/**
 * Consumes an intu16 from data, and calculates the sfloat as described in MDER Annex F.7.
 *
 * @param stream The current ByteStreamReader.
 * @param error Error feedback
 * @return float Converted float from stream.
 */
SFLOAT_Type read_sfloat(ByteStreamReader *stream, int *error)
{
	intu16 int_data = read_intu16(stream, error);
	if (*error)
		return 0;

	intu16 mantissa = int_data & 0x0FFF;
	int8 expoent = int_data >> 12;

	if (expoent >= 0x0008) {
		expoent = -((0x000F + 1) - expoent);
	}

	float output = 0;

	if (mantissa >= FIRST_S_RESERVED_VALUE && mantissa
	    <= MDER_S_NEGATIVE_INFINITY) {
		output = reserved_float_values[mantissa
					       - FIRST_S_RESERVED_VALUE];
	} else {
		if (mantissa >= 0x0800) {
			mantissa = -((0x0FFF + 1) - mantissa);
		}
		double magnitude = pow(10.0f, expoent);
		output = (mantissa * magnitude);
	}

	return output;
}


/**
 * ByteStreamWriter constructor.
 *
 * @param size Data stream max size.
 * @return ByteStreamWriter A ByteStreamWriter pointer.
 */
ByteStreamWriter *byte_stream_writer_instance(intu32 size)
{
	ByteStreamWriter *stream = (ByteStreamWriter *) calloc(1, sizeof(ByteStreamWriter));
	stream->buffer = (intu8 *) calloc(1, size * sizeof(intu8));
	stream->size = 0;
	stream->buffer_size = size;
	stream->open = 0;

	return stream;
}

/**
 * ByteStreamWriter constructor (open version)
 *
 * @param hint Hint of data stream size for optimization
 * @return ByteStreamWriter A ByteStreamWriter pointer.
 */
ByteStreamWriter *open_stream_writer(intu32 hint)
{
	ByteStreamWriter *stream = byte_stream_writer_instance(hint);
	stream->open = 1;
	return stream;
}


/**
 * Checks stream size and extends if necessary (if it iso open)
 *
 * @param stream The stream
 * @param need Additional octets needed
 */
static int check_writer(ByteStreamWriter *stream, int need)
{
	if ((signed) (stream->size + need) <= stream->buffer_size) {
		return 1;
	}

	if (! stream->open) {
		// keep old behavior, let it fail
		return 0;
	}
	
	// make more space
	int increase = stream->buffer_size + need;
	stream->buffer = realloc(stream->buffer, stream->buffer_size + increase);
	memset(stream->buffer + stream->buffer_size, 0, increase);
	stream->buffer_size += increase;

	return 1;
}


/**
 * Writes an intu8 into stream.
 *
 * @param stream The current ByteStreamWriter.
 * @param data intu8 to be written.
 * @return Error code - count of octets written, (0) error
 */
intu32 write_intu8(ByteStreamWriter *stream, intu8 data)
{
	if (check_writer(stream, 1)) {
		*(stream->buffer + stream->size) = data;
		stream->size++;
		return 1; // true
	} else {
		ERROR("write_intu8");
		return 0; // false
	}

}

/**
 * Writes a number of intu8's into stream.
 *
 * @param stream The current ByteStreamWriter.
 * @param data intu8 buffer to be written.
 * @param len the exact number of intu8's to be written
 * @param error 1 if error, 0 if ok (unambiguous in case of len==0)
 * @return Error code - count of octets written, (0) error
 */
intu32 write_intu8_many(ByteStreamWriter *stream, intu8 *data, int len, int *error)
{
	if (check_writer(stream, len)) {
		memcpy(stream->buffer + stream->size, data, len);
		stream->size += len;
		*error = 0;
		return len; 
	} else {
		ERROR("write_intu8_many %d > %d", len, stream->buffer_size);
		*error = 1;
		return 0; // false
	}

}

/**
 * Writes an intu16 into stream, rearranging it to the proper endianism.
 *
 * @param stream The current ByteStreamWriter.
 * @param data intu16 to be written.
 * @return Error code - count of octets written, (0) error
 */
intu32 write_intu16(ByteStreamWriter *stream, intu16 data)
{
	if (check_writer(stream, 2)) {
		*((uint16_t *) (stream->buffer + stream->size)) = htons(data);
		stream->size += 2;
		return 2; // true
	} else {
		ERROR("write_intu16");
		return 0; // false
	}
}

/**
 * Reserve space for one intu16 in stream, generally for a 'length' field
 *
 * @param stream The current ByteStreamWriter.
 * @param position Pointer to store position of intu16 in stream
 * @return octets written or (0) error
 */
intu32 reserve_intu16(ByteStreamWriter *stream, int* position)
{
	if (check_writer(stream, 2)) {
		*position = stream->size;
		*((uint16_t *) (stream->buffer + stream->size)) = 0;
		stream->size += 2;
		return 2; // true
	} else {
		ERROR("reserve_intu16");
		return 0; // false
	}
}

/**
 * Use space reserved for one intu16 in stream, for a 'length' field
 *
 * @param stream The writer stream
 * @param position the position in stream to be filled
 * @param data intu16 to be written.
 */
void commit_intu16(ByteStreamWriter *stream, int position, intu16 data)
{
	*((uint16_t *) (stream->buffer + position)) = htons(data);
}

/**
 * Writes an intu16 into stream, rearranging it to the proper endianism.
 *
 * @param stream The current ByteStreamWriter.
 * @param data intu32 to be written.
 * @return Error code - count of octets written, (0) error
 */
intu32 write_intu32(ByteStreamWriter *stream, intu32 data)
{
	if (check_writer(stream, 4)) {
		*((uint32_t *) (stream->buffer + stream->size)) = htonl(data);
		stream->size += 4;
		return 4; // true
	} else {
		ERROR("write_intu32");
		return 0; // false
	}
}

/**
 * Writes an intu16 from data based on the float as described in MDER Annex F.8.
 *
 * @param stream The current ByteStreamWriter.
 * @param data to be converted to intu32 and written into stream.
 * @return Error code - count of octets written, (0) error
 */
intu32 write_sfloat(ByteStreamWriter *stream, SFLOAT_Type data)
{
	intu16 result = MDER_S_NaN;

	if (isnan(data)) {
		goto finally;
	} else if (data > MDER_SFLOAT_MAX) {
		result = MDER_S_POSITIVE_INFINITY;
		goto finally;
	} else if (data < MDER_FLOAT_MIN) {
		result = MDER_S_NEGATIVE_INFINITY;
		goto finally;
	} else if (data >= -MDER_SFLOAT_EPSILON &&
		data <= MDER_SFLOAT_EPSILON) {
		result = 0;
		goto finally;
	}

	double sgn = data > 0 ? +1 : -1;
	double mantissa = fabs(data);
	int exponent = 0; // Note: 10**x exponent, not 2**x

	// scale up if number is too big
	while (mantissa > MDER_SFLOAT_MANTISSA_MAX) {
		mantissa /= 10.0;
		++exponent;
		if (exponent > MDER_SFLOAT_EXPONENT_MAX) {
			// argh, should not happen
			if (sgn > 0) {
				result = MDER_S_POSITIVE_INFINITY;
			} else {
				result = MDER_S_NEGATIVE_INFINITY;
			}
			goto finally;
		}
	}

	// scale down if number is too small
	while (mantissa < 1) {
		mantissa *= 10;
		--exponent;
		if (exponent < MDER_SFLOAT_EXPONENT_MIN) {
			// argh, should not happen
			result = 0;
			goto finally;
		}
	}

	// scale down if number needs more precision
	double smantissa = round(mantissa * MDER_SFLOAT_PRECISION);
	double rmantissa = round(mantissa) * MDER_SFLOAT_PRECISION;
	double mdiff = abs(smantissa - rmantissa);
	while (mdiff > 0.5 && exponent > MDER_SFLOAT_EXPONENT_MIN &&
			(mantissa * 10) <= MDER_SFLOAT_MANTISSA_MAX) {
		mantissa *= 10;
		--exponent;
		smantissa = round(mantissa * MDER_SFLOAT_PRECISION);
		rmantissa = round(mantissa) * MDER_SFLOAT_PRECISION;
		mdiff = abs(smantissa - rmantissa);
	}

	intu16 int_mantissa = (int) round(sgn * mantissa);
	result = ((exponent & 0xF) << 12) | (int_mantissa & 0xFFF);

finally:
	return write_intu16(stream, result);
}

/**
 * Writes an intu32 from data based on the float as described in MDER Annex F.8.
 *
 * @param stream The current ByteStreamWriter.
 * @param data to be converted to intu32 and written into stream.
 * @return Error code - count of octets written, (0) error
 */
intu32 write_float(ByteStreamWriter *stream, FLOAT_Type data)
{
	intu32 result = MDER_NaN;

	if (isnan(data)) {
		goto finally;
	} else if (data > MDER_FLOAT_MAX) {
		result = MDER_POSITIVE_INFINITY;
		goto finally;
	} else if (data < MDER_FLOAT_MIN) {
		result = MDER_NEGATIVE_INFINITY;
		goto finally;
	} else if (data >= -MDER_FLOAT_EPSILON &&
		data <= MDER_FLOAT_EPSILON) {
		result = 0;
		goto finally;
	}

	double sgn = data > 0 ? +1 : -1;
	double mantissa = fabs(data);
	int exponent = 0; // Note: 10**x exponent, not 2**x

	// scale up if number is too big
	while (mantissa > MDER_FLOAT_MANTISSA_MAX) {
		mantissa /= 10.0;
		++exponent;
		if (exponent > MDER_FLOAT_EXPONENT_MAX) {
			// argh, should not happen
			if (sgn > 0) {
				result = MDER_POSITIVE_INFINITY;
			} else {
				result = MDER_NEGATIVE_INFINITY;
			}
			goto finally;
		}
	}

	// scale down if number is too small
	while (mantissa < 1) {
		mantissa *= 10;
		--exponent;
		if (exponent < MDER_FLOAT_EXPONENT_MIN) {
			// argh, should not happen
			result = 0;
			goto finally;
		}
	}

	// scale down if number needs more precision
	double smantissa = round(mantissa * MDER_FLOAT_PRECISION);
	double rmantissa = round(mantissa) * MDER_FLOAT_PRECISION;
	double mdiff = abs(smantissa - rmantissa);
	while (mdiff > 0.5 && exponent > MDER_FLOAT_EXPONENT_MIN &&
			(mantissa * 10) <= MDER_FLOAT_MANTISSA_MAX) {
		mantissa *= 10;
		--exponent;
		smantissa = round(mantissa * MDER_FLOAT_PRECISION);
		rmantissa = round(mantissa) * MDER_FLOAT_PRECISION;
		mdiff = abs(smantissa - rmantissa);
	}

	intu32 int_mantissa = (int) round(sgn * mantissa);
	result = (exponent << 24) | (int_mantissa & 0xFFFFFF);

finally:
	return write_intu32(stream, result);
}

/**
 * ByteStreamWriter destructor.
 *
 * @param del_fields inform 1 to also delete fields of structure
 * @param stream ByteStreamWriter to be destroyed.
 * @return ByteStreamWriter A ByteStreamWriter pointer to be freed.
 */
void del_byte_stream_writer(ByteStreamWriter *stream, int del_fields)
{
	if (stream) {
		if (del_fields) {
			free(stream->buffer);
			stream->buffer = NULL;
		}

		free(stream);
	}
}

/**
 * ByteStreamWriter destructor.
 * @param del_fields inform 1 to also delete fields of structure.
 * @param stream ByteStreamReader to be destroyed.
 * @return ByteStreamReader A ByteStreamReader pointer to be freed.
 */
void del_byte_stream_reader(ByteStreamReader *stream, int del_fields)
{
	if (stream) {
		if (del_fields) {
			free(stream->buffer);
			stream->buffer = NULL;
		}

		free(stream);
	}
}
/** @} */
