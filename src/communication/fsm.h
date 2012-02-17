/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file fsm.h
 * \brief Finite State Machine header.
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
 * IEEE 11073 Communication Model - Finite State Machine implementation
 *
 * \author Fabricio Silva, Jose Martins
 * \date Jun 21, 2010
 */

/**
 * @addtogroup FSM
 * @{
 */
#ifndef FSM_H_
#define FSM_H_

#include "../asn1/phd_types.h"

/**
 *  Machine States
 */
typedef enum {
	/* Disconnected Group*/
	fsm_state_disconnected = 0,

	/* Connected Group */
	fsm_state_disassociating,
	fsm_state_unassociated,
	fsm_state_associating,
	fsm_state_config_sending,
	fsm_state_waiting_approval,
	fsm_state_operating,
	fsm_state_checking_config,
	fsm_state_waiting_for_config,

	fsm_state_size // !< The last value of states enumeration, it does not represent a real state
} fsm_states;

/**
 *  Machine Events
 */
typedef enum {
	// IND - Transport layer indications
	fsm_evt_ind_transport_connection = 0,
	fsm_evt_ind_transport_disconnect,
	fsm_evt_ind_timeout,

	// REQ - Manager requests
	fsm_evt_req_assoc_rel,
	fsm_evt_req_assoc_abort,
	fsm_evt_req_agent_supplied_unknown_configuration,
	fsm_evt_req_agent_supplied_known_configuration,

	// REQ - Agent requests
	fsm_evt_req_send_config,
	fsm_evt_req_send_event,
	fsm_evt_req_assoc,

	// Rx - Received APDU

	/**
	 * aarq(*)
	 */
	fsm_evt_rx_aarq,
	fsm_evt_rx_aarq_acceptable_and_known_configuration,
	fsm_evt_rx_aarq_acceptable_and_unknown_configuration,
	fsm_evt_rx_aarq_unacceptable_configuration,
	fsm_evt_rx_aare,
	fsm_evt_rx_aare_rejected,
	fsm_evt_rx_aare_accepted_known,
	fsm_evt_rx_aare_accepted_unknown,
	fsm_evt_rx_rlrq,
	fsm_evt_rx_rlre,
	fsm_evt_rx_abrt,
	fsm_evt_rx_prst,

	/**
	 * roiv-*
	 */
	fsm_evt_rx_roiv,
	fsm_evt_rx_roiv_event_report,
	fsm_evt_rx_roiv_confirmed_event_report,
	fsm_evt_rx_roiv_all_except_confirmed_event_report,
	fsm_evt_rx_roiv_get,
	fsm_evt_rx_roiv_set,
	fsm_evt_rx_roiv_confirmed_set,
	fsm_evt_rx_roiv_action,
	fsm_evt_rx_roiv_confirmed_action,

	/**
	 * rors-*
	 */
	fsm_evt_rx_rors,
	fsm_evt_rx_rors_confirmed_event_report,
	fsm_evt_rx_rors_confirmed_event_report_unknown,
	fsm_evt_rx_rors_confirmed_event_report_known,
	fsm_evt_rx_rors_get,
	fsm_evt_rx_rors_confirmed_set,
	fsm_evt_rx_rors_confirmed_action,

	fsm_evt_rx_roer,
	fsm_evt_rx_rorj,

	fsm_evt_size
} fsm_events;

/**
 * Event Data types
 */
typedef enum {
	FSM_EVT_DATA_ASSOCIATION_RESULT_CHOSEN = 1,
	FSM_EVT_DATA_RELEASE_REQUEST_REASON,
	FSM_EVT_DATA_RELEASE_RESPONSE_REASON,
	FSM_EVT_DATA_CONFIGURATION_RESULT,
	FSM_EVT_DATA_ERROR_RESULT,
	FSM_SVT_PHD_ASSOC_INFORMATION
} FSMEventData_choice_values;

/**
 * Event status
 */
typedef enum {
	FSM_PROCESS_EVT_RESULT_STATE_UNCHANGED = 0, // !<  Represent the result of event processing when machine state keeps the state
	FSM_PROCESS_EVT_RESULT_STATE_CHANGED, // !< Represent the result of event processing when machine go to new state
	FSM_PROCESS_EVT_RESULT_NOT_PROCESSED // !< Represent the result of event processing when could not process the event
} FSM_PROCESS_EVT_STATUS;


/**
 * State Machine Event Data
 */
typedef struct FSMEventData {

	/**
	 * Set when the incoming APDU is associated with the event
	 */
	APDU *received_apdu;
	/**
	 * Union type selected
	 */
	FSMEventData_choice_values choice;
	/**
	 * Possible data tokens, given the choice
	 */
	union {
		Associate_result association_result;
		Release_request_reason release_request_reason;
		Release_response_reason release_response_reason;
		ConfigResult configuration_result;
		ErrorResult error_result;
		PhdAssociationInformation assoc_information;
	} u;
} FSMEventData;

/**
 * Finite State Machine
 */
typedef struct FSM {
	/**
	 * Current machine state
	 */
	fsm_states state;

	/**
	 * Array of transition rules
	 */
	struct FsmTransitionRule *transition_table;

	/**
	 * State table size
	 */
	int32 transition_table_size;
} FSM;


/**
 * FSM typedef for Context
 */
typedef struct Context FSMContext;

/**
 *  Action to be executed during state transition
 *
 *  @param fsm state machine
 *  @param evt input event
 *  @param data event data
 */
typedef void (*fsm_action)(FSMContext *ctx, fsm_events evt, FSMEventData *data);

/**
 * State Machine Transition Rule
 */
typedef struct FsmTransitionRule {
	fsm_states currentState;
	fsm_events inputEvent;
	fsm_states nextState;
	fsm_action post_action;
} FsmTransitionRule;

FSM *fsm_instance();

void fsm_destroy(FSM *fsm);

void fsm_set_manager_state_table(FSM *fsm);
void fsm_set_agent_state_table(FSM *fsm);

void fsm_init(FSM *fsm, fsm_states entry_point_state, FsmTransitionRule *transition_table, int table_size);

FSM_PROCESS_EVT_STATUS  fsm_process_evt(FSMContext *ctx, fsm_events evt, FSMEventData *data);

char *fsm_get_current_state_name(FSM *fsm);

char *fsm_state_to_string(fsm_states state);

char *fsm_event_to_string(fsm_events evt);

/** @} */

#endif /* FSM_H_ */
