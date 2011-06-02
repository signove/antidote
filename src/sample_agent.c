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
#include <dbus/dbus.h>

#include <ieee11073.h>
#include "agent.h"

// EPX FIXME EPX send data calling 

/**
 * /brief The main application is a command-line-based tool that simply receives
 * raw data from IEEE devices and print them.
 *
 */

static DBusConnection *connection = NULL;

/**
 * Port used by agent to send network data
 */
static ContextId CONTEXT_ID = 0;

/**
 * PLugin definition
 */
static CommunicationPlugin comm_plugin = COMMUNICATION_PLUGIN_NULL;

/**
 * Callback function that is called whenever a new data
 * has been received.
 *
 * @param ctx current context.
 * @param list the new list of elements.
 */
void new_data_received(Context *ctx, DataList *list)
{
	fprintf(stderr, "Medical Device Data Updated:\n");

	char *data = json_encode_data_list(list);

	if (data != NULL) {
		fprintf(stderr, "%s", data);
		fprintf(stderr, "\n");

		fflush(stderr);
		free(data);
	}
}

/**
 * Callback function that is called whenever a new device
 * has been available
 *
 * @param ctx current context.
 * @param list the new list of elements.
 */
void device_associated(Context *ctx, DataList *list)
{
	fprintf(stderr, " Medical Device System Associated:\n");

	char *data = json_encode_data_list(list);

	if (data != NULL) {
		fprintf(stderr, "%s", data);
		fprintf(stderr, "\n");

		fflush(stderr);
		free(data);
	}
}

/**
 * Prints device attribute values
 */
/*
void print_device_attributes()
{
	DataList *list = agent_get_mds_attributes(CONTEXT_ID);

	char *data = json_encode_data_list(list);

	if (data != NULL) {
		fprintf(stderr, "%s", data);
		fprintf(stderr, "\n");

		fflush(stderr);
	}

	data_list_del(list);

	free(data);
}
*/

/**
 * Prints utility command-line tool help.
 */
static void print_help()
{
	printf(
		"Utility tool to receive and print data from IEEE devices\n\n"
		"Usage: ieee_agent [OPTION]\n"
		"Options:\n"
		"        --help                Print this help\n"
		"        --dbus                Run DBUS mode\n"
		"        --fifo                Run FIFO mode with default file descriptors\n"
		"        --tcp                 Run TCP mode on default port\n");
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
 * Configure application to use dbus plugin
 */
static void dbus_mode()
{
	// TODO add a D-Bus plugin
	// plugin_network_dbus_setup(&comm_plugin, 0);
	printf("Currently, D-Bus mode is not supported in this app.\n");
	printf("Use healthd service as an example of D-Bus plug-in usage.\n");
	exit(1);
}

/**
 * Configure application to use fifo plugin
 */
static void fifo_mode()
{
	plugin_network_fifo_setup(&comm_plugin, CONTEXT_ID, 0);
}

/**
 * Configure application to use tcp plugin
 */
static void tcp_mode()
{
	int port = 6024;
	CONTEXT_ID = port;
	plugin_network_tcp_setup(&comm_plugin, 1, port);
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
	unsigned int size;
	char *input;

	comm_plugin = communication_plugin();

	if (argc == 2) {

		if (strcmp(argv[1], "--help") == 0) {
			print_help();
			exit(0);
		} else if (strcmp(argv[1], "--dbus") == 0) {
			dbus_mode();
		} else if (strcmp(argv[1], "--tcp") == 0) {
			tcp_mode();
		} else if (strcmp(argv[1], "--fifo") == 0) {
			fifo_mode();
		} else {
			fprintf(stderr, "ERROR: invalid option: %s\n", argv[1]);
			fprintf(stderr, "Try `ieee_agent --help'"
				" for more information.\n");
			exit(1);
		}

	} else if (argc > 2) {
		fprintf(stderr, "ERROR: Invalid number of options\n");
		fprintf(stderr, "Try `ieee_agent --help'"
			" for more information.\n");
		exit(1);
	} else {
		// Dbus is default mode
		dbus_mode();
	}

	fprintf(stderr, "\nIEEE 11073 Sample application\n");

	size = 100;
	input = (char *) malloc(size);

	comm_plugin.timer_count_timeout = timer_count_timeout;
	comm_plugin.timer_reset_timeout = timer_reset_timeout;
	agent_init(comm_plugin);

	AgentListener listener = AGENT_LISTENER_EMPTY;
	listener.device_available = &device_associated;

	agent_add_listener(listener);

	agent_start();

	agent_connection_loop(CONTEXT_ID);

	agent_finalize();

	free(input);
	return 0;
}

/**
 * Sets the DBus connection handle.
 *
 * @param conn the new DBus connection handle.
 */
void plugin_network_dbus_handle_created_connection(DBusConnection *conn)
{
	connection = conn;
}

/**
 * Starts "main loop" and waits for signals being emitted.
 *
 * @return NETWORK_ERROR_NONE if data is available or NETWORK_ERROR if error.
 */
/*
int plugin_network_dbus_wait_for_data()
{
	DBusMessage *msg;
	// non blocking read of the next available message
	dbus_connection_read_write(connection, 0);
	msg = dbus_connection_pop_message(connection);

	if (NULL != msg && plugin_network_dbus_read_msg(msg)) {
		return NETWORK_ERROR_NONE;
	}

	sleep(1);
	return NETWORK_ERROR;
}
*/
