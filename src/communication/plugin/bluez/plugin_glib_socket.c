/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/** plugin_tcp.c
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
 * IEEE 11073 Communication Model - Finite State Machine implementation
 *
 * \author Fabricio Silva Epaminondas
 * \date Jul 07, 2010
 */

/**
 * @addtogroup GlibTcpPlugin
 * @{
 */

#include <glib.h>
#include <gio/gio.h>
#include <sys/socket.h>
#include <string.h>

#include "src/util/strbuff.h"
#include "src/communication/communication.h"
#include "plugin_glib_socket.h"
#include "src/util/log.h"
#include "src/util/linkedlist.h"
#include "src/util/ioutil.h"
#include <stdarg.h>
#include <stdarg.h>
#include <stdlib.h>

// #define TEST_FRAGMENTATION 1

/**
 * Plugin ID attributed by stack
 */
static unsigned int plugin_id = 0;

/**
 * Connection ID generator (each connection gets a new ID)
 */
static unsigned int last_conn_id = 0;

/**
 * \cond Undocumented
 */

static const int TCP_ERROR = NETWORK_ERROR;
static const int TCP_ERROR_NONE = NETWORK_ERROR_NONE;
static const int BACKLOG = 1;

/**
 * \endcond
 */

/**
 * Struct which contains network context
 */
typedef struct NetworkSocket {
	GSocketService *service;
	int tcp_port;
} NetworkSocket;

/**
 * Struct which contains connection context
 */
typedef struct Connection {
	GSocket *socket;
	GSocketConnection *connection;
	unsigned int conn_id;
	char *addr;
	unsigned char *buffer;
	int buffer_length;
} Connection;

/**
 * List of the sockets
 */
static LinkedList *sockets = NULL;

/**
 * List of connections
 */
static LinkedList *connections = NULL;


/**
 * Search connection by connid - comparison function
 *
 * @param arg Port number
 * @param element element to compare
 * @return non-0 if matches
 */
static int search_connection_by_connid(void *arg, void *element)
{
	unsigned int conn_id = *((unsigned int *) arg);
	Connection *conn = (Connection *) element;

	if (conn == NULL) {
		return 0;
	}

	return conn_id == conn->conn_id;
}

/**
 * Get Connection given connection ID
 *
 * @param conn_id the connection context id
 * @return Connection or NULL if not found
 */
static Connection *get_connection(unsigned int conn_id)
{
	return (Connection *) llist_search_first(connections, &conn_id,
			&search_connection_by_connid);
}

/**
 * Services disconnection request from stack
 *
 * @param ctx Context
 * @return status
 */
static int network_force_disconnect(Context *ctx)
{
	Connection *conn = get_connection(ctx->id.connid);
	gint fd = g_socket_get_fd(conn->socket);
	close(fd);
	return TCP_ERROR_NONE;
}

/**
 * Destroy connection structure
 *
 * @param element Pointer to Connection
 * @return 0 if failure
 */
static int destroy_connection(void *element)
{
	Connection *conn = (Connection*) element;

	if (conn != NULL) {
		DEBUG(" glib socket: connection %d closed ", conn->conn_id);
		g_object_unref(conn->connection);
		free(conn->addr);
		conn->addr = 0;
		free(conn->buffer);
		conn->buffer = 0;
		conn->buffer_length = 0;

		free(conn);
		conn = NULL;
	}

	return 1;
}

/**
 * Remove connection structure from list
 *
 * @param element Pointer to Connection
 * @return 0 if failure
 */
static int remove_connection(Connection *conn)
{
	llist_remove(connections, conn);
	destroy_connection(conn);
	return 1;
}

/**
 * Data or condition received callback
 *
 * @param source GLib struct representing connection
 * @param condition condition received
 * @param data Related Context
 * @return TRUE if connection stays open, FALSE otherwise
 */
