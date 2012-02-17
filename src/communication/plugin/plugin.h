/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin.h
 * \brief Stub plugin.
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
 * \author Fabricio Silva
 *
 * \date Jul 07, 2010
 */

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <util/bytelib.h>

/**
 * Value returned to signal error condition
 */
#define NETWORK_ERROR 0

/**
 * Value returned to signal success
 */
#define NETWORK_ERROR_NONE 1

/**
 * NULL communication plugin declaration
 */
#define COMMUNICATION_PLUGIN_NULL {\
			.network_init = NULL,\
			.network_wait_for_data = NULL,\
			.network_get_apdu_stream = NULL,\
			.network_send_apdu_stream = NULL,\
			.network_disconnect = NULL,\
			.network_finalize = NULL,\
			.thread_init = NULL,\
			.thread_finalize = NULL,\
			.thread_lock = NULL,\
			.thread_unlock = NULL,\
			.timer_count_timeout = NULL,\
			.timer_wait_for_timeout = NULL,\
			.timer_reset_timeout = NULL, \
			.type = 0 \
			}

/**
 * Plugin-private typedef for Context
 */
typedef struct Context PluginContext;

/**
 * Function prototype for Multithread support
 */
typedef void (*thread_lock_ptr)(PluginContext *ctx);
/**
 * Function prototype for Multithread support
 */
typedef void (*thread_unlock_ptr)(PluginContext *ctx);
/**
 * Function prototype for Multithread support
 */
typedef void (*thread_init_ptr)(PluginContext *ctx);
/**
 * Function prototype for Multithread support
 */
typedef void (*thread_finalize_ptr)(PluginContext *ctx);

/**
 * Function prototype for Network support
 */
typedef int (*network_init_ptr)(unsigned int plugin_label);
/**
 * Function prototype for Network support
 */
typedef int (*network_finalize_ptr)();
/**
 * Function prototype for Network support
 */
typedef ByteStreamReader* (*network_get_apdu_stream_ptr)(PluginContext *ctx);
/**
 * Function prototype for Network support
 */
typedef int (*network_wait_for_data_ptr)(PluginContext *ctx);
/**
 * Function prototype for Network support
 */
typedef int (*network_send_apdu_stream_ptr)(PluginContext *ctx, ByteStreamWriter *stream);

/**
 * Function prototype for Network support
 */
typedef int (*network_disconnect_ptr)(PluginContext *ctx);

/**
 * Function prototype for Time Schedule support
 */
typedef void (*timer_wait_for_timeout_ptr)(PluginContext *ctx);
/**
 * Function prototype for Time Schedule support
 */
typedef void (*timer_reset_timeout_ptr)(PluginContext *ctx);

/**
 * Function prototype for Time Schedule support
 */
typedef int (*timer_count_timeout_ptr)(PluginContext *ctx);


/**
 * CommunicationPlugin interface definition. Its determine the
 * behaviour of communication layer.
 *
 */
typedef struct CommunicationPlugin {

	/**
	 * Initialize network layer.
	 *

	 *
	 * @return NETWORK_ERROR_NONE if operation succeeds.
	 */
	network_init_ptr network_init;

	/**
	 * Finalizes network layer and deallocated data.
	 * @return NETWORK_ERROR_NONE if operation succeeds
	 */
	network_finalize_ptr network_finalize;

	/**
	 * Reads an APDU
	 *
	 * @return a byteStream with the read APDU
	 */
	network_get_apdu_stream_ptr network_get_apdu_stream;

	/**
	 * Blocks to wait data to be available
	 *
	 * @param time_sec timeout to wait for. 0 means wait forever.
	 * @return NETWORK_ERROR_NONE if data is available or 0 if timeout.
	 */
	network_wait_for_data_ptr network_wait_for_data;

	/**
	 * Sends an encoded apdu
	 *
	 * @param stream the apdu to be sent
	 * @return NETWORK_ERROR_NONE if data sent successfully and NETWORK_ERROR otherwise
	 */
	network_send_apdu_stream_ptr network_send_apdu_stream;

	/**
	 * Closes a connection
	 */
	network_disconnect_ptr network_disconnect;

	/**
	 * Locks connection context
	 *
	 * Must be implemented by the client to support multithreading
	 */
	thread_lock_ptr thread_lock;

	/**
	 * Unlocks connection context
	 *
	 * Must be implemented by the client to support multithreading.
	 */
	thread_unlock_ptr thread_unlock;

	/**
	 * Initialize connection context mutexes
	 *
	 * Must be implemented by the client to support multithreading.
	 */
	thread_init_ptr thread_init;

	/**
	 * Finalizes connection context mutexes
	 *
	 * Must be implemented by the client to support multithreading.
	 */
	thread_finalize_ptr thread_finalize;

	/**
	 * Waits X seconds for timeout and execute callback function.
	 */
	int (*timer_count_timeout)(PluginContext *ctx);

	/**
	 * Blocks current execution until timeout function executes.
	 */
	timer_wait_for_timeout_ptr timer_wait_for_timeout;

	/**
	 * Cancel timeout function.
	 */
	timer_reset_timeout_ptr timer_reset_timeout;

	/**
	 * Plug-in type (manager or agent)
	 */
	int type;

} CommunicationPlugin;

CommunicationPlugin communication_plugin();
void communication_plugin_clear(CommunicationPlugin *plugin);


#endif /* PLUGIN_H_ */
