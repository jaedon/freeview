/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_output.h>
#include <svc_si.h>
#include <svc_fs.h>
#include <svc_pb.h>
#include <svc_cas.h>
#include <svc_av.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_sys.h>
#include <svc_resources.h>

#include <get_enum_str.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_output.h>
#include <mgr_pg.h>
#include <mgr_rscmap.h>
#include <mgr_action.h>
#include <mgr_media.h>
#include <mgr_svclist.h>
#include <mgr_live.h>
#include <mgr_playback.h>
#include <_xmgr_playback.h>
#include "local_include/_xmgr_playback_base.h"

#if defined(CONFIG_MW_CI) || defined(CONFIG_MW_CAS)
#include <mgr_cas.h>
#endif

#include <xmgr_cas.h>


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define PLAYBACK_PMT_TIMESTAMP_MARGIN	(10)		// example: When Recording, wrote pmt stamp 725. But when playback, SI's PMT signal happens at 715.

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define	XmgrPb_SUBTITLE_EBU		eSI_SUBTITLE_EBU
#define	XmgrPb_SUBTITLE_DVB		eSI_SUBTITLE_DVB

#define	XmgrPb_SubtitleInfo			SvcSi_SubtitleInfo_t

#define XmgrPb_SUBTITLE_INFO_Clear(pstSubtitleInfo)		HxSTD_MemSet(pstSubtitleInfo, 0xff, sizeof(XmgrPb_SubtitleInfo));
#define	XmgrPb_SUBTITLE_INFO_Comp(pstInfo1, pstInfo2)	HxSTD_MemCmp(pstInfo1, pstInfo2, sizeof(XmgrPb_SubtitleInfo))

typedef	enum
{
	ePbDrm_ValidLicense = 0,
	ePbDrm_InvalidLicense,
	ePbDrm_NoLicense,
} xmgrPb_DrmErrorStatus_e;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC xmgrPbContext_t			s_astMgrPbContext[MAX_NUM_OF_PLAY];

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define _________LOCAL_FUNC__________
STATIC HERROR xmgr_playback_InitAllContext_Base (void)
{
	HUINT32					 ulCount;

	HxSTD_MemSet (s_astMgrPbContext, 0, sizeof(xmgrPbContext_t) * MAX_NUM_OF_PLAY);

	for (ulCount = 0; ulCount < MAX_NUM_OF_PLAY; ulCount++)
	{
		s_astMgrPbContext[ulCount].hAction = HANDLE_NULL;
	}

	return ERR_OK;
}

STATIC HBOOL xmgr_playback_IsTrickableState_Base (xmgrPbContext_t *pstContext)
{
	HBOOL bAvStarted = (pstContext->svcInfo.eSvcType == eDxSVC_TYPE_RADIO) ? pstContext->bAudioStarted : pstContext->bVideoStarted;
	HBOOL bIsBgAction = FALSE;

	HxLOG_Debug("pstContext->svcInfo.eSvcType (0x%x)\n", pstContext->svcInfo.eSvcType);
	HxLOG_Debug("bVideoStarted (%d), bAudioStarted (%d)\n", pstContext->bVideoStarted, pstContext->bAudioStarted);

	bIsBgAction = MGR_ACTION_IsBackGroundAction(pstContext->ulActionId);
	HxLOG_Debug("bPBStart (%d), bIsBgAction (%d)\n", pstContext->bPBStart, bIsBgAction);
	if(TRUE == bIsBgAction)
	{
		return (pstContext->bPBStart == TRUE) ? TRUE : FALSE;
	}
	else
	{
		if( pstContext->stSetup.bStartPaused == TRUE )
		{
			// Start&Pause 시에는 bAvStarted가 Set되지 않는다.
			return (pstContext->bPBStart == TRUE) ? TRUE : FALSE;
		}
		else
		{
			return ((pstContext->bPBStart == TRUE) /*&& (pstContext->bFirstPmt == TRUE)*/ && (bAvStarted == TRUE)) ? TRUE : FALSE;
		}
	}
}

HBOOL xmgr_playback_CheckPossibleSCPincode_Base(HUINT32 ulActionId)
{
	xmgrPbContext_t 		*pstContext = NULL;

	pstContext = xmgr_playback_GetContext_Base(ulActionId);
	if( NULL == pstContext )
		return ERR_FAIL;

	return FALSE;
}

STATIC HERROR xmgr_playback_StopTsrPlayback_Base (HUINT32 ulActionId)
{
	BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);
	return ERR_OK;
}

STATIC HERROR xmgr_playback_StopChasePlayback_Base (HUINT32 ulActionId)
{
	BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);
	return ERR_OK;
}

STATIC HERROR xmgr_playback_CheckPinStateChanged_Base(MgrPb_State_e eOldState, MgrPb_State_e eNewState, HBOOL *pbChanged, HBOOL *pbBlocked)
{
	if((pbChanged == NULL) || (pbBlocked == NULL))
	{
		return ERR_FAIL;
	}
	switch(eOldState)
	{
		case eMgrPb_RATING_PIN:
			switch(eNewState)
			{
				case eMgrPb_RATING_PIN:
					*pbBlocked = TRUE;
					*pbChanged = FALSE;
					break;
				case eMgrPb_RATING_PIN_DENIED:
					*pbBlocked = TRUE;
					*pbChanged = TRUE;
					break;
				default:
					*pbBlocked = FALSE;
					*pbChanged = TRUE;
					break;
			}
			break;
		case eMgrPb_RATING_PIN_DENIED:
			switch(eNewState)
			{
				case eMgrPb_RATING_PIN:
					*pbBlocked = TRUE;
					*pbChanged = TRUE;
					break;
				case eMgrPb_RATING_PIN_DENIED:
					*pbBlocked = TRUE;
					*pbChanged = FALSE;
					break;
				default:
					*pbBlocked = FALSE;
					*pbChanged = TRUE;
					break;
			}
			break;
		default:
			switch(eNewState)
			{
				case eMgrPb_RATING_PIN:
				case eMgrPb_RATING_PIN_DENIED:
					*pbBlocked = TRUE;
					*pbChanged = TRUE;
					break;
				default:
					*pbBlocked = FALSE;
					*pbChanged = FALSE;
					break;
			}
			break;
	}
	return ERR_OK;
}

HBOOL xmgr_playback_CheckAvDecUsed_Base (xmgrPbContext_t *pstContext)
{
	return (0 == (pstContext->ePbType & (eMgrPb_PlayType_Transcoder | eMgrPb_PlayType_Transcoder_SectionGathering))) ? TRUE : FALSE;
}

STATIC HBOOL xmgr_playback_CheckSectionGathering_Base (xmgrPbContext_t *pstContext)
{
	// TrRec을 위해 section gathering 중인지 확인
	return (pstContext->ePbType & eMgrPb_PlayType_Transcoder_SectionGathering) ? TRUE : FALSE;
}

STATIC void xmgr_playback_SendPinCtrlChanged_Base(xmgrPbContext_t *pstContext, HBOOL bBlocked)
{
	Handle_t		hAction;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	HBOOL		nBlockedByCas = FALSE;
#else
	HUINT32 systemId = 0;
#endif
	if(pstContext == NULL)
		return;

	hAction = SVC_PIPE_GetActionId (pstContext->ulActionId);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
{
	HBOOL			bPossibleSCPincode = FALSE;

	bPossibleSCPincode = xmgr_playback_CheckPossibleSCPincode_Base(pstContext->ulActionId);
	if(bPossibleSCPincode == TRUE)
	{
		nBlockedByCas = SVC_CAS_CheckSmartCardValidity(eDxCAS_GROUPID_NA);
	}
	else
	{
		nBlockedByCas = FALSE;
	}
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_PLAYBACK_EVENT_RATING_CTRL_CHANGED, hAction, bBlocked, pstContext->ulRatingValue, nBlockedByCas);
#else
	BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_PLAYBACK_EVENT_RATING_CTRL_CHANGED, hAction, bBlocked, pstContext->ulRatingValue, systemId);
#endif
}

STATIC HINT8* xmgr_playback_GetUiStateStr_Base(MgrPb_State_e uiState)
{
	switch(uiState)
	{
		ENUM_TO_STR(eMgrPb_NONE);
		ENUM_TO_STR(eMgrPb_CAS);
		ENUM_TO_STR(eMgrPb_LOCK_PIN);
		ENUM_TO_STR(eMgrPb_LOCK_PIN_DENIED);
		ENUM_TO_STR(eMgrPb_RATING_PIN);
		ENUM_TO_STR(eMgrPb_RATING_PIN_DENIED);
		ENUM_TO_STR(eMgrPb_PB_LOCK);
		ENUM_TO_STR(eMgrPb_PB_LOCK_DENIED);
		ENUM_TO_STR(eMgrPb_AV_UNDER_RUN);
		ENUM_TO_STR(eMgrPb_HDMI_BLOCKED);
		ENUM_TO_STR(eMgrPb_NO_AV);
		ENUM_TO_STR(eMgrPb_NO_VIDEO);
	}

	HxLOG_Critical("\n\n");
	return "Unknown uiState";
}

STATIC void xmgr_playback_SetUiState_Base(xmgrPbContext_t *pstContext, MgrPb_State_e uiState)
{
	HxLOG_Info("xmgr_playback_SetUiState_Base+(%s)\n", xmgr_playback_GetUiStateStr_Base(uiState));

#if defined( CONFIG_MW_CI_PLUS)		// A/V가 blanking 되었을 때 그에 맞는 system message 출력을 위하여 event를 올리는 부분...
	if (pstContext->stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
	{
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		if(pstContext->eCiPlusDrmState == eXmgrPbCiPlusDrmState_None)	// DRM state가 아무것도 걸려있지 않을 때에는 A/V를 풀어 줘야 함. but xmgr_playback_SetUiState_Base()에 eMgrPb_CAS로 타는 경우 eXmgrPbCiPlusDrmState_None인 경우는 없을 것 임.
		{
			if(uiState == eMgrPb_AV_UNDER_RUN)
			{
				HxLOG_Info("AV UNDER RUN ...\n");
				BUS_SendMessage(NULL, eMEVT_PVR_PLAYBACK_EVENT_DRM_BLOCK, pstContext->hAction, ePbDrm_NoLicense, 0, 0);
			}
			else
			{
			HxLOG_Info("A/V will be shown... again...\n");
				BUS_SendMessage(NULL, eMEVT_PVR_PLAYBACK_EVENT_DRM_BLOCK, pstContext->hAction, ePbDrm_ValidLicense, 0, 0);
			}
		}
		else
#endif
		{
			HxLOG_Info("Send MSG for DRM Block UI...\n");
			BUS_SendMessage(NULL, eMEVT_PVR_PLAYBACK_EVENT_DRM_BLOCK, pstContext->hAction, ePbDrm_InvalidLicense, 0, 0);
		}
	}
	else
#endif
	{
		if(uiState == eMgrPb_AV_UNDER_RUN)
		{
			HxLOG_Info("AV UNDER RUN ...\n");

			/*
			 * Under Run에 대한 이유에 Action이 되야함.
			 */
			if(uiState == eMgrPb_AV_UNDER_RUN)
			{
				HxLOG_Info("AV UNDER RUN ...\n");
				BUS_SendMessage(NULL, eMEVT_PVR_PLAYBACK_EVENT_DRM_BLOCK, pstContext->hAction, ePbDrm_NoLicense, 0, 0);
			}
		}
		else if(uiState == eMgrPb_NONE)
		{
			HxLOG_Info("A/V will be shown... again...\n");
			BUS_SendMessage(NULL, eMEVT_PVR_PLAYBACK_EVENT_DRM_BLOCK, pstContext->hAction, ePbDrm_ValidLicense, 0, 0);
		}
	}

	if(uiState != pstContext->uiState)
	{
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
		if (uiState == eMgrPb_CAS)
		{
			if( BUS_MGR_Get(AP_CasMain_GetActionProc(ulActionId)) != NULL )
			{
				BUS_MGR_Destroy(AP_CasMain_GetActionProc(ulActionId));
			}

			BUS_MGR_Create(NULL, APP_CAS_PRIORITY, AP_CasMain_GetActionProc(ulActionId), ulActionId, pstContext->nCasSystemId, (HINT32)pstContext->pCasInfo, pstContext->nCasEvt);
		}
		else
#endif
		{
			HBOOL bChanged = FALSE, bBlocked = FALSE;
			xmgr_playback_CheckPinStateChanged_Base(pstContext->uiState, uiState, &bChanged, &bBlocked);

			/* UI state 바꾸고... */
			pstContext->uiState = uiState;
			BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STATE_CHANGED, pstContext->hAction, uiState, 0, 0);

			/* ui 찌꺼기를 남기지 않기 위해 pin code dialog를 띄울 때는 먼저, destroy시킬 때는 나중에 event를 발생시켜야 한다. */
			if(bChanged == TRUE && bBlocked == TRUE)
				xmgr_playback_SendPinCtrlChanged_Base(pstContext, bBlocked);

			if(bChanged == TRUE && bBlocked == FALSE)
				xmgr_playback_SendPinCtrlChanged_Base(pstContext, bBlocked);

		}
	}
	else
	{
		/* 같은 state다. 그냥 있으면 되겠네... */
		HxLOG_Info("\t=> Same UI state.\n");
	}
	HxLOG_Info("xmgr_playback_SetUiState_Base-(%s)\n", xmgr_playback_GetUiStateStr_Base(uiState));

}

STATIC void xmgr_playback_CheckAV_Base(xmgrPbContext_t *pstContext)
{
	HxLOG_Info("xmgr_playback_CheckAV_Base()\n");

	if(pstContext->bPmtValid == TRUE)
	{
		if(	(pstContext->svcInfo.usAudioPid >= PID_NULL) &&
			(pstContext->svcInfo.usVideoPid >= PID_NULL) &&
			(pstContext->svcInfo.usDolbyPid >= PID_NULL) )
		{ /* PMT까지 들어온 상황에서 PID가 모두 NULL이면 NO_AV ui state로 감. PMT 이전에는 유보... */
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_NO_AV);
		}
		else if( (pstContext->svcInfo.eSvcType == eDxSVC_TYPE_TV) &&
				(pstContext->svcInfo.usVideoPid >= PID_NULL) )
		{ /* TV service에서 Video PID가 없을 경우 */
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_NO_VIDEO);
		}
		else
		{
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_NONE);
		}
	}
	else
	{
		xmgr_playback_SetUiState_Base(pstContext, eMgrPb_NONE);
	}
}

STATIC void xmgr_playback_CheckRadioBackground_Base(xmgrPbContext_t *pstContext)
{
	HUINT32		ulActionId = pstContext->ulActionId;

	HxLOG_Info("xmgr_playback_CheckRadioBackground_Base(eSvcType[%d], usVideoPid[%x], bRadioBackground[%d]\n",
		pstContext->svcInfo.eSvcType, pstContext->svcInfo.usVideoPid, pstContext->bRadioBackground);

	if( (ulActionId == MGR_ACTION_GetMainActionId()) && /* Main view action 이고 ... */
		(pstContext->svcInfo.eSvcType == eDxSVC_TYPE_RADIO) && /* Radio service 이고 ... */
		(pstContext->svcInfo.usVideoPid >= PID_NULL) /* Video PID가 NULL 이면 background 생성. */
	)
	{
		HxLOG_Print("OK! to draw RADIO BACKGROUND !!\n");

		if(pstContext->bRadioBackground == FALSE) /* Radio background가 안 떴으면... */
		{
			HxLOG_Print("\tActually drawing RADIO BACKGROUND!!\n");

			pstContext->bRadioBackground = TRUE; /* Drawing 중에 이 flag를 본다. */
			BUS_SendMessage (NULL, eMEVT_SVCLIST_RADIOBG_CHANGED, pstContext->hAction, (HINT32)pstContext->bRadioBackground, 0, 0);
		}
	}
	else
	{
		HxLOG_Print("\tNot in the radio background condition.\n");

		if(pstContext->bRadioBackground == TRUE) /* Radio background가 떠 있으면... */
		{
			HxLOG_Print("\tErase already drawn radio background.\n");
			pstContext->bRadioBackground = FALSE; /* Drawing 중에 이 flag를 본다. */
			BUS_SendMessage (NULL, eMEVT_SVCLIST_RADIOBG_CHANGED, pstContext->hAction, (HINT32)pstContext->bRadioBackground, 0, 0);
		}
	}
}


STATIC HBOOL	xmgr_playback_IsSubtitlePermitted_Base(xmgrPbContext_t *pstContext)
{
	if (NULL == pstContext)
		return FALSE;

	HxLOG_Print("\t =====> bSubtitleAllowed[%d], bTtxPageEnabled[%d], ePbState[%d].\n", pstContext->bSubtitleAllowed, pstContext->bTtxPageEnabled, pstContext->ePbState);

	if( (FALSE == pstContext->bSubtitleAllowed) ||
		(TRUE  == pstContext->bTtxPageEnabled)  ||
		(eXmgrPbState_AV != pstContext->ePbState) )
	{
		return FALSE;
	}

	return TRUE;
}

STATIC HERROR xmgr_playback_FindSubtitleInfo_Base(xmgrPbContext_t *pstContext, XmgrPb_SubtitleInfo *pstSubtitleInfo)
{
	HERROR		hErr		=	ERR_FAIL;
	HUINT32 	ulActionId	=	0;
	HUINT32 	ulSelectIdx	= 	0;
	SvcMeta_Service_t	stSvcInfo;

	if( (NULL == pstSubtitleInfo) || (NULL == pstContext) )
		return ERR_FAIL;


	/*prefer*/
	ulActionId = pstContext->ulActionId;
	ulSelectIdx = pstContext->svcInfo.ucSubttlIdx;

	HxSTD_MemSet(&stSvcInfo, 0, sizeof(SvcMeta_Service_t));
	hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, &stSvcInfo);

	/*get*/
	{
		HINT32 nMinTrack = 0;
		SvcSi_SubtitleList_t stSubtitleList;

		HxSTD_MemSet( &stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t) );
		hErr = SVC_SI_GetSubtitleList (ulActionId, stSvcInfo.usSvcId, &stSubtitleList);
		if( ERR_OK == hErr )
		{
			HINT32 menuLangIndex = 0;

			if(pstContext->svcInfo.bSubttlUserFlag == FALSE)
			{// Apply the menu language
				// 1. Finding subtitle-language same with primary subtitle language
				hErr = xmgr_playback_GetMenuSubtitleLangIndex(eDB_PARAM_ITEM_LANG_SUBTITLE0, &stSubtitleList, &menuLangIndex);
				if(hErr != ERR_OK)
				{
					// 2. Finding subtitle-language same with secandary subtitle language
					hErr = xmgr_playback_GetMenuSubtitleLangIndex(eDB_PARAM_ITEM_LANG_SUBTITLE1, &stSubtitleList, &menuLangIndex);
				}

				// if find to subtitle-language same with menu, change index.
				if(hErr == ERR_OK)
				{
					ulSelectIdx = pstContext->svcInfo.ucSubttlIdx = menuLangIndex;
				}
#if defined( CONFIG_OP_NORDIG )
				else
				{
					// if subtitle-language same with menu is not exist, does not display subtitle at nordig.
					SVC_SI_FreeSubtitleList(&stSubtitleList);
					return ERR_FAIL;
				}
#endif

				hErr = ERR_OK;
			}

			nMinTrack = (stSubtitleList.ulSubtitleNum<SvcSi_MAX_SUBTTL_ES_NUM)? stSubtitleList.ulSubtitleNum : SvcSi_MAX_SUBTTL_ES_NUM;

			if( nMinTrack > ulSelectIdx )
			{
				pstSubtitleInfo->usPid	=	stSubtitleList.pstSubtitleInfo[ulSelectIdx].usPid;

				switch(stSubtitleList.pstSubtitleInfo[ulSelectIdx].eType)
				{
					case XmgrPb_SUBTITLE_DVB:
					{
						pstSubtitleInfo->eType	=	XmgrPb_SUBTITLE_DVB;
						HxSTD_MemCopy(	&pstSubtitleInfo->utArg.stDvbInfo.aucLangCode,
									&stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stDvbInfo.aucLangCode,
									sizeof(stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stDvbInfo.aucLangCode) );

						pstSubtitleInfo->utArg.stDvbInfo.ucType 				= stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stDvbInfo.ucType;
						pstSubtitleInfo->utArg.stDvbInfo.ucComponentTag			= stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stDvbInfo.ucComponentTag;
						pstSubtitleInfo->utArg.stDvbInfo.usCompositionPageId	= stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stDvbInfo.usCompositionPageId;
						pstSubtitleInfo->utArg.stDvbInfo.usAncillaryPageId		= stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stDvbInfo.usAncillaryPageId;

						break;
					}
					case XmgrPb_SUBTITLE_EBU:
					{
						pstSubtitleInfo->eType	=	XmgrPb_SUBTITLE_EBU;
						HxSTD_MemCopy(	&pstSubtitleInfo->utArg.stEbuInfo.aucLangCode,
									&stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stEbuInfo.aucLangCode,
									sizeof(stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stEbuInfo.aucLangCode) );

						pstSubtitleInfo->utArg.stEbuInfo.ucType 		= stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stEbuInfo.ucType;
						pstSubtitleInfo->utArg.stEbuInfo.ucComponentTag	= stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stEbuInfo.ucComponentTag;
						pstSubtitleInfo->utArg.stEbuInfo.ucMagazineNum	= stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stEbuInfo.ucMagazineNum;
						pstSubtitleInfo->utArg.stEbuInfo.ucPageNum		= stSubtitleList.pstSubtitleInfo[ulSelectIdx].utArg.stEbuInfo.ucPageNum;

						break;
					}
					default : hErr = ERR_FAIL;
				}
			}
			else
			{
				/*out of index*/
				hErr = ERR_FAIL;
			}

			SVC_SI_FreeSubtitleList(&stSubtitleList);	//free
		}
	}

	return hErr;
}

STATIC void xmgr_playback_SetPinPassedEventId_Base(xmgrPbContext_t *pstContext)
{
	HUINT8			ucPRate;
	HBOOL			bRatingPassed;

	MGR_LIVE_PincodeIsEventIdRatingPassed(pstContext->hSvc, pstContext->ulEventId, &ucPRate, &bRatingPassed);
	if(bRatingPassed == FALSE)
	{
		HxLOG_Print("\t=> Register event ID to PVR PIN code history.\n");
		MGR_LIVE_PincodeSetRatingPassedEventId(pstContext->hSvc, pstContext->ulEventId, (HUINT8)pstContext->ulRatingValue);
	}
}

STATIC HERROR xmgr_playback_StopSubtitle_Base(xmgrPbContext_t *pstContext)
{
	HERROR				hErr;

	HxLOG_Info("xmgr_playback_StopSubtitle_Base()\n");

	if (xmgr_playback_IsSubtitlePermitted_Base(pstContext))
	{ /* AV state 이면 Subtitle을 stop한다. */
		switch (pstContext->subtitleInfo.eType)
		{
#if defined(CONFIG_MW_SUBTITLE)
		case XmgrPb_SUBTITLE_DVB:
			hErr= SVC_SBTL_StopService(pstContext->hAction, eSyncMode);
			if (ERR_OK != hErr)
			{
				HxLOG_Critical("Critical Eror!!!\n");
			}
			break;
#endif
#if defined(CONFIG_MW_TELETEXT)
		case XmgrPb_SUBTITLE_EBU:
			if (PID_NULL == pstContext->subtitleInfo.usPid)
			{
				hErr = SVC_TTX_StopService (pstContext->hAction, eSyncMode);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("SVC_TTX_StopService Error \n");
				}
			}
			HxLOG_Debug("OxSVC_TTX : [%s:%d] MW_TTX_HidePage()\n", __HxFILE__, __HxLINE__);
			break;
#endif
		default:
			return ERR_FAIL;
		}

		/* stop을 한 경우 기존 subtitle 정보를 지운다. */
		XmgrPb_SUBTITLE_INFO_Clear(&pstContext->subtitleInfo);
		return ERR_OK;
	}

	NOT_USED_PARAM(hErr);
	return ERR_FAIL;
}

