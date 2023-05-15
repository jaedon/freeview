/**
	@file     ap_na_emmupdate_mgr.c
	@brief    file_name & simple comment.

	Description: for portal UI \n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2010/04/12		modified					Moonsg\n

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

#include "local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas.h"

#include <svc_resources.h>
#include <svc_ch.h>
#include <svc_si.h>
#include <svc_epg.h>
#include <svc_cas.h>
#include <svc_resources.h>

#include <db_svc.h>
#include <mgr_cas.h>
#include <mgr_rscmap.h>
#include <mgr_emmupdate.h>

#include <namgr_main.h>

#include "local_include/_xmgr_cas_na_util.h"
#include <sapi.h>

#if defined (CONFIG_OP_SES)
#include <otl.h>
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define NA_EMMUPDATE_TIMER_TUNE_ID			(2)

#define NA_EMMUPDATE_TIMER_TUNE_DUR			(4 * 1000)	// 4sec

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagEMMUpdateContent
{
	Handle_t			hAction;
	HUINT32			ulActionId;

	HUINT32			ulScheduleTimerId;
	Handle_t			hHomeSvcHandle;

	DxTuneParam_t	stTuningInfo;
} XmgrCas_NaEmmUpdateContent_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_NaEmmUpdateContent_t	s_stEMMUpdateContent;

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC BUS_Result_t		xmgr_cas_NaEmmUpdateGetContent(XmgrCas_NaEmmUpdateContent_t **ppContent);
STATIC INLINE HERROR	xmgr_cas_NaEmmUpdateCheckActionId(XmgrCas_NaEmmUpdateContent_t *pstContents, Handle_t hAction);
STATIC BUS_Result_t		xmgr_cas_NaEmmUpdateInitApp(void);
STATIC BUS_Result_t		xmgr_cas_NaEmmUpdateStartSvcChannelService(void);
STATIC BUS_Result_t		xmgr_cas_NaEmmUpdateTuneSvcChannel(void);
STATIC BUS_Result_t		xmgr_cas_NaEmmUpdateDestroyApp(void);
STATIC BUS_Result_t 	xmgr_cas_NaEmmUpdateGetNextWaking(void);

#define	________________Static_Function_________________________

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

STATIC BUS_Result_t xmgr_cas_NaEmmUpdateGetContent(XmgrCas_NaEmmUpdateContent_t **ppContent)
{
	BUS_Result_t lResult = ERR_FAIL;

	if (ppContent == NULL)
	{
		HxLOG_Print("Param fail...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	*ppContent = &s_stEMMUpdateContent;
	lResult = ERR_BUS_NO_ERROR;

EXIT_EMMUPDATE_FUNC :
	return lResult;
}


STATIC INLINE HERROR xmgr_cas_NaEmmUpdateCheckActionId (XmgrCas_NaEmmUpdateContent_t *pstContents, Handle_t hAction)
{
	HUINT32	ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);

	if (pstContents->ulActionId != ulActionId)
	{
		HxLOG_Info("Is not matched actionID(0x%x)\n", ulActionId);
		return MESSAGE_PASS;
	}
	return ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t xmgr_cas_NaEmmUpdateInitApp(void)
{
	HERROR 		hError;
	BUS_Result_t lResult = ERR_FAIL;

	if (xmgr_cas_NaIrdCmdEmmInitWakingTable() == ERR_OK)
	{
		HUINT8 bNextJob = FALSE;

		hError = xmgr_cas_NaIrdCmdEmmHasMoreNextWaking(&bNextJob);
		if ( (hError != ERR_OK) || (bNextJob == FALSE) )
		{
			HxLOG_Info("error in MW_CA_NA_IRDCMD_EMM_HasMoreNextWaking...(hError:0x%x, NextJob:%d)\n", hError, bNextJob);
			goto EXIT_EMMUPDATE_FUNC;
		}

		hError = xmgr_cas_NaEmmUpdateGetNextWaking();
		if (hError != ERR_OK)
		{
			HxLOG_Info("error in xmgr_cas_NaEmmUpdateGetNextWaking...\n");
			goto EXIT_EMMUPDATE_FUNC;
		}
	}
	else
	{
		HxLOG_Info("error in MW_CA_NA_IRDCMD_EMM_InitWakingTable...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}
	lResult = ERR_BUS_NO_ERROR;

EXIT_EMMUPDATE_FUNC :
	return lResult;
}


STATIC BUS_Result_t xmgr_cas_NaEmmUpdateStartSvcChannelService(void)
{
	HERROR				hError = ERR_FAIL;
	BUS_Result_t			lResult = ERR_FAIL;
	XmgrCas_NaEmmUpdateContent_t	*pContent = NULL;

	lResult = xmgr_cas_NaEmmUpdateGetContent(&pContent);
	if (lResult != ERR_OK)
	{
		HxLOG_Print("error in local_EMMUpdate_GetContent...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	lResult = ERR_FAIL;
	hError = MGR_CAS_StartService(pContent->hAction, pContent->hHomeSvcHandle, eCAS_ActionType_Live, eDxCAS_GROUPID_NA, NULL);
	if (hError != ERR_OK)
	{
		HxLOG_Print("error in MGR_CAS_StartService...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}
	lResult = ERR_BUS_NO_ERROR;

EXIT_EMMUPDATE_FUNC :
	return lResult;
}


STATIC BUS_Result_t xmgr_cas_NaEmmUpdateTuneSvcChannel (void)
{
	HERROR				hError = ERR_FAIL;
	BUS_Result_t			lResult = ERR_FAIL;
	XmgrCas_NaEmmUpdateContent_t	*pContent = NULL;
	SvcRsc_Info_t			 stRscInfo;

	lResult = xmgr_cas_NaEmmUpdateGetContent(&pContent);
	if (lResult != ERR_OK)
	{
		HxLOG_Print("error in local_EMMUpdate_GetContent...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	lResult = ERR_FAIL;
	hError = SVC_CH_StopTune (SVC_PIPE_GetActionHandle(pContent->ulActionId), (HUINT16)-1, eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Print("error in SVC_CH_StopTune...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	// version up~
	pContent->hAction = SVC_PIPE_IncreaseActionVersion(pContent->ulActionId);

	MGR_RSC_GetResourceInfo (pContent->ulActionId, &stRscInfo);
	hError = SVC_CH_StartTune(pContent->hAction, &stRscInfo, (HUINT16)-1, &pContent->stTuningInfo);
	if (hError != ERR_OK)
	{
		HxLOG_Print("error in SVC_CH_StartTune...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	BUS_SetTimer(NA_EMMUPDATE_TIMER_TUNE_ID, NA_EMMUPDATE_TIMER_TUNE_DUR);
	lResult = ERR_BUS_NO_ERROR;

EXIT_EMMUPDATE_FUNC :
	return lResult;
}


STATIC BUS_Result_t xmgr_cas_NaEmmUpdateDestroyApp(void)
{
	HERROR				hError = ERR_FAIL;
	BUS_Result_t			lResult = ERR_FAIL;
	XmgrCas_NaEmmUpdateContent_t	*pContent = NULL;

	lResult = xmgr_cas_NaEmmUpdateGetContent(&pContent);
	if (lResult != ERR_OK)
	{
		HxLOG_Print("error in local_EMMUpdate_GetContent ...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	lResult = ERR_FAIL;
	hError = SVC_CH_StopTune(pContent->hAction, (HUINT16)-1, eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Print("error in SVC_CH_StopTune...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = MGR_CAS_StopService(pContent->hAction, HANDLE_NULL, eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Print("error in MGR_CAS_StopService...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = MGR_ACTION_Release(pContent->ulActionId, eActionType_CAS_TUNE);
	if (hError != ERR_OK)
	{
		HxLOG_Print("error in MGR_ACTION_Release...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

#if defined(CONFIG_APCORE_INFO_UPDATE_WAKEUP)
	BUS_PostMessage (NULL, eMEVT_EMMUPDATE_DONE, 0, 0, 0, 0);
#endif
	lResult = ERR_BUS_NO_ERROR;

EXIT_EMMUPDATE_FUNC :
	return lResult;
}


STATIC BUS_Result_t xmgr_cas_NaEmmUpdateGetNextWaking(void)
{
	HERROR				hError = ERR_FAIL;
	HUINT32 				ulActionId = 0;
	DbSvc_TsInfo_t			stTsInfo;
	BUS_Result_t			lResult = ERR_FAIL;
	DxTuneParam_t 		stTuningInfo;
	XmgrCas_NaEmmUpdateContent_t *pContent = NULL;

	HUINT8 				ucNetIdFlag, ucTsIdFlag, ucSvcIdFlag;
	HUINT16             		usNetId, usTsId, usSvcId;

	DxDeliveryType_e 		eOrgDeliType;
	HUINT16				nNetIdx;

	HINT32				i;
	HINT32				nPreTsNum;
	DbSvc_TsCondition_t		stTsCond;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	HINT32				nPreSvcNum;
	DbSvc_Condition_t 		stSvcCond;
	Handle_t				hSvc = HANDLE_NULL;
	Handle_t				*phPreSvcHandleArray = NULL;
	HBOOL				bActionFail = FALSE;

#if defined(CONFIG_MW_CH_SATELLITE)
	eOrgDeliType = eDxDELIVERY_TYPE_SAT;
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
	eOrgDeliType = eDxDELIVERY_TYPE_TER;
#elif defined(CONFIG_MW_CH_CABLE)
	eOrgDeliType = eDxDELIVERY_TYPE_CAB;
#else
	eOrgDeliType = eDxDELIVERY_TYPE_ALL;
#endif

	// 1. Init Content
	lResult = xmgr_cas_NaEmmUpdateGetContent(&pContent);
	if (lResult != ERR_OK)
	{
		HxLOG_Info("error in local_EMMUpdate_GetContent...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	lResult = ERR_FAIL;
	// 2. Get Waking Tune Info.
	hError = xmgr_cas_NaIrdCmdEmmGetWakingTuneInfo(&ucNetIdFlag, &usNetId, &ucTsIdFlag, &usTsId, &ucSvcIdFlag, &usSvcId);
	if (hError != ERR_OK)
	{
		HxLOG_Info("error in MW_CA_NA_IRDCMD_EMM_GetWakingTuneInfo...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	if (ucNetIdFlag == 1)
	{
		if (DB_SVC_FindNetworkIndexById(eOrgDeliType, usNetId, &nNetIdx) != ERR_OK)
		{
			HxLOG_Info("error in DB_SVC_FindNetworkIndexById...\n");
			//goto EXIT_EMMUPDATE_FUNC;
			nNetIdx = -1;
		}

		DB_SVC_InitTsFindCondition(&stTsCond);
		stTsCond.nDeliType = eOrgDeliType;
		stTsCond.nNetIdx = nNetIdx;

		if (ucTsIdFlag == 1)
		{
			stTsCond.nTsId = usTsId;
		}

		if ( (DB_SVC_FindTsInfoList(&stTsCond, &nPreTsNum, &pstTsArray) != ERR_OK) || (nPreTsNum <= 0) )
		{
			HxLOG_Info("error in DB_SVC_FindTsInfoList...\n");
#if defined (CONFIG_OP_SES)
			DB_SVC_FreeTsInfoList(pstTsArray);
#endif
			goto EXIT_EMMUPDATE_FUNC;
		}

		for (i=0; i<nPreTsNum; i++)
		{
			DB_SVC_InitSvcFindCondition (&stSvcCond);
			stSvcCond.nOrgDeliType = eOrgDeliType;
			stSvcCond.nTsIdx = pstTsArray[i].usTsIdx;

			if (ucSvcIdFlag == 1)
			{
				stSvcCond.nSvcId = (HINT32)usSvcId;
			}

			if ( (DB_SVC_FindSvcHandleList(&stSvcCond, &nPreSvcNum, &phPreSvcHandleArray) != ERR_OK) || (nPreSvcNum <= 0) )
			{
				HxLOG_Info("error in DB_SVC_FindSvcHandleList...\n");
#if defined (CONFIG_OP_SES)
				DB_SVC_FreeServiceList(phPreSvcHandleArray);
#endif
				continue;
			}

			hSvc = phPreSvcHandleArray[0];
			DB_SVC_FreeServiceList(phPreSvcHandleArray);
			break;
		}
		DB_SVC_FreeTsInfoList(pstTsArray);
	}
	else
	{
		hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_CUR_SVC, (HUINT32 *)&hSvc, 0);	// lastsvc
		if (hError != ERR_OK)
		{
			HxLOG_Info("Not existing current service \n");
			goto EXIT_EMMUPDATE_FUNC;
		}
	}

	hError = DB_SVC_CheckValidServiceHandle(hSvc);
	if (hError == ERR_OK)
	{
		HxLOG_Info("Valid Service Handle(0x%x) \n", hSvc);
	}
	else
	{
		HxLOG_Info("Invalid Service Handle(0x%x) \n", hSvc);
		goto EXIT_EMMUPDATE_FUNC;
	}

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
	if (hError != ERR_OK)
	{
		HxLOG_Info("error in DB_SVC_GetTsInfoByServiceHandle...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stTuningInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Info("error in OTL_CONV_DbTuningInfo2DxTuneParam...\n");
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
		HxLOG_Info("error in MGR_ACTION_SetupCasTune...\n");
		bActionFail = TRUE;
		goto EXIT_EMMUPDATE_FUNC;
	}

	hError = MGR_ACTION_Take(pContent->ulActionId, eActionType_CAS_TUNE, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hError != ERR_OK)
	{
		HxLOG_Info("error in MGR_ACTION_Take...\n");
		bActionFail = TRUE;
		goto EXIT_EMMUPDATE_FUNC;
	}


/**************************************************************************************************
  *
  * Tune svc channel
  *
  *************************************************************************************************/
	hError = xmgr_cas_NaEmmUpdateTuneSvcChannel();
	if (hError != ERR_OK)
	{
		HxLOG_Info("error in xmgr_cas_NaEmmUpdateTuneSvcChannel...\n");
		goto EXIT_EMMUPDATE_FUNC;
	}

	lResult = ERR_BUS_NO_ERROR;

