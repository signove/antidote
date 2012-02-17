/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file struct_cleaner.c
 * \brief Struct cleaner implementation.
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
 * \date Jun 21, 2010
 */

/**
 * \addtogroup ASN1Codec
 *
 * @{
 */

#include <stdlib.h>
#include <string.h>
#include "src/communication/parser/struct_cleaner.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/dim/nomenclature.h"

/**
 * \cond Undocumented
 */

#define QUOTE(x) #x

#define CLVC()				\
	memset(pointer, 0, sizeof(*pointer));

#define CLV()				\
	free(pointer->value);		\
	CLVC();

#define CHILDREN_GENERIC(delfunction)								\
	if (pointer->value) {									\
		int i;										\
		for (i = 0; i < pointer->count; i++) {						\
			delfunction(pointer->value + i);					\
		}										\
	}											

#define DEL_FUNCTION(type) del_##type
#define CHILDREN(lctype) CHILDREN_GENERIC(DEL_FUNCTION(lctype))

/**
 * \endcond
 */

/**
 * Delete SegmentDataResult
 *
 * @param *pointer
 */
void del_segmentdataresult(SegmentDataResult *pointer)
{
	del_segmdataeventdescr(&pointer->segm_data_event_descr);
}

/**
 * Delete ScanReportPerVar
 *
 * @param *pointer
 */
void del_scanreportpervar(ScanReportPerVar *pointer)
{
	del_observationscanlist(&pointer->obs_scan_var);
}

/**
 * Delete TypeVer
 *
 * @param *pointer
 */
void del_typever(TypeVer *pointer)
{
}

/**
 * Delete ModificationList
 *
 * @param *pointer
 */
void del_modificationlist(ModificationList *pointer)
{
	CHILDREN(attributemodentry);
	CLV();
}

/**
 * Delete ProductionSpec
 *
 * @param *pointer
 */
void del_productionspec(ProductionSpec *pointer)
{
	CHILDREN(prodspecentry);
	CLV();
}

/**
 * Delete ActionArgumentSimple
 *
 * @param *pointer
 */
void del_actionargumentsimple(ActionArgumentSimple *pointer)
{
	del_any(&pointer->action_info_args);
}

/**
 * Delete ScaleRangeSpec32
 *
 * @param *pointer
 */
void del_scalerangespec32(ScaleRangeSpec32 *pointer)
{
}

/**
 * Delete AVA_Type
 *
 * @param *pointer
 */
void del_ava_type(AVA_Type *pointer)
{
	del_any(&pointer->attribute_value);
}

/**
 * Delete ConfigReport
 *
 * @param *pointer
 */
void del_configreport(ConfigReport *pointer)
{
	del_configobjectlist(&pointer->config_obj_list);
}

/**
 * Delete AttrValMapEntry
 *
 * @param *pointer
 */
void del_attrvalmapentry(AttrValMapEntry *pointer)
{
}

/**
 * Delete AbsoluteTime
 *
 * @param *pointer
 */
void del_absolutetime(AbsoluteTime *pointer)
{
}

/**
 * Delete NuObsValueCmp
 *
 * @param *pointer
 */
void del_nuobsvaluecmp(NuObsValueCmp *pointer)
{
	CHILDREN(nuobsvalue);
	CLV();
}

/**
 * Delete ScanReportInfoMPFixed
 *
 * @param *pointer
 */
void del_scanreportinfompfixed(ScanReportInfoMPFixed *pointer)
{
	del_scanreportperfixedlist(&pointer->scan_per_fixed);
}

/**
 * Delete RejectResult
 *
 * @param *pointer
 */
void del_rejectresult(RejectResult *pointer)
{
}

/**
 * Delete ManufSpecAssociationInformation
 *
 * @param *pointer
 */
void del_manufspecassociationinformation(
	ManufSpecAssociationInformation *pointer)
{
	del_uuid_ident(&pointer->data_proto_id_ext);
	del_any(&pointer->data_proto_info_ext);
}

/**
 * Delete EnumObsValue
 *
 * @param *pointer
 */
void del_enumobsvalue(EnumObsValue *pointer)
{
	del_enumval(&pointer->value);
}

/**
 * Delete octet_string
 *
 * @param *pointer
 */
void del_octet_string(octet_string *pointer)
{
	CLV();
}

