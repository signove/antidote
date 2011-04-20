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
 * pmsegment.c
 *
 * Created on: Jun 16, 2010
 *     Author: Jose Martins da Nobrega Filho
**********************************************************************/

#ifdef TEST_ENABLED

#include "testpmsegment.h"
#include "Basic.h"
#include <stdio.h>

int testpmsegment_init_suite(void)
{
	return 0;
}

int testpmsegment_finish_suite(void)
{
	return 0;
}

void testpmsegment_add_suite()
{
	CU_pSuite suite = CU_add_suite("PMSegment Test Suite", testpmsegment_init_suite, testpmsegment_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_pmsegment_test1", test_pmsegment_test1);

	/* Add tests here - End */
}

void test_pmsegment_test1(void)
{
}

#endif