EXIT_EMMUPDATE_FUNC :
	HxLOG_Info("Result = 0x%x\n", lResult);

	if (lResult != ERR_BUS_NO_ERROR)
	{
		BUS_MGR_DestroyWithReason(NULL, bActionFail?eDESTROY_BUS_ACTION:eDESTROY_BUS_MYSELF, 0);
	}
	return lResult;
}


#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/
BUS_Result_t xproc_cas_NaEmmUpdate(HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	HUINT32				ulActionId = 0;
	BUS_Result_t		lResult = ERR_FAIL;
	XmgrCas_NaEmmUpdateContent_t	*pstContents = NULL;

	ulActionId = SVC_PIPE_GetActionId(hAction);
	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Info("eMEVT_BUS_CREATE...\n");
			lResult = xmgr_cas_NaEmmUpdateInitApp();
			if (lResult != ERR_OK)
			{
				HxLOG_Print("error in xmgr_cas_NaEmmUpdateInitApp...\n");
				break;
			}
			break;

		case eMEVT_BUS_TIMER :
			HxLOG_Info("eMEVT_BUS_TIMER... lParam1:%d\n", lParam1);
			if (lParam1 == NA_EMMUPDATE_TIMER_TUNE_ID)
			{
				BUS_KillTimer(NA_EMMUPDATE_TIMER_TUNE_ID);

				BUS_MGR_Destroy(xproc_cas_NaEmmUpdate);

			}
			break;

		case eMEVT_BUS_GO_OPERATION :
			HxLOG_Print("eMEVT_BUS_GO_OPERATION...\n");
			lResult = BUS_MGR_Destroy(xproc_cas_NaEmmUpdate);
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			HxLOG_Print("eMEVT_BUS_READY_SHUTDOWN...\n");
			return MESSAGE_PASS;

		case eSEVT_CH_LOCKED :
			HxLOG_Info("eSEVT_CH_LOCKED...\n");
			BUS_KillTimer(NA_EMMUPDATE_TIMER_TUNE_ID);

			lResult = xmgr_cas_NaEmmUpdateGetContent(&pstContents);
			if (lResult != ERR_OK)
			{
				HxLOG_Print("error on xmgr_cas_NaEmmUpdateGetContent...\n");
				return MESSAGE_BREAK;
			}

			if (pstContents->ulActionId != ulActionId)
			{
				HxLOG_Print("Incorrect action_id (0x%x:0x%x)\n", pstContents->ulActionId, ulActionId);
				return MESSAGE_BREAK;
			}

			lResult = xmgr_cas_NaEmmUpdateStartSvcChannelService();
			if (lResult != ERR_OK)
			{
				HxLOG_Print("error in xmgr_cas_NaEmmUpdateStartSvcChannelService...\n");
				return MESSAGE_BREAK;
			}
			break;

		case eSEVT_CH_NO_SIGNAL:
			HxLOG_Info("eSEVT_CH_NO_SIGNAL...\n");
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Info("eMEVT_BUS_DESTROY...\n");
			lResult = xmgr_cas_NaEmmUpdateDestroyApp();
			if (lResult != ERR_OK)
			{
				HxLOG_Print("error in xmgr_cas_NaEmmUpdateDestroyApp...\n");
				break;
			}
			break;
	}

	return BUS_ProcessMessageDefault(lMessage, hAction, lParam1, lParam2, lParam3);
}


