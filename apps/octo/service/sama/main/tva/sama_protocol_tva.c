/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_protocol_tva.c
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

STATIC HUINT32 sama_prtc_tva_GetMaxSlotByType (DxRsvType_e eRsvType)
{
	// 호출시에 type을 체크한다.
	switch (eRsvType)
	{
		case DxRSVTYPE_REGULAR_OTA:	return 1;
		case DxRSVTYPE_ASO_NOTIFY:		return 1;
		default: 	return 1;
	}

}

STATIC void sama_prtc_tva_FindAndDeleteSameTimeSchedules (DxSchedule_t *pstNewSchedule)
{
	HUINT32 ulCount,i;
	DxSchedule_t	*pstToDeleteSchedule=NULL;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	DxRsvType_e eRsvType;

	if (pstNewSchedule == NULL)
	{
		return;
	}

	eRsvType = pstNewSchedule->eRsvType;
	stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;
	ulCount = SAMA_SCHEDULE_CountScheduleByType(eRsvType);
	for (i=0; i<ulCount; i++)
	{
		pstToDeleteSchedule = SAMA_SCHEDULE_GetScheduleByType(eRsvType,i);
		if (pstToDeleteSchedule == NULL ) continue;

		if (SAMA_Memcmp((&pstNewSchedule->stStartTime),  &(pstToDeleteSchedule->stStartTime), sizeof(HxDATETIME_t)) != 0)
		{
			continue;
		}

		HxLOG_Debug("Delete SameTime Schedule. type=(%d) new slot=(%d), delete slot=(%d) \n", eRsvType, pstNewSchedule->ulSlot, pstToDeleteSchedule->ulSlot);
		SAMA_SCHEDULE_RemoveSchedule (pstToDeleteSchedule->ulSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
	}
}

STATIC void sama_prtc_tva_FindAndDeleteOverMaxSlotSchedules (DxSchedule_t *pstNewSchedule)
{
	HUINT32 ulCount,i,ulSlot;
	DxSchedule_t	*pstToDeleteSchedule=NULL;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	DxRsvType_e eRsvType;
	HUINT32			ulMaxSlot;

	if (pstNewSchedule == NULL)
	{
		return;
	}

	eRsvType = pstNewSchedule->eRsvType;
	if ( !(eRsvType == DxRSVTYPE_ASO_NOTIFY
		    || eRsvType == DxRSVTYPE_REGULAR_OTA))
	{
		return;
	}

	ulMaxSlot = sama_prtc_tva_GetMaxSlotByType(eRsvType);

	stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;
	ulCount = SAMA_SCHEDULE_CountScheduleByType(eRsvType);

	for (i=ulMaxSlot-1; i<ulCount; i++)
	{
		pstToDeleteSchedule = SAMA_SCHEDULE_GetScheduleByType(eRsvType,i);
		if (pstToDeleteSchedule == NULL ) continue;

		ulSlot = pstToDeleteSchedule->ulSlot;
		HxLOG_Debug("Delete Over Max Slot schedule. type=(%d) new slot=(%d), delete slot=(%d) \n", eRsvType,pstNewSchedule->ulSlot,pstToDeleteSchedule->ulSlot);
		SAMA_SCHEDULE_RemoveSchedule (ulSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
	}
}


STATIC void sama_prtc_tva_FindAndDeleteRsvType (DxSchedule_t *pstSchedule)
{
	DxRsvType_e eRsvType;

	if (pstSchedule == NULL)
	{
		return;
	}

	eRsvType = pstSchedule->eRsvType;

	if (eRsvType == DxRSVTYPE_ASO_NOTIFY)
	{
		// delete same time schedules
		sama_prtc_tva_FindAndDeleteSameTimeSchedules(pstSchedule);
	}

	if (eRsvType == DxRSVTYPE_ASO_NOTIFY
		|| eRsvType == DxRSVTYPE_REGULAR_OTA)
	{
		// delete extra number of schedules
		sama_prtc_tva_FindAndDeleteOverMaxSlotSchedules(pstSchedule);
	}
}

STATIC HINT32  sama_prtc_tva_CompareSlot (const void *arg1, const void *arg2)
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

STATIC HERROR sama_prtc_tva_CancelTvaSchedule (SERIES_SET *pstSeriesSet, HUINT32 ulSlot,HINT32 nRequestId)
{
	/***************************************************
	* cancel은 다음의 9개의 경우를 모두 다룰 수 있어야 한다.
	* 0. 일반 schedule 삭제
	* 1. single programme 삭제
	* 2. single split 삭제
	* 3. series programme 삭제
	* 4. series split 삭제
	* 5. last series programme 삭제
	* 6. last series split 삭제
	* 7. tracking series programme 삭제
	* 8. tracking series split 삭제
	****************************************************/

	// case. series
	PROGRAM_SET		*pstProgSet;
	INSTANCE_SET	*pstInstSet;
	EVENT_PERIOD	*pstEventPeriod;

	INSTANCE_SET	*pstRemoveInstSet = NULL;
	HUINT32			ulRemoveSlot;
	HUINT32			i = 0;
	HUINT32			ulTotalRemoveSlots = 0;
	HERROR 			hErr = ERR_OK;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	HxVector_t		*pstRemoveSlots = NULL;

	pstRemoveSlots = HLIB_VECTOR_NewEasy(32, NULL, (HxCompareFunc_t)sama_prtc_tva_CompareSlot);
	if (pstRemoveSlots == NULL)
	{
		HxLOG_Error("memalloc failed. \n");
		return ERR_FAIL;
	}

	// 1. split를 다 지워야 한다. (instance set 단위로 지워야 한다.)
	for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet != NULL; pstProgSet=pstProgSet->pNext)
	{
		for (pstInstSet = pstProgSet->pstSelectedInst; pstInstSet != NULL; pstInstSet = pstInstSet->pNext)
		{
			for (pstEventPeriod = pstInstSet->pstEventList; pstEventPeriod != NULL; pstEventPeriod = pstEventPeriod->pNext)
			{
				if (pstEventPeriod->ulSlot == ulSlot)
				{
					pstRemoveInstSet = pstInstSet;
					break;
				}
			}
			if (pstRemoveInstSet != NULL) break;
		}
		if (pstRemoveInstSet != NULL) break;
	}

	if (pstRemoveInstSet == NULL)
	{
		HxLOG_Error("No schedules to cancel. slot=(%d) \n",ulSlot);
		return ERR_FAIL;
			}

	for (pstEventPeriod=pstRemoveInstSet->pstEventList; pstEventPeriod != NULL; pstEventPeriod=pstEventPeriod->pNext)
	{
		HLIB_VECTOR_Add(pstRemoveSlots, (void *)pstEventPeriod->ulSlot);
	}

	ulTotalRemoveSlots = HLIB_VECTOR_Length(pstRemoveSlots);
	HxLOG_Debug("Remove %d slots \n",ulTotalRemoveSlots);
	for (i=0; i<ulTotalRemoveSlots; i++)
	{
		ulRemoveSlot = (HUINT32)HLIB_VECTOR_ItemAt(pstRemoveSlots,i);
		stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;
		hErr |= SAMA_SCHEDULE_RemoveSchedule(ulRemoveSlot, &stRemovedInfo, FALSE, nRequestId, TRUE);
	}

	HLIB_VECTOR_Delete(pstRemoveSlots);

	return hErr;
}


STATIC HERROR sama_prtc_tva_CancelWholeSeriesSchedule (SERIES_SET *pstSeriesSet, HUINT32 ulSlot,HINT32 nRequestId)
{
	PROGRAM_SET		*pstProgSet;
	EVENT_PERIOD	*pstEventPeriod;

	INSTANCE_SET	*pstRemoveInstSet = NULL;
	HUINT32			ulRemoveSlot;
	HUINT32			i = 0;
	HUINT32			ulTotalRemoveSlots = 0;
	HERROR 			hErr = ERR_OK;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	HxVector_t		*pstRemoveSlots = NULL;

	pstRemoveSlots = HLIB_VECTOR_NewEasy(32, NULL, (HxCompareFunc_t)sama_prtc_tva_CompareSlot);
	if (pstRemoveSlots == NULL)
	{
		HxLOG_Error("memalloc failed. \n");
		return ERR_FAIL;
	}

	for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet != NULL; pstProgSet=pstProgSet->pNext)
	{
		pstRemoveInstSet = pstProgSet->pstSelectedInst;
		for (pstEventPeriod=pstRemoveInstSet->pstEventList; pstEventPeriod != NULL; pstEventPeriod=pstEventPeriod->pNext)
			{
			HLIB_VECTOR_Add(pstRemoveSlots, (void *)pstEventPeriod->ulSlot);
		}
	}
	SAMALIB_TVA_DeleteSeriesSet(pstSeriesSet);

	ulTotalRemoveSlots = HLIB_VECTOR_Length(pstRemoveSlots);
	HxLOG_Debug("Remove %d slots \n",ulTotalRemoveSlots);
	for (i=0; i<ulTotalRemoveSlots; i++)
	{
		// DeleteSeriesSet과 RemoveSchedule의 순서를 지켜줘야 한다. 왜냐하면 SeriesSet이 있는 상태에서 remove는 series tracking schedule을 만들기 때문이다.
		// series tracking schedule을 만들지 않기 위해서는 DeleteSeriesSet -> RemoveSchedule의 순서로 해야 한다
		ulRemoveSlot = (HUINT32)HLIB_VECTOR_ItemAt(pstRemoveSlots,i);
		stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;
		hErr |= SAMA_SCHEDULE_RemoveSchedule(ulRemoveSlot, &stRemovedInfo, FALSE, nRequestId, TRUE);
	}

	HLIB_VECTOR_Delete(pstRemoveSlots);

	return hErr;
}

