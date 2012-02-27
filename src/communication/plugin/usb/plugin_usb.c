/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_usb.c
 * \brief USB interface source.
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
 * @addtogroup UsbPlugin
 */

// TODO support hotplugging (depends on future version of libusb: 1.1)

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
#include "usb_phdc_drive.h"

static unsigned int plugin_id = 0;

static char *current_data = NULL;
static int data_len = 0;

usb_phdc_context *phdc_context = NULL;
int sch_search = 0;
int sch_usb = 0;

typedef struct device_object {
	usb_phdc_device *impl;
	char *addr;
} device_object;

// Map between channel and device objects are always 1:1
// We kept this for congruence with Bluetooth plug-in only.
typedef struct channel_object {
	usb_phdc_device *impl;
	usb_phdc_device *device;
	guint64 handle;
} channel_object;

static GSList *gios = NULL;

static GSList *devices = NULL;
static GSList *channels = NULL;
static guint64 last_handle = 0;

// static GSList *apps = NULL;
static PluginUsbListener *listener = NULL;

// static int send_data(guint64 handle, unsigned char *data, int len);

static int init();
static int finalize();
static ByteStreamReader *get_apdu(struct Context *ctx);
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream);

/**
 * Callback called from USB layer, when device connects (BT-wise)
 *
 * @param handle Connection handler
 * @param device Device connected
 */
void device_connected(guint64 handle, const char *device)
{
	ContextId cid = {plugin_id, handle};

	if (listener) {
		listener->agent_connected(cid, device);
	}

	communication_transport_connect_indication(cid);
}

/**
 * Callback called from USB layer when device disconnects (BT-wise)
 *
 * @param handle
 * @param device
 */
