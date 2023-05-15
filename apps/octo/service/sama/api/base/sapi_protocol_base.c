/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sapi_protocol.c
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
#include <sapi.h>

#include <sapi_int.h>
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

#define ___PRIVATE_FUNCTIONS_DECLARATION___
/*******************************************************************/
/********************      Private Functions Declaration    ********************/
/*******************************************************************/

#define ___PRIVATE_FUNCTIONS___
/*******************************************************************/
/********************      Private Functions    ********************/
/*******************************************************************/
#define _____RPC_METHODS_____



#define ___MEMBER_FUNCTIONS___
/*******************************************************************/
/********************      Member Functions       ******************/
/*******************************************************************/
#define _____REGISTRATION_FUNCTIONS_____
HERROR sapi_base_RegisterRpcCallback (sapi_Inst_t *pstInst)
{
	if (NULL == pstInst)					{ return ERR_FAIL; }

	// sama.onResourceInfoSet (SAPI_RscInfo_t *pstRscInfo)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onResourceInfoSet", "b", (void*)pstInst, sapi_base_OnResourceInfoSet);

	// sama.onRfInputAbilitySet (int nRfInputId, int bAbility)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onRfInputAbilitySet", "ii", (void*)pstInst, sapi_base_OnRfInputAbilitySet);

	// sama.onAvailableSlotsChanged (int nPid, int nSlotNum, int *anSlotArray)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onAvailableSlotsChanged", "iib", (void*)pstInst, sapi_base_OnAvailableSlotsChanged);

	// sama.onSchedulesLoaded (int nSchNum, DxSchedule_t *pstSchArray)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onSchedulesLoaded", "ib", (void*)pstInst, sapi_base_OnSchedulesLoaded);

	// sama.onScheduleAdded (int nSlotId, DxSchedule_t *pstSchedule)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onScheduleAdded", "ib", (void*)pstInst, sapi_base_OnScheduleAdded);

	// sama.onScheduleChanged (int nSlotId, HBOOL bExcuteNotifier, DxSchedule_t *pstSchedule)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onScheduleChanged", "iib", (void*)pstInst, sapi_base_OnScheduleChanged);

	// sama.onScheduleChangedEx (int nSlotId, HBOOL bExcuteNotifier, DxSchedule_t *pstSchedule)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onScheduleChangedEx", "ib", (void*)pstInst, sapi_base_OnScheduleChangedEx);

	// sama.onScheduleRemoved (int nSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onScheduleRemoved", "ib", (void*)pstInst, sapi_base_OnScheduleRemoved);

	// sama.onSchedulePaused (int nSlotId)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onSchedulePaused", "i", (void*)pstInst, sapi_base_OnSchedulePaused);

	// sama.onScheduleResumed (int nSlotId)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onScheduleResumed", "i", (void*)pstInst, sapi_base_OnScheduleResumed);

	// sama.onConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onConflictAddTried", "biib", (void*)pstInst, sapi_base_OnConflictAddTried);

	// sama.onTimeUp (int nSlotId, SAPI_TimerItemType_e eTimerType, DxSchedule_t *pstSchedule)
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onTimeUp", "iib", (void*)pstInst, sapi_base_OnTimeUp);

	// sama.onResourceRefreshed ( )
	HLIB_RPC_RegisterNotifyCallback(pstInst->nRpcHandle, "sama.onResourceRefreshed", NULL, (void*)pstInst, sapi_base_OnResourceRefreshed);

	return ERR_OK;
}

HBOOL sapi_base_IsScheduleVolatile (DxSchedule_t *pstSchedule)
{
	if (NULL != pstSchedule)
	{
		switch (pstSchedule->eRsvType)
		{
		case DxRSVTYPE_RECORD_INSTANT:
		case DxRSVTYPE_RECORD_DELAYED:
		case DxRSVTYPE_NETSTREAMING_LIVE:
		case DxRSVTYPE_NETSTREAMING_FILE:
		case DxRSVTYPE_EPG_GATHERING:
		case DxRSVTYPE_DYNAMIC_UPDATE:
			return TRUE;

		default:
			break;
		}
	}

	return FALSE;
}


