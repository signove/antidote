/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file manager.c
 * \brief Protocol manager implementation.
 * \author Adrian Guedes
 * \author Fabricio Silva
 * \date Jun 22, 2010
 *
 * \internal
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
 */

/**
 * \defgroup Manager Manager
 * \brief The Manager provides a facade to use IEEE standard implementation. All health/wellness
 * application should access IEEE standard library through this class.
 *
 * The following code shows how to implement a simple application that reads data from device. Once
 * the health device is properly paired with manager (desktop), the application receives measures
 * and print them. It is also possible to execute commands, such as <em>connect</em>, <em>disconnect</em>
 * and <em>exit</em>.
 *
 * \code
 *
 * int main() {
 *		unsigned int size = 100;
 *		char *input = (char *) malloc(size);
 *
 *		manager_init();
 *
 *		manager_add_listener(manager_evt_measurement_data_updated, new_data_received);
 *
 *		manager_start();
 *
 *		while (1) {
 *			fprintf(stderr, "--> ");
 *			unsigned int read_bytes = getline(&input, &size, stdin);
 *
 *			input[read_bytes-1] = '\0';
 *
 *			if (strcmp(input, "exit") == 0) {
 *				print_device_attributes();
 *				break;
 *			} else if (strcmp(input, "start") == 0) {
 *				manager_start();
 *			} else if (strcmp(input, "stop") == 0) {
 *				manager_stop();
 *			} else if (strcmp(input, "state") == 0) {
 *				char *state = communication_get_state_name();
 *				fprintf(stderr, "Current State: %s\n", state);
 *			} else if (strcmp(input, "print_mds") == 0) {
 *				print_device_attributes();
 *			} else {
 *				fprintf(stderr, "Command not recognized\n");
 *			}
 *		}
 *
 *		manager_finalize();
 *		free(input);
 *		return 0;
 * }
 *
 * \endcode
 *
 * @{
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "src/manager_p.h"
#include "src/api/data_encoder.h"
#include "src/communication/plugin/plugin.h"
#include "src/communication/communication.h"
#include "src/communication/context_manager.h"
#include "src/communication/extconfigurations.h"
#include "src/communication/configuring.h"
#include "src/communication/stdconfigurations.h"
#include "src/specializations/blood_pressure_monitor.h"
#include "src/specializations/pulse_oximeter.h"
#include "src/specializations/weighing_scale.h"
#include "src/specializations/glucometer.h"
#include "src/util/log.h"


/**
 * Manager listener list
 */
static ManagerListener *manager_listener_list = NULL;

/**
 * Manager listener count
 */
static int manager_listener_count = 0;

static void manager_handle_transition_evt(Context *ctx, fsm_states previous, fsm_states next);


/*! \mainpage Antidote: IEEE 11073-20601 Implementation
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
 * command methodologies, and communicate the data from an agent to a manager.
 *
 * For more information about how to use IEEE standard implementation, see \ref Manager
 * implementation (including examples).
 *
 * For more information about how to add new device specializations at IEEE library
 * , see documentation on \ref SpecializationDescription "IEEE device specializations"
 */

/**
 * Initializes the manager on application load. This function also
 * registers existing device specializations.
 *
 * This method should be invoked in a thread safe execution.
 *
 * @param plugins the configured communication plugins
 */
void manager_init(CommunicationPlugin **plugins)
{
	DEBUG("Manager Initialization");

	while (*plugins) {
		(*plugins)->type |= MANAGER_CONTEXT;
		communication_add_plugin(*plugins);
		++plugins;
	}

	// Listen to all communication state transitions
	communication_add_state_transition_listener(fsm_state_size, &manager_handle_transition_evt);

	// Register standard configurations for each specialization.
	// (comment these if you want to test acquisition of extended
	// configurations)
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

	// Load Configurations File
	ext_configurations_load_configurations();

}

/**
 * Finalizes the manager
 *
 * This method should be invoked in a thread safe execution.
 *
 */
void manager_finalize()
{
	DEBUG("Manager Finalization");

	manager_remove_all_listeners();
	communication_finalize();

	ext_configurations_destroy();
	std_configurations_destroy();
}


/**
 * Adds a manager listener.
 *
 * This method should be invoked in a thread safe execution.
 *
 * @param listener the listen to be added.
 * @return 1 if operation succeeds, 0 if not.
 */
