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
#include "src/util/log.h"
#include "src/communication/parser/encoder_ASN1.h"

/**
 * \cond Undocumented
 */

#ifdef WIN32
#define __func__ __FUNCTION__
#endif

#define PROLOGUE()			\
	int octets = 0;

#define WRITE_STRING(len) 								\
	{										\
		int error = 0;								\
		int count = write_intu8_many(stream, pointer->value, len, &error);	\
		if (error) {								\
			ERROR("%s encoding string", __func__);				\
			return 0;							\
		}									\
		octets += count;							\
	}

#define WRITE_STRING_WITH_LENGTH()			\
	CHK(write_intu16(stream, pointer->length));	\
	WRITE_STRING(pointer->length);

#define CHK(fff) \
	{						\
		int count = (fff);			\
		if (! count) {				\
			ERROR("%s encoding", __func__);	\
			return 0;			\
		}					\
		octets += count;				\
	}

#define COUNT()						\
	CHK(write_intu16(stream, pointer->count));

#define RESERVE_LENGTH()				\
	int length_position;				\
	int header_octets;				\
	pointer->length = 0;				\
	CHK(reserve_intu16(stream, &length_position));	\
	header_octets = octets;	

#define CL_PROLOGUE()					\
	PROLOGUE();					\
	COUNT();					\
	RESERVE_LENGTH();

#define L_EPILOGUE()							\
	pointer->length = octets - header_octets;			\
	commit_intu16(stream, length_position, pointer->length);	\
	EPILOGUE();

#define EPILOGUE()			\
	return octets;

#define CHILDREN_G(encodefunction)					\
	int i;								\
	for (i = 0; i < pointer->count; i++) {				\
		CHK(encodefunction(stream, pointer->value + i));	\
	}

#define CHILDREN(type)	CHILDREN_G(encode_##type)

#define CHILDREN_16()							\
	int i;								\
	for (i = 0; i < pointer->count; i++) {				\
		CHK(write_intu16(stream, *(pointer->value + i)));	\
	}

#define CHILDREN_32()							\
	int i;								\
	for (i = 0; i < pointer->count; i++) {				\
		CHK(write_intu32(stream, *(pointer->value + i)));	\
	}

#define CHILDREN_FLOAT()						\
	int i;								\
	for (i = 0; i < pointer->count; i++) {				\
		CHK(write_float(stream, *(pointer->value + i)));	\
	}

#define CHILDREN_SFLOAT()						\
	int i;								\
	for (i = 0; i < pointer->count; i++) {				\
		CHK(write_sfloat(stream, *(pointer->value + i)));	\
	}

/**
 * \endcond
 */

