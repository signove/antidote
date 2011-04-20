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
 * testencoder.h
 *
 * Created on: Jun 16, 2010
 *     Author: mateus.lima
 **********************************************************************/

#ifndef TESTENCODER_H_
#define TESTENCODER_H_

#ifdef TEST_ENABLED

void testencoder_add_suite(void);
void test_encoder_h212_apdu_encoder();
void test_encoder_h222_apdu_encoder();
void test_encoder_h232_apdu_encoder();
void test_encoder_h242_apdu_encoder();
void test_encoder_h243_apdu_encoder();
void test_encoder_data_apdu_encoder_1();
void test_encoder_data_apdu_encoder_2();
void test_encoder_data_apdu_encoder_3();

void test_enconder_byte_stream_writer();

#endif /* TEST_ENABLED */

#endif /* TESTENCODER_H_ */

