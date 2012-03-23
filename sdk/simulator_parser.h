/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file simulator_parser.h
 * \brief Simple Health Simulator parser.
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
 * \author Jalf
 * \date Feb 22, 2012
 */
#include <stdlib.h>
#include <string.h>

#include "jsmn.h"

#ifndef SIMULATOR_PARSER_H_
#define SIMULATOR_PARSER_H_

typedef enum SIMULTOR_MESSAGE {
	none = 0,
	send_connect = 1,
	send_disconnect = 2,
	send_data = 3
} SIMULTOR_MESSAGE;

/* Device definition */
typedef struct PARSER_DEVICE {
	char *type;
	char *systemID;
	char *modelNumber;
	char *modelManufacturer;
	char *prodSpecUnspecified;
	char *prodSpecSerial;
	char *prodSpecPart;
	char *prodSpecHardware;
	char *prodSpecSofware;
	char *prodSpecFirmware;
	char *prodSpecProtocol;
	char *prodSpecGMDN;
} PARSER_DEVICE;

/* A single measure definition */
typedef struct PARSER_MEASURE {
	char *type;
	char *value;
	char *unit;
	char *timeStamp;
	char *personID;
} PARSER_MEASURE;

/* Parser result */
typedef struct PARSER_RESULT {
	SIMULTOR_MESSAGE message;
	PARSER_DEVICE *device;
	int measures_count;
	PARSER_MEASURE **measures;
} PARSER_RESULT;

/* Functions */
PARSER_RESULT *parse(char *json);
void free_parser_result(PARSER_RESULT *result);

#endif
