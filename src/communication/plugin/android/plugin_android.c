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
 * @addtogroup Communication
 * @{
 */

#include <string.h>
#include <jni.h>
#include "src/communication/plugin/plugin.h"
#include "src/communication/communication.h"
#include "src/util/log.h"
#include "plugin_android.h"

static char *current_data = NULL;
static int data_len = 0;

static int init();
static int finalize();
static ByteStreamReader *get_apdu(struct Context *ctx);
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream);
static int force_disconnect_channel(struct Context *ctx);

JNIEnv *env;
jclass cls;
jobject obj;
jmethodID jni_up_disconnect_channel;
jmethodID jni_up_send_data;

/**
 * Callback called from Android, when device connects (BT-wise)
 *
 * @param handle
 */
void Java_com_signove_health_healthservice_JniBridge_channel_connected(JNIEnv *env, jobject obj, jint handle)
{
	communication_transport_connect_indication(handle);
}

/**
 * Callback called from Android layer when device disconnects (BT-wise)
 *
 * @param handle
 */
void Java_com_signove_health_healthservice_JniBridge_channel_disconnected(JNIEnv *env, jobject obj, jint handle)
{
	communication_transport_disconnect_indication(handle);
}

void plugin_android_setup(CommunicationPlugin *plugin, JNIEnv *bridge_env, jobject bridge_obj)
{
	plugin->network_init = init;
	plugin->network_get_apdu_stream = get_apdu;
	plugin->network_send_apdu_stream = send_apdu_stream;
	plugin->network_disconnect = force_disconnect_channel;
	plugin->network_finalize = finalize;

	env = bridge_env;
	obj = bridge_obj;
	cls = (*env)->GetObjectClass(env, obj);
	jni_up_send_data = (*env)->GetMethodID(env, cls, "send_data", "(I[B)V");
	jni_up_disconnect_channel = (*env)->GetMethodID(env, cls, "disconnect_channel", "(I)V");
}

/**
 * Forces closure of a channel
 */
static int force_disconnect_channel(Context *c)
{
	(*env)->CallVoidMethod(env, obj, jni_up_disconnect_channel, (jint) c->id);
	return 1;
}

/**
 * Starts Health link with Android.
 *
 * @return success status
 */
static int init()
{
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
 * @return a byteStream with the read APDU.
 */
static ByteStreamReader *get_apdu(struct Context *ctx)
{
	unsigned char *buffer;
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
void Java_com_signove_health_healthservice_JniBridge_data_received(JNIEnv *env, jobject obj, jint handle, jbyteArray buf)
{	
	int len = (*env)->GetArrayLength(env, buf);
	char *data = malloc(len);
	(*env)->GetByteArrayRegion(env, buf, 0, len, (jbyte *) data);

	if (data) {
		data_len = len;
		current_data = malloc(len + 1);
		memcpy(current_data, buf, len);
		current_data[len] = '\0';
		communication_read_input_stream(context_get(handle));
	}
}


/**
 * Sends IEEE data to HDP channel
 *
 * @return success status
 */
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream)
{
	jbyteArray ba = (*env)->NewByteArray(env, stream->size);
	(*env)->SetByteArrayRegion(env, ba, 0, stream->size, (jbyte*) stream->buffer);

	(*env)->CallVoidMethod(env, obj, jni_up_send_data, (jint) ctx->id, ba);

	return NETWORK_ERROR_NONE;
}


/** @} */
