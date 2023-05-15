/**
	@file     mgr_live.c
	@brief

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>
#include <linkedlist.h>
#include <svc_av.h>
#include <svc_si.h>

#include <mgr_common.h>
#include <mgr_output.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_liveota.h>
#include <mgr_dynamicupdate.h>

#include <pmgr_live.h>
#include <pmgr_model.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	MgrLive_LiveMode_e	 		eLiveMode;
	HUINT32						ulActionId;
	Handle_t					hService;
} MgrLive_UiState_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC MgrLive_UiState_Context_t 	s_astMGR_UiState_Context[eActionId_MAX];

/*******************************************************************/
/***************** Static Function Prototypes  *********************/
/*******************************************************************/

STATIC HERROR mgr_live_StartCbFunc(HUINT32 ulActionId, BUS_Callback_t pfnProc, MgrLive_START_t *pstStartArg)
{
	Handle_t		hAction = SVC_PIPE_GetActionHandle (ulActionId);
	BUS_Result_t	eResult = ERR_BUS_NO_ERROR;

	if (NULL == pfnProc)
	{
		HxLOG_Error ("ActionId(%d) mgr_live_StartCbFunc NULL: \n", ulActionId);
		return ERR_FAIL;
	}

	// Action이 살아있지 않으면 살리고 이미 도는게 있으면 Start Service 메시지만 보낸다.
	if (NULL == BUS_MGR_Get (pfnProc))
	{
		eResult = BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, pfnProc, hAction, (HINT32)pstStartArg, 0, 0);
		if (eResult == ERR_BUS_INITIAL_FAILED)
		{
			HxLOG_Error ("ActionId(%d) BUS_MGR_Create() failed.. \n", ulActionId);
			return ERR_FAIL;
		}
	}

	eResult = BUS_SendMessage (pfnProc, eMEVT_LIVE_START_SVC, hAction, (HINT32)pstStartArg, 0, 0);
	if (eResult == ERR_BUS_INITIAL_FAILED)
	{
		HxLOG_Error ("ActionId(%d) eMEVT_LIVE_START_SVC failed.. \n", ulActionId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR mgr_live_StopCbFunc(HUINT32 ulActionId, BUS_Callback_t pfnProc)
{
	if (NULL == pfnProc)
	{
		HxLOG_Error ("ActionId(%d) mgr_live_StopCbFunc NULL: \n", ulActionId);
		return ERR_FAIL;
	}

	// Action이 살아있을때만 destroy 한다
	if (NULL == BUS_MGR_Get (pfnProc))
	{
		HxLOG_Error ("ActionId(%d) Action Proc not alive: \n", ulActionId);
		return ERR_FAIL;
	}

	BUS_MGR_Destroy (pfnProc);

	return ERR_OK;
}

STATIC HUINT8* mgr_live_GetLiveModeStr(MgrLive_LiveMode_e eLiveMode)
{
	switch(eLiveMode)
	{
	ENUM_TO_STR(eMgrLiveMode_None);
	ENUM_TO_STR(eMgrLiveMode_MainLive);
	ENUM_TO_STR(eMgrLiveMode_PipLive);
	ENUM_TO_STR(eMgrLiveMode_LiveStreaming);
	ENUM_TO_STR(eMgrLiveMode_FccLive);

	ENUM_TO_STR(eMgrLiveMode_EndOfCase);
	default:			break;
	}

	HxLOG_Print("Unkown live mode [%d]\n", eLiveMode);

	return "UNKNOWN";
}

STATIC HERROR mgr_live_StopLive (HUINT32 ulActionId)
{
	BUS_Callback_t			 pfnProc;

	// reset ui state
	pmgr_live_ResetUiStateView(ulActionId, FALSE);
	pmgr_live_ResetUiStateLock(ulActionId, FALSE);

	/**
	  *		Live view
	**/
	pfnProc = NULL;
	pmgr_live_GetLiveViewProc(ulActionId, &pfnProc);
	if (mgr_live_StopCbFunc(ulActionId, pfnProc) != ERR_OK)
	{
		HxLOG_Error ("ActionId(%d) pmgr_live_GetLiveViewProc() failed! \n", ulActionId);
	}

	/**
	  *		Live parental control
	**/
	pfnProc = NULL;
	pmgr_live_GetLiveLockProc(ulActionId, &pfnProc);
	if (mgr_live_StopCbFunc(ulActionId, pfnProc) != ERR_OK)
	{
		HxLOG_Error ("ActionId(%d) pmgr_live_GetLiveLockProc() failed! \n", ulActionId);
	}

	/**
	  *		Complementary, SW update
	**/
#if defined(CONFIG_SUPPORT_FCC)
	if (ulActionId == MGR_ACTION_GetMainActionId())
#endif
	{
		HERROR hErr = ERR_FAIL;
		MgrLive_LiveMode_e	eLiveMode;

		hErr = pmgr_live_GetLiveMode (ulActionId, &eLiveMode);
		if (ERR_FAIL == hErr)
		{
			HxLOG_Error ("Not found Context . ActionId(%d)\n", ulActionId);
		}
		else
		{
			if (eMgrLiveMode_LiveStreaming != eLiveMode)
			{
				pfnProc = NULL;
				pmgr_live_GetLiveCompProc(ulActionId, &pfnProc);
				if (mgr_live_StopCbFunc(ulActionId, pfnProc) != ERR_OK)
				{
					HxLOG_Error ("ActionId(%d) pmgr_live_GetLiveCompProc() failed \n", ulActionId);
				}
			}

			if (eMgrLiveMode_MainLive == eLiveMode)
			{
				MGR_LIVEOTA_Stop();
			}
		}
	}

#if defined(CONFIG_SUPPORT_FCC)
	// fcc에 선적용해보고 괜찮으면 기본 적용 예정
	s_astMGR_UiState_Context[ulActionId].eLiveMode 	= eMgrLiveMode_None;
	s_astMGR_UiState_Context[ulActionId].hService 	= HANDLE_NULL;
#endif

	return ERR_OK;
}

#if defined(CONFIG_SUPPORT_FCC)
STATIC HERROR mgr_live_FindFccActionIdBySvc(Handle_t hSvc, HUINT32 *pulActionId)
{
	MgrLive_UiState_Context_t	*pstMgrLiveContext = NULL;
	HUINT32 					ulActionId;
	HBOOL						bFccIsRunning = FALSE;

	if (pulActionId == NULL)
	{
		return ERR_FAIL;
	}

	for (ulActionId = eActionId_VIEW_FIRST; ulActionId <= eActionId_VIEW_LAST; ulActionId++)
	{
		pstMgrLiveContext = &s_astMGR_UiState_Context[ulActionId];

		// 현재 context에 fcc로 동일한 svc가 할당되어 있다면 이를 그대로 사용한다
		if ((pstMgrLiveContext->eLiveMode == eMgrLiveMode_FccLive) && (pstMgrLiveContext->hService == hSvc))
		{
			bFccIsRunning = SVC_AV_isRunning(ulActionId);

			if( bFccIsRunning == FALSE)
			{
				HxLOG_Error("ulActionId = [%d], It is Not Running!!! \n", ulActionId);
			}
			else
			{
				*pulActionId = ulActionId;
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

STATIC HUINT32 mgr_live_GetFccActionId(void)
{
	HUINT32						ulLastFccActId = (HUINT32)-1;
	HUINT32						ulLastFccActCnt = (HUINT32)-1;
	HUINT32 					ulActionId;
	HERROR						hErr;

	for (ulActionId = eActionId_VIEW_FIRST; ulActionId <= (eActionId_VIEW_LAST - 1); ulActionId++)
	{
		if (eActionType_NONE == MGR_ACTION_GetType(ulActionId))
		{
			return ulActionId;
		}
	}

	// 현재 사용하는 FCC Action중 가장 과거에 잡은 것을 준다.
	for (ulActionId = eActionId_VIEW_FIRST; ulActionId <= (eActionId_VIEW_LAST - 1); ulActionId++)
	{
		if (eViewType_FCC == MGR_ACTION_GetType(ulActionId))
		{
			HUINT32			 ulCnt = 0;

			hErr = MGR_ACTION_GetActionSetupCount(ulActionId, &ulCnt);
			if (ERR_OK == hErr)
			{
				if (((HUINT32)-1 == ulLastFccActId) || (ulLastFccActCnt > ulCnt))
				{
					ulLastFccActId = ulActionId;
					ulLastFccActCnt = ulCnt;
				}
			}
		}
	}

	return ulLastFccActId;
}

STATIC HERROR mgr_live_StopAllFccSvc(void)
{
	HUINT32 ulActionId;

	for (ulActionId = eActionId_VIEW_FIRST; ulActionId < NUM_VIEW_ACTION; ulActionId++)
	{
		if (s_astMGR_UiState_Context[ulActionId].eLiveMode == eMgrLiveMode_FccLive)
		{
			mgr_live_StopLive(ulActionId);
		}
	}

	return ERR_OK;
}

#if 0
STATIC HERROR mgr_live_CheckMainSvcConflict(Handle_t hSvc, HBOOL *pbStopAllFcc)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HERROR					hErr;
	DbSvc_Info_t			stSvcInfo;
	DbSvc_TsInfo_t			stTsInfo;
	DxTuneParam_t 			stTuningInfo;
	DxSat_Polarization_e 	eMainPolarization;
	HUINT32					ulActId;

	MgrLive_UiState_Context_t	*pstMgrLiveContext = NULL;

	if (pbStopAllFcc == NULL)		{ return ERR_FAIL; }
	*pbStopAllFcc = FALSE;

	// get main svc locking info
	HxSTD_MemSet(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
	hErr = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stSvcInfo), &stTsInfo);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}
	eMainPolarization = stTsInfo.stTuningParam.sat.ePolarization;

	// fcc svc를 뒤져서 main svc와 polarization이 다른 svc가 있다면 stop 시키자
	for (ulActId = eActionId_VIEW_FIRST; ulActId <= eActionId_VIEW_LAST; ulActId++)
	{
		pstMgrLiveContext = &s_astMGR_UiState_Context[ulActId];
		if (pstMgrLiveContext->eLiveMode == eMgrLiveMode_FccLive)
		{
			HxSTD_MemSet(&stTuningInfo, 0x00, sizeof(DxTuneParam_t));
			hErr = SVC_CH_GetLockedInfo(ulActId, &stTuningInfo);
			if (hErr != ERR_FAIL)
			{
				if (eMainPolarization != stTuningInfo.sat.tuningInfo.ePolarization)
				{
					// found diff polarization
					*pbStopAllFcc = TRUE;
					break;
				}
			}
		}
	}

	return ERR_OK;
#else
	return ERR_OK;
#endif
}
#endif

STATIC HERROR mgr_live_ChangeMainSvc(MgrLive_START_t *pstStartArg, HUINT32 ulNewMainActionId)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32 ulOldMainActionId = MGR_ACTION_GetMainActionId();
	HUINT32			ulLastViewprocTuneStatus = 0;
	BUS_Callback_t	 pfnProc;

	if (ulNewMainActionId >= NUM_VIEW_ACTION)
	{
		// main svc가 fcc svc context에 없다
		HxLOG_Error("Invalid fcc action id(%d) \n", ulNewMainActionId);
		return ERR_FAIL;
	}

	/* main action 을 변경하기 전에 epg를 중지시켜야 한다 */
	hErr = pmgr_live_StopAllEpg(SVC_PIPE_GetActionHandle(ulOldMainActionId));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error ! pmgr_live_StopEpg()\n");
	}

	/* main action 을 변경하기 전에 Dynamic Update를 중지시켜야 한다 */
#if defined(CONFIG_MW_DYNAMIC_UPDATE)
	hErr = MGR_DYNAMICUPDATE_SvcStop(SVC_PIPE_GetActionHandle(ulOldMainActionId));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error ! pmgr_dynamicupdate_SvcStop()\n");
	}
#endif

	/* view proc에서 관리하는 cas/si 등의 mw svc를 중지시킨다. new main이 시작되면서 다시 시작될 것이다 */
	BUS_SendMessage (NULL, eMEVT_LIVE_STOP_MW_SVC_FOR_FCC, SVC_PIPE_GetActionHandle(ulOldMainActionId), ulOldMainActionId, ulNewMainActionId, 0);
	BUS_SendMessage (NULL, eMEVT_LIVE_STOP_MW_SVC_FOR_FCC, SVC_PIPE_GetActionHandle(ulNewMainActionId), ulOldMainActionId, ulNewMainActionId, 0);

	/* start new main svc */
	// view id - act id mapping table도 업데이트 해주고
	hErr = MGR_ACTION_OverwriteViewIdTable(0 /* main live이니까 0으로 고정 */, ulNewMainActionId);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("MGR_ACTION_UpdateViewIdTable() failed...!! viewId(0) newActId(%d) \n", ulNewMainActionId);
	}

	// action manager에 새로운 main action id를 설정해 주고
	MGR_ACTION_SetMainActionId(ulNewMainActionId);
	hErr = MGR_ACTION_ChangeType(ulNewMainActionId, eViewType_FCC, eViewType_WTV);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_ChangeType() failed.. \n");
	}

	hErr = MGR_ACTION_ChangePriority(ulNewMainActionId, eAmPriority_VIEW_ACTION);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_ChangePriority() failed.. \n");
	}

	// old main action id도 property 변경해주고
	hErr = MGR_ACTION_ChangeType(ulOldMainActionId, eViewType_WTV, eViewType_FCC);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_ChangeType() failed.. \n");
	}

	hErr = MGR_ACTION_ChangePriority(ulOldMainActionId, eAmPriority_BGJOB);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_ChangePriority() failed.. \n");
	}

	// stop subtitle
