/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file association.c
 * \brief Association module source.
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
 * \date Jun 23, 2010
 */

/**
 * \defgroup Association Association
 * \brief Association module.
 * \ingroup FSM
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/manager_p.h"
#include "src/agent_p.h"
#include "src/communication/stdconfigurations.h"
#include "src/communication/extconfigurations.h"
#include "src/communication/association.h"
#include "src/communication/communication.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/util/bytelib.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/dim/mds.h"
#include "src/util/log.h"


/**
 * Retry count association constant
 */
static const intu32 ASSOCIATION_RC = 3;

/**
 * Time out (seconds) association constant
 */
static const intu32 ASSOCIATION_TO = 10;

static int association_check_config_id(PhdAssociationInformation *info);

static void association_process_aarq_apdu(Context *ctx, APDU *apdu);

static void association_process_aare_apdu(Context *ctx, APDU *apdu);

static void association_accept_data_protocol_20601(Context *ctx, DataProto *proto);

static void populate_aare(APDU *apdu, PhdAssociationInformation *response_info);

/**
 * Process incoming APDU's in unassociated state.
 *
 * @param ctx the current context.
 * @param apdu received APDU.
 */
void association_unassociated_process_apdu(Context *ctx, APDU *apdu)
{
	switch (apdu->choice) {
	case AARQ_CHOSEN:
		association_process_aarq_apdu(ctx, apdu);
		break;
	case AARE_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_aare, NULL);
		break;
	case RLRQ_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_rlrq, NULL);
		break;
	case PRST_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_prst, NULL);
		break;
	case ABRT_CHOSEN: // ignore
		break;
	case RLRE_CHOSEN: // ignore
		break;
	default:
		// TODO error handling
		break;
	}
}

/**
 * Process incoming APDU's in unassociated state - agent
 *
 * @param ctx the current context.
 * @param apdu received APDU.
 */
void association_unassociated_process_apdu_agent(Context *ctx, APDU *apdu)
{
	switch (apdu->choice) {
	case AARQ_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_aarq, NULL);
		break;
	case AARE_CHOSEN:
		association_process_aare_apdu(ctx, apdu);
		break;
	case RLRQ_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_rlrq, NULL);
		break;
	case PRST_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_prst, NULL);
		break;
	case ABRT_CHOSEN: // ignore
		break;
	case RLRE_CHOSEN: // ignore
		break;
	default:
		// TODO error handling
		break;
	}
}

/**
 * Process the association request apdu
 *
 * @param apdu
 */
static void association_process_aarq_apdu(Context *ctx, APDU *apdu)
{
	DEBUG(" associating: processing association request ");

	if (apdu != NULL && apdu->u.aarq.assoc_version == ASSOC_VERSION1) {
		DEBUG("associating: association protocol version accepted ");
		int is_no_common_protocol_accepted = 1;

		intu8 proto_arr_count = apdu->u.aarq.data_proto_list.count;
		DataProto *proto = NULL;
		int i = 0;

		for (i = 0; i < proto_arr_count && is_no_common_protocol_accepted; i++) {

			proto = &apdu->u.aarq.data_proto_list.value[i];

			switch (proto->data_proto_id) {
			case DATA_PROTO_ID_20601:
				// Accept protocol
				is_no_common_protocol_accepted = 0;

				association_accept_data_protocol_20601(ctx, proto);

				break;
			case DATA_PROTO_ID_EXTERNAL:
				// Outside ISO/IEEE 11073 family of standards
				DEBUG("\n associating:"
				      " association external protocol"
				      " id <%d> of Agent not supported ", proto->data_proto_id);
				break;
			default:
				DEBUG("\n associating: "
				      "Association protocol id <%d> of"
				      " Agent not supported ", proto->data_proto_id);
			}
		}

		if (is_no_common_protocol_accepted) {
			// fire Event REJECTED_NO_COMMON_PROTOCOL;
			FSMEventData data;
			data.choice = FSM_EVT_DATA_ASSOCIATION_RESULT_CHOSEN;
			data.u.association_result = REJECTED_NO_COMMON_PROTOCOL;

			communication_fire_evt(ctx, fsm_evt_rx_aarq_unacceptable_configuration,
					       &data);
		}

	} else {
		// fire Event REJECTED_UNSUPPORTED_ASSOC_VERSION;

		FSMEventData data;
		data.choice = FSM_EVT_DATA_ASSOCIATION_RESULT_CHOSEN;
		data.u.association_result = REJECTED_UNSUPPORTED_ASSOC_VERSION;

		communication_fire_evt(ctx, fsm_evt_rx_aarq_unacceptable_configuration,
				       &data);
	}

	// TODO check rejected conditions
	// REJECTED_UNAUTHORIZED;
	// REJECTED_TRANSIENT;
	// REJECTED_PERMANENT;
	// REJECTED_UNKNOWN;

}

