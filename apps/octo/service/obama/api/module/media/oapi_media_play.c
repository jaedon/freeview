/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_mediaplay.c
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <oapi.h>
#include <obama_int.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#define OAPI_MP_SEM_GET			HxSEMT_Get		(s_ulOxMpSema)
#define OAPI_MP_SEM_RELEASE		HxSEMT_Release	(s_ulOxMpSema)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eOxMP_MHEG_PAT,
	eOxMP_MHEG_PMT,
	eOxMP_MHEG_EndOfCase
} OxMediaPlay_MHEG_TableType_e;


typedef struct
{
	/*APPKIT Identity*/
	HUINT32					 ulMpAppId;
	HUINT32					 ulSessionId;

	HxList_t				*pstListener;			// oapiMpNotifier_t

	/*OM Identity*/
	HUINT32					 ulViewId;
	HBOOL					bReadyAitListener;

	/*CACHE*/
	OxMediaPlay_Cache_t		stCache;

} oapiMpContext_t;


typedef struct
{
	OxMediaPlay_Notifier_t	 pfListener;
	void					*pvUserData;
} oapiMpNotifier_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL				 s_bOapiMp_Inited = FALSE;
STATIC HUINT32				 s_ulOapiMp_MainViewId = 0, s_ulOapiMp_ViewNum = 0;
STATIC oapiMpContext_t		*s_pstOapiMp_Context = NULL;
STATIC OxMediaPlay_ActivatedCb_t s_pfActivated	=	NULL;
STATIC HUINT32				 s_ulOxMpSema = 0;
STATIC HINT32				 s_nRpcHandle = 0;

static HBOOL s_bGetResource = FALSE;
static OxMediaPlay_ResourceMap_t *s_pstResourceMap = NULL;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
#if defined(CONFIG_DEBUG)
STATIC HERROR oapi_media_play_printResourceStatus (OxMediaPlay_ConflictInfo_t *pstLiveConfInfo)
{
	HUINT32			 ulIndex;

	HxLOG_Debug ("[OAPI]\n\n\n\n");
	HxLOG_Debug ("[RSCSTATUS] RF-Input Number = %d\n", pstLiveConfInfo->ulRfInfoNum);
	for (ulIndex = 0; ulIndex < pstLiveConfInfo->ulRfInfoNum; ulIndex++)
	{
		HxLOG_Debug ("[RSCSTATUS] RF-Input %d : Available(%d), Allocated(%d), Freq(%d)\n", ulIndex,
						pstLiveConfInfo->astRfInfo[ulIndex].bAvailable,
						pstLiveConfInfo->astRfInfo[ulIndex].bAllocated,
						pstLiveConfInfo->astRfInfo[ulIndex].unTuneInfo.ter.ulFrequency);

	}

	HxLOG_Debug ("\n");
	HxLOG_Debug ("[RSCSTATUS] Tuner Number = %d\n", pstLiveConfInfo->ulTunerInfoNum);
	for (ulIndex = 0; ulIndex < pstLiveConfInfo->ulTunerInfoNum; ulIndex++)
	{
		HxLOG_Debug ("[RSCSTATUS] Tuner %d : Available(%d), Allocated(%d), Freq(%d)\n", ulIndex,
						pstLiveConfInfo->astTunerInfo[ulIndex].bAvailable,
						pstLiveConfInfo->astTunerInfo[ulIndex].bAllocated,
						pstLiveConfInfo->astTunerInfo[ulIndex].unTuneInfo.ter.ulFrequency);

	}

	HxLOG_Debug ("[OAPI]\n\n\n\n");
	return ERR_OK;
}
#endif

STATIC void oapi_mp_CleanAitSectionCache(oapiMpContext_t *pstContext)
{
	HUINT32			 i;
	HUINT8			*pucSec = NULL;

	for (i = 0 ; i < OxMP_AIT_SECTION_NUM_MAX ; i++)
	{
		pucSec = pstContext->stCache.stAitSecData.paucAitRawSec[i];
		if (pucSec != NULL)
		{
			HLIB_STD_MemFree(pucSec);
		}
	}

	HxSTD_MemSet(&(pstContext->stCache.stAitSecData),			0, sizeof(OxMediaPlay_AitSectionData_t) );
	return ;
}

STATIC void oapi_mp_CleanRctDataCache(oapiMpContext_t *pstContext)
{
	OxMediaPlay_RctData_t	*pstRctData = NULL;

	pstRctData = &(pstContext->stCache.stRctData);

	if(pstRctData != NULL)
	{
		DxService_RctInfo_t				*pstRctInfo = NULL;
		DxService_RctImageIconInfo_t	*pstRctImageInfo = NULL;

		pstRctInfo = &(pstRctData->stRctInfo);

		if(pstRctInfo->pstLinkInfoList != NULL)
		{
			HLIB_STD_MemFree(pstRctInfo->pstLinkInfoList );
			pstRctInfo->pstLinkInfoList = NULL;
		}

		pstRctImageInfo =&(pstRctInfo->stImageIconInfo);
		if(pstRctImageInfo->pucDatabyte !=NULL)
		{
			HLIB_STD_MemFree(pstRctImageInfo->pucDatabyte);
			pstRctImageInfo->pucDatabyte = NULL;
		}

		HxSTD_MemSet(pstRctData,	0, sizeof(OxMediaPlay_RctData_t) );
	}

	return ;
}

STATIC void oapi_mp_CleanMhegSectionCache(oapiMpContext_t *pstContext)
{
	HUINT32			 i;
	HUINT8			*pucSec = NULL;

	for (i = 0 ; i < OxMP_PAT_SECTION_NUM_MAX ; i++)
	{
		pucSec = pstContext->stCache.stMhegSecData.paucPatRawSec[i];
		if (pucSec != NULL)
		{
			HLIB_STD_MemFree(pucSec);
		}
	}

	for (i = 0 ; i < OxMP_PMT_SECTION_NUM_MAX ; i++)
	{
		pucSec = pstContext->stCache.stMhegSecData.paucPmtRawSec[i];
		if (pucSec != NULL)
		{
			HLIB_STD_MemFree(pucSec);
		}
	}

	HxSTD_MemSet(&(pstContext->stCache.stMhegSecData),			0, sizeof(OxMediaPlay_MhegSectionData_t) );
	return ;
}

STATIC void oapi_mp_CleanCache(oapiMpContext_t *pstContext, OxMediaPlay_MediaType_e eMediaType)
{
	if( NULL == pstContext )
		return ;

	switch(eMediaType)
	{
		case eOxMP_MEDIATYPE_LIVE:
		{
			HxSTD_MemSet(&(pstContext->stCache.stLive), 0, sizeof(OxMediaPlay_StartLive_t) );
//			pstContext->stCache.stLive.ulMasterSvcUid	=	(HUINT32)-1;
			break;
		}
		case eOxMP_MEDIATYPE_PVRPLAYBACK:	HxSTD_MemSet(&(pstContext->stCache.stPvrPb),		0, sizeof(OxMediaPlay_StartPvrPb_t) );	break;
		case eOxMP_MEDIATYPE_TSRPLAYBACK:	HxSTD_MemSet(&(pstContext->stCache.stTsrPb),		0, sizeof(OxMediaPlay_StartTsrPb_t) );	break;
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		{
			/* clean cache */
			HxSTD_MemSet(&(pstContext->stCache.stMediaPlay),			0, sizeof(OxMediaPlay_StartMediaPlay_t) );
			HxSTD_MemSet(&(pstContext->stCache.stMediaSutbitleStatus),	0, sizeof(OxMediaPlay_SubtitleStatus_t) );
			HxSTD_MemSet(&(pstContext->stCache.stBufferInfo),			0, sizeof(OxMediaPlay_BufferInfo_t) );
			pstContext->stCache.eBufferState			=	eOxMP_BUFFERSTATE_None;
			pstContext->stCache.nSubtitle_SyncTime		=	0;
			pstContext->stCache.nSubtitle_FontSize		=	0;
			pstContext->stCache.nSubtitle_TextPosition	=	0;
			pstContext->stCache.ulPumpId				=	0;
			pstContext->stCache.unAudioDecoderVolume	=	0;

			break;
		}
		default:
			break;
	}

	HxSTD_MemSet(&(pstContext->stCache.stVideoCompTable),		0, sizeof(OxMediaPlay_VideoCompTable_t) );
	HxSTD_MemSet(&(pstContext->stCache.stAudioCompTable),		0, sizeof(OxMediaPlay_AudioCompTable_t) );
	HxSTD_MemSet(&(pstContext->stCache.stSubtitleCompTable),	0, sizeof(OxMediaPlay_SubtitleCompTable_t) );
	HxSTD_MemSet(&(pstContext->stCache.stDataCompTable),		0, sizeof(OxMediaPlay_DataCompTable_t) );

	oapi_mp_CleanAitSectionCache(pstContext);
	oapi_mp_CleanRctDataCache(pstContext);
	oapi_mp_CleanMhegSectionCache(pstContext);

	pstContext->stCache.eType	=	eOxMP_MEDIATYPE_NONE;

	pstContext->stCache.stTsr.ulTsrStartTime		=	0;
	//pstContext->stCache.ulDurationTime		=	0;	//live <-> TSR 전환시 유지 필요
	pstContext->stCache.ulBufferRemaindTime	=	0;
	pstContext->stCache.ulBufferPercent		=	0;
	pstContext->stCache.ulBufferred			=	0;
	pstContext->stCache.ulBitrate			= 	0;

	return ;
}

STATIC HCHAR *oapi_mp_GetEventString (OxMediaPlay_Event_e eEvent)
{
	switch (eEvent)
	{
		ENUM_TO_STR (eOxMP_EVENT_TUNE_Locked)
		ENUM_TO_STR (eOxMP_EVENT_TUNE_NoSignal)

		ENUM_TO_STR (eOxMP_EVENT_TUNE_AntennaOk)
		ENUM_TO_STR (eOxMP_EVENT_TUNE_AntennaNok)

		ENUM_TO_STR (eOxMP_EVENT_LIVE_SvcChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_SvcStopped)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_ConflictChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_EventRelay)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_Ews)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_TSRTimecodeChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_BsCtrlStarted)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_ViewStateChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_LockStateChanged)

		ENUM_TO_STR (eOxMP_EVENT_MEDIA_ProbeErrored)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_StopErrored)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_BufferStateChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_BufferInfoChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_PlayInfo)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_PumpIndexChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_SeekReady)
		ENUM_TO_STR	(eOxMP_EVENT_MEDIA_AudioCodecSwitched)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_AudioFlushCompleted)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_AudioRestartCompleted)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_AudioFadeCompleted)
		ENUM_TO_STR	(eOxMP_EVENT_MEDIA_AudioFadeFailed)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_AudioDecoderVolumeChanged)

		ENUM_TO_STR (eOxMP_EVENT_BOF)
		ENUM_TO_STR (eOxMP_EVENT_EOF)
		ENUM_TO_STR (eOxMP_EVENT_STARTED)
		ENUM_TO_STR (eOxMP_EVENT_STOPPED)
		ENUM_TO_STR (eOxMP_EVENT_PLAYING)
		ENUM_TO_STR (eOxMP_EVENT_PAUSE)
		ENUM_TO_STR (eOxMP_EVENT_TRICK)
		ENUM_TO_STR (eOxMP_EVENT_RATING_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_AV_STARTED)
		ENUM_TO_STR (eOxMP_EVENT_AV_STOPPED)
		ENUM_TO_STR (eOxMP_EVENT_SPEED_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_AV_MEDIARECT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SupportedSpeedChanged)
		ENUM_TO_STR (eOxMP_EVENT_VIDEOSTREAMINFO)
		ENUM_TO_STR (eOxMP_EVENT_AUDIOCLIPCOMPLETED)
		ENUM_TO_STR (eOxMP_EVENT_PinCtrlChanged)

		ENUM_TO_STR (eOxMP_EVENT_SI_PMT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_EIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_SIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_AIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_RCT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_NETWORK_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_NETWORK_URI_CHANGED)

		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_VIDEO_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_AUDIO_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_SUBTITLE_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_SELECTED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_UNSELECTED)

		ENUM_TO_STR (eOxMP_EVENT_DRM_BLOCK)

		ENUM_TO_STR (eOxMP_EVENT_RESOURCE_TAKEN)
		ENUM_TO_STR (eOxMP_EVENT_RESOURCE_LOST)

		ENUM_TO_STR (eOxMP_EVENT_PLAYERROR)
		ENUM_TO_STR (eOxMP_EVENT_PLAYSTATE)
		ENUM_TO_STR (eOxMP_EVENT_PLAYTIME)

		ENUM_TO_STR (eOxMP_EVENT_RADIOBG_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_TTX_UPDATE)
		ENUM_TO_STR (eOxMP_EVENT_SBTL_UPDATE)


		ENUM_TO_STR (eOxMP_EVENT_TrickRestrictModeChanged)


	default:
		break;
	}

	return "Unknown";
}

STATIC oapiMpContext_t *oapi_mp_GetContext (HUINT32 ulViewId)
{
	if ((ulViewId < s_ulOapiMp_ViewNum) && (NULL != s_pstOapiMp_Context))
	{
		return &(s_pstOapiMp_Context[ulViewId]);
	}

	return NULL;
}

STATIC HUINT32 oapi_mp_GetSessionId (oapiMpContext_t *pstContext)
{
	return (((pstContext->ulMpAppId & 0xFF) << 24) |
			((pstContext->ulViewId & 0xFF) << 16) |
			(pstContext->ulSessionId & 0xFFFF));
}

STATIC void oapi_mp_GetAppIdViewIdSessionVer (HUINT32 ulSessionId, HUINT32 *pulMpAppId, HUINT32 *pulViewId, HUINT32 *pulSessionId)
{
	if (NULL != pulMpAppId)							{ *pulMpAppId = (ulSessionId >> 24) & 0xFF; }
	if (NULL != pulViewId)							{ *pulViewId = (ulSessionId >> 16) & 0xFF; }
	if (NULL != pulSessionId)						{ *pulSessionId = ulSessionId & 0xFFFF; }
}

STATIC void oapi_mp_IncreaseSessionVer (oapiMpContext_t *pstContext)
{
	pstContext->ulSessionId = (pstContext->ulSessionId >= 0xFFEF) ? 0x0001 : (pstContext->ulSessionId + 1);
}

STATIC void oapi_mp_NotifyToListener (oapiMpContext_t *pstContext, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstData, HBOOL bBlockOldVer)
{
	HxList_t			*pstListItem;

	if (TRUE == bBlockOldVer)
	{
		HUINT32 			 ulMpAppId = pstContext->ulMpAppId;
		HUINT32 			 ulMsgSessionId = pstContext->ulSessionId;

		oapi_mp_GetAppIdViewIdSessionVer (ulMsgSessionId, &ulMpAppId, NULL, &ulMsgSessionId);

		if ((ulMpAppId == pstContext->ulMpAppId) &&
			(ulMsgSessionId != pstContext->ulSessionId))
		{
			HxLOG_Error ("[%s:%d] Event (%d) blocked: Context SessionId(0x%08x), MSG SessionId(0x%08x)\n",
																	__FUNCTION__, __LINE__,
																	oapi_mp_GetEventString (eEvent),
																	oapi_mp_GetSessionId (pstContext), ulMsgSessionId);
			return;
		}
	}

	for (pstListItem = pstContext->pstListener; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiMpNotifier_t			*pstNotifier = (oapiMpNotifier_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstNotifier) && (NULL != pstNotifier->pfListener))
		{
			HxLOG_Debug("[MediaType:%d] EVENT : (%s)\n", eType, oapi_mp_GetEventString(eEvent));
			(pstNotifier->pfListener) (pstContext->ulViewId, ulSessionId, eType, eEvent, pstData, pstNotifier->pvUserData);
		}
	}
}

STATIC oapiMpNotifier_t *oapi_mp_FindNotifier (oapiMpContext_t *pstContext, OxMediaPlay_Notifier_t pfListener, void *pvUserData)
{
	HxList_t			*pstListItem;

	for (pstListItem = pstContext->pstListener; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiMpNotifier_t			*pstNotifier = (oapiMpNotifier_t *)HLIB_LIST_Data (pstListItem);

		if ((pfListener == pstNotifier->pfListener) &&
			(pvUserData == pstNotifier->pvUserData))
		{
			return pstNotifier;
		}
	}

	return NULL;
}

#ifdef	CONFIG_DEBUG
STATIC void	oapi_mp_PrintComponent(OxMediaPlay_Component_e eType, OxMediaPlay_Component_t *pstComponent)
{
	if (pstComponent == NULL)
		return;

	switch (eType)
	{
		case eOxMP_COMPONENT_SUBTITLE:
			{
				switch (pstComponent->stSubtitle.eSubtitleType)
				{
					case eOxMP_SUBTITLETYPE_Dvb:
						HxLOG_Debug("\tDVB : PID[0x%x] LangCode[%s]\n", pstComponent->stSubtitle.nPid, pstComponent->stSubtitle.utArg.stDvbInfo.szLangCode);
						break;
					case eOxMP_SUBTITLETYPE_Ebu:
						HxLOG_Debug("\tEBU : PID[0x%x] LangCode[%s] Magazine[%d] Page[%x]\n", pstComponent->stSubtitle.nPid, pstComponent->stSubtitle.utArg.stEbuInfo.szLangCode, pstComponent->stSubtitle.utArg.stEbuInfo.nMagazineNum, pstComponent->stSubtitle.utArg.stEbuInfo.nPageNum);
						break;
					case eOxMP_SUBTITLETYPE_Media:
						HxLOG_Debug("\tMEDIA : PID[0x%x] \n", pstComponent->stSubtitle.nPid);
						break;
					default:
						break;
				}
			}
			break;
		case eOxMP_COMPONENT_VIDEO:
			HxLOG_Debug("\tDVB : PID[0x%x] tag(%d) codec(%d) type(%d)\n", pstComponent->stVideo.nPid
				, pstComponent->stVideo.nComponentTag
				,pstComponent->stVideo.eCodec
				,pstComponent->stVideo.nComponentType);
			break;
		case eOxMP_COMPONENT_AUDIO:
			HxLOG_Debug("\tDVB : PID[0x%x] LangCode[%s]\n", pstComponent->stAudio.nPid, pstComponent->stAudio.aucStreamName);
			break;
		case eOxMP_COMPONENT_TELETEXT:
		case eOxMP_COMPONENT_CLOSEDCAPTION:
		case eOxMP_COMPONENT_DATA:
			break;
		default:
			break;
	}

}
#endif