static gboolean network_read_apdu(GIOChannel *source, GIOCondition condition,
			   gpointer data)
{
	DEBUG(" glib socket: network_read_apdu");

	unsigned int conn_id = GPOINTER_TO_UINT(data);
	Connection *conn = get_connection(conn_id);
	if (!conn)
		return FALSE;

	int fd = g_io_channel_unix_get_fd(source);

	if (condition & (G_IO_ERR | G_IO_HUP)) {
		DEBUG(" glib socket: conn closed with error");
		ContextId cid = {plugin_id, conn->conn_id};
		communication_transport_disconnect_indication(cid, conn->addr);
		g_io_channel_unref(source);
		close(fd);
		remove_connection(conn);
		return FALSE;
	}

	gsize bytes_read;
	unsigned char localbuf[64512]; // Max packet length for protocol

	bytes_read = recv(fd, localbuf, 64512, 0);

	if (bytes_read <= 0) {
		DEBUG(" glib socket: connection closed read %" G_GSIZE_FORMAT "", bytes_read);
		ContextId cid = {plugin_id, conn->conn_id};
		communication_transport_disconnect_indication(cid, conn->addr);
		close(fd);
		g_io_channel_unref(source);
		remove_connection(conn);
		return FALSE;
	}

	conn->buffer = realloc(conn->buffer, conn->buffer_length + bytes_read);
	memcpy(conn->buffer + conn->buffer_length, localbuf, bytes_read);
	conn->buffer_length += bytes_read;

	while (conn->buffer) {
		if (conn->buffer_length < 4) {
			DEBUG("Received partial APDU (read only %d)",
						conn->buffer_length);
			return TRUE;
		}

		int apdu_size = (conn->buffer[2] << 8 | conn->buffer[3]) + 4;

		// Check APDU size
		if (conn->buffer_length < apdu_size) {
			DEBUG("Received partial APDU (read %d, expected %d)",
				conn->buffer_length, apdu_size);
			return TRUE;
		}

		unsigned char *apdu_buffer = conn->buffer;
		conn->buffer = 0;
		conn->buffer_length -= apdu_size;

		if (conn->buffer_length > 0) {
			// leave the rest of next APDU in buffer
			conn->buffer = malloc(conn->buffer_length);
			memcpy(conn->buffer, apdu_buffer + apdu_size,
						conn->buffer_length);
		}

		// Create bytestream
		ByteStreamReader *stream = byte_stream_reader_instance(apdu_buffer,
									apdu_size);

		if (stream == NULL) {
			ERROR(" glib socket: Error creating stream");
			free(apdu_buffer);
			apdu_buffer = NULL;
			return TRUE;
		}

		DEBUG(" glib socket: APDU received ");
		ioutil_print_buffer(stream->buffer_cur, apdu_size);

		ContextId id = {plugin_id, conn_id};
		Context *ctx = context_get_and_lock(id);

		if (ctx) {
			DEBUG(" glib socket: sending to stack...");
			communication_process_input_data(ctx, stream);
			context_unlock(ctx);
		}
	}

	return TRUE;
}

/**
 * New TCP connection callback
 *
 * @param service Listener
 * @param connection New connection
 * @param source_object unused
 * @param user_data related NetworkSocket
 * @return TRUE
 */
