/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * \file pmstore.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "pmstore.h"
#include "pmstore_req.h"
#include "src/api/api_definitions.h"
#include "src/api/data_encoder.h"
#include "src/api/data_list.h"
#include "src/api/text_encoder.h"
#include "src/util/dateutil.h"
#include "src/util/bytelib.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/communication/service.h"
#include "src/manager_p.h"
#include "src/util/log.h"
#include "src/dim/mds.h"
#include "src/dim/dimutil.h"

// FIXME EPX2 if number_of_segments comes in configuration step,
// check if number_of_segments == segment_list_count

/**
 * \defgroup PMStore PMStore
 * \ingroup ObjectClasses
 * \brief PMStore Object Class
 *
 * An instance of the PM-store class provides long-term storage
 * capabilities for metric data. Data are stored in a variable number
 * of PM-segment objects.
 *
 * The stored data of the PM-store object are requested from the agent
 * by the manager using object access services.
 *
 * @{
 */

/**
 * Time out (seconds) Confirm Action
 */
static const intu32 PM_STORE_TO_CONFIRM_ACTION = 3;

/**
 * Time out (seconds) Confirm GET
 */
static const intu32 PM_STORE_TO_GET = 3;

/**
 * Time out (seconds) Confirm SET
 */
static const intu32 PM_STORE_TO_CONFIRM_SET = 3;

static int pmstore_fill_segment_attr(struct PMSegment *pm_segment,
					OID_Type attr_id,
					ByteStreamReader *stream);

static void pmstore_populate_all_attributes(MDS *mds, struct PMStore *pmstore,
						struct PMSegment *pmsegment,
						DataEntry *segm_data_entry);

static void decode_fixed_segment_data(Context *ctx, struct PMStore *pmstore,
					struct PMSegment *pmsegment);

/**
 * Returns one instance of the PMStore structure.
 * The attributes must be defined through direct assignments, including
 * conditional and optional attributes.
 *
 * \return a pointer to a PMStore structure.
 */
struct PMStore *pmstore_instance()
{
	struct PMStore *store = calloc(1, sizeof(struct PMStore));
//	if (store == NULL) {
//		exit(1);
//	}
	store->dim.id = pmstore_get_nomenclature_code();
	store->segm_list = NULL;
	store->segment_list_count = 0;
	store->number_of_segments = 0;
	return store;
}

/**
 * Deletes the data currently stored in one or more selected PMsegments.
 * All entries in the selected PM-segments are deleted. If the agent
 * supports a variable number of PM-segments, the agent may delete empty
 * PM-segments.
 *
 * \param pm_store the PMStore struct to have the segments cleared.
 * \param selection the selected segments.
 *
 */
void pmstore_service_action_clear_segments(struct PMStore *pm_store, SegmSelection selection)
{
	// TODO check PM-Store-Capab attribute bits
	int i;
	int j;
	InstNumber *inst_num = NULL;
	intu16 length = 0;

	AbsoluteTime selection_start = selection.u.abs_time_range.from_time;
	AbsoluteTime selection_end = selection.u.abs_time_range.to_time;

	switch (selection.choice) {
	case ALL_SEGMENTS_CHOSEN:

		// Remove all Segments or only entries of all segments
		for (i = 0; i < pm_store->segment_list_count; ++i) {
			if (pm_store->segm_list != NULL) {
				pmsegment_remove_all_entries(
					pm_store->segm_list[i]);
			}
		}

		if (pm_store->pm_store_capab & pmsc_clear_segm_remove) {
			if (pm_store->segm_list != NULL) {
				int i;

				for (i = 0; i < pm_store->segment_list_count; i++) {
					free(pm_store->segm_list[i]);
				}

				free(pm_store->segm_list);
				pm_store->segment_list_count = 0;
				pm_store->segm_list = NULL;
			}
		}

		break;

	case SEGM_ID_LIST_CHOSEN:
		// Remove all segments selected or only entries of all segments selected

		for (i = 0; i < selection.u.segm_id_list.length; ++i) {
			InstNumber id = selection.u.segm_id_list.value[i];

			for (j = 0; j < pm_store->segment_list_count; ++j) {
				if (pm_store->segm_list[j]->instance_number
				    == id) {
					pmsegment_remove_all_entries(
						pm_store->segm_list[i]);
				}
			}
		}

		if (pm_store->pm_store_capab & pmsc_clear_segm_remove) {
			pmstore_remove_selected_segm(pm_store,
						     selection.u.segm_id_list.value,
						     selection.u.segm_id_list.length);
		}

		break;

	case ABS_TIME_RANGE_CHOSEN:
		// Removes all segments that are between two dates or only those entries segment
		inst_num = malloc(sizeof(InstNumber)
				  * pm_store->segment_list_count);

		if (inst_num == NULL) {
			exit(1);
		}

		for (i = 0; i < pm_store->segment_list_count; ++i) {
			AbsoluteTime segm_start =
				pm_store->segm_list[i]->segment_start_abs_time;
			AbsoluteTime segm_end =
				pm_store->segm_list[i]->segment_end_abs_time;
			int start_comp = date_util_compare_absolute_time(
						 segm_start, selection_start);
			int end_comp = date_util_compare_absolute_time(
					       segm_end, selection_end);

			if (start_comp > 0 && end_comp < 0) {
				pmsegment_remove_all_entries(
					pm_store->segm_list[i]);
				inst_num[length]
				= pm_store->segm_list[i]->instance_number;
				length++;
			}
		}

		if ((length > 0) && (pm_store->pm_store_capab
				     & pmsc_clear_segm_remove)) {
			pmstore_remove_selected_segm(pm_store, inst_num, length);
		}

		if (inst_num != NULL) {
			free(inst_num);
			length = 0;
			inst_num = NULL;
		}

		break;

	default:
		break;
	}
}