#define _____RPC_METHODS_____
// sapi.init (int nPid)
HERROR sapi_base_CmdInit (sapi_Inst_t *pstInst)
{
	HxLOG_Trace();

	(void)HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.init", "i", pstInst->nPid);

	return ERR_OK;
}

// sapi.term (int nPid)
HERROR sapi_base_CmdTerm (sapi_Inst_t *pstInst)
{
	HxLOG_Trace();

	(void)HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.term", "i", pstInst->nPid);

	return ERR_OK;
}

// sapi.setRscInfo (SAPI_RscInfo_t *pstRscInfo)
HERROR sapi_base_CmdSetRscInfo (sapi_Inst_t *pstInst, SAPI_RscInfo_t *pstRscInfo)
{
	HxLOG_Trace();

	(void)HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.setRscInfo", "b", (void *)pstRscInfo, sizeof(SAPI_RscInfo_t));

	return ERR_OK;
}

// sapi.setRfInputAbility (int nRfInputId, int nAbility)
HERROR sapi_base_CmdSetRfInputAbility (sapi_Inst_t *pstInst, HUINT32 ulRfInputId, HBOOL bAbility)
{
	HxLOG_Trace();

	(void)HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.setRfInputAbility", "ii", ulRfInputId, bAbility);

	return ERR_OK;
}

// sapi.refreshSchedule (void)
HERROR sapi_base_CmdRefreshScheduleData (sapi_Inst_t *pstInst)
{
	HxLOG_Trace();

	(void)HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.refreshSchedule", NULL);

	return ERR_OK;
}

// sapi.makeSchedule (int nSlot, DxSchedule_t *pstSchedule)
HERROR sapi_base_CmdMakeSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, DxSchedule_t *pstSchedule, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.makeSchedule", "ib", ulSlotId, (void *)pstSchedule, sizeof(DxSchedule_t));
	return ERR_OK;
}

// sapi.updateSchedule (int nSlot, DxSchedule_t *pstSchedule)
HERROR sapi_base_CmdUpdateSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, DxSchedule_t *pstSchedule, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.updateSchedule", "ib", ulSlotId, (void *)pstSchedule, sizeof(DxSchedule_t));
	return ERR_OK;
}

// sapi.cancelSchedule (int nSlot)
HERROR sapi_base_CmdCancelSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.cancelSchedule", "i", ulSlotId);
	return ERR_OK;
}

// sapi.ChangeNextSeriesSchedule (int nSlot)
HERROR sapi_base_CmdChangeSeriesScheduleToNext (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.ChangeNextSeriesSchedule", "i", ulSlotId);
	return ERR_OK;
}


// sapi.stopSchedule (int nSlot)
HERROR sapi_base_CmdStopSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.stopSchedule", "i", ulSlotId);
	return ERR_OK;
}

// sapi.pauseSchedule (int nSlot)
HERROR sapi_base_CmdPauseSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.pauseSchedule", "i", ulSlotId);
	return ERR_OK;
}

// sapi.resumeSchedule (int nSlot)
HERROR sapi_base_CmdResumeSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.resumeSchedule", "i", ulSlotId);
	return ERR_OK;
}

// sapi.incompleteSchedule (int nSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo)
HERROR sapi_base_CmdIncompleteSchedule (sapi_Inst_t *pstInst, HUINT32 ulSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.incompleteSchedule", "ib", ulSlotId, pstRemovedInfo, sizeof(DxSchedule_RemovedInfo_t));
	return ERR_OK;
}

// sapi.informConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray)
HERROR sapi_base_CmdInformConflictAddTried (sapi_Inst_t *pstInst, SAPI_Conflict_t *pstConflict, HINT32 *pnRequestId)
{
	HUINT32				 ulNumConflict, ulConfArraySize;
	SAPI_ConflictItem_t	 stDummyConflict;
	SAPI_ConflictItem_t	*pstConfArray;

	HxLOG_Trace();

	if ((pstConflict->ulNumConflict > 0) && (NULL != pstConflict->pstConflictArray))
	{
		ulNumConflict = pstConflict->ulNumConflict;
		ulConfArraySize = sizeof(SAPI_ConflictItem_t) * ulNumConflict;
		pstConfArray = pstConflict->pstConflictArray;
	}
	else
	{
		ulNumConflict = 0;
		ulConfArraySize = sizeof(SAPI_ConflictItem_t);
		pstConfArray = &(stDummyConflict);
	}


	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.informConflictAddTried", "biib", &(pstConflict->stTriedSchedule), sizeof(DxSchedule_t),
									pstConflict->eType, ulNumConflict, pstConfArray, ulConfArraySize);
	return ERR_OK;
}

