/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file agent.c
 * \brief Protocol agent implementation.
 * \author Adrian Guedes
 * \author Fabricio Silva
 * \author Elvis Pfutzenreuter
 * \date May 31, 2011
 *
 * \internal
 * Copyright (C) 2011 Signove Tecnologia Corporation.
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
 */

/**
 * \defgroup Agent Agent
 * \brief The Agent provides a facade to use IEEE standard implementation. All health/wellness
 * application should access IEEE standard library through this class.
 *
 * The following code shows how to implement a simple application that reads data from device. Once
 * the health device is properly paired with agent (desktop), the application receives measures
 * and print them. It is also possible to execute commands, such as <em>connect</em>, <em>disconnect</em>
 * and <em>exit</em>.
 *
 * @{
 */

#include <stdlib.h>
#include <stdio.h>
#include "src/agent_p.h"
#include "src/api/data_encoder.h"
#include "src/communication/plugin/plugin.h"
#include "src/communication/communication.h"
#include "src/communication/configuring.h"
#include "src/communication/stdconfigurations.h"
#include "src/specializations/blood_pressure_monitor.h"
#include "src/specializations/pulse_oximeter.h"
#include "src/specializations/weighing_scale.h"
#include "src/specializations/glucometer.h"
#include "src/dim/mds.h"
#include "src/util/log.h"


/**
 * Agent listener list
 */
static AgentListener *agent_listener_list = NULL;
/**
 * Agent listener count
 */
static int agent_listener_count = 0;

/**
 * Agent specialization standard configuration id
 * and callbacks that supply data
 */
ConfigId agent_specialization;

/**
 * Function called to generate data when measurement event is being prepared
 */
void *(*agent_event_report_cb)();

/**
 * Function called when agent's system id is needed by stack
 */
struct mds_system_data *(*agent_mds_data_cb)();

static void agent_handle_transition_evt(Context *ctx, fsm_states previous, fsm_states next);


/*!
 *
 * This API implements the IEEE 11073-20601 Standard and some device
 * specializations (IEEE 11073-104XX).
 *
 * The IEEE 11073-20601 defines a common framework for making an abstract
 * model of personal health data available in transport-independent
 * transfer syntax required to establish logical connections between
 * systems and to provide presentation capabilities and services needed
 * to perform communication tasks. The protocol is optimized to personal
 * health usage requirements and leverages commonly used methods and
 * tools wherever possible.
 *
 * The ISO/IEEE 11073 family of standards is based on an object-oriented
 * systems management paradigm. Data (measurement, state, and so on) are
 * modeled in the form of information objects that are accessed and
 * manipulated using an object access service protocol.
 *
 * The overall ISO/IEEE 11073 system model is divided into three principal
 * components: the DIM, the service model, and the communications model.
 * These three models work together to represent data, define data access and
 * command methodologies, and communicate the data from an agent to a agent.
 *
 * For more information about how to use IEEE standard implementation, see \ref Agent
 * implementation (including examples).
 *
 * For more information about how to add new device specializations at IEEE library
 * , see documentation on \ref SpecializationDescription "IEEE device specializations"
 */

/**
 * Initializes the agent on application load. This function also
 * registers existing device specializations.
 *
 * This method should be invoked in a thread safe execution.
 *
 * @param plugins the configured plugins to define communication behavior
 * @param specialization Specialization of the agent
 * @param event_report_cb The event report callback
 * @param mds_data_cb Data callback
 */
void agent_init(CommunicationPlugin **plugins, int specialization,
		void *(*event_report_cb)(),
		struct mds_system_data *(*mds_data_cb)())
{
	DEBUG("Agent Initialization");

	agent_specialization = specialization;
	agent_event_report_cb = event_report_cb;
	agent_mds_data_cb = mds_data_cb;
	
	while (*plugins) {
		(*plugins)->type |= AGENT_CONTEXT;
		communication_add_plugin(*plugins);
		++plugins;
	}

	// Listen to all communication state transitions
	communication_add_state_transition_listener(fsm_state_size, &agent_handle_transition_evt);

	// Register standard configurations for each specialization.
	std_configurations_register_conf(
		blood_pressure_monitor_create_std_config_ID02BC());
	std_configurations_register_conf(
		pulse_oximeter_create_std_config_ID0190());
	std_configurations_register_conf(
		pulse_oximeter_create_std_config_ID0191());
	std_configurations_register_conf(
		weighting_scale_create_std_config_ID05DC());
	std_configurations_register_conf(
		glucometer_create_std_config_ID06A4());
}

/**
 * Finalizes the agent
 *
 * This method should be invoked in a thread safe execution.
 *
 */
void agent_finalize()
{
	DEBUG("Agent Finalization");

	agent_remove_all_listeners();
	communication_finalize();
	std_configurations_destroy();
}


/**
 * Adds a agent listener.
 *
 * This method should be invoked in a thread safe execution.
 *
 * @param listener the listen to be added.
 * @return 1 if operation succeeds, 0 if not.
 */
int agent_add_listener(AgentListener listener)
{

	// test if there is not elements in the list
	if (agent_listener_count == 0) {
		agent_listener_list = malloc(sizeof(struct AgentListener));

	} else { // change the list size
		agent_listener_list = realloc(agent_listener_list,
						sizeof(struct AgentListener)
						* (agent_listener_count + 1));
	}

	// add element to list

	if (agent_listener_list == NULL) {
		return 0;
	}

	agent_listener_list[agent_listener_count] = listener;

	agent_listener_count++;

	return 1;

}


/**
 * Removes all agent's listeners
 *
 * This method should be invoked in a thread safe execution.
 */
