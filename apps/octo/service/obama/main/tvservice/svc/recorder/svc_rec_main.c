/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_rec_main.c
	@brief

	Description:  									\n
	Module: SVC / REC								\n

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
/********************	   Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <vkernel.h>
#include <octo_common.h>

#include <pal_pipe.h>
#include <pal_rec.h>
#include <pal_ringbuf.h>

//#include <mwc_util.h>

#include <svc_rec.h>
#include "./local_include/_svc_rec.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	svcRec_ContextState_e	eState;

	Handle_t				hAction;

	void					 (*pfAction)(SvcRec_Msg_t *pstMsg, void *pvData);
	void					*pvData;
	unsigned long			 ulSyncSem;
} svcRec_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC unsigned long			 s_ulSvcRec_TaskId	= 0, ulSvcRec_MsgQId = 0;
STATIC svcRec_Context_t*		 s_pSvcRec_Contexts	= NULL;
STATIC HINT32					 s_nRec_ContextCnt	= 0;

#define ___LOCAL_FUNCTIONS___
STATIC INLINE unsigned long svc_rec_main_GetRecMsgQueueId (void)
{
	return ulSvcRec_MsgQId;
}

STATIC INLINE HINT32 svc_rec_main_GetContextCnt(void)
{
	return s_nRec_ContextCnt;
}

STATIC INLINE svcRec_Context_t *svc_rec_main_GetContext (HUINT32 ulIndex)
{
	if( NULL == s_pSvcRec_Contexts )
		return NULL;

	return (ulIndex < s_nRec_ContextCnt) ? &(s_pSvcRec_Contexts[ulIndex]) : NULL;
}

STATIC INLINE Handle_t svc_rec_main_GetHandleFromResourceId (HUINT32 ulResourceId, svcRec_ResType_e	eResType)
{
	if( NULL == s_pSvcRec_Contexts )
		return HANDLE_NULL;

	{
		HINT32 i;
		Handle_t hAction = HANDLE_NULL;

		for( i=0 ; i<s_nRec_ContextCnt ; i++ )
		{
			if( NULL == s_pSvcRec_Contexts[i].pvData )
				continue;

			hAction = svc_rec_GetActionHandleFromResourceId( 	s_pSvcRec_Contexts[i].pvData,
																	ulResourceId,
																	eResType);

			if( (HANDLE_NULL != hAction) && (0 != hAction) )
			{
				return hAction;
			}
		}
	}

	return HANDLE_NULL;
}

STATIC svcRec_Context_t *svc_rec_main_GetContextFromActionHandle (Handle_t hAction)
{
	HUINT32 i=0;

	if( NULL == s_pSvcRec_Contexts )
		return NULL;

	/* check hAction */
	for( i=0 ; i< s_nRec_ContextCnt ; i++ )
	{
		if( s_pSvcRec_Contexts[i].hAction == hAction )
		{
			return &s_pSvcRec_Contexts[i];
		}
	}

	return NULL;
}

STATIC HINT32 svc_rec_main_GetContextIndex (Handle_t hAction)
{
	HUINT32 i=0;
	HUINT32 nCtxCnt = 0;

	nCtxCnt = svc_rec_main_GetContextCnt();

	/* check hAction */
	for( i=0 ; i< nCtxCnt ; i++ )
	{
		svcRec_Context_t *pstContext = svc_rec_main_GetContext(i);

		if( pstContext->hAction == hAction )
		{
			return i;
		}
	}

	return -1;
}

