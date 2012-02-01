/**
 * \file plugin_usb.h
 * \brief BlueZ interface header.
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
 * $LICENSE_TEXT:END$ *
 *
 * \author Elvis Pfutzenreuter
 * \author Jose Luis do Nascimento
 * \date Mar 25, 2011
 */


/**
 * Usb plugin listener definition
 */
typedef struct PluginUsbListener {
	/**
	 * Called when agent connects
	 */
	gboolean (*agent_connected)(ContextId conn_handle, const char *addr);

	/**
	 * Called when agent disconnects
	 */
	gboolean (*agent_disconnected)(ContextId conn_handle, const char *addr);
} PluginUsbListener;

void plugin_usb_setup(CommunicationPlugin *plugin);
void plugin_usb_set_listener(PluginUsbListener *plugin);
