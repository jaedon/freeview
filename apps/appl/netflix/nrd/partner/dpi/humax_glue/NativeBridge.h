/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NATIVEBRIDGE_H_
#define NATIVEBRIDGE_H_

/** @file NativeBridge.h
 * interface
 *
 */

#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>

// Dynamic Symbol Linking

//#define NXLINK_DYNAMIC 1

#ifdef NXLINK_DYNAMIC
#include <dlfcn.h>
#else
#include <nxlink.h>
#endif

namespace netflix {
namespace device {

typedef enum {
	NativeBridge_ERR_OK = 0,
	NativeBridge_ERR_FAIL,
	NativeBridge_ERR_INVALID_PARAM,
	NativeBridge_ERR_MAX
} NativeBridge_ERR_e;

/** HDCP version
 *
 * It is REQUIRED to report correct HDCP version whenever there is external
 * link is established for the devices that has external digital video
 * output links.
 *
 * For self displaying devices with VideoOutputType "INTERNAL" like DTV,
 * phone, tablet and laptops, return HDCP_NOT_APPLICABLE. In this case, SDK
 * does not perform any downloadable filtering based on HDCP.
 *
 * For self displaying device with digital output, return correct HDCP
 * version engaged in digital output. In this case, SDK will perform
 * downloadable filtering as well as fast playback start decision based on
 * HDCP version.
 *
 * For external streaming devices, return currently engaged HDCP
 * version. Based on engaged HDCP version, and each content's
 * resolution/HDCP mapping, SDK can filter out downloadables
 *
 * When changed, device should notify via event mechanism defined by
 * EventDispather.
 */
typedef enum {
    NativeBridge_HDCP_NOT_APPLICABLE = -1,
    NativeBridge_HDCP_NOT_ENGAGED    =  0, /* HDCP_NOT_ENGAGED */
    NativeBridge_HDCP_1_0            = 10,
    NativeBridge_HDCP_1_1,
    NativeBridge_HDCP_1_2,
    NativeBridge_HDCP_1_3,
    NativeBridge_HDCP_1_4,                 /* HDCP_1_4 in [enum HDCPVersion] */
    NativeBridge_HDCP_2_0            = 20,
    NativeBridge_HDCP_2_1,
    NativeBridge_HDCP_2_2
} NativeBridge_HDCPVersion_e;

typedef enum {
    NativeBridge_PLAYCMD_NA    = 0,
    NativeBridge_PLAYCMD_OPEN,
    NativeBridge_PLAYCMD_STOP,
    NativeBridge_PLAYCMD_START,
    NativeBridge_PLAYCMD_PLAY,
    NativeBridge_PLAYCMD_PAUSE
} NativeBridge_PlayCmd_e;

class NativeBridge
{
public:
	static const NativeBridge & instance();
	static uint32_t init();
	static uint32_t deinit();

	class System; class ESManager; class PlaybackGroup;

	static System *system() { return sSystem_; };
	static ESManager &esManager() { return *sESManager_; };
	static PlaybackGroup &playbackGroup() { return *sPlaybackGroup_; };
public :
	class BridgeObject
	{
	protected:
		BridgeObject() {};
		virtual ~BridgeObject() {};
		BridgeObject(const BridgeObject &); // prevent copy-construction
		BridgeObject & operator=(const BridgeObject &); // prevent assignment
	};

	class System : public BridgeObject
	{
	public:
		std::string getSoftwareVersion();
		std::string getModelName(); /* Registered modelName at NRD portal */
		std::string getOctoModelName(); /* Getting by OCTO */
		std::string getFriendlyName();
		std::string getLanguage();

		uint32_t getVolume();
		uint32_t getVolumeStep();
		bool isMuted();

