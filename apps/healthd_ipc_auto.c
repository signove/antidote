/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file healthd_ipc_auto.c
 * \brief Health manager service - TCP IPC
 *
 * Copyright (C) 2012 Signove Tecnologia Corporation.
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
 * \date Apr 18, 2012
 */

/**
 * @addtogroup Healthd
 * @{
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "src/communication/context_manager.h"
#include "src/util/log.h"
#include "src/util/linkedlist.h"
#include "healthd_common.h"
#include "healthd_service.h"
#include "healthd_ipc.h"
#include "healthd_ipc_auto.h"

static void announce(const char *command, ContextId ctx, const char *arg)
{
	char *msg;

	if (asprintf(&msg, "%s\t%d:%llu\t%s\n", command, ctx.plugin, ctx.connid, arg) < 0) {
		return;
	}

	printf("------------------------------\n");
	printf("(Auto-test) %s\n", msg);
	printf("------------------------------\n");
	
	free(msg);
}

static void self_configure()
{
	uint16_t hdp_data_types[] = {0x1004, 0x1007, 0x1029, 0x100f, 0x0};
	hdp_types_configure(hdp_data_types);
}

/**
 * Function that calls agent.Connected method.
 *
 * @param ctx Context ID
 * @param low_addr Device address e.g. Bluetooth MAC
 * @return TRUE if success
 */
static void call_agent_connected(ContextId ctx, const char *low_addr)
{
	DEBUG("call_agent_connected");
	announce("CONNECTED", ctx, low_addr);
}

/**
 * Function that calls agent.Associated method.
 *
 * @param ctx Context ID
 * @param xml Data in XML format
 * @return success status
 */
static void call_agent_associated(ContextId ctx, char *xml)
{
	DEBUG("call_agent_associated");
	announce("ASSOCIATED", ctx, "");
	announce("DESCRIPTION", ctx, xml);
}

/**
 * Function that calls agent.MeasurementData method.
 *
 * @param ctx device handle
 * @param xml Data in xml format
 * @return success status
 */
static void call_agent_measurementdata(ContextId ctx, char *xml)
{
	DEBUG("call_agent_measurementdata");
	announce("MEASUREMENT", ctx, xml);
}

/**
 * Function that calls agent.SegmentInfo method.
 *
 * @param ctx Context ID
 * @param handle PM-Store handle
 * @param xml PM-Segment instance data in XML format
 * @return success status
 */
static void call_agent_segmentinfo(ContextId ctx, unsigned int handle, char *xml)
{
	DEBUG("call_agent_segmentinfo");

	char *params;
	if (asprintf(&params, "%d %s", handle, xml) < 0) {
		return; // FALSE;
	}
	announce("SEGMENTINFO", ctx, params);
	free(params);
}


/**
 * Function that calls agent.SegmentDataResponse method.
 *
 * @param ctx device handle
 * @param handle PM-Store handle
 * @param instnumber PM-Segment instance number
 * @param status Return status
 * @return success status
 */
static void call_agent_segmentdataresponse(ContextId ctx,
			unsigned int handle, unsigned int instnumber,
			unsigned int retstatus)
{
	DEBUG("call_agent_segmentdataresponse");

	char *params;
	if (asprintf(&params, "%d %d %d", handle, instnumber, retstatus) < 0) {
		return; // FALSE;
	}
	announce("SEGMENTDATARESPONSE", ctx, params);
	free(params);
}


/**
 * Function that calls agent.SegmentData method.
 *
 * @param ctx device handle
 * @param handle PM-Store handle
 * @param instnumber PM-Segment instance number
 * @param xml PM-Segment instance data in XML format
 * @return success status
 */
static void call_agent_segmentdata(ContextId ctx, unsigned int handle,
					unsigned int instnumber, char *xml)
{
	DEBUG("call_agent_segmentdata");

	char *params;
	if (asprintf(&params, "%d %d %s", handle, instnumber, xml) < 0) {
		return; // FALSE;
	}
	announce("SEGMENTDATA", ctx, params);
	free(params);
}


/**
 * Function that calls agent.PMStoreData method.
 *
 * @param ctx device handle
 * @param handle PM-Store handle
 * @param xml PM-Store data attributes in XML format
 * @return success status
 */
static void call_agent_pmstoredata(ContextId ctx, unsigned int handle, char *xml)
{
	DEBUG("call_agent_pmstoredata");

	char *params;
	if (asprintf(&params, "%d %s", handle, xml) < 0) {
		return; // FALSE;
	}
	announce("PMSTOREDATA", ctx, params);
	free(params);
}


/**
 * Function that calls agent.SegmentCleared method.
 *
 * @param ctx device handle
 * @param handle PM-Store handle
 * @param PM-Segment instance number
 * @return success status
 */
static void call_agent_segmentcleared(ContextId ctx, unsigned int handle,
							unsigned int instnumber,
							unsigned int retstatus)
{
	char *params;
	if (asprintf(&params, "%d %d %d", handle, instnumber, retstatus) < 0) {
		return; // FALSE;
	}
	announce("SEGMENTCLEARED", ctx, params);
	free(params);
}


/**
 * Function that calls agent.DeviceAttributes method.
 *
 * @param ctx Context ID
 * @param xml Data in xml format
 * @return success status
 */
static void call_agent_deviceattributes(ContextId ctx, char *xml)
{
	announce("ATTRIBUTES", ctx, xml);
}

/**
 * Function that calls agent.Disassociated method.
 *
 * @param ctx Context ID
 * @return success status
 */
static void call_agent_disassociated(ContextId ctx)
{
	DEBUG("call_agent_disassociated");
	announce("DISASSOCIATE", ctx, "");
}

/**
 * Function that calls agent.Disconnected method.
 *
 * @param ctx Context ID
 * @param low_addr Device address e.g. Bluetooth MAC
 * @return success status
 */
static void call_agent_disconnected(ContextId ctx, const char *low_addr)
{
	DEBUG("call_agent_disconnected");
	announce("DISCONNECT", ctx, "");
}

static void start()
{
	self_configure();
}

static void stop()
{
}

void healthd_ipc_auto_init(healthd_ipc *ipc)
{
	ipc->call_agent_measurementdata = call_agent_measurementdata;
	ipc->call_agent_connected = &call_agent_connected;
	ipc->call_agent_disconnected = &call_agent_disconnected;
	ipc->call_agent_associated = &call_agent_associated;
	ipc->call_agent_disassociated = &call_agent_disassociated;
	ipc->call_agent_segmentinfo = &call_agent_segmentinfo;
	ipc->call_agent_segmentdataresponse = &call_agent_segmentdataresponse;
	ipc->call_agent_segmentdata = &call_agent_segmentdata;
	ipc->call_agent_segmentcleared = &call_agent_segmentcleared;
	ipc->call_agent_pmstoredata = &call_agent_pmstoredata;
	ipc->call_agent_deviceattributes = &call_agent_deviceattributes;
	ipc->start = &start;
	ipc->stop = &stop;
}

/** @} */
