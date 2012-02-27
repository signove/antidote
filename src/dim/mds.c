/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file mds.c
 * \brief MDS type implementation.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mds.h"
#include "dimutil.h"
#include "nomenclature.h"
#include "pmstore.h"
#include "rtsa.h"
#include "src/util/bytelib.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/communication/service.h"
#include "src/communication/operating.h"
#include "src/communication/stdconfigurations.h"
#include "src/communication/extconfigurations.h"
#include "src/api/data_encoder.h"
#include "src/api/text_encoder.h"
#include "src/api/oid_string.h"
#include "src/manager_p.h"
#include "src/util/log.h"

/**
 * \defgroup MDS MDS
 * \ingroup ObjectClasses
 * \brief MDS Object Class
 *
 * Each personal health device agent is defined by an object-oriented model as
 * defined on IEEE 11073-20601 Standard (section 6.3, page 11).
 *
 * The top-level object of each agent is instantiated from de MDS object class.
 * Each agent has one MDS object. The MDS represents the identification and status
 * of the agent through its attributes.
 *
 * @{
 */

/**
 * Time out (seconds) release MDS constant
 */
static const intu32 MDS_TO_CONFIRM_ACTION = 3;

/**
 * Time out (seconds) GET
 */
static const intu32 MDS_TO_GET = 3;

/**
 * Time out (seconds) Confirm Set
 */
static const intu32 MDS_TO_CONFIRM_SET = 3;

/**
 * Time out (seconds) Inter Service
 */
static const intu32 MDS_TO_INTER_SERVICE = 3;

/**
 * Returns a new instance of an MDS object, with an empty object list.
 *
 * \return pointer to MDS the structure created.
 */
MDS *mds_create()
{
	MDS *mds = calloc(1, sizeof(struct MDS));
	mds->dim.id = mds_get_nomenclature_code();
	mds->objects_list = NULL;
	mds->objects_list_count = 0;
	mds->handle = MDS_HANDLE;
	return mds;
}



/**
 * Returns the class identification.
 *
 * \return nomenclature code of this object class.
 */
int mds_get_nomenclature_code()
{
	return MDC_MOC_VMS_MDS_SIMP;
}

/**
 * Action used to retrieve segments info of PMStores
 *
 * \param ctx current context
 * \param handle PM-Store handle
 * \param request_callback
 *
 * \return Request
 */
Request *mds_service_get_segment_info(Context *ctx, int handle,
			 service_request_callback request_callback)
{
	if (ctx->mds != NULL) {
		SegmSelection *selection = calloc(1, sizeof(SegmSelection));
		selection->choice = ALL_SEGMENTS_CHOSEN;
		selection->length = 2;
		selection->u.all_segments = 0;
		int i;

		for (i = 0; i < ctx->mds->objects_list_count; ++i) {
			if (ctx->mds->objects_list[i].choice == MDS_OBJ_PMSTORE &&
				(handle < 0 || handle == ctx->mds->objects_list[i].obj_handle)) {
				Request *result = pmstore_service_action_get_segment_info(
							  ctx,
							  &(ctx->mds->objects_list[i].u.pmstore),
							  selection,
							  request_callback);
				free(selection);
				return result;
			}
		}

		free(selection);
	} else {
		ERROR("No MDS data is available");
	}

	return NULL;
}

/**
 * Action used to retrieve segments info of PMStores
 *
 * \param ctx current context.
 * \param handle PM-Store handle (-1 if any)
 * \param instnumber PM-Instance number (-1 if all)
 * \param request_callback
 * \return Request
 */
Request *mds_service_get_segment_data(Context *ctx,
					int handle,
					int instnumber,
					service_request_callback request_callback)
{
	if (ctx->mds != NULL) {
		struct PMStore pmstore;
		TrigSegmDataXferReq *seg_data_xfer = calloc(1,
						     sizeof(TrigSegmDataXferReq));
		int i;
		int j;

		for (i = 0; i < ctx->mds->objects_list_count; ++i) {
			if (ctx->mds->objects_list[i].choice == MDS_OBJ_PMSTORE && 
			    (handle < 0 || handle == ctx->mds->objects_list[i].obj_handle)) {
				pmstore = ctx->mds->objects_list[i].u.pmstore;

				for (j = 0; j < pmstore.segment_list_count; ++j) {
					if (instnumber >= 0) {
						// want specific instance
						if (instnumber !=
							pmstore.segm_list[j]->instance_number) {
							// not the one we want
							continue;
						}
					}

					seg_data_xfer->seg_inst_no =
							pmstore.segm_list[j]->instance_number;

					Request *result = pmstore_service_action_trig_segment_data_xfer(
									  ctx,
									  &pmstore,
									  seg_data_xfer,
									  request_callback);
					free(seg_data_xfer);
					return result;
				}
			}
		}

		free(seg_data_xfer);
	} else {
		ERROR("No MDS data is available");
	}

	return NULL;
}



/**
 * Action used to clera segments info of PMStores
 *
 * \param ctx current context.
 * \param handle PM-Store handle (-1 if any)
 * \param instnumber PM-Instance number (-1 if all)
 * \param request_callback
 * \return Request
 */
Request *mds_service_clear_segment(Context *ctx, int handle, int instnumber,
				service_request_callback request_callback)
{
	if (ctx->mds != NULL) {
		SegmSelection *selection = calloc(1, sizeof(SegmSelection));
		if (instnumber < 0) {
			selection->choice = ALL_SEGMENTS_CHOSEN;
			selection->length = 2;
			selection->u.all_segments = 0;
		} else {
			selection->choice = SEGM_ID_LIST_CHOSEN;
			selection->length = 2 + 2 + 2; 
			selection->u.segm_id_list.count = 1;
			selection->u.segm_id_list.length = 2;
			selection->u.segm_id_list.value = malloc(2);
			selection->u.segm_id_list.value[0] = instnumber;
		}

		int i;

		for (i = 0; i < ctx->mds->objects_list_count; ++i) {
			if (ctx->mds->objects_list[i].choice == MDS_OBJ_PMSTORE &&
				(handle < 0 || handle == ctx->mds->objects_list[i].obj_handle)) {
				return pmstore_service_action_clear_segments_send_command(
					       ctx,
					       &(ctx->mds->objects_list[i].u.pmstore),
					       selection, request_callback);
			}
		}

		del_segmselection(selection);
		free(selection);
	} else {
		ERROR("No MDS data is available");
	}

	return NULL;
}