/**
 * Delete EnumPrintableString
 *
 * @param *pointer
 */
void del_enumprintablestring(EnumPrintableString *pointer)
{
	CLV();
}

/**
 * Delete HighResRelativeTime
 *
 * @param *pointer
 */
void del_highresrelativetime(HighResRelativeTime *pointer)
{
}

/**
 * Delete SampleType
 *
 * @param *pointer
 */
void del_sampletype(SampleType *pointer)
{
}

/**
 * Delete AttributeList
 *
 * @param *pointer
 */
void del_attributelist(AttributeList *pointer)
{
	CHILDREN(ava_type);
	CLV();
}

/**
 * Delete SegmIdList
 *
 * @param *pointer
 */
void del_segmidlist(SegmIdList *pointer)
{
	CLV();
}

/**
 * Delete SimpleNuObsValueCmp
 *
 * @param *pointer
 */
void del_simplenuobsvaluecmp(SimpleNuObsValueCmp *pointer)
{
	CLV();
}

/**
 * Delete GetResultSimple
 *
 * @param *pointer
 */
void del_getresultsimple(GetResultSimple *pointer)
{
	del_attributelist(&pointer->attribute_list);
}


/**
 * Delete HANDLEList
 *
 * @param *pointer
 */
void del_handlelist(HANDLEList *pointer)
{
	CLV();
}

/**
 * Delete SegmDataEventDescr
 *
 * @param *pointer
 */
void del_segmdataeventdescr(SegmDataEventDescr *pointer)
{
}

/**
 * Delete AttrValMap
 *
 * @param *pointer
 */
void del_attrvalmap(AttrValMap *pointer)
{
	CHILDREN(attrvalmapentry);
	CLV();
}

/**
 * Delete ScaleRangeSpec8
 *
 * @param *pointer
 */
void del_scalerangespec8(ScaleRangeSpec8 *pointer)
{
}

/**
 * Delete PhdAssociationInformation
 *
 * @param *pointer
 */
void del_phdassociationinformation(PhdAssociationInformation *pointer)
{
	del_octet_string(&pointer->system_id);
	del_datareqmodecapab(&pointer->data_req_mode_capab);
	del_attributelist(&pointer->optionList);
}

/**
 * Delete ScanReportPerFixedList
 *
 * @param *pointer
 */
void del_scanreportperfixedlist(ScanReportPerFixedList *pointer)
{
	CHILDREN(scanreportperfixed);
	CLV();
}

/**
 * Delete ScanReportPerGroupedList
 *
 * @param *pointer
 */
void del_scanreportpergroupedlist(ScanReportPerGroupedList *pointer)
{
	CHILDREN(scanreportpergrouped);
	CLV();
}

/**
 * Delete DataProtoList
 *
 * @param *pointer
 */
void del_dataprotolist(DataProtoList *pointer)
{
	CHILDREN(dataproto);
	CLV();
}

/**
 * Delete SegmSelection
 *
 * @param *pointer
 */
void del_segmselection(SegmSelection *pointer)
{
	switch (pointer->choice) {
	case ALL_SEGMENTS_CHOSEN:
		break;
	case SEGM_ID_LIST_CHOSEN:
		del_segmidlist(&pointer->u.segm_id_list);
		break;
	case ABS_TIME_RANGE_CHOSEN:
		del_abstimerange(&pointer->u.abs_time_range);
		break;
	default:
		break;
	}
	CLVC();
}

/**
 * Delete ErrorResult
 *
 * @param *pointer
 */
void del_errorresult(ErrorResult *pointer)
{
	del_any(&pointer->parameter);
}

/**
 * Delete HandleAttrValMap
 *
 * @param *pointer
 */
void del_handleattrvalmap(HandleAttrValMap *pointer)
{
	CHILDREN(handleattrvalmapentry);
	CLV();
}

/**
 * Delete AbsoluteTimeAdjust
 *
 * @param *pointer
 */
void del_absolutetimeadjust(AbsoluteTimeAdjust *pointer)
{
}

/**
 * Delete AARE_apdu
 *
 * @param *pointer
 */
void del_aare_apdu(AARE_apdu *pointer)
{
	del_dataproto(&pointer->selected_data_proto);
}

/**
 * Delete RLRE_apdu
 *
 * @param *pointer
 */
void del_rlre_apdu(RLRE_apdu *pointer)
{
}

