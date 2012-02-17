/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file linkedlist.h
 * \brief Linked list definitions.
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

/**
 * \addtogroup Utility
 * @{
 */

#include "linkedlist.h"
#include <stdlib.h>

/**
 * Destroys a list node.
 *
 * @param node the node to be destroyed.
 */
static void destroy_node(LinkedNode *node)
{
	if (node != NULL) {
		node->element = NULL;
		node->next = NULL;
		free(node);
	}
}

/**
 * Creates a pointer to a new LinkedList.
 *
 * @return a pointer to a new LinkedList.
 */
LinkedList *llist_new()
{
	return calloc(1, sizeof(LinkedList));
}

/**
 * Adds an element at the end of a linked list.
 *
 * @param list the list to add a new element.
 * @param element the element to be added to the linked list.
 * @returns 1 if operation succeds; 0, otherwise.
 */
int llist_add(LinkedList *list, void *element)
{
	if (list != NULL) {
		LinkedNode *newNode = calloc(1, sizeof(LinkedNode));

		if (newNode != NULL) {

			if (list->first == NULL) {
				// Add the first element
				list->first = newNode;
				list->last = newNode;
			} else if (list->last != NULL) {
				// Append the last element
				list->last->next = newNode;
				list->last = newNode;
			}

			newNode->element = element;
			list->size += 1;
			return 1;
		}
	}

	return 0;
}

/**
 * Remove all occurrences of the element in linked list.
 *
 * @param list the list from which elements are removed.
 * @param element the element to be removed.
 * @returns 1 if operation succeds; otherwise, it returns 0
 */
int llist_remove(LinkedList *list, void *element)
{
	int status = 0;

	if (list != NULL) {

		LinkedNode *node = list->first;
		LinkedNode *previousNode = NULL;

		while (node != NULL) {

			if (node->element == element) {
				// Remove i element
				status = 1;

				// -- Remove first --
				if (list->first == node) {
					// Remove the first element
					list->first = node->next;
				}

				// -- Remove last --
				if (list->last == node) {
					if (previousNode != NULL) {
						previousNode->next = NULL;
					}

					list->last = previousNode;
				}

				// -- Remove middle --
				if (list->first != node && list->last != node) {
					if (previousNode != NULL) {
						previousNode->next = node->next;
					}
				}

				// update variables
				list->size -= 1;

				LinkedNode *next = node->next;
				destroy_node(node);

				previousNode = node;
				node = next;
			} else {
				previousNode = node;
				node = node->next;
			}

		}
	}

	return status;
}

/**
 * Returns the element at specified position.
 *
 * @param list the list.
 * @param index the position.
 * @return the element or \b NULL if cannot find one.
 */
void *llist_get(LinkedList *list, int index)
{
	if (list != NULL) {
		LinkedNode *node = list->first;
		int count = 0;

		while (node != NULL) {
			if (count == index) {
				return node->element;
			}

			node = node->next;
			count++;
		}
	}

	return NULL;
}

/**
 * Returns the first match of search argument
 *
 * @param list the list.
 * @param arg search argument.
 * @param match_function this function checks the argument matches the list element.
 * @return element or NULL if cannot find one.
 */
void *llist_search_first(LinkedList *list, void *arg,
			 llist_search_match match_function)
{
	if (list != NULL && match_function != NULL) {
		LinkedNode *node = list->first;

		while (node != NULL) {
			if (match_function(arg, node->element)) {
				return node->element;
			}

			node = node->next;
		}
	}

	return NULL;
}

/**
 * Returns the index of first occurrence of element
 *
 * @param list the list
 * @param element the element
 * @return the index of the first occurrence of the element; or -1 if cannot find one.
 */
int llist_index_of(LinkedList *list, void *element)
{
	int index = -1;

	if (list != NULL) {
		LinkedNode *node = list->first;

		while (node != NULL) {
			index++;

			if (element == node->element) {
				break;
			}

			node = node->next;
		}
	}

	return index;
}
/**
 * Destroys the linked list.
 *
 * @param list the list
 * @param destroy_function Function to destroy list element, it can be NULL.
 *
 * The return value of destroy_function is ignored
 */
void llist_destroy(LinkedList *list, llist_handle_element destroy_function)
{
	if (list != NULL) {
		// Iterative search in linked list
		LinkedNode *node = list->first;

		while (node != NULL) {
			if (destroy_function != NULL) {
				(destroy_function)(node->element);
			}

			LinkedNode *next = node->next;
			destroy_node(node);
			node = next;
		}

		list->first = NULL;
		list->last = NULL;
		free(list);
	}
}

/**
 * Iterates over the list and calls handle_iteration function for each element
 *
 * @param list the list
 * @param handle_iteration Function to handle current element in iteration. If function return 0, the iteration stops,
 * if not it continues to next element.
 *
 * @return 1 if loop reaches the end of list, 0 if the loop stops before the end.
 */
int llist_iterate(LinkedList *list, llist_handle_element handle_iteration)
{
	if (list != NULL) {
		// Iterative search in linked list
		LinkedNode *node = list->first;

		while (node != NULL) {
			LinkedNode *next = node->next;

			if (handle_iteration != NULL) {
				if (!(handle_iteration)(node->element)) {
					return 0;
				}
			}

			node = next;
		}
	}

	return 1;
}

/*! @} */