#if defined(CONFIG_MW_TELETEXT)
	pmgr_live_StopSubtitle(SVC_PIPE_GetActionHandle(ulNewMainActionId), eSyncMode);
#endif

	// main view 전환을 위해 msg를 날린다
	BUS_SendMessage (NULL, eMEVT_LIVE_SET_FCC_MAINVIEW_ACTION, SVC_PIPE_GetActionHandle(ulNewMainActionId), (HINT32)ulOldMainActionId, 0, 0);

	// Lock Proc이 View Proc에서 보내 eMEVT_LIVE_NOTIFY_SVC_CHANGED 를 먼저 받을수 있도록  여기서 restart
	pfnProc = NULL;
	pmgr_live_GetLiveLockProc (ulOldMainActionId, &pfnProc);
	if (pfnProc != NULL)
	{
		if (mgr_live_StopCbFunc(ulOldMainActionId, pfnProc) != ERR_OK)
		{
			HxLOG_Error ("lock proc : ActionId(%d) mgr_live_StopCbFunc() failed \n", ulOldMainActionId);
		}

		MGR_LIVE_SvcLockResetPassed(ulNewMainActionId);

		pmgr_live_GetLiveLockProc (ulNewMainActionId, &pfnProc);
		if (mgr_live_StartCbFunc(ulNewMainActionId, pfnProc, pstStartArg) != ERR_OK)
		{
			HxLOG_Error ("lock proc : ActionId(%d) mgr_live_StartCbFunc() failed \n", ulNewMainActionId);
		}
	}
	else
	{
		HxLOG_Error ("lock proc : pmgr_live_GetLiveLockProc() failed.. actionId(%d) \n", ulNewMainActionId);
	}

	// restart complementary proc
	pfnProc = NULL;
	pmgr_live_GetLiveCompProc(ulNewMainActionId, &pfnProc);
	if (pfnProc != NULL)
	{
		if (mgr_live_StopCbFunc(ulNewMainActionId, pfnProc) != ERR_OK)
		{
			HxLOG_Error ("comp proc : ActionId(%d) mgr_live_StopCbFunc() failed \n", ulOldMainActionId);
		}

		if (mgr_live_StartCbFunc(ulNewMainActionId, pfnProc, pstStartArg) != ERR_OK)
		{
			HxLOG_Error ("comp proc : ActionId(%d) mgr_live_StartCbFunc() failed \n", ulNewMainActionId);
		}
	}
	else
	{
		HxLOG_Error ("comp proc : pmgr_live_GetLiveCompProc() failed..actionId(%d) \n", ulNewMainActionId);
	}

	/* 현재 view state 공지 */
	BUS_SendMessage(NULL, eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED, SVC_PIPE_GetActionHandle(ulNewMainActionId), 0, 0, 0);

	// view proc에서 last tune status를 가져와서 이를 msg로 날린다 (om_media, lock proc에서 받음)
	if (pmgr_live_GetLastTuneStatus(ulNewMainActionId, &ulLastViewprocTuneStatus) != ERR_OK)
	{
		HxLOG_Error ("pmgr_live_GetLastTuneStatus() failed.. actionId(%d) \n", ulNewMainActionId);
	}
	else
	{
		BUS_SendMessage (NULL, eMEVT_LIVE_NOTIFY_FCC_TUNE_STATUS, SVC_PIPE_GetActionHandle(ulNewMainActionId), ulLastViewprocTuneStatus, 0, 0);
	}

	/* start subtitle */
	pmgr_live_StartSubtitle(SVC_PIPE_GetActionHandle(ulNewMainActionId));

	return ERR_OK;
}

