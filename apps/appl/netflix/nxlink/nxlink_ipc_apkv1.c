
#include <stdio.h>
#include <stdlib.h>

#include <nxlink.h>
#include "nxlink_ipc_apkv1.h"
#include <htype.h>
#include <vkernel.h>
#include <hlib.h>
#include <hapi.h>
#include <oapi.h>
#include <appkit.h>


/******************************************************************
	Define
******************************************************************/
#undef		HxLOG_DOMAIN
#define		HxLOG_DOMAIN				"Netflix"

#define	__APKV1_OCTO_INCOMPLETED_WORKING__      0 /* Removed when completing the OCTO interaction */
#define _APKV1_PROC_EVENT_TASK_PRIORITY         (40)
#define _APKV1_PROC_EVENT_TASK_SIZE             (0x4000)
#define _MAX_VIDEO_WINDOW_WIDTH                 (1280)
#define _MAX_VIDEO_WINDOW_HEIGHT                (720)


typedef enum tTaskStatus
{
	NXL_TASK_STATUS_TERMINATED=0,  /* Task terminated */
	NXL_TASK_STATUS_HALTING,       /* Prepare task halt */
	NXL_TASK_STATUS_READY,         /* Ready status with looping */
	NXL_TASK_STATUS_RUNNING,       /* Looping after oapi task start */
}eNXL_TaskStatus_e;

typedef	struct	_NXL_APKV1_Info_tag
{
	uint32_t           oapiInited;
	unsigned long	   tTaskId;
	eNXL_TaskStatus_e  taskStatus;
	uint32_t           requestExit;
	uint32_t           hasBoxInfoListener;
	uint32_t           liveStarted;

	uint32_t           gotVolumeInfo;
	uint32_t           volumeCur;
	uint32_t           volumeMax;
	uint32_t           volumeMin;
	uint32_t           volumeMuted;

	/* system event
	 * https://nrd.netflix.com/display/DOC42/System+Capabilities */
	uint32_t                lastOutputVolumeChanged;     // nrd->outputVolumeChanged
	APKE_HdcpVer_e          lastOutputHdcpVersion;
	uint32_t                lastOutputHdmiChanged;       // nrd->videoOutputStatusChanged
	uint32_t                lastOutputResolutionWidth;
	uint32_t                lastOutputResolutionHeight;
	APKE_ResolutionSelect_e lastSelectedResolution;      /* APKE_OutputResolution_e */
	uint32_t                lastOutputResolutionChanged; // nrd->videoOutputResolutionChanged
	uint32_t                lastScreensaverChanged;      // nrd->screensaverStateChanged
	uint32_t                lastViewModeChanged;         // nrd->viewModeChanged
	uint32_t                lastLanguageChanged;         // nrd->languageChanged
	uint32_t                lastNetworkChanged;          // nrd->networkChanged

	/* playback and netflix:nrd's videoAttributes */
	NXL_PLAYBACKGROUP_CMD_e lastPlaybackCmd;
	uint32_t                streamPAR_X;                 // nrd->PixelAspectRatio X
	uint32_t                streamPAR_Y;                 // nrd->PixelAspectRatio Y
	uint32_t                streamCurrentWidth;          // nrd->sampleAttr->VideoAttributes->ImageWidth
	uint32_t                streamCurrentHeight;         // nrd->sampleAttr->VideoAttributes->ImageHeight
	uint32_t                streamMaxWidth;              // nrd->PlaybackDevice::initVideoPlayer(VideoAttributes *videoAttr)
	uint32_t                streamMaxHeight;             // nrd->PlaybackDevice::initVideoPlayer(VideoAttributes *videoAttr)

	eNXL_PWR_STATUS	          powerStatus;
	NXL_PowerStateChange_t         pfnPowerChangeCallback;
	APKS_MEDIADEC_RESOURCE_MAP_t  stResourceMap;
} NXL_APKV1_Info_t;

/******************************************************************
	Static variables
******************************************************************/
static uint32_t s_bApkv1Inited = FALSE;
static NXL_APKV1_Info_t s_stApkv1Info;
static HUINT8 tmpbuf[_NXL_BLEN_64] = {0, };

static APKS_Pair_t  _stAudioSoundModeList[] = {
	{eDxAUDIO_CONFIG_STEREO,    "Stereo" },
	{eDxAUDIO_CONFIG_MONOLEFT,  "Left"   },
	{eDxAUDIO_CONFIG_MONORIGHT, "Right"  },
	{eDxAUDIO_CONFIG_MIX,       "Mix"    },
	{eDxAUDIO_CONFIG_UNKNOWN,   NULL     }
};

static APKS_Pair_t  _stDigitalOutList[] = {
	{eDigitalOut_DownMix_PCM,     "PCM"           },
	{eDigitalOut_ByPass_All,      "Pass through"  },
	{eDigitalOut_ByPass_Specific, "Pass Spec"     },
	{eDigitalOut_Unknown,         NULL            }
};

static APKS_Pair_t  _astAspectRatioPairList[] = {
	{eAspectRatio_4x3,      "4:3"},
	{eAspectRatio_16x9,     "16:9"},
	{eAspectRatio_14x9,     "14:9"},
	{eAspectRatio_Unknown,  "Unknown"},
	{eAspectRatio_Unknown,  NULL}
};

static APKS_Pair_t  _astResolutionSelectPairList[] = {
	{eRESSELECT_720x480I,        "480i"},
	{eRESSELECT_720x480P,        "480p"},
	{eRESSELECT_720x576I,        "576i"},
	{eRESSELECT_720x576P,        "576p"},
	{eRESSELECT_1080x720P,       "720p"},
	{eRESSELECT_1920x1080I,      "1080i"},
	{eRESSELECT_1920x1080P,      "1080p"},
	{eRESSELECT_FOLLOW_SOURCE,   "Original"},
	{eRESSELECT_EDID_MAX,        "Native"},
	{eRESSELECT_D1,              "D1"},
	{eRESSELECT_D2,              "D2"},
	{eRESSELECT_D3,              "D3"},
	{eRESSELECT_D4,              "D4"},
	{eRESSELECT_UNKNOWN,         "Unknown"},
	{eRESSELECT_UNKNOWN,         NULL}
};

static APKS_Pair_t _astVideoStandardPairList[] =
{
	{eVideoStandard_PAL,       "PAL"},
	{eVideoStandard_NTSC,      "NTSC"},
	{eVideoStandard_SECAM,     "SECAM"},
	{eVideoStandard_Unknown,   "Unknown"},
	{eVideoStandard_Unknown,    NULL}
};

uint32_t nxl_apkv1_sys_getModelName(char *pBuf, uint32_t size);
uint32_t nxl_apkv1_sys_getSoftwareVersion(char *pBuf, uint32_t size);
uint32_t nxl_apkv1_sys_getSystemID(char *pszID);
uint32_t nxl_apkv1_sys_getScreenLanguage(char *pszLanguage, uint32_t size);
uint32_t nxl_apkv1_output_getOutputResolution(uint32_t *pWidth, uint32_t *pHeight);

/******************************************************************
	Static functions
******************************************************************/

static void local_v1_outputAudioEventCallback(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	uint32_t bMute;
	NXL_LOG_WRN("++[%s]++\n",__FUNCTION__);
	if (APK_SQCAUDIO_isSytemMuted(&bMute) == ERR_OK)
	{
		s_stApkv1Info.volumeMuted = bMute;
		s_stApkv1Info.lastOutputVolumeChanged = 1;
	}
	NXL_LOG_WRN("--[%s]--\n",__FUNCTION__);
}

