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
 * test_pericfgscanner.h
 *
 * Created on: Jun 16, 2010
 *     Author: Jose Martins da Nobrega Filho
**********************************************************************/

#ifdef TEST_ENABLED

#include "test_pericfgscanner.h"
#include "test_functional.h"
#include "src/manager_p.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/util/bytelib.h"
#include "src/communication/communication.h"
#include "src/dim/mds.h"

#include <Basic.h>
#include <stdio.h>
#include <stdlib.h>

int test_pericfgscanner_init_suite(void)
{
	return functional_test_init();
}

int test_pericfgscanner_finish_suite(void)
{
	return functional_test_finish();
}

void functionaltest_pericfgscanner_add_suite()
{
	CU_pSuite suite = CU_add_suite("Functional Tests - Periodic Scanner Module",
				       test_pericfgscanner_init_suite,
				       test_pericfgscanner_finish_suite);

	/* Add tests here - Start */

	CU_add_test(suite, "functional_test_pericfgscanner_tc_6_1",
		    functional_test_pericfgscanner_tc_6_1);

	CU_add_test(suite, "functional_test_pericfgscanner_tc_6_2",
		    functional_test_pericfgscanner_tc_6_2);

	CU_add_test(suite, "functional_test_pericfgscanner_tc_6_3",
		    functional_test_pericfgscanner_tc_6_3);

	CU_add_test(suite, "functional_test_pericfgscanner_tc_6_4",
		    functional_test_pericfgscanner_tc_6_4);

	CU_add_test(suite, "functional_test_pericfgscanner_tc_6_5",
		    functional_test_pericfgscanner_tc_6_5);

	CU_add_test(suite, "functional_test_pericfgscanner_tc_6_6",
		    functional_test_pericfgscanner_tc_6_6);

	/* Add tests here - End */

}

void functional_test_pericfgscanner_tc_6_1_callback(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_enabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	func_simulate_incoming_apdu(apdu_pulse_oximeter_buf_scan_report_grouped);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	obj = mds_get_object_by_handle(func_mds(), 1);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 98, 0);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.measurement_status, 0);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.year, 0x08);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.month, 0x07);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.day, 0x03);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);

	obj = mds_get_object_by_handle(func_mds(), 10);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 73, 0.1);

	obj = mds_get_object_by_handle(func_mds(), 3);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 98, 0.1);

	// Disconnect and Disassociation
	func_simulate_incoming_apdu(apdu_blood_pressure_rlrq);
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	manager_stop();
	CU_ASSERT_EQUAL(fsm_state_disconnected, communication_get_state(func_ctx()));

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
}

void functional_test_pericfgscanner_tc_6_1()
{
	// Connect and Association
	manager_start();
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	func_simulate_incoming_apdu(apdu_pulse_oximeter_association_request);
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, communication_get_state(func_ctx()));

	func_simulate_incoming_apdu(apdu_pulse_oximeter_noti_config_with_scanner);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_disabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	Request *request = manager_set_operational_state_of_the_scanner(FUNC_TEST_SINGLE_CONTEXT, 0X0028, os_enabled,
			   functional_test_pericfgscanner_tc_6_1_callback);

	func_simulate_service_response(apdu_pulse_oximeter_periodic_set_response, request);
}

void functional_test_pericfgscanner_tc_6_2_callback()
{
	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_enabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	func_simulate_incoming_apdu(apdu_pulse_oximeter_buf_scan_report_fixed);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	obj = mds_get_object_by_handle(func_mds(), 1);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 98, 0.1);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);


	obj = mds_get_object_by_handle(func_mds(), 10);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 72, 0.1);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);

	// Disconnect and Disassociation
	func_simulate_incoming_apdu(apdu_blood_pressure_rlrq);
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	manager_stop();
	CU_ASSERT_EQUAL(fsm_state_disconnected, communication_get_state(func_ctx()));

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

}

void functional_test_pericfgscanner_tc_6_2()
{
	// Connect and Association
	manager_start();
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	func_simulate_incoming_apdu(apdu_pulse_oximeter_association_request);
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, communication_get_state(func_ctx()));

	func_simulate_incoming_apdu(apdu_pulse_oximeter_noti_config_with_scanner);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_disabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	Request *request = manager_set_operational_state_of_the_scanner(FUNC_TEST_SINGLE_CONTEXT, 0X0028, os_enabled,
			   functional_test_pericfgscanner_tc_6_2_callback);

	func_simulate_service_response(apdu_pulse_oximeter_periodic_set_response, request);
}

void functional_test_pericfgscanner_tc_6_3_callback()
{
	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_enabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	func_simulate_incoming_apdu(apdu_pulse_oximeter_buf_scan_report_var);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	obj = mds_get_object_by_handle(func_mds(), 1);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 98, 0.1);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);


	obj = mds_get_object_by_handle(func_mds(), 10);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 72, 0.1);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);

	// Disconnect and Disassociation
	func_simulate_incoming_apdu(apdu_blood_pressure_rlrq);
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	manager_stop();
	CU_ASSERT_EQUAL(fsm_state_disconnected, communication_get_state(func_ctx()));

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

}

