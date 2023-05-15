/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_media_play.h
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

#ifndef __OAPI_MEDIA_PLAY_H__
#define __OAPI_MEDIA_PLAY_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>


#define __________DEFINE__________
#define OxMP_MEDIAPLAY_URL				2048
#define OxMP_SUPPORTING_SPEED_MAX		24
#define OxMP_AUDIO_LANG_NUM				4
#define OxMP_COMP_ROLE_NUM				256
#define OxMP_COMP_NAME_NUM				256
#define OxMP_ES_STRING_LEN				81

//TRACK
#define OxMP_VIDEO_ES_NUM_MAX			2
#define OxMP_AUDIO_ES_NUM_MAX			16
#define OxMP_SUBTTL_ES_NUM_MAX			12
#define OxMP_DATA_ES_NUM_MAX			12	/* TR-B14 제3편 2.1.2.5 ES수의 제한 */

//Live Conflict
#define OxMP_CONFLICT_RFINFO_MAX		4
#define OxMP_CONFLICT_TUNEINFO_MAX		8
#define OxMP_UNIVERSAL_LNB_FREQ			0xFFFFFFFF

//Mediapb
#define OxMP_MEDIAPB_CHUNKS_MAX			20

//Live AIT
#define OxMP_AIT_SECTION_NUM_MAX		8

//Live PAT, PMT
#define OxMP_PAT_SECTION_NUM_MAX		4
#define OxMP_PMT_SECTION_NUM_MAX		4

#define OxMP_PVR_BOOKMARK_NAME_LEN		32
#define	OxMP_PVR_BOOKMARK_MAX			4

#define __________RPC__________
// APPKIT -> OBAMA
#define RPC_OAPI_MEDIA_PLAY_Activate						"oapi.media.play.Activate"
#define RPC_OAPI_MEDIA_PLAY_SetMainViewId					"oapi.media.play.SetMainViewId"
#define RPC_OAPI_MEDIA_PLAY_Probe							"oapi.media.play.Probe"
#define RPC_OAPI_MEDIA_PLAY_Start							"oapi.media.play.Start"
#define RPC_OAPI_MEDIA_PLAY_Stop							"oapi.media.play.Stop"
#define RPC_OAPI_MEDIA_PLAY_SetComponentIndexEx				"oapi.media.play.SetComponentIndexEx"
#define RPC_OAPI_MEDIA_PLAY_SetComponentIndex				"oapi.media.play.SetComponentIndex"
#define RPC_OAPI_MEDIA_PLAY_UnsetComponentIndex				"oapi.media.play.UnsetComponentIndex"
#define RPC_OAPI_MEDIA_PLAY_SetPlaySpeed					"oapi.media.play.SetPlaySpeed"
#define RPC_OAPI_MEDIA_PLAY_SetPlayPosition					"oapi.media.play.SetPlayPosition"
#define RPC_OAPI_MEDIA_PLAY_SetPlaySegment					"oapi.media.play.SetPlaySegment"
#define RPC_OAPI_MEDIA_PLAY_SetTSREnable					"oapi.media.play.SetTSREnable"
#define RPC_OAPI_MEDIA_PLAY_SetSubtitleEnable				"oapi.media.play.SetSubtitleEnable"
#define RPC_OAPI_MEDIA_PLAY_SetVideoHideEnable				"oapi.media.play.SetVideoHideEnable"
#define RPC_OAPI_MEDIA_PLAY_SetVideoSize					"oapi.media.play.SetVideoSize"
#define RPC_OAPI_MEDIA_PLAY_ResetVideoSize					"oapi.media.play.ResetVideoSize"
#define RPC_OAPI_MEDIA_PLAY_SetPigRect						"oapi.media.play.SetPigRect"
#define RPC_OAPI_MEDIA_PLAY_SaveThumbnail 					"oapi.media.play.SaveThumbnail"
#define RPC_OAPI_MEDIA_PLAY_SetSubtitleSyncTime 			"oapi.media.play.SetSubtitleSyncTime"
#define RPC_OAPI_MEDIA_PLAY_SetSubtitleFontSize 			"oapi.media.play.SetSubtitleFontSize"
#define RPC_OAPI_MEDIA_PLAY_SetSubtitleTextPosition 		"oapi.media.play.SetSubtitleTextPosition"
#define RPC_OAPI_MEDIA_PLAY_SeBufferingScenario 			"oapi.media.play.SeBufferingScenario"
#define RPC_OAPI_MEDIA_PLAY_SetDrmLicenseData 				"oapi.media.play.SetDrmLicenseData"
#define RPC_OAPI_MEDIA_PLAY_SetBrowserUserAgentData 		"oapi.media.play.SetBrowserUserAgentData"
#define RPC_OAPI_MEDIA_PLAY_ResourceMap						"oapi.media.play.ResourceMap"
#define RPC_OAPI_MEDIA_PLAY_SetDuration						"oapi.media.play.SetDuration"
#define RPC_OAPI_MEDIA_PLAY_SetPlayerType					"oapi.media.play.SetPlayerType"
#define RPC_OAPI_MEDIA_PLAY_AudioFlush						"oapi.media.play.AudioFlush"
#define RPC_OAPI_MEDIA_PLAY_AudioRestart					"oapi.media.play.AudioRestart"
#define RPC_OAPI_MEDIA_PLAY_AudioCodecSwitch				"oapi.media.play.AudioCodecSwitch"
#define RPC_OAPI_MEDIA_PLAY_SetFade							"oapi.media.play.SetFade"
#define RPC_OAPI_MEDIA_PLAY_SetPlayReadyInfo           		"oapi.media.play.SetPlayReadyInfo"
#define RPC_OAPI_MEDIA_PLAY_SetVmxCas               		"oapi.media.play.SetVmxCas"
#define RPC_OAPI_MEDIA_PLAY_UnsetVmxCas             		"oapi.media.play.UnsetVmxCas"
#define RPC_OAPI_MEDIA_PLAY_GetOsdDisplayPoint              "oapi.media.play.GetOsdDisplayPoint"
#define RPC_OAPI_MEDIA_PLAY_CaptureStillPicture				"oapi.media.play.CaptureStillPicture"
#define RPC_OAPI_MEDIA_PLAY_StartStillPicture				"oapi.media.play.StartStillPicture"
#define RPC_OAPI_MEDIA_PLAY_StopStillPicture				"oapi.media.play.StopStillPicture"
#define RPC_OAPI_MEDIA_PLAY_StartAudioClip					"oapi.media.play.StartAudioClip"
#define RPC_OAPI_MEDIA_PLAY_PauseAudioClip					"oapi.media.play.PauseAudioClip"
#define RPC_OAPI_MEDIA_PLAY_ResumeAudioClip					"oapi.media.play.ResumeAudioClip"
#define RPC_OAPI_MEDIA_PLAY_StopAudioClip					"oapi.media.play.StopAudioClip"
#define RPC_OAPI_MEDIA_PLAY_SetVideoFreeze					"oapi.media.play.SetVideoFreeze"
#define RPC_OAPI_MEDIA_PLAY_SetAudioFreeze					"oapi.media.play.SetAudioFreeze"
#define RPC_OAPI_MEDIA_PLAY_SetTempAvPid					"oapi.media.play.SetTempAvPid"
#define RPC_OAPI_MEDIA_PLAY_CheckAitSection					"oapi.media.play.CheckAitSection"
#define RPC_OAPI_MEDIA_PLAY_SetMhegStatus					"oapi.media.play.SetMhegStatus"
#define RPC_OAPI_MEDIA_PLAY_ResultPincode					"oapi.media.play.ResultPincode"
#define RPC_OAPI_MEDIA_PLAY_RequestPincode					"oapi.media.play.RequestPincode"
#define RPC_OAPI_MEDIA_PLAY_SetMhegInterruptibleFlag		"oapi.media.play.SetMhegInterruptibleFlag"
#define RPC_OAPI_MEDIA_PLAY_GetDefaultSupportedSpeed		"oapi.media.play.GetDefaultSupportedSpeed"
#define RPC_OAPI_MEDIA_PLAY_StopHbbtvMonitoring				"oapi.media.play.StopHbbtvMonitoring"

#define RPC_OAPI_MEDIA_PLAY_MhegBgStart							"oapi.media.play.MhegBgStart"

// OBAMA -> APPKIT (SETTER)
#define RPC_OBAMA_MEDIA_PLAY_OnMainViewChanged				"obama.media.play.OnMainViewId"
#define RPC_OBAMA_MEDIA_PLAY_OnTSREnable					"obama.media.play.OnTSREnable"
#define RPC_OBAMA_MEDIA_PLAY_OnSubtitleEnable				"obama.media.play.OnSubtitleEnable"
#define RPC_OBAMA_MEDIA_PLAY_OnVideoHideEnable				"obama.media.play.OnVideoHideEnable"
#define RPC_OBAMA_MEDIA_PLAY_OnVideoSize					"obama.media.play.OnVideoSize"
#define RPC_OBAMA_MEDIA_PLAY_OnPigRect						"obama.media.play.OnPigRect"
#define RPC_OBAMA_MEDIA_PLAY_OnSubtitleSyncTime 			"obama.media.play.OnSubtitleSyncTime"
#define RPC_OBAMA_MEDIA_PLAY_OnSubtitleFontSize 			"obama.media.play.OnSubtitleFontSize"
#define RPC_OBAMA_MEDIA_PLAY_OnSubtitleTextPosition 		"obama.media.play.OnSubtitleTextPosition"
#define RPC_OBAMA_MEDIA_PLAY_OnBufferingScenario 			"obama.media.play.OnBufferingScenario"
#define RPC_OBAMA_MEDIA_PLAY_OnDrmLicenseData 				"obama.media.play.OnDrmLicenseData"
#define RPC_OBAMA_MEDIA_PLAY_OnBrowserUserAgentData 		"obama.media.play.OnBrowserUserAgentData"
#define RPC_OBAMA_MEDIA_PLAY_OnDuration						"obama.media.play.OnDuration"
#define RPC_OBAMA_MEDIA_PLAY_OnPlayerType					"obama.media.play.OnPlayerType"


