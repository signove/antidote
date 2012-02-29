/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file healthd_service.c
 * \brief Health manager service
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
 * \author Walter Guerra
 * \author Fabricio Silva
 * \date Jul 7, 2010
 */

/**
 * @addtogroup Healthd
 * @{
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <glib.h>
#include <gio/gio.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <ieee11073.h>
#include "src/communication/plugin/bluez/plugin_bluez.h"
#include "src/communication/plugin/trans/plugin_trans.h"
#include "src/trans/plugin/example_oximeter.h"
#ifndef ANDROID_HEALTHD
#include "src/communication/plugin/usb/plugin_usb.h"
#endif
#include "src/util/log.h"
#include "src/communication/service.h"
#include "src/dim/pmstore_req.h"

#define SRV_SERVICE_NAME "com.signove.health"
#define SRV_OBJECT_PATH "/com/signove/health"
#define SRV_INTERFACE "com.signove.health.manager"

#define DEVICE_OBJECT_PATH "/com/signove/health/device"
#define DEVICE_INTERFACE "com.signove.health.device"

#define AGENT_INTERFACE "com.signove.health.agent"

static PluginBluezListener bluez_listener;
#ifndef ANDROID_HEALTHD
static PluginUsbListener usb_listener;
#endif

static const int DBUS_SERVER = 0;
static const int TCP_SERVER = 1;
static const int AUTOTESTING = 2;

static int opmode;

static gboolean call_agent_measurementdata(ContextId, char *);
static gboolean call_agent_disassociated(ContextId);
static gboolean call_agent_associated(ContextId, char *);
static gboolean call_agent_segmentinfo(ContextId, guint, char *);
static gboolean call_agent_segmentdataresponse(ContextId, guint, guint, guint);
static gboolean call_agent_segmentdata(ContextId, guint, guint, char *);
static gboolean call_agent_segmentcleared(ContextId, guint, guint, guint);
static gboolean call_agent_pmstoredata(ContextId, guint, char *);


/* TCP clients */

typedef struct {
	int fd;
	char *buf;
} tcp_client;

static const unsigned int PORT = 9005;
static GSList *tcp_clients = NULL;
static int server_fd = -1;

static void tcp_close(tcp_client *client)
{
	DEBUG("TCP: freeing client %p", client);

	shutdown(client->fd, SHUT_RDWR);
	close(client->fd);
	client->fd = -1;
	free(client->buf);
	client->buf = 0;
	tcp_clients = g_slist_remove(tcp_clients, client);
	free(client);
}

static gboolean tcp_write(GIOChannel *src, GIOCondition cond, gpointer data)
{
	tcp_client *client = (tcp_client*) data;
	gsize len = strlen(client->buf);
	gsize written;
	char *newbuf;
	gboolean more;

	if (cond != G_IO_OUT) {
		DEBUG("TCP: write: false alarm");
		return TRUE;
	}

	if (len <= 0) {
		g_io_channel_unref(src);
		return FALSE;
	}

	DEBUG("TCP: writing client %p", data);

	int fd = g_io_channel_unix_get_fd(src);
	written = send(fd, client->buf, len, 0);

	DEBUG("TCP: client %p written %d bytes", data, (int) written);

	if (written <= 0) {
		free(client->buf);
		client->buf = strdup("");
		g_io_channel_unref(src);
		return FALSE;
	}

	if (written >= len) {
		newbuf = strdup("");
		g_io_channel_unref(src);
		more = FALSE;
	} else {
		newbuf = strdup(client->buf + written);
		more = TRUE;
	}
	free(client->buf);
	client->buf = newbuf;

	return more;
}

static gboolean tcp_read(GIOChannel *src, GIOCondition cond, gpointer data)
{
	char buf[256];
	gsize count;

	DEBUG("TCP: reading client %p", data);

	tcp_client *client = (tcp_client*) data;

	if (cond != G_IO_IN) {
		tcp_close(client);
		g_io_channel_unref(src);
		return FALSE;
	}

	int fd = g_io_channel_unix_get_fd(src);
	count = recv(fd, buf, 256, 0);

	if (count == 0) {
		tcp_close(client);
		g_io_channel_unref(src);
		return FALSE;
	}

	return TRUE;
}

static void tcp_send(tcp_client *client, const char *msg)
{
	char *newbuf;

	DEBUG("TCP: scheduling write %p", client);

	if (asprintf(&newbuf, "%s%s", client->buf, msg) < 0) {
		return;
	}

	free(client->buf);
	client->buf = newbuf;

	GIOChannel *channel = g_io_channel_unix_new(client->fd);
	g_io_add_watch(channel, G_IO_OUT, tcp_write, client);
}

static gboolean tcp_accept(GIOChannel *src, GIOCondition cond, gpointer data)
{
	tcp_client *new_client;
	struct sockaddr_in addr;
	int fd;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	bzero(&addr, sizeof(addr));

	DEBUG("TCP: accepting condition");

	fd = accept(g_io_channel_unix_get_fd(src), (struct sockaddr *) &addr, &addrlen);

	if (fd < 0) {
		DEBUG("TCP: Failed accept");
		return TRUE;
	}

	new_client = g_new0(tcp_client, 1);
	new_client->fd = fd;
	new_client->buf = strdup("");

	DEBUG("TCP: adding client %p to list", new_client);

	GIOChannel *channel = g_io_channel_unix_new(fd);
	g_io_add_watch(channel, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL, tcp_read, new_client);

	tcp_clients = g_slist_prepend(tcp_clients, new_client);

	return TRUE;
}

