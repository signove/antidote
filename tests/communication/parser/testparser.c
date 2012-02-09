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
 *
 * testparser.c
 *
 * Created on: Jun 16, 2010
 *     Author: mateus.lima
**********************************************************************/

#ifdef TEST_ENABLED

#include "Basic.h"
#include "src/asn1/phd_types.h"
#include "src/dim/nomenclature.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/util/bytelib.h"
#include "src/util/ioutil.h"
#include "tests/functional_test_cases/test_functional.h"

#include "testparser.h"

#include <stdlib.h>
#include <stdio.h>


int testparser_init_suite(void)
{
	return 0;
}

int testparser_finish_suite(void)
{
	return 0;
}

void testparser_add_suite()
{
	CU_pSuite suite = CU_add_suite("Parser Test Suite",
				       testparser_init_suite, testparser_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_parser_h211_apdu_parser",
		    test_parser_h211_apdu_parser);
	CU_add_test(suite, "test_parser_h221_apdu_parser",
		    test_parser_h221_apdu_parser);
	CU_add_test(suite, "test_parser_h233_apdu_parser",
		    test_parser_h233_apdu_parser);
	CU_add_test(suite, "test_parser_h241_apdu_parser",
		    test_parser_h241_apdu_parser);
	CU_add_test(suite, "test_parser_h244_apdu_parser",
		    test_parser_h244_apdu_parser);
	CU_add_test(suite, "test_parser_float_parser",
		    test_float_parser);
	CU_add_test(suite, "test_parser_sfloat_parser",
		    test_sfloat_parser);

	/* Add tests here - End */
}

void test_parser_h211_apdu_parser()
{
	int error = 0;

	// Create a ByteStreamReader
	unsigned long buffer_size = 0;
	unsigned char  *buffer = ioutil_buffer_from_file(apdu_H211, &buffer_size);

	CU_ASSERT(buffer[0] == 0xe2);

	ByteStreamReader *stream = byte_stream_reader_instance(buffer, buffer_size);
	CU_ASSERT_PTR_NOT_NULL(stream);

	// Create the APDU data type
	APDU *apdu = (APDU *) malloc(sizeof(APDU));
	CU_ASSERT_PTR_NOT_NULL(apdu);

	decode_apdu(stream, apdu, &error);

	CU_ASSERT(apdu->choice == 0xe200);
	CU_ASSERT(apdu->length == 50);

	CU_ASSERT(apdu->u.aarq.assoc_version == 0x80000000);
	CU_ASSERT(apdu->u.aarq.data_proto_list.count == 1);
	CU_ASSERT(apdu->u.aarq.data_proto_list.length == 42);

	CU_ASSERT(apdu->u.aarq.data_proto_list.value->data_proto_id == 20601);
	CU_ASSERT(apdu->u.aarq.data_proto_list.value->data_proto_info.length == 38);

	if (apdu->u.aarq.data_proto_list.value->data_proto_id == 20601) {
		PhdAssociationInformation assoc_information;

		intu8 *assoc_information_buffer = apdu->u.aarq.data_proto_list.value->data_proto_info.value;
		intu8 assoc_information_size = apdu->u.aarq.data_proto_list.value->data_proto_info.length;

		ByteStreamReader *stream2 = byte_stream_reader_instance(assoc_information_buffer, assoc_information_size);

		decode_phdassociationinformation(stream2, &assoc_information, &error);

		CU_ASSERT(assoc_information.protocolVersion == 0x80000000);
		CU_ASSERT(assoc_information.encodingRules == 0xA000);
		CU_ASSERT(assoc_information.nomenclatureVersion == 0x80000000);
		CU_ASSERT(assoc_information.functionalUnits == 0);
		CU_ASSERT(assoc_information.systemType == 0x00800000);

		CU_ASSERT(assoc_information.system_id.length == 8);
		CU_ASSERT(assoc_information.system_id.value[0] == 0x88);
		CU_ASSERT(assoc_information.system_id.value[1] == 0x77);
		CU_ASSERT(assoc_information.system_id.value[2] == 0x66);
		CU_ASSERT(assoc_information.system_id.value[3] == 0x55);
		CU_ASSERT(assoc_information.system_id.value[4] == 0x44);
		CU_ASSERT(assoc_information.system_id.value[5] == 0x33);
		CU_ASSERT(assoc_information.system_id.value[6] == 0x22);
		CU_ASSERT(assoc_information.system_id.value[7] == 0x11);

		CU_ASSERT(assoc_information.dev_config_id == 0x4000);

		CU_ASSERT(assoc_information.data_req_mode_capab.data_req_mode_flags == 0x0081);
		CU_ASSERT(assoc_information.data_req_mode_capab.data_req_init_agent_count == 0x01);
		CU_ASSERT(assoc_information.data_req_mode_capab.data_req_init_manager_count == 0x01);

		CU_ASSERT(assoc_information.optionList.count == 0);
		CU_ASSERT(assoc_information.optionList.length == 0);

		free(stream2);
		del_phdassociationinformation(&assoc_information);
	}

	del_apdu(apdu);

	free(apdu);
	free(stream);

	free(buffer);
	buffer = NULL;

}

