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
#include <jni.h>
#include "src/communication/plugin/android/plugin_android.h"
#include "src/util/log.h"
#include "src/communication/service.h"
#include "src/dim/pmstore_req.h"

JNIEnv *env = 0;
jclass cls = 0;
jobject obj = 0;
jmethodID jni_up_cancel_timer = 0;
jmethodID jni_up_create_timer = 0;
jmethodID jni_up_associated = 0;
jmethodID jni_up_disassociated = 0;
jmethodID jni_up_deviceattributes = 0;
jmethodID jni_up_measurementdata = 0;
// FIXME fill up
jmethodID jni_up_pmstoredata = 0;
jmethodID jni_up_segmentinfo = 0;
jmethodID jni_up_segmentdataresponse = 0;
jmethodID jni_up_segmentdata = 0;
jmethodID jni_up_segmentcleared = 0;

static void notif_java_measurementdata(unsigned long long, char *);
static void notif_java_disassociated(unsigned long long);
static void notif_java_associated(unsigned long long, char *);
static void notif_java_segmentinfo(unsigned long long, int, char *);
static void notif_java_segmentdataresponse(unsigned long long, int, int, int);
static void notif_java_segmentdata(unsigned long long, int, int, char *);
static void notif_java_segmentcleared(unsigned long long, int, int, int);
static void notif_java_pmstoredata(unsigned long long, int, char *);


/* Called by IEEE library */

/**
 * Resets a framework-depende timer
 */
static void timer_reset_timeout(Context *ctx)
{
	if (ctx->timeout_action.id) {
		jint handle = ctx->timeout_action.id;
		(*env)->CallVoidMethod(env, obj, jni_up_cancel_timer, handle);
	}
}
/**
 * Timer callback.
 * Calls the supplied callback when timer reaches timeout, and cancels timer.
 *
 */
void Java_com_signove_health_healthservice_JniBridge_timer_alarm(JNIEnv *env, jobject obj, jint id)
{
	DEBUG("timer_alarm");
	Context *ctx = context_get(id);
	if (ctx) {
		void (*f)() = ctx->timeout_action.func;
		if (f) {
			f(ctx);
		}
	}
}

/**
 * Initiates a timer in behalf of IEEE library
 *
 * @return The timer handle
 */
static int timer_count_timeout(Context *ctx)
{
	ctx->timeout_action.id = (int) (*env)->CallIntMethod(env, obj, jni_up_create_timer,
					ctx->timeout_action.timeout * 1000, ctx->id);
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
/*
// FIXME
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
*/


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

	// g_idle_add(segment_data_received_phase2, evt);
	// FIXME
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
	jstring jxml = (*env)->NewStringUTF(env, xml);
	(*env)->CallVoidMethod(env, obj, jni_up_associated, (jint) conn_handle, jxml);
}

/**
 * Function that calls D-Bus agent.MeasurementData method.
 *
 * @param conn_handle device handle
 * @param xml Data in xml format
 * @return success status
 */
static void notif_java_measurementdata(unsigned long long conn_handle, char *xml)
{
	jstring jxml = (*env)->NewStringUTF(env, xml);
	(*env)->CallVoidMethod(env, obj, jni_up_measurementdata, (jint) conn_handle, jxml);
}

/**
 * Function that calls D-Bus agent.SegmentInfo method.
 *
 * @param handle PM-Store handle
 * @param xml PM-Segment instance data in XML format
 * @return success status
 */
