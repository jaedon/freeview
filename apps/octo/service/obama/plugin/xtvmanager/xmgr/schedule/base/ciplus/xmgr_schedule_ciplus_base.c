/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_schedule_ciplus_base.c
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

#if defined(CONFIG_MW_CI_PLUS)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <sapi.h>
#include <papi.h>
#include <octo_common.h>
#include <otl.h>

#include <db_svc.h>
#include <db_param.h>

#include <svc_fs.h>
#include <svc_pipe.h>
#include <svc_ringbuf.h>
#include <svc_mediaserver.h>
#include <svc_output.h>
#include <svc_sys.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_schedule.h>
#include <mgr_live.h>
#include <mgr_pvr_action.h>
#include <mgr_satip.h>
#include <mgr_ntls.h>
#include <mgr_epg.h>

#include "../../local_include/_xmgr_schedule.h"

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


STATIC HERROR xmgr_schedule_CiPlusRecordDescRecordCheckSchedule_Base(HUINT32 ulStartLimit, HUINT32 ulEndLimit)
{
	HUINT32			ulCnt, ulNumSch = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	// 전체 스케쥴을 찾아서...1시 - 5시 사이에 스케쥴이 존재하는지 찾는다.
	// 스케쥴이 존재하면 FAIL리턴...찾는 걸 실패해도 FAIL리턴...
	(void)SAPI_CountSchedule(&ulNumSch);
	if (0 == ulNumSch)
	{
		HxLOG_Error("[%s:%d] No Schedule: Nothing to stop\n", __FUNCTION__, __LINE__);
		hErr = ERR_OK;
		goto END_FUNC;
	}

	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("[%s:%d] Memory allocation failed\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);
	if(hErr == ERR_OK)
	{
		for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
		{
			DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
			HUINT32			ulSchStartTime, ulSchEndTime;
			if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime, &ulSchStartTime)==ERR_OK)
			{
				if(ulSchStartTime > ulEndLimit)
				{
					// 5시이후 시작하는 아이템은 제외...
					continue;
				}
			}

			ulSchEndTime = ulSchStartTime + pstSchedule->ulDuration;
			if(ulSchEndTime <= ulStartLimit)
			{
				// 1시 이전에 시작하는 아이템은 제외...
				continue;
			}

			if(pstSchedule->eRsvType != DxRSVTYPE_IP_EPG_GATHERING
				&& pstSchedule->eRsvType != DxRSVTYPE_DESC_REC_STANDBY)
			{
				// IPEPG는 상관없을 듯....나머지는 자기자신일 듯...
				HxLOG_Error("[%s:%d] Found Schedule between %d and %d\n", __FUNCTION__, __LINE__, ulStartLimit, ulEndLimit);
				goto END_FUNC;
			}
		}
	}


	hErr = ERR_OK;

END_FUNC:


	if (pstSchArray)
	{
		HLIB_STD_MemFree(pstSchArray);
	}

	return hErr;
}

STATIC void xmgr_schedule_CiPlusRecordDescRecordDataReceived_Base (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxRecListData_t	*pstRecData = (DxRecListData_t *)userdata;
	DAxIterator_t	stIter;
	HBOOL			bFound = FALSE;

	HxLOG_Trace();

	if (size == 0)
	{
		HxLOG_Error("size is 0 \n");
		return;
	}

	DAPI_InitIterator(&stIter, data, size, 0);
	while (DAPI_NextIterator(&stIter))
	{
		DxRecListData_t 	*pstTempRecData = (DxRecListData_t *)stIter.data;
		// 기본적으로 가장 오래된 파일 순으로 Sorting...
		if(pstTempRecData)
		{
			// 우선순위 1순위....시청한 적이 없는 파일...음 ? 플레이한 적 있는 거랑 다른건가...
			HxLOG_Debug("[%s:%d] Priority 1 ... (%s)\n", __FUNCTION__, __LINE__, pstTempRecData->szName);
			if(pstTempRecData->bScrambled == TRUE && pstTempRecData->bPlayed == FALSE)
			{
				HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));
				HxSTD_MemCopy(pstRecData, pstTempRecData, sizeof(DxRecListData_t));
				bFound = TRUE;
				break;
			}
		}
	}

	if(bFound ==FALSE)
	{
		DAPI_InitIterator(&stIter, data, size, 0);
		while (DAPI_NextIterator(&stIter))
		{
			DxRecListData_t 	*pstTempRecData = (DxRecListData_t *)stIter.data;
			// 기본적으로 가장 오래된 파일 순으로 Sorting...
			if(pstTempRecData)
			{
				// 1순위를 제외한 Case는 일단
				HxLOG_Debug("[%s:%d] Priority 2 ... (%s)\n", __FUNCTION__, __LINE__, pstTempRecData->szName);
				if(pstTempRecData->bScrambled == TRUE && pstTempRecData->bPlayed == TRUE)
				{
					HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));
					HxSTD_MemCopy(pstRecData, pstTempRecData, sizeof(DxRecListData_t));
					break;
				}
			}
		}
	}
}

