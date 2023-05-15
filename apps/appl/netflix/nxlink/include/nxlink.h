/**************************************************************************************
 * @file nxlink.h
 * defines APIs's header
 *
 * NXLINK (NetfliX external LINK API for OCTO interface)
 *
 * @author
 * @date
 * @attention   Copyright (c) 2015-2016 Humax - All rights reserved.
 *
 **************************************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#ifndef __NXLINK_H__
#define __NXLINK_H__

#include <stdint.h>

/******************************************************************
	Define
******************************************************************/


/******************************************************************
	Enum
 ******************************************************************/
typedef enum {
	NXL_ERR_OK = 0,
	NXL_ERR_FAIL,
	NXL_ERR_INVALID_PARAM,
	NXL_ERR_UNINITIALIZED,
	NXL_ERR_ALREADY_INITIALIZED,
	NXL_ERR_UNSUPPORTED,
	NXL_ERR_MAX
} NXL_ERR;

typedef enum {
	NXL_AUDIO_DIGITAL_OUT_UNKNOWN  = 0, /* APKE_DigitalOut_e:eDigitalOut_Unknown         */
	NXL_AUDIO_DIGITAL_OUT_DOWNMIX_PCM,  /* APKE_DigitalOut_e:eDigitalOut_DownMix_PCM     */
	NXL_AUDIO_DIGITAL_OUT_BYPASS,       /* APKE_DigitalOut_e:eDigitalOut_ByPass_All or eDigitalOut_ByPass_Specific */
	NXL_AUDIO_DIGITAL_OUT_OFF           /* APKE_DigitalOut_e:eDigitalOut_Off             */
} eNXL_AudioDigitalOutType;

typedef enum {
	NXL_TVOUT_STANDARD_UNKNOWN  = 0,
	NXL_TVOUT_STANDARD_NTSC     = 1,
	NXL_TVOUT_STANDARD_PAL      = 2,
	NXL_TVOUT_STANDARD_SECAM    = 4
} eNXL_TVOUT_STANDARD;

typedef enum /* SQC HDMI  */
{
    NXL_HDMI_HOTPLUG_NONE,
    NXL_HDMI_HOTPLUG_IN,
    NXL_HDMI_HOTPLUG_OUT,
    NXL_HDMI_HOTPLUG_MAX
} eNXL_HDMIHotPlugStatus; /* Matched with APKE_HDMIHotPlugStatus_e */

typedef enum /* HDCP VERSION..*/
{
	NXL_HDCP_VERSION_1_0 = 0,
	NXL_HDCP_VERSION_1_1,
	NXL_HDCP_VERSION_1_2,
	NXL_HDCP_VERSION_1_3,
	NXL_HDCP_VERSION_1_4,
	NXL_HDCP_VERSION_2_0,
	NXL_HDCP_VERSION_2_1,
	NXL_HDCP_VERSION_2_2,
	NXL_HDCP_VERSION_NA,
} eNXL_HdcpVer; /* Matched with APKE_HdcpVer_e */

/*
 * Digital Audio Output 에 SCMS Control Mode For CI Plus(EMI) Requirement.
 * embedded cas 등에서 digtial audio copyright control 시에도 사용 한다.
 * digital audio 에 대해서 protection on/off가 다 인 경우, copy free와 copy never를 사용한다.
 */
typedef enum
{
	NXL_COPYRIGHT_COPY_FREE    = 0,			/* copy protection 없음. */
	NXL_COPYRIGHT_COPY_NOMORE  = 1,
	NXL_COPYRIGHT_COPY_ONCE    = 2,
	NXL_COPYRIGHT_COPY_NEVER   = 3
} eNXL_CopyRight; /* Matched with APKE_CopyRight_t */

typedef enum {
	NXL_AVOUT_TYPE_NONE		= 0,
	NXL_AVOUT_TYPE_VIDEO,
	NXL_AVOUT_TYPE_AUDIO,
	NXL_AVOUT_TYPE_BOTH
} eNXL_AVOutputType;

typedef enum {
	NXL_NETWORK_EVENT_NONE = 0,
	NXL_NETWORK_EVENT_ONLINE,
	NXL_NETWORK_EVENT_OFFLINE,
	NXL_NETWORK_EVENT_SWITCHING,
	NXL_NETWORK_EVENT_UNDEFINED
} eNXL_NETWORK_EVENT;

