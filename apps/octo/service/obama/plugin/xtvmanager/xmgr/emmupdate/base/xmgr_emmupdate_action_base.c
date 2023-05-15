/**
	@file     mgr_emmupdate.c
	@brief    file_name & simple comment.

	Description: for portal UI \n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/11		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_pipe.h>

#include <bus.h>


#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_common.h>

#include <svc_resources.h>
#include <svc_ch.h>
#include <svc_si.h>
#include <svc_epg.h>
#include <svc_cas.h>

#include <svc_resources.h>
#include <mgr_rscmap.h>
#include <mgr_emmupdate.h>
#include <mgr_cas.h>

#include <db_svc.h>
#include "../local_include/_xmgr_emmupdate.h"

#include <otl.h>

#include <sapi.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	EMMUPDATE_TIMER_LOCKING_ID				1
#define	EMMUPDATE_TIMER_LOCKING_TIME			(4*1000)	/* 4 sec */

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagEMMUpdateContent
{
	HUINT32				ulActionId;
	Handle_t			hAction;

	HUINT32				ulScheduleTimerId;
	Handle_t			hHomeSvcHandle;
	DxTuneParam_t		stTuningInfo;
}EMMUpdate_Content_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC EMMUpdate_Content_t	s_stEMMUpdateContent;


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC BUS_Result_t	xmgr_emmupdate_GetContent(EMMUpdate_Content_t **ppContent);
STATIC INLINE HERROR 	xmgr_emmupdate_CheckActionId (EMMUpdate_Content_t *pstContents, Handle_t hAction);

STATIC BUS_Result_t	xmgr_emmupdate_InitApp(void);
STATIC BUS_Result_t	xmgr_emmupdate_StartSvcChannelService(void);
STATIC BUS_Result_t	xmgr_emmupdate_TuneSvcChannel (void);
STATIC BUS_Result_t	xmgr_emmupdate_DestroyApp(void);
STATIC BUS_Result_t	proc_emmupdate_Action_Base(HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);


#define	________________Static_Function_________________________

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

