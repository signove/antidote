/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file numeric.h
 * \brief Numeric type definitions.
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

#ifndef NUMERIC_H_
#define NUMERIC_H_

#include "metric.h"

/**
 * Numeric object struct
 */
struct Numeric {
	/**
	 * The DIM structure
	 */
	struct DIM dim;

	/**
	 * The Metric structure
	 */
	struct Metric metric;

	/**
	 * This attribute reports the numerical observed value of the
	 * object without any7 further embedded status information as
	 * found in Nu-Observed-Value
	 *
	 * Qualifier: Conditional
	 */
	SimpleNuObsValue simple_nu_observed_value;

	/**
	 * This attribute represents an array of
	 * Simple-Nu-Observed-Values
	 *
	 * Qualifier: Conditional
	 */
	SimpleNuObsValueCmp compound_simple_nu_observed_value;

	/**
	 * This attribute represents an array of
	 * Simple-Nu-Observed-Values
	 *
	 * Qualifier: Conditional
	 */
	BasicNuObsValue basic_nu_observed_value;

	/**
	 * This attribute represents an array of
	 * Simple-Nu-Observed-Values
	 *
	 * Qualifier: Conditional
	 */
	BasicNuObsValueCmp compound_basic_nu_observed_value;

	/**
	 * This attribute defines the numerical observed
	 * value of the object and combines it with measurement
	 * status and unit information
	 *
	 * Qualifier: Conditional
	 */
	NuObsValue nu_observed_value;

	/**
	 * This attribute combines an array of value, status, and unit.
	 *
	 * Qualifier: Conditional
	 */
	NuObsValueCmp compound_nu_observed_value;

	/**
	 * This attribute defines the maximum deviation
	 * of actual value from reported observed value
	 *
	 * Qualifier: Optional
	 */
	FLOAT_Type accuracy;

};

struct Numeric *numeric_instance(struct Metric *metric);

void numeric_destroy(struct Numeric *numeric);

int numeric_get_nomenclature_code();

#endif /* NUMERIC_H_ */
