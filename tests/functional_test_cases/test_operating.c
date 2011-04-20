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
 * test_operating.c
 *
 * Created on: Jul 16, 2010
 *     Author: diego
**********************************************************************/

#ifdef TEST_ENABLED

#include "test_operating.h"
#include "test_functional.h"
#include "src/manager_p.h"
#include "src/communication/fsm.h"
#include "src/communication/communication.h"

#include <Basic.h>
#include <stdio.h>

int test_operating_init_suite(void)
{
	return functional_test_init();
}

int test_operating_finish_suite(void)
{
	return functional_test_finish();
}

void functionaltest_operating_add_suite(void)
{
	CU_pSuite suite = CU_add_suite("Functional Tests - Operating Module",
				       test_operating_init_suite,
				       test_operating_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "functional_test_operating_tc_3_1",
		    functional_test_operating_tc_3_1);

	CU_add_test(suite, "functional_test_operating_tc_3_2",
		    functional_test_operating_tc_3_2);

	CU_add_test(suite, "functional_test_operating_tc_3_3",
		    functional_test_operating_tc_3_3);

	CU_add_test(suite, "functional_test_operating_tc_3_4",
		    functional_test_operating_tc_3_4);

	CU_add_test(suite, "functional_test_operating_tc_3_5",
		    functional_test_operating_tc_3_5);

	CU_add_test(suite, "functional_test_operating_tc_3_6",
		    functional_test_operating_tc_3_6);

	CU_add_test(suite, "functional_test_operating_tc_3_7",
		    functional_test_operating_tc_3_7);

	/* Add tests here - End */
}

void assert_tc_3_1()
{
	MDS *mds = func_mds();
	CU_ASSERT_EQUAL(mds->system_type_spec_list.count, 1);
	CU_ASSERT_EQUAL(mds->system_type_spec_list.length, 4);
	CU_ASSERT_EQUAL(mds->system_type_spec_list.value[0].type, MDC_DEV_SPEC_PROFILE_BP);
	CU_ASSERT_EQUAL(mds->system_type_spec_list.value[0].version, 1);

	CU_ASSERT_EQUAL(mds->date_and_time.century, 0x20);
	CU_ASSERT_EQUAL(mds->date_and_time.year, 0x07);
	CU_ASSERT_EQUAL(mds->date_and_time.month, 0x02);
	CU_ASSERT_EQUAL(mds->date_and_time.day, 0x01);
	CU_ASSERT_EQUAL(mds->date_and_time.hour, 0x12);
	CU_ASSERT_EQUAL(mds->date_and_time.minute, 0x05);
	CU_ASSERT_EQUAL(mds->date_and_time.second, 0x00);
	CU_ASSERT_EQUAL(mds->date_and_time.sec_fractions, 0x00);

	const char *s1 = "TheCompany";
	const char *s2 = "TheBPM ABC";

	CU_ASSERT_NSTRING_EQUAL(s1, mds->system_model.manufacturer.value, 10);
	CU_ASSERT_NSTRING_EQUAL(s2, mds->system_model.model_number.value, 10);

	CU_ASSERT_EQUAL(mds->production_specification.count, 1);
	CU_ASSERT_EQUAL(mds->production_specification.length, 14);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].spec_type, 1);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].component_id, 0);

	const char *s3 = "DE124567";
	CU_ASSERT_NSTRING_EQUAL(s3, mds->production_specification.value[0].prod_spec.value, 8);
}

void assert_tc_3_2()
{
	MDS *mds = func_mds();

	func_simulate_incoming_apdu(apdu_scan_report_info_var_BP);

	CU_ASSERT_DOUBLE_EQUAL(mds->objects_list[0].u.metric.u.numeric.compound_basic_nu_observed_value.value[0],
			       120.000, 0.001);
	CU_ASSERT_DOUBLE_EQUAL(mds->objects_list[0].u.metric.u.numeric.compound_basic_nu_observed_value.value[1],
			       80.000, 0.001);
	CU_ASSERT_DOUBLE_EQUAL(mds->objects_list[0].u.metric.u.numeric.compound_basic_nu_observed_value.value[2],
			       100.000, 0.001);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);

	CU_ASSERT_DOUBLE_EQUAL(mds->objects_list[1].u.metric.u.numeric.basic_nu_observed_value,
			       60.0, 0.01);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);
}