/**
 * Checks if the manager-initiated measurement data transmission
 * is supported.
 *
 * \param mds the mds
 * \param data_req_mode configuration of the manager-initiated transmission
 *
 * \return return zero if the manager-initiated measurement data transmission
 *	 is supported
 */
int mds_is_supported_data_request(MDS *mds, DataReqMode data_req_mode)
{
	DataReqModeFlags capab = mds->data_req_mode_capab.data_req_mode_flags;
	// Check data_req_mode_capab;
	int supportedReqType = (data_req_mode & DATA_REQ_START_STOP) ||
			       (data_req_mode & DATA_REQ_CONTINUATION) ||
			       (capab & DATA_REQ_SUPP_STOP);

	int supportedModeType = ((data_req_mode & DATA_REQ_MODE_SINGLE_RSP) && (capab & DATA_REQ_SUPP_MODE_SINGLE_RSP)) ||
				((data_req_mode & DATA_REQ_MODE_TIME_PERIOD) && (capab & DATA_REQ_SUPP_MODE_TIME_PERIOD)) ||
				((data_req_mode & DATA_REQ_MODE_TIME_NO_LIMIT) && (capab & DATA_REQ_SUPP_MODE_TIME_NO_LIMIT));


	int supportedScopeType = ((data_req_mode & DATA_REQ_SCOPE_ALL) && (capab & DATA_REQ_SUPP_SCOPE_ALL)) ||
				 ((data_req_mode & DATA_REQ_SCOPE_TYPE) && (capab & DATA_REQ_SUPP_SCOPE_CLASS)) ||
				 ((data_req_mode & DATA_REQ_SCOPE_HANDLE) && (capab & DATA_REQ_SUPP_SCOPE_HANDLE));


	int supportedPersonalId = (!(data_req_mode & DATA_REQ_MODE_DATA_REQ_PERSON_ID) || (capab & DATA_REQ_SUPP_PERSON_ID));


	return supportedReqType && supportedModeType && supportedScopeType && supportedPersonalId;
}


/**
 * Enables/Disables measurement data transmission from the agent.
 *
 * \param ctx the current context.
 * \param data_req_mode
 * \param class_id
 * \param handle_list
 * \param request_callback callback function called whenever the request
 * response is received.
 *
 * \return a Request struct created to receive future responses.
 */
Request *mds_service_action_data_request(Context *ctx, DataReqMode data_req_mode,
		OID_Type *class_id, HANDLEList *handle_list,
		service_request_callback request_callback)
{
	if (ctx->mds == NULL) {
		ERROR("No MDS data is available");
		return NULL;
	}

	if (mds_is_supported_data_request(ctx->mds, data_req_mode)) {
		APDU *apdu = (APDU *)calloc(1, sizeof(APDU));
		DATA_apdu *data_apdu = (DATA_apdu *) calloc(1, sizeof(DATA_apdu));

		if (apdu != NULL && data_apdu != NULL) {
			apdu->choice = PRST_CHOSEN;

			data_apdu->message.choice = ROIV_CMIP_CONFIRMED_ACTION_CHOSEN;

			data_apdu->message.u.roiv_cmipConfirmedAction.obj_handle = 0;
			data_apdu->message.u.roiv_cmipConfirmedAction.action_type
			= MDC_ACT_DATA_REQUEST;

			DataRequest *request = (DataRequest *) calloc(1,
					       sizeof(DataRequest));
			request->data_req_id = 0x0100; // Just one data request
			request->data_req_mode = data_req_mode;
			request->data_req_time = 0;
			request->data_req_person_id = 0;

			if (class_id != NULL) {
				request->data_req_class = *class_id;
			}

			if (handle_list != NULL) {
				request->data_req_obj_handle_list = *handle_list;
			} else {
				request->data_req_obj_handle_list.count = 0;
				request->data_req_obj_handle_list.length = 0;
				request->data_req_obj_handle_list.value = NULL;
			}

			int length = sizeof(DataReqId) + sizeof(DataReqMode)
				     + sizeof(RelativeTime) + sizeof(intu16)
				     + sizeof(OID_Type) + 2 * sizeof(intu16)
				     + request->data_req_obj_handle_list.length;

			ByteStreamWriter *writer1 = byte_stream_writer_instance(length);
			encode_datarequest(writer1, request);

			free(request);
			request = NULL;

			data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.value
			= writer1->buffer;
			data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.length
			= writer1->size;

			data_apdu->message.length
			= data_apdu->message.u.roiv_cmipConfirmedAction.action_info_args.length
			  + sizeof(intu16)
			  + sizeof(OID_Type)
			  + sizeof(HANDLE);

			apdu->u.prst.length = data_apdu->message.length
					      + sizeof(intu16) + sizeof(DATA_apdu_choice)
					      + sizeof(InvokeIDType);

			apdu->length = apdu->u.prst.length + sizeof(intu16);
			encode_set_data_apdu(&apdu->u.prst, data_apdu);

			// Send APDU
			timeout_callback no_timeout = NO_TIMEOUT;

			Request *req = service_send_remote_operation_request(ctx, apdu,
					no_timeout, request_callback);

			free(writer1); // writer1->buffer is used in data_apdu

			return req;
		}

		return NULL;
	} else {
		// TODO Error Condition
		return NULL;
	}


}

/**
 * Allows the manager system to set a real-time clock (RTC) with the absolute
 * time. The agent indicates whether the Set-Time command is valid by using
 * the mds-time-capab-set-clock bit in the Mds-Time-Info attribute.
 *
 * \param ctx
 * \param time
 * \param request_callback
 *
 * \return
 *
 */
