/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_debug.c
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
#include <vkernel.h>
#include <hlib.h>
#include <dapi.h>
#include <dlib.h>
#include <sapi.h>

#include "sama_int.h"
#include <sama_lib.h>

#if defined(CONFIG_DEBUG)
/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/
#define	hCmdHandle			"SamaCmd"

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

STATIC HINT32 sama_dbg_PrintTrackingStatus (void *pvArg)
{
	SAMALIB_TRACKING_PrintTrackingStatus();
	return HxCMD_OK;
}
STATIC HINT32 sama_dbg_PrintSeriesMem (void *pvArg)
{
	SAMALIB_TVA_PrintMemoryStatus();

	return HxCMD_OK;
}


STATIC HINT32 sama_dbg_PrintSeriesSet (void *pvArg)
{
	SAMALIB_TVA_PrintAllSeriesSet(HLIB_CMD_Printf);

	return HxCMD_OK;
}

STATIC HINT32 sama_dbg_PrintSlot (void *pvArg)
{
	SAMA_SCHEDULE_PrintSlotStatus(HLIB_CMD_Printf);

	return HxCMD_OK;
}

STATIC HINT32 sama_dbg_PrintSchedule (void *pvArg)
{
	HCHAR	*aucArg=NULL;
	HUINT32	ulSlotId;
	DxSchedule_t	*pstSchedule;
	HCHAR			*szSchedStr = NULL;

	/* 1st argument */
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
	{
		SAMA_SCHEDULE_PrintAllSchedules(HLIB_CMD_Printf);

		return HxCMD_OK;
	}
	if( sscanf(aucArg, "%d", &ulSlotId) == 0 )
		return HxCMD_ERR;

	pstSchedule = SAMA_SCHEDULE_GetSchedule(ulSlotId);
	if (pstSchedule == NULL)
	{
		HxLOG_Error("SAMA_SCHEDULE_GetSchedule failed. slot=(%d) \n", ulSlotId);
		return HxCMD_ERR;
	}

	szSchedStr = (HCHAR *)HLIB_STD_MemAlloc(4096);

	if (NULL == szSchedStr)
	{
		return ERR_FAIL;
	}

	HLIB_CMD_Printf("\n - Print Schedule slot=(%d) -\n\n",ulSlotId);
	HLIB_CMD_Printf("  -------------------------------------------\n");
	(void)SAMA_SCHEDULE_StrPrintSchedule(szSchedStr, 4096, pstSchedule);
	HLIB_CMD_Printf("%s\n", szSchedStr);
	HLIB_CMD_Printf("  -------------------------------------------\n");

	HLIB_STD_MemFree(szSchedStr);

	return HxCMD_OK;


}

STATIC HINT32 sama_dbg_PrintTimer (void *pvArg)
{
	SAMA_TIMER_PrintAllTimer(HLIB_CMD_Printf);

	return HxCMD_OK;
}

STATIC HINT32 sama_dbg_CancelSchedule (void *pvArg)
{
	HCHAR	*aucArg=NULL;
	HUINT32	ulSlotId;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

	/* 1st argument */
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulSlotId) == 0 )
		return HxCMD_ERR;

	stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;
	SAMA_SCHEDULE_RemoveSchedule(ulSlotId, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);

	return HxCMD_OK;
}

STATIC HINT32 sama_dbg_CancelAllSchedule (void *pvArg)
{
	HUINT32 ulNumSch = 0, i;
	DxSchedule_t	*pstSchArray = NULL;
	HUINT32	ulSlotId;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

	SAMA_SCHEDULE_GetAllSchedules(&ulNumSch,&pstSchArray);

	for(i=0; i < ulNumSch; i++)
	{
		ulSlotId = pstSchArray[i].ulSlot;
		stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;
		SAMA_SCHEDULE_RemoveSchedule(ulSlotId, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
	}

	if (NULL != pstSchArray)					{ SAMA_Free(pstSchArray); }

	return HxCMD_OK;
}

STATIC HINT32 sama_dbg_UpdateSeries (void *pvArg)
{
	SAMA_SCHEDULE_UpdateSeries(SAMA_GetVirtualRequestId());

	return HxCMD_OK;
}

STATIC HINT32 sama_dbg_TrackSchedule (void *pvArg)
{
	HCHAR	*aucArg=NULL;
	HUINT32	ulSlotId;
	DxSchedule_t	*pstSchedule;

	/* 1st argument */
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulSlotId) == 0 )
		return HxCMD_ERR;

	pstSchedule = SAMA_SCHEDULE_GetSchedule(ulSlotId);
	if (pstSchedule == NULL)
	{
		HxLOG_Error("SAMA_SCHEDULE_GetSchedule failed. slot=(%d) \n", ulSlotId);
		return HxCMD_ERR;
	}

	SAMALIB_TRACKING_StartTracking(pstSchedule->ulSvcUid, pstSchedule->uExtInfo.stRec.nEvtId, ulSlotId, 0);

	return HxCMD_OK;
}

