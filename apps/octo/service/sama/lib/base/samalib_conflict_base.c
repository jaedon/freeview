/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_conflict_base.c
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

#include <dlib.h>
#include <rlib.h>
#include <sama_lib.h>
#include "../local_include/sama_lib_int.h"

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
#ifdef CONFIG_DEBUG
STATIC void samalib_cflt_base_PrintPadRemovalList (sama_ConflictCheck_t *pstInst)
{
	HxList_t				*pstListItem = NULL;
	sama_PadRemoval_t		*pstPadRemoval;

	HxLOG_Trace();

	for (pstListItem = pstInst->pstPadRemoveInfoList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstPadRemoval = (sama_PadRemoval_t *)HLIB_LIST_Data(pstListItem);
		if (NULL != pstPadRemoval)
		{
			HxLOG_Print("ulSlot: %d, pstPadRemoval->ePadRemoval: %d \n", pstPadRemoval->ulSlot, pstPadRemoval->ePadRemoval);
		}
	}


}
#endif


STATIC HERROR samalib_cflt_base_appendScheduleToTimeLine (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule)
{
	HUINT32					 ulCount;
	UNIXTIME				 utCurrTime = 0, utStartTime, utEndTime, utNextTime;
	UNIXTIME				 utPadStartTime, utPadEndTime;
	sama_CnftTimeEvt_t		*pstTimeEvt;
	HERROR					 hErr;

	HxLOG_Trace();

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (utCurrTime < SAMA_TIME_AVAIL_MIN)
	{
		HxLOG_Error("HLIB_STD_GetSystemTime err:\n");
		return ERR_FAIL;
	}

	hErr = SAMALIB_TIME_GetRepeatedScheduleStartTime(pstSchedule, utCurrTime, &utStartTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAMALIB_TIME_GetRepeatedScheduleStartTime err:\n");
		return ERR_FAIL;
	}

	for (ulCount = 0; ulCount < 16; ulCount++)				// 무한 Repeat를 막기 위한 항목임.  보통은 밑단에서 NextTime이 없어서 한번만 실행됨.
	{
		hErr = SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utStartTime, &utPadStartTime, &utPadEndTime);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SAMALIB_TIME_GetPaddingStartEndTime err:\n");

			utPadStartTime = utStartTime;
			utPadEndTime = utStartTime + pstSchedule->ulDuration;
		}

		utEndTime = utStartTime + pstSchedule->ulDuration;

		// Padding Start Time:
		if (utPadStartTime < utStartTime)
		{
			pstTimeEvt = (sama_CnftTimeEvt_t *)SAMA_Calloc(sizeof(sama_CnftTimeEvt_t));
			if (NULL != pstTimeEvt)
			{
				pstTimeEvt->eTimerType		= eSAPI_TIMER_TYPE_TL_PaddingStart;
				pstTimeEvt->utTime			= utPadStartTime;
				pstTimeEvt->pstSchedule 	= pstSchedule;
				pstTimeEvt->bActionRelated	= TRUE;

				pstInst->pstCfltTimeLine = HLIB_LIST_AppendSortEx(pstInst->pstCfltTimeLine, (void *)pstTimeEvt, samalib_cflt_cbCmpCfltTimeEvtByTime);
			}
		}

		// Start Time
		pstTimeEvt = (sama_CnftTimeEvt_t *)SAMA_Calloc(sizeof(sama_CnftTimeEvt_t));
		if (NULL != pstTimeEvt)
		{
			pstTimeEvt->eTimerType	= eSAPI_TIMER_TYPE_TL_Start;
			pstTimeEvt->utTime		= utStartTime;
			pstTimeEvt->pstSchedule = pstSchedule;
			pstTimeEvt->bActionRelated	= (utPadStartTime < utStartTime) ? FALSE : TRUE;

			pstInst->pstCfltTimeLine = HLIB_LIST_AppendSortEx(pstInst->pstCfltTimeLine, (void *)pstTimeEvt, samalib_cflt_cbCmpCfltTimeEvtByTime);
		}

		// End Time
		pstTimeEvt = (sama_CnftTimeEvt_t *)SAMA_Calloc(sizeof(sama_CnftTimeEvt_t));
		if (NULL != pstTimeEvt)
		{
			pstTimeEvt->eTimerType		= eSAPI_TIMER_TYPE_TL_Stop;
			pstTimeEvt->utTime			= utEndTime;
			pstTimeEvt->pstSchedule 	= pstSchedule;
			pstTimeEvt->bActionRelated	= (utEndTime < utPadEndTime) ? FALSE : TRUE;

			pstInst->pstCfltTimeLine = HLIB_LIST_AppendSortEx(pstInst->pstCfltTimeLine, (void *)pstTimeEvt, samalib_cflt_cbCmpCfltTimeEvtByTime);
		}

		// Padding End Time
		if (utEndTime < utPadEndTime)
		{
			pstTimeEvt = (sama_CnftTimeEvt_t *)SAMA_Calloc(sizeof(sama_CnftTimeEvt_t));
			if (NULL != pstTimeEvt)
			{
				pstTimeEvt->eTimerType		= eSAPI_TIMER_TYPE_TL_PaddingEnd;
				pstTimeEvt->utTime			= utPadEndTime;
				pstTimeEvt->pstSchedule 	= pstSchedule;
				pstTimeEvt->bActionRelated	= TRUE;

				pstInst->pstCfltTimeLine = HLIB_LIST_AppendSortEx(pstInst->pstCfltTimeLine, (void *)pstTimeEvt, samalib_cflt_cbCmpCfltTimeEvtByTime);
			}
		}

		utNextTime = utStartTime;
		if((pstSchedule->eRepeat != DxRSVREPEAT_ONCE) && (pstSchedule->eRepeat != DxRSVREPEAT_EndOfRepeatCase))
		{
			hErr = SAMALIB_TIME_GetNextTime(pstSchedule, utStartTime, &utNextTime);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SAMALIB_TIME_GetNextTime err:\n");
			}
		}

		if (utStartTime >= utNextTime)
		{
			break;
		}
		else
		{
			utStartTime = utNextTime;
		}
	}

	return ERR_OK;
}


STATIC HBOOL samalib_cflt_base_CheckPadRemovalListItemAlreadyExist (sama_ConflictCheck_t *pstInst, HUINT32 ulSlot, HUINT32 ePadRemoval)
{
	HxList_t				*pstListItem = NULL;
	sama_PadRemoval_t		*pstPadRemoval;

	HxLOG_Trace();

	for (pstListItem = pstInst->pstPadRemoveInfoList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstPadRemoval = (sama_PadRemoval_t *)HLIB_LIST_Data(pstListItem);
		if (NULL != pstPadRemoval)
		{
			if(pstPadRemoval->ulSlot == ulSlot && pstPadRemoval->ePadRemoval == ePadRemoval)
			{
				return TRUE;
			}
		}
	}

	return FALSE;

}


// 임시 계산을 위해서만 부른다. 즉, 계산하고 싶은 schedule의 복사본을 넣어준다.
// 실제 padding 제거는 SAMA_SCHEDULE_RemovePadding 에서 해야함.
STATIC void samalib_cflt_base_RemovePaddingTemporary (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule)
{
	HxList_t				*pstListItem = NULL;
	sama_PadRemoval_t		*pstPadRemoval;

	HxLOG_Trace();

	for (pstListItem = pstInst->pstPadRemoveInfoList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstPadRemoval = (sama_PadRemoval_t *)HLIB_LIST_Data(pstListItem);
		if (NULL != pstPadRemoval)
		{
			if(pstPadRemoval->ulSlot == pstSchedule->ulSlot)
			{
				if ((pstPadRemoval->ePadRemoval & eSAPI_PADDINGRMV_StartPad) != 0)
				{
					HxLOG_Debug("slot: %d, remove start pad \n", pstSchedule->ulSlot);
					pstSchedule->uExtInfo.stRec.ulPreOffsetTime = 0;
				}

				if ((pstPadRemoval->ePadRemoval & eSAPI_PADDINGRMV_StopPad) != 0)
				{
					HxLOG_Debug("slot: %d, remove end pad \n", pstSchedule->ulSlot);
					pstSchedule->uExtInfo.stRec.ulPostOffsetTime = 0;
				}

			}
		}
	}

	return;

}