#define _____NOTIFICATION_CALLBACKS_____
STATIC HERROR oapi_mp_CbOnMainViewChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Trace();

	if (nArgc == 1)
	{
		HUINT32	ulMainView	=	0;

		ulMainView	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		s_ulOapiMp_MainViewId	=	ulMainView;
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnTSREnable (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId;
	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 2)
	{
		HBOOL	bEnable	=	FALSE;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		bEnable		= (HBOOL)HxOBJECT_INT (apArgv[1]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.bTSREnable = bEnable;

			OAPI_MP_SEM_RELEASE;
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSubtitleEnable (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 2)
	{
		HBOOL	bEnable	=	FALSE;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		bEnable		= (HBOOL)HxOBJECT_INT (apArgv[1]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.bSubtitleEnable = bEnable;

			OAPI_MP_SEM_RELEASE;
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnVideoHideEnable (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 2)
	{
		HBOOL	bEnable	=	FALSE;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		bEnable		= (HBOOL)HxOBJECT_INT (apArgv[1]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.bVideoHideEnable = bEnable;

			OAPI_MP_SEM_RELEASE;
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnVideoSize (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 2)
	{
		OxMediaPlay_VideoSize_t		*pstVideoSize = NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
			pstVideoSize = (OxMediaPlay_VideoSize_t*)HxOBJECT_BIN_DATA (apArgv[1]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if ( (NULL != pstContext) && (NULL != pstVideoSize) )
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy(&(pstContext->stCache.stVideoSize), pstVideoSize, sizeof(OxMediaPlay_VideoSize_t) );
			OAPI_MP_SEM_RELEASE;
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPigRect (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 2)
	{
		OxMediaPlay_PigSize_t 	*pstPigSize = NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
			pstPigSize		= (OxMediaPlay_PigSize_t*)HxOBJECT_BIN_DATA (apArgv[1]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if ( (NULL != pstContext) && (NULL != pstPigSize) )
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy(&(pstContext->stCache.stPigSize), pstPigSize, sizeof(OxMediaPlay_PigSize_t) );
			OAPI_MP_SEM_RELEASE;
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSubtitleSyncTime (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 2)
	{
		HINT32	nSubtitle_SyncTime	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
			nSubtitle_SyncTime	= (HINT32)HxOBJECT_INT (apArgv[1]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.nSubtitle_SyncTime = nSubtitle_SyncTime;

			OAPI_MP_SEM_RELEASE;
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSubtitleFontSize (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 2)
	{
		HINT32	nSubtitle_FontSize	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
			nSubtitle_FontSize	= (HINT32)HxOBJECT_INT (apArgv[1]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.nSubtitle_FontSize = nSubtitle_FontSize;

			OAPI_MP_SEM_RELEASE;
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSubtitleTextPosition (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 2)
	{
		HINT32	nSubtitle_TextPosition	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
			nSubtitle_TextPosition	= (HINT32)HxOBJECT_INT (apArgv[1]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.nSubtitle_TextPosition = nSubtitle_TextPosition;

			OAPI_MP_SEM_RELEASE;
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnBufferingSecenario (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 1)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
#if 0 // 추후 필요하면 캐시 추가
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			OAPI_MP_SEM_RELEASE;
#endif
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnDrmLicenseData (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId;
	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 1)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
#if 0 // 추후 필요하면 캐시 추가
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			OAPI_MP_SEM_RELEASE;
#endif
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnBrowseruserAgentData (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId;
	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 1)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
#if 0 // 추후 필요하면 캐시 추가
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			OAPI_MP_SEM_RELEASE;
#endif
		}
	}

	return ERR_OK;
}


STATIC HERROR oapi_mp_CbOnDuration (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId;
	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 1)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
#if 0 // 추후 필요하면 캐시 추가
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			OAPI_MP_SEM_RELEASE;
#endif
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPlayerType (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId;
	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 1)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
#if 0 // 추후 필요하면 캐시 추가
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			OAPI_MP_SEM_RELEASE;
#endif
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnLocked (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e		eMediaType;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		Handle_t	hSvc	=	HANDLE_NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		hSvc		= (Handle_t)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)hSvc;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)hSvc;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//hSvc

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_TUNE_Locked, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnNoSignal (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e		eMediaType;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		Handle_t	hSvc	=	HANDLE_NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		hSvc		= (Handle_t)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)hSvc;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)hSvc;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//hSvc

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_TUNE_NoSignal, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnAntennaOk (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e		eMediaType;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		Handle_t	hSvc	=	HANDLE_NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		hSvc		= (Handle_t)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)hSvc;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)hSvc;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//hSvc

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_TUNE_AntennaOk, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnAntennaNok (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e		eMediaType;

	oapiMpContext_t				*pstContext	=	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		Handle_t	hSvc	=	HANDLE_NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		hSvc		= (Handle_t)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)hSvc;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)hSvc;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//hSvc

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_TUNE_AntennaNok, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSvcChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		HUINT32		 ulSvcUid = 0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ulSvcUid	= (HUINT32)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)ulSvcUid;
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
			stNotifyData.stEventNotify.ullParam2	= (HUINT64)pstContext->stCache.stLive.eTuneMode;
#else
			stNotifyData.stEventNotify.ullParam2	= 0;
#endif
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)ulSvcUid;
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
			stNotifyData.stEventNotify.ulParam2	= (HUINT32)pstContext->stCache.stLive.eTuneMode;
#else
			stNotifyData.stEventNotify.ulParam2	= 0;
#endif
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
#if defined(CONFIG_TIMESTAMP_64)
			pstContext->stCache.ullDurationTime	=	0;
			pstContext->stCache.ullPosition		=	0;
#else
			pstContext->stCache.ulDurationTime	=	0;
			pstContext->stCache.ulPosition		=	0;
#endif
			HxSTD_MemSet(&pstContext->stCache.stTsr, 0, sizeof(OxMediaPlay_Tsr_t));

			pstContext->stCache.eType = eOxMP_MEDIATYPE_LIVE;
			pstContext->stCache.stLive.ulMasterSvcUid = ulSvcUid;
			pstContext->stCache.stLive.ulSuppleSvcUid = ulSvcUid;
			// TSR_REC/REC duration reset
			// 주석 풀면 TSR playback으로 전환시 duration이 0이 됨...
			//pstContext->stCache.ulDurationTime	=	0;

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_LIVE_SvcChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSvcStopped (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		Handle_t	hSvc	=	HANDLE_NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		hSvc		= (Handle_t)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)hSvc;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)hSvc;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_LIVE_SvcStopped, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnConflictChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;
	oapiMpContext_t				*pstContext				=	NULL;
	OxMediaPlay_ConflictInfo_t	*pstLiveConflictInfo	=	NULL;

	if (nArgc == 4)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
			eMediaType			= (HUINT32)HxOBJECT_INT (apArgv[2]);
			pstLiveConflictInfo	= (OxMediaPlay_ConflictInfo_t*)HxOBJECT_BIN_DATA (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if( (NULL != pstContext) && (NULL != pstLiveConflictInfo) )
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy (&(pstContext->stCache.stLiveConflictInfo), pstLiveConflictInfo, sizeof(OxMediaPlay_ConflictInfo_t));

			OAPI_MP_SEM_RELEASE;
#if defined(CONFIG_DEBUG)
			oapi_media_play_printResourceStatus(pstLiveConflictInfo);
#endif
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnEventRelay (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		HINT32		nEvtRealyHandle	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		nEvtRealyHandle		= (HINT32)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)nEvtRealyHandle;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT64)nEvtRealyHandle;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//nEvtRealyHandle

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_LIVE_EventRelay, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnEws (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 5)
	{
		HINT32		nEwssvcuid		=	0;
		HINT32		nEwscursvcuid	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		nEwssvcuid			= (HINT32)HxOBJECT_INT (apArgv[3]);
		nEwscursvcuid		= (HINT32)HxOBJECT_INT (apArgv[4]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)nEwssvcuid;
			stNotifyData.stEventNotify.ullParam2	= (HUINT64)nEwscursvcuid;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)nEwssvcuid;
			stNotifyData.stEventNotify.ulParam2	= (HUINT32)nEwscursvcuid;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//nEwssvcuid
			//nEwscursvcuid

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_LIVE_Ews, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnBsCtrlStarted (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		Handle_t	hMasterSvc	=	HANDLE_NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		hMasterSvc		= (Handle_t)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)hMasterSvc;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)hMasterSvc;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//hMasterSvc

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_LIVE_BsCtrlStarted, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPinCtrlChangedByBCAS (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}
STATIC HERROR oapi_mp_CbOnTSRTimecodeChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 6)
	{
		HUINT32		ulTsrStartTime	=	0;
		HUINT32 	ulRecTime	=	0;
		HUINT32		ulLimitDuration	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		ulTsrStartTime	= (HUINT32)HxOBJECT_INT (apArgv[3]);
		ulRecTime		= (HUINT32)HxOBJECT_INT (apArgv[4]);
		ulLimitDuration	= (HUINT32)HxOBJECT_INT (apArgv[5]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)ulRecTime;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;//ulLimitDuration;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)ulRecTime;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;//ulLimitDuration;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			pstContext->stCache.stTsr.ulTsrStartTime	=	ulTsrStartTime;
			pstContext->stCache.stTsr.ulTsrRecTime	=	ulRecTime;

			if( eOxMP_MEDIATYPE_MEDIAPLAY != pstContext->stCache.eType )
			{
#if defined(CONFIG_TIMESTAMP_64)
				pstContext->stCache.ullDurationTime	=	(HUINT64)ulLimitDuration;
#else
				pstContext->stCache.ulDurationTime	=	ulLimitDuration;
#endif
			}

			if( eOxMP_MEDIATYPE_LIVE == pstContext->stCache.eType )
#if defined(CONFIG_TIMESTAMP_64)
				pstContext->stCache.ullPosition	=	(HUINT64)(ulRecTime + ulTsrStartTime);
#else
				pstContext->stCache.ulPosition	=	ulRecTime + ulTsrStartTime;
#endif

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			/*
			   There is nowhere using this event.
			   Therefore, we do not need to notify this event, but update the cache data.
			   Application can get this data whenever it is needed.
			*/
			//oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_LIVE_TSRTimecodeChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
			
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnViewStateChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		OxMediaPlay_LiveViewState_e 	eViewState	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		eViewState	= (OxMediaPlay_LiveViewState_e)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)eViewState;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)eViewState;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.eViewState	=	eViewState;

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_LIVE_ViewStateChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnLockStateChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		OxMediaPlay_LiveLockState_e 	eLockState	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		eLockState	= (OxMediaPlay_LiveLockState_e)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)eLockState;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)eLockState;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.eLockState	=	eLockState;

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_LIVE_LockStateChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnProbeErrored (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_ProbeErrored, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnStopErrored (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_StopErrored, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnBufferStateChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		OxMediaPlay_BufferState_e		eBufferState	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
			eBufferState	=	(OxMediaPlay_BufferState_e)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)eBufferState;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)eBufferState;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.eBufferState	=	eBufferState;

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_BufferStateChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnBufferInfoChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		OxMediaPlay_BufferInfo_t *pstBufferInfo	=	NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
			pstBufferInfo	=	(OxMediaPlay_BufferInfo_t*)HxOBJECT_BIN_DATA(apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy(&(pstContext->stCache.stBufferInfo), pstBufferInfo, sizeof(OxMediaPlay_BufferInfo_t));

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_BufferInfoChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPlayInfo (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();


	if (nArgc == 6)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
#if defined(CONFIG_TIMESTAMP_64)
			pstContext->stCache.ullPosition = (HUINT64)HxOBJECT_INT64 (apArgv[3]);
			pstContext->stCache.ullDurationTime = (HUINT64)HxOBJECT_INT64 (apArgv[4]);
#else
			pstContext->stCache.ulPosition = (HUINT32)HxOBJECT_INT (apArgv[3]);
			pstContext->stCache.ulDurationTime = (HUINT32)HxOBJECT_INT (apArgv[4]);
#endif
			pstContext->stCache.ulBitrate = (HUINT32)HxOBJECT_INT (apArgv[5]);
			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_PlayInfo, NULL, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPumpIndexChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 5)
	{
		HUINT32	ulPumpId	=	0;
		OxMediaPlay_PumpType_e ePumpType = eOxMP_MEDIAPLAY_PUMPTYPE_AV;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ulPumpId	= (HUINT32)HxOBJECT_INT (apArgv[3]);
		ePumpType	= (OxMediaPlay_PumpType_e)HxOBJECT_INT (apArgv[4]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)ulPumpId;
			stNotifyData.stEventNotify.ullParam2	= (HUINT64)ePumpType;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)ulPumpId;
			stNotifyData.stEventNotify.ulParam2	= (HUINT32)ePumpType;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_PumpIndexChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}
	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSeekReady (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_SeekReady, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbAudioCodecSwitched (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;
	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

#if 0		/* do not need to update cache */
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			OAPI_MP_SEM_RELEASE;
#endif

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_AudioCodecSwitched, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbAudioFlushCompleted (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;
	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if(nArgc == 3)
	{
		ulViewId 	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT(apArgv[1]);
		eMediaType 	= (OxMediaPlay_MediaType_e)HxOBJECT_INT(apArgv[2]);

		pstContext = oapi_mp_GetContext(ulViewId);
		if(pstContext != NULL)
		{
			OxMediaPlay_EventNotify_t	 stEventNotify;

#if defined(CONFIG_TIMESTAMP_64)
			stEventNotify.ullParam1	=	0;
			stEventNotify.ullParam2	=	0;
			stEventNotify.ullParam3	=	0;
#else
			stEventNotify.ulParam1 =	0;
			stEventNotify.ulParam2 =	0;
			stEventNotify.ulParam3 =	0;
#endif

			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_AudioFlushCompleted, (OxMediaPlay_NotifierData_t *)&stEventNotify, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbAudioRestartCompleted (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;
	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if(nArgc == 3)
	{
		ulViewId 	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT(apArgv[1]);
		eMediaType 	= (OxMediaPlay_MediaType_e)HxOBJECT_INT(apArgv[2]);

		pstContext = oapi_mp_GetContext(ulViewId);
		if(pstContext != NULL)
		{
			OxMediaPlay_EventNotify_t	 stEventNotify;

#if defined(CONFIG_TIMESTAMP_64)
			stEventNotify.ullParam1	=	0;
			stEventNotify.ullParam2	=	0;
			stEventNotify.ullParam3	=	0;
#else
			stEventNotify.ulParam1 =	0;
			stEventNotify.ulParam2 =	0;
			stEventNotify.ulParam3 =	0;
#endif

			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_AudioRestartCompleted, (OxMediaPlay_NotifierData_t *)&stEventNotify, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbAudioFadeCompleted (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;
	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if(nArgc == 3)
	{
		ulViewId 	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT(apArgv[1]);
		eMediaType 	= (OxMediaPlay_MediaType_e)HxOBJECT_INT(apArgv[2]);

		pstContext = oapi_mp_GetContext(ulViewId);
		if(pstContext != NULL)
		{
			OxMediaPlay_EventNotify_t	 stEventNotify;

#if defined(CONFIG_TIMESTAMP_64)
			stEventNotify.ullParam1	=	0;
			stEventNotify.ullParam2	=	0;
			stEventNotify.ullParam3	=	0;
#else
			stEventNotify.ulParam1 =	0;
			stEventNotify.ulParam2 =	0;
			stEventNotify.ulParam3 =	0;
#endif

#if 0		/* do not need to update cache */
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			OAPI_MP_SEM_RELEASE;
#endif
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_AudioFadeCompleted, (OxMediaPlay_NotifierData_t *)&stEventNotify, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbAudioFadeFailed (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;
	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if(nArgc == 3)
	{
		ulViewId 	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT(apArgv[1]);
		eMediaType 	= (OxMediaPlay_MediaType_e)HxOBJECT_INT(apArgv[2]);

		pstContext = oapi_mp_GetContext(ulViewId);
		if(pstContext != NULL)
		{
			OxMediaPlay_EventNotify_t	 stEventNotify;

#if defined(CONFIG_TIMESTAMP_64)
			stEventNotify.ullParam1	=	0;
			stEventNotify.ullParam2	=	0;
			stEventNotify.ullParam3	=	0;
#else
			stEventNotify.ulParam1 =	0;
			stEventNotify.ulParam2 =	0;
			stEventNotify.ulParam3 =	0;
#endif

#if 0		/* do not need to update cache */
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			OAPI_MP_SEM_RELEASE;
#endif
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_AudioFadeFailed, (OxMediaPlay_NotifierData_t *)&stEventNotify, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbAudioDecoderVolumeChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId = 0, ulSessionId = 0, unVolume = 0;
	OxMediaPlay_MediaType_e 	 eMediaType;
	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if(nArgc == 4)
	{
		ulViewId 	= (HUINT32)HxOBJECT_INT(apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT(apArgv[1]);
		eMediaType 	= (OxMediaPlay_MediaType_e)HxOBJECT_INT(apArgv[2]);
		unVolume	= (HUINT32)HxOBJECT_INT(apArgv[3]);

		pstContext = oapi_mp_GetContext(ulViewId);
		if(pstContext != NULL)
		{
			OxMediaPlay_EventNotify_t	 stEventNotify;

#if defined(CONFIG_TIMESTAMP_64)
			stEventNotify.ullParam1	=	0;
			stEventNotify.ullParam2	=	0;
			stEventNotify.ullParam3	=	0;
#else
			stEventNotify.ulParam1 =	0;
			stEventNotify.ulParam2 =	0;
			stEventNotify.ulParam3 =	0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.unAudioDecoderVolume = unVolume;
			OAPI_MP_SEM_RELEASE;

			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MEDIA_AudioDecoderVolumeChanged, (OxMediaPlay_NotifierData_t *)&stEventNotify, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSBTLUpdate (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 4)
	{
		OxMediaPlay_SubtitleStatus_t *pstStatus	=	NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		pstStatus	= (OxMediaPlay_SubtitleStatus_t*)HxOBJECT_BIN_DATA (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if ( (NULL != pstContext) && (NULL != pstStatus) )
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy(&(pstContext->stCache.stMediaSutbitleStatus), pstStatus, sizeof(OxMediaPlay_SubtitleStatus_t) );
			//

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SBTL_UPDATE, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnResourceMapChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	OxMediaPlay_MediaType_e 	eMediaType;
	OxMediaPlay_RPCResourceMap_t	*pstRPCResourceMap = NULL;
	HUINT32 ulPumpSize = 0;
	HUINT32 ulDmaSize = 0;

	HxLOG_Trace();

	if (nArgc == 5)
	{
		HxLOG_Trace();
		pstRPCResourceMap = HLIB_STD_MemCalloc( sizeof(OxMediaPlay_RPCResourceMap_t) );
		if(pstRPCResourceMap == NULL)
		{
			HxLOG_Error ("Out of Memory\n");
			return ERR_OK;
		}
		eMediaType = (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[0]);
		pstRPCResourceMap->ulNumOfPlaypump = (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[1]);
		pstRPCResourceMap->ulNumOfDma = (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		pstRPCResourceMap->ulAudioMixerHandle = (HUINT32)HxOBJECT_INT(apArgv[3]);
		pstRPCResourceMap->aResourceArray = (HCHAR *)HxOBJECT_BIN_DATA(apArgv[4]);

		if(s_pstResourceMap != NULL)
		{
			if(s_pstResourceMap->pulPlaypumpList != NULL)
			{
				HLIB_STD_MemFree(s_pstResourceMap->pulPlaypumpList);
				s_pstResourceMap->pulPlaypumpList = NULL;
			}

			if(s_pstResourceMap->pulDmaList != NULL)
			{
				HLIB_STD_MemFree(s_pstResourceMap->pulDmaList);
				s_pstResourceMap->pulDmaList = NULL;
			}

			HLIB_STD_MemFree(s_pstResourceMap);
			s_pstResourceMap = NULL;
		}

		s_pstResourceMap = (OxMediaPlay_ResourceMap_t*)HLIB_STD_MemCalloc( sizeof(OxMediaPlay_ResourceMap_t) );
		if(s_pstResourceMap == NULL)
		{
			HLIB_STD_MemFree(pstRPCResourceMap);
			pstRPCResourceMap = NULL;
			HxLOG_Error ("Out of Memory\n");
			return ERR_FAIL;
		}

		HxLOG_Trace();

		s_pstResourceMap->ulNumOfPlaypump = pstRPCResourceMap->ulNumOfPlaypump;
		s_pstResourceMap->ulNumOfDma = pstRPCResourceMap->ulNumOfDma;

		ulPumpSize = sizeof(s_pstResourceMap->pulPlaypumpList) * s_pstResourceMap->ulNumOfPlaypump;
		ulDmaSize = sizeof(s_pstResourceMap->pulDmaList) * s_pstResourceMap->ulNumOfDma;

		s_pstResourceMap->pulPlaypumpList = HLIB_STD_MemCalloc( ulPumpSize );
		if(s_pstResourceMap->pulPlaypumpList  == NULL)
		{
			HLIB_STD_MemFree(pstRPCResourceMap);
			pstRPCResourceMap = NULL;
			HLIB_STD_MemFree(s_pstResourceMap);
			s_pstResourceMap = NULL;
			HxLOG_Error ("Out of Memory\n");
			return ERR_FAIL;
		}

		s_pstResourceMap->pulDmaList = HLIB_STD_MemCalloc( ulDmaSize );
		if(s_pstResourceMap->pulDmaList  == NULL)
		{
			HLIB_STD_MemFree(pstRPCResourceMap);
			pstRPCResourceMap = NULL;
			HLIB_STD_MemFree(s_pstResourceMap->pulPlaypumpList);
			s_pstResourceMap->pulPlaypumpList = NULL;
			HLIB_STD_MemFree(s_pstResourceMap);
			s_pstResourceMap = NULL;

			HxLOG_Error ("Out of Memory\n");
			return ERR_FAIL;
		}

		HxLOG_Error("ulDmaListSize:%d, ulPumpListSize:%d\n", ulDmaSize, ulPumpSize);
		HxSTD_MemCopy( s_pstResourceMap->pulPlaypumpList, &pstRPCResourceMap->aResourceArray[0], ulPumpSize );
		HxSTD_MemCopy( s_pstResourceMap->pulDmaList, &pstRPCResourceMap->aResourceArray[ulPumpSize], ulDmaSize );

		s_pstResourceMap->ulAudioMixerHandle = pstRPCResourceMap->ulAudioMixerHandle;

#if 0
		HxLOG_Trace();
		HxLOG_Error("ulNumOfDma:%d, ulNumOfPlaypump:%d\n", s_pstResourceMap->ulNumOfDma, s_pstResourceMap->ulNumOfPlaypump);
		{
			int i=0;
			for(i=0; i<s_pstResourceMap->ulNumOfPlaypump; ++i)
			{
				HxLOG_Error("[%08X]\n", s_pstResourceMap->pulPlaypumpList[i]);
			}
		}
#endif
		if(pstRPCResourceMap)
		{
			HLIB_STD_MemFree(pstRPCResourceMap);
			pstRPCResourceMap = NULL;
		}
		/* cached Resource Map. Don't used oapi_mp_NotifyToListener */
	}

	s_bGetResource = FALSE;
	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnBof (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.eState	=	eOxMP_PLAYSTATE_BOS;

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_BOF, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}


STATIC HERROR oapi_mp_CbOnEof (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	HxLOG_Trace();

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.eState	=	eOxMP_PLAYSTATE_EOS;

			OAPI_MP_SEM_RELEASE;

			/* EMIT EVENT */
			HxLOG_Trace();
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_EOF, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnStarted (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						 ulViewId, ulSessionId;
	OxMediaPlay_StartInfo_t		*punStart;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		OxMediaPlay_NotifierData_t	stNotifyData;

		HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		punStart	= (OxMediaPlay_StartInfo_t *)HxOBJECT_BIN_DATA (apArgv[3]);

		HxSTD_memcpy(&stNotifyData.stPlayStarted, punStart, sizeof(OxMediaPlay_StartInfo_t));

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			switch(punStart->eType)
			{
				case eOxMP_MEDIATYPE_LIVE:			HxSTD_MemCopy(&(pstContext->stCache.stLive), 	  punStart, sizeof(OxMediaPlay_StartLive_t) );	break;
				case eOxMP_MEDIATYPE_PVRPLAYBACK:	HxSTD_MemCopy(&(pstContext->stCache.stPvrPb), 	  punStart, sizeof(OxMediaPlay_StartPvrPb_t) ); break;
				case eOxMP_MEDIATYPE_TSRPLAYBACK:	HxSTD_MemCopy(&(pstContext->stCache.stTsrPb), 	  punStart, sizeof(OxMediaPlay_StartTsrPb_t) ); break;
				case eOxMP_MEDIATYPE_MEDIAPLAY:		HxSTD_MemCopy(&(pstContext->stCache.stMediaPlay), punStart, sizeof(OxMediaPlay_StartMediaPlay_t) ); break;
				default:
					break;
			}

			pstContext->stCache.eType	=	eMediaType;
			pstContext->stCache.eState	=	eOxMP_PLAYSTATE_PLAYING;

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_STARTED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnStopped (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			oapi_mp_CleanCache(pstContext, eMediaType);

			pstContext->stCache.eState	=	eOxMP_PLAYSTATE_STOPPED;

			OAPI_MP_SEM_RELEASE;
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_STOPPED, NULL, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPlayStateChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;
	OxMediaPlay_PlayState_e		 ePlayState;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		OxMediaPlay_NotifierData_t	stNotifyData;

		HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ePlayState	= (OxMediaPlay_PlayState_e)HxOBJECT_INT (apArgv[3]);

		stNotifyData.eStateChanged = ePlayState;

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.eState	=	ePlayState;

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_PLAYSTATE, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPlayErrorChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;
	OxMediaPlay_PlayError_e		 ePlayError;
	OxMediaPlay_ErrorReason_e	 eReason;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 5)
	{
		OxMediaPlay_NotifierData_t	stNotifyData;

		HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ePlayError	= (OxMediaPlay_PlayError_e)HxOBJECT_INT (apArgv[3]);
		eReason		= (OxMediaPlay_ErrorReason_e)HxOBJECT_INT (apArgv[4]);

		stNotifyData.eErrorChanged = ePlayError;

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.eError	=	ePlayError;
			pstContext->stCache.eReason	=	eReason;

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_PLAYERROR, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnVmxDrmErrorChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
    HUINT32                      ulViewId, ulSessionId;
    oapiMpContext_t             *pstContext;
    HINT32                       nErrorCode;

    if (nArgc == 3)
    {
        ulViewId    = (HUINT32)HxOBJECT_INT (apArgv[0]);
        ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
        nErrorCode  = (HINT32)HxOBJECT_INT (apArgv[2]);

        pstContext = oapi_mp_GetContext (ulViewId);
        if (NULL != pstContext)
        {
			pstContext->stCache.nVmxErrorCode = nErrorCode;
			if(nErrorCode == 0)
            {
               	pstContext->stCache.eVmxStatus = eOxMP_VMXSTATUS_INITIALISING;
            }
            else
            {
               	pstContext->stCache.eVmxStatus = eOxMP_VMXSTATUS_ERROR;
            }
        }
    }

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnVmxDrmStatusChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
    HUINT32                      ulViewId, ulSessionId;
    oapiMpContext_t             *pstContext;
    OxMediaplay_VmxStatus_e     eStatus;

    if (nArgc == 3)
    {
        ulViewId    = (HUINT32)HxOBJECT_INT (apArgv[0]);
        ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
        eStatus = (OxMediaplay_VmxStatus_e)HxOBJECT_INT (apArgv[2]);

        pstContext = oapi_mp_GetContext (ulViewId);
        if (NULL != pstContext)
        {
            pstContext->stCache.eVmxStatus = eStatus;
        }
    }
	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPlaying (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.eState	=	eOxMP_PLAYSTATE_PLAYING;

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_PLAYING, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPaused (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.eState	=	eOxMP_PLAYSTATE_PAUSE;

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_PAUSE, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnTricked (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_TRICK, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPlayTimeChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 5)
	{
#if defined(CONFIG_TIMESTAMP_64)
		HUINT64	ullPosition		=	0;
		HUINT64	ullDurationTime	=	0;
#else
		HUINT64 ulPosition 	=	0;
		HUINT64 ulDurationTime =	0;
#endif

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
#if defined(CONFIG_TIMESTAMP_64)
		ullPosition 	= (HUINT64)HxOBJECT_INT64 (apArgv[3]);
		ullDurationTime	= (HUINT64)HxOBJECT_INT64 (apArgv[4]);
#else
		ulPosition 	= (HUINT32)HxOBJECT_INT (apArgv[3]);
		ulDurationTime = (HUINT32)HxOBJECT_INT (apArgv[4]);
#endif

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
#if defined(CONFIG_TIMESTAMP_64)
			if( (HINT64)ullPosition != -1 )
			{
				pstContext->stCache.ullPosition	=	ullPosition;
			}

			if( (HINT64)ullDurationTime != -1 )
				pstContext->stCache.ullDurationTime =	ullDurationTime;
#else
			if( (HINT32)ulPosition != -1 )
			{
				pstContext->stCache.ulPosition =	ulPosition;
			}

			if( (HINT32)ulDurationTime != -1 )
				pstContext->stCache.ulDurationTime =	ulDurationTime;
#endif
			OAPI_MP_SEM_RELEASE;

#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= pstContext->stCache.ullPosition;
			stNotifyData.stEventNotify.ullParam2	= pstContext->stCache.ullDurationTime;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= pstContext->stCache.ulPosition;
			stNotifyData.stEventNotify.ulParam2	= pstContext->stCache.ulDurationTime;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_PLAYTIME, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPlayDurationChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 5)
	{
#if defined(CONFIG_TIMESTAMP_64)
		HUINT64	ullDurationTime	=	0;
#else
		HUINT32 ulDurationTime =	0;
#endif
		HBOOL bInfinity = 0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
#if defined(CONFIG_TIMESTAMP_64)
		ullDurationTime	= (HUINT64)HxOBJECT_INT64 (apArgv[3]);
#else
		ulDurationTime = (HUINT32)HxOBJECT_INT (apArgv[3]);
#endif
		bInfinity	= (HBOOL)HxOBJECT_INT (apArgv[4]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
#if defined(CONFIG_TIMESTAMP_64)
			if( ullDurationTime != -1 )
				pstContext->stCache.ullDurationTime = ullDurationTime;
#else
			if( ulDurationTime != -1 )
				pstContext->stCache.ulDurationTime = ulDurationTime;
#endif
			pstContext->stCache.bInfinity =	bInfinity;

			OAPI_MP_SEM_RELEASE;

#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= pstContext->stCache.ullDurationTime;
			stNotifyData.stEventNotify.ullParam2	= (HUINT64)pstContext->stCache.bInfinity;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= pstContext->stCache.ulDurationTime;
			stNotifyData.stEventNotify.ulParam2	= (HUINT64)pstContext->stCache.bInfinity;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_PLAYDURATION, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSpeedChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		HINT32	nSpeed		=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		nSpeed		= (HINT32)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)nSpeed;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= nSpeed;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.nSpeed	=	nSpeed;

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SPEED_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnRatingChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 6)
	{
		HINT32		nBlocked		=	0;
		HINT32		nRating 		=	0;
#if defined(CONFIG_OP_SES)
		HINT32		nBlockedByCas	=	0;
#else
		HINT32		nDrmSystemId	=	0;
#endif
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		nBlocked		= (HINT32)HxOBJECT_INT (apArgv[3]);
		nRating			= (HINT32)HxOBJECT_INT (apArgv[4]);
#if defined(CONFIG_OP_SES)
		nBlockedByCas	= (HINT32)HxOBJECT_INT (apArgv[5]);
#else
		nDrmSystemId	= (HINT32)HxOBJECT_INT (apArgv[5]);
#endif
		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)nBlocked;
			stNotifyData.stEventNotify.ullParam2	= (HUINT64)nRating;
			#if defined(CONFIG_OP_SES)
			stNotifyData.stEventNotify.ullParam3	=	(HUINT64)nBlockedByCas;
			#else
			stNotifyData.stEventNotify.ullParam3	=	(HUINT64)nDrmSystemId;
			#endif
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)nBlocked;
			stNotifyData.stEventNotify.ulParam2	= (HUINT32)nRating;
			#if defined(CONFIG_OP_SES)
			stNotifyData.stEventNotify.ulParam3	=	(HUINT32)nBlockedByCas;
			#else
			stNotifyData.stEventNotify.ulParam3	=	(HUINT32)nDrmSystemId;
			#endif
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.nBlocked		=	nBlocked;
			pstContext->stCache.nRating			=	nRating;
#if defined(CONFIG_OP_SES)
			pstContext->stCache.nBlockedByCas	=	nBlockedByCas;
#else
			pstContext->stCache.nDrmSystemId	=	nDrmSystemId;
#endif
			OAPI_MP_SEM_RELEASE;

			/*
			 *	현재 OPL 에서 RatingChanged에 관한 로직이 PinCtrlChanged Event로 처리 됨.
			 *  Event 관련 정리시 두 Event에 대한 통합 or OPL 관련 수정이 필요함.
			 */
#if 1
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_PinCtrlChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
#else
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_RATING_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
#endif
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnRequestPincode (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		HUINT8	*pszMsg		=	NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[1]);
		pszMsg		= (HUINT8*)HxOBJECT_STR (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)pszMsg;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)pszMsg;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//pstContext->stCache.nBlocked		=	nBlocked;
			OAPI_MP_SEM_RELEASE;

			/*
			 *	현재 OPL 에서 RatingChanged에 관한 로직이 PinCtrlChanged Event로 처리 됨.
			 *  Event 관련 정리시 두 Event에 대한 통합 or OPL 관련 수정이 필요함.
			 */
			oapi_mp_NotifyToListener (pstContext, pstContext->ulSessionId, eMediaType, eOxMP_EVENT_RequestPincode, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnResultPincode (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId = 0; //view 0 fix
	OxMediaPlay_MediaType_e		eMediaType = eOxMP_MEDIATYPE_LIVE;
	HBOOL						bPermit;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 1)
	{
		bPermit		= (HBOOL)HxOBJECT_INT (apArgv[0]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)bPermit;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)bPermit;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			oapi_mp_NotifyToListener (pstContext, pstContext->ulSessionId, eMediaType, eOxMP_EVENT_ResultPincode, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}



STATIC HERROR oapi_mp_CbOnAVStarted	(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_AV_STARTED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnAVStopped (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_AV_STOPPED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnAVResume (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_AV_RESUME, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnAVUnderrun (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_AV_UNDERRUN, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnAVMediaRectChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_AV_MEDIARECT_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSupportedSpeedChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 5)
	{
		HUINT32		ulNumSpeed;
		HINT32		*pSpeeds;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ulNumSpeed	= (HUINT32)HxOBJECT_INT (apArgv[3]);
		pSpeeds 	= (HINT32*)HxOBJECT_BIN_DATA (apArgv[4]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			if( 0 < ulNumSpeed )
			{
				pstContext->stCache.ulNumSpeed	=	ulNumSpeed;
				HxSTD_MemCopy( &(pstContext->stCache.anSupportSpeeds), pSpeeds, sizeof(HINT32) * ulNumSpeed);
			}

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SupportedSpeedChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSiPMTChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SI_PMT_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSiEITChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SI_EIT_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSiSITChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SI_SIT_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSiAITChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 	 ulViewId, ulSessionId;
	HUINT8							*pucAitRaw = NULL;
	OxMediaPlay_MediaType_e 	 	 eMediaType;
	OxMediaPlay_AitSectionData_t	*pstAitSecData;

	oapiMpContext_t 				*pstContext =	NULL;

	if (nArgc == 5)
	{
		ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType		= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		pstAitSecData 	= (OxMediaPlay_AitSectionData_t *)HxOBJECT_BIN_DATA (apArgv[3]);
		pucAitRaw 		= (HUINT8 *)HxOBJECT_BIN_DATA (apArgv[4]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			oapi_mp_CleanAitSectionCache(pstContext);
			HxSTD_MemCopy(&(pstContext->stCache.stAitSecData), pstAitSecData, sizeof(OxMediaPlay_AitSectionData_t) );

			if (pstContext->stCache.stAitSecData.eAitState == eAitState_Received)
			{
				HUINT32			 i, ulSecLen, ulCurPos;
				HUINT8			*pucSection = NULL;

				ulCurPos = 0;
				for (i = 0 ; i < OxMP_AIT_SECTION_NUM_MAX ; i++)
				{
					// check current raw section
					pucSection = pstContext->stCache.stAitSecData.paucAitRawSec[i];
					if (pucSection != NULL)
					{
						// 이런 일이 생기면 안 됨..
						HLIB_STD_MemFree(pucSection);
						pucSection = NULL;
					}

					ulSecLen = pstContext->stCache.stAitSecData.aulAitSecLen[i];
					if (ulSecLen == 0)
					{
						break;
					}

					// make raw section  cache
					pucSection = (HUINT8 *)HLIB_STD_MemCalloc (ulSecLen);
					HxSTD_MemCopy(pucSection, &pucAitRaw[ulCurPos], ulSecLen);
					pstContext->stCache.stAitSecData.paucAitRawSec[i] = pucSection;

					ulCurPos += ulSecLen;
				}

				pstContext->stCache.stAitSecData.ulAitTotaSeclLen 	= i;
			}
			else
			{
				pstContext->stCache.stAitSecData.ulAitTotaSeclLen 	= 0;
			}
			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SI_AIT_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnSiRCTChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 	ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 	eMediaType;
	OxMediaPlay_RctState_e			eRctState;
	oapiMpContext_t 				*pstContext =	NULL;
	HxObject_t						*pstObjectRct = NULL, *pstObjectImage = NULL;
	HERROR							 hResult = ERR_FAIL;

	if (nArgc == 5)
	{
		ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType		= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		eRctState			= (OxMediaPlay_RctState_e)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			if(eRctState == eRctState_Received)
			{
				void							*pvRctInfo = NULL;
				HUINT32 						ulSize;
				OxMediaPlay_RctData_t			*pstRctData= NULL;
				void							*pvImageInfo = NULL;
				HUINT32 						ulImageInfoSize;

				pvRctInfo 	= (void *)HxOBJECT_BIN_DATA (apArgv[4]);
				ulSize 	= (HUINT32 )HxOBJECT_BIN_SIZE (apArgv[4]);

				OAPI_MP_SEM_GET;

				oapi_mp_CleanRctDataCache(pstContext);
				pstContext->stCache.stRctData.eRctState = eRctState;

				pstObjectRct = HLIB_SERIALIZER_Unpack(pvRctInfo, ulSize);
				if (pstObjectRct == NULL)
				{
					HxLOG_Error("cannot unpack the data (pvRctInfo)\n");
					if(pvRctInfo != NULL)
					{
						HLIB_STD_MemFree(pvRctInfo);
					}
					OAPI_MP_SEM_RELEASE;
					goto END_FUNC;
				}

				if (HxOBJECT_ARR_LEN(pstObjectRct) != 6)
				{
					HxLOG_Error("invalid object arr len.(pvRctInfo) \n");
					if(pvRctInfo != NULL)
					{
						HLIB_STD_MemFree(pvRctInfo);
					}
					OAPI_MP_SEM_RELEASE;
					goto END_FUNC;
				}

				pstRctData = &(pstContext->stCache.stRctData);
				if(pstRctData != NULL)
				{
					pstRctData->stRctInfo.usServiceId			= (HUINT16)HxOBJECT_ARR_VAL(pstObjectRct, 0)->u.num;
					pstRctData->stRctInfo.usYearOffset 		= (HUINT16)HxOBJECT_ARR_VAL(pstObjectRct, 1)->u.num;
					pstRctData->stRctInfo.ulTotalLinkCnt		= (HUINT32)HxOBJECT_ARR_VAL(pstObjectRct, 2)->u.num;

					pstRctData->stRctInfo.pstLinkInfoList = 		(DxService_RctLinkInfo_t*)HLIB_STD_MemCalloc(sizeof(DxService_RctLinkInfo_t) *pstRctData->stRctInfo.ulTotalLinkCnt );
					if(pstRctData->stRctInfo.pstLinkInfoList != NULL)
					{
						HxSTD_MemCopy(pstRctData->stRctInfo.pstLinkInfoList,
										(void*)HxOBJECT_ARR_VAL(pstObjectRct, 3)->u.bin.data,
										(HUINT32)HxOBJECT_ARR_VAL(pstObjectRct, 3)->u.bin.size);
					}

					// short evnet Descriptor
					HxSTD_MemCopy(&pstRctData->stRctInfo.stShortEvtInfo,
									(void*)HxOBJECT_ARR_VAL(pstObjectRct, 4)->u.bin.data,
									(HUINT32)HxOBJECT_ARR_VAL(pstObjectRct, 4)->u.bin.size);

					// Image Icon Descriptor
					pvImageInfo = (void *)HxOBJECT_ARR_VAL(pstObjectRct, 5)->u.bin.data;
					ulImageInfoSize = (HUINT32)HxOBJECT_ARR_VAL(pstObjectRct, 5)->u.bin.size;

					pstObjectImage = HLIB_SERIALIZER_Unpack(pvImageInfo, ulImageInfoSize);
					if (pstObjectImage != NULL)
					{
						pstRctData->stRctInfo.stImageIconInfo.ucIconId		= (HUINT8)HxOBJECT_ARR_VAL(pstObjectImage, 0)->u.num;
						pstRctData->stRctInfo.stImageIconInfo.ucTransPortMode	= (HUINT8)HxOBJECT_ARR_VAL(pstObjectImage, 1)->u.num;
						pstRctData->stRctInfo.stImageIconInfo.ucPositionFlag		= (HUINT8)HxOBJECT_ARR_VAL(pstObjectImage, 2)->u.num;
						pstRctData->stRctInfo.stImageIconInfo.ucCoordinateSystem  = (HUINT8)HxOBJECT_ARR_VAL(pstObjectImage, 3)->u.num;
						pstRctData->stRctInfo.stImageIconInfo.usHorizontalOrigin	= (HUINT16)HxOBJECT_ARR_VAL(pstObjectImage, 4)->u.num;
						pstRctData->stRctInfo.stImageIconInfo.usVerticalOrigin	= (HUINT16)HxOBJECT_ARR_VAL(pstObjectImage, 5)->u.num;
						HxSTD_MemCopy(pstRctData->stRctInfo.stImageIconInfo.szIconTypeChar,
										(void*)HxOBJECT_ARR_VAL(pstObjectImage, 6)->u.bin.data,
										(HUINT32)HxOBJECT_ARR_VAL(pstObjectImage, 6)->u.bin.size);

						pstRctData->stRctInfo.stImageIconInfo.ulDataByteSize	= (HUINT32)HxOBJECT_ARR_VAL(pstObjectImage, 7)->u.num;
						if(pstRctData->stRctInfo.stImageIconInfo.ulDataByteSize)
						{
							pstRctData->stRctInfo.stImageIconInfo.pucDatabyte  = (HUINT8*)HLIB_STD_MemCalloc(sizeof(HUINT8) *pstRctData->stRctInfo.stImageIconInfo.ulDataByteSize );
							if(pstRctData->stRctInfo.stImageIconInfo.pucDatabyte != NULL)
							{
								HxSTD_MemCopy(pstRctData->stRctInfo.stImageIconInfo.pucDatabyte,
									(void*)HxOBJECT_ARR_VAL(pstObjectImage, 8)->u.bin.data,
									(HUINT32)HxOBJECT_ARR_VAL(pstObjectImage, 8)->u.bin.size);
							}

							HxSTD_MemCopy(pstRctData->stRctInfo.stImageIconInfo.szIconUrl,
											(void*)HxOBJECT_ARR_VAL(pstObjectImage, 9)->u.bin.data,
											(HUINT32)HxOBJECT_ARR_VAL(pstObjectImage, 9)->u.bin.size);
						}
						else
						{
							HxSTD_MemCopy(pstRctData->stRctInfo.stImageIconInfo.szIconUrl,
											(void*)HxOBJECT_ARR_VAL(pstObjectImage, 8)->u.bin.data,
											(HUINT32)HxOBJECT_ARR_VAL(pstObjectImage, 8)->u.bin.size);
						}
					}

#if defined(CONFIG_TIMESTAMP_64)
					stNotifyData.stEventNotify.ullParam1 = (HUINT64)pstRctData->stRctInfo.ulTotalLinkCnt;
					stNotifyData.stEventNotify.ullParam2 = (HUINT64)pstRctData->stRctInfo.stImageIconInfo.ulDataByteSize;
					stNotifyData.stEventNotify.ullParam3 = 0;
#else
					stNotifyData.stEventNotify.ulParam1 = pstRctData->stRctInfo.ulTotalLinkCnt;
					stNotifyData.stEventNotify.ulParam2 = pstRctData->stRctInfo.stImageIconInfo.ulDataByteSize;
					stNotifyData.stEventNotify.ulParam3 = 0;
#endif

				}

				OAPI_MP_SEM_RELEASE;
			}
			else
			{
				OAPI_MP_SEM_GET;
				oapi_mp_CleanRctDataCache(pstContext);
				pstContext->stCache.stRctData.eRctState = eRctState;
				OAPI_MP_SEM_RELEASE;

			}

#if defined(CONFIG_TIMESTAMP_64)
			HxLOG_Debug("[%s]  Send 'eOxMP_EVENT_SI_RCT_CHANGED' RctState(%d) Param(%llu, %llu, %llu) \n", __FUNCTION__, eRctState, stNotifyData.stEventNotify.ullParam1, stNotifyData.stEventNotify.ullParam2, stNotifyData.stEventNotify.ullParam3);
#else
			HxLOG_Debug("[%s]  Send 'eOxMP_EVENT_SI_RCT_CHANGED' RctState(%d) Param(%d, %d, %d) \n", __FUNCTION__, eRctState, stNotifyData.stEventNotify.ulParam1, stNotifyData.stEventNotify.ulParam2, stNotifyData.stEventNotify.ulParam3);
#endif

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SI_RCT_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstObjectRct)					{ HLIB_SERIALIZER_FreeObject(pstObjectRct); }
	if (NULL != pstObjectImage)					{ HLIB_SERIALIZER_FreeObject(pstObjectImage); }

	return hResult;
}

STATIC HERROR oapi_mp_CbOnSiRCTRequestImageIcon (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 						ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e		eMediaType;
	oapiMpContext_t				*pstContext = NULL;

	ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
	ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
	eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);


	pstContext = oapi_mp_GetContext (ulViewId);
	if(pstContext != NULL)
	{
		HUINT8					*pszImageIconUrl = NULL;
		OxMediaPlay_NotifierData_t	stNotifyData;

		HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));

		pszImageIconUrl = (HUINT8*)HxOBJECT_STR (apArgv[3]);

		OAPI_MP_SEM_GET;

		HxSTD_MemCopy(pstContext->stCache.szRctImageIconUrl, pszImageIconUrl, DxRCT_MAX_IMAGE_ICON_URL_LEN + 1);

#if defined(CONFIG_TIMESTAMP_64)
		stNotifyData.stEventNotify.ullParam1 = (HUINT64)pstContext->stCache.szRctImageIconUrl;
		stNotifyData.stEventNotify.ullParam2 = 0;
		stNotifyData.stEventNotify.ullParam3 = 0;
#else
		stNotifyData.stEventNotify.ulParam1 = (HUINT32)pstContext->stCache.szRctImageIconUrl;
		stNotifyData.stEventNotify.ulParam2 = 0;
		stNotifyData.stEventNotify.ulParam3 = 0;
#endif

		OAPI_MP_SEM_RELEASE;

		oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SI_RCT_REQUEST_IMAEG_ICON, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnMhegMonStarted (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 	 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 	 eMediaType;
	oapiMpContext_t 				*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType		= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			oapi_mp_NotifyToListener(pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MHEG_MON_STARTED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnMhegMonStopped (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 	 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 	 eMediaType;
	oapiMpContext_t 				*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType		= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			oapi_mp_NotifyToListener(pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MHEG_MON_STOPPED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnMhegDmxChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 	 ulViewId, ulSessionId;
	HUINT32							 ulDemuxId;
	OxMediaPlay_MediaType_e 	 	 eMediaType;
	oapiMpContext_t 				*pstContext =	NULL;

	if (nArgc == 4)
	{
		ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType		= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ulDemuxId		= (HUINT32)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)ulViewId;
			stNotifyData.stEventNotify.ullParam2	= (HUINT64)ulDemuxId;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= ulViewId;
			stNotifyData.stEventNotify.ulParam2	= ulDemuxId;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			oapi_mp_NotifyToListener(pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MHEG_DMX_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnMhegPmtChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 	 ulViewId, ulSessionId;
	HUINT32							 ulSvcUid;
	HUINT8							*pucPatSecRaw = NULL;
	HUINT8							*pucPmtSecRaw = NULL;
	OxMediaPlay_MediaType_e 	 	 eMediaType;
	OxMediaPlay_MhegSectionData_t	*pstMhegSecData;
	oapiMpContext_t 				*pstContext =	NULL;
	OxMediaPlay_MhegPmtStatus_e		eMhegPmtState = eMhegPmtStatus_None;

	HUINT32							 i, ulSecLen, ulCurPos;
	HUINT8							*pucSection = NULL;

	OxMediaPlay_NotifierData_t	stNotifyData;

	HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));

	if (nArgc == 8)
	{
		ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType		= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ulSvcUid		= (HUINT32)HxOBJECT_INT (apArgv[3]);
		pstMhegSecData 	= (OxMediaPlay_MhegSectionData_t *)HxOBJECT_BIN_DATA (apArgv[4]);
		pucPatSecRaw	= (HUINT8 *)HxOBJECT_BIN_DATA (apArgv[5]);
		pucPmtSecRaw	= (HUINT8 *)HxOBJECT_BIN_DATA (apArgv[6]);
		eMhegPmtState =(HUINT32)HxOBJECT_INT (apArgv[7]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* UPDATE Cache only */

			oapi_mp_CleanMhegSectionCache(pstContext);
			HxSTD_MemCopy(&(pstContext->stCache.stMhegSecData), pstMhegSecData, sizeof(OxMediaPlay_MhegSectionData_t));

			// make PAT raw section cache
			if (pstMhegSecData->ulPatTotalSecLen > 1)
			{
				ulCurPos = 0;
				for (i = 0 ; i < OxMP_PAT_SECTION_NUM_MAX ; i++)
				{
					// check current raw section
					pucSection = pstContext->stCache.stMhegSecData.paucPatRawSec[i];
					if (pucSection != NULL)
					{
						// 이런 일이 생기면 안 됨..
						HLIB_STD_MemFree(pucSection);
						pucSection = NULL;
					}

					ulSecLen = pstContext->stCache.stMhegSecData.aulPatSecLen[i];
					if (ulSecLen == 0)
					{
						break;
					}

					pucSection = (HUINT8 *)HLIB_STD_MemCalloc (ulSecLen);
					HxSTD_MemCopy(pucSection, &pucPatSecRaw[ulCurPos], ulSecLen);
					pstContext->stCache.stMhegSecData.paucPatRawSec[i] = pucSection;

					ulCurPos += ulSecLen;
				}
			}
			else
			{
				pstContext->stCache.stMhegSecData.ulPatTotalSecLen 	= 0;
			}

			// make PMT raw section cache
			if (pstMhegSecData->ulPmtTotalSecLen > 1)
			{
				ulCurPos = 0;
				for (i = 0 ; i < OxMP_PMT_SECTION_NUM_MAX ; i++)
				{
					// check current raw section
					pucSection = pstContext->stCache.stMhegSecData.paucPmtRawSec[i];
					if (pucSection != NULL)
					{
						// 이런 일이 생기면 안 됨..
						HLIB_STD_MemFree(pucSection);
						pucSection = NULL;
					}

					ulSecLen = pstContext->stCache.stMhegSecData.aulPmtSecLen[i];
					if (ulSecLen == 0)
					{
						break;
					}

					pucSection = (HUINT8 *)HLIB_STD_MemCalloc (ulSecLen);
					HxSTD_MemCopy(pucSection, &pucPmtSecRaw[ulCurPos], ulSecLen);
					pstContext->stCache.stMhegSecData.paucPmtRawSec[i] = pucSection;

					ulCurPos += ulSecLen;
				}
			}
			else
			{
				pstContext->stCache.stMhegSecData.ulPmtTotalSecLen 	= 0;
			}

			OAPI_MP_SEM_RELEASE;

#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1 = (HUINT64)ulViewId;
			stNotifyData.stEventNotify.ullParam2 = (HUINT64)ulSvcUid;
			stNotifyData.stEventNotify.ullParam3 = (HUINT64)eMhegPmtState; // mheg pmt Status;
#else
			stNotifyData.stEventNotify.ulParam1 = ulViewId;
			stNotifyData.stEventNotify.ulParam2 = ulSvcUid;
			stNotifyData.stEventNotify.ulParam3 = eMhegPmtState; // mheg pmt Status;
#endif

			oapi_mp_NotifyToListener(pstContext, ulSessionId, eMediaType, eOxMP_EVENT_MHEG_PMT_RECEIVED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);

		}
	}

	return ERR_OK;
}
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
STATIC HERROR oapi_mp_CbOnMhegNDTChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32						ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e		eMediaType;
	oapiMpContext_t				*pstContext	=	NULL;
	OxMediaPlay_TuningMode_e		eTuneMode;

	HxLOG_Trace();
	
	if (nArgc == 4)
	{

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		eTuneMode	= (OxMediaPlay_TuningMode_e)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{			
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.stLive.eTuneMode = eTuneMode;

			OAPI_MP_SEM_RELEASE;
		}
	}

	return ERR_OK;
}
#endif

STATIC HERROR oapi_mp_CbOnAvailabilityStartTimeChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;
	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		HCHAR* pszStartTime =	NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		pszStartTime	= (HCHAR*)HxOBJECT_STR (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_snprintf(pstContext->stCache.szAvailabilityStartTime, sizeof(pstContext->stCache.szAvailabilityStartTime), "%s", pszStartTime);
			OAPI_MP_SEM_RELEASE;
		}
	}
	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnComponentVideoChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		OxMediaPlay_VideoCompTable_t *pstVideoCompTable	=	NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		pstVideoCompTable	= (OxMediaPlay_VideoCompTable_t*)HxOBJECT_BIN_DATA (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy(&(pstContext->stCache.stVideoCompTable), pstVideoCompTable, sizeof(OxMediaPlay_VideoCompTable_t) );

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_COMPONENT_VIDEO_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);

#if defined(CONFIG_DEBUG)
{
			HINT32 i=0;

			HxLOG_Debug("--VIDEO TRACK LIST--\n");
			for( i=0 ; i<pstContext->stCache.stVideoCompTable.nArraySize ; i++ )
				oapi_mp_PrintComponent (eOxMP_COMPONENT_VIDEO, (OxMediaPlay_Component_t*)&pstContext->stCache.stVideoCompTable.astCompArray[i]);
}
#endif

		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnComponentAudioChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		OxMediaPlay_AudioCompTable_t *pstAudioCompTable	=	NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		pstAudioCompTable	= (OxMediaPlay_AudioCompTable_t*)HxOBJECT_BIN_DATA (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy(&(pstContext->stCache.stAudioCompTable), pstAudioCompTable, sizeof(OxMediaPlay_AudioCompTable_t) );

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_COMPONENT_AUDIO_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);

#if defined(CONFIG_DEBUG)
{
			HINT32 i=0;

			HxLOG_Debug("--AUDIO TRACK LIST--\n");

			for( i=0 ; i<pstContext->stCache.stAudioCompTable.nArraySize ; i++ )
				oapi_mp_PrintComponent (eOxMP_COMPONENT_AUDIO, (OxMediaPlay_Component_t*)&pstContext->stCache.stAudioCompTable.astCompArray[i]);
}
#endif

		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnComponentSubtitleChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable	=	NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		pstSubtitleCompTable	= (OxMediaPlay_SubtitleCompTable_t*)HxOBJECT_BIN_DATA (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)pstSubtitleCompTable->nCurrIndex;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)pstSubtitleCompTable->nCurrIndex;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy(&(pstContext->stCache.stSubtitleCompTable), pstSubtitleCompTable, sizeof(OxMediaPlay_SubtitleCompTable_t) );

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_COMPONENT_SUBTITLE_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);

#if defined(CONFIG_DEBUG)
{
			HINT32 i=0;

			HxLOG_Debug("--SUBTITLE TRACK LIST--\n");
			for( i=0 ; i<pstContext->stCache.stSubtitleCompTable.nArraySize ; i++ )
				oapi_mp_PrintComponent (eOxMP_COMPONENT_SUBTITLE, (OxMediaPlay_Component_t*)&pstContext->stCache.stSubtitleCompTable.astCompArray[i]);
}
#endif

		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnComponentDataChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		OxMediaPlay_DataCompTable_t *pstDataCompTable	=	NULL;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		pstDataCompTable	= (OxMediaPlay_DataCompTable_t*)HxOBJECT_BIN_DATA (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy(&(pstContext->stCache.stDataCompTable), pstDataCompTable, sizeof(OxMediaPlay_DataCompTable_t) );

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_COMPONENT_DATA_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnComponentSelected (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 6)
	{
		OxMediaPlay_Component_e 	 eCompType	=	0;
		HINT32	nSelectIndex	=	0, nSelectSubIndex = 0;


		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		eCompType		= (OxMediaPlay_Component_e)HxOBJECT_INT(apArgv[3]);
		nSelectIndex	= (HINT32)HxOBJECT_INT(apArgv[4]);
		nSelectSubIndex	= (HINT32)HxOBJECT_INT(apArgv[5]);	// SelectSubIndex 는 Audio 일때만 사용됨.subtitle, video은 사용 안함..(-1)

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)eCompType;
			stNotifyData.stEventNotify.ullParam2	= (HUINT64)nSelectIndex;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)eCompType;
			stNotifyData.stEventNotify.ulParam2	= (HUINT32)nSelectIndex;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			switch( eCompType )
			{
				case eOxMP_COMPONENT_VIDEO:
					pstContext->stCache.stVideoCompTable.nCurrIndex	=	nSelectIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d \n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache.stVideoCompTable.nCurrIndex);
					break;
				case eOxMP_COMPONENT_AUDIO:
					pstContext->stCache.stAudioCompTable.nCurrIndex	=	nSelectIndex;
					pstContext->stCache.stAudioCompTable.nCurrSubIndex	=	nSelectSubIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d , nCurrSubIndex = %d\n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache.stAudioCompTable.nCurrIndex, pstContext->stCache.stAudioCompTable.nCurrSubIndex);
					break;
				case eOxMP_COMPONENT_SUBTITLE:
					pstContext->stCache.stSubtitleCompTable.nCurrIndex	=	nSelectIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d \n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache.stSubtitleCompTable.nCurrIndex);
					break;
				case eOxMP_COMPONENT_DATA:
					pstContext->stCache.stDataCompTable.nCurrIndex		=	nSelectIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d \n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache.stDataCompTable.nCurrIndex);
					break;
				default:
					break;
			}


			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_COMPONENT_SELECTED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnComponentUnselected (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 6)
	{
		OxMediaPlay_Component_e 	 eCompType	=	0;
		HINT32	nUnselectIndex	=	0, nUnselectSubIndex = 0;


		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		eCompType		= (OxMediaPlay_Component_e)HxOBJECT_INT(apArgv[3]);
		nUnselectIndex	= (HINT32)HxOBJECT_INT(apArgv[4]);
		nUnselectSubIndex	= (HINT32)HxOBJECT_INT(apArgv[5]);	// SelectSubIndex 는 Audio 일때만 사용됨.subtitle, video은 사용 안함..(-1)

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)eCompType;
			stNotifyData.stEventNotify.ullParam2	= (HUINT64)nUnselectIndex;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)eCompType;
			stNotifyData.stEventNotify.ulParam2	= (HUINT32)nUnselectIndex;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			switch( eCompType )
			{
				case eOxMP_COMPONENT_VIDEO:
					pstContext->stCache.stVideoCompTable.nCurrIndex	=	nUnselectIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d \n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache.stVideoCompTable.nCurrIndex);
					break;
				case eOxMP_COMPONENT_AUDIO:
					pstContext->stCache.stAudioCompTable.nCurrIndex	=	nUnselectIndex;
					pstContext->stCache.stAudioCompTable.nCurrSubIndex	=	nUnselectSubIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d , nCurrSubIndex = %d\n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache.stAudioCompTable.nCurrIndex, pstContext->stCache.stAudioCompTable.nCurrSubIndex);
					break;
				case eOxMP_COMPONENT_SUBTITLE:
					pstContext->stCache.stSubtitleCompTable.nCurrIndex	=	nUnselectIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d \n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache.stSubtitleCompTable.nCurrIndex);
					break;
				case eOxMP_COMPONENT_DATA:
					pstContext->stCache.stDataCompTable.nCurrIndex		=	nUnselectIndex;
					HxLOG_Debug("[%s][%d] 2. ulViewId=%d, ulSessionId=%x, eMediaType=%d, eCompType=%d, nCurrIndex=%d \n",
						__FUNCTION__, __LINE__, ulViewId, ulSessionId, eMediaType, eCompType, pstContext->stCache.stDataCompTable.nCurrIndex);
					break;
				default:
					break;
			}


			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_COMPONENT_UNSELECTED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnTTXUpdated (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_TTX_UPDATE, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnDrmBlocked (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		OxMediaPlay_DrmRightsError_t	*pstDrmRightsError;
//		HINT32		nBlocked		=	0;
//		HINT32		nRating 		=	0;
//		HINT32		nDrmSystemId	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		pstDrmRightsError = (OxMediaPlay_DrmRightsError_t*)HxOBJECT_BIN_DATA(apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_DRM_BLOCK, (OxMediaPlay_NotifierData_t *)pstDrmRightsError, TRUE);
		}
	}

#if 0
	if (nArgc == 6)
	{
		HINT32		nBlocked		=	0;
		HINT32		nRating 		=	0;
		HINT32		nDrmSystemId	=	0;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
			nBlocked 		= (HINT32)HxOBJECT_INT (apArgv[3]);
			nRating 		= (HINT32)HxOBJECT_INT (apArgv[4]);
			nDrmSystemId 	= (HINT32)HxOBJECT_INT (apArgv[5]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)nBlocked;
			stNotifyData.stEventNotify.ullParam2	= (HUINT64)nRating;
			stNotifyData.stEventNotify.ullParam3	= (HUINT64)nDrmSystemId;

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.nBlocked		=	nBlocked;
			pstContext->stCache.nRating			=	nRating;
			pstContext->stCache.nDrmSystemId	=	nDrmSystemId;

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_DRM_BLOCK, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}
#endif
	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnPinCtrlChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			//

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_PinCtrlChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnResourceTaken (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.eType = eMediaType;
			OAPI_MP_SEM_RELEASE;

			HxLOG_Warning("sizeof(OxMediaPlay_EventNotify_t) = %d\n", sizeof(OxMediaPlay_EventNotify_t));

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_RESOURCE_TAKEN, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnResourceLost (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			oapi_mp_CleanCache(pstContext, eMediaType);

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_RESOURCE_LOST, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnRadioBGChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		HBOOL	bRadigBG	=	FALSE;

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
			bRadigBG	= (HBOOL)HxOBJECT_INT (apArgv[3]);
		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)bRadigBG;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= (HUINT32)bRadigBG;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.bRadigBG	=	bRadigBG;

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_RADIOBG_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnVideoStreamInfo	(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 						ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 		eMediaType;
	OxMediaPlay_VideoStreamInfo_t	stVideoStreamInfo;

	oapiMpContext_t 				*pstContext =	NULL;

	HxLOG_Debug("\n\n****************************** %s ***************************************\n\n", __FUNCTION__);
	if (nArgc == 4)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		stVideoStreamInfo = *(OxMediaPlay_VideoStreamInfo_t *)HxOBJECT_BIN_DATA(apArgv[3]);

		HxLOG_Debug("\t\t\tH size : %d\n", stVideoStreamInfo.ulHSize);
		HxLOG_Debug("\t\t\tV size : %d\n", stVideoStreamInfo.ulVSize);
		HxLOG_Debug("\t\t\tAR : %d\n", stVideoStreamInfo.eAspectRatio);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OAPI_MP_SEM_GET;

			/* CACHE UPDATE */
			pstContext->stCache.stVideoStreamInfo = stVideoStreamInfo;

			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_VIDEOSTREAMINFO, (OxMediaPlay_NotifierData_t *)&stVideoStreamInfo, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnAudioClipCompleted	(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 						 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 		 eMediaType;
	HUINT32							 ulRequestId;
	oapiMpContext_t 				*pstContext =	NULL;

	if (nArgc == 4)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ulRequestId = (HUINT32)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)ulRequestId;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= ulRequestId;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_AUDIOCLIPCOMPLETED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbCcTrickRestrictModeChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;
	HUINT32					 uTrickRestrictMode;
#if defined(CONFIG_OP_SES)
		HUINT32 				 ulStoragRright;
#endif

	oapiMpContext_t 			*pstContext =	NULL;

#if defined(CONFIG_OP_SES)
	if (nArgc == 5)
#else
	if (nArgc == 4)
#endif

	{

		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		uTrickRestrictMode	= HxOBJECT_INT (apArgv[3]);
#if defined(CONFIG_OP_SES)
		ulStoragRright	= HxOBJECT_INT (apArgv[4]);
#endif

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
#if defined(CONFIG_OP_SES)//defined(CONFIG_DRM_TEST)
			/*
			HD02-02000 : eMediaType == eOxMP_MEDIATYPE_TSRPLAYBACK 인경우 ulStoragRright == eDxCopyrightControl_SesStorageRight_NoMore90Min 인경우
						*  MSG 68
			HD02-02009 : eMediaType == eOxMP_MEDIATYPE_PVRPLAYBACK 인경우 ulStoragRright == eDxCopyrightControl_SesStorageRight_ExpireSameDay 인경우
																								eDxCopyrightControl_SesStorageRight_Expire3Day 인경우
																								eDxCopyrightControl_SesStorageRight_Expire7Day 인경우
																								eDxCopyrightControl_SesStorageRight_Expire60Day 인경우
						참고 : 실제 web에서 계산시에는 다음날 새벽 03:00 까지의 시간으로 다시 계산한다.
					       * 현재시간 < Expiration Time : MSG 67 + Expiration Time
					       * 현재 시간 >= Expiration Time : MSG 60
			*/

			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)uTrickRestrictMode;
			stNotifyData.stEventNotify.ullParam2	= (HUINT64)ulStoragRright;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= uTrickRestrictMode;
			stNotifyData.stEventNotify.ulParam2	= ulStoragRright;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			HxLOG_Info("============> uTrickRestrictMode = 0x%x\n", uTrickRestrictMode);
			HxLOG_Info("============> ulStoragRright = 0x%x\n", ulStoragRright);

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy( &(pstContext->stCache.uTrickRestrictMode), &uTrickRestrictMode, sizeof(DxCopyrightControl_TrickRestrictMode_u));
			HxSTD_MemCopy( &(pstContext->stCache.eStorageRight), &ulStoragRright, sizeof(eDxCopyrightControl_SesStorageRight_e));
			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_TrickRestrictModeChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
#else
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)uTrickRestrictMode;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= uTrickRestrictMode;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			HxSTD_MemCopy( &(pstContext->stCache.uTrickRestrictMode), &uTrickRestrictMode, sizeof(DxCopyrightControl_TrickRestrictMode_u));
			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_TrickRestrictModeChanged, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
#endif
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbHbbtvOnHdplusRedbutton (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;
	HUINT32					 	ulHdplusRedbutton;
	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 4)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ulHdplusRedbutton	= HxOBJECT_INT (apArgv[3]);
		pstContext = oapi_mp_GetContext (ulViewId);

		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= (HUINT64)ulHdplusRedbutton;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= ulHdplusRedbutton;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif
			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_NAGRA_PRM_UPDATE, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnAudioOutputRestrict (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;

	if (nArgc == 3)
	{
		ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_EventNotify_t	 stEventNotify;

#if defined(CONFIG_TIMESTAMP_64)
			stEventNotify.ullParam1	=	0;
			stEventNotify.ullParam2	=	0;
			stEventNotify.ullParam3	=	0;
#else
			stEventNotify.ulParam1 =	0;
			stEventNotify.ulParam2 =	0;
			stEventNotify.ulParam3 =	0;
#endif

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			OAPI_MP_SEM_RELEASE;

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_AudioOutputRestrict, (OxMediaPlay_NotifierData_t *)&stEventNotify, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbNetworkChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	HUINT32 					ulStartTimeOfChange, ulASODuration;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;
	DxNcdNetworkInfo_t		*pstNetworkMsg = NULL;


	if (nArgc == 6)
	{
		ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ulStartTimeOfChange	= (HUINT32)HxOBJECT_INT (apArgv[3]);
		ulASODuration	= (HUINT32)HxOBJECT_INT (apArgv[4]);
		pstNetworkMsg	= (DxNcdNetworkInfo_t *)HxOBJECT_BIN_DATA (apArgv[5]);


		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));

			HxSTD_MemCopy(pstContext->stCache.szNcdnetworkMsg, pstNetworkMsg->szNcdnetworkMsg, DxNCD_MAX_MESSAGE_LEN);

#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	=	(HUINT64)ulStartTimeOfChange;
			stNotifyData.stEventNotify.ullParam2	=	(HUINT64)ulASODuration;
			stNotifyData.stEventNotify.ullParam3	=	(HUINT64)pstContext->stCache.szNcdnetworkMsg;
#else
			stNotifyData.stEventNotify.ulParam1	=	(HUINT32)ulStartTimeOfChange;
			stNotifyData.stEventNotify.ulParam2	=	(HUINT32)ulASODuration;
			stNotifyData.stEventNotify.ulParam3	=	(HUINT32)pstContext->stCache.szNcdnetworkMsg;
#endif

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SI_NETWORK_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbNetworkUriChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;

	oapiMpContext_t 			*pstContext =	NULL;
	DxNetworkUriInfo_t		*pstNetworkUri = NULL;

	if (nArgc == 4)
	{
		ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		pstNetworkUri	= (DxNetworkUriInfo_t *)HxOBJECT_BIN_DATA (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));
                        
                        HxSTD_MemCopy(pstContext->stCache.szNetworkUriPrefix, pstNetworkUri->szNetworkUriPrefix, DxMAX_URI_PREFIX_INFO_LEN);
#if defined(CONFIG_TIMESTAMP_64)			
                        stNotifyData.stEventNotify.ullParam1    = (HUINT64)pstContext->stCache.szNetworkUriPrefix;
                        stNotifyData.stEventNotify.ullParam2    = 0;
                        stNotifyData.stEventNotify.ullParam3    = 0;
#else
                        stNotifyData.stEventNotify.ulParam1     = (HUINT32)pstContext->stCache.szNetworkUriPrefix;
                        stNotifyData.stEventNotify.ulParam2     = 0;
                        stNotifyData.stEventNotify.ulParam3     = 0;
#endif

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_SI_NETWORK_URI_CHANGED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}
STATIC HERROR oapi_mp_CbOnTsrPaused (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	 eMediaType;
	OxMediaPlay_TsrPaused_e		 ePausedReason;
	oapiMpContext_t 			*pstContext =	NULL;
	OxMediaPlay_Bookmark_t	*pstBookmark;
	HUINT32					i;

	if (nArgc == 5)
	{
		ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType		= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ePausedReason	= (OxMediaPlay_TsrPaused_e)HxOBJECT_INT (apArgv[3]);
		pstBookmark		= (OxMediaPlay_Bookmark_t *)HxOBJECT_BIN_DATA (apArgv[4]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));

			OAPI_MP_SEM_GET;

			/* CACHE UPDATE */
			pstContext->stCache.stTsr.bPaused = TRUE;
			pstContext->stCache.stTsr.ePausedReason = ePausedReason;

			for (i=0; i<OxMP_PVR_BOOKMARK_MAX; i++)
			{
				/* empty or same one exists */
				if (HxSTD_StrEmpty(pstContext->stCache.stTsr.stPausedPoint[i].szName)
					||pstContext->stCache.stTsr.stPausedPoint[i].ulPos == pstBookmark->ulPos)
				{
					HxSTD_MemCopy( &(pstContext->stCache.stTsr.stPausedPoint[i]), pstBookmark, sizeof(OxMediaPlay_Bookmark_t));
					break;
				}
			}
			OAPI_MP_SEM_RELEASE;

#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_TSR_PAUSED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_CbOnTsrResumed (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 					ulViewId, ulSessionId;
	OxMediaPlay_MediaType_e 	eMediaType;
	OxMediaPlay_TsrPaused_e		ePausedReason;
	oapiMpContext_t 			*pstContext = NULL;

	if (nArgc == 4)
	{
		ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
		ulSessionId 	= (HUINT32)HxOBJECT_INT (apArgv[1]);
		eMediaType		= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[2]);
		ePausedReason	= (OxMediaPlay_TsrPaused_e)HxOBJECT_INT (apArgv[3]);

		pstContext = oapi_mp_GetContext (ulViewId);
		if (NULL != pstContext)
		{
			OxMediaPlay_NotifierData_t	stNotifyData;

			HxSTD_memset(&stNotifyData, 0, sizeof(OxMediaPlay_NotifierData_t));

			(void)ePausedReason;

			OAPI_MP_SEM_GET;
			/* CACHE UPDATE */
			pstContext->stCache.stTsr.bPaused = FALSE;
			pstContext->stCache.stTsr.ePausedReason = eTsrPaused_None;
			OAPI_MP_SEM_RELEASE;

#if defined(CONFIG_TIMESTAMP_64)
			stNotifyData.stEventNotify.ullParam1	= 0;
			stNotifyData.stEventNotify.ullParam2	= 0;
			stNotifyData.stEventNotify.ullParam3	= 0;
#else
			stNotifyData.stEventNotify.ulParam1	= 0;
			stNotifyData.stEventNotify.ulParam2	= 0;
			stNotifyData.stEventNotify.ulParam3	= 0;
#endif

			oapi_mp_NotifyToListener (pstContext, ulSessionId, eMediaType, eOxMP_EVENT_TSR_RESUMED, (OxMediaPlay_NotifierData_t *)&stNotifyData, TRUE);
		}
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_MM_MEDIA)
STATIC void oapi_mp_registerNotifyCallbacks_media(void)
{
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnProbeErrored			, NULL, NULL, oapi_mp_CbOnProbeErrored);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnStopErrored			, NULL, NULL, oapi_mp_CbOnStopErrored);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBufferStateChanged		, "iiii", NULL, oapi_mp_CbOnBufferStateChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBufferInfoChanged 		, "iiib", NULL, oapi_mp_CbOnBufferInfoChanged);
#if defined(CONFIG_TIMESTAMP_64)
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayInfo				, "iiilli", NULL, oapi_mp_CbOnPlayInfo);
#else
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayInfo				, "iiiiii", NULL, oapi_mp_CbOnPlayInfo);
#endif
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPumpIndexChanged		, "iiiii", NULL, oapi_mp_CbOnPumpIndexChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSeekReady 				, "iii", NULL, oapi_mp_CbOnSeekReady);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioCodecSwitched	, "iii", NULL, oapi_mp_CbAudioCodecSwitched);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioFlushCompleted	, "iii", NULL, oapi_mp_CbAudioFlushCompleted);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioRestartCompleted	, "iii", NULL, oapi_mp_CbAudioRestartCompleted);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioFadeCompleted	, "iii", NULL, oapi_mp_CbAudioFadeCompleted);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioFadeFailed		, "iii", NULL, oapi_mp_CbAudioFadeFailed);
	HLIB_RPC_RegisterNotifyCallback	(s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioDecoderVolumeChanged,	"iiii", NULL, oapi_mp_CbAudioDecoderVolumeChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSBTLUpdate				, "iiib", NULL, oapi_mp_CbOnSBTLUpdate);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResourceMapChanged		, "iiiib", NULL, oapi_mp_CbOnResourceMapChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVmxDrmStatusChanged	, "iii", NULL, oapi_mp_CbOnVmxDrmStatusChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVmxDrmErrorChanged	, "iii", NULL, oapi_mp_CbOnVmxDrmErrorChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleSyncTime 	, "ii", NULL, oapi_mp_CbOnSubtitleSyncTime);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleFontSize 	, "ii", NULL, oapi_mp_CbOnSubtitleFontSize);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleTextPosition, "ii", NULL, oapi_mp_CbOnSubtitleTextPosition);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBufferingScenario	, "i", NULL, oapi_mp_CbOnBufferingSecenario);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnDrmLicenseData		, "i", NULL, oapi_mp_CbOnDrmLicenseData);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBrowserUserAgentData, "i", NULL, oapi_mp_CbOnBrowseruserAgentData);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnDuration 			, "i", NULL, oapi_mp_CbOnDuration);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayerType			, "i", NULL, oapi_mp_CbOnPlayerType);

#ifdef CONFIG_SUPPORT_IMMA
	#if defined(CONFIG_TIMESTAMP_64)
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayDurationChanged			, "iiili", NULL, oapi_mp_CbOnPlayDurationChanged);
	#else
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayDurationChanged			, "iiiii", NULL, oapi_mp_CbOnPlayDurationChanged);
	#endif
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAvailabilityStartTimeChanged	, "iiis", NULL, oapi_mp_CbOnAvailabilityStartTimeChanged);
#endif

	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentUnselected 		, "iiiiii", NULL, oapi_mp_CbOnComponentUnselected);

}
#endif

