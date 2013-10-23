/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_bluez.c
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
 * \author Mateus Lima
 * \date Jul 14, 2010
 */

/**
 * @addtogroup BlueZPlugin
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "src/communication/plugin/plugin.h"
#include "src/communication/communication.h"
#include "src/util/linkedlist.h"
#include "src/util/log.h"
#include "plugin_bluez.h"

/**
 * Plugin ID attributed by stack
 */
static unsigned int plugin_id = 0;

/**
 * \cond Undocumented
 */

static DBusGConnection *conn = NULL;
static DBusGProxy *manager = NULL;

static char *current_data = NULL;
static int data_len = 0;

typedef struct device_object {
	char *path;
	char *addr;
	char *adapter;
	DBusGProxy *proxy;
	int channels;
	guint64 first_handle;
} device_object;

typedef struct channel_object {
	char *path;
	char *device;
	DBusGProxy *proxy;
	int first;
	int fd;
	int initiated_by_us;
	guint64 handle;
} channel_object;

typedef struct adapter_object {
	char *path;
	DBusGProxy *proxy;
} adapter_object;

typedef struct app_object {
	char *path;
	guint16 data_type;
	gboolean is_sink;
} app_object;


static LinkedList *_apps = NULL;
static LinkedList *_adapters = NULL;
static LinkedList *_devices = NULL;
static LinkedList *_channels = NULL;
static guint64 last_handle = 0;

static LinkedList *apps()
{
	if (! _apps) {
		_apps = llist_new();
	}
	return _apps;
}

static LinkedList *adapters()
{
	if (! _adapters){
		_adapters = llist_new();
	}
	return _adapters;
}

static LinkedList *devices()
{
	if (! _devices) {
		_devices = llist_new();
	}
	return _devices;
}

static LinkedList *channels()
{
	if (! _channels){
		_channels = llist_new();
	}
	return _channels;
}


/**
 * \endcond
 */

static int send_data(uint64_t connid, unsigned char *data, int len);

static int init(unsigned int plugin_label);
static int finalize();
static ByteStreamReader *get_apdu(struct Context *ctx);
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream);
static gboolean data_received(GIOChannel *gio, GIOCondition cond, gpointer dummy);
static int force_disconnect_channel(struct Context *ctx);
static int disconnect_channel(guint64 handle, int passive);

/**
 * Callback called from BlueZ layer, when device connects (BT-wise)
 *
 * @param handle
 * @param device
 */
static void device_connected(guint64 handle, const char *device)
{
	ContextId cid = {plugin_id, handle};
	communication_transport_connect_indication(cid, device);
}

/**
 * Callback called from BlueZ layer when device disconnects (BT-wise)
 *
 * @param handle
 * @param device
 */
static void device_disconnected(guint64 handle, const char *device)
{
	ContextId cid = {plugin_id, handle};
	communication_transport_disconnect_indication(cid, device);
}

/**
 * Setup BlueZ plugin
 *
 * @param plugin the Plugin descriptor to be filled in
 */
void plugin_bluez_setup(CommunicationPlugin *plugin)
{
	plugin->network_init = init;
	plugin->network_get_apdu_stream = get_apdu;
	plugin->network_send_apdu_stream = send_apdu_stream;
	plugin->network_disconnect = force_disconnect_channel;
	plugin->network_finalize = finalize;
}

static int cmp_device_object(void *arg, void *nodeElement)
{
	const char *path = (char *) arg;
	device_object *m = (device_object *) nodeElement;

	if (strcmp(m->path, path) == 0)
		return 1;
	return 0;
}

/**
 * Fetch device struct from proxy list
 */
static device_object *get_device_object(const char *path)
{
	return (device_object *) llist_search_first(devices(), (void *) path, cmp_device_object);
}

static int cmp_object_by_addr(void *arg, void *nodeElement)
{
	const char *bdaddr = (char *) arg;
	device_object *m = (device_object *) nodeElement;

	if (strcmp(m->addr, bdaddr) == 0)
		return 1;
	return 0;
}

/**
 * Fetch device struct from proxy list, by addr
 */
static device_object *get_device_object_by_addr(const char *bdaddr)
{
	return (device_object *) llist_search_first(devices(), (void *) bdaddr, cmp_object_by_addr);
}

/**
 * Fetch device from proxy list
 */
static DBusGProxy *get_device(const char *path)
{
	device_object *dev = get_device_object(path);

	if (dev)
		return dev->proxy;

	return NULL;
}


/**
 * Remove device path from device proxy list
 */
static void remove_device(const char *path)
{
	device_object *dev = get_device_object(path);

	if (dev) {
		g_free(dev->path);
		g_free(dev->addr);
		g_free(dev->adapter);
		g_object_unref(dev->proxy);
		g_free(dev);
		llist_remove(devices(), dev);
	}
}