#define _____RPC_METHOD_REGISTRATION_____
// sapi.init (int nPid)
STATIC HERROR sama_prtc_tva_cmdInit (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32			 nPid;
	HUINT32			 ulCnt, ulSlotNum;
	HUINT32			 aulSlotArray[SAMA_EMPTY_SLOT_PER_PROCESS];
	SAMA_Context_t	*pstContext = SAMA_GetContext();
	HERROR			 hErr;

	if (nArgc != 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		return ERR_FAIL;
	}

	nPid = (HINT32)HxOBJECT_INT (apArgv[0]);

	// 해당되는 Process에게 새로 Slot을 할당해서 보내 준다.
	hErr = SAMA_SCHEDULE_GetEmptySlotsForSapi (nPid, &ulSlotNum, aulSlotArray);
	if (ERR_OK == hErr)
	{
		(void)SAMA_PROTOCOL_NotifyAvailableSlotsChanged (nPid, ulSlotNum, aulSlotArray, nRequestId);
	}
	else
	{
		HxLOG_Error ("SAMA_SCHEDULE_GetEmptySlotsForSapi err:\n");
	}

	// 이미 Resource가 Set 상태라면 Resource Notify 도 알려준다.
	if (TRUE == pstContext->stSamaConflictCheck.bRscSet)
	{
		(void)SAMA_PROTOCOL_NotifyRscInfoSet (&(pstContext->stSamaConflictCheck.stRscInfo), nRequestId);
	}

	// 이미 RF-Input Ability Set 상태라면 이 정보 역시 Notify로 알려줘야 한다.
	for (ulCnt = 0; ulCnt < RxRFINPUT_NUM_MAX; ulCnt++)
	{
		if (pstContext->stSamaConflictCheck.anRfInputAbility[ulCnt] >= 0)
		{
			(void)SAMA_PROTOCOL_NotifyRfInputAbilitySet (ulCnt, (HBOOL)pstContext->stSamaConflictCheck.anRfInputAbility[ulCnt], nRequestId);
		}
	}

	return ERR_OK;
}


