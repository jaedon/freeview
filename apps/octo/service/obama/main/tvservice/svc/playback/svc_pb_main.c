/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_pb_main.c
	@brief

	Description:  									\n
	Module: SVC / Playback							\n

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
/********************	   Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <vkernel.h>
#include <octo_common.h>

#include <pal_pipe.h>
#include <pal_pb.h>

//#include <mwc_util.h>

#include <svc_pb.h>
#include "./local_include/_svc_pb.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	svcPb_ContextState_e	eState;

	Handle_t				 hAction;
	void					 (*pfAction)(SvcPb_Msg_t *pstMsg, void *pvData);
	void					*pvData;
	unsigned long			 ulSyncSem;
} svcPb_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC unsigned long			 s_ulSvcPb_TaskId	= 0, ulSvcPb_MsgQId = 0;
STATIC svcPb_Context_t*		 s_pSvcPb_Contexts	= NULL;
STATIC HINT32					 s_nPb_ContextCnt	= 0;

#define ___LOCAL_FUNCTIONS___
STATIC INLINE unsigned long svc_pb_main_GetPbMsgQueueId (void)
{
	return ulSvcPb_MsgQId;
}

STATIC INLINE HINT32 svc_pb_main_GetContextCnt (void)
{
	return s_nPb_ContextCnt;
}

STATIC INLINE svcPb_Context_t *svc_pb_main_GetContext (HUINT32 ulIndex)
{
	if( NULL == s_pSvcPb_Contexts )
		return NULL;

	return (ulIndex < s_nPb_ContextCnt) ? &(s_pSvcPb_Contexts[ulIndex]) : NULL;
}

STATIC svcPb_Context_t *svc_pb_main_GetContextFromActionHandle (Handle_t hAction)
{
	HUINT32 i=0;

	if( NULL == s_pSvcPb_Contexts )
		return NULL;

	/* check hAction */
	for( i=0 ; i< s_nPb_ContextCnt ; i++ )
	{
		if( s_pSvcPb_Contexts[i].hAction == hAction )
		{
			return &s_pSvcPb_Contexts[i];
		}
	}

	return NULL;
}

