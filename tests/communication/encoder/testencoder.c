/**********************************************************************
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
 * testencoder.c
 *
 * Created on: Jun 16, 2010
 *     Author: mateus.lima
**********************************************************************/

#ifdef TEST_ENABLED

#include "Basic.h"
#include "src/asn1/phd_types.h"
#include "src/dim/nomenclature.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/util/bytelib.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/util/ioutil.h"
#include "tests/functional_test_cases/test_functional.h"
#include "testencoder.h"

#include <stdlib.h>
#include <stdio.h>

static unsigned char *h212_buffer = NULL;
static unsigned char *h222_buffer = NULL;
static unsigned char *h232_buffer = NULL;
static unsigned char *h242_buffer = NULL;
static unsigned char *h243_buffer = NULL;
static intu8 *data_apdu1_buffer = NULL;
static intu8 *data_apdu2_buffer = NULL;

static unsigned long h212_size = 0;
static unsigned long h222_size = 0;
static unsigned long h232_size = 0;
static unsigned long h242_size = 0;
static unsigned long h243_size = 0;
static unsigned long data_apdu1_size = 0;
static unsigned long data_apdu2_size = 0;

int testencoder_init_suite(void)
{
	h212_buffer = ioutil_buffer_from_file(apdu_H212, &h212_size);
	h222_buffer = ioutil_buffer_from_file(apdu_H222, &h222_size);
	h232_buffer = ioutil_buffer_from_file(apdu_H232, &h232_size);
	h242_buffer = ioutil_buffer_from_file(apdu_H242, &h242_size);
	h243_buffer = ioutil_buffer_from_file(apdu_H243, &h243_size);
	data_apdu1_buffer = ioutil_buffer_from_file(test_data_apdu1_file, &data_apdu1_size);
	data_apdu2_buffer = ioutil_buffer_from_file(test_data_apdu2_file, &data_apdu2_size);
	return 0;
}

int testencoder_finish_suite(void)
{
	free(h212_buffer);
	free(h222_buffer);
	free(h232_buffer);
	free(h242_buffer);
	free(h243_buffer);
	free(data_apdu1_buffer);
	free(data_apdu2_buffer);

	h212_buffer = NULL;
	h222_buffer = NULL;
	h232_buffer = NULL;
	h242_buffer = NULL;
	h243_buffer = NULL;
	data_apdu1_buffer = NULL;
	data_apdu2_buffer = NULL;

	return 0;
}

void testencoder_add_suite()
{
	CU_pSuite suite = CU_add_suite("Encoder Test Suite",
				       testencoder_init_suite, testencoder_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_encoder_h212_apdu_encoder",
		    test_encoder_h212_apdu_encoder);
	CU_add_test(suite, "test_encoder_h222_apdu_encoder",
		    test_encoder_h222_apdu_encoder);
	CU_add_test(suite, "test_encoder_h232_apdu_encoder",
		    test_encoder_h232_apdu_encoder);
	CU_add_test(suite, "test_encoder_h242_apdu_encoder",
		    test_encoder_h242_apdu_encoder);
	CU_add_test(suite, "test_encoder_h243_apdu_encoder",
		    test_encoder_h243_apdu_encoder);

	CU_add_test(suite, "test_encoder_byte_stream_writer",
		    test_enconder_byte_stream_writer);
	CU_add_test(suite, "test_encoder_data_apdu_encoder_1",
		    test_encoder_data_apdu_encoder_1);

	CU_add_test(suite, "test_encoder_data_apdu_encoder_2",
		    test_encoder_data_apdu_encoder_2);

	CU_add_test(suite, "test_encoder_data_apdu_encoder_3",
		    test_encoder_data_apdu_encoder_3);
	/* Add tests here - End */
}

