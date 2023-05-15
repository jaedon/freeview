/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <isosvc.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_fs.h>
#include <svc_pb.h>
#include <svc_cas.h>
#include <svc_av.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_sys.h>
#include <get_enum_str.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_output.h>
#include <mgr_pg.h>
#include <svc_resources.h>
#include <mgr_rscmap.h>
#include <mgr_action.h>
#include <mgr_media.h>
#include <mgr_svclist.h>
#include <mgr_live.h>
#include <mgr_playback.h>
#include <_xmgr_playback.h>
#include "../local_include/_xmgr_playback_base.h"

#include <mgr_cas.h>
#include <xmgr_cas.h>
#include <svc_output.h>

#define _____PROTO_PROCEDURE_____

STATIC HERROR xmgr_playback_NagraGetDrmInfoByTime_Base (SvcMeta_TimestampList_t *pstTimeStampList, HUINT32 ulSourceTime, DxCopyrightControl_t *pstInfo)
{
	SvcMeta_Timestamp_t *pstTimeStamp = NULL;
	HUINT32 ulTimeStampNum = 0;
	HUINT32 i, ulMaxIndex=0, ulMaxTime = 0, ulCount=0;

	if (NULL == pstInfo)
	{
		HxLOG_Error("pstInfo is NULL\n");
		return ERR_FAIL;
	}

	ulTimeStampNum = pstTimeStampList->ulNumber;

	for (i=0; i<ulTimeStampNum; i++)
	{
		pstTimeStamp = &pstTimeStampList->pstData[i].stTimeStamp;

		if (eSvcMeta_Timestamp_DRM != pstTimeStamp->eType)
			continue;

		if( pstTimeStamp->ulTime > ulSourceTime )
			continue;

		ulCount++;

		if( ulMaxTime <= pstTimeStamp->ulTime )
		{
			ulMaxIndex = i;
			ulMaxTime =  pstTimeStamp->ulTime;
		}
	}

	if (0 == ulCount)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy (pstInfo, (DxCopyrightControl_t *)pstTimeStampList->pstData[ulMaxIndex].pData, sizeof(DxCopyrightControl_t));

	return ERR_OK;
}