#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
STATIC void oapi_mp_registerNotifyCallbacks_mheg(void)
{
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegMonStarted		, "iii", NULL, oapi_mp_CbOnMhegMonStarted);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegMonStopped		, "iii", NULL, oapi_mp_CbOnMhegMonStopped);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegDmxChanged		, "iiii", NULL, oapi_mp_CbOnMhegDmxChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegPmtChanged		, "iiiibbbi", NULL, oapi_mp_CbOnMhegPmtChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegNDTChanged		, "iiii", NULL, oapi_mp_CbOnMhegNDTChanged);
}
#endif

STATIC void	oapi_mp_OnDefaultSupportedSpeeds(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	oapiMpContext_t	*pstContext = (oapiMpContext_t *)userdata;
	if( pstContext == NULL )
	{
		HxLOG_Warning("pstContext is null!! \n");
		return;
	}

	if (nArgc == 2)
	{
		HUINT32 	ulNumSpeed = (HUINT32)HxOBJECT_INT (apArgv[0]);
		HINT32		*pSpeeds = (HINT32*)HxOBJECT_BIN_DATA (apArgv[1]);

		pstContext->stCache.ulNumSpeed	=	ulNumSpeed;
		HxSTD_MemCopy( &(pstContext->stCache.anSupportSpeeds), pSpeeds, sizeof(HINT32) * ulNumSpeed);

#if defined(CONFIG_DEBUG)
		{
			HINT32 i = 0;
			HxLOG_Debug ("Get Default Supported Speeds. ulNumSpeed( %d )\n", ulNumSpeed);
			for( ; i < ulNumSpeed ; ++i )
			{
				HxLOG_Debug("Speed( %d )\n", pSpeeds[i]);
			}
		}
#endif
	}

}

