/**
 * di_pb.c
*/

/**
 * @defgroup		DI_PVR DI_PB : Playback module
 * @author		Dong-joon Kim
 * @note			DI_PB APIs
 * @brief			Define DI_PB APIs
 * @file 			di_pb.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "nexus_platform.h"
#include "drv_pvr.h"
#include "drv_demux.h"
#include "di_demux.h"
#include "di_video.h"
#include "drv_video.h"
#include "drv_crypt.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "vkernel.h"
#include "taskdef.h"

#include <sys/stat.h>

#if defined (USE_PLAYREADY)
#include "prdy_core.h"
#include "nexus_playpump.h"
#endif
#if defined(CONFIG_THUMBNAIL_DECODING)
#include "drv_osd.h"
#include "bthumbnail_extractor.h"
#include "nexus_surface.h"
#ifdef CONFIG_VFIO
#ifdef CONFIG_MEDIA20
#include "vfio20.h"
#else
#include "vfio.h"
#endif
#endif
#endif
#if defined(CONFIG_BMEM_OPTIMIZE)
#include "di_config.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DI_PVR
#if defined(CONFIG_DELAYED_RECORD)
#define ENABLE_CHUNK_FILE_PLAYBACK
#endif

/* playback event or state code */
#define	PB_BOF_EVENTS						(1)
#define	PB_EOF_EVENTS						(2)

#define	PB_TIME_MSG_QUEUE_SIZE		10
#define	PB_MSG_TIMEOUT				(0xffffffff)
#define	PB_TIME_NOTIFY_TIMER_VALUE		(100) //ms
#define	PB_PAUSE_TIMER_VALUE		(100) //ms

#if defined(CONFIG_BMEM_OPTIMIZE)
#define	PB_NUM_PLAYPUMPS				CONFIG_NUM_PLAYPUMPS
#else
#define	PB_NUM_PLAYPUMPS				NEXUS_NUM_PLAYPUMPS
#endif

#if defined(CONFIG_LARGE_SIZE_PLAYPUMP_BUFFER)
#define PB_NUM_PLAYPUMPS_CLASS1  1/*Large-size*/
#define PB_NUM_PLAYPUMPS_CLASS0 (PB_NUM_PLAYPUMPS-PB_NUM_PLAYPUMPS_CLASS1)/*Small-size*/
#else
#define PB_NUM_PLAYPUMPS_CLASS1 0
#define PB_NUM_PLAYPUMPS_CLASS0 PB_NUM_PLAYPUMPS
#endif

// workaround, tsr play를 pause로 시작한 경우 hdmi에서 정지 영상 나오지 않은 이슈의 workaround
#define TSR_PAUSE_WORKAROUND

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum {
	P_PLAY_MSG_TIMECODE_START,
	P_PLAY_MSG_TIMECODE_END
} P_PLAY_MSG_TYPE;

typedef	struct	stP_PLAY_TRICK_TABLE
{
	NEXUS_PlaybackHostTrickMode mode;
	int	modeModifier;
	unsigned long slowMotionRate;
} P_PLAY_TRICK_TABLE;

typedef	struct	stP_PLAY_TASK_MSG
{
	unsigned long	deviceId;
	P_PLAY_MSG_TYPE		msgId;
	unsigned long	param1;
	unsigned long	param2;
	unsigned long	param3;
} P_PLAY_TASK_MSG;

typedef	struct	stP_PLAY_PID_CHANNEL_INFO
{
	NEXUS_PidChannelHandle videoPidHandle;
	NEXUS_PidChannelHandle dummyVideoPidHandle;
	NEXUS_PidChannelHandle audioMainPidHandle;
	NEXUS_PidChannelHandle dummyAudioMainPidHandle;
	NEXUS_PidChannelHandle audioSubPidHandle;
	NEXUS_PidChannelHandle dummyAudioSubPidHandle;
	unsigned short videoPid;
	unsigned short audioMainPid;
	unsigned short audioSubPid;
} P_PLAY_PID_CHANNEL_INFO;

typedef	struct	stP_PLAY_INSTANCE
{
	DI_PVR_STATUS	status;
	NEXUS_PlaybackHandle	pbHandle;
	NEXUS_PlaypumpHandle	pbPumpHandle;
	NEXUS_FilePlayHandle	filePbHandle;
	NEXUS_KeySlotHandle		decKeyHandle;
	DI_PVR_Event_Callback	pbEvtCbFunc[DI_PVR_EVENT_MAX_NUMBER];
	DI_PVR_PLAY_SETUP_t 			setup;
	int						isExternalPlay;
	P_PLAY_PID_CHANNEL_INFO			pbPidInfo;
	unsigned long					diSemId;
	DI_PVR_TRICKSPEED_t				playSpeed;
	HBOOL							pauseRetry;
	unsigned long					pauseTimerId;
	DRV_PVR_REC_DEVICE_INFO			recDeviceInfo; /* for TSR/chase playback */
	unsigned long			lastTimeStatus; /* used for consistent time notify (ms) */
	unsigned				lastPacingTsRangeError;
#if defined(CONFIG_DELAYED_RECORD)
	int						isDelayedRecording;
#endif
#if defined(TSR_PAUSE_WORKAROUND)
// workaround, tsr play를 pause로 시작한 경우 hdmi에서 정지 영상 나오지 않은 이슈의 workaround
	unsigned int			decoderMuteDisableCount;
#endif

}P_PLAY_INSTANCE;

typedef	struct	stP_PLAYPUMP_INSTANCE
{
	NEXUS_PlaypumpHandle		playpump;
	HBOOL	bIsUsed;
	HUINT32	ulBuffClass; /*0:small buffer, 1:large buffer*/
}P_PLAYPUMP_INSTANCE;

#if defined(CONFIG_THUMBNAIL_DECODING)
typedef struct stP_PLAY_THUMBNAIL_INFO {
    char *datafilename;
    char *indexfilename;
    NEXUS_TransportType transportType;
    NEXUS_VideoCodec videoCodec;
    unsigned pid;
    BKNI_EventHandle still_done;

#ifdef CONFIG_VFIO
    VFIO_Handle_t	VFIO_indexfile, VFIO_datafile;
#else
   FILE *stdio_indexfile, *stdio_datafile;
#endif
    bfile_io_read_t indexfile, datafile;
    bthumbnail_extractor_t thumbnail_extractor;

   // NEXUS_Graphics2DHandle blitter;
    NEXUS_PlaypumpHandle playpump;
  //  NEXUS_StillDecoderHandle stillDecoder;
    NEXUS_PidChannelHandle pidChannel;
} P_PLAY_THUMBNAIL_INFO;
#endif

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
extern NEXUS_DmaHandle g_ulDmaHandle;

#if defined(CONFIG_USB_FLASH_RECORD)
extern HBOOL g_bTunePdflushForUsbFlash;
#endif


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static P_PLAY_INSTANCE	g_astPbInst[MAX_PLAY_INST_NUM];
static P_PLAYPUMP_INSTANCE	g_astPlaypumpInst[PB_NUM_PLAYPUMPS];

static unsigned long		g_ulPlayPumpSemId;
static unsigned long	g_ulPbTimeTaskMsgQueueId;
static unsigned long 	g_ulPbTimeTaskId;
static unsigned long	g_ulPbTaskEventId;
static unsigned long	g_ulPbEventTaskId;
static unsigned long	g_ulPbPauseEventId;
static unsigned long	g_ulPbPauseTaskId;
static DI_PVR_Event_Callback pbEvtCbBackup[DI_PVR_EVENT_MAX_NUMBER];

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
#if defined(CONFIG_THUMBNAIL_DECODING)
extern DRV_Error DRV_VIDEO_GetThumbnailImage(HUINT32 ulDeviceId, NEXUS_SurfaceHandle *psurfaceHandle, DRV_PVR_THUMBNAIL_SETUP_t *pthumbSetup);
#endif
static DRV_Error P_PLAY_ChangeTrickSpeedToTrickInfo(HUINT32 ulDeviceId, DI_PVR_TRICKSPEED_t eTrickSpeed, NEXUS_PlaybackTrickModeSettings *pstTrickModeSettings);
static DRV_Error P_PLAY_SetDecryption(NEXUS_PlaybackHandle ulPbHandle, NEXUS_KeySlotHandle ulDecKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstDecryptSettings);
static void P_PLAY_GenericCallback(void *context, int param);
static DRV_Error P_PLAY_TryPause(HUINT32 ulDeviceId, HBOOL *pbNeedRetry);
static DRV_Error P_PLAY_ResetPacing(HUINT32 ulDeviceId);
static DRV_Error P_PLAY_SetPosition(HUINT32 ulDeviceId, HUINT32 ulTime, HUINT32 *pulResultTimeInMs);
static DRV_Error P_PLAY_Resume(HUINT32 ulDeviceId);
static void P_PLAY_PauseTimerCallback(unsigned long ulTimerId, void *params);
static void P_PLAY_PauseTask(void *param);
static void P_PLAY_EventTask(void *param);
static void P_PLAY_TimeTask(void *param);

#if defined (USE_PLAYREADY)
static void DRV_pump_update_asf_stream_cfg(void *cntx, unsigned stream_id, basf_stream_cfg *cfg)
{
	void * dcrypt_ctx;
	NEXUS_Playpump_GetDcryptCtx(cntx, dcrypt_ctx);
	bdrm_update_asf_stream_cfg(dcrypt_ctx, stream_id, cfg);
	return;
}
#endif

int P_PLAY_IsDir(const char *dirname)
{
	struct stat st;
	return (0 == stat(dirname, &st) && S_ISDIR(st.st_mode)) ? 1 : 0;
}