Request *mds_service_action_set_time(Context *ctx, SetTimeInvoke *time, service_request_callback request_callback)
{
	// FIXME transcoded device handling (not necessarily here)
	return operating_action_set_time(ctx, time, MDS_TO_CONFIRM_ACTION, request_callback);
}

/**
 * The manager may request the MDS object attributes of the agent in which case
 * the manager shall send the "Remote Operation Invoke | Get" command
 * with the reserved handle value of 0. The agent shall respond by reporting
 * its MDS object attributes to the manager using the "Remote Operation
 * Response | Get" response. In the response to a Get MDS Object command,
 * only attributes implemented by the agent are returned.
 *
 * \param ctx current context.
 * \param attributeids_list list of ids or NULL if you want to get all attributes
 * \param attributeids_list_count number of attribute ids 0 if you want to get all attributes
 * \param request_callback
 *
 * \return request created
 */
Request *mds_service_get(Context *ctx, OID_Type *attributeids_list, int attributeids_list_count, service_request_callback request_callback)
{
	return operating_service_get(ctx, MDS_HANDLE, attributeids_list, attributeids_list_count,
					MDS_TO_GET, request_callback);
}


/**
 * Request PM-Store attributes from agent device
 *
 * \param ctx current context.
 * \param handle PM-Store handle
 * \param request_callback
 *
 * \return request created
 */
Request *mds_get_pmstore(Context *ctx, int handle, service_request_callback request_callback)
{
	// FIXME transcoded device handling (not necessarily here)
	return operating_service_get(ctx, handle, NULL, 0, MDS_TO_GET, request_callback);
}

/**
 * This function configure the MDS structure using agent sent data which
 * provides information about the supported measurement capabilities
 * of the agent.
 *
 * After configuration steps the Manager is ready to execute operational mode
 *
 * \param ctx context Operating Context
 * \param config_obj_list Configuration object list
 * \param manager Manager flag
 */
void mds_configure_operating(Context *ctx, ConfigObjectList *config_obj_list,
				int manager)
{
	int obj_list_size = config_obj_list->count;
	int attr_list_size = 0;
	int i;
	int j;

	MDS *mds  = ctx->mds;

	for (i = 0; i < obj_list_size; ++i) {
		struct MDS_object object;

		ConfigObject *cfgObj = &(config_obj_list->value[i]);
		attr_list_size = cfgObj->attributes.count;

		switch (cfgObj->obj_class) {
			// TODO remove metric case
		case MDC_MOC_VMO_METRIC:
			// TODO: need to add metric to metric_object list?
			break;
		case MDC_MOC_VMO_METRIC_NU: {
			object.choice = MDS_OBJ_METRIC;
			struct Metric_object *metric_obj = &(object.u.metric);
			metric_obj->choice = METRIC_NUMERIC;
			object.obj_handle = cfgObj->obj_handle;

			struct Metric *metric = metric_instance();
			struct Numeric *numeric = numeric_instance(metric);

			metric_obj->u.numeric = *numeric;

			free(numeric);
			free(metric);
			numeric = NULL;
			metric = NULL;

			for (j = 0; j < attr_list_size; ++j) {
				ByteStreamReader *stream = byte_stream_reader_instance(cfgObj->attributes.value[j].attribute_value.value,
							   cfgObj->attributes.value[j].attribute_value.length);

				dimutil_fill_numeric_attr(&(object.u.metric.u.numeric),
							  cfgObj->attributes.value[j].attribute_id,
							  stream, NULL);
				free(stream);
			}

			object.u.metric.u.numeric.metric.handle = cfgObj->obj_handle;
			mds_add_object(mds, object);
			break;
		}
		case MDC_MOC_VMO_METRIC_ENUM: {
			object.choice = MDS_OBJ_METRIC;
			struct Metric_object *metric_obj = &(object.u.metric);
			metric_obj->choice = METRIC_ENUM;
			object.obj_handle = cfgObj->obj_handle;

			struct Metric *metric = metric_instance();
			struct Enumeration *enumeration = enumeration_instance(metric);

			metric_obj->u.enumeration = *enumeration;

			free(enumeration);
			free(metric);
			enumeration = NULL;
			metric = NULL;

			for (j = 0; j < attr_list_size; ++j) {
				ByteStreamReader *stream = byte_stream_reader_instance(cfgObj->attributes.value[j].attribute_value.value,
							   cfgObj->attributes.value[j].attribute_value.length);

				dimutil_fill_enumeration_attr(&(object.u.metric.u.enumeration),
							      cfgObj->attributes.value[j].attribute_id,
							      stream, NULL);
				free(stream);
			}

			object.u.metric.u.enumeration.metric.handle = cfgObj->obj_handle;
			mds_add_object(mds, object);

			break;
		}
		case MDC_MOC_VMO_METRIC_SA_RT: {
			// TODO: verify rtsa_instance
			/*
			object.choice = METRIC;
			struct Metric_object *metric_obj = &(object.u.metric);
			metric_obj->choice = METRIC_RTSA;
			object.obj_handle = cfgObj->obj_handle;


			struct Metric *metric = metric_instance();
			struct METRIC_RTSA *rtsa = rtsa(metric);

			metric_obj->u.rtsa = *rtsa;

			free(rtsa);
			free(metric);
			rtsa = NULL;
			metric = NULL;

			for (j = 0; j < attr_list_size; ++j) {
				ByteStreamReader *stream = byte_stream_reader_instance(cfgObj->attributes.value[j].attribute_value.value,
								cfgObj->attributes.value[j].attribute_value.length);
				result = mds_fill_metric_attr(&(object.u.metric.u.rtsa.metric),
						cfgObj->attributes.value[j].attribute_id,
						stream);
				free(stream);
			}
			mds_add_object(object);
			*/
			break;
		}
		case MDC_MOC_VMO_PMSTORE: {
			object.obj_handle = cfgObj->obj_handle;
			object.u.pmstore.handle = cfgObj->obj_handle;
			object.choice = MDS_OBJ_PMSTORE;

			for (j = 0; j < attr_list_size; ++j) {
				// TODO: check if data can come aggregated
				ByteStreamReader *stream = byte_stream_reader_instance(cfgObj->attributes.value[j].attribute_value.value,
							   cfgObj->attributes.value[j].attribute_value.length);
				pmstore_set_attribute(&(object.u.pmstore),
						      cfgObj->attributes.value[j].attribute_id,
						      stream);
				free(stream);
			}

			mds_add_object(mds, object);
			break;
		}
		case MDC_MOC_SCAN:
			// TODO: need to add scanner to scanner_object list?
			break;
		case MDC_MOC_SCAN_CFG:
			// TODO: need to add cfg_scanner to cfg_scanner_object list?
			break;
		case MDC_MOC_SCAN_CFG_EPI: {
			object.choice = MDS_OBJ_SCANNER;
			object.obj_handle = cfgObj->obj_handle;

			struct Scanner *scanner = scanner_instance(cfgObj->obj_handle, os_disabled);
			struct CfgScanner *cfg_scanner = cfg_scanner_instance(scanner, unconfirmed);
			struct EpiCfgScanner *epi_cfg_scanner = epi_cfg_scanner_instance(cfg_scanner);

			struct Scanner_object *scanner_obj = &(object.u.scanner);
			scanner_obj->choice = EPI_CFG_SCANNER;
			scanner_obj->u.epi_cfg_scanner = *epi_cfg_scanner;

			free(scanner);
			free(cfg_scanner);
			free(epi_cfg_scanner);

			for (j = 0; j < attr_list_size; ++j) {
				ByteStreamReader *stream = byte_stream_reader_instance(cfgObj->attributes.value[j].attribute_value.value,
							   cfgObj->attributes.value[j].attribute_value.length);
				dimutil_fill_epi_scanner_attr(&(object.u.scanner.u.epi_cfg_scanner),
							      cfgObj->attributes.value[j].attribute_id,
							      stream, NULL);
				free(stream);
			}

			mds_add_object(mds, object);
			break;
		}
		case MDC_MOC_SCAN_CFG_PERI: {
			object.choice = MDS_OBJ_SCANNER;
			object.obj_handle = cfgObj->obj_handle;

			struct Scanner *scanner = scanner_instance(cfgObj->obj_handle, os_disabled);
			struct CfgScanner *cfg_scanner = cfg_scanner_instance(scanner, unconfirmed);
			struct PeriCfgScanner *peri_cfg_scanner = peri_cfg_scanner_instance(cfg_scanner);

			struct Scanner_object *scanner_obj = &(object.u.scanner);
			scanner_obj->choice = PERI_CFG_SCANNER;
			scanner_obj->u.peri_cfg_scanner = *peri_cfg_scanner;

			free(scanner);
			free(cfg_scanner);
			free(peri_cfg_scanner);

			for (j = 0; j < attr_list_size; ++j) {
				ByteStreamReader *stream = byte_stream_reader_instance(cfgObj->attributes.value[j].attribute_value.value,
							   cfgObj->attributes.value[j].attribute_value.length);
				dimutil_fill_peri_scanner_attr(&(object.u.scanner.u.peri_cfg_scanner),
							       cfgObj->attributes.value[j].attribute_id,
							       stream, NULL);
				free(stream);
			}

			mds_add_object(mds, object);
			break;
		}
		default:
			// TODO: error handling
			break;
		}
	}

	service_init(ctx);

	if (manager) {
		DataList *list = data_list_new(1);
		mds_populate_attributes(mds, &list->values[0]);

		manager_notify_evt_device_available(ctx, list);
	}

	del_configobjectlist(config_obj_list);
	config_obj_list = NULL;
}

