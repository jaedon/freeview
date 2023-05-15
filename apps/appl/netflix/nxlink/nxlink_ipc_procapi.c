
#include <stdio.h>
#include <stdlib.h>

#include <nxlink.h>
#include "nxlink_ipc_procapi.h"
#include <vkernel.h>
#if 0
#include <hlib.h>
#include <hapi.h>
#include <oapi.h>
#include <appkit.h>
#endif

/******************************************************************
	Define
******************************************************************/
#undef		HxLOG_DOMAIN
#define		HxLOG_DOMAIN				"Netflix"

#ifdef	CONFIG_DEBUG
#define NXL_LOG_DBG(...)  do { printf(__VA_ARGS__); } while(0)
#define NXL_LOG_WRN(...)  do { printf(__VA_ARGS__); } while(0)
#define NXL_LOG_ERR(...)  do { printf(__VA_ARGS__); } while(0)
#else
#define NXL_LOG_DBG(...)
#define NXL_LOG_WRN(...)
#define NXL_LOG_ERR(...)
#endif

#define __CHECK_PARAM_PTR(a) {if (a == NULL) {printf("Error> argument is NULL at [%s:%d]\n", __FUNCTION__, __LINE__); return NXL_ERR_INVALID_PARAM;}}

#define _PROCAPI_PROC_EVENT_TASK_PRIORITY	(40)
#define _PROCAPI_PROC_EVENT_TASK_SIZE		(0x4000)


typedef	struct	_NXL_PROCAPI_Info_tag
{
	uint32_t           octoInited;
	uint32_t           stopOapiTask;
	uint32_t           hasBoxInfoListener;
	uint32_t           liveStarted;
	
	uint32_t           gotVolumeInfo;
	uint32_t           volumeCur;
	uint32_t           volumeMax;
	uint32_t           volumeMin;
	uint32_t           volumeMuted;
	
	eNXL_PWR_STATUS	          powerStatus; 
	NXL_PowerStateChange_t         pfnPowerChangeCallback;
	/* APKS_MEDIADEC_RESOURCE_MAP_t  stResourceMap; */
} NXL_PROCAPI_Info_t;

/******************************************************************
	Static variables
******************************************************************/
static uint32_t s_bProcapiInited = FALSE;
static NXL_PROCAPI_Info_t s_stProcapiInfo;


/******************************************************************
	Static functions
******************************************************************/

static void local_volumeEventNotifier(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	return;
}

static void local_processEventTask(void *args)
{
	return;
}

/******************************************************************
	Interfaces
******************************************************************/
uint32_t nxl_procapi_init(void)
{
	NXL_LOG_DBG("++[%s]++\n",__FUNCTION__); 
	NXL_LOG_DBG("--[%s]--\n",__FUNCTION__); 
	return NXL_ERR_UNSUPPORTED;
}

void nxl_procapi_deinit(void)
{
	NXL_LOG_DBG("++[%s]++\n",__FUNCTION__); 
	NXL_LOG_DBG("--[%s]--\n",__FUNCTION__); 
}

uint32_t nxl_procapi_sys_getApplicationID(uint32_t *pAppId) // TODO
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_setInputGrap(uint32_t bOn)
{
	return NXL_ERR_UNSUPPORTED;
}

/* System */
uint32_t nxl_procapi_sys_getModelName(char *pBuf, uint32_t size)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_getSoftwareVersion(char *pBuf, uint32_t size)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_getLoaderVersion(char *pszLoaderVer)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_getPvrEnabled(uint32_t *pbEnabled) /* Will be used for decision whether octo is ready */
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_getSystemID(char *pszID)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_getScreenLanguage(char *pszLanguage, uint32_t size)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_getSubtitleFontType(uint32_t *pnSubtitleFontType)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_getPreferredAudioLanuage(char *pszLanguage)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_getPreferredSubtitleLanuage(char *pszLanguage)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_getPreferredMenuLanuage(char *pszLanguage)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_getFriendlyName(char *pBuf, uint32_t size)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_sys_isScreensaverOn(void) /* TODO: Screen Saver */
{
	return NXL_ERR_UNSUPPORTED;
}

#define ____NXL_Volume_Related_Functions____

/* Audio Volume Control */
uint32_t nxl_procapi_output_getVolumePercent(uint32_t *pPercent)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_getVolumeLevel(uint32_t *pnVolume)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_setVolumePercent(uint32_t percent)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_setVolumeLevel( uint32_t nVolume)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_getVolumeStep(uint32_t *pStep)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_isMuted(uint32_t *pMute)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_setMute(uint32_t mute)
{
	return NXL_ERR_UNSUPPORTED;
}


#define ____NXL_Audio_Related_Functions____

uint32_t nxl_procapi_audio_getSoundMode(uint32_t ulDevIdx, char* szSoundMode)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_audio_getDigitalAudioOutputMode(uint32_t ulDevIdx, char* szDigitalOutput)
{
	return NXL_ERR_UNSUPPORTED;
}


#define ____NXL_Output_Related_Functions____

uint32_t nxl_procapi_output_getTVStandard(eNXL_TVOUT_STANDARD *eStandard)
{
	return NXL_ERR_UNSUPPORTED;
}

/* Video Output */
uint32_t nxl_procapi_output_getSupportedVidoeOutput(void)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_getAVOutputsCount( int *pnCount)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_getNumOfDev(uint32_t *pnNumOfDev)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_isSupportScart(uint32_t ulDevIdx, uint32_t *pIsExist)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_isSupportTvScart(uint32_t ulDevIdx, uint32_t *pIsExist)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_isSupportVcrScart(uint32_t ulDevIdx, uint32_t *pIsExist)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_isSupportComposite(uint32_t ulDevIdx, uint32_t *pIsExist)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_isSupportComponent(uint32_t ulDevIdx, uint32_t *pIsExist)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_isSupportRF(uint32_t ulDevIdx, uint32_t *pIsExist)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_isSupportHdmi(uint32_t ulDevIdx, uint32_t *pIsExist)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_isSupportSpdif(uint32_t ulDevIdx, uint32_t *pIsExist)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_getActiveVideoOutput(void)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_getResolutionSelect(uint32_t ulDevIdx, char *pszResolutionSelect, int nMaxStrlen)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_getVideoOutputResolution(uint32_t *pWidth, uint32_t *pHeight)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_getSupportedResolutions(uint32_t ulDevIdx, int *count, char ***pszResolutions)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_getTvAspectRatio(uint32_t ulDevIdx, char *pszTvAspectRatio)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_output_getVideoStandard(uint32_t ulDevIdx, char *pszVideoStandard)
{
	return NXL_ERR_UNSUPPORTED;
}


#define ____NXL_Media_Related_Functions____

/* ES Manager */
uint32_t nxl_procapi_mediapb_getResourceMap(void *pMap)
{
	return NXL_ERR_UNSUPPORTED;
}

/* Playback Group */
uint32_t nxl_procapi_mediapb_registerNotifier(void *pFunc)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_unregisterNotifier(void *pFunc)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_start(void *pProbeInfo)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_stop(void)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_getNotifyArgs(uint32_t p1, void *pEvent)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_setSpeed(int32_t nSpeedSec)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_setPosition(uint32_t unPlayTime)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_getPumperIndex(uint32_t *punIndex)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_getPosition(uint32_t *punPosition)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_getPlayState(void *pState)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_setDuration(double dblCurrentMaxPts)
{
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_procapi_mediapb_audioFlush(void)
{
	return NXL_ERR_UNSUPPORTED;
}



