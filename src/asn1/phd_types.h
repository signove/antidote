/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file phd_types.h
 * \brief ASN1 PHD types header.
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
 * \author Mateus Lima
 * \date Jun 9, 2010
 */

#ifndef PHD_TYPES_H_
#define PHD_TYPES_H_

#include <stdint.h>

/**
 * @addtogroup ASN1 ASN1 and PHD Types
 * @{
 */

typedef uint8_t intu8;
typedef int8_t int8;
typedef uint16_t intu16;
typedef int16_t int16;
typedef uint32_t intu32;
typedef int32_t int32;

typedef intu8 BITS_8;
typedef intu16 BITS_16;
typedef intu32 BITS_32;

typedef struct octet_string {
	intu16 length;
	intu8 *value; /* first element of the array */
} octet_string;

typedef struct Any {
	intu16 length;
	intu8 *value; /* first element of the array */
} Any;

typedef intu16 OID_Type;
typedef intu16 PrivateOid;
typedef intu16 HANDLE;
typedef intu16 InstNumber;

typedef enum {
	nom_part_unspec = 0,
	nom_part_obj = 1,
	nom_part_metric = 2,
	nom_part_alert = 3,
	nom_part_dim = 4,
	nom_part_vattr = 5,
	nom_part_pgrp = 6,
	nom_part_sites = 7,
	nom_part_infrastruct = 8,
	nom_part_fef = 9,
	nom_part_ecg_extn = 10,
	nom_part_phd_dm = 128,
	nom_part_phd_hf = 129,
	nom_part_phd_ai = 130,
	nom_part_ret_code = 255,
	nom_part_ext_nom = 256,
	nom_part_priv = 1024
} NomPartition_Values;

typedef intu16 NomPartition;

typedef double FLOAT_Type;
typedef double SFLOAT_Type;
typedef intu32 RelativeTime;

typedef struct HighResRelativeTime {
	intu8 value[8];
} HighResRelativeTime;

typedef struct AbsoluteTimeAdjust {
	intu8 value[6];
} AbsoluteTimeAdjust;

typedef struct AbsoluteTime {
	intu8 century;
	intu8 year;
	intu8 month;
	intu8 day;
	intu8 hour;
	intu8 minute;
	intu8 second;
	intu8 sec_fractions;
} AbsoluteTime;

typedef struct TYPE {
	NomPartition partition;
	OID_Type code;
} TYPE;

typedef struct AVA_Type {
	OID_Type attribute_id;
	Any attribute_value;
} AVA_Type;

typedef struct AttributeList {
	intu16 count;
	intu16 length;
	AVA_Type *value; /* first element of the array */
} AttributeList;

typedef struct AttributeIdList {
	intu16 count;
	intu16 length;
	OID_Type *value; /* first element of the array */
} AttributeIdList;

typedef enum {
	os_disabled = 0, os_enabled = 1, os_notAvailable = 2
} OperationalState_Values;

typedef intu16 OperationalState;

typedef struct SystemModel {
	octet_string manufacturer;
	octet_string model_number;
} SystemModel;

typedef enum {
	ps_unspecified = 0,
	ps_serial_number = 1,
	ps_part_number = 2,
	ps_hw_revision = 3,
	ps_sw_revision = 4,
	ps_fw_revision = 5,
	ps_protocol_revision = 6,
	ps_prod_spec_gmdn = 7
} ProdSpecEntry_spec_type_Values;

typedef intu16 ProdSpecEntry_spec_type;

typedef struct ProdSpecEntry {
	ProdSpecEntry_spec_type spec_type;
	PrivateOid component_id;
	octet_string prod_spec;
} ProdSpecEntry;

typedef struct ProductionSpec {
	intu16 count;
	intu16 length;
	ProdSpecEntry *value; /* first element of the array */
} ProductionSpec;

typedef BITS_16 PowerStatus;
#define ON_MAINS 0x8000
#define ON_BATTERY 0x4000
#define CHARGING_FULL 0x0080
#define CHARGING_TRICKLE 0x0040
#define CHARGING_OFF 0x0020

typedef struct BatMeasure {
	FLOAT_Type value;
	OID_Type unit;
} BatMeasure;

typedef BITS_16 MeasurementStatus;

