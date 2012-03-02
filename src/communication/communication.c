/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file communication.c
 * \brief Communication module implementation.
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

/**
 * \defgroup Communication Communication
 * \brief Communication module.
 * \ingroup FSM
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include "src/manager_p.h"
#include "src/agent_p.h"
#include "src/trans/trans.h"
#include "src/communication/context_manager.h"
#include "src/communication/communication.h"
#include "src/communication/association.h"
#include "src/communication/operating.h"
#include "src/communication/configuring.h"
#include "src/communication/disassociating.h"
#include "src/communication/plugin/plugin.h"
#include "src/communication/plugin/plugin_fifo.h"
#include "src/communication/plugin/plugin_tcp.h"
#include "src/communication/service.h"
#include "src/util/bytelib.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/util/log.h"

// #define APDU_DUMP

#ifdef APDU_DUMP
#include <util/ioutil.h>
#endif

/**
 * Represents the network layer status
 */
typedef enum {
	NETWORK_STATUS_NOT_INITIALIZED = 0,
	NETWORK_STATUS_INITIALIZED
} communication_network_status;

/**
 * Indicates if the network has been initialized.
 */
static communication_network_status network_status;


/**
 * CommunicationPlugin connection abstraction.
 */
static unsigned int plugin_count = 0;
static CommunicationPlugin **comm_plugins = NULL;

/**
 * Mask to remote operation (ro*) type checking
 */
#define REMOTE_OPERATION_TYPE_MASK 0x0F00

/**
 * Remote operation invoke message type (roiv-*)
 */
static const int ROIV_TYPE = ROIV_CMIP_ACTION_CHOSEN
			     & REMOTE_OPERATION_TYPE_MASK;

/**
 * Remote operation response type (rors-*)
 */
static const int RORS_TYPE = RORS_CMIP_GET_CHOSEN & REMOTE_OPERATION_TYPE_MASK;

/**
 * Remote operation error type (roer)
 */
static const int ROER_TYPE = ROER_CHOSEN & REMOTE_OPERATION_TYPE_MASK;

/**
 * Remote operation reject type (rorj)
 */
static const int RORJ_TYPE = RORJ_CHOSEN & REMOTE_OPERATION_TYPE_MASK;

static int communication_notify_state_transition_evt(Context *ctx, fsm_states previous, fsm_states next);

/**
 * State machine transition listener definition
 */
typedef struct StateTransitionListener {
	/**
	 * The entering state to listen
	 */
	fsm_states state;

	/**
	 * Function called when after state transition occurs.
	 */
	communication_state_transition_handler_function handler;
} StateTransitionListener;


/**
 * List of State machine transition listeners
 */
static StateTransitionListener *state_transition_listener_list = NULL;

/**
 * Number of state machine transition listeners
 */
static int state_transition_listener_size = 0;

static int communication_fire_transport_disconnect_evt(Context *ctx);

/**
 * Get Plugin ID based on pointer
 */
unsigned int communication_plugin_id(CommunicationPlugin *plugin)
{
	int i;
	for (i = 1; i <= plugin_count; ++i) {
		if (comm_plugins[i] == plugin) {
			return i;
		}
	}

	DEBUG("Plugin %p not found in registry", plugin);

	return 0;
}

/**
 * Add a communication plugin. Plugins must be added
 * before any other communication function is called.
 * 
 * @param plugin Communication plugin
 */
void communication_add_plugin(CommunicationPlugin *plugin)
{
	++plugin_count;
	int size = sizeof(CommunicationPlugin *) * (plugin_count + 1);

	if (!comm_plugins) {
		// keep zero as invalid
		comm_plugins = malloc(size);
		comm_plugins[0] = 0;
	} else {
		comm_plugins = realloc(comm_plugins, size);
	}

	comm_plugins[plugin_count] = plugin;
}

/**
 * Get communication plugin impl structure
 */
CommunicationPlugin *communication_get_plugin(unsigned int label)
{
	if (!label || (label > plugin_count)) {
		ERROR("Plugin id %d unknown", label);
		return 0;
	}
	return comm_plugins[label];
}

