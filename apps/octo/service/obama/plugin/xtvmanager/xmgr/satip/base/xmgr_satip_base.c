#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_rscmap.h>
#include <mgr_satip.h>

#include <svc_ch.h>
#include <svc_rec.h>
#include <svc_transcoder.h>
#include <svc_sys.h>
#include <_xmgr_satip_base.h>

#include <otl.h>



#define CONFIG_TRANSCODER_FIXED_PCRPID	0x1FFE
#define _SECTION_INSERTION_PERIOD		300		/* ms */
#define _DUMP_SECTION_

#define CHECK_SATIP_ACTION_HANDLE()	\
	do {																	\
		HUINT32			 ulMsgActionId = SVC_PIPE_GetActionId (hAction);	\
		if (SVC_PIPE_GetActionId (pstContext->hAction) != ulMsgActionId)	\
		{																	\
			return MESSAGE_PASS;											\
		}																	\
	} while(0)

#define ___LOCAL_VARIABLES___
STATIC xMgrSatIpContext_t	s_astSatIpCtx[NUM_BG_ACTION];


#define ___LOCAL_FUNCTION___
STATIC xMgrSatIpContext_t* xmgr_satip_base_GetContext (HUINT32 ulIndex)
{
	if (ulIndex >= NUM_BG_ACTION)
		return NULL;

	return &s_astSatIpCtx[ulIndex];
}

STATIC MgrAction_Priority_e	xmgr_satip_base_GetActPriority (HUINT16 usPriority)
{
	MgrAction_Priority_e		 eActPriority = eAmPriority_RECORD;
	DxLiveStreamPriority_e		 eLsPriority = eDxLsPriority_EQUAL_REC;
	HERROR						 hErr = ERR_FAIL;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LIVESTREAM_PRIORITY, (HUINT32 *)&eLsPriority, 0);
	if (ERR_OK == hErr)
	{
		switch (eLsPriority)
		{
		case eDxLsPriority_EQUAL_REC:
			eActPriority = eAmPriority_RECORD;
			break;

		case eDxLsPriority_EQUAL_VIEW:
			eActPriority = eAmPriority_VIEW_ACTION;
			break;

		case eDxLsPriority_BELOW_VIEW:
			eActPriority = (eAmPriority_VIEW_ACTION - (1<<ACTION_PRIORITY_BIT_OFFSET));
			break;

		case eDxLsPriority_UPPER_REC:
			eActPriority = (eAmPriority_RECORD + (1<<ACTION_PRIORITY_BIT_OFFSET));
			break;

		default:
			break;
		}
	}

	if (usPriority == 0xFFFF)
	{
		// Current implementation depends on 0xFFFF as magic force priority.
		// If input of 0xFFFF priority is possible, then some needs to be changed.
		// option 1. use ACTION_PRIORITY_BIT_OFFSET of 17
		// option 2. allow kill same priority in RLIB
		HxLOG_Error("WARNING: 0xFFFF priority can't force setup against another 0xFFFF priority");
	}

	// use usPriority as lower part
	eActPriority &= ACTION_PRIORITY_UPPER_MASK;
	eActPriority += usPriority;

	return eActPriority;
}
STATIC void xmgr_satip_base_ClearContext (xMgrSatIpContext_t *pstContext)
{
	if (NULL == pstContext)
		return;

	HxSTD_MemSet (pstContext, 0, sizeof(xMgrSatIpContext_t));
	pstContext->hAction	=	HANDLE_NULL;
	pstContext->eState	=	eSatIpState_Idle;

	/*init PID*/
	{
		HINT32 i;
		for (i = 0; i < DxRSV_PIDTABLE_COUNT; i++)
		{
			pstContext->stSetup.usPIDs[i] = PID_NULL;
		}
	}

	return ;
}

STATIC HBOOL xmgr_satip_base_CheckValidPidList (HINT32 nPidsCount, HUINT16 *pusPids)
{
	if ( (nPidsCount <= 0) || (DxRSV_PIDTABLE_COUNT <= nPidsCount) )
	{
		HxLOG_Error ("Error. : nPidsCount is invaild (%d)\n", nPidsCount);
		return FALSE;
	}

	if (NULL == pusPids)
	{
		HxLOG_Error ("Error. : Invaild Arguments\n");
		return FALSE;
	}

	return TRUE;
}

STATIC HERROR xmgr_satip_base_stopTranscoder (xMgrSatIpContext_t *pstContext)
{
	HERROR			 hErr = ERR_FAIL;
#if defined(CONFIG_MW_MM_TRANSCODING)
	hErr = SVC_TRANSCODER_StopService (pstContext->hAction, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_TRANSCODER_StopService err: hAction(0x%08x)\n", pstContext->hAction);
		return ERR_FAIL;
	}
#endif
	return hErr;
}

