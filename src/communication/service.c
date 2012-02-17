/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file service.c
 * \brief Service module source.
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
 * \ingroup Service
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include "src/communication/service.h"
#include "src/communication/communication.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/util/log.h"

static void service_change_state(Context *ctx, ServiceState new_state);
static void service_send_apdu_now(Context *ctx, APDU *apdu, timeout_callback timeout);
static void service_release_resources(Context *ctx);


/**
 * Construct service structure
 *
 * @return service structure
 */
Service *service_instance()
{
	Service *s = calloc(1, sizeof(struct Service));
	return s;
}

/**
 * Sanitize Request struct
 * @param req Request
 */
void clean_request(Request *req)
{
	req->is_valid = REQUEST_INVALID;
	req->apdu = NULL;
	req->timeout.func = NULL;
	req->timeout.timeout = 0;
	req->timeout.id = 0;
	req->request_callback = NULL;
	if (req->context) {
		free(req->context);
		req->context = NULL;
	}
	if (req->return_data) {
		if (req->return_data->del_function) {
			(req->return_data->del_function)(req->return_data);
		}
		free(req->return_data);
		req->return_data = NULL;
	}
}

/**
 * Initializes service.
 * @param ctx Current context.
 */
void service_init(Context *ctx)
{
	int i;

	if (ctx->service != NULL) {
		service_destroy(ctx->service);
	}

	ctx->service = service_instance();
	ctx->service->state = READY;
	ctx->service->last_invoke_id = 0xF;
	ctx->service->current_invoke_id = 0;
	ctx->service->requests_count = 0;

	// Make sure unused requests are clean
	for (i = 0; i < 15; ++i) {
		clean_request(&ctx->service->requests_list[i]);
	}

}

/**
 * Destroys Service.
 *
 * @param service Service to be destroyed.
 */
void service_destroy(Service *service)
{
	if (service != NULL) {
		int i = 0;

		for (i = 0; i < 15; ++i) {
			service_del_request(&service->requests_list[i]);
		}

		free(service);
	}
}


/**
 * Release resources.
 *
 * @param ctx Current context.
 */
static void service_release_resources(Context *ctx)
{
	Service *service = ctx->service;
	int i;

	for (i = 0; i < 16; ++i) {
		if (service->requests_list[i].is_valid == REQUEST_VALID) {
			service->requests_list[i].is_valid = REQUEST_INVALID;
			service_del_request(&service->requests_list[i]);
			service->requests_count--;
		}
	}

	service_change_state(ctx, FINALIZED);
	service_change_state(ctx, READY);
	service->last_invoke_id = 0xF;
	service->current_invoke_id = 0;
	service->requests_count = 0;
}

/**
 * Create and retrieve a new invoke id.
 *
 * @param ctx Current context.
 * @return The new invoke id.
 */
InvokeIDType service_get_new_invoke_id(Context *ctx)
{
	if (ctx != NULL) {
		Service *service = ctx->service;
		service->last_invoke_id = (service->last_invoke_id + 1) & 0xF;
		return service->last_invoke_id;
	}

	return INVOKE_ID_ERROR;
}

/**
 * Tries to send the Remote Operation Invoke apdu through communication layer. If processing another request,
 * it queues this request and send it later.
 *
 * @param apdu Pointer to an APDU to be sent through communication.
 * All structures inside APDU must have been created on heap.
 * @param ctx Current context.
 * @param timeout A timeout function for this request.
 * @param request_callback Request callback function.
 *
 * @return the request created
 */
Request *service_send_remote_operation_request(Context *ctx, APDU *apdu, timeout_callback timeout,  service_request_callback request_callback)
{
	Service *service = ctx->service;

	if (apdu->choice == PRST_CHOSEN) {
		if (service->requests_count < 16) {
			DATA_apdu *data_apdu = encode_get_data_apdu(&apdu->u.prst);
			data_apdu->invoke_id = service_get_new_invoke_id(ctx);
			Request *req = &service->requests_list[service->last_invoke_id];

			req->apdu = apdu;
			req->timeout = timeout;
			req->is_valid = REQUEST_VALID;
			req->request_callback = request_callback;

			service->requests_count++;

			if (service->state == READY) {
				service_send_apdu_now(ctx, apdu, timeout);
			}

			return req;
		} else {
			del_apdu(apdu);
			free(apdu);
			return NULL;
		}
	}

	return NULL;
}

