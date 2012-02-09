/**********************************************************************
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
 * test_association.c
 *
 * Created on: Jul 9, 2010
 *     Author: fabricio.silva
 **********************************************************************/
#ifdef TEST_ENABLED

#include <Basic.h>
#include <stdlib.h>

#include "test_functional.h"
#include "src/manager_p.h"
#include "src/util/bytelib.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/communication/communication.h"
#include "src/communication/context_manager.h"
#include "src/communication/extconfigurations.h"
#include "src/util/ioutil.h"
#include "src/util/log.h"
#include "src/communication/plugin/plugin_fifo.h"
#include "src/communication/plugin/plugin_pthread.h"
#include <unistd.h>

CommunicationPlugin FUNCTIONAL_TEST_COMM_PLUGIN;


int functional_test_init()
{
	FUNCTIONAL_TEST_COMM_PLUGIN = communication_plugin();
	plugin_network_fifo_setup(&FUNCTIONAL_TEST_COMM_PLUGIN, FUNC_TEST_SINGLE_CONTEXT, 0);
	plugin_pthread_setup(&FUNCTIONAL_TEST_COMM_PLUGIN);
	CommunicationPlugin *plugins[] = {&FUNCTIONAL_TEST_COMM_PLUGIN, 0};
	manager_init(plugins);
	return 0;
}

int functional_test_finish()
{
	manager_finalize();
	ext_configurations_remove_all_configs();
	return 0;
}

void func_simulate_incoming_apdu(const char *apdu_file_path)
{
	int error = 0;
	unsigned long buffer_size = 0;

	unsigned char *buffer =
		ioutil_buffer_from_file(apdu_file_path, &buffer_size);
	ByteStreamReader *stream = byte_stream_reader_instance(buffer, buffer_size);

	APDU *apdu = (APDU *) malloc(sizeof(APDU));

	if (stream != NULL && apdu != NULL) {
		decode_apdu(stream, apdu, &error);
		communication_process_apdu(context_get(FUNC_TEST_SINGLE_CONTEXT), apdu);
		del_apdu(apdu);
	}

	free(apdu);
	free(stream);
	free(buffer);
	buffer = NULL;
}

/**
 * Simulate response to this request. The request invoke-id will override the
 * invoke-id of apdu file
 *
 * @param response_apdu_file
 * @param request
 */
void func_simulate_service_response(const char *response_apdu_file,
				    Request *request)
{
	int error = 0;
	unsigned long buffer_size = 0;

	unsigned char *buffer =
		ioutil_buffer_from_file(response_apdu_file, &buffer_size);
	ByteStreamReader *stream = byte_stream_reader_instance(buffer, buffer_size);

	APDU *apdu = (APDU *) calloc(1, sizeof(APDU));

	Context *ctx = func_ctx();

	if (stream != NULL && apdu != NULL) {
		decode_apdu(stream, apdu, &error);

		if (apdu->choice == PRST_CHOSEN) {
			DATA_apdu *data_apdu = encode_get_data_apdu(&apdu->u.prst);
			data_apdu->invoke_id = service_get_invoke_id(ctx, request);
		}

		communication_process_apdu(ctx, apdu);
		del_apdu(apdu);
	}

	free(apdu);
	free(stream);
	free(buffer);
	buffer = NULL;
}

/**
 * Return connection context of functional tests
 */
Context *func_ctx()
{
	return context_get(FUNC_TEST_SINGLE_CONTEXT);
}

/**
 * Return current mds in functional test connection context
 */
MDS *func_mds()
{
	return func_ctx()->mds;
}

#endif