void test_encoder_h212_apdu_encoder()
{
	APDU apdu;
	apdu.choice = AARE_CHOSEN;
	apdu.length = 44;
	apdu.u.aare.result = ACCEPTED_UNKNOWN_CONFIG;
	apdu.u.aare.selected_data_proto.data_proto_id = DATA_PROTO_ID_20601;
	apdu.u.aare.selected_data_proto.data_proto_info.length = 38;

	PhdAssociationInformation association_information;
	association_information.protocolVersion = ASSOC_VERSION1;
	association_information.encodingRules = MDER;
	association_information.nomenclatureVersion = NOM_VERSION1;
	association_information.functionalUnits = 0x00000000;
	association_information.systemType = SYS_TYPE_MANAGER;

	association_information.system_id.length = 8;
	unsigned char system_id_value[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
	association_information.system_id.value = system_id_value;

	association_information.dev_config_id = 0x0000;
	association_information.data_req_mode_capab.data_req_mode_flags = 0x0000;
	association_information.data_req_mode_capab.data_req_init_agent_count = 0x00;
	association_information.data_req_mode_capab.data_req_init_manager_count = 0x00;

	association_information.optionList.count = 0;
	association_information.optionList.length = 0;

	ByteStreamWriter *stream_writer2 = byte_stream_writer_instance(38);
	CU_ASSERT_PTR_NOT_NULL(stream_writer2);
	encode_phdassociationinformation(stream_writer2, &association_information);

	apdu.u.aare.selected_data_proto.data_proto_info.value = stream_writer2->buffer;

	ByteStreamWriter *stream_writer = byte_stream_writer_instance(apdu.length + 4/* apdu header */);
	CU_ASSERT_PTR_NOT_NULL(stream_writer);

	encode_apdu(stream_writer, &apdu);

	int i;

	for (i = 0; i < h212_size; ++i) {
		CU_ASSERT_EQUAL(stream_writer->buffer[i], h212_buffer[i]);
	}

	del_byte_stream_writer(stream_writer, 1);
	del_byte_stream_writer(stream_writer2, 1);
}

void test_encoder_h222_apdu_encoder()
{
	APDU apdu;
	apdu.choice = PRST_CHOSEN;
	apdu.length = 22;
	apdu.u.prst.length = 20;

	DATA_apdu data_apdu;
	data_apdu.invoke_id = 0x4321;
	data_apdu.message.choice = RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN;
	data_apdu.message.length = 14;
	data_apdu.message.u.rors_cmipConfirmedEventReport.obj_handle = 0x0000;
	data_apdu.message.u.rors_cmipConfirmedEventReport.currentTime = 0x00000000;
	data_apdu.message.u.rors_cmipConfirmedEventReport.event_type = MDC_NOTI_CONFIG;
	data_apdu.message.u.rors_cmipConfirmedEventReport.event_reply_info.length = 0x0004;

	ConfigReportRsp report_resp;
	report_resp.config_report_id = 0x4000;
	report_resp.config_result = ACCEPTED_CONFIG;

	ByteStreamWriter *stream_writer = byte_stream_writer_instance(4);
	CU_ASSERT_PTR_NOT_NULL(stream_writer);

	encode_configreportrsp(stream_writer, &report_resp);

	data_apdu.message.u.rors_cmipConfirmedEventReport.event_reply_info.value = stream_writer->buffer;

	encode_set_data_apdu(&apdu.u.prst, &data_apdu);

	ByteStreamWriter *stream_writer2 = byte_stream_writer_instance(apdu.length + 4/* apdu header */);
	CU_ASSERT_PTR_NOT_NULL(stream_writer2);

	encode_apdu(stream_writer2, &apdu);

	int i;

	for (i = 0; i < h222_size; ++i) {
		CU_ASSERT_EQUAL(stream_writer2->buffer[i], h222_buffer[i]);
	}

	del_byte_stream_writer(stream_writer, 1);
	del_byte_stream_writer(stream_writer2, 1);
}

void test_encoder_h232_apdu_encoder()
{
	APDU apdu;
	apdu.choice = PRST_CHOSEN;
	apdu.length = 14;
	apdu.u.prst.length = 12;

	DATA_apdu data_apdu;
	data_apdu.invoke_id = 0x3456;
	data_apdu.message.choice = ROIV_CMIP_GET_CHOSEN;
	data_apdu.message.length = 6;
	data_apdu.message.u.roiv_cmipGet.obj_handle = 0x0000;
	data_apdu.message.u.roiv_cmipGet.attribute_id_list.count = 0x0000;
	data_apdu.message.u.roiv_cmipGet.attribute_id_list.length = 0x0000;

	encode_set_data_apdu(&apdu.u.prst, &data_apdu);

	ByteStreamWriter *stream_writer = byte_stream_writer_instance(apdu.length + 4/* apdu header */);
	CU_ASSERT_PTR_NOT_NULL(stream_writer);

	encode_apdu(stream_writer, &apdu);

	int i;

	for (i = 0; i < h232_size; ++i) {
		CU_ASSERT_EQUAL(stream_writer->buffer[i], h232_buffer[i]);
	}

	del_byte_stream_writer(stream_writer, 1);
}

void test_encoder_h242_apdu_encoder()
{
	APDU apdu;
	memset(&apdu, 0, sizeof(APDU));
	apdu.choice = PRST_CHOSEN;
	apdu.length = 18;
	apdu.u.prst.length = 16;

	DATA_apdu data_apdu;
	memset(&data_apdu, 0, sizeof(DATA_apdu));
	data_apdu.invoke_id = 0x4321;
	data_apdu.message.choice = RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN;
	data_apdu.message.length = 10;
	data_apdu.message.u.rors_cmipConfirmedEventReport.obj_handle = 0x0000;
	data_apdu.message.u.rors_cmipConfirmedEventReport.currentTime = 0x00000000;
	data_apdu.message.u.rors_cmipConfirmedEventReport.event_type = MDC_NOTI_SCAN_REPORT_FIXED;
	data_apdu.message.u.rors_cmipConfirmedEventReport.event_reply_info.length = 0x0000;

	encode_set_data_apdu(&apdu.u.prst, &data_apdu);

	ByteStreamWriter *stream_writer = byte_stream_writer_instance(apdu.length + 4/* apdu header */);
	CU_ASSERT_PTR_NOT_NULL(stream_writer);

	encode_apdu(stream_writer, &apdu);

	int i;

	for (i = 0; i < h242_size; ++i) {

		CU_ASSERT_EQUAL(stream_writer->buffer[i], h242_buffer[i]);
	}

	del_byte_stream_writer(stream_writer, 1);
}

void test_encoder_h243_apdu_encoder(void)
{
	APDU apdu;
	apdu.choice = PRST_CHOSEN;
	apdu.length = 30;
	apdu.u.prst.length = 28;

	DATA_apdu data_apdu;
	data_apdu.invoke_id = 0x7654;
	data_apdu.message.choice = ROIV_CMIP_CONFIRMED_ACTION_CHOSEN;
	data_apdu.message.length = 22;
	data_apdu.message.u.roiv_cmipConfirmedAction.obj_handle = 0x0000;
	data_apdu.message.u.roiv_cmipConfirmedAction.action_type = MDC_ACT_DATA_REQUEST;
	data_apdu.message.u.roiv_cmipConfirmedAction.action_info_args.length = 16;

	DataRequest data_request;
	data_request.data_req_id = 0x0100;
	data_request.data_req_mode = DATA_REQ_START_STOP | DATA_REQ_SCOPE_TYPE | DATA_REQ_MODE_SINGLE_RSP;
	data_request.data_req_time = 0x00000000;
	data_request.data_req_person_id = 0x0000;
	data_request.data_req_class = MDC_MOC_VMO_METRIC_NU;
	data_request.data_req_obj_handle_list.count = 0x0000;
	data_request.data_req_obj_handle_list.length = 0x0000;

	ByteStreamWriter *stream_writer2 = byte_stream_writer_instance(16);
	CU_ASSERT_PTR_NOT_NULL(stream_writer2);

	encode_datarequest(stream_writer2, &data_request);
	data_apdu.message.u.roiv_cmipConfirmedAction.action_info_args.value = stream_writer2->buffer;

	encode_set_data_apdu(&apdu.u.prst, &data_apdu);

	ByteStreamWriter *stream_writer = byte_stream_writer_instance(apdu.length + 4/* apdu header */);
	CU_ASSERT_PTR_NOT_NULL(stream_writer);

	encode_apdu(stream_writer, &apdu);

	int i;

	for (i = 0; i < h243_size; ++i) {

		CU_ASSERT_EQUAL(stream_writer->buffer[i], h243_buffer[i]);
	}

	del_byte_stream_writer(stream_writer, 1);
	del_byte_stream_writer(stream_writer2, 1);
}

void test_enconder_byte_stream_writer()
{
	// Create a ByteStreamReader
	ByteStreamReader *stream = byte_stream_reader_instance(h243_buffer, h243_size);
	CU_ASSERT_PTR_NOT_NULL(stream);

	ByteStreamWriter *stream_writer = byte_stream_writer_instance(h243_size);
	CU_ASSERT_PTR_NOT_NULL(stream_writer);

	int i;

	for (i = 0; i < h243_size; i++) {
		intu8 data = read_intu8(stream, NULL);
		write_intu8(stream_writer, data);
	}

	for (i = 0; i < h243_size; ++i) {
		CU_ASSERT_EQUAL(stream_writer->buffer[i], h243_buffer[i]);
	}

	CU_ASSERT_EQUAL(write_intu8(stream_writer, 10), 0);
	CU_ASSERT_EQUAL(write_intu16(stream_writer, 10), 0);
	CU_ASSERT_EQUAL(write_intu32(stream_writer, 10), 0);

	CU_ASSERT_EQUAL(stream_writer->buffer[h243_size-1], h243_buffer[h243_size-1]);

	// Create a ByteStreamReader
	ByteStreamReader *stream2 = byte_stream_reader_instance(h243_buffer, h243_size);
	CU_ASSERT_PTR_NOT_NULL(stream2);

	ByteStreamWriter *stream_writer2 = byte_stream_writer_instance(h243_size);
	CU_ASSERT_PTR_NOT_NULL(stream_writer2);

	for (i = 0; i < (h243_size / 2); ++i) {
		intu16 data = read_intu16(stream2, NULL);
		write_intu16(stream_writer2, data);
	}

	int size_to_count = (h243_size / 2) * 2;

	for (i = 0; i < size_to_count; ++i) {

		CU_ASSERT_EQUAL(stream_writer2->buffer[i], h243_buffer[i]);
	}

	// Create a ByteStreamReader
	ByteStreamReader *stream3 = byte_stream_reader_instance(h243_buffer, h243_size);
	CU_ASSERT_PTR_NOT_NULL(stream3);

	ByteStreamWriter *stream_writer3 = byte_stream_writer_instance(h243_size);
	CU_ASSERT_PTR_NOT_NULL(stream_writer3);

	for (i = 0; i < (h243_size / 4); ++i) {
		intu32 data = read_intu32(stream3, NULL);
		write_intu32(stream_writer3, data);
	}

	size_to_count = (h243_size / 4) * 4;

	for (i = 0; i < size_to_count; ++i) {
		CU_ASSERT_EQUAL(stream_writer3->buffer[i], h243_buffer[i]);
	}

	ByteStreamWriter *stream_writer4 = byte_stream_writer_instance(100);
	CU_ASSERT_PTR_NOT_NULL(stream_writer4);

	write_float(stream_writer4, 62.4);
	write_float(stream_writer4, 25.8);
	write_float(stream_writer4, -0.2);
	write_float(stream_writer4, 0.03);
	write_float(stream_writer4, 15000000.0);
	write_float(stream_writer4, 15000000.1);
	write_float(stream_writer4, 1500000.1);
	write_float(stream_writer4, 1499999.99);
	write_float(stream_writer4, NAN);
	write_float(stream_writer4, 1e300);
	write_float(stream_writer4, -1e300);

	write_sfloat(stream_writer4, 62.4);
	write_sfloat(stream_writer4, 25.8);
	write_sfloat(stream_writer4, -0.2);
	write_sfloat(stream_writer4, 0.03);
	write_sfloat(stream_writer4, 15000.0);
	write_sfloat(stream_writer4, 15000.1);
	write_sfloat(stream_writer4, 1500.1);
	write_sfloat(stream_writer4, 1499.9);
	write_sfloat(stream_writer4, NAN);
	write_sfloat(stream_writer4, 1e300);
	write_sfloat(stream_writer4, -1e300);

	CU_ASSERT_EQUAL(stream_writer4->buffer[0], 0xFF);
	CU_ASSERT_EQUAL(stream_writer4->buffer[1], 0x00);
	CU_ASSERT_EQUAL(stream_writer4->buffer[2], 0x02);
	CU_ASSERT_EQUAL(stream_writer4->buffer[3], 0x70);

	CU_ASSERT_EQUAL(stream_writer4->buffer[4], 0xFF);
	CU_ASSERT_EQUAL(stream_writer4->buffer[5], 0x00);
	CU_ASSERT_EQUAL(stream_writer4->buffer[6], 0x01);
	CU_ASSERT_EQUAL(stream_writer4->buffer[7], 0x02);

	CU_ASSERT_EQUAL(stream_writer4->buffer[8], 0xFF);
	CU_ASSERT_EQUAL(stream_writer4->buffer[9], 0xFF);
	CU_ASSERT_EQUAL(stream_writer4->buffer[10], 0xFF);
	CU_ASSERT_EQUAL(stream_writer4->buffer[11], 0xFE);

	CU_ASSERT_EQUAL(stream_writer4->buffer[12], 0xFE);
	CU_ASSERT_EQUAL(stream_writer4->buffer[13], 0x00);
	CU_ASSERT_EQUAL(stream_writer4->buffer[14], 0x00);
	CU_ASSERT_EQUAL(stream_writer4->buffer[15], 0x03);

	CU_ASSERT_EQUAL(stream_writer4->buffer[16], 0x01);
	CU_ASSERT_EQUAL(stream_writer4->buffer[17], 0x16);
	CU_ASSERT_EQUAL(stream_writer4->buffer[18], 0xe3);
	CU_ASSERT_EQUAL(stream_writer4->buffer[19], 0x60);

	CU_ASSERT_EQUAL(stream_writer4->buffer[20], 0x01);
	CU_ASSERT_EQUAL(stream_writer4->buffer[21], 0x16);
	CU_ASSERT_EQUAL(stream_writer4->buffer[22], 0xe3);
	CU_ASSERT_EQUAL(stream_writer4->buffer[23], 0x60);

	CU_ASSERT_EQUAL(stream_writer4->buffer[24], 0x00);
	CU_ASSERT_EQUAL(stream_writer4->buffer[25], 0x16);
	CU_ASSERT_EQUAL(stream_writer4->buffer[26], 0xe3);
	CU_ASSERT_EQUAL(stream_writer4->buffer[27], 0x60);

	CU_ASSERT_EQUAL(stream_writer4->buffer[28], 0x00);
	CU_ASSERT_EQUAL(stream_writer4->buffer[29], 0x16);
	CU_ASSERT_EQUAL(stream_writer4->buffer[30], 0xe3);
	CU_ASSERT_EQUAL(stream_writer4->buffer[31], 0x60);

	CU_ASSERT_EQUAL(stream_writer4->buffer[32], 0x00);
	CU_ASSERT_EQUAL(stream_writer4->buffer[33], 0x7F);
	CU_ASSERT_EQUAL(stream_writer4->buffer[34], 0xFF);
	CU_ASSERT_EQUAL(stream_writer4->buffer[35], 0xFF);

	CU_ASSERT_EQUAL(stream_writer4->buffer[36], 0x00);
	CU_ASSERT_EQUAL(stream_writer4->buffer[37], 0x7F);
	CU_ASSERT_EQUAL(stream_writer4->buffer[38], 0xFF);
	CU_ASSERT_EQUAL(stream_writer4->buffer[39], 0xFE);

	CU_ASSERT_EQUAL(stream_writer4->buffer[40], 0x00);
	CU_ASSERT_EQUAL(stream_writer4->buffer[41], 0x80);
	CU_ASSERT_EQUAL(stream_writer4->buffer[42], 0x00);
	CU_ASSERT_EQUAL(stream_writer4->buffer[43], 0x02);

	CU_ASSERT_EQUAL(stream_writer4->buffer[44], 0xF2);
	CU_ASSERT_EQUAL(stream_writer4->buffer[45], 0x70);

	CU_ASSERT_EQUAL(stream_writer4->buffer[46], 0xF1);
	CU_ASSERT_EQUAL(stream_writer4->buffer[47], 0x02);

	CU_ASSERT_EQUAL(stream_writer4->buffer[48], 0xFF);
	CU_ASSERT_EQUAL(stream_writer4->buffer[49], 0xFE);

	CU_ASSERT_EQUAL(stream_writer4->buffer[50], 0xE0);
	CU_ASSERT_EQUAL(stream_writer4->buffer[51], 0x03);

	CU_ASSERT_EQUAL(stream_writer4->buffer[52], 0x15);
	CU_ASSERT_EQUAL(stream_writer4->buffer[53], 0xDC);

	CU_ASSERT_EQUAL(stream_writer4->buffer[54], 0x15);
	CU_ASSERT_EQUAL(stream_writer4->buffer[55], 0xDC);

	CU_ASSERT_EQUAL(stream_writer4->buffer[56], 0x05);
	CU_ASSERT_EQUAL(stream_writer4->buffer[57], 0xDC);

	CU_ASSERT_EQUAL(stream_writer4->buffer[58], 0x05);
	CU_ASSERT_EQUAL(stream_writer4->buffer[59], 0xDC);

	CU_ASSERT_EQUAL(stream_writer4->buffer[60], 0x07);
	CU_ASSERT_EQUAL(stream_writer4->buffer[61], 0xFF);

	CU_ASSERT_EQUAL(stream_writer4->buffer[62], 0x07);
	CU_ASSERT_EQUAL(stream_writer4->buffer[63], 0xFE);

	CU_ASSERT_EQUAL(stream_writer4->buffer[64], 0x08);
	CU_ASSERT_EQUAL(stream_writer4->buffer[65], 0x02);

	free(stream);
	free(stream2);
	free(stream3);

	del_byte_stream_writer(stream_writer, 1);
	del_byte_stream_writer(stream_writer2, 1);
	del_byte_stream_writer(stream_writer3, 1);
	del_byte_stream_writer(stream_writer4, 1);
}

void test_encoder_data_apdu_encoder_1(void)
{
	DATA_apdu data_apdu;
	data_apdu.invoke_id = 0x4321;
	data_apdu.message.choice = RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN;
	data_apdu.message.length = 14;
	data_apdu.message.u.rors_cmipConfirmedEventReport.obj_handle = 0x0000;
	data_apdu.message.u.rors_cmipConfirmedEventReport.currentTime = 0;
	data_apdu.message.u.rors_cmipConfirmedEventReport.event_type = MDC_NOTI_CONFIG;
	data_apdu.message.u.rors_cmipConfirmedEventReport.event_reply_info.length = 4;
	intu8 info_value[] = {0x40, 0x00, 0x00, 0x00};
	data_apdu.message.u.rors_cmipConfirmedEventReport.event_reply_info.value = info_value;

	ByteStreamWriter *stream_writer = byte_stream_writer_instance(data_apdu.message.length + 6 /* id, choice and length*/);
	CU_ASSERT_PTR_NOT_NULL(stream_writer);

	encode_data_apdu(stream_writer, &data_apdu);

	int i;

	for (i = 0; i < data_apdu1_size; ++i) {
		printf("\n%X\t%X", stream_writer->buffer[i], data_apdu1_buffer[i]);
		CU_ASSERT_EQUAL(stream_writer->buffer[i], data_apdu1_buffer[i]);
	}

	del_byte_stream_writer(stream_writer, 1);
}

void test_encoder_data_apdu_encoder_2(void)
{
	/*
	 Why this test is commented??

	DATA_apdu data_apdu;
	data_apdu.invoke_id = 0x7654;
	data_apdu.message.choice = ROIV_CMIP_CONFIRMED_ACTION_CHOSEN;
	data_apdu.message.length = 22;
	data_apdu.message.u.roiv_cmipConfirmedAction.obj_handle = 0x0000;
	data_apdu.message.u.roiv_cmipConfirmedAction.action_type = MDC_ACT_DATA_REQUEST;
	data_apdu.message.u.roiv_cmipConfirmedAction.action_info_args.length = 16;

	DataRequest data_request;
	data_request.data_req_id = 0x0100;
	data_request.data_req_mode = DATA_REQ_START_STOP | DATA_REQ_SCOPlE_TYPE | DATA_REQ_MODE_SINGLE_RSP;
	data_request.data_req_time = 0x00000000;
	data_request.data_req_person_id = 0x0000;
	data_request.data_req_class = MDC_MOC_VMO_METRIC_NU;
	data_request.data_req_obj_handle_list.count = 0x0000;
	data_request.data_req_obj_handle_list.length = 0x0000;

	ByteStreamWriter *stream_writer2 = byte_stream_writer_instance(16);
	CU_ASSERT_PTR_NOT_NULL(stream_writer2);

	encode_datarequest(stream_writer2, &data_request);
	data_apdu.message.u.roiv_cmipConfirmedAction.action_info_args.value = stream_writer2->buffer;

	// id, choice and length
	ByteStreamWriter *stream_writer = byte_stream_writer_instance(data_apdu.message.length + 6);
	CU_ASSERT_PTR_NOT_NULL(stream_writer);
	encode_data_apdu(stream_writer, &data_apdu);

	int i;
	for (i = 0; i < data_apdu1_size; ++i) {
		printf("\n%X\t%X", stream_writer->buffer[i], data_apdu1_buffer[i]);
		CU_ASSERT_EQUAL(stream_writer->buffer[i], data_apdu1_buffer[i]);
	}

	del_byte_stream_writer(stream_writer);
	*/
}

void test_encoder_data_apdu_encoder_3(void)
{
	typedef enum {
			continua_version_struct = 1,
			continua_reg_struct = 2
	} ContinuaStructType_Values;

	typedef enum {
		auth_body_empty = 0,
		auth_body_ieee_11073 = 1,
		auth_body_continua = 2,
		auth_body_experimental = 254,
		auth_body_reserved = 255
	} AuthBody_Values;

	typedef intu16 CertifiedDeviceClassEntry;

	typedef struct CertifiedDeviceClassList {
		intu16 count;
		intu16 length;
		CertifiedDeviceClassEntry *value; /* first element of the array */
	} CertifiedDeviceClassList;

	typedef struct ContinuaBodyStruct {
		intu8 major_ig_number;
		intu8 minor_ig_number;
		CertifiedDeviceClassList certified_devices;
	} ContinuaBodyStruct;

	RegCertData data;
	data.auth_body_and_struc_type.auth_body = auth_body_continua; // intu8 1
	data.auth_body_and_struc_type.auth_body_struc_type = continua_version_struct; // intu8 1

	data.auth_body_data.length = 8;

	ByteStreamWriter *stream_writer = byte_stream_writer_instance(8);
	CU_ASSERT_PTR_NOT_NULL(stream_writer);

	ContinuaBodyStruct continua_body;
	continua_body.major_ig_number = 1; // 1
	continua_body.minor_ig_number = 5; // 1
	continua_body.certified_devices.count = 1; // 2
	continua_body.certified_devices.length = 2; // 2

	CertifiedDeviceClassEntry deviceEntry = 0x4008; // Thermometer with Wireless PAN
	continua_body.certified_devices.value = &deviceEntry; // 2

	write_intu8(stream_writer, continua_body.major_ig_number);
	write_intu8(stream_writer, continua_body.minor_ig_number);
	write_intu16(stream_writer, continua_body.certified_devices.count);
	write_intu16(stream_writer, continua_body.certified_devices.length);
	write_intu16(stream_writer, *(continua_body.certified_devices.value));

	data.auth_body_data.value = stream_writer->buffer;

	RegCertDataList dataList;
	dataList.count = 1;
	dataList.length = 12;
	dataList.value = &data;

	ByteStreamWriter *stream_writer2 = byte_stream_writer_instance(16);
	CU_ASSERT_PTR_NOT_NULL(stream_writer2);

	encode_regcertdatalist(stream_writer2, &dataList);

	/* Disabling outputs
	intu8* buffer = stream_writer2->buffer;
	int i;
	for (i = 0; i < 16; ++i) {
		printf("%X\t", buffer[i]);
	}
	ioutil_buffer_to_file("regCertDataList.bin", 16, buffer, 0);
	*/

	del_byte_stream_writer(stream_writer, 1);
	del_byte_stream_writer(stream_writer2, 1);
}

#endif