STATIC HERROR xmgr_playback_StartSubtitle_Base(xmgrPbContext_t *pstContext)
{
	XmgrPb_SubtitleInfo		subtitleInfo;
	HERROR					hErr;

	/* Subtitle 허가되고 teletext 상태가 아니면 subtitle activate한다. */
	if (FALSE == xmgr_playback_IsSubtitlePermitted_Base(pstContext))
	{
		HxLOG_Error("\t=> Subtitle is not allowed.\n");
		return ERR_FAIL;
	}

	hErr = xmgr_playback_FindSubtitleInfo_Base(pstContext, &subtitleInfo);
	if (ERR_OK != hErr)
	{
#if defined(CONFIG_OP_NORDIG)
		// 선택 가능한 subtitle이 없으면 이전것은 끄는것이 맞다.
		pstContext->subtitleInfo.usPid = PID_NULL;
		xmgr_playback_StopSubtitle_Base(pstContext); /* 기존 것 stop ... */
#endif
		XmgrPb_SUBTITLE_INFO_Clear(&pstContext->subtitleInfo);
		return ERR_FAIL;
	}

	HxSTD_MemCopy(&pstContext->subtitleInfo, &subtitleInfo, sizeof(XmgrPb_SubtitleInfo)); /* Valid한 정보를 넣어서 subtitle 동작한다고 표시함... */

	if(subtitleInfo.eType == XmgrPb_SUBTITLE_DVB)
	{ /* Subtitle DVB case */
		HxLOG_Info("\t=> Starting subtitle in DVB mode(PID:0x%X).\n", subtitleInfo.usPid);

		if(pstContext->bCasStarted == TRUE)
		{
			if(eMgrPb_PlayType_Tsr == (pstContext->ePbType & 0x0000000f))
			{
				hErr = SVC_CAS_SetPid(pstContext->hAction,
									eCAS_ActionType_Tsr_Play,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									subtitleInfo.usPid);
			}
			else
			{
				hErr = SVC_CAS_SetPid(pstContext->hAction,
									eCAS_ActionType_Playback,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									subtitleInfo.usPid);
			}
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_CAS_SetPid ret :0x%X\n",hErr);
			}
		}

#if defined(CONFIG_MW_SUBTITLE)
		hErr = SVC_SBTL_StartService(pstContext->hAction,
								subtitleInfo.usPid,
								subtitleInfo.utArg.stDvbInfo.usCompositionPageId,
								subtitleInfo.utArg.stDvbInfo.usAncillaryPageId);
		if (ERR_OK != hErr)
		{
			HxLOG_Critical("SVC_SBTL_StartService : Critical Eror!!!\n");
		}
#endif
	}

	else if(subtitleInfo.eType == XmgrPb_SUBTITLE_EBU)
	{  /* Subtitle EBU case
		* if subtitle index is changed, EBU subtitle sets Teletext pid.
		*/
		HxLOG_Info("\t=> Starting subtitle in EBU mode(PID:0x%X).\n", subtitleInfo.usPid);

		if(pstContext->bCasStarted == TRUE)
		{
			if(eMgrPb_PlayType_Tsr == (pstContext->ePbType & 0x0000000f))
			{
				hErr = SVC_CAS_SetPid(pstContext->hAction,
									eCAS_ActionType_Tsr_Play,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									subtitleInfo.usPid);
			}
			else
			{
				hErr = SVC_CAS_SetPid(pstContext->hAction,
									eCAS_ActionType_Playback,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									PID_CURRENT,
									subtitleInfo.usPid);
			}
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_CAS_SetPid ret :0x%X\n",hErr);
			}
		}

#if defined(CONFIG_MW_TELETEXT)
		if (PID_NULL != subtitleInfo.usPid)
		{
			hErr = SVC_TTX_StartService (pstContext->hAction, subtitleInfo.usPid);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SVC_TTX_StartService()  failed.. \n");
			}
		}
#endif
	}

	else
	{ /* 여기 들어오면 안되는데... */
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return hErr;
}

STATIC HINT8* xmgr_playback_GetAppStateStr_Base(xmgrPb_State_e appState)
{
	switch(appState)
	{
		ENUM_TO_STR(eXmgrPbState_IDLE);
		ENUM_TO_STR(eXmgrPbState_STARTED);
		ENUM_TO_STR(eXmgrPbState_WAIT_PMT);
		ENUM_TO_STR(eXmgrPbState_STOPPED);
		ENUM_TO_STR(eXmgrPbState_WAIT_PB_LOCK);
		ENUM_TO_STR(eXmgrPbState_WAIT_PB_SETUP);
		ENUM_TO_STR(eXmgrPbState_WAIT_AV_STARTED);
		ENUM_TO_STR(eXmgrPbState_WAIT_RATING);
		ENUM_TO_STR(eXmgrPbState_LOCK_PIN);
		ENUM_TO_STR(eXmgrPbState_RATING_PIN);
		ENUM_TO_STR(eXmgrPbState_AV);
		ENUM_TO_STR(eXmgrPbState_CAS_BLOCKED);
	}

	HxLOG_Critical("\n\n");
	return "Unknown appState";
}


STATIC void xmgr_playback_DisableOutput_Base(xmgrPbContext_t *pstContext)
{
	if(pstContext->bAVHide != TRUE)
	{
		if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
		{
			SVC_AV_SetVideoHide(pstContext->ulActionId, eAvMuteFlag_View, TRUE);
			SVC_AV_SetAudioHide(pstContext->ulActionId, eAvMuteFlag_View, TRUE);
		}
	}

	pstContext->bAVHide = TRUE;
}

STATIC void xmgr_playback_EnableOutput(xmgrPbContext_t *pstContext)
{
	if(pstContext->bAVHide == TRUE)
	{
		if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
		{
			HUINT32 ulClientMask = eAvMuteFlag_View;

			SVC_AV_SetVideoHide(pstContext->ulActionId, ulClientMask, FALSE);
			SVC_AV_SetAudioHide(pstContext->ulActionId, ulClientMask, FALSE);
		}
	}

	pstContext->bAVHide = FALSE;
}

STATIC void xmgr_playback_CheckRating_Base(xmgrPbContext_t *pstContext)
{
	DbSvc_Info_t	svcInfo;
	HUINT8 		ucPRate = 0;
	HBOOL		bIsRatingPassed = FALSE;
	HUINT32		ulParentalSetup = 0;
	HERROR		hErr = ERR_OK;

	HxLOG_Info("xmgr_playback_CheckRating_Base()\n");

	switch (pstContext->ePbType & 0x0000000f)
	{
		case eMgrPb_PlayType_Tsr:
		case eMgrPb_PlayType_Chase:
#if defined(CONFIG_OP_NORDIG)
		case eMgrPb_PlayType_Normal:
#endif
			break;

		default:
			xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
			return;
	}
	HxSTD_MemSet(&svcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &svcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo failed. hSvc(%d)", pstContext->hSvc);
	}

	// Service lock 해제한 적인 있는지 확인한다.
	if( (DbSvc_GetLocked(&svcInfo) == TRUE) && (pstContext->bLockPinPassed != TRUE) )
	{ /* Lock이 걸려 있는 서비스에서 아직 lock PIN을 푼적이 없다. */
		if(MGR_LIVE_SvcLockIsPassed(pstContext->ulActionId, pstContext->hSvc) == TRUE)
		{ /* WTV등에 의하여 lock이 풀린 hostory가 있다. */
			pstContext->bLockPinPassed = TRUE;
		}
		else
		{ /* PVR history에도 푼 흔적이 없다. 그냥 lock PIN 걸어야 함. */
			xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_LOCK_PIN);
			return;
		}
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	{
		HBOOL			bPossibleSCPincode = FALSE;

		bPossibleSCPincode = xmgr_playback_CheckPossibleSCPincode_Base(pstContext->ulActionId);
		if(bPossibleSCPincode == TRUE)
		{
			pstContext->bSCPinCodeValid = SVC_CAS_CheckSmartCardValidity(eDxCAS_GROUPID_NA);
		}
		else
		{
			pstContext->bSCPinCodeValid = FALSE;
		}
	}
#endif

	if( (pstContext->bSCPinCodeValid == TRUE) && (pstContext->bSCPinPassed == TRUE) )
	{ /* Smart card rating PIN 통과 되어 있음. OK to watch */
		xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
		xmgr_playback_SetPinPassedEventId_Base(pstContext); /* 여기서 기록해야 함. */
	}
	else if( (pstContext->bSCPinCodeValid != TRUE) && (pstContext->bRatingPinPassed == TRUE) )
	{ /* Rating PIN 통과 되어 있음. OK to watch */
		xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
		xmgr_playback_SetPinPassedEventId_Base(pstContext); /* 여기서 기록해야 함. */
	}
	else
	{ /* rating PIN 통과되지 않았음. */
		ulParentalSetup = MGR_PG_GetRatingFromSetup();
		if( (ulParentalSetup == eDxRATING_VIEW_ALL))
		{
			/* No parental check. OK to watch */
			xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
		}
		else if(pstContext->bRatingValid != TRUE)
		{
			/* 아직 rating check할 정보가 부족함. CAS에서 계약 사항을 parental 보다 먼저 확인 해야 하고,
				EIT등에서 rating 정보를 받아오던가, EIT timeout이 나야 함. */
			xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_WAIT_RATING);
		}
		else if(MGR_PG_IsRatingOk(pstContext->ulActionId, pstContext->hSvc, DbSvc_GetDeliType(svcInfo), ulParentalSetup, pstContext->ulRatingValue) == TRUE)
		{ /* OK to watch */
			xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
		}
		else
		{ /* Should check PIN code */
#if defined(CONFIG_APCORE_EVENT_BASED_RATING)
			if( (DbSvc_GetRatingPassedEvent(&svcInfo) != (HUINT32)-1) && /* PIN code pass된 event ID가 등록되어 있고... */
				(pstContext->ulEventId == DbSvc_GetRatingPassedEvent(&svcInfo)) /* 현재의 event ID 여전히 등록된 event ID를 유지하고 있는 경우. */
			)
			{ /* OK to watch */
				HxLOG_Print("OK to watch for the same event ID as the previous PIN passed one.\n");
				xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
			}
			else
#endif
			{ /* PIN passed event ID가 아직 등록되어 있지 않거나 등록된 event ID가 다른 경우... */
				// WTV등에서 해당 event 를 해제한 적인 있는지 확인한다.
				MGR_LIVE_PincodeIsEventIdRatingPassed(pstContext->hSvc, pstContext->ulEventId, &ucPRate, &bIsRatingPassed);
				if(bIsRatingPassed == TRUE)
				{
					HxLOG_Print("OK to watch by PVR PIN code history.\n");
					pstContext->bRatingPinPassed = TRUE;
					pstContext->bSCPinPassed = TRUE;
					xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
				}
				else
				{
					HxLOG_Print("First event ID or NOT registared event ID. Block AV.\n");
					xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_RATING_PIN);
				}
			}
		}
	}
}


STATIC void xmgr_playback_StartPid_Base(xmgrPbContext_t *pstContext)
{
	HERROR				hErr = ERR_OK;
	SvcMeta_Record_t		stRecordInfo;

	HxLOG_Info("xmgr_playback_StartPid_Base()\n");

#if defined(CONFIG_MW_CAS_CONAX)
	if(pstContext->svcInfo.eCasType & eDxCAS_TYPE_CONAX)
#else
	hErr = SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecordInfo);
	if((hErr == ERR_OK) && (stRecordInfo.u1cScrambled == 1))
#endif
	{
		/* CAS 를 다시 시작 시킨다. */
		if(eMgrPb_PlayType_Tsr == (pstContext->ePbType & 0x0000000f))
		{
			hErr = MGR_CAS_StartService(pstContext->hAction, HANDLE_NULL, eCAS_ActionType_Tsr_Play, eDxCAS_GROUPID_NONE, NULL);
			/* CAS에게 PID 반영을 통보하고 */
			hErr = SVC_CAS_SetPid(pstContext->hAction,
								eCAS_ActionType_Tsr_Play,
								pstContext->svcInfo.usVideoPid,
								pstContext->svcInfo.usAudioPid,
								pstContext->svcInfo.usAudioAuxPid,
								pstContext->svcInfo.usDolbyPid,
								pstContext->svcInfo.usTtxPid,
								PID_CURRENT);
		}
		else
		{
			hErr = MGR_CAS_StartService(pstContext->hAction, HANDLE_NULL, eCAS_ActionType_Playback, eDxCAS_GROUPID_NONE, NULL);
			/* CAS에게 PID 반영을 통보하고 */
			hErr = SVC_CAS_SetPid(pstContext->hAction,
								eCAS_ActionType_Playback,
								pstContext->svcInfo.usVideoPid,
								pstContext->svcInfo.usAudioPid,
								pstContext->svcInfo.usAudioAuxPid,
								pstContext->svcInfo.usDolbyPid,
								pstContext->svcInfo.usTtxPid,
								PID_CURRENT);
		}
		pstContext->bCasStarted = TRUE;
	}
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	else if((hErr == ERR_OK) && (stRecordInfo.bCamNeedContent == TRUE))
	{
		SvcCas_CamStartData_t	stCamStartData;

		HxLOG_Info("Playback --> CI+ 1.3 content...\n");

		HxSTD_MemSet(&stCamStartData, 0, sizeof(SvcCas_CamStartData_t));
		HxSTD_MemCopy(&stCamStartData.aucCAMID, stRecordInfo.aucBindedCamId, SVC_META_CAMID_LEN);

		if(eMgrPb_PlayType_Tsr == (pstContext->ePbType & 0x0000000f))
		{
			hErr = MGR_CAS_StartService(pstContext->hAction, HANDLE_NULL, eCAS_ActionType_Tsr_Play, eDxCAS_GROUPID_CAM, (void *)&stCamStartData);
			/* CAS에게 PID 반영을 통보하고 */
			hErr = SVC_CAS_SetPid(pstContext->hAction,
								eCAS_ActionType_Tsr_Play,
								pstContext->svcInfo.usVideoPid,
								pstContext->svcInfo.usAudioPid,
								pstContext->svcInfo.usAudioAuxPid,
								pstContext->svcInfo.usDolbyPid,
								pstContext->svcInfo.usTtxPid,
								PID_CURRENT);
		}
		else
		{
			hErr = MGR_CAS_StartService(pstContext->hAction, HANDLE_NULL, eCAS_ActionType_Playback, eDxCAS_GROUPID_CAM, (void *)&stCamStartData);
			/* CAS에게 PID 반영을 통보하고 */
			hErr = SVC_CAS_SetPid(pstContext->hAction,
								eCAS_ActionType_Playback,
								pstContext->svcInfo.usVideoPid,
								pstContext->svcInfo.usAudioPid,
								pstContext->svcInfo.usAudioAuxPid,
								pstContext->svcInfo.usDolbyPid,
								pstContext->svcInfo.usTtxPid,
								PID_CURRENT);
		}
		pstContext->bCasStarted = TRUE;
	}
#endif
	else
	{
//	Irdeto PVR의 경우 Scramble Bit와는 상관없이 무조건 PVR서비스를 열어야 한다.
//	또한 PB가 Start되기 전에 CAS를 열어서 Play가능 여부를 봐야한다.
//	eSEVT_AV_SESSION_STARTED(Setup Player후에 오는 Event)에서 MW_CAS_StartService를 수행하고
//	CAS가 주는 Event에 따라서 Start유무를 결정한다.
#if	!defined(CONFIG_MW_CAS_IRDETO_PVR)
		pstContext->bCasStarted = FALSE;
#endif
	}
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

	/* Rating check로 넘어감 */
	xmgr_playback_CheckRating_Base(pstContext);
}

STATIC void xmgr_playback_CheckSbtlAllowed_Base(xmgrPbContext_t *pstContext)
{
	HxLOG_Info("xmgr_playback_CheckSbtlAllowed_Base() bSubtitleAllowed[%d], ucSubttlIdx[0x%x]\n", pstContext->bSubtitleAllowed, pstContext->svcInfo.ucSubttlIdx);

	if(pstContext->bSubtitleAllowed == TRUE)
	{
		HxLOG_Print("is already allowed \n");
		return;
	}

	if(pstContext->svcInfo.ucSubttlIdx < SvcSi_MAX_SUBTITLE_NUM)
	{
		pstContext->bSubtitleAllowed = TRUE;
	}

	// 160225 - Live에서 HOH관련 0xfe index 추가로 인한 if 구문 추가.
	if (pstContext->svcInfo.ucSubttlIdx == 0xff || pstContext->svcInfo.ucSubttlIdx == 0xfe)
	{
		SvcSi_SubtitleList_t stSubtitleList;
		HERROR hErr;

		HxSTD_MemSet( &stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t) );
		hErr = SVC_SI_GetSubtitleList (pstContext->ulActionId, pstContext->svcInfo.usSvcId, &stSubtitleList);

		HxLOG_Print("META subttl num[%d] PMT Subtitle NUM[%d]\n", pstContext->svcInfo.stSbtlInfoList.ulItemNum, stSubtitleList.ulSubtitleNum);
		if((pstContext->svcInfo.stSbtlInfoList.ulItemNum > 0) || (hErr == ERR_OK && stSubtitleList.ulSubtitleNum > 0))
		{
			pstContext->bSubtitleAllowed = TRUE;
		}
	}
}

STATIC void xmgr_playback_ChangeSubtitle_Base(xmgrPbContext_t *pstContext)
{
	SvcMeta_Service_t		newSvcInfo;
	XmgrPb_SubtitleInfo		newSubtitleInfo;
	HERROR					hErr;

	HxLOG_Info("xmgr_playback_ChangeSubtitle_Base()\n");

	/* New index를 알아보기 위해 Service info를 다시 찾아 봄. */
	hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, &newSvcInfo);
	pstContext->svcInfo.ucSubttlIdx = (HUINT32)newSvcInfo.ucSubttlIdx;

	XmgrPb_SUBTITLE_INFO_Clear(&newSubtitleInfo);
	if (xmgr_playback_IsSubtitlePermitted_Base(pstContext))
	{
		/* New index로 new subtitle info를 다시 찾아 봄. */
		hErr = xmgr_playback_FindSubtitleInfo_Base(pstContext, &newSubtitleInfo);
		if(hErr != ERR_OK)
		{
#if defined(CONFIG_OP_NORDIG)
			// 선택 가능한 subtitle이 없으면 이전것은 끄는것이 맞다.
			pstContext->subtitleInfo.usPid = PID_NULL;
			xmgr_playback_StopSubtitle_Base(pstContext); /* 기존 것 stop ... */
			XmgrPb_SUBTITLE_INFO_Clear(&pstContext->subtitleInfo);
			return;
#else
			XmgrPb_SUBTITLE_INFO_Clear(&newSubtitleInfo);
#endif
		}

		if (XmgrPb_SUBTITLE_INFO_Comp(&newSubtitleInfo, &pstContext->subtitleInfo) != 0)
		{ /* 기존 정보와 차이가 남.  */
			xmgr_playback_StopSubtitle_Base(pstContext); /* 기존 것 stop ... */
			xmgr_playback_StartSubtitle_Base(pstContext);
		}
	}
}

// 재생되는 파일의 Front Panel LED(ICON) Display.
STATIC void xmgr_playback_SetPanelLedDisplay_Base(xmgrPbContext_t *pstContext)
{
	SvcMeta_Service_t svcInfo;
	if(pstContext->ulActionId == MGR_ACTION_GetMainActionId())
	{
		// Standby 에서 Thumbnail 을 만들기 위해 시작한 playback 이 아니면 Panel 에 Display
		if ( MGR_ACTION_GetState() != eAmState_ACTIVE_STANDBY )
		{
			HERROR hErr;

			// STB 에서는 Dolby 스트림 재생시 화면 우측 상단에 dolby 아이콘을 출력하도록 한다.
			// 이유는 Dolby 의 경우 bypass 혹은 디코더 성능상 소리가 나오지 않을 경우를 대비한 STB 공통 시나리오임.
			hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, &svcInfo); /* New PMT 반영된 service info 다시 읽음. */
			if (hErr != ERR_OK)
			{
				HxLOG_Error("SVC_META_GetServiceInfo failed. cid(%d) \n",pstContext->ulContentId);
			}

			if(svcInfo.eSvcType == eDxSVC_TYPE_RADIO)
			{
				SVC_SYS_SetLedOnOff(eDxLED_ID_TV, FALSE);
				SVC_SYS_SetLedOnOff(eDxLED_ID_RADIO, TRUE);
			}
			else
			{
				SVC_SYS_SetLedOnOff(eDxLED_ID_TV, TRUE);
				SVC_SYS_SetLedOnOff(eDxLED_ID_RADIO, FALSE);
			}
			if(svcInfo.eVideoType == eDxVIDEO_TYPE_HD)
			{
				SVC_SYS_SetLedOnOff(eDxLED_ID_HD, TRUE);
			}
			else
			{
				SVC_SYS_SetLedOnOff(eDxLED_ID_HD, FALSE);
			}

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_HDR3000T)
			if	(svcInfo.bDolbyFlag == TRUE)
			{
				SVC_SYS_SetLedOnOff(eDxLED_ID_DOLBY, TRUE);
				if(svcInfo.eDolbyCodec == eDxAUDIO_CODEC_DOLBY_AC3P)
				{
					SVC_SYS_SetLedOnOff(eDxLED_ID_DOLBY_PLUS, TRUE);
				}
			}
			else
			{
				SVC_SYS_SetLedOnOff(eDxLED_ID_DOLBY, FALSE);
			}
#endif
		}
	}
}

STATIC HERROR xmgr_playback_UpdateMetaInfoPID_Base(xmgrPbContext_t *pstContext, HBOOL *pbChanged)
{
	Handle_t			hAction = HANDLE_NULL;
	SvcMeta_Service_t	stOrgSvcMetaInfo, stNewSvcMetaInfo;
	HBOOL				bChanged = FALSE;
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulContentId = 0;

	if(NULL == pstContext || NULL == pbChanged)
	{
		HxLOG_Error("Check the param.\n");
		return hErr;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);

	HxSTD_MemSet(&stOrgSvcMetaInfo, 0, sizeof(SvcMeta_Service_t));
	HxSTD_MemSet(&stNewSvcMetaInfo, 0, sizeof(SvcMeta_Service_t));

	*pbChanged = FALSE;
	hErr = SVC_META_GetContentIdByhAction(hAction, &ulContentId);
	if (hErr == ERR_OK)
	{
		hErr = SVC_META_GetServiceInfo(ulContentId, &stOrgSvcMetaInfo);
	}
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetServiceInfo Error (ulContentId : %d )\n", ulContentId);
		return hErr;
	}

	hErr = MGR_PLAYBACK_UpdateServiceInfo(pstContext->ulActionId, stOrgSvcMetaInfo.usSvcId, &stOrgSvcMetaInfo, &stNewSvcMetaInfo, &bChanged);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_PLAYBACK_UpdateServiceInfo() failed.. : hService(0x%X)", pstContext->hSvc);
		return hErr;
	}

	if (bChanged == TRUE)
	{
		HxLOG_Warning("PID Info ==========\n");
		HxLOG_Warning("usPmtPid( %d -> %d )\n", pstContext->svcInfo.usPmtPid, stNewSvcMetaInfo.usPmtPid);
		HxLOG_Warning("usPcrPid( %d -> %d )\n", pstContext->svcInfo.usPcrPid, stNewSvcMetaInfo.usPcrPid);
		HxLOG_Warning("usTtxPid( %d -> %d )\n", pstContext->svcInfo.usTtxPid, stNewSvcMetaInfo.usTtxPid);
		HxLOG_Warning("usCaptionPid( %d -> %d )\n", pstContext->svcInfo.usCaptionPid, stNewSvcMetaInfo.usCaptionPid);
		HxLOG_Warning("usCaptionPid( %d -> %d )\n", pstContext->svcInfo.usCaptionPid, stNewSvcMetaInfo.usCaptionPid);
		HxLOG_Warning("usVideoPid( %d -> %d )\n", pstContext->svcInfo.usVideoPid, stNewSvcMetaInfo.usVideoPid);
		HxLOG_Warning("eVideoCodec( %d -> %d )\n", pstContext->svcInfo.eVideoCodec, stNewSvcMetaInfo.eVideoCodec);
		HxLOG_Warning("usAudioPid( %d -> %d )\n", pstContext->svcInfo.usAudioPid, stNewSvcMetaInfo.usAudioPid);
		HxLOG_Warning("eAudioCodec( %d -> %d )\n", pstContext->svcInfo.eAudioCodec, stNewSvcMetaInfo.eAudioCodec);
		HxLOG_Warning("usAudioAuxPid( %d -> %d )\n", pstContext->svcInfo.usAudioAuxPid, stNewSvcMetaInfo.usAudioAuxPid);
		HxLOG_Warning("eAudioAuxCodec( %d -> %d )\n", pstContext->svcInfo.eAudioAuxCodec, stNewSvcMetaInfo.eAudioAuxCodec);
		HxLOG_Warning("usDolbyPid( %d -> %d )\n", pstContext->svcInfo.usDolbyPid, stNewSvcMetaInfo.usDolbyPid);
		HxLOG_Warning("eDolbyCodec( %d -> %d )\n", pstContext->svcInfo.eDolbyCodec, stNewSvcMetaInfo.eDolbyCodec);
		HxLOG_Warning("bDolbyFlag( %d -> %d )\n", pstContext->svcInfo.bDolbyFlag, stNewSvcMetaInfo.bDolbyFlag);
		HxLOG_Warning("====================\n\n\n");

		*pbChanged = TRUE;
		hErr = SVC_META_SetServiceInfo(ulContentId, &stNewSvcMetaInfo, TRUE);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_META_SetServiceInfo() failed.. : hService(0x%X)", pstContext->hSvc);
			return hErr;
		}
	}

	hErr = ERR_OK;
	return hErr;
}

