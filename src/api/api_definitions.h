/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file api_definitions.h
 * \brief Contains simplified IEEE type defitions.
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

/**
 * \defgroup API API
 * \brief Simplified Type Definitions of IEEE layer for high level application.
 * Most of functions/types defined in this group are used to convert IEEE
 *  types/operations into useful data formats, such as XML and text.
 */

/**
 * \ingroup API
 * @{
 */

#ifndef API_DEFINITIONS_H_
#define API_DEFINITIONS_H_



/**
 * The API types in api-def-types.xsd
 */
typedef char *APIDEF_type;
#define APIDEF_TYPE_STRING "string"
#define APIDEF_TYPE_INT32 "int32"
#define APIDEF_TYPE_INTU32 "intu32"
#define APIDEF_TYPE_INT16 "int16"
#define APIDEF_TYPE_INTU16 "intu16"
#define APIDEF_TYPE_INT8 "int8"
#define APIDEF_TYPE_INTU8 "intu8"
#define APIDEF_TYPE_FLOAT "float"
#define APIDEF_TYPE_HEX "hex"

/**
 * Represents a simple text data entry
 */
typedef struct SimpleDataEntry {
	char *name;
	APIDEF_type type;
	char *value;
} SimpleDataEntry;

/**
 * Represents a compound text data entry
 */
typedef struct CompoundDataEntry {
	/**
	 * Entry Name
	 */
	char *name;
	/**
	 * Number of child entries
	 */
	int entries_count;
	struct DataEntry *entries;
} CompoundDataEntry;


/**
 * Represents a meta attribute of entry
 */
typedef struct MetaAtt {
	char *name;
	char *value;
} MetaAtt;

/**
 * Abstracts an array of meta-data
 */
typedef struct MetaData {
	int size;
	MetaAtt *values;
} MetaData;



typedef enum {
	SIMPLE_DATA_ENTRY = 0, // !<  If entry is simple
	COMPOUND_DATA_ENTRY    // !<  If entry is composite by other entries
} DataEntry_choice;


/**
 * Abstracts all text data entry formats
 */
typedef struct DataEntry {
	MetaData meta_data;
	DataEntry_choice choice;
	union {
		SimpleDataEntry simple;
		CompoundDataEntry compound;
	} u;
} DataEntry;

/**
 * Represents an array of Data Elements
 */
typedef struct DataList {
	int size;
	DataEntry *values;
} DataList;

/** @} */

#endif /* API_DEFINITIONS_H_ */