DRV_Error DRV_PVR_PLAYPUMP_Init(void)
{
	int err;
	unsigned long i;
	NEXUS_PlaypumpOpenSettings openSettings;

	VK_memset(g_astPlaypumpInst, 0x00, sizeof(P_PLAYPUMP_INSTANCE)*PB_NUM_PLAYPUMPS);

	NEXUS_Playpump_GetDefaultOpenSettings(&openSettings);
	openSettings.fifoSize = DRV_PVR_PLAY_BUFFER;
	openSettings.alignment = 12; /* 4096 bytes (block size) aligment */
	openSettings.numDescriptors = DRV_PVR_PLAY_DESC_NUM;
#if defined (USE_PLAYREADY)
	openSettings.prdyCallback = DRV_pump_update_asf_stream_cfg;
#endif
	for(i=0; i<PB_NUM_PLAYPUMPS; i++)
	{
#if defined(CONFIG_LARGE_SIZE_PLAYPUMP_BUFFER)
		g_astPlaypumpInst[i].ulBuffClass = 0;
		if(i>= PB_NUM_PLAYPUMPS_CLASS0)
		{
			openSettings.fifoSize = DRV_PVR_PLAY_BUFFER_LARGE;
			g_astPlaypumpInst[i].ulBuffClass = 1;
		}
#endif
		g_astPlaypumpInst[i].playpump = NEXUS_Playpump_Open(i, &openSettings);
		if (!g_astPlaypumpInst[i].playpump)
		{
			PrintError("[DRV_PVR_PLAYPUMP_Init] Error in NEXUS_Playpump_Open(%d)\n\r",i);
			return DRV_ERR;
		}
	}

	err = VK_SEM_Create(&g_ulPlayPumpSemId, "playpump", DEFAULT_SUSPEND_TYPE);
	if(err != VK_OK)
	{
		PrintError("[DRV_PVR_PLAYPUMP_Init] Error(0x%x) in VK_SEM_Create()\n\r", err);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_PVR_GetPlaypumpHandleByClass(NEXUS_PlaypumpHandle* pPlaypumpHandle, HUINT32 ulClass)
{
	unsigned long i, startIndex;

#if defined(CONFIG_LARGE_SIZE_PLAYPUMP_BUFFER)
	if(ulClass == 0)
	{
		startIndex = 0;
	}
	else if(ulClass == 1)
	{
		startIndex = PB_NUM_PLAYPUMPS_CLASS0;
	}
	else
	{
		PrintError("[DRV_PVR_GetPlaypumpHandle] Unknown pump class (%u)\n\r", ulClass);
		return DRV_ERR;
	}
#else
	startIndex = 0;
#endif
	if(VK_SEM_Get(g_ulPlayPumpSemId) == VK_OK)
	{
		for( i=startIndex ; i<PB_NUM_PLAYPUMPS ; i++ )
		{
			if(g_astPlaypumpInst[i].bIsUsed == FALSE)
			{
				*pPlaypumpHandle = g_astPlaypumpInst[i].playpump;
				g_astPlaypumpInst[i].bIsUsed = TRUE;
				VK_SEM_Release(g_ulPlayPumpSemId);
				if(ulClass < g_astPlaypumpInst[i].ulBuffClass)
				{
					PrintDebug("[DRV_PVR_GetPlaypumpHandle] allocatd pump buffer is bigger than requested. (requested:%u, alloccated:%u)\n", ulClass, g_astPlaypumpInst[i].ulBuffClass);
				}
				PrintDebug("[DRV_PVR_GetPlaypumpHandle] %d pumpHandle=0x%lx\n\r", i,*pPlaypumpHandle);
				return DRV_OK;
			}
		}
		VK_SEM_Release(g_ulPlayPumpSemId);
	}

	PrintError("[DRV_PVR_GetPlaypumpHandle] Get error\n\r");

	return DRV_ERR;
}

DRV_Error DRV_PVR_GetPlaypumpHandle(NEXUS_PlaypumpHandle* pPlaypumpHandle)
{
	return DRV_PVR_GetPlaypumpHandleByClass(pPlaypumpHandle, BUFF_GROUP_0);
}

DRV_Error DRV_PVR_ReleasePlaypumpHandle(NEXUS_PlaypumpHandle playpumpHandle)
{
	unsigned long i;

	if(VK_SEM_Get(g_ulPlayPumpSemId) == VK_OK)
	{
		for( i=0 ; i<PB_NUM_PLAYPUMPS ; i++ )
		{
			if((g_astPlaypumpInst[i].playpump == playpumpHandle) && (g_astPlaypumpInst[i].bIsUsed == TRUE))
			{
				g_astPlaypumpInst[i].bIsUsed = FALSE;
				VK_SEM_Release(g_ulPlayPumpSemId);
				PrintDebug("[DRV_PVR_ReleasePlaypumpHandle] %d\n\r", i);
				return DRV_OK;
			}
		}
		VK_SEM_Release(g_ulPlayPumpSemId);
	}

	PrintError("[DRV_PVR_ReleasePlaypumpHandle] Release error\n\r");

	return DRV_ERR;
}

void DRV_PVR_PrintPlaypumpStatus(void)
{
	unsigned long i;

	if(VK_SEM_Get(g_ulPlayPumpSemId) == VK_OK)
	{
#if defined(CONFIG_BMEM_OPTIMIZE)
		for( i=0 ; i<CONFIG_NUM_PLAYPUMPS ; i++ )
#else
		for( i=0 ; i<NEXUS_NUM_PLAYPUMPS ; i++ )
#endif
		{
			PrintError("[%d] Allocated:%d BuffClass:%u\n", i, g_astPlaypumpInst[i].bIsUsed, g_astPlaypumpInst[i].ulBuffClass);
		}
		VK_SEM_Release(g_ulPlayPumpSemId);
	}

	return;
}

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
void DRV_PVR_PLAY_Uninit(void)
{
	int i;

	for(i=0; i<MAX_PLAY_INST_NUM; i++)
	{
		if(g_astPbInst[i].decKeyHandle != NULL)
		{
			NEXUS_Security_FreeKeySlot(g_astPbInst[i].decKeyHandle);
			g_astPbInst[i].decKeyHandle = NULL;
		}
	}
}

void DRV_PVR_PLAY_Reinit(void)
{
	int i;
	NEXUS_SecurityKeySlotSettings keySettings;

	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
	for(i=0; i<MAX_PLAY_INST_NUM; i++)
	{
		if(g_astPbInst[i].decKeyHandle != NULL)
		{
			g_astPbInst[i].decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
		}
	}
}
#endif

DRV_Error DRV_PVR_PLAY_Init(void)
{
	int err;
	unsigned long i;
	unsigned long diSemId;
	NEXUS_PlaypumpHandle	playPumpHandle = NULL;
	NEXUS_PlaybackHandle 	playbackHandle = NULL;
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
	NEXUS_SecurityKeySlotSettings keySettings;
#endif
	NEXUS_KeySlotHandle		decKeyHandle = NULL;

	PrintEnter("DRV_PVR_PLAY_Init");

	VK_memset(g_astPbInst, 0x00, sizeof(P_PLAY_INSTANCE)*MAX_PLAY_INST_NUM);

	DRV_PVR_PLAYPUMP_Init();

	for(i=0; i<MAX_PLAY_INST_NUM; i++)
	{
		playbackHandle = NEXUS_Playback_Create();
		if (!playbackHandle)
		{
	        PrintError("[DRV_PVR_PLAY_Init] Error in NEXUS_Playback_Create()\n\r");
			return DRV_ERR;
	    }
#if 0
		NEXUS_Playback_GetSettings(playbackHandle, &PlaySettings);
		PlaySettings.playpump = playPumpHandle;
		neResult = NEXUS_Playback_SetSettings(playbackHandle, &PlaySettings);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[DRV_PVR_PLAY_Init] Error(0x%x) in NEXUS_Playback_SetSettings()\n\r", neResult);
			return DRV_ERR;
		}
#endif
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
		/* Allocate decryption keyslot */
		NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
		keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
		decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
		if(decKeyHandle == NULL)
		{
			PrintError("[DRV_PVR_PLAY_Init] Error in NEXUS_Security_AllocateKeySlot()\n\r");
			return DRV_ERR;
		}
#endif

		err = VK_SEM_Create(&diSemId, "di_pb", VK_SUSPENDTYPE_FIFO);
		if(err != VK_OK)
		{
			PrintError("[DRV_PVR_PLAY_Init] Error(0x%x) in VK_SEM_Create()\n\r", err);
			return DRV_ERR;
		}

		g_astPbInst[i].status = DI_PVR_FREE;
		g_astPbInst[i].diSemId = diSemId;
		g_astPbInst[i].pbPumpHandle = playPumpHandle;
		g_astPbInst[i].pbHandle = playbackHandle;
		g_astPbInst[i].pbPidInfo.videoPidHandle = NULL;
		g_astPbInst[i].pbPidInfo.dummyVideoPidHandle = NULL;
		g_astPbInst[i].pbPidInfo.audioMainPidHandle = NULL;
		g_astPbInst[i].pbPidInfo.dummyAudioMainPidHandle = NULL;
		g_astPbInst[i].pbPidInfo.audioSubPidHandle = NULL;
		g_astPbInst[i].pbPidInfo.dummyAudioSubPidHandle = NULL;
		g_astPbInst[i].decKeyHandle = decKeyHandle;
		g_astPbInst[i].pauseRetry = false;
		g_astPbInst[i].recDeviceInfo.deviceId = -1;
#if defined(TSR_PAUSE_WORKAROUND)
		// workaround, tsr play를 pause로 시작한 경우 hdmi에서 정지 영상 나오지 않은 이슈의 workaround
		g_astPbInst[i].decoderMuteDisableCount = 0;
#endif
	}

	/* playback event notify를 위한 TASK 설정 */
	/* Create event for P_PLAY_EventTask*/
	err = VK_EVENT_Create(&g_ulPbTaskEventId, "pb_evt");
	if(err != VK_OK)
	{
		PrintError("[DRV_PVR_PLAY_Init] Error(0x%x) in VK_EVENT_Create()\n", err);
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_PLAY_EventTask,
						PLAYBACK_EVENT_TASK_PRIORITY,
						PLAYBACK_EVENT_TASK_STACK_SIZE,
						"pbEvent",
						NULL,
						&g_ulPbEventTaskId,
						false);
	if(err != VK_OK)
	{
		PrintError("[DRV_PVR_PLAY_Init] Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}
	VK_TASK_Start(g_ulPbEventTaskId);


	/* Time 정보 notify를 위한 TASK 설정 */
	err = VK_MSG_Create(PB_TIME_MSG_QUEUE_SIZE,
						sizeof(P_PLAY_TASK_MSG),
						"pTimeQ",
						&g_ulPbTimeTaskMsgQueueId,
						VK_SUSPENDTYPE_FIFO);
	if(err != VK_OK)
	{
		PrintError("[DRV_PVR_PLAY_Init] Error(0x%x) in VK_MSG_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_PLAY_TimeTask,
						PLAYBACK_TIME_TASK_PRIORITY,
						PLAYBACK_TIME_TASK_STACK_SIZE,
						"pbTime",
						NULL,
						&g_ulPbTimeTaskId,
						false);
	if(err != VK_OK)
	{
		PrintError("[DRV_PVR_PLAY_Init] Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}
	VK_TASK_Start(g_ulPbTimeTaskId);

	/* Async pause 를 위한 TASK 설정 */
	/* Create event for pause */
	err = VK_EVENT_Create(&g_ulPbPauseEventId, "pb_pause");
	if(err != VK_OK)
	{
		PrintError("[DRV_PVR_PLAY_Init] Error(0x%x) in VK_EVENT_Create()\n", err);
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_PLAY_PauseTask,
						PLAYBACK_LOAD_TASK_PRIORITY,
						PLAYBACK_LOAD_TASK_STACK_SIZE,
						"pbPause",
						NULL,
						&g_ulPbPauseTaskId,
						false);
	if(err != VK_OK)
	{
		PrintError("[DRV_PVR_PLAY_Init] Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}

	#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DRV_PM_RegisterCallback(DRV_PVR_PLAY_Uninit, DRV_PVR_PLAY_Reinit);
	#endif

	VK_TASK_Start(g_ulPbPauseTaskId);

	PrintExit("DRV_PVR_PLAY_Init");

	return DRV_OK;
}

#if defined(CONFIG_FAST_AUDIO_CHANGE)
DRV_Error DRV_PVR_PB_GetPlaybackHandle(NEXUS_PidChannelHandle pstPIDChannel, NEXUS_PlaybackHandle *ppstPlaybackHandle)
{
	if((pstPIDChannel == NULL) || (ppstPlaybackHandle == NULL))
	{
		PrintError("[DRV_PVR_PB_GetPlaybackHandle] Error : Invalid parameter\n\r");
		return DRV_ERR;
	}

	*ppstPlaybackHandle = g_astPbInst[0].pbHandle;

	return DRV_OK;
}
#endif

DI_ERR_CODE DI_PVR_PLAY_GetCapability(HUINT32 *pulNumOfDeviceId)
{
	PrintEnter("DI_PVR_PLAY_GetCapability");
	*pulNumOfDeviceId = MAX_PLAY_INST_NUM;
	PrintExit("DI_PVR_PLAY_GetCapability");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_PLAY_RegisterEventCallback(HUINT32 ulDeviceId, DI_PVR_EVENT_t eEvent, DI_PVR_Event_Callback pCallbackFunc)
{
	PrintEnter("DI_PVR_PLAY_RegisterEventCallback");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_RegisterEventCallback] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		switch(eEvent)
		{
			case DI_PVR_EVENT_EOF:
				g_astPbInst[ulDeviceId].pbEvtCbFunc[DI_PVR_EVENT_EOF] = pCallbackFunc;
				break;
			case DI_PVR_EVENT_BOF:
				g_astPbInst[ulDeviceId].pbEvtCbFunc[DI_PVR_EVENT_BOF] = pCallbackFunc;
				break;
			case DI_PVR_EVENT_TIMECODE_CHANGED:
				g_astPbInst[ulDeviceId].pbEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED] = pCallbackFunc;
				break;
			case DI_PVR_EVENT_DISK_FULL:
				g_astPbInst[ulDeviceId].pbEvtCbFunc[DI_PVR_EVENT_DISK_FULL] = pCallbackFunc;
				break;
			default:
				PrintError("[DI_PVR_PLAY_RegisterEventCallback] Invalid event! (eEvent : %d)\n\r", eEvent);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
				break;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}
	else
	{
		PrintError("[DI_PVR_PLAY_RegisterEventCallback] VK_SEM_Get failed.\n");
		return DI_ERR_ERROR;
	}

	PrintExit("DI_PVR_PLAY_RegisterEventCallback");

	return DI_ERR_OK;
}

#if defined(CONFIG_THUMBNAIL_DECODING)
static char g_AVFileName[256];
static char g_IndexFileName[256];
#endif
DI_ERR_CODE DI_PVR_PLAY_Setup(HUINT32 ulDeviceId, DI_PVR_PLAY_SETUP_t *pstSetup )
{
	NEXUS_Error	neResult;
	DRV_Error drvErr;
	char *avFileName = NULL;
	char *indexFileName = NULL;
	NEXUS_FilePlayHandle	filePlayHandle = NULL;
	NEXUS_PlaypumpHandle	playPumpHandle;
	NEXUS_PlaybackSettings	PlaySettings;
#if defined(CONFIG_RECORD)
	DRV_PVR_REC_DEVICE_INFO recDevInfo;
#endif
#if defined(ENABLE_CHUNK_FILE_PLAYBACK)
	HBOOL	isChunkedPlay = false;
#endif
	HBOOL	isExternalPlay = false;

	PrintEnter("DI_PVR_PLAY_Setup");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_Setup] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPbInst[ulDeviceId].status == DI_PVR_FREE)
		{
			avFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(avFileName == NULL)
			{
				PrintError("[DI_PVR_PLAY_Setup] Error in DD_MEM_Alloc()\n\r");
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			indexFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(indexFileName == NULL)
			{
				PrintError("[DI_PVR_PLAY_Setup] Error in DD_MEM_Alloc()\n\r");
				DD_MEM_Free(avFileName);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
#if defined(CONFIG_THUMBNAIL_DECODING)
			VK_memset(g_AVFileName,0,256);
			VK_memset(g_IndexFileName,0,256);
#endif
			VK_memset(avFileName,0,DI_PVR_MAX_NAME_LEN);
			VK_memset(indexFileName,0,DI_PVR_MAX_NAME_LEN);
			VK_memcpy((void *)avFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
#if defined(CONFIG_USE_PRIVATE_FILE_EXTENTION_HTS)
			VK_strncat(avFileName, ".hts\0", 4);
#else
			VK_strncat(avFileName, ".ts\0", 3); // Add  fileExtension
#endif
			VK_memcpy((void *)indexFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
			VK_strncat(indexFileName, ".nts\0", 4); // Add  fileExtension
#if defined(ENABLE_CHUNK_FILE_PLAYBACK)
			if(P_PLAY_IsDir(avFileName) != 0)
			{
				isChunkedPlay = true;
			}
#endif
#if defined(CONFIG_THUMBNAIL_DECODING)
			VK_memcpy((void *)g_AVFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
#if defined(CONFIG_USE_PRIVATE_FILE_EXTENTION_HTS)
			VK_strncat(g_AVFileName, ".hts\0", 4);
#else
			VK_strncat(g_AVFileName, ".ts\0", 3); // Add  fileExtension
#endif
			VK_memcpy((void *)g_IndexFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
			VK_strncat(g_IndexFileName, ".nts\0", 4); // Add  fileExtension
#endif
#if defined(CONFIG_RECORD)
			if(pstSetup->bIsRecording || pstSetup->eFileType == eDI_PVR_FileType_Circular)
			{
				if(g_astPbInst[ulDeviceId].recDeviceInfo.deviceId == -1)
				{
					PrintError("[DI_PVR_PLAY_Setup] Invalid record device ID!\n\r");
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
				drvErr = DRV_PVR_REC_GetDeviceInfo((HUINT32)g_astPbInst[ulDeviceId].recDeviceInfo.deviceId, &recDevInfo);
				if (drvErr != DRV_OK)
				{
					PrintError("[DI_PVR_PLAY_Setup] Error(0x%x) in DRV_PVR_REC_GetDeviceInfo()\n\r", drvErr);
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
			}
#endif
			switch(pstSetup->eFileType)
			{
				case eDI_PVR_FileType_Linear :
#if defined(ENABLE_CHUNK_FILE_PLAYBACK)
					if(isChunkedPlay == true)
					{
						filePlayHandle = NEXUS_ChunkedFilePlay_Open(avFileName, indexFileName, NULL);
					}
					else
					{
						filePlayHandle = NEXUS_FilePlay_OpenPosix(avFileName, indexFileName);
					}
#else
#if defined(CONFIG_USB_FLASH_RECORD)
					if(pstSetup->bIsUsbFlashRecord == TRUE)
					{
						filePlayHandle = NEXUS_FilePlay_OpenPosix_Nodirect(avFileName, indexFileName);
					}
					else
#endif
					{
						filePlayHandle = NEXUS_FilePlay_OpenPosix(avFileName, indexFileName);
					}
#endif
					if (!filePlayHandle)
					{
				        PrintError("[DI_PVR_PLAY_Setup] Error in NEXUS_FilePlay_OpenPosix()\n\r");
						DD_MEM_Free(avFileName);
						DD_MEM_Free(indexFileName);
						VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
						return DI_ERR_ERROR;
				    }
					if(pstSetup->bIsRecording)
					{
					    neResult = NEXUS_Record_AddPlayback(recDevInfo.recordHandle, g_astPbInst[ulDeviceId].pbHandle);
						if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[DI_PVR_PLAY_Setup] Error(0x%x) in NEXUS_Record_AddPlayback()\n\r", neResult);
							DD_MEM_Free(avFileName);
							DD_MEM_Free(indexFileName);
							NEXUS_FilePlay_Close(filePlayHandle);
							VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
							return DI_ERR_ERROR;
						}
				    }
					break;
#if defined(CONFIG_RECORD)
				case eDI_PVR_FileType_Circular :
#if defined(ENABLE_CHUNK_FILE_PLAYBACK)
					filePlayHandle = NEXUS_ChunkedFifoPlay_Open(avFileName, indexFileName, recDevInfo.fifoRecHandle);
#else
#if defined(CONFIG_USB_FLASH_RECORD)
					if(pstSetup->bIsUsbFlashRecord == TRUE)
					{
						filePlayHandle = NEXUS_FifoPlay_OpenForUsbFlash(avFileName, indexFileName, recDevInfo.fifoRecHandle);
					}
					else
#endif
					{
						filePlayHandle = NEXUS_FifoPlay_Open(avFileName, indexFileName, recDevInfo.fifoRecHandle);
					}
#endif
					if (!filePlayHandle)
					{
				        PrintError("[DI_PVR_PLAY_Setup] Error in NEXUS_FifoPlay_Open()\n\r");
						DD_MEM_Free(avFileName);
						DD_MEM_Free(indexFileName);
						VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
						return DI_ERR_ERROR;
				    }
				    neResult = NEXUS_Record_AddPlayback(recDevInfo.recordHandle, g_astPbInst[ulDeviceId].pbHandle);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_PVR_PLAY_Setup] Error(0x%x) in NEXUS_Record_AddPlayback()\n\r", neResult);
						DD_MEM_Free(avFileName);
						DD_MEM_Free(indexFileName);
						NEXUS_FilePlay_Close(filePlayHandle);
						VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
						return DI_ERR_ERROR;
					}
					break;
#endif
				default:
					PrintError("[DI_PVR_PLAY_Setup] Invalid file type! (pstSetup->eFileType : %d)\n\r", pstSetup->eFileType);
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			}
			DD_MEM_Free(avFileName);
			DD_MEM_Free(indexFileName);

			drvErr = DRV_DEMUX_GetPlaybackOutputPath(ulDeviceId, &isExternalPlay);
			if (drvErr != DRV_OK)
			{
		        PrintError("[DI_PVR_PLAY_Setup] Error(0x%x) in DRV_DEMUX_GetPlaybackOutputPath()\n\r", drvErr);
				NEXUS_FilePlay_Close(filePlayHandle);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
		    }

			drvErr = DRV_PVR_GetPlaypumpHandle(&playPumpHandle);
			if (drvErr != DRV_OK)
			{
		        PrintError("[DI_PVR_PLAY_Setup] Error(0x%x) in DRV_PVR_GetPlaypumpHandle()\n\r", drvErr);
				NEXUS_FilePlay_Close(filePlayHandle);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
		    }
#if defined(CONFIG_DELAYED_RECORD)
			if(pstSetup->bIsRecording && recDevInfo.delayedRecordOn == 1)
			{
				g_astPbInst[ulDeviceId].isDelayedRecording = 1;
			}
#endif
			NEXUS_Playback_GetSettings(g_astPbInst[ulDeviceId].pbHandle, &PlaySettings);
			PlaySettings.playpump = playPumpHandle;
			PlaySettings.playpumpSettings.allPass = isExternalPlay ? true : false;
			PlaySettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
			PlaySettings.playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_e32_Binary;
#else
			PlaySettings.playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eMod300;
#endif
			PlaySettings.playpumpSettings.timestamp.timebase = NEXUS_Timebase_eInvalid;
			PlaySettings.playpumpSettings.timestamp.pacing = isExternalPlay ? true : false;
			PlaySettings.playpumpSettings.timestamp.pacingMaxError = 2048;
#endif
		    PlaySettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay; /* EOF->change to pause or normal play(TSR) */
		    PlaySettings.beginningOfStreamAction = NEXUS_PlaybackLoopMode_ePlay; /* BOF->change to normal play */
		    PlaySettings.timeshifting = pstSetup->bIsRecording  ? true : false;
		    PlaySettings.endOfStreamCallback.callback = P_PLAY_GenericCallback;
		    PlaySettings.endOfStreamCallback.context = (void *)g_ulPbTaskEventId;
		    PlaySettings.endOfStreamCallback.param = PB_EOF_EVENTS;
		    PlaySettings.beginningOfStreamCallback.callback = P_PLAY_GenericCallback;
		    PlaySettings.beginningOfStreamCallback.context = (void *)g_ulPbTaskEventId;
		    PlaySettings.beginningOfStreamCallback.param = PB_BOF_EVENTS;
		    PlaySettings.errorCallback.callback = P_PLAY_GenericCallback;
		    PlaySettings.errorCallback.context = (void *)g_ulPbTaskEventId;
		    PlaySettings.errorCallback.param = (int)NULL;
			if(pstSetup->stDecryptSettings.eEncType == DI_PVR_ENCRYPTION_TYPE_NONE)
			{
				PlaySettings.playpumpSettings.securityDma = NULL;
    			PlaySettings.playpumpSettings.securityContext = NULL;
			}
			PlaySettings.startPaused = false; // To do : 이 부분 어떻게?
			neResult = NEXUS_Playback_SetSettings(g_astPbInst[ulDeviceId].pbHandle, &PlaySettings);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[DI_PVR_PLAY_Setup] Error(0x%x) in NEXUS_Playback_SetSettings()\n\r", neResult);
				NEXUS_FilePlay_Close(filePlayHandle);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
		    }

			if(pstSetup->stDecryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
			{
				drvErr = P_PLAY_SetDecryption(g_astPbInst[ulDeviceId].pbHandle, g_astPbInst[ulDeviceId].decKeyHandle, &(pstSetup->stDecryptSettings));
				if (drvErr != DRV_OK)
				{
			        PrintError("[DI_PVR_PLAY_Setup] Error(0x%x) in P_PLAY_SetDecryption()\n\r", drvErr);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					NEXUS_FilePlay_Close(filePlayHandle);
					return DI_ERR_ERROR;
			    }
			}

			g_astPbInst[ulDeviceId].setup = *pstSetup;
			g_astPbInst[ulDeviceId].isExternalPlay = (int)isExternalPlay;
			g_astPbInst[ulDeviceId].filePbHandle = filePlayHandle;
			g_astPbInst[ulDeviceId].pbPumpHandle = playPumpHandle;
			g_astPbInst[ulDeviceId].lastPacingTsRangeError = 0;
#if defined(CONFIG_RECORD)
			if(pstSetup->bIsRecording || pstSetup->eFileType == eDI_PVR_FileType_Circular)
			{
				g_astPbInst[ulDeviceId].recDeviceInfo = recDevInfo;
			}
#endif
			g_astPbInst[ulDeviceId].status = DI_PVR_ALLOCATED;
		}
		else
		{
			PrintError("[DI_PVR_PLAY_Setup] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
#if defined(CONFIG_USB_FLASH_RECORD)
		if(pstSetup->bIsUsbFlashRecord == TRUE)
		{
			if(g_bTunePdflushForUsbFlash == FALSE)
			{
				DRV_PVR_TunePdflush(TRUE);
				g_bTunePdflushForUsbFlash = TRUE;
			}
		}
		else
		{
			if(g_bTunePdflushForUsbFlash == TRUE)
			{
				DRV_PVR_TunePdflush(FALSE);
				g_bTunePdflushForUsbFlash = FALSE;
			}
		}
#endif
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_Setup");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_PLAY_Start(HUINT32 ulDeviceId )
{
	NEXUS_Error	neResult;
	int err;
	NEXUS_PlaybackStartSettings playStartSettings;
	NEXUS_PlaybackPosition	startTime;
	NEXUS_PlaybackStatus	playStatus;
	P_PLAY_TASK_MSG	timeTaskMsg;
	DI_ERR_CODE diError;

	PrintEnter("DI_PVR_PLAY_Start");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_Start] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPbInst[ulDeviceId].status == DI_PVR_ALLOCATED)
		{
			NEXUS_Playback_GetDefaultStartSettings(&playStartSettings);
#if	1 //defined(CONFIG_DELAYED_RECORD) Delayed record를 지원하지 않더라도 delayed record file을 재생할 수는 있어야 한다. (for contents sharing)
			playStartSettings.delayRecordedOffsetAlign = DRV_PVR_ATOM_SIZE;
#endif
#if defined(CONFIG_DELAYED_RECORD)
			if(g_astPbInst[ulDeviceId].isDelayedRecording == 1)
			{
				playStartSettings.delayedRecordingStatus = ((playStartSettings.delayedRecordingStatus & 0xFFFFFFFE) |0x00000001);
			}
#endif
			neResult = NEXUS_Playback_Start(g_astPbInst[ulDeviceId].pbHandle, g_astPbInst[ulDeviceId].filePbHandle, &playStartSettings);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[DI_PVR_PLAY_Start] Error(0x%x) in NEXUS_Playback_Start()\n\r", neResult);
				diError = DI_ERR_ERROR;
				goto start_error;
		    }

			g_astPbInst[ulDeviceId].playSpeed = eDI_PVR_Speed_Normal;
			g_astPbInst[ulDeviceId].status = DI_PVR_ENABLED;

			switch(g_astPbInst[ulDeviceId].setup.eStartCondition)
			{
				case DI_PVR_PLAY_START_MODE_LASTVIEW:
					startTime = (NEXUS_PlaybackPosition)g_astPbInst[ulDeviceId].setup.ulStartTime;
					neResult = NEXUS_Playback_GetStatus(g_astPbInst[ulDeviceId].pbHandle, &playStatus);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_PVR_PLAY_Start] Error(0x%x) in NEXUS_Playback_GetStatus()\n\r", neResult);
						diError = DI_ERR_ERROR;
						goto error;
					}

					startTime = startTime*1000; //sec -> ms
					if (startTime > playStatus.last)
					{
						startTime = playStatus.last;
					}
					else if(startTime < playStatus.first)
					{
						startTime = playStatus.first;
					}

					neResult = NEXUS_Playback_Seek(g_astPbInst[ulDeviceId].pbHandle, startTime);
		    		if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_PVR_PLAY_Start] Error(0x%x) in NEXUS_Playback_Seek\n\r", neResult);
						diError = DI_ERR_ERROR;
						goto error;
					}
					g_astPbInst[ulDeviceId].lastTimeStatus = (unsigned long)startTime;
					break;
				case DI_PVR_PLAY_START_MODE_END:
					neResult = NEXUS_Playback_GetStatus(g_astPbInst[ulDeviceId].pbHandle, &playStatus);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_PVR_PLAY_Start] Error(0x%x) in NEXUS_Playback_GetStatus()\n\r", neResult);
						diError = DI_ERR_ERROR;
						goto error;
					}
					neResult = NEXUS_Playback_Seek(g_astPbInst[ulDeviceId].pbHandle, playStatus.last-3*1000);
		    		if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_PVR_PLAY_Start] Error(0x%x) in NEXUS_Playback_Seek\n\r", neResult);
						diError = DI_ERR_ERROR;
						goto error;
					}
					g_astPbInst[ulDeviceId].lastTimeStatus = (unsigned long)playStatus.last;
					break;
				case DI_PVR_PLAY_START_MODE_BEGIN:
					g_astPbInst[ulDeviceId].lastTimeStatus = (unsigned long)0;
					break;
				default:
					PrintError("[DI_PVR_PLAY_Start] Invalid start condition (eStartCondition : %d)\n\r", g_astPbInst[ulDeviceId].setup.eStartCondition);
					diError = DI_ERR_ERROR;
					goto error;
					break;
			}

			timeTaskMsg.deviceId = ulDeviceId;
			timeTaskMsg.msgId = P_PLAY_MSG_TIMECODE_START;
			err = VK_MSG_Send(g_ulPbTimeTaskMsgQueueId, (void *)&timeTaskMsg, sizeof(P_PLAY_TASK_MSG));
			if(err != VK_OK)
			{
				PrintError("[DI_PVR_PLAY_Start] Error(0x%x) in VK_MSG_Send\n\r", err);
				diError = DI_ERR_ERROR;
				goto error;
			}

			//g_astPbInst[ulDeviceId].playSpeed = eDI_PVR_Speed_Normal;
			//g_astPbInst[ulDeviceId].status = DI_PVR_ENABLED;
		}
		else
		{
			PrintError("[DI_PVR_PLAY_Start] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_Start");

	return DI_ERR_OK;

error:
	NEXUS_Playback_Stop(g_astPbInst[ulDeviceId].pbHandle);
start_error:
#if defined(CONFIG_RECORD)
		if(g_astPbInst[ulDeviceId].setup.eFileType == eDI_PVR_FileType_Circular || g_astPbInst[ulDeviceId].setup.bIsRecording)
		{
			if(g_astPbInst[ulDeviceId].recDeviceInfo.deviceId != -1)
			{
				NEXUS_Record_RemovePlayback(g_astPbInst[ulDeviceId].recDeviceInfo.recordHandle, g_astPbInst[ulDeviceId].pbHandle);
			}
		}
#endif
		DRV_PVR_ReleasePlaypumpHandle(g_astPbInst[ulDeviceId].pbPumpHandle);
		NEXUS_FilePlay_Close(g_astPbInst[ulDeviceId].filePbHandle);
		g_astPbInst[ulDeviceId].status = DI_PVR_FREE;
		g_astPbInst[ulDeviceId].filePbHandle = NULL;
		g_astPbInst[ulDeviceId].pbPumpHandle = NULL;
		g_astPbInst[ulDeviceId].pauseRetry = false;
		g_astPbInst[ulDeviceId].recDeviceInfo.deviceId = -1;
#if defined(CONFIG_DELAYED_RECORD)
		g_astPbInst[ulDeviceId].isDelayedRecording = 0;
#endif
	VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);

	return diError;
}

DI_ERR_CODE DI_PVR_PLAY_Stop(HUINT32 ulDeviceId )
{
	NEXUS_Error neResult;
	int err;
	P_PLAY_TASK_MSG timeTaskMsg;

	PrintEnter("DI_PVR_PLAY_Stop");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_Stop] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
#if defined(CONFIG_RECORD)
		if(g_astPbInst[ulDeviceId].setup.eFileType == eDI_PVR_FileType_Circular || g_astPbInst[ulDeviceId].setup.bIsRecording)
		{
			if(g_astPbInst[ulDeviceId].recDeviceInfo.deviceId == -1)
			{
				PrintError("[DI_PVR_PLAY_Stop] Invalid record device ID!\n\r");
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			NEXUS_Record_RemovePlayback(g_astPbInst[ulDeviceId].recDeviceInfo.recordHandle, g_astPbInst[ulDeviceId].pbHandle);
			/*
		   		Proceed without checking return value.
		   		Record can be stopped before this playback instance ended.
			*/
		}
#endif

		if( !(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED || g_astPbInst[ulDeviceId].status == DI_PVR_ALLOCATED) || g_astPbInst[ulDeviceId].filePbHandle == NULL)
		{
			PrintError("[DI_PVR_PLAY_Stop] Invalid device status! (status : %d, filePbHandle : 0x%x)\n\r",
				g_astPbInst[ulDeviceId].status, (unsigned long)g_astPbInst[ulDeviceId].filePbHandle);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}

		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			if(g_astPbInst[ulDeviceId].isExternalPlay)
			{
				neResult = NEXUS_Playpump_SetPause(g_astPbInst[ulDeviceId].pbPumpHandle, false);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[DI_PVR_PLAY_Stop] Error(0x%x) in NEXUS_Playpump_SetPause\n\r", neResult);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
			}

			timeTaskMsg.deviceId = ulDeviceId;
			timeTaskMsg.msgId = P_PLAY_MSG_TIMECODE_END;
			err = VK_MSG_Send(g_ulPbTimeTaskMsgQueueId, (void *)&timeTaskMsg, sizeof(P_PLAY_TASK_MSG));
			if(err != VK_OK)
			{
				PrintError("[DI_PVR_PLAY_Stop] Error(0x%x) in VK_MSG_Send\n\r", err);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
		}

		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			NEXUS_Playback_Stop(g_astPbInst[ulDeviceId].pbHandle);
		}
		DRV_PVR_ReleasePlaypumpHandle(g_astPbInst[ulDeviceId].pbPumpHandle);
		NEXUS_FilePlay_Close(g_astPbInst[ulDeviceId].filePbHandle);

		g_astPbInst[ulDeviceId].status = DI_PVR_FREE;
		g_astPbInst[ulDeviceId].filePbHandle = NULL;
		g_astPbInst[ulDeviceId].pbPumpHandle = NULL;
		g_astPbInst[ulDeviceId].pauseRetry = false;
		g_astPbInst[ulDeviceId].recDeviceInfo.deviceId = -1;
#if defined(CONFIG_DELAYED_RECORD)
		g_astPbInst[ulDeviceId].isDelayedRecording = 0;
#endif
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_Stop");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_PLAY_SetDecryption(HUINT32 ulDeviceId, DI_PVR_CRYPTO_SETTINGS *pstDecryptSettings)
{
	DRV_Error drvErr;

	PrintEnter("DI_PVR_PLAY_SetDecryption");

	if(pstDecryptSettings == NULL)
	{
		PrintError("[DI_PVR_PLAY_SetDecryption] pstDecryptSettings == NULL\n\r");
		return DI_ERR_INVALID_PARAM;
	}

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_SetDecryption] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPbInst[ulDeviceId].status == DI_PVR_ALLOCATED)
		{
			drvErr = P_PLAY_SetDecryption(g_astPbInst[ulDeviceId].pbHandle, g_astPbInst[ulDeviceId].decKeyHandle, pstDecryptSettings);
			if (drvErr != DRV_OK)
			{
		        PrintError("[DI_PVR_PLAY_SetDecryption] Error(0x%x) in P_PLAY_SetDecryption()\n\r", drvErr);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
		    }
			g_astPbInst[ulDeviceId].setup.stDecryptSettings = *pstDecryptSettings;
		}
		else
		{
			PrintError("[DI_PVR_PLAY_SetDecryption] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_SetDecryption");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_PLAY_Pause(HUINT32 ulDeviceId )
{
	int err;
	DRV_Error drvErr;

	PrintEnter("DI_PVR_PLAY_Pause");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_Pause] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		g_astPbInst[ulDeviceId].pauseRetry = false;

		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			drvErr = P_PLAY_TryPause(ulDeviceId, (HBOOL *)&g_astPbInst[ulDeviceId].pauseRetry);
			if(drvErr != DRV_OK)
			{
				PrintError("[DI_PVR_PLAY_Pause] Error(0x%x) in P_PLAY_TryPause\n\r", drvErr);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			if(g_astPbInst[ulDeviceId].pauseRetry)
			{
				err = VK_TIMER_EventAfter(PB_PAUSE_TIMER_VALUE, P_PLAY_PauseTimerCallback, NULL, 0, &g_astPbInst[ulDeviceId].pauseTimerId);
				if(err != VK_OK)
				{
					PrintError("[DI_PVR_PLAY_Pause] Error(0x%x) in VK_TIMER_EventAfter\n\r", err);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
			}
			else
			{
				g_astPbInst[ulDeviceId].playSpeed = eDI_PVR_Speed_Pause;
				g_astPbInst[ulDeviceId].status = DI_PVR_PAUSED;
			}
		}
		else
		{
			PrintError("[DI_PVR_PLAY_Pause] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_Pause");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_PLAY_Resume(HUINT32 ulDeviceId )
{
	DRV_Error drvErr;

	PrintEnter("DI_PVR_PLAY_Resume");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_Resume] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		g_astPbInst[ulDeviceId].pauseRetry = false;

		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			drvErr = P_PLAY_Resume(ulDeviceId);
			if(drvErr != DRV_OK)
			{
				PrintError("[DI_PVR_PLAY_Resume] Error(0x%x) in P_PLAY_Resume\n\r", drvErr);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			g_astPbInst[ulDeviceId].playSpeed = eDI_PVR_Speed_Normal;
			g_astPbInst[ulDeviceId].status = DI_PVR_ENABLED;
		}
		else
		{
			PrintError("[DI_PVR_PLAY_Resume] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_Resume");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_PLAY_FrameAdvance(HUINT32 ulDeviceId, HBOOL bForward )
{
	NEXUS_Error	neResult;
	NEXUS_PlaybackStatus	playStatus;

	PrintEnter("DI_PVR_PLAY_FrameAdvance");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_FrameAdvance] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		if((g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED) /* Pause 상태를 전제로 함 */
			&& (g_astPbInst[ulDeviceId].setup.eStreamType == eDI_PVR_Stream_Video))
		{
			if(g_astPbInst[ulDeviceId].isExternalPlay)
			{
				neResult = NEXUS_Playpump_SetPause(g_astPbInst[ulDeviceId].pbPumpHandle, false);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[DI_PVR_PLAY_FrameAdvance] Error(0x%x) in NEXUS_Playpump_SetPause\n\r", neResult);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
			}

			neResult = NEXUS_Playback_FrameAdvance(g_astPbInst[ulDeviceId].pbHandle, (bool)bForward);
		    if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[DI_PVR_PLAY_FrameAdvance] Error(0x%x) in NEXUS_Playback_FrameAdvance\n\r", neResult);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}

			if(g_astPbInst[ulDeviceId].isExternalPlay)
			{
				neResult = NEXUS_Playpump_SetPause(g_astPbInst[ulDeviceId].pbPumpHandle, true);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[DI_PVR_PLAY_FrameAdvance] Error(0x%x) in NEXUS_Playpump_SetPause\n\r", neResult);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
			}

			/* Pause 상태에서는 진행 방향을 forward로 가정하기 때문에 !bForward인 경우  P_PLAY_TimeTask에서의 time notify시 반영되지 않을 것임.
			  따라서 이 함수에서 바로 lastTimeStatus를 update하여 time notify시 반영되도록 함. */
			if(!bForward)
			{
				neResult = NEXUS_Playback_GetStatus(g_astPbInst[ulDeviceId].pbHandle, &playStatus);
				if(neResult == NEXUS_SUCCESS)
				{
					if(playStatus.position < g_astPbInst[ulDeviceId].lastTimeStatus)
					{
						g_astPbInst[ulDeviceId].lastTimeStatus = playStatus.position;
					}
				}
			}
		}
		else
		{
			PrintError("[DI_PVR_PLAY_FrameAdvance] Invalid device status! (status : %d, eStreamType : %d)\n\r",
				g_astPbInst[ulDeviceId].status, g_astPbInst[ulDeviceId].setup.eStreamType);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_FrameAdvance");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_PLAY_SetSpeed(HUINT32 ulDeviceId, DI_PVR_TRICKSPEED_t eSpeed )
{
	NEXUS_Error	neResult;
	int err;
	DRV_Error drvErr;
	NEXUS_PlaybackTrickModeSettings trickModeSettings;

	PrintEnter("DI_PVR_PLAY_SetSpeed");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_SetSpeed] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		g_astPbInst[ulDeviceId].pauseRetry = false;

		/* 중복 동작도 NEXUS에서 처리하기 때문에 status를 따지지 않는다.(pause/resume도 마찬가지..) */
		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			if(eSpeed == eDI_PVR_Speed_Pause)
			{
				drvErr = P_PLAY_TryPause(ulDeviceId, (HBOOL *)&g_astPbInst[ulDeviceId].pauseRetry);
				if(drvErr != DRV_OK)
				{
					PrintError("[DI_PVR_PLAY_SetSpeed] Error(0x%x) in P_PLAY_TryPause\n\r", drvErr);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
				if(g_astPbInst[ulDeviceId].pauseRetry)
				{
					err = VK_TIMER_EventAfter(PB_PAUSE_TIMER_VALUE, P_PLAY_PauseTimerCallback, NULL, 0, &g_astPbInst[ulDeviceId].pauseTimerId);
					if(err != VK_OK)
					{
						PrintError("[DI_PVR_PLAY_SetSpeed] Error(0x%x) in VK_TIMER_EventAfter\n\r", err);
						VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
						return DI_ERR_ERROR;
					}
				}
				else
				{
					g_astPbInst[ulDeviceId].playSpeed = eSpeed;
					g_astPbInst[ulDeviceId].status = DI_PVR_PAUSED;
				}
			}
			else if(eSpeed == eDI_PVR_Speed_Normal)
			{
				drvErr = P_PLAY_Resume(ulDeviceId);
				if(drvErr != DRV_OK)
				{
					PrintError("[DI_PVR_PLAY_SetSpeed] Error(0x%x) in P_PLAY_Resume\n\r", drvErr);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
				g_astPbInst[ulDeviceId].playSpeed = eSpeed;
				g_astPbInst[ulDeviceId].status = DI_PVR_ENABLED;
			}
			else
			{
				drvErr = P_PLAY_ChangeTrickSpeedToTrickInfo(ulDeviceId, eSpeed, &trickModeSettings);
				if(drvErr != DRV_OK)
				{
					PrintError("[DI_PVR_PLAY_SetSpeed] Error(0x%x) in P_PLAY_ChangeTrickStateToTrickInfo\n\r", drvErr);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}

			    neResult = NEXUS_Playback_TrickMode(g_astPbInst[ulDeviceId].pbHandle, &trickModeSettings);
			    if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[DI_PVR_PLAY_SetSpeed] Error(0x%x) in NEXUS_Playback_TrickMode\n\r", neResult);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
				g_astPbInst[ulDeviceId].playSpeed = eSpeed;
				g_astPbInst[ulDeviceId].status = DI_PVR_ENABLED;
			}
		}
		else
		{
			PrintError("[DI_PVR_PLAY_SetSpeed] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_SetSpeed");

	return DI_ERR_OK;
}

static DRV_Error P_PVR_PLAY_GetSpeed(HUINT32 ulDeviceId, DI_PVR_TRICKSPEED_t *peSpeed )
{
	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_GetSpeed] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			*peSpeed = g_astPbInst[ulDeviceId].playSpeed;
		}
		else
		{
			PrintError("[DI_PVR_PLAY_GetSpeed] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DRV_ERR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}


	return DRV_OK;
}

DI_ERR_CODE DI_PVR_PLAY_GetSpeed(HUINT32 ulDeviceId, DI_PVR_TRICKSPEED_t *peSpeed )
{
	DRV_Error drvError = DRV_OK;
	DI_ERR_CODE diError = DI_ERR_OK;

	PrintEnter("DI_PVR_PLAY_GetSpeed");

	drvError = P_PVR_PLAY_GetSpeed(ulDeviceId, peSpeed);
	if(drvError == DRV_ERR)
	{
		diError = DI_ERR_ERROR;
	}
	PrintExit("DI_PVR_PLAY_GetSpeed");

	return diError;
}

DRV_Error DRV_PVR_PLAY_IsPaused(HUINT32 ulDeviceId, HBOOL *pbPause )
{
	DRV_Error drvError = DRV_OK;
	DI_PVR_TRICKSPEED_t eSpeed;


	PrintEnter("DI_PVR_PLAY_GetSpeed");

	drvError = P_PVR_PLAY_GetSpeed(ulDeviceId, &eSpeed);
	if(drvError != DRV_OK)
	{
		return drvError;
	}
	if(eSpeed == eDI_PVR_Speed_Pause)
	{
		*pbPause = TRUE;
	}
	else
	{
		*pbPause = FALSE;
	}
	PrintExit("DI_PVR_PLAY_GetSpeed");

	return drvError;
}


DI_ERR_CODE DI_PVR_PLAY_SetPosition(HUINT32 ulDeviceId, HUINT32 ulTime )
{
	DRV_Error drvErr;
	HUINT32 seekResultTimeInMs;

	PrintEnter("DI_PVR_PLAY_SetPosition");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_SetPosition] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		/* status == DI_PVR_PAUSED 에서 set position 실행하면 jump 지점에서 paused */
		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			drvErr = P_PLAY_SetPosition(ulDeviceId, ulTime, &seekResultTimeInMs);
			if(drvErr != DRV_OK)
			{
				PrintError("[DI_PVR_PLAY_SetPosition] Error(0x%x) in P_PLAY_SetPosition\n\r", drvErr);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			g_astPbInst[ulDeviceId].lastTimeStatus = (unsigned long)seekResultTimeInMs;
		}
		else
		{
			PrintError("[DI_PVR_PLAY_SetPosition] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	DRV_VIDEO_ClearTimeoutTimer();

	PrintExit("DI_PVR_PLAY_SetPosition");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_PLAY_GetPosition(HUINT32 ulDeviceId, HUINT32 *pulTime )
{
	NEXUS_Error	neResult;
	NEXUS_PlaybackStatus	playStatus;

	PrintEnter("DI_PVR_PLAY_GetPosition");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_GetPosition] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			neResult = NEXUS_Playback_GetStatus(g_astPbInst[ulDeviceId].pbHandle, &playStatus);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[DI_PVR_PLAY_GetPosition] Error(0x%x) in NEXUS_Playback_GetStatus\n\r", neResult);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			*pulTime = (HUINT32)(playStatus.position/1000); //ms -> sec
		}
		else
		{
			PrintError("[DI_PVR_PLAY_GetPosition] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_GetPosition");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_PLAY_GetDuration(HUINT32 ulDeviceId, HUINT32 *pulDuration )
{
	PrintEnter("DI_PVR_PLAY_GetDuration");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_GetDuration] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			BSTD_UNUSED(pulDuration); // for clean compile
		}
		else
		{
			PrintError("[DI_PVR_PLAY_GetDuration] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_GetDuration");

	return DI_ERR_OK;
}

DI_ERR_CODE  DI_PVR_PLAY_ConvertTimeToPosition(HUINT32 ulDeviceId, HUINT32 ulTime, HUINT64 *pulPosition)
{
	PrintEnter("DI_PVR_PLAY_ConvertTimeToPosition");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_ConvertTimeToPosition] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			BSTD_UNUSED(ulTime); // for clean compile
			BSTD_UNUSED(pulPosition); // for clean compile
		}
		else
		{
			PrintError("[DI_PVR_PLAY_ConvertTimeToPosition] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_ConvertTimeToPosition");

	return DI_ERR_OK;
}

DI_ERR_CODE  DI_PVR_PLAY_ConvertPositionToTime(HUINT32 ulDeviceId, HUINT64 ulPosition, HUINT32 *pulTime)
{
	PrintEnter("DI_PVR_PLAY_ConvertPositionToTime");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DI_PVR_PLAY_ConvertPositionToTime] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPbInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPbInst[ulDeviceId].status == DI_PVR_PAUSED)
		{
			BSTD_UNUSED(ulPosition); // for clean compile
			BSTD_UNUSED(pulTime); // for clean compile
		}
		else
		{
			PrintError("[DI_PVR_PLAY_ConvertPositionToTime] Invalid device status! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_PLAY_ConvertPositionToTime");

	return DI_ERR_OK;
}

DRV_Error DRV_PVR_PLAY_BackupCallback(HUINT32 ulDeviceId, HBOOL backup)
{
	unsigned long i;

	PrintEnter("DRV_PVR_PLAY_BackupCallback");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DRV_PVR_PLAY_BackupCallback] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		for(i=0; i<DI_PVR_EVENT_MAX_NUMBER; i++)
		{
			if(backup)
			{
				pbEvtCbBackup[i] = g_astPbInst[ulDeviceId].pbEvtCbFunc[i];
			}
			else
			{
				g_astPbInst[ulDeviceId].pbEvtCbFunc[i] = pbEvtCbBackup[i];
			}
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DRV_PVR_PLAY_BackupCallback");

	return DRV_OK;
}

DRV_Error DRV_PVR_PLAY_PidChannelOpen(HUINT32 ulDeviceId, DRV_DEMUX_CHANNELTYPE nChannelType, HUINT16 usPid, NEXUS_PidChannelHandle *pulPidChannelHandle)
{
#if (NEXUS_VERSION < 1250)
	NEXUS_Error	neResult;
#endif
	NEXUS_PlaybackPidChannelSettings pidChannelSettings;

	PrintEnter("DRV_PVR_PLAY_PidChannelOpen");

	if(pulPidChannelHandle == NULL)
	{
		PrintError("[DRV_PVR_PLAY_PidChannelOpen] pulPidChannelHandle == NULL\n\r");
		return DRV_ERR_INVALID_PARAMETER;
	}

	*pulPidChannelHandle = NULL;

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DRV_PVR_PLAY_PidChannelOpen] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		NEXUS_Playback_GetDefaultPidChannelSettings(&pidChannelSettings);
#if (NEXUS_VERSION>=1250)
		pidChannelSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE;
#endif

		switch(nChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO :
				if(!g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle)
				{
					pidChannelSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
		            pidChannelSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eUnknown;
		            pidChannelSettings.pidTypeSettings.video.decoder = NULL; /* Decode will set this later */
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelOpen] Playback video PID channel is already opened!\n\r");
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO :
			case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF : /* 동시에 2가지 type의 PID channel이 모두 open되지 않는다고 가정 */
				if(!g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle)
				{
					pidChannelSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
					if(g_astPbInst[ulDeviceId].setup.eStreamType == eDI_PVR_Stream_Audio && g_astPbInst[ulDeviceId].setup.eFileType == eDI_PVR_FileType_Circular)
					{
						pidChannelSettings.pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eMpeg;
					}
					else
					{
						pidChannelSettings.pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* audio.codec is required for VOB substream id remapping. So ignored. */
					}
					pidChannelSettings.pidTypeSettings.audio.primary = NULL;/* Decode will set this later */
		            pidChannelSettings.pidTypeSettings.audio.secondary = NULL; /* Decode will set this later */
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelOpen] Playback audio main PID channel is already opened!\n\r");
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION :
				if(!g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle)
				{
					pidChannelSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
					pidChannelSettings.pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* audio.codec is required for VOB substream id remapping. So ignored. */
					pidChannelSettings.pidTypeSettings.audio.primary = NULL;/* Decode will set this later */
		            pidChannelSettings.pidTypeSettings.audio.secondary = NULL; /* Decode will set this later */
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelOpen] Playback audio sub PID channel is already opened!\n\r");
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;

#if defined(CONFIG_FAST_AUDIO_CHANGE)
			case DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER :
				pidChannelSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
				pidChannelSettings.pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* audio.codec is required for VOB substream id remapping. So ignored. */
				pidChannelSettings.pidTypeSettings.audio.primary = NULL;/* Decode will set this later */
				pidChannelSettings.pidTypeSettings.audio.secondary = NULL; /* Decode will set this later */
				break;
#endif
			default :
#if (NEXUS_VERSION>=1250)
				pidChannelSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;
#endif
#if (NEXUS_VERSION < 1250)
				/* audio/video PID 와 동일한 경우 설정이 바뀌지 않도록 함 */
				if(g_astPbInst[ulDeviceId].pbPidInfo.videoPid == usPid && g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle)
				{
					neResult = NEXUS_Playback_GetPidChannelSettings(g_astPbInst[ulDeviceId].pbHandle, g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle, &pidChannelSettings);
			        if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DRV_PVR_PLAY_PidChannelOpen] Error(0x%x) in NEXUS_Playback_GetPidChannelSettings()\n\r", neResult);
						VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
						return DRV_ERR;
				    }
				}
				else if(g_astPbInst[ulDeviceId].pbPidInfo.audioMainPid == usPid && g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle)
				{
					neResult = NEXUS_Playback_GetPidChannelSettings(g_astPbInst[ulDeviceId].pbHandle, g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle, &pidChannelSettings);
			        if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DRV_PVR_PLAY_PidChannelOpen] Error(0x%x) in NEXUS_Playback_GetPidChannelSettings()\n\r", neResult);
						VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
						return DRV_ERR;
				    }
				}
				else if(g_astPbInst[ulDeviceId].pbPidInfo.audioSubPid == usPid && g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle)
				{
					neResult = NEXUS_Playback_GetPidChannelSettings(g_astPbInst[ulDeviceId].pbHandle, g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle, &pidChannelSettings);
			        if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DRV_PVR_PLAY_PidChannelOpen] Error(0x%x) in NEXUS_Playback_GetPidChannelSettings()\n\r", neResult);
						VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
						return DRV_ERR;
				    }
				}
				else
				{
					pidChannelSettings.pidSettings.pidType = NEXUS_PidType_eUnknown;
				}
#endif
				break;
		}
		*pulPidChannelHandle = NEXUS_Playback_OpenPidChannel(g_astPbInst[ulDeviceId].pbHandle, usPid, &pidChannelSettings);
		if(*pulPidChannelHandle == NULL)
		{
			PrintError("[DRV_PVR_PLAY_PidChannelOpen] Error in NEXUS_Playback_OpenPidChannel\n\r");
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DRV_ERR;
		}

		switch(nChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO :
				g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle = *pulPidChannelHandle;
				g_astPbInst[ulDeviceId].pbPidInfo.videoPid = usPid;
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO :
			case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF : /* 동시에 2가지 type의 PID channel이 모두 open되지 않는다고 가정 */
				g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle = *pulPidChannelHandle;
				g_astPbInst[ulDeviceId].pbPidInfo.audioMainPid = usPid;
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION :
				g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle = *pulPidChannelHandle;
				g_astPbInst[ulDeviceId].pbPidInfo.audioSubPid = usPid;
				break;
			default :
				/* No Action */
				break;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DRV_PVR_PLAY_PidChannelOpen");

	return DRV_OK;
}