/**
 *  Populates data entry with configuration object attributes
 *
 *  \param oid type of object
 *  \param atts attributes of object
 *  \param superentry master data entry of object
 */
static void mds_populate_configuration_attributes(OID_Type supertype, 
						const char *name,
						const AttributeList *atts,
						DataEntry *superentry)
{
	int j;

	// These objects are here just to satifsy the dimutil_*_fill_attr()
	// functions, that are also used to update actual MDS objects

	struct Metric *metric = metric_instance();
	struct Metric *metric_n = metric_instance();
	struct Metric *metric_e = metric_instance();
	struct Metric *metric_s = metric_instance();
	struct Numeric *numeric = numeric_instance(metric_n);
	struct Enumeration *enumeration = enumeration_instance(metric_e);
	struct PMStore *pmstore = pmstore_instance();

	octet_string simple_sa_obs_value;
	simple_sa_obs_value.length = 64;
	simple_sa_obs_value.value = calloc(64, sizeof(intu8));
	ScaleRangeSpec32 srs32;
	SaSpec saspec;
	struct RTSA *sart = rtsa_instance_spec32(metric_s, 1, simple_sa_obs_value,
							srs32, saspec);

	superentry->choice = COMPOUND_DATA_ENTRY;
	superentry->u.compound.entries_count = atts->count;
	superentry->u.compound.entries = calloc(atts->count, sizeof(DataEntry));
	superentry->u.compound.name = data_strcp(name);
		

	for (j = 0; j < atts->count; ++j) {
		AVA_Type att = atts->value[j];
		OID_Type type = att.attribute_id;
		Any data = att.attribute_value;
		ByteStreamReader *r = byte_stream_reader_instance(data.value, data.length);
		DataEntry *entry = &(superentry->u.compound.entries[j]);
		int res = 0;

		if (supertype == MDC_MOC_VMO_METRIC_NU) {
			res = dimutil_fill_numeric_attr(numeric, type, r, entry);
		} else if (supertype == MDC_MOC_VMO_METRIC_ENUM) {
			res = dimutil_fill_enumeration_attr(enumeration, type, r, entry);
		} else if (supertype == MDC_MOC_VMO_METRIC) {
			res = dimutil_fill_metric_attr(metric, type, r, entry);
		} else if (supertype == MDC_MOC_VMO_METRIC_SA_RT) {
			res = dimutil_fill_rtsa_attr(sart, type, r, entry);
		} else if (supertype == MDC_MOC_VMO_PMSTORE) {
			res = dimutil_fill_pmstore_attr(pmstore, type, r, entry);
		}

		if (!res) {
			data_set_oid_type(entry, "OID", &type);
		}

		free(r);
	}

	metric_destroy(metric);
	numeric_destroy(numeric);
	pmstore_destroy(pmstore);
	rtsa_destroy(sart);
	enumeration_destroy(enumeration);

	free(metric);
	free(metric_e);
	free(metric_n);
	free(metric_s);
	free(numeric);
	free(pmstore);
	free(sart);
	free(enumeration);
}

