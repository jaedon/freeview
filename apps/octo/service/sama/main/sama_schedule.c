/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_schedule.c
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
#include <hapi.h>
#include <papi.h>
#include <sapi.h>

#include "sama_int.h"
#include <sama_lib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SAMA_MAX_SLOT					2048

#define SAMA_SEC_PER_DAY				(60 * 60 * 24)
#define SAMA_SEC_PER_WEEK				(SAMA_SEC_PER_DAY * 7)
#define SAMA_SEC_PER_MONTH				(SAMA_SEC_PER_DAY * 30)			// 어디까지나 빠른 계산을 위한 단위이므로 실제 정확해야 할 이유가 전혀 없다.
#define SAMA_SEC_PER_YEAR				(SAMA_SEC_PER_DAY * 256)		// 어디까지나 빠른 계산을 위한 단위이므로 실제 정확해야 할 이유가 전혀 없다.

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSAMA_SLOTSTATE_Empty				= 0,	// 비어 있고, 'Available Slot'으로써 타 process의 SAPI에 잡혀 있지도 않은 slot
	eSAMA_SLOTSTATE_Scheduled			= 1,	// 비어 있지 않은 Slot
	eSAMA_SLOTSTATE_ReservedBySapi		= 2,		// 비어 있지만 특정 process의 SAPI 에 Available Slot 으로 할당된 slot
	eSAMA_SLOTSTATE_ReservedBySama	= 3		// 비어 있지만 Sama에 의해 할당. 특수한 경우 (series rec 등 ) sama에서 직접 schedule 걸 경우 사용.
} samaSlotState_e;

typedef struct
{
	samaSlotState_e		 eState;
	HINT32				 nClientHandle;
	HINT32				 nPid;
} samaSlotSatus_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
//STATIC HBOOL			 s_bSama_ScheduleLoaded = FALSE;
//STATIC HBOOL			 s_bSama_PvrDeviceMounted = FALSE;

STATIC HxTREE_t			*s_pstSama_ScheduleTree = NULL;

STATIC HUINT32			 s_ulSama_MaxSlotId = 0;
STATIC samaSlotSatus_t	*s_pstSama_SlotStatusArray = NULL;

// for Debuggind
STATIC HCHAR			 s_szSama_StringForTime[128];

#define ___LOCAL_FUNCTIONS___

STATIC HINT32 sama_sch_cbCmpRsvItemKey (const void *a, const void *b, void *userdata)
{
	return (HINT32)((HINT32)a - (HINT32)b);
}

STATIC void sama_sch_cbRemoveRsvItem (void *data)
{
	if (data)
	{
		SAMA_Free(data);
		data = NULL;
	}
}

// dapi thread 이므로, sama context 를 조작하면 안됨.
static void sama_sch_cbRsvItemGetter (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	HxTREE_t			*pstSchTree = (HxTREE_t*)userdata;
	samaSlotSatus_t		*pstSlotStatus;
	DxSchedule_t		*pstSrcSch, *pstDstSch;
	DAxIterator_t		 stIterator;

	HxLOG_Info("(+)\n");

	if (NULL == pstSchTree)
	{
		HxLOG_Critical("Schedule Tree not initialized!!\n");
		goto END_FUNC;
	}

	DAPI_InitIterator(&stIterator, data, size, sizeof(DxSchedule_t));
	while (DAPI_NextIterator(&stIterator))
	{
		pstSrcSch = (DxSchedule_t *)stIterator.data;
		if (NULL == pstSrcSch)
		{
			continue;
		}

		if (pstSrcSch->ulSlot >= SAMA_MAX_SLOT)
		{
			HxLOG_Critical("Slot ID more than SAMA_MAX_SLOT: something wrong!!!\n");
			continue;
		}

		if (NULL != HLIB_TREE_Lookup(pstSchTree, (const void *)pstSrcSch->ulSlot))
		{
			HxLOG_Critical("Same Slot ID(%d) found in the tree!\n", pstSrcSch->ulSlot);
			continue;
		}

		if (pstSrcSch->ulSlot > s_ulSama_MaxSlotId)
		{
			s_ulSama_MaxSlotId = pstSrcSch->ulSlot;
		}

		pstSlotStatus = &(s_pstSama_SlotStatusArray[pstSrcSch->ulSlot]);
		pstSlotStatus->eState = eSAMA_SLOTSTATE_Scheduled;

		pstDstSch = (DxSchedule_t *)SAMA_Calloc(sizeof(DxSchedule_t));
		if (NULL != pstDstSch)
		{
			SAMA_MemCpy(pstDstSch, pstSrcSch, sizeof(DxSchedule_t));
			//pstDstSch->eStatus = DxRSVSTATUS_WAITING;

			// If the schedule is repeatition type, then it need not be removed automatically.
			// We need not sustain the status from power on.
			if (DxRSVREPEAT_ONCE != pstDstSch->eRepeat)
			{
				pstDstSch->eStatus = DxRSVSTATUS_WAITING;
			}

			if (pstDstSch->bRemoving != FALSE)
			{
				HxLOG_Error("volatile value is not false. bRemoving(%d) \n",pstDstSch->bRemoving);
				pstDstSch->bRemoving = FALSE;
			}

			SAMALIB_SEPC_CheckScheduleSpec(pstDstSch);

			HLIB_TREE_Insert(pstSchTree, (void *)pstDstSch->ulSlot, (void *)pstDstSch);
		}
	}

END_FUNC:
	HxLOG_Info("(-)\n");
	return;
}

STATIC HERROR sama_sch_sendAllScheduleToSapi (HINT32 nRequestId)
{
	HUINT32			 ulSchNum = 0;
	void			*pvIter;
	DxSchedule_t	*pstSchArray = NULL;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();
	HERROR			 hErr, hResult = ERR_FAIL;

	if (NULL == pstSchTree)
	{
		HxLOG_Error("No Schedule Tree\n");
		goto END_FUNC;
	}

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if (NULL != pstSchedule)
		{
			ulSchNum ++;
		}
	}

	if (0 != ulSchNum)
	{
		pstSchArray = (DxSchedule_t *)SAMA_Malloc(sizeof(DxSchedule_t) * ulSchNum);
		if (NULL == pstSchArray)
		{
			HxLOG_Error("Memory allocation failed\n");
			goto END_FUNC;
		}

		ulSchNum = 0;
		for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
		{
			DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

			if (NULL != pstSchedule)
			{
				SAMA_MemCpy(&(pstSchArray[ulSchNum]), pstSchedule, sizeof(DxSchedule_t));
				ulSchNum ++;
			}
		}
	}

	hErr = SAMA_PROTOCOL_NotifySchedulesLoaded(ulSchNum, pstSchArray, nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAMA_PROTOCOL_NotifySchedulesLoaded failed: ulSchNum(%d)\n", ulSchNum);
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSchArray)					{ SAMA_Free(pstSchArray); }
	return hResult;
}

STATIC HERROR		sama_sch_releaseSlotFor(samaSlotState_e eSlotState, HUINT32 ulSlotId)
{
	samaSlotSatus_t		*pstSlotStatus;

	if (NULL == s_pstSama_SlotStatusArray)
	{
		HxLOG_Error("NULL == s_pstSama_SlotStatusArray ! \n");
		return ERR_FAIL;
	}

	if(ulSlotId > SAMA_MAX_SLOT)
	{
		HxLOG_Error("ulSlotId: %d is invalid \n");
		return ERR_FAIL;
	}


	pstSlotStatus = &(s_pstSama_SlotStatusArray[ulSlotId]);

	if(pstSlotStatus->eState == eSlotState)
	{
		pstSlotStatus->eState = eSAMA_SLOTSTATE_Empty;
	}
	else
	{
		HxLOG_Error("pstSlotStatus->eState: %d is invalid \n", pstSlotStatus->eState);
		return ERR_FAIL;
	}

	return ERR_OK;

}

STATIC DxSchedule_t *sama_sch_findSchedule (HUINT32 ulSlot)
{
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();

	if (NULL != pstSchTree)
	{
		return (DxSchedule_t *)HLIB_TREE_Lookup(pstSchTree, (void*)ulSlot);
	}

	return NULL;
}

STATIC HERROR sama_sch_getAllSchedules (HUINT32 *pulSchNum, DxSchedule_t **ppstSchArray)
{
	HUINT32			 ulCount = 0, ulSchNum = 0;
	void			*pvIter;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			 hResult = ERR_FAIL;

	if (NULL == pstSchTree)					{ goto END_FUNC; }

	*pulSchNum = 0;
	*ppstSchArray = NULL;

	ulSchNum = HLIB_TREE_Nnodes(pstSchTree);
	if (0 == ulSchNum)
	{
		goto END_FUNC;
	}

	pstSchArray = (DxSchedule_t *)SAMA_Malloc(sizeof(DxSchedule_t) * ulSchNum);
	if (NULL == pstSchArray)
	{
		goto END_FUNC;
	}

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if (NULL != pstSchedule)
		{
			HxSTD_MemCopy(&(pstSchArray[ulCount]), pstSchedule, sizeof(DxSchedule_t));
			ulCount ++;
		}

	}

	if (0 == ulCount)
	{
		goto END_FUNC;
	}

	*pulSchNum = ulCount;
	*ppstSchArray = pstSchArray;
	pstSchArray = NULL;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSchArray)				{ SAMA_Free(pstSchArray); }

	return hResult;
}

STATIC HUINT32 sama_sch_countScheduleBySvcUid (HUINT32 ulSvcUid)
{
	HUINT32			 ulCount = 0;
	void			*pvIter;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();

	if (NULL == pstSchTree)					{ return 0; }

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if ((NULL != pstSchedule) && (ulSvcUid == pstSchedule->ulSvcUid))
		{
			ulCount ++;
		}
	}

	return ulCount;
}

STATIC HUINT32 sama_sch_countScheduleByType (DxRsvType_e eRsvType)
{
	HUINT32			 ulCount = 0;
	void			*pvIter;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();

	if (NULL == pstSchTree)					{ return 0; }

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if ((NULL != pstSchedule) && (eRsvType == pstSchedule->eRsvType))
		{
			ulCount ++;
		}
	}

	return ulCount;
}

STATIC DxSchedule_t *sama_sch_findScheduleBySvcUid (HUINT32 ulSvcUid, HUINT32 ulIndex)
{
	HUINT32			 ulCount = 0;
	void			*pvIter;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();

	if (NULL == pstSchTree)					{ return NULL; }

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if ((NULL != pstSchedule) && (ulSvcUid == pstSchedule->ulSvcUid))
		{
			if (ulCount >= ulIndex)
			{
				return pstSchedule;
			}

			ulCount ++;
		}
	}

	return NULL;
}

STATIC DxSchedule_t *sama_sch_findScheduleByType (DxRsvType_e eRsvType, HUINT32 ulIndex)
{
	HUINT32			 ulCount = 0;
	void			*pvIter;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();

	if (NULL == pstSchTree)					{ return NULL; }

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if ((NULL != pstSchedule) && (eRsvType == pstSchedule->eRsvType))
		{
			if (ulCount >= ulIndex)
			{
				return pstSchedule;
			}

			ulCount ++;
		}
	}

	return NULL;
}

STATIC DxSchedule_t *sama_sch_findEarliestSchedule (void)
{
	void			*pvIter;
	UNIXTIME		 utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	UNIXTIME		 utStartTime, utEarliestTime = 0xFFFFFFFF;
	DxSchedule_t	*pstEarliest = NULL;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();

	if (NULL == pstSchTree)					{ return NULL; }

	if (utCurrTime < SAMA_TIME_AVAIL_MIN)
	{
		HxLOG_Error("HLIB_STD_GetSystemTime err:\n");
		return NULL;
	}

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if (NULL != pstSchedule && pstSchedule->bRemoved != TRUE)
		{
			utStartTime = 0;

			(void)SAMALIB_TIME_GetRepeatedScheduleStartTime(pstSchedule, utCurrTime, &utStartTime);
			if (utStartTime > 0)
			{
				if ((NULL == pstEarliest) ||
					(utStartTime < utEarliestTime))
				{
					pstEarliest = pstSchedule;
					utEarliestTime = utStartTime;
				}
			}
		}
	}

	return pstEarliest;
}