DRV_Error DRV_PVR_PLAY_PidChannelClose(HUINT32 ulDeviceId, NEXUS_PidChannelHandle ulPidChannelHandle, DRV_DEMUX_CHANNELTYPE nChannelType)
{
	PrintEnter("DRV_PVR_PLAY_PidChannelClose");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DRV_PVR_PLAY_PidChannelClose] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		switch(nChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO :
				if(g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle && (g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle == ulPidChannelHandle))
				{
					g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle = NULL;
					g_astPbInst[ulDeviceId].pbPidInfo.videoPid = 0x2000;
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelClose] Playback video PID channel is already closed or mismatched with pulPidChannelHandle!(0x%x, 0x%x)\n\r",
						g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle, ulPidChannelHandle);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO :
			case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF : /* 동시에 2가지 type의 PID channel이 모두 open되지 않는다고 가정 */
				if(g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle && (g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle == ulPidChannelHandle))
				{
					g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle = NULL;
					g_astPbInst[ulDeviceId].pbPidInfo.audioMainPid = 0x2000;
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelClose] Playback audio main PID channel is already closed or mismatched with pulPidChannelHandle!(0x%x, 0x%x)\n\r",
						g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle, ulPidChannelHandle);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION :
				if(g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle && (g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle == ulPidChannelHandle))
				{
					g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle = NULL;
					g_astPbInst[ulDeviceId].pbPidInfo.audioSubPid = 0x2000;
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelClose] Playback audio sub PID channel is already closed or mismatched with pulPidChannelHandle!(0x%x, 0x%x)\n\r",
						g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle, ulPidChannelHandle);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			default :
				/* No Action */
				break;
		}
		NEXUS_Playback_ClosePidChannel(g_astPbInst[ulDeviceId].pbHandle, ulPidChannelHandle);
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DRV_PVR_PLAY_PidChannelClose");

	return DRV_OK;
}

