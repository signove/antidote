/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file healthd_android.c
 * \brief Health manager for Android
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
 * @addtogroup main
 * @{
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ieee11073.h>
#include "src/communication/plugin/bluez/plugin_android.h"
#include "src/util/log.h"
#include "src/communication/service.h"
#include "src/dim/pmstore_req.h"

static void notif_java_measurementdata(unsigned long long, char *);
static void notif_java_disassociated(unsigned long long);
static void notif_java_associated(unsigned long long, char *);
static void notif_java_segmentinfo(unsigned long long, guint, char *);
static void notif_java_segmentdataresponse(unsigned long long, guint, guint, guint);
static void notif_java_segmentdata(unsigned long long, guint, guint, char *);
static void notif_java_segmentcleared(unsigned long long, guint, guint, guint);
static void notif_java_pmstoredata(unsigned long long, guint, char *);


/* Called by IEEE library */

/**
 * Resets a framework-depende timer
 */
static void timer_reset_timeout(Context *ctx)
{
	if (ctx->timeout_action.id) {
		jni_cb_mgr_cancel_timer(ctx->timeout_action.id);
	}
}
/**
 * Timer callback.
 * Calls the supplied callback when timer reaches timeout, and cancels timer.
 *
 * @return FALSE (to cancel the timeout)
 */
static void jni_cb_mgr_timer_alarm(gpointer data) FIXME
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
 * @return The timer handle
 */
static int timer_count_timeout(Context *ctx)
{
	ctx->timeout_action.id = jni_cb_mgr_create_timer(ctx->timeout_action.timeout
					                 * 1000, ctx); FIXME
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
		notif_java_measurementdata(ctx->id, data);
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
static void segment_data_received_phase2(gpointer revt)
{
	segment_data_evt *evt = revt;

	DEBUG("PM-Segment Data phase 2");

	char *data = xml_encode_data_list(evt->list);

	if (data) {
		notif_java_segmentdata(evt->id, evt->handle, evt->instnumber, data);
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
	FIXME
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
		notif_java_associated(ctx->id, data);
		free(data);
	}
}

/**
 * Device has been disassociated
 */
void device_disassociated(Context *ctx)
{
	DEBUG("Device unassociated");
	notif_java_disassociated(ctx->id);
}

/************* Agent method call proxies ***************/

/**
 * Function that calls D-Bus agent.Associated method.
 *
 * @return success status
 */
static void notif_java_associated(unsigned long long conn_handle, char *xml)
{
	jni_cb_mgr_associated(conn_handle, xml);
}

/**
 * Function that calls D-Bus agent.MeasurementData method.
 *
 * @param conn_handle device handle
 * @param xml Data in xml format
 * @return success status
 */
static void notif_java_measurementdata(unsigned long long conn_handle, gchar *xml)
{
	jni_cb_mgr_measurementdata(conn_handle, xml);
}

/**
 * Function that calls D-Bus agent.SegmentInfo method.
 *
 * @param handle PM-Store handle
 * @param xml PM-Segment instance data in XML format
 * @return success status
 */
static void notif_java_segmentinfo(unsigned long long conn_handle, guint handle, gchar *xml)
{
	jni_cb_mgr_segmentinfo(conn_handle, handle, xml);
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
static void notif_java_segmentdataresponse(unsigned long long conn_handle,
			guint handle, guint instnumber, guint retstatus)
{
	jni_cb_mgr_segmentdataresponse(conn_handle, handle, instnumber, retstatus);
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
static void notif_java_segmentdata(unsigned long long conn_handle, guint handle,
					guint instnumber, gchar *xml)
{
	jni_cb_mgr_segmentdata(conn_handle, handle, instnumber, xml);
}


/**
 * Function that calls D-Bus agent.PMStoreData method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param xml PM-Store data attributes in XML format
 * @return success status
 */
static void notif_java_pmstoredata(unsigned long long conn_handle, guint handle, gchar *xml)
{
	jni_cb_mgr_pmstoredata(conn_handle, handle, xml);
}


/**
 * Function that calls D-Bus agent.SegmentCleared method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param PM-Segment instance number
 * @return success status
 */
static void notif_java_segmentcleared(unsigned long long conn_handle, guint handle,
							guint instnumber,
							guint retstatus)
{
	jni_cb_mgr_segmentcleared(conn_handle, handle, instnumber, retstatus);
}


/**
 * Function that calls D-Bus agent.DeviceAttributes method.
 *
 * @return success status
 */
static void notif_java_deviceattributes(unsigned long long conn_handle, gchar *xml)
{
	jni_cb_mgr_deviceattributes(conn_handle, handle, xml);
}

/**
 * Function that calls D-Bus agent.Disassociated method.
 *
 * @return success status
 */
static void notif_java_disassociated(unsigned long long conn_handle)
{
	jni_cb_mgr_disassociated(conn_handle);
}


/*Callback used to request mds attributes
 *
 *\param ctx
 *\param response_apdu
 * */
static void device_reqmdsattr_callback(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	DEBUG("Medical Device Attributes");

	DataList *list = manager_get_mds_attributes(ctx->id);

	if (list) {
		char *data = xml_encode_data_list(list);
		if (data) {
			notif_java_deviceattributes(ctx->id, data);
			free(data);
		}
		data_list_del(list);
	}
}


/*interface to request mds attributes
 *
 *\param obj
 *\param err
 * */
void jni_mgr_reqmdsattr(unsigned long long handle)
{
	DEBUG("device_reqmdsattr");
	manager_request_get_all_mds_attributes(handle, device_reqmdsattr_callback);
	return TRUE;
}

/*interface to get device configuration
 *
 *\param obj
 *\param err
 * */
void jni_mgr_getconfig(unsigned long long handle, char** xml_out)
{
	DataList *list;

	DEBUG("device_getconfig");
	list = manager_get_configuration(handle);

	if (list) {
		*xml_out = xml_encode_data_list(list);
		data_list_del(list);
	} else {
		*xml_out = strdup("");
	}

	return TRUE;
}

/*interface to request measuremens
 *
 *\param obj
 *\param err
 * */
void jni_mgr_reqmeasurement(unsigned long long handle)
{
	DEBUG("device_reqmeasurement");
	manager_request_measurement_data_transmission(handle, NULL);
	return TRUE;
}

/*interface to activate scanner
 *
 *\param obj
 *\param err
 * */
void jni_mgr_reqactivationscanner(unsigned long long handle, int ihandle)
{
	DEBUG("device_reqactivationscanner");
	manager_set_operational_state_of_the_scanner(handle, (HANDLE) ihandle, os_enabled, NULL);
	return TRUE;
}

/*interface to deactivate scanner
 *
 *\param obj
 *\param err
 * */
void jni_mgr_reqdeactivationscanner(unsigned long long handle, int ihandle)
{
	DEBUG("device_reqdeactivationscanner");
	manager_set_operational_state_of_the_scanner(handle, (HANDLE) ihandle, os_disabled, NULL);
	return TRUE;
}

/*interface to release association
 *
 *\param obj
 *\param err
 * */
void jni_mgr_releaseassoc(unsigned long long handle)
{
	DEBUG("device_releaseassoc");
	manager_request_association_release(handle);
	return TRUE;
}

/*interface to abort association
 *
 *\param obj
 *\param err
 * */
void jni_mgr_abortassoc(unsigned long long handle)
{
	DEBUG("device_abortassoc");
	manager_request_association_release(handle);
	return TRUE;
}

/*Callback for PM-Store GET
 *
 *\param ctx
 *\param r Request object
 *\param response_apdu
 * */
static void device_get_pmstore_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreGetRet *ret = (PMStoreGetRet*) r->return_data;
	DataList *list;
	char *data;

	if (!ret)
		return;

	if ((list = manager_get_pmstore_data(ctx->id, ret->handle))) {
		if ((data = xml_encode_data_list(list))) {
			notif_java_pmstoredata(ctx->id, ret->handle, data);
			free(data);
			data_list_del(list);
		}
	}
}

/*interface to get PM-Store attributes
 *
 *\param obj
 *\param handle PM-Store handle
 *\param ret Preliminary return status (actual data goes via Agent callback)
 *\param err
 * */
void jni_mgr_get_pmstore(unsigned long long handle, int ihandle,
				int* ret)
{
	DEBUG("device_get_pmstore");
	manager_request_get_pmstore(handle, ihandle, device_get_pmstore_cb);
	*ret = 0;
	return TRUE;
}

/*Callback for PM-Store get segment info response
 *
 *\param ctx
 *\param r Request object
 *\param response_apdu
 * */
static void device_get_segminfo_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreGetSegmInfoRet *ret = (PMStoreGetSegmInfoRet*) r->return_data;
	DataList *list;
	char *data;

	if (!ret)
		return;

	if ((list = manager_get_segment_info_data(ctx->id, ret->handle))) {
		if ((data = xml_encode_data_list(list))) {
			notif_java_segmentinfo(ctx->id, ret->handle, data);
			free(data);
			data_list_del(list);
		}
	}
}

/*Callback for PM-Store segment data response
 *
 *\param ctx
 *\param response_apdu
 * */
static void device_get_segmdata_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreGetSegmDataRet *ret = (PMStoreGetSegmDataRet*) r->return_data;

	if (!ret)
		return;

	notif_java_segmentdataresponse(ctx->id, ret->handle, ret->inst, ret->response);
}

/*Callback for PM-Store clear segment response
 *
 *\param ctx
 *\param response_apdu
 * */
static void device_clear_segm_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreClearSegmRet *ret = (PMStoreClearSegmRet*) r->return_data;

	if (!ret)
		return;

	notif_java_segmentcleared(ctx->id, ret->handle, ret->inst, ret->response);
}

