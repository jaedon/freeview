/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NativeBridge.h"

#include <cassert>
#include <fstream>

#include <nrd/AppLog.h>

using namespace std;
using namespace netflix::device;


struct  NXLINK_Symbol{

	uint32_t (*init)(void);
	uint32_t (*deinit)(void);
	uint32_t (*input_grap)(uint32_t bOn);

	/* System */
	uint32_t (*system_getSoftwareVersion)(char *pBuf, uint32_t size);
	uint32_t (*system_getModelName)(char *pBuf, uint32_t size);
	uint32_t (*system_getOctoModelName)(char *pBuf, uint32_t size);
	uint32_t (*system_getFriendlylName)(char *pBuf, uint32_t size);
	uint32_t (*system_getLanguage)(char *pBuf, uint32_t size);
	uint32_t (*system_getVolume)(uint32_t *pVolume);
	uint32_t (*system_getVolumeStep)(uint32_t *pStep);
	uint32_t (*system_isMuted)(uint32_t *pMute);
	uint32_t (*system_getOutputDeviceNum)(uint32_t *pDeviceNum);
	uint32_t (*system_hasOutputScart)(uint32_t *pExisted);
	uint32_t (*system_hasOutputTvScart)(uint32_t *pExisted);
	uint32_t (*system_hasOutputVcrScart)(uint32_t *pExisted);
	uint32_t (*system_hasOutputComposite)(uint32_t *pExisted);
	uint32_t (*system_hasOutputComponent)(uint32_t *pExisted);
	uint32_t (*system_hasOutputRF)(uint32_t *pExisted);
	uint32_t (*system_hasOutputHDMI)(uint32_t *pExisted, eNXL_HdcpVer *pHdcpVer);
	uint32_t (*system_hasOutputSpdif)(uint32_t *pExisted);
	uint32_t (*system_isActiveOutputHDMI)(uint32_t *pExisted, eNXL_HdcpVer *pHdcpVer);
	uint32_t (*system_isActiveOutputSpdif)(uint32_t *pExisted);
	uint32_t (*system_getHdcpStatus)(uint32_t *pbOn);
	uint32_t (*system_setHdcpStatus)(uint32_t setOn, uint32_t bForce);
	uint32_t (*system_getHdcpVersion)(eNXL_HdcpVer *pHdcpVer);
	uint32_t (*system_getCgmsStatus)(eNXL_CopyRight *pCgmsStatus);
	uint32_t (*system_setCgmsStatus)(eNXL_CopyRight eCgmsStatus);
	uint32_t (*system_getEdidDisplayResolution)(uint32_t *pResx, uint32_t *pResy);
	uint32_t (*system_getActiveVideoOutput)(uint32_t idx, uint32_t *pVideoOut); /* (uint32_t idx) */
	uint32_t (*system_getVideoOutputResolution)(uint32_t *width, uint32_t *height);
	uint32_t (*system_getDigitalAudioOutputMode)(uint32_t *pOutputMode); // eNXL_AudioDigitalOutType
	uint32_t (*system_isDigitalAudioMultiChannel)(uint32_t *isMulti);
	uint32_t (*system_isSupport1080pVideo)(uint32_t *isMulti);
	uint32_t (*system_checkOutputVolumeUpdated)(uint32_t *pUpdated);
	uint32_t (*system_checkOutputHdmiUpdated)(uint32_t *pUpdated);
	uint32_t (*system_checkOutputResolutionUpdated)(uint32_t *pUpdated);
	uint32_t (*system_hasScreenSaver)(uint32_t *pExisted);
	uint32_t (*system_isScreensaverOn)(uint32_t *isOn);
	uint32_t (*system_checkScreenSaverUpdated)(uint32_t *pUpdated);
	uint32_t (*system_checkNetworkEventUpdated)(uint32_t *pUpdated);
	uint32_t (*system_hasWifiDevice)(uint32_t *pExisted);
	uint32_t (*system_hasWifiEth1Bridge)(uint32_t *pExisted);
	uint32_t (*system_registerOutputVolumeNotifier)(void *pFunc);
	uint32_t (*system_unregisterOutputVolumeNotifier)(void *pFunc);
	uint32_t (*system_registerOutputHdmiNotifier)(void *pFunc);
	uint32_t (*system_unregisterOutputHdmiNotifier)(void *pFunc);
	uint32_t (*system_registerOutputResolutionNotifier)(void *pFunc);
	uint32_t (*system_unregisterOutputResolutionNotifier)(void *pFunc);
	uint32_t (*system_registerScreenSaverNotifier)(void *pFunc);
	uint32_t (*system_unregisterScreenSaverNotifier)(void *pFunc);

	/* ES Manager */
	uint32_t (*esManager_getResourceMap)(void *pMap);
	uint32_t (*esManager_getSystemMuteStatus)(uint32_t *pMute);

