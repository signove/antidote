/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file fsm.c
 * \brief Finite State Machine source.
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
 * \author Fabricio Silva, Jose Martins
 * \date Jun 21, 2010
 */


/**
 * \defgroup FSM FSM
 * \brief Finite State Machine
 *
 * An overview of the manager state machine is shown in the below figure.
 * \image html fsm.png
 *
 * @{
 */

#include <stdlib.h>
#include <stdio.h>
#include "src/communication/fsm.h"
#include "src/communication/communication.h"
#include "src/communication/association.h"
#include "src/communication/disassociating.h"
#include "src/communication/configuring.h"
#include "src/communication/operating.h"
#include "src/communication/agent_ops.h"
#include "src/util/log.h"

static char *fsm_state_strings[] = {
	"disconnected",
	"disassociating",
	"unassociated",
	"associating",
	"config_sending",
	"waiting_approval",
	"operating",
	"checking_config",
	"waiting_for_config"
};


static char *fsm_event_strings[] = {
	"fsm_evt_ind_transport_connection",
	"fsm_evt_ind_transport_disconnect",
	"fsm_evt_ind_timeout",
	"fsm_evt_req_assoc_rel",
	"fsm_evt_req_assoc_abort",
	"fsm_evt_req_agent_supplied_unknown_configuration",
	"fsm_evt_req_agent_supplied_known_configuration",
	"fsm_evt_req_send_config",
	"fsm_evt_req_send_event",
	"fsm_evt_req_assoc",
	"fsm_evt_rx_aarq",
	"fsm_evt_rx_aarq_acceptable_and_known_configuration",
	"fsm_evt_rx_aarq_acceptable_and_unknown_configuration",
	"fsm_evt_rx_aarq_unacceptable_configuration",
	"fsm_evt_rx_aare",
	"fsm_evt_rx_aare_rejected",
	"fsm_evt_rx_aare_accepted_known",
	"fsm_evt_rx_aare_accepted_unknown",
	"fsm_evt_rx_rlrq",
	"fsm_evt_rx_rlre",
	"fsm_evt_rx_abrt",
	"fsm_evt_rx_prst",
	"fsm_evt_rx_roiv",
	"fsm_evt_rx_roiv_event_report",
	"fsm_evt_rx_roiv_confirmed_event_report",
	"fsm_evt_rx_roiv_all_except_confirmed_event_report",
	"fsm_evt_rx_roiv_get",
	"fsm_evt_rx_roiv_set",
	"fsm_evt_rx_roiv_confirmed_set",
	"fsm_evt_rx_roiv_action",
	"fsm_evt_rx_roiv_confirmed_action",
	"fsm_evt_rx_rors",
	"fsm_evt_rx_rors_confirmed_event_report",
	"fsm_evt_rx_rors_confirmed_event_report_unknown",
	"fsm_evt_rx_rors_confirmed_event_report_known",
	"fsm_evt_rx_rors_get",
	"fsm_evt_rx_rors_confirmed_set",
	"fsm_evt_rx_rors_confirmed_action",
	"fsm_evt_rx_roer",
	"fsm_evt_rx_rorj"
};

/**
 * IEEE 11073 Manager State Table
 */