#define _____MSG_ARGUMENT_FUNCTIONS_____
STATIC HERROR svc_rec_main_GetSetupMsgArgPosAndSize (SvcRec_Msg_t *pstMsg, svcRec_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eRECMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSetup.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eRECMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSetup.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eRECMSG_ARGTYPE_Setup:
	case eRECMSG_ARGTYPE_UpdateVideoCodec:
	case eRECMSG_ARGTYPE_SetEncryption:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSetup.stSetup));
		*pulArgSize = sizeof(SvcRec_Setup_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_rec_main_GetSetPidMsgArgPosAndSize (SvcRec_Msg_t *pstMsg, svcRec_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eRECMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stPIDList.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eRECMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stPIDList.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eRECMSG_ARGTYPE_SetPid:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stPIDList.stPIDList));
		*pulArgSize = sizeof(SvcRec_PIDList_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_rec_main_GetSetupDelayedMsgArgPosAndSize (SvcRec_Msg_t *pstMsg, svcRec_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eRECMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stDelayedSetup.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eRECMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stDelayedSetup.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case eRECMSG_ARGTYPE_SetupDelayed:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stDelayedSetup.stDelayedSetup));
		*pulArgSize = sizeof(SvcRec_DelayedRecSetup_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_rec_main_GetBaseMsgArgPosAndSize (SvcRec_Msg_t *pstMsg, svcRec_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eRECMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stBase.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case eRECMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stBase.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_rec_main_GetEvtRecordMsgArgPosAndSize (SvcRec_Msg_t *pstMsg, svcRec_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case eRECMSG_ARGTYPE_RecEvtParam:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stEvtParam.stEvtParam));
		*pulArgSize = sizeof(SvcRec_EvtParam_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_rec_main_GetMsgArgPosAndSize (SvcRec_Msg_t *pstMsg, svcRec_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (pstMsg->eMsgClass)
	{
	case eRECMSG_CMD_SETUP:
	case eRECMSG_CMD_UPDATE_VIDEOCODEC:
	case eRECMSG_CMD_SET_ENCRYPTION:
		return svc_rec_main_GetSetupMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);
	case eRECMSG_CMD_SET_PID:
		return svc_rec_main_GetSetPidMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);
	case eRECMSG_CMD_STARTDELAYED:
		return svc_rec_main_GetSetupDelayedMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	/*BASE MSG*/
	case eRECMSG_CMD_START:
	case eRECMSG_CMD_PAUSE:
	case eRECMSG_CMD_RESUME:
	case eRECMSG_CMD_STOP:
	case eRECMSG_CMD_CLOSE:
		return svc_rec_main_GetBaseMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	case eRECMSG_CMD_EVT_RECORD:
		return svc_rec_main_GetEvtRecordMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	default:
		break;
	}

	// Rec Section Callback Messages:

	// Nothing:
	return ERR_FAIL;
}