STATIC HBOOL samalib_cflt_base_CheckSameServiceOverlappedTime (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstTriedSchedule, HUINT32 *pPadOverlappedSlot, sama_PaddingRemoval_e *pePadRemoval, HBOOL *pbIsBodyOverlapped, HUINT32 *pBodyOverlappedSlot)
{
	HxList_t			*pstFoundList = NULL;
	DxSchedule_FindFilterAttr_t *pstFilter = NULL;
	HxList_t			*pstListItem = NULL;
	DxSchedule_t		*pstSameServiceSchedule = NULL;
	UNIXTIME			 utTriedSchStart, utSameServiceSchStart;
	UNIXTIME			 utTriedSchEnd, utSameServiceSchEnd;
	UNIXTIME			 utTriedPadStart, utSameServicePadStart;
	UNIXTIME			 utTriedPadEnd, utSameServicePadEnd;
	HBOOL			bIsOverlapped = FALSE;
	SAPI_ActionType_e		eActType;
	DxSchedule_t		stSameServiceScheduleBackup;
	DxSchedule_t		stTriedScheduleBackup;

	HxLOG_Trace();

	if(pstTriedSchedule == NULL)
	{
		HxLOG_Debug("there is no new schedule \n");
		goto END_FUNC;
	}

	SAMA_MemCpy(&stTriedScheduleBackup, pstTriedSchedule, sizeof(DxSchedule_t));

	eActType = samalib_cflt_base_getSapiActionTypeBySchedule(&stTriedScheduleBackup);
	HxLOG_Debug("slot: %d, eRsvType: 0x%x, eActType: %d \n", stTriedScheduleBackup.ulSlot, stTriedScheduleBackup.eRsvType, eActType);
	if(eActType != eSAPI_ACTION_TYPE_Record) 	// 일단 record만...
	{
		HxLOG_Debug("do not check eActType: %d \n", eActType);
		goto END_FUNC;
	}

	pstFilter = (DxSchedule_FindFilterAttr_t*)SAMA_Calloc(sizeof(DxSchedule_FindFilterAttr_t));
	if (pstFilter == NULL)
	{
		HxLOG_Error("alloc fail \n");
		goto END_FUNC;
	}

	SAMALIB_CFLT_FindScheduleListFilterInit(pstFilter);
	SAMA_MemCpy(&(pstFilter->stTripleId), &(stTriedScheduleBackup.stTripleId), sizeof(DxSchedule_TripleId_t));
	pstFilter->ulSvcUid = stTriedScheduleBackup.ulSvcUid;

	(void)SAMALIB_CFLT_FindScheduleListUsingFilter(pstInst->pstScheduleList, pstFilter, &pstFoundList);
	if(pstFoundList == NULL)
	{
		HxLOG_Debug("there is no schedule list satisfing filter \n");
		goto END_FUNC;
	}

	HLIB_DATETIME_ConvertDateTimeToUnixTime (&(stTriedScheduleBackup.stStartTime), &utTriedSchStart);
	samalib_cflt_base_RemovePaddingTemporary(pstInst, &stTriedScheduleBackup);
	SAMALIB_TIME_GetPaddingStartEndTime(&stTriedScheduleBackup, utTriedSchStart, &utTriedPadStart, &utTriedPadEnd);
	utTriedSchEnd = utTriedSchStart + stTriedScheduleBackup.ulDuration;
	/*          |-----start Pad-----|-----Body-----|-----end Pad-----|                    */
	/*  TriedPadStart             TriedSchStart         TriedSchEnd              TriedPadEnd            */

	// list item이 여러개더라도, 어차피 1개만 check 하면 됨.
	for (pstListItem = pstFoundList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstSameServiceSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

		if (pstSameServiceSchedule == NULL)
		{
			HxLOG_Error("Null list item. \n");
			continue;
		}

		SAMA_MemCpy(&stSameServiceScheduleBackup, pstSameServiceSchedule, sizeof(DxSchedule_t));

		HxLOG_Debug("check, slot: %d, slot: %d\n", stTriedScheduleBackup.ulSlot, stSameServiceScheduleBackup.ulSlot);

		if(stTriedScheduleBackup.ulSlot == stSameServiceScheduleBackup.ulSlot)
		{
			HxLOG_Debug("do not check the same schedule.\n");
			continue;
		}

		HxLOG_Debug("slot: %d, eRsvType: 0x%x, eActType: %d \n", stSameServiceScheduleBackup.ulSlot, stSameServiceScheduleBackup.eRsvType, eActType);
		eActType = samalib_cflt_base_getSapiActionTypeBySchedule(&stSameServiceScheduleBackup);
		if(eActType != eSAPI_ACTION_TYPE_Record) 	// 일단 record만...
		{
			HxLOG_Debug("do not check eActType: %d, skip \n", eActType);
			continue;
		}

		HLIB_DATETIME_ConvertDateTimeToUnixTime (&(stSameServiceScheduleBackup.stStartTime), &utSameServiceSchStart);
		samalib_cflt_base_RemovePaddingTemporary(pstInst, &stSameServiceScheduleBackup);
		SAMALIB_TIME_GetPaddingStartEndTime(&stSameServiceScheduleBackup, utSameServiceSchStart, &utSameServicePadStart, &utSameServicePadEnd);
		utSameServiceSchEnd = utSameServiceSchStart + stSameServiceScheduleBackup.ulDuration;
		/*            |-----start Pad-----|-----Body-----|-----end Pad-----|            */
		/*    ListPadStart               ListSchStart            ListSchEnd                ListPadEnd      */

		*pbIsBodyOverlapped = HLIB_DATETIME_IsTimeOverwrapped(utTriedSchStart, utTriedSchEnd, utSameServiceSchStart, utSameServiceSchEnd);
		if(*pbIsBodyOverlapped == TRUE)
		{
			HxLOG_Debug("*pbIsBodyOverlapped is TRUE!, there is no reason to check the padding remove!, Tried slot: %d, List slot: %d\n", stTriedScheduleBackup.ulSlot, stSameServiceScheduleBackup.ulSlot);
			*pBodyOverlappedSlot = stSameServiceScheduleBackup.ulSlot;
			goto END_FUNC;
		}


		if(utTriedPadEnd < utSameServicePadStart)
		{
			HxLOG_Debug("utTriedPadEnd < utSameServicePadStart, skip \n");
			continue;
		}

		HxLOG_Debug("new Schedule start, end time \n");
		HxLOG_Debug("Tried Pad Start %s \n",HLIB_DATETIME_UnixTimeToString(utTriedPadStart,NULL, 0));
		HxLOG_Debug("Tried Pad   End %s \n",HLIB_DATETIME_UnixTimeToString(utTriedPadEnd,NULL, 0));
		HxLOG_Debug("existing Schedule start, end time \n");
		HxLOG_Debug("SameService Pad Start %s \n",HLIB_DATETIME_UnixTimeToString(utSameServicePadStart,NULL, 0));
		HxLOG_Debug("SameService Pad   End %s \n",HLIB_DATETIME_UnixTimeToString(utSameServicePadEnd,NULL, 0));


		bIsOverlapped = HLIB_DATETIME_IsTimeOverwrapped(utTriedPadStart, utTriedPadEnd, utSameServicePadStart, utSameServicePadEnd);
		if(bIsOverlapped == TRUE)
		{
			HxLOG_Debug("Time overlapped, Tried's slot: %d, List's slot: %d\n", stTriedScheduleBackup.ulSlot, stSameServiceScheduleBackup.ulSlot);

			*pPadOverlappedSlot = stSameServiceScheduleBackup.ulSlot;

			if(utTriedPadStart >= utSameServicePadStart && utSameServicePadEnd >= utTriedPadEnd)
			{
				//포함
				// [<---Same Service---[<----Tried---->]---Same Service--->]
				if(HLIB_DATETIME_IsTimeOverwrapped(utTriedPadStart, utTriedPadEnd, utSameServiceSchStart, utSameServicePadEnd)==FALSE)
				{
					// NOT overlapped after remove list's StartPad (utSameServiceSchStart)
					*pPadOverlappedSlot = stSameServiceScheduleBackup.ulSlot;
					*pePadRemoval = eSAPI_PADDINGRMV_StartPad;
				}
				else if(HLIB_DATETIME_IsTimeOverwrapped(utTriedPadStart, utTriedPadEnd, utSameServicePadStart, utSameServiceSchEnd)==FALSE)
				{
					// NOT overlapped after remove list's EndPad (utSameServiceSchEnd)
					*pPadOverlappedSlot = stSameServiceScheduleBackup.ulSlot;
					*pePadRemoval = eSAPI_PADDINGRMV_StopPad;
				}
			}
			if(utSameServicePadStart >= utTriedPadStart && utTriedPadEnd >= utSameServicePadEnd)
			{
				// [<---Tried---[<----Same Service---->]---Tried--->]
				if(HLIB_DATETIME_IsTimeOverwrapped(utTriedSchStart, utTriedPadEnd, utSameServicePadStart, utSameServicePadEnd)==FALSE)
				{
					// NOT overlapped after remove Tried's StartPad (utTriedSchStart)
					*pPadOverlappedSlot = stTriedScheduleBackup.ulSlot;
					*pePadRemoval = eSAPI_PADDINGRMV_StartPad;
				}
				else if(HLIB_DATETIME_IsTimeOverwrapped(utTriedPadStart, utTriedSchEnd, utSameServicePadStart, utSameServicePadEnd)==FALSE)
				{
					// NOT overlapped after remove Tried's EndPad (utTriedSchEnd)
					*pPadOverlappedSlot = stTriedScheduleBackup.ulSlot;
					*pePadRemoval = eSAPI_PADDINGRMV_StopPad;
				}
			}
			if(utSameServicePadStart >= utTriedPadStart && utSameServicePadEnd >= utTriedPadEnd && utTriedPadEnd >= utSameServicePadStart)
			{
				// [<---Tried-------------------->]
				//                 [<----Same Service--------------->]
				HBOOL	bIsExist = FALSE;

				bIsExist = samalib_cflt_base_CheckPadRemovalListItemAlreadyExist(pstInst, stSameServiceScheduleBackup.ulSlot, eSAPI_PADDINGRMV_StartPad);

				// pstSameServiceSchedule 의 앞과 겹침.
				if(bIsExist)
				{
					*pPadOverlappedSlot = stTriedScheduleBackup.ulSlot;
					*pePadRemoval = eSAPI_PADDINGRMV_StopPad;
				}
				else
				{
					*pPadOverlappedSlot = stSameServiceScheduleBackup.ulSlot;
					*pePadRemoval = eSAPI_PADDINGRMV_StartPad;
				}
			}
			if(utTriedPadStart >= utSameServicePadStart && utTriedPadEnd >= utSameServicePadEnd && utSameServicePadEnd >= utTriedPadStart)
			{
				// [<---Same Service---------->]
				//                 [<----Tried--------------->]
				HBOOL	bIsExist = FALSE;

				bIsExist = samalib_cflt_base_CheckPadRemovalListItemAlreadyExist(pstInst, stTriedScheduleBackup.ulSlot, eSAPI_PADDINGRMV_StartPad);

				// pstSameServiceSchedule 의 뒤와 겹칩.
				if(bIsExist)
				{
					*pPadOverlappedSlot = stSameServiceScheduleBackup.ulSlot;
					*pePadRemoval = eSAPI_PADDINGRMV_StopPad;
				}
				else
				{
					*pPadOverlappedSlot = stTriedScheduleBackup.ulSlot;
					*pePadRemoval = eSAPI_PADDINGRMV_StartPad;
				}
			}

			*pBodyOverlappedSlot = stSameServiceScheduleBackup.ulSlot;

			{
				HBOOL	bIsExist = FALSE;

				bIsExist = samalib_cflt_base_CheckPadRemovalListItemAlreadyExist(pstInst, *pPadOverlappedSlot, *pePadRemoval);
				if(bIsExist != TRUE)
				{
					break;
				}
				else
				{
					bIsOverlapped = FALSE;
				}
			}

		}
	}


END_FUNC:
	if(pstFilter != NULL)
	{
		SAMA_Free(pstFilter);
	}

	if(pstFoundList != NULL)
	{
		HLIB_LIST_RemoveAll(pstFoundList);
	}

	return bIsOverlapped;
}


STATIC HERROR samalib_cflt_base_appendRemovablePadding(sama_ConflictCheck_t *pstInst, HUINT32 ulSlot,sama_PaddingRemoval_e ePad)
{
	sama_PadRemoval_t		*pstPadRemoval = NULL;
	HBOOL					bSamePadExists = FALSE;

	bSamePadExists = samalib_cflt_base_CheckPadRemovalListItemAlreadyExist(pstInst,ulSlot,ePad);
	if (bSamePadExists == FALSE)
	{
		HxLOG_Debug("Add Removable padding. slot(%d) pad(%d) \n",ulSlot, ePad);
		pstPadRemoval = (sama_PadRemoval_t *)SAMA_Malloc(sizeof(sama_PadRemoval_t));
		if (NULL != pstPadRemoval)
		{
			pstPadRemoval->ulSlot = ulSlot;
			pstPadRemoval->ePadRemoval = ePad;
			pstInst->pstPadRemoveInfoList = HLIB_LIST_Append(pstInst->pstPadRemoveInfoList, (void *)pstPadRemoval);
		}
	}
	else
	{
		HxLOG_Debug("Removable padding exists. slot(%d) pad(%d) \n",ulSlot, ePad);
	}
#ifdef CONFIG_DEBUG
	samalib_cflt_base_PrintPadRemovalList(pstInst);
#endif
	return ERR_OK;
}

