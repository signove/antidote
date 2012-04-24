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

// #define TEST_FRAGMENTATION 1

unsigned int plugin_id = 0;

static const int TCP_ERROR = NETWORK_ERROR;
static const int TCP_ERROR_NONE = NETWORK_ERROR_NONE;
static const int BACKLOG = 1;

static int sk = -1;
static int port = 0;
static intu8 *buffer = NULL;
static int buffer_size = 0;
static int buffer_retry = 0;

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
	communication_transport_connect_indication(cid, "tcp");

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

	if (buffer_retry) {
		// there may be another APDU in buffer already
		return TCP_ERROR_NONE;
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
	ContextId cid = {plugin_id, port};

	if (sk < 0) {
		ERROR("network tcp: network_get_apdu_stream cannot found a valid sokcet");
		communication_transport_disconnect_indication(cid, "tcp");
		return NULL;
	}

	if (buffer_retry) {
		// handling letover data in buffer
		buffer_retry = 0;
	} else {
		intu8 localbuf[65535];
		int bytes_read = read(sk, localbuf, 65535);

		if (bytes_read < 0) {
			close(sk);
			free(buffer);
			buffer = 0;
			buffer_size = 0;
			communication_transport_disconnect_indication(cid, "tcp");
			DEBUG(" network:tcp error");
			sk = -1;
			return NULL;
		} else if (bytes_read == 0) {
			close(sk);
			free(buffer);
			buffer = 0;
			buffer_size = 0;
			communication_transport_disconnect_indication(cid, "tcp");
			DEBUG(" network:tcp closed");
			sk = -1;
			return NULL;
		}

		buffer = realloc(buffer, buffer_size + bytes_read);
		memcpy(buffer + buffer_size, localbuf, bytes_read);
		buffer_size += bytes_read;
	}

	if (buffer_size < 4) {
		DEBUG(" network:tcp incomplete APDU (received %d)", buffer_size);
		return NULL;
	}

	int apdu_size = (buffer[2] << 8 | buffer[3]) + 4;

	if (buffer_size < apdu_size) {
		DEBUG(" network:tcp incomplete APDU (expected %d received %d",
		      					apdu_size, buffer_size);
		return NULL;
	}

	// Create bytestream
	ByteStreamReader *stream = byte_stream_reader_instance(buffer, apdu_size);

	if (stream == NULL) {
		DEBUG(" network:tcp Error creating bytelib");
		free(buffer);
		buffer = NULL;
		buffer_size = 0;
		return NULL;
	}

	buffer = 0;
	buffer_size -= apdu_size;
	if (buffer_size > 0) {
		// leave next APDU in buffer
		buffer_retry = 1;
		buffer = malloc(buffer_size);
		memcpy(buffer, stream->buffer_cur + apdu_size, buffer_size);
	}

	DEBUG(" network:tcp APDU received ");
	ioutil_print_buffer(stream->buffer_cur, apdu_size);

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
	unsigned int written = 0;

	while (written < stream->size) {
		int to_send = stream->size - written;
#ifdef TEST_FRAGMENTATION
		to_send = to_send > 50 ? 50 : to_send;
#endif
		int ret = write(sk, stream->buffer + written, to_send);

		DEBUG(" network:tcp sent %d bytes", to_send);

		if (ret <= 0) {
			DEBUG(" network:tcp Error sending APDU.");
			return TCP_ERROR;
		}

		written += ret;
	}

	DEBUG(" network:tcp APDU sent ");
	ioutil_print_buffer(stream->buffer, stream->size);

	return TCP_ERROR_NONE;
}

/**
 * Network disconnect
 *
 * @param ctx
 * @return TCP_ERROR_NONE
 */
static int network_disconnect(Context *ctx)
{
	DEBUG("taking the initiative of disconnection");
	close(sk);
	sk = -1;

	free(buffer);
	buffer = 0;
	buffer_size = 0;
	buffer_retry = 0;

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

	free(buffer);
	buffer = 0;
	buffer_size = 0;

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