#define _____MSG_RELATED_FUNCTIONS_____
STATIC HERROR svc_rec_main_MakeStopMsgByNextStartMsg (svcRec_Context_t *pstContext, SvcRec_Msg_t *pstStartMsg, SvcRec_Msg_t *pstStopMsg)
{
	HxSTD_MemSet (pstStopMsg, 0, sizeof(SvcRec_Msg_t));

	pstStopMsg->hAction = pstContext->hAction;

	switch (pstStartMsg->eMsgClass)
	{
	case eRECMSG_CMD_START:
		pstStopMsg->eMsgClass				=	eRECMSG_CMD_CLOSE;
		pstStopMsg->unArg.stBase.bSyncMode	=	TRUE;
		break;

	default:
		HxLOG_Error ("the msg (%d) has no pairing stop message:\n", pstStartMsg->eMsgClass);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_rec_main_ExecuteContextAction (svcRec_Context_t *pstContext, SvcRec_Msg_t *pstMsg)
{
	if (NULL == pstContext->pfAction)				{ return ERR_FAIL; }

	pstContext->pfAction (pstMsg, pstContext->pvData);
	return ERR_OK;
}

#define _____ACTION_ALLOCATION_____
STATIC HERROR svc_rec_main_ResetContext (svcRec_Context_t *pstContext)
{
	pstContext->pfAction		= NULL;
	if (NULL != pstContext->pvData)
	{
		svc_rec_FreeActionData ( pstContext->pvData);
		pstContext->pvData = NULL;
	}

	pstContext->hAction = HANDLE_NULL;

	return ERR_OK;
}

STATIC HERROR svc_rec_main_InitContext (svcRec_Context_t *pstContext, Handle_t hAction, SvcRec_Msg_t *pstMsg)
{
	pstContext->hAction = hAction;

	pstContext->pfAction = svc_rec_GetActionFunc ();
	if (NULL == pstContext->pfAction)
	{
		HxLOG_Critical ("svc_rec_GetActionFunc return NULL:\n");
		goto ERROR;
	}

	pstContext->pvData = svc_rec_AllocActionData ();
	if (NULL == pstContext->pvData)
	{
		HxLOG_Critical ("svc_rec_AllocActionData return NULL:\n");
		goto ERROR;
	}
#if 0
	/* register callback */
	{
		HUINT32			ulActionId;
		HUINT32			ulRecordId;

		ulActionId = PAL_PIPE_GetActionId( pstMsg->hAction );
		PAL_PIPE_GetResourceId( ulActionId, eRxRSCTYPE_HDD_REC, &ulRecordId);

		switch(pstMsg->unArg.stSetup.stSetup.eRecActType)
		{
			case eRec_ActType_Satip:
						PAL_REC_RegisterMemoryCallback (ulRecordId, svc_rec_main_MemIOCallback_t);
						break;
			default:	PAL_REC_RegisterNotifier (ulRecordId, svc_rec_main_Callback_t);
						break;
		}
	}
#endif
	return ERR_OK;

ERROR:
	svc_rec_main_ResetContext (pstContext);
	return ERR_FAIL;
}

#define _____CMD_FUNCTIONS_____
STATIC HERROR svc_rec_main_CmdCloseSession (svcRec_Context_t *pstContext, SvcRec_Msg_t *pstMsg)
{
	Handle_t			 hAction = pstMsg->hAction;
	HERROR				 hResult = ERR_FAIL;

	HxLOG_Info ("MsgClass(%d), Status(%d)\n", pstMsg->eMsgClass, pstContext->eState);

	switch (pstContext->eState)
	{
	case eSvcRec_CtxtState_None:
		pstContext->hAction = hAction;
		hResult = ERR_OK;
		break;

	case eSvcRec_CtxtState_Waiting:
		pstContext->hAction = hAction;
		if (NULL != pstContext->pfAction)
		{
			svc_rec_main_ExecuteContextAction (pstContext, pstMsg);
		}

		hResult = ERR_OK;
		break;

	case eSvcRec_CtxtState_Running:
		pstContext->hAction	= hAction;
		pstContext->eState	= eSvcRec_CtxtState_Waiting;

		if (NULL != pstContext->pfAction)
		{
			svc_rec_main_ExecuteContextAction (pstContext, pstMsg);
		}
		else
		{
			HxLOG_Critical ("!!! State Running but No Action !!!\n");
		}
		svc_rec_main_ResetContext (pstContext);

		hResult = ERR_OK;
		break;

	default:
		hResult = ERR_FAIL;
		break;
	}

	/* free resources */
	if( NULL == pstContext->pvData )
	{
		svc_rec_FreeActionData (pstContext->pvData);
		pstContext->pvData = NULL;
	}

//	MWC_UTIL_PostMsgToAp (EVT_REC_STOP, hAction, 0, 0, 0);
	return hResult;
}

STATIC HERROR svc_rec_main_CmdOpenSession (svcRec_Context_t *pstContext, SvcRec_Msg_t *pstMsg)
{
	Handle_t				 hAction = pstMsg->hAction;
	SvcRec_Msg_t 			*pstStopMsg = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	HxLOG_Info ("MsgClass(%d), Status(%d)\n", pstMsg->eMsgClass, pstContext->eState);

	switch (pstContext->eState)
	{
	case eSvcRec_CtxtState_None:
		hErr = svc_rec_main_InitContext (pstContext, hAction, pstMsg);
		if (ERR_OK == hErr)
		{
			pstContext->hAction = hAction;
			pstContext->eState = eSvcRec_CtxtState_Waiting;

			hResult = svc_rec_main_CmdOpenSession (pstContext, pstMsg);
		}

		break;

	case eSvcRec_CtxtState_Waiting:
		if (NULL == pstContext->pfAction)
		{
			HxLOG_Critical ("Action Function shall be set in the waiting state!!!\n");
			return ERR_FAIL;
		}

		/* check !!! actiontype, plugin */

		{
			pstContext->hAction	= hAction;
			pstContext->eState	= eSvcRec_CtxtState_Running;
			svc_rec_main_ExecuteContextAction (pstContext, pstMsg);
			hResult = ERR_OK;
		}

		break;

	case eSvcRec_CtxtState_Running:
		pstStopMsg = (SvcRec_Msg_t *)OxMW_Calloc (sizeof(SvcRec_Msg_t));
		if (NULL != pstStopMsg)
		{
			hErr = svc_rec_main_MakeStopMsgByNextStartMsg (pstContext, pstMsg, pstStopMsg);
			if (ERR_OK == hErr)
			{
				svc_rec_main_CmdCloseSession (pstContext, pstStopMsg);
			}

			OxMW_Free (pstStopMsg);
		}

		if (eSvcRec_CtxtState_Running != pstContext->eState)
		{
			hResult = svc_rec_main_CmdOpenSession (pstContext, pstMsg);
		}

		break;

	default:
		hResult = ERR_FAIL;
		break;
	}

	return hResult;
}

STATIC void svc_rec_main_Task(void *pvArg)
{
	HBOOL					 bSync;
	HINT32					 nContextId;
	HERROR					*phResult;
	SvcRec_Msg_t			 stMsg;
	svcRec_Context_t		*pstContext;
	HERROR					 hErr, hResult;

	while (1)
	{
		hErr = VK_MSG_Receive (svc_rec_main_GetRecMsgQueueId(), &stMsg, sizeof (SvcRec_Msg_t));
		if (VK_OK != hErr)					{ continue; }

		nContextId = svc_rec_GetContextIndex(stMsg.hAction);
		if( -1 == nContextId )
			continue;

		pstContext = svc_rec_main_GetContext (nContextId);
		if (NULL == pstContext)
		{
			bSync = FALSE;
			phResult = NULL;

			hErr = svc_rec_GetMsgArg (&stMsg, eRECMSG_ARGTYPE_ApiResultPtr, (void *)&phResult);
			if ((ERR_OK == hErr) && (NULL != phResult))
			{
				*phResult = ERR_FAIL;
			}

			hErr = svc_rec_GetMsgArg (&stMsg, eRECMSG_ARGTYPE_SyncMode, (void *)&bSync);
			if ((ERR_OK == hErr) && (TRUE == bSync))
			{
				svc_rec_ReleaseSyncSem (0);
			}

			continue;
		}

		hResult = ERR_FAIL;

		switch (stMsg.eMsgClass)
		{
		case eRECMSG_CMD_SETUP:
			if (PAL_PIPE_IsActionHandleLatest (stMsg.hAction) == ERR_OK)
			{
				pstContext->eState = eSvcRec_CtxtState_None;
				hResult = svc_rec_main_CmdOpenSession (pstContext, &stMsg);
			}
			break;

		case eRECMSG_CMD_CLOSE:
			hResult = svc_rec_main_CmdCloseSession (pstContext, &stMsg);
			break;

		default:
			HxLOG_Debug("eState [%d] - [%p, %p]\n", pstContext->eState, stMsg.hAction, pstContext->hAction);
			if ((eSvcRec_CtxtState_Running == pstContext->eState) && (stMsg.hAction == pstContext->hAction))
			{
				hResult = svc_rec_main_ExecuteContextAction (pstContext, &stMsg);
			}
			else
			{
				hResult = ERR_FAIL;
			}

			break;
		}

		phResult = NULL;
		hErr = svc_rec_GetMsgArg (&stMsg, eRECMSG_ARGTYPE_ApiResultPtr, (void *)&phResult);
		if ((ERR_OK == hErr) && (NULL != phResult))
		{
			*phResult = hResult;
		}

		bSync = FALSE;
		hErr = svc_rec_GetMsgArg (&stMsg, eRECMSG_ARGTYPE_SyncMode, (void *)&bSync);
		if ((ERR_OK == hErr) && (TRUE == bSync))
		{
			svc_rec_ReleaseSyncSem (nContextId);
		}
	}
}


#define ___MEMBER_FUNCTIONS___
HERROR svc_rec_InitTask(void)
{
	HUINT32					 ulCnt;
	HCHAR					 szSemName[16];
	HERROR					 hErr;

	hErr = VK_MSG_Create (MW_PVR_MSGQ_SIZE, sizeof(SvcRec_Msg_t), "SvcRecQ", &ulSvcRec_MsgQId, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_MSG_Create error:\n");
		return ERR_FAIL;
	}

	hErr = VK_TASK_Create (svc_rec_main_Task, MW_PVR_RECORDER_TASK_PRIORITY, MW_PVR_STACK_SIZE, "SvcRecTask", NULL, &s_ulSvcRec_TaskId, 0);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_TASK_Create error:\n");
		return ERR_FAIL;
	}

	/*init contexts*/
	{
		/*get number of rec*/
		if( (NULL == s_pSvcRec_Contexts) && (0 == s_nRec_ContextCnt) )
		{
			HUINT32 ulNumDevice = 0;
			hErr = PAL_REC_GetCapability (&ulNumDevice);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("PAL_REC_GetCapability error:\n");
				return ERR_FAIL;
			}

			s_nRec_ContextCnt	= ulNumDevice;
			s_pSvcRec_Contexts	= (svcRec_Context_t*)OxMW_Malloc( sizeof(svcRec_Context_t) * ulNumDevice );
			if( NULL == s_pSvcRec_Contexts )
			{
				HxLOG_Error ("OxMW_Malloc error:\n");
				return ERR_FAIL;
			}
		}
		else
		{
			HxLOG_Error ("Already alloc contexts error:\n");
			return ERR_FAIL;
		}

		for (ulCnt = 0; ulCnt < s_nRec_ContextCnt; ulCnt++)
		{
			svcRec_Context_t *pstContext = svc_rec_main_GetContext (ulCnt);

			HxSTD_MemSet (pstContext, 0, sizeof(svcRec_Context_t));

			pstContext->hAction			= HANDLE_NULL;

			HxSTD_PrintToStrN (szSemName, 16, "SvcRecSync%02d", ulCnt);

			hErr = VK_SEM_Create (&(pstContext->ulSyncSem), (const char *)szSemName, VK_SUSPENDTYPE_PRIORITY);
			if (ERR_OK == hErr)
			{
				svc_rec_GetSyncSem (ulCnt);
			}
			else
			{
				HxLOG_Error ("VK_SEM_Create err: Sem(%d) failed\n", ulCnt);
			}
		}
	}

	// Init the other REC sub modules:
//	svc_rec_cb_Init();

	/* Start REC Task */
	VK_TASK_Start (s_ulSvcRec_TaskId);
	return ERR_OK;
}

