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
 * test_association.h
 *
 * Created on: Jul 21, 2010
 *     Author: marcos.pereira
 **********************************************************************/

#ifdef TEST_ENABLED

#ifndef TEST_DISASSOCIATING_H_
#define TEST_DISASSOCIATING_H_

int test_disassociating_init_suite();

int test_disassociating_finish_suite();

void functionaltest_disassociating_add_suite();

/**
 * TC 4.1
 *
 * 1 - Manager starts in 'Disconnected' state
 * 2 - Manager connects
 * 3 - Manager enters in 'Unassociated' state
 * 4 - Agent sends association request
 * 5 - Manager accepts association request
 * 6 - Manager enters in 'Operating' state
 * 7 - Manager requests association release
 * 8 - Manager enters in 'Disassociating' state
 * 9 - Manager requests association release
 * 10 - Manager stays in 'Disassociating' state
 */
void functional_test_disassociating_tc_4_1();

/**
 * TC 4.2
 *
 * 1 - Manager starts in 'Disconnected' state
 * 2 - Manager connects
 * 3 - Manager enters in 'Unassociated' state
 * 4 - Agent sends association request
 * 5 - Manager accepts association request
 * 6 - Manager enters in 'Operating' state
 * 7 - Manager requests association release
 * 8 - Manager enters in 'Disassociating' state
 * 9 - Manager request abort association
 * 10 - Manager enters in 'Unassociated' state
 */
void functional_test_disassociating_tc_4_2();

/**
 * TC 4.3
 *
 * 1 - Manager starts in 'Disconnected' state
 * 2 - Manager connects
 * 3 - Manager enters in 'Unassociated' state
 * 4 - Agent sends association request
 * 5 - Manager accepts association request
 * 6 - Manager enters in 'Operating' state
 * 7 - Manager requests association release
 * 8 - Manager enters in 'Disassociating' state
 * 9 - Agent sends association request
 * 10 - Manager enters in 'Unassociated' state
 */
void functional_test_disassociating_tc_4_3();

/**
 * TC 4.4
 *
 * 1 - Manager starts in 'Disconnected' state
 * 2 - Manager connects
 * 3 - Manager enters in 'Unassociated' state
 * 4 - Agent sends association request
 * 5 - Manager accepts association request
 * 6 - Manager enters in 'Operating' state
 * 7 - Manager requests association release
 * 8 - Manager enters in 'Disassociating' state
 * 9 - Agent sends association response
 * 10 - Manager enters in Unassociated state
 */
void functional_test_disassociating_tc_4_4();

/**
 * TC 4.5
 *
 * 1 - Manager starts in 'Disconnected' state
 * 2 - Manager connects
 * 3 - Manager enters in 'Unassociated' state
 * 4 - Agent sends association request
 * 5 - Manager accepts association request
 * 6 - Manager enters in 'Operating' state
 * 7 - Manager requests association release
 * 8 - Manager enters in 'Disassociating' state
 * 9 - Agent sends release association request
 * 10 - Manager stays in 'Disassociating' state
 */
void functional_test_disassociating_tc_4_5();

/**
 * TC 4.6
 *
 * 1 - Manager starts in 'Disconnected' state
 * 2 - Manager connects
 * 3 - Manager enters in 'Unassociated' state
 * 4 - Agent sends association request
 * 5 - Manager accepts association request
 * 6 - Manager enters in 'Operating' state
 * 7 - Manager requests association release
 * 8 - Manager enters in 'Disassociating' state
 * 9 - Agent sends release association response
 * 10 - Manager enters in 'Unassociated' state
 */
void functional_test_disassociating_tc_4_6();

/**
 * TC 4.7
 *
 * 1 - Manager starts in 'Disconnected' state
 * 2 - Manager connects
 * 3 - Manager enters in 'Unassociated' state
 * 4 - Agent sends association request
 * 5 - Manager accepts association request
 * 6 - Manager enters in 'Operating' state
 * 7 - Manager requests association release
 * 8 - Manager enters in 'Disassociating' state
 * 9 - Agent sends abort association
 * 10 - Manager enters in 'Unassociated' state
 */
void functional_test_disassociating_tc_4_7();

/**
 * TC 4.8
 *
 * 1 - Manager starts in 'Disconnected' state
 * 2 - Manager connects
 * 3 - Manager enters in 'Unassociated' state
 * 4 - Agent sends association request
 * 5 - Manager accepts association request
 * 6 - Manager enters in 'Operating' state
 * 7 - Manager requests association release
 * 8 - Manager enters in 'Disassociating' state
 * 9 - Agent sends remove invoke operation
 * 10 - Manager stays in 'Disassociated' state
 */
void functional_test_disassociating_tc_4_8();

/**
 * TC 4.9
 *
 * 1 - Manager starts in 'Disconnected' state
 * 2 - Manager connects
 * 3 - Manager enters in 'Unassociated' state
 * 4 - Agent sends association request
 * 5 - Manager accepts association request
 * 6 - Manager enters in 'Operating' state
 * 7 - Manager requests association release
 * 8 - Manager enters in 'Disassociating' state
 * 9 - Agent sends remove invoke operation response
 * 10 - Manager stays in 'Unassociated' state
 */
void functional_test_disassociating_tc_4_9();

void assert_tc_4_9();



#endif /* TEST_DISASSOCIATING_H_ */
#endif
