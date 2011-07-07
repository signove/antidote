/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file sample_bt_agent.c
 * \brief Main application implementation.
 *
 * Copyright (C) 2011 Signove Tecnologia Corporation.
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
 * \date Jun 30, 2011
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <time.h>
#include <glib.h>
#include <dbus/dbus-glib.h>

#include <ieee11073.h>
#include "src/communication/plugin/bluez/plugin_bluez.h"
#include "specializations/pulse_oximeter.h"
#include "agent.h"

static const intu8 AGENT_SYSTEM_ID_VALUE[] = { 0x11, 0x33, 0x55, 0x77, 0x99,
		0xbb, 0xdd, 0xff};

static int oximeter_specialization = 0x0190;
guint16 hdp_data_types[] = {0x1004, 0x00};

/**
 * Plugin definition
 */
static CommunicationPlugin comm_plugin = COMMUNICATION_PLUGIN_NULL;
static PluginBluezListener bluez_listener;
static GMainLoop *mainloop = NULL;

static int alarms = 0;
static guint alrm_handle = 0;

/**
 * App clean-up in termination phase
 */
static void app_clean_up()
{
	g_main_loop_unref(mainloop);
}

/* Called by IEEE library */

/**
 * Resets a framework-depende timer
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
 * @return FALSE (to cancel the timeout)
 */
static gboolean timer_alarm(gpointer data)
{
	Context *ctx = data;
	void (*f)() = ctx->timeout_action.func;
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
	ctx->timeout_action.id = g_timeout_add(ctx->timeout_action.timeout
					       * 1000, timer_alarm, ctx);
	return ctx->timeout_action.id;
}

static gboolean sigalrm(gpointer data);

/**
 * Initiates a timer for agent-related actions
 *
 * @return The timer handle
 */
static void schedule_alarm(ContextId id, int to)
{
	if (alrm_handle) {
		g_source_remove(alrm_handle);
	}
	alrm_handle = g_timeout_add(to * 1000, sigalrm, GUINT_TO_POINTER((guint) id));
}

/**
 * Callback for agent actions
 */
static gboolean sigalrm(gpointer data)
{
	unsigned int id = GPOINTER_TO_UINT(data);

	fprintf(stderr, "==== alarm %d ====\n", alarms);

	alrm_handle = 0;
	
	if (alarms > 2) {
		agent_send_data(id);
		schedule_alarm(id, 3);
	} else if (alarms == 2) {
		agent_request_association_release(id);
		schedule_alarm(id, 2);
	} else if (alarms == 1) {
		agent_disconnect(id);
		schedule_alarm(id, 2);
	} else {
		g_main_loop_quit(mainloop);
	}

	--alarms;

	return FALSE;
}

/**
 * Callback function that is called whenever a new device
 * has associated
 *
 * @param ctx current context.
 */
void device_associated(Context *ctx)
{
	fprintf(stderr, " main: Associated\n");
	alarms = 5;
	schedule_alarm(ctx->id, 3);
}

/**
 * Callback function that is called whenever a device
 * has disassociated
 *
 * @param ctx current context.
 */
void device_unavailable(Context *ctx)
{
	fprintf(stderr, " main: Disasociated\n");
	if (alarms > 2) {
		// involuntary; go straight to disconnection
		alarms = 1;
	}
}

/**
 * Callback function that is called whenever a new device
 * has connected (but not associated)
 *
 * @param ctx current context.
 */
void device_connected(Context *ctx)
{
	fprintf(stderr, "main: Connected\n");

	// ok, make it proceed with association
	// (agent has the initiative)
	agent_associate(ctx->id);
}

/**
 * Callback when a Bluetooth device connects
 */
static gboolean bt_connected(guint64 conn_handle, const char *btaddr)
{
	fprintf(stderr, "bt_connected %s\n", btaddr);
	return TRUE;
}

/**
 * Callback when a Bluetooth device disconnects
 */
static gboolean bt_disconnected(guint64 conn_handle, const char *btaddr)
{
	fprintf(stderr, "bt_disconnected %s\n", btaddr);
	return TRUE;
}


/**
 * Generate data for oximeter event report
 */
static void *event_report_cb()
{
	time_t now;
	struct tm nowtm;
	struct oximeter_event_report_data* data =
		malloc(sizeof(struct oximeter_event_report_data));

	time(&now);
	localtime_r(&now, &nowtm);

	data->beats = 60.5 + random() % 20;
	data->oximetry = 90.5 + random() % 10;
	data->century = nowtm.tm_year / 100;
	data->year = nowtm.tm_year % 100;
	data->month = nowtm.tm_mon + 1;
	data->day = nowtm.tm_mday;
	data->hour = nowtm.tm_hour;
	data->minute = nowtm.tm_min;
	data->second = nowtm.tm_sec;
	data->sec_fractions = 50;

	return data;
}

/**
 * Generate data for MDS
 */
static struct mds_system_data *mds_data_cb()
{
	struct mds_system_data* data = malloc(sizeof(struct mds_system_data));
	memcpy(&data->system_id, AGENT_SYSTEM_ID_VALUE, 8);
	return data;
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
	g_type_init();

	comm_plugin = communication_plugin();

	if (argc != 2) {
		fprintf(stderr, "Usage: sample_bt_agent <bdaddr>\n");
		exit(1);
	}

	fprintf(stderr, "\nIEEE 11073 sample agent\n");

	comm_plugin.timer_count_timeout = timer_count_timeout;
	comm_plugin.timer_reset_timeout = timer_reset_timeout;

	plugin_bluez_setup(&comm_plugin);
	bluez_listener.peer_connected = bt_connected;
	bluez_listener.peer_disconnected = bt_disconnected;
	plugin_bluez_set_listener(&bluez_listener);

	agent_init(&comm_plugin, oximeter_specialization,
			event_report_cb, mds_data_cb);

	AgentListener listener = AGENT_LISTENER_EMPTY;
	listener.device_connected = &device_connected;
	listener.device_associated = &device_associated;
	listener.device_unavailable = &device_unavailable;

	agent_add_listener(listener);

	agent_start();

	plugin_bluez_update_data_types(FALSE, hdp_data_types); // FALSE=source

	if (!plugin_bluez_connect(argv[1], hdp_data_types[0], HDP_CHANNEL_RELIABLE)) {
		exit(1);
	}

	// wait 10 seconds for connection
	alarms = 0;
	schedule_alarm(0, 5);

	fprintf(stderr, "Main loop started\n");
	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_ref(mainloop);
	g_main_loop_run(mainloop);
	fprintf(stderr, "Main loop stopped\n");

	agent_finalize();
	app_clean_up();

	return 0;
}
