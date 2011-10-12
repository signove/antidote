/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file decoder_ASN1.c
 * \brief ASN1 decoder implementation.
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
 * \author Walter Guerra, Mateus Lima
 * \date Jun 11, 2010
 */

/**
 * \addtogroup ASN1Decoder
 *
 * \ingroup Parser
 *
 * @{
 */
#include "src/asn1/phd_types.h"
#include "encoder_ASN1.h"
#include "decoder_ASN1.h"

#include <stdlib.h>

/**
 * Decodes SegmentDataResult.
 *
 * @param stream the SegmentDataResult content decoded as ByteStreamReader.
 * @param pointer the SegmentDataResult to be decoded.
 */
void decode_segmentdataresult(ByteStreamReader *stream, SegmentDataResult *pointer)
{
	decode_segmdataeventdescr(stream, &pointer->segm_data_event_descr); // SegmDataEventDescr segm_data_event_descr
}

/**
 * Decodes ScanReportPerVar.
 *
 * @param stream the ScanReportPerVar content decoded as ByteStreamReader.
 * @param pointer the ScanReportPerVar to be decoded.
 */
void decode_scanreportpervar(ByteStreamReader *stream, ScanReportPerVar *pointer)
{
	pointer->person_id = read_intu16(stream, NULL); // intu16 person_id
	decode_observationscanlist(stream, &pointer->obs_scan_var); // ObservationScanList obs_scan_var
}

/**
 * Decodes TypeVer.
 *
 * @param stream the TypeVer content decoded as ByteStreamReader.
 * @param pointer the TypeVer to be decoded.
 */
void decode_typever(ByteStreamReader *stream, TypeVer *pointer)
{
	pointer->type = read_intu16(stream, NULL); // OID_Type type
	pointer->version = read_intu16(stream, NULL); // intu16 version
}

/**
 * Decodes ModificationList.
 *
 * @param stream the ModificationList content decoded as ByteStreamReader.
 * @param pointer the ModificationList to be decoded.
 */
void decode_modificationlist(ByteStreamReader *stream, ModificationList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		int i;

		pointer->value = calloc(pointer->count, sizeof(AttributeModEntry));

		for (i = 0; i < pointer->count; i++) {
			decode_attributemodentry(stream, pointer->value + i);
		}
	}
}

/**
 * Decodes ProductionSpec.
 *
 * @param stream the ProductionSpec content decoded as ByteStreamReader.
 * @param pointer the ProductionSpec to be decoded.
 */
void decode_productionspec(ByteStreamReader *stream, ProductionSpec *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ProdSpecEntry));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_prodspecentry(stream, pointer->value + i);
		}
	}
}

/**
 * Decodes ActionArgumentSimple.
 *
 * @param stream the ActionArgumentSimple content decoded as ByteStreamReader.
 * @param pointer the ActionArgumentSimple to be decoded.
 */
void decode_actionargumentsimple(ByteStreamReader *stream,
				 ActionArgumentSimple *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	pointer->action_type = read_intu16(stream, NULL); // OID_Type action_type
	decode_any(stream, &pointer->action_info_args); // Any action_info_args
}

/**
 * Decodes ScaleRangeSpec32.
 *
 * @param stream the ScaleRangeSpec32 content decoded as ByteStreamReader.
 * @param pointer the ScaleRangeSpec32 to be decoded.
 */
void decode_scalerangespec32(ByteStreamReader *stream, ScaleRangeSpec32 *pointer)
{
	pointer->lower_absolute_value = read_float(stream); // FLOAT_Type lower_absolute_value
	pointer->upper_absolute_value = read_float(stream); // FLOAT_Type upper_absolute_value
	pointer->lower_scaled_value = read_intu32(stream, NULL); // intu32 lower_scaled_value
	pointer->upper_scaled_value = read_intu32(stream, NULL); // intu32 upper_scaled_value
}

/**
 * Decodes AVA_Type.
 *
 * @param stream the AVA_Type content decoded as ByteStreamReader.
 * @param pointer the AVA_Type to be decoded.
 */
void decode_ava_type(ByteStreamReader *stream, AVA_Type *pointer)
{
	pointer->attribute_id = read_intu16(stream, NULL); // OID_Type attribute_id
	decode_any(stream, &pointer->attribute_value); // Any attribute_value
}

/**
 * Decodes ConfigReport.
 *
 * @param stream the ConfigReport content decoded as ByteStreamReader.
 * @param pointer the ConfigReport to be decoded.
 */
void decode_configreport(ByteStreamReader *stream, ConfigReport *pointer)
{
	pointer->config_report_id = read_intu16(stream, NULL); // ConfigId config_report_id
	decode_configobjectlist(stream, &pointer->config_obj_list); // ConfigObjectList config_obj_list
}

/**
 * Decodes AttrValMapEntry.
 *
 * @param stream the AttrValMapEntry content decoded as ByteStreamReader.
 * @param pointer the AttrValMapEntry to be decoded.
 */
void decode_attrvalmapentry(ByteStreamReader *stream, AttrValMapEntry *pointer)
{
	pointer->attribute_id = read_intu16(stream, NULL); // OID_Type attribute_id
	pointer->attribute_len = read_intu16(stream, NULL); // intu16 attribute_len
}

/**
 * Decodes AbsoluteTime
 *
 * @param stream the AbsoluteTime content decoded as ByteStreamReader.
 * @param pointer the AbsoluteTime to be decoded.
 */
void decode_absolutetime(ByteStreamReader *stream, AbsoluteTime *pointer)
{
	// TODO: Error handling
	pointer->century = read_intu8(stream, NULL); // intu8 century
	pointer->year = read_intu8(stream, NULL); // intu8 year
	pointer->month = read_intu8(stream, NULL); // intu8 month
	pointer->day = read_intu8(stream, NULL); // intu8 day
	pointer->hour = read_intu8(stream, NULL); // intu8 hour
	pointer->minute = read_intu8(stream, NULL); // intu8 minute
	pointer->second = read_intu8(stream, NULL); // intu8 second
	pointer->sec_fractions = read_intu8(stream, NULL); // intu8 sec_fractions
}

