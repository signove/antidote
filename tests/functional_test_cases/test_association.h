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
 * Created on: Jul 9, 2010
 *     Author: fabricio.silva
 **********************************************************************/

#ifdef TEST_ENABLED

#ifndef TEST_ASSOCIATION_H_
#define TEST_ASSOCIATION_H_

void functionaltest_association_add_suite(void);

/**
 * TC 1.1
 *
 * 1 - Agent request association
 * 2 - Manager accepts unknown conf. and request configuration
 * 3 - Agent did not send response
 * 4 - Manager fire Event: Configuration Timeout
 * 5 - Check if machine state return to unassociated.
 * 6 - Manager disconnect
 *
 */
void functional_test_association_tc_1_1();

/**
 * TC 1.2 (Pulse oximeter)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Manager disconnect
 */
void functional_test_association_tc_1_2();

/**
 * TC 1.3 (Blood pressure)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Manager disconnect
 */
void functional_test_association_tc_1_3();

/**
 * TC 1.4 (Body composition)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Manager disconnect
 */
void functional_test_association_tc_1_4();

/**
 * TC 1.5 (Pulse oximeter - disassociation)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Agent request disassociation.
 * 5 - Manager accepts.
 * 6 - Enter in Unassociated state.
 */
void functional_test_association_tc_1_5();

/**
 * TC 1.6 (Blood pressure - disassociation)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Agent request disassociation.
 * 5 - Manager accepts.
 * 6 - Enter in Unassociated state.
 */
void functional_test_association_tc_1_6();

/**
 * TC 1.7 (Weighing scale - disassociation by agent)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Agent request disassociation.
 * 5 - Manager accepts.
 * 6 - Enter in Unassociated state.
 */
void functional_test_association_tc_1_7();


/**
 * TC 1.8 (Pulse oximeter - Association request again - Should not happen)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Agent request association again.
 * 5 - Manager aborts
 * 6 - Enter in Unassociated state.
 */
void functional_test_association_tc_1_8();

/**
 * TC 1.9 (Abort association)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Agent request to abort association.
 * 5 - Enter in Unassociated state.
 */
void functional_test_association_tc_1_9();

/**
 * TC 1.10 (Association response)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 5 - Agent sent a wrong aare apdu
 * 6 - Manager aborts and enter in Unassociated state.
 */
void functional_test_association_tc_1_10();

/**
 * TC 1.11 (Normal disassociation by manager)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Manager request disassociation.
 * 5 - Manager accepts.
 * 6 - Enter in Disassociated state.
 */
void functional_test_association_tc_1_11();

/**
 * TC 1.12 (Normal abort by manager)
 *
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Manager request abort.
 * 5 - Manager accepts.
 * 6 - Enter in Unassociated state.
 */
void functional_test_association_tc_1_12();

#endif /* TEST_ASSOCIATION_H_ */
#endif
