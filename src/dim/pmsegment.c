/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * \file pmsegment.c
 * \brief PMSegment definition
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
 * \author Walter Guerra
 */

#include <stdlib.h>
#include "pmsegment.h"
#include "src/communication/parser/struct_cleaner.h"

/**
 * \defgroup PMSegment PMSegment
 * \ingroup ObjectClasses
 * \brief PMSegment Object Class
 *
 * An instance of the PM-segment class represents a persistently
 * stored episode of measurement data.
 *
 * A PMsegment object is not part of the static agent configuration
 * because the number of instantiated PM-segment instances may
 * dynamically change. The manager accesses PM-segment objects
 * indirectly by methods and events of the PM-store object
 *
 * @{
 */

/**
 * Returns one instance of the PMSegment structure.
 * The attributes must be defined through direct assignments, including
 * conditional and optional attributes.
 *
 * \param instance_number the instance number of the PMSegment
 *
 * \return a pointer to a PMSegment structure.
 */
struct PMSegment *pmsegment_instance(InstNumber instance_number)
{
	struct PMSegment *segment = calloc(1, sizeof(struct PMSegment));

	if (segment == NULL) {
		return segment;
	}

	segment->instance_number = instance_number;
	segment->dim.id = pmsegment_get_nomenclature_code();
	segment->fixed_segment_data.value = NULL;
	segment->fixed_segment_data.length = 0;
	segment->segment_usage_count = 0;
	segment->empiric_usage_count = 0;
	return segment;
}

/**
 * Removes all entries of a PMSegment.
 *
 * \param pm_segment the PMSegment to have the entries removed.
 *
 */
void pmsegment_remove_all_entries(struct PMSegment *pm_segment)
{
	if (pm_segment->fixed_segment_data.value != NULL) {
		pm_segment->segment_usage_count = 0;
		pm_segment->empiric_usage_count = 0;
		free(pm_segment->fixed_segment_data.value);
		pm_segment->fixed_segment_data.value = NULL;
	}
}

/**
 * Returns the code used to identify the PMSegment structure.
 *
 * \return the code that identify the PMSegment structure.
 */
int pmsegment_get_nomenclature_code()
{
	return MDC_MOC_PM_SEGMENT;
}

/**
 * Finalizes and deallocate the given PMSegment.
 *
 * \param pm_segment the PMSegment to be destroyed.
 */
void pmsegment_destroy(struct PMSegment *pm_segment)
{
	if (pm_segment != NULL) {
		del_pmsegmententrymap(&pm_segment->pm_segment_entry_map);
		del_octet_string(&pm_segment->segment_label);
		del_absolutetimeadjust(&pm_segment->date_and_time_adjustment);
		del_segmentstatistics(&pm_segment->segment_statistics);
		del_octet_string(&pm_segment->fixed_segment_data);
	}
}

/** @} */
