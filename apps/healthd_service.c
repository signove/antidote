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
#include <ieee11073.h>
#include "src/communication/plugin/bluez/plugin_bluez.h"
#include "src/communication/plugin/trans/plugin_trans.h"
#include "src/trans/plugin/example_oximeter.h"
#include "src/communication/plugin/usb/plugin_usb.h"
#include "src/communication/plugin/bluez/plugin_glib_socket.h"
#include "src/trans/trans.h"
#include "src/util/log.h"
#include "src/util/linkedlist.h"
#include "src/communication/service.h"
#include "src/dim/pmstore_req.h"
#include "healthd_service.h"
#include "healthd_common.h"
#include "healthd_ipc_dbus.h"
#include "healthd_ipc_tcp.h"
#include "healthd_ipc_auto.h"

static const int DBUS_SERVER = 0;
static const int TCP_SERVER = 1;
static const int AUTOTESTING = 2;

healthd_ipc ipc;

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

static gboolean healthd_idle_cb(gpointer d)
{
	void **data = d;
	void (*f)(void*) = data[0];
	void *param = data[1];
	f(param);
	free(data);
	return FALSE;
}

/**
 * Schedules a function to be called in idle loop
 */
void healthd_idle_add(void *f, void *param)
{
	void **data = malloc(2 * sizeof(void*));
	data[0] = f;
	data[1] = param;
	g_idle_add(&healthd_idle_cb, data);
}

/**
 * Configures HDP data types
 */
void hdp_types_configure(uint16_t hdp_data_types[])
{
	plugin_bluez_update_data_types(TRUE, hdp_data_types); // TRUE=sink
}

static GMainLoop *mainloop = NULL;

/**
 * App clean-up in termination phase
 */
static void app_clean_up()
{
	g_main_loop_unref(mainloop);

	ipc.stop();
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
	app_setup_signals();
	#if !GLIB_CHECK_VERSION(2, 35, 0)
		g_type_init();
	#endif

	CommunicationPlugin bt_plugin;
	CommunicationPlugin usb_plugin;
	CommunicationPlugin trans_plugin;
	CommunicationPlugin tcp_plugin;

	CommunicationPlugin *plugins[] = {0, 0, 0, 0};
	int plugin_count = 0;

	int trans_support = 0;
	int usb_support = 0;
	int tcpp_support = 0;

	int i;

	int opmode = DBUS_SERVER;

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
		} else if (strcmp(argv[i], "--usb") == 0) {
			usb_support = 1;
		} else if (strcmp(argv[i], "--tcpp") == 0) {
			tcpp_support = 1;
		}
	}

	if (opmode == DBUS_SERVER) {
		healthd_ipc_dbus_init(&ipc);
	} else if (opmode == TCP_SERVER) {
		healthd_ipc_tcp_init(&ipc);
	} else if (opmode == AUTOTESTING) {
		healthd_ipc_auto_init(&ipc);
	}

	bt_plugin = communication_plugin();
	trans_plugin = communication_plugin();
	usb_plugin = communication_plugin();
	tcp_plugin = communication_plugin();

	plugin_bluez_setup(&bt_plugin);
	bt_plugin.timer_count_timeout = timer_count_timeout;
	bt_plugin.timer_reset_timeout = timer_reset_timeout;
	plugins[plugin_count++] = &bt_plugin;

	if (usb_support) {
		plugin_usb_setup(&usb_plugin);
		usb_plugin.timer_count_timeout = timer_count_timeout;
		usb_plugin.timer_reset_timeout = timer_reset_timeout;
		plugins[plugin_count++] = &usb_plugin;
	}

	if (trans_support) {
		plugin_trans_setup(&trans_plugin);
		trans_plugin.timer_count_timeout = timer_count_timeout;
		trans_plugin.timer_reset_timeout = timer_reset_timeout;
		plugins[plugin_count++] = &trans_plugin;
	}

	if (tcpp_support) {
		plugin_glib_socket_setup(&tcp_plugin, 1, 6024);
		tcp_plugin.timer_count_timeout = timer_count_timeout;
		tcp_plugin.timer_reset_timeout = timer_reset_timeout;
		plugins[plugin_count++] = &tcp_plugin;
	}

	manager_init(plugins);

	ManagerListener listener = MANAGER_LISTENER_EMPTY;
	listener.measurement_data_updated = &new_data_received;
	listener.segment_data_received = &segment_data_received;
	listener.device_available = &device_associated;
	listener.device_unavailable = &device_disassociated;
	listener.device_connected = &device_connected;
	listener.device_disconnected = &device_disconnected;

	manager_add_listener(listener);
	manager_start();

	if (trans_support) {
		trans_plugin_oximeter_register();
	}

	ipc.start();

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_ref(mainloop);
	g_main_loop_run(mainloop);
	DEBUG("Main loop stopped");
	manager_finalize();
	app_clean_up();
	DEBUG("Stopped.");

	return 0;
}

/** @} */