static void del_PMStoreClearSegmRet(void *p)
{
	PMStoreClearSegmRet *rs = p;
	del_segmselection(&(rs->segm_selection));
}

/**
 * Deletes the data currently stored in one or more selected PMsegments.
 * All entries in the selected PM-segments are deleted. If the agent
 * supports a variable number of PM-segments, the agent may delete empty
 * PM-segments.
 *
 * \param ctx context
 * \param pm_store the PMStore struct to have the segments cleared.
 * \param selection the selected segments.
 * \param request_callback
 *
 */
Request *pmstore_service_action_clear_segments_send_command(Context *ctx, struct PMStore *pm_store,
		SegmSelection *selection, service_request_callback request_callback)
{
	ByteStreamWriter *writer;
	ByteStreamReader *reader;
	writer = byte_stream_writer_instance(selection->length + sizeof(SegmSelection_choice) + sizeof(intu16));
	encode_segmselection(writer, selection);

	APDU *apdu = (APDU *)malloc(sizeof(APDU));

	apdu->choice = PRST_CHOSEN;

	DATA_apdu *data_apdu = (DATA_apdu *) calloc(1, sizeof(DATA_apdu));
	data_apdu->message.choice = ROIV_CMIP_CONFIRMED_ACTION_CHOSEN;

	data_apdu->message.u.roiv_cmipConfirmedAction.obj_handle = pm_store->handle;
	data_apdu->message.u.roiv_cmipConfirmedAction.action_type = MDC_ACT_SEG_CLR;

	data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.value = writer->buffer;
	data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.length = writer->size;

	data_apdu->message.length = data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.length
				    + sizeof(intu16) + sizeof(OID_Type) + sizeof(HANDLE);

	apdu->u.prst.length = data_apdu->message.length + sizeof(intu16) + sizeof(DATA_apdu_choice)
			      + sizeof(InvokeIDType);


	apdu->length = apdu->u.prst.length + sizeof(intu16);

	// Send APDU
	timeout_callback timeout_callback = NO_TIMEOUT;
	encode_set_data_apdu(&apdu->u.prst, data_apdu);
	Request *req = service_send_remote_operation_request(ctx, apdu, timeout_callback,
								request_callback);
	
	PMStoreClearSegmRet *rs = calloc(1, sizeof(PMStoreClearSegmRet));
	req->return_data = (struct RequestRet*) rs;
	rs->handle = pm_store->handle;
	rs->del_function = &del_PMStoreClearSegmRet;
	reader = byte_stream_reader_instance(writer->buffer, writer->size);
	decode_segmselection(reader, &(rs->segm_selection));
	free(reader);

	del_byte_stream_writer(writer, 1);

	return req;
}

/**
 * Removes PMSegments from RAM when clear segm confirmation comes back
 *
 * \param pmstore the PMStore to have PMSegments removed from.
 * \param ret_data the return data associated with Request
 *
 */
void pmstore_clear_segment_result(struct PMStore *pmstore, struct RequestRet *ret_data)
{
	PMStoreClearSegmRet *rs = (PMStoreClearSegmRet*) ret_data;
	pmstore_service_action_clear_segments(pmstore, rs->segm_selection);
}

/**
 * Removes PMSegments from a PMStore.
 *
 * \param pm_store the PMStore to have PMSegments removed from.
 * \param selection the PMSegment instance numbers selection.
 * \param length the number of PMSegments to be removed.
 *
 */
void pmstore_remove_selected_segm(struct PMStore *pm_store, intu16 *selection,
				  intu16 length)
{
	struct PMSegment *temp_segm;
	temp_segm = NULL;
	intu16 temp_size;
	temp_size = pm_store->segment_list_count - length;
	temp_segm = malloc(sizeof(struct PMSegment) * temp_size);

	if (temp_segm == NULL) {
		exit(1);
	}

	int j;
	int k = 0;

	for (j = 0; j < pm_store->segment_list_count; ++j) {
		int found = 0;
		int i = 0;

		while (found == 0 && i < length) {
			if (pm_store->segm_list[j]->instance_number
			    == selection[i]) {
				found = 1;
			}

			i++;
		}

		if (found == 0) {
			temp_segm[k] = *pm_store->segm_list[j];
			k++;
		}

		free(pm_store->segm_list[j]);
		pm_store->segm_list[j] = NULL;

	}

	if (pm_store->segment_list_count > 0) {
		free(pm_store->segm_list);
		pm_store->segm_list = NULL;

		if (temp_size != 0) {
			pm_store->segm_list = malloc(sizeof(struct PMSegment *) * temp_size);
		}

		for (j = 0; j < temp_size; ++j) {
			pm_store->segm_list[j] = malloc(sizeof(struct PMSegment));
			*pm_store->segm_list[j] = temp_segm[j];
		}

		pm_store->segment_list_count = temp_size;

		free(temp_segm);
	} else {
		exit(1);
	}

}

