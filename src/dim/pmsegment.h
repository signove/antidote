/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * \file pmsegment.h
 * \brief PMSegment definition.
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

#ifndef PMSEGMENT_H_
#define PMSEGMENT_H_

#include <stdlib.h>
#include "asn1/phd_types.h"
#include "nomenclature.h"
#include "dim.h"

/**
 * \brief An instance of the PM-segment class represents a
 * persistently stored episode of measurement data.
 */
struct PMSegment {

	/**
	 * The DIM struct.
	 */
	struct DIM dim;

	/**
	 * The Instance-Number is the ID of a specific PM-segment
	 * object instance. It is used by the manager to address a PMsegment.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	InstNumber instance_number;

	/**
	 * This attribute defines the format and contents of one
	 * stored entry. An entry has an optional header containing
	 * information applicable to all elements in the entry. The
	 * entry then contains one or more elements, defined by the
	 * class, metric ID, handle, and an attribute value map
	 * defining the object attributes for each element in the PMsegment.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	PmSegmentEntryMap pm_segment_entry_map;

	/**
	 * This standard supports devices that have simple
	 * support for data from multiple persons.
	 *
	 * Qualifier: Conditional
	 *
	 */
	PersonId pm_seg_person_id;

	/**
	 * This attribute indicates if new entries are currently being
	 * inserted into this PMsegment.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	OperationalState operational_state;

	/**
	 * This attribute defines the frequency at which entries are
	 * added to the PM-segment
	 *
	 * Qualifier: Conditional
	 *
	 */
	RelativeTime sample_period;

	/**
	 * This attribute is an application-dependent label in
	 * printable ASCII for the segment to indicate its
	 * intended use and may be used for display purposes.
	 *
	 * Qualifier: Optional
	 *
	 */
	octet_string segment_label;

	/**
	 * This attribute defines the start time of segment.
	 *
	 * Qualifier: Optional
	 *
	 */
	AbsoluteTime segment_start_abs_time;

	/**
	 * This attribute defines the end time of segment.
	 *
	 * Qualifier: Optional
	 *
	 */
	AbsoluteTime segment_end_abs_time;

	/**
	 * This attribute reports any date and time adjustments that
	 * occur either due to a person’s changing the clock or events
	 * such as daylight savings time.
	 *
	 * Qualifier: Conditional
	 *
	 */
	AbsoluteTimeAdjust date_and_time_adjustment;

	/**
	 * This attribute gives the actual (current) number of stored
	 * entries.
	 *
	 * Qualifier: Optional
	 *
	 */
	intu32 segment_usage_count;

	/**
	 * This attribute gives the actual (current) number of stored
	 * entries, based on actual data events. It shall be equal to
	 * segment_usage_count, unless the latter is outdated in local
	 * cache (getting segment info again solves this.)
	 *
	 */
	intu32 empiric_usage_count;

	/**
	 * This attribute defines the array for reporting minimum,
	 * mean, maximum statistics for each element to be tagged.
	 *
	 * Qualifier: Optional
	 *
	 */
	SegmentStatistics segment_statistics;

	/**
	 * This attribute defines the segment data transferred as an
	 * array of entries in a format as specified in the PM-Segment-Entry-Map
	 * attribute.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	octet_string fixed_segment_data;

	/**
	 * This informational timeout attribute defines the minimum
	 * time that the agent shall wait for a Response message from
	 * the manager after issuing a Confirmed Event Report
	 * invoke message before timing out and transitioning to the
	 * Unassociated state.
	 *
	 * Qualifier: Optional
	 *
	 */
	RelativeTime confirm_timeout;

	/**
	 * This timeout attribute defines the minimum time that the
	 * manager shall wait for the complete transfer of PMsegment
	 * information.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	RelativeTime transfer_timeout;

};

struct PMSegment *pmsegment_instance(InstNumber instance_number);

void pmsegment_remove_all_entries(struct PMSegment *pm_segment);

int pmsegment_get_nomenclature_code();

void pmsegment_destroy(struct PMSegment *pm_segment);

#endif /* PMSEGMENT_H_ */