static gboolean new_connection(GSocketService *service, GSocketConnection *connection,
			GObject *source_object, gpointer user_data)
{
	g_object_ref(connection);

	NetworkSocket *sk = (NetworkSocket *) user_data;

	GSocketAddress *sockaddr = g_socket_connection_get_remote_address(
					   connection, NULL);
	GInetAddress *addr = g_inet_socket_address_get_address(
				     G_INET_SOCKET_ADDRESS(sockaddr));

	char *saddr = g_inet_address_to_string(addr);
	DEBUG("New Connection from %s:%d\n", saddr, sk->tcp_port);

	GSocket *socket = g_socket_connection_get_socket(connection);

	gint fd = g_socket_get_fd(socket);
	GIOChannel *channel = g_io_channel_unix_new(fd);

	unsigned int conn_id = ++last_conn_id;
	ContextId cid = {plugin_id, conn_id};

	char *lladdr;
	if (asprintf(&lladdr, "%s:%d", saddr, conn_id) < 0) {
		free(saddr);
		g_object_unref(connection);
		g_object_unref(sockaddr);
		return FALSE;
	}

	free(saddr);
	saddr = NULL;

	Context *ctx = communication_transport_connect_indication(cid, lladdr);

	if (ctx != NULL && channel != NULL) {
		Connection *conn = calloc(1, sizeof(Connection));

		conn->socket = socket;
		conn->connection = connection;
		conn->conn_id = conn_id;
		conn->addr = lladdr;

		llist_add(connections, conn);

		g_io_add_watch(channel, G_IO_IN | G_IO_HUP | G_IO_ERR,
					(GIOFunc) network_read_apdu,
					GUINT_TO_POINTER(conn_id));
	} else {
		DEBUG("Could not add watch");
		g_object_unref(connection);
	}

	g_object_unref(sockaddr);

	return TRUE;
}

/**
 * Initialize network layer.
 * Initialize network layer, in this case opens and initializes the tcp socket.
 *
 * @param element Struct which contains network context
 * @return 1 if operation succeeds and 0 otherwise
 */
static int init_socket(void *element)
{
	NetworkSocket *sk = (NetworkSocket *) element;

	if (sk->tcp_port == 0) {
		DEBUG(" network:tcp Error: TCP port not set");
		return 0;
	}

	sk->service = g_socket_service_new();
	GInetAddress *address = g_inet_address_new_from_string("127.0.0.1");
	GSocketAddress *socket_address = g_inet_socket_address_new(address,
					 sk->tcp_port);

	g_socket_listener_add_address(G_SOCKET_LISTENER(sk->service),
				      socket_address, G_SOCKET_TYPE_STREAM,
				      G_SOCKET_PROTOCOL_TCP, NULL, NULL, NULL);

	g_object_unref(socket_address);
	g_object_unref(address);

	DEBUG("network tcp: starting socket  %d", sk->tcp_port);

	g_socket_service_start(sk->service);

	g_signal_connect(sk->service, "incoming", G_CALLBACK(new_connection), sk);

	return 1;
}

/**
 * Initialize network layer, in this case opens and initializes
 * the file descriptors
 *
 * @param Plugin id label determined by stack.
 * @return TCP_ERROR_NONE if operation succeeds
 */
static int network_init(unsigned int plugin_label)
{
	plugin_id = plugin_label;

	if (llist_iterate(sockets, init_socket)) {
		return TCP_ERROR_NONE;
	}

	return TCP_ERROR;
}

/**
 * Called by stack to block/sleep while waiting for data.
 * Not implemented in this plugin because it works in non-blocking mode.
 *
 * @param ctx Context
 * @return TCP_ERROR
 */
static int network_wait_for_data(Context *ctx)
{
	DEBUG("glib socket: network_wait_for_data function does nothing");
	return TCP_ERROR;
}

/**
 * Called by stack to fetch received APDU
 *
 * @param ctx Context
 * @return Byte stream containing APDU data
 */
static ByteStreamReader *network_get_apdu_stream(Context *ctx)
{
	DEBUG("glib socket: network_get_apdu_stream function does nothing");
	return NULL;
}

/**
 * Sends an encoded apdu
 *
 * @param ctx
 * @param stream the apdu to be sent
 * @return TCP_ERROR_NONE if data sent successfully and TCP_ERROR otherwise
 */