void functional_test_pericfgscanner_tc_6_3()
{
	// Connect and Association
	manager_start();
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	func_simulate_incoming_apdu(apdu_pulse_oximeter_association_request);
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, communication_get_state(func_ctx()));

	func_simulate_incoming_apdu(apdu_pulse_oximeter_noti_config_with_scanner);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_disabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	Request *request = manager_set_operational_state_of_the_scanner(FUNC_TEST_SINGLE_CONTEXT, 0X0028, os_enabled,
			   functional_test_pericfgscanner_tc_6_3_callback);

	func_simulate_service_response(apdu_pulse_oximeter_periodic_set_response, request);
}

void functional_test_pericfgscanner_tc_6_4_callback()
{
	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_enabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	func_simulate_incoming_apdu(apdu_pulse_oximeter_buf_scan_report_mp_grouped);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	obj = mds_get_object_by_handle(func_mds(), 1);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 98, 0);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.measurement_status, 0);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.year, 0x08);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.month, 0x07);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.day, 0x03);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);

	obj = mds_get_object_by_handle(func_mds(), 10);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 73, 0.1);

	obj = mds_get_object_by_handle(func_mds(), 3);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 98, 0.1);

	// Disconnect and Disassociation
	func_simulate_incoming_apdu(apdu_blood_pressure_rlrq);
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	manager_stop();
	CU_ASSERT_EQUAL(fsm_state_disconnected, communication_get_state(func_ctx()));

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

}

void functional_test_pericfgscanner_tc_6_4()
{
	// Connect and Association
	manager_start();
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	func_simulate_incoming_apdu(apdu_pulse_oximeter_association_request);
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, communication_get_state(func_ctx()));

	func_simulate_incoming_apdu(apdu_pulse_oximeter_noti_config_with_scanner);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_disabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	Request *request = manager_set_operational_state_of_the_scanner(FUNC_TEST_SINGLE_CONTEXT, 0X0028, os_enabled,
			   functional_test_pericfgscanner_tc_6_4_callback);

	func_simulate_service_response(apdu_pulse_oximeter_periodic_set_response, request);
}

void functional_test_pericfgscanner_tc_6_5_callback()
{
	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_enabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	func_simulate_incoming_apdu(apdu_pulse_oximeter_buf_scan_report_mp_fixed);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	obj = mds_get_object_by_handle(func_mds(), 1);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 98, 0.1);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);


	obj = mds_get_object_by_handle(func_mds(), 10);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 72, 0.1);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);

	// Disconnect and Disassociation
	func_simulate_incoming_apdu(apdu_blood_pressure_rlrq);
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	manager_stop();
	CU_ASSERT_EQUAL(fsm_state_disconnected, communication_get_state(func_ctx()));

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));

}

void functional_test_pericfgscanner_tc_6_5()
{
	// Connect and Association
	manager_start();
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	func_simulate_incoming_apdu(apdu_pulse_oximeter_association_request);
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, communication_get_state(func_ctx()));

	func_simulate_incoming_apdu(apdu_pulse_oximeter_noti_config_with_scanner);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_disabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	Request *request = manager_set_operational_state_of_the_scanner(FUNC_TEST_SINGLE_CONTEXT, 0X0028, os_enabled,
			   functional_test_pericfgscanner_tc_6_5_callback);

	func_simulate_service_response(apdu_pulse_oximeter_periodic_set_response, request);
}


void functional_test_pericfgscanner_tc_6_6_callback()
{
	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_enabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	func_simulate_incoming_apdu(apdu_pulse_oximeter_buf_scan_report_mp_var);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	obj = mds_get_object_by_handle(func_mds(), 1);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 98, 0.1);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);


	obj = mds_get_object_by_handle(func_mds(), 10);
	CU_ASSERT_DOUBLE_EQUAL(obj->u.metric.u.numeric.basic_nu_observed_value, 72, 0.1);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.century, 0x20);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.year, 0x07);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.month, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.day, 0x06);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.hour, 0x12);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.minute, 0x10);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.second, 0x00);
	CU_ASSERT_EQUAL(obj->u.metric.u.numeric.metric.absolute_time_stamp.sec_fractions, 0x00);

	// Disconnect and Disassociation
	func_simulate_incoming_apdu(apdu_blood_pressure_rlrq);
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	manager_stop();
	CU_ASSERT_EQUAL(fsm_state_disconnected, communication_get_state(func_ctx()));

	printf(" Current machine state %s\n", communication_get_state_name(func_ctx()));
}


void functional_test_pericfgscanner_tc_6_6()
{
	// Connect and Association
	manager_start();
	CU_ASSERT_EQUAL(fsm_state_unassociated, communication_get_state(func_ctx()));
	func_simulate_incoming_apdu(apdu_pulse_oximeter_association_request);
	CU_ASSERT_EQUAL(fsm_state_waiting_for_config, communication_get_state(func_ctx()));

	func_simulate_incoming_apdu(apdu_pulse_oximeter_noti_config_with_scanner);
	CU_ASSERT_EQUAL(fsm_state_operating, communication_get_state(func_ctx()));

	struct MDS_object *obj = mds_get_object_by_handle(func_mds(), 40);
	CU_ASSERT_EQUAL(os_disabled, obj->u.scanner.u.peri_cfg_scanner.scanner.scanner.operational_state);

	Request *request = manager_set_operational_state_of_the_scanner(FUNC_TEST_SINGLE_CONTEXT, 0X0028, os_enabled,
			   functional_test_pericfgscanner_tc_6_6_callback);

	func_simulate_service_response(apdu_pulse_oximeter_periodic_set_response, request);
}

#endif /* TEST_ENABLED */