/**
 * Get communication plugin impl structure
 */
int communication_is_trans(Context *ctx)
{
	CommunicationPlugin *plugin = communication_get_plugin(ctx->id.plugin);
	if (plugin)
		if (plugin->type & TRANS_CONTEXT)
			return 1;
	return 0;
}

/**
 * Finalizes thread context
 */
int communication_finalize_thread_context(Context *ctx)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return 0;

	comm_plugin->thread_finalize(ctx);
	return 1;
}

/**
 * Breaks connection
 */
int communication_force_disconnect(Context *ctx)
{
	if (! ctx) {
		return 0;
	}

	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return 0;

	comm_plugin->network_disconnect(ctx);
	return 1;
}

/**
 * Finalizes the communication layer and free memory.
 * This method locks the communication layer thread.
 *
 * This method should be invoked in a thread safe execution.
 */
void communication_finalize()
{
	int i;
	DEBUG(" communication: Finalizing. ");

	// Reset all timeouts

	// Finalizes all threads in execution

	if (communication_is_network_started()) {
		for (i = 1; i <= plugin_count; ++i) {
			CommunicationPlugin *comm_plugin = comm_plugins[i];
			if (comm_plugin->network_finalize() != NETWORK_ERROR_NONE) {
				DEBUG("Trouble finalizing plugin %d", i);
			}
		}

		network_status = NETWORK_STATUS_NOT_INITIALIZED;

		context_iterate(&communication_fire_transport_disconnect_evt);
	}

	communication_remove_all_state_transition_listeners();
	context_remove_all();

	for (i = 1; i <= plugin_count; ++i) {
		CommunicationPlugin *comm_plugin = comm_plugins[i];
		communication_plugin_clear(comm_plugin);
		comm_plugins[i] = NULL;
	}

	free(comm_plugins);
	comm_plugins = NULL;
	plugin_count = 0;

	trans_finalize();

	// thread-safe block - end
}


/**
 * Adds a state transition listener.
 *
 * @param state the state to attach a listener.
 * @param listener_function function.
 *
 * @return 1 if operation succeeds, 0 if not.
 */
int communication_add_state_transition_listener(
	fsm_states state,
	communication_state_transition_handler_function listener_function)
{

	int size = state_transition_listener_size;

	// test if there is not elements in the list
	if (size == 0) {
		state_transition_listener_list = malloc(
				sizeof(struct StateTransitionListener));

	} else { // change the list size
		state_transition_listener_list
		= realloc(
			  state_transition_listener_list,
			  sizeof(struct StateTransitionListener)
			  * (size	+ 1));
	}

	// add element to list
	if (state_transition_listener_list == NULL) {
		return 0;
	}

	StateTransitionListener *listener = &state_transition_listener_list[size];
	listener->state = state;
	listener->handler = listener_function;

	state_transition_listener_size++;

	return 1;
}

/**
 * Removes all state transition listeners
 */
void communication_remove_all_state_transition_listeners()
{
	if (state_transition_listener_list != NULL) {
		state_transition_listener_size = 0;
		free(state_transition_listener_list);
		state_transition_listener_list = NULL;
	}
}


/**
 * Start network layer. After this operation
 * the connection loop will be ready to be executed.
 *
 * This method should be invoked in a thread safe execution.
 */
void communication_network_start()
{
	if (network_status == NETWORK_STATUS_NOT_INITIALIZED) {
		int i;

		for (i = 1; i <= plugin_count; ++i) {
			CommunicationPlugin *comm_plugin = comm_plugins[i];
			int ret_code = comm_plugin->network_init(i);

			if (ret_code != NETWORK_ERROR_NONE) {
				ERROR(" Cannot initialize plugin %d", i);
			}
		}

		network_status = NETWORK_STATUS_INITIALIZED;

	} else if (network_status == NETWORK_STATUS_INITIALIZED) {
		INFO(" Network is already initialized.");
	}
}

