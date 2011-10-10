/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file extconfigurations.c
 * \brief Device extended configuration implementation.
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
 * \author Jose Martins
 * \date Jul 15, 2010
 */

/**
 * @addtogroup Communication
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "src/util/bytelib.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/util/bytelib.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/util/ioutil.h"
#include "src/util/log.h"

#define EXT_CONFIG_FILE "config_list.bin"

struct ExtConfig {
	octet_string system_id;
	ConfigId config_id;
	intu16 obj_size;
};

static struct ExtConfig *ext_configuration_list = NULL;

static int ext_configuration_size = 0;

static char *ext_configurations_get_file_name(octet_string *system_id,
		ConfigId config_id);

static void ext_configurations_write_file(octet_string *system_id,
		ConfigId config_id, ByteStreamWriter *stream);

static struct ExtConfig *ext_configurations_get_config(
	octet_string *system_id, ConfigId config_id);

static void ext_configurations_create_environment();

static char *ext_concat_path_file()
{
	char *tmp = ioutil_get_tmp();
	char *path = calloc(strlen(tmp) + strlen(EXT_CONFIG_FILE) + 1,
			    sizeof(char));
	sprintf(path, "%s"EXT_CONFIG_FILE, tmp);
	free(tmp);
	return path;
}

static void ext_configurations_create_environment()
{
	struct stat st;
	char *config_path = ioutil_get_tmp();

	if (stat(config_path, &st) != 0) {
		int status;

		status = mkdirp(config_path, S_IRWXU | S_IRWXG | S_IROTH);
		DEBUG("mkdirp status %d", status);

		if (status != 0) {
			// TODO Check Error condition
			ERROR("Unable to create configuration directory: %d", \
			      errno);
			return;
		} else {
			DEBUG("Configuration directory created");
		}
	}

	free(config_path);

	char *concat = ext_concat_path_file();

	if (stat(concat, &st) != 0) {
		FILE *fd = fopen(concat, "a+");
		if (fd) {
			fclose(fd);
			DEBUG("Configuration file created");
		} else {
			DEBUG("Unable to create configuration file: %d", errno);
		}
	}

	free(concat);
}

static char *ext_configurations_get_file_name(octet_string *system_id,
		ConfigId config_id)
{
	int length = 0;
	char *config_path = ioutil_get_tmp();
	length = length + strlen(config_path);
	length = length + 2 * system_id->length; // each system_id element is "??"
	length = length + 9; // "-????.bin"
	length = length + 1; // "\0"

	char *file_path = calloc(length, sizeof(char));
	sprintf(file_path, "%s", config_path);
	int i;

	for (i = 0; i < system_id->length; i++) {
		sprintf(file_path, "%s%.2x", file_path, system_id->value[i]);
	}

	sprintf(file_path, "%s-%.4x.bin", file_path, config_id);
	free(config_path);
	return file_path;
}

/**
 * This method destroys the list of available settings but maintains
 * persistent data for later use.
 */
void ext_configurations_destroy()
{
	if (ext_configuration_list != NULL) {
		int index;

		for (index = 0; index < ext_configuration_size; index++) {
			del_octet_string(&ext_configuration_list[index].system_id);
		}

		free(ext_configuration_list);
		ext_configuration_list = NULL;

		ext_configuration_size = 0;
	}
}

void ext_configurations_remove_all_configs()
{
	int index;

	for (index = 0; index < ext_configuration_size; index++) {
		octet_string system_id =
			ext_configuration_list[index].system_id;
		ConfigId config_id = ext_configuration_list[index].config_id;
		char *file_path = ext_configurations_get_file_name(&system_id,
				  config_id);

		if (remove(file_path) != 0) {
			ERROR("\n[Error] Unable to remove file %s", file_path);
		}

		free(file_path);
		file_path = NULL;
	}

	char *concat = ext_concat_path_file();

	if (remove(concat) != 0) {
		ERROR("\n[Error] Unable to remove file %s", concat);
	}

	free(concat);

	ext_configurations_destroy();
}

/**
 * This method loads the list of available configurations.
 */
void ext_configurations_load_configurations()
{
	ext_configurations_create_environment();

	unsigned long buffer_size = 0;
	char *concat = ext_concat_path_file();
	unsigned char *buffer = ioutil_buffer_from_file(concat, &buffer_size);
	free(concat);

	if (buffer != NULL) {
		ByteStreamReader *stream = byte_stream_reader_instance(buffer, buffer_size);

		if (ext_configuration_list != NULL) {
			free(ext_configuration_list);
			ext_configuration_list = NULL;
			ext_configuration_size = 0;
		}

		intu16 size = buffer_size / (sizeof(ConfigId) + 6
					     * sizeof(intu16));
		ext_configuration_list = calloc(size, sizeof(struct ExtConfig));
		ext_configuration_size = size;

		int index;

		for (index = 0; index < ext_configuration_size; index++) {
			ext_configuration_list[index].config_id = read_intu16(
						stream, NULL);
			decode_octet_string(
				stream,
				&ext_configuration_list[index].system_id);
			ext_configuration_list[index].obj_size = read_intu16(
						stream, NULL); // APDU size
		}

		free(buffer);
		free(stream);
	}
}

