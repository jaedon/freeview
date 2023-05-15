/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_media_play.h
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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

#ifndef ___APK_MEDIA_PLAY_H___
#define ___APK_MEDIA_PLAY_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <oapi_media_play.h>

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define _____BASE_____
HERROR APK_MEDIA_PLAY_Init(void);
HERROR APK_MEDIA_PLAY_GetViewNumber(HUINT32 *pulViewNum);
HERROR APK_MEDIA_PLAY_SetMainViewId(HUINT32 ulViewId);
HERROR APK_MEDIA_PLAY_GetMainViewId(HUINT32 *pulViewId);
HERROR APK_MEDIA_PLAY_GetPlayType(HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType);
HERROR APK_MEDIA_PLAY_GetNeedRadioBg(HUINT32 ulViewId, HBOOL *pbNeed);
HERROR APK_MEDIA_PLAY_RegisterNotifier(HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, OxMediaPlay_EventFilter_t pfEventFilter, void *pvUserData);
HERROR APK_MEDIA_PLAY_UnregisterNotifier(HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, void *pvUserData);
HERROR APK_MEDIA_PLAY_GetSessionId(HUINT32 ulViewId, HUINT32 *pulSessionId);
HERROR APK_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId);
HERROR APK_MEDIA_PLAY_GetStreamAspectRatio (HUINT32 ulViewId, DxAspectRatio_e *peAspectRatio);
HERROR APK_MEDIA_PLAY_GetStreamVideoSize(HUINT32 ulViewId, HUINT32 *ulHSize, HUINT32 *ulVSize);
HERROR APK_MEDIA_PLAY_GetOsdDisplayPoint (HUINT32 ulViewId, HINT32 nRefWidth, HINT32 nRefHeight,  HINT32 nVideoPointX, HINT32 nVideoPointY,HINT32 *pnScaledX, HINT32 *pnScaledY);

#define _____FOR_MHEG_____
HERROR APK_MEDIA_PLAY_CaptureStillPicture (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT32 ulCaptureWidth, HUINT32 ulCaptureHeight, DxPixelFormat_e ePixelFormat, HUINT32 ulBufferSize, HUINT8 *pBuffer);
HERROR APK_MEDIA_PLAY_StartStillPicture (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT32 ulBufferSize, HUINT8 *pBuffer);
HERROR APK_MEDIA_PLAY_StopStillPicture (HUINT32 ulViewId);
HERROR APK_MEDIA_PLAY_StartAudioClip (HUINT32 ulRequestId, HUINT8 *pucBuffer, HUINT32 ulBufferBytes, DxAudioCodec_e	eCodec, HUINT32 ulLoopCnt);
HERROR APK_MEDIA_PLAY_PauseAudioClip(HUINT32 ulRequestId);
HERROR APK_MEDIA_PLAY_ResumeAudioClip(HUINT32 ulRequestId);
HERROR APK_MEDIA_PLAY_StopAudioClip(HUINT32 ulRequestId);
HERROR APK_MEDIA_PLAY_SetVideoFreeze(HUINT32 ulViewId, HBOOL bFreeze);
HERROR APK_MEDIA_PLAY_SetAudioFreeze(HUINT32 ulViewId, HBOOL bFreeze);
HERROR APK_MEDIA_PLAY_SetMhegStatus (OxMediaPlay_MhegStatus_e eStatus);
HERROR APK_MEDIA_PLAY_RequestPincode (HUINT32 ulViewId, OxMediaPlay_MediaType_e eMediaType, HUINT8 *pszMsg);
HERROR APK_MEDIA_PLAY_ResultPincode (HBOOL bPermit);
HERROR APK_MEDIA_PLAY_StopHbbtvMonitoring (HUINT32 ulViewId);


#define _____TRACK_____
HERROR APK_MEDIA_PLAY_SetComponentIndexEx(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, HUINT8 *pstrLang);
HERROR APK_MEDIA_PLAY_SetComponentIndex(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex);
HERROR APK_MEDIA_PLAY_UnsetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex);
HERROR APK_MEDIA_PLAY_GetComponentNum(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HUINT32 *pulCompNum);
HERROR APK_MEDIA_PLAY_GetComponent(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, OxMediaPlay_Component_t *pstComponet);
HERROR APK_MEDIA_PLAY_GetComponentIndex(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 *pnCompIndex, HINT32 *pnCompSubIndex);

#define _____PLAYER_CONTROL_____
HERROR APK_MEDIA_PLAY_Start(HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart, HUINT32 *pulSessionId);
HERROR APK_MEDIA_PLAY_Stop(HUINT32 ulViewId);
HERROR APK_MEDIA_PLAY_StopWithParams(HUINT32 ulViewId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulSessionId);
HERROR APK_MEDIA_PLAY_SetPlaySpeed(HUINT32 ulViewId, HINT32 nSpeed);

