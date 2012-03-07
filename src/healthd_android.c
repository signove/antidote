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
 * @addtogroup Android
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

JavaVM *cached_jvm = 0;
JNIEnv *java_get_env();
jobject bridge_obj = 0;

char *android_tmp_location = 0;

// TODO this assumes that there is only one communication plugin!
static unsigned int plugin_id = 0;

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

// FIXME create extented configuration file in proper Android path

static void notif_java_measurementdata(ContextId, char *);
static void notif_java_disassociated(ContextId);
static void notif_java_associated(ContextId, char *);
static void notif_java_segmentinfo(ContextId, int, char *);
static void notif_java_segmentdataresponse(ContextId, int, int, int);
static void notif_java_segmentdata(ContextId, int, int, char *);
static void notif_java_segmentcleared(ContextId, int, int, int);
static void notif_java_pmstoredata(ContextId, int, char *);

CommunicationPlugin plugin;

/* Called by IEEE library */

/**
 * Resets a framework-depende timer
 *
 * @param ctx Context
 */
static void timer_reset_timeout(Context *ctx)
{
	if (ctx->timeout_action.id) {
		jint handle = ctx->timeout_action.id;
		JNIEnv *env = java_get_env();
		(*env)->CallVoidMethod(env, bridge_obj, jni_up_cancel_timer, handle);
	}
}
/**
 * Timer callback.
 * Calls the supplied callback when timer reaches timeout, and cancels timer.
 *
 * @param env JNI thread environment
 * @param obj JNI bridge object
 * @param id Context id (supplied when timer was created)
 */