STATIC void xmgr_schedule_CiPlusBatchDescRecordDataReceived_Base (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxRecListData_t	*pstRecData = (DxRecListData_t *)userdata;
	DAxIterator_t	stIter;
	HBOOL			bFound = FALSE;

	HxLOG_Trace();

	if (size == 0)
	{
		HxLOG_Error("size is 0 \n");
		return;
	}

	DAPI_InitIterator(&stIter, data, size, 0);
	while (DAPI_NextIterator(&stIter))
	{
		DxRecListData_t 	*pstTempRecData = (DxRecListData_t *)stIter.data;
		// 기본적으로 가장 오래된 파일 순으로 Sorting...
		if(pstTempRecData)
		{
			// 우선순위 1순위....시청한 적이 없는 파일...음 ? 플레이한 적 있는 거랑 다른건가...
			HxLOG_Debug("Priority 1 ... (%s)\n", pstTempRecData->szName);
			if(pstTempRecData->bScrambled == TRUE && pstTempRecData->bPlayed == FALSE)
			{
				HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));
				HxSTD_MemCopy(pstRecData, pstTempRecData, sizeof(DxRecListData_t));
				bFound = TRUE;
				break;
			}
		}
	}

	if(bFound ==FALSE)
	{
		DAPI_InitIterator(&stIter, data, size, 0);
		while (DAPI_NextIterator(&stIter))
		{
			DxRecListData_t 	*pstTempRecData = (DxRecListData_t *)stIter.data;
			// 기본적으로 가장 오래된 파일 순으로 Sorting...
			if(pstTempRecData)
			{
				// 1순위를 제외한 Case는 일단
				HxLOG_Debug("Priority 2 ... (%s)\n", pstTempRecData->szName);
				if(pstTempRecData->bScrambled == TRUE && pstTempRecData->bPlayed == TRUE)
				{
					HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));
					HxSTD_MemCopy(pstRecData, pstTempRecData, sizeof(DxRecListData_t));
					break;
				}
			}
		}
	}
}

STATIC HERROR xmgr_schedule_CiPlusBatchDescRecordMakeRsvItem_Base (HUINT16 ulSvcuid, HxDATETIME_t *pstStartTime, UNIXTIME utDuration)
{
	DxSchedule_t	stSchedule = {0, };
	HERROR			hErr = ERR_FAIL;
	Handle_t 		hSvc;
	SAPI_Conflict_t	stConflict;

	VK_TASK_Sleep(500);

	if(pstStartTime==NULL)
	{
		HxLOG_Error("Can not found Time to reserve.\n");
		return ERR_FAIL;
	}

	if(utDuration>DESC_REC_AVAILABLE_TIME)
	{
		// 4시간이상은 들어올 수 없지만...혹시나...나중에 상황이 변할 수 있으므로...
		HxLOG_Error("This file is over than 4 hours.\n");
		return ERR_FAIL;
	}

	VK_memset(&stSchedule, 0, sizeof(DxSchedule_t) );
	HxSTD_MemCopy(&stSchedule.stStartTime, pstStartTime, sizeof(HxDATETIME_t));

	hErr = SAPI_GetAvailableSlot(&stSchedule.ulSlot);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("There is no empty slot for sama!\n");
		return ERR_FAIL;
	}
	HxLOG_Debug("Available Slot ... (%d)\n", stSchedule.ulSlot);

	HxLOG_Debug("ulSvcuid (%d) \n", ulSvcuid);

	stSchedule.eRsvType 		= DxRSVTYPE_DESC_REC_STANDBY;
	hErr = DB_SVC_GetServiceHandleBySvcIdx(ulSvcuid, &hSvc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx Error !\n" );
		return ERR_FAIL;
	}
	stSchedule.ulSvcUid			= DB_SVC_GetServiceUID(hSvc);
	stSchedule.eRsvReady		= DxRSVREADY_30_SEC;
	stSchedule.ulDuration		= utDuration;
	stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
	stSchedule.ulRepeatDays 	= 0;
	stSchedule.eStatus			= DxRSVSTATUS_WAITING;
	stSchedule.bExceptConflict	= FALSE;
	stSchedule.eEpgType 		= eDxEPG_TYPE_SC;

	HxLOG_Debug("on Solt: %d,  SVC UID: 0x%x\n",  stSchedule.ulSlot, stSchedule.ulSvcUid);

	hErr = SAPI_MakeSchedule (stSchedule.ulSlot, &stSchedule, &stConflict);

	return ERR_OK;
}