STATIC HERROR xmgr_satip_base_SetupAction (HUINT32 ulActionId, MgrSatIp_Setup_t *pstSetup)
{
	MgrAction_Priority_e		 eActPriority = eAmPriority_RECORD;
	HERROR						 hErr;
	HBOOL						 bForce = FALSE;

	eActPriority = xmgr_satip_base_GetActPriority(pstSetup->usPriority);
	bForce = (pstSetup->eAllocMode == DxRsvStream_AllocMode_FORCE)? TRUE: FALSE;

	HxLOG_Debug("Setup Satip. force=(%d) \n",bForce);
	HxLOG_Debug("Setup Satip. bTranscode=(%d) \n",pstSetup->stStartArg.bTranscode);

	hErr = MGR_ACTION_SetupSatIp (ulActionId, &pstSetup->stTuneParam, eActPriority, bForce, pstSetup->stStartArg.bTranscode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("can't take MGR_ACTION_SetupSatIp() err=%d\n", hErr);
		return hErr;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_satip_base_RestartAction (HUINT32 ulActionId)
{
	xMgrSatIpContext_t	*pstContext = xmgr_satip_base_GetContext (ulActionId - eActionId_BG_FIRST);
	SvcRsc_Info_t		 stRscInfo;
	HERROR				 hErr;

	if (NULL == pstContext)
		return ERR_FAIL;

	/* Restart the recording or trasncoding ... */
	/* 1. Stop the recording or trasncoding		*/
	if (eSatIpState_Running <= pstContext->eState)
	{
		/* stop service */
		if (TRUE == pstContext->stSetup.stStartArg.bTranscode)
		{
			hErr = xmgr_satip_base_stopTranscoder(pstContext);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("xmgr_satip_base_stopTranscoder. Error!!!\n");
			}
		}
		else
		{
			hErr = SVC_REC_Stop (pstContext->hAction, TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_REC_Stop. Error!!!\n");
			}

			hErr = SVC_REC_Close(pstContext->hAction, TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_REC_Close. Error!!!\n");
			}
		}
	}

	hErr = SVC_CH_StopTune (pstContext->hAction, (HUINT16)-1, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_CH_StopTune. Error!!!\n");
	}

	pstContext->eState = eSatIpState_Idle;

	HxSTD_MemSet(&stRscInfo, 0, sizeof(SvcRsc_Info_t));

	hErr = MGR_RSC_GetResourceInfo(ulActionId, &stRscInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_RSC_GetResourceInfo. Error!!!\n");
		return hErr;
	}

	hErr = SVC_CH_StartTune(pstContext->hAction, &stRscInfo, -1, &(pstContext->stSetup.stTuneParam));
	if (ERR_OK != hErr)
	{
		BUS_SendMessage(NULL, eMEVT_SATIP_NOTIFY_START_FAILED, pstContext->hAction, eMgrSatIpFailed_CREATE, pstContext->stSetup.ulRsvSlotId, 0);
		HxLOG_Error("SVC_CH_StartTune. Error!!!\n");
		return hErr;
	}

	pstContext->eState = eSatIpState_ReqStart;

	return ERR_OK;
}

HERROR xmgr_satip_base_setupTranscoder(xMgrSatIpContext_t *pstContext)
{
	HERROR				hErr = ERR_FAIL;
#if defined(CONFIG_MW_MM_TRANSCODING)
	hErr = DB_SVC_CheckValidServiceHandle(pstContext->stSetup.stStartArg.hSvcHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("hSvc(0x%08x), DB_SVC_CheckValidServiceHandle err:\n", pstContext->stSetup.stStartArg.hSvcHandle);
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle(pstContext->stSetup.stStartArg.hSvcHandle, &pstContext->stSetup.stStartArg.stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_GetTsInfoByServiceHandle ret :0x%X\n",hErr);
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.stStartArg.hSvcHandle, &pstContext->stSetup.stStartArg.stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->stSetup.stStartArg.hSvcHandle);
		return ERR_FAIL;
	}

	pstContext->stSetup.stStartArg.stSrc.usVideoPid		= DbSvc_GetVideoPid(&pstContext->stSetup.stStartArg.stSvcInfo);
	pstContext->stSetup.stStartArg.stSrc.usAudioPid		= DbSvc_GetAudioPid(&pstContext->stSetup.stStartArg.stSvcInfo);
	pstContext->stSetup.stStartArg.stSrc.usPcrPid		= DbSvc_GetPcrPid(&pstContext->stSetup.stStartArg.stSvcInfo);
	pstContext->stSetup.stStartArg.stSrc.eVideoCodec	= DbSvc_GetVideoCodec(&pstContext->stSetup.stStartArg.stSvcInfo);
	pstContext->stSetup.stStartArg.stSrc.eAudioCodec	= DbSvc_GetAudioCodec(&pstContext->stSetup.stStartArg.stSvcInfo);

	pstContext->stSetup.stStartArg.stPsi.usTsUniqId 	= pstContext->stSetup.stStartArg.stTsInfo.usUniqueId;
	pstContext->stSetup.stStartArg.stPsi.usTsId 		= DbSvc_GetTsId(&pstContext->stSetup.stStartArg.stSvcInfo);
	pstContext->stSetup.stStartArg.stPsi.usSvcId		= DbSvc_GetSvcId(&pstContext->stSetup.stStartArg.stSvcInfo);
	pstContext->stSetup.stStartArg.stPsi.usPmtPid		= DbSvc_GetPmtPid(&pstContext->stSetup.stStartArg.stSvcInfo);
#endif
	return hErr;
}

#define ___MSG_FUNCTION___
STATIC BUS_Result_t xmgr_satip_base_msgBusCreate (xMgrSatIpContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hErr		= 	ERR_OK;
	HUINT32 			ulActionId	=	SVC_PIPE_GetActionId(hAction);
	MgrSatIp_Setup_t	*pstSetup	=	(MgrSatIp_Setup_t*)p1;

	HxLOG_Message("[SATIP_TIME]\n");
	if (NULL == pstSetup)
	{
		HxLOG_Error("Error : Invaild Param\n");
		return MESSAGE_BREAK;
	}

	/*resource check*/
	{
		hErr = MGR_ACTION_Take(ulActionId, eActionType_SATIP, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
		if (ERR_OK != hErr)
		{
			BUS_SendMessage(NULL, eMEVT_SATIP_NOTIFY_START_FAILED, pstContext->hAction, eMgrSatIpFailed_CREATE, pstContext->stSetup.ulRsvSlotId, 0);
			HxLOG_Critical("Critical : MGR_ACTION_Take() hAction=0x%x\n", hAction);
			return MESSAGE_BREAK;
		}
	}

	/* setup memrec pstContext */
	{
		pstContext->hAction 		= SVC_PIPE_IncreaseActionVersion(ulActionId);
		pstContext->eState			= eSatIpState_ReqStart;
		pstContext->pfActionProc	= BUS_MGR_GetMgrCallback(BUS_MGR_GetThis());

		HxSTD_MemCopy (&pstContext->stSetup, pstSetup, sizeof(MgrSatIp_Setup_t));
	}

	if(TRUE == pstContext->stSetup.stStartArg.bTranscode)
	{
		hErr = xmgr_satip_base_setupTranscoder(pstContext);
		if (ERR_OK != hErr)
		{
			BUS_SendMessage(NULL, eMEVT_SATIP_NOTIFY_START_FAILED, pstContext->hAction, eMgrSatIpFailed_CREATE, pstContext->stSetup.ulRsvSlotId, 0);
			HxLOG_Critical("Critical : xmgr_satip_base_setupTranscoder() hAction=0x%x\n", hAction);
			return MESSAGE_BREAK;
		}
	}

	/* start tune */
	{
		SvcRsc_Info_t		stRscInfo;

		HxSTD_MemSet(&stRscInfo, 0, sizeof(SvcRsc_Info_t));

		hErr = MGR_RSC_GetResourceInfo (ulActionId, &stRscInfo);
		if (ERR_OK != hErr)
		{
			BUS_SendMessage(NULL, eMEVT_SATIP_NOTIFY_START_FAILED, pstContext->hAction, eMgrSatIpFailed_CREATE, pstContext->stSetup.ulRsvSlotId, 0);
			HxLOG_Error("MGR_RSC_GetResourceInfo. Error!!!\n");
			return hErr;
		}

		HxLOG_Message(HxANSI_COLOR_YELLOW("SVC_CH_StartTune (hAction = 0x%x)\n"), hAction);
		hErr = SVC_CH_StartTune (pstContext->hAction, &stRscInfo, -1, &(pstContext->stSetup.stTuneParam));
		if (ERR_OK != hErr)
		{
			BUS_SendMessage(NULL, eMEVT_SATIP_NOTIFY_START_FAILED, pstContext->hAction, eMgrSatIpFailed_CREATE, pstContext->stSetup.ulRsvSlotId, 0);
			HxLOG_Error("SVC_CH_StartTune. Error!!!\n");
			return hErr;
		}
	}

	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_satip_base_msgBusDestroy (xMgrSatIpContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr = ERR_FAIL;

	if (eSatIpState_Running <= pstContext->eState)
	{
		/* stop service */
		if (TRUE == pstContext->stSetup.stStartArg.bTranscode)
		{
			hErr = xmgr_satip_base_stopTranscoder(pstContext);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("xmgr_satip_base_stopTranscoder. Error!!!\n");
			}
		}
		else
		{
			hErr = SVC_REC_Stop (pstContext->hAction, TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_REC_Stop. Error!!!\n");
			}

			hErr = SVC_REC_Close(pstContext->hAction, TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_REC_Close. Error!!!\n");
			}
		}
	}

	hErr = SVC_CH_StopTune (pstContext->hAction, (HUINT16)-1, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_CH_StopTune. Error!!!\n");
	}

	/* action */
	MGR_ACTION_Release (SVC_PIPE_GetActionId(pstContext->hAction), eActionType_SATIP);

	/* emit event */
	BUS_SendMessage (NULL, eMEVT_SATIP_NOTIFY_START_STOPPED, pstContext->hAction, 0, 0, 0);

	/* clean context */
	xmgr_satip_base_ClearContext (pstContext);

	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_satip_base_setPsiTable (xMgrSatIpContext_t *pstContext)
{
	HERROR						hErr = ERR_FAIL;
#if defined(CONFIG_MW_MM_TRANSCODING)
	SvcSi_MakeSecArg_t 			 unArg;
	SvcTranscoder_PSITable_t	*pstTrPsiTable;
	SvcSi_MakeSectionSpec_t		stMakeSecSpec;

	pstTrPsiTable = (SvcTranscoder_PSITable_t *)OxAP_Malloc (sizeof(SvcTranscoder_PSITable_t));
	if (NULL != pstTrPsiTable)
	{
		/* Create PAT */
		unArg.eSiType			= eSIxTABLETYPE_Pat;
		unArg.stPat.eSiType		= eSIxTABLETYPE_Pat;
		unArg.stPat.usTsId		= pstContext->stSetup.stStartArg.stPsi.usTsId;
		unArg.stPat.usSvcId		= pstContext->stSetup.stStartArg.stPsi.usSvcId;
		unArg.stPat.usPmtPid 	= pstContext->stSetup.stStartArg.stPsi.usPmtPid;

		pstTrPsiTable->ulRawSectionSize	= 0;
		HxSTD_MemSet(pstTrPsiTable->aucRawSectionBuffer, 0, MAX_TRANS_RAWSECTION_LEN);

		hErr = MGR_TRANSCODER_GetMakeSectionSiSpec(&stMakeSecSpec);
		if (hErr == ERR_OK)
		{
			hErr = SVC_SI_MakeSiSection(SVC_PIPE_GetActionId(pstContext->hAction), &stMakeSecSpec, &unArg, pstTrPsiTable->aucRawSectionBuffer, &(pstTrPsiTable->ulRawSectionSize));
		}

#ifdef _DUMP_SECTION_
		HxLOG_DecIntDump(unArg.stPat.eSiType);
		HxLOG_HexIntDump(unArg.stPat.usTsId);
		HxLOG_HexIntDump(unArg.stPat.usSvcId);
		HxLOG_HexIntDump(unArg.stPat.usPmtPid);
		HxLOG_DecIntDump(pstTrPsiTable->ulRawSectionSize);
		HLIB_LOG_Dump(pstTrPsiTable->aucRawSectionBuffer, 64, 0, 0);
#endif

		if ((ERR_OK == hErr) && (pstTrPsiTable->ulRawSectionSize > 0))
		{
			pstTrPsiTable->bContinue		= TRUE;
			pstTrPsiTable->usPeriodTime		= _SECTION_INSERTION_PERIOD;
			pstTrPsiTable->usPid			= PAT_PID;
			pstTrPsiTable->eSiType			= eSI_TableType_PAT;

			hErr = SVC_TRANSCODER_SetPSITable (pstContext->hAction, pstTrPsiTable, eSyncMode);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SVC_TRANSCODER_SetPSITable err: PAT\n");
			}
		}
		else
		{
			HxLOG_Error ("SVC_SI_MakeSiSection: siType:PAT, ulActionId(%d)\n", SVC_PIPE_GetActionId(pstContext->hAction));
		}

		/* Create PMT */
		unArg.eSiType			= eSIxTABLETYPE_Pmt;
		unArg.stPmt.eSiType		= eSIxTABLETYPE_Pmt;
		unArg.stPmt.usTsUniqId	= pstContext->stSetup.stStartArg.stPsi.usTsUniqId;
		unArg.stPmt.usSvcId		= pstContext->stSetup.stStartArg.stPsi.usSvcId;

		/* PMT settings should be the same as the SVC_TRANSCODER_Open settings */
		unArg.stPmt.eMakeSrcType							= eSVCSI_MAKESEC_PMT_SetByArgs;
		unArg.stPmt.unArgs.stSettingArg.ucVideoStreamType	= ISO_VIDEO_4; /* H.264 is ISO_VIDEO_4 */
		unArg.stPmt.unArgs.stSettingArg.usVideoPid			= pstContext->stSetup.stStartArg.stSrc.usVideoPid;
		unArg.stPmt.unArgs.stSettingArg.ucAudioStreamType	= ISO_AUDIO_AAC; /* AAC is ISO_AUDIO_4, or ISO_AUDIO_AAC */
		unArg.stPmt.unArgs.stSettingArg.usAudioPid			= pstContext->stSetup.stStartArg.stSrc.usAudioPid;
		unArg.stPmt.unArgs.stSettingArg.usPcrPid 			= CONFIG_TRANSCODER_FIXED_PCRPID;

		pstTrPsiTable->ulRawSectionSize	= 0;
		HxSTD_MemSet(pstTrPsiTable->aucRawSectionBuffer, 0, MAX_TRANS_RAWSECTION_LEN);

		hErr = MGR_TRANSCODER_GetMakeSectionSiSpec(&stMakeSecSpec);
		if (hErr == ERR_OK)
		{
			hErr = SVC_SI_MakeSiSection(SVC_PIPE_GetActionId(pstContext->hAction), &stMakeSecSpec, &unArg, pstTrPsiTable->aucRawSectionBuffer, &(pstTrPsiTable->ulRawSectionSize));
		}

#ifdef _DUMP_SECTION_
		HxLOG_DecIntDump(pstContext->stSetup.stStartArg.bTranscode);
		HxLOG_DecIntDump(unArg.stPmt.eSiType);
		HxLOG_HexIntDump(unArg.stPmt.usTsUniqId);
		HxLOG_HexIntDump(unArg.stPmt.usSvcId);
		HxLOG_HexIntDump(unArg.stPmt.unArgs.stSettingArg.usVideoPid);
		HxLOG_HexIntDump(unArg.stPmt.unArgs.stSettingArg.usAudioPid);
		HxLOG_HexIntDump(unArg.stPmt.unArgs.stSettingArg.usPcrPid);
		HxLOG_DecIntDump(pstTrPsiTable->ulRawSectionSize);
		HLIB_LOG_Dump(pstTrPsiTable->aucRawSectionBuffer, 64, 0, 0);
#endif

		if ((ERR_OK == hErr) && (pstTrPsiTable->ulRawSectionSize > 0))
		{
			pstTrPsiTable->bContinue		= TRUE;
			pstTrPsiTable->usPeriodTime		= _SECTION_INSERTION_PERIOD;
			pstTrPsiTable->usPid			= pstContext->stSetup.stStartArg.stPsi.usPmtPid;
			pstTrPsiTable->eSiType			= eSI_TableType_PMT;

			hErr = SVC_TRANSCODER_SetPSITable (pstContext->hAction, pstTrPsiTable, eSyncMode);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SVC_TRANSCODER_SetPSITable err: PMT\n");
			}
		}
		else
		{
			HxLOG_Error ("SVC_SI_MakeSiSection: siType:PMT, ulActionId(%d)\n", SVC_PIPE_GetActionId(pstContext->hAction));
		}

		/* Does not implement SIT : Implement when is necessary. */

		OxAP_Free (pstTrPsiTable);
	}
#endif
	return hErr;
}

STATIC HERROR xmgr_satip_base_startTranscoder (xMgrSatIpContext_t *pstContext)
{
	HERROR						hErr = ERR_FAIL;
#if defined(CONFIG_MW_MM_TRANSCODING)
	SvcTranscoder_Settings_t	stSettings;
	HUINT32						ulHddRecId, ulTranscoderId, ulDemuxId;
	HUINT32						ulActionId = SVC_PIPE_GetActionId (pstContext->hAction);

	HxLOG_Message("[SATIP_TIME_TRANSCODER_SERVICE]\n");

	HxSTR_Memset(&stSettings, 0x00, sizeof(SvcTranscoder_Settings_t));

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_HDD_REC, &ulHddRecId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_GetResourceId(eRxRSCTYPE_HDD_REC) failed: ActionId(%d)\n", ulActionId);
		return hErr;
	}

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_TRANSCODER, &ulTranscoderId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_GetResourceId(eRxRSCTYPE_TRANSCODER) failed: ActionId(%d)\n", ulActionId);
		return hErr;
	}

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_GetResourceId(eRxRSCTYPE_DEMUX) failed: ActionId(%d)\n", ulActionId);
		return hErr;
	}

	HxLOG_Debug ("------------<XMGR_SATIP:START:TRANSCODER>------------\n");
	HxLOG_Debug ("\t[hAction\t\t:\t0x%x]\n", pstContext->hAction);
	HxLOG_Debug ("\t[hRingBuf\t\t:\t0x%x]\n", pstContext->stSetup.hRingBuf);
	HxLOG_Debug ("\t[ulRecDeviceId\t\t:\t0x%x]\n", stSettings.ulRecDeviceId);
	HxLOG_Debug ("\t[ulDeviceId\t\t:\t0x%x]\n", stSettings.ulDeviceId);
	HxLOG_Debug ("\t[ulDemuxId\t\t:\t0x%x]\n", stSettings.ulDemuxId);

	stSettings.bTranscode			= TRUE;

	/* Include PCR Ts packet*/
	stSettings.bPcrTsPacketInsert	= pstContext->stSetup.stStartArg.bPcrTsPacketInsert;

	/* Status */
	stSettings.eTransEncodeMode		= eSVC_TRANSCODER_ENCODEMODE_AV; // TBD
	stSettings.eTransOutputType		= eSVC_TRANSCODER_OUTPUTTYPE_BUFFER;
	stSettings.eTransSourceType		= eSVC_TRANSCODER_SRCTYPE_Live;

	/* Resource ID */
	stSettings.ulRecDeviceId		= ulHddRecId;
	stSettings.ulDeviceId			= ulTranscoderId;
	stSettings.ulDemuxId			= ulDemuxId;

	stSettings.stAvPid.usVideoPid	= pstContext->stSetup.stStartArg.stSrc.usVideoPid;
	stSettings.stAvPid.usAudioPid	= pstContext->stSetup.stStartArg.stSrc.usAudioPid;
	stSettings.stAvPid.usPcrPid		= pstContext->stSetup.stStartArg.stSrc.usPcrPid;
	stSettings.eVideoCodec			= pstContext->stSetup.stStartArg.stSrc.eVideoCodec;
	stSettings.eAudioCodec			= pstContext->stSetup.stStartArg.stSrc.eAudioCodec;

	/* Profile(Output) */
	stSettings.nTranscodeProfileId				= -1;
	stSettings.stProfile.eVideoCodec			= pstContext->stSetup.stStartArg.stProfile.eVideoCodec;
	stSettings.stProfile.eAudioCodec			= pstContext->stSetup.stStartArg.stProfile.eAudioCodec;
	stSettings.stProfile.ucVideoStreamType		= 0;
	stSettings.stProfile.ucAudioStreamType		= 0;

	stSettings.stProfile.nAudioBitrate_kbps 	= pstContext->stSetup.stStartArg.stProfile.nAudioBitrateKbps;
	stSettings.stProfile.nVideoBitrate_kbps 	= pstContext->stSetup.stStartArg.stProfile.nVideoBitrateKbps;
	stSettings.stProfile.nHdVideoBitrate_kbps	= pstContext->stSetup.stStartArg.stProfile.nHdVideoBitrateKbps;
	stSettings.stProfile.nScreenWidth			= pstContext->stSetup.stStartArg.stProfile.nScreenWidth;
	stSettings.stProfile.nScreenHeight			= pstContext->stSetup.stStartArg.stProfile.nScreenHeight;
	stSettings.stProfile.nFrameRate 			= pstContext->stSetup.stStartArg.stProfile.nFrameRate;

	stSettings.stProfile.bIsInteraceMode		= pstContext->stSetup.stStartArg.stProfile.bInterlaceMode;

	stSettings.stProfile.eVideoAspectRatio		= pstContext->stSetup.stStartArg.stProfile.eVideoAspectRatio;

	stSettings.stProfile.eVideoCodecProfile 	= pstContext->stSetup.stStartArg.stProfile.eVideoCodecProfile;
	stSettings.stProfile.eVideoCodecLevel		= pstContext->stSetup.stStartArg.stProfile.eVideoCodecLevel;
	stSettings.stProfile.bUsingLiveAudioCodec	= pstContext->stSetup.stStartArg.stProfile.bUsingLiveAudioCodec;
	stSettings.stProfile.bUsingLiveAspectRatio	= pstContext->stSetup.stStartArg.stProfile.bUsingLiveAspectRatio;

	/*output setting*/
	stSettings.hRingBuf = pstContext->stSetup.hRingBuf;

	HxLOG_Debug ("------------<Debug : SAT>IP Transcoder Input Settings>------------\n");
	HxLOG_Debug ("\t[bTranscode\t\t:\t%d]\n", 			stSettings.bTranscode);
	HxLOG_Debug ("\t[bPcrTsPacketInsert\t:\t%d]\n",		stSettings.bPcrTsPacketInsert);
	HxLOG_Debug ("\t[eTransEncodeMode\t:\t%d]\n",		stSettings.eTransEncodeMode);
	HxLOG_Debug ("\t[eTransOutputType\t:\t%d]\n",		stSettings.eTransOutputType);
	HxLOG_Debug ("\t[eTransSourceType\t:\t%d]\n",		stSettings.eTransSourceType);
	HxLOG_Debug ("\t[usVideoPid\t\t:\t0x%x]\n",			stSettings.stAvPid.usVideoPid);
	HxLOG_Debug ("\t[usAudioPid\t\t:\t0x%x]\n",			stSettings.stAvPid.usAudioPid);
	HxLOG_Debug ("\t[usPcrPid\t\t:\t0x%x]\n",			stSettings.stAvPid.usPcrPid);
	HxLOG_Debug ("\t[eVideoCodec\t\t:\t%d]\n",			stSettings.eVideoCodec);
	HxLOG_Debug ("\t[eAudioCodec\t\t:\t%d]\n",			stSettings.eAudioCodec);

	HxLOG_Debug ("------------<Debug : SAT>IP Transcoder Profile(Output) Settings>------------\n");
	HxLOG_Debug ("\t[nTranscodeProfileId\t:\t%d]\n",	stSettings.nTranscodeProfileId);
	HxLOG_Debug ("\t[eVideoCodec\t\t:\t%d]\n",			stSettings.stProfile.eVideoCodec);
	HxLOG_Debug ("\t[eAudioCodec\t\t:\t%d]\n",			stSettings.stProfile.eAudioCodec);
	HxLOG_Debug ("\t[ucVideoStreamType\t:\t%d]\n",		stSettings.stProfile.ucVideoStreamType);
	HxLOG_Debug ("\t[ucAudioStreamType\t:\t%d]\n",		stSettings.stProfile.ucAudioStreamType);
	HxLOG_Debug ("\t[nAudioBitrate_kbps\t:\t%d]\n",		stSettings.stProfile.nAudioBitrate_kbps);
	HxLOG_Debug ("\t[nVideoBitrate_kbps\t:\t%d]\n",		stSettings.stProfile.nVideoBitrate_kbps);
	HxLOG_Debug ("\t[nHdVideoBitrate_kbps\t:\t%d]\n",	stSettings.stProfile.nHdVideoBitrate_kbps);
	HxLOG_Debug ("\t[nScreenWidth\t\t:\t%d]\n",			stSettings.stProfile.nScreenWidth);
	HxLOG_Debug ("\t[nScreenHeight\t\t:\t%d]\n",		stSettings.stProfile.nScreenHeight);
	HxLOG_Debug ("\t[nFrameRate\t\t:\t%d]\n",			stSettings.stProfile.nFrameRate);
	HxLOG_Debug ("\t[bIsInteraceMode\t:\t%d]\n",		stSettings.stProfile.bIsInteraceMode);
	HxLOG_Debug ("\t[eVideoAspectRatio\t:\t%d]\n",		stSettings.stProfile.eVideoAspectRatio);
	HxLOG_Debug ("\t[eVideoCodecProfile\t:\t%d]\n",		stSettings.stProfile.eVideoCodecProfile);
	HxLOG_Debug ("\t[eVideoCodecLevel\t:\t%d]\n",		stSettings.stProfile.eVideoCodecLevel);
	HxLOG_Debug ("\t[bUsingLiveAudioCodec\t:\t%d]\n",	stSettings.stProfile.bUsingLiveAudioCodec);
	HxLOG_Debug ("\t[bUsingLiveAspectRatio\t:\t%d]\n",	stSettings.stProfile.bUsingLiveAspectRatio);

	hErr = SVC_TRANSCODER_Open (pstContext->hAction, &stSettings, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_TRANSCODER_Open err: hAction(0x%08x)\n", pstContext->hAction);
		return hErr;
	}

	/* Set PSI Table (PAT, PMT) */
	hErr = xmgr_satip_base_setPsiTable (pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("xmgr_satip_base_setPsiTable err: hAction(0x%08x)\n", pstContext->hAction);
		return hErr;
	}

	hErr = SVC_TRANSCODER_StartService (pstContext->hAction, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_TRANSCODER_StartService err: hAction(0x%08x)\n", pstContext->hAction);
		return hErr;
	}

	pstContext->eState = eSatIpState_Running;