	/* Playback Group */
	uint32_t (*playbackGroup_registerNotifier)(void *pFunc);
	uint32_t (*playbackGroup_unregisterNotifier)(void *pFunc);
	uint32_t (*playbackGroup_mediaPbStart)(void *pProbeInfo);
	uint32_t (*playbackGroup_mediaPbStop)(void);
	uint32_t (*playbackGroup_getNotifyArgs)(uint32_t ulViewId, uint32_t ulSessionId, uint32_t eType, uint32_t eEvent, void *pAVCtrlEvent);
	uint32_t (*playbackGroup_setPlaySpeed)(int32_t nSpeedSec);
	uint32_t (*playbackGroup_jump)(uint32_t unPlayTime);
	uint32_t (*playbackGroup_getPumpIndex)(uint32_t *punIndex);
	uint32_t (*playbackGroup_getPlayPosition)(uint32_t *punPosition);
	uint32_t (*playbackGroup_getMediaCtrlPlayState)(void *pState);
	uint32_t (*playbackGroup_getMediaCtrlBufferState)(void *pState);
	uint32_t (*playbackGroup_checkLastPlayCmd)(uint32_t *eNbCmd);
	uint32_t (*playbackGroup_informLastPlayCmd)(uint32_t eNbCmd);
	uint32_t (*playbackGroup_setDuration)(double dblCurrentMaxPts);
	uint32_t (*playbackGroup_audioFlush)(uint32_t unTryToChangeAudioTrack);
	uint32_t (*playbackGroup_audioRestart)(void);
	uint32_t (*playbackGroup_audioCodecSwitch)(uint32_t unTryToChangeAudioTrack, int32_t nAudioCodec);
	uint32_t (*playbackGroup_notifyStreamCurrentPAR)(uint32_t w, uint32_t h);
	uint32_t (*playbackGroup_notifyStreamCurrentSize)(uint32_t w, uint32_t h);
	uint32_t (*playbackGroup_notifyStreamMaxSize)(uint32_t w, uint32_t h);
	uint32_t (*playbackGroup_setVideoSize)(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
	uint32_t (*playbackGroup_resetVideoSize)(void);
	uint32_t (*playbackGroup_setPlayerType)(void);
	uint32_t (*playbackGroup_setFade)(uint32_t unStartVolume, uint32_t unTargetVolume, int32_t nEase, int32_t nEaseTimeElapsed, int32_t nEaseTransitionFrames);
	uint32_t (*playbackGroup_getAudioDecoderVolume)(uint32_t *punVolume);
};

static struct NXLINK_Symbol sNxlSymbol;
static void *sLibNXL = NULL;


// NativeBridge

NativeBridge * NativeBridge::sInstance_ = NULL;
NativeBridge::System * NativeBridge::sSystem_ = NULL;
NativeBridge::ESManager * NativeBridge::sESManager_ = NULL;
NativeBridge::PlaybackGroup * NativeBridge::sPlaybackGroup_ = NULL;

NativeBridge::NativeBridge()
{
}

NativeBridge::~NativeBridge()
{
}

uint32_t NativeBridge::init()
{
#ifdef NXLINK_DYNAMIC
	printf("\n[NativeBridge] nxlink Library Mode : Dynamic\n");
	if (!sInstance_) {
		sInstance_ = new NativeBridge();

		sLibNXL = dlopen("/usr/lib/libnxlink.so", RTLD_LAZY | RTLD_GLOBAL);
		if (!sLibNXL) {
			printf("linxlink.so ...... not exist()\n");
		}
	}

	if (sLibNXL) {
		sNxlSymbol.init = (uint32_t (*)(void))dlsym(sLibNXL, "NXL_Init");
		sNxlSymbol.deinit = (uint32_t (*)(void))dlsym(sLibNXL, "NXL_Deinit");
	}
#else
	printf("\n[NativeBridge] nxlink Library Mode : Shared\n");
	if (!sInstance_) {
		sInstance_ = new NativeBridge();
		sNxlSymbol.init =  NXL_Init;
		sNxlSymbol.deinit = NXL_Deinit;
		sNxlSymbol.input_grap =  NXL_SetInputGrap;
	}
#endif

	if (sNxlSymbol.init) {
		sNxlSymbol.init();
	}

	if (!sSystem_) {
		sSystem_ = new NativeBridge::System();
	}
	if (!sESManager_) {
		sESManager_ = new NativeBridge::ESManager();
	}
	if (!sPlaybackGroup_) {
		sPlaybackGroup_ = new NativeBridge::PlaybackGroup();
	}

#if 1
	if (sNxlSymbol.input_grap) {
		sNxlSymbol.input_grap(1);
	}
#endif
	return 0;
}

uint32_t NativeBridge::deinit()
{
#if 1
	if (sNxlSymbol.input_grap) {
		sNxlSymbol.input_grap(0);
	}
#endif
	if (sNxlSymbol.deinit) {
		sNxlSymbol.deinit();
	}
	return 0;
}

const NativeBridge & NativeBridge::instance()
{
	if (!sInstance_) {
		NativeBridge::init();
	}
	return *sInstance_;
};


NativeBridge::System::System()
{

#ifdef NXLINK_DYNAMIC
	if (sLibNXL) {
		sNxlSymbol.system_getSoftwareVersion =
			(uint32_t (*)(char *, uint32_t))dlsym(sLibNXL, "NXL_SYSTEM_GetSoftwareVersion");
		sNxlSymbol.system_getModelName =
			(uint32_t (*)(char *, uint32_t))dlsym(sLibNXL, "NXL_SYSTEM_GetModelName");
		sNxlSymbol.system_getOctoModelName =
			(uint32_t (*)(char *, uint32_t))dlsym(sLibNXL, "NXL_SYSTEM_GetOctoModelName");
		sNxlSymbol.system_getFriendlylName =
			(uint32_t (*)(char *, uint32_t))dlsym(sLibNXL, "NXL_SYSTEM_GetFriendlyName");
	}
#else
	sNxlSymbol.system_getSoftwareVersion         = NXL_SYSTEM_GetSoftwareVersion;
	sNxlSymbol.system_getModelName               = NXL_SYSTEM_GetModelName;
	sNxlSymbol.system_getOctoModelName           = NXL_SYSTEM_GetOctoModelName;
	sNxlSymbol.system_getFriendlylName           = NXL_SYSTEM_GetFriendlyName;
	sNxlSymbol.system_getLanguage                = NXL_SYSTEM_GetLanguage;

	sNxlSymbol.system_getVolume                  = NXL_SYSTEM_GetVolume;
	sNxlSymbol.system_getVolumeStep              = NXL_SYSTEM_GetVolumeStep;
	sNxlSymbol.system_isMuted                    = NXL_SYSTEM_IsMuted;
	sNxlSymbol.system_getOutputDeviceNum         = NXL_SYSTEM_GetOutputDeviceNum;
	sNxlSymbol.system_hasOutputScart             = NXL_SYSTEM_HasOutputScart;
	sNxlSymbol.system_hasOutputTvScart           = NXL_SYSTEM_HasOutputTvScart;
	sNxlSymbol.system_hasOutputVcrScart          = NXL_SYSTEM_HasOutputVcrScart;
	sNxlSymbol.system_hasOutputComposite         = NXL_SYSTEM_HasOutputComposite;
	sNxlSymbol.system_hasOutputComponent         = NXL_SYSTEM_HasOutputComponent;
	sNxlSymbol.system_hasOutputRF                = NXL_SYSTEM_HasOutputRF;
	sNxlSymbol.system_hasOutputHDMI              = NXL_SYSTEM_HasOutputHDMI;
	sNxlSymbol.system_hasOutputSpdif             = NXL_SYSTEM_HasOutputSpdif;
	sNxlSymbol.system_isActiveOutputHDMI         = NXL_SYSTEM_IsActiveOutputHDMI;
	sNxlSymbol.system_isActiveOutputSpdif        = NXL_SYSTEM_IsActiveOutputSpdif;
	sNxlSymbol.system_getHdcpStatus              = NXL_SYSTEM_GetHdcpStatus;
	sNxlSymbol.system_setHdcpStatus              = NXL_SYSTEM_SetHdcpStatus;
	sNxlSymbol.system_getHdcpVersion             = NXL_SYSTEM_GetHdcpVersion;
	sNxlSymbol.system_getCgmsStatus              = NXL_SYSTEM_GetCgmsStatus;
	sNxlSymbol.system_setCgmsStatus              = NXL_SYSTEM_SetCgmsStatus;
	sNxlSymbol.system_getEdidDisplayResolution   = NXL_SYSTEM_GetEdidDisplayResolution;
	sNxlSymbol.system_getActiveVideoOutput       = NXL_SYSTEM_GetActiveVideoOutput;
	sNxlSymbol.system_getVideoOutputResolution   = NXL_SYSTEM_GetVideoOutputResolution;
	sNxlSymbol.system_getDigitalAudioOutputMode  = NXL_SYSTEM_GetDigitalAudioOutputMode;
	sNxlSymbol.system_isDigitalAudioMultiChannel = NXL_SYSTEM_IsDigitalAudioMultiChannel;
	sNxlSymbol.system_isSupport1080pVideo        = NXL_SYSTEM_IsSupport1080pVideo;
	sNxlSymbol.system_hasScreenSaver             = NXL_SYSTEM_HasScreenSaver;
	sNxlSymbol.system_isScreensaverOn            = NXL_SYSTEM_IsScreensaverOn;
	
	sNxlSymbol.system_checkOutputVolumeUpdated           = NXL_SYSTEM_CheckOutputVolumeUpdated;
	sNxlSymbol.system_checkOutputHdmiUpdated             = NXL_SYSTEM_CheckOutputHdmiUpdated;
	sNxlSymbol.system_checkOutputResolutionUpdated       = NXL_SYSTEM_CheckOutputResolutionUpdated;
	sNxlSymbol.system_checkScreenSaverUpdated            = NXL_SYSTEM_CheckScreenSaverUpdated;
	sNxlSymbol.system_checkNetworkEventUpdated           = NXL_SYSTEM_CheckNetworkEventUpdated;
	sNxlSymbol.system_hasWifiDevice                      = NXL_SYSTEM_HasWifiDevice;
	sNxlSymbol.system_hasWifiEth1Bridge                  = NXL_SYSTEM_HasWifiEth1Bridge;
	sNxlSymbol.system_registerOutputVolumeNotifier       = NXL_SYSTEM_RegisterOutputVolumeListener;
	sNxlSymbol.system_unregisterOutputVolumeNotifier     = NXL_SYSTEM_UnregisterOutputVolumeListener;
	sNxlSymbol.system_registerOutputHdmiNotifier         = NXL_SYSTEM_RegisterOutputHdmiListener;
	sNxlSymbol.system_unregisterOutputHdmiNotifier       = NXL_SYSTEM_UnregisterOutputHdmiListener;
	sNxlSymbol.system_registerOutputResolutionNotifier   = NXL_SYSTEM_RegisterOutputResolutionListener;
	sNxlSymbol.system_unregisterOutputResolutionNotifier = NXL_SYSTEM_UnregisterOutputResolutionListener;
	sNxlSymbol.system_registerScreenSaverNotifier        = NXL_SYSTEM_RegisterScreenSaverListener;
	sNxlSymbol.system_unregisterScreenSaverNotifier      = NXL_SYSTEM_UnregisterScreenSaverListener;
#endif
}

NativeBridge::System::~System()
{

}

std::string NativeBridge::System::getSoftwareVersion()
{
	char ver[64] = { 0 };
	if(sNxlSymbol.system_getSoftwareVersion)
		sNxlSymbol.system_getSoftwareVersion(ver, sizeof(ver));
	return std::string(ver);
}

std::string NativeBridge::System::getModelName()
{
	char name[32] = { 0 };
	if(sNxlSymbol.system_getModelName)
		sNxlSymbol.system_getModelName(name, sizeof(name));

	printf("[%s:%d] modelName:%s\n", __func__, __LINE__, name);
	return std::string(name);
}

std::string NativeBridge::System::getOctoModelName()
{
	char name[32] = { 0 };
	if(sNxlSymbol.system_getOctoModelName)
		sNxlSymbol.system_getOctoModelName(name, sizeof(name));

	printf("[%s:%d] octoModelName:%s\n", __func__, __LINE__, name);
	return std::string(name);
}

std::string NativeBridge::System::getFriendlyName()
{
	char name[256] = { 0 };
	if(sNxlSymbol.system_getFriendlylName)
		sNxlSymbol.system_getFriendlylName(name, sizeof(name));
	return std::string(name);
}

std::string NativeBridge::System::getLanguage()
{
	char lang[32] = { 0 };
	if(sNxlSymbol.system_getLanguage)
		sNxlSymbol.system_getLanguage(lang, sizeof(lang));
	return std::string(lang);
}

uint32_t NativeBridge::System::getVolume()
{
	uint32_t volume = 0;
	if(sNxlSymbol.system_getVolume)
		sNxlSymbol.system_getVolume(&volume);
	return volume;
}

uint32_t NativeBridge::System::getVolumeStep()
{
	uint32_t step = 5;
	if(sNxlSymbol.system_getVolumeStep)
		sNxlSymbol.system_getVolumeStep(&step);
	return step;
}

bool NativeBridge::System::isMuted()
{
	uint32_t mute = 0;
	if(sNxlSymbol.system_isMuted)
		sNxlSymbol.system_isMuted(&mute);
	return mute ? true : false;
}

uint32_t NativeBridge::System::getOutputDeviceNum()
{
	uint32_t outputNum = 5;
	if(sNxlSymbol.system_getOutputDeviceNum)
		sNxlSymbol.system_getOutputDeviceNum(&outputNum);
	return outputNum;
}

bool NativeBridge::System::hasOutputScart()
{
	uint32_t isSupported = 0;
	if(sNxlSymbol.system_hasOutputScart)
		sNxlSymbol.system_hasOutputScart(&isSupported);
	return isSupported ? true : false;
}

bool NativeBridge::System::hasOutputTvScart()
{
	uint32_t isSupported = 0;
	if(sNxlSymbol.system_hasOutputTvScart)
		sNxlSymbol.system_hasOutputTvScart(&isSupported);
	return isSupported ? true : false;
}

bool NativeBridge::System::hasOutputVcrScart()
{
	uint32_t isSupported = 0;
	if(sNxlSymbol.system_hasOutputVcrScart)
		sNxlSymbol.system_hasOutputVcrScart(&isSupported);
	return isSupported ? true : false;
}

bool NativeBridge::System::hasOutputComposite()
{
	uint32_t isSupported = 0;
	if(sNxlSymbol.system_hasOutputComposite)
		sNxlSymbol.system_hasOutputComposite(&isSupported);
	return isSupported ? true : false;
}

bool NativeBridge::System::hasOutputComponent()
{
	uint32_t isSupported = 0;
	if(sNxlSymbol.system_hasOutputComponent)
		sNxlSymbol.system_hasOutputComponent(&isSupported);
	return isSupported ? true : false;
}

bool NativeBridge::System::hasOutputRF()
{
	uint32_t isSupported = 0;
	if(sNxlSymbol.system_hasOutputRF)
		sNxlSymbol.system_hasOutputRF(&isSupported);
	return isSupported ? true : false;
}

uint32_t NativeBridge::System::hasOutputHDMI(bool *pHdmiSupported, NativeBridge_HDCPVersion_e *pHdcpVer)
{
	uint32_t isSupported = 0;
	eNXL_HdcpVer eHdcpVer = NXL_HDCP_VERSION_NA;

	*pHdmiSupported = false; *pHdcpVer = NativeBridge_HDCP_NOT_ENGAGED;
	if(sNxlSymbol.system_hasOutputHDMI)
	{
		sNxlSymbol.system_hasOutputHDMI(&isSupported, &eHdcpVer);
		*pHdmiSupported = isSupported ? true : false;

		switch (eHdcpVer) {
		case NXL_HDCP_VERSION_1_0 : *pHdcpVer = NativeBridge_HDCP_1_0; break;
		case NXL_HDCP_VERSION_1_1 : *pHdcpVer = NativeBridge_HDCP_1_1; break;
		case NXL_HDCP_VERSION_1_2 : *pHdcpVer = NativeBridge_HDCP_1_2; break;
		case NXL_HDCP_VERSION_1_3 : *pHdcpVer = NativeBridge_HDCP_1_3; break;
		case NXL_HDCP_VERSION_1_4 : *pHdcpVer = NativeBridge_HDCP_1_4; break;
		case NXL_HDCP_VERSION_2_0 : *pHdcpVer = NativeBridge_HDCP_2_0; break;
		case NXL_HDCP_VERSION_2_1 : *pHdcpVer = NativeBridge_HDCP_2_1; break;
		case NXL_HDCP_VERSION_2_2 : *pHdcpVer = NativeBridge_HDCP_2_2; break;
		default:
			*pHdcpVer = NativeBridge_HDCP_NOT_ENGAGED; break; /* or NativeBridge_HDCP_NOT_APPLICABLE */
		}
	}
	return 0;
}

bool NativeBridge::System::hasOutputSpdif()
{
	uint32_t isSupported = 0;
	if(sNxlSymbol.system_hasOutputSpdif)
		sNxlSymbol.system_hasOutputSpdif(&isSupported);
	return isSupported ? true : false;
}

uint32_t NativeBridge::System::isActiveOutputHDMI(bool *pHdmiActivated, NativeBridge_HDCPVersion_e *pHdcpVer)
{
	uint32_t isActivated = 0;
	eNXL_HdcpVer eHdcpVer = NXL_HDCP_VERSION_NA;

	*pHdmiActivated = false; *pHdcpVer = NativeBridge_HDCP_NOT_ENGAGED;
	if(sNxlSymbol.system_hasOutputHDMI)
	{
		sNxlSymbol.system_isActiveOutputHDMI(&isActivated, &eHdcpVer);
		*pHdmiActivated = isActivated ? true : false;

		switch (eHdcpVer) {
		case NXL_HDCP_VERSION_1_0 : *pHdcpVer = NativeBridge_HDCP_1_0; break;
		case NXL_HDCP_VERSION_1_1 : *pHdcpVer = NativeBridge_HDCP_1_1; break;
		case NXL_HDCP_VERSION_1_2 : *pHdcpVer = NativeBridge_HDCP_1_2; break;
		case NXL_HDCP_VERSION_1_3 : *pHdcpVer = NativeBridge_HDCP_1_3; break;
		case NXL_HDCP_VERSION_1_4 : *pHdcpVer = NativeBridge_HDCP_1_4; break;
		case NXL_HDCP_VERSION_2_0 : *pHdcpVer = NativeBridge_HDCP_2_0; break;
		case NXL_HDCP_VERSION_2_1 : *pHdcpVer = NativeBridge_HDCP_2_1; break;
		case NXL_HDCP_VERSION_2_2 : *pHdcpVer = NativeBridge_HDCP_2_2; break;
		default:
			*pHdcpVer = NativeBridge_HDCP_NOT_ENGAGED; break; /* or NativeBridge_HDCP_NOT_APPLICABLE */
		}
	}
	return 0;
}

bool NativeBridge::System::isActiveOutputSpdif()
{
	uint32_t isActivated = 0;
	if(sNxlSymbol.system_isActiveOutputSpdif)
		sNxlSymbol.system_isActiveOutputSpdif(&isActivated);
	return isActivated ? true : false;
}

uint32_t NativeBridge::System::getHdcpStatus()
{
	uint32_t isOn = 5;
	if(sNxlSymbol.system_getHdcpStatus)
		sNxlSymbol.system_getHdcpStatus(&isOn);
	return isOn;
}

uint32_t NativeBridge::System::setHdcpStatus(uint32_t setOn, uint32_t bForce)
{
	if(sNxlSymbol.system_setHdcpStatus)
		sNxlSymbol.system_setHdcpStatus(setOn, bForce);
	return 0;
}

uint32_t NativeBridge::System::getHdcpVersion() // eNXL_HdcpVer
{
	eNXL_HdcpVer hdcpVer;
	if(sNxlSymbol.system_getHdcpVersion)
		sNxlSymbol.system_getHdcpVersion(&hdcpVer);
	return (uint32_t)hdcpVer;
}

uint32_t NativeBridge::System::getCgmsStatus() // eNXL_CopyRight
{
	eNXL_CopyRight cgmsStatus;
	if(sNxlSymbol.system_getCgmsStatus)
		sNxlSymbol.system_getCgmsStatus(&cgmsStatus);
	return (uint32_t)cgmsStatus;
}

uint32_t NativeBridge::System::setCgmsStatus(eNXL_CopyRight cgmsStatus) // eNXL_CopyRight
{
	if(sNxlSymbol.system_setCgmsStatus)
		sNxlSymbol.system_setCgmsStatus(cgmsStatus);
	return 0;
}

uint32_t NativeBridge::System::getEdidDisplayResolution(uint32_t *pResx, uint32_t *pResy)
{
	uint32_t resx = 0, resy = 0;
	*pResx = *pResy = 0;
	if(sNxlSymbol.system_getEdidDisplayResolution)
	{
		sNxlSymbol.system_getEdidDisplayResolution(&resx, &resy);
		*pResx = resx; *pResy = resy;
	}
	return 0;
}

uint32_t NativeBridge::System::getActiveVideoOutput(uint32_t devidx)
{
	uint32_t activated = 0;
	if(sNxlSymbol.system_getActiveVideoOutput)
		sNxlSymbol.system_getActiveVideoOutput(devidx, &activated);
	return activated;
}

uint32_t NativeBridge::System::getVideoOutputResolution(uint32_t *width, uint32_t *height)
{
	if(sNxlSymbol.system_getVideoOutputResolution)
		sNxlSymbol.system_getVideoOutputResolution(width, height);
	return 0;
}

uint32_t NativeBridge::System::getDigitalAudioOutputMode()
{
	uint32_t outputmode = 0; // eNXL_AudioDigitalOutType
	if(sNxlSymbol.system_getDigitalAudioOutputMode)
		sNxlSymbol.system_getDigitalAudioOutputMode(&outputmode);
	return outputmode;
}

uint32_t NativeBridge::System::isDigitalAudioMultiChannel()
{
	uint32_t multi = 0;
	if(sNxlSymbol.system_isDigitalAudioMultiChannel)
		sNxlSymbol.system_isDigitalAudioMultiChannel(&multi);
	return multi;
}

uint32_t NativeBridge::System::isSupport1080pVideo()
{
	uint32_t supported = 0;
	if(sNxlSymbol.system_isSupport1080pVideo)
		sNxlSymbol.system_isSupport1080pVideo(&supported);
	return supported;
}

uint32_t NativeBridge::System::checkOutputVolumeUpdated()
{
	uint32_t updated = 0;
	if(sNxlSymbol.system_checkOutputVolumeUpdated)
		sNxlSymbol.system_checkOutputVolumeUpdated(&updated);
	return updated;
}

uint32_t NativeBridge::System::checkOutputHdmiUpdated()
{
	uint32_t updated = 0;
	if(sNxlSymbol.system_checkOutputHdmiUpdated)
	{
		sNxlSymbol.system_checkOutputHdmiUpdated(&updated);
	}
	return updated;
}

uint32_t NativeBridge::System::checkOutputResolutionUpdated()
{
	uint32_t updated = 0;
	if(sNxlSymbol.system_checkOutputResolutionUpdated)
		sNxlSymbol.system_checkOutputResolutionUpdated(&updated);
	return updated;
}

bool NativeBridge::System::hasScreenSaver()
{
	uint32_t isSupported = 0;
	if(sNxlSymbol.system_hasScreenSaver)
		sNxlSymbol.system_hasScreenSaver(&isSupported);
	return isSupported ? true : false;
}

uint32_t NativeBridge::System::isScreensaverOn()
{
	uint32_t running = 0;
	if(sNxlSymbol.system_isScreensaverOn)
		sNxlSymbol.system_isScreensaverOn(&running);
	return running;
}

uint32_t NativeBridge::System::checkScreenSaverUpdated()
{
	uint32_t updated = 0;
	if(sNxlSymbol.system_checkScreenSaverUpdated)
		sNxlSymbol.system_checkScreenSaverUpdated(&updated);
	return updated;
}

uint32_t NativeBridge::System::checkNetworkEventUpdated()
{
	uint32_t updated = 0;
	if(sNxlSymbol.system_checkNetworkEventUpdated)
		sNxlSymbol.system_checkNetworkEventUpdated(&updated);
	return updated;
}

bool NativeBridge::System::hasWifiDevice()
{
	uint32_t hasWifiDevice = 0;
	if(sNxlSymbol.system_hasWifiDevice)
		sNxlSymbol.system_hasWifiDevice(&hasWifiDevice);
	return hasWifiDevice ? true : false;
}

bool NativeBridge::System::hasWifiEth1Bridge()
{
	uint32_t hasWifiEth1Bridge = 0;
	if(sNxlSymbol.system_hasWifiEth1Bridge)
		sNxlSymbol.system_hasWifiEth1Bridge(&hasWifiEth1Bridge);
	return hasWifiEth1Bridge ? true : false;
}

uint32_t NativeBridge::System::registerOutputVolumeNotifier(void *pFunc)
{
	uint32_t eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pFunc == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto func_error;
	}