// sapi.term (void)
STATIC HERROR sama_prtc_tva_cmdTerm (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	return ERR_OK;
}

// sapi.setRscInfo (SAPI_RscInfo_t *pstRscInfo)
STATIC HERROR sama_prtc_tva_cmdSetRscInfo (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	SAPI_RscInfo_t		*pstRscInfo;
	HERROR				 hErr, hResult = ERR_FAIL;

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	pstRscInfo = (SAPI_RscInfo_t *)HxOBJECT_BIN_DATA (apArgv[0]);
	if (NULL == pstRscInfo)
	{
		HxLOG_Error ("pstRscInfo NULL\n");
		goto END_FUNC;
	}

	hErr = SAMA_RSC_SetResourceInfo (pstRscInfo, nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_RSC_SetResourceInfo err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

// sapi.setRfInputAbility (int nRfInputId, int nAbility)
STATIC HERROR sama_prtc_tva_cmdSetRfInputAbility (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL				 bAbility;
	HUINT32				 ulRfInputId;
	HERROR				 hErr, hResult = ERR_FAIL;

	if (nArgc < 2)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulRfInputId = (HUINT32)HxOBJECT_INT (apArgv[0]);
	bAbility = (HBOOL)HxOBJECT_INT (apArgv[1]);

	hErr = SAMA_RSC_SetRfInputAbility (ulRfInputId, bAbility, nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_RSC_SetRfInputAbility err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

// sapi.refreshSchedule (void)
STATIC HERROR sama_prtc_tva_cmdRefreshSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	(void)SAMA_SCHEDULE_SendAllScheduleToSapi(nRequestId);
	return ERR_OK;
}

// sapi.makeSchedule (int nSlot, DxSchedule_t *pstSchedule)
STATIC HERROR sama_prtc_tva_cmdMakeSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulSlot;
	DxSchedule_t		*pstSchedule = NULL;
	HERROR				 hErr, hResult = ERR_FAIL;
	DxSchedule_t		*pstScheduleForSeriesRecord = NULL;
	HUINT32				ulSeriesIndex = 0;
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	if (nArgc < 2)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	HxLOG_Trace();

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		goto END_FUNC;
	}


	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	pstSchedule	= HxOBJECT_BIN_DATA (apArgv[1]);
	if (NULL == pstSchedule)
	{
		HxLOG_Error ("Schedule NULL\n");
		goto END_FUNC;
	}

	// client 입장에서 Sapi_MakeSchedule ~ sapi_base_OnScheduleAdded 사이에는 MakeSchedule의 결과를 받을 수 없다.
	// 그러므로 client에서 MakeSchedule 하기전에 체크해야 하는 부분은 server에서 처리해준다.
	if (pstSchedule->eRsvType == DxRSVTYPE_ASO_NOTIFY)
	{
		sama_prtc_tva_FindAndDeleteRsvType(pstSchedule);
	}

	if ((TRUE != pstContext->bSama_TvaInited)
		&&(MACRO_IS_TVA_SCHEDULE_TYPE(pstSchedule->eRsvType)))
	{
		HxLOG_Error("Tva not loaded yet. possibly because of time load.\n");
		goto END_FUNC;
	}

 	if (MACRO_IS_TVA_SCHEDULE_TYPE(pstSchedule->eRsvType))
	{
		pstScheduleForSeriesRecord = SAMA_Calloc(sizeof(DxSchedule_t));
		if(pstScheduleForSeriesRecord != NULL)
		{
			// free는 받는 애가.
			SAMA_MemCpy(pstScheduleForSeriesRecord, pstSchedule, sizeof(DxSchedule_t));
			// series rec할 schedule은 여러개의 series CRID를 가질 수 있다. 이에 따라 series rec시에는 series CRID에 index 값을 선택한다.
			ulSeriesIndex = 0; // 첫 번째로 찾을 수 있는 series CRID에 대한 series
			SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD, (HINT32)pstScheduleForSeriesRecord, ulSeriesIndex, 0, 0, 0, 0);
		}
		hResult = ERR_OK;
		goto END_FUNC;
	}

	hErr = SAMA_SCHEDULE_AddSchedule (ulSlot, pstSchedule, FALSE, nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_SCHEDULE_MakeScheduler err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	if ( hResult != ERR_OK
		&& (pstSchedule != NULL)
		&& (MACRO_IS_TVA_SCHEDULE_TYPE(pstSchedule->eRsvType)))
	{
		HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL \033[0m \n",__FUNCTION__, __LINE__);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstScheduleForSeriesRecord, 0, 0, 0, 0, 0);
	}

	return hResult;
}

// sapi.updateSchedule (int nSlot, DxSchedule_t *pstSchedule)
STATIC HERROR sama_prtc_tva_cmdUpdateSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulSlot;
	DxSchedule_t		*pstSchedule;
	HERROR				 hErr, hResult = ERR_FAIL;

	if (nArgc < 2)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	pstSchedule	= HxOBJECT_BIN_DATA (apArgv[1]);
	if (NULL == pstSchedule)
	{
		HxLOG_Error ("Schedule NULL\n");
		goto END_FUNC;
	}

	if ( MACRO_IS_TRACKING_SCHEDULE(pstSchedule) )
	{
		pstSchedule->eRsvReady = DxRSVREADY_TVA_TRACKING;
	}
	else
	{
		// tracking schedule이다가 padding이 바뀌어서 tracking이 아니게 된 경우 ready 값이 이미 tracking 값으로 들어가 있으므로 변경해줘야 한다.
		if ( pstSchedule->eRsvReady == DxRSVREADY_TVA_TRACKING )
		{
			pstSchedule->eRsvReady = DxRSVREADY_30_SEC; // opl scheduler에서 택한 값과 동일하게 가져갔다. 변경이 있을 경우 같이 변경
		}
	}

	// TODO: time duration 조절로 인한 update인 경우 timer reinsert로 인해 ready를 두번 하는 등의 문제가 일어난다.
	// ukdtt는 tracking <-> padding 의 변경 때문에 timer를 reinsert 해야 한다. -> TRUE 값
	hErr = SAMA_SCHEDULE_UpdateSchedule (ulSlot, TRUE, pstSchedule, nRequestId, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_SCHEDULE_UpdateSchedule err:\n");
		goto END_FUNC;
	}

	hErr=SAMA_TIMER_ExecuteTimer();
	if(hErr != ERR_OK)
	{
		HxLOG_Critical(" SAMA_TIMER_ExecuteTimer Error!!\n");	
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}



// sapi.cancelSchedule (int nSlot)
STATIC HERROR sama_prtc_tva_cmdCancelSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 			 ulSlot;
	HERROR				 hErr, hResult = ERR_FAIL;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	SERIES_SET			*pstSeriesSet;

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);

	stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;

	// 모든 episode가 녹화된 상태에서는 하나의 schedule이 series tracking이라는 removed info를 가지고 DB에 남아 있다.
	// 이 schedule을 제거하려고 하는 경우에는 13주 series update를 더 이상 하지 않도록 한다.
	pstSeriesSet = SAMALIB_TVA_GetTrackingSeriesSetBySlotId(ulSlot);
	if (pstSeriesSet != NULL)
	{
		HxLOG_Debug("Cancel tracking series. slot=(%d) \n", ulSlot);
		SAMALIB_TVA_DeleteSeriesSet(pstSeriesSet);
	}

	pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(ulSlot);
	if ( pstSeriesSet != NULL )
	{
		// case. TVA
		hErr = sama_prtc_tva_CancelTvaSchedule(pstSeriesSet, ulSlot, nRequestId);
	}
	else
	{
		// case. Not TVA ( ex. ota, power, tbr, reminder, delayed, ... )
		hErr = SAMA_SCHEDULE_RemoveSchedule (ulSlot, &stRemovedInfo, FALSE, nRequestId, TRUE);
	}



	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_SCHEDULE_RemoveSchedule err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}