STATIC HERROR xmgr_playback_NagraIsValidCasDrmInfo_Base(DxCopyrightControl_t *pstDrmInfo)
{
	if(pstDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo is NULL\n");
		return ERR_FAIL;
	}

	if(pstDrmInfo->bCasCc == FALSE)
	{
		HxLOG_Error("bCasCc is error\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if(pstDrmInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_NagraSes)
	{
		HxLOG_Error("bCasCc : eDrmType is not eDxCopyrightControl_CasCcType_NagraSes\n");
		return ERR_FAIL;
	}
#else
	if(pstDrmInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_Nagra)
	{
		HxLOG_Error("bCasCc : eDrmType is not eDxCopyrightControl_CasCcType_NagraSes\n");
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

void xmgr_playback_NagraSendDrmInfo_Base(xmgrPbContext_t *pstContext)
{
	Handle_t		hAction;
	SvcMeta_Record_t 	stRecordInfo;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	DxCopyrightControl_CasCcType_e	 		eCasCcType = eDxCopyrightControl_CasCcType_None;
	DxCopyrightControl_SiCcType_e			eSiCcType = eDxCopyrightControl_SiCcType_None;
#endif

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return;
	}

	hAction = SVC_PIPE_GetActionId (pstContext->ulActionId);

	HxSTD_MemSet(&stRecordInfo, 0, sizeof(SvcMeta_Record_t));

	if(SVC_META_GetRecordInfo (pstContext->stSetup.ulContentId, &stRecordInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo is error\n");
	}

	HxLOG_Debug("DRM CAS(%d), SI(%d)\n",stRecordInfo.stDrmInfo.bCasCc, stRecordInfo.stDrmInfo.bSiCc);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if(stRecordInfo.stDrmInfo.bCasCc == TRUE)
	{
		eCasCcType = xmgr_playback_NagraGetCasDrmType_Ses(&stRecordInfo.stDrmInfo);

		if(eCasCcType == eDxCopyrightControl_CasCcType_NagraSes)
		{
			HxLOG_Debug("DRM CAS(%d), SI(%d) --> eSEVT_CAS_DRM_SIGNALLED\n");
			BUS_SendMessage(NULL, eSEVT_CAS_DRM_SIGNALLED, hAction, 0, 0, (HINT32)&stRecordInfo.stDrmInfo);
		}
		else if(eCasCcType == eDxCopyrightControl_CasCcType_Nagra)
		{
//			BUS_SendMessage(NULL, eSEVT_CAS_DRM_SIGNALLED, hAction, 0, 0, (void *)&stRecordInfo.stDrmInfo);
		}
	}
	else
	{
		if(stRecordInfo.stDrmInfo.bSiCc == TRUE)
		{
			eSiCcType = xmgr_playback_NagraGetSiDrmType_Ses(&stRecordInfo.stDrmInfo);

			if(eSiCcType == eDxCopyrightControl_SiCcType_Ses)
			{
				HxLOG_Debug("DRM CAS(%d), SI(%d) --> eSEVT_CAS_DRM_SIGNALLED\n");
				BUS_SendMessage(NULL, eSEVT_SI_DRM_SIGNALLED, hAction, 0, 0, (HINT32)&stRecordInfo.stDrmInfo);
			}
		}
	}
#endif
}

BUS_Result_t xmgr_playback_NagraMsgBusCreate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT8					ucPlayIdx = xmgr_playback_GetContextIndexFromActionId (ulActionId);
	MgrAction_Type_e			eActionType;
	SvcMeta_Record_t			stRecordInfo;
	MgrRec_RecInfo_t	stInfo;
#if defined(CONFIG_APCORE_MOVING_THUMBNAIL_CACHE)
	HBOOL					bRecord = FALSE;
#endif
	HERROR					hErr;
	HUINT32					ulContentID			=	pstContext->stSetup.ulContentId;
	SvcMeta_TimestampList_t *pstTimeStampList	=	&pstContext->stTimeStampList; /* owner playback of mgr-context*/

	HxLOG_Debug("\n");

	if(pstContext->ePbState != eXmgrPbState_IDLE)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}
	HxLOG_Debug("Player(%d) : eMEVT_BUS_CREATE ulContentId=%d ulStartTime=%d\n", ucPlayIdx, ulContentID, pstContext->stSetup.ulStartTime);


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

	HxLOG_Info("Player(%d) : eActionType=%d ePlayType=%d\n", ucPlayIdx, eActionType, pstContext->stSetup.ePlayType);
	// Resouce 는 모두 Ready된 상태임.
	hErr = MGR_ACTION_Take(ulActionId, eActionType, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		HxLOG_Debug("Player(%d) : can't take MGR_ACTION_Take eActionType=%x Handle_t=%x\n", ucPlayIdx, eActionType, pstContext->stSetup.hCurrSvc);
	//	goto FINISH;
	}

	/* init recorder app pstContext */
	pstContext->ulActionId = ulActionId;
	pstContext->hAction = hAction;

	hErr = MGR_RSC_GetResourceId(ulActionId, eRxRSCTYPE_AV_DEC, &pstContext->ulVideoDecId);
	if(ERR_OK!=hErr)		HxLOG_Critical("Critical Eror!!!\n");
	hErr = MGR_RSC_GetResourceId(ulActionId, eRxRSCTYPE_PVR_PLAYER, &pstContext->ulPlayerId);
	if(ERR_OK!=hErr)		HxLOG_Critical("Critical Eror!!!\n");
	hErr = MGR_RSC_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &pstContext->ulDemuxId);
	if(ERR_OK!=hErr)		HxLOG_Critical("Critical Eror!!!\n");

	pstContext->ucPlayIdx		= ucPlayIdx;
	pstContext->hSvc			= pstContext->stSetup.hCurrSvc;
	pstContext->hMasterSvc		= pstContext->stSetup.hMasterSvc;
	pstContext->eSvcListGroup	= pstContext->stSetup.eSvcListGroup;
	pstContext->ePbType 		= pstContext->stSetup.ePlayType;
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
	pstContext->eTrickSpeed 	= eMgrPb_Speed_X1;
	pstContext->bPmtValid		= FALSE;
	pstContext->nCasErrCode 	= 0;
	pstContext->bCasOk			= FALSE;
	pstContext->ulContentId 	= pstContext->stSetup.ulContentId;
	pstContext->uiState 		= eMgrPb_NONE;
	pstContext->bRadioBackground = FALSE;
	pstContext->bRestartAV		= FALSE;
	pstContext->ulEventId		= (HUINT32)-1;
	pstContext->bAVHide 		= FALSE;
	pstContext->bIsEOF			= FALSE;
	pstContext->bRatingPinPassed = FALSE;
	pstContext->bSCPinPassed	= FALSE;
	pstContext->bRatingPassByLockPass = FALSE;
	pstContext->bSCPinCodeValid = FALSE;

	HxSTD_MemSet(&pstContext->svcInfo, 0, sizeof(SvcMeta_Service_t));
	HxSTD_MemSet(&stRecordInfo, 0, sizeof(SvcMeta_Record_t));
	HxSTD_MemSet(&stInfo, 0, sizeof(MgrRec_RecInfo_t));

	(void)xmgr_playback_RefreshSvcMeta_Base(pstContext, pstContext->stSetup.ulStartTime);

	hErr = SVC_META_GetRecordInfo (ulContentID, &stRecordInfo);
	hErr = SVC_META_GetServiceInfo(ulContentID, &pstContext->svcInfo);

	HxLOG_Debug("hErr(%d), ulContentID(%d)\n", hErr, ulContentID);

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


	/* Session version 증가 */
	pstContext->hAction = SVC_PIPE_IncreaseActionVersion(ulActionId);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if(stRecordInfo.stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_NagraSes)
	{
		HxLOG_Debug(": Copy DrmInfo\n");
 		HxSTD_MemCopy(&pstContext->stDrmInfo, &stRecordInfo.stDrmInfo, sizeof(DxCopyrightControl_t));
	}
#else
	if(stRecordInfo.stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_Nagra)
	{
		HxLOG_Debug(": Copy DrmInfo\n");
 		HxSTD_MemCopy(&pstContext->stDrmInfo, &stRecordInfo.stDrmInfo, sizeof(DxCopyrightControl_t));
	}
#endif

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

		stSetup.ulStartTime 	=	pstContext->stSetup.ulStartTime;

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


		if(stRecordInfo.u1cScrambled == 1)
		{
			stSetup.bDescramblePlay = TRUE;
		}
		else
		{
			stSetup.bDescramblePlay =FALSE;
		}


		stSetup.eSvcType		=	pstContext->svcInfo.eSvcType;
		stSetup.bEncryption 	=	(1 == stRecordInfo.u1cEncryption)?TRUE:FALSE;

		HxLOG_Debug("bEncryption(%d), u1cScrambled(%d)\n", stSetup.bEncryption, stRecordInfo.u1cScrambled);

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
		stSetup.bStartPaused = pstContext->stSetup.bStartPaused;

		pstContext->ePbState = eXmgrPbState_WAIT_PB_SETUP;

		hErr = SVC_PB_Setup(pstContext->hAction, &stSetup, TRUE);

		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_PB_Setup Error. (%x)\n", pstContext->hAction);
		}

		hErr = xmgr_playback_NagraGetDrmInfoByTime_Base (pstTimeStampList, stRecordInfo.ulStartTime, &pstContext->stDrmInfo );

		if (ERR_OK == hErr)
		{
			SVC_META_SetDRMInfo (ulContentID, &pstContext->stDrmInfo, FALSE);
		}

	}

