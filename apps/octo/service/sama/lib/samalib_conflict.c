/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_conflict.c
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



#include <sama_lib.h>
#include "./local_include/sama_lib_int.h"

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

STATIC INLINE void samalib_cflt_CheckRlibSession(sama_ConflictCheck_t *pstInst)
{
	if(pstInst == NULL)
	{
		return;
	}

	if(pstInst->hRlibSession == (Handle_t)NULL)
	{
		pstInst->hRlibSession = RLIB_CreateSessionHandle("SAMA_LIB_CFLT");
	}

	return;

}


#define ___MEMBER_FUNCTIONS___
/*******************************************************************/
/********************      Member Functions       ******************/
/*******************************************************************/
HINT32 samalib_cflt_cbCmpCfltTimeEvtByTime (void *pvUser, void *pvListItem)
{
	sama_CnftTimeEvt_t	*pstUserData = (sama_CnftTimeEvt_t *)pvUser;
	sama_CnftTimeEvt_t	*pstListItem = (sama_CnftTimeEvt_t *)pvListItem;

	// user가 미래일 경우 뒤로 보낸다.
	if (pstUserData->utTime > pstListItem->utTime)
	{
		return -1;
	}
	// user가 과거인 경우 앞으로 보낸다.
	else if (pstUserData->utTime < pstListItem->utTime)
	{
		return 1;
	}

	// 같은 시간이더라도 Stop/Padding Stop이 Start/Padding Start 보다 먼저 TimeLine에 존재해야 충돌 체크를 제대로 합니다.
	if (((pstUserData->eTimerType == eSAPI_TIMER_TYPE_TL_PaddingStart) || (pstUserData->eTimerType == eSAPI_TIMER_TYPE_TL_Start)) &&
		((pstListItem->eTimerType == eSAPI_TIMER_TYPE_TL_PaddingEnd) || (pstListItem->eTimerType == eSAPI_TIMER_TYPE_TL_Stop)))
	{
		return -1;
	}

	if (((pstUserData->eTimerType == eSAPI_TIMER_TYPE_TL_PaddingEnd) || (pstUserData->eTimerType == eSAPI_TIMER_TYPE_TL_Stop)) &&
		((pstListItem->eTimerType == eSAPI_TIMER_TYPE_TL_PaddingStart) || (pstListItem->eTimerType == eSAPI_TIMER_TYPE_TL_Start)))
	{
		return 1;
	}

	// slot이 작으면 앞으로 보낸다. (timer에 넣는 순서와 동일하게 유지한다.)
	if ( (pstUserData->pstSchedule)&&(pstListItem->pstSchedule)
		&&(pstUserData->pstSchedule->ulSlot < pstListItem->pstSchedule->ulSlot))
	{
		return 1;
	}

	//HxLOG_Debug(HxANSI_COLOR_RED("sort with out unique order. \n"));

	return 0;
}


HERROR SAMALIB_CFLT_SetResourceInfo (sama_ConflictCheck_t *pstInst)
{
	HUINT32				 ulCnt, ulActionNum = 0;
	HUINT32				 ulRfInputNum, ulTunerNum;
	SAPI_RscInfo_t		*pstRscInfo = &(pstInst->stRscInfo);

	HxLOG_Trace();

	if (TRUE != pstInst->bRscSet)
	{
		HxLOG_Error("pstInst->bRscSet is FALSE \n");
		return ERR_FAIL;
	}

	for (ulCnt = 0; ulCnt < eSAPI_ACTION_TYPE_MAX; ulCnt++)
	{
		ulActionNum += pstRscInfo->anMaxActionNum[ulCnt];
	}

	samalib_cflt_CheckRlibSession(pstInst);

	HxLOG_Debug("pstInst->hRlibSession: 0x%x \n", pstInst->hRlibSession);

	(void)RLIB_SetActionNumber(pstInst->hRlibSession, ulActionNum);

	for (ulCnt = 0; ulCnt < eRxRSCTYPE_MAX; ulCnt++)
	{
		(void)RLIB_SetResourceNumber(pstInst->hRlibSession, (RxRscType_e)ulCnt, pstRscInfo->anRscNum[ulCnt]);
	}

	ulRfInputNum = (pstRscInfo->anRscNum[eRxRSCTYPE_RFINPUT] > RxRFINPUT_NUM_MAX) ? RxRFINPUT_NUM_MAX : pstRscInfo->anRscNum[eRxRSCTYPE_RFINPUT];
	ulTunerNum = (pstRscInfo->anRscNum[eRxRSCTYPE_TUNER_GROUP] > RxTUNER_NUM_MAX) ? RxTUNER_NUM_MAX : pstRscInfo->anRscNum[eRxRSCTYPE_TUNER_GROUP];

	for (ulCnt = 0; ulCnt < ulRfInputNum; ulCnt++)
	{
		(void)RLIB_SetRfInfoCapacity(pstInst->hRlibSession, ulCnt, &(pstRscInfo->astRfInfoCap[ulCnt]));
	}

	for (ulCnt = 0; ulCnt < ulTunerNum; ulCnt++)
	{
		(void)RLIB_SetTunerCapacity(pstInst->hRlibSession, ulCnt, pstRscInfo->aeTunerSupportDeliType[ulCnt]);
	}

	RLIB_SetRfInputAbility(pstInst->hRlibSession, 0, TRUE);
	return ERR_OK;
}

