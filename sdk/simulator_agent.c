/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file simulator_agent.c
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
 * \author Jalf
 * \date Feb 17, 2012
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <ieee11073.h>
#include "communication/plugin/plugin_tcp_agent.h"
#include "specializations/pulse_oximeter.h"
#include "specializations/blood_pressure_monitor.h"
#include "specializations/weighing_scale.h"
#include "specializations/glucometer.h"
#include "agent.h"
#include "simulator_parser.h"
#include "../apps/sample_agent_common.h"

// HTTP stuff
#define SIMULATOR_PORT 8036
#define USERAGENT "ANTIDOTE"

// HealthSim stuff
#define EMPTY_EVENTS "{\"ConnectedDevices\":[],\"DisconnectedDevices\":[],\"Measurements\":[]}"

/**
 * Port used by agent to send network data
 */
static ContextId CONTEXT_ID = { 0, 0 };

/**
 * PLugin definition
 */
static CommunicationPlugin comm_plugin = COMMUNICATION_PLUGIN_NULL;

/** Execution state */
typedef enum STATE {
	waiting_emulator = 2,
	releasing_antidote = 11
} STATE;
static STATE state;

/* Current simulator command */
static PARSER_RESULT *simulator_command;

/* HealtSim address */
static char *host;
static char *sensor_page;
/**
 * Build a HTTP query command
 */
char *build_get_query(char *host, char *page)
{
	char *query;
	char *getpage = page;
	char *tpl = "GET /device=%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
	if (getpage[0] == '/') {
		getpage = getpage + 1;
		fprintf(stderr, "Removing leading \"/\", converting %s to %s\n", page, getpage);
	}
	// -5 is to consider the %s %s %s in tpl and the ending \0
	query = (char *) malloc(
			strlen(host) + strlen(getpage) + strlen(USERAGENT) + strlen(tpl) - 5);
	sprintf(query, tpl, getpage, host, USERAGENT);
	return query;
}

/**
 * Create a TCP socket
 */
int create_tcp_socket()
{
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "Can't create TCP socket");
		exit(1);
	}
	return sock;
}

/**
 * Check if the param is a IP number like '192.168.0.1'
 */
int is_ip_number(char *host)
{
	int i, size;
	size = strlen(host);
	for (i = 0; i < size; i++)
		if (host[i] < '0' && host[i] > '9' && host[i] != '.')
			return 0;
	return 1;
}

/**
 * Get a IP address from a hostname
 */
char *get_ip(char *host)
{
	struct hostent *hent;
	int iplen = 15; //XXX.XXX.XXX.XXX
	char *ip = (char *) malloc(iplen + 1);
	memset(ip, 0, iplen + 1);
	// Check if is a ip number
	if (is_ip_number(host)) {
		strcpy(ip, host);
		return ip;
	}
	// Resolve DNS
	if ((hent = gethostbyname(host)) == NULL) {
		fprintf(stderr, "Can't get IP:%s\n", host);
		exit(1);
	}
	if (inet_ntop(AF_INET, (void *) hent->h_addr_list[0], ip, iplen) == NULL) {
		fprintf(stderr, "Can't resolve host:%s\n", host);
		exit(1);
	}
	return ip;
}

/**
 * Execute a single call to HealthSim to get new events
 */
char *call_Emulator(char *host, char* page)
{
	char buf[65536];
	struct sockaddr_in *remote;

	int sock = create_tcp_socket();
	char *ip = get_ip(host);

	remote = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;
	int tmpres = inet_pton(AF_INET, ip, (void *) (&(remote->sin_addr.s_addr)));
	if (tmpres < 0) {
		fprintf(stderr, "Can't set remote->sin_addr.s_addr");
		exit(1);
	} else if (tmpres == 0) {
		fprintf(stderr, "%s is not a valid IP address\n", ip);
		exit(1);
	}
	remote->sin_port = htons(SIMULATOR_PORT);

	if (connect(sock, (struct sockaddr *) remote, sizeof(struct sockaddr)) < 0) {
		fprintf(stderr, "Could not connect");
		exit(1);
	}
	char *get = build_get_query(host, page);

	//Send the query to the server
	unsigned int sent = 0;
	while (sent < strlen(get)) {
		tmpres = send(sock, get + sent, strlen(get) - sent, 0);
		if (tmpres == -1) {
			fprintf(stderr, "Can't send query");
			exit(1);
		}
		sent += tmpres;
	}
	//now it is time to receive the emulator data
	memset(buf, 0, sizeof(buf));

	// Read data
	tmpres = recv(sock, buf, 65535, 0); //TODO: Check for other macro instead of BUFSIZ

	free(get);
	free(remote);
	free(ip);
	close(sock);

	// Create the result
	char *content = strstr(buf, "{");
	char *result = calloc(1, strlen(content) + 1);
	strcpy(result, content);
	return result;
}

/**
 * Callback function that is called whenever a new device
 * has associated
 *
 * @param ctx current context.
 */
void device_associated(Context *ctx)
{
	fprintf(stderr, " main: Associated\n");
	state = waiting_emulator;
	alarm(1);
}

/**
 * Callback function that is called whenever a device
 * has disassociated
 *
 * @param ctx current context.
 */