/**
 * Delete MetricIdList
 *
 * @param *pointer
 */
void del_metricidlist(MetricIdList *pointer)
{
	CLV();
}

/**
 * Delete ScanReportPerFixed
 *
 * @param *pointer
 */
void del_scanreportperfixed(ScanReportPerFixed *pointer)
{
	del_observationscanfixedlist(&pointer->obs_scan_fix);
}

/**
 * Delete ScanReportInfoGrouped
 *
 * @param *pointer
 */
void del_scanreportinfogrouped(ScanReportInfoGrouped *pointer)
{
	del_scanreportinfogroupedlist(&pointer->obs_scan_grouped);
}

/**
 * Delete ObservationScan
 *
 * @param *pointer
 */
void del_observationscan(ObservationScan *pointer)
{
	del_attributelist(&pointer->attributes);
}

/**
 * Delete ScanReportPerGrouped
 *
 * @param *pointer
 */
void del_scanreportpergrouped(ScanReportPerGrouped *pointer)
{
	del_octet_string(&pointer->obs_scan_grouped);
}

/**
 * Delete SystemModel
 *
 * @param *pointer
 */
void del_systemmodel(SystemModel *pointer)
{
	del_octet_string(&pointer->manufacturer);
	del_octet_string(&pointer->model_number);
}

/**
 * Delete ObservationScanList
 *
 * @param *pointer
 */
void del_observationscanlist(ObservationScanList *pointer)
{
	CHILDREN(observationscan);
	CLV();
}

/**
 * Delete APDU
 *
 * @param *pointer
 */
void del_apdu(APDU *pointer)
{
	switch (pointer->choice) {
	case AARQ_CHOSEN:
		del_aarq_apdu(&pointer->u.aarq);
		break;
	case AARE_CHOSEN:
		del_aare_apdu(&pointer->u.aare);
		break;
	case RLRQ_CHOSEN:
		del_rlrq_apdu(&pointer->u.rlrq);
		break;
	case RLRE_CHOSEN:
		del_rlre_apdu(&pointer->u.rlre);
		break;
	case ABRT_CHOSEN:
		del_abrt_apdu(&pointer->u.abrt);
		break;
	case PRST_CHOSEN:
		del_prst_apdu(&(pointer->u.prst));
		break;
	default:
		break;
	}
	CLVC();
}

/**
 * Delete PRST_apdu
 *
 * @param *pointer
 */
void del_prst_apdu(PRST_apdu *pointer)
{
	if (pointer->value != NULL) {
		del_data_apdu(encode_get_data_apdu(pointer));
	}
	CLV();
}

/**
 * Delete PmSegmentEntryMap
 *
 * @param *pointer
 */
void del_pmsegmententrymap(PmSegmentEntryMap *pointer)
{
	del_segmentryelemlist(&pointer->segm_entry_elem_list);
}

/**
 * Delete Any
 *
 * @param *pointer
 */
void del_any(Any *pointer)
{
	CLV();
}

/**
 * Delete SetArgumentSimple
 *
 * @param *pointer
 */
void del_setargumentsimple(SetArgumentSimple *pointer)
{
	del_modificationlist(&pointer->modification_list);
}

/**
 * Delete SegmentInfo
 *
 * @param *pointer
 */
void del_segmentinfo(SegmentInfo *pointer)
{
	del_attributelist(&pointer->seg_info);
}

/**
 * Delete PmSegmElemStaticAttrList
 *
 * @param *pointer
 */
void del_pmsegmelemstaticattrlist(PmSegmElemStaticAttrList *pointer)
{
	CHILDREN(segmelemstaticattrentry);
	CLV();
}

/**
 * Delete AbsTimeRange
 *
 * @param *pointer
 */
void del_abstimerange(AbsTimeRange *pointer)
{
	del_absolutetime(&pointer->from_time);
	del_absolutetime(&pointer->to_time);
}

/**
 * Delete ScanReportInfoMPVar
 *
 * @param *pointer
 */
void del_scanreportinfompvar(ScanReportInfoMPVar *pointer)
{
	del_scanreportpervarlist(&pointer->scan_per_var);
}

/**
 * Delete UUID_Ident
 *
 * @param *pointer
 */
void del_uuid_ident(UUID_Ident *pointer)
{
}

/**
 * Delete GetArgumentSimple
 *
 * @param *pointer
 */
