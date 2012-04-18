/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file context_manager.c
 * \brief Connection Context Manager source.
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
 * \author Fabricio Silva
 * \author Elvis Pfutzenreuter
 * \date Aug 26, 2010
 */

/**
 * \defgroup ContextManager Connection Context Manager
 * \ingroup FSM
 * \brief Multiple context execution handling.
 *
 * Multiple contexts are necessary to execute multiple connections to various
 *  devices.
 *
 * @{
 */

#include "src/communication/communication.h"
#include "src/communication/communication_p.h"
#include "src/dim/mds.h"
#include "context_manager.h"
#include "src/util/log.h"
#include "src/util/linkedlist.h"
#include <stdlib.h>

/**
 * List of the context.
 */
static LinkedList *context_list = NULL;


/**
 * @brief Destroys the given context.
 *
 * Deletes context element and remove if from list.
 *
 * @param context context to be cleaned up.
 * @return 1 if the context was successfully destroyed.
 */
static int destroy_context(Context *context)
{
	if (context != NULL) {
		DEBUG("destroying context %u:%llu",
				context->id.plugin, context->id.connid);

		if (context->fsm != NULL) {
			fsm_destroy(context->fsm);
			context->fsm = NULL;
		}

		if (context->mds != NULL) {
			mds_destroy(context->mds);
			context->mds = NULL;
		}

		if (context->service != NULL) {
			service_destroy(context->service);
			context->service = NULL;
		}

		if (context->multithread != NULL) {
			communication_finalize_thread_context(context);
		}

		free(context);
	}

	return 1;
}


/**
 * @brief Verify if the context element have the given id.
 *
 * @param arg ID to be compared.
 * @param element Context to be compared.
 * @return True if the element's id matches with the given ID.
 */
static int context_search_by_id(void *arg, void *element)
{
	ContextId *id = (ContextId *) arg;

	Context *c = (Context *) element;

	if (c == NULL) {
		return 0;
	}

	return (id->plugin == c->id.plugin) && (id->connid == c->id.connid);
}

/**
 * @brief Creates execution context.
 *
 * If the context ID already exists, it will be recreated.
 *
 * @param id id to be verified.
 * @param type The context type
 * @return Context pointer or NULL if cannot create one.
 */
Context *context_create(ContextId id, int type)
{
	if (context_list == NULL) {
		context_list = llist_new();
	}

	// Remove from list if exists any previous
	context_remove(id);

	Context *context = calloc(1, sizeof(struct Context));

	if (context == NULL) {
		ERROR("Cannot create context %u:%llu", context->id.plugin, context->id.connid);
		return NULL;
	}

	context->type = type;
	context->fsm = fsm_instance();

	if (type & MANAGER_CONTEXT) {
		fsm_set_manager_state_table(context->fsm);
	} else {
		fsm_set_agent_state_table(context->fsm);
	}

	context->id = id;
	context->ref = 1; // reference from list

	gil_lock();
	llist_add(context_list, context);
	gil_unlock();

	DEBUG("Created context id %u:%llu", context->id.plugin, context->id.connid);

	return context;
}

/**
 * @brief Removes execution context from list.
 *
 * @param id ID of the context to be cleaned up.
 */
void context_remove(ContextId id)
{
	DEBUG("Removing context %u:%llu", id.plugin, id.connid);

	// grab context and remove from list atomically
	gil_lock();

	Context *context = context_get_and_lock(id);
	if (!context) {
		gil_unlock();
		return;
	}

	llist_remove(context_list, context);

	gil_unlock();

	--context->ref; // remove reference from list

	context_unlock(context);
}

/**
 * @brief Destroys all execution context.
 */
void context_remove_all()
{
	while (1) {
		// make sure no one will mess the table while we
		// get one context id to be destroyed
		gil_lock();

		if ((!context_list) || (context_list->size <= 0)) {
			gil_unlock();
			break;
		}
		Context *c = llist_get(context_list, 0);
		ContextId id = c->id;

		gil_unlock();

		// safely remove
		context_remove(id);
	}

	gil_lock();
	free(context_list);
	context_list = NULL;
	gil_unlock();
}

/**
 * @brief Get execution context and lock it in a single move
 *
 * @param id Context ID
 * @return pointer to context struct or NULL if cannot find.
 */
Context *context_get_and_lock(ContextId id)
{
	gil_lock();

	Context *ctx = (Context *) llist_search_first(context_list, &id,
			&context_search_by_id);

	if (ctx == NULL) {
		WARNING("Cannot find context id %u:%llu", id.plugin, id.connid);
		gil_unlock();
		return ctx;
	}

	if (ctx) {
		communication_lock(ctx);
		++ctx->ref;
		DEBUG("Context @%p %u:%llu addref to %d", ctx,
			ctx->id.plugin, ctx->id.connid, ctx->ref);
	}

	gil_unlock();

	return ctx;
}

/**
 * @brief Shorthand to unlock execution context
 *
 * @param ctx Context pointer
 */
void context_unlock(Context *ctx)
{
	if (ctx) {
		--ctx->ref;
		communication_unlock(ctx);
		DEBUG("Context @%p %u:%llu unref to %d", ctx,
			ctx->id.plugin, ctx->id.connid, ctx->ref);
		// if ref=0, it is not on the list, so
		// nobody has ownership and nobody will find it
		// between unlocking and destruction
		if (ctx->ref <= 0) {
			destroy_context(ctx);
		}
	}
}

/**
 * @brief Iterate over all contexts and call context_handle for each one.
 *
 * @param function Handle function called at each iterated element.
 */
void context_iterate(context_handle function)
{
	llist_iterate(context_list, (llist_handle_element) function);
}

/** @} */
