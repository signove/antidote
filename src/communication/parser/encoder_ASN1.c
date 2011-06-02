/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file encoder_ASN1.c
 * \brief ASN1 encoder implementation.
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
 * \author Walter Guerra
 * \date Jun 22, 2010
 */


/**
 * \addtogroup ASN1Encoder
 * \ingroup Parser
 *
 * @{
 */
#include <src/asn1/phd_types.h>
#include <stdlib.h>
#include "src/util/bytelib.h"
#include "src/communication/parser/encoder_ASN1.h"

/**
 * Encode SegmentDataResult
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmentdataresult(ByteStreamWriter *stream,
			      SegmentDataResult *pointer)
{
	encode_segmdataeventdescr(stream, &pointer->segm_data_event_descr); // SegmDataEventDescr segm_data_event_descr
}

/**
 * Encode ScanReportPerVar
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportpervar(ByteStreamWriter *stream,
			     ScanReportPerVar *pointer)
{
	write_intu16(stream, pointer->person_id);
	encode_observationscanlist(stream, &pointer->obs_scan_var); // ObservationScanList obs_scan_var
}

/**
 * Encode TypeVer
 *
 * @param *stream
 * @param *pointer
 */
void encode_typever(ByteStreamWriter *stream, TypeVer *pointer)
{
	write_intu16(stream, pointer->type);
	write_intu16(stream, pointer->version);
}

/**
 * Encode ModificationList
 *
 * @param *stream
 * @param *pointer
 */
void encode_modificationlist(ByteStreamWriter *stream,
			     ModificationList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_attributemodentry(stream, pointer->value + i);
	}

}

/**
 * Encode ProductionSpec
 *
 * @param *stream
 * @param *pointer
 */
void encode_productionspec(ByteStreamWriter *stream, ProductionSpec *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_prodspecentry(stream, pointer->value + i);
	}

}

/**
 * Encode ActionArgumentSimple
 *
 * @param *stream
 * @param *pointer
 */
void encode_actionargumentsimple(ByteStreamWriter *stream,
				 ActionArgumentSimple *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	write_intu16(stream, pointer->action_type);
	encode_any(stream, &pointer->action_info_args); // Any action_info_args
}

/**
 * Encode ScaleRangeSpec32
 *
 * @param *stream
 * @param *pointer
 */
void encode_scalerangespec32(ByteStreamWriter *stream,
			     ScaleRangeSpec32 *pointer)
{
	write_float(stream, pointer->lower_absolute_value);
	write_float(stream, pointer->upper_absolute_value);
	write_intu32(stream, pointer->lower_scaled_value);
	write_intu32(stream, pointer->upper_scaled_value);
}

/**
 * Encode AVA_Type
 *
 * @param *stream
 * @param *pointer
 */
void encode_ava_type(ByteStreamWriter *stream, AVA_Type *pointer)
{
	write_intu16(stream, pointer->attribute_id);
	encode_any(stream, &pointer->attribute_value); // Any attribute_value
}

/**
 * Encode ConfigReport
 *
 * @param *stream
 * @param *pointer
 */
void encode_configreport(ByteStreamWriter *stream, ConfigReport *pointer)
{
	write_intu16(stream, pointer->config_report_id);
	encode_configobjectlist(stream, &pointer->config_obj_list); // ConfigObjectList config_obj_list
}

/**
 * Encode AttrValMapEntry
 *
 * @param *stream
 * @param *pointer
 */
void encode_attrvalmapentry(ByteStreamWriter *stream, AttrValMapEntry *pointer)
{
	write_intu16(stream, pointer->attribute_id);
	write_intu16(stream, pointer->attribute_len);
}

/**
 * Encode AbsoluteTime
 *
 * @param *stream
 * @param *pointer
 */
void encode_absolutetime(ByteStreamWriter *stream, AbsoluteTime *pointer)
{
	write_intu8(stream, pointer->century);
	write_intu8(stream, pointer->year);
	write_intu8(stream, pointer->month);
	write_intu8(stream, pointer->day);
	write_intu8(stream, pointer->hour);
	write_intu8(stream, pointer->minute);
	write_intu8(stream, pointer->second);
	write_intu8(stream, pointer->sec_fractions);
}

/**
 * Encode NuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 */
void encode_nuobsvaluecmp(ByteStreamWriter *stream, NuObsValueCmp *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_nuobsvalue(stream, pointer->value + i);
	}

}

/**
 * Encode ScanReportInfoMPFixed
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportinfompfixed(ByteStreamWriter *stream,
				  ScanReportInfoMPFixed *pointer)
{
	write_intu16(stream, pointer->data_req_id);
	write_intu16(stream, pointer->scan_report_no);
	encode_scanreportperfixedlist(stream, &pointer->scan_per_fixed); // ScanReportPerFixedList scan_per_fixed
}

/**
 * Encode RejectResult
 *
 * @param *stream
 * @param *pointer
 */
void encode_rejectresult(ByteStreamWriter *stream, RejectResult *pointer)
{
	write_intu16(stream, pointer->problem);
}

/**
 * Encode ManufSpecAssociationInformation
 *
 * @param *stream
 * @param *pointer
 */
