/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file stdconfigurations.c
 * \brief Standard Configurations source.
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
 * \date Jun 24, 2010
 */

/**
 * \defgroup Standard Configurations
 *
 * @{
 */

#include <stdlib.h>
#include <stdio.h>
#include "src/communication/stdconfigurations.h"
#include "src/util/log.h"

/**
 * Number of the standard configurations that are supported.
 */
static int std_configurations_count = 0;

/**
 * List of the standard configurations that are supported.
 */
static struct StdConfiguration **std_configuration_list = NULL;

/**
 * This method adds a new configuration.
 *
 * @param config Configuration described in the specialization document
 */
void std_configurations_register_conf(struct StdConfiguration *config)
{
	std_configurations_count++;
	int last_index = std_configurations_count-1;

	int new_size = std_configurations_count;
	new_size *= sizeof(struct StdConfiguration *);

	if (std_configuration_list == NULL) {
		std_configuration_list = malloc(new_size);
	} else {
		std_configuration_list =
			realloc(std_configuration_list, new_size);
	}

	std_configuration_list[last_index] = config;
}

/**
 * Return the configuration described in the config_report parameter
 *
 * @param config_id Identify the configuration described in the specialization
 *                      document;
 *
 * @return Configuration described in the config_report parameter
 */
struct StdConfiguration *std_configurations_get_supported_standard(ConfigId config_id)
{
	int i;

	for (i = 0; i < std_configurations_count; i++) {
		if (std_configuration_list[i]->dev_config_id == config_id) {
			return std_configuration_list[i];
		}
	}

	return NULL;
}

/**
 * Check if agent system id is supported
 *
 * @param system_id agent system id
 * @return 0 if not supported, >0 if supported
 */
int std_configurations_is_system_id_supported(octet_string system_id)
{
	// TODO Implement method for checking the system_id
	return 1;
}

/**
 * Return true if the configuration described in the config_report
 * parameter is supported.
 *
 * @param config_id Identify the configuration described in the specialization
 *                      document;
 *
 * @return true if the configuration is supported
 */
int std_configurations_is_supported_standard(ConfigId config_id)
{
	return std_configurations_get_supported_standard(config_id) != NULL;
}

/**
 * This method return the Extended Configuration described in the specialization
 * document and identified by config_report parameter
 *
 * @param config_id Identify the configuration described in the specialization
 *                      document;
 *
 * @return The Extended Configuration described in the specialization document
 */
ConfigObjectList *std_configurations_get_configuration_attributes(
	ConfigId config_id)
{

	struct StdConfiguration *standard =
		std_configurations_get_supported_standard(config_id);

	if (standard != NULL) {
		return standard->configure_action();
	}

	return NULL;
}

/**
 * Deallocate all structures of standard configuration
 */
void std_configurations_destroy()
{
	if (std_configuration_list != NULL) {
		struct StdConfiguration *std_conf = NULL;
		int i = 0;

		for (i = 0; i < std_configurations_count; i++) {
			std_conf = std_configuration_list[i];
			free(std_conf);
			std_conf = NULL;
		}
	}

	free(std_configuration_list);

	std_configuration_list = NULL;
	std_configurations_count = 0;
}

/** @} */