void assert_tc_3_3()
{
	MDS *mds = func_mds();

	func_simulate_incoming_apdu(apdu_H241_ID_02BC);

	CU_ASSERT_DOUBLE_EQUAL(mds->objects_list[0].u.metric.u.numeric.compound_basic_nu_observed_value.value[0],
			       120.000, 0.001);
	CU_ASSERT_DOUBLE_EQUAL(mds->objects_list[0].u.metric.u.numeric.compound_basic_nu_observed_value.value[1],
			       80.000, 0.001);
	CU_ASSERT_DOUBLE_EQUAL(mds->objects_list[0].u.metric.u.numeric.compound_basic_nu_observed_value.value[2],
			       100.000, 0.001);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(mds->objects_list[0].u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);

	CU_ASSERT_DOUBLE_EQUAL(mds->objects_list[1].u.metric.u.numeric.basic_nu_observed_value,
			       60.0, 0.01);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(mds->objects_list[1].u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);
}

void assert_tc_3_5()
{
	MDS *mds = func_mds();

	func_simulate_incoming_apdu(apdu_data_report_ws);

	struct MDS_object *object =  mds_get_object_by_handle(mds, 1);

	CU_ASSERT_PTR_NOT_NULL(object);

	CU_ASSERT_DOUBLE_EQUAL(object->u.metric.u.numeric.simple_nu_observed_value, 79.6, 0.1);

	object =  mds_get_object_by_handle(mds, 3);

	CU_ASSERT_PTR_NOT_NULL(object);

	CU_ASSERT_DOUBLE_EQUAL(object->u.metric.u.numeric.simple_nu_observed_value, 27.2, 0.1);

	object =  mds_get_object_by_handle(mds, 2);
	CU_ASSERT_PTR_NOT_NULL(object);
	printf("Height: %.2f\n", object->u.metric.u.numeric.simple_nu_observed_value);

	object =  mds_get_object_by_handle(mds, 4);
	CU_ASSERT_PTR_NOT_NULL(object);
	printf("Body fat: %.2f\n", object->u.metric.u.numeric.simple_nu_observed_value);

	object =  mds_get_object_by_handle(mds, 5);
	CU_ASSERT_PTR_NOT_NULL(object);
	printf("Basal: %.2f\n", object->u.metric.u.numeric.simple_nu_observed_value);

	object =  mds_get_object_by_handle(mds, 6);
	CU_ASSERT_PTR_NOT_NULL(object);
	printf("Viceral: %.2f\n", object->u.metric.u.numeric.basic_nu_observed_value);

	object =  mds_get_object_by_handle(mds, 7);
	CU_ASSERT_PTR_NOT_NULL(object);
	printf("Body Age: %.2f\n", object->u.metric.u.numeric.basic_nu_observed_value);

	object =  mds_get_object_by_handle(mds, 8);
	CU_ASSERT_PTR_NOT_NULL(object);
	printf("Skeletal: %.2f\n", object->u.metric.u.numeric.compound_basic_nu_observed_value.value[0]);


	object =  mds_get_object_by_handle(mds, 10);
	CU_ASSERT_PTR_NOT_NULL(object);
	printf("Skeletal: %.2f\n", object->u.metric.u.numeric.compound_basic_nu_observed_value.value[0]);
}

void functional_test_operating_tc_3_1()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_02BC);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	printf(" Blood pressure associated\n");

	Request *request = manager_request_get_all_mds_attributes(FUNC_TEST_SINGLE_CONTEXT, assert_tc_3_1);

	func_simulate_service_response(apdu_H233_ID_02BC, request);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);

}

void functional_test_operating_tc_3_2()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_02BC);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	printf(" Blood pressure associated\n");

	Request *request = manager_request_get_all_mds_attributes(FUNC_TEST_SINGLE_CONTEXT, assert_tc_3_2);

	func_simulate_service_response(apdu_H233_ID_02BC, request);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);

}

void functional_test_operating_tc_3_3()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_02BC);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	printf(" Blood pressure associated\n");

	Request *request = manager_request_get_all_mds_attributes(FUNC_TEST_SINGLE_CONTEXT, assert_tc_3_3);

	func_simulate_service_response(apdu_H233_ID_02BC, request);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);

}

