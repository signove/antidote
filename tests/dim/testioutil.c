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
 * testioutil.c
 *
 * Created on: Sep 2, 2010
 *     Author: Adrian Livio Vasconcelos Guedes
 **********************************************************************/

#ifdef TEST_ENABLED

#include "testioutil.h"
#include "src/util/ioutil.h"
#include "Basic.h"
#include <stdio.h>
#include <stdlib.h>

int test_ioutil_init_suite(void)
{
	return 0;
}

int test_ioutil_finish_suite(void)
{
	return 0;
}

void testioutil_add_suite()
{
	CU_pSuite suite = CU_add_suite("IOUtil Test Suite",
				       test_ioutil_init_suite, test_ioutil_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_ioutil_get_tmp", test_ioutil_get_tmp);
	/* Add tests here - End */
}

void test_ioutil_get_tmp(void)
{
	char *tmp = ioutil_get_tmp();
	free(tmp);
}
#endif