void encode_manufspecassociationinformation(ByteStreamWriter *stream,
		ManufSpecAssociationInformation *pointer)
{
	encode_uuid_ident(stream, &pointer->data_proto_id_ext); // UUID_Ident data_proto_id_ext
	encode_any(stream, &pointer->data_proto_info_ext); // Any data_proto_info_ext
}

/**
 * Encode EnumObsValue
 *
 * @param *stream
 * @param *pointer
 */
void encode_enumobsvalue(ByteStreamWriter *stream, EnumObsValue *pointer)
{
	write_intu16(stream, pointer->metric_id);
	write_intu16(stream, pointer->state);
	encode_enumval(stream, &pointer->value); // EnumVal value
}

/**
 * Encode octet_string
 *
 * @param *stream
 * @param *pointer
 */
void encode_octet_string(ByteStreamWriter *stream, octet_string *pointer)
{
	write_intu16(stream, pointer->length);
	write_intu8_many(stream, pointer->value, pointer->length);
}

/**
 * Encode HighResRelativeTime
 *
 * @param *stream
 * @param *pointer
 */
void encode_highresrelativetime(ByteStreamWriter *stream,
				HighResRelativeTime *pointer)
{
	// intu8 value[8];
	write_intu8_many(stream, pointer->value, 8);
}

/**
 * Encode SampleType
 *
 * @param *stream
 * @param *pointer
 */
void encode_sampletype(ByteStreamWriter *stream, SampleType *pointer)
{
	write_intu8(stream, pointer->sample_size);
	write_intu8(stream, pointer->significant_bits);
}

/**
 * Encode AttributeList
 *
 * @param *stream
 * @param *pointer
 */
void encode_attributelist(ByteStreamWriter *stream, AttributeList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_ava_type(stream, pointer->value + i);
	}

}

/**
 * Encode SegmIdList
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmidlist(ByteStreamWriter *stream, SegmIdList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		write_intu16(stream, *(pointer->value + i));
	}

}

/**
 * Encode SimpleNuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 */
void encode_simplenuobsvaluecmp(ByteStreamWriter *stream,
				SimpleNuObsValueCmp *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		write_float(stream, *(pointer->value + i));
	}
}

/**
 * Encode GetResultSimple
 *
 * @param *stream
 * @param *pointer
 */
void encode_getresultsimple(ByteStreamWriter *stream, GetResultSimple *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	encode_attributelist(stream, &pointer->attribute_list); // AttributeList attribute_list
}

/**
 * Encode HANDLEList
 *
 * @param *stream
 * @param *pointer
 */
void encode_handlelist(ByteStreamWriter *stream, HANDLEList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		write_intu16(stream, *(pointer->value + i));
	}

}

/**
 * Encode SegmDataEventDescr
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmdataeventdescr(ByteStreamWriter *stream,
			       SegmDataEventDescr *pointer)
{
	write_intu16(stream, pointer->segm_instance);
	write_intu32(stream, pointer->segm_evt_entry_index);
	write_intu32(stream, pointer->segm_evt_entry_count);
	write_intu16(stream, pointer->segm_evt_status);
}

/**
 * Encode AttrValMap
 *
 * @param *stream
 * @param *pointer
 */
void encode_attrvalmap(ByteStreamWriter *stream, AttrValMap *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_attrvalmapentry(stream, pointer->value + i);
	}

}

/**
 * Encode ScaleRangeSpec8
 *
 * @param *stream
 * @param *pointer
 */
void encode_scalerangespec8(ByteStreamWriter *stream, ScaleRangeSpec8 *pointer)
{
	write_float(stream, pointer->lower_absolute_value);
	write_float(stream, pointer->upper_absolute_value);
	write_intu8(stream, pointer->lower_scaled_value);
	write_intu8(stream, pointer->upper_scaled_value);
}

/**
 * Encode PhdAssociationInformation
 *
 * @param *stream
 * @param *pointer
 */
void encode_phdassociationinformation(ByteStreamWriter *stream,
				      PhdAssociationInformation *pointer)
{
	write_intu32(stream, pointer->protocolVersion);
	write_intu16(stream, pointer->encodingRules);
	write_intu32(stream, pointer->nomenclatureVersion);
	write_intu32(stream, pointer->functionalUnits);
	write_intu32(stream, pointer->systemType);
	encode_octet_string(stream, &pointer->system_id); // octet_string system_id
	write_intu16(stream, pointer->dev_config_id);
	encode_datareqmodecapab(stream, &pointer->data_req_mode_capab); // DataReqModeCapab data_req_mode_capab
	encode_attributelist(stream, &pointer->optionList); // AttributeList optionList
}

/**
 * Encode ScanReportPerFixedList
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportperfixedlist(ByteStreamWriter *stream,
				   ScanReportPerFixedList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_scanreportperfixed(stream, pointer->value + i);
	}

}

/**
 * Encode ScanReportPerGroupedList
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportpergroupedlist(ByteStreamWriter *stream,
				     ScanReportPerGroupedList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_scanreportpergrouped(stream, pointer->value + i);
	}

}

/**
 * Encode DataProtoList
 *
 * @param *stream
 * @param *pointer
 */