/* Named bit constants */
#define MS_INVALID 0x8000
#define MS_QUESTIONABLE 0x4000
#define MS_NOT_AVAILABLE 0x2000
#define MS_CALIBRATION_ONGOING 0x1000
#define MS_TEST_DATA 0x0800
#define MS_DEMO_DATA 0x0400
#define MS_VALIDATED_DATA 0x0080
#define MS_EARLY_INDICATION 0x0040
#define MS_MSMT_ONGOING 0x0020

typedef struct NuObsValue {
	OID_Type metric_id;
	MeasurementStatus state;
	OID_Type unit_code;
	FLOAT_Type value;
} NuObsValue;

typedef struct NuObsValueCmp {
	intu16 count;
	intu16 length;
	NuObsValue *value; /* first element of the array */
} NuObsValueCmp;

typedef struct SampleType {
	intu8 sample_size;
	intu8 significant_bits;
} SampleType;

#define SAMPLE_TYPE_SIGNIFICANT_BITS_SIGNED_SAMPLES 255
typedef BITS_16 SaFlags;

/* Named bit constants */
#define SMOOTH_CURVE 0x8000
#define DELAYED_CURVE 0x4000
#define STATIC_SCALE 0x2000
#define SA_EXT_VAL_RANGE 0x1000

typedef struct SaSpec {
	intu16 array_size;
	SampleType sample_type;
	SaFlags flags;
} SaSpec;

typedef struct ScaleRangeSpec8 {
	FLOAT_Type lower_absolute_value;
	FLOAT_Type upper_absolute_value;
	intu8 lower_scaled_value;
	intu8 upper_scaled_value;
} ScaleRangeSpec8;

typedef struct ScaleRangeSpec16 {
	FLOAT_Type lower_absolute_value;
	FLOAT_Type upper_absolute_value;
	intu16 lower_scaled_value;
	intu16 upper_scaled_value;
} ScaleRangeSpec16;

typedef struct ScaleRangeSpec32 {
	FLOAT_Type lower_absolute_value;
	FLOAT_Type upper_absolute_value;
	intu32 lower_scaled_value;
	intu32 upper_scaled_value;
} ScaleRangeSpec32;

typedef enum {
	OBJ_ID_CHOSEN = 0x0001,
	TEXT_STRING_CHOSEN = 0x0002,
	BIT_STR_CHOSEN = 0x0010
} EnumVal_choice_values;

typedef intu16 EnumVal_choice;

typedef struct EnumVal {
	EnumVal_choice choice;
	intu16 length;
	union {
		OID_Type enum_obj_id;
		octet_string enum_text_string;
		intu32 enum_bit_str; // BITS-32
	} u;
} EnumVal;

typedef struct EnumObsValue {
	OID_Type metric_id;
	MeasurementStatus state;
	EnumVal value;
} EnumObsValue;

typedef struct AttrValMapEntry {
	OID_Type attribute_id;
	intu16 attribute_len;
} AttrValMapEntry;

typedef struct AttrValMap {
	intu16 count;
	intu16 length;
	AttrValMapEntry *value; /* first element of the array */
} AttrValMap;

typedef struct HandleAttrValMapEntry {
	HANDLE obj_handle;
	AttrValMap attr_val_map;
} HandleAttrValMapEntry;

typedef enum {
	unconfirmed = 0, confirmed = 1
} ConfirmMode_Values;

typedef intu16 ConfirmMode;

typedef struct HandleAttrValMap {
	intu16 count;
	intu16 length;
	HandleAttrValMapEntry *value; /* first element of the array */
} HandleAttrValMap;

typedef enum {
	st_alg_nos = 0,
	st_alg_moving_average = 1,
	st_alg_recursive = 2,
	st_alg_min_pick = 3,
	st_alg_max_pick = 4,
	st_alg_median = 5,
	st_alg_trended = 512,
	st_alg_no_downsampling = 1024,
	st_alg_manuf_specific_start = 61440,
	st_alg_manuf_specific_end = 65535
} StoSampleAlg_Values;

typedef intu16 StoSampleAlg;

typedef struct SetTimeInvoke {
	AbsoluteTime date_time;
	FLOAT_Type accuracy;
} SetTimeInvoke;

typedef struct SegmIdList {
	intu16 count;
	intu16 length;
	InstNumber *value; /* first element of the array */
} SegmIdList;

typedef struct AbsTimeRange {
	AbsoluteTime from_time;
	AbsoluteTime to_time;
} AbsTimeRange;