/**
 * Get device address by calling BlueZ on device path
 * Returns path on failure
 */
static char *get_device_addr(const char *path)
{
	DBusGProxy *proxy;
	char *addr;
	const char *caddr;
	GError *error = NULL;
	GValue *gaddr;
	GHashTable *props;

	proxy = dbus_g_proxy_new_for_name(conn, "org.bluez", path,
					  "org.bluez.Device");

	if (!proxy) {
		ERROR("Can't get org.bluez.Device interface");
		return g_strdup(path);
	}


	if (!dbus_g_proxy_call(proxy, "GetProperties",
			       &error,
			       G_TYPE_INVALID,
			       dbus_g_type_get_map("GHashTable", G_TYPE_STRING, G_TYPE_VALUE),
			       &props, G_TYPE_INVALID)) {
		if (error) {
			ERROR("Can't call device GetProperties: %s", error->message);
			g_error_free(error);
		} else {
			ERROR("Can't call device GetProperties, probably disconnected");
		}

		return g_strdup(path);
	}

	gaddr = g_hash_table_lookup(props, "Address");

	if (gaddr) {
		caddr = g_value_get_string(gaddr);

		if (caddr) {
			DEBUG("\tDevice address is %s", caddr);
		} else {
			ERROR("Address property is not a string");
			caddr = path;
		}
	} else {
		ERROR("No Address property in device");
		caddr = path;
	}

	addr = g_ascii_strdown(caddr, -1);

	g_hash_table_destroy(props);
	g_object_unref(proxy);

	return addr;
}


/**
 * Add device into device proxy list
 */
static void add_device(const char *path, DBusGProxy *proxy, const char *adapter_path)
{
	device_object *dev;

	if (get_device(path)) {
		remove_device(path);
	}

	dev = g_new(device_object, 1);
	dev->addr = get_device_addr(path);
	dev->path = g_strdup(path);
	dev->adapter = g_strdup(adapter_path);
	dev->proxy = proxy;
	dev->channels = 0;

	llist_add(devices(), dev);
}

static int cmp_channel(void* arg, void* nodeElement)
{
	const char *path = (char *) arg;
	channel_object *m = (channel_object *) nodeElement;

	if (strcmp(m->path, path) == 0)
		return 1;
	return 0;
}

/**
 * Fetch channel struct from list
 */
static channel_object *get_channel(const char *path)
{
	return (channel_object *) llist_search_first(channels(), (void *) path, cmp_channel);
}

static int cmp_channel_by_handle(void *arg, void *nodeElement)
{
	guint64 varg = *(guint64*) arg;
	channel_object *m = (channel_object *) nodeElement;

	if (m->handle == varg)
		return 1;
	return 0;
}

/**
 * Fetch channel struct from list by handle
 */
static channel_object *get_channel_by_handle(guint64 handle)
{
	return (channel_object *) llist_search_first(channels(), &handle, cmp_channel_by_handle);
}

static int cmp_channel_by_fd(void* arg, void* nodeElement)
{
	channel_object *m = (channel_object *) nodeElement;
	int fd = *((int*) arg);

	if (m->fd == fd && fd >= 0)
		return 1;
	return 0;
}

/**
 * Fetch channel struct from list by fd
 */
static channel_object *get_channel_by_fd(int fd)
{
	return (channel_object *) llist_search_first(channels(), &fd, cmp_channel_by_fd);
}


/**
 * Remove channel path from channel proxy list
 */
static void remove_channel(const char *path, int passive)
{
	channel_object *c = get_channel(path);

	if (c) {
		GError *error = NULL;
		device_object *d = get_device_object(c->device);

		if (d) {
			d->channels--;
		}

		if (d && !passive) {
			DEBUG("Destroying channel %s", path);
			if (!dbus_g_proxy_call(d->proxy, "DestroyChannel",
						&error,
						DBUS_TYPE_G_OBJECT_PATH,
						path,
						G_TYPE_INVALID,
						G_TYPE_INVALID)) {
				if (error) {
					DEBUG("Err destroying HDP channel %s",
								error->message);
					g_error_free(error);
				}
			}
		}

		g_free(c->path);
		g_free(c->device);
		g_object_unref(c->proxy);
		shutdown(c->fd, SHUT_RDWR);
		close(c->fd);
		c->fd = -1;
		llist_remove(channels(), c);
		g_free(c);
	}
}


/**
 * Add channel into channel proxy list
 */