/**
 * Callback for requests related to transcoded devices.
 * Transcoded devices don't implement 11073, so there is no
 * actual 11073 request going through the wire. FLow is
 * simulated by scheduling this callback as a 0ms timeout.
 *
 * @param ctx Current context.
 */
void service_trans_request_cb(Context *ctx)
{
	int invoke_id = (16
			+ ctx->service->last_invoke_id
			- ctx->service->requests_count + 1)
			% 16;

	Request *req = &ctx->service->requests_list[invoke_id];
	req->request_callback(ctx, req, 0);
	req->is_valid = REQUEST_INVALID;
	service_del_request(req);
	ctx->service->requests_count--;
}

/**
 * Returns Request* faking Remote Operation Invoke and schedules the callback
 *
 * All structures inside APDU must have been created on heap.
 * @param ctx Current context.
 * @param request_callback Request callback function.
 *
 * @return the request created
 */
Request *service_trans_request(Context *ctx, service_request_callback request_callback)
{
	if (ctx->service->requests_count >= 16) {
		return 0;
	}

	int invoke_id = service_get_new_invoke_id(ctx);

	// fabricate a request
	Request *req = &ctx->service->requests_list[invoke_id];
	req->apdu = 0;
	req->timeout.func = NULL;
	req->timeout.timeout = 0;
	req->is_valid = REQUEST_VALID;
	req->request_callback = request_callback;

	ctx->service->requests_count++;

	// make it wait for the next event loop cycle
	communication_count_timeout(ctx, service_trans_request_cb, 0);

	return req;
}

/**
 * Tries to send unconfirmed Remote Operation Invoke apdu through communication layer.
 *
 * @param apdu Pointer to an APDU to be sent through communication. Does not take ownership
 * @param ctx Current context.
 */
void service_send_unconfirmed_operation_request(Context *ctx, APDU *apdu)
{
	// Service *service = ctx->service;

	if (apdu->choice == PRST_CHOSEN) {
		DATA_apdu *data_apdu = encode_get_data_apdu(&apdu->u.prst);
		data_apdu->invoke_id = 0x1111;
		communication_send_apdu(ctx, apdu);
	}

	del_apdu(apdu);
	free(apdu);
}

/**
 * Returns current invoke id expected or in process
 *
 * @param ctx Current context.
 * @return Current invoke id.
 */
InvokeIDType service_get_current_invoke_id(Context *ctx)
{
	return ctx->service->current_invoke_id;
}

/**
 * Returns the invoke ID of this  request
 * @param ctx Current context.
 * @param req service request
 * @return the invoke id of this request, if the apdu is not valid return INVOKE_ID_ERROR
 */
InvokeIDType service_get_invoke_id(Context *ctx, Request *req)
{
	if (req != NULL) {
		int is_valid_data_apdu = req->apdu->choice == PRST_CHOSEN
					 && req->apdu->u.prst.length > 0
					 && req->apdu->u.prst.value != NULL;

		if (is_valid_data_apdu) {
			return encode_get_data_apdu(&req->apdu->u.prst)->invoke_id;
		}
	}

	return INVOKE_ID_ERROR;
}

/**
 * Delete the APDU from the request.
 *
 * @param req The deleted APDU.
 */
void service_del_request(Request *req)
{
	if (req != NULL) {
		if (req->apdu != NULL) {
			del_apdu(req->apdu);
			free(req->apdu);
		}
		clean_request(req);
	}
}

/**
 * Request to be retired from requests queue. After removing, if the request queue still have
 * pending requests, this function starts sending the next request.
 *
 * @param ctx Current context.
 * @param response_apdu Response APDU
 */