static void tcp_listen()
{
	struct sockaddr_in addr;
	int reuse = 1;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);
	bind(server_fd, (struct sockaddr *) &addr, sizeof(addr));
	listen(server_fd, 5);

	GIOChannel *channel = g_io_channel_unix_new(server_fd);
	g_io_add_watch(channel, G_IO_IN, tcp_accept, 0);

	DEBUG("TCP: listening");
}

static void tcp_announce(const char *command, const char *path, const char *arg)
{
	GSList *i;
	char *msg;
	char *j;
	char *arg2 = strdup(arg);

	for (j = arg2; *j; ++j)
		if ((*j == '\t') || (*j == '\n'))
			*j = ' ';

	if (asprintf(&msg, "%s\t%s\t%s\n", command, path, arg2) < 0) {
		free(arg2);
		return;
	}

	printf("%s\n", msg);
	
	for (i = tcp_clients; i; i = i->next) {
		tcp_send(i->data, msg);
	}

	free(arg2);
	free(msg);
}


/* Called by IEEE library */

/**
 * Resets a framework-depende timer
 *
 * @param ctx Context
 */
static void timer_reset_timeout(Context *ctx)
{
	if (ctx->timeout_action.id) {
		g_source_remove(ctx->timeout_action.id);
	}
}
/**
 * Timer callback.
 * Calls the supplied callback when timer reaches timeout, and cancels timer.
 *
 * @param data Pointer to context
 * @return FALSE (to cancel the timeout)
 */
static gboolean timer_alarm(gpointer data)
{
	DEBUG("timer_alarm");
	Context *ctx = data;
	void (*f)() = ctx->timeout_action.func;
	if (f)
		f(ctx);
	return FALSE;
}

/**
 * Initiates a timer in behalf of IEEE library
 *
 * @param ctx Context
 * @return The timer handle
 */
static int timer_count_timeout(Context *ctx)
{
	ctx->timeout_action.id = g_timeout_add(ctx->timeout_action.timeout
					       * 1000, timer_alarm, ctx);
	return ctx->timeout_action.id;
}

/**
 * Callback for when new data has been received.
 *
 * @param ctx current context.
 * @param list a pointer to data list.
 */
void new_data_received(Context *ctx, DataList *list)
{
	DEBUG("Medical Device System Data");

	char *data = xml_encode_data_list(list);

	if (data) {
		call_agent_measurementdata(ctx->id, data);
		free(data);
	}
}

typedef struct {
	ContextId id;
	int handle;
	int instnumber;
	DataList *list;
} segment_data_evt;

/**
 * Delayed PM-Segment data handling
 *
 * @param data pointer to event struct
 * @return always FALSE
 */
static gboolean segment_data_received_phase2(gpointer revt)
{
	segment_data_evt *evt = revt;

	DEBUG("PM-Segment Data phase 2");

	char *data = xml_encode_data_list(evt->list);

	if (data) {
		call_agent_segmentdata(evt->id, evt->handle, evt->instnumber, data);
		free(data);
	}

	data_list_del(evt->list);
	free(revt);
	return FALSE;
}

/**
 * Callback for when PM-Segment data has been received.
 *
 * @param ctx current context.
 * @param handle the PM-Store handle
 * @param instnumber the PM-Segment instance number
 * @param list a pointer to data list.
 */
void segment_data_received(Context *ctx, int handle, int instnumber, DataList *list)
{
	DEBUG("PM-Segment Data");
	segment_data_evt *evt = calloc(1, sizeof(segment_data_evt));

	// Different from other callback events, "list" is not freed by core, but
	// it is passed ownership instead.

	// Encoding a whole PM-Segment to XML may take a *LONG* time. If the program
	// is single-threaded, encoding here would block the 11073 stack, causing
	// the agent to abort because it didn't get confirmation in time.

	// So, encoding XML from data list is better left to a thread, or, at very
	// least, delayed until there are no pending events.

	evt->id = ctx->id;
	evt->handle = handle;
	evt->instnumber = instnumber;
	evt->list = list;

	g_idle_add(segment_data_received_phase2, evt);
}


/**
 * Device has been associated.
 *
 * @param ctx current context.
 * @param list the data list of elements.
 */
void device_associated(Context *ctx, DataList *list)
{
	DEBUG("Device associated");

	char *data = xml_encode_data_list(list);

	if (data) {
		call_agent_associated(ctx->id, data);
		free(data);
	}
}

/**
 * Device has been disassociated
 */
void device_disassociated(Context *ctx)
{
	DEBUG("Device unassociated");
	call_agent_disassociated(ctx->id);
}


/* Object class definitions */

typedef struct {
	GObject parent;
} Serv;

typedef struct {
	GObject parent;
	ContextId handle;
	char *path;
	char *addr;
} Device;

typedef struct {
	GObjectClass parent;
} ServClass;

typedef struct {
	GObjectClass parent;
} DeviceClass;

/**
 * \cond Undocumented
 */

GType srv_object_get_type(void);
GType device_object_get_type(void);

