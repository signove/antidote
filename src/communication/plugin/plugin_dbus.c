/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_dbus.c
 * \brief DBus plugin source.
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
 * \author Mateus Lima
 * \author Fabricio Silva Epaminondas
 * \date Jul 14, 2010
 */

/**
 * @addtogroup Plugin
 * @{
 */

// WARNING: this plug-in is deprecated and uses a non-public MCAP API
// meant only for PTS tests by HDP and MCAP maintainers. It is kept
// here for historical and debugging reasons only.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "src/communication/communication.h"
#include "src/communication/plugin/plugin_dbus.h"
#include "src/util/log.h"
#include "src/util/ioutil.h"


static const int DBUS_ERROR = NETWORK_ERROR;
static const int DBUS_ERROR_NONE = NETWORK_ERROR_NONE;

static DBusConnection *conn = NULL;
static char *default_adapter = NULL;

static dbus_uint64_t mdl_handle = 0;
static char *current_data = NULL;
static int data_len = 0;

static const intu16 mcap_control_psm = 0x1001;
static const intu16 mcap_data_psm = 0x1003;

static int dbus_timeout = 0;

static char *get_default_adapter_path(DBusConnection *conn);
static int start_session(DBusConnection *conn, char *default_adapter_path,
			 int cpsm, int dpsm);
static int add_hdp_record(DBusConnection *connection, char *default_adapter_path);
static int send_data(unsigned char *data, int len);
static int network_dbus_wait_for_data(Context *ctx);

/**
 * Read data from XML file.
 * @return char*
 */
static char *get_xml_data()
{
	FILE *file;
	char *buffer;
	unsigned long fileLen;

	file = fopen("src/resources/health_record.xml", "rb");

	if (!file) {
		ERROR(" network:dbus can't open file %s", "src/resources/health_record.xml");
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	fileLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = (char *) malloc(fileLen + 1);

	if (!buffer) {
		ERROR(" network:dbus Memory error!");
		fclose(file);
		exit(1);
	}

	fread(buffer, fileLen, 1, file);
	buffer[fileLen] = '\0';

	fclose(file);

	return buffer;
}

/**
 * Blocks to wait data to be available from the network dbus.
 * The timeout is set at plugin_network_dbus_setup.
 *
 * @param ctx current connection context.
 * @return NETWORK_ERROR_NONE if data is available or NETWORK_ERROR if error.
 */
static int network_dbus_wait_for_data(Context *ctx)
{
	return plugin_network_dbus_wait_for_data();
}

/**
 * Notifies 'connection created' event.
 * Notifies to application that a connection was created.
 *
 * @param conn the connection created.
 */
static void notify_created_dbus_connection(DBusConnection *conn)
{
	plugin_network_dbus_handle_created_connection(conn);
}

/**
 * Initialize network layer.
 * Initialize network layer, in this case opens and initializes the dbus connection.
 *
 * @return DBUS_ERROR_NONE if operation succeeds.
 */
static int network_init()
{
	DBusError err;
	int record_handle, session_handle;

	DEBUG(" network:dbus Listening for signals");
	fflush(stdout);

	// initialise the errors
	dbus_error_init(&err);

	// connect to the bus and check for errors
	conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);

	if (dbus_error_is_set(&err)) {
		ERROR(" network:dbus Connection Error (%s)", err.message);
		dbus_error_free(&err);
	}

	if (NULL == conn) {
		exit(1);
	}

	// add a rule for which messages we want to see
	dbus_bus_add_match(conn, "type='signal',interface='org.bluez.mcap'", &err); // see signals from the given interface
	dbus_connection_flush(conn);

	if (dbus_error_is_set(&err)) {
		ERROR(" network:dbus Match Error (%s)", err.message);
		dbus_error_free(&err);
		exit(1);
	}

	DEBUG(" network:dbus Match rule sent");

	default_adapter = get_default_adapter_path(conn);

	if (default_adapter == NULL) {
		ERROR("Error on DefaultAdapter");
		exit(1);
	}

	record_handle = add_hdp_record(conn, default_adapter);

	if (record_handle == 0) {
		ERROR("Error on AddAdapter. Handle value %u", record_handle);
		exit(1);
	}

	// ------> Start connect procedure from here <--------------
	session_handle = start_session(conn, default_adapter, mcap_control_psm,
				       mcap_data_psm);

	if (session_handle == 0) {
		ERROR("Error on StartSession. Handle value %u",
		      session_handle);
		exit(1);
	}

	notify_created_dbus_connection(conn);
	communication_transport_connect_indication(mdl_handle);
	return DBUS_ERROR_NONE;
}

