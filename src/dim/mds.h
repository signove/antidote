/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file mds.h
 * \brief MDS type definition.
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
 * \date Jun 09, 2010
 * \author Fabricio Silva
 */

#ifndef MDS_H_
#define MDS_H_

#include <api/api_definitions.h>
#include <asn1/phd_types.h>
#include <dim/dim.h>
#include <dim/enumeration.h>
#include <dim/numeric.h>
#include <dim/rtsa.h>
#include <dim/pmstore.h>
#include <dim/scanner.h>
#include <dim/peri_cfg_scanner.h>
#include <dim/epi_cfg_scanner.h>

/*
 * Static mds handle value
 */
#define MDS_HANDLE 0

/**
 * Medical Device System structure
 */
typedef struct MDS {
	/**
	 * DIM
	 */
	struct DIM dim;

	/**
	 * The Handle attribute represents a reference ID for this object.
	 * The value of the MDS Handle attribute shall be 0.
	 *
	 * Qualifier: Mandatory
	 */
	HANDLE handle;

	/**
	 * This attribute defines the type of the agent, as defined in
	 * nomenclature (e.g., weighing scale). The values shall come
	 * from ISO/IEEE 11073-10101 [B12], nom-part-object partition,
	 * and subpartition MD-Gen(Medical Device - Generic).
	 * Either this attribute or System-Type-Spec-List shall be present.
	 * This attribute shall remainunchanged after the configuration
	 * is agreed upon.
	 *
	 * Qualifier: Conditional
	 */
	TYPE system_type;

	/**
	 * This attribute defines manufacturer and model number of
	 * the agent device. This attribute shall remain unchanged
	 * after the configuration is agreed upon.
	 *
	 * Qualifier: Mandatory
	 */
	SystemModel system_model;

	/**
	 * This attribute is an IEEE EUI-64, which consists of a
	 * 24-bit unique organizationally unique identifier (OUI)
	 * followed by a 40-bit manufacturer-defined ID. The
	 * OUI shall be a value assigned by the IEEE Registration
	 * Authority (http://standards.ieee.org/regauth/index.html)
	 * and shall be used in accordance with IEEE Std 802-2001.6
	 * This attribute shall remain unchanged after the configuration
	 * is agreed upon.
	 *
	 * Qualifier: Mandatory
	 */
	octet_string system_id;

	/**
	 * This attribute defines the identification of the
	 * agent device configuration. This Dev-Configuration-Id
	 * is static during the lifetime of an association; it is
	 * normally exchanged during the association procedure. The
	 * manager can GET this attribute during operation. If this attribute
	 * is queried prior to when the agent and manager agree upon a
	 * configuration, the agent shall return the configuration ID that is
	 * being offered at that time. For more information on this attribute,
	 *  see 8.7.6. This attribute shall remain unchanged after the
	 *  configuration is agreed upon.
	 *
	 *  Qualifier: Mandatory
	 */
	ConfigId dev_configuration_id;

	/**
	 * This attribute defines the attributes that are reported in
	 * the fixed format data update messages (see 7.4.5 for more
	 * information). Usage of this	attribute is mandatory if the agent
	 * device uses fixed format value messages to report dynamic data
	 * for the object.
	 *
	 * Qualifier: Conditional
	 */
	AttrValMap attribute_value_map;

	/**
	 * ProductionSpec This attribute defines component revisions,
	 * serial numbers, and so on in a manufacturer-specific format.
	 * This attribute shall	remain unchanged after the configuration
	 * is agreed upon.
	 *
	 * Qualifier: Optional
	 */
	ProductionSpec production_specification;

	/**
	 * MdsTimeInfo This attribute defines the time handling
	 * capabilities and the status of the MDS. Usage of this
	 * attribute is required if synchronization or settable time
	 * is supported.
	 *
	 * Qualifier: Conditional
	 */
	MdsTimeInfo mds_time_info;

	/**
	 * AbsoluteTime This attribute defines the date and
	 * time of an agent with resolution of 1/100 of a second,
	 * if available.
	 * For more information on this	attribute, see 8.12.
	 *  If the agent reports AbsoluteTime in any other message,
	 *  it shall report its current value of AbsoluteTime in
	 *  this attribute.
	 *
	 *  Qualifier: Conditional
	 */
	AbsoluteTime date_and_time;

	/**
	 * If the agent reports RelativeTime in any other message,
	 * it shall report its current value of RelativeTime
	 * in this attribute.
	 *
	 * Qualifier: Conditional
	 */
	RelativeTime relative_time;

	/**
	 * HighResRelativeTime If the agent reports HighResRelativeTime
	 * in any other message, it shall report its current value of
	 * HighResRelativeTime in this attribute.
	 *
	 * Qualifier: Conditional
	 */
	HighResRelativeTime hires_relative_time;

	/**
	 * AbsoluteTimeAdjust This attribute reports any date
	 * and time adjustments that occur either due to a person's
	 * changing the clock or events such as	daylight savings time.
	 * This is used in event reports only. If queried with Get MDS Object
	 * command, this value shall be not present or 0. If the agent ever
	 * adjusts the date and time, this attribute is used in an event report
	 * to report such adjustment.
	 *
	 * Qualifier: Conditional
	 */
	AbsoluteTimeAdjust date_and_time_adjustment;

	/**
	 * This attribute reports whether power is being drawn from
	 * battery or main power lines and the status of charging.
	 *
	 * Qualifier: Optional
	 */
	PowerStatus power_status;

	/**
	 * This attribute reports the percentage of battery capacity
	 * remaining, which is undefined if value > 100.
	 *
	 * Qualifier: Optional
	 */
	intu16 battery_level;

	/**
	 * This attribute represents the predicted amount of operational
	 * time left on the batteries. The BatMeasure's unit shall be set to
	 * one of MDC_DIM_MIN, MDC_DIM_HR, or MDC_DIM_DAY for minutes, hours,
	 * or days, respectively.
	 *
	 * Qualifier: Optional
	 */
	BatMeasure remaining_battery_time;

	/**
	 * This attribute lists various	regulatory and/or certification
	 * compliance items to which the agent claims adherence as an
	 * informative statement. The Implementation Conformace	Statements
	 * (see Clause 9) take precedence over this attribute and are the
	 * legally binding claims. This attribute shall remain unchanged
	 * after the configuration is agreed upon.
	 *
	 * Qualifier: Optional
	 */
	RegCertDataList reg_cert_data_list;

	/**
	 * TypeVerList This attribute reports the type(s) of the agent,
	 * as defined in nomenclature (e.g., weighing scale).
	 * The values shall come from ISO/IEEE 11073-10101 [B12],
	 * nom-part-infrastruct	partition, subpartition DEVspec, and reference
	 * ISO/IEEE 11073-104zz specializations. If an agent does not follow
	 * any specialization, the list shall be left blank. This list shall
	 * also	contain the version of the specialization. Either this
	 * attribute or System-Type shall be present. If the agent is
	 * multifunction, this attribute shall be present.
	 * This attribute shall	remain unchanged after the configuration is
	 * agreed upon.
	 *
	 * Qualifier: Conditional
	 */
	TypeVerList system_type_spec_list;

	/**
	 * This informational timeout attribute defines the minimum time
	 * that the agent shall wait for a Response message from the manager
	 * after issuing a Confirmed Event Report invoke message before
	 * timing out and transitioning to the Unassociated state.
	 * This is an informational attribute for the benefit of the manager.
	 * If this attribute is supplied, it shall match the actual timeout
	 * value that the agent uses for the Confirmed Event Report generated
	 * from the MDS object.	This attribute is informational for the manager
	 * in the sense that the manager does not use this attribute in an
	 * actual implementation of the protocol (i.e., the manager does not
	 * time	out on an agent-generated Confirmed Event Report).
	 *
	 * Qualifier: Optional
	 */
	RelativeTime confirm_timeout;

	/**
	 * When an agent supports manager-initiated transmission, it shall
	 * indicate what features it supports using the DataReqModeCapab
	 * structure
	 */
	DataReqModeCapab data_req_mode_capab;

	/**
	 * List of children objects
 	 */
	struct MDS_object *objects_list;

	/**
	 * Count of children objects
 	 */
	int objects_list_count;

	/**
	 * Count of PM-Store objects among children
 	 */
	int pmstore_count;
} MDS;