#define SRV_TYPE_OBJECT (srv_object_get_type())

#define SRV_OBJECT(object) \
	(G_TYPE_CHECK_INSTANCE_CAST((object), \
		SRV_TYPE_OBJECT, Serv))
#define SRV_OBJECT_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass), \
		SRV_TYPE_OBJECT, ServClass))
#define SRV_IS_OBJECT(object) \
	(G_TYPE_CHECK_INSTANCE_TYPE((object), \
		SRV_TYPE_OBJECT))
#define SRV_IS_OBJECT_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass), \
		SRV_TYPE_OBJECT))
#define SRV_OBJECT_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS((obj), \
		SRV_TYPE_OBJECT, ServClass))

G_DEFINE_TYPE(Serv, srv_object, G_TYPE_OBJECT);

static void srv_object_init(Serv *obj)
{
	g_assert(obj != NULL);
}

#define DEVICE_TYPE_OBJECT (device_object_get_type())

#define DEVICE_OBJECT(object) \
	(G_TYPE_CHECK_INSTANCE_CAST((object), \
		DEVICE_TYPE_OBJECT, Serv))
#define DEVICE_OBJECT_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass), \
		DEVICE_TYPE_OBJECT, ServClass))
#define DEVICE_IS_OBJECT(object) \
	(G_TYPE_CHECK_INSTANCE_TYPE((object), \
		DEVICE_TYPE_OBJECT))
#define DEVICE_IS_OBJECT_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass), \
		DEVICE_TYPE_OBJECT))
#define DEVICE_OBJECT_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS((obj), \
		SRV_TYPE_OBJECT, ServClass))

G_DEFINE_TYPE(Device, device_object, G_TYPE_OBJECT);

static void device_object_init(Device *obj)
{
	ContextId z = {0, 0};

	g_assert(obj != NULL);
	obj->handle = z;
	obj->path = NULL;
	obj->addr = NULL;
}

/**
 * \endcond
 */


static char *client_agent = NULL;
static char *client_name = NULL;

static GSList *devices = NULL;

DBusGConnection *bus = NULL;
DBusGProxy *agent_proxy = NULL;

static const char *get_device_object(const char *, ContextId);
static void get_agent_proxy();

/**
 * Callback related to manager.Configure D-Bus method.
 * Called when D-Bus service client wants to initiate a connection.
 * Currently not supported, always returns error.
 *
 * @param obj Serv object (GObject)
 * @param agent D-Bus agent address
 * @param addr target 11073 agent to connect to
 * @param data_types list of data specialization types to support
 * @param call Method invocation struct
 * @return success status
 */
gboolean srv_configure(Serv *obj, gchar *agent, gchar *addr,
		       GArray *data_types, DBusGMethodInvocation *call)
{
	DEBUG("Agent: %s Addr: %s", agent, addr);

	if (agent_proxy) {
		/* There is a client connected already */
		GQuark domain = g_quark_from_static_string("com.signove_health_service_error");
		GError *error = g_error_new(domain, 1, "Client already connected");
		dbus_g_method_return_error(call, error);
		g_error_free(error);
		return FALSE;
	}

	GQuark domain = g_quark_from_static_string(
				"com.signove_health_service_error");
	GError *error = g_error_new(domain, 1,
				    "Connection initiation not supported");
	dbus_g_method_return_error(call, error);
	g_error_free(error);

	return FALSE;
}

/**
 * Callback related to manager.ConfigurePassive D-Bus method.
 * Called when D-Bus service client wants to accept connections.
 *
 * @param obj Serv object (GObject)
 * @param agent D-Bus agent address
 * @param data_types list of data specialization types to support
 * @param call Method invocation struct
 * @return success status
 */
gboolean srv_configurepassive(Serv *obj, gchar *agent,
			      GArray *data_types,
			      DBusGMethodInvocation *call)
{
	gchar *sender;
	int i;
	guint16 *hdp_data_types;

	DEBUG("Agent: %s", agent);

	if (agent_proxy) {
		/* There is a client connected already */
		GQuark domain = g_quark_from_static_string("com.signove_health_service_error");
		GError *error = g_error_new(domain, 1, "Client already connected");
		dbus_g_method_return_error(call, error);
		g_error_free(error);
		return FALSE;
	}

	hdp_data_types = g_new0(guint16, data_types->len + 1);

	for (i = 0; i < data_types->len; ++i) {
		DEBUG("Data type: %x", g_array_index(data_types, int, i));
		hdp_data_types[i] = g_array_index(data_types, int, i);
	}

	client_agent = g_strdup(agent);
	sender = dbus_g_method_get_sender(call);
	client_name = g_strdup(sender);
	g_free(sender);
	get_agent_proxy();
	dbus_g_method_return(call);

	plugin_bluez_update_data_types(TRUE, hdp_data_types); // TRUE=sink

	g_free(hdp_data_types);

	return TRUE;
}

/**
 * Configures data types for auto-testing and TCP mode
 */
void self_configure()
{
	guint16 hdp_data_types[] = {0x1004, 0x1007, 0x1029, 0x100f, 0x0};
	plugin_bluez_update_data_types(TRUE, hdp_data_types); // TRUE=sink
}

/**
 * Finds device object given handle
 *
 * @param handle Context ID
 * @return device pointer or NULL if not found
 */
