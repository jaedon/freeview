/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _om_media_play.h
	@brief

	Description:  									\n
	Module: TV MANAGER / OAPI					\n

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

#ifndef __INT_OM_MEDIA_PLAY_H__
#define __INT_OM_MEDIA_PLAY_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <oapi.h>
#include <mgr_media.h>
#include <mgr_hbbtv.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	/*
	 *	OAPI Manager <-> OAPI.so 간의 정보 Sync 가 필요없는 Cache Struct
	 */

	/* DRM Information CACHE for IMMA*/
	MgrMedia_DrmInformation_t                  stDrmInformation;

	DxCopyrightControl_t			     stCcInfo;

}OmMediaPlay_Cache_t;


typedef struct
{
	HUINT32						ulActionId;
	HUINT32						ulSessionId;
	HBOOL						bActionTaken;

	OxMediaPlay_MediaType_e		eMediaType;
	OxMediaPlay_MediaType_e		ePrevMediaType;

	OxMediaPlay_Cache_t			stCache;

	OmMediaPlay_Cache_t			stOmCache;

	void						*pExtension;
	HBOOL						bInitialized;

} omMediaPlay_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
//	om_media_play
void 					om_media_play_CleanCache(omMediaPlay_Context_t *pstContext, OxMediaPlay_MediaType_e eMediaType);
omMediaPlay_Context_t*	om_media_play_GetContext (HUINT32 ulActionId);
HUINT32 				om_media_play_GetViewId(HUINT32 ulActionId);
HUINT32 				om_media_play_GetActionId(HUINT32 ulViewId);
#if defined(CONFIG_MW_SI_AIT)
HERROR					om_media_play_AitChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, MgrHbbtv_AitState_e eAitState, Handle_t hHbbtvAction, HUINT16 usAitPid);
#endif

void om_media_play_CbOnBof (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnEof (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnStarted (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_StartInfo_t *punStart);
void om_media_play_CbOnStopped (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnPlaying (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnPaused (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnTricked (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnPlayStateChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_PlayState_e ePlayState);
void om_media_play_CbOnPlayErrorChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_PlayError_e ePlayError, OxMediaPlay_ErrorReason_e eReason);
#if defined(CONFIG_TIMESTAMP_64)
void om_media_play_CbOnPlayTimeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT64 ullPlayTime, HUINT64 ullDurationTime);
void om_media_play_CbOnPlayDurationChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT64 ullDurationTime, HBOOL bInfinity);
#else
void om_media_play_CbOnPlayTimeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulPlayTime, HUINT32 ulDurationTime);
void om_media_play_CbOnPlayDurationChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulDurationTime, HBOOL bInfinity);
#endif
void om_media_play_CbOnSpeedChanged	(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HINT32 nSpeed);
void om_media_play_CbOnRatingChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HINT32 nBlocked, HINT32 nRating, HINT32 nDrmSystemId);
void om_media_play_CbOnAVStarted (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnAVStopped (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnAVResume (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnAVUnderrun(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnVideoStreamInfo(HUINT32 ulViewId,HUINT32 ulSessionId,OxMediaPlay_MediaType_e eMediaType,OxMediaPlay_VideoStreamInfo_t * pstOapiVideoStreamInfo);
void om_media_play_CbOnAudioClipCompleted(HUINT32 ulViewId,HUINT32 ulSessionId,OxMediaPlay_MediaType_e eMediaType, HUINT32 ulRequestId);
void om_media_play_CbOnAVMediaRectChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnSupportedSpeedChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulNumSpeed, HINT32 *pSpeeds);
void om_media_play_CbOnSiPMTChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnSiEITChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnSiSITChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnSiAITChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_AitSectionData_t *pstAitSectionData, HUINT8 *pucAitRaw);
void om_media_play_CbOnSiRCTChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_RctState_e eRctState, void *pvRctSectionData, HUINT32 ulSize);
void om_media_play_CbOnSiRCTRequestImageIcon (HUINT32 ulViewId, HUINT32 ulSessionId,  OxMediaPlay_MediaType_e eMediaType, HUINT8 *pszUrl);
void om_media_play_CbOnAvailabilityStartTimeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HCHAR *pString);
void om_media_play_CbOnComponentVideoChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_VideoCompTable_t *pstVideoCompTable);
void om_media_play_CbOnComponentAudioChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_AudioCompTable_t *pstAudioCompTable);
void om_media_play_CbOnComponentSubtitleChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable);
void om_media_play_CbOnComponentDataChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_DataCompTable_t *pstDataCompTable);
void om_media_play_CbOnComponentSelected (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_Component_e eCompType, HINT32 nSelectIndex, HINT32 nSelectSubIndex);
void om_media_play_CbOnComponentUnselected (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_Component_e eCompType, HINT32 nUnselectIndex, HINT32 nUnselectSubIndex);
void om_media_play_CbOnTTXUpdated (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnDrmBlocked (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, OxMediaPlay_DrmRightsError_t* pstDrmRightsError);
void om_media_play_CbOnPinCtrlChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnResourceTaken (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnResourceLost (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbOnRadioBGChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HBOOL bRadioBG);
void om_media_play_CbOnSubtitleEnable (HUINT32 ulViewId, HBOOL bEnable);
void om_media_play_CbOnTSREnable (HUINT32 ulViewId, HBOOL bEnable);
void om_media_play_CbOnVideoHideEnable (HUINT32 ulViewId, HBOOL bEnable);
void om_media_play_CbOnVideoSize (HUINT32 ulViewId, OxMediaPlay_VideoSize_t *pstVideoSize);
void om_media_play_CbOnPigRect (HUINT32 ulViewId, OxMediaPlay_PigSize_t *pstPigSize);
#if defined(CONFIG_OP_SES)
void om_media_play_CbCcTrickRestrictModeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, DxCopyrightControl_TrickRestrictMode_u uTrickRestrictMode, eDxCopyrightControl_SesStorageRight_e eStorageRight);
#else
void om_media_play_CbCcTrickRestrictModeChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, DxCopyrightControl_TrickRestrictMode_u uTrickRestrictMode);
#endif
void om_media_play_CbOnSiNetworkChanged(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulStartTimeOfChange, HUINT32 ulASODuration,DxNcdNetworkInfo_t *pstNetworkMsg);
void om_media_play_CbOnSiNetworkUriChanged(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, DxNetworkUriInfo_t *pstNetworkUriMsg);
void om_media_play_CbOnSiMhegAvailable(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType);
void om_media_play_CbHbbtvOnHdplusRedbutton (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulHdplusRedbutton);

