/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_tcp.c
 * \brief TCP plugin source.
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
 * \author Adrian Guedes
 * \author Fabricio Silva Epaminondas
 * \date Jul 05, 2010
 */

/**
 * @addtogroup TcpPlugin
 */

#include "src/util/strbuff.h"
#include "src/communication/communication.h"
#include "src/communication/plugin/plugin_tcp.h"
#include "src/util/log.h"
#include "src/util/ioutil.h"
#include "src/util/linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
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
	/**
	 * Listener socket
	 */
	int server_sk;

	/**
	 * Connection socket
	 */
	int client_sk;

	/**
	 * TCP port to listen
	 */
	int tcp_port;
	
	/**
	 * Server sockaddr
 	 */
	struct sockaddr_in server;

	/**
	 * Connected status
	 */
	int connected;
} NetworkSocket;

/**
 * List of the sockets
 */
static LinkedList *sockets = NULL;

/**
 * \cond Undocumented
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
 * \endcond
 */

/**
 * Gets a Network socket
 *
 * @param port The port of the socket
 * @return The network socket
 */
static NetworkSocket *get_socket(int port)
{
	return (NetworkSocket *) llist_search_first(sockets, &port,
			&search_socket_by_port);
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
	int error = TCP_ERROR_NONE;

	NetworkSocket *sk = (NetworkSocket *) element;

	if (sk->tcp_port == 0) {
		DEBUG(" network:tcp Error: TCP port not set");
		return 0;
	}

	DEBUG("network tcp: starting socket  %d", sk->tcp_port);

	memset(&(sk->server), 0x00, sizeof(sk->server));
	sk->server.sin_family = AF_INET;
	sk->server.sin_addr.s_addr = INADDR_ANY;
	sk->server.sin_port = htons(sk->tcp_port);

	sk->server_sk = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	error = sk->server_sk;

	if (error < 0) {
		DEBUG(" network:tcp Error opening the tcp socket");
		close(sk->server_sk);
		sk->server_sk = 0;
		return 0;
	}

	// Set the socket options
	int opt = 1; /* option is to be on/TRUE or off/FALSE */

	setsockopt(sk->server_sk, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
		   sizeof(opt));

	error = bind(sk->server_sk, (struct sockaddr *) &sk->server,
		     sizeof(struct sockaddr));

	if (error < 0) {
		DEBUG(" network:tcp Error in bind %d socket: %d", sk->server_sk, errno);
		close(sk->server_sk);
		sk->server_sk = 0;
		return 0;
	}

	error = listen(sk->server_sk, BACKLOG);

	if (error < 0) {
		DEBUG(" network:tcp Error in listen %d", sk->server_sk);
		close(sk->server_sk);
		sk->server_sk = 0;
		return 0;
	}

	ContextId cid = {plugin_id, sk->tcp_port};
	communication_transport_connect_indication(cid);

	if (error < 0) {
		DEBUG(" network:tcp Error in listen %d", sk->server_sk);
		close(sk->server_sk);
		sk->server_sk = 0;
		return 0;
	}

	return 1;
}

/**
 * Initialize network layer, in this case opens and initializes
 *  the file descriptors
 *
 * @param plugin_label the Plugin ID or label attributed by stack to this plugin
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
 * Blocks to wait data to be available from the file descriptor
 *
 * @param ctx current connection context.
 * @return TCP_ERROR_NONE if data is available or TCP_ERROR if error.
 */
static int network_tcp_wait_for_data(Context *ctx)
{
	NetworkSocket *sk = get_socket(ctx->id.connid);

	if (sk != NULL) {
		if (sk->connected == 0) {
			struct sockaddr_in client;
			socklen_t client_addr_size = sizeof(struct sockaddr_in);

			sk->client_sk = accept(sk->server_sk,
					       (struct sockaddr *) &client,
					       &client_addr_size);

			int error = TCP_ERROR_NONE;

			error = sk->client_sk;

			if (error < 0) {
				DEBUG(" network:tcp Error in accept %d", sk->server_sk);
				close(sk->client_sk);
				close(sk->server_sk);
				sk->client_sk = 0;
				sk->server_sk = 0;
				return TCP_ERROR;
			}

			sk->connected = 1;
		}

		return TCP_ERROR_NONE;
	}

	DEBUG("network tcp: network_wait_for_data unknown context");
	return TCP_ERROR;

}

/**
 * Reads an APDU from the file descriptor
 * @param ctx
 * @return a byteStream with the read APDU or NULL if error.
 */
