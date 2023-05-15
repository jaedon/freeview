/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_core_player.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_core_player.h>

/**************************************************************************************************/
#define ___________________Private_variable________________________________________________
/**************************************************************************************************/
static	NxPlayerContext_t *s_pstPlayerContext = NULL;
/**************************************************************************************************/
#define ___________________Private_typedef________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ___________________Private_prototype________________________________________________
/**************************************************************************************************/
static NxPlayerContext_t *nx_player_GetContext(void);
static void		nx_player_ResetPlayerContent(NxPlayerContext_t *pstPlayerContext, HINT32 nPlayerdContentIndex);
static void		nx_player_AllResetPlayerContent(NxPlayerContext_t *pstPlayerContetx);

static HERROR	nx_player_SetPositionMedia(NxPlayerContentInform_t *pstPlayerContent, HUINT32 ulPosition);
static HERROR	nx_player_SetPositionTsr(NxPlayerContentInform_t *pstPlayerContent, HUINT32 ulPosition);

static void 	nx_player_Notifier ( unsigned int ulViewId, unsigned int ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData );
static void 	nx_player_Received_PvrPlayEvent(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData);
static void 	nx_player_Received_MediaPlayEvent(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData);

#if defined(CONFIG_DEBUG)
static void nx_player_Command_Init(void);
#endif

/**************************************************************************************************/
#define ___________________Private_functions________________________________________________
/**************************************************************************************************/
static 	NxPlayerContext_t *nx_player_GetContext(void)
{
	if (s_pstPlayerContext == NULL)
	{
		HINT32	i = 0;
		s_pstPlayerContext = (NxPlayerContext_t*)NX_APP_Calloc(sizeof(NxPlayerContext_t));
		for (i = 0; i < NX_PLAYER_SUPPORT_INST_NUM; i++)
		{
			s_pstPlayerContext->pstPlayerInstance[i] = (NxPlayerContentInform_t*)NX_APP_Calloc(sizeof(NxPlayerContentInform_t));
		}
	}
	return s_pstPlayerContext;
}

static void		nx_player_ResetPlayerContent(NxPlayerContext_t *pstPlayerContext, HINT32 nPlayerdContentIndex)
{
	HxSTD_MemSet(pstPlayerContext->pstPlayerInstance[nPlayerdContentIndex], 0, sizeof(NxPlayerContentInform_t));
}

static void		nx_player_AllResetPlayerContent(NxPlayerContext_t *pstPlayerContetx)
{
	HINT32 i = 0;
	for (i = 0; i < NX_PLAYER_SUPPORT_INST_NUM; i++)
	{
		nx_player_ResetPlayerContent(pstPlayerContetx, i);
	}
}

#define _________related_setPosition____________
static	HERROR	nx_player_SetPositionMedia(NxPlayerContentInform_t *pstPlayerContent, HUINT32 ulPosition)
{
	return NX_PORT_MEDIA_PLAY_SetPlayPosition(pstPlayerContent->ulViewId, ulPosition);
}