/**
 * Decodes NuObsValueCmp.
 *
 * @param stream the NuObsValueCmp content decoded as ByteStreamReader.
 * @param pointer the NuObsValueCmp to be decoded.
 */
void decode_nuobsvaluecmp(ByteStreamReader *stream, NuObsValueCmp *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(NuObsValue));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_nuobsvalue(stream, pointer->value + i);
		}
	}
}

/**
 * Decodes ScanReportInfoMPFixed.
 *
 * @param stream the ScanReportInfoMPFixed content decoded as ByteStreamReader.
 * @param pointer the ScanReportInfoMPFixed to be decoded.
 */
void decode_scanreportinfompfixed(ByteStreamReader *stream,
				  ScanReportInfoMPFixed *pointer)
{
	pointer->data_req_id = read_intu16(stream, NULL); // DataReqId data_req_id
	pointer->scan_report_no = read_intu16(stream, NULL); // intu16 scan_report_no
	decode_scanreportperfixedlist(stream, &pointer->scan_per_fixed); // ScanReportPerFixedList scan_per_fixed
}

/**
 * Decodes RejectResult.
 *
 * @param stream the RejectResult content decoded as ByteStreamReader.
 * @param pointer the RejectResult  to be decoded.
 */
void decode_rejectresult(ByteStreamReader *stream, RejectResult *pointer)
{
	pointer->problem = read_intu16(stream, NULL); // RorjProblem problem
}

/**
 * Decodes ManufSpecAssociationInformation.
 *
 * @param stream the ManufSpecAssociationInformation content decoded as ByteStreamReader.
 * @param pointer the ManufSpecAssociationInformation to be decoded.
 */
void decode_manufspecassociationinformation(ByteStreamReader *stream,
		ManufSpecAssociationInformation *pointer)
{
	decode_uuid_ident(stream, &pointer->data_proto_id_ext); // UUID_Ident data_proto_id_ext
	decode_any(stream, &pointer->data_proto_info_ext); // Any data_proto_info_ext
}

/**
 * Decodes EnumObsValue.
 *
 * @param stream the EnumObsValue content decoded as ByteStreamReader.
 * @param pointer the EnumObsValue to be decoded.
 */
void decode_enumobsvalue(ByteStreamReader *stream, EnumObsValue *pointer)
{
	pointer->metric_id = read_intu16(stream, NULL); // OID_Type metric_id
	pointer->state = read_intu16(stream, NULL); // MeasurementStatus state
	decode_enumval(stream, &pointer->value); // EnumVal value
}

/**
 * Decodes octet_string.
 *
 * @param stream the octet_string content decoded as ByteStreamReader.
 * @param pointer the octet_string to be decoded.
 */
void decode_octet_string(ByteStreamReader *stream, octet_string *pointer)
{
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->length > 0) {
		pointer->value = calloc(pointer->length, sizeof(intu8));

		if (pointer->value == NULL) {
			return;
		}

		read_intu8_many(stream, pointer->value, pointer->length, NULL);
	}
}

/**
 * Decodes HighResRelativeTime.
 *
 * @param stream the HighResRelativeTime content decoded as ByteStreamReader.
 * @param pointer the HighResRelativeTime to be decoded.
 */
void decode_highresrelativetime(ByteStreamReader *stream,
				HighResRelativeTime *pointer)
{
	// intu8 value[8];
	int i;

	for (i = 0; i < 8; i++) {
		// TODO: ERROR handling
		*(pointer->value + i) = read_intu8(stream, NULL);
	}

}

/**
 * Decodes SampleType.
 *
 * @param stream the SampleType content decoded as ByteStreamReader.
 * @param pointer the SampleType to be decoded.
 */
void decode_sampletype(ByteStreamReader *stream, SampleType *pointer)
{
	pointer->sample_size = read_intu8(stream, NULL); // intu8 sample_size
	pointer->significant_bits = read_intu8(stream, NULL); // intu8 significant_bits
}

/**
 * Decodes AttributeList.
 *
 * @param stream the AttributeList content decoded as ByteStreamReader.
 * @param pointer the AttributeList to be decoded.
 */
void decode_attributelist(ByteStreamReader *stream, AttributeList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(AVA_Type));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_ava_type(stream, pointer->value + i);
		}
	}
}

/**
 * Decode SegmIdList
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmidlist(ByteStreamReader *stream, SegmIdList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(InstNumber));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_intu16(stream, NULL);
		}
	}
}

/**
 * Decode SimpleNuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 */
void decode_simplenuobsvaluecmp(ByteStreamReader *stream,
				SimpleNuObsValueCmp *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(SimpleNuObsValue));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_float(stream);
		}
	}
}

/**
 * Decode GetResultSimple
 *
 * @param *stream
 * @param *pointer
 */
void decode_getresultsimple(ByteStreamReader *stream, GetResultSimple *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	decode_attributelist(stream, &pointer->attribute_list); // AttributeList attribute_list
}

/**
 * Decode HANDLEList
 *
 * @param *stream
 * @param *pointer
 */
void decode_handlelist(ByteStreamReader *stream, HANDLEList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(HANDLE));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_intu16(stream, NULL);
		}
	}
}

/**
 * Decode SegmDataEventDescr
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmdataeventdescr(ByteStreamReader *stream, SegmDataEventDescr *pointer)
{
	pointer->segm_instance = read_intu16(stream, NULL); // InstNumber segm_instance
	pointer->segm_evt_entry_index = read_intu32(stream, NULL); // intu32 segm_evt_entry_index
	pointer->segm_evt_entry_count = read_intu32(stream, NULL); // intu32 segm_evt_entry_count
	pointer->segm_evt_status = read_intu16(stream, NULL); // SegmEvtStatus segm_evt_status
}

/**
 * Decode AttrValMap
 *
 * @param *stream
 * @param *pointer
 */