STATIC HBOOL samalib_cflt_base_IsTpTunable (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule)
{
	sama_ActionItem_t	*pstAction = NULL;
	HERROR				hErr;
	HBOOL				bTpTunable = FALSE;

	// 1. Take action
	pstAction = samalib_cflt_TakeAction(pstInst, 0);
	if (pstAction == NULL)
	{
		HxLOG_Debug("Fail to take action. \n");
		goto END_FUNC;
	}

	hErr = samalib_cflt_TryResource(pstInst, pstAction, pstSchedule);
	if (hErr != ERR_OK)
	{
		HxLOG_Debug("Fail to try resource. \n");
		goto END_FUNC;
	}

	bTpTunable = TRUE;

END_FUNC:
	if (pstAction != NULL)
	{
		samalib_cflt_ReleaseAction(pstInst, pstAction);
	}

	return bTpTunable;
}
#define ___MEMBER_FUNCTIONS___
/*******************************************************************/
/********************      Member Functions       ******************/
/*******************************************************************/
SAPI_ActionType_e samalib_cflt_base_getSapiActionTypeBySchedule (DxSchedule_t *pstSchedule)
{
	if (NULL != pstSchedule)
	{
		switch (pstSchedule->eRsvType)
		{
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_TBR:
		case DxRSVTYPE_WATCHING_SERIES_EBR:
			return eSAPI_ACTION_TYPE_LiveView;

		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_INSTANT:
		case DxRSVTYPE_RECORD_DELAYED:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
			return eSAPI_ACTION_TYPE_Record;

		case DxRSVTYPE_NETSTREAMING_LIVE:
		case DxRSVTYPE_NETSTREAMING_FILE:
			return eSAPI_ACTION_TYPE_NetStreaming;

		default:
			break;
		}
	}

	return eSAPI_ACTION_TYPE_None;
}

HERROR samalib_cflt_base_CheckStreamingConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule)
{
	UNIXTIME				 utCurrTime;
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Trace();

	if ((NULL == pstInst) || (NULL == pstSchedule))
	{
		HxLOG_Error("(NULL == pstInst) || (NULL == pstSchedule), return ERR_FAIL \n");
		return ERR_FAIL;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (utCurrTime < SAMA_TIME_AVAIL_MIN)
	{
		HxLOG_Error("HLIB_STD_GetSystemTime err:\n");
		return ERR_FAIL;
	}


	// homma 에서도 action count 에 대한 check 를 하지만, 여기서도 일단 해둔다. (MGR_SCHEDULE_Init 에서 max streaming 수를 일단 2개로 해두어서 가능)
	switch(pstSchedule->eRsvType)
	{
		case DxRSVTYPE_NETSTREAMING_LIVE:
		case DxRSVTYPE_NETSTREAMING_FILE:
			hErr = samalib_cflt_base_CheckRecConflictWithSchedule(pstInst, pstSchedule);

			if(pstInst->stConflictResult.eType == eSAPI_CONFLICT_TYPE_None)
			{
				HxLOG_Debug("pstInst->stConflictResult.eType is none. check with live. \n");
				hErr = samalib_cflt_base_CheckLiveConflictWithSchedule(pstInst, pstSchedule);
#if 0	 // DxRSVTYPE_NETSTREAMING_FILE 일 경우만...??
				// 임시.
				if(pstInst->stConflictResult.eType != eSAPI_CONFLICT_TYPE_None)
				{
					pstInst->stConflictResult.eType = eSAPI_CONFLICT_TYPE_Tp;
				}
#endif
			}
			break;

		default:
			hErr = ERR_FAIL;
			break;
	}

	return hErr;

}

HERROR samalib_cflt_base_CheckLiveConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule)
{
	UNIXTIME				 utCurrTime, utStartTime, utPadStartTime, utPadEndTime;
	HxList_t				*pstListItem;
	HERROR					 hErr;

	HxLOG_Trace();

	if ((NULL == pstInst) || (NULL == pstSchedule))
	{
		HxLOG_Error("(NULL == pstInst) || (NULL == pstSchedule), return ERR_FAIL \n");
		return ERR_FAIL;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (utCurrTime < SAMA_TIME_AVAIL_MIN)
	{
		HxLOG_Error("HLIB_STD_GetSystemTime err:\n");
		return ERR_FAIL;
	}

	hErr = SAMALIB_TIME_GetRepeatedScheduleStartTime(pstSchedule, utCurrTime, &utStartTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAMALIB_TIME_GetRepeatedScheduleStartTime err:\n");
		return ERR_FAIL;
	}

	hErr = SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utStartTime, &utPadStartTime, &utPadEndTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAMALIB_TIME_GetPaddingStartEndTime err:\n");
		return ERR_FAIL;
	}

	for (pstListItem = pstInst->pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		UNIXTIME		 utSchPadStartTime, utSchPadEndTime;
		DxSchedule_t	*pstSrcSched = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstSrcSched)
		{
			if (pstSrcSched->ulSlot == pstSchedule->ulSlot)
			{
				continue;
			}

			switch (pstSrcSched->eRsvType)
			{
			case DxRSVTYPE_WATCHING_EBR:
			case DxRSVTYPE_WATCHING_TBR:
			case DxRSVTYPE_WATCHING_SERIES_EBR:
			// watching 과 HLS 를 비교할 수가 없다.  정확한 비교는 tune 가능한가를 봐야 하는 것이고, 그것조차 힘들다.
			// 왜냐하면 HLS가 1분 duration을 가지고 계속 update 되는데... watching 의 시간에 겹치는 것을 알기 힘들어...
			// 대신 HLS에 대한(HLS 끼리의) action 갯수에 대한 check 는 samalib_cflt_base_TryStartAction에서 하고 있다.
			//case DxRSVTYPE_NETSTREAMING_LIVE:
				hErr = SAMALIB_TIME_GetRepeatedScheduleStartTime(pstSrcSched, utCurrTime, &utStartTime);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("SAMALIB_TIME_GetRepeatedScheduleStartTime err:\n");
					return ERR_FAIL;
				}

				hErr = SAMALIB_TIME_GetPaddingStartEndTime(pstSrcSched, utStartTime, &utSchPadStartTime, &utSchPadEndTime);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("SAMALIB_TIME_GetPaddingStartEndTime err:\n");
					return ERR_FAIL;
				}

				if (utSchPadStartTime == utPadStartTime)
				{
					SAPI_ConflictItem_t		 stConflictItem;

					pstInst->stConflictResult.eType = eSAPI_CONFLICT_TYPE_View;

					stConflictItem.ulSlot = pstSrcSched->ulSlot;
					(void)SAMALIB_CFLT_AppendConflictResult(&(pstInst->stConflictResult), &stConflictItem);
				}

				break;

			default:
				break;
			}
		}
	}

	return ERR_OK;
}




HERROR samalib_cflt_base_CheckRecConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule)
{

	HUINT32					 ulCount;
	UNIXTIME				 utCurrTime = 0;
	UNIXTIME				 utStartTime, utEndTime, utNextTime, utPadStartTime, utPadEndTime;
	HERROR					 hErr;

	HxLOG_Trace();

	if ((NULL == pstInst) || (NULL == pstSchedule))
	{
		HxLOG_Error("(NULL == pstInst) || (NULL == pstSchedule), return ERR_FAIL \n");
		return ERR_FAIL;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (utCurrTime < SAMA_TIME_AVAIL_MIN)
	{
		HxLOG_Error("HLIB_STD_GetSystemTime err:\n");
		return ERR_FAIL;
	}

	(void)samalib_cflt_MakeConflictTimeLine (pstInst, pstInst->bScheduleChange, pstSchedule->ulSlot);

	if (NULL == pstInst->pstCfltTimeLine)
	{
		// 검사할 타임라인이 없다 : 충돌할 가능성도 없다:
		HxLOG_Info ("Nothing to check in the timeline: No confliction  slot[%d]\n" , pstSchedule->ulSlot);
		return ERR_OK;
	}

	// StartTime 관련으로 만든다.

	hErr = SAMALIB_TIME_GetRepeatedScheduleStartTime(pstSchedule, utCurrTime, &utStartTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAMALIB_TIME_GetRepeatedScheduleStartTime err:\n");
		return ERR_FAIL;
	}

	for (ulCount = 0; ulCount < 16; ulCount++)				// 무한 Repeat를 막기 위한 항목임.  보통은 밑단에서 NextTime이 없어서 한번만 실행됨.
	{
		HBOOL					 bMemAllocFailed = FALSE;
		sama_CnftTimeEvt_t		*pstPadStartTimeEvt = NULL, *pstPadEndTimeEvt = NULL;
		sama_CnftTimeEvt_t		*pstStartTimeEvt = NULL, *pstEndTimeEvt = NULL;

		hErr = SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utStartTime, &utPadStartTime, &utPadEndTime);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SAMALIB_TIME_GetPaddingStartEndTime err:\n");

			utPadStartTime = utStartTime;
			utPadEndTime = utStartTime + pstSchedule->ulDuration;
		}

		utEndTime = utStartTime + pstSchedule->ulDuration;

		// Padding Start Time
		if (utPadStartTime < utStartTime)
		{
			pstPadStartTimeEvt = (sama_CnftTimeEvt_t *)SAMA_Calloc(sizeof(sama_CnftTimeEvt_t));
			if (NULL != pstPadStartTimeEvt)
			{
				pstPadStartTimeEvt->eTimerType 	= eSAPI_TIMER_TYPE_TL_PaddingStart;
				pstPadStartTimeEvt->utTime 		= utPadStartTime;
				pstPadStartTimeEvt->pstSchedule	= pstSchedule;
			}
			else
			{
				bMemAllocFailed = TRUE;
			}
		}

		// Start Time
		pstStartTimeEvt = (sama_CnftTimeEvt_t *)SAMA_Calloc(sizeof(sama_CnftTimeEvt_t));
		if (NULL != pstStartTimeEvt)
		{
			pstStartTimeEvt->eTimerType 		= eSAPI_TIMER_TYPE_TL_Start;
			pstStartTimeEvt->utTime 			= utStartTime;
			pstStartTimeEvt->pstSchedule		= pstSchedule;
		}
		else
		{
			bMemAllocFailed = TRUE;
		}

		// End Time
		pstEndTimeEvt = (sama_CnftTimeEvt_t *)SAMA_Calloc(sizeof(sama_CnftTimeEvt_t));
		if (NULL != pstEndTimeEvt)
		{
			pstEndTimeEvt->eTimerType 			= eSAPI_TIMER_TYPE_TL_Stop;
			pstEndTimeEvt->utTime 				= utEndTime;
			pstEndTimeEvt->pstSchedule			= pstSchedule;
		}
		else
		{
			bMemAllocFailed = TRUE;
		}

		// Padding End Time
		if (utEndTime < utPadEndTime)
		{
			pstPadEndTimeEvt = (sama_CnftTimeEvt_t *)SAMA_Calloc(sizeof(sama_CnftTimeEvt_t));
			if (NULL != pstPadEndTimeEvt)
			{
				pstPadEndTimeEvt->eTimerType 	= eSAPI_TIMER_TYPE_TL_PaddingEnd;
				pstPadEndTimeEvt->utTime 		= utPadEndTime;
				pstPadEndTimeEvt->pstSchedule	= pstSchedule;
			}
			else
			{
				bMemAllocFailed = TRUE;
			}
		}

		if (TRUE != bMemAllocFailed)
		{


			if (NULL != pstPadStartTimeEvt)
			{
				pstInst->pstCfltTimeLine = HLIB_LIST_AppendSortEx(pstInst->pstCfltTimeLine, (void *)pstPadStartTimeEvt, samalib_cflt_cbCmpCfltTimeEvtByTime);
			}

			pstInst->pstCfltTimeLine = HLIB_LIST_AppendSortEx(pstInst->pstCfltTimeLine, (void *)pstStartTimeEvt, samalib_cflt_cbCmpCfltTimeEvtByTime);
			pstInst->pstCfltTimeLine = HLIB_LIST_AppendSortEx(pstInst->pstCfltTimeLine, (void *)pstEndTimeEvt, samalib_cflt_cbCmpCfltTimeEvtByTime);

			if (NULL != pstPadEndTimeEvt)
			{
				pstInst->pstCfltTimeLine = HLIB_LIST_AppendSortEx(pstInst->pstCfltTimeLine, (void *)pstPadEndTimeEvt, samalib_cflt_cbCmpCfltTimeEvtByTime);
			}

			pstInst->pstCheckStartEvt = (NULL != pstPadStartTimeEvt) ? pstPadStartTimeEvt : pstStartTimeEvt;
			pstInst->pstCheckEndEvt = (NULL != pstPadEndTimeEvt) ? pstPadEndTimeEvt : pstEndTimeEvt;

			(void)samalib_cflt_CheckConflictUsingTimeLine(pstInst);
		}
		else
		{
			HxLOG_Critical("Memory allocation Failed !!!!\n\n");

			if (NULL != pstStartTimeEvt)			{ SAMA_Free(pstStartTimeEvt); }
			if (NULL != pstEndTimeEvt)				{ SAMA_Free(pstEndTimeEvt); }
			if (NULL != pstPadStartTimeEvt)			{ SAMA_Free(pstPadStartTimeEvt); }
			if (NULL != pstPadEndTimeEvt)			{ SAMA_Free(pstPadEndTimeEvt); }
		}

		utNextTime = utStartTime;

		(void)SAMALIB_TIME_GetNextTime(pstSchedule, utStartTime, &utNextTime);
		if (utStartTime >= utNextTime)
		{
			break;
		}
		else
		{
			utStartTime = utNextTime;
		}
	}

	return ERR_OK;
}

