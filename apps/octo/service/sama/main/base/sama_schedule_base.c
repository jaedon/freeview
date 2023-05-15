/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_schedule_base.c
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
#include <oapi_types.h>
#include <sapi.h>
#include <hapi.h>

#include "sama_int.h"
#include <sama_lib.h>

/*******************************************************************/
/********************      Defines         *************************/
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




STATIC void sama_sch_base_OnDamaCurrentSeriesRecordDataReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxSchedule_t *		pstSchedule = (DxSchedule_t *)userdata;
	DxEvent_t			*Event;
	DAxIterator_t		iter;
	HUINT32						ulSeason;
	HUINT32						ulEpisodeNum;
	HUINT32						ulTotalNum;


	HBOOL			bFoundCurrentItem = FALSE;

	HxLOG_Trace();

	if(pstSchedule == NULL)
	{
		HxLOG_Error("pstSchedule is NULL \n");
		goto END_FUNC;
	}

	if (size == 0)
	{
		HxLOG_Error("size is 0 \n");
		bFoundCurrentItem = FALSE;
		goto END_FUNC;
	}



	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		Event = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (Event)
		{
			ulSeason = Event->extension.ipEpg.season;
			ulEpisodeNum = Event->extension.ipEpg.episodeNum;
			ulTotalNum = Event->extension.ipEpg.totalNum;
			bFoundCurrentItem = TRUE;

			DLIB_FreeType(iter.name, Event);
		}
		break;
	}

END_FUNC:
	if(bFoundCurrentItem == TRUE)
	{
		HxLOG_Debug("send SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED \n");
		SAMA_PostMessage(SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED, (HINT32)pstSchedule, ulSeason, ulEpisodeNum, ulTotalNum, 0, 0);
	}

	else
	{
		HxLOG_Error("Cannot find current item ! \n");
		HxLOG_Debug("send SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED\n");
		SAMA_PostMessage(SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}


	return;

}