STATIC HERROR oapi_mp_CbOnActive (HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32				 nCnt, nViewNum;
	HINT32				 nMpAppId;
	HUINT32				 ulMainViewId;
	OxMediaPlay_Cache_t		*pstMainViewCache	=	NULL;

	if (TRUE == s_bOapiMp_Inited)
		return ERR_OK;

	nMpAppId = HxOBJECT_INT (apArgv[0]);
	nViewNum = HxOBJECT_INT (apArgv[1]);
	ulMainViewId		=	HxOBJECT_INT (apArgv[2]);
	pstMainViewCache	=	HxOBJECT_BIN_DATA(apArgv[3]);

	if (nViewNum > 0)
	{
		oapiMpContext_t			*pstContextArray = (oapiMpContext_t *)HLIB_STD_MemCalloc (sizeof(oapiMpContext_t) * nViewNum);

		if (NULL != pstContextArray)
		{
			for (nCnt = 0; nCnt < nViewNum; nCnt++)
			{
				oapiMpContext_t 	*pstContext = &(pstContextArray[nCnt]);

				pstContext->ulMpAppId		= (HUINT32)nMpAppId;
				pstContext->ulViewId		= (HUINT32)nCnt;
				pstContext->ulSessionId		= 0x0001;
				pstContext->bReadyAitListener = FALSE;
			}

			s_ulOapiMp_ViewNum		= (HUINT32)nViewNum;
			s_ulOapiMp_MainViewId	= ulMainViewId;
			s_pstOapiMp_Context		= pstContextArray;

			if( NULL != pstMainViewCache )
			{
				OAPI_MP_SEM_GET;

				/*MainView의 Cache 정보 Sync*/
				HxSTD_MemCopy(&(s_pstOapiMp_Context[ulMainViewId].stCache), pstMainViewCache, sizeof(OxMediaPlay_Cache_t) );

				OAPI_MP_SEM_RELEASE;
			}
		}
	}

	// Activate the notifiers:

	/* SETTER */
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMainViewChanged		, "i", NULL, oapi_mp_CbOnMainViewChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSREnable			, "ii", NULL, oapi_mp_CbOnTSREnable);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleEnable		, "ii", NULL, oapi_mp_CbOnSubtitleEnable);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVideoHideEnable		, "ii", NULL, oapi_mp_CbOnVideoHideEnable);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVideoSize			, "ib", NULL, oapi_mp_CbOnVideoSize);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPigRect				, "ib", NULL, oapi_mp_CbOnPigRect);

	/* EVENT */
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnLocked				, "iiii", NULL, oapi_mp_CbOnLocked);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnNoSignal				, "iiii", NULL, oapi_mp_CbOnNoSignal);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAntennaOk			, "iiii", NULL, oapi_mp_CbOnAntennaOk);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAntennaNok			, "iiii", NULL, oapi_mp_CbOnAntennaNok);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSvcChanged			, "iiii", NULL, oapi_mp_CbOnSvcChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSvcStopped			, "iiii", NULL, oapi_mp_CbOnSvcStopped);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnConflictChanged		, "iiib", NULL, oapi_mp_CbOnConflictChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnEventRelay				, "iiii", NULL, oapi_mp_CbOnEventRelay);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnEws					, "iiii", NULL, oapi_mp_CbOnEws);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBsCtrlStarted 			, "iiii", NULL, oapi_mp_CbOnBsCtrlStarted);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPinCtrlChangedByBCAS	, "iiii", NULL, oapi_mp_CbOnPinCtrlChangedByBCAS);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSRTimecodeChanged	, "iiiiii", NULL, oapi_mp_CbOnTSRTimecodeChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnViewStateChanged		, "iiii", NULL, oapi_mp_CbOnViewStateChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnLockStateChanged		, "iiii", NULL, oapi_mp_CbOnLockStateChanged);

	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBof						, "iii", NULL, oapi_mp_CbOnBof);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnEof						, "iii", NULL, oapi_mp_CbOnEof);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnStarted					, "iiib", NULL, oapi_mp_CbOnStarted);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnStopped					, "iii", NULL, oapi_mp_CbOnStopped);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlaying					, "iii", NULL, oapi_mp_CbOnPlaying);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPaused					, "iii", NULL, oapi_mp_CbOnPaused);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTricked					, "iii", NULL, oapi_mp_CbOnTricked);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayStateChanged		, "iiii", NULL, oapi_mp_CbOnPlayStateChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayErrorChanged		, "iiiii", NULL, oapi_mp_CbOnPlayErrorChanged);
