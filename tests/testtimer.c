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
 * testtimer.c
 *
 * Created on: Jul 6, 2010
 *     Author: fabricio.silva
 **********************************************************************/

#ifdef TEST_ENABLED

#include "Basic.h"
#include "src/manager_p.h"
#include "testtimer.h"
#include "functional_test_cases/test_functional.h"
#include "src/communication/communication.h"
#include "src/util/log.h"


int test_timer_init_suite(void)
{
	return functional_test_init();
}

int test_timer_finish_suite(void)
{
	return functional_test_finish();
}

void testtimer_add_suite(void)
{
	CU_pSuite suite = CU_add_suite("Test Timer Suite",
				       test_timer_init_suite, test_timer_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_timer", test_timer);

	/* Add tests here - End */

}

static int thread_modifiable_value = 0;
void testtimer_execute()
{
	DEBUG(" testtimer_execute: Changing thread_modifiable_value to 1");
	thread_modifiable_value = 1;
}

void test_timer(void)
{
	manager_start();

	thread_modifiable_value = 0;

	Context *ctx = func_ctx();
	communication_count_timeout(ctx, &testtimer_execute, 20);

	communication_reset_timeout(ctx);

	CU_ASSERT_EQUAL(thread_modifiable_value, 0);

	communication_count_timeout(ctx, &testtimer_execute, 2);

	communication_wait_for_timeout(ctx);
	CU_ASSERT_EQUAL(thread_modifiable_value, 1);

	communication_count_timeout(ctx, &testtimer_execute, 10);
	communication_count_timeout(ctx, &testtimer_execute, 9);
	communication_count_timeout(ctx, &testtimer_execute, 1);

	manager_stop();
}

#endif

