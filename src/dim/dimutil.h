/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file dimutil.h
 * \brief DIM utility functions definitions.
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
 * \date Jun 11, 2010
 * \author José Martins
 */

#ifndef DIMUTIL_H_
#define DIMUTIL_H_

#include "epi_cfg_scanner.h"
#include "enumeration.h"
#include "metric.h"
#include "numeric.h"
#include "peri_cfg_scanner.h"
#include "pmstore.h"
#include "rtsa.h"
#include "api/api_definitions.h"
#include "asn1/phd_types.h"
#include "util/bytelib.h"


int dimutil_fill_metric_attr(struct Metric *metric, OID_Type attr_id,
			     ByteStreamReader *stream, DataEntry *data_entry);

int dimutil_fill_numeric_attr(struct Numeric *numeric, OID_Type attr_id,
			      ByteStreamReader *stream, DataEntry *data_entry);

int dimutil_fill_rtsa_attr(struct RTSA *rtsa, OID_Type attr_id,
			   ByteStreamReader *stream, DataEntry *data_entry);

int dimutil_fill_enumeration_attr(struct Enumeration *enumeration,
				  OID_Type attr_id, ByteStreamReader *stream, DataEntry *data_entry);

int dimutil_fill_pmstore_attr(struct PMStore *pmstore, OID_Type attr_id,
			      ByteStreamReader *stream, DataEntry *data_entry);

int dimutil_fill_peri_scanner_attr(struct PeriCfgScanner *peri_scanner,
				   OID_Type attr_id, ByteStreamReader *stream, DataEntry *data_entry);

int dimutil_fill_epi_scanner_attr(struct EpiCfgScanner *epi_scanner,
				  OID_Type attr_id, ByteStreamReader *stream, DataEntry *data_entry);

void dimutil_update_mds_from_obs_scan(struct MDS *mds, ObservationScan *var_obs,
				      DataEntry *data_entry);

void dimutil_update_mds_from_obs_scan_fixed(struct MDS *mds, ObservationScanFixed *fixed_obs,
		DataEntry *data_entry);

void dimutil_update_mds_from_grouped_observations(struct MDS *mds, ByteStreamReader *stream,
		HandleAttrValMapEntry *val_map_entry,
		DataEntry *measurement_entry);

#endif /* DIMUTIL_H_ */
