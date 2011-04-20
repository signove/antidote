/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file phd_types_generated.h
 * \brief ASN1 PHD generated types header.
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
 *
 * \author Mateus Lima
 * \date Jun 9, 2010
 */

#ifndef PHD_TYPES_GENERATED_H_
#define PHD_TYPES_GENERATED_H_

#include <stdint.h>


/**
 * @addtogroup ASN1
 * @{
 */

/**
 *
 *  intu8
 *
 */
typedef uint8_t intu8;

/**
 *
 *  int8
 *
 */
typedef int8_t int8;

/**
 *
 *  intu16
 *
 */
typedef uint16_t intu16;

/**
 *
 *  int16
 *
 */
typedef int16_t int16;

/**
 *
 *  intu32
 *
 */
typedef uint32_t intu32;

/**
 *
 *  int32
 *
 */
typedef int32_t int32;

/**
 *
 *  BITS_16
 *
 */
typedef struct octet_string {
	intu16 length;
	intu8 value[1]; /* first element of the array */
} octet_string;

/**
 *
 * Dynamic binary string structure
 *
 * This structure is used in generated code for XSD hexBinary and
 * base64Binary types.
 */
typedef struct OSDynOctStr {
	intu32     numocts;
	const intu8 *data;
} OSDynOctStr;

typedef struct BITS_16 {
	intu16 length;
	intu8 value[2]; /* first element of the array */
} BITS_16;

typedef struct Any {
	intu16 length;
	intu8 value[1]; /* first element of the array */
} Any;

/**
 *
 *  BITS_32
 *
 */
typedef struct BITS_32 {
	intu16 length;
	intu8 value[4]; /* first element of the array */
} BITS_32;

/**
 *
 *  OID_Type
 *
 */
typedef intu16 OID_Type;

/**
 *
 *  PrivateOid
 *
 */
typedef intu16 PrivateOid;

/**
 *
 *  HANDLE
 *
 */
typedef intu16 HANDLE;

/**
 *
 *  InstNumber
 *
 */
typedef intu16 InstNumber;

/**
 *
 *  NomPartition
 *
 */
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
} NomPartition_Root;

typedef intu16 NomPartition;

/**
 *
 *  FLOAT_Type
 *
 */
typedef intu32 FLOAT_Type;

/**
 *
 *  SFLOAT_Type
 *
 */
typedef intu16 SFLOAT_Type;

/**
 *
 *  RelativeTime
 *
 */
typedef intu32 RelativeTime;

/**
 *
 *  HighResRelativeTime
 *
 */
typedef struct HighResRelativeTime {
	intu32 numocts;
	intu8 data[8];
} HighResRelativeTime;

/**
 *
 *  AbsoluteTimeAdjust
 *
 */
typedef struct AbsoluteTimeAdjust {
	intu32 numocts;
	intu8 data[6];
} AbsoluteTimeAdjust;

/**
 *
 *  OperationalState
 *
 */
typedef enum {
	disabled = 0,
	enabled = 1,
	notAvailable = 2
} OperationalState_Root;

typedef intu16 OperationalState;

/**
 *
 *  PowerStatus
 *
 */

/* Named bit constants */

#define PowerStatus_onMains                         0
#define PowerStatus_onBattery                       1
#define PowerStatus_chargingFull                    8
#define PowerStatus_chargingTrickle                 9
#define PowerStatus_chargingOff                     10

typedef struct PowerStatus {
	intu32 numbits;
	intu8  data[2];
} PowerStatus;

/**
 *
 *  MeasurementStatus
 *
 */

/* Named bit constants */

#define MeasurementStatus_invalid                         0
#define MeasurementStatus_questionable                    1
#define MeasurementStatus_not_available                   2
#define MeasurementStatus_calibration_ongoing             3
#define MeasurementStatus_test_data                       4
#define MeasurementStatus_demo_data                       5
#define MeasurementStatus_validated_data                  8
#define MeasurementStatus_early_indication                9
#define MeasurementStatus_msmt_ongoing                    10

typedef struct MeasurementStatus {
	intu32 numbits;
	intu8  data[2];
} MeasurementStatus;

/**
 *
 *  SaFlags
 *
 */

/* Named bit constants */

#define SaFlags_smooth_curve                    0
#define SaFlags_delayed_curve                   1
#define SaFlags_static_scale                    2
#define SaFlags_sa_ext_val_range                3

typedef struct SaFlags {
	intu32 numbits;
	intu8  data[2];
} SaFlags;

/**
 *
 *  ConfirmMode
 *
 */

typedef enum {
	unconfirmed = 0,
	confirmed = 1
} ConfirmMode_Root;

typedef intu16 ConfirmMode;

/**
 *
 *  StoSampleAlg
 *
 */

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
} StoSampleAlg_Root;

typedef intu16 StoSampleAlg;

/**
 *
 *  TimeProtocolId
 *
 */

typedef OID_Type TimeProtocolId;

/**
 *
 *  DataProtoId
 *
 */
typedef enum {
	data_proto_id_empty = 0,
	data_proto_id_20601 = 20601,
	data_proto_id_external = 65535
} DataProtoId_Root;

typedef intu16 DataProtoId;

/**
 *
 *  Abort_reason
 *
 */

typedef enum {
	undefined = 0,
	buffer_overflow = 1,
	response_timeout = 2,
	configuration_timeout = 3
} Abort_reason_Root;

typedef intu16 Abort_reason;

/**
 *
 *  AssociateResult
 *
 */
typedef enum {
	accepted = 0,
	rejected_permanent = 1,
	rejected_transient = 2,
	accepted_unknown_config = 3,
	rejected_no_common_protocol = 4,
	rejected_no_common_parameter = 5,
	rejected_unknown = 6,
	rejected_unauthorized = 7,
	rejected_unsupported_assoc_version = 8
} AssociateResult_Root;