/**
 * Process the association response apdu
 *
 * @param apdu
 */
static void association_process_aare_apdu(Context *ctx, APDU *apdu)
{
	DEBUG(" associating: processing aare ");

	if (!apdu)
		return;

	if (apdu->u.aare.result == ACCEPTED) {
		communication_fire_evt(ctx, fsm_evt_rx_aare_accepted_known, NULL);
	} else if (apdu->u.aare.result == ACCEPTED_UNKNOWN_CONFIG) {
		communication_fire_evt(ctx, fsm_evt_rx_aare_accepted_unknown, NULL);
	} else {
		DEBUG("associating: aare rejection code %d", apdu->u.aare.result);
		communication_fire_evt(ctx, fsm_evt_rx_aare_rejected, NULL);
	}
}

/**
 * Execute association data protocol 20601 -- inner
 * 
 * @param ctx
 * @param agent_assoc_information Agent Association Information
 * @param trans 0 if regular agent, 1 if transcoded 'agent'
 * @return 1 if incompatible protocol version, 2 if config known,
 *         3 if config unknown
 */
int association_accept_data_protocol_20601_in(Context *ctx,
					PhdAssociationInformation agent_assoc_information,
					int trans)
{
	FSMEventData evt;
	FSMEventData *evtp = NULL;

	if (! trans) {
		// transcoded events don't need apdu responses
		// and they are muted by omitting FSMEventData
		evtp = &evt;
	}

	if (agent_assoc_information.protocolVersion != PROTOCOL_VERSION1) {
		// fire Event REJECTED_NO_COMMON_PARAMETER;
		evt.choice = FSM_EVT_DATA_ASSOCIATION_RESULT_CHOSEN;
		evt.u.association_result = REJECTED_NO_COMMON_PARAMETER;
		communication_fire_evt(ctx, fsm_evt_rx_aarq_unacceptable_configuration, evtp);
		return 1;
	}

	evt.choice = FSM_EVT_DATA_ASSOCIATION_RESULT_CHOSEN;

	DEBUG("associating: accepted data protocol version <%.2X>", \
	      PROTOCOL_VERSION1);

	if (ctx->mds != NULL) {
		mds_destroy(ctx->mds);
	}

	MDS *mds = mds_create();
	ctx->mds = mds;

	mds->dev_configuration_id = agent_assoc_information.dev_config_id;
	mds->data_req_mode_capab = agent_assoc_information.data_req_mode_capab;

	mds->system_id.length = agent_assoc_information.system_id.length;
	mds->system_id.value = malloc(agent_assoc_information.system_id.length
					      * sizeof(intu8));

	if (mds->system_id.value != NULL) {
		memcpy(mds->system_id.value,
		       agent_assoc_information.system_id.value,
		       mds->system_id.length * sizeof(intu8));
	}

	if ((! trans) && association_check_config_id(&agent_assoc_information)) {
		// Configuration known
		ConfigId id = agent_assoc_information.dev_config_id;
		ConfigObjectList *config;

		if (std_configurations_is_supported_standard(id)) {
			config = std_configurations_get_configuration_attributes(id);
		} else {
			config = ext_configurations_get_configuration_attributes(
					 &agent_assoc_information.system_id, id);
		}

		if (config) {
			evt.u.association_result = ACCEPTED;
			communication_fire_evt(ctx,
				       fsm_evt_rx_aarq_acceptable_and_known_configuration,
				       evtp);

			// must be called AFTER communication_fire_evt()
			// because the manager may do something like request
			// MDS attributes and the request must go after
			// "configuration accepted" packet.
			mds_configure_operating(ctx, config, 1);
			free(config);

			return 2;
		}
	}

	// associated, configuration unknown
	evt.u.association_result = ACCEPTED_UNKNOWN_CONFIG;
	communication_fire_evt(ctx,
				fsm_evt_rx_aarq_acceptable_and_unknown_configuration,
				evtp);
	return 3;
}

/**
 * Execute association data protocol 20601
 *
 * @param ctx
 * @param proto information from agent
 */
