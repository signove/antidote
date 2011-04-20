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
 * test_configuring.h
 *
 * Created on: Jul 9, 2010
 *     Author: fabricio.silva
 **********************************************************************/

#ifdef TEST_ENABLED

#ifndef TEST_CONFIGURING_H_
#define TEST_CONFIGURING_H_

void functionaltest_configuring_add_suite(void);

/**
 *  TC 2.1 (Blood pressure - accept unknown configuration and release)
 * 1 - Association to enter in operational state
 * 2 - Manager requests "MDS GET ALL"
 * 3 - Agent send response
 * 4 - Manager update MDS
 * 5 - Manager disconnects
 */
void functional_test_configuring_tc_2_1();

/**
 *
 */
void functional_test_configuring_tc_2_1_1();

void functional_test_configuring_tc_2_2();

void functional_test_configuring_tc_2_3();

void functional_test_configuring_tc_2_4();

void functional_test_configuring_tc_2_5();

void functional_test_configuring_tc_2_7();


#endif /* TEST_CONFIGURING_H_ */
#endif
