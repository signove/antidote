/**
 * \file plugin_bluez.h
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
 * \author Mateus Lima
 * \date Jul 14, 2010
 */


/**
 * Bluez plugin listener definition
 */
typedef struct PluginBluezListener {
	/**
	 * Called when agent connects
	 */
	gboolean (*peer_connected)(ContextId cid, const char *btaddr);

	/**
	 * Called when agent disconnects
	 */
	gboolean (*peer_disconnected)(ContextId cid, const char *btaddr);
} PluginBluezListener;

void plugin_bluez_setup(CommunicationPlugin *plugin);
void plugin_bluez_set_listener(PluginBluezListener *plugin);
gboolean plugin_bluez_update_data_types(gboolean is_sink, guint16 data_types[]);
gboolean plugin_bluez_connect(const char *btaddr, guint16 data_type,
				int reliability);

#define HDP_CHANNEL_RELIABLE 1
#define HDP_CHANNEL_STREAMING 2
#define HDP_CHANNEL_ANY 3