STATIC HERROR mgr_live_CheckGarbageMainLive(HUINT32 ulMainActionId)
{
	MgrLive_UiState_Context_t	*pstMgrLiveContext = NULL;
	HUINT32 					ulActionId;

	for (ulActionId = eActionId_VIEW_FIRST; ulActionId <= eActionId_VIEW_LAST; ulActionId++)
	{
		pstMgrLiveContext = &s_astMGR_UiState_Context[ulActionId];

		// 새로 할당된 main action id가 아닌데 live mode가 main live이면 stop 시킨다
		if ((ulMainActionId != ulActionId)
			&& (pstMgrLiveContext->eLiveMode == eMgrLiveMode_MainLive))
		{
			HxLOG_Error("Found garbage main action id(%d) : curMainActionId(%d)\n", ulActionId, ulMainActionId);
			MGR_LIVE_Stop(ulActionId);
		}
	}

	return ERR_OK;
}

STATIC HERROR mgr_live_ClearActionIdMapTable(void)
{
	MgrLive_UiState_Context_t	*pstMgrLiveContext = NULL;
	HUINT32 					 ulActionId;
	HBOOL						 bStillRunning = FALSE;

	for (ulActionId = eActionId_VIEW_FIRST; ulActionId <= eActionId_VIEW_LAST; ulActionId++)
	{
		pstMgrLiveContext = &s_astMGR_UiState_Context[ulActionId];

		if (pstMgrLiveContext->eLiveMode != eMgrLiveMode_None)
		{
			bStillRunning = TRUE;
		}
		else
		{
			pstMgrLiveContext->ulActionId = ulActionId;
			pstMgrLiveContext->hService = HANDLE_NULL;
		}
	}

	if (bStillRunning != FALSE)
	{
		HxLOG_Error("Live still running.. \n");
		return ERR_OK;
	}

	// 모든 live가 stop되었다..
	// actionId - viewId mapping table을 reset하자
	MGR_ACTION_ResetViewInfo();

	// main action id가 0이 아니면 0으로 초기화 해주자
	if (MGR_ACTION_GetMainActionId() != eActionId_VIEW_FIRST)
	{
		MGR_ACTION_SetMainActionId(eActionId_VIEW_FIRST);
		SVC_AV_SetMainMediaActionId(eActionId_VIEW_FIRST);
	}

	return ERR_OK;
}
#endif

HERROR MGR_LIVE_Init (void)
{
	HUINT32 	i;

	// init context
	HxSTD_memset(s_astMGR_UiState_Context, 0x00, sizeof(s_astMGR_UiState_Context));

	for (i = 0; i < eActionId_MAX; i++)
	{
		s_astMGR_UiState_Context[i].hService = HANDLE_NULL;
		s_astMGR_UiState_Context[i].ulActionId = i;
	}

	return pmgr_live_Init();
}

#if defined(CONFIG_SUPPORT_FCC)
HERROR MGR_LIVE_FindFccActionIdBySvc(Handle_t hService, HUINT32 *pulActionId)
{
	return mgr_live_FindFccActionIdBySvc(hService, pulActionId);
}

HUINT32 MGR_LIVE_GetFccActionId(void)
{
	return mgr_live_GetFccActionId();
}

// release된 action의 context를 정리한다
HERROR MGR_LIVE_ActionNotifyReleased(HUINT32 ulActionId)
{
	if (MGR_ACTION_IsViewAction(ulActionId) != TRUE)
	{
		return ERR_FAIL;
	}

	s_astMGR_UiState_Context[ulActionId].eLiveMode 	= eMgrLiveMode_None;
	s_astMGR_UiState_Context[ulActionId].hService 	= HANDLE_NULL;

	return ERR_OK;
}
#endif

