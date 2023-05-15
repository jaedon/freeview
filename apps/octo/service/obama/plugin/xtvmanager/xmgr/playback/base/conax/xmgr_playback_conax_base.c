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

#include <conax.h>
#include <conax_ctrl.h>

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

#if defined(CONFIG_DEBUG)
STATIC void xmgr_playback_ConaxDebugRegisterCmd(void);
#endif

#define _____LOCAL_FUNCTIONS_____
STATIC HERROR xmgr_playback_ConaxUpdateSessionKey(xmgrPbContext_t *pstContext)
{
	SvcPb_Setup_t		stSetup;
	SvcMeta_Record_t	stRecInfo;

	HxSTD_memset(&stRecInfo, 0, sizeof(SvcMeta_Record_t));

	if(SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo() error\n");
	}

	if(stRecInfo.u1cEncryption)
	{
		stSetup.bEncryption = TRUE;
		stSetup.bUseUserEncKey = TRUE;

		if(CXAPI_PVR_Pb_GetSessionKey(pstContext->hAction, stSetup.aucKey) != ERR_OK)
		{
			HxLOG_Error("CXAPI_PVR_Pb_GetSessionKey() error\n");
			return ERR_FAIL;
		}

		if(SVC_PB_SetDecrypt(pstContext->hAction, &stSetup, TRUE) != ERR_OK)
		{
			HxLOG_Error("SVC_PB_SetDecrypt failed. hAction=(0x%x) \n", pstContext->hAction);
		}
	}

	return ERR_OK;
}

STATIC void xmgr_playback_ConaxDisableOutput(xmgrPbContext_t *pstContext)
{
	if(pstContext->bAVHide != TRUE)
	{
		if(SVC_AV_SetVideoHide(pstContext->ulActionId, eAvMuteFlag_Cas, TRUE) != ERR_OK)
			HxLOG_Error("SVC_AV_SetVideoHide() error\n");

		if(SVC_AV_SetAudioHide(pstContext->ulActionId, eAvMuteFlag_Cas, TRUE) != ERR_OK)
			HxLOG_Error("SVC_AV_SetAudioHide() error\n");
	}

	pstContext->bAVHide = TRUE;
}

STATIC void xmgr_playback_ConaxEnableOutput(xmgrPbContext_t *pstContext)
{
	if(pstContext->bAVHide == TRUE)
	{
		if(SVC_AV_SetVideoHide(pstContext->ulActionId, eAvMuteFlag_Cas, FALSE) != ERR_OK)
			HxLOG_Error("SVC_AV_SetVideoHide() error\n");

		if(SVC_AV_SetAudioHide(pstContext->ulActionId, eAvMuteFlag_Cas, FALSE) != ERR_OK)
			HxLOG_Error("SVC_AV_SetAudioHide() error\n");
	}

	pstContext->bAVHide = FALSE;
}

// conax 6.0 vt 87p, 8.2.2.6 - automatically change to normal playback mode (change trickplay)
STATIC BUS_Result_t xmgr_playback_ConaxCheckUriTrickPlay(xmgrPbContext_t *pstContext)
{
	MgrPvr_TrickState_e	stState = 0;
	MgrPb_TrickSpeed_e	stSpeed = 0;
	HUINT8        	ucTrickPlay = 0;
	HBOOL       	bChangedTrick = FALSE;

	stState = pstContext->eTrickState;
	stSpeed = pstContext->eTrickSpeed;

	if(pstContext->bUriValid == TRUE && pstContext->bAvStarted == TRUE)
	{
		ucTrickPlay = (HUINT8)pstContext->stCxUri.eTrickPlayCtrl;
		HxLOG_Debug("CurTrick: %d , UriTrick: %d/%d **\n", ucTrickPlay, stState, stSpeed);

		switch(ucTrickPlay)
		{
			case eDxCopyrightControl_ConaxTrickPlayCtrl_NoRestriction:
			    break;

			case eDxCopyrightControl_ConaxTrickPlayCtrl_FF2X:
			    if (stState == eMgrPb_TickState_FF && stSpeed > eMgrPb_Speed_X1)
				{
					bChangedTrick = TRUE;
			    }
			    break;

			case eDxCopyrightControl_ConaxTrickPlayCtrl_FF4X:
			    if (stState == eMgrPb_TickState_FF && stSpeed > eMgrPb_Speed_X2)
				{
					bChangedTrick = TRUE;
			    }
			    break;

			case eDxCopyrightControl_ConaxTrickPlayCtrl_FF8X:
			    if (stState == eMgrPb_TickState_FF && stSpeed > eMgrPb_Speed_X3)
				{
					bChangedTrick = TRUE;
			    }
			    break;

			case eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpFF:
			    if (stState == eMgrPb_TickState_FF)
				{
					bChangedTrick = TRUE;
			    }
			    break;

			case eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpPauseFF:
			    if (stState == eMgrPb_TickState_PAUSE || stState == eMgrPb_TickState_FF)
			    {
					bChangedTrick = TRUE;
			    }
			    break;

			default:
			    break;
		}

		if(bChangedTrick)
		{
			MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 100);
			HxLOG_Debug("change trick play state!! -> Normal play!!!!\n");
		}
	}
    return ERR_OK;
}