/**
 * Encode SegmentDataResult
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmentdataresult(ByteStreamWriter *stream,
			      SegmentDataResult *pointer)
{
	PROLOGUE();
	CHK(encode_segmdataeventdescr(stream, &pointer->segm_data_event_descr));
	// SegmDataEventDescr segm_data_event_descr
	EPILOGUE();
}

/**
 * Encode ScanReportPerVar
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportpervar(ByteStreamWriter *stream,
			     ScanReportPerVar *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->person_id));
	CHK(encode_observationscanlist(stream, &pointer->obs_scan_var));
	// ObservationScanList obs_scan_var
	EPILOGUE();
}

/**
 * Encode TypeVer
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_typever(ByteStreamWriter *stream, TypeVer *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->type));
	CHK(write_intu16(stream, pointer->version));
	EPILOGUE();
}

/**
 * Encode ModificationList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_modificationlist(ByteStreamWriter *stream,
			     ModificationList *pointer)
{
	CL_PROLOGUE();

	CHILDREN(attributemodentry);

	L_EPILOGUE();
}

/**
 * Encode ProductionSpec
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_productionspec(ByteStreamWriter *stream, ProductionSpec *pointer)
{
	CL_PROLOGUE();

	CHILDREN(prodspecentry);

	L_EPILOGUE();
}

/**
 * Encode ActionArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_actionargumentsimple(ByteStreamWriter *stream,
				 ActionArgumentSimple *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(write_intu16(stream, pointer->action_type));
	CHK(encode_any(stream, &pointer->action_info_args));
	// Any action_info_args
	EPILOGUE();
}

/**
 * Encode ScaleRangeSpec32
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scalerangespec32(ByteStreamWriter *stream,
			     ScaleRangeSpec32 *pointer)
{
	PROLOGUE();
	CHK(write_float(stream, pointer->lower_absolute_value));
	CHK(write_float(stream, pointer->upper_absolute_value));
	CHK(write_intu32(stream, pointer->lower_scaled_value));
	CHK(write_intu32(stream, pointer->upper_scaled_value));
	EPILOGUE();
}

/**
 * Encode AVA_Type
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_ava_type(ByteStreamWriter *stream, AVA_Type *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->attribute_id));
	CHK(encode_any(stream, &pointer->attribute_value));
	// Any attribute_value
	EPILOGUE();
}

/**
 * Encode ConfigReport
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_configreport(ByteStreamWriter *stream, ConfigReport *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->config_report_id));
	CHK(encode_configobjectlist(stream, &pointer->config_obj_list));
	// ConfigObjectList config_obj_list
	EPILOGUE();
}

/**
 * Encode AttrValMapEntry
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_attrvalmapentry(ByteStreamWriter *stream, AttrValMapEntry *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->attribute_id));
	CHK(write_intu16(stream, pointer->attribute_len));
	EPILOGUE();
}

/**
 * Encode AbsoluteTime
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_absolutetime(ByteStreamWriter *stream, AbsoluteTime *pointer)
{
	PROLOGUE();
	CHK(write_intu8(stream, pointer->century));
	CHK(write_intu8(stream, pointer->year));
	CHK(write_intu8(stream, pointer->month));
	CHK(write_intu8(stream, pointer->day));
	CHK(write_intu8(stream, pointer->hour));
	CHK(write_intu8(stream, pointer->minute));
	CHK(write_intu8(stream, pointer->second));
	CHK(write_intu8(stream, pointer->sec_fractions));
	EPILOGUE();
}

/**
 * Encode NuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_nuobsvaluecmp(ByteStreamWriter *stream, NuObsValueCmp *pointer)
{
	CL_PROLOGUE();

	CHILDREN(nuobsvalue);

	L_EPILOGUE();
}

/**
 * Encode ScanReportInfoMPFixed
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportinfompfixed(ByteStreamWriter *stream,
				  ScanReportInfoMPFixed *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_scanreportperfixedlist(stream, &pointer->scan_per_fixed));
	// ScanReportPerFixedList scan_per_fixed
	EPILOGUE();
}

/**
 * Encode RejectResult
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_rejectresult(ByteStreamWriter *stream, RejectResult *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->problem));
	EPILOGUE();
}

/**
 * Encode ManufSpecAssociationInformation
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_manufspecassociationinformation(ByteStreamWriter *stream,
		ManufSpecAssociationInformation *pointer)
{
	PROLOGUE();
	CHK(encode_uuid_ident(stream, &pointer->data_proto_id_ext));
	// UUID_Ident data_proto_id_ext
	CHK(encode_any(stream, &pointer->data_proto_info_ext));
	// Any data_proto_info_ext
	EPILOGUE();
}

/**
 * Encode EnumObsValue
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_enumobsvalue(ByteStreamWriter *stream, EnumObsValue *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->metric_id));
	CHK(write_intu16(stream, pointer->state));
	CHK(encode_enumval(stream, &pointer->value));
	// EnumVal value
	EPILOGUE();
}

/**
 * Encode octet_string
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_octet_string(ByteStreamWriter *stream, octet_string *pointer)
{
	PROLOGUE();
	WRITE_STRING_WITH_LENGTH();
	EPILOGUE();
}

/**
 * Encode HighResRelativeTime
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_highresrelativetime(ByteStreamWriter *stream,
				HighResRelativeTime *pointer)
{
	PROLOGUE();
	// intu8 value[8];
	WRITE_STRING(8);
	EPILOGUE();
}

/**
 * Encode SampleType
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_sampletype(ByteStreamWriter *stream, SampleType *pointer)
{
	PROLOGUE();
	CHK(write_intu8(stream, pointer->sample_size));
	CHK(write_intu8(stream, pointer->significant_bits));
	EPILOGUE();
}

/**
 * Encode AttributeList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_attributelist(ByteStreamWriter *stream, AttributeList *pointer)
{
	CL_PROLOGUE();

	CHILDREN(ava_type);

	L_EPILOGUE();
}

/**
 * Encode SegmIdList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmidlist(ByteStreamWriter *stream, SegmIdList *pointer)
{
	CL_PROLOGUE();
	CHILDREN_16();
	L_EPILOGUE();
}

/**
 * Encode SimpleNuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_simplenuobsvaluecmp(ByteStreamWriter *stream,
				SimpleNuObsValueCmp *pointer)
{
	CL_PROLOGUE();
	CHILDREN_FLOAT();
	L_EPILOGUE();
}

/**
 * Encode GetResultSimple
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_getresultsimple(ByteStreamWriter *stream, GetResultSimple *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attributelist(stream, &pointer->attribute_list));
	// AttributeList attribute_list
	EPILOGUE();
}

/**
 * Encode HANDLEList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_handlelist(ByteStreamWriter *stream, HANDLEList *pointer)
{
	CL_PROLOGUE();
	CHILDREN_16();
	L_EPILOGUE();
}

/**
 * Encode SegmDataEventDescr
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmdataeventdescr(ByteStreamWriter *stream,
			       SegmDataEventDescr *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->segm_instance));
	CHK(write_intu32(stream, pointer->segm_evt_entry_index));
	CHK(write_intu32(stream, pointer->segm_evt_entry_count));
	CHK(write_intu16(stream, pointer->segm_evt_status));
	EPILOGUE();
}

/**
 * Encode AttrValMap
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_attrvalmap(ByteStreamWriter *stream, AttrValMap *pointer)
{
	CL_PROLOGUE();

	CHILDREN(attrvalmapentry);

	L_EPILOGUE();
}

/**
 * Encode ScaleRangeSpec8
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scalerangespec8(ByteStreamWriter *stream, ScaleRangeSpec8 *pointer)
{
	PROLOGUE();
	CHK(write_float(stream, pointer->lower_absolute_value));
	CHK(write_float(stream, pointer->upper_absolute_value));
	CHK(write_intu8(stream, pointer->lower_scaled_value));
	CHK(write_intu8(stream, pointer->upper_scaled_value));
	EPILOGUE();
}

/**
 * Encode PhdAssociationInformation
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_phdassociationinformation(ByteStreamWriter *stream,
				      PhdAssociationInformation *pointer)
{
	PROLOGUE();
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
	EPILOGUE();
}

/**
 * Encode ScanReportPerFixedList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportperfixedlist(ByteStreamWriter *stream,
				   ScanReportPerFixedList *pointer)
{
	CL_PROLOGUE();

	CHILDREN(scanreportperfixed);

	L_EPILOGUE();
}

/**
 * Encode ScanReportPerGroupedList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportpergroupedlist(ByteStreamWriter *stream,
				     ScanReportPerGroupedList *pointer)
{
	CL_PROLOGUE();

	CHILDREN(scanreportpergrouped);

	L_EPILOGUE();
}

/**
 * Encode DataProtoList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_dataprotolist(ByteStreamWriter *stream, DataProtoList *pointer)
{
	CL_PROLOGUE();

	CHILDREN(dataproto);

	L_EPILOGUE();
}

/**
 * Encode SegmSelection
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmselection(ByteStreamWriter *stream, SegmSelection *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->choice));
	RESERVE_LENGTH();

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
		ERROR("encoding: segm selection choice unknown");
		return 0;
		break;
	}
	L_EPILOGUE();
}

/**
 * Encode ErrorResult
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_errorresult(ByteStreamWriter *stream, ErrorResult *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->error_value));
	CHK(encode_any(stream, &pointer->parameter));
	// Any parameter
	EPILOGUE();
}

/**
 * Encode HandleAttrValMap
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_handleattrvalmap(ByteStreamWriter *stream,
			     HandleAttrValMap *pointer)
{
	CL_PROLOGUE();
	CHILDREN(handleattrvalmapentry);
	L_EPILOGUE();
}

/**
 * Encode AbsoluteTimeAdjust
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_absolutetimeadjust(ByteStreamWriter *stream,
			       AbsoluteTimeAdjust *pointer)
{
	PROLOGUE();
	// intu8 value[6]
	WRITE_STRING(6);

	EPILOGUE();
}

/**
 * Encode AARE_apdu
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_aare_apdu(ByteStreamWriter *stream, AARE_apdu *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->result));
	CHK(encode_dataproto(stream, &pointer->selected_data_proto));
	// DataProto selected_data_proto
	EPILOGUE();
}

/**
 * Encode RLRE_apdu
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_rlre_apdu(ByteStreamWriter *stream, RLRE_apdu *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->reason));
	EPILOGUE();
}

/**
 * Encode MetricIdList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_metricidlist(ByteStreamWriter *stream, MetricIdList *pointer)
{
	CL_PROLOGUE();
	CHILDREN_16();
	L_EPILOGUE();
}

/**
 * Encode ScanReportPerFixed
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportperfixed(ByteStreamWriter *stream,
			       ScanReportPerFixed *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->person_id));
	CHK(encode_observationscanfixedlist(stream, &pointer->obs_scan_fix));
	// ObservationScanFixedList obs_scan_fix
	EPILOGUE();
}

/**
 * Encode ScanReportInfoGrouped
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportinfogrouped(ByteStreamWriter *stream,
				  ScanReportInfoGrouped *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_scanreportinfogroupedlist(stream, &pointer->obs_scan_grouped));
	// ScanReportInfoGroupedList obs_scan_grouped
	EPILOGUE();
}

/**
 * Encode ObservationScan
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_observationscan(ByteStreamWriter *stream, ObservationScan *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attributelist(stream, &pointer->attributes));
	// AttributeList attributes
	EPILOGUE();
}

/**
 * Encode ScanReportPerGrouped
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportpergrouped(ByteStreamWriter *stream,
				 ScanReportPerGrouped *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->person_id));
	CHK(encode_octet_string(stream, &pointer->obs_scan_grouped));
	EPILOGUE();
}

/**
 * Encode SystemModel
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_systemmodel(ByteStreamWriter *stream, SystemModel *pointer)
{
	PROLOGUE();
	CHK(encode_octet_string(stream, &pointer->manufacturer));
	// octet_string manufacturer
	CHK(encode_octet_string(stream, &pointer->model_number));
	// octet_string model_number
	EPILOGUE();
}

/**
 * Encode ObservationScanList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_observationscanlist(ByteStreamWriter *stream,
				ObservationScanList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(observationscan);
	L_EPILOGUE();
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
 * @return encoded byte count if ok, 0 if error
 */