HERROR SAMALIB_CFLT_SetRfInputAbility (sama_ConflictCheck_t *pstInst, HUINT32 ulRfInputId, HBOOL bAbility)
{
	HERROR			 hErr;

	HxLOG_Trace();

	if (NULL == pstInst)
	{
		HxLOG_Error("NULL == pstInst");
		return ERR_FAIL;
	}


	if (ulRfInputId >= pstInst->stRscInfo.anRscNum[eRxRSCTYPE_RFINPUT])
	{
		HxLOG_Error("ulRfInputId (%d) >= pstInst->stRscInfo.anRscNum[eRxRSCTYPE_RFINPUT] (%d)", ulRfInputId, pstInst->stRscInfo.anRscNum[eRxRSCTYPE_RFINPUT]);
		return ERR_FAIL;
	}

	samalib_cflt_CheckRlibSession(pstInst);

	hErr = RLIB_SetRfInputAbility(pstInst->hRlibSession, ulRfInputId, bAbility);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_SetRfInputAbility err: ulRfInputId(%d)\n", ulRfInputId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SAMALIB_CFLT_ClearConflictResult (SAPI_Conflict_t *pstConflict)
{
	HUINT32						 ulArraySize;
	SAPI_ConflictItem_t			*pstArray;

	ulArraySize	= pstConflict->ulConflictArraySize;
	pstArray	= pstConflict->pstConflictArray;

	if ((NULL != pstArray) && (ulArraySize > 0))
	{
		SAMA_MemSet(pstArray, 0, sizeof(SAPI_ConflictItem_t) * ulArraySize);
	}
	else if ((NULL != pstArray) && (0 == ulArraySize))
	{
		SAMA_Free(pstArray);
		pstArray = NULL;
	}
	else
	{
		pstArray = NULL;
		ulArraySize = 0;
	}

	SAMA_MemSet(pstConflict, 0, sizeof(SAPI_Conflict_t));

	pstConflict->eType					= eSAPI_CONFLICT_TYPE_None;
	pstConflict->ulConflictArraySize	= ulArraySize;
	pstConflict->pstConflictArray		= pstArray;

	return ERR_OK;
}

HERROR samalib_cflt_DuplicateConflictResult (SAPI_Conflict_t *pstDstConflict, SAPI_Conflict_t *pstSrcConflict)
{
	HUINT32						 ulCount;

	SAMALIB_CFLT_ClearConflictResult(pstDstConflict);

	SAMA_MemCpy(&(pstDstConflict->stTriedSchedule), &(pstSrcConflict->stTriedSchedule), sizeof(DxSchedule_t));
	pstDstConflict->eType = pstSrcConflict->eType;
	pstDstConflict->ePaddingConflictType = pstSrcConflict->ePaddingConflictType;

	for (ulCount = 0; ulCount < pstSrcConflict->ulNumConflict; ulCount++)
	{
		(void)SAMALIB_CFLT_AppendConflictResult(pstDstConflict, &(pstSrcConflict->pstConflictArray[ulCount]));
	}

	return ERR_OK;
}

HERROR SAMALIB_CFLT_AppendConflictResult (SAPI_Conflict_t *pstConflict, SAPI_ConflictItem_t *pstItem)
{
	HUINT32					 ulCount;
	SAPI_ConflictItem_t 	*pstArrayItem;

	if (pstConflict->ulNumConflict >= pstConflict->ulConflictArraySize)
	{
		HUINT32				 ulNewArraySize = pstConflict->ulNumConflict + 4;
		SAPI_ConflictItem_t	*pstNewArray = (SAPI_ConflictItem_t *)SAMA_Malloc(sizeof(SAPI_ConflictItem_t) * ulNewArraySize);

		if (NULL == pstNewArray)
		{
			HxLOG_Error ("Memory allocation failed:\n");
			return ERR_FAIL;
		}

		if (NULL != pstConflict->pstConflictArray)
		{
			SAMA_MemCpy(pstNewArray, pstConflict->pstConflictArray, sizeof(SAPI_ConflictItem_t) * pstConflict->ulNumConflict);
			SAMA_Free(pstConflict->pstConflictArray);
			pstConflict->pstConflictArray = NULL;
		}

		pstConflict->pstConflictArray = pstNewArray;
		pstConflict->ulConflictArraySize = ulNewArraySize;
	}

	if (pstConflict->stTriedSchedule.ulSlot == pstItem->ulSlot)
	{
		HxLOG_Debug ("Slot(%d) is the conflict-checking schedule:\n", pstItem->ulSlot);
		return ERR_OK;
	}

	for (ulCount = 0; ulCount < pstConflict->ulNumConflict; ulCount++)
	{
		pstArrayItem = &(pstConflict->pstConflictArray[ulCount]);

		if (pstArrayItem->ulSlot == pstItem->ulSlot)
		{
			HxLOG_Error ("Slot(%d) Already added to the conflict list:\n", pstItem->ulSlot);
			return ERR_OK;
		}
	}

	HxLOG_Debug("%d th conflict item pstItem->ulSlot: %d\n", pstConflict->ulNumConflict, pstItem->ulSlot);

	pstArrayItem = &(pstConflict->pstConflictArray[pstConflict->ulNumConflict]);
	SAMA_MemCpy(pstArrayItem, pstItem, sizeof(SAPI_ConflictItem_t));
	pstConflict->ulNumConflict ++;

	return ERR_OK;
}

HERROR samalib_cflt_ClearActions (sama_ConflictCheck_t *pstInst)
{
	HUINT32				 ulCnt, ulNewActionNum = 0;

	if (TRUE != pstInst->bRscSet)					{ return ERR_FAIL; }

	for (ulCnt = 0; ulCnt < eSAPI_ACTION_TYPE_MAX; ulCnt++)
	{
		ulNewActionNum += pstInst->stRscInfo.anMaxActionNum[ulCnt];
	}

	if (ulNewActionNum > pstInst->ulActionNum)
	{
		sama_ActionItem_t			*pstNewActionArray;

		pstNewActionArray = (sama_ActionItem_t *)SAMA_Malloc(sizeof(sama_ActionItem_t) * ulNewActionNum);
		if (NULL == pstNewActionArray)				{ return ERR_FAIL; }

		if (NULL != pstInst->pstActionArray)			{ SAMA_Free(pstInst->pstActionArray); }
		pstInst->ulActionNum		= ulNewActionNum;
		pstInst->pstActionArray 	= pstNewActionArray;
	}

	SAMA_MemSet(pstInst->pstActionArray, 0, sizeof(sama_ActionItem_t) * pstInst->ulActionNum);
	for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
	{
		sama_ActionItem_t			*pstAction = &(pstInst->pstActionArray[ulCnt]);

		pstAction->bAllocated	= FALSE;
		pstAction->ulActionId	= ulCnt;
		pstAction->ulSlot		= 0xFFFFFFFF;
		pstAction->eActState	= eSAPI_ACTIONSTATE_None;

		(void)RLIB_ClearActionResource(pstInst->hRlibSession, ulCnt);
	}

	SAMA_MemSet(pstInst->anMaxActCnt, 0, sizeof(HINT32) * eSAPI_ACTION_TYPE_MAX);

	return ERR_OK;
}
HERROR samalib_cflt_GetActionPriority(sama_ActionState_e eState, HUINT32 *pulPriority)
{
	if(eState == eSAPI_ACTIONSTATE_PaddingStart) // 1
	{
		*pulPriority = eSAPI_ACTIONPRIORITY_PaddingStart;
	}
	else if(eState == eSAPI_ACTIONSTATE_PaddingStop) // 2
	{
		*pulPriority = eSAPI_ACTIONPRIORITY_PaddingStop;
	}
	else if(eState == eSAPI_ACTIONSTATE_Running) // 8
	{
		*pulPriority = eSAPI_ACTIONPRIORITY_Running;
	}
	else
	{
		return ERR_FAIL;
	}
	return ERR_OK;

}
sama_ActionItem_t *samalib_cflt_TakeAction (sama_ConflictCheck_t *pstInst, HUINT32 ulPriority)
{
	HUINT32				 ulCnt;

	if ((pstInst->ulActionNum > 0) && (NULL != pstInst->pstActionArray))
	{
		for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
		{
			sama_ActionItem_t			*pstAction = &(pstInst->pstActionArray[ulCnt]);

			if (TRUE != pstAction->bAllocated)
			{
				pstAction->bAllocated	= TRUE;
				pstAction->eActType		= eSAPI_ACTION_TYPE_None;
				pstAction->ulPriority 	= ulPriority;
				return pstAction;
			}
		}
	}

	return NULL;
}

void samalib_cflt_ReleaseAction (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction)
{
	if (NULL != pstAction)
	{
		pstAction->bAllocated	= FALSE;
		pstAction->ulSlot		= 0xFFFFFFFF;
		pstAction->eActType		= eSAPI_ACTION_TYPE_None;
	}
}

sama_ActionItem_t *samalib_cflt_GetActionByActionId (sama_ConflictCheck_t *pstInst, HUINT32 ulActionId)
{
	HUINT32				 ulCnt;

	if ((pstInst->ulActionNum > 0) && (NULL != pstInst->pstActionArray))
	{
		for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
		{
			sama_ActionItem_t			*pstAction = &(pstInst->pstActionArray[ulCnt]);

			if ((TRUE == pstAction->bAllocated) && (ulActionId == pstAction->ulActionId))
			{
				return pstAction;
			}
		}
	}

	return NULL;
}

HUINT32 samalib_cflt_CountActionByActionType (sama_ConflictCheck_t *pstInst, SAPI_ActionType_e eActType)
{
	HUINT32				 ulCnt, ulNum = 0;

	if ((pstInst->ulActionNum > 0) && (NULL != pstInst->pstActionArray))
	{
		for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
		{
			sama_ActionItem_t			*pstAction = &(pstInst->pstActionArray[ulCnt]);

			if ((TRUE == pstAction->bAllocated) && (eActType == pstAction->eActType))
			{
				ulNum ++;
			}
		}
	}

	return ulNum;
}

sama_ActionItem_t *samalib_cflt_GetActionByActionType (sama_ConflictCheck_t *pstInst, SAPI_ActionType_e eActType, HUINT32 ulIndex)
{
	HUINT32				 ulCnt, ulNum = 0;

	if ((pstInst->ulActionNum > 0) && (NULL != pstInst->pstActionArray))
	{
		for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
		{
			sama_ActionItem_t			*pstAction = &(pstInst->pstActionArray[ulCnt]);

			if ((TRUE == pstAction->bAllocated) && (eActType == pstAction->eActType))
			{
				if (ulIndex == ulNum)
				{
					return pstAction;
				}

				ulNum ++;
			}
		}
	}

	return NULL;
}

HUINT32 samalib_cflt_CountActionByActionState (sama_ConflictCheck_t *pstInst, sama_ActionState_e eState)
{
	HUINT32				 ulCnt, ulNum = 0;

	if ((pstInst->ulActionNum > 0) && (NULL != pstInst->pstActionArray))
	{
		for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
		{
			sama_ActionItem_t			*pstAction = &(pstInst->pstActionArray[ulCnt]);

			if ((TRUE == pstAction->bAllocated) && (eState == pstAction->eActState))
			{
				ulNum ++;
			}
		}
	}

	return ulNum;
}


sama_ActionItem_t *samalib_cflt_GetActionByActionState (sama_ConflictCheck_t *pstInst, sama_ActionState_e eState, HUINT32 ulIndex)
{
	HUINT32				 ulCnt, ulNum = 0;

	if ((pstInst->ulActionNum > 0) && (NULL != pstInst->pstActionArray))
	{
		for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
		{
			sama_ActionItem_t			*pstAction = &(pstInst->pstActionArray[ulCnt]);

			if ((TRUE == pstAction->bAllocated) && (eState == pstAction->eActState))
			{
				if (ulIndex == ulNum)
				{
					return pstAction;
				}

				ulNum ++;
			}
		}
	}

	return NULL;
}

sama_ActionItem_t *samalib_cflt_GetActionBySlotId (sama_ConflictCheck_t *pstInst, HUINT32 ulSlot)
{
	HUINT32				 ulCnt;

	if ((pstInst->ulActionNum > 0) && (NULL != pstInst->pstActionArray))
	{
		for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
		{
			sama_ActionItem_t			*pstAction = &(pstInst->pstActionArray[ulCnt]);

			if ((TRUE == pstAction->bAllocated) && (ulSlot == pstAction->ulSlot))
			{
				return pstAction;
			}
		}
	}

	return NULL;
}

HERROR samalib_cflt_ForEachAction (sama_ConflictCheck_t *pstInst, SAMA_ForEachResult_e (*pfCbFunc)(sama_ActionItem_t *pstAction, HINT32 nArgc, HUINT32 *pulArgv), HINT32 nArgc, HUINT32 *pulArgv)
{
	HUINT32					 ulCnt;
	SAMA_ForEachResult_e	 eRet;

	if ((NULL == pstInst) || (NULL == pfCbFunc))
	{
		return ERR_FAIL;
	}

	if ((pstInst->ulActionNum > 0) && (NULL != pstInst->pstActionArray))
	{
		for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
		{
			sama_ActionItem_t			*pstAction = &(pstInst->pstActionArray[ulCnt]);

			if ((NULL != pstAction) && (TRUE == pstAction->bAllocated))
			{
				eRet = pfCbFunc(pstAction, nArgc, pulArgv);
				if (eSAMA_Result_BreakLoop == eRet)
				{
					break;
				}
			}
		}
	}

	return ERR_OK;
}



#define _____SPEC_SPECIFIED_____

STATIC HERROR samalib_cflt_CheckStreamingConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstNewSched)
{
	HERROR				 hErr = ERR_FAIL;

	if ((NULL == pstInst) || (NULL == pstNewSched))			{ return ERR_FAIL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_cflt_base_CheckStreamingConflictWithSchedule(pstInst, pstNewSched);
		break;
	}

	return hErr;
}


