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
#include "src/util/log.h"

#include <stdlib.h>
#include <string.h>

// TODO improve error handling (out of loops)
// TODO empty structs upon errors

/**
 * Decodes SegmentDataResult.
 *
 * @param stream the SegmentDataResult content decoded as ByteStreamReader.
 * @param pointer the SegmentDataResult to be decoded.
 * @param error Error feedback
 */
void decode_segmentdataresult(ByteStreamReader *stream, SegmentDataResult *pointer, int *error)
{
	decode_segmdataeventdescr(stream, &pointer->segm_data_event_descr, error); // SegmDataEventDescr segm_data_event_descr
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
	pointer->person_id = read_intu16(stream, error); // intu16 person_id
	decode_observationscanlist(stream, &pointer->obs_scan_var, error); // ObservationScanList obs_scan_var
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
	pointer->type = read_intu16(stream, error); // OID_Type type
	pointer->version = read_intu16(stream, error); // intu16 version
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		int i;

		pointer->value = calloc(pointer->count, sizeof(AttributeModEntry));

		for (i = 0; i < pointer->count; i++) {
			decode_attributemodentry(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec notificationlist underflow");
				break;
			}
		}
	}
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ProdSpecEntry));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_prodspecentry(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec productionspec underflow");
				break;
			}
		}
	}
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
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	pointer->action_type = read_intu16(stream, error); // OID_Type action_type
	decode_any(stream, &pointer->action_info_args, error); // Any action_info_args
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
	pointer->lower_absolute_value = read_float(stream, error); // FLOAT_Type lower_absolute_value
	pointer->upper_absolute_value = read_float(stream, error); // FLOAT_Type upper_absolute_value
	pointer->lower_scaled_value = read_intu32(stream, error); // intu32 lower_scaled_value
	pointer->upper_scaled_value = read_intu32(stream, error); // intu32 upper_scaled_value
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
	pointer->attribute_id = read_intu16(stream, error); // OID_Type attribute_id
	if (*error) {
		DEBUG("ava type: id error");
		return;
	}
	decode_any(stream, &pointer->attribute_value, error); // Any attribute_value
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
	pointer->config_report_id = read_intu16(stream, error); // ConfigId config_report_id
	// ConfigObjectList config_obj_list
	decode_configobjectlist(stream, &pointer->config_obj_list, error);
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
	pointer->attribute_id = read_intu16(stream, error); // OID_Type attribute_id
	pointer->attribute_len = read_intu16(stream, error); // intu16 attribute_len
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
	pointer->century = read_intu8(stream, error); // intu8 century
	pointer->year = read_intu8(stream, error); // intu8 year
	pointer->month = read_intu8(stream, error); // intu8 month
	pointer->day = read_intu8(stream, error); // intu8 day
	pointer->hour = read_intu8(stream, error); // intu8 hour
	pointer->minute = read_intu8(stream, error); // intu8 minute
	pointer->second = read_intu8(stream, error); // intu8 second
	pointer->sec_fractions = read_intu8(stream, error); // intu8 sec_fractions
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(NuObsValue));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_nuobsvalue(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec nuobsvaluecmp underflow");
				break;
			}
		}
	}
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
	pointer->data_req_id = read_intu16(stream, error); // DataReqId data_req_id
	pointer->scan_report_no = read_intu16(stream, error); // intu16 scan_report_no
	decode_scanreportperfixedlist(stream, &pointer->scan_per_fixed, error);
	// ScanReportPerFixedList scan_per_fixed
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
	pointer->problem = read_intu16(stream, error); // RorjProblem problem
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
	decode_uuid_ident(stream, &pointer->data_proto_id_ext, error); // UUID_Ident data_proto_id_ext
	decode_any(stream, &pointer->data_proto_info_ext, error); // Any data_proto_info_ext
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
	pointer->metric_id = read_intu16(stream, error); // OID_Type metric_id
	pointer->state = read_intu16(stream, error); // MeasurementStatus state
	decode_enumval(stream, &pointer->value, error); // EnumVal value
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
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error)
		return;

	if (pointer->length > 0) {
		pointer->value = calloc(pointer->length, sizeof(intu8));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->length = 0;
			return;
		}

		read_intu8_many(stream, pointer->value, pointer->length, error);
		if (*error) {
			*error = 2;
			free(pointer->value);
			pointer->value = NULL;
		}
	}
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
		*(pointer->value + i) = read_intu8(stream, error);
		if (*error) {
			DEBUG("dec highresrelativetime underflow");
			// but let it fill with zeros
		}
	}
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
	pointer->sample_size = read_intu8(stream, error); // intu8 sample_size
	pointer->significant_bits = read_intu8(stream, error); // intu8 significant_bits
}

