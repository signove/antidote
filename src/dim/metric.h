/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file metric.h
 * \brief Metric type definitions.
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
 * \author Diego Bezerra
 */

#ifndef METRIC_H_
#define METRIC_H_

#include "asn1/phd_types.h"
#include "dim.h"

/**
 * Metric object structure
 */
struct Metric {
	/**
	 * The DIM structure
	 */
	struct DIM dim;

	/**
	 * The Handle attribute represents a reference ID for this object.
	 * Each object shall have a unique ID assigned by the agent.
	 *
	 * Qualifier: Mandatory
	 */
	HANDLE handle;

	/**
	 * This attribute defines a specific static type of this object
	 * as defined in the nomenclature
	 *
	 * Qualifier: Mandatory
	 */
	TYPE type;

	/**
	 *This attribute describes the characteristics of the measurements
	 *
	 * Qualifier: Mandatory
	 */
	MetricSpecSmall metric_spec_small;

	/**
	 *This attribute may be used to convey supplemental information
	 *about the object beyond the Type and Metric-Id attributes
	 *
	 * Qualifier: Mandatory
	 */
	SupplementalTypeList supplemental_types;

	/**
	 *This attribute describes the structure of the measurement.
	 *If no present, the manager shall assume
	 *MetricStructureSmall := {ms-struct-simple, 0}
	 *
	 * Qualifier: Optional
	 */
	MetricStructureSmall metric_structure_small;

	/**
	 *This attribute indicates the validity of a
	 *particular value or set of sample
	 *
	 * Qualifier: Optional
	 */
	MeasurementStatus measurement_status;

	/**
	 *This attribute may be used to hold an identification
	 *that is more specific than the generic ID in the Type
	 *attribute
	 *
	 * Qualifier: Optional
	 */
	OID_Type metric_id;

	/**
	 *This attribute may be used to define the partition
	 *from which the Metric-Id or Metric-Id-List nomenclature
	 *terms are taken
	 *
	 * Qualifier: Optional
	 */
	NomPartition metric_id_partition;

	/**
	 *This attribute defines nomenclature codes for the units
	 *of measure from the nom-part-dim partition
	 *
	 * Qualifier: Optional
	 */
	OID_Type unit_code;

	/**
	 *This attribute establishes a relation of this object instance
	 *to a source object
	 *
	 * Qualifier: Optional
	 */
	HANDLE source_handle_reference;

	/**
	 *This attribute defines a textual representation of Type
	 *attribute in printable ASCII
	 *
	 * Qualifier: Optional
	 */
	octet_string label_string;

	/**
	 *This attribute defines a textual representation of
	 *Unit-Code dimension in printable ASCII
	 *
	 * Qualifier: Optional
	 *
	 */
	octet_string unit_label_string;

	/**
	 *This attribute defines the time duration of
	 *observation period in seconds
	 *
	 * Qualifier: Optional
	 */
	FLOAT_Type measure_active_period;

	/**
	 *This attribute shall be used if a compound observed value
	 *is used and does not incorporate the Metric-Id directly so
	 *that element in the observed value list can be identified
	 *individually
	 *
	 * Qualifier: Conditional
	 */
	MetricIdList metric_id_list;

	/**
	 *This attribute defines the attributes that are reported in
	 *the fixed format data update messages. Usage of this attribute
	 *is mandatory if the agent uses fixed format value messages to
	 *report dynamic measurement data for the object
	 *
	 * Qualifier: Conditional
	 */
	AttrValMap attribute_value_map;

	/**
	 *This attribute defines the date and time of observation with
	 *resolution of 1/100 of a second, if available
	 *
	 * Qualifier: Conditional
	 */
	AbsoluteTime absolute_time_stamp;

	/**
	 *This attribute defines the time of observation (time stamp in
	 *a relative time format/number of clock ticks as defined by
	 *RelativeTime data type)
	 *
	 * Qualifier: Conditional
	 */
	RelativeTime relative_time_stamp;

	/**
	 *This attribute defines the time of observation (time stamp in
	 *a high-resolution relative time format/number of clock ticks as defined by
	 *HighResRelativeTime data type)
	 *
	 * Qualifier: Conditional
	 */
	HighResRelativeTime hi_res_time_stamp;

	/**
	 * Indicates that the metric_id attribute is being used
	 */
	int use_metric_id_field;

	/**
	 * Indicates that the metric_id_partition attribute is being used
	 */
	int use_metric_id_partition_field;
};

struct Metric *metric_instance();

void metric_destroy(struct Metric *metric);

int metric_get_nomenclature_code();

#endif /* METRIC_H_ */
