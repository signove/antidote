/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file ioutil.c
 * \brief IO Utilities implementation.
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
 * \date Jun 22, 2010
 * \author Fabricio Silva
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ioutil.h"
#include "log.h"
#include "src/asn1/phd_types.h"

#define DIR_SEP '/'

/**
 * \addtogroup Utility
 * @{
 */

/**
 *  Returns the file content as a string. If the file does not
 *  exist, this function returns \b NULL. If it is not possible to
 *  allocate enough memory for string, this function returns \b NULL.
 *
 *  \param file_path the file path.
 *  \param buffer_size the length of result string.
 *
 *  \return the file content as a string.
 */
intu8 *ioutil_buffer_from_file(const char *file_path,
			       unsigned long *buffer_size)
{
	FILE *file;
	long int fileLen;
	intu8 *buffer = NULL;

	// Open file
	file = fopen(file_path, "rb");

	if (!file) {
		ERROR("Unable to open file %s", file_path);
		return NULL;
	}

	// Get file length
	fseek(file, 0, SEEK_END);
	fileLen = ftell(file);

	if (fileLen == -1L) {
		ERROR("Unable to detect file length");
		fclose(file);
		return NULL;
	}

	fseek(file, 0, SEEK_SET);

	// Allocate memory
	buffer = malloc(fileLen);

	if (!buffer) {
		ERROR("Memory error!");
		fclose(file);
		return NULL;
	}

	*buffer_size = fileLen;

	// Read file contents into buffer_cur
	if (fread(buffer, fileLen, 1, file) != fileLen) {
		// TODO handle error
	}
	fclose(file);

	// Do what ever with buffer_cur
	return buffer;
}

/**
 *  Saves a string into a file. This function return success code (\b 0) whether
 *  operation is properly performed. If it is possible to open/create
 *  the specified file, this function return error code (\b 1).
 *
 *  \param file_path the file path.
 *  \param buffer_size the length of input string.
 *  \param buffer the input string.
 *  \param append tells whether a new file should be created (\b 0) or not (\b 1)
 *
 *  \return \b 0, if the operation is properly performed; \b 1 otherwise.
 */
int ioutil_buffer_to_file(const char *file_path,
			  unsigned long buffer_size, unsigned char *buffer, int append)
{
	FILE *file;

	// Open file
	file = fopen(file_path, (append ? "a" : "w"));

	if (!file) {
		ERROR("Unable to open file %s", file_path);
		return 1;
	}

	// Read file contents into buffer_cur
	fwrite(buffer, buffer_size, 1, file);
	fclose(file);
	return 0;
}

/**
 * Print buffer data
 * @param *buffer
 * @param size
 */
void ioutil_print_buffer(intu8 *buffer, int size)
{
	DEBUG("Buffer data:");

	char  *str = calloc(size*4, sizeof(char));

	int i;

	for (i = 0; i < size; i++) {
		sprintf(str, "%s%.2X ", str, buffer[i]);
	}

	DEBUG("%s", str);

	fflush(stdout);
	free(str);
	str = NULL;
}

#ifdef ANDROID

extern char *android_tmp_location;

#else
static const char *tmp_locations[] = { "HEALTHD_TMP", "TMPDIR", "TEMP", "TMP",
				       NULL };
#endif

/**
 * @brief Gets the temporary files directory location.
 *
 * The returned string must be free by the user.
 * The returned directory is defined by:
 * - $TMPDIR/healthd/
 * - $TEMP/healthd/
 * - $TMP/healthd/
 * - /tmp/healthd/
 *
 * @return A string containing the location of the temporary directory.
 */
char *ioutil_get_tmp()
{
	int i = 0;
	char *tmp = NULL;
	char *result = NULL;
	int tmp_len = 0;

#ifdef ANDROID
	tmp = android_tmp_location;
	mkdir(tmp, 0770);
#else
	while (tmp_locations[i]) {
		tmp = getenv(tmp_locations[i]);

		if (tmp && (strcmp(tmp, "") != 0)) {
			break;
		}

		i++;
	}

	if (!tmp || (strcmp(tmp, "") == 0)) {
		tmp = "/tmp/";
	}
#endif

	tmp_len = strlen(tmp);

	if (tmp[tmp_len - 1] == DIR_SEP) {
		result = calloc(tmp_len + strlen("healhtd/") + 1, sizeof(char));
		sprintf(result, "%shealhtd/", tmp);
	} else {
		result = calloc(tmp_len + strlen("healhtd/") + 2, sizeof(char));
		sprintf(result, "%s/healhtd/", tmp);
	}

	return result;
}

/**
 * Creates a directory recursively.
 * @param path Path to create the dir.
 * @param mode Mode of the created dir.
 * @return -1 if error and 0 is success @see mkdir.
 */
int mkdirp(const char *path, __mode_t mode)
{
	int tmp_len = strlen(path);
	char *tmp = calloc(tmp_len + 1, sizeof(char));
	char *p = NULL;
	int result = 0;

	strcpy(tmp, path);

	// remove trailing '/'
	if (tmp[tmp_len - 1] == DIR_SEP) {
		tmp[tmp_len - 1] = '\0';
	}

	// skip if the root '/'
	if (tmp[0] == DIR_SEP)
		p = &tmp[1];

	// make mkdir for each subdir
	for (; *p != '\0'; p++)
		if (*p == DIR_SEP) {
			*p = '\0';
			result = mkdir(tmp, mode);

			if ((result == -1) && (errno != EEXIST)) {
				free(tmp);
				return result;
			}

			*p = DIR_SEP;
		}

	result = mkdir(tmp, mode);
	free(tmp);
	return result;
}

/*! @} */
