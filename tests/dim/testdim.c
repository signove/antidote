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
 * test_dim.c
 *
 * Created on: Jun 11, 2010
 *     Author: fabricio.silva
 **********************************************************************/
#ifdef TEST_ENABLED

#include "Basic.h"
#include "src/asn1/phd_types.h"
#include "src/dim/nomenclature.h"
#include "src/dim/mds.h"
#include "src/dim/scanner.h"
#include "src/dim/peri_cfg_scanner.h"
#include "src/dim/epi_cfg_scanner.h"
#include "src/dim/cfg_scanner.h"
#include "src/dim/metric.h"
#include "src/dim/numeric.h"
#include "src/dim/rtsa.h"
#include "src/dim/pmstore.h"
#include "src/dim/pmsegment.h"
#include "src/dim/enumeration.h"
#include "src/dim/rtsa.h"
#include "testdim.h"

#include <stdlib.h>

int test_dim_init_suite(void)
{
	return 0;
}

int test_dim_finish_suite(void)
{
	return 0;
}

void testdim_add_suite()
{
	CU_pSuite suite = CU_add_suite("DIM Test Suite", test_dim_init_suite,
				       test_dim_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_dim_mds_initialization", test_dim_mds_initialization);

	CU_add_test(suite, "test_dim_scanner_initialization",
		    test_dim_scanner_initialization);

	CU_add_test(suite, "test_dim_cfg_scanner_initialization",
		    test_dim_cfg_scanner_initialization);

	CU_add_test(suite, "test_dim_peri_cfg_scanner_initialization",
		    test_dim_peri_cfg_scanner_initialization);

	CU_add_test(suite, "test_dim_epi_cfg_scanner_initialization",
		    test_dim_epi_cfg_scanner_initialization);

	CU_add_test(suite, "test_dim_metric_initialization",
		    test_dim_metric_initialization);


	/* Add tests here - End */

}

void test_dim_mds_initialization(void)
{
	MDS *mds = mds_create();

	CU_ASSERT_PTR_NOT_NULL(mds);
	CU_ASSERT_EQUAL(mds->dim.id, MDC_MOC_VMS_MDS_SIMP);

	mds_destroy(mds);
}

void test_dim_scanner_initialization(void)
{
	HANDLE handle = 0;
	OperationalState state = 0;
	struct Scanner *sc = scanner_instance(handle, state);

	CU_ASSERT_PTR_NOT_NULL(sc);
	CU_ASSERT_EQUAL(sc->dim.id, MDC_MOC_SCAN);
	free(sc);
}

void test_dim_cfg_scanner_initialization(void)
{
	HANDLE handle = 0;
	OperationalState state = 0;
	struct Scanner *sc = scanner_instance(handle, state);

	ConfirmMode mode = 0;
	struct CfgScanner *cfg = cfg_scanner_instance(sc, mode);

	CU_ASSERT_PTR_NOT_NULL(cfg);
	CU_ASSERT_EQUAL(cfg->dim.id, MDC_MOC_SCAN_CFG);

	free(cfg);
	free(sc);
}
void test_dim_peri_cfg_scanner_initialization(void)
{

	HANDLE handle = 0;
	OperationalState state = 0;
	struct Scanner *sc = scanner_instance(handle, state);

	ConfirmMode mode = 0;
	struct CfgScanner *cfg_scanner = cfg_scanner_instance(sc, mode);


	struct PeriCfgScanner *peri = peri_cfg_scanner_instance(cfg_scanner);

	CU_ASSERT_PTR_NOT_NULL(peri);
	CU_ASSERT_EQUAL(peri->dim.id, MDC_MOC_SCAN_CFG_PERI);

	free(peri);
	free(cfg_scanner);
	free(sc);
}

void test_dim_epi_cfg_scanner_initialization(void)
{
	HANDLE handle = 0;
	OperationalState state = 0;
	struct Scanner *sc = scanner_instance(handle, state);

	ConfirmMode mode = 0;
	struct CfgScanner *cfg_scanner = cfg_scanner_instance(sc, mode);

	struct EpiCfgScanner *epi = epi_cfg_scanner_instance(cfg_scanner);

	CU_ASSERT_PTR_NOT_NULL(epi);
	CU_ASSERT_EQUAL(epi->dim.id, MDC_MOC_SCAN_CFG_EPI);
	free(epi);
	free(cfg_scanner);
	free(sc);
}

void test_dim_metric_initialization(void)
{

	HANDLE handle = 0;
	MetricSpecSmall small = 0;
	TYPE type;

	struct Metric *metric = metric_instance(handle, type, small);

	CU_ASSERT_PTR_NOT_NULL(metric);
	CU_ASSERT_EQUAL(metric->dim.id, MDC_MOC_VMO_METRIC);

	struct Numeric *num = numeric_instance(metric);

	CU_ASSERT_PTR_NOT_NULL(num);
	CU_ASSERT_EQUAL(num->dim.id, MDC_MOC_VMO_METRIC_NU);
	free(num);

	struct Enumeration *enum_ = enumeration_instance(metric);

	CU_ASSERT_PTR_NOT_NULL(enum_);
	CU_ASSERT_EQUAL(enum_->dim.id, MDC_MOC_VMO_METRIC_ENUM);
	free(enum_);

	RelativeTime sample = 0;
	octet_string simple;
	ScaleRangeSpec8 scale8;
	ScaleRangeSpec16 scale16;
	ScaleRangeSpec32 scale32;
	SaSpec sa;

	struct RTSA *rtsa8 = rtsa_instance_spec8(metric, sample, simple, scale8, sa);
	CU_ASSERT_PTR_NOT_NULL(rtsa8);
	CU_ASSERT_EQUAL(rtsa8->dim.id, MDC_MOC_VMO_METRIC_SA_RT);
	free(rtsa8);

	struct RTSA *rtsa16 = rtsa_instance_spec16(metric, sample, simple, scale16, sa);
	CU_ASSERT_PTR_NOT_NULL(rtsa16);
	CU_ASSERT_EQUAL(rtsa16->dim.id, MDC_MOC_VMO_METRIC_SA_RT);
	free(rtsa16);

	struct RTSA *rtsa32 = rtsa_instance_spec32(metric, sample, simple, scale32, sa);
	CU_ASSERT_PTR_NOT_NULL(rtsa32);
	CU_ASSERT_EQUAL(rtsa32->dim.id, MDC_MOC_VMO_METRIC_SA_RT);
	free(rtsa32);

	free(metric);
}

#endif