STATIC HERROR xmgr_schedule_CiPlusBatchDescRecordCheckSchedule_Base(HUINT32 ulStartLimit, HUINT32 ulEndLimit)
{
	HUINT32			ulCnt, ulNumSch = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	// 전체 스케쥴을 찾아서...1시 - 5시 사이에 스케쥴이 존재하는지 찾는다.
	// 스케쥴이 존재하면 FAIL리턴...찾는 걸 실패해도 FAIL리턴...
	(void)SAPI_CountSchedule(&ulNumSch);
	if (0 == ulNumSch)
	{
		HxLOG_Error("No Schedule: Nothing to stop\n");
		return ERR_OK;
	}

	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("Memory allocation failed\n");
		goto END_FUNC;
	}

	hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);
	if(hErr == ERR_OK)
	{
		for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
		{
			DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
			HUINT32			ulSchStartTime, ulSchEndTime;
			if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime, &ulSchStartTime)==ERR_OK)
			{
				if(ulSchStartTime > ulEndLimit)
				{
					// 5시이후 시작하는 아이템은 제외...
					continue;
				}
			}

			ulSchEndTime = ulSchStartTime + pstSchedule->ulDuration;
			if(ulSchEndTime <= ulStartLimit)
			{
				// 1시 이전에 시작하는 아이템은 제외...
				continue;
			}

			if(pstSchedule->eRsvType == DxRSVTYPE_DESC_REC_STANDBY)
			{
				// 설마 남아 있지는 않겠지만...
				SAPI_CancelSchedule(pstSchedule->ulSlot);
				continue;
			}
			else if(pstSchedule->eRsvType != DxRSVTYPE_IP_EPG_GATHERING)
			{
				// IPEPG는 상관없을 듯....
				HxLOG_Error("Found Schedule between %d and %d\n", ulStartLimit, ulEndLimit);
				hErr = ERR_FAIL;
				goto END_FUNC;
			}
		}
	}


	hErr = ERR_OK;

END_FUNC:


	if (pstSchArray)
	{
		HLIB_STD_MemFree(pstSchArray);
	}

	return hErr;
}

#define ___GLOBAL_FUNCTIONS___