#endif
	return hErr;
}

STATIC HERROR xmgr_satip_base_startRecorder (xMgrSatIpContext_t *pstContext)
{
	HBOOL				bPidAvailable = FALSE;
	HERROR				hErr = ERR_FAIL;
	HINT32				i;
	SvcRec_PIDList_t	stRecPidInfo;

	SvcRec_Setup_t stSetup;

	HxSTD_MemSet(&stSetup, 0, sizeof(SvcRec_Setup_t));

	stSetup.eRecActType =	eRec_ActType_Satip;
	stSetup.eRecOutType =	eRec_OutType_MemoryBuf;

	stSetup.eSvcType	=	eDxSVC_TYPE_TV;
	stSetup.bEncryption =	FALSE;
	stSetup.eVideoCodec =	eDxVIDEO_CODEC_MPEG2;
	stSetup.ulStartTime =	0;

	stSetup.unOutArg.hMemoryBuf =	pstContext->stSetup.hRingBuf;

	HxLOG_Debug ("------------<XMGR_SATIP:START>------------\n");
	HxLOG_Debug ("\t[hAction\t\t:\t0x%x]\n", pstContext->hAction);
	HxLOG_Debug ("\t[hRingBuf\t\t:\t0x%x]\n", pstContext->stSetup.hRingBuf);
	HxLOG_Debug ("------------<START:XMGR_SATIP>------------\n");

	hErr = SVC_REC_Setup (pstContext->hAction, &stSetup, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_REC_Setup. Error\n");
		return hErr;
	}

	SVC_REC_CleanReqPidInfo (eRec_PidSetType_Unknown, &stRecPidInfo);

	/*PID copy*/
	if (-1 == pstContext->stSetup.nPIDsCount)
	{
		//ALL_PID
		stRecPidInfo.ePidSetType = eRec_PidSetType_All;
		bPidAvailable = TRUE;
	}
	else
	{
		for (i=0 ; i<pstContext->stSetup.nPIDsCount ; i++ )
		{
			stRecPidInfo.stUnknown.stPIDs[i].usPid =  pstContext->stSetup.usPIDs[i];
			if (PID_NULL != stRecPidInfo.stUnknown.stPIDs[i].usPid)
			{
				bPidAvailable = TRUE;
			}
		}
	}

#ifdef CONFIG_DEBUG
	SVC_REC_DebugPrintRecPidInfo_Base(&stRecPidInfo, __FUNCTION__, __LINE__);
#endif

	if (TRUE == bPidAvailable)
	{
		HxLOG_Message(HxANSI_COLOR_YELLOW("call SVC_REC_SetPID (hAction = 0x%x)\n"), pstContext->hAction);

		hErr = SVC_REC_SetPID( pstContext->hAction, &stRecPidInfo, TRUE );
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_REC_SetPID. Error\n");
			return hErr;
		}

		hErr = SVC_REC_Start( pstContext->hAction, TRUE );
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_REC_Start. Error\n");
			return hErr;
		}
	}

	if (TRUE == bPidAvailable)
	{
		pstContext->eState = eSatIpState_Running;
	}
	else
	{
		pstContext->eState = eSatIpState_NonPid;
	}

	hErr = ERR_OK;
	return hErr;
}

