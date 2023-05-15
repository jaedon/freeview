/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_schedule.c
	@brief

	Description:  									\n
	Module: OBAMA / plugin / tvmanager / schedule	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#include <vkernel.h>
#include <hlib.h>
#include <sapi.h>
#include <octo_common.h>
#include <mgr_common.h>

#include <mgr_schedule.h>
#include <pmgr_schedule.h>
#include "./local_include/_xmgr_schedule.h"

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

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___

#define ___GLOBAL_FUNCTIONS___
HERROR pmgr_schedule_Init (void)
{
	SAPI_SpecOption_t	 stSpecOption;
	HERROR				 hErr;

	hErr = SAPI_GetSpecOptions (&stSpecOption);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAPI_GetSpecOptions err:\n");
		return ERR_FAIL;
	}

	switch (stSpecOption.eSpec)
	{
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		hErr = xmgr_schedule_Init_tva();
		break;
#endif
	case eDxSchedule_Spec_Base:
	default:
		hErr = xmgr_schedule_Init_Base();
		break;
	}

	return hErr;
}

HERROR pmgr_schedule_CheckFrontPannelDisplay (void)
{
	SAPI_SpecOption_t	 stSpecOption;
	HERROR				 hErr;

	hErr = SAPI_GetSpecOptions (&stSpecOption);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAPI_GetSpecOptions err:\n");
		return ERR_FAIL;
	}

	switch (stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = xmgr_schedule_CheckFrontPannelDisplay_Base();
		break;
	}

	return hErr;
}

HERROR pmgr_schedule_SetScheduleInCancelDB(HUINT32 nRequestID, DxSchedule_t *pstSchedule)
{
	SAPI_SpecOption_t	 stSpecOption;
	HERROR				 hErr;

	hErr = SAPI_GetSpecOptions (&stSpecOption);

	switch (stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		#if defined(CONFIG_OP_SES)
		hErr = xmgr_schedule_SetScheduleInCancelDB_Ses(nRequestID, pstSchedule);
		#else
		hErr = ERR_OK;
		#endif
		break;
	}

	return hErr;
}

HERROR			pmgr_schedule_StartDescRecordFinedItem(DxRecListData_t	*pstRecData)
{
	SAPI_SpecOption_t	 stSpecOption;
	HERROR				 hErr;

	hErr = SAPI_GetSpecOptions (&stSpecOption);

	switch (stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
	#if defined(CONFIG_MW_CI_PLUS)
		hErr = xmgr_schedule_CiPlusStartDescRecordFinedItem_Base(pstRecData);
	#else
		hErr = ERR_OK;
	#endif
		break;
	}

	return hErr;

}


HERROR			pmgr_schedule_RescheduleDescrambleRecordBatch(void)
{
	SAPI_SpecOption_t	 stSpecOption;
	HERROR				 hErr;

	hErr = SAPI_GetSpecOptions (&stSpecOption);

	switch (stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		#if defined(CONFIG_MW_CI_PLUS)
		hErr = xmgr_schedule_CiPlusRescheduleDescrambleRecordBatch_Base();
		#else
		hErr = ERR_OK;
		#endif
		break;
	}

	return hErr;
}

HERROR pmgr_schedule_ReScheduletBurstUpdate(void)
{
	HERROR	hErr = ERR_OK;
	SAPI_SpecOption_t	 stSpecOption;

	hErr = SAPI_GetSpecOptions (&stSpecOption);

	switch (stSpecOption.eSpec)
	{
		case eDxSchedule_Spec_Base:
		default:
#if defined (CONFIG_OP_SES)
			hErr = xmgr_schedule_RescheduleBurstUpdate_ses();
#endif
			break;
	}


	return hErr;
}


HERROR pmgr_schedule_CheckConflictSchedule(void)
{
	HERROR	hErr = ERR_OK;
	SAPI_SpecOption_t	 stSpecOption;

	hErr = SAPI_GetSpecOptions (&stSpecOption);

	switch (stSpecOption.eSpec)
	{
		case eDxSchedule_Spec_TvAnyTime:
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME)
			hErr = xmgr_schedule_CheckConflictSchedule_tva();
#endif
			break;
		case eDxSchedule_Spec_Base:
		default:
#if defined (CONFIG_OP_SES)
			hErr = xmgr_schedule_CheckConflictSchedule_ses();
#endif
			break;
	}


	return hErr;
}

Handle_t pmgr_schedule_GetEarliestTrackingSvc (void)
{
	Handle_t	ret = HANDLE_NULL;
	SAPI_SpecOption_t	 stSpecOption;

	(void)SAPI_GetSpecOptions (&stSpecOption);

	switch (stSpecOption.eSpec)
	{
		case eDxSchedule_Spec_TvAnyTime:
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME)
			ret = xmgr_schedule_GetEarliestTrackingSvc_tva();
#endif
			break;
		default:
			break;
	}

	return ret;
}

HERROR	pmgr_schedule_MakeRegularOta (void)
{
	HERROR	hErr = ERR_OK;
	SAPI_SpecOption_t	 stSpecOption;

	(void)SAPI_GetSpecOptions (&stSpecOption);

	switch (stSpecOption.eSpec)
	{
		case eDxSchedule_Spec_TvAnyTime:
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME)
			hErr = xmgr_schedule_MakeRegularOta_tva();
#endif
			break;
		default:
			break;
	}

	return hErr;
}

HERROR	pmgr_schedule_SetRecordFailReason (HUINT32 ulSlot, DxSchedule_RemovedReason_e eRemovedReason, HUINT32 eDetailedInfo)
{
	HERROR	hErr = ERR_OK;
	SAPI_SpecOption_t	 stSpecOption;

	(void)SAPI_GetSpecOptions (&stSpecOption);

	switch (stSpecOption.eSpec)
	{
		case eDxSchedule_Spec_Base:
		default:
			hErr = xmgr_schedule_SetRecordFailReason_Base(ulSlot, eRemovedReason, eDetailedInfo);
			break;
	}

	return hErr;
}


#define _____DEBUG_FUNCTION_____
#if defined(CONFIG_DEBUG)
HINT32 pmgr_schedule_base_get(void *szArgList)
{
	return xmgr_schedule_base_get (szArgList);
}
HINT32 pmgr_schedule_base_start(void *szArgList)
{
	return xmgr_schedule_base_start (szArgList);
}
HINT32 pmgr_schedule_base_stop(void *szArgList)
{
	return xmgr_schedule_base_stop (szArgList);
}
HINT32 pmgr_schedule_base_startMediaServer(void *szArgList)
{
	return xmgr_schedule_base_startMediaServer (szArgList);
}
HINT32 pmgr_schedule_base_makeTBRSchedule(void *szArgList)
{
	return xmgr_schedule_base_makeTBRSchedule (szArgList);
}
#endif