/**
 * Retrieves PM-segment attributes of one or more PM-segments.
 *
 * \param ctx
 * \param pm_store the PMStore.
 * \param selection the selection of PMSegments.
 * \param request_callback the callback function to be invoked when the response
 * of this request arrives.
 *
 * \return request a pointer to a get PMSegmentInfo request.
 *
 */
Request *pmstore_service_action_get_segment_info(Context *ctx, struct PMStore *pm_store,
		SegmSelection *selection,  service_request_callback request_callback)
{
	ByteStreamWriter *writer;
	writer = byte_stream_writer_instance(selection->length + sizeof(SegmSelection_choice) + sizeof(intu16));
	encode_segmselection(writer, selection);

	APDU *apdu = (APDU *)malloc(sizeof(APDU));

	apdu->choice = PRST_CHOSEN;

	DATA_apdu *data_apdu = (DATA_apdu *) malloc(sizeof(DATA_apdu));
	data_apdu->message.choice = ROIV_CMIP_CONFIRMED_ACTION_CHOSEN;

	data_apdu->message.u.roiv_cmipConfirmedAction.obj_handle = pm_store->handle;
	data_apdu->message.u.roiv_cmipConfirmedAction.action_type = MDC_ACT_SEG_GET_INFO;

	data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.value = writer->buffer;
	data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.length = writer->size;

	data_apdu->message.length = data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.length
				    + sizeof(intu16) + sizeof(OID_Type) + sizeof(HANDLE);

	apdu->u.prst.length = data_apdu->message.length + sizeof(intu16) + sizeof(DATA_apdu_choice)
			      + sizeof(InvokeIDType);


	apdu->length = apdu->u.prst.length + sizeof(intu16);

	free(writer);

	// Send APDU
	timeout_callback timeout_callback = NO_TIMEOUT;
	encode_set_data_apdu(&apdu->u.prst, data_apdu);
	Request *req = service_send_remote_operation_request(ctx, apdu, timeout_callback, request_callback);

	return req;
}

/**
 * Function called in response to get segments information action.
 *
 * \param pm_store the PMStore.
 * \param ret_data Response data related to Request
 */
void pmstore_get_data_result(struct PMStore *pm_store,
				struct RequestRet **ret_data)
{
	PMStoreGetRet *rd;

	rd = calloc(1, sizeof(PMStoreGetRet));
	*ret_data = (struct RequestRet *) rd;
	rd->handle = pm_store->handle;
	rd->response = 0;
}

/**
 * Function called in response to get segments information action.
 *
 * \param pm_store the PMStore.
 * \param info_list the list of PMSegment info.
 * \param ret_data Response data related to Request
 */
void pmstore_get_segmentinfo_result(struct PMStore *pm_store,
					SegmentInfoList info_list,
					struct RequestRet **ret_data)
{
	struct PMSegment *pmsegment = NULL;
	PMStoreGetSegmInfoRet *rd;
	int i;
	int j;
	int info_list_size = info_list.count;

	rd = calloc(1, sizeof(PMStoreGetSegmInfoRet));
	*ret_data = (struct RequestRet *) rd;
	rd->handle = pm_store->handle;
	rd->response = 0;

	// FIXME EPX2 where errors come from? roer
	// FIXME EPX2 check what it does under multiple passes

	for (i = 0; i < info_list_size; ++i) {
		InstNumber inst_number = info_list.value[i].seg_inst_no;
		int seg_info_size = info_list.value[i].seg_info.count;
		AttributeList attr_list = info_list.value[i].seg_info;
		OID_Type attr_id;
		octet_string value;

		pmsegment = pmsegment_instance(inst_number);

		for (j = 0; j < seg_info_size; ++j) {
			attr_id = attr_list.value[j].attribute_id;
			value.length = attr_list.value[j].attribute_value.length;
			value.value = attr_list.value[j].attribute_value.value;
			ByteStreamReader *stream = byte_stream_reader_instance(value.value,
						   value.length);
			pmstore_fill_segment_attr(pmsegment, attr_id, stream);
			free(stream);
		}

		pmstore_add_segment(pm_store, pmsegment);
	}
}

