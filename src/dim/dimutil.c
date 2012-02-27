/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file dimutil.c
 * \brief DIM utility functions implementation.
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
 * \date Jun 11, 2010
 * \author José Martins
 */


#include "dimutil.h"
#include "mds.h"
#include "src/api/data_encoder.h"
#include "src/api/text_encoder.h"
#include "src/api/oid_string.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/util/log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @addtogroup MDS MDS
 * @{
 */

/**
 * Returns a valid metric-id for an Metric object.
 *
 * \param metric the metric instance
 *
 * \return metric-id used in the object Metric.
 */
static int dimutil_get_metric_ids(struct Metric *metric)
{
	if (metric != NULL) {
		if (metric->use_metric_id_field) {
			return metric->metric_id;
		} else {
			return metric->type.code;
		}
	}

	return 0;
}

/**
 * Returns a valid partition for an Metric object.
 *
 * \param metric the metric instance.
 *
 * \return partition used in the object Metric.
 */
static int dimutil_get_metric_partition(struct Metric *metric)
{
	if (metric != NULL) {
		if (metric->use_metric_id_partition_field) {
			return metric->metric_id_partition;
		} else {
			return metric->type.partition;
		}
	}

	return 0;
}

/**
 * Returns unit code for Metric object.
 *
 * \param metric the metric instance.
 *
 * \return unit code
 */
static int dimutil_get_unit_code(struct Metric *metric)
{
	if (metric != NULL) {
		return metric->unit_code;
	}

	return 0;
}

/**
 * Returns unit name for Metric object.
 *
 * \param metric the metric instance.
 *
 * \return unit name. Caller owns the pointer
 */
static char *dimutil_get_unit(struct Metric *metric)
{
	return strdup(oid_get_unit_code_string(dimutil_get_unit_code(metric)));
}

/**
 * Returns a valid partition for an Enumeration object.
 *
 * \param enumeration the enumeration instance.
 *
 * \return partition used in the object Enumeration.
 */
static int dimutil_get_enumeration_partition(struct Enumeration *enumeration)
{
	if (&(enumeration->metric) != NULL) {
		if (enumeration->use_nom_partition) {
			return enumeration->enum_observed_value_partition;
		} else if (enumeration->metric.use_metric_id_partition_field) {
			return enumeration->metric.metric_id_partition;
		} else {
			return enumeration->metric.type.partition;
		}
	}

	return 0;
}


/**
 * Initializes a given Metric attribute from stream content.
 *
 * \param metric the Metric.
 * \param attr_id the Metric's attribute ID.
 * \param stream the value of Metric's attribute.
 * \param data_entry output parameter to describe data value.
 *
 * \return \b 1, if the Metric's attribute is properly modified; \b 0 otherwise.
 *
 */
int dimutil_fill_metric_attr(struct Metric *metric, OID_Type attr_id,
			     ByteStreamReader *stream, DataEntry *data_entry)
{

	int result = 1;
	int error = 0;

	switch (attr_id) {
	case MDC_ATTR_ID_HANDLE:
		metric->handle = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_handle(data_entry, "Handle", &metric->handle);
		break;
	case MDC_ATTR_ID_TYPE:
		del_type(&(metric->type));
		decode_type(stream, &(metric->type), &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_type(data_entry, "Type", &metric->type);
		break;
	case MDC_ATTR_SUPPLEMENTAL_TYPES:
		del_supplementaltypelist(&metric->supplemental_types);
		decode_supplementaltypelist(stream,
					    &(metric->supplemental_types),
						&error);
		if (error) {
			result = 0;
			break;
		}
		break;
	case MDC_ATTR_METRIC_SPEC_SMALL:
		del_metricspecsmall(&metric->metric_spec_small);
		decode_metricspecsmall(stream,
				       &(metric->metric_spec_small),
					&error);
		if (error) {
			result = 0;
			break;
		}
		data_set_attribute_metric_spec_small(data_entry, "Metric-Spec-Small",
					&metric->metric_spec_small);
		break;
	case MDC_ATTR_METRIC_STRUCT_SMALL:
		del_metricstructuresmall(&(metric->metric_structure_small));
		decode_metricstructuresmall(stream,
					    &(metric->metric_structure_small),
						&error);
		if (error) {
			result = 0;
			break;
		}
		data_set_attribute_metric_structure_small(data_entry, "Metric-Structure-Small",
					&metric->metric_structure_small);
		break;
	case MDC_ATTR_MSMT_STAT:
		metric->measurement_status = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu16(data_entry, "Measurement-Status", &metric->measurement_status);
		break;
	case MDC_ATTR_ID_PHYSIO:
		metric->metric_id = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		metric->use_metric_id_field = 1; // TRUE
		data_set_oid_type(data_entry, "Metric-Id", &metric->metric_id);
		break;
	case MDC_ATTR_ID_PHYSIO_LIST:
		del_metricidlist(&metric->metric_id_list);
		decode_metricidlist(stream, &metric->metric_id_list, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_metric_id_list(data_entry, "Metric-Id-List", &metric->metric_id_list);
		break;
	case MDC_ATTR_METRIC_ID_PART:
		metric->metric_id_partition = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		metric->use_metric_id_partition_field = 1; // TRUE
		data_set_intu16(data_entry, "Metric-Id-Partition", &metric->metric_id_partition);
		break;
	case MDC_ATTR_UNIT_CODE:
		metric->unit_code = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_oid_type(data_entry, "Unit-Code", &metric->unit_code);
		break;
	case MDC_ATTR_ATTRIBUTE_VAL_MAP:
		del_attrvalmap(&metric->attribute_value_map);
		decode_attrvalmap(stream, &(metric->attribute_value_map), &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_attribute_value_map(data_entry, "Attribute-Value-Map",
						&metric->attribute_value_map);
		break;
	case MDC_ATTR_SOURCE_HANDLE_REF:
		metric->source_handle_reference = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_handle(data_entry, "Source-Handle-Reference", &metric->source_handle_reference);
		break;
	case MDC_ATTR_ID_LABEL_STRING:
		del_octet_string(&metric->label_string);
		decode_octet_string(stream, &(metric->label_string), &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_label_string(data_entry, "Label-String", &metric->label_string);
		break;
	case MDC_ATTR_UNIT_LABEL_STRING:
		del_octet_string(&metric->unit_label_string);
		decode_octet_string(stream, &(metric->unit_label_string), &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_label_string(data_entry, "Unit-LabelString", &metric->unit_label_string);
		break;
	case MDC_ATTR_TIME_STAMP_ABS:
		del_absolutetime(&metric->absolute_time_stamp);
		decode_absolutetime(stream, &(metric->absolute_time_stamp), &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_absolute_time(data_entry, "Absolute-Time-Stamp",
				       &metric->absolute_time_stamp);
		break;
	case MDC_ATTR_TIME_STAMP_REL:
		metric->relative_time_stamp = read_intu32(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu32(data_entry, "Relative-Time-Stamp", &metric->relative_time_stamp);
		break;
	case MDC_ATTR_TIME_STAMP_REL_HI_RES:
		del_highresrelativetime(&metric->hi_res_time_stamp);
		decode_highresrelativetime(stream,
					   &(metric->hi_res_time_stamp),
						&error);
		if (error) {
			result = 0;
			break;
		}
		data_set_high_res_relative_time(data_entry, "HiRes-Time-Stamp", &metric->hi_res_time_stamp);
		break;
	case MDC_ATTR_TIME_PD_MSMT_ACTIVE:
		metric->measure_active_period = read_float(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_float(data_entry, "Measure-Active-Period", &metric->measure_active_period);
		break;
	default:
		ERROR("Unrecognized metric attribute-id %d", attr_id);
		break;
	}

	return result;
}

/**
 * Initializes a given Numeric attribute from stream content.
 *
 * \param numeric the Numeric.
 * \param attr_id the Numeric's attribute ID.
 * \param stream the value of Numeric's attribute.
 * \param data_entry output parameter to describe data value
 *
 * \return \b 1, if the Numeric's attribute is properly modified; \b 0 otherwise.
 *
 */
int dimutil_fill_numeric_attr(struct Numeric *numeric, OID_Type attr_id,
			      ByteStreamReader *stream, DataEntry *data_entry)
{

	int result = 1;
	int error = 0;

	switch (attr_id) {
	case MDC_ATTR_NU_VAL_OBS_SIMP:
		del_simplenuobsvalue(&numeric->simple_nu_observed_value);
		decode_simplenuobsvalue(stream,
					&(numeric->simple_nu_observed_value),
					&error);
		if (error) {
			result = 0;
			break;
		}

		data_set_simple_nu_obs_value(data_entry,
					     "Simple-Nu-Observed-Value",
					     &(numeric->simple_nu_observed_value));

		if (data_entry) {
			data_set_meta_att(data_entry, data_strcp("partition"),
					  intu16_2str(dimutil_get_metric_partition(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("metric-id"),
					  intu16_2str(dimutil_get_metric_ids(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("unit-code"),
					  intu16_2str(dimutil_get_unit_code(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("unit"),
					  dimutil_get_unit(&(numeric->metric)));
		}

		break;
	case MDC_ATTR_NU_CMPD_VAL_OBS_SIMP:
		del_simplenuobsvaluecmp(
			&numeric->compound_simple_nu_observed_value);
		decode_simplenuobsvaluecmp(stream,
					   &(numeric->compound_simple_nu_observed_value),
						&error);
		if (error) {
			result = 0;
			break;
		}

		data_set_simple_nu_obs_val_cmp(data_entry,
					       "Compound-Simple-Nu-Observed-Value",
					       &(numeric->compound_simple_nu_observed_value),
					       dimutil_get_metric_partition(&(numeric->metric)),
					       numeric->metric.metric_id_list.value);

		if (data_entry) {
			data_set_meta_att(data_entry, data_strcp("partition"),
					  intu16_2str(dimutil_get_metric_partition(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("metric-id"),
					  intu16_2str(dimutil_get_metric_ids(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("unit-code"),
					  intu16_2str(dimutil_get_unit_code(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("unit"),
					  dimutil_get_unit(&(numeric->metric)));
		}

		break;
	case MDC_ATTR_NU_VAL_OBS_BASIC:
		numeric->basic_nu_observed_value = read_sfloat(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_basic_nu_obs_val(data_entry,
					  "Basic-Nu-Observed-Value",
					  &(numeric->basic_nu_observed_value));

		data_set_meta_att(data_entry, data_strcp("partition"),
				  intu16_2str(dimutil_get_metric_partition(&(numeric->metric))));

		data_set_meta_att(data_entry, data_strcp("metric-id"),
				  intu16_2str(dimutil_get_metric_ids(&(numeric->metric))));

		data_set_meta_att(data_entry, data_strcp("unit-code"),
				  intu16_2str(dimutil_get_unit_code(&(numeric->metric))));

		data_set_meta_att(data_entry, data_strcp("unit"),
				  dimutil_get_unit(&(numeric->metric)));
		break;
	case MDC_ATTR_NU_CMPD_VAL_OBS_BASIC:
		del_basicnuobsvaluecmp(
			&numeric->compound_basic_nu_observed_value);
		decode_basicnuobsvaluecmp(stream,
					  &numeric->compound_basic_nu_observed_value,
					&error);
		if (error) {
			result = 0;
			break;
		}
		data_set_basic_nu_obs_val_cmp(data_entry,
					      "Compound-Basic-Nu-Observed-Value",
					      &(numeric->compound_basic_nu_observed_value),
					      dimutil_get_metric_partition(&(numeric->metric)),
					      numeric->metric.metric_id_list.value);

		if (data_entry) {
			data_set_meta_att(data_entry, data_strcp("partition"),
					  intu16_2str(dimutil_get_metric_partition(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("metric-id"),
					  intu16_2str(dimutil_get_metric_ids(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("unit-code"),
					  intu16_2str(dimutil_get_unit_code(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("unit"),
					  dimutil_get_unit(&(numeric->metric)));
		}

		break;
	case MDC_ATTR_NU_VAL_OBS:
		del_nuobsvalue(&numeric->nu_observed_value);
		decode_nuobsvalue(stream, &numeric->nu_observed_value, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_nu_obs_val(data_entry, "Nu-Observed-Value",
				    &numeric->nu_observed_value);

		if (data_entry) {
			data_set_meta_att(data_entry, data_strcp("partition"),
					  intu16_2str(dimutil_get_metric_partition(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("metric-id"),
					  intu16_2str(numeric->nu_observed_value.metric_id));

			data_set_meta_att(data_entry, data_strcp("unit-code"),
					  intu16_2str(dimutil_get_unit_code(&(numeric->metric))));

			data_set_meta_att(data_entry, data_strcp("unit"),
					  dimutil_get_unit(&(numeric->metric)));
		}

		break;
	case MDC_ATTR_NU_CMPD_VAL_OBS:
		del_nuobsvaluecmp(&numeric->compound_nu_observed_value);
		decode_nuobsvaluecmp(stream,
				     &numeric->compound_nu_observed_value,
					&error);
		if (error) {
			result = 0;
			break;
		}

		data_set_nu_obs_val_cmp(data_entry,
					"Compound-Nu-Observed-Value",
					&numeric->compound_nu_observed_value,
					dimutil_get_metric_partition(&(numeric->metric)));

		break;
	case MDC_ATTR_NU_ACCUR_MSMT:
		numeric->accuracy = read_float(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_float(data_entry, "Accuracy", &numeric->accuracy);
		break;
	default:
		result = dimutil_fill_metric_attr(&numeric->metric, attr_id,
						  stream, data_entry);
		break;
	}

	return result;
}


/**
 * Initializes a given METRIC_RTSA attribute from stream content.
 *
 * \param rtsa the METRIC_RTSA.
 * \param attr_id the METRIC_RTSA's attribute ID.
 * \param stream the value of METRIC_RTSA's attribute.
 * \param data_entry output parameter to describe data value
 *
 * \return \b 1, if the METRIC_RTSA's attribute is properly modified; \b 0 otherwise.
 *
 */
int dimutil_fill_rtsa_attr(struct RTSA *rtsa, OID_Type attr_id,
			   ByteStreamReader *stream, DataEntry *data_entry)
{

	int result = 1;
	int error = 0;

	switch (attr_id) {
	case MDC_ATTR_TIME_PD_SAMP:
		rtsa->sample_period = read_intu32(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_sample_period(data_entry,
				       "Sample-Period",
				       rtsa->sample_period);
		break;
	case MDC_ATTR_SIMP_SA_OBS_VAL:
		del_octet_string(&rtsa->simple_sa_observed_value);
		decode_octet_string(stream, &(rtsa->simple_sa_observed_value), &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_simple_sa_observed_value(data_entry,
						  "Simple-Sa-Observed-Value",
						  &(rtsa->simple_sa_observed_value));
		break;
	case MDC_ATTR_SCALE_SPECN_I8:
		del_scalerangespec8(&rtsa->scale_and_range_specification_8);
		decode_scalerangespec8(stream,
				       &(rtsa->scale_and_range_specification_8),
					&error);
		if (error) {
			result = 0;
			break;
		}
		data_set_scale_and_range_specification_8(data_entry,
				"Scale-and-Range-Specification",
				&(rtsa->scale_and_range_specification_8));
		break;
	case MDC_ATTR_SCALE_SPECN_I16:
		del_scalerangespec16(&rtsa->scale_and_range_specification_16);
		decode_scalerangespec16(stream,
					&(rtsa->scale_and_range_specification_16),
					&error);
		if (error) {
			result = 0;
			break;
		}
		data_set_scale_and_range_specification_16(data_entry,
				"Scale-and-Range-Specification",
				&(rtsa->scale_and_range_specification_16));
		break;
	case MDC_ATTR_SCALE_SPECN_I32:
		del_scalerangespec32(&rtsa->scale_and_range_specification_32);
		decode_scalerangespec32(stream,
					&(rtsa->scale_and_range_specification_32),
					&error);
		if (error) {
			result = 0;
			break;
		}
		data_set_scale_and_range_specification_32(data_entry,
				"Scale-and-Range-Specification",
				&(rtsa->scale_and_range_specification_32));
		break;
	case MDC_ATTR_SA_SPECN:
		del_saspec(&rtsa->sa_specification);
		decode_saspec(stream, &rtsa->sa_specification, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_sa_specification(data_entry,
					  "Sa-Specification",
					  &(rtsa->sa_specification));
		break;
	default:
		result = dimutil_fill_metric_attr(&rtsa->metric, attr_id, stream, data_entry);
		break;
	}

	return result;
}

/**
 * Fill a DataEntry's partition and ids information from  the given Enumeration
 * @param data_entry DataEntry to be filled. If NULL nothing is done, and no
 * additional memory is allocated.
 * @param enumeration Data source.
 */
static void dimutil_fill_data_entry_partition_ids(DataEntry *data_entry,
		struct Enumeration *enumeration)
{
	if (data_entry) {
		int partition;
		int ids;

		partition = dimutil_get_enumeration_partition(enumeration);
		data_set_meta_att(data_entry, data_strcp("partition"),
				  intu16_2str(partition));

		ids = dimutil_get_metric_ids(&(enumeration->metric));
		data_set_meta_att(data_entry, data_strcp("metric-id"),
				  intu16_2str(ids));
	}
}

/**
 * Initializes a given Enumeration attribute from stream content.
 *
 * \param enumeration the Enumeration.
 * \param attr_id the Enumeration attribute ID.
 * \param stream the value of Enumeration's attribute.
 * \param data_entry output parameter to describe data value
 *
 * \return \b 1, if the Enumeration's attribute is properly modified; \b 0 otherwise.
 *
 */
int dimutil_fill_enumeration_attr(struct Enumeration *enumeration,
				  OID_Type attr_id, ByteStreamReader *stream, DataEntry *data_entry)
{

	int result = 1;
	int error = 0;

	switch (attr_id) {
	case MDC_ATTR_ENUM_OBS_VAL_SIMP_OID:
		enumeration->enum_observed_value_simple_OID = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_observed_value_simple_OID(data_entry,
						   "Enum-Observed-Value-Simple-OID",
						   enumeration->enum_observed_value_simple_OID);

		dimutil_fill_data_entry_partition_ids(data_entry, enumeration);

		DEBUG("ENUM ATTR: [type:%s, value:%d]" ,
		      "Enum-Observed-Value-Simple-OID",
		      enumeration->enum_observed_value_simple_OID);

		DEBUG("ENUM ATTR: [type:%s, value:%d]", "Enum-Observed-Value-Simple-OID", enumeration->enum_observed_value_simple_OID);
		break;
	case MDC_ATTR_ENUM_OBS_VAL_SIMP_BIT_STR:
		enumeration->enum_observed_value_simple_bit_str = read_intu32(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_observed_value_simple_bit_str(data_entry,
						       "Enum-Observed-Value-Simple-Bit-Str",
						       enumeration->enum_observed_value_simple_bit_str);

		dimutil_fill_data_entry_partition_ids(data_entry, enumeration);

		DEBUG("ENUM ATTR: [type:%s, value:%d]", "Enum-Observed-Value-Simple-Bit-Str", enumeration->enum_observed_value_simple_bit_str);
		break;
	case MDC_ATTR_ENUM_OBS_VAL_BASIC_BIT_STR:
		enumeration->enum_observed_value_basic_bit_str = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_observed_value_basic_bit_str(data_entry,
						      "Enum-Observed-Value-Basic-Bit-Str",
						      enumeration->enum_observed_value_basic_bit_str);

		dimutil_fill_data_entry_partition_ids(data_entry, enumeration);

		DEBUG("ENUM ATTR: [type:%s, value:%d]", "Enum-Observed-Value-Basic-Bit-Str", enumeration->enum_observed_value_basic_bit_str);
		break;
	case MDC_ATTR_ENUM_OBS_VAL_SIMP_STR:
		del_octet_string(&enumeration->enum_observed_value_simple_str);
		decode_octet_string(stream,
				    &(enumeration->enum_observed_value_simple_str),
					&error);
		if (error) {
			result = 0;
			break;
		}
		data_set_observed_value_simple_str(data_entry,
						   "Enum-Observed-Value-Simple-Str",
						   &(enumeration->enum_observed_value_simple_str));

		dimutil_fill_data_entry_partition_ids(data_entry, enumeration);

		DEBUG("ENUM ATTR: [type:%s, value:octect_string]", "Enum-Observed-Value-Simple-Str");
		break;
	case MDC_ATTR_VAL_ENUM_OBS:
		del_enumobsvalue(&enumeration->enum_observed_value);
		decode_enumobsvalue(stream, &(enumeration->enum_observed_value),
					&error);
		if (error) {
			result = 0;
			break;
		}
		data_set_enum_observed_value(data_entry,
					     "Enum-Observed-Value",
					     &(enumeration->enum_observed_value));

		dimutil_fill_data_entry_partition_ids(data_entry, enumeration);

		DEBUG("ENUM ATTR: [type:%s, value:Enum-Observed-Value]", "Enum-Observed-Value");
		break;
	case MDC_ATTR_ENUM_OBS_VAL_PART:

		enumeration->enum_observed_value_partition = read_intu16(
					stream, NULL);
		enumeration->use_nom_partition = 1;

		data_set_value_partition(data_entry,
					 "Enum-Observed-Value-Partition",
					 enumeration->enum_observed_value_partition);

		DEBUG("ENUM ATTR: [type:%s, value:%d]", "Enum-Observed-Value-Partition", enumeration->enum_observed_value_partition);
		break;
	default:
		result = dimutil_fill_metric_attr(&enumeration->metric, attr_id, stream, data_entry);
		break;

	}

	return result;
}

/**
 * Initializes a given PMStore attribute from stream content.
 *
 * \param pmstore the PMStore.
 * \param attr_id the PMStore's attribute ID.
 * \param stream the value of PMStore's attribute.
 * \param data_entry output parameter to describe data value
 *
 * \return \b 1, if the PMStore's attribute is properly modified; \b 0 otherwise.
 *
 */
int dimutil_fill_pmstore_attr(struct PMStore *pmstore, OID_Type attr_id,
			      ByteStreamReader *stream,
				DataEntry *data_entry)
{
	int result = 1;
	int error = 0;

	switch (attr_id) {
	case MDC_ATTR_ID_HANDLE:
		pmstore->handle = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_handle(data_entry, "Handle", &pmstore->handle);
		break;
	case MDC_ATTR_PM_STORE_CAPAB:
		pmstore->pm_store_capab = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu16(data_entry, "Capabilities", &pmstore->pm_store_capab);
		break;
	case MDC_ATTR_METRIC_STORE_SAMPLE_ALG:
		pmstore->store_sample_algorithm = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu16(data_entry, "Store-Sample-Algorithm",
				&pmstore->store_sample_algorithm);
		break;
	case MDC_ATTR_METRIC_STORE_CAPAC_CNT:
		pmstore->store_capacity_count = read_intu32(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu32(data_entry, "Store-Capacity-Count",
				&pmstore->store_capacity_count);
		break;
	case MDC_ATTR_METRIC_STORE_USAGE_CNT:
		pmstore->store_usage_count = read_intu32(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu32(data_entry, "Store-Usage-Count",
				&pmstore->store_usage_count);
		break;
	case MDC_ATTR_OP_STAT:
		pmstore->operational_state = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu16(data_entry, "Operational-State",
				&pmstore->operational_state);
		break;
	case MDC_ATTR_PM_STORE_LABEL_STRING:
		del_octet_string(&pmstore->pm_store_label);
		decode_octet_string(stream, &pmstore->pm_store_label, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_label_string(data_entry, "PM-Store-Label",
				&pmstore->pm_store_label);
		break;
	case MDC_ATTR_TIME_PD_SAMP:
		pmstore->sample_period = read_intu32(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu32(data_entry, "Sample-Period",
				&pmstore->sample_period);
		break;
	case MDC_ATTR_NUM_SEG:
		pmstore->number_of_segments = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu16(data_entry, "Number-Of-Segments",
				&pmstore->number_of_segments);
		break;
	case MDC_ATTR_CLEAR_TIMEOUT:
		pmstore->clear_timeout = read_intu32(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu32(data_entry, "Clear-Timeout",
				&pmstore->clear_timeout);
		break;
	default:
		ERROR("Can't fill pmstore attribute attribute id is %d", attr_id);
		break;
	}

	return result;
}

/**
 * Initializes a given Scanner attribute from stream content.
 *
 * \param scanner the Scanner.
 * \param attr_id the Scanner's attribute ID.
 * \param stream the value of Scanner's attribute.
 * \param data_entry output parameter to describe data value
 *
 * \return \b 1, if the Scanner's attribute is properly modified; \b 0 otherwise.
 */
static int dimutil_fill_scanner_attr(struct Scanner *scanner,
				     OID_Type attr_id, ByteStreamReader *stream, DataEntry *data_entry)
{
	int result = 1;
	int error = 0;

	switch (attr_id) {
	case MDC_ATTR_ID_HANDLE:
		scanner->handle = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_handle(data_entry, "Handle", &scanner->handle);
		break;
	case MDC_ATTR_OP_STAT:
		scanner->operational_state = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu16(data_entry, "Operational-State", &scanner->operational_state);
		break;
	case MDC_ATTR_SCAN_HANDLE_LIST:
		del_handlelist(&scanner->scan_handle_list);
		decode_handlelist(stream, &scanner->scan_handle_list, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_handle_list(data_entry, "Scan-Handle-List", &scanner->scan_handle_list);
		break;
	case MDC_ATTR_SCAN_HANDLE_ATTR_VAL_MAP:
		del_handleattrvalmap(&scanner->scan_handle_attr_val_map);
		decode_handleattrvalmap(stream, &scanner->scan_handle_attr_val_map,
					&error);
		if (error) {
			result = 0;
			break;
		}
		data_set_handle_attr_val_map(data_entry, "Scan-Handle-Attr-Val-Map", &scanner->scan_handle_attr_val_map);
		break;
	default:
		ERROR("Can't fill scanner attribute attribute id is %d", attr_id);
		break;
	}

	return result;
}

/**
 * Initializes a given CfgScanner attribute from stream content.
 *
 * \param cfg_scanner the CfgScanner.
 * \param attr_id the CfgScanner's attribute ID.
 * \param stream the value of CfgScanner's attribute.
 * \param data_entry output parameter to describe data value
 *
 * \return \b 1, if the CfgScanner's attribute is properly modified; \b 0 otherwise.
 *
 */
static int dimutil_fill_cfg_scanner_attr(struct CfgScanner *cfg_scanner,
		OID_Type attr_id, ByteStreamReader *stream, DataEntry *data_entry)
{

	int result = 1;
	int error = 0;

	switch (attr_id) {
	case MDC_ATTR_CONFIRM_MODE:
		cfg_scanner->confirm_mode = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu16(data_entry, "Confirm-Mode", &cfg_scanner->confirm_mode);
		break;
	case MDC_ATTR_CONFIRM_TIMEOUT:
		cfg_scanner->confirm_timeout = read_intu32(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu32(data_entry, "Confirm-Timeout", &cfg_scanner->confirm_timeout);
		break;
	case MDC_ATTR_TX_WIND:
		cfg_scanner->transmit_window = read_intu16(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu16(data_entry, "Transmit-Window", &cfg_scanner->transmit_window);
		break;
	default:
		DEBUG("Can't fill cfg_scanner attribute attribute id is %d", attr_id);
		result = dimutil_fill_scanner_attr(&cfg_scanner->scanner, attr_id, stream, data_entry);
		break;
	}

	return result;
}

/**
 * Initializes a given PeriCfgScanner attribute from stream content.
 *
 * \param peri_scanner the PeriCfgScanner.
 * \param attr_id the PeriCfgScanner's attribute ID.
 * \param stream the value of PeriCfgScanner's attribute.
 * \param data_entry output parameter to describe data value
 *
 * \return \b 1, if the PeriCfgScanner's attribute is properly modified; \b 0 otherwise.
 *
 */
int dimutil_fill_peri_scanner_attr(struct PeriCfgScanner *peri_scanner,
				   OID_Type attr_id, ByteStreamReader *stream, DataEntry *data_entry)
{
	int result = 1;
	int error = 0;

	switch (attr_id) {
	case MDC_ATTR_SCAN_REP_PD:
		peri_scanner->reporting_interval = read_intu32(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu32(data_entry, "Reporting-Interval", &peri_scanner->reporting_interval);
		break;
	default:
		DEBUG("Can't fill peri_scanner attribute attribute id is %d", attr_id);
		result = dimutil_fill_cfg_scanner_attr(&peri_scanner->scanner, attr_id, stream, data_entry);
		break;
	}

	return result;
}

/**
 * Initializes a given EpiCfgScanner attribute from stream content.
 *
 * \param epi_scanner the EpiCfgScanner.
 * \param attr_id the EpiCfgScanner's attribute ID.
 * \param stream the value of EpiCfgScanner's attribute.
 * \param data_entry output parameter to describe data value
 *
 * \return \b 1, if the EpiCfgScanner's attribute is properly modified; \b 0 otherwise.
 *
 */
int dimutil_fill_epi_scanner_attr(struct EpiCfgScanner *epi_scanner,
				  OID_Type attr_id, ByteStreamReader *stream, DataEntry *data_entry)
{
	int result = 1;
	int error = 0;

	switch (attr_id) {
	case MDC_ATTR_SCAN_REP_PD_MIN:
		epi_scanner->min_reporting_interval = read_intu32(stream, &error);
		if (error) {
			result = 0;
			break;
		}
		data_set_intu32(data_entry, "Min-Reporting-Interval", &epi_scanner->min_reporting_interval);
		break;
	default:
		DEBUG("Can't fill epi_scanner attribute, attribute id is %d", attr_id);
		result = dimutil_fill_cfg_scanner_attr(&epi_scanner->scanner, attr_id, stream, data_entry);
		break;
	}

	return result;
}

/**
 * Initializes a given Metric/Numeric/Enumeration/RT-SA object from a list
 * of attributes.
 *
 * \param mds
 * \param data_entry output parameter to describe data value
 * \param metric_obj the Metric_object.
 * \param attr_list list of the attributes to initialize the Metric_object.
 */
static void dimutil_fill_metric_object(struct MDS *mds, DataEntry *data_entry,
				       struct Metric_object *metric_obj, AttributeList *attr_list)
{

	data_entry->choice = COMPOUND_DATA_ENTRY;
	CompoundDataEntry *cmp_entry =  &data_entry->u.compound;
	cmp_entry->entries_count =  attr_list->count;
	cmp_entry->entries = calloc(attr_list->count, sizeof(DataEntry));

	int j;
	octet_string val;
	OID_Type attr_id;

	switch (metric_obj->choice) {
	case METRIC_NUMERIC:
		cmp_entry->name = data_strcp("Numeric");

		for (j = 0; j < attr_list->count; ++j) {
			attr_id = attr_list->value[j].attribute_id;
			val.length = attr_list->value[j].attribute_value.length;
			val.value = attr_list->value[j].attribute_value.value;

			ByteStreamReader *stream = byte_stream_reader_instance(val.value,
						   val.length);

			int result = dimutil_fill_numeric_attr(&(metric_obj->u.numeric), attr_id, stream, &(cmp_entry->entries[j]));

			if (result == 0) {
				ERROR("ERROR filling numeric attribute");
			}

			free(stream);
		}

		break;
	case METRIC_ENUM:
		cmp_entry->name = data_strcp("Enumeration");

		for (j = 0; j < attr_list->count; ++j) {
			attr_id = attr_list->value[j].attribute_id;
			val.length = attr_list->value[j].attribute_value.length;
			val.value = attr_list->value[j].attribute_value.value;

			ByteStreamReader *stream = byte_stream_reader_instance(val.value,
						   val.length);

			int result = dimutil_fill_enumeration_attr(&(metric_obj->u.enumeration), attr_id,
					stream, &(cmp_entry->entries[j]));

			if (result == 0) {
				ERROR("ERROR filling enumeration attr");
			}

			free(stream);
		}

		break;
	case METRIC_RTSA:
		cmp_entry->name = data_strcp("RT-SA");

		for (j = 0; j < attr_list->count; ++j) {
			attr_id = attr_list->value[j].attribute_id;
			val.length = attr_list->value[j].attribute_value.length;
			val.value = attr_list->value[j].attribute_value.value;

			ByteStreamReader *stream = byte_stream_reader_instance(val.value,
						   val.length);

			int result = dimutil_fill_rtsa_attr(&(metric_obj->u.rtsa), attr_id,
							    stream, &(cmp_entry->entries[j]));

			if (result == 0) {
				ERROR("ERROR filling stsa attr");
			}

			free(stream);
		}

		break;
	default:
		break;
	}

}


/**
 * Update MDS objects with data reported in the var-format.
 *
 * \param mds
 * \param var_obs The measured data that were reported in the var-format.
 * \param data_entry output parameter to describe data value.
 */
void dimutil_update_mds_from_obs_scan(struct MDS *mds, ObservationScan *var_obs,
				      DataEntry *data_entry)
{

	HANDLE obj_handle = 0;
	struct MDS_object *object = NULL;
	struct Metric_object *metric_obj = NULL;
	struct PMStore *pmstore = NULL;

	obj_handle = var_obs->obj_handle;

	// TODO: refactor
	object = mds_get_object_by_handle(mds, obj_handle);

	if (object != NULL) {
		if (object->choice == MDS_OBJ_METRIC)
			metric_obj = &(object->u.metric);
		else if (object->choice == MDS_OBJ_PMSTORE)
			pmstore = &(object->u.pmstore);
	}

	AttributeList attr_list = var_obs->attributes;
	int attr_list_size = attr_list.count;
	OID_Type attr_id;

	if (metric_obj != NULL) {

		DataEntry *measurement_data = data_entry;
		data_meta_set_handle(measurement_data, obj_handle);

		dimutil_fill_metric_object(mds, measurement_data,
					   metric_obj, &attr_list);
	}

	if (pmstore != NULL) {
		int j;

		for (j = 0; j < attr_list_size; ++j) {
			attr_id = attr_list.value[j].attribute_id;
			intu16 length = attr_list.value[j].attribute_value.length;
			intu8 *value = attr_list.value[j].attribute_value.value;

			// TODO: check if data can come aggregated
			ByteStreamReader *stream = byte_stream_reader_instance(value, length);
			pmstore_set_attribute(pmstore, attr_id, stream);
			free(stream);
		}
	}
}

/**
 * Update MDS objects with data reported in the fixed-format.
 *
 * \param mds
 * \param fixed_obs The measured data that were reported in the fixed-format.
 * \param data_entry output parameter to describe data value.
 */
void dimutil_update_mds_from_obs_scan_fixed(struct MDS *mds, ObservationScanFixed *fixed_obs,
		DataEntry *data_entry)
{

	int result;
	struct Metric_object *metric_obj = NULL;
	struct MDS_object *object = NULL;
	HANDLE handle = fixed_obs->obj_handle;
	object = mds_get_object_by_handle(mds, handle);

	if (object != NULL) {
		if (object->choice == MDS_OBJ_METRIC)
			metric_obj = &(object->u.metric);
	}

	int attr_list_size;

	if (metric_obj != NULL) {
		DataEntry *measurement_entry = data_entry;
		measurement_entry->choice = COMPOUND_DATA_ENTRY;
		data_meta_set_handle(measurement_entry, handle);
		CompoundDataEntry *cmp_entry = &measurement_entry->u.compound;

		octet_string value = fixed_obs->obs_val_data;
		ByteStreamReader *stream = byte_stream_reader_instance(value.value, value.length);

		switch (metric_obj->choice) {
		case METRIC_NUMERIC: {
			AttrValMap val_map = metric_obj->u.numeric.metric.attribute_value_map;
			cmp_entry->name = data_strcp("Numeric");
			attr_list_size = metric_obj->u.numeric.metric.attribute_value_map.count;
			cmp_entry->entries_count = attr_list_size;
			cmp_entry->entries = calloc(attr_list_size, sizeof(DataEntry));
			int j;

			for (j = 0; j < attr_list_size; ++j) {
				result = dimutil_fill_numeric_attr(&(metric_obj->u.numeric),
								   val_map.value[j].attribute_id,
								   stream, &(cmp_entry->entries[j]));

				if (result == 0) {
					ERROR("ERROR filling numeric attr");
				}
			}
		}
		break;
		case METRIC_ENUM: {
			AttrValMap val_map = metric_obj->u.enumeration.metric.attribute_value_map;
			cmp_entry->name = data_strcp("Enumeration");

			attr_list_size = metric_obj->u.enumeration.metric.attribute_value_map.count;
			cmp_entry->entries_count = attr_list_size;
			cmp_entry->entries = calloc(attr_list_size, sizeof(DataEntry));
			int j;

			for (j = 0; j < attr_list_size; ++j) {
				result = dimutil_fill_enumeration_attr(&(metric_obj->u.enumeration),
								       val_map.value[j].attribute_id,
								       stream,
								       &(cmp_entry->entries[j]));

				if (result == 0) {
					ERROR("ERROR filling enumeration attr");
				}
			}
		}
		break;
		case METRIC_RTSA: {
			AttrValMap val_map = metric_obj->u.rtsa.metric.attribute_value_map;
			cmp_entry->name = data_strcp("RT-SA");

			attr_list_size = metric_obj->u.rtsa.metric.attribute_value_map.count;
			cmp_entry->entries_count = attr_list_size;
			cmp_entry->entries = calloc(attr_list_size, sizeof(DataEntry));
			int j;

			for (j = 0; j < attr_list_size; ++j) {
				result = dimutil_fill_rtsa_attr(&(metric_obj->u.rtsa),
								val_map.value[j].attribute_id,
								stream,  &(cmp_entry->entries[j]));

				if (result == 0) {
					ERROR("ERROR filling rtsa attr");
				}
			}
		}
		break;
		}

		free(stream);
	}
}

/**
 * Update MDS objects with data reported in the grouped-format.
 *
 * \param mds
 * \param stream The measured data that were reported in the grouped-format.
 *
 * \param val_map_entry This parameter defines the metric-derived objects, the
 * attributes, and the order in which objects and attribute values are reported
 * in a Unbuf-Scan-Report-Grouped, Buf-Scan-Report-Grouped, Unbuf-Scan-Report-MP-Grouped,
 * Buf-Scan-Report-MP-Grouped.
 *
 * \param measurement_entry output parameter to describe data value.
 */
void dimutil_update_mds_from_grouped_observations(struct MDS *mds, ByteStreamReader *stream,
		HandleAttrValMapEntry *val_map_entry,
		DataEntry *measurement_entry)
{

	struct MDS_object *obj = mds_get_object_by_handle(mds, val_map_entry->obj_handle);
	AttrValMap *val_map = &val_map_entry->attr_val_map;

	measurement_entry->choice = COMPOUND_DATA_ENTRY;
	data_meta_set_handle(measurement_entry, val_map_entry->obj_handle);

	CompoundDataEntry *cmp_entry = &measurement_entry->u.compound;
	cmp_entry->entries_count = val_map->count;
	cmp_entry->entries = calloc(val_map->count, sizeof(DataEntry));

	if (val_map->count > 0) {

		if (obj->u.metric.choice == METRIC_NUMERIC) {
			cmp_entry->name = data_strcp("Numeric");
		} else if (obj->u.metric.choice == METRIC_ENUM) {
			cmp_entry->name = data_strcp("Enumeration");
		} else {
			cmp_entry->name = data_strcp("RT-SA");
		}
	}

	int k;

	for (k = 0; k < val_map->count; k++) {
		switch (obj->u.metric.choice) {
		case METRIC_NUMERIC: {
			int result = dimutil_fill_numeric_attr(&(obj->u.metric.u.numeric),
							       val_map->value[k].attribute_id,
							       stream, &cmp_entry->entries[k]);

			if (!result) {
				// TODO: Make a better error handling
				ERROR("Attribute id not found");
			}
		}
		break;
		case METRIC_ENUM: {
			int result = dimutil_fill_enumeration_attr(&(obj->u.metric.u.enumeration),
					val_map->value[k].attribute_id,
					stream, &cmp_entry->entries[k]);

			if (!result) {
				// TODO: Make a better error handling
				ERROR("Attribute id not found");
			}
		}
		break;
		case METRIC_RTSA: {
			int result = dimutil_fill_rtsa_attr(
					     &(obj->u.metric.u.rtsa),
					     val_map->value[k].attribute_id,
					     stream, &cmp_entry->entries[k]);

			if (!result) {
				// TODO: Make a better error handling
				ERROR("Attribute id not found");
			}
		}
		break;
		}
	}
}

/** @} */