HERROR samalib_cflt_base_CheckConflictUsingTimeLine (sama_ConflictCheck_t *pstInst)
{
	HCHAR					*szEvtTimeStr = NULL;
	HxList_t				*pstListItem = NULL;
	sama_CnftTimeEvt_t		*pstTimeEvt = NULL;
	sama_ActionItem_t		*pstAction = NULL;
	SAPI_Conflict_t			 stConflict;
	HERROR					 hErr;
	sama_CnftTimeEvt_t *pstStartTimeEvt, *pstEndTimeEvt;

	if ((NULL == pstInst) || (NULL == pstInst->pstCheckStartEvt) || (NULL == pstInst->pstCheckEndEvt) )
	{
		return ERR_FAIL;
	}
	pstStartTimeEvt = pstInst->pstCheckStartEvt;
	pstEndTimeEvt = pstInst->pstCheckEndEvt;

	hErr = samalib_cflt_ClearActions(pstInst);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("samalib_cflt_ClearActions err:\n");
		return ERR_FAIL;
	}

	pstTimeEvt = samalib_cflt_FindFirstRelatedTimeLineEvent(pstInst->stSpecOption.eSpec, pstInst->pstCfltTimeLine, pstStartTimeEvt->utTime);
	if (NULL == pstTimeEvt)
	{
		HxLOG_Info("No Related Time Line Event: Do not worry about the conflict\n");
		return ERR_OK;
	}

	szEvtTimeStr = (HCHAR *)HLIB_STD_MemCalloc(128);

	// For Debug:
	if (NULL != szEvtTimeStr)
	{
		HxLOG_Debug("\n[SAMA_TIMELINE] TimeLine Check Start:\n");
	}

	SAMA_MemSet(&stConflict, 0, sizeof(SAPI_Conflict_t));
	SAMA_MemCpy(&(stConflict.stTriedSchedule), &(pstInst->stConflictResult.stTriedSchedule), sizeof(DxSchedule_t));

	pstListItem = HLIB_LIST_Find(pstInst->pstCfltTimeLine, (const void *)pstTimeEvt);
	for (; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		if (stConflict.eType != eSAPI_CONFLICT_TYPE_Tp)
		{
			// TODO: delete SAMALIB_CFLT_ClearConflictResult if no side-effect occurs
			(void)SAMALIB_CFLT_ClearConflictResult(&stConflict);
		}
		SAMA_MemCpy(&(stConflict.stTriedSchedule), &(pstInst->stConflictResult.stTriedSchedule), sizeof(DxSchedule_t));

		pstTimeEvt = (sama_CnftTimeEvt_t *)HLIB_LIST_Data (pstListItem);
		if (NULL != pstTimeEvt)
		{
			SAPI_ActionType_e	 eActType;
			DxSchedule_t		*pstSchedule = pstTimeEvt->pstSchedule;

			// SAMA_SCHEDULE_RemovePadding에서 자기 자신에 관계된 padding 만 없애도록 하고 여기는 막는다.
			// 추후 필요하면 연다. (보다 정교한 계산이 필요할 경우 대비해서... 근데 일단 막음)
#if 0//def SAMA_LAZY_PADDING_CONFLICT_REMOVE
			// SAMA_LAZY_PADDING_CONFLICT_REMOVE 에서는 padding 이 이제야 없어지므로...이 for문 안에서.
			// 정확한 계산을 위해.
			// 정확히 계산해야 딱 바로 뒤 예약의 start pad 까지만 없앨 수 있다.
			// 공통 적용해도 되겠지만, test 필요해서..
			// 추후 함수로 뺀다.
			UNIXTIME			utSchStartTime1;
			UNIXTIME			utSchStartTime2;
			UNIXTIME			utStartTime1, utStartTime2;
			UNIXTIME			utEndTime1, utEndTime2;
			UNIXTIME			utTime2;
			DxSchedule_t		stScheduleBackup1;
			DxSchedule_t		stScheduleBackup2;

			SAMA_MemCpy(&stScheduleBackup1, pstEndTimeEvt->pstSchedule, sizeof(DxSchedule_t));
			HLIB_DATETIME_ConvertDateTimeToUnixTime (&(stScheduleBackup1.stStartTime), &utSchStartTime1);
			samalib_cflt_base_RemovePaddingTemporary(pstInst, &stScheduleBackup1);
			SAMALIB_TIME_GetPaddingStartEndTime(&stScheduleBackup1, utSchStartTime1, &utStartTime1, &utEndTime1);

			SAMA_MemCpy(&stScheduleBackup2, pstTimeEvt->pstSchedule, sizeof(DxSchedule_t));
			HLIB_DATETIME_ConvertDateTimeToUnixTime (&(stScheduleBackup2.stStartTime), &utSchStartTime2);
			samalib_cflt_base_RemovePaddingTemporary(pstInst, &stScheduleBackup2);
			SAMALIB_TIME_GetPaddingStartEndTime(&stScheduleBackup2, utSchStartTime2, &utStartTime2, &utEndTime2);

#ifdef CONFIG_DEBUG
			HxLOG_Debug("pstEndTimeEvt start, end time \n");
			HLIB_DATETIME_PrintDateTime(utStartTime1);
			HLIB_DATETIME_PrintDateTime(utEndTime1);
			HxLOG_Debug("pstTimeEvt start, end time \n");
			HLIB_DATETIME_PrintDateTime(utStartTime2);
			HLIB_DATETIME_PrintDateTime(utEndTime2);
#endif

			switch(pstTimeEvt->eTimerType)
			{
				case eSAPI_TIMER_TYPE_TL_PaddingStart:
					utTime2 = utStartTime2;
					break;
				case eSAPI_TIMER_TYPE_TL_Start:
					utTime2 = utSchStartTime2;
					break;
				case eSAPI_TIMER_TYPE_TL_Stop:
					utTime2 = utSchStartTime2 + pstTimeEvt->pstSchedule->ulDuration;
					break;
				case eSAPI_TIMER_TYPE_TL_PaddingEnd:
					utTime2 = utEndTime2;
					break;
				default:
					utTime2 = utSchStartTime2;
					break;
			}

			HxLOG_Debug("pstTimeEvt->eTimerType:%d, utTime2  \n", pstTimeEvt->eTimerType);
			HLIB_DATETIME_PrintDateTime(utTime2);

			// Conflict time line에서 새로운 예약의 end time 이상으로 체크 할 필요 없음.
			if(utTime2 >= utEndTime1)
			{
				break;
			}
#else
			// Conflict time line에서 새로운 예약의 end time 이상으로 체크 할 필요 없음.
			if (pstTimeEvt->utTime > pstEndTimeEvt->utTime)
			{
				break;
			}
#endif

			eActType = samalib_cflt_base_getSapiActionTypeBySchedule(pstSchedule);
			if (eSAPI_ACTION_TYPE_None == eActType)					{ break; }

#if defined(CONFIG_DEBUG)
			// For Debug:
			if (NULL != szEvtTimeStr)
			{
				HxDATETIME_t			 stEvtTime;

				(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(pstTimeEvt->utTime, &stEvtTime);
				(void)HLIB_DATETIME_GetStringByDateTime((const HxDATETIME_t *)&stEvtTime, szEvtTimeStr, 128);

				HxLOG_Debug("[SAMA_TIMELINE] -- Event Time (%s)\n", szEvtTimeStr);
				switch (pstTimeEvt->eTimerType)
				{
				case eSAPI_TIMER_TYPE_TL_PaddingStart:
					HxLOG_Debug("[SAMA_TIMELINE]	 Schedule Slot(%d) Padding Started\n", pstSchedule->ulSlot);
					break;

				case eSAPI_TIMER_TYPE_TL_PaddingEnd:
					HxLOG_Debug("[SAMA_TIMELINE]	 Schedule Slot(%d) Padding Ended\n", pstSchedule->ulSlot);
					break;

				case eSAPI_TIMER_TYPE_TL_Start:
					HxLOG_Debug("[SAMA_TIMELINE]	 Schedule Slot(%d) Started\n", pstSchedule->ulSlot);
					break;

				case eSAPI_TIMER_TYPE_TL_Stop:
					HxLOG_Debug("[SAMA_TIMELINE]	 Schedule Slot(%d) Stopped\n", pstSchedule->ulSlot);
					break;

				default:
					HxLOG_Debug("[SAMA_TIMELINE]	 Schedule Slot(%d) Unknown Type(%d)\n", pstSchedule->ulSlot, pstTimeEvt->eTimerType);
					break;
				}
			}
#endif
			switch (pstTimeEvt->eTimerType)
			{
			case eSAPI_TIMER_TYPE_TL_PaddingStart:
				pstAction = samalib_cflt_TryStartAction(pstInst, pstSchedule, eSAPI_ACTIONSTATE_PaddingStart, &stConflict);
				if (NULL != pstAction)
				{
					pstAction->eActState = eSAPI_ACTIONSTATE_PaddingStart;
				}
				else
				{
					HxLOG_Debug("Schedule(%d) Failed to Start in the padding time: retry in the Start Time\n", pstSchedule->ulSlot);

					samalib_cflt_base_appendRemovablePadding(pstInst, pstSchedule->ulSlot, eSAPI_PADDINGRMV_StartPad);
				}

				break;

			case eSAPI_TIMER_TYPE_TL_Start:
				pstAction = samalib_cflt_GetActionBySlotId(pstInst, pstSchedule->ulSlot);
				if (NULL == pstAction)
				{
					pstAction = samalib_cflt_TryStartAction(pstInst, pstSchedule, eSAPI_ACTIONSTATE_Running, &stConflict);
				}

				if (NULL != pstAction)
				{
					pstAction->eActState = eSAPI_ACTIONSTATE_Running;
				}
				else
				{
					samalib_cflt_DuplicateConflictResult(&(pstInst->stConflictResult), &stConflict);
				}

				break;

			case eSAPI_TIMER_TYPE_TL_Stop:
				pstAction = samalib_cflt_GetActionBySlotId(pstInst, pstSchedule->ulSlot);
				if (NULL != pstAction)
				{
					if (TRUE != pstTimeEvt->bActionRelated)
					{
						pstAction->eActState = eSAPI_ACTIONSTATE_PaddingStop;
					}
					else
					{
						// Release Resource
						(void)RLIB_ClearActionResource(pstInst->hRlibSession, pstAction->ulActionId);

						samalib_cflt_ReleaseAction(pstInst, pstAction);
						pstInst->anMaxActCnt[eActType] --;
					}
				}
				else
				{
					HxLOG_Error("Action NULL!!!! Something wrong\n");
				}

				break;

			case eSAPI_TIMER_TYPE_TL_PaddingEnd:
				pstAction = samalib_cflt_GetActionBySlotId(pstInst, pstSchedule->ulSlot);
				if (NULL != pstAction)
				{
					// Release Resource
					(void)RLIB_ClearActionResource(pstInst->hRlibSession, pstAction->ulActionId);

					samalib_cflt_ReleaseAction(pstInst, pstAction);
					pstInst->anMaxActCnt[eActType] --;
				}

				break;


			default:
				break;
			}

		}
	}

	// TODO: SAMA_SCHEDULE_RemovePadding() 에서 conflict none 일 경우만, padding remove를 하므로,
	// TODO: 여기서 conflict none 일 경우 remove padding list를 clear 해 줄 필요는 없지만,,,참고삼아...
#ifdef CONFIG_DEBUG
	samalib_cflt_base_PrintPadRemovalList(pstInst);
#endif

	(void)SAMALIB_CFLT_ClearConflictResult(&stConflict);


	// For Debug:
	if (NULL != szEvtTimeStr)
	{
		HxLOG_Debug("\n[SAMA_TIMELINE] TimeLine Check End:\n");
		HLIB_STD_MemFree(szEvtTimeStr);
	}

	return ERR_OK;
}

