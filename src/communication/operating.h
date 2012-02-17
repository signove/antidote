/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file operating.h
 * \brief Operating module header.
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
 * \author Diego Bezerra, Mateus Lima
 * \date Jun 23, 2010
 */

/**
 * \defgroup Operating Operating
 * \ingroup FSM
 * @{
 */

#ifndef OPERATING_H_
#define OPERATING_H_

#include "src/asn1/phd_types.h"
#include "src/communication/fsm.h"
#include "src/communication/service.h"

/*
typedef struct EventReportResult {
	InvokeIDType invoke_id;
	HANDLE obj_handle;
	RelativeTime currentTime;
	OID_Type event_type;
	Any event_reply_info;
} EventReportResult;
*/

/**
 * Process the incoming APDU calling to proper procedure
 *
 * @param ctx current context.
 * @param apdu
 */
void operating_process_apdu(Context *ctx, APDU *apdu);

void operating_process_apdu_agent(Context *ctx, APDU *apdu);

Request *operating_service_get(Context *ctx, HANDLE handle, OID_Type *attributeids_list,
				int attributeids_list_count, intu32 timeout,
				service_request_callback request_callback);

void operating_get_response(Context *ctx, fsm_events evt, FSMEventData *data);

void operating_set_scanner_response(Context *ctx, fsm_events evt, FSMEventData *data);

Request *operating_action_set_time(Context *ctx, SetTimeInvoke *time, intu32 timeout, service_request_callback request_callback);

void operating_event_report(Context *ctx, fsm_events evt, FSMEventData *data);

void operating_event_report_response_tx(Context *ctx, InvokeIDType invoke_id, HANDLE obj_handle,
					RelativeTime currentTime, OID_Type event_type,
					Any event_reply_info);

void operating_segment_data_event_response_tx(Context *ctx, InvokeIDType invoke_id, HANDLE obj_handle,
		RelativeTime currentTime, OID_Type event_type, SegmentDataResult result);

void operating_assoc_release_req_tx(Context *ctx, fsm_events evt, FSMEventData *data);

void operating_decode_mds_event(Context *ctx, OID_Type event_type, Any *event);

void operating_decode_segment_info(struct MDS *mds, Any *event, HANDLE obj_handle, Request *r);

void operating_decode_segment_data_event(Context *ctx, InvokeIDType invoke_id, HANDLE obj_handle,
		RelativeTime currentTime, OID_Type event_type, Any *event);

void operating_rors_confirmed_action_tx(Context *ctx, fsm_events evt, FSMEventData *data);

Request *operating_set_scanner(Context *ctx, HANDLE handle, OperationalState state, intu32 timeout,
			       service_request_callback callback);

/** @} */

#endif /* OPERATING_H_ */
