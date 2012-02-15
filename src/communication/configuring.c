/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file configuring.c
 * \brief Configuring module source.
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
 * \author Jose Martins
 * \date Jun 23, 2010
 */

/**
 * \defgroup Configuring Configuring
 * \brief Configuring module.
 * \ingroup FSM
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include "src/dim/mds.h"
#include "src/dim/nomenclature.h"
#include "src/communication/communication.h"
#include "src/communication/configuring.h"
#include "src/communication/association.h"
#include "src/communication/service.h"
#include "src/communication/stdconfigurations.h"
#include "src/communication/extconfigurations.h"
#include "src/communication/disassociating.h"
#include "src/util/bytelib.h"
#include "src/util/bytelib.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/util/log.h"


/**
 * Retry count configuration constant
 */
static const intu32 CONFIGURING_RC = 0;
/**
 * Time out (seconds) configuration constant
 */
static const intu32 CONFIGURING_TO = 10;

static ConfigResult configuring_evaluate_configuration_validity(
	ConfigReport *config_report);

/**
 * Process incoming APDU's in checking_for_config state
 *
 * @param ctx context
 * @param apdu received APDU
 */
void configuring_waiting_state_process_apdu(Context *ctx, APDU *apdu)
{
	FSMEventData evt;
	evt.received_apdu = apdu;

	switch (apdu->choice) {
	case PRST_CHOSEN: {
		DATA_apdu *input_data_apdu = encode_get_data_apdu(&apdu->u.prst);
		Data_apdu_message message = input_data_apdu->message;

		if (input_data_apdu->message.choice
		    == ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN) {
			EventReportArgumentSimple args =
				message.u.roiv_cmipConfirmedEventReport;

			if (args.event_type == MDC_NOTI_CONFIG) {
				communication_reset_timeout(ctx);
				communication_fire_evt(ctx,
						       fsm_evt_rx_roiv_confirmed_event_report,
						       &evt);
			}

		} else if (communication_is_roiv_type(input_data_apdu)) {
			evt.choice = FSM_EVT_DATA_ERROR_RESULT;
			evt.u.error_result.error_value
			= NO_SUCH_OBJECT_INSTANCE;
			evt.u.error_result.parameter.length = 0;

			communication_fire_evt(ctx, fsm_evt_rx_roiv_event_report,
					       &evt);

		} else if (communication_is_rors_type(input_data_apdu)) {
			communication_fire_evt(ctx, fsm_evt_rx_rors, &evt);
		} else if (communication_is_roer_type(input_data_apdu)) {
			communication_fire_evt(ctx, fsm_evt_rx_roer, NULL);
		} else if (communication_is_rorj_type(input_data_apdu)) {
			communication_fire_evt(ctx, fsm_evt_rx_rorj, NULL);
		}

	}
	break;

	case AARQ_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_aarq, NULL);
		break;
	case AARE_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_aare, NULL);
		break;
	case RLRE_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_rlre, NULL);
		break;
	case RLRQ_CHOSEN: {
		evt.choice = FSM_EVT_DATA_RELEASE_RESPONSE_REASON;
		evt.u.release_response_reason = RELEASE_RESPONSE_REASON_NORMAL;
		communication_fire_evt(ctx, fsm_evt_rx_rlrq, &evt);
	}
	break;
	case ABRT_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_abrt, NULL);
		break;
	default:
		// ignore ABRT_CHOSEN, RLRE_CHOSEN
		break;
	}
}

/**
 * Process incoming APDU's in config_sending state (Agent)
 *
 * @param ctx context
 * @param apdu received APDU
 */
void configuring_agent_config_sending_process_apdu(Context *ctx, APDU *apdu)
{
	// agent does not stay in this state for very long
	// same handling as waiting_approval
	configuring_agent_waiting_approval_process_apdu(ctx, apdu);
}

/**
 * Process rors in APDU
 *
 * @param ctx
 * @param *apdu
 */
static void comm_agent_process_confirmed_event_report(Context *ctx, APDU *apdu,
					EventReportResultSimple *report,
					FSMEventData *data)
{
	int error = 0;

	if (report->obj_handle != MDS_HANDLE)
		goto fail;
	if (report->event_type != MDC_NOTI_CONFIG)
		goto fail;

	ConfigReportRsp rsp;
	ByteStreamReader *stream = byte_stream_reader_instance(report->event_reply_info.value,
						report->event_reply_info.length);
	decode_configreportrsp(stream, &rsp, &error);
	free(stream);

	if (error)
		goto fail;

	if (rsp.config_result == ACCEPTED_CONFIG) {
		communication_fire_evt(ctx,
		       fsm_evt_rx_rors_confirmed_event_report_known,
		       data);
	} else {
		communication_fire_evt(ctx,
		       fsm_evt_rx_rors_confirmed_event_report_unknown,
		       data);
	}

	return;
fail:
	communication_fire_evt(ctx, fsm_evt_rx_rors, data);
}