void test_parser_h221_apdu_parser()
{
	int error = 0;
	unsigned long buffer_size = 0;
	unsigned char  *buffer = ioutil_buffer_from_file(apdu_H221, &buffer_size);

	CU_ASSERT(buffer[0] == 0xe7);


	// Create a ByteStreamReader
	ByteStreamReader *stream = byte_stream_reader_instance(buffer, buffer_size);
	CU_ASSERT_PTR_NOT_NULL(stream);

	// Create the APDU data type
	APDU *apdu = (APDU *) malloc(sizeof(APDU));
	CU_ASSERT_PTR_NOT_NULL(apdu);

	decode_apdu(stream, apdu, &error);

	CU_ASSERT(apdu->choice == 0xe700);
	CU_ASSERT(apdu->length == 112);

	DATA_apdu *data_apdu = encode_get_data_apdu(&apdu->u.prst);
	CU_ASSERT_EQUAL(data_apdu->invoke_id, 0x4321);
	CU_ASSERT_EQUAL(data_apdu->message.choice, ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN);
	CU_ASSERT_EQUAL(data_apdu->message.length, 104);

	CU_ASSERT_EQUAL(data_apdu->message.u.roiv_cmipConfirmedEventReport.obj_handle, 0);
	CU_ASSERT_EQUAL(data_apdu->message.u.roiv_cmipConfirmedEventReport.event_time, 0);
	CU_ASSERT_EQUAL(data_apdu->message.u.roiv_cmipConfirmedEventReport.event_type, MDC_NOTI_CONFIG);
	CU_ASSERT_EQUAL(data_apdu->message.u.roiv_cmipConfirmedEventReport.event_info.length, 94);

	if (data_apdu->message.u.roiv_cmipConfirmedEventReport.event_type == MDC_NOTI_CONFIG) {
		ConfigReport config;

		intu8 *config_buffer = data_apdu->message.u.roiv_cmipConfirmedEventReport.event_info.value;
		intu8 config_size = data_apdu->message.u.roiv_cmipConfirmedEventReport.event_info.length;

		ByteStreamReader *stream2 = byte_stream_reader_instance(config_buffer, config_size);
		decode_configreport(stream2, &config, &error);

		CU_ASSERT_EQUAL(config.config_report_id, 0x4000);
		CU_ASSERT_EQUAL(config.config_obj_list.count, 2);
		CU_ASSERT_EQUAL(config.config_obj_list.length, 88);

		CU_ASSERT_EQUAL(config.config_obj_list.value[0].obj_class, MDC_MOC_VMO_METRIC_NU);
		CU_ASSERT_EQUAL(config.config_obj_list.value[0].obj_handle, 1);
		CU_ASSERT_EQUAL(config.config_obj_list.value[0].attributes.count, 4);
		CU_ASSERT_EQUAL(config.config_obj_list.value[0].attributes.length, 36);

		CU_ASSERT_EQUAL(config.config_obj_list.value[0].attributes.value[0].attribute_id, MDC_ATTR_ID_TYPE);
		intu8 ava_size = config.config_obj_list.value[0].attributes.value[0].attribute_value.length;
		intu8 *ava_buffer = config.config_obj_list.value[0].attributes.value[0].attribute_value.value;
		ByteStreamReader *stream3 = byte_stream_reader_instance(ava_buffer, ava_size);
		TYPE type;
		decode_type(stream3, &type, &error);
		CU_ASSERT_EQUAL(type.partition, MDC_PART_SCADA);
		CU_ASSERT_EQUAL(type.code, MDC_MASS_BODY_ACTUAL);

		CU_ASSERT_EQUAL(config.config_obj_list.value[0].attributes.value[1].attribute_id, MDC_ATTR_METRIC_SPEC_SMALL);
		CU_ASSERT_EQUAL(config.config_obj_list.value[0].attributes.value[1].attribute_value.length, 2);
		intu8 ava2_size = config.config_obj_list.value[0].attributes.value[1].attribute_value.length;
		intu8 *ava2_buffer = config.config_obj_list.value[0].attributes.value[1].attribute_value.value;
		ByteStreamReader *stream4 = byte_stream_reader_instance(ava2_buffer, ava2_size);
		MetricSpecSmall spec;
		decode_metricspecsmall(stream4, &spec, &error);
		CU_ASSERT_EQUAL(spec, 0xD040);

		CU_ASSERT_EQUAL(config.config_obj_list.value[0].attributes.value[2].attribute_id, MDC_ATTR_UNIT_CODE);
		CU_ASSERT_EQUAL(config.config_obj_list.value[0].attributes.value[2].attribute_value.length, 2);
		intu8 ava3_size = config.config_obj_list.value[0].attributes.value[2].attribute_value.length;
		intu8 *ava3_buffer = config.config_obj_list.value[0].attributes.value[2].attribute_value.value;
		ByteStreamReader *stream5 = byte_stream_reader_instance(ava3_buffer, ava3_size);
		intu16 unit_code;
		decode_metricspecsmall(stream5, &unit_code, &error);
		CU_ASSERT_EQUAL(unit_code, MDC_DIM_KILO_G);

		CU_ASSERT_EQUAL(config.config_obj_list.value[0].attributes.value[3].attribute_id, MDC_ATTR_ATTRIBUTE_VAL_MAP);
		CU_ASSERT_EQUAL(config.config_obj_list.value[0].attributes.value[3].attribute_value.length, 12);
		intu8 ava4_size = config.config_obj_list.value[0].attributes.value[3].attribute_value.length;
		intu8 *ava4_buffer = config.config_obj_list.value[0].attributes.value[3].attribute_value.value;
		ByteStreamReader *stream6 = byte_stream_reader_instance(ava4_buffer, ava4_size);
		AttrValMap valmap;
		decode_attrvalmap(stream6, &valmap, &error);
		CU_ASSERT_EQUAL(valmap.count, 2);
		CU_ASSERT_EQUAL(valmap.length, 8);
		CU_ASSERT_EQUAL(valmap.value[0].attribute_id, MDC_ATTR_NU_VAL_OBS_SIMP);
		CU_ASSERT_EQUAL(valmap.value[0].attribute_len, 4);
		CU_ASSERT_EQUAL(valmap.value[1].attribute_id, MDC_ATTR_TIME_STAMP_ABS);
		CU_ASSERT_EQUAL(valmap.value[1].attribute_len, 8);

		CU_ASSERT_EQUAL(config.config_obj_list.value[1].obj_class, MDC_MOC_VMO_METRIC_NU);
		CU_ASSERT_EQUAL(config.config_obj_list.value[1].obj_handle, 2);
		CU_ASSERT_EQUAL(config.config_obj_list.value[1].attributes.count, 4);
		CU_ASSERT_EQUAL(config.config_obj_list.value[1].attributes.length, 36);

		CU_ASSERT_EQUAL(config.config_obj_list.value[1].attributes.value[0].attribute_id, MDC_ATTR_ID_TYPE);
		CU_ASSERT_EQUAL(config.config_obj_list.value[1].attributes.value[0].attribute_value.length, 4);
		intu8 size2 = config.config_obj_list.value[1].attributes.value[0].attribute_value.length;
		intu8 *buffer2 = config.config_obj_list.value[1].attributes.value[0].attribute_value.value;
		ByteStreamReader *stream7 = byte_stream_reader_instance(buffer2, size2);
		TYPE type2;
		decode_type(stream7, &type2, &error);
		CU_ASSERT_EQUAL(type2.partition, MDC_PART_SCADA);
		CU_ASSERT_EQUAL(type2.code, MDC_BODY_FAT);

		CU_ASSERT_EQUAL(config.config_obj_list.value[1].attributes.value[1].attribute_id, MDC_ATTR_METRIC_SPEC_SMALL);
		CU_ASSERT_EQUAL(config.config_obj_list.value[1].attributes.value[1].attribute_value.length, 2);
		intu8 size3 = config.config_obj_list.value[1].attributes.value[1].attribute_value.length;
		intu8 *buffer3 = config.config_obj_list.value[1].attributes.value[1].attribute_value.value;
		ByteStreamReader *stream8 = byte_stream_reader_instance(buffer3, size3);
		MetricSpecSmall spec2;
		decode_metricspecsmall(stream8, &spec2, &error);
		CU_ASSERT_EQUAL(spec2, 0xD042);

		CU_ASSERT_EQUAL(config.config_obj_list.value[1].attributes.value[2].attribute_id, MDC_ATTR_UNIT_CODE);
		CU_ASSERT_EQUAL(config.config_obj_list.value[1].attributes.value[2].attribute_value.length, 2);
		intu8 size4 = config.config_obj_list.value[1].attributes.value[2].attribute_value.length;
		intu8 *buffer4 = config.config_obj_list.value[1].attributes.value[2].attribute_value.value;
		ByteStreamReader *stream9 = byte_stream_reader_instance(buffer4, size4);
		intu16 unit_code2;
		decode_metricspecsmall(stream9, &unit_code2, &error);
		CU_ASSERT_EQUAL(unit_code2, MDC_DIM_PERCENT);

		CU_ASSERT_EQUAL(config.config_obj_list.value[1].attributes.value[3].attribute_id, MDC_ATTR_ATTRIBUTE_VAL_MAP);
		CU_ASSERT_EQUAL(config.config_obj_list.value[1].attributes.value[3].attribute_value.length, 12);
		intu8 size5 = config.config_obj_list.value[1].attributes.value[3].attribute_value.length;
		intu8 *buffer5 = config.config_obj_list.value[1].attributes.value[3].attribute_value.value;
		ByteStreamReader *stream10 = byte_stream_reader_instance(buffer5, size5);
		AttrValMap valmap2;
		decode_attrvalmap(stream10, &valmap2, &error);
		CU_ASSERT_EQUAL(valmap2.count, 2);
		CU_ASSERT_EQUAL(valmap2.length, 8);
		CU_ASSERT_EQUAL(valmap.value[0].attribute_id, MDC_ATTR_NU_VAL_OBS_SIMP);
		CU_ASSERT_EQUAL(valmap.value[0].attribute_len, 4);
		CU_ASSERT_EQUAL(valmap.value[1].attribute_id, MDC_ATTR_TIME_STAMP_ABS);
		CU_ASSERT_EQUAL(valmap.value[1].attribute_len, 8);

		del_configreport(&config);
		del_type(&type);
		del_metricspecsmall(&spec);
		del_attrvalmap(&valmap);
		del_type(&type2);
		del_metricspecsmall(&spec2);
		del_attrvalmap(&valmap2);

		free(stream2);
		free(stream3);
		free(stream4);
		free(stream5);
		free(stream6);
		free(stream7);
		free(stream8);
		free(stream9);
		free(stream10);
	}

	del_apdu(apdu);

	free(apdu);

	free(stream);

	free(buffer);
	buffer = NULL;

}