/**
 * Check if network layer is running
 * @return 1 if true, 0 if not
 */
int communication_is_network_started()
{
	return network_status == NETWORK_STATUS_INITIALIZED;
}

/**
 * @TODO_REVIEW_DOC
 *
 * @param ctx
 */
static int communication_fire_transport_disconnect_evt(Context *ctx)
{
	if (ctx != NULL) {
		communication_lock(ctx);
		communication_fire_evt(ctx, fsm_evt_ind_transport_disconnect, NULL);
		communication_reset_timeout(ctx);
		communication_unlock(ctx);
	}

	return 1;
}
/**
 * Stops network layer.
 *
 * This method should be invoked in a thread safe execution.
 *
 * @return 1 if operation succeeds, 0 otherwise
 */

int communication_network_stop()
{
	DEBUG("communication: shutting down network.");

	int i;

	for (i = 1; i <= plugin_count; ++i) {
		CommunicationPlugin *comm_plugin = comm_plugins[i];
		if (comm_plugin->network_finalize() != NETWORK_ERROR_NONE) {
			DEBUG("Trouble finalizing plugin %d", i);
		}
	}

	network_status = NETWORK_STATUS_NOT_INITIALIZED;

	context_iterate(&communication_fire_transport_disconnect_evt);

	return 1;
}

/**
 * Creates a connection context for multiple agent support,
 * see topic 8.1 of IEEE 11071-20601 documentation
 *
 * @param id context id
 * @return created context to handle this connection
 */
Context *communication_transport_connect_indication(ContextId id)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(id.plugin);

	if (!comm_plugin)
		return 0;

	Context *ctx = context_create(id, comm_plugin->type);

	// thread-safe block - begin
	comm_plugin->thread_init(ctx);
	communication_lock(ctx);

	if (ctx != NULL) {
		communication_fire_evt(ctx, fsm_evt_ind_transport_connection,
				       NULL);
	}

	communication_unlock(ctx);
	// thread-safe block - end
	return ctx;
}


/**
 * Destroys a connection context,
 * see topic 8.1 of IEEE 11071-20601 documentation
 *
 * @param id context id
 */
void communication_transport_disconnect_indication(ContextId id)
{
	Context *ctx = context_get(id);
	communication_fire_transport_disconnect_evt(ctx);
	context_remove(id);
}


/**
 * Locks this connection context if communication runs with
 * multithread implementation
 *
 * @param ctx
 */
void communication_lock(Context *ctx)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return;

	comm_plugin->thread_lock(ctx);
}

/**
 * Unlocks this connection context if communication runs with
 * multithread implementation
 *
 * @param ctx
 */
void communication_unlock(Context *ctx)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return;

	comm_plugin->thread_unlock(ctx);
}


/**
 * Wait for data input from network.
 */
int communication_wait_for_data_input(Context *ctx)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return 0;

	return comm_plugin->network_wait_for_data(ctx);
}

/**
 * Reads APDU from transport layer stream
 *
 * @param ctx connection context
 */
void communication_read_input_stream(Context *ctx)
{
	if (ctx != NULL) {
		communication_process_input_data(ctx, communication_get_apdu_stream(ctx));
	}
}

/**
 * Process the read stream data
 *
 * @param ctx connection context
 * @param stream the stream with input data
 */
void communication_process_input_data(Context *ctx, ByteStreamReader *stream)
{
	int error = 0;

	if (ctx != NULL) {
		if (stream == NULL) {
			return;
		}

#ifdef APDU_DUMP
		ioutil_buffer_to_file("apdu_dump", 5, (unsigned char *) "recv ", 1);
		ioutil_buffer_to_file("apdu_dump", stream->unread_bytes, stream->buffer, 1);
		ioutil_buffer_to_file("apdu_dump", 1, (unsigned char *) "\n", 1);
#endif

		// Decode the APDU
		APDU apdu;
		decode_apdu(stream, &apdu, &error);
		if (error) {
			// TODO abort association
			DEBUG("Invalid APDU, ignoring");
			return;
		}

		// Process APDU
		communication_process_apdu(ctx, &apdu);

		// Delete APDU
		del_apdu(&apdu);

		del_byte_stream_reader(stream, 1);
	}
}