/**
 * Finalizes network layer and deallocated data.
 *
 * @return FIFO_ERROR_NONE if operation succeeds.
 */
static int network_finalize()
{
	// TODO: network_dbus_finalize
	if (!dbus_connection_get_is_connected(conn)) {
		dbus_connection_unref(conn);
	}

	if (default_adapter) {
		free(default_adapter);
		default_adapter = NULL;
	}

	return DBUS_ERROR_NONE;
}

/**
 * Reads an APDU from the file descriptor.
 *
 * @return a byteStream with the read APDU.
 */
static ByteStreamReader *network_get_apdu_stream(Context *ctx)
{
	// Create bytestream
	intu8 *buffer = (intu8 *) malloc(data_len);
	memcpy(buffer, (unsigned char *)current_data, data_len);

	ByteStreamReader *stream = byte_stream_reader_instance(buffer, data_len);

	if (stream == NULL) {
		DEBUG(" network:dbus Error creating bytelib");
		return NULL;
	}

	DEBUG(" network:dbus APDU received ");
	ioutil_print_buffer(stream->buffer_cur, data_len);

	return stream;
}

/**
 * Sends an encoded apdu
 *
 * @param ctx
 * @param stream the apdu to be sent
 * @return FIFO_ERROR_NONE if data sent successfully and 0 otherwise
 */
static int network_send_apdu_stream(Context *ctx, ByteStreamWriter *stream)
{

	int ret = send_data(stream->buffer, stream->size);

	if (ret != stream->size) {
		DEBUG("Error sending APDU. %d bytes sent. Should have sent %d.",
		      ret, stream->size);
		return DBUS_ERROR;
	}

	DEBUG(" network:dbus APDU sent ");
	ioutil_print_buffer(stream->buffer, stream->size);

	return DBUS_ERROR_NONE;
}

/**
 * Send data through DBus
 *
 * @param *data
 * @param len
 */
static int send_data(unsigned char *data, int len)
{
	DBusMessage *msg, *reply;
	DBusMessageIter args, arrayIter;
	DBusError err;
	int sent = 0;

	msg = dbus_message_new_method_call("org.bluez", default_adapter,
					   "org.bluez.mcap", "Send");

	if (!msg) {
		ERROR("Can't allocate new method call");
		return -1;
	}

	dbus_message_iter_init_append(msg, &args);

	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT64, &mdl_handle)) {
		ERROR("Out Of Memory!");
		return -1;
	}

	if (!dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "y", &arrayIter)) {
		ERROR("Out Of Memory!");
		return -1;
	}

	if (!dbus_message_iter_append_fixed_array(&arrayIter, DBUS_TYPE_BYTE, &data, len)) {
		ERROR("No memory!");
		return -1;
	}

	dbus_message_iter_close_container(&args, &arrayIter);
	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

	dbus_message_unref(msg);

	if (!reply) {
		ERROR("Can't send data");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
		}

		return -1;
	}

	if (!dbus_message_get_args(reply, &err, DBUS_TYPE_INT32, &sent,
				   DBUS_TYPE_INVALID)) {
		ERROR("Can't get reply arguments");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
		}

		return -1;
	}

	dbus_message_unref(reply);

	dbus_connection_flush(conn);

	return sent;
}

/**
 * Initiate a CommunicationPlugin struct to use dbus connection.
 * @param plugin CommunicationPlugin pointer
 * @param timeout Timeout.
 * @return DBUS_ERROR if error
 */
int plugin_network_dbus_setup(CommunicationPlugin *plugin, int timeout)
{
	dbus_timeout = timeout;
	plugin->network_init = network_init;
	plugin->network_wait_for_data = network_dbus_wait_for_data;
	plugin->network_get_apdu_stream = network_get_apdu_stream;
	plugin->network_send_apdu_stream = network_send_apdu_stream;
	plugin->network_finalize = network_finalize;

	return DBUS_ERROR_NONE;
}

/**
 * Gets the default adapter (org.bluez)
 *
 * @param *conn
 */
