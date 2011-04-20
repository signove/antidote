/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file metric.c
 * \brief Metric type implementation
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

#include <stdlib.h>
#include "metric.h"
#include "nomenclature.h"
#include "src/communication/parser/struct_cleaner.h"

/**
 * \defgroup Metric Metric
 * \ingroup ObjectClasses
 * \brief Metric Object Class
 *
 * The metric class is the base class for all objects representing measurements,
 * status, and context data.
 *
 * The metric class is not instantiated; therefore, it is never part of the agent
 * configuration. As a base class, it defines all attributes, methods, events, and
 * services that are common for all objects representing measurements.
 *
 * @{
 */

/**
 * Creates a new instance of Metric object class.
 *
 * \return a pointer to Metric struct.
 */
struct Metric *metric_instance()
{
	struct Metric *metric = calloc(1, sizeof(struct Metric));
	metric->dim.id = metric_get_nomenclature_code();
	metric->metric_id_list.count = 0;
	metric->metric_id_list.value = NULL;
	return metric;
}

/**
 * Deallocates a pointer to a Metric struct.
 *
 * \param metric the Metric pointer to be deallocated.
 */
void metric_destroy(struct Metric *metric)
{
	if (metric != NULL) {
		del_type(&metric->type);
		del_supplementaltypelist(&metric->supplemental_types);
		del_metricspecsmall(&metric->metric_spec_small);
		del_metricstructuresmall(&metric->metric_structure_small);
		del_metricidlist(&metric->metric_id_list);
		del_attrvalmap(&metric->attribute_value_map);
		del_octet_string(&metric->label_string);
		del_octet_string(&metric->unit_label_string);
		del_absolutetime(&metric->absolute_time_stamp);
		del_highresrelativetime(&metric->hi_res_time_stamp);
	}
}

/**
 * Returns the class identification.
 *
 * \return code of class identification.
 */
int metric_get_nomenclature_code()
{
	return MDC_MOC_VMO_METRIC;
}

/** @} */
