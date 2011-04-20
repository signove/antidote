/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file linkedlist.c
 * \brief Linked list implementation.
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
 * \date Aug 26, 2010
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

/**
 * Function type to be called when operation requires element handling
 */
typedef int (*llist_handle_element)(void *element);

/**
 * Function type to check element if matches search argument
 *
 * @param arg search argument to be used
 * @param element the linked list element
 * @return 1 if element matches, 0 if not
 */
typedef int (*llist_search_match)(void *arg, void *element);

/**
 * Represent a node of linked list
 */
typedef struct LinkedNode {
	/**
	 * Pointer to the node value.
 	 */
	void *element;

	/**
	 * Pointer to the next element (node).
	 */
	struct LinkedNode *next;
} LinkedNode;

/**
 * Represents a Linked List, and this type should be manipulated
 * only by linkedlist.c functions, which maintain the correct state of all variables.
 */
typedef struct LinkedList {
	/**
	 * Pointer to first linked list element
	 */
	LinkedNode *first;
	/**
	 * Pointer to last linked list element
	 */
	LinkedNode *last;

	/**
	 * Number of elements in list
	 */
	int size;
} LinkedList;

LinkedList *llist_new();

int llist_add(LinkedList *list, void *element);

int llist_remove(LinkedList *list, void *element);

void *llist_get(LinkedList *list, int index);

void *llist_search_first(LinkedList *list, void *arg, llist_search_match match_function);

int llist_index_of(LinkedList *list, void *element);

void llist_destroy(LinkedList *list, llist_handle_element destroy_function);

int llist_iterate(LinkedList *list, llist_handle_element handle_iteration);

#endif /* LINKEDLIST_H_ */