/* For external playback */
DRV_Error DRV_PVR_PLAY_PidChannelOpenDummy(HUINT32 ulDeviceId, DRV_DEMUX_CHANNELTYPE nChannelType, HUINT16 usPid, NEXUS_PidChannelHandle ulPidChannelHandle)
{
	NEXUS_PlaybackPidChannelSettings pidChannelSettings;
	NEXUS_PidChannelHandle dummyPidChannelHandle = NULL;

	PrintEnter("DRV_PVR_PLAY_PidChannelOpenDummy");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DRV_PVR_PLAY_PidChannelOpenDummy] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		NEXUS_Playback_GetDefaultPidChannelSettings(&pidChannelSettings);
#if (NEXUS_VERSION>=1250)
		pidChannelSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE;
#endif

		switch(nChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO :
				if(!g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle)
				{
					pidChannelSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
		            pidChannelSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eUnknown;
		            pidChannelSettings.pidTypeSettings.video.decoder = NULL; /* Decode will set this later */
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelOpenDummy] Playback video PID channel is already opened!\n\r");
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO :
			case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF : /* 동시에 2가지 type의 PID channel이 모두 open되지 않는다고 가정 */
				if(!g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle)
				{
					pidChannelSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
					pidChannelSettings.pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* audio.codec is required for VOB substream id remapping. So ignored. */
					pidChannelSettings.pidTypeSettings.audio.primary = NULL;/* Decode will set this later */
		            pidChannelSettings.pidTypeSettings.audio.secondary = NULL; /* Decode will set this later */
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelOpenDummy] Playback audio main PID channel is already opened!\n\r");
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION :
				if(!g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle)
				{
					pidChannelSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
					pidChannelSettings.pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* audio.codec is required for VOB substream id remapping. So ignored. */
					pidChannelSettings.pidTypeSettings.audio.primary = NULL;/* Decode will set this later */
		            pidChannelSettings.pidTypeSettings.audio.secondary = NULL; /* Decode will set this later */
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelOpenDummy] Playback audio sub PID channel is already opened!\n\r");
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			default :
				PrintError("[DRV_PVR_PLAY_PidChannelOpenDummy] Invalid channel type! (nChannelType : %d)\n\r", nChannelType);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DRV_ERR;
		}
		dummyPidChannelHandle = NEXUS_Playback_OpenPidChannel(g_astPbInst[ulDeviceId].pbHandle, usPid, &pidChannelSettings);
		if(dummyPidChannelHandle == NULL)
		{
			PrintError("[DRV_PVR_PLAY_PidChannelOpenDummy] Error in NEXUS_Playback_OpenPidChannel\n\r");
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DRV_ERR;
		}

		switch(nChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO :
				g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle = ulPidChannelHandle;
				g_astPbInst[ulDeviceId].pbPidInfo.dummyVideoPidHandle = dummyPidChannelHandle;
				g_astPbInst[ulDeviceId].pbPidInfo.videoPid = usPid;
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO :
			case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF : /* 동시에 2가지 type의 PID channel이 모두 open되지 않는다고 가정 */
				g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle = ulPidChannelHandle;
				g_astPbInst[ulDeviceId].pbPidInfo.dummyAudioMainPidHandle = dummyPidChannelHandle;
				g_astPbInst[ulDeviceId].pbPidInfo.audioMainPid = usPid;
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION :
				g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle = ulPidChannelHandle;
				g_astPbInst[ulDeviceId].pbPidInfo.dummyAudioSubPidHandle = dummyPidChannelHandle;
				g_astPbInst[ulDeviceId].pbPidInfo.audioSubPid = usPid;
				break;
			default :
				/* No Action */
				break;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DRV_PVR_PLAY_PidChannelOpenDummy");

	return DRV_OK;
}

/* For external playback */
DRV_Error DRV_PVR_PLAY_PidChannelCloseDummy(HUINT32 ulDeviceId, NEXUS_PidChannelHandle ulPidChannelHandle, DRV_DEMUX_CHANNELTYPE nChannelType)
{
	PrintEnter("DRV_PVR_PLAY_PidChannelCloseDummy");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DRV_PVR_PLAY_PidChannelCloseDummy] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		switch(nChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO :
				if(g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle && (g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle == ulPidChannelHandle))
				{
					NEXUS_Playback_ClosePidChannel(g_astPbInst[ulDeviceId].pbHandle, g_astPbInst[ulDeviceId].pbPidInfo.dummyVideoPidHandle);
					g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle = NULL;
					g_astPbInst[ulDeviceId].pbPidInfo.dummyVideoPidHandle = NULL;
					g_astPbInst[ulDeviceId].pbPidInfo.videoPid = 0x2000;
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelCloseDummy] Playback video PID channel is already closed or mismatched with pulPidChannelHandle!(0x%x, 0x%x)\n\r",
						g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle, ulPidChannelHandle);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO :
			case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF : /* 동시에 2가지 type의 PID channel이 모두 open되지 않는다고 가정 */
				if(g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle && (g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle == ulPidChannelHandle))
				{
					NEXUS_Playback_ClosePidChannel(g_astPbInst[ulDeviceId].pbHandle, g_astPbInst[ulDeviceId].pbPidInfo.dummyAudioMainPidHandle);
					g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle = NULL;
					g_astPbInst[ulDeviceId].pbPidInfo.dummyAudioMainPidHandle = NULL;
					g_astPbInst[ulDeviceId].pbPidInfo.audioMainPid = 0x2000;
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelCloseDummy] Playback audio main PID channel is already closed or mismatched with pulPidChannelHandle!(0x%x, 0x%x)\n\r",
						g_astPbInst[ulDeviceId].pbPidInfo.audioMainPidHandle, ulPidChannelHandle);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION :
				if(g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle && (g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle == ulPidChannelHandle))
				{
					NEXUS_Playback_ClosePidChannel(g_astPbInst[ulDeviceId].pbHandle, g_astPbInst[ulDeviceId].pbPidInfo.dummyAudioSubPidHandle);
					g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle = NULL;
					g_astPbInst[ulDeviceId].pbPidInfo.dummyAudioSubPidHandle = NULL;
					g_astPbInst[ulDeviceId].pbPidInfo.audioSubPid = 0x2000;
				}
				else
				{
					PrintError("[DRV_PVR_PLAY_PidChannelCloseDummy] Playback audio sub PID channel is already closed or mismatched with pulPidChannelHandle!(0x%x, 0x%x)\n\r",
						g_astPbInst[ulDeviceId].pbPidInfo.audioSubPidHandle, ulPidChannelHandle);
					VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
					return DRV_ERR;
				}
				break;
			default :
				PrintError("[DRV_PVR_PLAY_PidChannelCloseDummy] Invalid channel type! (nChannelType : %d)\n\r", nChannelType);
				VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
				return DRV_ERR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DRV_PVR_PLAY_PidChannelCloseDummy");

	return DRV_OK;
}

DRV_Error DRV_PVR_PLAY_VideoPidChannelSetting(NEXUS_PidChannelHandle ulPidChannelHandle, NEXUS_VideoDecoderHandle ulVideoDecHandle, NEXUS_VideoCodec nVideoCodec)
{
	NEXUS_Error	neResult;
	int i;
	unsigned long deviceId;
	NEXUS_PlaybackPidChannelSettings pidChannelSettings;

	PrintEnter("DRV_PVR_PLAY_VideoPidChannelSetting");

	for(i=0;i<MAX_PLAY_INST_NUM;i++)
	{
		if(g_astPbInst[i].pbPidInfo.videoPidHandle == ulPidChannelHandle)
		{
			deviceId = i;
			break;
		}
	}
	if(i == MAX_PLAY_INST_NUM)
	{
		PrintError("[DRV_PVR_PLAY_VideoPidChannelSetting] Invalid PID channel handle! (ulPidChannelHandle : 0x%x)\n\r", ulPidChannelHandle);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[deviceId].diSemId) == VK_OK)
	{
		if(g_astPbInst[deviceId].pbPidInfo.videoPidHandle != ulPidChannelHandle) // get semaphore 후 한 번 더 확인
		{
			PrintError("[DRV_PVR_PLAY_VideoPidChannelSetting] Maybe..video PID channel handle was changed! (videoPidHandle : 0x%x, ulPidChannelHandle : 0x%x)\n\r",
					g_astPbInst[deviceId].pbPidInfo.videoPidHandle, ulPidChannelHandle);
			VK_SEM_Release(g_astPbInst[deviceId].diSemId);
			return DRV_ERR;
		}

        neResult = NEXUS_Playback_GetPidChannelSettings(g_astPbInst[deviceId].pbHandle,
													g_astPbInst[deviceId].isExternalPlay ? g_astPbInst[deviceId].pbPidInfo.dummyVideoPidHandle : ulPidChannelHandle,
													&pidChannelSettings);
        if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[DRV_PVR_PLAY_VideoPidChannelSetting] Error(0x%x) in NEXUS_Playback_GetPidChannelSettings()\n\r", neResult);
			VK_SEM_Release(g_astPbInst[deviceId].diSemId);
			return DRV_ERR;
	    }
        pidChannelSettings.pidTypeSettings.video.decoder = ulVideoDecHandle;
		pidChannelSettings.pidTypeSettings.video.codec = nVideoCodec;
		pidChannelSettings.pidTypeSettings.video.index = true;
        neResult = NEXUS_Playback_SetPidChannelSettings(g_astPbInst[deviceId].pbHandle,
													g_astPbInst[deviceId].isExternalPlay ? g_astPbInst[deviceId].pbPidInfo.dummyVideoPidHandle : ulPidChannelHandle,
													&pidChannelSettings);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[DRV_PVR_PLAY_VideoPidChannelSetting] Error(0x%x) in NEXUS_Playback_SetPidChannelSettings()\n\r", neResult);
			VK_SEM_Release(g_astPbInst[deviceId].diSemId);
			return DRV_ERR;
	    }

		/* external play인 경우 A/V decoder가 start할 때 playback mode로 인식하지 못하기 때문에 PB channel resume을 못함.
		  따라서 강제로 PB channel resume을 해줘야 함. */
		if(g_astPbInst[deviceId].isExternalPlay)
		{
			NEXUS_PidChannel_ConsumerStarted(g_astPbInst[deviceId].pbPidInfo.dummyVideoPidHandle);
		}
		VK_SEM_Release(g_astPbInst[deviceId].diSemId);
	}

	PrintExit("DRV_PVR_PLAY_VideoPidChannelSetting");

	return DRV_OK;
}

DRV_Error DRV_PVR_PLAY_AudioPidChannelSetting(NEXUS_PidChannelHandle ulPidChannelHandle, HBOOL isSubAudioPid,
														NEXUS_AudioDecoderHandle ulAudioDecHandle,
														NEXUS_AudioDecoderHandle ulAudioPassThroughDecHandle)
{
	NEXUS_Error	neResult;
	int i;
	unsigned long deviceId;
	NEXUS_PidChannelHandle pidChannelHandle;
	NEXUS_PlaybackPidChannelSettings pidChannelSettings;

	PrintEnter("DRV_PVR_PLAY_AudioPidChannelSetting");

	for(i=0;i<MAX_PLAY_INST_NUM;i++)
	{
		pidChannelHandle = isSubAudioPid == false ? g_astPbInst[i].pbPidInfo.audioMainPidHandle : g_astPbInst[i].pbPidInfo.audioSubPidHandle;
		if(pidChannelHandle == ulPidChannelHandle)
		{
			deviceId = i;
			break;
		}
	}
	if(i == MAX_PLAY_INST_NUM)
	{
		PrintError("[DRV_PVR_PLAY_AudioPidChannelSetting] Invalid PID channel handle! (ulPidChannelHandle : 0x%x)\n\r", ulPidChannelHandle);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[deviceId].diSemId) == VK_OK)
	{
		if(!isSubAudioPid && (g_astPbInst[deviceId].pbPidInfo.audioMainPidHandle != ulPidChannelHandle)) // get semaphore 후 한 번 더 확인
		{
			PrintError("[DRV_PVR_PLAY_AudioPidChannelSetting] Maybe..audio main PID channel handle was changed! (audioMainPidHandle : 0x%x, ulPidChannelHandle : 0x%x)\n\r",
					g_astPbInst[deviceId].pbPidInfo.audioMainPidHandle, ulPidChannelHandle);
			VK_SEM_Release(g_astPbInst[deviceId].diSemId);
			return DRV_ERR;
		}
		else if(isSubAudioPid && (g_astPbInst[deviceId].pbPidInfo.audioSubPidHandle != ulPidChannelHandle))
		{
			PrintError("[DRV_PVR_PLAY_AudioPidChannelSetting] Maybe..audio sub PID channel handle was changed! (audioSubPidHandle : 0x%x, ulPidChannelHandle : 0x%x)\n\r",
					g_astPbInst[deviceId].pbPidInfo.audioSubPidHandle, ulPidChannelHandle);
			VK_SEM_Release(g_astPbInst[deviceId].diSemId);
			return DRV_ERR;
		}

		if(g_astPbInst[deviceId].isExternalPlay)
		{
			pidChannelHandle = isSubAudioPid ? g_astPbInst[deviceId].pbPidInfo.dummyAudioSubPidHandle : g_astPbInst[deviceId].pbPidInfo.dummyAudioMainPidHandle;
		}
		else
		{
			pidChannelHandle = ulPidChannelHandle;
		}
		neResult = NEXUS_Playback_GetPidChannelSettings(g_astPbInst[deviceId].pbHandle, pidChannelHandle, &pidChannelSettings);
        if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[DRV_PVR_PLAY_AudioPidChannelSetting] Error(0x%x) in NEXUS_Playback_GetPidChannelSettings()\n\r", neResult);
			VK_SEM_Release(g_astPbInst[deviceId].diSemId);
			return DRV_ERR;
	    }

		if(isSubAudioPid)
		{
            pidChannelSettings.pidTypeSettings.audio.primary = ulAudioDecHandle;
			pidChannelSettings.pidTypeSettings.audio.secondary = NULL;
		}
		else
		{
			pidChannelSettings.pidTypeSettings.audio.primary = ulAudioDecHandle;
			pidChannelSettings.pidTypeSettings.audio.secondary = ulAudioPassThroughDecHandle;
		}
		neResult = NEXUS_Playback_SetPidChannelSettings(g_astPbInst[deviceId].pbHandle, pidChannelHandle, &pidChannelSettings);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[DRV_PVR_PLAY_AudioPidChannelSetting] Error(0x%x) in NEXUS_Playback_SetPidChannelSettings()\n\r", neResult);
			VK_SEM_Release(g_astPbInst[deviceId].diSemId);
			return DRV_ERR;
	    }

		/* external play인 경우 A/V decoder가 start할 때 playback mode로 인식하지 못하기 때문에 PB channel resume을 못함.
		  따라서 강제로 PB channel resume을 해줘야 함. */
		if(g_astPbInst[deviceId].isExternalPlay)
		{
			NEXUS_PidChannel_ConsumerStarted(pidChannelHandle);
		}
		VK_SEM_Release(g_astPbInst[deviceId].diSemId);
	}

	PrintExit("DRV_PVR_PLAY_AudioPidChannelSetting");

	return DRV_OK;
}

