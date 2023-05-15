/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_media_play.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <svc_pipe.h>
#include <svc_output.h>
#include <svc_av.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_media.h>
#include <mgr_live.h>
#include <mgr_copyrightcontrol.h>
#include <mgr_output.h>
#include <mgr_drmagent.h>
#include <mgr_3rdparty.h>
#include <mgr_hbbtv.h>

#include "_om_media_play.h"
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


// View Action Handle로 올라오지 않는 이벤트에 한해 이걸로 비교... 예를 들어  Record Event ...
#define CHECK_MAIN_ACTION_HANDLE()	\
	do {	\
		HUINT32	ulMainActionId	= 	MGR_ACTION_GetMainActionId();	\
		if(pstContext->ulActionId != ulMainActionId)	\
		{	\
			return MESSAGE_PASS;	\
		}	\
	} while(0)

// View Action Handle로 올라오는 이벤트는 이걸로 체크...
#define CHECK_ACTION_HANDLE()	\
	do {	\
		HUINT32 ulActionId	=	SVC_PIPE_GetActionId (hHandle);	\
		if(pstContext->ulActionId != ulActionId)	\
		{	\
			return MESSAGE_PASS;	\
		}	\
	} while(0)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32					s_ulLastMpAppId = 0x0010;
STATIC HUINT32					s_ulTsrRecActionId = 0;
STATIC omMediaPlay_Context_t	s_astMgrMpContext[NUM_VIEW_ACTION];
STATIC HINT32				 	s_nRpcHandle = 0;


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/


#define ___LOCAL_FUNCTIONS___
STATIC HERROR om_media_play_logicStop(omMediaPlay_Context_t	*pstContext, HUINT32 ulActionId, HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e  eMediaType)
{
	HERROR					 hErr = ERR_FAIL;

	if (pstContext == NULL)
	{
		return ERR_FAIL;
	}

	// 밑에서 Stop하기 때문에 Context 값이 달라질 수 있다.
	// 즉, MediaType 값이 달라질 수 있으니 여기서 backup해서 Notify에 쓴다.
	pstContext->stCache.eState	= eOxMP_PLAYSTATE_STOPPING;

	switch (eMediaType)
	{
	case eOxMP_MEDIATYPE_LIVE:
		hErr = om_media_play_live_Stop (ulActionId);
		break;
#if defined(CONFIG_MW_MM_PVR)
	case eOxMP_MEDIATYPE_PVRPLAYBACK:
		hErr = om_media_play_pvrpb_Stop (ulActionId);
		break;

	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		hErr = om_media_play_tsrpb_Stop (ulActionId);
		break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
	case eOxMP_MEDIATYPE_MEDIAPLAY:
		hErr = om_media_play_mediapb_Stop (ulActionId);
		break;
#endif

	default:
		hErr = ERR_FAIL;
		break;
	}

	if (ERR_OK == hErr)
	{
		/* clean cache */
		switch (eMediaType)
		{
		case eOxMP_MEDIATYPE_LIVE:
			HxSTD_MemSet(&(pstContext->stCache.stLive), 0, sizeof(OxMediaPlay_StartLive_t) );
			pstContext->stCache.stLive.ulMasterSvcUid	=	(HUINT32)-1;
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
			HxSTD_MemSet(&(pstContext->stCache.stPvrPb), 0, sizeof(OxMediaPlay_StartPvrPb_t) );
			break;

		case eOxMP_MEDIATYPE_TSRPLAYBACK:
			HxSTD_MemSet(&(pstContext->stCache.stTsrPb), 0, sizeof(OxMediaPlay_StartTsrPb_t) );
			break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
		case eOxMP_MEDIATYPE_MEDIAPLAY:
			HxSTD_MemSet(&(pstContext->stCache.stMediaPlay),			0, sizeof(OxMediaPlay_StartMediaPlay_t) );
			HxSTD_MemSet(&(pstContext->stCache.stMediaSutbitleStatus),	0, sizeof(OxMediaPlay_SubtitleStatus_t) );
			HxSTD_MemSet(&(pstContext->stCache.stBufferInfo),			0, sizeof(OxMediaPlay_BufferInfo_t) );
			pstContext->stCache.eBufferState			=	eOxMP_BUFFERSTATE_None;
			pstContext->stCache.nSubtitle_SyncTime		=	0;
			pstContext->stCache.nSubtitle_FontSize		=	0;
			pstContext->stCache.nSubtitle_TextPosition	=	0;
			pstContext->stCache.ulPumpId				=	0;
			break;
#endif
		default:
			break;
		}

		/* broadcast */
		om_media_play_CbOnStopped(ulViewId, ulSessionId, eMediaType);
	}

	hErr = MGR_OUTPUT_ResetCopyProtect(pstContext->ulActionId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("MGR_OUTPUT_ResetCopyProtect Failed \n");
		hErr = ERR_FAIL;
	}

	return ERR_OK;
}

void om_media_play_InitContext( omMediaPlay_Context_t *pstContext )
{
	if( NULL == pstContext )
		return ;

	pstContext->ulActionId 	=	0xFFFF;
	pstContext->ulSessionId	=	0xFFFF;

	pstContext->bActionTaken	=	FALSE;

	pstContext->eMediaType		=	eOxMP_MEDIATYPE_NONE;

	HxSTD_MemSet (&(pstContext->stCache), 0, sizeof(OxMediaPlay_Cache_t));

	if( NULL != pstContext->pExtension )
		OxMGR_Free( pstContext->pExtension );

	pstContext->pExtension	=	NULL;

	return ;
}

#define _____OAPI_CALLBACK_FUNCTIONS_____
void om_media_play_CbOnBof (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBof, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnEof (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnEof, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnStarted (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_StartInfo_t *punStart)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnStarted, "iiib", ulViewId, ulSessionId, eMediaType,  punStart, sizeof(OxMediaPlay_StartInfo_t) );
}
void om_media_play_CbOnStopped (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnStopped, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnPlaying (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlaying, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnPaused (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPaused, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnTricked (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTricked, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnPlayStateChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_PlayState_e ePlayState)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayStateChanged, "iiii", ulViewId, ulSessionId, eMediaType, ePlayState);
}
void om_media_play_CbOnPlayErrorChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_PlayError_e ePlayError, OxMediaPlay_ErrorReason_e eReason)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayErrorChanged, "iiiii", ulViewId, ulSessionId, eMediaType, ePlayError, eReason);
}
#if defined(CONFIG_TIMESTAMP_64)
void om_media_play_CbOnPlayTimeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT64 ullPlayTime, HUINT64 ullDurationTime)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayTimeChanged, "iiill", ulViewId, ulSessionId, eMediaType, ullPlayTime, ullDurationTime);
}
void om_media_play_CbOnPlayDurationChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT64 ullDurationTime, HBOOL bInfinity)
{
		HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayDurationChanged, "iiili", ulViewId, ulSessionId, eMediaType, ullDurationTime, bInfinity);
}
#else
void om_media_play_CbOnPlayTimeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulPlayTime, HUINT32 ulDurationTime)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayTimeChanged, "iiiii", ulViewId, ulSessionId, eMediaType, ulPlayTime, ulDurationTime);
}
void om_media_play_CbOnPlayDurationChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulDurationTime, HBOOL bInfinity)
{
		HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayDurationChanged, "iiiii", ulViewId, ulSessionId, eMediaType, ulDurationTime, bInfinity);
}
#endif
void om_media_play_CbOnSpeedChanged	(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HINT32 nSpeed)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSpeedChanged, "iiii", ulViewId, ulSessionId, eMediaType, nSpeed);
}
#if defined (CONFIG_OP_SES)
void om_media_play_CbOnRatingChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HINT32 nBlocked, HINT32 nRating, HINT32 nBlockedByCas)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnRatingChanged, "iiiiii", ulViewId, ulSessionId, eMediaType, nBlocked, nRating, nBlockedByCas);
}
#else
void om_media_play_CbOnRatingChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HINT32 nBlocked, HINT32 nRating, HINT32 nDrmSystemId)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnRatingChanged, "iiiiii", ulViewId, ulSessionId, eMediaType, nBlocked, nRating, nDrmSystemId);
}
#endif
void om_media_play_CbOnAVStarted (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVStarted, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnAVStopped (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVStopped, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnAVResume (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVResume, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnAVUnderrun(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVUnderrun, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnVideoStreamInfo(HUINT32 ulViewId,HUINT32 ulSessionId,OxMediaPlay_MediaType_e eMediaType,OxMediaPlay_VideoStreamInfo_t * pstOapiVideoStreamInfo)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVideoStreamInfo, "iiib", ulViewId, ulSessionId, eMediaType, pstOapiVideoStreamInfo, sizeof(OxMediaPlay_VideoStreamInfo_t));
}
void om_media_play_CbOnAudioClipCompleted(HUINT32 ulViewId,HUINT32 ulSessionId,OxMediaPlay_MediaType_e eMediaType, HUINT32 ulRequestId)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioClipCompleted, "iiii", ulViewId, ulSessionId, eMediaType, ulRequestId);
}
void om_media_play_CbOnAVMediaRectChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVMediaRectChanged, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnSupportedSpeedChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulNumSpeed, HINT32 *pSpeeds)
{
	if( ulNumSpeed == 0 )
		return ;

	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSupportedSpeedChanged, "iiiib", ulViewId, ulSessionId, eMediaType, ulNumSpeed, pSpeeds, sizeof(HINT32) * ulNumSpeed);
}
void om_media_play_CbOnSiPMTChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiPMTChanged, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnSiEITChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiEITChanged, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnSiSITChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiSITChanged, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnSiAITChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType,
														OxMediaPlay_AitSectionData_t *pstAitSectionData, HUINT8 *pucAitRaw)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiAITChanged, "iiibb", ulViewId, ulSessionId, eMediaType, pstAitSectionData, sizeof(OxMediaPlay_AitSectionData_t), pucAitRaw, pstAitSectionData->ulAitTotaSeclLen);
}
void om_media_play_CbOnSiRCTChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType,
														OxMediaPlay_RctState_e eRctState, void *pvRctSectionData, HUINT32 ulSize)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiRCTChanged, "iiiib", ulViewId, ulSessionId, eMediaType, eRctState, pvRctSectionData, ulSize);
}

void om_media_play_CbOnSiRCTRequestImageIcon (HUINT32 ulViewId, HUINT32 ulSessionId,  OxMediaPlay_MediaType_e eMediaType, HUINT8 *pszUrl)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiRCTRequestImageIcon, "iiis", ulViewId, ulSessionId, eMediaType, pszUrl);
}

void om_media_play_CbOnAvailabilityStartTimeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HCHAR *pString)
{
		HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAvailabilityStartTimeChanged, "iiis", ulViewId, ulSessionId, eMediaType, pString);
}
void om_media_play_CbOnComponentVideoChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_VideoCompTable_t *pstVideoCompTable)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentVideoChanged, "iiib", ulViewId, ulSessionId, eMediaType, pstVideoCompTable, sizeof(OxMediaPlay_VideoCompTable_t));
}
void om_media_play_CbOnComponentAudioChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_AudioCompTable_t *pstAudioCompTable)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentAudioChanged, "iiib", ulViewId, ulSessionId, eMediaType, pstAudioCompTable, sizeof(OxMediaPlay_AudioCompTable_t) );
}
void om_media_play_CbOnComponentSubtitleChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentSubtitleChanged, "iiib", ulViewId, ulSessionId, eMediaType, pstSubtitleCompTable, sizeof(OxMediaPlay_SubtitleCompTable_t) );
}
void om_media_play_CbOnComponentDataChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_DataCompTable_t *pstDataCompTable)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentDataChanged, "iiib", ulViewId, ulSessionId, eMediaType, pstDataCompTable, sizeof(OxMediaPlay_DataCompTable_t) );
}
void om_media_play_CbOnComponentSelected (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_Component_e eCompType, HINT32 nSelectIndex, HINT32 nSelectSubIndex)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentSelected, "iiiiii", ulViewId, ulSessionId, eMediaType, eCompType, nSelectIndex, nSelectSubIndex);
}
void om_media_play_CbOnComponentUnselected (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_Component_e eCompType, HINT32 nUnselectIndex, HINT32 nUnselectSubIndex)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentUnselected, "iiiiii", ulViewId, ulSessionId, eMediaType, eCompType, nUnselectIndex, nUnselectSubIndex);
}
void om_media_play_CbOnTTXUpdated (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTTXUpdated, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnDrmBlocked (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_DrmRightsError_t* pstDrmRightsError)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnDrmBlocked, "iiib", ulViewId, ulSessionId, eMediaType, pstDrmRightsError, sizeof(OxMediaPlay_DrmRightsError_t));
}
void om_media_play_CbOnPinCtrlChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPinCtrlChanged, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnResourceTaken (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResourceTaken, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnResourceLost (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResourceLost, "iii", ulViewId, ulSessionId, eMediaType);
}
void om_media_play_CbOnRadioBGChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HBOOL bRadioBG)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnRadioBGChanged, "iiii", ulViewId, ulSessionId, eMediaType, bRadioBG);
}
void om_media_play_CbOnMainViewChanged (HUINT32 ulMainView)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMainViewChanged, "i", ulMainView);
}
void om_media_play_CbOnTSREnable (HUINT32 ulViewId, HBOOL bEnable)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSREnable, "ii", ulViewId, bEnable);
}
void om_media_play_CbOnSubtitleEnable (HUINT32 ulViewId, HBOOL bEnable)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleEnable, "ii", ulViewId, bEnable);
}
void om_media_play_CbOnVideoHideEnable (HUINT32 ulViewId, HBOOL bEnable)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVideoHideEnable, "ii", ulViewId, bEnable);
}
void om_media_play_CbOnVideoSize (HUINT32 ulViewId, OxMediaPlay_VideoSize_t *pstVideoSize)
{
	if( NULL == pstVideoSize )
		return ;

	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVideoSize, "ib", ulViewId, pstVideoSize, sizeof(OxMediaPlay_VideoSize_t));
}
void om_media_play_CbOnPigRect (HUINT32 ulViewId, OxMediaPlay_PigSize_t *pstPigSize)
{
	if( NULL == pstPigSize )
		return ;

	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPigRect, "ib", ulViewId, pstPigSize, sizeof(OxMediaPlay_PigSize_t) );
}