HERROR samalib_cflt_base_MakeConflictTimeLine (sama_ConflictCheck_t *pstInst, HBOOL bOmitOneSch, HUINT32 ulSlotToOmit)
{
	DxSchedule_t		*pstSrcSched;
	HxList_t			*pstListItem;


	// 기존의 Conflict Time Line을 제거한다.
	if (NULL != pstInst->pstCfltTimeLine)
	{
		pstInst->pstCfltTimeLine = HLIB_LIST_RemoveAllFunc(pstInst->pstCfltTimeLine, SAMA_Free_CB);
	}

	HxLOG_Debug("Clear Pad Remove List \n");
	pstInst->pstPadRemoveInfoList = HLIB_LIST_RemoveAllFunc(pstInst->pstPadRemoveInfoList, SAMA_Free_CB);


	for (pstListItem = pstInst->pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstSrcSched = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);
		if (NULL != pstSrcSched)
		{
			if ((TRUE == bOmitOneSch) &&
				(pstSrcSched->ulSlot == ulSlotToOmit))
			{
				continue;
			}

			if(pstSrcSched->bExceptConflict == TRUE)
			{
				continue;
			}

			// ignore schedule that status is between remove request and removed response
			if (pstSrcSched->bRemoving == TRUE)
			{
				// TODO: make all SAPI's return value as requestId
				// TODO: make SAPI_Wait to take argument requestId
				// TODO: make OIPF's SAPI call to sync call ( by SAPI_Wait)
				// TODO: delete bRemoving
				HxLOG_Error("Removing status exists. There shouldn't be removing status schedule. slot(%d) \n",pstSrcSched->ulSlot);
				continue;
			}

			switch (pstSrcSched->eRsvType)
			{
			case DxRSVTYPE_RECORD_EBR:
			case DxRSVTYPE_RECORD_TBR:
			case DxRSVTYPE_RECORD_INSTANT:
			case DxRSVTYPE_RECORD_DELAYED:
			case DxRSVTYPE_NETSTREAMING_LIVE:
			case DxRSVTYPE_NETSTREAMING_FILE:
			case DxRSVTYPE_RECORD_SERIES_EBR:
			case DxRSVTYPE_RECORD_SATRECORD_TBR:
				(void)samalib_cflt_base_appendScheduleToTimeLine(pstInst, pstSrcSched);
				break;

			default:
				break;
			}
		}
	}

	return ERR_OK;
}

sama_CnftTimeEvt_t *samalib_cflt_base_FindFirstRelatedTimeLineEvent (HxList_t *pstTimeLine, HUINT32 ulBaseTime)
{
	HINT32				 nActionCnt = 0;
	sama_CnftTimeEvt_t	*pstTimeEvt = NULL;
	HxList_t			*pstListItem;

	for (pstListItem = pstTimeLine; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstTimeEvt = (sama_CnftTimeEvt_t *)HLIB_LIST_Data(pstListItem);
		if (NULL != pstTimeEvt)
		{
			if (pstTimeEvt->utTime >= ulBaseTime)
			{
				break;
			}

			if (TRUE == pstTimeEvt->bActionRelated)
			{
				switch (pstTimeEvt->eTimerType)
				{
				case eSAPI_TIMER_TYPE_TL_PaddingStart:
				case eSAPI_TIMER_TYPE_TL_Start:
					nActionCnt ++;
					break;

				case eSAPI_TIMER_TYPE_TL_PaddingEnd:
				case eSAPI_TIMER_TYPE_TL_Stop:
					nActionCnt --;
					break;

				default:
					break;
				}
			}
		}
	}

	if ((NULL == pstListItem) || (NULL == pstTimeEvt))
	{
		HxLOG_Info ("No Time Event Found... nothing to conflict:\n");
		return NULL;
	}

	if (nActionCnt <= 0)
	{
		return pstTimeEvt;
	}

	for (pstListItem = pstListItem->prev; NULL != pstListItem; pstListItem = pstListItem->prev)
	{
		pstTimeEvt = (sama_CnftTimeEvt_t *)HLIB_LIST_Data(pstListItem);
		if ((NULL != pstTimeEvt) && (TRUE == pstTimeEvt->bActionRelated))
		{
			switch (pstTimeEvt->eTimerType)
			{
			case eSAPI_TIMER_TYPE_TL_PaddingStart:
			case eSAPI_TIMER_TYPE_TL_Start:
				nActionCnt --;
				break;

			case eSAPI_TIMER_TYPE_TL_PaddingEnd:
			case eSAPI_TIMER_TYPE_TL_Stop:
				nActionCnt ++;
				break;

			default:
				break;
			}

			if (nActionCnt <= 0)
			{
				break;
			}
		}
	}

	pstTimeEvt = (sama_CnftTimeEvt_t *)HLIB_LIST_Data(pstListItem);
	if (NULL != pstTimeEvt)
	{
		return pstTimeEvt;
	}

	return (sama_CnftTimeEvt_t *)HLIB_LIST_Data(pstTimeLine);
}


HERROR samalib_cflt_base_TakeResource (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule)
{
	RxRscReqHandle_t		 hReqHandle = (RxRscReqHandle_t)NULL;
	DxService_t				 stService;
	DxTransponder_t			 stTransponder;
	RxTrialArg_t			 unTrialArg;
	HERROR					 hErr, hResult = ERR_FAIL;

	hReqHandle = RLIB_MakeResourceRequestHandle(pstInst->hRlibSession, pstAction->ulActionId, pstAction->ulPriority);
	if ((RxRscReqHandle_t)NULL == hReqHandle)
	{
		HxLOG_Error("RLIB_MakeResourceRequestHandle err:\n");
		goto END_FUNC;
	}

	hErr = samalib_svc_GetService(pstInst, (pstSchedule->ulSvcUid & 0xFFFF), &stService);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("samalib_svc_GetService err:\n");
		goto END_FUNC;
	}

	hErr = samalib_svc_GetTransponder(pstInst, stService.tsuid, &stTransponder);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("samalib_svc_GetTransponder err:\n");
		goto END_FUNC;
	}

	unTrialArg.stTuneParam.eArgType = eRxARGTYPE_TUNEPARAM;
	SAMA_MemCpy(&(unTrialArg.stTuneParam.stTuneParam), &(stTransponder.tuningParam), sizeof(DxTuneParam_t));

	hErr = RLIB_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_RFINPUT, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	hErr = RLIB_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_TUNER_GROUP, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	hErr = RLIB_AddArgumentToRequestHandle(hReqHandle, &unTrialArg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	hErr = RLIB_TryResourceRequest(hReqHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_TryResourceRequest err:\n");
		goto END_FUNC;
	}

	hErr = RLIB_SetResourcesByTrialResults(hReqHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_TryResourceRequest err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	if ((RxRscReqHandle_t)NULL != hReqHandle)
	{
		(void)RLIB_DestroyResourceRequestHandle(hReqHandle);
	}

	return hResult;
}

HERROR samalib_cflt_base_GetTpInfoUsingSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule, DxTransponder_t *pstTransponder)
{
	HERROR					hErr;

	HxLOG_Trace();

	if( pstSchedule->eRsvType != DxRSVTYPE_NETSTREAMING_LIVE
		&& pstSchedule->eRsvType != DxRSVTYPE_NETSTREAMING_FILE)
	{
		HxLOG_Error("cannot use Tpinfo in schedule data !\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	// 현재 SAMA에서 tune info 외에 정보는 필요 없음.
	SAMA_MemSet(pstTransponder, 0x00, sizeof(DxTransponder_t));

	SAMA_MemCpy(&(pstTransponder->tuningParam), &(pstSchedule->uExtInfo.stStrm.stTuningParam), sizeof(DxTuneParam_t));
	hErr = ERR_OK;


END_FUNC:
	return hErr;

}

HERROR samalib_cflt_base_GetTpInfoUsingSvcUid (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule, DxTransponder_t *pstTransponder)
{
	HERROR					hErr;
	DxService_t				stService;


	HxLOG_Trace();

	hErr = samalib_svc_GetService(pstInst, (pstSchedule->ulSvcUid & 0xFFFF), &stService);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("samalib_svc_GetService err:\n");
		goto END_FUNC;
	}

	hErr = samalib_svc_GetTransponder(pstInst, stService.tsuid, pstTransponder);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("samalib_svc_GetTransponder err:\n");
		goto END_FUNC;
	}


	hErr = ERR_OK;
END_FUNC:
	return hErr;

}