	if(sNxlSymbol.system_registerOutputVolumeNotifier)
	{
		(void)sNxlSymbol.system_registerOutputVolumeNotifier(pFunc);
	}

func_error:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

uint32_t NativeBridge::System::unregisterOutputVolumeNotifier(void *pFunc)
{
	uint32_t eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pFunc == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto func_error;
	}

	if(sNxlSymbol.system_unregisterOutputVolumeNotifier)
	{
		(void)sNxlSymbol.system_unregisterOutputVolumeNotifier(pFunc);
	}

func_error:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

uint32_t NativeBridge::System::registerOutputHdmiNotifier(void *pFunc)
{
	uint32_t eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pFunc == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto func_error;
	}

	if(sNxlSymbol.system_registerOutputHdmiNotifier)
	{
		(void)sNxlSymbol.system_registerOutputHdmiNotifier(pFunc);
	}

func_error:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

uint32_t NativeBridge::System::unregisterOutputHdmiNotifier(void *pFunc)
{
	uint32_t eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pFunc == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto func_error;
	}

	if(sNxlSymbol.system_unregisterOutputHdmiNotifier)
	{
		(void)sNxlSymbol.system_unregisterOutputHdmiNotifier(pFunc);
	}

func_error:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

uint32_t NativeBridge::System::registerOutputResolutionNotifier(void *pFunc)
{
	uint32_t eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pFunc == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto func_error;
	}