void device_unavailable(Context *ctx)
{
	fprintf(stderr, " main: Disasociated\n");
	state = waiting_emulator;
	alarm(1);
}

/**
 * Callback function that is called whenever a new device
 * has connected (but not associated)
 *
 * @param ctx current context.
 */
void device_connected(Context *ctx, const char *addr)
{
	fprintf(stderr, "main: Connected\n");
	CONTEXT_ID = ctx->id;
	state = waiting_emulator;
	alarm(1);
}

/**
 * Prints utility command-line tool help.
 */
static void show_help_info()
{
	printf("Utility tool to connect Health Simulator\n\n"
		"Usage: agent_simulator [OPTION]\n"
		"Options:\n"
		"        --help                Print this help\n"
		"        --host <address>      Emulator machine\n"
		"        --sensor <type>       {PulseOximeter, BloodPressure, WeightScale or GlucoseMeter}\n\n");
	exit(0);
}


static void show_command_error_info(char *exec)
{
	fprintf(stderr, "ERROR: invalid parameters");
	fprintf(stderr, "Try `%s --help'"
			" for more information.\n", exec);
	exit(1);
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
static void configure_tcp_plugin()
{
	int port = 6024;
	// NOTE we know that plugin id = 1 here, but
	// might not be the case!
	CONTEXT_ID.plugin = 1;
	CONTEXT_ID.connid = port;
	plugin_network_tcp_agent_setup(&comm_plugin, port);
}

/**
 * SIGALRM handler
 */
void sigalrm(int dummy)
{
	char *emulator_Data;

	fprintf(stderr, "State: %d\n", state);
	switch (state) {
	case (releasing_antidote):
		agent_disconnect(CONTEXT_ID);
	break;
	case (waiting_emulator):
		// Check if has new events in emulator
		emulator_Data = call_Emulator(host, sensor_page);
		if (emulator_Data == NULL) {
			free(emulator_Data);
			emulator_Data = NULL;
			fprintf(stderr, "ERROR: HealthSim not available.\n");
			break;
		}
		if (strcmp(emulator_Data, EMPTY_EVENTS) != 0) {
			printf("Event available:\n%s\n", emulator_Data);

			//Parse data sent by simulator
			simulator_command = parse(emulator_Data);

			// Run command
			if (simulator_command != NULL) {
				printf("Simulator command, message %d\n", simulator_command->message);
				if (simulator_command->message == send_data) {
					agent_send_data(CONTEXT_ID);
				}

				if (simulator_command->message == send_disconnect) {
					agent_request_association_release(CONTEXT_ID);
				}

				if (simulator_command->message == send_connect) {
					agent_associate(CONTEXT_ID);
				}

				free_parser_result(simulator_command);
			} else {
				printf("*** Simulator command NOT parsed\n");
			}
		}
		free(emulator_Data);
		emulator_Data = NULL;
		alarm(1);
	break;
	}
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
	void *event_report_cb = NULL;
	int specialization = 0;
	// Just for test..
	//host = "192.168.100.129";

	if (argc == 5 && strstr(argv[1], "--host") != 0 && strstr(argv[3], "--sensor") != 0) {
		host = argv[2];
		sensor_page = argv[4];
		if (strstr(argv[4], "PulseOximeter") != 0) {
			event_report_cb = oximeter_event_report_cb;
			// change to 0x0191 if you want timestamps
			specialization = 0x0190;
		} else if (strstr(argv[4], "BloodPressure") != 0) {
			event_report_cb = blood_pressure_event_report_cb;
			specialization = 0x02BC;
			AGENT_SYSTEM_ID_VALUE[7] -= 1;
		} else if (strstr(argv[4], "WeightScale") != 0) {
			event_report_cb = weightscale_event_report_cb;
			specialization = 0x05DC;
			AGENT_SYSTEM_ID_VALUE[7] -= 2;
		} else if (strstr(argv[4], "GlucoseMeter") != 0) {
			event_report_cb = glucometer_event_report_cb;
			specialization = 0x06A4;
			AGENT_SYSTEM_ID_VALUE[7] -= 3;
		} else {
			show_command_error_info(argv[0]);
		}
	} else if (argc == 2 && strstr(argv[1], "--help") != 0) {
		show_help_info();
	} else {
		show_command_error_info(argv[0]);
	}

	printf("\nHealthSim agent\n");

	// setup the TCP layer
	comm_plugin = communication_plugin();
	configure_tcp_plugin();

	// setup the plugin
	comm_plugin.timer_count_timeout = timer_count_timeout;
	comm_plugin.timer_reset_timeout = timer_reset_timeout;

	CommunicationPlugin *plugins[] = { &comm_plugin, 0 };

	agent_init(plugins, specialization, event_report_cb, mds_data_cb);
	AgentListener listener = AGENT_LISTENER_EMPTY;
	listener.device_connected = &device_connected;
	listener.device_associated = &device_associated;
	listener.device_unavailable = &device_unavailable;
	agent_add_listener(listener);

	agent_start();
	// Signals are used to control the state of the transfer data
	signal(SIGALRM, sigalrm);

	// Process the simulator events
	agent_connection_loop(CONTEXT_ID);
	agent_finalize();

	return 0;
}