int encode_set_data_apdu(PRST_apdu *prst, DATA_apdu *data_apdu)
{
	PROLOGUE();
	prst->value = (intu8 *) data_apdu;
	EPILOGUE();
}

/**
 * Get Data_apdu from PRST_apdu definition, see encode_set_data_apdu().
 *
 * @param prst
 * @return data_apdu
 * @return encoded byte count if ok, 0 if error
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
 * @return encoded byte count if ok, 0 if error
 */
int encode_apdu(ByteStreamWriter *stream, APDU *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->choice));
	RESERVE_LENGTH();

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
		ERROR("encoding: apdu type unknown");
		return 0;
		break;
	}
	L_EPILOGUE();
}

/**
 * Encode PRST_apdu
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_prst_apdu(ByteStreamWriter *stream, PRST_apdu *pointer)
{
	PROLOGUE();
	RESERVE_LENGTH();

	if (pointer->value != NULL) {
		CHK(encode_data_apdu(stream, encode_get_data_apdu(pointer)));
	} else {
		ERROR("encode_prst_apdu pointer value");
		return 0;
	}
	L_EPILOGUE();
}

/**
 * Encode PmSegmentEntryMap
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_pmsegmententrymap(ByteStreamWriter *stream,
			      PmSegmentEntryMap *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->segm_entry_header));
	CHK(encode_segmentryelemlist(stream, &pointer->segm_entry_elem_list));
	// SegmEntryElemList segm_entry_elem_list
	EPILOGUE();
}

/**
 * Encode Any
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_any(ByteStreamWriter *stream, Any *pointer)
{
	PROLOGUE();
	WRITE_STRING_WITH_LENGTH();
	EPILOGUE();
}

/**
 * Encode SetArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_setargumentsimple(ByteStreamWriter *stream,
			      SetArgumentSimple *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_modificationlist(stream, &pointer->modification_list));
	// ModificationList modification_list
	EPILOGUE();
}

/**
 * Encode SegmentInfo
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmentinfo(ByteStreamWriter *stream, SegmentInfo *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->seg_inst_no));
	CHK(encode_attributelist(stream, &pointer->seg_info));
	// AttributeList seg_info
	EPILOGUE();
}

/**
 * Encode PmSegmElemStaticAttrList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_pmsegmelemstaticattrlist(ByteStreamWriter *stream,
				     PmSegmElemStaticAttrList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(segmelemstaticattrentry);
	L_EPILOGUE();
}

/**
 * Encode AbsTimeRange
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_abstimerange(ByteStreamWriter *stream, AbsTimeRange *pointer)
{
	PROLOGUE();
	CHK(encode_absolutetime(stream, &pointer->from_time));
	// AbsoluteTime from_time
	CHK(encode_absolutetime(stream, &pointer->to_time));
	// AbsoluteTime to_time
	EPILOGUE();
}

/**
 * Encode ScanReportInfoMPVar
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportinfompvar(ByteStreamWriter *stream,
				ScanReportInfoMPVar *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_scanreportpervarlist(stream, &pointer->scan_per_var));
	// ScanReportPerVarList scan_per_var
	EPILOGUE();
}

/**
 * Encode UUID_Ident
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_uuid_ident(ByteStreamWriter *stream, UUID_Ident *pointer)
{
	PROLOGUE();
	// intu8 value[16]
	WRITE_STRING(16);
	EPILOGUE();
}

/**
 * Encode GetArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_getargumentsimple(ByteStreamWriter *stream,
			      GetArgumentSimple *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attributeidlist(stream, &pointer->attribute_id_list));
	// AttributeIdList attribute_id_list
	EPILOGUE();
}

/**
 * Encode RegCertDataList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_regcertdatalist(ByteStreamWriter *stream, RegCertDataList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(regcertdata);
	L_EPILOGUE();
}

/**
 * Encode ConfigReportRsp
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_configreportrsp(ByteStreamWriter *stream, ConfigReportRsp *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->config_report_id));
	CHK(write_intu16(stream, pointer->config_result));
	EPILOGUE();
}

/**
 * Encode DataProto
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_dataproto(ByteStreamWriter *stream, DataProto *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->data_proto_id));
	CHK(encode_any(stream, &pointer->data_proto_info));
	// Any data_proto_info
	EPILOGUE();
}

/**
 * Encode MetricStructureSmall
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_metricstructuresmall(ByteStreamWriter *stream,
				 MetricStructureSmall *pointer)
{
	PROLOGUE();
	CHK(write_intu8(stream, pointer->ms_struct));
	CHK(write_intu8(stream, pointer->ms_comp_no));
	EPILOGUE();
}

/**
 * Encode SegmentStatisticEntry
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmentstatisticentry(ByteStreamWriter *stream,
				  SegmentStatisticEntry *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->segm_stat_type));
	CHK(encode_octet_string(stream, &pointer->segm_stat_entry));
	// octet_string segm_stat_entry
	EPILOGUE();
}

/**
 * Encode SegmentDataEvent
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmentdataevent(ByteStreamWriter *stream,
			     SegmentDataEvent *pointer)
{
	PROLOGUE();
	CHK(encode_segmdataeventdescr(stream, &pointer->segm_data_event_descr));
	// SegmDataEventDescr segm_data_event_descr
	CHK(encode_octet_string(stream, &pointer->segm_data_event_entries));
	// octet_string segm_data_event_entries
	EPILOGUE();
}

/**
 * Encode SegmEntryElemList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmentryelemlist(ByteStreamWriter *stream,
			      SegmEntryElemList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(segmentryelem);
	L_EPILOGUE();
}

/**
 * Encode SaSpec
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_saspec(ByteStreamWriter *stream, SaSpec *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->array_size));
	CHK(encode_sampletype(stream, &pointer->sample_type));
	// SampleType sample_type
	CHK(write_intu16(stream, pointer->flags));
	EPILOGUE();
}

/**
 * Encode AttributeModEntry
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_attributemodentry(ByteStreamWriter *stream,
			      AttributeModEntry *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->modify_operator));
	CHK(encode_ava_type(stream, &pointer->attribute));
	// AVA_Type attribute
	EPILOGUE();
}

/**
 * Encode MdsTimeInfo
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_mdstimeinfo(ByteStreamWriter *stream, MdsTimeInfo *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->mds_time_cap_state));
	CHK(write_intu16(stream, pointer->time_sync_protocol));
	CHK(write_intu32(stream, pointer->time_sync_accuracy));
	CHK(write_intu16(stream, pointer->time_resolution_abs_time));
	CHK(write_intu16(stream, pointer->time_resolution_rel_time));
	CHK(write_intu32(stream, pointer->time_resolution_high_res_time));
	EPILOGUE();
}

/**
 * Encode EnumVal
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_enumval(ByteStreamWriter *stream, EnumVal *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->choice));
	RESERVE_LENGTH();

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
		ERROR("encoding: enumval choice unknown");
		return 0;
		break;
	}
	L_EPILOGUE();
}

/**
 * Encode TrigSegmDataXferReq
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_trigsegmdataxferreq(ByteStreamWriter *stream,
				TrigSegmDataXferReq *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->seg_inst_no));
	EPILOGUE();
}

/**
 * Encode BatMeasure
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_batmeasure(ByteStreamWriter *stream, BatMeasure *pointer)
{
	PROLOGUE();
	CHK(write_float(stream, pointer->value));
	CHK(write_intu16(stream, pointer->unit));
	EPILOGUE();
}

/**
 * Encode SegmentStatistics
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmentstatistics(ByteStreamWriter *stream,
			      SegmentStatistics *pointer)
{
	CL_PROLOGUE();
	CHILDREN(segmentstatisticentry);
	L_EPILOGUE();
}

/**
 * Encode AttributeIdList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_attributeidlist(ByteStreamWriter *stream, AttributeIdList *pointer)
{
	CL_PROLOGUE();
	CHILDREN_16();
	L_EPILOGUE();
}

/**
 * Encode ScanReportInfoFixed
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportinfofixed(ByteStreamWriter *stream,
				ScanReportInfoFixed *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_observationscanfixedlist(stream, &pointer->obs_scan_fixed));
	// ObservationScanFixedList obs_scan_fixed
	EPILOGUE();
}

/**
 * Encode DataRequest
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_datarequest(ByteStreamWriter *stream, DataRequest *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->data_req_mode));
	CHK(write_intu32(stream, pointer->data_req_time));
	CHK(write_intu16(stream, pointer->data_req_person_id));
	CHK(write_intu16(stream, pointer->data_req_class));
	CHK(encode_handlelist(stream, &pointer->data_req_obj_handle_list));
	// HANDLEList data_req_obj_handle_list
	EPILOGUE();
}

/**
 * Encode AuthBodyAndStrucType
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_authbodyandstructype(ByteStreamWriter *stream,
				 AuthBodyAndStrucType *pointer)
{
	PROLOGUE();
	CHK(write_intu8(stream, pointer->auth_body));
	CHK(write_intu8(stream, pointer->auth_body_struc_type));
	EPILOGUE();
}

/**
 * Encode RLRQ_apdu
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_rlrq_apdu(ByteStreamWriter *stream, RLRQ_apdu *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->reason));
	EPILOGUE();
}

/**
 * Encode Data_apdu_message
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_data_apdu_message(ByteStreamWriter *stream,
			      Data_apdu_message *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->choice));
	RESERVE_LENGTH();

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
		return 0;
		break;
	}
	L_EPILOGUE();
}

/**
 * Encode EventReportArgumentSimple
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_eventreportargumentsimple(ByteStreamWriter *stream,
				      EventReportArgumentSimple *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(write_intu32(stream, pointer->event_time));
	CHK(write_intu16(stream, pointer->event_type));
	CHK(encode_any(stream, &pointer->event_info));
	// Any event_info
	EPILOGUE();
}

/**
 * Encode ScanReportInfoVar
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportinfovar(ByteStreamWriter *stream,
			      ScanReportInfoVar *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_observationscanlist(stream, &pointer->obs_scan_var));
	// ObservationScanList obs_scan_var
	EPILOGUE();
}

/**
 * Encode ScanReportInfoMPGrouped
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportinfompgrouped(ByteStreamWriter *stream,
				    ScanReportInfoMPGrouped *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->data_req_id));
	CHK(write_intu16(stream, pointer->scan_report_no));
	CHK(encode_scanreportpergroupedlist(stream, &pointer->scan_per_grouped));
	// ScanReportPerGroupedList scan_per_grouped
	EPILOGUE();
}

/**
 * Encode ConfigObject
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_configobject(ByteStreamWriter *stream, ConfigObject *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_class));
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attributelist(stream, &pointer->attributes));
	// AttributeList attributes
	EPILOGUE();
}

/**
 * Encode ScanReportInfoGroupedList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportinfogroupedlist(ByteStreamWriter *stream,
				      ScanReportInfoGroupedList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(octet_string);
	L_EPILOGUE();
}

/**
 * Encode EventReportResultSimple
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_eventreportresultsimple(ByteStreamWriter *stream,
				    EventReportResultSimple *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(write_intu32(stream, pointer->currentTime));
	CHK(write_intu16(stream, pointer->event_type));
	CHK(encode_any(stream, &pointer->event_reply_info));
	// Any event_reply_info
	EPILOGUE();
}

/**
 * Encode TYPE
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_type(ByteStreamWriter *stream, TYPE *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->partition));
	CHK(write_intu16(stream, pointer->code));
	EPILOGUE();
}

/**
 * Encode MetricSpecSmall
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_metricspecsmall(ByteStreamWriter *stream, MetricSpecSmall *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, *pointer));
	EPILOGUE();
}

/**
 * Encode ObservationScanFixed
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_observationscanfixed(ByteStreamWriter *stream,
				 ObservationScanFixed *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_octet_string(stream, &pointer->obs_val_data));
	// octet_string obs_val_data
	EPILOGUE();
}

/**
 * Encode DataResponse
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_dataresponse(ByteStreamWriter *stream, DataResponse *pointer)
{
	PROLOGUE();
	CHK(write_intu32(stream, pointer->rel_time_stamp));
	CHK(write_intu16(stream, pointer->data_req_result));
	CHK(write_intu16(stream, pointer->event_type));
	CHK(encode_any(stream, &pointer->event_info));
	// Any event_info
	EPILOGUE();
}

/**
 * Encode ProdSpecEntry
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_prodspecentry(ByteStreamWriter *stream, ProdSpecEntry *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->spec_type));
	CHK(write_intu16(stream, pointer->component_id));
	CHK(encode_octet_string(stream, &pointer->prod_spec));
	// octet_string prod_spec
	EPILOGUE();
}

/**
 * Encode ScaleRangeSpec16
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scalerangespec16(ByteStreamWriter *stream,
			     ScaleRangeSpec16 *pointer)
{
	PROLOGUE();
	CHK(write_float(stream, pointer->lower_absolute_value));
	CHK(write_float(stream, pointer->upper_absolute_value));
	CHK(write_intu16(stream, pointer->lower_scaled_value));
	CHK(write_intu16(stream, pointer->upper_scaled_value));
	EPILOGUE();
}

/**
 * Encode SegmEntryElem
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmentryelem(ByteStreamWriter *stream, SegmEntryElem *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->class_id));
	CHK(encode_type(stream, &pointer->metric_type));
	// TYPE metric_type
	CHK(write_intu16(stream, pointer->handle));
	CHK(encode_attrvalmap(stream, &pointer->attr_val_map));
	// AttrValMap attr_val_map
	EPILOGUE();
}

/**
 * Encode ABRT_apdu
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_abrt_apdu(ByteStreamWriter *stream, ABRT_apdu *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->reason));
	EPILOGUE();
}

/**
 * Encode DataReqModeCapab
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_datareqmodecapab(ByteStreamWriter *stream,
			     DataReqModeCapab *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->data_req_mode_flags));
	CHK(write_intu8(stream, pointer->data_req_init_agent_count));
	CHK(write_intu8(stream, pointer->data_req_init_manager_count));
	EPILOGUE();
}

/**
 * Encode SupplementalTypeList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_supplementaltypelist(ByteStreamWriter *stream,
				 SupplementalTypeList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(type);
	L_EPILOGUE();
}

/**
 * Encode ObservationScanFixedList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_observationscanfixedlist(ByteStreamWriter *stream,
				     ObservationScanFixedList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(observationscanfixed);
	L_EPILOGUE();
}

/**
 * Encode TrigSegmDataXferRsp
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_trigsegmdataxferrsp(ByteStreamWriter *stream,
				TrigSegmDataXferRsp *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->seg_inst_no));
	CHK(write_intu16(stream, pointer->trig_segm_xfer_rsp));
	EPILOGUE();
}

/**
 * Encode DATA_apdu
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_data_apdu(ByteStreamWriter *stream, DATA_apdu *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->invoke_id));
	CHK(encode_data_apdu_message(stream, &pointer->message));
	// Data_apdu_message message
	EPILOGUE();
}

/**
 * Encode AARQ_apdu
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_aarq_apdu(ByteStreamWriter *stream, AARQ_apdu *pointer)
{
	PROLOGUE();
	CHK(write_intu32(stream, pointer->assoc_version));
	CHK(encode_dataprotolist(stream, &pointer->data_proto_list));
	// DataProtoList data_proto_list
	EPILOGUE();
}

/**
 * Encode TypeVerList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_typeverlist(ByteStreamWriter *stream, TypeVerList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(typever);
	L_EPILOGUE();
}

/**
 * Encode RegCertData
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_regcertdata(ByteStreamWriter *stream, RegCertData *pointer)
{
	PROLOGUE();
	CHK(encode_authbodyandstructype(stream, &pointer->auth_body_and_struc_type));
	// AuthBodyAndStrucType auth_body_and_struc_type
	CHK(encode_any(stream, &pointer->auth_body_data));
	// Any auth_body_data
	EPILOGUE();
}

/**
 * Encode NuObsValue
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_nuobsvalue(ByteStreamWriter *stream, NuObsValue *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->metric_id));
	CHK(write_intu16(stream, pointer->state));
	CHK(write_intu16(stream, pointer->unit_code));
	CHK(write_float(stream, pointer->value));
	EPILOGUE();
}

/**
 * Encode ScanReportPerVarList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_scanreportpervarlist(ByteStreamWriter *stream,
				 ScanReportPerVarList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(scanreportpervar);
	L_EPILOGUE();
}

/**
 * Encode SetTimeInvoke
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_settimeinvoke(ByteStreamWriter *stream, SetTimeInvoke *pointer)
{
	PROLOGUE();
	CHK(encode_absolutetime(stream, &pointer->date_time));
	// AbsoluteTime date_time
	CHK(write_float(stream, pointer->accuracy));
	EPILOGUE();
}

/**
 * Encode SegmentInfoList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmentinfolist(ByteStreamWriter *stream, SegmentInfoList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(segmentinfo);
	L_EPILOGUE();
}

/**
 * Encode ActionResultSimple
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_actionresultsimple(ByteStreamWriter *stream,
			       ActionResultSimple *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(write_intu16(stream, pointer->action_type));
	CHK(encode_any(stream, &pointer->action_info_args));
	// Any action_info_args
	EPILOGUE();
}

/**
 * Encode SegmElemStaticAttrEntry
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_segmelemstaticattrentry(ByteStreamWriter *stream,
				    SegmElemStaticAttrEntry *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->class_id));
	CHK(encode_type(stream, &pointer->metric_type));
	// TYPE metric_type
	CHK(encode_attributelist(stream, &pointer->attribute_list));
	// AttributeList attribute_list
	EPILOGUE();
}

/**
 * Encode BasicNuObsValue
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_basicnuobsvalue(ByteStreamWriter *stream,
			       BasicNuObsValue *pointer)
{
	PROLOGUE();
	CHK(write_sfloat(stream, *pointer));
	EPILOGUE();
}

/**
 * Encode BasicNuObsValueCmp
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_basicnuobsvaluecmp(ByteStreamWriter *stream,
			       BasicNuObsValueCmp *pointer)
{
	CL_PROLOGUE();
	CHILDREN_SFLOAT();
	L_EPILOGUE();
}

/**
 * Encode ConfigObjectList
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_configobjectlist(ByteStreamWriter *stream,
			     ConfigObjectList *pointer)
{
	CL_PROLOGUE();
	CHILDREN(configobject);
	L_EPILOGUE();
}

/**
 * Encode SetResultSimple
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_setresultsimple(ByteStreamWriter *stream, SetResultSimple *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attributelist(stream, &pointer->attribute_list));
	// AttributeList attribute_list
	EPILOGUE();
}

/**
 * Encode HandleAttrValMapEntry
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_handleattrvalmapentry(ByteStreamWriter *stream,
				  HandleAttrValMapEntry *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, pointer->obj_handle));
	CHK(encode_attrvalmap(stream, &pointer->attr_val_map));
	// AttrValMap attr_val_map
	EPILOGUE();
}

/**
 * Encode SimpleNuObsValue
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_simplenuobsvalue(ByteStreamWriter *stream,
			     SimpleNuObsValue *pointer)
{
	PROLOGUE();
	CHK(write_float(stream, *pointer));
	EPILOGUE();
}

/**
 * Encode ConfigId
 *
 * @param *stream
 * @param *pointer
 * @return encoded byte count if ok, 0 if error
 */
int encode_configid(ByteStreamWriter *stream, ConfigId *pointer)
{
	PROLOGUE();
	CHK(write_intu16(stream, *pointer));
	EPILOGUE();
}

/** @} */
