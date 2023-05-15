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
 * @file	  		nx_core_av.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_core_av.h>


/**************************************************************************************************/
#define ___________________Private_variable________________________________________________
/**************************************************************************************************/
static NX_AvContext_t	*s_AvContext	=	NULL;

/**************************************************************************************************/
#define ___________________Private_typedef________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ___________________Private_prototype________________________________________________
/**************************************************************************************************/
static NX_AvContext_t* GetAvContext(void);
static void nx_av_notifyFunc ( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData );
static void nx_av_live_proc(unsigned int ulViewId, unsigned int session, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData);
static void nx_av_tsr_proc(unsigned int ulViewId, unsigned int session, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData);

/**************************************************************************************************/
#define ___________________Private_functions________________________________________________
/**************************************************************************************************/
static NX_AvContext_t* GetAvContext(void)
{
	if (s_AvContext == NULL)
	{
		s_AvContext = NX_APP_Calloc(sizeof(NX_AvContext_t));
	}
	return s_AvContext;
}

static void nx_av_notifyFunc ( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
	switch( eType )
	{
		case eOxMP_MEDIATYPE_LIVE:
		{
			nx_av_live_proc(ulViewId, ulSessionId, eType, eEvent, pstNotifyData, pvUserData);
			break;
		}
		case eOxMP_MEDIATYPE_TSRPLAYBACK:
		{
			// Chase를 PVR PLAYBACK으로 간주하면...여기도..
			nx_av_tsr_proc(ulViewId, ulSessionId, eType, eEvent, pstNotifyData, pvUserData);
			break;
		}
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		default :
			break;
	}
}

