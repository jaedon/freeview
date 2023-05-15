/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_timer.c
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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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
#include <papi.h>
#include <hapi.h>
#include <dlib.h>
#include <oapi_types.h>
#include <sapi.h>

#include "sama_int.h"
#include <sama_lib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define USE_SAMA_TIMER_SEMAPHORE

#ifdef USE_SAMA_TIMER_SEMAPHORE
	#define ENTER_CRITICAL(pstManager)			VK_SEM_Get(pstManager->ulSemId)
	#define LEAVE_CRITICAL(pstManager)			VK_SEM_Release(pstManager->ulSemId)
#else
	#define ENTER_CRITICAL(pstManager)			(void)(pstManager)
	#define LEAVE_CRITICAL(pstManager)			(void)(pstManager)
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSAMA_TIMER_MSGTYPE_None,

	eSAMA_TIMER_MSGTYPE_OneSec,
	eSAMA_TIMER_MSGTYPE_ExecSch,

	eSAMA_TIMER_MSGTYPE_EndOfCase
} sama_TimerMsgType_e;

typedef struct
{
	sama_TimerMsgType_e		 eMsgType;

	HUINT32					 ulSlot;
	SAPI_TimerItemType_e	 eTimerType;
	unsigned long			 ulTime;
	SAMA_CbTimerExec		 pfCbFunc;
} sama_TimerMsg_t;

typedef struct
{
	HBOOL					 bReady;

	unsigned long			 ulOneSecTimerId;
	unsigned long			 ulMsgQueueId;
	unsigned long			 ulSemId;
	unsigned long			 ulTaskId;

	HxList_t				*pstTimerList;			// SAMA_TimerItem_t

	SAMA_TimerItem_t		*pstFreeList;
} sama_TimerManager_t;

typedef struct
{
	HUINT32				 ulCurrTime;
	SAMA_TimerItem_t	*pstExecItem;
	HxList_t			*pstExecList;			// SAMA_TimerItem_t
} sama_TimerTest_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC sama_TimerManager_t		 s_stSamaTimer_Manager;


STATIC HCHAR *sapi_timer_getStringOfTimerItemType (SAPI_TimerItemType_e eType);


#define ___LOCAL_FUNCTIONS___
STATIC sama_TimerManager_t *sama_timer_getManager (void)
{
	return &(s_stSamaTimer_Manager);
}

STATIC void sama_timer_cbTimerOneSec (unsigned long ulTimerId, void *pvParam)
{
	sama_TimerMsg_t		 stMsg;
	sama_TimerManager_t	*pstManager = *((sama_TimerManager_t **)pvParam);
	HERROR				 hErr;

	stMsg.eMsgType = eSAMA_TIMER_MSGTYPE_OneSec;

	hErr = VK_MSG_SendTimeout (pstManager->ulMsgQueueId, (const void *)&stMsg, sizeof(sama_TimerMsg_t), 0);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("One Sec MSG Sent failed!\n");
	}
}

STATIC SAMA_TimerItem_t *sama_timer_allocTimer (sama_TimerManager_t *pstManager)
{
	SAMA_TimerItem_t		*pstItem;

	if (NULL == pstManager->pstFreeList)
	{
		pstManager->pstFreeList = (SAMA_TimerItem_t *)SAMA_Calloc (sizeof(SAMA_TimerItem_t));
		if (NULL == pstManager->pstFreeList)
		{
			return NULL;
		}
	}

	pstItem = pstManager->pstFreeList;
	pstManager->pstFreeList = pstItem->pstNextItem;

	SAMA_MemSet (pstItem, 0, sizeof(SAMA_TimerItem_t));
	return pstItem;
}

STATIC void sama_timer_freeTimer (sama_TimerManager_t *pstManager, SAMA_TimerItem_t *pstItem)
{
	if (NULL != pstItem)
	{
		pstItem->pstNextItem = pstManager->pstFreeList;
		pstManager->pstFreeList = pstItem;
	}
}

STATIC SAMA_TimerItem_t *sama_timer_findTimer (sama_TimerManager_t *pstManager, SAMA_TimerItem_t *pstTimerData)
{
	HxList_t			*pstListItem;

	for (pstListItem = pstManager->pstTimerList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SAMA_TimerItem_t	*pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstTimerItem) &&
			(pstTimerData->ulTime == pstTimerItem->ulTime) &&
			(pstTimerData->ulSlot == pstTimerItem->ulSlot) &&
			(pstTimerData->eItemType == pstTimerItem->eItemType) &&
			(pstTimerData->pfCbFunc == pstTimerItem->pfCbFunc))
		{
			return pstTimerItem;
		}
	}

	return NULL;
}

STATIC HINT32 sapi_timer_cbCmpTimerItemByTimeAndReservationType (void *pvUser, void *pvListItem)
{
	SAMA_TimerItem_t	*pstUserData = (SAMA_TimerItem_t *)pvUser;
	SAMA_TimerItem_t	*pstListItem = (SAMA_TimerItem_t *)pvListItem;

	if(pstUserData == NULL || pstListItem == NULL)
	{
		return 0;
	}

	// �ð� ��������.
	if (pstUserData->ulTime > pstListItem->ulTime)
	{
		return -1;
	}
	else if (pstUserData->ulTime < pstListItem->ulTime)
	{
		return 1;
	}
	else
	{
		// �ð� ���� ���, rsv type ����ġ �� �� �ֵ���. ������ �׳� �ܼ��� ������.
		// DxRSVTYPE_WATCHING_EBR �� DxRSVTYPE_POWER_ON ���� �ڷ� ������. (���� �ð� ������ ��� reminder�� ch�� zap)
		// TODO: ���� �ʿ��ϸ�, �Լ��� ���� plug in ó��.
		if(pstUserData->eRsvType < pstListItem->eRsvType)
		{
			return -1;
		}
		else if(pstUserData->eRsvType > pstListItem->eRsvType)
		{
			return 1;
		}
	}

	if (pstUserData->ulSlot < pstListItem->ulSlot)
	{
		return 1;
	}

	return 0;

}

