/**************************************************************************************
 * @file nxlink_ipc_apkv2.h
 * defines APIs's internal header
 *
 * NXLINK (NetfliX external LINK API for OCTO interface)
 *
 * @author
 * @date
 * @attention   Copyright (c) 2015-2016 Humax - All rights reserved.
 *
 **************************************************************************************/
 
#ifndef __NXLINK_APKV2_H__
#define __NXLINK_APKV2_H__

#include <stdint.h>
#include <nxlink.h>

#ifdef  __cplusplus
extern "C" {
#endif

uint32_t nxl_apkv2_init(NXL_BoxInfo_t *pstBoxInfo);
void nxl_apkv2_deinit(void);

/* ____NXL_HAMA_Related_Functions____ */
uint32_t nxl_apkv2_sys_getApplicationID(uint32_t *pAppId);
uint32_t nxl_apkv2_sys_setInputGrap(uint32_t bOn);

/* ____NXL_System_Related_Functions____ */
uint32_t nxl_apkv2_sys_getModelName(char *pBuf, uint32_t size);
uint32_t nxl_apkv2_sys_getSoftwareVersion(char *pBuf, uint32_t size);
uint32_t nxl_apkv2_sys_getLoaderVersion(char *pszLoaderVer, uint32_t size);
uint32_t nxl_apkv2_sys_getPvrEnabled(uint32_t *pbEnabled);
uint32_t nxl_apkv2_sys_getSystemID(char *pszID);
uint32_t nxl_apkv2_sys_getScreenLanguage(char *pszLanguage, uint32_t size);
uint32_t nxl_apkv2_sys_getSubtitleFontType(uint32_t *pnSubtitleFontType);
uint32_t nxl_apkv2_sys_getPreferredAudioLanuage(char *pszLanguage);
uint32_t nxl_apkv2_sys_getPreferredSubtitleLanuage(char *pszLanguage);
uint32_t nxl_apkv2_sys_getPreferredMenuLanuage(char *pszLanguage);
uint32_t nxl_apkv2_sys_getDialFriendlyName(char *pBuf, uint32_t size);
uint32_t nxl_apkv2_sys_isScreensaverOn(uint32_t *onScreenSaver);
uint32_t nxl_apkv2_sys_checkNetworkChanged(uint32_t *pUpdated);

/* ____NXL_AV_Output_Related_Functions____ */
uint32_t nxl_apkv2_output_checkLastOutputVolumeChanged(uint32_t *pUpdated);
uint32_t nxl_apkv2_output_checkLastOutputHdmiChanged(uint32_t *pUpdated);
uint32_t nxl_apkv2_output_registerOutputVolumeListener(void *pFunc);
uint32_t nxl_apkv2_output_unregisterOutputVolumeListener(void *pFunc);
uint32_t nxl_apkv2_output_registerOutputHdmiListener(void *pFunc);
uint32_t nxl_apkv2_output_unregisterOutputHdmiListener(void *pFunc);

uint32_t nxl_apkv2_output_getVolumePercent(uint32_t *pPercent);
uint32_t nxl_apkv2_output_getVolumeLevel(uint32_t *pnVolume);
uint32_t nxl_apkv2_output_setVolumePercent(uint32_t percent);
uint32_t nxl_apkv2_output_setVolumeLevel( uint32_t nVolume);
uint32_t nxl_apkv2_output_getVolumeStep(uint32_t *pStep);
uint32_t nxl_apkv2_output_isMuted(uint32_t *pMute);
uint32_t nxl_apkv2_output_setMute(uint32_t mute);

/* ____NXL_Audio_Related_Functions____ */
uint32_t nxl_apkv2_audio_getSoundMode(uint32_t ulDevIdx, char* szSoundMode);
uint32_t nxl_apkv2_audio_getDigitalAudioOutputMode(eNXL_AudioDigitalOutType *pDigitalOut) ; /* (char* szDigitalOutput) */

/* ____NXL_AV_Output_Related_Functions____ */
uint32_t nxl_apkv2_output_getTVStandard(eNXL_TVOUT_STANDARD *eStandard);
//uint32_t nxl_apkv2_output_getSupportedVidoeOutput(void);

uint32_t nxl_apkv2_output_getDeviceNum(uint32_t *pnNumOfDev);
uint32_t nxl_apkv2_output_isSupportScart(uint32_t *pExisted);
uint32_t nxl_apkv2_output_isSupportTvScart(uint32_t *pExisted);
uint32_t nxl_apkv2_output_isSupportVcrScart(uint32_t *pExisted);
uint32_t nxl_apkv2_output_isSupportComposite(uint32_t *pExisted);
uint32_t nxl_apkv2_output_isSupportComponent(uint32_t *pExisted);
uint32_t nxl_apkv2_output_isSupportRF(uint32_t *pExisted);
uint32_t nxl_apkv2_output_isSupportHDMI(uint32_t *pExisted, eNXL_HdcpVer *pHdcpVer);
uint32_t nxl_apkv2_output_isSupportSpdif(uint32_t *pExisted);
uint32_t nxl_apkv2_output_isActiveOutputHDMI(uint32_t *pActivated, eNXL_HdcpVer *pHdcpVer);
uint32_t nxl_apkv2_output_isActiveOutputSpdif(uint32_t *pActivated);
uint32_t nxl_apkv2_output_getHdcpStatus(uint32_t *pbOn);
uint32_t nxl_apkv2_output_setHdcpStatus(uint32_t bOn, uint32_t bForce);
uint32_t nxl_apkv2_output_getHdcpVersion(eNXL_HdcpVer *pHdcpVer);
uint32_t nxl_apkv2_output_getCgmsStatus(eNXL_CopyRight *pCgmsStatus);
uint32_t nxl_apkv2_output_setCgmsStatus(eNXL_CopyRight eCgmsStatus);
uint32_t nxl_apkv2_output_getEdidDisplayResolution(uint32_t *puDisplayResX, uint32_t *puDisplayResY);
uint32_t nxl_apkv2_output_getActiveVideoOutput(void);
uint32_t nxl_apkv2_output_getOutputResolution(uint32_t *width, uint32_t *height);
uint32_t nxl_apkv2_output_checkLastOutputResolutionChanged(uint32_t *pUpdated);
uint32_t nxl_apkv2_output_getSupportedResolutions(int *count, char ***pszResolutions);
uint32_t nxl_apkv2_output_getTvAspectRatio(uint32_t ulDevIdx, char *pszTvAspectRatio);
uint32_t nxl_apkv2_output_getVideoStandard(uint32_t ulDevIdx, char *pszVideoStandard);

/* ____NXL_Playback_Group_Related_Functions____ */
uint32_t nxl_apkv2_mediapb_getResourceMap(void *pMap);
uint32_t nxl_apkv2_mediapb_registerNotifier(void *pFunc);
uint32_t nxl_apkv2_mediapb_unregisterNotifier(void *pFunc);
uint32_t nxl_apkv2_mediapb_start(void *pProbeInfo);
uint32_t nxl_apkv2_mediapb_stop(void);
uint32_t nxl_apkv2_mediapb_resetPlayback(void);
uint32_t nxl_apkv2_mediapb_getNotifyArgs(uint32_t ulViewId, uint32_t ulSessionId, uint32_t eType, uint32_t eEvent, void *pAVCtrlEvent);
uint32_t nxl_apkv2_mediapb_setSpeed(int32_t nSpeedSec);
uint32_t nxl_apkv2_mediapb_setPosition(uint32_t unPlayTime);
uint32_t nxl_apkv2_mediapb_getPumperIndex(uint32_t *punIndex);
uint32_t nxl_apkv2_mediapb_getPosition(uint32_t *punPosition);
uint32_t nxl_apkv2_mediapb_getPlayState(void *pState);
uint32_t nxl_apkv2_mediapb_getBufferState(void *pState);
uint32_t nxl_apkv2_mediapb_checkLastPlayCmd(NXL_PLAYBACKGROUP_CMD_e *pCmd);
uint32_t nxl_apkv2_mediapb_informLastPlayCmd(NXL_PLAYBACKGROUP_CMD_e eCmd);
uint32_t nxl_apkv2_mediapb_setDuration(double dblCurrentMaxPts);
uint32_t nxl_apkv2_mediapb_audioFlush(uint32_t unTryToChangeAudioTrack);
uint32_t nxl_apkv2_mediapb_audioRestart(void);
uint32_t nxl_apkv2_mediapb_audioCodecSwitch(uint32_t unTryToChangeAudioTrack, int32_t nAudioCodec);
uint32_t nxl_apkv2_mediapb_notifyStreamCurrentPAR(uint32_t pixelAspectRatioX, uint32_t pixelAspectRatioY); /* VideoDecoderNative */
uint32_t nxl_apkv2_mediapb_notifyStreamCurrentSize(uint32_t imageWidth, uint32_t imageHeight); /* VideoDecoderNative */
uint32_t nxl_apkv2_mediapb_notifyStreamMaxSize(uint32_t maxImageWidth, uint32_t maxImageHeight); /* PlaybackDevice::initVideoPlayer */
uint32_t nxl_apkv2_mediapb_setVideoSize(uint32_t ulPosX, uint32_t ulPosY, uint32_t ulWidth, uint32_t ulHeight);
uint32_t nxl_apkv2_mediapb_resetVideoSize(void);
uint32_t nxl_apkv2_mediapb_setPlayerType(void);
uint32_t nxl_apkv2_mediapb_setFade(uint32_t unStartVolume, uint32_t unTargetVolume, int32_t nEase, int32_t nEaseTimeElapsed, int32_t nEaseTransitionFrames);
uint32_t nxl_apkv2_mediapb_getAudioDecoderVolume(uint32_t *punVolume);

/* ____NXL_Network_Related_Functions____ */


/* #define ____NXL_Power_Related_Functions____ */


#ifdef  __cplusplus
}
#endif

#endif /* __NXLINK_APKV2_H__ */