static guint64 add_channel(const char *path, const char *device, int fd,
						gboolean initiated_by_us)
{
	DBusGProxy *proxy;
	GIOChannel *gio;

	gio = g_io_channel_unix_new(fd);
	g_io_add_watch(gio, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL,
		       data_received, NULL);

	channel_object *c = get_channel(path);
	if (c) {
		c->fd = fd;
		return 0;
	}

	proxy = dbus_g_proxy_new_for_name(conn, "org.bluez", path,
					  "org.bluez.HealthChannel");

	c = (channel_object *) g_new(channel_object, 1);
	c->path = g_strdup(path);
	c->device = g_strdup(device);
	c->proxy = proxy;
	c->fd = fd;
	c->initiated_by_us = initiated_by_us;

	llist_add(channels(), c);

	device_object *dev = get_device_object(device);
	if (dev) {
		c->first = (++dev->channels == 1);
		if (c->first) {
			c->handle = ++last_handle;
			dev->first_handle = c->handle;
			return c->handle;
		} else {
			c->handle = dev->first_handle;
			return 0;
		}
	} else {
		ERROR("unknown dev %s", device);
	}

	return 0;
}

static int cmp_adapter(void* arg, void* nodeElement)
{
	const char *path = (char *) arg;
	adapter_object *m = (adapter_object *) nodeElement;

	if (strcmp(m->path, path) == 0)
		return 1;
	return 0;
}

/**
 * Fetch adapter struct from list
 */
static adapter_object *get_adapter(const char *path)
{
	return (adapter_object *) llist_search_first(adapters(), (void *) path, cmp_adapter);
}


/**
 * Remove adapter path from list
 */
static void remove_adapter(const char *path)
{
	adapter_object *a = get_adapter(path);

	if (a) {
		g_free(a->path);
		g_object_unref(a->proxy);
		g_free(a);
		llist_remove(adapters(), a);
	}
}


/**
 * Add adapter to list
 */
static void add_adapter(const char *path, DBusGProxy *proxy)
{
	adapter_object *a;

	if (get_adapter(path))
		remove_adapter(path);

	a = (adapter_object *) g_new(adapter_object, 1);
	a->path = g_strdup(path);
	a->proxy = proxy;

	llist_add(adapters(), a);
}

/**
 * Callback for org.bluez.HealthDevice.ChannelConnected signal
 * Reused by CreateChannel callback (channel initiation)
 */
static void channel_connected(DBusGProxy *proxy, const char *path, gpointer user_data)
{
	int fd;
	guint64 handle;
	device_object *dev;
	const char *device_path;

	channel_object *c = get_channel(path);
	if (c) {
		if (c->initiated_by_us && c->fd >= 0) {
			DEBUG("initiated channel already connected: %s", path);
			return;
		}
	}

	DEBUG("channel connected: %s", path);

	// Need to use non-GLib code here because GLib still does not have
	// the UNIX FD type.

	DBusMessage *msg, *reply;
	DBusError err;

	msg = dbus_message_new_method_call("org.bluez", path,
					   "org.bluez.HealthChannel", "Acquire");

	if (!msg) {
		ERROR(" network:dbus Can't allocate new method call");
		return;
	}

	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(
			dbus_g_connection_get_connection(conn),
			msg, -1, &err);

	dbus_message_unref(msg);

	if (!reply) {
		DEBUG(" network:dbus Can't acquire FD");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
			return;
		}
	}

	if (!dbus_message_get_args(reply, &err,
				   DBUS_TYPE_UNIX_FD, &fd,
				   DBUS_TYPE_INVALID)) {
		DEBUG(" network:dbus Can't get reply arguments");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
			dbus_message_unref(reply);
			return;
		}
	}

	dbus_message_unref(reply);

	// dbus_connection_flush(dbus_g_connection_get_connection(conn));
	DEBUG("File descriptor: %d", fd);

	device_path = dbus_g_proxy_get_path(proxy);
	handle = add_channel(path, device_path, fd, user_data != NULL);

	if (!handle) {
		// channel already known, reconnected
		// or channel is secondary
		return;
	}

	dev = get_device_object(device_path);

	if (dev) {
		device_connected(handle, dev->addr);
	} else {
		ERROR("Channel from unknown device: %s", device_path);
		device_connected(handle, device_path);
	}
}


static void channel_closed(const char *path);

/**
 * Callback for org.bluez.HealthDevice.ChannelDestroyed signal
 */
static void channel_deleted(DBusGProxy *proxy, const char *path, gpointer user_data)
{
	DEBUG("channel destroyed: %s", path);
	channel_closed(path);
}

/**
 * Forces closure of a channel
 */
static int disconnect_channel(guint64 handle, int passive)
{
	channel_object *c;

	while ((c = get_channel_by_handle(handle))) {
		DEBUG("removing channel");
		remove_channel(c->path, passive);
	}

	return 1;
}


/**
 * Forces closure of a channel -- called by higher layer
 */
static int force_disconnect_channel(Context *c)
{
	return disconnect_channel(c->id.connid, 0);
}