STATIC BUS_Result_t	xmgr_emmupdate_GetContent(EMMUpdate_Content_t **ppContent)
{
	BUS_Result_t	lResult = ERR_FAIL;

	if(ppContent == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	*ppContent = &s_stEMMUpdateContent;

	lResult = ERR_BUS_NO_ERROR;

EXIT_EMMUPDATE_FUNC :

	return	lResult;
}

STATIC INLINE HERROR xmgr_emmupdate_CheckActionId (EMMUpdate_Content_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);

	if (pstContents->ulActionId != ulActionId)
	{
		HxLOG_Print("is not matched actionID(0x%x)! line(%d)\n", ulActionId, __LINE__);
		return MESSAGE_PASS;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	xmgr_emmupdate_InitApp(void)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				ulActionId = 0;
	DbSvc_TsInfo_t			stTsInfo;
	BUS_Result_t			lResult = ERR_FAIL;
	DxTuneParam_t			stTuningInfo;
	EMMUpdate_Content_t		*pContent = NULL;
	HBOOL					bActionFail = FALSE;
	Handle_t			hSvc = HANDLE_NULL;

	HxLOG_Trace();

	// Init Content
	hError = xmgr_emmupdate_GetContent(&pContent);
	if(hError != ERR_OK)
	{
		HxLOG_Print("xmgr_emmupdate_GetContent fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	// emm update를 얻기 위한 서비스 get
	if ( MGR_EMMUpdate_GetStartSvc(&hSvc) != ERR_OK)
		goto EXIT_EMMUPDATE_FUNC;

	hError = MGR_ACTION_GetEmptyBgActionId(&ulActionId);
	if (hError != ERR_OK)
	{
		HxLOG_Info("cannot get bg action id\n");
		bActionFail = TRUE;
		goto EXIT_EMMUPDATE_FUNC;
	}
	pContent->ulActionId = ulActionId;
	pContent->hHomeSvcHandle = hSvc;

	HxSTD_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
	HxSTD_memset(&stTuningInfo, 0x00, sizeof(DxTuneParam_t));
	hError = DB_SVC_GetTsInfoByServiceHandle(hSvc, &stTsInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Print("DB_SVC_GetTsInfoByServiceHandle fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stTuningInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Print("OTL_CONV_DbTuningInfo2DxTuneParam fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}
	HxSTD_memcpy(&pContent->stTuningInfo, &stTuningInfo, sizeof(DxTuneParam_t));

/**************************************************************************************************
  *
  * Action setup
  *
  *************************************************************************************************/
	hError = MGR_ACTION_SetupCasTune(pContent->ulActionId, hSvc, eAmPriority_VIEW_ACTION);
	if (hError != ERR_OK)
	{
		HxLOG_Print("MGR_ACTION_SetupCasTune fail!!\n");
		bActionFail = TRUE;
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = MGR_ACTION_Take(pContent->ulActionId, eActionType_CAS_TUNE, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hError != ERR_OK)
	{
		HxLOG_Print("MGR_ACTION_Take fail!!\n");
		bActionFail = TRUE;
		goto EXIT_EMMUPDATE_FUNC;
	}

/**************************************************************************************************
  *
  * Tune svc channel
  *
  *************************************************************************************************/
	hError = xmgr_emmupdate_TuneSvcChannel();
	if (hError != ERR_OK)
	{
		HxLOG_Print("xmgr_emmupdate_TuneSvcChannel fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	lResult = ERR_BUS_NO_ERROR;
	HxLOG_Print("lResult: %d \n", lResult);

	return lResult;

EXIT_EMMUPDATE_FUNC :

	HxLOG_Print("EXIT without emm update !!!\n");


	if(lResult != ERR_BUS_NO_ERROR)
	{
		BUS_MGR_DestroyWithReason(NULL, bActionFail?eDESTROY_BUS_ACTION:eDESTROY_BUS_MYSELF, 0);

	}

	return lResult;
}

STATIC BUS_Result_t	xmgr_emmupdate_StartSvcChannelService(void)
{
	HERROR				hError = ERR_FAIL;
	BUS_Result_t		lResult = ERR_FAIL;
	EMMUpdate_Content_t	*pContent = NULL;

	HxLOG_Trace();

	lResult = xmgr_emmupdate_GetContent(&pContent);
	if (lResult != ERR_OK)
	{
		HxLOG_Print("xmgr_emmupdate_GetContent fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = MGR_CAS_StartService(pContent->hAction, pContent->hHomeSvcHandle, eCAS_ActionType_Live, eDxCAS_GROUPID_NONE, NULL);
	if (hError != ERR_OK)
	{
		HxLOG_Print("MGR_CAS_StartService fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	lResult = ERR_BUS_NO_ERROR;
	HxLOG_Print("lResult: %d \n", lResult);

EXIT_EMMUPDATE_FUNC :

	return lResult;
}


STATIC BUS_Result_t	xmgr_emmupdate_TuneSvcChannel (void)
{
	HERROR				hError = ERR_FAIL;
	BUS_Result_t		lResult = ERR_FAIL;
	SvcRsc_Info_t			stRscInfo;
	EMMUpdate_Content_t	*pContent = NULL;

	HxLOG_Trace();

	lResult = xmgr_emmupdate_GetContent(&pContent);
	if (lResult != ERR_OK)
	{
		HxLOG_Print("xmgr_emmupdate_GetContent fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = SVC_CH_StopTune(SVC_PIPE_GetActionHandle(pContent->ulActionId), (HUINT16)-1, eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Print("SVC_CH_StopTune fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	// version up~
	pContent->hAction = SVC_PIPE_IncreaseActionVersion(pContent->ulActionId);

	MGR_RSC_GetResourceInfo (pContent->ulActionId, &stRscInfo);
	hError = SVC_CH_StartTune(pContent->hAction, &stRscInfo, (HUINT16)-1, &pContent->stTuningInfo);
	if (hError != ERR_OK)
	{
		HxLOG_Print("SVC_CH_StartTune fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	// DD에서 Tuning Signal이 올라오지 않아 무한으로 기다리는 것을 막기 위해 여기서 timer를 처리하도록 한다.
	//BUS_SetTimer (EMMUPDATE_TIMER_LOCKING_ID, EMMUPDATE_TIMER_LOCKING_TIME);
	lResult = ERR_BUS_NO_ERROR;
	HxLOG_Print("lResult: %d \n", lResult);

EXIT_EMMUPDATE_FUNC :

	return lResult;
}


STATIC BUS_Result_t	xmgr_emmupdate_DestroyApp(void)
{
	HERROR				hError = ERR_FAIL;
	BUS_Result_t		lResult = ERR_FAIL;
	EMMUpdate_Content_t	*pContent = NULL;

	HxLOG_Trace();

	lResult = xmgr_emmupdate_GetContent(&pContent);
	if (lResult != ERR_OK)
	{
		HxLOG_Print("xmgr_emmupdate_GetContent fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = SVC_CH_StopTune(pContent->hAction, (HUINT16)-1, eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Print("SVC_CH_StopTune fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = MGR_CAS_StopService(pContent->hAction, pContent->hHomeSvcHandle, eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Print("EXIT_EMMUPDATE_FUNC fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = MGR_ACTION_Release(pContent->ulActionId, eActionType_CAS_TUNE);
	if (hError != ERR_OK)
	{
		HxLOG_Print("MGR_ACTION_Release fail!!\n");
		goto EXIT_EMMUPDATE_FUNC;
	}


#if defined(CONFIG_APCORE_INFO_UPDATE_WAKEUP)
	BUS_PostMessage (NULL, eMEVT_EMMUPDATE_DONE, 0, 0, 0, 0);
#endif



	lResult = ERR_BUS_NO_ERROR;
	HxLOG_Print("lResult: %d \n", lResult);

EXIT_EMMUPDATE_FUNC :

	return lResult;
}


#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/
STATIC BUS_Result_t	proc_emmupdate_Action_Base(HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	HUINT32				ulActionId = 0;
	BUS_Result_t		lResult = ERR_FAIL;
	EMMUpdate_Content_t		*pstContents = NULL;

	ulActionId = SVC_PIPE_GetActionId (hAction);
	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("eMEVT_BUS_CREATE\n");
			lResult = xmgr_emmupdate_InitApp();
			if (lResult != ERR_OK)
			{
				HxLOG_Print("xmgr_emmupdate_InitApp fail!!\n");
				break;
			}
			break;

		case eMEVT_BUS_TIMER :
			HxLOG_Print("eMEVT_BUS_TIMER !!\n");
			break;

		case eMEVT_BUS_GO_OPERATION :
			HxLOG_Print("\t eMEVT_BUS_GO_OPERATION!! \n");
			lResult = BUS_MGR_Destroy(proc_emmupdate_Action_Base);
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			HxLOG_Print("\t eMEVT_BUS_READY_SHUTDOWN!! \n");
			return MESSAGE_PASS;

		case eSEVT_CH_LOCKED :
			HxLOG_Print("eSEVT_CH_LOCKED !!\n");
			lResult = xmgr_emmupdate_GetContent(&pstContents);
			if (lResult != ERR_OK)
			{
				HxLOG_Print("xmgr_emmupdate_GetContent fail!!\n");
				return MESSAGE_BREAK;
			}
			if (pstContents->ulActionId != ulActionId)
			{
				HxLOG_Print("ulActionId fail!!\n");
				return MESSAGE_BREAK;
			}

			lResult = xmgr_emmupdate_StartSvcChannelService();
			if (lResult != ERR_OK)
			{
				HxLOG_Print("xmgr_emmupdate_StartSvcChannelService fail!!\n");
				return MESSAGE_BREAK;
			}
			break;

		case eSEVT_CH_NO_SIGNAL:
			HxLOG_Print("eSEVT_CH_NO_SIGNAL !!\n");
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Print("eMEVT_BUS_DESTROY !!\n");
			lResult = xmgr_emmupdate_DestroyApp();
			if (lResult != ERR_OK)
			{
				HxLOG_Print("xmgr_emmupdate_DestroyApp fail!!\n");
				break;
			}
			//MGR_ACTION_QuitStandby(eWAKEUP_BY_TIMER);
			break;
	}

	return BUS_ProcessMessageDefault(lMessage, hAction, lParam1, lParam2, lParam3);
}

BUS_Callback_t xmgr_emmupdate_GetProc_Base(void)
{
	return proc_emmupdate_Action_Base;
}

HERROR	xmgr_emmupdate_Start_Base(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Create((char*)"proc_emmupdate_Action", APP_ACTION_PRIORITY, xmgr_emmupdate_GetProc_Base(), HANDLE_NULL, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_emmupdate_Stop_Base(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Destroy(xmgr_emmupdate_GetProc_Base());

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_emmupdate_GetConfiguration_Base (HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration)
{
	#if defined(CONFIG_DEBUG)
	if (pulWakeupInterval)		*pulWakeupInterval		= (SECONDS_PER_MIN * 10);
	if (pulStartMin)			*pulStartMin			= 0; // 당장 시작의 의미.
	if (pulUpdateDuration)		*pulUpdateDuration		= (SECONDS_PER_MIN * 5);
	#else
	if (pulWakeupInterval)		*pulWakeupInterval		= (SECONDS_PER_HOUR * 4);
	if (pulStartMin)			*pulStartMin			= 0; // 당장 시작의 의미.
	if (pulUpdateDuration)		*pulUpdateDuration		= (SECONDS_PER_MIN * 60);
	#endif
	return ERR_OK;
}


HERROR xmgr_emmupdate_BatchDeleteExistSchedule(void)
{
	HUINT32			ulCnt, ulNumSch = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;

	(void)SAPI_CountSchedule(&ulNumSch);
	if (0 == ulNumSch)
	{
		HxLOG_Error("No Schedule: Nothing to stop\n");
		return ERR_OK;
	}

	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("Memory allocation failed\n");
		goto END_FUNC;
	}

	hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);
	if(hErr == ERR_OK)
	{
		for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
		{
			DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);

			if(pstSchedule->eRsvType == DxRSVTYPE_EMM)
			{
				HxLOG_Error("cancel existing Emm Wakeup Schedule (slot:%d) !!!\n", pstSchedule->ulSlot);
				SAPI_CancelSchedule(pstSchedule->ulSlot);
			}
		}
	}

	hErr = ERR_OK;

END_FUNC:

	if (pstSchArray)
	{
		HLIB_STD_MemFree(pstSchArray);
	}

	return hErr;
}

HERROR xmgr_emmupdate_BatchMakeSchedule (HxDATETIME_t *pstStartTime)
{
	DxSchedule_t		stSchedule = {0, };
	HERROR			hErr = ERR_FAIL;
	Handle_t 			hSvc = HANDLE_NULL;
	SAPI_Conflict_t	stConflict;

	if(pstStartTime==NULL)
	{
		HxLOG_Error(" Can not found Time to reserve.\n");
		return ERR_FAIL;
	}

	VK_memset(&stSchedule, 0, sizeof(DxSchedule_t) );
	HxSTD_MemCopy(&stSchedule.stStartTime, pstStartTime, sizeof(HxDATETIME_t));

	hErr = SAPI_GetAvailableSlot(&stSchedule.ulSlot);
	if (hErr != ERR_OK)
	{
		HxLOG_Error(" There is no empty slot for sama!\n");
		return ERR_FAIL;
	}
	HxLOG_Debug("Available Slot ... (%d)\n", stSchedule.ulSlot);

	stSchedule.eRsvType = DxRSVTYPE_EMM;

	stSchedule.ulSvcUid			= DB_SVC_GetServiceUID(hSvc);		// 깨어날때 처리하자.
	stSchedule.eRsvReady		= DxRSVREADY_30_SEC;
	stSchedule.ulDuration		= 0;
	stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
	stSchedule.ulRepeatDays 	= 0;
	stSchedule.eStatus			= DxRSVSTATUS_WAITING;
	stSchedule.bExceptConflict	= FALSE;
	stSchedule.eEpgType 		= eDxEPG_TYPE_NONE;

	HxLOG_Debug("on Solt: %d,  SVC UID: 0x%x\n", stSchedule.ulSlot, stSchedule.ulSvcUid);

	hErr = SAPI_MakeSchedule (stSchedule.ulSlot, &stSchedule, &stConflict);
	
	return ERR_OK;
}

HERROR	xmgr_emmupdate_GetBatchCBList_Base(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	fnBatchProcCBList->fnAppInit = NULL;
	fnBatchProcCBList->fnAppDeinit = NULL;
	fnBatchProcCBList->fnAppReSchedule = MGR_EMMUpdate_RescheduleBatch;

	return ERR_OK;
}


HERROR	xmgr_emmupdate_RescheduleBatch_Base(void)
{
	HERROR			hError = ERR_FAIL;
	HUINT32 		ulStartTime = 0;
	HxDATETIME_t		stStartTime;
	HUINT32		ulInterval = 0;

	HxLOG_Trace();

	ulStartTime = HLIB_STD_GetSystemTime();

	hError = MGR_EMMUpdate_GetConfiguration(&ulInterval, NULL, NULL);
	if(hError == ERR_FAIL)
	{
		HxLOG_Error("pmgr_emmupdate_GetConfiguration() fail! \n");
		return ERR_FAIL;
	}

	ulStartTime += ulInterval;

	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &stStartTime) == ERR_FAIL)
	{
		HxLOG_Error(" Fail to Convert Current Time.\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Error("stStartTime(%d) : %d-%d-%d %d:%d:%d \n", ulStartTime,
				stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay,
				stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond);
#endif

#if defined (CONFIG_OP_SES)
	// cancel existing emm schedule
	MGR_EMMUpdate_BatchDeleteExistSchedule();

	// make reservation for Emm Wakeup
	MGR_EMMUpdate_BatchMakeSchedule(&stStartTime);
#else
	// cancel existing emm schedule
	xmgr_emmupdate_BatchDeleteExistSchedule();

	// make reservation for Emm Wakeup
	xmgr_emmupdate_BatchMakeSchedule(&stStartTime);
#endif
	return ERR_OK;
}

/*********************** End of File ******************************/

