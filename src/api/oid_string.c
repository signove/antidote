/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file oid_string.c
 * \brief Implementation of oid_string.h header.
 *
 * Copyright (C) 2011 Signove Tecnologia Corporation.
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
 * \author Elvis Pfutzenreuter
 * \date Oct 10, 2011
 */

#include "oid_string.h"
#include <dim/nomenclature.h>

/**
 * Parses the unit code to a human readable string value.
 *
 * \param unit_code the unit code to be parsed.
 *
 * \return unit code in a human readable string; "" if cannot parse the code.
 */
const char *oid_get_unit_code_string(OID_Type unit_code)
{
	switch (unit_code) {
	case MDC_DIM_PERCENT:
		return "%";
	case MDC_DIM_KILO_G:
		return "kg";
	case MDC_DIM_MIN:
		return "min";
	case MDC_DIM_HR:
		return "h";
	case MDC_DIM_DAY:
		return "d";
	case MDC_DIM_DEGC:
		return "oC";

		/* Blood pressure monitor */
	case MDC_DIM_BEAT_PER_MIN:
		return "bpm";
	case MDC_DIM_KILO_PASCAL:
		return "kPa";
	case MDC_DIM_MMHG:
		return "mmHg";

		/* Body composition monitor*/
	case MDC_DIM_CENTI_M:
		return "cm";
	case MDC_DIM_INCH:
		return "in";
	case MDC_DIM_LB:
		return "lb";
	case MDC_DIM_KG_PER_M_SQ:
		return "kg m-2";

		/* Cardiovascular and activity monitor */
	case MDC_DIM_ANG_DEG:
		return "o";
	case MDC_DIM_X_M:
		return "m";
	case MDC_DIM_X_FOOT:
		return "ft";
	case MDC_DIM_X_G:
		return "g";
	case MDC_DIM_X_M_PER_MIN:
		return "m min-1";
	case MDC_DIM_YR:
		return "y";
	case MDC_DIM_X_JOULES:
		return "J";
	case MDC_DIM_X_WATT:
		return "W";
	case MDC_DIM_RESP_PER_MIN:
		return "resp min-1";
	case MDC_DIM_X_STEP:
		return "step";
	case MDC_DIM_X_FOOT_PER_MIN:
		return "ft per minute";
	case MDC_DIM_X_INCH_PER_MIN:
		return "inch per minute";
	case MDC_DIM_X_STEP_PER_MIN:
		return "step per minute";
	case MDC_DIM_X_CAL:
		return "cal";
	case MDC_DIM_RPM:
		return "rpm";

		/*Glucometer*/
	case MDC_DIM_MILLI_L:
		return "mL";
	case MDC_DIM_MILLI_G:
		return "mg";
	case MDC_DIM_MILLI_G_PER_DL:
		return "mg dL-1";
	case MDC_DIM_MILLI_MOLE_PER_L:
		return "mmol L-1";

		/* Thermometer*/
	case MDC_DIM_FAHR:
		return "oF";

	default:
		return "";
	}

	return "";
}


/**
 * Parses the OID VMO type to human readable string value.
 *
 * \param unit_code OID unit code to be parsed
 *
 * \return OID code in a human readable string; "" if cannot parse the code.
 */
const char *oid_get_moc_vmo_string(OID_Type unit_code)
{
	switch (unit_code) {
	case MDC_MOC_VMO_METRIC:
		return "Metric";
	case MDC_MOC_VMO_METRIC_ENUM:
		return "Enumeration";
	case MDC_MOC_VMO_METRIC_NU:
		return "Numeric";
	case MDC_MOC_VMO_METRIC_SA_RT:
		return "SA-RT";
	case MDC_MOC_VMO_PMSTORE:
		return "PM-Store";
	}

	return "";
}


/** @} */