/**
 * Starts the transfer of the Fixed-Segment-Data attribute of a specified
 * PM-segment.
 *
 * \param ctx
 * \param pm_store the PMStore.
 * \param trig_req the data transfer request attribute.
 * \param request_callback callback function to be called when the response
 * to this request arrives.
 *
 * \return a pointer to a Request struct to starts the transfer.
 *
 */
Request *pmstore_service_action_trig_segment_data_xfer(Context *ctx, struct PMStore *pm_store,
		TrigSegmDataXferReq *trig_req,  service_request_callback request_callback)
{
	ByteStreamWriter *writer;
	writer = byte_stream_writer_instance(sizeof(TrigSegmDataXferReq));
	write_intu16(writer, trig_req->seg_inst_no);

	APDU *apdu = (APDU *)malloc(sizeof(APDU));

	apdu->choice = PRST_CHOSEN;

	DATA_apdu *data_apdu = (DATA_apdu *) malloc(sizeof(DATA_apdu));
	data_apdu->message.choice = ROIV_CMIP_CONFIRMED_ACTION_CHOSEN;

	data_apdu->message.u.roiv_cmipConfirmedAction.obj_handle = pm_store->handle;
	data_apdu->message.u.roiv_cmipConfirmedAction.action_type = MDC_ACT_SEG_TRIG_XFER;

	data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.value = writer->buffer;
	data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.length = writer->size;

	data_apdu->message.length = data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.length
				    + sizeof(intu16) + sizeof(OID_Type) + sizeof(HANDLE);

	apdu->u.prst.length = data_apdu->message.length + sizeof(intu16) + sizeof(DATA_apdu_choice)
			      + sizeof(InvokeIDType);


	apdu->length = apdu->u.prst.length + sizeof(intu16);

	free(writer);

	// Send APDU
	timeout_callback timeout_callback = NO_TIMEOUT;
	encode_set_data_apdu(&apdu->u.prst, data_apdu);
	Request *req = service_send_remote_operation_request(ctx, apdu, timeout_callback, request_callback);
	return req;
}

/**
 * Responses to request fixed segment data request.
 *
 * \param pm_store the PMStore.
 * \param trig_rsp the data transfer response attribute.
 * \param ret_data the related Request->RequestRet pointer to be filled
 */
void pmstore_trig_segment_data_xfer_response(struct PMStore *pm_store,
						TrigSegmDataXferRsp trig_rsp,
						struct RequestRet **ret_data)
{
	PMStoreGetSegmDataRet *ret = calloc(1, sizeof(PMStoreGetSegmDataRet));
	*ret_data = (struct RequestRet *) ret;

	ret->handle = pm_store->handle;
	ret->inst = trig_rsp.seg_inst_no;
	ret->response = trig_rsp.trig_segm_xfer_rsp;
}

/**
 * Receives data stored in the Fixed-Segment-Data of a PM-segment from the agent and
 * save it into the PMStore as PMSegments.
 *
 * \param ctx
 * \param pm_store the PMStore.
 * \param event the object event which refers to a new data received.
 *
 * \return the data sent by agent as a PM-segment.
 */
void pmstore_segment_data_event(Context *ctx, struct PMStore *pm_store,
				SegmentDataEvent event)
{
	// TODO fixed segment data can comes as NaN. Verify page 37.
	// TODO verify if is needed to use event.segm_data_event_descr.segm_evt_status;
	struct PMSegment *pmsegment = NULL;
	InstNumber inst_number = event.segm_data_event_descr.segm_instance;
	pmsegment = pmstore_get_segment_by_inst_number(pm_store, inst_number);

	if (!pmsegment)
		return;

	// FIXME EPX2 see how it does over multiple passes

	pmsegment->fixed_segment_data.length += event.segm_data_event_entries.length;

	if (pmsegment->segment_usage_count == 0) {
		pmsegment->fixed_segment_data.value = calloc(1, pmsegment->fixed_segment_data.length);
	} else {
		pmsegment->fixed_segment_data.value
		= realloc(pmsegment->fixed_segment_data.value,
			  pmsegment->fixed_segment_data.length);
	}

	pmsegment->segment_usage_count += event.segm_data_event_descr.segm_evt_entry_count;

	memcpy(&(pmsegment->fixed_segment_data.value[event.segm_data_event_descr.segm_evt_entry_index]),
	       	event.segm_data_event_entries.value,
		event.segm_data_event_entries.length);

	decode_fixed_segment_data(ctx, pm_store, pmsegment);
}

/**
 * Initializes a given PMSegment attribute with a stream content.
 *
 * \param pm_segment the PMSegment.
 * \param attr_id the PMSegment's attribute ID.
 * \param stream the value of PMSegment's attribute.
 *
 * \return \b 1, if the PMSegment's attribute is properly modified; \b 0 otherwise.
 *
 */
