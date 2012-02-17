/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_trans.c
 * \brief Transcoding "dummy" plug-in interface source.
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
 * \author Elvis Pfutzenreuter
 * \date Dec 01, 2011
 */

/**
 * @addtogroup Transcoding
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "src/communication/plugin/plugin.h"
#include "src/communication/communication.h"
#include "src/trans/trans.h"
#include "src/util/log.h"

// From src/trans/trans.c
extern CommunicationPlugin *trans_comm_plugin;

/**
 * Starts Transcoding dummy plug-in.
 *
 * @return success status
 */
static int init()
{
	DEBUG("Starting Transcoding plugin...");
	return NETWORK_ERROR_NONE;
}

/**
 * Stops dummy transcoding plug-in. May be restarted again afterwards.
 *
 * @return success status
 */
static int finalize()
{
	DEBUG("Finalizing Transcoding plugin...");
	return NETWORK_ERROR_NONE;
}


/**
 * Captures tentative of reading APDU 
 * (should not happen for a transoded device)
 *
 * @return a byteStream with the read APDU.
 */
static ByteStreamReader *get_apdu(struct Context *ctx)
{
	DEBUG("##################");
	DEBUG("Error: transcoding context %d trying to read APDU", (int) ctx->id.connid);
	DEBUG("##################");

	return NULL;
}


/**
 * Captures tentative to send APDU
 * (should not happen for transcoded device)
 *
 * @return success status
 */
static int send_apdu_stream(struct Context *ctx, ByteStreamWriter *stream)
{
	DEBUG("##################");
	DEBUG("Error: transcoding context %d trying to send APDU", (int) ctx->id.connid);
	DEBUG("##################");

	return NETWORK_ERROR_NONE;
}

/**
 * Forces closure of a channel. Forwards request to transcoding layer.
 */
static int force_disconnect_channel(Context *c)
{
	trans_force_disconnect(c->id);
	return 1;
}

/**
 * Configure dummy transcoding plugin. This plugin is basically
 * a placeholder that aids transcoding debugging. The only real
 * tasks that this plugin intermediates are: forcing disconnections
 * and timer servicing.
 *
 * @param plugin Pointer to Communication Plugin struct
 */
void plugin_trans_setup(CommunicationPlugin *plugin)
{
	trans_comm_plugin = plugin;
	plugin->type |= TRANS_CONTEXT;

	plugin->network_init = init;
	plugin->network_get_apdu_stream = get_apdu;
	plugin->network_send_apdu_stream = send_apdu_stream;
	plugin->network_disconnect = force_disconnect_channel;
	plugin->network_finalize = finalize;
}


/** @} */
