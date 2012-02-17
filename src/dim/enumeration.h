/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file enumeration.h
 * \brief Enumeration type definition.
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

#ifndef ENUMERATION_H_
#define ENUMERATION_H_

#include "metric.h"

/**
 * Enumeration object struct
 */
struct Enumeration {
	/**
	 * The DIM structure
	 */
	struct DIM dim;

	/**
	 * The Metric structure
	 */
	struct Metric metric;

	/**
	 * The name is reported as a nomenclature code. If
	 * the Enum-Observed-Value-Partition attribute is valued,
	 * it defines the nomenclature partition for the attribute.
	 * Otherwise, the OID-Type is taken from the same nomenclature
	 * partition as defined in the partition field of the Type
	 * attribute
	 *
	 * Qualifier: Conditional
	 */
	OID_Type enum_observed_value_simple_OID;

	/**
	 * The name is reported as a bit string of 32-bits
	 *
	 * Qualifier: Conditional
	 */
	BITS_32 enum_observed_value_simple_bit_str;

	/**
	 * The name is reported as a bit string of 16-bits
	 *
	 * Qualifier: Conditional
	 */
	BITS_16 enum_observed_value_basic_bit_str;

	/**
	 * The name is reported as an ASCII printable string
	 *
	 * Qualifier: Conditional
	 */
	EnumPrintableString enum_observed_value_simple_str;

	/**
	 * This attribute defines a structured observed value that
	 * permits additional flexibility about the data type of the
	 * reported value
	 *
	 * Qualifier: Conditional
	 */
	EnumObsValue enum_observed_value;

	/**
	 * This attribute may be used to define the partition from
	 * which the Enum-Observed-value-Simple-OID or the
	 * Enum-Observed-Value's observation OID nomenclature term
	 * was taken
	 *
	 * Qualifier: Optional
	 */
	NomPartition enum_observed_value_partition;

	/**
	 * Use nomenclature partition
	 */
	int use_nom_partition;
};

struct Enumeration *enumeration_instance(struct Metric *metric);

int enumeration_get_nomenclature_code();

void enumeration_destroy(struct Enumeration *enumeration);

#endif /* ENUMERATION_H_ */
