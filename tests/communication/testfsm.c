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
 * testfsm.c
 *
 * Created on: Jun 22, 2010
 *     Author: fabricio.silva
 **********************************************************************/
#ifdef TEST_ENABLED

#include "testfsm.h"
#include "Basic.h"
#include <stdio.h>
#include "src/communication/communication.h"
#include "src/communication/context_manager.h"
#include "src/communication/fsm.h"

void testfsm_action1(Context *ctx, fsm_events evt, FSMEventData *data);
void testfsm_action2(Context *ctx, fsm_events evt, FSMEventData *data);
void testfsm_action3(Context *ctx, fsm_events evt, FSMEventData *data);

int test_fsm_init_suite(void)
{
	return 0;
}

int test_fsm_finish_suite(void)
{
	return 0;
}

void testfsm_add_suite()
{
	CU_pSuite suite = CU_add_suite("FSM Test Suite", test_fsm_init_suite,
				       test_fsm_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_fsm", test_fsm);

	/* Add tests here - End */

}

void test_fsm()
{
	// Create connection context which include the state machine
	ContextId cid = {1, 99};
	Context *ctx = context_create(cid, MANAGER_CONTEXT);

	FSM *fsm = ctx->fsm;

	// Initialize FSM
	FsmTransitionRule transition_table[] = { { 0, 1, 1, &testfsm_action1 },
		{ 1, 2, 2, &testfsm_action2 }, {
			2, 3, 0,
			&testfsm_action3
		}
	};

	fsm_init(fsm, 0, transition_table, sizeof(transition_table)
		 / sizeof(struct FsmTransitionRule));

	CU_ASSERT_EQUAL(fsm->transition_table_size, 3);

	// Process inputs
	fsm_process_evt(ctx, 1, NULL);
	CU_ASSERT_EQUAL(fsm->state, 1);

	fsm_process_evt(ctx, 2, NULL);
	CU_ASSERT_EQUAL(fsm->state, 2);

	fsm_process_evt(ctx, 3, NULL);
	CU_ASSERT_EQUAL(fsm->state, 0);

	fsm_set_manager_state_table(fsm);
	CU_ASSERT_EQUAL(fsm->state, fsm_state_disconnected);

	context_remove(cid);
}

void testfsm_action1(Context *ctx, fsm_events evt, FSMEventData *data)
{
	printf("\n Transition Action 1 %d - fsm state %d\n", evt,
	       ctx->fsm->state);
}

void testfsm_action2(Context *ctx, fsm_events evt, FSMEventData *data)
{
	printf("\n Transition Action 2 %d - fsm state %d\n", evt,
	       ctx->fsm->state);
}

void testfsm_action3(Context *ctx, fsm_events evt, FSMEventData *data)
{
	printf("\n Transition Action 3 %d - fsm state %d\n", evt,
	       ctx->fsm->state);
}

#endif