STATIC BUS_Result_t xmgr_satip_base_evtChLocked (xMgrSatIpContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	hErr = ERR_FAIL;

	HxLOG_Message("[SATIP_TIME]\n");

	if(eSatIpState_Running == pstContext->eState)
	{
		HxLOG_Warning("satip already running... (ulRsvSlotId = %d)\n", pstContext->stSetup.ulRsvSlotId);
		return MESSAGE_PASS;
	}

	if (TRUE == pstContext->stSetup.stStartArg.bTranscode)
	{
		hErr = xmgr_satip_base_startTranscoder (pstContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Error ("xmgr_satip_base_startTranscoder err: hAction(0x%08x)\n", pstContext->hAction);
			BUS_SendMessage(NULL, eMEVT_SATIP_NOTIFY_START_FAILED, pstContext->hAction, eMgrSatIpFailed_CREATE, pstContext->stSetup.ulRsvSlotId, 0);
			return hErr;
		}
	}
	else
	{
		hErr = xmgr_satip_base_startRecorder (pstContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Error ("xmgr_satip_base_startRecorder err: hAction(0x%08x)\n", pstContext->hAction);
			BUS_SendMessage(NULL, eMEVT_SATIP_NOTIFY_START_FAILED, pstContext->hAction, eMgrSatIpFailed_CREATE, pstContext->stSetup.ulRsvSlotId, 0);
			return hErr;
		}
	}

	/* emit event */
	HxLOG_Message(HxANSI_COLOR_YELLOW("call BUS_SendMessage(eMEVT_SATIP_NOTIFY_STARTED)\n"));
	BUS_SendMessage(NULL, eMEVT_SATIP_NOTIFY_STARTED, pstContext->hAction, 0, 0, 0);

	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_satip_base_evtChNoSignal (xMgrSatIpContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Message("[SATIP_TIME]\n");
	HxLOG_Error ("NoSignal !!!\n");

#if 0
	/* emit event */
	BUS_SendMessage(NULL, eMEVT_SATIP_NOTIFY_START_FAILED, pstContext->hAction, eMgrSatIpFailed_NOSIGNAL, pstContext->stSetup.ulRsvSlotId, 0);

	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);
#else
	HxLOG_Error ("Waiting for signal re-connection...\n");
#endif

	return MESSAGE_PASS;
}