void encode_dataprotolist(ByteStreamWriter *stream, DataProtoList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_dataproto(stream, pointer->value + i);
	}

}

/**
 * Encode SegmSelection
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmselection(ByteStreamWriter *stream, SegmSelection *pointer)
{
	write_intu16(stream, pointer->choice);
	write_intu16(stream, pointer->length);

	switch (pointer->choice) {
	case ALL_SEGMENTS_CHOSEN:
		write_intu16(stream, pointer->u.all_segments);
		break;
	case SEGM_ID_LIST_CHOSEN:
		encode_segmidlist(stream, &pointer->u.segm_id_list);
		break;
	case ABS_TIME_RANGE_CHOSEN:
		encode_abstimerange(stream, &pointer->u.abs_time_range);
		break;
	default:
		// TODO: manage errors
		break;
	}
}

/**
 * Encode ErrorResult
 *
 * @param *stream
 * @param *pointer
 */
void encode_errorresult(ByteStreamWriter *stream, ErrorResult *pointer)
{
	write_intu16(stream, pointer->error_value);
	encode_any(stream, &pointer->parameter); // Any parameter
}

/**
 * Encode HandleAttrValMap
 *
 * @param *stream
 * @param *pointer
 */
void encode_handleattrvalmap(ByteStreamWriter *stream,
			     HandleAttrValMap *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_handleattrvalmapentry(stream, pointer->value + i);
	}
}

/**
 * Encode AbsoluteTimeAdjust
 *
 * @param *stream
 * @param *pointer
 */
void encode_absolutetimeadjust(ByteStreamWriter *stream,
			       AbsoluteTimeAdjust *pointer)
{
	// intu8 value[6]
	write_intu8_many(stream, pointer->value, 6);

}

/**
 * Encode AARE_apdu
 *
 * @param *stream
 * @param *pointer
 */
void encode_aare_apdu(ByteStreamWriter *stream, AARE_apdu *pointer)
{
	write_intu16(stream, pointer->result);
	encode_dataproto(stream, &pointer->selected_data_proto); // DataProto selected_data_proto
}

/**
 * Encode RLRE_apdu
 *
 * @param *stream
 * @param *pointer
 */
void encode_rlre_apdu(ByteStreamWriter *stream, RLRE_apdu *pointer)
{
	write_intu16(stream, pointer->reason);
}

/**
 * Encode MetricIdList
 *
 * @param *stream
 * @param *pointer
 */
void encode_metricidlist(ByteStreamWriter *stream, MetricIdList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		write_intu16(stream, *(pointer->value + i));
	}

}

/**
 * Encode ScanReportPerFixed
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportperfixed(ByteStreamWriter *stream,
			       ScanReportPerFixed *pointer)
{
	write_intu16(stream, pointer->person_id);
	encode_observationscanfixedlist(stream, &pointer->obs_scan_fix); // ObservationScanFixedList obs_scan_fix
}

/**
 * Encode ScanReportInfoGrouped
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportinfogrouped(ByteStreamWriter *stream,
				  ScanReportInfoGrouped *pointer)
{
	write_intu16(stream, pointer->data_req_id);
	write_intu16(stream, pointer->scan_report_no);
	encode_scanreportinfogroupedlist(stream, &pointer->obs_scan_grouped); // ScanReportInfoGroupedList obs_scan_grouped
}

/**
 * Encode ObservationScan
 *
 * @param *stream
 * @param *pointer
 */
void encode_observationscan(ByteStreamWriter *stream, ObservationScan *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	encode_attributelist(stream, &pointer->attributes); // AttributeList attributes
}

/**
 * Encode ScanReportPerGrouped
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportpergrouped(ByteStreamWriter *stream,
				 ScanReportPerGrouped *pointer)
{
	write_intu16(stream, pointer->person_id);
	encode_octet_string(stream, &pointer->obs_scan_grouped);
}

/**
 * Encode SystemModel
 *
 * @param *stream
 * @param *pointer
 */
void encode_systemmodel(ByteStreamWriter *stream, SystemModel *pointer)
{
	encode_octet_string(stream, &pointer->manufacturer); // octet_string manufacturer
	encode_octet_string(stream, &pointer->model_number); // octet_string model_number
}

/**
 * Encode ObservationScanList
 *
 * @param *stream
 * @param *pointer
 */
void encode_observationscanlist(ByteStreamWriter *stream,
				ObservationScanList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_observationscan(stream, pointer->value + i);
	}
}

/**
 * Set Data_apdu into PRST_apdu definition.
 *
 * This function sets a pointer to Data_apdu in prst->value field
 * instead of store encoded/decoded bytes (array of intu8).
 *
 * The real encoding will be executed when the encoder_apdu() function
 * parses all APDU structure to a binary stream.
 *
 * This avoids the need to encode Data_apdu and after copy
 * bytes to stream, the encode_apdu() function writes encoded bytes
 * directly in binary stream.
 *
 * The same approach is valid to decode_apdu function, you should use
 * encode_get_data_apdu to retrieve Data_apdu from PRST_apdu.
 *
 * Although this forces a strange casting operation, it enables
 * a better encoder performance since there is no bytes copy overhead.
 *
 * @param prst
 * @param data_apdu
 */
