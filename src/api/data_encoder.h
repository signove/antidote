/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file data_encoder.h
 * \brief Utility decoding functions for DataEntry type.
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
 * \author Fabricio Silva
 * \date Jul 2, 2010
 */

#ifndef DATA_ENCODER_H_
#define DATA_ENCODER_H_

#include "src/asn1/phd_types.h"
#include "api_definitions.h"
#include "data_list.h"

char *data_strcp(const char *str);

// Meta attributes
void data_set_meta_att(DataEntry *data, char *name, char *value);
void data_meta_set_handle(DataEntry *data, HANDLE value);
void data_meta_set_part_code(DataEntry *data, int part_code);
void data_meta_set_attr_id(DataEntry *data, intu16 attr_id);
void data_meta_set_personal_id(DataEntry *data, intu16 personal_id);

// Data Entries
void data_set_float(DataEntry *data, char *att_name, FLOAT_Type *type);

void data_set_nu_obs_val_cmp(DataEntry *data,
			     char *att_name, NuObsValueCmp *value, intu16 partition);

void data_set_simple_nu_obs_value(DataEntry *data, char *att_name, SimpleNuObsValue *value);

void data_set_simple_nu_obs_val_cmp(DataEntry *data, char *att_name,
				    SimpleNuObsValueCmp *value, intu16 partition,
				    OID_Type *metric_id_list);

void data_set_basic_nu_obs_val(DataEntry *data, char *att_name,
			       BasicNuObsValue *value);

void data_set_basic_nu_obs_val_cmp(DataEntry *data, char *att_name,
				   BasicNuObsValueCmp *value, intu16 partition,
				   OID_Type *metric_id_list);
void data_set_observed_value_simple_OID(DataEntry *data, char *att_name, OID_Type oid_type);
void data_set_observed_value_simple_bit_str(DataEntry *data, char *att_name,
		BITS_32 simple_bit_str);
void data_set_observed_value_basic_bit_str(DataEntry *data, char *att_name,
		BITS_16 basic_bit_str);
void data_set_observed_value_simple_str(DataEntry *data, char *att_name,
					octet_string *simple_str);
void data_set_enum_observed_value(DataEntry *data, char *att_name,
				  EnumObsValue *enum_obs_value);
void data_set_value_partition(DataEntry *data, char *att_name,
			      NomPartition part_value);

void data_set_nu_obs_val(DataEntry *data, char *att_name, NuObsValue *value);

void data_set_absolute_time(DataEntry *data, char *att_name, AbsoluteTime *value);

void data_set_production_spec(DataEntry *data,  char *att_name,
			      ProductionSpec *spec);

void data_set_dev_config_id(DataEntry *data,  char *att_name, ConfigId *confid);

void data_set_system_id(DataEntry *data,  char *att_name, octet_string *system_id);
void data_set_sys_type_spec_list(DataEntry *data, char *att_name,
		TypeVerList *system_type_spec_list);

void data_set_type(DataEntry *data,  char *att_name, TYPE *type);

void data_set_system_model(DataEntry *data,  char *att_name,
			   SystemModel *system_model);

void data_set_sample_period(DataEntry *data, char *att_name,
			    RelativeTime sample_period);
void data_set_simple_sa_observed_value(DataEntry *data, char *att_name,
				       octet_string *simple_sa_observed_value);
void data_set_scale_and_range_specification_8(DataEntry *data, char *att_name,
		ScaleRangeSpec8 *scale_and_range_specification_8);
void data_set_scale_and_range_specification_16(DataEntry *data, char *att_name,
		ScaleRangeSpec16 *scale_and_range_specification_16);
void data_set_scale_and_range_specification_32(DataEntry *data, char *att_name,
		ScaleRangeSpec32 *scale_and_range_specification_32);
void data_set_sa_specification(DataEntry *data, char *att_name,
			       SaSpec *sa_specification);

void data_set_oid_type(DataEntry *data, char *att_name, OID_Type *type);
void data_set_handle(DataEntry *data, char *att_name, HANDLE *handle);
void data_set_attribute_metric_spec_small(DataEntry *data, char *att_name, MetricSpecSmall *spec_small);
void data_set_attribute_metric_structure_small(DataEntry *data, char *att_name, MetricStructureSmall *struct_small);
void data_set_attribute_value_map(DataEntry *data, char *att_name, AttrValMap *val_map);
void data_set_metric_id_list(DataEntry *data, char *att_name, MetricIdList *list);
void data_set_handle_list(DataEntry *data, char *att_name, HANDLEList *list);
void data_set_handle_attr_val_map(DataEntry *data, char *att_name, HandleAttrValMap *map);
void data_set_pm_segment_entry_map(DataEntry *data, char *att_name, PmSegmentEntryMap *map);
void data_set_intu16(DataEntry *data, char *att_name, intu16 *value);
void data_set_intu32(DataEntry *data, char *att_name, intu32 *value);
void data_set_high_res_relative_time(DataEntry *data, char *att_name, HighResRelativeTime *time);
void data_set_absolute_time_adj(DataEntry *data, char *att_name, AbsoluteTimeAdjust *adj);

void data_set_label_string(DataEntry *data, char *att_name, octet_string *str);

#endif /* DATA_ENCODER_H_ */