/**
 * Get received APDU stream.
 */
ByteStreamReader *communication_get_apdu_stream(Context *ctx)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return 0;

	return comm_plugin->network_get_apdu_stream(ctx);
}

/**
 * Fires a event of communication layer.
 * This method locks the communication layer thread.
 *
 * @param ctx connection context
 * @param evt input event
 * @param data
 */
void communication_fire_evt(Context *ctx, fsm_events evt, FSMEventData *data)
{
	// thread-safe block - start
	communication_lock(ctx);

	// thread safe state transition
	FSM *fsm  = ctx->fsm;
	fsm_states previous = fsm->state;

	if (fsm_process_evt(ctx, evt, data) == FSM_PROCESS_EVT_RESULT_STATE_CHANGED) {
		communication_notify_state_transition_evt(ctx, previous, fsm->state);
	}

	communication_unlock(ctx);
	// thread-safe block - end
}

static void communication_process_apdu_agent(Context *ctx, APDU *apdu)
{
	switch (ctx->fsm->state) {
	case fsm_state_disconnected:
		ERROR("Cannot process APDU in disconnected state ");
		break;
	case fsm_state_unassociated:
		// TODO does it make sense manager to take initiative?
		association_unassociated_process_apdu_agent(ctx, apdu);
		break;
	case fsm_state_associating:
		association_unassociated_process_apdu_agent(ctx, apdu);
		break;
	case fsm_state_operating:
		operating_process_apdu_agent(ctx, apdu);
		break;
	case fsm_state_disassociating:
		disassociating_process_apdu_agent(ctx, apdu);
		break;
	case fsm_state_config_sending:
		configuring_agent_config_sending_process_apdu(ctx, apdu);
		break;
	case fsm_state_waiting_approval:
		configuring_agent_waiting_approval_process_apdu(ctx, apdu);
		break;
	default:
		// TODO error handling
		ERROR(" service error: Invalid machine state ");
		exit(1);
	}
}

static void communication_process_apdu_manager(Context *ctx, APDU *apdu)
{
	switch (ctx->fsm->state) {
	case fsm_state_disconnected:
		ERROR("Cannot process APDU in disconnected state ");
		break;
	case fsm_state_unassociated:
		association_unassociated_process_apdu(ctx, apdu);
		break;
	case fsm_state_associating:
		// The associating state is not used by manager
		break;
	case fsm_state_operating:
		operating_process_apdu(ctx, apdu);
		break;
	case fsm_state_disassociating:
		disassociating_process_apdu(ctx, apdu);
		break;
	case fsm_state_checking_config:
		configuring_checking_state_process_apdu(ctx, apdu);
		break;
	case fsm_state_waiting_for_config:
		configuring_waiting_state_process_apdu(ctx, apdu);
		break;
	default:
		// TODO error handling
		ERROR(" service error: Invalid machine state ");
		exit(1);
	}
}

/**
 * Process the incoming APDU calling to proper procedure.
 * This method locks the communication layer thread.
 *
 * @param ctx current context
 * @param apdu APDU
 */
void communication_process_apdu(Context *ctx, APDU *apdu)
{
	// thread-safe block - start
	communication_lock(ctx);

	DEBUG(" communication: current sm(%s) ", fsm_get_current_state_name(ctx->fsm));

	if (ctx->type & AGENT_CONTEXT) {
		communication_process_apdu_agent(ctx, apdu);
	} else {
		communication_process_apdu_manager(ctx, apdu);
	}

	communication_unlock(ctx);
	// thread-safe block - end
}

/**
 * Send APDU to agent.
 * This method locks the communication layer thread.
 *
 * @param ctx context
 * @param apdu APDU
 * @return 1 if operation succeeds, 0 otherwise
 */