// sapi.cancelWholeSeries (int nSlot)
STATIC HERROR sama_prtc_tva_cmdCancelWholeSeries (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 			 ulSlot;
	HERROR				 hErr, hResult = ERR_FAIL;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	SERIES_SET			*pstSeriesSet;

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);

	stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;

	// 모든 episode가 녹화된 상태에서는 하나의 schedule이 series tracking이라는 removed info를 가지고 DB에 남아 있다.
	// 이 schedule을 제거하려고 하는 경우에는 13주 series update를 더 이상 하지 않도록 한다.
	pstSeriesSet = SAMALIB_TVA_GetTrackingSeriesSetBySlotId(ulSlot);
	if (pstSeriesSet != NULL)
	{
		HxLOG_Debug("Cancel tracking series. slot=(%d) \n", ulSlot);
		SAMALIB_TVA_DeleteSeriesSet(pstSeriesSet);
		SAMA_SCHEDULE_RemoveSchedule(ulSlot, &stRemovedInfo, FALSE, nRequestId, TRUE);
		hErr = ERR_OK;
		goto END_FUNC;

	}

	pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(ulSlot);
	if ( pstSeriesSet == NULL )
	{
		HxLOG_Error("Can't find series. slot=(%d) \n",ulSlot);
		goto END_FUNC;
	}

	// case. TVA
	hErr = sama_prtc_tva_CancelWholeSeriesSchedule(pstSeriesSet, ulSlot, nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("sama_prtc_tva_CancelWholeSeriesSchedule err:\n");
		goto END_FUNC;
}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}



