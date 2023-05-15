/**************************************************************************************
 * @file nxlink_ipc_apkv2.c
 * defines APIs's internal body for appkit_v2
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nxlink.h>
#include "nxlink_int.h"
#include "nxlink_ipc_apkv2.h"
#include <globalconfig.h>

#include <htype.h>
#include <vkernel.h>
#include <hlib.h>
#include <hapi.h>
#include <oapi.h>

#include <apk.h>
/* #include <oapi.h> */
/* #include <dapi.h> */
#include <uapi.h>
#include <papi.h>

/******************************************************************
	Define
******************************************************************/
#undef  NXL_LOG_DBG
#undef  NXL_LOG_WRN
#undef  NXL_LOG_ERR
#undef  NXL_Assert
#define NXL_LOG_DBG                             HxLOG_Print
#define NXL_LOG_WRN                             HxLOG_Warning
#define NXL_LOG_ERR                             HxLOG_Error
#define NXL_Assert(x)                           HxLOG_Assert(x)

//#define CONFIG_TIMESTAMP_64 1

#undef		HxLOG_DOMAIN
#define		HxLOG_DOMAIN				"Netflix"

#define _APKV2_PROC_EVENT_TASK_PRIORITY         (40)
#define _APKV2_PROC_EVENT_TASK_SIZE             (0x4000)
#define _MAX_VIDEO_WINDOW_WIDTH                 (1280)
#define _MAX_VIDEO_WINDOW_HEIGHT                (720)
#define _VIDEO_RESIZE_CHECK_INTERVAL            (1500)
#define _NETWORK_EVENT_CHECK_INTERVAL           (10000)

#if 0
#define	_LOCK_CTX(context)                      HxLOG_Assert(context->sema); HxSEMT_Get(context->sema)
#define	_LOCK_CTX_TIMEOUT(context)              HxSEMT_GetTimeout(context->sema, 10000)
#define	_UNLOCK_CTX(context)                    HxSEMT_Release(context->sema)
#else
#define	_LOCK_CTX(context)                      local_apkv2_lock(context)
#define	_UNLOCK_CTX(context)                    local_apkv2_unlock(context)
#endif

typedef	struct
{
	HUINT32		ulValue;
	HCHAR		*pszValue;
} EnumStringPair_t;

typedef struct
{
	HUINT32  x;
	HUINT32  y;
	HUINT32  width;
	HUINT32  height;
} NXL_APKV2_VideoRect_t;

typedef	struct	_NXL_APKV2_Info_tag
{
	HBOOL              ctxInited;
	HBOOL              appkitInited; /* TODO : Change this with flags for appkit connection/disconnection */
	HULONG             sema;
	HULONG	           tTaskId;
	NXL_TaskStatus_e   taskStatus;
	HBOOL              requestExit;
	HBOOL              hasBoxInfoListener;
	HBOOL              liveStarted;

	HBOOL              gotVolumeInfo;
	HUINT32            volumeCur;
	HUINT32            volumeMax;
	HUINT32            volumeMin;
	HUINT32            volumeMuted;

	/* system event
	 * https://nrd.netflix.com/display/DOC42/System+Capabilities */
	HBOOL                     lastOutputVolumeChanged;     // nrd->outputVolumeChanged
	DxHdcpVersion_e           lastOutputHdcpVersion;
	HBOOL                     lastOutputHdmiEnabled;
	HBOOL                     lastOutputHdmiChanged;       // nrd->videoOutputStatusChanged
	HUINT32                   lastOutputResolutionWidth;
	HUINT32                   lastOutputResolutionHeight;
	DxResolutionSelect_e      lastSelectedResolution;      /* APKE_OutputResolution_e */
	HBOOL                     lastOutputResolutionChanged; // nrd->videoOutputResolutionChanged
	HBOOL                     lastScreensaverChanged;      // nrd->screensaverStateChanged
	HBOOL                     lastViewModeChanged;         // nrd->viewModeChanged
	HBOOL                     lastLanguageChanged;         // nrd->languageChanged
	eNXL_NETWORK_EVENT        lastNetworkEvent;
	HUINT32                   lastNetworkEventTick;
	HBOOL                     lastNetworkChanged;          // nrd->networkChanged

	/* playback and netflix:nrd's videoAttributes */
	HUINT32                   playerSessionId;
	OxMediaPlay_PlayerType_e  playerType;
	NXL_PLAYBACKGROUP_CMD_e   lastPlaybackCmd;
	HUINT32                   streamPAR_X;                 // nrd->PixelAspectRatio X
	HUINT32                   streamPAR_Y;                 // nrd->PixelAspectRatio Y
	HUINT32                   streamCurrentWidth;          // nrd->sampleAttr->VideoAttributes->ImageWidth
	HUINT32                   streamCurrentHeight;         // nrd->sampleAttr->VideoAttributes->ImageHeight
	HUINT32                   streamMaxWidth;              // nrd->PlaybackDevice::initVideoPlayer(VideoAttributes *videoAttr)
	HUINT32                   streamMaxHeight;             // nrd->PlaybackDevice::initVideoPlayer(VideoAttributes *videoAttr)
	NXL_APKV2_VideoRect_t     lastVideoResizeRect;
	HUINT32                   lastVideoResizeTick;

	HBOOL                     screenSaverRunning;
	eNXL_PWR_STATUS	          powerStatus;
	NXL_PowerStateChange_t    pfnPowerChangeCallback;
	OxMediaPlay_ResourceMap_t stResourceMap;
} NXL_APKV2_Context_t;

/******************************************************************
	Static variables
******************************************************************/
static NXL_APKV2_Context_t s_stApkv2Context = {0, };
static HUINT8 tmpbuf[_NXL_BLEN_64] = {0, };

static EnumStringPair_t  _astDigitalOutList[] =
{
	{eDxDIGITALAUDIO_FORMAT_PCM,             "PCM"           },
	{eDxDIGITALAUDIO_FORMAT_MULTICHANNEL,    "MultiChannel"  }, /* Pass through */
	{eDxDIGITALAUDIO_FORMAT_UNKNOWN,         NULL            }
};

static EnumStringPair_t  _astAspectRatioPairList[] =
{
	{eDxASPECTRATIO_4X3,      "4:3"},
	{eDxASPECTRATIO_16X9,     "16:9"},
	{eDxASPECTRATIO_14X9,     "14:9"},
	{eDxASPECTRATIO_UNKNOWN,  NULL}
};

static EnumStringPair_t  _astResolutionSelectPairList[] =
{
	{eDxRESOLUTION_SELECT_480I,       "480i"},
	{eDxRESOLUTION_SELECT_480P,       "480p"},
	{eDxRESOLUTION_SELECT_576I,       "576i"},
	{eDxRESOLUTION_SELECT_576P,       "576p"},
	{eDxRESOLUTION_SELECT_720P,       "720p"},
	{eDxRESOLUTION_SELECT_1080I,      "1080i"},
	{eDxRESOLUTION_SELECT_1080P,      "1080p"},
	{eDxRESOLUTION_SELECT_ORIGINAL,   "Original"},
	{eDxRESOLUTION_SELECT_EDID_MAX,   "Native"},
	{eDxRESOLUTION_SELECT_UNKNOWN,    NULL}
};

static EnumStringPair_t _astVideoStandardPairList[] =
{
	{eDxCOMPOSITE_STANDARD_PAL,       "PAL"},
	{eDxCOMPOSITE_STANDARD_NTSC,      "NTSC"},
	{eDxCOMPOSITE_STANDARD_SECAM,     "SECAM"},
	{eDxCOMPOSITE_STANDARD_UNKNOWN,   "Unknown"},
	{eDxCOMPOSITE_STANDARD_UNKNOWN,    NULL}
};

uint32_t nxl_apkv2_sys_getModelName(char *pBuf, uint32_t size);
uint32_t nxl_apkv2_sys_getSoftwareVersion(char *pBuf, uint32_t size);
uint32_t nxl_apkv2_sys_getSystemID(char *pszID);
uint32_t nxl_apkv2_sys_getScreenLanguage(char *pszLanguage, uint32_t size);
uint32_t nxl_apkv2_output_getOutputResolution(uint32_t *pWidth, uint32_t *pHeight);
static HCHAR* local_apkv2_getStringFromValue(EnumStringPair_t *pstPairList, const HUINT32 ulValue);
static HUINT32 local_apkv2_getValueFromString(EnumStringPair_t *pstPairList, const HCHAR *pszData);


/******************************************************************
	Static functions
******************************************************************/
static NXL_APKV2_Context_t *__getContext(void)
{
	return &s_stApkv2Context;
}

static void local_apkv2_lock(NXL_APKV2_Context_t *context)
{
	NXL_Assert(context);
#if 0
	VK_SEM_Get(context->sema);
#else
	if (VK_SEM_GetTimeout(context->sema, 20000) != VK_OK)
	{
		NXL_LOG_ERR("Error> SEM_GetTimeout() failed - ID(0x%X)\n", (HINT32)context->sema);
		return;
	}
#endif
}

static void local_apkv2_unlock(NXL_APKV2_Context_t *context)
{
	NXL_Assert(context);
	VK_SEM_Release(context->sema);
}

static void local_apkv2_outputAudioEventCallback(void)
{
	NXL_APKV2_Context_t *context = __getContext();
	HBOOL bMute = FALSE;
	HUINT32 nVolume = 0;

	_NOT_USED_PARAM(nVolume);

	NXL_LOG_DBG("++\n");
	NXL_LOG_DBG("AudioEvent\n");
	if (APK_OUTPUT_AUDIO_GetMasterMute(&bMute) == ERR_OK)
	{
		_LOCK_CTX(context);
		context->volumeMuted = bMute;
		context->lastOutputVolumeChanged = 1;
		_UNLOCK_CTX(context);
	}
	NXL_LOG_DBG("--\n");
}