void service_request_retired(Context *ctx, DATA_apdu *response_apdu)
{
	Service *service = ctx->service;

	if (response_apdu->invoke_id < 0 || response_apdu->invoke_id > 15) {
		DEBUG("APDU invoke id out of rante");
		return;
	}

	Request *req = &(service->requests_list[response_apdu->invoke_id]);

	if (response_apdu->invoke_id != service->current_invoke_id) {
		return;
	}

	if (req != NULL && req->is_valid == REQUEST_VALID) {

		communication_reset_timeout(ctx);

		InvokeIDType retiredInvokeID = response_apdu->invoke_id;
		req->is_valid = REQUEST_INVALID;

		if (req->request_callback != NULL) {
			(req->request_callback)(ctx, req, response_apdu);
		}

		service_del_request(req);
		service->requests_count--;


		if (service->state == PROCESSING) {
			service_change_state(ctx, READY);
			InvokeIDType next_id_to_process = (retiredInvokeID + 1) & 0xF;
			service->current_invoke_id = next_id_to_process;

			if (service->requests_count > 0) {
				if (service->requests_list[next_id_to_process].is_valid == REQUEST_VALID) {
					service_send_apdu_now(ctx, service->requests_list[next_id_to_process].apdu,
							      service->requests_list[next_id_to_process].timeout);
				}
			}
		}

		if (service->state == FINALIZING) {
			service_release_resources(ctx);
		}
	}
}

/**
 * Release resources if state is on READY state or changes service to finalizing state,
 * waiting for current processing request to be retired, releasing resources after all.
 * On finalizing, service calls the service_state_callback_function, reporting that it
 * is on finalized state, moving to READY state after that.
 *
 * @param ctx current context.
 * @param state_changed_callback A function to be called when service state changes.
 */
void service_finalize(Context *ctx, service_state_callback_function state_changed_callback)
{
	Service *service = ctx->service;
	service->state_changed_callback = state_changed_callback;
	ServiceState previous_state = service->state;

	service_change_state(ctx, FINALIZING);

	if (previous_state == READY) {
		service_release_resources(ctx);
	}
}

/**
 * Send an APDU. If the APDU is not sent, a callback function is called.
 *
 * @param ctx Current context.
 * @param apdu The APDU sent
 * @param timeout The timeout callback
 */
static void service_send_apdu_now(Context *ctx, APDU *apdu, timeout_callback timeout)
{
	communication_send_apdu(ctx, apdu);
	communication_count_timeout(ctx, timeout.func, timeout.timeout);
	service_change_state(ctx, PROCESSING);
}

/**
 * Check if an id is valid
 *
 * @param ctx Current context.
 * @param invoke_id ID to be checked.
 *
 * @return 1 If valid and 0 if not.
 */
int service_is_id_valid(Context *ctx, InvokeIDType invoke_id)
{
	if (invoke_id > 15)
		return 0;

	return ctx->service->requests_list[invoke_id].is_valid == REQUEST_VALID;
}

/**
 * Returns request by invoke id.
 *
 * @param ctx Current context.
 * @param invoke_id The ID of invoke ID type.
 * @return the request, null if cannot not found.
 */
Request *service_get_request(Context *ctx, InvokeIDType invoke_id)
{
	if (invoke_id > 15)
		return NULL;

	return &ctx->service->requests_list[invoke_id];
}

/**
 * Change the state of the service.
 *
 * @param ctx Current context.
 * @param new_state New state of the service.
 */
static void service_change_state(Context *ctx, ServiceState new_state)
{
	Service *service = ctx->service;
	service->state = new_state;

	if (service->state_changed_callback != NULL) {
		service->state_changed_callback(ctx, new_state);
	}
}

/**
 * Checks if the data APDU has a known invoke ID, if not
 * an "REQ association abort" will be executed.
 *
 * @param ctx Current context.
 * @param data_apdu Data APDU
 *
 * @return 1 if invoke id is valid, 0 if not, in this case
 *      and association will be aborted
 */
Request *service_check_known_invoke_id(Context *ctx, DATA_apdu *data_apdu)
{
	if (!service_is_id_valid(ctx, data_apdu->invoke_id)) {
		communication_fire_evt(ctx, fsm_evt_req_assoc_abort, NULL);
		return 0;
	}

	return service_get_request(ctx, data_apdu->invoke_id);
}

/** @} */

