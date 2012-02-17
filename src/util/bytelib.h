/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file bytelib.h
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
 * \addtogroup ASN1Codec
 *
 * @{
 */
#ifndef BYTELIB_H_
#define BYTELIB_H_

#include <asn1/phd_types.h>


/**
 * ByteStreamReader is used to read byte array as a contiguous stream.
 *
 */
typedef struct ByteStreamReader {
	/**
	 *
	 * Number of unread bytes
	 */
	intu32 unread_bytes;
	/**
	 * The pointer to current offset in processing
	 */
	intu8 *buffer_cur;

	/**
	 * The pointer to start of buffer_cur
	 */
	intu8 *buffer;

} ByteStreamReader;

/**
 * ByteStreamWriter is used to aggregate byte array as a contiguous stream.
 */
typedef struct ByteStreamWriter {
	intu32 size;

	intu8 *buffer;
	intu8 *buffer_cur;
	intu8 *buffer_end;
} ByteStreamWriter;

ByteStreamReader *byte_stream_reader_instance(intu8 *stream, intu32 size);

intu8 read_intu8(ByteStreamReader *stream, int *error);

void read_intu8_many(ByteStreamReader *stream, intu8 *buf, int len, int *error);

intu16 read_intu16(ByteStreamReader *stream, int *error);

intu32 read_intu32(ByteStreamReader *stream, int *error);

FLOAT_Type read_float(ByteStreamReader *stream, int *error);

SFLOAT_Type read_sfloat(ByteStreamReader *stream, int *error);

ByteStreamWriter *byte_stream_writer_instance(intu32 size);

intu32 write_intu8(ByteStreamWriter *stream, intu8 data);

intu32 write_intu8_many(ByteStreamWriter *stream, intu8 *data, int len);

intu32 write_intu16(ByteStreamWriter *stream, intu16 data);

intu32 write_intu32(ByteStreamWriter *stream, intu32 data);

intu32 write_sfloat(ByteStreamWriter *stream, SFLOAT_Type data);

intu32 write_float(ByteStreamWriter *stream, FLOAT_Type data);

void del_byte_stream_writer(ByteStreamWriter *stream, int del_fields);
void del_byte_stream_reader(ByteStreamReader *stream, int del_fields);

/** @} */

#endif /* BYTELIB_H_ */

