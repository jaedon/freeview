/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ??2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/
// ?œê?
#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>

#include <hapi.h>
#include <rema_int.h>
#include "CUnit/Basic.h"
#include "test_CUnit_Api.h"

#include "hCwmpApi.h"
#include "rema_cwmp.h"
#include "rema_DM.h"


#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___

#define ___STATIC_VARIABLES___

#define ___INTERNAL_FUNCTIONS___
static void test_check_NULL_1(void)
{
	rema_DM_Reister(NULL);
}
static void test_rema_cwmp_GetIndex(void)
{
	HUINT32 unIndex=-1; 

	TCU_ASSERT_EQUAL(rema_cwmp_GetIndex(NULL, NULL), -1);

	TCU_ASSERT_EQUAL(rema_cwmp_GetIndex(NULL, &unIndex), -1);
	TCU_ASSERT_EQUAL(rema_cwmp_GetIndex("d", NULL), -1);

	TCU_ASSERT_EQUAL(rema_cwmp_GetIndex("ddd", &unIndex), -1);
	TCU_ASSERT_NOT_EQUAL(rema_cwmp_GetIndex("33", &unIndex), -1);
	TCU_ASSERT_EQUAL(unIndex, 33);

	TCU_ASSERT_NOT_EQUAL(rema_cwmp_GetIndex("33.55", &unIndex), -1);
	TCU_ASSERT_EQUAL(unIndex, 33);
}

#define ___PUBLIC_FUNCTIONS___

int test_cwmp_register(void)
{
	int nRS = -1;

	CU_pSuite pSuite = NULL;
	CU_pTest  pTest = NULL;
	pSuite = CU_add_suite("test_cwmp_Check_NULL", NULL, NULL);
	if (NULL == pSuite) 
		return nRS;
	TCU_add_test(pTest, pSuite, test_check_NULL_1);

	TCU_add_test(pTest, pSuite, test_rema_cwmp_GetIndex);
	
	return 0;
}
#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