void test_parser_h233_apdu_parser()
{

	int error = 0;
	unsigned long buffer_size = 0;
	unsigned char  *buffer = ioutil_buffer_from_file(apdu_H233, &buffer_size);

	// Create a ByteStreamReader
	ByteStreamReader *stream = byte_stream_reader_instance(buffer, buffer_size);
	CU_ASSERT_PTR_NOT_NULL(stream);

	// Create the APDU data type
	APDU apdu;
	decode_apdu(stream, &apdu, &error);

	CU_ASSERT_EQUAL(apdu.choice, PRST_CHOSEN);
	CU_ASSERT_EQUAL(apdu.length, 110);
	CU_ASSERT_EQUAL(apdu.u.prst.length, 108);

	DATA_apdu *data_apdu = encode_get_data_apdu(&apdu.u.prst);
	CU_ASSERT_EQUAL(data_apdu->invoke_id, 0x3456);
	CU_ASSERT_EQUAL(data_apdu->message.choice, RORS_CMIP_GET_CHOSEN);
	CU_ASSERT_EQUAL(data_apdu->message.length, 102);

	CU_ASSERT_EQUAL(data_apdu->message.u.rors_cmipGet.obj_handle, 0);

	AttributeList attribute_list = data_apdu->message.u.rors_cmipGet.attribute_list;
	CU_ASSERT_EQUAL(attribute_list.count, 6);
	CU_ASSERT_EQUAL(attribute_list.length, 96);

	CU_ASSERT_EQUAL(attribute_list.value[0].attribute_id, MDC_ATTR_SYS_TYPE_SPEC_LIST);
	CU_ASSERT_EQUAL(attribute_list.value[0].attribute_value.length, 8);

	// Create a ByteStreamReader
	ByteStreamReader *stream1 = byte_stream_reader_instance(attribute_list.value[0].attribute_value.value,
				    attribute_list.value[0].attribute_value.length);
	CU_ASSERT_PTR_NOT_NULL(stream1);

	TypeVerList type_ver_list;
	decode_typeverlist(stream1, &type_ver_list, &error);

	CU_ASSERT_EQUAL(type_ver_list.count, 1);
	CU_ASSERT_EQUAL(type_ver_list.length, 4);
	CU_ASSERT_EQUAL(type_ver_list.value[0].type, MDC_DEV_SPEC_PROFILE_SCALE);
	CU_ASSERT_EQUAL(type_ver_list.value[0].version, 1);

	/************************************/

	CU_ASSERT_EQUAL(attribute_list.value[1].attribute_id, MDC_ATTR_ID_MODEL);
	CU_ASSERT_EQUAL(attribute_list.value[1].attribute_value.length, 26);

	// Create a ByteStreamReader
	ByteStreamReader *stream2 = byte_stream_reader_instance(attribute_list.value[1].attribute_value.value,
				    attribute_list.value[1].attribute_value.length);
	CU_ASSERT_PTR_NOT_NULL(stream2);

	SystemModel system_model;
	decode_systemmodel(stream2, &system_model, &error);
	CU_ASSERT_NSTRING_EQUAL(system_model.manufacturer.value, "TheCompany", 10);
	CU_ASSERT_NSTRING_EQUAL(system_model.model_number.value, "TheScaleABC\0", 12);

	/************************************/

	CU_ASSERT_EQUAL(attribute_list.value[2].attribute_id, MDC_ATTR_SYS_ID);
	CU_ASSERT_EQUAL(attribute_list.value[2].attribute_value.length, 10);

	// Create a ByteStreamReader
	ByteStreamReader *stream3 = byte_stream_reader_instance(attribute_list.value[2].attribute_value.value,
				    attribute_list.value[2].attribute_value.length);
	CU_ASSERT_PTR_NOT_NULL(stream3);

	octet_string system_ID;
	decode_octet_string(stream3, &system_ID, &error);

	CU_ASSERT_EQUAL(system_ID.length, 8);
	/* system_ID.value
	 * This attribute is an IEEE EUI-64 which consists of a 24-bit unique organizationally
	 * unique identifier (OUI) followed by a 40-bit manufacturer-defined ID.*/
	CU_ASSERT(system_ID.value[0] == 0x88);
	CU_ASSERT(system_ID.value[1] == 0x77);
	CU_ASSERT(system_ID.value[2] == 0x66);
	CU_ASSERT(system_ID.value[3] == 0x55);
	CU_ASSERT(system_ID.value[4] == 0x44);
	CU_ASSERT(system_ID.value[5] == 0x33);
	CU_ASSERT(system_ID.value[6] == 0x22);
	CU_ASSERT(system_ID.value[7] == 0x11);

	/************************************/

	CU_ASSERT_EQUAL(attribute_list.value[3].attribute_id, MDC_ATTR_DEV_CONFIG_ID);
	CU_ASSERT_EQUAL(attribute_list.value[3].attribute_value.length, 2);

	// Create a ByteStreamReader
	ByteStreamReader *stream4 = byte_stream_reader_instance(attribute_list.value[3].attribute_value.value,
				    attribute_list.value[3].attribute_value.length);
	CU_ASSERT_PTR_NOT_NULL(stream4);

	ConfigId config_ID;
	decode_configid(stream4, &config_ID, &error);
	CU_ASSERT_EQUAL(config_ID, 16384);

	/************************************/

	CU_ASSERT_EQUAL(attribute_list.value[4].attribute_id, MDC_ATTR_ID_PROD_SPECN);
	CU_ASSERT_EQUAL(attribute_list.value[4].attribute_value.length, 18);

	// Create a ByteStreamReader
	ByteStreamReader *stream5 = byte_stream_reader_instance(attribute_list.value[4].attribute_value.value,
				    attribute_list.value[4].attribute_value.length);
	CU_ASSERT_PTR_NOT_NULL(stream5);

	ProductionSpec prod_spec;
	decode_productionspec(stream5, &prod_spec, &error);

	CU_ASSERT_EQUAL(prod_spec.count, 1);
	CU_ASSERT_EQUAL(prod_spec.length, 14);
	CU_ASSERT_EQUAL(prod_spec.value->spec_type, 1);
	CU_ASSERT_EQUAL(prod_spec.value->component_id, 0);

	CU_ASSERT_NSTRING_EQUAL(prod_spec.value->prod_spec.value, "DE124567", prod_spec.value->prod_spec.length);

	/************************************/

	CU_ASSERT_EQUAL(attribute_list.value[5].attribute_id, MDC_ATTR_TIME_ABS);
	CU_ASSERT_EQUAL(attribute_list.value[5].attribute_value.length, 8);

	// Create a ByteStreamReader
	ByteStreamReader *stream6 = byte_stream_reader_instance(attribute_list.value[5].attribute_value.value,
				    attribute_list.value[5].attribute_value.length);
	CU_ASSERT_PTR_NOT_NULL(stream5);

	AbsoluteTime abs_time;
	decode_absolutetime(stream6, &abs_time, &error);

	CU_ASSERT_EQUAL(abs_time.century, 0x20);
	CU_ASSERT_EQUAL(abs_time.year, 0x07);
	CU_ASSERT_EQUAL(abs_time.month, 0x02);
	CU_ASSERT_EQUAL(abs_time.day, 0x01);
	CU_ASSERT_EQUAL(abs_time.hour, 0x12);
	CU_ASSERT_EQUAL(abs_time.minute, 0x05);
	CU_ASSERT_EQUAL(abs_time.second, 0x00);
	CU_ASSERT_EQUAL(abs_time.sec_fractions, 0x00);

	del_apdu(&apdu);

	del_typeverlist(&type_ver_list);
	del_systemmodel(&system_model);
	del_octet_string(&system_ID);
	del_configid(&config_ID);
	del_productionspec(&prod_spec);
	del_absolutetime(&abs_time);


	free(stream);
	free(stream1);
	free(stream2);
	free(stream3);
	free(stream4);
	free(stream5);
	free(stream6);

	free(buffer);
	buffer = NULL;
}

