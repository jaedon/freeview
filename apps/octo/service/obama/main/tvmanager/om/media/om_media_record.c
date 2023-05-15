/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_media_record.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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
#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <sapi.h>
#include <octo_common.h>
#include <svc_pipe.h>
#include <bus.h>
#include <dlib.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_pvr_action.h>
#include <mgr_schedule.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32 					 ulRsvSlotId;
	OxMediaRec_RecState_e		 eRecState;

	HUINT32 					 ulLastRecTimeTick;
} omMediaRecordContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC omMediaRecordContext_t			s_astOmMediaRecordContext[NUM_MAX_REC_ACTION];
STATIC HINT32				 			s_nRpcHandle = 0;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#define _____CONTEXT_FUNCTION_____
omMediaRecordContext_t *om_media_record_GetContext (HUINT32 ulRecId)
{
	if (ulRecId < NUM_MAX_REC_ACTION)
	{
		return &(s_astOmMediaRecordContext[ulRecId]);
	}

	return NULL;
}

omMediaRecordContext_t *om_media_record_GetContextByActionId (HUINT32 ulActionId)
{
	if ((ulActionId >= eActionId_REC_FIRST) && (ulActionId <= eActionId_REC_LAST))
	{
		return &(s_astOmMediaRecordContext[ulActionId - eActionId_REC_FIRST]);
	}

	return NULL;
}

omMediaRecordContext_t *om_media_record_FindContextByRsvSlot (HUINT32 ulSlot)
{
	HUINT32			 ulCount;

	for (ulCount = 0; ulCount < NUM_MAX_REC_ACTION; ulCount++)
	{
		omMediaRecordContext_t		*pstContext = &(s_astOmMediaRecordContext[ulCount]);

		if ((eOxMR_RECSTATE_STOPPED != pstContext->eRecState) &&
			(ulSlot == pstContext->ulRsvSlotId))
		{
			return pstContext;
		}
	}

	return NULL;
}

#define _____RECORD_FUNCTION_____
STATIC HBOOL om_media_record_IsRecordFree (HUINT32 ulRecId)
{
	omMediaRecordContext_t	*pstContext = om_media_record_GetContext (ulRecId);

	if (NULL != pstContext)
	{
		switch (pstContext->eRecState)
		{
		case eOxMR_RECSTATE_NONE:
		case eOxMR_RECSTATE_STOPPED:
			return TRUE;

		default:
			break;
		}
	}

	return FALSE;
}

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void om_media_record_CbOnRecStarted (HUINT32 ulSchSlot, HUINT32 ulContentsId, OxMediaRec_StartInfo_t *pstStart)
{
	HLIB_RPC_Notify (s_nRpcHandle,RPC_OAPI_MEDIA_REC_OnStarted, "iib", ulSchSlot, ulContentsId, pstStart, sizeof(OxMediaRec_StartInfo_t));
}

STATIC void om_media_record_CbOnRecStopped (HUINT32 ulSchSlot)
{
	HLIB_RPC_Notify (s_nRpcHandle,RPC_OAPI_MEDIA_REC_OnStopped, "i", ulSchSlot);
}

STATIC void om_media_record_CbOnRecTimecodeChanged (HUINT32 ulSchSlot, HUINT32 ulRecTime)
{
	HLIB_RPC_Notify (s_nRpcHandle,RPC_OAPI_MEDIA_REC_OnTimecodeChanged, "ii", ulSchSlot, ulRecTime);
}

STATIC void om_media_record_CbOnRecMaxInstanceChagned (HUINT32 ulMaxRecInst, HUINT32 ulMaxRecAll)
{
	HLIB_RPC_Notify (s_nRpcHandle,RPC_OAPI_MEDIA_REC_OnMaxInstanceChanged, "ii", ulMaxRecInst, ulMaxRecAll);
}

#define _____OAPI_COMMAND_FUNCTIONS_____
STATIC HERROR om_media_record_CbActivate (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HLIB_RPC_Response(s_nRpcHandle, nClientHandle, nRequestId, ERR_OK, "iii", NUM_REC_ACTION, NUM_MAX_REC_ACTION, NUM_TSRREC_ACTION);

	return ERR_OK;
}

