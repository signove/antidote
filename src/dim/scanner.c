/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* \file scanner.c
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

#include <stdlib.h>
#include "scanner.h"
#include "src/communication/operating.h"
#include "src/communication/parser/struct_cleaner.h"

/**
 * \defgroup Scanner Scanner
 * \ingroup ObjectClasses
 * \brief Scanner Object Class
 *
 * A scanner object is an observer and “summarizer” of object attribute
 * values. It observes attributes of metric objects (e.g., numeric objects)
 * and generates summaries in the form of notification event reports. The
 * different scanners are summarized on the following UML diagram.
 *
 * \image html scanner.png
 *
 * The different scanner classes (periodic and episodic) as well as the
 * different instances should be used to distribute different data types
 * across one or many data flows represented by a scanner instance.
 *
 * A pulse oximetry application might choose to have a periodic configurable scanner
 * for RT-SA objects with a Reporting-Interval of 50 ms, a periodic configurable
 * scanner with a Reporting-Interval of 1 s for numeric and enumeration
 * objects, and an episodic configurable scanner for beat-to-beat metric
 * objects (numeric or enumeration objects).
 *
 * @{
 */

/**
 * Time out (seconds) SET
 */
static const intu32 SCANNER_TO_SET = 3;


/**
 * Returns one instance of the Scanner structure.
 * The attributes must be defined through direct assignments, including
 * conditional and optional attributes.
 *
 * \param handle Scanners are identified by handles. This attribute shall
 * remain unchanged after the configuration is agreed upon.
 *
 * \param operational_state This attribute defines if scanner is active and
 * can be set by the manager.
 *
 * \return a pointer to a Scanner structure.
 */
struct Scanner *scanner_instance(HANDLE handle,
				 OperationalState operational_state) {
	struct Scanner *result = calloc(1, sizeof(struct Scanner));
	result->dim.id = scanner_get_nomenclature_code();
	result->handle = handle;
	result->operational_state = operational_state;
	result->scan_handle_list.count = 0;
	result->scan_handle_list.length = 0;
	result->scan_handle_list.value = NULL;
	result->scan_handle_attr_val_map.count = 0;
	result->scan_handle_attr_val_map.length = 0;
	result->scan_handle_attr_val_map.value = NULL;
	return result;
}

/**
 * Deallocates a pointer to a Scanner struct.
 *
 * \param self the Scanner pointer to be deallocated.
 */
void scanner_destroy(struct Scanner *self)
{
	if (self != NULL) {
		del_handlelist(&self->scan_handle_list);
		del_handleattrvalmap(&self->scan_handle_attr_val_map);
	}
}


/**
 * Agents that have scanner derived objects shall support the
 * SET service for the Operational-State attribute of the scanner objects.
 *
 * \param ctx
 * \param scanner the scanner to have operation state set.
 * \param new_operational_state the new operational state.
 * \param callback callback function of the request
 *
 * \return a pointer to a request just done.
 */
Request *scanner_set_operational_state(Context *ctx, struct Scanner *scanner,
				       OperationalState new_operational_state, service_request_callback callback)
{
	return operating_set_scanner(ctx, scanner->handle, new_operational_state, SCANNER_TO_SET, callback);
}

/**
 * Agents that have scanner derived objects shall support the
 * SET service for the Operational-State attribute of the scanner objects.
 *
 * In this case, the attribute should any be modified after receiving the agent
 * response.
 *
 * \param scanner the scanner to have operation state set.
 * \param new_operational_state the new operational state.
 */
void scanner_set_operational_state_response(struct Scanner *scanner, OperationalState new_operational_state)
{
	scanner->operational_state = new_operational_state;
}

/**
 * Returns the code used to identify the scanner structure.
 *
 * \return the code that identify the scanner structure.
 */
int scanner_get_nomenclature_code()
{
	return MDC_MOC_SCAN;
}

/** @} */