STATIC void om_media_play_CbOnTSRTimecodeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulStartRecTime, HUINT32 ulRecTime, HUINT32 ulLimitDuration)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSRTimecodeChanged, "iiiiii", ulViewId, ulSessionId, eMediaType, ulStartRecTime, ulRecTime, ulLimitDuration);
}

#if defined(CONFIG_OP_SES)
void om_media_play_CbCcTrickRestrictModeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, DxCopyrightControl_TrickRestrictMode_u uTrickRestrictMode, eDxCopyrightControl_SesStorageRight_e eStorageRight)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnCcTrickRestrictModeChanged, "iiiii", ulViewId, ulSessionId, eMediaType, uTrickRestrictMode, eStorageRight);
}
#else
void om_media_play_CbCcTrickRestrictModeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, DxCopyrightControl_TrickRestrictMode_u uTrickRestrictMode)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnCcTrickRestrictModeChanged, "iiii", ulViewId, ulSessionId, eMediaType, uTrickRestrictMode);
}
#endif
void om_media_play_CbOnSiNetworkChanged(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulStartTimeOfChange, HUINT32 ulASODuration,DxNcdNetworkInfo_t *pstNetworkMsg)
{

	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiNetworkChanged, "iiiiib",  ulViewId, ulSessionId, eMediaType, ulStartTimeOfChange,  ulASODuration,pstNetworkMsg,sizeof(DxNcdNetworkInfo_t));
}

void om_media_play_CbOnSiNetworkUriChanged(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, DxNetworkUriInfo_t *pstNetworkUri)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiNetworkUriChanged, "iiib",  ulViewId, ulSessionId, eMediaType, pstNetworkUri,sizeof(DxNetworkUriInfo_t));
}

void om_media_play_CbHbbtvOnHdplusRedbutton (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulHdplusRedbutton)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnHbbtvOnHdplusRedbutton, "iiii", ulViewId, ulSessionId, eMediaType, ulHdplusRedbutton);
}

void om_media_play_CbOnTSRPaused (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType,OxMediaPlay_TsrPaused_e eReason,OxMediaPlay_Bookmark_t *pstBookmark)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSRPaused, "iiiib", ulViewId, ulSessionId, eMediaType, eReason,pstBookmark, sizeof(OxMediaPlay_Bookmark_t));
}

void om_media_play_CbOnTSRResumed (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType,OxMediaPlay_TsrPaused_e eReason)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSRResumed, "iiii", ulViewId, ulSessionId, eMediaType,eReason);
}

void	om_media_play_CbOnSBTLUpdate (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_SubtitleStatus_t *pstStatus)
{
	if (NULL == pstStatus)
		return;

	HxLOG_Trace();

	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSBTLUpdate, "iiib", ulViewId, ulSessionId, eMediaType, pstStatus, sizeof(OxMediaPlay_SubtitleStatus_t) );
}

#define _____OAPI_COMMAND_FUNCTIONS_____
STATIC HERROR om_media_play_CbActivate (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	STATIC HBOOL			 s_bInitSubModule = FALSE;
	HUINT32				 	 ulActionId, ulViewId = 0;
	omMediaPlay_Context_t	*pstContext	= NULL;
	HUINT32					 ulNumViewAction = NUM_DISPLAY;
	HERROR					 hErr;

	s_ulLastMpAppId ++;

	if (TRUE != s_bInitSubModule)
	{
		/* sub-module*/
		om_media_play_live_Init();
#if defined(CONFIG_MW_MM_PVR)
		om_media_play_tsrpb_Init();
		om_media_play_pvrpb_Init();
#endif
#if defined(CONFIG_MW_MM_MEDIA)
		om_media_play_mediapb_Init();
#endif

		s_bInitSubModule = TRUE;
	}

	ulActionId = MGR_ACTION_GetMainActionId();
	pstContext = om_media_play_GetContext(ulActionId);

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("MGR_ACTION_GetViewIdByActionId err! MainActionID(%d) shall have view ID!!!\n", ulActionId);
	}

	if( NULL == pstContext )
	{
		HxLOG_Error("Not found context 'actionId(%d)' \n", ulActionId);

		HLIB_RPC_Response(s_nRpcHandle, nClientHandle, nRequestId, ERR_OK, "iii", s_ulLastMpAppId, ulNumViewAction, ulViewId);
		return ERR_FAIL;
	}

	pstContext->stCache.eType = pstContext->eMediaType;

	/*finish*/
	HLIB_RPC_Response(s_nRpcHandle, nClientHandle, nRequestId, ERR_OK, "iiib",
												s_ulLastMpAppId,
												ulNumViewAction,
												ulViewId,
												&(pstContext->stCache),
												sizeof(OxMediaPlay_Cache_t) );

	return ERR_OK;
}

STATIC HERROR om_media_play_CbSetMainViewId (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulViewId = (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulActionId;
	Handle_t				 hErr;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewID(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	MGR_ACTION_SetMainActionId(ulActionId);

	/* broadcast */
	om_media_play_CbOnMainViewChanged(ulViewId);
	return ERR_OK;
}

STATIC HERROR om_media_play_CbStart (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId = (HUINT32)HxOBJECT_INT (apArgv[1]);
	HUINT32					 ulActionId;
	OxMediaPlay_StartInfo_t	*punStart = (OxMediaPlay_StartInfo_t *)HxOBJECT_BIN_DATA (apArgv[2]);
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;
#if defined(CONFIG_SUPPORT_FCC)
	HUINT32					ulLiveTakenActionId;
#endif

#if defined(CONFIG_PRISM_ZAPPING)
    MgrLiveView_UiState_e	eViewState;
	MgrLiveLock_UiState_e	eLockState;
	HBOOL					bBlocked = FALSE;
	DxRatingAge_e			eRatingValue = eDxRATING_VIEW_ALL;
#endif

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

#if defined(CONFIG_SUPPORT_FCC)
	ulLiveTakenActionId = ulActionId;
#endif

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
#if defined(CONFIG_SUPPORT_FCC)
		if (punStart->eType != eOxMP_MEDIATYPE_LIVE)
#endif
		{
			pstContext->ulSessionId 	= ulSessionId;

			if (pstContext->eMediaType != punStart->eType)
			{
				switch(pstContext->eMediaType)
				{
					case eOxMP_MEDIATYPE_MEDIAPLAY:
						om_media_play_logicStop(pstContext, ulActionId, ulViewId, ulSessionId, pstContext->eMediaType);

					default:
						break;
				}
			}
			pstContext->eMediaType		= punStart->eType;
		}

		switch (punStart->eType)
		{
		case eOxMP_MEDIATYPE_LIVE:
#if defined(CONFIG_PRISM_ZAPPING)
			if(pstContext->eMediaType == eOxMP_MEDIATYPE_LIVE)
			{
				if(pstContext->stCache.stLive.ulMasterSvcUid == punStart->stLive.ulMasterSvcUid)
				{
					if(pstContext->ulActionId == ulActionId)
					{
						hErr = MGR_LIVE_GetState(ulActionId, eLiveProc_View, (HUINT32 *)&eViewState);
						if(hErr == ERR_OK)
						{
							switch(eViewState)
							{
							case eLiveViewUi_OK:
								// Nothing ...
								hErr = MGR_LIVE_GetState (ulActionId, eLiveProc_Lock, (HUINT32 *)&eLockState);
								if (hErr == ERR_OK)
								{
									switch(eLockState)
									{
									case eOxMP_LiveLockState_OK:
										HxLOG_Print("[%s:%d] SKIP TRY Start Live !!! \n", __FUNCTION__, __LINE__);
										return ERR_FAIL;

									default:
										HxLOG_Print("[%s:%d] TRY Start Live !!! \n", __FUNCTION__, __LINE__);
										break;
									}
								}

								break;

							default:
								break;
							}
						}
					}
				}
			}
#endif
			hErr = om_media_play_live_Start(ulActionId,
#if defined(CONFIG_SUPPORT_FCC)
						ulSessionId,
						&ulLiveTakenActionId,
#endif
						&(punStart->stLive));

#if defined(CONFIG_SUPPORT_FCC)
			// FCC는 Background로 동작해야 하는 것이므로 Context Cache 변화를 일으켜서도 안 되고, MSG도 올려서도 안된다.
			if (eOxMP_LIVETYPE_FCC == punStart->stLive.eLiveType)
			{
				return ERR_OK;
			}

			// 새로운 actionId가 할당되었으니 이에 맞는 context를 다시 가져온다
			pstContext = om_media_play_GetContext(ulLiveTakenActionId);
			if (NULL == pstContext)
			{
				HxLOG_Error("om_media_play_GetContext() failed..!! \n");
			}
			else
			{
				// eMediaType 는 om_media_play_live_Start()에서 이미 적었음
				pstContext->ulSessionId 	= ulSessionId;
			}
#endif
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
			hErr = om_media_play_pvrpb_Start(ulActionId, &(punStart->stPvrPb));
			break;

		case eOxMP_MEDIATYPE_TSRPLAYBACK:
			hErr = om_media_play_tsrpb_Start(ulActionId, &(punStart->stTsrPb));
			break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
		case eOxMP_MEDIATYPE_MEDIAPLAY:
			hErr = om_media_play_mediapb_Start(ulActionId, &(punStart->stMediaPlay));
			break;
#endif

		default:
			hErr = ERR_FAIL;
			break;
		}

		if(pstContext != NULL)
		{
			if(ERR_OK == hErr)
			{
				pstContext->stCache.eState	= eOxMP_PLAYSTATE_PLAYING;
				pstContext->stCache.eError	= eOxMP_PLAYERROR_NO_ERROR;

				/* cache update */
				switch (punStart->eType)
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
			else
			{
				pstContext->stCache.eState	= eOxMP_PLAYSTATE_STOPPED;
				pstContext->stCache.eError	= eOxMP_PLAYERROR_NO_RESOURCE;
			}
		}

		/* broadcast */
		if (ERR_OK == hErr)
		{
			om_media_play_CbOnStarted(ulViewId, ulSessionId, punStart->eType, punStart);
		}
		else
		{
			om_media_play_CbOnPlayErrorChanged(ulViewId, ulSessionId, punStart->eType, eOxMP_PLAYERROR_NO_RESOURCE, 0);
			om_media_play_CbOnStopped(ulViewId, ulSessionId, punStart->eType);
		}
	}

	return ERR_OK;
}

STATIC HERROR om_media_play_CbStop (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId = (HUINT32)HxOBJECT_INT (apArgv[1]);
	OxMediaPlay_MediaType_e  eMediaType = HxOBJECT_INT (apArgv[2]);
	HUINT32					 ulActionId;
	omMediaPlay_Context_t	*pstContext;
	HERROR					 hErr;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	return om_media_play_logicStop(pstContext, ulActionId, ulViewId, ulSessionId, eMediaType);
}

STATIC HERROR om_media_play_CbSetComponentIndex (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	OxMediaPlay_Component_e  eCompType		= (HUINT32)HxOBJECT_INT (apArgv[2]);
	HINT32 					 nCompIndex		= (HINT32)HxOBJECT_INT (apArgv[3]);

	HUINT32					 ulActionId;
	omMediaPlay_Context_t	*pstContext;
	HERROR					 hErr = ERR_FAIL;
	HUINT8					*pstrLang = NULL;

	/* external parameter */
	if (nArgc>=5)
	{
		pstrLang = (HUINT8*)HxOBJECT_STR (apArgv[4]);
	}

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
		case eOxMP_MEDIATYPE_LIVE:
			hErr = om_media_play_live_SetComponentIndex (ulActionId, eCompType, nCompIndex, pstrLang);
			break;
#if defined(CONFIG_MW_MM_PVR)
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
			hErr = om_media_play_pvrpb_SetComponentIndex (ulActionId, eCompType, nCompIndex, pstrLang);
			break;

		case eOxMP_MEDIATYPE_TSRPLAYBACK:
			hErr = om_media_play_tsrpb_SetComponentIndex (ulActionId, eCompType, nCompIndex, pstrLang);
			break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
		case eOxMP_MEDIATYPE_MEDIAPLAY:
			hErr = om_media_play_mediapb_SetComponentIndex (ulActionId, eCompType, nCompIndex);
			break;
#endif

		default:
			hErr = ERR_FAIL;
			break;
		}

		if (ERR_OK == hErr)
		{
			/* broadcast */
			om_media_play_CbOnComponentSelected( ulViewId, ulSessionId, pstContext->eMediaType, eCompType, nCompIndex, -1);
		}
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_CbUnsetComponentIndex (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
    HUINT32                  ulSessionId    = (HUINT32)HxOBJECT_INT (apArgv[0]);
    HUINT32                  ulViewId       = (HUINT32)HxOBJECT_INT (apArgv[1]);
    OxMediaPlay_Component_e  eCompType      = (HUINT32)HxOBJECT_INT (apArgv[2]);
    HINT32                   nCompIndex     = (HINT32)HxOBJECT_INT (apArgv[3]);

    HUINT32                  ulActionId;
    omMediaPlay_Context_t           *pstContext;
    HERROR                   hErr = ERR_FAIL;
    HUINT8                   *pstrLang = NULL;

    /* external parameter */
    if (nArgc>=5)
    {
        pstrLang = (HUINT8*)HxOBJECT_STR (apArgv[4]);
    }

    hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
    if (ERR_OK != hErr)
    {
        HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
        return ERR_FAIL;
    }

    pstContext = om_media_play_GetContext (ulActionId);
    if (NULL != pstContext)
    {
        switch (pstContext->eMediaType)
        {
        case eOxMP_MEDIATYPE_LIVE:
            break;

        case eOxMP_MEDIATYPE_PVRPLAYBACK:
            break;

        case eOxMP_MEDIATYPE_TSRPLAYBACK:
            break;

#if defined(CONFIG_MW_MM_MEDIA)
        case eOxMP_MEDIATYPE_MEDIAPLAY:
            hErr = om_media_play_mediapb_UnsetComponentIndex (ulActionId, eCompType, nCompIndex);
            break;
#endif

        default:
            hErr = ERR_FAIL;
            break;
        }

	if (ERR_OK == hErr)
	{
	    /* broadcast */
	    om_media_play_CbOnComponentUnselected( ulViewId, ulSessionId, pstContext->eMediaType, eCompType, nCompIndex, -1);
	}
    }


    (void)(ulSessionId);

    return ERR_OK;
}

STATIC BUS_Result_t om_media_play_EvtSBTLReset (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulActionId = SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId ;
	OxMediaPlay_SubtitleStatus_t stSubttlStatus = {0,};
	HERROR					 hErr = ERR_FAIL;

	hErr = MGR_ACTION_GetViewIdByActionId( ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetViewIdByActionId err: ulActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	HxLOG_Trace();

	if (NULL == pstContext)	{ return ERR_FAIL; }

	stSubttlStatus.eState = eSUBTTLState_Reset;

	om_media_play_CbOnSBTLUpdate (ulViewId, pstContext->ulSessionId, pstContext->eMediaType, &stSubttlStatus);

	return MESSAGE_PASS;
}

STATIC HERROR om_media_play_CbSetPlaySpeed (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	HINT32 					 nSpeed			= (HUINT32)HxOBJECT_INT (apArgv[2]);

	HUINT32					 ulActionId;
	omMediaPlay_Context_t	*pstContext;
	HERROR					 hErr = ERR_FAIL;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
		case eOxMP_MEDIATYPE_LIVE:
			hErr = om_media_play_live_SetPlaySpeed(ulActionId, nSpeed);
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
			hErr = om_media_play_pvrpb_SetPlaySpeed(ulActionId, nSpeed);
			om_media_play_EvtSBTLReset(pstContext, ulActionId, 0, 0, 0);
			break;

		case eOxMP_MEDIATYPE_TSRPLAYBACK:
			hErr = om_media_play_tsrpb_SetPlaySpeed(ulActionId, nSpeed);
			om_media_play_EvtSBTLReset(pstContext, ulActionId, 0, 0, 0);
			break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
		case eOxMP_MEDIATYPE_MEDIAPLAY:
			hErr = om_media_play_mediapb_SetPlaySpeed(ulActionId, nSpeed);
			break;
#endif
		default:
			hErr = ERR_FAIL;
			break;
		}
	}

	if (ERR_OK == hErr)
	{
		/* broadcast */
//		om_media_play_CbOnSpeedChanged(ulViewId, ulSessionId, pstContext->eMediaType, nSpeed);

		/* Event 정리시 State, Speed 관련 통합 고려 필요 */
//		if( nSpeed == 0 ) 	om_media_play_CbOnPlayStateChanged(ulViewId, ulSessionId,pstContext->eMediaType, eOxMP_PLAYSTATE_PAUSE);
//		else 				om_media_play_CbOnPlayStateChanged(ulViewId, ulSessionId,pstContext->eMediaType, eOxMP_PLAYSTATE_PLAYING);
	}

	return ERR_OK;
}

STATIC HERROR om_media_play_CbSetPlayPosition (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);
#if defined(CONFIG_TIMESTAMP_64)
	HUINT64 				 ullPosition		= (HUINT64)HxOBJECT_INT64 (apArgv[2]);
#else
	HUINT32 				 ulPosition		= (HUINT32)HxOBJECT_INT (apArgv[2]);
#endif
	HUINT32					 ulActionId;
	omMediaPlay_Context_t	*pstContext;
	HERROR					 hErr = ERR_FAIL;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
#if defined(CONFIG_TIMESTAMP_64)
		HxLOG_Print("ulActionId = %d, pstContext->eMediaType = %d, ulPosition = %lld\n", ulActionId, pstContext->eMediaType, ullPosition);
#else
		HxLOG_Print("ulActionId = %d, pstContext->eMediaType = %d, ulPosition = %d\n", ulActionId, pstContext->eMediaType, ulPosition);
#endif

		switch (pstContext->eMediaType)
		{
		case eOxMP_MEDIATYPE_LIVE:
#if defined(CONFIG_TIMESTAMP_64)
			hErr = om_media_play_live_SetPlayPosition (ulActionId, (HUINT32)ullPosition);
#else
			hErr = om_media_play_live_SetPlayPosition (ulActionId, ulPosition);
#endif
			break;
#if defined(CONFIG_MW_MM_PVR)
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
#if defined(CONFIG_TIMESTAMP_64)
			hErr = om_media_play_pvrpb_SetPlayPosition (ulActionId, (HUINT32)ullPosition);
#else
			hErr = om_media_play_pvrpb_SetPlayPosition (ulActionId, ulPosition);
#endif
			om_media_play_EvtSBTLReset(pstContext, ulActionId, 0, 0, 0);
			break;

		case eOxMP_MEDIATYPE_TSRPLAYBACK:
#if defined(CONFIG_TIMESTAMP_64)
			hErr = om_media_play_tsrpb_SetPlayPosition (ulActionId, (HUINT32)ullPosition);
#else
			hErr = om_media_play_tsrpb_SetPlayPosition (ulActionId, ulPosition);
#endif
			om_media_play_EvtSBTLReset(pstContext, ulActionId, 0, 0, 0);
			break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
		case eOxMP_MEDIATYPE_MEDIAPLAY:
#if defined(CONFIG_TIMESTAMP_64)
			hErr = om_media_play_mediapb_SetPlayPosition (ulActionId, ullPosition);
#else
			hErr = om_media_play_mediapb_SetPlayPosition (ulActionId, ulPosition);
#endif
			break;
#endif
		default:
			hErr = ERR_FAIL;
			break;
		}

		if (ERR_OK == hErr)
		{
			/* broadcast */
#if defined(CONFIG_TIMESTAMP_64)
			om_media_play_CbOnPlayTimeChanged( ulViewId, ulSessionId, pstContext->eMediaType, ullPosition, -1 /*ignore*/);
#else
			om_media_play_CbOnPlayTimeChanged( ulViewId, ulSessionId, pstContext->eMediaType, ulPosition, -1 /*ignore*/);
#endif
		}

	}


	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_CbSetPlaySegment (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);
