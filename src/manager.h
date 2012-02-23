/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file manager.h
 * \brief Protocol manager public definitions.
 *
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
 * \author Adrian Guedes
 * \author Fabricio Silva
 * \date Jun 22, 2010
 */

#ifndef MANAGER_H_
#define MANAGER_H_

#include <api/api_definitions.h>
#include <communication/context.h>
#include <communication/plugin/plugin.h>
#include <communication/service.h>

/**
 * Manager event listener definition
 */
typedef struct ManagerListener {
	/**
	 *  Called when Medical Measurement is received and stored
	 */
	void (*measurement_data_updated)(Context *ctx, DataList *list);
	/**
	 *  Called when PM-Segment data event is received. In this case,
	 *  DataList ownership is passed to the caller.
	 */
	void (*segment_data_received)(Context *ctx, int handle, int instnumber,
					DataList *list);
	/**
	 * Called after device is operational
	 */
	void (*device_available)(Context *ctx, DataList *list);
	/**
	 * Called after device is not operational
	 */
	void (*device_unavailable)(Context *ctx);
	/**
	 * Called after timeout occurs
	 */
	void (*timeout)(Context *ctx);
} ManagerListener;

#define MANAGER_LISTENER_EMPTY {\
			.measurement_data_updated = NULL,\
			.segment_data_received = NULL, \
			.device_available = NULL,\
			.device_unavailable = NULL,\
			.timeout = NULL\
			}

void manager_init(CommunicationPlugin **plugins);

void manager_finalize();

void manager_start();

void manager_stop();

void manager_connection_loop(ContextId context_id);

int manager_add_listener(ManagerListener listener);

DataList *manager_get_mds_attributes(ContextId id);

Request *manager_request_measurement_data_transmission(ContextId id, service_request_callback callback);

Request *manager_request_get_all_mds_attributes(ContextId id, service_request_callback callback);

Request *manager_request_get_pmstore(ContextId id, int handle, service_request_callback callback);

DataList *manager_get_pmstore_data(ContextId id, int handle);

Request *manager_request_get_segment_info(ContextId id, int handle, service_request_callback callback);

DataList *manager_get_segment_info_data(ContextId id, int handle);

Request *manager_set_operational_state_of_the_scanner(ContextId id, HANDLE handle, OperationalState state,
		service_request_callback callback);

Request *manager_request_get_segment_data(ContextId id, int handle, int instnumber, service_request_callback callback);

Request *manager_request_clear_segment(ContextId id, int handle, int instnumber,
					 service_request_callback callback);

Request *manager_request_clear_segments(ContextId id, int handle, service_request_callback callback);

DataList *manager_get_configuration(ContextId id);

void manager_request_association_release(ContextId id);

void manager_request_association_abort(ContextId id);

#endif /* MANAGER_H_ */