static void ext_configurations_write_file(octet_string *system_id,
		ConfigId config_id, ByteStreamWriter *stream)
{
	int size = sizeof(ConfigId) + 6 * sizeof(intu16);
	ByteStreamWriter *header_stream = byte_stream_writer_instance(size);
	encode_configid(header_stream, &config_id);
	encode_octet_string(header_stream, system_id);
	write_intu16(header_stream, stream->size);

	int error_result;
	char *concat = ext_concat_path_file();
	error_result = ioutil_buffer_to_file(concat, header_stream->size,
					     header_stream->buffer, 1);
	free(concat);
	del_byte_stream_writer(header_stream, 1);

	if (error_result) {
		// TODO Check Error condition
		ERROR(" ");
		return;
	}

	char *file_path =
		ext_configurations_get_file_name(system_id, config_id);
	error_result = ioutil_buffer_to_file(file_path, stream->size,
					     stream->buffer, 1);
	free(file_path);
	file_path = NULL;

	if (error_result) {
		// TODO Check Error condition
		ERROR(" ");
	}
}

/**
 * This method adds a new configuration. The handle configuration is
 * composed of system_id and config_id.
 *
 * @param system_id Identify the agent;
 * @param config_id Identify the configuration described in the
 *					specialization document;
 * @param object_list The configuration that should be registered;
 */
void ext_configurations_register_conf(octet_string *system_id,
				      ConfigId config_id, ConfigObjectList *object_list)
{
	if (ext_configuration_list == NULL) {
		ext_configurations_load_configurations();
	}

	int size = object_list->length + 2 * sizeof(object_list->count);
	ByteStreamWriter *stream = byte_stream_writer_instance(size);
	encode_configobjectlist(stream, object_list);

	ext_configurations_write_file(system_id, config_id, stream);
	ext_configuration_size++;

	ext_configuration_list = realloc(ext_configuration_list,
					 ext_configuration_size * sizeof(struct ExtConfig));

	// update list
	ext_configuration_list[ext_configuration_size - 1].config_id =
		config_id;
	ext_configuration_list[ext_configuration_size - 1].obj_size =
		stream->size;

	size = system_id->length * sizeof(intu8) + sizeof(intu16);
	ByteStreamWriter *w_stream = byte_stream_writer_instance(size);
	encode_octet_string(w_stream, system_id);

	ByteStreamReader *r_stream = byte_stream_reader_instance(w_stream->buffer, size);
	decode_octet_string(
		r_stream,
		&ext_configuration_list[ext_configuration_size - 1].system_id);

	del_byte_stream_writer(w_stream, 1);
	free(r_stream);

	if (ext_configuration_list == NULL) {
		// TODO Check Error condition
		ERROR(" ");
	}

	del_byte_stream_writer(stream, 1);
}

static struct ExtConfig *ext_configurations_get_config(octet_string *system_id,
		ConfigId config_id) {
	int index;

	for (index = 0; index < ext_configuration_size; index++) {
		ConfigId selected_conf_id =
			ext_configuration_list[index].config_id;
		octet_string selected_sys_id =
			ext_configuration_list[index].system_id;

		if (selected_conf_id == config_id && selected_sys_id.length
		    == system_id->length) {
			int i = 0;
			int is_different = 0;

			while (i < selected_sys_id.length && is_different == 0) {
				is_different |= selected_sys_id.value[i]
						!= system_id->value[i];
				i++;
			}

			if (is_different == 0) {
				return &ext_configuration_list[index];
			}
		}
	}

	return NULL;
}

/**
 * Return true if the configuration described in the config_report
 * parameter is known.
 *
 * @param system_id Identify the agent;
 * @param config_id Identify the configuration described in the
 *					specialization document;
 *
 * @return true if the configuration is known
 */
int ext_configurations_is_supported_standard(octet_string *system_id,
		ConfigId config_id)
{
	return ext_configurations_get_config(system_id, config_id) != NULL;
}

/**
 * This method return the Extended Configuration that was recorded.
 *
 * @param system_id Identify the agent;
 * @param config_id Identify the configuration described in the
 *					specialization document;
 *
 * @return The Extended Configuration that was recorded
 */
ConfigObjectList *ext_configurations_get_configuration_attributes(
	octet_string *system_id, ConfigId config_id)
{

	struct ExtConfig *config = ext_configurations_get_config(system_id,
				   config_id);

	if (config != NULL) {
		ConfigId selected_cfg_id = config->config_id;
		octet_string selected_sys_id = config->system_id;
		// TODO: Check this type conversion
		unsigned long size = config->obj_size;

		char *file_path = ext_configurations_get_file_name(
					  &selected_sys_id, selected_cfg_id);
		intu8 *buffer = ioutil_buffer_from_file(file_path, &size);

		free(file_path);
		file_path = NULL;

		if (buffer == NULL) {
			// TODO Check Error condition
			ERROR(" ");
			return NULL;
		} else {
			ConfigObjectList *result = malloc(
							   sizeof(ConfigObjectList));
			ByteStreamReader *stream = byte_stream_reader_instance(buffer, size);
			decode_configobjectlist(stream, result);
			free(stream);
			free(buffer);
			return result;
		}

	}

	return NULL;
}

/** @} */
