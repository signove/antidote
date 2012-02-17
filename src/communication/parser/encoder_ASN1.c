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
 * \addtogroup ASN1Codec
 *
 * @{
 */
#include <src/asn1/phd_types.h>
#include <stdlib.h>
#include "src/util/bytelib.h"
#include "src/util/log.h"
#include "src/communication/parser/encoder_ASN1.h"


/**
 * \cond Undocumented
 */
#define CHK(fff) if (! (fff)) { ERROR("%s encoding", __func__); ok = 0; }
/**
 * \endcond
 */


/**
 * Encode SegmentDataResult
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmentdataresult(ByteStreamWriter *stream,
			      SegmentDataResult *pointer)
{
	int ok = 1;
	CHK(encode_segmdataeventdescr(stream, &pointer->segm_data_event_descr));
	// SegmDataEventDescr segm_data_event_descr
	return ok;
}

/**
 * Encode ScanReportPerVar
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportpervar(ByteStreamWriter *stream,
			     ScanReportPerVar *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->person_id));
	CHK(encode_observationscanlist(stream, &pointer->obs_scan_var));
	// ObservationScanList obs_scan_var
	return ok;
}

/**
 * Encode TypeVer
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_typever(ByteStreamWriter *stream, TypeVer *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->type));
	CHK(write_intu16(stream, pointer->version));
	return ok;
}

/**
 * Encode ModificationList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_modificationlist(ByteStreamWriter *stream,
			     ModificationList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_attributemodentry(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode ProductionSpec
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_productionspec(ByteStreamWriter *stream, ProductionSpec *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_prodspecentry(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode ActionArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_actionargumentsimple(ByteStreamWriter *stream,
				 ActionArgumentSimple *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(write_intu16(stream, pointer->action_type));
	CHK(encode_any(stream, &pointer->action_info_args));
	// Any action_info_args
	return ok;
}

/**
 * Encode ScaleRangeSpec32
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scalerangespec32(ByteStreamWriter *stream,
			     ScaleRangeSpec32 *pointer)
{
	int ok = 1;
	CHK(write_float(stream, pointer->lower_absolute_value));
	CHK(write_float(stream, pointer->upper_absolute_value));
	CHK(write_intu32(stream, pointer->lower_scaled_value));
	CHK(write_intu32(stream, pointer->upper_scaled_value));
	return ok;
}

/**
 * Encode AVA_Type
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_ava_type(ByteStreamWriter *stream, AVA_Type *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->attribute_id));
	CHK(encode_any(stream, &pointer->attribute_value));
	// Any attribute_value
	return ok;
}

/**
 * Encode ConfigReport
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_configreport(ByteStreamWriter *stream, ConfigReport *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->config_report_id));
	CHK(encode_configobjectlist(stream, &pointer->config_obj_list));
	// ConfigObjectList config_obj_list
	return ok;
}

/**
 * Encode AttrValMapEntry
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_attrvalmapentry(ByteStreamWriter *stream, AttrValMapEntry *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->attribute_id));
	CHK(write_intu16(stream, pointer->attribute_len));
	return ok;
}

/**
 * Encode AbsoluteTime
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_absolutetime(ByteStreamWriter *stream, AbsoluteTime *pointer)
{
	int ok = 1;
	CHK(write_intu8(stream, pointer->century));
	CHK(write_intu8(stream, pointer->year));
	CHK(write_intu8(stream, pointer->month));
	CHK(write_intu8(stream, pointer->day));
	CHK(write_intu8(stream, pointer->hour));
	CHK(write_intu8(stream, pointer->minute));
	CHK(write_intu8(stream, pointer->second));
	CHK(write_intu8(stream, pointer->sec_fractions));
	return ok;
}

/**
 * Encode NuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_nuobsvaluecmp(ByteStreamWriter *stream, NuObsValueCmp *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_nuobsvalue(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode ScanReportInfoMPFixed
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportinfompfixed(ByteStreamWriter *stream,
				  ScanReportInfoMPFixed *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_scanreportperfixedlist(stream, &pointer->scan_per_fixed));
	// ScanReportPerFixedList scan_per_fixed
	return ok;
}

/**
 * Encode RejectResult
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_rejectresult(ByteStreamWriter *stream, RejectResult *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->problem));
	return ok;
}

/**
 * Encode ManufSpecAssociationInformation
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_manufspecassociationinformation(ByteStreamWriter *stream,
		ManufSpecAssociationInformation *pointer)
{
	int ok = 1;
	CHK(encode_uuid_ident(stream, &pointer->data_proto_id_ext));
	// UUID_Ident data_proto_id_ext
	CHK(encode_any(stream, &pointer->data_proto_info_ext));
	// Any data_proto_info_ext
	return ok;
}

/**
 * Encode EnumObsValue
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_enumobsvalue(ByteStreamWriter *stream, EnumObsValue *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->metric_id));
	CHK(write_intu16(stream, pointer->state));
	CHK(encode_enumval(stream, &pointer->value));
	// EnumVal value
	return ok;
}

/**
 * Encode octet_string
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_octet_string(ByteStreamWriter *stream, octet_string *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->length));
	CHK(write_intu8_many(stream, pointer->value, pointer->length));
	return ok;
}

/**
 * Encode HighResRelativeTime
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_highresrelativetime(ByteStreamWriter *stream,
				HighResRelativeTime *pointer)
{
	int ok = 1;
	// intu8 value[8];
	CHK(write_intu8_many(stream, pointer->value, 8));
	return ok;
}

/**
 * Encode SampleType
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_sampletype(ByteStreamWriter *stream, SampleType *pointer)
{
	int ok = 1;
	CHK(write_intu8(stream, pointer->sample_size));
	CHK(write_intu8(stream, pointer->significant_bits));
	return ok;
}

/**
 * Encode AttributeList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_attributelist(ByteStreamWriter *stream, AttributeList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_ava_type(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode SegmIdList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmidlist(ByteStreamWriter *stream, SegmIdList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(write_intu16(stream, *(pointer->value + i)));
	}

	return ok;
}

/**
 * Encode SimpleNuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_simplenuobsvaluecmp(ByteStreamWriter *stream,
				SimpleNuObsValueCmp *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(write_float(stream, *(pointer->value + i)));
	}
	return ok;
}

/**
 * Encode GetResultSimple
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_getresultsimple(ByteStreamWriter *stream, GetResultSimple *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attributelist(stream, &pointer->attribute_list));
	// AttributeList attribute_list
	return ok;
}

/**
 * Encode HANDLEList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_handlelist(ByteStreamWriter *stream, HANDLEList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(write_intu16(stream, *(pointer->value + i)));
	}

	return ok;
}

/**
 * Encode SegmDataEventDescr
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmdataeventdescr(ByteStreamWriter *stream,
			       SegmDataEventDescr *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->segm_instance));
	CHK(write_intu32(stream, pointer->segm_evt_entry_index));
	CHK(write_intu32(stream, pointer->segm_evt_entry_count));
	CHK(write_intu16(stream, pointer->segm_evt_status));
	return ok;
}

/**
 * Encode AttrValMap
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_attrvalmap(ByteStreamWriter *stream, AttrValMap *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_attrvalmapentry(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode ScaleRangeSpec8
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scalerangespec8(ByteStreamWriter *stream, ScaleRangeSpec8 *pointer)
{
	int ok = 1;
	CHK(write_float(stream, pointer->lower_absolute_value));
	CHK(write_float(stream, pointer->upper_absolute_value));
	CHK(write_intu8(stream, pointer->lower_scaled_value));
	CHK(write_intu8(stream, pointer->upper_scaled_value));
	return ok;
}

/**
 * Encode PhdAssociationInformation
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_phdassociationinformation(ByteStreamWriter *stream,
				      PhdAssociationInformation *pointer)
{
	int ok = 1;
	CHK(write_intu32(stream, pointer->protocolVersion));
	CHK(write_intu16(stream, pointer->encodingRules));
	CHK(write_intu32(stream, pointer->nomenclatureVersion));
	CHK(write_intu32(stream, pointer->functionalUnits));
	CHK(write_intu32(stream, pointer->systemType));
	CHK(encode_octet_string(stream, &pointer->system_id));
	// octet_string system_id
	CHK(write_intu16(stream, pointer->dev_config_id));
	CHK(encode_datareqmodecapab(stream, &pointer->data_req_mode_capab));
	// DataReqModeCapab data_req_mode_capab
	CHK(encode_attributelist(stream, &pointer->optionList));
	// AttributeList optionList
	return ok;
}

/**
 * Encode ScanReportPerFixedList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportperfixedlist(ByteStreamWriter *stream,
				   ScanReportPerFixedList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_scanreportperfixed(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode ScanReportPerGroupedList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportpergroupedlist(ByteStreamWriter *stream,
				     ScanReportPerGroupedList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_scanreportpergrouped(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode DataProtoList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_dataprotolist(ByteStreamWriter *stream, DataProtoList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_dataproto(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode SegmSelection
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmselection(ByteStreamWriter *stream, SegmSelection *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->choice));
	CHK(write_intu16(stream, pointer->length));

	switch (pointer->choice) {
	case ALL_SEGMENTS_CHOSEN:
		CHK(write_intu16(stream, pointer->u.all_segments));
		break;
	case SEGM_ID_LIST_CHOSEN:
		CHK(encode_segmidlist(stream, &pointer->u.segm_id_list));
		break;
	case ABS_TIME_RANGE_CHOSEN:
		CHK(encode_abstimerange(stream, &pointer->u.abs_time_range));
		break;
	default:
		// TODO: manage errors
		break;
	}
	return ok;
}

/**
 * Encode ErrorResult
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_errorresult(ByteStreamWriter *stream, ErrorResult *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->error_value));
	CHK(encode_any(stream, &pointer->parameter));
	// Any parameter
	return ok;
}

/**
 * Encode HandleAttrValMap
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_handleattrvalmap(ByteStreamWriter *stream,
			     HandleAttrValMap *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_handleattrvalmapentry(stream, pointer->value + i));
	}
	return ok;
}

/**
 * Encode AbsoluteTimeAdjust
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_absolutetimeadjust(ByteStreamWriter *stream,
			       AbsoluteTimeAdjust *pointer)
{
	int ok = 1;
	// intu8 value[6]
	CHK(write_intu8_many(stream, pointer->value, 6));

	return ok;
}

/**
 * Encode AARE_apdu
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_aare_apdu(ByteStreamWriter *stream, AARE_apdu *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->result));
	CHK(encode_dataproto(stream, &pointer->selected_data_proto));
	// DataProto selected_data_proto
	return ok;
}

/**
 * Encode RLRE_apdu
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_rlre_apdu(ByteStreamWriter *stream, RLRE_apdu *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->reason));
	return ok;
}

/**
 * Encode MetricIdList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_metricidlist(ByteStreamWriter *stream, MetricIdList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(write_intu16(stream, *(pointer->value + i)));
	}

	return ok;
}

/**
 * Encode ScanReportPerFixed
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportperfixed(ByteStreamWriter *stream,
			       ScanReportPerFixed *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->person_id));
	CHK(encode_observationscanfixedlist(stream, &pointer->obs_scan_fix));
	// ObservationScanFixedList obs_scan_fix
	return ok;
}

/**
 * Encode ScanReportInfoGrouped
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportinfogrouped(ByteStreamWriter *stream,
				  ScanReportInfoGrouped *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_scanreportinfogroupedlist(stream, &pointer->obs_scan_grouped));
	// ScanReportInfoGroupedList obs_scan_grouped
	return ok;
}

/**
 * Encode ObservationScan
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_observationscan(ByteStreamWriter *stream, ObservationScan *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attributelist(stream, &pointer->attributes));
	// AttributeList attributes
	return ok;
}

/**
 * Encode ScanReportPerGrouped
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportpergrouped(ByteStreamWriter *stream,
				 ScanReportPerGrouped *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->person_id));
	CHK(encode_octet_string(stream, &pointer->obs_scan_grouped));
	return ok;
}

/**
 * Encode SystemModel
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_systemmodel(ByteStreamWriter *stream, SystemModel *pointer)
{
	int ok = 1;
	CHK(encode_octet_string(stream, &pointer->manufacturer));
	// octet_string manufacturer
	CHK(encode_octet_string(stream, &pointer->model_number));
	// octet_string model_number
	return ok;
}

/**
 * Encode ObservationScanList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_observationscanlist(ByteStreamWriter *stream,
				ObservationScanList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_observationscan(stream, pointer->value + i));
	}
	return ok;
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
 * @return 1 if ok, 0 if error
 */
int encode_set_data_apdu(PRST_apdu *prst, DATA_apdu *data_apdu)
{
	int ok = 1;
	prst->value = (void *) data_apdu;
	return ok;
}

/**
 * Get Data_apdu from PRST_apdu definition, see encode_set_data_apdu().
 *
 * @param prst
 * @return data_apdu
 * @return 1 if ok, 0 if error
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
 * @return 1 if ok, 0 if error
 */
int encode_apdu(ByteStreamWriter *stream, APDU *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->choice));
	CHK(write_intu16(stream, pointer->length));

	switch (pointer->choice) {
	case AARQ_CHOSEN:
		CHK(encode_aarq_apdu(stream, &pointer->u.aarq));
		break;
	case AARE_CHOSEN:
		CHK(encode_aare_apdu(stream, &pointer->u.aare));
		break;
	case RLRQ_CHOSEN:
		CHK(encode_rlrq_apdu(stream, &pointer->u.rlrq));
		break;
	case RLRE_CHOSEN:
		CHK(encode_rlre_apdu(stream, &pointer->u.rlre));
		break;
	case ABRT_CHOSEN:
		CHK(encode_abrt_apdu(stream, &pointer->u.abrt));
		break;
	case PRST_CHOSEN:
		CHK(encode_prst_apdu(stream, &pointer->u.prst));
		break;
	default:
		// TODO: manage errors
		break;
	}
	return ok;
}

