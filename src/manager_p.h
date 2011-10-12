/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file manager.h
 * \brief Protocol manager definitions.
 * \brief Protocol manager private definitions.
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
 * \date Dec 06, 2010
 */

#ifndef MANAGER_P_H_
#define MANAGER_P_H_

#include "src/manager.h"


intu8 *manager_system_id();
unsigned short int manager_system_id_length();

void manager_remove_all_listeners();

int manager_notify_evt_device_available(Context *ctx, DataList *data_list);

int manager_notify_evt_device_unavailable(Context *ctx);

int manager_notify_evt_measurement_data_updated(Context *ctx, DataList *data_list);

int manager_notify_evt_timeout(Context *ctx);

int manager_notify_evt_segment_data(Context *ctx, int handle, int instnumber,
					DataList *data_list);

#endif /* MAINAPP_H_ */