static void association_accept_data_protocol_20601(Context *ctx, DataProto *proto)
{
	int error = 0;

	DEBUG("associating: accepted data protocol id <%d>", \
	      DATA_PROTO_ID_20601);

	intu8 *data_buffer = proto->data_proto_info.value;
	intu8 data_length = proto->data_proto_info.length;

	ByteStreamReader *stream = byte_stream_reader_instance(data_buffer, data_length);

	PhdAssociationInformation agent_assoc_information;
	memset(&agent_assoc_information, 0, sizeof(PhdAssociationInformation));

	decode_phdassociationinformation(stream, &agent_assoc_information, &error);

	if (!error) {
		association_accept_data_protocol_20601_in(ctx, agent_assoc_information, 0);
		del_phdassociationinformation(&agent_assoc_information);
	}

	free(stream);
}

/**
 * Check if association is supported without configuration
 *
 * @param info
 * @return 1 if true, 0 otherwise
 */
static int association_check_config_id(PhdAssociationInformation *info)
{
	int result = std_configurations_is_system_id_supported(info->system_id);
	result &= std_configurations_is_supported_standard(info->dev_config_id)
		  | ext_configurations_is_supported_standard(&info->system_id,
				  info->dev_config_id);
	return result;
}

/**
 * Send apdu accepting configuration
 *
 * @param ctx connection context
 * @param evt input event
 * @param data should inform the association result to use:
 *		known or unknown configuration.
 *
 */
void association_accept_config_tx(Context *ctx, fsm_events evt,
				  FSMEventData *data)
{
	if (data != NULL && data->choice == FSM_EVT_DATA_ASSOCIATION_RESULT_CHOSEN) {

		if (data->u.association_result == ACCEPTED) {
			DEBUG("associating: known configuration ");
		} else if (data->u.association_result == ACCEPTED_UNKNOWN_CONFIG) {
			DEBUG("associating: unknown configuration ");
		} else {
			ERROR("associating: invalid association result"
			      " to accept configuration ");
			exit(1);
		}

		APDU response_apdu;
		PhdAssociationInformation response_info;
		memset(&response_info, 0, sizeof(PhdAssociationInformation));

		populate_aare(&response_apdu, &response_info);

		response_apdu.u.aare.result = data->u.association_result;

		// Encode APDU
		ByteStreamWriter *encoded_value = byte_stream_writer_instance(
				response_apdu.u.aare.selected_data_proto.data_proto_info.length);

		encode_phdassociationinformation(encoded_value, &response_info);

		response_apdu.u.aare.selected_data_proto.data_proto_info.value
		= encoded_value->buffer;

		int ret_code = communication_send_apdu(ctx, &response_apdu);

		del_phdassociationinformation(&response_info);
		del_byte_stream_writer(encoded_value, 1);

		if (ret_code == 0) {
			ERROR("Could not send association response ");
		}
	}
}

/**
 * Send apdu rejecting configuration
 *
 * @param ctx connection context
 * @param evt input event
 * @param data should inform the association result, e.g.:reject
 *
 * @return 1 if operation succeeds, 0 otherwise
 */
void association_unaccept_config_tx(Context *ctx, fsm_events evt,
				    FSMEventData *data)
{
	if (data != NULL && data->choice == FSM_EVT_DATA_ASSOCIATION_RESULT_CHOSEN) {
		APDU response_apdu;
		PhdAssociationInformation response_info;
		memset(&response_info, 0, sizeof(PhdAssociationInformation));

		populate_aare(&response_apdu, &response_info);

		response_apdu.u.aare.result = data->u.association_result;

		// Encode APDU
		ByteStreamWriter
		*encoded_value =
			byte_stream_writer_instance(
				response_apdu.u.aare.selected_data_proto .data_proto_info.length);
		encode_phdassociationinformation(encoded_value, &response_info);

		response_apdu.u.aare.selected_data_proto.data_proto_info.value
		= encoded_value->buffer;

		communication_send_apdu(ctx, &response_apdu);

		del_phdassociationinformation(&response_info);
		del_byte_stream_writer(encoded_value, 1);
	}
}

/**
 * Internal function to populate the AARE with default response data.
 *
 * @param apdu
 * @param response_info
 */