typedef struct SegmentInfo {
	InstNumber seg_inst_no;
	AttributeList seg_info;
} SegmentInfo;

typedef struct SegmentInfoList {
	intu16 count;
	intu16 length;
	SegmentInfo *value; /* first element of the array */
} SegmentInfoList;

typedef enum {
	ALL_SEGMENTS_CHOSEN = 0x0001,
	SEGM_ID_LIST_CHOSEN = 0x0002,
	ABS_TIME_RANGE_CHOSEN = 0x0003
} SegmSelection_choice_values;

typedef intu16 SegmSelection_choice;

typedef struct SegmSelection {
	SegmSelection_choice choice;
	intu16 length;
	union {
		intu16 all_segments;
		SegmIdList segm_id_list;
		AbsTimeRange abs_time_range;
	} u;
} SegmSelection;

/* Named bit constants */
#define pmsc_var_no_of_segm             0x8000
#define pmsc_epi_seg_entries            0x0800
#define pmsc_peri_seg_entries           0x0400
#define pmsc_abs_time_select            0x0200
#define pmsc_clear_segm_by_list_sup     0x0100
#define pmsc_clear_segm_by_time_sup     0x0080
#define pmsc_clear_segm_remove          0x0040
#define pmsc_multi_person               0x0008

typedef BITS_16 PMStoreCapab;

#define SEG_ELEM_HDR_ABSOLUTE_TIME 0x8000
#define SEG_ELEM_HDR_RELATIVE_TIME 0x4000
#define SEG_ELEM_HDR_HIRES_RELATIVE_TIME 0x2000

typedef BITS_16 SegmEntryHeader;

typedef struct SegmEntryElem {
	OID_Type class_id;
	TYPE metric_type;
	HANDLE handle;
	AttrValMap attr_val_map;
} SegmEntryElem;

typedef struct SegmEntryElemList {
	intu16 count;
	intu16 length;
	SegmEntryElem *value; /* first element of the array */
} SegmEntryElemList;

typedef struct PmSegmentEntryMap {
	SegmEntryHeader segm_entry_header;
	SegmEntryElemList segm_entry_elem_list;
} PmSegmentEntryMap;

typedef struct SegmElemStaticAttrEntry {
	OID_Type class_id;
	TYPE metric_type;
	AttributeList attribute_list;
} SegmElemStaticAttrEntry;

typedef struct PmSegmElemStaticAttrList {
	intu16 count;
	intu16 length;
	SegmElemStaticAttrEntry *value; /* first element of the array */
} PmSegmElemStaticAttrList;

typedef struct TrigSegmDataXferReq {
	InstNumber seg_inst_no;
} TrigSegmDataXferReq;

typedef intu16 TrigSegmXferRsp;
#define TSXR_SUCCESSFUL 0
#define TSXR_FAIL_NO_SUCH_SEGMENT 1
#define TSXR_FAIL_SEGM_TRY_LATER 2
#define TSXR_FAIL_SEGM_EMPTY 3
#define TSXR_FAIL_OTHER 512

typedef struct TrigSegmDataXferRsp {
	InstNumber seg_inst_no;
	TrigSegmXferRsp trig_segm_xfer_rsp;
} TrigSegmDataXferRsp;

typedef BITS_16 SegmEvtStatus;
#define SEVTSTA_FIRST_ENTRY 0x8000
#define SEVTSTA_LAST_ENTRY 0x4000
#define SEVTSTA_AGENT_ABORT 0x0800
#define SEVTSTA_MANAGER_CONFIRM 0x0080
#define SEVTSTA_MANAGER_ABORT 0x0008

typedef struct SegmDataEventDescr {
	InstNumber segm_instance;
	intu32 segm_evt_entry_index;
	intu32 segm_evt_entry_count;
	SegmEvtStatus segm_evt_status;
} SegmDataEventDescr;

typedef struct SegmentDataEvent {
	SegmDataEventDescr segm_data_event_descr;
	octet_string segm_data_event_entries;
} SegmentDataEvent;

typedef struct SegmentDataResult {
	SegmDataEventDescr segm_data_event_descr;
} SegmentDataResult;

typedef intu16 SegmStatType;
#define SEGM_STAT_TYPE_MINIMUM 1
#define SEGM_STAT_TYPE_MAXIMUM 2
#define SEGM_STAT_TYPE_AVERAGE 3