HERROR MGR_LIVE_Start (MgrLive_START_t *pstStartArg, HUINT32 *pulActionId)
{
	HUINT32						 ulActionId = eActionId_MAX;
	BUS_Callback_t				 pfnProc;
	MgrLive_UiState_Context_t	*pstContext = NULL;
	HERROR						 hErr;

	if ((NULL == pstStartArg) || (NULL == pulActionId))
	{
		return ERR_FAIL;
	}

	HxLOG_Debug ("eLiveMode(%s) start \n", mgr_live_GetLiveModeStr(pstStartArg->eLiveMode));

	switch (pstStartArg->eLiveMode)
	{
	case eMgrLiveMode_MainLive:
		ulActionId = MGR_ACTION_GetMainActionId();

#if defined(CONFIG_SUPPORT_FCC)
		hErr = mgr_live_FindFccActionIdBySvc(pstStartArg->hService, &ulActionId);
		if (hErr == ERR_OK)
		{
			// fcc svc중에 main svc가 있으니 fcc svc  -> main svc로 전환한다
			hErr = mgr_live_ChangeMainSvc(pstStartArg, ulActionId);
			if (hErr == ERR_OK)
			{
				*pulActionId = ulActionId;
				return ERR_OK;
			}

			HxLOG_Critical("mgr_live_ChangeMainSvc() failed.. \n");
		}

#if 0
		// check tunable
		{
			HBOOL 		bStopFccSvc = FALSE;

			hErr = mgr_live_CheckMainSvcConflict(pstStartArg->hService, &bStopFccSvc);
			if ((hErr == ERR_OK) && (bStopFccSvc == TRUE))
			{
				MGR_LIVE_StopAllFccSvc();
			}
		}
#endif

		// check garbage main view
		mgr_live_CheckGarbageMainLive(ulActionId);
#endif
		break;

	case eMgrLiveMode_PipLive:
		ulActionId = MGR_ACTION_GetSubActionId();
		break;

#if defined(CONFIG_SUPPORT_FCC)
	case eMgrLiveMode_FccLive:
		{
			// main live가 없다면 fcc 시작을 하지 않는다
			// chase pb 상태에서 standby 진입시에 main live가 시작되자마자 shutdown msg를 받아서 죽고
			// 이후 fcc가 시작되면서 main action id를 가지는 문제 수정

			MgrLive_UiState_Context_t	*pstMgrLiveContext = NULL;
			HUINT32						 ulMainActionId = MGR_ACTION_GetMainActionId();

			pstMgrLiveContext = &s_astMGR_UiState_Context[ulMainActionId];

			if ((pstMgrLiveContext->eLiveMode != eMgrLiveMode_MainLive)
				|| (pstMgrLiveContext->hService == HANDLE_NULL))
			{
				HxLOG_Error(" Main live not stated..! can't start FCC !! \n");
				HxLOG_Error(" mainAid(%d) liveMode(%d) hSvc(0x%X) \n", ulMainActionId, pstMgrLiveContext->eLiveMode, pstMgrLiveContext->hService);
				return ERR_FAIL;
			}
		}

		hErr = mgr_live_FindFccActionIdBySvc(pstStartArg->hService, &ulActionId);
		if (hErr == ERR_OK)
		{
			*pulActionId = ulActionId;
			return ERR_OK;
		}

		ulActionId = mgr_live_GetFccActionId();
		break;
#endif

	case eMgrLiveMode_LiveStreaming:
		hErr = MGR_ACTION_GetEmptyBgActionId (&ulActionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("MGR_ACTION_GetEmptyBgActionId err\n");
			return ERR_FAIL;
		}
		break;

	default:
		return ERR_FAIL;
	}

#if !defined(CONFIG_SUPPORT_FCC)	// start를 마치고 나서 설정한다.. 일단 fcc에서 적용해보고 괜찮으면 전체적용 예정
	// reset ui context
	pstContext = &s_astMGR_UiState_Context[ulActionId];
	pstContext->eLiveMode = pstStartArg->eLiveMode;
	pstContext->ulActionId = ulActionId;
	pstContext->hService = pstStartArg->hService;
#endif

	pmgr_live_ResetUiStateView(ulActionId, TRUE);
	pmgr_live_ResetUiStateLock(ulActionId, TRUE);

	/**
	  *		Sw update check - view에서 action setup, take 할 때 반응해야 하니 view보다 먼저 시작해야 한다
	**/
	if (pstStartArg->eLiveMode == eMgrLiveMode_MainLive)
	{
		MGR_LIVEOTA_Start();
	}

	/**
	  *		Complementary - view에서 msg를 날려주면 반응해야 하므로 view보다 먼저 생성하자
	**/

#if defined(CONFIG_SUPPORT_FCC)
	// TODO: pretune이 main이 되면 comp proc이 시작되어야 한당..
	if ((eMgrLiveMode_LiveStreaming != pstStartArg->eLiveMode)
		&& (eMgrLiveMode_FccLive != pstStartArg->eLiveMode)
		&& (eMgrLiveMode_PipLive != pstStartArg->eLiveMode))
#else
	if (eMgrLiveMode_LiveStreaming != pstStartArg->eLiveMode
        && (eMgrLiveMode_PipLive != pstStartArg->eLiveMode))
#endif
	{
		pfnProc = NULL;
		pmgr_live_GetLiveCompProc(ulActionId, &pfnProc);
		if (mgr_live_StartCbFunc(ulActionId, pfnProc, pstStartArg) != ERR_OK)
		{
			HxLOG_Error ("ActionId(%d) mgr_live_StartCbFunc() failed \n", ulActionId);
			return ERR_FAIL;
		}
	}

	/**
	  *		Live view
	**/
	pfnProc = NULL;
	pmgr_live_GetLiveViewProc(ulActionId, &pfnProc);
	if (mgr_live_StartCbFunc(ulActionId, pfnProc, pstStartArg) != ERR_OK)
	{
		HxLOG_Error ("ActionId(%d) mgr_live_StartCbFunc() failed \n", ulActionId);
		MGR_LIVE_Stop(ulActionId);
		return ERR_FAIL;
	}

	/**
	  *		Live lock(parental rating)
	**/
	// 추후 여기가 파생 가능하도록 수정되어야 함!!!!
#if defined(CONFIG_SUPPORT_FCC)
	if (eMgrLiveMode_FccLive != pstStartArg->eLiveMode)
#endif
	{
		pfnProc = NULL;
		pmgr_live_GetLiveLockProc (ulActionId, &pfnProc);
		if (mgr_live_StartCbFunc(ulActionId, pfnProc, pstStartArg) != ERR_OK)
		{
			HxLOG_Error ("ActionId(%d) mgr_live_StartCbFunc() failed \n", ulActionId);
			MGR_LIVE_Stop(ulActionId);
			return ERR_FAIL;
		}
	}

#if defined(CONFIG_SUPPORT_FCC)
	// reset ui context
	pstContext = &s_astMGR_UiState_Context[ulActionId];
	pstContext->eLiveMode = pstStartArg->eLiveMode;
	pstContext->ulActionId = ulActionId;
	pstContext->hService = pstStartArg->hService;
#endif


	// Inform the live is started...
#if defined(CONFIG_SUPPORT_FCC)
	if (pstStartArg->eLiveMode == eMgrLiveMode_MainLive)
#endif
	{
		BUS_SendMessage (NULL, eMEVT_LIVE_NOTIFY_SVC_STARTED, SVC_PIPE_GetActionHandle (ulActionId), (HINT32)pstStartArg->hService, 0, 0);
	}

	*pulActionId = ulActionId;

	return ERR_OK;
}

HERROR MGR_LIVE_Stop (HUINT32 ulActionId)
{
	HERROR						hErr;
#if defined(CONFIG_SUPPORT_FCC)
	MgrLive_LiveMode_e	 		eLiveMode;

	eLiveMode = s_astMGR_UiState_Context[ulActionId].eLiveMode;
#endif

	hErr = mgr_live_StopLive(ulActionId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("mgr_live_StopLive() failed..!! \n", ulActionId);
	}

#if defined(CONFIG_SUPPORT_FCC)
	if (eLiveMode == eMgrLiveMode_MainLive)
#endif
	{
#if defined(CONFIG_SUPPORT_FCC)
		mgr_live_ClearActionIdMapTable();
#endif

		BUS_SendMessage (NULL, eMEVT_LIVE_NOTIFY_SVC_STOPPED, SVC_PIPE_GetActionHandle (ulActionId), 0, 0, 0);
	}

	return ERR_OK;
}

#if defined(CONFIG_SUPPORT_FCC)
HERROR MGR_LIVE_StopFccSvc(HUINT32 ulFccActionId)
{
	if (s_astMGR_UiState_Context[ulFccActionId].eLiveMode == eMgrLiveMode_FccLive)
	{
		return mgr_live_StopLive(ulFccActionId);
	}

	return ERR_FAIL;
}

HERROR MGR_LIVE_StopAllFccSvc(void)
{
	return mgr_live_StopAllFccSvc();
}

#endif