static	HERROR	nx_player_SetPositionTsr(NxPlayerContentInform_t *pstPlayerContent, HUINT32 ulPosition)
{
	HERROR			hErr = ERR_FAIL;
	DxService_t		 *pstMasterSvc = NULL;
	OxMediaPlay_MediaType_e eType;
	HBOOL	bPlayBuffer = FALSE;

	pstMasterSvc = NX_CHANNEL_FindServiceBySvcUid(pstPlayerContent->stMasterChannel.svcUid);

	if (pstMasterSvc == NULL)
	{
		HxLOG_Error("Not Found Master ServiceInfo!\n");
		return ERR_FAIL;
	}

	bPlayBuffer = (ulPosition + 2 < (pstPlayerContent->ulStartTime + pstPlayerContent->ulPlayMaxTime)) ? TRUE : FALSE;

	HxLOG_Debug("bPlayBuffer(%d), ulposition(%u), playMaxTime(%u)\n", bPlayBuffer, (ulPosition + 2), (pstPlayerContent->ulStartTime + pstPlayerContent->ulPlayMaxTime));

	NX_PORT_MEDIA_PLAY_GetPlayType(pstPlayerContent->ulViewId, &eType);

	switch(eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
		if (bPlayBuffer == TRUE)
		{
			HUINT32 					 ulSessionId;
			OxMediaPlay_StartInfo_t 	 unStart = {0,};

			unStart.stTsrPb.eType			= eOxMP_MEDIATYPE_TSRPLAYBACK;
			unStart.stTsrPb.ulMasterSvcUid	= pstPlayerContent->stMasterChannel.svcUid;
			unStart.stTsrPb.ulSuppleSvcUid	= pstPlayerContent->stMasterChannel.svcUid;
			unStart.stTsrPb.eSuppSvcType	= pstMasterSvc->svcType;
			unStart.stTsrPb.ulStartTime 	= ulPosition;

			hErr = NX_PORT_MEDIA_PLAY_Start(pstPlayerContent->ulViewId, &unStart, &ulSessionId);
			if( ERR_OK != hErr )
			{
				HxLOG_Error("NX_PORT_MEDIA_PLAY_Start Error \n");
				return hErr;
			}

			HxLOG_Error("\nStart eOxMP_MEDIATYPE_TSRPLAYBACK. svcuid(%u)\n", pstPlayerContent->stMasterChannel.svcUid);
		}
		break;

	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		if (bPlayBuffer == TRUE)
		{
			HxLOG_Debug("NX_PORT_MEDIA_PLAY_SetPlayPosition. ulPosition(%u)\n", ulPosition);
			NX_PORT_MEDIA_PLAY_SetPlayPosition(pstPlayerContent->ulViewId, ulPosition);
		}
		else
		{
			HxLOG_Error("NX_PORT_MEDIA_PLAY_Stop (eOxMP_MEDIATYPE_TSRPLAYBACK)\n");
			NX_PORT_MEDIA_PLAY_Stop(pstPlayerContent->ulViewId);
			// for start live
			ONDK_GWM_PostMessage(NULL, MSG_CORE_PLAYER_STOPPED_TSR_PLAYBACK, 0, 0, 0, 0);
		}
		break;

	case eOxMP_MEDIATYPE_NONE:
	default:
		if (bPlayBuffer != TRUE)
		{
			// start live
		}
		break;
	}


	return ERR_OK;
}

#define _________related_setPosition____________
static	HERROR	nx_player_SetSpeedMedia(NxPlayerContentInform_t *pstPlayerContent, HINT32 lSpeed)
{
	return NX_PORT_MEDIA_PLAY_SetPlaySpeed(pstPlayerContent->ulViewId, lSpeed);
}

#define _________related_player_setup___________
static 	HERROR	nx_player_SetupScenario(NxPlayerContentInform_t *pstPlayerContent)
{
	// 먼가,, media type 을 보고 시나리오가 달라지는것이 존재한다면.. 여기에..

	HCHAR	drmBuf[1024] = {'\0',};

	switch(pstPlayerContent->ePlayerType)
	{
	case ePlayerType_PVRPLAYBACK:
		break;

	case ePlayerType_MEDIAPLAY:
		break;

	default:
		break;
	}

	NX_PORT_MEDIA_PLAY_SetDrmLicenseData(pstPlayerContent->ulViewId, drmBuf);

	NX_PORT_MEDIA_PLAY_SeBufferingScenario(pstPlayerContent->ulViewId, FALSE, 10000);

	return ERR_OK;
}

static  HERROR	nx_player_Probe(NxPlayerContentInform_t *pstPlayerContent)
{
	if (pstPlayerContent == NULL)
	{
		HxLOG_Error("pstVodContent is NULL!!\n");
		return ERR_FAIL;
	}

	nx_player_SetupScenario(pstPlayerContent);

	return	NX_PORT_MEDIA_PLAY_Probe(pstPlayerContent->ulViewId, pstPlayerContent->stStartInfo.stMediaPlay.szUrl);
}