static int network_send_apdu_stream(Context *ctx, ByteStreamWriter *stream)
{
	Connection *conn = get_connection(ctx->id.connid);

	if (conn != NULL && g_socket_is_connected(conn->socket)) {
		DEBUG(" glib socket: sending APDU...");

		unsigned int written = 0;

		while (written < stream->size) {
			GError *error = NULL;
			int to_send = stream->size - written;
#ifdef TEST_FRAGMENTATION
			to_send = to_send > 50 ? 50 : to_send;
#endif
			DEBUG(" network:tcp sending %d bytes...", to_send);
			int ret = g_socket_send(conn->socket,
					((gchar *) stream->buffer) + written,
					to_send, NULL, &error);
			if (ret < 0) {
				DEBUG(" glib socket : Error writing: %s\n", error->message);
				g_error_free(error);
				return TCP_ERROR;
			} else if (ret == 0) {
				DEBUG(" glib socket : socket closed on write");
				return TCP_ERROR;
			}

			written += ret;
		}

		DEBUG(" network:tcp APDU sent ");
		ioutil_print_buffer(stream->buffer, stream->size);
		return TCP_ERROR_NONE;

	}

	DEBUG(" glib socket : cannot send APDU, socket is not connected");
	return TCP_ERROR;
}

/**
 * Destroy socket structure
 *
 * @param element Pointer to NetworkSocket
 * @return 0 if failure
 */
static int destroy_socket(void *element)
{
	NetworkSocket *socket = (NetworkSocket *) element;

	if (socket != NULL) {
		if (socket->service != 0) {
			DEBUG(" glib socket: Closing socket %d", socket->tcp_port);
			g_socket_service_stop(socket->service);
			// g_socket_close(socket);
			g_object_unref(socket->service);
		}

		DEBUG(" glib socket: socket %d closed ", socket->tcp_port);

		free(socket);
		socket = NULL;
	}

	return 1;

}

/**
 * Finalizes network layer and deallocated data
 *
 * @return TCP_ERROR_NONE if operation succeeds
 */
static int network_finalize()
{
	llist_destroy(connections, &destroy_connection);
	llist_destroy(sockets, &destroy_socket);
	sockets = NULL;
	connections = NULL;
	return TCP_ERROR_NONE;
}

/**
 * Create listening socket for a given port
 *
 * @param port TCP port to listen
 * @param TCP_ERROR_NONE if ok
 */
static int create_socket(int port)
{
	DEBUG("network tcp: creating socket configuration on port %d", port);

	if (sockets == NULL) {
		sockets = llist_new();
	}

	NetworkSocket *socket = calloc(1, sizeof(struct NetworkSocket));

	if (socket == NULL || !llist_add(sockets, socket)) {
		ERROR("network tcp: Cannot create socket %d", port);
		return TCP_ERROR;
	}

	socket->tcp_port = port;
	return TCP_ERROR_NONE;
}

/**
 * Initiate a CommunicationPlugin struct to use tcp connection.
 *
 * @param plugin CommunicationPlugin pointer
 * @param numberOfPorts number of socket ports
 *
 * @return TCP_ERROR if error
 */
int plugin_glib_socket_setup(CommunicationPlugin *plugin, int numberOfPorts,
			     ...)
{
	va_list port_list;
	va_start(port_list, numberOfPorts);

	DEBUG("network:tcp Initializing %d sockets", numberOfPorts);

	int port;
	int i = 0;

	for (i = 0; i < numberOfPorts; i++) {
		port = va_arg(port_list, int);

		if (create_socket(port) == TCP_ERROR) {
			return TCP_ERROR;
		}
	}

	va_end(port_list);

	connections = llist_new();

	plugin->network_init = network_init;
	plugin->network_wait_for_data = network_wait_for_data;
	plugin->network_get_apdu_stream = network_get_apdu_stream;
	plugin->network_send_apdu_stream = network_send_apdu_stream;
	plugin->network_disconnect = network_force_disconnect;
	plugin->network_finalize = network_finalize;

	return TCP_ERROR_NONE;
}

/** @} */