// OBAMA -> APPKIT (EVENT)
#define RPC_OBAMA_MEDIA_PLAY_OnLocked						"obama.media.play.OnLocked"
#define RPC_OBAMA_MEDIA_PLAY_OnNoSignal						"obama.media.play.OnNoSignal"
#define RPC_OBAMA_MEDIA_PLAY_OnAntennaOk					"obama.media.play.OnAntennaOk"
#define RPC_OBAMA_MEDIA_PLAY_OnAntennaNok					"obama.media.play.OnAntennaNok"
#define RPC_OBAMA_MEDIA_PLAY_OnSvcChanged					"obama.media.play.live.OnSvcChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSvcStopped					"obama.media.play.live.OnSvcStopped"
#define RPC_OBAMA_MEDIA_PLAY_OnConflictChanged				"obama.media.play.live.OnConflictChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnEventRelay					"obama.media.play.live.OnEventRelay"
#define RPC_OBAMA_MEDIA_PLAY_OnEws							"obama.media.play.live.OnEws"
#define RPC_OBAMA_MEDIA_PLAY_OnBsCtrlStarted				"obama.media.play.live.OnBsCtrlStarted"
#define RPC_OBAMA_MEDIA_PLAY_OnPinCtrlChangedByBCAS			"obama.media.play.live.OnPinCtrlChangedByBCAS"
#define RPC_OBAMA_MEDIA_PLAY_OnTSRTimecodeChanged			"obama.media.play.live.OnTSRTimecodeChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnViewStateChanged				"obama.media.play.live.OnViewStateChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnLockStateChanged				"obama.media.play.live.OnLockStateChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnProbeErrored					"obama.media.play.media.OnProbeErrored"
#define RPC_OBAMA_MEDIA_PLAY_OnStopErrored					"obama.media.play.media.OnStopErrored"
#define RPC_OBAMA_MEDIA_PLAY_OnBufferStateChanged			"obama.media.play.media.OnBufferStateChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnBufferInfoChanged			"obama.media.play.media.OnBufferInfoChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnPlayInfo						"obama.media.play.media.OnPlayInfo"
#define RPC_OBAMA_MEDIA_PLAY_OnPumpIndexChanged				"obama.media.play.media.OnPumpIndexChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSeekReady					"obama.media.play.media.OnSeekReady"
#define RPC_OBAMA_MEDIA_PLAY_OnAudioCodecSwitched			"obama.media.play.media.OnAudioCodecSwitched"
#define RPC_OBAMA_MEDIA_PLAY_OnAudioFlushCompleted			"obama.media.play.media.OnAudioFlushCompleted"
#define RPC_OBAMA_MEDIA_PLAY_OnAudioRestartCompleted		"obama.media.play.media.OnAudioRestartCompleted"
#define RPC_OBAMA_MEDIA_PLAY_OnAudioFadeCompleted			"obama.media.play.media.OnAudioFadeCompleted"
#define RPC_OBAMA_MEDIA_PLAY_OnAudioFadeFailed				"obama.media.play.media.OnAudioFadeFailed"
#define RPC_OBAMA_MEDIA_PLAY_OnAudioDecoderVolumeChanged	"obama.media.play.media.OnAudioDecoderVolumeChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSBTLUpdate					"obama.media.play.media.OnSBTLUpdate"
#define RPC_OBAMA_MEDIA_PLAY_OnResourceMapChanged			"obama.media.play.media.OnResourceMapChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnBof							"obama.media.play.OnBof"
#define RPC_OBAMA_MEDIA_PLAY_OnEof							"obama.media.play.OnEof"
#define RPC_OBAMA_MEDIA_PLAY_OnStarted						"obama.media.play.OnStarted"
#define RPC_OBAMA_MEDIA_PLAY_OnStopped						"obama.media.play.OnStopped"
#define RPC_OBAMA_MEDIA_PLAY_OnPlaying						"obama.media.play.OnPlaying"
#define RPC_OBAMA_MEDIA_PLAY_OnPaused						"obama.media.play.OnPaused"
#define RPC_OBAMA_MEDIA_PLAY_OnTricked						"obama.media.play.OnTricked"
#define RPC_OBAMA_MEDIA_PLAY_OnPlayStateChanged				"obama.media.play.OnPlayStateChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnPlayErrorChanged				"obama.media.play.OnPlayErrorChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnPlayTimeChanged				"obama.media.play.OnPlayTimeChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnPlayDurationChanged			"obama.media.play.OnPlayDurationChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSpeedChanged					"obama.media.play.OnSpeedChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnRatingChanged				"obama.media.play.OnRatingChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnAVStarted					"obama.media.play.OnAVStarted"
#define RPC_OBAMA_MEDIA_PLAY_OnAVStopped					"obama.media.play.OnAVStopped"
#define RPC_OBAMA_MEDIA_PLAY_OnAVResume						"obama.media.play.OnAVResume"
#define RPC_OBAMA_MEDIA_PLAY_OnAVUnderrun					"obama.media.play.OnAVUnderrun"
#define RPC_OBAMA_MEDIA_PLAY_OnAVMediaRectChanged			"obama.media.play.OnAVMediaRectChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSupportedSpeedChanged		"obama.media.play.OnSupportedSpeedChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSiPMTChanged					"obama.media.play.OnSiPMTChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSiEITChanged					"obama.media.play.OnSiEITChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSiSITChanged					"obama.media.play.OnSiSITChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSiAITChanged					"obama.media.play.OnSiAITChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSiRCTChanged					"obama.media.play.OnSiRCTChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSiRCTRequestImageIcon					"obama.media.OnSiRCTRequestImageIcon"
#define RPC_OBAMA_MEDIA_PLAY_OnAvailabilityStartTimeChanged	"obama.media.play.OnAvailabilityStartTimeChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnComponentVideoChanged		"obama.media.play.OnComponentVideoChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnComponentAudioChanged		"obama.media.play.OnComponentAudioChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnComponentSubtitleChanged		"obama.media.play.OnComponentSubtitleChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnComponentDataChanged			"obama.media.play.OnComponentDataChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnComponentSelected			"obama.media.play.OnComponentSelected"
#define RPC_OBAMA_MEDIA_PLAY_OnComponentUnselected			"obama.media.play.OnComponentUnselected"
#define RPC_OBAMA_MEDIA_PLAY_OnTTXUpdated					"obama.media.play.OnTTXUpdated"
#define RPC_OBAMA_MEDIA_PLAY_OnDrmBlocked					"obama.media.play.OnDrmBlocked"
#define RPC_OBAMA_MEDIA_PLAY_OnPinCtrlChanged				"obama.media.play.OnPinCtrlChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnResourceTaken				"obama.media.play.OnResourceTaken"
#define RPC_OBAMA_MEDIA_PLAY_OnResourceLost					"obama.media.play.OnResourceLost"
#define RPC_OBAMA_MEDIA_PLAY_OnRadioBGChanged				"obama.media.play.OnRadioBGChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnVmxDrmErrorChanged    		"obama.media.play.OnVmxDrmErrorChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnVmxDrmStatusChanged   		"obama.media.play.OnVmxDrmStatusChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnVideoStreamInfo				"obama.media.play.OnVideoStreamInfo"
#define RPC_OBAMA_MEDIA_PLAY_OnAudioClipCompleted			"obama.media.play.OnAudioClipCompleted"
#define RPC_OBAMA_MEDIA_PLAY_OnCcTrickRestrictModeChanged	"obama.media.play.OnCcTrickRestrictModeChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnAudioOutputRestrict		"obama.media.play.OnAudioOutputRestrict"
#define RPC_OBAMA_MEDIA_PLAY_OnSiNetworkChanged				"obama.media.play.OnSiNetworkChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnSiNetworkUriChanged			"obama.media.play.OnSiNetworkUriChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnAdvancedErrorChanged		"obama.media.play.OnAdvancedErrorChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnHbbtvOnHdplusRedbutton		"obama.media.play.OnHbbtvOnHdplusRedbutton"
#define RPC_OBAMA_MEDIA_PLAY_OnRequestPincode				"obama.media.play.OnRequestPincode"
#define RPC_OBAMA_MEDIA_PLAY_OnResultPincode				"obama.media.play.OnResultPincode"

#define RPC_OBAMA_MEDIA_PLAY_OnMhegMonStarted				"obama.media.play.OnMhegMonStarted"
#define RPC_OBAMA_MEDIA_PLAY_OnMhegMonStopped				"obama.media.play.OnMhegMonStopped"
#define RPC_OBAMA_MEDIA_PLAY_OnMhegDmxChanged				"obama.media.play.OnMhegDmxChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnMhegSvcStarted				"obama.media.play.OnMhegSvcStarted"
#define RPC_OBAMA_MEDIA_PLAY_OnMhegSvcStopped				"obama.media.play.OnMhegSvcStopped"
#define RPC_OBAMA_MEDIA_PLAY_OnMhegPmtChanged				"obama.media.play.OnMhegPmtChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnMhegRctChanged				"obama.media.play.OnMhegRctChanged"
#define RPC_OBAMA_MEDIA_PLAY_OnMhegNDTChanged				"obama.media.play.OnMhegNDTChanged"

#define RPC_OBAMA_MEDIA_PLAY_OnTSRPaused					"obama.media.play.OnTSRPaused"
#define RPC_OBAMA_MEDIA_PLAY_OnTSRResumed					"obama.media.play.OnTSRResumed"