int communication_send_apdu(Context *ctx, APDU *apdu)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return 0;

	// thread-safe block - start
	communication_lock(ctx);

	DEBUG(" communication: sending APDU ");

	ByteStreamWriter *encoded_apdu = NULL;
	encoded_apdu = byte_stream_writer_instance(apdu->length + 4/*apdu header*/);

	encode_apdu(encoded_apdu, apdu);

#ifdef APDU_DUMP
	ioutil_buffer_to_file("apdu_dump", 5, (unsigned char *) "send ", 1);
	ioutil_buffer_to_file("apdu_dump", encoded_apdu->size, encoded_apdu->buffer, 1);
	ioutil_buffer_to_file("apdu_dump", 1, (unsigned char *) "\n", 1);
#endif

	// send encoded_apdu bytes
	int return_val = comm_plugin->network_send_apdu_stream(ctx, encoded_apdu);

	del_byte_stream_writer(encoded_apdu, 1);

	DEBUG(" communication: APDU sent ");
	communication_unlock(ctx);
	// thread-safe block - end

	if (return_val == NETWORK_ERROR_NONE)
		return 1;

	return 0;
}

/**
 * Abort communication.
 *
 * @param ctx current context.
 * @param reason reason to abort.
 */
void communication_abort(Context *ctx, Abort_reason reason)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return;

	// thread-safe block - start
	communication_lock(ctx);

	APDU apdu;
	apdu.choice = ABRT_CHOSEN;
	apdu.length = sizeof(ABRT_apdu);
	apdu.u.abrt.reason = reason;

	communication_send_apdu(ctx, &apdu);

	communication_unlock(ctx);
	// thread-safe block - end
}

/**
 * Send communication abort APDU to agent.
 * This method locks the communication layer thread.
 *
 * @param ctx connection context
 * @param evt input event not used
 * @param data not used
 */
void communication_abort_undefined_reason_tx(Context *ctx, fsm_events evt,
		FSMEventData *data)
{
	// thread-safe block - start
	communication_lock(ctx);

	communication_abort(ctx, ABORT_REASON_UNDEFINED);

	communication_unlock(ctx);
	// thread-safe block - end
}

/**
 *
 * Check if invoke ID is known, if not abort association
 * This method locks the communication layer thread.
 *
 * @param ctx context
 * @param evt input event
 * @param data should inform the received APDU of type PRST
 */
void communication_check_invoke_id_abort_tx(Context *ctx, fsm_events evt,
		FSMEventData *data)
{
	// thread-safe block - start
	communication_lock(ctx);

	if (data != NULL && data->received_apdu != NULL
	    && data->received_apdu->choice == PRST_CHOSEN) {
		APDU *apdu = data->received_apdu;
		DATA_apdu *data_apdu = encode_get_data_apdu(&apdu->u.prst);
		service_check_known_invoke_id(ctx, data_apdu);
	}

	communication_unlock(ctx);
	// thread-safe block - end
}

/**
 * Receive the disconnected signal
 * This method locks the communication layer thread.
 *
 * @param ctx context
 * @param evt input event
 * @param data event data
 */
void communication_disconnect_tx(Context *ctx, fsm_events evt,
				 FSMEventData *data)
{
	// thread-safe block - start
	communication_lock(ctx);

	DEBUG(" communication: disconnected ");

	communication_unlock(ctx);
	// thread-safe block - end
}

/**
 * Send error message to Agent.
 * This method locks the communication layer thread.
 *
 * @param ctx current context
 * @param invoke_id invoke_id of the original apdu.
 * @param error contain the error type and its arguments
 */
void communication_send_roer(Context *ctx, InvokeIDType invoke_id, ErrorResult *error)
{
	// thread-safe block - start
	communication_lock(ctx);

	APDU apdu;
	apdu.choice = PRST_CHOSEN;

	DATA_apdu data_apdu;
	data_apdu.invoke_id = invoke_id;
	data_apdu.message.choice = ROER_CHOSEN;

	data_apdu.message.u.roer = *error;

	data_apdu.message.length = data_apdu.message.u.roer.parameter.length
				   + sizeof(ERROR);

	apdu.u.prst.length = data_apdu.message.length
			     + sizeof(DATA_apdu_choice) + sizeof(InvokeIDType);
	apdu.length = apdu.u.prst.length + sizeof(intu16);

	encode_set_data_apdu(&apdu.u.prst, &data_apdu);

	communication_send_apdu(ctx, &apdu);

	communication_unlock(ctx);
	// thread-safe block - end
}

