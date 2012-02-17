/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_android.c
 * \brief Android JNI interface source.
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

#include <string.h>
#include <jni.h>
#include "src/communication/plugin/plugin.h"
#include "src/communication/communication.h"
#include "src/util/log.h"
#include "plugin_android.h"

/**
 * Plugin ID attribued by stack
 */
unsigned int plugin_id = 0;

/**
 * Current data APDU waiting for collection by stack
 */
static char *current_data = NULL;

/**
 * Size of data APDU waiting for collection by stack
 */
static int data_len = 0;

static int init();
static int finalize();
static ByteStreamReader *get_apdu(struct Context *ctx);
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream);
static int force_disconnect_channel(struct Context *ctx);

/**
 * Cached Java VM context
 */
extern JavaVM *cached_jvm;

/**
 * Get Java Environment
 * @return env
 */
JNIEnv *java_get_env();

/**
 * Proxy object in Java level that uses our functions
 * as native methods
 */
extern jobject bridge_obj;

/**
 * Method ID of Java method to disconnect channel
 */
jmethodID jni_up_disconnect_channel;

/**
 * Method ID of Java method to send data
 */
jmethodID jni_up_send_data;

/**
 * Callback called from Android, when device connects (BT-wise)
 *
 * @param env Java thread environment
 * @param obj Java object
 * @param handle Communication handle
 */
void Java_com_signove_health_service_JniBridge_Cchannelconnected(JNIEnv *env,
						jobject obj, jint handle)
{
	DEBUG("channel connected at plugin");
	ContextId cid = {plugin_id, handle};
	communication_transport_connect_indication(cid);
}

/**
 * Callback called from Android layer when device disconnects (BT-wise)
 *
 * @param env Java thread environment
 * @param obj Java object
 * @param handle Communication handle
 */
void Java_com_signove_health_service_JniBridge_Cchanneldisconnected(JNIEnv *env, jobject obj,
								jint handle)
{
	DEBUG("channel disconnected at plugin");
	ContextId cid = {plugin_id, handle};
	communication_transport_disconnect_indication(cid);
}

/**
 * Plugin setup.
 *
 * @param plugin the plugin descriptor structure to fill in
 */
void plugin_android_setup(CommunicationPlugin *plugin)
{
	plugin->network_init = init;
	plugin->network_get_apdu_stream = get_apdu;
	plugin->network_send_apdu_stream = send_apdu_stream;
	plugin->network_disconnect = force_disconnect_channel;
	plugin->network_finalize = finalize;

	JNIEnv *env = java_get_env();
	jclass cls = (*env)->GetObjectClass(env, bridge_obj);
	jni_up_send_data = (*env)->GetMethodID(env, cls,
							"send_data", "(I[B)V");
	jni_up_disconnect_channel = (*env)->GetMethodID(env, cls,
							"disconnect_channel", "(I)V");
	DEBUG("healthd C: method send_data %p", jni_up_send_data);
	DEBUG("healthd C: method disconnect_channel %p", jni_up_disconnect_channel);
}

/**
 * Forces closure of a channel
 * @param context Communication context
 * @return success status
 */
static int force_disconnect_channel(Context *c)
{
	JNIEnv *env = java_get_env();
	(*env)->CallVoidMethod(env, bridge_obj,
					jni_up_disconnect_channel,
					(jint) c->id.connid);
	return 1;
}

/**
 * Starts Health link with Android.
 *
 * @param plugin_label the internal plugin label id
 * @return success status
 */
static int init(unsigned int plugin_label)
{
	plugin_id = plugin_label;
	return NETWORK_ERROR_NONE;
}

/**
 * Stops Android link. Link may be restarted again afterwards.
 *
 * @return success status
 */
static int finalize()
{
	return NETWORK_ERROR_NONE;
}


/**
 * Reads an APDU from buffer
 *
 * @param context the communication context
 * @return a byteStream with the read APDU.
 */
static ByteStreamReader *get_apdu(struct Context *ctx)
{
	unsigned char *buffer;
	DEBUG("\nandplug get APDU stream");

	// Create bytestream
	buffer = malloc(data_len);
	memcpy(buffer, (unsigned char *) current_data, data_len);

	ByteStreamReader *stream = byte_stream_reader_instance(buffer, data_len);

	if (stream == NULL) {
		ERROR("\n andplug Error creating bytelib");
		return NULL;
	}

	DEBUG(" andplug APDU received ");

	return stream;
}


/**
 * Socket data receiving callback
 *
 * @param env JNI thread environment
 * @param obj JNI bridge object
 * @param handle Connection handle (maps to a ContextID)
 * @param buf Byte array containing data
 */
void Java_com_signove_health_service_JniBridge_Cdatareceived(JNIEnv *env, jobject obj,
							jint handle, jbyteArray buf)
{	
	DEBUG("data received at plugin");

	int len = (*env)->GetArrayLength(env, buf);
	char *data = malloc(len + 1);
	(*env)->GetByteArrayRegion(env, buf, 0, len, (jbyte *) data);

	data_len = len;
	current_data = data;
	current_data[len] = '\0';
	ContextId cid = {plugin_id, handle};
	communication_read_input_stream(context_get(cid));
}


/**
 * Sends IEEE data to HDP channel
 *
 * @param ctx Communication context
 * @param stream Byte stream
 * @return success status
 */
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream)
{
	JNIEnv *env = java_get_env();
	jbyteArray ba = (*env)->NewByteArray(env, stream->size);
	(*env)->SetByteArrayRegion(env, ba, 0, stream->size, (jbyte*) stream->buffer);
	DEBUG("healthd c: calling send_data");
	(*env)->CallVoidMethod(env, bridge_obj,
					jni_up_send_data,
					(jint) ctx->id.connid, ba);

	return NETWORK_ERROR_NONE;
}


/** @} */