static int pmstore_fill_segment_attr(struct PMSegment *pm_segment, OID_Type attr_id,
				     ByteStreamReader *stream)
{
	int result = 1;

	switch (attr_id) {
	case MDC_ATTR_PM_SEG_MAP:
		decode_pmsegmententrymap(stream, &pm_segment->pm_segment_entry_map);
		break;
	case MDC_ATTR_PM_SEG_PERSON_ID:
		pm_segment->pm_seg_person_id = read_intu16(stream, NULL);
		break;
	case MDC_ATTR_OP_STAT:
		pm_segment->operational_state = read_intu16(stream, NULL);
		break;
	case MDC_ATTR_TIME_PD_SAMP:
		pm_segment->sample_period = read_intu32(stream, NULL);
		break;
	case MDC_ATTR_PM_SEG_LABEL_STRING:
		decode_octet_string(stream, &(pm_segment->segment_label));
		break;
	case MDC_ATTR_TIME_START_SEG:
		decode_absolutetime(stream, &(pm_segment->segment_start_abs_time));
		break;
	case MDC_ATTR_TIME_END_SEG:
		decode_absolutetime(stream, &(pm_segment->segment_end_abs_time));
		break;
	case MDC_ATTR_TIME_ABS_ADJUST:
		decode_absolutetimeadjust(stream, &(pm_segment->date_and_time_adjustment));
		break;
	case MDC_ATTR_SEG_USAGE_CNT:
		pm_segment->segment_usage_count = read_intu32(stream, NULL);
		break;
	case MDC_ATTR_SEG_STATS:
		decode_segmentstatistics(stream, &(pm_segment->segment_statistics));
		break;
	case MDC_ATTR_CONFIRM_TIMEOUT:
		pm_segment->confirm_timeout = read_intu32(stream, NULL);
		break;
	case MDC_ATTR_TRANSFER_TIMEOUT:
		pm_segment->transfer_timeout = read_intu32(stream, NULL);
		break;
	default:
		result = 0;
		break;
	}

	return result;

}

/**
 * Returns a PMSegment with the given instance number.
 *
 * \param pm_store the PMStore to select a PMSegment.
 * \param inst_number the PMSegment instance number
 *
 * \return a pointer to a PMSegment with the given instance number; /b NULL
 * if there is no PMSegment in the PMStore with the given instance number.
 *
 */
struct PMSegment *pmstore_get_segment_by_inst_number(struct PMStore *pm_store,
		InstNumber inst_number) {
	if (pm_store->segment_list_count == pm_store->number_of_segments) {
		int i;

		for (i = 0; i < pm_store->segment_list_count; ++i) {
			if (pm_store->segm_list[i]->instance_number == inst_number) {
				return pm_store->segm_list[i];
			}
		}
	}

	return NULL;
}

/**
 * Adds a new segment into PMStore.
 *
 * \param pm_store the PMStore
 * \param segment the segment to be inserted into the PMStore.
 *
 */
void pmstore_add_segment(struct PMStore *pm_store, struct PMSegment *segment)
{
	// test if there is not elements in the list
	if (pm_store->segment_list_count == 0) {
		pm_store->segm_list = calloc(1, sizeof(struct PMSegment *));

	} else { // change the list size
		pm_store->segm_list = realloc(pm_store->segm_list,
					      sizeof(struct PMSegment *)
					      * (pm_store->segment_list_count
						 + 1));

		if (pm_store->segm_list == NULL) {
			ERROR("ERROR");
			exit(1);
		}
	}

	// add element to list
	pm_store->segm_list[pm_store->segment_list_count] = segment;
	pm_store->segment_list_count += 1;
}

/**
 * Returns the code used to identify the PMStore structure.
 *
 * \return the code that identify the PMStore structure.
 */
int pmstore_get_nomenclature_code()
{
	return MDC_MOC_VMO_PMSTORE;
}

/**
 * Sets the PMStore's attribute.
 *
 * \param pm_store the PMStore to have its attribute set.
 * \param attribute the attribute value to be replaced on PMStore.
 *
 */
void pmstore_service_set_attribute(struct PMStore *pm_store, AVA_Type *attribute)
{
	ByteStreamReader *stream = byte_stream_reader_instance(attribute->attribute_value.value,
				   attribute->attribute_value.length);
	pmstore_set_attribute(pm_store, attribute->attribute_id, stream);
	free(stream);
}

/**
 * Initializes a given PMStore attribute with a stream content.
 *
 * \param pmstore the PMStore.
 * \param attr_id the PMStore's attribute ID.
 * \param stream the value of PMStore's attribute.
 *
 * \return \b 1, if the PMStore's attribute is properly modified; \b 0 otherwise.
 *
 */