/**
 * Send error message to Agent.
 * This method locks the communication layer thread.
 *
 * @param ctx context
 * @param evt input event
 * @param data Contains event data which in this case is the structure ErrorResult
 */
void communication_roer_tx(Context *ctx, fsm_events evt, FSMEventData *data)
{
	// thread-safe block - start
	communication_lock(ctx);

	if (data != NULL && data->choice == FSM_EVT_DATA_ERROR_RESULT) {
		DATA_apdu *input_data_apdu = encode_get_data_apdu(
						     &data->received_apdu->u.prst);
		communication_send_roer(ctx, input_data_apdu->invoke_id,
					&data->u.error_result);
	}

	communication_unlock(ctx);
	// thread-safe block - end
}

/**
 * Notifies the communication layer that timeout indication occurs.
 * This method locks the communication layer thread.
 *
 * @param ctx Context
 */
void communication_timeout(Context *ctx)
{
	// thread-safe block - start
	communication_lock(ctx);

	if (ctx != NULL) {
		communication_fire_evt(ctx, fsm_evt_ind_timeout, NULL);
		if (ctx->type & MANAGER_CONTEXT)
			manager_notify_evt_timeout(ctx);
		else if (ctx->type & AGENT_CONTEXT)
			agent_notify_evt_timeout(ctx);
	}

	communication_unlock(ctx);
	// thread-safe block - end
}

/**
 * Check if message choice is Remote Operation Invoke
 *
 * @param data Data APDU
 * @return 1 if message choice matches a roiv-*
 */
int communication_is_roiv_type(DATA_apdu *data)
{
	return (data->message.choice & REMOTE_OPERATION_TYPE_MASK) == ROIV_TYPE;
}

/**
 * Check if message choice is Remote Operation Response
 *
 * @param data APDU
 * @return 1 if message choice matches a rors-*
 */
int communication_is_rors_type(DATA_apdu *data)
{
	return (data->message.choice & REMOTE_OPERATION_TYPE_MASK) == RORS_TYPE;
}

/**
 * Check if message choice is Remote Operation Error
 *
 * @param data Data APDU
 * @return 1 if message choice matches a roer
 */
int communication_is_roer_type(DATA_apdu *data)
{
	return (data->message.choice & REMOTE_OPERATION_TYPE_MASK) == ROER_TYPE;
}

/**
 * Check if message choice is Remote Operation Reject
 *
 * @param data Data APDU
 * @return 1 if message choice matches a rorj
 */
int communication_is_rorj_type(DATA_apdu *data)
{
	return (data->message.choice & REMOTE_OPERATION_TYPE_MASK) == RORJ_TYPE;
}

/**
 * Gets the current state.
 *
 * @param ctx Context
 * @return the current state.
 */
fsm_states communication_get_state(Context *ctx)
{
	fsm_states state;
	// thread-safe block - start
	communication_lock(ctx);

	state = ctx->fsm->state;

	communication_unlock(ctx);
	// thread-safe block - end

	return state;
}

/**
 * Returns the current state name
 */
char *communication_get_state_name(Context *ctx)
{
	// thread-safe block - start
	communication_lock(ctx);

	char *ret = fsm_get_current_state_name(ctx->fsm);

	communication_unlock(ctx);
	// thread-safe block - end

	return ret;
}

/**
 * Notifies to state transition listeners the transition.
 *
 * @param ctx context
 * @param previous previous FSM state.
 * @param next next FSM state.
 *
 * @return 1 if any listener catches the notification, 0 if not.
 */