#if defined(CONFIG_MW_MM_MEDIA)
	HUINT32 				 ulSegment		= (HUINT32)HxOBJECT_INT (apArgv[2]);
#endif
	HUINT32					 ulActionId;
	omMediaPlay_Context_t	*pstContext;
	HERROR					 hErr = ERR_FAIL;
#if defined(CONFIG_TIMESTAMP_64)
	HUINT64					 ullPosition = 0;
#else
	HUINT32 				 ulPosition = 0;
#endif

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
		case eOxMP_MEDIATYPE_LIVE:
//			hErr = om_media_play_live_SetPlayPosition (ulActionId, ulPosition);
//			break;
#if defined(CONFIG_MW_MM_PVR)
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
//			hErr = om_media_play_pvrpb_SetPlayPosition (ulActionId, ulPosition);
//			break;

		case eOxMP_MEDIATYPE_TSRPLAYBACK:
//			hErr = om_media_play_tsrpb_SetPlayPosition (ulActionId, ulPosition);
//			break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
		case eOxMP_MEDIATYPE_MEDIAPLAY:
#if defined(CONFIG_TIMESTAMP_64)
			hErr = om_media_play_mediapb_SetPlaySegment(ulActionId, ulSegment, &ullPosition);
#else
			hErr = om_media_play_mediapb_SetPlaySegment(ulActionId, ulSegment, &ulPosition);
#endif
			break;
#endif
		default:
			hErr = ERR_FAIL;
			break;
		}

		if (ERR_OK == hErr)
		{
			/* broadcast */
#if defined(CONFIG_TIMESTAMP_64)
			om_media_play_CbOnPlayTimeChanged( ulViewId, ulSessionId, pstContext->eMediaType, ullPosition, -1 /*ignore*/);
#else
			om_media_play_CbOnPlayTimeChanged( ulViewId, ulSessionId, pstContext->eMediaType, ulPosition, -1 /*ignore*/);
#endif
		}
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_CbSetSubtitleEnable (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR					 hErr = ERR_FAIL;
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	HBOOL 					 bEnable	= (HBOOL)HxOBJECT_INT (apArgv[2]);

	HUINT32					 ulActionId;

	omMediaPlay_Context_t *pstContext	=	NULL;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
		case eOxMP_MEDIATYPE_LIVE:
			hErr = om_media_play_live_SetSubtitleEnable (ulActionId, bEnable);
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
			hErr = om_media_play_pvrpb_SetSubtitleEnable (ulActionId, bEnable);
			break;

		case eOxMP_MEDIATYPE_TSRPLAYBACK:
			hErr = om_media_play_tsrpb_SetSubtitleEnable (ulActionId, bEnable);
			break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
		case eOxMP_MEDIATYPE_MEDIAPLAY:
			hErr = om_media_play_mediapb_SetSubtitleEnable (ulActionId, bEnable);
			break;
#endif

		default:
			hErr = ERR_FAIL;
			break;
		}
	}

	if (ERR_OK == hErr)
	{
		/* broadcast */
		om_media_play_CbOnSubtitleEnable(ulViewId, bEnable);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_CbSetVideoHideEnable (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR					 hErr = ERR_FAIL;
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	HBOOL 					 bEnable	= (HBOOL)HxOBJECT_INT (apArgv[2]);

	HUINT32					 ulActionId;
 	omMediaPlay_Context_t	*pstContext	= NULL;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
		case eOxMP_MEDIATYPE_LIVE:
			hErr = om_media_play_live_SetVideoHideEnable (ulActionId, bEnable);
			break;
#if defined(CONFIG_MW_MM_PVR)
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
			hErr = om_media_play_pvrpb_SetVideoHideEnable (ulActionId, bEnable);
			break;

		case eOxMP_MEDIATYPE_TSRPLAYBACK:
			hErr = om_media_play_tsrpb_SetVideoHideEnable (ulActionId, bEnable);
			break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
		case eOxMP_MEDIATYPE_MEDIAPLAY:
			hErr = om_media_play_mediapb_SetVideoHideEnable (ulActionId, bEnable);
			break;
#endif

		default:
			hErr = ERR_FAIL;
			break;
		}
	}

	if (ERR_OK == hErr)
	{
		/* broadcast */
		om_media_play_CbOnVideoHideEnable(ulViewId, bEnable);
	}

	(void)(ulSessionId);

	return ERR_OK;
}


STATIC HERROR om_media_play_CbSetVideoSize (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	OxMediaPlay_VideoSize_t *pVideoSize		= (OxMediaPlay_VideoSize_t *)HxOBJECT_BIN_DATA(apArgv[2]);

	HUINT32					 ulActionId;
	omMediaPlay_Context_t	*pstContext;
	HERROR					 hErr = ERR_FAIL;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
		PIG_Rect_t stVidWindow;

		HxSTD_memset(&stVidWindow,0,sizeof(PIG_Rect_t));

		stVidWindow.stOutRect.nX	= pVideoSize->nStartX;
		stVidWindow.stOutRect.nY	= pVideoSize->nStartY;
		stVidWindow.stOutRect.nW	= pVideoSize->nWidth;
		stVidWindow.stOutRect.nH	= pVideoSize->nHeight;
		stVidWindow.ulOutRefWidth	= pVideoSize->nMaxWidth;
		stVidWindow.ulOutRefHeight	= pVideoSize->nMaxHeight;
		stVidWindow.bRoundOff           = pVideoSize->bRoundOff;
		stVidWindow.bForceSourceBase    = pVideoSize->bForceSourceBase;

		hErr = SVC_OUT_SetOsdLayerOutput(eOsdZOrder_Ui, eDxASPECTRATIO_UNKNOWN, eDxDFC_SELECT_UNKNOWN, stVidWindow, eDxASPECTRATIO_UNKNOWN);
		if ( hErr != ERR_OK )
		{
			HxLOG_Error ("Error :\n");
		}
	}

	if (ERR_OK == hErr)
	{
		/* broadcast */
		om_media_play_CbOnVideoSize(ulViewId, pVideoSize);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_CbResetVideoSize (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	HUINT32					 ulActionId;
	omMediaPlay_Context_t	*pstContext;
	HERROR					 hErr = ERR_FAIL;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
		PIG_Rect_t stVidWindow;

		HxSTD_MemSet(&stVidWindow,0,sizeof(PIG_Rect_t));

		hErr = SVC_OUT_SetOsdLayerOutput(eOsdZOrder_Ui, eDxASPECTRATIO_UNKNOWN, eDxDFC_SELECT_UNKNOWN, stVidWindow, eDxASPECTRATIO_UNKNOWN);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_OUT_SetOsdLayerOutput error.\n");
		}
	}

	if (ERR_OK == hErr)
	{
		/* broadcast */
		//om_media_play_CbOnResetVideoSize(ulViewId);
	}

	(void)(ulSessionId);
	(void)(ulActionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_CbSetPigRect (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	OxMediaPlay_PigSize_t 	*pPigSize		= (OxMediaPlay_PigSize_t *)HxOBJECT_BIN_DATA(apArgv[2]);

	HUINT32					 ulActionId;
	omMediaPlay_Context_t	*pstContext;
	HERROR					 hErr = ERR_FAIL;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
		PIG_Rect_t	stVidWindow;

		if (NULL == pPigSize)					{ return ERR_FAIL; }

		HxSTD_MemSet(&stVidWindow,0,sizeof(PIG_Rect_t));

		// inRect
		stVidWindow.stInRect.nX 	= pPigSize->nInRect_StartX;
		stVidWindow.stInRect.nY 	= pPigSize->nInRect_StartY;
		stVidWindow.stInRect.nW 	= pPigSize->nInRect_Width;
		stVidWindow.stInRect.nH 	= pPigSize->nInRect_Height;
		stVidWindow.ulInRefWidth	= pPigSize->nInRect_RefWidth;
		stVidWindow.ulInRefHeight	= pPigSize->nInRect_RefHeight;

		// outRect
		stVidWindow.stOutRect.nX	= pPigSize->nOutRect_StartX;
		stVidWindow.stOutRect.nY	= pPigSize->nOutRect_StartY;
		stVidWindow.stOutRect.nW	= pPigSize->nOutRect_Width;
		stVidWindow.stOutRect.nH	= pPigSize->nOutRect_Height;
		stVidWindow.ulOutRefWidth	= pPigSize->nOutRect_RefWidth;
		stVidWindow.ulOutRefHeight	= pPigSize->nOutRect_RefHeight;

		// Default false; This is for rounding-off flag used at Netflix VideoResize
		stVidWindow.bRoundOff          = FALSE; /* Unnecessary param here; om_media_play_CbSetVideoSize() 통한 bRoundOff만 반영 */
		stVidWindow.bForceSourceBase   = FALSE; /* Unnecessary param here; om_media_play_CbSetVideoSize() 통한 bForceSourceBase만 반영 */

		hErr = SVC_OUT_SetOsdLayerOutput(pPigSize->ulZOrder, pPigSize->eOsdAspectRatio, pPigSize->eDfc, stVidWindow, pPigSize->ePigAspectRatio);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("Error :\n");
		}
	}