void test_parser_h241_apdu_parser()
{

	int error = 0;
	unsigned long buffer_size = 0;
	unsigned char  *buffer = ioutil_buffer_from_file(apdu_H241, &buffer_size);

	// Create a ByteStreamReader
	ByteStreamReader *stream = byte_stream_reader_instance(buffer, buffer_size);
	CU_ASSERT_PTR_NOT_NULL(stream);

	// Create the APDU data type
	APDU apdu;
	decode_apdu(stream, &apdu, &error);

	CU_ASSERT_EQUAL(apdu.choice, PRST_CHOSEN);
	CU_ASSERT_EQUAL(apdu.length, 58);
	CU_ASSERT_EQUAL(apdu.u.prst.length, 56);

	DATA_apdu *data_apdu = encode_get_data_apdu(&apdu.u.prst);
	CU_ASSERT_EQUAL(data_apdu->invoke_id, 0x4321);
	CU_ASSERT_EQUAL(data_apdu->message.choice, ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN);
	CU_ASSERT_EQUAL(data_apdu->message.length, 50);

	CU_ASSERT_EQUAL(data_apdu->message.u.roiv_cmipConfirmedEventReport.obj_handle, 0);
	CU_ASSERT_EQUAL(data_apdu->message.u.roiv_cmipConfirmedEventReport.event_time, 0);
	CU_ASSERT_EQUAL(data_apdu->message.u.roiv_cmipConfirmedEventReport.event_type, MDC_NOTI_SCAN_REPORT_FIXED);

	// Create a ByteStreamReader
	ByteStreamReader *event_info_stream = byte_stream_reader_instance(
			data_apdu->message.u.roiv_cmipConfirmedEventReport.event_info.value,
			data_apdu->message.u.roiv_cmipConfirmedEventReport.event_info.length);
	CU_ASSERT_PTR_NOT_NULL(event_info_stream);

	ScanReportInfoFixed report;
	decode_scanreportinfofixed(event_info_stream, &report, &error);

	CU_ASSERT_EQUAL(report.data_req_id, 0xF000);
	CU_ASSERT_EQUAL(report.scan_report_no, 0);
	ObservationScanFixedList scan_fixed_list = report.obs_scan_fixed;

	CU_ASSERT_EQUAL(scan_fixed_list.count, 2);
	CU_ASSERT_EQUAL(scan_fixed_list.length, 32);

	CU_ASSERT_EQUAL(scan_fixed_list.value[0].obj_handle, 1);
	CU_ASSERT_EQUAL(scan_fixed_list.value[0].obs_val_data.length, 12);

	// Create a ByteStreamReader
	ByteStreamReader *obs_val_data_stream = byte_stream_reader_instance(
			scan_fixed_list.value[0].obs_val_data.value,
			scan_fixed_list.value[0].obs_val_data.length);
	CU_ASSERT_PTR_NOT_NULL(obs_val_data_stream);

	SimpleNuObsValue nu_float_value = 0;
	decode_simplenuobsvalue(obs_val_data_stream, &nu_float_value, &error);
	CU_ASSERT_DOUBLE_EQUAL(nu_float_value, 62.4, 0.00001);

	AbsoluteTime absolute_time;
	decode_absolutetime(obs_val_data_stream, &absolute_time, &error);

	CU_ASSERT_EQUAL(absolute_time.century, 0x20);
	CU_ASSERT_EQUAL(absolute_time.year, 0x07);
	CU_ASSERT_EQUAL(absolute_time.month, 0x02);
	CU_ASSERT_EQUAL(absolute_time.day, 0x01);
	CU_ASSERT_EQUAL(absolute_time.hour, 0x12);
	CU_ASSERT_EQUAL(absolute_time.minute, 0x10);
	CU_ASSERT_EQUAL(absolute_time.second, 0x00);
	CU_ASSERT_EQUAL(absolute_time.sec_fractions, 0x00);

	CU_ASSERT_EQUAL(scan_fixed_list.value[1].obj_handle, 2);
	CU_ASSERT_EQUAL(scan_fixed_list.value[1].obs_val_data.length, 12);

	// Create a ByteStreamReader
	ByteStreamReader *obs_val_data_stream1 = byte_stream_reader_instance(
				scan_fixed_list.value[1].obs_val_data.value,
				scan_fixed_list.value[1].obs_val_data.length);
	CU_ASSERT_PTR_NOT_NULL(obs_val_data_stream1);

	SimpleNuObsValue nu_float_value1 = 0;
	decode_simplenuobsvalue(obs_val_data_stream1, &nu_float_value1, &error);
	CU_ASSERT_DOUBLE_EQUAL(nu_float_value1, 25.6, 0.00001);

	AbsoluteTime absolute_time1;
	decode_absolutetime(obs_val_data_stream1, &absolute_time1, &error);

	CU_ASSERT_EQUAL(absolute_time1.century, 0x20);
	CU_ASSERT_EQUAL(absolute_time1.year, 0x07);
	CU_ASSERT_EQUAL(absolute_time1.month, 0x02);
	CU_ASSERT_EQUAL(absolute_time1.day, 0x01);
	CU_ASSERT_EQUAL(absolute_time1.hour, 0x12);
	CU_ASSERT_EQUAL(absolute_time1.minute, 0x10);
	CU_ASSERT_EQUAL(absolute_time1.second, 0x00);
	CU_ASSERT_EQUAL(absolute_time1.sec_fractions, 0x00);


	del_apdu(&apdu);
	del_scanreportinfofixed(&report);
	del_absolutetime(&absolute_time);
	del_simplenuobsvalue(&nu_float_value1);
	del_absolutetime(&absolute_time1);

	free(stream);
	free(event_info_stream);
	free(obs_val_data_stream);
	free(obs_val_data_stream1);

	free(buffer);
	buffer = NULL;
}