STATIC void sama_sch_base_OnDamaNextSeriesRecordDataReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxEvent_t			*Event;
	DAxIterator_t		iter;

	HxList_t *		pList = NULL;
	HUINT32			ulSlot = 0;
	Sama_SameSeriesRecListCheckData_t *pstCheckData = (Sama_SameSeriesRecListCheckData_t *)userdata;
	DxSchedule_t *		pstSchedule = NULL;
	HERROR			hErr = ERR_FAIL;
	HBOOL			bExistSameSeriesItem = FALSE;

	HxLOG_Trace();

	if(pstCheckData == NULL)
	{
		HxLOG_Error("pstCheckData is NULL \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstSchedule = pstCheckData->pstSchedule;
	bExistSameSeriesItem = pstCheckData->bExistSameSeriesItem;

	if(pstSchedule == NULL)
	{
		HxLOG_Error("pstSchedule is NULL \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	ulSlot = pstSchedule->ulSlot;


	if (size == 0)
	{
		HxLOG_Error("size is 0 \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}



	// find next item.
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		Event = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (Event)
		{
			Sama_SeriesRecordEventInfo_t		*pstSeriesRecordEventInfo;
			pstSeriesRecordEventInfo = (Sama_SeriesRecordEventInfo_t *)SAMA_Malloc(sizeof(Sama_SeriesRecordEventInfo_t));

			pstSeriesRecordEventInfo->ulSlot = ulSlot;
			SAMA_MemCpy(&(pstSeriesRecordEventInfo->stEvent), Event, sizeof(DxEvent_t));

			HxLOG_Debug("appned event, event ID: %d \n", Event->eventId);
			pList = HLIB_LIST_Append(pList, (void *)pstSeriesRecordEventInfo);
		}
		DLIB_FreeType(iter.name, Event);
	}
	hErr = ERR_OK;

END_FUNC:

	if(pstCheckData)
	{
		SAMA_Free(pstCheckData);
		pstCheckData = NULL;
	}


	if(hErr == ERR_OK)
	{
		// pList의 free는 SAMA_SCHEDULE_BASE_MakeNextSeriesRecord() 에서.
		HxLOG_Debug("send SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM, ulSlot: %d, bExistSameSeriesItem: %d \n", ulSlot, bExistSameSeriesItem);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM, (HINT32)pstSchedule, (HINT32)pList, bExistSameSeriesItem, 0, 0, 0);
	}
	else
	{
		HxLOG_Debug("send SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, ulSlot: %d \n", ulSlot);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}

	return;

}

#ifdef SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST


STATIC void sama_sch_base_OnDamaSameSeriesRecordListDataReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{

	DAxIterator_t		iter;
	DxRecListData_t *pstTempData = NULL;
	HBOOL			 bFound = FALSE;
	Sama_SameSeriesRecListCheckData_t *pstCheckData = (Sama_SameSeriesRecListCheckData_t *)userdata;
	DxSchedule_t			*pstSchedule = NULL;
	HUINT32 				ulSeason = 0;
	HUINT32 				ulEpisodeNum = 0;
	HUINT32 				ulTotalNum = 0;

	HxLOG_Trace();

	if(pstCheckData == NULL)
	{
		HxLOG_Error("pstCheckData is NULL \n");
		bFound = FALSE;
		goto END_FUNC;
	}

	pstSchedule	= pstCheckData->pstSchedule;
	ulSeason	= pstCheckData->ulSeason;
	ulEpisodeNum = pstCheckData->ulEpisodeNum;
	ulTotalNum	= pstCheckData->ulTotalNum;


	if (size == 0)
	{
		HxLOG_Error("size is 0 \n");
		bFound = FALSE;
		goto END_FUNC;
	}


	// find next item.
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstTempData = (DxRecListData_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);

		if (pstTempData)
		{
			if(pstTempData->svcUId == pstCheckData->svcuid)
 			{
				if(pstTempData->stEvt.ulEpisodeNum == pstCheckData->ulEpisodeNum)
				{
					if((pstTempData->stEvt.ulSeason==0)
						||(pstTempData->stEvt.ulSeason == pstCheckData->ulSeason))
					{

						DLIB_FreeType(iter.name, pstTempData);

						bFound = TRUE;
						break;
					}
				}
			}
		}
		DLIB_FreeType(iter.name, pstTempData);
	}

END_FUNC:

	if(pstCheckData)
	{
		SAMA_Free(pstCheckData);
		pstCheckData = NULL;
	}

	HxLOG_Debug("send SAMAMESSAGE_CHECK_SAME_SERIES_RECORD_EXIST_RESULT, bFound: %d\n", bFound);
	SAMA_PostMessage(SAMAMESSAGE_CHECK_SAME_SERIES_RECORD_EXIST_RESULT, (HINT32)pstSchedule, (HINT32)ulSeason, (HINT32)ulEpisodeNum, (HINT32)ulTotalNum, bFound, 0);

}
#endif

STATIC HUINT8 *sama_sch_base_getSeriesRecordEpgType(DxEPG_Type_e eEpgType)
{
	switch(eEpgType)
	{
		ENUM_TO_STR(eDxEPG_TYPE_IP);
		ENUM_TO_STR(eDxEPG_TYPE_TVTV);


		default:
			HxLOG_Error("eEpgType: %d is not appropriate\n", eEpgType);
			return "unknown";
	}
}
STATIC HINT32  sama_sch_base_CompareSlot (const void *arg1, const void *arg2)
{
	if ((HUINT32)arg1 > (HUINT32)arg2)
	{
		return 1;
	}
	else if ((HUINT32)arg1 < (HUINT32)arg2)
	{
		return -1;
	}

	return 0;
}


STATIC void sama_sch_base_cbScheduleOneWay (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime)
{
	SAMA_PostMessage(SAMAMESSAGE_PROCESSONEWAYTIMER, ulSlot, eTimerType, utTime, 0, 0, 0);
}

STATIC void sama_sch_base_cbScheduleTwoWay (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime)
{
	SAMA_PostMessage(SAMAMESSAGE_PROCESSTWOWAYTIMER, ulSlot, eTimerType, utTime, 0, 0, 0);
}




#define ___GLOBAL_FUNCTIONS___

void SAMA_SCHEDULE_BASE_ProcessOneWayTimer (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime)
{
	DxSchedule_t		*pstSchedule = SAMA_SCHEDULE_GetSchedule(ulSlot);
	HERROR				 hErr;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

	HxLOG_Debug("ulSlot: %d, eTimerType: %d \n", ulSlot, eTimerType);

	if (NULL == pstSchedule)
	{
		HxLOG_Error("No Schedule with the Slot ID (%d)\n", ulSlot);
		return;
	}

	switch (eTimerType)
	{
	case eSAPI_TIMER_TYPE_Ready:
		pstSchedule->eStatus = DxRSVSTATUS_READY;
		break;

	case eSAPI_TIMER_TYPE_Start:
		pstSchedule->eStatus = DxRSVSTATUS_RUNNING;
		break;

	case eSAPI_TIMER_TYPE_Stop:
		pstSchedule->eStatus = (DxRSVREPEAT_ONCE == pstSchedule->eRepeat) ? DxRSVSTATUS_STOP : DxRSVSTATUS_WAITING;
		break;

	default:
		break;
	}

	// Call the timer execution:
	SAMA_PROTOCOL_NotifyTimeUp(ulSlot, eTimerType, pstSchedule);

	// Finish: 만약 Schedule이 Repeat가 아니라면 여기서 지워야 한다.
	if (SAMA_TIMER_CountRemainingTimer(ulSlot) == 0)
	{
		HUINT32				 ulSlot = pstSchedule->ulSlot;
		HBOOL				 bUpdated = FALSE;

		HxLOG_Debug("there is no remaining timer for ulSlot: %d   \n", ulSlot);
		hErr = SAMA_SCHEDULE_AdjustTime(pstSchedule, &bUpdated);

		HxLOG_Debug("hErr: %d, bUpdated: %d \n", hErr, bUpdated);
		if ((ERR_OK == hErr) && (TRUE == bUpdated))
		{
			pstSchedule->eStatus = DxRSVSTATUS_WAITING;
			hErr = SAMA_SCHEDULE_UpdateSchedule(pstSchedule->ulSlot, TRUE, pstSchedule, SAMA_GetVirtualRequestId(), TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SAMA_SCHEDULE_UpdateSchedule err: ulSlot(%d)\n", ulSlot);
			}
		}
		else
		{
			stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Completed;
			hErr = SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SAMA_SCHEDULE_RemoveSchedule err: ulSlot(%d)\n", ulSlot);
			}
		}
	}
	else
	{
		// DLIB을 통해 저장한다. // rsv status 가 변경...
		(void)DAPI_Set(DxNAMEOF(DxSchedule_t), (HUID)pstSchedule->ulSlot, (void*)pstSchedule, sizeof(DxSchedule_t));
		(void)DAPI_Sync(DxNAMEOF(DxSchedule_t));

	}
}

void SAMA_SCHEDULE_BASE_ProcessTwoWayTimer (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime)
{
	DxSchedule_t		*pstSchedule = SAMA_SCHEDULE_GetSchedule(ulSlot);
	HERROR				 hErr;

	SAMA_Context_t		*pstContext = SAMA_GetContext();
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

	HxLOG_Trace();

	if (NULL == pstSchedule)
	{
		HxLOG_Error("No Schedule with the Slot ID (%d)\n", ulSlot);
		return;
	}

	HxLOG_Error("Timer SlotID(%04d), eTimerType(%d), Time(%d), (%s) \n",
							ulSlot,
							eTimerType,
							utTime,
							SAMA_SCHEDULE_GetStringOfUnixTime(utTime));
	HxLOG_Error("Timer SlotID(%04d), eTimerType(%d), Time(%d), (%s) \n",
							ulSlot,
							eTimerType,
							HLIB_STD_GetSystemTime(),
							SAMA_SCHEDULE_GetStringOfUnixTime(HLIB_STD_GetSystemTime()));

	switch (eTimerType)
	{
	case eSAPI_TIMER_TYPE_Ready:
		pstSchedule->eStatus = DxRSVSTATUS_READY;
		break;

	case eSAPI_TIMER_TYPE_Start:
		pstSchedule->eStatus = DxRSVSTATUS_RUNNING;
		break;

	case eSAPI_TIMER_TYPE_Stop:
		pstSchedule->eStatus = (DxRSVREPEAT_ONCE == pstSchedule->eRepeat) ? DxRSVSTATUS_STOP : DxRSVSTATUS_WAITING;
		break;

	default:
		break;
	}

	HxLOG_Debug("ulSlot: %d, eTimerType: %d \n", ulSlot, eTimerType);


	// 기본적으로 sapi에서 conflict check 를 다 해서 들어온 reservation item 이지만, 예외상황 처리를 위해 sama에서 ready 시에 한번 더 check 한다.
	// 예를 들어 시간이 valid할 경우의 이미 걸어놓은 예약과, 재부팅 후 시간이 invalid 할 때의 instant rec이 시간이 valid해지면서 schedule update가 일어난 후 conflict 가 생길수 있다.
	if(eSAPI_TIMER_TYPE_Ready == eTimerType)
	{
		SAMA_SCHEDULE_TreeValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));

		// Conflict Result를 일단 비워야 한다.
		(void)SAMALIB_CFLT_ClearConflictResult(&(pstContext->stSamaConflictCheck.stConflictResult));
		pstContext->stSamaConflictCheck.bScheduleChange = TRUE; // 여기서는 무조건 TRUE. 왜냐하면 이미 sama schedule list 에 있기 때문.
		(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstContext->stSamaConflictCheck), pstSchedule);

		// 필요 없음? 아직 확인 안해봄.
		//(void)SAMA_SCHEDULE_RemovePadding(pstContext, FALSE, eSAPI_PADDINGRMV_StartPad | eSAPI_PADDINGRMV_StopPad);

		HxLOG_Debug("stSamaConflictCheck.stConflictResult.eType: %d \n", pstContext->stSamaConflictCheck.stConflictResult.eType);

		if (eSAPI_CONFLICT_TYPE_None != pstContext->stSamaConflictCheck.stConflictResult.eType)
		{
			HxLOG_Error("Conflict occurred!!!\n");

			switch (pstContext->stSamaConflictCheck.stSpecOption.eSpec)
			{
			case eDxSchedule_Spec_Base:
			default:
				HxLOG_Debug("tired schedule slot: %d \n", pstContext->stSamaConflictCheck.stConflictResult.stTriedSchedule.ulSlot);
				(void)SAMA_PROTOCOL_NotifyConflictAddTried(&(pstContext->stSamaConflictCheck.stConflictResult.stTriedSchedule), pstContext->stSamaConflictCheck.stConflictResult.eType, pstContext->stSamaConflictCheck.stConflictResult.ulNumConflict, pstContext->stSamaConflictCheck.stConflictResult.pstConflictArray, SAMA_GetVirtualRequestId());
				break;
			}
			goto END_FUNC;
		}
	}
	// Series rec.
	else if (eSAPI_TIMER_TYPE_CheckSeriesRecordReady == eTimerType)
	{
		if (SAMA_SCHEDULE_IsSeriesRecord(pstSchedule) == TRUE)
		{
			DxSchedule_t	*pstScheduleForSeriesRecord = NULL;
			pstScheduleForSeriesRecord = SAMA_Calloc(sizeof(DxSchedule_t));
			if (pstScheduleForSeriesRecord != NULL)
			{
				// free는 받는 애가.
				SAMA_MemCpy(pstScheduleForSeriesRecord, pstSchedule, sizeof(DxSchedule_t));
				SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD, (HINT32)pstScheduleForSeriesRecord, 0, 0, 0, 0, 0);
			}
		}
		else
		{
			HxLOG_Debug("do not check series record bacuase pstSchedule->eRsvType: %d[%s]", pstSchedule->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstSchedule->eRsvType));
		}
	}