typedef enum {
	NXL_PWR_STATUS_UNKNOWN  = 0,
	NXL_PWR_STATUS_OPERATION,
	NXL_PWR_STATUS_STANDBY,
} eNXL_PWR_STATUS;

/* ES Manager */
typedef struct NXL_ESMANAGER_ResourceMap
{
	uint32_t *hPlaypumpList;
	uint32_t *hDmaList;
	uint32_t uiNumOfPlaypump;
	uint32_t uiNumOfDma;
	uint32_t uiAudioMixerHandle;
}NXL_ESMANAGER_ResourceMap_t;

/* Playback Group */

/* 	Below enumeration(NXL_PLAYBACKGROUP_MEDIA_EVENT_e) references AVController.h
	If AVController.h changes, you should check below enumerations */
typedef enum
{
	NXL_PLAYBACKGROUP_MEDIA_EVENT_Unknown = 0,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_Started            = 1,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_Stopped            = 2,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_PlaytimeChanged    = 3,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_PlayStateChanged   = 4,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_PumpIndexChanged   = 5,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_SeekReady          = 6,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioCodecSwitched = 7,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_BufferStateChanged = 8,	/* Group #2 models check underflow through buffer state. */ 
	NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioFadeCompleted = 9,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioFlushCompleted = 10,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioRestartCompleted = 11,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioFadeFailed	 		= 12,
	NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioDecoderVolumeChanged = 13,	
} NXL_PLAYBACKGROUP_MEDIA_EVENT_e;

/* 	Below enumeration(NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e) references AVController.h
	If AVController.h changes, you should check below enumerations */
typedef enum
{
	NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Stop = 0,
	NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Playing            = 1,
	NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Paused             = 2,
	NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_EndOfStream        = 3,
	NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Error              = 4,
	NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_BufferingUnderflow = 5,
}NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e;

/* 	Below enumeration(NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_e) references AVController.h
	If AVController.h changes, you should check below enumerations */
typedef enum
{
	NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_None = 0,
	NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_CanPlay,
	NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_NotEnough,
	NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_Enough,
	NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_Buffering_Finished,

	NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_ENDOFCASE
}NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_e;

typedef enum
{
	NXL_PLAYBACKGROUP_CMD_NA = 0,
	NXL_PLAYBACKGROUP_CMD_Open, /* PlaybackGroupNative::getResourceMap() or PlaybackGroupNative::csoOpen() */
	NXL_PLAYBACKGROUP_CMD_Stop,
	NXL_PLAYBACKGROUP_CMD_Start,
	NXL_PLAYBACKGROUP_CMD_Play,
	NXL_PLAYBACKGROUP_CMD_Pause,
}NXL_PLAYBACKGROUP_CMD_e;

typedef struct NXL_PLAYBACKGROUP_ProbeInfo
{
	int32_t video_codec;
	int32_t audio_codec;
	uint32_t video_pid;
	uint32_t audio_pid;
}NXL_PLAYBACKGROUP_ProbeInfo_t;

/******************************************************************
	Typedef
******************************************************************/
typedef struct
{
	char *model_name;
	char *software_version;
	char *screen_language;
	char *system_id;
}NXL_BoxInfo_t;

typedef	void (*NXL_PowerStateChange_t)(int curPowerState);