static void		nx_av_live_proc(unsigned int ulViewId, unsigned int session, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HINT32		nTrackIndex = 0;
	HUINT32		ulSvcUid = 0, ulParam1 = 0, ulParam2 = 0, ulParam3 = 0, ulMainViewId = 0;
	NX_AvContext_t	*pstAvContext = GetAvContext();


	ulParam1 = ((ulViewId & 0x000F) << 24) | ((eType & 0x000F) << 16) | (eEvent & 0x00FF);
	if (NULL != pstNotifyData)
	{
		ulParam2 = pstNotifyData->stEventNotify.ulParam1;
		ulParam3 = pstNotifyData->stEventNotify.ulParam2;
	}

	// oplbroadcast 내부의 status 관련 처리 :
	switch (eEvent)
	{
		case eOxMP_EVENT_LIVE_SvcChanged:
			{
				pstAvContext->stVideoRes.eTunningState	= eNxTunningState_INITIALIZED;
				pstAvContext->stVideoRes.ePlayState		= eNxPlayState_CONNECTING;
				pstAvContext->stVideoRes.eViewState		= eOxMP_LiveViewState_NONE;
				pstAvContext->stVideoRes.eLockState		= eOxMP_LiveLockState_NONE;

				NX_PORT_MEDIA_PLAY_GetMainViewId(&ulMainViewId);
				if (ulMainViewId == ulViewId)
				{
					NX_PORT_MEDIA_PLAY_GetRequestId (ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&ulSvcUid);
					if (ulSvcUid > 0)
					{
						NX_CHANNEL_SaveLastChannel(ulSvcUid);
					}
				}
			}
			break;

		case eOxMP_EVENT_SI_EIT_CHANGED:
			break;
		case eOxMP_EVENT_LIVE_EventRelay:
			break;
		case eOxMP_EVENT_LIVE_Ews:
			break;
		case eOxMP_EVENT_PinCtrlChanged:
		case eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS:
			break;

		case eOxMP_EVENT_TrickRestrictModeChanged:
			break;

		case eOxMP_EVENT_COMPONENT_SELECTED:
			{
				if (eOxMP_COMPONENT_SUBTITLE == ulParam1)
				{
					nTrackIndex	= ulParam2;
					if (nTrackIndex >= 0)
					{
						OxMediaPlay_Component_t	stComponent;

						if (NX_PORT_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_SUBTITLE, nTrackIndex, &stComponent) == ERR_OK)
						{
							HAPI_BroadcastSignal("signal:onSubtitleTypeChanged", TRUE, "i", stComponent.stSubtitle.eSubtitleType);
							HAPI_BroadcastSignal("signal:onSubtitleState", TRUE, "i", 1);
						}
					}
				}
			}
			break;
		default:
			break;
	}

	/*
	내부 state 관리가 필요해지는 시기가 오면, 여기서 추가 구현해 넣자.
	msg 는 내부 ondk msg 처리로 send 하도록 한다.
	*/


	switch(eEvent)
	{
	case eOxMP_EVENT_STARTED:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_STARTED)\n");
		break;
	case eOxMP_EVENT_COMPONENT_AUDIO_CHANGED:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_COMPONENT_AUDIO_CHANGED)\n");
		break;
	case eOxMP_EVENT_LIVE_SvcChanged:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_LIVE_SvcChanged)\n");
		break;
	case eOxMP_EVENT_SI_EIT_CHANGED:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_SI_EIT_CHANGED)\n");
		break;
	case eOxMP_EVENT_SI_RCT_CHANGED:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_SI_RCT_CHANGED)\n");
		break;
	case eOxMP_EVENT_LIVE_EventRelay:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_LIVE_EventRelay)\n");
		break;
	case eOxMP_EVENT_LIVE_Ews:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_LIVE_Ews)\n");
		break;
	case eOxMP_EVENT_PinCtrlChanged:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_PinCtrlChanged)\n");
		break;
	case eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS)\n");
		break;
	case eOxMP_EVENT_TUNE_Locked:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_TUNE_Locked)\n");
		HxLOG_Info("=============== eOxMP_EVENT_TUNE_Locked ===========\n");
		HxLOG_Info("======== ulViewId   : 0x%x ========================\n", ulViewId);
		HxLOG_Info("======== eType      : %d ==========================\n", eType);
		if(pstAvContext->stVideoRes.eTunningState == eNxTunningState_INITIALIZED)
		{
			pstAvContext->stVideoRes.eTunningState = eNxTunningState_STARTED;

			if ((pstAvContext->stVideoRes.eViewState == eOxMP_LiveViewState_NONE)
				&& (pstAvContext->stVideoRes.eLockState == eOxMP_LiveLockState_NONE))
			{
				pstAvContext->stVideoRes.ePlayState		= eNxPlayState_CONNECTING;
			}
			else
			if ((pstAvContext->stVideoRes.eViewState == eOxMP_LiveViewState_OK)
				&& (pstAvContext->stVideoRes.eLockState == eOxMP_LiveLockState_OK))
			{
				pstAvContext->stVideoRes.ePlayState		= eNxPlayState_PRESENTING;
			}
			else
			{
				pstAvContext->stVideoRes.ePlayState		= eNxPlayState_CONNECTING;
			}
		}
		else
		{
			if (pstAvContext->stVideoRes.ePlayState != eNxPlayState_PRESENTING)
			{
				pstAvContext->stVideoRes.ePlayState = eNxPlayState_PRESENTING;
			}
		}

		if (NULL != pstNotifyData)
		{
			HxLOG_Info("======== Servic UID : 0x%x ========================\n", pstNotifyData->stEventNotify.ulParam1);
		}
		HxLOG_Info("===================================================\n");
		break;
	case eOxMP_EVENT_TUNE_NoSignal:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_TUNE_NoSignal)\n");
		if (pstAvContext->stVideoRes.eTunningState == eNxTunningState_INITIALIZED)
		{
			pstAvContext->stVideoRes.eTunningState = eNxTunningState_STARTED;
		}
		break;
	case eOxMP_EVENT_TUNE_AntennaNok:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_TUNE_AntennaNok)\n");
		break;
	case eOxMP_EVENT_LIVE_ViewStateChanged:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_LIVE_ViewStateChanged)\n");
		HxLOG_Info("===== eOxMP_EVENT_LIVE_ViewStateChanged - ulParam1:(%u), ulParam2:(%u), ulParam3:(%u) \n",
				pstNotifyData->stEventNotify.ulParam1, pstNotifyData->stEventNotify.ulParam2, pstNotifyData->stEventNotify.ulParam3);
		break;
	case eOxMP_EVENT_LIVE_LockStateChanged:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_LIVE_LockStateChanged)\n");
		HxLOG_Info("===== eOxMP_EVENT_LIVE_LockStateChanged - ulParam1:(%u), ulParam2:(%u), ulParam3:(%u) \n",
				pstNotifyData->stEventNotify.ulParam1, pstNotifyData->stEventNotify.ulParam2, pstNotifyData->stEventNotify.ulParam3);
		break;
	case eOxMP_EVENT_LIVE_SvcStopped:
		HxLOG_Debug("Received MSG (eOxMP_EVENT_LIVE_SvcStopped)\n");
		if (pstAvContext->stVideoRes.ePlayState != eNxPlayState_STOPPED)
		{
			pstAvContext->stVideoRes.ePlayState = eNxPlayState_STOPPED;
		}
		if (pstAvContext->stVideoRes.eTunningState == eNxTunningState_INITIALIZED)
		{
			pstAvContext->stVideoRes.eTunningState = eNxTunningState_STARTED;
		}
		break;
	default:
		break;
	}

	NX_COMMON_NOT_USED_PARAM(session);
	NX_COMMON_NOT_USED_PARAM(pvUserData);

	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_MEDIA, ulParam1, ulParam2, ulParam3);
}

