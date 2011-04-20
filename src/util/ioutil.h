/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file ioutil.h
 * \brief IO Utilities definitions.
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

#ifndef IOUTIL_H_
#define IOUTIL_H_

#include <sys/types.h>
#include "src/asn1/phd_types.h"

intu8 *ioutil_buffer_from_file(const char *file_path,
			       unsigned long *buffer_size);

int ioutil_buffer_to_file(const char *file_path,
			  unsigned long buffer_size, unsigned char *buffer, int append);

void ioutil_print_buffer(intu8 *buffer, int size);

char *ioutil_get_tmp();

int mkdirp(const char *path, __mode_t mode);

#endif /* IOUTIL_H_ */
