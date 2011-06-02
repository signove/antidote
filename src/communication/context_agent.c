/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file context_agent.c
 * \brief Connection Context Agent source.
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
 * \date May 31, 2011
 */

/**
 * \defgroup ContextAgent Connection Context Agent
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
#include "context_agent.h"
#include "src/util/log.h"
#include "src/util/linkedlist.h"
#include <stdlib.h>

/**
 * List of the context.
 */
static LinkedList *agent_context_list = NULL;

/**
 * @brief Destroys the given context.
 *
 * Deletes context element and remove if from list.
 *
 * @param element context to be cleaned up.
 * @return 1 if the context was successfully destroyed.
 */
static int agent_destroy_context(void *element)
{
	Context *context = (Context *) element;

	if (context != NULL) {
		DEBUG(" communication: destroying context %llx", context->id);

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
static int agent_context_search_by_id(void *arg, void *element)
{
	ContextId *id = (ContextId *) arg;

	Context *c = (Context *) element;

	if (c == NULL) {
		return 0;
	}

	return *id == c->id;
}

/**
 * @brief Creates execution context.
 *
 * If the context ID already exists, it will be recreated.
 *
 * @param id id to be verified.
 * @return Context pointer or NULL if cannot create one.
 */
Context *agent_context_create(ContextId id)
{

	if (agent_context_list == NULL) {
		agent_context_list = llist_new();
	}

	// Remove if exists any previous
	context_remove(id);

	Context *context = calloc(1, sizeof(struct Context));

	if (context == NULL || !llist_add(agent_context_list, context)) {
		ERROR("Cannot create context  %llx", context->id);
		return NULL;
	}

	context->type = AGENT_CONTEXT;
	context->fsm = fsm_instance();
	fsm_set_agent_state_table(context->fsm);

	context->id = id;

	DEBUG("Created context id %llx", context->id);

	return context;
}

/**
 * @brief Destroys execution context.
 *
 * @param id ID of the context to be cleaned up.
 */
void agent_context_remove(ContextId id)
{
	Context *context = context_get(id);

	if (context != NULL) {
		llist_remove(agent_context_list, context);
	}

	agent_destroy_context(context);
}

/**
 * @brief Destroys all execution context.
 */
void agent_context_remove_all()
{
	llist_destroy(agent_context_list, &agent_destroy_context);
	agent_context_list = NULL;
}

/**
 * @brief Get execution context.
 *
 * @return context or NULL if cannot find.
 */
Context *agent_context_get(ContextId id)
{
	Context *ctx = (Context *) llist_search_first(agent_context_list, &id,
			&agent_context_search_by_id);

	if (ctx == NULL) {
		WARNING("Cannot find context id %llx", id);
	}

	return ctx;
}

/**
 * @brief Iterate over all contexts and call context_handle for each one.
 *
 * @param function Handle function called at each iterated element.
 */
void agent_context_iterate(context_handle function)
{
	llist_iterate(agent_context_list, (llist_handle_element) function);
}

/** @} */
