/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_tcp_agent.c
 * \brief TCP plugin source.
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
 * IEEE 11073 Communication Model - Finite State Machine implementation
 *
 * \author Elvis Pfutzenreuter
 * \author Adrian Guedes
 * \author Fabricio Silva Epaminondas
 * \date Jun 28, 2011
 */

/**
 * @addtogroup AgentTcpPlugin
 * @{
 */

#include "src/util/strbuff.h"
#include "src/communication/communication.h"
#include "src/communication/plugin/plugin_tcp_agent.h"
#include "src/util/log.h"
#include "src/util/ioutil.h"
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

unsigned int plugin_id = 0;

static const int TCP_ERROR = NETWORK_ERROR;
static const int TCP_ERROR_NONE = NETWORK_ERROR_NONE;
static const int BACKLOG = 1;

static int sk = -1;
static int port = 0;

/**
 * Initialize network layer.
 * Initialize network layer, in this case opens and initializes the tcp socket.
 *
 * @return 1 if operation succeeds and 0 otherwise
 */
static int init_socket()
{
	struct sockaddr_in sa;

	int error = TCP_ERROR_NONE;

	DEBUG("network tcp: starting socket");

	bzero(&sa, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(port);

	sk = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sk < 0) {
		DEBUG(" network:tcp Error opening the tcp socket");
		return 0;
	}

	// Set the socket options
	int opt = 1; /* option is to be on/TRUE or off/FALSE */

	setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
		   sizeof(opt));

	connect(sk, &sa, sizeof(struct sockaddr_in));

	if (error < 0) {
		DEBUG(" network:tcp Error in connect");
		close(sk);
		sk = -1;
		return 0;
	}

	ContextId cid = {plugin_id, port};
	communication_transport_connect_indication(cid);

	return 1;
}

/**
 * Initialize network layer, in this case opens and initializes
 *  the file descriptors
 *
 * @return TCP_ERROR_NONE if operation succeeds
 */
static int network_init(unsigned int plugin_label)
{
	plugin_id = plugin_label;

	if (init_socket()) {
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
	DEBUG("network tcp: network_wait_for_data");

	if (sk < 0) {
		DEBUG("network tcp: network_wait_for_data error");
		return TCP_ERROR;
	}

	fd_set fds;

	int ret_value;

	while (1) {
		if (sk < 0) {
			return TCP_ERROR;
		}

		FD_ZERO(&fds);
		FD_SET(sk, &fds);

		ret_value = select(sk + 1, &fds, NULL, NULL, NULL);
		if (ret_value < 0) {
			if (errno == EINTR) {
				DEBUG(" network:fd Select failed with EINTR");
				continue;
			}
			DEBUG(" network:fd Select failed");
			return TCP_ERROR;
		} else if (ret_value == 0) {
			DEBUG(" network:fd Select timeout");
			return TCP_ERROR;
		}

		break;
	}

	return TCP_ERROR_NONE;
}

/**
 * Reads an APDU from the file descriptor
 * @param ctx
 * @return a byteStream with the read APDU or NULL if error.
 */
static ByteStreamReader *network_get_apdu_stream(Context *ctx)
{
	if (sk < 0) {
		ERROR("network tcp: network_get_apdu_stream cannot found a valid sokcet");
		return NULL;
	}

	// Needed to get buffer_cur size
	intu8 tmp_buffer[4];
	int ret = read(sk, &tmp_buffer, 4);

	ContextId cid = {plugin_id, port};

	if (ret <= -1) {
		close(sk);
		communication_transport_disconnect_indication(cid);
		DEBUG(" network:tcp error");
		sk = -1;
		return NULL;
	} else if (ret == 0) {
		close(sk);
		communication_transport_disconnect_indication(cid);
		DEBUG(" network:tcp closed");
		sk = -1;
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
	int bytes_read = read(sk, (buffer + 4), size);

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

/**
 * Sends an encoded apdu
 *
 * @param ctx
 * @param stream the apdu to be sent
 * @return TCP_ERROR_NONE if data sent successfully and TCP_ERROR otherwise
 */
static int network_send_apdu_stream(Context *ctx, ByteStreamWriter *stream)
{
	int ret = write(sk, stream->buffer, stream->size);

	if (ret != stream->size) {
		DEBUG(" network:tcp Error sending APDU. %d bytes sent. "
		      "Should have sent %d.", ret, stream->size);
		return TCP_ERROR;
	}

	DEBUG(" network:tcp APDU sent ");
	ioutil_print_buffer(stream->buffer, stream->size);

	return TCP_ERROR_NONE;
}

/**
 * Sends an encoded apdu
 *
 * @param ctx
 * @return TCP_ERROR_NONE
 */
static int network_disconnect(Context *ctx)
{
	close(sk);
	sk = -1;

	return TCP_ERROR_NONE;
}

/**
 * Finalizes network layer and deallocated data
 *
 * @return TCP_ERROR_NONE if operation succeeds
 */
static int network_finalize()
{
	close(sk);
	sk = -1;

	return TCP_ERROR_NONE;
}

static int create_socket(int pport)
{
	DEBUG("network tcp: creating socket configuration on port %d", pport);
	port = pport;
	return TCP_ERROR_NONE;
}

/**
 * Initiate a CommunicationPlugin struct to use tcp connection.
 *
 * @param plugin CommunicationPlugin pointer
 * @param pport Port of the socket
 *
 * @return TCP_ERROR if error
 */
int plugin_network_tcp_agent_setup(CommunicationPlugin *plugin, int pport)
{
	DEBUG("network:tcp Initializing agent socket");

	if (create_socket(pport) == TCP_ERROR) {
		return TCP_ERROR;
	}

	plugin->network_init = network_init;
	plugin->network_wait_for_data = network_tcp_wait_for_data;
	plugin->network_get_apdu_stream = network_get_apdu_stream;
	plugin->network_send_apdu_stream = network_send_apdu_stream;
	plugin->network_disconnect = network_disconnect;
	plugin->network_finalize = network_finalize;

	return TCP_ERROR_NONE;
}

/** @} */
