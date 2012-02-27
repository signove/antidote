/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file data_encoder.c
 * \brief Implementation of data_encode.h header.
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

#include "data_encoder.h"
#include "data_list.h"
#include "text_encoder.h"
#include "src/util/strbuff.h"
#include "src/asn1/phd_types.h"
#include "api_definitions.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <arpa/inet.h>

/**
 *
 * \addtogroup DataEncoder Data Encoder
 * \ingroup API
 *
 * \brief Data encoder parses types of IEEE layer into data entries for high
 * level application usage.
 *
 * @{
 */

/**
 * Fills a simple data entry.
 *
 * @param simple data entry to be filled.
 * @param name to set name field, this value will be deallocated on data-entry destruction.
 * @param type to set type field, this value will be deallocated on data-entry destruction.
 * @param value to set value field, this value will be deallocated on data-entry destruction.
 */
static void fill_simple(SimpleDataEntry *simple, char *name, char *type,
			char *value)
{
	if (simple == NULL)
		return;

	simple->name = name;
	simple->type = type;
	simple->value = value;
}

/**
 * Sets data entry as simple data entry and fill the field values.
 *
 * @param data entry to be filled.
 * @param name to set name field, this value will be deallocated on data-entry destruction.
 * @param type to set type field, this value will be deallocated on data-entry destruction.
 * @param value to set value field, this value will be deallocated on data-entry destruction.
 */
static void set_simple(DataEntry *data, char *name, char *type, char *value)
{
	if (data == NULL)
		return;

	data->choice = SIMPLE_DATA_ENTRY;
	fill_simple(&data->u.simple, name, type, value);
}

/**
 * Set data entry as compound data entry.
 *
 * @param data compound data.
 * @param name to set name field, this value will be deallocated on data-entry destruction.
 * @param size number of child entries.
 */
static void set_cmp(DataEntry *data, char *name, int size)
{
	if (data == NULL)
		return;

	data->choice = COMPOUND_DATA_ENTRY;
	data->u.compound.name = name;
	data->u.compound.entries_count = size;
	data->u.compound.entries = calloc(size, sizeof(DataEntry));
}

/**
 * Allocates a string in a new memory area and initializes its contents with
 * the same value as the given string.
 *
 * @param str the initial value of the string just created.
 * @return string a new string with the same value of the parameter.
 */
char *data_strcp(const char *str)
{
	int len = strlen(str);
	char *result = calloc(len + 1, sizeof(char));
	strcpy(result, str);
	return result;
}

/**
 * Sets meta data attribute of this entry.
 *
 * @param data the entry to be modified.
 * @param name name of meta-data attribute, this value will be deallocated on data-entry destruction.
 * @param value value of meta-data attribute, this value will be deallocated on data-entry destruction.
 */
void data_set_meta_att(DataEntry *data, char *name, char *value)
{
	if (data == NULL)
		return;

	// test if there is not elements in the list
	if (data->meta_data.size == 0) {
		data->meta_data.values = malloc(sizeof(struct MetaAtt));
	} else {
		// change the list size
		data->meta_data.values = realloc(data->meta_data.values,
						 sizeof(struct MetaAtt) *
						 (data->meta_data.size + 1));
	}

	// add element to list
	if (data->meta_data.values == NULL)
		return;

	MetaAtt *meta = &data->meta_data.values[data->meta_data.size];
	meta->name = name;
	meta->value = value;

	data->meta_data.size += 1;

}

/**
 * Fills compound child entry.
 *
 * @param cmp compound data entry.
 * @param index of child entry in this compound.
 * @param name to set name field, this value will be deallocated on data-entry destruction.
 * @param type to set type field, this value will be deallocated on data-entry destruction.
 * @param value to set value field, this value will be deallocated on data-entry destruction.
 */
static void fill_cmp_child(DataEntry *cmp, int index, char *name, char *type,
			   char *value)
{
	if (cmp == NULL)
		return;

	set_simple(&cmp->u.compound.entries[index], name, type, value);
}

/**
 * Sets meta attribute HANDLE.
 *
 * @param data the entry to have \b HANDLE attribute set.
 * @param value the new value of entry's \b HANDLE attribute.
 */
