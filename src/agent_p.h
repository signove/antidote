/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file agent.h
 * \brief Protocol agent definitions.
 * \brief Protocol agent private definitions.
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

#ifndef AGENT_P_H_
#define AGENT_P_H_

#include "src/agent.h"
#include "src/communication/service.h"


/**
 * The unique system ID of the agent. EUI-64 is used to uniquely identify the agent. An agent may
 * use this field to determine whether it is communicating with the intended agent.
 *
 * See http://standards.ieee.org/regauth/index.html
 */
void agent_remove_all_listeners();

int agent_notify_evt_device_available(Context *ctx, DataList *data_list);

int agent_notify_evt_device_unavailable(Context *ctx);

int  agent_notify_evt_measurement_data_updated(Context *ctx, DataList *data_list);

int  agent_notify_evt_timeout(Context *ctx);

void agent_request_association_release(ContextId id);

void agent_request_association_abort(ContextId id);

#endif /* AGENT_P_H_ */