#define __________EVENT__________
typedef enum
{
	eOxMP_EVENT_None,						//	None

	/* LIVE */
	eOxMP_EVENT_TUNE_Locked,				//	eSEVT_CH_LOCKED
	eOxMP_EVENT_TUNE_NoSignal,			//	eSEVT_CH_NO_SIGNAL
	eOxMP_EVENT_TUNE_AntennaOk,			//	eSEVT_CH_ANTENNA_OK
	eOxMP_EVENT_TUNE_AntennaNok,			//	eSEVT_CH_ANTENNA_FAULT
	eOxMP_EVENT_LIVE_SvcChanged,			//	eMEVT_LIVE_NOTIFY_SVC_CHANGED
	eOxMP_EVENT_LIVE_SvcStopped,			//	eMEVT_LIVE_NOTIFY_SVC_STOPPED
	eOxMP_EVENT_LIVE_ConflictChanged,		//
	eOxMP_EVENT_LIVE_EventRelay,			//	eMEVT_LIVE_NOTIFY_EVENT_RELAY
	eOxMP_EVENT_LIVE_Ews,					//	eMEVT_LIVE_NOTIFY_EVENT_EWS
	eOxMP_EVENT_LIVE_BsCtrlStarted, 		//	XMSG_CAS_BCAS_BSCONTROL_STARTED
	eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS,	//	XMSG_CAS_BCAS_RATING_CTRL_CHANGED
	eOxMP_EVENT_LIVE_TSRTimecodeChanged,	//	RED_TIMECODE
	eOxMP_EVENT_LIVE_ViewStateChanged,		//	eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED
	eOxMP_EVENT_LIVE_LockStateChanged,		//	eMEVT_LIVE_NOTIFY_LOCK_STATE_CHANGED

	/* MEDIA */
	eOxMP_EVENT_MEDIA_ProbeErrored, 		//	MW_MEDIA_EVENT_PROBEERROR
	eOxMP_EVENT_MEDIA_StopErrored,			//	MW_MEDIA_EVENT_STOPERROR
	eOxMP_EVENT_MEDIA_BufferStateChanged,	//	MW SvcMp_Event_e
	eOxMP_EVENT_MEDIA_BufferInfoChanged,	//	MW SvcMp_Event_e
	eOxMP_EVENT_MEDIA_PlayInfo, 			//	MW_MEDIA_EVENT_PLAYINFO
	eOxMP_EVENT_MEDIA_PumpIndexChanged,		//	ulPumpIndex
	eOxMP_EVENT_MEDIA_SeekReady,			//	Seek Ready
	eOxMP_EVENT_MEDIA_AudioCodecSwitched,	// 	Switch audio codec for Netflix
	eOxMP_EVENT_MEDIA_AudioFlushCompleted,	//  Audio flush complete event for Netflix
	eOxMP_EVENT_MEDIA_AudioRestartCompleted,// 	Audio restart complete event for Netflix
	eOxMP_EVENT_MEDIA_AudioFadeCompleted,	//	Audio Ease function for Netflix
	eOxMP_EVENT_MEDIA_AudioFadeFailed,		//	Audio Ease function for Netflix
	eOxMP_EVENT_MEDIA_AudioDecoderVolumeChanged,	//	Audio Ease function for Netflix
	eOxMP_EVENT_SBTL_UPDATE,				//	eSEVT_SBTL_START, eSEVT_SBTL_STOPPED, eSEVT_SBTL_PACKET_RECEIVED
	eOxMP_EVENT_RESOURCE_MAP,

	/* PLAYBACK */
	eOxMP_EVENT_BOF,						//	eSEVT_PB_BOF, MW_MEDIA_EVENT_BEGINOFSTREAM
	eOxMP_EVENT_EOF,						//	eSEVT_PB_EOF, MW_MEDIA_EVENT_ENDOFSTREAM
	eOxMP_EVENT_STARTED,					//	eMEVT_PVR_NOTIFY_PLAYBACK_STARTED, MW_MEDIA_EVENT_START
	eOxMP_EVENT_STOPPED,					//	eMEVT_PVR_NOTIFY_PLAYBACK_STOPPED, MW_MEDIA_EVENT_STOP
	eOxMP_EVENT_PLAYING,					//	MW_MEDIA_EVENT_PLAYING
	eOxMP_EVENT_PAUSE,						//	MW_MEDIA_EVENT_PAUSE
	eOxMP_EVENT_TRICK,						//	MW_MEDIA_EVENT_TRICK

	/* STATE */
	eOxMP_EVENT_PLAYERROR,					//	MW_MEDIA_EVENT_PLAYERROR, MW_MEDIA_EVENT_ERROROFSTREAM
	eOxMP_EVENT_PLAYTIME,					//	eSVC_MP_EVENT_PLAY_POSITION_CHANGED
	eOxMP_EVENT_PLAYDURATION,				//	MW_MEDIA_EVENT_PLAY_DURATION_CHANGED
	eOxMP_EVENT_PLAYSTATE,					//	MW SvcMp_Event_e

	/* CONTROL */
	eOxMP_EVENT_SPEED_CHANGED, 				//	eSVC_MP_EVENT_PLAY_SPEED_CHANGED
	eOxMP_EVENT_RATING_CHANGED, 			//	eMEVT_LIVE_NOTIFY_EVENT_RATING_CTRL_CHANGED, MSG_MEDIA_EVENT_RATING_CTRL_CHANGED
	eOxMP_EVENT_AV_STARTED, 				//	eMEVT_OUTPUT_AV_STARTED
	eOxMP_EVENT_AV_STOPPED, 				//	eMEVT_OUTPUT_AV_STOPPED
	eOxMP_EVENT_AV_RESUME, 					//	eMEVT_OUTPUT_AV_RESUME
	eOxMP_EVENT_AV_UNDERRUN, 				//	eMEVT_OUTPUT_AV_UNDERRUN
	eOxMP_EVENT_AV_MEDIARECT_CHANGED,		//	eSEVT_AV_MEDIA_RECT_CHANGED
	eOxMP_EVENT_SupportedSpeedChanged,		//
	eOxMP_EVENT_VIDEOSTREAMINFO,
	eOxMP_EVENT_AUDIOCLIPCOMPLETED,			//	eSEVT_AV_AUDIO_CLIP_COMPLETED
	eOxMP_EVENT_TSR_PAUSED,					//	eMEVT_PVR_RECORD_TSR_PAUSED
	eOxMP_EVENT_TSR_RESUMED,				//	eMEVT_PVR_RECORD_TSR_RESUMED

	eOxMP_EVENT_SI_PMT_CHANGED,				//	eSEVT_SI_PMT
	eOxMP_EVENT_SI_EIT_CHANGED,				//	eSEVT_SI_EIT
	eOxMP_EVENT_SI_SIT_CHANGED,				//	EVT_SI_SIT
	eOxMP_EVENT_SI_AIT_CHANGED,				//	eSEVT_SI_AIT, eSEVT_SI_NONE_AIT, eSEVT_SI_AIT_TIMEOUT
	eOxMP_EVENT_SI_RCT_CHANGED, 				//	eSEVT_SI_RCT, eSEVT_SI_RCT_NONE, eSEVT_SI_RCT_TIMEOUT
	eOxMP_EVENT_SI_RCT_REQUEST_IMAEG_ICON, 	//	eSEVT_SI_REQUEST_RCT_IMAGE_BY_MHEG
	eOxMP_EVENT_SI_NETWORK_CHANGED,			//	eSEVT_SI_NETWORK_CHANGED_NOTIFY
	eOxMP_EVENT_SI_NETWORK_URI_CHANGED,		//	eSEVT_SI_NETWORK_CHANGED_NOTIFY
	eOxMP_EVENT_COMPONENT_VIDEO_CHANGED,	//	eSEVT_SI_PMT, eSEVT_SI_EIT
	eOxMP_EVENT_COMPONENT_AUDIO_CHANGED, 	//	eSEVT_SI_PMT, eSEVT_SI_EIT
	eOxMP_EVENT_COMPONENT_SUBTITLE_CHANGED,	//	eSEVT_SI_PMT
	eOxMP_EVENT_COMPONENT_DATA_CHANGED, 	//	eSEVT_SI_PMT
	eOxMP_EVENT_COMPONENT_SELECTED,			//	Component Changed
	eOxMP_EVENT_COMPONENT_UNSELECTED,		//	Component Changed

	/* DRM */
	eOxMP_EVENT_DRM_BLOCK,					//	eMEVT_PVR_PLAYBACK_EVENT_DRM_BLOCK
	eOxMP_EVENT_PinCtrlChanged, 			//

	/* Verimatrix DRM */
	eOxMP_EVENT_VMXERROR,					//	Verimatrix ErrorCode
	eOxMP_EVENT_VMXSTATUS,				  //  Verimatrix Status
	eOxMP_EVENT_VMXIDENTIFIER,			  //  Verimatrix Identifier

	/* RESOURCE */
	eOxMP_EVENT_RESOURCE_TAKEN, 			//	MEDIA_CTRL_RES_RESOURCE_TAKEN
	eOxMP_EVENT_RESOURCE_LOST,				//	MEDIA_CTRL_RES_RESOURCE_LOST

	/* ENV */
	eOxMP_EVENT_RADIOBG_CHANGED, 			//	eMEVT_SVCLIST_RADIOBG_CHANGED
	eOxMP_EVENT_TTX_UPDATE, 				//	EVT_TTX_PAGE_UPDATED

	eOxMP_EVENT_TrickRestrictModeChanged,		//
	eOxMP_EVENT_AudioOutputRestrict,			// SampleRate of HD Audio is Restricted on HDMI Output

	eOxMP_EVENT_AdvancedErrorChanged,		// Advanced Error : Network, Server, System, I/O.

	/* Nagra PRM */
	eOxMP_EVENT_NAGRA_PRM_UPDATE,			//

	/*MHEG*/
	eOxMP_EVENT_RequestPincode,
	eOxMP_EVENT_ResultPincode,
	eOxMP_EVENT_MHEG_MON_STARTED,
	eOxMP_EVENT_MHEG_MON_STOPPED,
	eOxMP_EVENT_MHEG_DMX_CHANGED,
	eOxMP_EVENT_MHEG_PMT_RECEIVED,

	//END
	eOxMP_EVENT_EndofCase
} OxMediaPlay_Event_e;



#define __________TYPE__________
typedef enum
{
	eOxMP_MEDIATYPE_NONE			= 0,

	eOxMP_MEDIATYPE_LIVE,
	eOxMP_MEDIATYPE_PVRPLAYBACK,
	eOxMP_MEDIATYPE_TSRPLAYBACK,
	eOxMP_MEDIATYPE_MEDIAPLAY,

	eOxMP_MEDIATYPE_ENDOFCASE
} OxMediaPlay_MediaType_e;

/* FCC 에서 사용하는 live type */
typedef enum
{
	eOxMP_LIVETYPE_NONE			= 0,

	eOxMP_LIVETYPE_MAIN,		// live main (audio, video)
	eOxMP_LIVETYPE_PIP,			// live pip (video)
	eOxMP_LIVETYPE_FCC,			// pre-tune for fast ch change

	eOxMP_LIVETYPE_ENDOFCASE
} OxMediaPlay_LiveType_e;

typedef enum
{
	eOxMP_MEDIAPLAY_PLAYERTYPE_NONE	= 0,

	eOxMP_MEDIAPLAY_PLAYERTYPE_HMX,
	eOxMP_MEDIAPLAY_PLAYERTYPE_MSE,
	eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT,

	eOxMP_MEDIAPLAY_PLAYERTYPE_ENDOFCASE
} OxMediaPlay_PlayerType_e;

typedef enum
{
	eOxMP_MEDIAPLAY_PUMPTYPE_AV = 0,
	eOxMP_MEDIAPLAY_PUMPTYPE_A,
	eOxMP_MEDIAPLAY_PUMPTYPE_V,

	eOxMP_MEDIAPLAY_PUMPTYPE_ENDOFCASE
}OxMediaPlay_PumpType_e;

typedef enum
{
	eOxMP_AUDMODE_SINGLEMONO	= 0,
	eOxMP_AUDMODE_DUALMONO,
	eOxMP_AUDMODE_STEREO,
	eOxMP_AUDMODE_MULTILINGUAL,
	eOxMP_AUDMODE_SURROUND,
	eOxMP_AUDMODE_DOLBY,

	eOxMP_AUDMODE_ENDOFCASE
} OxMediaPlay_AudioMode_e;