int pmstore_set_attribute(struct PMStore *pmstore, OID_Type attr_id, ByteStreamReader *stream)
{
	int result = 1;

	switch (attr_id) {
	case MDC_ATTR_ID_HANDLE:
		pmstore->handle = read_intu16(stream, NULL);
		break;
	case MDC_ATTR_PM_STORE_CAPAB:
		pmstore->pm_store_capab = read_intu16(stream, NULL);
		break;
	case MDC_ATTR_METRIC_STORE_SAMPLE_ALG:
		pmstore->store_sample_algorithm = read_intu16(stream, NULL);
		break;
	case MDC_ATTR_METRIC_STORE_CAPAC_CNT:
		pmstore->store_capacity_count = read_intu32(stream, NULL);
		break;
	case MDC_ATTR_METRIC_STORE_USAGE_CNT:
		pmstore->store_usage_count = read_intu32(stream, NULL);
		break;
	case MDC_ATTR_OP_STAT:
		pmstore->operational_state = read_intu16(stream, NULL);
		break;
	case MDC_ATTR_PM_STORE_LABEL_STRING:
		del_octet_string(&pmstore->pm_store_label);
		decode_octet_string(stream, &pmstore->pm_store_label);
		break;
	case MDC_ATTR_TIME_PD_SAMP:
		pmstore->sample_period = read_intu32(stream, NULL);
		break;
	case MDC_ATTR_NUM_SEG:
		pmstore->number_of_segments = read_intu16(stream, NULL);
		break;
	case MDC_ATTR_CLEAR_TIMEOUT:
		pmstore->clear_timeout = read_intu32(stream, NULL);
		break;
	default:
		result = 0;
		break;
	}

	return result;
}

/**
 * Scan a segment of index segment_index, decode segment data and generate xml
 *
 * \param pmstore the PMStore.
 * \param segment the PMSegment
 * \param segm_data_entry output parameter to describe data value.
 */
static void pmstore_populate_all_attributes(struct MDS *mds, struct PMStore *pmstore,
						struct PMSegment *segment, 
						DataEntry *segm_data_entry)
{

	AbsoluteTime abs_time; // length 8
	RelativeTime rel_time; // length 4
	HighResRelativeTime hires_rel_time;	// 8
	intu16 value_ptr_pos = 0;

	int entry_count = segment->segment_usage_count;

	segm_data_entry->choice = COMPOUND_DATA_ENTRY;
	segm_data_entry->u.compound.name = data_strcp("PM-Segment");
	segm_data_entry->u.compound.entries_count = entry_count;
	segm_data_entry->u.compound.entries = calloc(entry_count, sizeof(DataEntry));

	int i;

	for (i = 0; i < entry_count; ++i) {
		DataEntry *data_entry = &segm_data_entry->u.compound.entries[i];
		data_entry->choice = COMPOUND_DATA_ENTRY;
		data_entry->u.compound.name = data_strcp("Pm-Segment-Entry-Map");
		data_entry->u.compound.entries_count = 2;
		data_entry->u.compound.entries = calloc(2, sizeof(DataEntry));


		DataEntry *header_data_entry = &data_entry->u.compound.entries[0];

		switch (segment->pm_segment_entry_map.segm_entry_header) {
		case SEG_ELEM_HDR_ABSOLUTE_TIME: {
			ByteStreamReader *stream = byte_stream_reader_instance(
							   segment->fixed_segment_data.value + value_ptr_pos, 8);
			value_ptr_pos += 8;
			decode_absolutetime(stream, &abs_time);
			data_set_absolute_time(header_data_entry, "Segment-Absolute-Time", &abs_time);
			free(stream);
			break;
		}
		case SEG_ELEM_HDR_RELATIVE_TIME: {
			ByteStreamReader *stream = byte_stream_reader_instance(
							   segment->fixed_segment_data.value + value_ptr_pos, 4);
			value_ptr_pos += 4;
			rel_time = read_intu32(stream, NULL);
			data_set_intu32(header_data_entry, "Segment-Relative-Time", &rel_time);
			free(stream);
			break;
		}
		case SEG_ELEM_HDR_HIRES_RELATIVE_TIME: {
			ByteStreamReader *stream = byte_stream_reader_instance(
							   segment->fixed_segment_data.value + value_ptr_pos, 8);
			value_ptr_pos += 8;
			decode_highresrelativetime(stream, &hires_rel_time);
			data_set_high_res_relative_time(header_data_entry, "Segment-Hires-Relative-Time", &hires_rel_time);
			free(stream);
			break;
		}
		}

		PmSegmentEntryMap entry_map;
		entry_map = segment->pm_segment_entry_map;
		int info_size = entry_map.segm_entry_elem_list.count;

		struct Metric_object *metric_obj = NULL;
		struct MDS_object *object = NULL;

		DataEntry *objs_data_entry = &data_entry->u.compound.entries[1];
		objs_data_entry->choice = COMPOUND_DATA_ENTRY;
		objs_data_entry->u.compound.name = data_strcp("Segm-Entry-Elem-List");
		objs_data_entry->u.compound.entries_count = info_size;
		objs_data_entry->u.compound.entries = calloc(info_size, sizeof(DataEntry));


		int j;

		for (j = 0; j < info_size; ++j) {
			HANDLE handle = entry_map.segm_entry_elem_list.value[j].handle;
			AttrValMap val_map = entry_map.segm_entry_elem_list.value[j].attr_val_map;
			int attr_count = val_map.count;


			object = mds_get_object_by_handle(mds, handle);

			if (object != NULL) {
				if (object->choice == MDS_OBJ_METRIC)
					metric_obj = &(object->u.metric);
			}

			if (metric_obj != NULL) {

				DataEntry *obj_data_entry = &objs_data_entry->u.compound.entries[j];
				obj_data_entry->choice = COMPOUND_DATA_ENTRY;
				obj_data_entry->u.compound.entries_count = attr_count;
				obj_data_entry->u.compound.entries = calloc(attr_count, sizeof(DataEntry));
				data_meta_set_handle(obj_data_entry, handle);

				if (metric_obj->choice == METRIC_NUMERIC) {
					obj_data_entry->u.compound.name = data_strcp("Numeric");
				} else if (metric_obj->choice == METRIC_ENUM) {
					obj_data_entry->u.compound.name = data_strcp("Enumeration");
				} else {
					obj_data_entry->u.compound.name = data_strcp("RT-SA");
				}

				int k;
				struct Metric *metric = NULL;

				for (k = 0; k < attr_count; ++k) {
					DataEntry *entry = &obj_data_entry->u.compound.entries[k];

					ByteStreamReader *stream = byte_stream_reader_instance(
									   segment->fixed_segment_data.value + value_ptr_pos,
									   val_map.value[k].attribute_len);

					value_ptr_pos += val_map.value[k].attribute_len;

					switch (metric_obj->choice) {
					case METRIC_NUMERIC: {
						metric = &metric_obj->u.numeric.metric;
						dimutil_fill_numeric_attr(&(metric_obj->u.numeric),
									  val_map.value[k].attribute_id,
									  stream, entry);
					}
					break;
					case METRIC_ENUM: {
						metric = &metric_obj->u.enumeration.metric;
						int err = dimutil_fill_enumeration_attr(&(metric_obj->u.enumeration),
											val_map.value[k].attribute_id,
											stream, entry);

						if (err == 0) {
							ERROR(" ");
						}
					}
					break;
					case METRIC_RTSA: {
						metric = &metric_obj->u.rtsa.metric;
						dimutil_fill_rtsa_attr(&(metric_obj->u.rtsa),
								       val_map.value[k].attribute_id,
								       stream, entry);
					}
					break;
					}

					free(stream);
				}

				data_set_meta_att(obj_data_entry, data_strcp("metric-id"),
						  intu16_2str((intu16) metric->metric_id));

				data_set_meta_att(obj_data_entry, data_strcp("partition-SCADA-code"),
						  intu16_2str((intu16) metric->type.code));
			}
		}
	}
}