#if defined(CONFIG_MW_CAS_NAGRA) // TODO 기능 확인 해야함.
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

	SVC_META_RegisterPvrAction(pstContext->hAction, ulContentID, eSvcMeta_UsingActType_Playback);

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_playback_NagraGetProperTimeStamp_Base (SvcMeta_TimestampList_t *pstTimeStampList, HUINT32 ulSourceTime, HUINT32 ulStartIdx, HUINT32 *pulFoundIdx)
{
	HINT32	nIndex, nNumber;
	SvcMeta_TimestampList_t	*pstList = NULL;

	if (NULL == pstTimeStampList)
	{
		return ERR_OK;
	}

	pstList = pstTimeStampList;
	nNumber = (HINT32)pstList->ulNumber;

	if (nNumber >= 2)
	{
#if 1 // defined(CONFIG_DRM_TEST)
		for (nIndex = (HINT32)ulStartIdx; nIndex < nNumber; nIndex++)
		{
			/* 처음 playback을 시작하는경우 무조건 drm 정보를 올려주기 위해 사용됨 */
			if(nIndex ==0 )
			{
				if(pstList->ulDrmRefTime <= 0)
				{
					if (pstList->pstData[nIndex].stTimeStamp.ulTime <= ulSourceTime)
					{
						*pulFoundIdx = (HUINT32)nIndex;
						return ERR_OK;
					}
				}
			}

			/* playback에해 drm이 한번이라도 update된경우에 back Jump or Rew에의해 drm 정보를 올려주어야 하는 경우가 생긴경우 사용됨  */
			if(ulSourceTime > 0 && pstList->ulDrmRefTime > 0)
			{
				if(ulSourceTime < pstList->ulDrmRefTime)
				{
					*pulFoundIdx = (HUINT32)nIndex;
					return ERR_OK;
				}
			}

			/* timestamp 시간에는 무조건 drm 정보를 올려주시위해 사용됨 */
			if (pstList->pstData[nIndex].stTimeStamp.ulTime == ulSourceTime)
			{
				*pulFoundIdx = (HUINT32)nIndex;
				return ERR_OK;
			}
		}
#else
		for (nIndex = (HINT32)ulStartIdx; nIndex <= nNumber - 2; nIndex++)
		{
			if ((pstList->pstData[nIndex].stTimeStamp.ulTime <= ulSourceTime) &&
				(pstList->pstData[nIndex + 1].stTimeStamp.ulTime > ulSourceTime))
			{
				*pulFoundIdx = (HUINT32)nIndex;
				return ERR_OK;
			}
		}
#endif
	}
	else if ((nNumber == 1) && (0 == ulStartIdx))
	{
		if (pstList->pstData[0].stTimeStamp.ulTime <= ulSourceTime)
		{
			*pulFoundIdx = 0;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR xmgr_playback_NagraCheckDrmTimeStamp_Base (xmgrPbContext_t *pstContext, HUINT32 ulSourceTime)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulIndex, ulFoundIdx;
	SvcMeta_TimestampData_t *pstTimeStamp = NULL;
	SvcMeta_TimestampList_t *pstList = NULL;

	if (NULL == pstContext)
	{
		return ERR_FAIL;
	}

	pstList 	= &(pstContext->stTimeStampList);
	ulIndex 	= 0;

	while (ulIndex < pstList->ulNumber)
	{
		ulFoundIdx = pstList->ulNumber;

		hErr = xmgr_playback_NagraGetProperTimeStamp_Base (pstList, ulSourceTime, ulIndex, &ulFoundIdx);
		if ((ERR_OK != hErr) || (ulFoundIdx >= pstList->ulNumber))
		{
			break;
		}

		ulIndex = ulFoundIdx + 1;
		pstTimeStamp = &(pstList->pstData[ulFoundIdx]);

		switch (pstTimeStamp->stTimeStamp.eType)
		{
			case eSvcMeta_Timestamp_DRM:
			{
				DxCopyrightControl_t	*pstMetaDrm	=	NULL;
				pstMetaDrm = (DxCopyrightControl_t *)pstTimeStamp->pData;
				if ((NULL != pstMetaDrm) && (pstList->ulDrmRefTime != pstTimeStamp->stTimeStamp.ulTime))
				{
 					HxLOG_Debug("################## [%s:%d] <%ld : %ld> : bCasCc(%d), bSiCc(%d), eCasCcType(%d), ByteA(0x%x), ByteB(0x%x)\n",
							__FUNCTION__, __LINE__, pstList->ulDrmRefTime, pstTimeStamp->stTimeStamp.ulTime, pstMetaDrm->bCasCc, pstMetaDrm->bSiCc, pstMetaDrm->stCasCcInfo.eCasCcType, pstMetaDrm->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA, pstMetaDrm->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB);

					if(pstMetaDrm->bCasCc == TRUE)
					{
						BUS_SendMessage(NULL, eSEVT_CAS_DRM_SIGNALLED, pstContext->hAction, 0, 0, (HINT32)pstMetaDrm);
					}
					else if(pstMetaDrm->bSiCc == TRUE)
					{
						BUS_SendMessage(NULL, eSEVT_SI_DRM_SIGNALLED, pstContext->hAction, 0, 0, (HINT32)pstMetaDrm);
					}

					pstList->ulDrmRefTime = pstTimeStamp->stTimeStamp.ulTime;
				}


				HxLOG_Error("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXxx\n");
				break;
			}

		default:
			break;
		}
	}

	return ERR_OK;
}

STATIC HERROR xmgr_playback_NagraCheckTimeStamp_Base (xmgrPbContext_t *pstContext, HUINT32 ulSourceTime)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulIndex, ulFoundIdx;
	SvcMeta_TimestampData_t *pstTimeStamp	= NULL;
	SvcMeta_TimestampList_t *pstList		= NULL;

	if(pstContext == NULL)
	{
		return ERR_FAIL;
	}

	pstList 	=	&(pstContext->stTimeStampList);
	ulIndex 	=	0;
	ulFoundIdx	=	0;

	while (ulIndex < pstList->ulNumber)
	{
		ulFoundIdx = pstList->ulNumber;

		hErr = xmgr_playback_NagraGetProperTimeStamp_Base (pstList, ulSourceTime, ulIndex, &ulFoundIdx);
		if ((ERR_OK != hErr) || (ulFoundIdx >= pstList->ulNumber))
				break;

		ulIndex = ulFoundIdx + 1;
		pstTimeStamp = &(pstList->pstData[ulFoundIdx]);

		switch (pstTimeStamp->stTimeStamp.eType)
		{
			case eSvcMeta_Timestamp_PMT:
			{
				SvcMeta_Service_t*pstMetaSvc = NULL;
				pstMetaSvc = (SvcMeta_Service_t *)pstTimeStamp->pData;
				if ((NULL != pstMetaSvc) && (pstList->ulPmtRefTime != pstTimeStamp->stTimeStamp.ulTime))
				{
					hErr = SVC_META_SetServiceInfo (pstContext->ulContentId, pstMetaSvc, FALSE);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("SVC_META_SetServiceInfo err:\n");
						break;
					}

					/* emit EVT_PVR_PMT_CHANGED*/
					//


					pstList->ulPmtRefTime = pstTimeStamp->stTimeStamp.ulTime;
				}

				break;
			}
			case eSvcMeta_Timestamp_EIT:
			{
				SvcMeta_Event_t *pstMetaEvent	=	NULL;
				pstMetaEvent = (SvcMeta_Event_t *)pstTimeStamp->pData;
				if ((NULL != pstMetaEvent) && (pstList->ulEitRefTime != pstTimeStamp->stTimeStamp.ulTime))
				{
					hErr = SVC_META_SetEventInfo (pstContext->ulContentId, pstMetaEvent, FALSE);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("SVC_META_SetEventInfo err:\n");
					}

					/* emit EVT_PVR_EIT_CHANGED */
					//

					pstList->ulEitRefTime = pstTimeStamp->stTimeStamp.ulTime;
				}

				break;
			}

			case eSvcMeta_Timestamp_DRM:
			{
				DxCopyrightControl_t	*pstMetaDrm =	NULL;
				pstMetaDrm = (DxCopyrightControl_t *)pstTimeStamp->pData;
				if ((NULL != pstMetaDrm) && (pstList->ulDrmRefTime != pstTimeStamp->stTimeStamp.ulTime))
				{
 					if(pstMetaDrm->bCasCc == TRUE)
					{
						BUS_SendMessage(NULL, eSEVT_CAS_DRM_SIGNALLED, pstContext->hAction, 0, 0, (HINT32)pstMetaDrm);
					}
					else if(pstMetaDrm->bSiCc == TRUE)
					{
						BUS_SendMessage(NULL, eSEVT_SI_DRM_SIGNALLED, pstContext->hAction, 0, 0, (HINT32)pstMetaDrm);
					}

					pstList->ulDrmRefTime = pstTimeStamp->stTimeStamp.ulTime;
				}

				break;
			}

			default:
			break;
		}
	}

	return ERR_OK;
}

