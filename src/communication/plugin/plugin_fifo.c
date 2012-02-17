/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_fifo.c
 * \brief FIFO plugin source.
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
 * \date Jun 23, 2010
 */

/**
 * @addtogroup FifoPlugin
 * @{
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "src/communication/plugin/plugin_fifo.h"
#include "src/communication/communication.h"
#include "src/util/log.h"
#include "src/util/ioutil.h"

static unsigned int plugin_id = 0;

static const int FIFO_ERROR = NETWORK_ERROR;
static const int FIFO_ERROR_NONE = NETWORK_ERROR_NONE;

static int read_fd = 0;
static int write_fd = 0;
static int fifo_timeout = 0;
static ContextId ctx_id;

/**
 * Initialize network layer.
 * Initialize network layer, in this case opens and initializes the file descriptors.
 *
 * @return FIFO_ERROR_NONE if operation succeeds.
 */
static int network_init(unsigned int plugin_label)
{
	plugin_id = plugin_label;

	DEBUG(" network:fd Initializing network layer ");
	DEBUG(" opening file descriptors for Data I/O ");

	int type = communication_get_plugin(plugin_id)->type;

	// exchange FIFOs between manager and agent
	const char *rd = (type & MANAGER_CONTEXT) ? "read_fifo" : "write_fifo";
	const char *wr = (type & MANAGER_CONTEXT) ? "write_fifo" : "read_fifo";

	read_fd = open(rd, O_RDWR | O_NONBLOCK);

	if (read_fd <= 0) {
		DEBUG(" network:fd Error opening file");
		DEBUG("Please have read_fifo and write_fifo created.");
		return FIFO_ERROR;
	}

	write_fd = open(wr, O_RDWR | O_NONBLOCK);

	if (write_fd <= 0) {
		DEBUG(" network:fd Error opening file");
		DEBUG("Please have read_fifo and write_fifo created.");
		return FIFO_ERROR;
	}

	communication_transport_connect_indication(ctx_id);
	return FIFO_ERROR_NONE;
}

/**
 * Finalizes network layer and deallocated data.
 *
 * @return FIFO_ERROR_NONE if operation succeeds.
 */
static int network_finalize()
{
	if (write_fd > 0)
		close(write_fd);

	write_fd = 0;

	if (read_fd > 0)
		close(read_fd);

	read_fd = 0;

	fifo_timeout = 0;

	return FIFO_ERROR_NONE;
}

/**
 * Reads an APDU from the file descriptor.
 *
 * @return a byteStream with the read APDU.
 */
static ByteStreamReader *network_get_apdu_stream(Context *ctx)
{
	// Needed to get buffer_cur size
	intu8 tmp_buffer[4];
	int ret = read(read_fd, &tmp_buffer, 4);

	if (ret != 4) {
		DEBUG(" network:fd Stream should have at least 4 bytes.");
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
	int bytes_read = read(read_fd, (buffer + 4), size);

	if (buffer_size != (bytes_read + 4)) {
		DEBUG(" network:fd Read failed: incomplete APDU received.");
		DEBUG(" network:fd Should have read %d bytes, but read %d bytes",
		      buffer_size, bytes_read);
		DEBUG(" network:fd RECEIVED ");
		ioutil_print_buffer(buffer, bytes_read + 4);
		return NULL;
	}

	// Create bytestream
	ByteStreamReader *stream = byte_stream_reader_instance(buffer, buffer_size);

	if (stream == NULL) {
		DEBUG(" network:fd Error creating bytelib");
		return NULL;
	}

	DEBUG(" network:fd APDU received ");
	ioutil_print_buffer(stream->buffer_cur, buffer_size);

	return stream;
}

/**
 * Blocks to wait data to be available from the file descriptor.
 * The timeout is set at plugin_network_fifo_setup.
 *
 * @param ctx current connection context.
 * @return FIFO_ERROR_NONE if data is available or 0 if timeout.
 */
static int network_fifo_wait_for_data(Context *ctx)
{
	fd_set fds;

	struct timeval timeout;

	FD_ZERO(&fds);
	FD_SET(read_fd, &fds);

	int ret_value;

	while (1) {
		if (read_fd <= 0) {
			return FIFO_ERROR;
		}

		if (fifo_timeout == 0) {
			ret_value = select(read_fd + 1, &fds, NULL, NULL, NULL);
		} else {
			timeout.tv_sec = fifo_timeout;
			timeout.tv_usec = 0;

			ret_value = select(read_fd + 1, &fds, NULL, NULL, &timeout);
		}

		if (ret_value < 0) {
			if (errno == EINTR)
				continue;
			DEBUG(" network:fd Select failed");
			return FIFO_ERROR;
		} else if (ret_value == 0) {
			DEBUG(" network:fd Select timeout");
			return FIFO_ERROR;
		}

		break;
	}

	return FIFO_ERROR_NONE;
}

/**
 * Sends an encoded apdu
 *
 * @param stream the apdu to be sent
 * @return FIFO_ERROR_NONE if data sent successfully and 0 otherwise
 */
static int network_send_apdu_stream(Context *ctx, ByteStreamWriter *stream)
{
	int ret = write(write_fd, stream->buffer, stream->size);

	if (ret != stream->size) {
		DEBUG(
			"Error sending APDU. %d bytes sent. Should have sent %d.",
			ret, stream->size);
		return FIFO_ERROR;
	}

	DEBUG(" network:fd APDU sent ");
	ioutil_print_buffer(stream->buffer, stream->size);

	return FIFO_ERROR_NONE;
}

/**
 * Closes communication channel (used by agents)
 *
 * @param context
 * @return FIFO_ERROR_NONE if data sent successfully and 0 otherwise
 */
static int network_disconnect(Context *ctx)
{
	close(read_fd);
	close(write_fd);
	read_fd = write_fd = -1;
	communication_transport_disconnect_indication(ctx->id);
	return FIFO_ERROR_NONE;
}

/**
 * Initiate a CommunicationPlugin struct to use fifo connection.
 * @param plugin CommunicationPlugin pointer
 * @param id of context to be created
 * @param timeout Timeout.
 * @return FIFO_ERROR if error
 */
int plugin_network_fifo_setup(CommunicationPlugin *plugin, ContextId id,
				int timeout)
{
	if (read_fd != 0) {
		DEBUG("Input file already open");
		return FIFO_ERROR;
	}

	if (write_fd != 0) {
		DEBUG("Output file already open");
		return FIFO_ERROR;
	}


	fifo_timeout = timeout;
	ctx_id = id;

	plugin->network_init = network_init;
	plugin->network_wait_for_data = network_fifo_wait_for_data;
	plugin->network_get_apdu_stream = network_get_apdu_stream;
	plugin->network_send_apdu_stream = network_send_apdu_stream;
	plugin->network_disconnect = network_disconnect;
	plugin->network_finalize = network_finalize;

	return FIFO_ERROR_NONE;
}

/** @} */