STATIC HINT32 sama_dbg_TimerSchedule (void *pvArg)
{
	HCHAR	*aucArg=NULL;
	HUINT32	ulSlotId;
	HUINT32 type;
	HCHAR	szType[256];

	/* 1st argument */
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulSlotId) == 0 )
		return HxCMD_ERR;
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%s", szType) == 0 )
		return HxCMD_ERR;

	if (HxSTD_StrNCmp(szType, "ready", 256) == 0)
	{
		type = eSAPI_TIMER_TYPE_Ready;
	}
	else if (HxSTD_StrNCmp(szType, "start", 256) == 0)
	{
		type = eSAPI_TIMER_TYPE_Start;
	}
	else if (HxSTD_StrNCmp(szType, "stop", 256) == 0)
	{
		type = eSAPI_TIMER_TYPE_Stop;
	}
	else if (HxSTD_StrNCmp(szType, "pending", 256) == 0)
	{
		type = eSAPI_TIMER_TYPE_StopPendingBroken;
	}
	else if (HxSTD_StrNCmp(szType, "runaway", 256) == 0)
	{
		type = eSAPI_TIMER_TYPE_StopRunaway;
	}
	else if (HxSTD_StrNCmp(szType, "tracking", 256) == 0)
	{
		type = eSAPI_TIMER_TYPE_ReadyTracking;
	}
	else if (HxSTD_StrNCmp(szType, "delayed", 256) == 0)
	{
		type = eSAPI_TIMER_TYPE_UpdateDelayed;
	}
	else
	{
		type = atoi(szType);
	}

	SAMA_PostMessage(SAMAMESSAGE_PROCESSTWOWAYTIMER, ulSlotId, type, 0, 0, 0, 0);

	return HxCMD_OK;
}


STATIC HINT32 sama_dbg_PrintSysTime (void *pvArg)
{
	HINT32	offsetTime = 0;
	HINT32	ulUnixTime = 0;

	HLIB_DATETIME_GetTimeOffsetWithDst(&offsetTime);

	ulUnixTime = HLIB_STD_GetSystemTime();
	HLIB_CMD_Printf("\r current (unix)system time: %d \n",ulUnixTime);


	ulUnixTime += (offsetTime * SECONDS_PER_MIN);
	HLIB_CMD_Printf("current local time (with offset): %d \n", ulUnixTime);

	return HxCMD_OK;
}

STATIC HINT32 sama_dbg_PrintEarliestSchedule (void *pvArg)
{
	DxSchedule_t	*pstSchedule = NULL;
	HCHAR			*szSchedStr = NULL;

	pstSchedule = SAMA_SCHEDULE_GetEarliestSchedule();
	if (pstSchedule == NULL)
	{
		HxLOG_Error("SAMA_SCHEDULE_GetEarliestSchedule failed. \n");
		return HxCMD_ERR;
	}

	szSchedStr = (HCHAR *)HLIB_STD_MemAlloc(4096);
	if (NULL == szSchedStr)
	{
		return ERR_FAIL;
	}

	HLIB_CMD_Printf("\n - Print Earliest Schedule -\n");
	HLIB_CMD_Printf("  -------------------------------------------\n");
	(void)SAMA_SCHEDULE_StrPrintSchedule(szSchedStr, 4096, pstSchedule);
	HLIB_CMD_Printf("%s\n", szSchedStr);
	HLIB_CMD_Printf("  -------------------------------------------\n");

	HLIB_STD_MemFree(szSchedStr);

	return HxCMD_OK;
}