// sapi.stopSchedule (int nSlot)
STATIC HERROR sama_prtc_tva_cmdStopSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulSlot;
	HERROR				 hErr, hResult = ERR_FAIL;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	SERIES_SET			*pstSeriesSet;

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);

	// 모든 episode가 녹화된 상태에서는 하나의 schedule이 series tracking이라는 removed info를 가지고 DB에 남아 있다.
	// 이 schedule을 제거하려고 하는 경우에는 13주 series update를 더 이상 하지 않도록 한다.
	pstSeriesSet = SAMALIB_TVA_GetTrackingSeriesSetBySlotId(ulSlot);
	if (pstSeriesSet != NULL)
	{
		HxLOG_Debug("Cancel tracking series. slot=(%d) \n", ulSlot);
		SAMALIB_TVA_DeleteSeriesSet(pstSeriesSet);
	}

	pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(ulSlot);
	if ( pstSeriesSet != NULL )
	{
		// case. TVA
		hErr = sama_prtc_tva_CancelTvaSchedule(pstSeriesSet, ulSlot, nRequestId);
	}
	else
	{
		// case. Not TVA ( ex. ota, power, tbr, reminder, delayed, ... )
		stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Stopped;
		hErr = SAMA_SCHEDULE_StopSchedule (ulSlot, &stRemovedInfo, nRequestId);
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_SCHEDULE_StopSchedule err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

// sapi.pauseSchedule (int nSlot)
STATIC HERROR sama_prtc_tva_cmdPauseSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulSlot;
	HERROR				 hErr, hResult = ERR_FAIL;

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);

	hErr = SAMA_SCHEDULE_PauseSchedule (ulSlot, nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_SCHEDULE_PauseSchedule err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

// sapi.resumeSchedule (int nSlot)
STATIC HERROR sama_prtc_tva_cmdResumeSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulSlot;
	HERROR				 hErr, hResult = ERR_FAIL;

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);

	hErr = SAMA_SCHEDULE_ResumeSchedule (ulSlot, nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Debug ("SAMA_SCHEDULE_ResumeSchedule err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

// sapi.incompleteSchedule (int nSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo)
STATIC HERROR sama_prtc_tva_cmdIncompleteSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 			 ulSlot;
	HERROR				 hErr, hResult = ERR_FAIL;
	DxSchedule_RemovedInfo_t *pstRemovedInfo;


	HxLOG_Trace();

	if (nArgc < 2)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	pstRemovedInfo = (DxSchedule_RemovedInfo_t *)HxOBJECT_BIN_DATA(apArgv[1]);

#if 1  // 아직 test 는 안해봄.
	hErr = SAMA_SCHEDULE_StopSchedule (ulSlot, pstRemovedInfo, nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_SCHEDULE_StopSchedule err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;
#else

	pstSchedule = SAMA_SCHEDULE_GetSchedule (ulSlot);
	if (NULL == pstSchedule)
	{
		HxLOG_Error ("SAMA_SCHEDULE_GetSchedule err:\n");
		goto END_FUNC;
	}

	hErr = SAMA_SCHEDULE_AdjustTime (pstSchedule, &bUpdated);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_SCHEDULE_AdjustTime err:\n");
		goto END_FUNC;
	}

	if(bUpdated)
	{
		hErr = SAMA_SCHEDULE_UpdateSchedule (ulSlot, FALSE, pstSchedule, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SAMA_SCHEDULE_UpdateSchedule err:\n");
			goto END_FUNC;
		}
	}
	else
	{

		hErr = SAMA_SCHEDULE_RemoveSchedule (ulSlot);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SAMA_SCHEDULE_RemoveSchedule err:\n");
			goto END_FUNC;
		}

	}

	hResult = ERR_OK;
#endif

END_FUNC:
	return hResult;
}