DRV_Error DRV_PVR_PLAY_SetStcChannelHandle(HUINT32 ulDeviceId, NEXUS_StcChannelHandle ulStcChannelHandle)
{
	NEXUS_Error	neResult;
	NEXUS_PlaybackSettings	PlaySettings;

	PrintEnter("DRV_PVR_PLAY_SetStcChannelHandle");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DRV_PVR_PLAY_SetStcChannelHandle] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		NEXUS_Playback_GetSettings(g_astPbInst[ulDeviceId].pbHandle, &PlaySettings);
		PlaySettings.stcChannel = ulStcChannelHandle;
		PlaySettings.stcTrick = false; /* STC trick은 false로.. -> STC trick을 on하면 video에서 현재 trick중인지 판단하는 기준이 바뀌어야 한다. */
		neResult = NEXUS_Playback_SetSettings(g_astPbInst[ulDeviceId].pbHandle, &PlaySettings);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[DRV_PVR_PLAY_SetStcChannelHandle] Error(0x%x) in NEXUS_Playback_SetSettings()\n\r", neResult);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
	    }
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DRV_PVR_PLAY_SetStcChannelHandle");

	return DRV_OK;
}

DRV_Error DRV_PVR_PLAY_UnsetStcChannelHandle(HUINT32 ulDeviceId)
{
	NEXUS_Error	neResult;
	NEXUS_PlaybackSettings	PlaySettings;

	PrintEnter("DRV_PVR_PLAY_UnsetStcChannelHandle");

	if(ulDeviceId >= MAX_PLAY_INST_NUM)
	{
		PrintError("[DRV_PVR_PLAY_UnsetStcChannelHandle] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		NEXUS_Playback_GetSettings(g_astPbInst[ulDeviceId].pbHandle, &PlaySettings);
		PlaySettings.stcChannel = NULL;
		neResult = NEXUS_Playback_SetSettings(g_astPbInst[ulDeviceId].pbHandle, &PlaySettings);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[DRV_PVR_PLAY_UnsetStcChannelHandle] Error(0x%x) in NEXUS_Playback_SetSettings()\n\r", neResult);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
	    }
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DRV_PVR_PLAY_UnsetStcChannelHandle");

	return DRV_OK;
}

DRV_Error DRV_PVR_PLAY_SetRecordPath(HUINT32 ulDeviceId, HUINT32 ulRecDeviceId)
{
	PrintEnter("DRV_PVR_PLAY_SetRecordPath");

	if(ulDeviceId >= MAX_PLAY_INST_NUM || ulRecDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DRV_PVR_PLAY_SetRecordPath] Invalid parameter! (ulDeviceId : %d, ulRecDeviceId : %d)\n\r", ulDeviceId, ulRecDeviceId);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPbInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPbInst[ulDeviceId].recDeviceInfo.deviceId == -1)
		{
			g_astPbInst[ulDeviceId].recDeviceInfo.deviceId = (HINT32)ulRecDeviceId;
		}
		else
		{
			PrintError("[DRV_PVR_PLAY_SetRecordPath] Invalid device status or already existing record path! (status : %d)\n\r", g_astPbInst[ulDeviceId].status);
			VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
			return DRV_ERR;
		}
		VK_SEM_Release(g_astPbInst[ulDeviceId].diSemId);
	}

	PrintExit("DRV_PVR_PLAY_SetRecordPath");

	return DRV_OK;
}

#if 0 /* Host trick mode를 사용하지 않는 경우에 대한 NEXUS 코드에서의 처리가 미흡함 */
static const P_PLAY_TRICK_TABLE SCD_TrickTable[2*eDI_PVR_Speed_Fast_Forward7 + 1] =
{
	{NEXUS_PlaybackHostTrickMode_ePlayI, -25, 1}, 	//eDI_PVR_Speed_Fast_Backward7, -128x
	{NEXUS_PlaybackHostTrickMode_ePlayI, -10, 1},	//eDI_PVR_Speed_Fast_Backward6, -64x
	{NEXUS_PlaybackHostTrickMode_ePlayI, -5, 1},  	//eDI_PVR_Speed_Fast_Backward5, -32x
	{NEXUS_PlaybackHostTrickMode_ePlayI, -2, 1},  	//eDI_PVR_Speed_Fast_Backward4, -16x
	{NEXUS_PlaybackHostTrickMode_ePlayI, -1, 1},  	//eDI_PVR_Speed_Fast_Backward3, -8x
	{NEXUS_PlaybackHostTrickMode_ePlayBrcm, -5, 1},  	//eDI_PVR_Speed_Fast_Backward2, -4x
	{NEXUS_PlaybackHostTrickMode_ePlayBrcm, -2, 1},  	//eDI_PVR_Speed_Fast_Backward1, -2x
	{NEXUS_PlaybackHostTrickMode_ePlayBrcm, -1, 1},  	//eDI_PVR_Speed_Normal_Backward, -1x
	{NEXUS_PlaybackHostTrickMode_ePlayBrcm, -1, 10},  	//eDI_PVR_Speed_Slow_Backward4, -1/10
	{NEXUS_PlaybackHostTrickMode_ePlayBrcm, -1, 8},  	//eDI_PVR_Speed_Slow_Backward3, -1/8
	{NEXUS_PlaybackHostTrickMode_ePlayBrcm, -1, 4},  	//eDI_PVR_Speed_Slow_Backward2, -1/4
	{NEXUS_PlaybackHostTrickMode_ePlayBrcm, -1, 2},  	//eDI_PVR_Speed_Slow_Backward1, -1/2
	{NEXUS_PlaybackHostTrickMode_eNormal, 0, 1},  	//eDI_PVR_Speed_Pause
	{NEXUS_PlaybackHostTrickMode_eNormal, 1, 2},  //eDI_PVR_Speed_Slow_Forward1, 1/2
	{NEXUS_PlaybackHostTrickMode_eNormal, 1, 4},  	//eDI_PVR_Speed_Slow_Forward2, 1/4
	{NEXUS_PlaybackHostTrickMode_eNormal, 1, 8},		//eDI_PVR_Speed_Slow_Forward3, 1/8
	{NEXUS_PlaybackHostTrickMode_eNormal, 1, 10}, 	//eDI_PVR_Speed_Slow_Forward4, 1/10
	{NEXUS_PlaybackHostTrickMode_eNormal, 1, 1}, 	//eDI_PVR_Speed_Normal, 1x
	{NEXUS_PlaybackHostTrickMode_ePlayIP, 0, 1}, 	//eDI_PVR_Speed_Fast_Forward1, 2x
	{NEXUS_PlaybackHostTrickMode_ePlaySkipP, 1, 1}, 	//eDI_PVR_Speed_Fast_Forward2, 4x
	{NEXUS_PlaybackHostTrickMode_ePlayI, 1, 1}, 	//eDI_PVR_Speed_Fast_Forward3, 8x
	{NEXUS_PlaybackHostTrickMode_ePlayI, 2, 1}, 	//eDI_PVR_Speed_Fast_Forward4, 16x
	{NEXUS_PlaybackHostTrickMode_ePlayI, 5, 1}, 	//eDI_PVR_Speed_Fast_Forward5, 32x
	{NEXUS_PlaybackHostTrickMode_ePlayI, 10, 1},		//eDI_PVR_Speed_Fast_Forward6, 64x
	{NEXUS_PlaybackHostTrickMode_ePlayI, 25, 1} 		//eDI_PVR_Speed_Fast_Forward7, reserved
};

static const P_PLAY_TRICK_TABLE SCD_AVCTrickTable[2*eDI_PVR_Speed_Fast_Forward7 + 1] =
{
	{NEXUS_PlaybackHostTrickMode_eNone, -12800, 1}, 	//eDI_PVR_Speed_Fast_Backward7, -128x
	{NEXUS_PlaybackHostTrickMode_eNone, -6400, 1},	//eDI_PVR_Speed_Fast_Backward6, -64x
	{NEXUS_PlaybackHostTrickMode_eNone, -3200, 1},  	//eDI_PVR_Speed_Fast_Backward5, -32x
	{NEXUS_PlaybackHostTrickMode_eNone, -1600, 1},  	//eDI_PVR_Speed_Fast_Backward4, -16x
	{NEXUS_PlaybackHostTrickMode_eNone, -800, 1},  	//eDI_PVR_Speed_Fast_Backward3, -8x
	{NEXUS_PlaybackHostTrickMode_eNone, -400, 1},  	//eDI_PVR_Speed_Fast_Backward2, -4x
	{NEXUS_PlaybackHostTrickMode_eNone, -200, 1},  	//eDI_PVR_Speed_Fast_Backward1, -2x
	{NEXUS_PlaybackHostTrickMode_eNone, -100, 1},  	//eDI_PVR_Speed_Normal_Backward, -1x
	{NEXUS_PlaybackHostTrickMode_eNone, -50, 1},  	//eDI_PVR_Speed_Slow_Backward4, -1/2
	{NEXUS_PlaybackHostTrickMode_eNone, -25, 1},  	//eDI_PVR_Speed_Slow_Backward3, -1/4
	{NEXUS_PlaybackHostTrickMode_eNone, -12, 1},  	//eDI_PVR_Speed_Slow_Backward2, -1/8
	{NEXUS_PlaybackHostTrickMode_eNone, -10, 1},  	//eDI_PVR_Speed_Slow_Backward1, -1/10
	{NEXUS_PlaybackHostTrickMode_eNormal, 0, 1},  	//eDI_PVR_Speed_Pause
	{NEXUS_PlaybackHostTrickMode_eNormal, 1, 10},  //eDI_PVR_Speed_Slow_Forward1, 1/10
	{NEXUS_PlaybackHostTrickMode_eNormal, 1, 8},  	//eDI_PVR_Speed_Slow_Forward2, 1/8
	{NEXUS_PlaybackHostTrickMode_eNormal, 1, 4},		//eDI_PVR_Speed_Slow_Forward3, 1/4
	{NEXUS_PlaybackHostTrickMode_eNormal, 1, 2}, 	//eDI_PVR_Speed_Slow_Forward4, 1/2
	{NEXUS_PlaybackHostTrickMode_eNormal, 1, 1}, 	//eDI_PVR_Speed_Normal, 1x
	{NEXUS_PlaybackHostTrickMode_ePlayIP, 0, 1}, 	//eDI_PVR_Speed_Fast_Forward1, 2x
	{NEXUS_PlaybackHostTrickMode_ePlaySkipP, 1, 1}, 	//eDI_PVR_Speed_Fast_Forward2, 4x
	{NEXUS_PlaybackHostTrickMode_ePlayI, 1, 1}, 	//eDI_PVR_Speed_Fast_Forward3, 8x
	{NEXUS_PlaybackHostTrickMode_ePlayI, 2, 1}, 	//eDI_PVR_Speed_Fast_Forward4, 16x
	{NEXUS_PlaybackHostTrickMode_ePlayI, 5, 1}, 	//eDI_PVR_Speed_Fast_Forward5, 32x
	{NEXUS_PlaybackHostTrickMode_ePlayI, 10, 1},		//eDI_PVR_Speed_Fast_Forward6, 64x
	{NEXUS_PlaybackHostTrickMode_ePlayI, 25, 1} 		//eDI_PVR_Speed_Fast_Forward7, reserved
};
#else
static const P_PLAY_TRICK_TABLE SCD_TrickTable[2*eDI_PVR_Speed_Fast_Forward7 + 1] =
{
	{NEXUS_PlaybackHostTrickMode_eNone, -12800, 1}, 	//eDI_PVR_Speed_Fast_Backward7, -128x
	{NEXUS_PlaybackHostTrickMode_eNone, -6400, 1},	//eDI_PVR_Speed_Fast_Backward6, -64x
	{NEXUS_PlaybackHostTrickMode_eNone, -3200, 1},  	//eDI_PVR_Speed_Fast_Backward5, -32x
	{NEXUS_PlaybackHostTrickMode_eNone, -1600, 1},  	//eDI_PVR_Speed_Fast_Backward4, -16x
	{NEXUS_PlaybackHostTrickMode_eNone, -800, 1},  	//eDI_PVR_Speed_Fast_Backward3, -8x
	{NEXUS_PlaybackHostTrickMode_eNone, -400, 1},  	//eDI_PVR_Speed_Fast_Backward2, -4x
	{NEXUS_PlaybackHostTrickMode_eNone, -200, 1},  	//eDI_PVR_Speed_Fast_Backward1, -2x
	{NEXUS_PlaybackHostTrickMode_eNone, -100, 1},  	//eDI_PVR_Speed_Normal_Backward, -1x
	{NEXUS_PlaybackHostTrickMode_eNone, -10, 1},  	//eDI_PVR_Speed_Slow_Backward4, -1/10
	{NEXUS_PlaybackHostTrickMode_eNone, -12, 1},  	//eDI_PVR_Speed_Slow_Backward3, -1/8
	{NEXUS_PlaybackHostTrickMode_eNone, -25, 1},  	//eDI_PVR_Speed_Slow_Backward2, -1/4
	{NEXUS_PlaybackHostTrickMode_eNone, -50, 1},  	//eDI_PVR_Speed_Slow_Backward1, -1/2
	{NEXUS_PlaybackHostTrickMode_eNone, 0, 1},  	//eDI_PVR_Speed_Pause
	{NEXUS_PlaybackHostTrickMode_eNone, 50, 1},  //eDI_PVR_Speed_Slow_Forward1, 1/2
	{NEXUS_PlaybackHostTrickMode_eNone, 25, 1},  	//eDI_PVR_Speed_Slow_Forward2, 1/4
	{NEXUS_PlaybackHostTrickMode_eNone, 12, 1},		//eDI_PVR_Speed_Slow_Forward3, 1/8
	{NEXUS_PlaybackHostTrickMode_eNone, 10, 1}, 	//eDI_PVR_Speed_Slow_Forward4, 1/10
	{NEXUS_PlaybackHostTrickMode_eNone, 100, 1}, 	//eDI_PVR_Speed_Normal, 1x
	{NEXUS_PlaybackHostTrickMode_eNone, 300, 1}, 	//eDI_PVR_Speed_Fast_Forward1, 2x
	{NEXUS_PlaybackHostTrickMode_eNone, 500, 1}, 	//eDI_PVR_Speed_Fast_Forward2, 4x
	{NEXUS_PlaybackHostTrickMode_eNone, 800, 1}, 	//eDI_PVR_Speed_Fast_Forward3, 8x
	{NEXUS_PlaybackHostTrickMode_eNone, 1600, 1}, 	//eDI_PVR_Speed_Fast_Forward4, 16x
	{NEXUS_PlaybackHostTrickMode_eNone, 3200, 1}, 	//eDI_PVR_Speed_Fast_Forward5, 32x
	{NEXUS_PlaybackHostTrickMode_eNone, 6400, 1},		//eDI_PVR_Speed_Fast_Forward6, 64x
	{NEXUS_PlaybackHostTrickMode_eNone, 12800, 1} 		//eDI_PVR_Speed_Fast_Forward7, reserved
};

static const P_PLAY_TRICK_TABLE SCD_AVCTrickTable[2*eDI_PVR_Speed_Fast_Forward7 + 1] =
{
	{NEXUS_PlaybackHostTrickMode_eNone, -12800, 1}, 	//eDI_PVR_Speed_Fast_Backward7, -128x
	{NEXUS_PlaybackHostTrickMode_eNone, -6400, 1},	//eDI_PVR_Speed_Fast_Backward6, -64x
	{NEXUS_PlaybackHostTrickMode_eNone, -3200, 1},  	//eDI_PVR_Speed_Fast_Backward5, -32x
	{NEXUS_PlaybackHostTrickMode_eNone, -1600, 1},  	//eDI_PVR_Speed_Fast_Backward4, -16x
	{NEXUS_PlaybackHostTrickMode_eNone, -800, 1},  	//eDI_PVR_Speed_Fast_Backward3, -8x
	{NEXUS_PlaybackHostTrickMode_eNone, -400, 1},  	//eDI_PVR_Speed_Fast_Backward2, -4x
	{NEXUS_PlaybackHostTrickMode_eNone, -200, 1},  	//eDI_PVR_Speed_Fast_Backward1, -2x
	{NEXUS_PlaybackHostTrickMode_eNone, -100, 1},  	//eDI_PVR_Speed_Normal_Backward, -1x
	{NEXUS_PlaybackHostTrickMode_eNone, -10, 1},  	//eDI_PVR_Speed_Slow_Backward4, -1/10
	{NEXUS_PlaybackHostTrickMode_eNone, -12, 1},  	//eDI_PVR_Speed_Slow_Backward3, -1/8
	{NEXUS_PlaybackHostTrickMode_eNone, -25, 1},  	//eDI_PVR_Speed_Slow_Backward2, -1/4
	{NEXUS_PlaybackHostTrickMode_eNone, -50, 1},  	//eDI_PVR_Speed_Slow_Backward1, -1/2
	{NEXUS_PlaybackHostTrickMode_eNone, 0, 1},  	//eDI_PVR_Speed_Pause
	{NEXUS_PlaybackHostTrickMode_eNone, 50, 1},  //eDI_PVR_Speed_Slow_Forward1, 1/2
	{NEXUS_PlaybackHostTrickMode_eNone, 25, 1},  	//eDI_PVR_Speed_Slow_Forward2, 1/4
	{NEXUS_PlaybackHostTrickMode_eNone, 12, 1},		//eDI_PVR_Speed_Slow_Forward3, 1/8
	{NEXUS_PlaybackHostTrickMode_eNone, 10, 1}, 	//eDI_PVR_Speed_Slow_Forward4, 1/10
	{NEXUS_PlaybackHostTrickMode_eNone, 100, 1}, 	//eDI_PVR_Speed_Normal, 1x
	{NEXUS_PlaybackHostTrickMode_eNone, 301, 1}, 	//eDI_PVR_Speed_Fast_Forward1, 2x
	{NEXUS_PlaybackHostTrickMode_eNone, 500, 1}, 	//eDI_PVR_Speed_Fast_Forward2, 4x
	{NEXUS_PlaybackHostTrickMode_eNone, 800, 1}, 	//eDI_PVR_Speed_Fast_Forward3, 8x
	{NEXUS_PlaybackHostTrickMode_eNone, 1600, 1}, 	//eDI_PVR_Speed_Fast_Forward4, 16x
	{NEXUS_PlaybackHostTrickMode_eNone, 3200, 1}, 	//eDI_PVR_Speed_Fast_Forward5, 32x
	{NEXUS_PlaybackHostTrickMode_eNone, 6400, 1},		//eDI_PVR_Speed_Fast_Forward6, 64x
	{NEXUS_PlaybackHostTrickMode_eNone, 12800, 1} 		//eDI_PVR_Speed_Fast_Forward7, reserved
};
#endif