#define TEST_DURATION_SEC	600
#define TEST_INTERVAL_SEC	0
#define TEST_NUM_CH_UID		100
STATIC HINT32 sama_dbg_MakeRandomSchedule (void *pvArg)
{
	HCHAR			*aucArg=NULL;
	HUINT32 		n = 0, ulCount = 0, successCount = 0;
	HUINT32			ulSysTime =0;
	DxSchedule_t		stSchedule = {0, };
	HERROR			hErr = ERR_FAIL;
	HUINT32 		uUID[TEST_NUM_CH_UID] = {0, };
	HUINT32 		ulNumOfSvcUid = 0;
	SAMA_Context_t		*pstContext = SAMA_GetContext();


	/* 1st argument */
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulCount) == 0 )
		return HxCMD_ERR;

	/* 2nd argument */
	while(1)
	{
		if(ulNumOfSvcUid >= TEST_NUM_CH_UID)
			break;

		aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
		if( aucArg == NULL )
			break;
		if( sscanf(aucArg, "%d", &uUID[ulNumOfSvcUid]) == 0 )
			break;

		ulNumOfSvcUid++;
	}

	HxLOG_Debug("ulNumOfSvcUid: %d \n", ulNumOfSvcUid);

	if(ulNumOfSvcUid == 0)
	{
		return HxCMD_ERR;
	}

	ulSysTime = HLIB_STD_GetSystemTime();
	ulSysTime += 300;

	// schedule list 를 다 가지고 와서.
	if (NULL != pstContext->stSamaConflictCheck.pstScheduleList)
	{
		pstContext->stSamaConflictCheck.pstScheduleList = HLIB_LIST_RemoveAllFunc (pstContext->stSamaConflictCheck.pstScheduleList, SAMA_Free_CB);
	}
	SAMA_SCHEDULE_TreeValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));

	srand(ulSysTime);

	HxLOG_Debug("make %d TBR items \n", ulCount);

	for(n = 0; n < ulCount; n++)
	{
		VK_memset( &stSchedule, 0, sizeof(DxSchedule_t) );


		hErr = SAMA_SCHEDULE_GetEmptySlotForSama(&(stSchedule.ulSlot));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("There is no empty slot for sama!\n");
			continue;
		}



		stSchedule.eRsvType 		= DxRSVTYPE_RECORD_TBR;
		stSchedule.ulSvcUid 		= uUID[HLIB_MATH_Random()%ulNumOfSvcUid];
		stSchedule.eRsvReady		= DxRSVREADY_0_SEC;
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSysTime, &(stSchedule.stStartTime));
		stSchedule.ulDuration		= TEST_DURATION_SEC;
		stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
		stSchedule.ulRepeatDays 	= 0;
		stSchedule.eStatus			= DxRSVSTATUS_WAITING;
		stSchedule.bExceptConflict		= FALSE;
		stSchedule.eEpgType 		= eDxEPG_TYPE_SC;

		HxLOG_Debug("on Solt: %d,  SVC UID: 0x%x\n", stSchedule.ulSlot, stSchedule.ulSvcUid);

		HxLOG_Debug("\r TIME is (0x%x) [%d-%d-%d] : [%d-%d-%d-%d]\r\n",ulSysTime, stSchedule.stStartTime.stDate.usYear,stSchedule.stStartTime.stDate.ucMonth,stSchedule.stStartTime.stDate.ucDay,
			stSchedule.stStartTime.stTime.ucHour,stSchedule.stStartTime.stTime.ucMinute,stSchedule.stStartTime.stTime.ucSecond,stSchedule.stStartTime.stTime.usMillisecond);

		(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstContext->stSamaConflictCheck), &stSchedule);

#ifndef SAMA_LAZY_PADDING_CONFLICT_REMOVE
		 (void)SAMA_SCHEDULE_RemovePadding(pstContext, FALSE, eSAPI_PADDINGRMV_StartPad | eSAPI_PADDINGRMV_StopPad);
#endif

		HxLOG_Debug("stSamaConflictCheck.stConflictResult.eType: %d \n", pstContext->stSamaConflictCheck.stConflictResult.eType);

		if (eSAPI_CONFLICT_TYPE_None == pstContext->stSamaConflictCheck.stConflictResult.eType)
		{

			HxLOG_Debug("no Conflict. make reservation\n");

			switch (pstContext->stSamaConflictCheck.stSpecOption.eSpec)
			{
			case eDxSchedule_Spec_Base:
			default:
				HxLOG_Debug("add schedule on slot: %d \n", stSchedule.ulSlot);

				(void)SAMA_SCHEDULE_AddSchedule(stSchedule.ulSlot, &stSchedule, TRUE, SAMA_GetVirtualRequestId());
				break;
			}
		}

		ulSysTime = ulSysTime + TEST_DURATION_SEC + TEST_INTERVAL_SEC;
		successCount++;
	}

	HxLOG_Debug("%d of %d items were scheduled. \n", successCount, ulCount);

	return HxCMD_OK;
}

