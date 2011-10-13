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
 * test_mds.c
 *
 * Created on: Aug 10, 2010
 *     Author: fabricio.silva
 **********************************************************************/
#ifdef TEST_ENABLED

#include "Basic.h"
#include "src/asn1/phd_types.h"
#include "src/dim/mds.h"
#include "testmds.h"
#include <stdlib.h>

int test_mds_init_suite(void)
{
	return 0;
}

int test_mds_finish_suite(void)
{
	return 0;
}

void testmds_add_suite()
{
	CU_pSuite suite = CU_add_suite("MDS Test Suite", test_mds_init_suite,
				       test_mds_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_mds_is_supported_data_request",
		    test_mds_is_supported_data_request);
	/* Add tests here - End */

}

void test_mds_is_supported_data_request(void)
{
	MDS *mds = mds_create();
	//DataReqModeCapab cap;
	//cap.data_req_init_agent_count = 1;
	//cap.data_req_init_manager_count = 1;
	//cap.data_req_mode_flags = 0;

	mds->data_req_mode_capab.data_req_mode_flags = DATA_REQ_SUPP_STOP | DATA_REQ_SUPP_MODE_SINGLE_RSP | DATA_REQ_SUPP_SCOPE_ALL;

	CU_ASSERT_TRUE(mds_is_supported_data_request(mds, (!DATA_REQ_START_STOP) | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_ALL));
	CU_ASSERT_FALSE(mds_is_supported_data_request(mds, (!DATA_REQ_START_STOP) | DATA_REQ_MODE_TIME_NO_LIMIT | DATA_REQ_SCOPE_ALL));
	CU_ASSERT_FALSE(mds_is_supported_data_request(mds, (!DATA_REQ_START_STOP) | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_HANDLE));
	CU_ASSERT_TRUE(mds_is_supported_data_request(mds, DATA_REQ_CONTINUATION | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_ALL));
	CU_ASSERT_TRUE(mds_is_supported_data_request(mds, (!DATA_REQ_START_STOP) | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_ALL));


	mds->data_req_mode_capab.data_req_mode_flags = (!DATA_REQ_SUPP_STOP) | DATA_REQ_SUPP_MODE_SINGLE_RSP | DATA_REQ_SUPP_SCOPE_ALL;
	CU_ASSERT_TRUE(mds_is_supported_data_request(mds, DATA_REQ_START_STOP | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_ALL));
	CU_ASSERT_TRUE(mds_is_supported_data_request(mds, DATA_REQ_CONTINUATION | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_ALL));
	CU_ASSERT_FALSE(mds_is_supported_data_request(mds, (!DATA_REQ_START_STOP) | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_ALL));

	mds->data_req_mode_capab.data_req_mode_flags = 0;
	CU_ASSERT_FALSE(mds_is_supported_data_request(mds, (!DATA_REQ_START_STOP) | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_ALL));
	CU_ASSERT_FALSE(mds_is_supported_data_request(mds, (!DATA_REQ_START_STOP) | DATA_REQ_MODE_TIME_NO_LIMIT | DATA_REQ_SCOPE_ALL));
	CU_ASSERT_FALSE(mds_is_supported_data_request(mds, (!DATA_REQ_START_STOP) | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_HANDLE));
	CU_ASSERT_FALSE(mds_is_supported_data_request(mds, DATA_REQ_CONTINUATION | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_ALL));
	CU_ASSERT_FALSE(mds_is_supported_data_request(mds, (!DATA_REQ_START_STOP) | DATA_REQ_MODE_SINGLE_RSP | DATA_REQ_SCOPE_ALL));
	CU_ASSERT_FALSE(mds_is_supported_data_request(mds, 0));


	mds_destroy(mds);
}

#endif
