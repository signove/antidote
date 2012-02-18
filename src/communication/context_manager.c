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
#include "src/dim/mds.h"
#include "context_manager.h"
#include "src/util/log.h"
#include "src/util/linkedlist.h"
#include <stdlib.h>

/**
 * List of the context.
 */
static LinkedList *context_list = NULL;

// TODO multithreading protection

/**
 * @brief Destroys the given context.
 *
 * Deletes context element and remove if from list.
 *
 * @param element context to be cleaned up.
 * @return 1 if the context was successfully destroyed.
 */
static int destroy_context(void *element)
{
	Context *context = (Context *) element;

	if (context != NULL) {
		DEBUG(" communication: destroying context %u:%llx",
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

	// Remove if exists any previous
	context_remove(id);

	Context *context = calloc(1, sizeof(struct Context));

	if (context == NULL || !llist_add(context_list, context)) {
		ERROR("Cannot create context %u:%llx", context->id.plugin, context->id.connid);
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

	DEBUG("Created context id %u:%llx", context->id.plugin, context->id.connid);

	return context;
}

/**
 * @brief Destroys execution context.
 *
 * @param id ID of the context to be cleaned up.
 */
void context_remove(ContextId id)
{
	Context *context = context_get(id);

	if (context != NULL) {
		llist_remove(context_list, context);
	}

	destroy_context(context);
}

/**
 * @brief Destroys all execution context.
 */
void context_remove_all()
{
	llist_destroy(context_list, &destroy_context);
	context_list = NULL;
}

/**
 * @brief Get execution context.
 *
 * @param id Context ID
 * @return pointer to context struct or NULL if cannot find.
 */
Context *context_get(ContextId id)
{
	Context *ctx = (Context *) llist_search_first(context_list, &id,
			&context_search_by_id);

	if (ctx == NULL) {
		WARNING("Cannot find context id %u:%llx", id.plugin, id.connid);
	}

	return ctx;
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