#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
	else if(eSAPI_TIMER_TYPE_PaddingStartReady == eTimerType
		|| eSAPI_TIMER_TYPE_PaddingStopReady == eTimerType)
	{
		HBOOL		bUpdate = FALSE;
		sama_PaddingRemoval_e		eUpdateFlag = eSAPI_PADDINGRMV_None;

		HxLOG_Debug("eTimerType: %d, check padding!!! \n", eTimerType);
		SAMA_SCHEDULE_TreeValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));

		// Conflict Result를 일단 비워야 한다.
		(void)SAMALIB_CFLT_ClearConflictResult(&(pstContext->stSamaConflictCheck.stConflictResult));
		pstContext->stSamaConflictCheck.bScheduleChange = TRUE; // 여기서는 무조건 TRUE. 왜냐하면 이미 sama schedule list 에 있기 때문.
		(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstContext->stSamaConflictCheck), pstSchedule);

		if(eSAPI_TIMER_TYPE_PaddingStartReady == eTimerType)
		{
			eUpdateFlag = eSAPI_PADDINGRMV_StartPad;
		}
		else
		{
			eUpdateFlag = eSAPI_PADDINGRMV_StopPad;
		}

		bUpdate = SAMA_SCHEDULE_RemovePadding(pstContext, FALSE, eUpdateFlag);
#if defined(CONFIG_DEBUG)
		HxLOG_Debug("bUpdate: %d \n", bUpdate);
		HxLOG_Print("print all schedules and timer \n");
		SAMA_SCHEDULE_PrintAllSchedules(HLIB_CMD_Printf);
		SAMA_TIMER_PrintAllTimer(HLIB_CMD_Printf);