int manager_add_listener(ManagerListener listener)
{

	// test if there is not elements in the list
	if (manager_listener_count == 0) {
		manager_listener_list = malloc(sizeof(struct ManagerListener));

	} else { // change the list size
		manager_listener_list = realloc(manager_listener_list,
						sizeof(struct ManagerListener)
						* (manager_listener_count + 1));
	}

	// add element to list

	if (manager_listener_list == NULL) {
		return 0;
	}

	manager_listener_list[manager_listener_count] = listener;

	manager_listener_count++;

	return 1;

}


/**
 * Removes all manager's listeners
 *
 * This method should be invoked in a thread safe execution.
 */
void manager_remove_all_listeners()
{
	if (manager_listener_list != NULL) {
		manager_listener_count = 0;
		free(manager_listener_list);
		manager_listener_list = NULL;
	}
}

/**
 * Notifies 'device available'  event.
 * This function should be visible to source layer of events.
 * This function must be called in a thread safe communication context.
 *
 * @param ctx
 * @param data_list with association information and configuration
 * @return 1 if any listener catches the notification, 0 if not
 */
int manager_notify_evt_device_available(Context *ctx, DataList *data_list)
{
	int ret_val = 0;
	int i;

	for (i = 0; i < manager_listener_count; i++) {
		ManagerListener *l = &manager_listener_list[i];

		if (l != NULL && l->device_available != NULL) {
			(l->device_available)(ctx, data_list);
			ret_val = 1;
		}
	}

	data_list_del(data_list);
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
int manager_notify_evt_device_unavailable(Context *ctx)
{
	int ret_val = 0;
	int i;

	for (i = 0; i < manager_listener_count; i++) {
		ManagerListener *l = &manager_listener_list[i];

		if (l != NULL && l->device_unavailable != NULL) {
			(l->device_unavailable)(ctx);
			ret_val = 1;
		}
	}

	return ret_val;
}


/**
 * Notifies 'measurement data updated'  event.
 * This function should be visible to source layer of events.
 * This function must be called in a thread safe communication context.
 *
 * @param ctx
 * @param data_list with the measured data.
 * @return 1 if any listener catches the notification, 0 if not
 */
int manager_notify_evt_measurement_data_updated(Context *ctx, DataList *data_list)
{
	int ret_val = 0;
	int i;

	for (i = 0; i < manager_listener_count; i++) {
		ManagerListener *l = &manager_listener_list[i];

		if (l != NULL && l->measurement_data_updated != NULL) {
			(l->measurement_data_updated)(ctx, data_list);
			ret_val = 1;
		}
	}

	data_list_del(data_list);
	return ret_val;

}

/**
 * Notifies 'segment data xfer'  event.
 * This function should be visible to source layer of events.
 * This function must be called in a thread safe communication context.
 *
 * @param ctx
 * @param handle PM-Store handle
 * @param instnumber PM-Segment instance number
 * @param data_list with the segment data. Ownership is transferred; delete with data_list_del.
 * @return 1 if any listener catches the notification, 0 if not
 */
int manager_notify_evt_segment_data(Context *ctx, int handle, int instnumber,
							DataList *data_list)
{
	int ret_val = 0;
	int i;

	for (i = 0; i < manager_listener_count; i++) {
		ManagerListener *l = &manager_listener_list[i];

		if (l && l->segment_data_received) {
			(l->segment_data_received)(ctx, handle, instnumber, data_list);
			ret_val = 1;
		}
	}

	// Since encoding this may take a lot of time, we pass ownership to
	// listeners. If there is more than one in app, it must make a deep
	// copy of DataList or coordinate between listeners to free in time.

	// data_list_del(data_list);

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
int manager_notify_evt_timeout(Context *ctx)
{
	int ret_val = 0;
	int i;

	for (i = 0; i < manager_listener_count; i++) {
		ManagerListener *l = &manager_listener_list[i];

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
void manager_start()
{
	// kill any previous connection
	if (communication_is_network_started()) {
		DEBUG("Manager restarting...");
		manager_stop();
	} else {
		DEBUG("Manager starting...");
	}

	communication_network_start();
}


/**
 * Stop to listen agents, all open network connections will be closed
 */
void manager_stop()
{
	if (communication_is_network_started()) {
		DEBUG("Manager stopping...");
		communication_network_stop();
	}
}

/**
 * Runs connection loop over communication layer.
 * This function must run after 'manager_start()' operation if
 * no event loop framework is used e.g. GLib. It is meant for
 * simple tests only.
 * 
 * @param context_id Current context.
 */
void manager_connection_loop(ContextId context_id)
{
	communication_connection_loop(context_get(context_id));
}

/**
 * Requests "association release request" to agent
 * @param id the ID of current context.
 */
void manager_request_association_release(ContextId id)
{
	FSMEventData evt;

	evt.choice = FSM_EVT_DATA_RELEASE_REQUEST_REASON;
	evt.u.release_request_reason = RELEASE_REQUEST_REASON_NORMAL;

	communication_fire_evt(context_get(id), fsm_evt_req_assoc_rel, &evt);
}

/**
 * Requests "association abort" to agent
 * @param id the ID of current context.
 */
void manager_request_association_abort(ContextId id)
{
	communication_fire_evt(context_get(id), fsm_evt_req_assoc_abort, NULL);
}


/**
 * Sets the Operational-State attribute of the scanner
 * objects. This attribute defines if scanner is active.
 *
 * @param id the ID of current context.
 * @param handle Scanner Object Id
 * @param state new value of the Operational-State attribute
 * @param callback callback function of the request
 * @return pointer to request sent
 */
Request *manager_set_operational_state_of_the_scanner(ContextId id,
		HANDLE handle, OperationalState state,
		service_request_callback callback)
{
	Context *ctx = context_get(id);

	if (ctx != NULL) {
		// thread-safe block - start
		communication_lock(ctx);

		Request *req = mds_set_operational_state_of_the_scanner(ctx, handle, state, callback);

		communication_unlock(ctx);
		// thread-safe block - end
		return req;
	}

	return NULL;
}

/**
 * Returns configuration of medical device
 *
 * @param id context id
 * @return data list with attribute values
 */
DataList *manager_get_configuration(ContextId id)
{
	Context *ctx = context_get(id);

	if (!ctx)
		return NULL;

	MDS *mds = ctx->mds;

	if (!mds) {
		ERROR("No MDS data is available");
		return NULL;
	}

	DataList *list = mds_populate_configuration(mds);

	return list;
}

/**
 * Returns attributes from medical device since last updated.
 *
 * @param id context id
 * @return data list with attribute values
 */
DataList *manager_get_mds_attributes(ContextId id)
{
	Context *ctx = context_get(id);

	if (ctx != NULL) {
		MDS *mds = ctx->mds;

		if (mds == NULL) {
			ERROR("No MDS data is available");
			return NULL;
		}

		DataList *list = data_list_new(1);

		if (list == NULL) {
			return NULL;
		}

		mds_populate_attributes(mds, &list->values[0]);

		return list;
	}

	return NULL;
}

/**
 * Requests "measurement data transmission", if agent support this feature it
 * will start to send back measurement data.
 *
 * @param id context id
 * @param callback
 * @return pointer to request sent
 */
Request *manager_request_measurement_data_transmission(ContextId id, service_request_callback callback)
{

	Context *ctx = context_get(id);

	if (ctx != NULL) {
		// thread-safe block - start
		communication_lock(ctx);

		DataReqMode mode = DATA_REQ_START_STOP
				   | DATA_REQ_SUPP_SCOPE_CLASS
				   | DATA_REQ_SUPP_MODE_SINGLE_RSP;
		OID_Type class_id = MDC_MOC_VMO_METRIC_NU;
		Request *req = mds_service_action_data_request(ctx, mode, &class_id,
				NULL, callback);

		communication_unlock(ctx);
		// thread-safe block - end

		return req;
	}

	return NULL;
}

/**
 * Requests all attribute values of medical device
 *
 * @param id context id
 * @param callback
 * @return pointer to request sent
 */
Request *manager_request_get_all_mds_attributes(ContextId id, service_request_callback callback)
{

	Context *ctx = context_get(id);

	if (ctx != NULL) {
		// thread-safe block - start
		communication_lock(ctx);

		Request *req = mds_service_get(ctx, NULL, 0, callback);

		communication_unlock(ctx);
		// thread-safe block - end
		return req;
	}

	return NULL;
}

/**
 * Requests PM-Store attributes
 *
 * @param id context id
 * @param handle the PM-Store handle
 * @param callback
 * @return pointer to request sent
 */
Request *manager_request_get_pmstore(ContextId id, int handle,
					service_request_callback callback)
{
	Context *ctx = context_get(id);

	if (ctx != NULL) {
		// thread-safe block - start
		communication_lock(ctx);

		Request *req = mds_get_pmstore(ctx, handle, callback);

		communication_unlock(ctx);
		// thread-safe block - end
		return req;
	}

	return NULL;
}

/**
 * Requests segments information
 *
 * @param id context id
 * @param handle PM-Store handle
 * @param callback
 * @return pointer to request sent
 */
Request *manager_request_get_segment_info(ContextId id, int handle,
				service_request_callback callback)
{
	Context *ctx = context_get(id);

	if (ctx != NULL) {
		// thread-safe block - start
		communication_lock(ctx);

		Request *req;
		req = mds_service_get_segment_info(ctx, handle, callback);

		communication_unlock(ctx);
		// thread-safe block - end
		return req;
	}

	return NULL;
}

/**
 * Requests segments data
 *
 * @param id context id
 * @param handle PM-Store handle
 * @param instnumber PM-Instance number
 * @param callback
 * @return pointer to request sent
 */
Request *manager_request_get_segment_data(ContextId id,
					int handle,
					int instnumber,
					service_request_callback callback)
{
	Context *ctx = context_get(id);

	if (ctx != NULL) {
		// thread-safe block - start
		communication_lock(ctx);

		Request *req = mds_service_get_segment_data(ctx, handle, instnumber, callback);

		communication_unlock(ctx);
		// thread-safe block - end
		return req;
	}

	return NULL;
}

/**
 * Requests clear segments data
 *
 * @param id context id
 * @param handle PM-Store handle
 * @param callback
 * @return pointer to request sent
 */
Request *manager_request_clear_segments(ContextId id,
					int handle,
					service_request_callback callback)
{
	Context *ctx = context_get(id);

	if (ctx != NULL) {

		// thread-safe block - start
		communication_lock(ctx);

		Request *req = mds_service_clear_segment(ctx, handle, -1, callback);

		communication_unlock(ctx);
		// thread-safe block - end
		return req;
	}

	return NULL;
}


/**
 * Requests clear segments data
 *
 * @param id context id
 * @param handle PM-Store handle
 * @param instnumber PM-Instance number
 * @param callback
 * @return pointer to request sent
 */
Request *manager_request_clear_segment(ContextId id,
					int handle,
					int instnumber,
					service_request_callback callback)
{
	Context *ctx = context_get(id);

	if (ctx != NULL) {

		// thread-safe block - start
		communication_lock(ctx);

		Request *req = mds_service_clear_segment(ctx, handle, instnumber, callback);

		communication_unlock(ctx);
		// thread-safe block - end
		return req;
	}

	return NULL;
}


/**
 * Handle low level communication state transition events and notifies
 * high level application
 *
 * @param ctx context
 * @param previous the previous FSM state.
 * @param next the next FSM state.
 */
void manager_handle_transition_evt(Context *ctx, fsm_states previous, fsm_states next)
{
	if (previous == fsm_state_operating && next != previous) {
		DEBUG(" manager: Notify device unavailable.\n");
		// Exiting operating state
		manager_notify_evt_device_unavailable(ctx);

	}
}

// TODO static variable, functions to register from app
static const intu8 MANAGER_SYSTEM_ID[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
static const intu16 MANAGER_SYSTEM_ID_LEN = 8;

/**
 * Return length of manager system id
 *
 * @return length in octets
 */
unsigned short int manager_system_id_length()
{
	return MANAGER_SYSTEM_ID_LEN;
}

/**
 * Return manager system id
 *
 * @return copy of system id (callee gets ownership)
 */
intu8 *manager_system_id()
{
	intu16 len = manager_system_id_length();
	intu8 *id = malloc(len);
	memcpy(id, MANAGER_SYSTEM_ID, len);
	return id;
}


/**
 * Returns known PM-Store attributes
 *
 * @param id context id
 * @param handle PM-Store handle
 * @return data list with attribute values
 */
DataList *manager_get_pmstore_data(ContextId id, int handle)
{
	Context *ctx = context_get(id);

	if (!ctx)
		return NULL;

	DataList *list = pmstore_get_data_as_datalist(ctx, handle);

	return list;
}


/**
 * Returns known PM-Store segment info 
 *
 * @param id context id
 * @param handle PM-Store handle
 * @return data list with attribute values
 */
DataList *manager_get_segment_info_data(ContextId id, int handle)
{
	Context *ctx = context_get(id);

	if (!ctx)
		return NULL;

	DataList *list = pmstore_get_segment_info_data_as_datalist(ctx, handle);

	return list;
}

/** @} */