void del_getargumentsimple(GetArgumentSimple *pointer)
{
	del_attributeidlist(&pointer->attribute_id_list);
}

/**
 * Delete RegCertDataList
 *
 * @param *pointer
 */
void del_regcertdatalist(RegCertDataList *pointer)
{
	CHILDREN(regcertdata);
	CLV();
}

/**
 * Delete ConfigReportRsp
 *
 * @param *pointer
 */
void del_configreportrsp(ConfigReportRsp *pointer)
{
}

/**
 * Delete DataProto
 *
 * @param *pointer
 */
void del_dataproto(DataProto *pointer)
{
	del_any(&pointer->data_proto_info);
}

/**
 * Delete MetricStructureSmall
 *
 * @param *pointer
 */
void del_metricstructuresmall(MetricStructureSmall *pointer)
{
}

/**
 * Delete SegmentStatisticEntry
 *
 * @param *pointer
 */
void del_segmentstatisticentry(SegmentStatisticEntry *pointer)
{
	del_octet_string(&pointer->segm_stat_entry);
}

/**
 * Delete SegmentDataEvent
 *
 * @param *pointer
 */
void del_segmentdataevent(SegmentDataEvent *pointer)
{
	del_segmdataeventdescr(&pointer->segm_data_event_descr);
	del_octet_string(&pointer->segm_data_event_entries);
}

/**
 * Delete SegmEntryElemList
 *
 * @param *pointer
 */
void del_segmentryelemlist(SegmEntryElemList *pointer)
{
	CHILDREN(segmentryelem);
	CLV();
}

/**
 * Delete SaSpec
 *
 * @param *pointer
 */
void del_saspec(SaSpec *pointer)
{
	del_sampletype(&pointer->sample_type);
}

/**
 * Delete AttributeModEntry
 *
 * @param *pointer
 */
void del_attributemodentry(AttributeModEntry *pointer)
{
	del_ava_type(&pointer->attribute);
}

/**
 * Delete MdsTimeInfo
 *
 * @param *pointer
 */
void del_mdstimeinfo(MdsTimeInfo *pointer)
{
}

/**
 * Delete EnumVal
 *
 * @param *pointer
 */
void del_enumval(EnumVal *pointer)
{
	switch (pointer->choice) {
	case OBJ_ID_CHOSEN:
		break;
	case TEXT_STRING_CHOSEN:
		del_octet_string(&pointer->u.enum_text_string);
		break;
	case BIT_STR_CHOSEN:
		break;
	default:
		break;
	}
	CLVC();
}

/**
 * Delete TrigSegmDataXferReq
 *
 * @param *pointer
 */
void del_trigsegmdataxferreq(TrigSegmDataXferReq *pointer)
{
}

/**
 * Delete BatMeasure
 *
 * @param *pointer
 */
void del_batmeasure(BatMeasure *pointer)
{
}

/**
 * Delete SegmentStatistics
 *
 * @param *pointer
 */
void del_segmentstatistics(SegmentStatistics *pointer)
{
	CHILDREN(segmentstatisticentry);
	CLV();
}

/**
 * Delete AttributeIdList
 *
 * @param *pointer
 */
void del_attributeidlist(AttributeIdList *pointer)
{
	CLV();
}

/**
 * Delete ScanReportInfoFixed
 *
 * @param *pointer
 */
void del_scanreportinfofixed(ScanReportInfoFixed *pointer)
{
	del_observationscanfixedlist(&pointer->obs_scan_fixed);
}

/**
 * Delete DataRequest
 *
 * @param *pointer
 */
void del_datarequest(DataRequest *pointer)
{
	del_handlelist(&pointer->data_req_obj_handle_list);
}

/**
 * Delete AuthBodyAndStrucType
 *
 * @param *pointer
 */
void del_authbodyandstructype(AuthBodyAndStrucType *pointer)
{
}

/**
 * Delete RLRQ_apdu
 *
 * @param *pointer
 */
void del_rlrq_apdu(RLRQ_apdu *pointer)
{
}

/**
 * Delete Data_apdu_message
 *
 * @param *pointer
 */