// sapi.changePaddingTime (HUINT32 ulOffsetTime, HBOOL bIsPreOffset)
HERROR sapi_base_CmdChangePaddingTime (sapi_Inst_t *pstInst, HUINT32 ulOffsetTime, HBOOL bIsPreOffset, HINT32 *pnRequestId)
{
	HxLOG_Trace();

	*pnRequestId = HLIB_RPC_Call (pstInst->nRpcHandle, "sapi.changePaddingTime", "ii", ulOffsetTime, bIsPreOffset);
	return ERR_OK;
}

#define _____RPC_CALLBACKS_____
// sama.onResourceInfoSet (SAPI_RscInfo_t *pstRscInfo)
HERROR sapi_base_OnResourceInfoSet (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	SAPI_RscInfo_t			*pstRscInfo;
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;

	HxLOG_Trace();

	if (1 != nArgc)
	{
		HxLOG_Error ("Argument not valid: sama.onResourceInfoSet (SAPI_RscInfo_t *pstRscInfo)\n");
		return ERR_FAIL;
	}

	pstRscInfo = (SAPI_RscInfo_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	if (NULL == pstRscInfo)
	{
		HxLOG_Error ("pstRscInfo NULL:\n");
		return ERR_FAIL;
	}

	pstInst->stSamaConflictCheck.bRscSet = TRUE;
	SAMA_MemCpy (&(pstInst->stSamaConflictCheck.stRscInfo), pstRscInfo, sizeof(SAPI_RscInfo_t));


	// SAPI에서도 set 을 해줘야 각각의 process 들이 문제없이 사용 가능. sapi instance 가 rlib handle을 가지고. 그리고 모든 sapi는 공통된 resource 공유.
	(void)SAMALIB_CFLT_SetResourceInfo (&(pstInst->stSamaConflictCheck));

	return ERR_OK;
}

// sama.onRfInputAbilitySet (int nRfInputId, int bAbility)
HERROR sapi_base_OnRfInputAbilitySet (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL					 bAbility;
	HUINT32					 ulRfInputId;
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	HERROR					 hErr;

	HxLOG_Trace();

	if (2 != nArgc)
	{
		HxLOG_Error ("Argument not valid: sama.onRfInputAbilitySet (int nRfInputId, int bAbility)\n");
		return ERR_FAIL;
	}

	ulRfInputId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	bAbility = (HBOOL)HxOBJECT_INT(apArgv[1]);

	if (ulRfInputId >= RxRFINPUT_NUM_MAX)
	{
		HxLOG_Error ("ulRfInputId(%d), RxRFINPUT_NUM_MAX(%d)\n", ulRfInputId, RxRFINPUT_NUM_MAX);
		return ERR_FAIL;
	}

	pstInst->stSamaConflictCheck.anRfInputAbility[ulRfInputId] = (HINT32)bAbility;


	hErr = SAMALIB_CFLT_SetRfInputAbility(&(pstInst->stSamaConflictCheck), ulRfInputId, bAbility);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAMALIB_CFLT_SetRfInputAbility err: ulRfInputId(%d), bAbility(%d)\n", ulRfInputId, bAbility);
		return ERR_FAIL;
	}


	return ERR_OK;
}