HERROR samalib_cflt_base_TakeResource2 (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule, HxList_t *pstRemovableActList)
{
	HUINT32					 ulCnt, ulActionId;
	RxRscReqHandle_t		 hReqHandle = (RxRscReqHandle_t)NULL;
	DxTransponder_t			 stTransponder;
	RxTrialArg_t			 unTrialArg;
	HxList_t				*pstListItem;
	sama_ActionItem_t		*pstRemoveAct;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	HxLOG_Debug("pstInst->hRlibSession: 0x%x, pstAction->ulActionId: %d, pstAction->ulPriority: %d \n", pstInst->hRlibSession, pstAction->ulActionId, pstAction->ulPriority);

	// samalib_cflt_base_TakeResource2() 에서 사실은 transcoder resource 에 관한 처리도 해 주는 것이 맞지만,
	// PVR HLS의 경우 편의상 이미 sama context의 max action count 로 check 하고 있으므로
	// samalib_cflt_base_TakeResource2() 에서 바로 ERR_OK 처리
	if(DxRSVTYPE_NETSTREAMING_FILE == pstSchedule->eRsvType)
	{
		HxLOG_Debug("pstSchedule->eRsvType(0x%x) is OK\n", pstSchedule->eRsvType);
		return ERR_OK;
	}

	hReqHandle = RLIB_MakeResourceRequestHandle(pstInst->hRlibSession, pstAction->ulActionId, pstAction->ulPriority);
	if ((RxRscReqHandle_t)NULL == hReqHandle)
	{
		HxLOG_Error("RLIB_MakeResourceRequestHandle err, pstInst->hRlibSession: 0x%0x, pstAction->ulActionId: %d, pstAction->ulPriority: %d\n", pstInst->hRlibSession, pstAction->ulActionId, pstAction->ulPriority);
		goto END_FUNC;
	}

	HxLOG_Debug("hReqHandle: 0x%x \n", hReqHandle);

	hErr = samalib_cflt_GetTpInfo(pstInst, pstSchedule, &stTransponder);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("samalib_cflt_GetTpInfo err:\n");
		goto END_FUNC;
	}

#ifdef CONFIG_DEBUG
	HxLOG_Debug("Tune info============================================================================\r\n", __FUNCTION__, __LINE__);
	HxLOG_HexIntDump(stTransponder.tuningParam.eDeliType);
	switch(stTransponder.tuningParam.eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		HxLOG_DecIntDump(stTransponder.tuningParam.sat.tuningInfo.ulFrequency);
		HxLOG_DecIntDump(stTransponder.tuningParam.sat.tuningInfo.ulSymbolRate);
		HxLOG_HexIntDump(stTransponder.tuningParam.sat.tuningInfo.eFecCodeRate);
		HxLOG_HexIntDump(stTransponder.tuningParam.sat.tuningInfo.ePolarization);
		HxLOG_HexIntDump(stTransponder.tuningParam.sat.tuningInfo.eTransSpec);
		HxLOG_HexIntDump(stTransponder.tuningParam.sat.tuningInfo.antuid);

		HxLOG_Debug("=========== Ant info=====================================================\r\n", __FUNCTION__, __LINE__);
		HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.uid);
		HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.antType);
		switch(stTransponder.tuningParam.sat.antInfo.antType)
		{
		case eDxANT_TYPE_LNB_ONLY:
			HxLOG_Debug("=====================LNB info=====================================================\r\n", __FUNCTION__, __LINE__);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.lnb.ulLnbFreq);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.lnb.lnbVoltage);
			break;

		case eDxANT_TYPE_DISEQC:
			HxLOG_Debug("=====================DISEQC info=====================================================\r\n", __FUNCTION__, __LINE__);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.diseqc.ulLnbFreq);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.diseqc.lnbVoltage);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.diseqc.b22kTone);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.diseqc.diseqcVersion);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.diseqc.diseqcInput);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.diseqc.toneBurst);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.diseqc.ucMotorPosition);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.diseqc.sSatLonggitude);
			break;

		case eDxANT_TYPE_SCD:
			HxLOG_Debug("=====================SCD info=====================================================\r\n", __FUNCTION__, __LINE__);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.ulLnbFreq);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.lnbVoltage);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.b22kTone);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.scdInput);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.ulScdUserBandNum);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.ulScdUserBandFreq);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.ulScdUserBandNum1);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.ulScdUserBandFreq1);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.ulScdUserBandNum2);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.ulScdUserBandFreq2);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.ulScdUserBandNum3);
			HxLOG_HexIntDump(stTransponder.tuningParam.sat.antInfo.info.scd.ulScdUserBandFreq3);
			break;

		case eDxANT_TYPE_SMATV:
			HxLOG_Debug("=====================SMATV info=====================================================\r\n", __FUNCTION__, __LINE__);
			break;

		case eDxANT_TYPE_MONOBLOCK_LNB:
			HxLOG_Debug("=====================MONOBLOCK info=====================================================\r\n", __FUNCTION__, __LINE__);
			break;

		default:
			break;
		}
		break;

	case eDxDELIVERY_TYPE_CAB:
		HxLOG_DecIntDump(stTransponder.tuningParam.cab.ulFrequency);
		HxLOG_DecIntDump(stTransponder.tuningParam.cab.ulSymbolRate);
		HxLOG_DecIntDump(stTransponder.tuningParam.cab.nTsId);
		HxLOG_DecIntDump(stTransponder.tuningParam.cab.nNetId);
		break;

	case eDxDELIVERY_TYPE_TER:
		HxLOG_DecIntDump(stTransponder.tuningParam.ter.ulFrequency);
		HxLOG_DecIntDump(stTransponder.tuningParam.ter.eBandWidth);
		HxLOG_DecIntDump(stTransponder.tuningParam.ter.deliveryType);
		HxLOG_DecIntDump(stTransponder.tuningParam.ter.ulTunerGroupId);
		HxLOG_DecIntDump(stTransponder.tuningParam.ter.eTransMode);
		break;

	default:
		break;
	}
	HxLOG_Debug("============================================================================\r\n", __FUNCTION__, __LINE__);
#endif

	unTrialArg.stTuneParam.eArgType = eRxARGTYPE_TUNEPARAM;
	SAMA_MemCpy(&(unTrialArg.stTuneParam.stTuneParam), &(stTransponder.tuningParam), sizeof(DxTuneParam_t));

	hErr = RLIB_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_RFINPUT, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	hErr = RLIB_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_TUNER_GROUP, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	hErr = RLIB_AddArgumentToRequestHandle(hReqHandle, &unTrialArg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	for (pstListItem = pstRemovableActList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstRemoveAct = (sama_ActionItem_t *)HLIB_LIST_Data(pstListItem);
		if (NULL != pstRemoveAct)
		{
			pstRemoveAct->bToBeDeleted = FALSE;

			HxLOG_Debug("call RLIB_AddRemovableActionIdToRequestHandle() with \n");
			HxLOG_Debug("bAllocated: %d, ulActionId: %d, ulSlot: %d, eActType: %d, eActState: %d, ulPriority: %d, bToBeDeleted: %d \n",
							pstRemoveAct->bAllocated ,
							pstRemoveAct->ulActionId ,
							pstRemoveAct->ulSlot ,
							pstRemoveAct->eActType ,
							pstRemoveAct->eActState ,
							pstRemoveAct->ulPriority ,
							pstRemoveAct->bToBeDeleted );

			if (pstAction->ulPriority > pstRemoveAct->ulPriority)
			{
				hErr = RLIB_AddRemovableActionIdToRequestHandle(hReqHandle, pstRemoveAct->ulActionId, pstRemoveAct->ulPriority);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
				}
			}
		}
	}

	hErr = RLIB_TryResourceRequest(hReqHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("RLIB_TryResourceRequest err:\n"));
		goto END_FUNC;
	}

	pstTrialResult = RLIB_GetRequestTrialResult(hReqHandle);
	if (NULL != pstTrialResult)
	{
		for (ulCnt = 0; ulCnt < pstTrialResult->ulNumActionToBeKilled; ulCnt++)
		{
			ulActionId = pstTrialResult->pulActionsToBeKilled[ulCnt];

			for (pstListItem = pstRemovableActList; NULL != pstListItem; pstListItem = pstListItem->next)
			{
				pstRemoveAct = (sama_ActionItem_t *)HLIB_LIST_Data(pstListItem);
				if ((NULL != pstRemoveAct) && (ulActionId == pstRemoveAct->ulActionId))
				{
					pstRemoveAct->bToBeDeleted = TRUE;
					RLIB_ClearActionResource(pstInst->hRlibSession, ulActionId);
					break;
				}
			}
		}
	}

	hErr = RLIB_SetResourcesByTrialResults(hReqHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_TryResourceRequest err:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	if ((RxRscReqHandle_t)NULL != hReqHandle)
	{
		(void)RLIB_DestroyResourceRequestHandle(hReqHandle);
	}

	return hResult;
}