#define ___PROC_FUNCTION___
STATIC BUS_Result_t xmgr_satip_base_protoProc (xMgrSatIpContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eRet = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Message(HxANSI_COLOR_YELLOW("***************************************************************\n"));
		HxLOG_Message(HxANSI_COLOR_YELLOW("*  XMGR : SATIP : eMEVT_BUS_CREATE\n"));
		HxLOG_Message(HxANSI_COLOR_YELLOW("***************************************************************\n"));
		eRet = xmgr_satip_base_msgBusCreate (pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Message(HxANSI_COLOR_YELLOW("***************************************************************\n"));
		HxLOG_Message(HxANSI_COLOR_YELLOW("*  XMGR : SATIP : eMEVT_BUS_DESTROY\n"));
		HxLOG_Message(HxANSI_COLOR_YELLOW("***************************************************************\n"));
		eRet = xmgr_satip_base_msgBusDestroy (pstContext, hAction, p1, p2, p3);
		break;

	case eSEVT_CH_LOCKED :
		CHECK_SATIP_ACTION_HANDLE();
		HxLOG_Message(HxANSI_COLOR_YELLOW("MESSAGE : eSEVT_CH_LOCKED\n"));
		eRet = xmgr_satip_base_evtChLocked (pstContext, hAction, p1, p2, p3);
		break;
	case eSEVT_CH_NO_SIGNAL :
		CHECK_SATIP_ACTION_HANDLE();
		HxLOG_Message(HxANSI_COLOR_YELLOW("MESSAGE : eSEVT_CH_NO_SIGNAL\n"));
		eRet = xmgr_satip_base_evtChNoSignal (pstContext, hAction, p1, p2, p3);
		break;
	default:
		break;
	}

	return (MESSAGE_BREAK == eRet) ? MESSAGE_BREAK : MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_satip_base_procBg0 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_satip_base_protoProc (xmgr_satip_base_GetContext (0), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_satip_base_procBg1 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_satip_base_protoProc (xmgr_satip_base_GetContext (1), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_satip_base_procBg2 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_satip_base_protoProc (xmgr_satip_base_GetContext (2), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_satip_base_procBg3 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_satip_base_protoProc (xmgr_satip_base_GetContext (3), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_satip_base_procBg4 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_satip_base_protoProc (xmgr_satip_base_GetContext (4), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_satip_base_procBg5 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_satip_base_protoProc (xmgr_satip_base_GetContext (5), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_satip_base_procBg6 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_satip_base_protoProc (xmgr_satip_base_GetContext (6), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_satip_base_procBg7 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_satip_base_protoProc (xmgr_satip_base_GetContext (7), message, hHandle, p1, p2, p3);
}

STATIC BUS_Callback_t xmgr_satip_base_GetActionProc (HUINT32 ulActionId)
{
	if (xmgr_satip_base_GetContext (ulActionId-eActionId_BG_FIRST) == NULL)
	{
		HxLOG_Error ("Action ID(%d) is not allowed for SATIP\n");
		return NULL;
	}

	switch (ulActionId)
	{
	case (eActionId_BG_FIRST + 0):
		return xmgr_satip_base_procBg0;

	case (eActionId_BG_FIRST + 1):
		return xmgr_satip_base_procBg1;

	case (eActionId_BG_FIRST + 2):
		return xmgr_satip_base_procBg2;

	case (eActionId_BG_FIRST + 3):
		return xmgr_satip_base_procBg3;

	case (eActionId_BG_FIRST + 4):
		return xmgr_satip_base_procBg4;

	case (eActionId_BG_FIRST + 5):
		return xmgr_satip_base_procBg5;

	case (eActionId_BG_FIRST + 6):
		return xmgr_satip_base_procBg6;

	case (eActionId_BG_FIRST + 7):
		return xmgr_satip_base_procBg7;

	default:
		break;
	}

	return NULL;

}

#define ___PLUGIN_FUNCTION___
HERROR xmgr_satip_Init_BASE (void)
{
	/* Clear Context */
	{
		HINT32 i;
		for (i=0 ; i<NUM_BG_ACTION ; i++)
		{
			xmgr_satip_base_ClearContext (&s_astSatIpCtx[i]);
		}
	}
	return ERR_OK;
}

HERROR xmgr_satip_Start_BASE (MgrSatIp_Setup_t *pstSetup, HUINT32 *pulActionId)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulActionId = 0;

	if ( (NULL == pstSetup) || (NULL == pulActionId) )
	{
		HxLOG_Error ("Error. : Invaild Arguments\n");
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_GetEmptyBgActionId(&ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error. : Alloc ActionId\n");
		return ERR_FAIL;
	}
	HxLOG_Print(HxANSI_COLOR_YELLOW("ulActionId = %d\n"), ulActionId);

	hErr = xmgr_satip_base_SetupAction (ulActionId, pstSetup);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error. : Setup Action\n");
		return ERR_FAIL;
	}

	/* Create MSG Proc */
	{
		Handle_t hAction = SVC_PIPE_GetActionHandle (ulActionId);

		hErr = BUS_MGR_Create (	NULL,
								APP_ACTION_PRIORITY,
								xmgr_satip_base_GetActionProc (ulActionId),
								hAction,
								(HINT32)pstSetup,
								0,
								0);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("Error. : Create Proc\n");
			return ERR_FAIL;
		}
	}

	/* output */
	*pulActionId = ulActionId;

	return ERR_OK;
}

HERROR xmgr_satip_ChangePids_BASE (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids)
{
	HINT32 				 i;
	HBOOL				 bPidAvailable = TRUE;
	SvcRec_PIDList_t	 stRecPidInfo;
	xMgrSatIpContext_t	*pstContext = NULL;
	HERROR				 hErr;

	HxLOG_Message(HxANSI_COLOR_YELLOW("Enter \n"));

	if (NULL == pusPids)
	{
		HxLOG_Error ("pusPids is NULL pointer!\n");
		return ERR_FAIL;
	}

	pstContext = xmgr_satip_base_GetContext (ulActionId - eActionId_BG_FIRST);
	if (NULL == pstContext)
	{
		HxLOG_Error ("pstContext is NULL pointer!\n");
		return ERR_FAIL;
	}

	if (TRUE == pstContext->stSetup.stStartArg.bTranscode)
	{
		HxLOG_Debug ("SAT>IP cannot update requested PID(s) because it is not supported with transcoding.\n");
		return ERR_FAIL;
	}

	if (((pstContext->stSetup.nPIDsCount < 0) && (nPidsCount > 0)) ||
		((pstContext->stSetup.nPIDsCount > 0) && (nPidsCount < 0)))
	{
		hErr = xmgr_satip_base_RestartAction(ulActionId);
		if (ERR_OK == hErr)
		{
			pstContext->stSetup.nPIDsCount = nPidsCount;
			if (nPidsCount > 0)
			{
				HxSTD_MemCopy(pstContext->stSetup.usPIDs, pusPids, sizeof(HUINT16) * nPidsCount);
			}
		}
		else
		{
			HxLOG_Error("xmgr_satip_base_RestartAction ActionId(%d) err:\n", ulActionId);
			return ERR_FAIL;
		}

		return ERR_OK;
	}
	else if ((pstContext->stSetup.nPIDsCount < 0) && (nPidsCount < 0))
	{
		// ALL -> ALL : Nothing changed:
		return ERR_OK;
	}
	else if (nPidsCount == 0)
	{
		bPidAvailable = FALSE;
	}
	else if (nPidsCount > 0)
	{
		bPidAvailable = FALSE;

		for (i=0 ; i < nPidsCount ; i++ )
		{
			if (pusPids[i] < PID_NULL)
			{
				bPidAvailable = TRUE;
				break;
			}
		}
	}

	// TODO: update pid를 context로 복사를 여기서 하도록 수정. PID none인 경우에도 문제 없는지 확인 후에 수정

	// PID NONE
	if (TRUE != bPidAvailable)
	{
		if (pstContext->eState <= eSatIpState_Running)
		{
			/* stop service */
			hErr = SVC_REC_Stop (pstContext->hAction, TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_REC_Stop. Error!!!\n");
			}

			hErr = SVC_REC_Close(pstContext->hAction, TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_REC_Close. Error!!!\n");
			}
		}

		pstContext->eState = eSatIpState_NonPid;
 		return ERR_OK;
	}

	// PID -> PID : PID Change Function
	if (pstContext->eState <= eSatIpState_Running)
	{
		if( xmgr_satip_base_CheckValidPidList (nPidsCount, pusPids) == FALSE )
			return ERR_FAIL;

		// TODO: 아래 코드는 위로 올라가야 한다.
		pstContext->stSetup.nPIDsCount = nPidsCount;
		if (nPidsCount > 0)
		{
			HxSTD_MemCopy(pstContext->stSetup.usPIDs, pusPids, sizeof(HUINT16) * nPidsCount);
		}

		/*PID copy*/
		SVC_REC_CleanReqPidInfo (eRec_PidSetType_Unknown, &stRecPidInfo);
		for (i=0 ; i<nPidsCount ; i++ )
		{
			stRecPidInfo.stUnknown.stPIDs[i].usPid = pusPids[i];
		}

		HxLOG_Message(HxANSI_COLOR_YELLOW("call SVC_REC_SetPID\n"));
		SVC_REC_SetPID( pstContext->hAction, &stRecPidInfo, TRUE );
	}
	// PID NONE -> PID : Restart the work
	else
	{
		hErr = xmgr_satip_base_RestartAction(ulActionId);
		if (ERR_OK == hErr)
		{
			// TODO: 아래 코드는 위로 올라가야 한다.
			pstContext->stSetup.nPIDsCount = nPidsCount;
			if (nPidsCount > 0)
			{
				HxSTD_MemCopy(pstContext->stSetup.usPIDs, pusPids, sizeof(HUINT16) * nPidsCount);
			}
		}
		else
		{
			HxLOG_Error("xmgr_satip_base_RestartAction ActionId(%d) err:\n", ulActionId);
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

HERROR xmgr_satip_ChangePriority_BASE (HUINT32 ulActionId, HUINT16 usPriority)
{
	MgrAction_Priority_e		 eActPriority = eAmPriority_RECORD;

	eActPriority = xmgr_satip_base_GetActPriority(usPriority);

	HxLOG_Debug("Change Action Priority. actionId(%d) schedule priority(%d) action priority(0x%x) \n",ulActionId, usPriority, eActPriority);
	(void)MGR_ACTION_ChangePriority(ulActionId, eActPriority);

	return ERR_OK;
}

HERROR xmgr_satip_ChangeTranscodingProfile_BASE (HUINT32 ulActionId, MgrSatIp_TranscoderStreamProfile stProfile)
{
	HERROR					hErr		= ERR_FAIL;
#if defined(CONFIG_MW_MM_TRANSCODING)
	xMgrSatIpContext_t		*pstContext = NULL;
	SvcTranscoder_Profile_t	stSettings;

	HxLOG_Debug("Change Transcoding Profile. actionId(%d). \n", ulActionId);

	pstContext = xmgr_satip_base_GetContext (ulActionId - eActionId_BG_FIRST);
	if (NULL == pstContext)
	{
		HxLOG_Error("Error! Null context. actionId(%d). \n", ulActionId);
		return ERR_FAIL;
	}

	HxSTR_Memset(&stSettings, 0x00, sizeof(SvcTranscoder_Profile_t));

	stSettings.nVideoBitrate_kbps	= stProfile.nVideoBitrateKbps;
	stSettings.nFrameRate			= stProfile.nFrameRate;
	stSettings.eVideoAspectRatio	= stProfile.eVideoAspectRatio;
	stSettings.nScreenWidth			= stProfile.nScreenWidth;
	stSettings.nScreenHeight		= stProfile.nScreenHeight;

	stSettings.eVideoCodec			= stProfile.eVideoCodec;
	stSettings.eAudioCodec			= stProfile.eAudioCodec;
	stSettings.nAudioBitrate_kbps	= stProfile.nAudioBitrateKbps;
	stSettings.bIsInteraceMode		= stProfile.bInterlaceMode;

	HxLOG_Debug ("------------<Debug : SAT>IP Transcoder Change Profile Settings>------------\n");
	HxLOG_Debug ("\t[eVideoCodec\t\t:\t%d]\n",			stSettings.eVideoCodec);
	HxLOG_Debug ("\t[eAudioCodec\t\t:\t%d]\n",			stSettings.eAudioCodec);
	HxLOG_Debug ("\t[nAudioBitrate_kbps\t:\t%d]\n",		stSettings.nAudioBitrate_kbps);
	HxLOG_Debug ("\t[nVideoBitrate_kbps\t:\t%d]\n",		stSettings.nVideoBitrate_kbps);
	HxLOG_Debug ("\t[nScreenWidth\t\t:\t%d]\n",			stSettings.nScreenWidth);
	HxLOG_Debug ("\t[nScreenHeight\t\t:\t%d]\n",		stSettings.nScreenHeight);
	HxLOG_Debug ("\t[nFrameRate\t\t:\t%d]\n",			stSettings.nFrameRate);
	HxLOG_Debug ("\t[bIsInteraceMode\t:\t%d]\n",		stSettings.bIsInteraceMode);
	HxLOG_Debug ("\t[eVideoAspectRatio\t:\t%d]\n",		stSettings.eVideoAspectRatio);

	hErr = SVC_TRANSCODER_ChangeProfile(pstContext->hAction, &stSettings, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_TRANSCODER_ChangeProfile err: hAction(0x%08x)\n", pstContext->hAction);
		return hErr;
	}
#endif
	return hErr;
}

HERROR xmgr_satip_Try_BASE (MgrSatIp_Setup_t *pstSetup)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulActionId = 0;
	MgrAction_Priority_e		 eActPriority = eAmPriority_RECORD;
	HBOOL	bForce = FALSE;

	hErr = MGR_ACTION_GetEmptyBgActionId(&ulActionId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("Error. : Alloc ActionId\n");
		return hErr;
	}

	eActPriority = xmgr_satip_base_GetActPriority(pstSetup->usPriority);
	bForce = (pstSetup->eAllocMode == DxRsvStream_AllocMode_FORCE)? TRUE: FALSE;
	HxLOG_Debug("try satip. act priority(0x%x) force(%d) \n",eActPriority, bForce);
	hErr = MGR_ACTION_TrySatIp (ulActionId, &pstSetup->stTuneParam, eActPriority, bForce,pstSetup->stStartArg.bTranscode);
	if (hErr != ERR_OK)
	{
		HxLOG_Debug("satip try failed. actPiority(0x%x) \n",eActPriority);
		return hErr;
	}

	return hErr;
}


HERROR xmgr_satip_Stop_BASE (HUINT32 ulActionId)
{
	xMgrSatIpContext_t* pstContext = NULL;

	pstContext = xmgr_satip_base_GetContext (ulActionId - eActionId_BG_FIRST);

	if (NULL != pstContext)
	{
		if(NULL != pstContext->pfActionProc)
		{
			BUS_MGR_Destroy (pstContext->pfActionProc);
		}

		HxLOG_Debug("Debug. SATIP STOP! (ulActionId = %d)\n", ulActionId);
	}
	else
	{
		HxLOG_Error("Error. not found SATIP's context\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