/**
 * Disconnect HealthDevice signals
 */
static gboolean disconnect_device_signals(const char *device_path)
{
	channel_object *c;
	DBusGProxy *proxy = get_device(device_path);
	gboolean dirty;

	if (!device_path)
		return FALSE;

	dbus_g_proxy_disconnect_signal(proxy, "ChannelConnected",
				       G_CALLBACK(channel_connected),
				       NULL);

	dbus_g_proxy_disconnect_signal(proxy, "ChannelDeleted",
				       G_CALLBACK(channel_deleted),
				       NULL);

	DEBUG("Disconnected device %s", device_path);

	// Disconnect channels related to this device
	do {
		dirty = FALSE;
		LinkedNode *i = channels()->first;

		while (i) {
			c = i->element;
		
			if (strcmp(c->device, device_path) == 0) {
				disconnect_channel(c->handle, 0);
				dirty = TRUE;
				break;
			}
			i = i->next;
		}
        
	} while (dirty);

	remove_device(device_path);

	return TRUE;
}


/**
 * Disconnect all devices from HealthDevice signals
 */
static void disconnect_all_devices()
{
	device_object *dev;

	while (devices()->first) {
		dev = devices()->first->element;
		disconnect_device_signals(dev->path);
	}

	llist_destroy(devices(), NULL);
	_devices = NULL;
}


/**
 * Takes care of channel closure, when initiative is remote
 */
static void channel_closed(const char *path)
{
	channel_object *c = get_channel(path);
	device_object *d;

	if (!c)
		return;

	d = get_device_object(c->device);

	if (c->first) {
		// notifies higher layers
		if (d) {
			device_disconnected(c->handle, d->addr);
		} else {
			ERROR("Unknown device: %s", c->device);
			device_disconnected(c->handle, c->device);
		}
	}

	disconnect_channel(c->handle, 1);
}


/**
 * Forces closure of all open channels
 */
static void disconnect_all_channels()
{
	channel_object *chan;

	while (channels()->first) {
		chan = channels()->first->element;
		disconnect_channel(chan->handle, 0);
	}

	llist_destroy(channels(), NULL);
	_channels = NULL; 
}

/**
 * Connect HealthDevice signals, to get channel connection signals later
 */
static void connect_device_signals(const char *device_path, const char *adapter_path)
{
	DBusGProxy *proxy;

	DEBUG("device to be connected: %s", device_path);

	if (get_device(device_path)) {
		disconnect_device_signals(device_path);
	}

	proxy = dbus_g_proxy_new_for_name(conn, "org.bluez", device_path,
					  "org.bluez.HealthDevice");

	if (!proxy) {
		ERROR("%s not a HealthDevice", device_path);
		return;
	}

	add_device(device_path, proxy, adapter_path);

	dbus_g_proxy_add_signal(proxy, "ChannelConnected",
				DBUS_TYPE_G_OBJECT_PATH, G_TYPE_INVALID);

	dbus_g_proxy_add_signal(proxy, "ChannelDeleted",
				DBUS_TYPE_G_OBJECT_PATH, G_TYPE_INVALID);

	dbus_g_proxy_connect_signal(proxy, "ChannelConnected",
				    G_CALLBACK(channel_connected),
				    NULL, NULL);

	dbus_g_proxy_connect_signal(proxy, "ChannelDeleted",
				    G_CALLBACK(channel_deleted),
				    NULL, NULL);

	return;
}


/**
 * Callback for org.bluez.Adapter.DeviceCreated signal
 */
static void device_created(DBusGProxy *proxy, const char *path, gpointer user_data)
{
	connect_device_signals(path, dbus_g_proxy_get_path(proxy));
	DEBUG("device created: %s", path);
}


/**
 * Callback for org.bluez.Adapter.DeviceCreated signal
 */
static void device_removed(DBusGProxy *proxy, const char *path, gpointer user_data)
{
	disconnect_device_signals(path);
	DEBUG("device removed: %s", path);
}


/**
 * Get manager proxy
 */
static gboolean create_manager_proxy()
{
	manager = dbus_g_proxy_new_for_name(conn, "org.bluez", "/",
					    "org.bluez.Manager");

	if (!manager) {
		ERROR("BlueZ manager service not found");
	}

	return !!manager;
}


/**
 * Destroy manager proxy
 */
static void destroy_manager_proxy()
{
	g_object_unref(manager);
	manager = NULL;
}


/**
 * Connect adapter and devices D-Bus signals
 */
