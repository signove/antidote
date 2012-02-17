/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * \file rtsa.h
 * \brief METRIC_RTSA RealTime-SA definition
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
 * \date Jun 10, 2010
 * \author Diego Bezerra
 */

#ifndef RTSA_H_
#define RTSA_H_

#include "metric.h"


/**
 * Definition of RealTime-SA type
 */
struct RTSA {

	/**
	 * The DIM structure
	 */
	struct DIM dim;

	/**
	 * The Metric structure
	 */
	struct Metric metric;

	/**
	 * This attribute defines time interval between
	 * successive samples given in 1/8 of a millisecond
	 *
	 * Qualifier: Mandatory
	 */
	RelativeTime sample_period;

	/**
	 * This byte array contains the samples that are reported
	 * by the agent in the format that is described by the
	 * Sa-Specification and Scale-and-Range-Specification.
	 *
	 * Qualifier: Mandatory
	 */
	octet_string simple_sa_observed_value;

	/**
	 * This attribute defines mapping between samples and actual
	 * values as well as measurement range. The type depend on
	 * sample resolution (sample-size field within sample-type
	 * field of Sa-Specification). Exactly one of the three
	 * specifications (8, 16 or 32) shall be included. This attribute shall
	 * remain unchanged after the configuration is agreed upon
	 *
	 * Qualifier: Mandatory
	 */
	ScaleRangeSpec8 scale_and_range_specification_8;

	/**
	 * This attribute defines mapping between samples and actual
	 * values as well as measurement range. The type depend on
	 * sample resolution (sample-size field within sample-type
	 * field of Sa-Specification). Exactly one of the three
	 * specifications (8, 16 or 32) shall be included. This attribute shall
	 * remain unchanged after the configuration is agreed upon
	 *
	 * Qualifier: Mandatory
	 */
	ScaleRangeSpec16 scale_and_range_specification_16;

	/**
	 * This attribute defines mapping between samples and actual
	 * values as well as measurement range. The type depend on
	 * sample resolution (sample-size field within sample-type
	 * field of Sa-Specification). Exactly one of the three
	 * specifications (8, 16 or 32) shall be included. This attribute shall
	 * remain unchanged after the configuration is agreed upon
	 *
	 * Qualifier: Mandatory
	 */
	ScaleRangeSpec32 scale_and_range_specification_32;

	/**
	 * This attribute describes the sample array and sample
	 * types. This attribute shall remain unchanged after
	 * the configuration is agreed upon
	 *
	 * Qualifier: Mandatory
	 */
	SaSpec sa_specification;
};

struct RTSA *rtsa_instance_spec8(struct Metric *metric,
				 RelativeTime sample_period,
				 octet_string simple_sa_observed_value,
				 ScaleRangeSpec8 scale_and_range_specification_8,
				 SaSpec sa_specification);

struct RTSA *rtsa_instance_spec16(struct Metric *metric,
				  RelativeTime sample_period,
				  octet_string simple_sa_observed_value,
				  ScaleRangeSpec16 scale_and_range_specification_16,
				  SaSpec sa_specification);

struct RTSA *rtsa_instance_spec32(struct Metric *metric,
				  RelativeTime sample_period,
				  octet_string simple_sa_observed_value,
				  ScaleRangeSpec32 scale_and_range_specification_32,
				  SaSpec sa_specification);


int rtsa_get_nomenclature_code();

void rtsa_destroy(struct RTSA *rtsa);

#endif /* RTSA_H_ */
