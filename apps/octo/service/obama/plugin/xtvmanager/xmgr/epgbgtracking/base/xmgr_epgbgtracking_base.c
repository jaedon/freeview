/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_epg.h>
#include <svc_resources.h>

#include <mgr_common.h>
#include <mgr_rscmap.h>
#include <mgr_action.h>
#include <mgr_schedule.h>

#include "../local_include/_xmgr_epgbgtracking.h"
#include <_mgr_epgbgtracking.h>

#include <otl.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


// eit pf 관련 filter가 잘못 걸린건지, live가 있는데도 other가 안 들어오는 문제가 있는 듯 하다.
// 임시방편으로 epg bg tracking에서도 epg를 걸게 하지만, live의 eit pf filter가 고쳐 질 경우 아래 define은 제거되어야 한다.
//#define TEST_ALWAYS_ON
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC EpgBGTracking_Content_t		s_stEpgBgTrackActionContent = {0, };

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/
BUS_Result_t xmgr_epgbgtracking_GetContent_Base(EpgBGTracking_Content_t **ppstContent);

#define _________Staitc_Function_____________

STATIC BUS_Result_t xmgr_epgbgtracking_StartEpgBgTracking_Base(void)
{
	BUS_Result_t					lResult = ERR_FAIL;
	EpgBGTracking_Content_t		*pstContent = NULL;
	HERROR						hError = ERR_FAIL;

	HxLOG_Debug(" +\n");
	lResult = xmgr_epgbgtracking_GetContent_Base(&pstContent);
	if(lResult != ERR_OK)
	{
		HxLOG_Debug("xmgr_epgbgtracking_GetContent_Base fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	HxLOG_Debug(" Content hAction(0x%x), actionId(%d), hTrackingSvc(0x%x) \n",pstContent->hAction, pstContent->ulActionId, pstContent->hTrackService);

	hError = SVC_EPG_Stop(eEPG_KIND_DVB, eEPG_EVT_KIND_ALL, pstContent->hAction);
	if (hError != ERR_OK)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("SVC_EPG_Stop failed. hAction=(0x%x) \n "),pstContent->hAction);
	}

	/* MW EPG Live를 통해 Tracking을 하는 구조는 변경될 수 있음 */
	//hError = MW_EPG_StartLivePF(pstContent->hAction, pstContent->hTrackService);

	hError = SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_PF_ALL, pstContent->hAction, pstContent->hTrackService, 0, 0, 0);
	if (hError != ERR_OK)
	{
		HxLOG_Debug(" SVC_EPG_Start(%d, eEPG_EVT_KIND_PF_ALL) fail!!\n", eEPG_KIND_DVB);
		goto EXIT_EPGBGACT_FUNC;
	}

	/* Tracking 동안 EIT Schedule Update 반영을 위해 함께 걸어주자 */
	//hError = MW_EPG_StartLiveSCH(pstContent->hAction, pstContent->hTrackService);

	hError = SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_SCH_ALL, pstContent->hAction, pstContent->hTrackService, 0, 0, 0);
	if (hError != ERR_OK)
	{
		HxLOG_Debug(" SVC_EPG_Start(%d, eEPG_EVT_KIND_SCH_ALL fail!!\n", eEPG_KIND_DVB);
		goto EXIT_EPGBGACT_FUNC;
	}

	lResult = ERR_BUS_NO_ERROR;
	HxLOG_Debug(" -\n");

EXIT_EPGBGACT_FUNC :

	return lResult;
}