STATIC HERROR sama_timer_ExecuteTimer (sama_TimerManager_t *pstManager)
{
	sama_TimerMsg_t		 stMsg;
	HERROR				 hErr;
	stMsg.eMsgType = eSAMA_TIMER_MSGTYPE_ExecSch;
	hErr = VK_MSG_SendTimeout (pstManager->ulMsgQueueId, &stMsg, sizeof(sama_TimerMsg_t), 100);
	if (VK_OK != hErr)
	{
		HxLOG_Critical("!!!!! VK_MSG_Sendout failed !!!!!!\n");
	}
	return hErr;
}
STATIC HERROR sama_timer_insertTimer (sama_TimerManager_t *pstManager, HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, DxRsvType_e eRsvType, unsigned long ulTime, SAMA_CbTimerExec pfCallback, HBOOL bPause)
{
	SAMA_TimerItem_t	*pstTimerItem;
	HERROR			 hResult = ERR_FAIL;

	pstTimerItem = (SAMA_TimerItem_t *)sama_timer_allocTimer (pstManager);
	if (NULL == pstTimerItem)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		goto END_FUNC;
	}

	pstTimerItem->ulSlot		= ulSlot;
	pstTimerItem->eItemType		= eTimerType;
	pstTimerItem->ulTime		= ulTime;
	pstTimerItem->pfCbFunc		= pfCallback;
	pstTimerItem->bPause		= bPause;
	pstTimerItem->eRsvType		= eRsvType;

	if (sama_timer_findTimer (pstManager, pstTimerItem) != NULL)
	{
		HxLOG_Error ("Same Timer already exist:\n");
		goto END_FUNC;
	}

	pstManager->pstTimerList = HLIB_LIST_AppendSortEx(pstManager->pstTimerList, (void *)pstTimerItem, sapi_timer_cbCmpTimerItemByTimeAndReservationType);

	pstTimerItem = NULL;


	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstTimerItem)					{ sama_timer_freeTimer (pstManager, pstTimerItem); }
	return hResult;
}

STATIC HERROR sama_timer_removeTimer (sama_TimerManager_t *pstManager, HUINT32 ulSlot)
{
	HxList_t			*pstListItem, *pstNextItem;
	SAMA_TimerItem_t	*pstTimerItem;

	pstListItem = pstManager->pstTimerList;
	while (NULL != pstListItem)
	{
		pstNextItem = pstListItem->next;

		pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data (pstListItem);
		if ((NULL != pstTimerItem) && (ulSlot == pstTimerItem->ulSlot))
		{
			pstManager->pstTimerList = HLIB_LIST_Remove (pstManager->pstTimerList, (void *)pstTimerItem);
			sama_timer_freeTimer (pstManager, pstTimerItem);
		}

		pstListItem = pstNextItem;
	}

	return ERR_OK;
}

STATIC HUINT32 sama_timer_getTimerTime (sama_TimerManager_t *pstManager, HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long *pulTime)
{
	HERROR			hErr = ERR_FAIL;
	HxList_t			*pstListItem;

	for (pstListItem = pstManager->pstTimerList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SAMA_TimerItem_t		*pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstTimerItem) && (eTimerType == pstTimerItem->eItemType) && (pstTimerItem->ulSlot == ulSlot))
		{
			*pulTime = pstTimerItem->ulTime;
			hErr = ERR_OK;
			break;
		}
	}

	return hErr;
}

STATIC HUINT32 sama_timer_countRemainingTimer (sama_TimerManager_t *pstManager, HUINT32 ulSlot)
{
	HUINT32				 ulCount = 0;
	HxList_t			*pstListItem;

	for (pstListItem = pstManager->pstTimerList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SAMA_TimerItem_t		*pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstTimerItem) && (eSAPI_TIMER_TYPE_None != pstTimerItem->eItemType) && (pstTimerItem->ulSlot == ulSlot))
		{
			ulCount ++;
		}
	}

	return ulCount;
}