uint32_t nxl_apkv2_output_checkLastOutputVolumeChanged(uint32_t *pUpdated)
{
	NXL_APKV2_Context_t *context = __getContext();

	_LOCK_CTX(context);
	*pUpdated = context->lastOutputVolumeChanged;
	context->lastOutputVolumeChanged = 0;
	_UNLOCK_CTX(context);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_registerOutputVolumeListener(void *pFunc)
{
	/* APK_OUTPUT_AUDIO_Init(); */
	APK_OUTPUT_AUDIO_RegisterNotifier(pFunc);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_unregisterOutputVolumeListener(void *pFunc)
{
	APK_OUTPUT_AUDIO_UnregisterNotifier();
	return NXL_ERR_OK;
}

static void local_apkv2_outputHdmiEventCallback(void)
{
	NXL_APKV2_Context_t *context = __getContext();
	HBOOL bHdmiEnabled = FALSE;
	NXL_LOG_DBG("++\n");

	if (APK_OUTPUT_HDMI_IsPlugIn(&bHdmiEnabled) != ERR_OK)
	{
		NXL_LOG_ERR("Error> IsPlugIn() failed\n");
		return;
	}
	_LOCK_CTX(context);
	NXL_LOG_DBG("output event -- HDMI enabled(%d, %d)\n", bHdmiEnabled, context->lastOutputHdmiEnabled);
	context->lastOutputHdmiEnabled = bHdmiEnabled;
	context->lastOutputHdmiChanged = 1;
	_UNLOCK_CTX(context);
	NXL_LOG_DBG("--\n");
}

uint32_t nxl_apkv2_output_checkLastOutputHdmiChanged(uint32_t *pUpdated)
{
	NXL_APKV2_Context_t *context = __getContext();
	HBOOL bHdmiEnabled = FALSE;
	DxHdcpVersion_e eDxHdcpVer = eDxHDCP_VERSION_MAX;

	*pUpdated = FALSE;
	_LOCK_CTX(context);
	if (APK_OUTPUT_HDMI_IsPlugIn(&bHdmiEnabled) == ERR_OK)
	{
		/* NXL_LOG_DBG("[checkLastOutputHdmiChanged] HDMI(%d -> %d)\n", context->lastOutputHdmiEnabled, bHdmiEnabled); */
		if (bHdmiEnabled != context->lastOutputHdmiEnabled)
		{
			NXL_LOG_ERR("[checkLastOutputHdmiChanged] Changed HDMI(%d -> %d)\n", context->lastOutputHdmiEnabled, bHdmiEnabled);
			context->lastOutputHdmiEnabled = bHdmiEnabled;
			_UNLOCK_CTX(context);
			*pUpdated = TRUE;
			return NXL_ERR_OK;
		}
	}

	if (APK_OUTPUT_HDMI_GetHdcpVersion(&eDxHdcpVer) == ERR_OK)
	{
		/* NXL_LOG_DBG("[checkLastOutputHdmiChanged] HDCP ver(%d -> %d)\n", context->lastOutputHdcpVersion,  eDxHdcpVer); */
		if (eDxHdcpVer != context->lastOutputHdcpVersion)
		{
			NXL_LOG_ERR("[checkLastOutputHdmiChanged] Changed HDCP ver(%d -> %d)\n", context->lastOutputHdcpVersion,  eDxHdcpVer);
			context->lastOutputHdcpVersion = eDxHdcpVer;
			_UNLOCK_CTX(context);
			*pUpdated = TRUE;
			return NXL_ERR_OK;
		}
	}

	/* NXL_LOG_DBG("[checkLastOutputHdmiChanged] Check HDMI updated(%d)\n", *pUpdated); */
	context->lastOutputHdmiChanged = FALSE;
	_UNLOCK_CTX(context);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_registerOutputHdmiListener(void *pFunc)
{
	APK_OUTPUT_HDMI_RegisterNotifier(pFunc);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_unregisterOutputHdmiListener(void *pFunc)
{
	APK_OUTPUT_HDMI_UnregisterNotifier();
	return NXL_ERR_OK;
}

/**
 * @fn static void	local_apkv2_networkEventCallback( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
 * @brief APK_SQCNetwork에 등록된 이벤트 Callback 함수.
 */
static void local_apkv2_networkEventCallback(HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	NXL_APKV2_Context_t *context = __getContext();
	HINT32 devIdx = (HINT32)ulParam1;
	eNXL_NETWORK_EVENT eNetworkEvent = NXL_NETWORK_EVENT_NONE;
	NXL_LOG_DBG("Nima[event:%d]\n", eEvent);

	switch ( eEvent )
	{
		case eNETWORK_STATUS_CON_SUCCESS :
		case eNETWORK_STATUS_CON_AUTOIP_SUCCESS :
		case eNETWORK_STATUS_WIFI_WPS_SUCCESS :
			eNetworkEvent = NXL_NETWORK_EVENT_ONLINE;
			break;
		case eNETWORK_STATUS_CABLE_LINK_DISCONNECTED :
		case eNETWORK_STATUS_WIFI_USB_EXTRACTED :
			eNetworkEvent = NXL_NETWORK_EVENT_OFFLINE;
			break;

		// CON_FAIL 와 CON_AUTOIP_FAIL이 반드시 OFF Line은 아니다.
		// 특정 NIC가 연결이 안되는 경우에, 연결이 가능한 NIC가 있을 수 있다.
		// IP는 갱신할 필요성 있어서 처리한다. ON/OFF Line 판정은 기존 코드를 유지한다.
		case eNETWORK_STATUS_CON_FAIL :
		case eNETWORK_STATUS_CON_AUTOIP_FAIL :
			eNetworkEvent = NXL_NETWORK_EVENT_NONE;
			break;

		/* 다른 이벤트는 필요 없음. */
		case eNETWORK_STATUS_CABLE_LINK_CONNECTED :
		case eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS :
		case eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL :
		case eNETWORK_STATUS_WIFI_WPS_FAIL :
		case eNETWORK_STATUS_LINK_UPDATED:
		default :
			return;
	}

	NXL_LOG_ERR("received network event(idx:%d, evt:%d,%d)\n", devIdx, eEvent, eNetworkEvent);

	if (NXL_NETWORK_EVENT_NONE != eNetworkEvent)
	{
		_LOCK_CTX(context);
		context->lastNetworkEvent |= eNetworkEvent;
		context->lastNetworkEventTick = HLIB_STD_GetSystemTick();
		_UNLOCK_CTX(context);
	}

#if 0
	/* mxDlna_NIC_change() must be used as an auxiliary for mis-catching IP change(). */
	if ((eEvent == eNETWORK_STATUS_CON_SUCCESS) ||
		/* (eEvent == eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS) ||
		(eEvent == eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL) ||
		(eEvent == eNETWORK_STATUS_WIFI_AP_DISCONNECTED) || */
		(eEvent == eNETWORK_STATUS_CON_FAIL))
	{
		;
	}
#endif
	return;
}

uint32_t nxl_apkv2_sys_checkNetworkChanged(uint32_t *pUpdated)
{
	HINT32 uTickDiff = 0x0;
	NXL_APKV2_Context_t *context = __getContext();

	*pUpdated = 0;

	_LOCK_CTX(context);
	if ((NXL_NETWORK_EVENT_NONE != context->lastNetworkEvent) && (context->lastNetworkEventTick))
	{
		uTickDiff = (HLIB_STD_GetSystemTick() - context->lastNetworkEventTick);
		if ((uTickDiff > 0x0) && (uTickDiff < _NETWORK_EVENT_CHECK_INTERVAL))
		{
			*pUpdated = TRUE;
		}
		NXL_LOG_ERR("[checkNetworkChanged] -- [updated:%d, evt:%d, changed:%d -- tick[diff:%d, last:%d]\n", *pUpdated, context->lastNetworkEvent, context->lastNetworkChanged, uTickDiff, context->lastNetworkEventTick);
		context->lastNetworkEvent = NXL_NETWORK_EVENT_NONE;
		context->lastNetworkChanged = 0x0;
	}

	_UNLOCK_CTX(context);
	return NXL_ERR_OK;
}

#if defined(__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__)
static HBOOL local_apkv2_mediapbEventFilter(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated)
{
	*updated = FALSE;

	if (eOxMP_MEDIATYPE_MEDIAPLAY == eType)
	{
		*updated = TRUE;
	}

	/* unavailable or unnecessary event */
	return TRUE;
}
#endif

static void local_apkv2_mediapbEventCallback( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
	OxMediaPlay_MediaType_e	eMediaType = eOxMP_MEDIATYPE_NONE;
#if defined(__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__)
	HBOOL availableEvent = TRUE;
#endif

	if (0) /* Commented to prevent unnecessary OAPI IPC consumption because eMediaType is unused */
	{
		APK_MEDIA_PLAY_GetPlayType (ulViewId, &eMediaType);
	}

#if defined(__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__)
	(void)local_apkv2_mediapbEventFilter(ulViewId, eType, eEvent, &availableEvent);
	if (availableEvent == FALSE)
	{
		/* unavailable or unnecessary event */
		return;
	}
#endif

	switch (eEvent)
	{
		case eOxMP_EVENT_PLAYTIME:
			{
#if defined(CONFIG_TIMESTAMP_64)
				HUINT64 ulPosition = 0, ulDuration = 0;
#else
				HUINT32 ulPosition = 0, ulDuration = 0;
#endif
				APK_MEDIA_PLAY_GetPlayPosition (ulViewId, &ulPosition);
				APK_MEDIA_PLAY_GetDurationTime (ulViewId, &ulDuration);

				HxLOG_Debug("### MEDIA PLAYING TIME CHANGED (%d/%d)  ### \n", ulPosition, ulDuration);
			}
			break;

		case eOxMP_EVENT_STOPPED:
			{
				HxLOG_Debug("eOxMP_EVENT_STOPPED\n");
			}
			break;

		case  eOxMP_EVENT_PLAYSTATE:
			{
				OxMediaPlay_PlayState_e eState = eOxMP_PLAYSTATE_NONE;
				APK_MEDIA_PLAY_GetPlayState (ulViewId, &eState);
				HxLOG_Debug("eOxMP_EVENT_PLAYSTATE (%d)\n", eState);
				switch(eState)
				{
					case eOxMP_PLAYSTATE_PAUSE:
						{
							HxLOG_Debug("eOxMP_PLAYSTATE_PAUSE (%d)\n", eState);
						}
						break;

					case eOxMP_PLAYSTATE_STOPPING:
						HxLOG_Debug("eOxMP_PLAYSTATE_STOPPING (%d)\n", eState);
						break;

					case eOxMP_PLAYSTATE_STOPPED:
						{
							HxLOG_Debug("eOxMP_PLAYSTATE_STOPPED (%d)\n", eState);
						}
						break;

					case eOxMP_PLAYSTATE_REQUESTED:
						HxLOG_Debug("eOxMP_PLAYSTATE_REQUESTED (%d)\n", eState);
						break;

					case eOxMP_PLAYSTATE_CONNECTING:
						HxLOG_Debug("eOxMP_PLAYSTATE_CONNECTING(%d)\n", eState);
						break;

					case eOxMP_PLAYSTATE_BUFFERING:
						HxLOG_Debug("eOxMP_PLAYSTATE_BUFFERING(%d)\n", eState);
						break;

					case eOxMP_PLAYSTATE_PLAYING:
						{
							HxLOG_Debug("eOxMP_PLAYSTATE_PLAYING(%d)\n", eState);
						}
						break;

					default:
						break;
				}
			}
			break;

		default :
			{
				//HxLOG_Debug("Not need event. (%d)\n", eEvent );
				;
			}
		   	return;
	}
}

static void local_v2_processEventTask(void *args)
{
	NXL_TaskStatus_e prevStatus = eNXL_TASK_STATUS_TERMINATED;
	NXL_APKV2_Context_t *context = __getContext();

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	_LOCK_CTX(context);

	if (context->taskStatus < eNXL_TASK_STATUS_RUNNING)
	{
		context->taskStatus = eNXL_TASK_STATUS_RUNNING;
	}
	prevStatus = context->taskStatus;

	NXL_LOG_DBG("parkjh4 : Start dispatch!!!\n");
	while(1)
	{
		if ((context->taskStatus == eNXL_TASK_STATUS_TERMINATED) ||
			(context->taskStatus == eNXL_TASK_STATUS_HALTING))
		{
			prevStatus = context->taskStatus;
			NXL_LOG_ERR("@@@@@ task received halt event(status : %d)\n", context->taskStatus);
			_UNLOCK_CTX(context);
			VK_TASK_Sleep(20);
			_LOCK_CTX(context);
			goto func_exit;
		}

		if (context->requestExit == TRUE)
		{
			prevStatus = context->taskStatus;
			context->taskStatus = eNXL_TASK_STATUS_TERMINATED;
			NXL_LOG_ERR("@@@@@ task loop status changed(%d -> %d)\n", prevStatus, context->taskStatus);
		}

		_UNLOCK_CTX(context);
		APK_EVENT_Dispatch();
		VK_TASK_Sleep(10);
		_LOCK_CTX(context);
	}
	NXL_LOG_ERR("@@@@@ task loop terminated(%d -> %d)\n", prevStatus, context->taskStatus);

func_exit:
	if (context->taskStatus != eNXL_TASK_STATUS_TERMINATED)
	{
		NXL_LOG_DBG("@@@@@ task status changed(%d -> %d)\n", prevStatus, context->taskStatus);
		context->taskStatus = eNXL_TASK_STATUS_TERMINATED;
	}

	_UNLOCK_CTX(context);
	NXL_LOG_DBG("parkjh4 : Exit from dispatch!!!\n");
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return;
}

static HERROR local_apkv2_hapiScreenSaverStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HBOOL screenSaverRunning = FALSE;
	NXL_APKV2_Context_t *context = __getContext();

	if (nArgc == 1)
	{
		screenSaverRunning = HxOBJECT_INT(apArgv[0]);
		_LOCK_CTX(context);
		NXL_LOG_ERR("screenSaverStatus[%d -> %d]\n", context->screenSaverRunning, screenSaverRunning);
		context->screenSaverRunning = screenSaverRunning;
		_UNLOCK_CTX(context);
		/* local_apkv2_systemChangedCallback(systemStatus); */
	}
	return ERR_OK;
}

static HERROR local_apkv2_hapiNetflixStartApp(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	if( nArgc < 1 )
	{
		return ERR_FAIL;
	}
	return ERR_OK;
}

static HERROR local_apkv2_hapiNetflixStopApp(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	if( nArgc < 1 )
	{
		return ERR_FAIL;
	}
	return ERR_OK;
}

static HERROR local_apkv2_hapiNetflixStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	if (nArgc < 1)
	{
		return ERR_FAIL;
	}

#if 0
#if defined(CONFIG_NETFLIX_APP)
	NFLIX_NotifyState_t appState = (NFLIX_NotifyState_t)status;

	if (!statusChangedCallback)
	{
		return;
	}

	switch (appState) {
	case NFLIX_NOTIFY_EXIT_NORMAL  : /* processed between webapp and netflixManager */
	case NFLIX_NOTIFY_START_NORMAL :
	case NFLIX_NOTIFY_EXIT_STANDBY :
	case NFLIX_NOTIFY_EXIT_CH_UP   :
	case NFLIX_NOTIFY_EXIT_CH_DOWN :
	case NFLIX_NOTIFY_EXIT_DIAL    :
	case NFLIX_NOTIFY_EXIT_DIAL_OTHER_LAUNCH :
		statusChangedCallback(status);
		break;
	case NFLIX_NOTIFY_TRYTO_EXIT : /* processed between netflixManager and netflix, except webapp */
	default :
		break;
	}
#endif
#endif
	return ERR_OK;
}

static HBOOL local_apkv2_initContext(NXL_APKV2_Context_t *context)
{
	if (context == NULL)
	{
		NXL_LOG_ERR("Error> invalid NULL arg!\n");
		return FALSE;
	}

	if (context->ctxInited == TRUE)
	{
		NXL_LOG_ERR("Error> Already initialized!\n");
		return FALSE;
	}

	HxSTD_MemSet(&s_stApkv2Context, 0, sizeof(NXL_APKV2_Context_t));
	context->requestExit                 = FALSE;
	context->tTaskId                     = 0x0;
	context->taskStatus                  = eNXL_TASK_STATUS_TERMINATED;
	(void)VK_SEM_Create(&(context->sema), "nxl_sema", VK_SUSPENDTYPE_FIFO);

	/* Initialize stream videoAttributes */
	context->playerSessionId             = 0x0;
	context->playerType                  = eOxMP_MEDIAPLAY_PLAYERTYPE_MSE; /* or eOxMP_MEDIAPLAY_PLAYERTYPE_MSE_NETFLIX */
	context->lastPlaybackCmd             = NXL_PLAYBACKGROUP_CMD_NA;
	context->streamPAR_X                 = context->streamPAR_Y = 0;
	context->streamCurrentWidth          = context->streamCurrentHeight = 0;
	context->streamMaxWidth              = context->streamMaxHeight = 0;
	context->lastVideoResizeTick         = 0x0;
	HxSTD_MemSet(&context->lastVideoResizeRect, 0x0, sizeof(NXL_APKV2_VideoRect_t));

	context->lastOutputVolumeChanged     = FALSE; /* APK_OUTPUT_AUDIO_RegisterNotifier */
	context->lastOutputHdcpVersion       = eDxHDCP_VERSION_MAX;
	context->lastOutputHdmiEnabled       = FALSE;
	context->lastOutputHdmiChanged       = FALSE;
	context->lastOutputResolutionWidth   = 0;
	context->lastOutputResolutionHeight  = 0;
	context->lastSelectedResolution      = eDxRESOLUTION_SELECT_UNKNOWN; /* TODO : */
	context->lastOutputResolutionChanged = FALSE; /* TODO : */
	context->lastScreensaverChanged      = FALSE; /* TODO : Changed with product profile; no screensaver */
	context->lastViewModeChanged         = FALSE; /* TODO : Changed with product profile; Unused because of being fixed with 2D (no 3D model) */
	context->lastLanguageChanged         = FALSE; /* TODO : in developing */
	context->lastNetworkEvent            = NXL_NETWORK_EVENT_NONE;
	context->lastNetworkEventTick        = 0x0;
	context->lastNetworkChanged          = FALSE; /* TODO : currently use partner/dpi network timer for eth0 and wlan0 */
	context->screenSaverRunning          = FALSE;
	context->powerStatus                 = NXL_PWR_STATUS_UNKNOWN;
	context->pfnPowerChangeCallback      = NULL;

	context->ctxInited = TRUE;
	return TRUE;
}

/******************************************************************
	Interfaces
******************************************************************/
uint32_t nxl_apkv2_init(NXL_BoxInfo_t *pstBoxInfo) /* TODO: Check mem leakage */
{
	HUINT32 ulViewId = 0x0;
	HERROR herr = ERR_OK;
	NXL_ERR nxerr = NXL_ERR_OK;
	NXL_APKV2_Context_t *context = NULL;
	HINT32 nVkErr = VK_OK;

	NXL_LOG_ERR("++\n");

	if (pstBoxInfo == NULL)
	{
		NXL_LOG_ERR("Error> NULL arg!\n");
		return NXL_ERR_FAIL;
	}
	else
	{
		HxSTD_MemSet(pstBoxInfo, 0x0, sizeof(NXL_BoxInfo_t));
		pstBoxInfo->model_name = NULL;
		pstBoxInfo->software_version = NULL;
		pstBoxInfo->screen_language = NULL;
		pstBoxInfo->system_id = NULL;
	}

	context = __getContext();
	if (local_apkv2_initContext(context) != TRUE)
	{
		NXL_LOG_ERR("Error> Cant initialize the context!\n");
		return NXL_ERR_FAIL;
	}

	VK_Init();

	HxLOG_Trace();

	HAPI_Init();
	(void)APK_EVENT_Init();

	_LOCK_CTX(context);

	/* Create the thread to receive events coming from APPKIT layer */
	context->taskStatus = eNXL_TASK_STATUS_READY;
	nVkErr = VK_TASK_Create(local_v2_processEventTask, _APKV2_PROC_EVENT_TASK_PRIORITY, _APKV2_PROC_EVENT_TASK_SIZE,
		"local_v2_processEventTask", NULL, (HULONG *)&context->tTaskId, 1);
	if (nVkErr != VK_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Failed to create local_v2_processEventTask!, nVkErr = %d\033[0m\n", nVkErr);
		_UNLOCK_CTX(context);
		return NXL_ERR_FAIL;
	}
	nVkErr = VK_TASK_Start(context->tTaskId);
	if (nVkErr != VK_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Failed to start local_v2_processEventTask!, nVkErr = %d\033[0m\n", nVkErr);
		_UNLOCK_CTX(context);
		return NXL_ERR_FAIL;
	}

	/* APK_XXX_Init() 이후 EVENT_Dispatch() 처리 위해서 unlock() */
	_UNLOCK_CTX(context);

	APK_SYSINFO_STB_Init();
	APK_SCENARIO_Init();

	APK_NETWORK_Init();
	APK_NETWORK_RegisterNotifier( local_apkv2_networkEventCallback );

	herr = APK_MEDIA_PLAY_Init();
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_MEDIA_PLAY_Init() failed\n");
		_UNLOCK_CTX(context);
		return NXL_ERR_FAIL;
	}
	herr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	/* System Volume, Output(HDMI, SPDIF, ...) ... */
	/* Register Volume, HDMI, ... */
	APK_OUTPUT_AUDIO_Init();
	APK_OUTPUT_AUDIO_RegisterNotifier(local_apkv2_outputAudioEventCallback);

	/* Register Volume, HDMI, ... */
	APK_OUTPUT_VIDEO_Init();
	/* APK_OUTPUT_VIDEO_RegisterNotifier(local_apkv2_outputVideoEventCallback); */

	APK_OUTPUT_HDMI_Init();
	APK_OUTPUT_HDMI_RegisterNotifier(local_apkv2_outputHdmiEventCallback);

	APK_DIAL_Init();

#if defined(CONFIG_PROD_FVP4000T)    
	APK_DLNA_COMMON_RegisterNotifier(NULL);
#endif

	HAPI_ConnectSignal("signal:onScreenSaverStateChanged", NULL, local_apkv2_hapiScreenSaverStateChanged);
	/* HAPI_ConnectSignal("signal:onSystemStateChanged",  NULL, local_apkv2_hapiPowerStateChanged); */
	/* HAPI_ConnectSignal("signal:onNetworkConnected", NULL, local_apkv2_hapiNetworkChanged); */
	/* HAPI_ConnectSignal("signal:onStartNetflixAppl", NULL, local_apkv2_hapiNetflixStartApp); */
	/* HAPI_ConnectSignal("signal:onStopNetflixAppl",  NULL, local_apkv2_hapiNetflixStopApp); */
	/* HAPI_ConnectSignal("signal:onNetflixStateChanged", NULL, local_apkv2_hapiNetflixStateChanged); */

	if (1)
	{
		/* Fill pstBoxInfo */
		HxSTD_MemSet(tmpbuf, 0, sizeof(tmpbuf));

		/* Update system/stbinfo */
		nxerr = nxl_apkv2_sys_getModelName(tmpbuf, _NXL_BLEN_64);
		if (nxerr == NXL_ERR_OK)
		{
			if ( pstBoxInfo->model_name ) free(pstBoxInfo->model_name);
			pstBoxInfo->model_name = strdup(tmpbuf);
			NXL_LOG_DBG("[model_name:%s]\n", tmpbuf);
		}
		else
		{
			pstBoxInfo->model_name = NULL;
		}

		nxerr = nxl_apkv2_sys_getSoftwareVersion(tmpbuf, _NXL_BLEN_64);
		if (nxerr == NXL_ERR_OK)
		{
			if ( pstBoxInfo->software_version ) free(pstBoxInfo->software_version);
			pstBoxInfo->software_version = strdup(tmpbuf+7);
			NXL_LOG_DBG("[software_version:%s]\n", tmpbuf);
		}
		else
		{
			pstBoxInfo->software_version = NULL;
		}

		nxerr = nxl_apkv2_sys_getScreenLanguage(tmpbuf, _NXL_BLEN_64);
		if (nxerr == NXL_ERR_OK)
		{
			if ( pstBoxInfo->screen_language ) free(pstBoxInfo->screen_language);
			pstBoxInfo->screen_language = strdup(tmpbuf);
			NXL_LOG_DBG("[screen_language:%s]\n", tmpbuf);
		}
		else
		{
			pstBoxInfo->screen_language = NULL;
		}

		nxerr = nxl_apkv2_sys_getSystemID(tmpbuf);
		if (nxerr == NXL_ERR_OK)
		{
			if ( pstBoxInfo->system_id ) free(pstBoxInfo->system_id);
			pstBoxInfo->system_id = strdup(tmpbuf);
			NXL_LOG_DBG("[system_id:%s]\n", tmpbuf);
		}
		else
		{
			pstBoxInfo->system_id = NULL;
		}
	}

	if (1)
	{
		/* Update video */
#if 0
		if (1) context->lastSelectedResolution = APK_DB_GetInt((HUINT8 *)USERDB_VIDEO_RESOLUTION);
		else APK_OUTPUT_VIDEO_GetResolutionSelect(&context->lastSelectedResolution);
#else
		APK_OUTPUT_VIDEO_GetResolutionSelect(&context->lastSelectedResolution);
#endif
	}

	if (1)
	{
		/* Update audio */
		context->gotVolumeInfo = TRUE;
		if (APK_OUTPUT_AUDIO_GetVolume(&context->volumeCur) != ERR_OK)
		{
			NXL_LOG_ERR("Getting the volume level is failed.\n");
			context->gotVolumeInfo = FALSE;
		}
		if (APK_OUTPUT_AUDIO_GetVolumeMax(&context->volumeMax) != ERR_OK)
		{
			NXL_LOG_ERR("Getting the volume max is failed.\n");
			context->gotVolumeInfo = FALSE;
		}
#if 0
		if (APK_OUTPUT_AUDIO_GetVolumeMin(&context->volumeMin) != ERR_OK)
#else
		context->volumeMin = 0;
#endif

		if (APK_OUTPUT_AUDIO_GetMasterMute(&context->volumeMuted) != ERR_OK)
		{
			NXL_LOG_ERR("Getting the volume mute is failed.\n");
			context->gotVolumeInfo = FALSE;
		}
	}

	context->appkitInited = TRUE;
	_UNLOCK_CTX(context);
	NXL_LOG_ERR("--\n");
	return NXL_ERR_OK;
}

void nxl_apkv2_deinit(void)
{
	NXL_APKV2_Context_t *context = __getContext();
	HUINT32 uLoopCnt = 0, uWaitCnt = 0;
	HINT32 nVkErr = VK_OK;

	NXL_LOG_ERR("++\n");

	_LOCK_CTX(context);

	APK_OUTPUT_AUDIO_UnregisterNotifier();
//#if defined(CONFIG_PROD_FVP4000T)
//	APK_OUTPUT_AUDIO_DeInit();
//#endif

	APK_OUTPUT_HDMI_UnregisterNotifier();
//#if defined(CONFIG_PROD_FVP4000T)
//	APK_OUTPUT_HDMI_DeInit();
//#endif

	/* APK_OUTPUT_VIDEO_UnregisterNotifier(); */
//#if defined(CONFIG_PROD_FVP4000T)
//	APK_OUTPUT_VIDEO_DeInit();
//#endif

//#if defined(CONFIG_PROD_FVP4000T)
//	APK_SYSINFO_STB_DeInit();
//#endif

//#if defined(CONFIG_PROD_FVP4000T)
//	APK_SCENARIO_DeInit();
//#endif

	APK_DIAL_DeInit();

	APK_NETWORK_UnRegisterNotifier(local_apkv2_networkEventCallback);
//  Commented NAPI_DeInit by concerns due to asynchronous behavior of napi_agent_stop/VK_TASK_Destroy().
//	APK_NETWORK_DeInit();
	HAPI_DisconnectSignal("signal:onScreenSaverStateChanged", local_apkv2_hapiScreenSaverStateChanged);
	context->appkitInited = FALSE;
	context->requestExit = TRUE;

	/* Stop event task itself */
	uLoopCnt = 0;
	uWaitCnt = 10;
	if (context->taskStatus > eNXL_TASK_STATUS_HALTING)
	{
		/* Wait for stopping event task */
		while(1)
		{
			if (context->taskStatus == eNXL_TASK_STATUS_TERMINATED)
			{
				NXL_LOG_ERR("task terminated(status:%d)\n", context->taskStatus);
				_UNLOCK_CTX(context);
				VK_TASK_Sleep(20);
				_LOCK_CTX(context);
				break;
			}

			if (uLoopCnt > uWaitCnt)
			{
				NXL_LOG_ERR("Too many looping(%d) for task termination(status:%d)\n", uLoopCnt, context->taskStatus);
				break;
			}

			_UNLOCK_CTX(context);
			VK_TASK_Sleep(10);
			uLoopCnt++;
			_LOCK_CTX(context);
		}
	}

	// Dont concern about asynchronous behavior of VK_TASK_Destroy()
	// because local_v2_processEventTask() will be already self-destroyed.
	nVkErr = VK_TASK_Destroy(context->tTaskId);
	if (nVkErr != VK_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to destroy VK_TASK : TaskId = %lu, nVkErr = %d\033[0m\n", context->tTaskId, nVkErr);
	}

	/* Initialize stream videoAttributes */
	context->lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_NA;
	context->streamPAR_X = context->streamPAR_Y = 0;
	context->streamCurrentWidth = context->streamCurrentHeight = 0;
	context->streamMaxWidth = context->streamMaxHeight = 0;
	context->ctxInited = FALSE;
	_UNLOCK_CTX(context);

	(void)VK_SEM_Destroy(context->sema);
	context->sema = 0x0;
	NXL_LOG_ERR("--\n");
}

uint32_t nxl_apkv2_sys_getApplicationID(uint32_t *pAppId)
{
	__CHECK_PARAM_PTR(pAppId);
	*pAppId = (uint32_t)eAWINDOW_NETFLIX;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_sys_setInputGrap(uint32_t bOn)
{
	NXL_LOG_DBG("On(%s)\n", bOn ? "TRUE" : "FALSE");
#if 0 /* In HDR-1000S, HAPI KeyGrab() cause the change of HWStyle and key froze-up for netflix relaunch */
	HAPI_SendKeyGrab(eAWINDOW_NETFLIX, (bOn ? TRUE : FALSE));
#endif
	return NXL_ERR_OK;
}

/* System */
uint32_t nxl_apkv2_sys_getModelName(char *pBuf, uint32_t size)
{
	__CHECK_PARAM_PTR(pBuf);

	HxSTD_MemSet(pBuf, 0, sizeof(HCHAR)*size);
	APK_SYSINFO_STB_GetModelName(pBuf, size);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_sys_getSoftwareVersion(char *pBuf, uint32_t size)
{
	__CHECK_PARAM_PTR(pBuf);

	HxSTD_MemSet(pBuf, 0, sizeof(HCHAR)*size);
	APK_SYSINFO_STB_GetSoftwareVersion(pBuf, size);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_sys_getLoaderVersion(char *pszLoaderVer, uint32_t size)
{
	__CHECK_PARAM_PTR(pszLoaderVer);

	HxSTD_MemSet(pszLoaderVer, 0, sizeof(HCHAR)*size);
	APK_SYSINFO_STB_GetLoaderVersion(pszLoaderVer, size);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_sys_getSystemID(char *pszID)
{
	__CHECK_PARAM_PTR(pszID);

	if (APK_SYSINFO_STB_GetSystemId(pszID, 32) != ERR_OK)
	{
		NXL_LOG_ERR("Error> Cant get the systemID!\n");
		return NXL_ERR_FAIL;
	}
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_sys_getScreenLanguage(char *pszLanguage, uint32_t size)
{
	__CHECK_PARAM_PTR(pszLanguage);
	/* APK_SCENARIO_GetMainLanguage(pszLanguage); */
	if (APK_SCENARIO_GetSubtitleLanguage(0, pszLanguage) != ERR_OK)
	{
		NXL_LOG_ERR("Error> Cant get the mainLanguage!\n");
		return NXL_ERR_FAIL;
	}
	pszLanguage[3] = '\0';
	return NXL_ERR_OK;
}

#define	USERDB_SUBTITLE_FONT						"UDB/SUBTITLE_Font"

uint32_t nxl_apkv2_sys_getSubtitleFontType(uint32_t *pnSubtitleFontType)
{
	__CHECK_PARAM_PTR(pnSubtitleFontType);

	APK_DB_GetInt(USERDB_SUBTITLE_FONT, (HINT32*)pnSubtitleFontType);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_sys_getPreferredAudioLanuage(char *pszLanguage)
{
	__CHECK_PARAM_PTR(pszLanguage);

	APK_SCENARIO_GetAudioLanguage(0, pszLanguage);
	pszLanguage[3] = '\0';
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_sys_getPreferredSubtitleLanuage(char *pszLanguage)
{
	__CHECK_PARAM_PTR(pszLanguage);

	APK_SCENARIO_GetSubtitleLanguage(0, pszLanguage);
	pszLanguage[3] = '\0';
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_sys_getPreferredMenuLanuage(char *pszLanguage)
{
	__CHECK_PARAM_PTR(pszLanguage);

	APK_SCENARIO_GetMainLanguage(pszLanguage);
	pszLanguage[3] = '\0';
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_sys_getDialFriendlyName(char *pBuf, uint32_t size)
{
	HERROR herr = ERR_OK;
	HBOOL bDialActivated = FALSE;
	HCHAR szDefaultFriendlyName[128] = {0, };
#if defined(CONFIG_PRODUCT_NAME)
	const char *productName = CONFIG_PRODUCT_NAME;
#else
	const char *productName = "UNKNOWN";
#endif

	__CHECK_PARAM_PTR(pBuf);

	herr = APK_DIAL_GetDialStatus(&bDialActivated);
	if ((herr != ERR_OK) || (bDialActivated == FALSE))
	{
		NXL_LOG_ERR("Error> invalid DIAL status(herr:%d, act:%d)!\n", herr, bDialActivated);
		goto func_error;
	}

	HxSTD_MemSet(pBuf, 0, size);
	herr = APK_DIAL_GetFriendlyName(pBuf, size);
	if ((herr != ERR_OK) || HxSTD_StrEmpty(pBuf))
	{
		NXL_LOG_ERR("Error> cant get friendlyName!\n");
		herr = APK_SYSINFO_STB_GetModelName(szDefaultFriendlyName, 128);
		if ((herr != ERR_OK) || HxSTD_StrEmpty(pBuf))
		{
			HxSTD_snprintf(pBuf, sizeof(pBuf)-1, "%s", productName);
		}
		else
		{
			HxSTD_snprintf(pBuf, sizeof(pBuf)-1, "%s", szDefaultFriendlyName);
			HxSTD_StrNCat(pBuf, " DIAL Server", sizeof(pBuf)-1);
		}
	}

	NXL_LOG_DBG("friendlyName(%s)\n", pBuf);
	return NXL_ERR_OK;

func_error:
	HxSTD_PrintToStrN(pBuf, size, "Friendly Netflix Device");
	NXL_LOG_ERR("Error> failed to get friendlyName(%s)\n", pBuf);
	return NXL_ERR_FAIL;
}

uint32_t nxl_apkv2_sys_isScreensaverOn(uint32_t *onScreenSaver) /* TODO: Screen Saver */
{
	NXL_APKV2_Context_t *context = __getContext();
	_LOCK_CTX(context);
	*onScreenSaver = context->screenSaverRunning;
	_UNLOCK_CTX(context);
	return NXL_ERR_UNSUPPORTED;
}

#define ____NXL_Volume_Related_Functions____

/* Audio Volume Control */
uint32_t nxl_apkv2_output_getVolumePercent(uint32_t *pPercent)
{
	HUINT32 max = 0, min = 0;
	HUINT32 volume = 0;
	HUINT32 unit = 0;

	__CHECK_PARAM_PTR(pPercent);

	APK_OUTPUT_AUDIO_GetVolumeMax(&max);
#if 0
	APK_OUTPUT_AUDIO_GetVolumeMin(&min);
#else
	min = 0;
#endif
	APK_OUTPUT_AUDIO_GetVolume(&volume);
	unit = max-min ;
	unit = unit <= 0 ? volume : unit;

	/*volume scope :  0 ~ 100 */
	*pPercent = (HUINT32)((volume / (float)unit) * 100);
	NXL_LOG_DBG("(volume:%d, percent:%d)\n", volume, *pPercent);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_getVolumeLevel(uint32_t *pnVolume)
{
	HINT32	nVolume;
	__CHECK_PARAM_PTR(pnVolume);

	// APK_OUTPUT_AUDIO_GetVolume((HINT32*)pnVolume);
	// *pnVolume = APK_DB_GetInt((unsigned char *)USERDB_SURFER_VOLUME_LEVEL);
	{
		APK_OUTPUT_AUDIO_GetVolume(&nVolume);
		*pnVolume = nVolume;
	}
	NXL_LOG_DBG("(volume:%d)\n", nVolume);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_setVolumePercent(uint32_t percent)
{
	HUINT32 max = 0, min = 0;
	HUINT32 volume = 0;
	HUINT32 unit = 0;

	if (percent > 100)
		return NXL_ERR_INVALID_PARAM;

	APK_OUTPUT_AUDIO_GetVolumeMax(&max);
#if 0
	APK_OUTPUT_AUDIO_GetVolumeMin(&min);
#else
	min = 0;
#endif
	APK_OUTPUT_AUDIO_GetVolume(&volume);
	unit = max-min ;
	unit = unit <= 0 ? 1 : unit;

	/*volume scope :  0 ~ 100 */
	if (percent)
		volume = (HUINT32)((percent / (float)100.0f) * unit);

	NXL_LOG_DBG("(volume:%d, percent:%d)\n", volume, percent);
	APK_OUTPUT_AUDIO_SetVolume(volume);

	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_setVolumeLevel( uint32_t nVolume)
{
	/*
	if (nVolume)
	{
		HBOOL bMute = FALSE;

		APK_OUTPUT_AUDIO_GetMasterMute(&bMute);
		if (bMute)
		{
			APK_OUTPUT_AUDIO_SetMasterMute(FALSE);
			APK_UDB_SetValue((HUINT8 *)USERDB_AUDIO_MUTE, (HUINT32)FALSE);
		}
	}
	*/
#if 0
	APK_OUTPUT_AUDIO_SetVolume(nVolume);
	APK_UDB_SetValue((HUINT8 *)USERDB_SURFER_VOLUME_LEVEL, (HUINT32)nVolume);
#endif
	NXL_LOG_DBG("(volume:%d)\n", nVolume);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_getVolumeStep(uint32_t *pStep)
{
	HUINT32 max = 0, min = 0;
	HUINT32 unit = 0;

	__CHECK_PARAM_PTR(pStep);

	APK_OUTPUT_AUDIO_GetVolumeMax(&max);
#if 0
	APK_OUTPUT_AUDIO_GetVolumeMin(&min);
#else
	min = 0;
#endif
	unit = max-min ;
	unit = unit <= 0 ? 1 : unit;
	*pStep = 1 * 100 / unit ; // ++1, --1

	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isMuted(uint32_t *pMute)
{
	NXL_APKV2_Context_t *context = __getContext();

	__CHECK_PARAM_PTR(pMute);

	//APK_OUTPUT_AUDIO_GetMasterMute(pMute);
	_LOCK_CTX(context);
	*pMute = context->volumeMuted;
	_UNLOCK_CTX(context);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_setMute(uint32_t mute)
{
	APK_OUTPUT_AUDIO_SetMasterMute(mute ? TRUE : FALSE);
	return NXL_ERR_OK;
}


#define ____NXL_Audio_Related_Functions____

uint32_t nxl_apkv2_audio_getSoundMode(uint32_t ulDevIdx, char* szSoundMode)
{
#if 0
	DxStereoSelect_e		eAudioConfig;
	HCHAR					*data;
	__CHECK_PARAM_PTR(szSoundMode);

	APK_OUTPUT_AUDIO_GetStereoSelect(&eAudioConfig);

	data = OPL_UTIL_GetStringFromValue(_SoundModeList, eAudioConfig);

	NXL_LOG_DBG("GetSoundMode:%s[%d]\n",data, eAudioConfig);
	HxSTD_StrNCpy(szSoundMode, data, 31);
#endif
	return NXL_ERR_OK;
}

static eNXL_AudioDigitalOutType local_apkv2_convertAudioDigitalOutType(DxDigitalAudioFormat_e eDxDigitalOut)
{
	switch (eDxDigitalOut)
	{
		case eDxDIGITALAUDIO_FORMAT_UNKNOWN:           return NXL_AUDIO_DIGITAL_OUT_UNKNOWN;
		case eDxDIGITALAUDIO_FORMAT_PCM:               return NXL_AUDIO_DIGITAL_OUT_DOWNMIX_PCM;
		case eDxDIGITALAUDIO_FORMAT_MULTICHANNEL:      return NXL_AUDIO_DIGITAL_OUT_BYPASS;
		default: break;
	}
	return NXL_AUDIO_DIGITAL_OUT_UNKNOWN;
}

static uint32_t local_apkv2_getNumOfAudioDev(uint32_t *pnNumOfDev)
{
	HUINT32		ulDevNum = 1;
#if defined(CONFIG_SUPPORT_PRISM_APP)
	ulDevNum = 2;
#else
	ulDevNum = 1;
#endif
	*pnNumOfDev = ulDevNum = 1;
	return NXL_ERR_OK;
}

static uint32_t local_apkv2_getSpdifAudioOutputMode(uint32_t ulDevIdx, char* szSpdifOutput)
{
	DxDigitalAudioFormat_e		eSpdifOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	HCHAR						*data;
	__CHECK_PARAM_PTR(szSpdifOutput);

	if (APK_OUTPUT_AUDIO_GetSpdifFormat(&eSpdifOut) != ERR_OK)
	{
		NXL_LOG_ERR("Getting spdifOut format is failed.\n");
		return NXL_ERR_FAIL;
	}

	data = local_apkv2_getStringFromValue(_astDigitalOutList, eSpdifOut);
	HxSTD_StrNCpy(szSpdifOutput, data, 31);
	return NXL_ERR_OK;
}

static uint32_t local_apkv2_getHdmiAudioOutputMode(uint32_t ulDevIdx, char* szHdmiOutput)
{
	DxDigitalAudioFormat_e		eHdmiOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	HCHAR						*data;
	__CHECK_PARAM_PTR(szHdmiOutput);

	if (APK_OUTPUT_AUDIO_GetHdmiFormat(&eHdmiOut) != ERR_OK)
	{
		NXL_LOG_ERR("Getting HdmiAudioOut is failed.\n");
		return NXL_ERR_FAIL;
	}

	data = local_apkv2_getStringFromValue(_astDigitalOutList, eHdmiOut);
	HxSTD_StrNCpy(szHdmiOutput, data, 31);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_audio_getDigitalAudioOutputMode(eNXL_AudioDigitalOutType *pDigitalOut) /* TODO : ulDevIdx */
{
	HUINT32	ulDevNum = 1, ulDevIdx = 0;
	DxDigitalAudioFormat_e eHdmiOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	DxDigitalAudioFormat_e eSpdifOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
//	HCHAR *data;
//
//#if 0
//	eDxDigitalOut = APK_DB_GetInt((const HUINT8 *)USERDB_AUDIO_DIGITAL_AUDIO_OUT);  /* Read current setting */
//#else
//	APK_SQCAUDIO_GetDigitalOutputMode(&eDxDigitalOut);
//#endif
//	data = local_apkv2_getStringFromValue(_astDigitalOutList, eDxDigitalOut);

	ulDevNum = 1; // local_apkv2_getNumOfAudioDev(&ulDevNum);
	if (ulDevNum > 1 )
	{
		if (0 == ulDevIdx) // HDMI
		{
			if (APK_OUTPUT_AUDIO_GetHdmiFormat(&eHdmiOut) != ERR_OK)
			{
				NXL_LOG_ERR("Getting HdmiAudioOut is failed.\n");
				return NXL_ERR_FAIL;
			}
		}
		else if (1 == ulDevIdx) // SPDIF
		{
			if (APK_OUTPUT_AUDIO_GetSpdifFormat(&eSpdifOut) != ERR_OK)
			{
				NXL_LOG_ERR("Getting spdifOut format is failed.\n");
				return NXL_ERR_FAIL;
			}
		}
		else
		{
			return NXL_ERR_FAIL;
		}
	}
	else
	{
		if (APK_OUTPUT_AUDIO_GetHdmiFormat(&eHdmiOut) != ERR_OK)
		{
			NXL_LOG_ERR("Getting HdmiAudioOut is failed.\n");
			return NXL_ERR_FAIL;
		}
	}
	*pDigitalOut = local_apkv2_convertAudioDigitalOutType(eHdmiOut);
	return NXL_ERR_OK;
}


#define ____NXL_Output_Related_Functions____

uint32_t nxl_apkv2_output_getTVStandard(eNXL_TVOUT_STANDARD *eStandard)
{
	DxCompositeStandard_e	eVideoStandard;
	__CHECK_PARAM_PTR(eStandard);
	*eStandard = NXL_TVOUT_STANDARD_UNKNOWN;

	APK_OUTPUT_VIDEO_GetCompositeStandard(&eVideoStandard);
	switch (eVideoStandard)
	{
		case eDxCOMPOSITE_STANDARD_UNKNOWN:
			*eStandard = NXL_TVOUT_STANDARD_UNKNOWN;
			break;
		case eDxCOMPOSITE_STANDARD_NTSC:
			*eStandard = NXL_TVOUT_STANDARD_NTSC;
			break;
		case eDxCOMPOSITE_STANDARD_PAL:
		case eDxCOMPOSITE_STANDARD_PAL_M:
		case eDxCOMPOSITE_STANDARD_PAL_N:
			*eStandard = NXL_TVOUT_STANDARD_PAL;
			break;
		case eDxCOMPOSITE_STANDARD_SECAM:
			*eStandard = NXL_TVOUT_STANDARD_SECAM;
			break;
		default:
			break;
	}
	return NXL_ERR_OK;
}

/* Video Output */
//uint32_t nxl_apkv2_output_getSupportedVidoeOutput(void)
//{
//	HUINT32 devnum = 0;
//	APK_OUTPUT_VIDEO_GetNumOfDevice(&devnum);
//	return NXL_ERR_OK;
//}

uint32_t nxl_apkv2_output_getDeviceNum(uint32_t *pnNumOfDev)
{
	HUINT32 ulDevNum = 1;
	__CHECK_PARAM_PTR(pnNumOfDev);
	*pnNumOfDev = 0;

#if 0
	HERROR herr = ERR_OK;
	herr = APK_OUTPUT_VIDEO_GetNumOfDevice((HUINT32*)pnNumOfDev);
	if (ERR_OK != herr)
	{
		NXL_LOG_ERR("APK_OUTPUT_VIDEO_GetNumOfDevice rc herr:0x%X.\n",herr);
		return NXL_ERR_FAIL;
	}
#else
	ulDevNum = 1;
#endif
	*pnNumOfDev = ulDevNum = 1;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isSupportScart(uint32_t *pExisted)
{
	HBOOL bExist = FALSE;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_OUTPUT_VIDEO_SupportTvScart(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("SupportTvScart rc Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isSupportTvScart(uint32_t *pExisted)
{
	HBOOL bExist = FALSE;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_OUTPUT_VIDEO_SupportTvScart(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("SupportTvScart rc Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isSupportVcrScart(uint32_t *pExisted)
{
	HBOOL bExist = FALSE;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_OUTPUT_VIDEO_SupportVcrScart(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("SupportVcrScart rc Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isSupportComposite(uint32_t *pExisted)
{
	HBOOL bExist = FALSE;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_OUTPUT_VIDEO_SupportComposite(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("SupportComposite rc Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isSupportComponent(uint32_t *pExisted)
{
#if 0
	HBOOL bExist = FALSE;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_OUTPUT_VIDEO_SupportComponent(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("SupportComponent rc Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
#else
	*pExisted = FALSE;
#endif
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isSupportRF(uint32_t *pExisted)
{
	__CHECK_PARAM_PTR(pExisted);

#if 0
	HBOOL bExist = FALSE;
	if (APK_OUTPUT_VIDEO_SupportRf(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("SupportRf rc Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
#else
	*pExisted = FALSE;
#endif
	return NXL_ERR_OK;
}

static uint32_t local_apkv2_isSupportHdmi(HBOOL *pExisted)
{
	__CHECK_PARAM_PTR(pExisted);
	*pExisted = TRUE;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isSupportHDMI(uint32_t *pExisted, eNXL_HdcpVer *pHdcpVer)
{
	HBOOL bExist = FALSE;
	DxHdcpVersion_e eDxHdcpVer = eDxHDCP_VERSION_1_4;

	*pExisted = FALSE; *pHdcpVer = NXL_HDCP_VERSION_NA;
	__CHECK_PARAM_PTR(pExisted);
#if 0
	if (local_apkv2_isSupportHdmi(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("IsSupportHdmi rc Error.\n");
		return NXL_ERR_FAIL;
	}
#else
	bExist = TRUE;
#endif
	*pExisted = bExist;

	if (APK_OUTPUT_HDMI_GetHdcpVersion(&eDxHdcpVer) != ERR_OK)
	{
		NXL_LOG_ERR("Error> GetHdcpVersion() failed\n");
		return NXL_ERR_OK;
	}

	switch (eDxHdcpVer) {
	case eDxHDCP_VERSION_1_0: *pHdcpVer = NXL_HDCP_VERSION_1_0; break;
	case eDxHDCP_VERSION_1_1: *pHdcpVer = NXL_HDCP_VERSION_1_1; break;
	case eDxHDCP_VERSION_1_2: *pHdcpVer = NXL_HDCP_VERSION_1_2; break;
	case eDxHDCP_VERSION_1_3: *pHdcpVer = NXL_HDCP_VERSION_1_3; break;
	case eDxHDCP_VERSION_1_4: *pHdcpVer = NXL_HDCP_VERSION_1_4; break;
	case eDxHDCP_VERSION_2_0: *pHdcpVer = NXL_HDCP_VERSION_2_0; break;
	case eDxHDCP_VERSION_2_1: *pHdcpVer = NXL_HDCP_VERSION_2_1; break;
	case eDxHDCP_VERSION_2_2: *pHdcpVer = NXL_HDCP_VERSION_2_2; break;
	default:
		/* NXL_LOG_ERR("Error> unsupported HDCP ver(%d)\n", eDxHdcpVer); */
		*pHdcpVer = NXL_HDCP_VERSION_NA;
		return NXL_ERR_OK;
	}

	/* NXL_LOG_DBG("Getting HDCP ver(%d)\n", eDxHdcpVer); */
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isSupportSpdif (uint32_t *pExisted)
{
	HBOOL bExist = FALSE;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_OUTPUT_AUDIO_SupportSpdif (&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("SupportSpdif rc Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isActiveOutputHDMI(uint32_t *pActivated, eNXL_HdcpVer *pHdcpVer)
{
	/* DxHdmiHotplugStatus_e eHotplugStatus; */
	HBOOL bHdmiEnabled = FALSE;

	*pActivated = FALSE; *pHdcpVer = eDxHDCP_VERSION_MAX;
	__CHECK_PARAM_PTR(pActivated);

	if (APK_OUTPUT_HDMI_IsPlugIn(&bHdmiEnabled) != ERR_OK)
	{
		NXL_LOG_ERR("Error> IsPlugIn() failed\n");
		return NXL_ERR_OK;
	}
	*pActivated = bHdmiEnabled;
	/* NXL_LOG_DBG("HDMI activated(%d)\n", bHdmiEnabled); */
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_isActiveOutputSpdif (uint32_t *pActivated)
{
	HBOOL bActivated = FALSE;
	HBOOL bSpdifSupport = TRUE;
	DxDigitalAudioFormat_e eSpdifOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	/* DxAudioCodec_e eSpecificCodec; */
	__CHECK_PARAM_PTR(pActivated);

	*pActivated = FALSE;
	(void)APK_OUTPUT_AUDIO_SupportSpdif (&bSpdifSupport);
	if (bSpdifSupport == FALSE)
	{
		NXL_LOG_ERR("Error> unsupported spdifOut output\n");
		goto func_done;
	}

	if (APK_OUTPUT_AUDIO_GetSpdifFormat(&eSpdifOut) != ERR_OK)
	{
		NXL_LOG_ERR("Error> Getting spdifOut format is failed!\n");
		goto func_done;
	}

	switch (eSpdifOut) {
	case eDxDIGITALAUDIO_FORMAT_PCM:
	case eDxDIGITALAUDIO_FORMAT_MULTICHANNEL:
		bActivated = TRUE;
		break;
	case eDxDIGITALAUDIO_FORMAT_UNKNOWN:
	default:
		bActivated = FALSE;
		break;
	}

func_done:
	*pActivated = (uint32_t)bActivated;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_getHdcpStatus(uint32_t *pbOn)
{
#if 0
	HBOOL isOn = FALSE;
	/* DxHdcpStatus_e eHdcpStatus = eDxHDCPSTATUS_OFF; */

	*pbOn = FALSE;
#if 0
	HERROR herr = ERR_OK;
	herr = APK_OUTPUT_HDMI_GetHdcpStatus(&eHdcpStatus);
	if (herr == ERR_OK)
	{
		if (eHdcpStatus == eDxHDCPSTATUS_OK) // or eDxHDCPSTATUS_TRY
		{
			isOn = TRUE;
		}
	}
	NXL_LOG_DBG("GetHdcp(status:%d)\n", eHdcpStatus);
#else
	if (APK_OUTPUT_HDMI_GetHdcp(&isOn) != ERR_OK)
	{
		NXL_LOG_ERR("Error> GetHdcp() failed\n");
		return NXL_ERR_FAIL;
	}
#endif
	*pbOn = isOn;
	NXL_LOG_DBG("GetHdcp(on:%d)\n", isOn);
#else
    DxNflixDrmStatus_e eNflixDrmStatus;

    *pbOn = FALSE;
    if (APK_OUTPUT_HDMI_GetNetflixDrmStatus(&eNflixDrmStatus) != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_OUTPUT_HDMI_GetNetflixDrmStatus() failed\n");
		return NXL_ERR_FAIL;
	}

    if (eNflixDrmStatus == eNFLIX_DRM_RUNNING)
    {
	    *pbOn = TRUE;
    }
	NXL_LOG_DBG("getHdcpStatus(%d:%d)\n", eNflixDrmStatus, *pbOn);
#endif
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_setHdcpStatus(uint32_t bOn, uint32_t bForce) /* CUP (Content Usage Policy) */
{
	NXL_APKV2_Context_t *context = __getContext();
        HUINT32 nxErr = NXL_ERR_OK;
        HUINT32 resWidth = 0x0, resHeight = 0x0;
        HUINT32 maxImageWidth = 0x0, maxImageHeight = 0x0;

	_NOT_USED_PARAM(nxErr);
	_NOT_USED_PARAM(resHeight);
	_NOT_USED_PARAM(resWidth);

	_LOCK_CTX(context);
#if 0
	maxImageWidth = context->streamMaxWidth;
	maxImageHeight = context->streamMaxHeight;
#else
        maxImageWidth = context->streamCurrentWidth;
        maxImageHeight = context->streamCurrentHeight;
#endif
	_UNLOCK_CTX(context);
        NXL_LOG_ERR("setNetflixDRMStatus(on:%d, force:%d), w:%d, h:%d\n", bOn?eNFLIX_DRM_RUNNING:eNFLIX_DRM_INIT, bForce, maxImageWidth, maxImageHeight);

        if (!bForce && bOn && maxImageWidth && maxImageHeight ) /* Netflix HDCP Scenario : allow the normal playback/output for SD content. */
        {
            if (maxImageHeight < 720)
            {
                /* SD : 720x480, 720x576, ...
                 * HD : 1280x720, 1920x1080, 1440x1080, ...
                 */
                NXL_LOG_ERR("[HDCP ON] is ignored under SD(%d x %d)\n", maxImageWidth, maxImageHeight);
                return NXL_ERR_OK;
            }
        }

    if (bOn == TRUE)
	{
		APK_OUTPUT_HDMI_SetNetflixDrmStatus(eNFLIX_DRM_RUNNING);
	}
	else
	{
		APK_OUTPUT_HDMI_SetNetflixDrmStatus(eNFLIX_DRM_INIT);
	}

	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_getHdcpVersion(eNXL_HdcpVer *pHdcpVer) /* CUP (Content Usage Policy) */
{
	DxHdcpVersion_e eDxHdcpVer = eDxHDCP_VERSION_1_4;

	*pHdcpVer = NXL_HDCP_VERSION_1_4; /* default */
	if (APK_OUTPUT_HDMI_GetHdcpVersion(&eDxHdcpVer) != ERR_OK)
	{
		NXL_LOG_ERR("Error> GetHdcpVersion() failed\n");
		return NXL_ERR_FAIL;
	}

	switch (eDxHdcpVer) {
	case eDxHDCP_VERSION_1_0:  *pHdcpVer = NXL_HDCP_VERSION_1_0; break;
	case eDxHDCP_VERSION_1_1:  *pHdcpVer = NXL_HDCP_VERSION_1_1; break;
	case eDxHDCP_VERSION_1_2:  *pHdcpVer = NXL_HDCP_VERSION_1_2; break;
	case eDxHDCP_VERSION_1_3:  *pHdcpVer = NXL_HDCP_VERSION_1_3; break;
	case eDxHDCP_VERSION_1_4:  *pHdcpVer = NXL_HDCP_VERSION_1_4; break;
	case eDxHDCP_VERSION_2_0:  *pHdcpVer = NXL_HDCP_VERSION_2_0; break;
	case eDxHDCP_VERSION_2_1:  *pHdcpVer = NXL_HDCP_VERSION_2_1; break;
	case eDxHDCP_VERSION_2_2:  *pHdcpVer = NXL_HDCP_VERSION_2_2; break;
	default:
		NXL_LOG_ERR("Error> unsupported HDCP ver(%d)\n", eDxHdcpVer);
		return NXL_ERR_FAIL;
	}

	NXL_LOG_DBG("Getting HDCP ver(%d)\n", eDxHdcpVer);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_getCgmsStatus(eNXL_CopyRight *pCgmsStatus)
{
	DxCopyRight_e eDxCopyRight = eDxCOPYRIGHT_COPY_FREE;

	*pCgmsStatus = NXL_COPYRIGHT_COPY_FREE;
	if (APK_OUTPUT_VIDEO_GetVideoBlankCopyRight(&eDxCopyRight) != ERR_OK)
	{
		NXL_LOG_ERR("Error> GetCgmsStatus() failed\n");
		return NXL_ERR_FAIL;
	}

	switch (eDxCopyRight) {
	case eDxCOPYRIGHT_COPY_FREE:    *pCgmsStatus = NXL_COPYRIGHT_COPY_FREE; break;
	case eDxCOPYRIGHT_COPY_NOMORE:  *pCgmsStatus = NXL_COPYRIGHT_COPY_NOMORE; break;
	case eDxCOPYRIGHT_COPY_ONCE:    *pCgmsStatus = NXL_COPYRIGHT_COPY_ONCE; break;
	case eDxCOPYRIGHT_COPY_NEVER:   *pCgmsStatus = NXL_COPYRIGHT_COPY_NEVER; break;
	default:
		NXL_LOG_ERR("Error> unmatched copyRight(%d)\n", eDxCopyRight);
		return NXL_ERR_FAIL;
	}

	NXL_LOG_DBG("Getting copyRight(%d)\n", eDxCopyRight);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_setCgmsStatus(eNXL_CopyRight eCgmsStatus)
{
	DxCopyRight_e eDxCopyRight = eDxCOPYRIGHT_COPY_FREE;

	switch (eCgmsStatus) {
		case NXL_COPYRIGHT_COPY_FREE:    eDxCopyRight = eDxCOPYRIGHT_COPY_FREE; break;
		case NXL_COPYRIGHT_COPY_NOMORE:  eDxCopyRight = eDxCOPYRIGHT_COPY_NOMORE; break;
		case NXL_COPYRIGHT_COPY_ONCE:    eDxCopyRight = eDxCOPYRIGHT_COPY_ONCE; break;
		case NXL_COPYRIGHT_COPY_NEVER:   eDxCopyRight = eDxCOPYRIGHT_COPY_NEVER; break;
		default:
			NXL_LOG_ERR("Error> unsupported CopyRight(%d)\n", eCgmsStatus);
			return NXL_ERR_FAIL;
	}

	NXL_LOG_ERR("SetCgmsStatus(%d)\n", eDxCopyRight);
	if (APK_OUTPUT_VIDEO_SetVideoBlankCopyRight(eDxCopyRight) != ERR_OK)
	{
		NXL_LOG_ERR("Error> SetCgmsStatus() failed\n");
		return NXL_ERR_FAIL;
	}
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_getEdidDisplayResolution(uint32_t *puDisplayResX, uint32_t *puDisplayResY)
{
	HERROR herr = ERR_OK;
	HUINT32 ulDisplayResX = 0, ulDisplayResY = 0;
	/* DxResolutionStatus_b ulResolutionCapability = eDxRESOLUTION_STATUS_UNKNOWN; */

	*puDisplayResX = *puDisplayResY = 0;
	/* Getting display size thru EDID */
	herr = APK_OUTPUT_HDMI_GetEdidDisplayResolution(&ulDisplayResX, &ulDisplayResY);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("Error> GetEdidDisplayResolution() failed\n");
		return NXL_ERR_FAIL;
	}
	*puDisplayResX = ulDisplayResX; *puDisplayResY = ulDisplayResY;
	NXL_LOG_DBG("GetEdidDisplayResolution(x:%d, y:%d)\n", ulDisplayResX, ulDisplayResY);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_getActiveVideoOutput(void)
{
	// Not implemented
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_apkv2_output_getOutputResolution(uint32_t *pWidth, uint32_t *pHeight)
{
	NXL_APKV2_Context_t *context = __getContext();
	DxResolutionSelect_e eResolutionSelect = eDxRESOLUTION_SELECT_UNKNOWN; /* APKE_OutputResolution_e */
	HUINT32 width = 0, height = 0;

	__CHECK_PARAM_PTR(pWidth);
	__CHECK_PARAM_PTR(pHeight);
	*pWidth = *pHeight = 0;

	if ((APK_OUTPUT_VIDEO_GetResolutionSelect(&eResolutionSelect) != ERR_OK) || (eResolutionSelect == eDxRESOLUTION_SELECT_UNKNOWN))
	{
		NXL_LOG_ERR("Error> failed to get resolutionSelect(%d)!\n", eResolutionSelect);
#ifdef __APKV2_UDB__
		eResolutionSelect = APK_DB_GetInt((HUINT8 *)USERDB_VIDEO_RESOLUTION);
#endif
	}

	if (eResolutionSelect == eDxRESOLUTION_SELECT_UNKNOWN)
	{
		NXL_LOG_ERR("Error> unknown resolutionSelect(%d)!\n", eResolutionSelect);
	}

	/*
	data = local_apkv2_getStringFromValue(_astResolutionSelectPairList, eResolutionSelect);
	HxSTD_StrNCpy(pszResolutionSelect, data, (nMaxStrlen-1));
	pszResolutionSelect[(nMaxStrlen-1)] = '\0';
	*/

	switch(eResolutionSelect)
	{
		case eDxRESOLUTION_SELECT_480I:
		case eDxRESOLUTION_SELECT_480P:
#if 1
			width = 640; height = 480;
#else
			width = 720; height = 480;
#endif
			break;
		case eDxRESOLUTION_SELECT_576I:
		case eDxRESOLUTION_SELECT_576P:
			width = 720; height = 576;
			break;
		case eDxRESOLUTION_SELECT_720P:
#if 0 /* DPI-040-TC9/DPI-175-TC6 : DPI - Verify support2DVideoResize - 720p */
			width = 1080; height = 720;
#else
			width = 1280; height = 720;
#endif
			break;
		case eDxRESOLUTION_SELECT_1080I:
		case eDxRESOLUTION_SELECT_1080P:
			width = 1920; height = 1080;
			break;
#if 0
		case eDxRESOLUTION_SELECT_2160P:
			width = 4096; height = 2160;
			break;
#endif
		default:
			width = 0; height = 0;
			break;
	}

	_LOCK_CTX(context);

	if (context->lastSelectedResolution != eResolutionSelect)
	{
		context->lastOutputResolutionChanged = TRUE;
	}

	if ((context->lastOutputResolutionWidth != width) || (context->lastOutputResolutionHeight != height))
	{
		context->lastOutputResolutionChanged = TRUE;
	}


	if (eResolutionSelect != eDxRESOLUTION_SELECT_UNKNOWN)
	{
		context->lastSelectedResolution = eResolutionSelect;
	}
	if (width && height)
	{
		context->lastOutputResolutionWidth = width;
		context->lastOutputResolutionHeight = height;
	}
	_UNLOCK_CTX(context);

	/* NXL_LOG_DBG("eResolutionSelect(%d) - width(%d), height(%d)\n", eResolutionSelect, width, height); */
	*pWidth = width; *pHeight = height;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_checkLastOutputResolutionChanged(uint32_t *pUpdated)
{
	NXL_APKV2_Context_t *context = __getContext();
	HUINT32 width = 0, height = 0;

	*pUpdated = 0;
	if (nxl_apkv2_output_getOutputResolution(&width, &height) == NXL_ERR_OK)
	{
		_LOCK_CTX(context);
		*pUpdated = context->lastOutputResolutionChanged;
		context->lastOutputResolutionChanged = FALSE;
		_UNLOCK_CTX(context);
	}

	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_getSupportedResolutions(/* (uint32_t ulDevIdx) */ int *count, char ***pszResolutions)
{
	HERROR		nRet;
	HUINT32		ulMaxNum,idx,ulChkIdx,ulMaxValue;
	HUINT32		ulExistCnt=0;
	char			**myStrings=NULL;
	DxResolutionSelect_e	eSupportedResolutions, eResolution;

	if ((NULL == count) || (NULL == pszResolutions))
		return NXL_ERR_FAIL;

	*pszResolutions = NULL;
	*count = 0;

	nRet = APK_OUTPUT_VIDEO_GetResolutionSelect(&eSupportedResolutions);
	if (ERR_OK != nRet)
	{
		NXL_LOG_ERR("Error> GetSupportedResolution rc Error:0x%X\n",nRet);
		return NXL_ERR_OK;

	}
	NXL_LOG_DBG("eSupportedResolutions:0x%X\n",eSupportedResolutions);

	ulMaxNum = 14; ulExistCnt = 0;
	for (idx=0; idx < ulMaxNum; idx++)
	{
		eResolution = (eSupportedResolutions & (eDxRESOLUTION_SELECT_480I << idx));
		if (eResolution)
		{
			ulExistCnt++;
		}
	}
	if (0 == ulExistCnt)
	{
		NXL_LOG_ERR(" ulExistCnt is zero\n");
		return NXL_ERR_OK;
	}

	*count = (int)ulExistCnt;
	NXL_LOG_DBG("\tulExistCnt : %d\n",ulExistCnt);
	myStrings = (char **)malloc(sizeof(char*) * ulExistCnt);
	ulChkIdx=0;
	for (ulMaxValue = 0, idx=0; ulMaxValue != eDxRESOLUTION_SELECT_MAX_NUM; ulMaxValue = ((eDxRESOLUTION_SELECT_480I << idx) | ulMaxValue), idx++)
	{
		HCHAR	*data;

		NXL_LOG_DBG("idx:%d ulMaxValue:0x%X\n",idx,ulMaxValue);

		eResolution = (eSupportedResolutions & (eDxRESOLUTION_SELECT_480I << idx));
		if (eResolution)
		{
			data = local_apkv2_getStringFromValue(_astResolutionSelectPairList, eResolution);
			NXL_LOG_DBG("\t\t[%02d][0x%X][%s]\n",idx,eResolution,data);
			myStrings[ulChkIdx] = strdup(data);
			ulChkIdx++;
		}
		if (ulChkIdx == ulExistCnt)
			break;
	}
	*pszResolutions = myStrings;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_getTvAspectRatio(uint32_t ulDevIdx, char *pszTvAspectRatio)
{
	HCHAR	*data;
	DxAspectRatio_e	eTVAspectRatio;
	__CHECK_PARAM_PTR(pszTvAspectRatio);

	APK_OUTPUT_VIDEO_GetTvAspectRatio((DxAspectRatio_e *)&eTVAspectRatio);
	NXL_LOG_DBG(" eTVAspectRatio:0x%X\n",eTVAspectRatio);

	data = local_apkv2_getStringFromValue(_astAspectRatioPairList, eTVAspectRatio);
	HxSTD_StrNCpy(pszTvAspectRatio, data, 31);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_output_getVideoStandard(uint32_t ulDevIdx, char *pszVideoStandard)
{
	HCHAR		*data;
	HERROR		nRet;
	DxCompositeStandard_e	eVideoStandard;
	__CHECK_PARAM_PTR(pszVideoStandard);

	nRet = APK_OUTPUT_VIDEO_GetCompositeStandard(&eVideoStandard);
	if (ERR_OK != nRet)
	{
		NXL_LOG_ERR("GetCompositeStandard rc Error:0x%X.\n",nRet);
		return NXL_ERR_FAIL;
	}
	NXL_LOG_DBG(" eVideoStandard:0x%X\n",eVideoStandard);

	data = local_apkv2_getStringFromValue(_astVideoStandardPairList, eVideoStandard);
	HxSTD_StrNCpy(pszVideoStandard, data, 31);
	return NXL_ERR_OK;
}


#define ____NXL_Media_Related_Functions____

/* ES Manager */
uint32_t nxl_apkv2_mediapb_getResourceMap(void *pMap)
{
	NXL_APKV2_Context_t *context = __getContext();
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	if (pMap == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> pMap is NULL\033[0m\n");
		return NXL_ERR_INVALID_PARAM;
	}

	_LOCK_CTX(context);
	HxSTD_MemSet(&context->stResourceMap, 0x00, sizeof(OxMediaPlay_ResourceMap_t));

	herr = APK_MEDIA_PLAY_GetResourceMap(&context->stResourceMap);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to get a resource map\033[0m\n");
		_UNLOCK_CTX(context);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	((NXL_ESMANAGER_ResourceMap_t *)pMap)->hPlaypumpList   = context->stResourceMap.pulPlaypumpList;
	((NXL_ESMANAGER_ResourceMap_t *)pMap)->hDmaList        = context->stResourceMap.pulDmaList;
	((NXL_ESMANAGER_ResourceMap_t *)pMap)->uiNumOfPlaypump = context->stResourceMap.ulNumOfPlaypump;
	((NXL_ESMANAGER_ResourceMap_t *)pMap)->uiNumOfDma      = context->stResourceMap.ulNumOfDma;
	((NXL_ESMANAGER_ResourceMap_t *)pMap)->uiAudioMixerHandle = context->stResourceMap.ulAudioMixerHandle;
	_UNLOCK_CTX(context);

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

/* Playback Group */
uint32_t nxl_apkv2_mediapb_registerNotifier(void *pFunc)
{
	HERROR herr = ERR_OK;
	NXL_ERR nRet = NXL_ERR_OK;
	HUINT32 ulViewId = 0x0;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	if (pFunc == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> pFunc is NULL\033[0m\n");
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
#if defined(__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__)
	herr = APK_MEDIA_PLAY_RegisterNotifier(ulViewId, pFunc, local_apkv2_mediapbEventFilter, NULL); /* TODO: Check eventFilter usage */
#else
	herr = APK_MEDIA_PLAY_RegisterNotifier(ulViewId, pFunc, NULL);
#endif
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("Error> failed to register event callback!\n");
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_unregisterNotifier(void *pFunc)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HUINT32 ulViewId = 0x0;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	if (pFunc == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> pFunc is NULL\033[0m\n");
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	APK_MEDIA_PLAY_UnregisterNotifier(ulViewId, pFunc, NULL);

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_start(void *pProbeInfo)
{
	NXL_APKV2_Context_t *context = __getContext();
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	HUINT32 ulSessionId = 0;
	OxMediaPlay_StartInfo_t unStart;
	NXL_PLAYBACKGROUP_ProbeInfo_t *pstProbeInfo = NULL;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	/* Notify Netflix mediapb state to netflixManager to prepare suspend, exit, channel up/down, ... */
	(void)HAPI_BroadcastSignal("signal:onNetflixPlaybackChanged", 1, "i", eNXL_NOTIFY_NETFLIXPB_STARTING);

	if (pProbeInfo == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to start Media PB\033[0m\n");
		return NXL_ERR_INVALID_PARAM;
	}
	pstProbeInfo = (NXL_PLAYBACKGROUP_ProbeInfo_t *)pProbeInfo;

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	HxSTD_MemSet(&unStart, 0x0, sizeof(OxMediaPlay_StartInfo_t));
	unStart.stMediaPlay.eType = eOxMP_MEDIATYPE_MEDIAPLAY;
#if defined(CONFIG_TIMESTAMP_64)
	unStart.stMediaPlay.ullStartTime = 0;
#else
	unStart.stMediaPlay.ulStartTime = 0;
#endif
	unStart.stMediaPlay.ulAudioPid = pstProbeInfo->audio_pid;
	unStart.stMediaPlay.ulVideoPid = pstProbeInfo->video_pid;
	unStart.stMediaPlay.eAudioCodec = (DxAudioCodec_e)pstProbeInfo->audio_codec;
	unStart.stMediaPlay.eVideoCodec = (DxVideoCodec_e)pstProbeInfo->video_codec;
	HxSTD_StrNCpy(unStart.stMediaPlay.szUrl, (const char *)("MSE"), sizeof("MSE"));

	herr = APK_MEDIA_PLAY_Start (ulViewId, &unStart, &ulSessionId);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Starting mediapb failed, herr = %d\033[0m\n", herr);
		goto FUNCTION_EXIT;
	}
	_LOCK_CTX(context);
	context->playerSessionId = ulSessionId;
	context->lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_Start; /* Distinguish between the START and PLAY to the check BUFFERING state in AMC::OverlayTask() . */
	_UNLOCK_CTX(context);

	/* Notify Netflix mediapb state to netflixManager to prepare suspend, exit, channel up/down, ... */
	(void)HAPI_BroadcastSignal("signal:onNetflixPlaybackChanged", 1, "i", eNXL_NOTIFY_NETFLIXPB_PLAYING);

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_stop(void)
{
	NXL_APKV2_Context_t *context = __getContext();
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	OxMediaPlay_MediaType_e ePlayType = eOxMP_MEDIATYPE_MEDIAPLAY;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	// Test for delayed mediapb stop() when exiting Netflix or channel up/down
	// if (1)
	// {
	// 	VK_TASK_Sleep(1000*20);
	// }

	/* Notify Netflix mediapb state to netflixManager to prepare suspend, exit, channel up/down, ... */
	(void)HAPI_BroadcastSignal("signal:onNetflixPlaybackChanged", 1, "i", eNXL_NOTIFY_NETFLIXPB_STOPPING);

	_LOCK_CTX(context);

	herr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to stop Media PB, herr:%d, play:%d\033[0m\n", herr, ePlayType);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	/* Dont trust OAPI cache info */
	if (0)
	{
		herr = APK_MEDIA_PLAY_GetPlayType(ulViewId , &ePlayType );
		if ((herr != ERR_OK) || (eOxMP_MEDIATYPE_MEDIAPLAY != ePlayType))
		{
			NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to stop Media PB, herr:%d, play:%d\033[0m\n", herr, ePlayType);
			nRet = NXL_ERR_FAIL;
			goto FUNCTION_EXIT;
		}
	}

	/*
	 * - Fixed LIVE black-out when exiting Netflix (or channel up/down).
	 * - Prevented the unexpected LivePB stop()
	 *     in switching between last MediaPB stop() and newly launched LivePB start().
	 * - TODO : Remove APK_MEDIA_PLAY_StopWithParams() for suspend-timeout
	 */
	herr = APK_MEDIA_PLAY_StopWithParams(ulViewId, eOxMP_MEDIATYPE_MEDIAPLAY, context->playerSessionId);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to stop playback, herr = %d\033[0m\n", __func__, __LINE__, herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	/* Notify Netflix mediapb state to netflixManager to prepare suspend, exit, channel up/down, ... */
	(void)HAPI_BroadcastSignal("signal:onNetflixPlaybackChanged", 1, "i", eNXL_NOTIFY_NETFLIXPB_STOPPED);

	/* Initialize stream videoAttributes */
	context->lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_Stop;
	context->streamPAR_X = context->streamPAR_Y = 0;
	context->streamCurrentWidth = context->streamCurrentHeight = 0;
	context->streamMaxWidth = context->streamMaxHeight = 0;
	_UNLOCK_CTX(context);
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_resetPlayback(void) /* Currently, unused */
{
	NXL_ERR nRet = NXL_ERR_OK;
#if 0
	NXL_APKV2_Context_t *context = __getContext();

	// platform_dfb_ipc: closing client:0x41cc438 with fd:328
	// bipc_server_destroy:0x15e3340 cleaning up object 0x429c210(328,328):brc_client(0x41cc474)
	// nexus_platform_server: abnormal termination of an unprotected client 0x4dcfb18 is not supported
	_LOCK_CTX(context);
	if (context->lastPlaybackCmd > NXL_PLAYBACKGROUP_CMD_Stop)
	{
		_UNLOCK_CTX(context);
		(void)nxl_apkv2_mediapb_stop();
	}
#endif
	return nRet;
}

uint32_t nxl_apkv2_mediapb_getNotifyArgs(uint32_t ulViewId, uint32_t ulSessionId, uint32_t eType, uint32_t eEvent, void *pAVCtrlEvent)
{
	NXL_ERR nRet = NXL_ERR_OK;
	/* HERROR herr = ERR_OK; */

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+) - [type:%d, evt:%d] \033[0m\n", __FUNCTION__, eType, eEvent);

	if (pAVCtrlEvent == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> pAVCtrlEvent is NULL\033[0m\n");
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	switch(eEvent)
	{
		case eOxMP_EVENT_STARTED:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_STARTED\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_Started;
			break;
		case eOxMP_EVENT_STOPPED:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_STOPPED\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_Stopped;
			break;
		case eOxMP_EVENT_PLAYTIME:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_PLAYTIME\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_PlaytimeChanged;
			break;
		case eOxMP_EVENT_PLAYSTATE:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_PLAYSTATE\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_PlayStateChanged;
			break;
		case eOxMP_EVENT_MEDIA_PumpIndexChanged:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_MEDIA_PumpIndexChanged\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_PumpIndexChanged;
			break;
		case eOxMP_EVENT_MEDIA_SeekReady:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_MEDIA_SeekReady\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_SeekReady;
			break;
		case eOxMP_EVENT_MEDIA_AudioCodecSwitched:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_MEDIA_AudioCodecSwitched\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioCodecSwitched;
			break;
		case eOxMP_EVENT_MEDIA_BufferStateChanged:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_MEDIA_BufferStateChanged\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_BufferStateChanged;
			break;
		case eOxMP_EVENT_MEDIA_AudioFlushCompleted:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_MEDIA_AudioFlushComplete\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioFlushCompleted;
			break;
		case eOxMP_EVENT_MEDIA_AudioRestartCompleted:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_MEDIA_AudioRestartComplete\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioRestartCompleted;
			break;
		case eOxMP_EVENT_MEDIA_AudioFadeCompleted:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_MEDIA_AudioFadeCompleted\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioFadeCompleted;
			break;
		case eOxMP_EVENT_MEDIA_AudioFadeFailed:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_MEDIA_AudioFadeFailed\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioFadeFailed;
			break;
		case eOxMP_EVENT_MEDIA_AudioDecoderVolumeChanged:
			NXL_LOG_DBG("\033[1;31m @@@@@ eOxMP_EVENT_MEDIA_AudioDecoderVolumeChanged\033[0m\n");
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioDecoderVolumeChanged;
			break;
		default:
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pAVCtrlEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_Unknown;
			break;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_setSpeed(int32_t nSpeedSec)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	NXL_APKV2_Context_t *context = __getContext();

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	herr = APK_MEDIA_PLAY_SetPlaySpeed(ulViewId, nSpeedSec);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Setting play speed failed, herr = %d\033[0m\n", herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	_LOCK_CTX(context);
	if (nSpeedSec)
	{
		/* Notify Netflix mediapb state to netflixManager to prepare suspend, exit, channel up/down, ... */
		(void)HAPI_BroadcastSignal("signal:onNetflixPlaybackChanged", 1, "i", eNXL_NOTIFY_NETFLIXPB_PLAYING);
		context->lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_Play;
	}
	else
	{
		/* Notify Netflix mediapb state to netflixManager to prepare suspend, exit, channel up/down, ... */
		(void)HAPI_BroadcastSignal("signal:onNetflixPlaybackChanged", 1, "i", eNXL_NOTIFY_NETFLIXPB_PAUSED);
		context->lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_Pause;
	}
	_UNLOCK_CTX(context);

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_setPosition(uint32_t unPlayTime)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr= APK_MEDIA_PLAY_SetPlayPosition(ulViewId, (HUINT64)unPlayTime);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to jump to position(%d), herr = %d\033[0m\n", unPlayTime, herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_getPumperIndex(uint32_t *punIndex)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	HUINT32 index = 0;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	if (punIndex == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> punIndex is NULL\033[0m\n");
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}
	*punIndex = 0x0;

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr = APK_MEDIA_PLAY_GetPumpIndex(ulViewId, &index);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to get a playpump index, herr = %d\033[0m\n", herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	*punIndex = index;

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_getPosition(uint32_t *punPosition)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	//OxMediaPlay_MediaType_e ePlayType;
#if defined(CONFIG_TIMESTAMP_64)
	HUINT64 unPlayTime = 0x0;
#else
	HUINT32 unPlayTime = 0x0;
#endif

	//NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	if (punPosition == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> punPosition is NULL\033[0m\n");
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	herr = APK_MEDIA_PLAY_GetPlayPosition (ulViewId, &unPlayTime);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to get a playing position, herr = %d\033[0m\n", herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	*punPosition = unPlayTime;

FUNCTION_EXIT:
	//NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_getPlayState(void *pState)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	//OxMediaPlay_MediaType_e ePlayType;
	OxMediaPlay_PlayState_e ePlayState = eOxMP_PLAYSTATE_NONE;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	if (pState == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> pState is NULL\033[0m\n");
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr = APK_MEDIA_PLAY_GetPlayState(ulViewId, &ePlayState);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_MEDIA_PLAY_GetPlayState() failed\n");
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	NXL_LOG_DBG("\033[1;31m @@@@@ MEDIACTRL PlayState = %d\033[0m\n", ePlayState);
	switch(ePlayState)
	{
		case eOxMP_PLAYSTATE_NONE:
		case eOxMP_PLAYSTATE_STOPPING:
		case eOxMP_PLAYSTATE_STOPPED:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Stop;
			break;
		case eOxMP_PLAYSTATE_PLAYING:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Playing;
			break;
		case eOxMP_PLAYSTATE_PAUSE:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Paused;
			break;
		case eOxMP_PLAYSTATE_EOS:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_EndOfStream;
			break;
#if 0
		case eOxMP_PLAYSTATE_BUFFERING:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_BufferingUnderflow;
			break;
		/* case eOxMP_PLAYSTATE_REQUESTED : */
		/* case eOxMP_PLAYSTATE_CONNECTING : */
#endif
		default:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Error;
			break;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_getBufferState(void *pState)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	OxMediaPlay_BufferState_e eBufferState = eOxMP_BUFFERSTATE_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	if (pState == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> pState is NULL\033[0m\n");
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr = APK_MEDIA_PLAY_GetBufferState(ulViewId, &eBufferState);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_MEDIA_PLAY_GetBufferState() failed\n");
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	NXL_LOG_DBG("\033[1;31m @@@@@ MEDIACTRL BufferState = %d\033[0m\n", eBufferState);
	switch(eBufferState)
	{
		case eOxMP_BUFFERSTATE_None:
			*((NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_None;
			break;
		case eOxMP_BUFFERSTATE_CanPlay:
			*((NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_CanPlay;
			break;
		case eOxMP_BUFFERSTATE_NotEnough:
			*((NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_NotEnough;
			break;
		case eOxMP_BUFFERSTATE_Enough:
			*((NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_Enough;
			break;
		case eOxMP_BUFFERSTATE_Buffering_Finished:
			*((NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_Buffering_Finished;
			break;
		default:
			*((NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_BUFFER_STATE_None;
			break;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_checkLastPlayCmd(NXL_PLAYBACKGROUP_CMD_e *pCmd)
{
	NXL_ERR nRet = NXL_ERR_OK;
	NXL_APKV2_Context_t *context = __getContext();

	//NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	if (pCmd == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> NULL arg\033[0m\n");
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}
	_LOCK_CTX(context);
	*pCmd =  context->lastPlaybackCmd;
	_UNLOCK_CTX(context);

FUNCTION_EXIT:
	//NXL_LOG_DBG("\033[1;36m @@@@@ lastPlaybackCmd:%d (-)\033[0m\n", (uint32_t)*pCmd);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_informLastPlayCmd(NXL_PLAYBACKGROUP_CMD_e eCmd)
{
	NXL_APKV2_Context_t *context = __getContext();
	_LOCK_CTX(context);
	context->lastPlaybackCmd = eCmd;
	_UNLOCK_CTX(context);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_mediapb_setDuration(double dblCurrentMaxPts)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	//OxMediaPlay_MediaType_e ePlayType;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr = APK_MEDIA_PLAY_SetDuration(ulViewId, dblCurrentMaxPts);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to set a duration, herr = %d\033[0m\n", herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_audioFlush(uint32_t unTryToChangeAudioTrack)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	//OxMediaPlay_MediaType_e ePlayType;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr = APK_MEDIA_PLAY_AudioFlush(ulViewId, unTryToChangeAudioTrack);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to flush an audio decoder buffer, herr = %d\033[0m\n", herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_audioRestart(void)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	//OxMediaPlay_MediaType_e ePlayType;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr = APK_MEDIA_PLAY_AudioRestart(ulViewId);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to restart an audio decoder\033[0m\n");
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_audioCodecSwitch(uint32_t unTryToChangeAudioTrack, int32_t nAudioCodec) // TODO for appkit_v2
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	//OxMediaPlay_MediaType_e ePlayType;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr = APK_MEDIA_PLAY_AudioCodecSwitch(ulViewId, unTryToChangeAudioTrack, nAudioCodec);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to switch an audio codec\033[0m\n");
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_notifyStreamCurrentPAR(uint32_t pixelAspectRatioX, uint32_t pixelAspectRatioY) /* VideoDecoderNative */
{
	NXL_APKV2_Context_t *context = __getContext();
	/* NXL_LOG_ERR("[pixelAspectRatioX:%d, pixelAspectRatioY:%d]\n", pixelAspectRatioX, pixelAspectRatioY); */

	_LOCK_CTX(context);
	context->streamPAR_X = pixelAspectRatioX;
	context->streamPAR_Y = pixelAspectRatioY;
	_UNLOCK_CTX(context);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_mediapb_notifyStreamCurrentSize(uint32_t imageWidth, uint32_t imageHeight) /* VideoDecoderNative */
{
	NXL_APKV2_Context_t *context = __getContext();

	_LOCK_CTX(context);
	if ((imageWidth != context->streamCurrentWidth) ||
		(imageHeight != context->streamCurrentHeight))
	{
		NXL_LOG_ERR("Changed streamImage[w:%d, h:%d]\n", imageWidth, imageHeight);
	}

	context->streamCurrentWidth  = imageWidth;
	context->streamCurrentHeight = imageHeight;
	_UNLOCK_CTX(context);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_mediapb_notifyStreamMaxSize(uint32_t maxImageWidth, uint32_t maxImageHeight) /* PlaybackDevice::initVideoPlayer */
{
	NXL_APKV2_Context_t *context = __getContext();

	_LOCK_CTX(context);
	if ((maxImageWidth != context->streamMaxWidth) ||
		(maxImageHeight != context->streamMaxHeight))
	{
		NXL_LOG_ERR("Changed maxImage[w:%d, h:%d]\n", maxImageWidth, maxImageHeight);
	}

	context->streamMaxWidth  = maxImageWidth;
	context->streamMaxHeight = maxImageHeight;
	_UNLOCK_CTX(context);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv2_mediapb_resetVideoSize(void)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	//OxMediaPlay_MediaType_e ePlayType;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr = APK_MEDIA_PLAY_ResetVideoSize(ulViewId);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to set video window size\033[0m\n");
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_setVideoSize(uint32_t ulPosX, uint32_t ulPosY, uint32_t ulWidth, uint32_t ulHeight)
{
	NXL_APKV2_Context_t *context = __getContext();
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0x0;
	OxMediaPlay_MediaType_e ePlayType = eOxMP_MEDIATYPE_MEDIAPLAY;
	OxMediaPlay_VideoSize_t stVideoSize = {0, };
	HINT32 uTickDiff = 0x0;

	NXL_LOG_DBG("\033[1;36m @@@@@ (+) - [x:%d, y:%d, w:%d, h:%d]\033[0m\n",
			ulPosX, ulPosY, ulWidth, ulHeight);

	herr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("Error> failed to getMainViewId!\n");
		return NXL_ERR_FAIL;
	}

	if (0) /* unused, sometimes appkit context's invalid status */
	{
		herr = APK_MEDIA_PLAY_GetPlayType(ulViewId, &ePlayType);
		if ((herr != ERR_OK) || ((eOxMP_MEDIATYPE_MEDIAPLAY != ePlayType) && (eOxMP_MEDIATYPE_MEDIAPLAY != ePlayType)))
		{
			NXL_LOG_ERR("Error> invalid playback[err:%d, type:%d]!\n", herr, ePlayType);
			return NXL_ERR_FAIL;
		}
	}

	_LOCK_CTX(context);

	/* Prevent duplicated videoResize() command to obama while resizing */
	if ((context->lastVideoResizeTick) &&
		((context->lastVideoResizeRect.x == ulPosX) &&
		(context->lastVideoResizeRect.y == ulPosY) &&
		(context->lastVideoResizeRect.width == ulWidth) &&
		(context->lastVideoResizeRect.height == ulHeight)))
	{
		uTickDiff = (HLIB_STD_GetSystemTick() - context->lastVideoResizeTick);
		if ((uTickDiff > 0x0) && (uTickDiff < _VIDEO_RESIZE_CHECK_INTERVAL))
		{
			NXL_LOG_ERR("Error> Too short interval[diff:%d, last:%d] for (%d,%d,%d,%d)!\n",
						uTickDiff, context->lastVideoResizeTick,
						ulPosX, ulPosY, ulWidth, ulHeight);
			goto func_done;
		}
	}

	/* Check exception cases */
	if (!ulPosX && !ulPosY && !ulWidth && !ulHeight)
	{
		/* (0,0,0,0),ref(w:1280, h:720) --> Full screen at Step#8--step#14 in DPI-040-TC9 */
		NXL_LOG_ERR("\033[1;36m @@@@@ --> resetVideoSize(0,0,0,0) -- ref[w:%d, h:%d]\033[0m\n",
					context->lastOutputResolutionWidth, context->lastOutputResolutionHeight);
		nRet = nxl_apkv2_mediapb_resetVideoSize();
		if (nRet == NXL_ERR_OK) { context->lastVideoResizeTick = HLIB_STD_GetSystemTick(); }
		goto func_done;
	}

	/* Resize videoWindow */
	if (0)
	{
		/* Removed because of NTS 2DVideoResize error for 1080p - DPI-040-TC6/DPI-175-TC4 */
		herr = APK_MEDIA_PLAY_SetVideoSize(ulViewId, ulPosX, ulPosY, ulWidth, ulHeight);
	}
	else
	{
		HxSTD_MemSet(&stVideoSize, 0x0, sizeof(OxMediaPlay_VideoSize_t));
		stVideoSize.nStartX          = ulPosX;
		stVideoSize.nStartY          = ulPosY;
		stVideoSize.nWidth           = ulWidth;
		stVideoSize.nHeight          = ulHeight;
		stVideoSize.bRoundOff        = TRUE;
#if defined(CONFIG_PROD_HDR3000T)  || defined(CONFIG_PROD_HMS1000T) /* TODO : Remove product config dependency */
		stVideoSize.bForceSourceBase = TRUE;
#elif defined(CONFIG_PROD_FVP4000T) || defined(CONFIG_PROD_HDR4000T)
		stVideoSize.bForceSourceBase = TRUE;
#else
		/* Not implemented... */
		/* TODO : Remove product config dependency */
#endif

		if (context->lastOutputResolutionWidth)
		{
			stVideoSize.nMaxWidth = context->lastOutputResolutionWidth;
		}
		else
		{
			stVideoSize.nMaxWidth = 1280; /* WEB max size is 1280x720 */
		}

		if (context->lastOutputResolutionHeight)
		{
			stVideoSize.nMaxHeight = context->lastOutputResolutionHeight;
		}
		else
		{
			stVideoSize.nMaxHeight = 720; /* WEB max size is 1280x720 */
		}

		NXL_LOG_ERR("--> ResizeVideoWindow(x:%d, y:%d, w:%d, h:%d) -- ref[w:%d, h:%d]\n",
					ulPosX, ulPosY, ulWidth, ulHeight,
					stVideoSize.nMaxWidth, stVideoSize.nMaxHeight);
		herr = APK_MEDIA_PLAY_ResizeVideoWindow(ulViewId, &stVideoSize); /* For supporting 2DVideoResize - 1080p DPI-040-TC6/DPI-175-TC4 */
		if (herr == ERR_OK) { context->lastVideoResizeTick = HLIB_STD_GetSystemTick(); }
	}

func_done:
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to set video window size\033[0m\n");
		nRet = NXL_ERR_FAIL;
	}
	else
	{
		context->lastVideoResizeRect.x = ulPosX;
		context->lastVideoResizeRect.y = ulPosY;
		context->lastVideoResizeRect.width = ulWidth;
		context->lastVideoResizeRect.height = ulHeight;
	}
	_UNLOCK_CTX(context);
	NXL_LOG_DBG("\033[1;36m @@@@@ (-)\033[0m\n");
	return nRet;
}

uint32_t nxl_apkv2_mediapb_setPlayerType(void)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	/* In the case of Netflix, player type is MSE in OBAMA */
	herr = APK_MEDIA_PLAY_SetPlayerType(ulViewId, eOxMP_MEDIAPLAY_PLAYERTYPE_MSE);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to set player type, herr = %d\033[0m\n", herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_setFade(uint32_t unStartVolume, uint32_t unTargetVolume, int32_t nEase, int32_t nEaseTimeElapsed, int32_t nEaseTransitionFrames)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __FUNCTION__);

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr = APK_MEDIA_PLAY_SetFade(ulViewId, unStartVolume, unTargetVolume, nEase, nEaseTimeElapsed, nEaseTransitionFrames);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Error> Failed to set player type, herr = %d\033[0m\n", herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __FUNCTION__);
	return nRet;
}

uint32_t nxl_apkv2_mediapb_getAudioDecoderVolume(uint32_t *punVolume)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR herr = ERR_OK;
	HUINT32 ulViewId = 0;

	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (+)\033[0m\n", __func__);

	if (punVolume == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Failed to set player type, herr = %d\033[0m\n", herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	*punVolume = 0x0;

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	herr = APK_MEDIA_PLAY_GetAudioDecoderVolume(ulViewId, punVolume);
	if (herr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ Failed to get audio decoder volume, herr = %d\033[0m\n", herr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ %s() (-)\033[0m\n", __func__);
	return nRet;
}

static HCHAR* local_apkv2_getStringFromValue(EnumStringPair_t *pstPairList, const HUINT32 ulValue)
{
	while (pstPairList->pszValue)
	{
		if (pstPairList->ulValue == ulValue)
			return pstPairList->pszValue;

		pstPairList++;
	}

	return "Unknown";
}

static HUINT32 local_apkv2_getValueFromString(EnumStringPair_t *pstPairList, const HCHAR *pszData)
{
	while (pstPairList->pszValue)
	{
		if (HxSTD_StrCmp(pstPairList->pszValue, pszData) == 0)
			return pstPairList->ulValue;

		pstPairList++;
	}

	return pstPairList->ulValue;
}

