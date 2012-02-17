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
 * \addtogroup ASN1Codec
 *
 * @{
 */

#ifndef ENCODER_ASN1_H_
#define ENCODER_ASN1_H_

#include "src/util/bytelib.h"

int encode_segmentdataresult(ByteStreamWriter *stream, SegmentDataResult *pointer);
int encode_scanreportpervar(ByteStreamWriter *stream, ScanReportPerVar *pointer);
int encode_typever(ByteStreamWriter *stream, TypeVer *pointer);
int encode_modificationlist(ByteStreamWriter *stream, ModificationList *pointer);
int encode_productionspec(ByteStreamWriter *stream, ProductionSpec *pointer);
int encode_actionargumentsimple(ByteStreamWriter *stream, ActionArgumentSimple *pointer);
int encode_scalerangespec32(ByteStreamWriter *stream, ScaleRangeSpec32 *pointer);
int encode_ava_type(ByteStreamWriter *stream, AVA_Type *pointer);
int encode_configreport(ByteStreamWriter *stream, ConfigReport *pointer);
int encode_attrvalmapentry(ByteStreamWriter *stream, AttrValMapEntry *pointer);
int encode_absolutetime(ByteStreamWriter *stream, AbsoluteTime *pointer);
int encode_nuobsvaluecmp(ByteStreamWriter *stream, NuObsValueCmp *pointer);
int encode_scanreportinfompfixed(ByteStreamWriter *stream, ScanReportInfoMPFixed *pointer);
int encode_rejectresult(ByteStreamWriter *stream, RejectResult *pointer);
int encode_manufspecassociationinformation(ByteStreamWriter *stream, ManufSpecAssociationInformation *pointer);
int encode_enumobsvalue(ByteStreamWriter *stream, EnumObsValue *pointer);
int encode_octet_string(ByteStreamWriter *stream, octet_string *pointer);
int encode_highresrelativetime(ByteStreamWriter *stream, HighResRelativeTime *pointer);
int encode_sampletype(ByteStreamWriter *stream, SampleType *pointer);
int encode_attributelist(ByteStreamWriter *stream, AttributeList *pointer);
int encode_segmidlist(ByteStreamWriter *stream, SegmIdList *pointer);
int encode_simplenuobsvaluecmp(ByteStreamWriter *stream, SimpleNuObsValueCmp *pointer);
int encode_getresultsimple(ByteStreamWriter *stream, GetResultSimple *pointer);
int encode_handlelist(ByteStreamWriter *stream, HANDLEList *pointer);
int encode_segmdataeventdescr(ByteStreamWriter *stream, SegmDataEventDescr *pointer);
int encode_attrvalmap(ByteStreamWriter *stream, AttrValMap *pointer);
int encode_scalerangespec8(ByteStreamWriter *stream, ScaleRangeSpec8 *pointer);
int encode_phdassociationinformation(ByteStreamWriter *stream, PhdAssociationInformation *pointer);
int encode_scanreportperfixedlist(ByteStreamWriter *stream, ScanReportPerFixedList *pointer);
int encode_scanreportpergroupedlist(ByteStreamWriter *stream, ScanReportPerGroupedList *pointer);
int encode_dataprotolist(ByteStreamWriter *stream, DataProtoList *pointer);
int encode_segmselection(ByteStreamWriter *stream, SegmSelection *pointer);
int encode_errorresult(ByteStreamWriter *stream, ErrorResult *pointer);
int encode_handleattrvalmap(ByteStreamWriter *stream, HandleAttrValMap *pointer);
int encode_absolutetimeadjust(ByteStreamWriter *stream, AbsoluteTimeAdjust *pointer);
int encode_aare_apdu(ByteStreamWriter *stream, AARE_apdu *pointer);
int encode_rlre_apdu(ByteStreamWriter *stream, RLRE_apdu *pointer);
int encode_metricidlist(ByteStreamWriter *stream, MetricIdList *pointer);
int encode_scanreportperfixed(ByteStreamWriter *stream, ScanReportPerFixed *pointer);
int encode_scanreportinfogrouped(ByteStreamWriter *stream, ScanReportInfoGrouped *pointer);
int encode_observationscan(ByteStreamWriter *stream, ObservationScan *pointer);
int encode_scanreportpergrouped(ByteStreamWriter *stream, ScanReportPerGrouped *pointer);
int encode_systemmodel(ByteStreamWriter *stream, SystemModel *pointer);
int encode_observationscanlist(ByteStreamWriter *stream, ObservationScanList *pointer);
int encode_set_data_apdu(PRST_apdu *prst, DATA_apdu *data_apdu);
DATA_apdu *encode_get_data_apdu(PRST_apdu *prst);
int encode_apdu(ByteStreamWriter *stream, APDU *pointer);
int encode_prst_apdu(ByteStreamWriter *stream, PRST_apdu *pointer);
int encode_pmsegmententrymap(ByteStreamWriter *stream, PmSegmentEntryMap *pointer);
int encode_any(ByteStreamWriter *stream, Any *pointer);
int encode_setargumentsimple(ByteStreamWriter *stream, SetArgumentSimple *pointer);
int encode_segmentinfo(ByteStreamWriter *stream, SegmentInfo *pointer);
int encode_pmsegmelemstaticattrlist(ByteStreamWriter *stream, PmSegmElemStaticAttrList *pointer);
int encode_abstimerange(ByteStreamWriter *stream, AbsTimeRange *pointer);
int encode_scanreportinfompvar(ByteStreamWriter *stream, ScanReportInfoMPVar *pointer);
int encode_uuid_ident(ByteStreamWriter *stream, UUID_Ident *pointer);
int encode_getargumentsimple(ByteStreamWriter *stream, GetArgumentSimple *pointer);
int encode_regcertdatalist(ByteStreamWriter *stream, RegCertDataList *pointer);
int encode_configreportrsp(ByteStreamWriter *stream, ConfigReportRsp *pointer);
int encode_dataproto(ByteStreamWriter *stream, DataProto *pointer);
int encode_metricstructuresmall(ByteStreamWriter *stream, MetricStructureSmall *pointer);
int encode_segmentstatisticentry(ByteStreamWriter *stream, SegmentStatisticEntry *pointer);
int encode_segmentdataevent(ByteStreamWriter *stream, SegmentDataEvent *pointer);
int encode_segmentryelemlist(ByteStreamWriter *stream, SegmEntryElemList *pointer);
int encode_saspec(ByteStreamWriter *stream, SaSpec *pointer);
int encode_attributemodentry(ByteStreamWriter *stream, AttributeModEntry *pointer);
int encode_mdstimeinfo(ByteStreamWriter *stream, MdsTimeInfo *pointer);
int encode_enumval(ByteStreamWriter *stream, EnumVal *pointer);
int encode_trigsegmdataxferreq(ByteStreamWriter *stream, TrigSegmDataXferReq *pointer);
int encode_batmeasure(ByteStreamWriter *stream, BatMeasure *pointer);
int encode_segmentstatistics(ByteStreamWriter *stream, SegmentStatistics *pointer);
int encode_attributeidlist(ByteStreamWriter *stream, AttributeIdList *pointer);
int encode_scanreportinfofixed(ByteStreamWriter *stream, ScanReportInfoFixed *pointer);
int encode_datarequest(ByteStreamWriter *stream, DataRequest *pointer);
int encode_authbodyandstructype(ByteStreamWriter *stream, AuthBodyAndStrucType *pointer);
int encode_rlrq_apdu(ByteStreamWriter *stream, RLRQ_apdu *pointer);
int encode_data_apdu_message(ByteStreamWriter *stream, Data_apdu_message *pointer);
int encode_eventreportargumentsimple(ByteStreamWriter *stream, EventReportArgumentSimple *pointer);
int encode_scanreportinfovar(ByteStreamWriter *stream, ScanReportInfoVar *pointer);
int encode_scanreportinfompgrouped(ByteStreamWriter *stream, ScanReportInfoMPGrouped *pointer);
int encode_configobject(ByteStreamWriter *stream, ConfigObject *pointer);
int encode_scanreportinfogroupedlist(ByteStreamWriter *stream, ScanReportInfoGroupedList *pointer);
int encode_eventreportresultsimple(ByteStreamWriter *stream, EventReportResultSimple *pointer);
int encode_type(ByteStreamWriter *stream, TYPE *pointer);
int encode_observationscanfixed(ByteStreamWriter *stream, ObservationScanFixed *pointer);
int encode_dataresponse(ByteStreamWriter *stream, DataResponse *pointer);
int encode_prodspecentry(ByteStreamWriter *stream, ProdSpecEntry *pointer);
int encode_scalerangespec16(ByteStreamWriter *stream, ScaleRangeSpec16 *pointer);
int encode_segmentryelem(ByteStreamWriter *stream, SegmEntryElem *pointer);
int encode_abrt_apdu(ByteStreamWriter *stream, ABRT_apdu *pointer);
int encode_datareqmodecapab(ByteStreamWriter *stream, DataReqModeCapab *pointer);
int encode_supplementaltypelist(ByteStreamWriter *stream, SupplementalTypeList *pointer);
int encode_observationscanfixedlist(ByteStreamWriter *stream, ObservationScanFixedList *pointer);
int encode_trigsegmdataxferrsp(ByteStreamWriter *stream, TrigSegmDataXferRsp *pointer);
int encode_data_apdu(ByteStreamWriter *stream, DATA_apdu *pointer);
int encode_aarq_apdu(ByteStreamWriter *stream, AARQ_apdu *pointer);
int encode_typeverlist(ByteStreamWriter *stream, TypeVerList *pointer);
int encode_regcertdata(ByteStreamWriter *stream, RegCertData *pointer);
int encode_nuobsvalue(ByteStreamWriter *stream, NuObsValue *pointer);
int encode_scanreportpervarlist(ByteStreamWriter *stream, ScanReportPerVarList *pointer);
int encode_settimeinvoke(ByteStreamWriter *stream, SetTimeInvoke *pointer);
int encode_segmentinfolist(ByteStreamWriter *stream, SegmentInfoList *pointer);
int encode_actionresultsimple(ByteStreamWriter *stream, ActionResultSimple *pointer);
int encode_segmelemstaticattrentry(ByteStreamWriter *stream, SegmElemStaticAttrEntry *pointer);
int encode_basicnuobsvalue(ByteStreamWriter *stream, BasicNuObsValue *pointer);
int encode_basicnuobsvaluecmp(ByteStreamWriter *stream, BasicNuObsValueCmp *pointer);
int encode_configobjectlist(ByteStreamWriter *stream, ConfigObjectList *pointer);
int encode_setresultsimple(ByteStreamWriter *stream, SetResultSimple *pointer);
int encode_handleattrvalmapentry(ByteStreamWriter *stream, HandleAttrValMapEntry *pointer);
int encode_simplenuobsvalue(ByteStreamWriter *stream, SimpleNuObsValue *pointer);
int encode_metricspecsmall(ByteStreamWriter *stream, MetricSpecSmall *pointer);
int encode_configid(ByteStreamWriter *stream, ConfigId *pointer);

/** @} */

#endif /* ENCODER_ASN1_H_ */