// sama.onAvailableSlotsChanged (int nPid, int nSlotNum, int *anSlotArray)
HERROR sapi_base_OnAvailableSlotsChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32					 nPid;
	HUINT32					 ulCount, ulSlotNum;
	HUINT32					*pulSlotArray;
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;


	HxLOG_Trace();


	if (3 != nArgc)
	{
		HxLOG_Error ("Argument not valid: sama.onSchedulesLoaded (int nSchNum, DxSchedule_t *pstSchArray)\n");
		return ERR_FAIL;
	}

	nPid			= (HINT32)HxOBJECT_INT(apArgv[0]);
	ulSlotNum		= (HUINT32)HxOBJECT_INT(apArgv[1]);
	pulSlotArray	= (HUINT32 *)HxOBJECT_BIN_DATA(apArgv[2]);

	// 내부 Cache 조정:
	if (nPid == pstInst->nPid)
	{
		pstInst->ulAvailSlotNum = (ulSlotNum < SAMA_EMPTY_SLOT_PER_PROCESS) ? ulSlotNum : SAMA_EMPTY_SLOT_PER_PROCESS;

		for (ulCount = 0; ulCount < SAMA_EMPTY_SLOT_PER_PROCESS; ulCount++)
		{
			pstInst->aulAvailSlots[ulCount] = SAPI_SLOTID_NULL;
		}

		for (ulCount = 0; ulCount < pstInst->ulAvailSlotNum; ulCount++)
		{
			pstInst->aulAvailSlots[ulCount] = pulSlotArray[ulCount];
		}
	}

	return ERR_OK;
}


// sama.onSchedulesLoaded (int nSchNum, DxSchedule_t *pstSchArray)
HERROR sapi_base_OnSchedulesLoaded (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulCnt, ulSchNum;
	DxSchedule_t			*pstSchArray;
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	SAPI_NotiInfo_t			 stNotiInfo;


	HxLOG_Trace();


	if (2 != nArgc)
	{
		HxLOG_Error ("Argument not valid: sama.onSchedulesLoaded (int nSchNum, DxSchedule_t *pstSchArray)\n");
		return ERR_FAIL;
	}

	ulSchNum = (HUINT32)HxOBJECT_INT(apArgv[0]);
	pstSchArray = (DxSchedule_t *)HxOBJECT_BIN_DATA(apArgv[1]);

	SAMA_MemSet (&stNotiInfo, 0, sizeof(SAPI_NotiInfo_t));

	// 내부 Cache 조정:
	pstInst->bScheduleLoaded = FALSE;

	if (NULL != pstInst->stSamaConflictCheck.pstScheduleList)
	{
		pstInst->stSamaConflictCheck.pstScheduleList = HLIB_LIST_RemoveAllFunc (pstInst->stSamaConflictCheck.pstScheduleList, SAMA_Free_CB);
	}

	HxLOG_Info ("[%s] SAPI Loaded the Schedule List from SAMA: (+)\n", __FUNCTION__);

	for (ulCnt = 0; ulCnt < ulSchNum; ulCnt++)
	{
		DxSchedule_t			*pstSrcSchedule = &(pstSchArray[ulCnt]);

		HxLOG_Info ("[%s] >> Item Slot(%02d): Type(%d)[%s], StartTime %d.%d.%d %d:%d, Duration %dmin\n", __FUNCTION__,
								pstSrcSchedule->ulSlot, pstSrcSchedule->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e),pstSrcSchedule->eRsvType),
								pstSrcSchedule->stStartTime.stDate.usYear,
								pstSrcSchedule->stStartTime.stDate.ucMonth,
								pstSrcSchedule->stStartTime.stDate.ucDay,
								pstSrcSchedule->stStartTime.stTime.ucHour,
								pstSrcSchedule->stStartTime.stTime.ucMinute,
								(pstSrcSchedule->ulDuration / 60));

		if(sapi_base_IsScheduleVolatile(pstSrcSchedule)==FALSE)
		{
			DxSchedule_t			*pstDstSchedule = (DxSchedule_t *)SAMA_Malloc (sizeof(DxSchedule_t));

			if (NULL != pstDstSchedule)
			{
				SAMA_MemCpy (pstDstSchedule, pstSrcSchedule, sizeof(DxSchedule_t));
				pstInst->stSamaConflictCheck.pstScheduleList = HLIB_LIST_Append (pstInst->stSamaConflictCheck.pstScheduleList, pstDstSchedule);
			}
		}
	}

	HxLOG_Info ("[%s] SAPI Loaded the Schedule List from SAMA: (-)\n", __FUNCTION__);

	pstInst->bScheduleLoaded = TRUE;

	// 외부 notifier 실행
	stNotiInfo.stSchLoaded.eNotifyType	= eSAPI_NOTIFY_TYPE_SchedulesLoaded;
	stNotiInfo.stSchLoaded.ulSchNum 	= ulSchNum;
	stNotiInfo.stSchLoaded.pstSchArray	= pstSchArray;
	stNotiInfo.stSchLoaded.nRequestPid	= HLIB_RPC_GetProgramIdFromRequestId(nRequestId);


	sapi_ExecuteNotifiers (pstInst, &stNotiInfo);
	return ERR_OK;
}