typedef struct SegmentStatisticEntry {
	SegmStatType segm_stat_type;
	octet_string segm_stat_entry;
} SegmentStatisticEntry;

typedef struct SegmentStatistics {
	intu16 count;
	intu16 length;
	SegmentStatisticEntry *value; /* first element of the array */
} SegmentStatistics;

typedef intu8 AuthBody;
typedef intu8 AuthBodyStrucType;

typedef struct AuthBodyAndStrucType {
	AuthBody auth_body;
	AuthBodyStrucType auth_body_struc_type;
} AuthBodyAndStrucType;

typedef struct RegCertData {
	AuthBodyAndStrucType auth_body_and_struc_type;
	Any auth_body_data;
} RegCertData;

typedef struct ObservationScan {
	HANDLE obj_handle;
	AttributeList attributes;
} ObservationScan;

typedef OID_Type TimeProtocolId;
typedef BITS_32 AssociationVersion;
#define ASSOC_VERSION1 0x80000000
typedef BITS_32 ProtocolVersion;
#define PROTOCOL_VERSION1 0x80000000
typedef BITS_16 EncodingRules;
#define MDER 0x8000
#define XER 0x4000
#define PER 0x2000

typedef struct UUID_Ident {
	intu8 value[16];
} UUID_Ident;

typedef intu16 DataProtoId;
#define DATA_PROTO_ID_20601 20601
#define DATA_PROTO_ID_EXTERNAL 65535
typedef struct DataProto {
	DataProtoId data_proto_id;
	Any data_proto_info;
} DataProto;

typedef struct DataProtoList {
	intu16 count;
	intu16 length;
	DataProto *value; /* first element of the array */
} DataProtoList;

typedef struct AARQ_apdu {
	AssociationVersion assoc_version;
	DataProtoList data_proto_list;
} AARQ_apdu;

typedef intu16 Associate_result;
#define ACCEPTED 0
#define REJECTED_PERMANENT 1
#define REJECTED_TRANSIENT 2
#define ACCEPTED_UNKNOWN_CONFIG 3
#define REJECTED_NO_COMMON_PROTOCOL 4
#define REJECTED_NO_COMMON_PARAMETER 5
#define REJECTED_UNKNOWN 6
#define REJECTED_UNAUTHORIZED 7
#define REJECTED_UNSUPPORTED_ASSOC_VERSION 8
typedef struct AARE_apdu {
	Associate_result result;
	DataProto selected_data_proto;
} AARE_apdu;

typedef intu16 Release_request_reason;

#define RELEASE_REQUEST_REASON_NORMAL 0

typedef struct RLRQ_apdu {
	Release_request_reason reason;
} RLRQ_apdu;

typedef intu16 Release_response_reason;
#define RELEASE_RESPONSE_REASON_NORMAL 0
typedef struct RLRE_apdu {
	Release_response_reason reason;
} RLRE_apdu;

typedef intu16 Abort_reason;
#define ABORT_REASON_UNDEFINED 0
#define ABORT_REASON_BUFFER_OVERFLOW 1
#define ABORT_REASON_RESPONSE_TIMEOUT 2
#define ABORT_REASON_CONFIGURATION_TIMEOUT 3
typedef struct ABRT_apdu {
	Abort_reason reason;
} ABRT_apdu;

typedef octet_string PRST_apdu;
typedef intu16 InvokeIDType;
typedef struct EventReportArgumentSimple {
	HANDLE obj_handle;
	RelativeTime event_time;
	OID_Type event_type;
	Any event_info;
} EventReportArgumentSimple;

typedef struct GetArgumentSimple {
	HANDLE obj_handle;
	AttributeIdList attribute_id_list;
} GetArgumentSimple;

typedef intu16 ModifyOperator;
#define REPLACE 0
#define ADD_VALUES 1
#define REMOVE_VALUES 2
#define SET_TO_DEFAULT 3
typedef struct AttributeModEntry {
	ModifyOperator modify_operator;
	AVA_Type attribute;
} AttributeModEntry;

typedef struct ModificationList {
	intu16 count;
	intu16 length;
	AttributeModEntry *value; /* first element of the array */
} ModificationList;

typedef struct SetArgumentSimple {
	HANDLE obj_handle;
	ModificationList modification_list;
} SetArgumentSimple;

