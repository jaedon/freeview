/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_channel_tunable.c
	@brief

	Description:  									\n
	Module: node/binding/datatype		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#include <hlib.h>
#include <dlib.h>
#include <rlib.h>
#include <vkernel.h>
#include <apk.h>
#include <rest_maintask.h>
#include <rest_datatype.h>
#include <rest_bind.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32					 ulActionId;
	HUINT32					 ulActionPriority;
} restBindActInfo_t;

typedef struct
{
	RestTuneCheckDst_e		 eDest;

	HUINT32					 ulStartedSession;
	
	HUINT32					 ulCurrPos, ulTunableNum;
	RestTsTunable_t			*pstTunableArray;

	HUINT32					 ulTrialActionId;
	HUINT32					 ulTrialActionPriority;

	HUINT32					 ulRemovableActNum;
	restBindActInfo_t		*pstRemovableActs;
	
} restBindTsTunableCb_t;

typedef struct
{
	HUINT32					 ulSession;
	HxVector_t				*pstTsVector;
} restTuneCheckMsg_t;


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG					 s_ulRestTuneCheckTask_MsgQid = 0;
STATIC unsigned long			 s_ulRestTuneCheckTask_TaskId = 0;

#define ___LOCAL_FUNCTIONS___
STATIC HBOOL rest_bind_tunable_CbCheckTunable (void *data, void *userdata)
{
	DxTransponder_t 		*pstTs = (DxTransponder_t *)data;
	restBindTsTunableCb_t	*pstCbInfo = (restBindTsTunableCb_t *)userdata;
	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();
	HERROR					 hErr;

	if ((NULL == pstContext) || (pstContext->ulLatestCheckSession != pstCbInfo->ulStartedSession))
	{
		HxLOG_Error("Session changed! Stop right now...\n");
		return TRUE;
	}

	if ((NULL != pstTs) && (pstCbInfo->ulCurrPos < pstCbInfo->ulTunableNum))
	{
		HUINT32					 ulCnt;
		Handle_t				 hReqHandle = (Handle_t)NULL;
		RxTrialArg_t			 unTrialArg;
		RestTsTunable_t		 	*pstTunableItem = &(pstCbInfo->pstTunableArray[pstCbInfo->ulCurrPos]);

		hReqHandle = APK_RSC_MakeResourceRequestHandle(pstCbInfo->ulTrialActionId, pstCbInfo->ulTrialActionPriority, FALSE);
		if ((Handle_t)NULL != hReqHandle)
		{
			pstTunableItem->ulTsUid = pstTs->uid;

			// Set RF-Input:
			if (eDxDELIVERY_TYPE_SAT == pstTs->deliType)
			{
				hErr = APK_RSC_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_RFINPUT, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("APK_RSC_AddRequiredResourceToRequestHandle eRxRSCTYPE_RFINPUT err:\n");
				}
			}

			// Set Tuner
			hErr = APK_RSC_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_TUNER_GROUP, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("APK_RSC_AddRequiredResourceToRequestHandle eRxRSCTYPE_TUNER_GROUP err:\n");
			}

			// Set Tuning Parameter
			unTrialArg.eArgType = eRxARGTYPE_TUNEPARAM;
			unTrialArg.stTuneParam.stTuneParam = pstTs->tuningParam;
			hErr = APK_RSC_AddArgumentToRequestHandle(hReqHandle, &unTrialArg);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("APK_RSC_AddArgumentToRequestHandle err:\n");
			}

			// Set the removable (ignorable) actions:
			for (ulCnt = 0; ulCnt < pstCbInfo->ulRemovableActNum; ulCnt++)
			{
				hErr = APK_RSC_AddIgnorableActionIdToRequestHandle(hReqHandle, pstCbInfo->pstRemovableActs[ulCnt].ulActionId);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("APK_RSC_AddRemovableActionIdToRequestHandle: actionId(%d), priority(%d)\n", pstCbInfo->pstRemovableActs[ulCnt].ulActionId, pstCbInfo->pstRemovableActs[ulCnt].ulActionPriority);
				}
				
			}

			hErr = APK_RSC_TryResourceRequest(hReqHandle);
			pstTunableItem->abTunable[pstCbInfo->eDest] = (ERR_OK == hErr) ? TRUE : FALSE;

			hErr = APK_RSC_DestroyResourceRequestHandle(hReqHandle);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("APK_RSC_DestroyResourceRequestHandle err:\n");
			}

			pstCbInfo->ulCurrPos ++;
		}
	}
	
	return FALSE;
}