	if(sNxlSymbol.system_registerOutputResolutionNotifier)
	{
		(void)sNxlSymbol.system_registerOutputResolutionNotifier(pFunc);
	}

func_error:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

uint32_t NativeBridge::System::unregisterOutputResolutionNotifier(void *pFunc)
{
	uint32_t eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pFunc == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto func_error;
	}

	if(sNxlSymbol.system_unregisterOutputResolutionNotifier)
	{
		(void)sNxlSymbol.system_unregisterOutputResolutionNotifier(pFunc);
	}

func_error:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

uint32_t NativeBridge::System::registerScreenSaverNotifier(void *pFunc)
{
	uint32_t eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pFunc == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto func_error;
	}

	if(sNxlSymbol.system_registerScreenSaverNotifier)
	{
		(void)sNxlSymbol.system_registerScreenSaverNotifier(pFunc);
	}

func_error:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

uint32_t NativeBridge::System::unregisterScreenSaverNotifier(void *pFunc)
{
	uint32_t eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pFunc == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto func_error;
	}

	if(sNxlSymbol.system_unregisterScreenSaverNotifier)
	{
		(void)sNxlSymbol.system_unregisterScreenSaverNotifier(pFunc);
	}

func_error:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge::ESManager::ESManager()
{
#ifdef NXLINK_DYNAMIC
#else
	sNxlSymbol.esManager_getResourceMap = NXL_ESMANAGER_GetResourceMap;
	sNxlSymbol.esManager_getSystemMuteStatus = NXL_SYSTEM_IsMuted;	
#endif
}

NativeBridge::ESManager::~ESManager()
{

}

NativeBridge_ERR_e NativeBridge::ESManager::getResourceMap(void *pMap)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pMap == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pMap is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.esManager_getResourceMap)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.esManager_getResourceMap(pMap);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to get a resource map, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::ESManager::getSystemMuteStatus(bool *pMute)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
    uint32_t bMute = 0;
		    
	if(pMute == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pMap is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.esManager_getSystemMuteStatus)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.esManager_getSystemMuteStatus(&bMute);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to get mute status, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	    if(bMute)
	    {
		    *pMute = true;    
		}
		else
		{
			*pMute = false;
		}
	}
