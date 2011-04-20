/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_usb.c
 * \brief BlueZ interface source.
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
 * \author Elvis Pfutzenreuter
 * \date Mar 25, 2011
 */

/**
 * @addtogroup Communication
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <glib.h>
#include "src/communication/plugin/plugin.h"
#include "src/communication/communication.h"
#include "src/util/log.h"
#include "plugin_usb.h"

static char *current_data = NULL;
static int data_len = 0;

typedef struct device_object {
	char *path;
	char *addr;
	char *adapter;
} device_object;

typedef struct channel_object {
	char *path;
	char *device;
	int fd;
	guint64 handle;
} channel_object;

/*
static GSList *devices = NULL;
*/
static GSList *channels = NULL;
guint64 last_handle = 0;

// static GSList *apps = NULL;
static PluginUsbListener *listener = NULL;

// static int send_data(guint64 handle, unsigned char *data, int len);

static int init();
static int finalize();
static ByteStreamReader *get_apdu(struct Context *ctx);
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream);
static gboolean data_received(GIOChannel *gio, GIOCondition cond, gpointer dummy);


/**
 * Callback called from BlueZ layer, when device connects (BT-wise)
 *
 * @param handle
 * @param device
 */
void device_connected(guint64 handle, const char *device)
{
	if (listener) {
		listener->agent_connected(handle, device);
	}
	communication_transport_connect_indication(handle);
}

/**
 * Callback called from BlueZ layer when device disconnects (BT-wise)
 *
 * @param handle
 * @param device
 */
static void device_disconnected(guint64 handle, const char *device) __attribute__ ((unused)); // FIXME remove

static void device_disconnected(guint64 handle, const char *device)
{
	communication_transport_disconnect_indication(handle);
	if (listener) {
		listener->agent_disconnected(handle, device);
	}
}


void plugin_usb_setup(CommunicationPlugin *plugin)
{
	plugin->network_init = init;
	plugin->network_get_apdu_stream = get_apdu;
	plugin->network_send_apdu_stream = send_apdu_stream;
	plugin->network_finalize = finalize;
}

/**
 * Sets a listener to event of this plugin
 * @param plugin
 */
void plugin_usb_set_listener(PluginUsbListener *plugin)
{
	listener = plugin;
}

/**
 * Fetch device struct from proxy list
 */
/*
static device_object *get_device_object(const char *path)
{
	GSList *i;
	device_object *m;

	for (i = devices; i; i = i->next) {
		m = i->data;

		if (strcmp(m->path, path) == 0)
			return m;
	}

	return NULL;
}
*/


/**
 * Remove device path from device proxy list
 */
/*
static void remove_device(const char *path)
{
	device_object *dev = get_device_object(path);

	if (dev) {
		g_free(dev->path);
		g_free(dev->addr);
		g_free(dev->adapter);
		g_free(dev);
		devices = g_slist_remove(devices, dev);
	}
}
*/


/**
 * Get device address
 * Returns path on failure
 */
/*
static char *get_device_addr(const char *path)
{
	// FIXME

	return g_strdup("FIXME");
}
*/


/**
 * Add device into device proxy list
 */
static void add_device(const char *path, const char *adapter_path) __attribute__ ((unused)); // FIXME remove

static void add_device(const char *path, const char *adapter_path)
{
	/*
	device_object *dev;

	if (get_device_object(path)) {
		remove_device(path);
	}

	dev = g_new(device_object, 1);
	dev->addr = get_device_addr(path);
	dev->path = g_strdup(path);
	dev->adapter = g_strdup(adapter_path);

	devices = g_slist_prepend(devices, dev);
	*/
}


/**
 * Fetch channel struct from list
 */
static channel_object *get_channel(const char *path)
{
	GSList *i;
	channel_object *m;

	for (i = channels; i; i = i->next) {
		m = i->data;

		if (strcmp(m->path, path) == 0)
			return m;
	}

	return NULL;

}


/**
 * Fetch channel struct from list by handle
 */
static channel_object *get_channel_by_handle(guint64 handle)  __attribute__ ((unused)); // FIXME remove

static channel_object *get_channel_by_handle(guint64 handle)
{
	GSList *i;
	channel_object *m;

	for (i = channels; i; i = i->next) {
		m = i->data;

		if (m->handle == handle)
			return m;
	}

	return NULL;

}


/**
 * Fetch channel struct from list by fd
 */