STATIC HERROR rest_bind_tunable_FindRemovableActions (restBindTsTunableCb_t *pstCbInfo, HUINT32 ulTotalActionNum)
{
	HUINT32					 ulActionId, ulPriority;
	HERROR					 hErr;
	
	pstCbInfo->ulRemovableActNum = 0;

	for (ulActionId = 0; ulActionId < ulTotalActionNum; ulActionId++)
	{
		ulPriority = 0xFFFF;
		hErr = APK_RSC_GetActionPriority(ulActionId, &ulPriority);

		if ((ERR_OK == hErr) && (ulPriority < pstCbInfo->ulTrialActionPriority))
		{
			restBindActInfo_t		*pstRemoveAct = &(pstCbInfo->pstRemovableActs[pstCbInfo->ulRemovableActNum]);

			pstRemoveAct->ulActionId = ulActionId;
			pstRemoveAct->ulActionPriority = ulPriority;
			
			pstCbInfo->ulRemovableActNum ++;
		}
	}

	return ERR_OK;
}

STATIC void rest_bind_tunable_CbRscNotifier (OxRsc_Event_e eEvent, OxRsc_NotifierData_t *pstNotifyData, void *pvUserData)
{
	STATIC HBOOL			 s_bActivated = FALSE;

	if (eOxRSC_EVENT_ACTIVATED == eEvent)
	{
		s_bActivated = TRUE;
	}

	if (TRUE != s_bActivated)
	{
		return;
	}
	
	switch (eEvent)
	{
	case eOxRSC_EVENT_RSC_TAKEN:
	case eOxRSC_EVENT_RSC_RELEASED:
	case eOxRSC_EVENT_PRIORITY_CHANGED:
		rest_bind_tunable_CheckTunableTps();
		break;

	default:
		break;	
	}
}

#define _____CHECK_TASK_____
STATIC void rest_bind_tunable_IncreaseSession (void)
{
	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();

	pstContext->ulLatestCheckSession ++;
	if (pstContext->ulLatestCheckSession >= 0xFFFFFFFE)
	{
		pstContext->ulLatestCheckSession = 1;
	}
}

STATIC HERROR rest_bind_tunable_CbNotifyTunable (void *pvArg)
{
	HINT32						 nJWriter = 0;
	HUINT32						 ulSession = (HUINT32)pvArg;
	HCHAR						*szPayload = NULL;
	RestBindChanContext_t		*pstContext = rest_bind_chan_GetContext();
	HERROR						 hResult = ERR_FAIL;

	if (ulSession != pstContext->ulLatestCheckSession)
	{
		HxLOG_Error("Latest Session changed!!!");
		return ERR_FAIL;
	}

	HxSEMT_Get(pstContext->ulSemForTunable);

	if ((NULL == pstContext->pstTunableArray) || (0 == pstContext->ulTunableInfoNum))
	{
		HxLOG_Error("No Tunable Information at all...\n");

		szPayload = rest_bind_MakeErrorString(REST_ERR_CODE_NO_TUNABLE_INFO, REST_ERR_STRING_NO_TUNABLE_INFO, TRUE);
		goto END_FUNC;
	}

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		szPayload = rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
		goto END_FUNC;
	}

	// Jsonize the state
	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Event
		(void)HxJSONWRITER_String(nJWriter, "event", "tunableChanged");
		
		// Data
		(void)HxJSONWRITER_Object(nJWriter, "data");
		{
			// Transponders Tunable:
			(void)rest_datatype_JsonizeTpTunable(nJWriter, pstContext->pstTunableArray, pstContext->ulTunableInfoNum, "tunableTpUids", NULL);
		}
		(void)HxJSONWRITER_ObjectEnd(nJWriter);
		
		// Error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szPayload = rest_bind_CloseJsonWriter(nJWriter);
	hResult = ERR_OK;