STATIC HINT32 sama_dbg_MakeSchedule (void *pvArg)
{
	HCHAR			*aucArg=NULL;
	HUINT32			ulStartTime =0;
	HUINT32			ulDuration =0;
	DxSchedule_t		stSchedule = {0, };
	HERROR			hErr = ERR_FAIL;
	HUINT32 		uUID;
	DxRsvType_e		eRsvType=0;
	SAMA_Context_t		*pstContext = SAMA_GetContext();


	/* 1st argument */
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &uUID) == 0 )
		return HxCMD_ERR;

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", (int*)&eRsvType) == 0 )
		return HxCMD_ERR;

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulStartTime) == 0 )
		return HxCMD_ERR;

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulDuration) == 0 )
		return HxCMD_ERR;




	// schedule list 를 다 가지고 와서.
	if (NULL != pstContext->stSamaConflictCheck.pstScheduleList)
	{
		pstContext->stSamaConflictCheck.pstScheduleList = HLIB_LIST_RemoveAllFunc (pstContext->stSamaConflictCheck.pstScheduleList, SAMA_Free_CB);
	}
	SAMA_SCHEDULE_TreeValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));




	VK_memset( &stSchedule, 0, sizeof(DxSchedule_t) );


	hErr = SAMA_SCHEDULE_GetEmptySlotForSama(&(stSchedule.ulSlot));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("There is no empty slot for sama!\n");
		return HxCMD_ERR;
	}



	stSchedule.eRsvType 		= eRsvType;
	stSchedule.ulSvcUid 			= uUID;
	stSchedule.eRsvReady		= DxRSVREADY_30_SEC;
	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &(stSchedule.stStartTime));
	stSchedule.ulDuration		= ulDuration;
	stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
	stSchedule.ulRepeatDays 	= 0;
	stSchedule.eStatus			= DxRSVSTATUS_WAITING;
	stSchedule.bExceptConflict		= FALSE;
	stSchedule.eEpgType 		= eDxEPG_TYPE_SC;
	stSchedule.uExtInfo.stRec.ulPreOffsetTime = 180;
	stSchedule.uExtInfo.stRec.ulPostOffsetTime = 180;

	if(stSchedule.eRsvType == 34) // test용... series rec
	{
		HxLOG_Debug("test series rec... \n");
		stSchedule.uExtInfo.stRec.ulSeriesId = 2829820;
		stSchedule.uExtInfo.stRec.nEvtId = 387339271;

		stSchedule.uExtInfo.stRec.ulPreOffsetTime = 0;
		stSchedule.uExtInfo.stRec.ulPostOffsetTime = 0;
		stSchedule.eEpgType 		= eDxEPG_TYPE_IP;

	}


	HxLOG_Debug("on Solt: %d,  SVC UID: 0x%x\n", stSchedule.ulSlot, stSchedule.ulSvcUid);

	HxLOG_Debug("\r TIME is (0x%x) [%d-%d-%d] : [%d-%d-%d-%d]\r\n",ulStartTime, stSchedule.stStartTime.stDate.usYear,stSchedule.stStartTime.stDate.ucMonth,stSchedule.stStartTime.stDate.ucDay,
		stSchedule.stStartTime.stTime.ucHour,stSchedule.stStartTime.stTime.ucMinute,stSchedule.stStartTime.stTime.ucSecond,stSchedule.stStartTime.stTime.usMillisecond);

	(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstContext->stSamaConflictCheck), &stSchedule);

#ifndef SAMA_LAZY_PADDING_CONFLICT_REMOVE
	 (void)SAMA_SCHEDULE_RemovePadding(pstContext, FALSE, eSAPI_PADDINGRMV_StartPad | eSAPI_PADDINGRMV_StopPad);