// sama.onScheduleAdded (int nSlotId, DxSchedule_t *pstSchedule)
HERROR sapi_base_OnScheduleAdded (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulSlotId;
	DxSchedule_t			*pstSchedule;
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	SAPI_NotiInfo_t			 stNotiInfo;
	HERROR					 hErr;

	HxLOG_Trace();

	if (2 != nArgc)
	{
		HxLOG_Error ("Argument not valid: (int nSlotId, DxSchedule_t *pstSchedule)\n");
		return ERR_FAIL;
	}

	ulSlotId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	pstSchedule = (DxSchedule_t *)HxOBJECT_BIN_DATA(apArgv[1]);

	// 내부 Cache 조정:
	hErr = sapi_AppendScheduleToCache (pstInst, pstSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Schedule Append failed\n");
		return ERR_OK;
	}

	// 외부 notifier 실행
	stNotiInfo.stSchAdded.eNotifyType	= eSAPI_NOTIFY_TYPE_ScheduleAdded;
	stNotiInfo.stSchAdded.ulSlotId		= ulSlotId;
	stNotiInfo.stSchAdded.pstSchedule	= pstSchedule;
	stNotiInfo.stSchAdded.nRequestPid	= HLIB_RPC_GetProgramIdFromRequestId(nRequestId);

	sapi_ExecuteNotifiers (pstInst, &stNotiInfo);

	return ERR_OK;
}

// sama.onScheduleChanged (int nSlotId,  HBOOL bExcuteNotifier, DxSchedule_t *pstSchedule)
HERROR sapi_base_OnScheduleChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulSlotId;
	DxSchedule_t			*pstSchedule, *pstSrcSchedule;
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	SAPI_NotiInfo_t			 stNotiInfo;
	HBOOL 				bExcuteNotifier;


	HxLOG_Trace();

	if (3 != nArgc)
	{
		HxLOG_Error ("Argument not valid: (int nSlotId, DxSchedule_t *pstSchedule)\n");
		return ERR_FAIL;
	}

	ulSlotId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	bExcuteNotifier = (HBOOL)HxOBJECT_INT(apArgv[1]);
	pstSchedule = (DxSchedule_t *)HxOBJECT_BIN_DATA(apArgv[2]);

	// 내부 Cache 조정:
	pstSrcSchedule = sapi_FindScheduleFromCache (pstInst, ulSlotId);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error ("Schedule with ulSlot(%d) not exists:\n", ulSlotId);
		return ERR_FAIL;
	}

	SAMA_MemCpy (pstSrcSchedule, pstSchedule, sizeof(DxSchedule_t));
	pstSrcSchedule->ulSlot = ulSlotId;



	sapi_PrintSchedule(pstSrcSchedule);


	// 외부 notifier 실행
	stNotiInfo.stSchChanged.eNotifyType	= eSAPI_NOTIFY_TYPE_ScheduleChanged;
	stNotiInfo.stSchChanged.ulSlotId	= ulSlotId;
	stNotiInfo.stSchChanged.pstSchedule	= pstSchedule;
	stNotiInfo.stSchChanged.nRequestPid	= HLIB_RPC_GetProgramIdFromRequestId(nRequestId);

	if(bExcuteNotifier == TRUE)
	{
		sapi_ExecuteNotifiers (pstInst, &stNotiInfo);
	}

	return ERR_OK;
}

