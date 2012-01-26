/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_android.c
 * \brief Android JNI interface source.
 *
 * Copyright (C) 2012 Signove Tecnologia Corporation.
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
 * \author Elvis Pfutzenreuter
 * \date Jan 25, 2012
 */

/**
 * @addtogroup Communication
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/communication/plugin/plugin.h"
#include "src/communication/communication.h"
#include "src/util/log.h"
#include "plugin_android.h"

static char *current_data = NULL;
static int data_len = 0;

static int send_data(guint64 handle, unsigned char *data, int len);

static int init();
static int finalize();
static ByteStreamReader *get_apdu(struct Context *ctx);
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream);
static int force_disconnect_channel(struct Context *ctx);
static int disconnect_channel(guint64 handle);

/**
 * Callback called from Android, when device connects (BT-wise)
 *
 * @param handle
 */
static void jni_cb_channel_connected(guint64 handle)
{
	communication_transport_connect_indication(handle);
}

/**
 * Callback called from Android layer when device disconnects (BT-wise)
 *
 * @param handle
 */
static void jni_cb_channel_disconnected(guint64 handle)
{
	communication_transport_disconnect_indication(handle);
}

void plugin_android_setup(CommunicationPlugin *plugin)
{
	plugin->network_init = init;
	plugin->network_get_apdu_stream = get_apdu;
	plugin->network_send_apdu_stream = send_apdu_stream;
	plugin->network_disconnect = force_disconnect_channel;
	plugin->network_finalize = finalize;
}

/**
 * Forces closure of a channel
 */
static int disconnect_channel(guint64 handle)
{
	channel_object *c = get_channel_by_handle(handle);

	if (c) {
		DEBUG("removing channel");
		remove_channel(c->path);
		return 1;
	} else {
		DEBUG("unknown handle/channel");
		return 0;
	}
}


/**
 * Forces closure of a channel
 */
static int force_disconnect_channel(Context *c)
{
	return jni_up_disconnect_channel(c->id);
}

/**
 * Starts Health link with Android.
 *
 * @return success status
 */
static int init()
{
	return NETWORK_ERROR_NONE;
}

/**
 * Stops Android link. Link may be restarted again afterwards.
 *
 * @return success status
 */
static int finalize()
{
	return NETWORK_ERROR_NONE;
}


/**
 * Reads an APDU from buffer
 *
 * @return a byteStream with the read APDU.
 */
static ByteStreamReader *get_apdu(struct Context *ctx)
{
	guchar *buffer;
	DEBUG("\ndbus get APDU stream");

	// Create bytestream
	buffer = malloc(data_len);
	memcpy(buffer, (unsigned char *) current_data, data_len);

	ByteStreamReader *stream = byte_stream_reader_instance(buffer, data_len);

	if (stream == NULL) {
		ERROR("\n network:dbus Error creating bytelib");
		return NULL;
	}

	DEBUG(" network:dbus APDU received ");

	return stream;
}


/**
 * Socket data receiving callback
 */
static void jni_cb_data_received(unsigned long long handle, char *buf, int len)
{
	data_len = len;
	current_data = malloc(len + 1);
	memcpy(current_data, buf, len);
	current_data[len] = '\0';

	communication_read_input_stream(context_get(handle));
}


/**
 * Sends IEEE data to HDP channel
 *
 * @return success status
 */
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream)
{
	int ret = jni_up_send_data(ctx->id, stream->buffer, stream->size);

	if (ret != stream->size) {
		DEBUG("Error sending APDU. %d bytes sent. Should have sent %d.",
		      ret, stream->size);
		return NETWORK_ERROR;
	}

	return NETWORK_ERROR_NONE;
}


/** @} */
