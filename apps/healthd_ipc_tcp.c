/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file healthd_ipc_tcp.c
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <glib.h>
#include <gio/gio.h>
#include "src/communication/context_manager.h"
#include "src/util/log.h"
#include "src/util/linkedlist.h"
#include "healthd_common.h"
#include "healthd_service.h"
#include "healthd_ipc.h"

/* TCP clients */

typedef struct {
	int fd;
	char *buf;
} tcp_client;

static const unsigned int PORT = 9005;
static LinkedList *_tcp_clients = NULL;
static int server_fd = -1;

static LinkedList *tcp_clients()
{
	if ( ! _tcp_clients) {
		_tcp_clients = llist_new();
	}
	return _tcp_clients;
}

static void tcp_close(tcp_client *client)
{
	DEBUG("TCP: freeing client %p", client);

	shutdown(client->fd, SHUT_RDWR);
	close(client->fd);
	client->fd = -1;
	free(client->buf);
	client->buf = 0;
	llist_remove(tcp_clients(), client);
	free(client);
}

static gboolean tcp_write(GIOChannel *src, GIOCondition cond, gpointer data)
{
	tcp_client *client = (tcp_client*) data;
	gsize len = strlen(client->buf);
	gsize written;
	char *newbuf;
	gboolean more;

	if (cond != G_IO_OUT) {
		DEBUG("TCP: write: false alarm");
		return TRUE;
	}

	if (len <= 0) {
		g_io_channel_unref(src);
		return FALSE;
	}

	DEBUG("TCP: writing client %p", data);

	int fd = g_io_channel_unix_get_fd(src);
	written = send(fd, client->buf, len, 0);

	DEBUG("TCP: client %p written %d bytes", data, (int) written);

	if (written <= 0) {
		free(client->buf);
		client->buf = strdup("");
		g_io_channel_unref(src);
		return FALSE;
	}

	if (written >= len) {
		newbuf = strdup("");
		g_io_channel_unref(src);
		more = FALSE;
	} else {
		newbuf = strdup(client->buf + written);
		more = TRUE;
	}
	free(client->buf);
	client->buf = newbuf;

	return more;
}

static gboolean tcp_read(GIOChannel *src, GIOCondition cond, gpointer data)
{
	char buf[256];
	gsize count;

	DEBUG("TCP: reading client %p", data);

	tcp_client *client = (tcp_client*) data;

	if (cond != G_IO_IN) {
		tcp_close(client);
		g_io_channel_unref(src);
		return FALSE;
	}

	int fd = g_io_channel_unix_get_fd(src);
	count = recv(fd, buf, 256, 0);

	if (count == 0) {
		tcp_close(client);
		g_io_channel_unref(src);
		return FALSE;
	}

	return TRUE;
}

static void tcp_send(tcp_client *client, const char *msg)
{
	char *newbuf;

	DEBUG("TCP: scheduling write %p", client);

	if (asprintf(&newbuf, "%s%s", client->buf, msg) < 0) {
		return;
	}

	free(client->buf);
	client->buf = newbuf;

	GIOChannel *channel = g_io_channel_unix_new(client->fd);
	g_io_add_watch(channel, G_IO_OUT, tcp_write, client);
}

static gboolean tcp_accept(GIOChannel *src, GIOCondition cond, gpointer data)
{
	tcp_client *new_client;
	struct sockaddr_in addr;
	int fd;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	bzero(&addr, sizeof(addr));

	DEBUG("TCP: accepting condition");

	fd = accept(g_io_channel_unix_get_fd(src), (struct sockaddr *) &addr, &addrlen);

	if (fd < 0) {
		DEBUG("TCP: Failed accept");
		return TRUE;
	}

	new_client = g_new0(tcp_client, 1);
	new_client->fd = fd;
	new_client->buf = strdup("");

	DEBUG("TCP: adding client %p to list", new_client);

	GIOChannel *channel = g_io_channel_unix_new(fd);
	g_io_add_watch(channel, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL, tcp_read, new_client);

	llist_add(tcp_clients(), new_client);

	return TRUE;
}