BUS_Result_t xmgr_playback_NagraMsgEvtPbTimecodeChange_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	 hErr = ERR_FAIL;
	Handle_t hRecAction = HANDLE_NULL;
	SvcRec_RecInfo stRecInfo;

	/* timestamp of meta */
	if (eMgrPb_TickState_PLAY == pstContext->eTrickState)
	{
		xmgr_playback_NagraCheckDrmTimeStamp_Base (pstContext, (HUINT32)p1);
	}
	else
	{
		xmgr_playback_NagraCheckTimeStamp_Base (pstContext, (HUINT32)p1);
	}

	/* PAUSE 상태는 TimeCode Update를 안함.*/
	if (eMgrPb_TickState_PAUSE == pstContext->eTrickState)
	{
		return MESSAGE_PASS;
	}

	pstContext->ulPlayTime = (UNIXTIME)p1;
	pstContext->bFirstTimecode = TRUE;


	// Medialist 에서 레코딩중인 파일을 재생한 경우에 대한 처리
	hErr = SVC_META_GetRecActionByContentId (pstContext->ulContentId, &hRecAction);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hRecAction))
	{
		return MESSAGE_PASS;
	}

	HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcRec_RecInfo));
	hErr = SVC_REC_GetInfo (hRecAction, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_REC_GetInfo. Error]n");
		return MESSAGE_PASS;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if(pstContext->stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_NagraSes)
	{
		// TODO : eCasCcType_Nagra_Ses인경우 처리해 주어야함.
		#if 0
		hErr = xmgr_playback_CiPlusApplyRetentionLimit_Base(pstContext, TRUE);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("ERROR: xmgr_playback_CiPlusApplyRetentionLimit_Base()\n");
		}
		#endif
	}
