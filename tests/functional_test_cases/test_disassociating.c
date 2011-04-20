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
 * Created on: Jul 21, 2010
 *     Author: marcos.pereira
 **********************************************************************/
#ifdef TEST_ENABLED

#include "test_disassociating.h"
#include "test_functional.h"
#include "src/manager_p.h"
#include "src/communication/communication.h"
#include "src/communication/fsm.h"

#include <Basic.h>

int test_disassociating_init_suite(void)
{
	return functional_test_init();
}

int test_disassociating_finish_suite(void)
{
	return functional_test_finish();
}

void functionaltest_disassociating_add_suite(void)
{
	CU_pSuite suite = CU_add_suite("Functional Tests - Disassociating Module",
				       test_disassociating_init_suite,
				       test_disassociating_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "functional_test_disassociating_tc_4_1",
		    functional_test_disassociating_tc_4_1);
	CU_add_test(suite, "functional_test_disassociating_tc_4_2",
		    functional_test_disassociating_tc_4_2);
	CU_add_test(suite, "functional_test_disassociating_tc_4_3",
		    functional_test_disassociating_tc_4_3);
	CU_add_test(suite, "functional_test_disassociating_tc_4_4",
		    functional_test_disassociating_tc_4_4);
	CU_add_test(suite, "functional_test_disassociating_tc_4_5",
		    functional_test_disassociating_tc_4_5);
	CU_add_test(suite, "functional_test_disassociating_tc_4_6",
		    functional_test_disassociating_tc_4_6);
	CU_add_test(suite, "functional_test_disassociating_tc_4_7",
		    functional_test_disassociating_tc_4_7);
	CU_add_test(suite, "functional_test_disassociating_tc_4_8",
		    functional_test_disassociating_tc_4_8);
	CU_add_test(suite, "functional_test_disassociating_tc_4_9",
		    functional_test_disassociating_tc_4_9);

	/* Add tests here - End */
}

void functional_test_disassociating_tc_4_1()
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

	manager_request_association_release(FUNC_TEST_SINGLE_CONTEXT);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disassociating, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_disassociating_tc_4_2()
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

	manager_request_association_abort(FUNC_TEST_SINGLE_CONTEXT);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_disassociating_tc_4_3()
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

	func_simulate_incoming_apdu(apdu_H211_ID_0190);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_disassociating_tc_4_4()
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

	func_simulate_incoming_apdu(apdu_rx_aare_example_one);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_disassociating_tc_4_5()
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

	func_simulate_incoming_apdu(apdu_rx_rlrq_normal);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disassociating, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_disassociating_tc_4_6()
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

	func_simulate_incoming_apdu(apdu_rx_rlre_normal);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_disassociating_tc_4_7()
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

	func_simulate_incoming_apdu(apdu_rx_abrt_undefined);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_disassociating_tc_4_8()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_02BC);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	manager_request_association_release(FUNC_TEST_SINGLE_CONTEXT);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disassociating, state);

	func_simulate_incoming_apdu(apdu_blood_pressure_roiv_mdc_noti_config);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disassociating, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void functional_test_disassociating_tc_4_9()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_02BC);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	manager_request_association_release(FUNC_TEST_SINGLE_CONTEXT);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disassociating, state);

	Request *request = manager_request_measurement_data_transmission(FUNC_TEST_SINGLE_CONTEXT, assert_tc_4_9);

	func_simulate_service_response(apdu_H244, request);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	service_del_request(request);
	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
}

void assert_tc_4_9()
{
}

#endif // TEST_ENABLED
