/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file numeric.c
 * \brief Numeric type implementation.
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
 * \date Jun 9, 2010
 * \author Diego Bezerra
 */

#include <stdlib.h>
#include "numeric.h"
#include "nomenclature.h"
#include "src/communication/parser/struct_cleaner.h"

/**
 * \defgroup Numeric Numeric
 * \ingroup ObjectClasses
 * \brief Numeric Object Class
 *
 * An instance of the numeric class represents a numerical measurement.
 *
 * The values of a numeric object are sent from the agent to the manager using
 * the EVENT REPORT service. This class is derived from the metric base class.
 *
 * @{
 */

/**
 * Creates a new instance of Numeric object class.
 *
 * \param metric the metric field of the Numeric struct just created.
 *
 * \return a pointer to Numeric struct.
 */
struct Numeric *numeric_instance(struct Metric *metric)
{
	struct Numeric *numeric = calloc(1, sizeof(struct Numeric));
	numeric->dim.id = numeric_get_nomenclature_code();
	numeric->metric = *metric;
	return numeric;
}

/**
 * Deallocates a pointer to a Numeric struct.
 *
 * \param numeric the Numeric pointer to be deallocated.
 */
void numeric_destroy(struct Numeric *numeric)
{
	if (numeric != NULL) {
		del_simplenuobsvalue(&numeric->simple_nu_observed_value);
		del_simplenuobsvaluecmp(&numeric->compound_simple_nu_observed_value);
		del_basicnuobsvaluecmp(&numeric->compound_basic_nu_observed_value);
		del_nuobsvalue(&numeric->nu_observed_value);
		del_nuobsvaluecmp(&numeric->compound_nu_observed_value);
		metric_destroy(&numeric->metric);
	}
}

/**
 * Returns the class identification.
 *
 * \return code of class identification.
 */
int numeric_get_nomenclature_code()
{
	return MDC_MOC_VMO_METRIC_NU;
}

/** @} */