void agent_remove_all_listeners()
{
	if (agent_listener_list != NULL) {
		agent_listener_count = 0;
		free(agent_listener_list);
		agent_listener_list = NULL;
	}
}

/**
 * Notifies 'device associated'  event.
 * This function should be visible to source layer of events.
 * This function must be called in a thread safe communication context.
 *
 * @param ctx
 * @return 1 if any listener catches the notification, 0 if not
 */
int agent_notify_evt_device_associated(Context *ctx)
{
	int ret_val = 0;
	int i;

	for (i = 0; i < agent_listener_count; i++) {
		AgentListener *l = &agent_listener_list[i];

		if (l != NULL && l->device_associated != NULL) {
			(l->device_associated)(ctx);
			ret_val = 1;
		}
	}

	return ret_val;
}

/**
 * Notifies 'device connected'  event.
 * This function should be visible to source layer of events.
 * This function must be called in a thread safe communication context.
 *
 * @param ctx
 * @return 1 if any listener catches the notification, 0 if not
 */
int agent_notify_evt_device_connected(Context *ctx)
{
	int ret_val = 0;
	int i;

	for (i = 0; i < agent_listener_count; i++) {
		AgentListener *l = &agent_listener_list[i];

		if (l != NULL && l->device_connected != NULL) {
			(l->device_connected)(ctx);
			ret_val = 1;
		}
	}

	return ret_val;
}

/**
 * Notifies 'device unavailable'  event.
 * This function should be visible to source layer of events.
 * This function must be called in a thread safe communication context.
 *
 * @param ctx
 * @return 1 if any listener catches the notification, 0 if not
 */
int agent_notify_evt_device_unavailable(Context *ctx)
{
	int ret_val = 0;
	int i;

	for (i = 0; i < agent_listener_count; i++) {
		AgentListener *l = &agent_listener_list[i];

		if (l != NULL && l->device_unavailable != NULL) {
			(l->device_unavailable)(ctx);
			ret_val = 1;
		}
	}

	return ret_val;
}


/**
 * Notifies 'communication timeout'  event.
 * This function should be visible to source layer of events.
 * This function must be called in a thread safe communication context.
 *
 * @param ctx current context
 * @return 1 if any listener catches the notification, 0 if not
 */
int agent_notify_evt_timeout(Context *ctx)
{
	int ret_val = 0;
	int i;

	for (i = 0; i < agent_listener_count; i++) {
		AgentListener *l = &agent_listener_list[i];

		if (l != NULL && l->timeout != NULL) {
			(l->timeout)(ctx);
			ret_val = 1;
		}
	}

	return ret_val;
}

/**
 * Start to listen agents, it means the network layer
 * is initialized and the communication will be ready to
 * read/send data.
 */
void agent_start()
{
	// kill any previous connection
	if (communication_is_network_started()) {
		DEBUG("Agent restarting...");
		agent_stop();
	} else {
		DEBUG("Agent starting...");
	}

	communication_network_start();
}


/**
 * Stop to listen agents, all open network connections will be closed
 */
void agent_stop()
{
	if (communication_is_network_started()) {
		DEBUG("Agent stopping...");
		communication_network_stop();
	}
}

/**
 * Runs connection loop over communication layer.
 * This function must run after 'agent_start()'operation if no mainloop
 * implementation is used e.g. GMainLoop.
 *
 * @param context_id Current context.
 */
void agent_connection_loop(ContextId context_id)
{
	communication_connection_loop(context_get(context_id));
}

/**
 * Requests "association release request" to agent
 * @param id the ID of current context.
 */
void agent_request_association_release(ContextId id)
{
	req_association_release(context_get(id));
}

/**
 * Requests "association abort" to agent
 * @param id the ID of current context.
 */
void agent_request_association_abort(ContextId id)
{
	communication_fire_evt(context_get(id), fsm_evt_req_assoc_abort, NULL);
}


/**
 * Handle low level communication state transition events and notifies
 * high level application
 *
 * @param ctx context
 * @param previous the previous FSM state.
 * @param next the next FSM state.
 */
void agent_handle_transition_evt(Context *ctx, fsm_states previous, fsm_states next)
{
	DEBUG("agent: handling transition event");

	if (previous == fsm_state_operating && next != previous) {
		DEBUG(" agent: Notify device unavailable.\n");
		// Exiting operating state
		agent_notify_evt_device_unavailable(ctx);
	}

	if (previous == fsm_state_disconnected &&
					next == fsm_state_unassociated) {
		agent_notify_evt_device_connected(ctx);
	}

	if (previous != next && next == fsm_state_operating) {
		agent_notify_evt_device_associated(ctx);
	}

	if (next == fsm_state_config_sending) {
		// provoke it to go to waiting_approval
		communication_fire_evt(ctx, fsm_evt_req_send_config, NULL);
	}
}

/**
 * Provoke agent to initiate association
 *
 * @param id context Id
 */
void agent_associate(ContextId id)
{
	DEBUG(" agent: Move state machine to init assoc");
	communication_fire_evt(context_get(id), fsm_evt_req_assoc, NULL);
}

/**
 * Provoke agent to terminate connection
 *
 * @param id context Id
 */
void agent_disconnect(ContextId id)
{
	DEBUG(" agent: terminate conn");
	communication_force_disconnect(context_get(id));
}

/**
 * Provoke agent to send event report with measure data
 *
 * @param id context Id
 */
void agent_send_data(ContextId id)
{
	communication_fire_evt(context_get(id), fsm_evt_req_send_event, NULL);
}

/** @} */