void decode_attrvalmap(ByteStreamReader *stream, AttrValMap *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(AttrValMapEntry));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_attrvalmapentry(stream, pointer->value + i);
		}
	}
}

/**
 * Decode ScaleRangeSpec8
 *
 * @param *stream
 * @param *pointer
 */
void decode_scalerangespec8(ByteStreamReader *stream, ScaleRangeSpec8 *pointer)
{
	pointer->lower_absolute_value = read_float(stream); // FLOAT_Type lower_absolute_value
	pointer->upper_absolute_value = read_float(stream); // FLOAT_Type upper_absolute_value
	pointer->lower_scaled_value = read_intu8(stream, NULL); // intu8 lower_scaled_value
	pointer->upper_scaled_value = read_intu8(stream, NULL); // intu8 upper_scaled_value
}

/**
 * Decode PhdAssociationInformation
 *
 * @param *stream
 * @param *pointer
 */
void decode_phdassociationinformation(ByteStreamReader *stream,
				      PhdAssociationInformation *pointer)
{
	pointer->protocolVersion = read_intu32(stream, NULL); // ProtocolVersion protocolVersion
	pointer->encodingRules = read_intu16(stream, NULL); // EncodingRules encodingRules
	pointer->nomenclatureVersion = read_intu32(stream, NULL); // NomenclatureVersion nomenclatureVersion
	pointer->functionalUnits = read_intu32(stream, NULL); // FunctionalUnits functionalUnits
	pointer->systemType = read_intu32(stream, NULL); // SystemType systemType
	decode_octet_string(stream, &pointer->system_id); // octet_string system_id
	pointer->dev_config_id = read_intu16(stream, NULL); // intu16 dev_config_id
	decode_datareqmodecapab(stream, &pointer->data_req_mode_capab); // DataReqModeCapab data_req_mode_capab
	decode_attributelist(stream, &pointer->optionList); // AttributeList optionList
}

/**
 * Decode ScanReportPerFixedList
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportperfixedlist(ByteStreamReader *stream,
				   ScanReportPerFixedList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ScanReportPerFixed));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_scanreportperfixed(stream, pointer->value + i);
		}
	}
}

/**
 * Decode ScanReportPerGroupedList
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportpergroupedlist(ByteStreamReader *stream,
				     ScanReportPerGroupedList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ScanReportPerGrouped));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_scanreportpergrouped(stream, pointer->value + i);
		}
	}
}

/**
 * Decode DataProtoList
 *
 * @param *stream
 * @param *pointer
 */
void decode_dataprotolist(ByteStreamReader *stream, DataProtoList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(DataProto));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_dataproto(stream, pointer->value + i);
		}
	}
}

/**
 * Decode SegmSelection
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmselection(ByteStreamReader *stream, SegmSelection *pointer)
{
	pointer->choice = read_intu16(stream, NULL); // SegmSelection_choice choice
	pointer->length = read_intu16(stream, NULL); // intu16 length

	switch (pointer->choice) {
	case ALL_SEGMENTS_CHOSEN:
		pointer->u.all_segments = read_intu16(stream, NULL);
		break;
	case SEGM_ID_LIST_CHOSEN:
		decode_segmidlist(stream, &pointer->u.segm_id_list);
		break;
	case ABS_TIME_RANGE_CHOSEN:
		decode_abstimerange(stream, &pointer->u.abs_time_range);
		break;
	default:
		// TODO: manage errors
		break;
	}
}

/**
 * Decode ErrorResult
 *
 * @param *stream
 * @param *pointer
 */
void decode_errorresult(ByteStreamReader *stream, ErrorResult *pointer)
{
	pointer->error_value = read_intu16(stream, NULL); // ERROR error_value
	decode_any(stream, &pointer->parameter); // Any parameter
}

/**
 * Decode HandleAttrValMap
 *
 * @param *stream
 * @param *pointer
 */
void decode_handleattrvalmap(ByteStreamReader *stream, HandleAttrValMap *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(HandleAttrValMapEntry));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_handleattrvalmapentry(stream, pointer->value + i);
		}
	}
}

/**
 * Decode AbsoluteTimeAdjust
 *
 * @param *stream
 * @param *pointer
 */
void decode_absolutetimeadjust(ByteStreamReader *stream, AbsoluteTimeAdjust *pointer)
{
	// intu8 value[6]
	int i;

	for (i = 0; i < 6; i++) {
		*(pointer->value + i) = read_intu8(stream, NULL);
	}

}

/**
 * Decode AARE_apdu
 *
 * @param *stream
 * @param *pointer
 */
void decode_aare_apdu(ByteStreamReader *stream, AARE_apdu *pointer)
{
	pointer->result = read_intu16(stream, NULL); // Associate_result result
	decode_dataproto(stream, &pointer->selected_data_proto); // DataProto selected_data_proto
}

/**
 * Decode RLRE_apdu
 *
 * @param *stream
 * @param *pointer
 */
void decode_rlre_apdu(ByteStreamReader *stream, RLRE_apdu *pointer)
{
	pointer->reason = read_intu16(stream, NULL); // Release_response_reason reason
}

/**
 * Decode MetricIdList
 *
 * @param *stream
 * @param *pointer
 */
void decode_metricidlist(ByteStreamReader *stream, MetricIdList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(OID_Type));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_intu16(stream, NULL);
		}
	}
}

/**
 * Decode ScanReportPerFixed
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportperfixed(ByteStreamReader *stream, ScanReportPerFixed *pointer)
{
	pointer->person_id = read_intu16(stream, NULL); // intu16 person_id
	decode_observationscanfixedlist(stream, &pointer->obs_scan_fix); // ObservationScanFixedList obs_scan_fix
}

/**
 * Decode ScanReportInfoGrouped
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportinfogrouped(ByteStreamReader *stream,
				  ScanReportInfoGrouped *pointer)
{
	pointer->data_req_id = read_intu16(stream, NULL); // intu16 data_req_id
	pointer->scan_report_no = read_intu16(stream, NULL); // intu16 scan_report_no
	decode_scanreportinfogroupedlist(stream, &pointer->obs_scan_grouped); // ScanReportInfoGroupedList obs_scan_grouped
}

/**
 * Decode ObservationScan
 *
 * @param *stream
 * @param *pointer
 */