typedef enum
{
	eOxMP_AUDIOTYPE_UNDEFINED			= 0,	/**< Normal Audio. */
	eOxMP_AUDIOTYPE_CLEAN_EFFECTS		= 1,
	eOxMP_AUDIOTYPE_HEARING_IMPAIRED	= 2,
	eOxMP_AUDIOTYPE_VISUAL_IMPAIRED		= 3,	/**< AD(Audio Description). */
	eOxMP_AUDIOTYPE_RESERVED			= 4
} OxMediaPlay_AUDIOType_e;

typedef enum
{
	eOxMP_AUDIOTYPE_SUPP_MAIN = 0,
	eOxMP_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED,
	eOxMP_AUDIOTYPE_SUPP_CLEAN_AUDIO_HEARING_IMPAIRED,
	eOxMP_AUDIOTYPE_SUPP_SPOKEN_SUBITITE_VISUAL_IMPARED,
	eOxMP_AUDIOTYPE_SUPPLEMENTARY_MAIN,
	eOxMP_AUDIOTYPE_SUPPLEMENTARY_UNKNOWN
} OxMediaPlay_AUDIOType_SUPP_e;

typedef enum
{
	eOxMP_TUNESTATE_NONE		= 0,

	eOxMP_TUNESTATE_LOCKED,
	eOxMP_TUNESTATE_NOSIGNAL,

	eOxMP_TUNESTATE_ENDOFCASE
} OxMediaPlay_TuneState_e;

typedef	enum
{
	eOxMP_CHECKTUNE_NoProblem	= 0,
	eOxMP_CHECKTUNE_SvcNotExist,
	eOxMP_CHECKTUNE_NoTunerResource,

	eOxMP_CheckTune_ENDOFCASE
} OxMediaPlay_CheckTune_e;

typedef enum
{
	eOxMP_TUNEMODE_NO_IPLATE		= 0x01,	/**< i-plate display 없이 튜닝함. */
	eOxMP_TUNEMODE_PINCODE_ON_TOP	= 0x02,	/**< PIN code는 exit전까지 메뉴 위에 뜸. Exit하면 바닥에 깔림.*/
	eOxMP_TUNEMODE_PINCODE_PASSED	= 0x04,	/**< PIN code는 외부에서 pass 했음.. pin code pass로 간주. */
	eOxMP_TUNEMODE_KEEP_PINPASS		= 0x08,	/**< PIN code pass flag를 현재 session 대로 유지하라. (ex. subfeed끼리 이동... ) */
	eOxMP_TUNEMODE_FORCE_TUNE		= 0x10,	/**< 현재 튜닝중인 서비스 핸들이라도 강제로 재 튜닝 시키도록 한다. */
	eOxMP_TUNEMODE_NDT_TUNE			= 0x20,	/**< MHEG의 Non Destructive Tune을 위한 mode로 이전 Video와 Audio를 끊지 않고 변경될 redkey가 변경할 service handle 만 MHEG manager로 전달하기 위함. */
	eOxMP_TUNEMODE_CA_ALTER_TUNE	= 0x40,	/**< CA 대체 서비스에 의해 튜닝됨.	*/
	eOxMP_TUNEMODE_EWS_TUNE			= 0x80,	/**< EWS에의해 튜닝됨.	*/
	eOxMP_TUNEMODE_MAX
} OxMediaPlay_TuningMode_e;

typedef	enum
{
	eOxMP_SUPPLESVCTYPE_Normal	= 0,	//Normal WatchTV session
	eOxMP_SUPPLESVCTYPE_SilentTune,		//MHEG, MHP등 data 방송에서 요청한 silent tuning
	eOxMP_SUPPLESVCTYPE_Replace,		//Free CA mode/running status change에 의한 replacement service tuning
	eOxMP_SUPPLESVCTYPE_Subfeed,		//Sub-feed service session
	eOxMP_SUPPLESVCTYPE_Tsmf_Low,		//일본향 TSMF 저계층 이동 상태
	eOxMP_SUPPLESVCTYPE_ENDOFCASE
} OxMediaPlay_SuppleSvcType_e;


typedef enum
{
	eOxMP_PLAYSTATE_NONE		= 0,

	eOxMP_PLAYSTATE_PAUSE,
	eOxMP_PLAYSTATE_STOPPING,
	eOxMP_PLAYSTATE_STOPPED,
	eOxMP_PLAYSTATE_REQUESTED,
	eOxMP_PLAYSTATE_CONNECTING,
	eOxMP_PLAYSTATE_BUFFERING,
	eOxMP_PLAYSTATE_PLAYING,
	eOxMP_PLAYSTATE_BOS,
	eOxMP_PLAYSTATE_EOS,

	eOxMP_PLAYSTATE_ENDOFCASE
} OxMediaPlay_PlayState_e;

typedef enum
{
	eOxMP_PLAYERROR_NO_ERROR	= 0,

	eOxMP_PLAYERROR_NO_RESOURCE,
	eOxMP_PLAYERROR_CONFLICT_RESOURCE,
	eOxMP_PLAYERROR_NOT_SUPPORTED,
	eOxMP_PLAYERROR_SERVER_FAIL,
	eOxMP_PLAYERROR_PRIORITY,
	eOxMP_PLAYERROR_PARENT_CONTROL,
	eOxMP_PLAYERROR_PROBE_FAILED,
	eOxMP_PLAYERROR_BUFFER_ERROR,
	eOxMP_PLAYERROR_PLAY_ERROR,
	eOxMP_PLAYERROR_SEEK_ERROR,
	eOxMP_PLAYERROR_UNKNWON,

	eOxMP_PLAYERROR_ENDOFCASE
} OxMediaPlay_PlayError_e;

typedef enum
{
	eOxMP_ERROR_REASON_NONE	= 0,
	eOxMP_ERROR_REASON_NET_UNREACHABLE,
	eOxMP_ERROR_REASON_NOT_SUPPORTED_FORMAT,
	eOxMP_ERROR_REASON_DATA_MALFORMED_GENERIC,
	eOxMP_ERROR_REASON_SERVER_HTTP_STATUS_NOT_FOUND,
	eOxMP_ERROR_REASON_SERVER_HTTP_STATUS_SERVER_ERROR,
	eOxMP_ERROR_REASON_NETWORK_UNREACHABLE,
	eOxMP_ERROR_REASON_X_KEY
	/* to do : add enum */
} OxMediaPlay_ErrorReason_e;

typedef enum
{
	eOxMP_BUFFERSTATE_None = 0,
	eOxMP_BUFFERSTATE_CanPlay,
	eOxMP_BUFFERSTATE_NotEnough,
	eOxMP_BUFFERSTATE_Enough,
	eOxMP_BUFFERSTATE_Buffering_Finished,

	eOxMP_BUFFERSTATE_ENDOFCASE
} OxMediaPlay_BufferState_e;

/* Verimatrix Status */
typedef enum
{
    eOxMP_VMXSTATUS_NONE        = 0,
    eOxMP_VMXSTATUS_UNKNOWN,
    eOxMP_VMXSTATUS_INITIALISING,
    eOxMP_VMXSTATUS_ERROR,
} OxMediaplay_VmxStatus_e;

typedef	enum
{
	eOxMP_DRMERROR_ValidLicense = 0,
	eOxMP_DRMERROR_InvalidLicense,
	eOxMP_DRMERROR_NoLicense,
} OxMediaPlay_DrmErrorStatus_e;

typedef enum
{
	eOxMP_COMPONENT_NONE	= 0,

	eOxMP_COMPONENT_VIDEO,
	eOxMP_COMPONENT_AUDIO,
	eOxMP_COMPONENT_SUBTITLE,
	eOxMP_COMPONENT_TELETEXT,
	eOxMP_COMPONENT_CLOSEDCAPTION,
	eOxMP_COMPONENT_DATA,

	eOxMP_COMPONENT_ENDOFCASE
} OxMediaPlay_Component_e;

typedef enum
{
	eOxMP_LiveViewState_OK,						/**< 정상 시청 상태. No error */
	eOxMP_LiveViewState_NONE,					/**< initial state */
	eOxMP_LiveViewState_EMPTY_SVC,				/**< 서비스 중단 메시지 */
	eOxMP_LiveViewState_NO_SIGNAL,				/**< No Signal 에러 메시지 */
	eOxMP_LiveViewState_NOK_ANTENNA,			/**< Antenna Fault 에러 메시지(지상파 경우 Short case임 */
	eOxMP_LiveViewState_MOTOR_MOVING,		/**< Motor antenna 이동 메시지 */
	eOxMP_LiveViewState_CAS,					/**< CAS 로 인한 상태  */
	eOxMP_LiveViewState_SVC_LOCKED,			/**< SVC User Lock 등으로 View가 holding 걸려버린 상태 */
	eOxMP_LiveViewState_AV_UNDER_RUN,			/**< Scrambled 또는 잘못된 PID의 경우와 같이 AV stream이 들어오지 않는 상태 */
	eOxMP_LiveViewState_HDMI_BLOCKED,			/**< Authentication failure로 인해 HDMI에서 AV block되었음. */
	eOxMP_LiveViewState_NO_VIDEO,				/**< Video가 없는 TV 채널에서 black 화면 방지용 메시지. */
	eOxMP_LiveViewState_NO_AV,					/**< Audio, Video PID가 모두 없는 경우 알림. */
	eOxMP_LiveViewState_NOT_RUNNING,				/**< SDT의 Running Status 가 NOT running으로 바뀌는 경우 메세지 . */
	eOxMP_LiveViewState_NO_SIGNAL_TIMEOUT,		/**< eOxMP_LiveViewState_NO_SIGNAL상태가 30초 이상 지속될때. WTV_UiState_t만 바뀌고 app_state_t는 변함없음 */
	eOxMP_LiveViewState_REMOVE_CH,				/** < 시청 중인 채널이 삭제된 경우 **/

	eOxMP_LiveViewState_MAX
} OxMediaPlay_LiveViewState_e;

typedef enum
{
	eOxMP_LiveLockState_OK,						/**< 정상 시청 상태. No error */
	eOxMP_LiveLockState_NONE,					/**< initial state */
	eOxMP_LiveLockState_LOCK_PIN,				/**< 유저 lock PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eOxMP_LiveLockState_LOCK_PIN_DENIED,			/**< 유저 lock PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eOxMP_LiveLockState_RATING_PIN,				/**< Rating PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eOxMP_LiveLockState_RATING_PIN_DENIED,		/**< Rating PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eOxMP_LiveLockState_SC_PIN,					/**< Smart card PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eOxMP_LiveLockState_SC_PIN_DENIED,			/**< Rating PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eOxMP_LiveLockState_NO_EIT,					/**< EIT 정보 없음. */

	eOxMP_LiveLockState_MAX
} OxMediaPlay_LiveLockState_e;

