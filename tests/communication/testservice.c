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
 * testservice.c
 *
 * Created on: Jul 8, 2010
 *     Author: mateus.lima
**********************************************************************/

#ifdef TEST_ENABLED

#include "testservice.h"
#include "Basic.h"
#include "src/manager_p.h"
#include "src/communication/communication.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/communication/context_manager.h"
#include "src/communication/parser/struct_cleaner.h"
#include "tests/functional_test_cases/test_functional.h"


#include <stdlib.h>
#include <stdio.h>

void test_service_callback_state(ServiceState state);

int test_service_init_suite(void)
{
	return functional_test_init();
}

int test_service_finish_suite(void)
{
	return  functional_test_finish();
}

void testservice_add_suite()
{
	CU_pSuite suite = CU_add_suite("Service Test Suite", test_service_init_suite,
				       test_service_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_service", test_service);

	/* Add tests here - End */

}

void test_service_assert_request_retired(Context *ctx, InvokeIDType invoke_id)
{
	DATA_apdu response_apdu;
	response_apdu.invoke_id = invoke_id;
	CU_ASSERT_TRUE(service_get_current_invoke_id(ctx) == response_apdu.invoke_id);
	service_request_retired(ctx, &response_apdu);

	CU_ASSERT_FALSE(service_is_id_valid(ctx, response_apdu.invoke_id));

}

void test_service()
{
	manager_start();


	Context *ctx = context_get(FUNC_TEST_SINGLE_CONTEXT);
	int size = 21;
	APDU **apdu_list = calloc(size, sizeof(APDU));
	int i = 0;

	for (i = 0; i < size; ++i) {
		APDU *apdu = calloc(1, sizeof(APDU));
		apdu->choice = PRST_CHOSEN;
		apdu->length = 14;
		apdu->u.prst.length = 12;

		DATA_apdu *data_apdu = calloc(1, sizeof(DATA_apdu));
		data_apdu->invoke_id = 0;
		data_apdu->message.choice = ROIV_CMIP_GET_CHOSEN;
		data_apdu->message.length = 6;
		data_apdu->message.u.roiv_cmipGet.obj_handle = 0;
		data_apdu->message.u.roiv_cmipGet.attribute_id_list.count = 0;
		data_apdu->message.u.roiv_cmipGet.attribute_id_list.length = 0;
		data_apdu->message.u.roiv_cmipGet.attribute_id_list.value = NULL;

		encode_set_data_apdu(&apdu->u.prst, data_apdu);
		apdu_list[i] = apdu;
	}

	service_init(ctx);

	timeout_callback no_timeout = NO_TIMEOUT;

	service_send_remote_operation_request(ctx, apdu_list[0], no_timeout, NULL);
	service_finalize(ctx, NULL);
	test_service_assert_request_retired(ctx, 0);

	service_send_remote_operation_request(ctx, apdu_list[1], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[2], no_timeout, NULL);
	test_service_assert_request_retired(ctx, 0);
	test_service_assert_request_retired(ctx, 1);

	service_send_remote_operation_request(ctx, apdu_list[3], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[4], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[5], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[6], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[7], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[8], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[9], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[10], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[11], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[12], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[13], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[14], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[15], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[16], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[17], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[18], no_timeout, NULL);
	service_send_remote_operation_request(ctx, apdu_list[19], no_timeout, NULL);

	test_service_assert_request_retired(ctx, 2);
	test_service_assert_request_retired(ctx, 3);
	test_service_assert_request_retired(ctx, 4);
	test_service_assert_request_retired(ctx, 5);
	test_service_assert_request_retired(ctx, 6);
	test_service_assert_request_retired(ctx, 7);
	test_service_assert_request_retired(ctx, 8);
	test_service_assert_request_retired(ctx, 9);
	test_service_assert_request_retired(ctx, 10);
	test_service_assert_request_retired(ctx, 11);
	test_service_assert_request_retired(ctx, 12);
	test_service_assert_request_retired(ctx, 13);
	test_service_assert_request_retired(ctx, 14);
	test_service_assert_request_retired(ctx, 15);
	test_service_assert_request_retired(ctx, 0);
	test_service_assert_request_retired(ctx, 1);
	test_service_assert_request_retired(ctx, 2);

	service_send_remote_operation_request(ctx, apdu_list[20], no_timeout, NULL);
	DATA_apdu response_apdu;
	response_apdu.invoke_id = 0;

	service_request_retired(ctx, &response_apdu); // not retired

	free(apdu_list);
	apdu_list = NULL;

	manager_stop();
}


#endif