STATIC void xmgr_playback_CheckAvPidAndChangePid(xmgrPbContext_t *pstContext, SvcMeta_Service_t *pNewSvcMetaInfo)
{
	HERROR hErr = ERR_FAIL;
	HBOOL bVideoChanged = FALSE;
	HBOOL bAudioChanged = FALSE;

	if(NULL == pstContext || NULL == pNewSvcMetaInfo)
	{
		HxLOG_Error("Error!!\n");
		return;
	}

	/********************** AV PID or user setting 변경 검사 *********************/
	if(pstContext->svcInfo.usPcrPid != pNewSvcMetaInfo->usPcrPid)
	{ /* PCR이 변경되면 audio, video 모두 변경된 것으로 간주한다. */
		HxLOG_Print("\t=> PCR PID changed from 0x%X to 0x%X\n", pstContext->svcInfo.usPcrPid, pNewSvcMetaInfo->usPcrPid);

		bVideoChanged = TRUE;
		bAudioChanged = TRUE;
	}
	else
	{ /* Video, Audio 변경 */
		if( (pstContext->svcInfo.usVideoPid != pNewSvcMetaInfo->usVideoPid) || (pstContext->svcInfo.eVideoCodec != pNewSvcMetaInfo->eVideoCodec) )
		{
			HxLOG_Print("\t=> VIDEO PID changed from 0x%X to 0x%X\n", pstContext->svcInfo.usVideoPid, pNewSvcMetaInfo->usVideoPid);
			bVideoChanged = TRUE;
		}

		if( (pstContext->svcInfo.usAudioPid != pNewSvcMetaInfo->usAudioPid) ||
			(pstContext->svcInfo.usAudioAuxPid != pNewSvcMetaInfo->usAudioAuxPid) ||
			(pstContext->svcInfo.usDolbyPid != pNewSvcMetaInfo->usDolbyPid) ||
			(pstContext->svcInfo.eAudioCodec != pNewSvcMetaInfo->eAudioCodec) ||
			(pstContext->svcInfo.eDolbyCodec != pNewSvcMetaInfo->eDolbyCodec) ||
			(pstContext->svcInfo.eAudioAuxCodec != pNewSvcMetaInfo->eAudioAuxCodec) ||
			(pstContext->svcInfo.bDolbyFlag != pNewSvcMetaInfo->bDolbyFlag) ||
			(pstContext->svcInfo.eSoundMode != pNewSvcMetaInfo->eSoundMode) ||
			(pstContext->svcInfo.eDualMono != pNewSvcMetaInfo->eDualMono))
		{
			HxLOG_Print("\t=> Main PID changed from 0x%X to 0x%X\n", pstContext->svcInfo.usAudioPid, pNewSvcMetaInfo->usAudioPid);
			HxLOG_Print("\t=> Aux PID changed from 0x%X to 0x%X\n", pstContext->svcInfo.usAudioAuxPid, pNewSvcMetaInfo->usAudioAuxPid);
			HxLOG_Print("\t=> Dolby PID changed from 0x%X to 0x%X\n", pstContext->svcInfo.usDolbyPid, pNewSvcMetaInfo->usDolbyPid);
			HxLOG_Print("\t=> Main Codec changed from 0x%X to 0x%X\n", pstContext->svcInfo.eAudioCodec, pNewSvcMetaInfo->eAudioCodec);
			HxLOG_Print("\t=> Aux Codec changed from 0x%X to 0x%X\n", pstContext->svcInfo.eAudioAuxCodec, pNewSvcMetaInfo->eAudioAuxCodec);
			HxLOG_Print("\t=> Dolby Flag changed from %d to %d\n", pstContext->svcInfo.bDolbyFlag, pNewSvcMetaInfo->bDolbyFlag);
			HxLOG_Print("\t=> SoundMode changed from %d to %d\n", pstContext->svcInfo.eSoundMode, pNewSvcMetaInfo->eSoundMode);
			HxLOG_Print("\t=> DualMono changed from %d to %d\n", pstContext->svcInfo.eDualMono, pNewSvcMetaInfo->eDualMono);
			bAudioChanged = TRUE;
		}
	}

	if( (bAudioChanged == TRUE) || (bVideoChanged == TRUE) )
	{
		/* 최신 PID 정보로 업데이트 */
		pstContext->svcInfo.usAudioPid = pNewSvcMetaInfo->usAudioPid;
		pstContext->svcInfo.usAudioAuxPid = pNewSvcMetaInfo->usAudioAuxPid;
		pstContext->svcInfo.usDolbyPid = pNewSvcMetaInfo->usDolbyPid;
		pstContext->svcInfo.usVideoPid = pNewSvcMetaInfo->usVideoPid;
		pstContext->svcInfo.usPcrPid = pNewSvcMetaInfo->usPcrPid;
		pstContext->svcInfo.eAudioCodec = pNewSvcMetaInfo->eAudioCodec;
		pstContext->svcInfo.eDolbyCodec = pNewSvcMetaInfo->eDolbyCodec;
		pstContext->svcInfo.eAudioAuxCodec = pNewSvcMetaInfo->eAudioAuxCodec;
		pstContext->svcInfo.eVideoCodec = pNewSvcMetaInfo->eVideoCodec;
		pstContext->svcInfo.bDolbyFlag = pNewSvcMetaInfo->bDolbyFlag;
		pstContext->svcInfo.eSoundMode = pNewSvcMetaInfo->eSoundMode;
		pstContext->svcInfo.eDualMono = pNewSvcMetaInfo->eDualMono;

		SVC_META_SetServiceInfo(pstContext->ulContentId, &pstContext->svcInfo, TRUE);

		if(pstContext->bCasStarted == TRUE)
		{
			/* CAS에 PID를 다시 셋팅해야 한다. */
			if(eMgrPb_PlayType_Tsr == (pstContext->ePbType & 0x0000000f))
			{
				hErr = SVC_CAS_SetPid(pstContext->hAction,
									eCAS_ActionType_Tsr_Play,
									pNewSvcMetaInfo->usVideoPid,
									pNewSvcMetaInfo->usAudioPid,
									pNewSvcMetaInfo->usAudioAuxPid,
									pNewSvcMetaInfo->usDolbyPid,
									PID_CURRENT,
									PID_CURRENT);
			}
			else
			{
				hErr = SVC_CAS_SetPid(pstContext->hAction,
									eCAS_ActionType_Playback,
									pNewSvcMetaInfo->usVideoPid,
									pNewSvcMetaInfo->usAudioPid,
									pNewSvcMetaInfo->usAudioAuxPid,
									pNewSvcMetaInfo->usDolbyPid,
									PID_CURRENT,
									PID_CURRENT);
			}
		}

		// 최초 재생시 menu 설정값을 읽어오지 못하므로, bFirstPmt가 FALSE일 경우는 ChangePid 허용.
		if(pstContext->ePbState >= eXmgrPbState_WAIT_RATING || pstContext->bFirstPmt == FALSE)
		{ /* AV state 였다면 AV를 다시 시작한다. */

			HxLOG_Print("\t=> Restart AV.\n");

			if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
			{
				hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, pNewSvcMetaInfo->eDualMono);
				if (hErr)
				{
					HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
				}

#if defined(SvcAv_START_SERVICE_NEW)
				hErr = SVC_AV_ChangePid(pstContext->ulActionId,
										pNewSvcMetaInfo->usAudioPid,
										pNewSvcMetaInfo->usAudioAuxPid,
										pNewSvcMetaInfo->usVideoPid,
										pNewSvcMetaInfo->usPcrPid,
										pNewSvcMetaInfo->eAudioCodec,
										pNewSvcMetaInfo->eAudioAuxCodec,
										pNewSvcMetaInfo->eVideoCodec
										);
#else
				hErr = SVC_AV_ChangePid(pstContext->ulActionId,
										pNewSvcMetaInfo->usAudioPid,
										pNewSvcMetaInfo->usAudioAuxPid,
										pNewSvcMetaInfo->usDolbyPid,
										pNewSvcMetaInfo->usVideoPid,
										pNewSvcMetaInfo->usPcrPid,
										pNewSvcMetaInfo->eAudioCodec,
										pNewSvcMetaInfo->eAudioAuxCodec,
										pNewSvcMetaInfo->eDolbyCodec, 		//2009.03.23 추가.
										pNewSvcMetaInfo->eVideoCodec,
										pNewSvcMetaInfo->bDolbyFlag,
										pNewSvcMetaInfo->eSoundMode
										);
#endif
				if(ERR_OK!=hErr)		HxLOG_Critical("Critical Eror!!!\n");
			}
		}
		else
		{
			/* 다른 state에서는 변동없다. */
			HxLOG_Print("\t=> Not in eXmgrPbState_AV. Don't restart AV.\n");
		}
	}

}

STATIC void xmgr_playback_CheckMetaInfoAndChangePid(xmgrPbContext_t *pstContext)
{
	HERROR hErr = ERR_FAIL;
	HBOOL bChanged = FALSE;
	SvcMeta_Service_t stNewSvcMetaInfo = {0, };

	// User Select 된경우는 제외.
	if( pstContext->svcInfo.bAudUserSetFlag == FALSE )
	{
		// menu 설정등 우선순위에 의해 Meta 정보가 update된다.
		hErr = xmgr_playback_UpdateMetaInfoPID_Base(pstContext, &bChanged);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("xmgr_playback_UpdateMetaInfoPID_Base failed..\n");
			return;
		}
	}

	if (bChanged == TRUE)
	{
		/* Service info를 다시 찾아 봄. */
		HxSTD_MemSet(&stNewSvcMetaInfo, 0, sizeof(SvcMeta_Service_t));
		hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, &stNewSvcMetaInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_META_GetServiceInfo Error (ulContentId : %d )\n", pstContext->ulContentId);
			return;
		}
		xmgr_playback_CheckAvPidAndChangePid(pstContext, &stNewSvcMetaInfo);
	}
}

STATIC void xmgr_playback_ChangePid_Base(xmgrPbContext_t *pstContext)
{
	HBOOL			bMetaUpdated = FALSE;
	SvcMeta_Service_t newSvcInfo = {0, };
	HERROR			hErr = ERR_FAIL;

	HxLOG_Info("xmgr_playback_ChangePid_Base()\n");

	/* Context->svcInfo.bAudUserSetFlag를 신뢰하면 안됨!!! Service info를 다시 찾아 봄. */
	HxSTD_MemSet(&newSvcInfo, 0, sizeof(SvcMeta_Service_t));
	hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, &newSvcInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetServiceInfo Error (ulContentId : %d )\n", pstContext->ulContentId);
	}
	if(newSvcInfo.bAudUserSetFlag == FALSE)
	{
		hErr = xmgr_playback_UpdateMetaInfoPID_Base(pstContext, &bMetaUpdated);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("xmgr_playback_UpdateMetaInfoPID_Base failed..\n");
		}
		if(bMetaUpdated == TRUE)
		{
			/* Service info를 다시 찾아 봄. Menu 설정등의 우선순위에 의해 다시 설정 됨.*/
			HxSTD_MemSet(&newSvcInfo, 0, sizeof(SvcMeta_Service_t));
			hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, &newSvcInfo);
		}
	}

	xmgr_playback_CheckAvPidAndChangePid(pstContext, &newSvcInfo);

	if(pstContext->ePbState == eXmgrPbState_AV)
	{ /* AV state에서만 check 해야 함. PMT valid가 아니면 no AV 안그리기 때문에 PID 변경하고 상관없이 check해야 함.*/
#if defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
		if (pstContext->uiState != eMgrPb_CAS)
		{
			xmgr_playback_CheckAV_Base(pstContext);
		}
#else
		xmgr_playback_CheckAV_Base(pstContext);
#endif
	}

	// Check if subtitle is changed or not - Don't believe index
	xmgr_playback_ChangeSubtitle_Base(pstContext);

	/* 혹시 video PID가 살아나거나 죽었을까봐 다시 한번 체크해 본다. */
	xmgr_playback_CheckRadioBackground_Base(pstContext);

	xmgr_playback_SetPanelLedDisplay_Base(pstContext);

}

STATIC HERROR xmgr_playback_ChangeSubtitleWithSbtlIndex_Base(xmgrPbContext_t *pstContext, HUINT32 ulIndex)
{
	XmgrPb_SubtitleInfo 	newSubtitleInfo;
	HERROR					hErr = ERR_FAIL;

	if (NULL == pstContext)
		return ERR_FAIL;

	if (ulIndex == pstContext->svcInfo.ucSubttlIdx)
	{	// not changed
		HxLOG_Print("same sbtitle index (%d)\n", ulIndex);
		return ERR_FAIL;
	}

	XmgrPb_SUBTITLE_INFO_Clear(&newSubtitleInfo);
	if (xmgr_playback_IsSubtitlePermitted_Base(pstContext))
	{	/* New index로 new subtitle info를 다시 찾아 봄. */
		pstContext->svcInfo.ucSubttlIdx = ulIndex;
		hErr = xmgr_playback_FindSubtitleInfo_Base(pstContext, &newSubtitleInfo);
		if (ERR_OK != hErr)
		{
#if defined(CONFIG_OP_NORDIG)
			pstContext->subtitleInfo.usPid = PID_NULL;
			xmgr_playback_StopSubtitle_Base(pstContext); /* 기존 것 stop ... */
			XmgrPb_SUBTITLE_INFO_Clear(&pstContext->subtitleInfo);
			return ERR_FAIL;
#else
			XmgrPb_SUBTITLE_INFO_Clear(&newSubtitleInfo);
#endif
		}

		if (0 != XmgrPb_SUBTITLE_INFO_Comp(&newSubtitleInfo, &pstContext->subtitleInfo))
		{ /* 기존 정보와 차이가 남.  */
			hErr = xmgr_playback_StopSubtitle_Base(pstContext); /* 기존 것 stop ... */
			hErr = xmgr_playback_StartSubtitle_Base(pstContext);

			if (ERR_OK == hErr)
			{
				pstContext->svcInfo.bSubttlUserFlag = TRUE;
				SVC_META_SetServiceInfo(pstContext->ulContentId, &pstContext->svcInfo, TRUE);
			}
		}
	}

	return hErr;
}

STATIC HERROR xmgr_playback_ChangeAudioWithAudInfo_Base(xmgrPbContext_t *pstContext, HUINT32 ulIndex, DxStereoSelect_e	eSoundMode)
{
	HERROR		hErr		= ERR_FAIL;
	HUINT32 	ulActionId	= 0;
	HUINT32 	ulSelectIdx	= ulIndex;
	DbSvc_Info_t	stSvcInfo;

	if( NULL == pstContext )
		return ERR_FAIL;

	/*prefer*/
	ulActionId = pstContext->ulActionId;

	HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);

	/*get*/
	{
		HINT32 nMinTrack=0;
		SvcSi_AudioList_t stAudioList;

		HxSTD_MemSet( &stAudioList, 0, sizeof(SvcSi_AudioList_t) );
		hErr = SVC_SI_GetAudioList (ulActionId, DbSvc_GetSvcId(&stSvcInfo), &stAudioList);
		if( ERR_OK == hErr )
		{
			nMinTrack = (stAudioList.nAudio<SvcSi_MAX_AUDIO_ES_NUM)? stAudioList.nAudio : SvcSi_MAX_AUDIO_ES_NUM;

			if( nMinTrack > ulSelectIdx )
			{
//				stAudioList[ulSelectIdx].usPid
			}
			else
			{
				/*out of index*/
				hErr = ERR_FAIL;
			}

			SVC_SI_FreeAudioList(&stAudioList); //free
		}
	}

	if( ERR_OK != hErr )
		return hErr;

	return hErr;
}

STATIC void xmgr_playback_StopTeletext_Base(xmgrPbContext_t *pstContext)
{
	HxLOG_Info("xmgr_playback_StopTeletext_Base()\n");

#if defined(CONFIG_MW_TELETEXT)
	if(pstContext->bTtxVbiStarted == TRUE)
	{
		HERROR	hErr;
		hErr = SVC_TTX_StopService(pstContext->hAction, eSyncMode);
		if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
		pstContext->bTtxVbiStarted = FALSE;
	}
#endif
}

// 재생되는 파일의 Front Panel String Display.
STATIC void xmgr_playback_SetPanelStringDisplay_Base(xmgrPbContext_t *pstContext)
{
	if(pstContext->ulActionId == MGR_ACTION_GetMainActionId())
	{
		// TODO:
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_HDR3000T)
		SvcMeta_Record_t pstInfo;

		if(SVC_META_GetRecordInfo(pstContext->ulContentId, &pstInfo)== ERR_OK)
		{
			if ( MGR_ACTION_GetState() != eAmState_ACTIVE_STANDBY )
			{
				SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, pstInfo.aucDisplayEvtName, eDxPANEL_ALIGN_LEFT);
			}
		}
#endif
	}
}

STATIC HERROR xmgr_playback_StartReservedCtrl_Base (xmgrPbContext_t *pstContext)
{
	if (xmgr_playback_IsTrickableState_Base (pstContext) == FALSE)
	{
		HxLOG_Error ("Not available yet (%d)\n", pstContext->ulActionId);
		return ERR_FAIL;
	}

	if (TRUE == pstContext->bSpeedReserved)
	{
		BUS_SetTimer (PB_RESERVED_TRICK_START_TIMER_ID, PB_RESERVED_TRICK_START_TIMEOUT);
	}

	if (TRUE == pstContext->bJumpReserved)
	{
		BUS_SetTimer (PB_RESERVED_JUMP_START_TIMER_ID, PB_RESERVED_JUMP_START_TIMEOUT);
	}

	return ERR_OK;
}

STATIC void xmgr_playback_ProcessParentalRate_Base(xmgrPbContext_t *pstContext, HBOOL bFindRate)
{
	if (NULL == pstContext)
		return;

// TODO: 정리후 삭제 할 CONFIG_OP_XXX
// CAS 관련 내용이라서 참고용으로 남겨둠..
#if defined(CONFIG_OP_ZIGGO) || defined(CONFIG_OP_RUSSIA_TVT)
	// ziggo spec 에서는 해당 국가와 일치하는 것만 pc 로 사용하며, 해당 국가가 없거나, PC 값이 없는 경우는 모두 lock 이라고 한다.
	if(bFindRate == FALSE)
	{
#if defined(CONFIG_MW_CAS_IRDETO)
		XMGR_CAS_IrSetSiTableStateTimeout(TRUE);
#endif
		pstContext->ulRatingValue = MGR_PG_GetRatingForNoEit(pstContext->svcInfo.eSvcType, bFindRate);
	}
	else
	{
#if defined(CONFIG_MW_CAS_IRDETO)
		XMGR_CAS_IrSetSiTableStateTimeout(FALSE);
#endif
	}
#endif
}

//FR, PAUSE, Slow 로 Chase/Tsr Playback 시 LastPosition 의 event 정보를 setting 한다.
STATIC void xmgr_playback_GetEitInfo_Base(xmgrPbContext_t *pstContext)
{
	HERROR			hErr;
	SvcMeta_Event_t 	stEvtInfo = {0, };
	HUINT32			ulEventId = (HUINT32)-1;
	HUINT8			idx;
	HBOOL			bFindRate = FALSE;
	HUINT8			szIso3166Code[LANG_639_CODE_MAX+1];

	if ((pstContext->eFirstTrickState == eMgrPb_TickState_FR) ||
		(pstContext->eFirstTrickState == eMgrPb_TickState_SLOW) ||
		(pstContext->eFirstTrickState == eMgrPb_TickState_PAUSE) )
	{
		Handle_t hRecAction = HANDLE_NULL;
		hErr = SVC_META_GetRecActionByContentId (pstContext->ulContentId, &hRecAction);
		if ((ERR_OK != hErr) || (HANDLE_NULL == hRecAction))
		{
			HxLOG_Error ("SVC_META_GetRecActionByContentId. Error \n");
			return ;
		}

		/* Rating value를 알아본다. */
//		hErr = SVC_PVR_GetEventByTime(pstContext->hAction, stInfo.ulEndTime - 1, &stEvtInfo);
	}
	else
	{
		/* Rating value를 알아본다. */
//		hErr = SVC_PVR_GetEventByTime(pstContext->hAction, pstContext->stSetup.ulStartTime, &stEvtInfo);
		HxLOG_Debug ("ulStartTime (%d)\n", pstContext->stSetup.ulStartTime);
	}

	ulEventId = (HUINT32)-1;
	pstContext->ulRatingValue = 0;

	{/* Event ID를 등록한다. */
//		ulEventId = (HUINT32)stEvtInfo.nEventId;	// 위 주석처리된 SVC_PVR_GetEventByTime() 코드로 인해 불필요한 내용일 듯...
		if(stEvtInfo.ucParentalRatingNum > 0)
		{
			HxCountry_e		eCountryId;

			// eCountryId = MWC_PARAM_GetCountryId();
			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
			if (ERR_OK != hErr)
			{
				HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
			}

#if 1
			{
				const HCHAR *pszIsoCountryCode = NULL;
				pszIsoCountryCode = HLIB_COUNTRY_IdTo3166_Alpha3(eCountryId);
				if (pszIsoCountryCode)
				{
					HxSTD_MemCopy(szIso3166Code, pszIsoCountryCode, 4);
					hErr = ERR_OK;
				}
				else
				{
					hErr = ERR_FAIL;
				}
			}
#else
			hErr = MWC_UTIL_GetIso3166CodeByCountryId(eCountryId, szIso3166Code);
#endif
			for(idx = 0; idx < stEvtInfo.ucParentalRatingNum && idx < PVR_PARENTAL_RATING_MAX; idx++)
			{
				MWC_UTIL_ConvertStringUpper(stEvtInfo.astParentalRating[idx].aucIsoCountryCode);
				if (MWC_UTIL_DvbStrncmp(stEvtInfo.astParentalRating[idx].aucIsoCountryCode, szIso3166Code, LANG_639_CODE_MAX) == 0)
				{
					pstContext->ulRatingValue = stEvtInfo.astParentalRating[idx].ucParentalRating;
					bFindRate = TRUE;
					HxLOG_Info("\t=>[xmgr_playback_GetEitInfo_Base] Event ID changed [0x%x]. ucParentRating=%d \n", ulEventId, pstContext->ulRatingValue);
					break;
				}
			}
		}
		if(bFindRate == FALSE)
		{
			// 첫번째 Parental Rate를 사용할지는 향마다 다르다.
			if(stEvtInfo.ucParentalRatingNum > 0)	//  첫번째 Parental Rate를 사용
			{
				pstContext->ulRatingValue = stEvtInfo.astParentalRating[0].ucParentalRating;
			}
			else
			{
				pstContext->ulRatingValue = 0;
			}
			HxLOG_Info("\t=> [xmgr_playback_GetEitInfo_Base] Event ID changed [0x%x]. NoParentalRate !!! ucParentRating=%d \n", ulEventId, pstContext->ulRatingValue);
		}
	}

	xmgr_playback_ProcessParentalRate_Base(pstContext, bFindRate);

	if(ulEventId != pstContext->ulEventId) /* event ID가 달라졌으면 rating PIN passed flag reset. */
	{
		HxLOG_Print("\t=> [xmgr_playback_GetEitInfo_Base] Event ID changed from [0x%x] to [0x%x].\n", pstContext->ulEventId, ulEventId);
		pstContext->bRatingPinPassed = FALSE;
		pstContext->bSCPinPassed = FALSE; /* APS 사양 AS5101 8.5.1.4 */
		pstContext->ulEventId = ulEventId; /* Event ID 갱신 */
	}

	pstContext->bRatingValid = TRUE;
	pstContext->bAlreadyExistEit = TRUE;

}

STATIC HERROR xmgr_playback_CvtPlaySpeedToTrickState_Base (HINT32 nPlaySpeed, MgrPvr_TrickState_e *peTrickState, MgrPb_TrickSpeed_e *peTrickSpeed)
{
	/*
		기존 AP Player가 Trick State, Trick Speed를 기록하며 동작에 관여하기 때문에
		부득이 만든 변환 코드임.
		내부에서 Trick State, Trick Speed를 쓰지 않도록 고친 후에 전부 삭제해야 한다.
	*/

	MgrPvr_TrickState_e		 eTrickState = eMgrPb_TickState_STOP;
	MgrPb_TrickSpeed_e		 eTrickSpeed = eMgrPb_Speed_X1;

	switch (nPlaySpeed)
	{
	case 0: 	eTrickState = eMgrPb_TickState_PAUSE;		eTrickSpeed = eMgrPb_Speed_X1;break;
	case 10:	eTrickState = eMgrPb_TickState_SLOW;			eTrickSpeed = eMgrPb_Speed_X4;break;
	case 12:	eTrickState = eMgrPb_TickState_SLOW;			eTrickSpeed = eMgrPb_Speed_X3;break;
	case 25:	eTrickState = eMgrPb_TickState_SLOW;			eTrickSpeed = eMgrPb_Speed_X2;break;
	case 50:	eTrickState = eMgrPb_TickState_SLOW;			eTrickSpeed = eMgrPb_Speed_X1;break;
	case 100:	eTrickState = eMgrPb_TickState_PLAY;			eTrickSpeed = eMgrPb_Speed_X1;break;
	case 120:	eTrickState = eMgrPb_TickState_FF;		eTrickSpeed = eMgrPb_Speed_X1;break;
	case 200:	eTrickState = eMgrPb_TickState_FF; 		eTrickSpeed = eMgrPb_Speed_X1;break;
	case 400:	eTrickState = eMgrPb_TickState_FF;		eTrickSpeed = eMgrPb_Speed_X2;break;
	case 800:	eTrickState = eMgrPb_TickState_FF;		eTrickSpeed = eMgrPb_Speed_X3;break;
	case 1000:	eTrickState = eMgrPb_TickState_FF;		eTrickSpeed = eMgrPb_Speed_X3;break;
	case 1600:	eTrickState = eMgrPb_TickState_FF;		eTrickSpeed = eMgrPb_Speed_X4;break;
	case 3000:	eTrickState = eMgrPb_TickState_FF;		eTrickSpeed = eMgrPb_Speed_X4;break;
	case 3200:	eTrickState = eMgrPb_TickState_FF;		eTrickSpeed = eMgrPb_Speed_X5;break;
	case 6400:	eTrickState = eMgrPb_TickState_FF;		eTrickSpeed = eMgrPb_Speed_X6;break;
	case 10000:	eTrickState = eMgrPb_TickState_FF;		eTrickSpeed = eMgrPb_Speed_X5;break;
	case 25600:	eTrickState = eMgrPb_TickState_FF;		eTrickSpeed = eMgrPb_Speed_X5;break;
	case -100:	eTrickState = eMgrPb_TickState_FR; 		eTrickSpeed = eMgrPb_Speed_X1;break;
	case -120:	eTrickState = eMgrPb_TickState_FR; 		eTrickSpeed = eMgrPb_Speed_X1;break;
	case -200:	eTrickState = eMgrPb_TickState_FR; 		eTrickSpeed = eMgrPb_Speed_X1;break;
	case -400:	eTrickState = eMgrPb_TickState_FR;		eTrickSpeed = eMgrPb_Speed_X2;break;
	case -800:	eTrickState = eMgrPb_TickState_FR;		eTrickSpeed = eMgrPb_Speed_X3;break;
	case -1000:	eTrickState = eMgrPb_TickState_FR;		eTrickSpeed = eMgrPb_Speed_X3;break;
	case -1600:	eTrickState = eMgrPb_TickState_FR;		eTrickSpeed = eMgrPb_Speed_X4;break;
	case -3000:	eTrickState = eMgrPb_TickState_FR;		eTrickSpeed = eMgrPb_Speed_X4;break;
	case -3200:	eTrickState = eMgrPb_TickState_FR;		eTrickSpeed = eMgrPb_Speed_X5;break;
	case -6400:	eTrickState = eMgrPb_TickState_FR;		eTrickSpeed = eMgrPb_Speed_X6;break;
	case -10000:eTrickState = eMgrPb_TickState_FR;		eTrickSpeed = eMgrPb_Speed_X5;break;
	case -25600:eTrickState = eMgrPb_TickState_FR;		eTrickSpeed = eMgrPb_Speed_X5;break;
	default:
		HxLOG_Error ("Not Supported Speed (%d)\n", nPlaySpeed);
		return ERR_FAIL;
	}

	*peTrickState = eTrickState;
	*peTrickSpeed = eTrickSpeed;
	return ERR_OK;
}