typedef	enum
{
	eSUBTTLState_Unknown	= 0,
	eSUBTTLState_Start,
	eSUBTTLState_Stopped,
	eSUBTTLState_Updated,
	eSUBTTLState_Reset
} OxMediaPlay_SUBTTLState_e;

typedef	enum
{
	eAitState_None		= 0,
	eAitState_Timeout,
	eAitState_Received
} OxMediaPlay_AitState_e;

typedef	enum
{
	eRctState_None		= 0,
	eRctState_Timeout,
	eRctState_Received
} OxMediaPlay_RctState_e;

typedef enum
{
	eMhegStatus_None			= 0,
	eMhegStatus_NotRunning,
	eMhegStatus_Starting,
	eMhegStatus_Running,
	eMhegStatus_Stopping
} OxMediaPlay_MhegStatus_e;

typedef enum
{
	eMhegPmtStatus_None			= 0,
	eMhegPmtStatus_Received,
	eMhegPmtStatus_Timeout,
	eMhegPmtStatus_PidChanged,
	eMhegPmtStatus_PidRemoved
} OxMediaPlay_MhegPmtStatus_e;

typedef enum
{
	eTsrPaused_None			= 0,
	eTsrPaused_ByDrm		= 1,
} OxMediaPlay_TsrPaused_e;

typedef enum
{
	eDrmError_NoLicense	= 0,
	eDrmError_InvalidLicense,
	eDrmError_ValidLicense
} OxMediaPlay_DrmErrorState_e;

#if defined(CONFIG_OP_NORDIG)
#define	OAPI_AUDIO_IS_AD_BM_TYPE(eAudType, ucMixType, eSubAudType)		(((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) && ((ucMixType == 0xff) && (eOxMP_AUDIOTYPE_SUPPLEMENTARY_UNKNOWN == eSubAudType))) || ((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) || ( eOxMP_AUDIOTYPE_UNDEFINED == eAudType) || (eOxMP_AUDIOTYPE_CLEAN_EFFECTS == eAudType)) && ( ((ucMixType == 1) && (eOxMP_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))
#define	OAPI_AUDIO_IS_AD_RM_TYPE(eAudType, ucMixType, eSubAudType)		(((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) && ((ucMixType == 0) && (eOxMP_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))
#else
#define	OAPI_AUDIO_IS_AD_BM_TYPE(eAudType, ucMixType, eSubAudType)		((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) ||(((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) || ( eOxMP_AUDIOTYPE_UNDEFINED == eAudType) || (eOxMP_AUDIOTYPE_CLEAN_EFFECTS == eAudType)) && ((ucMixType == 1) && (eOxMP_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))
#define	OAPI_AUDIO_IS_AD_RM_TYPE(eAudType, ucMixType, eSubAudType)		((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) || ((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) && ((ucMixType == 0) && (eOxMP_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))
#endif

#define __________DATA__________
typedef struct
{
	OxMediaPlay_MediaType_e		eType;
	OxMediaPlay_LiveType_e		eLiveType;			/*< FCC 지원시에만 사용하는 field */
	OxMediaPlay_TuningMode_e	eTuneMode;

	HUINT32						ulMasterSvcUid;
	HUINT32						ulSuppleSvcUid;
	OxMediaPlay_SuppleSvcType_e	eSuppSvcType;
	HUINT32						ulMajorCHNum;
} OxMediaPlay_StartLive_t;

typedef struct
{
	OxMediaPlay_MediaType_e		 eType;

	HUINT32						 ulContentId;
	HUINT32						 ulStartTime;
} OxMediaPlay_StartPvrPb_t;

typedef struct
{
	HUINT32 ulPos;
	HCHAR 	szName[OxMP_PVR_BOOKMARK_NAME_LEN+1];
} OxMediaPlay_Bookmark_t;

typedef struct
{
	/* TSR */
	HBOOL						bPaused;
	OxMediaPlay_TsrPaused_e		ePausedReason;
	OxMediaPlay_Bookmark_t		stPausedPoint[OxMP_PVR_BOOKMARK_MAX];

	HUINT32						ulTsrStartTime;
	HUINT32 					ulTsrRecTime;
} OxMediaPlay_Tsr_t;

typedef struct
{
	OxMediaPlay_MediaType_e		 eType;

	HUINT32						 ulMasterSvcUid;
	HUINT32						 ulSuppleSvcUid;
	OxMediaPlay_SuppleSvcType_e	 eSuppSvcType;
	HUINT32						 ulStartTime;
	HBOOL						 bStartPaused;
	HBOOL						 bUseLatestPbTime;
} OxMediaPlay_StartTsrPb_t;

typedef struct
{
	OxMediaPlay_MediaType_e		 eType;

	/* For PlayerType: eOxMP_CODEC_STREAMTYPE_MSE */
	DxVideoCodec_e				 eVideoCodec;
	DxAudioCodec_e				 eAudioCodec;
	HUINT32						 ulVideoPid;
	HUINT32						 ulAudioPid;

	/* For PlayerType: eOxMP_CODEC_STREAMTYPE_HMX */
	HCHAR						 szUrl[OxMP_MEDIAPLAY_URL];
#if defined(CONFIG_TIMESTAMP_64)
	HUINT64                      ullStartTime;
#else
	HUINT32                      ulStartTime;
#endif

} OxMediaPlay_StartMediaPlay_t;

typedef struct
{
    HUINT32     ulChunks;
    HUINT32     ullStartOffsets[OxMP_MEDIAPB_CHUNKS_MAX];
    HUINT32     ullEndOffsets[OxMP_MEDIAPB_CHUNKS_MAX];
	HUINT32		ulBufferRemainedTime;
	HUINT32		ulBufferPercent;
	HUINT32		ulSeekableChunks;
	HUINT64		ullSeekableStart[OxMP_MEDIAPB_CHUNKS_MAX];
	HUINT64		ullSeekableEnd[OxMP_MEDIAPB_CHUNKS_MAX];
} OxMediaPlay_BufferInfo_t;

typedef struct
{
	OxMediaPlay_Component_e		 eCompType;
	HINT32						 nPid;
	HINT32						 nComponentTag;
	DxVideoCodec_e				 eCodec;
	HINT32						 nCasFound;
	HINT32						 nHdVideo;

	HINT32						nStreamType;
	HINT32						nComponentType;
	HUINT8						aucStreamName[OxMP_ES_STRING_LEN];
	HUINT8						aucCompName[OxMP_COMP_NAME_NUM];
	HUINT8						aucCompRole[OxMP_COMP_ROLE_NUM];	//Video AdaptationSet Role

} OxMediaPlay_VideoComp_t;

typedef struct
{
	OxMediaPlay_AUDIOType_e	eAudType;
	HUINT8				szLangCode[DxLANG_CODE_LEN];
} OxMediaPlay_AudioALangInfo_t;

typedef struct
{
	OxMediaPlay_Component_e		 eCompType;
	HINT32						 nPid;					// Nothing : -1
	HINT32						 nComponentTag;			// Nothing : -1
	HINT32						 nLangNum;
	DxAudioCodec_e				 eCodec;
	HBOOL						 bIsDolby;
	OxMediaPlay_AudioMode_e		 eMode;
	OxMediaPlay_AudioALangInfo_t 	astLangInfo[OxMP_AUDIO_LANG_NUM];
	HUINT8							aucCompName[OxMP_COMP_NAME_NUM];	//Audio AdaptationSet ID
	HUINT8							aucCompRole[OxMP_COMP_ROLE_NUM];	//Audio AdaptationSet Role
	OxMediaPlay_AUDIOType_SUPP_e 	eSupAudType;
        HUINT32				ulSampleRate;
	HUINT8						ucMixType;

	/* For JLabs */
	HUINT8							ucComponentType;
	HUINT8							ucStreamContent;
	HUINT8							aucStreamName[OxMP_AUDIO_LANG_NUM];

} OxMediaPlay_AudioComp_t;

typedef struct
{
	HINT32						 nTypeValue;			// Nothing : -1
	HINT32						 nComponentTag;			// Nothing : -1
	HINT32						 nCompositionPageId;	// Nothing : -1
	HINT32						 nAncillaryPageId;		// Nothing : -1
	HCHAR						 szLangCode[4];			// Nothing : szLangCode[0] == '\0'
} OxMediaPlay_Subtitle_DVB_t;

typedef struct
{
	HINT32						 nTypeValue;			// Nothing : -1
	HINT32						 nComponentTag;			// Nothing : -1
	HINT32						 nMagazineNum;			// Nothing : -1
	HINT32						 nPageNum;				// Nothing : -1
	HCHAR						 szLangCode[4];			// Nothing : szLangCode[0] == '\0'
} OxMediaPlay_Subtitle_EBU_t;

typedef struct
{
	HCHAR						 szLangCode[32];
} OxMediaPlay_Subtitle_MEDIA_t;

typedef enum
{
	eOxMP_SUBTITLETYPE_Dvb				= 0,
	eOxMP_SUBTITLETYPE_Ebu				= 1,
	eOxMP_SUBTITLETYPE_Media			= 2,
	eOxMP_SUBTITLETYPE_Unknown			= 9
} OxMediaPlay_SubtitleType_e;

typedef struct
{
	OxMediaPlay_Component_e		 eCompType;
	HINT32						 nPid;					// Nothing : -1
	HINT32						 nComponentTag;			// Nothing : -1
	OxMediaPlay_SubtitleType_e	 eSubtitleType;

	union
	{
		OxMediaPlay_Subtitle_DVB_t	 stDvbInfo;
		OxMediaPlay_Subtitle_EBU_t	 stEbuInfo;
		OxMediaPlay_Subtitle_MEDIA_t stMediaInfo;
	} utArg;

} OxMediaPlay_SubtitleComp_t;

typedef struct
{
	OxMediaPlay_Component_e		 eCompType;
	HINT32						 nPid;					// Nothing : -1
	HINT32						 nComponentTag;			// Nothing : -1

	HCHAR						 szStreamName[OxMP_ES_STRING_LEN]; // Nothing : NULL
} OxMediaPlay_DataComp_t;

typedef	union
{
	OxMediaPlay_Component_e		 eCompType;
	OxMediaPlay_VideoComp_t		 stVideo;
	OxMediaPlay_AudioComp_t		 stAudio;
	OxMediaPlay_SubtitleComp_t	 stSubtitle;
	OxMediaPlay_DataComp_t		 stData;
	// .... blah blah
} OxMediaPlay_Component_t;

typedef	struct
{
	HINT32						 nCurrIndex;
	HINT32						 nArraySize, nMaxSize;
	OxMediaPlay_VideoComp_t		 astCompArray[OxMP_VIDEO_ES_NUM_MAX];
} OxMediaPlay_VideoCompTable_t;