#if defined(CONFIG_TIMESTAMP_64)
HERROR APK_MEDIA_PLAY_SetPlayPosition(HUINT32 ulViewId, HUINT64 ullPosition);
#else
HERROR APK_MEDIA_PLAY_SetPlayPosition(HUINT32 ulViewId, HUINT32 ulPosition);
#endif
HERROR APK_MEDIA_PLAY_SetPlaySegment (HUINT32 ulViewId, HUINT32 ulPosSeg);

#define _____PLAYER_GETTER_____
HERROR APK_MEDIA_PLAY_GetPlaySpeed(HUINT32 ulViewId, HINT32 *pnSpeed);

#if defined(CONFIG_TIMESTAMP_64)
HERROR APK_MEDIA_PLAY_GetPlayPosition(HUINT32 ulViewId, HUINT64 *pullPosition);
HERROR APK_MEDIA_PLAY_GetDurationTime(HUINT32 ulViewId, HUINT64 *pullDurationTime);
#else
HERROR APK_MEDIA_PLAY_GetPlayPosition(HUINT32 ulViewId, HUINT32 *pulPosition);
HERROR APK_MEDIA_PLAY_GetDurationTime(HUINT32 ulViewId, HUINT32 *pulDurationTime);
#endif

HERROR APK_MEDIA_PLAY_GetPlayState(HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState);
HERROR APK_MEDIA_PLAY_GetPlayError(HUINT32 ulViewId, OxMediaPlay_PlayError_e *peError, OxMediaPlay_ErrorReason_e *peReason);
HERROR APK_MEDIA_PLAY_GetBufferedTime(HUINT32 ulViewId, HUINT32 *pulBufferred);
HERROR APK_MEDIA_PLAY_GetBitrate(HUINT32 ulViewId, HUINT32 *pulBitrate);
HERROR APK_MEDIA_PLAY_GetTsrStartTime (HUINT32 ulViewId, HUINT32 *pulTsrStartTime);
HERROR APK_MEDIA_PLAY_GetTsrRecTime(HUINT32 ulViewId, HUINT32 *pulRecTime);
HERROR APK_MEDIA_PLAY_GetSupportedSpeeds(HUINT32 ulViewId, HUINT32 *pulNumSpeed, HUINT32 ulMaxNum, HINT32 *pnSpeeds);
HERROR APK_MEDIA_PLAY_GetTrickRestrictMode (HUINT32 ulViewId, HUINT32 *pulTrickRestrictMode);
HERROR APK_MEDIA_PLAY_GetVideoSize(HUINT32 ulViewId, HINT32 *pnStartX, HINT32 *pnStartY, HINT32 *pnWidth, HINT32 *pnHeight);
HERROR APK_MEDIA_PLAY_GetAvailabilityStartTime (HUINT32 ulViewId, HUINT32 pnSzBuffer, HUINT8 *pszTimeString);
HERROR APK_MEDIA_PLAY_GetTsrPausedPoints (HUINT32 ulViewId, HUINT32 *pulNum, OxMediaPlay_Bookmark_t **ppstBookmarks);
HERROR APK_MEDIA_PLAY_GetTsrPaused (HUINT32 ulViewId, HBOOL *pbIsPaused);
HERROR APK_MEDIA_PLAY_GetAvailabilityStartTime (HUINT32 ulViewId, HUINT32 pnSzBuffer, HUINT8 *pszTimeString);

#define _____VIEW_CONTROL_____
HERROR APK_MEDIA_PLAY_SetVideoSize(HUINT32 ulViewId, HINT32 nStartX, HINT32 nStartY, HINT32 nWidth, HINT32 nHeight);
HERROR APK_MEDIA_PLAY_ResizeVideoWindow(HUINT32 ulViewId, OxMediaPlay_VideoSize_t *pstVideoSize);
HERROR APK_MEDIA_PLAY_ResetVideoSize(HUINT32 ulViewId);
HERROR APK_MEDIA_PLAY_SetPigRect(HUINT32 ulViewId, HUINT32 ulZOrder, HINT32 nISX, HINT32 nISY, HINT32 nIW, HINT32 nIH, HINT32 nIRW, HINT32 nIRH, HINT32 nOSX, HINT32 nOSY, HINT32 nOW, HINT32 nOH, HINT32 nORW, HINT32 nORH, DxAspectRatio_e eOsdAspectRatio, DxDfcSelect_e eDfc, DxAspectRatio_e ePigAspectRatio);