HERROR MGR_LIVE_GetLiveMode(HUINT32 ulActionId, MgrLive_LiveMode_e *peLiveMode)
{
	MgrLive_UiState_Context_t	*pstContext = NULL;

	if (NULL == peLiveMode)					{ return ERR_FAIL; }

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("invalid actionId(%d)  \n", ulActionId);
		return ERR_FAIL;
	}

	pstContext = &s_astMGR_UiState_Context[ulActionId];
	if (pstContext == NULL)
	{

		HxLOG_Error ("invalid context..  ulActionId(%d) \n", ulActionId);
		return ERR_FAIL;
	}

	*peLiveMode = pstContext->eLiveMode;
	return ERR_OK;
}

HERROR MGR_LIVE_SetLiveMode(HUINT32 ulActionId, MgrLive_LiveMode_e eNewLiveMode)
{
	MgrLive_UiState_Context_t	*pstContext = NULL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("invalid actionId(%d)  \n", ulActionId);
		return ERR_FAIL;
	}

	pstContext = &s_astMGR_UiState_Context[ulActionId];
	if (pstContext == NULL)
	{

		HxLOG_Error ("invalid context..  ulActionId(%d) \n", ulActionId);
		return ERR_FAIL;
	}

	pstContext->eLiveMode = eNewLiveMode;
	return ERR_OK;
}

HERROR MGR_LIVE_GetState(HUINT32 ulActionId, MgrLive_ProcType_e eProcType, HUINT32 *pulUiState)
{
	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error (" invalid ulActionId(%d)  ! \n", ulActionId);
		return ERR_FAIL;
	}

	if (pulUiState == NULL)
	{
		HxLOG_Error ("ulActionId(%d) invalid parameter ! \n", ulActionId);
		return ERR_FAIL;
	}

	if (eProcType >= eLiveProc_MAX)
	{
		HxLOG_Error ("ulActionId(%d) invalid eProcType(%d) \n", ulActionId, eProcType);
		return ERR_FAIL;
	}

	return pmgr_live_GetState(ulActionId, eProcType, pulUiState);
}

HERROR MGR_LIVE_SetMainMedia(HUINT32 ulNewMainActId, HUINT32 ulCurrMainActId)
{
	HUINT32 					ulMainActionId;
	Handle_t					hSvc = HANDLE_NULL;

	ulMainActionId = MGR_ACTION_GetMainActionId();
	if ((ulMainActionId == ulNewMainActId)
		|| (ulNewMainActId == ulCurrMainActId))
	{
		// already main..
		HxLOG_Error ("actionId(%d) is already main view.. \n", ulMainActionId);
		return ERR_FAIL;
	}

	// action manager에 새로운 main action id를 설정해 주고
	MGR_ACTION_SetMainActionId(ulNewMainActId);

	// context의 liveMode도 변경해 주고
	BUS_SendMessage (NULL, eMEVT_LIVE_SET_MAINVIEW_ACTION, HANDLE_NULL, ulNewMainActId, ulCurrMainActId, 0);

	// swap을 하자
	SVC_AV_SetMainMedia(ulNewMainActId);

	// swap후엔 update last svc
	if (MGR_LIVE_GetServiceHandle(ulNewMainActId, eLiveViewPlaySvc_MASTER, &hSvc) != ERR_FAIL)
	{
		HxLOG_Print(" [%s,%d]  MainActionId(%d) lastSvcHandle(0x%X) \n", __FUNCTION__, __LINE__, ulNewMainActId, hSvc);
		MGR_LIVE_CurrentSvcInfoSet(ulNewMainActId, hSvc);
		MGR_LIVE_RegisterLastSvcInfo(hSvc);
	}

	// main / sub의 video 위치 조정은 별도로 내려온다.. done..
	HxLOG_Print(" [%s,%d]  >> Main(%d) Sub(%d) --> Main(%d) Sub(%d) \n\n", __FUNCTION__, __LINE__, ulCurrMainActId, ulNewMainActId, MGR_ACTION_GetMainActionId(), MGR_ACTION_GetSubActionId());

	return ERR_OK;
}

HERROR MGR_LIVE_SetVideoHideByAppkit(HUINT32 ulActionId, HBOOL bMuteFlag)
{
	HxLOG_Debug("bMuteFlag(%d) \n", bMuteFlag);
	return SVC_AV_SetVideoHide(ulActionId, eAvMuteFlag_Appkit, bMuteFlag);
}

HERROR MGR_LIVE_UnselectVideo(HUINT32 ulActionId)
{
	BUS_SendMessage (NULL, eMEVT_LIVE_SET_UNSELECT_COMPONENT,
						SVC_PIPE_GetActionHandle(ulActionId),
						(HINT32)TRUE, 	//<< unselect video
						0, 				//<< unselect audio
						0);
	return ERR_OK;
}

HERROR MGR_LIVE_UnselectAudio(HUINT32 ulActionId)
{
	BUS_SendMessage (NULL, eMEVT_LIVE_SET_UNSELECT_COMPONENT,
						SVC_PIPE_GetActionHandle(ulActionId),
						0, 				//<< unselect video
						(HINT32)TRUE, 	//<< unselect audio
						0);
	return ERR_OK;
}

#if 0
HERROR MGR_LIVE_SetAudioMuteAll(HUINT32 ulActionId, HBOOL bMuteFlag)
{
	return SVC_AV_SetAudioHide(ulActionId, eAvMuteFlag_All, bMuteFlag);
}

HERROR MGR_LIVE_SetVideoMuteAll(HUINT32 ulActionId, HBOOL bMuteFlag)
{
	return SVC_AV_SetVideoHide(ulActionId, eAvMuteFlag_All, bMuteFlag);
}

HERROR MGR_LIVE_SetAvMuteAll(HUINT32 ulActionId, HBOOL bMuteFlag)
{
	HERROR hErr = ERR_FAIL;

	hErr = MGR_LIVE_SetAudioMuteAll(ulActionId, bMuteFlag);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("MGR_SetAudioMute() failed.. err(%d) \n", hErr);
	}

	hErr = MGR_LIVE_SetVideoMuteAll(ulActionId, bMuteFlag);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("MGR_SetVideoMute() failed.. err(%d) \n", hErr);
	}

	return hErr;
}
#endif

HERROR MGR_LIVE_GetServiceHandle(HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, Handle_t *phSvc)
{
	return pmgr_live_GetServiceHandle(ulActionId, eSvcType, phSvc);
}

HERROR MGR_LIVE_GetSessionType(HUINT32 ulActionId, MgrLiveView_SessionType_t *pSessionType)
{
	return pmgr_live_GetSessionType(ulActionId, pSessionType);
}

HERROR MGR_LIVE_GetSvcInfo (HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, DbSvc_Info_t *pMasterSvcInfo)
{
	return pmgr_live_GetSvcInfo(ulActionId, eSvcType, pMasterSvcInfo);
}

HERROR MGR_LIVE_GetTsInfo(HUINT32 ulActionId, DbSvc_TsInfo_t *pTsInfo)
{
	return pmgr_live_GetTsInfo(ulActionId, pTsInfo);
}

HERROR MGR_LIVE_GetRating (HUINT32 ulActionId, DxRatingAge_e *peRatingValue)
{
	HERROR 			hErr;
	HBOOL 			bRatingValid = FALSE;
	DxRatingAge_e 	eRatingValue = eDxRATING_AGE_LOCK_ALL;

	if(ulActionId >= NUM_VIEW_ACTION)
	{
		return ERR_FAIL;
	}

	hErr = pmgr_live_GetRating(ulActionId, &bRatingValid, peRatingValue);
	if ((hErr == ERR_OK) && (bRatingValid == TRUE))
	{
		*peRatingValue = eRatingValue;
		hErr = ERR_OK;
	}
	else
	{
		*peRatingValue = eDxRATING_VIEW_ALL;
		hErr = ERR_FAIL;
	}

	return hErr;
}