#endif

		HxLOG_Debug("stSamaConflictCheck.stConflictResult.eType: %d \n", pstContext->stSamaConflictCheck.stConflictResult.eType);

		// Start Padding / Stop Padding shall be informed also...
		//goto END_FUNC;
	}
#endif


	// Call the timer execution:
	SAMA_PROTOCOL_NotifyTimeUp(ulSlot, eTimerType, pstSchedule);


END_FUNC:
	// Finish: 만약 Schedule이 Repeat가 아니라면 여기서 지워야 한다.
	if (SAMA_TIMER_CountRemainingTimer(ulSlot) == 0)
	{
		HBOOL				 bUpdated = FALSE;

		hErr = SAMA_SCHEDULE_AdjustTime(pstSchedule, &bUpdated);
		if ((ERR_OK == hErr) && (TRUE == bUpdated))
		{

			pstSchedule->eStatus = DxRSVSTATUS_WAITING;
			hErr = SAMA_SCHEDULE_UpdateSchedule(pstSchedule->ulSlot, TRUE, pstSchedule, SAMA_GetVirtualRequestId(), TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SAMA_SCHEDULE_UpdateSchedule err: ulSlot(%d)\n", pstSchedule->ulSlot);
			}
		}
		else
		{
			stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Completed;
			hErr = SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SAMA_SCHEDULE_RemoveSchedule err: ulSlot(%d)\n", pstSchedule->ulSlot);
			}
		}
	}
	else
	{
		// DLIB을 통해 저장한다. // rsv status 가 변경...
		(void)DAPI_Set(DxNAMEOF(DxSchedule_t), (HUID)pstSchedule->ulSlot, (void*)pstSchedule, sizeof(DxSchedule_t));
		(void)DAPI_Sync(DxNAMEOF(DxSchedule_t));

	}

}


HBOOL SAMA_SCHEDULE_BASE_KeepRemoveSchedule (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, DxSchedule_RemovedReason_e eRemoveReason)
{
	SAPI_ActionType_e	eActionType = eSAPI_ACTION_TYPE_None;

	if(pstContext == NULL || pstSchedule == NULL)
	{
		HxLOG_Error("pstContext == NULL || pstSchedule == NULL \n");
		return FALSE;
	}


	eActionType = SAMALIB_CFLT_GetSapiActionTypeBySchedule(pstSchedule);

	HxLOG_Debug("pstContext->bSama_ScheduleLoaded: %d, pstContext->bSama_PvrDeviceMounted: %d, eActionType: %d, eRemoveReason: %d \n", pstContext->bSama_ScheduleLoaded, pstContext->bSama_PvrDeviceMounted, eActionType, eRemoveReason);
	if( (pstContext->bSama_ScheduleLoaded == TRUE)
		&& (pstContext->bSama_PvrDeviceMounted == FALSE)
		&& (eSAPI_ACTION_TYPE_Record == eActionType)
		&& (DxSchedule_RemovedReason_Expired == eRemoveReason || DxSchedule_RemovedReason_NoPvrDevice == eRemoveReason))
	{
		return TRUE;
	}

	return FALSE;
}


HBOOL SAMA_SCHEDULE_BASE_IsSeriesRecord (DxSchedule_t *pstSchedule)
{
	if(pstSchedule == NULL)
	{
		HxLOG_Error("pstSchedule is NULL ! \n");
		return FALSE;
	}

	if(pstSchedule->eEpgType != eDxEPG_TYPE_IP && pstSchedule->eEpgType != eDxEPG_TYPE_TVTV)
	{
		HxLOG_Debug("pstSchedule->eEpgType(%d) is wrong ! \n", pstSchedule->eEpgType);
		return FALSE;
	}

	if(DxRSVTYPE_RECORD_SERIES_EBR == pstSchedule->eRsvType
		&& 0 != pstSchedule->uExtInfo.stRec.ulSeriesId)
	{
		return TRUE;
	}

	return FALSE;

}

HERROR SAMA_SCHEDULE_BASE_GetNextSeriesRecord (DxSchedule_t *pstSchedule, HUINT32 ulSeason, HUINT32 ulEpisodeNum, HUINT32 ulTotalNum, HBOOL bExistSameSeriesItem)
{
	HCHAR				query[256];
	HUINT32 				id;
	HUINT32			ulSeriesId;
	HUINT32			ulEndTime;
	HERROR			hErr = ERR_FAIL;
#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
	HUINT32 				svcuid = pstSchedule->ulSvcUid;
#else
	HCHAR	tripleid[32];
#endif
	Sama_SameSeriesRecListCheckData_t		*pstCheckData = NULL;

	ulSeriesId = pstSchedule->uExtInfo.stRec.ulSeriesId;

	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &ulEndTime);
	ulEndTime += pstSchedule->ulDuration;

#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
	if(ulEpisodeNum != 0)
	{
		if(ulSeason != 0)
		{
			// 정상 녹화된 에피소드 제외, 동일 svcuid, 동일 series, 현재 이벤트 End Time 이후
			HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE episode NOT IN  (SELECT episode FROM %s WHERE status=%d AND season=%d AND seriesID=%d) AND svcuid=%d AND seriesid=%d AND start_time>=%d ORDER BY start_time ASC"
				, sama_sch_base_getSeriesRecordEpgType(pstSchedule->eEpgType)
				, DxNAMEOF(DxRecListData_t)
				, eDxREC_STS_RECORDED
				, ulSeason
				, ulSeriesId
				, svcuid
				, ulSeriesId, ulEndTime
			);
		}
		else
		{
			// episodeNum만 가지고 비교.
			HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE episode NOT IN  (SELECT episode FROM %s WHERE status=%d AND seriesID=%d) AND svcuid=%d AND seriesid=%d AND start_time>=%d ORDER BY start_time ASC"
				, sama_sch_base_getSeriesRecordEpgType(pstSchedule->eEpgType)
				, DxNAMEOF(DxRecListData_t)
				, eDxREC_STS_RECORDED
				, ulSeriesId
				, svcuid
				, ulSeriesId, ulEndTime
			);

		}
	}
	else
	{
		// 동일 svcuid, 동일 series, 현재 이벤트 End Time 이후
		HxSTD_PrintToStrN(query, 256
			, "SELECT * FROM %s WHERE svcuid=%d AND seriesid=%d AND start_time>=%d ORDER BY start_time ASC"
			, sama_sch_base_getSeriesRecordEpgType(pstSchedule->eEpgType)
			, svcuid
			, ulSeriesId, ulEndTime
		);
	}