STATIC HERROR samalib_cflt_CheckLiveConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstNewSched)
{
	HERROR				 hErr = ERR_FAIL;

	if ((NULL == pstInst) || (NULL == pstNewSched))			{ return ERR_FAIL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_cflt_base_CheckLiveConflictWithSchedule(pstInst, pstNewSched);
		break;
	}

	return hErr;
}


STATIC HERROR samalib_cflt_CheckRecConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstNewSched)
{
	HERROR				 hErr = ERR_FAIL;

	if ((NULL == pstInst) || (NULL == pstNewSched))			{ return ERR_FAIL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_cflt_base_CheckRecConflictWithSchedule(pstInst, pstNewSched);
		break;
	}

	return hErr;
}

STATIC HBOOL	samalib_cflt_PossibleToUseSvcUidForTpInfo(DxSchedule_t *pstSchedule)
{
	if( (pstSchedule->eRsvType == DxRSVTYPE_NETSTREAMING_LIVE || pstSchedule->eRsvType == DxRSVTYPE_NETSTREAMING_FILE)
		&& (pstSchedule->uExtInfo.stStrm.eStreamType == DxRSVSTREAM_TYPE_SATIP_LIVE ||pstSchedule->uExtInfo.stStrm.eStreamType == DxRSVSTREAM_TYPE_BYPASS_LIVE))
	{
		return FALSE;
	}
	return TRUE;

}

