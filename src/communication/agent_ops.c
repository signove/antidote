/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file agent_ops.c
 * \brief Finite State Machine agent operations
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
 *
 * IEEE 11073 Communication Model - Finite State Machine implementation
 *
 * \author Elvis Pfutzenreuter
 * \date May 31, 2011
 */

#include "fsm.h"
#include "communication.h"
#include "communication/service.h"
#include "communication/stdconfigurations.h"
#include "communication/parser/encoder_ASN1.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/dim/mds.h"
#include "src/util/log.h"
#include "src/agent_p.h"
#include <string.h>

/**
 * Answer to unexpected AARQ (Agent)
 *
 * @param ctx state machine context
 * @param evt state machine event
 * @param data state machine event data
 */
void communication_agent_aare_rejected_permanent_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	/* TODO */
}

/**
 * Answer to unexpected ROIV (e.g. in config phase) (Agent)
 *
 * @param ctx state machine context
 * @param evt state machine event
 * @param data state machine event data
 */
void communication_agent_roer_no_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	/* TODO */
}

/**
 * Respond to a ROIV that is not ROIV GET
 *
 * @param ctx state machine context
 * @param evt state machine event
 * @param data state machine event data
 */
void communication_agent_roiv_respond_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	/* TODO */
}

/**
 * Respond to a ROIV event report
 *
 * @param ctx state machine context
 * @param evt state machine event
 * @param data state machine event data
 */
void communication_agent_roiv_event_report_respond_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	/* TODO */
}

/**
 * Respond to a confirmed ROIV event report
 *
 * @param ctx state machine context
 * @param evt state machine event
 * @param data state machine event data
 */
void communication_agent_roiv_confirmed_error_report_respond_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	/* TODO */
	/* FIXME function name */
}

/**
 * Respond to ROIV SET request (Agent)
 *
 * @param ctx state machine context
 * @param evt state machine event
 * @param data state machine event data
 */
void communication_agent_roiv_set_respond_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	/* TODO */
}

/**
 * Respond to confirmed ROIV SET request (Agent)
 *
 * @param ctx state machine context
 * @param evt state machine event
 * @param data state machine event data
 */
void communication_agent_roiv_confirmed_set_respond_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	/* TODO */
}

/**
 * Respond to confirmed ROIV Action request (Agent)
 *
 * @param ctx state machine context
 * @param evt state machine event
 * @param data state machine event data
 */
void communication_agent_roiv_confirmed_action_respond_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	/* TODO */
}

/**
 * Respond to ROIV Action request (Agent)
 *
 * @param ctx state machine context
 * @param evt state machine event
 * @param data state machine event data
 */
void communication_agent_roiv_action_respond_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	/* TODO */
}

/**
 * React to "Send Event" request to state machine (Agent)
 *
 * @param ctx state machine context
 * @param evt state machine event
 * @param evtdata state machine event data
 */
void communication_agent_send_event_tx(FSMContext *ctx, fsm_events evt, FSMEventData *evtdata)
{
	APDU *apdu = calloc(sizeof(APDU), 1);
	PRST_apdu prst;
	DATA_apdu *data;

	struct StdConfiguration *cfg =
		std_configurations_get_supported_standard(agent_specialization);

	if (!cfg) {
		DEBUG("No std configuration for %d, bailing out", agent_specialization);
		return;
	}

	void *evtreport = agent_event_report_cb();
	data = cfg->event_report(evtreport);
	free(evtreport);

	// prst = length + DATA_apdu
	// take into account data's invoke id and choice
	prst.length = data->message.length + 6; // 46 + 6 = 52 for oximeter
	encode_set_data_apdu(&prst, data);
	
	apdu->choice = PRST_CHOSEN;
	apdu->length = prst.length + 2; // 52 + 2 = 54 for oximeter
	apdu->u.prst = prst;

	// passes ownership
	if (data->message.choice == ROIV_CMIP_EVENT_REPORT_CHOSEN) {
		service_send_unconfirmed_operation_request(ctx, apdu);
	} else {
		// ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN, presumably
		timeout_callback tm = {.func = &communication_timeout, .timeout = 3};
		service_send_remote_operation_request(ctx, apdu, tm, NULL);
	}
}

/**
 * Execute association data protocol 20601 - agent
 *
 * @param ctx Context
 * @param evt State events
 * @param data Event data
 */
void association_agent_mds(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	DEBUG("association_agent_mds");

	if (ctx->mds != NULL) {
		mds_destroy(ctx->mds);
	}

	ConfigObjectList *cfg = std_configurations_get_configuration_attributes(agent_specialization);

	MDS *mds = mds_create();
	ctx->mds = mds;

	struct mds_system_data *mds_data = agent_mds_data_cb();

	mds->dev_configuration_id = agent_specialization;
	mds->data_req_mode_capab.data_req_mode_flags = DATA_REQ_SUPP_INIT_AGENT;
	// max number of simultaneous sessions
	mds->data_req_mode_capab.data_req_init_agent_count = 1;
	mds->data_req_mode_capab.data_req_init_manager_count = 0;
	mds->system_id.length = 8;
	mds->system_id.value = (intu8*) malloc(mds->system_id.length);
	memcpy(mds->system_id.value, &mds_data->system_id, mds->system_id.length);

	mds_configure_operating(ctx, cfg, 0);

	free(mds_data);
	free(cfg);
}

/**
 * Answer ROIV GET MDS - agent
 *
 * @param ctx Context
 * @param evt State events
 * @param data Event data
 */
void communication_agent_roiv_get_mds_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	DATA_apdu *rx = encode_get_data_apdu(&data->received_apdu->u.prst);
	InvokeIDType id = rx->invoke_id;

	if (rx->message.choice != ROIV_CMIP_GET_CHOSEN) {
		return;
	}

	if (rx->message.u.roiv_cmipGet.obj_handle != MDS_HANDLE) {
		communication_fire_evt(ctx, fsm_evt_rx_roiv, NULL);
		return;
	}

	DEBUG("send RORS with MDS");

	APDU apdu;
	apdu.choice = PRST_CHOSEN;

	DATA_apdu *data_apdu = calloc(sizeof(DATA_apdu), 1);
	data_apdu->invoke_id = id;
	data_apdu->message.choice = RORS_CMIP_GET_CHOSEN;

	AttributeList attrs;

	attrs.count = 0;
	attrs.length = 0;
	attrs.value = mds_get_attributes(ctx->mds, &attrs.count, &attrs.length);
	
	DEBUG("send RORS with MDS: %d attributes, length %d", attrs.count, attrs.length);

	data_apdu->message.u.rors_cmipGet.obj_handle = MDS_HANDLE;
	data_apdu->message.u.rors_cmipGet.attribute_list = attrs;

	data_apdu->message.length = sizeof(data_apdu->message.u.rors_cmipGet.obj_handle) +
					2 + 2 + attrs.length;

	apdu.u.prst.length = sizeof(id)
			     + sizeof(data_apdu->message.choice)
			     + sizeof(data_apdu->message.length)
			     + data_apdu->message.length;

	apdu.length = sizeof(apdu.u.prst.length) + apdu.u.prst.length;
	encode_set_data_apdu(&apdu.u.prst, data_apdu);
	communication_send_apdu(ctx, &apdu);

	// deletes information that came from mds_get_attributes()
	// and data_apdu
	del_apdu(&apdu);
}

/** @} */
