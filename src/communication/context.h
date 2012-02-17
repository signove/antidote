/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file context.h
 * \brief Context definitions.
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
 * \author Adrian Guedes
 * \author Fabricio Silva
 * \date Dec 7, 2010
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

/**
 * \ingroup Communication
 * @{
 */

/**
 * Context identification struct. Should be handled as
 * opaque most of the time, unless you are a plugin writer.
 */
typedef struct ContextId {
	/**
	 * Plugin ID, attributed by plug-in management
	 */
	unsigned int plugin;
	/**
	 * Connection ID, normally attributed by communication plug-in.
	 * Namespace of this ID is the plug-in.
	 */
	unsigned long long connid;
} ContextId;

struct MDS;
struct Service;
struct Context;

/**
 * Function prototype to represent callback action
 */
typedef void (*timer_callback_function)(struct Context *ctx);

/**
 * The callback structure
 */
typedef struct timeout_callback {
	/**
	 * Callback function that will be called after timeout
	 */
	timer_callback_function func;
	/**
	 * Timeout in seconds
	 */
	int timeout;

	/**
	 * Identifier of the callback, the id is used to reset or cancel
	 * the timer function if needed.
	 */
	unsigned int id;
} timeout_callback;

/**
 * This represents the Connection Context of IEEE Agent
 */
typedef struct Context {
	/**
         * Context is Manager or Agent
	 */
	int type;

	/**
	 * Unique value to identify agent connection
	 */
	ContextId id;

	/**
	 * Variable to control connection loop state, 1 means loop is active, 0 if not.
	 */
	int connection_loop_active;

	/**
	 * State machine.
	 */
	struct FSM *fsm;

	/**
	 * Medical Device System
	 */
	struct MDS *mds;

	/**
	 * Current service context contains pending requests queue
	 */
	struct Service *service;

	/**
	 *  This pointer is used by multithread implementation. If the thread strategy
	 *  needs to associate this context with another thread data, e.g. mutexes.
	 */
	void *multithread;


	/**
	 * The current action to be executed when time out occurs.
	 */
	timeout_callback timeout_action;

} Context;

#define MANAGER_CONTEXT 1
#define AGENT_CONTEXT 2
#define TRANS_CONTEXT 4

#endif /* CONTEXT_H_ */