STATIC HBOOL	samalib_cflt_compareScheduleUsingFilter(DxSchedule_t *pstSchedule, DxSchedule_FindFilterAttr_t *pstFilter)
{
	UNIXTIME			 utSchStartTime;
	UNIXTIME			 utStartTime;
	UNIXTIME			 utEndTime;


	if(pstFilter->eRsvType != DxRSVTYPE_NOTHING
		&& pstFilter->eRsvType != pstSchedule->eRsvType)
	{
		HxLOG_Debug("pstFilter->eRsvType(%d) != pstSchedule->eRsvType(%d) \n", pstFilter->eRsvType, pstSchedule->eRsvType);
		return FALSE;
	}

	if(pstFilter->ulSvcUid != 0
		&& pstFilter->ulSvcUid != pstSchedule->ulSvcUid)
	{
		HxLOG_Debug("pstFilter->ulSvcUid(%d) != pstSchedule->ulSvcUid(%d) \n", pstFilter->ulSvcUid, pstSchedule->ulSvcUid);
		return FALSE;
	}

	if((pstFilter->stTripleId.usOnId + pstFilter->stTripleId.usSvcId + pstFilter->stTripleId.usTsId) != 0
		&& (SAMA_Memcmp((&pstFilter->stTripleId),  &(pstSchedule->stTripleId), sizeof(DxSchedule_TripleId_t)) != 0))
	{
		HxLOG_Debug("pstFilter->stTripleId !=  pstFilter->stTripleId\n", pstFilter->ulSvcUid, pstSchedule->ulSvcUid);
		return FALSE;
	}

	if(pstFilter->eStatus != DxRSVSTATUS_MAX
		&& pstFilter->eStatus != pstSchedule->eStatus)
	{
		HxLOG_Debug("pstFilter->eStatus(%d) != pstSchedule->eStatus(%d) \n", pstFilter->eStatus, pstSchedule->eStatus);
		return FALSE;
	}

	if(pstFilter->eEpgType != eDxEPG_TYPE_ALL
		&& pstFilter->eEpgType != pstSchedule->eEpgType)
	{
		HxLOG_Debug("pstFilter->eEpgType(%d) != pstSchedule->eEpgType(%d) \n", pstFilter->eEpgType, pstSchedule->eEpgType);
		return FALSE;
	}

	HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstSchedule->stStartTime), &utSchStartTime);
	SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utSchStartTime, &utStartTime, &utEndTime);

	if(pstFilter->utStartTime != 0
		&& pstFilter->utStartTime != utStartTime)
	{
		HxLOG_Debug("pstFilter->utStartTime(%d) != utStartTime(%d) \n", pstFilter->utStartTime, utStartTime);
		return FALSE;
	}

	if(pstFilter->utEndTime != 0
		&& pstFilter->utEndTime != utEndTime)
	{
		HxLOG_Debug("pstFilter->utEndTime(%d) != utEndTime(%d) \n", pstFilter->utEndTime, utEndTime);
		return FALSE;
	}

	if(pstFilter->bRemoved != pstSchedule->bRemoved)
	{
		HxLOG_Debug("pstFilter->bRemoved(%d) != pstSchedule->bRemoved(%d) \n", pstFilter->bRemoved, pstSchedule->bRemoved);
		return FALSE;
	}

	if(pstFilter->stOverlapTimeRange.utOverlapTimeRangeStart != 0 && pstFilter->stOverlapTimeRange.utOverlapTimeRangeEnd != 0)
	{
		HBOOL	bIsOverlap = FALSE;

		bIsOverlap = HLIB_DATETIME_IsTimeOverwrapped(pstFilter->stOverlapTimeRange.utOverlapTimeRangeStart, pstFilter->stOverlapTimeRange.utOverlapTimeRangeEnd, utStartTime, utEndTime);

		if(bIsOverlap == FALSE)
		{
			HxLOG_Debug("pstFilter->stOverlapTimeRange.utOverlapTimeRangeStart(%d) pstFilter->stOverlapTimeRange.utOverlapTimeRangeEnd(%d) , utStartTime(%d), utEndTime(%d), not overlap\n",
				pstFilter->stOverlapTimeRange.utOverlapTimeRangeStart, pstFilter->stOverlapTimeRange.utOverlapTimeRangeEnd, utStartTime, utEndTime);
			return FALSE;
		}
	}

	HxLOG_Debug("Found Filtered schedule. slot=(%d) \n",pstSchedule->ulSlot);

	return TRUE;
}

