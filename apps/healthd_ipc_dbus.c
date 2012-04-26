/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file healthd_ipc_dbus.c
 * \brief Health manager service - DBUS API
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
 * \author Walter Guerra
 * \author Fabricio Silva
 * \date Apr 18, 2012
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
#include "src/communication/context_manager.h"
#include "src/util/log.h"
#include "src/util/linkedlist.h"
#include "src/dim/pmstore_req.h"
#include "healthd_ipc.h"
#include "healthd_ipc_dbus.h"
#include "healthd_common.h"
#include "healthd_service.h"

#define SRV_SERVICE_NAME "com.signove.health"
#define SRV_OBJECT_PATH "/com/signove/health"
#define SRV_INTERFACE "com.signove.health.manager"

#define DEVICE_OBJECT_PATH "/com/signove/health/device"
#define DEVICE_INTERFACE "com.signove.health.device"

#define AGENT_INTERFACE "com.signove.health.agent"

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

static LinkedList *_devices = NULL;

static DBusGConnection *bus = NULL;
static DBusGProxy *agent_proxy = NULL;

static const char *get_device_object(const char *, ContextId);
static void get_agent_proxy();

static LinkedList *devices()
{
	if (! _devices) {
		_devices = llist_new();
	}
	return _devices;
}

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
	unsigned int i;
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

	hdp_types_configure(hdp_data_types);

	g_free(hdp_data_types);

	return TRUE;
}

static int cmp_device_by_handle(void *arg, void *nodeElement) 
{
	ContextId handle = *((ContextId *) arg);
	Device *candidate = (Device *) nodeElement;

	if (candidate->handle.plugin == handle.plugin &&
	    candidate->handle.connid == handle.connid) {
		return 1;
	}
	return 0;
}

/**
 * Finds device object given handle
 *
 * @param handle Context ID
 * @return device pointer or NULL if not found
 */
static Device *device_by_handle(ContextId handle)
{
	/* search for conn handle in current devices */
	Device *dev = llist_search_first(devices(), &handle, cmp_device_by_handle);
	return dev;
}


static int cmp_device_by_addr(void *arg, void *nodeElement) 
{
	const char *low_addr = (const char *) arg;
	Device *candidate = (Device *) nodeElement;
	
	if (strcmp(low_addr, candidate->addr) == 0) {
		return 1;
	}
	return 0;
}

/**
 * Finds device object given an address
 *
 * @param low_addr device address (e.g. Bluetooth MAC)
 * @return device pointer or NULL if not found
 */
static Device *device_by_addr(const char *low_addr)
{
	/* search for addr in current devices */
	
	Device *dev = llist_search_first(devices(), 
				(void *) low_addr,
				cmp_device_by_addr);
	return dev;
}
/**
 * Destroys a device object, given handle and/or device pointer
 * @param device Device object to destroy
 */
static void destroy_device(Device *device)
{
	if (!device)
		return;

	dbus_g_connection_unregister_g_object(bus, G_OBJECT(device));

	ContextId z = {0, 0};

	g_free(device->path);
	g_free(device->addr);
	device->path = NULL;
	device->addr = NULL;
	device->handle = z;

	g_object_unref(device);
	llist_remove(devices(), device);
}

/**
 * Callback related to D-Bus service client disconnection.
 */
