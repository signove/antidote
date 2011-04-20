/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file extconfigurations.h
 * \brief Device extended configuration definitions.
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

#ifndef EXTCONFIGURATION_H_
#define EXTCONFIGURATION_H_

#include "src/asn1/phd_types.h"

void ext_configurations_load_configurations();

void ext_configurations_register_conf(octet_string *system_id,
				      ConfigId config_id, ConfigObjectList *object_list);

int ext_configurations_is_supported_standard(octet_string *system_id, ConfigId config_id);

void ext_configurations_remove_all_configs();

void ext_configurations_destroy();

ConfigObjectList *ext_configurations_get_configuration_attributes(octet_string *system_id, ConfigId config_id);

/** @} */

#endif /* EXTCONFIGURATION_H_ */
