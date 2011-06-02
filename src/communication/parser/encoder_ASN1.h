/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file encoder_ASN1.h
 * \brief ASN1 encoder definitions.
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
 * \defgroup ASN1Encoder ASN1Encoder
 * \brief ASN1 encoder.
 * \ingroup Parser
 *
 * @{
 */

#ifndef ENCODER_ASN1_H_
#define ENCODER_ASN1_H_

#include "src/util/bytelib.h"

void encode_segmentdataresult(ByteStreamWriter *stream, SegmentDataResult *pointer);
void encode_scanreportpervar(ByteStreamWriter *stream, ScanReportPerVar *pointer);
void encode_typever(ByteStreamWriter *stream, TypeVer *pointer);
void encode_modificationlist(ByteStreamWriter *stream, ModificationList *pointer);
void encode_productionspec(ByteStreamWriter *stream, ProductionSpec *pointer);
void encode_actionargumentsimple(ByteStreamWriter *stream, ActionArgumentSimple *pointer);
void encode_scalerangespec32(ByteStreamWriter *stream, ScaleRangeSpec32 *pointer);
void encode_ava_type(ByteStreamWriter *stream, AVA_Type *pointer);
void encode_configreport(ByteStreamWriter *stream, ConfigReport *pointer);
void encode_attrvalmapentry(ByteStreamWriter *stream, AttrValMapEntry *pointer);
void encode_absolutetime(ByteStreamWriter *stream, AbsoluteTime *pointer);
void encode_nuobsvaluecmp(ByteStreamWriter *stream, NuObsValueCmp *pointer);
void encode_scanreportinfompfixed(ByteStreamWriter *stream, ScanReportInfoMPFixed *pointer);
void encode_rejectresult(ByteStreamWriter *stream, RejectResult *pointer);
void encode_manufspecassociationinformation(ByteStreamWriter *stream, ManufSpecAssociationInformation *pointer);
void encode_enumobsvalue(ByteStreamWriter *stream, EnumObsValue *pointer);
void encode_octet_string(ByteStreamWriter *stream, octet_string *pointer);
void encode_highresrelativetime(ByteStreamWriter *stream, HighResRelativeTime *pointer);
void encode_sampletype(ByteStreamWriter *stream, SampleType *pointer);
void encode_attributelist(ByteStreamWriter *stream, AttributeList *pointer);
void encode_segmidlist(ByteStreamWriter *stream, SegmIdList *pointer);
void encode_simplenuobsvaluecmp(ByteStreamWriter *stream, SimpleNuObsValueCmp *pointer);
void encode_getresultsimple(ByteStreamWriter *stream, GetResultSimple *pointer);
void encode_handlelist(ByteStreamWriter *stream, HANDLEList *pointer);
void encode_segmdataeventdescr(ByteStreamWriter *stream, SegmDataEventDescr *pointer);
void encode_attrvalmap(ByteStreamWriter *stream, AttrValMap *pointer);
void encode_scalerangespec8(ByteStreamWriter *stream, ScaleRangeSpec8 *pointer);
void encode_phdassociationinformation(ByteStreamWriter *stream, PhdAssociationInformation *pointer);
void encode_scanreportperfixedlist(ByteStreamWriter *stream, ScanReportPerFixedList *pointer);
void encode_scanreportpergroupedlist(ByteStreamWriter *stream, ScanReportPerGroupedList *pointer);
void encode_dataprotolist(ByteStreamWriter *stream, DataProtoList *pointer);
void encode_segmselection(ByteStreamWriter *stream, SegmSelection *pointer);
void encode_errorresult(ByteStreamWriter *stream, ErrorResult *pointer);
void encode_handleattrvalmap(ByteStreamWriter *stream, HandleAttrValMap *pointer);
void encode_absolutetimeadjust(ByteStreamWriter *stream, AbsoluteTimeAdjust *pointer);
void encode_aare_apdu(ByteStreamWriter *stream, AARE_apdu *pointer);
void encode_rlre_apdu(ByteStreamWriter *stream, RLRE_apdu *pointer);
void encode_metricidlist(ByteStreamWriter *stream, MetricIdList *pointer);
void encode_scanreportperfixed(ByteStreamWriter *stream, ScanReportPerFixed *pointer);
void encode_scanreportinfogrouped(ByteStreamWriter *stream, ScanReportInfoGrouped *pointer);
void encode_observationscan(ByteStreamWriter *stream, ObservationScan *pointer);
void encode_scanreportpergrouped(ByteStreamWriter *stream, ScanReportPerGrouped *pointer);
void encode_systemmodel(ByteStreamWriter *stream, SystemModel *pointer);
void encode_observationscanlist(ByteStreamWriter *stream, ObservationScanList *pointer);
void encode_set_data_apdu(PRST_apdu *prst, DATA_apdu *data_apdu);
DATA_apdu *encode_get_data_apdu(PRST_apdu *prst);
void encode_apdu(ByteStreamWriter *stream, APDU *pointer);
void encode_prst_apdu(ByteStreamWriter *stream, PRST_apdu *pointer);
void encode_pmsegmententrymap(ByteStreamWriter *stream, PmSegmentEntryMap *pointer);
void encode_any(ByteStreamWriter *stream, Any *pointer);
void encode_setargumentsimple(ByteStreamWriter *stream, SetArgumentSimple *pointer);
void encode_segmentinfo(ByteStreamWriter *stream, SegmentInfo *pointer);
void encode_pmsegmelemstaticattrlist(ByteStreamWriter *stream, PmSegmElemStaticAttrList *pointer);
void encode_abstimerange(ByteStreamWriter *stream, AbsTimeRange *pointer);
void encode_scanreportinfompvar(ByteStreamWriter *stream, ScanReportInfoMPVar *pointer);
void encode_uuid_ident(ByteStreamWriter *stream, UUID_Ident *pointer);
void encode_getargumentsimple(ByteStreamWriter *stream, GetArgumentSimple *pointer);
void encode_regcertdatalist(ByteStreamWriter *stream, RegCertDataList *pointer);
void encode_configreportrsp(ByteStreamWriter *stream, ConfigReportRsp *pointer);
void encode_dataproto(ByteStreamWriter *stream, DataProto *pointer);
void encode_metricstructuresmall(ByteStreamWriter *stream, MetricStructureSmall *pointer);
void encode_segmentstatisticentry(ByteStreamWriter *stream, SegmentStatisticEntry *pointer);
void encode_segmentdataevent(ByteStreamWriter *stream, SegmentDataEvent *pointer);
void encode_segmentryelemlist(ByteStreamWriter *stream, SegmEntryElemList *pointer);
void encode_saspec(ByteStreamWriter *stream, SaSpec *pointer);
void encode_attributemodentry(ByteStreamWriter *stream, AttributeModEntry *pointer);
void encode_mdstimeinfo(ByteStreamWriter *stream, MdsTimeInfo *pointer);
void encode_enumval(ByteStreamWriter *stream, EnumVal *pointer);
void encode_trigsegmdataxferreq(ByteStreamWriter *stream, TrigSegmDataXferReq *pointer);
void encode_batmeasure(ByteStreamWriter *stream, BatMeasure *pointer);
void encode_segmentstatistics(ByteStreamWriter *stream, SegmentStatistics *pointer);
void encode_attributeidlist(ByteStreamWriter *stream, AttributeIdList *pointer);
void encode_scanreportinfofixed(ByteStreamWriter *stream, ScanReportInfoFixed *pointer);
void encode_datarequest(ByteStreamWriter *stream, DataRequest *pointer);
void encode_authbodyandstructype(ByteStreamWriter *stream, AuthBodyAndStrucType *pointer);
void encode_rlrq_apdu(ByteStreamWriter *stream, RLRQ_apdu *pointer);
void encode_data_apdu_message(ByteStreamWriter *stream, Data_apdu_message *pointer);
void encode_eventreportargumentsimple(ByteStreamWriter *stream, EventReportArgumentSimple *pointer);
void encode_scanreportinfovar(ByteStreamWriter *stream, ScanReportInfoVar *pointer);
void encode_scanreportinfompgrouped(ByteStreamWriter *stream, ScanReportInfoMPGrouped *pointer);
void encode_configobject(ByteStreamWriter *stream, ConfigObject *pointer);
void encode_scanreportinfogroupedlist(ByteStreamWriter *stream, ScanReportInfoGroupedList *pointer);
void encode_eventreportresultsimple(ByteStreamWriter *stream, EventReportResultSimple *pointer);
void encode_type(ByteStreamWriter *stream, TYPE *pointer);
void encode_observationscanfixed(ByteStreamWriter *stream, ObservationScanFixed *pointer);
void encode_dataresponse(ByteStreamWriter *stream, DataResponse *pointer);
void encode_prodspecentry(ByteStreamWriter *stream, ProdSpecEntry *pointer);
void encode_scalerangespec16(ByteStreamWriter *stream, ScaleRangeSpec16 *pointer);
void encode_segmentryelem(ByteStreamWriter *stream, SegmEntryElem *pointer);
void encode_abrt_apdu(ByteStreamWriter *stream, ABRT_apdu *pointer);
void encode_datareqmodecapab(ByteStreamWriter *stream, DataReqModeCapab *pointer);
void encode_supplementaltypelist(ByteStreamWriter *stream, SupplementalTypeList *pointer);
void encode_observationscanfixedlist(ByteStreamWriter *stream, ObservationScanFixedList *pointer);
void encode_trigsegmdataxferrsp(ByteStreamWriter *stream, TrigSegmDataXferRsp *pointer);
void encode_data_apdu(ByteStreamWriter *stream, DATA_apdu *pointer);
void encode_aarq_apdu(ByteStreamWriter *stream, AARQ_apdu *pointer);
void encode_typeverlist(ByteStreamWriter *stream, TypeVerList *pointer);
void encode_regcertdata(ByteStreamWriter *stream, RegCertData *pointer);
void encode_nuobsvalue(ByteStreamWriter *stream, NuObsValue *pointer);
void encode_scanreportpervarlist(ByteStreamWriter *stream, ScanReportPerVarList *pointer);
void encode_settimeinvoke(ByteStreamWriter *stream, SetTimeInvoke *pointer);
void encode_segmentinfolist(ByteStreamWriter *stream, SegmentInfoList *pointer);
void encode_actionresultsimple(ByteStreamWriter *stream, ActionResultSimple *pointer);
void encode_segmelemstaticattrentry(ByteStreamWriter *stream, SegmElemStaticAttrEntry *pointer);
void encode_basicnuobsvalue(ByteStreamWriter *stream, BasicNuObsValue *pointer);
void encode_basicnuobsvaluecmp(ByteStreamWriter *stream, BasicNuObsValueCmp *pointer);
void encode_configobjectlist(ByteStreamWriter *stream, ConfigObjectList *pointer);
void encode_setresultsimple(ByteStreamWriter *stream, SetResultSimple *pointer);
void encode_handleattrvalmapentry(ByteStreamWriter *stream, HandleAttrValMapEntry *pointer);
void encode_simplenuobsvalue(ByteStreamWriter *stream, SimpleNuObsValue *pointer);
void encode_metricspecsmall(ByteStreamWriter *stream, MetricSpecSmall *pointer);
void encode_configid(ByteStreamWriter *stream, ConfigId *pointer);

/** @} */

#endif /* ENCODER_ASN1_H_ */
