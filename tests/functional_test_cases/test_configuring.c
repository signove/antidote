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
 * test_configuring.c
 *
 * Created on: Jul 9, 2010
 *     Author: fabricio.silva
 **********************************************************************/
#ifdef TEST_ENABLED

#include "test_configuring.h"
#include "test_functional.h"
#include "src/manager_p.h"
#include "src/communication/communication.h"
#include "src/communication/fsm.h"
#include "src/communication/extconfigurations.h"

#include <stdio.h>
#include <stdlib.h>

#include <Basic.h>

int test_configuring_init_suite(void)
{
	return functional_test_init();
}

int test_configuring_finish_suite(void)
{
	return functional_test_finish();
}

void functionaltest_configuring_add_suite(void)
{
	CU_pSuite suite = CU_add_suite("Functional Tests - Configuration Module",
				       test_configuring_init_suite,
				       test_configuring_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "functional_test_configuring_tc_2_1",
		    functional_test_configuring_tc_2_1);

	CU_add_test(suite, "functional_test_configuring_tc_2_1_1",
		    functional_test_configuring_tc_2_1_1);

	CU_add_test(suite, "functional_test_configuring_tc_2_2",
		    functional_test_configuring_tc_2_2);

	CU_add_test(suite, "functional_test_configuring_tc_2_3",
		    functional_test_configuring_tc_2_3);

	CU_add_test(suite, "functional_test_configuring_tc_2_4",
		    functional_test_configuring_tc_2_4);

	CU_add_test(suite, "functional_test_configuring_tc_2_5",
		    functional_test_configuring_tc_2_5);

	CU_add_test(suite, "functional_test_configuring_tc_2_7",
		    functional_test_configuring_tc_2_7);

	/* Add tests here - End */
}

void functional_test_configuring_tc_2_1()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_blood_pressure_aarq);
	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, state);

	func_simulate_incoming_apdu(apdu_blood_pressure_roiv_mdc_noti_config);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	// TODO assert configuration is proper saved

	// Release request
	func_simulate_incoming_apdu(apdu_blood_pressure_rlrq);

	// send response
	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
}

void functional_test_configuring_tc_2_1_1()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_blood_pressure_aarq);
	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	func_simulate_incoming_apdu(apdu_blood_pressure_roiv_mds_note_scan_report_mp_fixed);

	// Release request
	func_simulate_incoming_apdu(apdu_blood_pressure_rlrq);

	// send response
	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
}

void functional_test_check_attributes_with_std_configuration_05DC()
{
	MDS *mds = func_mds();
	CU_ASSERT_EQUAL(mds->objects_list_count, 1);
	CU_ASSERT_EQUAL(mds->objects_list[0].obj_handle, 1);
	CU_ASSERT_EQUAL(mds->objects_list[0].choice, MDS_OBJ_METRIC);

	struct Metric_object obj = mds->objects_list[0].u.metric;
	CU_ASSERT_EQUAL(obj.choice, METRIC_NUMERIC);

	struct Metric metric = obj.u.numeric.metric;
	CU_ASSERT_EQUAL(metric.type.code, MDC_MASS_BODY_ACTUAL);
	CU_ASSERT_EQUAL(metric.type.partition, MDC_PART_SCADA);
	CU_ASSERT_EQUAL(metric.metric_spec_small, 0xF040);
	CU_ASSERT_EQUAL(metric.unit_code, MDC_DIM_KILO_G);

	CU_ASSERT_EQUAL(metric.attribute_value_map.count, 2);
	CU_ASSERT_EQUAL(metric.attribute_value_map.length, 8);
	CU_ASSERT_EQUAL(metric.attribute_value_map.value[0].attribute_id, MDC_ATTR_NU_VAL_OBS_SIMP);
	CU_ASSERT_EQUAL(metric.attribute_value_map.value[0].attribute_len, 4);
	CU_ASSERT_EQUAL(metric.attribute_value_map.value[1].attribute_id, MDC_ATTR_TIME_STAMP_ABS);
	CU_ASSERT_EQUAL(metric.attribute_value_map.value[1].attribute_len, 8);
}

void functional_test_configuring_tc_2_2()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, state);

	func_simulate_incoming_apdu(apdu_std_config_05DC_ws);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	functional_test_check_attributes_with_std_configuration_05DC();

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

}

void functional_test_configuring_tc_2_3()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, state);

	communication_wait_for_timeout(func_ctx());

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

}

void functional_test_check_attributes_with_ext_configuration_4000_ws()
{
	MDS *mds = func_mds();
	printf("\n\n%d\n\n", mds->objects_list_count);
	CU_ASSERT_EQUAL(mds->objects_list_count, 10);
	int i = 0;

	for (i = 0; i < 10; i++) {
		CU_ASSERT_EQUAL(mds->objects_list[i].obj_handle, i+1);
		CU_ASSERT_EQUAL(mds->objects_list[i].choice, MDS_OBJ_METRIC);

		struct Metric_object obj = mds->objects_list[0].u.metric;
		CU_ASSERT_EQUAL(obj.choice, METRIC_NUMERIC);
	}
}

void functional_test_configuring_tc_2_4()
{
	fsm_states state;

	ext_configurations_remove_all_configs();

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_assoc_req_ws);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, state);

	func_simulate_incoming_apdu(apdu_roiv_mdc_noti_config_ws);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	functional_test_check_attributes_with_ext_configuration_4000_ws();

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);


	// Second step

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_assoc_req_ws);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	functional_test_check_attributes_with_ext_configuration_4000_ws();

	manager_stop();

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

}


void functional_test_configuring_tc_2_5()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_05DC);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	functional_test_check_attributes_with_std_configuration_05DC();

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
}

void functional_test_configuring_tc_2_7()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_pedometer_association_request);
	func_simulate_incoming_apdu(apdu_pedometer_noti_config);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	manager_stop();
}

#endif