HERROR MGR_LIVE_SetRatingPassedFlag(HUINT32 ulActionId, HBOOL bPassedFlag)
{
	if(ulActionId >= NUM_VIEW_ACTION)
	{
		return ERR_FAIL;
	}

	BUS_SendMessage(NULL, eMEVT_LIVE_SET_RATING_PASSED_FLAG, SVC_PIPE_GetActionHandle(ulActionId), bPassedFlag, 0, 0);

	return ERR_OK;
}

HERROR MGR_LIVE_SetAudioIndex(HUINT32 ulActionId, HUINT32 ulIndex)
{
	return pmgr_live_SetAudioIndex(ulActionId, ulIndex);
}

HERROR MGR_LIVE_SetDualMonoConfig(HUINT32 ulActionId, DxDualMonoSelect_e eDualMonoConfig)
{
	return pmgr_live_SetDualMonoConfig(ulActionId, eDualMonoConfig);
}

HERROR MGR_LIVE_GetRingBufHandle (HUINT32 ulActionId, Handle_t *phRingBuf)
{
	return pmgr_live_GetRingBufHandle (ulActionId, phRingBuf);
}

HERROR	MGR_LIVE_GetProfileFromJSON(const HCHAR *pProfileString, MgrLive_LiveStreamArg_t *pstDestArg)
{
	return pmgr_live_GetProfileFromJSON (pProfileString, pstDestArg);
}

HERROR MGR_LIVE_ResetUiStateView (HUINT32 ulActionId, HBOOL bSvcStart)
{
	return pmgr_live_ResetUiStateView (ulActionId, bSvcStart);
}

HERROR MGR_LIVE_ResetUiStateLock (HUINT32 ulActionId, HBOOL bSvcStart)
{
	return pmgr_live_ResetUiStateLock (ulActionId, bSvcStart);
}

HERROR MGR_LIVE_CheckFccLiveContext (void)
{
#if defined(CONFIG_SUPPORT_FCC)
	HUINT32		ulActionId;

	// invalid한 상태의 fcc context가 있다면 정리한다
	for (ulActionId = eActionId_VIEW_FIRST; ulActionId <= eActionId_VIEW_LAST; ulActionId++)
	{
		// live mode가 fcc인데 action type이 fcc가 아니면 clear..!
		if ((s_astMGR_UiState_Context[ulActionId].eLiveMode == eMgrLiveMode_FccLive)
			&& (MGR_ACTION_GetType(ulActionId) != eViewType_FCC))
		{
			HxLOG_Print("Invalid FCC context status.. clear NOW ! aid(%d) actionType(%s)", ulActionId, MGR_ACTION_GetTypeStr(MGR_ACTION_GetType(ulActionId)));

			s_astMGR_UiState_Context[ulActionId].eLiveMode 	= eMgrLiveMode_None;
			s_astMGR_UiState_Context[ulActionId].hService 	= HANDLE_NULL;
		}
	}
#endif

	return ERR_OK;
}


#if 0
HERROR MGR_LIVE_GetUiStateView (HUINT32 ulActionId, MgrLiveView_UiState_e *peState)
{
	if (NULL == peState)
		return ERR_FAIL;

	return pmgr_live_GetUiStateView (ulActionId, peState);
}

HERROR MGR_LIVE_GetUiStateLock (HUINT32 ulActionId, MgrLiveLock_UiState_e *peState)
{
	if (NULL == peState)
		return ERR_FAIL;

	return pmgr_live_GetUiStateLock (ulActionId, peState);
}
#endif

HERROR MGR_LIVE_GetLiveSiSpec (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec)
{
	return MGR_MODEL_LiveGetSiSpec (pstStart, pstSiSpec);
}

HERROR MGR_LIVE_GetSatRecSiSpec (SvcSi_SatRecordSpec_t *pstSiSpec)
{
	return MGR_MODEL_SatrecGetSiSpec (pstSiSpec);
}

HUINT8* MGR_LIVE_GetLiveModeStr(MgrLive_LiveMode_e eLiveMode)
{
#if defined(CONFIG_DEBUG)
	switch(eLiveMode)
	{
		ENUM_TO_STR(eMgrLiveMode_None)
		ENUM_TO_STR(eMgrLiveMode_MainLive)
		ENUM_TO_STR(eMgrLiveMode_PipLive)
		ENUM_TO_STR(eMgrLiveMode_LiveStreaming)
		ENUM_TO_STR(eMgrLiveMode_FccLive)
		ENUM_TO_STR(eMgrLiveMode_EndOfCase)
		default:			break;
	}

	HxLOG_Print("Unkown live mode [%d]\n", eLiveMode);
#endif
	return "UNKNOWN";
}


void MGR_LIVE_PrintContext(void)
{
#if defined(CONFIG_DEBUG)
	MgrLive_UiState_Context_t	*pstContext = NULL;
	HUINT32 					 i;
	DbSvc_Info_t				 stSvcInfo;

	HxLOG_Error("\n\n");
	HxLOG_Error("=========================================\n");
	HxLOG_Error("		   MGR_LIVE_PrintContext \n");
	HxLOG_Error("=========================================\n");
	HxLOG_Error("	 Current Main aciton id (%d) eActionId_VIEW_LAST(%d) \n", MGR_ACTION_GetMainActionId(), eActionId_VIEW_LAST);
	HxLOG_Error("-------------------------------------\n");

	for (i=eActionId_VIEW_FIRST; i<= eActionId_VIEW_LAST; i++)
	{
		HxLOG_Error(" idx(%d) \n", i);

		pstContext = &s_astMGR_UiState_Context[i];
		if (pstContext != NULL)
		{
			HUINT32 	ulDmxId = 0xff;
#if defined(CONFIG_SUPPORT_FCC)
			HUINT32 	ulViewId = 0xff;	// action view id가 아님.. oapi이상에서 사용하는 view id

			if (MGR_ACTION_GetViewIdByActionId(i, &ulViewId) != ERR_FAIL)
			{
				HxLOG_Error(" OAPI ViewId(%d) \n", ulViewId);
			}
			else
			{
				HxLOG_Error(" MGR_ACTION_GetViewIdByActionId() error ! aid(%d) \n", i);
			}
#endif

			if (pstContext->eLiveMode == eMgrLiveMode_None)
			{
				HxLOG_Error(" eLiveMode(%s) \n", MGR_LIVE_GetLiveModeStr(pstContext->eLiveMode));
				HxLOG_Error("-------------------------------------\n");
				continue;
			}

			if (SVC_PIPE_GetResourceId(i, eRxRSCTYPE_DEMUX, &ulDmxId) != ERR_OK)
			{
				HxLOG_Error(" SVC_PIPE_GetResourceId() error ! aid(%d) \n", i);
			}

			HxLOG_Error(" eLiveMode(%s) dmxId(%d) \n", MGR_LIVE_GetLiveModeStr(pstContext->eLiveMode), ulDmxId);
			HxLOG_Error(" ulActionId(%d) actionType(%s) actionPriority(0x%x) \n", pstContext->ulActionId, MGR_ACTION_GetTypeStr(MGR_ACTION_GetType(pstContext->ulActionId)), MGR_ACTION_GetPriority(pstContext->ulActionId));

			if (DB_SVC_GetServiceInfo(pstContext->hService, &stSvcInfo) != ERR_OK)
			{
				HxLOG_Error(" hService(0x%X) \n", pstContext->hService);
			}
			else
			{
				HxLOG_Error(" svcName(%s) lcn(%d) hService(0x%X) \n", DbSvc_GetName(&stSvcInfo), DbSvc_GetLcn(&stSvcInfo), pstContext->hService);
			}

			// subtitle
			if (i == MGR_ACTION_GetMainActionId())
			{
				SvcSi_SubtitleList_t stSubtitleList;

				HxSTD_MemSet(&stSubtitleList, 0x00, sizeof(SvcSi_SubtitleList_t));

				if (SVC_SI_GetSubtitleList(i, DbSvc_GetSvcId(&stSvcInfo), &stSubtitleList) != ERR_OK)
				{
					HxLOG_Error(" ==> SVC_SI_GetSubtitleList() error!  aid(%d) svcId(0x%X) \n", i, DbSvc_GetSvcId(&stSvcInfo));
				}
				else
				{
					HxLOG_Error(" ==> aid(%d) svcId(0x%X) ulSubtitleNum(%d) \n", i, DbSvc_GetSvcId(&stSvcInfo), stSubtitleList.ulSubtitleNum);
				}
			}
		}

		HxLOG_Error("-------------------------------------\n");
	}

	HxLOG_Error("=========================================\n\n\n");

#endif

	return;
}