STATIC HERROR	xmgr_playback_UpdatePlaybackEndTimeInfo_Base(HUINT32 ulActionId, MgrPb_Info_t *pstInfo)
{
	HINT32 			idx;
	MgrAction_Type_e actType;
	xmgrPbContext_t	*pstContext = NULL;

	for(idx = 0; idx < MAX_NUM_OF_PLAY; idx++)
	{
		pstContext = xmgr_playback_GetContext_Base( idx );
		actType = MGR_ACTION_GetType(pstContext->ulActionId);
		if(	(actType == eViewType_PB) ||
			(actType == eViewType_TSR_PB) ||
			(actType == eViewType_CHASE_PB) )
		{
			if((pstInfo != NULL) && (pstContext->ePbType > 0))
			{
				if((pstContext->bPBStart == FALSE) ||(pstContext->bFirstPmt == FALSE)||(pstContext->bFirstTimecode == FALSE))
				{
					return ERR_FAIL;
				}

				pstContext->ulEndTime = pstInfo->ulEndTime;		/**< end time */
			}

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR xmgr_playback_SetAllowedContentStatusByDrm(xmgrPbContext_t *pstContext, HUINT32 ulStartTime)
{
	HERROR		hRet = ERR_FAIL;
	HULONG		ulSysTime = 0;
	HERROR		hErr;
	HUINT8		PLAY_ENDTIME_GAP = 3;
	HULONG		ulMinutesForRL = 0;
	HUINT32		ulDrmStartTime = 0;
	DxCopyrightControl_t 	stInfo;

	hRet = SVC_META_GetDRMInfoByTime(pstContext->ulContentId, eDxCopyrightControl_CiPlusDrmType_Uri, ulStartTime, &stInfo);
	if (hRet != ERR_OK) // DRM 정보가 없다면 권한 제한이 없는 경우
	{
		pstContext->ulStartTimeByDrm = 0;
		HxLOG_Print("There is no drm start time !!! \n");
		return ERR_OK;
	}

	ulMinutesForRL = stInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL;

	if (ulMinutesForRL == 0 && stInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode > CI_CC_URI_MODE_DEFAULT) // DRM 정보 unlimited인 경우
	{
		pstContext->ulStartTimeByDrm = 0;
		HxLOG_Print("There is no drm start time !!! \n");
		return ERR_OK;
	}

	hErr = VK_CLOCK_GetTime( &ulSysTime );
	if(hErr != ERR_OK)
	{
		// 시스템 시간이 설정되지 않은 경우에는 권한이 만료된 것으로 판단
		pstContext->ulStartTimeByDrm = pstContext->ulEndTime;
		return ERR_FAIL;
	}

	if ((ulSysTime - (pstContext->ulOrgStartTime + pstContext->ulEndTime) + PLAY_ENDTIME_GAP) > (ulMinutesForRL * 60))
	{
		// 권한 만료된 상황으로 play를 종료하도록 한다.
		pstContext->ulStartTimeByDrm = pstContext->ulEndTime;
		return ERR_FAIL;
	}

	// local_Pb_EvtPbTimecodeChange 에서 RL 체크하는 부분에의 ulSystem 은 이 함수에서의 ulSystem 보다 최소한 1초 이후에 발생하여 대략 +2 초를 더해줌
	ulDrmStartTime = ulSysTime - pstContext->ulOrgStartTime - (ulMinutesForRL * 60) + PLAY_ENDTIME_GAP + 2;

	// 음수인 경우에는 권한제한이 없는 것으로 판단
	if ((HINT32)ulDrmStartTime < 0)
	{
		ulDrmStartTime = 0;
	}

	pstContext->ulStartTimeByDrm = ulDrmStartTime;
	return ERR_OK;
}

#define _________INTERNAL_FUNC__________
xmgrPbContext_t	*xmgr_playback_GetContext_Base (HUINT32 ulActionId)
{
	HUINT32				 ulPbIndex = xmgr_playback_GetContextIndexFromActionId (ulActionId);

	if (ulPbIndex >= MAX_NUM_OF_PLAY)
	{
		HxLOG_Error ("Invalid Action ID(%d)\n", ulActionId);
		return NULL;
	}

	return &(s_astMgrPbContext[ulPbIndex]);
}

xmgrPbContext_t	*xmgr_playback_GetContextFromIndex_Base (HUINT32 ulPbIndex)
{
	if (ulPbIndex >= MAX_NUM_OF_PLAY)
	{
		HxLOG_Error ("Invalid Action ulPbIndex(%d)\n", ulPbIndex);
		return NULL;
	}

	return &(s_astMgrPbContext[ulPbIndex]);
}

void xmgr_playback_SetAppState_Base(xmgrPbContext_t *pstContext, xmgrPb_State_e appState)
{
	HERROR			hErr;
	HUINT32			ulActionId = pstContext->ulActionId;

	HxLOG_Info("xmgr_playback_SetAppState_Base(%s)\n", xmgr_playback_GetAppStateStr_Base(appState));

	/* 현재 state에서 나가면서 처리할 작업들 ... */
	if(appState == pstContext->ePbState)
	{ /* 전환하려는 state가 현재 state와 같다 */
		if(appState == eXmgrPbState_AV)
		{
			/* eWtvState_AV 끼리 움직이는 것이라면 nCasErrCode가 변했는지 확인해봐야 한다.
				Preview 및 이벤트 구매등으로 인하여 nCasErrCode 변경되었을 가능성이 있음... */
			if(pstContext->nCasErrCode != 0)
			{ /* Preview 상태로 감. AV가 정상적으로 나오고 있지만 CAS check 상태이다. */
				/* CAS appl이 preview를 처리하여 유저가 구매를 하면 다시 CAS_OK를 올려서 hErrCode를 0로 해야 함.
					즉, cas hError code가 달라져도 CAS event를 올려야 한다. */
				xmgr_playback_SetUiState_Base(pstContext, eMgrPb_CAS);
			}

			if(pstContext->ulStartTimeByDrm > pstContext->ulPlayTime)
			{
				xmgr_playback_DisableOutput_Base(pstContext);
				xmgr_playback_SetUiState_Base(pstContext, eMgrPb_CAS);
			}
		}

		HxLOG_Print("\t=> Same appState.\n");

		return;
	}
	else
	{ /* appState 변경이 생겼음. 변경 process 진행 */

		/* WTV_STATE_AV에서 다른 state로 이동 이면 AV, subtitle, teletext를 stop한다. */
		if(pstContext->ePbState == eXmgrPbState_AV)
		{
			//if(appState == eWtvState_WAIT_RATING)
			if(0)
			{ /* 다음 state가 WAIT_RATING이면 AV stop이 아니라 hide 해야 함. */
			}
			else
			{ /* 다른 경우는 모두 AV stop */
			//	hErr = SVC_AV_StopService(ulActionId, eSyncMode);
			//	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

				xmgr_playback_StopSubtitle_Base(pstContext);

			//	pstContext->bRestartAV = TRUE;
			}

			/* Application들에게 AV 중단을 통보한다. Subtitle, teletext, data 방송등이 이 action 을 처리하고 있었다면 같이 중단할 것이다. */
			BUS_SendMessage(NULL, eMEVT_OUTPUT_AV_STOPPED, pstContext->hAction, 0, 0, 0);
		}
	}

	HxLOG_Print("\t=> State transition <%s> => <%s>\n",
					xmgr_playback_GetAppStateStr_Base(pstContext->ePbState), xmgr_playback_GetAppStateStr_Base(appState));
	pstContext->ePbState = appState; /* 새 state 등록. */

	/* 새로운 state로 진입하면서 처리할 것들 ... */
	switch(appState)
	{
		case eXmgrPbState_LOCK_PIN :
			xmgr_playback_DisableOutput_Base(pstContext);
			if(pstContext->eTrickState != eMgrPb_TickState_PAUSE)
			{
				MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 0);
				//XMGR_PB_SetTrickState(pstContext->ulActionId, eMgrPb_TickState_PAUSE, eMgrPb_Speed_X1);
			}
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_LOCK_PIN);
			break;

		case eXmgrPbState_WAIT_PMT :
			/* PMT PID 변경과 service DB에 PID 없는 모드로 동작할 경우에는 진입 동작이 다르기 때문에
			공통 처리하면 안된다. 여기서는 그냥 state 변경만 해줌. */
			break;

		case eXmgrPbState_WAIT_RATING :
			// Parental Rating 정보를 기다리는 상태
			xmgr_playback_DisableOutput_Base(pstContext);
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_NONE);
			break;

		case eXmgrPbState_RATING_PIN :
			xmgr_playback_DisableOutput_Base(pstContext);
#if !defined(CONFIG_OP_NORDIG)
			if(pstContext->eTrickState != eMgrPb_TickState_PAUSE)
			{
				MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 0);
				//XMGR_PB_SetTrickState(pstContext->ulActionId, eMgrPb_TickState_PAUSE, eMgrPb_Speed_X1);
			}
#endif
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_RATING_PIN);
			break;

		case eXmgrPbState_AV :
		{
			if(pstContext->bAvUnderrun == TRUE)
			{ /* Underrun 예약 되어 있음 */
				xmgr_playback_SetUiState_Base(pstContext, eMgrPb_AV_UNDER_RUN);
			}
			else if(pstContext->bHdmiBlocked == TRUE)
			{ /* HDMI blocked 예약 되어 있음. */
				xmgr_playback_SetUiState_Base(pstContext, eMgrPb_HDMI_BLOCKED);
			}
			else
			{  /* NO AV 메시지 체크. */
				xmgr_playback_CheckAV_Base(pstContext);
			}

			xmgr_playback_SetCopyProtection(pstContext);
			xmgr_playback_EnableOutput(pstContext);
			if(pstContext->eTrickState == eMgrPb_TickState_PAUSE)
			{
				MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 100);
				//XMGR_PB_SetTrickState(pstContext->ulActionId, eMgrPb_TickState_PLAY, eMgrPb_Speed_X1);
			}
			if(pstContext->bRestartAV == TRUE)
			{
				if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
				{
#if !defined(SvcAv_START_SERVICE_NEW)
					hErr = SVC_AV_SetStereoSelect(pstContext->ulActionId, pstContext->svcInfo.eSoundMode);
					if (hErr)
					{
						HxLOG_Error("SVC_AV_SetStereoSelect ret :0x%X\n",hErr);
					}
#endif
					hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, pstContext->svcInfo.eDualMono);
					if (hErr)
					{
						HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
					}

#if defined(SvcAv_START_SERVICE_NEW)
					hErr = SVC_AV_StartService(pstContext->hAction,
											pstContext->ulVideoDecId,
											pstContext->ulDemuxId,
											pstContext->svcInfo.usAudioPid,
											pstContext->svcInfo.usAudioAuxPid,
											pstContext->svcInfo.usVideoPid,
											pstContext->svcInfo.usPcrPid,
											pstContext->svcInfo.eAudioCodec,
											pstContext->svcInfo.eAudioAuxCodec,
											pstContext->svcInfo.eVideoCodec,
											FALSE,
											FALSE
											);
#else
					hErr = SVC_AV_StartService(pstContext->hAction,
											pstContext->ulVideoDecId,
											pstContext->ulDemuxId,
											pstContext->svcInfo.usAudioPid,
											pstContext->svcInfo.usAudioAuxPid,
											pstContext->svcInfo.usDolbyPid,
											pstContext->svcInfo.usVideoPid,
											pstContext->svcInfo.usPcrPid,
											pstContext->svcInfo.eAudioCodec,
											pstContext->svcInfo.eAudioAuxCodec,
											pstContext->svcInfo.eDolbyCodec,			//2009.03.23 추가.
											pstContext->svcInfo.eVideoCodec,
											pstContext->svcInfo.bDolbyFlag,
											pstContext->svcInfo.eSoundMode,
											pstContext->svcInfo.eDualMono,
											FALSE,
											FALSE
											);
#endif
					if(ERR_OK!=hErr) 		HxLOG_Critical("Critical Eror!!!\n");
				}
			}

#if defined(CONFIG_MW_TELETEXT)
			/* Teletext는 PID가 NULL이 아니면 VBI insert를 위하여 무조건 시작한다. 단, session당 한번만 start되어야 함. */
			if( (pstContext->svcInfo.usTtxPid < PID_NULL) && (pstContext->bTtxVbiStarted == FALSE) )
			{
				hErr = SVC_TTX_StartService(pstContext->hAction, pstContext->svcInfo.usTtxPid);
				if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

				pstContext->bTtxVbiStarted = TRUE;
			}
#endif
			xmgr_playback_StartSubtitle_Base(pstContext);

			if(pstContext->nCasErrCode != 0)
			{ /* Preview 상태. AV가 정상적으로 나오고 있지만 CAS check 상태이다. */
				xmgr_playback_SetUiState_Base(pstContext, eMgrPb_CAS);
			}
			else
			{
				/* NO AV 메시지 체크. */
				xmgr_playback_CheckAV_Base(pstContext);
			}

			if(pstContext->ulStartTimeByDrm > pstContext->ulPlayTime)
			{
				xmgr_playback_DisableOutput_Base(pstContext);
				xmgr_playback_SetUiState_Base(pstContext, eMgrPb_CAS);
			}

			/* 혹시 video PID가 살아나거나 죽었을까봐 다시 한번 체크해 본다. */
			xmgr_playback_CheckRadioBackground_Base(pstContext);

			/* Application들에게 AV 시작을 알림. 이 action이 main action이라면 subtitle, teletext, data방송등이 시작될 것임. */
			BUS_SendMessage(NULL, eMEVT_OUTPUT_AV_STARTED, pstContext->hAction, pstContext->hSvc, 0, 0);

			// trick 을 해야 하는 경우인지 체크한다.
			switch(pstContext->eFirstTrickState)
			{
				// Pause 키로 재생을 시작한 경우
				case eMgrPb_TickState_PAUSE :
					if(pstContext->eTrickState == pstContext->eFirstTrickState)
						break;
					hErr = MGR_PLAYBACK_SetPlaySpeed (ulActionId, 0);
					break;

				// Slow 키로 재생을 시작한 경우
				case eMgrPb_TickState_SLOW :
					if(pstContext->eTrickState == pstContext->eFirstTrickState)
						break;

#if	defined(CONFIG_IGNORE_TRICKPLAY_IN_PMTCHANGED_SERVCE)
					if (!pstContext->bTrickEnable)
						break;
#endif

					hErr = MGR_PLAYBACK_SetPlaySpeed (ulActionId, 50);
break;

				// FR 키로 재생을 시작한 경우
				case eMgrPb_TickState_FR :
					if(pstContext->eTrickState == pstContext->eFirstTrickState)
						break;

#if	defined(CONFIG_IGNORE_TRICKPLAY_IN_PMTCHANGED_SERVCE)
					if (!pstContext->bTrickEnable)
						break;
#endif

					hErr = MGR_PLAYBACK_SetPlaySpeed (ulActionId, -200);
					break;

				// Normal Speed로 재생을 시작한 경우
				default :
					break;
			}
			pstContext->eFirstTrickState = eMgrPb_TickState_STOP;
			break;
		}

		case eXmgrPbState_CAS_BLOCKED :
			xmgr_playback_DisableOutput_Base(pstContext);
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_CAS);
			break;

		default :
			/* 위 case 말고 다른 state 나 WTV state가 아닌 값은 이 API를 쓰면 안된다. */
			HxLOG_Critical("\n\n");
			break;
	}

}
HUINT32	xmgr_playback_GetMarginedPmtTime (HINT32 nPlaySpeed, HUINT32 ulPlayTime)
{
	HUINT32	ulMarginedTime = 0;

	if (nPlaySpeed > 0)
	{
		ulMarginedTime = ulPlayTime + PLAYBACK_PMT_TIMESTAMP_MARGIN;
	}
	else if (nPlaySpeed < 0)
	{
		if (ulPlayTime > PLAYBACK_PMT_TIMESTAMP_MARGIN)
		{
			ulMarginedTime = ulPlayTime - PLAYBACK_PMT_TIMESTAMP_MARGIN;
		}
		else
		{
			ulMarginedTime = 0;
		}
	}
	else
	{
		ulMarginedTime = ulPlayTime;
	}

	return ulMarginedTime;
}
HERROR	xmgr_playback_GetRefTime(SvcMeta_TimestampList_t *pstList, SvcMeta_TimeStampType_e eCheckType, HUINT32 *pulRefTime)
{
	switch (eCheckType)
	{
		case eSvcMeta_Timestamp_PMT:
			*pulRefTime = pstList->ulPmtRefTime;
			break;
		case eSvcMeta_Timestamp_EIT:
			*pulRefTime = pstList->ulEitRefTime;
			break;
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xmgr_playback_CheckTimeStamp_Base (xmgrPbContext_t *pstContext, HUINT32 ulSourceTime)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 i;
	HUINT32 ulRefTime,ulTime;
	SvcMeta_TimestampData_t *pstTimeStamp	= NULL;
	SvcMeta_TimestampList_t *pstList		= NULL;

	if (pstContext == NULL
		|| pstContext->nPlaySpeed == 0)
	{
		return ERR_FAIL;
	}

	pstList 	=	&(pstContext->stTimeStampList);

	for (i=0; i<pstList->ulNumber; i++)
	{
		pstTimeStamp = &(pstList->pstData[i]);
		if (pstTimeStamp->pData == NULL )
		{
			continue;
		}

		// 1. Process only PMT, EIT
		hErr = xmgr_playback_GetRefTime(pstList, pstTimeStamp->stTimeStamp.eType, &ulRefTime);
		if (hErr != ERR_OK)
		{
			continue;
		}

		// 2. check if it is already processed index
		if (ulRefTime == pstTimeStamp->stTimeStamp.ulTime)
		{
			continue;
		}

		// 3. time check
		ulTime = pstTimeStamp->stTimeStamp.ulTime;
		if (ulRefTime < ulSourceTime)
		{
			 if ( !((ulRefTime < ulTime) && (ulTime < ulSourceTime)) )
			 {
				continue;
			 }
		}
		else if (ulSourceTime < ulRefTime)
		{
			 if ( !((ulTime < ulSourceTime) && (ulSourceTime < ulRefTime)) )
			 {
				continue;
			 }
		}

		// 4. process
		switch (pstTimeStamp->stTimeStamp.eType)
		{
			case eSvcMeta_Timestamp_PMT:
			{
				SvcMeta_Service_t *pstMetaSvc = NULL;
				SvcMeta_Service_t *pstOrgMetaSvc = NULL;
				pstMetaSvc = (SvcMeta_Service_t *)pstTimeStamp->pData;
				if (pstMetaSvc == NULL)
				{
					break;
				}

				/* When live didn't tune to recording service, service db might not been updated.
				Setting main audio <- dolby scenario was required, so added here.
				This code should be deleted when recorder update service db */
				// if main audio is empty and dolby exists
				if ( (pstMetaSvc->bDolbyFlag == TRUE)
					&&(pstMetaSvc->usAudioPid == PID_NULL)
					&&(pstMetaSvc->usDolbyPid != PID_NULL))
				{
					HxLOG_Warning("Use Dolby Audio. (%d) -> (%d) \n", PID_NULL, pstMetaSvc->usDolbyPid);

					// set main audio <- dolby
					pstMetaSvc->usAudioPid = pstMetaSvc->usDolbyPid;
					pstMetaSvc->eAudioCodec = pstMetaSvc->eDolbyCodec;
				}

				pstOrgMetaSvc = (SvcMeta_Service_t *)OxMGR_Calloc(sizeof(SvcMeta_Service_t));
				if (pstOrgMetaSvc == NULL)
				{
					HxLOG_Error("fail to mem alloc \n ");
					break;
				}

				/* keep user settings */
				hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, pstOrgMetaSvc);
				if (hErr == ERR_OK)
				{
					// audio
					if (pstOrgMetaSvc->bAudUserSetFlag == TRUE)
					{
						if (pstOrgMetaSvc->usAudioPid != PID_NULL)
						{
							pstMetaSvc->bAudUserSetFlag = pstOrgMetaSvc->bAudUserSetFlag;
							pstMetaSvc->usAudioPid = pstOrgMetaSvc->usAudioPid;
							pstMetaSvc->eAudioCodec = pstOrgMetaSvc->eAudioCodec;
							// TODO: dolby scenario ?
							// TODO: audio description
						}
						else
						{
							HxLOG_Warning("user selected but null pid \n");
						}
					}

					// sub
					pstMetaSvc->bSubttlUserFlag = pstOrgMetaSvc->bSubttlUserFlag;
					pstMetaSvc->ucSubttlIdx = pstOrgMetaSvc->ucSubttlIdx;
				}

				OxMGR_Free(pstOrgMetaSvc);

				hErr = SVC_META_SetServiceInfo (pstContext->ulContentId, pstMetaSvc, TRUE);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_META_SetServiceInfo err:\n");
				}

				pstList->ulPmtRefTime = pstTimeStamp->stTimeStamp.ulTime;

				break;
			}
			case eSvcMeta_Timestamp_EIT:
			{
				SvcMeta_Event_t *pstMetaEvent	=	NULL;
				pstMetaEvent = (SvcMeta_Event_t *)pstTimeStamp->pData;
				if (pstMetaEvent == NULL)
				{
					break;
				}

				hErr = SVC_META_SetEventInfo (pstContext->ulContentId, pstMetaEvent, TRUE);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_META_SetEventInfo err:\n");
				}

				pstList->ulEitRefTime = pstTimeStamp->stTimeStamp.ulTime;

				break;
			}

			default:
				break;
		}
	}

	return ERR_OK;
}

HERROR		xmgr_playback_RefreshSvcMeta_Base (xmgrPbContext_t *pstContext, HUINT32 ulPlayTime)
{
	HERROR	hErr;
	HUINT32	ulMarginedTime = 0;

	hErr = SVC_META_UnloadTimeStamp(&pstContext->stTimeStampList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_META_UnloadTimeStamp failed. \n");
	}

	hErr = SVC_META_LoadTimeStamp(pstContext->ulContentId, &pstContext->stTimeStampList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_META_LoadTimeStamp failed. \n");
	}

	ulMarginedTime = xmgr_playback_GetMarginedPmtTime(pstContext->nPlaySpeed, ulPlayTime);
	(void)xmgr_playback_CheckTimeStamp_Base(pstContext, ulMarginedTime);

	return ERR_OK;
}



#define _________MSG__________
BUS_Result_t xmgr_playback_MsgBusCreate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HUINT32 				ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT8					ucPlayIdx = xmgr_playback_GetContextIndexFromActionId (ulActionId);
	MgrAction_Type_e		eActionType;
	SvcMeta_Record_t		stRecordInfo;
#if defined(CONFIG_APCORE_MOVING_THUMBNAIL_CACHE)
	HBOOL					bRecord = FALSE;
