/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file context_manager.h
 * \brief Connection Context Manager header.
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
 * \date Aug 26, 2010
 */

#ifndef CONTEXT_MANAGER_H_
#define CONTEXT_MANAGER_H_

#include <communication/context.h>

/**
 * Function to handle context during an iteration
 * @return If function return 0, the iteration stops,
 * if not it continues to next element.
 */
typedef int (*context_handle)(Context *ctx);

Context *context_create(ContextId id, int type);
void context_remove(ContextId id);
void context_remove_all();
Context *context_get(ContextId id);
void context_iterate(context_handle function);

#endif /* CONTEXT_MANAGER_H_ */