/**
 *  Populates data entry with device configuration
 *
 *  \param mds the MDS object of agent
 *  \return data list representation of configuration
 */
DataList *mds_populate_configuration(MDS *mds)
{
	int i;

	if (mds == NULL) {
		return NULL;
	}

	ConfigObjectList *config;

	// gets a copy of config attributes
	if (std_configurations_is_supported_standard(mds->dev_configuration_id)) {
		config = std_configurations_get_configuration_attributes(
								mds->dev_configuration_id);
	} else {
		config = ext_configurations_get_configuration_attributes(&mds->system_id,
								mds->dev_configuration_id);
	}

	if (!config)
		return NULL;

	DataList *list = data_list_new(config->count);

	for (i = 0; i < config->count; ++i) {
		const ConfigObject *cfgobj = &(config->value[i]);
		const char *name = oid_get_moc_vmo_string(cfgobj->obj_class);
		DataEntry *entry = &(list->values[i]);
		const AttributeList *atts = &(cfgobj->attributes);

		entry->choice = COMPOUND_DATA_ENTRY;
		data_meta_set_handle(entry, cfgobj->obj_handle);

		mds_populate_configuration_attributes(cfgobj->obj_class, name, atts, entry);
	}

	del_configobjectlist(config);
	free(config);

	return list;
}

/**
 *  Populates data entry with mds attributes
 *
 *  @param mds
 *  @param entry
 */
void mds_populate_attributes(MDS *mds, DataEntry *entry)
{
	if (mds == NULL || entry == NULL) {
		return;
	}

	int size = 6;
	entry->choice = COMPOUND_DATA_ENTRY;
	entry->u.compound.entries_count = size;
	entry->u.compound.entries = calloc(size, sizeof(DataEntry));
	entry->u.compound.name = data_strcp("MDS");

	DataEntry *values = entry->u.compound.entries;

	data_set_production_spec(&values[--size],
				 "Production-Specification",
				 &mds->production_specification);
	data_meta_set_attr_id(&values[size], MDC_ATTR_ID_PROD_SPECN);

	data_set_dev_config_id(&values[--size], "Dev-Configuration-Id",
			       &mds->dev_configuration_id);
	data_meta_set_attr_id(&values[size], MDC_ATTR_DEV_CONFIG_ID);

	data_set_sys_type_spec_list(&values[--size], "System-Type-Spec-List",
				&mds->system_type_spec_list);
	data_meta_set_attr_id(&values[size], MDC_ATTR_SYS_TYPE_SPEC_LIST);

	data_set_system_id(&values[--size], "System-Id", &mds->system_id);
	data_meta_set_attr_id(&values[size], MDC_ATTR_SYS_ID);

	data_set_system_model(&values[--size], "System-Model",
			      &mds->system_model);
	data_meta_set_attr_id(&values[size], MDC_ATTR_ID_MODEL);

	data_set_type(&values[--size], "System-Type", &mds->system_type);
	data_meta_set_attr_id(&values[size], MDC_ATTR_SYS_TYPE);
}

/**
 * Adds a MDS_object to a dynamic list.
 *
 * \param mds the mds
 * \param object the MDS_object struct to be inserted into a dynamic list.
 */
void mds_add_object(MDS *mds, struct MDS_object object)
{
	// test if there is not elements in the list
	if (mds->objects_list_count == 0 && mds != NULL) {
		mds->objects_list = malloc(sizeof(struct MDS_object));
		memset(mds->objects_list, 0, sizeof(struct MDS_object));

	} else { // change the list size
		mds->objects_list = realloc(mds->objects_list,
					    sizeof(struct MDS_object)
					    * (mds->objects_list_count + 1));
		memset(mds->objects_list+mds->objects_list_count, 0, sizeof(struct MDS_object));

		if (mds->objects_list == NULL) {
			ERROR("ERROR");
			exit(1);
		}
	}

	// add element to list
	mds->objects_list[mds->objects_list_count] = object;
	mds->objects_list_count += 1;
}

/**
 * Return a MDS_object selected by the HANDLE. If there is no
 * MDS_object with given HANDLE, this function returns \b NULL.
 *
 * \param mds
 * \param obj_handle the handle used to select the MDS_object.
 *
 * \return a pointer to an MDS_object with the given HANDLE; \b NULL
 * if there is no MDS_object with the given HANDLE.
 *
 * \param mds
 */
struct MDS_object *mds_get_object_by_handle(MDS *mds, HANDLE obj_handle)
{
	int object_list_size = 0;
	int i;

	if (mds != NULL) {
		object_list_size = mds->objects_list_count;
	}

	for (i = 0; i < object_list_size; ++i) {
		if (mds->objects_list[i].obj_handle == obj_handle) {
			return &(mds->objects_list[i]);
		}
	}

	return NULL;
}


/**
 * This event provides dynamic data (typically measurements) from the agent for
 * some or all of the objects that the agent supports. Data for reported objects
 * are reported using a generic attribute list variable format. The event is
 * triggered by an MDS-Data-Request from the manager system, or it is sent as an
 * unsolicited message by the agent. For agents that support manager-initiated
 * measurement data transmission.
 *
 * \param ctx
 * \param info_var
 */