HERROR	xmgr_cas_NaEmmUpdate_GetConfiguration(HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration)
{
	HERROR			hError = ERR_FAIL;
	UNIXTIME			utCompTime = 0;
	UNIXTIME			utDuration = 0;

	hError = xmgr_cas_NaIrdCmdEmmGetWakeupTime(&utCompTime);
	if (hError != ERR_OK || utCompTime == 0)
	{
		HxLOG_Error("Nothing Emm Wakeup Time !!! (hError=%d, utCompTime = %d)\n", hError, utCompTime);
		return ERR_FAIL;
	}
	HxLOG_Error("utCompTime = %d sec\n", utCompTime);

	hError = xmgr_cas_NaIrdCmdEmmGetWakingTimer(&utDuration);
	if (hError != ERR_OK || utDuration == 0)
	{
		HxLOG_Error("Nothing Emm Wakeup Time !!! (hError=%d, utDuration = %d)\n", hError, utDuration);
		return ERR_FAIL;
	}

	HxLOG_Error("utDuration = %d sec\n", utDuration);

	if (pulWakeupInterval)		*pulWakeupInterval		= (0);
	if (pulStartMin)		*pulStartMin		= (HUINT32)utCompTime;
	if (pulUpdateDuration)	*pulUpdateDuration	= (HUINT32)utDuration;

	return ERR_OK;

}