#else

	HxSTD_PrintToStrN(tripleid, 32, "%x.%x.%x", pstSchedule->stTripleId.usOnId, pstSchedule->stTripleId.usTsId, pstSchedule->stTripleId.usSvcId);

	if(ulEpisodeNum != 0)
	{
		if(ulSeason != 0)
		{
			// 정상 녹화된 에피소드 제외, 동일 triple id, 동일 series, 현재 이벤트 End Time 이후
			HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE episode NOT IN  (SELECT episode FROM %s WHERE status=%d AND season=%d AND seriesID=%d) AND tripleid=\'%s\' AND seriesid=%d AND start_time>=%d ORDER BY start_time ASC"
				, sama_sch_base_getSeriesRecordEpgType(pstSchedule->eEpgType)
				, DxNAMEOF(DxRecListData_t)
				, eDxREC_STS_RECORDED
				, ulSeason
				, ulSeriesId
				, tripleid
				, ulSeriesId, ulEndTime
			);
		}
		else
		{
			// episodeNum만 가지고 비교.
			HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE episode NOT IN  (SELECT episode FROM %s WHERE status=%d AND seriesID=%d) AND tripleid=\'%s\' AND seriesid=%d AND start_time>=%d ORDER BY start_time ASC"
				, sama_sch_base_getSeriesRecordEpgType(pstSchedule->eEpgType)
				, DxNAMEOF(DxRecListData_t)
				, eDxREC_STS_RECORDED
				, ulSeriesId
				, tripleid
				, ulSeriesId, ulEndTime
			);


		}
	}
	else
	{
		// 동일 triple Id, 동일 series, 현재 이벤트 End Time 이후
		HxSTD_PrintToStrN(query, 256
			, "SELECT * FROM %s WHERE tripleid=\'%s\' AND seriesid=%d AND start_time>=%d ORDER BY start_time ASC"
			, sama_sch_base_getSeriesRecordEpgType(pstSchedule->eEpgType)
			, tripleid
			, ulSeriesId, ulEndTime
		);
	}

#endif
	pstCheckData = (Sama_SameSeriesRecListCheckData_t*)SAMA_Calloc(sizeof(Sama_SameSeriesRecListCheckData_t));
	if(pstCheckData == NULL)
	{
		HxLOG_Error("pstCheckData is NULL ! \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstCheckData->pstSchedule = pstSchedule;
	pstCheckData->bExistSameSeriesItem = bExistSameSeriesItem;


	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)sama_sch_base_OnDamaNextSeriesRecordDataReceived, (void*)pstCheckData);
	HxLOG_Error("DAPI_Query() id: %u \n", id);

	if(id == 0)
	{
		HxLOG_Error("Dapi Query Error!! ->[%s]\n",query);
		if(pstCheckData)
		{
			SAMA_Free(pstCheckData);
			pstCheckData = NULL;
		}
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	hErr = ERR_OK;
END_FUNC:
	if(hErr == ERR_FAIL)
	{
		// 다음 이벤트 쿼리 실패하면...뭐...동작 종료해야지...
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}
	return hErr;

}


HERROR SAMA_SCHEDULE_BASE_FindCurrentSeriesRecordInfo (DxSchedule_t *pstSchedule)
{
	HCHAR		query[256];
	HUINT32		id;
	HERROR		hErr = ERR_FAIL;

	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule is NULL ! \n");
		hErr = ERR_FAIL;
		// 어차피 free 할 것도 없지만 event는 보내 놓는다.
		goto END_FUNC;
	}

#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
	HxSTD_PrintToStrN(query, 256
		, "SELECT * FROM %s WHERE svcuid=%d AND seriesid=%d AND eventid=%d"
		, sama_sch_base_getSeriesRecordEpgType(pstSchedule->eEpgType)
		, pstSchedule->ulSvcUid
		, pstSchedule->uExtInfo.stRec.ulSeriesId
		, pstSchedule->uExtInfo.stRec.nEvtId
	);
#else
	{
		HCHAR	tripleid[32];
		HxSTD_PrintToStrN(tripleid, 32, "%x.%x.%x", pstSchedule->stTripleId.usOnId, pstSchedule->stTripleId.usTsId, pstSchedule->stTripleId.usSvcId);
		HxSTD_PrintToStrN(query, 256
			, "SELECT * FROM %s WHERE tripleid=\'%s\' AND seriesid=%d AND eventid=%d"
			, sama_sch_base_getSeriesRecordEpgType(pstSchedule->eEpgType)
			, tripleid
			, pstSchedule->uExtInfo.stRec.ulSeriesId
			, pstSchedule->uExtInfo.stRec.nEvtId
		);
	}
#endif


	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");
	id = DAPI_Query(query, (DAPI_Getter)sama_sch_base_OnDamaCurrentSeriesRecordDataReceived, (void*)pstSchedule);
	HxLOG_Error("DAPI_Query() id: %u \n", id);

	hErr = ERR_OK;

END_FUNC:

	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("Cannot find current item ! \n");
		HxLOG_Debug("send SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL\n");
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}

	return hErr;
}

