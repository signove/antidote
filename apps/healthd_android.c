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
#include <ieee11073.h>
#include "src/communication/plugin/android/plugin_android.h"
#include "src/communication/plugin/trans/plugin_trans.h"
#include "src/util/log.h"
#include "healthd_common.h"
#include "healthd_ipc.h"

healthd_ipc ipc;

JavaVM *cached_jvm = 0;
JNIEnv *java_get_env();
jobject bridge_obj = 0;

char *android_tmp_location = 0;

static unsigned int plugin_ids[2];

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
CommunicationPlugin trans_plugin;

// TODO kludge ahead!
// 
// We encode "plugin + id" from context id into a single int
// using "(plugin order) * 1000000 + id" in order not to change
// the JNI interfaces, that currently uses a single int as
// connection context. We use plugin order instead of 
// Antidote's plugin id in order to guarantee that numbers will
// be low and plugin #0 is HDP (see below)
//
// Moreover, we reserve the plugin 0 (handles < 1000000) to
// Bluetooth HDP devices, whose handle numbers are generated
// at Java level. The Android communication plugin expects
// this.

#define TRANS_PLUGIN_ID_RANGE 1000000

static jint context_to_handle(ContextId ctx)
{
	if (ctx.plugin == plugin_ids[0]) {
		return ctx.connid;
	} else if (ctx.plugin == plugin_ids[1]) {
		return TRANS_PLUGIN_ID_RANGE + ctx.connid;
	}
	DEBUG("Could not translate ctx %d:%d to handle", ctx.plugin, (int) ctx.connid);
	return 0;
}

static ContextId handle_to_context(jint handle)
{
	if ((handle >= 0) && (handle < TRANS_PLUGIN_ID_RANGE)) {
		ContextId c = {plugin_ids[0], handle};
		return c;
	} else if (handle >= TRANS_PLUGIN_ID_RANGE) {
		ContextId c = {plugin_ids[1], handle - TRANS_PLUGIN_ID_RANGE};
		return c;
	}
	DEBUG("Could not translate handle %d to context", handle);
	ContextId c = {0, 0};
	return c;
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
		jint timer_handle = ctx->timeout_action.id;
		JNIEnv *env = java_get_env();
		(*env)->CallVoidMethod(env, bridge_obj, jni_up_cancel_timer, timer_handle);
	}
}
/**
 * Timer callback.
 * Calls the supplied callback when timer reaches timeout, and cancels timer.
 *
 * @param env JNI thread environment
 * @param obj JNI bridge object
 * @param handle Handle to get context
 */
void Java_com_signove_health_service_JniBridge_Ctimeralarm(JNIEnv *env, jobject obj, jint handle)
{
	DEBUG("timer_alarm");
	ContextId cid = handle_to_context(handle); 
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
					context_to_handle(ctx->id));
	return ctx->timeout_action.id;
}

/**
 * TODO schedule a function call in idle loop
 */
void healthd_idle_add(void* f, void *param)
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
static void notif_java_associated(ContextId conn_cid, char *xml)
{
	JNIEnv *env = java_get_env();
	jstring jxml = (*env)->NewStringUTF(env, xml);
	(*env)->CallVoidMethod(env, bridge_obj, jni_up_associated,
					context_to_handle(conn_cid), jxml);
}

/**
 * Function that calls D-Bus agent.MeasurementData method.
 *
 * @param conn_cid device handle
 * @param xml Data in xml format
 * @return success status
 */
static void notif_java_measurementdata(ContextId conn_cid, char *xml)
{
	JNIEnv *env = java_get_env();
	jstring jxml = (*env)->NewStringUTF(env, xml);
	(*env)->CallVoidMethod(env, bridge_obj,
				jni_up_measurementdata,
				context_to_handle(conn_cid), jxml);
}

/**
 * Function that calls D-Bus agent.SegmentInfo method.
 *
 * @param handle PM-Store handle
 * @param xml PM-Segment instance data in XML format
 * @return success status
 */
static void notif_java_segmentinfo(ContextId conn_cid, unsigned int handle, char *xml)
{
	// JNIEnv *env = java_get_env();
	// (*env)->CallVoidMethod(env, bridge_obj,jni_up_segmentinfo(conn_handle, handle, xml);
	// FIXME
}


/**
 * Function that calls D-Bus agent.SegmentDataResponse method.
 *
 * @param conn_cid device handle
 * @param handle PM-Store handle
 * @param instnumber PM-Segment instance number
 * @param status Return status
 * @return success status
 */
static void notif_java_segmentdataresponse(ContextId conn_cid,
			unsigned int handle, unsigned int instnumber, unsigned int retstatus)
{
	// JNIEnv *env = java_get_env();
	// (*env)->CallVoidMethod(env, bridge_obj, jni_up_segmentdataresponse(conn_handle, handle, instnumber, retstatus);
	// FIXME
}


/**
 * Function that calls D-Bus agent.SegmentData method.
 *
 * @param conn_cid device handle
 * @param handle PM-Store handle
 * @param instnumber PM-Segment instance number
 * @param xml PM-Segment instance data in XML format
 * @return success status
 */
static void notif_java_segmentdata(ContextId conn_cid, unsigned int handle,
					unsigned int instnumber, char *xml)
{
	// JNIEnv *env = java_get_env();:q

	// (*env)->CallVoidMethod(env, bridge_obj, jni_up_segmentdata(conn_handle, handle, instnumber, jxml);
	// FIXME
}


