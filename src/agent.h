/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file agent.h
 * \brief Protocol agent public definitions.
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
 * \author Adrian Guedes
 * \author Fabricio Silva
 * \author Elvis Pfutzenreuter
 * \date May 31, 2011
 */

#ifndef AGENT_H_
#define AGENT_H_

#include <api/api_definitions.h>
#include <communication/context.h>
#include <communication/plugin/plugin.h>

/**
 * Agent event listener definition
 */
typedef struct AgentListener {
	/**
	 * Called after device is connected
	 */
	void (*device_connected)(Context *ctx);
	/**
	 * Called after device is operational
	 */
	void (*device_associated)(Context *ctx);
	/**
	 * Called after device is not operational
	 */
	void (*device_unavailable)(Context *ctx);
	/**
	 * Called after timeout occurs
	 */
	void (*timeout)(Context *ctx);
} AgentListener;

#define AGENT_LISTENER_EMPTY {\
			.device_connected = NULL,\
			.device_associated = NULL,\
			.device_unavailable = NULL,\
			.timeout = NULL\
			}

struct mds_system_data;

void agent_init(CommunicationPlugin **plugins, int specialization,
		void *(*event_report_cb)(),
		struct mds_system_data *(*mds_data_cb)());

void agent_finalize();

void agent_start();

void agent_stop();

void agent_connection_loop(ContextId context_id);

int agent_add_listener(AgentListener listener);

void agent_associate(ContextId id);

void agent_request_association_release(ContextId id);

void agent_disconnect(ContextId id);

void agent_send_data(ContextId id);

#endif /* AGENT_H_ */
