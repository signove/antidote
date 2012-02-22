/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * \file scanner.h
 * \brief Scanner definition.
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
 * \date Jun 09, 2010
 * \author Jose Martins
 */

#ifndef SCANNER_H_
#define SCANNER_H_

#include "asn1/phd_types.h"
#include "communication/service.h"
#include "nomenclature.h"
#include "dim.h"

/**
 * \brief The Scanner is an struct defining attributes that are common
 * for compose specialized Scanners.
 *
 * The scanner concept provides three different event report notifications:
 * variable format, fixed format, and grouped format. these events are described
 * in the following functions.
 */
struct Scanner {
	/**
	 * The DIM structure
	 */
	struct DIM dim;

	/**
	 * The Handle attribute represents a reference ID for this object.
	 * Each object shall have a unique ID assigned by the agent. The
	 * handle identifies the object in event reports sent to the
	 * manager and to address the object instance in messages
	 * invoking object methods.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	HANDLE handle;

	/**
	 * This attribute defines if scanner is active and can be set by the
	 * manager.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	OperationalState operational_state;

	/**
	 * This attribute defines the metric-derived objects that
	 * might be reported in the Unbuf-Scan-Report-Var, Buf-Scan-Report-Var,
	 * Unbuf-Scan-Report-Fixed, Buf-Scan-Report-Fixed, or any of the
	 * four multiple-person equivalents.
	 *
	 * Qualifier: Conditional
	 *
	 */
	HANDLEList scan_handle_list;

	/**
	 * This attribute defines the metric-derived objects, the
	 * attributes, and the order in which objects and attribute
	 * values are reported in a Unbuf-Scan-Report-Grouped,
	 * Buf-Scan-Report-Grouped, Unbuf-Scan-Report-MP-Grouped, or
	 * Buf-Scan-Report-MP-Grouped.
	 *
	 * Qualifier: Conditional
	 *
	 */
	HandleAttrValMap scan_handle_attr_val_map;
};

struct Scanner *scanner_instance(HANDLE handle,
				 OperationalState operational_state);

void scanner_destroy(struct Scanner *self);

Request *scanner_set_operational_state(Context *ctx, struct Scanner *scanner,
				       OperationalState new_operational_state, service_request_callback callback);

void scanner_set_operational_state_response(struct Scanner *scanner,
		OperationalState new_operational_state);

int scanner_get_nomenclature_code();

#endif /* SCANNER_H_ */