/**
 * Enumeration of choices inside Metric
 */
typedef enum {
	METRIC_NUMERIC = 0,
	METRIC_ENUM,
	METRIC_RTSA
} Metric_choice;

/**
 * Metric object structure
 */
struct Metric_object {
	Metric_choice choice;
	union {
		struct Numeric numeric;
		struct Enumeration enumeration;
		struct RTSA rtsa;
	} u;
};


/**
 * Enumeration of choices inside Scanner
 */
typedef enum {
	EPI_CFG_SCANNER = 0,
	PERI_CFG_SCANNER
} Scanner_choice;

/**
 * Scanner object structure
 */
struct Scanner_object {
	Scanner_choice choice;
	union {
		struct EpiCfgScanner epi_cfg_scanner;
		struct PeriCfgScanner peri_cfg_scanner;
	} u;
};


/**
 * MDS child object choice types
 */
typedef enum {
	MDS_OBJ_METRIC = 0,
	MDS_OBJ_PMSTORE,
	MDS_OBJ_SCANNER
} MDS_object_choice;

/**
 * MDS child object definition
 */
struct MDS_object {
	HANDLE obj_handle;
	MDS_object_choice choice;
	union {
		struct Scanner_object scanner;
		struct Metric_object metric;
		struct PMStore pmstore;
	} u;
};

