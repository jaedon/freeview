/**
	@file     ap_mheg_mgr.c
	@brief    MHEG engine 제어 application

	Description: MHEG엔진을 제어하는 application. Key와 system event를 받아 MHEG엔진이 필요한 형태로 가공하여 \n
				전달하고, MHEG이 발행하는 call 중 지연이나 통제가 필요한 이벤트들을 받아서 대신 처리한다.
	Module: AP/MHEG			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <hapi.h>
#include <octo_common.h>
#include <otl.h>
#include <svc_resources.h>
#include <svc_pipe.h>
#include <svc_si.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_3rdparty.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_rscmap.h>
#include <xmgr_mheg_base.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MGR_MHEG_MONITOR_PRIORITY				APP_PORTING_MGR_PRIORITY

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSiMonState_None			= 0,

	eSiMonState_ActionTaken,
	eSiMonState_RequestTune,
	eSiMonState_SiMonitoring,

	eSiMonState_EndOfCase
} xmgrMhegSiMonState_e;

typedef struct
{
	HUINT32					 ulLiveActionId;		// 지켜 볼 Live의 Action ID
	HUINT32					 ulActionId;			// Background Job의 Action ID
	Handle_t					hAction;
	Handle_t				 hSvc;
	HUINT32					 ulDemuxId;

	xmgrMhegSiMonState_e	 eState;


} xmgrMhegSiMonContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC xmgrMhegSiMonContext_t		 stMhegMonContext;

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC xmgrMhegSiMonContext_t *xmgr_mhegbase_simon_getContext (void)
{
	return &stMhegMonContext;
}

#if defined(CONFIG_DEBUG)
STATIC const HINT8 * xmgr_mhegbase_simon_StrState(xmgrMhegSiMonState_e eSiMonState)
{
	switch (eSiMonState)
	{
		ENUM_TO_STR(eSiMonState_None);
		ENUM_TO_STR(eSiMonState_ActionTaken);
		ENUM_TO_STR(eSiMonState_RequestTune);
		ENUM_TO_STR(eSiMonState_SiMonitoring);
		ENUM_TO_STR(eSiMonState_EndOfCase);
		default:
			break;
	}
	return ("Unknown");
}
#endif

#define _____MHEG_MGR_SI_MONITOR__

STATIC HERROR xmgr_mhegbase_simon_takeAction (xmgrMhegSiMonContext_t *pstContext, Handle_t hSvc, HUINT32 ulLiveActionId)
{
	HUINT32			 ulBgActId = (HUINT32)-1;
	HUINT32			 ulDemuxId = (HUINT32)-1;
	DbSvc_TsInfo_t	 stTsInfo;
	DxTuneParam_t	 stTuneParam;
	HERROR			 hErr;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("(+) %s \n", xmgr_mhegbase_simon_StrState(pstContext->eState));
#endif

	if (pstContext->eState >= eSiMonState_ActionTaken)
	{
		HxLOG_Error("Already the action was taken... eState(%d)\n", pstContext->eState);
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle(hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_GetTsInfoByServiceHandle err: hSvc(0x%08x)\n", hSvc);
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_GetEmptyBgActionId(&ulBgActId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetEmptyBgActionId err\n");
		return ERR_FAIL;
	}

	hErr = OTL_CONV_DbTuningInfo2DxTuneParam(&stTsInfo, &stTuneParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetEmptyBgActionId err\n");
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_SetupBgTune(ulBgActId, &stTuneParam, (eAmPriority_BGJOB - 2));
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_SetupBgTune err\n");
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_Take(ulBgActId, eActionType_BG_TUNE, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_Take err\n");
		return ERR_FAIL;
	}

	pstContext->ulActionId = ulBgActId;
	pstContext->hAction = SVC_PIPE_IncreaseActionVersion(ulBgActId);
	pstContext->hSvc = hSvc;
	pstContext->eState = eSiMonState_ActionTaken;
	pstContext->ulLiveActionId = ulLiveActionId;

	hErr = SVC_PIPE_GetResourceId(ulBgActId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if ((ERR_OK == hErr) && (pstContext->ulDemuxId != ulDemuxId))
	{
		BUS_SendMessage(NULL, eMEVT_3RD_MHEG_DEMUX_CHANGED, SVC_PIPE_GetActionHandle(ulBgActId), ulDemuxId, pstContext->ulLiveActionId, 0);
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("(-) pstContext->hAction(0x%x) , pstContext->ulActionId(0x%x) %s \n", pstContext->hAction, pstContext->ulActionId, xmgr_mhegbase_simon_StrState(pstContext->eState));
#endif

	return ERR_OK;
}

STATIC HERROR xmgr_mhegbase_simon_releaseAction (xmgrMhegSiMonContext_t *pstContext)
{
	Handle_t		hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	HERROR		hErr;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("(+) hAction(0x%x) %s \n", hAction, xmgr_mhegbase_simon_StrState(pstContext->eState));
#endif

	if (eSiMonState_SiMonitoring == pstContext->eState)
	{
		hErr = SVC_SI_StopLive(hAction, HANDLE_NULL, FALSE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_SI_StopLive err:\n");
		}

		pstContext->eState = eSiMonState_RequestTune;
	}

	if (eSiMonState_RequestTune == pstContext->eState)
	{
		hErr = SVC_CH_StopTune(hAction, (HUINT16)-1, eAsyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_CH_StopTune err:\n");
		}

		pstContext->eState = eSiMonState_ActionTaken;
	}

	if (eSiMonState_ActionTaken == pstContext->eState)
	{
		hErr = MGR_ACTION_Release(pstContext->ulActionId, eActionType_BG_TUNE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("MGR_ACTION_Release err:\n");
		}

		pstContext->eState = eSiMonState_None;
	}
	
#if defined(CONFIG_DEBUG)
	HxLOG_Debug("(-) %s \n", xmgr_mhegbase_simon_StrState(pstContext->eState));
#endif

	return ERR_OK;
}

STATIC HERROR xmgr_mhegbase_simon_tune (xmgrMhegSiMonContext_t *pstContext)
{
	Handle_t		 hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);
	DbSvc_TsInfo_t	 stTsInfo;
	DxTuneParam_t	 stTuneParam;
	SvcRsc_Info_t	 stRscInfo;
	HERROR			 hErr;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("(+) %s \n", xmgr_mhegbase_simon_StrState(pstContext->eState));
#endif

	if (pstContext->eState != eSiMonState_ActionTaken)
	{
		HxLOG_Error("Only the state(%d) can be entered: eState(%d)\n", eSiMonState_ActionTaken, pstContext->eState);
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle(pstContext->hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_GetTsInfoByServiceHandle err: hSvc(0x%08x)\n", pstContext->hSvc);
		return ERR_FAIL;
	}

	hErr = OTL_CONV_DbTuningInfo2DxTuneParam(&stTsInfo, &stTuneParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetEmptyBgActionId err\n");
		return ERR_FAIL;
	}

	hErr = MGR_RSC_GetResourceInfo(pstContext->ulActionId, &stRscInfo);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("MGR_RSC_GetResourceInfo err: ulActionId(%d)\n", pstContext->ulActionId);
		return ERR_FAIL;
	}

	hErr = SVC_CH_StartTune(hAction, &stRscInfo, stTsInfo.usTsIdx, &stTuneParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_CH_StartTune err\n");
		return ERR_FAIL;
	}

	pstContext->eState = eSiMonState_RequestTune;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("(-) %s \n", xmgr_mhegbase_simon_StrState(pstContext->eState));
#endif

	return ERR_OK;
}


STATIC BUS_Result_t xmgr_mhegbase_simon_mevtBusCreate (xmgrMhegSiMonContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			 ulLiveActionId = (HUINT32)p2;
	Handle_t		 hSvc = (Handle_t)p1;
	HERROR			 hErr;

	HxSTD_MemSet(pstContext, 0, sizeof(xmgrMhegSiMonContext_t));
	pstContext->ulDemuxId = (HUINT32)-1;

	hErr = xmgr_mhegbase_simon_takeAction(pstContext, hSvc, ulLiveActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_mhegbase_simon_takeAction err: hSvc(0x%08x)\n", hSvc);

		BUS_MGR_Destroy(NULL);
		return MESSAGE_PASS;
	}

	hErr = xmgr_mhegbase_simon_tune(pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_mhegbase_simon_tune err: hSvc(0x%08x)\n", hSvc);

		BUS_MGR_Destroy(NULL);
		return MESSAGE_PASS;
	}

	BUS_SendMessage(NULL, eMEVT_3RD_MHEG_MONITORING_STARTED, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, pstContext->ulLiveActionId, 0);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_mhegbase_simon_mevtBusDestroy (xmgrMhegSiMonContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			 hErr;

	hErr = xmgr_mhegbase_simon_releaseAction(pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_mhegbase_simon_releaseAction err");
	}

	BUS_SendMessage(NULL, eMEVT_3RD_MHEG_MONITORING_STOPPED, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, pstContext->ulLiveActionId, 0);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_mhegbase_simon_msgAmActionSetup (xmgrMhegSiMonContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrAction_Type_e 	 		eActType = (MgrAction_Type_e)p1;
	HERROR					hErr;

	switch (eActType)
	{
	// 추후 시나리오가 추가되면 여기에 넣도록 한다.
	case eViewType_WTV:
		// 현재 TSR이 진행중인데, 그것이 WatchTV와 충돌한다면 제거해야 한다.
		// 실제 액션을 잡기 전에 무조건 제거해줘야 Live Tuner가 막 바뀌는 문제를 막는다.
		{
			hErr = xmgr_mhegbase_simon_releaseAction(pstContext);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("xmgr_mhegbase_simon_releaseAction err");
			}
		}
		break;

	default:
		break;
	}

	//--------------------------------------------------------
	//NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_mhegbase_simon_mevtLiveNotifySvcChanged (xmgrMhegSiMonContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hHandle);
	Handle_t		 	hSvc = (Handle_t)p1;
	HERROR			hErr;

	HxLOG_Debug("pstContext->ulActionId(0x%x) pstContext->ulLiveActionId(0x%x) ulActionId(0x%x) pstContext->hSvc(0x%x) hSvc(0x%x)\n",
		pstContext->ulActionId, pstContext->ulLiveActionId, ulActionId, pstContext->hSvc , hSvc);

	if (pstContext->ulLiveActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	//Media, OnDemand, Record Playback 진입 후 Live로 전환 시 Mheg 뜨지 않는 문제 수정.
#if 0
	if (pstContext->hSvc == hSvc)
	{
		HxLOG_Print("Already same service : hSvc(0x%08x)\n", hSvc);
		return MESSAGE_PASS;
	}
#endif

	hErr = xmgr_mhegbase_simon_releaseAction(pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_mhegbase_simon_takeAction err: hSvc(0x%08x)\n", hSvc);

		BUS_MGR_Destroy(NULL);
		return MESSAGE_PASS;
	}

	hErr = xmgr_mhegbase_simon_takeAction(pstContext, hSvc, pstContext->ulLiveActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_mhegbase_simon_takeAction err: hSvc(0x%08x)\n", hSvc);

		BUS_MGR_Destroy(NULL);
		return MESSAGE_PASS;
	}

	hErr = xmgr_mhegbase_simon_tune(pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_mhegbase_simon_tune err: hSvc(0x%08x)\n", hSvc);

		BUS_MGR_Destroy(NULL);
		return MESSAGE_PASS;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_mhegbase_simon_sevtChLocked (xmgrMhegSiMonContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulActionId = SVC_PIPE_GetActionId(hHandle);
	MgrLive_START_t		 stStartArg;
	SvcSi_LiveSpec_t	 	stSiSpec;
	HERROR				 hErr;

	HxLOG_Debug(" pstContext->ulActionId(0x%x) pstContext->ulLiveActionId(0x%x) ulActionId(0x%x) pstContext->eState(0x%x) \n",
		pstContext->ulActionId, pstContext->ulLiveActionId, ulActionId, pstContext->eState);
#if defined(CONFIG_DEBUG)
	HxLOG_Debug("(+) %s \n", xmgr_mhegbase_simon_StrState(pstContext->eState));
#endif

	if (pstContext->hAction != hHandle)
	{
		HxLOG_Warning(" pstContext->hAction (0x%x)  hHandle(0x%x) \n",pstContext->hAction , hHandle);
		return MESSAGE_PASS;
	}

	if (eSiMonState_RequestTune != pstContext->eState)
	{
		HxLOG_Warning(" eSiMonState_RequestTune (0x%x)  pstContext->eState(0x%x) \n",eSiMonState_RequestTune , pstContext->eState);
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(&stStartArg, 0, sizeof(MgrLive_START_t));
	stStartArg.eLiveMode = eMgrLiveMode_MainLive;
	stStartArg.hMasterSvc = pstContext->hSvc;
	stStartArg.hService = pstContext->hSvc;

	hErr = MGR_LIVE_GetLiveSiSpec(&stStartArg, &stSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_LIVE_GetLiveSiSpec err: ulActionId(%d), hSvc(0x%08x)\n", pstContext->ulActionId, pstContext->hSvc);
		goto ERROR;
	}

	hErr = SVC_SI_StartLive(hHandle, &stSiSpec, pstContext->hSvc, FALSE, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_SI_StartLive err: ulActionId(%d), hSvc(0x%08x)\n", pstContext->ulActionId, pstContext->hSvc);
		goto ERROR;
	}

	pstContext->eState = eSiMonState_SiMonitoring;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("(-) %s \n", xmgr_mhegbase_simon_StrState(pstContext->eState));
#endif

	return MESSAGE_PASS;

ERROR:
	BUS_MGR_Destroy(NULL);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_mhegbase_simon_sevtSiPmt (xmgrMhegSiMonContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulActionId = SVC_PIPE_GetActionId(hHandle);

	HxLOG_Debug("pstContext->ulActionId(0x%x) pstContext->ulLiveActionId(0x%x) ulActionId(0x%x) pstContext->eState(0x%x) p3(0x%x) \n",
		pstContext->ulActionId, pstContext->ulLiveActionId, ulActionId, pstContext->eState, p3);

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("(+) %s \n", xmgr_mhegbase_simon_StrState(pstContext->eState));
#endif
	if (pstContext->hAction != hHandle)
	{
		HxLOG_Warning("pstContext->hAction (0x%x)  hHandle(0x%x) \n", pstContext->hAction , hHandle);
		return MESSAGE_PASS;
	}

	if (eSiMonState_SiMonitoring != pstContext->eState)
	{
		return MESSAGE_PASS;
	}

	BUS_SendMessage(NULL, eMEVT_3RD_MHEG_PMT_CHANGED, hHandle, pstContext->hSvc, pstContext->ulLiveActionId, p3);

 	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_mhegbase_simon_sevtSiPmtPidChanged (xmgrMhegSiMonContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulActionId = SVC_PIPE_GetActionId(hHandle);
	MgrLive_START_t		 stStartArg;
	SvcSi_LiveSpec_t	 stSiSpec;
	HERROR				 hErr;

	HxLOG_Debug("pstContext->ulActionId(0x%x) pstContext->ulLiveActionId(0x%x) ulActionId(0x%x) pstContext->eState(0x%x) \n",
		pstContext->ulActionId, pstContext->ulLiveActionId, ulActionId, pstContext->eState);

	if (pstContext->hAction != hHandle)
	{
		return MESSAGE_PASS;
	}

	if (eSiMonState_SiMonitoring != pstContext->eState)
	{
		return MESSAGE_PASS;
	}

	hErr = SVC_SI_StopLive(hHandle, HANDLE_NULL, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_SI_StopLive err:\n");
	}

	pstContext->eState = eSiMonState_RequestTune;

	HxSTD_MemSet(&stStartArg, 0, sizeof(MgrLive_START_t));
	stStartArg.eLiveMode = eMgrLiveMode_MainLive;
	stStartArg.hMasterSvc = pstContext->hSvc;
	stStartArg.hService = pstContext->hSvc;

	hErr = MGR_LIVE_GetLiveSiSpec(&stStartArg, &stSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_LIVE_GetLiveSiSpec err: ulActionId(%d), hSvc(0x%08x)\n", pstContext->ulActionId, pstContext->hSvc);
		goto ERROR;
	}

	hErr = SVC_SI_StartLive(hHandle, &stSiSpec, pstContext->hSvc, FALSE, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_SI_StartLive err: ulActionId(%d), hSvc(0x%08x)\n", pstContext->ulActionId, pstContext->hSvc);
		goto ERROR;
	}

	pstContext->eState = eSiMonState_SiMonitoring;

ERROR:

	BUS_SendMessage(NULL, eMEVT_3RD_MHEG_PMT_CHANGED, hHandle, pstContext->hSvc, pstContext->ulLiveActionId, eSEVT_SI_PMT_PIDCHANGED);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_mhegbase_simon_mevtBgStart (xmgrMhegSiMonContext_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		 ulActionId = SVC_PIPE_GetActionId(hHandle);
	Handle_t		 hSvc = (Handle_t)p1;
	HERROR		 hErr;

	HxLOG_Debug("pstContext->ulActionId(0x%x) pstContext->ulLiveActionId(0x%x) ulActionId(0x%x) pstContext->hSvc(0x%x) hSvc(0x%x)\n",
		pstContext->ulActionId, pstContext->ulLiveActionId, ulActionId, pstContext->hSvc , hSvc);


	if (pstContext->ulLiveActionId != ulActionId)
	{
		return MESSAGE_PASS;
	}

	//Media, OnDemand, Record Playback 진입 후 Live로 전환 시 Mheg 뜨지 않는 문제 수정.
#if 0
	if (pstContext->hSvc == hSvc)
	{
		HxLOG_Print("Already same service : hSvc(0x%08x)\n", hSvc);
		return MESSAGE_PASS;
	}
#endif

	hErr = xmgr_mhegbase_simon_releaseAction(pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_mhegbase_simon_takeAction err: hSvc(0x%08x)\n", hSvc);

		BUS_MGR_Destroy(NULL);
		return MESSAGE_PASS;
	}

	hErr = xmgr_mhegbase_simon_takeAction(pstContext, hSvc, pstContext->ulLiveActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_mhegbase_simon_takeAction err: hSvc(0x%08x)\n", hSvc);

		BUS_MGR_Destroy(NULL);
		return MESSAGE_PASS;
	}

	hErr = xmgr_mhegbase_simon_tune(pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_mhegbase_simon_tune err: hSvc(0x%08x)\n", hSvc);

		BUS_MGR_Destroy(NULL);
		return MESSAGE_PASS;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t  xproc_mhegbase_simonProto (xmgrMhegSiMonContext_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Debug("eMEVT_BUS_CREATE \n");
		eResult = xmgr_mhegbase_simon_mevtBusCreate(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Debug("eMEVT_BUS_DESTROY \n");
		eResult = xmgr_mhegbase_simon_mevtBusDestroy(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_ACTION_SETUP_REQUESTED:
		HxLOG_Debug("eMEVT_ACTION_SETUP_REQUESTED \n");
		eResult = xmgr_mhegbase_simon_msgAmActionSetup(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		HxLOG_Debug("eMEVT_LIVE_NOTIFY_SVC_CHANGED \n");
		eResult = xmgr_mhegbase_simon_mevtLiveNotifySvcChanged(pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_CH_LOCKED:
		HxLOG_Debug("eSEVT_CH_LOCKED \n");
		eResult = xmgr_mhegbase_simon_sevtChLocked(pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_PMT:
	case eSEVT_SI_PMT_TIMEOUT:
	case eSEVT_SI_PMT_PIDREMOVED:
		HxLOG_Debug("eSEVT_SI_PMT(0x%x) || eSEVT_SI_PMT_PIDREMOVED(0x%x)|| eSEVT_SI_PMT_TIMEOUT(0x%x) => Message (0x%x) \n",
			eSEVT_SI_PMT, eSEVT_SI_PMT_TIMEOUT, eSEVT_SI_PMT_PIDREMOVED, message);
		p3 = message;
		eResult = xmgr_mhegbase_simon_sevtSiPmt(pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_SI_PMT_PIDCHANGED:
		HxLOG_Debug("[%s][%d] eSEVT_SI_PMT_PIDCHANGED\n", __FUNCTION__, __LINE__);
		eResult = xmgr_mhegbase_simon_sevtSiPmtPidChanged(pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_CH_NO_SIGNAL:
		break;

	case eMEVT_MHEG_BG_START:
		HxLOG_Debug("eMEVT_MHEG_BG_START \n");
		eResult = xmgr_mhegbase_simon_mevtBgStart(pstContext, hHandle, p1, p2, p3);
		break;

	default:
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;

}

STATIC BUS_Result_t  xproc_mhegbase_simon0 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrMhegSiMonContext_t		*pstContext = xmgr_mhegbase_simon_getContext();

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HUINT8*)__FUNCTION__);
	}

	return xproc_mhegbase_simonProto(pstContext, message, hHandle, p1, p2, p3);
}

#define _____MHEG_MGR_BASE__

STATIC BUS_Result_t  xmgr_mhegbase_mevtBusCreate (Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t  xmgr_mhegbase_mevtLiveNotifySvcChanged (Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			 ulActionId = SVC_PIPE_GetActionId(hHandle);
	Handle_t		 hSvc = (Handle_t)p1;

	if (MGR_ACTION_GetMainActionId() == ulActionId)
	{
		if (BUS_MGR_Get(xproc_mhegbase_simon0) == NULL)
		{
			BUS_MGR_Create(NULL, (MGR_MHEG_MONITOR_PRIORITY + 1), xproc_mhegbase_simon0, hHandle, hSvc, ulActionId, 0);
		}
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t  xproc_mhegbase (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Debug("eMEVT_BUS_CREATE \n");
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HUINT8*)__FUNCTION__);
		eResult = xmgr_mhegbase_mevtBusCreate(hHandle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		HxLOG_Debug("eMEVT_LIVE_NOTIFY_SVC_CHANGED \n");
		eResult = xmgr_mhegbase_mevtLiveNotifySvcChanged(hHandle, p1, p2, p3);
		break;

	case eMEVT_BUS_READY_SHUTDOWN:
	case eMEVT_BUS_GO_SHUTDOWN:
		HxLOG_Debug("eMEVT_BUS_READY_SHUTDOWN \n");
		if (BUS_MGR_Get(xproc_mhegbase_simon0) != NULL)
		{
			eResult = BUS_MGR_Destroy(xproc_mhegbase_simon0);
		}
		break;

	default:
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;

}

/*******************************************************************/
/********************	Application Proc	*************************/
/*******************************************************************/
#define ___MEMBER_FUNCTIONS__
HERROR xmgr_mhegbase_Init (void)
{
	if (BUS_MGR_Get(xproc_mhegbase) == NULL)
	{
		BUS_MGR_Create (NULL, MGR_MHEG_MONITOR_PRIORITY, xproc_mhegbase, 0, 0, 0, 0);
	}

	return ERR_OK;
}