// local_action_SetupAutoEMMUpdateRsvTimeProcess() 에서 가져온 함수.
HERROR	xmgr_cas_NaEmmUpdateRescheduleBatch(void)
{
	BUS_Result_t		eRes = ERR_BUS_NO_ERROR;
	HERROR			hError = ERR_FAIL;
	UNIXTIME			utCompTime = 0;
	HxDATETIME_t		stCurtTime, stStartTime;
	UNIXTIME			ulLocalTime = 0;

	// 1. 현재시간 구하기...
	if ( VK_CLOCK_GetTime( (unsigned long *)&ulLocalTime ) != ERR_OK )
	{
		HxLOG_Error("error in VK_CLOCK_GetTime...\n");
		return ERR_FAIL;
	}
#if defined(CONFIG_DEBUG)
	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulLocalTime, &stCurtTime) == ERR_FAIL)
	{
		HxLOG_Error(" Fail to Convert Current Time.\n");
		return ERR_FAIL;
	}
	HxLOG_Error("Current Time : %d-%d-%d %d:%d:%d \n",
				stCurtTime.stDate.usYear, stCurtTime.stDate.ucMonth, stCurtTime.stDate.ucDay,
				stCurtTime.stTime.ucHour, stCurtTime.stTime.ucMinute, stCurtTime.stTime.ucSecond);
#endif

	hError = xmgr_cas_NaIrdCmdEmmGetWakeupTime(&utCompTime);
	if (hError != ERR_OK || utCompTime == 0)
	{
		HxLOG_Error("Nothing Emm Wakeup Time !!! (hError=%d, utCompTime = %d)\n", hError, utCompTime);
		return ERR_FAIL;
	}
	utCompTime = ulLocalTime + utCompTime;

	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(utCompTime, &stStartTime) == ERR_FAIL)
	{
		HxLOG_Error(" Fail to Convert Current Time.\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Error("stStartTime : %d-%d-%d %d:%d:%d \n",
				stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay,
				stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond);
#endif

	// cancel existing emm schedule
//	xmgr_emmupdate_BatchDeleteExistSchedule();
	MGR_EMMUpdate_BatchDeleteExistSchedule();

	// make reservation for Emm Wakeup
//	xmgr_emmupdate_BatchMakeSchedule(&stStartTime);
	MGR_EMMUpdate_BatchMakeSchedule(&stStartTime);

	return eRes;
}

#endif

/*********************** End of File ******************************/