HERROR sapi_base_OnScheduleChangedEx (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32				 ulSlotId;
	DxSchedule_t			*pstSrcSchedule;
	sapi_Inst_t			*pstInst = (sapi_Inst_t *)pvUserData;
	SAPI_ScheduleChanged_t		*pstChangedSchedule;

	HxLOG_Trace();

	if (2 != nArgc)
	{
		HxLOG_Error ("Argument not valid: (int nSlotId, DxSchedule_t *pstSchedule)\n");
		return ERR_FAIL;
	}

	ulSlotId  = (HUINT32)HxOBJECT_INT(apArgv[0]);
	pstChangedSchedule = (SAPI_ScheduleChanged_t *)HxOBJECT_BIN_DATA(apArgv[1]);

	// 내부 Cache 조정:
	pstSrcSchedule = sapi_FindScheduleFromCache (pstInst, ulSlotId);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error ("Schedule with ulSlot(%d) not exists:\n", ulSlotId);
		return ERR_FAIL;
	}

	if (pstChangedSchedule->bStartPaddingChanged)
	{
		pstSrcSchedule->uExtInfo.stRec.ulPreOffsetTime = pstChangedSchedule->ulStartPadding;
	}
	if (pstChangedSchedule->bEndPaddingChanged)
	{
		pstSrcSchedule->uExtInfo.stRec.ulPostOffsetTime = pstChangedSchedule->ulEndPadding;
	}
	if (pstChangedSchedule->bReadyChanged)
	{
		pstSrcSchedule->eRsvReady = pstChangedSchedule->ulReady;
	}

	return ERR_OK;
}

// sama.onScheduleRemoved (int nSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo)
HERROR sapi_base_OnScheduleRemoved (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulSlotId;
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	DxSchedule_t			*pstSchedule, *pstSrcSchedule;
	SAPI_NotiInfo_t			 stNotiInfo;
	HERROR					 hErr;
	DxSchedule_RemovedInfo_t *pstRemovedInfo;


	HxLOG_Trace();

	if (2 != nArgc)
	{
		HxLOG_Error ("Argument not valid: (int nSlotId)\n");
		return ERR_FAIL;
	}

	ulSlotId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	pstRemovedInfo = (DxSchedule_RemovedInfo_t *)HxOBJECT_BIN_DATA(apArgv[1]);

	// 내부 Cache 조정:
	pstSrcSchedule = sapi_FindScheduleFromCache (pstInst, ulSlotId);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error ("Schedule with ulSlot(%d) not exists:\n", ulSlotId);
		return ERR_FAIL;
	}

	pstSchedule = (DxSchedule_t *)SAMA_Malloc(sizeof(DxSchedule_t));
	if (NULL == pstSchedule)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstSchedule, pstSrcSchedule, sizeof(DxSchedule_t));

	hErr = sapi_RemoveScheduleFromCache (pstInst, ulSlotId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Schedule remove failed: ulSlot(%d)\n", ulSlotId);

		SAMA_Free(pstSchedule);
		return ERR_FAIL;
	}

	pstSchedule->bRemoved = TRUE;
	HxSTD_MemCopy(&(pstSchedule->stRemovedInfo), pstRemovedInfo, sizeof(DxSchedule_RemovedInfo_t));
	// 외부 notifier 실행
	stNotiInfo.stSchRemoved.eNotifyType	= eSAPI_NOTIFY_TYPE_ScheduleRemoved;
	stNotiInfo.stSchRemoved.ulSlotId	= ulSlotId;
	stNotiInfo.stSchRemoved.pstSchedule = pstSchedule;
	stNotiInfo.stSchRemoved.nRequestPid	= HLIB_RPC_GetProgramIdFromRequestId(nRequestId);


	sapi_ExecuteNotifiers (pstInst, &stNotiInfo);

	SAMA_Free(pstSchedule);
	return ERR_OK;
}

// sama.onSchedulePaused (int nSlotId)
HERROR sapi_base_OnSchedulePaused (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulSlotId;
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	DxSchedule_t			*pstSrcSchedule;


	HxLOG_Trace();

	if (1 != nArgc)
	{
		HxLOG_Error ("Argument not valid: (int nSlotId)\n");
		return ERR_FAIL;
	}

	ulSlotId = (HUINT32)HxOBJECT_INT(apArgv[0]);

	// 내부 Cache 조정:
	pstSrcSchedule = sapi_FindScheduleFromCache (pstInst, ulSlotId);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error ("Schedule with ulSlot(%d) not exists:\n", ulSlotId);
		return ERR_FAIL;
	}

	pstSrcSchedule->bPaused = TRUE;

	return ERR_OK;
}

