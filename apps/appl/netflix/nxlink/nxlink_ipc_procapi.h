
#ifndef __NXLINK_PROCAPI_H__
#define __NXLINK_PROCAPI_H__

#include <stdint.h>
#include <nxlink.h>

#ifdef  __cplusplus
extern "C" {
#endif

uint32_t nxl_procapi_init(void);
void nxl_procapi_deinit(void);

/* ____NXL_HAMA_Related_Functions____ */
uint32_t nxl_procapi_sys_getApplicationID(uint32_t *pAppId);
uint32_t nxl_procapi_sys_setInputGrap(uint32_t bOn);

/* ____NXL_System_Related_Functions____ */
uint32_t nxl_procapi_sys_getModelName(char *pBuf, uint32_t size);
uint32_t nxl_procapi_sys_getSoftwareVersion(char *pBuf, uint32_t size);
uint32_t nxl_procapi_sys_getLoaderVersion(char *pszLoaderVer);
uint32_t nxl_procapi_sys_getPvrEnabled(uint32_t *pbEnabled);
uint32_t nxl_procapi_sys_getSystemID(char *pszID);
uint32_t nxl_procapi_sys_getScreenLanguage(char *pszLanguage, uint32_t size);
uint32_t nxl_procapi_sys_getSubtitleFontType(uint32_t *pnSubtitleFontType);
uint32_t nxl_procapi_sys_getPreferredAudioLanuage(char *pszLanguage);
uint32_t nxl_procapi_sys_getPreferredSubtitleLanuage(char *pszLanguage);
uint32_t nxl_procapi_sys_getPreferredMenuLanuage(char *pszLanguage);
uint32_t nxl_procapi_sys_getFriendlyName(char *pBuf, uint32_t size);
uint32_t nxl_procapi_sys_isScreensaverOn(void);

/* ____NXL_Volume_Output_Related_Functions____ */
uint32_t nxl_procapi_output_getVolumePercent(uint32_t *pPercent);
uint32_t nxl_procapi_output_getVolumeLevel(uint32_t *pnVolume);
uint32_t nxl_procapi_output_setVolumePercent(uint32_t percent);
uint32_t nxl_procapi_output_setVolumeLevel( uint32_t nVolume);
uint32_t nxl_procapi_output_getVolumeStep(uint32_t *pStep);
uint32_t nxl_procapi_output_isMuted(uint32_t *pMute);
uint32_t nxl_procapi_output_setMute(uint32_t mute);

/* ____NXL_Audio_Related_Functions____ */
uint32_t nxl_procapi_audio_getSoundMode(uint32_t ulDevIdx, char* szSoundMode);
uint32_t nxl_procapi_audio_getDigitalAudioOutputMode(uint32_t ulDevIdx, char* szDigitalOutput);

/* ____NXL_AV_Output_Related_Functions____ */
uint32_t nxl_procapi_output_getTVStandard(eNXL_TVOUT_STANDARD *eStandard);
uint32_t nxl_procapi_output_getSupportedVidoeOutput(void);
uint32_t nxl_procapi_output_getAVOutputsCount( int *pnCount);
uint32_t nxl_procapi_output_getNumOfDev(uint32_t *pnNumOfDev);
uint32_t nxl_procapi_output_isSupportScart(uint32_t ulDevIdx, uint32_t *pIsExist);
uint32_t nxl_procapi_output_isSupportTvScart(uint32_t ulDevIdx, uint32_t *pIsExist);
uint32_t nxl_procapi_output_isSupportVcrScart(uint32_t ulDevIdx, uint32_t *pIsExist);
uint32_t nxl_procapi_output_isSupportComposite(uint32_t ulDevIdx, uint32_t *pIsExist);
uint32_t nxl_procapi_output_isSupportComponent(uint32_t ulDevIdx, uint32_t *pIsExist);
uint32_t nxl_procapi_output_isSupportRF(uint32_t ulDevIdx, uint32_t *pIsExist);
uint32_t nxl_procapi_output_isSupportHdmi(uint32_t ulDevIdx, uint32_t *pIsExist);
uint32_t nxl_procapi_output_isSupportSpdif(uint32_t ulDevIdx, uint32_t *pIsExist);
uint32_t nxl_procapi_output_getActiveVideoOutput(void);
uint32_t nxl_procapi_output_getResolutionSelect(uint32_t ulDevIdx, char *pszResolutionSelect, int nMaxStrlen);
uint32_t nxl_procapi_output_getVideoOutputResolution(uint32_t *width, uint32_t *height);
uint32_t nxl_procapi_output_getSupportedResolutions(uint32_t ulDevIdx, int *count, char ***pszResolutions);
uint32_t nxl_procapi_output_getTvAspectRatio(uint32_t ulDevIdx, char *pszTvAspectRatio);
uint32_t nxl_procapi_output_getVideoStandard(uint32_t ulDevIdx, char *pszVideoStandard);

/* ____NXL_Playback_Group_Related_Functions____ */
uint32_t nxl_procapi_mediapb_getResourceMap(void *pMap);
uint32_t nxl_procapi_mediapb_registerNotifier(void *pFunc);
uint32_t nxl_procapi_mediapb_unregisterNotifier(void *pFunc);
uint32_t nxl_procapi_mediapb_start(void *pProbeInfo);
uint32_t nxl_procapi_mediapb_stop(void);
uint32_t nxl_procapi_mediapb_getNotifyArgs(uint32_t p1, void *pEvent);
uint32_t nxl_procapi_mediapb_setSpeed(int32_t nSpeedSec);
uint32_t nxl_procapi_mediapb_setPosition(uint32_t unPlayTime);
uint32_t nxl_procapi_mediapb_getPumperIndex(uint32_t *punIndex);
uint32_t nxl_procapi_mediapb_getPosition(uint32_t *punPosition);
uint32_t nxl_procapi_mediapb_getPlayState(void *pState);
uint32_t nxl_procapi_mediapb_setDuration(double dblCurrentMaxPts);
uint32_t nxl_procapi_mediapb_audioFlush(void);

/* ____NXL_Network_Related_Functions____ */


/* #define ____NXL_Power_Related_Functions____ */


#ifdef  __cplusplus
}
#endif

#endif /* __NXLINK_PROCAPI_H__ */
