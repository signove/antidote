/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file struct_cleaner.h
 * \brief Struct cleaner header.
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

#ifndef STRUCT_CLEANER_H_
#define STRUCT_CLEANER_H_

#include "../../asn1/phd_types.h"

void del_segmentdataresult(SegmentDataResult *pointer);
void del_scanreportpervar(ScanReportPerVar *pointer);
void del_typever(TypeVer *pointer);
void del_modificationlist(ModificationList *pointer);
void del_productionspec(ProductionSpec *pointer);
void del_actionargumentsimple(ActionArgumentSimple *pointer);
void del_scalerangespec32(ScaleRangeSpec32 *pointer);
void del_ava_type(AVA_Type *pointer);
void del_configreport(ConfigReport *pointer);
void del_attrvalmapentry(AttrValMapEntry *pointer);
void del_absolutetime(AbsoluteTime *pointer);
void del_nuobsvaluecmp(NuObsValueCmp *pointer);
void del_scanreportinfompfixed(ScanReportInfoMPFixed *pointer);
void del_rejectresult(RejectResult *pointer);
void del_manufspecassociationinformation(ManufSpecAssociationInformation *pointer);
void del_enumobsvalue(EnumObsValue *pointer);
void del_octet_string(octet_string *pointer);
void del_enumprintablestring(EnumPrintableString *pointer);
void del_highresrelativetime(HighResRelativeTime *pointer);
void del_sampletype(SampleType *pointer);
void del_attributelist(AttributeList *pointer);
void del_segmidlist(SegmIdList *pointer);
void del_simplenuobsvaluecmp(SimpleNuObsValueCmp *pointer);
void del_getresultsimple(GetResultSimple *pointer);
void del_handlelist(HANDLEList *pointer);
void del_segmdataeventdescr(SegmDataEventDescr *pointer);
void del_attrvalmap(AttrValMap *pointer);
void del_scalerangespec8(ScaleRangeSpec8 *pointer);
void del_phdassociationinformation(PhdAssociationInformation *pointer);
void del_scanreportperfixedlist(ScanReportPerFixedList *pointer);
void del_scanreportpergroupedlist(ScanReportPerGroupedList *pointer);
void del_dataprotolist(DataProtoList *pointer);
void del_segmselection(SegmSelection *pointer);
void del_errorresult(ErrorResult *pointer);
void del_handleattrvalmap(HandleAttrValMap *pointer);
void del_absolutetimeadjust(AbsoluteTimeAdjust *pointer);
void del_aare_apdu(AARE_apdu *pointer);
void del_rlre_apdu(RLRE_apdu *pointer);
void del_metricidlist(MetricIdList *pointer);
void del_scanreportperfixed(ScanReportPerFixed *pointer);
void del_scanreportinfogrouped(ScanReportInfoGrouped *pointer);
void del_observationscan(ObservationScan *pointer);
void del_scanreportpergrouped(ScanReportPerGrouped *pointer);
void del_systemmodel(SystemModel *pointer);
void del_observationscanlist(ObservationScanList *pointer);
void del_apdu(APDU *pointer);
void del_prst_apdu(PRST_apdu *pointer);
void del_pmsegmententrymap(PmSegmentEntryMap *pointer);
void del_any(Any *pointer);
void del_setargumentsimple(SetArgumentSimple *pointer);
void del_segmentinfo(SegmentInfo *pointer);
void del_pmsegmelemstaticattrlist(PmSegmElemStaticAttrList *pointer);
void del_abstimerange(AbsTimeRange *pointer);
void del_scanreportinfompvar(ScanReportInfoMPVar *pointer);
void del_uuid_ident(UUID_Ident *pointer);
void del_getargumentsimple(GetArgumentSimple *pointer);
void del_regcertdatalist(RegCertDataList *pointer);
void del_configreportrsp(ConfigReportRsp *pointer);
void del_dataproto(DataProto *pointer);
void del_metricstructuresmall(MetricStructureSmall *pointer);
void del_segmentstatisticentry(SegmentStatisticEntry *pointer);
void del_segmentdataevent(SegmentDataEvent *pointer);
void del_segmentryelemlist(SegmEntryElemList *pointer);
void del_saspec(SaSpec *pointer);
void del_attributemodentry(AttributeModEntry *pointer);
void del_mdstimeinfo(MdsTimeInfo *pointer);
void del_enumval(EnumVal *pointer);
void del_trigsegmdataxferreq(TrigSegmDataXferReq *pointer);
void del_batmeasure(BatMeasure *pointer);
void del_segmentstatistics(SegmentStatistics *pointer);
void del_attributeidlist(AttributeIdList *pointer);
void del_scanreportinfofixed(ScanReportInfoFixed *pointer);
void del_datarequest(DataRequest *pointer);
void del_authbodyandstructype(AuthBodyAndStrucType *pointer);
void del_rlrq_apdu(RLRQ_apdu *pointer);
void del_data_apdu_message(Data_apdu_message *pointer);
void del_eventreportargumentsimple(EventReportArgumentSimple *pointer);
void del_scanreportinfovar(ScanReportInfoVar *pointer);
void del_scanreportinfompgrouped(ScanReportInfoMPGrouped *pointer);
void del_configobject(ConfigObject *pointer);
void del_scanreportinfogroupedlist(ScanReportInfoGroupedList *pointer);
void del_eventreportresultsimple(EventReportResultSimple *pointer);
void del_type(TYPE *pointer);
void del_observationscanfixed(ObservationScanFixed *pointer);
void del_dataresponse(DataResponse *pointer);
void del_prodspecentry(ProdSpecEntry *pointer);
void del_scalerangespec16(ScaleRangeSpec16 *pointer);
void del_segmentryelem(SegmEntryElem *pointer);
void del_abrt_apdu(ABRT_apdu *pointer);
void del_datareqmodecapab(DataReqModeCapab *pointer);
void del_supplementaltypelist(SupplementalTypeList *pointer);
void del_observationscanfixedlist(ObservationScanFixedList *pointer);
void del_trigsegmdataxferrsp(TrigSegmDataXferRsp *pointer);
void del_data_apdu(DATA_apdu *pointer);
void del_aarq_apdu(AARQ_apdu *pointer);
void del_typeverlist(TypeVerList *pointer);
void del_regcertdata(RegCertData *pointer);
void del_nuobsvalue(NuObsValue *pointer);
void del_scanreportpervarlist(ScanReportPerVarList *pointer);
void del_settimeinvoke(SetTimeInvoke *pointer);
void del_segmentinfolist(SegmentInfoList *pointer);
void del_actionresultsimple(ActionResultSimple *pointer);
void del_segmelemstaticattrentry(SegmElemStaticAttrEntry *pointer);
void del_basicnuobsvaluecmp(BasicNuObsValueCmp *pointer);
void del_configobjectlist(ConfigObjectList *pointer);
void del_setresultsimple(SetResultSimple *pointer);
void del_handleattrvalmapentry(HandleAttrValMapEntry *pointer);
void del_metricspecsmall(MetricSpecSmall *pointer);
void del_configid(ConfigId *pointer);
void del_simplenuobsvalue(SimpleNuObsValue *pointer);

/** @} */

#endif /* STRUCT_CLEANER_H_ */