STATIC DxSchedule_t *sama_sch_findEarliestScheduleWithPaddingAndReadyTime (UNIXTIME *putStartTimeWithPaddingAndReady)
{
	void			*pvIter;
	UNIXTIME		 utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	UNIXTIME		 utStartTime, utEarliestTime = 0xFFFFFFFF;
	DxSchedule_t	*pstEarliest = NULL;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();
	UNIXTIME		 utStartTimeWithPaddingAndReady;

	if (NULL == pstSchTree)					{ return NULL; }

	if (utCurrTime < SAMA_TIME_AVAIL_MIN)
	{
		HxLOG_Error("HLIB_STD_GetSystemTime err:\n");
		return NULL;
	}

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if (NULL != pstSchedule && pstSchedule->bRemoved != TRUE)
		{
			utStartTime = 0;
			utStartTimeWithPaddingAndReady = 0;

			(void)SAMALIB_TIME_GetRepeatedScheduleStartTime(pstSchedule, utCurrTime, &utStartTime);
			(void)SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utStartTime, &utStartTimeWithPaddingAndReady, NULL);
			utStartTimeWithPaddingAndReady -= pstSchedule->eRsvReady;

			if (utStartTimeWithPaddingAndReady > 0)
			{
				if ((NULL == pstEarliest) ||
					(utStartTimeWithPaddingAndReady < utEarliestTime))
				{
					pstEarliest = pstSchedule;
					utEarliestTime = utStartTimeWithPaddingAndReady;
				}
			}
		}
	}

	if (NULL != putStartTimeWithPaddingAndReady)
	{
		*putStartTimeWithPaddingAndReady = utEarliestTime;
	}

	return pstEarliest;
}

STATIC HERROR sama_sch_updateScheduleDataToDb (HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	if (NULL == pstSchedule)					{ return ERR_FAIL; }

	if (sama_sch_findSchedule(ulSlot) == NULL)
	{
		HxLOG_Error("No Schedule with ulSlot(%d) exists:\n", ulSlot);
		return ERR_FAIL;
	}

	pstSchedule->ulSlot = ulSlot;
	DAPI_Set(DxNAMEOF(DxSchedule_t), ulSlot, (void*)pstSchedule, sizeof(DxSchedule_t));

	return ERR_OK;
}


STATIC HERROR sama_sch_forEach (SAMA_ForEachResult_e (*pfCbFunc)(DxSchedule_t *pstSchedule, HINT32 nArgc, HUINT32 *pulArgv), HINT32 nArgc, HUINT32 *pulArgv)
{
	void			*pvIter;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();
	SAMA_ForEachResult_e	 eRet;

	if (NULL == pstSchTree)					{ return ERR_FAIL; }

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if (NULL != pstSchedule)
		{
			eRet = pfCbFunc(pstSchedule, nArgc, pulArgv);
			if (eSAMA_Result_BreakLoop == eRet)
			{
				break;
			}
		}
	}

	return ERR_OK;
}

SAMA_ForEachResult_e sama_sch_cbPauseTimers(SAMA_TimerItem_t *pstTimer, HINT32 nArgc, HUINT32 *pulArgv)
{
	HUINT32			*pulSlot = pulArgv;

	if ((NULL != pstTimer) && (NULL != pulSlot))
	{
		if (pstTimer->ulSlot == *pulSlot
			&& pstTimer->eItemType == eSAPI_TIMER_TYPE_Start) // start만 pause 한다. stop을 보장하기 위해서이다. stop이 아닌 것만 pause할 수도 있지만 그러면 pending과 runaway가 pause된다.
		{
			pstTimer->bPause = TRUE;
		}
	}

	return eSAMA_Result_OK;
}

SAMA_ForEachResult_e sama_sch_cbResumeTimers(SAMA_TimerItem_t *pstTimer, HINT32 nArgc, HUINT32 *pulArgv)
{
	HUINT32			*pulSlot = pulArgv;

	if ((NULL != pstTimer) && (NULL != pulSlot))
	{
		if (pstTimer->ulSlot == *pulSlot)
		{
			pstTimer->bPause = FALSE;
		}
	}

	return eSAMA_Result_OK;
}

STATIC HERROR sama_sch_adjustStartTimeForRepetition (DxRsvRepeat_e eRepeatType, UNIXTIME utStartTime, HUINT32 ulRepeatDays, HINT32 nCheckOffset, UNIXTIME *putUpdatedTime, HBOOL *pbUpdated)
{
	HBOOL			 bUpdated = FALSE;
	UNIXTIME		 utCurrTime, utNextTime, utTimeGap;
	HERROR			 hErr;

	*pbUpdated = FALSE;

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (utCurrTime < SAMA_TIME_AVAIL_MIN)
	{
		HxLOG_Error("HLIB_STD_GetSystemTime err:\n");
		return ERR_FAIL;
	}

	switch (eRepeatType)
	{
	case DxRSVREPEAT_DAILY:
		while ((utStartTime + nCheckOffset) <= utCurrTime)
		{
			hErr = HLIB_DATETIME_GetNextDay(utStartTime, &utNextTime, &utTimeGap);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("HLIB_DATETIME_GetNextDay err:\n");
				return ERR_FAIL;
			}

			utStartTime = utNextTime;
			bUpdated = TRUE;
		}

		break;

	case DxRSVREPEAT_WEEKLY:
		while ((utStartTime + nCheckOffset) <= utCurrTime)
		{
			hErr = HLIB_DATETIME_GetNextWeek(utStartTime, &utNextTime, &utTimeGap);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("HLIB_DATETIME_GetNextWeek err:\n");
				return ERR_FAIL;
			}

			utStartTime = utNextTime;
			bUpdated = TRUE;
		}

		break;

	case DxRSVREPEAT_WEEKDAYS:
		if (HLIB_DATETIME_IsWorkDay(utStartTime, utStartTime) != TRUE)
		{
			hErr = HLIB_DATETIME_GetNextWorkDay(utStartTime, &utNextTime, &utTimeGap);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("HLIB_DATETIME_GetNextWeek err:\n");
				return ERR_FAIL;
			}

			utStartTime = utNextTime;
			bUpdated = TRUE;
		}

		while ((utStartTime + nCheckOffset) <= utCurrTime)
		{
			hErr = HLIB_DATETIME_GetNextWorkDay(utStartTime, &utNextTime, &utTimeGap);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("HLIB_DATETIME_GetNextWeek err:\n");
				return ERR_FAIL;
			}

			utStartTime = utNextTime;
			bUpdated = TRUE;
		}

		break;

	case DxRSVREPEAT_WITHOUT_SUNDAY:
		while ((utStartTime + nCheckOffset) <= utCurrTime)
		{
			hErr = HLIB_DATETIME_GetNextWithoutSunday(utStartTime, &utNextTime, &utTimeGap);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("HLIB_DATETIME_GetNextWeek err:\n");
				return ERR_FAIL;
			}

			utStartTime = utNextTime;
			bUpdated = TRUE;
		}

		break;

	case DxRSVREPEAT_WEEKEND:
		if (HLIB_DATETIME_IsWorkDay(utStartTime, utStartTime) == TRUE)
		{
			hErr = HLIB_DATETIME_GetNextWorkDay(utStartTime, &utNextTime, &utTimeGap);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("HLIB_DATETIME_GetNextWeek err:\n");
				return ERR_FAIL;
			}

			utStartTime = utNextTime;
			bUpdated = TRUE;
		}

		while ((utStartTime + nCheckOffset) <= utCurrTime)
		{
			hErr = HLIB_DATETIME_GetNextWorkDay(utStartTime, &utNextTime, &utTimeGap);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("HLIB_DATETIME_GetNextWeek err:\n");
				return ERR_FAIL;
			}

			utStartTime = utNextTime;
			bUpdated = TRUE;
		}

		break;

	case DxRSVREPEAT_FEWDAY_OF_WEEK:
		while ((utStartTime + nCheckOffset) <= utCurrTime)
		{
			hErr = HLIB_DATETIME_GetNextFewDayofWeek(ulRepeatDays, utStartTime, &utNextTime, &utTimeGap);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("HLIB_DATETIME_GetNextWeek err:\n");
				return ERR_FAIL;
			}

			utStartTime = utNextTime;
			bUpdated = TRUE;
		}

		break;

	default:
		break;
	}

	*putUpdatedTime = utStartTime;
	*pbUpdated = bUpdated;

	return ERR_OK;
}

#define _____DERIVATION_FUNCTIONS_____
STATIC HERROR sama_sch_insertScheduleToTimer (DxSchedule_t *pstSchedule)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		return SAMA_SCHEDULE_TVA_InsertScheduleToTimer(pstSchedule);
#endif
	case eDxSchedule_Spec_Base:
	default:
		return SAMA_SCHEDULE_BASE_InsertScheduleToTimer(pstSchedule);
	}

	return ERR_FAIL;
}

#define ___GLOBAL_FUNCTIONS___

HERROR SAMA_SCHEDULE_Initialize (void)
{
	HERROR			hErr = ERR_FAIL;
	DxSchedule_Spec_e	eSpec = SAMALIB_SPEC_Get();

	switch ( eSpec )
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		case eDxSchedule_Spec_TvAnyTime:
			hErr = SAMA_SCHEDULE_TVA_Initialize();
			break;
#endif
		case eDxSchedule_Spec_Base:
		default:
			break;
	}

	return hErr;

}

HERROR SAMA_SCHEDULE_UpdateAndTrackEitPf (HUID* uidlist, HUINT32 size)
{
	HERROR			hErr = ERR_FAIL;
	DxSchedule_Spec_e	eSpec = SAMALIB_SPEC_Get();

	switch ( eSpec )
	{
		case eDxSchedule_Spec_TvAnyTime:
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
			hErr = SAMA_SCHEDULE_TVA_UpdateEitPf(uidlist,size);
#endif

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME_ACCURATE_RECORD)
			hErr |= SAMA_SCHEDULE_TVA_TrackEitPf(uidlist, size);
#endif

			break;

		case eDxSchedule_Spec_Base:
		default:
			break;
	}

	return hErr;

}

HERROR SAMA_SCHEDULE_UpdateEitSc (HUID* uidlist, HUINT32 size)
{
	HERROR			hErr = ERR_FAIL;
	DxSchedule_Spec_e	eSpec = SAMALIB_SPEC_Get();

	switch ( eSpec )
	{
		case eDxSchedule_Spec_TvAnyTime:
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
			hErr = SAMA_SCHEDULE_TVA_UpdateEitSc(uidlist,size);
#endif
			break;

		case eDxSchedule_Spec_Base:
		default:
			break;
	}

	return hErr;

}

void SAMA_SCHEDULE_ProcessTwoWayTimer (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	switch (pstContext->stSpecOption.eSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		SAMA_SCHEDULE_TVA_ProcessTwoWayTimer(ulSlot, eTimerType, utTime);
		break;
#endif
	case eDxSchedule_Spec_Base:
	default:
		SAMA_SCHEDULE_BASE_ProcessTwoWayTimer(ulSlot, eTimerType, utTime);
		break;
	}

	return ;
}

HBOOL SAMA_SCHEDULE_KeepRemoveSchedule (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, DxSchedule_RemovedReason_e eRemoveReason)
{
	HBOOL		bKeepSchedule = FALSE;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext == NULL || pstSchedule == NULL \n");
		return FALSE;
	}

	if (NULL == pstSchedule)
	{
		HxLOG_Error("NULL == pstSchedule \n");
		return FALSE;
	}

	switch (pstSchedule->eScheduleSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		bKeepSchedule = SAMA_SCHEDULE_TVA_KeepRemoveSchedule(pstContext, pstSchedule, eRemoveReason);
		break;
#endif
	case eDxSchedule_Spec_Base:
	default:
		bKeepSchedule = SAMA_SCHEDULE_BASE_KeepRemoveSchedule(pstContext, pstSchedule, eRemoveReason);
		break;
	}

	return bKeepSchedule;

}

#if 0
void SAMA_SCHEDULE_CheckSeriesRecord (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule)
{
	HxLOG_Trace();

	if(pstContext == NULL || pstSchedule == NULL)
	{
		HxLOG_Error("pstContext == NULL || pstSchedule == NULL \n");
		return;
	}

	switch (pstSchedule->eScheduleSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		SAMA_SCHEDULE_BASE_CheckSeriesRecord(pstContext, pstSchedule);
		break;
	}
}
#endif

HBOOL SAMA_SCHEDULE_IsSeriesRecord (DxSchedule_t *pstSchedule)
{
	HBOOL	ret = FALSE;
	SAPI_ActionType_e	eActionType = eSAPI_ACTION_TYPE_None;

	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule is NULL ! \n");
		return FALSE;
	}

	eActionType = SAMALIB_CFLT_GetSapiActionTypeBySchedule(pstSchedule);

	if(eActionType != eSAPI_ACTION_TYPE_Record)
	{
		HxLOG_Debug ("do not check eActionType: %d \n", eActionType);
		return FALSE;
	}


	switch (pstSchedule->eScheduleSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		ret = FALSE;
		break;
#endif
	case eDxSchedule_Spec_Base:
	default:
		ret = SAMA_SCHEDULE_BASE_IsSeriesRecord(pstSchedule);
		break;
	}

	return ret;
}