#if !defined(CONFIG_3RD_MHEG)
	// 사용하지도 않는데..왜 om cache로 다시 올려줄까요???
	if (ERR_OK == hErr)
	{
		/* broadcast */
		om_media_play_CbOnPigRect(ulViewId, pPigSize);
	}
#endif
	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_CbSaveThumbnail (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

#if defined(CONFIG_MW_MM_MEDIA)
	HUINT32 				 ulWidth		= (HUINT32)HxOBJECT_INT (apArgv[2]);
	HUINT32 				 ulHeight		= (HUINT32)HxOBJECT_INT (apArgv[3]);
	HCHAR 					*pszFilename	= (HCHAR*) HxOBJECT_STR (apArgv[4]);
#endif

	HUINT32					 ulActionId;
	omMediaPlay_Context_t	*pstContext;
	HERROR					 hErr = ERR_FAIL;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	pstContext = om_media_play_GetContext(ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			break;//N/A
#if defined(CONFIG_MW_MM_PVR)
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	break;//N/A
#endif
#if defined(CONFIG_MW_MM_MEDIA)
			case eOxMP_MEDIATYPE_MEDIAPLAY:		hErr = om_media_play_mediapb_SaveThumbnail( ulActionId, pszFilename, ulWidth, ulHeight ); break;
#endif

			default:
				hErr = ERR_FAIL;
				break;
		}
	}

	if (ERR_OK == hErr)
	{
		/* broadcast */
//		om_media_play_CbOnSaveThumnail(ulViewId);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_CbGetOsdDisplayPoint (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
//	HUINT32		ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HINT32		ulRefWidth		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	HINT32		ulRefHeight		= (HUINT32)HxOBJECT_INT (apArgv[2]);
	HINT32		nVideoPointX	= (HUINT32)HxOBJECT_INT (apArgv[3]);
	HINT32		nVideoPointY	= (HUINT32)HxOBJECT_INT (apArgv[4]);

	HERROR		hErr = ERR_FAIL;
	HINT32		nScaledX=0, nScaledY=0;

	hErr = SVC_OUT_GetOsdDisplayPoint(ulRefWidth, ulRefHeight, nVideoPointX, nVideoPointY, &nScaledX, &nScaledY);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_OUT_GetOsdDisplayPoint failed. \n");
	}

	HLIB_RPC_Response(s_nRpcHandle, nClientHandle, nRequestId, ERR_OK, "ii", nScaledX, nScaledY);

	return ERR_OK;
}

STATIC void om_media_play_CaptureStillCallback(HUINT8 *pucFrameBuffer, HUINT32 ulWidth, HUINT32 ulHeight, HUINT32 ulBufferSize)
{
}

STATIC HERROR om_media_play_CbCaptureStillPicture (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR 				hErr 			= ERR_FAIL;
	HUINT32				ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
	DxVideoCodec_e		eVideoCodec		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	HUINT32				ulCaptureWidth	= (HUINT32)HxOBJECT_INT (apArgv[2]);
	HUINT32				ulCaptureHeight = (HUINT32)HxOBJECT_INT (apArgv[3]);
	PixelFormat_e		ePixelFormat	= (HUINT32)HxOBJECT_INT (apArgv[4]);
	HUINT32				ulBufferSize	= (HUINT32)HxOBJECT_INT (apArgv[5]);
	HUINT8				*pBuffer		= (HUINT8*)HxOBJECT_BIN_DATA (apArgv[6]);
	HUINT32				ulActionId		= eActionId_VIEW_FIRST + ulViewId;
	Handle_t			hAction			= SVC_PIPE_GetActionHandle(ulActionId);

	hErr = SVC_AV_CaptureStillPicture(hAction, pBuffer, ulBufferSize, eVideoCodec, ulCaptureWidth, ulCaptureHeight, ePixelFormat, om_media_play_CaptureStillCallback);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_CaptureStillPicture err.\n");
	}

	return hErr;
}

STATIC HERROR om_media_play_CbStartStillPicture (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR 				hErr 			= ERR_FAIL;
	HUINT32				ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
	DxVideoCodec_e		eVideoCodec		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	HUINT32				ulBufferSize	= (HUINT32)HxOBJECT_INT (apArgv[2]);
	HUINT8				*pBuffer		= (HUINT8*)HxOBJECT_BIN_DATA (apArgv[3]);
	HUINT32				ulActionId		= eActionId_VIEW_FIRST + ulViewId;
	Handle_t			hAction			= SVC_PIPE_GetActionHandle(ulActionId);

	hErr = SVC_AV_StartStillPicture(hAction, pBuffer, ulBufferSize, eVideoCodec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_StartStillPicture err.\n");
	}

	return hErr;
}

STATIC HERROR om_media_play_CbStopStillPicture (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR 				hErr 			= ERR_FAIL;
	HUINT32				ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32				ulActionId		= eActionId_VIEW_FIRST + ulViewId;
	Handle_t			hAction			= SVC_PIPE_GetActionHandle(ulActionId);

	hErr = SVC_AV_StopStillPicture(hAction);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_StopStillPicture err.\n");
	}

	return hErr;
}

STATIC HERROR om_media_play_CbStartAudioClip (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR 				hErr 			= ERR_FAIL;
	HUINT32				ulRequestId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32				ulBufferBytes	= (HUINT32)HxOBJECT_INT (apArgv[1]);
	DxAudioCodec_e		eCodec			= (HUINT32)HxOBJECT_INT (apArgv[2]);
	HUINT32				ulLoopCnt    	= (HUINT32)HxOBJECT_INT (apArgv[3]);

	HUINT8 				*pucBuffer		= (HUINT8*)HxOBJECT_BIN_DATA (apArgv[4]);

	if (NULL == pucBuffer)
		return ERR_FAIL;


	hErr = SVC_AV_StartAudioClip (ulRequestId, pucBuffer, ulBufferBytes, eCodec, ulLoopCnt);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_StartAudioClip err.\n");
	}

	return hErr;
}

STATIC HERROR om_media_play_CbPauseAudioClip (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR 				hErr 			= ERR_FAIL;
	HUINT32				ulRequestId		= (HUINT32)HxOBJECT_INT (apArgv[0]);

	hErr = SVC_AV_PauseAudioClip (ulRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_PauseAudioClip err.\n");
	}

	return hErr;
}

STATIC HERROR om_media_play_CbResumeAudioClip (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR 				hErr 			= ERR_FAIL;
	HUINT32				ulRequestId		= (HUINT32)HxOBJECT_INT (apArgv[0]);

	hErr = SVC_AV_ResumeAudioClip (ulRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_ResumeAudioClip err.\n");
	}

	return hErr;
}

STATIC HERROR om_media_play_CbStopAudioClip (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR 				hErr 			= ERR_FAIL;
	HUINT32				ulRequestId		= (HUINT32)HxOBJECT_INT (apArgv[0]);

	hErr = SVC_AV_StopAudioClip (ulRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_StopAudioClip err.\n");
	}

	return hErr;
}

STATIC HERROR om_media_play_CbSetVideoFreeze (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR 				hErr 			= ERR_FAIL;
	HUINT32				ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HBOOL				bFreeze			= (HBOOL)HxOBJECT_INT (apArgv[1]);

	HUINT32				ulActionId;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	hErr = SVC_AV_SetVideoFreeze (ulActionId, bFreeze);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_SetVideoFreeze err.\n");
	}

	return hErr;
}

STATIC HERROR om_media_play_CbSetAudioFreeze (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR 				hErr 			= ERR_FAIL;
	HUINT32				ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HBOOL				bFreeze			= (HBOOL)HxOBJECT_INT (apArgv[1]);

	HUINT32				ulActionId;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	hErr = SVC_AV_SetAudioFreeze (ulActionId, bFreeze);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_SetAudioFreeze err.\n");
	}

	return hErr;
}

STATIC HERROR om_media_play_CbSetTempAvPid (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR					 hErr			= ERR_FAIL;
//	HUINT32 				 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32 				 ulViewId		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	DxVideoCodec_e 			 eVideoCodec 	= (DxVideoCodec_e)HxOBJECT_INT (apArgv[2]);
	HUINT16 				 usVideoPid 	= (HUINT16)HxOBJECT_INT (apArgv[3]);
	DxAudioCodec_e 			 eAudioCodec	= (DxAudioCodec_e)HxOBJECT_INT (apArgv[4]);
	HUINT16 				 usAudioPid		= (HUINT16)HxOBJECT_INT (apArgv[5]);
	HUINT16 				 usPcrPid		= (HUINT16)HxOBJECT_INT (apArgv[6]);
	HUINT32					 ulActionId;

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetActionIdByViewId err: ViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	hErr = MGR_LIVE_CmdChangeTempPid(SVC_PIPE_GetActionHandle(ulActionId), eVideoCodec, usVideoPid, eAudioCodec, usAudioPid, usPcrPid);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_CmdChangeTempPid err. actionId(%d) \n", ulActionId);
	}

	return hErr;
}

STATIC HERROR om_media_play_CbSetMhegStatus (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
#if defined(CONFIG_3RD_MHEG)
	OxMediaPlay_MhegStatus_e	eStatus = (OxMediaPlay_MhegStatus_e)HxOBJECT_INT (apArgv[0]);

	MGR_ACTION_SetMhegStatus((MgrAction_MhegStatus_e) eStatus);
	BUS_SendMessage (NULL, eMEVT_LIVE_SET_MHEG_STATUS_CHANGE,
									0,
									(HINT32)eStatus,
									0,
									0);
#endif

	return ERR_OK;
}

STATIC HERROR om_media_play_CbSetMhegInterruptibleFlag (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
#if defined(CONFIG_3RD_MHEG)
	om_media_play_live_SetMhegInterruptibleFlag((HBOOL)HxOBJECT_INT (apArgv[0])) ;
#endif

	return ERR_OK;
}

STATIC HERROR om_media_play_CbRequestPincode (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					ulViewId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	OxMediaPlay_MediaType_e eMediaType	= (OxMediaPlay_MediaType_e)HxOBJECT_INT (apArgv[1]);
	HUINT8					*pszMsg		= (HUINT8*)HxOBJECT_STR (apArgv[2]);

	HxLOG_Print("\033[30;43m[%s : %d] Enter Pincode : %s  \033[0m\n", __FUNCTION__, __LINE__, pszMsg);

	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnRequestPincode, "iis", ulViewId, eMediaType, pszMsg);

	return ERR_OK;
}

STATIC HERROR om_media_play_CbResultPincode (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HBOOL					bPermit	= (HUINT32)HxOBJECT_INT (apArgv[0]);

	HxLOG_Print("\033[30;43m[%s : %d] Result Pincode : %d  \033[0m\n", __FUNCTION__, __LINE__, bPermit);

	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResultPincode, "i", bPermit);

	return ERR_OK;
}

STATIC HERROR om_media_play_CbStopHbbtvMonitoring (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	BUS_PostMessage(NULL, eMEVT_HBBTV_STOP_MONITORING, HANDLE_NULL, 0, 0, 0);
	return ERR_OK;
}

STATIC HERROR om_media_play_CbGetDefaultSupportedSpeed (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HINT32	*pnSpeedArray	= NULL;
	HUINT32	ulSpeedNum		= 0;
#if defined(CONFIG_MW_MM_PVR)
	HERROR	hErr;
	HBOOL	bScrambled		= (HINT32)HxOBJECT_INT (apArgv[0]);
#endif

#if defined(CONFIG_MW_MM_PVR)
	hErr = SVC_PB_GetSupportSpeeds ((Handle_t)0, bScrambled, &ulSpeedNum, &pnSpeedArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_PB_GetSupportSpeeds failed. \n");
	}

	HLIB_RPC_Response(s_nRpcHandle, nClientHandle, nRequestId, ERR_OK, "ib", ulSpeedNum, pnSpeedArray, sizeof(HINT32) * ulSpeedNum);
#else
	HxLOG_Error("Not Support This Function - SVC_PB_GetSupportSpeeds. \n");
	HLIB_RPC_Response(s_nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, NULL);
#endif

	return ERR_OK;
}