#ifdef _SVC_REC_SYNC_SEM_DEBUG_
void _svc_rec_GetSyncSem (HUINT32 ulIndex, const HINT8 *szFunction, HINT32 nLine)
{
	svcRec_Context_t 	*pstContext = svc_rec_main_GetContext (ulIndex);
	HINT32				i;

	for(i = 0; i < ulIndex; i++)
	{
		HLIB_CMD_Printf("\t");
	}
	HLIB_CMD_Printf("++ _svc_rec_GetSyncSem: ");

	if ((NULL != pstContext) && (0 != pstContext->ulSyncSem))
	{
		HLIB_CMD_Printf("ulIndex(%d), [%s:%d]\n", ulIndex, szFunction, nLine);
		VK_SEM_Get (pstContext->ulSyncSem);
	}
	else
	{
		if(NULL == pstContext)
		{
			HLIB_CMD_Printf("ulIndex(%d), pstContext is NULL [%s:%d]\n", ulIndex, szFunction, nLine);
		}
		else
		{
			HLIB_CMD_Printf("ulIndex(%d), pstContext->ulSyncSem(0x%x) [%s:%d]\n", ulIndex, pstContext->ulSyncSem, szFunction, nLine);
		}
	}
}

void _svc_rec_ReleaseSyncSem (HUINT32 ulIndex, const HINT8 *szFunction, HINT32 nLine)
{
	svcRec_Context_t	*pstContext = svc_rec_main_GetContext (ulIndex);
	HINT32				i;

	for(i = 0; i < ulIndex; i++)
	{
		HLIB_CMD_Printf("\t");
	}
	HLIB_CMD_Printf("-- _svc_rec_ReleaseSyncSem: ");

	if ((NULL != pstContext) && (0 != pstContext->ulSyncSem))
	{
		HLIB_CMD_Printf("ulIndex(%d), [%s:%d]\n", ulIndex, szFunction, nLine);
		VK_SEM_Release (pstContext->ulSyncSem);
	}
	else
	{
		if(NULL == pstContext)
		{
			HLIB_CMD_Printf("ulIndex(%d), pstContext is NULL [%s:%d]\n", ulIndex, szFunction, nLine);
		}
		else
		{
			HLIB_CMD_Printf("ulIndex(%d), pstContext->ulSyncSem(0x%x) [%s:%d]\n", ulIndex, pstContext->ulSyncSem, szFunction, nLine);
		}
	}
}
#else
void svc_rec_GetSyncSem (HUINT32 ulIndex)
{
	svcRec_Context_t 		*pstContext = svc_rec_main_GetContext (ulIndex);

	if ((NULL != pstContext) && (0 != pstContext->ulSyncSem))
	{
		VK_SEM_Get (pstContext->ulSyncSem);
	}
}

