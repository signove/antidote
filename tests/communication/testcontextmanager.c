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
 * testfsm.c
 *
 * Created on: Jun 22, 2010
 *     Author: fabricio.silva
 **********************************************************************/

#ifdef TEST_ENABLED

#include "testcontextmanager.h"
#include "src/communication/context_manager.h"
#include "Basic.h"
#include <stdio.h>


static int test_init_suite(void)
{
	return 0;
}

static int test_finish_suite(void)
{
	return 0;
}

void testctxmanager_add_suite()
{
	CU_pSuite suite = CU_add_suite("Context Manager Test Suite", test_init_suite,
				       test_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "testctxmanager_test", testctxmanager_test);

	/* Add tests here - End */

}

void testctxmanager_test()
{
	// TODO check MDS and FSM destruction

	/*
	Context *c1 = context_create(1);
	Context *c2 = context_create(2);
	Context *c3 = context_create(3);

	CU_ASSERT_EQUAL(c1, context_get(1));
	CU_ASSERT_EQUAL(c2, context_get(2));
	CU_ASSERT_EQUAL(c3, context_get(3));

	context_remove(c1->id);
	CU_ASSERT_PTR_NULL(context_get(1));

	CU_ASSERT_EQUAL(c2, context_get(2));

	context_remove(c2->id);
	CU_ASSERT_PTR_NULL(context_get(2));

	CU_ASSERT_EQUAL(c3, context_get(3));

	context_remove(c3->id);
	CU_ASSERT_PTR_NULL(context_get(3));

	context_remove_all();
	*/
}


#endif