#else
	if(pstContext->stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_Nagra)
	{
		// TODO : eDxCopyrightControl_CasCcType_Nagra인경우 처리해 주어야함.
		#if 0
		hErr = xmgr_playback_CiPlusApplyRetentionLimit_Base(pstContext, TRUE);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("ERROR: xmgr_playback_CiPlusApplyRetentionLimit_Base()\n");
		}
		#endif
	}
#endif

	if (pstContext->ePbType & (eMgrPb_PlayType_Normal | eMgrPb_PlayType_Chase))
	{
		pstContext->ulEndTime = stRecInfo.ulRecTime;
	}
	else if ((pstContext->ePbType & 0x0000000f) == eMgrPb_PlayType_Tsr)
	{
		pstContext->ulStartTime = stRecInfo.ulStartTime;
		pstContext->ulEndTime	= stRecInfo.ulEndTime;
	}

#if	defined(CONFIG_IGNORE_TRICKPLAY_IN_PMTCHANGED_SERVCE)
	//	Need to control Trick position
	//	언제는 Disable인 경우도 오겠지?
	if (pstContext->bTrickEnable)
	{
		//	PMT is updated!!!
		if (TRUE == p3)
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

	NOT_USED_PARAM(p2);

	return MESSAGE_PASS;
	//return MESSAGE_BREAK; -> original code, why stop here ?
}