#endif
	HERROR					hErr;

	if(pstContext->ePbState != eXmgrPbState_IDLE)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}
	HxLOG_Info("Player(%d) : eMEVT_BUS_CREATE ulContentId=%d ulStartTime=%d\n", ucPlayIdx, pstContext->stSetup.ulContentId, pstContext->stSetup.ulStartTime);


	switch (pstContext->stSetup.ePlayType & 0x0000000f)
	{
		case eMgrPb_PlayType_Tsr:
			eActionType = eViewType_TSR_PB;
			break;
		case eMgrPb_PlayType_Chase:
			eActionType = eViewType_CHASE_PB;
			break;
		case eMgrPb_PlayType_Normal:
		default:
			eActionType = eViewType_PB;
			break;
	}

	// Resouce 는 모두 Ready된 상태임.
	hErr = MGR_ACTION_Take(ulActionId, eActionType, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		HxLOG_Info("Player(%d) : can't take MGR_ACTION_Take eActionType=%x Handle_t=%x\n", ucPlayIdx, eActionType, pstContext->stSetup.hCurrSvc);
	//	goto FINISH;
	}

	/* init recorder app pstContext */
	pstContext->ulActionId = ulActionId;
	pstContext->hAction = hAction;

	hErr = MGR_RSC_GetResourceId(ulActionId, eRxRSCTYPE_AV_DEC, &pstContext->ulVideoDecId);
	if(ERR_OK!=hErr) 		HxLOG_Critical("Critical Eror!!!\n");
	hErr = MGR_RSC_GetResourceId(ulActionId, eRxRSCTYPE_PVR_PLAYER, &pstContext->ulPlayerId);
	if(ERR_OK!=hErr) 		HxLOG_Critical("Critical Eror!!!\n");
	hErr = MGR_RSC_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &pstContext->ulDemuxId);
	if(ERR_OK!=hErr) 		HxLOG_Critical("Critical Eror!!!\n");

	HxLOG_Debug(HxANSI_COLOR_YELLOW("ulActionId = %d, eActionId_BG_FIRST(%d), eActionId_BG_LAST(%d), NUM_VIEW_ACTION(%d)\n")
		, ulActionId, eActionId_BG_FIRST, eActionId_BG_LAST, NUM_VIEW_ACTION);

	HxLOG_Debug(HxANSI_COLOR_YELLOW("Player(%d): eActionType=%d, ePlayType = 0x%x\n"), ucPlayIdx, eActionType, pstContext->stSetup.ePlayType);
	HxLOG_Debug(HxANSI_COLOR_YELLOW("pstContext->ulVideoDecId = %d\n"), pstContext->ulVideoDecId);
	HxLOG_Debug(HxANSI_COLOR_YELLOW("pstContext->ulPlayerId   = %d\n"), pstContext->ulPlayerId);
	HxLOG_Debug(HxANSI_COLOR_YELLOW("pstContext->ulDemuxId    = %d\n"), pstContext->ulDemuxId);

	pstContext->ucPlayIdx		= ucPlayIdx;
	pstContext->hSvc			= pstContext->stSetup.hCurrSvc;
	pstContext->hMasterSvc		= pstContext->stSetup.hMasterSvc;
	pstContext->eSvcListGroup	= pstContext->stSetup.eSvcListGroup;
	pstContext->ePbType			= pstContext->stSetup.ePlayType;
	pstContext->ePbState		= eXmgrPbState_STARTED;
	pstContext->eFirstTrickState = pstContext->stSetup.eTrickState;
	pstContext->ulPlayTime		= 0;
	if(pstContext->stSetup.bStartPaused == TRUE)
	{
		pstContext->nPlaySpeed		= 0;			// Start&Pause
		pstContext->eTrickState		= eMgrPb_TickState_PAUSE;
	}
	else
	{
		pstContext->nPlaySpeed		= 100;			// 1배속 Play (100%)
		pstContext->eTrickState 	= eMgrPb_TickState_PLAY;
	}
	pstContext->eTrickSpeed		= eMgrPb_Speed_X1;
	pstContext->bPmtValid		= FALSE;
	pstContext->nCasErrCode		= 0;
	pstContext->bCasOk			= FALSE;
	pstContext->ulContentId		= pstContext->stSetup.ulContentId;
	pstContext->uiState			= eMgrPb_NONE;
	pstContext->bRadioBackground = FALSE;
	pstContext->bRestartAV		= FALSE;
	pstContext->ulEventId		= (HUINT32)-1;
	pstContext->bAVHide			= FALSE;
	pstContext->bIsEOF			= FALSE;
	pstContext->bRatingPinPassed = FALSE;
	pstContext->bSCPinPassed	= FALSE;
	pstContext->bRatingPassByLockPass = FALSE;
	pstContext->bSCPinCodeValid = FALSE;

	HxSTD_MemSet(&pstContext->svcInfo, 0, sizeof(SvcMeta_Service_t));
	HxSTD_MemSet(&stRecordInfo, 0, sizeof(SvcMeta_Record_t));

	(void)xmgr_playback_RefreshSvcMeta_Base(pstContext, pstContext->stSetup.ulStartTime);

	hErr = SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecordInfo);
	hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, &pstContext->svcInfo);

	pstContext->ulStartTime		= 0;
	pstContext->ulOrgStartTime	= stRecordInfo.ulStartTime;
	pstContext->ulEndTime		= stRecordInfo.ulRecTime;

	// Medialist 에서 레코딩중인 파일을 재생한 경우에 대한 처리
	if (eActionType == eViewType_PB)
	{
		Handle_t hRecAction = HANDLE_NULL;

		hErr = SVC_META_GetRecActionByContentId (pstContext->ulContentId, &hRecAction);
		if ((ERR_OK == hErr) && (HANDLE_NULL != hRecAction))
		{
			SvcRec_RecInfo stRecInfo;
			HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcRec_RecInfo));
			hErr = SVC_REC_GetInfo (hRecAction, &stRecInfo);
			if (ERR_OK == hErr)
			{
				pstContext->ulEndTime = stRecInfo.ulRecTime;
			}
		}
	}

	// TSR PB 를 처음 시작할
	if ((eActionType == eViewType_TSR_PB) || (eActionType == eViewType_CHASE_PB))
	{
		DbSvc_Info_t		 stSvcInfo;

		hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &stSvcInfo);
		if (ERR_OK == hErr)
		{
			if (DbSvc_GetSubttlIdx(&stSvcInfo) != pstContext->svcInfo.ucSubttlIdx)
			{
				pstContext->svcInfo.ucSubttlIdx = DbSvc_GetSubttlIdx(&stSvcInfo);
				(void)SVC_META_SetServiceInfo(pstContext->ulContentId, &pstContext->svcInfo, FALSE);
			}
		}
	}

	/* Session version 증가 */
	pstContext->hAction = SVC_PIPE_IncreaseActionVersion(ulActionId);

	if(stRecordInfo.stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
	{
		HxSTD_MemCopy(&pstContext->stDrmInfo, &stRecordInfo.stDrmInfo, sizeof(DxCopyrightControl_t));
	}

	// MW_Pb_Open 하기 전에 drm 판단
	if ((pstContext->stSetup.ePlayType & 0x0000000f) == eMgrPb_PlayType_Normal)
	{
		// RL(Retention Limit) 과 관련된 정보가 webapp로 전달되고 webapp이 playback 가능한 시간 영역을 찾아서 jump play 하도록 해아 하지만
		// 현재 협업이 어려운 상황이라 여기서 처음 시작시에 AV를 block 하고 playback 시작하도록 함.
		// 만약 RL 동작의 주체가 Webapp 에서 판단해서 starttime 을 설정해서 전달한다면 관련 code 모두 제거하면 됨 .
		if (xmgr_playback_SetAllowedContentStatusByDrm(pstContext, pstContext->stSetup.ulStartTime) != ERR_OK)
		{
			// 권한이 만료된 경우이므로 play를 중지할 필요가 있을 경우 여기서 stop 하면 됨.
		}
	}

	/* setup a player */
	{
		SvcPb_Setup_t stSetup;

		HxSTD_MemSet( &stSetup, 0, sizeof(SvcPb_Setup_t) );

		switch(eActionType)
		{
			case eViewType_PB:					stSetup.ePbActType = ePb_ActType_Playback; break;
			case eViewType_TSR_PB:				stSetup.ePbActType = ePb_ActType_Tsr_Playback; break;
			case eViewType_CHASE_PB:			stSetup.ePbActType = ePb_ActType_Chase_Playback; break;
			default : break;
		}

		stSetup.ulStartTime		=	pstContext->stSetup.ulStartTime;

		switch (pstContext->stSetup.ePlayType & 0x0f000000)
		{
			case eMgrPb_PlayType_EndPos:
				stSetup.eStartPosType = ePb_StartPosition_End;
				break;
			case eMgrPb_PlayType_FirstPos:
				stSetup.eStartPosType = ePb_StartPosition_Begin;
				break;
			case eMgrPb_PlayType_RecentPos:
			default:
				stSetup.eStartPosType = ePb_StartPosition_LastView;
				break;
		}


		stSetup.bDescramblePlay	=FALSE;
		stSetup.eSvcType		=	pstContext->svcInfo.eSvcType;
		stSetup.bEncryption		=	(1 == stRecordInfo.u1cEncryption)?TRUE:FALSE;

		HxSTD_MemCopy( stSetup.aucPath, stRecordInfo.aucPath,MAX_FILE_LEN);
		HxSTD_MemCopy( stSetup.aucFileName, stRecordInfo.aucFileName,MAX_FILE_LEN);

		switch(eActionType)
		{
			case eViewType_TSR_PB:
			case eViewType_CHASE_PB:
				{
					HUINT32	 ulRecActionId	= 0;
					Handle_t hRecAction		= HANDLE_NULL;

					hErr = SVC_META_GetRecActionByContentId (pstContext->ulContentId, &hRecAction);
					if ((ERR_OK != hErr) || (HANDLE_NULL == hRecAction))
					{
						HxLOG_Error ("SVC_META_GetRecActionByContentId. Error\n");
						break;
					}

					ulRecActionId = SVC_PIPE_GetActionId (hRecAction);
					hErr = SVC_PIPE_GetResourceId (ulRecActionId, eRxRSCTYPE_HDD_REC, &stSetup.ulRecordId);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("SVC_PIPE_GetResourceId. Error\n");
						break;
					}
				}
				break;
			default :
				break;
		}

		stSetup.bMakeThumbnail = FALSE;
		stSetup.bIsUsbFlashRecord = pstContext->stSetup.bIsUsbFlashRecord;
		stSetup.bStartPaused = pstContext->stSetup.bStartPaused;

		pstContext->ePbState = eXmgrPbState_WAIT_PB_SETUP;

#if defined(CONFIG_DEBUG)
		HxLOG_Debug ("------------<XMGR_PLAYBACK:SETUP>------------\n");
		switch (stSetup.ePbActType)
		{
			case ePb_ActType_Playback:					HxLOG_Debug ("ePb_ActType_Playback\n"); break;
			case ePb_ActType_Tsr_Playback:				HxLOG_Debug ("ePb_ActType_Tsr_Playback\n"); break;
			case ePb_ActType_Chase_Playback:			HxLOG_Debug ("ePb_ActType_Chase_Playback\n"); break;
			case ePb_ActType_Transcoding_Playback:		HxLOG_Debug ("ePb_ActType_Transcoding_Playback\n"); break;
			case ePb_ActType_Transcoding_TsrPlayback: 	HxLOG_Debug ("ePb_ActType_Transcoding_TsrPlayback\n"); break;
			default: break;
		}
		HxLOG_Debug ("[ulContentId        %d]\n", pstContext->stSetup.ulContentId);
		HxLOG_Debug ("[eFirstTrickState   %d]\n", pstContext->stSetup.eTrickState);
		HxLOG_Debug ("[ulStartTime        %d]\n", stSetup.ulStartTime);
		HxLOG_Debug ("[eSvcType           %d]\n", stSetup.eSvcType);
		HxLOG_Debug ("[bEncryption        %d]\n", stSetup.bEncryption);
		HxLOG_Debug ("[bUseUserEncKey     %d]\n", stSetup.bUseUserEncKey);
		HxLOG_Debug ("[eStartPosType      %d]\n", stSetup.eStartPosType);
		HxLOG_Debug ("[aucPath            %s]\n", stSetup.aucPath);
		HxLOG_Debug ("[aucFileName        %s]\n", stSetup.aucFileName);
		HxLOG_Debug ("[bDescramblePlay    %d]\n", stSetup.bDescramblePlay);
		HxLOG_Debug ("[ulRecordId         %d]\n", stSetup.ulRecordId);
		HxLOG_Debug ("[bMakeThumbnail     %d]\n", stSetup.bMakeThumbnail);
		HxLOG_Debug ("[bIsUsbFlashRecord  %d]\n", stSetup.bIsUsbFlashRecord);
		HxLOG_Debug ("[bStartPaused	      %d]\n", stSetup.bStartPaused);
		HxLOG_Debug ("------------<XMGR_PLAYBACK:SETUP>------------\n");
#endif

		hErr = SVC_PB_Setup(pstContext->hAction, &stSetup, TRUE);
		if (hErr != ERR_OK)
		{
			HxLOG_Info("Player(id:%d) didn't initiate...  SvcPb_Open() handle_t=%x \n", ucPlayIdx, pstContext->hAction);
		//	goto FINISH;
		}
	}

#if defined(CONFIG_MW_CAS_NAGRA)
	{
		HBOOL			bPossibleSCPincode = FALSE;

		bPossibleSCPincode = xmgr_playback_CheckPossibleSCPincode_Base(pstContext->ulActionId);
		if(bPossibleSCPincode == TRUE)
		{
			pstContext->bSCPinCodeValid = SVC_CAS_CheckSmartCardValidity(eDxCAS_GROUPID_NA);
		}
		else
		{
			pstContext->bSCPinCodeValid = FALSE;
		}
	}

	{
		HBOOL		bScRemoved = FALSE;
		hErr = MGR_CAS_IsSmartcardRemoved(0, &bScRemoved);
		if (ERR_OK == hErr && TRUE == bScRemoved)
		{
			pstContext->nCasErrCode = 1;

			if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
			{
				SVC_AV_SetVideoHide(pstContext->ulActionId, eAvMuteFlag_Cas, TRUE);
				SVC_AV_SetAudioHide(pstContext->ulActionId, eAvMuteFlag_Cas, TRUE);
			}
		}
	}
#endif

	// Standby 에서 Thumbnail 을 만들기 위해 시작한 playback 이 아니면 Panel 에 Display
	if ( MGR_ACTION_GetState() != eAmState_ACTIVE_STANDBY )
	{
		if(eActionType == eViewType_PB)
		{
			SVC_SYS_SetLedOnOff(eDxLED_ID_PLAY, TRUE);
			SVC_SYS_SetLedOnOff(eDxLED_ID_PAUSE, FALSE);
			SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, "", eDxPANEL_ALIGN_CENTER);
		}
		BUS_SetTimer(PANEL_STRING_TIMER_ID, PANEL_STRING_TIME_OUT);
	}

	BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_PLAYBACK_STARTED, pstContext->hAction, 0, 0, 0);
#if defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
	if(pstContext->nCasErrCode)
		pstContext->ePbState = eXmgrPbState_CAS_BLOCKED;
#endif

	/* thumbnail */
#if defined(CONFIG_APCORE_MOVING_THUMBNAIL_CACHE)
	if (bRecord == TRUE)
	{
		// 녹화가 진행중이라서 rec time이 계속 변하는 경우... to be continue
	}
	else
	{
		// 녹화가 완료되어 rec time이 고정된 경우
		// 최소 간격 5초, 최대 개수 MAX_PB_THUMB
		pstContext->ulThumbInterval = (pstContext->ulEndTime / MAX_PB_THUMB) + 1;

		if (pstContext->ulThumbInterval < 5)
		{
			pstContext->ulThumbInterval = 5;
		}

		pstContext->ulThumbNum = pstContext->ulEndTime / pstContext->ulThumbInterval;
	}

	BUS_SetTimer(PB_EXTRACT_THUMB_TIMER_ID, PB_EXTRACT_THUMB_TIME);
#endif

	SVC_META_RegisterPvrAction(pstContext->hAction, pstContext->ulContentId, eSvcMeta_UsingActType_Playback);

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_playback_MsgBusDestroy_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR					hErr;
	HUINT32					ulActionId = pstContext->ulActionId;
	HBOOL					bRecord = FALSE;
	MgrRec_RecInfo_t		stRecInfo;
	HBOOL					bClearContext = FALSE;

	if (pstContext->ePbState != eXmgrPbState_IDLE)
	{
		bRecord = MGR_RECORDER_IsScrambleToDescrambleRecord(&stRecInfo);
		if(bRecord == TRUE)
		{
			BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, stRecInfo.hAction, 0, 0, 0);
		}
		/* destroy the player */
		HxLOG_Info("Player(%d) : eMEVT_BUS_DESTROY\n", pstContext->ucPlayIdx);

		pstContext->ePbState = eXmgrPbState_IDLE;

		/* 혹시 떠 있을 screen saver 없애고 */
		BUS_KillTimer(PB_EOF_TIMER_ID);

		xmgr_playback_StopSubtitle_Base(pstContext);
#if defined(CONFIG_MW_TELETEXT) && 0
		BUS_MGR_Destroy(PROC_Teletext_PB);
#endif

		/* STOP SI */
		SVC_SI_StopPb(pstContext->hAction, TRUE);


		HxLOG_Print("bCasStarted Flag : [%x]\n", pstContext->bCasStarted);
		if(pstContext->bCasStarted == TRUE)
		{
			hErr = MGR_CAS_StopService(pstContext->hAction, HANDLE_NULL, eSyncMode);
			if (hErr != ERR_OK)
			{
				HxLOG_Error ("MGR_CAS_StopService() failed.. \n");
			}
		}

#if defined(CONFIG_MW_SUBTITLE)
		hErr = SVC_SBTL_StopService(pstContext->hAction, eSyncMode);
		if (hErr != ERR_OK)
		{
			HxLOG_Error ("SVC_SBTL_StopService()	failed.. \n");
		}
#endif
#if defined(CONFIG_MW_TELETEXT)
		hErr = SVC_TTX_StopService(pstContext->hAction, eSyncMode);
		if (hErr != ERR_OK)
		{
			HxLOG_Error ("SVC_TTX_StopService()  failed.. \n");
		}
#endif

		/* disable radio still */
		if (pstContext->bRadioBackground)
		{
			BUS_SendMessage (NULL, eMEVT_SVCLIST_RADIOBG_CHANGED, pstContext->hAction, (HINT32)FALSE, 0, 0);
		}

		/* save timestamp to meta */
		{
			HUINT32	ulPlaybackTime	=	0;

			hErr = SVC_META_UnloadTimeStamp (&(pstContext->stTimeStampList));
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_META_UnloadTimeStamp Error. \n");
			}

			hErr = SVC_PB_GetPlayTime (pstContext->hAction, &ulPlaybackTime);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SVC_PB_GetPlayTime Error. (%x)\n", hErr);
				//레코딩을 실패하여 playback 자체를 실행못하는 case에 대해서는 playtime을 못가져오므로...막습니다..
				//return hErr;
			}

			hErr = SVC_META_SetLastPlayTime (pstContext->ulContentId, ulPlaybackTime);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SVC_META_SetLastPlayTime Error. (%x)\n", hErr);
				//레코딩을 실패하여 playback 자체를 실행못하는 case에 대해서는 playtime을 못가져오므로...막습니다..
				//return hErr;
			}
		}

		/* stop the player */
		SVC_PB_Stop(pstContext->hAction, TRUE);

		if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
		{
			SVC_AV_StopService(pstContext->ulActionId, eSyncMode);
		}

		MGR_OUTPUT_ResetCopyProtect(ulActionId);

		SVC_AV_SetVideoHide(MGR_ACTION_GetMainActionId(), eAvMuteFlag_All, FALSE);
		SVC_AV_SetAudioHide(MGR_ACTION_GetMainActionId(), eAvMuteFlag_All, FALSE);

		SVC_PB_Close(pstContext->hAction, TRUE);

		if ( MGR_ACTION_GetState() != eAmState_ACTIVE_STANDBY )
		{
			SVC_SYS_SetLedOnOff(eDxLED_ID_DOLBY, FALSE);
			SVC_SYS_SetLedOnOff(eDxLED_ID_HD, FALSE);
			SVC_SYS_SetLedOnOff(eDxLED_ID_PLAY, FALSE);
			SVC_SYS_SetLedOnOff(eDxLED_ID_PAUSE, FALSE);

			BUS_KillTimer(PANEL_STRING_TIMER_ID);
			SVC_SYS_ClearPanelString(ePANEL_STR_PRIORITY_VIEW_ACTION);
		}

		if (pstContext->bIsPbStopByError == TRUE)
		{
			BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_PLAYBACK_ERROR_STOPPED, pstContext->hAction, 0, 0, 0);
		}
		else
		{
			BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_PLAYBACK_STOPPED, pstContext->hAction, 0, 0, 0);
		}

		/* Action 을 릴리즈 함. */
		//if (xmgr_playback_CheckAvDecUsed_Base (pstContext) || xmgr_playback_CheckSectionGathering_Base(pstContext))
		{
			switch (pstContext->ePbType & 0x0000000f)
			{
				case eMgrPb_PlayType_Tsr:
					hErr = MGR_ACTION_Release(ulActionId, eViewType_TSR_PB);
					break;
				case eMgrPb_PlayType_Chase:
					hErr = MGR_ACTION_Release(ulActionId, eViewType_CHASE_PB);
					break;
				case eMgrPb_PlayType_Normal:
				default :
					hErr = MGR_ACTION_Release(ulActionId, eViewType_PB);
					if((pstContext->ulPlayTime + 3) < pstContext->ulEndTime)
					{
						SVC_META_SetLastPlayTime(pstContext->ulContentId, pstContext->ulPlayTime);
					}
					break;
			}
		}

		/* re-init player app pstContext */
		pstContext->uiState = eMgrPb_NONE;

		if(xmgr_playback_CheckSectionGathering_Base(pstContext))
		{
			BUS_PostMessage (NULL, eMEVT_PVR_TRREC_SECTION_GATHERING_FINISHED, HANDLE_NULL, 0, 0, 0);
		}

		bClearContext = TRUE;
	}

	SVC_META_UnRegisterPvrAction(pstContext->hAction);

	if (bClearContext == TRUE)
	{
		HxSTD_MemSet(pstContext, 0, sizeof(xmgrPbContext_t));
		pstContext->hMasterSvc = HANDLE_NULL;
		pstContext->hSvc = HANDLE_NULL;
		pstContext->ulEventId = (HUINT32)-1;
		pstContext->bIsEOF = FALSE;
	}


	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_playback_MsgBusTimer_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR		hErr;
	Handle_t	hRecAction 	= HANDLE_NULL;

#if defined(CONFIG_APCORE_MOVING_THUMBNAIL_CACHE)
	HUINT32 ulTime;
	HUINT8 szThumbFile[PB_THUMBNAIL_FILENAME_LEN];
	HBOOL bTsr;
