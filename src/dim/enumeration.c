/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file enumeration.c
 * \brief Enumeration type implementation.
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
#include "enumeration.h"
#include "nomenclature.h"
#include "src/communication/parser/struct_cleaner.h"

/**
 * \defgroup Enumeration Enumeration
 * \ingroup ObjectClasses
 * \brief Enumeration Object Class
 *
 * An instance of the enumeration class represents status information and/or
 * annotation information.
 *
 * The values of the enumeration object are coded in the form of normative
 * codes (as defined in ISO/IEEE 11073-10101) or in the form of free text.
 * The values of the enumeration object are sent from the agent to the
 * manager using the EVENT REPORT service. This class is derived from the
 * metric base class.
 *
 * @{
 */

/**
 * Creates a new instance of Enumeration object class.
 *
 * \param metric the metric field of the Enumeration struct just created.
 *
 * \return a pointer to Enumeration struct.
 */
struct Enumeration *enumeration_instance(struct Metric *metric)
{
	struct Enumeration *enumeration = calloc(1, sizeof(struct Enumeration));
	enumeration->dim.id = enumeration_get_nomenclature_code();
	enumeration->metric = *metric;
	return enumeration;
}

/**
 * Returns the class identification.
 *
 * \return code of class identification.
 */
int enumeration_get_nomenclature_code()
{
	return MDC_MOC_VMO_METRIC_ENUM;
}

/**
 * Deallocates a pointer to a Enumeration struct.
 *
 * \param enumeration the Enumeration pointer to be deallocated.
 */
void enumeration_destroy(struct Enumeration *enumeration)
{
	if (enumeration != NULL) {
		del_enumobsvalue(&(enumeration->enum_observed_value));
		del_enumprintablestring(&(enumeration->enum_observed_value_simple_str));
		metric_destroy(&(enumeration->metric));
	}
}

/** @} */