HxList_t *samalib_cflt_base_GetConflictedActionList (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule)
{
	HUINT32					 ulCnt;
	RxRscReqHandle_t		 hReqHandle = (RxRscReqHandle_t)NULL;
	DxService_t				 stService;
	DxTransponder_t			 stTransponder;
	RxTrialArg_t			 unTrialArg;
	HxList_t				*pstActionList = NULL;
	sama_ActionItem_t		*pstRemoveAct;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr;

	hReqHandle = RLIB_MakeResourceRequestHandle(pstInst->hRlibSession, pstAction->ulActionId, pstAction->ulPriority);
	if ((RxRscReqHandle_t)NULL == hReqHandle)
	{
		HxLOG_Error("RLIB_MakeResourceRequestHandle err:\n");
		goto END_FUNC;
	}

	hErr = samalib_svc_GetService(pstInst, (pstSchedule->ulSvcUid & 0xFFFF), &stService);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("samalib_svc_GetService err:\n");
		goto END_FUNC;
	}

	hErr = samalib_svc_GetTransponder(pstInst, stService.tsuid, &stTransponder);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("samalib_svc_GetTransponder err:\n");
		goto END_FUNC;
	}

	unTrialArg.stTuneParam.eArgType = eRxARGTYPE_TUNEPARAM;
	SAMA_MemCpy(&(unTrialArg.stTuneParam.stTuneParam), &(stTransponder.tuningParam), sizeof(DxTuneParam_t));

	hErr = RLIB_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_RFINPUT, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	hErr = RLIB_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_TUNER_GROUP, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	hErr = RLIB_AddArgumentToRequestHandle(hReqHandle, &unTrialArg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
	{
		sama_ActionItem_t		*pstRemoveAct = &(pstInst->pstActionArray[ulCnt]);

		HxLOG_Debug(">> ulCnt[%d],ulPriority[%d],bAllocated[%d],ulSlot[%d] \n",ulCnt,pstRemoveAct->ulPriority,pstRemoveAct->bAllocated,pstRemoveAct->ulSlot);

		if ( pstRemoveAct->ulPriority == eSAPI_ACTIONPRIORITY_Running && (TRUE == pstRemoveAct->bAllocated) && (pstRemoveAct->ulActionId != pstAction->ulActionId))
		{
			HxLOG_Debug("bAllocated: %d, ulActionId: %d, ulSlot: %d, eActType: %d, eActState: %d, ulPriority: %d, bToBeDeleted: %d \n",
							pstRemoveAct->bAllocated ,
							pstRemoveAct->ulActionId ,
							pstRemoveAct->ulSlot ,
							pstRemoveAct->eActType ,
							pstRemoveAct->eActState ,
							pstRemoveAct->ulPriority ,
							pstRemoveAct->bToBeDeleted );
			if (pstAction->ulPriority > pstRemoveAct->ulPriority)
			{
				hErr = RLIB_AddRemovableActionIdToRequestHandle(hReqHandle, pstRemoveAct->ulActionId, pstRemoveAct->ulPriority);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
				}
			}
		}
	}

	hErr = RLIB_TryResourceRequest(hReqHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("RLIB_TryResourceRequest err:\n"));
#if 1
		for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
		{
			sama_ActionItem_t		*pstRemoveAct = &(pstInst->pstActionArray[ulCnt]);

			if ((TRUE == pstRemoveAct->bAllocated) && (pstRemoveAct->ulActionId != pstAction->ulActionId))
			{
				HxLOG_Debug("bAllocated: %d, ulActionId: %d, ulSlot: %d, eActType: %d, eActState: %d, ulPriority: %d, bToBeDeleted: %d \n",
								pstRemoveAct->bAllocated ,
								pstRemoveAct->ulActionId ,
								pstRemoveAct->ulSlot ,
								pstRemoveAct->eActType ,
								pstRemoveAct->eActState ,
								pstRemoveAct->ulPriority ,
								pstRemoveAct->bToBeDeleted );


				pstActionList = HLIB_LIST_Append(pstActionList, (void *)pstRemoveAct);
			}
		}
#endif

		goto END_FUNC;
	}


	pstTrialResult = RLIB_GetRequestTrialResult(hReqHandle);
	if (NULL != pstTrialResult)
	{
		HxLOG_Debug("pstTrialResult->ulNumActionToBeKilled: %d \n", pstTrialResult->ulNumActionToBeKilled);
		for (ulCnt = 0; ulCnt < pstTrialResult->ulNumActionToBeKilled; ulCnt++)
		{
			pstRemoveAct = samalib_cflt_GetActionByActionId(pstInst, pstTrialResult->pulActionsToBeKilled[ulCnt]);
			if (NULL != pstRemoveAct)
			{
				pstActionList = HLIB_LIST_Append(pstActionList, (void *)pstRemoveAct);
			}
			else
			{
				HxLOG_Error("pstRemoveAct is NULL \n");
			}
		}
	}
	else
	{
		HxLOG_Error("pstTrialResult is NULL \n");
	}

END_FUNC:
	if ((RxRscReqHandle_t)NULL != hReqHandle)
	{
		(void)RLIB_DestroyResourceRequestHandle(hReqHandle);
	}

	return pstActionList;
}

sama_ActionItem_t *samalib_cflt_base_TryStartAction (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstSchedule, sama_ActionState_e eState, SAPI_Conflict_t *pstConflict)
{
	HUINT32					 ulCnt, ulActCount;
	SAPI_ActionType_e		 eActType;
	sama_ActionItem_t		*pstAction = NULL, *pstDstAction = NULL, *pstRemovableAction;
	HxList_t				*pstListItem, *pstRemoveActList = NULL;
	SAPI_ConflictItem_t		 stConflictItem;
	HERROR					 hErr;
	HBOOL	bIsPadOverlapped = FALSE;
	HBOOL	bIsBodyOverlapped = FALSE;
	HUINT32	ulPadOverlappedSlot = 0;
	HUINT32	ulBodyOverlappedSlot = 0;
	sama_PaddingRemoval_e		ePadRemoval = eSAPI_PADDINGRMV_None;
	HUINT32			ulPriority;

	HxLOG_Trace();

	if ((NULL == pstInst) || (NULL == pstSchedule) || (NULL == pstConflict))
	{
		return NULL;
	}

	{
		sama_PaddingRemoval_e	ePadding = eSAPI_PADDINGRMV_EndOfCase;
		HBOOL					bRemovedPad = FALSE;
		if (eState == eSAPI_ACTIONSTATE_PaddingStart)
		{
			ePadding = eSAPI_PADDINGRMV_StartPad;
		}
		else if (eState == eSAPI_ACTIONSTATE_PaddingStop)
		{
			ePadding = eSAPI_PADDINGRMV_StopPad;
		}
		bRemovedPad = samalib_cflt_base_CheckPadRemovalListItemAlreadyExist(pstInst, pstSchedule->ulSlot,ePadding);
		if (bRemovedPad == TRUE)
		{
			HxLOG_Debug("Tried Pad Start/End when pad is removed. slot(%d) action(%d) pad(%d) \n",pstSchedule->ulSlot, eState, ePadding);
			pstConflict->eType = eSAPI_CONFLICT_TYPE_OverCapacity;
			stConflictItem.ulSlot = pstSchedule->ulSlot;
			(void)SAMALIB_CFLT_AppendConflictResult(pstConflict, &stConflictItem);

			return NULL;
		}
	}

	eActType = samalib_cflt_base_getSapiActionTypeBySchedule(pstSchedule);

	// 일단 record만...check.
	bIsPadOverlapped = samalib_cflt_base_CheckSameServiceOverlappedTime(pstInst, pstSchedule, &ulPadOverlappedSlot,&ePadRemoval, &bIsBodyOverlapped, &ulBodyOverlappedSlot );
	HxLOG_Debug("bIsPadOverlapped: %d, bIsBodyOverlapped: %d \n", bIsPadOverlapped, bIsBodyOverlapped);
	if(bIsBodyOverlapped == TRUE)
	{
		pstConflict->eType = eSAPI_CONFLICT_TYPE_OverCapacity;
		stConflictItem.ulSlot = ulBodyOverlappedSlot;
		(void)SAMALIB_CFLT_AppendConflictResult(pstConflict, &stConflictItem);

		HxLOG_Debug("Overlapped body time!. eSAPI_CONFLICT_TYPE_OverCapacity. slot(%d)\n",ulBodyOverlappedSlot);
		return NULL;

	}

	if(bIsPadOverlapped == TRUE)
	{
		HBOOL	bIsExist = FALSE;

		bIsExist = samalib_cflt_base_CheckPadRemovalListItemAlreadyExist(pstInst, ulPadOverlappedSlot, ePadRemoval);
		HxLOG_Debug("ulPadOverlappedSlot: %d, bIsExist: %d, ePadRemoval: %d  \n", ulPadOverlappedSlot, bIsExist, ePadRemoval);
		if(bIsExist == TRUE)
		{
			pstConflict->eType = eSAPI_CONFLICT_TYPE_OverCapacity;
			stConflictItem.ulSlot = ulBodyOverlappedSlot;
			(void)SAMALIB_CFLT_AppendConflictResult(pstConflict, &stConflictItem);

			HxLOG_Debug("Overlapped body time!. eSAPI_CONFLICT_TYPE_OverCapacity. slot(%d) \n",ulBodyOverlappedSlot);
			return NULL;
		}

		samalib_cflt_base_appendRemovablePadding(pstInst,ulPadOverlappedSlot, ePadRemoval);


		// 개수를 제거해도 Resource 충돌이 일어날 수 있다
		HxLOG_Debug("recheck after remove padding! \n");
		return samalib_cflt_TryStartAction(pstInst, pstSchedule, eState, pstConflict);
	}





	// 0 4
	HxLOG_Debug("eActType: %d, pstInst->anMaxActCnt[eActType]: %d, pstInst->stRscInfo.anActionNum[eActType]: %d \n", eActType, pstInst->anMaxActCnt[eActType], pstInst->stRscInfo.anActionNum[eActType]);
	if ( pstInst->anMaxActCnt[eActType] >= pstInst->stRscInfo.anActionNum[eActType])
	{
		pstAction = samalib_cflt_GetActionByActionState(pstInst, eSAPI_ACTIONSTATE_PaddingStart, 0);
		if (pstAction == NULL
			&& eState != eSAPI_ACTIONSTATE_PaddingStart)
		{
			pstAction = samalib_cflt_GetActionByActionState(pstInst, eSAPI_ACTIONSTATE_PaddingStop, 0);
		}

		if (NULL != pstAction)
		{

			switch (pstAction->eActState)
			{
			case eSAPI_ACTIONSTATE_PaddingStart:
				samalib_cflt_base_appendRemovablePadding(pstInst,pstAction->ulSlot, eSAPI_PADDINGRMV_StartPad);
				break;

			case eSAPI_ACTIONSTATE_PaddingStop:
				samalib_cflt_base_appendRemovablePadding(pstInst,pstAction->ulSlot, eSAPI_PADDINGRMV_StopPad);
				break;

			default:
				break;
			}

			(void)RLIB_ClearActionResource(pstInst->hRlibSession, pstAction->ulActionId);
			samalib_cflt_ReleaseAction(pstInst, pstAction);
			pstInst->anMaxActCnt[eActType] --;

			// 개수를 제거해도 Resource 충돌이 일어날 수 있다
			HxLOG_Debug("recheck after remove padding! \n");
			return samalib_cflt_TryStartAction(pstInst, pstSchedule, eState, pstConflict);
		}
		else
		{
			HBOOL bIsTpTunable = FALSE;

			bIsTpTunable = samalib_cflt_base_IsTpTunable(pstInst, pstSchedule);
			if (bIsTpTunable)
			{
				pstConflict->eType = eSAPI_CONFLICT_TYPE_OverCapacity;
			}
			else
			{
				pstConflict->eType = eSAPI_CONFLICT_TYPE_Tp;
			}

			ulActCount = samalib_cflt_CountActionByActionState(pstInst, eSAPI_ACTIONSTATE_Running);
			for (ulCnt = 0; ulCnt < ulActCount; ulCnt++)
			{
				pstAction = samalib_cflt_GetActionByActionState(pstInst, eSAPI_ACTIONSTATE_Running, ulCnt);
				if (NULL != pstAction)
				{
					stConflictItem.ulSlot = pstAction->ulSlot;
					(void)SAMALIB_CFLT_AppendConflictResult(pstConflict, &stConflictItem);
				}
			}

			stConflictItem.ulSlot = pstSchedule->ulSlot;
			(void)SAMALIB_CFLT_AppendConflictResult(pstConflict, &stConflictItem);

			HxLOG_Debug("max action count exceeds!. type(%d) !!\n",pstConflict->eType);
			return NULL;
		}
	}

	hErr = samalib_cflt_GetActionPriority(eState, &ulPriority);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("samalib_cflt_GetActionPriority() fail! \n");
		return NULL;
	}

	pstAction = samalib_cflt_TakeAction(pstInst, ulPriority);
	if (NULL == pstAction)
	{
		pstConflict->eType = eSAPI_CONFLICT_TYPE_OverCapacity;

		ulActCount = samalib_cflt_CountActionByActionState(pstInst, eSAPI_ACTIONSTATE_Running);
		for (ulCnt = 0; ulCnt < ulActCount; ulCnt++)
		{
			pstAction = samalib_cflt_GetActionByActionState(pstInst, eSAPI_ACTIONSTATE_Running, ulCnt);
			if (NULL != pstAction)
			{
				stConflictItem.ulSlot = pstAction->ulSlot;
				(void)SAMALIB_CFLT_AppendConflictResult(pstConflict, &stConflictItem);
			}
		}

		stConflictItem.ulSlot = pstSchedule->ulSlot;
		(void)SAMALIB_CFLT_AppendConflictResult(pstConflict, &stConflictItem);

		HxLOG_Debug("samalib_cflt_TakeAction fail. eSAPI_CONFLICT_TYPE_OverCapacity !!\n");

		return NULL;
	}

	pstAction->ulSlot = pstSchedule->ulSlot;