#if defined(SvcSi_SUPPORT_LIB_ES_NEW)

STATIC HBOOL mgr_live_CompareSvc(const void *pvInfo1, const void *pvInfo2, HUINT32 ulSize)
{
	if (NULL == pvInfo1 || NULL == pvInfo2)
		return FALSE;

	return (HxSTD_MemCmp (pvInfo1, pvInfo2, sizeof(DbSvc_Info_t)) == 0) ? FALSE : TRUE;
}

STATIC HERROR mgr_live_UpdateSvc(const SvcSi_PmtEsInfo_t *pstPmtEsInfo, void *pvSvcInfo)
{
	DbSvc_Info_t	*pstSvcInfo = (DbSvc_Info_t *)pvSvcInfo;

	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	// PCR
	DbSvc_SetPcrPid(pstSvcInfo, pstPmtEsInfo->usPcrPid);

	// CAS type - PID는 아니지만 이것도 넣어줘야 cas type이 변경됐을 경우 처리할 수 있다
	DbSvc_SetCasType(pstSvcInfo, (pstPmtEsInfo->ulCasType > eDxCAS_TYPE_FTA) ? (DxCasType_e)pstPmtEsInfo->ulCasType : eDxCAS_TYPE_FTA);

	return ERR_OK;
}

STATIC HERROR mgr_live_UpdateVideoEs(Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	DbSvc_Info_t	*pstSvcInfo = NULL;

	pstSvcInfo = (DbSvc_Info_t *)pvSvcInfo;

	DbSvc_SetVideoPid(pstSvcInfo, PID_NULL);
	DbSvc_SetVideoCodec(pstSvcInfo, eDxVIDEO_CODEC_UNKNOWN);

	if (TRUE == pstSelected->bFound)
	{
		HERROR			hErr;
		HUINT16			usPid = PID_NULL;
		DxVideoCodec_e	eCodec = eDxVIDEO_CODEC_UNKNOWN;
//		HUINT8			ucComponentTag;

		hErr = SVC_SI_GetEsInfo(eES_UPDATE_Video, hEsInfoList, pstSelected->nIndexMain, &usPid, &eCodec, NULL, NULL);
		if (ERR_OK == hErr)
		{
			DbSvc_SetVideoPid(pstSvcInfo, usPid);
			DbSvc_SetVideoCodec(pstSvcInfo, eCodec);
		}
	}

	return ERR_OK;
}

STATIC void  mgr_live_ResetAudioInfoFromSvc(DbSvc_Info_t *pstSvcInfo)
{
	DbSvc_SetDolbyFlag(pstSvcInfo, FALSE);
	DbSvc_SetAdFlag(pstSvcInfo, FALSE);

	//DbSvc_SetAudUserSelected(pstSvcInfo, FALSE);
	//DbSvc_SetAudioLanguage(pstSvcInfo, "");

	DbSvc_SetAudioPid(pstSvcInfo, PID_NULL);
	DbSvc_SetAudioAuxPid(pstSvcInfo, PID_NULL);
	DbSvc_SetDolbyPid(pstSvcInfo, PID_NULL);
	DbSvc_SetAudioCodec(pstSvcInfo, eDxAUDIO_CODEC_UNKNOWN);
	DbSvc_SetAudioAuxCodec(pstSvcInfo, eDxAUDIO_CODEC_UNKNOWN);
	DbSvc_SetDolbyCodec(pstSvcInfo, eDxAUDIO_CODEC_UNKNOWN);
}

STATIC HERROR mgr_live_UpdateAudioEs(Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	DbSvc_Info_t	*pstSvcInfo = NULL;

	pstSvcInfo = (DbSvc_Info_t *)pvSvcInfo;

	mgr_live_ResetAudioInfoFromSvc(pstSvcInfo);

	if (TRUE == pstSelected->bFound)
	{
		HERROR			hErr;
		HUINT16 		usPid = PID_NULL;
		DxAudioCodec_e	eCodec = eDxAUDIO_CODEC_UNKNOWN;
//		HUINT8			ucComponentTag;

		// Main
		if (pstSelected->nIndexMain >= 0)
		{
			usPid = PID_NULL;
			eCodec = eDxAUDIO_CODEC_UNKNOWN;

			hErr = SVC_SI_GetEsInfo(eES_UPDATE_Audio, hEsInfoList, pstSelected->nIndexMain, &usPid, &eCodec, NULL, NULL);
			if(hErr == ERR_OK)
			{
				DbSvc_SetAudioPid(pstSvcInfo, usPid);
				DbSvc_SetAudioCodec(pstSvcInfo, eCodec);
			}

			DbSvc_SetDualmonoSelect(pstSvcInfo, (DxDualMonoSelect_e)pstSelected->ulParam1);
			DbSvc_SetDolbyFlag(pstSvcInfo, SvcSi_ES_AUDIO_IS_DOLBY_TYPE((DxAudioCodec_e)pstSelected->ulCodec));

			HxLOG_Debug("nIndexMain(%d) usPid(0x%x)\n", pstSelected->nIndexMain, usPid);
		}

		// AD
		if (pstSelected->nIndexAux >= 0)
		{
			usPid = PID_NULL;
			eCodec = eDxAUDIO_CODEC_UNKNOWN;

			hErr = SVC_SI_GetEsInfo(eES_UPDATE_Audio, hEsInfoList, pstSelected->nIndexAux, &usPid, &eCodec, NULL, NULL);
			if (ERR_OK == hErr)
			{
				DbSvc_SetAudioAuxPid(pstSvcInfo, usPid);
				DbSvc_SetAudioAuxCodec(pstSvcInfo, eCodec);

				DbSvc_SetAdFlag(pstSvcInfo, TRUE);
			}
			HxLOG_Debug("nIndexAux(%d) usPid(0x%x)\n", pstSelected->nIndexAux, usPid);
		}
	}

	return ERR_OK;
}

STATIC HERROR mgr_live_UpdateSubtitleEs(Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	DbSvc_Info_t	*pstSvcInfo = NULL;

	pstSvcInfo = (DbSvc_Info_t *)pvSvcInfo;

	if (TRUE == pstSelected->bFound)
	{
		if (pstSelected->nIndexMain >= 0)
		{
			DbSvc_SetSubttlIdx(pstSvcInfo, pstSelected->nIndexMain);

			HxLOG_Debug("nIndexMain(%d)\n", pstSelected->nIndexMain);
		}
	}

	return ERR_OK;
}