BUS_Result_t xmgr_playback_NagraMsgEvtCasDrmSignalled_Base(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCopyrightControl_t	stDrmInfo;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(&stDrmInfo, 0, sizeof(DxCopyrightControl_t));
	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	if(xmgr_playback_NagraIsValidCasDrmInfo_Base(&stDrmInfo) != ERR_OK)
	{
		HxLOG_Error("drmInfo is error\n");
		return MESSAGE_PASS;
	}

	pstContext->bDrmValid = TRUE;
	HxSTD_MemCopy(&pstContext->stDrmInfo.stCasCcInfo, &stDrmInfo.stCasCcInfo, sizeof(DxCopyrightControl_CasCcInfo_t));
	xmgr_playback_SetCopyProtection(pstContext);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if(stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_NagraSes)
	{
		xmgr_playback_NagraMsgEvtCasDrmSignalled_Ses(pstContext, hAction, p1, p2, p3);
	}
	else
	{
		// TODO : MODEL별 .
	}
#endif

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_playback_NagraMsgEvtRecordTimeStampUpdate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR					hErr				=	ERR_FAIL;
	HUINT32 				ulPlaybackTime		=	0;
	SvcMeta_TimestampList_t *pstTimeStampList	=	&pstContext->stTimeStampList; /* owner playback of mgr-context*/
	svcPb_State_e			ePbState			=	ePB_STATE_Init;
	HUINT32					ulContentID			=	pstContext->ulContentId;

	/* reload timestamp from meta to mgr-context*/
	if (NULL == pstTimeStampList)
		return ERR_FAIL;

	hErr = SVC_META_UnloadTimeStamp (pstTimeStampList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_META_UnloadTimeStamp Error. (%x)\n", hErr);
		return hErr;
	}

	hErr = SVC_META_LoadTimeStamp (ulContentID, pstTimeStampList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_META_LoadTimeStamp Error. (%x)\n", hErr);
		return hErr;
	}

	/* update timestamp to mgr-context */
	hErr = SVC_PB_GetState (hAction, &ePbState);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PB_GetState Error. (%x)\n", hErr);
		return hErr;
	}

	if (ePB_STATE_Trick == ePbState)
	{
		hErr = SVC_PB_GetPlayTime (hAction, &ulPlaybackTime );
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_PB_GetPlayTime Error. (%x)\n", hErr);
			return hErr;
		}
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xproc_playback_NagraProto_Base (xmgrPbContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_BUS_CREATE\n", ulActionId);
			eBusResult = xmgr_playback_NagraMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_BUS_DESTROY\n", ulActionId);
			eBusResult = xmgr_playback_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_TIMER:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_BUS_TIMER\n", ulActionId);
			eBusResult = xmgr_playback_MsgBusTimer_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_ACTION_NOTIFY_RELEASED:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_ACTION_NOTIFY_RELEASED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAmActionStopped_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_NOTIFY_RECORD_STOPPED:
		case eMEVT_PVR_NOTIFY_RECORD_SPLIT_FILE:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_PVR_NOTIFY_RECORD_STOPPED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppRecordStop_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eOEVT_PG_CHECK_RATING :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eOEVT_PG_CHECK_RATING\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppCheckRating_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_SVCLIST_SVC_PID_CHANGED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_SVCLIST_SVC_PID_CHANGED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppDbPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PG_PIN_CHECK_OK :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_PG_PIN_CHECK_OK\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppPinOK_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PG_PIN_CHECK_DENIED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_PG_PIN_CHECK_DENIED\n", ulActionId);
			eBusResult = xmgr_playback_MsgAppPinDenied_Base(pstContext, hAction, p1, p2, p3);
			break;


		case eSEVT_PB_SETUP:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_SETUP\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbSetup_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_SESSION_STARTED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_SESSION_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvDecodingStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_START:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_START\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbStart_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PAT_TIMEOUT:
		case eSEVT_SI_PMT_TIMEOUT:
		case eSEVT_SI_PMT_PIDREMOVED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_PAT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiPmtTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT_PIDCHANGED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_PMT_PIDCHANGED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiPmtPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_EIT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiEitReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT_TIMEOUT :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SI_EIT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSiEitTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_VIDEO_UNDERRUN :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_VIDEO_UNDERRUN\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvVideoUnderrun_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_VIDEO_STARTED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_VIDEO_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvVideoStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_AUDIO_UNDERRUN :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_AUDIO_UNDERRUN\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvAudioUnderrun_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_AV_AUDIO_STARTED :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_AV_AUDIO_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvAudioStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_TIMECODE_CHANGE :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_TIMECODE_CHANGE\n", ulActionId);
			eBusResult = xmgr_playback_NagraMsgEvtPbTimecodeChange_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_EOF :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_EOF\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbEOF_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_BOF :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_BOF\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtPbBOF_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SBTL_STOPPED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SBTL_STOPPED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSbtlStopped_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SBTL_STARTED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_SBTL_STARTED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtSbtlStarted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_OK :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_OK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasOk_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasFail_Base(pstContext, message, p1, p2, p3);
			break;

		case eSEVT_CAS_CHECK :
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_CHECK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasFail_Base(pstContext, message, p1, p2, p3);
			break;

		case eMEVT_CAS_SYSINFO_UPDATED:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_CAS_SYSINFO_UPDATED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasScInserted_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_SC_REMOVED:
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_SC_REMOVED\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtCasScRemoved_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_AV_BLANK:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_DRM_AV_BLANK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvBlankCtrlByCAS_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_PB_DRM_AV_BLANK:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_PB_DRM_AV_BLANK\n", ulActionId);
			eBusResult = xmgr_playback_MsgEvtAvBlankByDrmInfo_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_SIGNALLED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eSEVT_CAS_DRM_SIGNALLED\n", ulActionId);
			eBusResult = xmgr_playback_NagraMsgEvtCasDrmSignalled_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_RECORD_TIMESTAMP_UPDATE:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Debug("[MGR:Pb(%d)] MESSAGE : eMEVT_PVR_RECORD_TIMESTAMP_UPDATE\n", ulActionId);
			eBusResult = xmgr_playback_NagraMsgEvtRecordTimeStampUpdate_Base(pstContext, hAction, p1, p2, p3);
			break;

		default :
			break;
	}

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_NagraAction0_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (0);

	return xproc_playback_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_NagraAction1_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (1);

	return xproc_playback_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_NagraAction2_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (2);

	return xproc_playback_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_NagraAction3_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (3);

	return xproc_playback_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_NagraAction4_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (4);

	return xproc_playback_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_NagraAction5_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (5);

	return xproc_playback_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_NagraAction6_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (6);

	return xproc_playback_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_NagraAction7_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t		*pstContext = xmgr_playback_GetContextFromIndex_Base (7);

	return xproc_playback_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

#define _____MSG_PROCESSING_FUNCTIONS_____

BUS_Callback_t	xmgr_playback_NagraGetProcFunc_Base (HUINT32 ulActionId)
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

	switch (ulPbIndex)
	{
	case 0:
		return xproc_playback_NagraAction0_Base;

	case 1:
		return xproc_playback_NagraAction1_Base;

	case 2:
		return xproc_playback_NagraAction2_Base;

	case 3:
		return xproc_playback_NagraAction3_Base;

	case 4:
		return xproc_playback_NagraAction4_Base;

	case 5:
		return xproc_playback_NagraAction5_Base;

	case 6:
		return xproc_playback_NagraAction6_Base;

	case 7:
		return xproc_playback_NagraAction7_Base;

	default:
		break;
	}

	return NULL;
}