/**
 * Process rors in APDU
 *
 * @param ctx
 * @param *apdu
 */
static void communication_agent_process_rors(Context *ctx, APDU *apdu)
{
	DATA_apdu *data_apdu = encode_get_data_apdu(&apdu->u.prst);
	FSMEventData data;
	//RejectResult reject_result;

	if (service_check_known_invoke_id(ctx, data_apdu)) {
		switch (data_apdu->message.choice) {
		case RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
			data.received_apdu = apdu;
			comm_agent_process_confirmed_event_report(ctx, apdu,
				&(data_apdu->message.u.rors_cmipConfirmedEventReport),
 				&data);
			break;
		case RORS_CMIP_GET_CHOSEN:
			data.received_apdu = apdu;
			communication_fire_evt(ctx, fsm_evt_rx_rors_get, &data);
			break;
		case RORS_CMIP_CONFIRMED_ACTION_CHOSEN:
			data.received_apdu = apdu;
			communication_fire_evt(ctx,
					       fsm_evt_rx_rors_confirmed_action, &data);
			break;
		case RORS_CMIP_CONFIRMED_SET_CHOSEN:
			data.received_apdu = apdu;
			communication_fire_evt(ctx, fsm_evt_rx_rors_confirmed_set,
					       &data);
			break;
		default:
			//reject_result.problem = UNRECOGNIZED_OPERATION;
			break;
		}

		service_request_retired(ctx, data_apdu);
	}
}


/**
 * Process roiv in APDU
 *
 * @param ctx
 * @param *apdu
 */
static void communication_agent_process_roiv(Context *ctx, APDU *apdu)
{
	DATA_apdu *data_apdu = encode_get_data_apdu(&apdu->u.prst);
	FSMEventData data;

	switch (data_apdu->message.choice) {
		case ROIV_CMIP_GET_CHOSEN:
			data.received_apdu = apdu;
			communication_fire_evt(ctx, fsm_evt_rx_roiv_get, &data);
			break;
		default:
			communication_fire_evt(ctx, fsm_evt_rx_roiv, NULL);
			break;
	}
}


/**
 * Process incoming APDU's in waiting_approval state (Agent)
 *
 * @param ctx context
 * @param apdu received APDU
 */
void configuring_agent_waiting_approval_process_apdu(Context *ctx, APDU *apdu)
{
	switch (apdu->choice) {
	case PRST_CHOSEN: {
		DATA_apdu *data_apdu = encode_get_data_apdu(&apdu->u.prst);

		if (communication_is_roiv_type(data_apdu)) {
			communication_agent_process_roiv(ctx, apdu);
		} else if (communication_is_roer_type(data_apdu)) {
			communication_fire_evt(ctx, fsm_evt_rx_roer, NULL);
		} else if (communication_is_rorj_type(data_apdu)) {
			communication_fire_evt(ctx, fsm_evt_rx_rorj, NULL);
		} else if (communication_is_rors_type(data_apdu)) {
			communication_agent_process_rors(ctx, apdu);
		}
	}
	break;
	case AARQ_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_aarq, NULL);
		break;
	case AARE_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_aare, NULL);
		break;
	case RLRQ_CHOSEN: {
		FSMEventData evt;
		evt.choice = FSM_EVT_DATA_RELEASE_RESPONSE_REASON;
		evt.u.release_response_reason = RELEASE_RESPONSE_REASON_NORMAL;
		communication_fire_evt(ctx, fsm_evt_rx_rlrq, &evt);
	}
	break;
	case RLRE_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_rlre, NULL);
		break;
	case ABRT_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_abrt, NULL);
		break;
	default:
		break;
	}
}

/**
 * Process incoming APDU's in checking_for_config state
 * @param ctx context
 * @param apdu received APDU
 */