HERROR SAMA_SCHEDULE_GetSeriesRecordCheckTime (DxSchedule_t *pstSchedule, UNIXTIME *putTime)
{
	UNIXTIME			utTime;
	UNIXTIME			utStartTime;
	UNIXTIME			utEndTime;
	UNIXTIME			utSeriesRecordCheckTime;

	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utTime);

	SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utTime, &utStartTime, &utEndTime);

	utSeriesRecordCheckTime = ((utTime + pstSchedule->ulDuration) - SAMA_CHECK_SERIES_RECORD_READY_TIME);

	HxLOG_Debug("utTime: %ld, utSeriesRecordCheckTime: %ld, (utStartTime - pstSchedule->eRsvReady): %ld \n", utTime, utSeriesRecordCheckTime, (utStartTime - pstSchedule->eRsvReady));

	if(utSeriesRecordCheckTime < utStartTime)
	{
		return ERR_FAIL;
	}

	*putTime = utSeriesRecordCheckTime;

	return ERR_OK;

}
#ifdef SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST
HERROR SAMA_SCHEDULE_CheckSameSeriesRecordExist (DxSchedule_t *pstSchedule, HUINT32 ulSeason, HUINT32 ulEpisodeNum, HUINT32 ulTotalNum)
{
	HERROR				 hErr = ERR_FAIL;
	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule is NULL ! \n");
		return ERR_FAIL;
	}
	switch (pstSchedule->eScheduleSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = SAMA_SCHEDULE_BASE_CheckSameSeriesRecordExist(pstSchedule, ulSeason, ulEpisodeNum, ulTotalNum);
		break;
	}
	return hErr;
}
#endif

HERROR SAMA_SCHEDULE_MakeNextSeriesRecord (DxSchedule_t *pstSchedule, HxList_t *pSeriesRecordNextEventList, HBOOL bExistSameSeriesItem)
{
	HERROR				 hErr = ERR_FAIL;

	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule is NULL ! \n");
		return ERR_FAIL;
	}

	switch (pstSchedule->eScheduleSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		{
			HUINT32 ulSeriesIndex = (HUINT32)pSeriesRecordNextEventList;
			SERIES_SET *pstSeriesSet = (SERIES_SET *)bExistSameSeriesItem;
			DxRSV_TvAnyTimeRecord_t	 *pstTvaRecord = NULL;

			if (MACRO_IS_TVA_RECORD_TYPE(pstSchedule->eRsvType))
			{
				pstTvaRecord = &(pstSchedule->uExtInfo.stRec.ex.tva);
			}
			else if (MACRO_IS_TVA_WATCH_TYPE(pstSchedule->eRsvType))
			{
				pstTvaRecord = &(pstSchedule->uExtInfo.stWch.ex.tva);
			}
			else
			{
				HxLOG_Error("Not applicapable type. rsv type=(%d) \n", pstSchedule->eRsvType);
				return ERR_FAIL;
			}

			/*
			* 	Make Alternative와 Make 차이
			*	1. Alternative는 conflict 발생 시, alternative를 찾아서 이 schedule만 만든다.
			*	2. Alternative는 conflict noti를 하지 않는다. ( 애초에 conflict가 나지 않는 schedule만 골라 등록한다.)
			*/
			if ( (pstTvaRecord->ucRecKind & SAMA_SERIES_CONTENT_KIND_ALTINST)
				&& MACRO_IS_TVA_SCHEDULE(pstSchedule, pstSeriesSet) )
			{
				HxLOG_Debug("Make alternative schedule. \n");
				hErr = SAMA_SCHEDULE_TVA_MakeAlternativeSeriesSchedules(pstSchedule, ulSeriesIndex, pstSeriesSet);
			}
			else
			{
				HxLOG_Debug("Make normal(not alternative) schedule. \n");
				hErr = SAMA_SCHEDULE_TVA_MakeSeriesSchedules(pstSchedule, ulSeriesIndex, pstSeriesSet);
			}
		}
		break;
#endif

	case eDxSchedule_Spec_Base:
	default:
		hErr = SAMA_SCHEDULE_BASE_MakeNextSeriesRecord(pstSchedule, pSeriesRecordNextEventList, bExistSameSeriesItem);
		break;
	}

	return hErr;

}

HERROR SAMA_SCHEDULE_GetNextSeriesRecord (DxSchedule_t *pstSchedule, HUINT32 ulSeason, HUINT32 ulEpisodeNum, HUINT32 	ulTotalNum, HBOOL bExistSameSeriesItem)
{
	HERROR				 hErr = ERR_FAIL;

	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule is NULL ! \n");
		return ERR_FAIL;
	}

	switch (pstSchedule->eScheduleSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		{
			HUINT32 ulSeriesIndex = (HUINT32)ulSeason;
			SERIES_SET *pstSeriesSet = (SERIES_SET *)ulEpisodeNum;
			hErr = SAMA_SCHEDULE_TVA_GetEventsByCrid(pstSchedule, ulSeriesIndex, pstSeriesSet);
		}
		break;
#endif
	case eDxSchedule_Spec_Base:
	default:
		hErr = SAMA_SCHEDULE_BASE_GetNextSeriesRecord(pstSchedule, ulSeason, ulEpisodeNum, ulTotalNum, bExistSameSeriesItem);
		break;
	}

	return hErr;
}

HERROR SAMA_SCHEDULE_FindCurrentSeriesRecordInfo (DxSchedule_t *pstSchedule, HUINT32 p2)
{
	HERROR				 hErr = ERR_FAIL;

	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule is NULL ! \n");
		return ERR_FAIL;
	}

	NOT_USED_PARAM(p2);
	switch (pstSchedule->eScheduleSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		{
			DxRSV_TvAnyTimeRecord_t *pstTvAnyTime = NULL;

			if (MACRO_IS_TVA_RECORD_TYPE(pstSchedule->eRsvType))
			{
				pstTvAnyTime = &pstSchedule->uExtInfo.stRec.ex.tva;
			}
			else if (MACRO_IS_TVA_WATCH_TYPE(pstSchedule->eRsvType))
			{
				pstTvAnyTime = &pstSchedule->uExtInfo.stWch.ex.tva;
			}
			else
			{
				HxLOG_Error("Unknown rsv type. type=(%d) \n",pstSchedule->eRsvType);
				SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
				break;
			}
			if ( pstTvAnyTime->ucRecKind & SAMA_SERIES_CONTENT_KIND_CRID )
			{
				hErr = SAMA_SCHEDULE_TVA_MakeCridSchedule(pstSchedule);
			}
			else
			{
				hErr = SAMA_SCHEDULE_TVA_GetEventBySchedule(pstSchedule, p2);
			}
		}
		break;
#endif
	case eDxSchedule_Spec_Base:
	default:
		hErr = SAMA_SCHEDULE_BASE_FindCurrentSeriesRecordInfo(pstSchedule);
		break;
	}

	return hErr;
}



// 공통으로 두기 위험하지만..일단 sama_schedule_base.c 에서 볼일이 생겨서. 추후 정리.
HxTREE_t *SAMA_SCHEDULE_GetScheduleTree (void)
{
	return s_pstSama_ScheduleTree;
}

// 추후 schedule list 를 모두 tree 로 변환한 후 이런 logic 없앤다.
HINT32		SAMA_SCHEDULE_TreeValueToList (HxTREE_t *tree, HxList_t **list)
{
	HINT32			count;
	void				*iter;
	HxList_t			*pstList = NULL;

	if (NULL == tree)
	{
		HxLOG_Error("tree is NULL \n");
		return 0;
	}

	if (NULL != *list)
	{
		*list = HLIB_LIST_RemoveAllFunc (*list, SAMA_Free_CB);
		*list = NULL;
	}

	count = 0;
	for (iter = HLIB_TREE_Iterator(tree) ; iter ; iter = HLIB_TREE_NextIterator(iter))
	{
		DxSchedule_t			*pstSrcSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(iter);
		DxSchedule_t			*pstDstSchedule = (DxSchedule_t *)SAMA_Malloc (sizeof(DxSchedule_t));
		if (NULL != pstDstSchedule)
		{
			SAMA_MemCpy (pstDstSchedule, pstSrcSchedule, sizeof(DxSchedule_t));

			pstList = HLIB_LIST_Append (pstList, (void *)pstDstSchedule);
		}

		count++;
	}

	*list = pstList;

	return count;
}

HINT32		SAMA_SCHEDULE_TreeRunningValueToList (HxTREE_t *tree, HxList_t **list)
{
	HINT32			count;
	void				*iter;
	HxList_t			*pstList = NULL;

	if (NULL == tree)
	{
		HxLOG_Error("tree is NULL \n");
		return 0;
	}

	if (NULL != *list)
	{
		*list = HLIB_LIST_RemoveAllFunc (*list, SAMA_Free_CB);
		*list = NULL;
	}

	count = 0;
	for (iter = HLIB_TREE_Iterator(tree) ; iter ; iter = HLIB_TREE_NextIterator(iter))
	{
		DxSchedule_t			*pstSrcSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(iter);
		DxSchedule_t			*pstDstSchedule = NULL;

		// get only running,ready schedules
		if (!((pstSrcSchedule->eStatus == DxRSVSTATUS_RUNNING)
			|| (pstSrcSchedule->eStatus == DxRSVSTATUS_READY)))
		{
			continue;
		}

		pstDstSchedule= (DxSchedule_t *)SAMA_Malloc (sizeof(DxSchedule_t));
		if (NULL != pstDstSchedule)
		{
			SAMA_MemCpy (pstDstSchedule, pstSrcSchedule, sizeof(DxSchedule_t));

			pstList = HLIB_LIST_Append (pstList, (void *)pstDstSchedule);
			count++;
		}

	}

	*list = pstList;

	return count;
}





// inst (instance) 가 아니라 context..로 이름 변경.
// sapi_RemovePadding 와 동일한 함수이다.. 정리해야함.
HBOOL SAMA_SCHEDULE_RemovePadding(SAMA_Context_t *pstInst, HBOOL bReinsertTimer, sama_PaddingRemoval_e eUpdateFlag)
{
	HxList_t				*pstListItem = NULL;
	sama_PadRemoval_t		*pstPadRemoval;
//	HERROR					 hErr;
	DxSchedule_t			*pstSchedule = NULL;

	sama_CnftTimeEvt_t *pstStartTimeEvt;
	HBOOL				bUpdate = FALSE;
	HUINT32				ulUpdateCnt = 0;

	if ((NULL == pstInst) || (NULL == pstInst->stSamaConflictCheck.pstCheckStartEvt) || (NULL == pstInst->stSamaConflictCheck.pstCheckEndEvt) )
	{
		return FALSE;
	}

	pstStartTimeEvt = pstInst->stSamaConflictCheck.pstCheckStartEvt;

	if (eSAPI_CONFLICT_TYPE_None != pstInst->stSamaConflictCheck.stConflictResult.eType)
	{
		HxLOG_Debug("Conflict Occurred.  \n");
		goto FUNC_END;
	}

	HxLOG_Debug("No Conflict. now remove the padding time required\n");

	for (pstListItem = pstInst->stSamaConflictCheck.pstPadRemoveInfoList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		bUpdate = FALSE;
		pstPadRemoval = (sama_PadRemoval_t *)HLIB_LIST_Data(pstListItem);
		if (NULL == pstPadRemoval)
		{
			HxLOG_Error("Null Pad Removal. \n");
			continue;
		}

		HxLOG_Debug("padRemove slot=(%d), type=(%d) \n",pstPadRemoval->ulSlot, pstPadRemoval->ePadRemoval);

		pstSchedule = SAMA_SCHEDULE_GetSchedule(pstPadRemoval->ulSlot);
		if (pstSchedule != NULL)
		{
#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
			if ( (pstPadRemoval->ulSlot == pstStartTimeEvt->pstSchedule->ulSlot))
#endif
			{
				HxLOG_Debug("pstPadRemoval->ulSlot: %d, pstPadRemoval->ePadRemoval: 0x%x \n", pstPadRemoval->ulSlot, pstPadRemoval->ePadRemoval);
				if ( (eUpdateFlag & eSAPI_PADDINGRMV_StartPad)
					&& (pstPadRemoval->ePadRemoval & eSAPI_PADDINGRMV_StartPad))
				{
					if(pstSchedule->uExtInfo.stRec.ulPreOffsetTime != SAMA_TVA_REMOVED_PADDING_TIME)
					{
						pstSchedule->uExtInfo.stRec.ulPreOffsetTime = SAMA_TVA_REMOVED_PADDING_TIME;
						bUpdate = TRUE;
					}
				}

				if ( (eUpdateFlag & eSAPI_PADDINGRMV_StopPad)
					&& (pstPadRemoval->ePadRemoval & eSAPI_PADDINGRMV_StopPad))
				{
					if(pstSchedule->uExtInfo.stRec.ulPostOffsetTime != SAMA_TVA_REMOVED_PADDING_TIME)
					{
						pstSchedule->uExtInfo.stRec.ulPostOffsetTime = SAMA_TVA_REMOVED_PADDING_TIME;
						bUpdate = TRUE;
					}
				}

				if(bUpdate == TRUE)
				{
					ulUpdateCnt++;
					SAMA_SCHEDULE_UpdateSchedule(pstSchedule->ulSlot, bReinsertTimer, pstSchedule, SAMA_GetVirtualRequestId(), TRUE);
				}
			}

		}
		else if (pstPadRemoval->ulSlot == pstStartTimeEvt->pstSchedule->ulSlot)
		{
			if (((eUpdateFlag & eSAPI_PADDINGRMV_StartPad) != 0)
				&& ((pstPadRemoval->ePadRemoval & eSAPI_PADDINGRMV_StartPad) != 0))
			{
				pstStartTimeEvt->pstSchedule->uExtInfo.stRec.ulPreOffsetTime = SAMA_TVA_REMOVED_PADDING_TIME;
			}

			if (((eUpdateFlag & eSAPI_PADDINGRMV_StopPad) != 0)
				&& ((pstPadRemoval->ePadRemoval & eSAPI_PADDINGRMV_StopPad) != 0))
			{
				pstStartTimeEvt->pstSchedule->uExtInfo.stRec.ulPostOffsetTime = SAMA_TVA_REMOVED_PADDING_TIME;
			}
		}
	}


FUNC_END:
	// 여기서 free 하지 않는 것은 의도된 동작. 다른 곳에서 사용할수도 있기 때문에, make conflict time line 할때만 새로 만들어주는 형태임.
	//pstInst->pstPadRemoveInfoList = HLIB_LIST_RemoveAllFunc(pstInst->pstPadRemoveInfoList, SAMA_Free_CB);
	if(ulUpdateCnt > 0)
	{
		HxLOG_Debug("Remove Padding \n");
		bUpdate = TRUE;
	}
	else
	{
		bUpdate = FALSE;
	}

	return bUpdate;
}

