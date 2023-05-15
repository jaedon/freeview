/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama.c
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
STATIC HINT32  sama_prtc_base_CompareSlot (const void *arg1, const void *arg2)
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

STATIC HERROR sama_prtc_base_CancelWholeSeries (DxSchedule_t *pstSchedule, HINT32 nRequestId)
{
	HUINT32			ulRemoveSlot;
	HUINT32			i = 0;
	HUINT32			ulTotalRemoveSlots = 0;
	HERROR 			hErr = ERR_OK;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	HxVector_t		*pstRemoveSlots = NULL;

	HUINT32 		ulNumSch;
	DxSchedule_t	*pstSchArray = NULL;
	DxSchedule_t	*pstSchItem = NULL;

	hErr = SAMA_SCHEDULE_GetAllSchedules(&ulNumSch, &pstSchArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SAMA_SCHEDULE_GetAllSchedules failed. \n");
		goto END_FUNC;
	}

	pstRemoveSlots = HLIB_VECTOR_NewEasy(32, NULL, (HxCompareFunc_t)sama_prtc_base_CompareSlot);
	if (pstRemoveSlots == NULL)
	{
		HxLOG_Error("memalloc failed. \n");
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

		// ignore different seires or different service
		if ( (pstSchItem->uExtInfo.stRec.ulSeriesId != pstSchedule->uExtInfo.stRec.ulSeriesId)
			||(pstSchItem->ulSvcUid != pstSchedule->ulSvcUid))
		{
			continue;
		}

		HLIB_VECTOR_Add(pstRemoveSlots, (void *)pstSchItem->ulSlot);
	}

	ulTotalRemoveSlots = HLIB_VECTOR_Length(pstRemoveSlots);
	HxLOG_Debug("Remove %d slots \n",ulTotalRemoveSlots);
	for (i=0; i<ulTotalRemoveSlots; i++)
	{
		ulRemoveSlot = (HUINT32)HLIB_VECTOR_ItemAt(pstRemoveSlots,i);
		stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;
		hErr |= SAMA_SCHEDULE_RemoveSchedule(ulRemoveSlot, &stRemovedInfo, FALSE, nRequestId, TRUE);
	}

END_FUNC:
	if (pstRemoveSlots != NULL)
	{
		HLIB_VECTOR_Delete(pstRemoveSlots);
	}

	if (pstSchArray != NULL)
	{
		SAMA_Free(pstSchArray);
	}

	return hErr;
}

#define _____RPC_METHOD_REGISTRATION_____
// sapi.init (int nPid)
STATIC HERROR sama_prtc_base_cmdInit (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
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
STATIC HERROR sama_prtc_base_cmdTerm (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	return ERR_OK;
}

// sapi.setRscInfo (SAPI_RscInfo_t *pstRscInfo)
STATIC HERROR sama_prtc_base_cmdSetRscInfo (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
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
STATIC HERROR sama_prtc_base_cmdSetRfInputAbility (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
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
STATIC HERROR sama_prtc_base_cmdRefreshSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	(void)SAMA_SCHEDULE_SendAllScheduleToSapi(nRequestId);
	return ERR_OK;
}

// sapi.makeSchedule (int nSlot, DxSchedule_t *pstSchedule)
STATIC HERROR sama_prtc_base_cmdMakeSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
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

	hErr = SAMA_SCHEDULE_AddSchedule (ulSlot, pstSchedule, FALSE, nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_SCHEDULE_MakeScheduler err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

// sapi.updateSchedule (int nSlot, DxSchedule_t *pstSchedule)
STATIC HERROR sama_prtc_base_cmdUpdateSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
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

	hErr = SAMA_SCHEDULE_UpdateSchedule (ulSlot, FALSE, pstSchedule, nRequestId, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_SCHEDULE_UpdateSchedule err:\n");
		goto END_FUNC;
	}

	hErr = SAMA_TIMER_ExecuteTimer();
	if(hErr != ERR_OK)
	{
		HxLOG_Critical(" SAMA_TIMER_ExecuteTimer Error!!\n");
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}



// sapi.cancelSchedule (int nSlot)
STATIC HERROR sama_prtc_base_cmdCancelSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 			 ulSlot;
	HERROR				 hErr, hResult = ERR_FAIL;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	DxSchedule_t		*pstSchedule = NULL;

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);

	pstSchedule = SAMA_SCHEDULE_GetSchedule(ulSlot);
	if ((pstSchedule != NULL)
		&& (pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR))
	{
		hErr = sama_prtc_base_CancelWholeSeries(pstSchedule, nRequestId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("sama_prtc_base_CancelWholeSeries err:\n");
			goto END_FUNC;
		}
	}
	else
	{
		stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;
		hErr = SAMA_SCHEDULE_RemoveSchedule (ulSlot, &stRemovedInfo, FALSE, nRequestId, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SAMA_SCHEDULE_RemoveSchedule err:\n");
			goto END_FUNC;
		}
	}


	hResult = ERR_OK;

END_FUNC:
	return hResult;


}

// sapi.ChangeNextSeriesSchedule (int nSlot)
STATIC HERROR sama_prtc_base_cmdChangeNextSeriesSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 			 ulSlot;
	HERROR				 hErr, hResult = ERR_FAIL;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);

	stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;
	hErr = SAMA_SCHEDULE_ChangeNextSeriesSchedule (ulSlot, &stRemovedInfo, nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAMA_SCHEDULE_RemoveSchedule err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;


}