void encode_set_data_apdu(PRST_apdu *prst, DATA_apdu *data_apdu)
{
	prst->value = (void *) data_apdu;
}

/**
 * Get Data_apdu from PRST_apdu definition, see encode_set_data_apdu().
 *
 * @param prst
 * @return data_apdu
 */
DATA_apdu *encode_get_data_apdu(PRST_apdu *prst)
{
	return (DATA_apdu *) prst->value;
}

/**
 * Encode APDU
 *
 * @param *stream
 * @param *pointer
 */
void encode_apdu(ByteStreamWriter *stream, APDU *pointer)
{
	write_intu16(stream, pointer->choice);
	write_intu16(stream, pointer->length);

	switch (pointer->choice) {
	case AARQ_CHOSEN:
		encode_aarq_apdu(stream, &pointer->u.aarq);
		break;
	case AARE_CHOSEN:
		encode_aare_apdu(stream, &pointer->u.aare);
		break;
	case RLRQ_CHOSEN:
		encode_rlrq_apdu(stream, &pointer->u.rlrq);
		break;
	case RLRE_CHOSEN:
		encode_rlre_apdu(stream, &pointer->u.rlre);
		break;
	case ABRT_CHOSEN:
		encode_abrt_apdu(stream, &pointer->u.abrt);
		break;
	case PRST_CHOSEN:
		encode_prst_apdu(stream, &pointer->u.prst);
		break;
	default:
		// TODO: manage errors
		break;
	}
}

/**
 * Encode PRST_apdu
 *
 * @param *stream
 * @param *pointer
 */
void encode_prst_apdu(ByteStreamWriter *stream, PRST_apdu *pointer)
{
	write_intu16(stream, pointer->length);

	if (pointer->value != NULL) {
		encode_data_apdu(stream, encode_get_data_apdu(pointer));
	} /* else / TODO: manage errors */
}

/**
 * Encode PmSegmentEntryMap
 *
 * @param *stream
 * @param *pointer
 */
void encode_pmsegmententrymap(ByteStreamWriter *stream,
			      PmSegmentEntryMap *pointer)
{
	write_intu16(stream, pointer->segm_entry_header);
	encode_segmentryelemlist(stream, &pointer->segm_entry_elem_list); // SegmEntryElemList segm_entry_elem_list
}

/**
 * Encode Any
 *
 * @param *stream
 * @param *pointer
 */
void encode_any(ByteStreamWriter *stream, Any *pointer)
{
	write_intu16(stream, pointer->length);
	write_intu8_many(stream, pointer->value, pointer->length);
}

/**
 * Encode SetArgumentSimple
 *
 * @param *stream
 * @param *pointer
 */
void encode_setargumentsimple(ByteStreamWriter *stream,
			      SetArgumentSimple *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	encode_modificationlist(stream, &pointer->modification_list); // ModificationList modification_list
}

/**
 * Encode SegmentInfo
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmentinfo(ByteStreamWriter *stream, SegmentInfo *pointer)
{
	write_intu16(stream, pointer->seg_inst_no);
	encode_attributelist(stream, &pointer->seg_info); // AttributeList seg_info
}

/**
 * Encode PmSegmElemStaticAttrList
 *
 * @param *stream
 * @param *pointer
 */
void encode_pmsegmelemstaticattrlist(ByteStreamWriter *stream,
				     PmSegmElemStaticAttrList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_segmelemstaticattrentry(stream, pointer->value + i);
	}

}

/**
 * Encode AbsTimeRange
 *
 * @param *stream
 * @param *pointer
 */
void encode_abstimerange(ByteStreamWriter *stream, AbsTimeRange *pointer)
{
	encode_absolutetime(stream, &pointer->from_time); // AbsoluteTime from_time
	encode_absolutetime(stream, &pointer->to_time); // AbsoluteTime to_time
}

/**
 * Encode ScanReportInfoMPVar
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportinfompvar(ByteStreamWriter *stream,
				ScanReportInfoMPVar *pointer)
{
	write_intu16(stream, pointer->data_req_id);
	write_intu16(stream, pointer->scan_report_no);
	encode_scanreportpervarlist(stream, &pointer->scan_per_var); // ScanReportPerVarList scan_per_var
}

/**
 * Encode UUID_Ident
 *
 * @param *stream
 * @param *pointer
 */
void encode_uuid_ident(ByteStreamWriter *stream, UUID_Ident *pointer)
{
	// intu8 value[16]
	write_intu8_many(stream, pointer->value, 16);
}

/**
 * Encode GetArgumentSimple
 *
 * @param *stream
 * @param *pointer
 */
void encode_getargumentsimple(ByteStreamWriter *stream,
			      GetArgumentSimple *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	encode_attributeidlist(stream, &pointer->attribute_id_list); // AttributeIdList attribute_id_list
}

/**
 * Encode RegCertDataList
 *
 * @param *stream
 * @param *pointer
 */
void encode_regcertdatalist(ByteStreamWriter *stream, RegCertDataList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_regcertdata(stream, pointer->value + i);
	}

}