#if defined(CONFIG_TIMESTAMP_64)
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayTimeChanged			, "iiill", NULL, oapi_mp_CbOnPlayTimeChanged);
#else
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayTimeChanged			, "iiiii", NULL, oapi_mp_CbOnPlayTimeChanged);
#endif
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSpeedChanged			, "iiii", NULL, oapi_mp_CbOnSpeedChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnRatingChanged 			, "iiiiii", NULL, oapi_mp_CbOnRatingChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVStarted 				, "iii", NULL, oapi_mp_CbOnAVStarted);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVStopped 				, "iii", NULL, oapi_mp_CbOnAVStopped);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVResume 				, "iii", NULL, oapi_mp_CbOnAVResume);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVUnderrun 				, "iii", NULL, oapi_mp_CbOnAVUnderrun);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVMediaRectChanged		, "iii", NULL, oapi_mp_CbOnAVMediaRectChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSupportedSpeedChanged 	, "iiiib", NULL, oapi_mp_CbOnSupportedSpeedChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiPMTChanged			, "iii", NULL, oapi_mp_CbOnSiPMTChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiEITChanged			, "iii", NULL, oapi_mp_CbOnSiEITChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiSITChanged			, "iii", NULL, oapi_mp_CbOnSiSITChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiAITChanged			, "iiibb", NULL, oapi_mp_CbOnSiAITChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiRCTChanged			, "iiiib", NULL, oapi_mp_CbOnSiRCTChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiRCTRequestImageIcon	, "iiis", NULL, oapi_mp_CbOnSiRCTRequestImageIcon);

	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentVideoChanged 	, "iiib", NULL, oapi_mp_CbOnComponentVideoChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentAudioChanged 	, "iiib", NULL, oapi_mp_CbOnComponentAudioChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentSubtitleChanged, "iiib", NULL, oapi_mp_CbOnComponentSubtitleChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentDataChanged	, "iiib", NULL, oapi_mp_CbOnComponentDataChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentSelected 		, "iiiiii", NULL, oapi_mp_CbOnComponentSelected);

	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTTXUpdated				, "iii", NULL, oapi_mp_CbOnTTXUpdated);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnDrmBlocked				, "iiib", NULL, oapi_mp_CbOnDrmBlocked);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPinCtrlChanged			, "iii", NULL, oapi_mp_CbOnPinCtrlChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResourceTaken 			, "iii", NULL, oapi_mp_CbOnResourceTaken);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResourceLost			, "iii", NULL, oapi_mp_CbOnResourceLost);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnRadioBGChanged			, "iiii", NULL, oapi_mp_CbOnRadioBGChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVideoStreamInfo, "iiib", NULL, oapi_mp_CbOnVideoStreamInfo);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioClipCompleted, "iiii", NULL, oapi_mp_CbOnAudioClipCompleted);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioOutputRestrict 	, "iii", NULL, oapi_mp_CbOnAudioOutputRestrict);