void data_meta_set_handle(DataEntry *data, HANDLE value)
{
	if (data == NULL)
		return;

	data_set_meta_att(data, data_strcp("HANDLE"), int2str(value));
}

/**
 * Sets meta attribute partition-code.
 *
 * @param data the entry to have \b partition-code attribute set.
 * @param part_code the new value of entry's \b partition-code attribute.
 */
void data_meta_set_part_code(DataEntry *data, int part_code)
{
	if (data == NULL)
		return;

	data_set_meta_att(data, data_strcp("partition-code"), int2str(part_code));
}

/**
 * Sets meta attribute attribute-id.
 *
 * @param data the entry to have \b attribute-id attribute set.
 * @param attr_id the new value of entry's \b attribute-id attribute.
 */
void data_meta_set_attr_id(DataEntry *data, intu16 attr_id)
{
	if (data == NULL)
		return;

	data_set_meta_att(data, data_strcp("attribute-id"), intu16_2str(attr_id));
}

/**
 * Sets meta attribute personal-id.
 *
 * @param data the entry to have \b personal-id attribute set.
 * @param personal_id the new value of entry's \b personal-id attribute.
 */
void data_meta_set_personal_id(DataEntry *data, intu16 personal_id)
{
	if (data == NULL)
		return;

	data_set_meta_att(data, data_strcp("personal-id"), intu16_2str(personal_id));
}

/**
 * Sets data entry with passed type.
 *
 * @param data the entry to have its attribute set.
 * @param att_name the name of DIM attribute.
 * @param value the new value of DIM attribute.
 */
void data_set_float(DataEntry *data,  char *att_name,
		    FLOAT_Type *value)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_FLOAT, float2str(*value));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param value
 */
void data_set_simple_nu_obs_value(DataEntry *data,
				  char *att_name, SimpleNuObsValue *value)
{
	if (data == NULL)
		return;


	set_simple(data, data_strcp(att_name), APIDEF_TYPE_FLOAT, float2str(*value));

}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param value
 * @param partition partition code used
 * @param metric_id_list list of metric-ids used in the meta-data node
 */
void data_set_simple_nu_obs_val_cmp(DataEntry *data,
				    char *att_name, SimpleNuObsValueCmp *value, intu16 partition,
				    OID_Type *metric_id_list)
{
	if (data == NULL)
		return;


	set_cmp(data, data_strcp(att_name), value->count);
	int i;

	for (i = 0; i < value->count; ++i) {
		fill_cmp_child(data, i, int2str(i), APIDEF_TYPE_FLOAT,
			       float2str(value->value[i]));

		data_set_meta_att(&(data->u.compound.entries[i]), data_strcp("partition"),
				  intu16_2str(partition));

		data_set_meta_att(&(data->u.compound.entries[i]), data_strcp("metric-id"),
				  intu16_2str(metric_id_list[i]));
	}
}


/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param system_type_spec_list the TypeVerList value
 */
void data_set_sys_type_spec_list(DataEntry *data, char *att_name, TypeVerList * system_type_spec_list)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), system_type_spec_list->count);

	int i;
	for (i = 0; i < system_type_spec_list->count; ++i) {
		DataEntry *child = &(data->u.compound.entries[i]);
		set_cmp(child, int2str(i), 2);

		fill_cmp_child(child, 0, data_strcp("version"), APIDEF_TYPE_INTU16,
				intu16_2str(system_type_spec_list->value[i].version));
		data_set_oid_type(&child->u.compound.entries[1], "type",
				&system_type_spec_list->value[i].type);
	}
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param value
 */
void data_set_basic_nu_obs_val(DataEntry *data, char *att_name,
			       BasicNuObsValue *value)
{
	if (data == NULL)
		return;


	set_simple(data, data_strcp(att_name), APIDEF_TYPE_FLOAT, float2str(*value));
}

/**
 * Set data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param value
 * @param partition partition code used
 * @param metric_id_list list of metric-ids used in the meta-data node
 */