/**
 * Encode ConfigReportRsp
 *
 * @param *stream
 * @param *pointer
 */
void encode_configreportrsp(ByteStreamWriter *stream, ConfigReportRsp *pointer)
{
	write_intu16(stream, pointer->config_report_id);
	write_intu16(stream, pointer->config_result);
}

/**
 * Encode DataProto
 *
 * @param *stream
 * @param *pointer
 */
void encode_dataproto(ByteStreamWriter *stream, DataProto *pointer)
{
	write_intu16(stream, pointer->data_proto_id);
	encode_any(stream, &pointer->data_proto_info); // Any data_proto_info
}

/**
 * Encode MetricStructureSmall
 *
 * @param *stream
 * @param *pointer
 */
void encode_metricstructuresmall(ByteStreamWriter *stream,
				 MetricStructureSmall *pointer)
{
	write_intu8(stream, pointer->ms_struct);
	write_intu8(stream, pointer->ms_comp_no);
}

/**
 * Encode SegmentStatisticEntry
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmentstatisticentry(ByteStreamWriter *stream,
				  SegmentStatisticEntry *pointer)
{
	write_intu16(stream, pointer->segm_stat_type);
	encode_octet_string(stream, &pointer->segm_stat_entry); // octet_string segm_stat_entry
}

/**
 * Encode SegmentDataEvent
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmentdataevent(ByteStreamWriter *stream,
			     SegmentDataEvent *pointer)
{
	encode_segmdataeventdescr(stream, &pointer->segm_data_event_descr); // SegmDataEventDescr segm_data_event_descr
	encode_octet_string(stream, &pointer->segm_data_event_entries); // octet_string segm_data_event_entries
}

/**
 * Encode SegmEntryElemList
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmentryelemlist(ByteStreamWriter *stream,
			      SegmEntryElemList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_segmentryelem(stream, pointer->value + i);
	}

}

/**
 * Encode SaSpec
 *
 * @param *stream
 * @param *pointer
 */
void encode_saspec(ByteStreamWriter *stream, SaSpec *pointer)
{
	write_intu16(stream, pointer->array_size);
	encode_sampletype(stream, &pointer->sample_type); // SampleType sample_type
	write_intu16(stream, pointer->flags);
}

/**
 * Encode AttributeModEntry
 *
 * @param *stream
 * @param *pointer
 */
void encode_attributemodentry(ByteStreamWriter *stream,
			      AttributeModEntry *pointer)
{
	write_intu16(stream, pointer->modify_operator);
	encode_ava_type(stream, &pointer->attribute); // AVA_Type attribute
}

/**
 * Encode MdsTimeInfo
 *
 * @param *stream
 * @param *pointer
 */
void encode_mdstimeinfo(ByteStreamWriter *stream, MdsTimeInfo *pointer)
{
	write_intu16(stream, pointer->mds_time_cap_state);
	write_intu16(stream, pointer->time_sync_protocol);
	write_intu32(stream, pointer->time_sync_accuracy);
	write_intu16(stream, pointer->time_resolution_abs_time);
	write_intu16(stream, pointer->time_resolution_rel_time);
	write_intu32(stream, pointer->time_resolution_high_res_time);
}

/**
 * Encode EnumVal
 *
 * @param *stream
 * @param *pointer
 */
void encode_enumval(ByteStreamWriter *stream, EnumVal *pointer)
{
	write_intu16(stream, pointer->choice);
	write_intu16(stream, pointer->length);

	switch (pointer->choice) {
	case OBJ_ID_CHOSEN:
		write_intu16(stream, pointer->u.enum_obj_id);
		break;
	case TEXT_STRING_CHOSEN:
		encode_octet_string(stream, &pointer->u.enum_text_string);
		break;
	case BIT_STR_CHOSEN:
		write_intu32(stream, pointer->u.enum_bit_str);
		break;
	default:
		break;
	}
}

/**
 * Encode TrigSegmDataXferReq
 *
 * @param *stream
 * @param *pointer
 */
void encode_trigsegmdataxferreq(ByteStreamWriter *stream,
				TrigSegmDataXferReq *pointer)
{
	write_intu16(stream, pointer->seg_inst_no);
}

/**
 * Encode BatMeasure
 *
 * @param *stream
 * @param *pointer
 */
void encode_batmeasure(ByteStreamWriter *stream, BatMeasure *pointer)
{
	write_float(stream, pointer->value);
	write_intu16(stream, pointer->unit);
}

/**
 * Encode SegmentStatistics
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmentstatistics(ByteStreamWriter *stream,
			      SegmentStatistics *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_segmentstatisticentry(stream, pointer->value + i);
	}

}

/**
 * Encode AttributeIdList
 *
 * @param *stream
 * @param *pointer
 */
void encode_attributeidlist(ByteStreamWriter *stream, AttributeIdList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		write_intu16(stream, *(pointer->value + i));
	}

}

