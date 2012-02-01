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
 * test_association.h
 *
 * Created on: Jul 9, 2010
 *     Author: fabricio.silva
**********************************************************************/

#ifdef TEST_ENABLED

#ifndef TEST_FUNCTIONAL_H_
#define TEST_FUNCTIONAL_H_

#include "src/communication/service.h"
#include "src/dim/mds.h"

static const ContextId FUNC_TEST_SINGLE_CONTEXT = {1, 1};

#define apdu_blood_pressure_aarq "tests/resources/apdu/blood_pressure/aarq"
#define apdu_blood_pressure_aare_accept_unknown_conf "tests/resources/apdu/blood_pressure/aare_accept_unknown_conf"
#define apdu_blood_pressure_aare_accept_known_conf "tests/resources/apdu/blood_pressure/aare_accept_known_conf"
#define apdu_blood_pressure_roiv_mdc_noti_config "tests/resources/apdu/blood_pressure/roiv_mdc_noti_config"
#define apdu_blood_pressure_rors_accepted_config "tests/resources/apdu/blood_pressure/rors_accepted_config"
#define apdu_blood_pressure_roiv_mds_note_scan_report_mp_fixed "tests/resources/apdu/blood_pressure/roiv_mds_note_scan_report_mp_fixed"
#define apdu_blood_pressure_rlrq "tests/resources/apdu/blood_pressure/rlrq"
#define apdu_blood_pressure_rlre "tests/resources/apdu/blood_pressure/rlre"


#define apdu_H211 "tests/resources/apdu/H211"
#define apdu_H211_ID_0190 "tests/resources/apdu/H211_ID_0190"
#define apdu_H211_ID_02BC "tests/resources/apdu/H211_ID_02BC"
#define apdu_H211_ID_05DC "tests/resources/apdu/H211_ID_05DC"
#define apdu_H233_ID_02BC "tests/resources/apdu/H233_ID_02BC"
#define apdu_H233_ID_05DC "tests/resources/apdu/H233_ID_05DC"
#define apdu_H221 "tests/resources/apdu/H221"
#define apdu_H233 "tests/resources/apdu/H233"
#define apdu_H241 "tests/resources/apdu/H241"
#define apdu_H241_ID_02BC "tests/resources/apdu/H241_ID_02BC"
#define apdu_H241_ID_05DC "tests/resources/apdu/H241_ID_05DC"
#define apdu_H244 "tests/resources/apdu/H244"

#define apdu_H212 "tests/resources/apdu/H212"
#define apdu_H222 "tests/resources/apdu/H222"
#define apdu_H232 "tests/resources/apdu/H232"
#define apdu_H242 "tests/resources/apdu/H242"
#define apdu_H243 "tests/resources/apdu/H243"
#define test_data_apdu1_file "tests/resources/apdu/data_apdu1"
#define test_data_apdu2_file "tests/resources/apdu/data_apdu2"

#define apdu_rx_aare_example_one "tests/resources/apdu/rx_aare_example_one"
#define apdu_rx_abrt_buffer_overflow "tests/resources/apdu/rx_abrt_buffer_overflow"
#define apdu_rx_abrt_config_timeout "tests/resources/apdu/rx_abrt_config_timeout"
#define apdu_rx_abrt_response_timeout "tests/resources/apdu/rx_abrt_response_timeout"
#define apdu_rx_abrt_undefined "tests/resources/apdu/rx_abrt_undefined"
#define apdu_rx_rlre_normal "tests/resources/apdu/rx_rlre_normal"
#define apdu_rx_rlrq_normal "tests/resources/apdu/rx_rlrq_normal"
#define apdu_scan_report_info_var_BP "tests/resources/apdu/scan_report_info_var"

#define apdu_roiv_mdc_noti_config_ws "tests/resources/apdu/weighing_scale/roiv_mdc_noti_config"
#define apdu_assoc_req_ws "tests/resources/apdu/weighing_scale/association_request"
#define apdu_data_report_ws "tests/resources/apdu/weighing_scale/data_report"
#define apdu_std_config_05DC_ws "tests/resources/apdu/weighing_scale/std_config_05DC"

#define apdu_pedometer_noti_config "tests/resources/apdu/pedometer/pedometer_noti_config"
#define apdu_pedometer_association_request "tests/resources/apdu/pedometer/pedometer_association_request"

#define apdu_pulse_oximeter_noti_config "tests/resources/apdu/pulse_oximeter/pulse_oximeter_noti_config"
#define apdu_pulse_oximeter_noti_config_with_scanner "tests/resources/apdu/pulse_oximeter/pulse_oximeter_noti_config_with_scanner"
#define apdu_pulse_oximeter_association_request "tests/resources/apdu/pulse_oximeter/pulse_oximeter_association_request"
#define apdu_pulse_oximeter_episodic_set_response "tests/resources/apdu/pulse_oximeter/pulse_oximeter_episodic_set_response"
#define apdu_pulse_oximeter_periodic_set_response "tests/resources/apdu/pulse_oximeter/pulse_oximeter_periodic_set_response"

#define apdu_pulse_oximeter_unbuf_scan_report_grouped "tests/resources/apdu/pulse_oximeter/pulse_oximeter_unbuf_scan_report_grouped"
#define apdu_pulse_oximeter_unbuf_scan_report_mp_grouped "tests/resources/apdu/pulse_oximeter/pulse_oximeter_unbuf_scan_report_mp_grouped"
#define apdu_pulse_oximeter_unbuf_scan_report_fixed "tests/resources/apdu/pulse_oximeter/pulse_oximeter_unbuf_scan_report_fixed"
#define apdu_pulse_oximeter_unbuf_scan_report_mp_fixed "tests/resources/apdu/pulse_oximeter/pulse_oximeter_unbuf_scan_report_mp_fixed"
#define apdu_pulse_oximeter_unbuf_scan_report_var "tests/resources/apdu/pulse_oximeter/pulse_oximeter_unbuf_scan_report_var"
#define apdu_pulse_oximeter_unbuf_scan_report_mp_var "tests/resources/apdu/pulse_oximeter/pulse_oximeter_unbuf_scan_report_mp_var"

#define apdu_pulse_oximeter_buf_scan_report_grouped "tests/resources/apdu/pulse_oximeter/pulse_oximeter_buf_scan_report_grouped"
#define apdu_pulse_oximeter_buf_scan_report_mp_grouped "tests/resources/apdu/pulse_oximeter/pulse_oximeter_buf_scan_report_mp_grouped"
#define apdu_pulse_oximeter_buf_scan_report_fixed "tests/resources/apdu/pulse_oximeter/pulse_oximeter_buf_scan_report_fixed"
#define apdu_pulse_oximeter_buf_scan_report_mp_fixed "tests/resources/apdu/pulse_oximeter/pulse_oximeter_buf_scan_report_mp_fixed"
#define apdu_pulse_oximeter_buf_scan_report_var "tests/resources/apdu/pulse_oximeter/pulse_oximeter_buf_scan_report_var"
#define apdu_pulse_oximeter_buf_scan_report_mp_var "tests/resources/apdu/pulse_oximeter/pulse_oximeter_buf_scan_report_mp_var"

int functional_test_init();

int functional_test_finish();

void func_simulate_incoming_apdu(const char *apdu_file_path);

void func_simulate_service_response(const char *response_apdu_file, Request *request);

Context *func_ctx();

MDS *func_mds();


#endif /* TEST_FUNCTIONAL_H_ */
#endif