#endif

	if(pstContext != NULL)
	{
		switch (nParam1)
		{
		case PB_EOF_TIMER_ID:
			if(pstContext->bIsEOF == TRUE)
			{

				BUS_KillTimer(PB_EOF_TIMER_ID);

				// 재생 종료이므로 더이상 ContentList 에 유지할 필요가 없으므로 Skip Flag 를 FALSE 로 한다.
				// VideoList 를 Show 시 기존 ContentList 를 SVC_PVR_UnloadContentsInfo 로 해제하는데, Skip Flag 를 TRUE 로 하면 해재되지 않고 유지된다. FALSE 이면 해제
				hErr = SVC_META_GetRecActionByContentId (pstContext->ulContentId, &hRecAction);
				if ((ERR_OK != hErr) || (HANDLE_NULL == hRecAction))
				{
					hErr = SVC_META_SetAsDeleteSkipContent (pstContext->ulContentId, FALSE);
				}

				BUS_MGR_Destroy (NULL);
			}

			return MESSAGE_BREAK;

		case PANEL_STRING_TIMER_ID:
			SVC_SYS_DisablePanelAutoDisableOneTime();
			xmgr_playback_SetPanelStringDisplay_Base(pstContext);
			return MESSAGE_BREAK;

		case PB_RESERVED_TRICK_START_TIMER_ID:
			BUS_KillTimer (PB_RESERVED_TRICK_START_TIMER_ID);
			if (TRUE == pstContext->bSpeedReserved)
			{
				MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, pstContext->nReservedSpeed);
				pstContext->bSpeedReserved = FALSE;
			}
			return MESSAGE_BREAK;

		case PB_RESERVED_JUMP_START_TIMER_ID:
			BUS_KillTimer (PB_RESERVED_JUMP_START_TIMER_ID);
			if (TRUE == pstContext->bJumpReserved)
			{
				MGR_PLAYBACK_SetJumpPlay (pstContext->ulActionId, pstContext->nReservedJump);
				pstContext->bJumpReserved = FALSE;
			}
			return MESSAGE_BREAK;

#if defined(CONFIG_APCORE_MOVING_THUMBNAIL_CACHE)
		case PB_EXTRACT_THUMB_TIMER_ID:
			BUS_KillTimer(PB_EXTRACT_THUMB_TIMER_ID);

			while (pstContext->ulNextThumbIdx < pstContext->ulThumbNum)
			{
				if (pstContext->aucThumb[pstContext->ulNextThumbIdx] == FALSE)
				{
					ulTime = pstContext->ulThumbInterval * pstContext->ulNextThumbIdx;

					HxSTD_snprintf(szThumbFile, PB_THUMBNAIL_FILENAME_LEN, "%s%d.%s", PB_THUMBNAIL_DEFAULT_PATH, ulTime, PB_THUMBNAIL_DEFAULT_EXT);

					bTsr = (pstContext->ePbType & eMgrPb_PlayType_Tsr)? TRUE: FALSE;

					if (!HLIB_FILE_Exist(szThumbFile))
						SVC_PVR_ExtractThumbnail(pstContext->ulContentId, bTsr, pstContext->ulVideoDecId, ulTime, szThumbFile);

					pstContext->aucThumb[pstContext->ulNextThumbIdx] = TRUE;
					BUS_SetTimer(PB_EXTRACT_THUMB_TIMER_ID, PB_EXTRACT_THUMB_TIME);
					break;
				}

				pstContext->ulNextThumbIdx++;
			}
			return MESSAGE_BREAK;
#endif

		default:
			break;
		}
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgAmActionStopped_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	NOT_USED_PARAM (pstContext);
	NOT_USED_PROC_ARGUMENT (hAction, nParam1, nParam2, nParam3);
	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgAppRecordStop_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HUINT32 		ulContentId;
	MgrRec_RecordType_e	eRecType;
	HERROR					hError = ERR_FAIL;
	MgrPb_Info_t	stPlayInfo = {0, };
	SvcMeta_Record_t			stRecInfo;

	eRecType = nParam1;
	ulContentId = nParam2;

	if(eRecType == (MgrRec_RecordType_e)eRecType_TSR_REC)
	{
		if((pstContext->ulContentId == ulContentId) && ((pstContext->ePbType & 0x0000000f) == eMgrPb_PlayType_Tsr))
		{
			// TSR PB를 멈춘다.
			BUS_MGR_Destroy (NULL);
		}
	}
	else
	{
		if((pstContext->ulContentId == ulContentId) && ((pstContext->ePbType & 0x0000000f) == eMgrPb_PlayType_Chase))
		{
			pstContext->ePbType = eMgrPb_PlayType_Normal;
			// MGR_ACTION_ChangeChasePbToNormalPb(pstContext->ulActionId);
			MGR_ACTION_ChangeType (pstContext->ulActionId, eViewType_CHASE_PB, eViewType_PB);

			HxSTD_MemSet(&stPlayInfo, 0, sizeof(MgrPb_Info_t));
			HxSTD_MemSet(&stRecInfo, 0, sizeof(SvcMeta_Record_t));

			hError = MGR_PLAYBACK_GetPlaybackInfo(pstContext->ulActionId, &stPlayInfo);
			if (hError == ERR_OK)
			{
				if (SVC_META_GetRecordInfo(stPlayInfo.ulContentId, &stRecInfo) == ERR_OK)
				{
					if (stPlayInfo.ulEndTime != stRecInfo.ulRecTime)
					{
						stPlayInfo.ulEndTime = stRecInfo.ulEndTime - stRecInfo.ulStartTime;
						xmgr_playback_UpdatePlaybackEndTimeInfo_Base(pstContext->ulActionId, &stPlayInfo);
					}
				}
			}
		}
	}

	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgAppDbPidChanged_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgr_playback_ChangePid_Base(pstContext);

	// kangaroo
	// check subtitle
	xmgr_playback_ChangeSubtitle_Base(pstContext);

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgAppPinOK_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR hErr = ERR_FAIL;

	if(pstContext->ePbState == eXmgrPbState_LOCK_PIN)
	{
		pstContext->bLockPinPassed = TRUE;

		HxLOG_Info("\t=> [eXmgrPbState_LOCK_PIN] Saved Service Lock \n");
		MGR_LIVE_SvcLockSetPassed(pstContext->ulActionId, pstContext->hSvc);

		if(pstContext->bRatingValid == TRUE)
		{ /* Lock check 중 rating value가 들어왔으면 rating PIN까지도 통과한 것으로 처리해야 함. */
			HxLOG_Info("\t=> [eXmgrPbState_LOCK_PIN]Rating value already arrived. Regard rating PIN passed, too.\n");
			pstContext->bRatingPinPassed = TRUE; /* 바로 pass 처리함... */
		}
		else
		{ /* Rating value 결정 이전에 lock pass하면 바로 다음 event는 rating pass로 하기 위하여 flag setting. */
			pstContext->bRatingPassByLockPass = TRUE;
		}
	}
	else if(pstContext->ePbState == eXmgrPbState_RATING_PIN)
	{
		HxLOG_Info("\t=> [eXmgrPbState_RATING_PIN]Rating value already arrived. Regard rating PIN passed, too.\n");
		pstContext->bRatingPinPassed = TRUE; /* 바로 pass 처리함... */
		pstContext->bSCPinPassed = TRUE; /* 바로 pass 처리함... */
	}
	else if(pstContext->ePbState == eXmgrPbState_WAIT_PB_LOCK)
	{
		HxLOG_Info("\t=> [eXmgrPbState_WAIT_PB_LOCK]\n");
#if defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
		if(pstContext->nCasErrCode == 1)
		{
		    	xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_CAS_BLOCKED);
			return MESSAGE_BREAK;
		}
#endif
		pstContext->ePbState = eXmgrPbState_WAIT_AV_STARTED;

		if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
		{
#if !defined(SvcAv_START_SERVICE_NEW)
			hErr = SVC_AV_SetStereoSelect(pstContext->ulActionId, pstContext->svcInfo.eSoundMode);
			if (hErr)
			{
				HxLOG_Error("SVC_AV_SetStereoSelect ret :0x%X\n",hErr);
			}
#endif
			hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, pstContext->svcInfo.eDualMono);
			if (hErr)
			{
				HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
			}

#if defined(SvcAv_START_SERVICE_NEW)
			hErr = SVC_AV_StartService(pstContext->hAction,
										pstContext->ulVideoDecId,
										pstContext->ulDemuxId,
										pstContext->svcInfo.usAudioPid,
										pstContext->svcInfo.usAudioAuxPid,
										pstContext->svcInfo.usVideoPid,
										pstContext->svcInfo.usPcrPid,
										pstContext->svcInfo.eAudioCodec,
										pstContext->svcInfo.eAudioAuxCodec,
										pstContext->svcInfo.eVideoCodec,
										FALSE,
										FALSE
										);
#else
			hErr = SVC_AV_StartService(pstContext->hAction,
										pstContext->ulVideoDecId,
										pstContext->ulDemuxId,
										pstContext->svcInfo.usAudioPid,
										pstContext->svcInfo.usAudioAuxPid,
										pstContext->svcInfo.usDolbyPid,
										pstContext->svcInfo.usVideoPid,
										pstContext->svcInfo.usPcrPid,
										pstContext->svcInfo.eAudioCodec,
										pstContext->svcInfo.eAudioAuxCodec,
										pstContext->svcInfo.eDolbyCodec,			//2009.03.23 추가.
										pstContext->svcInfo.eVideoCodec,
										pstContext->svcInfo.bDolbyFlag,
										pstContext->svcInfo.eSoundMode,
										pstContext->svcInfo.eDualMono,
										FALSE,
										FALSE
										);
#endif
			if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
		}

		xmgr_playback_EnableOutput(pstContext);

		return MESSAGE_BREAK;
	}
	else
	{
		HxLOG_Print("\t=> Unexpected message in this state(%s). Ignore this message ...\n",
											xmgr_playback_GetAppStateStr_Base(pstContext->ePbState));
		return MESSAGE_BREAK;
	}

	xmgr_playback_CheckRating_Base(pstContext);

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgAppPinDenied_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	/* PIN deny는 다른 변경 없이 UI 만 없어진 것이므로 UI만 다시 생성해준다. */
	if(pstContext->ePbState == eXmgrPbState_LOCK_PIN)
	{
		xmgr_playback_SetUiState_Base(pstContext, eMgrPb_LOCK_PIN_DENIED);
	}
	else if(pstContext->ePbState == eXmgrPbState_RATING_PIN)
	{
		xmgr_playback_SetUiState_Base(pstContext, eMgrPb_RATING_PIN_DENIED);
	}
	else if(pstContext->ePbState == eXmgrPbState_WAIT_PB_LOCK)
	{
		pstContext->uiState = eMgrPb_PB_LOCK_DENIED;
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgAppCheckRating_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if( (pstContext->ePbState == eXmgrPbState_AV) ||
		(pstContext->ePbState == eXmgrPbState_LOCK_PIN) ||
		(pstContext->ePbState == eXmgrPbState_RATING_PIN) )
	{
		/* 이 세 state에서는 rating을 다시 체크해 봐야 하고... */

		pstContext->bLockPinPassed = FALSE; /* User에 의해 rating이 바뀐 상황이므로 먼저 넣은 PIN passed flag는 reset 한다. */
		pstContext->bRatingPinPassed = FALSE; /* User에 의해 rating이 바뀐 상황이므로 먼저 넣은 PIN passed flag는 reset 한다. */
		xmgr_playback_CheckRating_Base(pstContext);
	}
	else
	{
		/* 다른 state에서는 그냥 기록만 해 둔다. */
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtPbSetup_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR					hErr;

	if(nParam1 != ERR_OK)
	{
		HxLOG_Warning("xmgr_playback_EvtPbSetup Fail\n");
		pstContext->bIsPbStopByError = TRUE;

		switch (pstContext->ePbType & 0x0000000f)
		{
			case eMgrPb_PlayType_Tsr:
				xmgr_playback_StopTsrPlayback_Base (pstContext->ulActionId);
				break;
			case eMgrPb_PlayType_Chase:
				xmgr_playback_StopChasePlayback_Base (pstContext->ulActionId);
				break;
			case eMgrPb_PlayType_Normal:
			default :
				if(MGR_MakeThumbnail_IsAvailable() == TRUE)
				{
					BUS_SendMessage(MGR_MakeThumbnail_GetProc(), eMEVT_PVR_MEDIA_NEXT_FILE, 0, 0, 0, 0);
				}
				else
				{
					MGR_PLAYBACK_StopPlayback (pstContext->ulActionId);
				}
				break;
		}
	}
	else
	{
		if(pstContext->ePbState == eXmgrPbState_WAIT_PB_SETUP)
		{
			{
				SvcSi_PbSpec_t stSiSpec;
				SvcSi_PlaybackOption_t stPbOption;

				hErr = MGR_PLAYBACK_GetPbSiSpec (&stSiSpec);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("MGR_LIVE_GetLiveSiSpec hErr: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->hSvc);
					return ERR_FAIL;
				}

				stPbOption.usTsId	=	pstContext->svcInfo.usTsId;
				stPbOption.usSvcId	=	pstContext->svcInfo.usSvcId;
				stPbOption.usPmtPid	=	pstContext->svcInfo.usPmtPid;

				HxLOG_Debug(HxANSI_COLOR_YELLOW("stPbOption.usTsId   = 0x%x\n"), stPbOption.usTsId);
				HxLOG_Debug(HxANSI_COLOR_YELLOW("stPbOption.usSvcId  = 0x%x\n"), stPbOption.usSvcId);
				HxLOG_Debug(HxANSI_COLOR_YELLOW("stPbOption.usPmtPid = 0x%x\n"), stPbOption.usPmtPid);

				hErr = SVC_SI_StartPb (pstContext->hAction, &stSiSpec, &stPbOption, FALSE);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_SI_StartPb hErr: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->hSvc);
					return ERR_FAIL;
				}
			}


			if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
			pstContext->ePbState = eXmgrPbState_WAIT_AV_STARTED;
			pstContext->bIsPbStopByError = FALSE;


			switch (pstContext->ePbType & 0x0000000f)
			{
				case eMgrPb_PlayType_Tsr:
				case eMgrPb_PlayType_Chase:
#if defined(CONFIG_OP_NORDIG)
				case eMgrPb_PlayType_Normal:
#endif
					xmgr_playback_DisableOutput_Base(pstContext);
					xmgr_playback_GetEitInfo_Base(pstContext);
					break;

				default:
					xmgr_playback_EnableOutput(pstContext);
					break;
			}

			if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
			{
#if !defined(SvcAv_START_SERVICE_NEW)
				hErr = SVC_AV_SetStereoSelect(pstContext->ulActionId, pstContext->svcInfo.eSoundMode);
				if (hErr)
				{
					HxLOG_Error("SVC_AV_SetStereoSelect ret :0x%X\n",hErr);
				}
#endif
				hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, pstContext->svcInfo.eDualMono);
				if (hErr)
				{
					HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
				}

				// 저장된 service 정보의 PMT 로 사용한다.
#if defined(SvcAv_START_SERVICE_NEW)
				hErr = SVC_AV_StartService(pstContext->hAction,
											pstContext->ulVideoDecId,
											pstContext->ulDemuxId,
											pstContext->svcInfo.usAudioPid,
											pstContext->svcInfo.usAudioAuxPid,
											pstContext->svcInfo.usVideoPid,
											pstContext->svcInfo.usPcrPid,
											pstContext->svcInfo.eAudioCodec,
											pstContext->svcInfo.eAudioAuxCodec,
											pstContext->svcInfo.eVideoCodec,
											FALSE,
											FALSE
											);
#else
				hErr = SVC_AV_StartService(pstContext->hAction,
											pstContext->ulVideoDecId,
											pstContext->ulDemuxId,
											pstContext->svcInfo.usAudioPid,
											pstContext->svcInfo.usAudioAuxPid,
											pstContext->svcInfo.usDolbyPid,
											pstContext->svcInfo.usVideoPid,
											pstContext->svcInfo.usPcrPid,
											pstContext->svcInfo.eAudioCodec,
											pstContext->svcInfo.eAudioAuxCodec,
											pstContext->svcInfo.eDolbyCodec,			//2009.03.23 추가.
											pstContext->svcInfo.eVideoCodec,
											pstContext->svcInfo.bDolbyFlag,
											pstContext->svcInfo.eSoundMode,
											pstContext->svcInfo.eDualMono,
											FALSE,
											FALSE
											);
#endif
			}
			else
			{
				hErr = SVC_PB_Start (pstContext->hAction, TRUE);
			}

#if	defined(CONFIG_IGNORE_TRICKPLAY_IN_PMTCHANGED_SERVCE)
			if (nParam2)
				pstContext->bTrickEnable = FALSE;
			else
				pstContext->bTrickEnable = TRUE;
#endif
		}
	}

	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtAvDecodingStarted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR			hErr;

	HxLOG_Print("--------------------> GOGOGOG <------------------\n");
	if(pstContext->ePbState == eXmgrPbState_WAIT_AV_STARTED)
	{
		if(nParam1 != ERR_OK)
		{
			HxLOG_Warning("xmgr_playback_EvtAvDecodingStarted Fail\n");
			switch (pstContext->ePbType & 0x0000000f)
			{
				case eMgrPb_PlayType_Tsr:
					xmgr_playback_StopTsrPlayback_Base (pstContext->ulActionId);
					break;
				case eMgrPb_PlayType_Chase:
					xmgr_playback_StopChasePlayback_Base (pstContext->ulActionId);
					break;
				case eMgrPb_PlayType_Normal:
				default :
					if(MGR_MakeThumbnail_IsAvailable() == TRUE)
					{
						BUS_SendMessage(MGR_MakeThumbnail_GetProc(), eMEVT_PVR_MEDIA_NEXT_FILE, 0, 0, 0, 0);
					}
					else
					{
						MGR_PLAYBACK_StopPlayback (pstContext->ulActionId);
					}
					break;
			}
		}
		else
		{
			/* start the player */
			hErr = SVC_PB_Start(pstContext->hAction, TRUE);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("SVC_PB_Start failed. hAction(%d) \n",pstContext->hAction);
			}
			pstContext->ePbState = eXmgrPbState_WAIT_PMT;
		}
	}

	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtPbStart_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if(pstContext)
	{
		if(nParam1 != ERR_OK)
		{
			pstContext->bIsPbStopByError = TRUE;

			HxLOG_Warning("xmgr_playback_EvtPbStart Fail\n");
			switch (pstContext->ePbType & 0x0000000f)
			{
				case eMgrPb_PlayType_Tsr:
					xmgr_playback_StopTsrPlayback_Base (pstContext->ulActionId);
					break;
				case eMgrPb_PlayType_Chase:
					xmgr_playback_StopChasePlayback_Base (pstContext->ulActionId);
					break;
				case eMgrPb_PlayType_Normal:
				default :

					if(MGR_MakeThumbnail_IsAvailable() == TRUE)
					{
						BUS_SendMessage(MGR_MakeThumbnail_GetProc(), eMEVT_PVR_MEDIA_NEXT_FILE, 0, 0, 0, 0);
					}
					else
					{
						MGR_PLAYBACK_StopPlayback (pstContext->ulActionId);
					}
					break;
			}
		}
		else
		{
			/* start the player */
			pstContext->bPBStart = TRUE;

			xmgr_playback_StartReservedCtrl_Base (pstContext);
		}
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtSiPmtReceived_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR			hErr;
	HUINT32			ulMarginedTime = 0;
	HUINT32			ulPlayTime = 0;

	/* Do not use context's playtime (because while in trick mode, context's playtime might outdated)*/
	hErr = SVC_PB_GetPlayTime(pstContext->hAction, &ulPlayTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_PB_GetPlayTime failed. hAction(0x%X) \n",pstContext->hAction);
	}

	/* Si's PMT signal and recording meta's PMT time-stamp time might differ. example: When Recording, wrote PMT stamp 725. But when playback, SI's PMT signal happens at 715.
	    adjust time that checking with time-stamp */
	ulMarginedTime = xmgr_playback_GetMarginedPmtTime(pstContext->nPlaySpeed,ulPlayTime);
	(void)xmgr_playback_CheckTimeStamp_Base(pstContext, ulMarginedTime);

	if(pstContext->bPmtValid != TRUE)
	{ /* 첫 번째 PMT들어오는 경우 처리... */
		/* 먼저 PMT valid 표시를 해야 subtitle을 시작할 수 있다. */
		pstContext->bPmtValid = TRUE;

	//	/* 처음 들어온 PMT인 경우 subtitle을 start한다. */
		xmgr_playback_CheckSbtlAllowed_Base(pstContext);

		/* menu의 audio priority 고려하여 변경 */
		xmgr_playback_CheckMetaInfoAndChangePid(pstContext);
	}

	pstContext->bFirstPmt = TRUE;

	if(pstContext->ePbState == eXmgrPbState_WAIT_PMT)
	{ /* PMT를 기다렸다가 진행햐야 하는 경우에 걸려 있다. PMT를 받았으니 이제 진행하면 됨. */
		hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, &pstContext->svcInfo); /* New PMT 반영된 service info 다시 읽음. */
		if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

		xmgr_playback_StartPid_Base(pstContext);
	}
	else
	{
		/* AV/Teletext PID 변경 검사. pstContext->svcInfo를 먼저 update하면 안됨. 그러면 변경 확인이 안되니 변경 확인 후 update하도록 해야 함. */
		xmgr_playback_ChangePid_Base(pstContext);
		hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, &pstContext->svcInfo); /* New PMT 반영된 service info 다시 읽음. */
		if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
	}

	xmgr_playback_StartReservedCtrl_Base (pstContext);

	/* PID말고 다른 처리할 것이 있으면 여기다 하라. */
	xmgr_playback_SetPanelLedDisplay_Base(pstContext);

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtSiPmtTimeout_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtSiPmtPidChanged_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	/*
	// EVT_PVR_PMT_PIDCHANGED :
				SI service로부터의 PMT PID 변경 이벤트. PID가 다른 값으로 바뀜.
				새 PMT PID가 AV나 다른 콤포넌트의 PID와 겹칠 수도 있기 때문에 다 털어내야 한다.
				SI는 PMT를 자체적으로 걸지 말고 이 이벤트만 날리고 wtv의 처분을 기다려야 한다.
	*/

	HERROR			hErr;
	SvcMeta_Record_t		stRecordInfo;

	if (pstContext->eTrickState != eMgrPb_TickState_PLAY)
	{
		pstContext->ePrevTrickState = pstContext->eTrickState;
		pstContext->ePrevTrickSpeed = pstContext->eTrickSpeed;

		//	원상 복구는 우선 막음. 테스트가 제대로 안됨.
		//pstContext->eValidPrevTrick = TRUE;

		HxLOG_Print("\n\nForced Trick Play mode to NORMAL\n\n");
		//XMGR_PB_SetTrickState(pstContext->ulActionId, eMgrPb_TickState_PLAY, eMgrPb_Speed_X1);
		MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 100);
	}

	if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
	{
		hErr = SVC_AV_StopService(pstContext->ulActionId, eSyncMode);
		if(ERR_OK!=hErr) 		HxLOG_Critical("Critical Eror!!!\n");
	}

	xmgr_playback_StopSubtitle_Base(pstContext);
	pstContext->bRestartAV = TRUE;

	if(pstContext->bCasStarted == TRUE)
	{
		/* PMT PID가 바뀌면 모든 PID를 clear시켜야 하므로 CAS를 stop 시키고 ... (AV는 위에서 이미 stop됨) */
		hErr = MGR_CAS_StopService(pstContext->hAction, HANDLE_NULL, eSyncMode);
		if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
	}