void del_data_apdu_message(Data_apdu_message *pointer)
{
	switch (pointer->choice) {
	case ROIV_CMIP_EVENT_REPORT_CHOSEN:
		del_eventreportargumentsimple(&pointer->u.roiv_cmipEventReport);
		break;
	case ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		del_eventreportargumentsimple(
			&pointer->u.roiv_cmipConfirmedEventReport);
		break;
	case ROIV_CMIP_GET_CHOSEN:
		del_getargumentsimple(&pointer->u.roiv_cmipGet);
		break;
	case ROIV_CMIP_SET_CHOSEN:
		del_setargumentsimple(&pointer->u.roiv_cmipSet);
		break;
	case ROIV_CMIP_CONFIRMED_SET_CHOSEN:
		del_setargumentsimple(&pointer->u.roiv_cmipConfirmedSet);
		break;
	case ROIV_CMIP_ACTION_CHOSEN:
		del_actionargumentsimple(&pointer->u.roiv_cmipAction);
		break;
	case ROIV_CMIP_CONFIRMED_ACTION_CHOSEN:
		del_actionargumentsimple(&pointer->u.roiv_cmipConfirmedAction);
		break;
	case RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN:
		del_eventreportresultsimple(
			&pointer->u.rors_cmipConfirmedEventReport);
		break;
	case RORS_CMIP_GET_CHOSEN:
		del_getresultsimple(&pointer->u.rors_cmipGet);
		break;
	case RORS_CMIP_CONFIRMED_SET_CHOSEN:
		del_setresultsimple(&pointer->u.rors_cmipConfirmedSet);
		break;
	case RORS_CMIP_CONFIRMED_ACTION_CHOSEN:
		del_actionresultsimple(&pointer->u.rors_cmipConfirmedAction);
		break;
	case ROER_CHOSEN:
		del_errorresult(&pointer->u.roer);
		break;
	case RORJ_CHOSEN:
		del_rejectresult(&pointer->u.rorj);
		break;
	default:
		break;
	}
	CLVC();
}

/**
 * Delete EventReportArgumentSimple
 *
 * @param *pointer
 */
void del_eventreportargumentsimple(EventReportArgumentSimple *pointer)
{
	del_any(&(pointer->event_info));
}

/**
 * Delete ScanReportInfoVar
 *
 * @param *pointer
 */
void del_scanreportinfovar(ScanReportInfoVar *pointer)
{
	del_observationscanlist(&pointer->obs_scan_var);
}

/**
 * Delete ScanReportInfoMPGrouped
 *
 * @param *pointer
 */
void del_scanreportinfompgrouped(ScanReportInfoMPGrouped *pointer)
{
	del_scanreportpergroupedlist(&pointer->scan_per_grouped);
}

/**
 * Delete ConfigObject
 *
 * @param *pointer
 */
void del_configobject(ConfigObject *pointer)
{
	del_attributelist(&pointer->attributes);
}

/**
 * Delete ScanReportInfoGroupedList
 *
 * @param *pointer
 */
void del_scanreportinfogroupedlist(ScanReportInfoGroupedList *pointer)
{	
	CHILDREN(octet_string);
	CLV();
}

/**
 * Delete EventReportResultSimple
 *
 * @param *pointer
 */
void del_eventreportresultsimple(EventReportResultSimple *pointer)
{
	del_any(&pointer->event_reply_info);
}

/**
 * Delete TYPE
 *
 * @param *pointer
 */
void del_type(TYPE *pointer)
{
}

/**
 * Delete ObservationScanFixed
 *
 * @param *pointer
 */
void del_observationscanfixed(ObservationScanFixed *pointer)
{
	del_octet_string(&pointer->obs_val_data);
}

/**
 * Delete DataResponse
 *
 * @param *pointer
 */
void del_dataresponse(DataResponse *pointer)
{
	del_any(&pointer->event_info);
}

/**
 * Delete ProdSpecEntry
 *
 * @param *pointer
 */
void del_prodspecentry(ProdSpecEntry *pointer)
{
	del_octet_string(&pointer->prod_spec);
}

/**
 * Delete ScaleRangeSpec16
 *
 * @param *pointer
 */
void del_scalerangespec16(ScaleRangeSpec16 *pointer)
{
}

/**
 * Delete SegmEntryElem
 *
 * @param *pointer
 */
void del_segmentryelem(SegmEntryElem *pointer)
{
	del_type(&pointer->metric_type);
	del_attrvalmap(&pointer->attr_val_map);
}

