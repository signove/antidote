/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file sample_agent_common.c
 * \brief Main application implementation.
 *
 * Copyright (C) 2012 Signove Tecnologia Corporation.
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
 * \author Elvis Pfutzenreuter
 * \date Apr 17, 2012
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include <ieee11073.h>
#include "communication/plugin/plugin_tcp_agent.h"
#include "specializations/pulse_oximeter.h"
#include "specializations/blood_pressure_monitor.h"
#include "specializations/weighing_scale.h"
#include "specializations/glucometer.h"
#include "agent.h"

intu8 AGENT_SYSTEM_ID_VALUE[] = { 0x11, 0x33, 0x55, 0x77, 0x99,
					0xbb, 0xdd, 0xff};

/**
 * Generate data for oximeter event report
 */
void *oximeter_event_report_cb()
{
	time_t now;
	struct tm nowtm;
	struct oximeter_event_report_data* data =
		malloc(sizeof(struct oximeter_event_report_data));

	time(&now);
	localtime_r(&now, &nowtm);

	data->beats = 60.5 + random() % 20;
	data->oximetry = 90.5 + random() % 10;
	data->century = nowtm.tm_year / 100 + 19;
	data->year = nowtm.tm_year % 100;
	data->month = nowtm.tm_mon + 1;
	data->day = nowtm.tm_mday;
	data->hour = nowtm.tm_hour;
	data->minute = nowtm.tm_min;
	data->second = nowtm.tm_sec;
	data->sec_fractions = 50;

	return data;
}

/**
 * Generate data for blood pressure event report
 */
void *blood_pressure_event_report_cb()
{
	time_t now;
	struct tm nowtm;
	struct blood_pressure_event_report_data* data =
		malloc(sizeof(struct blood_pressure_event_report_data));

	time(&now);
	localtime_r(&now, &nowtm);

	data->systolic = 110 + random() % 30;
	data->diastolic = 70 + random() % 20;
	data->mean = 90 + random() % 10;
	data->pulse_rate = 60 + random() % 30;

	data->century = nowtm.tm_year / 100 + 19;
	data->year = nowtm.tm_year % 100;
	data->month = nowtm.tm_mon + 1;
	data->day = nowtm.tm_mday;
	data->hour = nowtm.tm_hour;
	data->minute = nowtm.tm_min;
	data->second = nowtm.tm_sec;
	data->sec_fractions = 50;

	return data;
}

/**
 * Generate data for weight scale event report
 */
void *weightscale_event_report_cb()
{
	time_t now;
	struct tm nowtm;
	struct weightscale_event_report_data* data =
		malloc(sizeof(struct weightscale_event_report_data));

	time(&now);
	localtime_r(&now, &nowtm);

	data->weight = 70.2 + random() % 20;
	data->bmi = 20.3 + random() % 10;

	data->century = nowtm.tm_year / 100 + 19;
	data->year = nowtm.tm_year % 100;
	data->month = nowtm.tm_mon + 1;
	data->day = nowtm.tm_mday;
	data->hour = nowtm.tm_hour;
	data->minute = nowtm.tm_min;
	data->second = nowtm.tm_sec;
	data->sec_fractions = 50;

	return data;
}

/**
 * Generate data for Glucometer event report
 */
void *glucometer_event_report_cb()
{
	time_t now;
	struct tm nowtm;
	struct glucometer_event_report_data* data =
		malloc(sizeof(struct glucometer_event_report_data));

	time(&now);
	localtime_r(&now, &nowtm);

	data->capillary_whole_blood = 10.2 + random() % 20;

	data->century = nowtm.tm_year / 100 + 19;
	data->year = nowtm.tm_year % 100;
	data->month = nowtm.tm_mon + 1;
	data->day = nowtm.tm_mday;
	data->hour = nowtm.tm_hour;
	data->minute = nowtm.tm_min;
	data->second = nowtm.tm_sec;
	data->sec_fractions = 50;

	return data;
}

/**
 * Generate data for MDS
 */
struct mds_system_data *mds_data_cb()
{
	struct mds_system_data* data = malloc(sizeof(struct mds_system_data));
	memcpy(&data->system_id, AGENT_SYSTEM_ID_VALUE, 8);
	return data;
}