STATIC HERROR sama_timer_changeTimerTime (sama_TimerManager_t *pstManager, DxSchedule_t *pstSchedule)
{
	UNIXTIME			 utTime, utStartTime, utEndTime;
	HxList_t			*pstListItem;

	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utTime);
	(void)SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utTime, &utStartTime, &utEndTime);

	for (pstListItem = pstManager->pstTimerList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SAMA_TimerItem_t		*pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstTimerItem) && (pstTimerItem->ulSlot == pstSchedule->ulSlot))
		{
			switch(pstTimerItem->eItemType)
			{
#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
			case eSAPI_TIMER_TYPE_PaddingStartReady:
				pstTimerItem->ulTime = utStartTime - pstSchedule->eRsvReady - SAMA_PADDING_READY_TIME;
				break;
#endif
			case eSAPI_TIMER_TYPE_Ready:
				pstTimerItem->ulTime = utStartTime - pstSchedule->eRsvReady;
				break;

			case eSAPI_TIMER_TYPE_Start:
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
				if (MACRO_IS_TRACKING_SCHEDULE(pstSchedule))
				{
					// tracking schedule�� start�� eit pf running status transition���� �ؾ� �Ѵ�.
					HxLOG_Error("Tracking Schedule's start Timer exists. slot=(%d) \n",pstSchedule->ulSlot);
				}
				else
				{
					pstTimerItem->ulTime = utStartTime;
				}
#else
				pstTimerItem->ulTime = utStartTime;
#endif
				break;

			case eSAPI_TIMER_TYPE_CheckSeriesRecordReady:
				if(SAMA_SCHEDULE_IsSeriesRecord(pstSchedule) == TRUE)
				{
					UNIXTIME		utTime;
					HERROR		hErr = ERR_FAIL;

					hErr = SAMA_SCHEDULE_GetSeriesRecordCheckTime(pstSchedule, &utTime);
					if(hErr == ERR_OK)
					{
						pstTimerItem->ulTime = utTime;
					}
					else
					{
						pstTimerItem->ulTime = utStartTime;
					}
				}
				else // ���⿡ �ɸ� ���� ����. series rec�� �ƴϸ� ���ʿ� eSAPI_TIMER_TYPE_CheckSeriesRecordReady�� ���� ������.
				{
					pstTimerItem->ulTime = utStartTime;
				}
				break;

#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
			case eSAPI_TIMER_TYPE_PaddingStopReady:
				pstTimerItem->ulTime = utTime + pstSchedule->ulDuration - SAMA_PADDING_READY_TIME;
				break;
#endif

			case eSAPI_TIMER_TYPE_Stop:
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
				if (MACRO_IS_TRACKING_SCHEDULE(pstSchedule))
				{
					// tracking schedule�� start�� eit pf running status transition���� �ؾ� �Ѵ�.
					HxLOG_Error("Tracking Schedule's End Timer exists. slot=(%d) \n",pstSchedule->ulSlot);
				}
				else
				{
					pstTimerItem->ulTime = utEndTime - 2;
				}
#else
				pstTimerItem->ulTime = utEndTime - 2;
#endif
				break;
			case eSAPI_TIMER_TYPE_StopRunaway:
				pstTimerItem->ulTime = utEndTime - 2 + SAMA_TVA_RUNAWAY_TIME;
				break;

			case eSAPI_TIMER_TYPE_StopPendingBroken:
				pstTimerItem->ulTime = utEndTime + pstSchedule->ulDuration/2 + 60;
				break;

			default:
				break;
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR sama_timer_forEach (sama_TimerManager_t *pstManager, SAMA_ForEachResult_e (*pfCbFunc)(SAMA_TimerItem_t *pstTimer, HINT32 nArgc, HUINT32 *pulArgv), HINT32 nArgc, HUINT32 *pulArgv)
{
	HxList_t				*pstListItem;
	SAMA_ForEachResult_e	 eRet = eSAMA_Result_OK;

	for (pstListItem = pstManager->pstTimerList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SAMA_TimerItem_t		*pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != pstTimerItem)
		{
			eRet = pfCbFunc(pstTimerItem, nArgc, pulArgv);
			if (eSAMA_Result_BreakLoop == eRet)
			{
				break;
			}
		}
	}

	return (eSAMA_Result_OK == eRet) ? ERR_OK : ERR_FAIL;
}

STATIC HINT32 sama_timer_getTimerPriority (SAMA_TimerItem_t *pstTimerItem)
{
	if (NULL != pstTimerItem)
	{
		switch (pstTimerItem->eItemType)
		{
#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
		case eSAPI_TIMER_TYPE_PaddingStopReady:
			return 5;

		case eSAPI_TIMER_TYPE_PaddingStartReady:
			return 4;
#endif
		case eSAPI_TIMER_TYPE_Stop:
			return 3;

		case eSAPI_TIMER_TYPE_Start:
			return 2;

		case eSAPI_TIMER_TYPE_Ready:
			return 1;

		// eSAPI_TIMER_TYPE_CheckSeriesRecordReady�� priority 0���� ó��.
		default:
			break;
		}
	}

	return 0;
}

STATIC HINT32 sama_timer_cbTimerExecPriority (void *pvUserData, void *pvNode)
{
	SAMA_TimerItem_t	*pstUser = (SAMA_TimerItem_t *)pvUserData;
	SAMA_TimerItem_t	*pstNode = (SAMA_TimerItem_t *)pvNode;
	HINT32			returnPriority = 0;

	if(pstUser->ulTime == pstNode->ulTime)
	{
		returnPriority = sama_timer_getTimerPriority(pstUser) - sama_timer_getTimerPriority(pstNode);
	}
	else
	{
		returnPriority = pstNode->ulTime - pstUser->ulTime;
	}

	return returnPriority;
}

STATIC HBOOL sama_timer_compTimeCondition (void *pvUserData, void *data)
{
	sama_TimerTest_t	*pstTestInfo  = (sama_TimerTest_t*)pvUserData;
	SAMA_TimerItem_t	*pstTimerInfo = (SAMA_TimerItem_t*)data;

	// TODO:
	// start time�� system time �������� �ϴ� ���� ������, end timer�� ��� duration�� �Ϻ��� ������ ���ؼ���
	// start timer �� tick�� start �Ͽ� ó���ϴ� ���� �´�.
	// ������, TBR/EBR�� insert timer�� ���� time offset�� Ƣ�� ��쿡 ���� ó��, ���� time line�� �̿��� conflict check ��ƾ�� ����, event trace (������ ���������) �� ���� ���䰡 �̷������ �Ѵ�.
	// �׸���, �ϴ��� ����� �̳��� �ð��� ƥ ��� �Ϲ����� reservation�� ��� ū ������ ���� (�� �� ���� �����̹Ƿ�.)
	// streaming�� ���� homma���� 60�� duration, 10�� ���� update�� �̷������ �־� ������ �� ������ �����Ѵ�. (��� TDT/TOT ó���� ���)

//	HxLOG_Print("pstTimerInfo->eItemType: %d, pstTimerInfo->ulTime: %ld, pstTestInfo->ulCurrTime: %ld \n", pstTimerInfo->eItemType, pstTimerInfo->ulTime, pstTestInfo->ulCurrTime);
	if ((eSAPI_TIMER_TYPE_None != pstTimerInfo->eItemType) && (pstTimerInfo->ulTime <= pstTestInfo->ulCurrTime))
	{
		if (TRUE != pstTimerInfo->bPause)
		{
			if(((pstTestInfo->ulCurrTime > (HUINT32)pstTimerInfo->ulTime) ? (pstTestInfo->ulCurrTime - (HUINT32)pstTimerInfo->ulTime) : ((HUINT32)pstTimerInfo->ulTime - pstTestInfo->ulCurrTime)) >= 3)
			{
				HxLOG_Error("Cur time(%d)(%s), please check ! \n",pstTestInfo->ulCurrTime, SAMA_SCHEDULE_GetStringOfUnixTime(pstTestInfo->ulCurrTime));
				HxLOG_Error("Sch time(%d)(%s), please check ! \n",pstTimerInfo->ulTime, SAMA_SCHEDULE_GetStringOfUnixTime(pstTimerInfo->ulTime));
			}

			pstTestInfo->pstExecList = HLIB_LIST_AppendSortEx(pstTestInfo->pstExecList, (void *)pstTimerInfo, sama_timer_cbTimerExecPriority);
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HERROR sama_timer_cmdExecuteTimerLists (sama_TimerManager_t *pstManager)
{
	UNIXTIME			 ulCurrTime;
//	SAMA_TimerItem_t	*pstTimerItem;
//	SAMA_TimerItem_t	*pstExecList = NULL;
	HxList_t			*pstListItem;
	sama_TimerTest_t	 stSamaTestInfo;

	ulCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

	// Semaphore �� ��Ȯ�� ó���� ���� Timer üũ, ���� ��Ʈ�� Timer Callback ��Ʈ�� �и��ؾ� �Ѵ�.
	ENTER_CRITICAL(pstManager);

	stSamaTestInfo.ulCurrTime = ulCurrTime;
	stSamaTestInfo.pstExecList = NULL;
	pstManager->pstTimerList = HLIB_LIST_RemoveIf(pstManager->pstTimerList, (void*)&stSamaTestInfo, sama_timer_compTimeCondition, NULL);

	LEAVE_CRITICAL(pstManager);

	// Execute the timer callbacks:
	if (NULL != stSamaTestInfo.pstExecList)
	{
		for (pstListItem = stSamaTestInfo.pstExecList; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			SAMA_TimerItem_t	*pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data(pstListItem);

			if (NULL != pstTimerItem)
			{
				if (NULL != pstTimerItem->pfCbFunc)
				{
					HxLOG_Debug("Excute timer, ulSlot: %d\n", pstTimerItem->ulSlot, pstTimerItem->eItemType);
					pstTimerItem->pfCbFunc(pstTimerItem->ulSlot, pstTimerItem->eItemType, pstTimerItem->ulTime);
				}

				sama_timer_freeTimer(pstManager, pstTimerItem);
			}
		}

		HLIB_LIST_RemoveAll(stSamaTestInfo.pstExecList);

	}

	return ERR_OK;
}

STATIC HERROR sama_timer_hapiCbOnSystemStateChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	PxSTATUS_e			 ePapiStatus;

	if (nArgc < 1)
	{
		HxLOG_Error("nArgc(%d) wrong\n", nArgc);
		return ERR_FAIL;
	}

	ePapiStatus = HxOBJECT_INT(apArgv[0]);

	SAMA_PostMessage(SAMAMESSAGE_PROCESSHAPISYSTEMSTATECHANGED, ePapiStatus, 0, 0, 0, 0, 0);

	return ERR_OK;
}

STATIC SAMA_ForEachResult_e sama_timer_LoadScheduleToTimer (DxSchedule_t *pstSchedule, HINT32 nArgc, HUINT32 *pulArgv)
{
	HBOOL					 bUpdated = FALSE;
	UNIXTIME				 utTime, utCurrTime, utStartTime, utEndTime;
	HCHAR					*szTimeStr;
	SAMA_TimerUpdateInfo_t	*pstUpdateInfo = (SAMA_TimerUpdateInfo_t *)pulArgv[0];
//	sama_TimerManager_t		*pstManager = sama_timer_getManager();
//	HERROR					 hErr;
	HBOOL					bFirstTimeUpdateAfterBoot = FALSE;
	HBOOL					bSync = TRUE;


	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

	if(pstUpdateInfo != NULL)
	{
		bSync = pstUpdateInfo->bSync;
	}

	HxLOG_Debug("[%s] Schedule Type (%d)[%s]\n", __FUNCTION__, pstSchedule->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstSchedule->eRsvType));
	// time ������ ���� ���¿��� rec �� ����(start)�ǰ�, �� ���Ŀ� time�� ���� ��쿡 ���� �����۾�. ����� �ȵ�.
	if (SAMA_SCHEDULE_IsScheduleVolatile(pstSchedule) == TRUE)
	{
		// bRemoved �� ���̵��� �ð� ���� �ʿ� ����.
		if (0 != pstUpdateInfo->ulAbsDiffTime && pstSchedule->bRemoved == FALSE)
		{
			UNIXTIME			 utLastStartTime, utNewStartTime;

			(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utLastStartTime);

			if (pstUpdateInfo->nDiffSign >= 0)
			{
				utNewStartTime = utLastStartTime + pstUpdateInfo->ulAbsDiffTime;
			}
			else
			{
				utNewStartTime = utLastStartTime - pstUpdateInfo->ulAbsDiffTime;
			}

			HxLOG_Error("[%s] Schedule (%d) Last StartTime(%s)\n", __FUNCTION__, pstSchedule->ulSlot, SAMA_SCHEDULE_GetStringOfUnixTime(utLastStartTime));
			HxLOG_Error("[%s] Schedule (%d) New  StartTime(%s)\n", __FUNCTION__, pstSchedule->ulSlot, SAMA_SCHEDULE_GetStringOfUnixTime(utNewStartTime));

			(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(utNewStartTime, &(pstSchedule->stStartTime));

			SAMA_SCHEDULE_UpdateSchedule(pstSchedule->ulSlot, FALSE, pstSchedule, SAMA_GetVirtualRequestId(), bSync);
		}
	}
	else
	{
		// bRemoved �� ���̵��� �ð� ���� �ʿ� ����.
		if(pstSchedule->bRemoved == FALSE)
		{
			if(pstUpdateInfo != NULL)
			{
				bFirstTimeUpdateAfterBoot = pstUpdateInfo->bFirstTimeUpdateAfterBoot;
			}

			(void)SAMA_SCHEDULE_AdjustTime(pstSchedule, &bUpdated);


			utStartTime = utCurrTime;
			(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utTime);
			SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utTime, &utStartTime, &utEndTime);


			HxLOG_Debug("pstSchedule->eStatus: %d, bFirstTimeUpdateAfterBoot: %d \n", pstSchedule->eStatus, bFirstTimeUpdateAfterBoot);
			switch (pstSchedule->eStatus)
			{
			case DxRSVSTATUS_WAITING:
			case DxRSVSTATUS_READY:
				utTime = (utStartTime + 5);
				break;

			case DxRSVSTATUS_RUNNING:
			default:
				// running �߿� power on/off ������ ���� item�� ���⼭ ������ ������, �ٽ� schedule�� ���󰡰� ��. (start time�� adjust �Ǿ.)
				// ���� �̷� ����� ���ϸ�, utTime = utEndTime �� ����ϸ� ��.
#ifdef SAMA_DO_NOT_RESTART_RUNNING_SCHEDUEL_AFTER_BOOT
				if ((bFirstTimeUpdateAfterBoot == TRUE) && (DxRSVTYPE_ACTIVATION_TIME != pstSchedule->eRsvType))
				{
					utTime = (utStartTime + 5);
				}
				else
#endif
				{
					utTime = utEndTime;
				}
				break;
			}

#if defined(CONFIG_DEBUG)
			HxLOG_Debug("utCurrTime: \n");
			HLIB_DATETIME_PrintDateTime(utCurrTime);
			HxLOG_Debug("utTime: \n");
			HLIB_DATETIME_PrintDateTime(utTime);
#endif
			if ((utCurrTime > utTime) && (DxRSVREPEAT_ONCE == pstSchedule->eRepeat))
			{
				HxLOG_Warning("Schedule(Slot:%d) expired for the time.\n", pstSchedule->ulSlot);

				szTimeStr = SAMA_SCHEDULE_GetStringOfUnixTime(utStartTime);
				HxLOG_Warning("Schedule Start Time : %s\n", szTimeStr);

				szTimeStr = SAMA_SCHEDULE_GetStringOfUnixTime(utCurrTime);
				HxLOG_Warning("Current Time 	   : %s\n", szTimeStr);

				if(pstUpdateInfo != NULL) {
					pstUpdateInfo->pstToBeRemoveSched = HLIB_LIST_Append(pstUpdateInfo->pstToBeRemoveSched, (void *)pstSchedule);
				}
			}
			else
			{
				//HxLOG_Debug("Print Schedule\n");
				//SAMA_SCHEDULE_PrintSchedule(pstSchedule);
				//HxLOG_Debug("bUpdated: %d\n", bUpdated);
				// �׳� ������ update �ص� �ȴ�. �����Ͱ� ���Ƶ� ũ�� ��� ������, Ȥ�ö� timer�� ���� �ȵǴ� issue ������.
				// ������ SAMA_SCHEDULE_UpdateSchedule �� call �ϸ� dama�� set�� �ϴ� ���� �Ⱦ... timer���� insert.
				//(�̹� sch���� ��� �ֱ� ������.)
				//if (TRUE == bUpdated)
				if (TRUE == bUpdated)
				{
					HxLOG_Debug("update Schedule:\n");
					SAMA_SCHEDULE_UpdateSchedule(pstSchedule->ulSlot, TRUE, pstSchedule, SAMA_GetVirtualRequestId(), bSync);
				}
				else
				{
					HxLOG_Debug("Insert the Schedule To Timer:\n");
					(void)SAMA_TIMER_RemoveTimer(pstSchedule->ulSlot);
					(void)SAMA_SCHEDULE_InsertScheduleToTimer(pstSchedule);
				}


			}
		}
	}

	return eSAMA_Result_OK;
}

STATIC HERROR sama_timer_processAfterSysTimeAvailable (HUINT32 ulAbsDiff, HINT32 nDiffSign, HBOOL bFirstTimeUpdateAfterBoot)
{
	SAMA_TimerUpdateInfo_t	 stUpdateInfo;
	HERROR			hErr;
	HUINT32					 aulArgv[1];

	HxLOG_Trace();

	HxSTD_MemSet(&stUpdateInfo, 0, sizeof(SAMA_TimerUpdateInfo_t));
	stUpdateInfo.ulAbsDiffTime	= ulAbsDiff;
	stUpdateInfo.nDiffSign		= nDiffSign;
	stUpdateInfo.bFirstTimeUpdateAfterBoot = bFirstTimeUpdateAfterBoot;
	stUpdateInfo.bSync			= FALSE;

	aulArgv[0] = (HUINT32)&stUpdateInfo;

	if (bFirstTimeUpdateAfterBoot == TRUE )
	{
		SAMA_SCHEDULE_Initialize();
		SAMA_PROTOCOL_NotifySystemTimeChanged(eSAPI_SYSTEM_TIME_CHANGED_TYPE_First_Available);
	}

	// volatile schedule �̿��� ������ ����Ѵ�.
	(void)SAMA_SCHEDULE_ForEach(sama_timer_LoadScheduleToTimer, 1, aulArgv);

	if (NULL != stUpdateInfo.pstToBeRemoveSched)
	{
		HxList_t			*pstListItem;
		DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

		for (pstListItem = stUpdateInfo.pstToBeRemoveSched; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

			if (NULL != pstSchedule)
			{
				stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Expired;
				SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), FALSE);
			}
		}

		HLIB_LIST_RemoveAll(stUpdateInfo.pstToBeRemoveSched);

	}

	hErr=SAMA_TIMER_ExecuteTimer();
	if(hErr != ERR_OK)
	{
		HxLOG_Critical(" SAMA_TIMER_ExecuteTimer Error!!\n");	
	}

	(void)DAPI_Sync(DxNAMEOF(DxSchedule_t));

	return ERR_OK;
}
#if 0
STATIC HERROR sama_timer_reloadSchedulerToTimer (HUINT32 ulAbsDiff, HINT32 nDiffSign)
{
	SAMA_TimerUpdateInfo_t	 stUpdateInfo;
	HUINT32					 aulArgv[1];

	HxSTD_MemSet(&stUpdateInfo, 0, sizeof(SAMA_TimerUpdateInfo_t));
	stUpdateInfo.ulAbsDiffTime	= ulAbsDiff;
	stUpdateInfo.nDiffSign		= nDiffSign;

	aulArgv[0] = (HUINT32)&stUpdateInfo;

	// volatile schedule �̿��� ������ ����Ѵ�.
	(void)SAMA_SCHEDULE_ForEach(sama_timer_LoadScheduleToTimer, 1, aulArgv);

	if (NULL != stUpdateInfo.pstToBeRemoveSched)
	{
		HxList_t			*pstListItem;

		for (pstListItem = stUpdateInfo.pstToBeRemoveSched; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

			if (NULL != pstSchedule)
			{
				SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, DxSchedule_RemovedReason_Expired, 0, FALSE, SAMA_GetVirtualRequestId(), TRUE);
			}
		}

		HLIB_LIST_RemoveAll(stUpdateInfo.pstToBeRemoveSched);

	}

	return ERR_OK;
}
#endif