void decode_observationscan(ByteStreamReader *stream, ObservationScan *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	decode_attributelist(stream, &pointer->attributes); // AttributeList attributes
}

/**
 * Decode ScanReportPerGrouped
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportpergrouped(ByteStreamReader *stream,
				 ScanReportPerGrouped *pointer)
{
	pointer->person_id = read_intu16(stream, NULL); // PersonId person_id
	decode_octet_string(stream, &pointer->obs_scan_grouped);
}

/**
 * Decode SystemModel
 *
 * @param *stream
 * @param *pointer
 */
void decode_systemmodel(ByteStreamReader *stream, SystemModel *pointer)
{
	decode_octet_string(stream, &pointer->manufacturer); // octet_string manufacturer
	decode_octet_string(stream, &pointer->model_number); // octet_string model_number
}

/**
 * Decode ObservationScanList
 *
 * @param *stream
 * @param *pointer
 */
void decode_observationscanlist(ByteStreamReader *stream,
				ObservationScanList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ObservationScan));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_observationscan(stream, pointer->value + i);
		}
	}
}

/**
 * Decode APDU
 *
 * @param *stream
 * @param *pointer
 */
void decode_apdu(ByteStreamReader *stream, APDU *pointer)
{
	pointer->choice = read_intu16(stream, NULL); // APDU_choice choice
	pointer->length = read_intu16(stream, NULL); // intu16 length

	switch (pointer->choice) {
	case AARQ_CHOSEN:
		decode_aarq_apdu(stream, &pointer->u.aarq);
		break;
	case AARE_CHOSEN:
		decode_aare_apdu(stream, &pointer->u.aare);
		break;
	case RLRQ_CHOSEN:
		decode_rlrq_apdu(stream, &pointer->u.rlrq);
		break;
	case RLRE_CHOSEN:
		decode_rlre_apdu(stream, &pointer->u.rlre);
		break;
	case ABRT_CHOSEN:
		decode_abrt_apdu(stream, &pointer->u.abrt);
		break;
	case PRST_CHOSEN:
		decode_prst_apdu(stream, &pointer->u.prst);
		break;
	default:
		// TODO: manage errors
		break;
	}
}

/**
 * Decode PRST_apdu
 *
 * @param *stream
 * @param *pointer
 */
void decode_prst_apdu(ByteStreamReader *stream, PRST_apdu *pointer)
{
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->length > 0) {
		DATA_apdu *data = calloc(1, sizeof(DATA_apdu));

		if (data == NULL) {
			return;
		} else {
			decode_data_apdu(stream, data);

			encode_set_data_apdu(pointer, data);
		} /* else / TODO: manage errors */
	}
}

/**
 * Decode PmSegmentEntryMap
 *
 * @param *stream
 * @param *pointer
 */
void decode_pmsegmententrymap(ByteStreamReader *stream, PmSegmentEntryMap *pointer)
{
	pointer->segm_entry_header = read_intu16(stream, NULL); // SegmEntryHeader segm_entry_header
	decode_segmentryelemlist(stream, &pointer->segm_entry_elem_list); // SegmEntryElemList segm_entry_elem_list
}

/**
 * Decode Any
 *
 * @param *stream
 * @param *pointer
 */
void decode_any(ByteStreamReader *stream, Any *pointer)
{
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->length > 0) {
		pointer->value = calloc(pointer->length, sizeof(intu8));

		if (pointer->value == NULL) {
			return;
		}

		read_intu8_many(stream, pointer->value, pointer->length, NULL);

		// TODO contents of Any in case of error?
	}
}

/**
 * Decode SetArgumentSimple
 *
 * @param *stream
 * @param *pointer
 */
void decode_setargumentsimple(ByteStreamReader *stream, SetArgumentSimple *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	decode_modificationlist(stream, &pointer->modification_list); // ModificationList modification_list
}

/**
 * Decode SegmentInfo
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmentinfo(ByteStreamReader *stream, SegmentInfo *pointer)
{
	pointer->seg_inst_no = read_intu16(stream, NULL); // InstNumber seg_inst_no
	decode_attributelist(stream, &pointer->seg_info); // AttributeList seg_info
}

/**
 * Decode PmSegmElemStaticAttrList
 *
 * @param *stream
 * @param *pointer
 */
void decode_pmsegmelemstaticattrlist(ByteStreamReader *stream,
				     PmSegmElemStaticAttrList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(SegmElemStaticAttrEntry));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_segmelemstaticattrentry(stream, pointer->value + i);
		}
	}
}

/**
 * Decode AbsTimeRange
 *
 * @param *stream
 * @param *pointer
 */
void decode_abstimerange(ByteStreamReader *stream, AbsTimeRange *pointer)
{
	decode_absolutetime(stream, &pointer->from_time); // AbsoluteTime from_time
	decode_absolutetime(stream, &pointer->to_time); // AbsoluteTime to_time
}

/**
 * Decode ScanReportInfoMPVar
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportinfompvar(ByteStreamReader *stream,
				ScanReportInfoMPVar *pointer)
{
	pointer->data_req_id = read_intu16(stream, NULL); // DataReqId data_req_id
	pointer->scan_report_no = read_intu16(stream, NULL); // intu16 scan_report_no
	decode_scanreportpervarlist(stream, &pointer->scan_per_var); // ScanReportPerVarList scan_per_var
}

/**
 * Decode UUID_Ident
 *
 * @param *stream
 * @param *pointer
 */