//////////////////////////
	ulActCount = samalib_cflt_CountActionByActionState(pstInst, eSAPI_ACTIONSTATE_PaddingStop);
	for (ulCnt = 0; ulCnt < ulActCount; ulCnt++)
	{
		pstRemovableAction = samalib_cflt_GetActionByActionState(pstInst, eSAPI_ACTIONSTATE_PaddingStop, ulCnt);
		if (NULL != pstRemovableAction && pstRemovableAction->ulSlot != pstSchedule->ulSlot)
		{
			pstDstAction = (sama_ActionItem_t *)SAMA_Malloc(sizeof(sama_ActionItem_t));
			if (NULL != pstDstAction)
			{
				SAMA_MemCpy(pstDstAction, pstRemovableAction, sizeof(sama_ActionItem_t));

				hErr = samalib_cflt_GetActionPriority(eSAPI_ACTIONSTATE_PaddingStop, &ulPriority);
				if(hErr == ERR_FAIL)
				{
					HxLOG_Error("samalib_cflt_GetActionPriority() fail! \n");
					return NULL;
				}

				pstDstAction->ulPriority = ulPriority;
				pstDstAction->bToBeDeleted = FALSE;

				HxLOG_Debug("add removable action, eSAPI_ACTIONSTATE_PaddingStop \n");
				HxLOG_Debug("bAllocated: %d, ulActionId: %d, ulSlot: %d, eActType: %d, eActState: %d, ulPriority: %d, bToBeDeleted: %d \n",
								pstDstAction->bAllocated ,
								pstDstAction->ulActionId ,
								pstDstAction->ulSlot ,
								pstDstAction->eActType ,
								pstDstAction->eActState ,
								pstDstAction->ulPriority ,
								pstDstAction->bToBeDeleted );
				pstRemoveActList = HLIB_LIST_Append(pstRemoveActList, (void *)pstDstAction);
			}
		}
	}

	ulActCount = samalib_cflt_CountActionByActionState(pstInst, eSAPI_ACTIONSTATE_PaddingStart);
	for (ulCnt = 0; ulCnt < ulActCount; ulCnt++)
	{
		pstRemovableAction = samalib_cflt_GetActionByActionState(pstInst, eSAPI_ACTIONSTATE_PaddingStart, ulCnt);
		if (NULL != pstRemovableAction && pstRemovableAction->ulSlot != pstSchedule->ulSlot)
		{
			pstDstAction = (sama_ActionItem_t *)SAMA_Malloc(sizeof(sama_ActionItem_t));
			if (NULL != pstDstAction)
			{
				SAMA_MemCpy(pstDstAction, pstRemovableAction, sizeof(sama_ActionItem_t));

				hErr = samalib_cflt_GetActionPriority(eSAPI_ACTIONSTATE_PaddingStart, &ulPriority);
				if(hErr == ERR_FAIL)
				{
					HxLOG_Error("samalib_cflt_GetActionPriority() fail! \n");
					return NULL;
				}

				pstDstAction->ulPriority = ulPriority;
				pstDstAction->bToBeDeleted = FALSE;

				HxLOG_Debug("add removable action, eSAPI_ACTIONSTATE_PaddingStart \n");
				HxLOG_Debug("bAllocated: %d, ulActionId: %d, ulSlot: %d, eActType: %d, eActState: %d, ulPriority: %d, bToBeDeleted: %d \n",
								pstDstAction->bAllocated ,
								pstDstAction->ulActionId ,
								pstDstAction->ulSlot ,
								pstDstAction->eActType ,
								pstDstAction->eActState ,
								pstDstAction->ulPriority ,
								pstDstAction->bToBeDeleted );
				pstRemoveActList = HLIB_LIST_Append(pstRemoveActList, (void *)pstDstAction);
			}
		}
	}
/////////////////////////////

	hErr = samalib_cflt_TakeResource2 (pstInst, pstAction, pstSchedule, pstRemoveActList);
	if (ERR_OK == hErr)
	{
		for (pstListItem = pstRemoveActList; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			pstDstAction = (sama_ActionItem_t *)HLIB_LIST_Data(pstListItem);
			if ((NULL != pstDstAction) && (TRUE == pstDstAction->bToBeDeleted))
			{
				switch (pstDstAction->eActState)
				{
				case eSAPI_ACTIONSTATE_PaddingStart:
					samalib_cflt_base_appendRemovablePadding(pstInst, pstDstAction->ulSlot,eSAPI_PADDINGRMV_StartPad);
					break;

				case eSAPI_ACTIONSTATE_PaddingStop:
					samalib_cflt_base_appendRemovablePadding(pstInst, pstDstAction->ulSlot,eSAPI_PADDINGRMV_StopPad);
					break;

				default:
					break;
				}
			}

		}




		(void)HLIB_LIST_RemoveAllFunc(pstRemoveActList, SAMA_Free_CB);
		HxLOG_Debug("it's ok after remove padding.\n");
		pstInst->anMaxActCnt[eActType] ++;
		return pstAction;
	}
	else
	{
		HxLOG_Debug("samalib_cflt_TakeResource2() fail!\n");
	}

	(void)HLIB_LIST_RemoveAllFunc(pstRemoveActList, SAMA_Free_CB);

	HxLOG_Debug("slot: %d, eSAPI_CONFLICT_TYPE_Tp!!!\n", pstSchedule->ulSlot);

	// PADDING을 빼도 Resource Out:
	pstConflict->eType = eSAPI_CONFLICT_TYPE_Tp;

	pstRemoveActList = samalib_cflt_GetConflictedActionList(pstInst, pstAction, pstSchedule);
	if (NULL != pstRemoveActList)
	{
		for (pstListItem = pstRemoveActList; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			pstDstAction = (sama_ActionItem_t *)HLIB_LIST_Data(pstListItem);
			if (NULL != pstDstAction)
			{
				stConflictItem.ulSlot = pstDstAction->ulSlot;
				(void)SAMALIB_CFLT_AppendConflictResult(pstConflict, &stConflictItem);
			}
		}

		(void)HLIB_LIST_RemoveAll(pstRemoveActList);
	}

	stConflictItem.ulSlot = pstSchedule->ulSlot;
	(void)SAMALIB_CFLT_AppendConflictResult(pstConflict, &stConflictItem);

	// 여기까지 왔으면, take action은 하였으나, take resource를 못했다. 그럼 action을 놔줘야 한다.
	samalib_cflt_ReleaseAction(pstInst, pstAction);

 	return NULL;
}

HERROR samalib_cflt_base_TryResource (sama_ConflictCheck_t *pstInst, sama_ActionItem_t *pstAction, DxSchedule_t *pstSchedule)
{
	RxRscReqHandle_t		 hReqHandle = (RxRscReqHandle_t)NULL;
	DxTransponder_t			 stTransponder;
	RxTrialArg_t			 unTrialArg;
	HERROR					 hErr, hResult = ERR_FAIL;

	HxLOG_Debug("pstInst->hRlibSession: 0x%x, pstAction->ulActionId: %d, pstAction->ulPriority: %d \n", pstInst->hRlibSession, pstAction->ulActionId, pstAction->ulPriority);

	// samalib_cflt_base_TakeResource2() 에서 사실은 transcoder resource 에 관한 처리도 해 주는 것이 맞지만,
	// PVR HLS의 경우 편의상 이미 sama context의 max action count 로 check 하고 있으므로
	// samalib_cflt_base_TakeResource2() 에서 바로 ERR_OK 처리
	if(DxRSVTYPE_NETSTREAMING_FILE == pstSchedule->eRsvType)
	{
		HxLOG_Debug("pstSchedule->eRsvType(0x%x) is OK\n", pstSchedule->eRsvType);
		return ERR_OK;
	}

	hReqHandle = RLIB_MakeResourceRequestHandle(pstInst->hRlibSession, pstAction->ulActionId, pstAction->ulPriority);
	if ((RxRscReqHandle_t)NULL == hReqHandle)
	{
		HxLOG_Error("RLIB_MakeResourceRequestHandle err, pstInst->hRlibSession: 0x%0x, pstAction->ulActionId: %d, pstAction->ulPriority: %d\n", pstInst->hRlibSession, pstAction->ulActionId, pstAction->ulPriority);
		goto END_FUNC;
	}

	HxLOG_Debug("hReqHandle: 0x%x \n", hReqHandle);

	hErr = samalib_cflt_GetTpInfo(pstInst, pstSchedule, &stTransponder);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("samalib_cflt_GetTpInfo err:\n");
		goto END_FUNC;
	}

	unTrialArg.stTuneParam.eArgType = eRxARGTYPE_TUNEPARAM;
	SAMA_MemCpy(&(unTrialArg.stTuneParam.stTuneParam), &(stTransponder.tuningParam), sizeof(DxTuneParam_t));

	hErr = RLIB_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_RFINPUT, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	hErr = RLIB_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_TUNER_GROUP, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	hErr = RLIB_AddArgumentToRequestHandle(hReqHandle, &unTrialArg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_AddRequiredResourceToRequestHandle err:\n");
	}

	hErr = RLIB_TryResourceRequest(hReqHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("RLIB_TryResourceRequest err:\n"));
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	if ((RxRscReqHandle_t)NULL != hReqHandle)
	{
		(void)RLIB_DestroyResourceRequestHandle(hReqHandle);
	}

	return hResult;
}