#if defined (CONFIG_OP_SES)//defined(CONFIG_DRM_TEST)
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnCcTrickRestrictModeChanged	, "iiiii", NULL, oapi_mp_CbCcTrickRestrictModeChanged);
#else
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnCcTrickRestrictModeChanged 	, "iiii", NULL, oapi_mp_CbCcTrickRestrictModeChanged);
#endif
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiNetworkChanged 	, "iiiiib", NULL, oapi_mp_CbNetworkChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiNetworkUriChanged 	, "iiib", NULL, oapi_mp_CbNetworkUriChanged);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnHbbtvOnHdplusRedbutton 	, "iiii", NULL, oapi_mp_CbHbbtvOnHdplusRedbutton);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnRequestPincode, "iis", NULL, oapi_mp_CbOnRequestPincode);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResultPincode, "i", NULL, oapi_mp_CbOnResultPincode);

	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSRPaused, "iiiib", NULL, oapi_mp_CbOnTsrPaused);
	HLIB_RPC_RegisterNotifyCallback (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSRResumed, "iiii", NULL, oapi_mp_CbOnTsrResumed);

#if defined(CONFIG_MW_MM_MEDIA)
	oapi_mp_registerNotifyCallbacks_media();
#endif

#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
	oapi_mp_registerNotifyCallbacks_mheg();
#endif

	// Get Default Supported Speeds
	for(nCnt = 0; nCnt < nViewNum; ++nCnt)
	{
		(void)HLIB_RPC_Request (s_nRpcHandle, (HxRPC_Response_t)oapi_mp_OnDefaultSupportedSpeeds, (void *)&s_pstOapiMp_Context[nCnt], RPC_OAPI_MEDIA_PLAY_GetDefaultSupportedSpeed, "i", 0);
	}

	/* init callback */
	s_pfActivated();
	s_bOapiMp_Inited = TRUE;

	return ERR_OK;
}


static void	oapi_mp_OnOsdDisplayPoint (HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	HINT32	*pScaledValue;
	//HxReturnFunc_t	ret;	//UNUSED

	pScaledValue = (HINT32 *)userdata;
	if (pScaledValue == NULL)
		return;

	if (nArgc == 2)
	{
		pScaledValue[0]	= (HINT32)HxOBJECT_INT (apArgv[0]);
		pScaledValue[1]	= (HINT32)HxOBJECT_INT (apArgv[1]);
	}
}

STATIC HERROR oapi_mp_MhegCountRawSection (HUINT32 ulViewId, OxMediaPlay_MHEG_TableType_e eTableType, HUINT32 *pulSecNum)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pulSecNum)						{ return ERR_FAIL; }

	switch (eTableType)
	{
		case eOxMP_MHEG_PAT:
			*pulSecNum = pstContext->stCache.stMhegSecData.ulPatSecCount;
			break;

		case eOxMP_MHEG_PMT:
			*pulSecNum = pstContext->stCache.stMhegSecData.ulPmtSecCount;
			break;

		default:
			*pulSecNum = 0;
			HxLOG_Error("Invalid table type!! \n");
			return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR oapi_mp_MhegGetRawSection (HUINT32 ulViewId, OxMediaPlay_MHEG_TableType_e eTableType, HUINT32 ulIndex, HUINT8 **ppucRawSec, HUINT32 *ulSecLen)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);
	HUINT8				*pucRawSec = NULL;
	HUINT8				*pucCachedRawSec = NULL;
	HUINT32				 ulSectionLen = 0;

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == ppucRawSec)						{ return ERR_FAIL; }
	if (NULL == ulSecLen)						{ return ERR_FAIL; }

	*ppucRawSec 	= NULL;
	*ulSecLen 		= 0;

	if (ulIndex >= OxMP_PAT_SECTION_NUM_MAX)
	{
		HxLOG_Error ("invalid index(%d) : max(%d) \n", ulIndex, OxMP_PAT_SECTION_NUM_MAX);
		return ERR_FAIL;
	}

	switch (eTableType)
	{
		case eOxMP_MHEG_PAT:
			ulSectionLen = pstContext->stCache.stMhegSecData.aulPatSecLen[ulIndex];
			pucCachedRawSec = pstContext->stCache.stMhegSecData.paucPatRawSec[ulIndex];
			break;

		case eOxMP_MHEG_PMT:
			ulSectionLen = pstContext->stCache.stMhegSecData.aulPmtSecLen[ulIndex];
			pucCachedRawSec = pstContext->stCache.stMhegSecData.paucPmtRawSec[ulIndex];
			break;

		default:
			HxLOG_Error("Invalid table type!! \n");
			return ERR_FAIL;
	}

	if (ulSectionLen > 0)
	{
		pucRawSec = (HUINT8 *)HLIB_STD_MemCalloc(ulSectionLen);
		if (pucRawSec == NULL)
		{
			HxLOG_Error ("mem alloc failed.....\n");
			return ERR_FAIL;
		}

		HxSTD_MemCopy(pucRawSec, pucCachedRawSec, ulSectionLen);

		*ppucRawSec = pucRawSec;
		*ulSecLen	= ulSectionLen;
	}

	return ERR_OK;
}


STATIC HERROR oapi_mp_MhegFreeRawSection (HUINT8 *pucRawSec)
{
	if (pucRawSec != NULL)
	{
		HLIB_STD_MemFree(pucRawSec);
	}

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___

#define _____BASE_____
HERROR OAPI_MEDIA_PLAY_Init (OxMediaPlay_ActivatedCb_t pfActivated)
{
	HERROR hErr;
	HINT32	nReqId;

	if (TRUE != s_bOapiMp_Inited)
	{
		hErr = OAPI_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
		if (ERR_OK != hErr )
		{
			HxLOG_Error("Error. \n");
			return ERR_FAIL;
		}

		s_pfActivated = pfActivated;

		HxSEMT_Create(&s_ulOxMpSema, "OxMediaPlaySvcMgr_t", 0);

		nReqId= HLIB_RPC_Request (s_nRpcHandle, (HxRPC_Response_t)oapi_mp_CbOnActive, NULL, RPC_OAPI_MEDIA_PLAY_Activate, NULL);
		if (HLIB_RPC_Join(s_nRpcHandle, nReqId, RPC_DEFAULT_TIMEOUT) != ERR_OK)
		{
			HxLOG_Error("==========> ERROR !!! [%s] has HLIB_RPC_Request timeout!!!\n\n", __FUNCTION__);
		}
	}

	return ERR_OK;
}

HUINT32 OAPI_MEDIA_PLAY_GetViewNumber (void)
{
	return s_ulOapiMp_ViewNum;
}

HERROR OAPI_MEDIA_PLAY_SetMainViewId (HUINT32 ulViewId)
{
	s_ulOapiMp_MainViewId = ulViewId;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetMainViewId, "i", ulViewId);

	return ERR_OK;
}

HUINT32 OAPI_MEDIA_PLAY_GetMainViewId (void)
{
	return s_ulOapiMp_MainViewId;
}

HERROR OAPI_MEDIA_PLAY_GetPlayType (HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	if (NULL != peType)
	{
		*peType = pstContext->stCache.eType;
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetNeedRadioBg (HUINT32 ulViewId, HBOOL *pbNeed)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pbNeed)							{ return ERR_FAIL; }

	*pbNeed	=	pstContext->stCache.bRadigBG;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);
	if( NULL == pstContext )					{ return ERR_FAIL; }

	switch(eType)
	{
		case eOxMP_MEDIATYPE_LIVE:			*pulRequestId = pstContext->stCache.stLive.ulMasterSvcUid;	break;
		case eOxMP_MEDIATYPE_PVRPLAYBACK:	*pulRequestId = pstContext->stCache.stPvrPb.ulContentId;	break;
		case eOxMP_MEDIATYPE_TSRPLAYBACK:	*pulRequestId = pstContext->stCache.stTsrPb.ulMasterSvcUid;	break;
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetListener (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfListener, void *pvUserData)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);
	oapiMpNotifier_t	*pstNotifier;

	if ((NULL == pstContext) || (NULL == pfListener))		{ return ERR_FAIL; }

	pstNotifier = oapi_mp_FindNotifier (pstContext, pfListener, pvUserData);
	if (NULL != pstNotifier)
	{
		HxLOG_Error ("Already registered...\n");
		return ERR_OK;
	}

	pstNotifier = (oapiMpNotifier_t *)HLIB_STD_MemAlloc (sizeof(oapiMpNotifier_t));
	if (NULL == pstNotifier)
	{
		HxLOG_Error ("Memory allocation failed\n");
		return ERR_FAIL;
	}

	pstNotifier->pfListener	= pfListener;
	pstNotifier->pvUserData	= pvUserData;

	pstContext->pstListener = HLIB_LIST_Append (pstContext->pstListener, (void *)pstNotifier);
	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_ResetListener (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfListener, void *pvUserData)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);
	oapiMpNotifier_t	*pstNotifier;

	if ((NULL == pstContext) || (NULL == pfListener))		{ return ERR_FAIL; }

	pstNotifier = oapi_mp_FindNotifier (pstContext, pfListener, pvUserData);
	if (NULL == pstNotifier)
	{
		HxLOG_Error ("No notifier...\n");
		return ERR_OK;
	}

	pstContext->pstListener = HLIB_LIST_Remove (pstContext->pstListener, (const void *)pstNotifier);
	HLIB_STD_MemFree (pstNotifier);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetCurrentSessionId (HUINT32 ulViewId, HUINT32 *pulSessionId)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	if (NULL != pulSessionId)					{ *pulSessionId = oapi_mp_GetSessionId (pstContext); }
	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetStreamAspectRatio (HUINT32 ulViewId, DxAspectRatio_e *peAspectRatio)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == peAspectRatio)						{ return ERR_FAIL; }

	*peAspectRatio = pstContext->stCache.stVideoStreamInfo.eAspectRatio;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetStreamVideoSize(HUINT32 ulViewId, HUINT32 *ulHSize, HUINT32 *ulVSize)
{
	oapiMpContext_t 	*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)
	{
		return ERR_FAIL;
	}

	if ( (NULL == ulHSize) || (NULL == ulVSize) )
	{
		return ERR_FAIL;
	}

	*ulHSize = pstContext->stCache.stVideoStreamInfo.ulHSize;
	*ulVSize = pstContext->stCache.stVideoStreamInfo.ulVSize;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetOsdDisplayPoint (HUINT32 ulViewId, HINT32 ulRefWidth, HINT32 ulRefHeight, HINT32 nVideoPointX, HINT32 nVideoPointY,HINT32 *pnScaledX, HINT32 *pnScaledY)
{
	HINT32	nRequestId = 0;
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	if (NULL == pnScaledX)						{ return ERR_FAIL; }
	if (NULL == pnScaledY)						{ return ERR_FAIL; }

	{
		HINT32	*pScaledValue = NULL;
		pScaledValue = (HINT32*)OxMEM_Malloc(sizeof(HINT32)*2);
		if (NULL == pScaledValue) /* fix for prevent #150739 */
			return ERR_FAIL;

		nRequestId = HLIB_RPC_Request (s_nRpcHandle, (HxRPC_Response_t)oapi_mp_OnOsdDisplayPoint, pScaledValue, RPC_OAPI_MEDIA_PLAY_GetOsdDisplayPoint, "iiiii", ulViewId, ulRefWidth, ulRefHeight, nVideoPointX, nVideoPointY);
		if (HLIB_RPC_Join(s_nRpcHandle, nRequestId, 10000) == ERR_FAIL)
		{
			OxMEM_Free(pScaledValue);
			return ERR_FAIL;
		}

		*pnScaledX = pScaledValue[0];
		*pnScaledY = pScaledValue[1];

		OxMEM_Free(pScaledValue);
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetDefaultSupportedSpeeds (HUINT32 ulViewId, HBOOL bScrambled)
{
	HINT32	nRequestId = 0;
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	nRequestId = HLIB_RPC_Request (s_nRpcHandle, (HxRPC_Response_t)oapi_mp_OnDefaultSupportedSpeeds, (void *)pstContext, RPC_OAPI_MEDIA_PLAY_GetDefaultSupportedSpeed, "i", bScrambled);
	if (HLIB_RPC_Join(s_nRpcHandle, nRequestId, 10000) == ERR_FAIL)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____FOR_MHEG_____
HERROR	OAPI_MEDIA_PLAY_CaptureStillPicture (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT32 ulCaptureWidth, HUINT32 ulCaptureHeight, DxPixelFormat_e ePixelFormat, HUINT32 ulBufferSize, HUINT8 *pBuffer)
{
	if (NULL == pBuffer)
		return ERR_FAIL;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_CaptureStillPicture, "iiiiiib", ulViewId, eVideoCodec, ulCaptureWidth, ulCaptureHeight, ePixelFormat, ulBufferSize, pBuffer, ulBufferSize);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_StartStillPicture (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT32 ulBufferSize, HUINT8 *pBuffer)
{
	if (NULL == pBuffer)
		return ERR_FAIL;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_StartStillPicture, "iiib", ulViewId, eVideoCodec, ulBufferSize, pBuffer, ulBufferSize);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_StopStillPicture (HUINT32 ulViewId)
{
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_StopStillPicture, "i", ulViewId);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_StartAudioClip (HUINT32 ulRequestId, HUINT8 *pucBuffer, HUINT32 ulBufferBytes, DxAudioCodec_e eCodec, HUINT32 ulLoopCnt)
{
	if (NULL == pucBuffer)
		return ERR_FAIL;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_StartAudioClip, "iiiib", ulRequestId, ulBufferBytes, eCodec, ulLoopCnt, pucBuffer, ulBufferBytes);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_PauseAudioClip(HUINT32 ulRequestId)
{
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_PauseAudioClip, "i", ulRequestId);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_ResumeAudioClip(HUINT32 ulRequestId)
{
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_ResumeAudioClip, "i", ulRequestId);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_StopAudioClip(HUINT32 ulRequestId)
{
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_StopAudioClip, "i", ulRequestId);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_SetVideoFreeze(HUINT32 ulViewId, HBOOL bFreeze)
{
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetVideoFreeze, "ii", ulViewId, bFreeze);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_SetAudioFreeze(HUINT32 ulViewId, HBOOL bFreeze)
{
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetAudioFreeze, "ii", ulViewId, bFreeze);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetMhegStatus (OxMediaPlay_MhegStatus_e eStatus)
{
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetMhegStatus, "i", eStatus);
	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetMhegInterruptibleFlag (HUINT32 ulViewId, HBOOL bMhegInterruptible)
{
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetMhegInterruptibleFlag, "i", bMhegInterruptible);
	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_RequestPincode (HUINT32 ulViewId, OxMediaPlay_MediaType_e eMediaType, HUINT8 *pszMsg)
{
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_RequestPincode, "iis", ulViewId, eMediaType, pszMsg);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_ResultPincode (HBOOL bPermit)
{
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_ResultPincode, "i", bPermit);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_StopHbbtvMonitoring (HUINT32 ulViewId)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_StopHbbtvMonitoring, "i", ulViewId);

	return ERR_OK;
}

#define _____TRACK_____
HERROR OAPI_MEDIA_PLAY_SetComponentIndexEx (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, HUINT8 *pstrLang)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);
	HBOOL bSendRPC = FALSE;

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pstrLang)						{ return ERR_FAIL; }

	HxLOG_Debug("ulViewId[%d] eCompType[%d] nCompIndex[%d]\n", ulViewId, eCompType, nCompIndex);
	switch( eCompType )
	{
		case eOxMP_COMPONENT_AUDIO:
			if( pstContext->stCache.stAudioCompTable.nArraySize < nCompIndex )
			{
				HxLOG_Error("out of audio track - index(%d) \n", nCompIndex );
				return ERR_FAIL;
			}

			/* compare language in track */
			{
				HINT32 i;
				OxMediaPlay_AudioComp_t *pstTrack = &(pstContext->stCache.stAudioCompTable.astCompArray[nCompIndex]);
				if (NULL == pstTrack)
					return ERR_FAIL;

				for (i=0 ; i<pstTrack->nLangNum ; i++)
				{
					if (HLIB_STD_StrCaseCmp(pstTrack->astLangInfo[i].szLangCode, pstrLang) == 0)
					{
						OAPI_MP_SEM_GET;
						pstContext->stCache.stAudioCompTable.nCurrIndex =	nCompIndex;
						HxLOG_Debug("ulViewId[%d] eCompType[%d] nCurrIndex[%d]\n", ulViewId, eCompType, pstContext->stCache.stAudioCompTable.nCurrIndex);
						OAPI_MP_SEM_RELEASE;

						bSendRPC = TRUE;
					}
				}
			}
			break;

		case eOxMP_COMPONENT_DATA:
		case eOxMP_COMPONENT_VIDEO:
		case eOxMP_COMPONENT_SUBTITLE:
		default:
				HxLOG_Error("This function is not available track !\n");
				return ERR_FAIL;
			break;
	}

	if (bSendRPC)
		HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetComponentIndexEx, "iiiis", oapi_mp_GetSessionId (pstContext), ulViewId, eCompType, nCompIndex, pstrLang);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	HxLOG_Debug("ulViewId[%d] eCompType[%d] nCompIndex[%d]\n", ulViewId, eCompType, nCompIndex);
	switch( eCompType )
	{
		case eOxMP_COMPONENT_VIDEO:
			if( pstContext->stCache.stVideoCompTable.nArraySize < nCompIndex )
			{
				HxLOG_Error("out of video track - index(%d) \n", nCompIndex );
				return ERR_FAIL;
			}

			OAPI_MP_SEM_GET;
			pstContext->stCache.stVideoCompTable.nCurrIndex	=	nCompIndex;
			OAPI_MP_SEM_RELEASE;

			break;
		case eOxMP_COMPONENT_AUDIO:
			if( pstContext->stCache.stAudioCompTable.nArraySize < nCompIndex )
			{
				HxLOG_Error("out of audio track - index(%d) \n", nCompIndex );
				return ERR_FAIL;
			}

			OAPI_MP_SEM_GET;
			pstContext->stCache.stAudioCompTable.nCurrIndex	=	nCompIndex;
			HxLOG_Debug("ulViewId[%d] eCompType[%d] nCurrIndex[%d]\n", ulViewId, eCompType, pstContext->stCache.stAudioCompTable.nCurrIndex);
			OAPI_MP_SEM_RELEASE;

			break;

		case eOxMP_COMPONENT_SUBTITLE:
			if( pstContext->stCache.stSubtitleCompTable.nArraySize < nCompIndex )
			{
				HxLOG_Error("out of subtitle track - index(%d) \n", nCompIndex );
				return ERR_FAIL;
			}

			OAPI_MP_SEM_GET;
			pstContext->stCache.stSubtitleCompTable.nCurrIndex = nCompIndex;
			HxLOG_Debug("ulViewId[%d] eCompType[%d] nCurrIndex[%d]\n", ulViewId, eCompType, pstContext->stCache.stSubtitleCompTable.nCurrIndex);
			OAPI_MP_SEM_RELEASE;
			break;
		case eOxMP_COMPONENT_DATA:
			if( pstContext->stCache.stDataCompTable.nArraySize < nCompIndex )
			{
				HxLOG_Error("out of data track - index(%d) \n", nCompIndex );
				return ERR_FAIL;
			}

			OAPI_MP_SEM_GET;
			pstContext->stCache.stDataCompTable.nCurrIndex = nCompIndex;
			HxLOG_Debug("ulViewId[%d] eCompType[%d] nCurrIndex[%d]\n", ulViewId, eCompType, pstContext->stCache.stDataCompTable.nCurrIndex);
			OAPI_MP_SEM_RELEASE;
			break;
		default:
			{
				HxLOG_Error("no support track type (%d) \n", eCompType );
				return ERR_FAIL;
			}
	}

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetComponentIndex, "iiii", oapi_mp_GetSessionId (pstContext), ulViewId, eCompType, nCompIndex);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_UnsetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	HxLOG_Debug("ulViewId[%d] eCompType[%d] nCompIndex[%d]\n", ulViewId, eCompType, nCompIndex);
	switch( eCompType )
	{
		case eOxMP_COMPONENT_VIDEO:
			HxLOG_Debug("eOxMP_COMPONENT_VIDEO, ulViewId[%d] eCompType[%d] nCompIndex[%d]\n", ulViewId, eCompType, nCompIndex);
			break;
		case eOxMP_COMPONENT_AUDIO:
			HxLOG_Debug("eOxMP_COMPONENT_AUDIO, ulViewId[%d] eCompType[%d] nCompIndex[%d]\n", ulViewId, eCompType, nCompIndex);
			break;
		case eOxMP_COMPONENT_SUBTITLE:
			break;
		default:
			{
				HxLOG_Error("no support track type (%d) \n", eCompType );
				return ERR_FAIL;
			}
	}

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_UnsetComponentIndex, "iiii", oapi_mp_GetSessionId (pstContext), ulViewId, eCompType, nCompIndex);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetComponentNum (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HUINT32* pulCompNum)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	switch( eCompType )
	{
		case eOxMP_COMPONENT_VIDEO:
			*pulCompNum = pstContext->stCache.stVideoCompTable.nArraySize;
			break;
		case eOxMP_COMPONENT_AUDIO:
			*pulCompNum = pstContext->stCache.stAudioCompTable.nArraySize;
			break;
		case eOxMP_COMPONENT_SUBTITLE:
			*pulCompNum = pstContext->stCache.stSubtitleCompTable.nArraySize;
			break;
		case eOxMP_COMPONENT_DATA:
			*pulCompNum = pstContext->stCache.stDataCompTable.nArraySize;
			break;
		default:
			{
				HxLOG_Error("no support track type (%d) \n", eCompType );
				return ERR_FAIL;
			}

	}
	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetComponent (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HUINT32 ulCompIndex, OxMediaPlay_Component_t *pstComponet)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pstComponet)					{ return ERR_FAIL; }

	HxSTD_MemSet(pstComponet, 0, sizeof(OxMediaPlay_Component_t));

	switch( eCompType )
	{
		case eOxMP_COMPONENT_VIDEO:
			if( pstContext->stCache.stVideoCompTable.nArraySize < ulCompIndex )
			{
				HxLOG_Error("out of video track - index(%d) \n", ulCompIndex );
				return ERR_FAIL;
			}

			HxSTD_MemCopy(pstComponet, &(pstContext->stCache.stVideoCompTable.astCompArray[ulCompIndex]), sizeof(OxMediaPlay_VideoComp_t));
			break;

		case eOxMP_COMPONENT_AUDIO:
			if( pstContext->stCache.stAudioCompTable.nArraySize < ulCompIndex )
			{
				HxLOG_Error("out of audio track - index(%d) \n", ulCompIndex );
				return ERR_FAIL;
			}

			HxSTD_MemCopy(pstComponet, &(pstContext->stCache.stAudioCompTable.astCompArray[ulCompIndex]), sizeof(OxMediaPlay_AudioComp_t));
			break;

		case eOxMP_COMPONENT_SUBTITLE:
			if( pstContext->stCache.stSubtitleCompTable.nArraySize < ulCompIndex )
			{
				HxLOG_Error("out of subtitle track - index(%d) \n", ulCompIndex );
				return ERR_FAIL;
			}

			HxSTD_MemCopy(pstComponet, &(pstContext->stCache.stSubtitleCompTable.astCompArray[ulCompIndex]), sizeof(OxMediaPlay_SubtitleComp_t));
			break;

		case eOxMP_COMPONENT_DATA:
			if( pstContext->stCache.stDataCompTable.nArraySize < ulCompIndex )
			{
				HxLOG_Error("out of data track - index(%d) \n", ulCompIndex );
				return ERR_FAIL;
			}

			HxSTD_MemCopy(pstComponet, &(pstContext->stCache.stDataCompTable.astCompArray[ulCompIndex]), sizeof(OxMediaPlay_DataComp_t));
			break;

		default:
			{
				HxLOG_Error("no support track type (%d) \n", eCompType );
				return ERR_FAIL;
			}

	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32* pnCompIndex, HINT32* pnCompSubIndex)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	switch( eCompType )
	{
		case eOxMP_COMPONENT_VIDEO:
			*pnCompIndex = pstContext->stCache.stVideoCompTable.nCurrIndex;
			break;
		case eOxMP_COMPONENT_AUDIO:
			*pnCompIndex = pstContext->stCache.stAudioCompTable.nCurrIndex;
			if (pnCompSubIndex != NULL)
			{
				*pnCompSubIndex = pstContext->stCache.stAudioCompTable.nCurrSubIndex;
			}
			break;

		case eOxMP_COMPONENT_SUBTITLE:
			*pnCompIndex = pstContext->stCache.stSubtitleCompTable.nCurrIndex;
			break;

		case eOxMP_COMPONENT_DATA:
			*pnCompIndex = pstContext->stCache.stDataCompTable.nCurrIndex;
			break;

		default:
			{
				HxLOG_Error("no support track type (%d) \n", eCompType );
				return ERR_FAIL;
			}

	}

	HxLOG_Debug("ulViewId[%d] eCompType[%d] *pnCompIndex[%d]\n", ulViewId, eCompType, *pnCompIndex);
	return ERR_OK;
}