		uint32_t getOutputDeviceNum();
		bool hasOutputScart();
		bool hasOutputTvScart();
		bool hasOutputVcrScart();
		bool hasOutputComposite();
		bool hasOutputComponent();
		bool hasOutputRF();
		uint32_t hasOutputHDMI(bool *pHdmiSupported, NativeBridge_HDCPVersion_e *pHdcpVer);
		bool hasOutputSpdif();
		uint32_t isActiveOutputHDMI(bool *pHdmiActivated, NativeBridge_HDCPVersion_e *pHdcpVer);
		bool isActiveOutputSpdif();
		uint32_t getHdcpStatus();
		uint32_t setHdcpStatus(uint32_t bOn, uint32_t bForce);
		uint32_t getHdcpVersion();
		uint32_t getCgmsStatus();
		uint32_t setCgmsStatus(eNXL_CopyRight eCgmsStatus);
		uint32_t getEdidDisplayResolution(uint32_t *resx, uint32_t *resy);
		uint32_t getActiveVideoOutput(uint32_t devidx);
		uint32_t getVideoOutputResolution(uint32_t *width, uint32_t *height);
		uint32_t getDigitalAudioOutputMode();
		uint32_t isDigitalAudioMultiChannel();
		uint32_t isSupport1080pVideo();
		uint32_t checkOutputVolumeUpdated();
		uint32_t checkOutputHdmiUpdated();
		uint32_t checkOutputResolutionUpdated();
		bool hasScreenSaver();
		uint32_t isScreensaverOn();
		uint32_t checkScreenSaverUpdated();
		uint32_t checkNetworkEventUpdated();
		bool hasWifiDevice();
		bool hasWifiEth1Bridge();
		uint32_t registerOutputVolumeNotifier(void *pFunc);
		uint32_t unregisterOutputVolumeNotifier(void *pFunc);
		uint32_t registerOutputHdmiNotifier(void *pFunc);
		uint32_t unregisterOutputHdmiNotifier(void *pFunc);
		uint32_t unregisterOutputResolutionNotifier(void *pFunc);
		uint32_t registerOutputResolutionNotifier(void *pFunc);
		uint32_t unregisterScreenSaverNotifier(void *pFunc);
		uint32_t registerScreenSaverNotifier(void *pFunc);

	protected:
		System();
		virtual ~System();
	friend uint32_t NativeBridge::init();
	};

	class ESManager : public BridgeObject
	{
	public:
		NativeBridge_ERR_e getResourceMap(void *pMap);
		NativeBridge_ERR_e getSystemMuteStatus(bool *pMute);
	protected:
		ESManager();
		virtual ~ESManager();
	friend uint32_t NativeBridge::init();
	};

	class PlaybackGroup : public BridgeObject
	{
	public:
		NativeBridge_ERR_e registerNotifier(void *pFunc);
		NativeBridge_ERR_e unregisterNotifier(void *pFunc);
		NativeBridge_ERR_e mediaPbStart(void *pProbeInfo);
		NativeBridge_ERR_e mediaPbStop();
		NativeBridge_ERR_e getNotifyArgs(uint32_t ulViewId, uint32_t ulSessionId, uint32_t eType, uint32_t eEvent, void *eAVCtrlEventType); // 
		NativeBridge_ERR_e setPlaySpeed(int32_t nSpeedSec);
		NativeBridge_ERR_e jump(uint32_t unPlayTime);
		NativeBridge_ERR_e getPumpIndex(uint32_t *punIndex);
		NativeBridge_ERR_e getPlayPosition(uint32_t *punPosition);
		NativeBridge_ERR_e getMediaCtrlPlayState(void *pState);
		NativeBridge_ERR_e getMediaCtrlBufferState(void *pState);
		NativeBridge_ERR_e checkLastPlayCmd(NativeBridge_PlayCmd_e *pCmd);
		NativeBridge_ERR_e informLastPlayCmd(NativeBridge_PlayCmd_e pCmd);
		NativeBridge_ERR_e setDuration(double dblCurrentMaxPts);
		NativeBridge_ERR_e audioFlush(uint32_t unTryToChangeAudioTrack);
		NativeBridge_ERR_e audioRestart();
		NativeBridge_ERR_e audioCodecSwitch(uint32_t unTryToChangeAudioTrack, int32_t nAudioCodec);
		NativeBridge_ERR_e notifyStreamCurrentPAR(uint32_t w, uint32_t h);
		NativeBridge_ERR_e notifyStreamCurrentSize(uint32_t w, uint32_t h);
		NativeBridge_ERR_e notifyStreamMaxSize(uint32_t w, uint32_t h);
		NativeBridge_ERR_e setVideoSize(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
		NativeBridge_ERR_e resetVideoSize();
		NativeBridge_ERR_e setPlayerType();
		NativeBridge_ERR_e setFade(uint32_t unStartVolume, uint32_t unTargetVolume, int32_t nEase, int32_t nEaseTimeElapsed, int32_t nEaseTransitionFrames);
		NativeBridge_ERR_e getAudioDecoderVolume(uint32_t *punVolume);
		
	protected:
		PlaybackGroup();
		virtual ~PlaybackGroup();
	friend uint32_t NativeBridge::init();
	};

protected:

private:
	NativeBridge();
	virtual  ~NativeBridge();
	NativeBridge(const NativeBridge &); // prevent copy-construction
	NativeBridge & operator=(const NativeBridge &); // prevent assignment

	static NativeBridge * sInstance_;
	static System *sSystem_;
	static ESManager *sESManager_;
	static PlaybackGroup *sPlaybackGroup_;
};

}}

#endif /* NATIVEBRIDGE_H_ */