// sapi.stopSchedule (int nSlot)
STATIC HERROR sama_prtc_base_cmdStopSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulSlot;
	HERROR				 hErr, hResult = ERR_FAIL;
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

	if (nArgc < 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", nArgc);
		goto END_FUNC;
	}

	ulSlot	= (HUINT32)HxOBJECT_INT (apArgv[0]);

	stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Stopped;
	hErr = SAMA_SCHEDULE_StopSchedule (ulSlot, &stRemovedInfo, nRequestId);
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
STATIC HERROR sama_prtc_base_cmdPauseSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
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
STATIC HERROR sama_prtc_base_cmdResumeSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
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
		HxLOG_Error ("SAMA_SCHEDULE_ResumeSchedule err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

// sapi.incompleteSchedule (int nSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo)
STATIC HERROR sama_prtc_base_cmdIncompleteSchedule (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
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
STATIC HERROR sama_prtc_base_cmdInformConflictAddTried (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
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

#if 0
// sapi.changePaddingTime (HUINT32 ulOffsetTime, HBOOL bIsPreOffset)
STATIC HERROR sama_prtc_base_cmdChangePaddingTime (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
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
#endif
STATIC HERROR sama_prtc_base_registerRpcMethods (HINT32 nRpcHandle)
{
	// sapi.init (void)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.init", "i", sama_prtc_base_cmdInit, NULL,
						"initialize the client information");

	// sapi.term (void)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.term", "i", sama_prtc_base_cmdTerm, NULL,
						"terminate the client information");

	// sapi.setRscInfo (SAPI_RscInfo_t *pstRscInfo)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.setRscInfo", "b", sama_prtc_base_cmdSetRscInfo, NULL,
						"Set the resource Information");

	// sapi.setRfInputAbility (int nRfInputId, int nAbility)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.setRfInputAbility", "ii", sama_prtc_base_cmdSetRfInputAbility, NULL,
						"Set the RF-Input Ability");

	// sapi.refreshSchedule (void)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.refreshSchedule", NULL, sama_prtc_base_cmdRefreshSchedule, NULL,
						"Re-send the schedules to the SAPI: (void)");

	// sapi.makeSchedule (int nSlot, DxSchedule_t *pstSchedule)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.makeSchedule", "ib", sama_prtc_base_cmdMakeSchedule, NULL,
						"Make a new schedule: (int nSlot, DxSchedule_t *pstSchedule)");

	// sapi.updateSchedule (int nSlot, DxSchedule_t *pstSchedule)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.updateSchedule", "ib", sama_prtc_base_cmdUpdateSchedule, NULL,
						"Make a new schedule: (int nSlot, DxSchedule_t *pstSchedule)");

	// sapi.cancelSchedule (int nSlot)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.cancelSchedule", "i", sama_prtc_base_cmdCancelSchedule, NULL,
						"Cancel the schedule: (int nSlotId)");

	// sapi.ChangeNextSeriesSchedule (int nSlot)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.ChangeNextSeriesSchedule", "i", sama_prtc_base_cmdChangeNextSeriesSchedule, NULL,
						"Change the Series schedule To Next: (int nSlotId)");

	// sapi.stopSchedule (int nSlot)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.stopSchedule", "i", sama_prtc_base_cmdStopSchedule, NULL,
						"Cancel the schedule: (int nSlotId)");

	// sapi.pauseSchedule (int nSlot)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.pauseSchedule", "i", sama_prtc_base_cmdPauseSchedule, NULL,
						"Pause the schedule: (int nSlotId)");

	// sapi.resumeSchedule (int nSlot)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.resumeSchedule", "i", sama_prtc_base_cmdResumeSchedule, NULL,
						"Resume the schedule: (int nSlotId)");

	// sapi.incompleteSchedule (int nSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.incompleteSchedule", "ib", sama_prtc_base_cmdIncompleteSchedule, NULL,
						"Adjust incomplte schedule: (int nSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo)");

	// sapi.informConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.informConflictAddTried", "biib", sama_prtc_base_cmdInformConflictAddTried, NULL,
						"Cancel the schedule: (int nSlotId)");