STATIC void sama_schedule_base_CbFreeSeriesRecordEventlist (void *pvData)
{
	Sama_SeriesRecordEventInfo_t		*pstSeriesRecordEventInfo = (Sama_SeriesRecordEventInfo_t*)pvData;

	if(pstSeriesRecordEventInfo != NULL)
	{
		SAMA_Free(pstSeriesRecordEventInfo);
		pstSeriesRecordEventInfo = NULL;
	}

	return;
}
#ifdef SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST
HERROR SAMA_SCHEDULE_BASE_CheckSameSeriesRecordExist (DxSchedule_t *pstSchedule, HUINT32 ulSeason, HUINT32 ulEpisodeNum, HUINT32 ulTotalNum)
{
	HCHAR				query[256];
	HUINT32 				id;
	HUINT32 		ulSeriesId;
	HUINT32 		ulEndTime;
	Sama_SameSeriesRecListCheckData_t		*pstCheckData = NULL;


	ulSeriesId = pstSchedule->uExtInfo.stRec.ulSeriesId;

	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &ulEndTime);
	ulEndTime += pstSchedule->ulDuration;

	if(ulEpisodeNum != 0)
	{
		if(ulSeason != 0)
		{
			// 정상 녹화된 에피소드 제외, 동일 svcuid, 동일 series
			HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE status=%d AND season=%d AND episode=%d AND seriesID=%d"
				, DxNAMEOF(DxRecListData_t)
				, eDxREC_STS_RECORDED
				, ulSeason
				, ulEpisodeNum
				, ulSeriesId
			);

		}
		else
		{
			// episodeNum만 가지고 비교.
			HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE status=%d AND episode=%d AND seriesID=%d"
				, DxNAMEOF(DxRecListData_t)
				, eDxREC_STS_RECORDED
				, ulEpisodeNum
				, ulSeriesId
			);

		}
	}
	else
	{
		// 정보 부재, 다음 step 으로 진행한다.
		HxLOG_Debug("ulEpisodeNum == 0 \n");
		HxLOG_Debug("send SAMAMESSAGE_CHECK_SAME_SERIES_RECORD_EXIST_RESULT, with bIsExistSameSeries FALSE\n");
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SAME_SERIES_RECORD_EXIST_RESULT, (HINT32)pstSchedule, ulSeason, ulEpisodeNum, ulTotalNum, FALSE, 0);
		return ERR_OK;
	}

	pstCheckData = (Sama_SameSeriesRecListCheckData_t*)SAMA_Calloc(sizeof(Sama_SameSeriesRecListCheckData_t));
	if(pstCheckData == NULL)
	{
		HxLOG_Error("pCheckData is NULL ! \n");
		return ERR_FAIL;
	}

	pstCheckData->pstSchedule = pstSchedule;
	pstCheckData->svcuid = pstSchedule->ulSvcUid;
	HxSTD_PrintToStrN(pstCheckData->tripleid, 32, "%x.%x.%x", pstSchedule->stTripleId.usOnId, pstSchedule->stTripleId.usTsId, pstSchedule->stTripleId.usSvcId);

	pstCheckData->ulSeason = ulSeason;
	pstCheckData->ulEpisodeNum = ulEpisodeNum;
	pstCheckData->ulTotalNum = ulTotalNum;

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)sama_sch_base_OnDamaSameSeriesRecordListDataReceived, (void *)pstCheckData);
	HxLOG_Error("DAPI_Query() id: %u \n", id);
	if(id == 0)
	{
		HxLOG_Error("Dapi Query Error!! ->[%s]\n",query);
		if(pstCheckData)
		{
			SAMA_Free(pstCheckData);
			pstCheckData = NULL;
		}
		return ERR_FAIL;
	}



	return ERR_OK;
}
#endif



HERROR SAMA_SCHEDULE_BASE_MakeNextSeriesRecord (DxSchedule_t *pstSchedule, HxList_t *pSeriesRecordNextEventList, HBOOL bExistSameSeriesItem)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();
	DxSchedule_t *			pstScheduleNext = NULL;
	HxList_t				*pstListItem;
	HERROR				hErr = ERR_FAIL;

	HxLOG_Trace();

	if(pstSchedule == NULL)
	{
		HxLOG_Error("pstSchedule == NULL \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstScheduleNext = SAMA_Calloc(sizeof(DxSchedule_t));
	if(pstScheduleNext ==NULL)
	{
		HxLOG_Error("Memory Alloc Failed. \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	// 내용이 거의 비슷하니 일단 복사해두고.
	HLIB_STD_MemCpySafe(pstScheduleNext, sizeof(DxSchedule_t), pstSchedule, sizeof(DxSchedule_t));

	hErr = SAMA_SCHEDULE_GetEmptySlotForSama(&(pstScheduleNext->ulSlot));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("There is no empty slot for sama!\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	// 현재 menu의 padding으로.
	hErr = HAPI_GetRegistryInt("startpadding", (HUINT32 *)&pstScheduleNext->uExtInfo.stRec.ulPreOffsetTime);
	hErr |= HAPI_GetRegistryInt("endpadding", (HUINT32 *)&pstScheduleNext->uExtInfo.stRec.ulPostOffsetTime);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("Get padding time error \n");
	}

	// schedule list 를 다 가지고 와서.
	if (NULL != pstContext->stSamaConflictCheck.pstScheduleList)
	{
		pstContext->stSamaConflictCheck.pstScheduleList = HLIB_LIST_RemoveAllFunc (pstContext->stSamaConflictCheck.pstScheduleList, SAMA_Free_CB);
	}
	SAMA_SCHEDULE_TreeValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));

	HxLOG_Debug("check pSeriesRecordNextEventList item start\n");
	for (pstListItem = pSeriesRecordNextEventList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		Sama_SeriesRecordEventInfo_t		*pstSeriesRecordEventInfo = (Sama_SeriesRecordEventInfo_t *)HLIB_LIST_Data(pstListItem);

		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(pstSeriesRecordEventInfo->stEvent.startTime, &(pstScheduleNext->stStartTime));
		pstScheduleNext->ulDuration = pstSeriesRecordEventInfo->stEvent.duration;
		pstScheduleNext->eStatus = DxRSVSTATUS_WAITING;
		pstScheduleNext->uExtInfo.stRec.nEvtId = pstSeriesRecordEventInfo->stEvent.eventId;
		pstScheduleNext->bExceptConflict = FALSE;
		pstScheduleNext->bRemoved = FALSE;
		SAMA_MemSet(&pstScheduleNext->stRemovedInfo, 0x00, sizeof(DxSchedule_RemovedInfo_t));

#ifdef SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST
		HxLOG_Debug("bExistSameSeriesItem: %d \n", bExistSameSeriesItem);

		if(bExistSameSeriesItem == TRUE)
		{
			DxSchedule_t	*pstScheduleForSeriesRecord = NULL;
			pstScheduleForSeriesRecord = SAMA_Calloc(sizeof(DxSchedule_t));
			if(pstScheduleForSeriesRecord != NULL)
			{
				// free는 받는 애가.
				SAMA_MemCpy(pstScheduleForSeriesRecord, pstScheduleNext, sizeof(DxSchedule_t));
				HxLOG_Debug("send SAMAMESSAGE_CHECK_SERIES_RECORD with new Schedule\n");
				// 다시 돌아 이 함수를 탔을 때, SAMA_SCHEDULE_GetSchedule()이 성공할 수 있도록.
				pstScheduleForSeriesRecord->ulSlot = pstSchedule->ulSlot;
				SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD, (HINT32)pstScheduleForSeriesRecord, 0, 0, 0, 0, 0);
			}

			break ;
		}
		else
#endif
		{
			// conflict check.
			HxLOG_Debug("Check conflict \n");

			//TODO: pstContext->stSamaConflictCheck.bScheduleChange = (sapi_FindScheduleFromCache(pstInst, pstSchedule->ulSlot) != NULL) ? TRUE : FALSE;
			(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstContext->stSamaConflictCheck), pstScheduleNext);

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
					HxLOG_Debug("add schedule on slot: %d, bExistSameSeriesItem: %d, it's releated to slot: %d \n", pstScheduleNext->ulSlot, bExistSameSeriesItem, pstSchedule->ulSlot);
					(void)SAMA_SCHEDULE_AddSchedule(pstScheduleNext->ulSlot, pstScheduleNext, TRUE, SAMA_GetVirtualRequestId());
					break;
				}
				break ;
			}
		}

	}
	HxLOG_Debug("check pSeriesRecordNextEventList item done\n");

	HLIB_LIST_RemoveAllFunc(pSeriesRecordNextEventList, sama_schedule_base_CbFreeSeriesRecordEventlist);

	hErr = ERR_OK;

