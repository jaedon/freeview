/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  re_init.h
	@brief

	Description:  									\n
	Module: HOMMA / API								\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/



#ifndef	__TEST_CUNIT_API_H__
#define	__TEST_CUNIT_API_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "CUnit/Basic.h"
#include "CUnit/Console.h"
#include "CUnit/Automated.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
//#define FULL_TEST
/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define TCU_add_test(pTest, pSuite, fName) \
	pTest = CU_add_test(pSuite, #fName , fName); \
	if (NULL == pTest) \
		return -1;


#define TCU_ASSERT_EQUAL(actual, expected) \
  CU_assertImplementation(((actual) == (expected)), __LINE__, ("CU_ASSERT_EQUAL(" #actual "," #expected ")"), __FILE__, "", CU_FALSE)


#define TCU_ASSERT_NOT_EQUAL(actual, expected) \
  CU_assertImplementation(((actual) != (expected)), __LINE__, ("CU_ASSERT_NOT_EQUAL(" #actual "," #expected ")"), __FILE__, "", CU_FALSE)

#define TCU_ASSERT_NOT_EQUAL_FATAL(actual, expected) \
  CU_assertImplementation(((actual) != (expected)), __LINE__, ("CU_ASSERT_NOT_EQUAL_FATAL(" #actual "," #expected ")"), __FILE__, "", CU_TRUE)

#define TCU_ASSERT_STRING_EQUAL(actual, expected) \
   CU_assertImplementation(!(strcmp((const char*)(actual), (const char*)(expected))), __LINE__, ("CU_ASSERT_STRING_EQUAL(" #actual ","  #expected ")"), __FILE__, "", CU_FALSE)

#define TCU_ASSERT_STRING_INCLUDE(actual, expected) \
   CU_assertImplementation((strstr((const char*)(actual), (const char*)(expected)) != NULL), __LINE__, ("CU_ASSERT_STRING_INCLUDE(" #actual ","  #expected ")"), __FILE__, "", CU_FALSE)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

static inline void TCU_printf(const char *func, int line,  const char *fmt, ...)
{
	va_list ap;
	printf("\n [%s]@%d : ", func, line);
	
	va_start(ap, fmt);
	vprintf(fmt, ap );
	va_end(ap);
	printf("\n");
}

#define TCU_MSG(...) TCU_printf(__FUNCTION__, __LINE__, __VA_ARGS__);
/*******************************************************************/
/********************      extern Functions     ********************/
/*******************************************************************/
#endif
