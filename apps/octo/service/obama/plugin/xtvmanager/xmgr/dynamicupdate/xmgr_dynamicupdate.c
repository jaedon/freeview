/**
	@file     mgr_dynamicupdate.c
	@brief    file_name & simple comment.

	Description: \n
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

#include <svc_pipe.h>

#include <svc_si.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_rscmap.h>

#include <mgr_dynamicupdate.h>

#include <otl.h>

#include "./local_include/_xmgr_dynamicupdate.h"
#include <_mgr_dynamicupdate.h>
#include <otl.h>

/*******************************************************************/
/********************      Defines         *************************/
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

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

HERROR xmgr_dynamicupdate_setupBackGroundAction (DynamicUpdate_Content_t *pstContents)
{
	HUINT32				ulMainActionId = 0;
	Handle_t 			hMainLiveSvc = HANDLE_NULL;
	DbSvc_TsInfo_t		stMainLiveTsInfo = {0, };
//	MgrAction_Type_e		eActionType = eActionType_NONE;
	HERROR				hErr = ERR_FAIL;

	HxLOG_Trace();

	if(pstContents->bManageBgAction == TRUE)
	{
		HxLOG_Debug("bManageBgAction is %d, already manage the background action. \n", pstContents->bManageBgAction);
		return ERR_FAIL;
	}

	// check main action.
	ulMainActionId = MGR_ACTION_GetMainActionId();
	// 이미 action type이 wtv임을 check 했음.
	// action taken이 아니라 sucess 에서 처리한 이유는 wtv의 경우, action setup take release 의 일반적인 경우가 아니라, setup setup setup 이 일어날 수 있다.
#if 0
	eActionType = MGR_ACTION_GetType(ulMainActionId);
	if(eActionType != eViewType_WTV)
	{
		HxLOG_Debug("cannot setup bg action because current ulMainActionId: %d, actionType is %d \n", ulMainActionId, eActionType);
		return ERR_FAIL;
	}
#endif
	// get bg action.
	hErr = MGR_ACTION_GetEmptyBgActionId (&(pstContents->ulBGActionId));
	if (ERR_OK != hErr)
	{
		HxLOG_Error("cannot get backgroud action!\n");
		return ERR_FAIL;
	}
	pstContents->hBGAction  = SVC_PIPE_GetActionHandle(pstContents->ulBGActionId);

	HxLOG_Debug("pstContents->hBGAction: 0x%x, pstContents->ulBGActionId: 0x%x \n", pstContents->hBGAction, pstContents->ulBGActionId);

	// setup bg tune using live tuning param.
	MGR_LIVE_GetServiceHandle(ulMainActionId, eLiveViewPlaySvc_ACTUAL, &hMainLiveSvc);
	hErr = DB_SVC_GetTsInfoByServiceHandle (hMainLiveSvc, &stMainLiveTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("cannot get Ts Info from live \n");
		return ERR_FAIL;
	}

	(void)OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stMainLiveTsInfo, &(pstContents->stBgTuneInfo));
	pstContents->usUniqueId = stMainLiveTsInfo.usUniqueId;

	hErr = MGR_ACTION_SetupBgTune(pstContents->ulBGActionId, &(pstContents->stBgTuneInfo), eAmPriority_BGJOB);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("background tune setup fail!\n");
		return ERR_FAIL;
	}

	// take action.
	hErr = MGR_ACTION_Take(pstContents->ulBGActionId, eActionType_BG_TUNE, BUS_MGR_GetMgrCallback (BUS_MGR_GetThis()));
	if(hErr != ERR_OK)
	{
		HxLOG_Error("take action fail!!\n");
		return ERR_FAIL;
	}

	pstContents->bManageBgAction = TRUE;

	HxLOG_Debug("(%s) return ERR_OK! \n", __FUNCTION__);
	return ERR_OK;
}