void configuring_checking_state_process_apdu(Context *ctx, APDU *apdu)
{
	switch (apdu->choice) {
	case PRST_CHOSEN: {
		DATA_apdu *input_data_apdu = encode_get_data_apdu(&apdu->u.prst);

		if (input_data_apdu->message.choice
		    == ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN) {
			FSMEventData data;
			data.received_apdu = apdu;
			communication_fire_evt(ctx,
					       fsm_evt_rx_roiv_confirmed_event_report,
					       &data); // 7.24
		} else if (communication_is_roiv_type(input_data_apdu)) {
			FSMEventData data;
			data.received_apdu = apdu;
			data.choice = FSM_EVT_DATA_ERROR_RESULT;
			data.u.error_result.error_value = NO_SUCH_ACTION;
			data.u.error_result.parameter.length = 0;
			communication_fire_evt(ctx,
					       fsm_evt_rx_roiv_all_except_confirmed_event_report,
					       &data); // 7.25
		} else if (communication_is_rors_type(input_data_apdu)) {
			communication_fire_evt(ctx, fsm_evt_rx_rors, NULL); // 7.26
		} else if (communication_is_roer_type(input_data_apdu)) {
			communication_fire_evt(ctx, fsm_evt_rx_roer, NULL); // 7.26
		} else if (communication_is_rorj_type(input_data_apdu)) {
			communication_fire_evt(ctx, fsm_evt_rx_rorj, NULL); // 7.26
		}
	}
	break;

	case AARQ_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_aarq, NULL); // 7.8
		break;
	case AARE_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_aare, NULL); // 7.12
		break;
	case RLRQ_CHOSEN: {
		FSMEventData evt;
		evt.choice = FSM_EVT_DATA_RELEASE_RESPONSE_REASON;
		evt.u.release_response_reason = RELEASE_RESPONSE_REASON_NORMAL;
		communication_fire_evt(ctx, fsm_evt_rx_rlrq, &evt); // 7.16
	}
	break;
	case RLRE_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_rlre, NULL); // 7.17
		break;
	case ABRT_CHOSEN:
		communication_fire_evt(ctx, fsm_evt_rx_abrt, NULL); // 7.18
		break;
	default:
		break;
	}
}

/**
 * performs validation of the configuration through the following criteria:
 *   1. is a standard configuration supported;
 *   2. the configuration is described in the config_report parameter
 *
 * @param config_report Identify the configuration described in the specialization
 *                      document and describes the extended configuration;
 *
 * @return Return the Configuration Result (ACCEPTED_CONFIG, STANDARD_CONFIG_UNKNOWN or
 *         UNSUPPORTED_CONFIG)
 */
static ConfigResult configuring_evaluate_configuration_validity(
	ConfigReport *config_report)
{
	int std_support = std_configurations_is_supported_standard(
				  config_report->config_report_id);

	if (std_support) {
		return ACCEPTED_CONFIG;
	} else if (config_report->config_obj_list.count == 0) {
		return STANDARD_CONFIG_UNKNOWN;
	} else {
		return ACCEPTED_CONFIG; // TODO CHECK UNSUPPORTED_CONFIG
	}

}

/**
 * Perform configuration (standard or extended), inner layer
 *
 * @param ctx context
 * @param config_report configuration report
 * @param apdu APDU data
 * @param trans whether context is a transcoded device
 * @return config result
 */
ConfigResult configuring_perform_configuration_in(Context *ctx,
					ConfigReport config_report,
					APDU *apdu,
					int trans)
{
	FSMEventData data;
	FSMEventData *datap = NULL;
	int event;

	if (! trans) {
		// transcoded devices don't need response APDUs
		// and we mute them by omitting data in event firing
		datap = &data;
	}

	communication_reset_timeout(ctx);

	ConfigResult result =
		configuring_evaluate_configuration_validity(&config_report);

	octet_string *system_id = &ctx->mds->system_id;

	if (result == ACCEPTED_CONFIG) {
		DEBUG(" configuring: accepting... ");
		event = fsm_evt_req_agent_supplied_known_configuration;

		ConfigObjectList *object_list;

		if (std_configurations_is_supported_standard(
				    config_report.config_report_id)) {
			DEBUG(" configuring: using standard configuration ");
			object_list = std_configurations_get_configuration_attributes(
					      config_report.config_report_id);

			mds_configure_operating(ctx, object_list, 1);

			del_configreport(&config_report);
			free(object_list);

		} else if (ext_configurations_is_supported_standard(system_id,
					config_report.config_report_id) &&
			  (object_list = ext_configurations_get_configuration_attributes(
					system_id, config_report.config_report_id))) {
			DEBUG(" configuring: using previous known extended configuration");

			mds_configure_operating(ctx, object_list, 1);

			del_configreport(&config_report);
			free(object_list);

		} else {
			DEBUG(" configuring: using new extended configuration");
			object_list = &config_report.config_obj_list;

			ext_configurations_register_conf(system_id,
				config_report.config_report_id, object_list);
			mds_configure_operating(ctx, object_list, 1);
			// do not free config_report, object_list
		}

	} else if (result == STANDARD_CONFIG_UNKNOWN) {
		event = fsm_evt_req_agent_supplied_unknown_configuration;
		DEBUG("   -> STANDARD_CONFIG_UNKNOWN");
	} else {
		event = fsm_evt_req_agent_supplied_unknown_configuration;
		DEBUG("   -> UNSUPPORTED_CONFIG");
	}

	data.received_apdu = apdu;
	data.choice = FSM_EVT_DATA_CONFIGURATION_RESULT;
	data.u.configuration_result = result;

	communication_fire_evt(ctx, event, datap);

	return result;
}