typedef	struct
{
	HINT32						 nCurrIndex;
	HINT32						 nCurrSubIndex;	//aux audio
	HINT32						 nArraySize, nMaxSize;
	OxMediaPlay_AudioComp_t		 astCompArray[OxMP_AUDIO_ES_NUM_MAX];
} OxMediaPlay_AudioCompTable_t;

typedef	struct
{
	HINT32						 nCurrIndex;
	HINT32						 nArraySize, nMaxSize;
	OxMediaPlay_SubtitleComp_t		 astCompArray[OxMP_SUBTTL_ES_NUM_MAX];
} OxMediaPlay_SubtitleCompTable_t;

typedef	struct
{
	HINT32						 nCurrIndex;
	HINT32						 nArraySize, nMaxSize;
	OxMediaPlay_DataComp_t		 astCompArray[OxMP_DATA_ES_NUM_MAX];
} OxMediaPlay_DataCompTable_t;

typedef struct
{
#if defined(CONFIG_TIMESTAMP_64)
	HUINT64 	ullParam1;
	HUINT64 	ullParam2;
	HUINT64 	ullParam3;
#else
	HUINT32 	ulParam1;
	HUINT32 	ulParam2;
	HUINT32 	ulParam3;
#endif
} OxMediaPlay_EventNotify_t;

typedef struct
{
	HUINT32 *pulPlaypumpList;
	HUINT32 *pulDmaList;
	HUINT32 ulNumOfPlaypump;
	HUINT32 ulNumOfDma;
	HUINT32	ulAudioMixerHandle;
} OxMediaPlay_ResourceMap_t;

typedef struct
{
	HUINT32 ulNumOfPlaypump;
	HUINT32 ulNumOfDma;
	HUINT32 ulAudioMixerHandle;
	HCHAR *aResourceArray; /*ulNumOfPlaypump+ulNumOfDma*/
} OxMediaPlay_RPCResourceMap_t;

typedef struct
{
	OxMediaPlay_DrmErrorState_e eErrorState;
	HCHAR szContentID[1024];
	HCHAR szDRMSystemID[1024];
	HCHAR szRightsIssuerURL[1024];
} OxMediaPlay_DrmRightsError_t;

typedef	struct
{
	OxMediaPlay_SUBTTLState_e	eState;

	HUINT32						ulSharedKey;
	HUINT32						ulSharedSize;
	HUINT32						ulLastRetrievingOffset;
	HUINT32						ulLastPacketSize;
} OxMediaPlay_SubtitleStatus_t;

typedef struct
{
	HINT32						nStartX;
	HINT32 						nStartY;
	HINT32 						nWidth;
	HINT32 						nHeight;
	HINT32 						nMaxWidth;
	HINT32 						nMaxHeight;
	HINT32						bRoundOff;        /* only for Netflix; For getting uppper rounded_ratio_calculation */
	HINT32						bForceSourceBase; /* only for Netflix; Prevent nexus clipBase/clipRect setting error by momentary DASH source-stream change */
} OxMediaPlay_VideoSize_t;

typedef struct
{
	HUINT32						 ulZOrder;
	HINT32						 nInRect_StartX,	nInRect_StartY;
	HINT32						 nInRect_Width,		nInRect_Height;
	HINT32						 nInRect_RefWidth,	nInRect_RefHeight;
	HINT32						 nOutRect_StartX,	nOutRect_StartY;
	HINT32						 nOutRect_Width,	nOutRect_Height;
	HINT32						 nOutRect_RefWidth, nOutRect_RefHeight;

	DxAspectRatio_e 			eOsdAspectRatio;
	DxDfcSelect_e				eDfc;
	DxAspectRatio_e 			ePigAspectRatio;

} OxMediaPlay_PigSize_t;


typedef struct
{
	HBOOL					 bAvailable, bAllocated;
	DxDeliveryType_e		 eRfDeliType;
	DxTuneParam_t			 unTuneInfo;

	// Connectable Tuner Info:
} OxMediaPlay_ConfRfInfo_t;

typedef struct
{
	HBOOL					 bAvailable, bAllocated;
	DxDeliveryType_e		 eTunerDeliType;
	DxTuneParam_t			 unTuneInfo;
} OxMediaplay_ConfTunerInfo_t;

typedef struct
{
	HUINT32							ulRfInfoNum;
	OxMediaPlay_ConfRfInfo_t		astRfInfo[OxMP_CONFLICT_RFINFO_MAX];

	HUINT32							ulTunerInfoNum;
	OxMediaplay_ConfTunerInfo_t		astTunerInfo[OxMP_CONFLICT_TUNEINFO_MAX];

	HBOOL							bResourceEmpty;
} OxMediaPlay_ConflictInfo_t;

typedef union
{
	OxMediaPlay_MediaType_e		 eType;
	OxMediaPlay_StartLive_t		 stLive;
	OxMediaPlay_StartPvrPb_t	 stPvrPb;
	OxMediaPlay_StartTsrPb_t	 stTsrPb;
	OxMediaPlay_StartMediaPlay_t	 stMediaPlay;
} OxMediaPlay_StartInfo_t;

typedef struct
{
	HUINT32							ulHSize;
	HUINT32							ulVSize;
	DxAspectRatio_e					eAspectRatio;
} OxMediaPlay_VideoStreamInfo_t;

typedef union
{
	OxMediaPlay_StartInfo_t		 	stPlayStarted;				// eOxMP_EVENT_PLAY_STARTED
	OxMediaPlay_PlayState_e		 	eStateChanged;				// eOxMP_EVENT_PLAYSTATE_CHANGED
	OxMediaPlay_PlayError_e		 	eErrorChanged;				// eOxMP_EVENT_PLAYSTATE_CHANGED
	OxMediaPlay_EventNotify_t	 	stEventNotify;				//
	OxMediaPlay_VideoStreamInfo_t	stVideoStreamInfo;			// eOxMP_EVENT_AV_STARTED
	OxMediaPlay_ResourceMap_t	 	stResourceMap;				// eOxMP_EVENT_RESOURCE_MAP
	OxMediaPlay_DrmRightsError_t	stDrmRightsError;			// eOxMP_EVENT_DRM_BLOCK
} OxMediaPlay_NotifierData_t;

typedef struct
{
	OxMediaPlay_AitState_e		 eAitState;
	HUINT32						 ulAitSecCount;								// ait section number
	HUINT16						 usPrevAitPid;
	HUINT16						 usCurrAitPid;

	HUINT32						 ulAitTotaSeclLen;							// sizeof pucAitRawSec
	HUINT32						 aulAitSecLen[OxMP_AIT_SECTION_NUM_MAX];	// length of each ait section
	HUINT8						*paucAitRawSec[OxMP_AIT_SECTION_NUM_MAX];
} OxMediaPlay_AitSectionData_t;

typedef struct
{
	OxMediaPlay_RctState_e		 eRctState;

	DxService_RctInfo_t			stRctInfo;

} OxMediaPlay_RctData_t;

typedef struct
{
	// PAT
	HUINT32						 ulPatSecCount;								// number of pat section
	HUINT32						 ulPatTotalSecLen;							// sizeof pucRawSec
	HUINT32						 aulPatSecLen[OxMP_PAT_SECTION_NUM_MAX];	// length of each raw section
	HUINT8						*paucPatRawSec[OxMP_PAT_SECTION_NUM_MAX];

	// PMT
	HUINT32						 ulPmtSecCount;								// number of pmt section
	HUINT32						 ulPmtTotalSecLen;							// sizeof pucRawSec
	HUINT32						 aulPmtSecLen[OxMP_PMT_SECTION_NUM_MAX];	// length of each raw section
	HUINT8						*paucPmtRawSec[OxMP_PMT_SECTION_NUM_MAX];
} OxMediaPlay_MhegSectionData_t;

typedef struct
{
	/*
	 *	TODO : 	OAPI Manager <-> OAPI.so 간의 정보 Sync 를 위한 Cache Struct
	 *			VIEW_ID는 LIVE, PVR, TSR, MEDIA 모두 공유 가능한 Resource.
	 *			OBAMA에서 기능 동작이 유지 되는한 CACHE도 유지한다.
	 */


	/* COMMON CACHE */
	OxMediaPlay_MediaType_e		eType;
	OxMediaPlay_PlayError_e		eError;
	OxMediaPlay_ErrorReason_e	eReason;
	OxMediaPlay_PlayState_e		eState;

		/* PLAYER CACHE */
		HUINT32						ulTsrStartTime;
#if defined(CONFIG_TIMESTAMP_64)
		HUINT64 					ullDurationTime;
#else
		HUINT32 					ulDurationTime;
#endif
		HUINT32 					ulTsrRecTime;
		HUINT32 					ulBufferRemaindTime;
		HUINT32 					ulBufferPercent;
		HUINT32 					ulBufferred;
		HUINT32						ulBitrate;
		HBOOL						bInfinity;

		/* SET CACHE */
		HINT32						nSpeed;
#if defined(CONFIG_TIMESTAMP_64)
		HUINT64 					ullPosition;
#else
		HUINT32 					ulPosition;
#endif
		OxMediaPlay_VideoSize_t 	stVideoSize;
		OxMediaPlay_PigSize_t		stPigSize;
		HBOOL						bTSREnable;
		HBOOL						bSubtitleEnable;
		HBOOL						bVideoHideEnable;
		HCHAR						szAvailabilityStartTime[64];

		/* INFO CACHE */
		OxMediaPlay_VideoCompTable_t		stVideoCompTable;
		OxMediaPlay_AudioCompTable_t		stAudioCompTable;
		OxMediaPlay_SubtitleCompTable_t		stSubtitleCompTable;
		OxMediaPlay_DataCompTable_t			stDataCompTable;

		/* VIDEO STREAM INFO CACHE */
		OxMediaPlay_VideoStreamInfo_t		stVideoStreamInfo;

		DxEvent_t 					stEventInfo;
		HBOOL						bRadigBG;

		HUINT32					ulNumSpeed;
		HINT32					anSupportSpeeds[OxMP_SUPPORTING_SPEED_MAX];

		HINT32		nBlocked;
		HINT32		nRating;
		HINT32		nDrmSystemId;
#if defined(CONFIG_OP_SES)
		HINT32		nBlockedByCas;
#endif
	/* LIVE CACHE */
	OxMediaPlay_StartLive_t			stLive;
	OxMediaPlay_ConflictInfo_t		stLiveConflictInfo;
	OxMediaPlay_LiveViewState_e 	eViewState;
	OxMediaPlay_LiveLockState_e 	eLockState;
	OxMediaPlay_AitSectionData_t 	stAitSecData;
	OxMediaPlay_MhegSectionData_t	stMhegSecData;
	OxMediaPlay_RctData_t 			stRctData;
	HUINT8							szRctImageIconUrl[DxRCT_MAX_IMAGE_ICON_URL_LEN + 1];
	HUINT8						szNcdnetworkMsg[DxNCD_MAX_MESSAGE_LEN+1]; 		// network change descriptor message
	HUINT8						szNetworkUriPrefix[DxMAX_URI_PREFIX_INFO_LEN+1];	// network uri prefix

	/* TSR CACHE */
	OxMediaPlay_Tsr_t			stTsr;

	/* PVR PLAYBACK CACHE */
	OxMediaPlay_StartPvrPb_t	stPvrPb;

	/* TSR PLAYBACK CACHE */
	OxMediaPlay_StartTsrPb_t	stTsrPb;

	/* MEDIA CACHE */
	OxMediaPlay_StartMediaPlay_t	stMediaPlay;

	/* VERIMATRIX CACHE */
	HUINT8					szVmxIdentifier[128];
	OxMediaplay_VmxStatus_e eVmxStatus;
	HINT32                  nVmxErrorCode;
	HINT32					szVmxVersion[128];

	/* PLAYREADY CACHE */
	HUINT8				*pucLAURL;
	HUINT8				*pucPRDYCustomData;

		/* SET SUBTITLE CACHE*/
		HINT32	nSubtitle_SyncTime;
		HINT32	nSubtitle_FontSize;
		HINT32	nSubtitle_TextPosition;

		/* INFO CACHE */
		OxMediaPlay_SubtitleStatus_t 	stMediaSutbitleStatus;
		OxMediaPlay_BufferState_e		eBufferState;
		HUINT32							ulPumpId;

		OxMediaPlay_BufferInfo_t		stBufferInfo;
		HUINT32							unAudioDecoderVolume;


	DxCopyrightControl_TrickRestrictMode_u				uTrickRestrictMode;
#if defined(CONFIG_OP_SES)
		eDxCopyrightControl_SesStorageRight_e				eStorageRight;
#endif

} OxMediaPlay_Cache_t;