void data_set_basic_nu_obs_val_cmp(DataEntry *data,
				   char *att_name, BasicNuObsValueCmp *value, intu16 partition,
				   OID_Type *metric_id_list)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), value->count);


	int i;

	for (i = 0; i < value->count; ++i) {
		fill_cmp_child(data, i, int2str(i), APIDEF_TYPE_FLOAT,
			       float2str(value->value[i]));

		data_set_meta_att(&(data->u.compound.entries[i]), data_strcp("partition"),
				  intu16_2str(partition));

		data_set_meta_att(&(data->u.compound.entries[i]), data_strcp("metric-id"),
				  intu16_2str(metric_id_list[i]));
	}
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param value
 */
void data_set_nu_obs_val(DataEntry *data,  char *att_name,
			 NuObsValue *value)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), 4);
	fill_cmp_child(data, 1, data_strcp("state"), APIDEF_TYPE_INTU16,
		       intu16_2str(value->state));
	fill_cmp_child(data, 2, data_strcp("unit-code"), APIDEF_TYPE_INTU16,
		       intu16_2str(value->unit_code));
	fill_cmp_child(data, 3, data_strcp("value"), APIDEF_TYPE_INTU16, float2str(
			       value->value));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param value
 * @param partition partition code used
 */
void data_set_nu_obs_val_cmp(DataEntry *data,  char *att_name,
			     NuObsValueCmp *value, intu16 partition)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), value->count);


	DataEntry *nu_obs_entry = NULL;
	int i = 0;

	for (i = 0; i < value->count; ++i) {
		nu_obs_entry = &data->u.compound.entries[i];
		NuObsValue *nu_obs = &value->value[i];

		data_set_nu_obs_val(nu_obs_entry, data_strcp("Nu-Observed-Value"), nu_obs);

		data_set_meta_att(nu_obs_entry, data_strcp("partition"),
				  intu16_2str(partition));
		data_set_meta_att(nu_obs_entry, data_strcp("metric-id"),
				  intu16_2str(nu_obs->metric_id));
	}
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param time
 */
void data_set_absolute_time(DataEntry *data, char *att_name,
			    AbsoluteTime *time)
{
	if (data == NULL)
		return;


	set_cmp(data, data_strcp(att_name), 8);
	fill_cmp_child(data, 0, data_strcp("century"), APIDEF_TYPE_INTU8,
		       bcdtime2number(time->century));
	fill_cmp_child(data, 1, data_strcp("year"), APIDEF_TYPE_INTU8,
		       bcdtime2number(time->year));
	fill_cmp_child(data, 2, data_strcp("month"), APIDEF_TYPE_INTU8,
		       bcdtime2number(time->month));
	fill_cmp_child(data, 3, data_strcp("day"), APIDEF_TYPE_INTU8,
		       bcdtime2number(time->day));
	fill_cmp_child(data, 4, data_strcp("hour"), APIDEF_TYPE_INTU8,
		       bcdtime2number(time->hour));
	fill_cmp_child(data, 5, data_strcp("minute"), APIDEF_TYPE_INTU8,
		       bcdtime2number(time->minute));
	fill_cmp_child(data, 6, data_strcp("second"), APIDEF_TYPE_INTU8,
		       bcdtime2number(time->second));
	fill_cmp_child(data, 7, data_strcp("sec_fractions"), APIDEF_TYPE_INTU8,
		       bcdtime2number(time->sec_fractions));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param adj AbsoluteTimeAdjust value
 */
void data_set_absolute_time_adj(DataEntry *data, char *att_name, AbsoluteTimeAdjust *adj)
{
	if (data == NULL)
		return;

	// AbsoluteTimeAdjust is a big-endian 6-octet uint embedded in a
	// string of 6 octets because there is no INT-U48 type.
	// Since there is no INT-U64 either, we opted by breaking
	// the value in two parts than can easily be combined by
	// client.

	const intu16* phi = (const intu16*) &adj->value[0];
	const intu32* plo = (const intu32*) &adj->value[2];

	intu16 hi = ntohs(*phi);
	intu32 lo = ntohl(*plo);

	set_cmp(data, data_strcp(att_name), 2);
	fill_cmp_child(data, 0, data_strcp("hi"), APIDEF_TYPE_INTU16, intu16_2str(hi));
	fill_cmp_child(data, 1, data_strcp("lo"), APIDEF_TYPE_INTU32, intu32_2str(lo));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param spec
 */
void data_set_production_spec(DataEntry *data, char *att_name,
			      ProductionSpec *spec)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), spec->count);



	DataEntry *prod_spec_entry = NULL;
	int i = 0;

	for (i = 0; i < spec->count; i++) {
		prod_spec_entry = &data->u.compound.entries[i];
		set_cmp(prod_spec_entry, int2str(i), 3);
		fill_cmp_child(prod_spec_entry, 0, data_strcp("component-id"),
			       APIDEF_TYPE_INTU16, intu16_2str(
				       spec->value[i].component_id));
		fill_cmp_child(prod_spec_entry, 1, data_strcp("prod-spec"),
			       APIDEF_TYPE_STRING, octet_string2str(
				       &spec->value[i].prod_spec));
		fill_cmp_child(prod_spec_entry, 2, data_strcp("spec-type"),
			       APIDEF_TYPE_INTU16, intu16_2str(
				       spec->value[i].spec_type));
	}
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param confid
 */