typedef intu16 AssociateResult;

/**
 *
 *  ReleaseRequestReason
 *
 */
typedef enum {
	normal = 0,
	no_more_configurations = 1,
	configuration_changed = 2
} ReleaseRequestReason_Root;

typedef intu16 ReleaseRequestReason;

/**
 *
 *  ReleaseResponseReason
 *
 */

typedef enum {
	ReleaseResponseReason_normal = 0
} ReleaseResponseReason_Root;

typedef intu16 ReleaseResponseReason;

/**
 *
 * AssociationVersion
 *
 */

/* Named bit constants */

#define AssociationVersion_assoc_version1                  0

typedef struct AssociationVersion {
	intu32 numbits;
	intu8  data[4];
} AssociationVersion;

/**
 *
 * ProtocolVersion
 *
 */

/* Named bit constants */

#define ProtocolVersion_protocol_version1               0

typedef struct ProtocolVersion {
	intu32 numbits;
	intu8  data[4];
} ProtocolVersion;

/**
 *
 * EncodingRules
 *
 */

/* Named bit constants */

#define EncodingRules_mder                            0
#define EncodingRules_xer                             1
#define EncodingRules_per                             2

typedef struct EncodingRules {
	intu32 numbits;
	intu8  data[2];
} EncodingRules;

/**
 *
 * NomenclatureVersion
 *
 */

/* Named bit constants */

#define NomenclatureVersion_nom_version1                    0

typedef struct NomenclatureVersion {
	intu32 numbits;
	intu8  data[4];
} NomenclatureVersion;

/**
 *
 * FunctionalUnits
 *
 */

/* Named bit constants */

#define FunctionalUnits_fun_units_unidirectional        0
#define FunctionalUnits_fun_units_havetestcap           1
#define FunctionalUnits_fun_units_createtestassoc       2

typedef struct FunctionalUnits {
	intu32 numbits;
	intu8  data[4];
} FunctionalUnits;

/**
 *
 * SystemType
 *
 */

/* Named bit constants */

#define SystemType_sys_type_manager                0
#define SystemType_sys_type_agent                  8

typedef struct SystemType {
	intu32 numbits;
	intu8  data[4];
} SystemType;

/**
 *
 * ConfigId
 *
 */

typedef enum {
	manager_config_response = 0,
	standard_config_start = 1,
	standard_config_end = 16383,
	extended_config_start = 16384,
	extended_config_end = 32767,
	reserved_start = 32768,
	reserved_end = 65535
} ConfigId_Root;

typedef intu16 ConfigId;

/**
 *
 * PrstApdu
 *
 */

typedef OSDynOctStr PrstApdu;

/**
 *
 * InvokeIDType
 *
 */

typedef intu16 InvokeIDType;

/**
 *
 * RoerErrorValue
 *
 */

typedef enum {
	no_such_object_instance = 1,
	no_such_action = 9,
	invalid_object_instance = 17,
	protocol_violation = 23,
	not_allowed_by_object = 24,
	action_timed_out = 25,
	action_aborted = 26
} RoerErrorValue_Root;

typedef intu16 RoerErrorValue;

/**
 *
 * RorjProblem
 *
 */

typedef enum {
	unrecognized_apdu = 0,
	badly_structured_apdu = 2,
	unrecognized_operation = 101,
	resource_limitation = 103,
	unexpected_error = 303
} RorjProblem_Root;

typedef intu16 RorjProblem;

/**
 *
 * ModifyOperator
 *
 */

typedef enum {
	replace = 0,
	addValues = 1,
	removeValues = 2,
	setToDefault = 3
} ModifyOperator_Root;

typedef intu16 ModifyOperator;

/**
 *
 * UuidIdent
 *
 */

typedef struct UuidIdent {
	intu32 numocts;
	intu8 data[16];
} UuidIdent;

/**
 *
 * MdsTimeCapState
 *
 */

/* Named bit constants */

#define MdsTimeCapState_mds_time_capab_real_time_clock  0
#define MdsTimeCapState_mds_time_capab_set_clock        1
#define MdsTimeCapState_mds_time_capab_relative_time    2
#define MdsTimeCapState_mds_time_capab_high_res_relative_time 3
#define MdsTimeCapState_mds_time_capab_sync_abs_time    4
#define MdsTimeCapState_mds_time_capab_sync_rel_time    5
#define MdsTimeCapState_mds_time_capab_sync_hi_res_relative_time 6
#define MdsTimeCapState_mds_time_state_abs_time_synced  8
#define MdsTimeCapState_mds_time_state_rel_time_synced  9
#define MdsTimeCapState_mds_time_state_hi_res_relative_time_synced 10
#define MdsTimeCapState_mds_time_mgr_set_time           11

typedef struct MdsTimeCapState {
	intu32 numbits;
	intu8  data[2];
} MdsTimeCapState;

/**
 *
 * AuthBody
 *
 */

typedef enum {
	auth_body_empty = 0,
	auth_body_ieee_11073 = 1,
	auth_body_continua = 2,
	auth_body_experimental = 254,
	auth_body_reserved = 255
} AuthBody_Root;

typedef intu8 AuthBody;

/**
 *
 * AuthBodyStrucType
 *
 */

typedef intu8 AuthBodyStrucType;

/**
 *
 * MetricSpecSmall
 *
 */

/* Named bit constants */

#define MetricSpecSmall_mss_avail_intermittent          0
#define MetricSpecSmall_mss_avail_stored_data           1
#define MetricSpecSmall_mss_upd_aperiodic               2
#define MetricSpecSmall_mss_msmt_aperiodic              3
#define MetricSpecSmall_mss_msmt_phys_ev_id             4
#define MetricSpecSmall_mss_msmt_btb_metric             5
#define MetricSpecSmall_mss_acc_manager_initiated       8
#define MetricSpecSmall_mss_acc_agent_initiated         9
#define MetricSpecSmall_mss_cat_manual                  12
#define MetricSpecSmall_mss_cat_setting                 13
#define MetricSpecSmall_mss_cat_calculation             14