// sapi.informConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray)
STATIC HERROR sama_prtc_tva_cmdInformConflictAddTried (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32				 nNumConflict;
	SAPI_ConflictType_e	 eType;
	DxSchedule_t		*pstSchedule;
	SAPI_ConflictItem_t	*pstConfArray;
	HERROR				 hResult = ERR_FAIL;

	if (nArgc < 4)
	{
		HxLOG_Error("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	pstSchedule = (DxSchedule_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	eType = (SAPI_ConflictType_e)HxOBJECT_INT(apArgv[1]);
	if (eType == eSAPI_CONFLICT_TYPE_Tp)
	{
		/* Tp conflict is used for satelite box. Terrestrial&Cable box doesn't need to handle Tp conflict.
		    If conflict occured and do need to cancel multiple schedules at once, then use TP conflict */
		HxLOG_Error("Tp Conflicted. Change it to over-capacity-conflict \n");
		eType = eSAPI_CONFLICT_TYPE_OverCapacity;
	}
	nNumConflict = (HINT32)HxOBJECT_INT(apArgv[2]);
	if (nNumConflict > 0)
	{
		pstConfArray = (SAPI_ConflictItem_t *)HxOBJECT_BIN_DATA(apArgv[3]);
	}
	else
	{
		nNumConflict = 0;
		pstConfArray = NULL;
	}

	(void)SAMA_PROTOCOL_NotifyConflictAddTried(pstSchedule, eType, nNumConflict, pstConfArray, nRequestId);

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

// sapi.updateSeries(void)
STATIC HERROR sama_prtc_tva_cmdUpdateSeries (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR				 hErr, hResult = ERR_FAIL;

	hErr = SAMA_SCHEDULE_UpdateSeries(nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Debug ("SAMA_SCHEDULE_UpdateSeries err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}


// sapi.changePaddingTime (HUINT32 ulOffsetTime, HBOOL bIsPreOffset)
STATIC HERROR sama_prtc_ukdtt_cmdChangePaddingTime (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 		ulOffsetTime = 0;
	HBOOL 		bIsPreOffset = FALSE;
	HERROR				 hErr = ERR_FAIL;

	if (nArgc < 2)
	{
		HxLOG_Error("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulOffsetTime = (HUINT32)HxOBJECT_INT(apArgv[0]);
	bIsPreOffset = (HBOOL)HxOBJECT_INT(apArgv[1]);

	HxLOG_Debug("ulOffsetTime: %d, bIsPreOffset: %d \n", ulOffsetTime, bIsPreOffset);

	hErr = SAMA_SCHEDULE_ChangePaddingTime(ulOffsetTime, bIsPreOffset, nRequestId);

END_FUNC:
	return hErr;
}

STATIC HERROR sama_prtc_tva_registerRpcMethods (HINT32 nRpcHandle)
{
	// sapi.init (void)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.init", "i", sama_prtc_tva_cmdInit, NULL,
						"initialize the client information");

	// sapi.term (void)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.term", "i", sama_prtc_tva_cmdTerm, NULL,
						"terminate the client information");

	// sapi.setRscInfo (SAPI_RscInfo_t *pstRscInfo)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.setRscInfo", "b", sama_prtc_tva_cmdSetRscInfo, NULL,
						"Set the resource Information");

	// sapi.setRfInputAbility (int nRfInputId, int nAbility)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.setRfInputAbility", "ii", sama_prtc_tva_cmdSetRfInputAbility, NULL,
						"Set the RF-Input Ability");

	// sapi.refreshSchedule (void)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.refreshSchedule", NULL, sama_prtc_tva_cmdRefreshSchedule, NULL,
						"Re-send the schedules to the SAPI: (void)");

	// sapi.makeSchedule (int nSlot, DxSchedule_t *pstSchedule)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.makeSchedule", "ib", sama_prtc_tva_cmdMakeSchedule, NULL,
						"Make a new schedule: (int nSlot, DxSchedule_t *pstSchedule)");

	// sapi.updateSchedule (int nSlot, DxSchedule_t *pstSchedule)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.updateSchedule", "ib", sama_prtc_tva_cmdUpdateSchedule, NULL,
						"Make a new schedule: (int nSlot, DxSchedule_t *pstSchedule)");

	// sapi.cancelSchedule (int nSlot)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.cancelSchedule", "i", sama_prtc_tva_cmdCancelSchedule, NULL,
						"Cancel the schedule: (int nSlotId)");

	// sapi.cancelWholeSeries (int nSlotId)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.cancelWholeSeries", "i", sama_prtc_tva_cmdCancelWholeSeries, NULL,
						"Cancel Whole series: (int nSlotId)");

	// sapi.stopSchedule (int nSlot)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.stopSchedule", "i", sama_prtc_tva_cmdStopSchedule, NULL,
						"Cancel the schedule: (int nSlotId)");

	// sapi.pauseSchedule (int nSlot)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.pauseSchedule", "i", sama_prtc_tva_cmdPauseSchedule, NULL,
						"Pause the schedule: (int nSlotId)");

	// sapi.resumeSchedule (int nSlot)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.resumeSchedule", "i", sama_prtc_tva_cmdResumeSchedule, NULL,
						"Resume the schedule: (int nSlotId)");

	// sapi.incompleteSchedule (int nSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.incompleteSchedule", "ib", sama_prtc_tva_cmdIncompleteSchedule, NULL,
						"Adjust incomplte schedule: (int nSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo)");

	// sapi.informConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.informConflictAddTried", "biib", sama_prtc_tva_cmdInformConflictAddTried, NULL,
						"Cancel the schedule: (int nSlotId)");

	// sapi.updateSeries (void)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.updateSeries", NULL, sama_prtc_tva_cmdUpdateSeries, NULL,
						"Update all series: (void)");

	// sapi.changePaddingTime (HUINT32 ulOffsetTime, HBOOL bIsPreOffset)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.changePaddingTime", "ii", sama_prtc_ukdtt_cmdChangePaddingTime, NULL,
						"Change Padding Time: (HUINT32 ulOffsetTime, HBOOL bIsPreOffset)");

	return ERR_OK;
}