FUNCTION_EXIT:
	return eRet;
}

NativeBridge::PlaybackGroup::PlaybackGroup()
{
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
#ifdef NXLINK_DYNAMIC
#else
	sNxlSymbol.playbackGroup_registerNotifier                = NXL_PLAYBACKGROUP_RegisterNotifier;
	sNxlSymbol.playbackGroup_unregisterNotifier              = NXL_PLAYBACKGROUP_UnregisterNotifier;
	sNxlSymbol.playbackGroup_mediaPbStart                    = NXL_PLAYBACKGROUP_MediaPbStart;
	sNxlSymbol.playbackGroup_mediaPbStop                     = NXL_PLAYBACKGROUP_MediaPbStop;
	sNxlSymbol.playbackGroup_getNotifyArgs                   = NXL_PLAYBACKGROUP_GetNotifyArgs;
	sNxlSymbol.playbackGroup_setPlaySpeed                    = NXL_PLAYBACKGROUP_SetPlaySpeed;
	sNxlSymbol.playbackGroup_jump                            = NXL_PLAYBACKGROUP_Jump;
	sNxlSymbol.playbackGroup_getPumpIndex                    = NXL_PLAYBACKGROUP_GetPumpIndex;
	sNxlSymbol.playbackGroup_getPlayPosition                 = NXL_PLAYBACKGROUP_GetPlayPosition;
	sNxlSymbol.playbackGroup_getMediaCtrlPlayState           = NXL_PLAYBACKGROUP_GetMediaCtrlPlayState;
	sNxlSymbol.playbackGroup_getMediaCtrlBufferState		 = NXL_PLAYBACKGROUP_GetMediaCtrlBufferState;
	sNxlSymbol.playbackGroup_checkLastPlayCmd                = NXL_PLAYBACKGROUP_CheckLastPlayCmd;
	sNxlSymbol.playbackGroup_informLastPlayCmd               = NXL_PLAYBACKGROUP_InformLastPlayCmd;
	sNxlSymbol.playbackGroup_setDuration                     = NXL_PLAYBACKGROUP_SetDuration;
	sNxlSymbol.playbackGroup_audioFlush                      = NXL_PLAYBACKGROUP_AudioFlush;
	sNxlSymbol.playbackGroup_audioRestart                    = NXL_PLAYBACKGROUP_AudioRestart;
	sNxlSymbol.playbackGroup_audioCodecSwitch                = NXL_PLAYBACKGROUP_AudioCodecSwitch;
	sNxlSymbol.playbackGroup_notifyStreamCurrentPAR          = NXL_PLAYBACKGROUP_NotifyStreamCurrentPAR;
	sNxlSymbol.playbackGroup_notifyStreamCurrentSize         = NXL_PLAYBACKGROUP_NotifyStreamCurrentSize;
	sNxlSymbol.playbackGroup_notifyStreamMaxSize             = NXL_PLAYBACKGROUP_NotifyStreamMaxSize;
	sNxlSymbol.playbackGroup_setVideoSize                    = NXL_PLAYBACKGROUP_SetVideoSize;
	sNxlSymbol.playbackGroup_resetVideoSize                  = NXL_PLAYBACKGROUP_ResetVideoSize;
	sNxlSymbol.playbackGroup_setPlayerType					 = NXL_PLAYBACKGROUP_SetPlayerType;
	sNxlSymbol.playbackGroup_setFade						 = NXL_PLAYBACKGROUP_SetFade;
	sNxlSymbol.playbackGroup_getAudioDecoderVolume			 = NXL_PLAYBACKGROUP_GetAudioDecoderVolume;
#endif
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
}

