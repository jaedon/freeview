/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_spec.c
	@brief

	Description:  									\n
	Module: SAMA		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <hlib.h>
#include <vkernel.h>

#include <sama_lib.h>
#include "./local_include/sama_lib_int.h"

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

// TODO: Schedule module의 큰 변화가 예상될 경우에는 새로운 DxSchedule_Spec_e 를 정의하여 plug-in 구조처럼 분기하도록 해야한다.
// TODO: 절대로 base spec 안의 code에 config 들어가지 않도록 한다.
#if defined(CONFIG_PROD_HDR1000S)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		TRUE
#elif defined(CONFIG_PROD_IR4000HD)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_ICORDPRO)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	#define SAMA_SPEC_EXTRA_BOOT_TIME			2*60-30
#else
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
#endif
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_HDNEO)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	#define SAMA_SPEC_EXTRA_BOOT_TIME			2*60-30
#else
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
#endif
#elif defined(CONFIG_PROD_HGS1000S)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	#define SAMA_SPEC_EXTRA_BOOT_TIME			2*60-30
#else
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
#endif
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_HD9000I)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_HMS1000T)
	#define SAMA_SPEC							eDxSchedule_Spec_TvAnyTime
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_IHDR6100C)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_FVP4000T)
	#define SAMA_SPEC							eDxSchedule_Spec_TvAnyTime
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_HMX4KBBC)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_DIGIMAXT2)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_YSR2000)
	#define SAMA_SPEC							eDxSchedule_Spec_TvAnyTime
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_YS1000)
	#define SAMA_SPEC							eDxSchedule_Spec_TvAnyTime
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_CDIG2000C)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_HA7100S)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#elif defined(CONFIG_PROD_M1)
	#define SAMA_SPEC							eDxSchedule_Spec_Base
	#define SAMA_SPEC_EXTRA_BOOT_TIME			4*60
	#define SAMA_SPEC_ENABLE_TBR_PADDING		FALSE
#else
	#error ("Sama spec (in samalib_spec.c) should be defined! \n")
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___PRIVATE_FUNCTIONS_DECLARATION___
/*******************************************************************/
/********************      Private Functions Declaration    ********************/
/*******************************************************************/

#define ___PRIVATE_FUNCTIONS___
/*******************************************************************/
/********************      Private Functions    ********************/
/*******************************************************************/

#define ___MEMBER_FUNCTIONS___
/*******************************************************************/
/********************      Member Functions       ******************/
/*******************************************************************/
void SAMALIB_SEPC_CheckScheduleSpec(DxSchedule_t	*pstSchedule)
{
	if(pstSchedule == NULL)
	{
		HxLOG_Error("pstSchedule is NULL \n");
		return;
	}

	if(pstSchedule->eScheduleSpec < eDxSchedule_Spec_None || pstSchedule->eScheduleSpec >= eDxSchedule_Spec_EndOfCase)
	{
		HxLOG_Error("pstSchedule->eScheduleSpec: %d is not appropriate, change pstSchedule->eScheduleSpec to eDxSchedule_Spec_Base \n", pstSchedule->eScheduleSpec);
		pstSchedule->eScheduleSpec = eDxSchedule_Spec_Base;
	}
	else
	{
		pstSchedule->eScheduleSpec = SAMA_SPEC;
	}


	return;

}

DxSchedule_Spec_e SAMALIB_SPEC_Get (void)
{
	DxSchedule_Spec_e				eSpec = SAMA_SPEC;


	return eSpec;
}


HUINT32			SAMALIB_SPEC_GetExtraBootTime(void)
{
	HUINT32					ulExtraBootTime = SAMA_SPEC_EXTRA_BOOT_TIME;

	return ulExtraBootTime;
}

HBOOL			SAMALIB_SPEC_IsEnableTBRPadding(void)
{
	return SAMA_SPEC_ENABLE_TBR_PADDING;
}