static void device_disconnected(guint64 handle, const char *device)
{
	ContextId cid = {plugin_id, handle};

	communication_transport_disconnect_indication(cid);
	if (listener) {
		listener->agent_disconnected(cid, device);
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
static device_object *get_device_object(const usb_phdc_device *impl)
{
	GSList *i;
	device_object *m;

	for (i = devices; i; i = i->next) {
		m = i->data;

		if (m->impl == impl)
			return m;
	}

	return NULL;
}


/**
 * Remove device from device proxy list
 */
static void remove_device(const usb_phdc_device *impl)
{
	device_object *dev = get_device_object(impl);

	if (dev) {
		g_free(dev->addr);
		g_free(dev);
		devices = g_slist_remove(devices, dev);
	}
}


/**
 * Get device address
 * Returns unknonwn on failure
 */
static char *get_device_addr(const usb_phdc_device *impl)
{
	char *sn;
	// TODO relation with system-id?
	int res = asprintf(&sn, "%04x:%04x:%s", impl->vendor_id,
				impl->product_id, impl->serial_number);
	if (res < 0) {
		// TODO handle error
	}
	return sn;
}


/**
 * Add device into device proxy list
 */
static void add_device(usb_phdc_device *impl)
{
	device_object *dev;

	if (get_device_object(impl)) {
		remove_device(impl);
	}

	dev = g_new(device_object, 1);
	dev->addr = get_device_addr(impl);
	dev->impl = impl;

	devices = g_slist_prepend(devices, dev);
}


/**
 * Fetch channel struct from list
 */
static channel_object *get_channel(const usb_phdc_device *impl)
{
	GSList *i;
	channel_object *m;

	for (i = channels; i; i = i->next) {
		m = i->data;

		if (m->impl == impl)
			return m;
	}

	return NULL;

}


/**
 * Fetch channel struct from list by handle
 */
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
 * Unlisten fd and destroy GIOChannel
 */
static void unlisten_fd(int fd)
{
	GSList *i;

	for (i = gios; i; i = i->next) {
		GIOChannel *gio = i->data;
		if (g_io_channel_unix_get_fd(gio) == fd) {
			g_io_channel_unref(gio);
			gios = g_slist_remove(gios, gio);
			DEBUG("\tunlistened fd %d", fd);
			break;
		}
	}
}

/**
 * Callback to stop listening for an FD (delete GIOChannel)
 */
static void removed_fd(int fd, void *user_data)
{
	DEBUG("removed_fd %d...", fd);
	unlisten_fd(fd);
}

static void unlisten_all_fds()
{
	while (gios)
		unlisten_fd(g_io_channel_unix_get_fd(gios->data));
}

static gboolean usb_event_received(GIOChannel *gio, GIOCondition cond, gpointer dev);

/**
 * Callback to add GIO channel for fd
 */
static void added_fd(int fd, short events, void *user_data)
{
	GIOChannel *gio;

	/* Guarantee that there are no more than one listener to the same fd */
	unlisten_fd(fd);

	gio = g_io_channel_unix_new(fd);

	g_io_add_watch(gio,
		((events & POLLIN) ? G_IO_IN : 0) |
		((events & POLLOUT) ? G_IO_OUT : 0) |
		((events & POLLPRI) ? G_IO_PRI : 0) |
		((events & POLLERR) ? G_IO_ERR : 0) |
		((events & POLLHUP) ? G_IO_HUP : 0) |
		((events & POLLNVAL) ? G_IO_NVAL : 0),
		usb_event_received,
		NULL);

	gios = g_slist_prepend(gios, gio);

	DEBUG("added_fd %d %d", fd, events);
}

/**
 * Remove channel id from channel proxy list
 */
static void remove_channel(const usb_phdc_device *impl)
{
	channel_object *c = get_channel(impl);

	if (c) {
		g_free(c);
		channels = g_slist_remove(channels, c);
	}
}

static void channel_closed(usb_phdc_device *impl);

/**
 * USB callback
 */
static gboolean usb_event_received(GIOChannel *gio, GIOCondition cond, gpointer dev)
{
	phdc_device_fd_event(phdc_context);
	return TRUE;
}

static void data_received(usb_phdc_device *dev, unsigned char *buf, int len)
{
	if (len <= 0) {
		ERROR("Data length <= 0");
		return;
	}

	DEBUG("Recv: %d bytes", len);

	if (current_data) {
		free(current_data);
		current_data = NULL;
	}

	data_len = len;
	current_data = malloc(len + 1);
	memcpy(current_data, buf, len);
	current_data[len] = '\0';

	channel_object *c = get_channel(dev);

	if (!c)	{
		ERROR("Unknown channel");
		return;
	}

	ContextId cid = {plugin_id, c->handle};

	communication_read_input_stream(context_get(cid));
}

static void data_error_received(usb_phdc_device *dev)
{
	// TODO some tolerance
	DEBUG("USB device: error receiving");
	channel_closed(dev);
}

static void device_gone(usb_phdc_device *dev)
{
	DEBUG("USB device is gone");
	channel_closed(dev);
}

/**
 * Add channel into channel proxy list
 */

static guint64 add_channel(usb_phdc_device *impl, usb_phdc_device *device)
{
	channel_object *c;

	if (get_channel(impl))
		remove_channel(impl);

	c = (channel_object *) g_new(channel_object, 1);
	c->impl = impl;
	c->device = device;
	c->handle = ++last_handle;

	channels = g_slist_prepend(channels, c);

	return c->handle;
}

static int disconnect_channel(guint64 handle);

/**
 * Takes care of channel closure
 */
static void channel_closed(struct usb_phdc_device *impl)
{
	channel_object *c = get_channel(impl);
	device_object *d;

	if (!c)
		return;

	d = get_device_object(c->device);

	// notifies higher layers
	if (d) {
		device_disconnected(c->handle, d->addr);
	} else {
		ERROR("Unknown device");
		device_disconnected(c->handle, "unknown");
	}

	disconnect_channel(c->handle);
}


/**
 * Forces closure of all open channels
 */
static void disconnect_all_channels()
{
	channel_object *chan;

	while (channels) {
		chan = channels->data;
		disconnect_channel(chan->handle);
	}
}

static void channel_connected(usb_phdc_device *dev, usb_phdc_device *impl);

static void schedule(int *id, gboolean (*cb)(gpointer), gpointer context, int to)
{
	if (*id) {
		g_source_remove(*id);
	}
	*id = g_timeout_add(to, cb, context);
}

gboolean usb_alarm(gpointer dummy)
{
	phdc_device_timeout_event(phdc_context);
	return FALSE;
}

static void schedule_usb_timeout(int ms)
{
	schedule(&sch_usb, usb_alarm, NULL, ms);
}

static gboolean search_devices(gpointer dummy)
{
	int i;

	search_phdc_devices(phdc_context);

	if (phdc_context->number_of_devices <= 0) {
		DEBUG("No devices found, retrying in 5 seconds...");
		schedule(&sch_search, search_devices, NULL, 5000);
		return FALSE;
	}

	for (i = 0; i < phdc_context->number_of_devices; ++i) {
		usb_phdc_device *usbdev = &(phdc_context->device_list[i]);

		DEBUG("Opening device #%d", i);
		add_device(usbdev);

		usbdev->data_read_cb = data_received;
		usbdev->error_read_cb = data_error_received;
		usbdev->device_gone_cb = device_gone;

		print_phdc_info(usbdev);

		if (open_phdc_handle(usbdev, phdc_context) == 1) {
			channel_connected(usbdev, usbdev);
			poll_phdc_device_pre(usbdev);
		} else {
			DEBUG("Trouble opening device #%d", i);
		}
	}

	return FALSE;
}

/**
 * Starts Health link with USB
 *
 * @return success status
 */
static int init(unsigned int plugin_label)
{
	DEBUG("Starting USB...");

	plugin_id = plugin_label;
	phdc_context = (usb_phdc_context *) calloc(1, sizeof(usb_phdc_context));

	init_phdc_usb_plugin(phdc_context, added_fd, removed_fd, schedule_usb_timeout);
	schedule(&sch_search, search_devices, NULL, 0);

	return NETWORK_ERROR_NONE;
}


/**
 * Does memory cleanup after USB link stopped.
 * This is made as a separate function because main loop must hava a chance
 * to handle stopping before objects are destroyed.
 */
static gboolean cleanup(gpointer data)
{
	disconnect_all_channels();

	g_free(current_data);
	current_data = NULL;

	release_phdc_resources(phdc_context);
	unlisten_all_fds();

	return FALSE;
}


/**
 * Stops USB link. Link may be restarted again afterwards.
 *
 * @return success status
 */
static int finalize()
{
	DEBUG("Stopping USB link...");

	cleanup(NULL);
	// g_idle_add(cleanup, NULL);

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
	DEBUG("USB: get APDU stream");

	// Create bytestream
	buffer = malloc(data_len);
	memcpy(buffer, (unsigned char *) current_data, data_len);

	ByteStreamReader *stream = byte_stream_reader_instance(buffer, data_len);

	if (stream == NULL) {
		ERROR("network:usb Error creating bytelib");
		return NULL;
	}

	DEBUG(" network:usb APDU received ");

	return stream;
}

/**
 * Sends IEEE data to USB
 *
 * @return success status
 */
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream)
{
	DEBUG("Send APDU");

	channel_object *c = get_channel_by_handle(ctx->id.connid);

	if (c) {
		return usb_send_apdu(c->impl, stream->buffer, stream->size);
	} else {
		return 0;
	}
}

/**
 * Forces closure of a channel
 */
static int disconnect_channel(guint64 handle)
{
	channel_object *c = get_channel_by_handle(handle);

	if (c) {
		DEBUG("removing channel");
		remove_channel(c->impl);
		return 1;
	} else {
		DEBUG("unknown handle/channel");
		return 0;
	}
}

static void channel_connected(usb_phdc_device *device, usb_phdc_device *impl)
{
	guint64 handle = add_channel(impl, device);
	device_object *dev = get_device_object(device);

	if (dev) {
		device_connected(handle, dev->addr);
	} else {
		ERROR("Channel from unknown device");
		device_connected(handle, "unknown");
	}
}
