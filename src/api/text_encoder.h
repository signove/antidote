/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file text_encoder.h
 * \brief Utility methods to generate text representation.
 *
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
 * \author Fabricio Silva
 * \date Jul 2, 2010
 */

#ifndef TEXT_ENCODER_H_
#define TEXT_ENCODER_H_

#include <asn1/phd_types.h>
#include <api/api_definitions.h>

char *int2str(int value);
char *int8_2str(int8 value);
char *intu8_2str(intu8 value);
char *int16_2str(int16 value);
char *intu16_2str(intu16 value);
char *int32_2str(int32 value);
char *intu32_2str(intu32 value);
char *intu16list_2str(intu16 *list, int size);
char *float2str(float value);
char *octet_string2str(octet_string *str);
char *octet_string2hex(octet_string *str);
char *high_res_relative_time2hex(HighResRelativeTime *time);
char *bcdtime2number(intu8 value);

#endif /* TEXT_ENCODER_H_ */