// 결국 list append 함수이므로, 다 사용하고 나서 list remove 해야함.
HBOOL SAMALIB_CFLT_FindScheduleListUsingFilter(HxList_t *pstScheduleList, DxSchedule_FindFilterAttr_t *pstFilter, HxList_t	**pstFoundScheduleList)
{
	HxList_t			*pstListItem = NULL;
	HBOOL			bSame = FALSE;
	DxSchedule_t		*pstSchedule = NULL;

	HxLOG_Trace();

	if(pstScheduleList == NULL || pstFilter == NULL || pstFoundScheduleList == NULL)
	{
		HxLOG_Error("pstScheduleList == NULL || pstFilter == NULL || pstFoundScheduleList == NULL \n");
		return ERR_FAIL;
	}

	for (pstListItem = pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);
		if(pstSchedule != NULL)
		{
			bSame = samalib_cflt_compareScheduleUsingFilter(pstSchedule, pstFilter);
			if(bSame == TRUE)
			{
				HxLOG_Debug("pstSchedule satisfy pstFilter. add to list. slot=(%d) \n",pstSchedule->ulSlot);
				*pstFoundScheduleList = HLIB_LIST_Append(*pstFoundScheduleList, (void *)pstSchedule);

			}
			else
			{
				HxLOG_Debug("pstSchedule cannot satisfy pstFilter. slot=(%d) \n",pstSchedule->ulSlot);
			}
		}
		else
		{
			HxLOG_Error("pstSchedule is NULL \n");
		}
	}

	return ERR_OK;
}