STATIC void sama_timer_task (void *pvArg)
{
	HBOOL					 bSysTimeAvail = FALSE;
	UNIXTIME				 utCurrTime, utLastTime;
	HUINT32					 ulCurrTick, ulLastTick, ulDiffTick;
	HUINT32					 ulAbsDiff;
	HINT32					 nDiffSign;
	sama_TimerMsg_t			 stMsg;
	sama_TimerManager_t		*pstManager = (sama_TimerManager_t *)pvArg;
	HERROR					 hErr;
	HBOOL					bIsFirstLaunch = TRUE;
	HxFILE_t				hFile;

	if (NULL == pstManager)
	{
		HxLOG_Critical("Context (pvArg) NULL !!!\n");
		return;
	}


	if (HLIB_FILE_Exist("tmp/samalaunched") == FALSE)
	{
		HxLOG_Error("create /tmp/samalaunched \n");
		hFile = HLIB_FILE_Open("/tmp/samalaunched", "w+");
		if (hFile != NULL)
		{
			HLIB_FILE_Close(hFile);
		}
		bIsFirstLaunch = TRUE;
	}
	else
	{
		bIsFirstLaunch = FALSE;
	}

	HxLOG_Error("bIsFirstLaunch: %d \n", bIsFirstLaunch);

	// �ϴ� Schedule List ���ʹ� �о� ���´� : SAPI�� �ִ��� ���� cache update �Ǿ�� �ϴϱ�
	SAMA_SCHEDULE_LoadScheduleData(bIsFirstLaunch);

	hErr = VK_TIMER_EventEvery(1000, sama_timer_cbTimerOneSec, (void *)&pstManager, sizeof(sama_TimerManager_t *), &(pstManager->ulOneSecTimerId));
	if (VK_OK != hErr)
	{
		HxLOG_Critical("VK_TIMER_EventEvery Failed\n");
	}

	pstManager->bReady = TRUE;

	// Initialize the scheduler after the system time is available:
	SAMA_SCHEDULE_StartTimer(bIsFirstLaunch);

	(void)HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, sama_timer_hapiCbOnSystemStateChanged);

	utLastTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	ulLastTick = HLIB_STD_GetSystemTick() / 1000;

	// Timer Task:
	while (1)
	{
		hErr = VK_MSG_Receive(pstManager->ulMsgQueueId, &stMsg, sizeof(sama_TimerMsg_t));
		if (VK_OK != hErr)
		{
			HxLOG_Error ("VK_MSG_Receive err:\n");
			continue;
		}

		utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
		ulCurrTick = HLIB_STD_GetSystemTick() / 1000;

		if ((FALSE == bSysTimeAvail) && (utCurrTime > SAMA_TIME_AVAIL_MIN))
		{
			HxLOG_Error("send, SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE at first time update!\n");
			// ó�� �ð� ���� ���.
			SAMA_PostMessage(SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE, 0, 0, TRUE, 0, 0, 0);
			bSysTimeAvail = TRUE;
		}

		ulDiffTick = ulCurrTick - ulLastTick;
		if (utCurrTime >= utLastTime + ulDiffTick)
		{
			ulAbsDiff = utCurrTime - utLastTime - ulDiffTick;
			nDiffSign = 1;
		}
		else
		{
			ulAbsDiff = utLastTime + ulDiffTick - utCurrTime;
			nDiffSign = -1;
		}

		if (ulAbsDiff >= (3 * 60))			// 3�� �̻� ����
		{
			HxLOG_Error("Time Update : LastTime(%s)\n", SAMA_SCHEDULE_GetStringOfUnixTime(utLastTime));
			HxLOG_Error("            : CurrTime(%s)\n", SAMA_SCHEDULE_GetStringOfUnixTime(utCurrTime));
			HxLOG_Error("            : ABS Diff(%d), Sign(%d)\n", ulAbsDiff, nDiffSign);
			HxLOG_Error("send, SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE at time difference occured!\n");

			// �ð��� Ƥ ���.
			SAMA_PostMessage(SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE, ulAbsDiff, nDiffSign, FALSE, 0, 0, 0);
			//sama_timer_reloadSchedulerToTimer(ulAbsDiff, nDiffSign);
		}

		utLastTime = utCurrTime;
		ulLastTick = ulCurrTick;

		// TODO: ���ľ� �� ��...
		//���⼭ timer thread���� SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE�� ������ main thread���� ó���ϴµ�,
		//���⼭ timer cb�� ���۽�Ų��... SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE �� ����� ó�� �� �Ǿ� ���� Ȯ���� �ִ�.
		//������ SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE �� sync ó�� �� ������ �� �����ϴ�. sama�� ���� rpc ��û��� �ٸ� thread���� ó���ع����� �ȵǱ� ������...
		// �� �κ��� ������ �Ǿ�� ������, ���� ������ �� Ȯ���� ����. (SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE �߻� �� timer cb ������ ���ÿ� �Ͼ Ȯ���� ����..)
		// �׸��� sem �� �ɷ������Ƿ�, �� �� ����غ���..
		// (������ 1�ʸ��� timer ��� �߻��ϰ�, SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE �� ���� ������� ���� sama_timer_cmdExecuteTimerLists�� ������ �ȵǴ���,
		// �ᱹ�� �ݿ��� �� ����..)
		switch (stMsg.eMsgType)
		{
		case eSAMA_TIMER_MSGTYPE_OneSec:
		case eSAMA_TIMER_MSGTYPE_ExecSch:
			hErr = sama_timer_cmdExecuteTimerLists (pstManager);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("sama_timer_cmdExecuteTimerLists err:\n");
			}

			break;

		default:
			break;
		}
	}
}