/******************************************************************
	Interfaces
******************************************************************/
#ifdef  __cplusplus
extern "C" {
#endif

void NXL_ProcEventTask(void *args);
uint32_t NXL_Init(void);
uint32_t NXL_Deinit(void);

uint32_t NXL_GetApplicationID(uint32_t *pAppId);
uint32_t NXL_SetInputGrap(uint32_t bOn);

/* system */
uint32_t NXL_SYSTEM_GetModelName(char *pBuf, uint32_t size);
uint32_t NXL_SYSTEM_GetOctoModelName(char *pBuf, uint32_t size);
uint32_t NXL_SYSTEM_GetSoftwareVersion(char *pBuf, uint32_t size);
uint32_t NXL_SYSTEM_GetFriendlyName(char *pBuf, uint32_t size);
uint32_t NXL_SYSTEM_GetLanguage(char *pBuf, uint32_t size);
uint32_t NXL_SYSTEM_GetVolume(uint32_t *pPercent);
uint32_t NXL_SYSTEM_SetVolume(uint32_t percent);
uint32_t NXL_SYSTEM_GetVolumeStep(uint32_t *pStep);
uint32_t NXL_SYSTEM_IsMuted(uint32_t *pMute);
uint32_t NXL_SYSTEM_SetMute(uint32_t mute);

/* system A/V/Output
   : https://nrd.netflix.com/display/DOC42/System+Capabilities */
uint32_t NXL_SYSTEM_GetDigitalAudioOutputMode(uint32_t *pDigitalOut); // eNXL_AudioDigitalOutType
uint32_t NXL_SYSTEM_IsDigitalAudioMultiChannel(uint32_t *pMultiChannel);
uint32_t NXL_SYSTEM_IsSupport1080pVideo(uint32_t *pSupported);
uint32_t NXL_SYSTEM_GetSupportedVidoeOutput(void);
uint32_t NXL_SYSTEM_GetOutputDeviceNum(uint32_t *pnNumOfDev);
uint32_t NXL_SYSTEM_HasOutputScart(uint32_t *pExisted);
uint32_t NXL_SYSTEM_HasOutputTvScart(uint32_t *pExisted);
uint32_t NXL_SYSTEM_HasOutputVcrScart(uint32_t *pExisted);
uint32_t NXL_SYSTEM_HasOutputComposite(uint32_t *pExisted);
uint32_t NXL_SYSTEM_HasOutputComponent(uint32_t *pExisted);
uint32_t NXL_SYSTEM_HasOutputRF(uint32_t *pExisted);
uint32_t NXL_SYSTEM_HasOutputHDMI(uint32_t *pExisted, eNXL_HdcpVer *pHdcpVer);
uint32_t NXL_SYSTEM_HasOutputSpdif(uint32_t *pExisted);
uint32_t NXL_SYSTEM_IsActiveOutputHDMI(uint32_t *pActivated, eNXL_HdcpVer *pHdcpVer);
uint32_t NXL_SYSTEM_IsActiveOutputSpdif(uint32_t *pActivated);
uint32_t NXL_SYSTEM_GetHdcpStatus(uint32_t *pbOn);
uint32_t NXL_SYSTEM_SetHdcpStatus(uint32_t bOn, uint32_t bForce);
uint32_t NXL_SYSTEM_GetHdcpVersion(eNXL_HdcpVer *pHdcpVer);
uint32_t NXL_SYSTEM_GetCgmsStatus(eNXL_CopyRight *pCgmsStatus);
uint32_t NXL_SYSTEM_SetCgmsStatus(eNXL_CopyRight eCgmsStatus);
uint32_t NXL_SYSTEM_GetEdidDisplayResolution(uint32_t *puDisplayResX, uint32_t *puDisplayResY);
uint32_t NXL_SYSTEM_GetActiveVideoOutput(uint32_t ulDevIdx, uint32_t *pActivated);
uint32_t NXL_SYSTEM_GetVideoOutputResolution(uint32_t *pWidth, uint32_t *pHeight);
uint32_t NXL_SYSTEM_HasScreenSaver(uint32_t *pExisted);
uint32_t NXL_SYSTEM_IsScreensaverOn(uint32_t *onScreenSaver);
uint32_t NXL_SYSTEM_CheckOutputVolumeUpdated(uint32_t *pUpdated);
uint32_t NXL_SYSTEM_CheckOutputHdmiUpdated(uint32_t *pUpdated);
uint32_t NXL_SYSTEM_CheckOutputResolutionUpdated(uint32_t *pUpdated);
uint32_t NXL_SYSTEM_CheckScreenSaverUpdated(uint32_t *pUpdated);
uint32_t NXL_SYSTEM_CheckNetworkEventUpdated(uint32_t *updated);
uint32_t NXL_SYSTEM_HasWifiDevice(uint32_t *hasWifi);
uint32_t NXL_SYSTEM_HasWifiEth1Bridge(uint32_t *hasWifiEth1Bridge);
uint32_t NXL_SYSTEM_RegisterOutputVolumeListener(void *pFunc); // outputVolumeChanged
uint32_t NXL_SYSTEM_UnregisterOutputVolumeListener(void *pFunc);
uint32_t NXL_SYSTEM_RegisterOutputHdmiListener(void *pFunc); // videoOutputStatusChanged
uint32_t NXL_SYSTEM_UnregisterOutputHdmiListener(void *pFunc);
uint32_t NXL_SYSTEM_RegisterOutputResolutionListener(void *pFunc); // videoOutputResolutionChanged
uint32_t NXL_SYSTEM_UnregisterOutputResolutionListener(void *pFunc);
uint32_t NXL_SYSTEM_RegisterScreenSaverListener(void *pFunc); // screensaverStateChanged
uint32_t NXL_SYSTEM_UnregisterScreenSaverListener(void *pFunc);

/* Video Output */
uint32_t NXL_SYSTEM_GetVideoOutputResolution(uint32_t *width, uint32_t *height);

/* MediaPB */
uint32_t NXL_ESMANAGER_GetResourceMap(void *pMap);

uint32_t NXL_PLAYBACKGROUP_RegisterNotifier(void *pFunc);
uint32_t NXL_PLAYBACKGROUP_UnregisterNotifier(void *pFunc);
uint32_t NXL_PLAYBACKGROUP_MediaPbStart(void *pProbeInfo);
uint32_t NXL_PLAYBACKGROUP_MediaPbStop(void);
uint32_t NXL_PLAYBACKGROUP_GetNotifyArgs(uint32_t ulViewId, uint32_t ulSessionId, uint32_t eType, uint32_t eEvent, void *pAVCtrlEvent);
uint32_t NXL_PLAYBACKGROUP_SetPlaySpeed(int32_t nSpeedSec);
uint32_t NXL_PLAYBACKGROUP_Jump(uint32_t unPlayTime);
uint32_t NXL_PLAYBACKGROUP_GetPumpIndex(uint32_t *punIndex);
uint32_t NXL_PLAYBACKGROUP_GetPlayPosition(uint32_t *punPosition);
uint32_t NXL_PLAYBACKGROUP_GetMediaCtrlPlayState(void *pState);
uint32_t NXL_PLAYBACKGROUP_GetMediaCtrlBufferState(void *pState);
uint32_t NXL_PLAYBACKGROUP_CheckLastPlayCmd(uint32_t *pCmd); /* NXL_PLAYBACKGROUP_CMD_e */
uint32_t NXL_PLAYBACKGROUP_InformLastPlayCmd(uint32_t eCmd); /* NXL_PLAYBACKGROUP_CMD_e */
uint32_t NXL_PLAYBACKGROUP_SetDuration(double dblCurrentMaxPts);
uint32_t NXL_PLAYBACKGROUP_AudioFlush(uint32_t unTryToChangeAudioTrack);
uint32_t NXL_PLAYBACKGROUP_AudioRestart(void);
uint32_t NXL_PLAYBACKGROUP_AudioCodecSwitch(uint32_t unTryToChangeAudioTrack, int32_t nAudioCodec);
uint32_t NXL_PLAYBACKGROUP_NotifyStreamCurrentPAR(uint32_t pixelAspectRatioX, uint32_t pixelAspectRatioY);
uint32_t NXL_PLAYBACKGROUP_NotifyStreamCurrentSize(uint32_t imageWidth, uint32_t imageHeight);
uint32_t NXL_PLAYBACKGROUP_NotifyStreamMaxSize(uint32_t maxImageWidth, uint32_t maxImageHeight);
uint32_t NXL_PLAYBACKGROUP_SetVideoSize(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
uint32_t NXL_PLAYBACKGROUP_ResetVideoSize(void);
uint32_t NXL_PLAYBACKGROUP_ResetPlayback(void);
uint32_t NXL_PLAYBACKGROUP_SetPlayerType(void);
uint32_t NXL_PLAYBACKGROUP_SetFade(uint32_t unStartVolume, uint32_t unTargetVolume, int32_t nEase, int32_t nEaseTimeElapsed, int32_t nEaseTransitionFrames);
uint32_t NXL_PLAYBACKGROUP_GetAudioDecoderVolume(uint32_t *punVolume);

#ifdef  __cplusplus
}
#endif

#endif /* __NXLINK_H__ */