typedef struct MetricSpecSmall {
	intu32 numbits;
	intu8  data[2];
} MetricSpecSmall;

/**
 *
 * EnumPrintableString
 *
 */

typedef OSDynOctStr EnumPrintableString;

/**
 *
 * PersonId
 *
 */

typedef enum {
	unknown_person_id = 65535
} PersonId_Root;

typedef intu16 PersonId;

/**
 *
 * ObservationScanGrouped
 *
 */

typedef OSDynOctStr ObservationScanGrouped;

/**
 *
 * DataReqId
 *
 */

typedef enum {
	data_req_id_manager_initiated_min = 0,
	data_req_id_manager_initiated_max = 61439,
	data_req_id_agent_initiated = 61440
} DataReqId_Root;

typedef intu16 DataReqId;

/**
 *
 * ConfigResult
 *
 */

typedef enum {
	accepted_config = 0,
	unsupported_config = 1,
	standard_config_unknown = 2
} ConfigResult_Root;

typedef intu16 ConfigResult;

/**
 *
 * DataReqMode
 *
 */

/* Named bit constants */

#define DataReqMode_data_req_start_stop             0
#define DataReqMode_data_req_continuation           1
#define DataReqMode_data_req_scope_all              4
#define DataReqMode_data_req_scope_class            5
#define DataReqMode_data_req_scope_handle           6
#define DataReqMode_data_req_mode_single_rsp        8
#define DataReqMode_data_req_mode_time_period       9
#define DataReqMode_data_req_mode_time_no_limit     10
#define DataReqMode_data_req_person_id              12

typedef struct DataReqMode {
	intu32 numbits;
	intu8  data[2];
} DataReqMode;

/**
 *
 * DataReqModeFlags
 *
 */

/* Named bit constants */

#define DataReqModeFlags_data_req_supp_stop              0
#define DataReqModeFlags_data_req_supp_scope_all         4
#define DataReqModeFlags_data_req_supp_scope_class       5
#define DataReqModeFlags_data_req_supp_scope_handle      6
#define DataReqModeFlags_data_req_supp_mode_single_rsp   8
#define DataReqModeFlags_data_req_supp_mode_time_period  9
#define DataReqModeFlags_data_req_supp_mode_time_no_limit 10
#define DataReqModeFlags_data_req_supp_person_id         11
#define DataReqModeFlags_data_req_supp_init_agent        15

typedef struct DataReqModeFlags {
	intu32 numbits;
	intu8  data[2];
} DataReqModeFlags;

/**
 *
 * DataReqResult
 *
 */

typedef enum {
	data_req_result_no_error = 0,
	data_req_result_unspecific_error = 1,
	data_req_result_no_stop_support = 2,
	data_req_result_no_scope_all_support = 3,
	data_req_result_no_scope_class_support = 4,
	data_req_result_no_scope_handle_support = 5,
	data_req_result_no_mode_single_rsp_support = 6,
	data_req_result_no_mode_time_period_support = 7,
	data_req_result_no_mode_time_no_limit_support = 8,
	data_req_result_no_person_id_support = 9,
	data_req_result_unknown_person_id = 11,
	data_req_result_unknown_class = 12,
	data_req_result_unknown_handle = 13,
	data_req_result_unsupp_scope = 14,
	data_req_result_unsupp_mode = 15,
	data_req_result_init_manager_overflow = 16,
	data_req_result_continuation_not_supported = 17,
	data_req_result_invalid_req_id = 18
} DataReqResult_Root;

typedef intu16 DataReqResult;

/**
 *
 * SimpleNuObsValue
 *
 */

typedef FLOAT_Type SimpleNuObsValue;

/**
 *
 * BasicNuObsValue
 *
 */

typedef SFLOAT_Type BasicNuObsValue;

/**
 *
 * PmStoreCapab
 *
 */

/* Named bit constants */

#define PMStoreCapab_pmsc_var_no_of_segm             0
#define PMStoreCapab_pmsc_epi_seg_entries            4
#define PMStoreCapab_pmsc_peri_seg_entries           5
#define PMStoreCapab_pmsc_abs_time_select            6
#define PMStoreCapab_pmsc_clear_segm_by_list_sup     7
#define PMStoreCapab_pmsc_clear_segm_by_time_sup     8
#define PMStoreCapab_pmsc_clear_segm_remove          9
#define PMStoreCapab_pmsc_multi_person               12

typedef struct PMStoreCapab {
	intu32 numbits;
	intu8  data[2];
} PMStoreCapab;

/**
 *
 * SegmEntryHeader
 *
 */

/* Named bit constants */

#define SegmEntryHeader_seg_elem_hdr_absolute_time      0
#define SegmEntryHeader_seg_elem_hdr_relative_time      1
#define SegmEntryHeader_seg_elem_hdr_hires_relative_time 2

typedef struct SegmEntryHeader {
	intu32 numbits;
	intu8  data[2];
} SegmEntryHeader;

/**
 *
 * TrigSegmXferRsp
 *
 */

typedef enum {
	tsxr_successful = 0,
	tsxr_fail_no_such_segment = 1,
	tsxr_fail_clear_in_process = 2,
	tsxr_fail_segm_empty = 3,
	tsxr_fail_not_otherwise_specified = 512
} TrigSegmXferRsp_Root;

typedef intu16 TrigSegmXferRsp;

/**
 *
 * SegmEvtStatus
 *
 */

/* Named bit constants */