#define _________player_event_listener__________
static void 	nx_player_Notifier ( unsigned int ulViewId, unsigned int ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
	HxLOG_Error("received MediaEvent. eType(%d), eEvent(%d)\n", eType, eEvent);

	switch( eType )
	{
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
		{
			nx_player_Received_PvrPlayEvent(ulViewId, ulSessionId, eEvent, pstNotifyData);
			break;
		}
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		{
			nx_player_Received_MediaPlayEvent(ulViewId, ulSessionId, eEvent, pstNotifyData);
			break;
		}
		case eOxMP_MEDIATYPE_LIVE:
		default :
			break;
	}
}

static void 	nx_player_Received_PvrPlayEvent(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData)
{
	switch ( eEvent )
	{
	case eOxMP_EVENT_STARTED:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_STARTED \n");
		return;

	case eOxMP_EVENT_PLAYTIME:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_PLAYTIME \n");
		break;

	case eOxMP_EVENT_PLAYSTATE:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_PLAYSTATE \n");
		break;

	case eOxMP_EVENT_PLAYERROR:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_PLAYERROR \n");
		break;

	case eOxMP_EVENT_MEDIA_BufferStateChanged:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_MEDIA_BufferStateChanged \n");
		break;

	case eOxMP_EVENT_MEDIA_BufferInfoChanged:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_MEDIA_BufferInfoChanged \n");
		break;

	case eOxMP_EVENT_VIDEOSTREAMINFO:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_VIDEOSTREAMINFO \n");
		break;

	case eOxMP_EVENT_STOPPED:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_STOPPED \n");

		break;

	case eOxMP_EVENT_SPEED_CHANGED:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_SPEED_CHANGED \n");
		break;

	case eOxMP_EVENT_SupportedSpeedChanged:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_SupportedSpeedChanged \n");
		break;

	case eOxMP_EVENT_SI_SIT_CHANGED:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_SI_SIT_CHANGED \n");
		break;

	case eOxMP_EVENT_MEDIA_PumpIndexChanged:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_MEDIA_PumpIndexChanged \n");
		break;

	case eOxMP_EVENT_MEDIA_SeekReady:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_MEDIA_SeekReady \n");
		break;

	case eOxMP_EVENT_AudioOutputRestrict:
		HxLOG_Print("Received vod Message : eOxMP_EVENT_AudioOutputRestrict \n");
		break;
	default:
		return;
	}
}

