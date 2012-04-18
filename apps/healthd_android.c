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
#include <jni.h>
#include "src/communication/context_manager.h"
#include "src/communication/plugin/android/plugin_android.h"
#include "src/util/log.h"
#include "healthd_common.h"

healthd_ipc ipc;

JavaVM *cached_jvm = 0;
JNIEnv *java_get_env();
jobject bridge_obj = 0;

char *android_tmp_location = 0;

// TODO this assumes that there is only one communication plugin!
static unsigned int plugin_id = 0;

static jmethodID jni_up_cancel_timer = 0;
static jmethodID jni_up_create_timer = 0;
static jmethodID jni_up_associated = 0;
static jmethodID jni_up_disassociated = 0;
static jmethodID jni_up_deviceattributes = 0;
static jmethodID jni_up_measurementdata = 0;
// FIXME fill up
static jmethodID jni_up_pmstoredata = 0;
static jmethodID jni_up_segmentinfo = 0;
static jmethodID jni_up_segmentdataresponse = 0;
static jmethodID jni_up_segmentdata = 0;
static jmethodID jni_up_segmentcleared = 0;

static CommunicationPlugin plugin;

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
	Context *ctx = context_get_and_lock(cid);
	if (ctx) {
		void (*f)() = ctx->timeout_action.func;
		if (f) {
			f(ctx);
		}
		context_unlock(ctx);
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
 * TODO schedule a function call in idle loop
 */
void healthd_idle_add(void*, void*)
{
	// TODO
	DEBUG("TODO healthd_idle_add");
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

	device_reqmdsatttr(cid);
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
	char *xml_out;

	DEBUG("jni getconfig");
	ContextId cid = {plugin_id, handle};

	device_get_configuration(cid, &xml_out);

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
	DEBUG("jni reqmeasurement");
	ContextId cid = {plugin_id, handle};

	device_req_measurementdata(cid);
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
	DEBUG("jni reqactivationscanner");
	ContextId cid = {plugin_id, handle};

	device_reqoperascanner(cid, ihandle);
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
	DEBUG("jni reqdeactivationscanner");
	ContextId cid = {plugin_id, handle};

	device_set_operational_state_of_scanner(cid, ihandle);
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
	DEBUG("jni releaseassoc");
	ContextId cid = {plugin_id, handle};

	device_releaseassoc(cid);
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
	DEBUG("jni abortassoc");
	ContextId cid = {plugin_id, handle};

	device_abortassoc(cid);
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
	DEBUG("jni get_pmstore");
	ContextId cid = {plugin_id, handle};

	device_get_pmstore(cid, ihandle);
	return 0;
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

	DEBUG("jni get_segminfo");
	ContextId cid = {plugin_id, handle};

	return device_request_get_segment_info(cid, ihandle);
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
	DEBUG("jni get_segmdata");
	ContextId cid = {plugin_id, handle};

	return device_request_get_segment(data, cid, ihandle, instnumber);
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

	DEBUG("jni clearsegmdata");
	ContextId cid = {plugin_id, handle};

	return device_clear_segment(cid, ihandle, instnumber);
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

	DEBUG("jni clearsegmdata");

	ContextId cid = {plugin_id, handle};

	return device_clearallsegmdata(cid, ihandle);
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

	ipc->call_agent_measurementdata = &notif_java_measurementdata;
	ipc->call_agent_connected = &notif_java_connected;
	ipc->call_agent_disconnected = &notif_java_disconnected;
	ipc->call_agent_associated = &notif_java_associated;
	ipc->call_agent_disassociated = &notif_java_disassociated;
	ipc->call_agent_segmentinfo = &notif_java_segmentinfo;
	ipc->call_agent_segmentdataresponse = &notif_java_segmentdataresponse;
	ipc->call_agent_segmentdata = &notif_java_segmentdata;
	ipc->call_agent_segmentcleared = &notif_java_segmentcleared;
	ipc->call_agent_pmstoredata = &notif_java_pmstoredata;
	ipc->call_agent_deviceattributes = &notif_java_deviceattributes;
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