/**
 * Encode ScanReportInfoFixed
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportinfofixed(ByteStreamWriter *stream,
				ScanReportInfoFixed *pointer)
{
	write_intu16(stream, pointer->data_req_id);
	write_intu16(stream, pointer->scan_report_no);
	encode_observationscanfixedlist(stream, &pointer->obs_scan_fixed); // ObservationScanFixedList obs_scan_fixed
}

/**
 * Encode DataRequest
 *
 * @param *stream
 * @param *pointer
 */
void encode_datarequest(ByteStreamWriter *stream, DataRequest *pointer)
{
	write_intu16(stream, pointer->data_req_id);
	write_intu16(stream, pointer->data_req_mode);
	write_intu32(stream, pointer->data_req_time);
	write_intu16(stream, pointer->data_req_person_id);
	write_intu16(stream, pointer->data_req_class);
	encode_handlelist(stream, &pointer->data_req_obj_handle_list); // HANDLEList data_req_obj_handle_list
}

/**
 * Encode AuthBodyAndStrucType
 *
 * @param *stream
 * @param *pointer
 */
void encode_authbodyandstructype(ByteStreamWriter *stream,
				 AuthBodyAndStrucType *pointer)
{
	write_intu8(stream, pointer->auth_body);
	write_intu8(stream, pointer->auth_body_struc_type);
}

/**
 * Encode RLRQ_apdu
 *
 * @param *stream
 * @param *pointer
 */
void encode_rlrq_apdu(ByteStreamWriter *stream, RLRQ_apdu *pointer)
{
	write_intu16(stream, pointer->reason);
}

/**
 * Encode Data_apdu_message
 *
 * @param *stream
 * @param *pointer
 */
void encode_data_apdu_message(ByteStreamWriter *stream,
			      Data_apdu_message *pointer)
{
	write_intu16(stream, pointer->choice);
	write_intu16(stream, pointer->length);

	switch (pointer->choice) {
	case ROIV_CMIP_EVENT_REPORT_CHOSEN:
		encode_eventreportargumentsimple(stream,
						 &pointer->u.roiv_cmipEventReport);
		break;
	case ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		encode_eventreportargumentsimple(stream,
						 &pointer->u.roiv_cmipConfirmedEventReport);
		break;
	case ROIV_CMIP_GET_CHOSEN:
		encode_getargumentsimple(stream, &pointer->u.roiv_cmipGet);
		break;
	case ROIV_CMIP_SET_CHOSEN:
		encode_setargumentsimple(stream, &pointer->u.roiv_cmipSet);
		break;
	case ROIV_CMIP_CONFIRMED_SET_CHOSEN:
		encode_setargumentsimple(stream,
					 &pointer->u.roiv_cmipConfirmedSet);
		break;
	case ROIV_CMIP_ACTION_CHOSEN:
		encode_actionargumentsimple(stream, &pointer->u.roiv_cmipAction);
		break;
	case ROIV_CMIP_CONFIRMED_ACTION_CHOSEN:
		encode_actionargumentsimple(stream,
					    &pointer->u.roiv_cmipConfirmedAction);
		break;
	case RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		encode_eventreportresultsimple(stream,
					       &pointer->u.rors_cmipConfirmedEventReport);
		break;
	case RORS_CMIP_GET_CHOSEN:
		encode_getresultsimple(stream, &pointer->u.rors_cmipGet);
		break;
	case RORS_CMIP_CONFIRMED_SET_CHOSEN:
		encode_setresultsimple(stream,
				       &pointer->u.rors_cmipConfirmedSet);
		break;
	case RORS_CMIP_CONFIRMED_ACTION_CHOSEN:
		encode_actionresultsimple(stream,
					  &pointer->u.rors_cmipConfirmedAction);
		break;
	case ROER_CHOSEN:
		encode_errorresult(stream, &pointer->u.roer);
		break;
	case RORJ_CHOSEN:
		encode_rejectresult(stream, &pointer->u.rorj);
		break;
	default:
		// TODO: manage errors
		break;
	}
}

/**
 * Encode EventReportArgumentSimple
 *
 * @param *stream
 * @param *pointer
 */
void encode_eventreportargumentsimple(ByteStreamWriter *stream,
				      EventReportArgumentSimple *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	write_intu32(stream, pointer->event_time);
	write_intu16(stream, pointer->event_type);
	encode_any(stream, &pointer->event_info); // Any event_info
}

/**
 * Encode ScanReportInfoVar
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportinfovar(ByteStreamWriter *stream,
			      ScanReportInfoVar *pointer)
{
	write_intu16(stream, pointer->data_req_id);
	write_intu16(stream, pointer->scan_report_no);
	encode_observationscanlist(stream, &pointer->obs_scan_var); // ObservationScanList obs_scan_var
}

/**
 * Encode ScanReportInfoMPGrouped
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportinfompgrouped(ByteStreamWriter *stream,
				    ScanReportInfoMPGrouped *pointer)
{
	write_intu16(stream, pointer->data_req_id);
	write_intu16(stream, pointer->scan_report_no);
	encode_scanreportpergroupedlist(stream, &pointer->scan_per_grouped); // ScanReportPerGroupedList scan_per_grouped
}

/**
 * Encode ConfigObject
 *
 * @param *stream
 * @param *pointer
 */