void data_set_dev_config_id(DataEntry *data, char *att_name,
			    ConfigId *confid)
{
	if (data == NULL)
		return;


	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INTU16, intu16_2str(
			   *confid));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param system_model
 */
void data_set_system_model(DataEntry *data, char *att_name,
			   SystemModel *system_model)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), 2);

	fill_cmp_child(data, 0, data_strcp("manufacturer"), APIDEF_TYPE_STRING,
		       octet_string2str(&system_model->manufacturer));
	fill_cmp_child(data, 1, data_strcp("model-number"), APIDEF_TYPE_STRING,
		       octet_string2str(&system_model->model_number));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param system_id
 */
void data_set_system_id(DataEntry *data, char *att_name,
			octet_string *system_id)
{
	if (data == NULL)
		return;


	set_simple(data, data_strcp(att_name), APIDEF_TYPE_HEX, octet_string2hex(
			   system_id));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param type
 */
void data_set_type(DataEntry *data, char *att_name, TYPE *type)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), 2);
	fill_cmp_child(data, 0, data_strcp("code"), APIDEF_TYPE_INTU16, intu16_2str(
			       type->code));
	fill_cmp_child(data, 1, data_strcp("partition"), APIDEF_TYPE_INTU16,
		       intu16_2str(type->partition));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param oid_type
 */
void data_set_observed_value_simple_OID(DataEntry *data, char *att_name,
					OID_Type oid_type)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INTU16,
		   intu16_2str(oid_type));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param simple_bit_str
 */
void data_set_observed_value_simple_bit_str(DataEntry *data, char *att_name,
		BITS_32 simple_bit_str)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INTU32,
		   intu32_2str(simple_bit_str));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param basic_bit_str
 */
void data_set_observed_value_basic_bit_str(DataEntry *data, char *att_name,
		BITS_16 basic_bit_str)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INTU16,
		   intu16_2str(basic_bit_str));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param simple_str
 */
void data_set_observed_value_simple_str(DataEntry *data, char *att_name,
					octet_string *simple_str)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_STRING,
		   octet_string2str(simple_str));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param enum_obs_value
 */
void data_set_enum_observed_value(DataEntry *data, char *att_name,
				  EnumObsValue *enum_obs_value)
{
	if (data == NULL)
		return;


	set_cmp(data, data_strcp(att_name), 3);
	fill_cmp_child(data, 0, data_strcp("metric-id"), APIDEF_TYPE_INTU16,
		       intu16_2str(enum_obs_value->metric_id));
	fill_cmp_child(data, 1, data_strcp("state"), APIDEF_TYPE_INTU16,
		       intu16_2str(enum_obs_value->state));

	switch (enum_obs_value->value.choice) {
	case OBJ_ID_CHOSEN:
		fill_cmp_child(data, 2, data_strcp("enum_value"), APIDEF_TYPE_INTU16,
			       intu16_2str(enum_obs_value->value.u.enum_obj_id));
		break;
	case TEXT_STRING_CHOSEN:
		fill_cmp_child(data, 2, data_strcp("enum_value"), APIDEF_TYPE_STRING,
			       octet_string2str(&(enum_obs_value->value.u.enum_text_string)));
		break;
	case BIT_STR_CHOSEN:
		fill_cmp_child(data, 2, data_strcp("enum_value"), APIDEF_TYPE_INTU32,
			       intu32_2str(enum_obs_value->value.u.enum_bit_str));
		break;
	default:
		break;
	}
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param part_value
 */
void data_set_value_partition(DataEntry *data, char *att_name,
			      NomPartition part_value)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INTU16,
		   intu16_2str(part_value));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param sample_period
 */