static void populate_aare(APDU *apdu, PhdAssociationInformation *response_info)
{
	apdu->choice = AARE_CHOSEN;
	apdu->length = 44;

	apdu->u.aare.selected_data_proto.data_proto_id = DATA_PROTO_ID_20601;
	apdu->u.aare.selected_data_proto.data_proto_info.length = 38;

	response_info->protocolVersion = ASSOC_VERSION1;
	response_info->encodingRules = MDER;
	response_info->nomenclatureVersion = NOM_VERSION1;

	response_info->functionalUnits = 0x00000000;
	response_info->systemType = SYS_TYPE_MANAGER;

	response_info->system_id.value = manager_system_id();
	response_info->system_id.length = manager_system_id_length();

	response_info->dev_config_id = MANAGER_CONFIG_RESPONSE;
	response_info->data_req_mode_capab.data_req_mode_flags = 0x0000;
	response_info->data_req_mode_capab.data_req_init_agent_count = 0x00;
	response_info->data_req_mode_capab.data_req_init_manager_count = 0x00;

	response_info->optionList.count = 0;
	response_info->optionList.length = 0;
}

static void populate_aarq(APDU *apdu, PhdAssociationInformation *config_info,
				DataProto *proto);

/**
 * Send apdu association request (normally, Agent does this)
 *
 * @param ctx connection context
 * @param evt input event
 * @param data fsm data, unused
 *
 */
void association_aarq_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	APDU config_apdu;
	PhdAssociationInformation config_info;
	DataProto proto;

	memset(&config_info, 0, sizeof(PhdAssociationInformation));
	populate_aarq(&config_apdu, &config_info, &proto);

	// Encode APDU
	ByteStreamWriter *encoded_value =
		byte_stream_writer_instance(proto.data_proto_info.length);
		encode_phdassociationinformation(encoded_value, &config_info);

	proto.data_proto_info.value = encoded_value->buffer;

	communication_send_apdu(ctx, &config_apdu);

	del_byte_stream_writer(encoded_value, 1);
	del_phdassociationinformation(&config_info);
}

/**
 * Populate AARQ APDU (Normally, Agent uses this)
 *
 * @param apdu APDU structure
 * @param config_info Configuration to send
 * @param proto Data protocol to send
 */
static void populate_aarq(APDU *apdu, PhdAssociationInformation *config_info,
				DataProto *proto)
{
	struct mds_system_data *mds_data = agent_mds_data_cb();

	apdu->choice = AARQ_CHOSEN;
	apdu->length = 50;

	apdu->u.aarq.assoc_version = ASSOC_VERSION1;
	apdu->u.aarq.data_proto_list.count = 1;
	apdu->u.aarq.data_proto_list.length = 42;
	apdu->u.aarq.data_proto_list.value = proto;

	proto->data_proto_id = DATA_PROTO_ID_20601;
	proto->data_proto_info.length = 38;

	config_info->protocolVersion = ASSOC_VERSION1;
	config_info->encodingRules = MDER;
	config_info->nomenclatureVersion = NOM_VERSION1;

	config_info->functionalUnits = 0x00000000;
	config_info->systemType = SYS_TYPE_AGENT;

	config_info->system_id.length = 8;
	config_info->system_id.value = malloc(config_info->system_id.length);
	memcpy(config_info->system_id.value, mds_data->system_id,
					config_info->system_id.length);

	config_info->dev_config_id = agent_specialization;

	config_info->data_req_mode_capab.data_req_mode_flags = DATA_REQ_SUPP_INIT_AGENT;
	// max number of simultaneous sessions
	config_info->data_req_mode_capab.data_req_init_agent_count = 0x01;
	config_info->data_req_mode_capab.data_req_init_manager_count = 0x00;

	config_info->optionList.count = 0;
	config_info->optionList.length = 0;

	free(mds_data);
}


/**
 * Send apdu association rejected (normally, Agent does this if Manager tries to associate)
 *
 * @param ctx connection context
 * @param evt input event
 * @param data fsm data, unused
 */
void association_agent_aare_rejected_permanent_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data)
{
	APDU response_apdu;
	PhdAssociationInformation response_info;

	memset(&response_info, 0, sizeof(PhdAssociationInformation));
	populate_aare(&response_apdu, &response_info);

	response_apdu.u.aare.result = REJECTED_PERMANENT;

	// Encode APDU
	ByteStreamWriter *encoded_value =
		byte_stream_writer_instance(
			response_apdu.u.aare.selected_data_proto .data_proto_info.length);

	encode_phdassociationinformation(encoded_value, &response_info);

	response_apdu.u.aare.selected_data_proto.data_proto_info.value =
		encoded_value->buffer;

	communication_send_apdu(ctx, &response_apdu);

	del_byte_stream_writer(encoded_value, 1);
	del_phdassociationinformation(&response_info);
}

/** @} */
