/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file healthd_common.c
 * \brief Health manager service - common functions
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
 * \author Walter Guerra
 * \author Fabricio Silva
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <ieee11073.h>
#include "src/util/log.h"
#include "src/util/linkedlist.h"
#include "src/communication/service.h"
#include "src/dim/pmstore_req.h"
#include "healthd_ipc.h"
#include "healthd_service.h"

extern healthd_ipc ipc;

/**
 * Callback for when new data has been received.
 *
 * @param ctx current context.
 * @param list a pointer to data list.
 */
void new_data_received(Context *ctx, DataList *list)
{
	DEBUG("Medical Device System Data");

	char *data = xml_encode_data_list(list);

	if (data) {
		ipc.call_agent_measurementdata(ctx->id, data);
		free(data);
	}
}

typedef struct {
	ContextId id;
	int handle;
	int instnumber;
	DataList *list;
} segment_data_evt;

/**
 * Delayed PM-Segment data handling
 *
 * @param data pointer to event struct
 * @return always FALSE
 */
static void segment_data_received_phase2(void *revt)
{
	segment_data_evt *evt = revt;

	DEBUG("PM-Segment Data phase 2");

	char *data = xml_encode_data_list(evt->list);

	if (data) {
		ipc.call_agent_segmentdata(evt->id, evt->handle, evt->instnumber, data);
		free(data);
	}

	data_list_del(evt->list);
	free(revt);
}

/**
 * Callback for when PM-Segment data has been received.
 *
 * @param ctx current context.
 * @param handle the PM-Store handle
 * @param instnumber the PM-Segment instance number
 * @param list a pointer to data list.
 */
void segment_data_received(Context *ctx, int handle, int instnumber, DataList *list)
{
	DEBUG("PM-Segment Data");
	segment_data_evt *evt = calloc(1, sizeof(segment_data_evt));

	// Different from other callback events, "list" is not freed by core, but
	// it is passed ownership instead.

	// Encoding a whole PM-Segment to XML may take a *LONG* time. If the program
	// is single-threaded, encoding here would block the 11073 stack, causing
	// the agent to abort because it didn't get confirmation in time.

	// So, encoding XML from data list is better left to a thread, or, at very
	// least, delayed until there are no pending events.

	evt->id = ctx->id;
	evt->handle = handle;
	evt->instnumber = instnumber;
	evt->list = list;

	healthd_idle_add(segment_data_received_phase2, evt);
}


/**
 * Device has been associated.
 *
 * @param ctx current context.
 * @param list the data list of elements.
 */
void device_associated(Context *ctx, DataList *list)
{
	DEBUG("Device associated");

	char *data = xml_encode_data_list(list);

	if (data) {
		ipc.call_agent_associated(ctx->id, data);
		free(data);
	}
}

/**
 * Device has connected
 *
 * @param ctx current context.
 * @param low_addr low-level transport address
 */
int device_connected(Context *ctx, const char *low_addr)
{
	DEBUG("Device connected");
	ipc.call_agent_connected(ctx->id, low_addr);
	return 1;
}

/**
 * Device has disconnected
 *
 * @param ctx current context.
 * @param low_addr low-level transport address
 */
int device_disconnected(Context *ctx, const char *low_addr)
{
	DEBUG("Device disconnected");
	ipc.call_agent_disconnected(ctx->id, low_addr);
	return 1;
}

/**
 * Device has been disassociated
 */
void device_disassociated(Context *ctx)
{
	DEBUG("Device unassociated");
	ipc.call_agent_disassociated(ctx->id);
}

/**
 * Callback used to request mds attributes
 *
 * \param ctx Context
 * \param r Related request struct (the same returned to request caller)
 * \param response_apdu Data APDU
 */
static void device_reqmdsattr_callback(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	DEBUG("Medical Device Attributes");

	DataList *list = manager_get_mds_attributes(ctx->id);

	if (list) {
		char *data = xml_encode_data_list(list);
		if (data) {
			ipc.call_agent_deviceattributes(ctx->id, data);
			free(data);
		}
		data_list_del(list);
	}
}


/** DBUS facade to request mds attributes
 *
 * \param ctx
 */
void device_reqmdsattr(ContextId ctx)
{
	DEBUG("device_reqmdsattr");
	manager_request_get_all_mds_attributes(ctx, device_reqmdsattr_callback);
}

/**
 * get device configuration
 *
 * \param ctx
 * \param xml_out pointer to string to be filled with result XML
 */
void device_getconfig(ContextId ctx, char** xml_out)
{
	DataList *list;

	DEBUG("device_getconfig");
	list = manager_get_configuration(ctx);

	if (list) {
		*xml_out = xml_encode_data_list(list);
		data_list_del(list);
	} else {
		*xml_out = strdup("");
	}
}

/**
 * request measuremens
 *
 * \param ctx
 */
void device_reqmeasurement(ContextId ctx)
{
	DEBUG("device_reqmeasurement");
	manager_request_measurement_data_transmission(ctx, NULL);
}

/**
 * Callback for Set-Time operation
 *
 * \param ctx Context
 * \param r Related request struct (the same returned to request caller)
 * \param response_apdu Data APDU
 */
static void device_set_time_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	DEBUG("device_set_time_cb");
}

/**
 * Request set-time
 *
 * \param ctx
 * \param time in time_t format
 */
void device_set_time(ContextId ctx, unsigned long long time)
{
	DEBUG("device_set_time");
	manager_set_time(ctx, time, device_set_time_cb);
}

/**
 * activate scanner
 *
 * \param ctx Context id
 * \param handle Object handle
 */
void device_reqactivationscanner(ContextId ctx, int handle)
{
	DEBUG("device_reqactivationscanner");
	manager_set_operational_state_of_the_scanner(ctx, (ASN1_HANDLE) handle,
							os_enabled, NULL);
}