/* interface to get segments info from a PM-Store
 *
 *\param obj
 *\param handle PM-Store handle
 *\param ret Preliminary return status (actual data goes via Agent callback)
 *\param err
 * */
void jni_mgr_get_segminfo(unsigned long long handle, int ihandle,
				int* ret)
{
	Request *req;

	DEBUG("device_get_segminfo");
	req = manager_request_get_segment_info(handle, ihandle,
						device_get_segminfo_cb);
	*ret = req ? 0 : 1;
	return TRUE;
}

/*interface to get segments data from a PM-Segment
 *
 *\param obj
 *\param handle PM-Store handle
 *\param instnumber PM-Segment InstNumber
 *\param ret Preliminary return status (actual data goes via Agent callback)
 *\param err
 * */
void jni_mgr_get_segmdata(unsigned long long handle, int ihandle, int instnumber,
				int* ret)
{
	Request *req;

	DEBUG("device_get_segmdata");
	req = manager_request_get_segment_data(handle, ihandle,
				instnumber, device_get_segmdata_cb);
	*ret = req ? 0 : 1;
	return TRUE;
}

/*interface to clear a PM-store segment
 *
 *\param obj
 *\param handle PM-Store handle
 *\param instnumber PM-Segment InstNumber
 *\param ret Preliminary return status (actual data goes via Agent callback)
 *\param err
 * */
