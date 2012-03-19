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

#include "src/util/strbuff.h"
#include "src/communication/communication.h"
#include "plugin_glib_socket.h"
#include "src/util/log.h"
#include "src/util/linkedlist.h"
#include "src/util/ioutil.h"
#include <stdarg.h>
#include <stdarg.h>
#include <stdlib.h>

/**
 * Plugin ID attributed by stack
 */
static unsigned int plugin_id = 0;

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
	GSocket *socket;
	GSocketConnection *connection;
	int tcp_port;

} NetworkSocket;

/**
 * List of the sockets
 */
static LinkedList *sockets = NULL;


/**
 * Search socket by port number - comparison function
 *
 * @param arg Port number
 * @param element element to compare
 * @return non-0 if matches
 */
static int search_socket_by_port(void *arg, void *element)
{
	int port = *((int *) arg);
	NetworkSocket *sk = (NetworkSocket *) element;

	if (sk == NULL) {
		return 0;
	}

	return port == sk->tcp_port;
}
/**
 * Get NetworkSocket given port number
 *
 * @param port
 * @return NetworkSocket or NULL if not found
 */
static NetworkSocket *get_socket(int port)
{
	return (NetworkSocket *) llist_search_first(sockets, &port,
			&search_socket_by_port);
}

/**
 * Services disconnection request from stack
 *
 * @param ctx Context
 * @return status
 */
static int network_force_disconnect(Context *ctx)
{
	NetworkSocket *sk = get_socket(ctx->id.connid);
	gint fd = g_socket_get_fd(sk->socket);
	close(fd);
	return TCP_ERROR_NONE;
}

/**
 * Data or condition received callback
 *
 * @param source GLib struct representing connection
 * @param condition condition received
 * @param data Related Context
 * @return TRUE if connection stays open, FALSE otherwise
 */
gboolean network_read_apdu(GIOChannel *source, GIOCondition condition,
			   gpointer data)
{
	DEBUG(" glib socket: network_read_apdu");

	Context *ctx = (Context *) data;
	NetworkSocket *sk = get_socket(ctx->id.connid);
	int fd = g_io_channel_unix_get_fd(source);

	if (condition & (G_IO_ERR | G_IO_HUP)) {
		DEBUG(" glib socket: conn closed with error");
		ContextId cid = {plugin_id, sk->tcp_port};
		communication_transport_disconnect_indication(cid);
		g_io_channel_unref(source);
		close(fd);
		g_object_unref(sk->connection);
		return FALSE;
	}

	gsize bytes_read;
	gsize max_size = 64512; // Max packet length for protocol
	gchar *buffer = (gchar *) malloc(max_size);

	bytes_read = recv(fd, buffer, max_size, 0);

	if (bytes_read <= 0) {
		DEBUG(" glib socket: connection closed read %" G_GSIZE_FORMAT "", bytes_read);
		ContextId cid = {plugin_id, sk->tcp_port};
		communication_transport_disconnect_indication(cid);
		close(fd);
		g_io_channel_unref(source);
		g_object_unref(sk->connection);
		free(buffer);
		buffer = NULL;
		return FALSE;
	}

	// Check Header size
	if (bytes_read < 4) {
		DEBUG(" glib socket: APDU header should have at least 4 bytes: %ld.",
		      (long) bytes_read);
		free(buffer);
		buffer = NULL;
		return TRUE;
	}

	int apdu_size = (buffer[2] << 8 | buffer[3]);

	// Check APDU size
	// FIXME this only works if TCP delivers atomically,
	// which is not true in real network conditions
	if (bytes_read != (apdu_size + 4)) {
		ERROR("Error reading apdu bytes");
		free(buffer);
		buffer = NULL;
		return TRUE;
	}


	// Create bytestream
	ByteStreamReader *stream = byte_stream_reader_instance((unsigned char *) buffer,
				   bytes_read);

	if (stream == NULL) {
		ERROR(" glib socket: Error creating bytelib");
		free(buffer);
		buffer = NULL;
		return TRUE;
	}

	DEBUG(" glib socket: APDU received ");
	ioutil_print_buffer(stream->buffer_cur, bytes_read);

	DEBUG(" glib socket: sending to stack...");
	communication_process_input_data(ctx, stream);

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
gboolean new_connection(GSocketService *service, GSocketConnection *connection,
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
	free(saddr);

	GSocket *socket = g_socket_connection_get_socket(connection);

	gint fd = g_socket_get_fd(socket);
	GIOChannel *channel = g_io_channel_unix_new(fd);

	ContextId cid = {plugin_id, sk->tcp_port};

	Context *ctx = communication_transport_connect_indication(cid);

	if (ctx != NULL && channel != NULL) {
		sk->socket = socket;
		sk->connection = connection;
		g_io_add_watch(channel, G_IO_IN | G_IO_HUP | G_IO_ERR,
					(GIOFunc) network_read_apdu, ctx);
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
	NetworkSocket *sk = get_socket(ctx->id.connid);

	if (sk != NULL && g_socket_is_connected(sk->socket)) {
		DEBUG(" glib socket: sending APDU...");

		GError *error = NULL;
		gsize bytes_written = g_socket_send(sk->socket,
						    (gchar *) stream->buffer, stream->size, NULL,
						    &error);

		if (bytes_written < 0) {
			g_error(" glib socket : Error writing: %s\n", error->message);
			return TCP_ERROR;
		}

		if (bytes_written != stream->size) {
			DEBUG(" glib socket: tcp Error sending APDU. %ld bytes sent. "
			      "Should have sent %d.", (long) bytes_written, stream->size);
			return TCP_ERROR;
		}

		DEBUG(" network:tcp APDU sent ");
		ioutil_print_buffer(stream->buffer, stream->size);
		return TCP_ERROR_NONE;

	}

	ERROR(" glib socket : cannot send APDU, socket is not connected");
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
	llist_destroy(sockets, &destroy_socket);
	sockets = NULL;
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

	plugin->network_init = network_init;
	plugin->network_wait_for_data = network_wait_for_data;
	plugin->network_get_apdu_stream = network_get_apdu_stream;
	plugin->network_send_apdu_stream = network_send_apdu_stream;
	plugin->network_disconnect = network_force_disconnect;
	plugin->network_finalize = network_finalize;

	return TCP_ERROR_NONE;
}

/** @} */