// oapi
HERROR om_media_play_mediapb_InitRpcFunctions (HINT32 nRpcHandle);
HERROR om_media_play_live_InitRpcFunctions (HINT32 nRpcHandle);

//	om_media_play_mediapb
BUS_Result_t proc_om_media_play_mediapb_Proto (omMediaPlay_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR om_media_play_mediapb_Init(void);
HERROR om_media_play_mediapb_InitAction(omMediaPlay_Context_t *pstContext);
HERROR om_media_play_mediapb_Probe (HUINT32 ulActionId, HCHAR *pszUri);
HERROR om_media_play_mediapb_Start (HUINT32 ulActionId, void *pstStart);
HERROR om_media_play_mediapb_Stop (HUINT32 ulActionId);
HERROR om_media_play_mediapb_SetComponentIndex (HUINT32 ulActionId, OxMediaPlay_Component_e eCompType, HINT32 CompIndex);
HERROR om_media_play_mediapb_UnsetComponentIndex (HUINT32 ulActionId, OxMediaPlay_Component_e eCompType, HINT32 CompIndex);
HERROR om_media_play_mediapb_SetVideoHide(HUINT32 ulActionId, HBOOL bHide);
HERROR om_media_play_mediapb_SetAudioHide(HUINT32 ulActionId, HBOOL bHide);
HERROR om_media_play_mediapb_SetPlaySpeed (HUINT32 ulActionId, HINT32 nSpeed);
#if defined(CONFIG_TIMESTAMP_64)
HERROR om_media_play_mediapb_SetPlaySegment (HUINT32 ulActionId, HINT32 ulSegment, HUINT64 *pulPosition);
HERROR om_media_play_mediapb_SetPlayPosition (HUINT32 ulActionId, HINT64 ulPosition);
#else
HERROR om_media_play_mediapb_SetPlaySegment (HUINT32 ulActionId, HINT32 ulSegment, HUINT32 *pulPosition);
HERROR om_media_play_mediapb_SetPlayPosition (HUINT32 ulActionId, HINT32 ulPosition);
#endif


HERROR om_media_play_mediapb_SetSubtitleEnable (HUINT32 ulActionId, HBOOL bEnable);
HERROR om_media_play_mediapb_SetVideoHideEnable (HUINT32 ulActionId, HBOOL bEnable);
HERROR om_media_play_mediapb_GetSupportedSpeeds (HUINT32 ulActionId, HUINT32 *pulNumSpeed, HINT32 *pSpeeds);
HERROR om_media_play_mediapb_SaveThumbnail (HUINT32 ulActionId, HCHAR * pszFilename, HUINT32 ulWidth, HUINT32 ulHeight);
HERROR om_media_play_mediapb_SetSubtitleSyncTime (HUINT32 ulActionId, HINT32 nValue);
HERROR om_media_play_mediapb_SetSubtitleFontSize (HUINT32 ulActionId, HINT32 nValue);
HERROR om_media_play_mediapb_SetSubtitleTextPosition (HUINT32 ulActionId, HINT32 nValue);
HERROR om_media_play_mediapb_SetBufferingScenario (HUINT32 ulActionId, HBOOL bAuto, HINT32 nThreshold);
HERROR om_media_play_mediapb_SetDrmLicenseData (HUINT32 ulActionId, HCHAR * pszData);
HERROR om_media_play_mediapb_SetBrowserUserAgentData (HUINT32 ulActionId, HCHAR * pszUAData);
HERROR om_media_play_mediapb_ResourceMap (void);
HERROR om_media_play_mediapb_SetDuration (HUINT32 ulActionId, HUINT64 ullDuration);
HERROR om_media_play_mediapb_SetPlayerType (HUINT32 ulActionId, OxMediaPlay_PlayerType_e eMediaPlayPlayerType);
HERROR om_media_play_mediapb_AudioFlush(HUINT32 ulActionId, HUINT32 unTryToChangeAudioTrack);
HERROR om_media_play_mediapb_AudioRestart(HUINT32 ulActionId);
HERROR om_media_play_mediapb_AudioCodecSwitch(HUINT32 ulActionId, HUINT32 unTryToChangeAudioTrack, HINT32 nAudioCodec);
HERROR om_media_play_mediapb_SetFade(HUINT32 ulActionId, HUINT32 unStartVolume, HUINT32 unTargetVolume, HINT32 nEase, HINT32 nEaseTimeElapsed, HINT32 nEaseTransitionFrames);
HERROR om_media_play_mediapb_SetVmxCas (HUINT32 ulActionId, HUINT8 *pCasUrl, HUINT8 *pVendorName);

//	base, live 에서 사용하는 message member 함수
BUS_Result_t om_media_play_live_UpdateConflictInfoChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

//	om_media_play_pvrpb
BUS_Result_t proc_om_media_play_pvrpb_Proto (omMediaPlay_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR om_media_play_pvrpb_Init (void);
HERROR om_media_play_pvrpb_Start (HUINT32 ulActionId, OxMediaPlay_StartPvrPb_t *pstStart);
HERROR om_media_play_pvrpb_Stop (HUINT32 ulActionId);
HERROR om_media_play_pvrpb_SetComponentIndex (HUINT32 ulActionId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, HUINT8 *pstrLang);
HERROR om_media_play_pvrpb_SetPlaySpeed (HUINT32 ulActionId, HINT32 nSpeed);
HERROR om_media_play_pvrpb_SetPlayPosition (HUINT32 ulActionId, HINT32 ulPosition);
HERROR om_media_play_pvrpb_SetSubtitleEnable (HUINT32 ulActionId, HBOOL bEnable);
HERROR om_media_play_pvrpb_SetVideoHideEnable (HUINT32 ulActionId, HBOOL bEnable);
HERROR om_media_play_pvrpb_GetSupportedSpeeds (HUINT32 ulActionId, HUINT32 *pulNumSpeed, HINT32 *pSpeeds);

//	om_media_play_live
BUS_Result_t proc_om_media_play_live_Proto (omMediaPlay_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR om_media_play_live_Init (void);
HERROR om_media_play_live_Start (HUINT32 ulActionId,
#if defined(CONFIG_SUPPORT_FCC)
											HUINT32  ulSessionId,
											HUINT32	*pulTakenActionId,
#endif
											OxMediaPlay_StartLive_t *pstStart);
HERROR om_media_play_live_Stop (HUINT32 ulActionId);
HERROR om_media_play_live_SetComponentIndex (HUINT32 ulActionId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, HUINT8 *pstrLang);
HERROR om_media_play_live_SetPlaySpeed (HUINT32 ulActionId, HINT32 nSpeed);
HERROR om_media_play_live_SetPlayPosition (HUINT32 ulActionId, HINT32 ulPosition);
HERROR om_media_play_live_SetSubtitleEnable (HUINT32 ulActionId, HBOOL bEnable);
HERROR om_media_play_live_SetVideoHideEnable (HUINT32 ulActionId, HBOOL bEnable);
#if defined(CONFIG_3RD_MHEG)
void om_media_play_live_SetMhegInterruptibleFlag (HBOOL bMhegInterruptibleFlag);
#endif

//	om_media_play_tsrpb
BUS_Result_t proc_om_media_play_tsrpb_Proto (omMediaPlay_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR om_media_play_tsrpb_Init (void);
HERROR om_media_play_tsrpb_Start (HUINT32 ulActionId, OxMediaPlay_StartTsrPb_t *pstStart);
HERROR om_media_play_tsrpb_Stop (HUINT32 ulActionId);
HERROR om_media_play_tsrpb_SetComponentIndex (HUINT32 ulActionId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, HUINT8 *pstrLang);
HERROR om_media_play_tsrpb_SetPlaySpeed (HUINT32 ulActionId, HINT32 nSpeed);
HERROR om_media_play_tsrpb_SetPlayPosition (HUINT32 ulActionId, HINT32 ulPosition);
HERROR om_media_play_tsrpb_SetSubtitleEnable (HUINT32 ulActionId,  HBOOL bEnable);
HERROR om_media_play_tsrpb_SetVideoHideEnable (HUINT32 ulActionId, HBOOL bEnable);
HERROR om_media_play_tsrpb_GetSupportedSpeeds (HUINT32 ulActionId, HUINT32 *pulNumSpeed, HINT32 *pSpeeds);

//	om_media_play_mheg
BUS_Result_t proc_om_media_play_mheg_Proto (omMediaPlay_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR om_media_play_mheg_InitRpcFunctions (HINT32 nRpcHandle);


#endif // __INT_OM_MEDIA_PLAY_H__