END_FUNC:

	HxLOG_Debug("send SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE \n");
	SAMA_PostMessage(SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE, (HINT32)pstSchedule, 0, 0, 0, 0, 0);

	if(pstScheduleNext)
	{
		SAMA_Free(pstScheduleNext);
		pstScheduleNext = NULL;
	}
	return hErr;

}

HERROR SAMA_SCHEDULE_BASE_InsertScheduleToTimer (DxSchedule_t *pstSchedule)
{
	UNIXTIME			 utCurrTime;
	UNIXTIME			 utTime;
	HERROR				 hErr = ERR_FAIL;
	UNIXTIME			 utReady;
	UNIXTIME			 utStartTime;
	UNIXTIME			 utEndTime;
	UNIXTIME			 utSeriesRecCheckTime;

	if (NULL == pstSchedule)					{ return ERR_FAIL; }

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if ((SAMA_SCHEDULE_IsScheduleVolatile(pstSchedule) != TRUE) &&
		(utCurrTime < SAMA_TIME_AVAIL_MIN))
	{
		HxLOG_Debug("VK_CLOCK_GetTime err:\n");
		return ERR_FAIL;
	}

	hErr = HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("HLIB_DATETIME_ConvertDateTimeToUnixTime err:\n");
		return ERR_FAIL;
	}

	// 완전히 동시시간으로 같은 Slot을 넣으면 Timer 처리시 완전 삭제로 판단할 수 있다
	utReady = (pstSchedule->eRsvReady > 1) ? (UNIXTIME)pstSchedule->eRsvReady : 2;

	switch (pstSchedule->eRsvType)
	{
	// Two-way based reservation: Ready, Start, Stop:
	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_WATCHING_TBR:
	case DxRSVTYPE_WATCHING_SERIES_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
	case DxRSVTYPE_ACTIVATION_TIME:
	case DxRSVTYPE_DESC_REC_STANDBY:
		SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utTime, &utStartTime, &utEndTime);
		HxLOG_Debug("pstSchedule->bPaused: %d, utCurrTime: %ld, utStartTime: %ld, utEndTime: %ld \n", pstSchedule->bPaused, utCurrTime, utStartTime, utEndTime);
		if (utCurrTime < utStartTime)
		{
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utStartTime - utReady, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utStartTime, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utEndTime - SAMA_PADDING_STOP_TIME, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
		}
		else
		{
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utStartTime - utReady, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utStartTime, sama_sch_base_cbScheduleTwoWay, TRUE);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utEndTime - SAMA_PADDING_STOP_TIME, sama_sch_base_cbScheduleTwoWay, TRUE);
			pstSchedule->bPaused = TRUE;
		}


		break;

	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_SERIES_EBR:
		SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utTime, &utStartTime, &utEndTime);
		HxLOG_Debug("pstSchedule->bPaused: %d, utCurrTime: %ld, utStartTime: %ld, utEndTime: %ld \n", pstSchedule->bPaused, utCurrTime, utStartTime, utEndTime);