#if 0
	// sapi.changePaddingTime (HUINT32 ulOffsetTime, HBOOL bIsPreOffset)
	(void)HLIB_RPC_RegisterCall (nRpcHandle, "sapi.changePaddingTime", "ii", sama_prtc_base_cmdChangePaddingTime, NULL,
						"Change Padding Time: (HUINT32 ulOffsetTime, HBOOL bIsPreOffset)");
#endif
	return ERR_OK;
}

STATIC HERROR sama_prtc_base_registerRpcCallbacks (HINT32 nRpcHandle)
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

	// sama.onResourceRefreshed ( )
	(void)HLIB_RPC_CreateNotification (nRpcHandle, "sama.onResourceRefreshed", NULL,
					"Notifying sama cache resource update:\n"
					"\t - ()"
					);

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR SAMA_PROTOCOL_BASE_RegisterRpcProtocols (HINT32 nRpcHandle)
{
	sama_prtc_base_registerRpcMethods (nRpcHandle);
	sama_prtc_base_registerRpcCallbacks (nRpcHandle);
	return ERR_OK;
}

// sama.onResourceInfoSet (SAPI_RscInfo_t *pstRscInfo)
HERROR SAMA_PROTOCOL_BASE_NotifyRscInfoSet (SAMA_Context_t *pstContext, SAPI_RscInfo_t *pstRscInfo, HINT32 nRequestId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	if (NULL == pstRscInfo) 				{ return ERR_FAIL; }

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onResourceInfoSet", nRequestId, "b", pstRscInfo, sizeof(SAPI_RscInfo_t));
	}

	return hErr;
}

// sama.onRfInputAbilitySet (int nRfInputId, int bAbility)
HERROR SAMA_PROTOCOL_BASE_NotifyRfInputAbilitySet (SAMA_Context_t *pstContext, HUINT32 ulRfInputId, HBOOL bAbility, HINT32 nRequestId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId(nRpcHandle, "sama.onRfInputAbilitySet", nRequestId, "ii", ulRfInputId, bAbility);
	}

	return hErr;
}

// sama.onAvailableSlotsChanged (int nPid, int nSlotNum, int *anSlotArray)
HERROR SAMA_PROTOCOL_BASE_NotifyAvailableSlotsChanged (SAMA_Context_t *pstContext, HINT32 nPid, HUINT32 ulSlotNum, HUINT32 *pulSlotArray, HINT32 nRequestId)
{
	HUINT32			 ulNum, ulDataSize;
	HUINT32			 ulSlotDummy;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	if ((0 == ulSlotNum) || (NULL == pulSlotArray))
	{
		ulNum = 0;
		ulDataSize = sizeof(HUINT32);
		pulSlotArray = &ulSlotDummy;
	}
	else
	{
		ulNum = ulSlotNum;
		ulDataSize = sizeof(HUINT32) * ulSlotNum;
	}

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onAvailableSlotsChanged", nRequestId, "iib", nPid, ulNum, pulSlotArray, ulDataSize);
	}

	return hErr;
}


// sama.onSchedulesLoaded (int nSchNum, DxSchedule_t *pstSchArray)
HERROR SAMA_PROTOCOL_BASE_NotifySchedulesLoaded (SAMA_Context_t *pstContext, HUINT32 ulSchNum, DxSchedule_t *pstSchArray, HINT32 nRequestId)
{
	HUINT32			 ulNum, ulDataSize;
	DxSchedule_t	 stSchedule;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	if ((0 == ulSchNum) || (NULL == pstSchArray))
	{
		ulNum = 0;
		ulDataSize = sizeof(DxSchedule_t);
		pstSchArray = &stSchedule;
	}
	else
	{
		ulNum = ulSchNum;
		ulDataSize = sizeof(DxSchedule_t) * ulSchNum;
	}

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onSchedulesLoaded", nRequestId, "ib", ulNum, pstSchArray, ulDataSize);
	}

	return hErr;
}

