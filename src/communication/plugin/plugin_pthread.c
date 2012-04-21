/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file plugin_pthread.c
 * \brief Posix thread plugin mixin source.
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
 * IEEE 11073 Communication Model - Finite State Machine implementation
 *
 * \author Fabricio Silva Epaminondas
 * \date Aug 16, 2010
 */

/**
 * @addtogroup PThreadPlugin
 * @{
 */

#include "src/communication/plugin/plugin_pthread.h"
#include "src/util/log.h"
#include "src/communication/communication.h"
#include "src/communication/context_manager.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Global stack lock, named "GIL" after Python :)
 * GIL is used when the stack needs a global protection
 * like when doing some global list manipulation
 */
static pthread_mutex_t gil;

/**
 * GIL pattributes
 */
static pthread_mutexattr_t gil_attr;

/*
 * Get multithreading control structure of a context
 * @param ctx context
 * @return ThreadContext structure
 */
static ThreadContext *get_thread_ctx(Context *ctx)
{
	if (ctx->multithread == NULL) {
		ERROR("The multithread context is not initialized");
	}

	return (ThreadContext *) ctx->multithread;
}

/**
 * Initialize thread context, in this case create pthread mutex.
 * @param ctx current context.
 */
static void plugin_pthread_ctx_init(Context *ctx)
{
	if (!ctx)
		return;

	ctx->multithread = calloc(1, sizeof(ThreadContext));
	ThreadContext *thread_ctx = (ThreadContext *) ctx->multithread;

	// Initialize communication mutex
	pthread_mutexattr_init(&thread_ctx->mutex_attr);
	pthread_mutexattr_settype(&thread_ctx->mutex_attr,
				  PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&thread_ctx->mutex, &thread_ctx->mutex_attr);
}

static void timer_reset_timeout(Context *ctx);

/**
 * Finalize thread context, in this case destroy pthread mutex.
 * @param ctx current context.
 */
static void plugin_pthread_ctx_finalize(Context *ctx)
{
	if (!ctx)
		return;

	timer_reset_timeout(ctx);

	ThreadContext *thread_ctx = get_thread_ctx(ctx);

	pthread_mutexattr_destroy(&thread_ctx->mutex_attr);
	pthread_mutex_destroy(&thread_ctx->mutex);

	free(ctx->multithread);
	ctx->multithread = NULL;
}

/**
 * Lock the context received.
 *
 * @param ctx the context that will be locked. NULL = lock GIL
 */
static void plugin_pthread_ctx_lock(Context *ctx)
{
	if (ctx) {
		ThreadContext *thread_ctx = get_thread_ctx(ctx);
		pthread_mutex_lock(&thread_ctx->mutex);
	} else {
		pthread_mutex_lock(&gil);
	}
}

/**
 * Unlock the context received.
 *
 * @param ctx the context that will be unlocked. NULL = GIL
 */
static void plugin_pthread_ctx_unlock(Context *ctx)
{
	if (ctx) {
		ThreadContext *thread_ctx = (ThreadContext *) ctx->multithread;
		pthread_mutex_unlock(&thread_ctx->mutex);
	} else {
		pthread_mutex_unlock(&gil);
	}
}

/**
 * Pthread-based implementation for timeout function
 * Runnable part
 */
static void *timer_run(void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	DEBUG(" timer: running timeout thread ");

	Context *ctx = (Context *) arg;

	timeout_callback *callback = &ctx->timeout_action;

	DEBUG(" timer: sleeping %d\n", callback->timeout);
	sleep(callback->timeout);
	DEBUG(" timer: wake up!\n");

	plugin_pthread_ctx_lock(ctx);

	if (callback->func != NULL) {
		(callback->func)(ctx);
	}

	DEBUG(" timer: finishing timeout thread.");
	ctx->timeout_action.func = NULL;
	ctx->timeout_action.timeout = 0;

	plugin_pthread_ctx_unlock(ctx);
	pthread_exit(NULL);
}

/**
 * Blocks current thread and waits for timeout thread termination.
 * This plug-in feature is actually used by unit-testing only.
 *
 * @param context
 * @return thread termination value
 */
static void timer_wait_for_timeout(Context *ctx)
{
	DEBUG(" timer: Waiting for timeout thread termination.");
	ThreadContext *thread_ctx = get_thread_ctx(ctx);
	pthread_join(*thread_ctx->timeout_thread, NULL);

	free(thread_ctx->timeout_thread);
	thread_ctx->timeout_thread = NULL;
}

/**
 * Reset timeout counter state.
 * This method locks the communication layer thread.
 * @param context
 */
static void timer_reset_timeout(Context *ctx)
{
	plugin_pthread_ctx_lock(ctx);
	ThreadContext *thread_ctx = get_thread_ctx(ctx);

	if (thread_ctx != NULL && thread_ctx->timeout_thread != NULL) {
		DEBUG(" timer: Reseting timeout thread ");

		pthread_cancel(*thread_ctx->timeout_thread);
		pthread_join(*thread_ctx->timeout_thread, NULL);
	
		free(thread_ctx->timeout_thread);
		thread_ctx->timeout_thread = NULL;
	}

	plugin_pthread_ctx_unlock(ctx);
}

/**
 * Pthread-based implementation for timeout function
 *
 * @param context
 * @return timer id
 */
static int timer_count_timeout(Context *ctx)
{
	static int last_id = 0;

	plugin_pthread_ctx_lock(ctx);

	timer_reset_timeout(ctx);
	ThreadContext *thread_ctx = get_thread_ctx(ctx);

	if (thread_ctx->timeout_thread == NULL) {
		thread_ctx->timeout_thread = malloc(sizeof(pthread_t));
		ctx->timeout_action.id = ++last_id;

		DEBUG("timer: Creating timeout counter thread id %d, time: %d",
		      ctx->timeout_action.id,
		      ctx->timeout_action.timeout);

		int return_code = pthread_create(thread_ctx->timeout_thread,
						 NULL, timer_run, (void *) ctx);

		if (return_code) {
			DEBUG("ERROR timer: return code from "
			      "pthread_create() is %d", return_code);
			return 0;
		}
	}

	plugin_pthread_ctx_unlock(ctx);
	return ctx->timeout_action.id;
}

/**
 * Setups PTHREAD support for communication lock/unlock operations
 * and for timer features.
 * Note: Since pthread is a "mixin" plug-in that adds multithreading
 * support e.g. context locking and unlocking.
 * It is not a complete communication plugin, so the CommunicationPlugin
 * struct must be completed by another plugin that actually implements
 * communication.
 *
 * @param plugin
 */
void plugin_pthread_setup(CommunicationPlugin *plugin)
{
	plugin->thread_init = plugin_pthread_ctx_init;
	plugin->thread_finalize = plugin_pthread_ctx_finalize;
	plugin->thread_lock = plugin_pthread_ctx_lock;
	plugin->thread_unlock = plugin_pthread_ctx_unlock;
	plugin->timer_count_timeout = timer_count_timeout;
	plugin->timer_reset_timeout = timer_reset_timeout;
	plugin->timer_wait_for_timeout = timer_wait_for_timeout;

	// Initialize GIL
	pthread_mutexattr_init(&gil_attr);
	pthread_mutexattr_settype(&gil_attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&gil, &gil_attr);
}

/** @} */