HERROR SAMA_SCHEDULE_LoadScheduleData (HBOOL bIsFirstLaunch)
{
	HUINT32			 ulGetId;
#if 0
	void			*pvIter;
	HxList_t		*pstVolatileList = NULL, *pstListItem;
#endif
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if (NULL == s_pstSama_SlotStatusArray)
	{
		s_pstSama_SlotStatusArray = (samaSlotSatus_t *)SAMA_Calloc(sizeof(samaSlotSatus_t) * SAMA_MAX_SLOT);
	}

	pstContext->bSama_ScheduleLoaded = FALSE;
	pstContext->bSama_TvaInited = FALSE;
	s_pstSama_ScheduleTree = HLIB_TREE_NewFull(sama_sch_cbCmpRsvItemKey, NULL, NULL, sama_sch_cbRemoveRsvItem);

	ulGetId = DAPI_GetAll(DxNAMEOF(DxSchedule_t), sama_sch_cbRsvItemGetter, (void*)s_pstSama_ScheduleTree);

	HxLOG_Debug("DAPI_Wait() start, ulGetId: %u \n", ulGetId);

	if (DAPI_Wait(600000, ulGetId) == DERR_TIMEOUT)
	{
		HxLOG_Critical("DAPI_Wait() timeout! \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("DAPI_Wait() finish \n");
#if 0
	if(bIsFirstLaunch == TRUE)
	{
		// 다 받은 Schedule 중 Volatile Schedule (Instant Rec, LiveStreaming) 등은 삭제한다.
		// 위의 Getter에서 처리하지 않는 이유는 DAPI Callback 내에서 DAPI_Reset을 부르는 것이 안 좋아서.
		// 허용된다면 차라리 cbRsvItemGetter 에서 밑의 로직을 처리하는 것이 훨씬 효율적이다.
		for (pvIter = HLIB_TREE_Iterator(s_pstSama_ScheduleTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
		{
			DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

			if (NULL != pstSchedule)
			{
				if (SAMA_SCHEDULE_IsScheduleVolatile(pstSchedule) == TRUE)
				{
					pstVolatileList = HLIB_LIST_Append(pstVolatileList, (void *)pstSchedule);
				}
			}
		}
	}

	if (NULL != pstVolatileList)
	{
		for (pstListItem = pstVolatileList; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

			if (NULL != pstSchedule)
			{
				(void)DAPI_Reset(DxNAMEOF(DxSchedule_t), (HUID)pstSchedule->ulSlot);
				(void)HLIB_TREE_Remove(s_pstSama_ScheduleTree, (void *)pstSchedule->ulSlot);
			}
		}

		HLIB_LIST_RemoveAll(pstVolatileList);
	}
#endif

	pstContext->bSama_ScheduleLoaded = TRUE;
	(void)sama_sch_sendAllScheduleToSapi(SAMA_GetVirtualRequestId());

	return ERR_OK;
}

HERROR SAMA_SCHEDULE_StartTimer (HBOOL bIsFirstLaunch)
{
	UNIXTIME		 utStartTime, utCurrTime;
	HCHAR			*szTimeStr;
	HxTREE_t		*pstSchTree;
	void			*pvIter;
	HERROR			 hErr;

	pstSchTree = SAMA_SCHEDULE_GetScheduleTree();
	if (NULL == pstSchTree)
	{
		HxLOG_Error("No schedule: need not add the timer now\n");
		return ERR_FAIL;
	}


	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if (NULL != pstSchedule)
		{
			HBOOL				 bUpdated = FALSE;
			DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

			//	NOW SAMA is starting timer --> remove useless (or unneeded schedule like INSTANT/DELAYED)
			//	If you want to write fail reason , please set reason to PowerOff Fail default.
			if (SAMA_SCHEDULE_IsScheduleVolatile(pstSchedule) == TRUE)
			{
				HxLOG_Warning("Volatile Schedule --> remove it! [%d]\n", pstSchedule->ulSlot);
				// system time 이 정상 처리 되고, remove 날려주기 위함.
#if 1
				// 필요하면 SAMA_SCHEDULE_BASE_KeepRemoveSchedule() 인 경우, 바로 remove.
				// 사실 watch는 바로 remove 해도 되지만, 현재 어차피 SAMA_SCHEDULE_IsScheduleVolatile 에서 걸리므로.
				pstSchedule->bRemoved = TRUE;
				pstSchedule->stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Expired;
#else
				stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Volatile;
				SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
#endif
			}
			else
			{
				hErr = SAMA_SCHEDULE_AdjustTime(pstSchedule, &bUpdated);
				if ((ERR_OK == hErr) && (TRUE == bUpdated))
				{
					sama_sch_updateScheduleDataToDb(pstSchedule->ulSlot, pstSchedule);
				}

				utStartTime = utCurrTime;
				(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utStartTime);

				if ((utCurrTime > (utStartTime + 15)) && (DxRSVREPEAT_ONCE == pstSchedule->eRepeat))
				{
					HxLOG_Warning("Schedule(Slot:%d) expired for the time.\n", pstSchedule->ulSlot);

					szTimeStr = SAMA_SCHEDULE_GetStringOfUnixTime(utStartTime);
					HxLOG_Warning("Schedule Start Time : %s\n", szTimeStr);

					szTimeStr = SAMA_SCHEDULE_GetStringOfUnixTime(utCurrTime);
					HxLOG_Warning("Current Time 	   : %s\n", szTimeStr);

					if (pstSchedule->eStatus == DxRSVSTATUS_RUNNING)
					{
						HxLOG_Error("scheduled already have started. slot=(%d) \n",pstSchedule->ulSlot);
						stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Unknown;
					}
					else
					{
						stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Expired;
					}
					SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
				}
				else
				{
//					HxLOG_Debug("Insert the Schedule To Timer:\n");
					//SAMA_SCHEDULE_PrintSchedule(pstSchedule);
//					HxLOG_Debug("\n");

					SAMA_SCHEDULE_InsertScheduleToTimer(pstSchedule);
				}
			}
		}
	}

	hErr=SAMA_TIMER_ExecuteTimer();
	if(hErr != ERR_OK)
	{
		HxLOG_Critical(" SAMA_TIMER_ExecuteTimer Error!!\n");	
	}

	return ERR_OK;
}
HERROR SAMA_SCHEDULE_ReleaseSlotForSama (HUINT32 ulSlotId)
{
	HERROR				hErr = ERR_FAIL;

	hErr = sama_sch_releaseSlotFor(eSAMA_SLOTSTATE_ReservedBySama, ulSlotId);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("release slot for sama failed. slot=(%d)\n",ulSlotId);
		return ERR_FAIL;
	}

	return ERR_OK;
}
HERROR SAMA_SCHEDULE_ReleaseSlotForSapi (HUINT32 ulSlotId)
{
	HERROR 				hErr 			= ERR_FAIL;
	samaSlotSatus_t		*pstSlotStatus;
	HINT32				nPid;
	HUINT32				aulSlotArray[SAMA_EMPTY_SLOT_PER_PROCESS];
	HUINT32				ulSlotNum = 0;

	if (NULL == s_pstSama_SlotStatusArray)
	{
		HxLOG_Error("Slot Status not initiated:\n");
		return ERR_FAIL;
	}

	// 1. release slot for sapi
	hErr = sama_sch_releaseSlotFor(eSAMA_SLOTSTATE_ReservedBySapi, ulSlotId);
	if (hErr != ERR_OK )
	{
		HxLOG_Error("release slot for sapi failed. slot=(%d) \n", ulSlotId);
		return hErr;
	}

	// 2. Notify available slots changed
	pstSlotStatus = &(s_pstSama_SlotStatusArray[ulSlotId]);
	nPid = pstSlotStatus->nPid;
	// 해당되는 Process에게 새로 Slot을 할당해서 보내 준다.
	hErr = SAMA_SCHEDULE_GetEmptySlotsForSapi(nPid, &ulSlotNum, aulSlotArray);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("SAMA_SCHEDULE_GetEmptySlotsForSapi err. slot=(%d) \n",ulSlotId);
		return ERR_FAIL;
	}

	(void)SAMA_PROTOCOL_NotifyAvailableSlotsChanged(nPid, ulSlotNum, aulSlotArray, SAMA_GetVirtualRequestId());

	return ERR_OK;
}

HERROR SAMA_SCHEDULE_GetEmptySlotForSama (HUINT32 *pulSlotId)
{
	HUINT32				 ulSlotId;
	samaSlotSatus_t		*pstSlotStatus;
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if (NULL == s_pstSama_SlotStatusArray)
	{
		HxLOG_Error("NULL == s_pstSama_SlotStatusArray ! \n");
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	for (ulSlotId = 0; ulSlotId < SAMA_MAX_SLOT; ulSlotId++)
	{
		pstSlotStatus = &(s_pstSama_SlotStatusArray[ulSlotId]);

		if (pstSlotStatus->eState == eSAMA_SLOTSTATE_Empty)
		{
			pstSlotStatus->eState = eSAMA_SLOTSTATE_ReservedBySama;
			*pulSlotId = ulSlotId;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


HERROR SAMA_SCHEDULE_GetEmptySlot (HUINT32 *pulSlotId)
{
	HUINT32				 ulSlotId;
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	for (ulSlotId = 1; ulSlotId < 0xFFFF; ulSlotId++)
	{
		if (sama_sch_findSchedule(ulSlotId) == NULL)
		{
			*pulSlotId = ulSlotId;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR SAMA_SCHEDULE_GetEmptySlotsForSapi (HINT32 nPid, HUINT32 *pulNumSlot, HUINT32 *pulSlotArray)
{
	HUINT32				 ulSlot, ulSlotNum = 0;
	HUINT32				 ulSlotMax = ((s_ulSama_MaxSlotId + 1) > SAMA_MAX_SLOT) ? SAMA_MAX_SLOT : (s_ulSama_MaxSlotId + 1);
	samaSlotSatus_t		*pstSlotStatus;

	if (NULL == pulSlotArray)					{ return ERR_FAIL; }
	if (NULL == s_pstSama_SlotStatusArray)		{ return ERR_FAIL; }

	for (ulSlot = 0; ulSlot < ulSlotMax; ulSlot++)
	{
		pstSlotStatus = &(s_pstSama_SlotStatusArray[ulSlot]);

		if ((eSAMA_SLOTSTATE_ReservedBySapi == pstSlotStatus->eState) &&
			(nPid == pstSlotStatus->nPid))
		{
			pstSlotStatus->eState = eSAMA_SLOTSTATE_Empty;
		}
	}

	for (ulSlot = 0; ulSlot < SAMA_MAX_SLOT; ulSlot++)
	{
		pstSlotStatus = &(s_pstSama_SlotStatusArray[ulSlot]);

		if (eSAMA_SLOTSTATE_Empty == pstSlotStatus->eState)
		{
			pstSlotStatus->eState	= eSAMA_SLOTSTATE_ReservedBySapi;
			pstSlotStatus->nPid		= nPid;

			if (ulSlot > s_ulSama_MaxSlotId)
			{
				s_ulSama_MaxSlotId = ulSlot;
			}

			pulSlotArray[ulSlotNum] = ulSlot;
			ulSlotNum ++;

			if (ulSlotNum >= SAMA_EMPTY_SLOT_PER_PROCESS)
			{
				break;
			}
		}
	}

	if (NULL != pulNumSlot)					{ *pulNumSlot = ulSlotNum; }
	return ERR_OK;
}

DxSchedule_t *SAMA_SCHEDULE_GetSchedule (HUINT32 ulSlot)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return NULL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return NULL;
	}

	return sama_sch_findSchedule(ulSlot);
}


HERROR SAMA_SCHEDULE_GetAllSchedules (HUINT32 *pulNumSch, DxSchedule_t **ppstSchArray)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

	if ((NULL == pulNumSch) || (NULL == ppstSchArray))
	{
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	return sama_sch_getAllSchedules (pulNumSch, ppstSchArray);
}

HUINT32 SAMA_SCHEDULE_CountScheduleBySvcUid (HUINT32 ulSvcUid)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return 0;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return 0;
	}

	return sama_sch_countScheduleBySvcUid(ulSvcUid);
}

HUINT32 SAMA_SCHEDULE_CountScheduleByType (DxRsvType_e eRsvType)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return 0;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return 0;
	}

	return sama_sch_countScheduleByType(eRsvType);
}

DxSchedule_t *SAMA_SCHEDULE_GetScheduleBySvcUid (HUINT32 ulSvcUid, HUINT32 ulIndex)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return NULL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return NULL;
	}

	return sama_sch_findScheduleBySvcUid(ulSvcUid, ulIndex);
}