static void 	nx_player_Received_MediaPlayEvent(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData)
{
	HINT32						lSpeed = 0;
	HFLOAT64					flSpeed = 0;
	OxMediaPlay_PlayError_e 	ePlayError;
	OxMediaPlay_ErrorReason_e 	eErrorReason;
	OxMediaPlay_BufferState_e 	eBufferState;

	switch ( eEvent )
	{
		case eOxMP_EVENT_STARTED:
			return;

		case eOxMP_EVENT_PLAYTIME:
			return;

		case eOxMP_EVENT_PLAYDURATION:
			return;

		case eOxMP_EVENT_PLAYSTATE:
			ONDK_GWM_PostMessage(NULL, MSG_CORE_PLAYER_PLAY_STATE_CHANGED, 0, pstNotifyData->eStateChanged, 0, 0);
			return;

		case eOxMP_EVENT_PLAYERROR:
			if (ERR_OK == NX_PORT_MEDIA_PLAY_GetPlayError(ulViewId, &ePlayError, &eErrorReason))
			{
				ONDK_GWM_PostMessage(NULL, MSG_CORE_PLAYER_PLAY_ERROR, 0, ePlayError, eErrorReason, 0);
			}
			return;

		case eOxMP_EVENT_MEDIA_BufferStateChanged:
			if (ERR_OK == NX_PORT_MEDIA_PLAY_GetBufferState(ulViewId, &eBufferState))
			{
				ONDK_GWM_PostMessage(NULL, MSG_CORE_PLAYER_PLAY_BUFFER_STATE_CHANGED, 0, eBufferState, 0, 0);
			}
			return;

		case eOxMP_EVENT_MEDIA_BufferInfoChanged:
			return;

		case eOxMP_EVENT_VIDEOSTREAMINFO:
			return;

		case eOxMP_EVENT_STOPPED:
			ONDK_GWM_PostMessage(NULL, MSG_CORE_PLAYER_PLAY_STATE_CHANGED, 0, (HINT32)eOxMP_PLAYSTATE_STOPPED, 0, 0);
			return;

		case eOxMP_EVENT_SPEED_CHANGED:
			if (ERR_OK == NX_PORT_MEDIA_PLAY_GetPlaySpeed(ulViewId, &lSpeed))
			{
				flSpeed = (double)(lSpeed / (double)100.0);	 //(-32/-16/-8/-4/-2/0/1/2/4/8/16/32)
				ONDK_GWM_PostMessage(NULL, MSG_CORE_PLAYER_PLAY_SPEED_CHANGED, 0, (HINT32)flSpeed, 0, 0);
			}
			return;

		case eOxMP_EVENT_SupportedSpeedChanged:
			ONDK_GWM_PostMessage(NULL, MSG_CORE_PLAYER_PLAY_SUPPORTED_PLAY_SPEED_CHANGED, 0, 0, 0, 0);
			return;

		case eOxMP_EVENT_SI_SIT_CHANGED:
			return;

		case eOxMP_EVENT_MEDIA_PumpIndexChanged:
			return;

		case eOxMP_EVENT_MEDIA_SeekReady:
			return;

		case eOxMP_EVENT_COMPONENT_SUBTITLE_CHANGED:
			ONDK_GWM_PostMessage(NULL, MSG_CORE_PLAYER_COMPONENT_SUBTITLE_CHANGED, 0, ePlayerType_MEDIAPLAY, 0, 0);
			return;

		default:
			return;
	}
}

/**************************************************************************************************/
#define ___________________Public_functions________________________________________________
/**************************************************************************************************/
HERROR	NX_PLAYER_Init(void)
{
	NxPlayerContext_t *pstNxVodContext = NULL;

	HxLOG_Error("Create NxPlayerContext_t.\n");
	pstNxVodContext	= nx_player_GetContext();

	nx_player_AllResetPlayerContent(pstNxVodContext);

#if defined(CONFIG_DEBUG)
	nx_player_Command_Init();
#endif

	return ERR_OK;
}

NxPlayerContentInform_t *NX_PLAYER_MakeContentPvrPlayback(HUINT32 ulViewId, HUINT32 ulContentId, HUINT32 ulStartTime)
{
	NxPlayerContentInform_t *pstPlayerContent = (NxPlayerContentInform_t*)NX_APP_Calloc(sizeof(NxPlayerContentInform_t));

	pstPlayerContent->ePlayerType = ePlayerType_PVRPLAYBACK;
	pstPlayerContent->ulViewId	= ulViewId;
	pstPlayerContent->ulSessionId = 0;
	pstPlayerContent->stStartInfo.stPvrPb.eType = eOxMP_MEDIATYPE_PVRPLAYBACK;
	pstPlayerContent->stStartInfo.stPvrPb.ulContentId = ulContentId;
	pstPlayerContent->stStartInfo.stPvrPb.ulStartTime = ulStartTime;

	return pstPlayerContent;
}

NxPlayerContentInform_t *NX_PLAYER_MakeContentMediaPlayback(HUINT32 ulViewId, HUINT32 ulStartTime, HCHAR *szUrl)
{
	NxPlayerContentInform_t *pstPlayerContent = (NxPlayerContentInform_t*)NX_APP_Calloc(sizeof(NxPlayerContentInform_t));

	pstPlayerContent->ePlayerType = ePlayerType_MEDIAPLAY;
	pstPlayerContent->ulViewId = ulViewId;
	pstPlayerContent->ulSessionId = 0;
	pstPlayerContent->stStartInfo.stMediaPlay.eType = eOxMP_MEDIATYPE_MEDIAPLAY;
	pstPlayerContent->stStartInfo.stMediaPlay.ulStartTime = ulStartTime;
	HxSTD_snprintf(pstPlayerContent->stStartInfo.stMediaPlay.szUrl, OxMP_MEDIAPLAY_URL, "%s", szUrl);

	return pstPlayerContent;
}

