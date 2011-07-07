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
 * testparser.h
 *
 * Created on: Jun 16, 2010
 *     Author: mateus.lima
 **********************************************************************/

#ifndef TESTPARSER_H_
#define TESTPARSER_H_

#ifdef TEST_ENABLED

void testparser_add_suite(void);
void test_parser_h211_apdu_parser();
void test_parser_h221_apdu_parser();
void test_parser_h233_apdu_parser();
void test_parser_h241_apdu_parser();
void test_parser_h244_apdu_parser();
void test_float_parser();
void test_sfloat_parser();

#endif /* TEST_ENABLED */

#endif /* TESTPARSER_H_ */