void mds_event_report_dynamic_data_update_var(Context *ctx, ScanReportInfoVar *info_var)
{
	// TODO Metric_object must have been created before, in requisition step
	int info_size = info_var->obs_scan_var.count;
	DataList *data_list = data_list_new(info_size);

	if (data_list != NULL && info_size > 0) {
		int i;

		for (i = 0; i < info_size; ++i) {
			dimutil_update_mds_from_obs_scan(ctx->mds, &info_var->obs_scan_var.value[i], &data_list->values[i]);
		}

		manager_notify_evt_measurement_data_updated(ctx, data_list);
	} else {
		// TODO Error Condition
	}
}



/**
 * This event provides dynamic data (typically measurements) from the agent for
 * some or all of the metric objects or the MDS object that the agent supports.
 * Data are reported in the fixed format defined by the Attribute-Value-Map
 * attribute for reported metric objects or the MDS object.
 *
 * The event is triggered by an MDS-Data-Request from the manager system (i.e., a
 * manager-initiated measurement data transmission), or it is sent as an
 * unsolicited message by the agent (i.e., an agent-initiated measurement data
 * transmission).
 *
 * \param ctx
 * \param info_fixed
 */
void mds_event_report_dynamic_data_update_fixed(Context *ctx, ScanReportInfoFixed *info_fixed)
{

	int info_size = info_fixed->obs_scan_fixed.count;
	DataList *data_list = data_list_new(info_size);

	if (data_list != NULL && info_size > 0) {
		int i;

		for (i = 0; i < info_size; ++i) {
			dimutil_update_mds_from_obs_scan_fixed(ctx->mds, &info_fixed->obs_scan_fixed.value[i], &data_list->values[i]);
		}

		manager_notify_evt_measurement_data_updated(ctx, data_list);
	} else {
		// TODO Error Condition
	}
}

/**
 * This is the same as MDS-Dynamic-Data-Update-Var, but allows inclusion
 * of data from multiple persons.
 *
 * \param ctx
 * \param info_mp_var
 */
void mds_event_report_dynamic_data_update_mp_var(Context *ctx,
		ScanReportInfoMPVar *info_mp_var)
{
	int info_mp_list_size = info_mp_var->scan_per_var.count;
	int i;

	for (i = 0; i < info_mp_list_size; ++i) {
		int info_size = info_mp_var->scan_per_var.value[i].obs_scan_var.count;
		DataList *data_list = data_list_new(info_size);

		if (data_list != NULL && info_size > 0) {
			int j;

			for (j = 0; j < info_size; ++j) {
				data_meta_set_personal_id(&data_list->values[j],
							  info_mp_var->scan_per_var.value[i].person_id);

				dimutil_update_mds_from_obs_scan(ctx->mds, &info_mp_var->scan_per_var.value[i].obs_scan_var.value[j],
								 &data_list->values[j]);
			}

			manager_notify_evt_measurement_data_updated(ctx, data_list);
		} else {
			// TODO Error Condition
		}
	}
}

/**
 * This is the same as MDS-Dynamic-Data-Update-Fixed, but allows inclusion
 * of data from multiple persons.
 *
 * \param ctx
 * \param info_mp_fixed
 */
void mds_event_report_dynamic_data_update_mp_fixed(Context *ctx,
		ScanReportInfoMPFixed *info_mp_fixed)
{
	int info_fixed_list_size = info_mp_fixed->scan_per_fixed.count;
	int i;

	for (i = 0; i < info_fixed_list_size; ++i) {
		int info_size = info_mp_fixed->scan_per_fixed.value[i].obs_scan_fix.count;
		DataList *data_list = data_list_new(info_size);

		if (data_list != NULL && info_size > 0) {
			int j;

			for (j = 0; j < info_size; ++j) {
				data_meta_set_personal_id(&data_list->values[j],
							  info_mp_fixed->scan_per_fixed.value[i].person_id);

				dimutil_update_mds_from_obs_scan_fixed(ctx->mds,
								       &info_mp_fixed->scan_per_fixed.value[i].obs_scan_fix.value[j],
								       &data_list->values[j]);
			}

			manager_notify_evt_measurement_data_updated(ctx, data_list);
		} else {
			// TODO Error Condition
		}
	}
}

/**
 * Gets all attributes of MDS Instance as an array of AVA_Type's
 *
 * \param mds the mds.
 * \param count The number of attributes
 * \param tot_length The total length of the MDS
 * \return Array of AVA_Type with attributes
 */
