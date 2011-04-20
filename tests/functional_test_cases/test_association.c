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
 * test_association.c
 *
 * Created on: Jul 9, 2010
 *     Author: fabricio.silva
 **********************************************************************/

#ifdef TEST_ENABLED

#include "test_association.h"
#include "test_functional.h"
#include "src/manager_p.h"
#include "src/communication/communication.h"
#include "src/communication/fsm.h"

#include <Basic.h>
#include <stdio.h>

int test_association_init_suite(void)
{
	return functional_test_init();
}

int test_association_finish_suite(void)
{
	return functional_test_finish();
}

void functionaltest_association_add_suite(void)
{
	CU_pSuite suite = CU_add_suite("Functional Tests - Association Module",
				       test_association_init_suite,
				       test_association_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "functional_test_association_tc_1_1",
		    functional_test_association_tc_1_1);
	CU_add_test(suite, "functional_test_association_tc_1_2",
		    functional_test_association_tc_1_2);
	CU_add_test(suite, "functional_test_association_tc_1_3",
		    functional_test_association_tc_1_3);
	CU_add_test(suite, "functional_test_association_tc_1_4",
		    functional_test_association_tc_1_4);
	CU_add_test(suite, "functional_test_association_tc_1_5",
		    functional_test_association_tc_1_5);
	CU_add_test(suite, "functional_test_association_tc_1_6",
		    functional_test_association_tc_1_6);
	CU_add_test(suite, "functional_test_association_tc_1_7",
		    functional_test_association_tc_1_7);
	CU_add_test(suite, "functional_test_association_tc_1_8",
		    functional_test_association_tc_1_8);
	CU_add_test(suite, "functional_test_association_tc_1_9",
		    functional_test_association_tc_1_9);
	CU_add_test(suite, "functional_test_association_tc_1_10",
		    functional_test_association_tc_1_10);
	CU_add_test(suite, "functional_test_association_tc_1_11",
		    functional_test_association_tc_1_11);
	CU_add_test(suite, "functional_test_association_tc_1_12",
		    functional_test_association_tc_1_12);

	/* Add tests here - End */
}

void functional_test_association_tc_1_1()
{
	fsm_states state;
	char *name;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, state);

	communication_wait_for_timeout(func_ctx());

	state = communication_get_state(func_ctx());
	name = communication_get_state_name(func_ctx());
	printf(" ==Current machine state %s\n", name);
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());

	CU_ASSERT_EQUAL(fsm_state_disconnected, state);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);
}

void functional_test_association_tc_1_2()
{
	fsm_states state;
	char *name;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_0190);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);

}

void functional_test_association_tc_1_3()
{
	fsm_states state;
	char *name;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_02BC);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);
	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_association_tc_1_4()
{
	fsm_states state;
	char *name;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_05DC);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);
	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_association_tc_1_5()
{
	fsm_states state;
	char *name;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_0190);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	func_simulate_incoming_apdu(apdu_rx_rlrq_normal);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_association_tc_1_6()
{
	fsm_states state;
	char *name;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_02BC);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	func_simulate_incoming_apdu(apdu_rx_rlrq_normal);
	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_association_tc_1_7()
{
	fsm_states state;
	char *name;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_05DC);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	func_simulate_incoming_apdu(apdu_rx_rlrq_normal);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}


void functional_test_association_tc_1_8()
{
	fsm_states state;
	char *name;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_0190);


	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	func_simulate_incoming_apdu(apdu_H211_ID_0190);

	name = communication_get_state_name(func_ctx());
	printf(" Current machine state %s\n", name);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_association_tc_1_9()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_0190);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	func_simulate_incoming_apdu(apdu_rx_abrt_undefined);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_association_tc_1_10()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_0190);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);


	func_simulate_incoming_apdu(apdu_rx_aare_example_one);
	// if receive aare - abort association

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_association_tc_1_11()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_0190);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	manager_request_association_release(FUNC_TEST_SINGLE_CONTEXT);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disassociating, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);

}

void functional_test_association_tc_1_12()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_0190);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	manager_request_association_abort(FUNC_TEST_SINGLE_CONTEXT);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

#endif