void decode_uuid_ident(ByteStreamReader *stream, UUID_Ident *pointer)
{
	// intu8 value[16]
	int i;

	for (i = 0; i < 16; i++) {
		*(pointer->value + i) = read_intu8(stream, NULL);
	}
}

/**
 * Decode GetArgumentSimple
 *
 * @param *stream
 * @param *pointer
 */
void decode_getargumentsimple(ByteStreamReader *stream, GetArgumentSimple *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	decode_attributeidlist(stream, &pointer->attribute_id_list); // AttributeIdList attribute_id_list
}

/**
 * Decode RegCertDataList
 *
 * @param *stream
 * @param *pointer
 */
void decode_regcertdatalist(ByteStreamReader *stream, RegCertDataList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(RegCertData));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_regcertdata(stream, pointer->value + i);
		}
	}
}

/**
 * Decode ConfirReportRsp
 *
 * @param *stream
 * @param *pointer
 */
void decode_configreportrsp(ByteStreamReader *stream, ConfigReportRsp *pointer)
{
	pointer->config_report_id = read_intu16(stream, NULL); // ConfigId config_report_id
	pointer->config_result = read_intu16(stream, NULL); // ConfigResult config_result
}

/**
 * Decode DataProto
 *
 * @param *stream
 * @param *pointer
 */
void decode_dataproto(ByteStreamReader *stream, DataProto *pointer)
{
	pointer->data_proto_id = read_intu16(stream, NULL); // DataProtoId data_proto_id
	decode_any(stream, &pointer->data_proto_info); // Any data_proto_info
}

/**
 * Decode MetricStructureSmall
 *
 * @param *stream
 * @param *pointer
 */
void decode_metricstructuresmall(ByteStreamReader *stream,
				 MetricStructureSmall *pointer)
{
	pointer->ms_struct = read_intu8(stream, NULL); // intu8 ms_struct
	pointer->ms_comp_no = read_intu8(stream, NULL); // intu8 ms_comp_no
}

/**
 * Decode SegmentStatisticEntry
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmentstatisticentry(ByteStreamReader *stream,
				  SegmentStatisticEntry *pointer)
{
	pointer->segm_stat_type = read_intu16(stream, NULL); // SegmStatType segm_stat_type
	decode_octet_string(stream, &pointer->segm_stat_entry); // octet_string segm_stat_entry
}

/**
 * Decode SegmentDataEvent
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmentdataevent(ByteStreamReader *stream, SegmentDataEvent *pointer)
{
	decode_segmdataeventdescr(stream, &pointer->segm_data_event_descr); // SegmDataEventDescr segm_data_event_descr
	decode_octet_string(stream, &pointer->segm_data_event_entries); // octet_string segm_data_event_entries
}

/**
 * Decode SegmEntryElemList
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmentryelemlist(ByteStreamReader *stream, SegmEntryElemList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(SegmEntryElem));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_segmentryelem(stream, pointer->value + i);
		}
	}
}

/**
 * Decode SaSpec
 *
 * @param *stream
 * @param *pointer
 */
void decode_saspec(ByteStreamReader *stream, SaSpec *pointer)
{
	pointer->array_size = read_intu16(stream, NULL); // intu16 array_size
	decode_sampletype(stream, &pointer->sample_type); // SampleType sample_type
	pointer->flags = read_intu16(stream, NULL); // SaFlags flags
}

/**
 * Decode AttributeModEntry
 *
 * @param *stream
 * @param *pointer
 */
void decode_attributemodentry(ByteStreamReader *stream, AttributeModEntry *pointer)
{
	pointer->modify_operator = read_intu16(stream, NULL); // ModifyOperator modify_operator
	decode_ava_type(stream, &pointer->attribute); // AVA_Type attribute
}

/**
 * Decode MdsTimeInfo
 *
 * @param *stream
 * @param *pointer
 */
void decode_mdstimeinfo(ByteStreamReader *stream, MdsTimeInfo *pointer)
{
	pointer->mds_time_cap_state = read_intu16(stream, NULL); // MdsTimeCapState mds_time_cap_state
	pointer->time_sync_protocol = read_intu16(stream, NULL); // TimeProtocolId time_sync_protocol
	pointer->time_sync_accuracy = read_intu32(stream, NULL); // RelativeTime time_sync_accuracy
	pointer->time_resolution_abs_time = read_intu16(stream, NULL); // intu16 time_resolution_abs_time
	pointer->time_resolution_rel_time = read_intu16(stream, NULL); // intu16 time_resolution_rel_time
	pointer->time_resolution_high_res_time = read_intu32(stream, NULL); // intu32 time_resolution_high_res_time
}

/**
 * Decode EnumVal
 *
 * @param *stream
 * @param *pointer
 */
void decode_enumval(ByteStreamReader *stream, EnumVal *pointer)
{
	pointer->choice = read_intu16(stream, NULL); // EnumVal_choice choice
	pointer->length = read_intu16(stream, NULL); // intu16 length

	switch (pointer->choice) {
	case OBJ_ID_CHOSEN:
		pointer->u.enum_obj_id = read_intu16(stream, NULL);
		break;
	case TEXT_STRING_CHOSEN:
		decode_octet_string(stream, &pointer->u.enum_text_string);
		break;
	case BIT_STR_CHOSEN:
		pointer->u.enum_bit_str = read_intu32(stream, NULL);
		break;
	default:
		break;
	}
}

/**
 * Decode TrigSegmDataXferReq
 *
 * @param *stream
 * @param *pointer
 */
void decode_trigsegmdataxferreq(ByteStreamReader *stream,
				TrigSegmDataXferReq *pointer)
{
	pointer->seg_inst_no = read_intu16(stream, NULL); // InstNumber seg_inst_no
}

/**
 * Decode BatMeasure
 *
 * @param *stream
 * @param *pointer
 */
void decode_batmeasure(ByteStreamReader *stream, BatMeasure *pointer)
{
	pointer->value = read_float(stream); // FLOAT_Type value
	pointer->unit = read_intu16(stream, NULL); // OID_Type unit
}