static void nx_av_tsr_proc(unsigned int ulViewId, unsigned int session, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HUINT32 ulParam1	=	0;
	HUINT32 ulParam2	=	0;
	HUINT32 ulParam3	=	0;

	ulParam1	=	((ulViewId & 0x000F) << 24) | ((eType & 0x000F) << 16) | (eEvent & 0x00FF);

	if (NULL != pstNotifyData)
	{
		ulParam2	=	pstNotifyData->stEventNotify.ulParam1;
		ulParam3	=	pstNotifyData->stEventNotify.ulParam2;
	}

	switch (eEvent)
	{
	case eOxMP_EVENT_PLAYSTATE:
		break;
	case eOxMP_EVENT_STARTED:
		break;
	case eOxMP_EVENT_STOPPED:
		break;
	case eOxMP_EVENT_BOF:
		break;
	case eOxMP_EVENT_EOF:
		break;
	case eOxMP_EVENT_SPEED_CHANGED:
		break;
	case eOxMP_EVENT_PLAYTIME:
		break;
	case eOxMP_EVENT_PinCtrlChanged:
		break;
	case eOxMP_EVENT_DRM_BLOCK:
		break;
	case eOxMP_EVENT_SupportedSpeedChanged:
		break;
	case eOxMP_EVENT_TrickRestrictModeChanged:
		break;
	case eOxMP_EVENT_PLAYERROR:
		break;
	case eOxMP_EVENT_COMPONENT_SELECTED:
		{
			if (eOxMP_COMPONENT_SUBTITLE == ulParam1)
			{
				HINT32	nTrackIndex = ulParam2;

				if (nTrackIndex >= 0)
				{
					OxMediaPlay_Component_t stComponent;

					if (NX_PORT_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_SUBTITLE, nTrackIndex, &stComponent) == ERR_OK)
					{
						HAPI_BroadcastSignal("signal:onSubtitleTypeChanged", TRUE, "i", stComponent.stSubtitle.eSubtitleType);
						HAPI_BroadcastSignal("signal:onSubtitleState", TRUE, "i", 1);
					}
				}
			}
		}
		break;

	default:
		break;
	}


	/*
	내부 state 관리가 필요해지는 시기가 오면, 여기서 추가 구현해 넣자.
	msg 는 내부 ondk msg 처리로 send 하도록 한다.
	*/

	switch(eEvent)
	{
	case eOxMP_EVENT_PinCtrlChanged:
		break;
	case eOxMP_EVENT_DRM_BLOCK:
		break;
	case eOxMP_EVENT_PLAYERROR:
		break;

	default:
		break;
	}


	NX_COMMON_NOT_USED_PARAM(session);
	NX_COMMON_NOT_USED_PARAM(pvUserData);

	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_MEDIA, ulParam1, ulParam2, ulParam3);
}


/**************************************************************************************************/
#define ___________________Public_functions________________________________________________
/**************************************************************************************************/
HERROR NX_AV_Init(void)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 	ulViewId;
	NX_AvContext_t	*pstAvContext = GetAvContext();

	pstAvContext->stVideoRes.ulVideoResourceId	=	0;		// resource 어케하나...?
	pstAvContext->stVideoRes.eTunningState		= 	eNxTunningState_INITIALIZED;
	pstAvContext->stVideoRes.ePlayState			=	eNxPlayState_UNREALIZED;
	pstAvContext->stVideoRes.eViewState			=	eOxMP_LiveViewState_NONE;
	pstAvContext->stVideoRes.eLockState			=	eOxMP_LiveLockState_NONE;

	ulViewId = pstAvContext->stVideoRes.ulVideoResourceId;

	hErr = NX_PORT_MEDIA_PLAY_RegisterNotifier(ulViewId, nx_av_notifyFunc, NULL, NULL);
	if( ERR_OK != hErr )
		HxLOG_Error("[%s:%d] NX_PORT_MEDIA_PLAY_RegisterNotifier Error: \n", __FUNCTION__, __LINE__);

	return ERR_OK;
}

HUINT32	NX_AV_GetVideoResourceId(void)
{
	NX_AvContext_t	*pstAvContext = GetAvContext();

	return pstAvContext->stVideoRes.ulVideoResourceId;
}

NX_VideoResource_t *NX_AV_GetVideoResInform(HUINT32 viewId)
{
	NX_AvContext_t	*pstAvContext = GetAvContext();

	return &pstAvContext->stVideoRes;
}


HERROR		NX_AV_StopChannel(void)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				ulViewId = 0;
	OxMediaPlay_MediaType_e	eOapiType;

	ulViewId = NX_AV_GetVideoResourceId();
	NX_PORT_MEDIA_PLAY_GetPlayType(ulViewId, &eOapiType);

	switch(eOapiType)
	{
		case eOxMP_MEDIATYPE_LIVE:
			hError = NX_PORT_MEDIA_PLAY_Stop (ulViewId);
			break;
		default:
			NX_APP_Error("[%s:%d] Error!!! Invalid PlayType[%d]\n", __FUNCTION__, __LINE__, eOapiType);
			break;
	}

	if (hError != ERR_OK)
	{
		HxLOG_Error ("Cannot stop the service : ulViewId=0x%08x\n", ulViewId);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