void SAMALIB_CFLT_FindScheduleListFilterInit(DxSchedule_FindFilterAttr_t *pstFilter)
{
	SAMA_MemSet(pstFilter, 0x00, sizeof(DxSchedule_FindFilterAttr_t));

	pstFilter->eStatus = DxRSVSTATUS_MAX;
	pstFilter->eEpgType = eDxEPG_TYPE_ALL;
}

HERROR SAMALIB_CFLT_CheckConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstNewSched)
{
	HERROR				 hErr = ERR_FAIL;

	if ((NULL == pstInst) || (NULL == pstNewSched))			{ return ERR_FAIL; }

	(void)SAMALIB_CFLT_ClearConflictResult(&(pstInst->stConflictResult));
	HxLOG_Debug("Clear Pad Remove Info \n");
	pstInst->pstPadRemoveInfoList = HLIB_LIST_RemoveAllFunc(pstInst->pstPadRemoveInfoList, SAMA_Free_CB);

	SAMA_MemCpy(&(pstInst->stConflictResult.stTriedSchedule), pstNewSched, sizeof(DxSchedule_t));

	switch (pstNewSched->eRsvType)
	{
	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_WATCHING_TBR:
	case DxRSVTYPE_WATCHING_SERIES_EBR:
		hErr = samalib_cflt_CheckLiveConflictWithSchedule(pstInst, pstNewSched);
		break;

	//net streaming 의 경우 2가지 case: from file / from tuner.
	//from file 의 경우 시작 시간 조차도 conflict check 할 필요가 없다.
	case DxRSVTYPE_NETSTREAMING_LIVE:
	case DxRSVTYPE_NETSTREAMING_FILE:
		hErr = samalib_cflt_CheckStreamingConflictWithSchedule(pstInst, pstNewSched);
		break;

	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_INSTANT:
	case DxRSVTYPE_RECORD_DELAYED:
	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		hErr = samalib_cflt_CheckRecConflictWithSchedule(pstInst, pstNewSched);
		break;

	default:
		hErr = ERR_OK;
		break;
	}

	return hErr;
}

