/**********************************************************************
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
 * testbytelib.c
 *
 * Created on: Set 03, 2010
 *     Author: Adrian Livio Vasconcelos Guedes
 **********************************************************************/

#ifdef TEST_ENABLED

#include "Basic.h"
#include "src/asn1/phd_types.h"
#include "src/dim/nomenclature.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/util/bytelib.h"
#include "src/util/ioutil.h"
#include "src/util/log.h"
#include "tests/functional_test_cases/test_functional.h"

#include "testbytelib.h"

#include <stdlib.h>
#include <stdio.h>

int testbytelib_init_suite(void)
{
	return 0;
}

int testbytelib_finish_suite(void)
{
	return 0;
}

void testbytelib_add_suite()
{
	CU_pSuite suite = CU_add_suite("Bytelib Test Suite",
				       testbytelib_init_suite, testbytelib_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_read_intu8", test_read_intu8);
	CU_add_test(suite, "test_read_intu8_many", test_read_intu8_many);
	CU_add_test(suite, "test_write_intu8_many", test_write_intu8_many);

	CU_add_test(suite, "test_read_intu16", test_read_intu16);
	CU_add_test(suite, "test_read_intu32", test_read_intu32);

	CU_add_test(suite, "test_float", test_float);
	/* Add tests here - End */
}

void test_read_intu8()
{
	intu8 test_data[2] = { 0xF3, 0xCF };
	ByteStreamReader *stream = byte_stream_reader_instance(test_data, 2);
	int error = 0;

	intu8 data = read_intu8(stream, &error);
	CU_ASSERT(!error);
	CU_ASSERT_EQUAL(data, 243);

	error = 0;
	data = read_intu8(stream, &error);
	CU_ASSERT(!error);
	CU_ASSERT_EQUAL(data, 207);

	error = 0;
	data = read_intu8(stream, &error);
	CU_ASSERT(error);
	CU_ASSERT_EQUAL(data, 0);

	free(stream);
	stream = NULL;
	error = 0;
	data = read_intu8(stream, &error);
	CU_ASSERT(error);
	CU_ASSERT_EQUAL(data, 0);
}

void test_read_intu8_many()
{
	intu8 test_data[10] = {0xF0, 0xda, 0x53, 0x00, 0x51, 0x73, 0x1a, 0x70, 0xff, 0x00};
	intu8 data[100];
	ByteStreamReader *stream = byte_stream_reader_instance(test_data, 10);
	int error = 0;

	read_intu8_many(stream, data, 11, &error);
	CU_ASSERT(error);

	error = 0;
	read_intu8_many(stream, data, 5, &error);
	CU_ASSERT(!error);

	CU_ASSERT_EQUAL(data[0], 0xf0);
	CU_ASSERT_EQUAL(data[1], 0xda);
	CU_ASSERT_EQUAL(data[2], 0x53);
	CU_ASSERT_EQUAL(data[3], 0x00);
	CU_ASSERT_EQUAL(data[4], 0x51);

	error = 0;
	read_intu8_many(stream, data, 6, &error);
	CU_ASSERT(error);

	error = 0;
	read_intu8_many(stream, data, 5, &error);
	CU_ASSERT(!error);

	CU_ASSERT_EQUAL(data[0], 0x73);
	CU_ASSERT_EQUAL(data[1], 0x1a);
	CU_ASSERT_EQUAL(data[2], 0x70);
	CU_ASSERT_EQUAL(data[3], 0xff);
	CU_ASSERT_EQUAL(data[4], 0x00);

	free(stream);
	stream = NULL;

	error = 0;
	data[0] = read_intu8(stream, &error);
	CU_ASSERT(error);
}


void test_write_intu8_many()
{
	int size = 10;
	intu8 data[10] = {0xF0, 0xda, 0x53, 0x00, 0x51, 0x73, 0x1a, 0x70, 0xff, 0x00};
	intu8 trash[3] = {0x00, 0x00, 0x00};

	ByteStreamWriter *stream = byte_stream_writer_instance(10);

	CU_ASSERT_TRUE(write_intu8_many(stream, data, 10));
	CU_ASSERT_FALSE(write_intu8_many(stream, trash, 3));

	CU_ASSERT_EQUAL(stream->size, size);
	int i;

	for (i = 0; i < size; i++) {
		CU_ASSERT_EQUAL(data[i], stream->buffer[i]);
	}

	del_byte_stream_writer(stream, 1);
}

void test_read_intu16()
{
	intu8 test_data[] = { 0xF3, 0xCF, 0x25, 0xB4, 0x66 };
	ByteStreamReader *stream = byte_stream_reader_instance(test_data, 5);
	int error = 0;

	intu16 data = read_intu16(stream, &error);
	CU_ASSERT(!error);
	CU_ASSERT_EQUAL(data, 62415);

	error = 0;
	data = read_intu16(stream, &error);
	CU_ASSERT(!error);
	CU_ASSERT_EQUAL(data, 9652);

	error = 0;
	data = read_intu16(stream, &error);
	CU_ASSERT(error);
	CU_ASSERT_EQUAL(data, 0);

	error = 0;
	data = read_intu16(stream, &error);
	CU_ASSERT(error);
	CU_ASSERT_EQUAL(data, 0);

	error = 0;
	free(stream);
	stream = NULL;
	data = read_intu16(stream, &error);
	CU_ASSERT(error);
	CU_ASSERT_EQUAL(data, 0);
}

void test_read_intu32()
{
	intu8 test_data[] = { 0xAD, 0x1A, 0x11, 0x10, 0xA5, 0xC0, 0x0C, 0xE0,
			      0xED
			    };
	ByteStreamReader *stream = byte_stream_reader_instance(test_data, 9);
	int error = 0;

	intu32 data = read_intu32(stream, &error);
	CU_ASSERT(!error);
	CU_ASSERT_EQUAL(data, 0xAD1A1110);

	error = 0;
	data = read_intu32(stream, &error);
	CU_ASSERT(!error);
	CU_ASSERT_EQUAL(data, 0xA5C00CE0);

	error = 0;
	data = read_intu32(stream, &error);
	CU_ASSERT(error);
	CU_ASSERT_EQUAL(data, 0);

	error = 0;
	data = read_intu32(stream, &error);
	CU_ASSERT(error);
	CU_ASSERT_EQUAL(data, 0);

	free(stream);
	stream = NULL;
	error = 0;
	data = read_intu32(stream, &error);
	CU_ASSERT(error);
	CU_ASSERT_EQUAL(data, 0);
}

void test_float()
{
	int error = 0; // FIXME
	ByteStreamWriter *stream = byte_stream_writer_instance(256);
	unsigned char test_data[256];

	CU_ASSERT_TRUE(write_float(stream, 0));
	CU_ASSERT_TRUE(write_float(stream, 0.5));
	CU_ASSERT_TRUE(write_float(stream, -0.5));
	CU_ASSERT_TRUE(write_float(stream, 25.1));
	CU_ASSERT_TRUE(write_float(stream, -25.1));
	CU_ASSERT_TRUE(write_float(stream, 36.7));
	CU_ASSERT_TRUE(write_float(stream, -36.7));

	memcpy(test_data, stream->buffer, 4*7);

	ByteStreamReader *rstream = byte_stream_reader_instance(test_data, 256);
	
	// numbers with exact float representation
	CU_ASSERT_EQUAL(read_float(rstream, &error), 0);
	CU_ASSERT_EQUAL(read_float(rstream, &error), 0.5);
	CU_ASSERT_EQUAL(read_float(rstream, &error), -0.5);
	// numbers w/o exact float representation
	CU_ASSERT_TRUE(fabs(read_float(rstream, &error) - 25.1) < 0.00001);
	CU_ASSERT_TRUE(fabs(read_float(rstream, &error) + 25.1) < 0.00001);
	CU_ASSERT_TRUE(fabs(read_float(rstream, &error) - 36.7) < 0.00001);
	CU_ASSERT_TRUE(fabs(read_float(rstream, &error) + 36.7) < 0.00001);

	del_byte_stream_writer(stream, 1);
	free(rstream);
}

#endif