/**
 * Decodes AttributeList.
 *
 * @param stream the AttributeList content decoded as ByteStreamReader.
 * @param pointer the AttributeList to be decoded.
 * @param err error feedback
 */
void decode_attributelist(ByteStreamReader *stream, AttributeList *pointer, int *err)
{
	pointer->count = read_intu16(stream, err); // intu16 count
	if (*err) {
		DEBUG("attribute list: count err");
		return;
	}

	pointer->length = read_intu16(stream, err); // intu16 length
	if (*err) {
		DEBUG("attribute list: length err");
		return;
	}

	if (pointer->length > stream->unread_bytes) {
		DEBUG("attribute list: underflow %d > %d",
			pointer->length, stream->unread_bytes);
		*err = 1;
		return;
	}

	pointer->value = NULL;

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(AVA_Type));

		if (pointer->value == NULL) {
			*err = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_ava_type(stream, pointer->value + i, err);
			if (*err) {
				DEBUG("attribute list: ava undeflow");
				return;
			}
		}
	}
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(InstNumber));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_intu16(stream, error);
			if (*error) {
				DEBUG("dec segmidlist underflow");
				break;
			}
		}
	}
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(SimpleNuObsValue));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_float(stream, error);
			if (*error) {
				DEBUG("dec cmp obs value: undeflow");
				break;
			}
		}
	}
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
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	decode_attributelist(stream, &pointer->attribute_list, error); // AttributeList attribute_list
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(HANDLE));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_intu16(stream, error);
			if (*error) {
				DEBUG("dec handle list: undeflow");
				break;
			}
		}
	}
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
	pointer->segm_instance = read_intu16(stream, error); // InstNumber segm_instance
	pointer->segm_evt_entry_index = read_intu32(stream, error); // intu32 segm_evt_entry_index
	pointer->segm_evt_entry_count = read_intu32(stream, error); // intu32 segm_evt_entry_count
	pointer->segm_evt_status = read_intu16(stream, error); // SegmEvtStatus segm_evt_status
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(AttrValMapEntry));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_attrvalmapentry(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec attrvalmap underflow");
				break;
			}
		}
	}
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
	pointer->lower_absolute_value = read_float(stream, error); // FLOAT_Type lower_absolute_value
	pointer->upper_absolute_value = read_float(stream, error); // FLOAT_Type upper_absolute_value
	pointer->lower_scaled_value = read_intu8(stream, error); // intu8 lower_scaled_value
	pointer->upper_scaled_value = read_intu8(stream, error); // intu8 upper_scaled_value
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
	pointer->protocolVersion = read_intu32(stream, error); // ProtocolVersion protocolVersion
	pointer->encodingRules = read_intu16(stream, error); // EncodingRules encodingRules
	pointer->nomenclatureVersion = read_intu32(stream, error); // NomenclatureVersion nomenclatureVersion
	pointer->functionalUnits = read_intu32(stream, error); // FunctionalUnits functionalUnits
	pointer->systemType = read_intu32(stream, error); // SystemType systemType
	decode_octet_string(stream, &pointer->system_id, error); // octet_string system_id
	pointer->dev_config_id = read_intu16(stream, error); // intu16 dev_config_id
	decode_datareqmodecapab(stream, &pointer->data_req_mode_capab, error);
	// DataReqModeCapab data_req_mode_capab
	decode_attributelist(stream, &pointer->optionList, error); // AttributeList optionList
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ScanReportPerFixed));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_scanreportperfixed(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec scanreportperfixedlist underflow");
				break;
			}
		}
	}
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ScanReportPerGrouped));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_scanreportpergrouped(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec scanreportpergroupedlist underflow");
				break;
			}
		}
	}
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(DataProto));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_dataproto(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec dataprotolist underflow");
				break;
			}
		}
	}
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
	pointer->choice = read_intu16(stream, error); // SegmSelection_choice choice
	pointer->length = read_intu16(stream, error); // intu16 length

	switch (pointer->choice) {
	case ALL_SEGMENTS_CHOSEN:
		pointer->u.all_segments = read_intu16(stream, error);
		break;
	case SEGM_ID_LIST_CHOSEN:
		decode_segmidlist(stream, &pointer->u.segm_id_list, error);
		break;
	case ABS_TIME_RANGE_CHOSEN:
		decode_abstimerange(stream, &pointer->u.abs_time_range, error);
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
 * @param error Error feedback
 */
void decode_errorresult(ByteStreamReader *stream, ErrorResult *pointer, int *error)
{
	pointer->error_value = read_intu16(stream, error); // ERROR error_value
	decode_any(stream, &pointer->parameter, error); // Any parameter
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(HandleAttrValMapEntry));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_handleattrvalmapentry(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec handleattrvalmap underflow");
				break;
			}
		}
	}
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
		*(pointer->value + i) = read_intu8(stream, error);
		if (*error) {
			DEBUG("dec absolutetimeadjust underflow");
			// but let it fill with zeros
		}
	}

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
	pointer->result = read_intu16(stream, error); // Associate_result result
	decode_dataproto(stream, &pointer->selected_data_proto, error); // DataProto selected_data_proto
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
	pointer->reason = read_intu16(stream, error); // Release_response_reason reason
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(OID_Type));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_intu16(stream, error);
			if (*error) {
				DEBUG("dec metricidlist underflow");
				break;
			}
		}
	}
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
	pointer->person_id = read_intu16(stream, error); // intu16 person_id
	decode_observationscanfixedlist(stream, &pointer->obs_scan_fix, error);
	// ObservationScanFixedList obs_scan_fix
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
	pointer->data_req_id = read_intu16(stream, error); // intu16 data_req_id
	pointer->scan_report_no = read_intu16(stream, error); // intu16 scan_report_no
	decode_scanreportinfogroupedlist(stream, &pointer->obs_scan_grouped, error);
	// ScanReportInfoGroupedList obs_scan_grouped
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
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	decode_attributelist(stream, &pointer->attributes, error); // AttributeList attributes
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
	pointer->person_id = read_intu16(stream, error); // PersonId person_id
	decode_octet_string(stream, &pointer->obs_scan_grouped, error);
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
	decode_octet_string(stream, &pointer->manufacturer, error); // octet_string manufacturer
	decode_octet_string(stream, &pointer->model_number, error); // octet_string model_number
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ObservationScan));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_observationscan(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec observationscanlist underflow");
				break;
			}
		}
	}
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
	pointer->choice = read_intu16(stream, error); // APDU_choice choice
	pointer->length = read_intu16(stream, error); // intu16 length

	if (*error) {
		pointer->choice = pointer->length = 0;
		return;
	}

	switch (pointer->choice) {
	case AARQ_CHOSEN:
		decode_aarq_apdu(stream, &pointer->u.aarq, error);
		break;
	case AARE_CHOSEN:
		decode_aare_apdu(stream, &pointer->u.aare, error);
		break;
	case RLRQ_CHOSEN:
		decode_rlrq_apdu(stream, &pointer->u.rlrq, error);
		break;
	case RLRE_CHOSEN:
		decode_rlre_apdu(stream, &pointer->u.rlre, error);
		break;
	case ABRT_CHOSEN:
		decode_abrt_apdu(stream, &pointer->u.abrt, error);
		break;
	case PRST_CHOSEN:
		decode_prst_apdu(stream, &pointer->u.prst, error);
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
void decode_prst_apdu(ByteStreamReader *stream, PRST_apdu *pointer, int *error)
{
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->length = 0;
		return;
	}

	if (pointer->length > 0) {
		DATA_apdu *data = calloc(1, sizeof(DATA_apdu));

		if (data == NULL) {
			*error = 1;
			pointer->length = 0;
			return;
		} else {
			decode_data_apdu(stream, data, error);

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
void decode_pmsegmententrymap(ByteStreamReader *stream, PmSegmentEntryMap *pointer, int *error)
{
	pointer->segm_entry_header = read_intu16(stream, error); // SegmEntryHeader segm_entry_header
	decode_segmentryelemlist(stream, &pointer->segm_entry_elem_list, error);
	// SegmEntryElemList segm_entry_elem_list
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
	pointer->length = read_intu16(stream, error); // intu16 length
	if (*error) {
		DEBUG("any struct: length error");
		return;
	}

	pointer->value = NULL;

	if (pointer->length > stream->unread_bytes) {
		DEBUG("any struct: underflow");
		*error = 1;
		return;
	}

	if (pointer->length > 0) {
		pointer->value = calloc(pointer->length, sizeof(intu8));

		if (pointer->value == NULL) {
			pointer->length = 0;
			*error = 1;
			return;
		}

		read_intu8_many(stream, pointer->value, pointer->length, error);

		if (*error) {
			memset(pointer->value, 0, pointer->length);
			return;
		}
	}
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
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	decode_modificationlist(stream, &pointer->modification_list, error);
	// ModificationList modification_list
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
	pointer->seg_inst_no = read_intu16(stream, error); // InstNumber seg_inst_no
	if (*error)
		return;
	decode_attributelist(stream, &pointer->seg_info, error); // AttributeList seg_info
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(SegmElemStaticAttrEntry));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_segmelemstaticattrentry(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec pmsegelemstatiattrlist underflow");
				break;
			}
		}
	}
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
	decode_absolutetime(stream, &pointer->from_time, error); // AbsoluteTime from_time
	decode_absolutetime(stream, &pointer->to_time, error); // AbsoluteTime to_time
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
	pointer->data_req_id = read_intu16(stream, error); // DataReqId data_req_id
	pointer->scan_report_no = read_intu16(stream, error); // intu16 scan_report_no
	decode_scanreportpervarlist(stream, &pointer->scan_per_var, error); // ScanReportPerVarList scan_per_var
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
		*(pointer->value + i) = read_intu8(stream, error);
		if (*error) {
			DEBUG("dec uuid_ident underflow");
			break;
		}
	}
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
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	decode_attributeidlist(stream, &pointer->attribute_id_list, error);
	// AttributeIdList attribute_id_list
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(RegCertData));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_regcertdata(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec regcertdatalist underflow");
				break;
			}
		}
	}
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
	pointer->config_report_id = read_intu16(stream, error); // ConfigId config_report_id
	pointer->config_result = read_intu16(stream, error); // ConfigResult config_result
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
	pointer->data_proto_id = read_intu16(stream, error); // DataProtoId data_proto_id
	decode_any(stream, &pointer->data_proto_info, error); // Any data_proto_info
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
	pointer->ms_struct = read_intu8(stream, error); // intu8 ms_struct
	pointer->ms_comp_no = read_intu8(stream, error); // intu8 ms_comp_no
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
	pointer->segm_stat_type = read_intu16(stream, error); // SegmStatType segm_stat_type
	decode_octet_string(stream, &pointer->segm_stat_entry, error);
	// octet_string segm_stat_entry
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
	decode_segmdataeventdescr(stream, &pointer->segm_data_event_descr, error);
	// SegmDataEventDescr segm_data_event_descr
	decode_octet_string(stream, &pointer->segm_data_event_entries, error);
	// octet_string segm_data_event_entries
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(SegmEntryElem));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_segmentryelem(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec segmentryelemlist underflow");
				break;
			}
		}
	}
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
	pointer->array_size = read_intu16(stream, error); // intu16 array_size
	decode_sampletype(stream, &pointer->sample_type, error); // SampleType sample_type
	pointer->flags = read_intu16(stream, error); // SaFlags flags
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
	pointer->modify_operator = read_intu16(stream, error); // ModifyOperator modify_operator
	decode_ava_type(stream, &pointer->attribute, error); // AVA_Type attribute
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
	pointer->mds_time_cap_state = read_intu16(stream, error); // MdsTimeCapState mds_time_cap_state
	pointer->time_sync_protocol = read_intu16(stream, error); // TimeProtocolId time_sync_protocol
	pointer->time_sync_accuracy = read_intu32(stream, error); // RelativeTime time_sync_accuracy
	pointer->time_resolution_abs_time = read_intu16(stream, error); // intu16 time_resolution_abs_time
	pointer->time_resolution_rel_time = read_intu16(stream, error); // intu16 time_resolution_rel_time
	pointer->time_resolution_high_res_time = read_intu32(stream, error); // intu32 time_resolution_high_res_time
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
	pointer->choice = read_intu16(stream, error); // EnumVal_choice choice
	pointer->length = read_intu16(stream, error); // intu16 length

	if (*error) {
		pointer->choice = pointer->length = 0;
		return;
	}

	switch (pointer->choice) {
	case OBJ_ID_CHOSEN:
		pointer->u.enum_obj_id = read_intu16(stream, error);
		break;
	case TEXT_STRING_CHOSEN:
		decode_octet_string(stream, &pointer->u.enum_text_string, error);
		break;
	case BIT_STR_CHOSEN:
		pointer->u.enum_bit_str = read_intu32(stream, error);
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
 * @param error Error feedback
 */
void decode_trigsegmdataxferreq(ByteStreamReader *stream,
				TrigSegmDataXferReq *pointer, int *error)
{
	pointer->seg_inst_no = read_intu16(stream, error); // InstNumber seg_inst_no
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
	pointer->value = read_float(stream, error); // FLOAT_Type value
	pointer->unit = read_intu16(stream, error); // OID_Type unit
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(SegmentStatisticEntry));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_segmentstatisticentry(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec segmentstatistics underflow");
				break;
			}
		}
	}
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(OID_Type));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_intu16(stream, error);
			if (*error) {
				DEBUG("dec attributeidlist underflow");
				break;
			}
		}
	}
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
	pointer->data_req_id = read_intu16(stream, error); // DataReqId data_req_id
	pointer->scan_report_no = read_intu16(stream, error); // intu16 scan_report_no
	decode_observationscanfixedlist(stream, &pointer->obs_scan_fixed, error);
	// ObservationScanFixedList obs_scan_fixed
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
	pointer->data_req_id = read_intu16(stream, error); // DataReqId data_req_id
	pointer->data_req_mode = read_intu16(stream, error); // DataReqMode data_req_mode
	pointer->data_req_time = read_intu32(stream, error); // RelativeTime data_req_time
	pointer->data_req_person_id = read_intu16(stream, error); // intu16 DataRequest_data_req_person_id
	pointer->data_req_class = read_intu16(stream, error); // OID_Type data_req_class
	decode_handlelist(stream, &pointer->data_req_obj_handle_list, error);
	// HANDLEList data_req_obj_handle_list
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
	pointer->auth_body = read_intu8(stream, error); // AuthBody auth_body
	pointer->auth_body_struc_type = read_intu8(stream, error); // AuthBodyStrucType auth_body_struc_type
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
	pointer->reason = read_intu16(stream, error); // Release_request_reason reason
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
	pointer->choice = read_intu16(stream, error); // DATA_apdu_choice choice
	pointer->length = read_intu16(stream, error); // intu16 length

	if (*error) {
		pointer->choice = pointer->length = 0;
		return;
	}

	switch (pointer->choice) {
	case ROIV_CMIP_EVENT_REPORT_CHOSEN:
		decode_eventreportargumentsimple(stream,
						 &pointer->u.roiv_cmipEventReport, error);
		break;
	case ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		decode_eventreportargumentsimple(stream,
						 &pointer->u.roiv_cmipConfirmedEventReport, error);
		break;
	case ROIV_CMIP_GET_CHOSEN:
		decode_getargumentsimple(stream, &pointer->u.roiv_cmipGet, error);
		break;
	case ROIV_CMIP_SET_CHOSEN:
		decode_setargumentsimple(stream, &pointer->u.roiv_cmipSet, error);
		break;
	case ROIV_CMIP_CONFIRMED_SET_CHOSEN:
		decode_setargumentsimple(stream,
					 &pointer->u.roiv_cmipConfirmedSet, error);
		break;
	case ROIV_CMIP_ACTION_CHOSEN:
		decode_actionargumentsimple(stream, &pointer->u.roiv_cmipAction, error);
		break;
	case ROIV_CMIP_CONFIRMED_ACTION_CHOSEN:
		decode_actionargumentsimple(stream,
					    &pointer->u.roiv_cmipConfirmedAction, error);
		break;
	case RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		decode_eventreportresultsimple(stream,
					       &pointer->u.rors_cmipConfirmedEventReport, error);
		break;
	case RORS_CMIP_GET_CHOSEN:
		decode_getresultsimple(stream, &pointer->u.rors_cmipGet, error);
		break;
	case RORS_CMIP_CONFIRMED_SET_CHOSEN:
		decode_setresultsimple(stream,
				       &pointer->u.rors_cmipConfirmedSet, error);
		break;
	case RORS_CMIP_CONFIRMED_ACTION_CHOSEN:
		decode_actionresultsimple(stream,
					  &pointer->u.rors_cmipConfirmedAction, error);
		break;
	case ROER_CHOSEN:
		decode_errorresult(stream, &pointer->u.roer, error);
		break;
	case RORJ_CHOSEN:
		decode_rejectresult(stream, &pointer->u.rorj, error);
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
 * @param error Error feedback
 */
void decode_eventreportargumentsimple(ByteStreamReader *stream,
				      EventReportArgumentSimple *pointer, int *error)
{
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	pointer->event_time = read_intu32(stream, error); // RelativeTime event_time
	pointer->event_type = read_intu16(stream, error); // OID_Type event_type
	decode_any(stream, &pointer->event_info, error); // Any event_info
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
	pointer->data_req_id = read_intu16(stream, error); // DataReqId data_req_id
	pointer->scan_report_no = read_intu16(stream, error); // intu16 scan_report_no
	decode_observationscanlist(stream, &pointer->obs_scan_var, error); // ObservationScanList obs_scan_var
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
	pointer->data_req_id = read_intu16(stream, error); // intu16 data_req_id
	pointer->scan_report_no = read_intu16(stream, error); // intu16 scan_report_no
	decode_scanreportpergroupedlist(stream, &pointer->scan_per_grouped, error); // ScanReportPerGroupedList scan_per_grouped
}

/**
 * Decode ConfigObject
 *
 * @param *stream
 * @param *pointer
 * @param err Error feedback
 */
void decode_configobject(ByteStreamReader *stream, ConfigObject *pointer, int *err)
{
	pointer->obj_class = read_intu16(stream, err); // OID_Type obj_class
	if (*err) {
		return;
	}
	pointer->obj_handle = read_intu16(stream, err); // HANDLE obj_handle
	if (*err) {
		return;
	}
	decode_attributelist(stream, &pointer->attributes, err); // AttributeList attributes
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count,
					sizeof(ObservationScanGrouped));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_octet_string(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec scanreportinfogroupedlist underflow");
				break;
			}
		}
	}
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
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	pointer->currentTime = read_intu32(stream, error); // RelativeTime currentTime
	pointer->event_type = read_intu16(stream, error); // OID_Type event_type
	decode_any(stream, &pointer->event_reply_info, error); // Any event_reply_info
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
	pointer->partition = read_intu16(stream, error); // NomPartition partition
	pointer->code = read_intu16(stream, error); // OID_Type code
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
	*pointer = read_intu16(stream, error);
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
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	decode_octet_string(stream, &pointer->obs_val_data, error); // octet_string obs_val_data
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
	pointer->rel_time_stamp = read_intu32(stream, error); // RelativeTime rel_time_stamp
	pointer->data_req_result = read_intu16(stream, error); // DataReqResult data_req_result
	pointer->event_type = read_intu16(stream, error); // OID_Type event_type
	decode_any(stream, &pointer->event_info, error); // Any event_info
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
	pointer->spec_type = read_intu16(stream, error); // ProdSpecEntry_spec_type spec_type
	pointer->component_id = read_intu16(stream, error); // PrivateOid component_id
	decode_octet_string(stream, &pointer->prod_spec, error); // octet_string prod_spec
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
	pointer->lower_absolute_value = read_float(stream, error); // FLOAT_Type lower_absolute_value
	pointer->upper_absolute_value = read_float(stream, error); // FLOAT_Type upper_absolute_value
	pointer->lower_scaled_value = read_intu16(stream, error); // intu16 lower_scaled_value
	pointer->upper_scaled_value = read_intu16(stream, error); // intu16 upper_scaled_value
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
	pointer->class_id = read_intu16(stream, error); // OID_Type class_id
	decode_type(stream, &pointer->metric_type, error); // TYPE metric_type
	pointer->handle = read_intu16(stream, error); // HANDLE handle
	decode_attrvalmap(stream, &pointer->attr_val_map, error); // AttrValMap attr_val_map
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
	pointer->reason = read_intu16(stream, error); // Abort_reason reason
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
	pointer->data_req_mode_flags = read_intu16(stream, error); // DataReqModeFlags data_req_mode_flags
	pointer->data_req_init_agent_count = read_intu8(stream, error); // intu8 data_req_init_agent_count
	pointer->data_req_init_manager_count = read_intu8(stream, error); // intu8 data_req_init_manager_count
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(TYPE));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_type(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec supplementaltypelist underflow");
				break;
			}
		}
	}
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ObservationScanFixed));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_observationscanfixed(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec observationscanfixedlist underflow");
				break;
			}
		}
	}
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
	pointer->seg_inst_no = read_intu16(stream, error); // InstNumber seg_inst_no
	pointer->trig_segm_xfer_rsp = read_intu16(stream, error); // TrigSegmXferRsp trig_segm_xfer_rsp
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
	pointer->invoke_id = read_intu16(stream, error); // InvokeIDType invoke_id
	decode_data_apdu_message(stream, &pointer->message, error); // Data_apdu_message message
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
	pointer->assoc_version = read_intu32(stream, error); // AssociationVersion assoc_version
	decode_dataprotolist(stream, &pointer->data_proto_list, error); // DataProtoList data_proto_list
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(TypeVer));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_typever(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec typeverlist underflow");
				break;
			}
		}
	}
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
	decode_authbodyandstructype(stream, &pointer->auth_body_and_struc_type, error);
	// AuthBodyAndStrucType auth_body_and_struc_type
	decode_any(stream, &pointer->auth_body_data, error); // Any auth_body_data
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
	pointer->metric_id = read_intu16(stream, error); // OID_Type metric_id
	pointer->state = read_intu16(stream, error); // MeasurementStatus state
	pointer->unit_code = read_intu16(stream, error); // OID_Type unit_code
	pointer->value = read_float(stream, error); // FLOAT_Type value
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(ScanReportPerVar));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_scanreportpervar(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec scanreportpervarlist underflow");
				break;
			}
		}
	}
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
	decode_absolutetime(stream, &pointer->date_time, error); // AbsoluteTime date_time
	pointer->accuracy = read_float(stream, error); // FLOAT_Type accuracy
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(SegmentInfo));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_segmentinfo(stream, pointer->value + i, error);
			if (*error) {
				DEBUG("dec segmentinfolist underflow");
				break;
			}
		}
	}
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
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	pointer->action_type = read_intu16(stream, error); // OID_Type action_type
	decode_any(stream, &pointer->action_info_args, error); // Any action_info_args
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
	pointer->class_id = read_intu16(stream, error); // OID_Type class_id
	decode_type(stream, &pointer->metric_type, error); // TYPE metric_type
	decode_attributelist(stream, &pointer->attribute_list, error); // AttributeList attribute_list
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		pointer->value = calloc(pointer->count, sizeof(BasicNuObsValue));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			*(pointer->value + i) = read_sfloat(stream, error);
			if (*error) {
				DEBUG("dec cmp basic nu undeflow");
				break;
			}
		}
	}
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
	pointer->count = read_intu16(stream, error); // intu16 count
	pointer->length = read_intu16(stream, error); // intu16 length
	pointer->value = NULL;

	if (*error) {
		pointer->count = pointer->length = 0;
		return;
	}

	if (pointer->count > 0) {
		if (pointer->length > stream->unread_bytes) {
			DEBUG("object list incomplete");
			pointer->count = pointer->length = 0;
			*error = 1;
			return;
		}

		pointer->value = calloc(pointer->count, sizeof(ConfigObject));

		if (pointer->value == NULL) {
			*error = 1;
			pointer->count = pointer->length = 0;
			return;
		}

		int i;

		for (i = 0; i < pointer->count; i++) {
			decode_configobject(stream, pointer->value + i, error);
			if (*error) {
				pointer->count = pointer->length = 0;
				free(pointer->value);
				pointer->value = 0;
				return;
			}
		}
	}
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
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	decode_attributelist(stream, &pointer->attribute_list, error); // AttributeList attribute_list
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
	pointer->obj_handle = read_intu16(stream, error); // HANDLE obj_handle
	decode_attrvalmap(stream, &pointer->attr_val_map, error); // AttrValMap attr_val_map
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
	*pointer = read_float(stream, error);
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
	*pointer = read_intu16(stream, error);
}

/** @} */