static Device *device_by_handle(ContextId handle)
{
	GSList *i;
	Device *device = NULL;

	/* search for conn handle in current devices */
	for (i = devices; i; i = i->next) {
		Device *candidate = i->data;

		if (candidate->handle.plugin == handle.plugin &&
		    candidate->handle.connid == handle.connid) {
			device = candidate;
			break;
		}
	}

	return device;
}

/**
 * Finds device object given an address
 *
 * @param low_addr device address (e.g. Bluetooth MAC)
 * @return device pointer or NULL if not found
 */
static Device *device_by_addr(const char *low_addr)
{
	GSList *i;
	Device *device = NULL;

	/* search for addr in current devices */
	for (i = devices; i; i = i->next) {
		Device *candidate = i->data;

		if (strcmp(candidate->addr, low_addr) == 0) {
			device = candidate;
			break;
		}
	}

	return device;
}

/**
 * Destroys a device object, given handle and/or device pointer
 * @param device Device object to destroy
 */
static void destroy_device(Device *device)
{
	if (!device)
		return;

	if (opmode != AUTOTESTING)
		dbus_g_connection_unregister_g_object(bus, G_OBJECT(device));

	ContextId z = {0, 0};

	g_free(device->path);
	g_free(device->addr);
	device->path = NULL;
	device->addr = NULL;
	device->handle = z;

	g_object_unref(device);
	devices = g_slist_remove(devices, device);
}

/**
 * Callback related to D-Bus service client disconnection.
 */
void client_disconnected()
{
	if (agent_proxy) {
		DEBUG("DBus client disconnected");
		g_object_unref(agent_proxy);
		g_free(client_agent);
		g_free(client_name);

		// TODO IMHO even if D-Bus client disconnected, it is not
		// a reason to remove all devices, because we may be in
		// the middle of a session and healthd must keep track of
		// devices even if no client is connected.
		if (0) {
			while (devices)
				destroy_device(devices->data);
		}

		agent_proxy = NULL;
		client_agent = NULL;
		client_name = NULL;
	}
}

/**
 * Creates a com.signove.Health.Device object.
 *
 * @param low_addr Device address (e.g. Bluetooth MAC)
 * @param conn_handle Context ID
 * @return a copy of object path (does not transfer ownership)
 */
static const char *get_device_object(const char *low_addr, ContextId conn_handle)
{
	static long int dev_counter = 0;

	Device *device = NULL;

	if (low_addr) {
		/* First search upon connection, normal to fail at first time */
		device = device_by_addr(low_addr);
	} else {
		/* This search should not fail */
		device = device_by_handle(conn_handle);

		if (!device) {
			DEBUG("SHOULD NOT HAPPEN: handle %u:%llx not found among devices",
				conn_handle.plugin, conn_handle.connid);
			return NULL;
		}
	}

	if (!device) {
		device = g_object_new(DEVICE_TYPE_OBJECT, NULL);
		device->addr = g_strdup(low_addr);
		if (asprintf(&(device->path), "%s/%ld",
				DEVICE_OBJECT_PATH, ++dev_counter) < 0) {
			// TODO handle error;
		}
		DEBUG("Create device object in %s", device->path);
		if (opmode != AUTOTESTING) {
			dbus_g_connection_register_g_object(bus, device->path,
						    	G_OBJECT(device));
		}
		devices = g_slist_prepend(devices, device);
	}

	device->handle = conn_handle;

	return device->path;
}

/**
 * Handles agent interface proxy destruction, when D-Bus client disappears.
 *
 * @param proxy D-Bus proxy
 * @param data Callback context pointer (unused)
 * @return success
 */
gboolean agent_proxy_destroyed(DBusGProxy *proxy, gpointer data)
{
	client_disconnected();
	return TRUE;
}

/**
 * Makes agent interface proxy in order to call methods later
 */
static void get_agent_proxy()
{
	GError *error = NULL;

	DEBUG("get_agent_proxy");

	agent_proxy = dbus_g_proxy_new_for_name_owner(bus, client_name,
			client_agent, AGENT_INTERFACE, &error);

	if (!agent_proxy) {
		DEBUG("Failed to get agent proxy: %s",
		      error->message);
		g_error_free(error);
		return;
	}

	g_signal_connect(G_OBJECT(agent_proxy),
			 "destroy",
			 G_CALLBACK(agent_proxy_destroyed),
			 NULL);
}


/************* Agent method call proxies ***************/

/**
 * Asynchronous agent call return handler.
 * Does nothing because calls to agent never return data.
 *
 * @param proxy D-Bus proxy
 * @param call D-Bus call struct
 * @param user_data Unused call context pointer
 */
static void call_agent_epilogue(DBusGProxy *proxy, DBusGProxyCall *call, gpointer user_data)
{

	GError *error = NULL;

	dbus_g_proxy_end_call(proxy, call, &error, DBUS_TYPE_INVALID);

	if (error != NULL) {
		DEBUG("Error calling D-Bus system bus");
		DEBUG("%s", error->message);
		g_error_free(error);
	}
}

/**
 * Function that calls D-Bus agent.Connected method.
 *
 * @param conn_handle Context ID
 * @param low_addr Device address e.g. Bluetooth MAC
 * @return TRUE if success
 */
