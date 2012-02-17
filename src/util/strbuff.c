/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file strbuff.c
 * \brief String Buffer implementation.
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
 * \date Aug 3, 2010
 * \author Fabricio Silva
 */

#include "strbuff.h"
#include <string.h>
#include <stdlib.h>
#include "src/util/log.h"


/**
 * \addtogroup Utility
 *
 *  String buffer utilities aims to help string concatenation and memory allocation
 *  in a easy way to developer
 *
 * @{
 */


/**
 * This value means additional space when reallocating buffer, so no
 * reallocation will be needed if small strings be appended.
 *
 */
static int ADDITIONAL_BUFF_SIZE = 100;

/**
 * Allocates if needed more memory to buffer to concatenate
 * a string with specified length.
 *
 * @param sb string buffer
 * @param len length of string to concatenate
 *
 * @return 1 if succeeds, 0 if not
 */
static int strbuff_alloc(StringBuffer *sb, int len)
{
	int cur_len = strlen(sb->str);
	int new_len = cur_len + len;

	if (new_len <= sb->size) {
		// Buffer supports to write length
		return 1;
	}

	// Should allocate more memory
	int new_size = new_len + ADDITIONAL_BUFF_SIZE;

	if (sb->str == NULL) {
		sb->str = calloc(new_size, sizeof(char));
	} else {
		sb->str = realloc(sb->str, sizeof(char) * new_size);
	}

	// error if cannot allocate
	if (sb->str == NULL) {
		return 0;
	}

	sb->size = new_size;

	return 1;
}

/**
 * Create a string buffer
 *
 * @param initial_size size of buffer
 *
 * @return sb string buffer, NULL if cannot create one
 */
StringBuffer *strbuff_new(int initial_size)
{
	StringBuffer *sb = calloc(1, sizeof(StringBuffer));
	sb->size = initial_size;

	sb->str = calloc(initial_size, sizeof(char));

	if (sb->str == NULL) {
		return NULL;
	}

	return sb;
}

/**
 * Concatenates the string with buffer
 * @param sb string buffer
 * @param str string to append
 * @param len number of chars of string to append
 *
 * @return 1 if succeeds, 0 if not
 */
static int strbuff_ncat(StringBuffer *sb, char *str, int len)
{
	if (sb == NULL || str == NULL || !strbuff_alloc(sb, len)) {
		return 0;
	}

	strncat(sb->str, str, len);
	return 1;
}

/**
 * Concatenates the string with buffer
 *
 * @param sb string buffer
 * @param str string to append
 * @return 1 if succeeds, 0 if not
 */
int strbuff_cat(StringBuffer *sb, char *str)
{
	// including null character
	if (sb == NULL || str == NULL) {
		return 0;
	}

	return strbuff_ncat(sb, str, strlen(str)+1);
}


/**
 * Destroy struct
 *
 * @param sb string buffer
 */
void strbuff_del(StringBuffer *sb)
{
	if (sb != NULL) {
		free(sb->str);
		sb->str = NULL;
		free(sb);
		sb = NULL;
	}
}


static inline char *xmlescapec(char *orig, char *s, char c, char *repl, int repl_len)
{
	unsigned int i = 0;
	unsigned int len = 0;
	char *cpos;
	
	while ((cpos = strchr(s + i, c))) {
		if (!len)
			len = strlen(s);
		unsigned int pos = cpos - s;

		if (s == orig) {
			// untouched
			s = malloc(len + repl_len - 1 + 1);
			memcpy(s, orig, len + 1);
		} else {
			// already malloc'ed
			s = realloc(s, len + repl_len - 1 + 1);
		}

		memmove(s + pos + repl_len, s + pos + 1, len - pos);
		memcpy(s + pos, repl, repl_len);

		i = pos + repl_len;
		len += repl_len - 1;
	}

	return s;
}

static inline char *xmlescape(char *s)
{
	char *se = xmlescapec(s, s, '&', "&amp;", 5);
	se = xmlescapec(s, se, '<', "&lt;", 4);
	se = xmlescapec(s, se, '>', "&gt;", 4);
	se = xmlescapec(s, se, '"', "&quot;", 6);
	se = xmlescapec(s, se, '\'', "&apos;", 6);
	return se;
}

/**
 * Concatenates the string with buffer, escaping XML 'forbidden' characters
 *
 * @param sb string buffer
 * @param s string to append
 * @return 1 if succeeds, 0 if not
 */
int strbuff_xcat(StringBuffer *sb, char *s)
{
	char *se = xmlescape(s);
	int ret = strbuff_cat(sb, se);
	if (s != se) {
		free(se);
	}
	return ret;
}

/*! @} */