#define SegmEvtStatus_sevtsta_first_entry             0
#define SegmEvtStatus_sevtsta_last_entry              1
#define SegmEvtStatus_sevtsta_agent_abort             4
#define SegmEvtStatus_sevtsta_manager_confirm         8
#define SegmEvtStatus_sevtsta_manager_abort           12

typedef struct SegmEvtStatus {
	intu32 numbits;
	intu8  data[2];
} SegmEvtStatus;

/**
 *
 * SegmStatType
 *
 */

typedef enum {
	segm_stat_type_undefined = 0,
	segm_stat_type_minimum = 1,
	segm_stat_type_maximum = 2,
	segm_stat_type_average = 3
} SegmStatType_Root;

typedef intu16 SegmStatType;

/**
 *
 * ProdSpecEntry_spec_type
 *
 */

typedef enum {
	unspecified = 0,
	serial_number = 1,
	part_number = 2,
	hw_revision = 3,
	sw_revision = 4,
	fw_revision = 5,
	protocol_revision = 6,
	prod_spec_gmdn = 7
} ProdSpecEntry_spec_type_Root;

typedef intu16 ProdSpecEntry_spec_type;

/**
 *
 * SampleType_significant_bits
 *
 */

typedef enum {
	signed_samples = 255
} SampleType_significant_bits_Root;

typedef intu8 SampleType_significant_bits;

/**
 *
 * MetricStructureSmall_ms_struct
 *
 */

typedef enum {
	ms_struct_simple = 0,
	ms_struct_compound = 1,
	ms_struct_reserved = 2,
	ms_struct_compound_fix = 3
} MetricStructureSmall_ms_struct_Root;

typedef intu8 MetricStructureSmall_ms_struct;

/**
 *
 * TYPE
 *
 */

typedef struct TYPE {
	NomPartition partition;
	OID_Type code;
} TYPE;

/**
 *
 * AVA_Type
 *
 */
typedef struct AVA_Type {
	OID_Type attribute_id;
	Any attribute_value;
} AVA_Type;

/**
 *
 * List of AVA_Type
 *
 */
typedef struct AttributeList {
	intu16 count;
	intu16 length;
	AVA_Type value[1]; /* first element of the array */
} AttributeList;

/**
 *
 * AttributeIdList
 *
 */

typedef struct AttributeIdList {
	intu32 n;
	OID_Type *elem;
} AttributeIdList;

/**
 *
 * AbsoluteTime
 *
 */

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

/**
 *
 * SystemModel
 *
 */

typedef struct SystemModel {
	OSDynOctStr manufacturer;
	OSDynOctStr model_number;
} SystemModel;

/**
 *
 * ProdSpecEntry
 *
 */

typedef struct ProdSpecEntry {
	ProdSpecEntry_spec_type spec_type;
	PrivateOid component_id;
	OSDynOctStr prod_spec;
} ProdSpecEntry;

/**
 *
 * List of ProductionSpecEntry
 *
 */
typedef struct ProductionSpec {
	intu16 count;
	intu16 length;
	ProdSpecEntry value[1]; /* first element of the array */
} ProductionSpec;

/**
 *
 * BatMeasure
 *
 */

typedef struct BatMeasure {
	FLOAT_Type value;
	OID_Type unit;
} BatMeasure;

/**
 *
 * NuObsValue
 *
 */

typedef struct NuObsValue {
	OID_Type metric_id;
	MeasurementStatus state;
	OID_Type unit_code;
	FLOAT_Type value;
} NuObsValue;

/**
 *
 * List of NuObsValueCmp
 *
 */
typedef struct NuObsValueCmp {
	intu16 count;
	intu16 length;
	NuObsValue value[1]; /* first element of the array */
} NuObsValueCmp;

/**
 *
 * SampleType
 *
 */

typedef struct SampleType {
	intu8 sample_size;
	SampleType_significant_bits significant_bits;
} SampleType;

/**
 *
 * SaSpec
 *
 */

typedef struct SaSpec {
	intu16 array_size;
	SampleType sample_type;
	SaFlags flags;
} SaSpec;

/**
 *
 * ScaleRangeSpec8
 *
 */

typedef struct ScaleRangeSpec8 {
	FLOAT_Type lower_absolute_value;
	FLOAT_Type upper_absolute_value;
	intu8 lower_scaled_value;
	intu8 upper_scaled_value;
} ScaleRangeSpec8;

/**
 *
 * ScaleRangeSpec16
 *
 */

typedef struct ScaleRangeSpec16 {
	FLOAT_Type lower_absolute_value;
	FLOAT_Type upper_absolute_value;
	intu16 lower_scaled_value;
	intu16 upper_scaled_value;
} ScaleRangeSpec16;

/**
 *
 * ScaleRangeSpec32
 *
 */

typedef struct ScaleRangeSpec32 {
	FLOAT_Type lower_absolute_value;
	FLOAT_Type upper_absolute_value;
	intu32 lower_scaled_value;
	intu32 upper_scaled_value;
} ScaleRangeSpec32;

/**
 *
 * EnumVal
 *
 */

/* Choice context tags */

#define TVC_EnumVal_enum_obj_id	(TM_CTXT|TM_PRIM|1)
#define TVC_EnumVal_enum_text_string	(TM_CTXT|TM_PRIM|2)
#define TVC_EnumVal_enum_bit_str	(TM_CTXT|TM_PRIM|16)

/* Choice tag constants */

#define T_EnumVal_enum_obj_id           1
#define T_EnumVal_enum_text_string      2
#define T_EnumVal_enum_bit_str          3

typedef struct EnumVal {
	int t;
	union {
		/* t = 1 */
		OID_Type enum_obj_id;
		/* t = 2 */
		OSDynOctStr *enum_text_string;
		/* t = 3 */
		BITS_32 *enum_bit_str;
	} u;
} EnumVal;