/**
 * Perform configuration (standard or extended)
 *
 * @param ctx context
 * @param evt
 * @param *event_data
 */
void configuring_perform_configuration(Context *ctx, fsm_events evt,
				       FSMEventData *event_data)
{
	int error = 0;
	DEBUG("\n configuring: performing");

	APDU *apdu = event_data->received_apdu;

	DATA_apdu *input_data_apdu = encode_get_data_apdu(&apdu->u.prst);
	EventReportArgumentSimple args = input_data_apdu->message.u.roiv_cmipConfirmedEventReport;

	ConfigReport config_report;
	ByteStreamReader *config_stream = byte_stream_reader_instance(args.event_info.value,
					  args.event_info.length);
	decode_configreport(config_stream, &config_report, &error);

	if (!error) {
		configuring_perform_configuration_in(ctx, config_report, apdu, 0);
	}

	free(config_stream);
}

/**
 * Send message to agent with a abort message or roer message (no such object instance)
 *
 * @param ctx connection state
 * @param evt input event
 * @param *event_data Contains event data which in this case is the original apdu.
 */
void configuring_new_measurements_response_tx(Context *ctx, fsm_events evt,
		FSMEventData *event_data)
{
	APDU *apdu = event_data->received_apdu;
	DATA_apdu *input_data_apdu = encode_get_data_apdu(&apdu->u.prst);
	EventReportArgumentSimple
	args =
		input_data_apdu->message.u.roiv_cmipConfirmedEventReport;

	if (args.event_type == MDC_NOTI_CONFIG) {
		communication_abort_undefined_reason_tx(ctx, evt, event_data);
	} else {
		FSMEventData data;
		data.received_apdu = apdu;
		data.choice = FSM_EVT_DATA_ERROR_RESULT;
		data.u.error_result.error_value = NO_SUCH_OBJECT_INSTANCE;
		data.u.error_result.parameter.length = 0;
		communication_roer_tx(ctx, evt, &data);
	}
}

/**
 * Send message with the configuration result (supported or unsupported)
 *
 * @param *ctx connection context
 * @param evt input event
 * @param *event_data Contains event data which in this case is the structure ConfigResult
 */
void configuring_configuration_response_tx(Context *ctx, fsm_events evt,
		FSMEventData *event_data)
{
	int error = 0;
	DEBUG("\n configuring: send configuration response");

	APDU *apdu = event_data->received_apdu;
	DATA_apdu *input_data_apdu = encode_get_data_apdu(&apdu->u.prst);
	EventReportArgumentSimple args =
		input_data_apdu->message.u.roiv_cmipConfirmedEventReport;

	ConfigReport config_report;
	ByteStreamReader *config_stream = byte_stream_reader_instance(args.event_info.value,
					  args.event_info.length);
	decode_configreport(config_stream, &config_report, &error);
	free(config_stream);

	if (error) {
		// TODO abort?
		DEBUG("bad config report, not responding");
		return;
	}

	ConfigId config_report_id = config_report.config_report_id;
	del_configreport(&config_report);

	// step01: create APDU structure

	APDU result_apdu;
	result_apdu.choice = PRST_CHOSEN;

	DATA_apdu data;
	data.invoke_id = input_data_apdu->invoke_id;
	data.message.choice = RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN; // Confirmed Event Report

	EventReportResultSimple *confirmed_result =
		&(data.message.u.rors_cmipConfirmedEventReport);
	confirmed_result->obj_handle = args.obj_handle; // MDS Object
	confirmed_result->currentTime = args.event_time;
	confirmed_result->event_type = args.event_type; // MDC_NOTI_CONFIG

	ConfigReportRsp config_resp;
	config_resp.config_report_id = config_report_id;
	config_resp.config_result = event_data->u.configuration_result;

	// step02: calculate the APDU size

	confirmed_result->event_reply_info.length = 4; // ConfigReportRsp

	data.message.length = sizeof(HANDLE) + sizeof(RelativeTime)
			      + sizeof(OID_Type) // EventReportResultSimple fields
			      + sizeof(intu16); // Any's length field
	data.message.length += confirmed_result->event_reply_info.length;

	result_apdu.u.prst.length = sizeof(InvokeIDType)
				    + sizeof(DATA_apdu_choice)
				    + sizeof(intu16); // data message's length field
	result_apdu.u.prst.length += data.message.length;

	result_apdu.length = result_apdu.u.prst.length
				+ sizeof(intu16); // prst length field

	// step03: encode APDU

	ByteStreamWriter *config_rsp_stream = byte_stream_writer_instance(
			confirmed_result->event_reply_info.length);
	encode_configreportrsp(config_rsp_stream, &config_resp);
	confirmed_result->event_reply_info.value = config_rsp_stream->buffer;


	ByteStreamWriter *apdu_stream = byte_stream_writer_instance(
						result_apdu.length
						+ 4); // APDU choice, length fields

	encode_set_data_apdu(&result_apdu.u.prst, &data);
	encode_apdu(apdu_stream, &result_apdu);

	// step04: send APDU
	communication_send_apdu(ctx, &result_apdu);

	del_byte_stream_writer(config_rsp_stream, 1);
	del_byte_stream_writer(apdu_stream, 1);
}