SAPI_ActionType_e SAMALIB_CFLT_GetSapiActionTypeBySchedule (DxSchedule_t *pstSchedule)
{
	DxSchedule_Spec_e				eSpec = SAMALIB_SPEC_Get();
	SAPI_ActionType_e			eActionType;

	if (NULL == pstSchedule)			{ return ERR_FAIL; }

	switch (eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		eActionType = samalib_cflt_base_getSapiActionTypeBySchedule(pstSchedule);
		break;
	}

	return eActionType;
}

HERROR samalib_cflt_CheckConflictUsingTimeLine (sama_ConflictCheck_t *pstInst)
{
	HERROR				 hErr = ERR_FAIL;

	if (NULL == pstInst)					{ return ERR_FAIL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_cflt_base_CheckConflictUsingTimeLine(pstInst);
		break;
	}

	return hErr;
}

HERROR samalib_cflt_MakeConflictTimeLine (sama_ConflictCheck_t *pstInst, HBOOL bOmitOneSch, HUINT32 ulSlotToOmit)
{
	HERROR				 hErr = ERR_FAIL;

	if (NULL == pstInst)					{ return ERR_FAIL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_cflt_base_MakeConflictTimeLine(pstInst, bOmitOneSch, ulSlotToOmit);
		break;
	}

	return hErr;
}