void jni_mgr_clearsegmdata(unsigned long long handle, int ihandle, int instnumber,
				int *ret)
{
	Request *req;

	DEBUG("device_clearsegmdata");
	req = manager_request_clear_segment(handle, ihandle,
				instnumber, device_clear_segm_cb);
	*ret = req ? 0 : 1;
	return TRUE;
}

/* facade to clear all segments of a PM-Store
 *
 *\param obj
 *\param handle PM-Store handle
 *\param ret Preliminary return status (actual data goes via Agent callback)
 *\param err
 * */
void jni_mgr_clearallsegmdata(unsigned long long handle, int ihandle,
				int *ret)
{
	Request *req;

	DEBUG("device_clearsegmdata");
	req = manager_request_clear_segments(handle, ihandle,
				device_clear_segm_cb);
	*ret = req ? 0 : 1;
	return TRUE;
}

/**
 * Main function
 * @return int
 */
void jni_mgr_healthd_init()
{
	CommunicationPlugin plugin;

	plugin = communication_plugin();

	plugin_android_setup(&plugin);
	plugin.timer_count_timeout = timer_count_timeout;
	plugin.timer_reset_timeout = timer_reset_timeout;

	manager_init(&plugin);

	ManagerListener listener = MANAGER_LISTENER_EMPTY;
	listener.measurement_data_updated = &new_data_received;
	listener.segment_data_received = &segment_data_received;
	listener.device_available = &device_associated;
	listener.device_unavailable = &device_disassociated;

	manager_add_listener(listener);
	manager_start();
}

void jni_mgr_healthd_finalize()
{
	manager_finalize();
}

/** @} */