/**
 *
 * EnumObsValue
 *
 */

typedef struct EnumObsValue {
	OID_Type metric_id;
	MeasurementStatus state;
	EnumVal value;
} EnumObsValue;

/**
 *
 * SetTimeInvoke
 *
 */

typedef struct SetTimeInvoke {
	AbsoluteTime date_time;
	FLOAT_Type accuracy;
} SetTimeInvoke;

/**
 *
 * SegmIdList
 *
 */

typedef struct SegmIdList {
	intu32 n;
	InstNumber *elem;
} SegmIdList;

/**
 *
 * AbsTimeRange
 *
 */

typedef struct AbsTimeRange {
	AbsoluteTime from_time;
	AbsoluteTime to_time;
} AbsTimeRange;

/**
 *
 * SegmSelection
 *
 */

/* Choice tag constants */

#define T_SegmSelection_all_segments    1
#define T_SegmSelection_segm_id_list    2
#define T_SegmSelection_abs_time_range  3

typedef struct SegmSelection {
	int t;
	union {
		/* t = 1 */
		intu16 all_segments;
		/* t = 2 */
		SegmIdList *segm_id_list;
		/* t = 3 */
		AbsTimeRange *abs_time_range;
	} u;
} SegmSelection;

/**
 *
 * SegmentInfo
 *
 */

typedef struct SegmentInfo {
	InstNumber seg_inst_no;
	AttributeList seg_info;
} SegmentInfo;

/**
 *
 * SegmentInfoList
 *
 * List of SegmentInfo
 *
 */
typedef struct SegmentInfoList {
	intu16 count;
	intu16 length;
	SegmentInfo value[1]; /* first element of the array */
} SegmentInfoList;

/**
 *
 * ObservationScan
 *
 */

typedef struct ObservationScan {
	HANDLE obj_handle;
	AttributeList attributes;
} ObservationScan;

/**
 *
 * DataProto
 *
 */
typedef struct DataProto {
	DataProtoId data_proto_id;
	Any data_proto_info;
} DataProto;

/**
 *
 * DataProtoList
 *
 * List of DataProto
 *
 */
typedef struct DataProtoList {
	intu16 count;
	intu16 length;
	DataProto value[1]; /* first element of the array */
} DataProtoList;

/**
 *
 * AarqApdu
 *
 */
typedef struct AarqApdu {
	AssociationVersion assoc_version;
	DataProtoList data_proto_list;
} AarqApdu;

/**
 *
 * AareApdu
 *
 */
typedef struct AareApdu {
	AssociateResult result;
	DataProto selected_data_proto;
} AareApdu;

/**
 *
 * RlrqApdu
 *
 */
typedef struct RlrqApdu {
	ReleaseRequestReason reason;
} RlrqApdu;

/**
 *
 * RlreApdu
 *
 */
typedef struct RlreApdu {
	ReleaseResponseReason reason;
} RlreApdu;

/**
 *
 * AbrtApdu
 *
 */

typedef struct AbrtApdu {
	Abort_reason reason;
} AbrtApdu;

/**
 *
 * ApduType
 *
 */

/* Choice context tags */

#define TVC_ApduType_aarq	(TM_CTXT|TM_CONS|57856)
#define TVC_ApduType_aare	(TM_CTXT|TM_CONS|58112)
#define TVC_ApduType_rlrq	(TM_CTXT|TM_CONS|58368)
#define TVC_ApduType_rlre	(TM_CTXT|TM_CONS|58624)
#define TVC_ApduType_abrt	(TM_CTXT|TM_CONS|58880)
#define TVC_ApduType_prst	(TM_CTXT|TM_PRIM|59136)

/* Choice tag constants */

#define T_ApduType_aarq                 1
#define T_ApduType_aare                 2
#define T_ApduType_rlrq                 3
#define T_ApduType_rlre                 4
#define T_ApduType_abrt                 5
#define T_ApduType_prst                 6

typedef struct ApduType {
	int t;
	union {
		/* t = 1 */
		AarqApdu *aarq;
		/* t = 2 */
		AareApdu *aare;
		/* t = 3 */
		RlrqApdu *rlrq;
		/* t = 4 */
		RlreApdu *rlre;
		/* t = 5 */
		AbrtApdu *abrt;
		/* t = 6 */
		PrstApdu *prst;
	} u;
} ApduType;

/**
 *
 * DataReqModeCapab
 *
 */

typedef struct DataReqModeCapab {
	DataReqModeFlags data_req_mode_flags;
	intu8 data_req_init_agent_count;
	intu8 data_req_init_manager_count;
} DataReqModeCapab;

/**
 *
 * PhdAssociationInformation
 *
 */

typedef struct PhdAssociationInformation {
	ProtocolVersion protocol_version;
	EncodingRules encoding_rules;
	NomenclatureVersion nomenclature_version;
	FunctionalUnits functional_units;
	SystemType system_type;
	OSDynOctStr system_id;
	ConfigId dev_config_id;
	DataReqModeCapab data_req_mode_capab;
	AttributeList option_list;
} PhdAssociationInformation;

/**
 *
 * ManufSpecAssociationInformation
 *
 */

// TODO: test
// typedef struct ManufSpecAssociationInformation {
//   UuidIdent data_proto_id_ext;
//   Any data_proto_info_ext;
// } ManufSpecAssociationInformation;

typedef struct ManufSpecAssociationInformation {
	UuidIdent data_proto_id_ext;
	Any data_proto_info_ext;
} ManufSpecAssociationInformation;

/**
 *
 * EventReportArgumentSimple
 *
 */
typedef struct EventReportArgumentSimple {
	HANDLE obj_handle;
	RelativeTime event_time;
	OID_Type event_type;
	Any event_info;
} EventReportArgumentSimple;