static int communication_notify_state_transition_evt(Context *ctx, fsm_states previous, fsm_states next)
{
	// thread-safe block - begin
	communication_lock(ctx);

	int ret_val = 0;
	int i;

	for (i = 0; i <  state_transition_listener_size; i++) {
		StateTransitionListener *l = &state_transition_listener_list[i];

		if ((l->state == next || l->state == fsm_state_size) && l->handler != NULL) {
			(l->handler)(ctx, previous, next);
			ret_val = 1;
		}
	}

	communication_unlock(ctx);
	// thread-safe block - end
	return ret_val;
}

/**
 * Atomic Get operation for connection_loop_active variable
 *
 * @param ctx connection context
 * @return returns 1 if connection loop is active
 */
static int get_connection_loop_active(Context *ctx)
{
	int ret = 0;
	communication_lock(ctx);
	ret = ctx->connection_loop_active;
	communication_unlock(ctx);
	return ret;
}

/**
 * Atomic Set operation for connection_loop_active variable
 *
 * @pararm ctx connection context
 * @param val
 */
static void set_connection_loop_active(Context *ctx, int val)
{
	communication_lock(ctx);
	ctx->connection_loop_active = val;
	communication_unlock(ctx);
}

/**
 * Runs connection loop over communication layer.
 * This function must run after 'communication_network_start()'operation.
 *
 * @param ctx current context
 */
void communication_connection_loop(Context *ctx)
{
	if (get_connection_loop_active(ctx))
		return; // connection loop already running

	if (!communication_is_network_started()) {
		ERROR(" The network layer is not initialized, call communication_network_start() first.");
		return;
	}

	set_connection_loop_active(ctx, 1); // activate loop flag

	DEBUG(" communication:Waiting for data\n");

	// context may become invalid right after wait_for_data_input,
	// so we need so check it every loop, based on ID.
	ContextId id = ctx->id;

	while ((ctx = context_get(id))) {
		if (!get_connection_loop_active(ctx)) {
			break;
		}

		int ret = communication_wait_for_data_input(ctx);

		if (ret == NETWORK_ERROR_NONE) {
			communication_read_input_stream(ctx);
		} else {
			// higher layer must see the error
			break;
		}
	}
}


/**
 * Cancels connection loop
 * @param ctx connection context
 */
void communication_cancel_connection_loop(Context *ctx)
{
	set_connection_loop_active(ctx, 0);
}

/**
 * Waits X seconds for timeout and execute callback function. The callback will be
 * called inside communication layer thread locking block.
 *
 * @param ctx context
 * @param func structure, it will be automatically deallocated
 * @param timeout
 *
 * @return 1 if operation succeeds, 0 if thread could not be created
 */
int communication_count_timeout(Context *ctx, timer_callback_function func, intu32 timeout)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return 0;

	communication_reset_timeout(ctx);
	ctx->timeout_action.func = func;
	ctx->timeout_action.timeout = timeout;
	return comm_plugin->timer_count_timeout(ctx);
}

/**
 * Reset timeout counter
 * @param ctx context
 */
void communication_reset_timeout(Context *ctx)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return;

	comm_plugin->timer_reset_timeout(ctx);
	ctx->timeout_action.func = NULL;
	ctx->timeout_action.timeout = 0;
	ctx->timeout_action.id = 0;
}

/**
 * Blocks execution until timeout function execute
 * @param ctx context
 */
void communication_wait_for_timeout(Context *ctx)
{
	CommunicationPlugin *comm_plugin =
		communication_get_plugin(ctx->id.plugin);

	if (!comm_plugin)
		return;

	comm_plugin->timer_wait_for_timeout(ctx);
}

/**
 * Release association
 *
 * @param ctx
 */
void req_association_release(Context *ctx)
{
	FSMEventData evt;

	evt.choice = FSM_EVT_DATA_RELEASE_REQUEST_REASON;
	evt.u.release_request_reason = RELEASE_REQUEST_REASON_NORMAL;

	communication_fire_evt(ctx, fsm_evt_req_assoc_rel, &evt);
}

/** @} */