#endif

	HxLOG_Debug("stSamaConflictCheck.stConflictResult.eType: %d \n", pstContext->stSamaConflictCheck.stConflictResult.eType);

	if (eSAPI_CONFLICT_TYPE_None == pstContext->stSamaConflictCheck.stConflictResult.eType)
	{

		HxLOG_Debug("no Conflict. make reservation\n");

		switch (pstContext->stSamaConflictCheck.stSpecOption.eSpec)
		{
		case eDxSchedule_Spec_Base:
		default:
			HxLOG_Debug("add schedule on slot: %d \n", stSchedule.ulSlot);

			(void)SAMA_SCHEDULE_AddSchedule(stSchedule.ulSlot, &stSchedule, TRUE, SAMA_GetVirtualRequestId());
			break;
		}
	}
	else
	{
		HxLOG_Error("Conflict occurred!!!\n");
	}

	return HxCMD_OK;
}


static int sama_dbg_PrintRPCFunction(void *szArgs)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		HLIB_RPC_PrintHelp(nRpcHandle, NULL);
	}

	return 0;
}

typedef struct
{
	HINT32 nEBR;
	HINT32 nTBR;
	HINT32 nSBR;
	HINT32 nInstant;
	HINT32 nDelayed;
} SAMA_DBG_SchRecCount_t;