/**
 *
 * GetArgumentSimple
 *
 */

typedef struct GetArgumentSimple {
	HANDLE obj_handle;
	AttributeIdList attribute_id_list;
} GetArgumentSimple;

/**
 *
 * AttributeModEntry
 *
 */

typedef struct AttributeModEntry {
	ModifyOperator modify_operator;
	AVA_Type attribute;
} AttributeModEntry;

/**
 *
 * ModificationList
 *
 */
typedef struct ModificationList {
	intu16 count;
	intu16 length;
	AttributeModEntry value[1]; /* first element of the array */
} ModificationList;

/**
 *
 * SetArgumentSimple
 *
 */

typedef struct SetArgumentSimple {
	HANDLE obj_handle;
	ModificationList modification_list;
} SetArgumentSimple;

/**
 *
 * ActionArgumentSimple
 *
 */
typedef struct ActionArgumentSimple {
	HANDLE obj_handle;
	OID_Type action_type;
	Any action_info_args;
} ActionArgumentSimple;

/**
 *
 * EventReportResultSimple
 *
 */

typedef struct EventReportResultSimple {
	HANDLE obj_handle;
	RelativeTime currentTime;
	OID_Type event_type;
	Any event_reply_info;
} EventReportResultSimple;

/**
 *
 * GetResultSimple
 *
 */

typedef struct GetResultSimple {
	HANDLE obj_handle;
	AttributeList attribute_list;
} GetResultSimple;

/**
 *
 * SetResultSimple
 *
 */

typedef struct SetResultSimple {
	HANDLE obj_handle;
	AttributeList attribute_list;
} SetResultSimple;

/**
 *
 * ActionResultSimple
 *
 */

typedef struct ActionResultSimple {
	HANDLE obj_handle;
	OID_Type action_type;
	Any action_info_args;
} ActionResultSimple;

/**
 *
 * ErrorResult
 *
 */

typedef struct ErrorResult {
	RoerErrorValue error_value;
	Any parameter;
} ErrorResult;

/**
 *
 * RejectResult
 *
 */

typedef struct RejectResult {
	RorjProblem problem;
} RejectResult;

/**
 *
 * DataApdu_message
 *
 */

/* Choice context tags */

#define TVC_DataApdu_message_roiv_cmip_event_report	(TM_CTXT|TM_CONS|256)
#define TVC_DataApdu_message_roiv_cmip_confirmed_event_report	(TM_CTXT|TM_CONS|257)
#define TVC_DataApdu_message_roiv_cmip_get	(TM_CTXT|TM_CONS|259)
#define TVC_DataApdu_message_roiv_cmip_set	(TM_CTXT|TM_CONS|260)
#define TVC_DataApdu_message_roiv_cmip_confirmed_set	(TM_CTXT|TM_CONS|261)
#define TVC_DataApdu_message_roiv_cmip_action	(TM_CTXT|TM_CONS|262)
#define TVC_DataApdu_message_roiv_cmip_confirmed_action	(TM_CTXT|TM_CONS|263)
#define TVC_DataApdu_message_rors_cmip_confirmed_event_report	(TM_CTXT|TM_CONS|513)
#define TVC_DataApdu_message_rors_cmip_get	(TM_CTXT|TM_CONS|515)
#define TVC_DataApdu_message_rors_cmip_confirmed_set	(TM_CTXT|TM_CONS|517)
#define TVC_DataApdu_message_rors_cmip_confirmed_action	(TM_CTXT|TM_CONS|519)
#define TVC_DataApdu_message_roer	(TM_CTXT|TM_CONS|768)
#define TVC_DataApdu_message_rorj	(TM_CTXT|TM_CONS|1024)

/* Choice tag constants */

#define T_DataApdu_message_roiv_cmip_event_report 1
#define T_DataApdu_message_roiv_cmip_confirmed_event_report 2
#define T_DataApdu_message_roiv_cmip_get 3
#define T_DataApdu_message_roiv_cmip_set 4
#define T_DataApdu_message_roiv_cmip_confirmed_set 5
#define T_DataApdu_message_roiv_cmip_action 6
#define T_DataApdu_message_roiv_cmip_confirmed_action 7
#define T_DataApdu_message_rors_cmip_confirmed_event_report 8
#define T_DataApdu_message_rors_cmip_get 9
#define T_DataApdu_message_rors_cmip_confirmed_set 10
#define T_DataApdu_message_rors_cmip_confirmed_action 11
#define T_DataApdu_message_roer         12
#define T_DataApdu_message_rorj         13

typedef struct DataApdu_message {
	int t;
	union {
		/* t = 1 */
		EventReportArgumentSimple *roiv_cmip_event_report;
		/* t = 2 */
		EventReportArgumentSimple *roiv_cmip_confirmed_event_report;
		/* t = 3 */
		GetArgumentSimple *roiv_cmip_get;
		/* t = 4 */
		SetArgumentSimple *roiv_cmip_set;
		/* t = 5 */
		SetArgumentSimple *roiv_cmip_confirmed_set;
		/* t = 6 */
		ActionArgumentSimple *roiv_cmip_action;
		/* t = 7 */
		ActionArgumentSimple *roiv_cmip_confirmed_action;
		/* t = 8 */
		EventReportResultSimple *rors_cmip_confirmed_event_report;
		/* t = 9 */
		GetResultSimple *rors_cmip_get;
		/* t = 10 */
		SetResultSimple *rors_cmip_confirmed_set;
		/* t = 11 */
		ActionResultSimple *rors_cmip_confirmed_action;
		/* t = 12 */
		ErrorResult *roer;
		/* t = 13 */
		RejectResult *rorj;
	} u;
} DataApdu_message;

/**
 *
 * DataApdu
 *
 */

