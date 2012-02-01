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
 * main_test_suite.c
 *
 * Created on: Jun 11, 2010
 *     Author: fabricio.silva
 **********************************************************************/
#ifdef TEST_ENABLED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include <Basic.h>
#include <Console.h>
#include <Automated.h>

#include "testtimer.h"
#include "testlinkedlist.h"
#include "communication/parser/testparser.h"
#include "communication/parser/testbytelib.h"
#include "communication/encoder/testencoder.h"
#include "api/testxml.h"
#include "communication/testcontextmanager.h"
#include "communication/testfsm.h"
#include "communication/testservice.h"
#include "communication/testextconfiguration.h"
#include "dim/testpmstore.h"
#include "dim/testpmsegment.h"
#include "dim/testdateutil.h"
#include "dim/testdim.h"
#include "dim/testmds.h"
#include "dim/testioutil.h"
#include "functional_test_cases/test_association.h"
#include "functional_test_cases/test_operating.h"
#include "functional_test_cases/test_configuring.h"
#include "functional_test_cases/test_disassociating.h"
#include "functional_test_cases/test_epicfgscanner.h"
#include "functional_test_cases/test_pericfgscanner.h"
#include "src/api/api_definitions.h"
#include "src/api/data_encoder.h"
#include "src/api/xml_encoder.h"
#include "src/asn1/phd_types.h"
#include "src/communication/communication.h"
#include "src/communication/service.h"
#include "src/communication/plugin/plugin_fifo.h"
#include "src/manager_p.h"
#include "src/util/strbuff.h"
#include "src/util/ioutil.h"



void add_test_suites()
{
	/* Add All test suites here - Start */

	// Unit tests
	testxml_add_suite();
	testdim_add_suite();
	testmds_add_suite();
	testpmstore_add_suite();
	testpmsegment_add_suite();
	testbytelib_add_suite();
	testparser_add_suite();
	testencoder_add_suite();
	testdateutil_add_suite();
	testioutil_add_suite();
	testfsm_add_suite();
	testservice_add_suite();
	testtimer_add_suite();
	testextconfiguration_add_suite();
	testctxmanager_add_suite();
	testllist_add_suite();

	// Functional tests
	functionaltest_association_add_suite();
	functionaltest_operating_add_suite();
	functionaltest_configuring_add_suite();
	functionaltest_disassociating_add_suite();
	functionaltest_epicfgscanner_add_suite();
	functionaltest_pericfgscanner_add_suite();

	/* Add All test suites here - End */
}

void console_mode()
{
	/* Run all tests using the console interface */
	printf("-- Console Interactive mode --\n\n");
	CU_console_run_tests();
}

void auto_mode()
{
	/* Run all tests using the automated interface */
	printf("-- Automatic Test mode --\n\n");
	CU_automated_run_tests();
	// Store tests to file
	CU_list_tests_to_file();
	printf("Test results are stored in file system\n");
}

void text_mode()
{
	/* Run all tests using the basic interface */
	printf("-- Text mode --\n\n");
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	printf("\n");
	CU_basic_show_failures(CU_get_failure_list());
	printf("\n\n");
}

int main(int argc, char *argv[])
{
	printf("Starting main_test_suite with all test cases\n");
	printf("Available program args: console, text, auto.\n\n");

	if (CU_initialize_registry() == CUE_SUCCESS) {
		printf("Test environment initialized\n");
		add_test_suites();
	} else {
		printf("Fail to initialize test environment\n");
		return CU_get_error();
	}

	if (argc > 1) {
		while (argc--) {
			if (strcmp(argv[argc], "console") == 0) {
				console_mode();
			} else if (strcmp(argv[argc], "auto") == 0) {
				auto_mode();
			} else if (strcmp(argv[argc], "text") == 0) {
				text_mode();
			}
		}
	} else {
		// Default test mode;
		text_mode();
	}

	/* Clean up registry and return */

	CU_cleanup_registry();
	pthread_exit((void *)CU_get_error());
	return CU_get_error();
}
#endif

#ifndef TEST_ENABLED
#include <stdio.h>
int main()
{
	printf("-- No test framework enabled, please install CUnit and compile the test suite again --\n");
	return 0;
}
#endif