/* 	conax 6.0 - enhanced pvr, retention limit
	- vt 75p ~ 81p, 8.2.1 Retention limits.
	- ct 22p. 9.3.B. the STB shall delete a recording when all content period have expired.
*/
STATIC HERROR xmgr_playback_ConaxCheckAllRetentionLimit(xmgrPbContext_t *pstContext)
{
    HERROR        hErr = ERR_OK;
    HBOOL        bExpired = FALSE;

    hErr = CXAPI_PVR_Pb_CheckAllUriRL(pstContext->hAction, pstContext->ulUnixStartTime, &bExpired);
    if(hErr == ERR_OK && bExpired == TRUE)
    {
	    pstContext->bAllUriExpired = TRUE;
		xmgr_playback_ConaxDisableOutput(pstContext);
		/* 2015/05/27
		GWM_SendMessage (NULL, MSG_APPC2U_INFORM_PLAYBACK_STOPPED_BY_DRM_EXPIRED, HANDLE_NULL, 0, 0, 0);
		*/
    }

    return hErr;
}

/* STB Conformity Requirements Specification, Chipset Pairing STB 6.0 - 9.2-C
	-> If a recording contains multiple content periods with different retention limits,
	the STB shall only allow playback of periods whose retention limit has not expired.
*/
STATIC HERROR xmgr_playback_ConaxCheckRetentionLimit(xmgrPbContext_t *pstContext, HBOOL *pbExpired)
{
	HERROR	hErr = ERR_OK;
	UNIXTIME ulExpiredTime = 0;

	hErr = SVC_META_CheckDrmExpired(&pstContext->stDrmInfo, pstContext->ulUnixStartTime + pstContext->ulPlayTime, pbExpired, &ulExpiredTime);
	if(hErr == ERR_OK && *pbExpired == TRUE)
	{
		//If retention limit has expired, pb next content period.
		hErr = MGR_PLAYBACK_SetJumpPlay(pstContext->ulActionId, pstContext->ulCxNextUriTime);
		hErr = MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 100);
		xmgr_playback_ConaxDisableOutput(pstContext);

		HxLOG_Warning("expired!!! endTime: %d, RL: %d\n", pstContext->ulEndTime, pstContext->stCxUri.eRetentionLimit);

		// display dialogue, jump playable position (block)
		/* 2015/05/27
		GWM_SendMessage (NULL, MSG_APPC2U_INFORM_PLAYBACK_STOPPED_BY_DRM_EXPIRED, HANDLE_NULL, 0, 0, TRUE);
		if(GWM_APP_Get(AP_Playbar_Proc) != NULL) //시나리오 문의.
		{
			GWM_APP_Destroy (AP_Playbar_Proc);
 		}
 		*/
	}
	return hErr;
}

