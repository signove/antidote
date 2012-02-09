/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file xml_encoder.c
 * \brief Implementation of xml_encode.h header.
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
 * \date Jul 5, 2010
 */

#include "xml_encoder.h"
#include "api_definitions.h"
#include "src/util/strbuff.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

/**
 * \addtogroup XMLEncoder XML Encoder
 * \ingroup API
 * \brief Encodes types of IEEE layer into XML representation.
 *
 * @{
 */

static void read_entries(DataEntry *values, int size, StringBuffer *sb);


/**
 * Converts the simple data entry to XML format and saves the value into a string buffer.
 *
 * @param simple the data to be converted into XML format.
 * @param sb the string buffer to save the data.
 */
static void describe_simple_entry(SimpleDataEntry *simple, StringBuffer *sb)
{
	if (!simple->name || !simple->type || !simple->value) {
		// A malformed message might generate empty Data Entries
		return;
	}
	strbuff_cat(sb, "<simple>");
	strbuff_cat(sb, "<name>");
	strbuff_xcat(sb, simple->name);
	strbuff_cat(sb, "</name>");
	strbuff_cat(sb, "<type>");
	strbuff_xcat(sb, simple->type);
	strbuff_cat(sb, "</type>");
	strbuff_cat(sb, "<value>");
	strbuff_xcat(sb, simple->value);
	strbuff_cat(sb, "</value>");
	strbuff_cat(sb, "</simple>");
}

/**
 * Converts the compound data entry to XML format and saves the value into a string buffer.
 *
 * @param cmp the data to be converted into XML format.
 * @param sb the string buffer to save the data.
 */
static void describe_cmp_entry(CompoundDataEntry *cmp, StringBuffer *sb)
{
	if (!cmp->name || !cmp->entries) {
		// A malformed message might generate empty Data Entries
		return;
	}
	strbuff_cat(sb, "<compound>");
	strbuff_cat(sb, "<name>");
	strbuff_xcat(sb, cmp->name);
	strbuff_cat(sb, "</name>");
	strbuff_cat(sb, "<entries>");

	read_entries(cmp->entries, cmp->entries_count, sb);

	strbuff_cat(sb, "</entries>");
	strbuff_cat(sb, "</compound>");
}

/**
 * Converts the data entry to XML format and saves the value into a string buffer.
 *
 * @param data the data to be converted into XML format.
 * @param sb the string buffer to save the data.
 */
static void describe_meta_data(DataEntry *data, StringBuffer *sb)
{

	if (data != NULL && data->meta_data.size > 0 && data->meta_data.values
	    != NULL) {

		strbuff_cat(sb, "<meta-data>");
		int i = 0;

		for (i = 0; i < data->meta_data.size; i++) {
			MetaAtt *meta = &data->meta_data.values[i];

			if (meta != NULL && meta->name != NULL) {
				strbuff_cat(sb, "<meta name=\"");
				strbuff_xcat(sb, meta->name);
				strbuff_cat(sb, "\">");
				strbuff_xcat(sb, meta->value);
				strbuff_cat(sb, "</meta>");
			}
		}

		strbuff_cat(sb, "</meta-data>");
	}
}

/**
 * Converts the data entry to XML format and saves the value into a string buffer.
 *
 * @param data the data to be converted into XML format.
 * @param sb the string buffer to save the data.
 */
static void describe_data_entry(DataEntry *data, StringBuffer *sb)
{
	if (data != NULL) {
		strbuff_cat(sb, "<entry>");
		describe_meta_data(data, sb);

		if (data->choice == SIMPLE_DATA_ENTRY) {
			describe_simple_entry(&data->u.simple, sb);
		} else if (data->choice == COMPOUND_DATA_ENTRY) {
			describe_cmp_entry(&data->u.compound, sb);
		}

		strbuff_cat(sb, "</entry>");
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
	int i = 0;

	for (i = 0; i < size; i++) {
		describe_data_entry(&values[i], sb);
	}
}

/**
 * Converts data list elements into XML notation.
 *
 * @param list of text data.
 * @return an string containing data list elements as XML notation.
 */
char *xml_encode_data_list(DataList *list)
{
	StringBuffer *sb = strbuff_new(100);
	strbuff_cat(sb, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	strbuff_cat(sb, "<data-list>");

	if (list != NULL && list->values != NULL) {
		read_entries(list->values, list->size, sb);
	}

	strbuff_cat(sb, "</data-list>");

	char *xml = sb->str;

	free(sb);
	sb = NULL;

	return xml;
}

/** @} */