void Java_com_signove_health_service_JniBridge_Ctimeralarm(JNIEnv *env, jobject obj, jint id)
{
	DEBUG("timer_alarm");
	ContextId cid = {plugin_id, id};
	Context *ctx = context_get(cid);
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
 * @param ctx Context
 * @return The timer handle
 */
static int timer_count_timeout(Context *ctx)
{
	JNIEnv *env = java_get_env();
	ctx->timeout_action.id = (int) (*env)->CallIntMethod(env, bridge_obj,
					jni_up_create_timer,
					(jint) ctx->timeout_action.timeout * 1000,
					(jint) ctx->id.connid);
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
static void notif_java_associated(ContextId conn_handle, char *xml)
{
	JNIEnv *env = java_get_env();
	jstring jxml = (*env)->NewStringUTF(env, xml);
	(*env)->CallVoidMethod(env, bridge_obj, jni_up_associated,
					(jint) conn_handle.connid, jxml);
}

/**
 * Function that calls D-Bus agent.MeasurementData method.
 *
 * @param conn_handle device handle
 * @param xml Data in xml format
 * @return success status
 */
static void notif_java_measurementdata(ContextId conn_handle, char *xml)
{
	JNIEnv *env = java_get_env();
	jstring jxml = (*env)->NewStringUTF(env, xml);
	(*env)->CallVoidMethod(env, bridge_obj,
				jni_up_measurementdata,
				(jint) conn_handle.connid, jxml);
}

/**
 * Function that calls D-Bus agent.SegmentInfo method.
 *
 * @param handle PM-Store handle
 * @param xml PM-Segment instance data in XML format
 * @return success status
 */
static void notif_java_segmentinfo(ContextId conn_handle, int handle, char *xml)
{
	// JNIEnv *env = java_get_env();
	// (*env)->CallVoidMethod(env, bridge_obj,jni_up_segmentinfo(conn_handle, handle, xml);
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
static void notif_java_segmentdataresponse(ContextId conn_handle,
			int handle, int instnumber, int retstatus)
{
	// JNIEnv *env = java_get_env();
	// (*env)->CallVoidMethod(env, bridge_obj, jni_up_segmentdataresponse(conn_handle, handle, instnumber, retstatus);
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
static void notif_java_segmentdata(ContextId conn_handle, int handle,
					int instnumber, char *xml)
{
	// JNIEnv *env = java_get_env();
	// (*env)->CallVoidMethod(env, bridge_obj, jni_up_segmentdata(conn_handle, handle, instnumber, jxml);
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
static void notif_java_pmstoredata(ContextId conn_handle, int handle, char *xml)
{
	// JNIEnv *env = java_get_env();
	// (*env)->CallVoidMethod(env, bridge_obj, jni_up_pmstoredata(conn_handle, handle, jxml);
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
static void notif_java_segmentcleared(ContextId conn_handle, int handle,
							int instnumber,
							int retstatus)
{
	// JNIEnv *env = java_get_env();
	// (*env)->CallVoidMethod(env, bridge_obj, jni_up_segmentcleared(conn_handle, handle, instnumber, retstatus);
	// FIXME
}


/**
 * Function that calls D-Bus agent.DeviceAttributes method.
 *
 * @return success status
 */
static void notif_java_deviceattributes(ContextId conn_handle, char *xml)
{
	JNIEnv *env = java_get_env();
	jstring jxml = (*env)->NewStringUTF(env, xml);
	(*env)->CallVoidMethod(env, bridge_obj,
			jni_up_deviceattributes,
			(jint) conn_handle.connid, jxml);
}

/**
 * Function that calls D-Bus agent.Disassociated method.
 *
 * @return success status
 */
static void notif_java_disassociated(ContextId conn_handle)
{
	JNIEnv *env = java_get_env();
	(*env)->CallVoidMethod(env, bridge_obj,
			jni_up_disassociated,
			(jint) conn_handle.connid);
}


/**
 * Callback used to request mds attributes
 *
 *\param ctx
 *\param response_apdu
 */
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


/**
 * Interface to request mds attributes
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 */
void Java_com_signove_health_service_JniBridge_Creqmdsattr(JNIEnv *env, jobject obj, jint handle)
{
	DEBUG("device_reqmdsattr");
	ContextId cid = {plugin_id, handle};

	manager_request_get_all_mds_attributes(cid, device_reqmdsattr_callback);
}

/**
 * Interface to get device configuration
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 */
jstring Java_com_signove_health_service_JniBridge_Cgetconfig(JNIEnv *env, jobject obj, jint handle)
{
	DataList *list;
	char *xml_out;

	DEBUG("device_getconfig");
	ContextId cid = {plugin_id, handle};

	list = manager_get_configuration(cid);

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

/**
 * interface to request measuremens
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 */
void Java_com_signove_health_service_JniBridge_Creqmeasurement(JNIEnv *env, jobject obj, jint handle)
{
	DEBUG("device_reqmeasurement");
	ContextId cid = {plugin_id, handle};

	manager_request_measurement_data_transmission(cid, NULL);
}

/**
 * interface to activate scanner
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 * \param ihandle Object handle
 */
void Java_com_signove_health_service_JniBridge_Creqactivationscanner(JNIEnv *env, jobject obj, jint handle, jint ihandle)
{
	DEBUG("device_reqactivationscanner");
	ContextId cid = {plugin_id, handle};

	manager_set_operational_state_of_the_scanner(cid, (HANDLE) ihandle, os_enabled, NULL);
}

/**
 * interface to deactivate scanner
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 * \param ihandle Object handle
 */
void Java_com_signove_health_service_JniBridge_Creqdeactivationscanner(JNIEnv *env, jobject obj, jint handle, jint ihandle)
{
	DEBUG("device_reqdeactivationscanner");
	ContextId cid = {plugin_id, handle};

	manager_set_operational_state_of_the_scanner(cid, (HANDLE) ihandle, os_disabled, NULL);
}

/**
 * interface to release association
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 */
void Java_com_signove_health_service_JniBridge_Creleaseassoc(JNIEnv *env, jobject obj, jint handle)
{
	DEBUG("device_releaseassoc");
	ContextId cid = {plugin_id, handle};

	manager_request_association_release(cid);
}

/**
 * interface to abort association
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 */
void Java_com_signove_health_service_JniBridge_Cabortassoc(JNIEnv *env, jobject obj, jint handle)
{
	DEBUG("device_abortassoc");
	ContextId cid = {plugin_id, handle};

	manager_request_association_release(cid);
}

/**
 * Callback for PM-Store GET
 *
 * \param ctx
 * \param r Request object
 * \param response_apdu
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
			notif_java_pmstoredata(ctx->id, ret->handle, data);
			free(data);
			data_list_del(list);
		}
	}
}

/**
 * interface to get PM-Store attributes
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 * \param ihandle Object handle
 */
jint Java_com_signove_health_service_JniBridge_Cgetpmstore(JNIEnv *env,
						jobject obj, jint handle, jint ihandle)
{
	DEBUG("device_get_pmstore");
	ContextId cid = {plugin_id, handle};
	manager_request_get_pmstore(cid, ihandle, device_get_pmstore_cb);
	return 0;
}

/**
 * Callback for PM-Store get segment info response
 *
 *\param ctx
 *\param r Request object
 *\param response_apdu
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
			notif_java_segmentinfo(ctx->id, ret->handle, data);
			free(data);
			data_list_del(list);
		}
	}
}

/**
 * Callback for PM-Store segment data response
 *
 *\param ctx
 *\param response_apdu
 */
static void device_get_segmdata_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreGetSegmDataRet *ret = (PMStoreGetSegmDataRet*) r->return_data;

	if (!ret)
		return;

	notif_java_segmentdataresponse(ctx->id, ret->handle, ret->inst, ret->response);
}

/**
 * Callback for PM-Store clear segment response
 *
 *\param ctx
 *\param response_apdu
 */
static void device_clear_segm_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreClearSegmRet *ret = (PMStoreClearSegmRet*) r->return_data;

	if (!ret)
		return;

	notif_java_segmentcleared(ctx->id, ret->handle, ret->inst, ret->response);
}

/**
 * interface to get segments info from a PM-Store
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 * \param ihandle Object handle
 */
jint Java_com_signove_health_service_JniBridge_Cgetsegminfo(JNIEnv *env, jobject obj, jint handle, jint ihandle)
{
	Request *req;

	DEBUG("device_get_segminfo");
	ContextId cid = {plugin_id, handle};

	req = manager_request_get_segment_info(cid, ihandle,
						device_get_segminfo_cb);
	return req ? 0 : 1;
}

/**
 * interface to get segments data from a PM-Segment
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 * \param ihandle Object handle
 * \param instnumber PM-Segment Instance number
 */
jint Java_com_signove_health_service_JniBridge_Cgetsegmdata(JNIEnv *env,
			jobject obj, jint handle, jint ihandle, jint instnumber)
{
	Request *req;

	DEBUG("device_get_segmdata");
	ContextId cid = {plugin_id, handle};

	req = manager_request_get_segment_data(cid, ihandle,
				instnumber, device_get_segmdata_cb);
	return req ? 0 : 1;
}

/**
 * interface to clear a PM-store segment
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 * \param ihandle Object handle
 * \param instnumber PM-Segment Instance number
 */
jint Java_com_signove_health_service_JniBridge_Cclearsegmdata(JNIEnv *env, jobject obj,
					jint handle, jint ihandle, jint instnumber)
{
	Request *req;

	DEBUG("device_clearsegmdata");
	ContextId cid = {plugin_id, handle};

	req = manager_request_clear_segment(cid, ihandle,
				instnumber, device_clear_segm_cb);
	return req ? 0 : 1;
}

/**
 * Facade to clear all segments of a PM-Store
 *
 *
 * \param env JNI thread environment
 * \param obj JNI bridge object
 * \param handle Context ID
 * \param ihandle Object handle
 */
jint Java_com_signove_health_service_JniBridge_Cclearallsegmdata(JNIEnv *env, jobject obj,
								jint handle, jint ihandle)
{
	Request *req;

	DEBUG("device_clearsegmdata");

	ContextId cid = {plugin_id, handle};

	req = manager_request_clear_segments(cid, ihandle,
				device_clear_segm_cb);
	return req ? 0 : 1;
}

/**
 * Main function
 * @param env JNI thread environment
 * @param obj JNI bridge object
 * @param tmp_path Path where application is allowed to write files
 * @return int
 */
void Java_com_signove_health_service_JniBridge_Chealthdinit(JNIEnv *env, jobject obj, jstring tmp_path)
{
	DEBUG("healthd C: initializing %p %p", env, obj);

	bridge_obj = (*env)->NewGlobalRef(env, obj);

	DEBUG("healthd C: storing tmp dir");
	
	{
	const char *cpath = (*env)->GetStringUTFChars(env, tmp_path, NULL);
	asprintf(&android_tmp_location, "%s", cpath);
	(*env)->ReleaseStringUTFChars(env, tmp_path, cpath);
	}

	DEBUG("healthd C: tmp dir is %s", android_tmp_location);

	DEBUG("healthd C: getting class and methods");

	jclass cls = (*env)->GetObjectClass(env, bridge_obj);
	jni_up_cancel_timer = (*env)->GetMethodID(env, cls, "cancel_timer", "(I)V");
	DEBUG("healthd C: method %p", jni_up_cancel_timer);
	jni_up_create_timer = (*env)->GetMethodID(env, cls, "create_timer", "(II)I");
	DEBUG("healthd C: method %p", jni_up_create_timer);
	jni_up_associated = (*env)->GetMethodID(env, cls, "associated", "(ILjava/lang/String;)V");
	DEBUG("healthd C: method ass %p", jni_up_associated);
	jni_up_disassociated = (*env)->GetMethodID(env, cls, "disassociated", "(I)V");
	DEBUG("healthd C: method dis %p", jni_up_disassociated);
	jni_up_deviceattributes = (*env)->GetMethodID(env, cls, "deviceattributes", "(ILjava/lang/String;)V");
	DEBUG("healthd C: method devattr %p", jni_up_deviceattributes);
	jni_up_measurementdata = (*env)->GetMethodID(env, cls, "measurementdata", "(ILjava/lang/String;)V");
	DEBUG("healthd C: method meas %p", jni_up_measurementdata);
	// FIXME PM-Store methods

	DEBUG("healthd C: init comm plugin");

	plugin = communication_plugin();

	plugin_android_setup(&plugin);
	plugin.timer_count_timeout = timer_count_timeout;
	plugin.timer_reset_timeout = timer_reset_timeout;

	DEBUG("healthd C: init manager");

	CommunicationPlugin *plugins[] = {&plugin, 0};
	manager_init(plugins);
	plugin_id = communication_plugin_id(&plugin);

	DEBUG("healthd C: plugin id %d", plugin_id);
	DEBUG("healthd C: init mgr listener");

	ManagerListener listener = MANAGER_LISTENER_EMPTY;
	listener.measurement_data_updated = &new_data_received;
	listener.segment_data_received = &segment_data_received;
	listener.device_available = &device_associated;
	listener.device_unavailable = &device_disassociated;

	manager_add_listener(listener);

	DEBUG("healthd C: starting mgr");
	manager_start();
}

void Java_com_signove_health_service_JniBridge_Chealthdfinalize(JNIEnv *env, jobject obj)
{
	if (! bridge_obj)
		return;

	manager_finalize();

	(*env)->DeleteGlobalRef(env, bridge_obj);
	bridge_obj = 0;

	free(android_tmp_location);
	android_tmp_location = 0;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
	cached_jvm = jvm;
	return JNI_VERSION_1_6;
}

JNIEnv *java_get_env()
{
	JNIEnv *env;
	(*cached_jvm)->GetEnv(cached_jvm, (void **) &env, JNI_VERSION_1_6);
	return env;
}

/** @} */