typedef struct ActionArgumentSimple {
	HANDLE obj_handle;
	OID_Type action_type;
	Any action_info_args;
} ActionArgumentSimple;

typedef struct EventReportResultSimple {
	HANDLE obj_handle;
	RelativeTime currentTime;
	OID_Type event_type;
	Any event_reply_info;
} EventReportResultSimple;

typedef struct GetResultSimple {
	HANDLE obj_handle;
	AttributeList attribute_list;
} GetResultSimple;

typedef struct TypeVer {
	OID_Type type;
	intu16 version;
} TypeVer;

typedef struct TypeVerList {
	intu16 count;
	intu16 length;
	TypeVer *value; /* first element of the array */
} TypeVerList;

typedef struct SetResultSimple {
	HANDLE obj_handle;
	AttributeList attribute_list;
} SetResultSimple;

typedef struct ActionResultSimple {
	HANDLE obj_handle;
	OID_Type action_type;
	Any action_info_args;
} ActionResultSimple;

typedef intu16 ERROR;
#define NO_SUCH_OBJECT_INSTANCE 1
#define ACCESS_DENIED 2
#define NO_SUCH_ACTION 9
#define INVALID_OBJECT_INSTANCE 17
#define PROTOCOL_VIOLATION 23
#define NOT_ALLOWED_BY_OBJECT 24
#define ACTION_TIMED_OUT 25
#define ACTION_ABORTED 26

typedef struct ErrorResult {
	ERROR error_value;
	Any parameter;
} ErrorResult;

typedef intu16 RorjProblem;
#define UNRECOGNIZED_APDU 0
#define BADLY_STRUCTURED_APDU 2
#define UNRECOGNIZED_OPERATION 101
#define RESOURCE_LIMITATION 103
#define UNEXPECTED_ERROR 303
typedef struct RejectResult {
	RorjProblem problem;
} RejectResult;

typedef enum {
	ROIV_CMIP_EVENT_REPORT_CHOSEN = 0x0100,
	ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN = 0x0101,
	ROIV_CMIP_GET_CHOSEN = 0x0103,
	ROIV_CMIP_SET_CHOSEN = 0x0104,
	ROIV_CMIP_CONFIRMED_SET_CHOSEN = 0x0105,
	ROIV_CMIP_ACTION_CHOSEN = 0x0106,
	ROIV_CMIP_CONFIRMED_ACTION_CHOSEN = 0x0107,
	RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN = 0x0201,
	RORS_CMIP_GET_CHOSEN = 0x0203,
	RORS_CMIP_CONFIRMED_SET_CHOSEN = 0x0205,
	RORS_CMIP_CONFIRMED_ACTION_CHOSEN = 0x0207,
	ROER_CHOSEN = 0x0300,
	RORJ_CHOSEN = 0x0400
} DATA_apdu_choice_values;

typedef intu16 DATA_apdu_choice;

typedef struct Data_apdu_message {
	DATA_apdu_choice choice;
	intu16 length;
	union {
		EventReportArgumentSimple roiv_cmipEventReport;
		EventReportArgumentSimple roiv_cmipConfirmedEventReport;
		GetArgumentSimple roiv_cmipGet;
		SetArgumentSimple roiv_cmipSet;
		SetArgumentSimple roiv_cmipConfirmedSet;
		ActionArgumentSimple roiv_cmipAction;
		ActionArgumentSimple roiv_cmipConfirmedAction;
		EventReportResultSimple rors_cmipConfirmedEventReport;
		GetResultSimple rors_cmipGet;
		SetResultSimple rors_cmipConfirmedSet;
		ActionResultSimple rors_cmipConfirmedAction;
		ErrorResult roer;
		RejectResult rorj;
	} u;
} Data_apdu_message;

typedef struct DATA_apdu {
	InvokeIDType invoke_id;
	Data_apdu_message message;
} DATA_apdu;

typedef enum {
	AARQ_CHOSEN = 0xE200,
	AARE_CHOSEN = 0xE300,
	RLRQ_CHOSEN = 0xE400,
	RLRE_CHOSEN = 0xE500,
	ABRT_CHOSEN = 0xE600,
	PRST_CHOSEN = 0xE700
} APDU_choice_values;

typedef intu16 APDU_choice;

