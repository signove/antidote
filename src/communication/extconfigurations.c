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

/**
 * Index file that lists saved configurations.
 * Internal structure is an encoded array of ExtConfig
 */
#define EXT_CONFIG_FILE "config_list.bin"

/**
 * Extended configuration structure.
 * The index file is an encoded array of this struct.
 */
struct ExtConfig {
	/**
	 * System ID of device
	 */
	octet_string system_id;
	/**
	 * Configuration ID (namespace = system id)
	 */
	ConfigId config_id;
	/**
	 * Configuration object size.
	 * Every configuration is written in a separate file.
	 * The index only knows (and checks) the size.
	 */
	intu16 obj_size;
};

/**
 * List of extended configurations in memory
 * Loaded from index file at startup
 */
static struct ExtConfig *ext_configuration_list = NULL;

// TODO multithreaded protection

/**
 * Size of extended configuration list in memory
 */
static int ext_configuration_size = 0;

static char *ext_configurations_get_file_name(octet_string *system_id,
		ConfigId config_id);

static void ext_configurations_write_file(octet_string *system_id,
		ConfigId config_id, ByteStreamWriter *stream, int new);

static struct ExtConfig *ext_configurations_get_config(
	octet_string *system_id, ConfigId config_id);

static void ext_configurations_create_environment();

/**
 * Returns fully qualified index file name
 * @return Heap-allocated of file name string
 */
static char *ext_concat_path_file()
{
	char *tmp = ioutil_get_tmp();
	char *path = calloc(strlen(tmp) + strlen(EXT_CONFIG_FILE) + 1,
			    sizeof(char));
	sprintf(path, "%s"EXT_CONFIG_FILE, tmp);
	free(tmp);
	return path;
}

/**
 * Creates extended configuration environment
 */
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

/**
 * Get the file name related to a system id/config id tuple
 * 
 * @param system_id system id of device
 * @param config_id id of extented configuration
 */
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

/**
 * Removes all saved configurations from disk
 */
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
 * This method wipes the ext config index file
 */
static void ext_configurations_wipe()
{
	ext_configurations_create_environment();
	ByteStreamWriter *stream = byte_stream_writer_instance(0);
	char *concat = ext_concat_path_file();
	ioutil_buffer_to_file(concat, stream->size, stream->buffer, 0);
	free(concat);
	del_byte_stream_writer(stream, 1);
	
	DEBUG("wiped ext config file");
}


/**
 * This method loads the list of available configurations.
 */
void ext_configurations_load_configurations()
{
	ext_configurations_create_environment();

	unsigned long buffer_size = 0;
	ByteStreamReader *stream = 0;
	char *concat = ext_concat_path_file();
	unsigned char *buffer = ioutil_buffer_from_file(concat, &buffer_size);
	free(concat);

	if (!buffer) {
		DEBUG("No ext config buffer to read");
		goto exit;
	}

	stream = byte_stream_reader_instance(buffer, buffer_size);
	if (!stream) {
		DEBUG("Zero-sized ext config buffer");
		goto exit;
	}

	if (ext_configuration_list != NULL) {
		free(ext_configuration_list);
		ext_configuration_list = NULL;
		ext_configuration_size = 0;
	}

	ext_configuration_list = calloc(0, sizeof(struct ExtConfig));
	ext_configuration_size = 0;

	while (stream->unread_bytes > 0) {
		int i = ext_configuration_size;
		int error = 0;

		int config_id = read_intu16(stream, &error);
		if (error) {
			DEBUG("ext config: err reading config_id %d", i);
			ext_configurations_wipe();
			break;
		}

		DEBUG("Decoding ext config id %x", config_id);

		octet_string system_id = {0, 0};
		decode_octet_string(stream, &system_id, &error);
		if (error) {
			DEBUG("ext config: err reading system_id %d %d", i, error);
				ext_configurations_wipe();
				break;
			}

		int obj_size = read_intu16(stream, &error);
		if (error) {
			DEBUG("ext config: err reading obj_size %d", i);
			del_octet_string(&system_id);
			ext_configurations_wipe();
			break;
		}

		ext_configuration_list = realloc(ext_configuration_list,
			++ext_configuration_size * sizeof(struct ExtConfig));

		ext_configuration_list[i].config_id = config_id;
		ext_configuration_list[i].system_id = system_id;
		ext_configuration_list[i].obj_size = obj_size;
	}

exit:
	free(buffer);
	free(stream);
}