typedef void (*OxMediaPlay_Notifier_t) (
											HUINT32 ulViewId,
											HUINT32 ulSessionId,
											OxMediaPlay_MediaType_e eType,
											OxMediaPlay_Event_e eEvent,
											OxMediaPlay_NotifierData_t *pstNotifyData,
											void *pvUserData
											);
typedef HBOOL (*OxMediaPlay_EventFilter_t) (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated);
typedef void (*OxMediaPlay_ActivatedCb_t) (void);

#define __________INTERFACE__________

#define _____BASE_____
extern HERROR	OAPI_MEDIA_PLAY_Init (OxMediaPlay_ActivatedCb_t pfActivated);
extern HUINT32	OAPI_MEDIA_PLAY_GetViewNumber (void);
extern HERROR 	OAPI_MEDIA_PLAY_SetMainViewId (HUINT32 ulViewId);
extern HUINT32	OAPI_MEDIA_PLAY_GetMainViewId (void);
extern HERROR	OAPI_MEDIA_PLAY_GetPlayType (HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType);
extern HERROR	OAPI_MEDIA_PLAY_GetNeedRadioBg (HUINT32 ulViewId, HBOOL *pbNeed);
extern HERROR	OAPI_MEDIA_PLAY_SetListener (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfListener, void *pvUserData);
extern HERROR	OAPI_MEDIA_PLAY_ResetListener (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfListener, void *pvUserData);
extern HERROR	OAPI_MEDIA_PLAY_GetCurrentSessionId (HUINT32 ulViewId, HUINT32 *pulSessionId);
extern HERROR 	OAPI_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId);
extern HERROR	OAPI_MEDIA_PLAY_GetStreamAspectRatio (HUINT32 ulViewId, DxAspectRatio_e *peAspectRatio);
extern HERROR	OAPI_MEDIA_PLAY_GetStreamVideoSize(HUINT32 ulViewId, HUINT32 *ulHSize, HUINT32 *ulVSize);
extern HERROR	OAPI_MEDIA_PLAY_GetOsdDisplayPoint (HUINT32 ulViewId, HINT32 ulRefWidth, HINT32 ulRefHeight, HINT32 nVideoPointX, HINT32 nVideoPointY,HINT32 *pnScaledX, HINT32 *pnScaledY);
extern HERROR	OAPI_MEDIA_PLAY_GetDefaultSupportedSpeeds (HUINT32 ulViewId, HBOOL bScrambled);

#define _____FOR_MHEG_____
extern HERROR	OAPI_MEDIA_PLAY_CaptureStillPicture (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT32 ulCaptureWidth, HUINT32 ulCaptureHeight, DxPixelFormat_e ePixelFormat, HUINT32 ulBufferSize, HUINT8 *pBuffer);
extern HERROR	OAPI_MEDIA_PLAY_StartStillPicture (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT32 ulBufferSize, HUINT8 *pBuffer);
extern HERROR	OAPI_MEDIA_PLAY_StopStillPicture (HUINT32 ulViewId);
extern HERROR	OAPI_MEDIA_PLAY_StartAudioClip (HUINT32 ulRequestId, HUINT8 *pucBuffer, HUINT32 ulBufferBytes, DxAudioCodec_e eCodec, HUINT32 ulLoopCnt);
extern HERROR	OAPI_MEDIA_PLAY_PauseAudioClip(HUINT32 ulRequestId);
extern HERROR	OAPI_MEDIA_PLAY_ResumeAudioClip(HUINT32 ulRequestId);
extern HERROR	OAPI_MEDIA_PLAY_StopAudioClip(HUINT32 ulRequestId);
extern HERROR	OAPI_MEDIA_PLAY_SetVideoFreeze(HUINT32 ulViewId, HBOOL bFreeze);
extern HERROR	OAPI_MEDIA_PLAY_SetAudioFreeze(HUINT32 ulViewId, HBOOL bFreeze);
extern HERROR	OAPI_MEDIA_PLAY_SetMhegStatus (OxMediaPlay_MhegStatus_e eStatus);
extern HERROR	OAPI_MEDIA_PLAY_RequestPincode (HUINT32 ulViewId, OxMediaPlay_MediaType_e eMediaType, HUINT8 *pszMsg);
extern HERROR	OAPI_MEDIA_PLAY_ResultPincode (HBOOL bPermit);
extern HERROR	OAPI_MEDIA_PLAY_StopHbbtvMonitoring (HUINT32 ulViewId);


#define _____TRACK_____
extern HERROR	OAPI_MEDIA_PLAY_SetComponentIndexEx (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, HUINT8 *pstrLang);
extern HERROR	OAPI_MEDIA_PLAY_SetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex);
extern HERROR	OAPI_MEDIA_PLAY_UnsetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex);
extern HERROR	OAPI_MEDIA_PLAY_GetComponentNum (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HUINT32* pulCompNum);
extern HERROR	OAPI_MEDIA_PLAY_GetComponent (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HUINT32 ulCompIndex, OxMediaPlay_Component_t *pstComponet);
extern HERROR	OAPI_MEDIA_PLAY_GetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32* pnCompIndex, HINT32* pnCompSubIndex);

#define _____TRACK_____
extern HERROR	OAPI_MEDIA_PLAY_Start (HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart, HUINT32 *pulSessionId);
extern HERROR	OAPI_MEDIA_PLAY_Stop (HUINT32 ulViewId);
extern HERROR	OAPI_MEDIA_PLAY_StopWithParams (HUINT32 ulViewId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulSessionId);
extern HERROR	OAPI_MEDIA_PLAY_SetPlaySpeed (HUINT32 ulViewId, HINT32 nSpeed);
#if defined(CONFIG_TIMESTAMP_64)
extern HERROR	OAPI_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT64 ullPosition);
#else
extern HERROR	OAPI_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT32 ulPosition);
#endif
extern HERROR	OAPI_MEDIA_PLAY_SetPlaySegment (HUINT32 ulViewId, HUINT32 ulPosSeg);

#define _____PLAYER_GETTER_____
extern HERROR	OAPI_MEDIA_PLAY_GetPlaySpeed (HUINT32 ulViewId, HINT32 *pnSpeed);
#if defined(CONFIG_TIMESTAMP_64)
extern HERROR	OAPI_MEDIA_PLAY_GetPlayPosition (HUINT32 ulViewId, HUINT64 *pullPosition);
#else
extern HERROR	OAPI_MEDIA_PLAY_GetPlayPosition (HUINT32 ulViewId, HUINT32 *pulPosition);
#endif
extern HERROR	OAPI_MEDIA_PLAY_GetPlayState (HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState);
extern HERROR	OAPI_MEDIA_PLAY_GetPlayError (HUINT32 ulViewId, OxMediaPlay_PlayError_e *peError, OxMediaPlay_ErrorReason_e *peReason);
extern HERROR	OAPI_MEDIA_PLAY_GetBufferedTime (HUINT32 ulViewId, HUINT32 *pulBufferred);
extern HERROR	OAPI_MEDIA_PLAY_GetTsrStartTime (HUINT32 ulViewId, HUINT32 *pulTsrStartTime);
extern HERROR 	OAPI_MEDIA_PLAY_GetTsrRecTime (HUINT32 ulViewId, HUINT32 *pulTsrRecTime);
#if defined(CONFIG_TIMESTAMP_64)
extern HERROR	OAPI_MEDIA_PLAY_GetDurationTime (HUINT32 ulViewId, HUINT64 *pullDurationTime);
#else
extern HERROR	OAPI_MEDIA_PLAY_GetDurationTime (HUINT32 ulViewId, HUINT32 *pulDurationTime);
#endif


extern HERROR 	OAPI_MEDIA_PLAY_GetBitrate (HUINT32 ulViewId, HUINT32 *pulBitrate);
extern HERROR	OAPI_MEDIA_PLAY_GetSupportedSpeeds (HUINT32 ulViewId, HUINT32 *pulNumSpeed, HUINT32 ulMaxNum, HINT32 *pnSpeeds);
extern HERROR 	OAPI_MEDIA_PLAY_GetTrickRestrictMode (HUINT32 ulViewId, DxCopyrightControl_TrickRestrictMode_u *puTrickRestrictMode);
extern HERROR	OAPI_MEDIA_PLAY_GetAvailabilityStartTime (HUINT32 ulViewId, HUINT32 szTimeStringBuffer, HUINT8 *pucTimeString);
extern HERROR	OAPI_MEDIA_PLAY_GetTsrPausedPoints (HUINT32 ulViewId, HUINT32 *pulNum, OxMediaPlay_Bookmark_t **ppstBookmarks);
extern HERROR	OAPI_MEDIA_PLAY_GetTsrPaused (HUINT32 ulViewId, HBOOL *pbIsPaused);