void data_set_sample_period(DataEntry *data, char *att_name,
			    RelativeTime sample_period)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INT32,
		   int32_2str(sample_period));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param simple_sa_observed_value
 */
void data_set_simple_sa_observed_value(DataEntry *data, char *att_name,
				       octet_string *simple_sa_observed_value)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_STRING,
		   octet_string2str(simple_sa_observed_value));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param scale_and_range_specification_8
 */
void data_set_scale_and_range_specification_8(DataEntry *data, char *att_name,
		ScaleRangeSpec8 *scale_and_range_specification_8)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), 4);
	fill_cmp_child(data, 0, data_strcp("lower_absolute_value"), APIDEF_TYPE_FLOAT,
		       float2str(scale_and_range_specification_8->lower_absolute_value));
	fill_cmp_child(data, 1, data_strcp("upper_absolute_value"), APIDEF_TYPE_FLOAT,
		       float2str(scale_and_range_specification_8->upper_absolute_value));
	fill_cmp_child(data, 2, data_strcp("lower_scaled_value"), APIDEF_TYPE_INTU8,
		       intu8_2str(scale_and_range_specification_8->lower_scaled_value));
	fill_cmp_child(data, 03, data_strcp("upper_scaled_value"), APIDEF_TYPE_INTU8,
		       intu8_2str(scale_and_range_specification_8->upper_scaled_value));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param scale_and_range_specification_16
 */
void data_set_scale_and_range_specification_16(DataEntry *data, char *att_name,
		ScaleRangeSpec16 *scale_and_range_specification_16)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), 4);
	fill_cmp_child(data, 0, data_strcp("lower_absolute_value"), APIDEF_TYPE_FLOAT,
		       float2str(scale_and_range_specification_16->lower_absolute_value));
	fill_cmp_child(data, 1, data_strcp("upper_absolute_value"), APIDEF_TYPE_FLOAT,
		       float2str(scale_and_range_specification_16->upper_absolute_value));
	fill_cmp_child(data, 2, data_strcp("lower_scaled_value"), APIDEF_TYPE_INTU8,
		       intu8_2str(scale_and_range_specification_16->lower_scaled_value));
	fill_cmp_child(data, 03, data_strcp("upper_scaled_value"), APIDEF_TYPE_INTU8,
		       intu8_2str(scale_and_range_specification_16->upper_scaled_value));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param scale_and_range_specification_32
 */
void data_set_scale_and_range_specification_32(DataEntry *data, char *att_name,
		ScaleRangeSpec32 *scale_and_range_specification_32)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), 4);
	fill_cmp_child(data, 0, data_strcp("lower_absolute_value"), APIDEF_TYPE_FLOAT,
		       float2str(scale_and_range_specification_32->lower_absolute_value));
	fill_cmp_child(data, 1, data_strcp("upper_absolute_value"), APIDEF_TYPE_FLOAT,
		       float2str(scale_and_range_specification_32->upper_absolute_value));
	fill_cmp_child(data, 2, data_strcp("lower_scaled_value"), APIDEF_TYPE_INTU8,
		       intu8_2str(scale_and_range_specification_32->lower_scaled_value));
	fill_cmp_child(data, 03, data_strcp("upper_scaled_value"), APIDEF_TYPE_INTU8,
		       intu8_2str(scale_and_range_specification_32->upper_scaled_value));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param sa_specification
 */
void data_set_sa_specification(DataEntry *data, char *att_name,
			       SaSpec *sa_specification)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), 4);
	fill_cmp_child(data, 0, data_strcp("array_size"), APIDEF_TYPE_INTU16,
		       intu16_2str(sa_specification->array_size));
	fill_cmp_child(data, 1, data_strcp("sample_size"), APIDEF_TYPE_INTU8,
		       intu8_2str(sa_specification->sample_type.sample_size));
	fill_cmp_child(data, 2, data_strcp("significan_bits"), APIDEF_TYPE_INTU8,
		       intu8_2str(sa_specification->sample_type.significant_bits));
	fill_cmp_child(data, 03, data_strcp("sa_flags"), APIDEF_TYPE_INTU16,
		       intu16_2str(sa_specification->flags));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param type the oid_type value
 */
