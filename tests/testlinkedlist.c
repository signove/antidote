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

#include "testlinkedlist.h"
#include "src/util/linkedlist.h"
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

void testllist_add_suite()
{
	CU_pSuite suite = CU_add_suite("Linked list Test Suite",
				       test_init_suite, test_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "testllist_test", testllist_test);

	/* Add tests here - End */

}
static int search(void *arg, void *element)
{
	return arg == element;
}

void testllist_test()
{
	LinkedList *l = llist_new();

	llist_add(l, (void *) 10);
	llist_add(l, (void *) 20);
	llist_add(l, (void *) 30);
	llist_add(l, (void *) 40);
	llist_add(l, (void *) 50);


	CU_ASSERT_EQUAL(5, l->size);

	CU_ASSERT_EQUAL((void *) 10, llist_get(l, 0));
	CU_ASSERT_EQUAL(0, llist_index_of(l, (void *) 10));

	CU_ASSERT_EQUAL((void *) 20, llist_get(l, 1));
	CU_ASSERT_EQUAL(1, llist_index_of(l, (void *) 20));

	CU_ASSERT_EQUAL((void *) 30, llist_get(l, 2));
	CU_ASSERT_EQUAL(2, llist_index_of(l, (void *) 30));

	CU_ASSERT_EQUAL((void *) 10, llist_search_first(l, (void *) 10, search));
	CU_ASSERT_EQUAL((void *) 20, llist_search_first(l, (void *) 20, search));
	CU_ASSERT_EQUAL((void *) 30, llist_search_first(l, (void *) 30, search));


	CU_ASSERT_TRUE(llist_remove(l, (void *) 10));
	CU_ASSERT_EQUAL(4, l->size);
	CU_ASSERT_PTR_NULL(llist_search_first(l, (void *) 10, search));

	CU_ASSERT_TRUE(llist_remove(l, (void *) 20));
	CU_ASSERT_EQUAL(3, l->size);
	CU_ASSERT_PTR_NULL(llist_search_first(l, (void *) 20, search));

	CU_ASSERT_TRUE(llist_remove(l, (void *) 30));
	CU_ASSERT_EQUAL(2, l->size);
	CU_ASSERT_PTR_NULL(llist_search_first(l, (void *) 20, search));


	llist_destroy(l, NULL);
}

#endif
