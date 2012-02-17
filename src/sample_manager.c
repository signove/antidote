/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file sample_manager.c
 * \brief Main application implementation.
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
 * \author Walter Guerra
 * \date Jul 7, 2010
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>

#include <ieee11073.h>
#include "communication/plugin/plugin_fifo.h"
#include "communication/plugin/plugin_tcp.h"
#include "src/communication/service.h"

/**
 * /brief The main application is a command-line-based tool that simply receives
 * raw data from IEEE devices and print them.
 *
 */

/**
 * Port used by agent to send network data
 */
static ContextId CONTEXT_ID = {0, 0};

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

	// uncomment for manager-initiated disassociation testing
	// manager_request_association_release(CONTEXT_ID);
}

void device_reqmdsattr();

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

	device_reqmdsattr();
}

/**
 * Prints device attribute values
 */
void print_device_attributes(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	DataList *list = manager_get_mds_attributes(CONTEXT_ID);
	char *data = json_encode_data_list(list);

	fprintf(stderr, "print_device_attributes\n");

	if (data != NULL) {
		fprintf(stderr, "%s", data);
		fprintf(stderr, "\n");

		fflush(stderr);
	}

	data_list_del(list);
	free(data);
}

/**
 * Request all MDS attributes
 *
 */
void device_reqmdsattr()
{
	fprintf(stderr, "device_reqmdsattr\n");
	manager_request_get_all_mds_attributes(CONTEXT_ID, print_device_attributes);
}

/**
 * Prints utility command-line tool help.
 */
static void print_help()
{
	printf(
		"Sample IEEE 11073 manager for testing\n\n"
		"Usage: ieee_manager [OPTION]\n"
		"Options:\n"
		"        --help                Print this help\n"
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
	// NOTE we know that plugin id=1 here,
	// but might not be the case if there were many plugins!
	CONTEXT_ID.plugin = 1;
	CONTEXT_ID.connid = port;
	plugin_network_tcp_setup(&comm_plugin, 1, port);
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
	comm_plugin = communication_plugin();

	if (argc == 2) {
		if (strcmp(argv[1], "--help") == 0) {
			print_help();
			exit(0);
		} else if (strcmp(argv[1], "--tcp") == 0) {
			tcp_mode();
		} else if (strcmp(argv[1], "--fifo") == 0) {
			fifo_mode();
		} else {
			fprintf(stderr, "ERROR: invalid option: %s\n", argv[1]);
			fprintf(stderr, "Try `ieee_manager --help'"
				" for more information.\n");
			exit(1);
		}

	} else if (argc > 2) {
		fprintf(stderr, "ERROR: Invalid number of options\n");
		fprintf(stderr, "Try `ieee_manager --help'"
			" for more information.\n");
		exit(1);
	} else {
		// FIFO is default mode
		fifo_mode();
	}

	fprintf(stderr, "\nIEEE 11073 Sample application\n");

	comm_plugin.timer_count_timeout = timer_count_timeout;
	comm_plugin.timer_reset_timeout = timer_reset_timeout;

	CommunicationPlugin *comm_plugins[] = {&comm_plugin, 0};
	manager_init(comm_plugins);

	ManagerListener listener = MANAGER_LISTENER_EMPTY;
	listener.measurement_data_updated = &new_data_received;
	listener.device_available = &device_associated;

	manager_add_listener(listener);

	manager_start();

	int x = 0;
	while (x++ < 3) {
		manager_connection_loop(CONTEXT_ID);
	}

	manager_finalize();

	return 0;
}