void test_parser_h244_apdu_parser(void)
{

	int error = 0;
	unsigned long buffer_size = 0;
	unsigned char  *buffer = ioutil_buffer_from_file(apdu_H244, &buffer_size);



	// Create a ByteStreamReader
	ByteStreamReader *stream = byte_stream_reader_instance(buffer, buffer_size);
	CU_ASSERT_PTR_NOT_NULL(stream);

	// Create the APDU data type
	APDU apdu;
	decode_apdu(stream, &apdu, &error);

	CU_ASSERT_EQUAL(apdu.choice, PRST_CHOSEN);
	CU_ASSERT_EQUAL(apdu.length, 64);
	CU_ASSERT_EQUAL(apdu.u.prst.length, 62);

	DATA_apdu *data_apdu = encode_get_data_apdu(&apdu.u.prst);
	CU_ASSERT_EQUAL(data_apdu->invoke_id, 0x7654);
	CU_ASSERT_EQUAL(data_apdu->message.choice, RORS_CMIP_CONFIRMED_ACTION_CHOSEN);
	CU_ASSERT_EQUAL(data_apdu->message.length, 56);

	CU_ASSERT_EQUAL(data_apdu->message.u.rors_cmipConfirmedAction.obj_handle, 0);
	CU_ASSERT_EQUAL(data_apdu->message.u.rors_cmipConfirmedAction.action_type, MDC_ACT_DATA_REQUEST);
	CU_ASSERT_EQUAL(data_apdu->message.u.rors_cmipConfirmedAction.action_info_args.length, 50);

	// Create a ByteStreamReader
	ByteStreamReader *data_response_stream = byte_stream_reader_instance(
				data_apdu->message.u.rors_cmipConfirmedAction.action_info_args.value,
				data_apdu->message.u.rors_cmipConfirmedAction.action_info_args.length);
	CU_ASSERT_PTR_NOT_NULL(data_response_stream);

	DataResponse data_response;
	decode_dataresponse(data_response_stream, &data_response, &error);

	RelativeTime time = 0;
	CU_ASSERT_EQUAL(data_response.rel_time_stamp, time);

	CU_ASSERT_EQUAL(data_response.data_req_result, 0);
	CU_ASSERT_EQUAL(data_response.event_type, MDC_NOTI_SCAN_REPORT_FIXED);

	// Create a ByteStreamReader
	ByteStreamReader *event_info_stream = byte_stream_reader_instance(
			data_response.event_info.value,
			data_response.event_info.length);
	CU_ASSERT_PTR_NOT_NULL(event_info_stream);

	ScanReportInfoFixed report;
	decode_scanreportinfofixed(event_info_stream, &report, &error);

	CU_ASSERT_EQUAL(report.data_req_id, 0x0100);
	CU_ASSERT_EQUAL(report.scan_report_no, 0);
	ObservationScanFixedList scan_fixed_list = report.obs_scan_fixed;

	CU_ASSERT_EQUAL(scan_fixed_list.count, 2);
	CU_ASSERT_EQUAL(scan_fixed_list.length, 32);

	CU_ASSERT_EQUAL(scan_fixed_list.value[0].obj_handle, 1);
	CU_ASSERT_EQUAL(scan_fixed_list.value[0].obs_val_data.length, 12);

	// Create a ByteStreamReader
	ByteStreamReader *obs_val_data_stream = byte_stream_reader_instance(
			scan_fixed_list.value[0].obs_val_data.value,
			scan_fixed_list.value[0].obs_val_data.length);
	CU_ASSERT_PTR_NOT_NULL(obs_val_data_stream);

	SimpleNuObsValue nu_float_value = 0;
	decode_simplenuobsvalue(obs_val_data_stream, &nu_float_value, &error);
	CU_ASSERT_DOUBLE_EQUAL(nu_float_value, 62.4, 0.00001);

	AbsoluteTime absolute_time;
	decode_absolutetime(obs_val_data_stream, &absolute_time, &error);

	CU_ASSERT_EQUAL(absolute_time.century, 0x20);
	CU_ASSERT_EQUAL(absolute_time.year, 0x07);
	CU_ASSERT_EQUAL(absolute_time.month, 0x02);
	CU_ASSERT_EQUAL(absolute_time.day, 0x01);
	CU_ASSERT_EQUAL(absolute_time.hour, 0x12);
	CU_ASSERT_EQUAL(absolute_time.minute, 0x10);
	CU_ASSERT_EQUAL(absolute_time.second, 0x00);
	CU_ASSERT_EQUAL(absolute_time.sec_fractions, 0x00);

	CU_ASSERT_EQUAL(scan_fixed_list.value[1].obj_handle, 2);
	CU_ASSERT_EQUAL(scan_fixed_list.value[1].obs_val_data.length, 12);

	// Create a ByteStreamReader
	ByteStreamReader *obs_val_data_stream1 = byte_stream_reader_instance(
				scan_fixed_list.value[1].obs_val_data.value,
				scan_fixed_list.value[1].obs_val_data.length);
	CU_ASSERT_PTR_NOT_NULL(obs_val_data_stream1);

	SimpleNuObsValue nu_float_value1 = 0;
	decode_simplenuobsvalue(obs_val_data_stream1, &nu_float_value1, &error);
	CU_ASSERT_DOUBLE_EQUAL(nu_float_value1, 25.6, 0.00001);

	AbsoluteTime absolute_time1;
	decode_absolutetime(obs_val_data_stream1, &absolute_time1, &error);

	CU_ASSERT_EQUAL(absolute_time1.century, 0x20);
	CU_ASSERT_EQUAL(absolute_time1.year, 0x07);
	CU_ASSERT_EQUAL(absolute_time1.month, 0x02);
	CU_ASSERT_EQUAL(absolute_time1.day, 0x01);
	CU_ASSERT_EQUAL(absolute_time1.hour, 0x12);
	CU_ASSERT_EQUAL(absolute_time1.minute, 0x10);
	CU_ASSERT_EQUAL(absolute_time1.second, 0x00);
	CU_ASSERT_EQUAL(absolute_time1.sec_fractions, 0x00);

	del_apdu(&apdu);
	del_dataresponse(&data_response);
	del_dataresponse(&data_response);
	del_scanreportinfofixed(&report);
	del_simplenuobsvalue(&nu_float_value);
	del_absolutetime(&absolute_time);
	del_simplenuobsvalue(&nu_float_value1);
	del_absolutetime(&absolute_time1);

	free(stream);
	free(data_response_stream);
	free(event_info_stream);
	free(obs_val_data_stream);
	free(obs_val_data_stream1);

	free(buffer);
	buffer = NULL;
}

void test_float_parser()
{
	int error = 0;
	intu8 test_data[4] = {0xFB, 0x12, 0xd6, 0x87};
	ByteStreamReader *stream = byte_stream_reader_instance(test_data, 4);
	double data = read_float(stream, &error);
	printf("%f", data);
	CU_ASSERT_DOUBLE_EQUAL(data, 12.34567, 0.000001);
	free(stream);
}

void test_sfloat_parser()
{
	int error = 0;
	intu8 test_data[2] = {0xF3, 0xCF};
	ByteStreamReader *stream = byte_stream_reader_instance(test_data, 2);
	double data = read_sfloat(stream, &error);
	printf("%f", data);
	CU_ASSERT_DOUBLE_EQUAL(data, 97.5, 0.0001);
	free(stream);
}

#endif
