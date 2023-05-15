/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_schedule_ses.c
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
#include <hlib.h>
#include <mgr_action.h>
#include <mgr_swup.h>
#include "../local_include/_xmgr_schedule.h"

#include <sapi.h>
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
STATIC HUINT64						dbIndex = 0;

HERROR xmgr_schedule_SetScheduleInCancelDB_Ses(HUINT32 nRequestID, DxSchedule_t *pstSchedule)
{
	HUINT32 		ulCurrentPid =	HLIB_STD_GetPID();

	if(pstSchedule != NULL)
	{
		if((ulCurrentPid != nRequestID) && (pstSchedule->eRsvType == DxRSVTYPE_RECORD_SATRECORD_TBR))
		{
			DAPI_Set("DxDB_SatRecord", dbIndex, (void*)pstSchedule, sizeof(DxSchedule_t));
			dbIndex++;
		}
	}

	if(dbIndex >= 0XFFFF)
		dbIndex = 0;

	return ERR_OK;
}

HBOOL xmgr_schedule_CheckExistScheduleIn30min_ses(void)
{
	UNIXTIME			ulCurTime;
	HUINT32				ulNumSch = 0;
	DxSchedule_t		*pstSchArray = NULL;

	DxSchedule_FindFilterAttr_t stFilter;

	ulCurTime = HLIB_STD_GetSystemTime();

	SAPI_FindScheduleFilterInit(&stFilter);
	stFilter.stOverlapTimeRange.utOverlapTimeRangeStart = ulCurTime;
	stFilter.stOverlapTimeRange.utOverlapTimeRangeEnd = ulCurTime + 30*60;

	SAPI_FindScheduleUsingFilter(&stFilter, &ulNumSch, &pstSchArray);
	SAPI_FindScheduleUsingFilterFreeList(pstSchArray);

	if (ulNumSch > 0)	return TRUE;

	return FALSE;
}


HERROR xmgr_schedule_RescheduleBurstUpdate_ses(void)
{
	HERROR		hErr;
	UNIXTIME	ulstartTime;
	Handle_t 			hSvc = HANDLE_NULL;
	HxDATETIME_t		stStartTime;
	DxSchedule_t		stSchedule = {0, };
	SAPI_Conflict_t		stConflict;

	//Get Next schedule info
	hErr = MGR_SWUP_GetNextBurstOtaSchedule(&ulstartTime);

	if(hErr == ERR_OK)
	{
		VK_memset(&stSchedule, 0, sizeof(DxSchedule_t) );

		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulstartTime, &stStartTime);
		HxSTD_MemCopy(&stSchedule.stStartTime, &stStartTime, sizeof(HxDATETIME_t));

		hErr = SAPI_GetAvailableSlot(&stSchedule.ulSlot);
		if (hErr != ERR_OK)
		{
			HxLOG_Error(" There is no empty slot for sama!\n");
			return ERR_FAIL;
		}

		HxLOG_Debug("Available Slot ... (%d)\n", stSchedule.ulSlot);

		stSchedule.eRsvType = DxRSVTYPE_REGULAR_OTA;

		stSchedule.ulSvcUid 		= DB_SVC_GetServiceUID(hSvc);		// 깨어날때 처리하자.
		stSchedule.eRsvReady		= DxRSVREADY_30_SEC;
		stSchedule.ulDuration		= 0;
		stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
		stSchedule.eEpgType 		= eDxEPG_TYPE_NONE;

		HxLOG_Debug("on Solt: %d,  SVC UID: 0x%x\n", stSchedule.ulSlot, stSchedule.ulSvcUid);

		hErr = SAPI_MakeSchedule (stSchedule.ulSlot, &stSchedule, &stConflict);

		if(hErr != ERR_OK)
		{
			HxLOG_Error("Error: Fail making new schedule\n");
		}
	}

	return hErr;
}

HERROR xmgr_schedule_CheckConflictSchedule_ses(void)
{
	HERROR hErr = ERR_OK;

	if(xmgr_schedule_CheckExistScheduleIn30min_ses() == TRUE || MGR_ACTION_GetRecordCount() > 0)
	{
		hErr = xmgr_schedule_RescheduleBurstUpdate_ses();

		if(hErr != ERR_OK)
		{
			HxLOG_Error(" Fail : Reschedule for burst update\n");
		}

	}

	return ERR_OK;
}