typedef struct APDU {
	APDU_choice choice;
	intu16 length;
	union {
		AARQ_apdu aarq;
		AARE_apdu aare;
		RLRQ_apdu rlrq;
		RLRE_apdu rlre;
		ABRT_apdu abrt;
		PRST_apdu prst;
	} u;
} APDU;

typedef BITS_32 NomenclatureVersion;
#define NOM_VERSION1 0x80000000
typedef BITS_32 FunctionalUnits;
#define FUN_UNITS_UNIDIRECTIONAL 0x80000000
#define FUN_UNITS_HAVETESTCAP 0x40000000
#define FUN_UNITS_CREATETESTASSOC 0x20000000
typedef BITS_32 SystemType;
#define SYS_TYPE_MANAGER 0x80000000
#define SYS_TYPE_AGENT 0x00800000
typedef intu16 ConfigId;
#define MANAGER_CONFIG_RESPONSE 0x0000
#define STANDARD_CONFIG_START 0x0001
#define STANDARD_CONFIG_END 0x3FFF
#define EXTENDED_CONFIG_START 0x4000
#define EXTENDED_CONFIG_END 0x7FFF
#define RESERVED_START 0x8000
#define RESERVED_END 0xFFFF

typedef BITS_16 DataReqModeFlags;

typedef struct DataReqModeCapab {
	DataReqModeFlags data_req_mode_flags;
	intu8 data_req_init_agent_count;
	intu8 data_req_init_manager_count;
} DataReqModeCapab;

#define DATA_REQ_SUPP_STOP 0x8000
#define DATA_REQ_SUPP_SCOPE_ALL 0x0800
#define DATA_REQ_SUPP_SCOPE_CLASS 0x0400
#define DATA_REQ_SUPP_SCOPE_HANDLE 0x0200
#define DATA_REQ_SUPP_MODE_SINGLE_RSP 0x0080
#define DATA_REQ_SUPP_MODE_TIME_PERIOD 0x0040
#define DATA_REQ_SUPP_MODE_TIME_NO_LIMIT 0x0020
#define DATA_REQ_SUPP_PERSON_ID 0x0010
#define DATA_REQ_SUPP_INIT_AGENT 0x0001

typedef struct PhdAssociationInformation {
	ProtocolVersion protocolVersion;
	EncodingRules encodingRules;
	NomenclatureVersion nomenclatureVersion;
	FunctionalUnits functionalUnits;
	SystemType systemType;
	octet_string system_id;
	intu16 dev_config_id;
	DataReqModeCapab data_req_mode_capab;
	AttributeList optionList;
} PhdAssociationInformation;

typedef struct ManufSpecAssociationInformation {
	UUID_Ident data_proto_id_ext;
	Any data_proto_info_ext;
} ManufSpecAssociationInformation;

typedef BITS_16 MdsTimeCapState;
#define MDS_TIME_CAPAB_REAL_TIME_CLOCK 0x8000
#define MDS_TIME_CAPAB_SET_CLOCK 0x4000
#define MDS_TIME_CAPAB_RELATIVE_TIME 0x2000
#define MDS_TIME_CAPAB_HIGH_RES_RELATIVE_TIME 0x1000
#define MDS_TIME_CAPAB_SYNC_ABS_TIME 0x0800
#define MDS_TIME_CAPAB_SYNC_REL_TIME 0x0400
#define MDS_TIME_CAPAB_SYNC_HI_RES_RELATIVE_TIME 0x0200
#define MDS_TIME_STATE_ABS_TIME_SYNCED 0x0080
#define MDS_TIME_STATE_REL_TIME_SYNCED 0x0040
#define MDS_TIME_STATE_HI_RES_RELATIVE_TIME_SYNCED 0x0020
#define MDS_TIME_MGR_SET_TIME 0x0010

typedef struct MdsTimeInfo {
	MdsTimeCapState mds_time_cap_state;
	TimeProtocolId time_sync_protocol;
	RelativeTime time_sync_accuracy;
	intu16 time_resolution_abs_time;
	intu16 time_resolution_rel_time;
	intu32 time_resolution_high_res_time;
} MdsTimeInfo;

