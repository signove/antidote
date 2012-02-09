/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file json_encoder.c
 * \brief Implementation of json_encode.h header.
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
 * \date Jul 2, 2010
 */

#include "json_encoder.h"
#include "api_definitions.h"
#include "src/util/strbuff.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

/**
 * \addtogroup JsonEncoder JSON Encoder
 * \brief Json encoder parses types of IEEE layer into data entries for high
 * level application usage.
 * \ingroup API
 *
 * @{
 */

static void read_entries(DataEntry *values, int size, StringBuffer *sb);

/**
 * Converts the simple data entry to JSON format and saves the value into a string buffer.
 *
 * @param simple the data to be converted into JSON format.
 * @param sb the string buffer to save the data.
 */
static void describe_simple_entry(SimpleDataEntry *simple, StringBuffer *sb)
{
	if (!simple->name || !simple->type || !simple->value) {
		// A malformed message might generate empty Data Entries
		strbuff_cat(sb, "simple: {}");
		return;
	}

	strbuff_cat(sb, "simple: {");
	strbuff_cat(sb, "name: \"");
	strbuff_cat(sb, simple->name);
	strbuff_cat(sb, "\", ");
	strbuff_cat(sb, "type: \"");
	strbuff_cat(sb, simple->type);
	strbuff_cat(sb, "\", ");
	strbuff_cat(sb, "value: \"");
	strbuff_cat(sb, simple->value);
	strbuff_cat(sb, "\"");
	strbuff_cat(sb, "}");
}

/**
 * Converts the compound data entry to JSON format and saves the value into a string buffer.
 *
 * @param cmp the data to be converted into JSON format.
 * @param sb the string buffer to save the data.
 */
static void describe_cmp_entry(CompoundDataEntry *cmp, StringBuffer *sb)
{
	if (!cmp->name || !cmp->entries) {
		// A malformed message might generate empty Data Entries
		strbuff_cat(sb, "compound: {}");
		return;
	}

	strbuff_cat(sb, "compound: { ");
	strbuff_cat(sb, "name: \"");
	strbuff_cat(sb, cmp->name);
	strbuff_cat(sb, "\", ");
	strbuff_cat(sb, "entries: ");

	read_entries(cmp->entries, cmp->entries_count, sb);

	strbuff_cat(sb, "}");
}

/**
 * Converts the data entry to JSON format and saves the value into a string buffer.
 *
 * @param data the data to be converted into JSON format.
 * @param sb the string buffer to save the data.
 */
static void describe_meta_data(DataEntry *data, StringBuffer *sb)
{

	if (data != NULL && data->meta_data.size > 0 && data->meta_data.values
	    != NULL) {

		strbuff_cat(sb, "meta_data: [");
		int i = 0;

		for (i = 0; i < data->meta_data.size; i++) {
			MetaAtt *meta = &data->meta_data.values[i];

			if (meta != NULL && meta->name != NULL) {
				strbuff_cat(sb, "{name: \"");
				strbuff_cat(sb, meta->name);
				strbuff_cat(sb, "\", value: \"");
				strbuff_cat(sb, meta->value);
				strbuff_cat(sb, "\"}");

				if (i < data->meta_data.size - 1) {
					strbuff_cat(sb, ", ");
				}
			}
		}

		strbuff_cat(sb, "], ");
	}
}

/**
 * Converts the data entry to JSON format and saves the value into a string buffer.
 *
 * @param data the data to be converted into JSON format.
 * @param sb the string buffer to save the data.
 */
static void describe_data_entry(DataEntry *data, StringBuffer *sb)
{
	if (data != NULL) {
		strbuff_cat(sb, "{");
		describe_meta_data(data, sb);

		if (data->choice == SIMPLE_DATA_ENTRY) {
			describe_simple_entry(&data->u.simple, sb);
		} else if (data->choice == COMPOUND_DATA_ENTRY) {
			describe_cmp_entry(&data->u.compound, sb);
		}

		strbuff_cat(sb, "}");
	}
}

/**
 * Reads all data entries and describe the result in string buffer
 *
 * @param values data entries
 * @param size number of entries
 * @param sb string buffer
 */
static void read_entries(DataEntry *values, int size, StringBuffer *sb)
{
	int i;
	strbuff_cat(sb, "[");

	for (i = 0; i < size; i++) {
		describe_data_entry(&values[i], sb);

		if (i < size - 1) {
			strbuff_cat(sb, ", ");
		}

	}

	strbuff_cat(sb, "] ");
}


/**
 * Converts data list elements into JSON notation.
 *
 * @param list of text data.
 * @return an string containing data list elements as JSON notation.
 */
char *json_encode_data_list(DataList *list)
{
	StringBuffer *sb = strbuff_new(100);

	if (list != NULL && list->values != NULL) {
		read_entries(list->values, list->size, sb);
	}

	char *json = sb->str;

	free(sb);
	sb = NULL;

	return json;
}

/** @} */