#if defined(CONFIG_MW_TELETEXT)
	hErr = SVC_TTX_StopService(pstContext->hAction, eSyncMode);
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
#endif

	hErr = SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecordInfo);
	if((hErr == ERR_OK) && (stRecordInfo.u1cScrambled == 1))
	{
		/* CAS를 다시 start 시킨 후 PMT를 기다린다. */
		if(eMgrPb_PlayType_Tsr == (pstContext->ePbType & 0x0000000f))
		{
			hErr = MGR_CAS_StartService(pstContext->hAction, HANDLE_NULL, eCAS_ActionType_Tsr_Play, eDxCAS_GROUPID_NONE, NULL);
		}
		else
		{
			hErr = MGR_CAS_StartService(pstContext->hAction, HANDLE_NULL, eCAS_ActionType_Playback, eDxCAS_GROUPID_NONE, NULL);
		}
		pstContext->bCasStarted = TRUE;
	}
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

	pstContext->bPmtValid = FALSE; /* 현재 PMT validity는 reset해야 함. */
	xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_WAIT_PMT);

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtSiEitReceived_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR			hErr;
	SvcMeta_Event_t 	stEvtInfo;
	HUINT32			ulEventId = (HUINT32)-1;
	HBOOL			bFindRate = FALSE;

	HxSTD_MemSet(&stEvtInfo, 0, sizeof(SvcMeta_Event_t));
	/* Rating value를 알아본다. */

	hErr = SVC_META_GetEventInfo(pstContext->ulContentId, &stEvtInfo);
	if(hErr == ERR_OK)
	{/* Event ID를 등록한다. */
		ulEventId = (HUINT32)stEvtInfo.nEventId;

		{
			DbSvc_TripleId_t	stTripleId;
			SvcSi_Event_t		stSiEvtInfo;

			HxSTD_MemSet(&stSiEvtInfo, 0, sizeof(SvcSi_Event_t) );

			stTripleId.usOnId	= pstContext->svcInfo.usOnId;
			stTripleId.usTsId	= pstContext->svcInfo.usTsId;
			stTripleId.usSvcId	= pstContext->svcInfo.usSvcId;

			hErr = SVC_SI_GetEvtInfoFromPfEvent(pstContext->ulActionId, stTripleId, TRUE, TRUE, &stSiEvtInfo, NULL);
			if( ERR_OK == hErr )
			{
				if (stSiEvtInfo.ucParentRating > 0)
				{
					bFindRate = TRUE;
					pstContext->ulRatingValue = stSiEvtInfo.ucParentRating;
				}
			}

			HxLOG_Print("\t=> ulEventId [0x%x] to stSiEvtInfo.nEvtId[0x%x].\n", ulEventId, (HUINT32)stSiEvtInfo.nEvtId);

			ulEventId = (HUINT32)stSiEvtInfo.nEvtId;

			if (SVC_SI_FreeEventInfo(&stSiEvtInfo) != ERR_OK)
			{
				HxLOG_Error("SVC_SI_FreeEventInfo() failed..! \n");
			}

			HxLOG_Info("pstContext->ulRatingValue = %d\n", pstContext->ulRatingValue);
		}

		if(bFindRate == FALSE)
		{
			// 첫번째 Parental Rate를 사용할지는 향마다 다르다.
			if(stEvtInfo.ucParentalRatingNum > 0)	//  첫번째 Parental Rate를 사용
			{
				pstContext->ulRatingValue = stEvtInfo.astParentalRating[0].ucParentalRating;
			}
			else
			{
				pstContext->ulRatingValue = 0;
			}
			HxLOG_Info("\t=> Event ID changed [0x%x]. NoParentalRate !!! ucParentRating=%d \n", ulEventId, pstContext->ulRatingValue);
		}
	}
	else
	{ /* Event ID가 없으면 -1로 초기화. */
		HxLOG_Info("\t=> No event ID in EIT\n");
		ulEventId = (HUINT32)-1;
		pstContext->ulRatingValue = 0;
	}

	xmgr_playback_ProcessParentalRate_Base(pstContext, bFindRate);

	if(ulEventId != pstContext->ulEventId) /* event ID가 달라졌으면 rating PIN passed flag reset. */
	{
		HxLOG_Print("\t=> Event ID changed from [0x%x] to [0x%x].\n", pstContext->ulEventId, ulEventId);
		pstContext->bRatingPinPassed = FALSE;
		pstContext->bSCPinPassed = FALSE; /* APS 사양 AS5101 8.5.1.4 */
		pstContext->ulEventId = ulEventId; /* Event ID 갱신 */
	}

	pstContext->bRatingValid = TRUE;
	pstContext->bAlreadyExistEit = TRUE;

	/* Lock pass에 의한 rating pass 처리 */
	if(pstContext->bRatingPassByLockPass == TRUE)
	{ /* Lock pass에 의하여 rating도 pass하라는 flag가 있음. */
		HxLOG_Print("\t=> Rating PIN is set passed by lock PIN pass.\n");
		pstContext->bRatingPinPassed = TRUE;
		pstContext->bRatingPassByLockPass = FALSE; /* 한번 쓴것은 꼭 버려야 한다. 안 그러면 매번 통과한다. */
	}

	if( (pstContext->ePbState == eXmgrPbState_AV) ||
		(pstContext->ePbState == eXmgrPbState_WAIT_RATING) ||
		(pstContext->ePbState == eXmgrPbState_RATING_PIN) )
	{
		/* 이 세 state에서는 rating을 다시 체크해 봐야 하고... */
		xmgr_playback_CheckRating_Base(pstContext);
	}
	else
	{
		/* 다른 state에서는 그냥 기록만 해 둔다. */
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtSiEitTimeout_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	switch (pstContext->ePbType & 0x0000000f)
	{
		case eMgrPb_PlayType_Tsr:
		case eMgrPb_PlayType_Chase:
#if defined(CONFIG_OP_NORDIG)
		case eMgrPb_PlayType_Normal:
#endif
			break;

		default:
			return ERR_BUS_NO_ERROR;
	}

	if(pstContext->bAlreadyExistEit == TRUE)
	{
		return ERR_BUS_NO_ERROR;
	}

	pstContext->ulRatingValue = MGR_PG_GetRatingForNoEit(pstContext->svcInfo.eSvcType, FALSE);
	pstContext->bRatingValid = TRUE;
	pstContext->ulEventId = (HUINT32)-1;

	if( (pstContext->ePbState == eXmgrPbState_AV) ||(pstContext->ePbState == eXmgrPbState_WAIT_RATING) ||(pstContext->ePbState == eXmgrPbState_RATING_PIN) )
	{
		/* 이 세 state에서는 rating을 다시 체크해 봐야 하고... */
		xmgr_playback_CheckRating_Base(pstContext);
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtAvVideoUnderrun_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if(pstContext->svcInfo.usVideoPid < PID_NULL)
	{ /* video PID가 valid일 때만 반응  */
		pstContext->bAvUnderrun = TRUE;

		if(pstContext->ePbState == eXmgrPbState_AV)
		{ /* AV state에서는 UI를 그림 */
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_AV_UNDER_RUN);
		}
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtAvVideoStarted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if(pstContext->svcInfo.usVideoPid < PID_NULL)
	{ /* video PID가 valid일 때만 반응  */
		pstContext->bVideoStarted = TRUE;
		pstContext->bAvUnderrun = FALSE;

		xmgr_playback_StartReservedCtrl_Base (pstContext);

  		if( (pstContext->ePbState == eXmgrPbState_AV) && (pstContext->uiState == eMgrPb_AV_UNDER_RUN) )
		{ /* AV state에서 UNDERRUN UI에서만 반응. */
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_NONE);
		}
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtAvAudioUnderrun_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if( (pstContext->svcInfo.usVideoPid >= PID_NULL) && (pstContext->svcInfo.usAudioPid < PID_NULL) )
	{ /* radio PID만 있을 때만 반응 */
		pstContext->bAvUnderrun = TRUE;

		if(pstContext->ePbState == eXmgrPbState_AV)
		{ /* AV state에서는 UI를 그림  */
			if(pstContext->eTrickState == eMgrPb_TickState_PLAY)
			{
				xmgr_playback_SetUiState_Base(pstContext, eMgrPb_AV_UNDER_RUN);
			}
			else
			{
				pstContext->uiState = eMgrPb_AV_UNDER_RUN;
			}
		}
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtAvAudioStarted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if( (pstContext->svcInfo.usVideoPid >= PID_NULL) && (pstContext->svcInfo.usAudioPid < PID_NULL) )
	{ /* radio PID만 있을 때만 반응 */
		pstContext->bAudioStarted = TRUE;
		pstContext->bAvUnderrun = FALSE;

		xmgr_playback_StartReservedCtrl_Base (pstContext);

		if( (pstContext->ePbState == eXmgrPbState_AV) && (pstContext->uiState == eMgrPb_AV_UNDER_RUN) )
		{ /* AV state에서 UNDERRUN UI에서만 반응. */
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_NONE);
		}
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_playback_MsgEvtPbTimecodeChange_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR	 hErr = ERR_FAIL;
	Handle_t hRecAction = HANDLE_NULL;
	SvcRec_RecInfo stRecInfo;

	pstContext->ulPlayTime		= (UNIXTIME)nParam1;
	pstContext->bFirstTimecode	= TRUE;


	/* update recording */
	hErr = SVC_META_GetRecActionByContentId (pstContext->ulContentId, &hRecAction);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hRecAction))
	{
		// normal pb면 여기로 오는게 정상이래요..
		HxLOG_Debug ("SVC_META_GetRecActionByContentId. Error\n");
		return MESSAGE_PASS;
	}

	HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcRec_RecInfo));
	hErr = SVC_REC_GetInfo (hRecAction, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_REC_GetInfo. Error \n");
		return MESSAGE_PASS;
	}

	if (pstContext->ePbType & (eMgrPb_PlayType_Normal | eMgrPb_PlayType_Chase))
	{
		pstContext->ulEndTime = stRecInfo.ulRecTime;
	}
	else if ((pstContext->ePbType & 0x0000000f) == eMgrPb_PlayType_Tsr)
	{

		pstContext->ulStartTime	= stRecInfo.ulStartTime;
		pstContext->ulEndTime	= stRecInfo.ulEndTime;
	}


#if	defined(CONFIG_IGNORE_TRICKPLAY_IN_PMTCHANGED_SERVCE)
	//	Need to control Trick position
	//	언제는 Disable인 경우도 오겠지?
	if (pstContext->bTrickEnable)
	{
		//	PMT is updated!!!
		if (TRUE == nParam3)
		{
			pstContext->bTrickEnable = FALSE;

			if (pstContext->eTrickState != eMgrPb_TickState_PLAY)
			{
				pstContext->ePrevTrickState = pstContext->eTrickState;
				pstContext->ePrevTrickSpeed = pstContext->eTrickSpeed;

				MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 100);
				HxLOG_Debug("Forced Trick Play mode to NORMAL\n");
			}
		}

		HxLOG_Debug("PMT Updated --> flag setting... Forced Trick to Normal Mode!!!\n");
	}
#endif

	NOT_USED_PARAM(nParam2);
	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgEvtPbEOF_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	// 더 이상 뒤로 갈 수 없으니 일단 Pause 상태로 만들고, 이 이후의 판단은 APP에 맡겨야 한다.
	MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 0);

	pstContext->bIsEOF = TRUE;

	if((pstContext->ePbType & 0x0000000f) == eMgrPb_PlayType_Normal)
	{
		HBOOL	bRecord = FALSE;
		MgrRec_RecInfo_t stRecInfo;

		bRecord = MGR_RECORDER_IsScrambleToDescrambleRecord(&stRecInfo);
		if(bRecord == TRUE)
		{
			BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, stRecInfo.hAction, 0, 0, 0);
			SVC_META_SetAsDeleteSkipContent (pstContext->ulContentId, FALSE);
			return MESSAGE_PASS;
		}
		if (MGR_MakeThumbnail_IsAvailable() == TRUE)
		{
			BUS_SendMessage(MGR_MakeThumbnail_GetProc(), eMEVT_PVR_MEDIA_NEXT_FILE, 0, 0, 0, 0);
			return MESSAGE_PASS;
		}
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgEvtPbBOF_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	// 더 이상 앞으로 갈 수 없으니 일단 Pause 상태로 만들고, 이 이후의 판단은 APP에 맡겨야 한다.
	MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 0);

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgEvtSbtlStopped_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	pstContext->bSubTitleOn = FALSE;

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgEvtSbtlStarted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	pstContext->bSubTitleOn = TRUE;

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgEvtCasOk_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	pstContext->bCasOk = TRUE;

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	pstContext->nCasSystemId = nParam1;
	pstContext->pCasInfo = NULL;
	pstContext->nCasEvt = 0;

	xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
	xmgr_playback_SetUiState_Base(pstContext, eMgrPb_NONE);
#endif

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgEvtCasFail_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	pstContext->bCasOk = FALSE;

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	pstContext->nCasSystemId = nParam1;
	pstContext->pCasInfo = (void*)nParam2;
	pstContext->nCasEvt = event;

	xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_CAS_BLOCKED);
#endif

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgEvtCasScInserted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
#if defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
	HERROR		hErr;
#else
	HBOOL		bSCValid = FALSE;
	HBOOL		bPossibleSCPincode = FALSE;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
	if (pstContext != NULL)
	{
		pstContext->nCasErrCode = 0;

		if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
		{
			SVC_AV_SetVideoHide(pstContext->ulActionId, eAvMuteFlag_Cas, FALSE);
			SVC_AV_SetAudioHide(pstContext->ulActionId, eAvMuteFlag_Cas, FALSE);
		}

		if (!pstContext->bPBStart) 	// Play back 들어오기전 부터 s/c가 삽입되지 있지 않을 경우, s/c재삽입시
		{
			pstContext->ePbState = eXmgrPbState_WAIT_AV_STARTED;

			switch (pstContext->ePbType & 0x0000000f)
			{
				case eMgrPb_PlayType_Tsr:
				case eMgrPb_PlayType_Chase:
					xmgr_playback_DisableOutput_Base(pstContext);
					xmgr_playback_GetEitInfo_Base(pstContext);
					break;

				default:
					xmgr_playback_EnableOutput(pstContext);
					break;
			}

			if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
			{
#if !defined(SvcAv_START_SERVICE_NEW)
				hErr = SVC_AV_SetStereoSelect(pstContext->ulActionId, pstContext->svcInfo.eSoundMode);
				if (hErr)
				{
					HxLOG_Error("SVC_AV_SetStereoSelect ret :0x%X\n",hErr);
				}
#endif
				hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, pstContext->svcInfo.eDualMono);
				if (hErr)
				{
					HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
				}

				// 저장된 service 정보의 PMT 로 사용한다.
#if defined(SvcAv_START_SERVICE_NEW)
				hErr = SVC_AV_StartService(pstContext->hAction,
											pstContext->ulVideoDecId,
											pstContext->ulDemuxId,
											pstContext->svcInfo.usAudioPid,
											pstContext->svcInfo.usAudioAuxPid,
											pstContext->svcInfo.usVideoPid,
											pstContext->svcInfo.usPcrPid,
											pstContext->svcInfo.eAudioCodec,
											pstContext->svcInfo.eAudioAuxCodec,
											pstContext->svcInfo.eVideoCodec,
											FALSE,
											FALSE
											);
#else
				hErr = SVC_AV_StartService(pstContext->hAction,
											pstContext->ulVideoDecId,
											pstContext->ulDemuxId,
											pstContext->svcInfo.usAudioPid,
											pstContext->svcInfo.usAudioAuxPid,
											pstContext->svcInfo.usDolbyPid,
											pstContext->svcInfo.usVideoPid,
											pstContext->svcInfo.usPcrPid,
											pstContext->svcInfo.eAudioCodec,
											pstContext->svcInfo.eAudioAuxCodec,
											pstContext->svcInfo.eDolbyCodec,			//2009.03.23 추가.
											pstContext->svcInfo.eVideoCodec,
											pstContext->svcInfo.bDolbyFlag,
											pstContext->svcInfo.eSoundMode
											pstContext->svcInfo.eDualMono,
											FALSE,
											FALSE
											);
#endif
			}
		}
		else					// Play back 된 이후 s/c가 재삽입 된 경우
		{
			xmgr_playback_EnableOutput(pstContext);
			xmgr_playback_CheckAV_Base(pstContext);
		}
	}
#else
	bPossibleSCPincode = xmgr_playback_CheckPossibleSCPincode_Base(pstContext->ulActionId);
	if(bPossibleSCPincode == TRUE)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		bSCValid = SVC_CAS_CheckSmartCardValidity(eDxCAS_GROUPID_NA);
#endif
		if(bSCValid == TRUE)
		{
			HxLOG_Print("Player(%d) : [xmgr_playback_EvtCasScInserted]SC-PIN-Code is now valid ! \n", pstContext->ucPlayIdx);
			pstContext->bSCPinCodeValid = TRUE;
			pstContext->bSCPinPassed = FALSE; /* Pass flag를 reset하고... */

			if((pstContext->ePbState == eXmgrPbState_RATING_PIN) ||
				(pstContext->ePbState == eXmgrPbState_WAIT_PB_LOCK) )
			{ /* 현재 STB-PIN-Code로 돌고 있을 것이므로 다시 SC-PIN-Code로 바꾼다. */
				HxLOG_Print("Change STB-PIN-Code to SC-PIN-Code\n");
#if 0
				// Destroy playback UI
				BUS_SendMessage (NULL, MSG_APPC2U_DESTROY_PLAYBACK_UI, pstContext->hAction, 0, 0, 0);

				// Draw Playback UI
				BUS_SendMessage (NULL, MSG_APPC2U_CREATE_PLAYBACK_UI, pstContext->hAction, pstContext->uiState, 0, 0);
#endif
			}
		}
	}
#endif

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgEvtCasScRemoved_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
#if defined(CONFIG_MW_CAS_NAGRA_OP_UPC)

	if (pstContext != NULL)
	{
		if (xmgr_playback_CheckAvDecUsed_Base (pstContext))
		{
			SVC_AV_SetVideoHide(pstContext->ulActionId, eAvMuteFlag_Cas, TRUE);
			SVC_AV_SetAudioHide(pstContext->ulActionId, eAvMuteFlag_Cas, TRUE);
		}

		pstContext->nCasErrCode = 1;
		if (pstContext->bPBStart)		// Play back 된 이후 s/c가 제거 된 경우
		{
			xmgr_playback_DisableOutput_Base(pstContext);
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_CAS);
		}
	}

#else
	HBOOL bSCValid = TRUE;

	if(pstContext->bSCPinCodeValid == TRUE)
	{ /* SC-PIN-Code valid 였는데... */
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		bSCValid = SVC_CAS_CheckSmartCardValidity(eDxCAS_GROUPID_NA);
#endif
		if(bSCValid != TRUE)
		{ /* SC-PIN-Code 검증 기능을 사용할 수 없게 되었다. */
			HxLOG_Print("SC-PIN-Code is no longer valid !\n");
			pstContext->bSCPinCodeValid = FALSE;
			pstContext->bSCPinPassed = FALSE; /* Pass flag를 reset하고... */

			if((pstContext->ePbState == eXmgrPbState_RATING_PIN) ||
				(pstContext->ePbState == eXmgrPbState_WAIT_PB_LOCK) )
			{ /* 현재 SC-PIN-Code로 돌고 있을 것이므로 다시 STB-PIN-Code로 바꾼다. */
				HxLOG_Print("Change SC-PIN-Code to STB-PIN-Code\n");
			}
		}
	}
#endif

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgEvtAvBlankCtrlByCAS_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	//DxCAS_GroupId_e	eCasGroupId = (DxCAS_GroupId_e)nParam1;
	HBOOL			bBlank = (HBOOL)nParam2;

	HxLOG_Info("[xmgr_playback_EvtAvBlankCtrlByCAS] Blank[%d]\n", bBlank);
	// TRUE 이면, AV를 막고, FALSE 이면 다시 AV를 보여주도록 한다.

	if(bBlank == TRUE)
	{
		xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_CAS_BLOCKED);
	}
	else if(bBlank == FALSE && pstContext->ePbState == eXmgrPbState_CAS_BLOCKED)
	{
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		pstContext->eCiPlusDrmState &= (~eXmgrPbCiPlusDrmState_PC);
		if(pstContext->eCiPlusDrmState)
		{
			// Retention Limit을 통과 하였다 하더라도, 다른 DRM 내용이 걸려 있다면 A/V를 계속해서 막는다.
			HxLOG_Info("Still A/V is blanking... because of CI+ DRM...\n");
		}
		else
		{
			xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_AV);
		}
#endif
	}
	else if(bBlank == FALSE && pstContext->ePbState == eXmgrPbState_AV)
	{
		// 정상적으로 A/V가 잘 나오고 있는 상태에서 A/V blank를 하지 말라고 올라오는 상태... 그냥 아무것도 하지 않고 그대로 AV 보여주면 됨.
		return MESSAGE_BREAK;
	}
	else
	{
		HxLOG_Error("Wrong sequence...");
		xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_CAS_BLOCKED);
	}

	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_playback_MsgEvtAvBlankByDrmInfo_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	DxCopyrightControl_t			*pstDrmInfo;

	pstDrmInfo = (DxCopyrightControl_t *)nParam1;

	HxLOG_Info("[xmgr_playback_EvtAvBlankByDrmInfo] eDrmType=%x\n", pstDrmInfo->stCasCcInfo.eCasCcType);

	xmgr_playback_SetAppState_Base(pstContext, eXmgrPbState_CAS_BLOCKED);

	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	return MESSAGE_BREAK;
}

//eMEVT_BUS_READY_SHUTDOWN
BUS_Result_t xmgr_playback_MsgBusGoStandby(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	PB_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	if(eMgrPb_PlayType_Transcoder == (pstContext->stSetup.ePlayType & eMgrPb_PlayType_Transcoder))
	{ // pvr hls
		HxLOG_Info ("for the live streaming: It shall not be killed by 'GoStandby!'\n");
	}
	else
	{ // normal pb
		BUS_MGR_Destroy (NULL);
	}

	PB_PARAM_WARNING_REMOVE(pstContext, hAction, nParam1, nParam2, nParam3);
	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_MsgEvtPbThumbnailCreate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if(MGR_MakeThumbnail_IsAvailable() == TRUE)
	{
		BUS_SendMessage(MGR_MakeThumbnail_GetProc(), eMEVT_PVR_MEDIA_NEXT_FILE, 0, 0, 0, 0);
	}

	NOT_USED_PARAM(nParam1);
	NOT_USED_PARAM(nParam2);
	NOT_USED_PARAM(nParam3);

	// Message가 Pass 되어야 다른 proc도 제대로 동작한다.
	return MESSAGE_PASS;
}

#define _____PROTO_PROCEDURE_____
STATIC BUS_Result_t xmgr_playback_ProtoProc (xmgrPbContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Warning ("********************************************************\n");
			HxLOG_Warning ("*  XMGR : PLAYBACK : CREATE (%d)\n", ulActionId);
			HxLOG_Warning ("********************************************************\n");

			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eMEVT_BUS_CREATE\n", ulActionId);
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			eBusResult = xmgr_playback_MsgBusCreate_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eMEVT_BUS_DESTROY\n", ulActionId);
			eBusResult = xmgr_playback_MsgBusDestroy_Base(pstContext, hAction, nParam1, nParam2, nParam3);

			HxLOG_Warning ("********************************************************\n");
			HxLOG_Warning ("*  XMGR : PLAYBACK : DESTROY (%d)\n", ulActionId);
			HxLOG_Warning ("********************************************************\n");
			break;

		case eMEVT_BUS_TIMER:
			HxLOG_Info("[MGR:Pb(%d)] MESSAGE : eMEVT_BUS_TIMER\n", ulActionId);
			eBusResult = xmgr_playback_MsgBusTimer_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eMEVT_ACTION_NOTIFY_RELEASED:
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eMEVT_ACTION_NOTIFY_RELEASED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAmActionStopped_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eMEVT_PVR_NOTIFY_RECORD_STOPPED:
		case eMEVT_PVR_NOTIFY_RECORD_SPLIT_FILE:
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eMEVT_PVR_NOTIFY_RECORD_STOPPED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppRecordStop_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eOEVT_PG_CHECK_RATING :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eOEVT_PG_CHECK_RATING\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppCheckRating_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eMEVT_SVCLIST_SVC_PID_CHANGED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eMEVT_SVCLIST_SVC_PID_CHANGED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppDbPidChanged_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eMEVT_PG_PIN_CHECK_OK :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eMEVT_PG_PIN_CHECK_OK\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppPinOK_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eMEVT_PG_PIN_CHECK_DENIED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eMEVT_PG_PIN_CHECK_DENIED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppPinDenied_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;


		case eSEVT_PB_SETUP:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_SETUP\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbSetup_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_AV_SESSION_STARTED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_SESSION_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvDecodingStarted_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_PB_START:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_START\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbStart_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_SI_PMT :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiPmtReceived_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_SI_PAT_TIMEOUT:
		case eSEVT_SI_PMT_TIMEOUT:
		case eSEVT_SI_PMT_PIDREMOVED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_PAT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiPmtTimeout_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_SI_PMT_PIDCHANGED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_PMT_PIDCHANGED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiPmtPidChanged_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_SI_EIT :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_EIT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiEitReceived_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_SI_EIT_TIMEOUT :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_EIT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiEitTimeout_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_AV_VIDEO_UNDERRUN :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_VIDEO_UNDERRUN\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvVideoUnderrun_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_AV_VIDEO_STARTED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_VIDEO_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvVideoStarted_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_AV_AUDIO_UNDERRUN :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_AUDIO_UNDERRUN\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvAudioUnderrun_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_AV_AUDIO_STARTED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_AUDIO_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvAudioStarted_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_PB_TIMECODE_CHANGE :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Info("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_TIMECODE_CHANGE\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbTimecodeChange_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_PB_EOF :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_EOF\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbEOF_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_PB_BOF :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_BOF\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbBOF_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_SBTL_STOPPED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_SBTL_STOPPED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSbtlStopped_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_SBTL_STARTED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_SBTL_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSbtlStarted_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_CAS_OK :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_OK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasOk_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasFail_Base(pstContext, message, nParam1, nParam2, nParam3);
			break;

		case eSEVT_CAS_CHECK :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_CHECK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasFail_Base(pstContext, message, nParam1, nParam2, nParam3);
			break;

		case eMEVT_CAS_SYSINFO_UPDATED:
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eMEVT_CAS_SYSINFO_UPDATED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasScInserted_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_CAS_SC_REMOVED:
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_SC_REMOVED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasScRemoved_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_CAS_DRM_AV_BLANK:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_DRM_AV_BLANK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvBlankCtrlByCAS_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_PB_DRM_AV_BLANK:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_DRM_AV_BLANK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvBlankByDrmInfo_Base(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		/**********************************************************
		  ***	STANDBY
		***********************************************************/
		case eMEVT_BUS_READY_SHUTDOWN:
			HxLOG_Print("[MGR:Pb(%d)] MESSAGE : eMEVT_BUS_READY_SHUTDOWN\n", ulActionId);
			eBusResult = xmgr_playback_MsgBusGoStandby(pstContext, hAction, nParam1, nParam2, nParam3);
			break;

		default :
			break;
	}

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, nParam1, nParam2, nParam3);
}

STATIC BUS_Result_t xmgr_playback_Proc_0 (HINT32 message, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrPbContext_t *pstContext = xmgr_playback_GetContextFromIndex_Base (0);

	return xmgr_playback_ProtoProc (pstContext, message, hAction, nParam1, nParam2, nParam3);
}

STATIC BUS_Result_t xmgr_playback_Proc_1 (HINT32 message, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrPbContext_t *pstContext = xmgr_playback_GetContextFromIndex_Base (1);

	return xmgr_playback_ProtoProc (pstContext, message, hAction, nParam1, nParam2, nParam3);
}

STATIC BUS_Result_t xmgr_playback_Proc_2 (HINT32 message, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrPbContext_t *pstContext = xmgr_playback_GetContextFromIndex_Base (2);

	return xmgr_playback_ProtoProc (pstContext, message, hAction, nParam1, nParam2, nParam3);
}

STATIC BUS_Result_t xmgr_playback_Proc_3 (HINT32 message, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrPbContext_t *pstContext = xmgr_playback_GetContextFromIndex_Base (3);

	return xmgr_playback_ProtoProc (pstContext, message, hAction, nParam1, nParam2, nParam3);
}

STATIC BUS_Result_t xmgr_playback_Proc_4 (HINT32 message, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrPbContext_t *pstContext = xmgr_playback_GetContextFromIndex_Base (4);

	return xmgr_playback_ProtoProc (pstContext, message, hAction, nParam1, nParam2, nParam3);
}

STATIC BUS_Result_t xmgr_playback_Proc_5 (HINT32 message, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrPbContext_t *pstContext = xmgr_playback_GetContextFromIndex_Base (5);

	return xmgr_playback_ProtoProc (pstContext, message, hAction, nParam1, nParam2, nParam3);
}

STATIC BUS_Result_t xmgr_playback_Proc_6 (HINT32 message, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrPbContext_t *pstContext = xmgr_playback_GetContextFromIndex_Base (6);

	return xmgr_playback_ProtoProc (pstContext, message, hAction, nParam1, nParam2, nParam3);
}

STATIC BUS_Result_t xmgr_playback_Proc_7 (HINT32 message, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrPbContext_t *pstContext = xmgr_playback_GetContextFromIndex_Base (7);

	return xmgr_playback_ProtoProc (pstContext, message, hAction, nParam1, nParam2, nParam3);
}


#define _________PLUGIN_FUNC__________
HERROR xmgr_playback_Init_Base (void)
{
	xmgr_playback_InitAllContext_Base();

	return ERR_OK;
}