static DRV_Error P_PLAY_ChangeTrickSpeedToTrickInfo(HUINT32 ulDeviceId, DI_PVR_TRICKSPEED_t eTrickSpeed, NEXUS_PlaybackTrickModeSettings *pstTrickModeSettings)
{
	NEXUS_Error	neResult;
	int	trickTblIdx;
	int rate;
	NEXUS_PlaybackPidChannelSettings videoPidChSettings;
	const P_PLAY_TRICK_TABLE *trickTable;

	PrintEnter("P_PLAY_ChangeTrickSpeedToTrickInfo\r");

	if((eTrickSpeed > eDI_PVR_Speed_Fast_Forward7) || (eTrickSpeed < eDI_PVR_Speed_Fast_Backward7)
		|| (eTrickSpeed == eDI_PVR_Speed_Pause) || (eTrickSpeed == eDI_PVR_Speed_Normal)) /* pause, normal speed는 NEXUS_Playback_TrickMode()를 사용하지 않는다 */
	{
		PrintError("[P_PLAY_ChangeTrickSpeedToTrickInfo] Invalid parameter! (eTrickSpeed : %d)\n\r", eTrickSpeed);
		return DRV_ERR_INVALID_PARAMETER;
	}

	NEXUS_Playback_GetDefaultTrickModeSettings(pstTrickModeSettings);

	/* DI_PVR_TRICKSPEED_t enum 최소값 define 바뀔 때 주의 */
	trickTblIdx = eTrickSpeed - eDI_PVR_Speed_Fast_Backward7;

	neResult = NEXUS_Playback_GetPidChannelSettings(g_astPbInst[ulDeviceId].pbHandle,
				g_astPbInst[ulDeviceId].isExternalPlay ? g_astPbInst[ulDeviceId].pbPidInfo.dummyVideoPidHandle : g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle,
				&videoPidChSettings);
    if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_PLAY_ChangeTrickSpeedToTrickInfo] Error(0x%x) in NEXUS_Playback_GetPidChannelSettings()\n\r", neResult);
		return DRV_ERR;
    }

	switch(videoPidChSettings.pidTypeSettings.video.codec)
	{
		case NEXUS_VideoCodec_eMpeg2 :
			trickTable = SCD_TrickTable;
			break;
		case NEXUS_VideoCodec_eH264 :
			trickTable = SCD_AVCTrickTable;
			break;
		default :
			PrintError("[P_PLAY_ChangeTrickSpeedToTrickInfo] Not supported video codec! (codec : %d)\n\r", videoPidChSettings.pidTypeSettings.video.codec);
			return DRV_ERR;
	}

	if(trickTable[trickTblIdx].mode == NEXUS_PlaybackHostTrickMode_eNone) /* trick by rate */
	{
		rate = trickTable[trickTblIdx].modeModifier * NEXUS_NORMAL_PLAY_SPEED / 100;
	}
	else
	{
		rate = NEXUS_NORMAL_PLAY_SPEED;
	}
	if (trickTable[trickTblIdx].slowMotionRate > 1)
	{
        rate /= (int)trickTable[trickTblIdx].slowMotionRate;
    }
	pstTrickModeSettings->rate = rate;
	pstTrickModeSettings->mode = trickTable[trickTblIdx].mode;
#if 1 /*temp: to comply with nexus ph5.0*/
	pstTrickModeSettings->mode_modifier = 1;
#else
	pstTrickModeSettings->mode_modifier = trickTable[trickTblIdx].modeModifier;
#endif

	PrintExit("P_PLAY_ChangeTrickSpeedToTrickInfo");

	return DRV_OK;
}

static DRV_Error P_PLAY_SetDecryption(NEXUS_PlaybackHandle ulPbHandle, NEXUS_KeySlotHandle ulDecKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstDecryptSettings)
{
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
	NEXUS_Error	neResult;
	NEXUS_SecurityAlgorithmSettings	algConfig;
	NEXUS_SecurityClearKey			key;
	NEXUS_SecurityEncryptedSessionKey encSessionKey;
	NEXUS_SecurityEncryptedControlWord encCw;
	NEXUS_PlaybackSettings			PlaySettings;
	HUINT8 	decKey[ENCRYPTION_KEY_LEN];
#if (BCHP_CHIP == 7584) && defined(CONFIG_CAS_CX)
	const HUINT8						klrcKey[ENCRYPTION_KEY_LEN] = {0x70, 0x72, 0x69, 0x65, 0x74, 0x61, 0x72, 0x79, 0x43, 0x6f, 0x6e, 0x61, 0x78, 0x50, 0x72, 0x6f};
#endif

	PrintEnter("P_PLAY_SetDecryption\r");

	NEXUS_Security_GetDefaultClearKey(&key);
	if(pstDecryptSettings->eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
	{
		VK_MEM_Memcpy(decKey, pstDecryptSettings->ucKey, ENCRYPTION_KEY_LEN);
		if(pstDecryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_3DES)
		{
			DRV_CRYPT_ExpandKeyLength(decKey,decKey);
		}
		NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);

#if defined(CONFIG_CAS_ASKM)
		algConfig.ivMode			= NEXUS_SecurityIVMode_eRegular;
		algConfig.solitarySelect		= NEXUS_SecuritySolitarySelect_eClear;
		algConfig.caVendorID		= 0xEAF6;
		algConfig.askmModuleID		= NEXUS_SecurityAskmModuleID_eModuleID_8;
		algConfig.otpId 			= NEXUS_SecurityOtpId_eOtpVal;
		algConfig.key2Select		= NEXUS_SecurityKey2Select_eRealKey;
#endif

		switch(pstDecryptSettings->eEncType)
		{
			case DI_PVR_ENCRYPTION_TYPE_3DES :
			case DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY :
				algConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
	   			algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				break;
			case DI_PVR_ENCRYPTION_TYPE_AES :
			case DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY :
				algConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
				algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				break;
			default :
				PrintError("[P_PLAY_SetDecryption] Not supported encryption type! (eEncType : %d)\n\r", pstDecryptSettings->eEncType);
				return DRV_ERR_INVALID_PARAMETER;
		}
	    algConfig.operation = NEXUS_SecurityOperation_eDecrypt;
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
		algConfig.enableTimestamps = true; /* enable timestamps */
#endif
	    neResult = NEXUS_Security_ConfigAlgorithm(ulDecKeyHandle, &algConfig);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[P_PLAY_SetDecryption] Error(0x%x) in NEXUS_Security_ConfigAlgorithm()\n\r", neResult);
			return DRV_ERR;
	    }

	    /* Load AV keys */
		switch(pstDecryptSettings->eEncType)
		{
			case DI_PVR_ENCRYPTION_TYPE_3DES :
			case DI_PVR_ENCRYPTION_TYPE_AES :
				key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
			    key.keySize = sizeof(decKey);
			    VK_memcpy(key.keyData, decKey, sizeof(decKey));
			    neResult = NEXUS_Security_LoadClearKey(ulDecKeyHandle, &key);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[P_PLAY_SetDecryption] Error(0x%x) in NEXUS_Security_LoadClearKey()\n\r", neResult);
					return DRV_ERR;
			    }
#if 0 /* The DI_PVR_ENCRYPTION_TYPE_AES of DI10 uses NEXUS_SecurityAlgorithmVariant_eEcb, so the below code does not need */
				if(pstDecryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_AES)
				{
					/* Load IV for AES_CBC*/
					NEXUS_Security_GetDefaultClearKey(&key);
#if 0	/*Use this conventional-way to be compatible with DI_CRYPT*/
					key.keyEntryType = NEXUS_SecurityKeyType_eIv;
#else   /*To apply this, DI_CRYPT shall also be modified.*/
					key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
					key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
#endif
					key.keySize = sizeof(pstDecryptSettings->ucIv);
				    VK_memcpy(key.keyData,pstDecryptSettings->ucIv,key.keySize);
					neResult = NEXUS_Security_LoadClearKey (ulDecKeyHandle, &key);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in Loading IV by NEXUS_Security_LoadClearKey()\n\r", neResult);
						return DRV_ERR;
					}
			    }
#endif
				break;
			case DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY :
			case DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY :
#if defined(CONFIG_SECURITY_EXTENSION)
				NEXUS_Security_GetDefaultSessionKeySettings(&encSessionKey);
#else
				VK_MEM_Memset((void *)&encSessionKey, 0x0, sizeof(NEXUS_SecurityEncryptedSessionKey));
#endif
				encSessionKey.keyladderID	= NEXUS_SecurityKeyladderID_eA;
				encSessionKey.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
				encSessionKey.swizzleType   = NEXUS_SecuritySwizzleType_eNone;
#if (BCHP_CHIP == 7584) && defined(CONFIG_CAS_CX)
				encSessionKey.cusKeyL       = 0x13;
				encSessionKey.cusKeyH       = 0x13;
				encSessionKey.cusKeyVarL    = 0x77;
				encSessionKey.cusKeyVarH    = 0x40;
#else
				encSessionKey.cusKeyL       = 0;
				encSessionKey.cusKeyH       = 0;
				encSessionKey.cusKeyVarL    = 0;
				encSessionKey.cusKeyVarH	= 0xFF;
#endif
				encSessionKey.rootKeySrc    = NEXUS_SecurityRootKeySrc_eOtpKeyA;
			 	encSessionKey.bRouteKey     = false;
			 	encSessionKey.operation     = NEXUS_SecurityOperation_eDecrypt;
				encSessionKey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
			 	encSessionKey.keyEntryType  = NEXUS_SecurityKeyType_eOdd;
				encSessionKey.custSubMode			= NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
#if (BCHP_CHIP == 7584) && defined(CONFIG_CAS_CX)
				encSessionKey.virtualKeyLadderID 	= NEXUS_SecurityVirtualKeyladderID_eVKL1;
#else
				encSessionKey.virtualKeyLadderID	= NEXUS_SecurityVirtualKeyladderID_eVKL5;
#endif
				encSessionKey.keyMode				= NEXUS_SecurityKeyMode_eRegular;
#if defined(CONFIG_SECURITY_EXTENSION)
				encSessionKey.keyGenCmdID			= NEXUS_SecurityKeyGenCmdID_eKeyGen;
				encSessionKey.sessionKeyOp		= NEXUS_SecuritySessionKeyOp_eNoProcess;
#if defined(CONFIG_CAS_ASKM)
				encSessionKey.bASKMMode 		= true;
#else
				encSessionKey.bASKMMode 		= false;
#endif
				encSessionKey.bSwapAESKey			= false;
				encSessionKey.keyDestIVType 		= NEXUS_SecurityKeyIVType_eNoIV;
#endif
#if (BCHP_CHIP == 7584) && defined(CONFIG_CAS_CX)
				VK_memcpy((void *)encSessionKey.keyData, (void *)klrcKey, sizeof(pstDecryptSettings->ucProtectionKey));
#else
				VK_memcpy((void *)encSessionKey.keyData, (void *)pstDecryptSettings->ucProtectionKey, sizeof(pstDecryptSettings->ucProtectionKey));
#endif
				neResult = NEXUS_Security_GenerateSessionKey(ulDecKeyHandle, &encSessionKey);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[P_PLAY_SetDecryption] Error(0x%x) in NEXUS_Security_GenerateSessionKey()\n\r", neResult);
					return DRV_ERR;
			    }

#if defined(CONFIG_SECURITY_EXTENSION)
				NEXUS_Security_GetDefaultControlWordSettings(&encCw);
#else
				VK_MEM_Memset((void *)&encCw, 0x0, sizeof(NEXUS_SecurityEncryptedControlWord));
#endif
				encCw.keyladderID = NEXUS_SecurityKeyladderID_eA;
				encCw.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
				encCw.bRouteKey = true;
				encCw.operation = NEXUS_SecurityOperation_eDecrypt;
				encCw.keyEntryType = NEXUS_SecurityKeyType_eOdd;
#if defined(CONFIG_SECURITY_EXTENSION)
				encCw.dest					= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
				encCw.bASKMMode 		= false;
#endif
				encCw.custSubMode			= NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
#if (BCHP_CHIP == 7584) && defined(CONFIG_CAS_CX)
				encCw.virtualKeyLadderID	= NEXUS_SecurityVirtualKeyladderID_eVKL1;
#else
				encCw.virtualKeyLadderID	= NEXUS_SecurityVirtualKeyladderID_eVKL5;
#endif
				encCw.keyMode				= NEXUS_SecurityKeyMode_eRegular;
#if defined(CONFIG_SECURITY_EXTENSION)
				encCw.keyDestIVType 		= NEXUS_SecurityKeyIVType_eNoIV;
				encCw.keyGenCmdID			= NEXUS_SecurityKeyGenCmdID_eKeyGen;
				encCw.bSwapAESKey			= false;
#endif
				encCw.keySize = sizeof(decKey);
				VK_memcpy((void *)encCw.keyData, (void *)decKey, sizeof(decKey));
				neResult = NEXUS_Security_GenerateControlWord(ulDecKeyHandle , &encCw);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[P_PLAY_SetDecryption] Error(0x%x) in NEXUS_Security_GenerateControlWord()\n\r", neResult);
					return DRV_ERR;
			    }
#if 0 /* The DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY of DI10 uses NEXUS_SecurityAlgorithmVariant_eEcb, so the below code does not need */
				if(pstDecryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY)
				{
					/* Load IV for AES_CBC*/
					NEXUS_Security_GetDefaultClearKey(&key);
#if 0	/*Use this conventional-way to be compatible with DI_CRYPT*/
					key.keyEntryType = NEXUS_SecurityKeyType_eIv;
#else   /*To apply this, DI_CRYPT shall also be modified.*/
					key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
					key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
#endif
					key.keySize = sizeof(pstDecryptSettings->ucIv);
				    VK_memcpy(key.keyData,pstDecryptSettings->ucIv,key.keySize);
					neResult = NEXUS_Security_LoadClearKey (ulDecKeyHandle, &key);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in Loading IV by NEXUS_Security_LoadClearKey()\n\r", neResult);
						return DRV_ERR;
					}
				}
#endif
				break;
			default :
				PrintError("[P_PLAY_SetDecryption] Not supported encryption type! (eEncType : %d)\n\r", pstDecryptSettings->eEncType);
				return DRV_ERR_INVALID_PARAMETER;
		}
	}

	NEXUS_Playback_GetSettings(ulPbHandle, &PlaySettings);
	PlaySettings.playpumpSettings.securityDma = pstDecryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_NONE ? NULL : g_ulDmaHandle;
    PlaySettings.playpumpSettings.securityContext = pstDecryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_NONE ? NULL : ulDecKeyHandle;
	neResult = NEXUS_Playback_SetSettings(ulPbHandle, &PlaySettings);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_PLAY_SetDecryption] Error(0x%x) in NEXUS_Playback_SetSettings()\n\r", neResult);
		return DRV_ERR;
    }

	PrintExit("P_PLAY_SetDecryption");
#endif
	return DRV_OK;
}

#if defined(CONFIG_THUMBNAIL_DECODING)
static DRV_Error P_PLAY_SetDecryptionRaw(NEXUS_PlaypumpHandle ulPumpHandle, NEXUS_KeySlotHandle ulDecKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstDecryptSettings)
{
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
	NEXUS_Error	neResult;
	NEXUS_SecurityAlgorithmSettings	algConfig;
	NEXUS_SecurityClearKey			key;
	NEXUS_SecurityEncryptedSessionKey encSessionKey;
	NEXUS_SecurityEncryptedControlWord encCw;
	NEXUS_PlaypumpSettings			pumpSettings;
	HUINT8 	decKey[ENCRYPTION_KEY_LEN];

	PrintEnter("P_PLAY_SetDecryptionRaw\r");

	NEXUS_Security_GetDefaultClearKey(&key);
	if(pstDecryptSettings->eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
	{
		VK_MEM_Memcpy(decKey, pstDecryptSettings->ucKey, ENCRYPTION_KEY_LEN);
		if(pstDecryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_3DES)
		{
			DRV_CRYPT_ExpandKeyLength(decKey,decKey);
		}
		NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
		switch(pstDecryptSettings->eEncType)
		{
			case DI_PVR_ENCRYPTION_TYPE_3DES :
			case DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY :
				algConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
	   			algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				break;
			case DI_PVR_ENCRYPTION_TYPE_AES :
			case DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY :
				algConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
				algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
				algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
				break;
			default :
				PrintError("[P_PLAY_SetDecryptionRaw] Not supported encryption type! (eEncType : %d)\n\r", pstDecryptSettings->eEncType);
				return DRV_ERR_INVALID_PARAMETER;
		}
	    algConfig.operation = NEXUS_SecurityOperation_eDecrypt;
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
		algConfig.enableTimestamps = true; /* enable timestamps */
#endif
	    neResult = NEXUS_Security_ConfigAlgorithm(ulDecKeyHandle, &algConfig);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[P_PLAY_SetDecryptionRaw] Error(0x%x) in NEXUS_Security_ConfigAlgorithm()\n\r", neResult);
			return DRV_ERR;
	    }

	    /* Load AV keys */
		switch(pstDecryptSettings->eEncType)
		{
			case DI_PVR_ENCRYPTION_TYPE_3DES :
			case DI_PVR_ENCRYPTION_TYPE_AES :
				key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
			    key.keySize = sizeof(decKey);
			    VK_memcpy(key.keyData, decKey, sizeof(decKey));
			    neResult = NEXUS_Security_LoadClearKey(ulDecKeyHandle, &key);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[P_PLAY_SetDecryptionRaw] Error(0x%x) in NEXUS_Security_LoadClearKey()\n\r", neResult);
					return DRV_ERR;
			    }
#if 0 /* The DI_PVR_ENCRYPTION_TYPE_AES of DI10 uses NEXUS_SecurityAlgorithmVariant_eEcb, so the below code does not need */
				if(pstDecryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_AES)
				{
					/* Load IV for AES_CBC*/
					NEXUS_Security_GetDefaultClearKey(&key);
#if 0	/*Use this conventional-way to be compatible with DI_CRYPT*/
					key.keyEntryType = NEXUS_SecurityKeyType_eIv;
#else   /*To apply this, DI_CRYPT shall also be modified.*/
					key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
					key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
#endif
					key.keySize = sizeof(pstDecryptSettings->ucIv);
				    VK_memcpy(key.keyData,pstDecryptSettings->ucIv,key.keySize);
					neResult = NEXUS_Security_LoadClearKey (ulDecKeyHandle, &key);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in Loading IV by NEXUS_Security_LoadClearKey()\n\r", neResult);
						return DRV_ERR;
					}
				}
#endif
				break;
			case DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY :
			case DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY :
				encSessionKey.keyladderID   = NEXUS_SecurityKeyladderID_eA;
				encSessionKey.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
				encSessionKey.swizzleType   = NEXUS_SecuritySwizzleType_eNone;
				encSessionKey.cusKeyL       = 0;
				encSessionKey.cusKeyH       = 0;
				encSessionKey.cusKeyVarL    = 0;
				encSessionKey.cusKeyVarH    = 0;
				encSessionKey.rootKeySrc    = NEXUS_SecurityRootKeySrc_eOtpKeyA;
			 	encSessionKey.bRouteKey     = false;
			 	encSessionKey.operation     = NEXUS_SecurityOperation_eDecrypt;
			 	encSessionKey.keyEntryType  = NEXUS_SecurityKeyType_eOdd;
				VK_memcpy((void *)encSessionKey.keyData, (void *)pstDecryptSettings->ucProtectionKey, sizeof(pstDecryptSettings->ucProtectionKey));
				neResult = NEXUS_Security_GenerateSessionKey(ulDecKeyHandle, &encSessionKey);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[P_PLAY_SetDecryptionRaw] Error(0x%x) in NEXUS_Security_GenerateSessionKey()\n\r", neResult);
					return DRV_ERR;
			    }

				encCw.keyladderID = NEXUS_SecurityKeyladderID_eA;
				encCw.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
				encCw.bRouteKey = true;
				encCw.operation = NEXUS_SecurityOperation_eDecrypt;
				encCw.keyEntryType = NEXUS_SecurityKeyType_eOdd;
				encCw.keySize = sizeof(decKey);
				VK_memcpy((void *)encCw.keyData, (void *)decKey, sizeof(decKey));
				neResult = NEXUS_Security_GenerateControlWord(ulDecKeyHandle , &encCw);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[P_PLAY_SetDecryptionRaw] Error(0x%x) in NEXUS_Security_GenerateControlWord()\n\r", neResult);
					return DRV_ERR;
			    }