/**
 * Send message with the release request
 *
 * @param ctx context
 * @param evt
 * @param *event_data
 */
void configuring_association_release_request_tx(Context *ctx, fsm_events evt,
		FSMEventData *event_data)
{
	communication_reset_timeout(ctx);
	disassociating_release_request_tx(ctx, evt, event_data);
}


/**
 * Listens to fsm event to enter in "waiting for config state"
 *
 * @param ctx context
 * @param evt
 * @param event_data
 */
void configuring_transition_waiting_for_config(Context *ctx, fsm_events evt,
		FSMEventData *event_data)
{
	association_accept_config_tx(ctx, evt, event_data);

	communication_count_timeout(ctx, &communication_timeout, CONFIGURING_TO);
}

/**
 * Sends configuration apdu (agent)
 *
 * @param ctx context
 * @param evt
 * @param event_data
 */
void configuring_send_config_tx(Context *ctx, fsm_events evt,
					FSMEventData *event_data)
{
	APDU *apdu = calloc(1, sizeof(APDU));
	PRST_apdu prst;
	DATA_apdu *data = calloc(1, sizeof(DATA_apdu));
	EventReportArgumentSimple evtrep;
	ConfigReport cfgrep;

	DEBUG("Sending configuration since manager does not know it");

	ConfigObjectList *cfg =
		std_configurations_get_configuration_attributes(
				      		agent_specialization);

	data->invoke_id = 0; // filled by service_* call
	data->message.choice = ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN;

	evtrep.obj_handle = 0;
	evtrep.event_time = 0xFFFFFFFF;
	evtrep.event_type = MDC_NOTI_CONFIG;

	cfgrep.config_obj_list = *cfg;
	cfgrep.config_report_id = agent_specialization;

	// compensate for config_report
	evtrep.event_info.length = cfg->length + 6; // 80 + 6 = 86 for oximeter

	ByteStreamWriter *cfg_writer = byte_stream_writer_instance(evtrep.event_info.length);
	encode_configreport(cfg_writer, &cfgrep);
	evtrep.event_info.value = cfg_writer->buffer;
	// APDU takes ownership of this buffer, deleted by del_apdu()
	del_byte_stream_writer(cfg_writer, 0);

	data->message.u.roiv_cmipConfirmedEventReport = evtrep;
	data->message.length = evtrep.event_info.length + 10; // 86 + 10 = 96 for oximeter

	// prst = length + DATA_apdu
	// take into account data's invoke id and choice
	prst.length = data->message.length + 6; // 96 + 6 = 102 for oximeter
	encode_set_data_apdu(&prst, data);

	apdu->choice = PRST_CHOSEN;
	apdu->length = prst.length + 2; // 102 + 2 = 104 for oximeter
	apdu->u.prst = prst;

	timeout_callback tm = {.func = &communication_timeout, .timeout = 3};

	// takes ownership of apdu and prst.value
	service_send_remote_operation_request(ctx, apdu, tm, NULL);

	del_configobjectlist(cfg);
	free(cfg);
}

/** @} */