static ByteStreamReader *network_get_apdu_stream(Context *ctx)
{
	NetworkSocket *sk = get_socket(ctx->id.connid);
	ContextId cid = {plugin_id, sk->tcp_port};

	if (sk != NULL) {
		// Needed to get buffer_cur size
		intu8 tmp_buffer[4];
		int ret = read(sk->client_sk, &tmp_buffer, 4);

		if (ret == -1) {
			sk->connected = 0;
			communication_transport_disconnect_indication(cid);
			// kludge to reinstante fixed context id used in samples
			communication_transport_connect_indication(cid);
			return NULL;
		} else if (ret == 0) {
			sk->connected = 0;
			communication_transport_disconnect_indication(cid);
			// kludge to reinstante fixed context id used in samples
			communication_transport_connect_indication(cid);
			return NULL;
		} else if (ret != 4) {
			DEBUG(" network:tcp Stream should have at least 4 bytes: %d.", ret);
			return NULL;
		}

		int size = (tmp_buffer[2] << 8 | tmp_buffer[3]);
		int buffer_size = size + 4;
		intu8 *buffer = (intu8 *) malloc(buffer_size);

		// Copy the read bytes to the final buffer_cur
		int i;

		for (i = 0; i < 4; i++) {
			buffer[i] = tmp_buffer[i];
		}

		// Read the remaining bytes
		int bytes_read = read(sk->client_sk, (buffer + 4), size);

		if (buffer_size != (bytes_read + 4)) {
			DEBUG(" network:tcp Read failed: incomplete APDU received.");
			DEBUG(" network:tcp Should have read %d bytes, but read %d bytes",
			      buffer_size, bytes_read);
			DEBUG(" network:tcp RECEIVED ");
			ioutil_print_buffer(buffer, bytes_read + 4);
			free(buffer);
			buffer = NULL;
			return NULL;
		}

		// Create bytestream
		ByteStreamReader *stream = byte_stream_reader_instance(buffer, buffer_size);

		if (stream == NULL) {
			DEBUG(" network:tcp Error creating bytelib");
			free(buffer);
			buffer = NULL;
			return NULL;
		}

		DEBUG(" network:tcp APDU received ");
		ioutil_print_buffer(stream->buffer_cur, buffer_size);

		return stream;
	}

	ERROR("network tcp: network_get_apdu_stream cannot found a valid sokcet");
	return NULL;
}

/**
 * Sends an encoded apdu
 *
 * @param ctx Context
 * @param stream the apdu to be sent
 * @return TCP_ERROR_NONE if data sent successfully and TCP_ERROR otherwise
 */
static int network_send_apdu_stream(Context *ctx, ByteStreamWriter *stream)
{
	NetworkSocket *sk = get_socket(ctx->id.connid);

	if (sk != NULL) {

		int ret = write(sk->client_sk, stream->buffer, stream->size);

		if (ret != stream->size) {
			DEBUG(" network:tcp Error sending APDU. %d bytes sent. "
			      "Should have sent %d.", ret, stream->size);
			return TCP_ERROR;
		}

		DEBUG(" network:tcp APDU sent ");
		ioutil_print_buffer(stream->buffer, stream->size);

		return TCP_ERROR_NONE;
	}

	return TCP_ERROR;
}

/**
 * Destroys a socket and its NetworkSocket descriptor
 *
 * @param element contains a NetworkSocket struct pointer
 */
static int destroy_socket(void *element)
{
	NetworkSocket *socket = (NetworkSocket *) element;

	if (socket != NULL) {
		if (socket->client_sk != 0) {
			DEBUG(" network:tcp Closing socket %d", socket->client_sk);
			close(socket->client_sk);
			socket->client_sk = 0;
		}

		if (socket->server_sk != 0) {
			DEBUG(" network:tcp Closing socket %d", socket->server_sk);
			close(socket->server_sk);
			socket->server_sk = 0;
		}

		socket->connected = 0;
		DEBUG(" network tcp: socket %d closed ", socket->tcp_port);

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
 * Creates a listener socket and NetworkSocket struct for the given port
 *
 * @param port TCP port
 * @return TCP_ERROR_NONE if ok
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
int plugin_network_tcp_setup(CommunicationPlugin *plugin, int numberOfPorts,
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
	plugin->network_wait_for_data = network_tcp_wait_for_data;
	plugin->network_get_apdu_stream = network_get_apdu_stream;
	plugin->network_send_apdu_stream = network_send_apdu_stream;
	plugin->network_finalize = network_finalize;

	return TCP_ERROR_NONE;
}