DxSchedule_t *SAMA_SCHEDULE_GetScheduleByType (DxRsvType_e eRsvType, HUINT32 ulIndex)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return NULL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return NULL;
	}

	return sama_sch_findScheduleByType(eRsvType, ulIndex);
}

DxSchedule_t *SAMA_SCHEDULE_GetEarliestSchedule (void)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return NULL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return NULL;
	}

	return sama_sch_findEarliestSchedule();
}

DxSchedule_t *SAMA_SCHEDULE_GetEarliestScheduleWithPaddingAndReadyTime (UNIXTIME *putStartTimeWithPaddingAndReady)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return NULL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return NULL;
	}

	return sama_sch_findEarliestScheduleWithPaddingAndReadyTime(putStartTimeWithPaddingAndReady);
}

HERROR SAMA_SCHEDULE_AddSchedule (HUINT32 ulSlot, DxSchedule_t *pstSchedule, HBOOL bFromSama, HINT32 nRequestId)
{
	HBOOL			 bTimeUpdated = FALSE;
	HINT32			 nPid;
	HUINT32			 ulSlotNum = 0;
	samaSlotSatus_t	*pstSlotStatus;
	DxSchedule_t	*pstDstSchdl = NULL;
	DxSchedule_t	stDatSchdl;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();
	HUINT32			 aulSlotArray[SAMA_EMPTY_SLOT_PER_PROCESS];
	HERROR			 hErr, hResult = ERR_FAIL;
	SAMA_Context_t		*pstContext = SAMA_GetContext();
	HBOOL			bAdjustStartTime = TRUE;

	HxLOG_Info("ulSlot: %d \n", ulSlot);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		goto END_FUNC;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		goto END_FUNC;
	}

	if (ulSlot >= SAMA_MAX_SLOT)
	{
		HxLOG_Error("Slot(%d) over:\n", ulSlot);
		goto END_FUNC;
	}

	if (NULL == s_pstSama_SlotStatusArray)
	{
		HxLOG_Error("Slot Status not initiated:\n");
		goto END_FUNC;
	}

	pstSlotStatus = &(s_pstSama_SlotStatusArray[ulSlot]);

	if(bFromSama)
	{
		if (eSAMA_SLOTSTATE_ReservedBySama != pstSlotStatus->eState)
		{
			HxLOG_Error("Slot is not allocated by the SAMA:\n");
			goto END_FUNC;
		}
	}
	else
	{
		if (eSAMA_SLOTSTATE_ReservedBySapi != pstSlotStatus->eState)
		{
			HxLOG_Error("Slot is not allocated by the SAPI applicant:\n");
			goto END_FUNC;
		}
	}
	nPid = pstSlotStatus->nPid;

	pstDstSchdl = (DxSchedule_t *)SAMA_Malloc(sizeof(DxSchedule_t));
	if (NULL == pstDstSchdl)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto END_FUNC;
	}

	// To DO: 충돌 체크를 이 시점에서 해야 한다....


	// 등록
	SAMA_MemCpy(pstDstSchdl, pstSchedule, sizeof(DxSchedule_t));
	pstDstSchdl->ulSlot = ulSlot;
	pstDstSchdl->eStatus = DxRSVSTATUS_WAITING;

 	//activation time 의 경우 현새 시간이 시작시간과 종료 시간 사이에 존재할 경우, timer에 현재 시간 ~ 종료시간의 시간 정보를 insert 하기 위해 아래 code 추가.
 	if(pstDstSchdl->eRsvType == DxRSVTYPE_ACTIVATION_TIME)
	{
		UNIXTIME			 utStartTime, utEndTime, utCurrTime;

		(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstDstSchdl->stStartTime), &utStartTime);
		utEndTime = utStartTime + pstDstSchdl->ulDuration;

		utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

		if((utCurrTime > utStartTime) && (utCurrTime < utEndTime))
		{
			bAdjustStartTime = FALSE;
			SAMA_MemCpy(&stDatSchdl, pstDstSchdl, sizeof(DxSchedule_t));
			(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(utCurrTime + 1, &(stDatSchdl.stStartTime));
			stDatSchdl.ulDuration = stDatSchdl.ulDuration - (utCurrTime + 1 - utStartTime);
		}
	}

	if(bAdjustStartTime)
	{
		(void)SAMA_SCHEDULE_AdjustTime(pstDstSchdl, &bTimeUpdated);
	}

	// Schedule Tree에 넣는다.
	HLIB_TREE_Insert(pstSchTree, (void *)ulSlot, (void *)pstDstSchdl);
	pstSlotStatus->eState	= eSAMA_SLOTSTATE_Scheduled;

	// DLIB을 통해 저장한다.
	(void)DAPI_Set(DxNAMEOF(DxSchedule_t), (HUID)ulSlot, (void*)pstDstSchdl, sizeof(DxSchedule_t));
	(void)DAPI_Sync(DxNAMEOF(DxSchedule_t));

	// Notify로 Schedule 추가를 알린다.
	(void)SAMA_PROTOCOL_NotifyScheduleAdded(ulSlot, pstDstSchdl, nRequestId);

	// 해당되는 Process에게 새로 Slot을 할당해서 보내 준다.
	hErr = SAMA_SCHEDULE_GetEmptySlotsForSapi(nPid, &ulSlotNum, aulSlotArray);
	if (ERR_OK == hErr)
	{
		(void)SAMA_PROTOCOL_NotifyAvailableSlotsChanged(nPid, ulSlotNum, aulSlotArray, nRequestId);
	}
	else
	{
		HxLOG_Error("SAMA_SCHEDULE_GetEmptySlotsForSapi err:\n");
	}

	// Timer에 넣는다.
	if(bAdjustStartTime)
	{
		hErr = sama_sch_insertScheduleToTimer(pstDstSchdl);
	}
	else
	{
		hErr = sama_sch_insertScheduleToTimer(&stDatSchdl);
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error("sama_sch_insertScheduleToTimer err:\n");
		//SAMA_PROTOCOL_NotifyScheduleAdded 와 sama_sch_insertScheduleToTimer 순서가 바뀌면 안된다. 즉시녹화나 ready/start가 한번에 올라가는 경우 윗단에서 꼬임.

		// Tree에 넣었는데 timer에 넣지 못했다고, 여기서 schedule 을 free해버리면 나중에 2중 free 문제 생길 수 있다.
		// timer에 넣지 못할 이유는 Volatile 예약인데 현재 시간이 invalid time 일 경우이다.
		// 이런 경우는 현재 시간이 정상화 되어 있을 경우 다시 timer에 넣어주는 작업이 되어 있다.

		//goto END_FUNC;
	}

	hErr=SAMA_TIMER_ExecuteTimer();
	if(hErr != ERR_OK)
	{
		HxLOG_Critical(" SAMA_TIMER_ExecuteTimer Error!!\n");	
	}

	pstDstSchdl = NULL;
	hResult = ERR_OK;

	HxLOG_Debug("add schedule\n");
	SAMA_SCHEDULE_PrintSchedule(pstDstSchdl);

END_FUNC:
	if (NULL != pstDstSchdl)
	{
		SAMA_Free(pstDstSchdl);
		pstDstSchdl = NULL;
	}

	return hResult;
}


HERROR SAMA_SCHEDULE_RemoveSchedule (HUINT32 ulSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo, HBOOL bForcedRemove, HINT32 nRequestId, HBOOL bSync)
{
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();
	SAMA_Context_t		*pstContext = SAMA_GetContext();
	HBOOL			bKeepSchedule = FALSE;
	DxSchedule_t	*pstSchedule = NULL;
	HERROR			hErr = ERR_OK;

	HxLOG_Info("ulSlot: %d \n", ulSlot);


	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	pstSchedule = sama_sch_findSchedule(ulSlot);
	if (pstSchedule == NULL)
	{
		HxLOG_Error("Can't find schedule slot[%d]..\n",ulSlot);
		return ERR_FAIL;
	}

	if(SAMA_SCHEDULE_IsSeriesRecord(pstSchedule) == TRUE
		&& (pstRemovedInfo->eRemovedReason != DxSchedule_RemovedReason_Canceled
			&& pstRemovedInfo->eRemovedReason != DxSchedule_RemovedReason_SameSeriesRecItemExist
			&& pstRemovedInfo->eRemovedReason != DxSchedule_RemovedReason_ServiceDeleted)
	   )
	{
		// check series rec
		DxSchedule_t	*pstScheduleForSeriesRecord = NULL;
		pstScheduleForSeriesRecord = SAMA_Calloc(sizeof(DxSchedule_t));
		if(pstScheduleForSeriesRecord != NULL)
		{
			// free는 받는 애가.
			SAMA_MemCpy(pstScheduleForSeriesRecord, pstSchedule, sizeof(DxSchedule_t));
			SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD, (HINT32)pstScheduleForSeriesRecord, 0, 0, 0, 0, 0);
		}
	}
	else
	{
//		HxLOG_Debug("do not check series record bacuase pstSchedule->eRsvType: %d[%s]\n", pstSchedule->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstSchedule->eRsvType));
	}

	{

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		DxSchedule_Spec_e	eSpec = SAMALIB_SPEC_Get();

		if ( eSpec == eDxSchedule_Spec_TvAnyTime)
		{
			SAMA_SCHEDULE_TVA_RemoveSchedule(ulSlot, pstSchedule, pstRemovedInfo);

			if ( pstRemovedInfo->eRemovedReason == DxSchedule_RemovedReason_SeriesTracking)
			{
				bForcedRemove = FALSE;
				HxLOG_Debug(HxANSI_COLOR_GREEN("Keep schedule condition. slot=(%d), removedReason=(%d) \n"),ulSlot,pstRemovedInfo->eRemovedReason);
			}
		}
#endif
	}


	// Timer에서 지운다.
	(void)SAMA_TIMER_RemoveTimer(ulSlot);



	bKeepSchedule = SAMA_SCHEDULE_KeepRemoveSchedule(pstContext, pstSchedule, pstRemovedInfo->eRemovedReason);
	HxLOG_Info("bKeepSchedule: %d \n", bKeepSchedule);

	pstSchedule->bExceptConflict = TRUE;
	pstSchedule->stRemovedInfo.eRemovedReason = pstRemovedInfo->eRemovedReason;
	pstSchedule->bRemoved = TRUE;


	if(bKeepSchedule == FALSE || bForcedRemove == TRUE)
	{
		// Schedule Tree에서 지운다.
		(void)HLIB_TREE_Remove(pstSchTree, (void *)ulSlot);

		// slot을 다시 사용할 수 있도록 Slot status를 empty로 한다.
		hErr = sama_sch_releaseSlotFor(eSAMA_SLOTSTATE_Scheduled, ulSlot);
		if (hErr != ERR_OK)
		{
			// series의 경우 sama로 의한 slot으로 잡혀 있으므로, 위 코드에서 status != scheduled로 인한 에러 로그가 뜨는 것은 정상이다.
			hErr = sama_sch_releaseSlotFor(eSAMA_SLOTSTATE_ReservedBySama, ulSlot);
			if (hErr != ERR_OK)
			{
				HxLOG_Debug("release slot failed. slot =(%d) \n",ulSlot);
			}
		}

		// DAPI를 통해 삭제
		(void)DAPI_Reset(DxNAMEOF(DxSchedule_t), (HUID)ulSlot);
		if(bSync)
		{
			(void)DAPI_Sync(DxNAMEOF(DxSchedule_t));
		}

		// Notify로 Schedule 삭제를 알린다.
		(void)SAMA_PROTOCOL_NotifyScheduleRemoved(ulSlot, pstRemovedInfo, nRequestId);

	}
	else
	{
		// DLIB을 통해 저장한다.
		(void)DAPI_Set(DxNAMEOF(DxSchedule_t), (HUID)ulSlot, (void*)pstSchedule, sizeof(DxSchedule_t));
		if(bSync)
		{
			(void)DAPI_Sync(DxNAMEOF(DxSchedule_t));
		}

		// sapi 쪽으로만 update 해 둔다.
		// Notify로 Schedule update를 알린다.
		(void)SAMA_PROTOCOL_NotifyScheduleModified(ulSlot, pstSchedule, TRUE, nRequestId);

	}


	return ERR_OK;
}