NxPlayerContentInform_t *NX_PLAYER_MakeContentTsrPlayback(HUINT32 ulViewId)
{
	NxPlayerContentInform_t *pstPlayerContent = (NxPlayerContentInform_t*)NX_APP_Calloc(sizeof(NxPlayerContentInform_t));

	pstPlayerContent->ePlayerType = ePlayerType_TSRPLAYBACK;
	pstPlayerContent->ulViewId = ulViewId;

	return pstPlayerContent;
}

HERROR	NX_PLAYER_SetContent(HINT32 nPlayerContentIdx, NxPlayerContentInform_t *pstContentInform)
{
	NxPlayerContext_t *pstNxPlayerContext = nx_player_GetContext();

	if (pstContentInform == NULL)
	{
		HxLOG_Error("pstContentInform is NULL!\n");
		return ERR_FAIL;
	}

	if (nPlayerContentIdx >= NX_PLAYER_SUPPORT_INST_NUM)
	{
		HxLOG_Error("over vod content idx!\n");
		return ERR_FAIL;
	}

	if (pstNxPlayerContext->pstPlayerInstance[nPlayerContentIdx] != NULL)
	{
		HxSTD_MemCopy(pstNxPlayerContext->pstPlayerInstance[nPlayerContentIdx], pstContentInform, sizeof(NxPlayerContentInform_t));
	}

	return ERR_OK;
}

HERROR	NX_PLAYER_Play(HINT32 nPlayerContentIdx)
{
	HERROR	hErr = ERR_FAIL;
	NxPlayerContext_t *pstNxPlayerContext = nx_player_GetContext();
	NxPlayerContentInform_t *pstPlayerContentInform = NULL;

	if (nPlayerContentIdx >= NX_PLAYER_SUPPORT_INST_NUM)
	{
		HxLOG_Error("over vod content idx!\n");
		return ERR_FAIL;
	}

	pstPlayerContentInform = pstNxPlayerContext->pstPlayerInstance[nPlayerContentIdx];

	if (pstPlayerContentInform == NULL || (pstPlayerContentInform->ePlayerType == ePlayerType_NONE))
	{
		HxLOG_Error("NxPlayerContentInform_t is NULL. or Vod Type is NONE.\n");
		return ERR_FAIL;
	}

	{
		#if 0
		char buf[1024] = {0,};
		#endif

		//NX_PORT_MEDIA_PLAY_SetVideoHide(0, FALSE);
		//NX_PORT_MEDIA_PLAY_SetBrowserUserAgentData(0, "Opera/9.80 (Linux mips; Opera TV Store/5954; HbbTV/1.2.1 (PVR; Humax; hms1000sph2; DESFAB 1.02.52; 1.0.0; wired; UX-PRISM--OP-NONE); ce-html/1.0) Presto/2.12.407 Version/12.50 Model/Humax-hms1000sph2");
		NX_PORT_MEDIA_PLAY_RegisterNotifier(0, nx_player_Notifier, NULL, NULL);

		nx_player_Probe(pstPlayerContentInform);

		#if 0
		NX_PORT_MEDIA_PLAY_SetDrmLicenseData(0, buf);

		NX_PORT_MEDIA_PLAY_SeBufferingScenario(0, false, 10000);

		NX_PORT_MEDIA_PLAY_Probe(0, pstVodContentInform->stStartInfo.stMediaPlay.szUrl);
		#endif
	}

	switch(pstPlayerContentInform->ePlayerType)
	{
	case ePlayerType_PVRPLAYBACK:
		hErr = NX_PORT_MEDIA_PLAY_Start(pstPlayerContentInform->ulViewId, &pstPlayerContentInform->stStartInfo, &pstPlayerContentInform->ulSessionId);
		if ( hErr != ERR_OK )
		{
			HxLOG_Error ("APK_MEDIA_PLAY_Start err: [ViewId=%u, SessionId=%u]\n",pstPlayerContentInform->ulViewId, pstPlayerContentInform->ulSessionId);
		}
		break;

	case ePlayerType_MEDIAPLAY:
		hErr = NX_PORT_MEDIA_PLAY_Start(pstPlayerContentInform->ulViewId, &pstPlayerContentInform->stStartInfo, &pstPlayerContentInform->ulSessionId);
		if ( hErr != ERR_OK )
		{
			HxLOG_Error ("APK_MEDIA_PLAY_Start err: [ViewId=%u, SessionId=%u]\n", pstPlayerContentInform->ulViewId, pstPlayerContentInform->ulSessionId);
		}

		NX_PORT_MEDIA_PLAY_SetPlaySpeed(0, 1);
		break;

	default:
		break;
	}

	return ERR_FAIL;
}