/**
 * Decode SegmentStatistics
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmentstatistics(ByteStreamReader *stream, SegmentStatistics *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(SegmentStatisticEntry));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_segmentstatisticentry(stream, pointer->value + i);
		}
	}
}

/**
 * Decode AttributeIdList
 *
 * @param *stream
 * @param *pointer
 */
void decode_attributeidlist(ByteStreamReader *stream, AttributeIdList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(OID_Type));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_intu16(stream, NULL);
		}
	}
}

/**
 * Decode ScanReportInfoFixed
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportinfofixed(ByteStreamReader *stream,
				ScanReportInfoFixed *pointer)
{
	pointer->data_req_id = read_intu16(stream, NULL); // DataReqId data_req_id
	pointer->scan_report_no = read_intu16(stream, NULL); // intu16 scan_report_no
	decode_observationscanfixedlist(stream, &pointer->obs_scan_fixed); // ObservationScanFixedList obs_scan_fixed
}

/**
 * Decode DataRequest
 *
 * @param *stream
 * @param *pointer
 */
void decode_datarequest(ByteStreamReader *stream, DataRequest *pointer)
{
	pointer->data_req_id = read_intu16(stream, NULL); // DataReqId data_req_id
	pointer->data_req_mode = read_intu16(stream, NULL); // DataReqMode data_req_mode
	pointer->data_req_time = read_intu32(stream, NULL); // RelativeTime data_req_time
	pointer->data_req_person_id = read_intu16(stream, NULL); // intu16 DataRequest_data_req_person_id
	pointer->data_req_class = read_intu16(stream, NULL); // OID_Type data_req_class
	decode_handlelist(stream, &pointer->data_req_obj_handle_list); // HANDLEList data_req_obj_handle_list
}

/**
 * Decode AuthBodyAndStrucType
 *
 * @param *stream
 * @param *pointer
 */
void decode_authbodyandstructype(ByteStreamReader *stream,
				 AuthBodyAndStrucType *pointer)
{
	pointer->auth_body = read_intu8(stream, NULL); // AuthBody auth_body
	pointer->auth_body_struc_type = read_intu8(stream, NULL); // AuthBodyStrucType auth_body_struc_type
}

/**
 * Decode RLRQ_apdu
 *
 * @param *stream
 * @param *pointer
 */
void decode_rlrq_apdu(ByteStreamReader *stream, RLRQ_apdu *pointer)
{
	pointer->reason = read_intu16(stream, NULL); // Release_request_reason reason
}

/**
 * Decode Data_apdu_message
 *
 * @param *stream
 * @param *pointer
 */
void decode_data_apdu_message(ByteStreamReader *stream, Data_apdu_message *pointer)
{
	pointer->choice = read_intu16(stream, NULL); // DATA_apdu_choice choice
	pointer->length = read_intu16(stream, NULL); // intu16 length

	switch (pointer->choice) {
	case ROIV_CMIP_EVENT_REPORT_CHOSEN:
		decode_eventreportargumentsimple(stream,
						 &pointer->u.roiv_cmipEventReport);
		break;
	case ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		decode_eventreportargumentsimple(stream,
						 &pointer->u.roiv_cmipConfirmedEventReport);
		break;
	case ROIV_CMIP_GET_CHOSEN:
		decode_getargumentsimple(stream, &pointer->u.roiv_cmipGet);
		break;
	case ROIV_CMIP_SET_CHOSEN:
		decode_setargumentsimple(stream, &pointer->u.roiv_cmipSet);
		break;
	case ROIV_CMIP_CONFIRMED_SET_CHOSEN:
		decode_setargumentsimple(stream,
					 &pointer->u.roiv_cmipConfirmedSet);
		break;
	case ROIV_CMIP_ACTION_CHOSEN:
		decode_actionargumentsimple(stream, &pointer->u.roiv_cmipAction);
		break;
	case ROIV_CMIP_CONFIRMED_ACTION_CHOSEN:
		decode_actionargumentsimple(stream,
					    &pointer->u.roiv_cmipConfirmedAction);
		break;
	case RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		decode_eventreportresultsimple(stream,
					       &pointer->u.rors_cmipConfirmedEventReport);
		break;
	case RORS_CMIP_GET_CHOSEN:
		decode_getresultsimple(stream, &pointer->u.rors_cmipGet);
		break;
	case RORS_CMIP_CONFIRMED_SET_CHOSEN:
		decode_setresultsimple(stream,
				       &pointer->u.rors_cmipConfirmedSet);
		break;
	case RORS_CMIP_CONFIRMED_ACTION_CHOSEN:
		decode_actionresultsimple(stream,
					  &pointer->u.rors_cmipConfirmedAction);
		break;
	case ROER_CHOSEN:
		decode_errorresult(stream, &pointer->u.roer);
		break;
	case RORJ_CHOSEN:
		decode_rejectresult(stream, &pointer->u.rorj);
		break;
	default:
		// TODO: manage errors
		break;
	}
}

/**
 * Decode EventReportArgumentSimple
 *
 * @param *stream
 * @param *pointer
 */
void decode_eventreportargumentsimple(ByteStreamReader *stream,
				      EventReportArgumentSimple *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	pointer->event_time = read_intu32(stream, NULL); // RelativeTime event_time
	pointer->event_type = read_intu16(stream, NULL); // OID_Type event_type
	decode_any(stream, &pointer->event_info); // Any event_info
}

/**
 * Decode ScanReportInfoVar
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportinfovar(ByteStreamReader *stream, ScanReportInfoVar *pointer)
{
	pointer->data_req_id = read_intu16(stream, NULL); // DataReqId data_req_id
	pointer->scan_report_no = read_intu16(stream, NULL); // intu16 scan_report_no
	decode_observationscanlist(stream, &pointer->obs_scan_var); // ObservationScanList obs_scan_var
}

/**
 * Decode ScanReportInfoMPGrouped
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportinfompgrouped(ByteStreamReader *stream,
				    ScanReportInfoMPGrouped *pointer)
{
	pointer->data_req_id = read_intu16(stream, NULL); // intu16 data_req_id
	pointer->scan_report_no = read_intu16(stream, NULL); // intu16 scan_report_no
	decode_scanreportpergroupedlist(stream, &pointer->scan_per_grouped); // ScanReportPerGroupedList scan_per_grouped
}

/**
 * Decode ConfigObject
 *
 * @param *stream
 * @param *pointer
 */