#define _____MSG_PROCESS_FUNCTIONS_____
// Create와 Destroy 만은 Context가 NULL일 수 있다.
STATIC BUS_Result_t om_media_play_MsgBusCreate (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hAction);

	if (NULL == pstContext)
	{
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

	HxSTD_MemSet (pstContext, 0, sizeof(omMediaPlay_Context_t));

	pstContext->ulActionId	= ulActionId;
	pstContext->eMediaType	= eOxMP_MEDIATYPE_NONE;

	pstContext->stCache.stAitSecData.usCurrAitPid = PID_NULL;
	pstContext->stCache.stAitSecData.usPrevAitPid = PID_NULL;

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_media_play_MsgBusDestroy (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (NULL == pstContext)
	{
		return MESSAGE_BREAK;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_media_play_MsgMgrActionTaken (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32						 ulViewId = 0, ulSessionId;
	HUINT32						 ulActionId = SVC_PIPE_GetActionId(hAction);
	MgrAction_Type_e			 eActType = (MgrAction_Type_e)p1;
	OxMediaPlay_MediaType_e		 eMediaType;
	HERROR						 hErr;

	if (NULL == pstContext)
	{
		return MESSAGE_PASS;
	}

	// View ID가 제대로 할당된 Action만 (Main View, PIP 등) 동작한다.
	hErr = MGR_ACTION_GetViewIdByActionId(pstContext->ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	ulSessionId = pstContext->ulSessionId;

	eMediaType = pstContext->eMediaType;
	if (ulActionId == pstContext->ulActionId)
	{
		switch (eActType)
		{
		case eViewType_WTV:
		case eViewType_PIP:
		case eViewType_FCC:
			eMediaType = eOxMP_MEDIATYPE_LIVE;
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eViewType_PB:
			eMediaType = eOxMP_MEDIATYPE_PVRPLAYBACK;
			break;

		case eViewType_CHASE_PB:
			switch(pstContext->eMediaType)
			{
				case eOxMP_MEDIATYPE_PVRPLAYBACK:
					eMediaType = eOxMP_MEDIATYPE_PVRPLAYBACK;
					break;
				case eOxMP_MEDIATYPE_TSRPLAYBACK:
				default:
					eMediaType = eOxMP_MEDIATYPE_TSRPLAYBACK;
					break;
			}
			break;

		case eViewType_TSR_PB:
			eMediaType = eOxMP_MEDIATYPE_TSRPLAYBACK;
			break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
		case eViewType_MEDIA:
		case eViewType_MOVIE_PB:
		case eViewType_RFVOD:
			eMediaType = eOxMP_MEDIATYPE_MEDIAPLAY;
			break;
#endif

		default:
			eMediaType = eOxMP_MEDIATYPE_NONE;
			break;
		}

		if (eMediaType != pstContext->eMediaType)
		{
			/*
			 * 현재 MediaType이 MEDIAPLAY일 경우에는 Stop처리 함.
			 */
			if (eOxMP_MEDIATYPE_MEDIAPLAY == eMediaType)
			{
				om_media_play_logicStop(pstContext, ulActionId, ulViewId, ulSessionId, eMediaType);
			}

			pstContext->eMediaType		=	eMediaType;
			pstContext->bActionTaken	=	TRUE;
			pstContext->pExtension		=	NULL;
		}

		/* boardcast */
		om_media_play_CbOnResourceTaken(ulViewId, ulSessionId, pstContext->eMediaType);
	}

	/* live conflict check */
	om_media_play_live_UpdateConflictInfoChanged(pstContext, hAction, p1, p2, p3);

	return MESSAGE_PASS;
}


STATIC BUS_Result_t om_media_play_MsgMgrActionRelease (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32						 ulViewId = 0, ulSessionId;
	HUINT32						 ulActionId	= SVC_PIPE_GetActionId (hAction);
	HERROR						 hErr;

	if (NULL == pstContext)
	{
		return MESSAGE_BREAK;
	}

	// View ID가 제대로 할당된 Action만 (Main View, PIP 등) 동작한다.
	hErr = MGR_ACTION_GetViewIdByActionId(pstContext->ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	ulSessionId =	pstContext->ulSessionId;

	if (ulActionId == pstContext->ulActionId)
	{
		/* boardcast */
		om_media_play_CbOnResourceLost(ulViewId, ulSessionId, pstContext->eMediaType);

		pstContext->bActionTaken	=	FALSE;
	}

	/* live conflict check */
	om_media_play_live_UpdateConflictInfoChanged(pstContext, hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_MsgMgrActionPriorityChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32						 ulViewId = 0;
	HERROR						 hErr;

	// View ID가 제대로 할당된 Action만 (Main View, PIP 등) 동작한다.
	hErr = MGR_ACTION_GetViewIdByActionId(pstContext->ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		return MESSAGE_PASS;
	}

	/* live conflict check */
	om_media_play_live_UpdateConflictInfoChanged(pstContext, hAction, p1, p2, p3);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_MsgMgrNotifyRecordStarted (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrRec_RecordType_e eRecType = (MgrRec_RecordType_e)p1;

	if (eMgrRecType_TSR == eRecType)
	{
		s_ulTsrRecActionId = SVC_PIPE_GetActionId (hAction);
	}
	else if (eMgrRecType_DELAYED== eRecType)
	{
		s_ulTsrRecActionId = 0;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_MsgMgrNotifyRecordStopped (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrRec_RecordType_e eRecType = (MgrRec_RecordType_e)p1;

	if (eMgrRecType_TSR == eRecType)
	{
		s_ulTsrRecActionId = 0;
	}
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_EvtRecTimecodeChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

#if defined(CONFIG_MW_MM_PVR)
	HUINT32						 ulRecActId = SVC_PIPE_GetActionId(hAction);

	if (ulRecActId == s_ulTsrRecActionId)
	{
		HUINT32						 ulMainActionId = MGR_ACTION_GetMainActionId();
		HUINT32						 ulViewId =	0;
		HUINT32						 ulLimitDuration = MGR_RECORDER_GetRecorderParam(eMgrRec_DEFAULT_DURATION_TSR_RECORD);
		HUINT32						 ulStartRecTime	=	0;
		HUINT32						 ulDuration = 0;
		MgrRec_RecInfo_t			 stInfo;
		HERROR						 hErr ;

		hErr = MGR_ACTION_GetViewIdByActionId(ulMainActionId, &ulViewId);
		if (ERR_OK != hErr)
		{
			return MESSAGE_PASS;
		}

		HxSTD_memset(&stInfo, 0x00, sizeof(MgrRec_RecInfo_t));
		hErr = MGR_RECORDER_GetRecordingSvcInfo(ulRecActId, &stInfo);
		if(hErr == ERR_OK)
		{
			ulStartRecTime	=	stInfo.ulStartTime;
			ulDuration		=	stInfo.ulRecTime;

			if( ulDuration > ulLimitDuration)
			{
				ulDuration = ulLimitDuration;
			}

			om_media_play_CbOnTSRTimecodeChanged(ulViewId, 0, eOxMP_MEDIATYPE_LIVE, ulStartRecTime, ulDuration, ulLimitDuration);
		}
	}
#endif
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_MsgTsrPaused (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulRecActId = SVC_PIPE_GetActionId(hAction);

	if (ulRecActId == s_ulTsrRecActionId)
	{
		OxMediaPlay_Bookmark_t	stPausedPoint;
		HUINT32					ulMainActionId = MGR_ACTION_GetMainActionId();
		HUINT32					ulViewId =	0;
		Handle_t				hErr;

		hErr = MGR_ACTION_GetViewIdByActionId(ulMainActionId, &ulViewId);
		if (ERR_OK != hErr)
		{
			return MESSAGE_PASS;
		}

		stPausedPoint.ulPos = (HUINT32)p2;
		HxSTD_snprintf(stPausedPoint.szName,OxMP_PVR_BOOKMARK_NAME_LEN,"%s",(HCHAR *)p3);

		om_media_play_CbOnTSRPaused(ulViewId, 0, eOxMP_MEDIATYPE_LIVE, p1, &stPausedPoint);
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_MsgTsrResumed (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulRecActId = SVC_PIPE_GetActionId(hAction);

	if (ulRecActId == s_ulTsrRecActionId)
	{
		HUINT32					ulMainActionId = MGR_ACTION_GetMainActionId();
		HUINT32					ulViewId =	0;
		Handle_t				hErr;

		hErr = MGR_ACTION_GetViewIdByActionId(ulMainActionId, &ulViewId);
		if (ERR_OK != hErr)
		{
			return MESSAGE_PASS;
		}

		om_media_play_CbOnTSRResumed(ulViewId, 0, eOxMP_MEDIATYPE_LIVE, p1);
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_SetDrmInformation (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();

	if((MgrMedia_DrmInformation_t*)p3 == NULL)
	{
		HxLOG_Error("DRM info is NULL \n");
		return MESSAGE_BREAK;
	}

	HxSTD_MemSet(&(pstContext->stOmCache.stDrmInformation), 0x00, sizeof(MgrMedia_DrmInformation_t));
	HxSTD_MemCopy(&(pstContext->stOmCache.stDrmInformation), (MgrMedia_DrmInformation_t*)p3, sizeof(MgrMedia_DrmInformation_t));

	return MESSAGE_PASS;
}
STATIC HERROR om_media_play_ParseDrmMessageType(HUINT8 * pucMsgType, e_MgrDrmAgent_SendMsgType_t *etMsgType)
{
	HUINT16 i = 0;
	*etMsgType = eMGR_DRMAGENT_MSGTYPE_MAX;

	if(pucMsgType == NULL)
	{
		HxLOG_Error("pucMsgType is NULL\n");
		return ERR_FAIL;
	}

	for(i = 0; i < eMGR_DRMAGENT_MSGTYPE_MAX; i++) // TODO:
	{
		if(HxSTD_StrCmp((const HUINT8 *)pucMsgType, (const HUINT8 *)stDrmType[i].ucStr) == 0)
		{
			*etMsgType = stDrmType[i].eMsgType;
			HxLOG_Error("Message Type = %d\n", *etMsgType);
		}
	}

	if(*etMsgType == eMGR_DRMAGENT_MSGTYPE_MAX)
	{
		HxLOG_Error("eMsgType is error\n");
	}
	return ERR_OK;
}

STATIC HERROR om_media_play_LoadDrmAgentElementList(DRMAGENT_ELEMENT_t **ppstElementList)
{
	if(ppstElementList == NULL)
	{
		return ERR_FAIL;
	}
	*ppstElementList = (DRMAGENT_ELEMENT_t *)&s_Element_PlayReadyInitiator;
	return ERR_OK;
}

STATIC HERROR om_media_play_MakeFieldOfDRMResultMsg(HUINT8 **pucMsg, HUINT32 *sizeofbuffer, e_MgrDrmAgent_XmlStructure_t tag, e_MgrDrmAgent_XmlStructure_t attribute, HUINT8 *pDrmResultElement)
{
	HUINT8	tagName[128] = "";
	HUINT8 *pTemp = NULL;
	HINT32 needlength = 0;

	if( pucMsg==NULL || *pucMsg == NULL)
	{
		HxLOG_Error("Invalid param \n");
		return ERR_FAIL;
	}

	needlength = HxSTD_StrLen(*pucMsg) + HxSTD_StrLen(st_DrmResultMsgField[tag].ucStr) + HxSTD_StrLen(st_DrmResultMsgField[attribute].ucStr);
	if(pDrmResultElement != NULL)
		needlength += HxSTD_StrLen(pDrmResultElement);

	if(*sizeofbuffer < needlength)
	{
		HUINT8 *newptr = NULL;
		*sizeofbuffer = needlength+1;
		newptr = HLIB_STD_MemAlloc(*sizeofbuffer);
		if(!newptr)
		{
			HxLOG_Error("Out Of Memory \n");
			return ERR_FAIL;
		}
		HxSTD_MemSet(newptr, 0, *sizeofbuffer);
		HxSTD_StrCpy(newptr, *pucMsg);
		HLIB_STD_MemFree(*pucMsg);
		*pucMsg = newptr;
	}

	if(tag!=eMGR_DRMAGENT_XML_TAG_NONE)// Tag Insert
	{
		HxSTD_StrCat(*pucMsg, st_DrmResultMsgField[tag].ucStr);
	}
	if(attribute!=eMGR_DRMAGENT_XML_ATTRIBUTE_NONE)// Attribute Insert
		{
		if(tag==eMGR_DRMAGENT_XML_TAG_NONE)
		{ // must specify tag value
			HxLOG_Error("Must Specify tag value \n");
			return ERR_FAIL;
		}
		HxSTD_StrNCpy(tagName, st_DrmResultMsgField[tag].ucStr+1, HxSTD_StrLen(st_DrmResultMsgField[tag].ucStr)-2);  // 1='<',  2= '<', '>'
		pTemp = HxSTD_StrStr(*pucMsg, tagName);
		if( pTemp == NULL )
		{
			HxLOG_Error("Not Find \n");
			return ERR_FAIL ;
		}
		pTemp += HxSTD_StrLen(tagName);
		HxSTD_StrNCpy(pTemp, " ",HxSTD_StrLen(" ")+1);

		pTemp += HxSTD_StrLen(" ");
		HxSTD_StrNCpy(pTemp, st_DrmResultMsgField[attribute].ucStr, HxSTD_StrLen(st_DrmResultMsgField[attribute].ucStr)+1);

		pTemp += HxSTD_StrLen(st_DrmResultMsgField[attribute].ucStr);
		HxSTD_StrNCpy(pTemp, ">", HxSTD_StrLen(" ")+1); // 1 = to insert null.

		//HxSTD_StrCat(*pucMsg, ">");
	}

	if(pDrmResultElement!=NULL)// Element Insert
	{
		if(tag==eMGR_DRMAGENT_XML_TAG_NONE)
		{
			HxLOG_Error("Must Specify tag value \n");
			return ERR_FAIL;
		}
		pTemp = HxSTD_StrStr(*pucMsg, st_DrmResultMsgField[tag].ucStr);
		if( pTemp == NULL )
		{
			HxLOG_Error("Not Find \n");
			return ERR_FAIL;
	}
		HxSTD_StrNCpy(pTemp+HxSTD_StrLen(st_DrmResultMsgField[tag].ucStr), pDrmResultElement, HxSTD_StrLen(pDrmResultElement)+1);
	}
	return ERR_OK;
	}


STATIC HERROR om_media_play_ParseAndProcessPRDYDrmMessage(omMediaPlay_Context_t *pstContext, HUINT8 * pucDrmMsg, e_MgrDrmAgent_ResultCodeType_t* etResultCode, HUINT8* pucResultElement, HUINT32 sizeofbuffer)
	{
	HUINT8 firstElement[128]={0,};
	HUINT8 lastElement[128]={0,};
	HUINT8 *firstsearched ;
	HUINT8 *lastsearched;
	int length;
	HERROR			eResult = ERR_OK;

	if(HxSTD_StrStr(pucDrmMsg, "LicenseServerUriOverride") != NULL)
	{
		HxSTD_StrCpy(firstElement, "<LA_URL>");
		HxSTD_StrCpy(lastElement, "</LA_URL>");
		firstsearched = HxSTD_StrStr(pucDrmMsg, firstElement);
		lastsearched = HxSTD_StrStr(pucDrmMsg, lastElement);
		if(firstsearched == NULL || lastsearched == NULL)
		{
			*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT ;
			return ERR_FAIL;
		}
		length = HxSTD_StrLen(firstsearched) - HxSTD_StrLen(lastsearched) - HxSTD_StrLen(firstElement);
		if(pstContext->stCache.pucLAURL != NULL)
		{
			HLIB_STD_MemFree(pstContext->stCache.pucLAURL);
			pstContext->stCache.pucLAURL = NULL;
		}
		if(length>0)
			{
			pstContext->stCache.pucLAURL = HLIB_STD_MemAlloc(length+1);
			if(pstContext->stCache.pucLAURL == NULL)
			{
				*etResultCode = eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR ;
				return ERR_FAIL;
			}
			HxSTD_MemSet(pstContext->stCache.pucLAURL, 0, length+1);
			HxSTD_memcpy(pstContext->stCache.pucLAURL, firstsearched+HxSTD_StrLen(firstElement), length);
			}
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_SUCCESSFUL ;
		eResult = ERR_OK;
		}
	else if(HxSTD_StrStr(pucDrmMsg, "SetCustomData") != NULL)
	{
		HxSTD_StrCpy(firstElement, "<CustomData>");
		HxSTD_StrCpy(lastElement, "</CustomData>");
		firstsearched = HxSTD_StrStr(pucDrmMsg, firstElement);
		lastsearched = HxSTD_StrStr(pucDrmMsg, lastElement);
		if(firstsearched == NULL || lastsearched == NULL)
		{
			*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT ;
			return ERR_FAIL;
		}
		length = HxSTD_StrLen(firstsearched) - HxSTD_StrLen(lastsearched) - HxSTD_StrLen(firstElement);
		if(pstContext->stCache.pucPRDYCustomData != NULL)
	{
			HLIB_STD_MemFree(pstContext->stCache.pucPRDYCustomData);
			pstContext->stCache.pucPRDYCustomData = NULL;
	}
		if(length>0)
	{
			pstContext->stCache.pucPRDYCustomData = HLIB_STD_MemAlloc(length+1);
			if(pstContext->stCache.pucPRDYCustomData == NULL)
			{
				*etResultCode = eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR ;
				return ERR_FAIL;
			}
			HxSTD_MemSet(pstContext->stCache.pucPRDYCustomData, 0, length+1);
			HxSTD_memcpy(pstContext->stCache.pucPRDYCustomData, firstsearched+HxSTD_StrLen(firstElement), length);
	}
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_SUCCESSFUL ;
	eResult = ERR_OK;

	}
	else
	{
		HxLOG_Error("Not yet supported \n");
		//In case of need of pucResultElement size calcuration, size compair should be needed.
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT ;
		eResult = ERR_FAIL;
	}
	return eResult;
}

STATIC HERROR om_media_play_ParseAndProcessVRWebDrmMessage(omMediaPlay_Context_t *pstContext, HUINT8 * pucDrmMsg, e_MgrDrmAgent_ResultCodeType_t* etResultCode, HUINT8* pucResultElement, HUINT32 sizeofbuffer)
{
	HUINT8	firstElement[128]={0,};
	HUINT8	lastElement[128]={0,};
	HUINT8	*firstsearched ;
	HUINT8	*lastsearched;
	HUINT8	*pucURL = NULL;
	HUINT8	*pucVendorName = NULL;
	int 	length;
	HERROR	hErr = ERR_OK;

	HxLOG_Trace();

	if(HxSTD_StrStr(pucDrmMsg, "SetVmxCas") != NULL)
	{
		HxSTD_StrCpy(firstElement, "<CAS_URL>");
		HxSTD_StrCpy(lastElement, "</CAS_URL>");
		firstsearched = HxSTD_StrStr(pucDrmMsg, firstElement);
		lastsearched = HxSTD_StrStr(pucDrmMsg, lastElement);
		if(firstsearched == NULL || lastsearched == NULL)
		{
			*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT ;
			return ERR_FAIL;
		}
		length = HxSTD_StrLen(firstsearched) - HxSTD_StrLen(lastsearched) - HxSTD_StrLen(firstElement);

		if(length>0)
		{
			pucURL = (HUINT8 *)HLIB_STD_MemAlloc(length+1);
			HxSTD_MemSet(pucURL, 0, length+1);
			HxSTD_memcpy(pucURL, firstsearched+HxSTD_StrLen(firstElement), length);
		}
		else
		{
			*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT ;
			return ERR_FAIL;
		}

		HxSTD_MemSet(firstElement, 0, 128);
		HxSTD_MemSet(lastElement, 0, 128);

		HxSTD_StrCpy(firstElement, "<VendorName>");
		HxSTD_StrCpy(lastElement, "</VendorName>");
		firstsearched = HxSTD_StrStr(pucDrmMsg, firstElement);
		lastsearched = HxSTD_StrStr(pucDrmMsg, lastElement);
		if(firstsearched == NULL || lastsearched == NULL)
		{
			if(pucURL != NULL)
			{
				HLIB_STD_MemFree(pucURL);
			}
			*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT ;
			return ERR_FAIL;
		}
		length = HxSTD_StrLen(firstsearched) - HxSTD_StrLen(lastsearched) - HxSTD_StrLen(firstElement);

		if(length>0)
		{
			pucVendorName = (HUINT8 *)HLIB_STD_MemAlloc(length+1);
			HxSTD_MemSet(pucVendorName, 0, length+1);
			HxSTD_memcpy(pucVendorName, firstsearched+HxSTD_StrLen(firstElement), length);
		}
		else
		{
			if(pucURL != NULL)
			{
				HLIB_STD_MemFree(pucURL);
			}
			*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT ;
			return ERR_FAIL;
		}

#if defined(CONFIG_MW_MM_PVR)
		hErr = om_media_play_mediapb_SetVmxCas(pstContext->ulActionId, pucURL, pucVendorName);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("om_media_play_mediapb_SetVmxCas ERROR!\n");
			*etResultCode = eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR ;
			hErr = ERR_FAIL;
		}
		else
		{
			*etResultCode = eMGR_DRMAGENT_RESULTCODE_SUCCESSFUL ;
			hErr = ERR_OK;
		}
#endif
		if(pucURL != NULL)
		{
			HLIB_STD_MemFree(pucURL);
		}
		if(pucVendorName != NULL)
		{
			HLIB_STD_MemFree(pucVendorName);
		}
	}
	else if(HxSTD_StrStr(pucDrmMsg, "GetVmxIdentifier") != NULL)
	{

	}
	else if(HxSTD_StrStr(pucDrmMsg, "DrmSystemStatus") != NULL)
	{

	}
	else if(HxSTD_StrStr(pucDrmMsg, "DrmRightsError") != NULL)
	{
		snprintf(pucResultElement, DRMAGENT_DRMMESSAGE_MAX_LEN, "{\"systemid\":\"urn:dvb:casystemid:22017\", \"status\":%d, \"errcode\":%d}",
				pstContext->stCache.eViewState, pstContext->stCache.nVmxErrorCode);
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_SUCCESSFUL;
		hErr = ERR_OK;
	}
	else
	{
		HxLOG_Error("Not yet supported \n");
		//In case of need of pucResultElement size calcuration, size compair should be needed.
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT ;
		hErr = ERR_FAIL;
	}
	return hErr;
}

STATIC HERROR om_media_play_MakePRDYDrmResultMsg(HUINT8 **pBuffer, HINT32 sizeofbuffer, HUINT8 *pDrmResultElement)
{
	HERROR			eResult = ERR_OK;
	HINT32			buffersize = sizeofbuffer;
	//strcat of xmldeclaration.
	HxSTD_StrCat(*pBuffer, s_XmlDeclaration); // XmlDeclaration must not over DRMAGENT_DRMMESSAGE_MAX_LEN.

	eResult = om_media_play_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_PLAYREADYRESPONSE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_XMLNS, NULL);
	if(eResult != ERR_OK)
		goto end_makeresult;
	eResult = om_media_play_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_DRM_RESULT_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "0");
	if(eResult != ERR_OK)
		goto end_makeresult;
	eResult = om_media_play_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_DRM_RESULT_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
	if(eResult != ERR_OK)
		goto end_makeresult;

	if(pDrmResultElement != NULL && HxSTD_StrLen(pDrmResultElement))
	{
		HxSTD_StrCat(*pBuffer, pDrmResultElement) ;
		buffersize += HxSTD_StrLen(pDrmResultElement) ;
	}

	eResult = om_media_play_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_PLAYREADYRESPONSE_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
	if(eResult != ERR_OK)
		goto end_makeresult;

end_makeresult:
	return eResult;
}

STATIC HERROR om_media_play_MakeVRWebDrmResultMsg(HUINT8 **pBuffer, HINT32 sizeofbuffer, HUINT8 *pDrmResultElement)
{
	HERROR			eResult = ERR_OK;
	HINT32			buffersize = sizeofbuffer;
	//strcat of xmldeclaration.
	HxSTD_StrCat(*pBuffer, s_XmlDeclaration); // XmlDeclaration must not over DRMAGENT_DRMMESSAGE_MAX_LEN.

	eResult = om_media_play_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_VIEWRIGHTWEBRESPONSE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
	if(eResult != ERR_OK)
		goto end_makeresult;
	eResult = om_media_play_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_DRM_RESULT_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "0");
	if(eResult != ERR_OK)
		goto end_makeresult;
	eResult = om_media_play_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_DRM_RESULT_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
	if(eResult != ERR_OK)
		goto end_makeresult;

	if(pDrmResultElement != NULL && HxSTD_StrLen(pDrmResultElement))
	{
		HxSTD_StrCat(*pBuffer, pDrmResultElement) ;
		buffersize += HxSTD_StrLen(pDrmResultElement) ;
	}

	eResult = om_media_play_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_VIEWRIGHTWEBRESPONSE_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
	if(eResult != ERR_OK)
		goto end_makeresult;

end_makeresult:
	return eResult;
}

STATIC BUS_Result_t om_media_play_SetSendDRMMessage (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrDrmAgentSendDrmMessage_t stDrmMessage;
	HUINT16 usMsgId = 0;
	e_MgrDrmAgent_ResultCodeType_t etResultCode = eMGR_DRMAGENT_RESULTCODE_SUCCESSFUL;
	e_MgrDrmAgent_SendMsgType_t etMsgType = eMGR_DRMAGENT_MSGTYPE_MAX;
	HUINT8 			*szResultBuffer=NULL;
	HUINT8 			*szResultElement=NULL;
	HERROR			eResult = ERR_OK;
	//HERROR hErr ;

	HxLOG_Trace();

	if((MgrDrmAgentSendDrmMessage_t*)p3 == NULL)
	{
		HxLOG_Error("SendDRMMessage is NULL \n");
		return MESSAGE_BREAK;
	}

	usMsgId = (HUINT16)p1;
	HxSTD_memcpy((void *)&stDrmMessage, (void *)p3, sizeof(MgrDrmAgentSendDrmMessage_t));
	szResultBuffer = HLIB_STD_MemAlloc(DRMAGENT_DRMMESSAGE_MAX_LEN);
	if(szResultBuffer == NULL)
	{
		HxLOG_Error("Out of Memory \n");
		return MESSAGE_BREAK;
	}
	szResultElement = HLIB_STD_MemAlloc(DRMAGENT_DRMMESSAGE_MAX_LEN);
	if(szResultElement == NULL)
	{
		HxLOG_Error("Out of Memory \n");
		HLIB_STD_MemFree(szResultBuffer);
		szResultBuffer = NULL;
		return MESSAGE_BREAK;
	}

	HxSTD_MemSet(szResultBuffer, 0, DRMAGENT_DRMMESSAGE_MAX_LEN);
	HxSTD_MemSet(szResultElement, 0, DRMAGENT_DRMMESSAGE_MAX_LEN);

	(void)om_media_play_ParseDrmMessageType(stDrmMessage.szDrmMsgType, &etMsgType);
	if( etMsgType == eMGR_DRMAGENT_MSGTYPE_PRDY)
	{
		(void)om_media_play_ParseAndProcessPRDYDrmMessage(pstContext, stDrmMessage.szDrmMsg, &etResultCode, szResultElement, DRMAGENT_DRMMESSAGE_MAX_LEN);
		eResult = om_media_play_MakePRDYDrmResultMsg(&szResultBuffer, DRMAGENT_DRMMESSAGE_MAX_LEN, szResultElement);
		if(eResult != ERR_OK || szResultBuffer == NULL)
		{
			goto end_msgprocess;
		}
	}
	else if(etMsgType == eMGR_DRMAGENT_MSGTYPE_VIEWRIGHT_WEB)
	{
		(void)om_media_play_ParseAndProcessVRWebDrmMessage(pstContext, stDrmMessage.szDrmMsg, &etResultCode, szResultElement, DRMAGENT_DRMMESSAGE_MAX_LEN);
		eResult = om_media_play_MakeVRWebDrmResultMsg(&szResultBuffer, DRMAGENT_DRMMESSAGE_MAX_LEN, szResultElement);
		if(eResult != ERR_OK || szResultBuffer == NULL)
		{
			goto end_msgprocess;
		}
	}
	else
	{
		etResultCode = eMGR_DRMAGENT_RESULTCODE_UNKNOWN_MIME_TYPE;
		eResult = om_media_play_MakePRDYDrmResultMsg(&szResultBuffer, DRMAGENT_DRMMESSAGE_MAX_LEN, szResultElement);
		if(eResult != ERR_OK || szResultBuffer == NULL)
		{
			goto end_msgprocess;
		}
	}

	(void)BUS_PostData(NULL, eMEVT_DRMAGENT_COMMON_RESPONSEDRMMESSAGERESULT, 0, usMsgId, etResultCode, (void*)szResultBuffer , HxSTD_StrLen(szResultBuffer)+1);

end_msgprocess:
	if(szResultBuffer != NULL)
		HLIB_STD_MemFree(szResultBuffer);
	if(szResultElement != NULL)
		HLIB_STD_MemFree(szResultElement);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_SetCopyrightControl (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr = ERR_OK;
	MgrCopyrightControl_OutputControlInfo_t stCopyControl;
	HxLOG_Trace();

	if((MgrCopyrightControl_OutputControlInfo_t*)p3 == NULL)
	{
		HxLOG_Error("Output control info is NULL \n");
		return MESSAGE_BREAK;
	}

	HxSTD_MemSet(&stCopyControl, 0x00, sizeof(MgrCopyrightControl_OutputControlInfo_t));
	HxSTD_MemCopy(&stCopyControl, (MgrCopyrightControl_OutputControlInfo_t*)p3, sizeof(MgrCopyrightControl_OutputControlInfo_t));

	hErr = MGR_OUTPUT_SetCopyProtect(pstContext->ulActionId, stCopyControl);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("MGR_OUTPUT_SetCopyProtect Fail \n");
		return MESSAGE_BREAK;
	}

	return MESSAGE_PASS;
}

#if defined(CONFIG_MW_SI_AIT)
STATIC BUS_Result_t om_media_play_MsgHbbtvNotifyAit (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR					 hErr;

	hErr = om_media_play_AitChanged(pstContext, hAction, (MgrHbbtv_AitState_e)p1, (Handle_t)p2, (HUINT16)p3);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_media_play_live_CbOnSiAITChanged err: State(%d), hHbbtvAction(0x%08x)\n", p1, p2);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

#endif

#define _____RPC_INITIALIZATION_____
STATIC HERROR om_media_play_InitRpcFunctions (void)
{
	HERROR hErr;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
	if (ERR_OK != hErr )
	{
		HxLOG_Error("Error. \n");
		return ERR_FAIL;
	}

	// APPKIT -> OBAMA
	HLIB_RPC_RegisterRequest (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_Activate, 			NULL, 	om_media_play_CbActivate			, NULL, "Activate the mediaplay module: (void)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetMainViewId, 		"i", 	om_media_play_CbSetMainViewId		, NULL, "SetMainViewId: (HUINT32 ulViewId)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_Start, 				"iib",	om_media_play_CbStart				, NULL, "Start the media: (HUINT32 ulSessionId, HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_Stop, 				"iii",	om_media_play_CbStop				, NULL, "Stop the media: (HUINT32 ulSessionId, HUINT32 ulViewId)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetComponentIndexEx,	"iiiis", om_media_play_CbSetComponentIndex	, NULL, "SetComponentIndex: (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, HUINT8 * pstrLang)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetComponentIndex,	"iiii", om_media_play_CbSetComponentIndex	, NULL, "SetComponentIndex: (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_UnsetComponentIndex, "iiii", om_media_play_CbUnsetComponentIndex   , NULL, "UnsetComponentIndex: (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPlaySpeed,			"iii",	om_media_play_CbSetPlaySpeed		, NULL, "SetPlaySpeed: (HUINT32 ulViewId, HINT32 nSpeed)");
#if defined(CONFIG_TIMESTAMP_64)
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPlayPosition,		"iil",	om_media_play_CbSetPlayPosition		, NULL, "SetPlayPosition: (HUINT32 ulViewId, HUINT64 ulPosition)");
#else
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPlayPosition,		"iii",	om_media_play_CbSetPlayPosition 	, NULL, "SetPlayPosition: (HUINT32 ulViewId, HUINT32 ulPosition)");
#endif
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPlaySegment,		"iii",	om_media_play_CbSetPlaySegment		, NULL, "SetPlaySegment: (HUINT32 ulViewId, HUINT32 ulSegment");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetSubtitleEnable,	"iii",	om_media_play_CbSetSubtitleEnable	, NULL, "SetSubtitleEnable: (HUINT32 ulViewId, HBOOL bEnable)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetVideoHideEnable,	"iii",	om_media_play_CbSetVideoHideEnable	, NULL, "SetVideoHideEnable: (HUINT32 ulViewId, HBOOL bEnable)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetVideoSize,			"iib",	om_media_play_CbSetVideoSize		, NULL, "SetVideoSize: (HUINT32 ulViewId, OxMediaPlay_VideoSize_t *pVideoSize");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_ResetVideoSize,		"ii",	om_media_play_CbResetVideoSize		, NULL, "ResetVideoSize: (HUINT32 ulViewId)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetPigRect,			"iib",	om_media_play_CbSetPigRect			, NULL, "SetPigRect: (HUINT32 ulViewId, OxMediaPlay_PigSize_t *pPigSize)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SaveThumbnail,		"iiiis",om_media_play_CbSaveThumbnail		, NULL, "SaveThumbnail: (HUINT32 ulViewId, HCHAR *pszFilename, HUINT32 ulWidth, HUINT32 ulHeight)");
	HLIB_RPC_RegisterRequest (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_GetOsdDisplayPoint,	"iiiii",	om_media_play_CbGetOsdDisplayPoint	, NULL, "GetOsdDisplayPoint: (HUINT32 ulViewId)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_CaptureStillPicture,	"iiiiiib",	om_media_play_CbCaptureStillPicture	, 	NULL, "CaptureStillPicture: (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT32 ulCaptureWidth, HUINT32 ulCaptureHeight, DxPixelFormat_e ePixelFormat, HUINT32 ulBufferSize, HUINT8 *pBuffer)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_StartStillPicture,		"iiib",	om_media_play_CbStartStillPicture	, 	NULL, "StartStillPicture: (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT32 ulBufferSize, HUINT8 *pBuffer)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_StopStillPicture,		"i",	om_media_play_CbStopStillPicture	, 	NULL, "StopStillPicture: (HUINT32 ulViewId)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_StartAudioClip,	"iiiib",	om_media_play_CbStartAudioClip	, 	NULL, "StartAudioClip: (HUINT32 ulRequestId, HUINT8 *pucBuffer, HUINT32 ulBufferBytes, DxAudioCodec_e eCodec, HUINT32 ulLoopCnt)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_PauseAudioClip,		"i",	om_media_play_CbPauseAudioClip	,	NULL, "ResetVideoSize: (HUINT32 ulRequestId)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_ResumeAudioClip,		"i",	om_media_play_CbResumeAudioClip	,	NULL, "ResetVideoSize: (HUINT32 ulRequestId)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_StopAudioClip,		"i",	om_media_play_CbStopAudioClip	,	NULL, "ResetVideoSize: (HUINT32 ulRequestId)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetVideoFreeze,		"ii",	om_media_play_CbSetVideoFreeze	,	NULL, "SetVideoFreeze: (HUINT32 ulViewId, HBOOL bFreeze)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetAudioFreeze,		"ii",	om_media_play_CbSetAudioFreeze	,	NULL, "SetAudioFreeze: (HUINT32 ulViewId, HBOOL bFreeze)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetTempAvPid,			"iiiiiii",	om_media_play_CbSetTempAvPid	,	NULL, "SetTempAvPid: HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT16 usVideoPid, DxAudioCodec_e eAudioCodec, HUINT16 usAudioPid, HUINT16 usPcrPid)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetMhegStatus, 		"i",	om_media_play_CbSetMhegStatus				, NULL, "SetMainViewId: (HUINT32 ulViewId)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_SetMhegInterruptibleFlag,		"i",	om_media_play_CbSetMhegInterruptibleFlag		, NULL, "SetMhegInterruptibleFlag : (HBOOL bInterruptible)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_RequestPincode, 		"iis",	om_media_play_CbRequestPincode				, NULL, "Enter Pincode : (HUINT8 *msg)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_ResultPincode,		"i",	om_media_play_CbResultPincode				, NULL, "Result Pincode : (HBOOL permit)");
	HLIB_RPC_RegisterCall (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_StopHbbtvMonitoring,		"i",	om_media_play_CbStopHbbtvMonitoring				, NULL, "Stop monitoring HBBTV temporary (void)");
	HLIB_RPC_RegisterRequest (s_nRpcHandle, RPC_OAPI_MEDIA_PLAY_GetDefaultSupportedSpeed,	"i",	om_media_play_CbGetDefaultSupportedSpeed	, NULL, "om_media_play_CbGetDefaultSupportedSpeed: (HBOOL bScrambled)");



	// OBAMA -> APPKIT
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMainViewChanged		,"i", "SET.BASE MainViewChanged.		\n\t   - (HUINT32 ulMainView)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSREnable				,"ii", "SET.BASE TSREnable.				\n\t   - (HUINT32 ulViewId, HBOOL bEnable)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleEnable			,"ii", "SET.BASE SubtitleEnable.			\n\t   - (HUINT32 ulViewId, HBOOL bEnable)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVideoHideEnable		,"ii", "SET.BASE VideoHideEnable.		\n\t   - (HUINT32 ulViewId, HBOOL bEnable)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVideoSize				,"ib", "SET.BASE VideoSize.				\n\t   - (HUINT32 ulViewId, OxMediaPlay_VideoSize_t *pstVideoSize)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPigRect				,"ib", "SET.BASE PigRect.				\n\t   - (HUINT32 ulViewId, OxMediaPlay_PigSize_t *pstPigSize)\n");

	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVideoStreamInfo		,"iiib", "BASE VideoStreamInfo.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, , OxMediaPlay_VideoStreamInfo_t *pstOapiVideoStreamInfo)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioClipCompleted	,"iiii", "BASE AudioClipCompleted.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, , HUINT32 ulRequestId)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSRTimecodeChanged		,"iiiiii", "BASE TSRTimecode.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulStartRecTime, HUINT32 ulRecTime)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBof					,"iii", "BASE BOF.				\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnEof					,"iii", "BASE EOF.				\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnStarted				,"iiib", "BASE Started.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_StartInfo_t *punStart)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnStopped				,"iii", "BASE Stopped.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlaying				,"iii", "BASE Playing.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPaused					,"iii", "BASE Pause.				\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTricked				,"iii", "BASE Tricked.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayStateChanged		,"iiii", "BASE StateChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_PlayState_e ePlayState)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayErrorChanged		,"iiiii", "BASE ErrorChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_PlayError_e ePlayError, OxMediaPlay_ErrorReason_e eReason)\n"); /* Fixed wrong RPC format(iii -> iiii); redmine #79103,#78673 */
#if defined(CONFIG_TIMESTAMP_64)
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayTimeChanged		,"iiill", "BASE TimeChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT64 ullPlayTime, HUINT64 ullDurationTime)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayDurationChanged   ,"iiili", "BASE DuraionChanged.        \n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT64 ullDurationTime, HBOOL bInfinity)\n");
#else
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayTimeChanged		,"iiiii", "BASE TimeChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ullPlayTime, HUINT32 ullDurationTime)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayDurationChanged	,"iiiii", "BASE DuraionChanged. 	   \n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ullDurationTime, HBOOL bInfinity)\n");
#endif
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSpeedChanged			,"iiii", "BASE SpeedChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HINT32 nSpeed)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnRatingChanged 			,"iiiiii", "BASE RatingChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HINT32 nBlocked, HINT32 nRating, HINT32 nDrmSystemId)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVStarted 				,"iii", "BASE AvStarted.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVStopped 				,"iii", "BASE AvStopped.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVResume					,"iii", "BASE AvResume.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVUnderrun				,"iii", "BASE AvUnderrun.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAVMediaRectChanged		,"iii", "BASE MediaRChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSupportedSpeedChanged 	,"iiiib", "BASE SuSpeedChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulNumSpeed, HINT32 *pSpeeds)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiPMTChanged			,"iii", "BASE PMT.				\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiEITChanged			,"iii", "BASE EIT.				\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiSITChanged			,"iii", "BASE SIT.				\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiAITChanged			,"iiibb", "BASE AIT.				\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_AitState_e eAitState, OxMediaPlay_AitSectionData_t *pstAitSectionData, HUINT8 *pucAitRaw)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiRCTChanged			,"iiiib", "BASE RCT.				\n\t   - HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_RctState_e eRctState, void *pvRctSectionData\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiRCTRequestImageIcon	,"iiis", "BASE RCTRequestImageIcon.		\n\t   - HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT8*pszUrl\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAvailabilityStartTimeChanged ,"iiis", "BASE AvailabilityStartTimeChanged. \n\t  - HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HCHAR *pString\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentVideoChanged 	,"iiib", "BASE ComVideoChanged.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_VideoCompTable_t *pstVideoCompTable)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentAudioChanged 	,"iiib", "BASE ComAudioChanged.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_AudioCompTable_t *pstAudioCompTable)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentSubtitleChanged,"iiib", "BASE ComSubtitleChanged.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentDataChanged	,"iiib", "BASE ComDataChanged.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_DataCompTable_t *pstDataCompTable)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentSelected 		,"iiiiii", "BASE ComSelected.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_Component_e eCompType, HINT32 nSelectIndex, HINT32 nSelectSubIndex)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnComponentUnselected   ,"iiiiii", "BASE ComUnselected.     \n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_Component_e eCompType, HINT32 nUnselectIndex, HINT32 nUnselectSubIndex)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTTXUpdated				,"iii", "BASE TTXUpdated.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnDrmBlocked				,"iiib", "BASE DrmBlocked.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HINT32 nBlocked, HINT32 nRating, HINT32 nDrmSystemId)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPinCtrlChanged			,"iii", "BASE PinCtrlChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResourceTaken 			,"iii", "BASE ResTaken.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResourceLost			,"iii", "BASE ResLost.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnRadioBGChanged			,"iiii", "BASE RadioBGChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HBOOL bRadioBG)\n");
#if defined(CONFIG_OP_SES)
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnCcTrickRestrictModeChanged	,"iiiii", "BASE TrickRestrictMode(FromCopyrightControl)Changed. 	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, DxCopyrightControl_TrickRestrictMode_u uTrickRestrictMode), eDxCopyrightControl_SesStorageRight_e eStorageRight\n");
#else
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnCcTrickRestrictModeChanged 	,"iiii", "BASE TrickRestrictMode(FromCopyrightControl)Changed.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, DxCopyrightControl_TrickRestrictMode_u uTrickRestrictMode)\n");
#endif
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiNetworkChanged 	,"iiiiib", "BASE NetworkChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulStartTimeOfChange, HUINT32 ulASODuration,OxMediaPlay_NetworkMsg_t *pstNetworkMsg)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSiNetworkUriChanged 	,"iiib", "BASE NetworkChanged.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, DxNcdNetworkInfo_t *pstNetworkMsg)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnHbbtvOnHdplusRedbutton 	,"iiii", "BASE HbbtvOnHdplusRedbutton.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulHdplusRedbutton)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnRequestPincode 			,"iis", "BASE pincode show.		\n\t   - (HUINT32 ulViewId, OxMediaPlay_MediaType_e eMediaType, HUINT8* pszMSg)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResultPincode 			,"i", "BASE pincode result.		\n\t   - (HBOOL permit)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSRPaused			,"iiiib", "BASE TSR Paused.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType,OxMediaPlay_TsrPaused_e eReason,OxMediaPlay_Bookmark_t *pstBookmark)\n");
	HLIB_RPC_CreateNotification (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnTSRResumed		,"iiii", "BASE TSR Resumed.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType,OxMediaPlay_TsrPaused_e eReason)\n");

	/*sub*/
	om_media_play_live_InitRpcFunctions (s_nRpcHandle);
