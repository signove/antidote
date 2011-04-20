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
 * testdateutil.c
 *
 * Created on: Jun 17, 2010
 *     Author: Jose Martins da Nobrega Filho
 **********************************************************************/

#ifdef TEST_ENABLED

#include "testdateutil.h"
#include "src/util/dateutil.h"
#include "src/asn1/phd_types.h"
#include "Basic.h"
#include <stdio.h>

int test_dateutil_init_suite(void)
{
	return 0;
}

int test_dateutil_finish_suite(void)
{
	return 0;
}

void testdateutil_add_suite()
{
	CU_pSuite suite = CU_add_suite("DateUtil Test Suite", test_dateutil_init_suite, test_dateutil_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_dateutil_bcd_convertion", test_dateutil_bcd_convertion);
	CU_add_test(suite, "test_dateutil_absolute_time_creation", test_dateutil_absolute_time_creation);
	CU_add_test(suite, "test_dateutil_absolute_time_comparation", test_dateutil_absolute_time_comparation);
	/* Add tests here - End */
}

void test_dateutil_bcd_convertion(void)
{
	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(1), 0x1);
	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(5), 0x5);
	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(9), 0x9);

	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(10), 0x10);
	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(17), 0x17);

	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(20), 0x20);
	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(28), 0x28);

	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(30), 0x30);
	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(31), 0x31);
	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(39), 0x39);

	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(90), 0x90);
	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(91), 0x91);
	CU_ASSERT_EQUAL(date_util_convert_number_to_bcd(99), 0x99);
}
void test_dateutil_absolute_time_creation(void)
{
	AbsoluteTime result_time = date_util_create_absolute_time(1998, 10, 25, 13, 54, 35, 1);

	CU_ASSERT_EQUAL(result_time.century, 0x19);
	CU_ASSERT_EQUAL(result_time.year, 0x98);
	CU_ASSERT_EQUAL(result_time.month, 0x10);
	CU_ASSERT_EQUAL(result_time.day, 0x25);
	CU_ASSERT_EQUAL(result_time.hour, 0x13);
	CU_ASSERT_EQUAL(result_time.minute, 0x54);
	CU_ASSERT_EQUAL(result_time.second, 0x35);
	CU_ASSERT_EQUAL(result_time.sec_fractions, 0x01);

	result_time = date_util_create_absolute_time(2000, 1, 31, 0, 0, 0, 0);

	CU_ASSERT_EQUAL(result_time.century, 0x20);
	CU_ASSERT_EQUAL(result_time.year, 0x00);
	CU_ASSERT_EQUAL(result_time.month, 0x01);
	CU_ASSERT_EQUAL(result_time.day, 0x31);
	CU_ASSERT_EQUAL(result_time.hour, 0x00);
	CU_ASSERT_EQUAL(result_time.minute, 0x00);
	CU_ASSERT_EQUAL(result_time.second, 0x00);
	CU_ASSERT_EQUAL(result_time.sec_fractions, 0x00);

	result_time = date_util_create_absolute_time(9999, 99, 99, 99, 99, 99, 99);

	CU_ASSERT_EQUAL(result_time.century, 0x99);
	CU_ASSERT_EQUAL(result_time.year, 0x99);
	CU_ASSERT_EQUAL(result_time.month, 0x99);
	CU_ASSERT_EQUAL(result_time.day, 0x99);
	CU_ASSERT_EQUAL(result_time.hour, 0x99);
	CU_ASSERT_EQUAL(result_time.minute, 0x99);
	CU_ASSERT_EQUAL(result_time.second, 0x99);
	CU_ASSERT_EQUAL(result_time.sec_fractions, 0x99);

	result_time = date_util_create_absolute_time(0, 0, 0, 0, 0, 0, 0);

	CU_ASSERT_EQUAL(result_time.century, 0x00);
	CU_ASSERT_EQUAL(result_time.year, 0x00);
	CU_ASSERT_EQUAL(result_time.month, 0x00);
	CU_ASSERT_EQUAL(result_time.day, 0x00);
	CU_ASSERT_EQUAL(result_time.hour, 0x00);
	CU_ASSERT_EQUAL(result_time.minute, 0x00);
	CU_ASSERT_EQUAL(result_time.second, 0x00);
	CU_ASSERT_EQUAL(result_time.sec_fractions, 0x00);
}
void test_dateutil_absolute_time_comparation(void)
{
}

#endif