static gboolean call_agent_connected(ContextId conn_handle, const char *low_addr)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_connected");

	device_path = get_device_object(low_addr, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) Device connected: %s\n\n",
							device_path);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		tcp_announce("CONNECTED", device_path, low_addr);
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "Connected",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_STRING, low_addr,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}

/**
 * Function that calls D-Bus agent.Associated method.
 *
 * @param conn_handle Context ID
 * @param xml Data in XML format
 * @return success status
 */
static gboolean call_agent_associated(ContextId conn_handle, char *xml)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_associated");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) Device associated: %s\n\n",
							device_path);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		tcp_announce("ASSOCIATED", device_path, "");
		tcp_announce("DESCRIPTION", device_path, xml);
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "Associated",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}

/**
 * Function that calls D-Bus agent.MeasurementData method.
 *
 * @param conn_handle device handle
 * @param xml Data in xml format
 * @return success status
 */
static gboolean call_agent_measurementdata(ContextId conn_handle, gchar *xml)
{
	/* Called back by new_data_received() */

	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_measurementdata");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) Measurement Data from %s\n\n%s\n\n",
							device_path, xml);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		tcp_announce("MEASUREMENT", device_path, xml);
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "MeasurementData",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}

/**
 * Function that calls D-Bus agent.SegmentInfo method.
 *
 * @param conn_handle Context ID
 * @param handle PM-Store handle
 * @param xml PM-Segment instance data in XML format
 * @return success status
 */
static gboolean call_agent_segmentinfo(ContextId conn_handle, guint handle, gchar *xml)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_segmentinfo");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) Segment Info from %s\nhandle %d\n%s\n\n",
						device_path, handle, xml);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		char *params;
		if (asprintf(&params, "%d %s", handle, xml) < 0) {
			return FALSE;
		}
		tcp_announce("SEGMENTINFO", device_path, params);
		free(params);
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "SegmentInfo",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INT, handle,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}


/**
 * Function that calls D-Bus agent.SegmentDataResponse method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param instnumber PM-Segment instance number
 * @param status Return status
 * @return success status
 */
static gboolean call_agent_segmentdataresponse(ContextId conn_handle,
			guint handle, guint instnumber, guint retstatus)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_segmentdataresponse");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) Segment Data Response from %s\n"
			"\thandle %d\n\tinstnumber %d retstatus %d\n\n",
			device_path, handle, instnumber, retstatus);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		char *params;
		if (asprintf(&params, "%d %d %d", handle, instnumber, retstatus) < 0) {
			return FALSE;
		}
		tcp_announce("SEGMENTDATARESPONSE", device_path, params);
		free(params);
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "SegmentDataResponse",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INT, handle,
				       G_TYPE_INT, instnumber,
				       G_TYPE_INT, retstatus,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}


/**
 * Function that calls D-Bus agent.SegmentData method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param instnumber PM-Segment instance number
 * @param xml PM-Segment instance data in XML format
 * @return success status
 */
static gboolean call_agent_segmentdata(ContextId conn_handle, guint handle,
					guint instnumber, gchar *xml)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_segmentdata");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) Segment Data from %s\n"
			"\thandle %d\n\tinstnumber %d\n%s\n\n",
			device_path, handle, instnumber, xml);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		char *params;
		if (asprintf(&params, "%d %d %s", handle, instnumber, xml) < 0) {
			return FALSE;
		}
		tcp_announce("SEGMENTDATA", device_path, params);
		free(params);
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "SegmentData",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INT, handle,
				       G_TYPE_INT, instnumber,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}


/**
 * Function that calls D-Bus agent.PMStoreData method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param xml PM-Store data attributes in XML format
 * @return success status
 */
static gboolean call_agent_pmstoredata(ContextId conn_handle, guint handle, gchar *xml)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_pmstoredata");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) PM-Store Data from %s\n"
			"\thandle %d\n%s\n\n",
			device_path, handle, xml);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		char *params;
		if (asprintf(&params, "%d %s", handle, xml) < 0) {
			return FALSE;
		}
		tcp_announce("PMSTOREDATA", device_path, params);
		free(params);
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "PMStoreData",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INT, handle,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}


/**
 * Function that calls D-Bus agent.SegmentCleared method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param PM-Segment instance number
 * @return success status
 */
static gboolean call_agent_segmentcleared(ContextId conn_handle, guint handle,
							guint instnumber,
							guint retstatus)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_segmentcleared");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) Segment Cleared from %s\n"
					"\thandle %d\n\tinstnumber %d\n\n",
					device_path, handle, instnumber);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		char *params;
		if (asprintf(&params, "%d %d %d", handle, instnumber, retstatus) < 0) {
			return FALSE;
		}
		tcp_announce("SEGMENTCLEARED", device_path, params);
		free(params);
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "SegmentCleared",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INT, handle,
				       G_TYPE_INT, instnumber,
				       G_TYPE_INT, retstatus,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}


/**
 * Function that calls D-Bus agent.DeviceAttributes method.
 *
 * @param conn_handle Context ID
 * @param xml Data in xml format
 * @return success status
 */
static gboolean call_agent_deviceattributes(ContextId conn_handle, gchar *xml)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_deviceattributes");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) Device Attributes from %s\n\n%s\n\n",
							device_path, xml);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		tcp_announce("ATTRIBUTES", device_path, xml);
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "DeviceAttributes",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}