HERROR  xmgr_schedule_CiPlusStartDescRecordFinedItem_Base(DxRecListData_t	*pstRecData)
{
	DxRecListData_t	*pstTmpRecData = NULL;
	HCHAR			query[256];
	HUINT32 		ulReqId;
	HUINT32			ulCurTime = 0,ulLimitTime = 0, ulLimitDuration = 0;
	HxDATETIME_t	stCurtTime,	stLimitTime;

	HxLOG_Trace();

	if(pstRecData == NULL)
	{
		HxLOG_Error("[%s:%d] pstRecData is NULL. \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	// 1. 현재시간 구하기...
	ulCurTime = HLIB_STD_GetSystemTime();
	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurTime, &stCurtTime) == ERR_FAIL)
	{
		HxLOG_Error("[%s:%d] Fail to Convert Current Time.\n",  __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	// 2. Descramble Recording제한시간 구하기...(오늘 5시)
	stLimitTime.stDate.usYear			= stCurtTime.stDate.usYear;
	stLimitTime.stDate.ucMonth			= stCurtTime.stDate.ucMonth;
	stLimitTime.stDate.ucDay			= stCurtTime.stDate.ucDay;
	stLimitTime.stTime.ucHour			= DESC_REC_END_TIME;
	stLimitTime.stTime.ucMinute 		= 0;
	stLimitTime.stTime.ucSecond 		= 0;
	stLimitTime.stTime.usMillisecond	= 0;

	if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&stLimitTime, &ulLimitTime)==ERR_FAIL)
	{
		HxLOG_Error("[%s:%d] Fail to Convert Limit Time.\n", __FUNCTION__, __LINE__);\
		return ERR_FAIL;
	}
	//  1:00:00 - 4:59:59
	if(stCurtTime.stTime.ucHour >= DESC_REC_START_TIME && stCurtTime.stTime.ucHour < DESC_REC_END_TIME)
	{
		// (1시 - 5시... 오늘 해야한다면 5시 - 현재시간 만큼 가능...)
		ulLimitDuration = ulLimitTime - ulCurTime;

		if(xmgr_schedule_CiPlusRecordDescRecordCheckSchedule_Base(ulCurTime, ulLimitTime)==ERR_OK)
		{
			// 가장 오래된 파일부터...녹화한다.... 1시에서 5시에 Descramble 가능한...
			HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE duration <= %d ORDER BY starttime ASC"
				, DxNAMEOF(DxRecListData_t)
				, ulLimitDuration
			);
			HxLOG_Debug("[%s:%d] Query = %s\n", __FUNCTION__, __LINE__, query);

			pstTmpRecData = (DxRecListData_t *)HLIB_STD_MemAlloc(sizeof(DxRecListData_t));
			if (pstTmpRecData == NULL)
			{
				HxLOG_Error("Memory allocation failed\n");
				return ERR_FAIL;
			}

			HxSTD_MemSet(pstTmpRecData, 0, sizeof(DxRecListData_t));

			//Descramble 할 파일 결정....
			ulReqId = DAPI_Query(query, (DAPI_Getter)xmgr_schedule_CiPlusRecordDescRecordDataReceived_Base, (void *)pstTmpRecData);
			DAPI_Wait(3000, ulReqId);
			if (ulReqId != 0 && pstTmpRecData->ulRecDuration > 0)
			{
				// 오늘 가능한 아이템을 찾은 경우... 예약 걸고 나가도 된다....
				HxSTD_MemCopy(pstRecData, pstTmpRecData, sizeof(DxRecListData_t));
				if(pstTmpRecData != NULL)
				{
					HLIB_STD_MemFree(pstTmpRecData);
					pstTmpRecData = NULL;
				}
				return ERR_OK;
			}
		}
	}

	if(pstTmpRecData != NULL)
	{
		HLIB_STD_MemFree(pstTmpRecData);
		pstTmpRecData = NULL;
	}
	return ERR_FAIL;
}