typedef octet_string EnumPrintableString;
typedef intu16 PersonId;
#define UNKNOWN_PERSON_ID 0xFFFF
typedef BITS_16 MetricSpecSmall;
#define MSS_AVAIL_INTERMITTENT 0x8000
#define MSS_AVAIL_STORED_DATA 0x4000
#define MSS_UPD_APERIODIC 0x2000
#define MSS_MSMT_APERIODIC 0x1000
#define MSS_MSMT_PHYS_EV_ID 0x0800
#define MSS_MSMT_BTB_METRIC 0x0400
#define MSS_ACC_MANAGER_INITIATED 0x0080
#define MSS_ACC_AGENT_INITIATED 0x0040
#define MSS_CAT_MANUAL 0x0008
#define MSS_CAT_SETTING 0x0004
#define MSS_CAT_CALCULATION 0x0002

typedef struct MetricStructureSmall {
	intu8 ms_struct;
#define MS_STRUCT_SIMPLE 0
#define MS_STRUCT_COMPOUND 1
#define MS_STRUCT_RESERVED 2
#define MS_STRUCT_COMPOUND_FIX 3
	intu8 ms_comp_no;
} MetricStructureSmall;

typedef struct MetricIdList {
	intu16 count;
	intu16 length;
	OID_Type *value; /* first element of the array */
} MetricIdList;

typedef struct RegCertDataList {
	intu16 count;
	intu16 length;
	RegCertData *value; /* first element of the array */
} RegCertDataList;

typedef struct SupplementalTypeList {
	intu16 count;
	intu16 length;
	TYPE *value; /* first element of the array */
} SupplementalTypeList;

typedef struct ObservationScanList {
	intu16 count;
	intu16 length;
	ObservationScan *value; /* first element of the array */
} ObservationScanList;

typedef struct ScanReportPerVar {
	intu16 person_id;
	ObservationScanList obs_scan_var;
} ScanReportPerVar;

typedef struct ScanReportPerVarList {
	intu16 count;
	intu16 length;
	ScanReportPerVar *value; /* first element of the array */
} ScanReportPerVarList;

typedef intu16 DataReqId;
#define DATA_REQ_ID_MANAGER_INITIATED_MIN 0x0000
#define DATA_REQ_ID_MANAGER_INITIATED_MAX 0xEFFF
#define DATA_REQ_ID_AGENT_INITIATED 0xF000

typedef struct ScanReportInfoMPVar {
	DataReqId data_req_id;
	intu16 scan_report_no;
	ScanReportPerVarList scan_per_var;
} ScanReportInfoMPVar;

typedef struct ObservationScanFixed {
	HANDLE obj_handle;
	octet_string obs_val_data;
} ObservationScanFixed;

typedef struct ObservationScanFixedList {
	intu16 count;
	intu16 length;
	ObservationScanFixed *value; /* first element of the array */
} ObservationScanFixedList;

typedef struct ScanReportPerFixed {
	intu16 person_id;
	ObservationScanFixedList obs_scan_fix;
} ScanReportPerFixed;

typedef struct ScanReportPerFixedList {
	intu16 count;
	intu16 length;
	ScanReportPerFixed *value; /* first element of the array */
} ScanReportPerFixedList;

typedef struct ScanReportInfoMPFixed {
	DataReqId data_req_id;
	intu16 scan_report_no;
	ScanReportPerFixedList scan_per_fixed;
} ScanReportInfoMPFixed;

typedef struct ScanReportInfoVar {
	DataReqId data_req_id;
	intu16 scan_report_no;
	ObservationScanList obs_scan_var;
} ScanReportInfoVar;

typedef struct ScanReportInfoFixed {
	DataReqId data_req_id;
	intu16 scan_report_no;
	ObservationScanFixedList obs_scan_fixed;
} ScanReportInfoFixed;

typedef octet_string ObservationScanGrouped;

typedef struct ScanReportInfoGroupedList {
	intu16 count;
	intu16 length;
	ObservationScanGrouped *value; /* first element of the array */
} ScanReportInfoGroupedList;

typedef struct ScanReportInfoGrouped {
	intu16 data_req_id;
	intu16 scan_report_no;
	ScanReportInfoGroupedList obs_scan_grouped;
} ScanReportInfoGrouped;

typedef struct ScanReportPerGrouped {
	PersonId person_id;
	ObservationScanGrouped obs_scan_grouped;
} ScanReportPerGrouped;

typedef struct ScanReportPerGroupedList {
	intu16 count;
	intu16 length;
	ScanReportPerGrouped *value; /* first element of the array */
} ScanReportPerGroupedList;

