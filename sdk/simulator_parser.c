/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file simulator_parser.h
 * \brief Simple Health Simulator parser implementation.
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

#include "simulator_parser.h"

/* Max tokens recognized */
#define PARSER_MAX_TOKENS	500

/* global variables */
static char *js;
static int token;
static jsmntok_t tok[PARSER_MAX_TOKENS];

/* Internal functions */
PARSER_DEVICE *parse_device();
PARSER_MEASURE *parse_measure();
char *get_attribute_value(const char *attribute);
void free_device(PARSER_DEVICE *device);
void free_measure(PARSER_MEASURE *measure);
int are_equal(jsmntok_t parserToken, const char *s2);

/* Main parser function */
PARSER_RESULT *parse(char *json)
{
	int r;
	jsmn_parser p;
	PARSER_DEVICE *device;
	PARSER_MEASURE *measure;
	PARSER_RESULT *result;
	js = json;
	jsmn_init(&p);
	r = jsmn_parse(&p, js, tok, PARSER_MAX_TOKENS);

	if (r == JSMN_SUCCESS) {
		token = 0;
		if (tok[token].type != JSMN_OBJECT)
			return NULL;  // invalid JSON
		token++;

		// Check the Connected Devices
		if (are_equal(tok[token], "ConnectedDevices")) {
			token++;
			// Expect a array
			if (tok[token].type != JSMN_ARRAY)
				return NULL;  // invalid JSON
			if (tok[token].size != 0) {
				int objectCount = tok[token].size;
				int i;
				for (i = 0; i < objectCount; i++) {
					token++;
					device = parse_device();
					if (device != NULL) {
						// Handle only the first message
						result = (PARSER_RESULT *) calloc(1,
								sizeof(PARSER_RESULT));
						result->message = send_connect;
						result->device = device;
						return result;
					}
				}
			} else
				token++;
		}

		// Check the Disconnected Devices
		if (are_equal(tok[token], "DisconnectedDevices")) {
			token++;
			// Expect a array
			if (tok[token].type != JSMN_ARRAY)
				return NULL;  // invalid JSON
			if (tok[token].size != 0) {
				int objectCount = tok[token].size;
				int i;
				for (i = 0; i < objectCount; i++) {
					token++;
					device = parse_device();
					if (device != NULL) {
						// Handle only the first message
						result = (PARSER_RESULT *) calloc(1,
								sizeof(PARSER_RESULT));
						result->message = send_disconnect;
						result->device = device;
						return result;
					}
				}
			} else
				token++;
		}

		// Check the Measurements
		if (are_equal(tok[token], "Measurements")) {
			int measureCount;
			int k;
			PARSER_MEASURE **measures;

			token++;
			// Expect a array
			if (tok[token].type != JSMN_ARRAY)
				return NULL;  // invalid JSON
			if (tok[token].size != 0) {
				token++;
				if (tok[token].type != JSMN_OBJECT)
					return NULL;  // invalid JSON

				token++;
				if (!are_equal(tok[token], "device"))
					return NULL;

				token++;
				device = parse_device();
				if (device == NULL)
					return NULL;  // invalid JSON

				if (!are_equal(tok[token], "measurements"))
					return NULL;

				token++;
				if (tok[token].type != JSMN_ARRAY)
					return NULL;  // invalid JSON
				measureCount = tok[token].size;

				measures = (PARSER_MEASURE **) calloc(measureCount,
						sizeof(PARSER_MEASURE *));
				token++;
				for (k = 0; k < measureCount; k++) {
					measure = parse_measure();
					if (measure != NULL) {
						measures[k] = measure;
					} else
						return NULL; //invalid JSON
				}

				// Build the result
				result = (PARSER_RESULT *) calloc(1, sizeof(PARSER_RESULT));
				result->device = device;
				result->message = send_data;
				result->measures_count = measureCount;
				result->measures = measures;
				return result;
			}
		}
	}
	return NULL;
}

PARSER_MEASURE *parse_measure()
{
	PARSER_MEASURE *measure;
	char *type;
	char *value;
	char *unit;
	char *timeStamp;
	char *personID;

	if (tok[token].type != JSMN_OBJECT)
		return NULL;  // invalid JSON
	token++;

	// Fill the attributes
	type = get_attribute_value("type");
	if (type == NULL)
		return NULL;

	value = get_attribute_value("value");
	if (value == NULL) {
		free(type);
		return NULL;
	}
	unit = get_attribute_value("unit");
	if (unit == NULL) {
		free(value);
		free(type);
		return NULL;
	}
	timeStamp = get_attribute_value("timeStamp");
	if (timeStamp == NULL) {
		free(unit);
		free(value);
		free(type);
		return NULL;
	}
	personID = get_attribute_value("personID");
	if (personID == NULL) {
		free(timeStamp);
		free(unit);
		free(value);
		free(type);
		return NULL;
	}

	// All attributes set. Now build the result object
	measure = (PARSER_MEASURE *) calloc(1, sizeof(PARSER_MEASURE));
	measure->personID = personID;
	measure->timeStamp = timeStamp;
	measure->type = type;
	measure->unit = unit;
	measure->value = value;

	return measure;
}