#if defined(CONFIG_MW_MM_MEDIA)
	om_media_play_mediapb_InitRpcFunctions (s_nRpcHandle);
#endif
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
	om_media_play_mheg_InitRpcFunctions(s_nRpcHandle);
#endif

	return ERR_OK;
}

#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_media_play_Proto (omMediaPlay_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = ERR_BUS_NO_ERROR;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		eResult = om_media_play_MsgBusCreate (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		eResult = om_media_play_MsgBusDestroy (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_ACTION_SETUP_SUCCEEDED:
		eResult = om_media_play_MsgMgrActionTaken (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_RELEASED:
		eResult = om_media_play_MsgMgrActionRelease (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_PRIORITY_CHANGED:
		eResult = om_media_play_MsgMgrActionPriorityChanged(pstContext, hHandle, p1, p2, p3);
		break;

	/*FOR TSR */
	case eMEVT_PVR_NOTIFY_RECORD_STARTED:
		eResult = om_media_play_MsgMgrNotifyRecordStarted (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_PVR_NOTIFY_RECORD_STOPPED:
		eResult = om_media_play_MsgMgrNotifyRecordStopped (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_REC_TIMECODE_CHANGE:
		eResult = om_media_play_EvtRecTimecodeChanged(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_PVR_RECORD_TSR_PAUSED:
		eResult = om_media_play_MsgTsrPaused (pstContext, hHandle, p1, p2, p3);
		break;
	case eMEVT_PVR_RECORD_TSR_RESUMED:
		eResult = om_media_play_MsgTsrResumed (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_MEDIA_SEND_DRM_INFORMATION:
		eResult = om_media_play_SetDrmInformation (pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_MEDIA_UPDATE_COPYCONTROL_INFO:
		eResult = om_media_play_SetCopyrightControl (pstContext, hHandle, p1, p2, p3);
		break;

	case eOEVT_DRMAGENT_SENDDRMMESSAGE_PLAYREADY_CUSTOMDATA:
	case eOEVT_DRMAGENT_SENDDRMMESSAGE_PLAYREADY_LAURL:
	case eOEVT_DRMAGENT_SENDDRMMESSAGE_VIEWRIGHT_WEB:
		eResult = om_media_play_SetSendDRMMessage (pstContext, hHandle, p1, p2, p3);
		break;

	case eSEVT_DB_PARAM_UPDATED:
		if (hHandle == HANDLE_NULL)
		{
			// menu에서 subtitle language 등의 설정을 변경하면 이 msg가 날아오는데 action handle이 HANDLE_NULL이라 처리가 안된다
			// 이 msg는 main view를 위한 것으로 간주하여 여기서 main action handle로 값을 넣어주기로 한다
			hHandle = SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
		}
		break;

#if defined(CONFIG_MW_SI_AIT)
	case eMEVT_HBBTV_NOTIFY_AIT:
		return om_media_play_MsgHbbtvNotifyAit(pstContext, hHandle, p1, p2, p3);
		break;
#endif
	default:
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
		if ((message > eMEVT_3RD_MHEG_HEAD) && (message < eMEVT_3RD_MHEG_TAIL))
		{
			return proc_om_media_play_mheg_Proto(pstContext, message, hHandle, p1, p2, p3);
		}
#endif

		eResult = MESSAGE_PASS;
		break;
	}

	if ( (pstContext != NULL) && (MESSAGE_BREAK != eResult) )
	{
		if (SVC_PIPE_GetActionId (hHandle) == pstContext->ulActionId)
		{
			switch (pstContext->eMediaType)
			{
			case eOxMP_MEDIATYPE_LIVE:
				eResult = proc_om_media_play_live_Proto (pstContext, message, hHandle, p1, p2, p3);
				break;

#if defined(CONFIG_MW_MM_PVR)
			case eOxMP_MEDIATYPE_PVRPLAYBACK:
				eResult = proc_om_media_play_pvrpb_Proto (pstContext, message, hHandle, p1, p2, p3);
				break;

			case eOxMP_MEDIATYPE_TSRPLAYBACK:
				eResult = proc_om_media_play_tsrpb_Proto (pstContext, message, hHandle, p1, p2, p3);
				break;
#endif

#if defined(CONFIG_MW_MM_MEDIA)
			case eOxMP_MEDIATYPE_MEDIAPLAY:
				eResult = proc_om_media_play_mediapb_Proto (pstContext, message, hHandle, p1, p2, p3);
				break;
#endif

			default:
				break;
			}
		}
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

STATIC BUS_Result_t proc_om_media_play_View0 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return proc_om_media_play_Proto (om_media_play_GetContext (eActionId_VIEW_FIRST + 0), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t proc_om_media_play_View1 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return proc_om_media_play_Proto (om_media_play_GetContext (eActionId_VIEW_FIRST + 1), message, hHandle, p1, p2, p3);
}

#if defined(CONFIG_SUPPORT_FCC)
STATIC BUS_Result_t proc_om_media_play_View2 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return proc_om_media_play_Proto (om_media_play_GetContext (eActionId_VIEW_FIRST + 2), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t proc_om_media_play_View3 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return proc_om_media_play_Proto (om_media_play_GetContext (eActionId_VIEW_FIRST + 3), message, hHandle, p1, p2, p3);
}
#endif

#define ___MEMBER_FUNCTIONS___
void om_media_play_CleanCache(omMediaPlay_Context_t *pstContext, OxMediaPlay_MediaType_e eMediaType)
{
	if( NULL == pstContext )
		return ;

	switch(eMediaType)
	{
		case eOxMP_MEDIATYPE_LIVE:
		{
			HxSTD_MemSet(&(pstContext->stCache.stLive), 0, sizeof(OxMediaPlay_StartLive_t) );
			pstContext->stCache.stLive.ulMasterSvcUid	=	(HUINT32)-1;
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
#if defined(CONFIG_MW_CAS_NAGRA_PRM) && defined(CONFIG_MW_CAS_NAGRA_PRM_2_6)
	// SES Test Suite로 PRM VOD TEST시 여기에서 DRM Clear되는 문제가 발생하여 encryption key가 imma로 전상적으로 전달이 안되는 문제 발생함.
	// 그래서 drm clear는 prm xmgr_cas_NadvlStartPlayback(), xmgr_cas_NadvlStopPlayback() 호출전에 clear하도록 한다.
	// - base와 협의후 방안 모색 -
	HxSTD_MemSet(&(pstContext->stOmCache.stCcInfo),		0, sizeof(DxCopyrightControl_t) );
#else
	HxSTD_MemSet(&(pstContext->stOmCache),		0, sizeof(OmMediaPlay_Cache_t) );
#endif

	return ;
}

omMediaPlay_Context_t *om_media_play_GetContext (HUINT32 ulActionId)
{
	if ((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST))
	{
		return &(s_astMgrMpContext[ulActionId - eActionId_VIEW_FIRST]);
	}

	HxLOG_Assert(0);

	return NULL;
}

#if defined(CONFIG_MW_SI_AIT)
HERROR om_media_play_AitChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, MgrHbbtv_AitState_e eAitState, Handle_t hHbbtvAction, HUINT16 usAitPid)
{
	HUINT32							 ulViewId = 0;
	HUINT32							 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32							 ulSessionId = pstContext->ulSessionId;
	HUINT32							 i, ulAitSecCount = 0, ulAitTotalLen = 0;
	HUINT8							 ucDummyRaw = 0;
	HUINT8							*pucAllAitRawSec = NULL;
	HUINT8							*pucMemBuffer = NULL;
	HUINT32							 aulSecLen[OxMP_AIT_SECTION_NUM_MAX];
	OxMediaPlay_AitSectionData_t	 stAitSectionData;
	HERROR							 hErr, hResult = ERR_FAIL;

	if (pstContext->ulActionId != ulActionId)
	{
		goto END_FUNC;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		goto END_FUNC;
	}

	HxSTD_MemSet(aulSecLen, 0x00, sizeof(aulSecLen));
	HxSTD_MemSet(&stAitSectionData, 0x00, sizeof(OxMediaPlay_AitSectionData_t));

	switch(eAitState)
	{
		case eMgrHbbtvAitState_NoAit:		stAitSectionData.eAitState = eAitState_None;		break;
		case eMgrHbbtvAitState_Timeout:		stAitSectionData.eAitState = eAitState_Timeout;		break;
		case eMgrHbbtvAitState_Exist:		stAitSectionData.eAitState = eAitState_Received;	break;

		default:
			HxLOG_Error("invalid eAitState(%d) \n", eAitState);
			goto END_FUNC;
	}

	pstContext->stCache.stAitSecData.usPrevAitPid = pstContext->stCache.stAitSecData.usCurrAitPid;
	pstContext->stCache.stAitSecData.usCurrAitPid = usAitPid;
	stAitSectionData.usPrevAitPid = pstContext->stCache.stAitSecData.usPrevAitPid;
	stAitSectionData.usCurrAitPid = pstContext->stCache.stAitSecData.usCurrAitPid;

	if (stAitSectionData.eAitState == eAitState_Received)
	{
		HUINT32		 ulCurPos 			= 0;
		HUINT32 	 ulHbbtvActId = SVC_PIPE_GetActionId(hHbbtvAction);
		HUINT8		*pucSiRawSection	=	NULL;
		HUINT8		*paucSitRawSec[OxMP_AIT_SECTION_NUM_MAX]	=	{NULL, };

		hErr = SVC_SI_GetAitRawSection(ulHbbtvActId, OxMP_AIT_SECTION_NUM_MAX, &paucSitRawSec[0], &ulAitSecCount);
		if ((ERR_OK == hErr) && (ulAitSecCount > 0))
		{
			// count total ait length
			for (i = 0; i < ulAitSecCount; i++)
			{
				pucSiRawSection = paucSitRawSec[i];
				aulSecLen[i] = (HUINT32)HxMACRO_Get12Bit(pucSiRawSection + 1) + 3;
				ulAitTotalLen += aulSecLen[i];
			}

			// make ait section data for OAPI
			pucMemBuffer = (HUINT8 *)OxMGR_Calloc(sizeof(HUINT8) * ulAitTotalLen);
			if (pucMemBuffer == NULL)
			{
				HxLOG_Error("OxMGR_Calloc Error: size(%d) \n", ulAitTotalLen);
				goto END_FUNC;
			}

			ulCurPos = 0;
			for (i = 0; i < ulAitSecCount; i++)
			{
				pucSiRawSection = paucSitRawSec[i];
				HxSTD_MemCopy(&pucMemBuffer[ulCurPos], pucSiRawSection, aulSecLen[i]);
				ulCurPos += aulSecLen[i];
			}

			stAitSectionData.ulAitSecCount 	= ulAitSecCount;
			stAitSectionData.ulAitTotaSeclLen 	= ulAitTotalLen;
			HxSTD_MemCopy(stAitSectionData.aulAitSecLen, aulSecLen, sizeof(stAitSectionData.aulAitSecLen));

			pucAllAitRawSec = pucMemBuffer;
		}
		else
		{
			HxLOG_Error("SVC_SI_GetAitRawSection err: ulHbbtvActId (%d)\n", ulHbbtvActId);
			goto END_FUNC;
		}
	}
	else
	{
		stAitSectionData.ulAitTotaSeclLen = 1;
		pucAllAitRawSec = &ucDummyRaw;
	}

	HxSTD_MemSet(&(pstContext->stCache.stAitSecData),	0, sizeof(OxMediaPlay_AitSectionData_t) );
	pstContext->stCache.stAitSecData.eAitState = stAitSectionData.eAitState;
	pstContext->stCache.stAitSecData.ulAitSecCount = stAitSectionData.ulAitSecCount;
	pstContext->stCache.stAitSecData.ulAitTotaSeclLen = stAitSectionData.ulAitTotaSeclLen;

	om_media_play_CbOnSiAITChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, &stAitSectionData, pucAllAitRawSec);

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pucMemBuffer)
	{
		OxMGR_Free(pucMemBuffer);
	}

	return hResult;
}
#endif

 #define ___GLOBAL_FUNCTIONS___
HERROR OM_MEDIA_PLAY_Init (void)
{
	{
		HINT32 i;
		HxSTD_MemSet (s_astMgrMpContext, 0, sizeof(omMediaPlay_Context_t) * NUM_VIEW_ACTION);

		for( i=0 ; i<NUM_VIEW_ACTION ; i++ )
		{
 			om_media_play_InitContext( &(s_astMgrMpContext[i]) );
		}
	}

	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_media_play_View0, SVC_PIPE_GetActionHandle(eActionId_VIEW_FIRST + 0), 0, 0, 0);
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_media_play_View1, SVC_PIPE_GetActionHandle(eActionId_VIEW_FIRST + 1), 0, 0, 0);
#if defined(CONFIG_SUPPORT_FCC)
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_media_play_View2, SVC_PIPE_GetActionHandle(eActionId_VIEW_FIRST + 2), 0, 0, 0);
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_media_play_View3, SVC_PIPE_GetActionHandle(eActionId_VIEW_FIRST + 3), 0, 0, 0);
#endif

	om_media_play_InitRpcFunctions();

	return ERR_OK;
}