#define _____FUNC_PROPERTY_____
HERROR APK_MEDIA_PLAY_SetTSREnable(HBOOL bEnable);
HERROR APK_MEDIA_PLAY_SetSubtitleEnable(HUINT32 ulViewId, HBOOL bEnable);
HERROR APK_MEDIA_PLAY_SetVideoHide(HUINT32 ulViewId, HBOOL bHide);
HERROR APK_MEDIA_PLAY_GetTSREnable(HBOOL *pbEnable);
HERROR APK_MEDIA_PLAY_GetSubtitleEnable(HUINT32 ulViewId, HBOOL *pbEnable);

#define _____ETC_____
HERROR APK_MEDIA_PLAY_GetEventInfo(HUINT32 ulViewId, DxEvent_t *pstEventInfo);
HERROR APK_MEDIA_PLAY_SaveThumbnail(HUINT32 ulViewId, HCHAR *pszFilename, HUINT32 ulWidth, HUINT32 ulHeight);

#define _____LIVE_ONLY_____
HERROR APK_MEDIA_PLAY_GetMajorChannel (HUINT32 ulViewId, HUINT32 * pulMajorCHNum);
HERROR APK_MEDIA_PLAY_CheckPlayChangable(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_StartInfo_t *punStart, HBOOL *pbChangable);
HERROR APK_MEDIA_PLAY_GetViewState(HUINT32 ulViewId, OxMediaPlay_LiveViewState_e *peViewState);
HERROR APK_MEDIA_PLAY_GetLockState(HUINT32 ulViewId, OxMediaPlay_LiveLockState_e *peLockState);
HERROR APK_MEDIA_PLAY_MemorizeMediaState(HUINT32 ulViewId, HBOOL bStopMedia);
HERROR APK_MEDIA_PLAY_RestoreMediaState(HUINT32 ulViewId);
HCHAR *APK_MEDIA_PLAY_GetThumbnail(HUINT32 ulViewId, HUINT32 ulPos, HUINT32 *pulCaptureTime);
HERROR APK_MEDIA_PLAY_CountAitSection (HUINT32 ulViewId, HUINT32 *pulAitNum);
HERROR APK_MEDIA_PLAY_GetAitPid (HUINT32 ulViewId, HUINT16 *pusPrevAitPid, HUINT16 *pulCurrAitPid);
HERROR APK_MEDIA_PLAY_GetAitSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucAitSec, HUINT32 *ulSecLen);
HERROR APK_MEDIA_PLAY_FreeAitSection (HUINT8 *pucAitSec);
HERROR APK_MEDIA_PLAY_GetRctInfoCount (HUINT32 ulViewId, HUINT32 *pulTotalLinkCount , HUINT32 *pulImgDataByteSize);
HERROR APK_MEDIA_PLAY_GetRctInfo(HUINT32 ulViewId, DxService_RctInfo_t *pstRetInfo);
HERROR APK_MEDIA_PLAY_FreeRctInfo (DxService_RctInfo_t *pstRetInfo);
HERROR APK_MEDIA_PLAY_CountMhegPatSection (HUINT32 ulViewId, HUINT32 *pulSecNum);
HERROR APK_MEDIA_PLAY_GetMhegPatSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucRawSec, HUINT32 *ulSecLen);
HERROR APK_MEDIA_PLAY_FreeMhegPatSection (HUINT8 *pucRawSec);
HERROR APK_MEDIA_PLAY_CountMhegPmtSection (HUINT32 ulViewId, HUINT32 *pulSecNum);
HERROR APK_MEDIA_PLAY_GetMhegPmtSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucRawSec, HUINT32 *ulSecLen);
HERROR APK_MEDIA_PLAY_FreeMhegPmtSection (HUINT8 *pucRawSec);
HERROR APK_MEDIA_PLAY_SetMhegInterruptibleFlag (HUINT32 ulViewId, HBOOL bMhegInterruptible);
HERROR APK_MEDIA_PLAY_SetTempAvPid (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT16 usVideoPid, DxAudioCodec_e eAudioCodec, HUINT16 usAudioPid, HUINT16 usPcrPid);
HERROR APK_MEDIA_PLAY_GetSuppleType(HUINT32 ulViewId, OxMediaPlay_SuppleSvcType_e * peSuppSvcType);
void APK_MEDIA_PLAY_SetAitListenerStatus(HUINT32 ulViewId,HBOOL bEnable);
HERROR APK_MEDIA_PLAY_CheckAitSection(HUINT32 ulViewId);