/**
 * Function that calls D-Bus agent.Disassociated method.
 *
 * @param conn_handle Context ID
 * @return success status
 */
static gboolean call_agent_disassociated(ContextId conn_handle)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_disassociated");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) Device disassociate: %s\n\n",
							device_path);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		tcp_announce("DISASSOCIATE", device_path, "");
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "Disassociated",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}

/**
 * Function that calls D-Bus agent.Disconnected method.
 *
 * @param conn_handle Context ID
 * @param low_addr Device address e.g. Bluetooth MAC
 * @return success status
 */
static gboolean call_agent_disconnected(ContextId conn_handle, const char *low_addr)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_disconnected");

	device_path = get_device_object(low_addr, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return FALSE;
	}

	if (opmode == AUTOTESTING) {
		DEBUG("\n\n(Auto-test) Device disconnect: %s\n\n",
							device_path);
		return TRUE;
	} else if (opmode == TCP_SERVER) {
		tcp_announce("DISCONNECT", device_path, "");
		return TRUE;
	}

	if (!agent_proxy) {
		return FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "Disconnected",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return FALSE;
	}

	return TRUE;
}


#include "serv_dbus_api.h"

/** DBUS facade to connect
 *
 * \param obj
 * \param err
 */
gboolean device_connect(Device *obj, GError **err)
{
	DEBUG("device_connect");
	return TRUE;
}

/** DBUS facade to disconnect
 *
 * \param obj
 * \param err
 */
gboolean device_disconnect(Device *obj, GError **err)
{
	DEBUG("device_disconnect");
	return TRUE;
}


/**
 * Callback used to request mds attributes
 *
 * \param ctx Context
 * \param r Related request struct (the same returned to request caller)
 * \param response_apdu Data APDU
 */
static void device_reqmdsattr_callback(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	DEBUG("Medical Device Attributes");

	DataList *list = manager_get_mds_attributes(ctx->id);

	if (list) {
		char *data = xml_encode_data_list(list);
		if (data) {
			call_agent_deviceattributes(ctx->id, data);
			free(data);
		}
		data_list_del(list);
	}
}


/** DBUS facade to request mds attributes
 *
 * \param obj
 * \param err
 */
gboolean device_reqmdsattr(Device *obj, GError **err)
{
	DEBUG("device_reqmdsattr");
	manager_request_get_all_mds_attributes(obj->handle, device_reqmdsattr_callback);
	return TRUE;
}

/**
 * DBUS facade to get device configuration
 *
 * \param obj
 * \param xml_out pointer to string to be filled with result XML
 * \param err
 * \return TRUE if success
 */
gboolean device_getconfig(Device *obj, char** xml_out, GError **err)
{
	DataList *list;

	DEBUG("device_getconfig");
	list = manager_get_configuration(obj->handle);

	if (list) {
		*xml_out = xml_encode_data_list(list);
		data_list_del(list);
	} else {
		*xml_out = strdup("");
	}

	return TRUE;
}

/**
 * DBUS facade to request measuremens
 *
 * \param obj
 * \param err
 */
gboolean device_reqmeasurement(Device *obj, GError **err)
{
	DEBUG("device_reqmeasurement");
	manager_request_measurement_data_transmission(obj->handle, NULL);
	return TRUE;
}

/**
 * DBUS facade to activate scanner
 *
 * \param obj
 * \param handle Object handle
 * \param err
 */
gboolean device_reqactivationscanner(Device *obj, gint handle, GError **err)
{
	DEBUG("device_reqactivationscanner");
	manager_set_operational_state_of_the_scanner(obj->handle, (HANDLE) handle, os_enabled, NULL);
	return TRUE;
}

/**
 * DBUS facade to deactivate scanner
 *
 * \param obj
 * \param handle Object handle
 * \param err
 */
gboolean device_reqdeactivationscanner(Device *obj, gint handle, GError **err)
{
	DEBUG("device_reqdeactivationscanner");
	manager_set_operational_state_of_the_scanner(obj->handle, (HANDLE) handle, os_disabled, NULL);
	return TRUE;
}

/**
 * DBUS facade to release association
 *
 * \param obj
 * \param err
 */
gboolean device_releaseassoc(Device *obj, GError **err)
{
	DEBUG("device_releaseassoc");
	manager_request_association_release(obj->handle);
	return TRUE;
}

/**
 * DBUS facade to abort association
 *
 * \param obj
 * \param err
 */
gboolean device_abortassoc(Device *obj, GError **err)
{
	DEBUG("device_abortassoc");
	manager_request_association_release(obj->handle);
	return TRUE;
}

/*Callback for PM-Store GET
 *
 * \param ctx Context
 * \param r Request object
 * \param response_apdu response Data APDU
 */
static void device_get_pmstore_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreGetRet *ret = (PMStoreGetRet*) r->return_data;
	DataList *list;
	char *data;

	if (!ret)
		return;

	if ((list = manager_get_pmstore_data(ctx->id, ret->handle))) {
		if ((data = xml_encode_data_list(list))) {
			call_agent_pmstoredata(ctx->id, ret->handle, data);
			free(data);
			data_list_del(list);
		}
	}
}

/**
 * DBUS facade to get PM-Store attributes
 *
 * \param obj
 * \param handle PM-Store handle
 * \param ret Preliminary return status (actual data goes via Agent callback)
 * \param err
 */