// sama.onScheduleAdded (int nSlotId, DxSchedule_t *pstSchedule)
HERROR SAMA_PROTOCOL_BASE_NotifyScheduleAdded (SAMA_Context_t *pstContext, HUINT32 ulSlot, DxSchedule_t *pstSchedule, HINT32 nRequestId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onScheduleAdded", nRequestId, "ib", ulSlot, pstSchedule, sizeof(DxSchedule_t));
	}

	return hErr;
}

// sama.onScheduleChanged (int nSlotId, HBOOL bExcuteNotifier, DxSchedule_t *pstSchedule)
HERROR SAMA_PROTOCOL_BASE_NotifyScheduleModified (SAMA_Context_t *pstContext, HUINT32 ulSlot, DxSchedule_t *pstSchedule, HBOOL bExcuteNotifier, HINT32 nRequestId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onScheduleChanged", nRequestId, "iib", ulSlot, bExcuteNotifier, pstSchedule, sizeof(DxSchedule_t));
	}

	return hErr;
}

// sama.onScheduleChangedEx (int nSlotId, HBOOL bExcuteNotifier, DxSchedule_t *pstSchedule)
HERROR SAMA_PROTOCOL_BASE_NotifyScheduleModifiedEx (SAMA_Context_t *pstContext, HUINT32 ulSlot, SAPI_ScheduleChanged_t *pstChangedSchedule, HINT32 nRequestId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onScheduleChangedEx", nRequestId, "ib", ulSlot, pstChangedSchedule, sizeof(SAPI_ScheduleChanged_t));
	}

	return hErr;
}


// sama.onScheduleRemoved (int nSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo)
HERROR SAMA_PROTOCOL_BASE_NotifyScheduleRemoved (SAMA_Context_t *pstContext, HUINT32 ulSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo, HINT32 nRequestId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onScheduleRemoved", nRequestId, "ib", ulSlot, pstRemovedInfo, sizeof(DxSchedule_RemovedInfo_t));
	}

	return hErr;
}

// sama.onSchedulePaused (int nSlotId)
HERROR SAMA_PROTOCOL_BASE_NotifySchedulePaused (SAMA_Context_t *pstContext, HUINT32 ulSlot, HINT32 nRequestId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onSchedulePaused", nRequestId, "i", ulSlot);
	}

	return hErr;
}

// sama.onScheduleResumed (int nSlotId)
HERROR SAMA_PROTOCOL_BASE_NotifyScheduleResumed (SAMA_Context_t *pstContext, HUINT32 ulSlot, HINT32 nRequestId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onScheduleResumed", nRequestId, "i", ulSlot);
	}

	return hErr;
}

// sama.onConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray)
HERROR SAMA_PROTOCOL_BASE_NotifyConflictAddTried (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, HINT32 nNumConflict, SAPI_ConflictItem_t *pstConfArray, HINT32 nRequestId)
{
	HUINT32				 ulNumConflict, ulArraySize;
	SAPI_ConflictItem_t	 stDummy;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	if ((nNumConflict > 0) && (NULL != pstConfArray))
	{
		ulNumConflict = (HUINT32)nNumConflict;
		ulArraySize = sizeof(SAPI_ConflictItem_t) * ulNumConflict;
	}
	else
	{
		ulNumConflict = 0;
		ulArraySize = sizeof(SAPI_ConflictItem_t);
		pstConfArray = &stDummy;
	}

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId (nRpcHandle, "sama.onConflictAddTried", nRequestId, "biib", pstSchedule, sizeof(DxSchedule_t),
							eType, ulNumConflict, pstConfArray, ulArraySize);
	}

	return hErr;
}

// sama.onTimeUp (int nSlotId, SAPI_TimerItemType_e eTimerType, DxSchedule_t *pstSchedule)
HERROR SAMA_PROTOCOL_BASE_NotifyTimeUp (SAMA_Context_t *pstContext, HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, DxSchedule_t *pstSchedule)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_Notify (nRpcHandle, "sama.onTimeUp", "iib", ulSlot, eTimerType, (void *)pstSchedule, sizeof(DxSchedule_t));
	}

	return hErr;
}

// sama.onResourceRefreshed ( )
HERROR SAMA_PROTOCOL_BASE_NotifyResourceRefreshed (SAMA_Context_t *pstContext, HINT32 nRequestId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = SAMA_GetRpcHandle (&nRpcHandle);

	if(hErr == ERR_OK)
	{
		hErr = HLIB_RPC_NotifyWithRequestId(nRpcHandle, "sama.onResourceRefreshed", nRequestId, NULL);
	}

	return hErr;
}