static void connect_adapter(const char *adapter)
{
	/* Find devices and connect to ChannelConnected/Destroyed signals */

	DBusGProxy *proxy;
	GHashTable *props;
	GValue *devlist;
	GError *error = NULL;
	GPtrArray *array;
	unsigned int i;

	DEBUG("connecting adapter: %s", adapter);

	proxy = dbus_g_proxy_new_for_name(conn, "org.bluez",
					  adapter, "org.bluez.Adapter");

	if (!proxy) {
		ERROR("Adapter interface not found");
		return;
	}

	add_adapter(adapter, proxy);

	dbus_g_proxy_add_signal(proxy, "DeviceCreated",
				DBUS_TYPE_G_OBJECT_PATH, G_TYPE_INVALID);

	dbus_g_proxy_add_signal(proxy, "DeviceRemoved",
				DBUS_TYPE_G_OBJECT_PATH, G_TYPE_INVALID);

	dbus_g_proxy_connect_signal(proxy, "DeviceCreated",
				    G_CALLBACK(device_created),
				    NULL, NULL);

	dbus_g_proxy_connect_signal(proxy, "DeviceRemoved",
				    G_CALLBACK(device_removed),
				    NULL, NULL);

	if (!dbus_g_proxy_call(proxy, "GetProperties", &error,
			       G_TYPE_INVALID,
			       dbus_g_type_get_map("GHashTable", G_TYPE_STRING, G_TYPE_VALUE),
			       &props, G_TYPE_INVALID)) {
		if (error) {
			ERROR("Can't get device list: %s", error->message);
			g_error_free(error);
		} else {
			ERROR("Can't get device list, probably disconnected");
		}

		return;
	}

	DEBUG("Getting known devices list");

	devlist = g_hash_table_lookup(props, "Devices");

	if (devlist) {
		array = g_value_get_boxed(devlist);

		if (array)
			for (i = 0; i < array->len; ++i)
				connect_device_signals(g_ptr_array_index(array, i),
						       adapter);
	} else {
		DEBUG("No Devices property in adapter properties");
	}

	g_hash_table_destroy(props);
}


/**
 * Disconnects adapter signals
 */
static void disconnect_adapter(const char *path)
{
	adapter_object *a = get_adapter(path);
	gboolean dirty;
	device_object *d;

	if (!a)
		return;

	DEBUG("Disconnecting adapter: %s", path);

	dbus_g_proxy_disconnect_signal(a->proxy, "DeviceCreated",
				       G_CALLBACK(device_created),
				       NULL);

	dbus_g_proxy_disconnect_signal(a->proxy, "DeviceRemoved",
				       G_CALLBACK(device_removed),
				       NULL);

	// Disconnect devices related to this adapter
	do {
		dirty = FALSE;
		LinkedNode *i = devices()->first;

		while (i) {
			d = i->element;
			if (strcmp(d->adapter, path) == 0) {
				disconnect_device_signals(d->path);
				dirty = TRUE;
				break;
			}
			i = i->next;
		}

	} while (dirty);


	remove_adapter(path);
}

static int cmp_data_type(void* arg, void* nodeElement)
{
	guint16 varg = *((guint16*) arg);
	app_object *app = (app_object *) nodeElement;

	if (app->data_type == varg || varg == 0) {
		return 1;
	}
	return 0;
    
}

/**
 * Gets a HealthApplication path given a data type
 */
static const app_object *find_application_by_type(guint16 data_type)
{
	return (app_object *) llist_search_first(apps(), &data_type, cmp_data_type);
}

/**
 * Creates the HealthApplication's by calling BlueZ
 *
 * @param is_sink TRUE if data type is Sink role
 * @param data_type Specialization or data type 
 */
gboolean create_health_application(gboolean is_sink, guint16 data_type)
{
	/* Create HealthApplication */

	// Need to use non-GLib code here because GLib still does not have
	// the G_TYPE_UINT16 type.

	DBusMessage *msg, *reply;
	DBusError err;
	DBusMessageIter iter, array, entry, variant;
	guint16 value;
	const char *svalue;
	const char *key;
	char *app_path;
	app_object *app;

	msg = dbus_message_new_method_call("org.bluez", "/org/bluez",
					   "org.bluez.HealthManager", "CreateApplication");

	if (!msg) {
		DEBUG(" network:dbus Can't allocate new method call");
		return FALSE;
	}

	dbus_message_iter_init_append(msg, &iter);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &array);

	key = "DataType";
	value = data_type;
	dbus_message_iter_open_container(&array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
	dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "q", &variant);
	dbus_message_iter_append_basic(&variant, DBUS_TYPE_UINT16, &value);
	dbus_message_iter_close_container(&entry, &variant);
	dbus_message_iter_close_container(&array, &entry);

	key = "Role";
	svalue = (is_sink ? "Sink" : "Source");
	dbus_message_iter_open_container(&array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
	dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "s", &variant);
	dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &svalue);
	dbus_message_iter_close_container(&entry, &variant);
	dbus_message_iter_close_container(&array, &entry);

	key = "Description";
	svalue = "healthd";
	dbus_message_iter_open_container(&array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
	dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "s", &variant);
	dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &svalue);
	dbus_message_iter_close_container(&entry, &variant);
	dbus_message_iter_close_container(&array, &entry);

	dbus_message_iter_close_container(&iter, &array);

	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(
			dbus_g_connection_get_connection(conn),
			msg, -1, &err);

	dbus_message_unref(msg);

	if (!reply) {
		DEBUG(" network:dbus Can't create application");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
		}

		return FALSE;
	}

	if (!dbus_message_get_args(reply, &err,
				   DBUS_TYPE_OBJECT_PATH, &app_path,
				   DBUS_TYPE_INVALID)) {
		DEBUG(" network:dbus Can't get reply arguments");

		if (dbus_error_is_set(&err)) {
			ERROR("%s", err.message);
			dbus_error_free(&err);
		}

		return FALSE;
	}

	app = g_new0(app_object, 1);
	app->path = g_strdup(app_path);
	app->data_type = data_type;
	app->is_sink = is_sink;

	llist_add(apps(), app);

	dbus_message_unref(reply);

	DEBUG("Created health application: %s", (char *) app->path);

	return TRUE;
}