/**
 * deactivate scanner
 *
 * \param ctx 
 * \param handle Object handle
 */
void device_reqdeactivationscanner(ContextId ctx, int handle)
{
	DEBUG("device_reqdeactivationscanner");
	manager_set_operational_state_of_the_scanner(ctx, (ASN1_HANDLE) handle,
							os_disabled, NULL);
}

/**
 * release association
 *
 * \param ctx
 */
void device_releaseassoc(ContextId ctx)
{
	DEBUG("device_releaseassoc");
	manager_request_association_release(ctx);
}

/**
 * Abort association
 *
 * \param ctx
 */
void device_abortassoc(ContextId ctx)
{
	DEBUG("device_abortassoc");
	manager_request_association_release(ctx);
}

/**
 * Callback for PM-Store GET
 *
 * \param ctx Context
 * \param r Request object
 * \param response_apdu response Data APDU
 */
static void device_get_pmstore_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreGetRet *ret = (PMStoreGetRet*) r->return_data;
	DataList *list;
	char *data;

	DEBUG("device_get_pmstore_cb");

	if (!ret)
		return;

	if (ret->error) {
		// some error
		ipc.call_agent_pmstoredata(ctx->id, ret->handle, "");
		return;
	}

	if ((list = manager_get_pmstore_data(ctx->id, ret->handle))) {
		if ((data = xml_encode_data_list(list))) {
			ipc.call_agent_pmstoredata(ctx->id, ret->handle, data);
			free(data);
			data_list_del(list);
		}
	}
}

/**
 * Get PM-Store attributes
 *
 * \param ctx
 * \param handle PM-Store handle
 * \param ret Preliminary return status (actual data goes via Agent callback)
 */
void device_get_pmstore(ContextId ctx, int handle, int* ret)
{
	DEBUG("device_get_pmstore");
	Request *r = manager_request_get_pmstore(ctx, handle, device_get_pmstore_cb);
	*ret = 0;
	if (!r) {
		DEBUG("no pmstore with handle %d", handle);
		*ret = 1;
	}
}

/**
 * Callback for PM-Store get segment info response
 *
 * \param ctx
 * \param r Request object
 * \param response_apdu
 */
static void device_get_segminfo_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreGetSegmInfoRet *ret = (PMStoreGetSegmInfoRet*) r->return_data;
	DataList *list;
	char *data;

	if (!ret)
		return;

	if ((list = manager_get_segment_info_data(ctx->id, ret->handle))) {
		if ((data = xml_encode_data_list(list))) {
			ipc.call_agent_segmentinfo(ctx->id, ret->handle, data);
			free(data);
			data_list_del(list);
		}
	}
}

/**
 * Callback for PM-Store segment data response
 *
 * \param ctx
 * \param r Request struct, the same originally returned to invoker
 * \param response_apdu Response data APDU
 */
static void device_get_segmdata_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreGetSegmDataRet *ret = (PMStoreGetSegmDataRet*) r->return_data;

	if (!ret)
		return;

	ipc.call_agent_segmentdataresponse(ctx->id, ret->handle, ret->inst, ret->error);
}

/**
 * Callback for PM-Store clear segment response
 *
 * \param ctx
 * \param r Request struct, the same originally returned to invoker
 * \param response_apdu
 */
static void device_clear_segm_cb(Context *ctx, Request *r, DATA_apdu *response_apdu)
{
	PMStoreClearSegmRet *ret = (PMStoreClearSegmRet*) r->return_data;

	if (!ret)
		return;

	ipc.call_agent_segmentcleared(ctx->id, ret->handle, ret->inst, ret->error);
}

/**
 * Get segments info from a PM-Store
 *
 * \param ctx
 * \param handle PM-Store handle
 * \param ret Preliminary return status (actual data goes via Agent callback)
 */
void device_get_segminfo(ContextId ctx, int handle, int* ret)
{
	Request *req;

	DEBUG("device_get_segminfo");
	req = manager_request_get_segment_info(ctx, handle,
						device_get_segminfo_cb);
	*ret = req ? 0 : 1;
}

/**
 * Get segments data from a PM-Segment
 *
 * \param ctx
 * \param handle PM-Store handle
 * \param instnumber PM-Segment InstNumber
 * \param ret Preliminary return status (actual data goes via Agent callback)
 */
void device_get_segmdata(ContextId ctx, int handle, int instnumber,
				int* ret)
{
	Request *req;

	DEBUG("device_get_segmdata");
	req = manager_request_get_segment_data(ctx, handle,
				instnumber, device_get_segmdata_cb);
	*ret = req ? 0 : 1;
}

/**
 * Clear a PM-store segment
 *
 * \param ctx
 * \param handle PM-Store handle
 * \param instnumber PM-Segment InstNumber
 * \param ret Preliminary return status (actual data goes via Agent callback)
 */
void device_clearsegmdata(ContextId ctx, int handle, int instnumber,
				int *ret)
{
	Request *req;

	DEBUG("device_clearsegmdata");
	req = manager_request_clear_segment(ctx, handle,
				instnumber, device_clear_segm_cb);
	*ret = req ? 0 : 1;
}

/**
 * Clear all segments of a PM-Store
 *
 * \param ctx
 * \param handle PM-Store handle
 * \param ret Preliminary return status (actual data goes via Agent callback)
 */
void device_clearallsegmdata(ContextId ctx, int handle, int *ret)
{
	Request *req;

	DEBUG("device_clearsegmdata");
	req = manager_request_clear_segments(ctx, handle,
				device_clear_segm_cb);
	*ret = req ? 0 : 1;
}

/** @} */
