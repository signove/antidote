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
 * \addtogroup ASN1Codec
 *
 * @{
 */

#ifndef DECODER_ASN1_H_
#define DECODER_ASN1_H_

#include "src/util/bytelib.h"

void decode_segmentdataresult(ByteStreamReader *stream, SegmentDataResult *pointer, int *error);
void decode_scanreportpervar(ByteStreamReader *stream, ScanReportPerVar *pointer, int *error);
void decode_typever(ByteStreamReader *stream, TypeVer *pointer, int *error);
void decode_modificationlist(ByteStreamReader *stream, ModificationList *pointer, int *error);
void decode_productionspec(ByteStreamReader *stream, ProductionSpec *pointer, int *error);
void decode_actionargumentsimple(ByteStreamReader *stream, ActionArgumentSimple *pointer, int *error);
void decode_scalerangespec32(ByteStreamReader *stream, ScaleRangeSpec32 *pointer, int *error);
void decode_ava_type(ByteStreamReader *stream, AVA_Type *pointer, int *error);
void decode_configreport(ByteStreamReader *stream, ConfigReport *pointer, int *error);
void decode_attrvalmapentry(ByteStreamReader *stream, AttrValMapEntry *pointer, int *error);
void decode_absolutetime(ByteStreamReader *stream, AbsoluteTime *pointer, int *error);
void decode_nuobsvaluecmp(ByteStreamReader *stream, NuObsValueCmp *pointer, int *error);
void decode_scanreportinfompfixed(ByteStreamReader *stream, ScanReportInfoMPFixed *pointer, int *error);
void decode_rejectresult(ByteStreamReader *stream, RejectResult *pointer, int *error);
void decode_manufspecassociationinformation(ByteStreamReader *stream, ManufSpecAssociationInformation *pointer, int *error);
void decode_enumobsvalue(ByteStreamReader *stream, EnumObsValue *pointer, int *error);
void decode_octet_string(ByteStreamReader *stream, octet_string *pointer, int *error);
void decode_highresrelativetime(ByteStreamReader *stream, HighResRelativeTime *pointer, int *error);
void decode_sampletype(ByteStreamReader *stream, SampleType *pointer, int *error);
void decode_attributelist(ByteStreamReader *stream, AttributeList *pointer, int *err);
void decode_segmidlist(ByteStreamReader *stream, SegmIdList *pointer, int *error);
void decode_simplenuobsvaluecmp(ByteStreamReader *stream, SimpleNuObsValueCmp *pointer, int *error);
void decode_getresultsimple(ByteStreamReader *stream, GetResultSimple *pointer, int *error);
void decode_handlelist(ByteStreamReader *stream, HANDLEList *pointer, int *error);
void decode_segmdataeventdescr(ByteStreamReader *stream, SegmDataEventDescr *pointer, int *error);
void decode_attrvalmap(ByteStreamReader *stream, AttrValMap *pointer, int *error);
void decode_scalerangespec8(ByteStreamReader *stream, ScaleRangeSpec8 *pointer, int *error);
void decode_phdassociationinformation(ByteStreamReader *stream, PhdAssociationInformation *pointer, int *error);
void decode_scanreportperfixedlist(ByteStreamReader *stream, ScanReportPerFixedList *pointer, int *error);
void decode_scanreportpergroupedlist(ByteStreamReader *stream, ScanReportPerGroupedList *pointer, int *error);
void decode_dataprotolist(ByteStreamReader *stream, DataProtoList *pointer, int *error);
void decode_segmselection(ByteStreamReader *stream, SegmSelection *pointer, int *error);
void decode_errorresult(ByteStreamReader *stream, ErrorResult *pointer, int *error);
void decode_handleattrvalmap(ByteStreamReader *stream, HandleAttrValMap *pointer, int *error);
void decode_absolutetimeadjust(ByteStreamReader *stream, AbsoluteTimeAdjust *pointer, int *error);
void decode_aare_apdu(ByteStreamReader *stream, AARE_apdu *pointer, int *error);
void decode_rlre_apdu(ByteStreamReader *stream, RLRE_apdu *pointer, int *error);
void decode_metricidlist(ByteStreamReader *stream, MetricIdList *pointer, int *error);
void decode_scanreportperfixed(ByteStreamReader *stream, ScanReportPerFixed *pointer, int *error);
void decode_scanreportinfogrouped(ByteStreamReader *stream, ScanReportInfoGrouped *pointer, int *error);
void decode_observationscan(ByteStreamReader *stream, ObservationScan *pointer, int *error);
void decode_scanreportpergrouped(ByteStreamReader *stream, ScanReportPerGrouped *pointer, int *error);
void decode_systemmodel(ByteStreamReader *stream, SystemModel *pointer, int *error);
void decode_observationscanlist(ByteStreamReader *stream, ObservationScanList *pointer, int *error);
void decode_apdu(ByteStreamReader *stream, APDU *pointer, int *error);
void decode_prst_apdu(ByteStreamReader *stream, PRST_apdu *pointer, int *error);
void decode_pmsegmententrymap(ByteStreamReader *stream, PmSegmentEntryMap *pointer, int *error);
void decode_any(ByteStreamReader *stream, Any *pointer, int *error);
void decode_setargumentsimple(ByteStreamReader *stream, SetArgumentSimple *pointer, int *error);
void decode_segmentinfo(ByteStreamReader *stream, SegmentInfo *pointer, int *error);
void decode_pmsegmelemstaticattrlist(ByteStreamReader *stream, PmSegmElemStaticAttrList *pointer, int *error);
void decode_abstimerange(ByteStreamReader *stream, AbsTimeRange *pointer, int *error);
void decode_scanreportinfompvar(ByteStreamReader *stream, ScanReportInfoMPVar *pointer, int *error);
void decode_uuid_ident(ByteStreamReader *stream, UUID_Ident *pointer, int *error);
void decode_getargumentsimple(ByteStreamReader *stream, GetArgumentSimple *pointer, int *error);
void decode_regcertdatalist(ByteStreamReader *stream, RegCertDataList *pointer, int *error);
void decode_configreportrsp(ByteStreamReader *stream, ConfigReportRsp *pointer, int *error);
void decode_dataproto(ByteStreamReader *stream, DataProto *pointer, int *error);
void decode_metricstructuresmall(ByteStreamReader *stream, MetricStructureSmall *pointer, int *error);
void decode_segmentstatisticentry(ByteStreamReader *stream, SegmentStatisticEntry *pointer, int *error);
void decode_segmentdataevent(ByteStreamReader *stream, SegmentDataEvent *pointer, int *error);
void decode_segmentryelemlist(ByteStreamReader *stream, SegmEntryElemList *pointer, int *error);
void decode_saspec(ByteStreamReader *stream, SaSpec *pointer, int *error);
void decode_attributemodentry(ByteStreamReader *stream, AttributeModEntry *pointer, int *error);
void decode_mdstimeinfo(ByteStreamReader *stream, MdsTimeInfo *pointer, int *error);
void decode_enumval(ByteStreamReader *stream, EnumVal *pointer, int *error);
void decode_trigsegmdataxferreq(ByteStreamReader *stream, TrigSegmDataXferReq *pointer, int *error);
void decode_batmeasure(ByteStreamReader *stream, BatMeasure *pointer, int *error);
void decode_segmentstatistics(ByteStreamReader *stream, SegmentStatistics *pointer, int *error);
void decode_attributeidlist(ByteStreamReader *stream, AttributeIdList *pointer, int *error);
void decode_scanreportinfofixed(ByteStreamReader *stream, ScanReportInfoFixed *pointer, int *error);
void decode_datarequest(ByteStreamReader *stream, DataRequest *pointer, int *error);
void decode_authbodyandstructype(ByteStreamReader *stream, AuthBodyAndStrucType *pointer, int *error);
void decode_rlrq_apdu(ByteStreamReader *stream, RLRQ_apdu *pointer, int *error);
void decode_data_apdu_message(ByteStreamReader *stream, Data_apdu_message *pointer, int *error);
void decode_eventreportargumentsimple(ByteStreamReader *stream, EventReportArgumentSimple *pointer, int *error);
void decode_scanreportinfovar(ByteStreamReader *stream, ScanReportInfoVar *pointer, int *error);
void decode_scanreportinfompgrouped(ByteStreamReader *stream, ScanReportInfoMPGrouped *pointer, int *error);
void decode_configobject(ByteStreamReader *stream, ConfigObject *pointer, int* error);
void decode_scanreportinfogroupedlist(ByteStreamReader *stream, ScanReportInfoGroupedList *pointer, int *error);
void decode_eventreportresultsimple(ByteStreamReader *stream, EventReportResultSimple *pointer, int *error);
void decode_type(ByteStreamReader *stream, TYPE *pointer, int *error);
void decode_observationscanfixed(ByteStreamReader *stream, ObservationScanFixed *pointer, int *error);
void decode_dataresponse(ByteStreamReader *stream, DataResponse *pointer, int *error);
void decode_prodspecentry(ByteStreamReader *stream, ProdSpecEntry *pointer, int *error);
void decode_scalerangespec16(ByteStreamReader *stream, ScaleRangeSpec16 *pointer, int *error);
void decode_segmentryelem(ByteStreamReader *stream, SegmEntryElem *pointer, int *error);
void decode_abrt_apdu(ByteStreamReader *stream, ABRT_apdu *pointer, int *error);
void decode_datareqmodecapab(ByteStreamReader *stream, DataReqModeCapab *pointer, int *error);
void decode_supplementaltypelist(ByteStreamReader *stream, SupplementalTypeList *pointer, int *error);
void decode_observationscanfixedlist(ByteStreamReader *stream, ObservationScanFixedList *pointer, int *error);
void decode_trigsegmdataxferrsp(ByteStreamReader *stream, TrigSegmDataXferRsp *pointer, int *error);
void decode_data_apdu(ByteStreamReader *stream, DATA_apdu *pointer, int *error);
void decode_aarq_apdu(ByteStreamReader *stream, AARQ_apdu *pointer, int *error);
void decode_typeverlist(ByteStreamReader *stream, TypeVerList *pointer, int *error);
void decode_regcertdata(ByteStreamReader *stream, RegCertData *pointer, int *error);
void decode_nuobsvalue(ByteStreamReader *stream, NuObsValue *pointer, int *error);
void decode_scanreportpervarlist(ByteStreamReader *stream, ScanReportPerVarList *pointer, int *error);
void decode_settimeinvoke(ByteStreamReader *stream, SetTimeInvoke *pointer, int *error);
void decode_segmentinfolist(ByteStreamReader *stream, SegmentInfoList *pointer, int *error);
void decode_actionresultsimple(ByteStreamReader *stream, ActionResultSimple *pointer, int *error);
void decode_segmelemstaticattrentry(ByteStreamReader *stream, SegmElemStaticAttrEntry *pointer, int *error);
void decode_basicnuobsvaluecmp(ByteStreamReader *stream, BasicNuObsValueCmp *pointer, int *error);
void decode_configobjectlist(ByteStreamReader *stream, ConfigObjectList *pointer, int *error);
void decode_setresultsimple(ByteStreamReader *stream, SetResultSimple *pointer, int *error);
void decode_handleattrvalmapentry(ByteStreamReader *stream, HandleAttrValMapEntry *pointer, int *error);
void decode_simplenuobsvalue(ByteStreamReader *stream, SimpleNuObsValue *pointer, int *error);
void decode_metricspecsmall(ByteStreamReader *stream, MetricSpecSmall *pointer, int *error);
void decode_configid(ByteStreamReader *stream, ConfigId *pointer, int *error);

/** @} */

#endif /* DECODER_ASN1_H_ */