HERROR	NX_PLAYER_Stop(HINT32 nPlayerContentIdx)
{
	OxMediaPlay_MediaType_e eOapiType;
	NxPlayerContext_t *pstNxPlayerContext = nx_player_GetContext();
	NxPlayerContentInform_t *pstPlayerContentInform = NULL;

	if (nPlayerContentIdx >= NX_PLAYER_SUPPORT_INST_NUM)
	{
		HxLOG_Error("over vod content idx!\n");
		return ERR_FAIL;
	}

	pstPlayerContentInform = pstNxPlayerContext->pstPlayerInstance[nPlayerContentIdx];

	NX_PORT_MEDIA_PLAY_GetPlayType(pstPlayerContentInform->ulViewId, &eOapiType);
	if (eOapiType != eOxMP_MEDIATYPE_LIVE)
	{
		NX_PORT_MEDIA_PLAY_Stop(pstPlayerContentInform->ulViewId);
	}
	return ERR_OK;
}

HERROR	NX_PLAYER_SetPosition(NxPlayerContentInform_t *pstPlayerContent, HUINT32 ulPosition)
{
	HERROR	hRet = ERR_FAIL;
	if (pstPlayerContent == NULL)
	{
		HxLOG_Error("pstPlayerContent is NULL!\n");
		return ERR_FAIL;
	}

	switch(pstPlayerContent->ePlayerType)
	{
	case ePlayerType_LIVE:
		break;

	case ePlayerType_PVRPLAYBACK:
		nx_player_SetPositionMedia(pstPlayerContent, ulPosition);
		break;

	case ePlayerType_TSRPLAYBACK:
		nx_player_SetPositionTsr(pstPlayerContent, ulPosition);
		break;

	case ePlayerType_MEDIAPLAY:
		nx_player_SetPositionMedia(pstPlayerContent, ulPosition);
		break;

	default:
		break;
	}

	return hRet;
}

HERROR	NX_PLAYER_SetSpeed(NxPlayerContentInform_t *pstPlayerContent, HINT32 lSpeed)
{
	HERROR	hRet = ERR_FAIL;
	if (pstPlayerContent == NULL)
	{
		HxLOG_Error("pstPlayerContent is NULL!\n");
		return ERR_FAIL;
	}

	switch(pstPlayerContent->ePlayerType)
	{
	case ePlayerType_LIVE:
		break;

	case ePlayerType_PVRPLAYBACK:
		break;

	case ePlayerType_TSRPLAYBACK:
		break;

	case ePlayerType_MEDIAPLAY:
		nx_player_SetSpeedMedia(pstPlayerContent, lSpeed);
		break;

	default:
		break;
	}

	return hRet;
}