#define _____MEDIAPB_ONLY_____
HERROR APK_MEDIA_PLAY_GetURI(HUINT32 ulViewId, HCHAR *pszURI);
HERROR APK_MEDIA_PLAY_Probe(HUINT32 ulViewId, HCHAR *pszURI);
HERROR APK_MEDIA_PLAY_SetSubtitleSyncTime(HUINT32 ulViewId, HUINT32 nSubtitle_SyncTime);
HERROR APK_MEDIA_PLAY_SetSubtitleFontSize(HUINT32 ulViewId, HUINT32 nSubtitle_FontSize);
HERROR APK_MEDIA_PLAY_SetSubtitleTextPosition(HUINT32 ulViewId, HUINT32 nSubtitle_TextPosition);
HERROR APK_MEDIA_PLAY_GetSubtitleSyncTime(HUINT32 ulViewId, HUINT32 *pnSubtitle_SyncTime);
HERROR APK_MEDIA_PLAY_GetSubtitleFontSize(HUINT32 ulViewId, HUINT32 *pnSubtitle_FontSize);
HERROR APK_MEDIA_PLAY_GetSubtitleTextPosition(HUINT32 ulViewId, HUINT32 *pnSubtitle_TextPosition);
HERROR APK_MEDIA_PLAY_GetSubtitleStatus(HUINT32 ulViewId, OxMediaPlay_SubtitleStatus_t *pstStatus);
HERROR APK_MEDIA_PLAY_SeBufferingScenario(HUINT32 ulViewId, HBOOL bAuto, HINT32 nThreshold);
HERROR APK_MEDIA_PLAY_GetBufferingChunkInfo (HUINT32 ulViewId, HUINT32 *nChunks, HUINT32 **pulStartOffsets, HUINT32 **ppulEndOffsets);
HERROR APK_MEDIA_PLAY_GetSeekableRangeInfo (HUINT32 ulViewId, HUINT32 *nCHunks, HUINT64 **ppullSeekableStart, HUINT64 **ppullSeekableEnd);
HERROR APK_MEDIA_PLAY_GetBufferingRemainedTime (HUINT32 ulViewId, HUINT32 *pulBufferRemaindTime);
HERROR APK_MEDIA_PLAY_GetBufferingPercent (HUINT32 ulViewId, HUINT32 *pulBufferPercent);
HERROR APK_MEDIA_PLAY_GetBufferState (HUINT32 ulViewId, OxMediaPlay_BufferState_e *peBufferState);
HERROR APK_MEDIA_PLAY_SetDrmLicenseData (HUINT32 ulViewId, const HCHAR *pszData);
HERROR APK_MEDIA_PLAY_SetBrowserUserAgentData (HUINT32 ulViewId, HCHAR *pszUAData);
HERROR APK_MEDIA_PLAY_GetResourceMap (OxMediaPlay_ResourceMap_t *pstResourceMap);
HERROR APK_MEDIA_PLAY_GetPumpIndex (HUINT32 ulViewId, HUINT32 *pulPumpId);
HERROR APK_MEDIA_PLAY_SetDuration (HUINT32 ulViewId, HUINT64 ullDuration);
HERROR APK_MEDIA_PLAY_SetPlayerType (HUINT32 ulViewId, OxMediaPlay_PlayerType_e eMediaPlayPlayerType);
HERROR APK_MEDIA_PLAY_AudioFlush(HUINT32 ulViewId, HUINT32 unTryToChangeAudioTrack);
HERROR APK_MEDIA_PLAY_AudioRestart(HUINT32 ulViewId);
HERROR APK_MEDIA_PLAY_AudioCodecSwitch(HUINT32 ulViewId, HUINT32 unTryToChangeAudioTrack, HINT32 nAudioCodec);
HERROR APK_MEDIA_PLAY_SetFade(HUINT32 ulViewId, HUINT32 unStartVolume, HUINT32 unTargetVolume, HINT32 nEase, HINT32 nEaseTimeElapsed, HINT32 nEaseTransitionFrames);
HERROR APK_MEDIA_PLAY_GetAudioDecoderVolume(HUINT32 ulViewId, HUINT32 *punVolume);
HERROR APK_MEDIA_PLAY_SetPlayReadyInfo (HUINT32 ulViewId, HCHAR *pszLaUrl, HCHAR *pszCustomData, HBOOL bControlFlag);


#define _____DRM_VERIMATRIX_____
HERROR APK_MEDIA_PLAY_GetVmxIdentifier (HUINT8 **pIdentifier);
HERROR APK_MEDIA_PLAY_GetVmxStatus (HUINT32 ulViewId, HINT32 *eStatus);
HERROR APK_MEDIA_PLAY_GetVmxError (HUINT32 ulViewId, HINT32 *nErrorCode);
HERROR APK_MEDIA_PLAY_SetVmxCas (HUINT32 ulViewId, HCHAR *pszVCASUrl, HCHAR *pszVendorName);
HERROR APK_MEDIA_PLAY_UnsetVmxCas (HUINT32 ulViewId);

#define _____BACKGROUND_____
HERROR APK_MEDIA_PLAY_MhegBgStart (HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart, HUINT32 *pulSessionId);

#endif
/* end of file */