/**
 * Write extended configuration to disk
 *
 * @param system_id System ID (device identification)
 * @param config_id Extended configuration ID
 * @param stream Stream of bytes with encoded configuration
 * @param new If not 0, indicates that config id was unknown
 *        Otherwise, overwrites old saved config with same ID
 */
static void ext_configurations_write_file(octet_string *system_id,
		ConfigId config_id, ByteStreamWriter *stream,
		int new)
{
	if (new) {
		// appends data to index file
		int size = 2 + sizeof(ConfigId) +
			sizeof(system_id->length) + system_id->length +
			2;

		ByteStreamWriter *header_stream = byte_stream_writer_instance(size);

		encode_configid(header_stream, &config_id); // 2
		encode_octet_string(header_stream, system_id); // 2 + len
		write_intu16(header_stream, stream->size); // 2 (size of config)
	
		char *concat = ext_concat_path_file();
		int err = ioutil_buffer_to_file(concat, header_stream->size,
					     header_stream->buffer, 1);
		free(concat);
		del_byte_stream_writer(header_stream, 1);

		if (err) {
			// TODO Check Error condition
			ERROR("error writing ext config index");
			return;
		}
	}

	// writes particular config+device file
	char *file_path =
		ext_configurations_get_file_name(system_id, config_id);
	int err = ioutil_buffer_to_file(file_path, stream->size,
					     stream->buffer, 0);
	free(file_path);
	file_path = NULL;

	if (err) {
		// TODO Check Error condition
		ERROR("error writing ext config file");
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
	int new;

	if (ext_configuration_list == NULL) {
		ext_configurations_load_configurations();
	}

	int size = object_list->length + 2 * sizeof(object_list->count);
	ByteStreamWriter *stream = byte_stream_writer_instance(size);
	encode_configobjectlist(stream, object_list);

	struct ExtConfig *cfg = ext_configurations_get_config(system_id, config_id);
	if (!cfg) {
		int error = 0;
		DEBUG("Adding new ext config to index");
		new = 1;
		ext_configuration_size++;
		ext_configuration_list = realloc(ext_configuration_list,
					 ext_configuration_size * sizeof(struct ExtConfig));
		cfg = &(ext_configuration_list[ext_configuration_size - 1]);

		cfg->config_id = config_id;

		size = system_id->length * sizeof(intu8) + sizeof(intu16);
		ByteStreamWriter *w_stream = byte_stream_writer_instance(size);
		encode_octet_string(w_stream, system_id);

		ByteStreamReader *r_stream = byte_stream_reader_instance(w_stream->buffer, size);
		// used just to alloc system_id into cfg; won't fail.
		decode_octet_string(r_stream, &cfg->system_id, &error);

		del_byte_stream_writer(w_stream, 1);
		free(r_stream);
	} else {
		DEBUG("Updating ext config");
		new = 0;
	}

	ext_configurations_write_file(system_id, config_id, stream, new);

	cfg->obj_size = stream->size;

	if (ext_configuration_list == NULL) {
		// TODO Check Error condition
		ERROR("ext configuration list is null");
	}

	del_byte_stream_writer(stream, 1);
}

/**
 * Get extended configuration for a given system and config id
 *
 * @param system_id System ID (device identification)
 * @param config_id Extended configuration ID
 * @return Extended configuration structure or NULL if not found
 */
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
			ERROR("ext_config_get could not read from file");
			return NULL;
		} else if (size != config->obj_size) {
			ERROR("ext_config_get: bad obj size");
			free(buffer);
			return NULL;
		} else {
			ConfigObjectList *result = malloc(sizeof(ConfigObjectList));
			ByteStreamReader *stream = byte_stream_reader_instance(buffer, size);
			int error = 0;
			decode_configobjectlist(stream, result, &error);
			if (error) {
				ERROR("ext_config_get: bad configuration data");
				free(result);
				result = NULL;
			}
			free(stream);
			free(buffer);
			return result;
		}
	}

	return NULL;
}

/** @} */