END_FUNC:
	HxSEMT_Release(pstContext->ulSemForTunable);

	if (NULL != szPayload)
	{
		rest_bind_chan_ExecNotifier("transponder", szPayload);
		rest_datatype_AppendStringBufferToTemporary(szPayload);
	}
	
	return hResult;
}

STATIC HERROR rest_bind_tunable_ExecCheckTunable (restTuneCheckMsg_t *pstChkMsg)
{
	STATIC HUINT32				 s_ulActionNum = 0;
	STATIC restBindActInfo_t	*s_pstRemovableActArray = NULL;
	HBOOL						 bNotify = TRUE;
	HUINT32						 ulCheckCnt, ulTunableInfoNum;
	HUINT32						 ulMainViewActionId = 0, ulEmptyRecActionId = 0, ulEmptyLiveStreamActionId = 0;
	RestBindChanContext_t		*pstContext = rest_bind_chan_GetContext();
	OxRsc_InitialInfo_t 		 stInitInfo;
	restBindTsTunableCb_t		 stCbInfo;
	HERROR						 hErr;

	if ((NULL == pstContext) || (NULL == pstChkMsg))
	{
		HxLOG_Critical("!!! Context NULL or Msg NULL !!!\n");
		return ERR_FAIL;
	}

	if (pstChkMsg->ulSession != pstContext->ulLatestCheckSession)
	{
		HxLOG_Error("Session wrong: MsgSession(%d), CurrentSession(%d)\n", pstChkMsg->ulSession, pstContext->ulLatestCheckSession);
		return ERR_FAIL;
	}

	if (NULL == pstChkMsg->pstTsVector)
	{
		HxLOG_Error("TS Vector NULL:\n");
		return ERR_FAIL;
	}
	
	hErr = APK_RSC_GetInitialInfo(&stInitInfo);
	if ((ERR_OK != hErr) || (0 == stInitInfo.ulTotalActionNum))
	{
		HxLOG_Error("APK_RSC_GetInitialInfo err\n");
		return ERR_FAIL;
	}
	
	if ((0 == s_ulActionNum) || (NULL == s_pstRemovableActArray))
	{
		restBindActInfo_t			*pstActArray;

		pstActArray = (restBindActInfo_t *)HLIB_STD_MemAlloc(sizeof(restBindActInfo_t) * stInitInfo.ulTotalActionNum);
		if (NULL == pstActArray)
		{
			HxLOG_Error("Memory allocation failed:\n");
			return ERR_FAIL;
		}

		s_ulActionNum = stInitInfo.ulTotalActionNum;
		s_pstRemovableActArray = pstActArray;
	}

	HxSTD_MemSet(s_pstRemovableActArray, 0, sizeof(restBindActInfo_t) * s_ulActionNum);
	
	ulTunableInfoNum = HLIB_VECTOR_Length(pstChkMsg->pstTsVector);
	if (0 == ulTunableInfoNum)
	{
		HxLOG_Error("No TS... error:\n");
		return ERR_FAIL;
	}

	// Clear the tunable information in the context
	if ((pstContext->ulUpdatingTunableNum == ulTunableInfoNum) && (NULL != pstContext->pstUpdatingTunableArray))
	{
		HxSTD_MemSet(pstContext->pstUpdatingTunableArray, 0, sizeof(RestTsTunable_t) * ulTunableInfoNum);
	}
	else
	{
		pstContext->ulUpdatingTunableNum = 0;
		if (NULL != pstContext->pstUpdatingTunableArray)
		{
			HLIB_STD_MemFree(pstContext->pstUpdatingTunableArray);
			pstContext->pstUpdatingTunableArray = NULL;
		}

		pstContext->pstUpdatingTunableArray = (RestTsTunable_t *)HLIB_STD_MemCalloc(sizeof(RestTsTunable_t) * ulTunableInfoNum);
		if (NULL == pstContext->pstUpdatingTunableArray)
		{
			HxLOG_Error("Memory allocation failed:\n");
			return ERR_FAIL;
		}

		pstContext->ulUpdatingTunableNum = ulTunableInfoNum;
	}

	ulMainViewActionId = stInitInfo.ulMainViewActId;
	(void)APK_RSC_GetEmptyRecordActionId(&ulEmptyRecActionId);
	(void)APK_RSC_GetEmptyBgActionId(&ulEmptyLiveStreamActionId);

	HxSTD_MemSet(&stCbInfo, 0, sizeof(restBindTsTunableCb_t));
	stCbInfo.ulStartedSession	= pstContext->ulLatestCheckSession;
	stCbInfo.ulTunableNum		= ulTunableInfoNum;
	stCbInfo.pstTunableArray	= pstContext->pstUpdatingTunableArray;
	stCbInfo.pstRemovableActs	= s_pstRemovableActArray;

	// Live Check:
	stCbInfo.eDest = eTUNE_CHECK_DST_LiveMain;
	stCbInfo.ulCurrPos = 0;
	stCbInfo.ulTrialActionId = ulMainViewActionId;
	stCbInfo.ulTrialActionPriority = stInitInfo.ulViewActPriority;
	(void)rest_bind_tunable_FindRemovableActions(&stCbInfo, s_ulActionNum);

	ulCheckCnt = HLIB_VECTOR_ForEach(pstChkMsg->pstTsVector, rest_bind_tunable_CbCheckTunable, (void *)&stCbInfo);
	if (ulCheckCnt < ulTunableInfoNum)
	{
		HxLOG_Error("Serial execution [eTUNE_CHECK_DST_LiveMain] halted: return");
		return ERR_FAIL;
	}

	// Record Check:
	stCbInfo.eDest = eTUNE_CHECK_DST_Rec;
	stCbInfo.ulCurrPos = 0;
	stCbInfo.ulTrialActionId = ulEmptyRecActionId;
	stCbInfo.ulTrialActionPriority = stInitInfo.ulRecActPriority;
	(void)rest_bind_tunable_FindRemovableActions(&stCbInfo, s_ulActionNum);

	ulCheckCnt = HLIB_VECTOR_ForEach(pstChkMsg->pstTsVector, rest_bind_tunable_CbCheckTunable, (void *)&stCbInfo);
	if (ulCheckCnt < ulTunableInfoNum)
	{
		HxLOG_Error("Serial execution [eTUNE_CHECK_DST_Rec] halted: return");
		return ERR_FAIL;
	}

	// Live Stream Check:
	stCbInfo.eDest = eTUNE_CHECK_DST_LiveStream;
	stCbInfo.ulCurrPos = 0;
	stCbInfo.ulTrialActionId = ulEmptyLiveStreamActionId;
	stCbInfo.ulTrialActionPriority = stInitInfo.ulLsActPriority;
	(void)rest_bind_tunable_FindRemovableActions(&stCbInfo, s_ulActionNum);

	ulCheckCnt = HLIB_VECTOR_ForEach(pstChkMsg->pstTsVector, rest_bind_tunable_CbCheckTunable, (void *)&stCbInfo);
	if (ulCheckCnt < ulTunableInfoNum)
	{
		HxLOG_Error("Serial execution [eTUNE_CHECK_DST_Rec] halted: return");
		return ERR_FAIL;
	}

	HxSEMT_Get(pstContext->ulSemForTunable);
	{
		HUINT32 				 ulSwapInfoNum;
		RestTsTunable_t 		*pstSwapArray;

		if ((NULL != pstContext->pstTunableArray) && (pstContext->ulUpdatingTunableNum == pstContext->ulTunableInfoNum))
		{
			if (HxSTD_MemCmp(pstContext->pstTunableArray, pstContext->pstUpdatingTunableArray, sizeof(RestTsTunable_t) * pstContext->ulTunableInfoNum) == 0)
			{
				bNotify = FALSE;
			}
		}

		ulSwapInfoNum = pstContext->ulTunableInfoNum;
		pstSwapArray = pstContext->pstTunableArray;

		pstContext->ulTunableInfoNum = pstContext->ulUpdatingTunableNum;
		pstContext->pstTunableArray = pstContext->pstUpdatingTunableArray;
		
		pstContext->ulUpdatingTunableNum = ulSwapInfoNum;
		pstContext->pstUpdatingTunableArray = pstSwapArray;
	}
	HxSEMT_Release(pstContext->ulSemForTunable);

	if (TRUE == bNotify)
	{
		hErr = rest_task_RequestToExecute(rest_bind_tunable_CbNotifyTunable, (void *)pstChkMsg->ulSession);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("rest_task_RequestToExecute err:\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;	
}

STATIC void rest_bind_tunable_MainTask (void *pvArg)
{
	restTuneCheckMsg_t		 stMsg;
	HERROR					 hErr;
	
	while (1)
	{
		HxSTD_MemSet(&stMsg, 0, sizeof(restTuneCheckMsg_t));
		hErr = HLIB_MSG_Receive(s_ulRestTuneCheckTask_MsgQid, &stMsg, sizeof(restTuneCheckMsg_t));
		if (ERR_OK == hErr)
		{
			hErr = rest_bind_tunable_ExecCheckTunable(&stMsg);
		}

		if (NULL != stMsg.pstTsVector)
		{
			HLIB_VECTOR_Delete(stMsg.pstTsVector);
		}
	}
}


#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_tunable_Init (void)
{
	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();
	HERROR					 hErr;
	
	hErr = HLIB_MSG_Create(32, sizeof(restTuneCheckMsg_t), "restTuneChkQ", &s_ulRestTuneCheckTask_MsgQid, eHLIB_SUSPENDTYPE_FIFO);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("!!! HLIB_MSG_Create failed !!!\n");
		return ERR_FAIL;
	}

	// Create the main task
	VK_TASK_Create(rest_bind_tunable_MainTask, 40, (64 * 1024), "restTuneChk_task", NULL, &s_ulRestTuneCheckTask_TaskId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("!!! VK_TASK_Create failed !!!\n");
		return ERR_FAIL;
	}

	// Start the task
	hErr = VK_TASK_Start(s_ulRestTuneCheckTask_TaskId);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("!!! VK_TASK_Start failed !!!\n");
		return ERR_FAIL;
	}

	// Make the semaphore for the tunable...
	hErr = HxSEMT_Create(&(pstContext->ulSemForTunable), "restTunable", HxSEMT_FIFO);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("!!! HxSEMT_Create failed !!!\n");
		return ERR_FAIL;
	}

	(void)APK_RSC_Init();
	(void)APK_RSC_SetListener(rest_bind_tunable_CbRscNotifier, NULL);
	
	return ERR_OK;
}