uint32_t nxl_apkv1_output_checkLastOutputVolumeChanged(uint32_t *pUpdated)
{
	*pUpdated = s_stApkv1Info.lastOutputVolumeChanged;
	s_stApkv1Info.lastOutputVolumeChanged = 0;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_registerOutputVolumeListener(void *pFunc)
{
	APK_SQCAUDIO_RegisterNotifier((CBObjectHandler)pFunc);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_unregisterOutputVolumeListener(void *pFunc)
{
	APK_SQCAUDIO_UnRegisterNotifier((CBObjectHandler)pFunc);
	return NXL_ERR_OK;
}

static void local_v1_outputHdmiEventCallback(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	uint32_t ret = NXL_ERR_OK;
	APKE_HDMIHotPlugStatus_e eHotplugStatus;
	APKE_HdcpVer_e eApkHdcpVer = eHDCP_VERSION_MAX;
	NXL_LOG_WRN("++[%s]++\n",__FUNCTION__);

	if (APK_SQCHdmi_GetHotPlugStatus(&eHotplugStatus) == ERR_OK)
	{
		if (eHotplugStatus == eHDMI_HOTPLUG_NONE)
		{
			NXL_LOG_DBG("[outputHdmiEventCallback] [status:eHDMI_HOTPLUG_NONE)\n");
		}
		else if (eHotplugStatus == eHDMI_HOTPLUG_IN)
		{
			NXL_LOG_DBG("[outputHdmiEventCallback] [status:eHDMI_HOTPLUG_IN)\n");
		}
		else if (eHotplugStatus == eHDMI_HOTPLUG_OUT)
		{
			NXL_LOG_DBG("[outputHdmiEventCallback] [status:eHDMI_HOTPLUG_OUT)\n");
		}
		s_stApkv1Info.lastOutputHdmiChanged = 1;
	}

	NXL_LOG_WRN("--[%s]--\n",__FUNCTION__);
}

uint32_t nxl_apkv1_output_checkLastOutputHdmiChanged(uint32_t *pUpdated)
{
	APKE_HdcpVer_e eApkHdcpVer = eHDCP_VERSION_MAX;

	if (APK_SQCOUTPUT_GetHdcpVersion(&eApkHdcpVer) == ERR_OK)
	{
		/* NXL_LOG_DBG("[checkLastOutputHdmiChanged] HDCP ver(%d -> %d)\n", s_stApkv1Info.lastOutputHdcpVersion,  eApkHdcpVer); */
		if (eApkHdcpVer != s_stApkv1Info.lastOutputHdcpVersion)
		{
			NXL_LOG_DBG("[checkLastOutputHdmiChanged] Changed HDCP ver(%d -> %d)\n", s_stApkv1Info.lastOutputHdcpVersion,  eApkHdcpVer);
			/* s_stApkv1Info.lastOutputHdmiChanged = 1; */
			s_stApkv1Info.lastOutputHdcpVersion = eApkHdcpVer;
			*pUpdated = 1;
			return NXL_ERR_OK;
		}
	}

	*pUpdated = s_stApkv1Info.lastOutputHdmiChanged;
	s_stApkv1Info.lastOutputHdmiChanged = 0;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_registerOutputHdmiListener(void *pFunc)
{
	APK_SQCHdmi_RegisterNotifier((CBObjectHandler)pFunc);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_unregisterOutputHdmiListener(void *pFunc)
{
	APK_SQCHdmi_UnRegisterNotifier((CBObjectHandler)pFunc);
	return NXL_ERR_OK;
}

static void local_v1_outputEventCallback(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	uint32_t ret = NXL_ERR_OK;
	APKE_ResolutionSelect_e eResolutionSelect;
	uint32_t resWidth = 0, resHeight = 0;
	NXL_LOG_WRN("++[%s]++\n",__FUNCTION__);
	
#if 0
	if (APK_SQCOUTPUT_GetResolutionSelect(&eResolutionSelect) == ERR_OK)
	{
		NXL_LOG_DBG("[outputEventCallback] eResolutionSelect:%d\n", eResolutionSelect);
	}
#else
	(void)nxl_apkv1_output_getOutputResolution(&resWidth, &resHeight);
#endif
	NXL_LOG_WRN("--[%s]--\n",__FUNCTION__);
}

static void local_v1_processEventTask(void *args)
{
	eNXL_TaskStatus_e prevStatus;
	NXL_LOG_DBG("++[%s]++\n",__FUNCTION__);

	if (s_stApkv1Info.taskStatus < NXL_TASK_STATUS_RUNNING)
	{
		s_stApkv1Info.taskStatus = NXL_TASK_STATUS_RUNNING;
	}
	prevStatus = s_stApkv1Info.taskStatus;

	while (1)
	{
		if ((s_stApkv1Info.taskStatus == NXL_TASK_STATUS_TERMINATED) ||
			(s_stApkv1Info.taskStatus == NXL_TASK_STATUS_HALTING))
		{
			prevStatus = s_stApkv1Info.taskStatus;
			NXL_LOG_WRN("task received halt event(status:%d)\n", s_stApkv1Info.taskStatus);
			goto func_exit;
		}

		if (s_stApkv1Info.requestExit == TRUE)
		{
			prevStatus = s_stApkv1Info.taskStatus;
			s_stApkv1Info.taskStatus = NXL_TASK_STATUS_TERMINATED;
			NXL_LOG_WRN("task loop status changed(%d -> %d)\n", prevStatus, s_stApkv1Info.taskStatus);
			goto func_exit;
		}


		OAPI_Process();

		VK_TASK_Sleep(10);
	}
	NXL_LOG_WRN("task loop terminated(%d -> %d)\n", prevStatus, s_stApkv1Info.taskStatus);

func_exit:
	if (s_stApkv1Info.taskStatus != NXL_TASK_STATUS_TERMINATED)
	{
		NXL_LOG_WRN("task status changed(%d -> %d)\n", prevStatus, s_stApkv1Info.taskStatus);
		s_stApkv1Info.taskStatus = NXL_TASK_STATUS_TERMINATED;
	}
	NXL_LOG_DBG("--[%s]--\n",__FUNCTION__);
	return;
}

static void local_v1_rcDialEventCallback(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	void	*pvGarbage = NULL;
	void	*pvGarbage2 = NULL;
	APKE_DialCtrl_Evt_e	eType = (APKE_DialCtrl_Evt_e)ulParam1;
#if 0
	if(eType == eDialApkEvt_ReqAppLaunch)
	{
		ulParam2  = (HUINT32)HxSTD_StrDup((HCHAR *)ulParam2);
		ulParam3  = (HUINT32)HxSTD_StrDup((HCHAR *)ulParam3);
		pvGarbage = (void *)ulParam2;
		pvGarbage2 = (void *)ulParam3;
	}
	else if(eType == eDialApkEvt_ReqAppStop)
	{
		ulParam2  = (HUINT32)HxSTD_StrDup((HCHAR *)ulParam2);
		ulParam3  = (HUINT32)HxSTD_StrDup((HCHAR *)ulParam3);
		pvGarbage = (void *)ulParam2;
		pvGarbage2 = (void *)ulParam3;
	}

	// Event_EmitV if eventListner is existed
#endif
}

/******************************************************************
	Interfaces
******************************************************************/
uint32_t nxl_apkv1_init(NXL_BoxInfo_t *pstBoxInfo)
{
	HBOOL bPvrEnabled = FALSE;
	uint32_t retryCnt = 10;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;
	APKE_MODULE_e eModule = eModule_NONE;
	HERROR hError = ERR_OK;
	NXL_LOG_DBG("++[%s]++\n",__FUNCTION__);

	if (pstBoxInfo == NULL)
	{
		NXL_LOG_ERR("Error> NULL arg!\n");
		return NXL_ERR_OK;
	}

	HxSTD_MemSet(&s_stApkv1Info, 0, sizeof(NXL_APKV1_Info_t));
	s_stApkv1Info.requestExit            = FALSE;
	s_stApkv1Info.tTaskId                = 0x0;
	s_stApkv1Info.taskStatus             = NXL_TASK_STATUS_TERMINATED;
	s_stApkv1Info.pfnPowerChangeCallback = NULL;

	VK_Init();
	OAPI_Init();

#if 0 /* oapi_manager crash when call APK_DeInit()  */
	APK_Init(eModule_ALL);
	APK_SetInitialValue(eModule_ALL);
#else
	APK_Init(eModule_SQC | eSQC_AUDIO | eSQC_OUTPUT | eSQC_SCENARIO | eSQC_DMS | eSQC_HDMI | eSQC_NETWORK | eSQC_BOXINFO | eSQC_VIDEO ); /* eSQC_MS, eSQC_SECURITY */
	APK_Init(eModule_UDB | eModule_MCTRL | eModule_OPERATOR | eSubModule_MASK);

	APK_SetInitialValue(eModule_SQC | eSQC_AUDIO | eSQC_OUTPUT | eSQC_SCENARIO | eSQC_DMS | eSQC_HDMI | eSQC_NETWORK | eSQC_BOXINFO | eSQC_VIDEO);
	APK_SetInitialValue(eModule_UDB | eModule_MCTRL | eModule_OPERATOR | eSubModule_MASK);
#endif

#if 0	/* TODO : Temporary code for playback test, modified by thoh */
	if (__APKV1_OCTO_INCOMPLETED_WORKING__)
	{
		/* Temp code : When netflix is launched, live should be stopped. */
		eDest = APK_MEDIACTRL_GetMainViewDestination();
		APK_MEDIACTRL_LIVE_Stop (eDest);
	}
#endif

	/* create the thread for Appkit */
	s_stApkv1Info.taskStatus = NXL_TASK_STATUS_READY;
	VK_TASK_Create(local_v1_processEventTask, _APKV1_PROC_EVENT_TASK_PRIORITY, _APKV1_PROC_EVENT_TASK_SIZE,
			"local_v1_processEventTask", NULL, (unsigned long *)&s_stApkv1Info.tTaskId, 1);
	VK_TASK_Start(s_stApkv1Info.tTaskId);

	if (0) /* Waiting octo ready */
	{
		while ( retryCnt-- )
		{
			APK_SQCSCENARIO_GetSupportInternalPVR(&bPvrEnabled); /* Or wait untill finishing /mnt/hd2 mount. */
			if (bPvrEnabled == TRUE) break;
			usleep(1000*1000);
		}
	}

	/* Initialize stream videoAttributes */
	s_stApkv1Info.lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_NA;
	s_stApkv1Info.streamPAR_X = s_stApkv1Info.streamPAR_Y = 0;
	s_stApkv1Info.streamCurrentWidth = s_stApkv1Info.streamCurrentHeight = 0;
	s_stApkv1Info.streamMaxWidth = s_stApkv1Info.streamMaxHeight = 0;

	/* System Volume, Output(HDMI, SPDIF, ...) ... */
	{
		s_stApkv1Info.gotVolumeInfo = TRUE;
		if (APK_SQCAUDIO_GetVolume(&s_stApkv1Info.volumeCur) != ERR_OK)
		{
			NXL_LOG_ERR("Getting the volume level is failed.\n");
			s_stApkv1Info.gotVolumeInfo = FALSE;
		}
		if (APK_SQCAUDIO_GetVolumeMax(&s_stApkv1Info.volumeMax) != ERR_OK)
		{
			NXL_LOG_ERR("Getting the volume max is failed.\n");
			s_stApkv1Info.gotVolumeInfo = FALSE;
		}
		if (APK_SQCAUDIO_GetVolumeMin(&s_stApkv1Info.volumeMin) != ERR_OK)
		{
			NXL_LOG_ERR("Getting the volume min is failed.\n");
			s_stApkv1Info.gotVolumeInfo = FALSE;
		}
		if (APK_SQCAUDIO_isSytemMuted(&s_stApkv1Info.volumeMuted) != ERR_OK)
		{
			NXL_LOG_ERR("Getting the volume mute is failed.\n");
			s_stApkv1Info.gotVolumeInfo = FALSE;
		}

		s_stApkv1Info.lastOutputVolumeChanged     = FALSE; /* APK_SQCAUDIO_RegisterNotifier */
		s_stApkv1Info.lastOutputHdcpVersion       = eHDCP_VERSION_MAX;
		s_stApkv1Info.lastOutputHdmiChanged       = FALSE; /* APK_SQCHdmi_RegisterNotifier */

		s_stApkv1Info.lastOutputResolutionWidth   = 0;
		s_stApkv1Info.lastOutputResolutionHeight  = 0;
		s_stApkv1Info.lastSelectedResolution      = eRESSELECT_UNKNOWN; /* TODO : */
		s_stApkv1Info.lastOutputResolutionChanged = FALSE; /* TODO : */
		s_stApkv1Info.lastScreensaverChanged      = FALSE; /* TODO : Changed with product profile; no screensaver */
		s_stApkv1Info.lastViewModeChanged         = FALSE; /* TODO : Changed with product profile; Unused because of being fixed with 2D (no 3D model) */
		s_stApkv1Info.lastLanguageChanged         = FALSE; /* TODO : in developing */
		s_stApkv1Info.lastNetworkChanged          = FALSE; /* TODO : currently use partner/dpi network timer for eth0 and wlan0 */

		if (1) s_stApkv1Info.lastSelectedResolution = APK_UDB_GetValue((HUINT8 *)USERDB_VIDEO_RESOLUTION);
		else APK_SQCOUTPUT_GetResolutionSelect(&s_stApkv1Info.lastSelectedResolution);

		/* Register Volume, HDMI, ... */
		APK_SQCAUDIO_RegisterNotifier(local_v1_outputAudioEventCallback);
		APK_SQCHdmi_RegisterNotifier(local_v1_outputHdmiEventCallback);
		APK_SQCOUTPUT_RegisterNotifier(local_v1_outputEventCallback);

		// APK_SQCBOXINFO_RegisterNotifier
		// APK_SQCSCENARIO_RegisterNotifier
		// APK_SQCNetwork_RegisterNotifier
		// APK_SQCVIDEO_RegisterNotifier

		APK_DIAL_RegisterNotifier(local_v1_rcDialEventCallback);
		APK_DIAL_SetUpdateInfo();
	}

	memset(pstBoxInfo, 0x0, sizeof(NXL_BoxInfo_t));
	{
		pstBoxInfo->model_name = NULL;
		pstBoxInfo->software_version = NULL;
		pstBoxInfo->screen_language = NULL;
		pstBoxInfo->system_id = NULL;
	}

	memset(tmpbuf, 0, sizeof(tmpbuf));
	if (1)
	{
		hError = nxl_apkv1_sys_getModelName(tmpbuf, _NXL_BLEN_64);
		if (hError == ERR_OK)
		{
			if ( pstBoxInfo->model_name ) free(pstBoxInfo->model_name);
			pstBoxInfo->model_name = strdup(tmpbuf);
			NXL_LOG_WRN("[%s:%d] -- [model_name:%s]\n", __func__, __LINE__, tmpbuf);
		}
		else
		{
			pstBoxInfo->model_name = NULL;
		}

		hError = nxl_apkv1_sys_getSoftwareVersion(tmpbuf, _NXL_BLEN_64);
		if (hError == ERR_OK)
		{
			if ( pstBoxInfo->software_version ) free(pstBoxInfo->software_version);
			NXL_LOG_WRN("[%s:%d] -- [software_version:%s]\n", __func__, __LINE__, tmpbuf);
			pstBoxInfo->software_version = strdup(tmpbuf+7);
			NXL_LOG_WRN("[%s:%d] -- [software_version:%s]\n", __func__, __LINE__, tmpbuf);
		}
		else
		{
			pstBoxInfo->software_version = NULL;
		}

		hError = nxl_apkv1_sys_getScreenLanguage(tmpbuf, _NXL_BLEN_64);
		if (hError == ERR_OK)
		{
			if ( pstBoxInfo->screen_language ) free(pstBoxInfo->screen_language);
			pstBoxInfo->screen_language = strdup(tmpbuf);
			NXL_LOG_WRN("[%s:%d] -- [screen_language:%s]\n", __func__, __LINE__, tmpbuf);
		}
		else
		{
			pstBoxInfo->screen_language = NULL;
		}

		hError = nxl_apkv1_sys_getSystemID(tmpbuf);
		if (hError == ERR_OK)
		{
			if ( pstBoxInfo->system_id ) free(pstBoxInfo->system_id);
			pstBoxInfo->system_id = strdup(tmpbuf);
			NXL_LOG_WRN("[%s:%d] -- [system_id:%s]\n", __func__, __LINE__, tmpbuf);
		}
		else
		{
			pstBoxInfo->system_id = NULL;
		}
	}

	s_stApkv1Info.oapiInited = TRUE;
	s_bApkv1Inited = TRUE;
	NXL_LOG_DBG("--[%s]--\n",__FUNCTION__);
	return NXL_ERR_OK;
}

void nxl_apkv1_deinit(void)
{
	uint32_t uLoopCnt = 0;
	uint32_t uWaitCnt = 10;
	int32_t nVkErr = VK_OK;
	
	NXL_LOG_DBG("++[%s]++\n",__FUNCTION__);

	APK_SQCAUDIO_UnRegisterNotifier(local_v1_outputAudioEventCallback);
	APK_SQCHdmi_UnRegisterNotifier(local_v1_outputHdmiEventCallback);
	APK_SQCOUTPUT_UnRegisterNotifier(local_v1_outputEventCallback);
	APK_DIAL_UnRegisterNotifier(local_v1_rcDialEventCallback);

	s_stApkv1Info.requestExit = TRUE;

	/* Stop oapi task itself */
	uLoopCnt = 0;
	if (s_stApkv1Info.taskStatus > NXL_TASK_STATUS_HALTING) /* including RUNNING */
	{
//		s_stApkv1Info.taskStatus = NXL_TASK_STATUS_HALTING;

		/* Wait for stopping oapi task */
		while(1)
		{
			if (s_stApkv1Info.taskStatus == NXL_TASK_STATUS_TERMINATED)
			{
				/* Start the encoding and feeding */
				break;
			}

			if (uLoopCnt > uWaitCnt)
			{
				NXL_LOG_WRN("Too many looping(%d) for task termination(status:%d)\n", uLoopCnt, s_stApkv1Info.taskStatus);
				break;
			}

			VK_TASK_Sleep(10);
			uLoopCnt++;
		}
	}
	
	nVkErr = VK_TASK_Destroy(s_stApkv1Info.tTaskId);
	if(nVkErr != VK_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to destroy VK_TASK : TaskId = %lu, nVkErr = %d\033[0m\n", __func__, __LINE__, s_stApkv1Info.tTaskId, nVkErr);
	}

	/* Initialize stream videoAttributes */
	s_stApkv1Info.lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_NA;
	s_stApkv1Info.streamPAR_X = s_stApkv1Info.streamPAR_Y = 0;
	s_stApkv1Info.streamCurrentWidth = s_stApkv1Info.streamCurrentHeight = 0;
	s_stApkv1Info.streamMaxWidth = s_stApkv1Info.streamMaxHeight = 0;
	s_stApkv1Info.taskStatus = NXL_TASK_STATUS_TERMINATED;
	if (0) APK_DeInit();
	s_stApkv1Info.oapiInited = FALSE;
	NXL_LOG_DBG("--[%s]--\n",__FUNCTION__);
}

uint32_t nxl_apkv1_sys_getApplicationID(uint32_t *pAppId) // TODO
{
	__CHECK_PARAM_PTR(pAppId);
	*pAppId = (uint32_t)eAWINDOW_NETFLIX;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_setInputGrap(uint32_t bOn)
{
	NXL_LOG_DBG("\n[nxlink] %s(). bOn(%s)", __FUNCTION__, bOn ? "TRUE" : "FALSE");
#if 0 /* In HDR-1000S, HAPI KeyGrab() cause the change of HWStyle and key froze-up for netflix relaunch */
	HAPI_SendKeyGrab(eAWINDOW_NETFLIX, (bOn ? TRUE : FALSE));
#endif
	return NXL_ERR_OK;
}

/* System */
uint32_t nxl_apkv1_sys_getModelName(char *pBuf, uint32_t size)
{
	__CHECK_PARAM_PTR(pBuf);

	APK_SQCBOXINFO_GetItem(eBoxInfo_Model_Name, pBuf);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_getSoftwareVersion(char *pBuf, uint32_t size)
{
	__CHECK_PARAM_PTR(pBuf);

	APK_SQCBOXINFO_GetItem(eBoxInfo_Software_Version, pBuf);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_getLoaderVersion(char *pszLoaderVer)
{
	__CHECK_PARAM_PTR(pszLoaderVer);

	if (APK_SQCBOXINFO_GetItem(eBoxInfo_Loader_Version, pszLoaderVer) != ERR_OK)
	{
		NXL_LOG_ERR("Error> Getting the device ID is failed.\n");
		return NXL_ERR_FAIL;
	}
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_getPvrEnabled(uint32_t *pbEnabled) /* Will be used for decision whether octo is ready */
{
	uint32_t supportInternalPVR, supportExternalPVR;
	__CHECK_PARAM_PTR(pbEnabled);

	APK_SQCSCENARIO_GetSupportInternalPVR(&supportInternalPVR);
	APK_SQCSCENARIO_GetSupportExternalPVR(&supportExternalPVR);

	if (supportInternalPVR || supportExternalPVR) *pbEnabled = TRUE;
	else *pbEnabled = FALSE;

	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_getSystemID(char *pszID)
{
	__CHECK_PARAM_PTR(pszID);

	if (APK_SQCBOXINFO_GetItem(eBoxInfo_System_Id, pszID) != ERR_OK)
	{
		NXL_LOG_ERR("Error> Cant get the systemID!\n");
		return NXL_ERR_FAIL;
	}
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_getScreenLanguage(char *pszLanguage, uint32_t size)
{
	__CHECK_PARAM_PTR(pszLanguage);
	if (APK_SQCSCENARIO_GetMainLanguage(pszLanguage) != ERR_OK)
	{
		NXL_LOG_ERR("Error> Cant get the mainLanguage!\n");
		return NXL_ERR_FAIL;
	}

	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_getSubtitleFontType(uint32_t *pnSubtitleFontType)
{
	__CHECK_PARAM_PTR(pnSubtitleFontType);

	*pnSubtitleFontType = (uint32_t)APK_UDB_GetValue((HUINT8 *)USERDB_SUBTITLE_FONT);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_getPreferredAudioLanuage(char *pszLanguage)
{
#if 0
	HUINT32		ulValue;
	const		HCHAR *pszBuf;
#endif

	__CHECK_PARAM_PTR(pszLanguage);

	APK_SQCSCENARIO_GetAudioLanguage(pszLanguage, 0);
#if 0
	ulValue = APK_UDB_GetValue((HUINT8 *)USERDB_LANGUAGE_AUDIO_LANGUAGE);
	pszBuf = HxLANGUAGE_IdTo639((HxLanguage_e)ulValue);
	if (NULL == pszBuf)
	{
		pszLanguage[0] = '\0';
	}
	else
	{
		HxSTD_StrNCpy(pszLanguage,pszBuf, 3);
	}
#endif

	pszLanguage[3] = '\0';
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_getPreferredSubtitleLanuage(char *pszLanguage)
{
#if 0
	HUINT32		ulValue;
	const		HCHAR *pszBuf;
#endif
	__CHECK_PARAM_PTR(pszLanguage);

	APK_SQCSCENARIO_GetSubtitleLanguage(pszLanguage, 0);
#if 0
	ulValue = APK_UDB_GetValue((HUINT8 *)USERDB_LANGUAGE_SUBTITLE_LANGUAGE);
	pszBuf = HxLANGUAGE_IdTo639((HxLanguage_e)ulValue);
	if (NULL == pszBuf)
	{
		pszLanguage[0] = '\0';
	}
	else
	{
		HxSTD_StrNCpy(pszLanguage,pszBuf, 3);
	}
#endif

	pszLanguage[3] = '\0';
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_getPreferredMenuLanuage(char *pszLanguage)
{
#if 0
	HUINT32		ulValue;
	const		HCHAR *pszBuf;
#endif
	__CHECK_PARAM_PTR(pszLanguage);

	APK_SQCSCENARIO_GetMainLanguage(pszLanguage);
#if 0
	ulValue = APK_UDB_GetValue((HUINT8 *)USERDB_LANGUAGE_SYSTEM_LANGUAGE);
	pszBuf = HxLANGUAGE_IdTo639((HxLanguage_e)ulValue);
	if (NULL == pszBuf)
	{
		pszLanguage[0] = '\0';
	}
	else
	{
		HxSTD_StrNCpy(pszLanguage,pszBuf, 3);
	}
#endif
	pszLanguage[3] = '\0';
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_getDialFriendlyName(char *pBuf, uint32_t size)
{
	HBOOL dialActivated = FALSE;
	/* HUINT8 szDialFriendlyName[33] = { 0, }; */
	
	__CHECK_PARAM_PTR(pBuf);

	HxSTD_MemSet(pBuf, 0, size);
	if ((APK_DIAL_GetDialStatus(&dialActivated) != ERR_OK) || (dialActivated != TRUE))
	{
		
		/* ("&lt;free time&gt; Remote Control Server") */
		if (APK_DIAL_SetUpdateInfo() == ERR_OK)
		{
			HxSTD_TaskSleep(50); /* Wait for updating dial's info in octo */
			(void)APK_DIAL_GetFriendlyName(pBuf);
		}
	}
	else
	{
		if ((APK_DIAL_GetFriendlyName(pBuf) == ERR_OK) && (HxSTD_StrLen(pBuf) > 0))
		{
			goto func_done;
		}
		
		/* ("&lt;free time&gt; Remote Control Server") */
		if (APK_DIAL_SetUpdateInfo() == ERR_OK)
		{
			HxSTD_TaskSleep(50); /* Wait for updating dial's info in octo */
			(void)APK_DIAL_GetFriendlyName(pBuf);
		}
	}
	
func_done:
	NXL_LOG_WRN("[%s] friendlyName(%s)\n",__FUNCTION__, pBuf);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_sys_isScreensaverOn(uint32_t *onScreenSaver) /* TODO: Screen Saver */
{
	*onScreenSaver = 0;
	return NXL_ERR_UNSUPPORTED;
}

#define ____NXL_Volume_Related_Functions____

/* Audio Volume Control */
uint32_t nxl_apkv1_output_getVolumePercent(uint32_t *pPercent)
{
	int32_t max = 0, min = 0;
	int32_t volume = 0;
	int32_t unit = 0;

	__CHECK_PARAM_PTR(pPercent);

	APK_SQCAUDIO_GetVolumeMax(&max);
	APK_SQCAUDIO_GetVolumeMin(&min);
	APK_SQCAUDIO_GetVolume(&volume);
	unit = max-min ;
	unit = unit <= 0 ? volume : unit;

	/*volume scope :  0 ~ 100 */
	*pPercent = (uint32_t)((volume / (float)unit) * 100);
	NXL_LOG_WRN("[%s] GetVolumePercent(volume:%d, percent:%d)\n",__FUNCTION__, volume, *pPercent);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_getVolumeLevel(uint32_t *pnVolume)
{
	HINT32	nVolume;
	__CHECK_PARAM_PTR(pnVolume);

	// APK_SQCAUDIO_GetVolume((HINT32*)pnVolume);
	// *pnVolume = APK_UDB_GetValue((unsigned char *)USERDB_SURFER_VOLUME_LEVEL);
	{
		APK_SQCAUDIO_GetVolume(&nVolume);
		*pnVolume = nVolume;
	}
	NXL_LOG_WRN("[%s] GetVolumeLevel(volume:%d)\n",__FUNCTION__, nVolume);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_setVolumePercent(uint32_t percent)
{
	int32_t max = 0, min = 0;
	int32_t volume = 0;
	int32_t unit = 0;

	if (percent > 100)
		return NXL_ERR_INVALID_PARAM;

	APK_SQCAUDIO_GetVolumeMax(&max);
	APK_SQCAUDIO_GetVolumeMin(&min);
	APK_SQCAUDIO_GetVolume(&volume);
	unit = max-min ;
	unit = unit <= 0 ? 1 : unit;

	/*volume scope :  0 ~ 100 */
	if (percent)
		volume = (uint32_t)((percent / (float)100.0f) * unit);

	NXL_LOG_WRN("[%s] SetVolumePercent(volume:%d, percent:%d)\n",__FUNCTION__, volume, percent);
	APK_SQCAUDIO_SetVolume(volume);

	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_setVolumeLevel( uint32_t nVolume)
{
	/*
	if (nVolume)
	{
		uint32_t	bMute;

		APK_SQCAUDIO_isSytemMuted(&bMute);
		if (bMute)
		{
			APK_SQCAUDIO_SetSystemMute(FALSE);
			APK_UDB_SetValue((HUINT8 *)USERDB_AUDIO_MUTE, (HUINT32)FALSE);
		}
	}
	*/
#if 0
	APK_SQCAUDIO_SetVolume(nVolume);
	APK_UDB_SetValue((HUINT8 *)USERDB_SURFER_VOLUME_LEVEL, (HUINT32)nVolume);
#endif
	NXL_LOG_WRN("[%s] setVolumeLevle(volume:%d)\n",__FUNCTION__, nVolume);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_getVolumeStep(uint32_t *pStep)
{
	int32_t max = 0, min = 0;
	int32_t unit = 0;

	__CHECK_PARAM_PTR(pStep);

	APK_SQCAUDIO_GetVolumeMax(&max);
	APK_SQCAUDIO_GetVolumeMin(&min);
	NXL_LOG_DBG("[%s] volume max(%d)\n",__func__, max);
	NXL_LOG_DBG("[%s] volume min(%d)\n",__func__, min);

	unit = max-min ;
	unit = unit <= 0 ? 1 : unit;
	*pStep = 1 * 100 / unit ; // ++1, --1

	NXL_LOG_DBG("[%s] volume step(%d)\n",__func__, *pStep);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isMuted(uint32_t *pMute)
{
	__CHECK_PARAM_PTR(pMute);

	APK_SQCAUDIO_isSytemMuted(pMute);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_setMute(uint32_t mute)
{
	APK_SQCAUDIO_SetSystemMute(mute ? TRUE : FALSE);
	return NXL_ERR_OK;
}


#define ____NXL_Audio_Related_Functions____

uint32_t nxl_apkv1_audio_getSoundMode(uint32_t ulDevIdx, char* szSoundMode)
{
	DxAudioConfig_e eAudioConfig;
	HCHAR *data;
	__CHECK_PARAM_PTR(szSoundMode);

#if 1
	eAudioConfig = APK_UDB_GetValue((const HUINT8 *)USERDB_AUDIO_SOUND_MODE);
#else
	APK_SQCAUDIO_GetSoundMode(&eAudioConfig);
#endif

	data = APK_UTIL_GetStringFromValue(_stAudioSoundModeList, eAudioConfig);
	NXL_LOG_DBG("getSoundMode:%s\n",data);
	HxSTD_StrNCpy(szSoundMode, data, 31);
	return NXL_ERR_OK;
}

static eNXL_AudioDigitalOutType local_v1_convertAudioDigitalOutType(APKE_DigitalOut_e eApkDigitalOut)
{
	switch (eApkDigitalOut)
	{
		case eDigitalOut_Unknown:           return NXL_AUDIO_DIGITAL_OUT_UNKNOWN;
		case eDigitalOut_DownMix_PCM:       return NXL_AUDIO_DIGITAL_OUT_DOWNMIX_PCM;
		case eDigitalOut_ByPass_All:        return NXL_AUDIO_DIGITAL_OUT_BYPASS;
		case eDigitalOut_ByPass_Specific :  return NXL_AUDIO_DIGITAL_OUT_BYPASS;
		case eDigitalOut_Off :              return NXL_AUDIO_DIGITAL_OUT_OFF;
		default:
			return NXL_AUDIO_DIGITAL_OUT_UNKNOWN;
	}
}

uint32_t nxl_apkv1_audio_getDigitalAudioOutputMode(eNXL_AudioDigitalOutType *pDigitalOut) /* (char* szDigitalOutput) */
{
	APKE_DigitalOut_e eApkDigitalOut;
	HCHAR *data;

#if 0
	eApkDigitalOut = APK_UDB_GetValue((const HUINT8 *)USERDB_AUDIO_DIGITAL_AUDIO_OUT);  /* Read current setting */
#else
	APK_SQCAUDIO_GetDigitalOutputMode(&eApkDigitalOut);
#endif
	data = APK_UTIL_GetStringFromValue(_stDigitalOutList, eApkDigitalOut);
	*pDigitalOut = local_v1_convertAudioDigitalOutType(eApkDigitalOut);
	return NXL_ERR_OK;
}


#define ____NXL_Output_Related_Functions____

uint32_t nxl_apkv1_output_getTVStandard(eNXL_TVOUT_STANDARD *eStandard)
{
	APKE_VideoStandard_e	eVideoStandard;
	__CHECK_PARAM_PTR(eStandard);
	*eStandard = NXL_TVOUT_STANDARD_UNKNOWN;

	APK_SQCOUTPUT_GetAnalogVideoStandard(&eVideoStandard);
	switch (eVideoStandard)
	{
		case eVideoStandard_Unknown:
			*eStandard = NXL_TVOUT_STANDARD_UNKNOWN;
			break;
		case eVideoStandard_NTSC:
			*eStandard = NXL_TVOUT_STANDARD_NTSC;
			break;
		case eVideoStandard_PAL:
			*eStandard = NXL_TVOUT_STANDARD_PAL;
			break;
		case eVideoStandard_SECAM:
			*eStandard = NXL_TVOUT_STANDARD_SECAM;
			break;
		default:
			break;
	}
	return NXL_ERR_OK;
}

/* Video Output */
//uint32_t nxl_apkv1_output_getSupportedVidoeOutput(void)
//{
//	HUINT32 devnum = 0;
//	APK_SQCOUTPUT_GetNumOfDevice(&devnum);
//	return NXL_ERR_OK;
//}

uint32_t nxl_apkv1_output_getDeviceNum(uint32_t *pnNumOfDev)
{
	HERROR hErr = ERR_OK;
	HUINT32 	ulDevNum;
	__CHECK_PARAM_PTR(pnNumOfDev);
	*pnNumOfDev = 0;

	hErr = APK_SQCOUTPUT_GetNumOfDevice((HUINT32*)pnNumOfDev);
	if (ERR_OK != hErr)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_GetNumOfDevice ret Error:0x%X.\n",hErr);
		return NXL_ERR_FAIL;
	}
	*pnNumOfDev = ulDevNum;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isSupportScart(uint32_t *pExisted)
{
	uint32_t	bExist;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_SQCOUTPUT_IsSupportScart(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_IsSupportScart ret Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isSupportTvScart(uint32_t *pExisted)
{
	uint32_t	bExist;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_SQCOUTPUT_IsSupportTvScart(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_IsSupportTvScart ret Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isSupportVcrScart(uint32_t *pExisted)
{
	uint32_t	bExist;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_SQCOUTPUT_IsSupportVcrScart(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_IsSupportVcrScart ret Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isSupportComposite(uint32_t *pExisted)
{
	uint32_t	bExist;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_SQCOUTPUT_IsSupportComposite(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_IsSupportComposite ret Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isSupportComponent(uint32_t *pExisted)
{
	uint32_t	bExist;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_SQCOUTPUT_IsSupportComponent(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_IsSupportComponent ret Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isSupportRF(uint32_t *pExisted)
{
	uint32_t	bExist;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_SQCOUTPUT_IsSupportRf(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_IsSupportRf ret Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isSupportHDMI(uint32_t *pExisted, eNXL_HdcpVer *pHdcpVer)
{
	uint32_t bExist = FALSE;
	APKE_HdcpVer_e eApkHdcpVer = eHDCP_VERSION_1_4;

	*pExisted = FALSE; *pHdcpVer = eHDCP_VERSION_MAX;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_SQCOUTPUT_IsSupportHdmi(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_IsSupportHdmi ret Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;

	if (APK_SQCOUTPUT_GetHdcpVersion(&eApkHdcpVer) != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_SQCOUTPUT_GetHdcp() failed\n");
		return NXL_ERR_OK;
	}

	switch (eApkHdcpVer) {
	case eHDCP_VERSION_1_0: *pHdcpVer = NXL_HDCP_VERSION_1_0; break;
	case eHDCP_VERSION_1_1: *pHdcpVer = NXL_HDCP_VERSION_1_1; break;
	case eHDCP_VERSION_1_2: *pHdcpVer = NXL_HDCP_VERSION_1_2; break;
	case eHDCP_VERSION_1_3: *pHdcpVer = NXL_HDCP_VERSION_1_3; break;
	case eHDCP_VERSION_1_4: *pHdcpVer = NXL_HDCP_VERSION_1_4; break;
	case eHDCP_VERSION_2_0: *pHdcpVer = NXL_HDCP_VERSION_2_0; break;
	case eHDCP_VERSION_2_1: *pHdcpVer = NXL_HDCP_VERSION_2_1; break;
	case eHDCP_VERSION_2_2: *pHdcpVer = NXL_HDCP_VERSION_2_2; break;
	default:
		/* NXL_LOG_ERR("Error> unsupported HDCP ver(%d)\n", eApkHdcpVer); */
		*pHdcpVer = eHDCP_VERSION_MAX;
		return NXL_ERR_OK;
	}

	/* NXL_LOG_DBG("Getting HDCP ver(%d)\n", eApkHdcpVer); */
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isSupportSpdif(uint32_t *pExisted)
{
	uint32_t	bExist;
	__CHECK_PARAM_PTR(pExisted);

	if (APK_SQCOUTPUT_IsSupportSpdif(&bExist) != ERR_OK)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_IsSupportSpdif ret Error.\n");
		return NXL_ERR_FAIL;
	}
	*pExisted = bExist;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isActiveOutputHDMI(uint32_t *pActivated, eNXL_HdcpVer *pHdcpVer)
{
	APKE_HDMIHotPlugStatus_e eHotplugStatus;
	APKE_HdcpVer_e eApkHdcpVer = eHDCP_VERSION_1_4;

	*pActivated = FALSE; *pHdcpVer = eHDCP_VERSION_MAX;
	__CHECK_PARAM_PTR(pActivated);

	if (APK_SQCHdmi_GetHotPlugStatus(&eHotplugStatus) != ERR_OK)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_IsActiveHdmi ret Error.\n");
		return NXL_ERR_FAIL;
	}

	if (eHotplugStatus == eHDMI_HOTPLUG_IN)
	{
		*pActivated = TRUE;
	}

	if (APK_SQCOUTPUT_GetHdcpVersion(&eApkHdcpVer) != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_SQCOUTPUT_GetHdcp() failed\n");
		return NXL_ERR_OK;
	}

	switch (eApkHdcpVer) {
	case eHDCP_VERSION_1_0: *pHdcpVer = NXL_HDCP_VERSION_1_0; break;
	case eHDCP_VERSION_1_1: *pHdcpVer = NXL_HDCP_VERSION_1_1; break;
	case eHDCP_VERSION_1_2: *pHdcpVer = NXL_HDCP_VERSION_1_2; break;
	case eHDCP_VERSION_1_3: *pHdcpVer = NXL_HDCP_VERSION_1_3; break;
	case eHDCP_VERSION_1_4: *pHdcpVer = NXL_HDCP_VERSION_1_4; break;
	case eHDCP_VERSION_2_0: *pHdcpVer = NXL_HDCP_VERSION_2_0; break;
	case eHDCP_VERSION_2_1: *pHdcpVer = NXL_HDCP_VERSION_2_1; break;
	case eHDCP_VERSION_2_2: *pHdcpVer = NXL_HDCP_VERSION_2_2; break;
	default:
		/* NXL_LOG_ERR("Unsupported HDCP ver(%d)\n", eApkHdcpVer); */
		*pHdcpVer = eHDCP_VERSION_MAX;
		return NXL_ERR_OK;
	}

	/* NXL_LOG_DBG("Getting HDCP ver(%d)\n", eApkHdcpVer); */
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_isActiveOutputSpdif(uint32_t *pActivated) /* TODO: Is this right? possible to detect Spdif? */
{
	uint32_t	bActivated;
	APKE_DigitalOut_e eDOutputMode;
	DxAudioCodec_e eSpecificCodec;
	__CHECK_PARAM_PTR(pActivated);

	*pActivated = FALSE;
 	if (APK_SQCAUDIO_GetDOutputMode(&eDOutputMode, &eSpecificCodec) != ERR_OK) // OxSVC_OUT_GetSpdifAudioOutFormat
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_IsActiveSpdif ret Error.\n");
		return NXL_ERR_FAIL;
	}

	switch (eDOutputMode) {
	case eDigitalOut_DownMix_PCM:
	case eDigitalOut_ByPass_All:
	case eDigitalOut_ByPass_Specific:
		bActivated = TRUE;
		break;
	case eDigitalOut_Unknown:
	case eDigitalOut_Off:
	default:
		bActivated = FALSE;
		break;
	}

	*pActivated = bActivated;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_getHdcpStatus(uint32_t *pbOn)
{
	HBOOL isOn = FALSE;

	*pbOn = FALSE;
	if (APK_SQCOUTPUT_GetHdcp(&isOn) != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_SQCOUTPUT_GetHdcp() failed\n");
		return NXL_ERR_FAIL;
	}
	*pbOn = isOn;
	NXL_LOG_DBG("APK_SQCOUTPUT_GetHdcp(%d)\n", isOn);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_setHdcpStatus(uint32_t bOn, uint32_t bForce) /* CUP (Content Usage Policy) */
{
	uint32_t nxErr = NXL_ERR_OK;
	uint32_t resWidth, resHeight;
	uint32_t maxImageWidth, maxImageHeight;

#if 0
	maxImageWidth = s_stApkv1Info.streamMaxWidth;
	maxImageHeight = s_stApkv1Info.streamMaxHeight;
#else
	maxImageWidth = s_stApkv1Info.streamCurrentWidth;
	maxImageHeight = s_stApkv1Info.streamCurrentHeight;
#endif
	NXL_LOG_DBG("APK_SQCOUTPUT_SetHdcp(on/off:%d, w:%d, h:%d)\n", bOn, maxImageWidth, maxImageHeight);

	if (!bForce && bOn && maxImageWidth && maxImageHeight ) /* Netflix HDCP Scenario : allow the normal playback/output for SD content. */
	{
		if (maxImageHeight < 720)
		{
			/* SD : 720x480, 720x576, ...
			 * HD : 1280x720, 1920x1080, 1440x1080, ...
			 */
			NXL_LOG_DBG("[HDCP ON] is ignored under SD(%d x %d)\n", maxImageWidth, maxImageHeight);
			return NXL_ERR_OK;
		}
	}

	if (APK_SQCOUTPUT_SetHdcp(bOn) != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_SQCOUTPUT_SetHdcp() failed\n");
		return NXL_ERR_FAIL;
	}

	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_getHdcpVersion(eNXL_HdcpVer *pHdcpVer) /* CUP (Content Usage Policy) */
{
	APKE_HdcpVer_e eApkHdcpVer = eHDCP_VERSION_1_4;

	*pHdcpVer = NXL_HDCP_VERSION_1_4; /* default */
	if (APK_SQCOUTPUT_GetHdcpVersion(&eApkHdcpVer) != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_SQCOUTPUT_GetHdcp() failed\n");
		return NXL_ERR_FAIL;
	}

	switch (eApkHdcpVer) {
	case eHDCP_VERSION_1_0:  *pHdcpVer = NXL_HDCP_VERSION_1_0; break;
	case eHDCP_VERSION_1_1:  *pHdcpVer = NXL_HDCP_VERSION_1_1; break;
	case eHDCP_VERSION_1_2:  *pHdcpVer = NXL_HDCP_VERSION_1_2; break;
	case eHDCP_VERSION_1_3:  *pHdcpVer = NXL_HDCP_VERSION_1_3; break;
	case eHDCP_VERSION_1_4:  *pHdcpVer = NXL_HDCP_VERSION_1_4; break;
	case eHDCP_VERSION_2_0:  *pHdcpVer = NXL_HDCP_VERSION_2_0; break;
	case eHDCP_VERSION_2_1:  *pHdcpVer = NXL_HDCP_VERSION_2_1; break;
	case eHDCP_VERSION_2_2:  *pHdcpVer = NXL_HDCP_VERSION_2_2; break;
	default:
		NXL_LOG_ERR("Error> unsupported HDCP ver(%d)\n", eApkHdcpVer);
		return NXL_ERR_FAIL;
	}

	NXL_LOG_DBG("Getting HDCP ver(%d)\n", eApkHdcpVer);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_getCgmsStatus(eNXL_CopyRight *pCgmsStatus)
{
	APKE_CopyRight_t eApkCopyRight = eCOPYRIGHT_COPY_FREE;

	*pCgmsStatus = NXL_COPYRIGHT_COPY_FREE;
	if (APK_SQCOUTPUT_GetCgmsStatus(&eApkCopyRight) != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_SQCOUTPUT_GetCgmsStatus() failed\n");
		return NXL_ERR_FAIL;
	}

	switch (eApkCopyRight) {
	case eCOPYRIGHT_COPY_FREE:    *pCgmsStatus = NXL_COPYRIGHT_COPY_FREE; break;
	case eCOPYRIGHT_COPY_NOMORE:  *pCgmsStatus = NXL_COPYRIGHT_COPY_NOMORE; break;
	case eCOPYRIGHT_COPY_ONCE:    *pCgmsStatus = NXL_COPYRIGHT_COPY_ONCE; break;
	case eCOPYRIGHT_COPY_NEVER:   *pCgmsStatus = NXL_COPYRIGHT_COPY_NEVER; break;
	default:
		NXL_LOG_ERR("Error> unmatched copyRight(%d)\n", eApkCopyRight);
		return NXL_ERR_FAIL;
	}

	NXL_LOG_DBG("Getting copyRight(%d)\n", eApkCopyRight);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_setCgmsStatus(eNXL_CopyRight eCgmsStatus)
{
	APKE_CopyRight_t eApkCopyRight = eCOPYRIGHT_COPY_FREE;

	switch (eCgmsStatus) {
		case NXL_COPYRIGHT_COPY_FREE:    eApkCopyRight = eCOPYRIGHT_COPY_FREE; break;
		case NXL_COPYRIGHT_COPY_NOMORE:  eApkCopyRight = eCOPYRIGHT_COPY_NOMORE; break;
		case NXL_COPYRIGHT_COPY_ONCE:    eApkCopyRight = eCOPYRIGHT_COPY_ONCE; break;
		case NXL_COPYRIGHT_COPY_NEVER:   eApkCopyRight = eCOPYRIGHT_COPY_NEVER; break;
		default:
			NXL_LOG_ERR("Error> unsupported CopyRight(%d)\n", eCgmsStatus);
			return NXL_ERR_FAIL;
	}

	NXL_LOG_DBG("APK_SQCOUTPUT_SetCgmsStatus(%d)\n", eApkCopyRight);
	if (APK_SQCOUTPUT_SetCgmsStatus(eCgmsStatus) != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_SQCOUTPUT_SetCgmsStatus() failed\n");
		return NXL_ERR_FAIL;
	}
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_getEdidDisplayResolution(uint32_t *puDisplayResX, uint32_t *puDisplayResY)
{
	HUINT32 ulDisplayResX = 0, ulDisplayResY = 0;

	*puDisplayResX = *puDisplayResY = 0;
	if (APK_SQCOUTPUT_GetEdidDisplayResolution(&ulDisplayResX, &ulDisplayResY) != ERR_OK)
	{
		NXL_LOG_ERR("Error> APK_SQCOUTPUT_GetEdidDisplayResolution() failed\n");
		return NXL_ERR_FAIL;
	}
	*puDisplayResX = ulDisplayResX;
	*puDisplayResY = ulDisplayResY;
	NXL_LOG_DBG("APK_SQCOUTPUT_GetEdidDisplayResolution(x:%d, y:%d)\n", ulDisplayResX, ulDisplayResY);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_getActiveVideoOutput(void)
{
	// Not implemented
	return NXL_ERR_UNSUPPORTED;
}

uint32_t nxl_apkv1_output_getOutputResolution(uint32_t *pWidth, uint32_t *pHeight)
{
	APKE_ResolutionSelect_e eResolutionSelect = eRESSELECT_UNKNOWN; /* APKE_OutputResolution_e */
	HCHAR *data;
	uint32_t width = 0, height = 0;

	__CHECK_PARAM_PTR(pWidth);
	__CHECK_PARAM_PTR(pHeight);
	*pWidth = *pHeight = 0;

	if ((APK_SQCOUTPUT_GetResolutionSelect(&eResolutionSelect) != ERR_OK) || (eResolutionSelect == eRESSELECT_UNKNOWN))
	{
		NXL_LOG_ERR("Error> failed to get resolutionSelect(%d)!\n", eResolutionSelect);
		eResolutionSelect = APK_UDB_GetValue((HUINT8 *)USERDB_VIDEO_RESOLUTION);
	}

	if (eResolutionSelect == eRESSELECT_UNKNOWN)
	{
		NXL_LOG_ERR("Error> unknown resolutionSelect(%d)!\n", eResolutionSelect);
	}

	/*
	data = APK_UTIL_GetStringFromValue(_astResolutionSelectPairList, eResolutionSelect);
	HxSTD_StrNCpy(pszResolutionSelect, data, (nMaxStrlen-1));
	pszResolutionSelect[(nMaxStrlen-1)] = '\0';
	*/

	switch(eResolutionSelect)
	{
		case eRESSELECT_640x480P:
			width = 640; height = 480;
			break;
		case eRESSELECT_720x480I:
		case eRESSELECT_720x480P:
			width = 720; height = 480;
			break;
		case eRESSELECT_720x576I:
		case eRESSELECT_720x576P:
			width = 720; height = 576;
			break;
#if 0 /* DPI-040-TC9/DPI-175-TC6 : DPI - Verify support2DVideoResize - 720p */
		case eRESSELECT_1080x720P:
			width = 1080; height = 720;
			break;
#else
		case eRESSELECT_1080x720P:
			/* No appkit API for APKE_OutputResolution_e::eRESOLUTION_1280X720P */
			width = 1280; height = 720;
			break;
#endif
		case eRESSELECT_1920x1080I:
		case eRESSELECT_1920x1080P:
			width = 1920; height = 1080;
			break;
		default:
			width = 0; height = 0;
			break;
	}
	
	if (s_stApkv1Info.lastSelectedResolution != eResolutionSelect)
	{
		s_stApkv1Info.lastOutputResolutionChanged = 1;
	}
	
	if ((s_stApkv1Info.lastOutputResolutionWidth != width) || (s_stApkv1Info.lastOutputResolutionHeight != height))
	{
		s_stApkv1Info.lastOutputResolutionChanged = 1;
	}
	
	
	if (eResolutionSelect != eRESSELECT_UNKNOWN)
	{
		s_stApkv1Info.lastSelectedResolution = eResolutionSelect;
	}
	if (width && height)
	{
		s_stApkv1Info.lastOutputResolutionWidth = width;
		s_stApkv1Info.lastOutputResolutionHeight = height;
	}

	/* NXL_LOG_DBG("[%s:%d] eResolutionSelect(%d) - width(%d), height(%d)\n", __FUNCTION__, __LINE__, eResolutionSelect, width, height); */
	*pWidth = width; *pHeight = height;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_checkLastOutputResolutionChanged(uint32_t *pUpdated)
{
	uint32_t width = 0, height = 0;

	*pUpdated = 0;
	if (nxl_apkv1_output_getOutputResolution(&width, &height) == NXL_ERR_OK)
	{
		*pUpdated = s_stApkv1Info.lastOutputResolutionChanged;
		s_stApkv1Info.lastOutputResolutionChanged = 0;
	}

	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_getSupportedResolutions(/* (uint32_t ulDevIdx) */ int *count, char ***pszResolutions)
{
	HERROR		nRet;
	HUINT32		ulMaxNum,idx,ulChkIdx,ulMaxValue;
	HUINT32		ulExistCnt=0;
	char			**myStrings=NULL;
	APKE_ResolutionSelect_e	eSupportedResolutions, eResolution;

	if ((NULL == count) || (NULL == pszResolutions))
		return NXL_ERR_FAIL;

	*pszResolutions = NULL;
	*count = 0;

	nRet = APK_SQCOUTPUT_GetSupportedResolution(&eSupportedResolutions); /* This isnot selected resolution w/ APK_SQCOUTPUT_GetResolutionSelect() */
	if (ERR_OK != nRet)
	{
		NXL_LOG_ERR("Error> GetSupportedResolution ret Error:0x%X\n",nRet);
		return NXL_ERR_OK;

	}
	NXL_LOG_DBG("eSupportedResolutions:0x%X\n",eSupportedResolutions);

	ulMaxNum = 14; ulExistCnt = 0;
	for (idx=0; idx < ulMaxNum; idx++)
	{
		eResolution = (eSupportedResolutions & (eRESSELECT_640x480P << idx));
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
	for (ulMaxValue = 0, idx=0; ulMaxValue != eRESSELECT_MAX; ulMaxValue = ((eRESSELECT_640x480P << idx) | ulMaxValue), idx++)
	{
		HCHAR	*data;

		NXL_LOG_DBG("idx:%d ulMaxValue:0x%X\n",idx,ulMaxValue);

		eResolution = (eSupportedResolutions & (eRESSELECT_640x480P << idx));
		if (eResolution)
		{
			data = APK_UTIL_GetStringFromValue(_astResolutionSelectPairList, eResolution);
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

uint32_t nxl_apkv1_output_getTvAspectRatio(uint32_t ulDevIdx, char *pszTvAspectRatio)
{
	HCHAR	*data;
	APKE_AspectRatio_e	eTVAspectRatio;
	__CHECK_PARAM_PTR(pszTvAspectRatio);

	eTVAspectRatio = (APKE_AspectRatio_e)APK_UDB_GetValue((HUINT8 *)USERDB_VIDEO_SCREEN_RATIO);
	NXL_LOG_DBG(" eTVAspectRatio:0x%X\n",eTVAspectRatio);

	data = APK_UTIL_GetStringFromValue(_astAspectRatioPairList, eTVAspectRatio);
	HxSTD_StrNCpy(pszTvAspectRatio, data, 31);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_output_getVideoStandard(uint32_t ulDevIdx, char *pszVideoStandard)
{
	HCHAR		*data;
	HERROR		nRet;
	APKE_VideoStandard_e	eVideoStandard;
	__CHECK_PARAM_PTR(pszVideoStandard);

	nRet = APK_SQCOUTPUT_GetAnalogVideoStandard(&eVideoStandard);
	if (ERR_OK != nRet)
	{
		NXL_LOG_ERR("APK_SQCOUTPUT_GetAnalogVideoStandard ret Error:0x%X.\n",nRet);
		return NXL_ERR_FAIL;
	}
	NXL_LOG_DBG(" eVideoStandard:0x%X\n",eVideoStandard);

	data = APK_UTIL_GetStringFromValue(_astVideoStandardPairList, eVideoStandard);
	HxSTD_StrNCpy(pszVideoStandard, data, 31);
	return NXL_ERR_OK;
}


#define ____NXL_Media_Related_Functions____

/* ES Manager */
uint32_t nxl_apkv1_mediapb_getResourceMap(void *pMap)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if (pMap == NULL)
	{
		NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] pMap is NULL\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	HxSTD_MemSet(&s_stApkv1Info.stResourceMap, 0x00, sizeof(APKS_MEDIADEC_RESOURCE_MAP_t));

	hErr = APK_MEDIACTRL_MEDIAPB_GetResourceMap(&s_stApkv1Info.stResourceMap);
	if (hErr != ERR_OK)
	{
		NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] Failed to get a resource map\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	((NXL_ESMANAGER_ResourceMap_t *)pMap)->hPlaypumpList   = s_stApkv1Info.stResourceMap.ulPlaypumpList;
	((NXL_ESMANAGER_ResourceMap_t *)pMap)->hDmaList        = s_stApkv1Info.stResourceMap.ulDmaList;
	((NXL_ESMANAGER_ResourceMap_t *)pMap)->uiNumOfPlaypump = s_stApkv1Info.stResourceMap.ulNumOfPlaypump;
	((NXL_ESMANAGER_ResourceMap_t *)pMap)->uiNumOfDma      = s_stApkv1Info.stResourceMap.ulNumOfDma;

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

/* Playback Group */
uint32_t nxl_apkv1_mediapb_registerNotifier(void *pFunc)
{
	NXL_ERR nRet = NXL_ERR_OK;
	APK_HANDLE hMediaPbHandle = NULL;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if (pFunc == NULL)
	{
		NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hMediaPbHandle = APK_MEDIA_GetHandle(eMctrlBase_MediaPlay, eDest);
	if (hMediaPbHandle == NULL)
	{
		NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] hMediaPbHandle is NULL\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	NXL_LOG_DBG("\033[1;32m @@@@@ [%s:%d] Before APK_MEDIACTRL_RegisterNotifier() : hMediaPbHandle = %p\033[0m\n", __func__, __LINE__, hMediaPbHandle);
	APK_MEDIACTRL_RegisterNotifier(hMediaPbHandle, (CBObjectHandler)pFunc);

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_unregisterNotifier(void *pFunc)
{
	NXL_ERR nRet = NXL_ERR_OK;
	APK_HANDLE hMediaPbHandle = NULL;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if (pFunc == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] pFunc is NULL\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hMediaPbHandle = APK_MEDIA_GetHandle(eMctrlBase_MediaPlay, eDest);
	if (hMediaPbHandle == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] hMediaPbHandle is NULL\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	APK_MEDIACTRL_UnRegisterNotifier(hMediaPbHandle, (CBObjectHandler)pFunc);

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_start(void *pProbeInfo)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;
	APKS_MEDIADEC_MEDIAPLAY_t unStart;
	NXL_PLAYBACKGROUP_ProbeInfo_t *pstProbeInfo = NULL;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	s_stApkv1Info.lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_Stop;
	if (pProbeInfo == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to start Media PB\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	pstProbeInfo = (NXL_PLAYBACKGROUP_ProbeInfo_t *)pProbeInfo;

	/* Call the play IPC function to play contents */
	eDest = APK_MEDIACTRL_GetMainViewDestination();

	memset(&unStart, 0x00, sizeof(APKS_MEDIADEC_MEDIAPLAY_t));
	unStart.ulStartTime = 0;
	unStart.ulAudioPid = pstProbeInfo->audio_pid;
	unStart.ulVideoPid = pstProbeInfo->video_pid;
	unStart.eAudioCodec = (DxAudioCodec_e)pstProbeInfo->audio_codec;
	unStart.eVideoCodec = (DxVideoCodec_e)pstProbeInfo->video_codec;

	HxSTD_StrNCpy(unStart.szUrl, (const char *)("MSE"), sizeof("MSE"));

	hErr = APK_MEDIACTRL_MEDIAPB_Start(eDest, &unStart);
	if (hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Starting mediapb failed, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
		goto FUNCTION_EXIT;
	}
	s_stApkv1Info.lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_Start; /* Distinguish between the START and PLAY to the check BUFFERING state in AMC::OverlayTask() . */

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_stop(void)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hErr = APK_MEDIACTRL_MEDIAPB_Stop(eDest);
	if (hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to stop Media PB, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	/* Initialize stream videoAttributes */
	s_stApkv1Info.lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_Stop;
	s_stApkv1Info.streamPAR_X = s_stApkv1Info.streamPAR_Y = 0;
	s_stApkv1Info.streamCurrentWidth = s_stApkv1Info.streamCurrentHeight = 0;
	s_stApkv1Info.streamMaxWidth = s_stApkv1Info.streamMaxHeight = 0;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_resetPlayback(void)
{
	NXL_ERR nRet = NXL_ERR_OK;
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] ++%d (+)\033[0m\n", __func__, __LINE__, s_stApkv1Info.lastPlaybackCmd);

#if 0
	// platform_dfb_ipc: closing client:0x41cc438 with fd:328 
	// bipc_server_destroy:0x15e3340 cleaning up object 0x429c210(328,328):brc_client(0x41cc474)
	// nexus_platform_server: abnormal termination of an unprotected client 0x4dcfb18 is not supported
	if (s_stApkv1Info.lastPlaybackCmd > NXL_PLAYBACKGROUP_CMD_Stop)
	{
		(void)nxl_apkv1_mediapb_stop();
	}
#endif
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_getNotifyArgs(uint32_t p1, void *pEvent)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HxJSON_t jsParam;
	APKE_MediaCtrl_Event_e eEvent;
	APKE_MediaCtrl_Destination_e eDest;
	uint32_t ulSessionHandle = 0;
	HERROR hErr = ERR_OK;

	//NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if (pEvent == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] pEvent is NULL\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	jsParam = (HxJSON_t)p1;

	hErr = APK_MEDIACTRL_GetNotifyArgs(jsParam, &eDest, &ulSessionHandle, &eEvent, NULL);
	if (hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to get notified arguments\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	switch(eEvent)
	{
		case eMctrlEvent_MEDIA_Started:
			//NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] eMctrlEvent_MEDIA_Started\033[0m\n", __func__, __LINE__);
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_Started;
			break;
		case eMctrlEvent_MEDIA_Stopped:
			//NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] eMctrlEvent_MEDIA_Stopped\033[0m\n", __func__, __LINE__);
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_Stopped;
			break;
		case eMctrlEvent_MEDIA_PlaytimeChanged:
			//NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] eMctrlEvent_MEDIA_PlaytimeChanged\033[0m\n", __func__, __LINE__);
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_PlaytimeChanged;
			break;
		case eMctrlEvent_MEDIA_PlayStateChanged:
			NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] eMctrlEvent_MEDIA_PlayStateChanged\033[0m\n", __func__, __LINE__);
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_PlayStateChanged;
			break;
		case eMctrlEvent_MEDIA_PumpIndexChanged:
			//NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] eMctrlEvent_MEDIA_PumpIndexChanged\033[0m\n", __func__, __LINE__);
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_PumpIndexChanged;
			break;
		case eMctrlEvent_MEDIA_SeekReady:
			//NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] eMctrlEvent_MEDIA_SeekReady\033[0m\n", __func__, __LINE__);
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_SeekReady;
			break;
		case eMctrlEvent_MEDIA_AudioCodecSwitched:
			//NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] eMctrlEvent_MEDIA_AudioCodecSwitched\033[0m\n", __func__, __LINE__);
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_AudioCodecSwitched;
			break;
		default:
			*((NXL_PLAYBACKGROUP_MEDIA_EVENT_e *)pEvent) = NXL_PLAYBACKGROUP_MEDIA_EVENT_Unknown;
			break;
	}

FUNCTION_EXIT:
	//NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_setSpeed(int32_t nSpeedSec)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	NXL_LOG_DBG("\033[1;32m @@@@@ [%s:%d] Before APK_MEDIACTRL_MEDIAPB_SetPlaySpeed() : nSpeedSec = %d\033[0m\n", __func__, __LINE__, nSpeedSec);
	hErr = APK_MEDIACTRL_MEDIAPB_SetPlaySpeed(eDest, nSpeedSec);
	if (hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Setting play speed failed, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if (nSpeedSec) s_stApkv1Info.lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_Play;
	else s_stApkv1Info.lastPlaybackCmd = NXL_PLAYBACKGROUP_CMD_Pause;

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_setPosition(uint32_t unPlayTime)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hErr = APK_MEDIACTRL_MEDIAPB_Jump(eDest, unPlayTime);
	if (hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to jump to position(%d), hErr = %d\033[0m\n", __func__, __LINE__, unPlayTime, hErr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_getPumperIndex(uint32_t *punIndex)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if (punIndex == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] punIndex is NULL\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hErr = APK_MEDIACTRL_MEDIAPB_GetPumpIndex(eDest, punIndex);
	if (hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to get a playpump index, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_getPosition(uint32_t *punPosition)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	//NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if (punPosition == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] punPosition is NULL\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hErr = APK_MEDIACTRL_GetPlayTime(eDest, punPosition);
	if (hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to get a playing position, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	//NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_getPlayState(void *pState)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;
	APKE_MediaCtrl_PlayState_e ePlayState = eMctrlPlayState_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if (pState == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] pState is NULL\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hErr = APK_MEDIACTRL_GetPlayState(eDest, &ePlayState);

	NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] MEDIACTRL PlayState = %d\033[0m\n", __func__, __LINE__, ePlayState);
	switch(ePlayState)
	{
		case eMctrlPlayState_None:
		case eMctrlPlayState_Stop:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Stop;
			break;
		case eMctrlPlayState_Playing:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Playing;
			break;
		case eMctrlPlayState_Paused:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Paused;
			break;
		case eMctrlPlayState_EndOfStream:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_EndOfStream;
			break;
		case eMctrlPlayState_BufferingUnderflow:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_BufferingUnderflow;
			break;
		default:
			*((NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_e *)pState) = NXL_PLAYBACKGROUP_MEDIA_PLAY_STATE_Error;
			break;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_checkLastPlayCmd(NXL_PLAYBACKGROUP_CMD_e *pCmd)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;

	//NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if (pCmd == NULL)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] NULL arg\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_INVALID_PARAM;
		goto FUNCTION_EXIT;
	}
	*pCmd =  s_stApkv1Info.lastPlaybackCmd;

FUNCTION_EXIT:
	//NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] lastPlaybackCmd:%d (-)\033[0m\n", __func__, __LINE__, (uint32_t)*pCmd);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_informLastPlayCmd(NXL_PLAYBACKGROUP_CMD_e eCmd)
{
	//NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	s_stApkv1Info.lastPlaybackCmd = eCmd;
	//NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] lastPlaybackCmd:%d (-)\033[0m\n", __func__, __LINE__, (uint32_t)eCmd);
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_mediapb_setDuration(double dblCurrentMaxPts)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hErr = APK_MEDIACTRL_MEDIAPB_SetDuration(eDest, (HUINT64)dblCurrentMaxPts);
	if (hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to set a duration, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_audioFlush(uint32_t unTryToChangeAudioTrack)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hErr = APK_MEDIACTRL_MEDIAPB_AudioFlush(eDest, unTryToChangeAudioTrack);
	if (hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to flush an audio decoder buffer, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_audioRestart(void)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hErr = APK_MEDIACTRL_MEDIAPB_AudioRestart(eDest);
	if(hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to restart an audio decoder\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_audioCodecSwitch(uint32_t unTryToChangeAudioTrack, int32_t nAudioCodec)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	hErr = APK_MEDIACTRL_MEDIAPB_AudioCodecSwitch(eDest, unTryToChangeAudioTrack, nAudioCodec);
	if(hErr != ERR_OK)
	{
		NXL_LOG_ERR("\033[1;31m @@@@@ [%s:%d] Failed to switch an audio codec\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_notifyStreamCurrentPAR(uint32_t pixelAspectRatioX, uint32_t pixelAspectRatioY) /* VideoDecoderNative */
{
	/* NXL_LOG_DBG("[%s:%d] -- [pixelAspectRatioX:%d, pixelAspectRatioY:%d]\n",
			__FUNCTION__, __LINE__, pixelAspectRatioX, pixelAspectRatioY);
	*/
	s_stApkv1Info.streamPAR_X = pixelAspectRatioX;
	s_stApkv1Info.streamPAR_Y = pixelAspectRatioY;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_mediapb_notifyStreamCurrentSize(uint32_t imageWidth, uint32_t imageHeight) /* VideoDecoderNative */
{
	/* NXL_LOG_DBG("[%s:%d] -- [imageWidth:%d, imageHeight:%d]\n",
			__FUNCTION__, __LINE__, imageWidth, imageHeight);
	*/
	s_stApkv1Info.streamCurrentWidth  = imageWidth;
	s_stApkv1Info.streamCurrentHeight = imageHeight;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_mediapb_notifyStreamMaxSize(uint32_t maxImageWidth, uint32_t maxImageHeight) /* PlaybackDevice::initVideoPlayer */
{
	/* NXL_LOG_DBG("[%s:%d] -- [maxImageWidth:%d, maxImageHeight:%d]\n",
			__FUNCTION__, __LINE__, maxImageWidth, maxImageHeight);
	*/
	s_stApkv1Info.streamMaxWidth  = maxImageWidth;
	s_stApkv1Info.streamMaxHeight = maxImageHeight;
	return NXL_ERR_OK;
}

uint32_t nxl_apkv1_mediapb_resetVideoSize(void)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	eDest = APK_MEDIACTRL_GetMainViewDestination();
	hErr = APK_MEDIACTRL_MEDIAPB_ResetVideoSize(eDest);
	if(hErr != ERR_OK)
	{
		NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] Failed to set video window size\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

uint32_t nxl_apkv1_mediapb_setVideoSize(uint32_t ulPosX, uint32_t ulPosY, uint32_t ulWidth, uint32_t ulHeight)
{
	NXL_ERR nRet = NXL_ERR_OK;
	HERROR hErr = ERR_OK;
	APKE_MediaCtrl_Destination_e eDest = eMctrlDest_None;
	APKS_MEDIADEC_VIDEOSIZE_t stVideoSize;
	uint32_t resWidth = 0, resHeight = 0;

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] (+) - [x:%d, y:%d, w:%d, h:%d] -- ref[w:%d, h:%d]\033[0m\n",
			__FUNCTION__, __LINE__,
			ulPosX, ulPosY, ulWidth, ulHeight,
			s_stApkv1Info.lastOutputResolutionWidth, s_stApkv1Info.lastOutputResolutionHeight);

	eDest = APK_MEDIACTRL_GetMainViewDestination();

	/* Check exception cases */
	if (!ulPosX && !ulPosY && !ulWidth && !ulHeight)
	{
		/* (0,0,0,0),ref(w:1280, h:720) --> Full screen at Step#8--step#14 in DPI-040-TC9 */
		NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] --> resetVideoSize(x:%d, y:%d, w:%d, h:%d) -- ref[w:%d, h:%d]\033[0m\n",
					__FUNCTION__, __LINE__,
					ulPosX, ulPosY, ulWidth, ulHeight,
					s_stApkv1Info.lastOutputResolutionWidth, s_stApkv1Info.lastOutputResolutionHeight);
		hErr = nxl_apkv1_mediapb_resetVideoSize();
		goto func_done;
	}

	/* Check exception cases */
	if (0)
	{
		/* (0,0,1920,1080),ref(w:1280, h:720) --> Full screen at Step#8--step#14 in DPI-040-TC9 */
		/* (0,0,1920,1080),ref(w:1920, h:1080) --> Full screen in DPI-040-TC4(Animation), DPI-040-TC8(AspectRatio) */
		if (!ulWidth || !ulHeight ||
			(ulWidth >= s_stApkv1Info.lastOutputResolutionWidth) ||
			(ulHeight >= s_stApkv1Info.lastOutputResolutionHeight))
		{
			NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] --> resetVideoSize(x:%d, y:%d, w:%d, h:%d) -- ref[w:%d, h:%d]\033[0m\n",
					__FUNCTION__, __LINE__,
					ulPosX, ulPosY, ulWidth, ulHeight,
					s_stApkv1Info.lastOutputResolutionWidth, s_stApkv1Info.lastOutputResolutionHeight);
			hErr = nxl_apkv1_mediapb_resetVideoSize();
			goto func_done;
		}
	}

	/* Check exception cases */
	if (1)
	{
		/* (0,0,1920,1080),ref(w:1280, h:720) --> Full screen at Step#8--step#14 in DPI-040-TC9 */
		/* (0,0,1920,1080),ref(w:1920, h:1080) --> Full screen in DPI-040-TC4(Animation), DPI-040-TC8(AspectRatio) */
		if ((ulWidth >= 1280) ||  (ulHeight >= 720))
		{
			NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] --> SetVideoSize(x:%d, y:%d, w:%d, h:%d) -- ref[w:%d, h:%d]\033[0m\n",
					__FUNCTION__, __LINE__,
					ulPosX, ulPosY, ulWidth, ulHeight,
					s_stApkv1Info.lastOutputResolutionWidth, s_stApkv1Info.lastOutputResolutionHeight);
			hErr = APK_MEDIACTRL_MEDIAPB_SetVideoSize(eDest, ulPosX, ulPosY, 1280, 720);
			goto func_done;
		}
	}

	/* Resize videoWindow */
	if (0)
	{
		/* Removed because of NTS 2DVideoResize error for 1080p - DPI-040-TC6/DPI-175-TC4 */
		hErr = APK_MEDIACTRL_MEDIAPB_SetVideoSize(eDest, ulPosX, ulPosY, ulWidth, ulHeight);
	}
	else
	{
		HxSTD_MemSet(&stVideoSize, 0, sizeof(APKS_MEDIADEC_VIDEOSIZE_t));
		stVideoSize.nStartX = ulPosX;
		stVideoSize.nStartY = ulPosY;
		stVideoSize.nWidth = ulWidth;
		stVideoSize.nHeight = ulHeight;
		stVideoSize.bRoundOff = TRUE;
		/* stVideoSize.bForceSourceBase = TRUE; */
		
		/*
		if (nxl_apkv1_output_getOutputResolution(&resWidth, &resHeight) == NXL_ERR_OK)
		{
			if ((resWidth && s_stApkv1Info.lastOutputResolutionWidth != resWidth) ||
				(resHeight && s_stApkv1Info.lastOutputResolutionHeight != resHeight))
			{
				;
			}
		}
		NXL_LOG_DBG("[%s:%d] --> resolution(w:%d, h:%d) -- prev[w:%d, h:%d]\033[0m\n",
				__FUNCTION__, __LINE__,
				resWidth, resHeight, 
				s_stApkv1Info.lastOutputResolutionWidth, s_stApkv1Info.lastOutputResolutionHeight);
		*/
		
		if (s_stApkv1Info.lastOutputResolutionWidth)
		{
			stVideoSize.nMaxWidth = s_stApkv1Info.lastOutputResolutionWidth;
		}
		else
		{
			stVideoSize.nMaxWidth = 1280; /* WEB max size is 1280x720 */
		}

		if (s_stApkv1Info.lastOutputResolutionHeight)
		{
			stVideoSize.nMaxHeight = s_stApkv1Info.lastOutputResolutionHeight;
		}
		else
		{
			stVideoSize.nMaxHeight = 720; /* WEB max size is 1280x720 */
		}

		NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] --> ResizeVideoWindow(x:%d, y:%d, w:%d, h:%d) -- ref[w:%d, h:%d]\033[0m\n",
					__FUNCTION__, __LINE__,
					ulPosX, ulPosY, ulWidth, ulHeight,
					stVideoSize.nMaxWidth, stVideoSize.nMaxHeight);
		hErr = APK_MEDIACTRL_MEDIAPB_ResizeVideoWindow(eDest, &stVideoSize); /* For supporting 2DVideoResize - 1080p DPI-040-TC6/DPI-175-TC4 */
	}

func_done:
	if(hErr != ERR_OK)
	{
		NXL_LOG_DBG("\033[1;31m @@@@@ [%s:%d] Error> Failed to set video window size\033[0m\n", __func__, __LINE__);
		nRet = NXL_ERR_FAIL;
	}

	NXL_LOG_DBG("\033[1;36m @@@@@ [%s:%d] (-)\033[0m\n", __FUNCTION__, __LINE__);
	return nRet;
}