#define	______________RELATED_PLAYBACK_STATUS____________________
HERROR	NX_PLAYER_UpdatePlaybackOffsetData(HUINT32 ulViewId, NX_Channel_t *pstMasterChannel, NxPlayerContentInform_t *pstPlayerContent)
{
	HUINT32		ulTime = 0, ulRsvSlot = 0;
	HERROR		hErr = ERR_FAIL;
	OxMediaPlay_MediaType_e 	eType;
	DxService_t	*pstService = NULL;

	if ((pstMasterChannel == NULL) || (pstPlayerContent == NULL))
	{
		HxLOG_Error("pstMasterChannel is NULL or pstPlayerContent is NULL!\n");
		return ERR_FAIL;
	}

	pstService = NX_CHANNEL_FindServiceBySvcUid(pstMasterChannel->svcUid);

	if (pstService == NULL)
	{
		HxLOG_Error("Not Found DxService_t of MasterChannel.(uid : %u)\n", pstMasterChannel->svcUid);
		return ERR_FAIL;
	}

	hErr = APK_MEDIA_PLAY_GetPlayType(ulViewId, &eType);

	switch(eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
		{
			HBOOL bIsRecording = FALSE;
			APK_MEDIA_RECORD_IsRecording(pstService->uid, &bIsRecording);
			if (bIsRecording == TRUE)
			{
				ulTime = 0;
				if (ERR_OK == APK_MEDIA_RECORD_GetRsvSlotByUID(pstService->uid, &ulRsvSlot))
				{
					if (ERR_OK == APK_MEDIA_RECORD_GetRecTime(ulRsvSlot, &ulTime))
					{
						pstPlayerContent->ulPlaybackOffset = ulTime - pstPlayerContent->ulStartTime;
						pstPlayerContent->ulPlayTime = ulTime;
					}
				}
				else
				{
					pstPlayerContent->ulPlaybackOffset = 0;
				}
			}
			else
			{
				if (ERR_OK == NX_PORT_MEDIA_PLAY_GetTsrRecTime(ulViewId, &ulTime))
				{
					pstPlayerContent->ulPlaybackOffset = ulTime;
				}
			}
		}
		break;

	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		{
			HINT32	nPlayTime = 0;
			HUINT32	ulStartTime = 0;
			HBOOL bGetStartTime = FALSE;
			HBOOL bIsRecording = FALSE;
			HUINT32 ulDurationTime = 0;

			APK_MEDIA_RECORD_IsRecording(pstService->uid, &bIsRecording);
			if (bIsRecording == TRUE)
			{
				if (ERR_OK == APK_MEDIA_RECORD_GetRsvSlotByUID(pstService->uid, &ulRsvSlot))
				{
					if (ERR_OK == APK_MEDIA_RECORD_GetStartTime(ulRsvSlot, &ulTime))
					{
						pstPlayerContent->ulStartTime = ulTime;
						bGetStartTime = TRUE;
					}
				}
			}
			if (bGetStartTime == FALSE)
			{
				if (ERR_OK == APK_MEDIA_PLAY_GetTsrStartTime(ulViewId, &ulStartTime))
				{
					pstPlayerContent->ulStartTime = ulStartTime;
				}
			}

			if (ERR_OK == NX_PORT_MEDIA_PLAY_GetPlayPosition(ulViewId, &ulTime))
			{
				pstPlayerContent->ulPlayTime = ulTime;
			}
			else
			{
				pstPlayerContent->ulPlayTime++;
			}

			nPlayTime = pstPlayerContent->ulPlayTime - pstPlayerContent->ulStartTime;

			if (bIsRecording == TRUE)
			{
				APK_MEDIA_RECORD_GetRecTime(ulRsvSlot, &ulDurationTime);
			}
			else
			{
				NX_PORT_MEDIA_PLAY_GetDurationTime(ulViewId, &ulDurationTime);
			}

			if (nPlayTime > (HINT32)ulDurationTime)
				nPlayTime = (HINT32)ulDurationTime;
			else if (nPlayTime < 1)
			{
				// nTsrPlaytime, nTsrStarttime 가 바로 바로 update 가 되지 않아 값이 역전되는 현상 발생
				// nPlayTime 은 0 보다 커야
				nPlayTime = 1;
			}
			pstPlayerContent->ulPlaybackOffset = nPlayTime;
		}
		break;

	default:
		pstPlayerContent->ulPlaybackOffset = 0;
		break;
	}

	return ERR_OK;
}