STATIC void	*rest_bind_tunable_CloneTsInfo (const void *pvSelf)
{
	void				*pvTsInfo = NULL;
	
	if (NULL != pvSelf)
	{
		pvTsInfo = (void *)HLIB_STD_MemAlloc(sizeof(DxTransponder_t));
		if (NULL != pvTsInfo)
		{
			HxSTD_MemCopy(pvTsInfo, pvSelf, sizeof(DxTransponder_t));
		}
		
	}

	return pvTsInfo;
}

HERROR rest_bind_tunable_CheckTunableTps (void)
{
	RestBindChanContext_t		*pstContext = rest_bind_chan_GetContext();
	restTuneCheckMsg_t			 stTuneCheckMsg;
	HERROR						 hErr;

	if ((NULL == pstContext) || (NULL == pstContext->pstTsVector))
	{
		HxLOG_Error("Context or TS Vector NULL:\n");
		return ERR_FAIL;
	}

	rest_bind_tunable_IncreaseSession();

	HxSTD_MemSet(&stTuneCheckMsg, 0, sizeof(restTuneCheckMsg_t));

	stTuneCheckMsg.ulSession = pstContext->ulLatestCheckSession;
	stTuneCheckMsg.pstTsVector = HLIB_VECTOR_CloneEx(pstContext->pstTsVector, (void *(*)(void *))rest_bind_tunable_CloneTsInfo);

	hErr = HLIB_MSG_Send(s_ulRestTuneCheckTask_MsgQid, (const void *)&stTuneCheckMsg, sizeof(restTuneCheckMsg_t));
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("\n\n\n!!!!!!!!! TUNE CHECK TASK MSG failed !!!!!!!!!!!\n\n\n");
	}

	return ERR_OK;
}