// conax 6.0 -check tsr retention limit (temp.)
STATIC HERROR xmgr_playback_ConaxCheckRetentionLimitForTsr(xmgrPbContext_t *pstContext)
{

	HERROR		hErr = ERR_OK;
	HUINT32 	ulUnixTime, ulPlayTime, ulRLTime = 0, ulMinRLTime = (90 * SECONDS_PER_MIN);
	HBOOL		bTsrRecord = FALSE;
	UNIXTIME	ulExpiredTime = 0;
 	HBOOL		bExpired = FALSE;
	SvcMeta_Record_t stRecInfo;

	// check if it is tsr recording or not, get current playtime
	bTsrRecord = SVC_META_IsTsrContents(pstContext->ulContentId); //AP_Record_IsTsrRecordId(pstContext->ulContentId, &stRecInfo);
	hErr = SVC_PB_GetPlayTime(pstContext->hAction, &ulPlayTime);
	if (hErr != ERR_OK || bTsrRecord == FALSE)
	{
		HxLOG_Debug("ERR_FAIL or bTsrRecord: %d **\n", bTsrRecord);
		return ERR_FAIL;
	}

	if(SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo() error\n");
		return ERR_FAIL;
	}
#if 0
	// temporary code - check minimum RL (spec.)
	if ((pstContext->ulEndTime < (ulPlayTime + ulMinRLTime)) || (pstContext->stCxUri.eRetentionLimit != eDxCopyrightControl_ConaxRetentionLimit_90Min))
	{
		HxLOG_Debug("endTime: %d, playTime: %d , play+MinRL: %d, RL: %d\n", pstContext->ulEndTime, ulPlayTime, ulPlayTime+ulMinRLTime, pstContext->stCxUri.eRetentionLimit);
		return ERR_OK;
	}
#endif
	// get expiredTime (get retention limit duration)
	hErr = SVC_META_CheckDrmExpired(&pstContext->stDrmInfo, pstContext->ulUnixStartTime, &bExpired, &ulExpiredTime);
	if (hErr == ERR_OK)
	{
		if(VK_CLOCK_GetTime(&ulUnixTime) != VK_OK)
		{
			HxLOG_Error("VK_CLOCK_GetTime() error\n");
		}

		ulExpiredTime = pstContext->ulUnixStartTime + CONFIG_MW_MM_TSR_DURATION;

		// cur rectime - cur playtime >  expired duration (drm) => expired!!

		if((ulUnixTime > ulExpiredTime)/* && (ulUnixTime - ulExpiredTime) > CONFIG_MW_MM_TSR_DURATION*/)
		{
			pstContext->bAllUriExpired = TRUE;

			ulRLTime = ulUnixTime - ulExpiredTime - ulPlayTime + 10;
			/*
				expired가 되었을 경우, expired가 되지 않은 위치로 jump, av block, expired dialogue를 띄웁니다.
				work around - expired 되지 않은 위치에서 10초 후의 위치를 지정하였습니다. (처리시간 및 expired 된 위치가 부정확할 것을 고려함.)
			*/
			hErr = MGR_PLAYBACK_SetJumpPlay(pstContext->ulActionId, ulRLTime);
			hErr = MGR_PLAYBACK_SetPlaySpeed (pstContext->ulActionId, 100);
			xmgr_playback_ConaxDisableOutput(pstContext);
			// display dialogue, jump playable position (block)
			/* 2015/05/27
			GWM_SendMessage (NULL, MSG_APPC2U_INFORM_PLAYBACK_STOPPED_BY_DRM_EXPIRED, HANDLE_NULL, 0, 0, TRUE);
			*/
			HxLOG_Warning("expired!!! endTime: %d, playTime: %d, RL: %d\n", pstContext->ulEndTime, ulPlayTime, pstContext->stCxUri.eRetentionLimit);
		}
	}
	return hErr;
}