void decode_configobject(ByteStreamReader *stream, ConfigObject *pointer)
{
	pointer->obj_class = read_intu16(stream, NULL); // OID_Type obj_class
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	decode_attributelist(stream, &pointer->attributes); // AttributeList attributes
}

/**
 * Decode ScanReportInfoGroupedList
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportinfogroupedlist(ByteStreamReader *stream,
				      ScanReportInfoGroupedList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count,
					sizeof(ObservationScanGrouped));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_octet_string(stream, pointer->value + i);
		}
	}
}

/**
 * Decode EventReportResultSimple
 *
 * @param *stream
 * @param *pointer
 */
void decode_eventreportresultsimple(ByteStreamReader *stream,
				    EventReportResultSimple *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	pointer->currentTime = read_intu32(stream, NULL); // RelativeTime currentTime
	pointer->event_type = read_intu16(stream, NULL); // OID_Type event_type
	decode_any(stream, &pointer->event_reply_info); // Any event_reply_info
}

/**
 * Decode TYPE
 *
 * @param *stream
 * @param *pointer
 */
void decode_type(ByteStreamReader *stream, TYPE *pointer)
{
	pointer->partition = read_intu16(stream, NULL); // NomPartition partition
	pointer->code = read_intu16(stream, NULL); // OID_Type code
}

/**
 * Decode MetricSpecSmall
 *
 * @param *stream
 * @param *pointer
 */
void decode_metricspecsmall(ByteStreamReader *stream, MetricSpecSmall *pointer)
{
	*pointer = read_intu16(stream, NULL);
}

/**
 * Decode ObservationScanFixed
 *
 * @param *stream
 * @param *pointer
 */
void decode_observationscanfixed(ByteStreamReader *stream,
				 ObservationScanFixed *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	decode_octet_string(stream, &pointer->obs_val_data); // octet_string obs_val_data
}

/**
 * Decode DataResponse
 *
 * @param *stream
 * @param *pointer
 */
void decode_dataresponse(ByteStreamReader *stream, DataResponse *pointer)
{
	pointer->rel_time_stamp = read_intu32(stream, NULL); // RelativeTime rel_time_stamp
	pointer->data_req_result = read_intu16(stream, NULL); // DataReqResult data_req_result
	pointer->event_type = read_intu16(stream, NULL); // OID_Type event_type
	decode_any(stream, &pointer->event_info); // Any event_info
}

/**
 * Decode ProdSpecEntry
 *
 * @param *stream
 * @param *pointer
 */
void decode_prodspecentry(ByteStreamReader *stream, ProdSpecEntry *pointer)
{
	pointer->spec_type = read_intu16(stream, NULL); // ProdSpecEntry_spec_type spec_type
	pointer->component_id = read_intu16(stream, NULL); // PrivateOid component_id
	decode_octet_string(stream, &pointer->prod_spec); // octet_string prod_spec
}

/**
 * Decode ScaleRangeSpec16
 *
 * @param *stream
 * @param *pointer
 */
void decode_scalerangespec16(ByteStreamReader *stream, ScaleRangeSpec16 *pointer)
{
	pointer->lower_absolute_value = read_float(stream); // FLOAT_Type lower_absolute_value
	pointer->upper_absolute_value = read_float(stream); // FLOAT_Type upper_absolute_value
	pointer->lower_scaled_value = read_intu16(stream, NULL); // intu16 lower_scaled_value
	pointer->upper_scaled_value = read_intu16(stream, NULL); // intu16 upper_scaled_value
}

/**
 * Decode SegmEntryElem
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmentryelem(ByteStreamReader *stream, SegmEntryElem *pointer)
{
	pointer->class_id = read_intu16(stream, NULL); // OID_Type class_id
	decode_type(stream, &pointer->metric_type); // TYPE metric_type
	pointer->handle = read_intu16(stream, NULL); // HANDLE handle
	decode_attrvalmap(stream, &pointer->attr_val_map); // AttrValMap attr_val_map
}

/**
 * Decode ABRT_apdu
 *
 * @param *stream
 * @param *pointer
 */
void decode_abrt_apdu(ByteStreamReader *stream, ABRT_apdu *pointer)
{
	pointer->reason = read_intu16(stream, NULL); // Abort_reason reason
}

/**
 * Decode DataReqModeCapab
 *
 * @param *stream
 * @param *pointer
 */
void decode_datareqmodecapab(ByteStreamReader *stream, DataReqModeCapab *pointer)
{
	pointer->data_req_mode_flags = read_intu16(stream, NULL); // DataReqModeFlags data_req_mode_flags
	pointer->data_req_init_agent_count = read_intu8(stream, NULL); // intu8 data_req_init_agent_count
	pointer->data_req_init_manager_count = read_intu8(stream, NULL); // intu8 data_req_init_manager_count
}

/**
 * Decode SupplementalTypeList
 *
 * @param *stream
 * @param *pointer
 */
void decode_supplementaltypelist(ByteStreamReader *stream,
				 SupplementalTypeList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(TYPE));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_type(stream, pointer->value + i);
		}
	}
}

/**
 * Decode ObservationScanFixedList
 *
 * @param *stream
 * @param *pointer
 */
void decode_observationscanfixedlist(ByteStreamReader *stream,
				     ObservationScanFixedList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ObservationScanFixed));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_observationscanfixed(stream, pointer->value + i);
		}
	}
}

/**
 * Decode TrigSegmDataXferRsp
 *
 * @param *stream
 * @param *pointer
 */