STATIC HERROR om_media_record_CbTsrStart (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	Handle_t				 hMasterSvc = HANDLE_NULL, hSuppleSvc = HANDLE_NULL;
	OxMediaRec_StartInfo_t	*pstStart = (OxMediaRec_StartInfo_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	HERROR					 hErr = ERR_FAIL;

	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstStart->ulMasterSvcUid & 0xFFFF), &hMasterSvc);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hMasterSvc))
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx err: ulMasterSvcUid(0x%08x)\n", pstStart->ulMasterSvcUid);
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstStart->ulSuppleSvcUid & 0xFFFF), &hSuppleSvc);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hSuppleSvc))
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx err: ulSuppleSvcUid(0x%08x)\n", pstStart->ulSuppleSvcUid);
		return ERR_FAIL;
	}

	hErr = MGR_PVR_StartTsrRecording(HANDLE_NULL, hMasterSvc, hSuppleSvc, pstStart->eSuppSvcType, eTsrType_ExternalRequest, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_PVR_StartTsrRecording Error. - svcUid(0x%08x), hSvc(0x%08x)\n", pstStart->ulMasterSvcUid, hMasterSvc);
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
	}

	return ERR_OK;
}

STATIC HERROR om_media_record_CbTsrStop (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	Handle_t				 hMasterSvc = HANDLE_NULL, hSuppleSvc = HANDLE_NULL;
	OxMediaRec_StartInfo_t	*pstStart = (OxMediaRec_StartInfo_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	HERROR					 hErr = ERR_FAIL;

	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstStart->ulMasterSvcUid & 0xFFFF), &hMasterSvc);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hMasterSvc))
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx err: ulMasterSvcUid(0x%08x)\n", pstStart->ulMasterSvcUid);
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstStart->ulSuppleSvcUid & 0xFFFF), &hSuppleSvc);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hSuppleSvc))
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx err: ulSuppleSvcUid(0x%08x)\n", pstStart->ulSuppleSvcUid);
		return ERR_FAIL;
	}

	hErr = MGR_PVR_StopTsrRecording(hMasterSvc, hSuppleSvc, eTsrType_ExternalRequest);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_PVR_StopTsrRecording Error. - svcUid(0x%08x), hSvc(0x%08x)\n", pstStart->ulMasterSvcUid, hMasterSvc);
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
	}

	return ERR_OK;
}