STATIC HERROR sama_prtc_tva_registerRpcCallbacks (HINT32 nRpcHandle)
{
	// sama.onResourceInfoSet (SAPI_RscInfo_t *pstRscInfo)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onResourceInfoSet", "b",
					"Notifying the Resource Info set:\n"
					"\t - (SAPI_RscInfo_t *pstRscInfo)"
					);

	// sama.onRfInputAbilitySet (int nRfInputId, int bAbility)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onRfInputAbilitySet", "ii",
					"Notifying the RF-Input Ability Set:\n"
					"\t - (int nRfInputId, int bAbility)"
					);

	// sama.onAvailableSlotsChanged (int nPid, int nSlotNum, int *anSlotArray)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onAvailableSlotsChanged", "iib",
					"Notifying the available slots changed:\n"
					"\t - (int nPid, int nSlotNum, int *anSlotArray)"
					);

	// sama.onSchedulesLoaded (int nSchNum, DxSchedule_t *pstSchArray)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onSchedulesLoaded", "ib",
					"Notifying the schedule item added:\n"
					"\t - (int nSlotId, DxSchedule_t *pstSchedule)"
					);

	// sama.onScheduleAdded (int nSlotId, DxSchedule_t *pstSchedule)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onScheduleAdded", "ib",
					"Notifying the schedule item added:\n"
					"\t - (int nSlotId, DxSchedule_t *pstSchedule)"
					);

	// sama.onScheduleChanged (int nSlotId, DxSchedule_t *pstSchedule)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onScheduleChanged", "iib",
					"Notifying the schedule item changed:\n"
					"\t - (int nSlotId, HBOOL bExcuteNotifier, DxSchedule_t *pstSchedule)"
					);

	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onScheduleChangedEx", "ib",
					"Notifying the schedule item changed:\n"
					"\t - (int nSlotId, HBOOL bExcuteNotifier, DxSchedule_t *pstSchedule)"
					);

	// sama.onScheduleRemoved (int nSlotId)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onScheduleRemoved", "ib",
					"Notifying the schedule item removed:\n"
					"\t - (int nSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo)"
					);

	// sama.onSchedulePaused (int nSlotId)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onSchedulePaused", "i",
					"Notifying the schedule item paused:\n"
					"\t - (int nSlotId)"
					);

	// sama.onScheduleResumed (int nSlotId)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onScheduleResumed", "i",
					"Notifying the schedule item resumed:\n"
					"\t - (int nSlotId)"
					);

	// sama.onConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onConflictAddTried", "biib",
					"Notifying the schedule conflicted during the request:\n"
					"\t - (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray)"
					);

	// sama.onTimeUp (int nSlotId, SAPI_TimerItemType_e eTimerType)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onTimeUp", "iib",
					"Notifying the schedule item removed:\n"
					"\t - (int nSlotId, int eTimerType, DxSchedule_t *pstSchedule)"
					);

	// sama.onAlternativeAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray, SAPI_AlternativeItem_t *pstAltItems)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onAlternativeAddTried", "biibb",
					"Notify the requested schedule has alternatives:\n"
					"\t - (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray, SAPI_AlternativeItem_t *pstAltItems)"
					);

	// sama.onSystemTimeChanged (SAPI_SystemTimeChangedType_e eSystemTimeChangedType)
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onSystemTimeChanged", "i",
					"Notify System time is changed:\n"
					"\t - (SAPI_SystemTimeChangedType_e eSystemTimeChangedType)"
					);

	// sama.onResourceRefreshed ( )
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onResourceRefreshed", NULL,
					"Notifying sama cache resource update:\n"
					"\t - ()"
					);

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR SAMA_PROTOCOL_TVA_RegisterRpcProtocols (HINT32 nRpcHandle)
{
	sama_prtc_tva_registerRpcMethods (nRpcHandle);
	sama_prtc_tva_registerRpcCallbacks (nRpcHandle);
	return ERR_OK;
}

