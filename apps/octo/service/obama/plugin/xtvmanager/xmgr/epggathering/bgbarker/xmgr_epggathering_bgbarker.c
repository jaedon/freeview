/**
	@file     xmgr_epggathering_BgBarker.c
	@brief    file_name & simple comment.

	Description: \n
	Module: \n
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

#include <bus.h>


#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_search.h>
#include <mgr_common.h>

#include <svc_si.h>


#include "../local_include/_xmgr_epggathering.h"
#include <_mgr_epggathering.h>
#include <otl.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define EPG_GATHERING_TIMER_ID				0x2754
#define EPG_GATHERING_TIMEOUT				(10*1000)

#if defined(CONFIG_OP_TELENOR)
#define TEMP_CONFIG_RETUNE_BARKER_AT_CHANGED_SVC
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
	//1 향별 추가되는 filed는 여기에. 밑으로.


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
STATIC EpgGathering_Content_t	s_stEpgGatheringContent_BgBarker;
STATIC HBOOL					s_bColdBoot = TRUE;


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC BUS_Result_t	proc_epggathering_BgBarker(HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
STATIC HERROR		xmgr_EpgGathering_InitContext_BgBarker(EpgGathering_Content_t	 *pstContents);
INLINE STATIC EpgGathering_Content_t	 *xmgr_EpgGathering_GetContent_BgBarker (void);


#define	________________Static_Function_________________________

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

INLINE STATIC EpgGathering_Content_t	 *xmgr_EpgGathering_GetContent_BgBarker (void)
{
	return &s_stEpgGatheringContent_BgBarker;
}


STATIC HERROR	xmgr_EpgGathering_CheckNit_BgBarker(DbSvc_TripleId_t *pstChannelInfo, HINT32 *pnOrgDeliType)
{
	HBOOL bValid = FALSE;
	return SVC_SI_HomeChannelGetChannelInfo(pstChannelInfo, pnOrgDeliType, &bValid);
}

STATIC HERROR xmgr_EpgGathering_CheckHomeTp(EpgGathering_Content_t	 *pstContents)
{
	DbSvc_TsInfo_t		*pstTsInfo = NULL;
	DbSvc_TripleId_t 	 stChannelInfo;
	DbSvc_TsCondition_t	 stCond;
	HERROR				 bCheckHomeTP = ERR_FAIL;

	DB_SVC_InitTsFindCondition(&stCond);
	HxSTD_memset(&stChannelInfo, 0x00, sizeof(DbSvc_TripleId_t));
	
	if(!pstContents)
	{
		return ERR_FAIL;
	}

	xmgr_EpgGathering_CheckNit_BgBarker(&stChannelInfo, &stCond.nDeliType);

	stCond.nTsId = stChannelInfo.usTsId;
	stCond.nOnId = stChannelInfo.usOnId;
	
	if(DB_SVC_FindTsInfoList(&stCond, &(pstContents->nTsCnt), &(pstTsInfo)) != ERR_OK)
	{
		HxLOG_Error(" DB_SVC_FindTsInfoList Error\n");
	}
	HxLOG_Print(" New onid[%d] tsid[%d]\n",stCond.nOnId, stCond.nTsId);

	if(pstContents->nTsCnt > 0)
	{
		if(pstContents->pstTsInfo)
		{
			HxLOG_Print("BARKER Desc ORG onid[%d] tsid[%d]", pstContents->pstTsInfo->usOnId, pstContents->pstTsInfo->usTsId);
		}

		if(pstContents->pstTsInfo == NULL || (memcmp(pstContents->pstTsInfo, pstTsInfo, sizeof(DbSvc_TsInfo_t)) != 0))
		{
			bCheckHomeTP = ERR_OK;
		}
#if defined(TEMP_CONFIG_RETUNE_BARKER_AT_CHANGED_SVC)
		else if(pstContents->pstTsInfo != NULL 	&& (memcmp(pstContents->pstTsInfo, pstTsInfo, sizeof(DbSvc_TsInfo_t)) == 0))
		{
			bCheckHomeTP = ERR_OK;
		}
#endif
	}
	else
	{
		HxLOG_Print("There is No Ts about HomeChannel\n");
	}

	if(bCheckHomeTP == ERR_OK)
	{
		if(pstContents->pstTsInfo)		
		{
			DB_SVC_FreeTsInfoList(pstContents->pstTsInfo);
		}
		
		pstContents->pstTsInfo = pstTsInfo;
	}
	else
	{
		if(pstTsInfo != NULL)
		{
			OxDB_Free(pstTsInfo);
		}
	}
	
	return bCheckHomeTP;
}


STATIC HERROR	xmgr_EpgGathering_InitContext_BgBarker(EpgGathering_Content_t	 *pstContents)
{
	HxLOG_Trace();

	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstContents, 0x00, sizeof(EpgGathering_Content_t));

	return ERR_OK;
}


HERROR xmgr_epggathering_BgBarker_BackGroundAction (EpgGathering_Content_t	 *pstContents)
{
	HERROR				 hErr = ERR_FAIL;
	HINT32				 nPreSvcNum;
	DbSvc_Condition_t	 stSvcCond;
	Handle_t			*phPreSvcHandleArray = NULL;


	HxLOG_Trace();
	// get bg action.
	if(pstContents == NULL || pstContents->pstTsInfo == NULL)
		return ERR_FAIL;

	hErr = MGR_ACTION_GetEmptyBgActionId (&(pstContents->ulBGActionId));

	if (ERR_OK != hErr)
	{
		HxLOG_Error("cannot get backgroud action!\n");
		return ERR_FAIL;
	}

	pstContents->hBGAction  = SVC_PIPE_GetActionHandle(pstContents->ulBGActionId);
	HxLOG_Debug("pstContents->hBGAction: 0x%x, pstContents->ulBGActionId: 0x%x \n", pstContents->hBGAction, pstContents->ulBGActionId);
	OTL_CONV_DbTuningInfo2DxTuneParam(pstContents->pstTsInfo, &(pstContents->stBgTuneInfo));

	pstContents->usUniqueId = pstContents->pstTsInfo->usUniqueId;

	hErr = MGR_ACTION_SetupBgTune(pstContents->ulBGActionId, &(pstContents->stBgTuneInfo), eAmPriority_BGJOB);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("background tune setup fail!.\n");
		return ERR_FAIL;
	}

	DB_SVC_InitSvcFindCondition (&stSvcCond);
	stSvcCond.nOrgDeliType = pstContents->pstTsInfo->eDeliType;
	stSvcCond.nTsIdx = pstContents->pstTsInfo->usTsIdx;

	if ( (DB_SVC_FindSvcHandleList(&stSvcCond, &nPreSvcNum, &phPreSvcHandleArray) != ERR_OK) || (nPreSvcNum <= 0))
	{
		if (phPreSvcHandleArray) {
			DB_SVC_FreeServiceList(phPreSvcHandleArray);
		}
		HxLOG_Info("error in DB_SVC_FindSvcHandleList...\n");
		return ERR_FAIL;
	}
	pstContents->hSvcInCurrentTs = phPreSvcHandleArray[0];
	DB_SVC_FreeServiceList(phPreSvcHandleArray);

	// take action.
	hErr = MGR_ACTION_Take(pstContents->ulBGActionId, eActionType_BG_TUNE, BUS_MGR_GetMgrCallback (BUS_MGR_GetThis()));

	pstContents->bManageBgAction = TRUE;

	if(hErr != ERR_OK)
	{
		HxLOG_Error("take action fail!!\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("(%s) return ERR_OK! \n", __FUNCTION__);
	return ERR_OK;
}


STATIC HERROR	xmgr_EpgGathering_DeinitContext_BgBarker(EpgGathering_Content_t	 *pstContents)
{
	HxLOG_Trace();
	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}
	if(pstContents->pstTsInfo)		DB_SVC_FreeTsInfoList(pstContents->pstTsInfo);

	HxSTD_memset(pstContents, 0x00, sizeof(EpgGathering_Content_t));
	return ERR_OK;;
}


STATIC HERROR	xmgr_EpgGathering_ReChecking_BgBarker(EpgGathering_Content_t	 *pstContents)
{
	HERROR	hErr = ERR_FAIL;
	
	if(xmgr_EpgGathering_CheckHomeTp(pstContents) == ERR_OK)
	{
		hErr = xmgr_epggathering_releaseBackGroundAction((EpgGathering_Content_t *)pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("xmgr_epggathering_releaseBackGroundAction failed. \n");
		}

		if(xmgr_epggathering_BgBarker_BackGroundAction(pstContents) == ERR_OK)
		{
			BUS_KillTimer(EPG_GATHERING_TIMER_ID);
			hErr = xmgr_epggathering_startTune(pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("xmgr_epggathering_startTune failed. \n");
			}
		}
		else
		{
			BUS_MGR_DestroyWithReason(NULL, eDESTROY_BUS_ACTION, 0);
		}
	}
	
	return ERR_OK;
}

STATIC HERROR	xmgr_EpgGathering_LiveNotifySvcChanged(EpgGathering_Content_t *pstContents)
{
	HBOOL	bReChecking_BgBarker = FALSE;
	
#if defined(TEMP_CONFIG_RETUNE_BARKER_AT_CHANGED_SVC)
	bReChecking_BgBarker = TRUE;
#else
	bReChecking_BgBarker = s_bColdBoot;
#endif

	if(bReChecking_BgBarker == TRUE)
	{
		xmgr_EpgGathering_ReChecking_BgBarker(pstContents);
	}

	if(s_bColdBoot == TRUE)
	{
		s_bColdBoot = FALSE;
	}
	
	return ERR_OK;
}

STATIC BUS_Result_t	proc_epggathering_BgBarker(HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	EpgGathering_Content_t			*pstContents = xmgr_EpgGathering_GetContent_BgBarker();
	HERROR							hErr = ERR_FAIL;

	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("eMEVT_BUS_CREATE\n");
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			hErr = xmgr_EpgGathering_InitContext_BgBarker(pstContents);
			if(s_bColdBoot == FALSE)
			{
				xmgr_EpgGathering_ReChecking_BgBarker(pstContents);
			}

			HxLOG_Print("xmgr_epggathering_BgBarker_BackGroundAction() s_bColdBoot: %d \n", s_bColdBoot);
			break;
			
		case eMEVT_LIVE_NOTIFY_SVC_CHANGED :
			HxLOG_Print("eMEVT_LIVE_NOTIFY_SVC_CHANGED[%d]\n", s_bColdBoot);
			xmgr_EpgGathering_LiveNotifySvcChanged(pstContents);
			break;

		case eSEVT_DB_PARAM_UPDATED:
			if(p1 == eDB_PARAM_ITEM_HOME_CHANNEL)
			{
				HxLOG_Print ("eSEVT_DB_PARAM_UPDATED[%d] (%d,%d,%d)\n",eDB_PARAM_ITEM_HOME_CHANNEL, p1, p2, p3);
				xmgr_EpgGathering_ReChecking_BgBarker(pstContents);

			}
			break;

		case eMEVT_SEARCH_SAVE_FINISHED :
		case eSEVT_DB_CHECK_SVC_DB :
			HxLOG_Print ("eSEVT_DB_CHECK_SVC_DB (%d,%d,%d)\n", p1, p2, p3);
			xmgr_EpgGathering_ReChecking_BgBarker(pstContents);
			break;

		case eMEVT_BUS_TIMER :
			if(p1 == EPG_GATHERING_TIMER_ID)
			{
				HxLOG_Print("eMEVT_BUS_TIMER !!\n");
				hErr = xmgr_epggathering_startTune(pstContents);
				BUS_KillTimer(EPG_GATHERING_TIMER_ID);
			}
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Print("eMEVT_BUS_DESTROY !!\n");
			hErr = xmgr_epggathering_releaseBackGroundAction((EpgGathering_Content_t *)pstContents);
			xmgr_EpgGathering_DeinitContext_BgBarker(pstContents);
			HxLOG_Print("xmgr_epggathering_releaseBackGroundAction() retrun hErr: %d \n", hErr);
			break;

		// channel event.
		case eSEVT_CH_LOCKED :
			hErr = xmgr_epggathering_checkActionIdAndType(pstContents, hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				HxLOG_Print("eSEVT_CH_LOCKED !!, hAction: 0x%x\n", hAction);
				HxLOG_Print("pstContents->hBGAction: 0x%x, pstContents->ulBGActionId: 0x%x \n", pstContents->hBGAction, pstContents->ulBGActionId);
				hErr = xmgr_epggathering_evtChLocked (pstContents, hAction, p1, p2, p3);
			}
			break;

		case eSEVT_CH_NO_SIGNAL:
			hErr = xmgr_epggathering_checkActionIdAndType(pstContents, hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				// SetTimer for retry
				HxLOG_Print("eSEVT_CH_NO_SIGNAL !!, hAction: 0x%x\n", hAction);
				HxLOG_Print("pstContents->hBGAction: 0x%x, pstContents->ulBGActionId: 0x%x \n", pstContents->hBGAction, pstContents->ulBGActionId);
				hErr = SVC_CH_StopTune(pstContents->hBGAction, -1, eSyncMode);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("StopTune fail!!\n");
				}

				xmgr_epggathering_evtChUnLocked(pstContents, hAction, p1, p2, p3);
				BUS_SetTimer(EPG_GATHERING_TIMER_ID, EPG_GATHERING_TIMEOUT);
			}
			break;
	}

	return BUS_ProcessMessageDefault(lMessage, hAction, p1, p2, p3);
}

#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

HERROR	xmgr_epggathering_Start_BgBarker(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Create((char*)"proc_epggathering_BgBarker", APP_ACTION_PRIORITY, proc_epggathering_BgBarker, HANDLE_NULL, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_epggathering_Stop_BgBarker(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Destroy(proc_epggathering_BgBarker);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

// for batch
BUS_Callback_t	xmgr_epggathering_GetProc_BgBarker(void)
{
	return proc_epggathering_BgBarker;
}


#ifdef CONFIG_DEBUG
int xmgr_epggathering_PrintContents(void)
{

	EpgGathering_Content_t			*pstContents = xmgr_EpgGathering_GetContent_BgBarker();

	if(pstContents == NULL)
		HLIB_CMD_Printf("No Barker Contents\n");

	HLIB_CMD_Printf("\t bManageBgAction(%d), hBGAction(%x), ActionId(%d) bStartedEpgGathering(%d)\n", pstContents->bManageBgAction,pstContents->hBGAction,  pstContents->ulBGActionId, pstContents->bStartedEpgGathering);
	if(pstContents->nTsCnt > 0)
	{
		HLIB_CMD_Printf("\t tsCnt(%d), tsIndex(%d), Onid(%x), TsId(%x) NetId(%x)\n"
						"\t Freq(%d), Symb(%d)\n", pstContents->nTsCnt
			, pstContents->pstTsInfo->usTsIdx, pstContents->pstTsInfo->usOnId, pstContents->pstTsInfo->usTsId, pstContents->pstTsInfo->usNetIdx
			, pstContents->pstTsInfo->stTuningParam.cab.ulFreq, pstContents->pstTsInfo->stTuningParam.cab.usSR);
	}

	return 0;
}
#endif


void CMD_Register_Xmgr_Barker(void)
{
#define	hCmdHandle		"mgr_action"

#ifdef CONFIG_DEBUG
	HLIB_CMD_RegisterWord(hCmdHandle,(void*)xmgr_epggathering_PrintContents,
			"print_barker",	/* keyword */
			"barker informations", /*	 help  */
			"print_barker");	/*	usage  */
#endif
}


/*********************** End of File ******************************/
/*******************************************************************/