static void client_disconnected()
{
	if (agent_proxy) {
		DEBUG("DBus client disconnected");
		g_object_unref(agent_proxy);
		g_free(client_agent);
		g_free(client_name);

		// IMHO even if D-Bus client disconnected, it is not
		// a reason to remove all devices, because we may be in
		// the middle of a session and healthd must keep track of
		// devices even if no client is connected.
	
		if (0) {
			while (devices()->first) {
				destroy_device(devices()->first->element);
			}

			llist_destroy(devices(), NULL);

			_devices = NULL;
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
			DEBUG("SHOULD NOT HAPPEN: handle %u:%llu not found among devices",
				conn_handle.plugin, conn_handle.connid);
			return NULL;
		}
	}

	if (!device) {
		device = g_object_new(DEVICE_TYPE_OBJECT, NULL);
		device->addr = g_strdup(low_addr);
		if (asprintf(&(device->path), "%s/%ld",
				DEVICE_OBJECT_PATH, ++dev_counter) < 0) {
			g_free(device->addr);
			g_object_unref(device);
			return NULL;
		}
		DEBUG("Create device object in %s", device->path);
		dbus_g_connection_register_g_object(bus, device->path,
					    	G_OBJECT(device));
		llist_add(devices(), device);
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
static gboolean agent_proxy_destroyed(DBusGProxy *proxy, gpointer data)
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
 * @param ctx Context ID
 * @param low_addr Device address e.g. Bluetooth MAC
 * @return TRUE if success
 */
static void call_agent_connected(ContextId ctx, const char *low_addr)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_connected");

	device_path = get_device_object(low_addr, ctx);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "Connected",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_STRING, low_addr,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return; // FALSE;
	}

	// return TRUE;
}

/**
 * Function that calls D-Bus agent.Associated method.
 *
 * @param conn_handle Context ID
 * @param xml Data in XML format
 */
static void call_agent_associated(ContextId conn_handle, char *xml)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_associated");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "Associated",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return; // FALSE;
	}

	// return TRUE;
}

/**
 * Function that calls D-Bus agent.MeasurementData method.
 *
 * @param conn_handle device handle
 * @param xml Data in xml format
 */
static void call_agent_measurementdata(ContextId conn_handle, char *xml)
{
	/* Called back by new_data_received() */

	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_measurementdata");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "MeasurementData",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return; // FALSE;
	}

	// return TRUE;
}

/**
 * Function that calls D-Bus agent.SegmentInfo method.
 *
 * @param conn_handle Context ID
 * @param handle PM-Store handle
 * @param xml PM-Segment instance data in XML format
 */
static void call_agent_segmentinfo(ContextId conn_handle, unsigned int handle, char *xml)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_segmentinfo");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "SegmentInfo",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INT, handle,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return; // FALSE;
	}

	// return TRUE;
}


/**
 * Function that calls D-Bus agent.SegmentDataResponse method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param instnumber PM-Segment instance number
 * @param status Return status
 */
static void call_agent_segmentdataresponse(ContextId conn_handle,
			unsigned int handle, unsigned int instnumber, unsigned int retstatus)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_segmentdataresponse");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
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
		return; // FALSE;
	}

	// return TRUE;
}


/**
 * Function that calls D-Bus agent.SegmentData method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param instnumber PM-Segment instance number
 * @param xml PM-Segment instance data in XML format
 */
static void call_agent_segmentdata(ContextId conn_handle, unsigned int handle,
					unsigned int instnumber, char *xml)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_segmentdata");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
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
		return; // FALSE;
	}

	// return TRUE;
}


/**
 * Function that calls D-Bus agent.PMStoreData method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param xml PM-Store data attributes in XML format
 */
static void call_agent_pmstoredata(ContextId conn_handle, unsigned int handle, char *xml)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_pmstoredata");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "PMStoreData",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INT, handle,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return; // FALSE;
	}

	// return TRUE;
}


/**
 * Function that calls D-Bus agent.SegmentCleared method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param PM-Segment instance number
 */
static void call_agent_segmentcleared(ContextId conn_handle, unsigned int handle,
							unsigned int instnumber,
							unsigned int retstatus)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_segmentcleared");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
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
		return; // FALSE;
	}

	// return TRUE;
}


/**
 * Function that calls D-Bus agent.DeviceAttributes method.
 *
 * @param conn_handle Context ID
 * @param xml Data in xml format
 * @return success status
 */
static void call_agent_deviceattributes(ContextId conn_handle, char *xml)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_deviceattributes");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "DeviceAttributes",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_STRING, xml,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return; // FALSE;
	}

	// return TRUE;
}

/**
 * Function that calls D-Bus agent.Disassociated method.
 *
 * @param conn_handle Context ID
 */