void svc_rec_ReleaseSyncSem (HUINT32 ulIndex)
{
	svcRec_Context_t			*pstContext = svc_rec_main_GetContext (ulIndex);

	if ((NULL != pstContext) && (0 != pstContext->ulSyncSem))
	{
		VK_SEM_Release (pstContext->ulSyncSem);
	}
}
#endif

HINT32 svc_rec_GetContextIndex (Handle_t hAction)
{
	return svc_rec_main_GetContextIndex (hAction);
}

HINT32 svc_rec_AllocContext (Handle_t hAction)
{
	HUINT32 i=0;
	HUINT32 nCtxCnt = 0;

	nCtxCnt = svc_rec_main_GetContextCnt();

	/* check hAction */
	for( i=0 ; i< nCtxCnt ; i++ )
	{
		svcRec_Context_t *pstContext = svc_rec_main_GetContext(i);

		if( pstContext->hAction == hAction )
		{
			HxLOG_Error ("Already alloced hAction : 0x%x error:\n", hAction);
			return -1;
		}
	}

	/* alloc */
	for( i=0 ; i< nCtxCnt ; i++ )
	{
		svcRec_Context_t *pstContext = svc_rec_main_GetContext(i);

		if( HANDLE_NULL == pstContext->hAction )
		{
			pstContext->hAction = hAction;
			return i;
		}
	}

	HxLOG_Error ("empty context not found  error:\n");
	return -1;
}