/**
 * Encode PRST_apdu
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_prst_apdu(ByteStreamWriter *stream, PRST_apdu *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->length));

	if (pointer->value != NULL) {
		CHK(encode_data_apdu(stream, encode_get_data_apdu(pointer)));
	} else {
		ERROR("encode_prst_apdu pointer value");
		ok = 0;
	}
	return ok;
}

/**
 * Encode PmSegmentEntryMap
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_pmsegmententrymap(ByteStreamWriter *stream,
			      PmSegmentEntryMap *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->segm_entry_header));
	CHK(encode_segmentryelemlist(stream, &pointer->segm_entry_elem_list));
	// SegmEntryElemList segm_entry_elem_list
	return ok;
}

/**
 * Encode Any
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_any(ByteStreamWriter *stream, Any *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->length));
	CHK(write_intu8_many(stream, pointer->value, pointer->length));
	return ok;
}

/**
 * Encode SetArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_setargumentsimple(ByteStreamWriter *stream,
			      SetArgumentSimple *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_modificationlist(stream, &pointer->modification_list));
	// ModificationList modification_list
	return ok;
}

/**
 * Encode SegmentInfo
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmentinfo(ByteStreamWriter *stream, SegmentInfo *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->seg_inst_no));
	CHK(encode_attributelist(stream, &pointer->seg_info));
	// AttributeList seg_info
	return ok;
}

/**
 * Encode PmSegmElemStaticAttrList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_pmsegmelemstaticattrlist(ByteStreamWriter *stream,
				     PmSegmElemStaticAttrList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_segmelemstaticattrentry(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode AbsTimeRange
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_abstimerange(ByteStreamWriter *stream, AbsTimeRange *pointer)
{
	int ok = 1;
	CHK(encode_absolutetime(stream, &pointer->from_time));
	// AbsoluteTime from_time
	CHK(encode_absolutetime(stream, &pointer->to_time));
	// AbsoluteTime to_time
	return ok;
}

/**
 * Encode ScanReportInfoMPVar
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportinfompvar(ByteStreamWriter *stream,
				ScanReportInfoMPVar *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_scanreportpervarlist(stream, &pointer->scan_per_var));
	// ScanReportPerVarList scan_per_var
	return ok;
}

/**
 * Encode UUID_Ident
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_uuid_ident(ByteStreamWriter *stream, UUID_Ident *pointer)
{
	int ok = 1;
	// intu8 value[16]
	CHK(write_intu8_many(stream, pointer->value, 16));
	return ok;
}

/**
 * Encode GetArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_getargumentsimple(ByteStreamWriter *stream,
			      GetArgumentSimple *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attributeidlist(stream, &pointer->attribute_id_list));
	// AttributeIdList attribute_id_list
	return ok;
}

/**
 * Encode RegCertDataList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_regcertdatalist(ByteStreamWriter *stream, RegCertDataList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_regcertdata(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode ConfigReportRsp
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_configreportrsp(ByteStreamWriter *stream, ConfigReportRsp *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->config_report_id));
	CHK(write_intu16(stream, pointer->config_result));
	return ok;
}

/**
 * Encode DataProto
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_dataproto(ByteStreamWriter *stream, DataProto *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->data_proto_id));
	CHK(encode_any(stream, &pointer->data_proto_info));
	// Any data_proto_info
	return ok;
}

/**
 * Encode MetricStructureSmall
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_metricstructuresmall(ByteStreamWriter *stream,
				 MetricStructureSmall *pointer)
{
	int ok = 1;
	CHK(write_intu8(stream, pointer->ms_struct));
	CHK(write_intu8(stream, pointer->ms_comp_no));
	return ok;
}

/**
 * Encode SegmentStatisticEntry
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmentstatisticentry(ByteStreamWriter *stream,
				  SegmentStatisticEntry *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->segm_stat_type));
	CHK(encode_octet_string(stream, &pointer->segm_stat_entry));
	// octet_string segm_stat_entry
	return ok;
}

/**
 * Encode SegmentDataEvent
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmentdataevent(ByteStreamWriter *stream,
			     SegmentDataEvent *pointer)
{
	int ok = 1;
	CHK(encode_segmdataeventdescr(stream, &pointer->segm_data_event_descr));
	// SegmDataEventDescr segm_data_event_descr
	CHK(encode_octet_string(stream, &pointer->segm_data_event_entries));
	// octet_string segm_data_event_entries
	return ok;
}

/**
 * Encode SegmEntryElemList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmentryelemlist(ByteStreamWriter *stream,
			      SegmEntryElemList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_segmentryelem(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode SaSpec
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_saspec(ByteStreamWriter *stream, SaSpec *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->array_size));
	CHK(encode_sampletype(stream, &pointer->sample_type));
	// SampleType sample_type
	CHK(write_intu16(stream, pointer->flags));
	return ok;
}

/**
 * Encode AttributeModEntry
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_attributemodentry(ByteStreamWriter *stream,
			      AttributeModEntry *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->modify_operator));
	CHK(encode_ava_type(stream, &pointer->attribute));
	// AVA_Type attribute
	return ok;
}

/**
 * Encode MdsTimeInfo
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_mdstimeinfo(ByteStreamWriter *stream, MdsTimeInfo *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->mds_time_cap_state));
	CHK(write_intu16(stream, pointer->time_sync_protocol));
	CHK(write_intu32(stream, pointer->time_sync_accuracy));
	CHK(write_intu16(stream, pointer->time_resolution_abs_time));
	CHK(write_intu16(stream, pointer->time_resolution_rel_time));
	CHK(write_intu32(stream, pointer->time_resolution_high_res_time));
	return ok;
}

/**
 * Encode EnumVal
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_enumval(ByteStreamWriter *stream, EnumVal *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->choice));
	CHK(write_intu16(stream, pointer->length));

	switch (pointer->choice) {
	case OBJ_ID_CHOSEN:
		CHK(write_intu16(stream, pointer->u.enum_obj_id));
		break;
	case TEXT_STRING_CHOSEN:
		CHK(encode_octet_string(stream, &pointer->u.enum_text_string));
		break;
	case BIT_STR_CHOSEN:
		CHK(write_intu32(stream, pointer->u.enum_bit_str));
		break;
	default:
		break;
	}
	return ok;
}

/**
 * Encode TrigSegmDataXferReq
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_trigsegmdataxferreq(ByteStreamWriter *stream,
				TrigSegmDataXferReq *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->seg_inst_no));
	return ok;
}

/**
 * Encode BatMeasure
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_batmeasure(ByteStreamWriter *stream, BatMeasure *pointer)
{
	int ok = 1;
	CHK(write_float(stream, pointer->value));
	CHK(write_intu16(stream, pointer->unit));
	return ok;
}

/**
 * Encode SegmentStatistics
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmentstatistics(ByteStreamWriter *stream,
			      SegmentStatistics *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_segmentstatisticentry(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode AttributeIdList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_attributeidlist(ByteStreamWriter *stream, AttributeIdList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(write_intu16(stream, *(pointer->value + i)));
	}

	return ok;
}

/**
 * Encode ScanReportInfoFixed
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportinfofixed(ByteStreamWriter *stream,
				ScanReportInfoFixed *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_observationscanfixedlist(stream, &pointer->obs_scan_fixed));
	// ObservationScanFixedList obs_scan_fixed
	return ok;
}

/**
 * Encode DataRequest
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_datarequest(ByteStreamWriter *stream, DataRequest *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->data_req_mode));
	CHK(write_intu32(stream, pointer->data_req_time));
	CHK(write_intu16(stream, pointer->data_req_person_id));
	CHK(write_intu16(stream, pointer->data_req_class));
	CHK(encode_handlelist(stream, &pointer->data_req_obj_handle_list));
	// HANDLEList data_req_obj_handle_list
	return ok;
}

/**
 * Encode AuthBodyAndStrucType
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_authbodyandstructype(ByteStreamWriter *stream,
				 AuthBodyAndStrucType *pointer)
{
	int ok = 1;
	CHK(write_intu8(stream, pointer->auth_body));
	CHK(write_intu8(stream, pointer->auth_body_struc_type));
	return ok;
}

/**
 * Encode RLRQ_apdu
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_rlrq_apdu(ByteStreamWriter *stream, RLRQ_apdu *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->reason));
	return ok;
}

/**
 * Encode Data_apdu_message
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_data_apdu_message(ByteStreamWriter *stream,
			      Data_apdu_message *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->choice));
	CHK(write_intu16(stream, pointer->length));

	switch (pointer->choice) {
	case ROIV_CMIP_EVENT_REPORT_CHOSEN:
		CHK(encode_eventreportargumentsimple(stream,
						 &pointer->u.roiv_cmipEventReport));
		break;
	case ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		CHK(encode_eventreportargumentsimple(stream,
						 &pointer->u.roiv_cmipConfirmedEventReport));
		break;
	case ROIV_CMIP_GET_CHOSEN:
		CHK(encode_getargumentsimple(stream, &pointer->u.roiv_cmipGet));
		break;
	case ROIV_CMIP_SET_CHOSEN:
		CHK(encode_setargumentsimple(stream, &pointer->u.roiv_cmipSet));
		break;
	case ROIV_CMIP_CONFIRMED_SET_CHOSEN:
		CHK(encode_setargumentsimple(stream,
					 &pointer->u.roiv_cmipConfirmedSet));
		break;
	case ROIV_CMIP_ACTION_CHOSEN:
		CHK(encode_actionargumentsimple(stream, &pointer->u.roiv_cmipAction));
		break;
	case ROIV_CMIP_CONFIRMED_ACTION_CHOSEN:
		CHK(encode_actionargumentsimple(stream,
					    &pointer->u.roiv_cmipConfirmedAction));
		break;
	case RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		CHK(encode_eventreportresultsimple(stream,
					       &pointer->u.rors_cmipConfirmedEventReport));
		break;
	case RORS_CMIP_GET_CHOSEN:
		CHK(encode_getresultsimple(stream, &pointer->u.rors_cmipGet));
		break;
	case RORS_CMIP_CONFIRMED_SET_CHOSEN:
		CHK(encode_setresultsimple(stream,
				       &pointer->u.rors_cmipConfirmedSet));
		break;
	case RORS_CMIP_CONFIRMED_ACTION_CHOSEN:
		CHK(encode_actionresultsimple(stream,
					  &pointer->u.rors_cmipConfirmedAction));
		break;
	case ROER_CHOSEN:
		CHK(encode_errorresult(stream, &pointer->u.roer));
		break;
	case RORJ_CHOSEN:
		CHK(encode_rejectresult(stream, &pointer->u.rorj));
		break;
	default:
		ERROR("encoding: Data apdu type uknown");
		ok = 0;
		break;
	}
	return ok;
}

/**
 * Encode EventReportArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_eventreportargumentsimple(ByteStreamWriter *stream,
				      EventReportArgumentSimple *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(write_intu32(stream, pointer->event_time));
	CHK(write_intu16(stream, pointer->event_type));
	CHK(encode_any(stream, &pointer->event_info));
	// Any event_info
	return ok;
}

/**
 * Encode ScanReportInfoVar
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportinfovar(ByteStreamWriter *stream,
			      ScanReportInfoVar *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_observationscanlist(stream, &pointer->obs_scan_var));
	// ObservationScanList obs_scan_var
	return ok;
}

/**
 * Encode ScanReportInfoMPGrouped
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportinfompgrouped(ByteStreamWriter *stream,
				    ScanReportInfoMPGrouped *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_scanreportpergroupedlist(stream, &pointer->scan_per_grouped));
	// ScanReportPerGroupedList scan_per_grouped
	return ok;
}

/**
 * Encode ConfigObject
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_configobject(ByteStreamWriter *stream, ConfigObject *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_class));
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attributelist(stream, &pointer->attributes));
	// AttributeList attributes
	return ok;
}

/**
 * Encode ScanReportInfoGroupedList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportinfogroupedlist(ByteStreamWriter *stream,
				      ScanReportInfoGroupedList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));

	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_octet_string(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode EventReportResultSimple
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_eventreportresultsimple(ByteStreamWriter *stream,
				    EventReportResultSimple *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(write_intu32(stream, pointer->currentTime));
	CHK(write_intu16(stream, pointer->event_type));
	CHK(encode_any(stream, &pointer->event_reply_info));
	// Any event_reply_info
	return ok;
}

/**
 * Encode TYPE
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_type(ByteStreamWriter *stream, TYPE *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->partition));
	CHK(write_intu16(stream, pointer->code));
	return ok;
}

/**
 * Encode MetricSpecSmall
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_metricspecsmall(ByteStreamWriter *stream, MetricSpecSmall *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, *pointer));
	return ok;
}

/**
 * Encode ObservationScanFixed
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_observationscanfixed(ByteStreamWriter *stream,
				 ObservationScanFixed *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_octet_string(stream, &pointer->obs_val_data));
	// octet_string obs_val_data
	return ok;
}

/**
 * Encode DataResponse
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_dataresponse(ByteStreamWriter *stream, DataResponse *pointer)
{
	int ok = 1;
	CHK(write_intu32(stream, pointer->rel_time_stamp));
	CHK(write_intu16(stream, pointer->data_req_result));
	CHK(write_intu16(stream, pointer->event_type));
	CHK(encode_any(stream, &pointer->event_info));
	// Any event_info
	return ok;
}

/**
 * Encode ProdSpecEntry
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_prodspecentry(ByteStreamWriter *stream, ProdSpecEntry *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->spec_type));
	CHK(write_intu16(stream, pointer->component_id));
	CHK(encode_octet_string(stream, &pointer->prod_spec));
	// octet_string prod_spec
	return ok;
}

/**
 * Encode ScaleRangeSpec16
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scalerangespec16(ByteStreamWriter *stream,
			     ScaleRangeSpec16 *pointer)
{
	int ok = 1;
	CHK(write_float(stream, pointer->lower_absolute_value));
	CHK(write_float(stream, pointer->upper_absolute_value));
	CHK(write_intu16(stream, pointer->lower_scaled_value));
	CHK(write_intu16(stream, pointer->upper_scaled_value));
	return ok;
}

/**
 * Encode SegmEntryElem
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmentryelem(ByteStreamWriter *stream, SegmEntryElem *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->class_id));
	CHK(encode_type(stream, &pointer->metric_type));
	// TYPE metric_type
	CHK(write_intu16(stream, pointer->handle));
	CHK(encode_attrvalmap(stream, &pointer->attr_val_map));
	// AttrValMap attr_val_map
	return ok;
}

/**
 * Encode ABRT_apdu
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_abrt_apdu(ByteStreamWriter *stream, ABRT_apdu *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->reason));
	return ok;
}

/**
 * Encode DataReqModeCapab
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_datareqmodecapab(ByteStreamWriter *stream,
			     DataReqModeCapab *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->data_req_mode_flags));
	CHK(write_intu8(stream, pointer->data_req_init_agent_count));
	CHK(write_intu8(stream, pointer->data_req_init_manager_count));
	return ok;
}

/**
 * Encode SupplementalTypeList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_supplementaltypelist(ByteStreamWriter *stream,
				 SupplementalTypeList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_type(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode ObservationScanFixedList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_observationscanfixedlist(ByteStreamWriter *stream,
				     ObservationScanFixedList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_observationscanfixed(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode TrigSegmDataXferRsp
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_trigsegmdataxferrsp(ByteStreamWriter *stream,
				TrigSegmDataXferRsp *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->seg_inst_no));
	CHK(write_intu16(stream, pointer->trig_segm_xfer_rsp));
	return ok;
}

/**
 * Encode DATA_apdu
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_data_apdu(ByteStreamWriter *stream, DATA_apdu *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->invoke_id));
	CHK(encode_data_apdu_message(stream, &pointer->message));
	// Data_apdu_message message
	return ok;
}

/**
 * Encode AARQ_apdu
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_aarq_apdu(ByteStreamWriter *stream, AARQ_apdu *pointer)
{
	int ok = 1;
	CHK(write_intu32(stream, pointer->assoc_version));
	CHK(encode_dataprotolist(stream, &pointer->data_proto_list));
	// DataProtoList data_proto_list
	return ok;
}

/**
 * Encode TypeVerList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_typeverlist(ByteStreamWriter *stream, TypeVerList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_typever(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode RegCertData
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_regcertdata(ByteStreamWriter *stream, RegCertData *pointer)
{
	int ok = 1;
	CHK(encode_authbodyandstructype(stream, &pointer->auth_body_and_struc_type));
	// AuthBodyAndStrucType auth_body_and_struc_type
	CHK(encode_any(stream, &pointer->auth_body_data));
	// Any auth_body_data
	return ok;
}

/**
 * Encode NuObsValue
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_nuobsvalue(ByteStreamWriter *stream, NuObsValue *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->metric_id));
	CHK(write_intu16(stream, pointer->state));
	CHK(write_intu16(stream, pointer->unit_code));
	CHK(write_float(stream, pointer->value));
	return ok;
}

/**
 * Encode ScanReportPerVarList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_scanreportpervarlist(ByteStreamWriter *stream,
				 ScanReportPerVarList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_scanreportpervar(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode SetTimeInvoke
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_settimeinvoke(ByteStreamWriter *stream, SetTimeInvoke *pointer)
{
	int ok = 1;
	CHK(encode_absolutetime(stream, &pointer->date_time));
	// AbsoluteTime date_time
	CHK(write_float(stream, pointer->accuracy));
	return ok;
}

/**
 * Encode SegmentInfoList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmentinfolist(ByteStreamWriter *stream, SegmentInfoList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_segmentinfo(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode ActionResultSimple
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_actionresultsimple(ByteStreamWriter *stream,
			       ActionResultSimple *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(write_intu16(stream, pointer->action_type));
	CHK(encode_any(stream, &pointer->action_info_args));
	// Any action_info_args
	return ok;
}

/**
 * Encode SegmElemStaticAttrEntry
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_segmelemstaticattrentry(ByteStreamWriter *stream,
				    SegmElemStaticAttrEntry *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->class_id));
	CHK(encode_type(stream, &pointer->metric_type));
	// TYPE metric_type
	CHK(encode_attributelist(stream, &pointer->attribute_list));
	// AttributeList attribute_list
	return ok;
}

/**
 * Encode BasicNuObsValue
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_basicnuobsvalue(ByteStreamWriter *stream,
			       BasicNuObsValue *pointer)
{
	int ok = 1;
	CHK(write_sfloat(stream, *pointer));
	return ok;
}

/**
 * Encode BasicNuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_basicnuobsvaluecmp(ByteStreamWriter *stream,
			       BasicNuObsValueCmp *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(write_intu16(stream, *(pointer->value + i)));
	}

	return ok;
}

/**
 * Encode ConfigObjectList
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_configobjectlist(ByteStreamWriter *stream,
			     ConfigObjectList *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->count));
	CHK(write_intu16(stream, pointer->length));
	int i;

	for (i = 0; i < pointer->count; i++) {
		CHK(encode_configobject(stream, pointer->value + i));
	}

	return ok;
}

/**
 * Encode SetResultSimple
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_setresultsimple(ByteStreamWriter *stream, SetResultSimple *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attributelist(stream, &pointer->attribute_list));
	// AttributeList attribute_list
	return ok;
}

/**
 * Encode HandleAttrValMapEntry
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_handleattrvalmapentry(ByteStreamWriter *stream,
				  HandleAttrValMapEntry *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attrvalmap(stream, &pointer->attr_val_map));
	// AttrValMap attr_val_map
	return ok;
}

/**
 * Encode SimpleNuObsValue
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_simplenuobsvalue(ByteStreamWriter *stream,
			     SimpleNuObsValue *pointer)
{
	int ok = 1;
	CHK(write_float(stream, *pointer));
	return ok;
}

/**
 * Encode ConfigId
 *
 * @param *stream
 * @param *pointer
 * @return 1 if ok, 0 if error
 */
int encode_configid(ByteStreamWriter *stream, ConfigId *pointer)
{
	int ok = 1;
	CHK(write_intu16(stream, *pointer));
	return ok;
}

/** @} */
