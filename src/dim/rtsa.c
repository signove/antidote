/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * \file rtsa.c
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

#include <stdlib.h>
#include "rtsa.h"
#include "nomenclature.h"
#include "src/communication/parser/struct_cleaner.h"

/**
 * \defgroup METRIC_RTSA RealTime-SA
 * \ingroup ObjectClasses
 * \brief RealTime-SA Object Class
 *
 * An instance of the RT-SA class represents a wave form measurement.
 *
 * The values of the RT-SA object are sent from the agent to the manager
 * using the EVENT REPORT service. This class is derived from the metric base class.
 *
 * @{
 */

/**
 * Creates a new instance of METRIC_RTSA object class.
 *
 * \return a pointer to METRIC_RTSA struct.
 */
static struct RTSA *new_instance()
{
	struct RTSA *rtsa = calloc(1, sizeof(struct RTSA));
	rtsa->dim.id = rtsa_get_nomenclature_code();
	return rtsa;
}

/**
 * Creates METRIC_RTSA struct with sample resolution of size 8.
 *
 * \param metric the METRIC base object of new METRIC_RTSA struct.
 * \param sample_period the sample period.
 * \param simple_sa_observed_value the sample value observed.
 * \param scale_and_range_specification_8 the sample resolution.
 * \param sa_specification the sample specification.
 *
 * \return a pointer to METRIC_RTSA struct with sample resolution of size 8.
 */
struct RTSA *rtsa_instance_spec8(struct Metric *metric,
				 RelativeTime sample_period,
				 octet_string simple_sa_observed_value,
				 ScaleRangeSpec8 scale_and_range_specification_8,
				 SaSpec sa_specification)
{
	struct RTSA *rtsa = new_instance();
	rtsa->metric = *metric;
	rtsa->sample_period = sample_period;
	rtsa->simple_sa_observed_value = simple_sa_observed_value;
	rtsa->scale_and_range_specification_8 = scale_and_range_specification_8;
	rtsa->sa_specification = sa_specification;
	return rtsa;
}

/**
 * Creates METRIC_RTSA struct with sample resolution of size 16.
 *
 * \param metric the METRIC base object of new METRIC_RTSA struct.
 * \param sample_period the sample period.
 * \param simple_sa_observed_value the sample value observed.
 * \param scale_and_range_specification_16 the sample resolution.
 * \param sa_specification the sample specification.
 *
 * \return a pointer to METRIC_RTSA struct with sample resolution of size 16.
 */
struct RTSA *rtsa_instance_spec16(struct Metric *metric,
				  RelativeTime sample_period,
				  octet_string simple_sa_observed_value,
				  ScaleRangeSpec16 scale_and_range_specification_16,
				  SaSpec sa_specification)
{
	struct RTSA *rtsa = new_instance();
	rtsa->metric = *metric;
	rtsa->sample_period = sample_period;
	rtsa->simple_sa_observed_value = simple_sa_observed_value;
	rtsa->scale_and_range_specification_16
	= scale_and_range_specification_16;
	rtsa->sa_specification = sa_specification;
	return rtsa;
}

/**
 * Creates METRIC_RTSA struct with sample resolution of size 32.
 *
 * \param metric the METRIC base object of new METRIC_RTSA struct.
 * \param sample_period the sample period.
 * \param simple_sa_observed_value the sample value observed.
 * \param scale_and_range_specification_32 the sample resolution.
 * \param sa_specification the sample specification.
 *
 * \return a pointer to METRIC_RTSA struct with sample resolution of size 32.
 */
struct RTSA *rtsa_instance_spec32(struct Metric *metric,
				  RelativeTime sample_period,
				  octet_string simple_sa_observed_value,
				  ScaleRangeSpec32 scale_and_range_specification_32,
				  SaSpec sa_specification)
{
	struct RTSA *rtsa = new_instance();
	rtsa->metric = *metric;
	rtsa->sample_period = sample_period;
	rtsa->simple_sa_observed_value = simple_sa_observed_value;
	rtsa->scale_and_range_specification_32
	= scale_and_range_specification_32;
	rtsa->sa_specification = sa_specification;
	return rtsa;
}

/**
 * Returns the class identification.
 *
 * \return code of class identification.
 */
int rtsa_get_nomenclature_code()
{
	return MDC_MOC_VMO_METRIC_SA_RT;
}

/**
 * Deallocates a pointer to a METRIC_RTSA struct.
 *
 * \param rtsa the METRIC_RTSA pointer to be deallocated.
 */
void rtsa_destroy(struct RTSA *rtsa)
{
	if (rtsa != NULL) {
		del_octet_string(&(rtsa->simple_sa_observed_value));
		del_scalerangespec8(&(rtsa->scale_and_range_specification_8));
		del_scalerangespec16(&(rtsa->scale_and_range_specification_16));
		del_scalerangespec32(&(rtsa->scale_and_range_specification_32));
		del_saspec(&(rtsa->sa_specification));
		metric_destroy(&(rtsa->metric));
	}
}

/** @} */
