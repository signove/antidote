/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/** main_test_console.c
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

/**
 * @addtogroup main
 * @{
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include "src/manager_p.h"
#include "src/communication/communication.h"
#include "src/communication/context_manager.h"
#include "src/api/api_definitions.h"
#include "src/api/data_encoder.h"
#include "src/api/xml_encoder.h"
#include "src/api/json_encoder.h"
#include "src/util/strbuff.h"
#include "src/communication/service.h"
#include "src/communication/plugin/plugin_tcp.h"
#include "src/communication/plugin/plugin_fifo.h"
#include "src/communication/plugin/plugin_pthread.h"

/**
 * @TODO_REVIEW_DOC
 */
/*
static DBusConnection *connection = NULL;
*/

/**
 * Context ID DEFAULT
 */
static ContextId DEFAULT_CONTEXT_ID = {1, 97};

/**
 * PLugin definition
 */
static CommunicationPlugin comm_plugin = COMMUNICATION_PLUGIN_NULL;

/**
 * New data has been received
 * @param data_list
 */
void new_data_received(Context *ctx, DataList *list)
{
	fprintf(stderr, "Medical Device System Data Updated (Context %u:%llx):\n",
			ctx->id.plugin, ctx->id.connid);

	char *data = xml_encode_data_list(list);

	if (data != NULL) {
		fprintf(stderr, "%s", data);
		fprintf(stderr, "\n");

		fflush(stderr);
		free(data);
		data = NULL;
	}
}

void device_associated(Context *ctx, DataList *list)
{
	fprintf(stderr, " Medical Device System Associated (Context %u:%llx):\n",
			ctx->id.plugin, ctx->id.connid);
}


void print_device_attributes()
{
	DataList *list = manager_get_mds_attributes(DEFAULT_CONTEXT_ID);

	char *data = json_encode_data_list(list);

	if (data != NULL) {
		fprintf(stderr, "%s", data);
		fprintf(stderr, "\n");

		fflush(stderr);
	}

	data_list_del(list);

	if (data != NULL) {
		free(data);
		data = NULL;
	}

}

static void print_commands()
{
	printf("\n"
		"	connect             Connects the IEEE manager\n"
		"	disconnect          Disconnects the IEEE manager\n"
		"	print_mds           Prints the attributes of latest received MDS\n"
		"	req_data            Requests measurement data transmission\n"
		"	get_all_mds         Gets all attributes of latest received MDS\n"
		"	get_segment_info    Gets segment information\n"
		"	data_xfer           Gets segment data\n"
		"	clear_segments      Clears all data segments\n"
		"	release             Requests an Association Release action\n"
		"	start_scan          Sets the Scanner operational state to enabled\n"
		"	stop_scan           Sets the Scanner operational state to disabled\n"
		"	help                Prints this help\n"
		"	exit                Closes application\n\n");
}

static void print_help()
{
	printf("Utility test tool to receive and print data from IEEE devices\n\n"
	       "Usage: ieee_manager_console [OPTION]\n"
	       "Options:\n"
	       "        --help                Print this help\n"
	       "        --dbus                Run DBUS mode\n"
	       "        --fifo                Run FIFO mode with default file descriptors\n"
	       "        --tcp                 Run TCP mode on default port\n");
}

static void dbus_mode()
{
	// FIXME use another D-Bus plugin
	// plugin_network_dbus_setup(&comm_plugin, 0);
}

static void fifo_mode()
{
	plugin_network_fifo_setup(&comm_plugin, DEFAULT_CONTEXT_ID, 0);
}

static void tcp_mode()
{
	int port = 6024;
	ContextId cid = {1, port};
	DEFAULT_CONTEXT_ID = cid;
	// Four concurrent Agent support
	plugin_network_tcp_setup(&comm_plugin, 4, DEFAULT_CONTEXT_ID, 6025, 6026, 6027);
}

/**
 * Main function
 * @return int
 */
int main(int argc, char **argv)
{
	size_t size;
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
			fprintf(stderr, "ERROR: %s invalid option\n", argv[1]);
			fprintf(stderr,
				"Try `ieee_manager_console --help' for more information.\n");
			exit(1);
		}

	} else if (argc > 2) {
		fprintf(stderr, "ERROR: Invalid number of options\n");
		fprintf(stderr, "Try `ieee_manager_console --help' for more information.\n");
		exit(1);
	} else {
		// Fifo is default mode
		dbus_mode();
	}

	fprintf(stderr, "\nIEEE 11073 Test application\n");

	size = 100;
	input = (char *)malloc(size);



	plugin_pthread_setup(&comm_plugin);

	CommunicationPlugin *plugins[] = {&comm_plugin, 0};
	manager_init(plugins);

	ManagerListener listener = MANAGER_LISTENER_EMPTY;
	listener.measurement_data_updated = &new_data_received;
	listener.device_available = &device_associated;

	manager_add_listener(listener);

	plugin_pthread_manager_start();

	while (1) {
		fprintf(stderr, "--> ");
		unsigned int read_bytes = getline(&input, &size, stdin);

		input[read_bytes-1] = '\0';

		if (strcmp(input, "exit") == 0) {
			break;
		} else if (strcmp(input, "start") == 0) {
			plugin_pthread_manager_start();
		} else if (strcmp(input, "stop") == 0) {
			plugin_pthread_manager_stop();
		} else if (strcmp(input, "print_mds") == 0) {
			print_device_attributes();
		}
		/*
		 * Remote Operation Invokes
		 */
		else if (strcmp(input, "req_data") == 0) {
			manager_request_measurement_data_transmission(DEFAULT_CONTEXT_ID, NULL);
		} else if (strcmp(input, "get_all_mds") == 0) {
			manager_request_get_all_mds_attributes(DEFAULT_CONTEXT_ID, NULL);
		} else if (strcmp(input, "get_segment_info") == 0) {
			manager_request_get_segment_info(DEFAULT_CONTEXT_ID, -1, NULL);
		} else if (strcmp(input, "data_xfer") == 0) {
			manager_request_get_segment_data(DEFAULT_CONTEXT_ID, -1, -1, NULL);
		} else if (strcmp(input, "clear_segments") == 0) {
			manager_request_clear_segments(DEFAULT_CONTEXT_ID, -1, NULL);
		} else if (strcmp(input, "release") == 0) {
			manager_request_association_release(DEFAULT_CONTEXT_ID);
		} else if (strcmp(input, "start_scan") == 0) {
			manager_set_operational_state_of_the_scanner(DEFAULT_CONTEXT_ID, 0X0037,
					os_enabled, NULL); // TODO Change Handle
		} else if (strcmp(input, "stop_scan") == 0) {
			manager_set_operational_state_of_the_scanner(DEFAULT_CONTEXT_ID, 0X0037,
					os_disabled, NULL); // TODO Change Handle
		} else if (strcmp(input, "help") == 0) {
			print_commands();
		} else {
			fprintf(stderr, "Command not recognized\n");
		}
	}

	plugin_pthread_manager_stop();
	manager_finalize();

	free(input);
	return 0;
}
/**
 *  @TODO_FIXME Remove this LOOP, use connection loop instead
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

void plugin_network_dbus_handle_created_connection(DBusConnection *conn)
{
	connection = conn;
}
*/



/** @} */