typedef struct ScanReportInfoMPGrouped {
	intu16 data_req_id;
	intu16 scan_report_no;
	ScanReportPerGroupedList scan_per_grouped;
} ScanReportInfoMPGrouped;

typedef struct ConfigObject {
	OID_Type obj_class;
	HANDLE obj_handle;
	AttributeList attributes;
} ConfigObject;

typedef struct ConfigObjectList {
	intu16 count;
	intu16 length;
	ConfigObject *value; /* first element of the array */
} ConfigObjectList;

typedef struct ConfigReport {
	ConfigId config_report_id;
	ConfigObjectList config_obj_list;
} ConfigReport;

typedef intu16 ConfigResult;
#define ACCEPTED_CONFIG 0x0000
#define UNSUPPORTED_CONFIG 0x0001
#define STANDARD_CONFIG_UNKNOWN 0x0002
typedef struct ConfigReportRsp {
	ConfigId config_report_id;
	ConfigResult config_result;
} ConfigReportRsp;

typedef BITS_16 DataReqMode;
#define DATA_REQ_START_STOP 0x8000
#define DATA_REQ_CONTINUATION 0x4000
#define DATA_REQ_SCOPE_ALL 0x0800
#define DATA_REQ_SCOPE_TYPE 0x0400
#define DATA_REQ_SCOPE_HANDLE 0x0200
#define DATA_REQ_MODE_SINGLE_RSP 0x0080
#define DATA_REQ_MODE_TIME_PERIOD 0x0040
#define DATA_REQ_MODE_TIME_NO_LIMIT 0x0020
#define DATA_REQ_MODE_DATA_REQ_PERSON_ID 0x0008

typedef struct HANDLEList {
	intu16 count;
	intu16 length;
	HANDLE *value; /* first element of the array */
} HANDLEList;

typedef struct DataRequest {
	DataReqId data_req_id;
	DataReqMode data_req_mode;
	RelativeTime data_req_time;
	intu16 data_req_person_id;
	OID_Type data_req_class;
	HANDLEList data_req_obj_handle_list;
} DataRequest;

typedef intu16 DataReqResult;
#define DATA_REQ_RESULT_NO_ERROR 0
#define DATA_REQ_RESULT_UNSPECIFIC_ERROR 1
#define DATA_REQ_RESULT_NO_STOP_SUPPORT 2
#define DATA_REQ_RESULT_NO_SCOPE_ALL_SUPPORT 3
#define DATA_REQ_RESULT_NO_SCOPE_CLASS_SUPPORT 4
#define DATA_REQ_RESULT_NO_SCOPE_HANDLE_SUPPORT 5
#define DATA_REQ_RESULT_NO_MODE_SINGLE_RSP_SUPPORT 6
#define DATA_REQ_RESULT_NO_MODE_TIME_PERIOD_SUPPORT 7
#define DATA_REQ_RESULT_NO_MODE_TIME_NO_LIMIT_SUPPORT 8
#define DATA_REQ_RESULT_NO_PERSON_ID_SUPPORT 9
#define DATA_REQ_RESULT_UNKNOWN_PERSON_ID 11
#define DATA_REQ_RESULT_UNKNOWN_CLASS 12
#define DATA_REQ_RESULT_UNKNOWN_HANDLE 13
#define DATA_REQ_RESULT_UNSUPP_SCOPE 14
#define DATA_REQ_RESULT_UNSUPP_MODE 15
#define DATA_REQ_RESULT_INIT_MANAGER_OVERFLOW 16
#define DATA_REQ_RESULT_CONTINUATION_NOT_SUPPORTED 17
#define DATA_REQ_RESULT_INVALID_REQ_ID 18
typedef struct DataResponse {
	RelativeTime rel_time_stamp;
	DataReqResult data_req_result;
	OID_Type event_type;
	Any event_info;
} DataResponse;

typedef FLOAT_Type SimpleNuObsValue;
typedef struct SimpleNuObsValueCmp {
	intu16 count;
	intu16 length;
	SimpleNuObsValue *value; /* first element of the array */
} SimpleNuObsValueCmp;

typedef SFLOAT_Type BasicNuObsValue;
typedef struct BasicNuObsValueCmp {
	intu16 count;
	intu16 length;
	BasicNuObsValue *value; /* first element of the array */
} BasicNuObsValueCmp;

/** @} */

#endif /* PHD_TYPES_H_ */