// of the Series by the Series for the Series
HERROR SAMA_SCHEDULE_ChangeNextSeriesSchedule (HUINT32 ulSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo, HINT32 nRequestId)
{
	DxSchedule_t	*pstSchedule;
	HERROR			 hErr;
	SAMA_Context_t		*pstContext = SAMA_GetContext();
	DxSchedule_t	stScheduleForSeriesRecCheck;

	HxLOG_Info("ulSlot: %d \n", ulSlot);



	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	pstSchedule = sama_sch_findSchedule(ulSlot);
	if (NULL == pstSchedule)
	{
		HxLOG_Error("No Schedule with ulSlot(%d) exists:\n", ulSlot);
		return ERR_FAIL;
	}

	SAMA_MemCpy(&stScheduleForSeriesRecCheck, pstSchedule, sizeof(DxSchedule_t));

	if(SAMA_SCHEDULE_IsSeriesRecord(pstSchedule) == TRUE)
	{
		// check series rec
		DxSchedule_t	*pstScheduleForSeriesRecord = NULL;
		pstScheduleForSeriesRecord = SAMA_Calloc(sizeof(DxSchedule_t));
		if(pstScheduleForSeriesRecord != NULL)
		{
			// free는 받는 애가.
			SAMA_MemCpy(pstScheduleForSeriesRecord, pstSchedule, sizeof(DxSchedule_t));
			SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD, (HINT32)pstScheduleForSeriesRecord, 0, 0, 0, 0, 0);
		}
	}
	else
	{
		HxLOG_Debug("do not check series record bacuase pstSchedule->eRsvType: %d[%s]\n", pstSchedule->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstSchedule->eRsvType));
	}
	hErr = SAMA_SCHEDULE_RemoveSchedule(ulSlot, pstRemovedInfo, FALSE, nRequestId, TRUE);

	return hErr;

	return ERR_OK;
}


HERROR SAMA_SCHEDULE_StopSchedule (HUINT32 ulSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo, HINT32 nRequestId)
{
//	UNIXTIME		 utStartTime = 0, utNextTime = 0;
	DxSchedule_t	*pstSchedule;
//	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();
	HERROR			 hErr;
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	HxLOG_Info("ulSlot: %d \n", ulSlot);



	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	pstSchedule = sama_sch_findSchedule(ulSlot);
	if (NULL == pstSchedule)
	{
		HxLOG_Error("No Schedule with ulSlot(%d) exists:\n", ulSlot);
		return ERR_FAIL;
	}

	if ((DxRSVSTATUS_READY != pstSchedule->eStatus) && (DxRSVSTATUS_RUNNING != pstSchedule->eStatus))
	{
		HxLOG_Error("Schedule(%d) not Started, or ready yet.\n", ulSlot);
		return ERR_FAIL;
	}


	if (DxRSVREPEAT_ONCE == pstSchedule->eRepeat)
	{

		hErr = SAMA_SCHEDULE_RemoveSchedule(ulSlot, pstRemovedInfo, FALSE, nRequestId, TRUE);

		return hErr;
	}

	// Timer에서 지운다.
	(void)SAMA_TIMER_RemoveTimer(ulSlot);
	if (DxRSVSTATUS_RUNNING == pstSchedule->eStatus)
	{
		pstSchedule->bRemoved = TRUE;
		SAMA_MemCpy(&(pstSchedule->stRemovedInfo), pstRemovedInfo, sizeof(DxSchedule_RemovedInfo_t));
		(void)SAMA_PROTOCOL_NotifyTimeUp(ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule);
	}

	// 시간 재조정:
#if 1
	// 어차피 status는 wating이 될 수 밖에 없다. 그리고 repeated item일 경우 time/schedule update 후 notify 도 줘야 한다. 그래서 update를 사용함.
	pstSchedule->eStatus = DxRSVSTATUS_WAITING;
	hErr = SAMA_SCHEDULE_UpdateSchedule (ulSlot, TRUE, pstSchedule, nRequestId, TRUE);
#else
	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utStartTime);
	utNextTime = utStartTime;
	hErr = SAMALIB_TIME_GetNextTime(pstSchedule, utStartTime, &utNextTime);
	if ((ERR_OK == hErr) && (utNextTime > utStartTime))
	{
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(utNextTime, &(pstSchedule->stStartTime));
		sama_sch_updateScheduleDataToDb(pstSchedule->ulSlot, pstSchedule);

		// Timer를 다시 넣는다.
		(void)SAMA_SCHEDULE_InsertScheduleToTimer(pstSchedule);
	}
#endif
	return ERR_OK;
}

HERROR SAMA_SCHEDULE_UpdateScheduleEx (HUINT32 ulSlot, HBOOL bReinsertTimer, DxSchedule_t *pstSchedule, HINT32 nRequestId, HBOOL bSync, HBOOL bExtend)
{
	HBOOL			 bTimeUpdated = FALSE;
	DxRsvStatus_e	 eStatus;
	DxSchedule_t	*pstDstSchdl;
	HERROR			 hErr;

	SAMA_Context_t		*pstContext = SAMA_GetContext();

	HxLOG_Trace();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	if (NULL == pstSchedule)					{ return ERR_FAIL; }

	pstDstSchdl = sama_sch_findSchedule(ulSlot);
	if (NULL == pstDstSchdl)
	{
		HxLOG_Error("No Schedule with ulSlot(%d) exists:\n", ulSlot);
		return ERR_FAIL;
	}

#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	{
		SERIES_SET		*pstSeriesSet = NULL;
		TVA_CONTENTID_T stDummyCrid;
		HERROR			hErr;

		// series tracking -> not series tracking
		pstSeriesSet = SAMALIB_TVA_GetTrackingSeriesSetBySlotId(ulSlot);
		if (pstSeriesSet != NULL)
		{
			HxLOG_Debug("Cancel tracking series. slot=(%d) \n", ulSlot);
			SAMALIB_TVA_DeleteSeriesSet(pstSeriesSet);
		}

		// tva -> not tva
		pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(ulSlot);
		if (!(MACRO_IS_TVA_SCHEDULE_TYPE(pstSchedule->eRsvType))	// Is after not tva
			 && pstSeriesSet != NULL)								// Is before tva
		{
			// series 구조에서 해당 schedule을 제거해준다.
			HxLOG_Debug("Tva -> Not Tva: Remove event period \n");
			hErr = SAMALIB_TVA_RemoveMatchingEventPeriod(pstDstSchdl->ulSvcUid, pstDstSchdl->uExtInfo.stRec.nEvtId, pstSeriesSet, &stDummyCrid);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("SAMALIB_TVA_RemoveMatchingEventPeriod failed. slot=(%d) \n",ulSlot);
				return ERR_FAIL;
			}
		}

		// tracking -> not tracking
		if (!(MACRO_IS_TRACKING_SCHEDULE(pstSchedule))						// Is after not tracking
			&& SAMALIB_TRACKING_IsTrackingBySlotId(pstSchedule->ulSlot))	// Is before tracking
		{
			// event id를 얻어오는 부분은 stRec과 stWch가 union으로 nEvtId의 위치가 동일함에 의존한다.
			HxLOG_Debug("Tracking -> Not Tracking: Stop Tracking \n");
			SAMALIB_TRACKING_StopTracking(pstDstSchdl->ulSvcUid, pstDstSchdl->uExtInfo.stRec.nEvtId);
		}
	}
#endif

	eStatus = pstDstSchdl->eStatus;

	if (pstDstSchdl != pstSchedule)
	{
		SAMA_MemCpy(pstDstSchdl, pstSchedule, sizeof(DxSchedule_t));
	}

	pstDstSchdl->ulSlot = ulSlot;
	pstDstSchdl->eStatus = eStatus;

	// To DO: 충돌 체크를 이 시점에서 해야 한다....
	// 시간 재조정:
	hErr = SAMA_SCHEDULE_AdjustTime(pstDstSchdl, &bTimeUpdated);
	if ((ERR_OK == hErr) && (TRUE == bTimeUpdated))
	{
		sama_sch_updateScheduleDataToDb(pstDstSchdl->ulSlot, pstDstSchdl);
	}

	// Timer를 수정한다.
	if (TRUE == bReinsertTimer)
	{
		(void)SAMA_TIMER_RemoveTimer(ulSlot);
		(void)SAMA_SCHEDULE_InsertScheduleToTimer(pstDstSchdl);
	}
	else
	{
		hErr = SAMA_TIMER_ChangeTimerTime(pstDstSchdl);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SAMA_TIME_ChangeTimerTime err:\n");
			return ERR_FAIL;
		}
	}
#ifdef CONFIG_DEBUG
	//HxLOG_Print("print all schedules and timer \n");
	//SAMA_SCHEDULE_PrintLogAllSchedules();
	//SAMA_TIMER_PrintLogAllTimer();
#endif
	// DLIB을 통해 저장한다.
	(void)DAPI_Set(DxNAMEOF(DxSchedule_t), (HUID)ulSlot, (void*)pstDstSchdl, sizeof(DxSchedule_t));
	if(bSync)
	{
		(void)DAPI_Sync(DxNAMEOF(DxSchedule_t));
	}

	if (bExtend)
	{
		SAPI_ScheduleChanged_t stChangedSchedule;
		stChangedSchedule.bStartPaddingChanged = TRUE;
		stChangedSchedule.ulStartPadding = pstDstSchdl->uExtInfo.stRec.ulPreOffsetTime;
		stChangedSchedule.bEndPaddingChanged = TRUE;
		stChangedSchedule.ulEndPadding = pstDstSchdl->uExtInfo.stRec.ulPostOffsetTime;
		stChangedSchedule.bReadyChanged = TRUE;
		stChangedSchedule.ulReady = pstDstSchdl->eRsvReady;

		(void)SAMA_PROTOCOL_NotifyScheduleModifiedEx(ulSlot, &stChangedSchedule, nRequestId);
	}
	else
	{
		// Notify로 Schedule update를 알린다.
		(void)SAMA_PROTOCOL_NotifyScheduleModified(ulSlot, pstDstSchdl, TRUE, nRequestId);
	}

	return ERR_OK;
}

HERROR SAMA_SCHEDULE_PauseSchedule (HUINT32 ulSlot, HINT32 nRequestId)
{
//	HBOOL			 bTimeUpdated = FALSE;
//	DxRsvStatus_e	 eStatus;
	DxSchedule_t	*pstSchedule;
//	HERROR			 hErr;
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	pstSchedule = sama_sch_findSchedule(ulSlot);
	if (NULL == pstSchedule)
	{
		HxLOG_Error("No Schedule with ulSlot(%d) exists:\n", ulSlot);
		return ERR_FAIL;
	}

	if (TRUE == pstSchedule->bPaused)
	{
		HxLOG_Error("Schedule(%d) is already paused:\n", ulSlot);
		return ERR_FAIL;
	}

	SAMA_TIMER_ForEach(sama_sch_cbPauseTimers, 0, &(pstSchedule->ulSlot));

	pstSchedule->bPaused = TRUE;

	(void)SAMA_PROTOCOL_NotifySchedulePaused(ulSlot, nRequestId);

	return ERR_OK;
}

HERROR SAMA_SCHEDULE_ResumeSchedule (HUINT32 ulSlot, HINT32 nRequestId)
{
//	HBOOL			 bTimeUpdated = FALSE;
//	DxRsvStatus_e	 eStatus;
	DxSchedule_t	*pstSchedule;
//	HERROR			 hErr;

	SAMA_Context_t		*pstContext = SAMA_GetContext();

	HxLOG_Info("ulSlot: %d \n", ulSlot);


	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	pstSchedule = sama_sch_findSchedule(ulSlot);
	if (NULL == pstSchedule)
	{
		HxLOG_Error("No Schedule with ulSlot(%d) exists:\n", ulSlot);
		return ERR_FAIL;
	}

	if (TRUE != pstSchedule->bPaused)
	{
		HxLOG_Debug("Schedule(%d) is not paused:\n", ulSlot);
		return ERR_FAIL;
	}

	SAMA_TIMER_ForEach(sama_sch_cbResumeTimers, 0, &(pstSchedule->ulSlot));

	pstSchedule->bPaused = FALSE;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	if (pstContext->stSpecOption.eSpec == eDxSchedule_Spec_TvAnyTime)
	{
		SAMA_SCHEDULE_TVA_ResumeSchedule(ulSlot);
	}
#endif

	(void)SAMA_PROTOCOL_NotifyScheduleResumed(ulSlot, nRequestId);

	return ERR_OK;
}