void data_set_oid_type(DataEntry *data, char *att_name, OID_Type *type)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INTU16,
		   intu16_2str(*type));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param str the label value
 */
void data_set_label_string(DataEntry *data, char *att_name, octet_string *str)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_STRING, octet_string2str(str));
}

/**
 * Set data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param handle the handle value
 */
void data_set_handle(DataEntry *data, char *att_name, HANDLE *handle)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INTU16,
		   intu16_2str(*handle));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param spec_small the MetricSpecSmall value
 */
void data_set_attribute_metric_spec_small(DataEntry *data, char *att_name,
		MetricSpecSmall *spec_small)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INTU16,
		   intu16_2str(*spec_small));
}

/**
 * Set data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param struct_small the MetricStructureSmall value
 */
void data_set_attribute_metric_structure_small(DataEntry *data, char *att_name,
		MetricStructureSmall *struct_small)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), 2);
	fill_cmp_child(data, 0, data_strcp("ms-struct"), APIDEF_TYPE_INTU8,
		       intu8_2str(struct_small->ms_struct));
	fill_cmp_child(data, 1, data_strcp("ms-comp-no"), APIDEF_TYPE_INTU8,
		       intu8_2str(struct_small->ms_comp_no));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param val_map the MetricStructureSmall value
 */
void data_set_attribute_value_map(DataEntry *data, char *att_name, AttrValMap *val_map)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), val_map->count);
	int i;

	for (i = 0; i < val_map->count; i++) {
		set_cmp(&data->u.compound.entries[i], data_strcp("AttrValMapEntry"), 2);

		DataEntry *attr_entry = &data->u.compound.entries[i].u.compound.entries[0];
		data_set_oid_type(attr_entry, "attribute-id", &val_map->value[i].attribute_id);

		attr_entry = &data->u.compound.entries[i].u.compound.entries[1];
		set_simple(attr_entry, data_strcp("attribute-len"), APIDEF_TYPE_INTU16,
			   intu16_2str(val_map->value[i].attribute_len));
	}
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param list the MetricIdList value
 */
void data_set_metric_id_list(DataEntry *data, char *att_name, MetricIdList *list)
{
	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), list->count);
	int i;

	for (i = 0; i < list->count; i++) {
		data_set_oid_type(&data->u.compound.entries[i], "Metric-Id", &list->value[i]);
	}
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param list the HANDLEList value
 */
void data_set_handle_list(DataEntry *data, char *att_name, HANDLEList *list)
{
	int i;

	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), list->count);

	for (i = 0; i < list->count; i++) {
		data_set_handle(&data->u.compound.entries[i], "Handle", &list->value[i]);
	}
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param map the HandleAttrValMap value
 */
void data_set_handle_attr_val_map(DataEntry *data, char *att_name, HandleAttrValMap *map)
{
	int i;

	if (data == NULL)
		return;

	set_cmp(data, data_strcp(att_name), map->count);

	for (i = 0; i < map->count; i++) {
		set_cmp(&data->u.compound.entries[i], data_strcp("HandleAttrValMapEntry"), 2);

		DataEntry *entry = &data->u.compound.entries[i];

		data_set_handle(&entry->u.compound.entries[0], "obj-handle", &map->value[i].obj_handle);
		data_set_attribute_value_map(&entry->u.compound.entries[1], "attr-val-map",
						&map->value[i].attr_val_map);
	}
}

/**
 * Sets data entry with passed type.
 *
 * @param entry
 * @param att_name
 * @param list
 */
static void data_set_segment_entry_list(DataEntry *entry, char *att_name, SegmEntryElemList *list)
{
	int i;

	set_cmp(entry, data_strcp(att_name), list->count);

	for (i = 0; i < list->count; ++i) {
		SegmEntryElem *elem = &list->value[i];
		DataEntry *sub1 = &entry->u.compound.entries[i];
		DataEntry *sub2;

		set_cmp(sub1, data_strcp("segment-entry"), 4);

		sub2 = &sub1->u.compound.entries[0];
		data_set_oid_type(sub2, "class-id", &elem->class_id);

		sub2 = &sub1->u.compound.entries[1];
		data_set_type(sub2, "type", &elem->metric_type);

		sub2 = &sub1->u.compound.entries[2];
		data_set_handle(sub2, "obj-handle", &elem->handle);

		sub2 = &sub1->u.compound.entries[3];
		data_set_attribute_value_map(sub2, "attr-val-map", &(elem->attr_val_map));
	}
}