AVA_Type* mds_get_attributes(MDS *mds, intu16* count, intu16 *tot_length)
{
	int i;
	*count = 17;
	AVA_Type *ava = calloc(*count, sizeof(AVA_Type));
	intu16 length;
	ByteStreamWriter *stream;

	*tot_length += 17 * sizeof(ava[0].attribute_id);
	*tot_length += 17 * sizeof(ava[0].attribute_value.length);

	length = 4;
	ava[0].attribute_id = MDC_ATTR_SYS_TYPE;
	stream = byte_stream_writer_instance(length);
	encode_type(stream, &mds->system_type);
	ava[0].attribute_value.value = stream->buffer;
	ava[0].attribute_value.length = length;
	// stream buffer lives beyond stream
	free(stream);
	*tot_length += length;

	length = sizeof(intu16) * 2 + mds->system_model.manufacturer.length +
					mds->system_model.model_number.length;
	ava[1].attribute_id = MDC_ATTR_ID_MODEL;
	stream = byte_stream_writer_instance(length);
	encode_systemmodel(stream, &mds->system_model);
	ava[1].attribute_value.value = stream->buffer;
	ava[1].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	// length includes intu16 because it is variable-size
	length = sizeof(intu16) + mds->system_id.length;
	ava[2].attribute_id = MDC_ATTR_SYS_ID;
	stream = byte_stream_writer_instance(length);
	encode_octet_string(stream, &mds->system_id);
	ava[2].attribute_value.value = stream->buffer;
	ava[2].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 2;
	ava[3].attribute_id = MDC_ATTR_DEV_CONFIG_ID;
	stream = byte_stream_writer_instance(length);
	encode_configid(stream, &mds->dev_configuration_id);
	ava[3].attribute_value.value = stream->buffer;
	ava[3].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 2 + 2 + mds->attribute_value_map.count * (2 + 2);
	ava[4].attribute_id = MDC_ATTR_ATTRIBUTE_VAL_MAP;
	stream = byte_stream_writer_instance(length);
	encode_attrvalmap(stream, &mds->attribute_value_map);
	ava[4].attribute_value.value = stream->buffer;
	ava[4].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 2 + 2 + mds->production_specification.count * (2 + 2 + 2);

        for (i = 0; i < mds->production_specification.count; i++) {
		ProdSpecEntry *entry = &mds->production_specification.value[i];
		length += entry->prod_spec.length;
        }

	ava[5].attribute_id = MDC_ATTR_ID_PROD_SPECN;
	stream = byte_stream_writer_instance(length);
	encode_productionspec(stream, &mds->production_specification);
	ava[5].attribute_value.value = stream->buffer;
	ava[5].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 2 + 2 + 4 + 2 + 2 + 4;
	ava[6].attribute_id = MDC_ATTR_MDS_TIME_INFO;
	stream = byte_stream_writer_instance(length);
	encode_mdstimeinfo(stream, &mds->mds_time_info);
	ava[6].attribute_value.value = stream->buffer;
	ava[6].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 8;
	ava[7].attribute_id = MDC_ATTR_TIME_ABS;
	stream = byte_stream_writer_instance(length);
	encode_absolutetime(stream, &mds->date_and_time);
	ava[7].attribute_value.value = stream->buffer;
	ava[7].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 4;
	ava[8].attribute_id = MDC_ATTR_TIME_REL;
	stream = byte_stream_writer_instance(length);
	write_intu32(stream, mds->relative_time);
	ava[8].attribute_value.value = stream->buffer;
	ava[8].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 8;
	ava[9].attribute_id = MDC_ATTR_TIME_REL_HI_RES;
	stream = byte_stream_writer_instance(length);
	encode_highresrelativetime(stream, &mds->hires_relative_time);
	ava[9].attribute_value.value = stream->buffer;
	ava[9].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 6;
	ava[10].attribute_id = MDC_ATTR_TIME_ABS_ADJUST;
	stream = byte_stream_writer_instance(length);
	encode_absolutetimeadjust(stream, &mds->date_and_time_adjustment);
	ava[10].attribute_value.value = stream->buffer;
	ava[10].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 2;
	ava[11].attribute_id = MDC_ATTR_POWER_STAT;
	stream = byte_stream_writer_instance(length);
	write_intu16(stream, mds->power_status);
	ava[11].attribute_value.value = stream->buffer;
	ava[11].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 2;
	ava[12].attribute_id = MDC_ATTR_VAL_BATT_CHARGE;
	stream = byte_stream_writer_instance(length);
	write_intu16(stream, mds->battery_level);
	ava[12].attribute_value.value = stream->buffer;
	ava[12].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 4 + 2;
	ava[13].attribute_id = MDC_ATTR_TIME_BATT_REMAIN;
	stream = byte_stream_writer_instance(length);
	encode_batmeasure(stream, &mds->remaining_battery_time);
	ava[13].attribute_value.value = stream->buffer;
	ava[13].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 2 + 2 + mds->reg_cert_data_list.count * ((1 + 1) + 2);

        for (i = 0; i < mds->reg_cert_data_list.count; i++) {
		RegCertData *entry = &mds->reg_cert_data_list.value[i];
		length += entry->auth_body_data.length;
        }

	ava[14].attribute_id = MDC_ATTR_REG_CERT_DATA_LIST;
	stream = byte_stream_writer_instance(length);
	encode_regcertdatalist(stream, &mds->reg_cert_data_list);
	ava[14].attribute_value.value = stream->buffer;
	ava[14].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 2 + 2 + mds->system_type_spec_list.count * (2 + 2);
	ava[15].attribute_id = MDC_ATTR_SYS_TYPE_SPEC_LIST;
	stream = byte_stream_writer_instance(length);
	encode_typeverlist(stream, &mds->system_type_spec_list);
	ava[15].attribute_value.value = stream->buffer;
	ava[15].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	length = 4;
	ava[16].attribute_id = MDC_ATTR_CONFIRM_TIMEOUT;
	stream = byte_stream_writer_instance(length);
	write_intu32(stream, mds->confirm_timeout);
	ava[16].attribute_value.value = stream->buffer;
	ava[16].attribute_value.length = length;
	free(stream);
	*tot_length += length;

	return ava;
}

/**
 * Sets the specified attribute of an MDS instance.
 *
 * \param mds the mds.
 * \param attribute a pointer to an attribute of an MDS instance.
 */
