/**
	@file     mgr_dynamicupdate.c
	@brief    file_name & simple comment.

	Description:  \n
	Module:  \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n


	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_si.h>
#include <svc_download.h>
#include <mgr_action.h>
#include <mgr_rscmap.h>
#include <mgr_live.h>
#include <mgr_hbbtv.h>
#include <otl.h>

#include "local_include/_mgr_hbbtv.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define HBBTV_START_TIMER_ID				0x1000
#define HBBTV_START_TIMER_TIMEOUT			5000				// 2.5 sec

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HBOOL			 bTimerSet;
	HBOOL			 bActionTaken;
	HBOOL			 bChTuned;
	HBOOL			 bSiStarted;
	HBOOL			 bDownloadStarted;

	HUINT32			 ulActionId;
	Handle_t		 hAction;
	Handle_t		 hSvcHandle;

} MgrHbbtv_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC MgrHbbtv_Context_t		 s_stMgrHbbtv_Context;

#define	___INTERNAL_FUNCTIONS___
/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
STATIC MgrHbbtv_Context_t * mgr_hbbtvact_GetContext (void)
{
	return &s_stMgrHbbtv_Context;
}

STATIC HERROR mgr_hbbtvact_InitContext (MgrHbbtv_Context_t *pstContext)
{
	HxSTD_MemSet(pstContext, 0, sizeof(MgrHbbtv_Context_t));

	pstContext->ulActionId	= HANDLE_NULL;
	pstContext->hAction		= HANDLE_NULL;
	pstContext->hSvcHandle	= HANDLE_NULL;

	return ERR_OK;
}

STATIC HERROR mgr_hbbtvact_ReleaseResources (MgrHbbtv_Context_t *pstContext)
{
	HERROR				 hErr;

	if (TRUE == pstContext->bDownloadStarted)
	{
		hErr = SVC_DOWNLOAD_Stop(pstContext->hAction, eAsyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_DOWNLOAD_Stop err: hAction(0x%08x)\n", pstContext->hAction);
		}

		pstContext->bDownloadStarted = FALSE;
	}

	if (TRUE == pstContext->bSiStarted)
	{
		hErr = SVC_SI_StopHbbtv(pstContext->hAction, FALSE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_SI_StopHbbtv err: hAction(0x%08x)\n", pstContext->hAction);
		}

		pstContext->bSiStarted = FALSE;
	}

	if (TRUE == pstContext->bChTuned)
	{
		hErr = SVC_CH_StopTune(pstContext->hAction, (HUINT16)-1, eAsyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_CH_StopTune err: hAction(0x%08x)\n", pstContext->hAction);
		}

		pstContext->bChTuned = FALSE;
	}


	if (TRUE == pstContext->bActionTaken)
	{
		hErr = MGR_ACTION_Release(pstContext->ulActionId, eActionType_BG_TUNE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("MGR_ACTION_Release err: ulActionId(%d)\n", pstContext->ulActionId);
		}

		pstContext->bActionTaken = FALSE;
	}

	pstContext->ulActionId	= HANDLE_NULL;
	pstContext->hAction		= HANDLE_NULL;

	return ERR_OK;
}

STATIC HERROR mgr_hbbtvact_StartAction (MgrHbbtv_Context_t *pstContext)
{
	HUINT32			 ulActionId = eActionId_MAX;
	DbSvc_TsInfo_t	 stTsInfo;
	DxTuneParam_t	 stTuneParam;
	HERROR			 hErr, hResult = ERR_FAIL;

	(void)mgr_hbbtvact_ReleaseResources(pstContext);

	// Now Set Action and try to tune:
	hErr = MGR_ACTION_GetEmptyBgActionId(&ulActionId);
	if ((ERR_OK != hErr) || (ulActionId >= eActionId_MAX))
	{
		HxLOG_Error("MGR_ACTION_GetEmptyBgActionId err:\n");
		goto END_FUNC;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle(pstContext->hSvcHandle, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetEmptyBgActionId err:\n");
		goto END_FUNC;
	}

	hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stTuneParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OTL_CONV_DbTuningInfo2DxTuneParam err:\n");
		goto END_FUNC;
	}

	hErr = MGR_ACTION_SetupBgTune(ulActionId, &stTuneParam, eAmPriority_BGJOB);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_SetupBgTune err: ulActionId(%d), hSvcHandle(0x%08x)\n", ulActionId, pstContext->hSvcHandle);
		goto END_FUNC;
	}

	hErr = MGR_ACTION_Take(ulActionId, eActionType_BG_TUNE, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_Take err: ulActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	pstContext->bActionTaken = TRUE;
	pstContext->ulActionId = ulActionId;
	pstContext->hAction = SVC_PIPE_IncreaseActionVersion(ulActionId);

	BUS_SendMessage(NULL, eMEVT_HBBTV_NOTIFY_SESSION_STARTED, pstContext->hAction, 0, 0, 0);

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

STATIC HERROR mgr_hbbtvact_StartTune (MgrHbbtv_Context_t *pstContext)
{
	DbSvc_TsInfo_t	 stTsInfo;
	DxTuneParam_t	 stTuneParam;
	SvcRsc_Info_t	 stRscInfo;
	HERROR			 hErr, hResult = ERR_FAIL;

	if ((TRUE != pstContext->bActionTaken) || (TRUE == pstContext->bChTuned))
	{
		HxLOG_Error("State wrong: ActionTaken(%d), ChTuned(%d)\n", pstContext->bActionTaken, pstContext->bChTuned);
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle(pstContext->hSvcHandle, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetEmptyBgActionId err:\n");
		goto END_FUNC;
	}

	hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stTuneParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OTL_CONV_DbTuningInfo2DxTuneParam err:\n");
		goto END_FUNC;
	}

	hErr = MGR_RSC_GetResourceInfo(pstContext->ulActionId, &stRscInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_RSC_GetResourceInfo err: ulActionId(%d)\n", pstContext->ulActionId);
		goto END_FUNC;
	}

	hErr = SVC_CH_StartTune(pstContext->hAction, &stRscInfo, (HUINT16)-1, &stTuneParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_CH_StartTune err: hAction(0x%08x)\n", pstContext->hAction);
		goto END_FUNC;
	}

	pstContext->bChTuned = TRUE;
	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

STATIC HERROR mgr_hbbtvact_StartSi (MgrHbbtv_Context_t *pstContext)
{
	DbSvc_Info_t		*pstSvcInfo = NULL;
	HERROR				 hErr, hResult = ERR_FAIL;

	pstSvcInfo = (DbSvc_Info_t *)OxAP_Calloc(sizeof(DbSvc_Info_t));
	if (NULL == pstSvcInfo)
	{
		HxLOG_Error("Memory allocation failed\n");
		goto END_FUNC;
	}

	hErr = DB_SVC_GetServiceInfo(pstContext->hSvcHandle, pstSvcInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo err: hSvcHandle(0x%08x)\n", pstContext->hSvcHandle);
		goto END_FUNC;
	}

	hErr = SVC_SI_StartHbbtv(pstContext->hAction, DbSvc_GetOnId(pstSvcInfo), DbSvc_GetTsId(pstSvcInfo), DbSvc_GetSvcId(pstSvcInfo), FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_SI_StartLive err: ulActionId(%d), hSvc(0x%08x)\n", pstContext->ulActionId, pstContext->hSvcHandle);
		goto END_FUNC;
	}

	pstContext->bSiStarted = TRUE;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSvcInfo)					{ OxAP_Free(pstSvcInfo); }
	return hResult;
}

#define _____INTERNAL_MSG_FUNCTIONS_____
STATIC BUS_Result_t mgr_hbbtvact_MsgMevtBusCreate (MgrHbbtv_Context_t *pstContext, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	(void)mgr_hbbtvact_InitContext(pstContext);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtvact_MsgMevtBusDestroy (MgrHbbtv_Context_t *pstContext, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t		 hHbbtvAct = (TRUE == pstContext->bActionTaken) ? pstContext->hAction : HANDLE_NULL;

	(void)mgr_hbbtvact_ReleaseResources(pstContext);

	if (HANDLE_NULL != hHbbtvAct)
	{
		BUS_SendMessage(NULL, eMEVT_HBBTV_NOTIFY_SESSION_STOPPED, hHbbtvAct, 0, 0, 0);
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtvact_MsgMevtActionSetupRequested (MgrHbbtv_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t		 hHbbtvAct;

	if (SVC_PIPE_GetActionId(hAction) != MGR_ACTION_GetMainActionId())
	{
		return MESSAGE_PASS;
	}

	switch (p1)
	{
	case eViewType_MEDIA:
	case eViewType_MOVIE_PB:
	case eViewType_IP:
		// Do not release the HBB-TV action: it is required to sustain the action during the IP media play.
		break;

	case eViewType_WTV:
	default:
		hHbbtvAct = (TRUE == pstContext->bActionTaken) ? pstContext->hAction : HANDLE_NULL;

		(void)mgr_hbbtvact_ReleaseResources(pstContext);

		if (TRUE == pstContext->bTimerSet)
		{
			BUS_KillTimer(HBBTV_START_TIMER_ID);
			pstContext->bTimerSet = FALSE;
		}

		if (HANDLE_NULL != hHbbtvAct)
		{
			BUS_SendMessage(NULL, eMEVT_HBBTV_NOTIFY_SESSION_STOPPED, hHbbtvAct, 0, 0, 0);
		}

		break;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtvact_MsgMevtLiveNotifySvcChanged (MgrHbbtv_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t		 hSvcHandle = (Handle_t)p1;
	HERROR			 hErr;

	pstContext->hSvcHandle = hSvcHandle;
	hErr = mgr_hbbtvact_StartAction(pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("mgr_hbbtvact_StartAction err:\n");
		return MESSAGE_PASS;
	}

	if (TRUE != pstContext->bTimerSet)
	{
		HxLOG_Info("Set Timer for starting...\n");
		BUS_SetTimer(HBBTV_START_TIMER_ID, HBBTV_START_TIMER_TIMEOUT);
		pstContext->bTimerSet = TRUE;
	}
	else
	{
		HxLOG_Info("Reset Timer for starting...\n");
		BUS_ResetTimer(HBBTV_START_TIMER_ID);
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtvact_MsgMevtBusTimer (MgrHbbtv_Context_t *pstContext, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			 hErr;

	switch (p1)
	{
	case HBBTV_START_TIMER_ID:
		(void)BUS_KillTimer(HBBTV_START_TIMER_ID);
		pstContext->bTimerSet = FALSE;

		hErr = mgr_hbbtvact_StartTune(pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("mgr_hbbtvact_StartTune err:\n");
		}

		break;

	default:
		break;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtvact_MsgSevtChLocked (MgrHbbtv_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				 hErr;

	if (hAction != pstContext->hAction)
	{
		return MESSAGE_PASS;
	}

	if (TRUE == pstContext->bSiStarted)
	{
		HxLOG_Info("Already SI started... no need to start again.\n");
		return MESSAGE_PASS;
	}

	hErr = mgr_hbbtvact_StartSi(pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Info("mgr_hbbtvact_StartSi err: ActionId(%d)\n", pstContext->ulActionId);
		return MESSAGE_PASS;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtvact_MsgSevtSiPmtPidChanged (MgrHbbtv_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				 hErr;

	if (hAction != pstContext->hAction)
	{
		return MESSAGE_PASS;
	}

	if (TRUE == pstContext->bSiStarted)
	{
		hErr = SVC_SI_StopHbbtv(pstContext->hAction, FALSE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_SI_StopHbbtv err: hAction(0x%08x)\n", pstContext->hAction);
		}

		pstContext->bSiStarted = FALSE;
	}

	hErr = mgr_hbbtvact_StartSi(pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Info("mgr_hbbtvact_StartSi err: ActionId(%d)\n", pstContext->ulActionId);
		return MESSAGE_PASS;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtvact_MsgSevtSiPmt (MgrHbbtv_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (hAction != pstContext->hAction)
	{
		return MESSAGE_PASS;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtvact_MsgSevtSiAit (MgrHbbtv_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32				 nPid = p1;
	Handle_t			 hMainActHandle;

	if (hAction != pstContext->hAction)
	{
		return MESSAGE_PASS;
	}

	hMainActHandle = SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
	BUS_SendMessage(NULL, eMEVT_HBBTV_NOTIFY_AIT, hMainActHandle, eMgrHbbtvAitState_Exist, (HINT32)pstContext->hAction, nPid);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtvact_MsgSevtSiNoneAit (MgrHbbtv_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t			 hMainActHandle;

	if (hAction != pstContext->hAction)
	{
		return MESSAGE_PASS;
	}

	hMainActHandle = SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
	BUS_SendMessage(NULL, eMEVT_HBBTV_NOTIFY_AIT, hMainActHandle, eMgrHbbtvAitState_NoAit, (HINT32)pstContext->hAction, PID_NULL);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtvact_MsgSevtSiAitTimeout (MgrHbbtv_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t			 hMainActHandle;

	if (hAction != pstContext->hAction)
	{
		return MESSAGE_PASS;
	}

	hMainActHandle = SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
	BUS_SendMessage(NULL, eMEVT_HBBTV_NOTIFY_AIT, hMainActHandle, eMgrHbbtvAitState_Timeout, (HINT32)pstContext->hAction, PID_NULL);

	return MESSAGE_PASS;
}

#define _____PROCEDURE_FUNCTION_____
STATIC BUS_Result_t mgr_hbbtvact_proc (HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrHbbtv_Context_t	*pstContext = mgr_hbbtvact_GetContext();
	BUS_Result_t		 eRet = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Debug("MESSAGE : eMEVT_BUS_CREATE, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HUINT8*)__FUNCTION__);
		eRet = mgr_hbbtvact_MsgMevtBusCreate(pstContext, handle, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Debug("MESSAGE : eMEVT_BUS_DESTROY, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtvact_MsgMevtBusDestroy(pstContext, handle, p1, p2, p3);
		break;

	case eMEVT_ACTION_SETUP_REQUESTED:
		HxLOG_Debug("MESSAGE : eMEVT_ACTION_SETUP_REQUESTED, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtvact_MsgMevtActionSetupRequested(pstContext, handle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		HxLOG_Debug("MESSAGE : eMEVT_LIVE_NOTIFY_SVC_CHANGED, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtvact_MsgMevtLiveNotifySvcChanged(pstContext, handle, p1, p2, p3);
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Debug("MESSAGE : eMEVT_BUS_TIMER, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtvact_MsgMevtBusTimer(pstContext, handle, p1, p2, p3);
		break;

	case eMEVT_BUS_READY_SHUTDOWN:
	case eMEVT_BUS_GO_SHUTDOWN:
		(void)mgr_hbbtvact_ReleaseResources(pstContext);
		break;

	case eSEVT_CH_LOCKED:
		HxLOG_Debug("MESSAGE : eSEVT_CH_LOCKED, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtvact_MsgSevtChLocked(pstContext, handle, p1, p2, p3);
		break;

	case eSEVT_SI_PMT_PIDCHANGED:
		HxLOG_Debug("MESSAGE : eSEVT_SI_PMT_PIDCHANGED, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtvact_MsgSevtSiPmtPidChanged(pstContext, handle, p1, p2, p3);
		break;

	case eSEVT_SI_PMT:
		HxLOG_Debug("MESSAGE : eSEVT_SI_PMT, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtvact_MsgSevtSiPmt(pstContext, handle, p1, p2, p3);
		break;

	case eSEVT_SI_AIT:
		HxLOG_Debug("MESSAGE : eSEVT_SI_AIT, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtvact_MsgSevtSiAit(pstContext, handle, p1, p2, p3);
		break;

	case eSEVT_SI_NONE_AIT:
		HxLOG_Debug("MESSAGE : eSEVT_SI_NONE_AIT, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtvact_MsgSevtSiNoneAit(pstContext, handle, p1, p2, p3);
		break;

	case eSEVT_SI_AIT_TIMEOUT:
		HxLOG_Debug("MESSAGE : eSEVT_SI_AIT_TIMEOUT, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtvact_MsgSevtSiAitTimeout(pstContext, handle, p1, p2, p3);
		break;

	default:
		break;
	}

	return eRet;
}

#define	___MODULE_FUNCTIONS___
/*******************************************************************/
/********************      	Module Function		 	********************/
/*******************************************************************/
HERROR mgr_hbbtv_StartAction (void)
{
	HERROR		 hErr = ERR_OK;

	if (BUS_MGR_Get(mgr_hbbtvact_proc) == NULL)
	{
		hErr = BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, mgr_hbbtvact_proc, HANDLE_NULL, 0, 0, 0);
	}

	return hErr;
}

HERROR mgr_hbbtv_StopAction (void)
{
	return (BUS_MGR_Destroy(mgr_hbbtvact_proc) == ERR_BUS_NO_ERROR) ? ERR_OK : ERR_FAIL;
}

#define	___API_FUNCTIONS___
/*******************************************************************/
/********************      	API Function    ********************/
/*******************************************************************/
HERROR MGR_HBBTV_GetActionId (HUINT32 *pulActionId)
{
	MgrHbbtv_Context_t	*pstContext = mgr_hbbtvact_GetContext();

	if ((BUS_MGR_Get(mgr_hbbtvact_proc) == NULL) || (TRUE != pstContext->bActionTaken))
	{
		return ERR_FAIL;
	}

	if (NULL != pulActionId)
	{
		*pulActionId = pstContext->ulActionId;
	}

	return ERR_OK;
}