typedef struct DataApdu {
	InvokeIDType invoke_id;
	DataApdu_message message;
} DataApdu;

/**
 *
 * TypeVer
 *
 */

typedef struct TypeVer {
	OID_Type type;
	intu16 version;
} TypeVer;

/**
 *
 * TypeVerList
 *
 */
typedef struct TypeVerList {
	intu16 count;
	intu16 length;
	TypeVer value[1]; /* first element of the array */
} TypeVerList;

/**
 *
 * AttrValMapEntry
 *
 */
typedef struct AttrValMapEntry {
	OID_Type attribute_id;
	intu16 attribute_len;
} AttrValMapEntry;

/**
 *
 * AttrValMap
 *
 */
typedef struct AttrValMap {
	intu16 count;
	intu16 length;
	AttrValMapEntry value[1]; /* first element of the array */
} AttrValMap;

/**
 *
 * MdsTimeInfo
 *
 */
typedef struct MdsTimeInfo {
	MdsTimeCapState mds_time_cap_state;
	TimeProtocolId time_sync_protocol;
	RelativeTime time_sync_accuracy;
	intu16 time_resolution_abs_time;
	intu16 time_resolution_rel_time;
	intu32 time_resolution_high_res_time;
} MdsTimeInfo;

/**
 *
 * AuthBodyAndStrucType
 *
 */

typedef struct AuthBodyAndStrucType {
	AuthBody auth_body;
	AuthBodyStrucType auth_body_struc_type;
} AuthBodyAndStrucType;

/**
 *
 * RegCertData
 *
 */

typedef struct RegCertData {
	AuthBodyAndStrucType auth_body_and_struc_type;
	Any auth_body_data;
} RegCertData;

/**
 *
 * RegCertDataList
 *
 */
typedef struct RegCertDataList {
	intu16 count;
	intu16 length;
	RegCertData value[1]; /* first element of the array */
} RegCertDataList;

/**
 *
 * SupplementalTypeList
 *
 */
typedef struct SupplementalTypeList {
	intu16 count;
	intu16 length;
	TYPE value[1]; /* first element of the array */
} SupplementalTypeList;

/**
 *
 * MetricStructureSmall
 *
 */

typedef struct MetricStructureSmall {
	MetricStructureSmall_ms_struct ms_struct;
	intu8 ms_comp_no;
} MetricStructureSmall;

/**
 *
 * MetricIdList
 *
 */

typedef struct MetricIdList {
	intu32 n;
	OID_Type *elem;
} MetricIdList;

/**
 *
 * HandleAttrValMapEntry
 *
 */

typedef struct HandleAttrValMapEntry {
	HANDLE obj_handle;
	AttrValMap attr_val_map;
} HandleAttrValMapEntry;

/**
 *
 * HandleAttrValMap
 *
 */
typedef struct HandleAttrValMap {
	intu16 count;
	intu16 length;
	HandleAttrValMapEntry value[1]; /* first element of the array */
} HandleAttrValMap;

/**
 *
 * HANDLEList
 *
 */

typedef struct HANDLEList {
	intu32 n;
	HANDLE *elem;
} HANDLEList;

/**
 *
 * _SeqOfObservationScan
 *
 */
typedef struct _SeqOfObservationScan {
	intu16 count;
	intu16 length;
	ObservationScan value[1]; /* first element of the array */
} _SeqOfObservationScan;

/**
 *
 * ScanReportInfoVar
 *
 */

typedef struct ScanReportInfoVar {
	DataReqId data_req_id;
	intu16 scan_report_no;
	_SeqOfObservationScan obs_scan_var;
} ScanReportInfoVar;

/**
 *
 * ObservationScanFixed
 *
 */

typedef struct ObservationScanFixed {
	HANDLE obj_handle;
	OSDynOctStr obs_val_data;
} ObservationScanFixed;

/**
 *
 * _SeqOfObservationScanFixed
 *
 */
typedef struct _SeqOfObservationScanFixed {
	intu16 count;
	intu16 length;
	ObservationScanFixed value[1]; /* first element of the array */
} _SeqOfObservationScanFixed;

/**
 *
 * ScanReportInfoFixed
 *
 */

typedef struct ScanReportInfoFixed {
	DataReqId data_req_id;
	intu16 scan_report_no;
	_SeqOfObservationScanFixed obs_scan_fixed;
} ScanReportInfoFixed;

/**
 *
 * _SeqOfObservationScanGrouped
 *
 */
typedef struct _SeqOfObservationScanGrouped {
	intu16 count;
	intu16 length;
	ObservationScanGrouped value[1]; /* first element of the array */
} _SeqOfObservationScanGrouped;

/**
 *
 * ScanReportInfoGrouped
 *
 */

typedef struct ScanReportInfoGrouped {
	intu16 data_req_id;
	intu16 scan_report_no;
	_SeqOfObservationScanGrouped obs_scan_grouped;
} ScanReportInfoGrouped;

/**
 *
 * ScanReportPerVar
 *
 */

typedef struct ScanReportPerVar {
	PersonId person_id;
	_SeqOfObservationScan obs_scan_var;
} ScanReportPerVar;

/**
 *
 * _SeqOfScanReportPerVar
 *
 */
typedef struct _SeqOfScanReportPerVar {
	intu16 count;
	intu16 length;
	ScanReportPerVar value[1]; /* first element of the array */
} _SeqOfScanReportPerVar;

/**
 *
 * ScanReportInfoMPVar
 *
 */

typedef struct ScanReportInfoMPVar {
	DataReqId data_req_id;
	intu16 scan_report_no;
	_SeqOfScanReportPerVar scan_per_var;
} ScanReportInfoMPVar;

/**
 *
 * ScanReportPerFixed
 *
 */