HERROR SAMA_SCHEDULE_UpdateSeries (HINT32 nRequestId)
{
	DxSchedule_Spec_e	eSpec = SAMALIB_SPEC_Get();
	HERROR				hErr = ERR_FAIL;

	switch (eSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		case eDxSchedule_Spec_TvAnyTime:
			hErr = SAMA_SCHEDULE_TVA_UpdateSeries();
			break;
#endif
		case eDxSchedule_Spec_Base:
		default:
			break;
	}

	return hErr;
}

HERROR SAMA_SCHEDULE_ChangePaddingTime (HUINT32 ulOffsetTime, HBOOL bIsPreOffset, HINT32 nRequestId)
{
	HERROR				 hErr = ERR_FAIL;
	SAMA_Context_t		*pstContext = SAMA_GetContext();
	HxList_t			*pstListItem;
	DxSchedule_t		*pstSchedule;

	HxLOG_Debug("ulOffsetTime: %d, bIsPreOffset: %d \n", ulOffsetTime, bIsPreOffset);

	SAMA_SCHEDULE_TreeValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));
	for (pstListItem = pstContext->stSamaConflictCheck.pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);
		if (NULL != pstSchedule && (DxRSVTYPE_RECORD_EBR == pstSchedule->eRsvType || DxRSVTYPE_RECORD_SERIES_EBR == pstSchedule->eRsvType))
		{
			if(bIsPreOffset == TRUE)
			{
				pstSchedule->uExtInfo.stRec.ulPreOffsetTime = ulOffsetTime;
			}
			else
			{
				pstSchedule->uExtInfo.stRec.ulPostOffsetTime = ulOffsetTime;
			}
#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
			if (MACRO_IS_TRACKING_SCHEDULE(pstSchedule))
			{
				pstSchedule->eRsvReady = DxRSVREADY_TVA_TRACKING;
			}
			else
			{
				if(pstSchedule->eRsvReady == DxRSVREADY_TVA_TRACKING)
				{
					pstSchedule->eRsvReady = DxRSVREADY_30_SEC;
				}
			}
			hErr=SAMA_SCHEDULE_UpdateScheduleEx(pstSchedule->ulSlot, TRUE, pstSchedule, nRequestId, FALSE, TRUE);
			if(ERR_OK != hErr)
			{
				HxLOG_Error("SAMA_SCHEDULE_UpdateSchedule err:\n");
			}
#else
			pstContext->stSamaConflictCheck.bScheduleChange = TRUE;
			(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstContext->stSamaConflictCheck), pstSchedule);


			HxLOG_Debug("stSamaConflictCheck.stConflictResult.eType: %d \n", pstContext->stSamaConflictCheck.stConflictResult.eType);

			if (eSAPI_CONFLICT_TYPE_None == pstContext->stSamaConflictCheck.stConflictResult.eType)
			{
				SAMA_SCHEDULE_UpdateSchedule(pstSchedule->ulSlot, FALSE, pstSchedule, nRequestId, TRUE);

#ifndef SAMA_LAZY_PADDING_CONFLICT_REMOVE
				(void)SAMA_SCHEDULE_RemovePadding(pstContext, FALSE, eSAPI_PADDINGRMV_StartPad | eSAPI_PADDINGRMV_StopPad);
#endif
			}
#endif
		}
	}

	hErr=SAMA_TIMER_ExecuteTimer();
	if(hErr != ERR_OK)
	{
		HxLOG_Critical(" SAMA_TIMER_ExecuteTimer Error!!\n");	
	}

	/* only once DAPI_Sync for updating all schedule padding time */
	(void)DAPI_Sync(DxNAMEOF(DxSchedule_t));

	return hErr;
}

HBOOL SAMA_SCHEDULE_IsScheduleVolatile (DxSchedule_t *pstSchedule)
{
	if (NULL != pstSchedule)
	{
		switch (pstSchedule->eRsvType)
		{
		case DxRSVTYPE_RECORD_INSTANT:
		case DxRSVTYPE_RECORD_DELAYED:
		case DxRSVTYPE_NETSTREAMING_LIVE:
		case DxRSVTYPE_NETSTREAMING_FILE:
			return TRUE;

		default:
			break;
		}
	}

	return FALSE;
}

HERROR SAMA_SCHEDULE_AdjustTime (DxSchedule_t *pstSchedule, HBOOL *pbUpdated)
{
	HBOOL				 bUpdated = FALSE;
	HINT32				 nCheckOffset;
	UNIXTIME			 utStartTime;
	HxDATETIME_t		 stStartDateTime;
	HERROR				 hErr;
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if (NULL != pbUpdated)						{ *pbUpdated = FALSE; }
	if (NULL == pstSchedule)					{ return ERR_FAIL; }


	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	if (DxRSVREPEAT_ONCE == pstSchedule->eRepeat)
	{
		return ERR_OK;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("before adjust time \n");
	HxLOG_Print("\r TIME is [%d-%d-%d] : [%d:%d:%d.%d]\r\n", pstSchedule->stStartTime.stDate.usYear,pstSchedule->stStartTime.stDate.ucMonth,pstSchedule->stStartTime.stDate.ucDay,
		pstSchedule->stStartTime.stTime.ucHour,pstSchedule->stStartTime.stTime.ucMinute,pstSchedule->stStartTime.stTime.ucSecond,pstSchedule->stStartTime.stTime.usMillisecond);
#endif

	hErr = HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utStartTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("HLIB_DATETIME_ConvertDateTimeToUnixTime err:\n");
		return ERR_FAIL;
	}

	SAMA_MemCpy(&stStartDateTime, &(pstSchedule->stStartTime), sizeof(HxDATETIME_t));

	// Only in the case of the activation time, the time is adjusted when the end time is over.
	// Except it, all cases are adjusted when the start time is over.
	if (DxRSVTYPE_ACTIVATION_TIME == pstSchedule->eRsvType)
	{
		nCheckOffset = (HINT32)pstSchedule->ulDuration;
	}
	else
	{
		nCheckOffset = 0;
	}

	hErr = sama_sch_adjustStartTimeForRepetition(pstSchedule->eRepeat, utStartTime, pstSchedule->ulRepeatDays, nCheckOffset, &utStartTime, &bUpdated);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("sama_sch_adjustStartTimeForRepetition err:\n");
		return ERR_FAIL;
	}

	if (TRUE == bUpdated)
	{
		hErr = HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(pstSchedule->stStartTime));
		if (ERR_OK != hErr)
		{
			HxLOG_Error("HLIB_DATETIME_ConvertUnixTimeToDateTime err:\n");
			return ERR_FAIL;
		}
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("after adjust time \n");
	HxLOG_Print("\r TIME is [%d-%d-%d] : [%d:%d:%d.%d]\r\n", pstSchedule->stStartTime.stDate.usYear,pstSchedule->stStartTime.stDate.ucMonth,pstSchedule->stStartTime.stDate.ucDay,
		pstSchedule->stStartTime.stTime.ucHour,pstSchedule->stStartTime.stTime.ucMinute,pstSchedule->stStartTime.stTime.ucSecond,pstSchedule->stStartTime.stTime.usMillisecond);
#endif

	if (NULL != pbUpdated)					{ *pbUpdated = bUpdated; }
	return ERR_OK;
}

HERROR SAMA_SCHEDULE_InsertScheduleToTimer (DxSchedule_t *pstSchedule)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	return sama_sch_insertScheduleToTimer(pstSchedule);
}

HERROR SAMA_SCHEDULE_SendAllScheduleToSapi (HINT32 nRequestId)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	return sama_sch_sendAllScheduleToSapi(nRequestId);
}

HERROR SAMA_SCHEDULE_ForEach (SAMA_ForEachResult_e (*pfCbFunc)(DxSchedule_t *pstSchedule, HINT32 nArgc, HUINT32 *pulArgv), HINT32 nArgc, HUINT32 *pulArgv)
{
	HERROR			 hErr;

	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

	if (TRUE != pstContext->bSama_ScheduleLoaded)
	{
		HxLOG_Error("Schedule not Loaded yet:\n");
		return ERR_FAIL;
	}

	if (NULL == pfCbFunc)					{ return ERR_FAIL; }

	hErr = sama_sch_forEach(pfCbFunc, nArgc, pulArgv);

	return hErr;
}

HERROR	SAMA_SCHEDULE_AddCurrentSeriesEvent (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, DxEvent_t *pstCurrentSeriesEvent)
{
	HERROR			hErr = ERR_FAIL;
	DxSchedule_Spec_e	eSpec = SAMALIB_SPEC_Get();

	switch ( eSpec )
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		case eDxSchedule_Spec_TvAnyTime:
			hErr = SAMA_SCHEDULE_TVA_AddCurrentSeriesEvent(pstSchedule, ulSeriesIndex, pstCurrentSeriesEvent);
			break;
#endif
		case eDxSchedule_Spec_Base:
		default:
			break;
	}

	return hErr;

}

HERROR			SAMA_SCHEDULE_AddSeriesEvents (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, SERIES_SET *pstSeriesSet, HxList_t *pstSeriesEventList)
{
	HERROR			hErr = ERR_FAIL;
	DxSchedule_Spec_e	eSpec = SAMALIB_SPEC_Get();

	switch ( eSpec )
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		case eDxSchedule_Spec_TvAnyTime:
			hErr = SAMA_SCHEDULE_TVA_AddSeriesEvents(pstSchedule, ulSeriesIndex, pstSeriesSet,pstSeriesEventList);
			break;
#endif
		case eDxSchedule_Spec_Base:
		default:
			break;
	}

	return hErr;
}

HERROR SAMA_SCHEDULE_UpdateRecordedProgramme (HUINT32 ulContentId, HBOOL bIsLoadAll)
{
	HERROR			hErr = ERR_FAIL;
	DxSchedule_Spec_e	eSpec = SAMALIB_SPEC_Get();

	switch ( eSpec )
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		case eDxSchedule_Spec_TvAnyTime:
			hErr = SAMA_SCHEDULE_TVA_UpdateRecordedProgramme(ulContentId, bIsLoadAll);
			break;
#endif
		case eDxSchedule_Spec_Base:
		default:
			break;
	}

	return hErr;
}

HUINT32	SAMA_SCHEDULE_CountSeries (DxSchedule_t *pstSchedule)
{
	DxSchedule_Spec_e	eSpec = SAMALIB_SPEC_Get();
	HUINT32				ulSeriesCnt = 0;

	switch ( eSpec )
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		case eDxSchedule_Spec_TvAnyTime:
			break;
#endif
		case eDxSchedule_Spec_Base:
		default:
			ulSeriesCnt = SAMA_SCHEDULE_BASE_CountSeries(pstSchedule);
			break;
	}

	return ulSeriesCnt;

}

#define _____DEBUGGER_FUNCTIONS_____

#if defined(CONFIG_DEBUG)
HERROR SAMA_SCHEDULE_PrintSchedule (DxSchedule_t *pstSchedule)
{
	HCHAR			*szString;
	HERROR			 hErr;

	if (NULL == pstSchedule)			{ return ERR_FAIL; }

	szString = (HCHAR *)HLIB_STD_MemAlloc(4096);
	if (NULL == szString)					{ return ERR_FAIL; }

	hErr = SAMA_SCHEDULE_StrPrintSchedule(szString, 4096, pstSchedule);
	if (ERR_OK == hErr)
	{
		HxLOG_Debug("%s", szString);
	}

	HLIB_STD_MemFree(szString);
	return hErr;
}

