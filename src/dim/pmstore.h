/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * \file pmstore.h
 * \brief PMStore definition
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
 * \author Walter Guerra
 */

#ifndef PMSTORE_H_
#define PMSTORE_H_

#include "nomenclature.h"
#include "dim.h"
#include "pmsegment.h"
#include "util/bytelib.h"
#include "communication/service.h"
#include "asn1/phd_types.h"
#include "api/api_definitions.h"


/**
 * \brief The PMStore is an struct defining attributes that are common
 * to compose PMStore object classes.
 *
 */
struct PMStore {

	/**
	 * The DIM structure
	 */
	struct DIM dim;

	/**
	 * The Handle attribute represents a reference ID for this object.
	 * Each object shall have a unique ID assigned by the agent. The
	 * handle identifies the object in event reports sent to the
	 * manager and to address the object instance in messages
	 * invoking object methods.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	HANDLE handle;

	/**
	 * This attribute defines basic capabilities of the PM-store
	 * object instance.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	PMStoreCapab pm_store_capab;

	/**
	 * This attribute describes how the sample values stored in the
	 * PM-segment have been processed.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	StoSampleAlg store_sample_algorithm;

	/**
	 * This attribute is the maximum number of stored PM-segment
	 * entries (entries in all contained PM-segments)
	 *
	 * Qualifier: Optional
	 *
	 */
	intu32 store_capacity_count;

	/**
	 * This attribute is the actual number of currently stored PMsegment
	 * entries (entries in all contained PM-segments).
	 *
	 * Qualifier: Optional
	 *
	 */
	intu32 store_usage_count;

	/**
	 * The attribute indicates if new entries are currently being
	 * inserted in any of the contained PM-segments
	 *
	 * Qualifier: Mandatory
	 *
	 */
	OperationalState operational_state;

	/**
	 * This attribute is an application-dependent label for the PMstore
	 * in printable ASCII to indicate its intended use and may be used
	 * for display purposes.
	 *
	 * Qualifier: Conditional
	 *
	 */
	octet_string pm_store_label;

	/**
	 * This attribute determines the frequency at which entries are
	 * added to the PM-segments.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	RelativeTime sample_period;

	/**
	 * This attribute is the number of currently instantiated PMsegments
	 * contained in the PMstore (as reported by agent)
	 *
	 * Qualifier: Mandatory
	 *
	 */
	intu16 number_of_segments;

	/**
	 * This timeout attribute defines the minimum time that the
	 * manager shall wait for the completion of a PM-store clear
	 * command.
	 *
	 * Qualifier: Mandatory
	 *
	 */
	RelativeTime clear_timeout;

	/**
	 * Count of actual segments in list (segm_list).
	 * Might differ from number_of_segments because
	 * number_of_segments is reported by Agent while
	 * this member is determined locally.
	 */
	intu16 segment_list_count;

	/**
	 * List of PM-Segments belonging to this PM-Store
	 */
	struct PMSegment **segm_list;
};

struct PMStore *pmstore_instance();

void pmstore_service_action_clear_segments(struct PMStore *pm_store, SegmSelection selection);

Request *pmstore_service_action_get_segment_info(Context *ctx, struct PMStore *pm_store,
		SegmSelection *selection, service_request_callback request_callback);

void pmstore_get_segmentinfo_result(struct PMStore *pm_store,
					SegmentInfoList info_list,
					struct RequestRet **ret_data);

void pmstore_get_data_result(struct PMStore *pm_store,
				struct RequestRet **ret_data);

void pmstore_destroy(struct PMStore *pm_store);

struct PMSegment *pmstore_get_segment_by_inst_number(struct PMStore *pm_store,
		InstNumber inst_number);

Request *pmstore_service_action_trig_segment_data_xfer(Context *ctx, struct PMStore *pm_store,
		TrigSegmDataXferReq *trig_req,  service_request_callback request_callback);

void pmstore_trig_segment_data_xfer_response(struct PMStore *pm_store,
						TrigSegmDataXferRsp trig_rsp,
						struct RequestRet **ret_data);

void pmstore_clear_segment_result(struct PMStore *pmstore, struct RequestRet *ret_data);

void pmstore_add_segment(struct PMStore *pm_store, struct PMSegment *segment);

void pmstore_remove_selected_segm(struct PMStore *pm_store, intu16 *selection,
				  intu16 length);

int pmstore_get_nomenclature_code();

int pmstore_segment_data_event(Context *ctx, struct PMStore *pm_store,
				SegmentDataEvent event);

Request  *pmstore_service_action_clear_segments_send_command(Context *ctx, struct PMStore *pm_store,
		SegmSelection *selection, service_request_callback request_callback);

void pmstore_service_set_attribute(struct PMStore *pm_store, AVA_Type *attribute);

int pmstore_set_attribute(struct PMStore *pmstore, OID_Type attr_id, ByteStreamReader *stream);

DataList *pmstore_get_data_as_datalist(Context *ctx, HANDLE handle);

DataList *pmstore_get_segment_info_data_as_datalist(Context *ctx, HANDLE handle);

#endif /* PMSTORE_H_ */