typedef struct ScanReportPerFixed {
	PersonId person_id;
	_SeqOfObservationScanFixed obs_scan_fixed;
} ScanReportPerFixed;

/**
 *
 * _SeqOfScanReportPerFixed
 *
 */
typedef struct _SeqOfScanReportPerFixed {
	intu16 count;
	intu16 length;
	ScanReportPerFixed value[1]; /* first element of the array */
} _SeqOfScanReportPerFixed;

/**
 *
 * ScanReportInfoMPFixed
 *
 */

typedef struct ScanReportInfoMPFixed {
	DataReqId data_req_id;
	intu16 scan_report_no;
	_SeqOfScanReportPerFixed scan_per_fixed;
} ScanReportInfoMPFixed;

/**
 *
 * ScanReportPerGrouped
 *
 */

typedef struct ScanReportPerGrouped {
	PersonId person_id;
	ObservationScanGrouped obs_scan_grouped;
} ScanReportPerGrouped;

/**
 *
 * _SeqOfScanReportPerGrouped
 *
 */
typedef struct _SeqOfScanReportPerGrouped {
	intu16 count;
	intu16 length;
	ScanReportPerGrouped value[1]; /* first element of the array */
} _SeqOfScanReportPerGrouped;

/**
 *
 * ScanReportInfoMPGrouped
 *
 */

typedef struct ScanReportInfoMPGrouped {
	intu16 data_req_id;
	intu16 scan_report_no;
	_SeqOfScanReportPerGrouped scan_per_grouped;
} ScanReportInfoMPGrouped;

/**
 *
 * ConfigObject
 *
 */

typedef struct ConfigObject {
	OID_Type obj_class;
	HANDLE obj_handle;
	AttributeList attributes;
} ConfigObject;

/**
 *
 * ConfigObjectList
 *
 */
typedef struct ConfigObjectList {
	intu16 count;
	intu16 length;
	ConfigObject value[1]; /* first element of the array */
} ConfigObjectList;

/**
 *
 * ConfigReport
 *
 */

typedef struct ConfigReport {
	ConfigId config_report_id;
	ConfigObjectList config_obj_list;
} ConfigReport;

/**
 *
 * ConfigReportRsp
 *
 */

typedef struct ConfigReportRsp {
	ConfigId config_report_id;
	ConfigResult config_result;
} ConfigReportRsp;

/**
 *
 * DataRequest
 *
 */

typedef struct DataRequest {
	DataReqId data_req_id;
	DataReqMode data_req_mode;
	RelativeTime data_req_time;
	intu16 data_req_person_id;
	OID_Type data_req_class;
	HANDLEList data_req_obj_handle_list;
} DataRequest;

/**
 *
 * DataResponse
 *
 */

typedef struct DataResponse {
	RelativeTime rel_time_stamp;
	DataReqResult data_req_result;
	OID_Type event_type;
	Any event_info;
} DataResponse;

/**
 *
 * SimpleNuObsValueCmp
 *
 */

typedef struct SimpleNuObsValueCmp {
	intu32 n;
	SimpleNuObsValue *elem;
} SimpleNuObsValueCmp;

/**
 *
 * BasicNuObsValueCmp
 *
 */

typedef struct BasicNuObsValueCmp {
	intu32 n;
	BasicNuObsValue *elem;
} BasicNuObsValueCmp;

/**
 *
 * SegmEntryElem
 *
 */

typedef struct SegmEntryElem {
	OID_Type class_id;
	TYPE metric_type;
	HANDLE handle;
	AttrValMap attr_val_map;
} SegmEntryElem;

/**
 *
 * SegmEntryElemList
 *
 */
typedef struct SegmEntryElemList {
	intu16 count;
	intu16 length;
	SegmEntryElem value[1]; /* first element of the array */
} SegmEntryElemList;

/**
 *
 * PmSegmentEntryMap
 *
 */

typedef struct PmSegmentEntryMap {
	SegmEntryHeader segm_entry_header;
	SegmEntryElemList segm_entry_elem_list;
} PmSegmentEntryMap;

/**
 *
 * TrigSegmDataXferReq
 *
 */

typedef struct TrigSegmDataXferReq {
	InstNumber seg_inst_no;
} TrigSegmDataXferReq;

/**
 *
 * TrigSegmDataXferRsp
 *
 */

typedef struct TrigSegmDataXferRsp {
	InstNumber seg_inst_no;
	TrigSegmXferRsp trig_segm_xfer_rsp;
} TrigSegmDataXferRsp;

/**
 *
 * SegmDataEventDescr
 *
 */

typedef struct SegmDataEventDescr {
	InstNumber segm_instance;
	intu32 segm_evt_entry_index;
	intu32 segm_evt_entry_count;
	SegmEvtStatus segm_evt_status;
} SegmDataEventDescr;

/**
 *
 * SegmentDataEvent
 *
 */
typedef struct SegmentDataEvent {
	SegmDataEventDescr segm_data_event_descr;
	OSDynOctStr segm_data_event_entries;
} SegmentDataEvent;

/**
 *
 * SegmentDataResult
 *
 */
typedef struct SegmentDataResult {
	SegmDataEventDescr segm_data_event_descr;
} SegmentDataResult;

/**
 *
 * SegmentStatisticEntry
 *
 */
typedef struct SegmentStatisticEntry {
	SegmStatType segm_stat_type;
	OSDynOctStr segm_stat_entry;
} SegmentStatisticEntry;

/**
 *
 * SegmentStatistics
 *
 * List of SegmentStatisticEntry
 *
 */
typedef struct SegmentStatistics {
	intu16 count;
	intu16 length;
	SegmentStatisticEntry value[1]; /* first element of the array */
} SegmentStatistics;

/** @} */

#endif /* PHD_TYPES_GENERATED_H_ */