#if defined(CONFIG_MW_CAS_CONAX_URI)
STATIC void xmgr_playback_ConaxApplyUri(xmgrPbContext_t *pstContext)
{
	HERROR	hErr = ERR_OK;
 	HBOOL	bExpired = FALSE;

	HxLOG_Trace();

	pstContext->bDrmValid = TRUE;
	HxSTD_MemSet(&pstContext->stDrmInfo, 0x00, sizeof(DxCopyrightControl_ConaxDrmInfo_t));
	pstContext->stDrmInfo.stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_Conax;
	HxSTD_memcpy(&pstContext->stDrmInfo.stCasCcInfo.info.stConaxDrmInfo, &pstContext->stCxUri, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

#if defined(CONFIG_DEBUG)
	CXSC_PrintUriValue(&pstContext->stCxUri);
#endif

	// current URI for the current content period shall be applied. (conax chipset pairing CT 9.2-A 20p)
	if(((pstContext->ePbType & eMgrPb_PlayType_Normal) == eMgrPb_PlayType_Normal)
		|| (((pstContext->ePbType & eMgrPb_PlayType_Chase) == eMgrPb_PlayType_Chase) && (pstContext->ulEndTime > 86400))) //  RL: 24h
	{
		hErr = xmgr_playback_ConaxCheckRetentionLimit(pstContext, &bExpired);
	}

	if(hErr == ERR_OK && bExpired == FALSE)
	{
		xmgr_playback_SetCopyProtection(pstContext);
		xmgr_playback_ConaxCheckUriTrickPlay(pstContext);
	}
}

STATIC BUS_Result_t xmgr_playback_ConaxMsgEvtPbDrmSignalled_Base(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCopyrightControl_t	*pstCcInfo;

	if (pstContext == NULL)
	{
		return ERR_BUS_NO_ERROR;
	}

	pstCcInfo = (DxCopyrightControl_t *)p3;
	if(pstCcInfo->bCasCc != TRUE)
		return ERR_BUS_NO_ERROR;

	if(pstCcInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_Conax)
		return ERR_BUS_NO_ERROR;

	// update uri info
	HxSTD_memcpy(&pstContext->stCxUri, &pstCcInfo->stCasCcInfo.info.stConaxDrmInfo, sizeof(DxCopyrightControl_ConaxDrmInfo_t));
	pstContext->bUriValid = TRUE;

	if (CXAPI_PVR_GetNextUriTimeFromMetaFile(pstContext->hAction, &pstContext->ulCxNextUriTime) != ERR_OK)
	{
		if(pstContext->ePbType & eMgrPb_PlayType_Tsr)
		{
			pstContext->ulCxNextUriTime = 0;
		}
		else
		{
		    pstContext->ulCxNextUriTime = pstContext->ulEndTime;
		}
	}

	HxLOG_Debug("curTime: %d , nextUriTime: %d \n", p1, pstContext->ulCxNextUriTime);

	xmgr_playback_ConaxApplyUri(pstContext);

	return ERR_BUS_NO_ERROR;
}
#endif

STATIC BUS_Result_t xmgr_playback_ConaxMsgEvtCasOk_Base(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(p1 != eDxCAS_GROUPID_CX)
		return MESSAGE_PASS;

	pstContext->bCasOk = TRUE;
	pstContext->nCasErrCode = 0;

	if(pstContext->bAllUriExpired != TRUE)
	{
		xmgr_playback_ConaxEnableOutput(pstContext);
	}

	switch(pstContext->ePbType & 0x0000000f)
	{
		case eMgrPb_PlayType_Normal:
			if(pstContext->bPauseByCAS == TRUE)
			{
				if(pstContext->eTrickState == eMgrPb_TickState_PAUSE)
				{
					MGR_PLAYBACK_SetPlaySpeed(pstContext->ulActionId, 100);
				}

				pstContext->bPauseByCAS = FALSE;
			}
			break;

		case eMgrPb_PlayType_Tsr:
		case eMgrPb_PlayType_Chase:
			break;

		default:
			return ERR_BUS_NO_ERROR;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_playback_ConaxMsgEvtCasFail_Base(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(p1 != eDxCAS_GROUPID_CX)
		return MESSAGE_PASS;

	pstContext->bCasOk = FALSE;
	pstContext->nCasErrCode = p2;

	xmgr_playback_ConaxDisableOutput(pstContext);

	switch(pstContext->ePbType & 0x0000000f)
	{
		case eMgrPb_PlayType_Normal:
			if(pstContext->bPauseByCAS != TRUE)
			{
				if(pstContext->eTrickState != eMgrPb_TickState_PAUSE)
				{
					MGR_PLAYBACK_SetPlaySpeed(pstContext->ulActionId, 0);
				}

				pstContext->bPauseByCAS = TRUE;
			}
			break;

		case eMgrPb_PlayType_Tsr:
		case eMgrPb_PlayType_Chase:
			break;

		default:
			return ERR_BUS_NO_ERROR;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_playback_ConaxMsgEvtCasCheck_Base(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(p1 != eDxCAS_GROUPID_CX)
		return MESSAGE_PASS;

	pstContext->bCasOk = FALSE;
	pstContext->nCasErrCode = p2;

	xmgr_playback_ConaxDisableOutput(pstContext);

	switch(pstContext->ePbType & 0x0000000f)
	{
		case eMgrPb_PlayType_Normal:
			if(pstContext->bPauseByCAS != TRUE)
			{
				if(pstContext->eTrickState != eMgrPb_TickState_PAUSE)
				{
					MGR_PLAYBACK_SetPlaySpeed(pstContext->ulActionId, 0);
				}

				pstContext->bPauseByCAS = TRUE;
			}
			break;

		case eMgrPb_PlayType_Tsr:
		case eMgrPb_PlayType_Chase:
			break;

		default:
			return ERR_BUS_NO_ERROR;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_playback_ConaxMsgEvtPbPlaybackError_Base(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulContentId = pstContext->ulContentId;
	HBOOL	bDelete = (HBOOL)p3;

	if((DxCAS_GroupId_e)p2 == eDxCAS_GROUPID_CX)
	{
		BUS_SendMessageToOM(NULL, eMEVT_SYSCTL_CONAX_PLAYBACK_ERROR, hAction, p1, p2, p3);

		if(bDelete == TRUE)
		{
			if(SVC_META_DeleteContents(ulContentId) != ERR_OK)
			{
				HxLOG_Error("SVC_META_DeleteContents() error\n");
			}
		}

		return MESSAGE_BREAK;
	}

	return MESSAGE_PASS;
}

#define _____PROTO_PROCEDURE_____

STATIC BUS_Result_t xmgr_playback_ConaxMsgBusCreate_Base(xmgrPbContext_t* pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusResult;
	SvcMeta_Record_t stRecInfo;

	eBusResult = xmgr_playback_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);

	if(pstContext->svcInfo.eCasType & eDxCAS_TYPE_CONAX)
	{
		if(CXAPI_PVR_Pb_Open(pstContext->hAction, pstContext->ulContentId, pstContext->hSvc) != ERR_OK)
		{
			HxLOG_Error("CXAPI_PVR_Pb_Open() error\n");
			return eBusResult;
		}

		// conax 6.0 - for enhanced pvr, need recording start/end time
		if(SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo) != ERR_OK)
		{
			HxLOG_Error("SVC_META_GetRecordInfo() error\n");
			return eBusResult;
		}

		pstContext->ulUnixStartTime = stRecInfo.ulStartTime;
		pstContext->ulUnixEndTime = stRecInfo.ulEndTime;
	}

	return eBusResult;
}

STATIC BUS_Result_t xmgr_playback_ConaxMsgEvtSiPmtReceived_Base(xmgrPbContext_t* pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eResult = ERR_BUS_NO_ERROR;

	eResult = xmgr_playback_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);

	if(pstContext->bAvStarted != TRUE)
	{
		if(pstContext->svcInfo.eCasType & eDxCAS_TYPE_CONAX)
		{
			xmgr_playback_ConaxDisableOutput(pstContext);
		}
	}

	return eResult;
}

STATIC BUS_Result_t xmgr_playback_ConaxMsgEvtPbTimecodeChange_Base(xmgrPbContext_t* pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eResult = ERR_BUS_NO_ERROR;
	HBOOL bUpdateKey = FALSE;

	if(pstContext->svcInfo.eCasType & eDxCAS_TYPE_CONAX)
	{
		if(CXAPI_PVR_Pb_ChangeTimecode(hAction, p1, &bUpdateKey) != ERR_OK)
		{
			HxLOG_Error("CXAPI_PVR_Pb_ChangeTimecode() error\n");
		}
#if defined(CONFIG_MW_CAS_CONAX_PRETEST)
		if(pstContext->bDrmValid == TRUE)
		{
			xmgr_playback_ConaxCheckUriTrickPlay(pstContext);

			if((pstContext->ePbType & eMgrPb_PlayType_Normal) == eMgrPb_PlayType_Normal)
			{
				if(pstContext->bAvStarted == FALSE && pstContext->bAllUriExpired == FALSE)
				{
					if(xmgr_playback_ConaxCheckAllRetentionLimit(pstContext) != ERR_OK)
					{
						HxLOG_Error("xmgr_playback_ConaxCheckAllRetentionLimit() error\n");
					}
				}

				if(pstContext->bAllUriExpired == TRUE) // all uri expired 되면 break 한다.
				{
					return ERR_BUS_NO_ERROR;
				}
			}

			if((pstContext->ePbType & eMgrPb_PlayType_Tsr) == eMgrPb_PlayType_Tsr)
			{
				if(xmgr_playback_ConaxCheckRetentionLimitForTsr(pstContext) != ERR_OK)
				{
					HxLOG_Error("xmgr_playback_ConaxCheckRetentionLimitForTsr() error\n");
				}
			}
		}
#endif
	}

	return eResult;
}

STATIC BUS_Result_t xmgr_playback_ConaxMsgEvtPbUpdateKey_Base(xmgrPbContext_t* pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(p2 != eDxCAS_GROUPID_CX)
	{
		HxLOG_Error("p2 != eDxCAS_GROUPID_CX\n");
		return MESSAGE_PASS;
	}

	if(pstContext->svcInfo.eCasType & eDxCAS_TYPE_CONAX)
	{
#if defined(CONAX_PVR_SPEC_ENABLE)
		if(xmgr_playback_ConaxUpdateSessionKey(pstContext) != ERR_OK)
		{
			HxLOG_Error("CXAPI_PVR_Pb_GetSessionKey() error\n");
			goto _RTN;
		}
#endif
	}
_RTN:
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_playback_ConaxMsgBusDestroy_Base(xmgrPbContext_t* pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusResult;

	if(pstContext->hAction != hAction)
	{
		HxLOG_Warning("pstContext->hAction (%x), hAction (%x)\n", pstContext->hAction, hAction);
		hAction = pstContext->hAction;
	}

	eBusResult = xmgr_playback_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);

	if(CXAPI_PVR_Pb_Close(hAction) != ERR_OK)
	{
		HxLOG_Error("CXAPI_PVR_Pb_Close() error\n");
		goto _RTN;
	}

_RTN:
	return eBusResult;
}

STATIC BUS_Result_t	xmgr_playback_ConaxEvtAvStarted_Base(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusResult = ERR_BUS_NO_ERROR;

	pstContext->bAvStarted = TRUE;

	return eBusResult;
}

STATIC BUS_Result_t xproc_playback_ConaxProto_Base(xmgrPbContext_t* pstContext, HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult	= ERR_BUS_NO_ERROR;
	HUINT32			ulActionId	= SVC_PIPE_GetActionId(hAction);
	BUS_Callback_t	pfBase;

	switch (nMessage)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Warning ("********************************************************************************\n");
		HxLOG_Warning ("*  XMGR : CONAX : PLAYBACK : CREATE (%d)\n", ulActionId);
		HxLOG_Warning ("********************************************************************************\n");

		HxLOG_Info("[MGR:PbConax(%d)] MESSAGE : eMEVT_BUS_CREATE\n", ulActionId);
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
		eBusResult = xmgr_playback_ConaxMsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Info("[MGR:PbConax(%d)] MESSAGE : eMEVT_BUS_DESTROY\n", ulActionId);
		eBusResult = xmgr_playback_ConaxMsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);

		HxLOG_Warning ("********************************************************************************\n");
		HxLOG_Warning ("*  XMGR : CONAX : PLAYBACK : DESTROY (%d)\n", ulActionId);
		HxLOG_Warning ("********************************************************************************\n");
		break;

	case eSEVT_SI_PMT:
		CHECK_PB_ACTION_HANDLE();
		HxLOG_Info("[MGR:PbConax(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
		eBusResult = xmgr_playback_ConaxMsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
		break;

	case eSEVT_PB_TIMECODE_CHANGE :
		CHECK_PB_ACTION_HANDLE();
		HxLOG_Info("[MGR:PbConax(%d)] MESSAGE : eSEVT_PB_TIMECODE_CHANGE\n", ulActionId);
		eBusResult = xmgr_playback_ConaxMsgEvtPbTimecodeChange_Base(pstContext, hAction, p1, p2, p3);
		break;

	case eSEVT_CAS_UPDATE_KEYINFO:
		CHECK_PB_ACTION_HANDLE();
		HxLOG_Info("[XMGR:PbConax(%d)] MESSAGE : eSEVT_CAS_UPDATE_KEYINFO\n", ulActionId);
		eBusResult = xmgr_playback_ConaxMsgEvtPbUpdateKey_Base(pstContext, hAction, p1, p2, p3);
		break;

	case eSEVT_AV_VIDEO_STARTED:
	case eSEVT_AV_AUDIO_STARTED:
		CHECK_PB_ACTION_HANDLE();
		HxLOG_Info("[XMGR:PbConax(%d)] MESSAGE : eSEVT_AV_VIDEO_STARTED\n", ulActionId);
		eBusResult = xmgr_playback_ConaxEvtAvStarted_Base(pstContext, hAction, p1, p2, p3);
		break;

	case eSEVT_CAS_OK:
		CHECK_PB_ACTION_HANDLE();
		HxLOG_Info("[XMGR:PbConax(%d)] MESSAGE : eSEVT_CAS_OK\n", ulActionId);
		eBusResult = xmgr_playback_ConaxMsgEvtCasOk_Base(pstContext, hAction, p1, p2, p3);
		break;

	case eSEVT_CAS_FAIL:
		CHECK_PB_ACTION_HANDLE();
		HxLOG_Info("[XMGR:PbConax(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
		eBusResult = xmgr_playback_ConaxMsgEvtCasFail_Base(pstContext, hAction, p1, p2, p3);
		break;

	case eSEVT_CAS_CHECK:
		CHECK_PB_ACTION_HANDLE();
		HxLOG_Info("[XMGR:PbConax(%d)] MESSAGE : eSEVT_CAS_CHECK\n", ulActionId);
		eBusResult = xmgr_playback_ConaxMsgEvtCasCheck_Base(pstContext, hAction, p1, p2, p3);
		break;

	case eSEVT_CAS_DRM_SIGNALLED:
		CHECK_PB_ACTION_HANDLE();
		HxLOG_Info("[XMGR:PbConax(%d)] MESSAGE : eSEVT_CAS_DRM_SIGNALLED\n", ulActionId);
		eBusResult = xmgr_playback_ConaxMsgEvtPbDrmSignalled_Base(pstContext, hAction, p1, p2, p3);
		break;

	case eSEVT_CAS_CONAX_PLAYBACK_ERROR:
		CHECK_PB_ACTION_HANDLE();
		HxLOG_Info("[XMGR:PbConax(%d)] MESSAGE : eSEVT_CAS_CONAX_PLAYBACK_ERROR\n", ulActionId);
		eBusResult = xmgr_playback_ConaxMsgEvtPbPlaybackError_Base(pstContext, hAction, p1, p2, p3);
		break;

	default :
		break;
	}

	if (eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	// playback_ConaxAction의 Context와 playback_Proc의 Context를 일치 시키기 위해서 pstContext->ulActionId를 사용한다.
	pfBase = xmgr_playback_GetProcFunc_Base(pstContext->ulActionId);

	return pfBase(nMessage, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_ConaxAction0_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(0);

	return xproc_playback_ConaxProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_ConaxAction1_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(1);

	return xproc_playback_ConaxProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_ConaxAction2_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(2);

	return xproc_playback_ConaxProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_ConaxAction3_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(3);

	return xproc_playback_ConaxProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_ConaxAction4_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(4);

	return xproc_playback_ConaxProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_ConaxAction5_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(5);

	return xproc_playback_ConaxProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_ConaxAction6_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(6);

	return xproc_playback_ConaxProto_Base(pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_playback_ConaxAction7_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrPbContext_t* 		pstContext	= xmgr_playback_GetContextFromIndex_Base(7);

	return xproc_playback_ConaxProto_Base(pstContext, message, hAction, p1, p2, p3);
}

#define _____MSG_PROCESSING_FUNCTIONS_____
HERROR xmgr_playback_ConaxInit_Base(void)
{
#if defined(CONFIG_DEBUG)
	xmgr_playback_ConaxDebugRegisterCmd();
#endif
	return xmgr_playback_Init_Base();
}

HERROR	xmgr_playback_ConaxSetJumpPlay_Base (HUINT32 ulActionId, HUINT32 ulTimeToPlay)
{
	xmgrPbContext_t	*pstContext = NULL;

	pstContext = xmgr_playback_GetContext_Base(ulActionId);
	if( NULL == pstContext )
		return ERR_FAIL;

	if(pstContext->bDrmValid != TRUE)
		goto _GOTO_RETURN;

	if(pstContext->stDrmInfo.bCasCc != TRUE)
		goto _GOTO_RETURN;

	if(pstContext->stDrmInfo.stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_Conax)
		goto _GOTO_RETURN;

	switch(pstContext->stDrmInfo.stCasCcInfo.info.stConaxDrmInfo.eTrickPlayCtrl)
	{
		case eDxCopyrightControl_ConaxTrickPlayCtrl_FF2X:
		case eDxCopyrightControl_ConaxTrickPlayCtrl_FF4X:
		case eDxCopyrightControl_ConaxTrickPlayCtrl_FF8X:
		case eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpFF:
		case eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpPauseFF:
			return ERR_FAIL;

		default:
			break;
	}

_GOTO_RETURN:
	return xmgr_playback_SetJumpPlay_Base (ulActionId, ulTimeToPlay);
}

BUS_Callback_t xmgr_playback_ConaxGetProcFunc_Base(HUINT32 ulActionId)
{
	HUINT32	ulPbIndex	= 0;

	if ((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST))
	{
		ulPbIndex = (ulActionId - eActionId_VIEW_FIRST);
	}
	else if ((ulActionId >= eActionId_BG_FIRST) && (ulActionId <= eActionId_BG_LAST))
	{
		ulPbIndex = (ulActionId - eActionId_BG_FIRST) + NUM_VIEW_ACTION;
	}

	if (ulPbIndex >= MAX_NUM_OF_PLAY)
	{
		HxLOG_Error("Wrong Action ID:(%d)\n", ulActionId);
		return NULL;
	}

	switch (ulPbIndex)
	{
	case 0:
		return xproc_playback_ConaxAction0_Base;

	case 1:
		return xproc_playback_ConaxAction1_Base;

	case 2:
		return xproc_playback_ConaxAction2_Base;

	case 3:
		return xproc_playback_ConaxAction3_Base;

	case 4:
		return xproc_playback_ConaxAction4_Base;

	case 5:
		return xproc_playback_ConaxAction5_Base;

	case 6:
		return xproc_playback_ConaxAction6_Base;

	case 7:
		return xproc_playback_ConaxAction7_Base;

	default:
		break;
	}

	return NULL;
}

#define __________DEBUG_FUNC_________
#if defined(CONFIG_DEBUG)
STATIC HINT32 xmgr_playback_ConaxDebugPlaybackError(void *szArgList)
{
	HCHAR	*pucArg=NULL;
	HUINT32	ulActionId = 0;
	HUINT32 ulParam = 0;
	xmgrPbContext_t *pstContext = NULL;

	/* 1st argument */
	pucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if (pucArg == NULL)
		return HxCMD_ERR;

	if(sscanf(pucArg, "%d", &ulActionId) == 0)
		return HxCMD_ERR;

	pucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if (pucArg != NULL)
	{
		if(sscanf(pucArg, "%d", &ulParam) == 0)
		{
			return HxCMD_ERR;
		}
	}

	pstContext = xmgr_playback_GetContext_Base(ulActionId - eActionId_VIEW_FIRST);
	if (pstContext == NULL)
	{
		HxLOG_Error("xmgr_playback_GetContext_Base() failed");
		return HxCMD_ERR;
	}

	BUS_PostMessage (NULL, eSEVT_CAS_CONAX_PLAYBACK_ERROR, pstContext->hAction, 0, eDxCAS_GROUPID_CX, ulParam ? TRUE : FALSE);

	return ERR_OK;
}

STATIC void xmgr_playback_ConaxDebugRegisterCmd(void)
{
#define	hCmdHandle		"mgr_pb_conax"
	HLIB_CMD_RegisterWord (hCmdHandle, xmgr_playback_ConaxDebugPlaybackError,
						"pb_stop",										/* keyword */
						"pb_stop",										/* help  */
						"pb_stop");										/* usage  */
}
#endif