static void call_agent_disassociated(ContextId conn_handle)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_disassociated");

	device_path = get_device_object(NULL, conn_handle);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "Disassociated",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return; // FALSE;
	}

	// return TRUE;
}

/**
 * Function that calls D-Bus agent.Disconnected method.
 *
 * @param conn_handle Context ID
 * @param low_addr Device address e.g. Bluetooth MAC
 */
static void call_agent_disconnected(ContextId ctx, const char *low_addr)
{
	DBusGProxyCall *call;
	const char *device_path;

	DEBUG("call_agent_disconnected");

	device_path = get_device_object(low_addr, ctx);

	if (!device_path) {
		DEBUG("No device associated with handle!");
		return; // FALSE;
	}

	if (!agent_proxy) {
		return; // FALSE;
	}

	call = dbus_g_proxy_begin_call(agent_proxy, "Disconnected",
				       call_agent_epilogue, NULL, NULL,
				       G_TYPE_STRING, device_path,
				       G_TYPE_INVALID, G_TYPE_INVALID);

	if (!call) {
		DEBUG("error calling agent");
		return; // FALSE;
	}

	// return TRUE;
}


#include "serv_dbus_api.h"

/** DBUS facade to connect
 *
 * \param obj
 * \param err
 */
gboolean dbus_device_connect(Device *obj, GError **err)
{
	DEBUG("device_connect");
	return TRUE;
}

/** DBUS facade to disconnect
 *
 * \param obj
 * \param err
 */
gboolean dbus_device_disconnect(Device *obj, GError **err)
{
	DEBUG("device_disconnect");
	return TRUE;
}

/** DBUS facade to request mds attributes
 *
 * \param obj
 * \param err
 */