static HBOOL	sama_dbg_PrintSchRec(void *data, void *userData)
{
	DxSchedule_t 	*pSch = (DxSchedule_t *)data;
	HxDATETIME_t 	stEndTime;
	UNIXTIME		ulSchStartTime = 0;
	HCHAR			cRsvType = ' ';
	SAMA_DBG_SchRecCount_t *pstSchRecCount = (SAMA_DBG_SchRecCount_t *)userData;

	// only recording schedule( EBR/TBR/SBR/Instant/Delayed )
	if( pSch->eRsvType == DxRSVTYPE_RECORD_EBR )				{ cRsvType = 'E'; pstSchRecCount->nEBR++; }
	else if( pSch->eRsvType == DxRSVTYPE_RECORD_TBR )			{ cRsvType = 'T'; pstSchRecCount->nTBR++; }
	else if( pSch->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR )	{ cRsvType = 'S'; pstSchRecCount->nSBR++; }
	else if( pSch->eRsvType == DxRSVTYPE_RECORD_INSTANT )		{ cRsvType = 'I'; pstSchRecCount->nInstant++; }
	else if( pSch->eRsvType == DxRSVTYPE_RECORD_DELAYED )		{ cRsvType = 'D'; pstSchRecCount->nDelayed++; }
	else return FALSE;

	if( HLIB_DATETIME_ConvertDateTimeToUnixTime(&pSch->stStartTime, &ulSchStartTime) != ERR_OK )
	{
		HLIB_CMD_Printf("[RETURN] %s:%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	HxSTD_MemSet(&stEndTime, 0x00, sizeof(HxDATETIME_t));
	if( HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSchStartTime + pSch->ulDuration, &stEndTime) != ERR_OK )
	{
		HLIB_CMD_Printf("[RETURN] %s:%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}


	HLIB_CMD_Printf("[%3d] [%c] %04d/%02d/%02d (%02d:%02d:%02d ~ %02d:%02d:%02d)[%3d][%3d]",
				pSch->ulSlot, cRsvType,
				pSch->stStartTime.stDate.usYear, pSch->stStartTime.stDate.ucMonth, pSch->stStartTime.stDate.ucDay,
				pSch->stStartTime.stTime.ucHour, pSch->stStartTime.stTime.ucMinute, pSch->stStartTime.stTime.ucSecond,
				stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond,
				pSch->uExtInfo.stRec.ulPreOffsetTime, pSch->uExtInfo.stRec.ulPostOffsetTime);

	HLIB_CMD_Printf(" [%d][%-30s]", pSch->ulSvcUid, pSch->uExtInfo.stRec.szEventName);

	if(pSch->eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
	{
		DxRSV_TvAnyTimeRecord_t *pstTvAnyTime = NULL;
		if (MACRO_IS_TVA_RECORD_TYPE(pSch->eRsvType))
		{
			pstTvAnyTime = &(pSch->uExtInfo.stRec.ex.tva);

		}
		HLIB_CMD_Printf(" [%s]", (pstTvAnyTime != NULL)?pstTvAnyTime->szCRID:"");
	}
	else if(pSch->eScheduleSpec == eDxSchedule_Spec_Freesat)
	{
		DxRSV_FreesatRecord_t *pstFreesat = NULL;
		if (MACRO_IS_TVA_RECORD_TYPE(pSch->eRsvType))
		{
			pstFreesat = &(pSch->uExtInfo.stRec.ex.freesat);

		}
		HLIB_CMD_Printf(" [%s]", (pstFreesat != NULL)?pstFreesat->szCRID:"");

	}
	HLIB_CMD_Printf("\n");

	return FALSE;
}

static HINT32	local_SchRec_SortByStartTime (const void *pvArg1, const void *pvArg2)
{
	DxSchedule_t *pstSchedule1 = *(DxSchedule_t **)pvArg1;
	DxSchedule_t *pstSchedule2 = *(DxSchedule_t **)pvArg2;
	UNIXTIME		ulSchStart1 = 0;
	UNIXTIME		ulSchStart2 = 0;
	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule1->stStartTime, &ulSchStart1);
	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule2->stStartTime, &ulSchStart2);

	return (ulSchStart1 - ulSchStart2);
}

STATIC HINT32 sama_dbg_PrintSchRecInfo(void *pvArg)
{
	HCHAR			*aucArg = NULL;
	HCHAR			szDateTime[32] = {0, };
	HxDATETIME_t	stDateTime;
	UNIXTIME		ulFilterStartTime = 0;
	UNIXTIME		ulSchStartTime = 0;
	HBOOL			bFiltering = FALSE;
	HINT32			nTotalCount = 0;
	void			*pvIter = NULL;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();
	HxVector_t		*pstVector = HLIB_VECTOR_New(NULL, NULL);
	SAMA_DBG_SchRecCount_t	stSchRecCount = {0, };

	if (NULL == pstSchTree)
		return ERR_FAIL;

	HLIB_CMD_Printf("\n");

	/* 1st argument */
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if(aucArg != NULL)
	{
		if(sscanf(aucArg, "%s", szDateTime) == 0)
		{
			HLIB_CMD_Printf("Invalid argument format(%d.%d.%d-%d:%d:%d)\n") ;
			return HxCMD_ERR;
		}

		(void)HLIB_DATETIME_GetDateTimeByString(szDateTime, &stDateTime);
		if( HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &ulFilterStartTime) != ERR_OK )
		{
			HLIB_CMD_Printf("Invalid argument format(%d.%d.%d-%d:%d:%d)\n") ;
			return HxCMD_ERR;
		}
		HLIB_CMD_Printf("*************************************************************\n");
		HLIB_CMD_Printf("Filter Time : %s( %d )\n", szDateTime, ulFilterStartTime);
		bFiltering = TRUE;
	}
	HLIB_CMD_Printf("*************************************************************\n");
	HLIB_CMD_Printf("[Slot] [Rec Type] Date (Start_Time ~ End_Time)[S Padding][E Padding] [SvcUID][Event Name] [CRID]\n");
	HLIB_CMD_Printf("*************************************************************\n");

	for(pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t *pSch = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if(NULL != pSch)
		{
			ulSchStartTime = 0;
			if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&pSch->stStartTime, &ulSchStartTime) != ERR_OK)
			{
				continue;
			}

			if(bFiltering)
			{
				if(ulFilterStartTime < ulSchStartTime)
				{
					continue;
				}
			}

			// Only Recording type
			if( pSch->eRsvType != DxRSVTYPE_RECORD_EBR &&
				pSch->eRsvType != DxRSVTYPE_RECORD_TBR &&
				pSch->eRsvType != DxRSVTYPE_RECORD_SERIES_EBR &&
				pSch->eRsvType != DxRSVTYPE_RECORD_INSTANT &&
				pSch->eRsvType != DxRSVTYPE_RECORD_DELAYED)
			{
				continue;
			}
			(void)HLIB_VECTOR_Add(pstVector, (void *)pSch);
			nTotalCount++;
		}
	}
	(void)HLIB_VECTOR_Sort(pstVector, local_SchRec_SortByStartTime);
	(void)HLIB_VECTOR_ForEach(pstVector, (HBOOL(*)(void *,void *))sama_dbg_PrintSchRec, &stSchRecCount);

	HLIB_CMD_Printf("*************************************************************\n");
	HLIB_CMD_Printf("TBR( %d ), EBR( %d ), SBR( %d ), Instant( %d ), Delayed( %d )\n", stSchRecCount.nTBR, stSchRecCount.nEBR, stSchRecCount.nSBR, stSchRecCount.nInstant, stSchRecCount.nDelayed);
	HLIB_CMD_Printf("Total : [%d]\n", nTotalCount);
	HLIB_CMD_Printf("*************************************************************\n");

	HLIB_VECTOR_Delete(pstVector);

	return HxCMD_OK;
}