HERROR xmgr_dynamicupdate_releaseBackGroundAction (DynamicUpdate_Content_t *pstContents)
{
	HERROR		hErr = ERR_FAIL;
	HERROR		hReturnErr = ERR_OK;

	if(pstContents->bManageBgAction == FALSE)
	{
		HxLOG_Debug("bManageBgAction is FALSE \n");
		return ERR_FAIL;
	}

	// stop si dynamic update
	hErr = xmgr_dynamicupdate_stopSiDynamicUpdate(pstContents);
//	hErr = SVC_SI_StopDynamicUpdate(pstContents->hBGAction, FALSE);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("StopTune fail!!\n");
	}

	// stop tune
	if (pstContents->bStartTune == TRUE)
	{
		hErr = SVC_CH_StopTune(pstContents->hBGAction, -1, eSyncMode);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("StopTune fail!!\n");
		}
		else
		{
			pstContents->bStartTune = FALSE;
		}
	}
	hReturnErr |= hErr;

	// release action
	hErr = MGR_ACTION_Release(pstContents->ulBGActionId, eActionType_BG_TUNE);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("release action fail!!\n");
	}
	hReturnErr |= hErr;

	if(hReturnErr == ERR_OK)
	{
		pstContents->bManageBgAction = FALSE;
	}

	HxLOG_Debug("(%s) return %d \n", __FUNCTION__, hReturnErr);
	return hReturnErr;
}

STATIC HERROR xmgr_dynamicupdate_startTune (DynamicUpdate_Content_t *pstContents)
{
	HERROR			hErr = ERR_FAIL;
	SvcRsc_Info_t		stRscInfo;

	HxLOG_Trace();

	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents == NULL!\n");
		return ERR_FAIL;
	}

	hErr = MGR_RSC_GetResourceInfo (pstContents->ulBGActionId, &stRscInfo);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("MGR_RSC_GetResourceInfo fail!!\n");
		return ERR_FAIL;
	}
#if 0
	hErr = SVC_CH_StopTune(pstContents->hBGAction,
								-1,
								eSyncMode);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("SVC_CH_StopTune fail!!\n");
		return ERR_FAIL;
	}
#endif
	// version up
	pstContents->hBGAction = SVC_PIPE_IncreaseActionVersion(pstContents->ulBGActionId);

	hErr = SVC_CH_StartTune (pstContents->hBGAction, &stRscInfo, pstContents->usUniqueId, &(pstContents->stBgTuneInfo));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_CH_StartTune fail!!\n");
		return ERR_FAIL;
	}

	pstContents->bStartTune = TRUE;

	return ERR_OK;
}


/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

HERROR xmgr_dynamicupdate_updateServiceInfo(Handle_t hSvc, DbSvc_Info_t *pSvcInfoOld, DbSvc_Info_t *pSvcInfoNew)
{
	DbSvc_Info_t		stSvcInfoTmp;

	HxSTD_memcpy(&stSvcInfoTmp, pSvcInfoNew, sizeof(DbSvc_Info_t));

	if (DbSvc_GetRenamed(pSvcInfoOld) == TRUE)
	{
		HCHAR	*pszSvcName = NULL;
		DbSvc_SetName(&stSvcInfoTmp, DbSvc_GetName(pSvcInfoOld));
		pszSvcName = DbSvc_GetName(&stSvcInfoTmp);
		pszSvcName[DbSvc_GetNameLength() - 1] = 0;
	}

	if (DbSvc_GetSvcRemoved(pSvcInfoOld))
	{
		//DB_SVC_SetUserFlag1(&stSvcInfoTmp, eSvcUserFlag_03_SvcDeletedByUser, TRUE);
		DbSvc_SetSvcRemoved(&stSvcInfoTmp, TRUE);
	}

	DB_SVC_UpdateServiceInfo(hSvc, &stSvcInfoTmp);

	return ERR_OK;
}

HBOOL xmgr_dynamicupdate_checkSvcTypeOption (DxSvcType_e eSvcTypeInOption, DxSvcType_e eSvcType)
{
	if (eSvcTypeInOption == eDxSVC_TYPE_All)
	{
		if (eSvcType == eDxSVC_TYPE_TV || eSvcType == eDxSVC_TYPE_RADIO || eSvcType == eDxSVC_TYPE_DATA || eSvcType == eDxSVC_TYPE_HIDDEN)
		{
			return TRUE;
		}
	}
	else if (eSvcTypeInOption == eDxSVC_TYPE_TV || eSvcTypeInOption == eDxSVC_TYPE_RADIO || eSvcTypeInOption == eDxSVC_TYPE_DATA|| eSvcType == eDxSVC_TYPE_HIDDEN )
	{
		if (eSvcType == eSvcTypeInOption)
		{
			return TRUE;
		}
	}

	return FALSE;
}