#define _____VIEW_CONTROL_____
extern HERROR	OAPI_MEDIA_PLAY_GetVideoSize (HUINT32 ulViewId, OxMediaPlay_VideoSize_t *pstVideoSize);
extern HERROR	OAPI_MEDIA_PLAY_SetVideoSize (HUINT32 ulViewId, OxMediaPlay_VideoSize_t *pVideoSize);
extern HERROR	OAPI_MEDIA_PLAY_ResetVideoSize (HUINT32 ulViewId);
extern HERROR	OAPI_MEDIA_PLAY_SetPigRect (HUINT32 ulViewId, OxMediaPlay_PigSize_t *pPigSize);

#define _____FUNC_PROPERTY_____
extern HERROR	OAPI_MEDIA_PLAY_SetTSREnable (HUINT32 ulViewId, HBOOL bEnable);
extern HERROR	OAPI_MEDIA_PLAY_SetSubtitleEnable (HUINT32 ulViewId, HBOOL bEnable);
extern HERROR	OAPI_MEDIA_PLAY_SetVideoHideEnable (HUINT32 ulViewId, HBOOL bEnable);
extern HERROR	OAPI_MEDIA_PLAY_GetTSREnable(HUINT32 ulViewId, HBOOL *pbEnable);
extern HERROR	OAPI_MEDIA_PLAY_GetSubtitleEnable(HUINT32 ulViewId, HBOOL *pbEnable);
extern HERROR	OAPI_MEDIA_PLAY_GetVideoHideEnable(HUINT32 ulViewId, HBOOL *pbEnable);

#define _____ETC_____
extern HERROR	OAPI_MEDIA_PLAY_GetEventInfo (HUINT32 ulViewId, DxEvent_t *pstEventInfo);
extern HERROR	OAPI_MEDIA_PLAY_SaveThumbnail (HUINT32 ulViewId, HCHAR *pszFilename, HUINT32 ulWidth, HUINT32 ulHeight);

#define _____LIVE_ONLY_____
extern HERROR	OAPI_MEDIA_PLAY_GetMajorChannel (HUINT32 ulViewId, HUINT32 * pulMajorCHNum);
extern HERROR	OAPI_MEDIA_PLAY_GetConflictInfo (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_ConflictInfo_t *pstConflictInfo);
extern HERROR 	OAPI_MEDIA_PLAY_GetViewState (HUINT32 ulViewId, OxMediaPlay_LiveViewState_e *peViewState);
extern HERROR	OAPI_MEDIA_PLAY_GetLockState (HUINT32 ulViewId, OxMediaPlay_LiveLockState_e *peLockState);
extern HERROR 	OAPI_MEDIA_PLAY_CountAitSection (HUINT32 ulViewId, HUINT32 *pulAitNum);
extern HERROR 	OAPI_MEDIA_PLAY_GetAitPid (HUINT32 ulViewId, HUINT16 *pusPrevAitPid, HUINT16 *pulCurrAitPid);
extern HERROR 	OAPI_MEDIA_PLAY_GetAitSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucAitSec, HUINT32 *ulSecLen);
extern HERROR 	OAPI_MEDIA_PLAY_FreeAitSection (HUINT8 *pucAitSec);
extern HERROR 	OAPI_MEDIA_PLAY_SetTempAvPid (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT16 usVideoPid, DxAudioCodec_e eAudioCodec, HUINT16 usAudioPid, HUINT16 usPcrPid);
extern void		OAPI_MEDIA_PLAY_SetAitListenerStatus (HUINT32 ulViewId, HBOOL bEnable);
extern HERROR	OAPI_MEDIA_PLAY_CheckAitSection (HUINT32 ulViewId);
extern HERROR 	OAPI_MEDIA_PLAY_GetRctInfoCount (HUINT32 ulViewId, HUINT32 *pulTotalLinkCount , HUINT32 *pulImgDataByteSize);
extern HERROR 	OAPI_MEDIA_PLAY_GetRctInfo (HUINT32 ulViewId, DxService_RctInfo_t *pstRctInfo);
extern HERROR 	OAPI_MEDIA_PLAY_FreeRctInfo (DxService_RctInfo_t *pstRctInfo);
extern HERROR 	OAPI_MEDIA_PLAY_CountMhegPatSection (HUINT32 ulViewId, HUINT32 *pulPatNum);
extern HERROR 	OAPI_MEDIA_PLAY_GetMhegPatSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucPatSec, HUINT32 *ulSecLen);
extern HERROR 	OAPI_MEDIA_PLAY_FreeMhegPatSection (HUINT8 *pucPatSec);
extern HERROR 	OAPI_MEDIA_PLAY_CountMhegPmtSection (HUINT32 ulViewId, HUINT32 *pulPmtNum);
extern HERROR 	OAPI_MEDIA_PLAY_GetMhegPmtSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucPmtSec, HUINT32 *ulSecLen);
extern HERROR 	OAPI_MEDIA_PLAY_FreeMhegPmtSection (HUINT8 *pucPatSec);
extern HERROR 	OAPI_MEDIA_PLAY_SetMhegInterruptibleFlag (HUINT32 ulViewId, HBOOL bMhegInterruptible);
extern HERROR	OAPI_MEDIA_PLAY_GetSuppleType (HUINT32 ulViewId, OxMediaPlay_SuppleSvcType_e * peSuppSvcType);

#define _____MEDIAPB_ONLY_____
extern HERROR	OAPI_MEDIA_PLAY_GetURI (HUINT32 ulViewId, HCHAR *pszURI);
extern HERROR	OAPI_MEDIA_PLAY_Probe(HUINT32 ulViewId, HCHAR *pszURI);
extern HERROR	OAPI_MEDIA_PLAY_SetSubtitleSyncTime (HUINT32 ulViewId, HUINT32 nSubtitle_SyncTime);
extern HERROR	OAPI_MEDIA_PLAY_SetSubtitleFontSize (HUINT32 ulViewId, HUINT32 nSubtitle_FontSize);
extern HERROR	OAPI_MEDIA_PLAY_SetSubtitleTextPosition (HUINT32 ulViewId, HUINT32 nSubtitle_TextPosition);
extern HERROR	OAPI_MEDIA_PLAY_GetSubtitleSyncTime (HUINT32 ulViewId, HUINT32 *pnSubtitle_SyncTime);
extern HERROR	OAPI_MEDIA_PLAY_GetSubtitleFontSize (HUINT32 ulViewId, HUINT32 *pnSubtitle_FontSize);
extern HERROR	OAPI_MEDIA_PLAY_GetSubtitleTextPosition (HUINT32 ulViewId, HUINT32 *pnSubtitle_TextPosition);
extern HERROR	OAPI_MEDIA_PLAY_GetSubtitleStatus (HUINT32 ulViewId, OxMediaPlay_SubtitleStatus_t *pstStatus);
extern HERROR	OAPI_MEDIA_PLAY_SetBufferingScenario(HUINT32 ulViewId, HBOOL bAuto, HINT32 nThreshold);
extern HERROR	OAPI_MEDIA_PLAY_GetBufferingChunkInfo (HUINT32 ulViewId, HUINT32 *pnChunks, HUINT32 **ppulStartOffsets, HUINT32 **ppulEndOffsets);
extern HERROR	OAPI_MEDIA_PLAY_GetSeekableRangeInfo (HUINT32 ulViewId, HUINT32 *pnChunks, HUINT64 **ppullSeekableStart, HUINT64 **ppullSeekableEnd);
extern HERROR	OAPI_MEDIA_PLAY_GetBufferingRemainedTime (HUINT32 ulViewId, HUINT32 *pulBufferRemaindTime);
extern HERROR	OAPI_MEDIA_PLAY_GetBufferingPercent (HUINT32 ulViewId, HUINT32 *pulBufferPercent);
extern HERROR	OAPI_MEDIA_PLAY_GetBufferState (HUINT32 ulViewId, OxMediaPlay_BufferState_e *peBufferState);
extern HERROR	OAPI_MEDIA_PLAY_SetDrmLicenseData (HUINT32 ulViewId, const HCHAR *pszData);
extern HERROR	OAPI_MEDIA_PLAY_SetBrowserUserAgentData (HUINT32 ulViewId, HCHAR *pszUAData);
extern HERROR	OAPI_MEDIA_PLAY_GetResourceMap (OxMediaPlay_ResourceMap_t *pstResourceMap);
extern HERROR	OAPI_MEDIA_PLAY_GetPumpIndex (HUINT32 ulViewId, HUINT32 *ulPumpId);
extern HERROR	OAPI_MEDIA_PLAY_SetDuration (HUINT32 ulViewId, HUINT64 ullDuration);
extern HERROR	OAPI_MEDIA_PLAY_SetPlayerType (HUINT32 ulViewId, OxMediaPlay_PlayerType_e eMediaPlayPlayerType);
extern HERROR 	OAPI_MEDIA_PLAY_AudioFlush(HUINT32 ulViewId, HUINT32 unTryToChangeAudioTrack);
extern HERROR	OAPI_MEDIA_PLAY_AudioRestart(HUINT32 ulViewId);
extern HERROR	OAPI_MEDIA_PLAY_AudioCodecSwitch(HUINT32 ulViewId, HUINT32 unTryToChangeAudioTrack, HINT32 nAudioCodec);
extern HERROR	OAPI_MEDIA_PLAY_SetFade(HUINT32 ulViewId, HUINT32 unStartVolume, HUINT32 unTargetVolume, HINT32 nEase, HINT32 nEaseTimeElapsed, HINT32 nEaseTransitionFrames);
extern HERROR	OAPI_MEDIA_PLAY_GetAudioDecoderVolume(HUINT32 ulViewId, HUINT32 *punVolume);
extern HERROR 	OAPI_MEDIA_PLAY_SetPlayReadyInfo (HUINT32 ulViewId, HCHAR *pszLaUrl, HCHAR *pszCustomData, HBOOL bControlFlag);
extern HERROR	OAPI_MEDIA_PLAY_GetVmxIdentifier (HUINT8 **pIdentifier);
extern HERROR	OAPI_MEDIA_PLAY_GetVmxStatus (HUINT32 ulViewId, HINT32 *eStatus);
extern HERROR	OAPI_MEDIA_PLAY_GetVmxError (HUINT32 ulViewId, HINT32 *nErrorCode);
extern HERROR 	OAPI_MEDIA_PLAY_SetVmxCas (HUINT32 ulViewId, HCHAR *pszVCASUrl, HCHAR *pszVendorName);
extern HERROR	OAPI_MEDIA_PLAY_UnsetVmxCas (HUINT32 ulViewId);

extern HERROR 	OAPI_MEDIA_PLAY_MhegBgStart (HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart, HUINT32 *pulSessionId);

// @BEGIN_AUTOGEN_FUNCTION

// @END_AUTOGEN_FUNCTION
#endif // __OAPI_MEDIA_PLAY_H__