// sama.onAlternativeAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray, SAPI_AlternativeItem_t *pstAltItems)
HERROR SAMA_PROTOCOL_TVA_NotifyAlternativeAddTried (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, HINT32 nNumConflict, SAPI_ConflictItem_t *pstConfArray, SAPI_AlternativeItems_t *pstAltItems, HINT32 nRequestId)
{
	HUINT32				ulNumConflict, ulConfArraySize;
	HUINT32				ulNumAlternatives;
	SAPI_ConflictItem_t	stDummy;
	HINT32				i;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	if ( pstConfArray == NULL
		|| pstAltItems == NULL )
	{
		HxLOG_Error("Invalid parma. conf=(0x%x), alt=(0x%x)",pstConfArray, pstAltItems);
		return ERR_FAIL;
	}

	if (nNumConflict > 0)
	{
		ulNumConflict = (HUINT32)nNumConflict;
		ulConfArraySize = sizeof(SAPI_ConflictItem_t) * ulNumConflict;
	}
	else
	{
		ulNumConflict = 0;
		ulConfArraySize = sizeof(SAPI_ConflictItem_t);
		pstConfArray = &stDummy;
	}

	ulNumAlternatives = pstAltItems->ulNumAlternatives;
	HxLOG_Print("#ofConf=(%d), #ofAlt=(%d), pstAltItems=(0x%x)\n",ulNumConflict, ulNumAlternatives, pstAltItems);
	for ( i=0; i<ulNumConflict; i++ )
	{
		HxLOG_Print("(%d) conflict slot:(%d)\n",i,pstConfArray[i].ulSlot);
	}
	for ( i=0; i<ulNumAlternatives; i++)
	{
		HxLOG_Print("(%d) alternative slot:(%d), evtid(%d), svcuid(%d)\n",i, pstAltItems->astAltItemArray[i].ulSlot
																		 , pstAltItems->astAltItemArray[i].ulAltEvtId
																		 , pstAltItems->astAltItemArray[i].ulAltSvcUid);
	}

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onAlternativeAddTried", nRequestId, "biibb", pstSchedule, sizeof(DxSchedule_t),
							eType, ulNumConflict, pstConfArray, ulConfArraySize, pstAltItems, sizeof(SAPI_AlternativeItems_t));
	}

	return hErr;
}



// sama.onSystemTimeChanged (SAPI_SystemTimeChangedType_e eSystemTimeChangedType)
HERROR SAMA_PROTOCOL_TVA_NotifySystemTimeChanged (SAMA_Context_t *pstContext, SAPI_SystemTimeChangedType_e eSystemTimeChangedType)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if (hErr == ERR_OK)
	{
		hErr = HLIB_RPC_Notify (nRpcHandle, "sama.onSystemTimeChanged", "i", eSystemTimeChangedType);
	}

	return hErr;
}