STATIC BUS_Result_t xmgr_epgbgtracking_TuneTrackingSvc_Base(void)
{
	BUS_Result_t					lResult = ERR_FAIL;
	HERROR						 hError = ERR_FAIL;
	EpgBGTracking_Content_t		*pstContent = NULL;
	SvcRsc_Info_t						stRscInfo;

	HxLOG_Debug(" +\n");
	lResult = xmgr_epgbgtracking_GetContent_Base(&pstContent);
	if(lResult != ERR_OK)
	{
		HxLOG_Debug("[xmgr_epg_bgtrack_TuneTrackingSvc_Base] xmgr_epgbgtracking_GetContent_Base fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	HxLOG_Debug(" Content hAction(0x%x), actionId(%d), hTrackingSvc(0x%x) \n",pstContent->hAction, pstContent->ulActionId, pstContent->hTrackService);

	hError = SVC_CH_StopTune(	SVC_PIPE_GetActionHandle(pstContent->ulActionId),
								(HUINT16)-1,
								eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Debug(" SVC_CH_StopTune fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	// version up~
	pstContent->hAction = SVC_PIPE_IncreaseActionVersion(pstContent->ulActionId);

	MGR_RSC_GetResourceInfo (pstContent->ulActionId, &stRscInfo);
	hError = SVC_CH_StartTune(	pstContent->hAction, &stRscInfo,
								(HUINT16)pstContent->stTsInfo.usUniqueId,
								&pstContent->stTuningInfo);

	if (hError != ERR_OK)
	{
		HxLOG_Debug(" SVC_CH_StartTune fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	HxLOG_Debug(" -\n");
	lResult = ERR_BUS_NO_ERROR;

EXIT_EPGBGACT_FUNC :

	return lResult;
}

STATIC BUS_Result_t xmgr_epgbgtracking_InitApp_Base(Handle_t hTrackHandle)
{
	BUS_Result_t					lResult = ERR_FAIL;
	HERROR						hError = ERR_FAIL;
	EpgBGTracking_Content_t		*pstContent = NULL;
	DbSvc_TsInfo_t					stTsInfo;
	DxTuneParam_t				stTuningInfo;
	HUINT32						ulActionId;

	HxLOG_Debug(" +\n");

	/* Tracking Handle은 RSVM에서 가져오는 구조로 해야 함 - 우선은 임시. */
	if(hTrackHandle == HANDLE_NULL)
	{
		HxLOG_Debug(" param fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	lResult = xmgr_epgbgtracking_GetContent_Base(&pstContent);
	if(lResult != ERR_OK)
	{
		HxLOG_Debug(" xmgr_epgbgtracking_GetContent_Base fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	HxLOG_Debug(" Content hAction(0x%x), actionId(%d), hTrackingSvc(0x%x) \n",pstContent->hAction, pstContent->ulActionId, pstContent->hTrackService);

	// MGR_ACTION_DestoryBackGroundAction(eActionId_BG_0);
	// MGR_ACTION_DestoryBackGroundAction(eActionId_BG_1);

	/* Get Bg Action. */
	hError = MGR_ACTION_GetEmptyBgActionId (&ulActionId);
	if (ERR_OK != hError)
	{
		HxLOG_Debug(" FAIL : Get Background Action ID!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	VK_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
	VK_memset(&stTuningInfo, 0x00, sizeof(DxTuneParam_t));
	hError = DB_SVC_GetTsInfoByServiceHandle(hTrackHandle, &stTsInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Debug(" DB_SVC_GetTsInfoByServiceHandle fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}
	hError = OTL_CONV_DbTuningInfo2DxTuneParam(&stTsInfo, &stTuningInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Debug(" MWC_UTIL_ConvDb2ChTuningParam fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}
	VK_memcpy(&pstContent->stTsInfo, &stTsInfo, sizeof(DxTuneParam_t));
	VK_memcpy(&pstContent->stTuningInfo, &stTuningInfo, sizeof(DxTuneParam_t));

#if defined(CONFIG_MW_MM_PVR)
/**************************************************************************************************
  *
  * Action setup
  *
  *************************************************************************************************/
	hError = MGR_ACTION_SetupGetEitPfInfo (ulActionId, hTrackHandle, eAmPriority_BGJOB);
	if(hError != ERR_OK)
	{
		HxLOG_Debug("MGR_ACTION_SetupEpgBgTrack fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	pstContent->ulActionId = ulActionId;
	pstContent->hTrackService = hTrackHandle;
	pstContent->hAction = HANDLE_NULL;

	hError = MGR_ACTION_Take(ulActionId, eActionType_EIT_PF_TUNE, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if(hError != ERR_OK)
	{
		HxLOG_Debug(" MGR_ACTION_Take fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}
#endif

	lResult = ERR_BUS_NO_ERROR;
	HxLOG_Debug(" -\n");

EXIT_EPGBGACT_FUNC :

	return lResult;
}

STATIC BUS_Result_t xmgr_epgbgtracking_StopEpgBgTracking(void)
{
	BUS_Result_t					lResult = ERR_FAIL;
	HERROR						hError = ERR_FAIL;
	EpgBGTracking_Content_t		*pstContent = NULL;

	HxLOG_Debug(" +\n");
	lResult = xmgr_epgbgtracking_GetContent_Base(&pstContent);
	if (lResult != ERR_OK)
	{
		HxLOG_Debug(" xmgr_epgbgtracking_GetContent_Base fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	HxLOG_Debug(" Content hAction(0x%x), actionId(%d), hTrackingSvc(0x%x) \n",pstContent->hAction, pstContent->ulActionId, pstContent->hTrackService);

	if (pstContent->hTrackService == HANDLE_NULL)
	{
		HxLOG_Debug(" Currently not tracking. Skip Ch Stop process.\n");
		lResult = ERR_BUS_NO_ERROR;
		goto EXIT_EPGBGACT_FUNC;
	}

	hError = SVC_EPG_Stop(eEPG_KIND_DVB, eEPG_EVT_KIND_ALL, pstContent->hAction);
	if (hError != ERR_OK)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("SVC_EPG_Stop failed. hAction=(0x%x) \n "),pstContent->hAction);
	}

	hError = SVC_CH_StopTune(pstContent->hAction, (HUINT16)-1, eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Debug(" SVC_CH_StopTune fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}


	hError = MGR_ACTION_Release(pstContent->ulActionId, eActionType_EIT_PF_TUNE);
	if(hError != ERR_OK)
	{
		HxLOG_Debug(" MGR_ACTION_Release fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	VK_memset(&pstContent->stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
	VK_memset(&pstContent->stTuningInfo, 0x00, sizeof(DxTuneParam_t));

	pstContent->ulActionId = 0xffff;		// invalid id value
	pstContent->hAction		= HANDLE_NULL;
	pstContent->hTrackService = HANDLE_NULL;

	HxLOG_Debug(" -\n");

	lResult = ERR_BUS_NO_ERROR;

EXIT_EPGBGACT_FUNC :

	return lResult;
}

STATIC HERROR xmgr_epgbgtracking_GetTrackingSvc_Base (Handle_t *phTrackingHandle)
{
	HINT32					nPaddingTime;
	HERROR					hError;

#if defined(CONFIG_MW_MM_PVR)
	MgrRec_RecInfo_t	stRecInfo;
	HUINT32 				i;
#endif
#if defined(TEST_ALWAYS_ON)
#else
	ActionId_t				eMainActionId;
	MgrAction_Type_e			eActionType;
#endif
	/* 1. Tracking 할 Reservation이 존재하는지 확인 */
	*phTrackingHandle = MGR_SCHEDULE_GetEarliestTrackingSvc();
	if (*phTrackingHandle == HANDLE_NULL)
	{
		return ERR_OK;
	}

#if defined(TEST_ALWAYS_ON)
	/* 2. Auto Tracking Mode가 아닐 경우 Tracking Manager를 생성할 필요가 없음 */
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_START, (HUINT32 *)&nPaddingTime, 0);
	if (ERR_OK != hError)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_START) Error\n");
	}
	if (nPaddingTime != 0)
	{
		HxLOG_Debug(" It isn't Auto Tracking mode(PRE)\n");
		return ERR_OK;
	}

	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_STOP, (HUINT32 *)&nPaddingTime, 0);
	if (ERR_OK != hError)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_STOP) Error\n");
	}
	if (nPaddingTime != 0)
	{
		HxLOG_Debug(" It isn't Auto Tracking mode(POST)\n");
		return ERR_OK;
	}

	// ALWAYS_ON으로 살아남기 위해선 붙을 수 있는 tuner에 잘 붙어야 한다. (3rec 중에 다른 TP에 EIT pf setup을 요청할순 없다)
#if defined(CONFIG_MW_MM_PVR)
	/* 5. Recording 중인 Action의 Service Handle */
	for (i = eActionId_REC_FIRST; i<= eActionId_REC_LAST; i++)
	{
		if (MGR_ACTION_GetType(i) == eRecType_REC)
		{
			hError = MGR_RECORDER_GetRecordingSvcInfo(i, &stRecInfo);
			if(hError == ERR_OK)
			{
				if(stRecInfo.hSvc != HANDLE_NULL)
				{
					HxLOG_Debug(" Recording mode. Return Recording Svc Handle \n");

					*phTrackingHandle = stRecInfo.hSvc;
					goto FIND_TRACKING_SERVICE_HANDLE;
				}
			}
		}
	}
#endif
	/* 6. Rsv 중의 Tracking이 필요한 Service Handle */
	if (*phTrackingHandle == HANDLE_NULL)
	{
		*phTrackingHandle = MGR_SCHEDULE_GetEarliestTrackingSvc();
	}

#else
	*phTrackingHandle = HANDLE_NULL;

	/* 2. Auto Tracking Mode가 아닐 경우 Tracking Manager를 생성할 필요가 없음 */
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_START, (HUINT32 *)&nPaddingTime, 0);
	if (ERR_OK != hError)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_START) Error\n");
	}
	if (nPaddingTime != 0)
	{
		HxLOG_Debug(" It isn't Auto Tracking mode(PRE)\n");
		return ERR_OK;
	}

	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_STOP, (HUINT32 *)&nPaddingTime, 0);
	if (ERR_OK != hError)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_STOP) Error\n");
	}
	if (nPaddingTime != 0)
	{
		HxLOG_Debug(" It isn't Auto Tracking mode(POST)\n");
		return ERR_OK;
	}

	/* 3. WTV 또는 Search의 경우 Tracking mgr 생성할 필요 없음 */
	eMainActionId = MGR_ACTION_GetMainActionId();
	eActionType = MGR_ACTION_GetType(eMainActionId);
	if (eActionType == eViewType_WTV || eActionType == eActionType_SEARCH)
	{
		HxLOG_Debug(" Main Action type is WTV or Search\n");
		return ERR_OK;
	}

#if defined(CONFIG_MW_MM_PVR)
	/* 4. TSR PB의 경우 TSR Rec Service Handle 사용 */
	if (eActionType == eViewType_TSR_PB)
	{
		MgrPb_Info_t		 stPbInfo;

		hError = MGR_PLAYBACK_GetPlaybackInfo (eMainActionId, &stPbInfo);
		if (ERR_OK == hError)
		{
			HxLOG_Debug(" TSR Playback mode. Return TSR REC Svc.\n");

			*phTrackingHandle = stPbInfo.hSvc;
			goto FIND_TRACKING_SERVICE_HANDLE;
		}
	}

	/* 5. Recording 중인 Action의 Service Handle */
	for (i = eActionId_REC_FIRST; i<= eActionId_REC_LAST; i++)
	{
		if (MGR_ACTION_GetType(i) == eRecType_REC)
		{
			hError = MGR_RECORDER_GetRecordingSvcInfo(i, &stRecInfo);
			if(hError == ERR_OK)
			{
				if(stRecInfo.hSvc != HANDLE_NULL)
				{
					HxLOG_Debug(" Recording mode. Return Recording Svc Handle \n");

					*phTrackingHandle = stRecInfo.hSvc;
					goto FIND_TRACKING_SERVICE_HANDLE;
				}
			}
		}
	}
#endif
	/* 6. Rsv 중의 Tracking이 필요한 Service Handle */
	if (*phTrackingHandle == HANDLE_NULL)
	{
		*phTrackingHandle = MGR_SCHEDULE_GetEarliestTrackingSvc();
	}
#endif

#if defined(CONFIG_MW_MM_PVR)
FIND_TRACKING_SERVICE_HANDLE:
#endif
#if defined(CONFIG_DEBUG)
	if (*phTrackingHandle != HANDLE_NULL)
	{
		DbSvc_Info_t	stSvcInfo;
		hError = DB_SVC_GetServiceInfo(*phTrackingHandle, &stSvcInfo);
		if (hError == ERR_OK)
		{
			HxLOG_Debug(" Tracking Svc : %d %s\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
		}
	}
#endif

	return ERR_OK;
}


STATIC HERROR xmgr_epgbgtracking_ChangeTrackingService_Base(EpgBGTracking_Content_t *pstContent)
{
	Handle_t						hTrackingSvc = HANDLE_NULL;
	BUS_Result_t					lResult = ERR_FAIL;
	HERROR							hErr;

	if(pstContent == NULL)
		return ERR_FAIL;

	/* Tracking Svc Handle을 변경해야 하는지 판단 */
	hErr = xmgr_epgbgtracking_GetTrackingSvc_Base(&hTrackingSvc);
	if (hErr != ERR_OK)
	{
		HxLOG_Debug(" Get tracking svc failed. \n");
		return ERR_FAIL;
	}

	if (hTrackingSvc == HANDLE_NULL)
	{
		// TODO: 이 경우에는 destroy 0 가 되야 하지 않을까?
		HxLOG_Debug(" hTrackingSvc == HANDLE_NULL\n");
		return ERR_FAIL;
	}
/*
	if (hTrackingSvc == pstContent->hTrackService)
	{
		HxLOG_Debug("[xmgr_epg_bgtrack_ChangeTrackingService_Base] hTrackingSvc == pstContent->hTrackService.\n");
		return ERR_OK;
	}
*/
	lResult  = xmgr_epgbgtracking_StopEpgBgTracking();
	if(lResult != ERR_OK)
	{
		HxLOG_Debug(" xmgr_epg_bgtrack_DestroyApp fail!!\n");
		return ERR_FAIL;
	}

	lResult  = xmgr_epgbgtracking_InitApp_Base(hTrackingSvc);
	if(lResult != ERR_OK)
	{
		HxLOG_Debug(" xmgr_epg_bgtrack_InitApp_Base fail!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xmgr_epgbgtracking_checkActionIdAndType (EpgBGTracking_Content_t *pstContent, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulActionId = SVC_PIPE_GetActionId (hAction);
	MgrAction_Type_e		 eActType = eActionType_NONE;

	HxLOG_Trace();

	eActType = MGR_ACTION_GetType(ulActionId);

	switch (eActType)
	{
	case eActionType_EIT_PF_TUNE:
		if (pstContent->ulActionId!= ulActionId)
		{
			HxLOG_Debug("ulActionId is Not Mine!!\n");
			return ERR_FAIL;
		}
		break;

	default:
		HxLOG_Debug("not defined action type: %d \n", eActType);
		return ERR_FAIL;
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);

	return ERR_OK;
}


#define _________Module_Function_____________

BUS_Result_t xproc_epgbgtracking_Base(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	BUS_Result_t					lResult = ERR_FAIL;
	EpgBGTracking_Content_t		*pstContent = NULL;
	HUINT32							ulActionId = SVC_PIPE_GetActionId (hHandle);
	Handle_t						hTrackingSvc;
	HERROR							hErr;

	lResult = xmgr_epgbgtracking_GetContent_Base(&pstContent);
	if(lResult != ERR_OK || pstContent == NULL)
	{
		HxLOG_Debug(" xmgr_epgbgtracking_GetContent_Base fail!!\n");
		return MESSAGE_PASS;
	}

	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Debug("MSG_BUS_CREATE\n");
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			VK_memset(pstContent, 0, sizeof(EpgBGTracking_Content_t));
			pstContent->hTrackService = HANDLE_NULL;

			hErr = xmgr_epgbgtracking_GetTrackingSvc_Base(&hTrackingSvc);
			if (hErr != ERR_OK)
			{
				HxLOG_Debug(" destroy by action. \n");
				break;
			}

			if (hTrackingSvc == HANDLE_NULL)
			{
				HxLOG_Debug(" hTrackingSvc == HANDLE_NULL\n");
				xmgr_epgbgtracking_StopEpgBgTracking();

				break;
			}

			lResult = xmgr_epgbgtracking_InitApp_Base(hTrackingSvc);
			if(lResult != ERR_OK)
			{
				HxLOG_Debug(" xmgr_epg_bgtrack_InitApp_Base fail!!\n");
				xmgr_epgbgtracking_StopEpgBgTracking();
			}
			break;

		case eSEVT_CH_NO_SIGNAL:
			HxLOG_Debug("EVT_CH_NO_SIGNAL ulActionId(%d), ActionType(%d)!!\n", ulActionId, lParam1);
			break;

		case eSEVT_CH_STOPPED:
			HxLOG_Debug("EVT_CH_STOPPED ulActionId(%d), ActionType(%d)!!\n", ulActionId, lParam1);
			break;

		case eSEVT_CH_LOCKED :
			HxLOG_Debug(" EVT_CH_LOCKED ulActionId(%d), ActionType(%d)!!\n", ulActionId, lParam1);
			hErr = xmgr_epgbgtracking_checkActionIdAndType(pstContent, hHandle, lParam1, lParam2, lParam3);
			if (hErr != ERR_OK)
			{
				HxLOG_Debug(HxANSI_COLOR_RED("Not my action. \n"));
				break;
			}

			lResult = xmgr_epgbgtracking_StartEpgBgTracking_Base();
			if(lResult != ERR_OK)
			{
				HxLOG_Debug(" xmgr_epg_bgtrack_StartEpgBgTracking_Base fail!!\n");
				xmgr_epgbgtracking_StopEpgBgTracking();
				break;
			}
			break;

		case eMEVT_ACTION_NOTIFY_TAKEN:
			HxLOG_Debug("MSG_ACTION_NOTIFY_TAKEN ulActionId(%d), ActionType[%s]!!\n",
												ulActionId, MGR_ACTION_GetTypeStr(lParam1));

#if defined TEST_ALWAYS_ON
			// 새로 걸기 위해 change 하는 의미가 중요하다. 어디로 걸리는지는 중요하지 않다.
			if ((eViewType_WTV == lParam1)
				|| (eRecType_REC == lParam1))
			{
				hErr = xmgr_epgbgtracking_ChangeTrackingService_Base(pstContent);
				if ( hErr != ERR_OK )
				{
					HxLOG_Debug("change tracking service by rec failed. \n");
					xmgr_epgbgtracking_StopEpgBgTracking();
				}
			}
#else
			if(eViewType_WTV == lParam1)
			{
				/* WTV가 시작되면 Tracking은 Live에서 동작 한다. */
				HxLOG_Debug("Destroy EPG Background Tracking App.!!\n");
				xmgr_epgbgtracking_StopEpgBgTracking();
			}
			else if(eRecType_REC == lParam1)
			{
				hErr = xmgr_epgbgtracking_ChangeTrackingService_Base(pstContent);
				if ( hErr != ERR_OK )
				{
					HxLOG_Debug("change tracking service by rec failed. \n");
					xmgr_epgbgtracking_StopEpgBgTracking();
				}
			}
#endif
			else if(eActionType_EIT_PF_TUNE == lParam1 && pstContent->ulActionId == ulActionId)
			{
				/**************************************************************************************************
				  *
				  * Tune Tracking channel
				  *
				  *************************************************************************************************/
				lResult = xmgr_epgbgtracking_TuneTrackingSvc_Base();
				if(lResult != ERR_OK)
				{
					HxLOG_Debug(" xmgr_epg_bgtrack_TuneTrackingSvc_Base fail!!\n");
					xmgr_epgbgtracking_StopEpgBgTracking();
				}
			}
			break;

		case eMEVT_ACTION_NOTIFY_RELEASED:
			HxLOG_Debug(" MSG_ACTION_NOTIFY_RELEASED ulActionId(%d), ActionType[%s]!!\n", ulActionId, MGR_ACTION_GetTypeStr(lParam1));
			if(eRecType_REC == lParam1
				|| eActionType_SEARCH == lParam1
				|| eViewType_WTV == lParam1)	// live 시작 시에 bgtracking을 안 죽여서, live end 시에 bgtracking을 다시 시작할 수 있게 한다.

			{
				/* Recording 종료 또는 Search가 끝나게 되면 Tracking handle을 검사해야 한다. */
				hErr = xmgr_epgbgtracking_ChangeTrackingService_Base(pstContent);
				if ( hErr != ERR_OK )
				{
					xmgr_epgbgtracking_StopEpgBgTracking();
				}
			}
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Debug(" MSG_BUS_DESTROY  \n");
			lResult = xmgr_epgbgtracking_StopEpgBgTracking();
			if(lResult != ERR_OK)
			{
				HxLOG_Debug("  xmgr_epg_bgtrack_DestroyApp fail!!\n");
				break;
			}
			break;

	//	case EVT_SI_SEARCH_FAIL:
	//		return MESSAGE_BREAK;
	}

	return MESSAGE_PASS;
}


BUS_Result_t xmgr_epgbgtracking_GetContent_Base(EpgBGTracking_Content_t **ppstContent)
{
	BUS_Result_t					lResult = ERR_FAIL;

	if(ppstContent == NULL)
	{
		HxLOG_Debug("  param fail!!\n");
		goto EXIT_EPGBGACT_FUNC;
	}

	*ppstContent = &s_stEpgBgTrackActionContent;

	lResult = ERR_BUS_NO_ERROR;

EXIT_EPGBGACT_FUNC :

	return lResult;

}

HERROR xmgr_epgbgtracking_Start_Base(void)
{
	BUS_Result_t	eRes = ERR_BUS_NO_OBJECT;

	if(BUS_MGR_Get(xproc_epgbgtracking_Base) == NULL)
	{
		HxLOG_Debug("  Create action for tracking ...  \n");
		eRes = BUS_MGR_Create("xproc_epgbgtracking", APP_ACTION_PRIORITY, xproc_epgbgtracking_Base, HANDLE_NULL, 0, 0, 0);
	}
	else
	{
		HxLOG_Debug("Tracking proc exists. \n");
	}

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_epgbgtracking_Stop_Base(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Destroy(xproc_epgbgtracking_Base);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

// for batch
BUS_Callback_t	xmgr_epgbgtracking_GetProc_Base(void)
{
	return xproc_epgbgtracking_Base;
}


/*********************** End of File ******************************/