static void notif_java_segmentinfo(unsigned long long conn_handle, int handle, char *xml)
{
	// (*env)->CallVoidMethod(env, obj, jni_up_segmentinfo(conn_handle, handle, xml);
	// FIXME
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
			int handle, int instnumber, int retstatus)
{
	// (*env)->CallVoidMethod(env, obj, jni_up_segmentdataresponse(conn_handle, handle, instnumber, retstatus);
	// FIXME
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
static void notif_java_segmentdata(unsigned long long conn_handle, int handle,
					int instnumber, char *xml)
{
	// (*env)->CallVoidMethod(env, obj, jni_up_segmentdata(conn_handle, handle, instnumber, jxml);
	// FIXME
}


/**
 * Function that calls D-Bus agent.PMStoreData method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param xml PM-Store data attributes in XML format
 * @return success status
 */
static void notif_java_pmstoredata(unsigned long long conn_handle, int handle, char *xml)
{
	// (*env)->CallVoidMethod(env, obj, jni_up_pmstoredata(conn_handle, handle, jxml);
	// FIXME
}


/**
 * Function that calls D-Bus agent.SegmentCleared method.
 *
 * @param conn_handle device handle
 * @param handle PM-Store handle
 * @param PM-Segment instance number
 * @return success status
 */
static void notif_java_segmentcleared(unsigned long long conn_handle, int handle,
							int instnumber,
							int retstatus)
{
	// (*env)->CallVoidMethod(env, obj, jni_up_segmentcleared(conn_handle, handle, instnumber, retstatus);
	// FIXME
}


/**
 * Function that calls D-Bus agent.DeviceAttributes method.
 *
 * @return success status
 */
static void notif_java_deviceattributes(unsigned long long conn_handle, char *xml)
{
	jstring jxml = (*env)->NewStringUTF(env, xml);
	(*env)->CallVoidMethod(env, obj, jni_up_deviceattributes, (jint) conn_handle, jxml);
}

/**
 * Function that calls D-Bus agent.Disassociated method.
 *
 * @return success status
 */
static void notif_java_disassociated(unsigned long long conn_handle)
{
	(*env)->CallVoidMethod(env, obj, jni_up_disassociated, (jint) conn_handle);
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
void Java_com_signove_health_healthservice_JniBridge_reqmdsattr(JNIEnv *env, jobject obj, jint handle)
{
	DEBUG("device_reqmdsattr");
	manager_request_get_all_mds_attributes(handle, device_reqmdsattr_callback);
}

/*interface to get device configuration
 *
 *\param obj
 *\param err
 * */
jstring Java_com_signove_health_healthservice_JniBridge_getconfig(JNIEnv *env, jobject obj, jint handle)
{
	DataList *list;
	char *xml_out;

	DEBUG("device_getconfig");
	list = manager_get_configuration(handle);

	if (list) {
		xml_out = xml_encode_data_list(list);
		data_list_del(list);
	} else {
		xml_out = strdup("");
	}

	jstring jxml = (*env)->NewStringUTF(env, xml_out);
	free(xml_out);

	return jxml;
}

/*interface to request measuremens
 *
 *\param obj
 *\param err
 * */
void Java_com_signove_health_healthservice_JniBridge_reqmeasurement(JNIEnv *env, jobject obj, jint handle)
{
	DEBUG("device_reqmeasurement");
	manager_request_measurement_data_transmission(handle, NULL);
}

/*interface to activate scanner
 *
 *\param obj
 *\param err
 * */
void Java_com_signove_health_healthservice_JniBridge_reqactivationscanner(JNIEnv *env, jobject obj, jint handle, jint ihandle)
{
	DEBUG("device_reqactivationscanner");
	manager_set_operational_state_of_the_scanner(handle, (HANDLE) ihandle, os_enabled, NULL);
}

/*interface to deactivate scanner
 *
 *\param obj
 *\param err
 * */
void Java_com_signove_health_healthservice_JniBridge_reqdeactivationscanner(JNIEnv *env, jobject obj, jint handle, jint ihandle)
{
	DEBUG("device_reqdeactivationscanner");
	manager_set_operational_state_of_the_scanner(handle, (HANDLE) ihandle, os_disabled, NULL);
}

/*interface to release association
 *
 *\param obj
 *\param err
 * */
void Java_com_signove_health_healthservice_JniBridge_releaseassoc(JNIEnv *env, jobject obj, jint handle)
{
	DEBUG("device_releaseassoc");
	manager_request_association_release(handle);
}

/*interface to abort association
 *
 *\param obj
 *\param err
 * */
void Java_com_signove_health_healthservice_JniBridge_abortassoc(JNIEnv *env, jobject obj, jint handle)
{
	DEBUG("device_abortassoc");
	manager_request_association_release(handle);
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
jint Java_com_signove_health_healthservice_JniBridge_get_pmstore(JNIEnv *env,
						jobject obj, jint handle, jint ihandle)
{
	DEBUG("device_get_pmstore");
	manager_request_get_pmstore(handle, ihandle, device_get_pmstore_cb);
	return 0;
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
jint Java_com_signove_health_healthservice_JniBridge_get_segminfo(JNIEnv *env, jobject obj, jint handle, jint ihandle)
{
	Request *req;

	DEBUG("device_get_segminfo");
	req = manager_request_get_segment_info(handle, ihandle,
						device_get_segminfo_cb);
	return req ? 0 : 1;
}

/*interface to get segments data from a PM-Segment
 *
 *\param obj
 *\param handle PM-Store handle
 *\param instnumber PM-Segment InstNumber
 *\param ret Preliminary return status (actual data goes via Agent callback)
 *\param err
 * */
jint Java_com_signove_health_healthservice_JniBridge_get_segmdata(JNIEnv *env,
			jobject obj, jint handle, jint ihandle, jint instnumber)
{
	Request *req;

	DEBUG("device_get_segmdata");
	req = manager_request_get_segment_data(handle, ihandle,
				instnumber, device_get_segmdata_cb);
	return req ? 0 : 1;
}

/*interface to clear a PM-store segment
 *
 *\param obj
 *\param handle PM-Store handle
 *\param instnumber PM-Segment InstNumber
 *\param ret Preliminary return status (actual data goes via Agent callback)
 *\param err
 * */
jint Java_com_signove_health_healthservice_JniBridge_clearsegmdata(JNIEnv *env, jobject obj, jint handle, jint ihandle, jint instnumber)
{
	Request *req;

	DEBUG("device_clearsegmdata");
	req = manager_request_clear_segment(handle, ihandle,
				instnumber, device_clear_segm_cb);
	return req ? 0 : 1;
}

/* facade to clear all segments of a PM-Store
 *
 *\param obj
 *\param handle PM-Store handle
 *\param ret Preliminary return status (actual data goes via Agent callback)
 *\param err
 * */
jint Java_com_signove_health_healthservice_JniBridge_clearallsegmdata(JNIEnv *env, jobject obj, jint handle, jint ihandle)
{
	Request *req;

	DEBUG("device_clearsegmdata");
	req = manager_request_clear_segments(handle, ihandle,
				device_clear_segm_cb);
	return req ? 0 : 1;
}

/**
 * Main function
 * @return int
 */
void Java_com_signove_health_healthservice_JniBridge_healthd_init(JNIEnv *bridge_env, jobject bridge_obj)
{
	env = bridge_env;
	obj = bridge_obj;
	cls = (*env)->GetObjectClass(env, obj);
	jni_up_cancel_timer = (*env)->GetMethodID(env, cls, "cancel_timer", "(I)V");
	jni_up_create_timer = (*env)->GetMethodID(env, cls, "create_timer", "(II)I");
	jni_up_associated = (*env)->GetMethodID(env, cls, "associated", "(ILjava/lang/String;)V");
	jni_up_disassociated = (*env)->GetMethodID(env, cls, "disassociated", "(I)V");
	jni_up_deviceattributes = (*env)->GetMethodID(env, cls, "deviceattributes", "(ILjava/lang/String;)V");
	jni_up_measurementdata = (*env)->GetMethodID(env, cls, "measurementdata", "(ILjava/lang/String;)V");
	// FIXME PM-Store methods

	CommunicationPlugin plugin;
	plugin = communication_plugin();

	plugin_android_setup(&plugin, env, obj);
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

void Java_com_signove_health_healthservice_JniBridge_healthd_finalize(JNIEnv *env, jobject obj)
{
	manager_finalize();
}

/** @} */