HERROR svc_rec_SendMsgToRecTask (SvcRec_Msg_t *pstMsg)
{
	int				 nRet;

	if (NULL == pstMsg)						{ return ERR_FAIL; }

	nRet = VK_MSG_Send (svc_rec_main_GetRecMsgQueueId(), pstMsg, sizeof(SvcRec_Msg_t));
	if (VK_OK != nRet)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_rec_GetMsgArg (SvcRec_Msg_t *pstMsg, svcRec_MsgArgType_e eArgType, void *pvData)
{
	HUINT32				 ulArgSize = 0;
	void				*pvSrcPos = NULL;
	HERROR				 hErr;

	hErr = svc_rec_main_GetMsgArgPosAndSize (pstMsg, eArgType, &pvSrcPos, &ulArgSize);
	if ((ERR_OK != hErr) || (NULL == pvSrcPos) || (0 == ulArgSize))
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy (pvData, pvSrcPos, ulArgSize);
	return ERR_OK;
}

HERROR svc_rec_ReplacePSI( Handle_t hAction, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize )
{
	svcRec_Context_t * pstContext = svc_rec_main_GetContextFromActionHandle(hAction);

	if( NULL == pstContext )
		return ERR_FAIL;

	return svc_rec_FuncReplacePSI(pstContext->pvData, usPid, pucSection, ulSectionSize );
}

HERROR svc_rec_CleanReqPidInfo(SvcRec_PidSetType_e ePidSetType, SvcRec_PIDList_t *pstReqInfo )
{
	svc_rec_FuncInitPid( pstReqInfo, ePidSetType );

	return ERR_OK;
}

HERROR svc_rec_CleanReqPidDes( SvcRec_PIDDes_t stPidDesArray[], HINT32 nArrayCnt )
{
	svc_rec_FuncInitPidDes(stPidDesArray, nArrayCnt);
	return ERR_OK;
}

HERROR svc_rec_GetInfo(Handle_t hAction, SvcRec_RecInfo *pstRecInfo)
{
	svcRec_Context_t * pstContext = svc_rec_main_GetContextFromActionHandle(hAction);

	if( NULL == pstContext )
		return ERR_FAIL;

	return svc_rec_FuncGetInfo(pstContext->pvData, pstRecInfo);
}

HBOOL svc_rec_IsRecording( HUINT8 *pucFileName )
{
	HINT32 i=0;

	for( i=0 ; i<svc_rec_main_GetContextCnt() ; i++ )
	{
		svcRec_Context_t * pstContext = svc_rec_main_GetContext(i);

		if( NULL != pstContext )
		{
			if( TRUE == svc_rec_FuncIsRecordingWithName( pstContext->pvData, pucFileName ) )
				return TRUE;
		}
	}

	return FALSE;
}

HBOOL svc_rec_IsAvailableDelayed( Handle_t hAction )
{
	svcRec_Context_t * pstContext = svc_rec_main_GetContextFromActionHandle(hAction);
	svcRec_BaseData_t	*pstData;

	if( NULL == pstContext )
		return ERR_FAIL;

	pstData = (svcRec_BaseData_t *)pstContext->pvData;

	if( NULL == pstData )
		return ERR_FAIL;

	if( eREC_STATE_Start != pstData->eState )
		return ERR_FAIL;

	return ERR_OK;
}

HERROR svc_rec_SetDRMInfo( Handle_t hAction, DxCopyrightControl_t *pstInfo, HBOOL bWriteTimeStamp )
{
	svcRec_Context_t * pstContext = svc_rec_main_GetContextFromActionHandle(hAction);

	if( NULL == pstContext )
		return ERR_FAIL;

	return svc_rec_FuncSetDRMInfo(pstContext->pvData, pstInfo, bWriteTimeStamp );
}

#define ___MSG_TASK___
#ifdef CONFIG_DEBUG
STATIC HUINT32	s_ulLastWriteTick;
#endif

void svc_rec_main_MemIOCallback_t(HUINT32 ulRecordId, HUINT8 *pucBuf, HUINT32 ulLength)
{
	Handle_t	hAction		= HANDLE_NULL;
	Handle_t	hMemoryBuf	= HANDLE_NULL;
	HERROR		hErr		= ERR_OK;
#ifdef CONFIG_DEBUG
	HUINT32		ulCurTick = 0, ulTimeGap = 0;
	HINT32		nKbps = 0;
#endif

	HxLOG_Print("[%s:%d] ulLength = %d \r\n", __FUNCTION__, __LINE__, ulLength);

	hAction = svc_rec_main_GetHandleFromResourceId( ulRecordId, eRecResType_RecordId );
	if (HANDLE_NULL != hAction)
	{
		svcRec_Context_t *pstContext = NULL;
		pstContext  = svc_rec_main_GetContextFromActionHandle(hAction);

		if (NULL != pstContext)
		{
			svcRec_BaseData_t *pstData = (svcRec_BaseData_t *)pstContext->pvData;
			hMemoryBuf = pstData->stSetupInfo.unOutArg.hMemoryBuf;

			if ((0<ulLength) && (HANDLE_NULL != hMemoryBuf))
			{
				HUINT32				 ulWrittenSize;


#if 0
				//if(ulLength < 50000)
				{
					HCHAR 		pszFileName[256] = {0, };
					HxFILE_t	*fp = NULL;

					HxSTD_snprintf(pszFileName, 255, "SATIP_obama_memio_%d_%d_%d.ts", (HINT32)hMemoryBuf, HLIB_STD_GetSystemTime(), ulLength);
					HxLOG_Warning("hMemoryBuf = %d, ulLength = %d, %s\n", hMemoryBuf, ulLength, pszFileName);

					fp = HLIB_FILE_Open(pszFileName, "wb");
					if(fp)
					{
						HLIB_FILE_Write(fp, (HUINT8 *)pucBuf, ulLength, 1);
						HLIB_FILE_Close(fp);
					}
				}
#endif


				hErr = PAL_RINGBUF_WriteData (hMemoryBuf, ulLength, pucBuf, &ulWrittenSize);

#ifdef CONFIG_DEBUG
				ulCurTick = HLIB_STD_GetSystemTick();
				ulTimeGap = ulCurTick - s_ulLastWriteTick;
				if(0 >= ulTimeGap)
				{
					ulTimeGap = 1;
				}
				nKbps = (ulLength / ulTimeGap) * 1000 / 1024; /* kbps */

				HxLOG_Print(HxANSI_COLOR_YELLOW("\n\t(1)[DATA_TRACE][OBAMA>WRITE TO RINGBUF(%d)] ulLength = %d, (%d kbps), ulWrittenSize = %d, tick = %d, gap = %d\r\n"),
					hMemoryBuf, ulLength, nKbps, ulWrittenSize, ulCurTick, ulTimeGap);
				s_ulLastWriteTick = ulCurTick;
#endif

				if (ERR_OK != hErr)
				{
					HxLOG_Error ("PAL_RINGBUF_WriteData failed: BufferHandle(0x%08x)\n", hMemoryBuf);
					return;
				}
			}
		}
	}

	return ;
}

void	svc_rec_main_Callback_t(HUINT32 ulEvent, HUINT32 ulRecordId, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HERROR	hErr				= ERR_FAIL;
	svcRec_ResType_e eResType	= eRecResType_RecordId;
	SvcRec_Msg_t stMsg;

	HxSTD_MemSet(&stMsg, 0, sizeof( SvcRec_Msg_t) );

	if( ePAL_REC_EVENT_DELAYED_RECORD_STATUS_CHANGED == ulEvent )
		eResType = eRecResType_DelayedId;

	stMsg.hAction = svc_rec_main_GetHandleFromResourceId( ulRecordId, eResType );

	if( HANDLE_NULL == stMsg.hAction )
	{
		HxLOG_Error ("event(0x%X) can't find hAction by ulRecordId(%d)\n", ulEvent, ulRecordId);
		return ;
	}

	stMsg.eMsgClass					= eRECMSG_CMD_EVT_RECORD;
	stMsg.unArg.stEvtParam.stEvtParam.ulParam1 = ulParam1;
	stMsg.unArg.stEvtParam.stEvtParam.ulParam2 = ulParam2;
	stMsg.unArg.stEvtParam.stEvtParam.ulParam3 = ulParam3;

	HxLOG_Debug("ulEvent : [%d]\n", ulEvent);
	switch( ulEvent )
	{
		case ePAL_REC_EVENT_TIMECODE_CHANGED:	stMsg.unArg.stEvtParam.stEvtParam.eType = eRECEVT_TIMECODE; 	break;
		case ePAL_REC_EVENT_DISKFULL:			stMsg.unArg.stEvtParam.stEvtParam.eType = eRECEVT_DISK_FULL; 	break;
		case ePAL_REC_EVENT_STREAM_UNSCRAMBLED:	stMsg.unArg.stEvtParam.stEvtParam.eType = eRECEVT_UNSCRAMBLED; break;
		case ePAL_REC_EVENT_STREAM_SCRAMBLED:	stMsg.unArg.stEvtParam.stEvtParam.eType = eRECEVT_SCRAMBLED; 	break;

		case ePAL_REC_EVENT_DELAYED_RECORD_STATUS_CHANGED:
			stMsg.unArg.stEvtParam.stEvtParam.eType	= eRECEVT_DELAYEDREC_CHANGED;
			stMsg.unArg.stEvtParam.stEvtParam.ulParam2	= ulRecordId;
			break;

		default:
			HxLOG_Error ("Unknown event(0x%X), ulInstance:0x%X\n", ulEvent, ulParam1);
			return;
	}

	hErr = svc_rec_SendMsgToRecTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_rec_SendMsgToSiTask err:\n");
		return ;
	}

	return ;
}

/* end of file */
