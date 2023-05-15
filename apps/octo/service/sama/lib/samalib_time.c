/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_time.c
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
HERROR SAMALIB_TIME_GetPrevTime (DxSchedule_t *pstSchedule, UNIXTIME utSrcTime, UNIXTIME *putPrevTime)
{
	DxSchedule_Spec_e				eSpec = SAMALIB_SPEC_Get();
	HERROR					 hErr;

	if (NULL == pstSchedule)				{ return ERR_FAIL; }

	switch (eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_time_base_GetPrevTime (pstSchedule, utSrcTime, putPrevTime);
		break;
	}

	return hErr;
}

HERROR SAMALIB_TIME_GetNextTime (DxSchedule_t *pstSchedule, UNIXTIME utSrcTime, UNIXTIME *putNextTime)
{
	DxSchedule_Spec_e				eSpec = SAMALIB_SPEC_Get();
	HERROR					 hErr;

	if (NULL == pstSchedule)				{ return ERR_FAIL; }

	switch (eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_time_base_GetNextTime (pstSchedule, utSrcTime, putNextTime);
		break;
	}

	return hErr;
}

/*
	RepeatedSchedule 일 경우, BaseTime에 가장 근접한 미래의 Start Time을 알려주는 함수:
	Repeated가 아닐 경우엔 원래 기록된 StartTime을 가르켜 준다.
	단, StartTime ~ Duration 상에 BaseTime이 걸쳐 있을 경우에는 그 걸쳐 있는 시간을 가르켜 주므로 BaseTime보다 작은 StartTime을 가르켜 주게 된다.
*/
HERROR SAMALIB_TIME_GetRepeatedScheduleStartTime (DxSchedule_t *pstSchedule, UNIXTIME utBaseTime, UNIXTIME *putStartTime)
{
	DxSchedule_Spec_e				eSpec = SAMALIB_SPEC_Get();
	HERROR					 hErr;

	if (NULL == pstSchedule)				{ return ERR_FAIL; }

	switch (eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_time_base_GetRepeatedScheduleStartTime (pstSchedule, utBaseTime, putStartTime);
		break;
	}

	return hErr;
}

HERROR SAMALIB_TIME_AdjustScheduleTime (DxSchedule_t *pstSchedule, UNIXTIME utBaseTime, HBOOL *pbUpdated)
{
	DxSchedule_Spec_e				eSpec = SAMALIB_SPEC_Get();
	HERROR					 hErr;

	if (NULL == pstSchedule)				{ return ERR_FAIL; }

	switch (eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_time_base_AdjustScheduleTime (pstSchedule, utBaseTime, pbUpdated);
		break;
	}

	return hErr;
}

HERROR SAMALIB_TIME_GetPaddingStartEndTime (DxSchedule_t *pstSchedule, UNIXTIME utStartTime, UNIXTIME *putStartTime, UNIXTIME *putEndTime)
{
	DxSchedule_Spec_e				eSpec = SAMALIB_SPEC_Get();
	HERROR					 hErr;

	if (NULL == pstSchedule)				{ return ERR_FAIL; }

	switch (eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_time_base_GetPaddingStartEndTime (pstSchedule, utStartTime, putStartTime, putEndTime);
		break;
	}

	return hErr;
}