void encode_configobject(ByteStreamWriter *stream, ConfigObject *pointer)
{
	write_intu16(stream, pointer->obj_class);
	write_intu16(stream, pointer->obj_handle);
	encode_attributelist(stream, &pointer->attributes); // AttributeList attributes
}

/**
 * Encode ScanReportInfoGroupedList
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportinfogroupedlist(ByteStreamWriter *stream,
				      ScanReportInfoGroupedList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);

	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_octet_string(stream, pointer->value + i);
	}

}

/**
 * Encode EventReportResultSimple
 *
 * @param *stream
 * @param *pointer
 */
void encode_eventreportresultsimple(ByteStreamWriter *stream,
				    EventReportResultSimple *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	write_intu32(stream, pointer->currentTime);
	write_intu16(stream, pointer->event_type);
	encode_any(stream, &pointer->event_reply_info); // Any event_reply_info
}

/**
 * Encode TYPE
 *
 * @param *stream
 * @param *pointer
 */
void encode_type(ByteStreamWriter *stream, TYPE *pointer)
{
	write_intu16(stream, pointer->partition);
	write_intu16(stream, pointer->code);
}

/**
 * Encode MetricSpecSmall
 *
 * @param *stream
 * @param *pointer
 */
void encode_metricspecsmall(ByteStreamWriter *stream, MetricSpecSmall *pointer)
{
	write_intu16(stream, *pointer);
}

/**
 * Encode ObservationScanFixed
 *
 * @param *stream
 * @param *pointer
 */
void encode_observationscanfixed(ByteStreamWriter *stream,
				 ObservationScanFixed *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	encode_octet_string(stream, &pointer->obs_val_data); // octet_string obs_val_data
}

/**
 * Encode DataResponse
 *
 * @param *stream
 * @param *pointer
 */
void encode_dataresponse(ByteStreamWriter *stream, DataResponse *pointer)
{
	write_intu32(stream, pointer->rel_time_stamp);
	write_intu16(stream, pointer->data_req_result);
	write_intu16(stream, pointer->event_type);
	encode_any(stream, &pointer->event_info); // Any event_info
}

/**
 * Encode ProdSpecEntry
 *
 * @param *stream
 * @param *pointer
 */
void encode_prodspecentry(ByteStreamWriter *stream, ProdSpecEntry *pointer)
{
	write_intu16(stream, pointer->spec_type);
	write_intu16(stream, pointer->component_id);
	encode_octet_string(stream, &pointer->prod_spec); // octet_string prod_spec
}

/**
 * Encode ScaleRangeSpec16
 *
 * @param *stream
 * @param *pointer
 */
void encode_scalerangespec16(ByteStreamWriter *stream,
			     ScaleRangeSpec16 *pointer)
{
	write_float(stream, pointer->lower_absolute_value);
	write_float(stream, pointer->upper_absolute_value);
	write_intu16(stream, pointer->lower_scaled_value);
	write_intu16(stream, pointer->upper_scaled_value);
}

/**
 * Encode SegmEntryElem
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmentryelem(ByteStreamWriter *stream, SegmEntryElem *pointer)
{
	write_intu16(stream, pointer->class_id);
	encode_type(stream, &pointer->metric_type); // TYPE metric_type
	write_intu16(stream, pointer->handle);
	encode_attrvalmap(stream, &pointer->attr_val_map); // AttrValMap attr_val_map
}

/**
 * Encode ABRT_apdu
 *
 * @param *stream
 * @param *pointer
 */
void encode_abrt_apdu(ByteStreamWriter *stream, ABRT_apdu *pointer)
{
	write_intu16(stream, pointer->reason);
}

/**
 * Encode DataReqModeCapab
 *
 * @param *stream
 * @param *pointer
 */
void encode_datareqmodecapab(ByteStreamWriter *stream,
			     DataReqModeCapab *pointer)
{
	write_intu16(stream, pointer->data_req_mode_flags);
	write_intu8(stream, pointer->data_req_init_agent_count);
	write_intu8(stream, pointer->data_req_init_manager_count);
}

/**
 * Encode SupplementalTypeList
 *
 * @param *stream
 * @param *pointer
 */
void encode_supplementaltypelist(ByteStreamWriter *stream,
				 SupplementalTypeList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_type(stream, pointer->value + i);
	}

}

/**
 * Encode ObservationScanFixedList
 *
 * @param *stream
 * @param *pointer
 */
void encode_observationscanfixedlist(ByteStreamWriter *stream,
				     ObservationScanFixedList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_observationscanfixed(stream, pointer->value + i);
	}

}

/**
 * Encode TrigSegmDataXferRsp
 *
 * @param *stream
 * @param *pointer
 */
void encode_trigsegmdataxferrsp(ByteStreamWriter *stream,
				TrigSegmDataXferRsp *pointer)
{
	write_intu16(stream, pointer->seg_inst_no);
	write_intu16(stream, pointer->trig_segm_xfer_rsp);
}

/**
 * Encode DATA_apdu
 *
 * @param *stream
 * @param *pointer
 */
void encode_data_apdu(ByteStreamWriter *stream, DATA_apdu *pointer)
{
	write_intu16(stream, pointer->invoke_id);
	encode_data_apdu_message(stream, &pointer->message); // Data_apdu_message message
}