sama_CnftTimeEvt_t *samalib_cflt_FindFirstRelatedTimeLineEvent (DxSchedule_Spec_e eSapiSpec, HxList_t *pstTimeLine, HUINT32 ulBaseTime)
{
	sama_CnftTimeEvt_t	*pstTimeEvt = NULL;

	if (NULL == pstTimeLine)					{ return NULL; }

	switch (eSapiSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		pstTimeEvt = samalib_cflt_base_FindFirstRelatedTimeLineEvent(pstTimeLine, ulBaseTime);
		break;
	}

	return pstTimeEvt;
}

HERROR samalib_cflt_TakeResource (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule)
{
	HERROR				 hErr = ERR_FAIL;

	if (NULL == pstInst)					{ return ERR_FAIL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_cflt_base_TakeResource(pstInst, pstAction, pstSchedule);
		break;
	}

	return hErr;
}

HERROR samalib_cflt_TakeResource2 (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule, HxList_t *pstRemovableActList)
{
	HERROR				 hErr = ERR_FAIL;

	if (NULL == pstInst)					{ return ERR_FAIL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_cflt_base_TakeResource2(pstInst, pstAction, pstSchedule, pstRemovableActList);
		break;
	}

	return hErr;
}

HERROR samalib_cflt_GetTpInfo (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule, DxTransponder_t *pstTransponder)
{
	HERROR				 hErr = ERR_FAIL;

	if (NULL == pstInst)					{ return ERR_FAIL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		if(samalib_cflt_PossibleToUseSvcUidForTpInfo(pstSchedule) == FALSE)
		{
			hErr = samalib_cflt_base_GetTpInfoUsingSchedule(pstInst, pstSchedule, pstTransponder);
		}
		else
		{
			hErr = samalib_cflt_base_GetTpInfoUsingSvcUid(pstInst, pstSchedule, pstTransponder);
		}
		break;
	}

	return hErr;
}

HxList_t *samalib_cflt_GetConflictedActionList (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule)
{
	HxList_t			*pstResult = NULL;

	if (NULL == pstInst)					{ return NULL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		pstResult = samalib_cflt_base_GetConflictedActionList(pstInst, pstAction, pstSchedule);
		break;
	}

	return pstResult;
}

sama_ActionItem_t *samalib_cflt_TryStartAction (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule, sama_ActionState_e eState, SAPI_Conflict_t *pstConflict)
{
	sama_ActionItem_t			*pstResult = NULL;

	if (NULL == pstInst)					{ return NULL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		pstResult = samalib_cflt_base_TryStartAction(pstInst, pstSchedule, eState, pstConflict);
		break;
	}

	return pstResult;
}

HERROR		samalib_cflt_TryResource (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pstInst)					{ return ERR_FAIL; }

	switch (pstInst->stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = samalib_cflt_base_TryResource(pstInst, pstAction, pstSchedule);
		break;
	}

	return hErr;
}