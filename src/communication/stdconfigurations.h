/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file stdconfigurations.h
 * \brief Standard Configurations header.
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

#ifndef STDCONFIGURATION_H_
#define STDCONFIGURATION_H_

#include <stdlib.h>
#include <asn1/phd_types.h>
#include <dim/mds.h>

/**
 * This Function Pointer return the Extended Configuration described
 * in the specialization document
 */
typedef ConfigObjectList *(*configure_action)();

/**
 * Returns a string representation of MDS values
 */
typedef char *(*mds_to_string)(MDS *mds);

/**
 * Populates an event report (agent)
 */
typedef DATA_apdu *(*agent_event_report)(void *data);

/**
 * Represent the standard configuration described in the
 * specialization document (IEEE-11073-10xxx)
 */
struct StdConfiguration {
	/**
	 * Configuration ID describe by specialization
	 */
	ConfigId dev_config_id;

	/**
	 * This function pointer return the Extended Configuration
	 */
	configure_action configure_action;

	/**
	 * This function pointer fills a DATA_apdu with data event report
	 */
	agent_event_report event_report;
};

void std_configurations_register_conf(struct StdConfiguration *config);

int std_configurations_is_supported_standard(ConfigId config_id);

ConfigObjectList *std_configurations_get_configuration_attributes(ConfigId config_id);

void std_configurations_destroy();

int std_configurations_is_system_id_supported(octet_string system_id);

struct StdConfiguration *std_configurations_get_supported_standard(ConfigId config_id);

/** @} */

#endif /* STDCONFIGURATION_H_ */