STATIC HERROR mgr_live_UpdateTeletextEs(Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	DbSvc_Info_t	*pstSvcInfo = NULL;

	pstSvcInfo = (DbSvc_Info_t *)pvSvcInfo;

	DbSvc_SetTtxPid(pstSvcInfo, PID_NULL);

	if (TRUE == pstSelected->bFound)
	{
		if (pstSelected->nIndexMain >= 0)
		{
			HERROR			hErr;
			HUINT16 		usPid = PID_NULL;

			hErr = SVC_SI_GetEsInfo(eES_UPDATE_Teletext, hEsInfoList, pstSelected->nIndexMain, &usPid, NULL, NULL, NULL);
			if (ERR_OK == hErr)
			{
				DbSvc_SetTtxPid(pstSvcInfo, usPid);
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR mgr_live_UpdateEs(SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	HERROR	hErr = ERR_FAIL;

	if (HANDLE_NULL == hEsInfoList)
		return ERR_FAIL;

	if (NULL == pvSvcInfo)
		return ERR_FAIL;

	if (NULL == pstSelected)
		return ERR_FAIL;

	switch (eType)
	{
	case eES_UPDATE_Video:			hErr = mgr_live_UpdateVideoEs(hEsInfoList, pvSvcInfo, pstSelected);			break;
	case eES_UPDATE_Audio:			hErr = mgr_live_UpdateAudioEs(hEsInfoList, pvSvcInfo, pstSelected);			break;
	case eES_UPDATE_Subtitle:		hErr = mgr_live_UpdateSubtitleEs(hEsInfoList, pvSvcInfo, pstSelected);		break;
	case eES_UPDATE_Teletext:		hErr = mgr_live_UpdateTeletextEs(hEsInfoList, pvSvcInfo, pstSelected);		break;
	default:
		break;
	}

	return hErr;
}

HERROR MGR_LIVE_UpdateEs(SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	if (HANDLE_NULL == hEsInfoList)
		return ERR_FAIL;

	if (NULL == pvSvcInfo)
		return ERR_FAIL;

	if (NULL == pstSelected)
		return ERR_FAIL;

	return mgr_live_UpdateEs(eType, hEsInfoList, pvSvcInfo, pstSelected);
}

HBOOL MGR_LIVE_CompareSvc(const void *pvInfo1, const void *pvInfo2, HUINT32 ulSize)
{
	if (NULL == pvInfo1 || NULL == pvInfo2)
		return FALSE;

	if (0 == ulSize)
		return FALSE;

	return mgr_live_CompareSvc(pvInfo1, pvInfo2, ulSize);
}

HERROR MGR_LIVE_UpdateSvc(const SvcSi_PmtEsInfo_t *pstPmtEsInfo, void *pvSvcInfo)
{
	if (NULL == pvSvcInfo)
		return ERR_FAIL;

	return mgr_live_UpdateSvc(pstPmtEsInfo, pvSvcInfo);
}

STATIC HERROR mgr_live_GetSiUserSettingsFromDbSvcInfo(const DbSvc_Info_t *pstSvcInfo, SvcSi_UserSettings_t *pstUserSettings)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	if (NULL == pstUserSettings)
		return ERR_FAIL;

	HxSTD_MemSet(pstUserSettings, 0x00, sizeof(SvcSi_UserSettings_t));

	// Audio Settings : User Value
	pstUserSettings->stAudio.bUserSelected		= DbSvc_GetAudUserSelected(pstSvcInfo);
	pstUserSettings->stAudio.eLang				= HLIB_LANG_639ToId((const HCHAR *)DbSvc_GetAudioLanguage(pstSvcInfo));
	pstUserSettings->stAudio.nPid				= DbSvc_GetAudioPid(pstSvcInfo);
	pstUserSettings->stAudio.bDolby				= DbSvc_GetDolbyFlag(pstSvcInfo);
	pstUserSettings->stAudio.bAudioDescription	= DbSvc_GetAdFlag(pstSvcInfo);

	// Subtitle Settings
	pstUserSettings->stSubtitle.bUserSelected	= DbSvc_GetSubttlFlag(pstSvcInfo);
	pstUserSettings->stSubtitle.eLang			= HLIB_LANG_639ToId((const HCHAR *)DbSvc_GetSubttlLanguage(pstSvcInfo));
	pstUserSettings->stSubtitle.nTrack			= DbSvc_GetSubttlIdx(pstSvcInfo);
	pstUserSettings->stSubtitle.bHoh			= DbSvc_GetHohFlag(pstSvcInfo);
	pstUserSettings->stSubtitle.bDvb			= FALSE;
	pstUserSettings->stSubtitle.bHd				= FALSE;

	/* subtitle OFF track :
	 *  - flag is TRUE
	 *  - index is 255 (0xff)
	 */
	if ((pstUserSettings->stSubtitle.bUserSelected == TRUE) && (pstUserSettings->stSubtitle.nTrack == SUBTITLE_OFF_IDX))
	{
		pstUserSettings->stSubtitle.bUserSelectedOff = TRUE;
	}

	return ERR_OK;
}

STATIC HERROR mgr_live_GetSiUpdate(SvcSi_UpdateEs_t *pstUpdate, DbSvc_Info_t *pstOrgInfo, DbSvc_Info_t *pstDestInfo)
{
	HxSTD_MemSet(pstUpdate, 0x00, sizeof(SvcSi_UpdateEs_t));

	pstUpdate->pvOrgInfo	= pstOrgInfo;
	pstUpdate->pvDstInfo	= pstDestInfo;
	pstUpdate->ulSize		= sizeof(DbSvc_Info_t);

	(void)mgr_live_GetSiUserSettingsFromDbSvcInfo((const DbSvc_Info_t *)pstOrgInfo, &pstUpdate->stUserSettings);

	pstUpdate->fnCompareSvc	= MGR_LIVE_CompareSvc;
	pstUpdate->fnUpdateSvc	= MGR_LIVE_UpdateSvc;
	pstUpdate->fnUpdateEs	= MGR_LIVE_UpdateEs;

	return ERR_OK;
}

HERROR MGR_LIVE_GetSiUpdate(SvcSi_UpdateEs_t *pstUpdate, DbSvc_Info_t *pstOrgInfo, DbSvc_Info_t *pstDestInfo)
{
	if (NULL == pstUpdate)
		return ERR_FAIL;

	return mgr_live_GetSiUpdate(pstUpdate, pstOrgInfo, pstDestInfo);
}
#endif

HERROR	MGR_LIVE_UpdateServiceInfo (Handle_t hAction, HUINT32 ulSvcId, DbSvc_Info_t *pstSrcSvcInfo, DbSvc_Info_t *pstDstSvcInfo, HBOOL *pbChanged)
{
	HERROR	hErr;

	if (NULL == pstSrcSvcInfo)
		return ERR_FAIL;

	if (NULL == pstDstSvcInfo)
		return ERR_FAIL;

#if defined(SvcSi_SUPPORT_LIB_ES_NEW)
	{
		SvcSi_UpdateEs_t stUpdate;

		hErr = MGR_LIVE_GetSiUpdate(&stUpdate, pstSrcSvcInfo, pstDstSvcInfo);


		if (ERR_OK == hErr)
		{
			hErr = SVC_SI_UpdateServiceInfo (hAction, ulSvcId, &stUpdate, pbChanged);
		}
	}
#else
	hErr = SVC_SI_GetLiveServiceInfoPid (hAction, ulSvcId, pstSrcSvcInfo, pstDstSvcInfo, pbChanged);
#endif

	return hErr;
}


/*********************** End of File ******************************/