void decode_trigsegmdataxferrsp(ByteStreamReader *stream,
				TrigSegmDataXferRsp *pointer)
{
	pointer->seg_inst_no = read_intu16(stream, NULL); // InstNumber seg_inst_no
	pointer->trig_segm_xfer_rsp = read_intu16(stream, NULL); // TrigSegmXferRsp trig_segm_xfer_rsp
}

/**
 * Decode DATA_apdu
 *
 * @param *stream
 * @param *pointer
 */
void decode_data_apdu(ByteStreamReader *stream, DATA_apdu *pointer)
{
	pointer->invoke_id = read_intu16(stream, NULL); // InvokeIDType invoke_id
	decode_data_apdu_message(stream, &pointer->message); // Data_apdu_message message
}

/**
 * Decode AARQ_apdu
 *
 * @param *stream
 * @param *pointer
 */
void decode_aarq_apdu(ByteStreamReader *stream, AARQ_apdu *pointer)
{
	pointer->assoc_version = read_intu32(stream, NULL); // AssociationVersion assoc_version
	decode_dataprotolist(stream, &pointer->data_proto_list); // DataProtoList data_proto_list
}

/**
 * Decode TypeVerList
 *
 * @param *stream
 * @param *pointer
 */
void decode_typeverlist(ByteStreamReader *stream, TypeVerList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(TypeVer));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_typever(stream, pointer->value + i);
		}
	}
}

/**
 * Decode RegCertData
 *
 * @param *stream
 * @param *pointer
 */
void decode_regcertdata(ByteStreamReader *stream, RegCertData *pointer)
{
	decode_authbodyandstructype(stream, &pointer->auth_body_and_struc_type); // AuthBodyAndStrucType auth_body_and_struc_type
	decode_any(stream, &pointer->auth_body_data); // Any auth_body_data
}

/**
 * Decode NuObsValue
 *
 * @param *stream
 * @param *pointer
 */
void decode_nuobsvalue(ByteStreamReader *stream, NuObsValue *pointer)
{
	pointer->metric_id = read_intu16(stream, NULL); // OID_Type metric_id
	pointer->state = read_intu16(stream, NULL); // MeasurementStatus state
	pointer->unit_code = read_intu16(stream, NULL); // OID_Type unit_code
	pointer->value = read_float(stream); // FLOAT_Type value
}

/**
 * Decode ScanREportPerVarList
 *
 * @param *stream
 * @param *pointer
 */
void decode_scanreportpervarlist(ByteStreamReader *stream,
				 ScanReportPerVarList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ScanReportPerVar));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_scanreportpervar(stream, pointer->value + i);
		}
	}
}

/**
 * Decode SetTimeInvoke
 *
 * @param *stream
 * @param *pointer
 */
void decode_settimeinvoke(ByteStreamReader *stream, SetTimeInvoke *pointer)
{
	decode_absolutetime(stream, &pointer->date_time); // AbsoluteTime date_time
	pointer->accuracy = read_float(stream); // FLOAT_Type accuracy
}

/**
 * Decode SegmentInfoList
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmentinfolist(ByteStreamReader *stream, SegmentInfoList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(SegmentInfo));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_segmentinfo(stream, pointer->value + i);
		}
	}
}

/**
 * Decode ActionResultSimple
 *
 * @param *stream
 * @param *pointer
 */
void decode_actionresultsimple(ByteStreamReader *stream, ActionResultSimple *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	pointer->action_type = read_intu16(stream, NULL); // OID_Type action_type
	decode_any(stream, &pointer->action_info_args); // Any action_info_args
}

/**
 * Decode SegmElemStaticAttrEntry
 *
 * @param *stream
 * @param *pointer
 */
void decode_segmelemstaticattrentry(ByteStreamReader *stream,
				    SegmElemStaticAttrEntry *pointer)
{
	pointer->class_id = read_intu16(stream, NULL); // OID_Type class_id
	decode_type(stream, &pointer->metric_type); // TYPE metric_type
	decode_attributelist(stream, &pointer->attribute_list); // AttributeList attribute_list
}

/**
 * Decode BasicNuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 */
void decode_basicnuobsvaluecmp(ByteStreamReader *stream, BasicNuObsValueCmp *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(BasicNuObsValue));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_sfloat(stream);
		}
	}
}

/**
 * Decode ConfigObjectList
 *
 * @param *stream
 * @param *pointer
 */
void decode_configobjectlist(ByteStreamReader *stream, ConfigObjectList *pointer)
{
	pointer->count = read_intu16(stream, NULL); // intu16 count
	pointer->length = read_intu16(stream, NULL); // intu16 length
	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ConfigObject));

		if (pointer->value == NULL) {
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_configobject(stream, pointer->value + i);
		}
	}
}

/**
 * Decode SetResultSimple
 *
 * @param *stream
 * @param *pointer
 */
void decode_setresultsimple(ByteStreamReader *stream, SetResultSimple *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	decode_attributelist(stream, &pointer->attribute_list); // AttributeList attribute_list
}

/**
 * Decode HandleAttrValMapEntry
 *
 * @param *stream
 * @param *pointer
 */
void decode_handleattrvalmapentry(ByteStreamReader *stream,
				  HandleAttrValMapEntry *pointer)
{
	pointer->obj_handle = read_intu16(stream, NULL); // HANDLE obj_handle
	decode_attrvalmap(stream, &pointer->attr_val_map); // AttrValMap attr_val_map
}

/**
 * Decode SimpleNuObsValue
 *
 * @param *stream
 * @param *pointer
 */
void decode_simplenuobsvalue(ByteStreamReader *stream, SimpleNuObsValue *pointer)
{
	*pointer = read_float(stream);
}

/**
 * Decode ConfigId
 *
 * @param *stream
 * @param *pointer
 */
void decode_configid(ByteStreamReader *stream, ConfigId *pointer)
{
	*pointer = read_intu16(stream, NULL);
}

/** @} */
