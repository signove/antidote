/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file sample_agent.c
 * \brief Main application implementation.
 *
 * Copyright (C) 2011 Signove Tecnologia Corporation.
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
 * \author Walter Guerra
 * \author Elvis Pfutzenreuter
 * \date May 31, 2011
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
#include "agent.h"
#include "sample_agent_common.h"

/**
 * /brief The main application is a command-line-based tool that simply receives
 * raw data from IEEE devices and print them.
 *
 */

/**
 * Port used by agent to send network data
 */
static ContextId CONTEXT_ID = {1, 1};

/**
 * PLugin definition
 */
static CommunicationPlugin comm_plugin = COMMUNICATION_PLUGIN_NULL;

static int alarms = 4;

/**
 * SIGALRM handler
 */
static void sigalrm(int dummy)
{
	// This is not incredibly safe, because signal may interrupt
	// processing, and is not a technique for a production agent,
	// but suffices for this quick 'n dirty sample
	
	if (alarms > 1) {
		agent_send_data(CONTEXT_ID);
		alarm(3);
	} else if (alarms == 1) {
		agent_request_association_release(CONTEXT_ID);
		alarm(3);
	} else {
		agent_disconnect(CONTEXT_ID);
	}

	--alarms;
}

/**
 * Callback function that is called whenever a new device
 * has associated
 *
 * @param ctx current context.
 */
static void device_associated(Context *ctx)
{
	fprintf(stderr, " main: Associated\n");
	alarm(3);
}

/**
 * Callback function that is called whenever a device
 * has disassociated
 *
 * @param ctx current context.
 */
static void device_unavailable(Context *ctx)
{
	fprintf(stderr, " main: Disasociated\n");
	alarms = 0;
}

/**
 * Callback function that is called whenever a new device
 * has connected (but not associated)
 *
 * @param ctx current context.
 * @param addr low-level address
 */
static void device_connected(Context *ctx, const char *addr)
{
	fprintf(stderr, "main: Connected\n");

	// ok, make it proceed with association
	// (agent has the initiative)
	agent_associate(ctx->id);
}

/**
 * Prints utility command-line tool help.
 */
static void print_help()
{
	printf(
			"Utility tool to simulate IEEE 11073 agent\n\n"
			"Usage: ieee_agent [OPTION]\n"
			"Options:\n"
			"        --help                 Print this help\n"
			"        --sensor=type          Sensor type, default pulseoximeter\n"
			"        Where type can be pulseoximeter, bloodpressure, glucometer or weightscale\n\n");
}

/**
 * Fake implementation of the reset timeout function.
 * @param ctx current context.
 */
static void timer_reset_timeout(Context *ctx)
{
}

/**
 * Waits 0 milliseconds for timeout.
 *
 * @param ctx current context.
 * @return fake timeout id
 */
static int timer_count_timeout(Context *ctx)
{
	return 1;
}

/**
 * Configure application to use tcp plugin
 */
static void tcp_mode()
{
	int port = 6024;
	// NOTE we know that plugin id = 1 here, but
	// might not be the case!
	CONTEXT_ID.plugin = 1;
	CONTEXT_ID.connid = port;
	plugin_network_tcp_agent_setup(&comm_plugin, port);
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
	int opt = 0;
	comm_plugin = communication_plugin();

	if (argc == 1) {
		opt = 1;
	} else if (argc == 2) {
                if (strstr(argv[1], "--help") != 0) {
                        print_help();
                        exit(0);
		} else if (strstr(argv[1], "pulseoximeter") != 0) {
			opt = 1;
		} else if (strstr(argv[1], "bloodpressure") != 0) {
			opt = 2;
		} else if (strstr(argv[1], "weightscale") != 0) {
			opt = 3;
		} else if (strstr(argv[1], "glucometer") != 0) {
			opt = 4;
		} else {
                        fprintf(stderr, "ERROR: invalid option: %s\n", argv[1]);
                        fprintf(stderr, "Try `%s --help'"
                                " for more information.\n", argv[0]);
                        exit(1);
		}
	} else {
		fprintf(stderr, "ERROR: invalid option: %s\n", argv[1]);
		fprintf(stderr, "Try `%s --help'"
				" for more information.\n", argv[0]);
		exit(1);
	}

	tcp_mode();

	fprintf(stderr, "\nIEEE 11073 sample agent\n");

	comm_plugin.timer_count_timeout = timer_count_timeout;
	comm_plugin.timer_reset_timeout = timer_reset_timeout;

	CommunicationPlugin *plugins[] = {&comm_plugin, 0};
	void *event_report_cb;
	int specialization;
	if (opt == 2) { /* Blood Pressure */
		fprintf(stderr, "Starting Blood Pressure Agent\n");
		event_report_cb = blood_pressure_event_report_cb;
		specialization = 0x02BC;
	} else if (opt == 3) { /* Weight Scale */
		fprintf(stderr, "Starting Weight Scale Agent\n");
		event_report_cb = weightscale_event_report_cb;
		specialization = 0x05DC;
	} else if (opt == 4) { /* Glucometer */
		fprintf(stderr, "Starting Glucometer Agent\n");
		event_report_cb = glucometer_event_report_cb;
		specialization = 0x06A4;
	} else { /* Default Pulse Oximeter */
		fprintf(stderr, "Starting Pulse Oximeter Agent\n");
		event_report_cb = oximeter_event_report_cb;
		// change to 0x0191 if you want timestamps
		specialization = 0x0190;
	}

	agent_init(plugins, specialization, event_report_cb, mds_data_cb);

	AgentListener listener = AGENT_LISTENER_EMPTY;
	listener.device_connected = &device_connected;
	listener.device_associated = &device_associated;
	listener.device_unavailable = &device_unavailable;

	agent_add_listener(listener);

	agent_start();

	signal(SIGALRM, sigalrm);
	agent_connection_loop(CONTEXT_ID);

	agent_finalize();

	return 0;
}