void mds_set_attribute(MDS *mds, AVA_Type *attribute)
{
	int error = 0;
	intu8 *value = attribute->attribute_value.value;
	intu16 length = attribute->attribute_value.length;

	switch (attribute->attribute_id) {
	case MDC_ATTR_ID_HANDLE:
		break;
	case MDC_ATTR_SYS_TYPE: {
		TYPE s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_type(stream, &s, &error);
		if (!error) {
			del_type(&mds->system_type);
			mds->system_type = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_ID_MODEL: {
		SystemModel s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_systemmodel(stream, &s, &error);
		if (!error) {
			del_systemmodel(&mds->system_model);
			mds->system_model = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_SYS_ID: {
		octet_string s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_octet_string(stream, &s, &error);
		if (! error) {
			del_octet_string(&mds->system_id);
			mds->system_id = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_DEV_CONFIG_ID: {
		ConfigId s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_configid(stream, &s, &error);
		if (! error) {
			del_configid(&mds->dev_configuration_id);
			mds->dev_configuration_id = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_ATTRIBUTE_VAL_MAP: {
		AttrValMap s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_attrvalmap(stream, &s, &error);
		if (! error) {
			del_attrvalmap(&mds->attribute_value_map);
			mds->attribute_value_map = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_ID_PROD_SPECN: {
		ProductionSpec s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_productionspec(stream, &s, &error);
		if (! error) {
			del_productionspec(&mds->production_specification);
			mds->production_specification = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_MDS_TIME_INFO: {
		MdsTimeInfo s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_mdstimeinfo(stream, &s, &error);
		if (! error) {
			del_mdstimeinfo(&mds->mds_time_info);
			mds->mds_time_info = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_TIME_ABS: {
		AbsoluteTime s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_absolutetime(stream, &s, &error);
		if (! error) {
			del_absolutetime(&mds->date_and_time);
			mds->date_and_time = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_TIME_REL: {
		RelativeTime s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		s = read_intu32(stream, &error);
		if (! error) {
			// del_relative_time not needed
			mds->relative_time = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_TIME_REL_HI_RES: {
		HighResRelativeTime s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_highresrelativetime(stream, &s, &error);
		if (! error) {
			del_highresrelativetime(&mds->hires_relative_time);
			mds->hires_relative_time = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_TIME_ABS_ADJUST: {
		AbsoluteTimeAdjust s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_absolutetimeadjust(stream, &s, &error);
		if (! error) {
			del_absolutetimeadjust(&mds->date_and_time_adjustment);
			mds->date_and_time_adjustment = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_POWER_STAT: {
		PowerStatus s;
		// del not needed
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		s = read_intu16(stream, &error);
		if (! error) {
			mds->power_status = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_VAL_BATT_CHARGE: {
		intu16 s;
		// del not needed
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		s = read_intu16(stream, &error);
		if (! error) {
			mds->battery_level = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_TIME_BATT_REMAIN: {
		BatMeasure s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_batmeasure(stream, &s, &error);
		if (! error) {
			del_batmeasure(&mds->remaining_battery_time);
			mds->remaining_battery_time = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_REG_CERT_DATA_LIST: {
		RegCertDataList s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_regcertdatalist(stream, &s, &error);
		if (! error) {
			del_regcertdatalist(&mds->reg_cert_data_list);
			mds->reg_cert_data_list = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_SYS_TYPE_SPEC_LIST: {
		TypeVerList s;
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		decode_typeverlist(stream, &s, &error);
		if (! error) {
			del_typeverlist(&mds->system_type_spec_list);
			mds->system_type_spec_list = s;
		}
		free(stream);
	}
	break;
	case MDC_ATTR_CONFIRM_TIMEOUT: {
		RelativeTime s;
		// del not needed
		ByteStreamReader *stream = byte_stream_reader_instance(value, length);
		s = read_intu32(stream, &error);
		if (! error)
			mds->confirm_timeout = s;
		free(stream);
	}
	break;
	}
}

/**
 * Agents that have scanner derived objects shall support the
 * SET service for the Operational-State attribute of the scanner objects.
 *
 * \param ctx current context.
 * \param handle object handle.
 * \param state new scanner operational state
 * \param callback callback function of the request
 *
 * \return a pointer to a request just done.
 */
Request *mds_set_operational_state_of_the_scanner(Context *ctx, HANDLE handle, OperationalState state,
		service_request_callback callback)
{
	Request *req = NULL;

	if (ctx->mds != NULL) {
		struct MDS_object *mds_obj = mds_get_object_by_handle(ctx->mds,
					     handle);

		if (mds_obj != NULL && mds_obj->choice == MDS_OBJ_SCANNER) {

			if (mds_obj->u.scanner.choice == EPI_CFG_SCANNER) {
				req = epi_cfg_scanner_set_operational_state(
					      ctx,
					      &mds_obj->u.scanner.u.epi_cfg_scanner,
					      state, callback);
			} else {
				req = peri_cfg_scanner_set_operational_state(
					      ctx,
					      &mds_obj->u.scanner.u.peri_cfg_scanner,
					      state, callback);
			}
		}
	} else {
		ERROR("No MDS data is available");
	}

	return req;

}


/**
 * Finalizes and deallocate the current MDS instance.
 *
 * \param mds the mds to be destroyed.
 */
void mds_destroy(MDS *mds)
{
	if (mds != NULL) {
		int i;

		if (mds->objects_list != NULL) {
			for (i = 0; i < mds->objects_list_count; ++i) {
				if (mds->objects_list[i].choice == MDS_OBJ_PMSTORE) {
					pmstore_destroy(
						&(mds->objects_list[i].u.pmstore));
				} else if (mds->objects_list[i].choice
					   == MDS_OBJ_METRIC) {


					switch (mds->objects_list[i].u.metric.choice) {
					case METRIC_NUMERIC:
						numeric_destroy(&(mds->objects_list[i].u.metric.u.numeric));
						break;
					case METRIC_ENUM:
						enumeration_destroy(&(mds->objects_list[i].u.metric.u.enumeration));
						break;
					case METRIC_RTSA:
						rtsa_destroy(&(mds->objects_list[i].u.metric.u.rtsa));
						break;
					default:
						break;
					}

				} else if (mds->objects_list[i].choice == MDS_OBJ_SCANNER) {
					switch (mds->objects_list[i].u.scanner.choice) {
					case EPI_CFG_SCANNER:
						epi_cfg_scanner_destroy(&(mds->objects_list[i].u.scanner.u.epi_cfg_scanner));
						break;
					case PERI_CFG_SCANNER:
						peri_cfg_scanner_destroy(&(mds->objects_list[i].u.scanner.u.peri_cfg_scanner));
						break;
					default:
						break;
					}
				}
			}

			free(mds->objects_list);
			mds->objects_list = NULL;
		}

		del_octet_string(&mds->system_id);
		del_productionspec(&mds->production_specification);
		del_systemmodel(&mds->system_model);
		del_typeverlist(&mds->system_type_spec_list);
		del_regcertdatalist(&mds->reg_cert_data_list);
		del_attrvalmap(&mds->attribute_value_map);
		del_highresrelativetime(&mds->hires_relative_time);
		del_typeverlist(&mds->system_type_spec_list);


		free(mds);
		mds = NULL;
	}
}


/** @} */