#if 0 /* The DI_PVR_ENCRYPTION_TYPE_AES of DI10 uses NEXUS_SecurityAlgorithmVariant_eEcb, so the below code does not need */
				if(pstDecryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY)
				{
					/* Load IV for AES_CBC*/
					NEXUS_Security_GetDefaultClearKey(&key);
#if 0	/*Use this conventional-way to be compatible with DI_CRYPT*/
					key.keyEntryType = NEXUS_SecurityKeyType_eIv;
#else   /*To apply this, DI_CRYPT shall also be modified.*/
					key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
					key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
#endif
					key.keySize = sizeof(pstDecryptSettings->ucIv);
				    VK_memcpy(key.keyData,pstDecryptSettings->ucIv,key.keySize);
					neResult = NEXUS_Security_LoadClearKey (ulDecKeyHandle, &key);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in Loading IV by NEXUS_Security_LoadClearKey()\n\r", neResult);
						return DRV_ERR;
					}
				}
#endif
				break;
			default :
				PrintError("[P_PLAY_SetDecryptionRaw] Not supported encryption type! (eEncType : %d)\n\r", pstDecryptSettings->eEncType);
				return DRV_ERR_INVALID_PARAMETER;
		}
	}

	NEXUS_Playpump_GetSettings(ulPumpHandle, &pumpSettings);
	pumpSettings.securityDma = pstDecryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_NONE ? NULL : g_ulDmaHandle;
    pumpSettings.securityContext = pstDecryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_NONE ? NULL : ulDecKeyHandle;
	neResult = NEXUS_Playpump_SetSettings(ulPumpHandle, &pumpSettings);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_PLAY_SetDecryptionRaw] Error(0x%x) in NEXUS_Playback_SetSettings()\n\r", neResult);
		return DRV_ERR;
	}

	PrintExit("P_PLAY_SetDecryptionRaw");
#endif
	return DRV_OK;
}
#endif

static void P_PLAY_GenericCallback(void *context, int param)
{
	VK_EVENT_Send((unsigned long)context, param);
}

static DRV_Error P_PLAY_ResetPacing(HUINT32 ulDeviceId)
{
	NEXUS_Error	neResult;

	neResult = NEXUS_Playpump_SuspendPacing(g_astPbInst[ulDeviceId].pbPumpHandle, true);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_PLAY_ResetPacing] Error(0x%x) in NEXUS_Playpump_SuspendPacing()\n\r", neResult);
		return DRV_ERR;
	}

	neResult = NEXUS_Playpump_SuspendPacing(g_astPbInst[ulDeviceId].pbPumpHandle, false);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_PLAY_ResetPacing] Error(0x%x) in NEXUS_Playpump_SuspendPacing()\n\r", neResult);
		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_PLAY_SetPosition(HUINT32 ulDeviceId, HUINT32 ulTime, HUINT32 *pulResultTimeInMs)
{
	NEXUS_Error	neResult;
	NEXUS_PlaybackStatus	playStatus;
	NEXUS_PlaybackPosition	seekTime;

	if(g_astPbInst[ulDeviceId].isExternalPlay)
	{
		neResult = NEXUS_Playpump_SuspendPacing(g_astPbInst[ulDeviceId].pbPumpHandle, true);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[P_PLAY_SetPosition] Error(0x%x) in NEXUS_Playpump_SuspendPacing()\n\r", neResult);
			return DRV_ERR;
		}
	}

	neResult = NEXUS_Playback_GetStatus(g_astPbInst[ulDeviceId].pbHandle, &playStatus);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_PLAY_SetPosition] Error(0x%x) in NEXUS_Playback_GetStatus()\n\r", neResult);
		return DRV_ERR;
	}
	seekTime = (NEXUS_PlaybackPosition)(ulTime*1000); //sec -> ms
	if (seekTime > playStatus.last)
	{
		seekTime = playStatus.last;
	}
	else if(seekTime < playStatus.first)
	{
		seekTime = playStatus.first;
	}
	neResult = NEXUS_Playback_Seek(g_astPbInst[ulDeviceId].pbHandle, seekTime);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_PLAY_SetPosition] Error(0x%x) in NEXUS_Playback_Seek()\n\r", neResult);
		return DRV_ERR;
	}
	*pulResultTimeInMs = (HUINT32)seekTime;

	if(g_astPbInst[ulDeviceId].isExternalPlay)
	{
		neResult = NEXUS_Playpump_SuspendPacing(g_astPbInst[ulDeviceId].pbPumpHandle, false);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[P_PLAY_SetPosition] Error(0x%x) in NEXUS_Playpump_SuspendPacing()\n\r", neResult);
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

static DRV_Error P_PLAY_TryPause(HUINT32 ulDeviceId, HBOOL *pbNeedRetry)
{
	NEXUS_Error	neResult;
	NEXUS_PidChannelHandle videoPidHandle = NULL; /* video decoder handle 정보를 얻어올 수 있는 video PID handle */
	NEXUS_PlaybackPidChannelSettings pidChannelSettings;
	NEXUS_VideoDecoderStatus videoDecStatus;
#if defined(TSR_PAUSE_WORKAROUND)
	// workaround, tsr play를 pause로 시작한 경우 hdmi에서 정지 영상 나오지 않은 이슈의 workaround
	NEXUS_VideoDecoderSettings decoderSettings;
#endif
	NEXUS_PlaybackStatus playStatus;

	*pbNeedRetry = false;
	if(g_astPbInst[ulDeviceId].setup.eStreamType == eDI_PVR_Stream_Video && g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle)
	{
		videoPidHandle = g_astPbInst[ulDeviceId].isExternalPlay ? g_astPbInst[ulDeviceId].pbPidInfo.dummyVideoPidHandle : g_astPbInst[ulDeviceId].pbPidInfo.videoPidHandle;
		neResult = NEXUS_Playback_GetPidChannelSettings(g_astPbInst[ulDeviceId].pbHandle,
													videoPidHandle,
													&pidChannelSettings);
	    if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[P_PLAY_TryPause] Error(0x%x) in NEXUS_Playback_GetPidChannelSettings()\n\r", neResult);
			return DRV_ERR;
	    }

		if(pidChannelSettings.pidTypeSettings.video.decoder)
		{
			neResult = NEXUS_VideoDecoder_GetStatus(pidChannelSettings.pidTypeSettings.video.decoder, &videoDecStatus);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[P_PLAY_TryPause] Error(0x%x) in NEXUS_VideoDecoder_GetStatus()\n\r", neResult);
				return DRV_ERR;
		    }

#if defined(TSR_PAUSE_WORKAROUND)
			// workaround, 2012-12-02
			// TSR play를 pause로 시작한 경우 hdmi에서 정지 영상 나오지 않은 이슈의 workaround
			// decoder mute가 disable 되는 시점이 pause가 되는 시점보다 늦거나 근소한 차이로 먼저되는 경우
			// hd display에 정지영상이 나타나지 않는다.
			// 그래서 decoder mute status를 읽어 playback pause를 해도 괜찮은 시점인지 체크하도록 한다.
			// 단, decoder mute 값을 set 한 이후 실제 동작하려면 시간이 걸리므로
			// decoder mute status가 false가 되더라도 1회 skip한다.
			NEXUS_VideoDecoder_GetSettings(pidChannelSettings.pidTypeSettings.video.decoder, &decoderSettings);

			if(decoderSettings.mute == false)
			{
				g_astPbInst[ulDeviceId].decoderMuteDisableCount++;
			}
			else
			{
				g_astPbInst[ulDeviceId].decoderMuteDisableCount=0;
			}

			PrintDebug("############# %s, %d, g_astPbInst[%d].decoderMuteDisableCount=%d\n",__FUNCTION__,__LINE__,ulDeviceId,g_astPbInst[ulDeviceId].decoderMuteDisableCount);
#endif

			if((videoDecStatus.numDecoded > 10) && (videoDecStatus.numDisplayed > 10)
				&& (videoDecStatus.numDecoded > videoDecStatus.numDecodeDrops)
				&& (videoDecStatus.numDisplayed > videoDecStatus.numDisplayDrops)
#if defined(TSR_PAUSE_WORKAROUND)
				// workaround, tsr play를 pause로 시작한 경우 hdmi에서 정지 영상 나오지 않은 이슈의 workaround
				&& (g_astPbInst[ulDeviceId].decoderMuteDisableCount > 1)
#endif
			)
			{
#if defined(TSR_PAUSE_WORKAROUND)
				// workaround, tsr play를 pause로 시작한 경우 hdmi에서 정지 영상 나오지 않은 이슈의 workaround
				g_astPbInst[ulDeviceId].decoderMuteDisableCount=0;
#endif
				/* external path playback인 경우 playpump pause(band hold가 동작하지 않아 계속 pumping하게 되기 때문에..) */
				if(g_astPbInst[ulDeviceId].isExternalPlay)
				{
					neResult = NEXUS_Playpump_SetPause(g_astPbInst[ulDeviceId].pbPumpHandle, true);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[P_PLAY_TryPause] Error(0x%x) in NEXUS_Playpump_SetPause\n\r", neResult);
						return DRV_ERR;
					}
				}
				neResult = NEXUS_Playback_Pause(g_astPbInst[ulDeviceId].pbHandle);
	   			if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[P_PLAY_TryPause] Error(0x%x) in NEXUS_Playback_Pause\n\r", neResult);
					return DRV_ERR;
				}
			}
			else
			{
				/* Reverse 로 BOF 에 도달했을때, videoDecStatus 의 값이 0 로 초기화됨에 따라 위 조건에 들어가지 않아서 추가함. */
				/* 이미 NEXUS_Playback 이 pause 상태인 경우, pause retry 를 하지않도록 */

				neResult = NEXUS_Playback_GetStatus(g_astPbInst[ulDeviceId].pbHandle, &playStatus);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[P_PLAY_SetPosition] Error(0x%x) in NEXUS_Playback_GetStatus()\n\r", neResult);
					return DRV_ERR;
				}
				if(playStatus.state == NEXUS_PlaybackState_ePaused)
				{
					PrintDebug("[P_PLAY_TryPause] already paused \n");
					*pbNeedRetry = false;
				}
				else
				{
					*pbNeedRetry = true;
				}
			}
		}
	}
	else
	{
		/* external path playback인 경우 playpump pause(band hold가 동작하지 않아 계속 pumping하게 되기 때문에..) */
		if(g_astPbInst[ulDeviceId].isExternalPlay)
		{
			neResult = NEXUS_Playpump_SetPause(g_astPbInst[ulDeviceId].pbPumpHandle, true);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[P_PLAY_TryPause] Error(0x%x) in NEXUS_Playpump_SetPause\n\r", neResult);
				return DRV_ERR;
			}
		}
		neResult = NEXUS_Playback_Pause(g_astPbInst[ulDeviceId].pbHandle);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[P_PLAY_TryPause] Error(0x%x) in NEXUS_Playback_Pause\n\r", neResult);
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

static DRV_Error P_PLAY_Resume(HUINT32 ulDeviceId)
{
	NEXUS_Error	neResult;

	if(g_astPbInst[ulDeviceId].isExternalPlay)
	{
		neResult = NEXUS_Playpump_SuspendPacing(g_astPbInst[ulDeviceId].pbPumpHandle, true);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[P_PLAY_Resume] Error(0x%x) in NEXUS_Playpump_SuspendPacing\n\r", neResult);
			return DRV_ERR;
		}
	}

	neResult = NEXUS_Playback_Play(g_astPbInst[ulDeviceId].pbHandle);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_PLAY_Resume] Error(0x%x) in NEXUS_Playback_Play\n\r", neResult);
		return DRV_ERR;
	}

	if(g_astPbInst[ulDeviceId].isExternalPlay)
	{
		neResult = NEXUS_Playpump_SetPause(g_astPbInst[ulDeviceId].pbPumpHandle, false);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[P_PLAY_Resume] Error(0x%x) in NEXUS_Playpump_SetPause\n\r", neResult);
			return DRV_ERR;
		}
		neResult = NEXUS_Playpump_SuspendPacing(g_astPbInst[ulDeviceId].pbPumpHandle, false);
		if(neResult != NEXUS_SUCCESS)
		{
			PrintError("[P_PLAY_Resume] Error(0x%x) in NEXUS_Playpump_SuspendPacing\n\r", neResult);
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

static void P_PLAY_PauseTimerCallback(unsigned long ulTimerId, void *params)
{
	unsigned long deviceId;

	BSTD_UNUSED(params);

	for(deviceId=0; deviceId<MAX_PLAY_INST_NUM; deviceId++)
	{
		if(g_astPbInst[deviceId].pauseTimerId == ulTimerId)
		{
			break;
		}
	}
	if(deviceId == MAX_PLAY_INST_NUM)
	{
		PrintError("[P_PLAY_PauseTimerCallback] Invalid timer ID! (ulTimerId : 0x%x)\n\r", ulTimerId);
		return;
	}

	if(!g_astPbInst[deviceId].pauseRetry)
	{
		PrintError("[P_PLAY_PauseTimerCallback] Pause retry is canceled!\n\r");
		return;
	}

	VK_EVENT_Send(g_ulPbPauseEventId, deviceId);
}

static void P_PLAY_PauseTask(void *param)
{
	int err;
	DRV_Error drvErr;
	unsigned long deviceId;

	BSTD_UNUSED(param);

	while(1)
	{
		err = VK_EVENT_Receive(g_ulPbPauseEventId, &deviceId); /* deviceId <- retrieved event */
		if(err != VK_OK)
		{
			PrintError("[P_PLAY_PauseTask] Error(0x%x) in VK_EVENT_Receive()\n\r", err);
			continue;
		}

		if(VK_SEM_Get(g_astPbInst[deviceId].diSemId) == VK_OK)
		{
			if(g_astPbInst[deviceId].pauseRetry)
			{
				drvErr = P_PLAY_TryPause(deviceId, (HBOOL *)&g_astPbInst[deviceId].pauseRetry);
				if(drvErr != DRV_OK)
				{
					PrintError("[P_PLAY_PauseTask] Error(0x%x) in P_PLAY_TryPause\n\r", drvErr);
					VK_SEM_Release(g_astPbInst[deviceId].diSemId);
					continue;
				}
				if(g_astPbInst[deviceId].pauseRetry)
				{
					err = VK_TIMER_EventAfter(PB_PAUSE_TIMER_VALUE, P_PLAY_PauseTimerCallback, NULL, 0, &g_astPbInst[deviceId].pauseTimerId);
					if(err != VK_OK)
					{
						PrintError("[P_PLAY_PauseTask] Error(0x%x) in VK_TIMER_EventAfter\n\r", err);
						VK_SEM_Release(g_astPbInst[deviceId].diSemId);
						continue;
					}
				}
				else
				{
					g_astPbInst[deviceId].playSpeed = eDI_PVR_Speed_Pause;
					g_astPbInst[deviceId].status = DI_PVR_PAUSED;
				}
			}
			VK_SEM_Release(g_astPbInst[deviceId].diSemId);
		}
	}
}

static void P_PLAY_EventTask(void *param)
{
	NEXUS_Error	neResult;
	DRV_Error drvErr = DRV_OK;
	int err;
	unsigned long deviceId;
	unsigned long retrieveEvents;
	DI_PVR_MSG_t	stCallbackMsg;
	HUINT32 		seekResultTimeInMs;
	NEXUS_PlaybackStatus	playStatus;
	unsigned long	lastTimeCode = 0;

	BSTD_UNUSED(param);

	deviceId = 0; // event 관련 함수의 interface 문제로 한 개의 playback device에 괸련된 event만 처리할 수 있다. default : PB device Id 0

	while(1)
	{
		VK_MEM_Memset(&stCallbackMsg, 0x0, sizeof(DI_PVR_MSG_t));
		err = VK_EVENT_Receive(g_ulPbTaskEventId, &retrieveEvents);
		if(err != VK_OK)
		{
			PrintError("[P_PLAY_EventTask] Error(0x%x) in VK_EVENT_Receive()\n\r", err);
			continue;
		}

		if(VK_SEM_Get(g_astPbInst[deviceId].diSemId) == VK_OK)
		{
			if((g_astPbInst[deviceId].status != DI_PVR_ENABLED && g_astPbInst[deviceId].status != DI_PVR_PAUSED)
				||(g_astPbInst[deviceId].recDeviceInfo.deviceId == -1 && g_astPbInst[deviceId].setup.eFileType == eDI_PVR_FileType_Circular))
			{
				PrintDebug("[P_PLAY_EventTask] Playback device stop :(%x)\n\r", retrieveEvents);
				VK_SEM_Release(g_astPbInst[deviceId].diSemId);
				continue;
			}

			if(retrieveEvents & (unsigned long)PB_BOF_EVENTS)
			{
				if(g_astPbInst[deviceId].setup.eFileType == eDI_PVR_FileType_Linear)
				{
					drvErr = P_PLAY_SetPosition(deviceId, 0, &seekResultTimeInMs);
					if(drvErr != DRV_OK)
					{
						PrintError("[P_PLAY_EventTask] Error(0x%x) in P_PLAY_SetPosition\n\r", drvErr);
					}
					g_astPbInst[deviceId].lastTimeStatus = (unsigned long)0;
					lastTimeCode = (unsigned long)0;

					if(g_astPbInst[deviceId].pbEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED] != NULL)
					{
						stCallbackMsg.ulSender = DI_PVR_PLAYER;
						stCallbackMsg.ulInstance = (HUINT32)deviceId;
						stCallbackMsg.ulEvent = DI_PVR_EVENT_TIMECODE_CHANGED;
						stCallbackMsg.ulParam1 = lastTimeCode;
						stCallbackMsg.ulParam2 = 0;
						PrintDebug("[P_PLAY_EventTask] DI_PVR_EVENT_TIMECODE_CHANGED:(%d)\n\r", lastTimeCode);
						(*g_astPbInst[deviceId].pbEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED])((void *)&stCallbackMsg);
					}
				}

				PrintDebug("[P_PLAY_EventTask] DI_PVR_EVENT_BOF\n\r");
				if(g_astPbInst[deviceId].pbEvtCbFunc[DI_PVR_EVENT_BOF] != NULL)
				{
					stCallbackMsg.ulSender = DI_PVR_PLAYER;
					stCallbackMsg.ulInstance = (HUINT32)deviceId;
					stCallbackMsg.ulEvent = DI_PVR_EVENT_BOF;
					stCallbackMsg.ulParam1 = 0;
					stCallbackMsg.ulParam2 = 0;
					(*g_astPbInst[deviceId].pbEvtCbFunc[DI_PVR_EVENT_BOF])((void *)&stCallbackMsg);
				}

			}
			if(retrieveEvents & (unsigned long)PB_EOF_EVENTS)
			{
				neResult = NEXUS_Playback_GetStatus(g_astPbInst[deviceId].pbHandle, &playStatus);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[P_PLAY_EventTask] Error(0x%x) in NEXUS_Playback_GetStatus\n\r", neResult);
					drvErr = DRV_ERR;
				}
				else
				{
					g_astPbInst[deviceId].lastTimeStatus = (unsigned long)playStatus.last;
					lastTimeCode = (unsigned long)(playStatus.last/1000); //ms -> sec
					drvErr = DRV_OK;
				}

				if (!g_astPbInst[deviceId].setup.bIsRecording)
				{
					if(drvErr == DRV_OK)
					{
						if(g_astPbInst[deviceId].pbEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED] != NULL)
						{
							stCallbackMsg.ulSender = DI_PVR_PLAYER;
							stCallbackMsg.ulInstance = (HUINT32)deviceId;
							stCallbackMsg.ulEvent = DI_PVR_EVENT_TIMECODE_CHANGED;
							stCallbackMsg.ulParam1 = lastTimeCode;
							stCallbackMsg.ulParam2 = 0;
							PrintDebug("[P_PLAY_EventTask] DI_PVR_EVENT_TIMECODE_CHANGED:(%d)\n\r", lastTimeCode);
							(*g_astPbInst[deviceId].pbEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED])((void *)&stCallbackMsg);
						}
					}
				}

				PrintDebug("[P_PLAY_EventTask] DI_PVR_EVENT_EOF\n\r");
				if(g_astPbInst[deviceId].pbEvtCbFunc[DI_PVR_EVENT_EOF] != NULL)
				{
					stCallbackMsg.ulSender = DI_PVR_PLAYER;
					stCallbackMsg.ulInstance = (HUINT32)deviceId;
					stCallbackMsg.ulEvent = DI_PVR_EVENT_EOF;
					stCallbackMsg.ulParam1 = 0;
					stCallbackMsg.ulParam2 = 0;
					(*g_astPbInst[deviceId].pbEvtCbFunc[DI_PVR_EVENT_EOF])((void *)&stCallbackMsg);
				}
			}
			else
			{
				PrintError("[P_PLAY_EventTask] Event not defined :(%x)\n\r", retrieveEvents);
			}
			VK_SEM_Release(g_astPbInst[deviceId].diSemId);
		}
	}
}

