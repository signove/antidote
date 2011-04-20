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
 * test_operating.h
 *
 * Created on: Jul 16, 2010
 *     Author: diego
**********************************************************************/

#ifndef TEST_OPERATING_H_
#define TEST_OPERATING_H_

void functionaltest_operating_add_suite(void);

/**
 * TC 3.1 (Blood pressure - get all)
 *
 * 1 - Association to enter in operational state
 * 2 - Manager requests "MDS GET ALL"
 * 3 - Agent send response
 * 4 - Manager update MDS
 * 5 - Manager disconnects
 */
void functional_test_operating_tc_3_1();

/*
 * TC 3.2 (Blood pressure - scan report info var)
 *  1 - Association to enter in operational state
 *  2 - Manager requests "MDS GET ALL"
 *  3 - Agent send response
 *  4 - Manager update MDS
 *  5 - Agent reports var data
 *  6 - Metric values updated
 *  7 - Manager disconnects
 */
void functional_test_operating_tc_3_2();

/*
TC 3.3 (Blood pressure - scan report info fixed)
1 - Association to enter in operational state
2 - Manager requests "MDS GET ALL"
3 - Agent send response
4 - Manager update MDS
5 - Agent reports fixed data
6 - Metric values updated
7 - Manager disconnects
*/
void functional_test_operating_tc_3_3();

/**
 * TC 3.4 (Weighing scale - MDS Get)
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Manager request mds attributes (GET all)
 * 5 - Agent responds with the values
 */
void functional_test_operating_tc_3_4();

/*
TC 3.5 (Weighing Scale - data report - scan report info fixed)
1 - Association to enter in operational state (config not known)
2 - Agent send extended configuration
3 - Manager requests "MDS GET ALL"
4 - Agent send response
5 - Manager update MDS
6 - Agent reports fixed data
7 - Metric values updated
8 - Manager disconnects
*/
void functional_test_operating_tc_3_5();

/**
 * TC 3.6 (Weighing scale - MDS Get - timeout)
 * 1 - Agent request association
 * 2 - Manager accepts and known conf.
 * 3 - Enter in Operating state
 * 4 - Manager request mds attributes (GET all)
 * 5 - Agent did not send response
 * 6 - Manager fire Event: Configuration Timeout
 * 7 - Check if machine state return to unassociated.
 * 8 - Manager disconnect
 */
void functional_test_operating_tc_3_6();

void functional_test_operating_tc_3_7();

#endif /* TEST_OPERATING_H_ */