#define _____PLAYER_CONTROL_____
HERROR OAPI_MEDIA_PLAY_Start (HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart, HUINT32 *pulSessionId)
{
	HUINT32				 ulSessionId = 0;
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)
	{
		return ERR_FAIL;
	}

	oapi_mp_IncreaseSessionVer (pstContext);
	ulSessionId = oapi_mp_GetSessionId (pstContext);

	if( punStart->stTsrPb.bStartPaused == TRUE )
	{
		pstContext->stCache.nSpeed = 0;
	}

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_Start, "iib", ulSessionId, ulViewId, punStart, sizeof(OxMediaPlay_StartInfo_t));

	// Starting 시킨 것을 Cache에 반영시킨다.
	// 단, FCC는 예외
	if ((eOxMP_MEDIATYPE_LIVE != punStart->eType) || (eOxMP_LIVETYPE_FCC != punStart->stLive.eLiveType))
	{
		pstContext->stCache.eType = punStart->eType;
		pstContext->stCache.eError = eOxMP_PLAYERROR_NO_ERROR;
		pstContext->stCache.eState = eOxMP_PLAYSTATE_REQUESTED;

		switch (pstContext->stCache.eType)
		{
			case eOxMP_MEDIATYPE_LIVE:
				HxSTD_MemCopy(&(pstContext->stCache.stLive), &(punStart->stLive), sizeof(OxMediaPlay_StartLive_t));
				break;

			case eOxMP_MEDIATYPE_PVRPLAYBACK:
				HxSTD_MemCopy(&(pstContext->stCache.stPvrPb), &(punStart->stPvrPb), sizeof(OxMediaPlay_StartPvrPb_t));
				break;

			case eOxMP_MEDIATYPE_TSRPLAYBACK:
				HxSTD_MemCopy(&(pstContext->stCache.stTsrPb), &(punStart->stTsrPb), sizeof(OxMediaPlay_StartTsrPb_t));
				break;

			case eOxMP_MEDIATYPE_MEDIAPLAY:
				HxSTD_MemCopy(&(pstContext->stCache.stMediaPlay), &(punStart->stMediaPlay), sizeof(OxMediaPlay_StartMediaPlay_t));
				break;

			default:
				break;
		}
	}

	if (NULL != pulSessionId)
	{
		*pulSessionId = ulSessionId;
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_Stop (HUINT32 ulViewId)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	HxLOG_Trace();
	if (NULL == pstContext)						{ return ERR_FAIL; }

	if ((eOxMP_PLAYSTATE_NONE 	  == pstContext->stCache.eState) ||
		(eOxMP_PLAYSTATE_STOPPING == pstContext->stCache.eState) ||
		(eOxMP_PLAYSTATE_STOPPED  == pstContext->stCache.eState) )
	{
		HxLOG_Error("Error> already media_play stopped!!! state(%d)\n", pstContext->stCache.eState);
		return ERR_OK;
	}
	else
	{
		if(eOxMP_PLAYSTATE_REQUESTED  == pstContext->stCache.eState)
		{
			HxLOG_Warning("already media_play requested!!! state(%d)\n", pstContext->stCache.eState);
		}
		else
		{
			HxLOG_Warning("already media_play!!! state(%d)\n", pstContext->stCache.eState);
		}
 	}

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_Stop, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, pstContext->stCache.eType);

	// stopping 시킨 것을 Cache에 반영시킨다.
	pstContext->stCache.eError = eOxMP_PLAYERROR_NO_ERROR;
	pstContext->stCache.eState = eOxMP_PLAYSTATE_STOPPING;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_StopWithParams (HUINT32 ulViewId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulSessionId) /* only for Netflix */
{
	/* TODO : Netflix NRDP suspend-timeout 문제가 해결되면 APK_MEDIA_PLAY_Stop 으로 대체되어야 함 */
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	HxLOG_Trace();
	if (NULL == pstContext)						{ return ERR_FAIL; }

	if ((eOxMP_PLAYSTATE_NONE 	  == pstContext->stCache.eState) ||
		(eOxMP_PLAYSTATE_STOPPING == pstContext->stCache.eState) ||
		(eOxMP_PLAYSTATE_STOPPED  == pstContext->stCache.eState) )
	{
		HxLOG_Error("Error> already media_play stopped!!! state(%d)\n", pstContext->stCache.eState);
		return ERR_OK;
	}
	else
	{
		if(eOxMP_PLAYSTATE_REQUESTED  == pstContext->stCache.eState)
		{
			HxLOG_Warning("already media_play requested!!! state(%d)\n", pstContext->stCache.eState);
		}
		else
		{
			HxLOG_Warning ("already media_play!!! state(%d)\n", pstContext->stCache.eState);
		}
 	}

	if (eMediaType == eOxMP_MEDIATYPE_MEDIAPLAY)
	{
		if (eMediaType != pstContext->stCache.eType)
		{
			/* Prevent to unexpected current LivePB stop by previous MediaPB (e.g. netflix playback by suspend delay)
			   in switching between last MediaPB and newly launched LivePB.*/
			/* This MediaPB stop() will be done at new LivePB start().*/
			HxLOG_Error("Error> failed to stop by unmatched playback between targetType(%d) and cacheType(%d)\n", __func__, __LINE__, eMediaType, pstContext->stCache.eType);
			return ERR_FAIL;
		}
	}

	/* Don't define addtional RPC_OAPI_MEDIA_PLAY_StopWithType */
	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_Stop, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, pstContext->stCache.eType);

	// stopping 시킨 것을 Cache에 반영시킨다.
	pstContext->stCache.eError = eOxMP_PLAYERROR_NO_ERROR;
	pstContext->stCache.eState = eOxMP_PLAYSTATE_STOPPING;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetPlaySpeed (HUINT32 ulViewId, HINT32 nSpeed)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	pstContext->stCache.nSpeed = nSpeed;
	OAPI_MP_SEM_RELEASE;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPlaySpeed, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, nSpeed);

	return ERR_OK;
}

#if defined(CONFIG_TIMESTAMP_64)
HERROR OAPI_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT64 ullPosition)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	pstContext->stCache.ullPosition = ullPosition;
	OAPI_MP_SEM_RELEASE;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPlayPosition, "iil", oapi_mp_GetSessionId (pstContext), ulViewId, ullPosition);

	return ERR_OK;
}
#else
HERROR OAPI_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT32 ulPosition)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	pstContext->stCache.ulPosition = ulPosition;
	OAPI_MP_SEM_RELEASE;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPlayPosition, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, ulPosition);

	return ERR_OK;
}
#endif

HERROR OAPI_MEDIA_PLAY_SetPlaySegment (HUINT32 ulViewId, HUINT32 ulPosSeg)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPlaySegment, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, ulPosSeg);

	return ERR_OK;
}

#define _____PLAYER_GETTER_____
HERROR OAPI_MEDIA_PLAY_GetPlaySpeed (HUINT32 ulViewId, HINT32 *pnSpeed)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pnSpeed = pstContext->stCache.nSpeed;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

#if defined(CONFIG_TIMESTAMP_64)
HERROR OAPI_MEDIA_PLAY_GetPlayPosition (HUINT32 ulViewId, HUINT64 *pullPosition)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	*pullPosition = pstContext->stCache.ullPosition;

	return ERR_OK;
}
#else
HERROR OAPI_MEDIA_PLAY_GetPlayPosition (HUINT32 ulViewId, HUINT32 *pulPosition)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	*pulPosition = pstContext->stCache.ulPosition;

	return ERR_OK;
}
#endif

HERROR OAPI_MEDIA_PLAY_GetPlayState (HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	*peState = pstContext->stCache.eState;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetPlayError (HUINT32 ulViewId, OxMediaPlay_PlayError_e *peError, OxMediaPlay_ErrorReason_e *peReason)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	if (peError)
		*peError = pstContext->stCache.eError;

	if (peReason)
		*peReason = pstContext->stCache.eReason;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetBufferedTime (HUINT32 ulViewId, HUINT32 *pulBufferred)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	*pulBufferred = pstContext->stCache.ulBufferred;
	HxLOG_Info ("*pulBufferred=(%d) \n", *pulBufferred);
	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetTsrStartTime (HUINT32 ulViewId, HUINT32 *pulTsrStartTime)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pulTsrStartTime)					{ return ERR_FAIL; }

	*pulTsrStartTime = pstContext->stCache.stTsr.ulTsrStartTime;
	HxLOG_Info ("*pulTsrStartTime=(%d) \n", *pulTsrStartTime);
	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetTsrRecTime (HUINT32 ulViewId, HUINT32 *pulTsrRecTime)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pulTsrRecTime)					{ return ERR_FAIL; }

	*pulTsrRecTime = pstContext->stCache.stTsr.ulTsrRecTime;
	HxLOG_Info ("*pulTsrRecTime=(%d) \n", *pulTsrRecTime);
	return ERR_OK;
}

#if defined(CONFIG_TIMESTAMP_64)
HERROR OAPI_MEDIA_PLAY_GetDurationTime (HUINT32 ulViewId, HUINT64 *pullDurationTime)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pullDurationTime)				{ return ERR_FAIL; }

	*pullDurationTime = pstContext->stCache.ullDurationTime;
	HxLOG_Info("*pulDurationTime=(%llu) \n", *pullDurationTime);
	return ERR_OK;
}
#else
HERROR OAPI_MEDIA_PLAY_GetDurationTime (HUINT32 ulViewId, HUINT32 *pulDurationTime)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pulDurationTime)				{ return ERR_FAIL; }

	*pulDurationTime = pstContext->stCache.ulDurationTime;
	HxLOG_Info("*pulDurationTime=(%d) \n", *pulDurationTime);
	return ERR_OK;
}
#endif

HERROR OAPI_MEDIA_PLAY_GetBitrate (HUINT32 ulViewId, HUINT32 *pulBitrate)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pulBitrate)				{ return ERR_FAIL; }

	*pulBitrate = pstContext->stCache.ulBitrate;
	HxLOG_Info("*pulBitrate=(%d) \n", *pulBitrate);
	return ERR_OK;
}


HERROR OAPI_MEDIA_PLAY_GetSupportedSpeeds (HUINT32 ulViewId, HUINT32 *pulNumSpeed, HUINT32 ulMaxNum, HINT32 *pnSpeeds)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if( NULL == pnSpeeds)						{ return ERR_FAIL; }

	*pulNumSpeed	=	pstContext->stCache.ulNumSpeed;

	HxSTD_MemCopy( pnSpeeds, pstContext->stCache.anSupportSpeeds, sizeof(HINT32)*
		((OxMP_SUPPORTING_SPEED_MAX > ulMaxNum)?ulMaxNum: OxMP_SUPPORTING_SPEED_MAX ) ); ;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetTrickRestrictMode (HUINT32 ulViewId, DxCopyrightControl_TrickRestrictMode_u *puTrickRestrictMode)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);


	if (NULL == pstContext)						{ return ERR_FAIL; }

	HxSTD_MemCopy( puTrickRestrictMode, &(pstContext->stCache.uTrickRestrictMode), sizeof(DxCopyrightControl_TrickRestrictMode_u));

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetAvailabilityStartTime (HUINT32 ulViewId, HUINT32 szTimeStringBuffer, HUINT8 *pucTimeString)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);
	HUINT32 szString;

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pucTimeString)					{ return ERR_FAIL; }

	szString = HxSTD_StrLen(pstContext->stCache.szAvailabilityStartTime);
	if(szString)
	{
		if(szString > szTimeStringBuffer)
		{
			szString = szTimeStringBuffer - 1;
		}
		HxSTD_MemCopy( pucTimeString, pstContext->stCache.szAvailabilityStartTime, szString);
		pucTimeString[szString] = '\0';
	}
	HxLOG_Info("*pucTimeString=(%s) \n", pucTimeString);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetTsrPausedPoints (HUINT32 ulViewId, HUINT32 *pulNum, OxMediaPlay_Bookmark_t **ppstBookmarks)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);
	HUINT32				i, num=0;

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if( NULL == pulNum || NULL == ppstBookmarks )	{return ERR_FAIL;}

	for (i=0,num=0; i<OxMP_PVR_BOOKMARK_MAX; i++)
	{
		/* not empty*/
		if (!HxSTD_StrEmpty(pstContext->stCache.stTsr.stPausedPoint[i].szName))
		{
			num++;
		}
	}

	*ppstBookmarks = (OxMediaPlay_Bookmark_t *)HLIB_STD_MemCalloc(num*sizeof(OxMediaPlay_Bookmark_t));
	if (*ppstBookmarks == NULL)
	{
		*pulNum = 0;
		*ppstBookmarks = NULL;

		return ERR_FAIL;
	}

	*pulNum = num;
	HxSTD_MemCopy(*ppstBookmarks, pstContext->stCache.stTsr.stPausedPoint, num*sizeof(OxMediaPlay_Bookmark_t));

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetTsrPaused (HUINT32 ulViewId, HBOOL *pbIsPaused)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);


	if (NULL == pstContext)						{ return ERR_FAIL; }

	*pbIsPaused = pstContext->stCache.stTsr.bPaused;

	return ERR_OK;
}
#define _____VIEW_CONTROL_____
HERROR OAPI_MEDIA_PLAY_SetPigRect (HUINT32 ulViewId, OxMediaPlay_PigSize_t *pstPigSize)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstPigSize)						{ return ERR_FAIL; }
	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	HxSTD_MemCopy( &pstContext->stCache.stPigSize, pstPigSize, sizeof(OxMediaPlay_PigSize_t) );
	OAPI_MP_SEM_RELEASE;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPigRect, "iib", oapi_mp_GetSessionId (pstContext), ulViewId, pstPigSize, sizeof(OxMediaPlay_PigSize_t) );

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetVideoSize (HUINT32 ulViewId, OxMediaPlay_VideoSize_t *pstVideoSize)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstVideoSize)						{ return ERR_FAIL; }
	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	HxSTD_MemCopy( &pstContext->stCache.stVideoSize, pstVideoSize, sizeof(OxMediaPlay_VideoSize_t) );
	OAPI_MP_SEM_RELEASE;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetVideoSize, "iib", oapi_mp_GetSessionId (pstContext), ulViewId, pstVideoSize, sizeof(OxMediaPlay_VideoSize_t));

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetVideoSize (HUINT32 ulViewId, OxMediaPlay_VideoSize_t *pstVideoSize)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstVideoSize)					{ return ERR_FAIL; }
	if (NULL == pstContext)						{ return ERR_FAIL; }


	HxSTD_MemCopy( pstVideoSize, &pstContext->stCache.stVideoSize, sizeof(OxMediaPlay_VideoSize_t) );

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_ResetVideoSize (HUINT32 ulViewId)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_ResetVideoSize, "ii", oapi_mp_GetSessionId (pstContext), ulViewId);

	return ERR_OK;
}

#define _____FUNC_PROPERTY_____
HERROR	OAPI_MEDIA_PLAY_SetTSREnable (HUINT32 ulViewId, HBOOL bEnable)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	pstContext->stCache.bTSREnable = bEnable;
	OAPI_MP_SEM_RELEASE;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetTSREnable, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, bEnable);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_SetSubtitleEnable (HUINT32 ulViewId, HBOOL bEnable)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	pstContext->stCache.bSubtitleEnable = bEnable;
	OAPI_MP_SEM_RELEASE;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetSubtitleEnable, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, bEnable);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_SetVideoHideEnable (HUINT32 ulViewId, HBOOL bEnable)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	pstContext->stCache.bVideoHideEnable = bEnable;
	OAPI_MP_SEM_RELEASE;

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetVideoHideEnable, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, bEnable);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_GetTSREnable (HUINT32 ulViewId, HBOOL *pbEnable)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pbEnable = pstContext->stCache.bTSREnable;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_GetSubtitleEnable (HUINT32 ulViewId, HBOOL *pbEnable)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pbEnable = pstContext->stCache.bSubtitleEnable;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_GetVideoHideEnable (HUINT32 ulViewId, HBOOL *pbEnable)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pbEnable = pstContext->stCache.bVideoHideEnable;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

#define _____ETC_____
HERROR OAPI_MEDIA_PLAY_GetEventInfo (HUINT32 ulViewId, DxEvent_t *pstEventInfo)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 							{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	HxSTD_MemCopy(pstEventInfo, &(pstContext->stCache.stEventInfo), sizeof(DxEvent_t));
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SaveThumbnail (HUINT32 ulViewId, HCHAR *pszFilename, HUINT32 ulWidth, HUINT32 ulHeight)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SaveThumbnail, "iiiis", oapi_mp_GetSessionId (pstContext), ulViewId, ulWidth, ulHeight, pszFilename);

	return ERR_OK;
}

#define _____LIVE_ONLY_____
HERROR	OAPI_MEDIA_PLAY_GetMajorChannel (HUINT32 ulViewId, HUINT32 * pulMajorCHNum)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pulMajorCHNum)					{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pulMajorCHNum = pstContext->stCache.stLive.ulMajorCHNum;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetConflictInfo (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_ConflictInfo_t *pstLiveConflictInfo)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);
	HERROR			hResult = ERR_FAIL;

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pstLiveConflictInfo)				{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;

	switch( eType )
	{
		case eOxMP_MEDIATYPE_LIVE:
			HxSTD_MemCopy(pstLiveConflictInfo, &(pstContext->stCache.stLiveConflictInfo), sizeof(OxMediaPlay_ConflictInfo_t));
			hResult = ERR_OK;
			break;
		default:
			hResult = ERR_FAIL;
			break;
	}

	OAPI_MP_SEM_RELEASE;

	return hResult;
}