static void P_PLAY_TimeTask(void *param)
{
	NEXUS_Error	neResult;
	DRV_Error drvErr;
	int err;
	P_PLAY_TASK_MSG	rcvMsg;
	unsigned long	deviceId;
	HBOOL	exitFlag = FALSE;
	unsigned long			eventTimeOut;
	DI_PVR_MSG_t 			stCallbackMsg;
	NEXUS_PlaybackStatus	playStatus;
	NEXUS_PlaypumpStatus	playpumpStatus;
	unsigned long			lastTime = 0;
	unsigned long			ulEndTime = 0;

	NEXUS_StcChannelStatus status;

	BSTD_UNUSED(param);

	eventTimeOut = PB_MSG_TIMEOUT;
	deviceId = 0; // device ID == 0이라고 가정

	while(1)
	{
		VK_MEM_Memset(&stCallbackMsg, 0x0, sizeof(DI_PVR_MSG_t));
		err = VK_MSG_ReceiveTimeout(g_ulPbTimeTaskMsgQueueId, (void *)&rcvMsg, sizeof(P_PLAY_TASK_MSG), eventTimeOut);
		if(err != VK_OK)
		{
			if(VK_SEM_Get(g_astPbInst[deviceId].diSemId) == VK_OK)
			{
				if(g_astPbInst[deviceId].status == DI_PVR_ENABLED || g_astPbInst[deviceId].status == DI_PVR_PAUSED)
				{
					exitFlag = FALSE;

					neResult = NEXUS_StcChannel_GetStatus(DRV_DEMUX_GetStcChannelHandle(), &status);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[P_PLAY_TimeTask] Error(0x%x) in NEXUS_Playback_GetStatus\n\r", neResult);
						exitFlag = TRUE;
					}

//					PrintDebug("[P_PLAY_TimeTask] Audio state = %d \n\r",status.audio_state);

//20091015 :: POOH :: audio recording file을 playback 도중 trick을 할 경우 이전 값이 pcr이 setting되는 도중에
//                                  올라오는 문제 수정. stream type을 넣은 이유는 video trick시 구분하기 위함.
#if 0 /*[hee] Enabled with nexus version 750 or below but shall be checked that this code is still required with latest nexus s/w.*/
					if ((status.audio_state != NEXUS_StcChannelDecoder_State_eLocked && status.audio_state != NEXUS_StcChannelDecoder_State_eLocking)
						&& (g_astPbInst[deviceId].setup.eStreamType == eDI_PVR_Stream_Audio))
					{
						PrintError("[P_PLAY_TimeTask] audio state is not NEXUS_StcChannelDecoder_State_eLocked \n\r");
						exitFlag = true;
					}
#endif
					neResult = NEXUS_Playback_GetStatus(g_astPbInst[deviceId].pbHandle, &playStatus);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[P_PLAY_TimeTask] Error(0x%x) in NEXUS_Playback_GetStatus\n\r", neResult);
						exitFlag = TRUE;
					}
#if 1 /* workaround for audio only playback time notify */
					if((g_astPbInst[deviceId].setup.eStreamType == eDI_PVR_Stream_Audio) && !exitFlag)
					{
						/* Audio의 PTS 값이 TPIT의 PCR 값과 상이한 경우가 있어서 playStatus.position을 제대로 얻어오지 못할 수 있음.
						  -> Audio only playback인 경우 playStatus.position 값을 emulation하여 사용하도록 함 */
						if(g_astPbInst[deviceId].playSpeed == eDI_PVR_Speed_Pause)
						{
							playStatus.position = g_astPbInst[deviceId].lastTimeStatus;
						}
						else
						{
							playStatus.position = g_astPbInst[deviceId].lastTimeStatus + eventTimeOut;
						}
					}
#endif

					if(!(playStatus.position/1000 == g_astPbInst[deviceId].lastTimeStatus/1000) )
					{
						PrintDebug("[ %d ]\n",playStatus.position/1000);
					}

					if(!exitFlag)
					{
						if(g_astPbInst[deviceId].playSpeed < eDI_PVR_Speed_Pause)
						{
							if(playStatus.position < g_astPbInst[deviceId].lastTimeStatus)
							{
								g_astPbInst[deviceId].lastTimeStatus = playStatus.position;
							}
						}
						else
						{
							if(playStatus.position > g_astPbInst[deviceId].lastTimeStatus)
							{
								g_astPbInst[deviceId].lastTimeStatus = playStatus.position;
							}
						}
					}
					lastTime = g_astPbInst[deviceId].lastTimeStatus/1000; //ms -> sec
					ulEndTime = playStatus.last/1000; //ms -> sec

					/* pacing error 발생 시 video 멈칫거리는 문제 관련 workaround */
					if(g_astPbInst[deviceId].isExternalPlay)
					{
						neResult = NEXUS_Playpump_GetStatus(g_astPbInst[deviceId].pbPumpHandle, &playpumpStatus);
						if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[P_PLAY_TimeTask] Error(0x%x) in NEXUS_Playpump_GetStatus\n\r", neResult);
						}
						else
						{
							if(playpumpStatus.pacingTsRangeError != g_astPbInst[deviceId].lastPacingTsRangeError)
							{
								drvErr = P_PLAY_ResetPacing(deviceId);
								if(drvErr != DRV_OK)
								{
									PrintError("[P_PLAY_TimeTask] Error(0x%x) in P_PLAY_ResetPacing\n\r", drvErr);
								}
								else
								{
									g_astPbInst[deviceId].lastPacingTsRangeError = playpumpStatus.pacingTsRangeError;
								}
							}
						}
					}

					if((g_astPbInst[deviceId].pbEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED] != NULL)&&(exitFlag != TRUE))
					{
						stCallbackMsg.ulSender = DI_PVR_PLAYER;
						stCallbackMsg.ulInstance = deviceId;
						stCallbackMsg.ulEvent = DI_PVR_EVENT_TIMECODE_CHANGED;
						stCallbackMsg.ulParam1 = lastTime;
						if (g_astPbInst[deviceId].setup.bIsRecording)
						{
							stCallbackMsg.ulParam2 = ulEndTime;
						}
						else
						{
							stCallbackMsg.ulParam2 = 0;
						}
						(*g_astPbInst[deviceId].pbEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED])((void *)&stCallbackMsg);

						PrintDebug("[P_PLAY_TimeTask] DI_PVR_EVENT_TIMECODE_CHANGED:(%d) %d\n\r", lastTime, ulEndTime);
					}
				}
				VK_SEM_Release(g_astPbInst[deviceId].diSemId);
			}
		}
		else
		{
			if(VK_SEM_Get(g_astPbInst[deviceId].diSemId) == VK_OK)
			{
				if(rcvMsg.msgId == P_PLAY_MSG_TIMECODE_START)
				{
					eventTimeOut = PB_TIME_NOTIFY_TIMER_VALUE;
					PrintDebug("[P_PLAY_TimeTask] P_PLAY_MSG_TIMECODE_START\n\r");
				}
				if(rcvMsg.msgId == P_PLAY_MSG_TIMECODE_END)
				{
					eventTimeOut = PB_MSG_TIMEOUT;
					PrintDebug("[P_PLAY_TimeTask] P_PLAY_MSG_TIMECODE_END\n\r");
				}
				VK_SEM_Release(g_astPbInst[deviceId].diSemId);
			}
		}
	}
}

#if defined(CONFIG_THUMBNAIL_DECODING)
P_PLAY_THUMBNAIL_INFO gThumbnailInfo;
extern unsigned int	  g_VideoPid;
NEXUS_KeySlotHandle	  decKeyHandle = NULL;
DRV_Error DRV_PVR_PB_ThumbnailInit(NEXUS_VideoCodec videoCodec, NEXUS_PidChannelHandle *pPidChannel, DRV_PVR_THUMBNAIL_SETUP_t *pthumbSetup)
{
	DRV_Error drvError = DRV_OK;
	NEXUS_Error nexError;
	bthumbnail_extractor_create_settings create_settings;
	bthumbnail_extractor_settings settings;
	NEXUS_PlaypumpOpenPidChannelSettings openPidChannelSettings;
	NEXUS_PlaypumpSettings playpumpSettings;
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
	NEXUS_SecurityKeySlotSettings keySettings;
#endif
	int res;
	HBOOL bfromPlayer = TRUE;

	PrintEnter();

	if(pthumbSetup != NULL)
	{
		bfromPlayer = FALSE;
	}

	if(bfromPlayer == TRUE)
	{
		gThumbnailInfo.datafilename = g_AVFileName;
		gThumbnailInfo.indexfilename = g_IndexFileName;
		gThumbnailInfo.transportType = NEXUS_TransportType_eTs;
		gThumbnailInfo.pid = g_VideoPid;
	}
	else
	{
		gThumbnailInfo.datafilename = pthumbSetup->ucAVFileName;
		gThumbnailInfo.indexfilename = pthumbSetup->ucIndexFileName;
		gThumbnailInfo.transportType = NEXUS_TransportType_eTs;
		gThumbnailInfo.pid = pthumbSetup->ulVideoPid;
	}

#if defined(ENABLE_CHUNK_FILE_PLAYBACK)
	if(P_PLAY_IsDir(gThumbnailInfo.datafilename) != 0)
	{
		VK_strncat(gThumbnailInfo.datafilename, "/chunk.1", 8); // Add  fileExtension
	}
#endif

	PrintDebug("pid = 0x%lx (codec=%d) \n", gThumbnailInfo.pid, videoCodec);
	PrintDebug("data file = %s\n", gThumbnailInfo.datafilename);
	PrintDebug("index file = %s\n", gThumbnailInfo.indexfilename);

	drvError = DRV_PVR_GetPlaypumpHandle(&gThumbnailInfo.playpump);
	if(drvError != DRV_OK)
	{
		goto just_err_ret;
	}

	NEXUS_Playpump_GetDefaultSettings(&playpumpSettings);
	nexError = NEXUS_Playpump_SetSettings(gThumbnailInfo.playpump, &playpumpSettings);
	if(nexError != NEXUS_SUCCESS)
	{
		goto release_playpump;
	}

	/* Allocate decryption keyslot */

	if(bfromPlayer == FALSE)
	{
		if(pthumbSetup->stDecryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
		{
			if(decKeyHandle == NULL)
			{
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
				NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
				keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
				decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
				if(decKeyHandle == NULL)
				{
					PrintError("[DRV_PVR_PB_ThumbnailInit] Error in NEXUS_Security_AllocateKeySlot()\n\r");
					drvError = DRV_ERR;
					goto release_playpump;
				}
#endif
			}

			drvError = P_PLAY_SetDecryptionRaw(gThumbnailInfo.playpump, decKeyHandle, &pthumbSetup->stDecryptSettings);
			if(drvError != DRV_OK)
			{
				goto free_key_slot;
			}
		}
	}
	else
	{
	if(g_astPbInst[0].setup.stDecryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
	{
		if(decKeyHandle == NULL)
		{
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
			NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
			keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
			decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
			if(decKeyHandle == NULL)
			{
				PrintError("[DRV_PVR_PB_ThumbnailInit] Error in NEXUS_Security_AllocateKeySlot()\n\r");
				drvError = DRV_ERR;
				goto release_playpump;
			}
#endif
		}

		drvError = P_PLAY_SetDecryptionRaw(gThumbnailInfo.playpump, decKeyHandle, &g_astPbInst[0].setup.stDecryptSettings);
		if(drvError != DRV_OK)
		{
			goto free_key_slot;
		}
	}
	}
#ifdef CONFIG_VFIO
	res = VFIO_Open(gThumbnailInfo.datafilename, "rb", &gThumbnailInfo.VFIO_datafile);
	if (res)
#else
	gThumbnailInfo.stdio_datafile = fopen(gThumbnailInfo.datafilename, "rb");
	if (!gThumbnailInfo.stdio_datafile)
#endif
	{
		PrintError("can't open file:%s\n", gThumbnailInfo.datafilename);
		drvError = DRV_ERR;
		goto free_key_slot;
	}
#ifdef CONFIG_VFIO
	gThumbnailInfo.datafile = bfile_stdio_read_attach(gThumbnailInfo.VFIO_datafile);
#else
	gThumbnailInfo.datafile = bfile_stdio_read_attach(gThumbnailInfo.stdio_datafile);
#endif
	if (gThumbnailInfo.indexfilename)
	{
#ifdef CONFIG_VFIO
		res = VFIO_Open(gThumbnailInfo.indexfilename, "rb", &gThumbnailInfo.VFIO_indexfile);
		if (res)
#else
		gThumbnailInfo.stdio_indexfile = fopen(gThumbnailInfo.indexfilename, "rb");
		if (!gThumbnailInfo.stdio_indexfile)
#endif
		{
			PrintError("can't open index:%s\n", gThumbnailInfo.indexfilename);
			drvError = DRV_ERR;
			goto close_data_file;
		}
#ifdef CONFIG_VFIO
		gThumbnailInfo.indexfile = bfile_stdio_read_attach(gThumbnailInfo.VFIO_indexfile);
#else
		gThumbnailInfo.indexfile = bfile_stdio_read_attach(gThumbnailInfo.stdio_indexfile);
#endif
	}
	else
	{
		gThumbnailInfo.indexfile = NULL;
	}

	bthumbnail_extractor_get_default_create_settings(&create_settings);
	gThumbnailInfo.thumbnail_extractor = bthumbnail_extractor_create(&create_settings);

	bthumbnail_extractor_get_settings(gThumbnailInfo.thumbnail_extractor, &settings);
	settings.videoCodec = videoCodec;
	settings.transportType = gThumbnailInfo.transportType;
	settings.videoPid = gThumbnailInfo.pid;
	settings.playpump = gThumbnailInfo.playpump;
	settings.datafile = gThumbnailInfo.datafile;
	settings.indexfile = gThumbnailInfo.indexfile;
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
	settings.timestampType = NEXUS_TransportTimestampType_e32_Binary;
#else
	settings.timestampType = NEXUS_TransportTimestampType_eMod300;
#endif
#else
	settings.timestampType = NEXUS_TransportTimestampType_eNone;
#endif

	res = bthumbnail_extractor_set_settings(gThumbnailInfo.thumbnail_extractor, &settings);
	if (res)
	{
		PrintError("[%s] bthumbnail_extractor_set_settings failed (%d)\n",__func__, res);

		drvError = DRV_ERR;
		goto close_thumbnail_extractor;
	}

	res = bthumbnail_extractor_start_playpump(gThumbnailInfo.thumbnail_extractor);
	if (res)
	{
		PrintError("[%s] bthumbnail_extractor_start_playpump failed (%d)\n",__func__, res);
		drvError = DRV_ERR;
		goto close_thumbnail_extractor;

	}

	NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&openPidChannelSettings);
	openPidChannelSettings.pidType = NEXUS_PidType_eVideo;
	gThumbnailInfo.pidChannel = NEXUS_Playpump_OpenPidChannel(gThumbnailInfo.playpump, gThumbnailInfo.pid, &openPidChannelSettings);
	if (gThumbnailInfo.pidChannel != NULL)
	{
		*pPidChannel = gThumbnailInfo.pidChannel;
	}
	else
	{
		drvError = DRV_ERR;
		goto stop_thumbnail_extractor_pump;
	}
	PrintExit();
	return DRV_OK;

stop_thumbnail_extractor_pump:
	bthumbnail_extractor_stop_playpump(gThumbnailInfo.thumbnail_extractor);

close_thumbnail_extractor:
	bthumbnail_extractor_destroy(gThumbnailInfo.thumbnail_extractor);

#ifdef CONFIG_VFIO
	if (gThumbnailInfo.indexfile)
	{
		bfile_stdio_read_detach(gThumbnailInfo.indexfile);
		 VFIO_Close(gThumbnailInfo.VFIO_indexfile);
	}
#else
	if (gThumbnailInfo.indexfile) bfile_stdio_read_detach(gThumbnailInfo.indexfile);
	if (gThumbnailInfo.stdio_indexfile) fclose(gThumbnailInfo.stdio_indexfile);
#endif

close_data_file:
#ifdef CONFIG_VFIO
	bfile_stdio_read_detach(gThumbnailInfo.datafile);
	VFIO_Close(gThumbnailInfo.VFIO_datafile);
#else
	bfile_stdio_read_detach(gThumbnailInfo.datafile);
	fclose(gThumbnailInfo.stdio_datafile);
#endif

free_key_slot:
	if(decKeyHandle != NULL)
	{
		NEXUS_Security_FreeKeySlot(decKeyHandle);
	}

release_playpump:
	DRV_PVR_ReleasePlaypumpHandle(gThumbnailInfo.playpump);
just_err_ret:
	PrintExit();
	return DRV_ERR;
}


DRV_Error DRV_PVR_PB_ThumbnailFeed(unsigned int timesec)
{
	int res;
	unsigned time;
	PrintEnter();

	time = timesec;
	if(timesec < 5)
	{
		timesec = 5;
	}
	res = bthumbnail_extractor_feed_picture(gThumbnailInfo.thumbnail_extractor, time * 1000);
	if (res)
	{
		BERR_TRACE(res);
		return DRV_ERR;
	}
	PrintExit();
	return DRV_OK;
}


void DRV_PVR_PB_ThumbnailUninit(void)
{
	PrintEnter();
	bthumbnail_extractor_stop_playpump(gThumbnailInfo.thumbnail_extractor);
	NEXUS_Playpump_ClosePidChannel(gThumbnailInfo.playpump, gThumbnailInfo.pidChannel);

	bthumbnail_extractor_destroy(gThumbnailInfo.thumbnail_extractor);
#ifdef CONFIG_VFIO
	bfile_stdio_read_detach(gThumbnailInfo.datafile);
	VFIO_Close(gThumbnailInfo.VFIO_datafile);
	if (gThumbnailInfo.indexfile)
	{
		bfile_stdio_read_detach(gThumbnailInfo.indexfile);
		 VFIO_Close(gThumbnailInfo.VFIO_indexfile);
	}
#else
	bfile_stdio_read_detach(gThumbnailInfo.datafile);
	fclose(gThumbnailInfo.stdio_datafile);
	if (gThumbnailInfo.indexfile) bfile_stdio_read_detach(gThumbnailInfo.indexfile);
	if (gThumbnailInfo.stdio_indexfile) fclose(gThumbnailInfo.stdio_indexfile);
#endif
	DRV_PVR_ReleasePlaypumpHandle(gThumbnailInfo.playpump);
	PrintExit();
}
#endif

/* below PBHandle is used by CMD */
DRV_Error DRV_PVR_PB_GetPBHandle(HUINT32 unDeviceId, NEXUS_PlaybackHandle *ppstPlaybackHandle)
{
	if((ppstPlaybackHandle == NULL) || (unDeviceId >= MAX_PLAY_INST_NUM) )
	{
		PrintError("[DRV_PVR_PB_GetPBHandle] Error : Invalid parameter\n\r");
		return DRV_ERR;
	}

	*ppstPlaybackHandle = g_astPbInst[unDeviceId].pbHandle;

	return DRV_OK;
}