/**
 * Destroy health applications
 */
static void destroy_health_applications()
{
	DBusGProxy *proxy;

	proxy = dbus_g_proxy_new_for_name(conn, "org.bluez",
					  "/org/bluez", "org.bluez.HealthManager");

	if (!proxy) {
		ERROR("BlueZ health manager service not found");
		return;
	}
	
	while (apps()->first) {
		GError *error = NULL;
		struct app_object *app = apps()->first->element;

		DEBUG("Destroying %s", app->path);

		if (!dbus_g_proxy_call(proxy, "DestroyApplication",
				       &error,
				       DBUS_TYPE_G_OBJECT_PATH, app->path,
				       G_TYPE_INVALID,
				       G_TYPE_INVALID)) {
			if (error) {
				ERROR("Can't call DestroyApplication: %s", error->message);
				g_error_free(error);
			} else {
				DEBUG("Can't call DestroyApplication, probably disconnected");
			}
		}

		llist_remove(apps(), app);
		g_free(app->path);
		g_free(app);
	}
	llist_destroy(apps(), NULL);
	_apps = NULL;
	g_object_unref(proxy);
}


/**
 * Create health applications for data types requested by client
 *
 * @param is_sink True if data types are of Sink role
 * @param hdp_data_types List of HDP data types (specializations)
 * @return TRUE if success
 */
gboolean plugin_bluez_update_data_types(gboolean is_sink, guint16 hdp_data_types[])
{
	guint16 *data_type;
	gboolean ok = TRUE;

	if (!hdp_data_types)
		return FALSE;

	destroy_health_applications();

	for (data_type = hdp_data_types; *data_type; ++data_type) {
		ok = ok && create_health_application(is_sink, *data_type);
	}

	return ok;
}


/**
 * Finds current adapters and connect to them
 */
static void find_current_adapters()
{
	GHashTable *props;
	GError *error = NULL;
	GValue *gv;
	GPtrArray *array;
	unsigned int i;

	if (!dbus_g_proxy_call(manager, "GetProperties", &error,
			       G_TYPE_INVALID,
			       dbus_g_type_get_map("GHashTable", G_TYPE_STRING, G_TYPE_VALUE),
			       &props, G_TYPE_INVALID)) {
		if (error) {
			ERROR("Can't get adapter list: %s", error->message);
			g_error_free(error);
		} else {
			ERROR("Can't get adapter list, probably disconnected");
		}

		return;
	}

	gv = g_hash_table_lookup(props, "Adapters");

	if (gv) {
		array = g_value_get_boxed(gv);

		if (array)
			for (i = 0; i < array->len; ++i)
				connect_adapter(g_ptr_array_index(array, i));
	} else {
		DEBUG("No Adapters property in manager properties");
	}

	g_hash_table_destroy(props);
}


/**
 * Callback for org.bluez.Manager.AdapterAdded
 */
static void adapter_added(DBusGProxy *proxy, const char *path, gpointer user_data)
{
	connect_adapter(path);
}


/**
 * Callback for org.bluez.Manager.AdapterRemoved
 */
static void adapter_removed(DBusGProxy *proxy, const char *path, gpointer user_data)
{
	disconnect_adapter(path);
}


/**
 * Connects to adapter added and removed signals,
 * and possibly others in the future
 */