void functional_test_operating_tc_3_4_callback()
{
	MDS *mds = func_mds();
	CU_ASSERT_EQUAL(mds->system_type_spec_list.count, 1);
	CU_ASSERT_EQUAL(mds->system_type_spec_list.value[0].version, 1);
	CU_ASSERT_EQUAL(mds->system_type_spec_list.value[0].type, MDC_DEV_SPEC_PROFILE_SCALE);

	CU_ASSERT_EQUAL(mds->system_model.manufacturer.length, 10);
	CU_ASSERT_EQUAL(mds->system_model.manufacturer.value[0], 0x54);
	CU_ASSERT_EQUAL(mds->system_model.manufacturer.value[1], 0x68);
	CU_ASSERT_EQUAL(mds->system_model.manufacturer.value[2], 0x65);
	CU_ASSERT_EQUAL(mds->system_model.manufacturer.value[3], 0x43);
	CU_ASSERT_EQUAL(mds->system_model.manufacturer.value[4], 0x6F);
	CU_ASSERT_EQUAL(mds->system_model.manufacturer.value[5], 0x6D);
	CU_ASSERT_EQUAL(mds->system_model.manufacturer.value[6], 0x70);
	CU_ASSERT_EQUAL(mds->system_model.manufacturer.value[7], 0x61);
	CU_ASSERT_EQUAL(mds->system_model.manufacturer.value[8], 0x6E);
	CU_ASSERT_EQUAL(mds->system_model.manufacturer.value[9], 0x79);

	CU_ASSERT_EQUAL(mds->system_model.model_number.length, 12);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[0], 0x54);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[1], 0x68);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[2], 0x65);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[3], 0x53);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[4], 0x63);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[5], 0x61);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[6], 0x6C);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[7], 0x65);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[8], 0x41);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[9], 0x42);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[10], 0x43);
	CU_ASSERT_EQUAL(mds->system_model.model_number.value[11], 0x00);

	CU_ASSERT_EQUAL(mds->system_id.length, 8);
	CU_ASSERT_EQUAL(mds->system_id.value[0], 0x11);
	CU_ASSERT_EQUAL(mds->system_id.value[1], 0x22);
	CU_ASSERT_EQUAL(mds->system_id.value[2], 0x33);
	CU_ASSERT_EQUAL(mds->system_id.value[3], 0x44);
	CU_ASSERT_EQUAL(mds->system_id.value[4], 0x55);
	CU_ASSERT_EQUAL(mds->system_id.value[5], 0x66);
	CU_ASSERT_EQUAL(mds->system_id.value[6], 0x77);
	CU_ASSERT_EQUAL(mds->system_id.value[7], 0x88);

	CU_ASSERT_EQUAL(mds->dev_configuration_id, 0x05dc);

	CU_ASSERT_EQUAL(mds->production_specification.count, 1);
	CU_ASSERT_EQUAL(mds->production_specification.length, 14);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].spec_type, 1);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].component_id, 0);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].prod_spec.length, 8);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].prod_spec.value[0], 0x44);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].prod_spec.value[1], 0x45);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].prod_spec.value[2], 0x31);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].prod_spec.value[3], 0x32);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].prod_spec.value[4], 0x34);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].prod_spec.value[5], 0x35);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].prod_spec.value[6], 0x36);
	CU_ASSERT_EQUAL(mds->production_specification.value[0].prod_spec.value[7], 0x37);

	CU_ASSERT_EQUAL(mds->date_and_time.century, 0x20);
	CU_ASSERT_EQUAL(mds->date_and_time.year, 0x07);
	CU_ASSERT_EQUAL(mds->date_and_time.month, 0x02);
	CU_ASSERT_EQUAL(mds->date_and_time.day, 0x01);
	CU_ASSERT_EQUAL(mds->date_and_time.hour, 0x12);
	CU_ASSERT_EQUAL(mds->date_and_time.minute, 0x05);
	CU_ASSERT_EQUAL(mds->date_and_time.second, 0x00);
	CU_ASSERT_EQUAL(mds->date_and_time.sec_fractions, 0x00);
}

void functional_test_operating_tc_3_4()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_05DC);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	Request *request = manager_request_get_all_mds_attributes(FUNC_TEST_SINGLE_CONTEXT, functional_test_operating_tc_3_4_callback);

	func_simulate_service_response(apdu_H233_ID_05DC, request);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);
	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

}

void functional_test_operating_tc_3_5()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_assoc_req_ws);
	func_simulate_incoming_apdu(apdu_roiv_mdc_noti_config_ws);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	Request *request = manager_request_get_all_mds_attributes(FUNC_TEST_SINGLE_CONTEXT, assert_tc_3_5);

	func_simulate_service_response(apdu_H233_ID_05DC, request);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

}

void functional_test_operating_tc_3_6_callback()
{
	CU_FAIL("Manager fire Event: Configuration Timeout; Callback function should not be called");
}

void functional_test_operating_tc_3_6()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_H211_ID_05DC);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	manager_request_get_all_mds_attributes(FUNC_TEST_SINGLE_CONTEXT, functional_test_operating_tc_3_6_callback);

	communication_wait_for_timeout(func_ctx());

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	manager_stop();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_disconnected, state);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

}

void functional_test_operating_tc_3_7()
{
	fsm_states state;

	manager_start();

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_unassociated, state);

	func_simulate_incoming_apdu(apdu_pulse_oximeter_association_request);
	func_simulate_incoming_apdu(apdu_pulse_oximeter_noti_config);

	state = communication_get_state(func_ctx());
	CU_ASSERT_EQUAL(fsm_state_operating, state);

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
}

#endif