static HBOOL	sama_dbg_PrintSeries(void *data, void *userData)
{
	DxSchedule_t 	*pSch = (DxSchedule_t *)data;
	HxDATETIME_t 	stEndTime;
	UNIXTIME		ulSchStartTime = 0;

	if( HLIB_DATETIME_ConvertDateTimeToUnixTime(&pSch->stStartTime, &ulSchStartTime) != ERR_OK )
	{
		HLIB_CMD_Printf("[RETURN] %s:%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	HxSTD_MemSet(&stEndTime, 0x00, sizeof(HxDATETIME_t));
	if( HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSchStartTime + pSch->ulDuration, &stEndTime) != ERR_OK )
	{
		HLIB_CMD_Printf("[RETURN] %s:%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}


	HLIB_CMD_Printf("[%3d] %04d/%02d/%02d (%02d:%02d:%02d ~ %02d:%02d:%02d)[%3d][%3d]",
				pSch->ulSlot,
				pSch->stStartTime.stDate.usYear, pSch->stStartTime.stDate.ucMonth, pSch->stStartTime.stDate.ucDay,
				pSch->stStartTime.stTime.ucHour, pSch->stStartTime.stTime.ucMinute, pSch->stStartTime.stTime.ucSecond,
				stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond,
				pSch->uExtInfo.stRec.ulPreOffsetTime, pSch->uExtInfo.stRec.ulPostOffsetTime);

	HLIB_CMD_Printf(" [%d][%-30s]", pSch->ulSvcUid, pSch->uExtInfo.stRec.szEventName);

	if(pSch->eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
	{
		DxRSV_TvAnyTimeRecord_t *pstTvAnyTime = &(pSch->uExtInfo.stRec.ex.tva);
		HLIB_CMD_Printf(" [%s]", (pstTvAnyTime != NULL)?pstTvAnyTime->szCRID:"");
	}
	else if(pSch->eScheduleSpec == eDxSchedule_Spec_Freesat)
	{
		DxRSV_FreesatRecord_t *pstFreesat = &(pSch->uExtInfo.stRec.ex.freesat);
		HLIB_CMD_Printf(" [%s]", (pstFreesat != NULL)?pstFreesat->szCRID:"");
	}
	HLIB_CMD_Printf("\n");

	return FALSE;
}

STATIC HINT32 sama_dbg_PrintSeriesInfo(void *pvArg)
{
	HCHAR			*aucArg = NULL;
	HCHAR			szCRID[DxPVR_TVANYTIME_CRID_TOTAL_LEN] = {0, };
	HBOOL			bFiltering = FALSE;
	HINT32			nTotalCount = 0;
	void			*pvIter = NULL;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();
	HxVector_t		*pstVector = HLIB_VECTOR_New(NULL, NULL);
	SAMA_DBG_SchRecCount_t	stSchRecCount = {0, };

	if (NULL == pstSchTree)
		return ERR_FAIL;

	HLIB_CMD_Printf("\n");

	/* 1st argument - CRID Filter*/
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg != NULL )
	{
		if(sscanf(aucArg, "%s", szCRID) == 0)
		{
			HLIB_CMD_Printf("USAGE : print_series [CRID]\n");
			return HxCMD_ERR;
		}
		HLIB_CMD_Printf("*************************************************************\n");
		HLIB_CMD_Printf("Filter CRID : %s\n", szCRID);
		bFiltering = TRUE;
	}

	HLIB_CMD_Printf("*************************************************************\n");
	HLIB_CMD_Printf("[Slot] Date (Start_Time ~ End_Time)[S Padding][E Padding] [SvcUID][Event Name] [CRID]\n");
	HLIB_CMD_Printf("*************************************************************\n");
	for(pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t *pSch = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if(NULL != pSch)
		{
			if( pSch->eRsvType != DxRSVTYPE_RECORD_SERIES_EBR)
				continue;

			if(bFiltering)
			{
				if(pSch->eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
				{
					DxRSV_TvAnyTimeRecord_t *pstTvAnyTime = &(pSch->uExtInfo.stRec.ex.tva);
					if(HxSTD_StrCmp(szCRID, pstTvAnyTime->szCRID) != 0)
					{
						continue;
					}
				}
				else if(pSch->eScheduleSpec == eDxSchedule_Spec_Freesat)
				{
					DxRSV_FreesatRecord_t *pstFreesat = &(pSch->uExtInfo.stRec.ex.freesat);
					if(HxSTD_StrCmp(szCRID, pstFreesat->szCRID) != 0)
					{
						continue;
					}

				}
				else
				{
					// TODO:
					continue;
				}
			}

			(void)HLIB_VECTOR_Add(pstVector, (void *)pSch);
			nTotalCount++;
		}
	}
	(void)HLIB_VECTOR_Sort(pstVector, local_SchRec_SortByStartTime);
	(void)HLIB_VECTOR_ForEach(pstVector, (HBOOL(*)(void *,void *))sama_dbg_PrintSeries, &stSchRecCount);

	HLIB_CMD_Printf("*************************************************************\n");
	HLIB_CMD_Printf("Total : [%d]\n", nTotalCount);
	HLIB_CMD_Printf("*************************************************************\n");

	HLIB_VECTOR_Delete(pstVector);

	return HxCMD_OK;
}


#define ___MEMBER_FUNCTIONS___
HERROR SAMA_DBG_Init (void)
{
//	SAMA_Context_t		*pstContext = SAMA_GetContext();

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintTrackingStatus,
						(HCHAR *)"tracking",
						(HCHAR *)"print SAMALIB TRACKING tracking status",
						(HCHAR *)"tracking");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintSeriesMem,
						(HCHAR *)"seriesmem",
						(HCHAR *)"print SAMALIB TVA Series Set's memory status",
						(HCHAR *)"seriesmem");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintSeriesSet,
						(HCHAR *)"seriesset",
						(HCHAR *)"print SAMALIB TVA Series Set",
						(HCHAR *)"seriesset");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintSlot,
						(HCHAR *)"slot",
						(HCHAR *)"print SAMA slot status",
						(HCHAR *)"slot");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintSchedule,
						(HCHAR *)"sch",
						(HCHAR *)"print SAMA Schedules",
						(HCHAR *)"sch (optional:slot)");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintTimer,
						(HCHAR *)"timer",
						(HCHAR *)"print SAMA Timer",
						(HCHAR *)"timer");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_MakeRandomSchedule,
						(HCHAR *)"ransch",
						(HCHAR *)"make SAMA Schedule (TBR)",
						(HCHAR *)"ransch count(integer) uid(integer) uid(integer) uid(integer) ...");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_MakeSchedule,
						(HCHAR *)"make",
						(HCHAR *)"make SAMA Schedule ",
						(HCHAR *)"make uid(integer) type(3:ebr, 4:tbr...) starttime duration");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintSysTime,
							(HCHAR *)"systime",
							(HCHAR *)"print system time",
							(HCHAR *)"systime");


	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_CancelSchedule,
						(HCHAR *)"del",
						(HCHAR *)"delete SAMA Schedule",
						(HCHAR *)"del slotId");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_CancelAllSchedule,
						(HCHAR *)"delall",
						(HCHAR *)"delete ALL SAMA Schedule",
						(HCHAR *)"delall");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_UpdateSeries,
						(HCHAR *)"update_series",
						(HCHAR *)"update ALL TVA Series Set",
						(HCHAR *)"update_series");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_TrackSchedule,
						(HCHAR *)"track_schedule",
						(HCHAR *)"start tracking schedule",
						(HCHAR *)"track_schedule slot");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_TimerSchedule,
						(HCHAR *)"timer_schedule",
						(HCHAR *)"timer TVA schedule",
						(HCHAR *)"timer_schedule slot timer_type");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintRPCFunction,
						(HCHAR *)"print_rpc",
						(HCHAR *)"print SAMA rpc",
						(HCHAR *)"print_prc");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintEarliestSchedule,
						(HCHAR *)"print_esch",
						(HCHAR *)"print earliest schedule",
						(HCHAR *)"print_esch");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintSchRecInfo,
						(HCHAR *)"print_schrec",
						(HCHAR *)"print scheduled recording info",
						(HCHAR *)"print_schrec [yyyy.mm.dd-hh:mm:ss]");

	HLIB_CMD_RegisterWord(hCmdHandle, sama_dbg_PrintSeriesInfo,
						(HCHAR *)"print_series",
						(HCHAR *)"print series recording info",
						(HCHAR *)"print_series [CRID]");

	return ERR_OK;
}

#endif