static void connect_manager_signals()
{
	dbus_g_proxy_add_signal(manager, "AdapterAdded",
				DBUS_TYPE_G_OBJECT_PATH, G_TYPE_INVALID);

	dbus_g_proxy_add_signal(manager, "AdapterRemoved",
				DBUS_TYPE_G_OBJECT_PATH, G_TYPE_INVALID);

	dbus_g_proxy_connect_signal(manager, "AdapterAdded",
				    G_CALLBACK(adapter_added),
				    NULL, NULL);

	dbus_g_proxy_connect_signal(manager, "AdapterRemoved",
				    G_CALLBACK(adapter_removed),
				    NULL, NULL);
}


/**
 * Disconnects from manager signals
 */
static void disconnect_manager_signals()
{
	dbus_g_proxy_disconnect_signal(manager, "AdapterAdded",
				       G_CALLBACK(adapter_added),
				       NULL);

	dbus_g_proxy_disconnect_signal(manager, "AdapterRemoved",
				       G_CALLBACK(adapter_removed),
				       NULL);
}


/**
 * Starts Health link with BlueZ.
 *
 * @param plugin_label sequential label of this plugin
 * @return success status
 */
static int init(unsigned int plugin_label)
{
	plugin_id = plugin_label;

	GError *error = NULL;

	if (conn)
		return NETWORK_ERROR_NONE;

	DEBUG("Starting BlueZ link...");

	// connect to the bus and check for errors
	conn = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);

	if (error != NULL) {
		ERROR(" network:dbus Connection Error (%s)", error->message);
		g_error_free(error);
		error = NULL;
	}

	if (NULL == conn) {
		ERROR(" network:dbus Connection Error, exiting");
		return NETWORK_ERROR;
	}

	if (!create_manager_proxy()) {
		ERROR(" network:dbus fail create_manager_proxy()");
		return NETWORK_ERROR;
	}

	connect_manager_signals();
	find_current_adapters();

	return NETWORK_ERROR_NONE;
}


/**
 * Does memory cleanup after BlueZ link stopped.
 * This is made as a separate function because main loop must hava a chance
 * to handle stopping before objects are destroyed.
 */
static gboolean cleanup(gpointer data)
{
	if (conn)
		g_object_unref(conn);

	g_free(current_data);

	current_data = NULL;
	conn = NULL;

	return FALSE;
}


/**
 * Disconnects all adapters
 */
static void disconnect_all_adapters()
{
	adapter_object *a;

	while (adapters()->first) {
		a = adapters()->first->element;
		disconnect_adapter(a->path);
	}

	llist_destroy(adapters(), NULL);
	_adapters = NULL;
}

/**
 * Stops BlueZ link. Link may be restarted again afterwards.
 *
 * @return success status
 */
static int finalize()
{
	DEBUG("Stopping BlueZ link...");

	disconnect_all_channels();
	disconnect_all_devices();
	destroy_health_applications();
	disconnect_all_adapters();
	disconnect_manager_signals();
	destroy_manager_proxy();

	g_idle_add(cleanup, NULL);

	return NETWORK_ERROR_NONE;
}


/**
 * Reads an APDU from buffer
 *
 * @param ctx The communication context
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

			ContextId cid = {plugin_id, c->handle};
			communication_read_input_stream(cid);
		}
	}

	if (!more) {
		g_io_channel_unref(gio);
		close(c->fd);
		c->fd = -1;
	}

	return more;
}


/**
 * Sends IEEE data to HDP channel
 *
 * @return success status
 */
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream)
{
	DEBUG("\nSend APDU");

	int ret = send_data(ctx->id.connid, stream->buffer, stream->size);

	if ((ret < 0) || ((unsigned) ret != stream->size)) {
		DEBUG("Error sending APDU. %d bytes sent. Should have sent %d.",
		      ret, stream->size);
		force_disconnect_channel(ctx);
		return NETWORK_ERROR;
	}


	return NETWORK_ERROR_NONE;
}


/**
 * Send data to data channel
 *
 * @param *data
 * @param len
 */
static int send_data(uint64_t connid, unsigned char *data, int len)
{
	int sent, just_sent;
	channel_object *c;

	// gets first channel because it is the first on list
	c = get_channel_by_handle(connid);

	if (!c) {
		DEBUG("unknown channel by handle");
		return -1;
	}

	if (c->fd < 0) {
		DEBUG("channel fd is not valid");
		return -1;
	}

	sent = 0;

	while (sent < len) {
		DEBUG("Sending %d bytes of data (offset %d)", len - sent, sent);
		just_sent = send(c->fd, data + sent, len - sent, 0);
		if (just_sent < 0) {
			DEBUG("Error: %s", strerror(errno));
		}

		if (just_sent == 0) {
			close(c->fd);
			break;
		} else if (just_sent < 0) {
			sent = -1;
			break;
		}

		sent += just_sent;
	}

	return sent;
}

/**
 * Initiated connection callback
 */