STATIC INLINE Handle_t svc_pb_main_GetHandleFromResourceId (HUINT32 ulResourceId, svcPb_ResType_e eResType)
{
	if( NULL == s_pSvcPb_Contexts )
		return HANDLE_NULL;

	{
		HINT32 i;
		Handle_t hAction = HANDLE_NULL;

		for( i=0 ; i<s_nPb_ContextCnt ; i++ )
		{
			if( NULL == s_pSvcPb_Contexts[i].pvData )
				continue;

			hAction = svc_pb_GetActionHandleFromResourceId(s_pSvcPb_Contexts[i].pvData,
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

STATIC HINT32 svc_pb_main_GetContextIndex (Handle_t hAction)
{
	HUINT32 i=0;
	HUINT32 nCtxCnt = 0;

	nCtxCnt = svc_pb_main_GetContextCnt();

	/* check hAction */
	for( i=0 ; i< nCtxCnt ; i++ )
	{
		svcPb_Context_t *pstContext = svc_pb_main_GetContext(i);

		if( pstContext->hAction == hAction )
		{
			return i;
		}
	}

	return -1;
}

#define _____MSG_ARGUMENT_FUNCTIONS_____
STATIC HERROR svc_pb_main_GetSetupMsgArgPosAndSize (SvcPb_Msg_t *pstMsg, svcPb_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case ePBMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSetup.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case ePBMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSetup.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	case ePBMSG_ARGTYPE_Setup:
	case ePBMSG_ARGTYPE_SetDecryption:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stSetup.stSetup));
		*pulArgSize = sizeof(SvcPb_Setup_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_pb_main_GetBaseMsgArgPosAndSize (SvcPb_Msg_t *pstMsg, svcPb_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case ePBMSG_ARGTYPE_SyncMode:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stBase.bSyncMode));
		*pulArgSize = sizeof(HBOOL);
		break;

	case ePBMSG_ARGTYPE_ApiResultPtr:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stBase.phResult));
		*pulArgSize = sizeof(HERROR *);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_pb_main_GetEvtPlaybackMsgArgPosAndSize (SvcPb_Msg_t *pstMsg, svcPb_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (eArgType)
	{
	case ePBMSG_ARGTYPE_PbEvtParam:
		*ppvArgPos = (void *)(&(pstMsg->unArg.stEvtParam.stEvtParam));
		*pulArgSize = sizeof(SvcPb_EvtParam_t);
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_pb_main_GetMsgArgPosAndSize (SvcPb_Msg_t *pstMsg, svcPb_MsgArgType_e eArgType, void **ppvArgPos, HUINT32 *pulArgSize)
{
	switch (pstMsg->eMsgClass)
	{
	case ePBMSG_CMD_SETUP:
	case ePBMSG_CMD_SETDECRYPT:
		return svc_pb_main_GetSetupMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);
	case ePBMSG_CMD_START:
	case ePBMSG_CMD_STOP:
	case ePBMSG_CMD_CLOSE:
	case ePBMSG_CMD_PAUSE:
	case ePBMSG_CMD_RESUME:
		return svc_pb_main_GetBaseMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);
	case ePBMSG_CMD_EVT_PLAYBACK:
		return svc_pb_main_GetEvtPlaybackMsgArgPosAndSize (pstMsg, eArgType, ppvArgPos, pulArgSize);

	default:
		break;
	}

	// Pb Section Callback Messages:


	// Nothing:
	return ERR_FAIL;
}

#define _____MSG_RELATED_FUNCTIONS_____
STATIC HERROR svc_pb_main_MakeStopMsgByNextStartMsg (svcPb_Context_t *pstContext, SvcPb_Msg_t *pstStartMsg, SvcPb_Msg_t *pstStopMsg)
{
	HxSTD_MemSet (pstStopMsg, 0, sizeof(SvcPb_Msg_t));

	pstStopMsg->hAction = pstContext->hAction;

	switch (pstStartMsg->eMsgClass)
	{
	case ePBMSG_CMD_START:
		pstStopMsg->eMsgClass = ePBMSG_CMD_CLOSE;
//		pstStopMsg->unArg.stStopLive.hNextSvc	= pstStartMsg->unArg.stStartLive.hSuppleSvc;
//		pstStopMsg->unArg.stStopLive.bSyncMode	= FALSE;
		break;

	default:
		HxLOG_Error ("the msg (%d) has no pairing stop message:\n", pstStartMsg->eMsgClass);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_pb_main_ExecuteContextAction (svcPb_Context_t *pstContext, SvcPb_Msg_t *pstMsg)
{
	if (NULL == pstContext->pfAction)				{ return ERR_FAIL; }

	pstContext->pfAction (pstMsg, pstContext->pvData);
	return ERR_OK;
}

#define ___MSG_TASK___
STATIC void	svc_pb_main_Callback_t(HUINT32 ulEvent, HUINT32 ulDeviceId, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HERROR	hErr =	ERR_OK;
	SvcPb_Msg_t stMsg;

	stMsg.eMsgClass = ePBMSG_CMD_EVT_PLAYBACK;

	switch( ulEvent )
	{
		case ePAL_PB_EVENT_TIMECODE_CHANGED:	stMsg.unArg.stEvtParam.stEvtParam.eType = ePBEVT_TIMECODE;	break;
		case ePAL_PB_EVENT_EOF:					stMsg.unArg.stEvtParam.stEvtParam.eType = ePBEVT_EOF;	break;
		case ePAL_PB_EVENT_BOF:					stMsg.unArg.stEvtParam.stEvtParam.eType = ePBEVT_BOF;	break;
		default:
			HxLOG_Error ("[%s:%d] Unknown event(0x%X), ulInstance:0x%X\n", ulEvent, ulParam1);
			return;
	}

	/* bypass */
	{
		stMsg.hAction								= svc_pb_main_GetHandleFromResourceId(ulDeviceId, ePbResType_PlayerId); // InstanceId
		stMsg.unArg.stEvtParam.stEvtParam.ulParam1	= ulParam1;
		stMsg.unArg.stEvtParam.stEvtParam.ulParam2	= ulParam2;
		stMsg.unArg.stEvtParam.stEvtParam.ulParam3	= ulParam3;

		hErr = svc_pb_SendMsgToPbTask( &stMsg );

		if( ERR_OK  != hErr )
		{
			HxLOG_Error ("event(0x%X) deactivated. cannot send\n", ulEvent);
		}
	}

	return ;
}

#define _____ACTION_ALLOCATION_____
STATIC HERROR svc_pb_main_ResetContext (svcPb_Context_t *pstContext)
{
	pstContext->hAction			= HANDLE_NULL;
	pstContext->pfAction		= NULL;
	if (NULL != pstContext->pvData)
	{
		svc_pb_FreeActionData (pstContext->pvData);
		pstContext->pvData = NULL;
	}

	return ERR_OK;
}

STATIC HERROR svc_pb_main_InitContext (svcPb_Context_t *pstContext, Handle_t hAction, SvcPb_Msg_t *pstMsg)
{
	pstContext->hAction = hAction;

	pstContext->pfAction = svc_pb_GetActionFunc ();
	if (NULL == pstContext->pfAction)
	{
		HxLOG_Critical ("svc_pb_GetActionFunc return NULL:\n");
		goto ERROR;
	}

	pstContext->pvData = svc_pb_AllocActionData ();
	if (NULL == pstContext->pvData)
	{
		HxLOG_Critical ("svc_pb_AllocActionData return NULL:\n");
		goto ERROR;
	}

	/* register callback */
	{
		HERROR hErr;
		HUINT32			ulActionId;
		HUINT32			ulPlayerId;
		SvcPb_Setup_t	stSetup;

		svc_pb_GetMsgArg (pstMsg, ePBMSG_ARGTYPE_Setup, (void *)&stSetup);

		ulActionId = PAL_PIPE_GetActionId( pstMsg->hAction );
		hErr = PAL_PIPE_GetResourceId( ulActionId, eRxRSCTYPE_PVR_PLAYER, &ulPlayerId);
		if (ERR_OK == hErr)
			PAL_PB_RegisterNotifier (ulPlayerId, svc_pb_main_Callback_t);
	}


	return ERR_OK;

ERROR:
	svc_pb_main_ResetContext (pstContext);
	return ERR_FAIL;
}

#define _____CMD_FUNCTIONS_____
STATIC HERROR svc_pb_main_CmdCloseSession (svcPb_Context_t *pstContext, SvcPb_Msg_t *pstMsg)
{
	Handle_t			 hAction = pstMsg->hAction;
	HERROR				 hResult = ERR_FAIL;

	HxLOG_Info ("MsgClass(%d), Status(%d)\n", pstMsg->eMsgClass, pstContext->eState);

	switch (pstContext->eState)
	{
	case eSvcPb_CtxtState_None:
		pstContext->hAction = hAction;
		hResult = ERR_OK;
		break;

	case eSvcPb_CtxtState_Waiting:
		pstContext->hAction = hAction;
		if (NULL != pstContext->pfAction)
		{
			svc_pb_main_ExecuteContextAction (pstContext, pstMsg);
		}

		hResult = ERR_OK;
		break;

	case eSvcPb_CtxtState_Running:
		pstContext->hAction	= hAction;
		pstContext->eState	= eSvcPb_CtxtState_Waiting;

		if (NULL != pstContext->pfAction)
		{
			svc_pb_main_ExecuteContextAction (pstContext, pstMsg);
		}
		else
		{
			HxLOG_Critical ("!!! State Running but No Action !!!\n");
		}

		hResult = ERR_OK;
		break;

	default:
		hResult = ERR_FAIL;
		break;
	}

	/* unreg */
	{
		HERROR hErr;
		HUINT32			ulActionId;
		HUINT32			ulPlayerId;

		ulActionId = PAL_PIPE_GetActionId( pstMsg->hAction );
		hErr = PAL_PIPE_GetResourceId( ulActionId, eRxRSCTYPE_PVR_PLAYER, &ulPlayerId);
		if (ERR_OK == hErr)
			PAL_PB_UnregisterNotifier (ulPlayerId, svc_pb_main_Callback_t);
	}

	/* free resources */
	svc_pb_main_ResetContext (pstContext);

//	MWC_UTIL_PostMsgToAp (EVT_PB_STOPPED, hAction, 0, 0, 0);
	return hResult;
}

STATIC HERROR svc_pb_main_CmdOpenSession (svcPb_Context_t *pstContext, SvcPb_Msg_t *pstMsg)
{
	Handle_t				 hAction = pstMsg->hAction;
	SvcPb_Msg_t 			*pstStopMsg = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	HxLOG_Info ("MsgClass(%d), Status(%d)\n", pstMsg->eMsgClass, pstContext->eState);

	switch (pstContext->eState)
	{
	case eSvcPb_CtxtState_None:
		hErr = svc_pb_main_InitContext (pstContext, hAction, pstMsg);
		if (ERR_OK == hErr)
		{
			pstContext->hAction = hAction;
			pstContext->eState = eSvcPb_CtxtState_Waiting;

			hResult = svc_pb_main_CmdOpenSession (pstContext, pstMsg);
		}

		break;

	case eSvcPb_CtxtState_Waiting:
		if (NULL == pstContext->pfAction)
		{
			HxLOG_Critical ("Action Function shall be set in the waiting state!!!\n");
			return ERR_FAIL;
		}

		{
			pstContext->hAction	= hAction;
			pstContext->eState	= eSvcPb_CtxtState_Running;
			svc_pb_main_ExecuteContextAction (pstContext, pstMsg);
			hResult = ERR_OK;
		}
		break;

	case eSvcPb_CtxtState_Running:
		pstStopMsg = (SvcPb_Msg_t *)OxMW_Calloc (sizeof(SvcPb_Msg_t));
		if (NULL != pstStopMsg)
		{
			hErr = svc_pb_main_MakeStopMsgByNextStartMsg (pstContext, pstMsg, pstStopMsg);
			if (ERR_OK == hErr)
			{
				svc_pb_main_CmdCloseSession (pstContext, pstStopMsg);
			}

			OxMW_Free (pstStopMsg);
		}

		if (eSvcPb_CtxtState_Running != pstContext->eState)
		{
			hResult = svc_pb_main_CmdOpenSession (pstContext, pstMsg);
		}

		break;

	default:
		hResult = ERR_FAIL;
		break;
	}

	return hResult;
}

STATIC void svc_pb_main_Task(void *pvArg)
{
	HBOOL					 bSync;
	HINT32					 nContextId;
	HERROR					*phResult;
	SvcPb_Msg_t			 stMsg;
	svcPb_Context_t		*pstContext;
	HERROR					 hErr, hResult;

	while (1)
	{
		hErr = VK_MSG_Receive (svc_pb_main_GetPbMsgQueueId(), &stMsg, sizeof (SvcPb_Msg_t));
		if (VK_OK != hErr)					{ continue; }

		nContextId = svc_pb_GetContextIndex (stMsg.hAction);
		if( -1 == nContextId )
			continue;

		pstContext = svc_pb_main_GetContext (nContextId);
		if (NULL == pstContext)
		{
			bSync = FALSE;
			phResult = NULL;

			hErr = svc_pb_GetMsgArg (&stMsg, ePBMSG_ARGTYPE_ApiResultPtr, (void *)&phResult);
			if ((ERR_OK == hErr) && (NULL != phResult))
			{
				*phResult = ERR_FAIL;
			}

			hErr = svc_pb_GetMsgArg (&stMsg, ePBMSG_ARGTYPE_SyncMode, (void *)&bSync);
			if ((ERR_OK == hErr) && (TRUE == bSync))
			{
				svc_pb_ReleaseSyncSem (0);
			}

			continue;
		}

		hResult = ERR_FAIL;

		switch (stMsg.eMsgClass)
		{
		case ePBMSG_CMD_SETUP:
			if (PAL_PIPE_IsActionHandleLatest (stMsg.hAction) == ERR_OK)
			{
				pstContext->eState = eSvcPb_CtxtState_None;
				hResult = svc_pb_main_CmdOpenSession (pstContext, &stMsg);
			}

			break;

		case ePBMSG_CMD_CLOSE:
			// Stop은 절대로 메시지를 막거나 하지 않는다.
			hResult = svc_pb_main_CmdCloseSession (pstContext, &stMsg);
			break;

		default:
			if ((eSvcPb_CtxtState_Running == pstContext->eState) && (stMsg.hAction == pstContext->hAction))
			{
				hResult = svc_pb_main_ExecuteContextAction (pstContext, &stMsg);
			}
			else
			{
				hResult = ERR_FAIL;
			}

			break;
		}

		phResult = NULL;
		hErr = svc_pb_GetMsgArg (&stMsg, ePBMSG_ARGTYPE_ApiResultPtr, (void *)&phResult);
		if ((ERR_OK == hErr) && (NULL != phResult))
		{
			*phResult = hResult;
		}

		bSync = FALSE;
		hErr = svc_pb_GetMsgArg (&stMsg, ePBMSG_ARGTYPE_SyncMode, (void *)&bSync);
		if ((ERR_OK == hErr) && (TRUE == bSync))
		{
			svc_pb_ReleaseSyncSem (nContextId);
		}
	}
}


#define ___MEMBER_FUNCTIONS___
HERROR svc_pb_InitTask(void)
{
	HUINT32					 ulCnt;
	HCHAR					 szSemName[16];
	HERROR					 hErr;

	hErr = VK_MSG_Create (MW_PVR_MSGQ_SIZE, sizeof(SvcPb_Msg_t), "SvcPbQ", &ulSvcPb_MsgQId, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_MSG_Create error:\n");
		return ERR_FAIL;
	}

	hErr = VK_TASK_Create (svc_pb_main_Task, MW_PVR_PLAYER_TASK_PRIORITY, MW_PVR_STACK_SIZE, "SvcPbTask", NULL, &s_ulSvcPb_TaskId, 0);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_TASK_Create error:\n");
		return ERR_FAIL;
	}

	/*init contexts*/
	{
		/*get number of pb*/
		if( (NULL == s_pSvcPb_Contexts) && (0 == s_nPb_ContextCnt) )
		{
			HUINT32 ulNumDevice = 0;
			hErr = PAL_PB_GetCapability (&ulNumDevice);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("PAL_PVR_PB_GetCapability error:\n");
				return ERR_FAIL;
			}

			s_nPb_ContextCnt	= ulNumDevice;
			s_pSvcPb_Contexts	= (svcPb_Context_t*)OxMW_Malloc( sizeof(svcPb_Context_t) * ulNumDevice );
			if( NULL == s_pSvcPb_Contexts )
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

		for (ulCnt = 0; ulCnt < s_nPb_ContextCnt; ulCnt++)
		{
			svcPb_Context_t *pstContext = svc_pb_main_GetContext (ulCnt);

			HxSTD_MemSet (pstContext, 0, sizeof(svcPb_Context_t));

			pstContext->hAction			= HANDLE_NULL;

			HxSTD_PrintToStrN (szSemName, 16, "SvcPbSync%02d", ulCnt);

			hErr = VK_SEM_Create (&(pstContext->ulSyncSem), (const char *)szSemName, VK_SUSPENDTYPE_PRIORITY);
			if (ERR_OK == hErr)
			{
				svc_pb_GetSyncSem (ulCnt);
			}
			else
			{
				HxLOG_Error ("VK_SEM_Create err: Sem(%d) failed\n", ulCnt);
			}
		}
	}

	// Init the other PB sub modules:
//	svc_pb_cb_Init();

	/* Start PB Task */
	VK_TASK_Start (s_ulSvcPb_TaskId);
	return ERR_OK;
}

void svc_pb_GetSyncSem (HUINT32 ulIndex)
{
	svcPb_Context_t 		*pstContext = svc_pb_main_GetContext (ulIndex);

	if ((NULL != pstContext) && (0 != pstContext->ulSyncSem))
	{
		VK_SEM_Get (pstContext->ulSyncSem);
	}
}

void svc_pb_ReleaseSyncSem (HUINT32 ulIndex)
{
	svcPb_Context_t			*pstContext = svc_pb_main_GetContext (ulIndex);

	if ((NULL != pstContext) && (0 != pstContext->ulSyncSem))
	{
		VK_SEM_Release (pstContext->ulSyncSem);
	}
}

HINT32 svc_pb_AllocContext (Handle_t hAction)
{
	HUINT32 i=0;
	HUINT32 nCtxCnt = 0;

	nCtxCnt = svc_pb_main_GetContextCnt();

	/* check hAction */
	for( i=0 ; i< nCtxCnt ; i++ )
	{
		svcPb_Context_t *pstContext = svc_pb_main_GetContext(i);

		if( pstContext->hAction == hAction )
		{
			HxLOG_Error ("Already alloced hAction : 0x%x error:\n", hAction);
			return -1;
		}
	}

	/* alloc */
	for( i=0 ; i< nCtxCnt ; i++ )
	{
		svcPb_Context_t *pstContext = svc_pb_main_GetContext(i);

		if( HANDLE_NULL == pstContext->hAction )
		{
			pstContext->hAction = hAction;
			return i;
		}
	}

	HxLOG_Error ("empty context not found  error:\n");
	return -1;
}

HINT32 svc_pb_GetContextIndex (Handle_t hAction)
{
	return svc_pb_main_GetContextIndex (hAction);
}

HERROR svc_pb_SendMsgToPbTask (SvcPb_Msg_t *pstMsg)
{
	int				 nRet;

	if (NULL == pstMsg)						{ return ERR_FAIL; }

	nRet = VK_MSG_Send (svc_pb_main_GetPbMsgQueueId(), pstMsg, sizeof(SvcPb_Msg_t));
	if (VK_OK != nRet)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_pb_GetMsgArg (SvcPb_Msg_t *pstMsg, svcPb_MsgArgType_e eArgType, void *pvData)
{
	HUINT32				 ulArgSize = 0;
	void				*pvSrcPos = NULL;
	HERROR				 hErr;

	hErr = svc_pb_main_GetMsgArgPosAndSize (pstMsg, eArgType, &pvSrcPos, &ulArgSize);
	if ((ERR_OK != hErr) || (NULL == pvSrcPos) || (0 == ulArgSize))
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy (pvData, pvSrcPos, ulArgSize);
	return ERR_OK;
}

HERROR svc_pb_FrameByFrame (Handle_t hAction, HBOOL bFoward)
{
	svcPb_Context_t * pstContext = svc_pb_main_GetContextFromActionHandle(hAction);

	if( NULL == pstContext )
		return ERR_FAIL;

	return svc_pb_Func_FrameByFrame (pstContext->pvData, bFoward);
}

HERROR svc_pb_SetSpeed (Handle_t hAction, HINT32 nSpeed)
{
	svcPb_Context_t * pstContext = svc_pb_main_GetContextFromActionHandle(hAction);

	if( NULL == pstContext )
		return ERR_FAIL;

	return svc_pb_Func_SetSpeed (pstContext->pvData, nSpeed);
}

HERROR svc_pb_GetSpeed (Handle_t hAction, HINT32 *pnSpeed)
{
	svcPb_Context_t * pstContext = svc_pb_main_GetContextFromActionHandle(hAction);

	if( NULL == pstContext )
		return ERR_FAIL;

	return svc_pb_Func_GetSpeed (pstContext->pvData, pnSpeed);
}

HERROR svc_pb_GetSupportSpeeds (Handle_t hAction, HBOOL bScrambled, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray)
{
	return svc_pb_Func_GetSupportSpeeds (NULL, bScrambled, pulSpeedNum, ppnSpeedArray);
}

HERROR svc_pb_SetPlayTime(Handle_t hAction, HUINT32 ulTime )
{
	svcPb_Context_t * pstContext = svc_pb_main_GetContextFromActionHandle(hAction);

	if( NULL == pstContext )
		return ERR_FAIL;

	return svc_pb_Func_SetPlayTime (pstContext->pvData, ulTime);
}

HERROR svc_pb_GetPlayTime(Handle_t hAction, HUINT32 *pulTime )
{
	svcPb_Context_t * pstContext = svc_pb_main_GetContextFromActionHandle(hAction);

	if( NULL == pstContext )
		return ERR_FAIL;

	return svc_pb_Func_GetPlayTime (pstContext->pvData, pulTime);
}

HERROR svc_pb_GetTimeInfo(Handle_t hAction, HUINT32 *pulCurrentTime, HUINT32 *pulEndTime )
{
	svcPb_Context_t * pstContext = svc_pb_main_GetContextFromActionHandle(hAction);

	if( NULL == pstContext )
		return ERR_FAIL;

	return svc_pb_Func_GetTimeInfo (pstContext->pvData, pulCurrentTime, pulEndTime);
}

HERROR svc_pb_GetState(Handle_t hAction, svcPb_State_e *pePbState)
{
	svcPb_Context_t * pstContext = svc_pb_main_GetContextFromActionHandle(hAction);

	if( NULL == pstContext )
		return ERR_FAIL;

	return svc_pb_Func_GetState (pstContext->pvData, pePbState);
}

/* end of file */
