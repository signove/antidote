/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_dbus.h
 * \brief DBus plugin header.
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
 * \author Mateus Lima
 * \date Jul 14, 2010
 */


#ifndef PLUGIN_NETWORK_H_
#define PLUGIN_NETWORK_H_

#include <communication/plugin/plugin.h>
#include <dbus/dbus.h>


int plugin_network_dbus_setup(CommunicationPlugin *plugin, int timeout);


int plugin_network_dbus_read_msg(DBusMessage *msg);

/**
 * Read
 * Must be implemented in the client application
 */
int plugin_network_dbus_wait_for_data();

/**
 * Receive the created D-BUS connection.
 * Must be implemented in the client application.
 */
void plugin_network_dbus_handle_created_connection(DBusConnection *conn);


#endif /* PLUGIN_NETWORK_H_ */