gboolean device_get_pmstore(Device *obj, gint handle,
				gint* ret, GError **err)
{
	DEBUG("device_get_pmstore");
	manager_request_get_pmstore(obj->handle, handle, device_get_pmstore_cb);
	*ret = 0;
	return TRUE;
}

/*Callback for PM-Store get segment info response
 *
 * \param ctx
 * \param r Request object
 * \param response_apdu
 */
static void device_get_segminfo_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreGetSegmInfoRet *ret = (PMStoreGetSegmInfoRet*) r->return_data;
	DataList *list;
	char *data;

	if (!ret)
		return;

	if ((list = manager_get_segment_info_data(ctx->id, ret->handle))) {
		if ((data = xml_encode_data_list(list))) {
			call_agent_segmentinfo(ctx->id, ret->handle, data);
			free(data);
			data_list_del(list);
		}
	}
}

/*Callback for PM-Store segment data response
 *
 * \param ctx
 * \param r Request struct, the same originally returned to invoker
 * \param response_apdu Response data APDU
 */
static void device_get_segmdata_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreGetSegmDataRet *ret = (PMStoreGetSegmDataRet*) r->return_data;

	if (!ret)
		return;

	call_agent_segmentdataresponse(ctx->id, ret->handle, ret->inst, ret->response);
}

/*Callback for PM-Store clear segment response
 *
 * \param ctx
 * \param r Request struct, the same originally returned to invoker
 * \param response_apdu
 */
static void device_clear_segm_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreClearSegmRet *ret = (PMStoreClearSegmRet*) r->return_data;

	if (!ret)
		return;

	call_agent_segmentcleared(ctx->id, ret->handle, ret->inst, ret->response);
}

/**
 * DBUS facade to get segments info from a PM-Store
 *
 * \param obj
 * \param handle PM-Store handle
 * \param ret Preliminary return status (actual data goes via Agent callback)
 * \param err
 */
gboolean device_get_segminfo(Device *obj, gint handle,
				gint* ret, GError **err)
{
	Request *req;

	DEBUG("device_get_segminfo");
	req = manager_request_get_segment_info(obj->handle, handle,
						device_get_segminfo_cb);
	*ret = req ? 0 : 1;
	return TRUE;
}

/**
 * DBUS facade to get segments data from a PM-Segment
 *
 * \param obj
 * \param handle PM-Store handle
 * \param instnumber PM-Segment InstNumber
 * \param ret Preliminary return status (actual data goes via Agent callback)
 * \param err
 */
gboolean device_get_segmdata(Device *obj, gint handle, gint instnumber,
				gint* ret, GError **err)
{
	Request *req;

	DEBUG("device_get_segmdata");
	req = manager_request_get_segment_data(obj->handle, handle,
				instnumber, device_get_segmdata_cb);
	*ret = req ? 0 : 1;
	return TRUE;
}

/**
 * DBUS facade to clear a PM-store segment
 *
 * \param obj
 * \param handle PM-Store handle
 * \param instnumber PM-Segment InstNumber
 * \param ret Preliminary return status (actual data goes via Agent callback)
 * \param err
 */
gboolean device_clearsegmdata(Device *obj, gint handle, gint instnumber,
				gint *ret, GError **err)
{
	Request *req;

	DEBUG("device_clearsegmdata");
	req = manager_request_clear_segment(obj->handle, handle,
				instnumber, device_clear_segm_cb);
	*ret = req ? 0 : 1;
	return TRUE;
}

/**
 * DBUS facade to clear all segments of a PM-Store
 *
 * \param obj
 * \param handle PM-Store handle
 * \param ret Preliminary return status (actual data goes via Agent callback)
 * \param err
 */
gboolean device_clearallsegmdata(Device *obj, gint handle,
				gint *ret, GError **err)
{
	Request *req;

	DEBUG("device_clearsegmdata");
	req = manager_request_clear_segments(obj->handle, handle,
				device_clear_segm_cb);
	*ret = req ? 0 : 1;
	return TRUE;
}

#include "serv_dbus_api_device.h"

/**
 * \cond Undocumented
 */

static void srv_object_class_init(ServClass *klass)
{
	DEBUG(" ");
	g_assert(klass != NULL);
	dbus_g_object_type_install_info(SRV_TYPE_OBJECT,
					&dbus_glib_srv_object_info);
}

static void device_object_class_init(DeviceClass *klass)
{
	g_assert(klass != NULL);
	dbus_g_object_type_install_info(DEVICE_TYPE_OBJECT,
					&dbus_glib_device_object_info);
}

/**
 * \endcond
 */

static GMainLoop *mainloop = NULL;

static DBusGProxy *busProxy = NULL;
static Serv *srvObj = NULL;

/**
 * App clean-up in termination phase
 */
static void app_clean_up()
{
	g_main_loop_unref(mainloop);

	if (opmode == DBUS_SERVER) {
		g_object_unref(busProxy);
		g_object_unref(srvObj);
		dbus_g_connection_unref(bus);
	}
}

/**
 * Stops main loop (which causes application quit)
 * @param int Signal code that is terminating the app
 */
static void app_finalize(int sig)
{
	DEBUG("Exiting with signal (%d)", sig);
	g_main_loop_quit(mainloop);
}