HERROR SAMA_SCHEDULE_StrPrintSchedule (HCHAR *szString, HUINT32 ulStrLen, DxSchedule_t *pstSchedule)
{
	HCHAR		*szTempBuf;

	if ((NULL == pstSchedule) || (NULL == szString))			{ return ERR_FAIL; }

	szTempBuf = (HCHAR *)HLIB_STD_MemAlloc(256);
	if (NULL == szTempBuf)										{ return ERR_FAIL; }

	szString[0] = '\0';

	HxSTD_PrintToStrN(szTempBuf, 256, "Schedule SlotID     : %d\n", pstSchedule->ulSlot);
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         RsvType    : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstSchedule->eRsvType));
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         SvcUID     : 0x%08x\n", pstSchedule->ulSvcUid);
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         OnID: 0x%04x, TsID: 0x%04x, SvcID: 0x%04x\n", pstSchedule->stTripleId.usOnId, pstSchedule->stTripleId.usTsId, pstSchedule->stTripleId.usSvcId);
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         RsvReady   : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvReady_e), pstSchedule->eRsvReady));
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         StartTime  : %s\n", SAMA_SCHEDULE_GetStringOfDateTime(&(pstSchedule->stStartTime)));
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         Duration   : %d sec\n", pstSchedule->ulDuration);
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	if(pstSchedule->eRsvType == DxRSVTYPE_RECORD_EBR || pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR)
	{
		HxSTD_PrintToStrN(szTempBuf, 256, "         Event ID   : %d (0x%x)\n", pstSchedule->uExtInfo.stRec.nEvtId, pstSchedule->uExtInfo.stRec.nEvtId);
		HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
		HxSTD_PrintToStrN(szTempBuf, 256, "         Event Name   : %s\n", pstSchedule->uExtInfo.stRec.szEventName);
		HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
		HxSTD_PrintToStrN(szTempBuf, 256, "         Start Padding   : %d sec\n", pstSchedule->uExtInfo.stRec.ulPreOffsetTime);
		HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
		HxSTD_PrintToStrN(szTempBuf, 256, "         End Padding   : %d sec\n", pstSchedule->uExtInfo.stRec.ulPostOffsetTime);
		HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
		HxSTD_PrintToStrN(szTempBuf, 256, "         series id   : %d \n", pstSchedule->uExtInfo.stRec.ulSeriesId);
		HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	} else if(pstSchedule->eRsvType == DxRSVTYPE_POWER_ON){
		HxSTD_PrintToStrN(szTempBuf, 256, "         Channel Number   : %d \n", pstSchedule->uExtInfo.stPwr.usChNum);
		HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
		HxSTD_PrintToStrN(szTempBuf, 256, "         Volumn  : %d \n", pstSchedule->uExtInfo.stPwr.ucVolume);
		HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	}
	else if (pstSchedule->eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR || pstSchedule->eRsvType == DxRSVTYPE_WATCHING_EBR)
	{
		HxSTD_PrintToStrN(szTempBuf, 256, "         Event Name   : %s\n", pstSchedule->uExtInfo.stWch.szEvtName);
		HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	}
	HxSTD_PrintToStrN(szTempBuf, 256, "         RepeatType : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvRepeat_e), pstSchedule->eRepeat));
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         RepeatDays : 0x%08x\n", pstSchedule->ulRepeatDays);
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         Status     : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvStatus_e), pstSchedule->eStatus));
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         EPG Type     : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxEPG_Type_e), pstSchedule->eEpgType));
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         Schedule Spec     : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxSchedule_Spec_e), pstSchedule->eScheduleSpec));
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         bPaused     : %d\n", pstSchedule->bPaused);
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         bRemoved     : %d\n", pstSchedule->bRemoved);
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "         Removed reason     : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxSchedule_RemovedReason_e), pstSchedule->stRemovedInfo.eRemovedReason));
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);

	switch (pstSchedule->eScheduleSpec)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	case eDxSchedule_Spec_TvAnyTime:
		if ( !MACRO_IS_TVA_SCHEDULE_TYPE(pstSchedule->eRsvType))
		{
			break;
		}
		{
			DxRSV_TvAnyTimeRecord_t *pstTvAnyTime;
			if (MACRO_IS_TVA_RECORD_TYPE(pstSchedule->eRsvType))
			{
				pstTvAnyTime = &(pstSchedule->uExtInfo.stRec.ex.tva);
			}
			else if (MACRO_IS_TVA_WATCH_TYPE(pstSchedule->eRsvType))
			{
				pstTvAnyTime = &(pstSchedule->uExtInfo.stWch.ex.tva);
			}
			else
			{
				break;
			}
			HxSTD_PrintToStrN(szTempBuf, 256, "         CRID type     : %d\n", pstTvAnyTime->ucCRIDType);
			HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
			HxSTD_PrintToStrN(szTempBuf, 256, "         CRID      : %s\n", pstTvAnyTime->szCRID);
			HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
			HxSTD_PrintToStrN(szTempBuf, 256, "         Programme CRID     : %s\n", pstTvAnyTime->szProgCRID);
			HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
			HxSTD_PrintToStrN(szTempBuf, 256, "         Series Set     : 0x%x\n", (HUINT32)SAMALIB_TVA_GetSeriesSetBySlotId(pstSchedule->ulSlot));
			HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
#if defined(CONFIG_OP_TDC)
			if (MACRO_IS_TVA_SERIES_TYPE(pstSchedule->eRsvType))
			{
				HxSTD_PrintToStrN(szTempBuf, 256, "         Series Title	 : %s\n", pstTvAnyTime->szSeriesTitle);
				HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
			}
#endif
		}


		break;
#endif
	case eDxSchedule_Spec_Base:
	default:
		break;
	}


	HLIB_STD_MemFree(szTempBuf);
	return ERR_OK;
}

HERROR SAMA_SCHEDULE_PrintAllSchedules (HERROR (*pfPrintFunc)(const HCHAR *format, ...))
{
	HCHAR			*szSchedStr = NULL;
	void			*pvIter;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();

	if (NULL == pfPrintFunc)									{ return ERR_FAIL; }
	if (NULL == pstSchTree)										{ return ERR_FAIL; }

	szSchedStr = (HCHAR *)HLIB_STD_MemAlloc(4096);

	if (NULL == szSchedStr)
	{
		return ERR_FAIL;
	}

	pfPrintFunc("\n - Print All Schedules in SAMA -\n\n");
	pfPrintFunc("  -------------------------------------------\n");

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if (NULL != pstSchedule)
		{
			(void)SAMA_SCHEDULE_StrPrintSchedule(szSchedStr, 4096, pstSchedule);
			pfPrintFunc("%s\n", szSchedStr);
			pfPrintFunc("  -------------------------------------------\n");
		}
	}
	HLIB_STD_MemFree(szSchedStr);

	return ERR_OK;
}


HERROR SAMA_SCHEDULE_PrintLogAllSchedules(void)
{
	HCHAR			*szSchedStr = NULL;
	void			*pvIter;
	HxTREE_t		*pstSchTree = SAMA_SCHEDULE_GetScheduleTree();

	if (NULL == pstSchTree)										{ return ERR_FAIL; }

	szSchedStr = (HCHAR *)HLIB_STD_MemAlloc(4096);

	if (NULL == szSchedStr)
	{
		return ERR_FAIL;
	}

	HxLOG_Debug("\n - Print All Schedules in SAMA -\n\n");
	HxLOG_Debug("  -------------------------------------------\n");

	for (pvIter = HLIB_TREE_Iterator(pstSchTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_TREE_IteratorValue(pvIter);

		if (NULL != pstSchedule)
		{
			(void)SAMA_SCHEDULE_StrPrintSchedule(szSchedStr, 4096, pstSchedule);
			HxLOG_Debug("%s\n", szSchedStr);
			HxLOG_Debug("  -------------------------------------------\n");
		}
	}
	HLIB_STD_MemFree(szSchedStr);

	return ERR_OK;
}


HCHAR *SAMA_SCHEDULE_GetStringOfUnixTime (UNIXTIME utTime)
{
	HxDATETIME_t		 stDateTime;
	HERROR				 hErr = ERR_FAIL;

	hErr = HLIB_DATETIME_ConvertUnixTimeToDateTime(utTime, &stDateTime);
	if (ERR_OK == hErr)
	{
		hErr = HLIB_DATETIME_GetStringByDateTime((const HxDATETIME_t *)&stDateTime, s_szSama_StringForTime, 128);
	}

	if (ERR_OK != hErr)
	{
		HLIB_STD_StrNCpySafe(s_szSama_StringForTime, "UNKNOWN", 128);
	}

	return s_szSama_StringForTime;
}

HCHAR *SAMA_SCHEDULE_GetStringOfDateTime (HxDATETIME_t *pstDateTime)
{
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pstDateTime)
	{
		hErr = HLIB_DATETIME_GetStringByDateTime((const HxDATETIME_t *)pstDateTime, s_szSama_StringForTime, 128);
	}

	if (ERR_OK != hErr)
	{
		HLIB_STD_StrNCpySafe(s_szSama_StringForTime, "UNKNOWN", 128);
	}

	return s_szSama_StringForTime;
}

HERROR SAMA_SCHEDULE_StrPrintSlot (HCHAR *szString, HUINT32 ulStrLen, samaSlotSatus_t *pstSlotStatus,HUINT32 ulIndex)
{
	HCHAR		*szTempBuf;

	if ((NULL == pstSlotStatus) || (NULL == szString))			{ return ERR_FAIL; }

	szTempBuf = (HCHAR *)HLIB_STD_MemAlloc(256);
	if (NULL == szTempBuf)										{ return ERR_FAIL; }

	szString[0] = '\0';

	HxSTD_PrintToStrN(szTempBuf, 256, "[%4d]", ulIndex);
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);

	switch ( pstSlotStatus->eState)
	{
		case eSAMA_SLOTSTATE_Empty:
				HxSTD_PrintToStrN(szTempBuf, 256, "%40s","[eSAMA_SLOTSTATE_Empty]");
				break;
		case eSAMA_SLOTSTATE_Scheduled:
				HxSTD_PrintToStrN(szTempBuf, 256, "%40s","[eSAMA_SLOTSTATE_Scheduled]");
				break;
		case eSAMA_SLOTSTATE_ReservedBySapi:
				HxSTD_PrintToStrN(szTempBuf, 256, "%40s","[eSAMA_SLOTSTATE_ReservedBySapi]");
				break;
		case eSAMA_SLOTSTATE_ReservedBySama:
				HxSTD_PrintToStrN(szTempBuf, 256, "%40s","[eSAMA_SLOTSTATE_ReservedBySama]");
				break;
		default:
				HxSTD_PrintToStrN(szTempBuf, 256, "%40s","[UNKNOWN]");
				break;
	}
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);

	HxSTD_PrintToStrN(szTempBuf, 256, "[PID:%4d]", pstSlotStatus->nPid);
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);
	HxSTD_PrintToStrN(szTempBuf, 256, "[CLIENT:%d]",pstSlotStatus->nClientHandle);
	HxSTD_StrNCat(szString, szTempBuf, ulStrLen);

	HLIB_STD_MemFree(szTempBuf);
	return ERR_OK;
}

HERROR SAMA_SCHEDULE_PrintSlotStatus(HERROR (*pfPrintFunc)(const HCHAR *format, ...))
{
	HCHAR			*szSlotStr = NULL;
	HUINT32			i;
	HUINT32			ulMaxPrint;

	if (NULL == pfPrintFunc)									{ return ERR_FAIL; }
	szSlotStr = (HCHAR *)HLIB_STD_MemAlloc(4096);

	if (NULL == szSlotStr)
	{
		return ERR_FAIL;
	}

	pfPrintFunc("\n - Print All Slot Status in SAMA -\n\n");
	pfPrintFunc(" max slot id = (%d) \n",s_ulSama_MaxSlotId);
	pfPrintFunc("  -------------------------------------------\n");

	ulMaxPrint = (s_ulSama_MaxSlotId+3 < SAMA_MAX_SLOT)? s_ulSama_MaxSlotId+3:SAMA_MAX_SLOT;
	for ( i=0; i<ulMaxPrint; i++ )
	{
		samaSlotSatus_t		*pstSlotStatus = (samaSlotSatus_t *)(&s_pstSama_SlotStatusArray[i]);

		if (NULL == pstSlotStatus)
		{
			continue;
		}
		(void)SAMA_SCHEDULE_StrPrintSlot(szSlotStr, 4096, pstSlotStatus,i);
		pfPrintFunc("%s\n", szSlotStr);
	}
	HLIB_STD_MemFree(szSlotStr);

	return ERR_OK;
}
#else
HERROR SAMA_SCHEDULE_PrintSchedule (DxSchedule_t *pstSchedule)
{
	return ERR_FAIL;
}

HERROR SAMA_SCHEDULE_StrPrintSchedule (HCHAR *szString, HUINT32 ulStrLen, DxSchedule_t *pstSchedule)
{
	return ERR_FAIL;
}

HERROR SAMA_SCHEDULE_PrintAllSchedules (HERROR (*pfPrintFunc)(const HCHAR *format, ...))
{
	return ERR_FAIL;
}

HERROR SAMA_SCHEDULE_PrintLogAllSchedules(void)
{
	return ERR_FAIL;
}

HCHAR *SAMA_SCHEDULE_GetStringOfUnixTime (UNIXTIME utTime)
{
	HLIB_STD_StrNCpySafe(s_szSama_StringForTime, "UNKNOWN", 128);
	return s_szSama_StringForTime;
}

HCHAR *SAMA_SCHEDULE_GetStringOfDateTime (HxDATETIME_t *pstDateTime)
{
	HLIB_STD_StrNCpySafe(s_szSama_StringForTime, "UNKNOWN", 128);
	return s_szSama_StringForTime;
}

HERROR SAMA_SCHEDULE_StrPrintSlot (HCHAR *szString, HUINT32 ulStrLen, samaSlotSatus_t *pstSlotStatus,HUINT32 ulIndex)
{
	return ERR_FAIL;
}

HERROR SAMA_SCHEDULE_PrintSlotStatus(HERROR (*pfPrintFunc)(const HCHAR *format, ...))
{
	return ERR_FAIL;
}
#endif

