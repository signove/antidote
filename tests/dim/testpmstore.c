/**********************************************************************
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
 * pmstore.c
 *
 * Created on: Jun 16, 2010
 *     Author: Jose Martins da Nobrega Filho
 *             fabricio.silva
 *             diego.bezerra
 **********************************************************************/

#ifdef TEST_ENABLED

#include <stdlib.h>
#include "testpmstore.h"
#include "Basic.h"
#include "src/dim/pmstore.h"
#include "src/dim/pmstore.h"
#include "src/dim/pmsegment.h"
#include "testdateutil.h"
#include "src/util/dateutil.h"

int testpmstore_init_suite(void)
{
	return 0;
}

int testpmstore_finish_suite(void)
{
	return 0;
}

void testpmstore_add_suite()
{
	CU_pSuite suite = CU_add_suite("PMStore Test Suite",
				       testpmstore_init_suite, testpmstore_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_pmstore_add_and_clear_segment",
		    test_pmstore_add_and_clear_segment);

	CU_add_test(suite, "test_pmstore_date_selection",
		    test_pmstore_date_selection);

	/* Add tests here - End */

}

void test_pmstore_add_and_clear_segment(void)
{
	struct PMStore *pmstore = pmstore_instance();
	CU_ASSERT_PTR_NOT_NULL(pmstore);

	// No pmstore capability enabled
	pmstore->pm_store_capab = 0; // turn of all bits

	SegmSelection all_selection;
	all_selection.choice = ALL_SEGMENTS_CHOSEN;

	InstNumber inst1 = 1;
	InstNumber inst2 = 2;
	InstNumber inst3 = 3;
	InstNumber inst4 = 4;

	/* Testing creation */
	struct PMSegment *segm1 = pmsegment_instance(inst1);
	// PmSegmentEntryMap *map1_1 = malloc(sizeof(PmSegmentEntryMap));
	// PmSegmentEntryMap *map1_2 = malloc(sizeof(PmSegmentEntryMap));
	CU_ASSERT_PTR_NOT_NULL(segm1);
	// CU_ASSERT_PTR_NOT_NULL(map1_1);
	// CU_ASSERT_PTR_NOT_NULL(map1_2);

	struct PMSegment *segm2 = pmsegment_instance(inst2);
	// PmSegmentEntryMap *map2 = malloc(sizeof(PmSegmentEntryMap));
	CU_ASSERT_PTR_NOT_NULL(segm2);
	// CU_ASSERT_PTR_NOT_NULL(map2);

	struct PMSegment *segm3 = pmsegment_instance(inst3);
	CU_ASSERT_PTR_NOT_NULL(segm3);

	struct PMSegment *segm4 = pmsegment_instance(inst4);
	CU_ASSERT_PTR_NOT_NULL(segm4);

	pmstore_add_segment(pmstore, segm1);
	pmstore_add_segment(pmstore, segm2);
	pmstore_add_segment(pmstore, segm3);

	/* Testing list */
	int size = pmstore->segment_list_count;
	CU_ASSERT_EQUAL(size, 3);

	// seg1 has 2 entries
	CU_ASSERT_EQUAL(pmstore->segm_list[0]->instance_number, segm1->instance_number);
	// pmsegment_add_entry(segm1, map1_1);
	// pmsegment_add_entry(segm1, map1_2);
	// CU_ASSERT_EQUAL(segm1->entry_number, 2);
	// CU_ASSERT_EQUAL(pmstore->segm_list[0]->entry_number, 2);

	// seg2 has 1 entry
	CU_ASSERT_EQUAL(pmstore->segm_list[1]->instance_number, segm2->instance_number);
	// pmsegment_add_entry(segm2, map2);
	// CU_ASSERT_EQUAL(segm2->entry_number, 1);
	// CU_ASSERT_EQUAL(pmstore->segm_list[1]->entry_number, 1);

	// seg3
	CU_ASSERT_EQUAL(pmstore->segm_list[2]->instance_number, segm3->instance_number);
	// CU_ASSERT_EQUAL(pmstore->segm_list[2]->entry_number, 0);



	/* Testing clear and deletion */
	// Try to clear without remove capability
	pmstore_service_action_clear_segments(pmstore, all_selection);
	CU_ASSERT_EQUAL(pmstore->segment_list_count, size);


	// check all entries are clean
	// int i = 0;
	// for (i = 0; i < pmstore->number_of_segments; i++){
	//	CU_ASSERT_EQUAL(pmstore->segm_list[i]->entry_number, 0);
	// }

	// Try to remove with capability
	pmstore->pm_store_capab = pmsc_clear_segm_remove;
	pmstore_service_action_clear_segments(pmstore, all_selection);
	CU_ASSERT_EQUAL(pmstore->number_of_segments, 0); // should be empty

	free(pmstore);
	// free(map1_1);
	// free(map1_2);
	// free(map2);

	free(segm4);

}

