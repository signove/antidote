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
 * testextconfiguration.c
 *
 * Created on: Jul 15, 2010
 *     Author: maemo
**********************************************************************/
//#define TEST_ENABLED
#ifdef TEST_ENABLED

#include "src/util/bytelib.h"
#include "src/communication/parser/decoder_ASN1.h"
#include "src/communication/parser/encoder_ASN1.h"
#include "src/communication/parser/struct_cleaner.h"
#include "src/communication/stdconfigurations.h"
#include "src/specializations/blood_pressure_monitor.h"

#include "src/specializations/pulse_oximeter.h"
#include "src/specializations/glucometer.h"
#include "Basic.h"
#include "src/communication/extconfigurations.h"
#include "testextconfiguration.h"
#include <stdlib.h>
#include <stdio.h>

int test_ext_configuration_init_suite(void)
{
	return 0;
}

int test_ext_configuration_finish_suite(void)
{
	return  0;
}

void testextconfiguration_add_suite()
{
	CU_pSuite suite = CU_add_suite("ExtConfiguration Test Suite", test_ext_configuration_init_suite,
				       test_ext_configuration_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_extconfiguration_persistent_config", test_extconfiguration_persistent_config);

	/* Add tests here - End */
}


void test_extconfiguration_persistent_config()
{
	// Test Data
	struct StdConfiguration *bp_std_config = blood_pressure_monitor_create_std_config_ID02BC();
	struct StdConfiguration *po_std_config = pulse_oximeter_create_std_config_ID0190();
	struct StdConfiguration *glu_std_config = glucometer_create_std_config_ID06A4();

	intu8 sys_id_one_buffer[] = {0x00, 0x22, 0x09, 0x22, 0x58, 0x08, 0x03, 0xcc};
	intu8 sys_id_two_buffer[] = {0x00, 0x22, 0x09, 0x22, 0x58, 0x07, 0xe8, 0x6b};
	intu8 sys_id_three_buffer[] = {0x00, 0x22, 0x09, 0x22, 0x58, 0x07, 0xe8, 0x77};

	octet_string sys_id_one;
	sys_id_one.length = 8;
	sys_id_one.value = sys_id_one_buffer;

	octet_string sys_id_two;
	sys_id_two.length = 8;
	sys_id_two.value = sys_id_two_buffer;

	octet_string sys_id_three;
	sys_id_three.length = 8;
	sys_id_three.value = sys_id_three_buffer;

	ConfigObjectList *bp_object_list = bp_std_config->configure_action();
	ConfigObjectList *po_object_list = po_std_config->configure_action();
	ConfigObjectList *glu_object_list = glu_std_config->configure_action();

	ConfigObjectList empty_object_list;
	empty_object_list.count = 0;
	empty_object_list.length = 0;

	// Remove previous execution
	ext_configurations_remove_all_configs();

	ext_configurations_load_configurations();

	// Empty list
	ConfigObjectList *result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x02BC);
	CU_ASSERT(result == NULL);

	result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x0190);
	CU_ASSERT(result == NULL);

	result = ext_configurations_get_configuration_attributes(&sys_id_two, 0x1010);
	CU_ASSERT(result == NULL);

	result = ext_configurations_get_configuration_attributes(&sys_id_three, 0x06A4);
	CU_ASSERT(result == NULL);

	// Adds configurations
	ext_configurations_register_conf(&sys_id_one, 0x02BC, bp_object_list);
	ext_configurations_register_conf(&sys_id_one, 0x0190, po_object_list);
	ext_configurations_register_conf(&sys_id_one, 0x1010, &empty_object_list);

	ext_configurations_register_conf(&sys_id_two, 0x02BC, po_object_list);
	ext_configurations_register_conf(&sys_id_two, 0x0190, &empty_object_list);
	ext_configurations_register_conf(&sys_id_two, 0x1010, bp_object_list);

	ext_configurations_register_conf(&sys_id_three, 0x06A4, glu_object_list);

	// Check configurations (one)
	result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x02BC);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == bp_object_list->count);
	CU_ASSERT(result->length == bp_object_list->length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x0190);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == po_object_list->count);
	CU_ASSERT(result->length == po_object_list->length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x1010);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == empty_object_list.count);
	CU_ASSERT(result->length == empty_object_list.length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	// Check configurations (two)
	result = ext_configurations_get_configuration_attributes(&sys_id_two, 0x02BC);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == po_object_list->count);
	CU_ASSERT(result->length == po_object_list->length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	result = ext_configurations_get_configuration_attributes(&sys_id_two, 0x0190);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == empty_object_list.count);
	CU_ASSERT(result->length == empty_object_list.length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	result = ext_configurations_get_configuration_attributes(&sys_id_two, 0x1010);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == bp_object_list->count);
	CU_ASSERT(result->length == bp_object_list->length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	// Check configurations (three)
	result = ext_configurations_get_configuration_attributes(&sys_id_three, 0x06A4);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == glu_object_list->count);
	CU_ASSERT(result->length == glu_object_list->length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	// Destroy
	ext_configurations_destroy();

	// Empty list (one)
	result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x02BC);
	CU_ASSERT(result == NULL);

	result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x0190);
	CU_ASSERT(result == NULL);

	result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x1010);
	CU_ASSERT(result == NULL);

	// Empty list (two)
	result = ext_configurations_get_configuration_attributes(&sys_id_two, 0x02BC);
	CU_ASSERT(result == NULL);

	result = ext_configurations_get_configuration_attributes(&sys_id_two, 0x0190);
	CU_ASSERT(result == NULL);

	result = ext_configurations_get_configuration_attributes(&sys_id_two, 0x1010);
	CU_ASSERT(result == NULL);

	// Empty list (three)
	result = ext_configurations_get_configuration_attributes(&sys_id_three, 0x06A4);
	CU_ASSERT(result == NULL);

	// Load
	ext_configurations_load_configurations();

	// Check configurations (one)
	result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x02BC);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == bp_object_list->count);
	CU_ASSERT(result->length == bp_object_list->length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x0190);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == po_object_list->count);
	CU_ASSERT(result->length == po_object_list->length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	result = ext_configurations_get_configuration_attributes(&sys_id_one, 0x1010);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == empty_object_list.count);
	CU_ASSERT(result->length == empty_object_list.length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	// Check configurations (two)
	result = ext_configurations_get_configuration_attributes(&sys_id_two, 0x02BC);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == po_object_list->count);
	CU_ASSERT(result->length == po_object_list->length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	result = ext_configurations_get_configuration_attributes(&sys_id_two, 0x0190);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == empty_object_list.count);
	CU_ASSERT(result->length == empty_object_list.length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	result = ext_configurations_get_configuration_attributes(&sys_id_two, 0x1010);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == bp_object_list->count);
	CU_ASSERT(result->length == bp_object_list->length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	// Check configurations (three)
	result = ext_configurations_get_configuration_attributes(&sys_id_three, 0x06A4);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->count == glu_object_list->count);
	CU_ASSERT(result->length == glu_object_list->length);
	del_configobjectlist(result);
	free(result);
	result = NULL;

	// free

	ext_configurations_destroy();

	free(bp_std_config);
	free(po_std_config);
	free(glu_std_config);
	del_configobjectlist(bp_object_list);
	del_configobjectlist(po_object_list);
	del_configobjectlist(glu_object_list);
	free(bp_object_list);
	free(po_object_list);
	free(glu_object_list);

}
#endif