// 현재 buffering 된 time
HUINT32	NX_PLAYER_GetPlayMaxTime(HUINT32 ulViewId, NX_Channel_t *pstMasterChannel)
{
	HUINT32	ulRetMaxOffsetTime = 0;
	HUINT32	ulRsvSlot = 0;
	HBOOL	bIsRecording = FALSE;

	if (ERR_FAIL == APK_MEDIA_RECORD_IsRecording(pstMasterChannel->svcUid, &bIsRecording))
	{
		HxLOG_Error("APK_MEDIA_RECORD_IsRecording Func Error!\n");
		return ulRetMaxOffsetTime;
	}

	if (TRUE == bIsRecording)
	{
		if (ERR_FAIL == APK_MEDIA_RECORD_GetRsvSlotByUID(pstMasterChannel->svcUid, &ulRsvSlot))
		{
			HxLOG_Error("APK_MEDIA_RECORD_GetRsvSlotByUID Func Error!\n");
			return ulRetMaxOffsetTime;
		}

		if (ERR_FAIL == APK_MEDIA_RECORD_GetRecTime(ulRsvSlot, &ulRetMaxOffsetTime))
		{
			HxLOG_Error("APK_MEDIA_RECORD_GetRecTime Func Error!\n");
			return 0;
		}
	}
	else
	{
		if (ERR_FAIL == NX_PORT_MEDIA_PLAY_GetTsrRecTime(ulViewId, &ulRetMaxOffsetTime))
		{
			HxLOG_Error("NX_PORT_MEDIA_PLAY_GetTsrRecTime Func Error!\n");
			return 0;
		}
	}

	return ulRetMaxOffsetTime;
}


#define ______________RELATED_PLAYER_COMMAND____________________
/***************************************************************************
Related Search Command
***************************************************************************/
#if defined(CONFIG_DEBUG)

static HINT32 cmdPlayerPlay(void *szArgs)
{
	HCHAR	*aucArg=NULL;
	HCHAR	szUrl[512] = {'\0',}, szFullUrl[512] = {'\0',};

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs );
	if( aucArg != NULL )
	{
		NxPlayerContentInform_t * pPlayerContent = NULL;
		if( sscanf(aucArg, "%s", (char *)&szUrl) == 0 ) return HxCMD_ERR;
		HxLOG_Print("szUrl (%s)\n", szUrl);

		HxSTD_snprintf(szFullUrl, 512, "file://media/drive1/Video/Movie Test Files/%s", szUrl);

		pPlayerContent = NX_PLAYER_MakeContentMediaPlayback(0, 0, "file://media/drive1/Video/Movie Test Files/[01_800X416] Jurassic.Park III.2001.XviD,AC3 5.1CH.avi");
						//"file://media/drive1/Video/Movie Test Files/[02_1280X720] true tears 01.avi");

		NX_PLAYER_SetContent(NX_PLAYER_INST_01, pPlayerContent);

		NX_APP_Free(pPlayerContent);

		NX_PLAYER_Play(NX_PLAYER_INST_01);
	}
	else
	{
		HxLOG_Print("Enter URL Plz....\n");
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

static HINT32 cmdPlayerStop(void *szArgs)
{
	NX_PLAYER_Stop(NX_PLAYER_INST_01);

	return HxCMD_OK;
}

static void nx_player_Command_Init(void)
{
#define cmdHandle		"nxplayercmd"

	HLIB_CMD_RegisterWord(cmdHandle, cmdPlayerPlay,
						 (HINT8 *)"play",
						 (HINT8 *)"param1(url)",
						 (HINT8 *)"play");
	HLIB_CMD_RegisterWord(cmdHandle, cmdPlayerStop,
						 (HINT8 *)"stop",
						 (HINT8 *)"stop",
						 (HINT8 *)"stop");
}

#endif		// #if defined(CONFIG_DEBUG)