HERROR xmgr_playback_SetPlaySetup_Base (HUINT32 ulActionId, MgrPb_PlaySetup_t *pstPbSetup)
{
	xmgrPbContext_t *pstContext = NULL;

	pstContext =  xmgr_playback_GetContext_Base(ulActionId);
	if( NULL != pstContext )
	{
		HxSTD_MemSet(pstContext, 0, sizeof(xmgrPbContext_t));
		HxSTD_MemCopy(&pstContext->stSetup, pstPbSetup, sizeof(MgrPb_PlaySetup_t));
	}

	return ERR_OK;
}

BUS_Callback_t	xmgr_playback_GetProcFunc_Base (HUINT32 ulActionId)
{
	HUINT32	ulPbIndex = 0;

	if ((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST))
		ulPbIndex	=  (ulActionId - eActionId_VIEW_FIRST);
	else if ((ulActionId >= eActionId_BG_FIRST) && (ulActionId <= eActionId_BG_LAST))
		ulPbIndex	=  (ulActionId - eActionId_BG_FIRST) + NUM_VIEW_ACTION;

	if (ulPbIndex >= MAX_NUM_OF_PLAY)
	{
		HxLOG_Error ("Wrong Action ID:(%d)\n", ulActionId);
		return NULL;
	}

	HxLOG_Debug("Action ID:(%d), ulPbIndex:(%d)\n", ulActionId, ulPbIndex);

	switch (ulPbIndex)
	{
	case 0:
		return xmgr_playback_Proc_0;

	case 1:
		return xmgr_playback_Proc_1;

	case 2:
		return xmgr_playback_Proc_2;

	case 3:
		return xmgr_playback_Proc_3;

	case 4:
		return xmgr_playback_Proc_4;

	case 5:
		return xmgr_playback_Proc_5;

	case 6:
		return xmgr_playback_Proc_6;

	case 7:
		return xmgr_playback_Proc_7;

	default:
		break;
	}

	return NULL;
}

HUINT32 xmgr_playback_GetPlaybackParam_Base (MgrPb_ParamType_e eType)
{
#if defined(CONFIG_OP_TDC)
	const HINT32	MIN_REQUIRED_PLAYBACK_TIMES			=	3;
#else
	const HINT32	MIN_REQUIRED_PLAYBACK_TIMES			=	10;
#endif

	switch (eType)
	{
	case eMgrPb_MIN_REQUIRED_PLAYBACK_TIME:		return MIN_REQUIRED_PLAYBACK_TIMES;
	default:									break;
	}

	return 0;
}

HERROR	xmgr_playback_SetPlaySpeed_Base (HUINT32 ulActionId, HINT32 nPlaySpeed)
{
	/*
		HERROR	XMGR_PB_SetTrickState(HUINT32 ulActionId, MgrPvr_TrickState_e stState, MgrPb_TrickSpeed_e stSpeed)
		함수를 새 Speed 기능으로 맞추기 위한 잠정적인 기능 함수
		추후 AP Player 자체가 전부 바뀌어야 한다.
	*/
	MgrPvr_TrickState_e 	 eTrickState = eMgrPb_TickState_STOP;
	MgrPb_TrickSpeed_e 	 eTrickSpeed = eMgrPb_Speed_X1;
	xmgrPbContext_t 	*pstContext = NULL;
	HBOOL				bAudioMute = FALSE;
	HERROR				 hErr;

	HxLOG_Debug(HxANSI_COLOR_YELLOW("ulActionId = %d, nPlaySpeed = %d\n"), ulActionId, nPlaySpeed);

	pstContext = xmgr_playback_GetContext_Base(ulActionId);
	if( NULL == pstContext )
		return ERR_FAIL;

	// Pause(0), Play(100) 이외의 Speed 변환은 Radio 채널에서는 해서는 안 된다.
	if ((pstContext->svcInfo.eSvcType == eDxSVC_TYPE_RADIO) &&
		(0 != nPlaySpeed) && (100 != nPlaySpeed))
	{
		HxLOG_Error ("Radio channel cannot change the play the speed (%d)\n", nPlaySpeed);
		return ERR_FAIL;
	}

	if (xmgr_playback_IsTrickableState_Base (pstContext) == FALSE)
	{
		HxLOG_Error ("Do not start playback Action ID(%d)\n", ulActionId);

		pstContext->bSpeedReserved = TRUE;
		pstContext->nReservedSpeed = nPlaySpeed;

		return ERR_FAIL;
	}

	pstContext->bIsEOF = FALSE;

	hErr = xmgr_playback_CvtPlaySpeedToTrickState_Base (nPlaySpeed, &eTrickState, &eTrickSpeed);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error\n");
		return ERR_FAIL;
	}

	/* context update 를 먼저함. */
	{
		pstContext->eTrickState = eTrickState;
		pstContext->eTrickSpeed = eTrickSpeed;
		pstContext->nPlaySpeed	= nPlaySpeed;
	}

	hErr = SVC_PB_SetSpeed (pstContext->hAction, nPlaySpeed);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SvcPb_SetSpeed hErr in ActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}
	HxLOG_Debug(HxANSI_COLOR_YELLOW("SVC_PB_SetSpeed OK\n"));

	if(nPlaySpeed != 100)
	{
		bAudioMute = TRUE;
	}
	else
	{
		bAudioMute = FALSE;
	}

	/*if use smooth winding feature(x1.2 ~ x4), Support to decode and output audio.
	  but, currently require that the audio is not output except for normal speed.*/
	SVC_AV_SetAudioHide(pstContext->ulActionId, eAvMuteFlag_PlaybackSpeed, bAudioMute);

	if (100 == nPlaySpeed)				// Play 상태 이면 TTX Start?
	{
		/* Teletext는 PID가 NULL이 아니면 VBI insert를 위하여 무조건 시작한다. 단, session당 한번만 start되어야 함. */
		if((pstContext->svcInfo.usTtxPid < PID_NULL) && (pstContext->bTtxVbiStarted == FALSE))
		{
#if defined(CONFIG_MW_TELETEXT)
			hErr = SVC_TTX_StartService(pstContext->hAction, pstContext->svcInfo.usTtxPid);
			if (ERR_OK != hErr)
			{
				HxLOG_Critical("Critical Eror!!!\n");
			}

			pstContext->bTtxVbiStarted = TRUE;
#endif
		}

		xmgr_playback_StartSubtitle_Base(pstContext);
	}
	else
	{
		xmgr_playback_StopSubtitle_Base(pstContext);
		xmgr_playback_StopTeletext_Base(pstContext);

#if defined(CONFIG_MW_TELETEXT) && 0
		if(BUS_MGR_Get(PROC_Teletext_PB) != NULL)
		{
			BUS_MGR_Destroy(PROC_Teletext_PB);
		}
#endif
	}

	return ERR_OK;
}


HERROR	xmgr_playback_SetJumpPlay_Base (HUINT32 ulActionId, HUINT32 ulTimeToPlay)
{
	xmgrPbContext_t 	*pstContext = NULL;
	HERROR				 hErr = ERR_FAIL;

	pstContext = xmgr_playback_GetContext_Base(ulActionId);
	if( NULL == pstContext )
		return ERR_FAIL;

	if (xmgr_playback_IsTrickableState_Base (pstContext) == FALSE)
	{
		pstContext->bJumpReserved = TRUE;
		pstContext->nReservedJump = ulTimeToPlay;

		HxLOG_Info("Do not start playback Action ID(%d)\n", ulActionId);
		return hErr;
	}

	pstContext->bIsEOF = FALSE;

#if defined(CONFIG_TSR_IMMEDIATELY_PAUSE)
	// startPaused로 인해 아직 Video frame이 시작되지 않은 경우 Jump시 해당 position의 video frame이 디코딩 되지 않음.
	// 최소한 한번은 Play를 해주도록 함.
	if(pstContext->bVideoStarted == FALSE && pstContext->eTrickState == eMgrPb_TickState_PAUSE)
	{
		SVC_PB_SetSpeed(pstContext->hAction, 100);
		SVC_PB_SetSpeed(pstContext->hAction, 0);
	}
#endif

//	if(pstContext->eTrickState == eMgrPb_TickState_PLAY || pstContext->eTrickState == eMgrPb_TickState_PAUSE)
	if(pstContext->eTrickState != eMgrPb_TickState_STOP)
	{
		xmgr_playback_StopSubtitle_Base(pstContext);
		xmgr_playback_StopTeletext_Base(pstContext);

		HxLOG_Print("call SvcPb_SetPlayTime() ulTimeToPlay=%u\n", ulTimeToPlay);
		hErr = SVC_PB_SetPlayTime(pstContext->hAction, ulTimeToPlay);
	}

	if(hErr == ERR_OK)
	{
		if (pstContext->ulStartTimeByDrm > ulTimeToPlay)
		{
			/*
				현재 시나리오 대로 jump play 하지 않는 것으로 함. jump play가 필요한 상황이라면 아래 주석의 코드를 사용하면 됨
				HxLog_Print("Can't not jump at this play time(%d) due to DRM start time(%d). so change this time to drm start time!!!", ulTimeToPlay, context->ulStartTimeByDrm);
				ulTimeToPlay = context->ulStartTimeByDrm;
			*/
			xmgr_playback_DisableOutput_Base(pstContext);
			xmgr_playback_SetUiState_Base(pstContext, eMgrPb_CAS);
		}

		if(pstContext->eTrickState == eMgrPb_TickState_PLAY)
		{
			/* Teletext는 PID가 NULL이 아니면 VBI insert를 위하여 무조건 시작한다. 단, session당 한번만 start되어야 함. */
#if defined(CONFIG_MW_TELETEXT)
			if( (pstContext->svcInfo.usTtxPid < PID_NULL) && (pstContext->bTtxVbiStarted == FALSE) )
			{
				hErr = SVC_TTX_StartService(pstContext->hAction, pstContext->svcInfo.usTtxPid);
				if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

				pstContext->bTtxVbiStarted = TRUE;
			}
#endif

			xmgr_playback_StartSubtitle_Base(pstContext);
		}
		else
		{
			xmgr_playback_StopSubtitle_Base(pstContext);
			xmgr_playback_StopTeletext_Base(pstContext);
		}
	}

	return hErr;
}


HERROR	xmgr_playback_GetPlaybackInfo_Base (HUINT32 ulActionId, MgrPb_Info_t *pstInfo)
{
	MgrAction_Type_e actType;
	xmgrPbContext_t *pstContext = NULL;
#if	defined(CONFIG_MW_MM_DELAYED_REC)
	HUINT32 		nParentalSetup;
#endif

	pstContext = xmgr_playback_GetContext_Base(ulActionId);
	if( NULL == pstContext )
		return ERR_FAIL;


	actType = MGR_ACTION_GetType(pstContext->ulActionId);
	if( (actType == eViewType_PB) ||
		(actType == eViewType_TSR_PB) ||
		(actType == eViewType_CHASE_PB) )
	{
		if((pstInfo != NULL) && (pstContext->ePbType > 0))
		{
			pstInfo->ulContentId = pstContext->ulContentId; /**< booting 이후의 ts 파일 재생/녹화를 위한 고유의 ID */
			pstInfo->ulStartTime = pstContext->ulStartTime; /**< start time */
			pstInfo->ulPlayTime = pstContext->ulPlayTime;		/**< current play time */
			pstInfo->ulEndTime = pstContext->ulEndTime; 	/**< end time */
			pstInfo->hAction = pstContext->hAction; 		/**< session version이 포함된 action handle */
			pstInfo->nPlaySpeed = pstContext->nPlaySpeed;
			pstInfo->eTrickState = pstContext->eTrickState;
			pstInfo->eTrickSpeed = pstContext->eTrickSpeed;
			pstInfo->hSvc = pstContext->hSvc;
			pstInfo->hMasterSvc = pstContext->hMasterSvc;
			pstInfo->eSvcListGroup = pstContext->eSvcListGroup;
#if	defined(CONFIG_MW_MM_DELAYED_REC)
			nParentalSetup = MGR_PG_GetRecordRatingFromSetup();
			pstInfo->bPinLock = MGR_RECORDER_IsTsrLockForDelayedRecord(pstInfo->ulContentId, pstContext->ulEventId, nParentalSetup);
#endif
			pstInfo->bPbStart = pstContext->bPBStart;
			pstInfo->nPbState = pstContext->ePbState;

			pstInfo->bDrmValid = pstContext->bDrmValid;
			HxSTD_MemCopy(&pstInfo->stDrmInfo, &pstContext->stDrmInfo, sizeof(DxCopyrightControl_t));
		}

		return ERR_OK;
	}

	return ERR_FAIL;
}


HERROR	xmgr_playback_GetPlaybackTimeInfo_Base (HUINT32 ulActionId, MgrPb_Info_t *pstInfo)
{
	HINT32 idx;
	MgrAction_Type_e actType;
	xmgrPbContext_t	*pstContext = NULL;

	for(idx = 0; idx < MAX_NUM_OF_PLAY; idx++)
	{
		pstContext = &s_astMgrPbContext[idx];
		actType = MGR_ACTION_GetType(pstContext->ulActionId);
		if(	(actType == eViewType_PB) ||
			(actType == eViewType_MOVIE_PB) ||
			(actType == eViewType_TSR_PB) ||
			(actType == eViewType_CHASE_PB) )
		{
			if((pstInfo != NULL) && (pstContext->ePbType > 0))
			{
				if((pstContext->bPBStart == FALSE) ||(pstContext->bFirstPmt == FALSE)||(pstContext->bFirstTimecode == FALSE))
				{
					return ERR_FAIL;
				}
				pstInfo->ulContentId = pstContext->ulContentId;	/**< booting 이후의 ts 파일 재생/녹화를 위한 고유의 ID */
				pstInfo->ulStartTime = pstContext->ulStartTime;	/**< start time */
				pstInfo->ulPlayTime = pstContext->ulPlayTime;		/**< current play time */
				pstInfo->ulEndTime = pstContext->ulEndTime;		/**< end time */
				pstInfo->hAction = pstContext->hAction;			/**< session version이 포함된 action handle */
				pstInfo->nPlaySpeed = pstContext->nPlaySpeed;
				pstInfo->eTrickState = pstContext->eTrickState;
				pstInfo->eTrickSpeed = pstContext->eTrickSpeed;
				pstInfo->hSvc = pstContext->hSvc;
				pstInfo->hMasterSvc = pstContext->hMasterSvc;
				pstInfo->eSvcListGroup = pstContext->eSvcListGroup;

				pstInfo->bPbStart = pstContext->bPBStart;
			}

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR	xmgr_playback_StopPlayback_Base (HUINT32 ulActionId)
{
	HERROR hErr = ERR_FAIL;
	xmgrPbContext_t *pstContext = NULL;
	Handle_t		hRecAction	= HANDLE_NULL;

	pstContext = xmgr_playback_GetContext_Base (ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("not found Context\n");
		return ERR_FAIL;
	}

	hErr = SVC_META_GetRecActionByContentId (pstContext->ulContentId, &hRecAction);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hRecAction))
	{
		HxLOG_Error ("SVC_META_GetRecActionByContentId. Error\n");
		return ERR_FAIL;
	}

	// 재생 종료이므로 더이상 ContentList 에 유지할 필요가 없으므로 Skip Flag 를 FALSE 로 한다.
	// VideoList 를 Show 시 기존 ContentList 를 SVC_PVR_UnloadContentsInfo 로 해제하는데, Skip Flag 를 TRUE 로 하면 해재되지 않고 유지된다. FALSE 이면 해제
	if ((ERR_OK != hErr) || (HANDLE_NULL == hRecAction))
	{
		hErr = SVC_META_SetAsDeleteSkipContent (pstContext->ulContentId, FALSE);
		{
			HxLOG_Error ("SVC_META_SetAsDeleteSkipContent. Error\n");
			return ERR_FAIL;
		}
	}

#if 0

		bRecord = MGR_RECORDER_IsScrambleToDescrambleRecord(&stRecInfo);
		if(bRecord == TRUE)
		{
			BUS_SendMessage (NULL, MSG_APPC2U_INFORM_SCR2DESC_PLAYBACK_STOPPED, HANDLE_NULL, 0, 0, 0);
			return ERR_OK;
		}
#endif

	BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);
	return ERR_OK;
}

HERROR	xmgr_playback_SetAudioInfo_Base (HUINT32 ulActionId, HUINT32 ulIndex, DxStereoSelect_e	eSoundMode)
{
	xmgrPbContext_t 	*pstContext = xmgr_playback_GetContext_Base(ulActionId);

	HxLOG_Info("set subtitle index as (%d)\n", ulIndex);
	return xmgr_playback_ChangeAudioWithAudInfo_Base(pstContext, ulIndex, eSoundMode);
}

HERROR	xmgr_playback_SetSubTitleDisplay_Base (HUINT32 ulActionId, HBOOL	bDisplay)
{
	xmgrPbContext_t 	*pstContext = xmgr_playback_GetContext_Base(ulActionId);

	if (bDisplay)
	{
		xmgr_playback_StartSubtitle_Base(pstContext);
	}
	else
	{
		xmgr_playback_StopSubtitle_Base(pstContext);
	}

	return ERR_OK;
}

HERROR	xmgr_playback_SetSubtitleLanguage_Base (HUINT32 ulActionId, HUINT32 ulIndex)
{
	xmgrPbContext_t 	*pstContext = xmgr_playback_GetContext_Base(ulActionId);

	return xmgr_playback_ChangeSubtitleWithSbtlIndex_Base(pstContext, ulIndex);
}

HERROR	xmgr_playback_GetSubtitleDisplay_Base (HUINT32 ulActionId, HBOOL	*pbDisplay)
{
	xmgrPbContext_t 	*pstContext = xmgr_playback_GetContext_Base(ulActionId);
	if (xmgr_playback_IsSubtitlePermitted_Base(pstContext))
	{
		*pbDisplay = pstContext->bSubTitleOn;
	}

	return ERR_OK;
}

HERROR	xmgr_playback_GetServiceInfo_Base (HUINT32 ulActionId, SvcMeta_Service_t	*pstService)
{
	xmgrPbContext_t 	*pstContext = xmgr_playback_GetContext_Base(ulActionId);

	if (pstContext)
	{
		HxSTD_MemCopy( pstService, &pstContext->svcInfo, sizeof(SvcMeta_Service_t) );
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR xmgr_playback_SetServiceInfo_Base (HUINT32 ulActionId, SvcMeta_Service_t	*pstService, HBOOL bUpdate)
{
	xmgrPbContext_t 	*pstContext = xmgr_playback_GetContext_Base(ulActionId);

	if (pstContext)
	{
		// audio, subtitle 변경시 기존의 svcInfo 와 변경된 것을 비교해야 하므로 변경된 svcInfo 로 update 하지 않는다.
		if(bUpdate == TRUE)
		{
			HxSTD_MemCopy(&pstContext->svcInfo, pstService, sizeof(SvcMeta_Service_t) );
		}
		return SVC_META_SetServiceInfo(pstContext->ulContentId, pstService, TRUE);
	}

	return ERR_FAIL;
}

HBOOL xmgr_playback_IsPlaybackId_Base (HUINT32 ulContentId, MgrPb_Info_t *pstInfo)
{
	HINT32 idx;
	MgrAction_Type_e actType;
	xmgrPbContext_t	*pstContext = NULL;


	for(idx = 0; idx < MAX_NUM_OF_PLAY; idx++)
	{
		pstContext = &s_astMgrPbContext[idx];

		if (pstContext->ePbState == eXmgrPbState_IDLE)
		{
			continue;
		}

		actType = MGR_ACTION_GetType(pstContext->ulActionId);

		if(((actType == eViewType_PB) || (actType == eViewType_CHASE_PB))&& (ulContentId == pstContext->ulContentId))
		{
			if((pstInfo != NULL) && (pstContext->ePbType > 0))
			{
				pstInfo->ulContentId = pstContext->ulContentId;	/**< booting 이후의 ts 파일 재생/녹화를 위한 고유의 ID */
				pstInfo->ulStartTime = pstContext->ulStartTime;	/**< start time */
				pstInfo->ulPlayTime = pstContext->ulPlayTime;		/**< current play time */
				pstInfo->ulEndTime = pstContext->ulEndTime;		/**< end time */
				pstInfo->hAction = pstContext->hAction;			/**< session version이 포함된 action handle */
				pstInfo->nPlaySpeed = pstContext->nPlaySpeed;
				pstInfo->eTrickState = pstContext->eTrickState;
				pstInfo->eTrickSpeed = pstContext->eTrickSpeed;
				pstInfo->hSvc = pstContext->hSvc;
				pstInfo->hMasterSvc = pstContext->hMasterSvc;
				pstInfo->eSvcListGroup = pstContext->eSvcListGroup;

				pstInfo->bPbStart = pstContext->bPBStart;
			}

			return TRUE;
		}
	}

	return FALSE;
}

HERROR xmgr_playback_GetMenuSubtitleLangIndex(DbParam_Item_e eSubtitleItem, SvcSi_SubtitleList_t *pstSutitleList, HINT32 *pnIndex)
{
	Handle_t	hErr = ERR_FAIL;
	HCHAR 		*pSubtitleLang = NULL;
	HxLANG_Id_e eSubtitleLangId = eLangID_NONE;
	HINT32		i = 0;
	HINT32		nSelectedIdx = -1;
	HINT32		nHighScore = 0;
	HINT32		nMyScore = 0;
	HBOOL		bFound = FALSE;
	HUINT8		aucLangCode[4] = {0, };
	HBOOL 		bHohEnabled = FALSE;

	if(NULL == pstSutitleList || NULL == pnIndex)
		goto END_FUNC;
	if( eSubtitleItem < eDB_PARAM_ITEM_LANG_SUBTITLE0 || eSubtitleItem > eDB_PARAM_ITEM_LANG_SUBTITLE7 )
		goto END_FUNC;

	// get subtitle language
	hErr = DB_PARAM_GetItem(eSubtitleItem, (HUINT32 *)&eSubtitleLangId, 0);
	if( hErr != ERR_OK )
		goto END_FUNC;

	// convert subtitleid to 639
	pSubtitleLang = HLIB_LANG_IdTo639(eSubtitleLangId);
	if( pSubtitleLang == NULL )
		goto END_FUNC;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HARDOFHEARING, (HUINT32 *)&bHohEnabled, 0);
	if(hErr != ERR_OK)
	{
		HxLOG_Warning("Fail to GetItem eDB_PARAM_ITEM_HARDOFHEARING\n");
	}
	HxLOG_Info("Hard of hearing?( %d ) nSubtitleNum( %d )\n", bHohEnabled, pstSutitleList->ulSubtitleNum);

	// find
	for(i = 0 ; i < pstSutitleList->ulSubtitleNum ; i++)
	{
		HxSTD_MemSet(aucLangCode, 0x00, sizeof(aucLangCode));
		if(pstSutitleList->pstSubtitleInfo[i].eType == eSI_SUBTITLE_DVB)
		{
			HxSTD_MemCopy(aucLangCode, pstSutitleList->pstSubtitleInfo[i].utArg.stDvbInfo.aucLangCode, sizeof(aucLangCode));
		}
		else if(pstSutitleList->pstSubtitleInfo[i].eType == eSI_SUBTITLE_EBU)
		{
			HxSTD_MemCopy(aucLangCode, pstSutitleList->pstSubtitleInfo[i].utArg.stEbuInfo.aucLangCode, sizeof(aucLangCode));
		}
		else if(pstSutitleList->pstSubtitleInfo[i].eType == eSI_SUBTITLE_SUPERCC)
		{
			HxLOG_Info("SUPERCC doesn't have langcode.");
			continue;
		}
		else
		{
			HxLOG_Warning("Unknown subtitle type\n");
			continue;
		}

		HxLOG_Info("Subtitle Idx( %d ) (%s : %s) HOH( %d )\n", i, pSubtitleLang, aucLangCode, SvcSi_IsHOHSubtitle(pstSutitleList->pstSubtitleInfo[i].utArg.stDvbInfo.ucType));
		if(HLIB_STD_StrCaseCmp(pSubtitleLang, aucLangCode) == 0)
		{
			if(pstSutitleList->pstSubtitleInfo[i].eType == eSI_SUBTITLE_DVB)
			{
				if(SvcSi_IsHOHSubtitle(pstSutitleList->pstSubtitleInfo[i].utArg.stDvbInfo.ucType))
				{
					if(bHohEnabled)
					{ // priority1 - DVB HOH( if hoh enabled )
						bFound = TRUE;
						nHighScore = nMyScore = 150;
						nSelectedIdx = i;
						break;
					}
					else
					{ // if HOH menu was disabled, Do not select HOH subtitle
						continue;
					}
				}

				// priority2 - DVB normal
				nMyScore = 100;
				bFound = TRUE;
			}
			else if(pstSutitleList->pstSubtitleInfo[i].eType == eSI_SUBTITLE_EBU)
			{ // priority3 - EBU
				nMyScore = 50;
				bFound = TRUE;
			}
			else
			{ // never come here...
				nMyScore = 0;
			}

			if(nHighScore < nMyScore)
			{
				nHighScore = nMyScore;
				nSelectedIdx = i;
			}

		}

	}

END_FUNC:

	if(bFound == TRUE)
	{
		HxLOG_Info("Find subtitle language( %s )\n", pSubtitleLang);
		HxLOG_Info("Score( %d ) : HOH - 150, DBV - 100, EBU - 50\n", nHighScore);
		*pnIndex = nSelectedIdx;
		hErr = ERR_OK;
	}
	else
	{
		HxLOG_Warning("Can not found same language.\n");
		hErr = ERR_FAIL;
	}

	return hErr;
}