/**
 * Encode AARQ_apdu
 *
 * @param *stream
 * @param *pointer
 */
void encode_aarq_apdu(ByteStreamWriter *stream, AARQ_apdu *pointer)
{
	write_intu32(stream, pointer->assoc_version);
	encode_dataprotolist(stream, &pointer->data_proto_list); // DataProtoList data_proto_list
}

/**
 * Encode TypeVerList
 *
 * @param *stream
 * @param *pointer
 */
void encode_typeverlist(ByteStreamWriter *stream, TypeVerList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_typever(stream, pointer->value + i);
	}

}

/**
 * Encode RegCertData
 *
 * @param *stream
 * @param *pointer
 */
void encode_regcertdata(ByteStreamWriter *stream, RegCertData *pointer)
{
	encode_authbodyandstructype(stream, &pointer->auth_body_and_struc_type); // AuthBodyAndStrucType auth_body_and_struc_type
	encode_any(stream, &pointer->auth_body_data); // Any auth_body_data
}

/**
 * Encode NuObsValue
 *
 * @param *stream
 * @param *pointer
 */
void encode_nuobsvalue(ByteStreamWriter *stream, NuObsValue *pointer)
{
	write_intu16(stream, pointer->metric_id);
	write_intu16(stream, pointer->state);
	write_intu16(stream, pointer->unit_code);
	write_float(stream, pointer->value);
}

/**
 * Encode ScanReportPerVarList
 *
 * @param *stream
 * @param *pointer
 */
void encode_scanreportpervarlist(ByteStreamWriter *stream,
				 ScanReportPerVarList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_scanreportpervar(stream, pointer->value + i);
	}

}

/**
 * Encode SetTimeInvoke
 *
 * @param *stream
 * @param *pointer
 */
void encode_settimeinvoke(ByteStreamWriter *stream, SetTimeInvoke *pointer)
{
	encode_absolutetime(stream, &pointer->date_time); // AbsoluteTime date_time
	write_float(stream, pointer->accuracy);
}

/**
 * Encode SegmentInfoList
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmentinfolist(ByteStreamWriter *stream, SegmentInfoList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_segmentinfo(stream, pointer->value + i);
	}

}

/**
 * Encode ActionResultSimple
 *
 * @param *stream
 * @param *pointer
 */
void encode_actionresultsimple(ByteStreamWriter *stream,
			       ActionResultSimple *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	write_intu16(stream, pointer->action_type);
	encode_any(stream, &pointer->action_info_args); // Any action_info_args
}

/**
 * Encode SegmElemStaticAttrEntry
 *
 * @param *stream
 * @param *pointer
 */
void encode_segmelemstaticattrentry(ByteStreamWriter *stream,
				    SegmElemStaticAttrEntry *pointer)
{
	write_intu16(stream, pointer->class_id);
	encode_type(stream, &pointer->metric_type); // TYPE metric_type
	encode_attributelist(stream, &pointer->attribute_list); // AttributeList attribute_list
}

/**
 * Encode BasicNuObsValue
 *
 * @param *stream
 * @param *pointer
 */
void encode_basicnuobsvalue(ByteStreamWriter *stream,
			       BasicNuObsValue *pointer)
{
	write_intu16(stream, *pointer);
}

/**
 * Encode BasicNuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 */
void encode_basicnuobsvaluecmp(ByteStreamWriter *stream,
			       BasicNuObsValueCmp *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		write_intu16(stream, *(pointer->value + i));
	}

}

/**
 * Encode ConfigObjectList
 *
 * @param *stream
 * @param *pointer
 */
void encode_configobjectlist(ByteStreamWriter *stream,
			     ConfigObjectList *pointer)
{
	write_intu16(stream, pointer->count);
	write_intu16(stream, pointer->length);
	int i;

	for (i = 0; i < pointer->count; i++) {
		encode_configobject(stream, pointer->value + i);
	}

}

/**
 * Encode SetResultSimple
 *
 * @param *stream
 * @param *pointer
 */
void encode_setresultsimple(ByteStreamWriter *stream, SetResultSimple *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	encode_attributelist(stream, &pointer->attribute_list); // AttributeList attribute_list
}

/**
 * Encode HandleAttrValMapEntry
 *
 * @param *stream
 * @param *pointer
 */
void encode_handleattrvalmapentry(ByteStreamWriter *stream,
				  HandleAttrValMapEntry *pointer)
{
	write_intu16(stream, pointer->obj_handle);
	encode_attrvalmap(stream, &pointer->attr_val_map); // AttrValMap attr_val_map
}

/**
 * Encode SimpleNuObsValue
 *
 * @param *stream
 * @param *pointer
 */
void encode_simplenuobsvalue(ByteStreamWriter *stream,
			     SimpleNuObsValue *pointer)
{
	write_float(stream, *pointer);
}

/**
 * Encode ConfigId
 *
 * @param *stream
 * @param *pointer
 */
void encode_configid(ByteStreamWriter *stream, ConfigId *pointer)
{
	write_intu16(stream, *pointer);
}

/** @} */