// sama.onScheduleResumed (int nSlotId)
HERROR sapi_base_OnScheduleResumed (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulSlotId;
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	DxSchedule_t			*pstSrcSchedule;


	HxLOG_Trace();

	if (1 != nArgc)
	{
		HxLOG_Error ("Argument not valid: (int nSlotId)\n");
		return ERR_FAIL;
	}

	ulSlotId = (HUINT32)HxOBJECT_INT(apArgv[0]);

	// 내부 Cache 조정:
	pstSrcSchedule = sapi_FindScheduleFromCache (pstInst, ulSlotId);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error ("Schedule with ulSlot(%d) not exists:\n", ulSlotId);
		return ERR_FAIL;
	}

	pstSrcSchedule->bPaused = FALSE;

	return ERR_OK;
}

// sama.onConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, int nNumConflict, SAPI_ConflictItem_t *pstConfArray)
HERROR sapi_base_OnConflictAddTried (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	DxSchedule_t			*pstSchedule;
	SAPI_Conflict_t			*pstConflict;
	SAPI_NotiInfo_t			 stNotiInfo;

	HxLOG_Trace();

	if (4 != nArgc)
	{
		HxLOG_Error ("Argument not valid: (int nSlotId)\n");
		return ERR_FAIL;
	}

	// 외부 notifier 실행
	pstSchedule = (DxSchedule_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	stNotiInfo.stConfSchAdded.eNotifyType	= eSAPI_NOTIFY_TYPE_ConflictScheduleAdded;
	pstConflict = &(stNotiInfo.stConfSchAdded.stConflicted);
	HxSTD_MemCopy(&(pstConflict->stTriedSchedule), pstSchedule, sizeof(DxSchedule_t));
	pstConflict->eType					= (SAPI_ConflictType_e)HxOBJECT_INT(apArgv[1]);
	pstConflict->ulNumConflict			= (HUINT32)HxOBJECT_INT(apArgv[2]);
	pstConflict->ulConflictArraySize	= sizeof(SAPI_ConflictItem_t) * pstConflict->ulNumConflict;
	if (pstConflict->ulNumConflict > 0)
	{
		pstConflict->pstConflictArray	= (SAPI_ConflictItem_t *)HxOBJECT_BIN_DATA(apArgv[3]);
	}
	else
	{
		pstConflict->pstConflictArray	= NULL;
	}

	HxLOG_Debug("[%s:%d] ulNumConflict(%d) \n", __FUNCTION__, __LINE__, pstConflict->ulNumConflict);

	//일반적으로 conflict result가 sapi->sama->sapi 로 오기 때문에, sapi instance 가 이미 conflict result를 가지고 있지만
	//예외적인 상황에서 sama가 직접 conflict check 후에 result 를 주는 경우가 있기 때문에 (sama->sapi) 여기서도 update 해둔다.
	{
		HUINT32		ulCnt = 0;

		SAMALIB_CFLT_ClearConflictResult(&(pstInst->stSamaConflictCheck.stConflictResult));

		SAMA_MemCpy(&(pstInst->stSamaConflictCheck.stConflictResult.stTriedSchedule), pstSchedule, sizeof(DxSchedule_t));

		pstInst->stSamaConflictCheck.stConflictResult.eType = pstConflict->eType;

		if (pstConflict->ulNumConflict > 0)
		{
			for(ulCnt = 0; ulCnt < pstConflict->ulNumConflict; ulCnt++)
			{
				if(NULL != pstConflict->pstConflictArray && NULL != &(pstConflict->pstConflictArray[ulCnt]))
				{
					SAMALIB_CFLT_AppendConflictResult(&(pstInst->stSamaConflictCheck.stConflictResult), &(pstConflict->pstConflictArray[ulCnt]));
				}
			}
		}


	}

	stNotiInfo.stConfSchAdded.nRequestPid	= HLIB_RPC_GetProgramIdFromRequestId(nRequestId);



	sapi_ExecuteNotifiers (pstInst, &stNotiInfo);

	return ERR_OK;
}


// sama.onTimeUp (int nSlotId, SAPI_TimerItemType_e eTimerType, DxSchedule_t *pstSchedule)
HERROR sapi_base_OnTimeUp (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32					 ulSlotId;
	SAPI_TimerItemType_e	 eTimerType;
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;
	DxSchedule_t			*pstSrcSchedule; // 내부 cache 수정용.
	SAPI_NotiInfo_t			 stNotiInfo;
	DxSchedule_t			*pstTimeUpSchedule; // one way, start time 만 가진 schedule 의 경우, sama schedule에서 삭제되므로, 같이 넘겨주기 위함.

	HxLOG_Trace();

	if (3 != nArgc)
	{
		HxLOG_Error ("Argument not valid: (int nSlotId, SAPI_TimerItemType_e eTimerType, DxSchedule_t *pstSchedule)\n");
		return ERR_FAIL;
	}

	ulSlotId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	eTimerType = (SAPI_TimerItemType_e)HxOBJECT_INT(apArgv[1]);
	pstTimeUpSchedule	= HxOBJECT_BIN_DATA (apArgv[2]);

	HxLOG_Debug("ulSlotId: %d, eTimerType: %d \n", ulSlotId, eTimerType);

	// 내부 Cache 조정:
	pstSrcSchedule = sapi_FindScheduleFromCache (pstInst, ulSlotId);

	if(NULL == pstSrcSchedule)
	{
		HxLOG_Error("pstSrcSchedule is NULL \n");
		return ERR_FAIL;
	}

	switch (pstSrcSchedule->eRsvType)
	{
	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_WATCHING_TBR:
	case DxRSVTYPE_WATCHING_SERIES_EBR:
	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_POWER_ON:
	case DxRSVTYPE_RECORD_INSTANT:
	case DxRSVTYPE_RECORD_DELAYED:
	case DxRSVTYPE_DLNA_EBR:
	case DxRSVTYPE_DLNA_TBR:
	case DxRSVTYPE_DLNA_DUBBING:
	case DxRSVTYPE_NETSTREAMING_LIVE:
	case DxRSVTYPE_NETSTREAMING_FILE:
	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
	case DxRSVTYPE_DESC_REC_STANDBY:
	case DxRSVTYPE_ACTIVATION_TIME:
		switch (eTimerType)
		{
		case eSAPI_TIMER_TYPE_Ready:
			pstSrcSchedule->eStatus = DxRSVSTATUS_READY;
			break;

		case eSAPI_TIMER_TYPE_Start:
			pstSrcSchedule->eStatus = DxRSVSTATUS_RUNNING;
			break;

		case eSAPI_TIMER_TYPE_Stop:
			pstSrcSchedule->eStatus = (DxRSVREPEAT_ONCE == pstSrcSchedule->eRepeat) ? DxRSVSTATUS_STOP : DxRSVSTATUS_WAITING;
			break;

		default:
			break;
		}

		break;
	case DxRSVTYPE_CIPLUS_OP_TUNE:
		// TODO: CI+ Operator Profile 에 따라 깨어나서 tunning 하며 service list를 만드는 시나리오 적용 필요.
		// hms1000s ph1 branch code rev#100570 참고.
	default:
		break;
	}

	pstTimeUpSchedule->eStatus = pstSrcSchedule->eStatus;

	// 외부 notifier 실행
	stNotiInfo.stTimeUp.eNotifyType		= eSAPI_NOTIFY_TYPE_TimeUp;
	stNotiInfo.stTimeUp.ulSlotId		= ulSlotId;
	stNotiInfo.stTimeUp.eTimerType		= eTimerType;
	stNotiInfo.stTimeUp.pstSchedule	= pstTimeUpSchedule;

	sapi_ExecuteNotifiers (pstInst, &stNotiInfo);

	return ERR_OK;
}

// sama.onResourceRefreshed ( )
HERROR sapi_base_OnResourceRefreshed (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	sapi_Inst_t				*pstInst = (sapi_Inst_t *)pvUserData;

	HxLOG_Trace();

	if (pstInst != NULL)
	{
		SAMALIB_SVC_ClearCache(&(pstInst->stSamaConflictCheck));
	}

	return ERR_OK;
}