// TODO: xmgr_action_BatchDescRecordProcessDoProcess 의 내용이 옮겨온 것임.
// TODO: new batch 구조에서는, dapi_wait(), sleep 등 다 없어져야함. 만약 꼭 써야 할 상황이라면 따로 task 두어야함.
// TODO: new batch는 batch에 달린 모든 proc들이 동시에 start 되는 구조이기 때문에, obama main task가 절대로 서면 안된다.
HERROR	xmgr_schedule_CiPlusRescheduleDescrambleRecordBatch_Base()
{
	DxRecListData_t *pstRecData = NULL;
	HCHAR			query[256];
	HUINT32 		ulReqId;
	HUINT32 		ulCurTime = 0,ulLimitTime = 0, ulStartTime = 0, ulLimitDuration = 0, ulTimeGap;
	HxDATETIME_t	stCurtTime, stLimitTime, stStartTime;
	HBOOL			bDescrambleOnOff = FALSE;
	HUINT32 		ulCnt = 0, ulNumSch = 0, ulNumSch2 = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	if(DB_PARAM_GetItem(eDB_PARAM_ITEM_DESCRAMBLEONOFF, (HUINT32 *)&bDescrambleOnOff, 0)==ERR_FAIL)
	{
		HxLOG_Error("Fail to Get DB.\n");
		return ERR_FAIL;
	}

	if(bDescrambleOnOff == FALSE)
	{
		HxLOG_Error("This Action is not available by user select.\n");
		(void)SAPI_CountScheduleByType(DxRSVTYPE_DESC_REC_STANDBY, &ulNumSch);
		if (0 == ulNumSch)
		{
			HxLOG_Error("No Schedule: Nothing to stop\n");
			return ERR_FAIL;
		}

		pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
		if (NULL == pstSchArray)
		{
			HxLOG_Error("Memory allocation failed\n");
			return ERR_FAIL;

		}

		hErr = SAPI_GetAllSchedulesByRsvType(ulNumSch, &ulNumSch2, pstSchArray, DxRSVTYPE_DESC_REC_STANDBY);
		if(hErr == ERR_OK)
		{
			for (ulCnt = 0; ulCnt < ulNumSch2; ulCnt++)
			{
				DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
				if(pstSchedule != NULL && pstSchedule->eRsvType == DxRSVTYPE_DESC_REC_STANDBY)
				{
					SAPI_CancelSchedule(pstSchedule->ulSlot);
				}
				continue;
			}
		}
		HLIB_STD_MemFree(pstSchArray);

		return ERR_FAIL;

	}

	// 1. 현재시간 구하기...
	ulCurTime = HLIB_STD_GetSystemTime();
	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurTime, &stCurtTime) == ERR_FAIL)
	{
		HxLOG_Error("Fail to Convert Current Time.\n");
		return ERR_FAIL;

	}



	// 2. Descramble Recording제한시간 구하기...(오늘 5시)
	stLimitTime.stDate.usYear			= stCurtTime.stDate.usYear;
	stLimitTime.stDate.ucMonth			= stCurtTime.stDate.ucMonth;
	stLimitTime.stDate.ucDay			= stCurtTime.stDate.ucDay;
	stLimitTime.stTime.ucHour			= DESC_REC_END_TIME;
	stLimitTime.stTime.ucMinute 		= 0;
	stLimitTime.stTime.ucSecond 		= 0;
	stLimitTime.stTime.usMillisecond	= 0;

	if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&stLimitTime, &ulLimitTime)==ERR_FAIL)
	{
		HxLOG_Error("Fail to Convert Limit Time.\n");
		return ERR_FAIL;
	}

	if(stCurtTime.stTime.ucHour >= DESC_REC_START_TIME && stCurtTime.stTime.ucHour < DESC_REC_END_TIME)
	{
		// (1시 - 5시... 오늘 해야한다면 5시 - 현재시간 만큼 가능...)
		ulLimitDuration = ulLimitTime - ulCurTime;

		if(xmgr_schedule_CiPlusBatchDescRecordCheckSchedule_Base(ulCurTime, ulLimitTime)==ERR_OK)
		{
			// 가장 오래된 파일부터...녹화한다.... 1시에서 5시에 Descramble 가능한...
			HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE duration <= %d ORDER BY starttime ASC"
				, DxNAMEOF(DxRecListData_t)
				, ulLimitDuration
			);
			HxLOG_Debug("Query = %s\n", query);

			pstRecData = (DxRecListData_t *)HLIB_STD_MemAlloc(sizeof(DxRecListData_t));
			if (pstRecData == NULL)
			{
				HxLOG_Error("Memory allocation failed\n");
				return ERR_FAIL;
			}
			HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));

			//Descramble 할 파일 결정....
			ulReqId = DAPI_Query(query, (DAPI_Getter)xmgr_schedule_CiPlusBatchDescRecordDataReceived_Base, (void *)pstRecData);
			DAPI_Wait(3000, ulReqId);
			if (ulReqId != 0 && pstRecData->ulRecDuration > 0)
			{
				// 오늘 가능한 아이템을 찾은 경우... 예약 걸고 나가도 된다....
				xmgr_schedule_CiPlusBatchDescRecordMakeRsvItem_Base((HUINT16)(pstRecData->svcUId & 0xFFFF), &stCurtTime, pstRecData->ulRecDuration);
				if(pstRecData != NULL)
				{
					HLIB_STD_MemFree(pstRecData);
					pstRecData = NULL;
				}

				return ERR_FAIL;
			}

			if(pstRecData != NULL)
			{
				HLIB_STD_MemFree(pstRecData);
				pstRecData = NULL;
			}
		}
	}

	ulLimitDuration = DESC_REC_AVAILABLE_TIME;

	// 예약이 시작될 시간을 정한다.
	if(stCurtTime.stTime.ucHour < DESC_REC_START_TIME)
	{
		// 1시부터는 무조건 컨텐츠 하나는 푸는 것이 가능...
		stStartTime.stDate.usYear			= stCurtTime.stDate.usYear;
		stStartTime.stDate.ucMonth			= stCurtTime.stDate.ucMonth;
		stStartTime.stDate.ucDay			= stCurtTime.stDate.ucDay;
		stStartTime.stTime.ucHour			= DESC_REC_START_TIME;
		stStartTime.stTime.ucMinute 		= 0;
		stStartTime.stTime.ucSecond 		= 0;
		stStartTime.stTime.usMillisecond	= 0;
	}
	else
	{
		// 나머지는 오늘 못하는 상황 ...
		// 5시전에 못푸는 것은 다음날 한시로...
		HLIB_DATETIME_GetNextDay(ulCurTime, &ulStartTime, &ulTimeGap);
		if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &stStartTime) == ERR_FAIL)
		{
			HxLOG_Error("Fail to Convert Next Time.\n");
			return ERR_FAIL;
		}
		stStartTime.stTime.ucHour			= DESC_REC_START_TIME;
		stStartTime.stTime.ucMinute 		= 0;
		stStartTime.stTime.ucSecond 		= 0;
		stStartTime.stTime.usMillisecond	= 0;

		HLIB_DATETIME_GetNextDay(ulLimitTime, &ulLimitTime, &ulTimeGap);
	}

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stStartTime, &ulStartTime);

	HxLOG_Debug("START Time : %s\n", HLIB_DATETIME_UnixTimeToString(ulStartTime, NULL, 0));

	if(xmgr_schedule_CiPlusBatchDescRecordCheckSchedule_Base(ulStartTime, ulLimitTime)==ERR_FAIL)
	{
		// 스케쥴을 찾거나, 찾는 과정에 문제라도 생기면 일단 무효...
		return ERR_FAIL;
	}

	// 가장 오래된 파일부터...녹화한다.... 1시에서 5시에 Descramble 가능한...
	HxSTD_PrintToStrN(query, 256
		, "SELECT * FROM %s WHERE duration <= %d ORDER BY starttime ASC"
		, DxNAMEOF(DxRecListData_t)
		, ulLimitDuration
	);
	HxLOG_Debug("Query = %s\n", query);

	pstRecData = (DxRecListData_t *)HLIB_STD_MemAlloc(sizeof(DxRecListData_t));
	if (pstRecData == NULL)
	{
		HxLOG_Error("Memory allocation failed\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));

	//Descramble 할 파일 결정....
	ulReqId = DAPI_Query(query, (DAPI_Getter)xmgr_schedule_CiPlusBatchDescRecordDataReceived_Base, (void *)pstRecData);
	DAPI_Wait(3000, ulReqId);
	if (ulReqId== 0)
	{
		HxLOG_Error("Query FAIL \n");

		if(pstRecData != NULL)
		{
			HLIB_STD_MemFree(pstRecData);
			pstRecData = NULL;
		}
		return ERR_FAIL;
	}

	if(pstRecData->ulRecDuration == 0)
	{
		// 녹화되지 않은 경우나 찾은 파일이 없는 경우...
		HxLOG_Error("There is no file to descramble.\n");
		if(pstRecData != NULL)
		{
			HLIB_STD_MemFree(pstRecData);
			pstRecData = NULL;
		}
		return ERR_FAIL;
	}
	// 결정된 파일에 대한 정보...

	HxLOG_Debug("Name : %s(Duration: %d) %s%s\n", pstRecData->szName, pstRecData->ulRecDuration, pstRecData->stMeta.szPath, pstRecData->stMeta.szFileName);
	// 일단 예약은..1시에서 5시로 걸것...
	xmgr_schedule_CiPlusBatchDescRecordMakeRsvItem_Base((HUINT16)(pstRecData->svcUId & 0xFFFF), &stStartTime, pstRecData->ulRecDuration);

	if(pstRecData != NULL)
	{
		HLIB_STD_MemFree(pstRecData);
		pstRecData = NULL;
	}

	return ERR_OK;
}

#define ___MESSAGE_FUNCTIONS___











#define _____DEBUG_FUNCTION_____

#endif