/**
 * Sets data entry with passed type.
 *
 * @param entry
 * @param att_name the name of DIM attribute
 * @param map the PM-Segment entry map
 */
void data_set_pm_segment_entry_map(DataEntry *entry, char *att_name, PmSegmentEntryMap *map)
{
	if (entry == NULL)
		return;

	set_cmp(entry, data_strcp(att_name), 2);

	data_set_intu16(&entry->u.compound.entries[0], "entry-header", &map->segm_entry_header);
	data_set_segment_entry_list(&entry->u.compound.entries[1], "entry-list",
					&map->segm_entry_elem_list);
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param value the entry value
 */
void data_set_intu16(DataEntry *data, char *att_name, intu16 *value)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INTU16, intu16_2str(*value));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param value the entry value
 */
void data_set_intu32(DataEntry *data, char *att_name, intu32 *value)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_INTU32, intu32_2str(*value));
}

/**
 * Sets data entry with passed type.
 *
 * @param data entry
 * @param att_name the name of DIM attribute
 * @param time the entry value
 */
void data_set_high_res_relative_time(DataEntry *data, char *att_name, HighResRelativeTime *time)
{
	if (data == NULL)
		return;

	set_simple(data, data_strcp(att_name), APIDEF_TYPE_HEX, high_res_relative_time2hex(
			   time));
}



/**
 * Deletes a simple data entry.
 *
 * @param pointer the simple data entry to be deleted.
 */
void data_entry_del_simple(SimpleDataEntry *pointer)
{
	if (pointer == NULL)
		return;

	free(pointer->name);
	pointer->name = NULL;
	free(pointer->value);
	pointer->value = NULL;
}

/**
 * Deletes a compound data entry.
 *
 * @param pointer the compound data entry to be deleted.
 */
void data_entry_del_compound(CompoundDataEntry *pointer)
{
	int i;

	if (pointer == NULL)
		return;

	free(pointer->name);
	pointer->name = NULL;

	for (i = 0; i < pointer->entries_count; i++) {
		data_entry_del(&pointer->entries[i]);
	}

	free(pointer->entries);
	pointer->entries = NULL;
}

/**
 * Deletes the data entry.
 *
 * @param pointer to data entry to be deleted.
 */
void data_entry_del(DataEntry *pointer)
{
	if (pointer == NULL)
		return;

	if (pointer->meta_data.values != NULL && pointer->meta_data.size > 0) {
		int i;

		for (i = 0; i < pointer->meta_data.size; i++) {
			MetaAtt *meta = &pointer->meta_data.values[i];
			free(meta->name);
			meta->name = NULL;
			free(meta->value);
			meta->value = NULL;
		}
	}

	free(pointer->meta_data.values);
	pointer->meta_data.values = NULL;

	if (pointer->choice == SIMPLE_DATA_ENTRY) {
		data_entry_del_simple(&pointer->u.simple);
	} else if (pointer->choice == COMPOUND_DATA_ENTRY) {
		data_entry_del_compound(&pointer->u.compound);
	}
}

/**
 * Creates a new empty list of elements with a given size.
 *
 * @param size the size of the new list of elements.
 * @return a pointer to a new list with \b size elements.
 */
DataList *data_list_new(int size)
{
	DataList *list;
	list = calloc(1, sizeof(DataList));
	list->size = size;
	list->values = calloc(size, sizeof(DataEntry));
	return list;
}

/**
 * Deletes all elements of the list. It also deletes the list.
 *
 * @param pointer the list of elements to be deleted.
 */
void data_list_del(DataList *pointer)
{
	if (pointer) {
		int i = 0;

		for (i = 0; i < pointer->size; i++) {
			data_entry_del(&pointer->values[i]);
		}

		free(pointer->values);
		pointer->values = NULL;

		free(pointer);
		pointer = NULL;
	}
}

/** @} */