/**
 * Delete ABRT_apdu
 *
 * @param *pointer
 */
void del_abrt_apdu(ABRT_apdu *pointer)
{
}

/**
 * Delete DataReqModeCapab
 *
 * @param *pointer
 */
void del_datareqmodecapab(DataReqModeCapab *pointer)
{
}

/**
 * Delete SupplementalTypeList
 *
 * @param *pointer
 */
void del_supplementaltypelist(SupplementalTypeList *pointer)
{
	CHILDREN(type);
	CLV();
}

/**
 * Delete ObservationScanFixedList
 *
 * @param *pointer
 */
void del_observationscanfixedlist(ObservationScanFixedList *pointer)
{
	CHILDREN(observationscanfixed);
	CLV();
}

/**
 * Delete TrigSegmDataXferRsp
 *
 * @param *pointer
 */
void del_trigsegmdataxferrsp(TrigSegmDataXferRsp *pointer)
{
}

/**
 * Delete DATA_apdu
 *
 * @param *pointer
 */
void del_data_apdu(DATA_apdu *pointer)
{
	del_data_apdu_message(&pointer->message);
}

/**
 * Delete AARQ_apdu
 *
 * @param *pointer
 */
void del_aarq_apdu(AARQ_apdu *pointer)
{
	del_dataprotolist(&pointer->data_proto_list);
}

/**
 * Delete TypeVerList
 *
 * @param *pointer
 */
void del_typeverlist(TypeVerList *pointer)
{
	CHILDREN(typever);
	CLV();
}

/**
 * Delete RegCertData
 *
 * @param *pointer
 */
void del_regcertdata(RegCertData *pointer)
{
	del_authbodyandstructype(&pointer->auth_body_and_struc_type);
	del_any(&pointer->auth_body_data);
}

/**
 * Delete NuObsValue
 *
 * @param *pointer
 */
void del_nuobsvalue(NuObsValue *pointer)
{
}

/**
 * Delete ScanReportPerVarList
 *
 * @param *pointer
 */
void del_scanreportpervarlist(ScanReportPerVarList *pointer)
{
	CHILDREN(scanreportpervar);
	CLV();
}

/**
 * Delete SetTimeInvoke
 *
 * @param *pointer
 */
void del_settimeinvoke(SetTimeInvoke *pointer)
{
	del_absolutetime(&pointer->date_time);
}

/**
 * Delete SegmentInfoList
 *
 * @param *pointer
 */
void del_segmentinfolist(SegmentInfoList *pointer)
{
	CHILDREN(segmentinfo);
	CLV();
}

/**
 * Delete ActionResultSimple
 *
 * @param *pointer
 */
void del_actionresultsimple(ActionResultSimple *pointer)
{
	del_any(&pointer->action_info_args);
}

/**
 * Delete SegmElemStaticAttrEntry
 *
 * @param *pointer
 */
void del_segmelemstaticattrentry(SegmElemStaticAttrEntry *pointer)
{
	del_type(&pointer->metric_type);
	del_attributelist(&pointer->attribute_list);
}

/**
 * Delete BasicNuObsValueCmp
 *
 * @param *pointer
 */
void del_basicnuobsvaluecmp(BasicNuObsValueCmp *pointer)
{
	CLV();
}

/**
 * Delete ConfigObjectList
 *
 * @param *pointer
 */
void del_configobjectlist(ConfigObjectList *pointer)
{
	CHILDREN(configobject);
	CLV();
}

/**
 * Delete SetResultSimple
 *
 * @param *pointer
 */
void del_setresultsimple(SetResultSimple *pointer)
{
	del_attributelist(&pointer->attribute_list);
}

/**
 * Delete HandleAttrValMapEntry
 *
 * @param *pointer
 */
void del_handleattrvalmapentry(HandleAttrValMapEntry *pointer)
{
	del_attrvalmap(&pointer->attr_val_map);
}

/**
 * Delete MetricSpecSmall
 *
 * @param *pointer
 */
void del_metricspecsmall(MetricSpecSmall *pointer)
{
}

/**
 * Delete ConfigId
 *
 * @param *pointer
 */
void del_configid(ConfigId *pointer)
{
}

/**
 * Delete SimpleNuObsValue
 *
 * @param *pointer
 */
void del_simplenuobsvalue(SimpleNuObsValue *pointer)
{
}

/** @} */