/**
 * Sets up application signal handlers, linking them to app finalization
 */
static void app_setup_signals()
{
	signal(SIGINT, app_finalize);
	signal(SIGTERM, app_finalize);
}

/**
 * Main function
 * @param argc number of command-line arguments + 1
 * @param argv list of argument strings
 * @return status (0 = ok)
 */
int main(int argc, char *argv[])
{
	GError *error = NULL;
	guint result;

	CommunicationPlugin bt_plugin;
	CommunicationPlugin usb_plugin;
	CommunicationPlugin trans_plugin;

	CommunicationPlugin *plugins[] = {0, 0, 0, 0};
	int plugin_count = 0;

	int trans_support = 0;
	int usb_support = 0;
	int i;

	opmode = DBUS_SERVER;

	for (i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--autotest") == 0) {
			opmode = AUTOTESTING;
		} else if (strcmp(argv[i], "--autotesting") == 0) {
			opmode = AUTOTESTING;
		} else if (strcmp(argv[i], "--auto") == 0) {
			opmode = AUTOTESTING;
		} else if (strcmp(argv[i], "--tcp") == 0) {
			opmode = TCP_SERVER;
		} else if (strcmp(argv[i], "--tcpserver") == 0) {
			opmode = TCP_SERVER;
		} else if (strcmp(argv[i], "--bluez") == 0) {
		} else if (strcmp(argv[i], "--trans") == 0) {
			trans_support = 1;
#ifndef ANDROID_HEALTHD
		} else if (strcmp(argv[i], "--usb") == 0) {
			usb_support = 1;
#endif
		}
	}

	app_setup_signals();
	g_type_init();

	if (opmode == TCP_SERVER)
		tcp_listen();

	if (opmode != DBUS_SERVER)
		goto init_plugin;

	bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);

	if (error != NULL) {
		DEBUG("Could not get D-Bus system bus");
		DEBUG("%s", error->message);
		g_error_free(error);
		app_clean_up();
		exit(1);
	}

	busProxy = dbus_g_proxy_new_for_name(bus, DBUS_SERVICE_DBUS,
					     DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	if (busProxy == NULL) {
		DEBUG("Failed to get D-Bus proxy");
		app_clean_up();
		exit(1);
	}

	if (!dbus_g_proxy_call(busProxy, "RequestName", &error, G_TYPE_STRING,
			       SRV_SERVICE_NAME, G_TYPE_UINT, 0, G_TYPE_INVALID,
			       G_TYPE_UINT, &result, G_TYPE_INVALID)) {

		DEBUG("D-Bus.RequestName RPC failed: %s", error->message);
		app_clean_up();
		exit(1);
	}

	if (result != 1) {
		DEBUG("Failed to get the primary well-known name");
		app_clean_up();
		exit(1);
	}

	srvObj = g_object_new(SRV_TYPE_OBJECT, NULL);

	if (!srvObj) {
		DEBUG("Failed to create one Value instance.");
		app_clean_up();
		exit(1);
	}

	dbus_g_connection_register_g_object(bus, SRV_OBJECT_PATH,
					    G_OBJECT(srvObj));

init_plugin:
	bt_plugin = communication_plugin();
	trans_plugin = communication_plugin();
	usb_plugin = communication_plugin();

	plugin_bluez_setup(&bt_plugin);
	bluez_listener.peer_connected = call_agent_connected;
	bluez_listener.peer_disconnected = call_agent_disconnected;
	plugin_bluez_set_listener(&bluez_listener);

	bt_plugin.timer_count_timeout = timer_count_timeout;
	bt_plugin.timer_reset_timeout = timer_reset_timeout;
	plugins[plugin_count++] = &bt_plugin;

#ifndef ANDROID_HEALTHD
	if (usb_support) {
		plugin_usb_setup(&usb_plugin);
		usb_listener.agent_connected = call_agent_connected;
		usb_listener.agent_disconnected = call_agent_disconnected;
		plugin_usb_set_listener(&usb_listener);

		usb_plugin.timer_count_timeout = timer_count_timeout;
		usb_plugin.timer_reset_timeout = timer_reset_timeout;
		plugins[plugin_count++] = &usb_plugin;
	}
#endif

	if (trans_support) {
		plugin_trans_setup(&trans_plugin);
		trans_plugin.timer_count_timeout = timer_count_timeout;
		trans_plugin.timer_reset_timeout = timer_reset_timeout;
		plugins[plugin_count++] = &trans_plugin;
	}

	manager_init(plugins);

	ManagerListener listener = MANAGER_LISTENER_EMPTY;
	listener.measurement_data_updated = &new_data_received;
	listener.segment_data_received = &segment_data_received;
	listener.device_available = &device_associated;
	listener.device_unavailable = &device_disassociated;

	manager_add_listener(listener);
	manager_start();

	if (trans_support) {
		trans_plugin_oximeter_register(call_agent_connected,
						call_agent_disconnected);
	}

	if (opmode != DBUS_SERVER)
		self_configure();

	if (opmode == TCP_SERVER)
		tcp_listen();

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_ref(mainloop);
	g_main_loop_run(mainloop);
	DEBUG("Main loop stopped");
	manager_finalize();
	app_clean_up();
	dbus_shutdown();
	return 0;
}

/** @} */