HERROR xmgr_dynamicupdate_checkActionId (DynamicUpdate_Content_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);

	if (pstContents->ulBGActionId != ulActionId)
	{
		HxLOG_Info("is not matched actionID(0x%x)! \n", ulActionId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xmgr_dynamicupdate_mevtLiveNotifySvcStopped(DynamicUpdate_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId = SVC_PIPE_GetActionId(hAction);
	HERROR		hErr = ERR_FAIL;

	if (MGR_ACTION_GetMainActionId() != ulActionId)
	{
		return ERR_FAIL;
	}

	hErr = xmgr_dynamicupdate_releaseBackGroundAction(pstContents);
	HxLOG_Debug("xmgr_dynamicupdate_releaseBackGroundAction() retrun hErr: %d \n", hErr);

	return hErr;
}

HERROR xmgr_dynamicupdate_mevtLiveNotifySvcChanged(DynamicUpdate_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId = SVC_PIPE_GetActionId(hAction);
	HERROR		hErr = ERR_FAIL;

	if (MGR_ACTION_GetMainActionId() != ulActionId)
	{
		return ERR_FAIL;
	}

	if(pstContents->bOnOff == TRUE)
	{
		// take background action.
		hErr = xmgr_dynamicupdate_setupBackGroundAction(pstContents);
		HxLOG_Debug("xmgr_dynamicupdate_setupBackGroundAction() retrun hErr: %d \n", hErr);

		if(hErr == ERR_FAIL)
		{
			HxLOG_Debug("xmgr_dynamicupdate_setupBackGroundAction() fail. suicide\n");
			BUS_MGR_DestroyWithReason(0, eDESTROY_BUS_ACTION, 0);
		}
		else
		{
			hErr = xmgr_dynamicupdate_startTune(pstContents);
		}
	}
	else
	{
		hErr = ERR_FAIL;
		HxLOG_Debug("pstContents->bOnOff: %d \n", pstContents->bOnOff);
	}

	return hErr;
}

HERROR xmgr_dynamicupdate_evtChLocked (DynamicUpdate_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId = SVC_PIPE_GetActionId (hAction);
	HERROR		hErr = ERR_FAIL;

	HxLOG_Trace();

	if(MGR_ACTION_GetType(ulActionId) != eActionType_BG_TUNE)
	{
		HxLOG_Debug("different action, skip. \n");
		return ERR_FAIL;
	}

	if (pstContents->ulBGActionId != ulActionId)
	{
		HxLOG_Error("ulActionId fail!!\n");
		return ERR_FAIL;
	}

	// start si dynamic update
	HxLOG_Print("call SVC_SI_StartDynamicUpdate\n");
	hErr = SVC_SI_StartDynamicUpdate(pstContents->hBGAction, &(pstContents->stDynamicUpdateSpec), &(pstContents->stDynamicUpdateOption), FALSE);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("cannot start dynamic update ! \n");
		return ERR_FAIL;
	}

	pstContents->bStartedDyUpdate = TRUE;

	return ERR_OK;
}

HERROR xmgr_dynamicupdate_evtSiDynamicUpdateDone (DynamicUpdate_Content_t *pstContents, HBOOL bUpdated)
{
	HxLOG_Trace();

	HxLOG_Debug("bUpdated: %d  \n", bUpdated);

	if (bUpdated == TRUE)
	{
		HxLOG_Debug("Send eSEVT_DB_CHECK_SVC_DB, eCheckDbType_ALL \n");
		// TODO: eSEVT_DB_CHECK_SVC_DB 는 원래 naming 상 source가 SVC 인데...
		BUS_PostMessage (NULL, eSEVT_DB_CHECK_SVC_DB, HANDLE_NULL, eCheckDbType_ALL, 0, 0);
	}
	else
	{
		HxLOG_Debug("SVC_SI_StartDynamicUpdate \n");
	}

	return ERR_OK;
}

HERROR xmgr_dynamicupdate_stopSiDynamicUpdate (DynamicUpdate_Content_t *pstContents)
{
	HxLOG_Trace();

	if (pstContents->bStartedDyUpdate == TRUE)
	{
		(void)SVC_SI_StopDynamicUpdate(pstContents->hBGAction, TRUE);
		pstContents->bStartedDyUpdate = FALSE;
	}

	return ERR_OK;
}

HERROR xmgr_dynamicupdate_checkOnOff (DynamicUpdate_Content_t *pstContents)
{
	HUINT32 ulValue = 0;
	HBOOL	bOnOff = FALSE;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Debug("check eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING \n");
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING, &ulValue, 0);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("cannot get eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING \n");
	}
	else
	{
		bOnOff = (HBOOL)ulValue;
	}

	pstContents->bOnOff = bOnOff;
	HxLOG_Debug("pstContents->bOnOff: %d \n", pstContents->bOnOff);


	return hErr;
}


/*********************** End of File ******************************/

