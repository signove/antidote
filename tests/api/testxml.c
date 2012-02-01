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
 * testxml.c
 *
 * Created on: Jun 16, 2010
 *     Author: mateus.lima
**********************************************************************/

#ifdef TEST_ENABLED

#include "Basic.h"
#include "src/util/strbuff.h"
#include "src/api/xml_encoder.h"
#include "tests/functional_test_cases/test_functional.h"
#include "testxml.h"
#include "src/util/log.h"

#include <stdlib.h>
#include <stdio.h>

int testxml_init_suite(void)
{
	return 0;
}

int testxml_finish_suite(void)
{
	return 0;
}

void testxml_add_suite()
{
	CU_pSuite suite = CU_add_suite("XML Test Suite",
				       testxml_init_suite, testxml_finish_suite);

	/* Add tests here - Start */
	CU_add_test(suite, "test_xml_1", test_xml_1);
	/* Add tests here - End */
}

void test_xml_1()
{
	char *a, *b;
	StringBuffer *sb;
	int i = 0;

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "abcde";
	b = "abcde";
	strbuff_cat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "abcde";
	b = "abcde";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "ab&de";
	b = "ab&amp;de";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "<bcde";
	b = "&lt;bcde";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "abcd>";
	b = "abcd&gt;";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "<>&";
	b = "&lt;&gt;&amp;";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "&";
	b = "&amp;";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "";
	b = "";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "&&&&&";
	b = "&amp;&amp;&amp;&amp;&amp;";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "<&>a";
	b = "&lt;&amp;&gt;a";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "&amp;";
	b = "&amp;amp;";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "&gt;";
	b = "&amp;gt;";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("%d", ++i);
	sb  = strbuff_new(1);
	a = "a<c>e";
	b = "a&lt;c&gt;e";
	strbuff_xcat(sb, a);
	CU_ASSERT_STRING_EQUAL(sb->str, b);
	strbuff_del(sb);

	DEBUG("test_xml_1");
}

#endif
