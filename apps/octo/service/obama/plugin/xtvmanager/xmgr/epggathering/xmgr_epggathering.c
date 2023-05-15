/**
	@file     xmgr_epggathering.c
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
#include <svc_epg.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_rscmap.h>

#include <mgr_epggathering.h>

#include <otl.h>

#include "./local_include/_xmgr_epggathering.h"
#include <_mgr_epggathering.h>
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
STATIC INLINE HERROR xmgr_epggathering_increaseCurrentTsIndex (EpgGathering_Content_t *pstContents)
{
	if(pstContents->nTsCnt == 0)
	{
		HxLOG_Error("pstContents->nTsCnt: %d \n", pstContents->nTsCnt);
		return ERR_FAIL;
	}
	pstContents->nCurrentTsIndex = (pstContents->nCurrentTsIndex + 1)%(pstContents->nTsCnt);
	return ERR_OK;
}

STATIC HERROR xmgr_epggathering_startEpg (EpgGathering_Content_t *pstContents)
{
	HERROR 				hErr = ERR_FAIL;
#if defined(CONFIG_MW_EPG_GATHERING_BARKER)
	DbSvc_TripleId_t 	stChannelInfo;
	HINT32 				nOrgDeliType = eDxDELIVERY_TYPE_ALL;
	HBOOL 				bValid = FALSE;
#endif

	HxLOG_Trace();

	if(pstContents->bStartedEpgGathering == TRUE)
	{
		HxLOG_Error("pstContents->bStartedEpgGathering is already TRUE\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_EPG_GATHERING_BARKER)
	// CONFIG_MW_EPG_GATHERING_BARKER를 사용하는 모델이라도 Barker를 dynamic 하게 사용할 수 있다
	hErr = SVC_SI_HomeChannelGetChannelInfo(&stChannelInfo, &nOrgDeliType, &bValid);
	if((ERR_OK == hErr) && (bValid == TRUE))
	{
		HxLOG_Print("Epg gathering by barker channel\n");
	}
	else
	{
		hErr = SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_PF_ALL, pstContents->hBGAction, pstContents->hSvcInCurrentTs, 0, 0, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_EPG_Start PF err: hAction(0x%08x), hSvc(0x%08x)\n", pstContents->hBGAction, pstContents->hSvcInCurrentTs);
			return ERR_FAIL;
		}
	}
#else
	hErr = SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_PF_ALL, pstContents->hBGAction, pstContents->hSvcInCurrentTs, 0, 0, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_EPG_Start PF err: hAction(0x%08x), hSvc(0x%08x)\n", pstContents->hBGAction, pstContents->hSvcInCurrentTs);
		return ERR_FAIL;
	}
#endif
	hErr = SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_SCH_ALL, pstContents->hBGAction, pstContents->hSvcInCurrentTs, 0, 0, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_EPG_Start SCH err: hAction(0x%08x), hSvc(0x%08x)\n", pstContents->hBGAction, pstContents->hSvcInCurrentTs);
		return ERR_FAIL;
	}

	pstContents->bStartedEpgGathering = TRUE;

	return ERR_OK;
}

STATIC HERROR xmgr_epggathering_stopEpg (EpgGathering_Content_t *pstContents)
{
	HERROR				hErr = ERR_FAIL;
#if defined(CONFIG_MW_EPG_GATHERING_BARKER)
	DbSvc_TripleId_t 	stChannelInfo;
	HINT32 				nOrgDeliType = eDxDELIVERY_TYPE_ALL;
	HBOOL 				bValid = FALSE;
#endif

	HxLOG_Trace();

	if(pstContents->bStartedEpgGathering == FALSE)
	{
		HxLOG_Debug("pstContents->bStartedEpgGathering is already FALSE\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_EPG_GATHERING_BARKER)
	// CONFIG_MW_EPG_GATHERING_BARKER를 사용하는 모델이라도 Barker를 dynamic 하게 사용할 수 있다
	hErr = SVC_SI_HomeChannelGetChannelInfo(&stChannelInfo, &nOrgDeliType, &bValid);
	if(ERR_OK == hErr)
	{
		HxLOG_Print("Epg gathering by barker channel\n");
	}
	else
	{
		hErr = SVC_EPG_Stop(eEPG_KIND_DVB, eEPG_EVT_KIND_PF_ALL, pstContents->hBGAction);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_EPG_Stop PF err: hAction(0x%08x)\n", pstContents->hBGAction);
			return ERR_FAIL;
		}
	}
#else
	hErr = SVC_EPG_Stop(eEPG_KIND_DVB, eEPG_EVT_KIND_PF_ALL, pstContents->hBGAction);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_EPG_Stop PF err: hAction(0x%08x)\n", pstContents->hBGAction);
		return ERR_FAIL;
	}
#endif
	hErr = SVC_EPG_Stop(eEPG_KIND_DVB, eEPG_EVT_KIND_SCH_ALL, pstContents->hBGAction);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_EPG_Stop SCH err: hAction(0x%08x)\n", pstContents->hBGAction);
		return ERR_FAIL;
	}
	pstContents->bStartedEpgGathering = FALSE;


	return ERR_OK;
}
#if defined(CONFIG_DEBUG)
STATIC void 	xmgr_epggathering_DebugPrintTuningParam(const DxTuneParam_t *pstTuningParam)
{
	if(NULL != pstTuningParam)
	{
		HxLOG_Print("\r\n========= TP Info ==========\r\n");

		HxLOG_HexIntDump(pstTuningParam->eDeliType);
		switch(pstTuningParam->eDeliType)
		{
		case eDxDELIVERY_TYPE_SAT:
			HxLOG_DecIntDump(pstTuningParam->sat.antennaType);
			HxLOG_DecIntDump(pstTuningParam->sat.ulRfInputId);
			HxLOG_DecIntDump(pstTuningParam->sat.ulTunerGroupId);

			HxLOG_DecIntDump(pstTuningParam->sat.antInfo.uid);
			HxLOG_StringDump(pstTuningParam->sat.antInfo.satType);
			HxLOG_StringDump(pstTuningParam->sat.antInfo.satName);
			HxLOG_DecIntDump(pstTuningParam->sat.antInfo.antType);
			switch(pstTuningParam->sat.antInfo.antType)
			{
			case eDxANT_TYPE_LNB_ONLY:
				HxLOG_Print("\tDxANT_TYPE_LNB_ONLY\r\n");
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.lnb.lnbVoltage);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.lnb.ulLnbFreq);
				break;

			case eDxANT_TYPE_DISEQC:
				HxLOG_Print("DxANT_TYPE_DISEQC\r\n");
				HxLOG_BoolDump(pstTuningParam->sat.antInfo.info.diseqc.b22kTone);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.diseqcInput);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.diseqcVersion);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.lnbVoltage);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.sSatLonggitude);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.toneBurst);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.ucMotorPosition);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.ulLnbFreq);
				break;

			case eDxANT_TYPE_SCD:
				HxLOG_Print("DxANT_TYPE_SCD\r\n");
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.scd.b22kTone);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.scd.lnbVoltage);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.scd.scdInput);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.scd.ulLnbFreq);
				break;
			default:
				break;
			}
			HxLOG_DecIntDump(pstTuningParam->sat.tuningInfo.ulFrequency);
			HxLOG_DecIntDump(pstTuningParam->sat.tuningInfo.ulSymbolRate);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_CodeRate_e), pstTuningParam->sat.tuningInfo.eFecCodeRate);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_Polarization_e), pstTuningParam->sat.tuningInfo.ePolarization);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_TransportSpec_e), pstTuningParam->sat.tuningInfo.eTransSpec);
			HxLOG_DecIntDump(pstTuningParam->sat.tuningInfo.antuid);
			HxLOG_StringDump(pstTuningParam->sat.tuningInfo.satType);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_RollOff_e), pstTuningParam->sat.tuningInfo.eRollOff);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_Pilot_e), pstTuningParam->sat.tuningInfo.ePilot);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_PskModulation_e), pstTuningParam->sat.tuningInfo.ePskMod);

			break;

		case eDxDELIVERY_TYPE_CAB:
			HxLOG_DecIntDump(pstTuningParam->cab.ulFrequency);
			HxLOG_DecIntDump(pstTuningParam->cab.ulSymbolRate);
			HxLOG_DecIntDump(pstTuningParam->cab.nTsId);
			HxLOG_DecIntDump(pstTuningParam->cab.nNetId);
			break;

		case eDxDELIVERY_TYPE_TER:
			HxLOG_DecIntDump(pstTuningParam->ter.ulFrequency);
			HxLOG_DecIntDump(pstTuningParam->ter.eBandWidth);
			HxLOG_DecIntDump(pstTuningParam->ter.deliveryType);
			HxLOG_DecIntDump(pstTuningParam->ter.ulTunerGroupId);
			HxLOG_DecIntDump(pstTuningParam->ter.eTransMode);
			break;

		default:
			break;
		}
		HxLOG_Print("======================================================\r\n");
	}
}

#endif

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

HERROR xmgr_epggathering_setupBackGroundAction (EpgGathering_Content_t *pstContents)
{
	HUINT32				 ulMainActionId = 0;
	Handle_t 			 hMainLiveSvc = HANDLE_NULL;
	DbSvc_TsInfo_t		 stMainLiveTsInfo = {0, };
//	MgrAction_Type_e	 eActionType = eActionType_NONE;
	HERROR				 hErr = ERR_FAIL;
	HINT32				 nPreSvcNum;
	DbSvc_Condition_t	 stSvcCond;
	Handle_t			*phPreSvcHandleArray = NULL;
	HINT32				 nCnt = 0;


	HxLOG_Trace();

	if(pstContents->bManageBgAction == TRUE)
	{
		HxLOG_Debug("bManageBgAction is %d, already manage the background action. \n", pstContents->bManageBgAction);
		return ERR_FAIL;
	}

	if(pstContents->nTsCnt == 0)
	{
		HxLOG_Debug("pstContents->nTsCnt is %d \n", pstContents->nTsCnt);
		return ERR_FAIL;
	}

	HxLOG_Debug("pstContents->nTsCnt(%d), pstContents->nCurrentTsIndex(%d) \n", pstContents->nTsCnt, pstContents->nCurrentTsIndex);
	hErr = xmgr_epggathering_increaseCurrentTsIndex(pstContents);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("fail xmgr_epggathering_increaseCurrentTsIndex() \n");
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


	// 현재 main live 의 ts 다음 것부터 하기 위해.
	// setup bg tune using live tuning param.
	ulMainActionId = MGR_ACTION_GetMainActionId();
	MGR_LIVE_GetServiceHandle(ulMainActionId, eLiveViewPlaySvc_ACTUAL, &hMainLiveSvc);
	hErr = DB_SVC_GetTsInfoByServiceHandle (hMainLiveSvc, &stMainLiveTsInfo);
	if (ERR_OK == hErr)
	{
		if(stMainLiveTsInfo.usUniqueId == pstContents->pstTsInfo[pstContents->nCurrentTsIndex].usUniqueId)
		{
			hErr = xmgr_epggathering_increaseCurrentTsIndex(pstContents);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("fail xmgr_epggathering_increaseCurrentTsIndex() \n");
				return ERR_FAIL;
			}
		}
	}
	else
	{
		HxLOG_Debug ("cannot get Ts Info from live. so no need to compare. just use pstContents->nCurrentTsIndex: %d \n", pstContents->nCurrentTsIndex);
	}

#if defined(CONFIG_MW_MM_REC_WITH_EPG)
	// TODO: rec 하고 있는 svc도 skip.
#endif


	while(1)
	{
		(void)OTL_CONV_DbTuningInfo2DxTuneParam(&(pstContents->pstTsInfo[pstContents->nCurrentTsIndex]), &(pstContents->stBgTuneInfo));
		pstContents->usUniqueId = pstContents->pstTsInfo[pstContents->nCurrentTsIndex].usUniqueId;

		hErr = MGR_ACTION_SetupBgTune(pstContents->ulBGActionId, &(pstContents->stBgTuneInfo), eAmPriority_BGJOB);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("background tune setup fail!try next tp.\n");
			hErr = xmgr_epggathering_increaseCurrentTsIndex(pstContents);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("fail xmgr_epggathering_increaseCurrentTsIndex() \n");
				return ERR_FAIL;
			}
		}
		else
		{
			HxLOG_Debug("setup bg tune OK! \n");
			break;
		}
		nCnt++;

		if(nCnt == pstContents->nTsCnt)
		{
			HxLOG_Error("background tune setup fail!\n");
			return ERR_FAIL;
		}

	}
	DB_SVC_InitSvcFindCondition (&stSvcCond);
	stSvcCond.nOrgDeliType = pstContents->pstTsInfo[pstContents->nCurrentTsIndex].eDeliType;
	stSvcCond.nTsIdx = pstContents->pstTsInfo[pstContents->nCurrentTsIndex].usTsIdx;

	if ( (DB_SVC_FindSvcHandleList(&stSvcCond, &nPreSvcNum, &phPreSvcHandleArray) != ERR_OK) || (nPreSvcNum <= 0) )
	{
		HxLOG_Info("error in DB_SVC_FindSvcHandleList...\n");
		if (phPreSvcHandleArray) {
			DB_SVC_FreeServiceList(phPreSvcHandleArray);
		}
		return ERR_FAIL;
	}
	pstContents->hSvcInCurrentTs = phPreSvcHandleArray[0];
	DB_SVC_FreeServiceList(phPreSvcHandleArray);

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

HERROR xmgr_epggathering_releaseBackGroundAction (EpgGathering_Content_t *pstContents)
{
	HERROR		hErr = ERR_OK;
	HERROR		hReturnErr = ERR_OK;

	if(pstContents->bManageBgAction == FALSE)
	{
		HxLOG_Debug("bManageBgAction is FALSE \n");
		return ERR_FAIL;
	}

	// stop epg
	hErr = xmgr_epggathering_stopEpg(pstContents);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_epggathering_stopEpg fail!!\n");
	}


	// stop tune
	hErr = SVC_CH_StopTune(pstContents->hBGAction, -1, eSyncMode);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("StopTune fail!!\n");
	}
	hReturnErr |= hErr;


	// release action이 sync call 로 event 발생시키므로..
	pstContents->bManageBgAction = FALSE;

	// release action
	hErr = MGR_ACTION_Release(pstContents->ulBGActionId, eActionType_BG_TUNE);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("release action fail!!\n");
	}
	hReturnErr |= hErr;


	HxLOG_Debug("(%s) return %d \n", __FUNCTION__, hReturnErr);
	return hReturnErr;
}

HERROR xmgr_epggathering_startTune (EpgGathering_Content_t *pstContents)
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

	// version up
	pstContents->hBGAction = SVC_PIPE_IncreaseActionVersion(pstContents->ulBGActionId);

#if defined(CONFIG_DEBUG)
	xmgr_epggathering_DebugPrintTuningParam(&(pstContents->stBgTuneInfo));
#endif

	hErr = SVC_CH_StartTune (pstContents->hBGAction, &stRscInfo, pstContents->usUniqueId, &(pstContents->stBgTuneInfo));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_CH_StartTune fail!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}






HERROR xmgr_epggathering_checkActionIdAndType (EpgGathering_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulActionId = SVC_PIPE_GetActionId (hAction);
	MgrAction_Type_e		 eActType = eActionType_NONE;

	HxLOG_Trace();

	eActType = MGR_ACTION_GetType(ulActionId);

	switch (eActType)
	{
	case eActionType_BG_TUNE:
		if (pstContents->ulBGActionId != ulActionId)
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


HERROR xmgr_epggathering_evtChLocked (EpgGathering_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hErr = ERR_FAIL;

	HxLOG_Trace();

	// start epg
	hErr = xmgr_epggathering_startEpg(pstContents);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_epggathering_startEpg fail!!\n");
	}


	return ERR_OK;
}

HERROR xmgr_epggathering_evtChUnLocked (EpgGathering_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hErr = ERR_FAIL;

	HxLOG_Trace();

	// stop epg
	hErr = xmgr_epggathering_stopEpg(pstContents);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_epggathering_startEpg fail!!\n");
	}


	return ERR_OK;
}

/*********************** End of File ******************************/