/**
 * Choose a segment to decode fixed segment data
 *
 * \param ctx
 * \param pmstore the PMStore.
 *
 */
static void decode_fixed_segment_data(Context *ctx, struct PMStore *pmstore,
						struct PMSegment *segment)
{
	DataList *list = data_list_new(1);

	pmstore_populate_all_attributes(ctx->mds, pmstore, segment, &list->values[0]);

	// FIXME2 flag with last segment etc.
	manager_notify_evt_segment_data(ctx, pmstore->handle,
					segment->instance_number, list);
}


/**
 * Finalizes and deallocate the given PMStore.
 *
 * \param pm_store the PMStore to be destroyed. This pointer
 * cannot be \b NULL.
 *
 */
void pmstore_destroy(struct PMStore *pm_store)
{
	if (pm_store != NULL) {
		if (pm_store->segm_list != NULL) {
			int i;

			for (i = 0; i < pm_store->segment_list_count; i++) {
				pmsegment_destroy(pm_store->segm_list[i]);
				free(pm_store->segm_list[i]);
			}

			free(pm_store->segm_list);
			pm_store->segm_list = NULL;
		}

		del_octet_string(&pm_store->pm_store_label);
	}
}

/**
 *  Populates data entry with PM-Store attributes
 *
 *  \param ctx the device context
 *  \param handle the PM-Store handle
 *  \return data list representation of PM-Store attributes
 */
DataList *pmstore_get_data_as_datalist(Context *ctx, HANDLE handle)
{
	if (ctx->mds == NULL) {
		return NULL;
	}

	struct MDS_object *mds_obj = mds_get_object_by_handle(ctx->mds, handle);

	if (mds_obj == NULL || mds_obj->choice != MDS_OBJ_PMSTORE) {
		return NULL;
	}

	struct PMStore *pmstore = &mds_obj->u.pmstore;

	int i;
	DataList *list = data_list_new(1);
	DataEntry *entry = &(list->values[0]);
	entry->choice = COMPOUND_DATA_ENTRY;
	data_meta_set_handle(entry, pmstore->handle);

	entry->u.compound.entries_count = 10;
	entry->u.compound.entries = calloc(10, sizeof(DataEntry));
	entry->u.compound.name = data_strcp("Attributes");

	DataEntry *values = entry->u.compound.entries;

	i = 0;

	data_set_intu16(&values[i], "Handle", &pmstore->handle);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_ID_HANDLE);

	data_set_intu16(&values[i], "Capabilities", &pmstore->pm_store_capab);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_PM_STORE_CAPAB);

	data_set_intu16(&values[i], "Store-Sample-Algorithm", &pmstore->store_sample_algorithm);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_METRIC_STORE_SAMPLE_ALG);

	data_set_intu32(&values[i], "Store-Capacity-Count", &pmstore->store_capacity_count);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_METRIC_STORE_CAPAC_CNT);

	data_set_intu32(&values[i], "Store-Usage-Count", &pmstore->store_usage_count);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_METRIC_STORE_USAGE_CNT);

	data_set_intu16(&values[i], "Operational-State", &pmstore->operational_state);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_OP_STAT);

	data_set_label_string(&values[i], "PM-Store-Label", &pmstore->pm_store_label);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_PM_STORE_LABEL_STRING);

	data_set_intu32(&values[i], "Sample-Period", &pmstore->sample_period);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_TIME_PD_SAMP);

	data_set_intu16(&values[i], "Number-Of-Segments", &pmstore->number_of_segments);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_NUM_SEG);

	data_set_intu32(&values[i], "Clear-Timeout", &pmstore->clear_timeout);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_CLEAR_TIMEOUT);
		
	return list;
}