PARSER_DEVICE *parse_device()
{
	PARSER_DEVICE *device;
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

	if (tok[token].type != JSMN_OBJECT)
		return NULL;  // invalid JSON
	token++;

	// Fill the attributes
	type = get_attribute_value("type");
	if (type == NULL)
		return NULL;

	systemID = get_attribute_value("systemID");
	if (systemID == NULL) {
		free(type);
		return NULL;
	}
	modelNumber = get_attribute_value("modelNumber");
	if (modelNumber == NULL) {
		free(systemID);
		free(type);
		return NULL;
	}
	modelManufacturer = get_attribute_value("modelManufacturer");
	if (modelManufacturer == NULL) {
		free(modelNumber);
		free(systemID);
		free(type);
		return NULL;
	}
	prodSpecUnspecified = get_attribute_value("prodSpecUnspecified");
	if (prodSpecUnspecified == NULL) {
		free(modelManufacturer);
		free(modelNumber);
		free(systemID);
		free(type);
		return NULL;
	}
	prodSpecSerial = get_attribute_value("prodSpecSerial");
	if (prodSpecSerial == NULL) {
		free(prodSpecUnspecified);
		free(modelManufacturer);
		free(modelNumber);
		free(systemID);
		free(type);
		return NULL;
	}
	prodSpecPart = get_attribute_value("prodSpecPart");
	if (prodSpecPart == NULL) {
		free(prodSpecSerial);
		free(prodSpecUnspecified);
		free(modelManufacturer);
		free(modelNumber);
		free(systemID);
		free(type);
		return NULL;
	}
	prodSpecHardware = get_attribute_value("prodSpecHardware");
	if (prodSpecHardware == NULL) {
		free(prodSpecPart);
		free(prodSpecSerial);
		free(prodSpecUnspecified);
		free(modelManufacturer);
		free(modelNumber);
		free(systemID);
		free(type);
		return NULL;
	}
	prodSpecSofware = get_attribute_value("prodSpecSofware");
	if (prodSpecSofware == NULL) {
		free(prodSpecHardware);
		free(prodSpecPart);
		free(prodSpecSerial);
		free(prodSpecUnspecified);
		free(modelManufacturer);
		free(modelNumber);
		free(systemID);
		free(type);
		return NULL;
	}
	prodSpecFirmware = get_attribute_value("prodSpecFirmware");
	if (prodSpecFirmware == NULL) {
		free(prodSpecSofware);
		free(prodSpecHardware);
		free(prodSpecPart);
		free(prodSpecSerial);
		free(prodSpecUnspecified);
		free(modelManufacturer);
		free(modelNumber);
		free(systemID);
		free(type);
		return NULL;
	}
	prodSpecProtocol = get_attribute_value("prodSpecProtocol");
	if (prodSpecProtocol == NULL) {
		free(prodSpecFirmware);
		free(prodSpecSofware);
		free(prodSpecHardware);
		free(prodSpecPart);
		free(prodSpecSerial);
		free(prodSpecUnspecified);
		free(modelManufacturer);
		free(modelNumber);
		free(systemID);
		free(type);
		return NULL;
	}
	prodSpecGMDN = get_attribute_value("prodSpecGMDN");
	if (prodSpecGMDN == NULL) {
		free(prodSpecProtocol);
		free(prodSpecFirmware);
		free(prodSpecSofware);
		free(prodSpecHardware);
		free(prodSpecPart);
		free(prodSpecSerial);
		free(prodSpecUnspecified);
		free(modelManufacturer);
		free(modelNumber);
		free(systemID);
		free(type);
		return NULL;
	}
	// All attributes set. Now build the result object
	device = (PARSER_DEVICE *) calloc(1, sizeof(PARSER_DEVICE));
	device->modelManufacturer = modelManufacturer;
	device->modelNumber = modelNumber;
	device->prodSpecFirmware = prodSpecFirmware;
	device->prodSpecGMDN = prodSpecGMDN;
	device->prodSpecHardware = prodSpecHardware;
	device->prodSpecPart = prodSpecPart;
	device->prodSpecProtocol = prodSpecProtocol;
	device->prodSpecSerial = prodSpecSerial;
	device->prodSpecSofware = prodSpecSofware;
	device->prodSpecUnspecified = prodSpecUnspecified;
	device->systemID = systemID;
	device->type = type;

	return device;
}

char *get_attribute_value(const char *attribute)
{
	char *result = NULL;
	if (are_equal(tok[token], attribute)) {
		token++;
		result = (char *) calloc((tok[token].end - tok[token].start) + 1, sizeof(char));
		strncpy(result, js + tok[token].start, tok[token].end - tok[token].start);
		token++;
	}
	return result;
}

int are_equal(jsmntok_t parserToken, const char *s2)
{
	int i;
	int count = parserToken.end - parserToken.start - 1;
	char *s1 = js + parserToken.start;
	for (i = 0; i < count; i++) {
		if (*(s1 + i) != *(s2 + i))
			return 0;
	}

	return 1;
}

void free_device(PARSER_DEVICE *device)
{
	free(device->prodSpecProtocol);
	free(device->prodSpecFirmware);
	free(device->prodSpecSofware);
	free(device->prodSpecHardware);
	free(device->prodSpecPart);
	free(device->prodSpecSerial);
	free(device->prodSpecUnspecified);
	free(device->modelManufacturer);
	free(device->modelNumber);
	free(device->systemID);
	free(device->prodSpecGMDN);
	free(device->type);
	free(device);
}

void free_measure(PARSER_MEASURE *measure)
{
	free(measure->personID);
	free(measure->timeStamp);
	free(measure->type);
	free(measure->unit);
	free(measure->value);
	free(measure);
}

void free_parser_result(PARSER_RESULT *result)
{
	if (result->message == send_data) {
		int i;
		for (i = 0; i < result->measures_count; i++) {
			free_measure(result->measures[i]);
		}
		free(result->measures);
	}
	free_device(result->device);
	free(result);
}
