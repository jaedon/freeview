/**
	@file     mgr_liveota.c
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
#include <xsvc_si.h>
#include <svc_sys.h>
#include <svc_swup.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_rscmap.h>

#include <mgr_liveota.h>
#include <mgr_swup.h>

#include <otl.h>

#include "./local_include/_xmgr_liveota.h"
#include <_mgr_liveota.h>
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
STATIC xmgrLiveOta_Context_t	s_stLiveContent;

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
INLINE STATIC xmgrLiveOta_Context_t *xmgr_liveota_GetContent(void)
{
	return &s_stLiveContent;
}

STATIC HERROR	xmgr_liveota_InitContext(xmgrLiveOta_Context_t *pstContents)
{
	HERROR	hErr = ERR_FAIL;

	HxLOG_Trace();

	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(xmgrLiveOta_Context_t));

	pstContents->stSiLiveSwupSpec.eActionKind = eSVCSI_ACT_KIND_LiveSwupCheck;
	pstContents->stSiLiveSwupSpec.ulActionSpec = eSVCSI_ACT_SPEC_DvbSsu;

	return hErr;
}

STATIC HERROR xmgr_liveota_SetupBackGroundAction (xmgrLiveOta_Context_t *pstContents)
{
	HUINT32						ulMainActionId = 0;
	Handle_t 					hMainLiveSvc = HANDLE_NULL;
	DbSvc_TsInfo_t			 	stMainLiveTsInfo = {0, };
	MgrAction_Type_e			eActionType = eActionType_NONE;
	HERROR						hErr = ERR_FAIL;

	HxLOG_Trace();

	if(pstContents->bManageBgAction == TRUE)
	{
		HxLOG_Debug("bManageBgAction is %d, already manage the background action. \n", pstContents->bManageBgAction);
		return ERR_FAIL;
	}

	// check main action.
	ulMainActionId = MGR_ACTION_GetMainActionId();
	eActionType = MGR_ACTION_GetType(ulMainActionId);
	if(eActionType != eViewType_WTV)
	{
		HxLOG_Debug("cannot setup bg action because current main action is %d \n", eActionType);
		return ERR_FAIL;
	}

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
		HxLOG_Error("background tune setup fail! - ulBGActionId(%d) ulBGActionId(0x%X) \n", pstContents->ulBGActionId, pstContents->hBGAction);
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
	pstContents->stSiLiveSwupOption.hLiveSvcHandle = hMainLiveSvc;

	HxLOG_Debug("return ERR_OK! \n");
	return ERR_OK;


}

STATIC HERROR xmgr_liveota_ReleaseBackGroundAction (xmgrLiveOta_Context_t *pstContents)
{
	HERROR			hErr = ERR_FAIL;
	HERROR			hReturnErr = ERR_OK;

	if(pstContents->bManageBgAction == FALSE)
	{
		HxLOG_Debug("bManageBgAction is FALSE \n");
		return ERR_FAIL;
	}

	// stop si dynamic update
	hErr = SVC_SI_StopLiveSwupCheck(pstContents->hBGAction, FALSE);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("StopTune fail!!\n");
	}

	// stop tune
	hErr = SVC_CH_StopTune(pstContents->hBGAction, -1, eSyncMode);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("StopTune fail!!\n");
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


STATIC HERROR xmgr_liveota_StartTune (xmgrLiveOta_Context_t *pstContents)
{
	HERROR						 hErr = ERR_FAIL;
	SvcRsc_Info_t					 stRscInfo;

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

	// version up
	pstContents->hBGAction = SVC_PIPE_IncreaseActionVersion(pstContents->ulBGActionId);

	hErr = SVC_CH_StartTune (pstContents->hBGAction, &stRscInfo, pstContents->usUniqueId, &(pstContents->stBgTuneInfo));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_CH_StartTune fail!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}



/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

HBOOL xmgr_liveota_CheckSvcTypeOption (DxSvcType_e eSvcTypeInOption, DxSvcType_e eSvcType)
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

HERROR xmgr_liveota_CheckActionId (xmgrLiveOta_Context_t *pstContents, Handle_t hAction)
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

static HERROR xmgr_liveota_mevtLiveNotifySvcStopped(xmgrLiveOta_Context_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId = SVC_PIPE_GetActionId(hAction);
	HERROR		hErr = ERR_FAIL;

	if (MGR_ACTION_GetMainActionId() != ulActionId)
	{
		return ERR_FAIL;
	}

	hErr = xmgr_liveota_ReleaseBackGroundAction(pstContents);
	HxLOG_Debug("xmgr_liveota_ReleaseBackGroundAction() retrun hErr: %d \n", hErr);

	return hErr;
}

static HERROR xmgr_liveota_mevtLiveNotifySvcChanged(xmgrLiveOta_Context_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId = SVC_PIPE_GetActionId(hAction);
	HERROR		hErr = ERR_FAIL;

	if (MGR_ACTION_GetMainActionId() != ulActionId)
	{
		return ERR_FAIL;
	}

	// Release Background Action
	hErr = xmgr_liveota_ReleaseBackGroundAction(pstContents);
	HxLOG_Debug("xmgr_liveota_ReleaseBackGroundAction() retrun hErr: %d \n", hErr);

	// take background action.
	hErr = xmgr_liveota_SetupBackGroundAction(pstContents);
	
	if(hErr == ERR_FAIL)
	{
		HxLOG_Debug("xmgr_liveota_SetupBackGroundAction() fail. suicide\n");
	}
	else
	{
		HxLOG_Debug(HxANSI_COLOR_GREEN("[xmgr_liveota_EvtChLocked]Live OTA Start Tune\n"));
		hErr = xmgr_liveota_StartTune(pstContents);
	}
	
	return hErr;
}

HERROR xmgr_liveota_EvtChLocked (xmgrLiveOta_Context_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulActionId = SVC_PIPE_GetActionId (hAction);
	HERROR					hErr = ERR_FAIL;

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

	// start si Live OTA
	hErr = SVC_SI_StartLiveSwupCheck(pstContents->hBGAction, &(pstContents->stSiLiveSwupSpec), &(pstContents->stSiLiveSwupOption), FALSE);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("cannot start live ota ! \n");
		return ERR_FAIL;
	}

	return ERR_OK;

}

static HERROR xmgr_liveota_EvtSiOtaSuccess (xmgrLiveOta_Context_t *pstContents, DxSwUpdate_Signal_e eSignalType, SvcSi_SwupSignal_t *pstSwUpInfo)
{
	HERROR	hErr;

	if(eSignalType == eDxSWUPDATE_SIGNAL_SINGLE)
	{
		SvcSwUpdate_DetectResult_t			*pstDetectResult;
		DxTuneParam_t					stChTuneInfo;
		DbSvc_TsInfo_t 					stOtaTsInfo;


		HxSTD_MemSet(&stChTuneInfo, 0x00, sizeof(DxTuneParam_t));
		HxSTD_MemSet(&stOtaTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

		pstDetectResult= (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_DetectResult_t));
		if(pstDetectResult != NULL)
		{
			HxSTD_MemSet(pstDetectResult, 0x00, sizeof(SvcSwUpdate_DetectResult_t));
			hErr = SVC_CH_GetLockedInfo (pstContents->ulBGActionId, &stChTuneInfo);
			if(hErr != ERR_OK)
			{
				// 어떻게 처리하지....??????
			}

			OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&stChTuneInfo, &stOtaTsInfo.stTuningParam);

			stOtaTsInfo.eDeliType = stChTuneInfo.eDeliType;

			pstDetectResult->bAvailable 		= TRUE;
			pstDetectResult->bFound		= TRUE;
			pstDetectResult->ulDataVersion	= pstSwUpInfo->ulDataVersion;
			pstDetectResult->usOtaPid 		= pstSwUpInfo->usPid;
			pstDetectResult->eSource		= eDxSWUPDATE_SOURCE_Rf;

			HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stChTuningInfo, &stChTuneInfo, sizeof(DxTuneParam_t));
			HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stSvcTsInfo, &stOtaTsInfo, sizeof(DbSvc_TsInfo_t));
			pstDetectResult->data.stRfInfo.eDeliType		= stOtaTsInfo.eDeliType;
			pstDetectResult->data.stRfInfo.usPid			= pstSwUpInfo->usPid;
			pstDetectResult->data.stRfInfo.eDownloadType 	= eSWUP_DOWNLOAD_DVBSSU;

			hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32)pstDetectResult, sizeof(SvcSwUpdate_DetectResult_t));
			if(hErr != ERR_OK)
			{
				// 어떻게 처리하지....??????
			}

			MGR_SWUPINFO_SetSearchedInfo (&stOtaTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)pstSwUpInfo);

			BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_SUCCESS, HANDLE_NULL, LIVE_SWUPDATE_HANDLE, eDxSWUPDATE_SIGNAL_SINGLE, (HINT32)pstDetectResult);

			return ERR_OK;
		}
		else
		{
			HxLOG_Error("HLIB_STD_MemAlloc Error! \n");
			return ERR_FAIL;
		}
	}
	else if(eSignalType == eDxSWUPDATE_SIGNAL_MULTI)
	{
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_SUCCESS, HANDLE_NULL, LIVE_SWUPDATE_HANDLE, eDxSWUPDATE_SIGNAL_MULTI, 0);

		return ERR_OK;
	}
	else
	{
		HxLOG_Error("Not Support eSignalType ! \n");

		return ERR_FAIL;
	}
}

static HERROR xmgr_liveota_EvtSiOtaFail (xmgrLiveOta_Context_t *pstContents)
{
	BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, LIVE_SWUPDATE_HANDLE, 0, 0);
	return MESSAGE_BREAK;
}

#define	________________Proc_Function_________________________

STATIC BUS_Result_t	proc_liveota(HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrLiveOta_Context_t					*pstContents = xmgr_liveota_GetContent();
	HERROR							hErr = ERR_FAIL;
//	HBOOL							bUpdated = FALSE;

	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("[%s] eMEVT_BUS_CREATE\n",__FUNCTION__);
			(void)xmgr_liveota_InitContext(pstContents);
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Print("[%s] eMEVT_BUS_DESTROY\n",__FUNCTION__);
			(void)xmgr_liveota_ReleaseBackGroundAction(pstContents);
			(void)xmgr_liveota_InitContext(pstContents);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_STOPPED:
			HxLOG_Print("[%s] eMEVT_LIVE_NOTIFY_SVC_STOPPED\n",__FUNCTION__);
			hErr = xmgr_liveota_mevtLiveNotifySvcStopped(pstContents, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			HxLOG_Print("[%s] eMEVT_LIVE_NOTIFY_SVC_CHANGED\n",__FUNCTION__);
			hErr = xmgr_liveota_mevtLiveNotifySvcChanged(pstContents, hAction, p1, p2, p3);
			break;

		// channel event.
		case eSEVT_CH_LOCKED :
			HxLOG_Print("[%s] eSEVT_CH_LOCKED\n",__FUNCTION__);
			hErr = xmgr_liveota_EvtChLocked (pstContents, hAction, p1, p2, p3);
			break;

		// si swupdate event.
		case eSEVT_SWUP_DETECT_SUCCESS:
			hErr = xmgr_liveota_CheckActionId (pstContents, hAction);
			if (hErr == ERR_FAIL)
			{
				return MESSAGE_PASS;
			}

			HxLOG_Print("[%s] eSEVT_SWUP_DETECT_SUCCESS\n",__FUNCTION__);
#if defined(CONFIG_OP_ERTELECOM)
			// stop si dynamic update
			hErr = SVC_SI_StopLiveSwupCheck(pstContents->hBGAction, FALSE);
#endif
			hErr = xmgr_liveota_EvtSiOtaSuccess (pstContents, (DxSwUpdate_Signal_e) p1, (SvcSi_SwupSignal_t *)p2);

			return MESSAGE_BREAK;

		case eSEVT_SWUP_DETECT_FAIL:
			hErr = xmgr_liveota_CheckActionId (pstContents, hAction);
			if (hErr == ERR_FAIL)
			{
				return MESSAGE_PASS;
			}

			HxLOG_Print("[%s] eSEVT_SWUP_DETECT_FAIL\n",__FUNCTION__);
#if defined(CONFIG_OP_ERTELECOM)
			// stop si dynamic update
			hErr = SVC_SI_StopLiveSwupCheck(pstContents->hBGAction, FALSE);
#endif
			hErr = xmgr_liveota_EvtSiOtaFail (pstContents);
			if (hErr == ERR_OK)
			{
				return MESSAGE_BREAK;
			}
			break;
	}

	return BUS_ProcessMessageDefault(lMessage, hAction, p1, p2, p3);
}

#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/


HERROR	xmgr_liveota_Start(void)
{
	BUS_Result_t		eRes = ERR_BUS_NO_ERROR;

	if (NULL == BUS_MGR_Get(proc_liveota))
	{
		// live swup proc이 없으면 생성한다
		eRes = BUS_MGR_Create((char*)"proc_liveota", APP_ACTION_PRIORITY, proc_liveota, HANDLE_NULL, 0, 0, 0);
	}

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_liveota_Stop(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Destroy(proc_liveota);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

BUS_Callback_t xmgr_liveota_GetProc(void)
{
	return proc_liveota;
}

/*********************** End of File ******************************/