HERROR OAPI_MEDIA_PLAY_GetViewState (HUINT32 ulViewId, OxMediaPlay_LiveViewState_e *peViewState)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == peViewState)					{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*peViewState = pstContext->stCache.eViewState;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetLockState (HUINT32 ulViewId, OxMediaPlay_LiveLockState_e *peLockState)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == peLockState)					{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*peLockState = pstContext->stCache.eLockState;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_CountAitSection (HUINT32 ulViewId, HUINT32 *pulAitNum)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pulAitNum)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pulAitNum = pstContext->stCache.stAitSecData.ulAitSecCount;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetAitPid (HUINT32 ulViewId, HUINT16 *pusPrevAitPid, HUINT16 *pulCurrAitPid)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pusPrevAitPid)						{ return ERR_FAIL; }
	if (NULL == pulCurrAitPid)						{ return ERR_FAIL; }

	*pusPrevAitPid = pstContext->stCache.stAitSecData.usPrevAitPid;
	*pulCurrAitPid = pstContext->stCache.stAitSecData.usCurrAitPid;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetAitSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucAitSec, HUINT32 *ulSecLen)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);
	HUINT8				*pucAitSec = NULL;
	HUINT32				 ulSectionLen = 0;
	HERROR				 hResult = ERR_FAIL;

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == ppucAitSec)						{ return ERR_FAIL; }
	if (NULL == ulSecLen)						{ return ERR_FAIL; }

	*ppucAitSec 	= NULL;
	*ulSecLen 		= 0;

	OAPI_MP_SEM_GET;

	if (ulIndex >= OxMP_AIT_SECTION_NUM_MAX)
	{
		HxLOG_Error ("invalid index(%d) : max(%d) \n", ulIndex, OxMP_AIT_SECTION_NUM_MAX);
		goto END_FUNC;
	}

	ulSectionLen = pstContext->stCache.stAitSecData.aulAitSecLen[ulIndex];
	if (ulSectionLen > 0)
	{
		pucAitSec = (HUINT8 *)HLIB_STD_MemCalloc(ulSectionLen);
		if (pucAitSec == NULL)
		{
			HxLOG_Error ("mem alloc failed.....\n");
			goto END_FUNC;
		}

		HxSTD_MemCopy(pucAitSec, pstContext->stCache.stAitSecData.paucAitRawSec[ulIndex], ulSectionLen);

		*ppucAitSec = pucAitSec;
		*ulSecLen	= ulSectionLen;
	}

	hResult = ERR_OK;

END_FUNC:
	OAPI_MP_SEM_RELEASE;
	return hResult;
}

HERROR OAPI_MEDIA_PLAY_FreeAitSection (HUINT8 *pucAitSec)
{
	if (pucAitSec != NULL)
	{
		HLIB_STD_MemFree(pucAitSec);
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetTempAvPid (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT16 usVideoPid, DxAudioCodec_e eAudioCodec, HUINT16 usAudioPid, HUINT16 usPcrPid)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetTempAvPid, "iiiiiii", oapi_mp_GetSessionId (pstContext), ulViewId, eVideoCodec, usVideoPid, eAudioCodec, usAudioPid, usPcrPid);

	return ERR_OK;
}

void OAPI_MEDIA_PLAY_SetAitListenerStatus (HUINT32 ulViewId, HBOOL bEnable)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	HxLOG_Debug("[VIEW: %d] AIT Listener Enable : %d \n", ulViewId, bEnable);

	if (NULL == pstContext)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetAitListenerStatus [VIEW: %d] AIT Listener Enable : %d \n", ulViewId, bEnable);
		return;
	}

	pstContext->bReadyAitListener = bEnable;
}

HERROR OAPI_MEDIA_PLAY_CheckAitSection (HUINT32 ulViewId)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	if(pstContext->bReadyAitListener == FALSE)
	{
		HxLOG_Error("[VIEW: %d] Not Ready AIT Listener.\n", ulViewId);
		return ERR_FAIL;
	}

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_CheckAitSection, "ii", oapi_mp_GetSessionId (pstContext), ulViewId);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetRctInfoCount (HUINT32 ulViewId, HUINT32 *pulTotalLinkCount , HUINT32 *pulImgDataByteSize)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pulTotalLinkCount)						{ return ERR_FAIL; }
	if (NULL == pulImgDataByteSize)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;

	*pulTotalLinkCount = pstContext->stCache.stRctData.stRctInfo.ulTotalLinkCnt;
	*pulImgDataByteSize = pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.ulDataByteSize;

	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetRctInfo (HUINT32 ulViewId, DxService_RctInfo_t *pstRctInfo)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pstRctInfo)						{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;

	pstRctInfo->usServiceId = pstContext->stCache.stRctData.stRctInfo.usServiceId;
	pstRctInfo->usYearOffset = pstContext->stCache.stRctData.stRctInfo.usYearOffset;
	pstRctInfo->ulTotalLinkCnt = pstContext->stCache.stRctData.stRctInfo.ulTotalLinkCnt;

	if(pstRctInfo->ulTotalLinkCnt >0)
		HxSTD_MemCopy(pstRctInfo->pstLinkInfoList, pstContext->stCache.stRctData.stRctInfo.pstLinkInfoList, sizeof(DxService_RctLinkInfo_t) * pstContext->stCache.stRctData.stRctInfo.ulTotalLinkCnt);

	HxSTD_MemCopy(&pstRctInfo->stShortEvtInfo, &pstContext->stCache.stRctData.stRctInfo.stShortEvtInfo, sizeof(DxService_RctShortEvtInfo_t));

	pstRctInfo->stImageIconInfo.ucIconId 			=  pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.ucIconId;
	pstRctInfo->stImageIconInfo.ucTransPortMode	=  pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.ucTransPortMode;
	pstRctInfo->stImageIconInfo.ucPositionFlag		=  pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.ucPositionFlag;
	pstRctInfo->stImageIconInfo.ucCoordinateSystem=  pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.ucCoordinateSystem;
	pstRctInfo->stImageIconInfo.usHorizontalOrigin	=  pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.usHorizontalOrigin;
	pstRctInfo->stImageIconInfo.usVerticalOrigin	=  pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.usVerticalOrigin;
	HxSTD_MemCopy(pstRctInfo->stImageIconInfo.szIconTypeChar, pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.szIconTypeChar, DxRCT_MAX_IMAGE_ICON_TYPE_CHART + 1);
	pstRctInfo->stImageIconInfo.ulDataByteSize	=  pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.ulDataByteSize;

	if(pstRctInfo->stImageIconInfo.ulDataByteSize>0)
		HxSTD_MemCopy(pstRctInfo->stImageIconInfo.pucDatabyte, pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.pucDatabyte, sizeof(HUINT8) * pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.ulDataByteSize);

	HxSTD_MemCopy(pstRctInfo->stImageIconInfo.szIconUrl, pstContext->stCache.stRctData.stRctInfo.stImageIconInfo.szIconUrl, DxRCT_MAX_IMAGE_ICON_URL_LEN + 1);

	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_FreeRctInfo (DxService_RctInfo_t *pstRctInfo)
{
	if (pstRctInfo != NULL)
	{
		DxService_RctImageIconInfo_t	*pstRctImageInfo = NULL;

		pstRctImageInfo = &pstRctInfo->stImageIconInfo;

		if(pstRctImageInfo->pucDatabyte !=NULL)
		{
			HLIB_STD_MemFree(pstRctImageInfo->pucDatabyte);
		}

		if(pstRctInfo->pstLinkInfoList!=NULL)
		{
			HLIB_STD_MemFree(pstRctInfo->pstLinkInfoList);
		}

		HLIB_STD_MemFree(pstRctInfo);
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_CountMhegPatSection (HUINT32 ulViewId, HUINT32 *pulPatNum)
{
	return oapi_mp_MhegCountRawSection(ulViewId, eOxMP_MHEG_PAT, pulPatNum);
}

HERROR OAPI_MEDIA_PLAY_GetMhegPatSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucPatSec, HUINT32 *ulSecLen)
{
	return oapi_mp_MhegGetRawSection(ulViewId, eOxMP_MHEG_PAT, ulIndex, ppucPatSec, ulSecLen);
}

HERROR OAPI_MEDIA_PLAY_FreeMhegPatSection (HUINT8 *pucPatSec)
{
	return oapi_mp_MhegFreeRawSection(pucPatSec);
}

HERROR OAPI_MEDIA_PLAY_CountMhegPmtSection (HUINT32 ulViewId, HUINT32 *pulPmtNum)
{
	return oapi_mp_MhegCountRawSection(ulViewId, eOxMP_MHEG_PMT, pulPmtNum);
}

HERROR OAPI_MEDIA_PLAY_GetMhegPmtSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucPmtSec, HUINT32 *ulSecLen)
{
	return oapi_mp_MhegGetRawSection(ulViewId, eOxMP_MHEG_PMT, ulIndex, ppucPmtSec, ulSecLen);
}

HERROR OAPI_MEDIA_PLAY_FreeMhegPmtSection (HUINT8 *pucPatSec)
{
	return oapi_mp_MhegFreeRawSection(pucPatSec);
}

HERROR	OAPI_MEDIA_PLAY_GetSuppleType (HUINT32 ulViewId, OxMediaPlay_SuppleSvcType_e * peSuppSvcType)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == peSuppSvcType)					{ return ERR_FAIL; }

	*peSuppSvcType = pstContext->stCache.stLive.eSuppSvcType;

	return ERR_OK;
}


#define _____MEDIAPB_ONLY_____
HERROR OAPI_MEDIA_PLAY_GetURI (HUINT32 ulViewId, HCHAR *pszURI)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }


	if ( eOxMP_MEDIATYPE_MEDIAPLAY != pstContext->stCache.eType )
		return ERR_FAIL;

	//HLIB_STD_StrUtf8NCpy (pszURI, pstContext->stCache.unStart.stMediaPb->szUri, OxMP_MEDIAPLAY_URL);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_Probe(HUINT32 ulViewId, HCHAR *pszURI)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

//	HLIB_STD_StrUtf8NCpy (pstContext->szUri, pszURI, OxMP_MEDIAPLAY_URL);
	/* reset cached duration */
#if defined(CONFIG_TIMESTAMP_64)
	pstContext->stCache.ullPosition = 0;
	pstContext->stCache.ullDurationTime = 0;
#else
	pstContext->stCache.ulPosition = 0;
	pstContext->stCache.ulDurationTime = 0;
#endif

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_Probe, "iis", oapi_mp_GetSessionId (pstContext), ulViewId, pszURI);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetSubtitleSyncTime (HUINT32 ulViewId, HUINT32 nSubtitle_SyncTime)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	switch( pstContext->stCache.eType )
	{
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		{
			OAPI_MP_SEM_GET;
			pstContext->stCache.nSubtitle_SyncTime = nSubtitle_SyncTime;
			OAPI_MP_SEM_RELEASE;

			HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetSubtitleSyncTime, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, nSubtitle_SyncTime);

			return ERR_OK;
		}
		default:
			break;
	}

	return ERR_FAIL;
}

HERROR OAPI_MEDIA_PLAY_SetSubtitleFontSize (HUINT32 ulViewId, HUINT32 nSubtitle_FontSize)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	switch( pstContext->stCache.eType )
	{
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		{
			OAPI_MP_SEM_GET;
			pstContext->stCache.nSubtitle_FontSize = nSubtitle_FontSize;
			OAPI_MP_SEM_RELEASE;

			HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetSubtitleFontSize, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, nSubtitle_FontSize);

			return ERR_OK;
		}
		default:
			break;
	}

	return ERR_FAIL;
}

HERROR OAPI_MEDIA_PLAY_SetSubtitleTextPosition (HUINT32 ulViewId, HUINT32 nSubtitle_TextPosition)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	switch( pstContext->stCache.eType )
	{
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		{
			OAPI_MP_SEM_GET;
			pstContext->stCache.nSubtitle_TextPosition = nSubtitle_TextPosition;
			OAPI_MP_SEM_RELEASE;

			HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetSubtitleTextPosition, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, nSubtitle_TextPosition);

			return ERR_OK;
		}
		default:
			break;
	}

	return ERR_FAIL;
}

HERROR OAPI_MEDIA_PLAY_GetSubtitleSyncTime (HUINT32 ulViewId, HUINT32 *pnSubtitle_SyncTime)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pnSubtitle_SyncTime = pstContext->stCache.nSubtitle_SyncTime;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetSubtitleFontSize (HUINT32 ulViewId, HUINT32 *pnSubtitle_FontSize)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pnSubtitle_FontSize = pstContext->stCache.nSubtitle_FontSize;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetSubtitleTextPosition (HUINT32 ulViewId, HUINT32 *pnSubtitle_TextPosition)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pnSubtitle_TextPosition = pstContext->stCache.nSubtitle_TextPosition;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetSubtitleStatus (HUINT32 ulViewId, OxMediaPlay_SubtitleStatus_t *pstStatus)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	HxSTD_MemCopy(pstStatus, &(pstContext->stCache.stMediaSutbitleStatus), sizeof(OxMediaPlay_SubtitleStatus_t) );
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetBufferingScenario(HUINT32 ulViewId, HBOOL bAuto, HINT32 nThreshold)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SeBufferingScenario, "iiii", oapi_mp_GetSessionId (pstContext), ulViewId, bAuto, nThreshold);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetBufferingChunkInfo (HUINT32 ulViewId, HUINT32 *pnChunks, HUINT32 **ppulStartOffsets, HUINT32 **ppulEndOffsets)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	if (NULL == pnChunks) 						{ return ERR_FAIL; }
	if (NULL == ppulStartOffsets) 				{ return ERR_FAIL; }
	if (NULL == ppulEndOffsets) 				{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pnChunks 			= pstContext->stCache.stBufferInfo.ulChunks;
	*ppulStartOffsets 	= pstContext->stCache.stBufferInfo.ullStartOffsets;
	*ppulEndOffsets   	= pstContext->stCache.stBufferInfo.ullEndOffsets;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetSeekableRangeInfo (HUINT32 ulViewId, HUINT32 *pnChunks, HUINT64 **ppullSeekableStart, HUINT64 **ppullSeekableEnd)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	if (NULL == pnChunks) 						{ return ERR_FAIL; }
	if (NULL == ppullSeekableStart) 				{ return ERR_FAIL; }
	if (NULL == ppullSeekableEnd) 				{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pnChunks 				= pstContext->stCache.stBufferInfo.ulSeekableChunks;
	*ppullSeekableStart		= pstContext->stCache.stBufferInfo.ullSeekableStart;
	*ppullSeekableEnd   	= pstContext->stCache.stBufferInfo.ullSeekableEnd;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetBufferingRemainedTime (HUINT32 ulViewId, HUINT32 *pulBufferRemaindTime)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pulBufferRemaindTime = pstContext->stCache.ulBufferRemaindTime;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetBufferingPercent (HUINT32 ulViewId, HUINT32 *pulBufferPercent)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*pulBufferPercent = pstContext->stCache.ulBufferPercent;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetBufferState (HUINT32 ulViewId, OxMediaPlay_BufferState_e *peBufferState)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	OAPI_MP_SEM_GET;
	*peBufferState = pstContext->stCache.eBufferState;
	OAPI_MP_SEM_RELEASE;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetDrmLicenseData (HUINT32 ulViewId, const HCHAR *pszData)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetDrmLicenseData, "iis", oapi_mp_GetSessionId (pstContext), ulViewId, pszData);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetBrowserUserAgentData (HUINT32 ulViewId, HCHAR *pszUAData)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetBrowserUserAgentData, "iis", oapi_mp_GetSessionId (pstContext), ulViewId, pszUAData);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetResourceMap (OxMediaPlay_ResourceMap_t *pstResourceMap)
{
	HxLOG_Trace();
	if(s_bGetResource == TRUE)
	{
		HxLOG_Error ("Processing ResourceMap...\n");
		return ERR_FAIL;
	}
	if(s_pstResourceMap == NULL)
	{
		/* 1st Call */
		s_bGetResource = TRUE;
		HxLOG_Trace();
		HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_ResourceMap, NULL);
	}
	else
	{
		if(pstResourceMap != NULL)
		{
			/* Over 2nd Call */
			pstResourceMap->pulPlaypumpList = s_pstResourceMap->pulPlaypumpList;
			pstResourceMap->pulDmaList = s_pstResourceMap->pulDmaList;
			pstResourceMap->ulNumOfPlaypump = s_pstResourceMap->ulNumOfPlaypump;
			pstResourceMap->ulNumOfDma = s_pstResourceMap->ulNumOfDma;
			pstResourceMap->ulAudioMixerHandle = s_pstResourceMap->ulAudioMixerHandle;
#if 0
			HxLOG_Trace();
			HxLOG_Error("ulNumOfDma:%d, ulNumOfPlaypump:%d\n", s_pstResourceMap->ulNumOfDma, s_pstResourceMap->ulNumOfPlaypump);
			{
				int i=0;
				for(i=0; i<s_pstResourceMap->ulNumOfPlaypump; ++i)
				{
					HxLOG_Error("[%08X]\n", s_pstResourceMap->pulPlaypumpList[i]);
				}
			}
#endif
		}
	}

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetPumpIndex (HUINT32 ulViewId, HUINT32 *ulPumpId)
{
	oapiMpContext_t         *pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)		{ return ERR_FAIL; }

	*ulPumpId = pstContext->stCache.ulPumpId;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetDuration (HUINT32 ulViewId, HUINT64 ullDuration)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetDuration, "iil", oapi_mp_GetSessionId (pstContext), ulViewId, ullDuration);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetPlayerType (HUINT32 ulViewId, OxMediaPlay_PlayerType_e eMediaPlayPlayerType)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPlayerType, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, eMediaPlayPlayerType);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_AudioFlush(HUINT32 ulViewId, HUINT32 unTryToChangeAudioTrack)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_AudioFlush, "iii", oapi_mp_GetSessionId (pstContext), ulViewId, unTryToChangeAudioTrack);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_AudioRestart(HUINT32 ulViewId)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_AudioRestart, "ii", oapi_mp_GetSessionId (pstContext), ulViewId);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_AudioCodecSwitch(HUINT32 ulViewId, HUINT32 unTryToChangeAudioTrack, HINT32 nAudioCodec)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_AudioCodecSwitch, "iiii", oapi_mp_GetSessionId (pstContext), ulViewId, unTryToChangeAudioTrack, nAudioCodec);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_SetFade(HUINT32 ulViewId, HUINT32 unStartVolume, HUINT32 unTargetVolume, HINT32 nEase, HINT32 nEaseTimeElapsed, HINT32 nEaseTransitionFrames)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetFade, "iiiiiii", oapi_mp_GetSessionId (pstContext), ulViewId, unStartVolume, unTargetVolume, nEase, nEaseTimeElapsed, nEaseTransitionFrames);

	return ERR_OK;
}

HERROR	OAPI_MEDIA_PLAY_GetAudioDecoderVolume(HUINT32 ulViewId, HUINT32 *punVolume)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext) 					{ return ERR_FAIL; }

	if(punVolume == NULL)
	{
		return ERR_FAIL;
	}

	*punVolume = pstContext->stCache.unAudioDecoderVolume;

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetPlayReadyInfo (HUINT32 ulViewId, HCHAR *pszLaUrl, HCHAR *pszCustomData, HBOOL bControlFlag)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPlayReadyInfo , "issi", ulViewId, pszLaUrl, pszCustomData, bControlFlag);

	return ERR_OK;
}


HERROR OAPI_MEDIA_PLAY_GetVmxIdentifier (HUINT8 **pIdentifier)
{
	*pIdentifier = (HUINT8 *)s_pstOapiMp_Context[s_ulOapiMp_MainViewId].stCache.szVmxIdentifier;
	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetVmxStatus (HUINT32 ulViewId, HINT32 *eStatus)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if( NULL == eStatus)						{ return ERR_FAIL; }

	*eStatus	=	pstContext->stCache.eVmxStatus;
	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_GetVmxError (HUINT32 ulViewId, HINT32 *nErrorCode)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if( NULL == nErrorCode)						{ return ERR_FAIL; }

	*nErrorCode	=	pstContext->stCache.nVmxErrorCode;
	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_SetVmxCas (HUINT32 ulViewId, HCHAR *pszVCASUrl, HCHAR *pszVendorName)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pszVCASUrl)						{ return ERR_FAIL; }
	if (NULL == pszVendorName)						{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetVmxCas , "iss", ulViewId, pszVCASUrl, pszVendorName);

	return ERR_OK;
}

HERROR OAPI_MEDIA_PLAY_UnsetVmxCas (HUINT32 ulViewId)
{
	oapiMpContext_t		*pstContext = oapi_mp_GetContext (ulViewId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_UnsetVmxCas , "i", ulViewId);
	pstContext->stCache.eVmxStatus = eOxMP_VMXSTATUS_NONE;

	return ERR_OK;
}

#define _____BACKGROUD_____

HERROR OAPI_MEDIA_PLAY_MhegBgStart (HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart, HUINT32 *pulSessionId)
{
	HUINT32				 ulSessionId = 0;

//	oapi_mp_IncreaseSessionVer (pstContext);
//	ulSessionId = oapi_mp_GetSessionId (pstContext);
	HxLOG_Debug("(+) ViewId (%d)  \n",  ulViewId);

	HLIB_RPC_Call (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_MhegBgStart, "iib", ulSessionId, ulViewId, punStart, sizeof(OxMediaPlay_StartInfo_t));

	return ERR_OK;

}
// @BEGIN_AUTOGEN_FUNCTION

// @END_AUTOGEN_FUNCTION