static channel_object *get_channel_by_fd(int fd)
{
	GSList *i;
	channel_object *m;

	for (i = channels; i; i = i->next) {
		m = i->data;

		if (m->fd == fd && fd >= 0)
			return m;
	}

	return NULL;

}


/**
 * Remove channel path from channel proxy list
 */
static void remove_channel(const char *path)
{
	channel_object *c = get_channel(path);

	if (c) {
		g_free(c->path);
		g_free(c->device);
		close(c->fd);
		c->fd = -1;
		g_free(c);
		channels = g_slist_remove(channels, c);
	}
}

static void channel_closed(const char *path);

/**
 * Socket data receiving callback
 */
static gboolean data_received(GIOChannel *gio, GIOCondition cond, gpointer dummy)
{
	int fd = g_io_channel_unix_get_fd(gio);
	channel_object *c = get_channel_by_fd(fd);
	gboolean more = FALSE;
	char buf[65536];
	int len;

	if (!c) {
		close(fd);
		g_io_channel_unref(gio);
		return FALSE;
	}

	if (cond == G_IO_IN) {
		len = recv(fd, &buf, sizeof(buf), 0);

		if (len > 0) {
			DEBUG("Recv: fd %d, %d bytes", fd, len);

			more = TRUE;

			if (current_data) {
				free(current_data);
				current_data = NULL;
			}

			data_len = len;
			current_data = malloc(len + 1);
			memcpy(current_data, buf, len);
			current_data[len] = '\0';

			communication_read_input_stream(context_get((ContextId)c->handle));
		}
	}

	if (!more) {
		channel_closed(c->path);
		g_io_channel_unref(gio);
	}

	return more;
}


/**
 * Add channel into channel proxy list
 */
static guint64 add_channel(const char *path, const char *device, int fd)  __attribute__ ((unused)); // FIXME remove

static guint64 add_channel(const char *path, const char *device, int fd)
{
	channel_object *c;
	GIOChannel *gio;

	if (get_channel(path))
		remove_channel(path);

	/* FIXME */

	gio = g_io_channel_unix_new(fd);
	g_io_add_watch(gio, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL,
		       data_received, NULL);

	c = (channel_object *) g_new(channel_object, 1);
	c->path = g_strdup(path);
	c->device = g_strdup(device);
	c->handle = ++last_handle;
	c->fd = fd;

	channels = g_slist_prepend(channels, c);

	return c->handle;
}


/**
 * Takes care of channel closure
 */
static void channel_closed(const char *path)
{
	/* FIXME */
}


/**
 * Forces closure of all open channels
 */
static void disconnect_all_channels()  __attribute__ ((unused)); // FIXME remove

static void disconnect_all_channels()
{
	channel_object *chan;

	while (channels) {
		chan = channels->data;
		// disconnect_channel(chan->handle); FIXME
	}
}

/**
 * FIXME
 */
gboolean create_health_application(guint16 data_type)
{

	/* FIXME */

	return TRUE;
}


/**
 * Destroy health applications
 */

static void destroy_health_applications()
{
	/* FIXME */
}


/**
 * Starts Health link with BlueZ.
 *
 * @return success status
 */
static int init()
{
	DEBUG("Starting USB...");

	/* FIXME */

	return NETWORK_ERROR_NONE;
}


/**
 * Does memory cleanup after BlueZ link stopped.
 * This is made as a separate function because main loop must hava a chance
 * to handle stopping before objects are destroyed.
 */
static gboolean cleanup(gpointer data)
{
	g_free(current_data);

	current_data = NULL;

	return FALSE;
}


/**
 * Stops BlueZ link. Link may be restarted again afterwards.
 *
 * @return success status
 */
static int finalize()
{
	DEBUG("Stopping USB link...");

	destroy_health_applications();

	g_idle_add(cleanup, NULL);

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
 * Sends IEEE data to HDP channel
 *
 * @return success status
 */
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream)
{
	DEBUG("\nSend APDU");

	/*
	int ret = send_data(ctx->id, stream->buffer, stream->size);

	if (ret != stream->size) {
		DEBUG("Error sending APDU. %d bytes sent. Should have sent %d.",
		      ret, stream->size);
		return NETWORK_ERROR;
	}
	*/

	return NETWORK_ERROR_NONE;
}


/**
 * Send data to data channel
 *
 * @param handle
 * @param *data
 * @param len
 */
/*
static int send_data(guint64 handle, unsigned char *data, int len)
{
	// FIXME
	return 0;
}
*/

/** @} */