#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
		if (utCurrTime < utStartTime - utReady - SAMA_PADDING_READY_TIME)
		{
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_PaddingStartReady, pstSchedule->eRsvType, utStartTime - utReady - SAMA_PADDING_READY_TIME, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utStartTime - utReady, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utStartTime, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_PaddingStopReady, pstSchedule->eRsvType, utTime + pstSchedule->ulDuration - SAMA_PADDING_READY_TIME, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utEndTime - SAMA_PADDING_STOP_TIME, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
		}
		else
		{
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_PaddingStartReady, pstSchedule->eRsvType, utCurrTime, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utCurrTime + SAMA_PADDING_READY_TIME, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utCurrTime + utReady + SAMA_PADDING_READY_TIME, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_PaddingStopReady, pstSchedule->eRsvType, utTime + pstSchedule->ulDuration - SAMA_PADDING_READY_TIME, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utEndTime - SAMA_PADDING_STOP_TIME, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);

		}

		if(SAMA_SCHEDULE_IsSeriesRecord(pstSchedule) == TRUE)
		{
			hErr = SAMA_SCHEDULE_GetSeriesRecordCheckTime(pstSchedule, &utSeriesRecCheckTime);
			if(hErr == ERR_OK)// 어차피 start time에도 check 하므로.
			{
				// 최소한 start time에는 한번 날려줘야 함. start time시점에 없으면 SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST 를 start time 보다 늦게 타게 되면 불완전한 상태의 같은 item이 생길 수 있어요.
				SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_CheckSeriesRecordReady, pstSchedule->eRsvType, utStartTime, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
				SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_CheckSeriesRecordReady, pstSchedule->eRsvType, utSeriesRecCheckTime, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			}
		}
#else
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utStartTime - utReady, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utStartTime, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utEndTime - SAMA_PADDING_STOP_TIME, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);

		if(SAMA_SCHEDULE_IsSeriesRecord(pstSchedule) == TRUE)
		{
			hErr = SAMA_SCHEDULE_GetSeriesRecordCheckTime(pstSchedule, &utSeriesRecCheckTime);
			if(hErr == ERR_OK)// 어차피 start time에도 check 하므로.
			{
				// 최소한 start time에는 한번 날려줘야 함. start time시점에 없으면 SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST 를 start time 보다 늦게 타게 되면 불완전한 상태의 같은 item이 생길 수 있어요.
				SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_CheckSeriesRecordReady, pstSchedule->eRsvType, utStartTime, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
				SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_CheckSeriesRecordReady, pstSchedule->eRsvType, utSeriesRecCheckTime, sama_sch_base_cbScheduleTwoWay, pstSchedule->bPaused);
			}
		}
#endif
		break;





	case DxRSVTYPE_RECORD_INSTANT:
	case DxRSVTYPE_RECORD_DELAYED:
	case DxRSVTYPE_NETSTREAMING_LIVE: // TODO: 위치 고민.
	case DxRSVTYPE_NETSTREAMING_FILE:
		SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utTime, &utStartTime, &utEndTime);
		HxLOG_Debug("pstSchedule->bPaused: %d, utCurrTime: %ld, utStartTime: %ld, utEndTime: %ld, eStatus: %d \n", pstSchedule->bPaused, utCurrTime, utStartTime, utEndTime, pstSchedule->eStatus);

		if(pstSchedule->eStatus == DxRSVSTATUS_RUNNING)
		{
			;
			HxLOG_Error("it's already running schedule. so do not add start timer. \n");
		}
		else
		{
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utStartTime, sama_sch_base_cbScheduleTwoWay, FALSE);
		}
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utEndTime - 2, sama_sch_base_cbScheduleTwoWay, FALSE);
		break;

	// One-way based reservation: Ready, Start
	case DxRSVTYPE_POWER_ON:
	case DxRSVTYPE_POWER_OFF:
	case DxRSVTYPE_REGULAR_OTA:
	case DxRSVTYPE_EPG_GATHERING:
	case DxRSVTYPE_EMM:
	case DxRSVTYPE_UPDATE_TASK:
	case DxRSVTYPE_ASO_NOTIFY:
 	case DxRSVTYPE_MYEPG_UPDATE:
	case DxRSVTYPE_NET_UPDATE:
	case DxRSVTYPE_IP_EPG_GATHERING:
	case DxRSVTYPE_IP_REMOTE_PROGRAMMING:
	case DxRSVTYPE_TVTVEPG_TIMER:
	case DxRSVTYPE_CHECK_SATRECORD_TIMER:
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utTime - utReady, sama_sch_base_cbScheduleOneWay, FALSE);
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utTime, sama_sch_base_cbScheduleOneWay, FALSE);
		break;

	default:
		HxLOG_Error("RsvType(%d) not supported:\n", pstSchedule->eRsvType);
		return ERR_FAIL;
	}

	return hErr;
}

HUINT32		SAMA_SCHEDULE_BASE_CountSeries (DxSchedule_t *pstSchedule)
{
	HUINT32			ulSeriesCnt = 0;
	HUINT32 		i = 0;
	HERROR			hErr = ERR_OK;

	HUINT32 		ulNumSch;
	DxSchedule_t	*pstSchArray = NULL;
	DxSchedule_t	*pstSchItem = NULL;

	hErr = SAMA_SCHEDULE_GetAllSchedules(&ulNumSch, &pstSchArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SAMA_SCHEDULE_GetAllSchedules failed. \n");
		goto END_FUNC;
	}

	for (i=0; i<ulNumSch; i++)
	{
		pstSchItem = &pstSchArray[i];
		if ( (pstSchItem == NULL)
			||(pstSchItem->eRsvType != DxRSVTYPE_RECORD_SERIES_EBR))
		{
			continue;
		}

		// ignore removed item
		if (pstSchItem->bRemoved == TRUE)
		{
			continue;
		}

		// ignore different seires or different service
		if ( (pstSchItem->uExtInfo.stRec.ulSeriesId != pstSchedule->uExtInfo.stRec.ulSeriesId)
			||(pstSchItem->ulSvcUid != pstSchedule->ulSvcUid))
		{
			continue;
		}

		// ignore running schedule
		if (pstSchItem->eStatus >= DxRSVSTATUS_RUNNING)
		{
			continue;
		}

		++ulSeriesCnt;
	}

END_FUNC:
	if (pstSchArray != NULL)
	{
		SAMA_Free(pstSchArray);
	}

	return ulSeriesCnt;
}