void test_pmstore_date_selection(void)
{
	struct PMStore *pmstore = pmstore_instance();
	CU_ASSERT_PTR_NOT_NULL(pmstore);

	// Defining pmstore capabilities
	pmstore->pm_store_capab = pmsc_abs_time_select;

	InstNumber inst1 = 1;
	InstNumber inst2 = 2;
	InstNumber inst3 = 3;
	InstNumber inst4 = 4;

	/* Testing creation */
	struct PMSegment *segm1 = pmsegment_instance(inst1);
	CU_ASSERT_PTR_NOT_NULL(segm1);
	segm1->segment_start_abs_time = date_util_create_absolute_time(2010,
					01, 01, 14, 00, 11, 01);
	segm1->segment_end_abs_time = date_util_create_absolute_time(2010, 01,
				      01, 14, 00, 21, 01);
	PmSegmentEntryMap *map1 = malloc(sizeof(PmSegmentEntryMap));

	struct PMSegment *segm2 = pmsegment_instance(inst2);
	CU_ASSERT_PTR_NOT_NULL(segm2);
	segm2->segment_start_abs_time = date_util_create_absolute_time(2010,
					01, 01, 14, 00, 31, 01);
	segm2->segment_end_abs_time = date_util_create_absolute_time(2010, 01,
				      01, 14, 00, 41, 01);
	PmSegmentEntryMap *map2 = malloc(sizeof(PmSegmentEntryMap));

	struct PMSegment *segm3 = pmsegment_instance(inst3);
	CU_ASSERT_PTR_NOT_NULL(segm3);
	segm3->segment_start_abs_time = date_util_create_absolute_time(2010,
					01, 01, 14, 01, 10, 01);
	segm3->segment_end_abs_time = date_util_create_absolute_time(2010, 01,
				      01, 14, 01, 11, 01);

	// Next day entry
	struct PMSegment *segm4 = pmsegment_instance(inst4);
	CU_ASSERT_PTR_NOT_NULL(segm4);
	segm4->segment_start_abs_time = date_util_create_absolute_time(2010,
					01, 02, 15, 00, 00, 00);
	segm4->segment_end_abs_time = date_util_create_absolute_time(2010, 01,
				      02, 15, 00, 10, 00);

	pmstore_add_segment(pmstore, segm1);
	pmstore_add_segment(pmstore, segm2);
	pmstore_add_segment(pmstore, segm3);
	pmstore_add_segment(pmstore, segm4);

	// pmsegment_add_entry(segm1, map1);
	// pmsegment_add_entry(segm2, map2);

	/* Testing list */
	int size = pmstore->segment_list_count;
	CU_ASSERT_EQUAL(size, 4);

	/* Test Selection */
	SegmSelection date_selection;
	date_selection.choice = ABS_TIME_RANGE_CHOSEN;

	date_selection.u.abs_time_range.from_time
	= date_util_create_absolute_time(2010,
					 01, 01, 00, 00, 00, 00);
	date_selection.u.abs_time_range.to_time
	= date_util_create_absolute_time(2010,
					 01, 03, 00, 00, 00, 00);

	// Clear all
	pmstore_service_action_clear_segments(pmstore, date_selection);
	CU_ASSERT_EQUAL(pmstore->segment_list_count, size);
	// int i = 0;
	// for (i = 0; i < pmstore->number_of_segments; i++) {
	//	CU_ASSERT_EQUAL(pmstore->segm_list[i]->entry_number, 0);
	// }

	// Clear and Remove one entry
	pmstore->pm_store_capab = pmsc_abs_time_select | pmsc_clear_segm_remove;
	date_selection.u.abs_time_range.from_time
	= date_util_create_absolute_time(2010, 01, 02, 00, 00,
					 00, 00);
	date_selection.u.abs_time_range.to_time
	= date_util_create_absolute_time(2010, 01, 02, 23, 59,
					 00, 00);

	pmstore_service_action_clear_segments(pmstore, date_selection);
	CU_ASSERT_EQUAL(pmstore->segment_list_count, 3);

	// Clear and Remove all
	pmstore->pm_store_capab = pmsc_abs_time_select | pmsc_clear_segm_remove;
	date_selection.u.abs_time_range.from_time
	= date_util_create_absolute_time(2009, 01, 01, 01, 00,
					 00, 00);
	date_selection.u.abs_time_range.to_time
	= date_util_create_absolute_time(2011, 01, 01, 01, 00,
					 00, 00);

	pmstore_service_action_clear_segments(pmstore, date_selection);
	CU_ASSERT_EQUAL(pmstore->number_of_segments, 0);

	free(pmstore);
	free(map1);
	free(map2);

}

#endif /* PMSTORE_C_ */
