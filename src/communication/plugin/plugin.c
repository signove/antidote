/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin.c
 * \brief Stub plugin source.
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
 * IEEE 11073 Communication Model - Finite State Machine implementation
 *
 * \author Adrian Guedes
 * \date Jul 07, 2010
 */

/**
 * @addtogroup Plugin
 *
 * \brief Plugin module provides different implementations for communication features like:
 * network, multithreading, time scheduling.
 *
 * @{
 */

#include <stdlib.h>
#include "src/communication/plugin/plugin.h"

/**
 * Stub function implementation
 */
static void stub_thread_lock_ptr(PluginContext *ctx)
{

}
/**
 * Stub function implementation
 */
static void stub_thread_unlock_ptr(PluginContext *ctx)
{

}
/**
 * Stub function implementation
 */
static void stub_thread_init_ptr(PluginContext *ctx)
{

}
/**
 * Stub function implementation
 */
static void stub_thread_finalize_ptr(PluginContext *ctx)
{

}
/**
 * Stub function implementation
 */
static int stub_network_init_ptr()
{
	return NETWORK_ERROR_NONE;
}
/**
 * Stub function implementation
 */
static int stub_network_finalize_ptr()
{
	return 0;
}
/**
 * Stub function implementation
 */
static ByteStreamReader *stub_network_get_apdu_stream_ptr(PluginContext *ctx)
{
	return NULL;
}
/**
 * Stub function implementation
 */
static int stub_network_wait_for_data_ptr(PluginContext *ctx)
{
	return 0;
}
/**
 * Stub function implementation
 */
static int stub_network_send_apdu_stream_ptr(PluginContext *ctx, ByteStreamWriter *stream)
{
	return 0;
}
/**
 * Stub function implementation
 */
static void stub_timer_wait_for_timeout_ptr(PluginContext *ctx)
{
}
/**
 * Stub function implementation
 */
static void stub_timer_reset_timeout_ptr(PluginContext *ctx)
{

}
/**
 * Stub function implementation
 */
static int stub_timer_count_timeout_ptr(PluginContext *ctx)
{
	return 0;

}

/**
 * Creates a stub plugin implementation
 *
 * @return default plugin
 */
CommunicationPlugin communication_plugin()
{
	CommunicationPlugin
	plugin = {
		.network_init = stub_network_init_ptr,
		.network_wait_for_data = stub_network_wait_for_data_ptr,
		.network_get_apdu_stream = stub_network_get_apdu_stream_ptr,
		.network_send_apdu_stream = stub_network_send_apdu_stream_ptr,
		.network_finalize = stub_network_finalize_ptr,
		.thread_lock = stub_thread_lock_ptr,
		.thread_unlock = stub_thread_unlock_ptr,
		.thread_init = stub_thread_init_ptr,
		.thread_finalize = stub_thread_finalize_ptr,
		.timer_count_timeout = stub_timer_count_timeout_ptr,
		.timer_reset_timeout = stub_timer_reset_timeout_ptr,
		.timer_wait_for_timeout = stub_timer_wait_for_timeout_ptr,
		.type = 0,
	};

	return plugin;
}

/**
 * Clear plugin structure.
 * @param plugin to CommunicationPlugin struct to clear.
 */
void communication_plugin_clear(CommunicationPlugin *plugin)
{
	plugin->network_init = NULL;
	plugin->network_wait_for_data = NULL;
	plugin->network_get_apdu_stream = NULL;
	plugin->network_send_apdu_stream = NULL;
	plugin->network_finalize = NULL;
	plugin->thread_lock = NULL;
	plugin->thread_unlock = NULL;
	plugin->thread_init = NULL;
	plugin->thread_finalize = NULL;
	plugin->timer_count_timeout = NULL;
	plugin->timer_reset_timeout = NULL;
	plugin->timer_wait_for_timeout = NULL;
}

/** @} */
