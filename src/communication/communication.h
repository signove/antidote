/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file communication.h
 * \brief Communication module header.
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
 * \author Fabricio Silva
 * \date Jun 28, 2010
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <communication/fsm.h>
#include <communication/plugin/plugin.h>
#include <communication/context_manager.h>

struct mds_system_data;

/**
 * Agent role specialization
 */
extern ConfigId agent_specialization;

/**
 * Agent role event report callback
 */
extern void *(*agent_event_report_cb)();

/**
 * Agent role MDS system data fetch callback
 */
extern struct mds_system_data *(*agent_mds_data_cb)();

/**
 * ZERO Timeout implementation
 */
#define NO_TIMEOUT { .func = NULL , .timeout = 0 }


/**
 * Function called when after state transition occurs.
 *
 * @param previous state
 * @param next state
 */
typedef void (*communication_state_transition_handler_function)(Context *ctx, fsm_states previous, fsm_states next);

void communication_add_plugin(CommunicationPlugin *plugin);

unsigned int communication_plugin_id(CommunicationPlugin *plugin);

CommunicationPlugin *communication_get_plugin(unsigned int label);
int communication_is_trans(Context *ctx);

int communication_init();

int communication_finalize_thread_context(Context *ctx);

void communication_finalize();

int communication_add_state_transition_listener(
	fsm_states state,
	communication_state_transition_handler_function listener_function);

void communication_remove_all_state_transition_listeners();

void communication_network_start();

int communication_is_network_started();

int communication_network_stop();


Context *communication_transport_connect_indication(ContextId id);

void communication_transport_disconnect_indication(ContextId id);

void communication_lock(Context *ctx);

void communication_unlock(Context *ctx);

int communication_wait_for_data_input(Context *ctx);

void communication_read_input_stream(Context *ctx);

void communication_process_input_data(Context *ctx, ByteStreamReader *stream);

void communication_timeout(Context *ctx);

ByteStreamReader *communication_get_apdu_stream(Context *ctx);

void communication_fire_evt(Context *ctx, fsm_events evt, FSMEventData *data);

void communication_process_apdu(Context *ctx, APDU *apdu);

int communication_send_apdu(Context *ctx, APDU *apdu);

void communication_abort_undefined_reason_tx(Context *ctx, fsm_events evt,
		FSMEventData *data);

void communication_check_invoke_id_abort_tx(Context *ctx, fsm_events evt,
		FSMEventData *data);

void communication_disconnect_tx(Context *ctx, fsm_events evt,
				 FSMEventData *data);

void communication_send_roer(Context *ctx, InvokeIDType invoke_id, ErrorResult *error);

void communication_roer_tx(Context *ctx, fsm_events evt, FSMEventData *data);

int communication_is_roiv_type(DATA_apdu *data);

int communication_is_rors_type(DATA_apdu *data);

int communication_is_roer_type(DATA_apdu *data);

int communication_is_rorj_type(DATA_apdu *data);

fsm_states communication_get_state(Context *ctx);

char *communication_get_state_name(Context *ctx);

void communication_connection_loop(Context *ctx);

void communication_cancel_connection_loop();

int communication_count_timeout(Context *ctx, timer_callback_function func, intu32 timeout);

void communication_reset_timeout(Context *ctx);

void communication_wait_for_timeout(Context *ctx);

void req_association_release(Context *ctx);

int communication_force_disconnect(Context *ctx);

/**
 * @}
 */

#endif /* COMMUNICATION_H_ */