static FsmTransitionRule IEEE11073_20601_manager_state_table[] = {
	{fsm_state_disconnected,	fsm_evt_ind_transport_connection,				fsm_state_unassociated,		NULL}, // 1.1
	{fsm_state_unassociated,	fsm_evt_ind_transport_disconnect,				fsm_state_disconnected,		&communication_disconnect_tx}, // 2.2
	{fsm_state_unassociated,	fsm_evt_req_assoc_rel,						fsm_state_unassociated,		NULL}, // 2.6
	{fsm_state_unassociated,	fsm_evt_req_assoc_abort,					fsm_state_unassociated,		NULL}, // 2.7
	// TODO: This function must return void
	{fsm_state_unassociated,	fsm_evt_rx_aarq_acceptable_and_known_configuration,		fsm_state_operating,		&association_accept_config_tx}, // 2.9
	{fsm_state_unassociated,	fsm_evt_rx_aarq_acceptable_and_unknown_configuration,		fsm_state_waiting_for_config,	&configuring_transition_waiting_for_config}, // 2.10

	{fsm_state_unassociated,	fsm_evt_rx_aarq_unacceptable_configuration,			fsm_state_unassociated,		&association_unaccept_config_tx}, // 2.11


	{fsm_state_unassociated,	fsm_evt_rx_aare,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 2.12
	{fsm_state_unassociated,	fsm_evt_rx_rlrq,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 2.16
	{fsm_state_unassociated,	fsm_evt_rx_rlre,						fsm_state_unassociated,		NULL}, // 2.17
	{fsm_state_unassociated,	fsm_evt_rx_abrt,						fsm_state_unassociated,		NULL}, // 2.18
	{fsm_state_unassociated,	fsm_evt_rx_prst,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 2.19


	{fsm_state_waiting_for_config,	fsm_evt_ind_transport_disconnect,				fsm_state_disconnected,		&communication_disconnect_tx}, // 6.2
	{fsm_state_waiting_for_config,	fsm_evt_ind_timeout,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 6.4
	{fsm_state_waiting_for_config,	fsm_evt_req_assoc_rel,						fsm_state_disassociating,	&configuring_association_release_request_tx}, // 6.6
	{fsm_state_waiting_for_config,	fsm_evt_req_assoc_abort,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 6.7
	{fsm_state_waiting_for_config,	fsm_evt_rx_aarq,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 6.8
	{fsm_state_waiting_for_config,	fsm_evt_rx_aare,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 6.12
	{fsm_state_waiting_for_config,	fsm_evt_rx_rlrq,						fsm_state_unassociated,		&disassociating_release_response_tx}, // 6.16
	{fsm_state_waiting_for_config,	fsm_evt_rx_rlre,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 6.17
	{fsm_state_waiting_for_config,	fsm_evt_rx_abrt,						fsm_state_unassociated,		NULL}, // 6.18
	{fsm_state_waiting_for_config,	fsm_evt_rx_roiv_confirmed_event_report,				fsm_state_checking_config,	&configuring_perform_configuration}, // 6.24
	{fsm_state_waiting_for_config,	fsm_evt_rx_roiv_event_report,					fsm_state_waiting_for_config,	&communication_roer_tx}, // 6.25
	{fsm_state_waiting_for_config,	fsm_evt_rx_roiv_get,						fsm_state_waiting_for_config,	NULL}, // 6.25
	{fsm_state_waiting_for_config,	fsm_evt_rx_roiv_set,						fsm_state_waiting_for_config,	NULL}, // 6.25
	{fsm_state_waiting_for_config,	fsm_evt_rx_roiv_confirmed_set,					fsm_state_waiting_for_config,	NULL}, // 6.25
	{fsm_state_waiting_for_config,	fsm_evt_rx_roiv_action,						fsm_state_waiting_for_config,	NULL}, // 6.25
	{fsm_state_waiting_for_config,	fsm_evt_rx_roiv_confirmed_action,				fsm_state_waiting_for_config,	NULL}, // 6.25
	{fsm_state_waiting_for_config,	fsm_evt_rx_rors,						fsm_state_waiting_for_config,	&communication_check_invoke_id_abort_tx}, // 6.26
	{fsm_state_waiting_for_config,	fsm_evt_rx_roer,						fsm_state_waiting_for_config,	NULL}, // 6.26 - remark on page 147
	{fsm_state_waiting_for_config,	fsm_evt_rx_rorj,						fsm_state_waiting_for_config,	NULL}, // 6.26 - remark on page 147
	{fsm_state_waiting_for_config,	fsm_evt_req_agent_supplied_unknown_configuration,		fsm_state_waiting_for_config,	NULL}, // transcoding
	{fsm_state_waiting_for_config,	fsm_evt_req_agent_supplied_known_configuration,			fsm_state_operating,		NULL}, // transcoding


	{fsm_state_checking_config,	fsm_evt_ind_transport_disconnect,				fsm_state_disconnected,		&communication_disconnect_tx}, // 7.2
	{fsm_state_checking_config,	fsm_evt_ind_timeout,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 7.4
	{fsm_state_checking_config,	fsm_evt_req_assoc_rel,						fsm_state_disassociating,	&disassociating_release_request_tx}, // 7.6
	{fsm_state_checking_config,	fsm_evt_req_assoc_abort,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 7.7
	{fsm_state_checking_config,	fsm_evt_rx_aarq_acceptable_and_known_configuration,		fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 7.8
	{fsm_state_checking_config,	fsm_evt_rx_aarq_acceptable_and_unknown_configuration,		fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 7.8
	{fsm_state_checking_config,	fsm_evt_rx_aarq_unacceptable_configuration,			fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 7.8
	{fsm_state_checking_config,	fsm_evt_rx_aare,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 7.12
	{fsm_state_checking_config,	fsm_evt_rx_rlrq,						fsm_state_unassociated,		&disassociating_release_response_tx}, // 7.16
	{fsm_state_checking_config,	fsm_evt_rx_rlre,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 7.17 - remark on page 148
	{fsm_state_checking_config,	fsm_evt_rx_abrt,						fsm_state_unassociated,		NULL}, // 7.18
	{fsm_state_checking_config,	fsm_evt_rx_roiv_confirmed_event_report,				fsm_state_checking_config,	&configuring_new_measurements_response_tx}, // 7.24
	{fsm_state_checking_config,	fsm_evt_rx_roiv_all_except_confirmed_event_report,		fsm_state_unassociated,		&communication_roer_tx}, // 7.25
	{fsm_state_checking_config,	fsm_evt_rx_rors_confirmed_event_report,				fsm_state_checking_config,	NULL}, // 7.26
	{fsm_state_checking_config,	fsm_evt_rx_rors_get,						fsm_state_checking_config,	NULL}, // 7.26
	{fsm_state_checking_config,	fsm_evt_rx_rors_confirmed_set,					fsm_state_checking_config,	NULL}, // 7.26
	{fsm_state_checking_config,	fsm_evt_rx_rors_confirmed_action,				fsm_state_checking_config,	NULL}, // 7.26
	{fsm_state_checking_config,	fsm_evt_rx_roer,						fsm_state_checking_config,	NULL}, // 7.26
	{fsm_state_checking_config,	fsm_evt_rx_rorj,						fsm_state_checking_config,	NULL}, // 7.26
	{fsm_state_checking_config,	fsm_evt_req_agent_supplied_unknown_configuration,		fsm_state_waiting_for_config,	&configuring_configuration_response_tx}, // 7.31
	{fsm_state_checking_config,	fsm_evt_req_agent_supplied_known_configuration,			fsm_state_operating,		&configuring_configuration_response_tx}, // 7.32


	{fsm_state_operating,		fsm_evt_ind_transport_disconnect,				fsm_state_disconnected,		NULL}, // 8.2
	{fsm_state_operating,		fsm_evt_ind_timeout,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 8.4
	{fsm_state_operating,		fsm_evt_req_assoc_rel,						fsm_state_disassociating,	&operating_assoc_release_req_tx}, // 8.6
	{fsm_state_operating,		fsm_evt_req_assoc_abort,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 8.7
	{fsm_state_operating,		fsm_evt_rx_aarq,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 8.8
	{fsm_state_operating,		fsm_evt_rx_aare,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 8.12
	{fsm_state_operating,		fsm_evt_rx_rlrq,						fsm_state_unassociated,		&disassociating_release_response_tx}, // 8.16
	{fsm_state_operating,		fsm_evt_rx_rlre,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 8.17
	{fsm_state_operating,		fsm_evt_rx_abrt,						fsm_state_unassociated,		NULL}, // 8.18
	{fsm_state_operating,		fsm_evt_rx_roiv_confirmed_event_report,				fsm_state_operating,		&operating_event_report}, // 8.21
	{fsm_state_operating,		fsm_evt_rx_roiv_event_report,					fsm_state_operating,		&operating_event_report}, // 8.21
	{fsm_state_operating,		fsm_evt_rx_rors_confirmed_event_report,				fsm_state_operating,		NULL}, // 8.26 - remark on page 149
	{fsm_state_operating,		fsm_evt_rx_rors_get,						fsm_state_operating,		&operating_get_response}, // 8.26 - remark on page 149
	{fsm_state_operating,		fsm_evt_rx_rors_confirmed_set,					fsm_state_operating,		&operating_set_scanner_response}, // 8.26 - remark on page 149
	{fsm_state_operating,		fsm_evt_rx_rors_confirmed_action,				fsm_state_operating,		&operating_rors_confirmed_action_tx}, // 8.26 - remark on page 149
	{fsm_state_operating,		fsm_evt_rx_roer,						fsm_state_operating,		NULL}, // 8.26 - remark on page 149
	{fsm_state_operating,		fsm_evt_rx_rorj,						fsm_state_operating,		NULL}, // 8.26 - remark on page 149


	{fsm_state_disassociating,	fsm_evt_ind_transport_disconnect,				fsm_state_disconnected,		NULL}, // 9.2
	{fsm_state_disassociating,	fsm_evt_ind_timeout,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.4
	{fsm_state_disassociating,	fsm_evt_req_assoc_rel,						fsm_state_disassociating,	NULL}, // 9.6
	{fsm_state_disassociating,	fsm_evt_req_assoc_abort,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.7
	{fsm_state_disassociating,	fsm_evt_rx_aarq,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.8
	{fsm_state_disassociating,	fsm_evt_rx_aare,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.12
	{fsm_state_disassociating,	fsm_evt_rx_rlrq,						fsm_state_disassociating,	&disassociating_release_response_tx}, // 9.16
	{fsm_state_disassociating,	fsm_evt_rx_rlre,						fsm_state_unassociated,		&disassociating_release_proccess_completed}, // 9.17
	{fsm_state_disassociating,	fsm_evt_rx_abrt,						fsm_state_unassociated,		NULL}, // 9.18
	{fsm_state_disassociating,	fsm_evt_rx_roiv,						fsm_state_disassociating,	NULL}, // 9.21
	{fsm_state_disassociating,	fsm_evt_rx_rors,						fsm_state_unassociated,		&communication_check_invoke_id_abort_tx}, // 9.26 - remark on page 150
	{fsm_state_disassociating,	fsm_evt_rx_roer,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26 - remark on page 150
	{fsm_state_disassociating,	fsm_evt_rx_rorj,						fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26 - remark on page 150

};


/**
 * IEEE 11073 Agent State Table
 */
static FsmTransitionRule IEEE11073_20601_agent_state_table[] = {
	// current			input							next				post_action
	{fsm_state_disconnected,	fsm_evt_ind_transport_connection,			fsm_state_unassociated,		&association_agent_mds}, // 1.1
	{fsm_state_unassociated,	fsm_evt_ind_transport_disconnect,			fsm_state_disconnected,		&communication_disconnect_tx}, // 2.2
	{fsm_state_unassociated,	fsm_evt_req_assoc,					fsm_state_associating,		&association_aarq_tx}, // 2.5
	{fsm_state_unassociated,	fsm_evt_req_assoc_rel,					fsm_state_unassociated,		NULL}, // 2.6
	{fsm_state_unassociated,	fsm_evt_req_assoc_abort,				fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 2.7
	{fsm_state_unassociated,	fsm_evt_rx_aarq,					fsm_state_unassociated,		&association_agent_aare_rejected_permanent_tx}, // 2.8
	{fsm_state_unassociated,	fsm_evt_rx_aare,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 2.12
	{fsm_state_unassociated,	fsm_evt_rx_rlrq,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 2.16
	{fsm_state_unassociated,	fsm_evt_rx_rlre,					fsm_state_unassociated,		NULL}, // 2.17
	{fsm_state_unassociated,	fsm_evt_rx_abrt,					fsm_state_unassociated,		NULL}, // 2.18
	{fsm_state_unassociated,	fsm_evt_rx_prst,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 2.19
	{fsm_state_associating,		fsm_evt_ind_transport_disconnect,			fsm_state_disconnected,		NULL}, // 3.2
	{fsm_state_associating,		fsm_evt_ind_timeout,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 3.4
	{fsm_state_associating,		fsm_evt_req_assoc_rel,					fsm_state_unassociated,		&disassociating_release_request_normal_tx}, // 3.6
	{fsm_state_associating,		fsm_evt_req_assoc_abort,				fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 3.7
	{fsm_state_associating,		fsm_evt_rx_aarq,					fsm_state_unassociated,		&association_agent_aare_rejected_permanent_tx}, // 3.8
	{fsm_state_associating,		fsm_evt_rx_aare_accepted_known,				fsm_state_operating,		NULL}, // 3.13
	{fsm_state_associating,		fsm_evt_rx_aare_accepted_unknown,			fsm_state_config_sending,	NULL}, // 3.14
	{fsm_state_associating,		fsm_evt_rx_aare_rejected,				fsm_state_unassociated,		NULL}, // 3.15
	{fsm_state_associating,		fsm_evt_rx_rlrq,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 3.16
	{fsm_state_associating,		fsm_evt_rx_rlre,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 3.17
	{fsm_state_associating,		fsm_evt_rx_abrt,					fsm_state_unassociated,		NULL}, // 3.18
	{fsm_state_associating,		fsm_evt_rx_prst,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 3.19
	{fsm_state_config_sending,	fsm_evt_ind_transport_disconnect,			fsm_state_disconnected,		NULL}, // 4.2
	{fsm_state_config_sending,	fsm_evt_ind_timeout,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.4
	{fsm_state_config_sending,	fsm_evt_req_assoc_rel,					fsm_state_disassociating,	&disassociating_release_request_normal_tx}, // 4.6
	{fsm_state_config_sending,	fsm_evt_req_assoc_abort,				fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.7
	{fsm_state_config_sending,	fsm_evt_rx_aarq,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.8
	{fsm_state_config_sending,	fsm_evt_rx_aare,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.12
	{fsm_state_config_sending,	fsm_evt_rx_rlrq,					fsm_state_unassociated,		&disassociating_release_response_tx_normal}, // 4.16
	{fsm_state_config_sending,	fsm_evt_rx_rlre,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.17
	{fsm_state_config_sending,	fsm_evt_rx_abrt,					fsm_state_unassociated,		NULL}, // 4.18
	{fsm_state_config_sending,	fsm_evt_rx_roiv_get,					fsm_state_config_sending,	&communication_agent_roiv_get_mds_tx}, // 4.22
	{fsm_state_config_sending,	fsm_evt_rx_roiv,					fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 4.23
	{fsm_state_config_sending,	fsm_evt_rx_roiv_event_report,				fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 4.23
	{fsm_state_config_sending,	fsm_evt_rx_roiv_confirmed_event_report,			fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 4.23
	{fsm_state_config_sending,	fsm_evt_rx_roiv_set,					fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 4.23
	{fsm_state_config_sending,	fsm_evt_rx_roiv_confirmed_set,				fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 4.23
	{fsm_state_config_sending,	fsm_evt_rx_roiv_action,					fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 4.23
	{fsm_state_config_sending,	fsm_evt_rx_roiv_confirmed_action,			fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 4.23
	{fsm_state_config_sending,	fsm_evt_rx_rors,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.26
	{fsm_state_config_sending,	fsm_evt_rx_rors_confirmed_event_report,			fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.26
	{fsm_state_config_sending,	fsm_evt_rx_rors_confirmed_event_report_unknown,		fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.26
	{fsm_state_config_sending,	fsm_evt_rx_rors_confirmed_event_report_known,		fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.26
	{fsm_state_config_sending,	fsm_evt_rx_rors_get,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.26
	{fsm_state_config_sending,	fsm_evt_rx_rors_confirmed_set,				fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.26
	{fsm_state_config_sending,	fsm_evt_rx_rors_confirmed_action,			fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.26
	{fsm_state_config_sending,	fsm_evt_rx_roer,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.26
	{fsm_state_config_sending,	fsm_evt_rx_rorj,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 4.26
	{fsm_state_config_sending,	fsm_evt_req_send_config,				fsm_state_waiting_approval,	&configuring_send_config_tx}, // 4.32
	{fsm_state_waiting_approval,	fsm_evt_ind_transport_disconnect,			fsm_state_disconnected,		NULL}, // 5.2
	{fsm_state_waiting_approval,	fsm_evt_ind_timeout,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.4
	{fsm_state_waiting_approval,	fsm_evt_req_assoc_rel,					fsm_state_disassociating,	&disassociating_release_request_normal_tx}, // 5.6
	{fsm_state_waiting_approval,	fsm_evt_req_assoc_abort,				fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.7
	{fsm_state_waiting_approval,	fsm_evt_rx_aarq,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.8
	{fsm_state_waiting_approval,	fsm_evt_rx_aare,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.12
	{fsm_state_waiting_approval,	fsm_evt_rx_rlrq,					fsm_state_unassociated,		&disassociating_release_response_tx_normal}, // 5.16
	{fsm_state_waiting_approval,	fsm_evt_rx_rlre,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.17
	{fsm_state_waiting_approval,	fsm_evt_rx_abrt,					fsm_state_unassociated,		NULL}, // 5.18
	{fsm_state_waiting_approval,	fsm_evt_rx_roiv_get,					fsm_state_config_sending,	&communication_agent_roiv_get_mds_tx}, // 5.22
	{fsm_state_waiting_approval,	fsm_evt_rx_roiv,					fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 5.23
	{fsm_state_waiting_approval,	fsm_evt_rx_roiv_event_report,				fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 5.23
	{fsm_state_waiting_approval,	fsm_evt_rx_roiv_confirmed_event_report,			fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 5.23
	{fsm_state_waiting_approval,	fsm_evt_rx_roiv_set,					fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 5.23
	{fsm_state_waiting_approval,	fsm_evt_rx_roiv_confirmed_set,				fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 5.23
	{fsm_state_waiting_approval,	fsm_evt_rx_roiv_action,					fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 5.23
	{fsm_state_waiting_approval,	fsm_evt_rx_roiv_confirmed_action,			fsm_state_config_sending,	&communication_agent_roer_no_tx}, // 5.23
	{fsm_state_waiting_approval,	fsm_evt_rx_rors_confirmed_event_report_unknown,		fsm_state_config_sending,	NULL}, // 5.27
	{fsm_state_waiting_approval,	fsm_evt_rx_rors_confirmed_event_report_known,		fsm_state_operating,		NULL}, // 5.29
	{fsm_state_waiting_approval,	fsm_evt_rx_rors,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.30
	{fsm_state_waiting_approval,	fsm_evt_rx_rors_confirmed_event_report,			fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.30
	{fsm_state_waiting_approval,	fsm_evt_rx_rors_get,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.30
	{fsm_state_waiting_approval,	fsm_evt_rx_rors_confirmed_set,				fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.30
	{fsm_state_waiting_approval,	fsm_evt_rx_rors_confirmed_action,			fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.30
	{fsm_state_waiting_approval,	fsm_evt_rx_roer,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.30
	{fsm_state_waiting_approval,	fsm_evt_rx_rorj,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 5.30
	{fsm_state_operating,		fsm_evt_ind_transport_disconnect,			fsm_state_disconnected,		NULL}, // 8.2
	{fsm_state_operating,		fsm_evt_ind_timeout,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 8.4
	{fsm_state_operating,		fsm_evt_req_assoc_rel,					fsm_state_disassociating,	&disassociating_release_request_normal_tx}, // 8.6

	{fsm_state_operating,		fsm_evt_req_assoc_abort,				fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 8.7
	{fsm_state_operating,		fsm_evt_req_send_event,					fsm_state_operating,		&communication_agent_send_event_tx}, // 8.7
	{fsm_state_operating,		fsm_evt_rx_aarq,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 8.8
	{fsm_state_operating,		fsm_evt_rx_aare,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 8.12
	{fsm_state_operating,		fsm_evt_rx_rlrq,					fsm_state_unassociated,		&disassociating_release_response_tx_normal}, // 8.16
	{fsm_state_operating,		fsm_evt_rx_rlre,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 8.17
	{fsm_state_operating,		fsm_evt_rx_abrt,					fsm_state_unassociated,		NULL}, // 8.18
	{fsm_state_operating,		fsm_evt_rx_roiv,					fsm_state_operating,		&communication_agent_roiv_respond_tx}, // 8.21
	{fsm_state_operating,		fsm_evt_rx_roiv_confirmed_event_report,			fsm_state_operating,		&communication_agent_roiv_confirmed_error_report_respond_tx}, // 8.21
	{fsm_state_operating,		fsm_evt_rx_roiv_get,					fsm_state_operating,		&communication_agent_roiv_get_mds_tx}, // 8.21
	{fsm_state_operating,		fsm_evt_rx_roiv_set,					fsm_state_operating,		&communication_agent_roiv_set_respond_tx}, // 8.21
	{fsm_state_operating,		fsm_evt_rx_roiv_confirmed_set,				fsm_state_operating,		&communication_agent_roiv_confirmed_set_respond_tx}, // 8.21
	{fsm_state_operating,		fsm_evt_rx_roiv_confirmed_action,			fsm_state_operating,		&communication_agent_roiv_confirmed_action_respond_tx}, // 8.21
	{fsm_state_operating,		fsm_evt_rx_roiv_action,					fsm_state_operating,		&communication_agent_roiv_action_respond_tx}, // 8.21
	{fsm_state_operating,		fsm_evt_rx_rors,					fsm_state_operating,		NULL}, // 8.26
	{fsm_state_operating,		fsm_evt_rx_rors_confirmed_event_report,			fsm_state_operating,		NULL}, // 8.26
	{fsm_state_operating,		fsm_evt_rx_rors_confirmed_event_report_unknown,		fsm_state_operating,		NULL}, // 8.26
	{fsm_state_operating,		fsm_evt_rx_rors_confirmed_event_report_known,		fsm_state_operating,		NULL}, // 8.26
	{fsm_state_operating,		fsm_evt_rx_rors_get,					fsm_state_operating,		NULL}, // 8.26
	{fsm_state_operating,		fsm_evt_rx_rors_confirmed_set,				fsm_state_operating,		NULL}, // 8.26
	{fsm_state_operating,		fsm_evt_rx_rors_confirmed_action,			fsm_state_operating,		NULL}, // 8.26
	{fsm_state_operating,		fsm_evt_rx_roer,					fsm_state_operating,		NULL}, // 8.26
	{fsm_state_operating,		fsm_evt_rx_rorj,					fsm_state_operating,		NULL}, // 8.26
	{fsm_state_disassociating,	fsm_evt_ind_transport_disconnect,			fsm_state_disconnected,		NULL}, // 9.2
	{fsm_state_disassociating,	fsm_evt_ind_timeout,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.4
	{fsm_state_disassociating,	fsm_evt_req_assoc_rel,					fsm_state_disassociating,	NULL}, // 9.6
	{fsm_state_disassociating,	fsm_evt_req_assoc_abort,				fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.7
	{fsm_state_disassociating,	fsm_evt_rx_aarq,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.8
	{fsm_state_disassociating,	fsm_evt_rx_aare,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.12
	{fsm_state_disassociating,	fsm_evt_rx_rlrq,					fsm_state_disassociating,	&disassociating_release_response_tx_normal}, // 9.16
	{fsm_state_disassociating,	fsm_evt_rx_rlre,					fsm_state_unassociated,		NULL}, // 9.17
	{fsm_state_disassociating,	fsm_evt_rx_abrt,					fsm_state_unassociated,		NULL}, // 9.18
	{fsm_state_disassociating,	fsm_evt_rx_roiv,					fsm_state_disassociating,	NULL}, // 9.21
	{fsm_state_disassociating,	fsm_evt_rx_rors,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26
	{fsm_state_disassociating,	fsm_evt_rx_rors_confirmed_event_report,			fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26
	{fsm_state_disassociating,	fsm_evt_rx_rors_confirmed_event_report_unknown,		fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26
	{fsm_state_disassociating,	fsm_evt_rx_rors_confirmed_event_report_known,		fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26
	{fsm_state_disassociating,	fsm_evt_rx_rors_get,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26
	{fsm_state_disassociating,	fsm_evt_rx_rors_confirmed_set,				fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26
	{fsm_state_disassociating,	fsm_evt_rx_rors_confirmed_action,			fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26
	{fsm_state_disassociating,	fsm_evt_rx_roer,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26
	{fsm_state_disassociating,	fsm_evt_rx_rorj,					fsm_state_unassociated,		&communication_abort_undefined_reason_tx}, // 9.26
	};


/**
 * Construct the state machine
 * @return finite state machine
 */
FSM *fsm_instance()
{
	FSM *fsm = malloc(sizeof(struct FSM));
	return fsm;
}

/**
 * Destroy state machine Deallocate the memory pointed by *fsm
 *
 * @param fsm
 */
void fsm_destroy(FSM *fsm)
{
	free(fsm);
}

/**
 * Initialize fsm with the states and transition rules of
 * IEEE 11073-20601 for Manager
 *
 * @param fsm
 */
void fsm_set_manager_state_table(FSM *fsm)
{
	int transition_table_size = sizeof(IEEE11073_20601_manager_state_table);
	int trasition_rule_size = sizeof(FsmTransitionRule);
	int table_size = transition_table_size / trasition_rule_size;
	fsm_init(fsm, fsm_state_disconnected, IEEE11073_20601_manager_state_table,
								table_size);
}

/**
 * Initialize fsm with the states and transition rules of
 * IEEE 11073-20601 for Agent
 *
 * @param fsm
 */
void fsm_set_agent_state_table(FSM *fsm)
{
	int transition_table_size = sizeof(IEEE11073_20601_agent_state_table);
	int trasition_rule_size = sizeof(FsmTransitionRule);
	int table_size = transition_table_size / trasition_rule_size;
	fsm_init(fsm, fsm_state_disconnected, IEEE11073_20601_agent_state_table,
								table_size);
}

/**
 * Initialize the state machine before process the inputs
 *
 * @param fsm state machine
 * @param entry_point_state the initial state of FSM
 * @param transition_table the transition rules table
 * @param table_size size of transition table array
 */
void fsm_init(FSM *fsm, fsm_states entry_point_state, FsmTransitionRule *transition_table, int table_size)
{
	// Define entry point state
	fsm->state = entry_point_state;
	/* Initialize Transition Rules */
	fsm->transition_table = transition_table;
	fsm->transition_table_size = table_size;
}

/**
 * Process the input events applying the transition rules, e.g.:
 * transport indications (IND), APDU arrival/departure (Rx/Tx),
 * manager requests (REQ).
 *
 * @param ctx connection context
 * @param evt event
 * @param data event data
 *
 * @return FSM_PROCESS_EVT_RESULT_STATE_CHANGED if the state has been changed,
 *  FSM_PROCESS_EVT_RESULT_STATE_UNCHANGED otherwise
 */
FSM_PROCESS_EVT_STATUS fsm_process_evt(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	FSM *fsm = ctx->fsm;

	DEBUG(" state machine(<%s>): process event <%s> ", fsm_state_to_string(fsm->state), fsm_event_to_string(evt));

	int i;
	FsmTransitionRule *rule = NULL;

	for (i = 0; i < fsm->transition_table_size; i++) {
		rule = &fsm->transition_table[i];

		if (fsm->state == rule->currentState && evt == rule->inputEvent) {
			int state_changed = fsm->state != rule->nextState;

			// pre-action


			// Make transition
			DEBUG(" state machine(<%s>): transition to <%s> ",
				fsm_state_to_string(fsm->state), fsm_state_to_string(rule->nextState));


			fsm->state = rule->nextState;

			if (rule->post_action != NULL) {
				// pos-action
				(rule->post_action)(ctx, evt, data);
			}

			if (state_changed) {
				return FSM_PROCESS_EVT_RESULT_STATE_CHANGED;
			} else {
				return FSM_PROCESS_EVT_RESULT_STATE_UNCHANGED;
			}

			break;
		}
	}

	fflush(stdout);

	return FSM_PROCESS_EVT_RESULT_NOT_PROCESSED;
}

/**
 * Gets current state name
 * @param fsm State machine
 * @return the name of the state
 */
char *fsm_get_current_state_name(FSM *fsm)
{
	return fsm_state_to_string(fsm->state);
}

/**
 * Convert state code to text representation
 * @param state State machine's state value
 * @return string representation of sate
 */
char *fsm_state_to_string(fsm_states state)
{
	if (state >= fsm_state_disconnected && state < fsm_state_size) {
		return fsm_state_strings[state];
	}

	return "not recognized";
}

/**
 * Convert event code to text representation
 * @param evt State machine event
 * @return string representation of event
 */
char *fsm_event_to_string(fsm_events evt)
{

	if (evt >= fsm_evt_ind_transport_connection && evt < fsm_evt_size) {
		return fsm_event_strings[evt];
	}

	return "not recognized";
}

/** @} */