static void tcp_listen()
{
	struct sockaddr_in addr;
	int reuse = 1;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);
	bind(server_fd, (struct sockaddr *) &addr, sizeof(addr));
	listen(server_fd, 5);

	GIOChannel *channel = g_io_channel_unix_new(server_fd);
	g_io_add_watch(channel, G_IO_IN, tcp_accept, 0);

	DEBUG("TCP: listening");
}

static void tcp_announce(const char *command, ContextId ctx, const char *arg)
{
	char *msg;
	char *j;
	char *arg2 = strdup(arg);

	for (j = arg2; *j; ++j)
		if ((*j == '\t') || (*j == '\n'))
			*j = ' ';

	if (asprintf(&msg, "%s\t%d:%llu\t%s\n", command, ctx.plugin, ctx.connid, arg2) < 0) {
		free(arg2);
		return;
	}

	printf("%s\n", msg);
	
	LinkedNode *i = tcp_clients()->first;

	while (i) {
		tcp_send(i->element, msg);
		i = i->next;
	}

	free(arg2);
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
	tcp_announce("CONNECTED", ctx, low_addr);
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
	tcp_announce("ASSOCIATED", ctx, "");
	tcp_announce("DESCRIPTION", ctx, xml);
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
	tcp_announce("MEASUREMENT", ctx, xml);
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
	tcp_announce("SEGMENTINFO", ctx, params);
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
	tcp_announce("SEGMENTDATARESPONSE", ctx, params);
	free(params);
}


/**
 * Function that calls agent.SegmentData method.
 *
 * @param ctx device handle
 * @param handle PM-Store handle
 * @param instnumber PM-Segment instance number
 * @param xml PM-Segment instance data in XML format
 */
static void call_agent_segmentdata(ContextId ctx, unsigned int handle,
					unsigned int instnumber, char *xml)
{
	DEBUG("call_agent_segmentdata");

	char *params;
	if (asprintf(&params, "%d %d %s", handle, instnumber, xml) < 0) {
		return; // FALSE;
	}
	tcp_announce("SEGMENTDATA", ctx, params);
	free(params);
}


/**
 * Function that calls agent.PMStoreData method.
 *
 * @param ctx device handle
 * @param handle PM-Store handle
 * @param xml PM-Store data attributes in XML format
 */
static void call_agent_pmstoredata(ContextId ctx, unsigned int handle, char *xml)
{
	DEBUG("call_agent_pmstoredata");

	char *params;
	if (asprintf(&params, "%d %s", handle, xml) < 0) {
		return; // FALSE;
	}
	tcp_announce("PMSTOREDATA", ctx, params);
	free(params);
}


/**
 * Function that calls agent.SegmentCleared method.
 *
 * @param ctx device handle
 * @param handle PM-Store handle
 * @param PM-Segment instance number
 */
static void call_agent_segmentcleared(ContextId ctx, unsigned int handle,
							unsigned int instnumber,
							unsigned int retstatus)
{
	char *params;
	if (asprintf(&params, "%d %d %d", handle, instnumber, retstatus) < 0) {
		return; // FALSE;
	}
	tcp_announce("SEGMENTCLEARED", ctx, params);
	free(params);
}


/**
 * Function that calls agent.DeviceAttributes method.
 *
 * @param ctx Context ID
 * @param xml Data in xml format
 */
static void call_agent_deviceattributes(ContextId ctx, char *xml)
{
	tcp_announce("ATTRIBUTES", ctx, xml);
}

/**
 * Function that calls agent.Disassociated method.
 *
 * @param ctx Context ID
 */
static void call_agent_disassociated(ContextId ctx)
{
	DEBUG("call_agent_disassociated");
	tcp_announce("DISASSOCIATE", ctx, "");
}

/**
 * Function that calls agent.Disconnected method.
 *
 * @param ctx Context ID
 * @param low_addr Device address e.g. Bluetooth MAC
 */
static void call_agent_disconnected(ContextId ctx, const char *low_addr)
{
	DEBUG("call_agent_disconnected");
	tcp_announce("DISCONNECT", ctx, "");
}

static void start()
{
	tcp_listen();
	self_configure();
}

static void stop()
{
}

void healthd_ipc_tcp_init(healthd_ipc *ipc)
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