#define _____MSG_PROCESS_FUNCTIONS_____
// Create와 Destroy 만은 Context가 NULL일 수 있다.
STATIC BUS_Result_t om_media_record_MsgBusCreate (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulCount;

	HxSTD_MemSet (s_astOmMediaRecordContext, 0, sizeof(omMediaRecordContext_t) * NUM_MAX_REC_ACTION);

	for (ulCount = 0; ulCount < NUM_MAX_REC_ACTION; ulCount++)
	{
		omMediaRecordContext_t			*pstContext = &(s_astOmMediaRecordContext[ulCount]);

//		pstContext->ulRecId		= ulCount;
		pstContext->eRecState	= eOxMR_RECSTATE_STOPPED;
		pstContext->ulRsvSlotId	= DxRSV_SLOT_NULL;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_media_record_MsgBusDestroy (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_media_record_MsgAppRecordNotifyStarted (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	MgrRec_RecordType_e 	 eRecType = (MgrRec_RecordType_e)p1;
	omMediaRecordContext_t	*pstContext;
	MgrRec_RecInfo_t		 stRecInfo;
	OxMediaRec_StartInfo_t	 stStartInfo;
	HERROR					 hErr;

	if (eMgrRecType_TSR == eRecType)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_RECORDER_GetRecordingSvcInfo(ulActionId, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("MGR_RECORDER_GetRecordingSvcInfo err: ActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	pstContext = om_media_record_GetContextByActionId(ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Critical("No Context: ActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	if (eOxMR_RECSTATE_RECORDING == pstContext->eRecState)
	{
		HxLOG_Critical("Already Record Context Running: ActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	pstContext->eRecState			= eOxMR_RECSTATE_RECORDING;
	pstContext->ulRsvSlotId			= stRecInfo.ulSlot;
	pstContext->ulLastRecTimeTick	= 0;

	HxSTD_MemSet(&stStartInfo, 0, sizeof(OxMediaRec_StartInfo_t));
	stStartInfo.ulMasterSvcUid	= DB_SVC_GetServiceUID(stRecInfo.hMasterSvc);
	stStartInfo.ulSuppleSvcUid	= DB_SVC_GetServiceUID(stRecInfo.hSvc);
	stStartInfo.eSuppSvcType	= stRecInfo.eSessionType;
	stStartInfo.nStartOffset	= -1;
	stStartInfo.ulDuration		= stRecInfo.ulEndTime - stRecInfo.ulStartTime;
	stStartInfo.nEvtId			= stRecInfo.nEventId;

	om_media_record_CbOnRecStarted(stRecInfo.ulSlot, stRecInfo.ulContentId, &stStartInfo);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_record_MsgAppRecordTimecodeChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32					 ulRecTime = (HUINT32)p1;
	HUINT32 				 ulTick = (HUINT32)VK_TIMER_GetSystemTick();
	HUINT32 				 ulDuration = MGR_RECORDER_GetRecorderParam(eMgrRec_DEFAULT_DURATION_TSR_RECORD);
	omMediaRecordContext_t	*pstContext;
	MgrRec_RecInfo_t		 stRecInfo;
	HERROR					 hErr;

	hErr = MGR_RECORDER_GetRecordingSvcInfo(ulActionId, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("MGR_RECORDER_GetRecordingSvcInfo err: ActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	pstContext = om_media_record_GetContextByActionId(ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Critical("No Context: ActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	if (eOxMR_RECSTATE_RECORDING != pstContext->eRecState)
	{
		// HxLOG_Critical("Rec Context not Recording: ActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	// Recording Time 을 너무 자주 올려주지 않는다. 그리고 그 중간 보정은 APPKIT에서 Tick을 이용해서 하도록 한다.
	// tsr full buffer 되는 순간엔 좀 자주 올려준다.
	if ((0xFFFFFFFF == pstContext->ulLastRecTimeTick) ||
		((ulTick - pstContext->ulLastRecTimeTick) >= 10000) ||
		(ulRecTime > ulDuration - 5 && ulRecTime < ulDuration + 5))
	{
		pstContext->ulLastRecTimeTick = ulTick;
		om_media_record_CbOnRecTimecodeChanged (pstContext->ulRsvSlotId, ulRecTime);
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_record_MsgAppRecordTsrCopyEnd(Handle_t hAction,HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	MgrRec_RecInfo_t		 stRecInfo;
	HERROR					 hErr;

	hErr = MGR_RECORDER_GetRecordingSvcInfo(ulActionId, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("MGR_RECORDER_GetRecordingSvcInfo err: ActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}
	HLIB_RPC_Notify (s_nRpcHandle,RPC_OAPI_MEDIA_REC_OnTsrBufferCopyEnd, "i", stRecInfo.ulSlot);
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_media_record_MsgAppRecordNotifyStopped (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	MgrRec_RecordType_e 	 eRecType = (MgrRec_RecordType_e)p1;
	omMediaRecordContext_t	*pstContext;
	MgrRec_RecInfo_t		 stRecInfo;
	HERROR					 hErr;

	if (eMgrRecType_TSR == eRecType)
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_RECORDER_GetRecordingSvcInfo(ulActionId, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("MGR_RECORDER_GetRecordingSvcInfo err: ActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	pstContext = om_media_record_GetContextByActionId(ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Critical("No Context: ActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	if (eOxMR_RECSTATE_RECORDING != pstContext->eRecState)
	{
		HxLOG_Critical("Rec Context not Recording: ActionId(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	pstContext->eRecState = eOxMR_RECSTATE_STOPPED;
	pstContext->ulRsvSlotId = 0xFFFF; //stRecInfo.ulSlot;

	om_media_record_CbOnRecStopped(stRecInfo.ulSlot);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_record_MsgAppRecordActionTypeChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_record_MsgAppRecordMaxInstanceChanged(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	om_media_record_CbOnRecMaxInstanceChagned ((HUINT32)p1, (HUINT32)p2);
	return MESSAGE_PASS;
}

#define _____RPC_INITIALIZATION_____
STATIC HERROR om_media_record_InitRpcFunctions (void)
{
	HERROR hErr;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
	if (ERR_OK != hErr )
	{
		HxLOG_Error("Error. \n");
		return ERR_FAIL;
	}


	// APPKIT -> OBAMA
	HLIB_RPC_RegisterRequest(s_nRpcHandle, RPC_OAPI_MEDIA_REC_Init,		NULL, om_media_record_CbActivate,	NULL, "Activate the mediarec module: (void)");
	HLIB_RPC_RegisterRequest(s_nRpcHandle, RPC_OAPI_MEDIA_REC_TsrStart,	"b", om_media_record_CbTsrStart,	NULL, "Start the TSR record: (int svcuid)");
	HLIB_RPC_RegisterRequest(s_nRpcHandle, RPC_OAPI_MEDIA_REC_TsrStop,	"b", om_media_record_CbTsrStop,		NULL, "Stop the TSR record: (int svcuid)");

	// OBAMA -> APPKIT
	HLIB_RPC_CreateNotification(s_nRpcHandle, RPC_OAPI_MEDIA_REC_OnStarted, "iib",
		"Record Started.\n"
		"\t   - (int nSlot, int nContentsId, OxMediaRec_StartInfo_t *pstStart)\n");

	HLIB_RPC_CreateNotification(s_nRpcHandle, RPC_OAPI_MEDIA_REC_OnStopped, "i",
		"Record Stopped.\n"
		"\t   - (int nSlot)\n");

	HLIB_RPC_CreateNotification(s_nRpcHandle, RPC_OAPI_MEDIA_REC_OnTimecodeChanged, "ii",
		"Record Time changed.\n"
		"\t   - (int nSlot, int nRecordTime)\n");

	HLIB_RPC_CreateNotification(s_nRpcHandle, RPC_OAPI_MEDIA_REC_OnTsrBufferCopyEnd, "i",
		"Record Tsr buffer Copy End.\n"
		"\t   - (int nContentsId)\n");

	HLIB_RPC_CreateNotification(s_nRpcHandle, RPC_OAPI_MEDIA_REC_OnMaxInstanceChanged, "ii",
		"Record Instacne Changed.\n"
		"\t   - (int Max Rec Instance)\n");

	return ERR_OK;
}

#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_media_record_Proto (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult;
	switch (message)
	{
	case eMEVT_BUS_CREATE:
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
		eResult = om_media_record_MsgBusCreate (hHandle, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		eResult = om_media_record_MsgBusDestroy (hHandle, p1, p2, p3);
		break;

	case eMEVT_PVR_NOTIFY_RECORD_STARTED:
		eResult = om_media_record_MsgAppRecordNotifyStarted (hHandle, p1, p2, p3);
		break;

	case eMEVT_PVR_NOTIFY_RECORD_STOPPED:
		eResult = om_media_record_MsgAppRecordNotifyStopped (hHandle, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_TYPE_CHANGED:
		eResult = om_media_record_MsgAppRecordActionTypeChanged (hHandle, p1, p2, p3);
		break;

	case eSEVT_REC_TIMECODE_CHANGE:
		eResult = om_media_record_MsgAppRecordTimecodeChanged (hHandle, p1, p2, p3);
		break;

	case eMEVT_PVR_RECORD_TSR_COPYEND:
		eResult = om_media_record_MsgAppRecordTsrCopyEnd (hHandle, p1, p2, p3);
		break;

	case eMEVT_SCHEDULE_REC_CHANGE_INSTANCE:
		eResult = om_media_record_MsgAppRecordMaxInstanceChanged(hHandle, p1, p2, p3);
		break;

	default:
		eResult = MESSAGE_PASS;
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define ___MEMBER_FUNCTIONS___

#define ___GLOBAL_FUNCTIONS___
HERROR OM_MEDIA_REC_Init (void)
{
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_media_record_Proto, HANDLE_NULL, 0, 0, 0);

	om_media_record_InitRpcFunctions();

	return ERR_OK;
}