/**
 * MDS data offered by agent application
 */
struct mds_system_data {
	intu8 system_id[8];
};

void mds_add_object(MDS *mds, struct MDS_object object);

struct MDS_object *mds_get_object_by_handle(MDS *mds, HANDLE obj_handle);

MDS *mds_create();

void mds_destroy(MDS *mds);

int mds_get_nomenclature_code();

void mds_set_attribute(MDS *mds, AVA_Type *attribute);

AVA_Type* mds_get_attributes(MDS *mds, intu16* count, intu16 *length);

Request *mds_set_operational_state_of_the_scanner(Context *ctx, HANDLE handle, OperationalState state,
		service_request_callback callback);


void mds_service_get_all_segment_info(service_request_callback request_callback);

Request *mds_service_get_segment_info(Context *ctx, int handle,
				      service_request_callback request_callback);

Request *mds_service_get_segment_data(Context *ctx, int handle, int instnumber,
				 service_request_callback request_callback);

Request *mds_service_clear_segment(Context *ctx, int handle, int instnumber,
					service_request_callback request_callback);

int mds_is_supported_data_request(MDS *mds, DataReqMode data_req_mode);

Request *mds_service_action_data_request(Context *ctx, DataReqMode data_req_mode,
		OID_Type *class_id, HANDLEList *handle_list,
		service_request_callback request_callback);

Request *mds_service_action_set_time(Context *ctx, SetTimeInvoke *time,  service_request_callback request_callback);

Request *mds_service_get(Context *ctx, OID_Type *attributeids_list, int attributeids_list_count, service_request_callback request_callback);

Request *mds_get_pmstore(Context *ctx, int handle, service_request_callback request_callback);

void mds_configure_operating(Context *ctx, ConfigObjectList *config_obj_list, int manager);

void mds_populate_attributes(MDS *mds, DataEntry *mds_entry);

DataList *mds_populate_configuration(MDS *mds);

void mds_event_report_dynamic_data_update_var(Context *ctx, ScanReportInfoVar *info_var);

void mds_event_report_dynamic_data_update_fixed(Context *ctx, ScanReportInfoFixed *info_fixed);

void mds_event_report_dynamic_data_update_mp_var(Context *ctx, ScanReportInfoMPVar *info_mp_var);

void mds_event_report_dynamic_data_update_mp_fixed(Context *ctx, ScanReportInfoMPFixed *info_mp_fixed);

#endif /* MDS_H_ */
