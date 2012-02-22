/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file service.h
 * \brief Service module header.
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
 * \author Fabricio Silva, Mateus Lima
 * \date Jun 22, 2010
 */

/**
 * \defgroup Service Service
 * \brief Service is responsible to handle requests from manager.
 *
 * It waits for a request to be completed to start sending the other ones. It can handle 16 simultaneous
 * requests, queuing them until service is on READY state again. If its clients tries to make other
 * requests, these ones are dropped.
 *
 * It is responsible to delete APDU's structures after sending them. On doing so, all pointers
 * inside this structure must have been created on heap.

 * @{
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include <asn1/phd_types.h>
#include <communication/fsm.h>
#include <communication/communication.h>

/**
 * Service states
 */
typedef enum {
	READY = 0,  // !< Service is able to send an apdu
	PROCESSING, // !< Service waiting for a request to be retired
	FINALIZING, // !< Service being finalized
	FINALIZED   // !< Service finalized
} ServiceState;

/**
 * Function called when service state changes.
 */
typedef void (*service_state_callback_function)(Context *ctx, ServiceState new_state);

struct Request;

/**
 * Function to be called when the response to this request arrives
 */
typedef void (*service_request_callback)(Context *ctx, struct Request *r,
						DATA_apdu *response_apdu);

/**
 * Constant to represent an active service request
 */
static const intu16 REQUEST_VALID = 1;

/**
 * Constant to represent an inactive service request
 */
static const intu16 REQUEST_INVALID = 0;

/**
 * This error code means the invoke ID cannot be retrieved
 */
static const InvokeIDType INVOKE_ID_ERROR = -1;

/**
 * Function to be called to free request response, which
 * may be a subclass of RequestRet
 */
typedef void (*request_ret_data_free)(void *);

/**
 * Request response structure, "abstract class"
 * Actual responses are extensions of this.
 */
struct RequestRet {
	request_ret_data_free del_function;
};

/**
 * Request structure
 */
typedef struct Request {
	intu16 is_valid;
	APDU *apdu;
	timeout_callback timeout;
	service_request_callback request_callback;
	void *context;
	struct RequestRet *return_data;
} Request;

/**
 * Service Context
 */
typedef struct Service {
	ServiceState state;
	InvokeIDType last_invoke_id;
	InvokeIDType current_invoke_id;

	int requests_count;
	Request requests_list[16];

	service_state_callback_function state_changed_callback;
} Service;

Service *service_instance();

void service_destroy(Service *serive);

void service_init(Context *ctx);

void service_del_request(Request *req);

Request  *service_send_remote_operation_request(Context *ctx, APDU *apdu, timeout_callback timeout,  service_request_callback request_callback);

Request *service_trans_request(Context *ctx, service_request_callback request_callback);

void service_send_unconfirmed_operation_request(Context *ctx, APDU *apdu);

InvokeIDType service_get_invoke_id(Context *ctx, Request *req);

InvokeIDType service_get_current_invoke_id(Context *ctx);

Request *service_get_request(Context *ctx, InvokeIDType invoke_id);

void service_request_retired(Context *ctx, DATA_apdu *response_apdu);

void service_finalize(Context *ctx, service_state_callback_function state_changed_callback);

int service_is_id_valid(Context *ctx, InvokeIDType invoke_id);

Request *service_check_known_invoke_id(Context *ctx, DATA_apdu *data_apdu);

void service_configure_attribute(Context *ctx, OID_Type obj_class, HANDLE obj_handle, OID_Type attribute_id, Any attribute_value);

/** @} */

#endif /* SERVICE_H_ */