/**
 * Function that calls D-Bus agent.PMStoreData method.
 *
 * @param conn_cid device handle
 * @param handle PM-Store handle
 * @param xml PM-Store data attributes in XML format
 * @return success status
 */
static void notif_java_pmstoredata(ContextId conn_cid, unsigned int handle, char *xml)
{
	// JNIEnv *env = java_get_env();
	// (*env)->CallVoidMethod(env, bridge_obj, jni_up_pmstoredata(conn_handle, handle, jxml);
	// FIXME
}


/**
 * Function that calls D-Bus agent.SegmentCleared method.
 *
 * @param conn_cid device handle
 * @param handle PM-Store handle
 * @param PM-Segment instance number
 * @return success status
 */
static void notif_java_segmentcleared(ContextId conn_cid, unsigned int handle,
							unsigned int instnumber,
							unsigned int retstatus)
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
static void notif_java_deviceattributes(ContextId conn_cid, char *xml)
{
	JNIEnv *env = java_get_env();
	jstring jxml = (*env)->NewStringUTF(env, xml);
	(*env)->CallVoidMethod(env, bridge_obj,
			jni_up_deviceattributes,
			context_to_handle(conn_cid), jxml);
}

/**
 * Function that calls D-Bus agent.Disassociated method.
 *
 * @return success status
 */
static void notif_java_disassociated(ContextId conn_cid)
{
	JNIEnv *env = java_get_env();
	(*env)->CallVoidMethod(env, bridge_obj,
			jni_up_disassociated,
			context_to_handle(conn_cid));
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
	DEBUG("jni reqmdsattr");
	ContextId cid = handle_to_context(handle);

	device_reqmdsattr(cid);
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
	ContextId cid = handle_to_context(handle);

	device_getconfig(cid, &xml_out);

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
	ContextId cid = handle_to_context(handle);

	device_reqmeasurement(cid);
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
	ContextId cid = handle_to_context(handle);

	device_reqactivationscanner(cid, handle);
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
	ContextId cid = handle_to_context(handle);

	device_reqdeactivationscanner(cid, handle);
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
	ContextId cid = handle_to_context(handle);

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
	ContextId cid = handle_to_context(handle);

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
	jint ret;

	DEBUG("jni get_pmstore");
	ContextId cid = handle_to_context(handle);

	device_get_pmstore(cid, ihandle, &ret);
	return ret;
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
	DEBUG("jni get_segminfo");
	ContextId cid = handle_to_context(handle);

	jint ret;
	device_get_segminfo(cid, ihandle, &ret);
	return ret;
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
	ContextId cid = handle_to_context(handle);

	jint ret;
	device_get_segmdata(cid, ihandle, instnumber, &ret);
	return ret;
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
	DEBUG("jni clearsegmdata");
	ContextId cid = handle_to_context(handle);

	jint ret;
	device_clearsegmdata(cid, ihandle, instnumber, &ret);
	return ret;
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
	DEBUG("jni clearsegmdata");

	ContextId cid = handle_to_context(handle);

	jint ret;
	device_clearallsegmdata(cid, ihandle, &ret);
	return ret;
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
	trans_plugin = communication_plugin();

	plugin_android_setup(&plugin);
	plugin.timer_count_timeout = timer_count_timeout;
	plugin.timer_reset_timeout = timer_reset_timeout;

	plugin_trans_setup(&trans_plugin);
	trans_plugin.timer_count_timeout = timer_count_timeout;
	trans_plugin.timer_reset_timeout = timer_reset_timeout;

	DEBUG("healthd C: init manager");

	CommunicationPlugin *plugins[] = {&plugin, &trans_plugin, 0};
	manager_init(plugins);
	plugin_ids[0] = communication_plugin_id(&plugin);
	plugin_ids[1] = communication_plugin_id(&trans_plugin);

	DEBUG("healthd C: plugin id %d", plugin_ids[0]);
	DEBUG("healthd C: trans plugin id %d", plugin_ids[1]);
	DEBUG("healthd C: init mgr listener");

	ManagerListener listener = MANAGER_LISTENER_EMPTY;
	listener.measurement_data_updated = &new_data_received;
	listener.segment_data_received = &segment_data_received;
	listener.device_available = &device_associated;
	listener.device_unavailable = &device_disassociated;

	manager_add_listener(listener);

	DEBUG("healthd C: starting mgr");
	manager_start();

	ipc.call_agent_measurementdata = &notif_java_measurementdata;
	ipc.call_agent_connected = 0;
	ipc.call_agent_disconnected = 0;
	ipc.call_agent_associated = &notif_java_associated;
	ipc.call_agent_disassociated = &notif_java_disassociated;
	ipc.call_agent_segmentinfo = &notif_java_segmentinfo;
	ipc.call_agent_segmentdataresponse = &notif_java_segmentdataresponse;
	ipc.call_agent_segmentdata = &notif_java_segmentdata;
	ipc.call_agent_segmentcleared = &notif_java_segmentcleared;
	ipc.call_agent_pmstoredata = &notif_java_pmstoredata;
	ipc.call_agent_deviceattributes = &notif_java_deviceattributes;
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