static char *get_default_adapter_path(DBusConnection *conn)
{
	DBusMessage *msg, *reply;
	DBusError err;
	const char *reply_path;
	char *path;

	msg = dbus_message_new_method_call("org.bluez", "/", "org.bluez.Manager",
					   "DefaultAdapter");

	if (!msg) {
		ERROR(" network:dbus Can't allocate new method call");
		return NULL;
	}

	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

	dbus_message_unref(msg);

	if (!reply) {
		ERROR(" network:dbus Can't get default adapter");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
		}

		return NULL;
	}

	if (!dbus_message_get_args(reply, &err, DBUS_TYPE_OBJECT_PATH, &reply_path,
				   DBUS_TYPE_INVALID)) {
		ERROR(" network:dbus Can't get reply arguments");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
		}

		return NULL;
	}

	path = strdup(reply_path);

	dbus_message_unref(reply);

	dbus_connection_flush(conn);

	return path;
}

/**
 * Add HDP record to DBus session
 *
 * @param *connection
 * @param *default_adapter_path
 */
static int add_hdp_record(DBusConnection *connection, char *default_adapter_path)
{
	DBusMessage *msg = NULL;
	DBusMessage *reply = NULL;
	DBusError err;
	int handle;
	char *xml_data = NULL;

	msg = dbus_message_new_method_call("org.bluez", default_adapter_path,
					   "org.bluez.Service", "AddRecord");

	if (!msg) {
		ERROR(" network:dbus Can't allocate new method call");
		return 0;
	}

	xml_data = get_xml_data();

	dbus_message_append_args(msg, DBUS_TYPE_STRING, &xml_data,
				 DBUS_TYPE_INVALID);

	free(xml_data);
	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(connection, msg, -1, &err);

	dbus_message_unref(msg);

	if (!reply) {
		ERROR(" network:dbus Can't add record");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
		}

		return 0;
	}

	if (!dbus_message_get_args(reply, &err, DBUS_TYPE_UINT32, &handle,
				   DBUS_TYPE_INVALID)) {
		ERROR(" network:dbus Can't get reply arguments");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
		}

		return 0;
	}


	dbus_message_unref(reply);

	dbus_connection_flush(connection);

	return handle;
}

/**
 * Start DBus session
 *
 * @param *conn
 * @param *default_adapter_path
 * @param cpsm
 * @param dpsm
 *
 * @return Handle session
 */
static int start_session(DBusConnection *conn, char *default_adapter_path,
			 int cpsm, int dpsm)
{
	DBusMessage *msg, *reply;
	DBusError err;
	int handle;

	msg = dbus_message_new_method_call("org.bluez", default_adapter_path,
					   "org.bluez.mcap", "StartSession");

	if (!msg) {
		ERROR(" network:dbus Can't allocate new method call");
		return 0;
	}

	dbus_message_append_args(msg, DBUS_TYPE_UINT16, &cpsm, DBUS_TYPE_UINT16,
				 &dpsm, DBUS_TYPE_INVALID);

	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

	dbus_message_unref(msg);

	if (!reply) {
		ERROR(" network:dbus Can't start session");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
		}

		return 0;
	}

	if (!dbus_message_get_args(reply, &err, DBUS_TYPE_UINT64, &handle,
				   DBUS_TYPE_INVALID)) {
		ERROR(" network:dbus Can't get reply arguments");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
		}

		return 0;
	}

	dbus_message_unref(reply);

	dbus_connection_flush(conn);

	return handle;
}

/**
 *  Reads a message from Dbus.
 *
 *  @param msg message to be read
 *  @return TRUE, if the message is properly read; FALSE, otherwise.
 */
int plugin_network_dbus_read_msg(DBusMessage *msg)
{
	int do_break = FALSE;
	DBusMessageIter args, array_args;

	// check if the message is a signal from the correct interface and with the correct name
	if (dbus_message_is_signal(msg, "org.bluez.mcap", "Recv")) {

		// read the parameters
		if (!dbus_message_iter_init(msg, &args)) {
			DEBUG("Message Has No Parameters");
		} else {
			if (DBUS_TYPE_UINT64 == dbus_message_iter_get_arg_type(&args)) {
				dbus_message_iter_get_basic(&args, &mdl_handle);
				dbus_message_iter_next(&args);
			}

			if ((DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&args))
			    && (DBUS_TYPE_BYTE
				== dbus_message_iter_get_element_type(&args))) {
				dbus_message_iter_recurse(&args, &array_args);
				dbus_message_iter_get_fixed_array(&array_args, &current_data,
								  &data_len);
			}

			do_break = TRUE;
			dbus_message_unref(msg);
			return do_break;
		}
	}

	// free the message
	dbus_message_unref(msg);
	return do_break;
}


/** @} */