STATIC HERROR sama_timer_initContextVk (sama_TimerManager_t *pstManager)
{
	HERROR				 hErr;

	SAMA_MemSet (pstManager, 0, sizeof(sama_TimerManager_t));

#if defined(USE_SAMA_TIMER_SEMAPHORE)
	hErr = VK_SEM_Create (&(pstManager->ulSemId), "SamaTimerSem", VK_SUSPENDTYPE_PRIORITY);
	if (VK_OK != hErr)
	{
		HxLOG_Critical("VK_SEM_Create Failed\n");
		return ERR_FAIL;
	}
#endif

	hErr = VK_MSG_Create (1024, sizeof(sama_TimerMsg_t), (const char *)"SamaTimerQ", (unsigned long *)&(pstManager->ulMsgQueueId), VK_SUSPENDTYPE_FIFO);
	if (VK_OK != hErr)
	{
		HxLOG_Critical("VK_MSG_Create Failed\n");
		return ERR_FAIL;
	}

	hErr = VK_TASK_Create (sama_timer_task, 64, 64*1024, "tSamaTimer", (void *)pstManager, &(pstManager->ulTaskId), VK_SUSPENDTYPE_FIFO);
	if (VK_OK != hErr)
	{
		HxLOG_Critical("VK_TASK_Create Failed\n");
		return ERR_FAIL;
	}

	hErr = VK_TASK_Start (pstManager->ulTaskId);
	if (VK_OK != hErr)
	{
		HxLOG_Critical("VK_TASK_Start Failed\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define ___GLOBAL_FUNCTIONS___

HERROR	SAMA_TIMER_ProcessAfterSystemTimeAvailable(HUINT32 ulAbsDiff, HINT32 nDiffSign, HBOOL bFirstTimeUpdateAfterBoot)
{
	return sama_timer_processAfterSysTimeAvailable(ulAbsDiff, nDiffSign, bFirstTimeUpdateAfterBoot);
}

HERROR SAMA_TIMER_ProcessHapiSystemStateChanged (PxSTATUS_e ePapiStatus)
{
	UNIXTIME			 utCurrTime;
	HCHAR				*szTimeStr;
	SAMA_Context_t		*pstContext = SAMA_GetContext();


	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

	HxLOG_Debug("papi status (%d) \n", ePapiStatus);

	switch(ePapiStatus)
	{
	case ePAMA_STATUS_Shutdown:
		{
			UNIXTIME		 utStartTime = 0, utEndTime;
			DxSchedule_t	*pstSchedule = NULL;
			UNIXTIME		 utStartTimeWithPaddingAndRsvReady = 0;

			pstSchedule = SAMA_SCHEDULE_GetEarliestScheduleWithPaddingAndReadyTime(&utStartTimeWithPaddingAndRsvReady);

			if (NULL != pstSchedule && utStartTimeWithPaddingAndRsvReady != 0)
			{
				HxLOG_Debug("Earliest Schedule:\n");
				SAMA_SCHEDULE_PrintSchedule(pstSchedule);

				(void)SAMALIB_TIME_GetRepeatedScheduleStartTime(pstSchedule, utCurrTime, &utStartTime);
				SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utStartTime, NULL, &utEndTime);

				// for Debugging:
				szTimeStr = SAMA_SCHEDULE_GetStringOfUnixTime(utCurrTime);
				HxLOG_Debug("CurrTime  (%s)\n", szTimeStr);

				szTimeStr = SAMA_SCHEDULE_GetStringOfUnixTime(utStartTimeWithPaddingAndRsvReady);
				HxLOG_Debug("StartTime With Padding and RsvReady (%s)\n", szTimeStr);

				szTimeStr = SAMA_SCHEDULE_GetStringOfUnixTime(utStartTime);
				HxLOG_Debug("StartTime (%s)\n", szTimeStr);

				switch ( pstContext->stSpecOption.eSpec )
				{
                    case eDxSchedule_Spec_TvAnyTime:
						if ( SAMALIB_TRACKING_IsTrackingBySlotId(pstSchedule->ulSlot) )
						{
							// tracking ���� schedule�� padding�� �������� �ʴ´�.
							utEndTime = utStartTime + pstSchedule->ulDuration;
							if ( utCurrTime >= utEndTime )
							{
								// tracking ���� schedule�� currtime�� end time ���� �̷��� delay �� �� �ִ�.
								// �� ��� 60 seconds wait �ϵ��� �Ѵ�.
								HxLOG_Debug("Tracking schedule. delay 60 seconds. slot=(%d) \n",pstSchedule->ulSlot);
								utEndTime = utCurrTime + 60;
							}
						}
						break;
					default:
						break;
				}
				szTimeStr = SAMA_SCHEDULE_GetStringOfUnixTime(utEndTime);
				HxLOG_Debug("EndTime  With Padding (%s)\n", szTimeStr);

				if ((utCurrTime + pstContext->stSpecOption.ulShutdownWaitTime) > utStartTimeWithPaddingAndRsvReady)
				{
					HxLOG_Debug("wait shutdown for %d milisec \n", (utEndTime - utCurrTime)*1000);
					PAPI_WaitForShutDown((utEndTime - utCurrTime)*1000);
				}
			}
		}

		break;

	default:
		break;
	}

	return ERR_OK;
}


HERROR SAMA_TIMER_InitTimer (void)
{
	sama_TimerManager_t	*pstManager = sama_timer_getManager();
	HERROR				 hErr, hResult = ERR_FAIL;

	HxLOG_Info ("(+)\n");

	hErr = sama_timer_initContextVk (pstManager);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("sama_timer_initContextVk err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	HxLOG_Info ("(-) hResult(0x%08x)\n", hResult);
	return hResult;
}

HERROR SAMA_TIMER_GetTimerTime (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long *pulTime)
{
	sama_TimerManager_t	*pstManager = sama_timer_getManager();
	HERROR				 hErr;

	ENTER_CRITICAL(pstManager);
	hErr = sama_timer_getTimerTime (pstManager, ulSlot, eTimerType, pulTime);
	LEAVE_CRITICAL(pstManager);

	return hErr;
}


HERROR SAMA_TIMER_InsertTimer (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, DxRsvType_e eRsvType, unsigned long ulTime, SAMA_CbTimerExec pfCallback, HBOOL bPause)
{
	sama_TimerManager_t	*pstManager = sama_timer_getManager();
	HERROR				 hErr;

	ENTER_CRITICAL(pstManager);
	hErr = sama_timer_insertTimer (pstManager, ulSlot, eTimerType, eRsvType, ulTime, pfCallback, bPause);
	LEAVE_CRITICAL(pstManager);

	return hErr;
}

HERROR SAMA_TIMER_RemoveTimer (HUINT32 ulSlot)
{
	sama_TimerManager_t	*pstManager = sama_timer_getManager();
	HERROR				 hErr;

	ENTER_CRITICAL(pstManager);
	hErr = sama_timer_removeTimer (pstManager, ulSlot);
	LEAVE_CRITICAL(pstManager);

	return hErr;
}

HERROR SAMA_TIMER_ExecuteTimer (void)
{
	sama_TimerManager_t	*pstManager = sama_timer_getManager();
	HERROR				 hErr;
	ENTER_CRITICAL(pstManager);
	hErr = sama_timer_ExecuteTimer (pstManager);
	LEAVE_CRITICAL(pstManager);
	return hErr;
}
HUINT32 SAMA_TIMER_CountRemainingTimer (HUINT32 ulSlot)
{
	HUINT32				 ulCount;
	sama_TimerManager_t	*pstManager = sama_timer_getManager();

	ENTER_CRITICAL(pstManager);
	ulCount = sama_timer_countRemainingTimer(pstManager, ulSlot);
	LEAVE_CRITICAL(pstManager);

	return ulCount;
}

HERROR SAMA_TIMER_ChangeTimerTime (DxSchedule_t *pstSchedule)
{
	sama_TimerManager_t	*pstManager = sama_timer_getManager();
	HERROR				 hErr;

	ENTER_CRITICAL(pstManager);
	hErr = sama_timer_changeTimerTime(pstManager, pstSchedule);
	LEAVE_CRITICAL(pstManager);

	return hErr;
}

HERROR SAMA_TIMER_ForEach (SAMA_ForEachResult_e (*pfCbFunc)(SAMA_TimerItem_t *pstTimer, HINT32 nArgc, HUINT32 *pulArgv), HINT32 nArgc, HUINT32 *pulArgv)
{
	sama_TimerManager_t		*pstManager = sama_timer_getManager();
	HERROR					 hErr;

	if (NULL == pfCbFunc)					{ return ERR_FAIL; }

	ENTER_CRITICAL(pstManager);
	hErr = sama_timer_forEach(pstManager, pfCbFunc, nArgc, pulArgv);
	LEAVE_CRITICAL(pstManager);

	return hErr;
}

#define _____DEBUGGER_FUNCTIONS_____
#if defined(CONFIG_DEBUG)
STATIC HINT32 sapi_timer_cbCmpTimerItemByTime (void *pvUser, void *pvListItem)
{
	SAMA_TimerItem_t	*pstUserData = (SAMA_TimerItem_t *)pvUser;
	SAMA_TimerItem_t	*pstListItem = (SAMA_TimerItem_t *)pvListItem;

	if (pstUserData->ulTime > pstListItem->ulTime)
	{
		return -1;
	}
	else if (pstUserData->ulTime < pstListItem->ulTime)
	{
		return 1;
	}

	return 0;
}

STATIC HCHAR *sapi_timer_getStringOfTimerItemType (SAPI_TimerItemType_e eType)
{
	switch (eType)
	{
	case eSAPI_TIMER_TYPE_None:					return "NONE";
	case eSAPI_TIMER_TYPE_Ready:				return "READY";
	case eSAPI_TIMER_TYPE_Start:				return "START";
	case eSAPI_TIMER_TYPE_Stop:					return "STOP";
#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
	case eSAPI_TIMER_TYPE_PaddingStartReady:					return "READY_PAD_START";
	case eSAPI_TIMER_TYPE_PaddingStopReady:					return "READY_PAD_STOP";
#endif
	case eSAPI_TIMER_TYPE_CheckSeriesRecordReady:				return "READY_CHECK_SERIES_RECORD";
	case eSAPI_TIMER_TYPE_StopPendingBroken:					return "PENDING_BROKEN";
	case eSAPI_TIMER_TYPE_StopRunaway:					return "RUNAWAY";
	case eSAPI_TIMER_TYPE_ReadyTracking:				return "TRACKING";


	default:
		break;
	}

	return "UNKNOWN";
}

HERROR SAMA_TIMER_PrintAllTimer (HERROR (*pfPrintFunc)(const HCHAR *format, ...))
{
	HxList_t			*pstListItem, *pstTimeSorted = NULL;
	sama_TimerManager_t	*pstManager = sama_timer_getManager();

	if (NULL == pfPrintFunc)					{ return ERR_FAIL; }

	ENTER_CRITICAL(pstManager);

	for (pstListItem = pstManager->pstTimerList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SAMA_TimerItem_t		*pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstTimerItem)
		{
			pstTimeSorted = HLIB_LIST_AppendSortEx(pstTimeSorted, (void *)pstTimerItem, sapi_timer_cbCmpTimerItemByTime);
		}
	}

	LEAVE_CRITICAL(pstManager);

	pfPrintFunc("[SAMA] Timer Status: Time Ascending Sorted\n");

	for (pstListItem = pstTimeSorted ; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SAMA_TimerItem_t		*pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstTimerItem)
		{
			pfPrintFunc("Timer SlotID(%04d), TimerType(%s), Time(%s), (%s) \n",
									pstTimerItem->ulSlot,
									sapi_timer_getStringOfTimerItemType(pstTimerItem->eItemType),
									SAMA_SCHEDULE_GetStringOfUnixTime(pstTimerItem->ulTime),
									(TRUE == pstTimerItem->bPause) ? "Paused" : "Running");
		}
	}

	pfPrintFunc("[SAMA] ---------------------------------------------\n");

	HLIB_LIST_RemoveAll(pstTimeSorted);

	return ERR_OK;
}


HERROR SAMA_TIMER_PrintLogAllTimer (void)
{
	HxList_t			*pstListItem, *pstTimeSorted = NULL;
	sama_TimerManager_t	*pstManager = sama_timer_getManager();


	ENTER_CRITICAL(pstManager);

	for (pstListItem = pstManager->pstTimerList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SAMA_TimerItem_t		*pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstTimerItem)
		{
			pstTimeSorted = HLIB_LIST_AppendSortEx(pstTimeSorted, (void *)pstTimerItem, sapi_timer_cbCmpTimerItemByTime);
		}
	}

	LEAVE_CRITICAL(pstManager);

	HxLOG_Print("[SAMA] Timer Status: Time Ascending Sorted\n");

	for (pstListItem = pstTimeSorted ; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SAMA_TimerItem_t		*pstTimerItem = (SAMA_TimerItem_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstTimerItem)
		{
			HxLOG_Print("Timer SlotID(%04d), TimerType(%s), Time(%s), (%s) \n",
									pstTimerItem->ulSlot,
									sapi_timer_getStringOfTimerItemType(pstTimerItem->eItemType),
									SAMA_SCHEDULE_GetStringOfUnixTime(pstTimerItem->ulTime),
									(TRUE == pstTimerItem->bPause) ? "Paused" : "Running");
		}
	}

	HxLOG_Print("[SAMA] ---------------------------------------------\n");

	HLIB_LIST_RemoveAll(pstTimeSorted);

	return ERR_OK;
}
#endif



