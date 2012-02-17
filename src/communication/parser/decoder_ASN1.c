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
 * \addtogroup ASN1Codec
 *
 * @{
 */
#include "src/asn1/phd_types.h"
#include "encoder_ASN1.h"
#include "decoder_ASN1.h"
#include "struct_cleaner.h"
#include "src/util/log.h"

#include <stdlib.h>
#include <string.h>

/**
 * \cond Undocumented
 */
#define QUOTE(x) #x

#define CHK(f)			\
	(f);			\
	if (*error)		\
		goto fail;

#define COUNT()			\
	CHK(pointer->count = read_intu16(stream, error));

#define LENGTH()			\
	CHK(pointer->length = read_intu16(stream, error));

#define CLV()				\
	pointer->value = NULL;		\
	COUNT();			\
	LENGTH();

#define CHILDREN_GENERIC(typeU, decodefunction)									\
	if (pointer->count > 0) {								\
		pointer->value = calloc(pointer->count, sizeof(typeU));				\
												\
		if (pointer->value == NULL) {							\
			ERROR("memory full");							\
			goto fail;								\
		}										\
												\
		int i;										\
												\
		for (i = 0; i < pointer->count; i++) {						\
			CHK(decodefunction);							\
		}										\
	}											

#define DECODE_FUNCTION(type) (decode_##type(stream, pointer->value + i, error))
#define PRIM_FUNCTION(f) (*(pointer->value + i) = f(stream, error))

#define CHILDREN(typeU, type) CHILDREN_GENERIC(typeU, DECODE_FUNCTION(type))
#define CHILDREN16(typeU) CHILDREN_GENERIC(typeU, PRIM_FUNCTION(read_intu16))
#define CHILDREN_FLOAT(typeU) CHILDREN_GENERIC(typeU, PRIM_FUNCTION(read_float))
#define CHILDREN_SFLOAT(typeU) CHILDREN_GENERIC(typeU, PRIM_FUNCTION(read_sfloat))

#define EPILOGUE(name) 			\
	return; 			\
fail:					\
	ERROR("err dec " QUOTE(name));	\
	del_##name(pointer);		\
	*error = 1;			\
	return;
/**
 * \endcond
 */

/**
 * Decodes SegmentDataResult.
 *
 * @param stream the SegmentDataResult content decoded as ByteStreamReader.
 * @param pointer the SegmentDataResult to be decoded.
 * @param error Error feedback
 */
void decode_segmentdataresult(ByteStreamReader *stream, SegmentDataResult *pointer, int *error)
{
	CHK(decode_segmdataeventdescr(stream, &pointer->segm_data_event_descr, error));
	// SegmDataEventDescr segm_data_event_descr
	EPILOGUE(segmentdataresult);
}

/**
 * Decodes ScanReportPerVar.
 *
 * @param stream the ScanReportPerVar content decoded as ByteStreamReader.
 * @param pointer the ScanReportPerVar to be decoded.
 * @param error Error feedback
 */
void decode_scanreportpervar(ByteStreamReader *stream, ScanReportPerVar *pointer, int *error)
{
	CHK(pointer->person_id = read_intu16(stream, error));
	// intu16 person_id
	CHK(decode_observationscanlist(stream, &pointer->obs_scan_var, error));
	// ObservationScanList obs_scan_var
	EPILOGUE(scanreportpervar);
}

/**
 * Decodes TypeVer.
 *
 * @param stream the TypeVer content decoded as ByteStreamReader.
 * @param pointer the TypeVer to be decoded.
 * @param error Error feedback
 */
void decode_typever(ByteStreamReader *stream, TypeVer *pointer, int *error)
{
	CHK(pointer->type = read_intu16(stream, error));
	// OID_Type type
	CHK(pointer->version = read_intu16(stream, error));
	// intu16 version
	EPILOGUE(typever);
}

/**
 * Decodes ModificationList.
 *
 * @param stream the ModificationList content decoded as ByteStreamReader.
 * @param pointer the ModificationList to be decoded.
 * @param error Error feedback
 */
void decode_modificationlist(ByteStreamReader *stream, ModificationList *pointer, int *error)
{
	CLV();
	CHILDREN(AttributeModEntry, attributemodentry);
	EPILOGUE(modificationlist);
}

/**
 * Decodes ProductionSpec.
 *
 * @param stream the ProductionSpec content decoded as ByteStreamReader.
 * @param pointer the ProductionSpec to be decoded.
 * @param error Error feedback
 */
void decode_productionspec(ByteStreamReader *stream, ProductionSpec *pointer, int *error)
{
	CLV();
	CHILDREN(ProdSpecEntry, prodspecentry);
	EPILOGUE(productionspec);
}

/**
 * Decodes ActionArgumentSimple.
 *
 * @param stream the ActionArgumentSimple content decoded as ByteStreamReader.
 * @param pointer the ActionArgumentSimple to be decoded.
 * @param error Error feedback
 */
void decode_actionargumentsimple(ByteStreamReader *stream,
				 ActionArgumentSimple *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(pointer->action_type = read_intu16(stream, error));
	// OID_Type action_type
	CHK(decode_any(stream, &pointer->action_info_args, error));
	// Any action_info_args

	EPILOGUE(actionargumentsimple);
}

/**
 * Decodes ScaleRangeSpec32.
 *
 * @param stream the ScaleRangeSpec32 content decoded as ByteStreamReader.
 * @param pointer the ScaleRangeSpec32 to be decoded.
 * @param error Error feedback
 */
void decode_scalerangespec32(ByteStreamReader *stream, ScaleRangeSpec32 *pointer, int *error)
{
	CHK(pointer->lower_absolute_value = read_float(stream, error));
	// FLOAT_Type lower_absolute_value
	CHK(pointer->upper_absolute_value = read_float(stream, error));
	// FLOAT_Type upper_absolute_value
	CHK(pointer->lower_scaled_value = read_intu32(stream, error));
	// intu32 lower_scaled_value
	CHK(pointer->upper_scaled_value = read_intu32(stream, error));
	// intu32 upper_scaled_value

	EPILOGUE(scalerangespec32);
}

/**
 * Decodes AVA_Type.
 *
 * @param stream the AVA_Type content decoded as ByteStreamReader.
 * @param pointer the AVA_Type to be decoded.
 * @param error Error feedback
 */
void decode_ava_type(ByteStreamReader *stream, AVA_Type *pointer, int *error)
{
	CHK(pointer->attribute_id = read_intu16(stream, error));
	// OID_Type attribute_id
	CHK(decode_any(stream, &pointer->attribute_value, error));
	// Any attribute_value

	EPILOGUE(ava_type);
}

/**
 * Decodes ConfigReport.
 *
 * @param stream the ConfigReport content decoded as ByteStreamReader.
 * @param pointer the ConfigReport to be decoded.
 * @param error Error feedback
 */
void decode_configreport(ByteStreamReader *stream, ConfigReport *pointer, int *error)
{
	CHK(pointer->config_report_id = read_intu16(stream, error));
	// ConfigId config_report_id
	// ConfigObjectList config_obj_list
	CHK(decode_configobjectlist(stream, &pointer->config_obj_list, error));

	EPILOGUE(configreport);
}

/**
 * Decodes AttrValMapEntry.
 *
 * @param stream the AttrValMapEntry content decoded as ByteStreamReader.
 * @param pointer the AttrValMapEntry to be decoded.
 * @param error Error feedback
 */
void decode_attrvalmapentry(ByteStreamReader *stream, AttrValMapEntry *pointer, int *error)
{
	CHK(pointer->attribute_id = read_intu16(stream, error));
	// OID_Type attribute_id
	CHK(pointer->attribute_len = read_intu16(stream, error));
	// intu16 attribute_len

	EPILOGUE(attrvalmapentry);
}

/**
 * Decodes AbsoluteTime
 *
 * @param stream the AbsoluteTime content decoded as ByteStreamReader.
 * @param pointer the AbsoluteTime to be decoded.
 * @param error Error feedback
 */
void decode_absolutetime(ByteStreamReader *stream, AbsoluteTime *pointer, int *error)
{
	CHK(pointer->century = read_intu8(stream, error));
	// intu8 century
	CHK(pointer->year = read_intu8(stream, error));
	// intu8 year
	CHK(pointer->month = read_intu8(stream, error));
	// intu8 month
	CHK(pointer->day = read_intu8(stream, error));
	// intu8 day
	CHK(pointer->hour = read_intu8(stream, error));
	// intu8 hour
	CHK(pointer->minute = read_intu8(stream, error));
	// intu8 minute
	CHK(pointer->second = read_intu8(stream, error));
	// intu8 second
	CHK(pointer->sec_fractions = read_intu8(stream, error));
	// intu8 sec_fractions

	EPILOGUE(absolutetime);
}

/**
 * Decodes NuObsValueCmp.
 *
 * @param stream the NuObsValueCmp content decoded as ByteStreamReader.
 * @param pointer the NuObsValueCmp to be decoded.
 * @param error Error feedback
 */
void decode_nuobsvaluecmp(ByteStreamReader *stream, NuObsValueCmp *pointer, int *error)
{
	CLV();
	CHILDREN(NuObsValue, nuobsvalue);
	EPILOGUE(nuobsvaluecmp);
}

/**
 * Decodes ScanReportInfoMPFixed.
 *
 * @param stream the ScanReportInfoMPFixed content decoded as ByteStreamReader.
 * @param pointer the ScanReportInfoMPFixed to be decoded.
 * @param error Error feedback
 */
void decode_scanreportinfompfixed(ByteStreamReader *stream,
				  ScanReportInfoMPFixed *pointer, int *error)
{
	CHK(pointer->data_req_id = read_intu16(stream, error));
	// DataReqId data_req_id
	CHK(pointer->scan_report_no = read_intu16(stream, error));
	// intu16 scan_report_no
	CHK(decode_scanreportperfixedlist(stream, &pointer->scan_per_fixed, error));
	// ScanReportPerFixedList scan_per_fixed

	EPILOGUE(scanreportinfompfixed);
}

/**
 * Decodes RejectResult.
 *
 * @param stream the RejectResult content decoded as ByteStreamReader.
 * @param pointer the RejectResult  to be decoded.
 * @param error Error feedback
 */
void decode_rejectresult(ByteStreamReader *stream, RejectResult *pointer, int *error)
{
	CHK(pointer->problem = read_intu16(stream, error));
	// RorjProblem problem

	EPILOGUE(rejectresult);
}

/**
 * Decodes ManufSpecAssociationInformation.
 *
 * @param stream the ManufSpecAssociationInformation content decoded as ByteStreamReader.
 * @param pointer the ManufSpecAssociationInformation to be decoded.
 * @param error Error feedback
 */
void decode_manufspecassociationinformation(ByteStreamReader *stream,
		ManufSpecAssociationInformation *pointer, int *error)
{
	CHK(decode_uuid_ident(stream, &pointer->data_proto_id_ext, error));
	// UUID_Ident data_proto_id_ext
	CHK(decode_any(stream, &pointer->data_proto_info_ext, error));
	// Any data_proto_info_ext

	EPILOGUE(manufspecassociationinformation);
}

/**
 * Decodes EnumObsValue.
 *
 * @param stream the EnumObsValue content decoded as ByteStreamReader.
 * @param pointer the EnumObsValue to be decoded.
 * @param error Error feedback
 */
void decode_enumobsvalue(ByteStreamReader *stream, EnumObsValue *pointer, int *error)
{
	CHK(pointer->metric_id = read_intu16(stream, error));
	// OID_Type metric_id
	CHK(pointer->state = read_intu16(stream, error));
	// MeasurementStatus state
	CHK(decode_enumval(stream, &pointer->value, error));
	// EnumVal value

	EPILOGUE(enumobsvalue);
}

/**
 * Decodes octet_string. In case of error, does not leak.
 *
 * @param stream the octet_string content decoded as ByteStreamReader.
 * @param pointer the octet_string to be decoded.
 * @param error error feedback. 
 */
void decode_octet_string(ByteStreamReader *stream, octet_string *pointer, int *error)
{
	pointer->value = NULL;
	CHK(pointer->length = read_intu16(stream, error));
	// intu16 length

	if (pointer->length > 0) {
		pointer->value = calloc(pointer->length, sizeof(intu8));

		if (pointer->value == NULL) {
			ERROR("memory full");
			goto fail;
		}

		CHK(read_intu8_many(stream, pointer->value, pointer->length, error));
	}
	EPILOGUE(octet_string);
}

/**
 * Decodes HighResRelativeTime.
 *
 * @param stream the HighResRelativeTime content decoded as ByteStreamReader.
 * @param pointer the HighResRelativeTime to be decoded.
 * @param error Error feedback
 */
void decode_highresrelativetime(ByteStreamReader *stream,
				HighResRelativeTime *pointer, int *error)
{
	// intu8 value[8];
	int i;

	for (i = 0; i < 8; i++) {
		CHK(*(pointer->value + i) = read_intu8(stream, error));
	}
	EPILOGUE(highresrelativetime);
}

/**
 * Decodes SampleType.
 *
 * @param stream the SampleType content decoded as ByteStreamReader.
 * @param pointer the SampleType to be decoded.
 * @param error Error feedback
 */
void decode_sampletype(ByteStreamReader *stream, SampleType *pointer, int *error)
{
	CHK(pointer->sample_size = read_intu8(stream, error));
	// intu8 sample_size
	CHK(pointer->significant_bits = read_intu8(stream, error));
	// intu8 significant_bits

	EPILOGUE(sampletype);
}

/**
 * Decodes AttributeList.
 *
 * @param stream the AttributeList content decoded as ByteStreamReader.
 * @param pointer the AttributeList to be decoded.
 * @param error error feedback
 */
void decode_attributelist(ByteStreamReader *stream, AttributeList *pointer, int *error)
{
	CLV();
	CHILDREN(AVA_Type, ava_type);
	EPILOGUE(attributelist);
}

/**
 * Decode SegmIdList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_segmidlist(ByteStreamReader *stream, SegmIdList *pointer, int *error)
{
	CLV();
	CHILDREN16(InstNumber);
	EPILOGUE(segmidlist);
}

/**
 * Decode SimpleNuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_simplenuobsvaluecmp(ByteStreamReader *stream,
				SimpleNuObsValueCmp *pointer, int *error)
{
	CLV();
	CHILDREN_FLOAT(SimpleNuObsValue);
	EPILOGUE(simplenuobsvaluecmp);
}

/**
 * Decode GetResultSimple
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_getresultsimple(ByteStreamReader *stream, GetResultSimple *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(decode_attributelist(stream, &pointer->attribute_list, error));
	// AttributeList attribute_list

	EPILOGUE(getresultsimple);
}

/**
 * Decode HANDLEList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_handlelist(ByteStreamReader *stream, HANDLEList *pointer, int *error)
{
	CLV();
	CHILDREN16(HANDLE);
	EPILOGUE(handlelist);
}

/**
 * Decode SegmDataEventDescr
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_segmdataeventdescr(ByteStreamReader *stream, SegmDataEventDescr *pointer, int *error)
{
	CHK(pointer->segm_instance = read_intu16(stream, error));
	// InstNumber segm_instance
	CHK(pointer->segm_evt_entry_index = read_intu32(stream, error));
	// intu32 segm_evt_entry_index
	CHK(pointer->segm_evt_entry_count = read_intu32(stream, error));
	// intu32 segm_evt_entry_count
	CHK(pointer->segm_evt_status = read_intu16(stream, error));
	// SegmEvtStatus segm_evt_status

	EPILOGUE(segmdataeventdescr);
}

/**
 * Decode AttrValMap
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_attrvalmap(ByteStreamReader *stream, AttrValMap *pointer, int *error)
{
	CLV();
	CHILDREN(AttrValMapEntry, attrvalmapentry);
	EPILOGUE(attrvalmap);
}

/**
 * Decode ScaleRangeSpec8
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scalerangespec8(ByteStreamReader *stream, ScaleRangeSpec8 *pointer, int *error)
{
	CHK(pointer->lower_absolute_value = read_float(stream, error));
	// FLOAT_Type lower_absolute_value
	CHK(pointer->upper_absolute_value = read_float(stream, error));
	// FLOAT_Type upper_absolute_value
	CHK(pointer->lower_scaled_value = read_intu8(stream, error));
	// intu8 lower_scaled_value
	CHK(pointer->upper_scaled_value = read_intu8(stream, error));
	// intu8 upper_scaled_value

	EPILOGUE(scalerangespec8);
}

/**
 * Decode PhdAssociationInformation
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_phdassociationinformation(ByteStreamReader *stream,
				      PhdAssociationInformation *pointer, int *error)
{
	CHK(pointer->protocolVersion = read_intu32(stream, error));
	// ProtocolVersion protocolVersion
	CHK(pointer->encodingRules = read_intu16(stream, error));
	// EncodingRules encodingRules
	CHK(pointer->nomenclatureVersion = read_intu32(stream, error));
	// NomenclatureVersion nomenclatureVersion
	CHK(pointer->functionalUnits = read_intu32(stream, error));
	// FunctionalUnits functionalUnits
	CHK(pointer->systemType = read_intu32(stream, error));
	// SystemType systemType
	CHK(decode_octet_string(stream, &pointer->system_id, error));
	// octet_string system_id
	CHK(pointer->dev_config_id = read_intu16(stream, error));
	// intu16 dev_config_id
	CHK(decode_datareqmodecapab(stream, &pointer->data_req_mode_capab, error));
	// DataReqModeCapab data_req_mode_capab
	CHK(decode_attributelist(stream, &pointer->optionList, error));
	// AttributeList optionList

	EPILOGUE(phdassociationinformation);
}

/**
 * Decode ScanReportPerFixedList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportperfixedlist(ByteStreamReader *stream,
				   ScanReportPerFixedList *pointer, int *error)
{
	CLV();
	CHILDREN(ScanReportPerFixed, scanreportperfixed);
	EPILOGUE(scanreportperfixedlist);
}

/**
 * Decode ScanReportPerGroupedList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportpergroupedlist(ByteStreamReader *stream,
				     ScanReportPerGroupedList *pointer, int *error)
{
	CLV();
	CHILDREN(ScanReportPerGrouped, scanreportpergrouped);
	EPILOGUE(scanreportpergroupedlist);
}

/**
 * Decode DataProtoList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_dataprotolist(ByteStreamReader *stream, DataProtoList *pointer, int *error)
{
	CLV();
	CHILDREN(DataProto, dataproto);
	EPILOGUE(dataprotolist);
}

/**
 * Decode SegmSelection
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_segmselection(ByteStreamReader *stream, SegmSelection *pointer, int *error)
{
	CHK(pointer->choice = read_intu16(stream, error));
	// SegmSelection_choice choice
	CHK(pointer->length = read_intu16(stream, error));
	// intu16 length

	switch (pointer->choice) {
	case ALL_SEGMENTS_CHOSEN:
		CHK(pointer->u.all_segments = read_intu16(stream, error));
		break;
	case SEGM_ID_LIST_CHOSEN:
		CHK(decode_segmidlist(stream, &pointer->u.segm_id_list, error));
		break;
	case ABS_TIME_RANGE_CHOSEN:
		CHK(decode_abstimerange(stream, &pointer->u.abs_time_range, error));
		break;
	default:
		ERROR("err dec unknown segm selection choice");
		goto fail;
	}

	EPILOGUE(segmselection);
}

/**
 * Decode ErrorResult
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_errorresult(ByteStreamReader *stream, ErrorResult *pointer, int *error)
{
	CHK(pointer->error_value = read_intu16(stream, error));
	// ERROR error_value
	CHK(decode_any(stream, &pointer->parameter, error));
	// Any parameter

	EPILOGUE(errorresult);
}

/**
 * Decode HandleAttrValMap
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_handleattrvalmap(ByteStreamReader *stream, HandleAttrValMap *pointer, int *error)
{
	CLV();
	CHILDREN(HandleAttrValMapEntry, handleattrvalmapentry);
	EPILOGUE(handleattrvalmap);
}

/**
 * Decode AbsoluteTimeAdjust
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_absolutetimeadjust(ByteStreamReader *stream, AbsoluteTimeAdjust *pointer, int *error)
{
	// intu8 value[6]
	int i;

	for (i = 0; i < 6; i++) {
		CHK(*(pointer->value + i) = read_intu8(stream, error));
	}

	EPILOGUE(absolutetimeadjust);
}

/**
 * Decode AARE_apdu
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_aare_apdu(ByteStreamReader *stream, AARE_apdu *pointer, int *error)
{
	CHK(pointer->result = read_intu16(stream, error));
	// Associate_result result
	CHK(decode_dataproto(stream, &pointer->selected_data_proto, error));
	// DataProto selected_data_proto

	EPILOGUE(aare_apdu);
}

/**
 * Decode RLRE_apdu
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_rlre_apdu(ByteStreamReader *stream, RLRE_apdu *pointer, int *error)
{
	CHK(pointer->reason = read_intu16(stream, error));
	// Release_response_reason reason

	EPILOGUE(rlre_apdu);
}

/**
 * Decode MetricIdList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_metricidlist(ByteStreamReader *stream, MetricIdList *pointer, int *error)
{
	CLV();
	CHILDREN16(OID_Type);
	EPILOGUE(metricidlist);
}

/**
 * Decode ScanReportPerFixed
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportperfixed(ByteStreamReader *stream, ScanReportPerFixed *pointer, int *error)
{
	CHK(pointer->person_id = read_intu16(stream, error));
	// intu16 person_id
	CHK(decode_observationscanfixedlist(stream, &pointer->obs_scan_fix, error));
	// ObservationScanFixedList obs_scan_fix

	EPILOGUE(scanreportperfixed);
}

/**
 * Decode ScanReportInfoGrouped
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportinfogrouped(ByteStreamReader *stream,
				  ScanReportInfoGrouped *pointer, int *error)
{
	CHK(pointer->data_req_id = read_intu16(stream, error));
	// intu16 data_req_id
	CHK(pointer->scan_report_no = read_intu16(stream, error));
	// intu16 scan_report_no
	CHK(decode_scanreportinfogroupedlist(stream, &pointer->obs_scan_grouped, error));
	// ScanReportInfoGroupedList obs_scan_grouped

	EPILOGUE(scanreportinfogrouped);
}

/**
 * Decode ObservationScan
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_observationscan(ByteStreamReader *stream, ObservationScan *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(decode_attributelist(stream, &pointer->attributes, error));
	// AttributeList attributes

	EPILOGUE(observationscan);
}

/**
 * Decode ScanReportPerGrouped
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportpergrouped(ByteStreamReader *stream,
				 ScanReportPerGrouped *pointer, int *error)
{
	CHK(pointer->person_id = read_intu16(stream, error));
	// PersonId person_id
	CHK(decode_octet_string(stream, &pointer->obs_scan_grouped, error));

	EPILOGUE(scanreportpergrouped);
}

/**
 * Decode SystemModel
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_systemmodel(ByteStreamReader *stream, SystemModel *pointer, int *error)
{
	CHK(decode_octet_string(stream, &pointer->manufacturer, error));
	// octet_string manufacturer
	CHK(decode_octet_string(stream, &pointer->model_number, error));
	// octet_string model_number

	EPILOGUE(systemmodel);
}

/**
 * Decode ObservationScanList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_observationscanlist(ByteStreamReader *stream,
				ObservationScanList *pointer, int *error)
{
	CLV();
	CHILDREN(ObservationScan, observationscan);
	EPILOGUE(observationscanlist);
}

/**
 * Decode APDU
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_apdu(ByteStreamReader *stream, APDU *pointer, int *error)
{
	CHK(pointer->choice = read_intu16(stream, error));
	// APDU_choice choice
	CHK(pointer->length = read_intu16(stream, error));
	// intu16 length

	switch (pointer->choice) {
	case AARQ_CHOSEN:
		CHK(decode_aarq_apdu(stream, &pointer->u.aarq, error));
		break;
	case AARE_CHOSEN:
		CHK(decode_aare_apdu(stream, &pointer->u.aare, error));
		break;
	case RLRQ_CHOSEN:
		CHK(decode_rlrq_apdu(stream, &pointer->u.rlrq, error));
		break;
	case RLRE_CHOSEN:
		CHK(decode_rlre_apdu(stream, &pointer->u.rlre, error));
		break;
	case ABRT_CHOSEN:
		CHK(decode_abrt_apdu(stream, &pointer->u.abrt, error));
		break;
	case PRST_CHOSEN:
		CHK(decode_prst_apdu(stream, &pointer->u.prst, error));
		break;
	default:
		ERROR("unknown apdu choice");
		goto fail;
	}
	EPILOGUE(apdu);
}

/**
 * Decode PRST_apdu
 *
 * @param *stream
 * @param *pointer
 * @param *error
 */
void decode_prst_apdu(ByteStreamReader *stream, PRST_apdu *pointer, int *error)
{
	pointer->value = NULL;
	CHK(pointer->length = read_intu16(stream, error));
	// intu16 length

	if (pointer->length > 0) {
		DATA_apdu *data = calloc(1, sizeof(DATA_apdu));

		if (data == NULL) {
			ERROR("memory full");
			goto fail;
		} else {
			CHK(decode_data_apdu(stream, data, error));
			encode_set_data_apdu(pointer, data);
		}
	}
	EPILOGUE(prst_apdu);
}

/**
 * Decode PmSegmentEntryMap
 *
 * @param stream
 * @param pointer
 * @param error
 */
void decode_pmsegmententrymap(ByteStreamReader *stream, PmSegmentEntryMap *pointer, int *error)
{
	CHK(pointer->segm_entry_header = read_intu16(stream, error));
	// SegmEntryHeader segm_entry_header
	CHK(decode_segmentryelemlist(stream, &pointer->segm_entry_elem_list, error));
	// SegmEntryElemList segm_entry_elem_list

	EPILOGUE(pmsegmententrymap);
}

/**
 * Decode Any
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_any(ByteStreamReader *stream, Any *pointer, int *error)
{
	pointer->value = NULL;
	CHK(pointer->length = read_intu16(stream, error));
	// intu16 length

	if (pointer->length > stream->unread_bytes) {
		ERROR("any struct: underflow");
		goto fail;
	}

	if (pointer->length > 0) {
		pointer->value = calloc(pointer->length, sizeof(intu8));

		if (pointer->value == NULL) {
			ERROR("memory full");
			goto fail;
		}

		CHK(read_intu8_many(stream, pointer->value, pointer->length, error));
	}
	EPILOGUE(any);
}

/**
 * Decode SetArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_setargumentsimple(ByteStreamReader *stream, SetArgumentSimple *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(decode_modificationlist(stream, &pointer->modification_list, error));
	// ModificationList modification_list

	EPILOGUE(setargumentsimple);
}

/**
 * Decode SegmentInfo
 *
 * @param *stream
 * @param *pointer
 * @param *error
 */
void decode_segmentinfo(ByteStreamReader *stream, SegmentInfo *pointer, int *error)
{
	CHK(pointer->seg_inst_no = read_intu16(stream, error));
	// InstNumber seg_inst_no
	CHK(decode_attributelist(stream, &pointer->seg_info, error));
	// AttributeList seg_info
	EPILOGUE(segmentinfo);
}

/**
 * Decode PmSegmElemStaticAttrList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_pmsegmelemstaticattrlist(ByteStreamReader *stream,
				     PmSegmElemStaticAttrList *pointer, int *error)
{
	CLV();
	CHILDREN(SegmElemStaticAttrEntry, segmelemstaticattrentry);
	EPILOGUE(pmsegmelemstaticattrlist);
}

/**
 * Decode AbsTimeRange
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_abstimerange(ByteStreamReader *stream, AbsTimeRange *pointer, int *error)
{
	CHK(decode_absolutetime(stream, &pointer->from_time, error));
	// AbsoluteTime from_time
	CHK(decode_absolutetime(stream, &pointer->to_time, error));
	// AbsoluteTime to_time

	EPILOGUE(abstimerange);
}

/**
 * Decode ScanReportInfoMPVar
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportinfompvar(ByteStreamReader *stream,
				ScanReportInfoMPVar *pointer, int *error)
{
	CHK(pointer->data_req_id = read_intu16(stream, error));
	// DataReqId data_req_id
	CHK(pointer->scan_report_no = read_intu16(stream, error));
	// intu16 scan_report_no
	CHK(decode_scanreportpervarlist(stream, &pointer->scan_per_var, error));
	// ScanReportPerVarList scan_per_var
	EPILOGUE(scanreportinfompvar);
}

/**
 * Decode UUID_Ident
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_uuid_ident(ByteStreamReader *stream, UUID_Ident *pointer, int *error)
{
	// intu8 value[16]
	int i;

	for (i = 0; i < 16; i++) {
		CHK(*(pointer->value + i) = read_intu8(stream, error));
	}
	EPILOGUE(uuid_ident);
}

/**
 * Decode GetArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_getargumentsimple(ByteStreamReader *stream, GetArgumentSimple *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(decode_attributeidlist(stream, &pointer->attribute_id_list, error));
	// AttributeIdList attribute_id_list

	EPILOGUE(getargumentsimple);
}

/**
 * Decode RegCertDataList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_regcertdatalist(ByteStreamReader *stream, RegCertDataList *pointer, int *error)
{
	CLV();
	CHILDREN(RegCertData, regcertdata);
	EPILOGUE(regcertdatalist);
}

/**
 * Decode ConfirReportRsp
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_configreportrsp(ByteStreamReader *stream, ConfigReportRsp *pointer, int *error)
{
	CHK(pointer->config_report_id = read_intu16(stream, error));
	// ConfigId config_report_id
	CHK(pointer->config_result = read_intu16(stream, error));
	// ConfigResult config_result

	EPILOGUE(configreportrsp);
}

/**
 * Decode DataProto
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_dataproto(ByteStreamReader *stream, DataProto *pointer, int *error)
{
	CHK(pointer->data_proto_id = read_intu16(stream, error));
	// DataProtoId data_proto_id
	CHK(decode_any(stream, &pointer->data_proto_info, error));
	// Any data_proto_info

	EPILOGUE(dataproto);
}

/**
 * Decode MetricStructureSmall
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_metricstructuresmall(ByteStreamReader *stream,
				 MetricStructureSmall *pointer, int *error)
{
	CHK(pointer->ms_struct = read_intu8(stream, error));
	// intu8 ms_struct
	CHK(pointer->ms_comp_no = read_intu8(stream, error));
	// intu8 ms_comp_no

	EPILOGUE(metricstructuresmall);
}

/**
 * Decode SegmentStatisticEntry
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_segmentstatisticentry(ByteStreamReader *stream,
				  SegmentStatisticEntry *pointer, int *error)
{
	CHK(pointer->segm_stat_type = read_intu16(stream, error));
	// SegmStatType segm_stat_type
	CHK(decode_octet_string(stream, &pointer->segm_stat_entry, error));
	// octet_string segm_stat_entry

	EPILOGUE(segmentstatisticentry);
}

/**
 * Decode SegmentDataEvent
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_segmentdataevent(ByteStreamReader *stream, SegmentDataEvent *pointer, int *error)
{
	CHK(decode_segmdataeventdescr(stream, &pointer->segm_data_event_descr, error));
	// SegmDataEventDescr segm_data_event_descr
	CHK(decode_octet_string(stream, &pointer->segm_data_event_entries, error));
	// octet_string segm_data_event_entries

	EPILOGUE(segmentdataevent);
}

/**
 * Decode SegmEntryElemList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_segmentryelemlist(ByteStreamReader *stream, SegmEntryElemList *pointer, int *error)
{
	CLV();
	CHILDREN(SegmEntryElem, segmentryelem);
	EPILOGUE(segmentryelemlist);
}

/**
 * Decode SaSpec
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_saspec(ByteStreamReader *stream, SaSpec *pointer, int *error)
{
	CHK(pointer->array_size = read_intu16(stream, error));
	// intu16 array_size
	CHK(decode_sampletype(stream, &pointer->sample_type, error));
	// SampleType sample_type
	CHK(pointer->flags = read_intu16(stream, error));
	// SaFlags flags

	EPILOGUE(saspec);
}

/**
 * Decode AttributeModEntry
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_attributemodentry(ByteStreamReader *stream, AttributeModEntry *pointer, int *error)
{
	CHK(pointer->modify_operator = read_intu16(stream, error));
	// ModifyOperator modify_operator
	CHK(decode_ava_type(stream, &pointer->attribute, error));
	// AVA_Type attribute

	EPILOGUE(attributemodentry);
}

/**
 * Decode MdsTimeInfo
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_mdstimeinfo(ByteStreamReader *stream, MdsTimeInfo *pointer, int *error)
{
	CHK(pointer->mds_time_cap_state = read_intu16(stream, error));
	// MdsTimeCapState mds_time_cap_state
	CHK(pointer->time_sync_protocol = read_intu16(stream, error));
	// TimeProtocolId time_sync_protocol
	CHK(pointer->time_sync_accuracy = read_intu32(stream, error));
	// RelativeTime time_sync_accuracy
	CHK(pointer->time_resolution_abs_time = read_intu16(stream, error));
	// intu16 time_resolution_abs_time
	CHK(pointer->time_resolution_rel_time = read_intu16(stream, error));
	// intu16 time_resolution_rel_time
	CHK(pointer->time_resolution_high_res_time = read_intu32(stream, error));
	// intu32 time_resolution_high_res_time

	EPILOGUE(mdstimeinfo);
}

/**
 * Decode EnumVal
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_enumval(ByteStreamReader *stream, EnumVal *pointer, int *error)
{
	CHK(pointer->choice = read_intu16(stream, error));
	// EnumVal_choice choice
	CHK(pointer->length = read_intu16(stream, error));
	// intu16 length

	switch (pointer->choice) {
	case OBJ_ID_CHOSEN:
		CHK(pointer->u.enum_obj_id = read_intu16(stream, error));
		break;
	case TEXT_STRING_CHOSEN:
		CHK(decode_octet_string(stream, &pointer->u.enum_text_string, error));
		break;
	case BIT_STR_CHOSEN:
		CHK(pointer->u.enum_bit_str = read_intu32(stream, error));
		break;
	default:
		ERROR("unknown enumval choice");
		goto fail;
		break;
	}
	EPILOGUE(enumval);
}

/**
 * Decode TrigSegmDataXferReq
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_trigsegmdataxferreq(ByteStreamReader *stream,
				TrigSegmDataXferReq *pointer, int *error)
{
	CHK(pointer->seg_inst_no = read_intu16(stream, error));
	// InstNumber seg_inst_no

	EPILOGUE(trigsegmdataxferreq);
}

/**
 * Decode BatMeasure
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_batmeasure(ByteStreamReader *stream, BatMeasure *pointer, int *error)
{
	CHK(pointer->value = read_float(stream, error));
	// FLOAT_Type value
	CHK(pointer->unit = read_intu16(stream, error));
	// OID_Type unit

	EPILOGUE(batmeasure);
}

/**
 * Decode SegmentStatistics
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_segmentstatistics(ByteStreamReader *stream, SegmentStatistics *pointer, int *error)
{
	CLV();
	CHILDREN(SegmentStatisticEntry, segmentstatisticentry);
	EPILOGUE(segmentstatistics);
}

/**
 * Decode AttributeIdList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_attributeidlist(ByteStreamReader *stream, AttributeIdList *pointer, int *error)
{
	CLV();
	CHILDREN16(OID_Type);
	EPILOGUE(attributeidlist);
}

/**
 * Decode ScanReportInfoFixed
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportinfofixed(ByteStreamReader *stream,
				ScanReportInfoFixed *pointer, int *error)
{
	CHK(pointer->data_req_id = read_intu16(stream, error));
	// DataReqId data_req_id
	CHK(pointer->scan_report_no = read_intu16(stream, error));
	// intu16 scan_report_no
	CHK(decode_observationscanfixedlist(stream, &pointer->obs_scan_fixed, error));
	// ObservationScanFixedList obs_scan_fixed

	EPILOGUE(scanreportinfofixed);
}

/**
 * Decode DataRequest
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_datarequest(ByteStreamReader *stream, DataRequest *pointer, int *error)
{
	CHK(pointer->data_req_id = read_intu16(stream, error));
	// DataReqId data_req_id
	CHK(pointer->data_req_mode = read_intu16(stream, error));
	// DataReqMode data_req_mode
	CHK(pointer->data_req_time = read_intu32(stream, error));
	// RelativeTime data_req_time
	CHK(pointer->data_req_person_id = read_intu16(stream, error));
	// intu16 DataRequest_data_req_person_id
	CHK(pointer->data_req_class = read_intu16(stream, error));
	// OID_Type data_req_class
	CHK(decode_handlelist(stream, &pointer->data_req_obj_handle_list, error));
	// HANDLEList data_req_obj_handle_list

	EPILOGUE(datarequest);
}

/**
 * Decode AuthBodyAndStrucType
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_authbodyandstructype(ByteStreamReader *stream,
				 AuthBodyAndStrucType *pointer, int *error)
{
	CHK(pointer->auth_body = read_intu8(stream, error));
	// AuthBody auth_body
	CHK(pointer->auth_body_struc_type = read_intu8(stream, error));
	// AuthBodyStrucType auth_body_struc_type

	EPILOGUE(authbodyandstructype);
}

/**
 * Decode RLRQ_apdu
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_rlrq_apdu(ByteStreamReader *stream, RLRQ_apdu *pointer, int *error)
{
	CHK(pointer->reason = read_intu16(stream, error));
	// Release_request_reason reason

	EPILOGUE(rlrq_apdu);
}

/**
 * Decode Data_apdu_message
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_data_apdu_message(ByteStreamReader *stream, Data_apdu_message *pointer, int *error)
{
	CHK(pointer->choice = read_intu16(stream, error));
	// DATA_apdu_choice choice
	CHK(pointer->length = read_intu16(stream, error));
	// intu16 length

	switch (pointer->choice) {
	case ROIV_CMIP_EVENT_REPORT_CHOSEN:
		CHK(decode_eventreportargumentsimple(stream,
				 &pointer->u.roiv_cmipEventReport, error));
		break;
	case ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		CHK(decode_eventreportargumentsimple(stream,
				 &pointer->u.roiv_cmipConfirmedEventReport, error));
		break;
	case ROIV_CMIP_GET_CHOSEN:
		CHK(decode_getargumentsimple(stream, &pointer->u.roiv_cmipGet, error));
		break;
	case ROIV_CMIP_SET_CHOSEN:
		CHK(decode_setargumentsimple(stream, &pointer->u.roiv_cmipSet, error));
		break;
	case ROIV_CMIP_CONFIRMED_SET_CHOSEN:
		CHK(decode_setargumentsimple(stream,
				 &pointer->u.roiv_cmipConfirmedSet, error));
		break;
	case ROIV_CMIP_ACTION_CHOSEN:
		CHK(decode_actionargumentsimple(stream, &pointer->u.roiv_cmipAction, error));
		break;
	case ROIV_CMIP_CONFIRMED_ACTION_CHOSEN:
		CHK(decode_actionargumentsimple(stream,
				    &pointer->u.roiv_cmipConfirmedAction, error));
		break;
	case RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		CHK(decode_eventreportresultsimple(stream,
				       &pointer->u.rors_cmipConfirmedEventReport, error));
		break;
	case RORS_CMIP_GET_CHOSEN:
		CHK(decode_getresultsimple(stream, &pointer->u.rors_cmipGet, error));
		break;
	case RORS_CMIP_CONFIRMED_SET_CHOSEN:
		CHK(decode_setresultsimple(stream,
			       &pointer->u.rors_cmipConfirmedSet, error));
		break;
	case RORS_CMIP_CONFIRMED_ACTION_CHOSEN:
		CHK(decode_actionresultsimple(stream,
				  &pointer->u.rors_cmipConfirmedAction, error));
		break;
	case ROER_CHOSEN:
		CHK(decode_errorresult(stream, &pointer->u.roer, error));
		break;
	case RORJ_CHOSEN:
		CHK(decode_rejectresult(stream, &pointer->u.rorj, error));
		break;
	default:
		ERROR("unknown data apdu choice");
		goto fail;
		break;
	}
	EPILOGUE(data_apdu_message);
}

/**
 * Decode EventReportArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_eventreportargumentsimple(ByteStreamReader *stream,
				      EventReportArgumentSimple *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(pointer->event_time = read_intu32(stream, error));
	// RelativeTime event_time
	CHK(pointer->event_type = read_intu16(stream, error));
	// OID_Type event_type
	CHK(decode_any(stream, &pointer->event_info, error));
	// Any event_info

	EPILOGUE(eventreportargumentsimple);
}

/**
 * Decode ScanReportInfoVar
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportinfovar(ByteStreamReader *stream, ScanReportInfoVar *pointer, int *error)
{
	CHK(pointer->data_req_id = read_intu16(stream, error));
	// DataReqId data_req_id
	CHK(pointer->scan_report_no = read_intu16(stream, error));
	// intu16 scan_report_no
	CHK(decode_observationscanlist(stream, &pointer->obs_scan_var, error));
	// ObservationScanList obs_scan_var

	EPILOGUE(scanreportinfovar);
}

/**
 * Decode ScanReportInfoMPGrouped
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportinfompgrouped(ByteStreamReader *stream,
				    ScanReportInfoMPGrouped *pointer, int *error)
{
	CHK(pointer->data_req_id = read_intu16(stream, error));
	// intu16 data_req_id
	CHK(pointer->scan_report_no = read_intu16(stream, error));
	// intu16 scan_report_no
	CHK(decode_scanreportpergroupedlist(stream, &pointer->scan_per_grouped, error));
	// ScanReportPerGroupedList scan_per_grouped

	EPILOGUE(scanreportinfompgrouped);
}

/**
 * Decode ConfigObject
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_configobject(ByteStreamReader *stream, ConfigObject *pointer, int *error)
{
	CHK(pointer->obj_class = read_intu16(stream, error));
	// OID_Type obj_class
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(decode_attributelist(stream, &pointer->attributes, error));
	// AttributeList attributes
	EPILOGUE(configobject);
}

/**
 * Decode ScanReportInfoGroupedList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportinfogroupedlist(ByteStreamReader *stream,
				      ScanReportInfoGroupedList *pointer, int *error)
{
	CLV();
	CHILDREN(ObservationScanGrouped, octet_string);
	EPILOGUE(scanreportinfogroupedlist);
}

/**
 * Decode EventReportResultSimple
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_eventreportresultsimple(ByteStreamReader *stream,
				    EventReportResultSimple *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(pointer->currentTime = read_intu32(stream, error));
	// RelativeTime currentTime
	CHK(pointer->event_type = read_intu16(stream, error));
	// OID_Type event_type
	CHK(decode_any(stream, &pointer->event_reply_info, error));
	// Any event_reply_info

	EPILOGUE(eventreportresultsimple);
}

/**
 * Decode TYPE
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_type(ByteStreamReader *stream, TYPE *pointer, int *error)
{
	CHK(pointer->partition = read_intu16(stream, error));
	// NomPartition partition
	CHK(pointer->code = read_intu16(stream, error));
	// OID_Type code

	EPILOGUE(type);
}

/**
 * Decode MetricSpecSmall
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_metricspecsmall(ByteStreamReader *stream, MetricSpecSmall *pointer, int *error)
{
	CHK(*pointer = read_intu16(stream, error));

	EPILOGUE(metricspecsmall);
}

/**
 * Decode ObservationScanFixed
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_observationscanfixed(ByteStreamReader *stream,
				 ObservationScanFixed *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(decode_octet_string(stream, &pointer->obs_val_data, error));
	// octet_string obs_val_data

	EPILOGUE(observationscanfixed);
}

/**
 * Decode DataResponse
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_dataresponse(ByteStreamReader *stream, DataResponse *pointer, int *error)
{
	CHK(pointer->rel_time_stamp = read_intu32(stream, error));
	// RelativeTime rel_time_stamp
	CHK(pointer->data_req_result = read_intu16(stream, error));
	// DataReqResult data_req_result
	CHK(pointer->event_type = read_intu16(stream, error));
	// OID_Type event_type
	CHK(decode_any(stream, &pointer->event_info, error));
	// Any event_info

	EPILOGUE(dataresponse);
}

/**
 * Decode ProdSpecEntry
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_prodspecentry(ByteStreamReader *stream, ProdSpecEntry *pointer, int *error)
{
	CHK(pointer->spec_type = read_intu16(stream, error));
	// ProdSpecEntry_spec_type spec_type
	CHK(pointer->component_id = read_intu16(stream, error));
	// PrivateOid component_id
	CHK(decode_octet_string(stream, &pointer->prod_spec, error));
	// octet_string prod_spec

	EPILOGUE(prodspecentry);
}

/**
 * Decode ScaleRangeSpec16
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scalerangespec16(ByteStreamReader *stream, ScaleRangeSpec16 *pointer, int *error)
{
	CHK(pointer->lower_absolute_value = read_float(stream, error));
	// FLOAT_Type lower_absolute_value
	CHK(pointer->upper_absolute_value = read_float(stream, error));
	// FLOAT_Type upper_absolute_value
	CHK(pointer->lower_scaled_value = read_intu16(stream, error));
	// intu16 lower_scaled_value
	CHK(pointer->upper_scaled_value = read_intu16(stream, error));
	// intu16 upper_scaled_value

	EPILOGUE(scalerangespec16);
}

/**
 * Decode SegmEntryElem
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_segmentryelem(ByteStreamReader *stream, SegmEntryElem *pointer, int *error)
{
	CHK(pointer->class_id = read_intu16(stream, error));
	// OID_Type class_id
	CHK(decode_type(stream, &pointer->metric_type, error));
	// TYPE metric_type
	CHK(pointer->handle = read_intu16(stream, error));
	// HANDLE handle
	CHK(decode_attrvalmap(stream, &pointer->attr_val_map, error));
	// AttrValMap attr_val_map

	EPILOGUE(segmentryelem);
}

/**
 * Decode ABRT_apdu
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_abrt_apdu(ByteStreamReader *stream, ABRT_apdu *pointer, int *error)
{
	CHK(pointer->reason = read_intu16(stream, error));
	// Abort_reason reason

	EPILOGUE(abrt_apdu);
}

/**
 * Decode DataReqModeCapab
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_datareqmodecapab(ByteStreamReader *stream, DataReqModeCapab *pointer, int *error)
{
	CHK(pointer->data_req_mode_flags = read_intu16(stream, error));
	// DataReqModeFlags data_req_mode_flags
	CHK(pointer->data_req_init_agent_count = read_intu8(stream, error));
	// intu8 data_req_init_agent_count
	CHK(pointer->data_req_init_manager_count = read_intu8(stream, error));
	// intu8 data_req_init_manager_count

	EPILOGUE(datareqmodecapab);
}

/**
 * Decode SupplementalTypeList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_supplementaltypelist(ByteStreamReader *stream,
				 SupplementalTypeList *pointer, int *error)
{
	CLV();
	CHILDREN(TYPE, type);
	EPILOGUE(supplementaltypelist);
}

/**
 * Decode ObservationScanFixedList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_observationscanfixedlist(ByteStreamReader *stream,
				     ObservationScanFixedList *pointer, int *error)
{
	CLV();
	CHILDREN(ObservationScanFixed, observationscanfixed);
	EPILOGUE(observationscanfixedlist);
}

/**
 * Decode TrigSegmDataXferRsp
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_trigsegmdataxferrsp(ByteStreamReader *stream,
				TrigSegmDataXferRsp *pointer, int *error)
{
	CHK(pointer->seg_inst_no = read_intu16(stream, error));
	// InstNumber seg_inst_no
	CHK(pointer->trig_segm_xfer_rsp = read_intu16(stream, error));
	// TrigSegmXferRsp trig_segm_xfer_rsp

	EPILOGUE(trigsegmdataxferrsp);
}

/**
 * Decode DATA_apdu
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_data_apdu(ByteStreamReader *stream, DATA_apdu *pointer, int *error)
{
	CHK(pointer->invoke_id = read_intu16(stream, error));
	// InvokeIDType invoke_id
	CHK(decode_data_apdu_message(stream, &pointer->message, error));
	// Data_apdu_message message

	EPILOGUE(data_apdu);
}

/**
 * Decode AARQ_apdu
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_aarq_apdu(ByteStreamReader *stream, AARQ_apdu *pointer, int *error)
{
	CHK(pointer->assoc_version = read_intu32(stream, error));
	// AssociationVersion assoc_version
	CHK(decode_dataprotolist(stream, &pointer->data_proto_list, error));
	// DataProtoList data_proto_list

	EPILOGUE(aarq_apdu);
}

/**
 * Decode TypeVerList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_typeverlist(ByteStreamReader *stream, TypeVerList *pointer, int *error)
{
	CLV();
	CHILDREN(TypeVer, typever);
	EPILOGUE(typeverlist);
}

/**
 * Decode RegCertData
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_regcertdata(ByteStreamReader *stream, RegCertData *pointer, int *error)
{
	CHK(decode_authbodyandstructype(stream, &pointer->auth_body_and_struc_type, error));
	// AuthBodyAndStrucType auth_body_and_struc_type
	CHK(decode_any(stream, &pointer->auth_body_data, error));
	// Any auth_body_data

	EPILOGUE(regcertdata);
}

/**
 * Decode NuObsValue
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_nuobsvalue(ByteStreamReader *stream, NuObsValue *pointer, int *error)
{
	CHK(pointer->metric_id = read_intu16(stream, error));
	// OID_Type metric_id
	CHK(pointer->state = read_intu16(stream, error));
	// MeasurementStatus state
	CHK(pointer->unit_code = read_intu16(stream, error));
	// OID_Type unit_code
	CHK(pointer->value = read_float(stream, error));
	// FLOAT_Type value

	EPILOGUE(nuobsvalue);
}

/**
 * Decode ScanREportPerVarList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_scanreportpervarlist(ByteStreamReader *stream,
				 ScanReportPerVarList *pointer, int *error)
{
	CLV();
	CHILDREN(ScanReportPerVar, scanreportpervar);
	EPILOGUE(scanreportpervarlist);
}

/**
 * Decode SetTimeInvoke
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_settimeinvoke(ByteStreamReader *stream, SetTimeInvoke *pointer, int *error)
{
	CHK(decode_absolutetime(stream, &pointer->date_time, error));
	// AbsoluteTime date_time
	CHK(pointer->accuracy = read_float(stream, error));
	// FLOAT_Type accuracy

	EPILOGUE(settimeinvoke);
}

/**
 * Decode SegmentInfoList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_segmentinfolist(ByteStreamReader *stream, SegmentInfoList *pointer, int *error)
{
	CLV();
	CHILDREN(SegmentInfo, segmentinfo);
	EPILOGUE(segmentinfolist);
}

/**
 * Decode ActionResultSimple
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_actionresultsimple(ByteStreamReader *stream, ActionResultSimple *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(pointer->action_type = read_intu16(stream, error));
	// OID_Type action_type
	CHK(decode_any(stream, &pointer->action_info_args, error));
	// Any action_info_args

	EPILOGUE(actionresultsimple);
}

/**
 * Decode SegmElemStaticAttrEntry
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_segmelemstaticattrentry(ByteStreamReader *stream,
				    SegmElemStaticAttrEntry *pointer, int *error)
{
	CHK(pointer->class_id = read_intu16(stream, error));
	// OID_Type class_id
	CHK(decode_type(stream, &pointer->metric_type, error));
	// TYPE metric_type
	CHK(decode_attributelist(stream, &pointer->attribute_list, error));
	// AttributeList attribute_list

	EPILOGUE(segmelemstaticattrentry);
}

/**
 * Decode BasicNuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_basicnuobsvaluecmp(ByteStreamReader *stream, BasicNuObsValueCmp *pointer, int *error)
{
	CLV();
	CHILDREN_SFLOAT(BasicNuObsValue);
	EPILOGUE(basicnuobsvaluecmp);
}

/**
 * Decode ConfigObjectList
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_configobjectlist(ByteStreamReader *stream, ConfigObjectList *pointer, int *error)
{
	CLV();
	CHILDREN(ConfigObject, configobject);
	EPILOGUE(configobjectlist);
}

/**
 * Decode SetResultSimple
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_setresultsimple(ByteStreamReader *stream, SetResultSimple *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(decode_attributelist(stream, &pointer->attribute_list, error));
	// AttributeList attribute_list

	EPILOGUE(setresultsimple);
}

/**
 * Decode HandleAttrValMapEntry
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_handleattrvalmapentry(ByteStreamReader *stream,
				  HandleAttrValMapEntry *pointer, int *error)
{
	CHK(pointer->obj_handle = read_intu16(stream, error));
	// HANDLE obj_handle
	CHK(decode_attrvalmap(stream, &pointer->attr_val_map, error));
	// AttrValMap attr_val_map

	EPILOGUE(handleattrvalmapentry);
}

/**
 * Decode SimpleNuObsValue
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_simplenuobsvalue(ByteStreamReader *stream, SimpleNuObsValue *pointer, int *error)
{
	CHK(*pointer = read_float(stream, error));

	EPILOGUE(simplenuobsvalue);
}

/**
 * Decode ConfigId
 *
 * @param *stream
 * @param *pointer
 * @param error Error feedback
 */
void decode_configid(ByteStreamReader *stream, ConfigId *pointer, int *error)
{
	CHK(*pointer = read_intu16(stream, error));

	EPILOGUE(configid);
}

/** @} */
