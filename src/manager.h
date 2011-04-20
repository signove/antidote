/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file manager.h
 * \brief Protocol manager public definitions.
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
 * \author Adrian Guedes
 * \author Fabricio Silva
 * \date Jun 22, 2010
 */

#ifndef MANAGER_H_
#define MANAGER_H_

#include <api/api_definitions.h>
#include <communication/context.h>
#include <communication/plugin/plugin.h>

/**
 * Manager event listener definition
 */
typedef struct ManagerListener {
	/**
	 *  Called when Medical Measurement is received and stored
	 */
	void (*measurement_data_updated)(Context *ctx, DataList *list);
	/**
	 * Called after device is operational
	 */
	void (*device_available)(Context *ctx, DataList *list);
	/**
	 * Called after device is not operational
	 */
	void (*device_unavailable)(Context *ctx);
	/**
	 * Called after timeout occurs
	 */
	void (*timeout)(Context *ctx);
} ManagerListener;

#define MANAGER_LISTENER_EMPTY {\
			.measurement_data_updated = NULL,\
			.device_available = NULL,\
			.device_unavailable = NULL,\
			.timeout = NULL\
			}

void manager_init(CommunicationPlugin plugin);

void manager_finalize();

void manager_start();

void manager_stop();

void manager_connection_loop(ContextId context_id);

int manager_add_listener(ManagerListener listener);

DataList *manager_get_mds_attributes(ContextId id);

#endif /* MANAGER_H_ */