NativeBridge::PlaybackGroup::~PlaybackGroup()
{
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::registerNotifier(void *pFunc)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pFunc == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.playbackGroup_registerNotifier)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_registerNotifier(pFunc);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to register a notifier, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::unregisterNotifier(void *pFunc)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pFunc == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.playbackGroup_unregisterNotifier)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_unregisterNotifier(pFunc);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to unregister a notifier, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::mediaPbStart(void *pProbeInfo)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pProbeInfo == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pProbeInfo is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.playbackGroup_mediaPbStart)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_mediaPbStart(pProbeInfo);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to start Media PB, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::mediaPbStop()
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(sNxlSymbol.playbackGroup_mediaPbStop)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_mediaPbStop();
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to stop Media PB, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::getNotifyArgs(uint32_t ulViewId, uint32_t ulSessionId, uint32_t eType, uint32_t eEvent, void *pAVCtrlEvent)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	//printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if (pAVCtrlEvent == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pEvent is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.playbackGroup_getNotifyArgs)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_getNotifyArgs(ulViewId, ulSessionId, eType, eEvent, pAVCtrlEvent);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to get notified arguments, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	//printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::setPlaySpeed(int32_t nSpeedSec)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(sNxlSymbol.playbackGroup_setPlaySpeed)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_setPlaySpeed(nSpeedSec);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to set playing speed(%d), eRet = %d\033[0m\n", __func__, __LINE__, nSpeedSec, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::jump(uint32_t unPlayTime)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(sNxlSymbol.playbackGroup_jump)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_jump(unPlayTime);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to jump to position(%d), eRet = %d\033[0m\n", __func__, __LINE__, unPlayTime, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::getPumpIndex(uint32_t *punIndex)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(punIndex == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] punIndex is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.playbackGroup_getPumpIndex)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_getPumpIndex(punIndex);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to get a playpump index, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::getPlayPosition(uint32_t *punPosition)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	//printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(punPosition == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] punPosition is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.playbackGroup_getPlayPosition)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_getPlayPosition(punPosition);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to get a playing position, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	//printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::getMediaCtrlPlayState(void *pState)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pState == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pState is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.playbackGroup_getMediaCtrlPlayState)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_getMediaCtrlPlayState(pState);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to get a MCtrl play state, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::getMediaCtrlBufferState(void *pState)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pState == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] pState is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.playbackGroup_getMediaCtrlBufferState)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_getMediaCtrlBufferState(pState);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to get a MCtrl buffer state, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::checkLastPlayCmd(NativeBridge_PlayCmd_e *pNbCmd) /* NativeBridge_PlayCmd_e */
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	NXL_PLAYBACKGROUP_CMD_e eNxlCmd = NXL_PLAYBACKGROUP_CMD_NA;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	*pNbCmd = NativeBridge_PLAYCMD_NA;
	if (pNbCmd == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] Error> pState is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if (sNxlSymbol.playbackGroup_checkLastPlayCmd)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_checkLastPlayCmd((uint32_t *)&eNxlCmd);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Error> Failed to check play state, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}

		switch (eNxlCmd) {
		case NXL_PLAYBACKGROUP_CMD_NA      : *pNbCmd = NativeBridge_PLAYCMD_NA; break;
		case NXL_PLAYBACKGROUP_CMD_Open    : *pNbCmd = NativeBridge_PLAYCMD_OPEN; break;
		case NXL_PLAYBACKGROUP_CMD_Stop    : *pNbCmd = NativeBridge_PLAYCMD_STOP; break;
		case NXL_PLAYBACKGROUP_CMD_Start   : *pNbCmd = NativeBridge_PLAYCMD_START; break;
		case NXL_PLAYBACKGROUP_CMD_Play    : *pNbCmd = NativeBridge_PLAYCMD_PLAY; break;
		case NXL_PLAYBACKGROUP_CMD_Pause   : *pNbCmd = NativeBridge_PLAYCMD_PAUSE; break;
		default:
			*pNbCmd = NativeBridge_PLAYCMD_NA; break;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::informLastPlayCmd(NativeBridge_PlayCmd_e eNbCmd) /* NativeBridge_PlayCmd_e */
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	NXL_PLAYBACKGROUP_CMD_e eNxlCmd = NXL_PLAYBACKGROUP_CMD_NA;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	if (sNxlSymbol.playbackGroup_informLastPlayCmd)
	{
		switch (eNbCmd) {
		case NativeBridge_PLAYCMD_NA       : eNxlCmd = NXL_PLAYBACKGROUP_CMD_NA; break;
		case NativeBridge_PLAYCMD_OPEN     : eNxlCmd = NXL_PLAYBACKGROUP_CMD_Open; break;
		case NativeBridge_PLAYCMD_STOP     : eNxlCmd = NXL_PLAYBACKGROUP_CMD_Stop; break;
		case NativeBridge_PLAYCMD_START    : eNxlCmd = NXL_PLAYBACKGROUP_CMD_Start; break;
		case NativeBridge_PLAYCMD_PLAY     :  eNxlCmd = NXL_PLAYBACKGROUP_CMD_Play; break;
		case NativeBridge_PLAYCMD_PAUSE    : eNxlCmd = NXL_PLAYBACKGROUP_CMD_Pause; break;
		default:
			eNxlCmd = NXL_PLAYBACKGROUP_CMD_NA; break;
		}
		
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_informLastPlayCmd(eNxlCmd);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Error> Failed to inform play state, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::setDuration(double dblCurrentMaxPts)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(sNxlSymbol.playbackGroup_setDuration)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_setDuration(dblCurrentMaxPts);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to set a duration, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::audioFlush(uint32_t unTryToChangeAudioTrack)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(sNxlSymbol.playbackGroup_audioFlush)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_audioFlush(unTryToChangeAudioTrack);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to flush an audio decoder buffer, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::audioRestart()
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(sNxlSymbol.playbackGroup_audioRestart)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_audioRestart();
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to restart an audio decoder, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::audioCodecSwitch(uint32_t unTryToChangeAudioTrack, int32_t nAudioCodec)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(sNxlSymbol.playbackGroup_audioCodecSwitch)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_audioCodecSwitch(unTryToChangeAudioTrack, nAudioCodec);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to switch an audio codec, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::notifyStreamCurrentPAR(uint32_t w, uint32_t h)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	if(sNxlSymbol.playbackGroup_notifyStreamCurrentPAR)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_notifyStreamCurrentPAR(w, h);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to notifyStreamCurrentPAR(w:%d, h:%d), eRet = %d\033[0m\n", __func__, __LINE__, w, h, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::notifyStreamCurrentSize(uint32_t w, uint32_t h)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	if(sNxlSymbol.playbackGroup_notifyStreamCurrentSize)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_notifyStreamCurrentSize(w, h);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to notifyStreamCurrentSize(w:%d, h:%d), eRet = %d\033[0m\n", __func__, __LINE__, w, h, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::notifyStreamMaxSize(uint32_t w, uint32_t h)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	if(sNxlSymbol.playbackGroup_notifyStreamMaxSize)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_notifyStreamMaxSize(w, h);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to notifyStreamMaxSize(w:%d, h:%d), eRet = %d\033[0m\n", __func__, __LINE__, w, h, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::setVideoSize(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(sNxlSymbol.playbackGroup_setVideoSize)
	{
		printf("\033[1;31m @@@@@ [%s:%d] setVideoSize(x:%d, y:%d, w:%d, h:%d), eRet = %d\033[0m\n", __func__, __LINE__, x, y, w, h, eRet);
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_setVideoSize(x, y, w, h);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to setVideoSize(x:%d, y:%d, w:%d, h:%d), eRet = %d\033[0m\n", __func__, __LINE__, x, y, w, h, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::resetVideoSize()
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(sNxlSymbol.playbackGroup_resetVideoSize)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_resetVideoSize();
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to resetVideoSize\033[0m\n", __func__, __LINE__);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::setPlayerType()
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	if(sNxlSymbol.playbackGroup_setPlayerType)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_setPlayerType();
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to set player type, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::setFade(uint32_t unStartVolume, uint32_t unTargetVolume, int32_t nEase, int32_t nEaseTimeElapsed, int32_t nEaseTransitionFrames)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(sNxlSymbol.playbackGroup_setFade)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_setFade(unStartVolume, unTargetVolume, nEase, nEaseTimeElapsed, nEaseTransitionFrames);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to set Fade, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:	
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}

NativeBridge_ERR_e NativeBridge::PlaybackGroup::getAudioDecoderVolume(uint32_t *punVolume)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	printf("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(punVolume == NULL)
	{
		printf("\033[1;31m @@@@@ [%s:%d] punVolume is NULL\033[0m\n", __func__, __LINE__);
		eRet = NativeBridge_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	if(sNxlSymbol.playbackGroup_getAudioDecoderVolume)
	{
		eRet = (NativeBridge_ERR_e)sNxlSymbol.playbackGroup_getAudioDecoderVolume(punVolume);
		if(eRet != NativeBridge_ERR_OK)
		{
			printf("\033[1;31m @@@@@ [%s:%d] Failed to get audio decoder volume, eRet = %d\033[0m\n", __func__, __LINE__, eRet);
			goto FUNCTION_EXIT;
		}
	}

FUNCTION_EXIT:	
	printf("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return eRet;
}
		
