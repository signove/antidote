/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file decoder_ASN1.h
 * \brief ASN1 decoder header.
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
 * IEEE 11073 Communication Model - Finite State Machine implementation
 *
 * \author Walter Guerra, Mateus Lima
 * \date Jun 11, 2010
 */

/**
 * \defgroup ASN1Decoder ASN1Decoder
 * \brief ASN1 decoder module.
 *
 * \ingroup Parser
 *
 * @{
 */

#ifndef DECODER_ASN1_H_
#define DECODER_ASN1_H_

#include "src/util/bytelib.h"


void decode_segmentdataresult(ByteStreamReader *stream, SegmentDataResult *pointer);
void decode_scanreportpervar(ByteStreamReader *stream, ScanReportPerVar *pointer);
void decode_typever(ByteStreamReader *stream, TypeVer *pointer);
void decode_modificationlist(ByteStreamReader *stream, ModificationList *pointer);
void decode_productionspec(ByteStreamReader *stream, ProductionSpec *pointer);
void decode_actionargumentsimple(ByteStreamReader *stream, ActionArgumentSimple *pointer);
void decode_scalerangespec32(ByteStreamReader *stream, ScaleRangeSpec32 *pointer);
void decode_ava_type(ByteStreamReader *stream, AVA_Type *pointer);
void decode_configreport(ByteStreamReader *stream, ConfigReport *pointer);
void decode_attrvalmapentry(ByteStreamReader *stream, AttrValMapEntry *pointer);
void decode_absolutetime(ByteStreamReader *stream, AbsoluteTime *pointer);
void decode_nuobsvaluecmp(ByteStreamReader *stream, NuObsValueCmp *pointer);
void decode_scanreportinfompfixed(ByteStreamReader *stream, ScanReportInfoMPFixed *pointer);
void decode_rejectresult(ByteStreamReader *stream, RejectResult *pointer);
void decode_manufspecassociationinformation(ByteStreamReader *stream, ManufSpecAssociationInformation *pointer);
void decode_enumobsvalue(ByteStreamReader *stream, EnumObsValue *pointer);
void decode_octet_string(ByteStreamReader *stream, octet_string *pointer);
void decode_highresrelativetime(ByteStreamReader *stream, HighResRelativeTime *pointer);
void decode_sampletype(ByteStreamReader *stream, SampleType *pointer);
void decode_attributelist(ByteStreamReader *stream, AttributeList *pointer);
void decode_segmidlist(ByteStreamReader *stream, SegmIdList *pointer);
void decode_simplenuobsvaluecmp(ByteStreamReader *stream, SimpleNuObsValueCmp *pointer);
void decode_getresultsimple(ByteStreamReader *stream, GetResultSimple *pointer);
void decode_handlelist(ByteStreamReader *stream, HANDLEList *pointer);
void decode_segmdataeventdescr(ByteStreamReader *stream, SegmDataEventDescr *pointer);
void decode_attrvalmap(ByteStreamReader *stream, AttrValMap *pointer);
void decode_scalerangespec8(ByteStreamReader *stream, ScaleRangeSpec8 *pointer);
void decode_phdassociationinformation(ByteStreamReader *stream, PhdAssociationInformation *pointer);
void decode_scanreportperfixedlist(ByteStreamReader *stream, ScanReportPerFixedList *pointer);
void decode_scanreportpergroupedlist(ByteStreamReader *stream, ScanReportPerGroupedList *pointer);
void decode_dataprotolist(ByteStreamReader *stream, DataProtoList *pointer);
void decode_segmselection(ByteStreamReader *stream, SegmSelection *pointer);
void decode_errorresult(ByteStreamReader *stream, ErrorResult *pointer);
void decode_handleattrvalmap(ByteStreamReader *stream, HandleAttrValMap *pointer);
void decode_absolutetimeadjust(ByteStreamReader *stream, AbsoluteTimeAdjust *pointer);
void decode_aare_apdu(ByteStreamReader *stream, AARE_apdu *pointer);
void decode_rlre_apdu(ByteStreamReader *stream, RLRE_apdu *pointer);
void decode_metricidlist(ByteStreamReader *stream, MetricIdList *pointer);
void decode_scanreportperfixed(ByteStreamReader *stream, ScanReportPerFixed *pointer);
void decode_scanreportinfogrouped(ByteStreamReader *stream, ScanReportInfoGrouped *pointer);
void decode_observationscan(ByteStreamReader *stream, ObservationScan *pointer);
void decode_scanreportpergrouped(ByteStreamReader *stream, ScanReportPerGrouped *pointer);
void decode_systemmodel(ByteStreamReader *stream, SystemModel *pointer);
void decode_observationscanlist(ByteStreamReader *stream, ObservationScanList *pointer);
void decode_apdu(ByteStreamReader *stream, APDU *pointer);
void decode_prst_apdu(ByteStreamReader *stream, PRST_apdu *pointer);
void decode_pmsegmententrymap(ByteStreamReader *stream, PmSegmentEntryMap *pointer);
void decode_any(ByteStreamReader *stream, Any *pointer);
void decode_setargumentsimple(ByteStreamReader *stream, SetArgumentSimple *pointer);
void decode_segmentinfo(ByteStreamReader *stream, SegmentInfo *pointer);
void decode_pmsegmelemstaticattrlist(ByteStreamReader *stream, PmSegmElemStaticAttrList *pointer);
void decode_abstimerange(ByteStreamReader *stream, AbsTimeRange *pointer);
void decode_scanreportinfompvar(ByteStreamReader *stream, ScanReportInfoMPVar *pointer);
void decode_uuid_ident(ByteStreamReader *stream, UUID_Ident *pointer);
void decode_getargumentsimple(ByteStreamReader *stream, GetArgumentSimple *pointer);
void decode_regcertdatalist(ByteStreamReader *stream, RegCertDataList *pointer);
void decode_configreportrsp(ByteStreamReader *stream, ConfigReportRsp *pointer);
void decode_dataproto(ByteStreamReader *stream, DataProto *pointer);
void decode_metricstructuresmall(ByteStreamReader *stream, MetricStructureSmall *pointer);
void decode_segmentstatisticentry(ByteStreamReader *stream, SegmentStatisticEntry *pointer);
void decode_segmentdataevent(ByteStreamReader *stream, SegmentDataEvent *pointer);
void decode_segmentryelemlist(ByteStreamReader *stream, SegmEntryElemList *pointer);
void decode_saspec(ByteStreamReader *stream, SaSpec *pointer);
void decode_attributemodentry(ByteStreamReader *stream, AttributeModEntry *pointer);
void decode_mdstimeinfo(ByteStreamReader *stream, MdsTimeInfo *pointer);
void decode_enumval(ByteStreamReader *stream, EnumVal *pointer);
void decode_trigsegmdataxferreq(ByteStreamReader *stream, TrigSegmDataXferReq *pointer);
void decode_batmeasure(ByteStreamReader *stream, BatMeasure *pointer);
void decode_segmentstatistics(ByteStreamReader *stream, SegmentStatistics *pointer);
void decode_attributeidlist(ByteStreamReader *stream, AttributeIdList *pointer);
void decode_scanreportinfofixed(ByteStreamReader *stream, ScanReportInfoFixed *pointer);
void decode_datarequest(ByteStreamReader *stream, DataRequest *pointer);
void decode_authbodyandstructype(ByteStreamReader *stream, AuthBodyAndStrucType *pointer);
void decode_rlrq_apdu(ByteStreamReader *stream, RLRQ_apdu *pointer);
void decode_data_apdu_message(ByteStreamReader *stream, Data_apdu_message *pointer);
void decode_eventreportargumentsimple(ByteStreamReader *stream, EventReportArgumentSimple *pointer);
void decode_scanreportinfovar(ByteStreamReader *stream, ScanReportInfoVar *pointer);
void decode_scanreportinfompgrouped(ByteStreamReader *stream, ScanReportInfoMPGrouped *pointer);
void decode_configobject(ByteStreamReader *stream, ConfigObject *pointer);
void decode_scanreportinfogroupedlist(ByteStreamReader *stream, ScanReportInfoGroupedList *pointer);
void decode_eventreportresultsimple(ByteStreamReader *stream, EventReportResultSimple *pointer);
void decode_type(ByteStreamReader *stream, TYPE *pointer);
void decode_observationscanfixed(ByteStreamReader *stream, ObservationScanFixed *pointer);
void decode_dataresponse(ByteStreamReader *stream, DataResponse *pointer);
void decode_prodspecentry(ByteStreamReader *stream, ProdSpecEntry *pointer);
void decode_scalerangespec16(ByteStreamReader *stream, ScaleRangeSpec16 *pointer);
void decode_segmentryelem(ByteStreamReader *stream, SegmEntryElem *pointer);
void decode_abrt_apdu(ByteStreamReader *stream, ABRT_apdu *pointer);
void decode_datareqmodecapab(ByteStreamReader *stream, DataReqModeCapab *pointer);
void decode_supplementaltypelist(ByteStreamReader *stream, SupplementalTypeList *pointer);
void decode_observationscanfixedlist(ByteStreamReader *stream, ObservationScanFixedList *pointer);
void decode_trigsegmdataxferrsp(ByteStreamReader *stream, TrigSegmDataXferRsp *pointer);
void decode_data_apdu(ByteStreamReader *stream, DATA_apdu *pointer);
void decode_aarq_apdu(ByteStreamReader *stream, AARQ_apdu *pointer);
void decode_typeverlist(ByteStreamReader *stream, TypeVerList *pointer);
void decode_regcertdata(ByteStreamReader *stream, RegCertData *pointer);
void decode_nuobsvalue(ByteStreamReader *stream, NuObsValue *pointer);
void decode_scanreportpervarlist(ByteStreamReader *stream, ScanReportPerVarList *pointer);
void decode_settimeinvoke(ByteStreamReader *stream, SetTimeInvoke *pointer);
void decode_segmentinfolist(ByteStreamReader *stream, SegmentInfoList *pointer);
void decode_actionresultsimple(ByteStreamReader *stream, ActionResultSimple *pointer);
void decode_segmelemstaticattrentry(ByteStreamReader *stream, SegmElemStaticAttrEntry *pointer);
void decode_basicnuobsvaluecmp(ByteStreamReader *stream, BasicNuObsValueCmp *pointer);
void decode_configobjectlist(ByteStreamReader *stream, ConfigObjectList *pointer);
void decode_setresultsimple(ByteStreamReader *stream, SetResultSimple *pointer);
void decode_handleattrvalmapentry(ByteStreamReader *stream, HandleAttrValMapEntry *pointer);
void decode_simplenuobsvalue(ByteStreamReader *stream, SimpleNuObsValue *pointer);
void decode_metricspecsmall(ByteStreamReader *stream, MetricSpecSmall *pointer);
void decode_configid(ByteStreamReader *stream, ConfigId *pointer);

/** @} */

#endif /* DECODER_ASN1_H_ */