gboolean dbus_device_reqmdsattr(Device *obj, GError **err)
{
	DEBUG("dbus_device_reqmdsattr");
	device_reqmdsattr(obj->handle);
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
gboolean dbus_device_getconfig(Device *obj, char** xml_out, GError **err)
{
	DEBUG("dbus_device_getconfig");
	device_getconfig(obj->handle, xml_out);
	return TRUE;
}

/**
 * DBUS facade to request measuremens
 *
 * \param obj
 * \param err
 */
gboolean dbus_device_reqmeasurement(Device *obj, GError **err)
{
	DEBUG("dbus_device_reqmeasurement");
	device_reqmeasurement(obj->handle);
	return TRUE;
}

/**
 * DBUS facade to request set-time
 *
 * \param obj
 * \param time in time_t format
 * \param err
 */
gboolean dbus_device_set_time(Device *obj, guint64 time, GError **err)
{
	DEBUG("device_set_time");
	device_set_time(obj->handle, time);
	return TRUE;
}

/**
 * DBUS facade to activate scanner
 *
 * \param obj
 * \param handle Object handle
 * \param err
 */
gboolean dbus_device_reqactivationscanner(Device *obj, gint handle, GError **err)
{
	DEBUG("dbus_device_reqactivationscanner");
	device_reqactivationscanner(obj->handle, handle);
	return TRUE;
}

/**
 * DBUS facade to deactivate scanner
 *
 * \param obj
 * \param handle Object handle
 * \param err
 */
gboolean dbus_device_reqdeactivationscanner(Device *obj, gint handle, GError **err)
{
	DEBUG("dbus_device_reqdeactivationscanner");
	device_reqdeactivationscanner(obj->handle, handle);
	return TRUE;
}

/**
 * DBUS facade to release association
 *
 * \param obj
 * \param err
 */
gboolean dbus_device_releaseassoc(Device *obj, GError **err)
{
	DEBUG("dbus_device_releaseassoc");
	device_releaseassoc(obj->handle);
	return TRUE;
}

/**
 * DBUS facade to abort association
 *
 * \param obj
 * \param err
 */
gboolean dbus_device_abortassoc(Device *obj, GError **err)
{
	DEBUG("dbus_device_abortassoc");
	device_abortassoc(obj->handle);
	return TRUE;
}

/**
 * DBUS facade to get PM-Store attributes
 *
 * \param obj
 * \param handle PM-Store handle
 * \param ret Preliminary return status (actual data goes via Agent callback)
 * \param err
 */
gboolean dbus_device_get_pmstore(Device *obj, gint handle,
				gint* ret, GError **err)
{
	DEBUG("dbus_device_get_pmstore");
	device_get_pmstore(obj->handle, handle, ret);
	return TRUE;
}

/**
 * DBUS facade to get segments info from a PM-Store
 *
 * \param obj
 * \param handle PM-Store handle
 * \param ret Preliminary return status (actual data goes via Agent callback)
 * \param err
 */
gboolean dbus_device_get_segminfo(Device *obj, gint handle,
				gint* ret, GError **err)
{
	DEBUG("dbus_device_get_segminfo");
	device_get_segminfo(obj->handle, handle, ret);
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
gboolean dbus_device_get_segmdata(Device *obj, gint handle, gint instnumber,
				gint* ret, GError **err)
{
	DEBUG("dbus_device_get_segmdata");
	device_get_segmdata(obj->handle, handle, instnumber, ret);
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
gboolean dbus_device_clearsegmdata(Device *obj, gint handle, gint instnumber,
						gint *ret, GError **err)
{
	DEBUG("dbus_device_clearsegmdata");
	device_clearsegmdata(obj->handle, handle, instnumber, ret);
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
gboolean dbus_device_clearallsegmdata(Device *obj, gint handle,
					gint *ret, GError **err)
{
	DEBUG("dbus_device_clearsegmdata");
	device_clearallsegmdata(obj->handle, handle, ret);
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

static DBusGProxy *busProxy = NULL;
static Serv *srvObj = NULL;

static void stop()
{
	g_object_unref(busProxy);
	g_object_unref(srvObj);
	dbus_g_connection_unref(bus);

	// dbus_shutdown();
}

static void start()
{
	GError *error = 0;
	int result;

	// called at main
	// g_type_init();

	bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);

	if (error != NULL) {
		DEBUG("Could not get D-Bus system bus");
		DEBUG("%s", error->message);
		exit(1);
	}

	busProxy = dbus_g_proxy_new_for_name(bus, DBUS_SERVICE_DBUS,
					     DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	if (busProxy == NULL) {
		DEBUG("Failed to get D-Bus proxy");
		exit(1);
	}

	if (!dbus_g_proxy_call(busProxy, "RequestName", &error, G_TYPE_STRING,
			       SRV_SERVICE_NAME, G_TYPE_UINT, 0, G_TYPE_INVALID,
			       G_TYPE_UINT, &result, G_TYPE_INVALID)) {

		DEBUG("D-Bus.RequestName RPC failed: %s", error->message);
		exit(1);
	}

	if (result != 1) {
		DEBUG("Failed to get the primary well-known name");
		exit(1);
	}

	srvObj = g_object_new(SRV_TYPE_OBJECT, NULL);

	if (!srvObj) {
		DEBUG("Failed to create one Value instance.");
		exit(1);
	}

	dbus_g_connection_register_g_object(bus, SRV_OBJECT_PATH,
					    G_OBJECT(srvObj));
}

void healthd_ipc_dbus_init(healthd_ipc *ipc)
{
	ipc->call_agent_measurementdata = call_agent_measurementdata;
	ipc->call_agent_connected = &call_agent_connected;
	ipc->call_agent_disconnected = &call_agent_disconnected;
	ipc->call_agent_associated = &call_agent_associated;
	ipc->call_agent_disassociated = &call_agent_disassociated;
	ipc->call_agent_segmentinfo = &call_agent_segmentinfo;
	ipc->call_agent_segmentdataresponse = &call_agent_segmentdataresponse;
	ipc->call_agent_segmentdata = &call_agent_segmentdata;
	ipc->call_agent_segmentcleared = &call_agent_segmentcleared;
	ipc->call_agent_pmstoredata = &call_agent_pmstoredata;
	ipc->call_agent_deviceattributes = &call_agent_deviceattributes;
	ipc->start = &start;
	ipc->stop = &stop;
}

/** @} */