void plugin_bluez_connect_cb(DBusGProxy *dev_proxy, DBusGProxyCall *id,
				gpointer user_data)
{
	gboolean ok;
	GError *error = NULL;
	char *channel_path = NULL;

	ok = dbus_g_proxy_end_call(dev_proxy, id, &error,
			DBUS_TYPE_G_OBJECT_PATH, &channel_path,
			G_TYPE_INVALID);

	if (!ok && error) {
		DEBUG("connection initiation error: %s", error->message);
		g_error_free(error);
		return;
	}

	channel_connected(dev_proxy, channel_path, "initiated");
	g_free(channel_path);
}

/**
 * Rediscover device to make sure we see HDP profile.
 * This is important e.g. when we are an agent and the manager
 * was not running HDP app at the moment of pairing. BlueZ does
 * not expose Health API interface for devices that don't implement
 * the HDP profile, and this information is cached, so we may need
 * to rediscover. (This might be the case when the manager takes
 * the initiative of connection, but generally the agents are 
 * specialized devices that run HDP all the time, and profiles
 * discovered at pairing never change, so rediscovery in this
 * case is hardly ever necessary. If you do need to rediscover
 * ad hoc, see the script sdk/rediscover.py in source tree.)
 */
int plugin_bluez_discover(const char *btaddr)
{
	int retstatus = 1;
	char *addr = g_ascii_strdown(btaddr, -1);
	DBusGProxy *proxy = NULL;

	{
		struct device_object *dev = get_device_object_by_addr(addr);
		if (!dev) {
			DEBUG("Device %s unknown by BlueZ", btaddr);
			goto finally;
		}

		proxy = dbus_g_proxy_new_for_name(conn, "org.bluez",
					dev->path, "org.bluez.Device");
	}

	if (!proxy) {
		ERROR("Can't get org.bluez.Device interface");
		goto finally;
	}

	GError *error = NULL;

	if (dbus_g_proxy_call(proxy, "DiscoverServices",
					&error,
					G_TYPE_STRING, "",
					G_TYPE_INVALID,
					G_TYPE_INVALID)) {
		if (error) {
			DEBUG("Error discovering %s: %s", btaddr, error->message);
			g_error_free(error);
		} else {
			DEBUG("Unknown error discovering %s:", btaddr);
		}
	}

	retstatus = 0;

finally:
	if (proxy) {
		g_object_unref(proxy);
	}
	g_free(addr);
	return retstatus;
}

/**
 * Take the initiative of a connection. Actual connection establishment
 * is reported via "peer_connected" callback, as happens with accepted
 * (passive) connections.
 *
 * @param *btaddr The Bluetooth address
 * @param data_type The data type
 * @param reliability The reliability of the HDP channel
 * @return success in initiating connection procedure
 *         (not necessarily connection will happen)
 */
gboolean plugin_bluez_connect(const char *btaddr, guint16 data_type, int reliability)
{
	const char *channel_type;
	const app_object *app;
	gboolean ok = FALSE;
	char *addr = g_ascii_strdown(btaddr, -1);
	struct device_object *dev = get_device_object_by_addr(addr);

	if (!dev) {
		DEBUG("Device %s unknown by BlueZ", btaddr);
		goto finally;
	}

	app = find_application_by_type(data_type);
	if (!app) {
		DEBUG("Data type %d could not be matched to any application",
			data_type);
		DEBUG("Make sure you had configured the plug-in with all"
			"possible data types you need to use");
		goto finally;
	}

	if (app->is_sink) {
		if (reliability != HDP_CHANNEL_ANY) {
			reliability = HDP_CHANNEL_ANY;
			DEBUG("HDP sinks can't choose channel type")
			DEBUG("Defaulting channel type to Any");
		}
	} else {
		if (reliability == HDP_CHANNEL_ANY) {
			reliability = HDP_CHANNEL_RELIABLE;
			DEBUG("HDP sources must define channel type, cannot use Any")
			DEBUG("Defaulting channel type to Reliable");
		}
	}

	if (reliability == HDP_CHANNEL_RELIABLE) {
		channel_type = "Reliable";
	} else if (reliability == HDP_CHANNEL_STREAMING) {
		channel_type = "Streaming";
	} else if (reliability == HDP_CHANNEL_ANY) {
		channel_type = "Any";
	} else {
		DEBUG("Invalid channel type, defaulting to Reliable");
		// safest choice
		channel_type = "Reliable";
	}

	if (dbus_g_proxy_begin_call(dev->proxy, "CreateChannel",
			plugin_bluez_connect_cb, NULL, NULL,
			DBUS_TYPE_G_OBJECT_PATH, app->path,
			G_TYPE_STRING, channel_type,
			G_TYPE_INVALID))
		ok = TRUE;

finally:
	g_free(addr);
	return ok;
}

/** @} */