/**
 *  Populates data list with PM-Segment
 *
 *  \param ctx the device context
 *  \param segment the PM-Segment
 *  \param data list to be filled
 */
static void pmstore_get_segment_data_as_datalist(struct PMSegment *segment,
						 DataEntry *entry)
{
	int i = 0;
	char *s_inst_number;

	entry->choice = COMPOUND_DATA_ENTRY;
	asprintf(&s_inst_number, "%d", segment->instance_number);
	data_set_meta_att(entry, data_strcp("Instance-Number"), s_inst_number);

	entry->u.compound.entries_count = 11;
	entry->u.compound.entries = calloc(11, sizeof(DataEntry));
	entry->u.compound.name = data_strcp("Segment");

	DataEntry *values = entry->u.compound.entries;

	data_set_intu16(&values[i], "Instance-Number", &segment->instance_number);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_ID_INSTNO);

	data_set_intu16(&values[i], "Operational-State", &segment->operational_state);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_OP_STAT);

	data_set_intu32(&values[i], "Sample-Period", &segment->sample_period);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_TIME_PD_SAMP);
		
	data_set_label_string(&values[i], "PM-Segment-Label", &segment->segment_label);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_PM_SEG_LABEL_STRING);
		
	data_set_intu16(&values[i], "Person-ID", &segment->pm_seg_person_id);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_PM_SEG_PERSON_ID);
	
	data_set_absolute_time(&values[i], "Start-Time", &segment->segment_start_abs_time);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_TIME_START_SEG);

	data_set_absolute_time(&values[i], "End-Time", &segment->segment_end_abs_time);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_TIME_END_SEG);

	/*
	data_set_intu32(&values[i], "Transfer-Timeout", &segment->transfer_timeout);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_TRANSFER_TIMEOUT);

	data_set_intu32(&values[i], "Confirm-Timeout", &segment->confirm_timeout);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_CONFIRM_TIMEOUT);
	*/

	data_set_segment_stat(&values[i], "Segment-Statistics", &segment->segment_statistics);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_SEG_STATS);

	data_set_intu32(&values[i], "Usage-Count", &segment->segment_usage_count);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_SEG_USAGE_CNT);

	data_set_pm_segment_entry_map(&values[i], "PM-Segment-Entry-Map",
						&segment->pm_segment_entry_map);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_PM_SEG_MAP);

	data_set_absolute_time_adj(&values[i], "Date-And-Time-Adjustment",
				&segment->date_and_time_adjustment);
	data_meta_set_attr_id(&values[i++], MDC_ATTR_TIME_ABS_ADJUST);

	assert(i == 11);
}

/**
 *  Populates data list with PM-Store segment info
 *
 *  \param ctx the device context
 *  \param handle the PM-Store handle
 *  \return data list representation of PM-Store attributes
 */
DataList *pmstore_get_segment_info_data_as_datalist(Context *ctx, HANDLE handle)
{
	if (ctx->mds == NULL) {
		return NULL;
	}

	struct MDS_object *mds_obj = mds_get_object_by_handle(ctx->mds, handle);

	if (mds_obj == NULL || mds_obj->choice != MDS_OBJ_PMSTORE) {
		return NULL;
	}

	struct PMStore *pmstore = &(mds_obj->u.pmstore);

	DataList *list = data_list_new(1);
	DataEntry *entry = &(list->values[0]);
	entry->choice = COMPOUND_DATA_ENTRY;
	data_meta_set_handle(entry, pmstore->handle);

	int i, n;

	n = pmstore->segment_list_count;

	entry->u.compound.entries_count = n;
	entry->u.compound.entries = calloc(n, sizeof(DataEntry));
	entry->u.compound.name = data_strcp("Segments");

	DataEntry *values = entry->u.compound.entries;

	for (i = 0; i < n; ++i) {
		struct PMSegment *seg = pmstore->segm_list[i];
		pmstore_get_segment_data_as_datalist(seg, &values[i]);
	}
		
	return list;
}
/** @} */
