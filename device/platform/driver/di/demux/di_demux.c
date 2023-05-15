/**
 * di_demux.c
*/

/**
 * @defgroup		DI_DEMUX DI_DEMUX : DI_DEMUX module
 * @author
 * @note			DI_DEMUX APIs
 * @brief			Define DI_DEMUX APIs
 * @file 			di_demux.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <sys/mman.h>

#include "nexus_platform.h"
#include "nexus_input_band.h"
#include "nexus_packetsub.h"
#include "nexus_message.h"
#include "nexus_remux.h"
#include "nexus_memory.h"
#include "nexus_pid_channel_scrambling.h"
#if (NEXUS_VERSION >= 1290)
#if defined(CONFIG_DEMOD_TSMF)
#include "nexus_tsmf.h"
#endif
#endif
#include "berr.h"
#include "blst_squeue.h"

#if defined(CONFIG_BCM_MTSIF)
#if ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_xpt_fe.h"
#endif
#endif
#include "linden_trace.h"
#include "drv_pvr.h"
#include "drv_demux.h"
#include "di_video.h"
#include "drv_video.h"
#include "di_demux.h"
#include "drv_tspath.h"
#include "di_uart.h"
#include "di_channel.h"
#if (NEXUS_VERSION >= 1290)
#if defined(CONFIG_DEMOD_TSMF) || defined(CONFIG_BCM_MTSIF)
#include "di_channel_priv.h"
#endif
#endif
#if defined(CONFIG_DEMOD_TSMF) || defined(CONFIG_BCM_MTSIF)
#include "drv_channel_config.h"
#endif
#include "di_err.h"
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"

#include "di_demux_priv.h"
#include "drv_gpio.h"
#if defined(CONFIG_CI)
#include "di_ci.h"
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
#include "drv_ci.h"
#else
#include "drv_cimax.h"
#endif
#endif

#if defined(CONFIG_MEDIA)
#if defined(CONFIG_MEDIA20)
#include "drv_media.h"
#elif defined(CONFIG_MEDIA10)
#include "drv_media_10.h"
#endif
#endif
#if defined(CONFIG_CALYPSO)
#include "drv_calypso.h"
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
#include "nexus_timebase.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

//#define CONFIG_PARSERBAND_OPTIMIZE

//#define CONFIG_NEW_STARTPID_API
#define CONFIG_MSG_PID2BUF
#define CONFIG_NEW_SETPATH_API
#define MESSAGE_FILTER_START_STOP_ONLY

#define INVALID_32BITS	0xFFFFFFFF
#define P_DEMUX_INBAND_NOT_USED (NEXUS_NUM_INPUT_BANDS-1)//NEXUS_InputBand_e5
#define TEMP_DEMUX_ID_e8	8

#define NOT_USED	0
#define TUNER_NONE 	0xFF

static P_DEMUX_Module demuxInstance;
static P_DEMUX_Callback gBackup_callback;

static HUINT32 gaRecorder2DemuxMapper[MAX_REC_CHANNEL];

void *PesFilterBuff[DRV_DMX_NUM_PID_CHANNEL];

#if defined(SAVE_FILTER_INFO)
static P_DEMUX_FilterDbgInfo filterDbgInfo[MAX_SECTION_FILTER_NUM];
#endif
volatile HUINT32 SectionDbgMode=  0x40000000; //0x80000002;

#if defined(CONFIG_PIP)
#define MAX_DECORD_NUM		2
#define MAIN_ID				0

static NEXUS_PidChannelHandle s_hPIPVideoPIDChannel = NULL;
static NEXUS_PidChannelHandle s_hPIPPcrPIDChannel = NULL;
static NEXUS_StcChannelHandle shstcChannel_o = NULL;
#endif

HUINT32	g_VideoPid;
static NEXUS_PidChannelHandle s_hVideoPIDChannel=NULL;
static NEXUS_PidChannelHandle s_hPcrPIDChannel = NULL;

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
#define DEMUX_PRIMER_MAX_COUNT		CONFIG_PRIMER_COUNT
static NEXUS_StcChannelHandle primerStcChannel[DEMUX_PRIMER_MAX_COUNT] = {NULL,};
#else
#define DEMUX_PRIMER_MAX_COUNT 0
#endif
static NEXUS_StcChannelHandle shstcChannel = NULL;
static NEXUS_PidChannelHandle s_hAudioPIDChannel=NULL;
static NEXUS_PidChannelHandle s_hDescriptionPIDChannel=NULL;

#if defined(CONFIG_TRANSCODER)
#define	NUM_TRANSCODE_STC		MAX_TRANS_CHANNEL
static NEXUS_StcChannelHandle s_subhstcChannel[NUM_TRANSCODE_STC] = {NULL,};
static NEXUS_StcChannelHandle s_stcChannelTranscode[NUM_TRANSCODE_STC] = {NULL,};
#endif

unsigned char *inter_buffer = NULL;
unsigned char *pes_inter_buffer = NULL;

HUINT32 PidChannelInfo[MAX_DEMUX_NUM][DI_DEMUX_CHANNELTYPE_MAX];

#if defined(CONFIG_DEBUG)
static char *PidType_Info[DI_DEMUX_CHANNELTYPE_MAX] = {
	"Video\0",
	"Audio Main\0",
	"Audio Sub\0",
	"Audio Spdif\0",
	"Audio track\0",
	"PCR primer\0",
	"Video primer\0",
	"Audio primer\0",
	"PCR\0",
	"Record\0",
	"Section\0",
	"PES\0",
	"All Pass\0",
	"Private\0",
};
#endif

static const char *strInputDev[DI_DEMUX_INPUT_MAX+1] = {
	"None",
	"Current",
	"Tuner",
	"Recorder",
	"dscRecorder",
	"Player",
	"dscPlayer",
	"Media",
	"External",
	"invalid"
};

static const char *strOutputDev[DI_DEMUX_OUTPUT_MAX+1] = {
	"None",
	"Current",
	"Recoder",
	"Decoder",
	"External",
	"Transcoder",
	"invalid"
};

#if defined(CONFIG_TS_MATRIX)
#define	MAX_TSMTRIX		6
static HUINT32	s_pTSMatrixValue[MAX_TSMTRIX] = {0,};
#endif

#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
HUINT8	gFootPrint = 0;
HUINT16  gTSCREQID = 0;
stTscCheck gTscCheck;
#endif

#if defined(CONFIG_BCM_MTSIF)
HULONG gulMTSIFInterface_Timer;
#endif

static HULONG ciLock;
static HULONG tscmonLock;
static HULONG pktsubLock;
static HULONG chLock[DRV_DMX_NUM_PID_CHANNEL];
static HULONG sqLock;
static HULONG inbandMonitorLock;
static HULONG cbLockSection;
static HULONG cbLockPES;
static HULONG cbLockTS;
static HULONG cbLockInbandChk;

static P_INBAND_MONITOR s_stInbandMonitor;

/*add TaskIDs */
HULONG sectiontaskId;
HULONG pestaskId;
HULONG inputbandmornitortaskId;
HULONG dbgsectiontaskId;
HULONG tsmontaskId;
HULONG tseventtaskId;

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
static P_REMUX_Module remuxInstance[REMUX_MAX_NUM];

#define CI_PATH_ON		1
#define CI_PATH_OFF		0
#define CI_LIVE_REMUX		0
#define CI_PLYABACK_REMUX	1
#define	PLAYBACK_PLAY		1

#define TUNER_MAX_NUM		4
HUINT32 CI_CONNECTED_TUNER[TUNER_MAX_NUM] = {0,};

#endif

BLST_SQ_HEAD(msgfilterlist_head, msgfilterlist);
struct msgfilterlist_head gMsgHead;

struct msgfilterlist
{
	BLST_SQ_ENTRY(msgfilterlist) link;
	unsigned long filterIndex;
};

struct msgfilterlist gMsgFilter[MAX_SECTION_FILTER_NUM];

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
static P_DEMUX_PRIMER_PID_CHANNEL_INFO_t s_stDemuxPrimerPidChannelInfo[DEMUX_PRIMER_MAX_COUNT];
#endif

#if defined(CONFIG_USE_RMX_EXT_CLOCK_INSTANCE)
extern NEXUS_InputBand s_ExtClockInputBand;
#endif

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
int DRV_DEMUX_CloseStcChannel(void);
static DRV_Error P_DEMUX_DisableFilter(P_DEMUX_FilterType eFilterType, HUINT8 filterIndex);

#if defined(CONFIG_TS_MATRIX)
#define GPIO_TS_MATRIX_MAX_NUM ((sizeof(GPIO_TsMatrixConfigInfoTable)) / (sizeof(GPIO_TsMatrixConfigInfo_t)))
DRV_Error DRV_SetTsMatrix(HUINT32 ulTs0_0, HUINT32 ulTs0_1, HUINT32 ulTs1_0, HUINT32 ulTs1_1, HUINT32 ulTs1_2, HUINT32 ulTs1_3);
static DRV_Error DRV_InitTsMatrix(void);
static DRV_Error DRV_UnInitTsMatrix(void);
#endif
#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
static DRV_Error P_DEMUX_StartPrimerPID(HUINT32 ulDemuxId, DI_DEMUX_CHANNELTYPE eChannelType, HUINT16 usPid, HUINT32 ulPrimerIndex);
static DRV_Error P_DEMUX_StopPrimerPID(HUINT32 ulChannelIndex, HUINT32 ulPrimerIndex);
#endif
static DRV_Error P_DEMUX_DEBUG_DestroySectionHandling(void);
#if defined(CONFIG_DEMUX_SET_UNUSED_INPUTBAND)
extern NEXUS_InputBand DRV_CFG_GetUnUsedInputBand(void);
#endif
extern HUINT32 DRV_CFG_GetTSMaxDevice(void);

static P_DEMUX_Module * P_DEMUX_GetInstance(void)
{
	return &demuxInstance;
}

static HUINT32 P_DEMUX_GetSizeAligned(HUINT32 ulSizeNotAligned)
{
	if(ulSizeNotAligned%4)
	{
		ulSizeNotAligned += 4- (ulSizeNotAligned%4);
	}
	return ulSizeNotAligned;
}

#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
static NEXUS_ParserBand P_DEMUX_GetIBParserForScrambledFilePlayback(void)
{
	return (NEXUS_ParserBand)(MAX_IBPARSER_BAND_NUM-1);
}

static NEXUS_ParserBand P_DEMUX_GetIBParserForScrambledFileRecording(void)
{
	return (NEXUS_ParserBand)(MAX_IBPARSER_BAND_NUM-2);
}
#endif

static NEXUS_ParserBand P_DEMUX_GetIBParserForHiddenChannel(void)
{
	return (NEXUS_ParserBand)(MAX_IBPARSER_BAND_NUM-2);
}

static DRV_Error P_DEMUX_CheckInputStatus(NEXUS_ParserBand parserBand, HUINT32 *pulCount)
{
	NEXUS_ParserBandSettings	pb_settings;
	NEXUS_InputBandStatus	inputStatus;

	if(parserBand >= NEXUS_ParserBand_eMax)
	{
		PrintError("Invalid parserband. (%ld)\n", parserBand);
		return DRV_ERR;
	}

	NEXUS_ParserBand_GetSettings(parserBand, &pb_settings);
	NEXUS_InputBand_GetStatus(pb_settings.sourceTypeSettings.inputBand, &inputStatus);
	*pulCount = inputStatus.syncCount;
	return DRV_OK;
}

static unsigned int P_DEMUX_GetPhysicalPidChannelIndex(NEXUS_PidChannelHandle pidChannel)
{
	NEXUS_Error	nexusError;
	NEXUS_PidChannelStatus pidChStatus;

	nexusError = NEXUS_PidChannel_GetStatus(pidChannel, &pidChStatus );
	if(nexusError != NEXUS_SUCCESS)
	{
		/*Just display error and keep going*/
		PrintError(" Failed to get status of pidChannel 0x%lx\n", pidChannel);
		return 255;
	}
	return pidChStatus.pidChannelIndex;
}

static NEXUS_MessageFormat P_DEMUX_GetNexusMessageFormat(P_DEMUX_FilterType eFilterType)
{
	NEXUS_MessageFormat nexusMsgFmt;

	switch(eFilterType)
	{
		case P_DEMUX_PsiFilter:
			nexusMsgFmt = NEXUS_MessageFormat_ePsi;
			break;
		case P_DEMUX_PesFilter:
			nexusMsgFmt = NEXUS_MessageFormat_ePes;
			break;

#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
		case P_DEMUX_TsFilter:
			nexusMsgFmt = NEXUS_MessageFormat_eTs;
			break;
#endif
#if defined(SECTION_FILTER_DEBUGGING)
		case P_DEMUX_DEBUG_PsiFilter:
			nexusMsgFmt = NEXUS_MessageFormat_ePsi;
			break;
#endif
		default:
			nexusMsgFmt = NEXUS_MessageFormat_eMax;
			break;
	}
	return nexusMsgFmt;
}

#if defined(CONFIG_MSG_PID2BUF)
#else
static bool P_DEMUX_CheckIfGroupingRequired(P_DEMUX_FilterType eFilterType)
{
	bool bGrouping;

	switch(eFilterType)
	{
		case P_DEMUX_PsiFilter:
			bGrouping = true;
			break;
		case P_DEMUX_PesFilter:
			bGrouping = false;
			break;

#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
		case P_DEMUX_TsFilter:
			bGrouping = false;
			break;
#endif
#if defined(SECTION_FILTER_DEBUGGING)
		case P_DEMUX_DEBUG_PsiFilter:
			bGrouping = true;
			break;
#endif
		default:
			bGrouping = false;
			break;
	}
	return bGrouping;
}
#endif

static HBOOL P_DEMUX_CheckIfFilterBeAttached(DI_DEMUX_CHANNELTYPE channelType)
{
	if((channelType >= DI_DEMUX_CHANNELTYPE_RECORD && channelType <= DI_DEMUX_CHANNELTYPE_REMUX) || (channelType == DI_DEMUX_CHANNELTYPE_PCR))
	{
		return TRUE;
	}
	return FALSE;
}

HBOOL DRV_DEMUX_CheckIfFilterBeAttached(DRV_DEMUX_CHANNELTYPE channelType)
{
	if((channelType >= DRV_DEMUX_CHANNELTYPE_RECORD && channelType <= DRV_DEMUX_CHANNELTYPE_REMUX) || (channelType == DRV_DEMUX_CHANNELTYPE_PCR))
	{
		return TRUE;
	}
	return FALSE;
}

static void P_DEMUX_InputBandMonitoringTask(void *param)
{
	int Count;
	HUINT32 ulSyncCount=0;
	BSTD_UNUSED(param);

	while(TRUE)
	{
		VK_TASK_Sleep(1000);
		for(Count = 0; Count < LIVE_DEMUX_NUM; Count++)
		{
			if(s_stInbandMonitor.bMonitor[Count] == TRUE)
			{
				P_DEMUX_CheckInputStatus((NEXUS_ParserBand)Count, &ulSyncCount);
				if(ulSyncCount == s_stInbandMonitor.ulPrevSyncCount[Count])
				{
					s_stInbandMonitor.ulCheckCount[Count]++;
				}
				else
				{
					s_stInbandMonitor.bNoInputReported[Count] = FALSE;
					s_stInbandMonitor.ulCheckCount[Count] = 0;
				}
				s_stInbandMonitor.ulPrevSyncCount[Count]= ulSyncCount;
			}
			else
			{
				s_stInbandMonitor.ulPrevSyncCount[Count] = 0;
				s_stInbandMonitor.ulCheckCount[Count] = 0;
			}
			if(s_stInbandMonitor.ulCheckCount[Count] > 3)
			{
				s_stInbandMonitor.ulCheckCount[Count] = 0;
				if(s_stInbandMonitor.bNoInputReported[Count] == FALSE)
				{
					s_stInbandMonitor.bNoInputReported[Count] = TRUE;
					VK_SEM_Get(cbLockInbandChk);
					if(s_stInbandMonitor.NoInputData != NULL)
					{
						s_stInbandMonitor.NoInputData(Count);
					}
					VK_SEM_Release(cbLockInbandChk);
				}
			}
		}

	}
}

static DRV_Error P_DEMUX_StartInbandMonitoring(void)
{
	P_DEMUX_Module *pDemux;
	HUINT32 i;
	HUINT32 err;

	err = VK_SEM_Create( &inbandMonitorLock, "INBAND_MON_SEM", VK_SUSPENDTYPE_FIFO);
	if(err){
		PrintError("VK_SEM_Create() error, DRV_DEMUX_Init(), inbandMonitorLock\n");
		return DRV_ERR;
	}
	for(i=0; i< LIVE_DEMUX_NUM; i++)
	{
		s_stInbandMonitor.numFilter[i] = 0;
		s_stInbandMonitor.bMonitor[i] = FALSE;
		s_stInbandMonitor.bNoInputReported[i]= FALSE;
		s_stInbandMonitor.ulCheckCount[i] = 0;
		s_stInbandMonitor.ulPrevSyncCount[i] = 0;
	}
	s_stInbandMonitor.NoInputData = NULL;

	pDemux = P_DEMUX_GetInstance();
	for(i=0; i<MAX_SECTION_FILTER_NUM; i++){
		pDemux->msgFilter[i].status = DRV_DEMUX_RESOURCE_FREE;
	}

	err = VK_TASK_Create(P_DEMUX_InputBandMonitoringTask,
						DRV_DEMUX_SEF_HANDLER_TASK_PRIORITY,
						DRV_DEMUX_SEF_HANDLER_TASK_STACK_SIZE,
						"InbandMon",
						NULL,
						&inputbandmornitortaskId, FALSE);
	if( err != VK_OK )
	{
		(void) VK_SEM_Destroy(inbandMonitorLock);
		PrintError("P_DEMUX_StartInbandMonitoring() Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Start(inputbandmornitortaskId);
	if(err != VK_OK)
	{
		(void) VK_SEM_Destroy(inbandMonitorLock);
		(void) VK_TASK_Destroy(inputbandmornitortaskId);
		PrintError("P_DEMUX_StartInbandMonitoring() Error(0x%x) in VK_TASK_Start()\n\r", err);
		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_DEMUX_StopInbandMonitoring(void)
{
	HUINT32 err;

	err = VK_TASK_Stop(inputbandmornitortaskId);
	if(err != VK_OK )
	{
		PrintError("P_DEMUX_StopInbandMonitoring() Error(0x%x) in VK_TASK_Stop()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Destroy(inputbandmornitortaskId);
	if(err != VK_OK )
	{
		PrintError("P_DEMUX_StopInbandMonitoring() Error(0x%x) in VK_TASK_Destroy()\n\r", err);
		return DRV_ERR;
	}

	err = VK_SEM_Destroy(inbandMonitorLock);
	if(err != VK_OK )
	{
		PrintError("P_DEMUX_StopInbandMonitoring() Error(0x%x) in VK_SEM_Destroy()\n\r", err);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error P_DEMUX_ConvertDRVtoDIChannelType(DRV_DEMUX_CHANNELTYPE drvChannelType, DI_DEMUX_CHANNELTYPE *pDiChannelType)
{

	DI_DEMUX_CHANNELTYPE channelType = DI_DEMUX_CHANNELTYPE_MAX;
	unsigned char numofdiChannelType = 0,numofdrvChannelType = 0;

	numofdiChannelType = DI_DEMUX_CHANNELTYPE_MAX;
	numofdrvChannelType = DRV_DEMUX_CHANNELTYPE_MAX;

	if(numofdiChannelType != numofdrvChannelType)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(drvChannelType == DRV_DEMUX_CHANNELTYPE_VIDEO)
		channelType = DI_DEMUX_CHANNELTYPE_VIDEO;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO_MAIN)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO_MAIN;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO_SUB)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO_SUB;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO_SPDIF)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF;
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_PCR_PRIMER)
		channelType = DI_DEMUX_CHANNELTYPE_PCR_PRIMER;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_VIDEO_PRIMER)
		channelType = DI_DEMUX_CHANNELTYPE_VIDEO_PRIMER;
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER)
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO_PRIMER)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER;
#endif
#elif defined(CONFIG_FAST_AUDIO_CHANGE)
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO_PRIMER)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER;
#else
#endif

	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_PCR)
		channelType = DI_DEMUX_CHANNELTYPE_PCR;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_RECORD)
		channelType = DI_DEMUX_CHANNELTYPE_RECORD;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_SECTION)
		channelType = DI_DEMUX_CHANNELTYPE_SECTION;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_PES)
		channelType = DI_DEMUX_CHANNELTYPE_PES;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_ALLPASS)
		channelType = DI_DEMUX_CHANNELTYPE_ALLPASS;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_PRIVATE)
		channelType = DI_DEMUX_CHANNELTYPE_PRIVATE;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_REMUX)
		channelType = DI_DEMUX_CHANNELTYPE_REMUX;
#if defined(CONFIG_PIP)
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_VIDEO_SUB)
		channelType = DI_DEMUX_CHANNELTYPE_VIDEO_SUB;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_PCR_SUB)
		channelType = DI_DEMUX_CHANNELTYPE_PCR_SUB;
#endif
#if defined(CONFIG_TRANSCODER)
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_1)
		channelType = DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_1;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO_TRANS_1)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_1;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_PCR_TRANS_1)
		channelType = DI_DEMUX_CHANNELTYPE_PCR_TRANS_1;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_2)
		channelType = DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_2;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO_TRANS_2)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_2;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2)
		channelType = DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2;
	else if(drvChannelType == DRV_DEMUX_CHANNELTYPE_PCR_TRANS_2)
		channelType = DI_DEMUX_CHANNELTYPE_PCR_TRANS_2;
#endif
	else
	{
		return DRV_ERR;
	}

	*pDiChannelType = channelType;

	return DRV_OK;

}

DRV_Error P_DEMUX_ConvertDItoDRVChannelType(DI_DEMUX_CHANNELTYPE diChannelType,DRV_DEMUX_CHANNELTYPE *pDrvChannelType)
{

	DRV_DEMUX_CHANNELTYPE channelType = DRV_DEMUX_CHANNELTYPE_MAX;
	unsigned char numofdiChannelType = 0,numofdrvChannelType = 0;

	numofdiChannelType = DI_DEMUX_CHANNELTYPE_MAX;
	numofdrvChannelType = DRV_DEMUX_CHANNELTYPE_MAX;

	if(numofdiChannelType != numofdrvChannelType)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(diChannelType == DI_DEMUX_CHANNELTYPE_VIDEO)
		channelType = DRV_DEMUX_CHANNELTYPE_VIDEO;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO_MAIN)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO_MAIN;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO_SUB)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO_SUB;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO_SPDIF;
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_PCR_PRIMER)
		channelType = DRV_DEMUX_CHANNELTYPE_PCR_PRIMER;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_VIDEO_PRIMER)
		channelType = DRV_DEMUX_CHANNELTYPE_VIDEO_PRIMER;
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER)
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO_PRIMER;
#endif
#elif defined(CONFIG_FAST_AUDIO_CHANGE)
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO_PRIMER;
#else
#endif

	else if(diChannelType == DI_DEMUX_CHANNELTYPE_PCR)
		channelType = DRV_DEMUX_CHANNELTYPE_PCR;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_RECORD)
		channelType = DRV_DEMUX_CHANNELTYPE_RECORD;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_SECTION)
		channelType = DRV_DEMUX_CHANNELTYPE_SECTION;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_PES)
		channelType = DRV_DEMUX_CHANNELTYPE_PES;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_ALLPASS)
		channelType = DRV_DEMUX_CHANNELTYPE_ALLPASS;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_PRIVATE)
		channelType = DRV_DEMUX_CHANNELTYPE_PRIVATE;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_REMUX)
		channelType = DRV_DEMUX_CHANNELTYPE_REMUX;
#if defined(CONFIG_PIP)
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_VIDEO_SUB)
		channelType = DRV_DEMUX_CHANNELTYPE_VIDEO_SUB;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_PCR_SUB)
		channelType = DRV_DEMUX_CHANNELTYPE_PCR_SUB;
#endif
#if defined(CONFIG_TRANSCODER)
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_1)
		channelType = DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_1;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_1)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO_TRANS_1;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_PCR_TRANS_1)
		channelType = DRV_DEMUX_CHANNELTYPE_PCR_TRANS_1;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_2)
		channelType = DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_2;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_2)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO_TRANS_2;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2)
		channelType = DRV_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2;
	else if(diChannelType == DI_DEMUX_CHANNELTYPE_PCR_TRANS_2)
		channelType = DRV_DEMUX_CHANNELTYPE_PCR_TRANS_2;
#endif
	else
	{
		return DRV_ERR;
	}

	*pDrvChannelType = channelType;

	return DRV_OK;

}

DI_ERR_CODE DI_DEMUX_RegisterInbandStatusCheckCallback (pfnDI_DEMUX_NoInputData pfnNoInputData)
{
	PrintEnter();
	VK_SEM_Get(cbLockInbandChk);
	s_stInbandMonitor.NoInputData = pfnNoInputData;
	VK_SEM_Release(cbLockInbandChk);
	PrintExit();

	return DI_ERR_OK;
}

static void P_DEMUX_SectionEventTask(void* param)
{
	P_DEMUX_Module *pDemux;
	HUINT32 message_length;
	HULONG  filter_index;
	HUINT8 *buffer, *tmp_buf;
	size_t size;

#if defined(CONFIG_COMPLETE_SECTION_IN_DI)
	HUINT8 *buffer_o;
	size_t size_o;
	size_t size_total;
	HUINT32 diff = 0;
#endif

	NEXUS_Error rc;
	int i;
	int vk_ret;
#if defined(CONFIG_MSG_PID2BUF)
	HUINT32	ulRetrieveCount = 0;
	HBOOL	bEventReceived = FALSE;
#endif
	P_DEMUX_MSG_Struct msg;
	unsigned long requestId;
	DI_ERR_CODE diError;
	HBOOL bSectionOverflow=FALSE;

	BSTD_UNUSED(param);
	pDemux = P_DEMUX_GetInstance();

	while(TRUE)
	{
		vk_ret = VK_MSG_Receive(pDemux->eventID[P_DEMUX_PsiFilter], &msg, sizeof(P_DEMUX_MSG_Struct));
		if(vk_ret != VK_OK)
		{
			PrintError("VK_MSG_Receive returns error. (err=%ld)\n", vk_ret);
			continue;
		}
#if defined(CONFIG_MSG_PID2BUF)
		bEventReceived = TRUE;
#endif
		filter_index = msg.ulFilterIDX;
		requestId = msg.ulReqID;
		bSectionOverflow = msg.bSectionOverflow;

		PrintDebug("P_DEMUX_SectionEventTask(), filter_index = %d  requestId = 0x%lx\n", filter_index, requestId);

		if(filter_index >= MAX_SECTION_FILTER_NUM)
		{
			PrintError("P_DEMUX_SectionEventTask(), filter_index is wrong = %d \n",filter_index);
			continue;
		}

		//Check if filter is still alive.
		VK_SEM_Get(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
#if defined(CONFIG_MSG_PID2BUF)
		ulRetrieveCount = 0;
RETRIEVE_AGAIN:
		if(ulRetrieveCount > 0)
		{
			ulRetrieveCount--;
		}
#endif
		if(pDemux->msgFilter[filter_index].status== DRV_DEMUX_RESOURCE_ENABLED &&
			requestId == pDemux->msgFilter[filter_index].requestId)
		{
#if defined(CONFIG_COMPLETE_SECTION_IN_DI)
			if((rc=NEXUS_Message_GetBufferWithWrap(pDemux->msgFilter[filter_index].msgHandle, (const void **)&buffer, &size, (const void **)&buffer_o, &size_o)))
			{
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				PrintDebug("P_DEMUX_SectionEventTask(), NEXUS_Message_GetBuffer() return error (%ld)[filter_index=%d]\n",rc, filter_index);
				continue;
			}
#else
			if((rc=NEXUS_Message_GetBuffer(pDemux->msgFilter[filter_index].msgHandle, (const void **)&buffer, &size)))
			{
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				PrintDebug("P_DEMUX_SectionEventTask(), NEXUS_Message_GetBuffer() return error (%ld)[filter_index=%d]\n",rc, filter_index);
				continue;
			}
#endif
			if(!size)
			{
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				/*size can be zero in normal condition. PrintError("P_DEMUX_SectionEventTask(), NEXUS_Message_GetBuffer() read size 0\n");*/
				continue;
			}
#if defined(CONFIG_MSG_PID2BUF)
			if(size < 3)
			{
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				/*nothing to do without section size info. this case can be happen in pid2buf mode.*/
				continue;
			}
#endif
			message_length = (((buffer[1]&0xF)<<8)|(buffer[2]))+3;
			message_length = P_DEMUX_GetSizeAligned(message_length);
#if defined(CONFIG_MSG_PID2BUF)
#if defined(CONFIG_COMPLETE_SECTION_IN_DI)
			size_total = size + size_o;

			if(size_total == message_length && message_length <= INTER_BUFFER_SIZE)
			{
				VK_memcpy(inter_buffer, buffer, size);

				if(size_o != 0)
					VK_memcpy(inter_buffer, buffer_o, size_o);

				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, message_length);
			}
			else if(size_total > message_length && message_length <= INTER_BUFFER_SIZE)
			{

				VK_memcpy(inter_buffer, buffer, size);

				if(message_length > size)
				{
					diff = message_length;
					diff -= size;

					VK_memcpy(inter_buffer, buffer_o, diff);
				}

				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, message_length);
				if(message_length <= 94 && bEventReceived == TRUE)
				{
					bEventReceived = FALSE;
					ulRetrieveCount = (188/message_length)-1;
				}
			}
#else
			if(size == message_length && message_length <= INTER_BUFFER_SIZE)
			{
				VK_memcpy(inter_buffer, buffer, message_length);
				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, message_length);
			}
			else if(size > message_length && message_length <= INTER_BUFFER_SIZE)
			{
				VK_memcpy(inter_buffer, buffer, message_length);
				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, message_length);
				if(message_length <= 94 && bEventReceived == TRUE)
				{
					bEventReceived = FALSE;
					ulRetrieveCount = (188/message_length)-1;
				}
			}
			else if(size < message_length)
			{

				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, 0);
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				/*waits next event. this case can be happen in pid2buf mode.*/
				continue;
			}
#endif
#else
			if(message_length == size && size <= INTER_BUFFER_SIZE)
			{
				VK_memcpy(inter_buffer, buffer, message_length);
				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, size);
			}
#endif
			else
			{
				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, size);
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				PrintError(" message_length(%ld) != size(%ld) or section is too big  [filter_index=%d]\n", message_length, size, filter_index);
				while(1);
			}

#if defined(CONFIG_DEBUG)
			PrintDebug("[recv] : ");
			for(i=0; i<6; i++)
				PrintDebug("%02x ", inter_buffer[i]);
			PrintDebug("\n");
#if defined(PRINT_KNOWN_SECTION_INFO)
			if(inter_buffer[0] == 0x00)
			{
				if(SectionDbgMode & 0x00000001)
				{
					DI_UART_Print("[%ld] PAT arrived (0x%02x) filter=%d reqid=0x%lx\n",pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId, inter_buffer[0], filter_index, pDemux->msgFilter[filter_index].requestId);
				}
			}
			else if(inter_buffer[0] == 0x02)
			{
				if(SectionDbgMode & 0x00000002)
				{
					DI_UART_Print("[%ld] PMT arrived (0x%02x) filter=%d reqid=0x%lx\n",pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId, inter_buffer[0], filter_index, pDemux->msgFilter[filter_index].requestId);
				}
			}
			else if(inter_buffer[0] == 0x42 || inter_buffer[0] == 0x46)
			{
				if(SectionDbgMode & 0x00000004)
				{
					DI_UART_Print("[%ld] SDT arrived (0x%02x) filter=%d reqid=0x%lx\n",pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId, inter_buffer[0], filter_index, pDemux->msgFilter[filter_index].requestId);
				}
			}
			else if(inter_buffer[0] == 0x4e || inter_buffer[0] == 0x4f)
			{
				if(SectionDbgMode & 0x00000008)
				{
					DI_UART_Print("[%ld] EIT arrived (0x%02x) filter=%d reqid=0x%lx\n",pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId, inter_buffer[0], filter_index, pDemux->msgFilter[filter_index].requestId);
				}
			}
#endif
#endif
			VK_SEM_Get(cbLockSection);
			if( pDemux->callback.pfnAllocBuffer != NULL  && pDemux->callback.pfnSectionNotify!= NULL && bSectionOverflow == FALSE)
			{
				diError = pDemux->callback.pfnAllocBuffer(message_length, &tmp_buf);
				if(diError != DI_ERR_OK)
				{
					VK_SEM_Release(cbLockSection);
					VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
					PrintError("pfnAllocBuffer() return error\n");
					continue;
				}
				VK_memcpy(tmp_buf, inter_buffer, message_length);
				diError = pDemux->callback.pfnSectionNotify(pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId \
												, tmp_buf, pDemux->msgFilter[filter_index].requestId);
				if(diError != DI_ERR_OK)
				{
					PrintError("pfnSectionNotify() return error filter_index=%ld, reqid=0x%lx\n", filter_index, pDemux->msgFilter[filter_index].requestId);
				}
			}
			VK_SEM_Release(cbLockSection);
#if defined(CONFIG_MSG_PID2BUF)
			if(ulRetrieveCount > 0)
			{
				goto RETRIEVE_AGAIN;
			}
#endif
			VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);

		}
		else
		{
			VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
			PrintDebug("%s, Message Filter (filterIdx: %ld) is already stopped.\n",__FUNCTION__, filter_index);\
			continue;
		}
	}
}

#define CHECK_NEXUS_RET_STATUS(rc) \
	if(rc != 0) \
	{ \
		VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]); \
		PrintError("%s returns error. (rc=%d)\n", __FUNCTION__, rc); \
		continue; \
	}

static DRV_Error P_DEMUX_ProcessVbiTeletext(unsigned char *pucPesPktBuf, unsigned long ulPesPktLength)
{
	DRV_Error	error=DRV_ERR;
  	unsigned char *ptr;
	unsigned char pesDataIdentifier; /* 8-bits field: 0x10 to 0x1F for EBU data */
	unsigned char pesDataUnitId;  /* 8-bits field: 0x02 for EBU ttxt non-subtitle data / 0x03 for subtitle data */
	unsigned char pesDataUnitLength; /* 8-bits field: always 0x2C for ttxt */
	unsigned char fieldParity;    /* 1-bit field: first or second field of a frame */
	unsigned char lineOffset;    /* 5-bits field: from 0x07 to 0x16, or 0x00 (undefined) */
	unsigned char framingCode;   /* 8-bits field: framing code 0xE4/0x27 */
	unsigned char topParityFlag=0, botParityFlag=0; /** Flags for valid data parsing from pes package */
	DRV_VIDEO_VBI_TT botData[VBI_TT_ENTRY_SIZE];	/** bottom field data and framing code */
	DRV_VIDEO_VBI_TT topData[VBI_TT_ENTRY_SIZE];	/** top field data and framing code */
	//unsigned long tabVal[625];
	unsigned long i;

	if((pucPesPktBuf == NULL) || (ulPesPktLength == 0) || (ulPesPktLength >= 65535))
		return DRV_ERR;

	/* PES packet: start code prefix(3) + stream_id(1) + PES length(2) +Option flag(2) + header_length(1)+header_data(36)
	              +Payload_data(....)
	 According to En300472  the header length must be 0x24(36). So the entire PES header
	 is 45 bytes long. And the first byte of PES payload is data identifier of EBU. It must be between 0x10 and 0x1F. Other value is reserved
	 */
	 /* data identifier offset = PES header(45) = 45 */
	ptr = pucPesPktBuf + 45;
	/* check data_identifier */
	pesDataIdentifier = *ptr;
	ptr++;

	for (i =0; i < VBI_TT_ENTRY_SIZE; i++)
	{
		topData[i].framingCode = VBI_TT_INVALID_FRAMING_CODE;
	}
	for (i =0; i < VBI_TT_ENTRY_SIZE; i++)
	{
		botData[i].framingCode = VBI_TT_INVALID_FRAMING_CODE;
	}

	while (ptr < (pucPesPktBuf + ulPesPktLength))
	{
		/* data_unit_id */
		pesDataUnitId = *ptr;
		ptr ++;
		/* data_unit_length */
		pesDataUnitLength = *ptr;
		ptr ++;
		/* reserved_for_future_use + field_parity + line_offset */
		fieldParity = *ptr;
		lineOffset = fieldParity & DRV_DEMUX_LINE_NUM_MASK;
		fieldParity = (fieldParity & DRV_DEMUX_ODD_FIELD_FLAG) != 0 ? 1 : 0;
		ptr++;
		/* framing_code */
		framingCode = *ptr;
		ptr++;
		/* magazine_and_packet_address */
		if ((pesDataUnitId == DRV_DEMUX_DATA_UNIT_ID_EBU_TTXT_DATA) ||
		    (pesDataUnitId == DRV_DEMUX_DATA_UNIT_ID_EBU_TTXT_SUBTITLE))
		{
			if (framingCode == VBI_TT_FRAMING_CODE)
		  	{
		    		if ((ptr+NEXUS_TELETEXT_LINESIZE) <= (pucPesPktBuf+ulPesPktLength))
				{
					if (fieldParity)
					{
						if (((unsigned)lineOffset-VBI_TT_FIELD_OFFSET) >= VBI_TT_ENTRY_SIZE)
						{
							PrintError(" Bottom Field Index too high\n");
							return DRV_ERR;
						}
						VK_memcpy(botData[lineOffset-VBI_TT_FIELD_OFFSET].data, ptr, NEXUS_TELETEXT_LINESIZE);
						botData[lineOffset-VBI_TT_FIELD_OFFSET].framingCode = VBI_TT_FRAMING_CODE;
						botParityFlag = 1;
					}
					else
					{
						if (((unsigned)lineOffset-VBI_TT_FIELD_OFFSET) >= VBI_TT_ENTRY_SIZE)
						{
							PrintError(" Top Field Index too high\n");
							return DRV_ERR;
						}
						VK_memcpy(topData[lineOffset-VBI_TT_FIELD_OFFSET].data, ptr, NEXUS_TELETEXT_LINESIZE);
						topData[lineOffset-VBI_TT_FIELD_OFFSET].framingCode = VBI_TT_FRAMING_CODE;
						topParityFlag = 1;
					}

		  			//tabVal[lineOffset]++;
				}
				else
				{
				 	/* We got a correct PES_DataField but the PES packet is imcomplete
				  	* so could be a wrap around not handled or a wrong packet size */
				  	PrintError(" PES packet imcomplet wrap-around:%d\n", ulPesPktLength);
				}
		  	}
		  	else
		  	{
		    		//PrintError(" Wrong Framing code 0x%02x Line:%d Filed:%d\n", framingCode, lineOffset, fieldParity);
		  	}
		}
		ptr += 42;
	}

	if(botParityFlag == 1) /** Only inserting bottom field data into VBI */
	{
		error = DRV_VIDEO_VBITeletext_Write(botData, VBI_TT_ENTRY_SIZE, 1);
		if(error != DRV_OK)
		{
			PrintError(" Bottom Field Error(0x%x) in DDI_VIDEO_SetVBITeletext\n\r", error);
			return DRV_ERR;
		}
	}
	if(topParityFlag == 1) /** Only inserting top field data into VBI */
	{
		error = DRV_VIDEO_VBITeletext_Write(topData, VBI_TT_ENTRY_SIZE, 0);
		if(error != DRV_OK)
		{
			PrintError(" Top Field Error(0x%x) in DDI_VIDEO_SetVBITeletext\n\r", error);
			return DRV_ERR;
		}
	}

	return error;
}



static void P_DEMUX_PesEventTask(void *param)
{
	P_DEMUX_Module *pDemux;
	HUINT32 message_length = 0;
	HULONG filter_index;
	HUINT8 *buffer, *tmp_buf;
	size_t size;
	NEXUS_Error rc;
	HUINT32 length_to_read = 0x0;
	HUINT32 dataRead=0;
	P_DEMUX_MSG_Struct msg;
	unsigned long requestId;
	DI_ERR_CODE diError;
	int vk_ret;

	BSTD_UNUSED(param);
	pDemux = P_DEMUX_GetInstance();

	while(TRUE)
	{
		vk_ret = VK_MSG_Receive(pDemux->eventID[P_DEMUX_PesFilter], &msg, sizeof(P_DEMUX_MSG_Struct));
		if(vk_ret != VK_OK)
		{
			PrintError("VK_MSG_Receive returns error. (err=%ld)\n", vk_ret);
			continue;
		}
		filter_index = msg.ulFilterIDX;
		requestId = msg.ulReqID;

		PrintDebug("[%s] filteridx = %d reqid = 0x%lx\n",__FUNCTION__, filter_index, requestId);
		message_length = 0;
		VK_SEM_Get(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
		if(pDemux->msgFilter[filter_index].status== DRV_DEMUX_RESOURCE_ENABLED &&
			requestId == pDemux->msgFilter[filter_index].requestId)
		{
			rc=NEXUS_Message_GetBuffer(pDemux->msgFilter[filter_index].msgHandle, (const void **)&buffer, &size);
			CHECK_NEXUS_RET_STATUS(rc);
			if(size >= PESH_SIZE)
			{
				//Check whether wrap-around condition or not.
				message_length = (((buffer[4])<<8)|(buffer[5]))+6;
				message_length = P_DEMUX_GetSizeAligned(message_length);
				if(message_length > size) //Wrap-around condition
				{
					VK_memcpy(pes_inter_buffer, buffer, size);
					rc=NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, size);
					dataRead = size;
					length_to_read = message_length - size;
					rc=NEXUS_Message_GetBuffer(pDemux->msgFilter[filter_index].msgHandle, (const void **)&buffer, &size);
					CHECK_NEXUS_RET_STATUS(rc);
					VK_memcpy(&pes_inter_buffer[dataRead], buffer, length_to_read);
					rc=NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, length_to_read);

				}
				else
				{
					VK_memcpy(pes_inter_buffer, buffer, message_length);
					rc=NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, message_length);
				}

			}
			else if(size > 0)
			{
				//Wrap-around condition
				VK_memcpy(pes_inter_buffer, buffer, size);
				rc=NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, size);
				dataRead = size;
				rc=NEXUS_Message_GetBuffer(pDemux->msgFilter[filter_index].msgHandle, (const void **)&buffer, &size);
				CHECK_NEXUS_RET_STATUS(rc);
				VK_memcpy(&pes_inter_buffer[dataRead], buffer, PESH_SIZE - dataRead);
				message_length = (((pes_inter_buffer[4])<<8)|(pes_inter_buffer[5]))+6;
				message_length = P_DEMUX_GetSizeAligned(message_length);
				length_to_read = message_length - dataRead;
				VK_memcpy(&pes_inter_buffer[dataRead],buffer,length_to_read);
				rc=NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, length_to_read);
			}
			else
			{
				//PrintDebug("Size 0\n");
			}
		}

		VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);

		/*Do VBI Insertion here for EBU TTX*/
		if(pDemux->msgFilter[filter_index].etPesType == DI_DEMUX_PESTYPE_TELETEXT && message_length != 0 )
		{
			P_DEMUX_ProcessVbiTeletext(pes_inter_buffer, message_length);
		}

		VK_SEM_Get(cbLockPES);
		if(message_length != 0 && pDemux->callback.pfnPesAllocBuffer[pDemux->msgFilter[filter_index].etPesType] != NULL && pDemux->callback.pfnPesNotify[pDemux->msgFilter[filter_index].etPesType] != NULL)
		{
			diError = pDemux->callback.pfnPesAllocBuffer[pDemux->msgFilter[filter_index].etPesType](message_length, &tmp_buf);
			if(diError != DI_ERR_OK)
			{
				VK_SEM_Release(cbLockPES);
				PrintError(" g_pfnAllocBuffer() return error\n");
				continue;
			}
			VK_memcpy(tmp_buf, pes_inter_buffer, message_length);
			diError = pDemux->callback.pfnPesNotify[pDemux->msgFilter[filter_index].etPesType](pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId \
										, tmp_buf, pDemux->msgFilter[filter_index].requestId);
			if(diError != DI_ERR_OK)
			{
				PrintError(" g_pfnSectionNotify() return error\n");
			}
		}
		VK_SEM_Release(cbLockPES);
	}

	return;
}

static void P_DEMUX_NEXUS_NotifyCB(void *context, int param)
{
	P_DEMUX_Module *pDemux;
	int ret;
	unsigned long qID;
	P_DEMUX_MSG_Struct msg;

	pDemux = P_DEMUX_GetInstance();
	qID = pDemux->eventID[GET_FILTERTYPE((unsigned int)context)];
	msg.ulFilterIDX = GET_FILTERINDEX((unsigned int)context);
	msg.ulReqID = (unsigned long)param;
	msg.bSectionOverflow = FALSE;
		PrintDebug("P_DEMUX_NEXUS_SectionNotifyCB(), filtertype = %x, filter index = %d reqid=%ld\n",GET_FILTERTYPE((unsigned int)context), msg.ulFilterIDX,msg.ulReqID );
	ret = VK_MSG_SendTimeout(qID, &msg, sizeof(P_DEMUX_MSG_Struct),0);
	if(ret == VK_TIMEOUT)
	{
		PrintError("Timeout occured! (filter_index = %d)\n",param);
	}

	return;
}

static void P_DEMUX_NEXUS_OverflowCB(void *context, int param)
{
	P_DEMUX_Module *pDemux;
	int ret;
	unsigned long qID;
	P_DEMUX_MSG_Struct msg;

	pDemux = P_DEMUX_GetInstance();
	qID = pDemux->eventID[GET_FILTERTYPE((unsigned int)context)];
	msg.ulFilterIDX = GET_FILTERINDEX((unsigned int)context);
	msg.ulReqID = (unsigned long)param;
	msg.bSectionOverflow = TRUE;

	if(GET_FILTERTYPE((unsigned int)context) == P_DEMUX_PsiFilter)
	{
		PrintError("P_DEMUX_NEXUS_OverflowCB(),Force to send filter index = %d reqid=0x%lx to PsiFilter Task!\n",msg.ulFilterIDX,msg.ulReqID );

		ret = VK_MSG_SendTimeout(qID, &msg, sizeof(P_DEMUX_MSG_Struct),0);
		if(ret == VK_TIMEOUT)
		{
			PrintError("Timeout occured! (filter_index = %d)\n",param);
		}
	}

	PrintDebug("P_DEMUX_NEXUS_OverflowCB(), context = 0x%lx, param = %ld\n", context, param);

	return;
}

static HBOOL P_DEMUX_HasCapability(HUINT32 ulDemuxId, DI_DEMUX_CAP cap)
{
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();
	return (pDemux->dmxDevice[ulDemuxId].stCapability.ulCapability & cap ? TRUE : FALSE);
}

static NEXUS_ParserBand P_DEMUX_GetParserBandByDemuxId(HUINT32 demuxId)
{
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBand parserBand;

#if MAX_IBPARSER_BAND_NUM == 5
	switch(demuxId){
		case DRV_DEMUX_ID_eHidden:
			parserBand = NEXUS_ParserBand_e3;
			break;
		case DRV_DEMUX_ID_e0 :
			parserBand = NEXUS_ParserBand_e0;
			break;
		case DRV_DEMUX_ID_e1 :
			parserBand = NEXUS_ParserBand_e1;
			break;
		case DRV_DEMUX_ID_e2 :
			parserBand = NEXUS_ParserBand_e2;
			break;
		case DRV_DEMUX_ID_e3 :
			pDemux = P_DEMUX_GetInstance();
			if(pDemux->dmxDevice[demuxId].nHiddenParserIndex == -1)
			{
				parserBand = NEXUS_ParserBand_ePb0;  /* parserBand is invalid if demuxId is for playback. Settings to a large value will flush out programming bugs. */
			}
			else
			{
				parserBand = NEXUS_ParserBand_e4;
			}
			break;
		case DRV_DEMUX_ID_e4 :
			pDemux = P_DEMUX_GetInstance();
			if(pDemux->dmxDevice[demuxId].nHiddenParserIndex == -1)
			{
				parserBand = NEXUS_ParserBand_ePb1;  /* parserBand is invalid if demuxId is for playback. Settings to a large value will flush out programming bugs. */
			}
			else
			{
				parserBand = NEXUS_ParserBand_e3;
			}
			break;
		default :
			parserBand = NEXUS_ParserBand_eInvalid;
			break;

	}
#else
	if(demuxId == DRV_DEMUX_ID_eHidden)
	{
		parserBand = P_DEMUX_GetIBParserForHiddenChannel();
	}
	else if(demuxId >= MAX_DEMUX_NUM)
	{
		parserBand = NEXUS_ParserBand_eInvalid;
	}
	else if(demuxId < LIVE_DEMUX_NUM)
	{
#if defined(CONFIG_PARSERBAND_OPTIMIZE)
		/*
			assign to the IB pareserband in Tuner ID
			for the sharing of the IB parserband
		*/
		pDemux = P_DEMUX_GetInstance();
		if(pDemux->dmxDevice[demuxId].pathInfo.ulRequestedTunerId == TUNER_NONE)
		{
			/* already path is set.
			    use latest parserband which is set by latest SetPath */
			parserBand = pDemux->dmxDevice[demuxId].pathInfo.parserBand;
		}
		else
		{
			/* on going set path.
			    use request tuner ID in setpath */
			parserBand = pDemux->dmxDevice[demuxId].pathInfo.ulRequestedTunerId;
		}
#else
		parserBand = (NEXUS_ParserBand)demuxId;
#endif
	}
	else /*LIVE_DEMUX_NUM <= demuxId < MAX_DEMUX_NUM*/
	{
		pDemux = P_DEMUX_GetInstance();
		if(pDemux->dmxDevice[demuxId].nHiddenParserIndex == -1)
		{
			parserBand = NEXUS_ParserBand_ePb0+(demuxId-LIVE_DEMUX_NUM);
		}
		else
		{
			parserBand = (MAX_IBPARSER_BAND_NUM-1)-(demuxId-LIVE_DEMUX_NUM);
		}
	}
#endif
	return parserBand;
}

static HINT32 P_DEMUX_GetAvailableHiddenParserIndex(HUINT32 ulDemuxId)
{
	HINT32 nHiddenParser = -1;

	if(ulDemuxId == (MAX_IBPARSER_BAND_NUM-2))
	{
		nHiddenParser = 0;
		PrintDebug("[%s] hidden parser index is (%d).\n", __func__, nHiddenParser);
	}
	else if(ulDemuxId == (MAX_IBPARSER_BAND_NUM-1))
	{
		nHiddenParser = 1;
		PrintDebug("[%s] hidden parser index is (%d).\n", __func__, nHiddenParser);
	}
	else
	{
		PrintDebug("[%s] Hidden parser index is not available for demux ID(%d).\n", __func__, ulDemuxId);
	}

	return nHiddenParser;
}

#if defined(CONFIG_CI)
static NEXUS_ParserBand P_DEMUX_GetCoParserBandForCI(NEXUS_ParserBand mainParserBand)
{
	NEXUS_ParserBand coParserBand;

	switch(mainParserBand)
	{
		default:
			coParserBand = (MAX_IBPARSER_BAND_NUM-1);
			break;
	}
	return coParserBand;
}
#endif

static HBOOL P_DEMUX_IsLiveParserBand(HUINT32 demuxId, DI_DEMUX_INPUTDEVICETYPE etProducer)
{
	if(demuxId < LIVE_DEMUX_NUM &&
		(etProducer == DI_DEMUX_INPUT_NONE ||
		etProducer == DI_DEMUX_INPUT_TUNER))
		return TRUE;
	else
		return FALSE;
}

static HBOOL P_DEMUX_IsPBParserBand(HUINT32 demuxId, DI_DEMUX_INPUTDEVICETYPE etProducer)
{
	if(demuxId >= LIVE_DEMUX_NUM &&
		( etProducer == DI_DEMUX_INPUT_NONE ||
		etProducer == DI_DEMUX_INPUT_RECORDER ||
		etProducer == DI_DEMUX_INPUT_SCRAMBLED_FILE_RECORDER ||
		etProducer == DI_DEMUX_INPUT_PLAYER ||
#if defined(CONFIG_SI_PARSING_FOR_MEDIA_PLAYBACK)
		etProducer == DI_DEMUX_INPUT_MEDIA_PLAYER ||
#endif
		etProducer == DI_DEMUX_INPUT_SCRAMBLED_FILE_PLAYER ))
		return TRUE;
	else
		return FALSE;
}

static NEXUS_InputBand P_DEMUX_GetInputBand(HUINT32 ulInputDevice_id)
{
	return DRV_TS_PATH_GetInbandID(ulInputDevice_id);
}

static NEXUS_InputBand P_DEMUX_GetUnusedInputBand(void)
{
#if defined(CONFIG_DEMUX_SET_UNUSED_INPUTBAND)
	return DRV_CFG_GetUnUsedInputBand();
#else
	int i;
	unsigned int tempInputbandIndex;
	unsigned int inputBandUsageStatus = 0;
#if INPUTBAND_FROM_REMUX_OUTPUT
//	int inputbandfromRemux = (int)INPUTBAND_FROM_REMUX_OUTPUT;
	int inputbandfromRemux = (int)DRV_CIRemux_PATH_GetInbandID(0);
#else
	int inputbandfromRemux = -1;
#endif
	static HINT32 firstUnusedInputBand = -1;
	if(firstUnusedInputBand == -1)
	{
		for(i=0;i<MAX_INPUT_BAND_NUM;i++)
		{
			tempInputbandIndex = P_DEMUX_GetInputBand(i);
			if(tempInputbandIndex == MAX_INPUT_BAND_NUM)
				break;
			inputBandUsageStatus |= (1<<tempInputbandIndex);
		}
		if(inputbandfromRemux >= 0)
			inputBandUsageStatus |= (1<<inputbandfromRemux);
		for(i=0;i<MAX_INPUT_BAND_NUM;i++)
		{
			if((inputBandUsageStatus & (1<<i)) == 0)
			{
				firstUnusedInputBand = i;
				break;
			}
		}
		if(i==MAX_INPUT_BAND_NUM)
		{
			firstUnusedInputBand = MAX_INPUT_BAND_NUM;
		}
	}

	return (NEXUS_InputBand)firstUnusedInputBand;
#endif
}

static DRV_Error P_DEMUX_PrepareSectionHandling(void)
{
	P_DEMUX_Module *pDemux;
	HUINT32 i;
	HUINT32 err;

	pDemux = P_DEMUX_GetInstance();
	for(i=0; i<MAX_SECTION_FILTER_NUM; i++){
		pDemux->msgFilter[i].status = DRV_DEMUX_RESOURCE_FREE;
	}

	err = VK_SEM_Create(&sqLock, "DmxSQ_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		PrintError("VK_SEM_Create() error, DmxSQ_SEM\n");
		return DRV_ERR;
	}
	BLST_SQ_INIT(&gMsgHead);
	for(i=0; i<MAX_SECTION_FILTER_NUM; i++)
	{
		gMsgFilter[i].filterIndex = i;
		BLST_SQ_INSERT_TAIL(&gMsgHead, &gMsgFilter[i], link);
	}

	if(VK_MSG_Create(DMX_MSGQ_COUNT, sizeof(P_DEMUX_MSG_Struct),"PsiMsg",&pDemux->eventID[P_DEMUX_PsiFilter],VK_SUSPENDTYPE_FIFO))
	{
		PrintError("DRV_DEMUX_Init(), VK_MSG_Create error\n");
		return DRV_ERR;
	}
	err = VK_TASK_Create(P_DEMUX_SectionEventTask,
						DRV_DEMUX_SEF_HANDLER_TASK_PRIORITY,
						DRV_DEMUX_SEF_HANDLER_TASK_STACK_SIZE,
						"SecTask",
						NULL,
						&sectiontaskId, FALSE);
	if( err != VK_OK )
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_PsiFilter]);
		PrintError("DRV_DEMUX_Init() Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Start(sectiontaskId);
	if(err != VK_OK)
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_PsiFilter]);
		VK_TASK_Destroy(sectiontaskId);
		PrintError("DRV_DEMUX_Init() Error(0x%x) in VK_TASK_Start()\n\r", err);
		return DRV_ERR;
	}
	return DRV_OK;
}

static DRV_Error P_DEMUX_DestorySectionHandling(void)
{
	P_DEMUX_Module *pDemux = NULL;
	HUINT32 err = 0;

	pDemux = P_DEMUX_GetInstance();
	if(pDemux == NULL)
	{
		PrintError("P_DEMUX_DestorySectionHandling() return err \n\r");
	}

	err = VK_TASK_Stop(sectiontaskId);
	if(err != VK_OK )
	{
		PrintError("P_DEMUX_DestorySectionHandling() Error(0x%x) in VK_TASK_Stop()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Destroy(sectiontaskId);
	if(err != VK_OK )
	{
		PrintError("P_DEMUX_DestorySectionHandling() Error(0x%x) in VK_TASK_Destroy()\n\r", err);
		return DRV_ERR;
	}

	err = VK_MSG_Destroy(pDemux->eventID[P_DEMUX_PsiFilter]);
	if(err != VK_OK )
	{
		PrintError("P_DEMUX_DestorySectionHandling() Error(0x%x) in VK_MSG_Destroy()\n\r", err);
		return DRV_ERR;
	}

	err = VK_SEM_Destroy(sqLock);
	if(err != VK_OK )
	{
		PrintError("P_DEMUX_DestorySectionHandling() Error(0x%x) in VK_SEM_Destroy()\n\r", err);
		return DRV_ERR;
	}

//	BLST_SQ_INITIALIZER(gMsgHead);


	return DRV_OK;
}

static DRV_Error P_DEMUX_PreparePESHandling(void)
{
	P_DEMUX_Module *pDemux;
	HUINT32 err;

	pDemux = P_DEMUX_GetInstance();

	if(VK_MSG_Create(DMX_MSGQ_COUNT, sizeof(P_DEMUX_MSG_Struct),"PesMsg",&pDemux->eventID[P_DEMUX_PesFilter],VK_SUSPENDTYPE_FIFO))
	{
		PrintError(" VK_MSG_Create error\n");
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_DEMUX_PesEventTask,
						DRV_DEMUX_PES_HANDLER_TASK_PRIORITY,
						DRV_DEMUX_PES_HANDLER_TASK_STACK_SIZE,
						"PesTask",
						NULL,
						&pestaskId, FALSE);
	if( err != VK_OK )
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_PesFilter]);
		PrintError(" Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Start(pestaskId);
	if(err != VK_OK)
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_PesFilter]);
		VK_TASK_Destroy(pestaskId);
		PrintError(" Error(0x%x) in VK_TASK_Start()\n\r", err);
		return DRV_ERR;
	}
	return DRV_OK;
}

static DRV_Error P_DEMUX_DestroyPESHandling(void)
{
	P_DEMUX_Module *pDemux = NULL;
	HUINT32 err = 0;

	pDemux = P_DEMUX_GetInstance();
	if(pDemux == NULL)
	{
		PrintError(" P_DEMUX_DestroyPESHandling() return err \n\r");
		return DRV_ERR;
	}

	err = VK_TASK_Stop(pestaskId);
	if(err != VK_OK)
	{
		PrintError(" P_DEMUX_DestroyPESHandling() Error(0x%x) in VK_TASK_Stop()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Destroy(pestaskId);
	if(err != VK_OK)
	{
		PrintError(" P_DEMUX_DestroyPESHandling() Error(0x%x) in VK_TASK_Destroy()\n\r", err);
		return DRV_ERR;
	}

	err = VK_MSG_Destroy(pDemux->eventID[P_DEMUX_PesFilter]);
	if(err != VK_OK)
	{
		PrintError(" P_DEMUX_DestroyPESHandling() Error(0x%x) in VK_MSG_Destroy()\n\r", err);
		return DRV_ERR;
	}

	return DRV_OK;
}


static int P_DEMUX_Logical2Physical(int logicalChannelIdx)
{
	if(logicalChannelIdx == RESERVED_CHANNEL_LIVE1_LOGICAL)
	{
		return RESERVED_CHANNEL_LIVE1_PHYSICAL;
	}
		return -1;
}

static NEXUS_ParserBand P_DEMUX_GetReservedParserBand(int logicalChannelIdx)
{
	if(logicalChannelIdx == RESERVED_CHANNEL_LIVE1_LOGICAL)
		return (NEXUS_ParserBand)(MAX_IBPARSER_BAND_NUM - 1);
	else
		return NEXUS_ParserBand_eInvalid;
}

#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
static void P_DEMUX_InitTscMonModule_2nd(void)
{
	int i;
	HUINT32 err;

	err = VK_SEM_Create(&gTscCheck.Lock, "TSCNew_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		PrintError("VK_SEM_Create() error, DmxSQ_SEM\n");
		return;
	}
	for(i=0; i<NUM_TSC_CHECK_RES; i++)
	{
		gTscCheck.stResource[i].bEnabled = FALSE;
		gTscCheck.stResource[i].bFilterOn= FALSE;
		gTscCheck.stResource[i].pidChannel = 0xFF;
		gTscCheck.stResource[i].filterIndex = 0xFF;
		gTscCheck.stResource[i].TSCValue = 0xFF;
		gTscCheck.stResource[i].timerId = 0;
	}
	return;
}
#endif

static void P_DEMUX_InitTscMonModule(void)
{
	P_DEMUX_Module *pDemux;
	int tscmonCnt;

	pDemux = P_DEMUX_GetInstance();
	for(tscmonCnt=0; tscmonCnt<MAX_TSCMON_MODULE_NUM; tscmonCnt++)
	{
		pDemux->stTscMonitor[tscmonCnt].Status = DRV_DEMUX_RESOURCE_FREE;
	}
}

static DRV_Error P_DEMUX_AllocateTSCMonitor(HUINT32 *pulIndex)
{
	DRV_Error drvError = DRV_OK;
	P_DEMUX_Module *pDemux;
	int tscmonCnt;

	pDemux = P_DEMUX_GetInstance();
	VK_SEM_Get(tscmonLock);
	for(tscmonCnt=0; tscmonCnt<MAX_TSCMON_MODULE_NUM; tscmonCnt++)
	{
		if(pDemux->stTscMonitor[tscmonCnt].Status == DRV_DEMUX_RESOURCE_FREE)
		{
			pDemux->stTscMonitor[tscmonCnt].Status = DRV_DEMUX_RESOURCE_ENABLED;
			break;
		}
	}
	VK_SEM_Release(tscmonLock);
	if(tscmonCnt >= MAX_TSCMON_MODULE_NUM)
	{
		drvError = DRV_ERR;
	}
	*pulIndex = tscmonCnt;

	return drvError;
}

static DRV_Error P_DEMUX_DeallocateTSCMonitor(HUINT32 ulIndex)
{
	DRV_Error drvError = DRV_OK;
	P_DEMUX_Module *pDemux;

	if(ulIndex >= MAX_TSCMON_MODULE_NUM)
	{
		drvError= DRV_ERR_INVALID_PARAMETER;
		return drvError;
	}
	pDemux = P_DEMUX_GetInstance();
	VK_SEM_Get(tscmonLock);
	if(pDemux->stTscMonitor[ulIndex].Status == DRV_DEMUX_RESOURCE_ENABLED)
	{
		pDemux->stTscMonitor[ulIndex].Status = DRV_DEMUX_RESOURCE_FREE;
	}
	else
	{
		drvError = DRV_ERR;
	}
	VK_SEM_Release(tscmonLock);

	return drvError;
}

static DRV_Error P_DEMUX_StartTSCMonitoring(HUINT32 ulPidChannel, int mode)
{
#if 0 //defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	int i;
	P_DEMUX_Module *pDemux;
	DRV_Error drvError = DRV_OK;
	NEXUS_PidChannelHandle nXpidHandle;

	BSTD_UNUSED(mode);
	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(pDemux->pidChannel[ulPidChannel].lock);
	nXpidHandle = pDemux->pidChannel[ulPidChannel].pidHandle;
	VK_SEM_Release(pDemux->pidChannel[ulPidChannel].lock);

	VK_SEM_Get(gTscCheck.Lock);
	for(i=0; i<NUM_TSC_CHECK_RES; i++)
	{
		if(gTscCheck.stResource[i].bEnabled == FALSE)
		{
			gTscCheck.stResource[i].bEnabled = TRUE;
			gTscCheck.stResource[i].pidChannel = ulPidChannel;
			gTscCheck.stResource[i].pidHandle = nXpidHandle;
			gTscCheck.stResource[i].footPrint = (i<<24 |gFootPrint<<16);
			gFootPrint++;
			/*if(gFootPrint > 0xFF)
				gFootPrint = 0;*/
			break;
		}
	}
	if(i==NUM_TSC_CHECK_RES)
	{
		drvError = DRV_ERR;
		PrintError("No more resources\n");
	}
	else
	{
		VK_SEM_Get(pDemux->pidChannel[ulPidChannel].lock);
		pDemux->pidChannel[ulPidChannel].bTSCMonitornigOn = TRUE;
		VK_SEM_Release(pDemux->pidChannel[ulPidChannel].lock);
		if(gTscCheck.stResource[i].bFilterOn == FALSE)
		{
			P_DEMUX_StartTSFilter(i);
			gTscCheck.stResource[i].bFilterOn = TRUE;
		}
	}
	VK_SEM_Release(gTscCheck.Lock);
#else //CONFIG_TSC_CHECK_USING_TS_FILTER
	DRV_Error drvError = DRV_OK;
	P_DEMUX_Module *pDemux;
	HUINT32 ulTscIndex;
	NEXUS_Error rc;
	NEXUS_PidChannelScramblingSettings scSettings;

	pDemux = P_DEMUX_GetInstance();

	drvError = P_DEMUX_AllocateTSCMonitor(&ulTscIndex);
	if(drvError != DRV_OK)
	{
		drvError = DRV_ERR;
	}
	else
	{
		VK_SEM_Get(pDemux->pidChannel[ulPidChannel].lock);
		if(pDemux->pidChannel[ulPidChannel].status == DRV_DEMUX_RESOURCE_ENABLED && pDemux->pidChannel[ulPidChannel].bTSCMonitornigOn == FALSE)
		{
			NEXUS_PidChannel_GetDefaultScramblingSettings(&scSettings);
			rc = NEXUS_PidChannel_StartScramblingCheck(pDemux->pidChannel[ulPidChannel].pidHandle, &scSettings);
			if(rc != 0)
			{
				PrintError("[%s] failed.(%d)\n",__FUNCTION__, rc);
				drvError =  DRV_ERR;
			}
			else
			{
				pDemux->pidChannel[ulPidChannel].bTSCMonitornigOn = TRUE;
				pDemux->pidChannel[ulPidChannel].ulTSCMonitorIndex = ulTscIndex;
			}
		}
		else if(pDemux->pidChannel[ulPidChannel].status == DRV_DEMUX_RESOURCE_ENABLED && pDemux->pidChannel[ulPidChannel].bTSCMonitornigOn == TRUE)
		{
			P_DEMUX_DeallocateTSCMonitor(ulTscIndex);
			PrintError(" failed. TSCMonitor is already on.\n");
			drvError = DRV_ERR;
		}
		else
		{
			P_DEMUX_DeallocateTSCMonitor(ulTscIndex);
			PrintError(" failed. PidChannel not enabled.\n");
			drvError = DRV_ERR;
		}
		VK_SEM_Release(pDemux->pidChannel[ulPidChannel].lock);
	}
#endif
	BSTD_UNUSED(mode);
	return drvError;
}

static DRV_Error P_DEMUX_StopTSCMonitoring(HUINT32 ulPidChannel)
{
#if 0 //defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	int i;
	P_DEMUX_Module *pDemux;
	HBOOL	bStopRequired = FALSE;
	DRV_Error drvError = DRV_OK;

	pDemux = P_DEMUX_GetInstance();

	if(pDemux->pidChannel[ulPidChannel].bTSCMonitornigOn == TRUE)
	{
		pDemux->pidChannel[ulPidChannel].bTSCMonitornigOn = FALSE;
		bStopRequired = TRUE;
	}

	if(bStopRequired != FALSE)
	{
		VK_SEM_Get(gTscCheck.Lock);
		for(i=0; i<NUM_TSC_CHECK_RES; i++)
		{
			if(gTscCheck.stResource[i].bEnabled == TRUE && gTscCheck.stResource[i].pidChannel == ulPidChannel)
			{
				gTscCheck.stResource[i].bEnabled = FALSE;
				gTscCheck.stResource[i].pidChannel = 0xFF;
				if(gTscCheck.stResource[i].bFilterOn != FALSE)
				{
					P_DEMUX_StopTSFilter(i);
					gTscCheck.stResource[i].bFilterOn = FALSE;
				}
				break;
			}
		}
		VK_SEM_Release(gTscCheck.Lock);
		if(i==NUM_TSC_CHECK_RES)
		{
			drvError = DRV_ERR;
			PrintError("Slot not found.\n");
		}
	}
#else
	DRV_Error drvError = DRV_OK;
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();
	VK_SEM_Get(pDemux->pidChannel[ulPidChannel].lock);
	if(pDemux->pidChannel[ulPidChannel].bTSCMonitornigOn == TRUE && pDemux->pidChannel[ulPidChannel].status == DRV_DEMUX_RESOURCE_ENABLED)
	{
		NEXUS_PidChannel_StopScramblingCheck(pDemux->pidChannel[ulPidChannel].pidHandle);
		P_DEMUX_DeallocateTSCMonitor(pDemux->pidChannel[ulPidChannel].ulTSCMonitorIndex);
		pDemux->pidChannel[ulPidChannel].bTSCMonitornigOn = FALSE;
	}
	else
	{
		PrintDebug("[%s] failed. TSCMonitor not started.\n",__FUNCTION__);
		drvError = DRV_ERR;
	}
	VK_SEM_Release(pDemux->pidChannel[ulPidChannel].lock);
#endif
	return drvError;
}

static DRV_Error P_DEMUX_GetTSCStatus(HUINT32 ulPidChannel,DI_DEMUX_TsState_t *peTsState)
{
	DRV_Error drvError = DRV_OK;
	P_DEMUX_Module *pDemux;
	NEXUS_Error rc;
	NEXUS_PidChannelScramblingStatus scStatus;

	pDemux = P_DEMUX_GetInstance();
	VK_SEM_Get(pDemux->pidChannel[ulPidChannel].lock);
	if(pDemux->pidChannel[ulPidChannel].bTSCMonitornigOn == TRUE && pDemux->pidChannel[ulPidChannel].status == DRV_DEMUX_RESOURCE_ENABLED)
	{
		rc = NEXUS_PidChannel_GetScramblingStatus(pDemux->pidChannel[ulPidChannel].pidHandle, &scStatus);
		if(rc != 0)
		{
			DI_UART_Print("[%s] failed.(%d)\n",__FUNCTION__, rc);
			drvError = DRV_ERR;
		}
		else
		{
			if(scStatus.pidExists == 0)
			{
				*peTsState = eDI_DMX_TS_UnKnown;
			}
			else if(scStatus.scrambled == true)
			{
				*peTsState = eDI_DMX_TS_Scramble;
			}
			else if(scStatus.scrambled == false)
			{
				*peTsState = eDI_DMX_TS_Clean;
			}
		}
		if(scStatus.scrambled == true)
		{
			NEXUS_PidChannel_ClearScramblingStatus(pDemux->pidChannel[ulPidChannel].pidHandle);
		}
	}
	VK_SEM_Release(pDemux->pidChannel[ulPidChannel].lock);

	return drvError;
}

static DRV_Error P_DEMUX_AllocateChannel(DI_DEMUX_CHANNELTYPE channelType, HUINT16 pid, HUINT32 *channelIndex)
{
	P_DEMUX_Module *pDemux;
	HUINT8 channel_index;

	if(channelIndex == NULL)
	{
		PrintError(" invalid parameter\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	for(channel_index = 0; channel_index < DRV_DMX_NUM_PID_CHANNEL; channel_index++)
	{
		VK_SEM_Get(pDemux->pidChannel[channel_index].lock);
		if(pDemux->pidChannel[channel_index].status == DRV_DEMUX_RESOURCE_FREE)
		{
			pDemux->pidChannel[channel_index].status = DRV_DEMUX_RESOURCE_ALLOCATED;
			pDemux->pidChannel[channel_index].channelType = channelType;
			pDemux->pidChannel[channel_index].pid = pid;
			VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
			*channelIndex = channel_index;
			PrintDebug("P_DEMUX_AllocateChannel(), channel %d is allocated\n", channel_index);
			return DRV_OK;
		}
		VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
	}

	PrintError(" no more pid channel to allocate\n");

	*channelIndex = DI_DEMUX_INVALID_CHANNEL_ID;
	return DRV_ERR;
}

static DRV_Error P_DEMUX_DeallocateChannel(HUINT32 channelIndex)
{
	P_DEMUX_Module *pDemux;

	if(channelIndex >= DRV_DMX_NUM_PID_CHANNEL)
	{
		PrintError(" invalid parameter (chIdx = %ld)\n",channelIndex);
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(pDemux->pidChannel[channelIndex].lock);
	if(pDemux->pidChannel[channelIndex].status != DRV_DEMUX_RESOURCE_ALLOCATED)
	{
		VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
		PrintError(" trying to deallocate free or enabled channel\n");
		return DRV_ERR;
	}
	pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_FREE;
	VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);

	return DRV_OK;
}

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
static DRV_Error P_DEMUX_AllocateStcChannel(HUINT32 *pStcChannelIndex)
{
	P_DEMUX_Module *pDemux;
	HUINT8 deviceIndex, stcChannelIndex;

	PrintEnter();

	if(pStcChannelIndex == NULL)
	{
		PrintError(" invalid parameter (chIdx = %ld)\n", pStcChannelIndex);
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	for(stcChannelIndex=0; stcChannelIndex<MAX_TIMEBASE_NUM; stcChannelIndex++)
	{
		VK_SEM_Get(pDemux->stcChannel[stcChannelIndex].lock);
		if(pDemux->stcChannel[stcChannelIndex].bStcChannelIsAllocated == FALSE)
		{
			pDemux->stcChannel[stcChannelIndex].bStcChannelIsAllocated = TRUE;
			VK_SEM_Release(pDemux->stcChannel[stcChannelIndex].lock);

			for(deviceIndex=0; deviceIndex<MAX_DEMUX_NUM; deviceIndex++)
			{
				VK_SEM_Get(pDemux->dmxDevice[deviceIndex].lock);
				pDemux->dmxDevice[deviceIndex].stCapability.NumOfAvailableStcChannel--;
				VK_SEM_Release(pDemux->dmxDevice[deviceIndex].lock);
				PrintDebug("[%s] number of available STC channel: (%d).\n", __func__, pDemux->dmxDevice[deviceIndex].stCapability.NumOfAvailableStcChannel);
			}

			*pStcChannelIndex = stcChannelIndex;
			PrintDebug("P_DEMUX_AllocateStcChannel(), STC channel %d is allocated.\n", stcChannelIndex);

			PrintExit();
			return DRV_OK;
		}
		VK_SEM_Release(pDemux->stcChannel[stcChannelIndex].lock);
	}

	PrintError(" no more STC channel is available!\n");

	*pStcChannelIndex = DI_DEMUX_INVALID_CHANNEL_ID;
	return DRV_ERR;
}

static DRV_Error P_DEMUX_DeallocateStcChannel(HUINT32 ulStcChannelIndex)
{
	P_DEMUX_Module *pDemux;
	HUINT8 deviceIndex;
	NEXUS_Error rc;
	NEXUS_StcChannelSettings stcSettings;

	PrintEnter();

	if(ulStcChannelIndex >= MAX_TIMEBASE_NUM)
	{
		PrintError(" Unknown STC channel index. (%d)\n", ulStcChannelIndex);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(pDemux->stcChannel[ulStcChannelIndex].lock);
	if(pDemux->stcChannel[ulStcChannelIndex].bStcChannelIsAllocated != TRUE)
	{
		VK_SEM_Release(pDemux->stcChannel[ulStcChannelIndex].lock);
		PrintError(" trying to deallocate free or enabled STC channel.\n");
		return DRV_ERR;
	}

	NEXUS_StcChannel_GetDefaultSettings(ulStcChannelIndex, &stcSettings);
	rc = NEXUS_StcChannel_SetSettings(pDemux->stcChannel[ulStcChannelIndex].stcHandle, &stcSettings);
	if(rc != 0)
	{
		VK_SEM_Release(pDemux->stcChannel[ulStcChannelIndex].lock);
		PrintError(" failed disconnecting PCR channel to STC.\n");
		return DRV_ERR;
	}

	pDemux->stcChannel[ulStcChannelIndex].bStcChannelIsAllocated = FALSE;
	VK_SEM_Release(pDemux->stcChannel[ulStcChannelIndex].lock);
	PrintDebug("[%s] StcChannelIndex(%d) is disabled!\n", __func__, ulStcChannelIndex);

	for(deviceIndex=0; deviceIndex<MAX_DEMUX_NUM; deviceIndex++)
	{
		VK_SEM_Get(pDemux->dmxDevice[deviceIndex].lock);
		pDemux->dmxDevice[deviceIndex].stCapability.NumOfAvailableStcChannel++;
		VK_SEM_Release(pDemux->dmxDevice[deviceIndex].lock);
		PrintDebug("[%s] number of available STC channel: (%d).\n", __func__, pDemux->dmxDevice[deviceIndex].stCapability.NumOfAvailableStcChannel);
	}

	PrintExit();

	return DRV_OK;
}

static DRV_Error P_DEMUX_GetStcChannelIndex(NEXUS_StcChannelHandle hStcHandle, HUINT32 *pStcChannelIndex)
{
	P_DEMUX_Module *pDemux;
	HUINT8 stcChannelIndex;

	PrintEnter();

	if((hStcHandle == NULL) || (pStcChannelIndex == NULL))
	{
		PrintError("[%s] Invalid Parameter: hStcHandle or pStcChannelIndex is NULL!\n", __func__);
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	for(stcChannelIndex=0; stcChannelIndex<MAX_TIMEBASE_NUM; stcChannelIndex++)
	{
		VK_SEM_Get(pDemux->stcChannel[stcChannelIndex].lock);
		if(hStcHandle == pDemux->stcChannel[stcChannelIndex].stcHandle)
		{
			*pStcChannelIndex = stcChannelIndex;
			VK_SEM_Release(pDemux->stcChannel[stcChannelIndex].lock);

			PrintDebug("[%s] stcChannel Index is (%d)!\n", __func__, stcChannelIndex);

			PrintExit();
			return DRV_OK;
		}
		VK_SEM_Release(pDemux->stcChannel[stcChannelIndex].lock);
	}

	PrintError(" no STC channel index is available!\n");

	PrintExit();
	return DRV_ERR;
}
#endif

static DRV_Error P_DEMUX_EnableChannel(DI_DEMUX_CHANNELTYPE channelType, HUINT32 demuxId, HUINT32 channelIndex, P_DEMUX_ChannelSubType eChSubtype)
{
	DRV_Error drvErr=DRV_OK;
	NEXUS_Error rc;
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBand parserBand;
	NEXUS_ParserBandSettings pb_settings;
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_PidChannelSettings pidChSettings;
	DRV_DEMUX_CHANNELTYPE drvChannelType = 0;
	DRV_Error drvRet = 0;
	HUINT32 stcChannelIndex = 0;
	HUINT32 index = 0;

	HAPPY(stcChannelIndex);
	HAPPY(index);
	HAPPY(drvRet);
	HAPPY(drvChannelType);

	parserBand = P_DEMUX_GetParserBandByDemuxId(demuxId);

	if(parserBand == NEXUS_ParserBand_eInvalid || channelIndex >= DRV_DMX_NUM_PID_CHANNEL)
	{
		PrintError(" invalid parameter\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();
	VK_SEM_Get(pDemux->pidChannel[channelIndex].lock);
	if(pDemux->pidChannel[channelIndex].status != DRV_DEMUX_RESOURCE_ALLOCATED)
	{
		VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
		PrintError(" pid channel %d is not allocated or already enabled\n", channelIndex);
		return DRV_ERR;
	}
	pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;

	if(parserBand >= NEXUS_ParserBand_ePb0) // for playback
	{
#if defined(CONFIG_SI_PARSING_FOR_MEDIA_PLAYBACK)
		if(pDemux->dmxDevice[demuxId].pathInfo.etProducer == DI_DEMUX_INPUT_MEDIA_PLAYER)
		{
#if defined(CONFIG_MEDIA)
			if(DRV_MEDIA_CheckStart() == TRUE)
			{
				drvErr = DRV_MEDIA_OpenPlaybackPidChannel(demuxId, channelType, pDemux->pidChannel[channelIndex].pid,
											&(pDemux->pidChannel[channelIndex].pidHandle));
				if(drvErr == DRV_OK)
				{
					pDemux->pidChannel[channelIndex].bFromMedia = TRUE;
				}
			}
#endif
#if defined(CONFIG_CALYPSO)
			if(DRV_CSO_CheckStart() == TRUE)
			{
				drvErr = DRV_CSO_OpenPlaypumpPidChannel(demuxId, channelType, pDemux->pidChannel[channelIndex].pid,
											&(pDemux->pidChannel[channelIndex].pidHandle));
				if(drvErr == DRV_OK)
				{
					pDemux->pidChannel[channelIndex].bFromMedia = TRUE;
				}
			}
#endif
		}
		else
		{
#if defined(CONFIG_TRANSCODER)
			if(pDemux->dmxDevice[demuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
			{
				drvRet = P_DEMUX_ConvertDItoDRVChannelType(channelType,&drvChannelType);
				if(drvRet != DRV_OK)
				{
					pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
					VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
					PrintError(" channel type invalid parmeter (%d)\n", channelIndex);
					return DRV_ERR;
				}
				drvErr = DRV_PVR_PLAY_PidChannelOpenWithDemuxId(demuxId, drvChannelType, pDemux->pidChannel[channelIndex].pid,
										&(pDemux->pidChannel[channelIndex].pidHandle));
			}
			else
#endif
#if defined(CONFIG_PVR)
			{
				drvRet = P_DEMUX_ConvertDItoDRVChannelType(channelType,&drvChannelType);
				if(drvRet != DRV_OK)
				{
					pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
					VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
					PrintError(" channel type invalid parmeter (%d)\n", channelIndex);
					return DRV_ERR;
				}

				drvErr = DRV_PVR_PLAY_PidChannelOpen(parserBand-NEXUS_ParserBand_ePb0, drvChannelType, pDemux->pidChannel[channelIndex].pid,
										&(pDemux->pidChannel[channelIndex].pidHandle));
			}
#endif
		}
#else
#if defined(CONFIG_TRANSCODER)
		if(pDemux->dmxDevice[demuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
		{
			drvRet = P_DEMUX_ConvertDItoDRVChannelType(channelType,&drvChannelType);
			if(drvRet != DRV_OK)
			{
				pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" channel type invalid parmeter (%d)\n", channelIndex);
				return DRV_ERR;
			}
			drvErr = DRV_PVR_PLAY_PidChannelOpenWithDemuxId(demuxId, drvChannelType, pDemux->pidChannel[channelIndex].pid,
									&(pDemux->pidChannel[channelIndex].pidHandle));
		}
		else
#endif
#if defined(CONFIG_PVR)
		{
			drvRet = P_DEMUX_ConvertDItoDRVChannelType(channelType,&drvChannelType);
			if(drvRet != DRV_OK)
			{
				pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" invalid channel type (%d)\n", channelIndex);
				return DRV_ERR;
			}

			drvErr = DRV_PVR_PLAY_PidChannelOpen(parserBand-NEXUS_ParserBand_ePb0, drvChannelType, pDemux->pidChannel[channelIndex].pid,
									&(pDemux->pidChannel[channelIndex].pidHandle));
		}
#endif
#endif
		if(drvErr != DRV_OK)
		{
			pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
			VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
			PrintError(" pid channel %d is not allocated or already enabled\n", channelIndex);
			return DRV_ERR;
		}
	}
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
	else if(parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback()) //for playback routing
	{
		NEXUS_PidChannel_GetDefaultSettings(&pidChSettings);
		if(P_DEMUX_CheckIfFilterBeAttached(channelType))
		{
			pidChSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;
		}
		else
		{
			pidChSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE;
		}
		pDemux->pidChannel[channelIndex].pidHandle = NEXUS_PidChannel_Open(parserBand,
													pDemux->pidChannel[channelIndex].pid, &pidChSettings);
		//PrintError("Enable Channel: %d\n", pDemux->pidChannel[channelIndex].pidHandle);
		if(pDemux->pidChannel[channelIndex].pidHandle == NULL)
		{
			pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
			VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
			PrintError(" pid channel %d is not allocated or already enabled\n", channelIndex);
			return DRV_ERR;
		}

#if defined(CONFIG_PVR)
		if(pDemux->pidChannel[channelIndex].channelType <= DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF)
		{
			drvRet = P_DEMUX_ConvertDItoDRVChannelType(channelType,&drvChannelType);
			if(drvRet != DRV_OK)
			{
				pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" channel type invalid parmeter (%d)\n", channelIndex);
				return DRV_ERR;
			}

			drvErr = DRV_PVR_PLAY_PidChannelOpenDummy(0, drvChannelType, pDemux->pidChannel[channelIndex].pid,
																	pDemux->pidChannel[channelIndex].pidHandle);
			if(drvErr != DRV_OK)
			{
				pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" pid channel %d is not allocated or already enabled\n", channelIndex);
				return DRV_ERR;
			}
		}
#endif
	}
#endif
	else
	{
	   	if(channelType == DI_DEMUX_CHANNELTYPE_ALLPASS)
		{

			NEXUS_ParserBand_GetSettings(parserBand, &pb_settings);
			pb_settings.allPass = TRUE;
			pb_settings.acceptNullPackets = TRUE;

			rc = NEXUS_ParserBand_SetSettings(parserBand, &pb_settings);
			if(rc != NEXUS_SUCCESS)
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" ParserBand[%d] All-Pass error.\n", parserBand);
				return DRV_ERR;
			}

			NEXUS_PidChannel_GetDefaultSettings(&pidChSettings);
			pidChSettings.pidChannelIndex = parserBand;
			pidChSettings.requireMessageBuffer = FALSE;
			pDemux->pidChannel[channelIndex].pidHandle = NEXUS_PidChannel_Open(parserBand,
													   0x0, &pidChSettings);
			if(pDemux->pidChannel[channelIndex].pidHandle == NULL)
			{
			   pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
			   VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
			   PrintError(" pid channel %d is not allocated or already enabled\n", channelIndex);
			   return DRV_ERR;
			}
		}
		else
		{
			NEXUS_PidChannel_GetDefaultSettings(&pidChSettings);
			if(P_DEMUX_CheckIfFilterBeAttached(channelType))
			{
			   pidChSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;
			}
			else
			{
			   pidChSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE;
			}
			pDemux->pidChannel[channelIndex].pidHandle = NEXUS_PidChannel_Open(parserBand,
													   pDemux->pidChannel[channelIndex].pid, &pidChSettings);
			if(pDemux->pidChannel[channelIndex].pidHandle == NULL)
			{
			   pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
			   VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
			   PrintError(" pid channel %d is not allocated or already enabled\n", channelIndex);
			   return DRV_ERR;
			}
		}
	}

	pDemux->pidChannel[channelIndex].demuxId = demuxId;
	pDemux->pidChannel[channelIndex].parserBand = parserBand;
	pDemux->pidChannel[channelIndex].channelsubInfo = eChSubtype;

	switch(channelType)
	{
		case DI_DEMUX_CHANNELTYPE_VIDEO:
#if defined(PRINT_DMX_AV_CALLSEQUENCE)
				if(SectionDbgMode & 0x40000000)
				{
					DI_UART_Print("[START VIDEO PID]  DemuxID = %d PID = 0x%lx  PID_Channel = 0x%x\n", demuxId, pDemux->pidChannel[channelIndex].pid, channelIndex);
				}
#endif

				g_VideoPid = pDemux->pidChannel[channelIndex].pid;
				s_hVideoPIDChannel = pDemux->pidChannel[channelIndex].pidHandle;
				break;
		case DI_DEMUX_CHANNELTYPE_AUDIO:
#if defined(PRINT_DMX_AV_CALLSEQUENCE)
				if(SectionDbgMode & 0x40000000)
				{
					DI_UART_Print("[START AUDIO PID]  DemuxID = %d PID = 0x%lx  PID_Channel = 0x%x\n", demuxId, pDemux->pidChannel[channelIndex].pid, channelIndex);
				}
#endif
				s_hAudioPIDChannel = pDemux->pidChannel[channelIndex].pidHandle;
				break;

		case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF:
				break;

		case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION:
#if defined(PRINT_DMX_AV_CALLSEQUENCE)
				if(SectionDbgMode & 0x40000000)
				{
					DI_UART_Print("[START AUDIO DESC PID]  DemuxID = %d PID = 0x%lx  PID_Channel = 0x%x\n", demuxId, pDemux->pidChannel[channelIndex].pid, channelIndex);
				}
#endif		
				s_hDescriptionPIDChannel = pDemux->pidChannel[channelIndex].pidHandle;
				break;
		case DI_DEMUX_CHANNELTYPE_PCR:
#if defined(PRINT_DMX_AV_CALLSEQUENCE)
				if(SectionDbgMode & 0x40000000)
				{
					DI_UART_Print("[START PCR PID]  DemuxID = %d PID = 0x%lx  PID_Channel = 0x%x\n", demuxId, pDemux->pidChannel[channelIndex].pid, channelIndex);
				}
#endif
				NEXUS_StcChannel_GetDefaultSettings(DEF_STC_CHANNEL_ID, &stcSettings);
				if(parserBand >= NEXUS_ParserBand_ePb0
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
					|| parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback()
#endif
					) // for playback or playback routing
				{
					stcSettings.timebase = NEXUS_Timebase_e0;
					stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
       				stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
#if defined(CONFIG_TEMP_YSR2000)
					/* for nordig certification*/
					stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eFirstAvailable;
#endif
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
					if(parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback())
					{
#if defined(CONFIG_TEMP_HGS1000S)
						/* HGS1000S no more need to set audio master for CI playback. Instead of that HGS1000S uses 'FirstAvailable'.*/
						stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eFirstAvailable;
#else
						stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eAudioMaster;
#endif
					}
#endif
				}
				else
				{
					stcSettings.timebase = NEXUS_Timebase_e0;
					stcSettings.mode = NEXUS_StcChannelMode_ePcr;
					stcSettings.modeSettings.pcr.pidChannel = pDemux->pidChannel[channelIndex].pidHandle;
				}
				rc = NEXUS_StcChannel_SetSettings(shstcChannel, &stcSettings);
				if(rc != 0)
				{
					VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
					PrintError(" failed connecting PCR channel to STC\n");
					return DRV_ERR;
				}
#if defined(CONFIG_PVR)
				if(parserBand >= NEXUS_ParserBand_ePb0
#if defined(CONFIG_CI)
					|| parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback()
#endif
					) // for playback or playback routing
				{
#if defined(CONFIG_DI10)
					drvErr = DRV_PVR_PLAY_SetStcChannelHandle( 0/*parserBand-NEXUS_ParserBand_ePb0*/, shstcChannel);
#else
	#if defined(CONFIG_CI)
					if(parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback())
					{
						drvErr = DRV_PVR_PLAY_SetStcChannelHandle( 0/*parserBand-NEXUS_ParserBand_ePb0*/, shstcChannel);
					}
					else
	#endif
					{
						drvErr = DRV_PVR_PLAY_SetStcChannelHandle( parserBand-NEXUS_ParserBand_ePb0, shstcChannel);
					}
#endif
					if(drvErr != DRV_OK)
					{
						/*playback error 발생 시(ex: trick중 stc설정은 에러발생) demux 동작에 영향없도록 수정.*/
						//VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
						PrintError(" stc channel set fail\n");
						//return DRV_ERR;
					}
				}
#endif
				s_hPcrPIDChannel = pDemux->pidChannel[channelIndex].pidHandle;
				break;
#if defined(CONFIG_TRANSCODER)
		case DI_DEMUX_CHANNELTYPE_PCR_TRANS_1:
		case DI_DEMUX_CHANNELTYPE_PCR_TRANS_2:
#if defined(PRINT_DMX_AV_CALLSEQUENCE)
				if(SectionDbgMode & 0x40000000)
				{
					DI_UART_Print("[START PCR PID][Transcode]  DemuxID = %d PID = 0x%lx  PID_Channel = 0x%x\n", demuxId, pDemux->pidChannel[channelIndex].pid, channelIndex);
				}
#endif


#if defined(CONFIG_FAST_CHANNEL_CHANGE)
				if(channelType == DI_DEMUX_CHANNELTYPE_PCR_TRANS_1)
				{
					index = P_DEMUX_TRANSCODER_ID_1;
				}
				else if(channelType == DI_DEMUX_CHANNELTYPE_PCR_TRANS_2)
				{
					index = P_DEMUX_TRANSCODER_ID_2;
				}
				else
				{
					VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
					PrintError(" Invalid parameter : channelType(%d) is not correct!\n", channelType);
					return DRV_ERR_INVALID_PARAMETER;
				}

				if(s_subhstcChannel[index] == NULL)
				{
					drvErr = P_DEMUX_AllocateStcChannel(&stcChannelIndex);
					if(drvErr != DRV_OK)
					{
						VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
						PrintError(" stc channel allocation is failed!\n");
						return DRV_ERR;
					}

					s_subhstcChannel[index] = pDemux->stcChannel[stcChannelIndex].stcHandle;
					PrintDebug("[%s] stcChannelIndex(%d) allocates to s_subhstcChannel[%d](0x%x)!\n", __func__, stcChannelIndex, index, s_subhstcChannel[index]);
				}

				if(pDemux->dmxDevice[demuxId].pathInfo.ulConsumerGroupId >= NUM_TRANSCODE_STC)
				{
					PrintError("transcode Path info is out of range\n");
				}

				if(pDemux->dmxDevice[demuxId].pathInfo.ulConsumerGroupId == 0)
				{
					NEXUS_StcChannel_GetDefaultSettings(stcChannelIndex, &stcSettings);
					stcSettings.timebase = NEXUS_Timebase_e0 + stcChannelIndex;
				}
				else
				{
					NEXUS_StcChannel_GetDefaultSettings(stcChannelIndex, &stcSettings);
					stcSettings.timebase = NEXUS_Timebase_e0 + stcChannelIndex;
				}

				if(parserBand >= NEXUS_ParserBand_ePb0
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
					|| parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback()
#endif
					) // for playback or playback routing
				{
					stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
    				stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
					if(parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback())
					{
						stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eAudioMaster;
					}
#endif
				}
				else
				{
					stcSettings.mode = NEXUS_StcChannelMode_ePcr;
					stcSettings.modeSettings.pcr.pidChannel = pDemux->pidChannel[channelIndex].pidHandle;
				}

				if(s_stcChannelTranscode[index] == NULL)
				{
					drvErr = P_DEMUX_AllocateStcChannel(&stcChannelIndex);
					if(drvErr != DRV_OK)
					{
						VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
						PrintError(" stc channel allocation is failed!\n");
						return DRV_ERR;
					}
					s_stcChannelTranscode[index] = pDemux->stcChannel[stcChannelIndex].stcHandle;
					PrintDebug("[%s] stcChannelIndex(%d) allocates to s_stcChannelTranscode[%d](0x%x)!\n", __func__, stcChannelIndex, index, s_stcChannelTranscode[index]);

					NEXUS_StcChannel_GetDefaultSettings(stcChannelIndex, &stcSettings);
					stcSettings.timebase = NEXUS_Timebase_e0 + stcChannelIndex;/* should be the same timebase for end-to-end locking */
					stcSettings.mode = NEXUS_StcChannelMode_eAuto;
					stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
					rc = NEXUS_StcChannel_SetSettings(s_stcChannelTranscode[index], &stcSettings);
					if(rc != 0)
					{
						VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
						PrintError(" failed connecting PCR channel to STC\n");
						return DRV_ERR;
					}
				}
#else
				if(pDemux->dmxDevice[demuxId].pathInfo.ulConsumerGroupId >= NUM_TRANSCODE_STC)
				{
					PrintError("transcode Path info is out of range\n");
				}

				if(pDemux->dmxDevice[demuxId].pathInfo.ulConsumerGroupId == 0)
				{
					NEXUS_StcChannel_GetDefaultSettings( DEF_STC_CHANNEL_ID + 1, &stcSettings);
					stcSettings.timebase = NEXUS_Timebase_e0 + 1;

				}
				else
				{

					NEXUS_StcChannel_GetDefaultSettings( DEF_STC_CHANNEL_ID + 3, &stcSettings);
					stcSettings.timebase = NEXUS_Timebase_e0 + 3;
				}

				if(parserBand >= NEXUS_ParserBand_ePb0
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
					|| parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback()
#endif
					) // for playback or playback routing
				{
					stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
        			stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
					if(parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback())
					{
						stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eAudioMaster;
					}
#endif
				}
				else
				{
					stcSettings.mode = NEXUS_StcChannelMode_ePcr;
					stcSettings.modeSettings.pcr.pidChannel = pDemux->pidChannel[channelIndex].pidHandle;
				}
#endif
				if(pDemux->dmxDevice[demuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
				{
					rc = NEXUS_StcChannel_SetSettings(s_subhstcChannel[pDemux->dmxDevice[demuxId].pathInfo.ulConsumerGroupId], &stcSettings);
					if(rc != 0)
					{
						VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
						PrintError(" failed connecting PCR channel to STC\n");
						return DRV_ERR;
					}
					if(parserBand >= NEXUS_ParserBand_ePb0
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
						|| parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback()
#endif
						) // for playback or playback routing
					{
						rc = NEXUS_StcChannel_SetSettings(s_subhstcChannel[pDemux->dmxDevice[demuxId].pathInfo.ulConsumerGroupId], &stcSettings);
						drvErr = DRV_PVR_PLAY_SetStcChannelHandleWithDemuxId(demuxId, s_subhstcChannel[pDemux->dmxDevice[demuxId].pathInfo.ulConsumerGroupId]);
						if(drvErr != DRV_OK)
						{
							/*playback error 발생 시(ex: trick중 stc설정은 에러발생) demux 동작에 영향없도록 수정.*/
							//VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
							PrintError(" stc channel set fail\n");
							//return DRV_ERR;
						}
					}
				}
				else
				{
					VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
					PrintError("PCR_TRANS_x types only allowed for Transcoding.\n");
					return DRV_ERR;
				}

				break;
#endif
#if defined(CONFIG_PIP)
				/* PIP 를 위한 추가 */
				case DI_DEMUX_CHANNELTYPE_VIDEO_SUB:
					s_hPIPVideoPIDChannel = pDemux->pidChannel[channelIndex].pidHandle;
				break;

				case DI_DEMUX_CHANNELTYPE_PCR_SUB:
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
					if(shstcChannel_o == NULL)
					{
						drvErr = P_DEMUX_AllocateStcChannel(&stcChannelIndex);
						if(drvErr != DRV_OK)
						{
							VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
							PrintError(" stc channel allocation is failed!\n");
							return DRV_ERR;
						}
						shstcChannel_o = pDemux->stcChannel[stcChannelIndex].stcHandle;
						PrintDebug("[%s] stcChannelIndex(%d) allocates to shstcChannel_o(0x%x)!\n", __func__, stcChannelIndex, shstcChannel_o);
					}
					NEXUS_StcChannel_GetDefaultSettings(stcChannelIndex, &stcSettings);
					stcSettings.timebase = NEXUS_Timebase_e0 + stcChannelIndex;
#else
					NEXUS_StcChannel_GetDefaultSettings(DEF_STC_CHANNEL_ID + 1, &stcSettings);
					stcSettings.timebase = NEXUS_Timebase_e1;
#endif
					stcSettings.mode = NEXUS_StcChannelMode_ePcr;
					stcSettings.modeSettings.pcr.pidChannel = pDemux->pidChannel[channelIndex].pidHandle;

					rc = NEXUS_StcChannel_SetSettings(shstcChannel_o, &stcSettings);
					if(rc != 0)
					{
						VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
						PrintError(" failed connecting PCR channel to STC\n");
						return DRV_ERR;
					}

					s_hPIPPcrPIDChannel = pDemux->pidChannel[channelIndex].pidHandle;
				break;
#endif
#if defined(PRINT_DMX_AV_CALLSEQUENCE)
		case DI_DEMUX_CHANNELTYPE_RECORD:
				if(SectionDbgMode & 0x40000000)
				{
					DI_UART_Print("[START RECORD PID]  DemuxID = %d PID = 0x%lx  PID_Channel = 0x%x\n", demuxId, pDemux->pidChannel[channelIndex].pid, channelIndex);
				}
				break;
#endif
		default:
				break;
	}
	VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);

	return DRV_OK;
}

static DRV_Error P_DEMUX_DisableChannel(HUINT32 channelIndex)
{
	DRV_Error drvErr=DRV_OK;
	NEXUS_Error rc;
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBand parserBand;
	NEXUS_StcChannelSettings stcSettings;
	HUINT32	demuxId = 0;
	DI_DEMUX_CHANNELTYPE channelType = DI_DEMUX_CHANNELTYPE_MAX;

	DRV_DEMUX_CHANNELTYPE drvChannelType = DRV_DEMUX_CHANNELTYPE_MAX;
	DRV_Error drvRet = 0;
	HUINT32 stcChannelIndex = 0;
	HUINT32 index;

	HAPPY(stcChannelIndex);
	HAPPY(index);
	HAPPY(drvRet);
	HAPPY(drvChannelType);

	if(channelIndex >= DRV_DMX_NUM_PID_CHANNEL)
	{
		PrintError("channelIndex %d is invalid\n", channelIndex);
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	demuxId = pDemux->pidChannel[channelIndex].demuxId;
#if defined(CONFIG_PARSERBAND_OPTIMIZE)
	/* release using by latest set parserBand
	    because some time this func is called after release path */
	parserBand = pDemux->pidChannel[channelIndex].parserBand;
#else
	parserBand = P_DEMUX_GetParserBandByDemuxId(demuxId);
#endif
	if(parserBand == NEXUS_ParserBand_eInvalid)
	{
		PrintError("Invalid parameter\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_SEM_Get(pDemux->pidChannel[channelIndex].lock);
	if(pDemux->pidChannel[channelIndex].status != DRV_DEMUX_RESOURCE_ENABLED)
	{
		VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
		PrintError("trying to disable channel which hasn't been enabled\n");
		return DRV_ERR;
	}

	if(pDemux->pidChannel[channelIndex].bTSCMonitornigOn == TRUE)
	{
#if 0 //defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
		P_DEMUX_StopTSCMonitoring(channelIndex);
#else
		NEXUS_PidChannel_StopScramblingCheck(pDemux->pidChannel[channelIndex].pidHandle);
		P_DEMUX_DeallocateTSCMonitor(pDemux->pidChannel[channelIndex].ulTSCMonitorIndex);
#endif
		pDemux->pidChannel[channelIndex].bTSCMonitornigOn = FALSE;
	}

	pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;

	if(parserBand >= NEXUS_ParserBand_ePb0) // for playback
	{
#if defined(CONFIG_SI_PARSING_FOR_MEDIA_PLAYBACK)
		if(pDemux->pidChannel[channelIndex].bFromMedia == TRUE)
		{
			pDemux->pidChannel[channelIndex].bFromMedia = FALSE;
#if defined(CONFIG_MEDIA)
			if(DRV_MEDIA_CheckStart() == TRUE)
			{
				drvErr = DRV_MEDIA_ClosePlaybackPidChannel(pDemux->pidChannel[channelIndex].demuxId, pDemux->pidChannel[channelIndex].channelType, pDemux->pidChannel[channelIndex].pidHandle);
			}
#endif
#if defined(CONFIG_CALYPSO)
			if(DRV_CSO_CheckStart() == TRUE)
			{
				drvErr = DRV_CSO_ClosePlaypumpPidChannel(pDemux->pidChannel[channelIndex].demuxId, pDemux->pidChannel[channelIndex].channelType, pDemux->pidChannel[channelIndex].pidHandle);
			}
#endif
		}
		else
		{
#if defined(CONFIG_TRANSCODER)
			if(pDemux->dmxDevice[demuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
			{
				drvRet = P_DEMUX_ConvertDItoDRVChannelType(pDemux->pidChannel[channelIndex].channelType,&drvChannelType);
				if(drvRet != DRV_OK)
				{
					pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
					VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
					PrintError(" invalid parameter\n");
					return DRV_ERR;
				}

				drvErr = DRV_PVR_PLAY_PidChannelCloseWithDemuxId(demuxId, pDemux->pidChannel[channelIndex].pidHandle, drvChannelType);
			}
			else
#endif
#if defined(CONFIG_PVR)
			{
				drvRet = P_DEMUX_ConvertDItoDRVChannelType(pDemux->pidChannel[channelIndex].channelType,&drvChannelType);
				if(drvRet != DRV_OK)
				{
					pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
					VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
					PrintError(" invalid parameter\n");
					return DRV_ERR;
				}

				drvErr = DRV_PVR_PLAY_PidChannelClose(parserBand-NEXUS_ParserBand_ePb0, pDemux->pidChannel[channelIndex].pidHandle, drvChannelType);
			}
#endif
		}
#else
#if defined(CONFIG_TRANSCODER)
		if(pDemux->dmxDevice[demuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
		{
			drvRet = P_DEMUX_ConvertDItoDRVChannelType(pDemux->pidChannel[channelIndex].channelType,&drvChannelType);
			if(drvRet != DRV_OK)
			{
				pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" invalid parameter\n");
				return DRV_ERR;
			}

			drvErr = DRV_PVR_PLAY_PidChannelCloseWithDemuxId(demuxId, pDemux->pidChannel[channelIndex].pidHandle,drvChannelType);
		}
		else
#endif
#if defined(CONFIG_PVR)
		{
			drvRet = P_DEMUX_ConvertDItoDRVChannelType(pDemux->pidChannel[channelIndex].channelType,&drvChannelType);
			if(drvRet != DRV_OK)
			{
				pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" channel type invalid parameter\n");
				return DRV_ERR;
			}

			drvErr = DRV_PVR_PLAY_PidChannelClose(parserBand-NEXUS_ParserBand_ePb0, pDemux->pidChannel[channelIndex].pidHandle, drvChannelType);
		}
#endif
#endif
		if(drvErr != DRV_OK)
		{
			pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
			VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
			PrintError(" invalid parameter\n");
			return DRV_ERR;
		}
	}
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
	else if(parserBand >= P_DEMUX_GetIBParserForScrambledFilePlayback())
	{
		if(pDemux->pidChannel[channelIndex].channelType <= DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF)
		{
			drvRet = P_DEMUX_ConvertDItoDRVChannelType(pDemux->pidChannel[channelIndex].channelType,&drvChannelType);
			if(drvRet != 0)
			{
				pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" channel type invalid parameter\n");
				return DRV_ERR;
			}
			NEXUS_PidChannel_Close(pDemux->pidChannel[channelIndex].pidHandle);

			drvErr = DRV_PVR_PLAY_PidChannelCloseDummy(0, pDemux->pidChannel[channelIndex].pidHandle, drvChannelType);
			if(drvErr != DRV_OK)
			{
				pDemux->pidChannel[channelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" invalid parameter\n");
				return DRV_ERR;
			}
		}
	}
#endif
	else
	{
		NEXUS_PidChannel_Close(pDemux->pidChannel[channelIndex].pidHandle);
	}

	channelType = pDemux->pidChannel[channelIndex].channelType;

	switch(channelType)
	{
		case DI_DEMUX_CHANNELTYPE_VIDEO:
			if(pDemux->pidChannel[channelIndex].pidHandle == s_hVideoPIDChannel)
			{
				s_hVideoPIDChannel = NULL;

			}
			break;

		case DI_DEMUX_CHANNELTYPE_AUDIO:
		case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF:
			if(pDemux->pidChannel[channelIndex].pidHandle == s_hAudioPIDChannel)
			{
				s_hAudioPIDChannel = NULL;
			}
			break;

		case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION:
			if(pDemux->pidChannel[channelIndex].pidHandle == s_hDescriptionPIDChannel)
			{
				s_hDescriptionPIDChannel = NULL;
			}
			break;

		case DI_DEMUX_CHANNELTYPE_PCR:
#if defined(CONFIG_PVR)
#if defined(CONFIG_CI)
			if(parserBand >= NEXUS_ParserBand_ePb0	|| parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback()) // for playback
			{
				drvErr = DRV_PVR_PLAY_UnsetStcChannelHandle(0/*parserBand-NEXUS_ParserBand_ePb0*/);
				if(drvErr != DRV_OK)
				{
					/*playback error 발생 시(ex: trick중 stc설정은 에러발생) demux 동작에 영향없도록 수정.*/
					//VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
					PrintError(" stc channel unset fail\n");
					//return DRV_ERR;
				}
			}
#else
			if(parserBand >= NEXUS_ParserBand_ePb0) // for playback
			{
#if defined(CONFIG_DI10)
				drvErr = DRV_PVR_PLAY_UnsetStcChannelHandle(0/*parserBand-NEXUS_ParserBand_ePb0*/);
#else
				drvErr = DRV_PVR_PLAY_UnsetStcChannelHandle(parserBand-NEXUS_ParserBand_ePb0);
#endif
				if(drvErr != DRV_OK)
				{
					/*playback error 발생 시(ex: trick중 stc설정은 에러발생) demux 동작에 영향없도록 수정.*/
					//VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
					PrintError(" stc channel unset fail\n");
					//return DRV_ERR;
				}
			}
#endif
#endif
			NEXUS_StcChannel_GetDefaultSettings(DEF_STC_CHANNEL_ID, &stcSettings);
			rc = NEXUS_StcChannel_SetSettings(shstcChannel, &stcSettings);
			if(rc != 0)
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" failed disconnecting PCR channel to STC\n");
				return DRV_ERR;
			}

			if(pDemux->pidChannel[channelIndex].pidHandle == s_hPcrPIDChannel)
			{
				s_hPcrPIDChannel = NULL;
			}
			break;
#if defined(CONFIG_TRANSCODER)
		case DI_DEMUX_CHANNELTYPE_PCR_TRANS_1:
		case DI_DEMUX_CHANNELTYPE_PCR_TRANS_2:
			if(parserBand >= NEXUS_ParserBand_ePb0
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
				|| parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback()
#endif
				) // for playback
			{

				drvErr = DRV_PVR_PLAY_UnsetStcChannelHandleWithDemuxId(demuxId/*parserBand-NEXUS_ParserBand_ePb0*/);
				if(drvErr != DRV_OK)
				{
					/*playback error 발생 시(ex: trick중 stc설정은 에러발생) demux 동작에 영향없도록 수정.*/
					//VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
					PrintError(" stc channel unset fail\n");
					//return DRV_ERR;
				}
			}
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
			if(channelType == DI_DEMUX_CHANNELTYPE_PCR_TRANS_1)
			{
				index = P_DEMUX_TRANSCODER_ID_1;
			}
			else if(channelType == DI_DEMUX_CHANNELTYPE_PCR_TRANS_2)
			{
				index = P_DEMUX_TRANSCODER_ID_2;
			}
			else
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" Invalid parameter : channelType(%d) is not correct!\n", channelType);
				return DRV_ERR_INVALID_PARAMETER;
			}

			drvErr = P_DEMUX_GetStcChannelIndex(s_subhstcChannel[index], &stcChannelIndex);
			if(drvErr != DRV_OK)
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" get STC channel index is failed!\n");
				return DRV_ERR;
			}

			drvErr = P_DEMUX_DeallocateStcChannel(stcChannelIndex);
			if(drvErr != DRV_OK)
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" de-allocation of STC channel is failed!\n");
				return DRV_ERR;
			}
			s_subhstcChannel[index] = NULL;

			drvErr= P_DEMUX_GetStcChannelIndex(s_stcChannelTranscode[index], &stcChannelIndex);
			if(drvErr != DRV_OK)
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" get STC channel index is failed!\n");
				return DRV_ERR;
			}

			drvErr = P_DEMUX_DeallocateStcChannel(stcChannelIndex);
			if(drvErr != DRV_OK)
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" de-allocation of STC channel is failed!\n");
				return DRV_ERR;
			}
			s_stcChannelTranscode[index] = NULL;
#else
			if(pDemux->dmxDevice[demuxId].pathInfo.ulConsumerGroupId >= NUM_TRANSCODE_STC)
			{
				PrintError("transcode Path info is out of range\n");
			}

			if(pDemux->dmxDevice[demuxId].pathInfo.ulConsumerGroupId == 0)
				NEXUS_StcChannel_GetDefaultSettings(DEF_STC_CHANNEL_ID + 1, &stcSettings);
			else
				NEXUS_StcChannel_GetDefaultSettings(DEF_STC_CHANNEL_ID + 3, &stcSettings);

			rc = NEXUS_StcChannel_SetSettings(s_subhstcChannel[pDemux->dmxDevice[demuxId].pathInfo.ulConsumerGroupId], &stcSettings);
			if(rc != 0)
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" failed disconnecting PCR channel to STC\n");
				return DRV_ERR;
			}
#endif
			break;
#endif
#if defined(CONFIG_PIP)
		case DI_DEMUX_CHANNELTYPE_VIDEO_SUB:
			if(pDemux->pidChannel[channelIndex].pidHandle == s_hPIPVideoPIDChannel)
			{
				s_hPIPVideoPIDChannel = NULL;
			}

			break;

		case DI_DEMUX_CHANNELTYPE_PCR_SUB:

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
			drvErr = P_DEMUX_GetStcChannelIndex(shstcChannel_o, &stcChannelIndex);
			if(drvErr != DRV_OK)
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" get STC channel index is failed!\n");
				return DRV_ERR;
			}

			drvErr = P_DEMUX_DeallocateStcChannel(stcChannelIndex);
			if(drvErr != DRV_OK)
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" de-allocation of STC channel is failed!\n");
				return DRV_ERR;
			}

			shstcChannel_o = NULL;
#else
			NEXUS_StcChannel_GetDefaultSettings(DEF_STC_CHANNEL_ID + 1, &stcSettings);
			rc = NEXUS_StcChannel_SetSettings(shstcChannel_o, &stcSettings);
			if(rc != 0)
			{
				VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
				PrintError(" failed disconnecting PCR channel to STC\n");
				return DRV_ERR;
			}
#endif
			if(pDemux->pidChannel[channelIndex].pidHandle == s_hPIPPcrPIDChannel)
			{
				s_hPIPPcrPIDChannel = NULL;
			}
			break;
#endif

#if defined(PRINT_DMX_AV_CALLSEQUENCE)
		case DI_DEMUX_CHANNELTYPE_RECORD:
			if(SectionDbgMode & 0x40000000)
			{
				DI_UART_Print("[STOP RECORD PID]  DemuxID = %d PID = 0x%lx	PID_Channel = 0x%x\n", pDemux->pidChannel[channelIndex].demuxId, pDemux->pidChannel[channelIndex].pid, channelIndex);
			}
			break;
#endif

		default:
				break;
	}

	pDemux->pidChannel[channelIndex].pidHandle = NULL;
	pDemux->pidChannel[channelIndex].pid = DI_DEMUX_INVALID_PID;
	pDemux->pidChannel[channelIndex].demuxId = -1;
	pDemux->pidChannel[channelIndex].parserBand = NEXUS_ParserBand_eInvalid;
	pDemux->pidChannel[channelIndex].channelType = DI_DEMUX_CHANNELTYPE_MAX;
	pDemux->pidChannel[channelIndex].channelsubInfo = P_DEMUX_CHSUBT_eNone;

	VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);

	return DRV_OK;
}
#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
static DRV_Error P_DEMUX_EnablePrimerChannel(DI_DEMUX_CHANNELTYPE eChannelType, HUINT32 ulDemuxId, HUINT32 ulChannelIndex, HUINT32 ulPrimerIndex)
{
	DRV_Error drvErr = DRV_OK;
	NEXUS_Error rc;
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBand parserBand;
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_PidChannelSettings pidChSettings;
	DRV_DEMUX_CHANNELTYPE drvChannelType = 0;
	HUINT32 stcChannelIndex = 0;
	NEXUS_TimebaseSettings timebaseSettings;

	parserBand = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);
	if(parserBand == NEXUS_ParserBand_eInvalid || ulChannelIndex >= DRV_DMX_NUM_PID_CHANNEL)
	{
		PrintError(" invalid parameter\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();
	if(pDemux == NULL)
	{
		PrintError(" pDemux is NULL!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_SEM_Get(pDemux->pidChannel[ulChannelIndex].lock);
	if(pDemux->pidChannel[ulChannelIndex].status != DRV_DEMUX_RESOURCE_ALLOCATED)
	{
		VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
		PrintError(" pid channel %d is not allocated or already enabled\n", ulChannelIndex);
		return DRV_ERR;
	}
	pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;

	if(parserBand >= NEXUS_ParserBand_ePb0) // for playback
	{
#if defined(CONFIG_SI_PARSING_FOR_MEDIA_PLAYBACK)
		if(pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer == DI_DEMUX_INPUT_MEDIA_PLAYER)
		{
#if defined(CONFIG_MEDIA)
			if(DRV_MEDIA_CheckStart() == TRUE)
			{
				drvErr = DRV_MEDIA_OpenPlaybackPidChannel(ulDemuxId, eChannelType, pDemux->pidChannel[ulChannelIndex].pid,
											&(pDemux->pidChannel[ulChannelIndex].pidHandle));
				if(drvErr == DRV_OK)
				{
					pDemux->pidChannel[ulChannelIndex].bFromMedia = TRUE;
				}
			}
#endif
#if defined(CONFIG_CALYPSO)
			if(DRV_CSO_CheckStart() == TRUE)
			{
				drvErr = DRV_CSO_OpenPlaypumpPidChannel(ulDemuxId, eChannelType, pDemux->pidChannel[ulChannelIndex].pid,
											&(pDemux->pidChannel[ulChannelIndex].pidHandle));
				if(drvErr == DRV_OK)
				{
					pDemux->pidChannel[ulChannelIndex].bFromMedia = TRUE;
				}
			}
#endif
		}
		else
		{
			drvErr = P_DEMUX_ConvertDItoDRVChannelType(eChannelType,&drvChannelType);
			if(drvErr != DRV_OK)
			{
				pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" channel type invalid parmeter (%d)\n", ulChannelIndex);
				return DRV_ERR;
			}
				drvErr = DRV_PVR_PLAY_PidChannelOpen(parserBand-NEXUS_ParserBand_ePb0, drvChannelType, pDemux->pidChannel[ulChannelIndex].pid,
									&(pDemux->pidChannel[ulChannelIndex].pidHandle));
		}
#else
		drvErr = P_DEMUX_ConvertDItoDRVChannelType(eChannelType,&drvChannelType);
		if(drvErr != DRV_OK)
		{
			pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
			VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
			PrintError(" invalid channel type (%d)\n", ulChannelIndex);
			return DRV_ERR;
		}
			drvErr = DRV_PVR_PLAY_PidChannelOpen(parserBand-NEXUS_ParserBand_ePb0, drvChannelType, pDemux->pidChannel[ulChannelIndex].pid,
								&(pDemux->pidChannel[ulChannelIndex].pidHandle));
#endif
		if(drvErr != DRV_OK)
		{
			pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
			VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
			PrintError(" pid channel %d is not allocated or already enabled\n", ulChannelIndex);
			return DRV_ERR;
		}
	}
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
	else if(parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback()) //for playback routing
	{
		NEXUS_PidChannel_GetDefaultSettings(&pidChSettings);
		if(P_DEMUX_CheckIfFilterBeAttached(eChannelType))
		{
			pidChSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;
		}
		else
		{
			pidChSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE;
		}
		pDemux->pidChannel[ulChannelIndex].pidHandle = NEXUS_PidChannel_Open(parserBand,
								pDemux->pidChannel[ulChannelIndex].pid, &pidChSettings);

		if(pDemux->pidChannel[ulChannelIndex].pidHandle == NULL)
		{
			pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
			VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
			PrintError(" pid channel %d is not allocated or already enabled\n", ulChannelIndex);
			return DRV_ERR;
		}

		if(pDemux->pidChannel[ulChannelIndex].channelType <= DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF)
		{
			drvErr = P_DEMUX_ConvertDItoDRVChannelType(eChannelType,&drvChannelType);
			if(drvErr != DRV_OK)
			{
				pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" channel type invalid parmeter (%d)\n", ulChannelIndex);
				return DRV_ERR;
			}

			drvErr = DRV_PVR_PLAY_PidChannelOpenDummy(0, drvChannelType, pDemux->pidChannel[ulChannelIndex].pid,
																		pDemux->pidChannel[ulChannelIndex].pidHandle);
			if(drvErr != DRV_OK)
			{
				pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" pid channel %d is not allocated or already enabled\n", ulChannelIndex);
				return DRV_ERR;
			}
		}
	}
#endif
	else
	{
		NEXUS_PidChannel_GetDefaultSettings(&pidChSettings);
		if(P_DEMUX_CheckIfFilterBeAttached(eChannelType))
		{
			pidChSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;
		}
		else
		{
			pidChSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE;
		}
		pDemux->pidChannel[ulChannelIndex].pidHandle = NEXUS_PidChannel_Open(parserBand,
													pDemux->pidChannel[ulChannelIndex].pid, &pidChSettings);

		if(pDemux->pidChannel[ulChannelIndex].pidHandle == NULL)
		{
			pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
			VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
			PrintError(" pid channel %d is not allocated or already enabled\n", ulChannelIndex);
			return DRV_ERR;
		}
	}

	pDemux->pidChannel[ulChannelIndex].demuxId = ulDemuxId;
	pDemux->pidChannel[ulChannelIndex].parserBand = parserBand;
	pDemux->pidChannel[ulChannelIndex].channelsubInfo = P_DEMUX_CHSUBT_eNone;

	switch(eChannelType)
	{
		case DI_DEMUX_CHANNELTYPE_VIDEO_PRIMER:
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulVideoPrimerPIDChannel = ulChannelIndex;
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId = ulDemuxId;
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].bVideoPrimerIsEnable = TRUE;
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].usPid[0] = pDemux->pidChannel[ulChannelIndex].pid;

			PidChannelInfo[ulDemuxId][eChannelType] = DI_DEMUX_INVALID_CHANNEL_ID;
			break;

		case DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER:
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulAudioPrimerPIDChannel = ulChannelIndex;
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId = ulDemuxId;
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].bAudioPrimerIsEnable = TRUE;
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].usPid[1] = pDemux->pidChannel[ulChannelIndex].pid;

			PidChannelInfo[ulDemuxId][eChannelType] = DI_DEMUX_INVALID_CHANNEL_ID;
			break;

		case DI_DEMUX_CHANNELTYPE_PCR_PRIMER:
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulPcrPrimerPIDChannel = ulChannelIndex;
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId = ulDemuxId;
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].usPid[2] = pDemux->pidChannel[ulChannelIndex].pid;

			PidChannelInfo[ulDemuxId][eChannelType] = DI_DEMUX_INVALID_CHANNEL_ID;

			if(primerStcChannel[ulPrimerIndex] == NULL)
			{
				drvErr = P_DEMUX_AllocateStcChannel(&stcChannelIndex);
				if(drvErr != DRV_OK)
				{
					VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
					PrintError(" stc channel allocation is failed!\n");
					return DRV_ERR;
				}

				primerStcChannel[ulPrimerIndex] = pDemux->stcChannel[stcChannelIndex].stcHandle;
				PrintDebug("[%s] stcChannelIndex(%d) allocates to primerStcChannel[%d](0x%x)!\n", __func__, stcChannelIndex, ulPrimerIndex, primerStcChannel[ulPrimerIndex]);
			}

			NEXUS_StcChannel_GetDefaultSettings(stcChannelIndex, &stcSettings);
			stcSettings.timebase = NEXUS_Timebase_e0 + stcChannelIndex;
			stcSettings.autoConfigTimebase = false;
			stcSettings.mode = NEXUS_StcChannelMode_ePcr;
			stcSettings.modeSettings.pcr.pidChannel = pDemux->pidChannel[ulChannelIndex].pidHandle;

			rc = NEXUS_StcChannel_SetSettings(primerStcChannel[ulPrimerIndex], &stcSettings);
			if(rc != 0)
			{
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" NEXUS_StcChannel_SetSettings failed, (error:%d)\n", rc);
				return DRV_ERR;
			}

			NEXUS_Timebase_GetSettings((NEXUS_Timebase_e0 + stcChannelIndex), &timebaseSettings);
			timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
			timebaseSettings.sourceSettings.pcr.pidChannel = pDemux->pidChannel[ulChannelIndex].pidHandle;
			timebaseSettings.sourceSettings.pcr.maxPcrError = stcSettings.modeSettings.pcr.maxPcrError;
			timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
			rc = NEXUS_Timebase_SetSettings((NEXUS_Timebase_e0 + stcChannelIndex), &timebaseSettings);
			if(rc != 0)
			{
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" NEXUS_Timebase_SetSettings failed, (error:%d)\n", rc);
				return DRV_ERR;
			}

			break;

		default:
			break;
	}
	VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);

	return DRV_OK;
}

static DRV_Error P_DEMUX_DisablePrimerChannel(HUINT32 ulChannelIndex, HUINT32 ulPrimerIndex)
{
	DRV_Error drvErr = DRV_OK;
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBand parserBand;
	HUINT32	ulDemuxId = 0;
	DI_DEMUX_CHANNELTYPE eChannelType = DI_DEMUX_CHANNELTYPE_MAX;
	DRV_DEMUX_CHANNELTYPE eDrvChannelType = DRV_DEMUX_CHANNELTYPE_MAX;
	HUINT32 stcChannelIndex = 0;

	if(ulChannelIndex >= DRV_DMX_NUM_PID_CHANNEL)
	{
		PrintError("channelIndex %d is invalid\n", ulChannelIndex);
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();
	if(pDemux == NULL)
	{
		PrintError(" pDemux is NULL!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	ulDemuxId = pDemux->pidChannel[ulChannelIndex].demuxId;
#if defined(CONFIG_PARSERBAND_OPTIMIZE)
	/* release using by latest set parserBand
     because some time this func is called after release path */
	parserBand = pDemux->pidChannel[ulChannelIndex].parserBand;
#else
	parserBand = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);
#endif

	if(parserBand == NEXUS_ParserBand_eInvalid)
	{
		PrintError("Invalid parameter\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_SEM_Get(pDemux->pidChannel[ulChannelIndex].lock);
	if(pDemux->pidChannel[ulChannelIndex].status != DRV_DEMUX_RESOURCE_ENABLED)
	{
		VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
		PrintError("trying to disable channel which hasn't been enabled\n");
		return DRV_ERR;
	}

	if(pDemux->pidChannel[ulChannelIndex].bTSCMonitornigOn == TRUE)
	{
		NEXUS_PidChannel_StopScramblingCheck(pDemux->pidChannel[ulChannelIndex].pidHandle);
		P_DEMUX_DeallocateTSCMonitor(pDemux->pidChannel[ulChannelIndex].ulTSCMonitorIndex);

		pDemux->pidChannel[ulChannelIndex].bTSCMonitornigOn = FALSE;
	}

	pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;


	if(parserBand >= NEXUS_ParserBand_ePb0) // for playback
	{
#if defined(CONFIG_SI_PARSING_FOR_MEDIA_PLAYBACK)
		if(pDemux->pidChannel[ulChannelIndex].bFromMedia == TRUE)
		{
			pDemux->pidChannel[ulChannelIndex].bFromMedia = FALSE;
#if defined(CONFIG_MEDIA)
			if(DRV_MEDIA_CheckStart() == TRUE)
			{
				drvErr = DRV_MEDIA_ClosePlaybackPidChannel(pDemux->pidChannel[ulChannelIndex].demuxId, pDemux->pidChannel[ulChannelIndex].channelType, pDemux->pidChannel[ulChannelIndex].pidHandle);
			}
#endif
#if defined(CONFIG_CALYPSO)
			if(DRV_CSO_CheckStart() == TRUE)
			{
				drvErr = DRV_CSO_ClosePlaypumpPidChannel(pDemux->pidChannel[ulChannelIndex].demuxId, pDemux->pidChannel[ulChannelIndex].channelType, pDemux->pidChannel[ulChannelIndex].pidHandle);
			}
#endif
		}
		else
		{
#if defined(CONFIG_PVR)
			drvErr = P_DEMUX_ConvertDItoDRVChannelType(pDemux->pidChannel[ulChannelIndex].channelType,&eDrvChannelType);
			if(drvErr != DRV_OK)
			{
				pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" invalid parameter\n");
				return DRV_ERR;
			}

			drvErr = DRV_PVR_PLAY_PidChannelClose(parserBand-NEXUS_ParserBand_ePb0, pDemux->pidChannel[ulChannelIndex].pidHandle, eDrvChannelType);

#endif
		}
#else
		{
#if defined(CONFIG_PVR)
			drvErr = P_DEMUX_ConvertDItoDRVChannelType(pDemux->pidChannel[ulChannelIndex].channelType,&eDrvChannelType);
			if(drvErr != DRV_OK)
			{
				pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" channel type invalid parameter\n");
				return DRV_ERR;
			}
			drvErr = DRV_PVR_PLAY_PidChannelClose(parserBand-NEXUS_ParserBand_ePb0, pDemux->pidChannel[ulChannelIndex].pidHandle, eDrvChannelType);
#endif
		}
#endif
		if(drvErr != DRV_OK)
		{
			pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
			VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
			PrintError(" invalid parameter\n");
			return DRV_ERR;
		}
	}
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
	else if(parserBand >= P_DEMUX_GetIBParserForScrambledFilePlayback())
	{
		if(pDemux->pidChannel[ulChannelIndex].channelType <= DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF)
		{
			drvErr = P_DEMUX_ConvertDItoDRVChannelType(pDemux->pidChannel[ulChannelIndex].channelType,&eDrvChannelType);
			if(drvErr != DRV_OK)
			{
				pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" channel type invalid parameter\n");
				return DRV_ERR;
			}

			drvErr = DRV_PVR_PLAY_PidChannelCloseDummy(0, pDemux->pidChannel[ulChannelIndex].pidHandle, eDrvChannelType);
			if(drvErr != DRV_OK)
			{
				pDemux->pidChannel[ulChannelIndex].status = DRV_DEMUX_RESOURCE_ENABLED;
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" invalid parameter\n");
				return DRV_ERR;
			}
		}
	}
#endif
	else
	{
		NEXUS_PidChannel_Close(pDemux->pidChannel[ulChannelIndex].pidHandle);
	}

	eChannelType = pDemux->pidChannel[ulChannelIndex].channelType;

	switch(eChannelType)
	{
		case DI_DEMUX_CHANNELTYPE_VIDEO_PRIMER:
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].bVideoPrimerIsEnable = FALSE;
			break;

		case DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER:
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].bAudioPrimerIsEnable = FALSE;
			break;

		case DI_DEMUX_CHANNELTYPE_PCR_PRIMER:
			drvErr= P_DEMUX_GetStcChannelIndex(primerStcChannel[ulPrimerIndex], &stcChannelIndex);
			if(drvErr != DRV_OK)
			{
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" get STC channel index is failed!\n");
				return DRV_ERR;
			}

			drvErr = P_DEMUX_DeallocateStcChannel(stcChannelIndex);
			if(drvErr != DRV_OK)
			{
				VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);
				PrintError(" de-allocation of STC channel is failed!\n");
				return DRV_ERR;
			}

			primerStcChannel[ulPrimerIndex] = NULL;
			break;

		default:
			break;
	}

	pDemux->pidChannel[ulChannelIndex].pidHandle = NULL;
	pDemux->pidChannel[ulChannelIndex].pid = DI_DEMUX_INVALID_PID;
	pDemux->pidChannel[ulChannelIndex].demuxId = -1;
	pDemux->pidChannel[ulChannelIndex].parserBand = NEXUS_ParserBand_eInvalid;
	pDemux->pidChannel[ulChannelIndex].channelType = DI_DEMUX_CHANNELTYPE_MAX;
	pDemux->pidChannel[ulChannelIndex].channelsubInfo = P_DEMUX_CHSUBT_eNone;

	VK_SEM_Release(pDemux->pidChannel[ulChannelIndex].lock);

	return DRV_OK;
}
#endif

static void P_DEMUX_InitPktSubModule(void)
{
	P_DEMUX_Module *pDemux;
	int pktmodCnt;

	pDemux = P_DEMUX_GetInstance();
	for(pktmodCnt=0; pktmodCnt<MAX_PKTSUB_MODULE_NUM; pktmodCnt++)
	{
		pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_FREE;
		pDemux->stPktSubstitution[pktmodCnt].pBuffer = NULL;
		pDemux->stPktSubstitution[pktmodCnt].buffSize = 0;
	}

	return;
}

/*CAUTION!!!!*/
/*Because of B7335(and it's variants)'s H/W bug, PSUB module can't be connected to the channel where packets to be replaced actually incoming.
   To workaround this bug, we need one more channel allocated to non-exist packet. It can be accomplished by setting parser not to pass nullpackets
   and allocating extra channel to null packet.  3 functions below are implemented to workaround this bug.*/

static HBOOL P_DEMUX_CheckPktSubIsRequired(HUINT32 ulDemuxId, HUINT16 usPid)
{
	P_DEMUX_Module *pDemux;
	int pktmodCnt;
	HBOOL	bRequired = FALSE;
	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(pktsubLock);
	for(pktmodCnt=0; pktmodCnt<MAX_PKTSUB_MODULE_NUM; pktmodCnt++)
	{
		if(   pDemux->stPktSubstitution[pktmodCnt].Status == DRV_DEMUX_RESOURCE_ALLOCATED &&
			pDemux->stPktSubstitution[pktmodCnt].ulDemuxId == ulDemuxId &&
			pDemux->stPktSubstitution[pktmodCnt].usPid == usPid)
		{
			//Found matched one. stop looping.
			break;
		}
	}
	if(pktmodCnt < MAX_PKTSUB_MODULE_NUM)
	{
		bRequired = TRUE;
	}
	VK_SEM_Release(pktsubLock);

	return bRequired;
}

#if defined(CONFIG_DI20)
static DRV_Error P_DEMUX_StartPktSub(HUINT32 ulDemuxId, HUINT32 ulPIDChannel)
{
	P_DEMUX_Module *pDemux;
	NEXUS_Error rc;
	NEXUS_PacketSubOpenSettings packetSubOpenSettings;
	NEXUS_PacketSubSettings packetSubSettings;
	HUINT8  *pPacket;
	HUINT32  pktSize;
	int pktmodCnt;

	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(pktsubLock);
	for(pktmodCnt=0; pktmodCnt<MAX_PKTSUB_MODULE_NUM; pktmodCnt++)
	{
		if(   pDemux->stPktSubstitution[pktmodCnt].Status == DRV_DEMUX_RESOURCE_ALLOCATED &&
			pDemux->stPktSubstitution[pktmodCnt].ulDemuxId == ulDemuxId &&
			((pDemux->stPktSubstitution[pktmodCnt].usPid == pDemux->pidChannel[ulPIDChannel].pid) ||
			(pDemux->stPktSubstitution[pktmodCnt].usPid == P_DEMUX_PAT_PID && pDemux->pidChannel[ulPIDChannel].pid == P_DEMUX_NULLPKT_PID)))
		{
			//Found matched one. stop looping.
			break;
		}
	}
	if(pktmodCnt >= MAX_PKTSUB_MODULE_NUM)
	{
		//No match found.
		VK_SEM_Release(pktsubLock);
		return DRV_ERR;
	}

	if(pDemux->stPktSubstitution[pktmodCnt].hPacketSub != NULL)
	{
		PrintError("pDemux->stPktSubstitution[%d].hPacketSub is NOT	released before.\n", pktmodCnt);
		VK_SEM_Release(pktsubLock);
		return DRV_ERR;
	}

	pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ENABLED;
	VK_SEM_Release(pktsubLock);

	NEXUS_PacketSub_GetDefaultOpenSettings(&packetSubOpenSettings);
	packetSubOpenSettings.fifoSize = pDemux->stPktSubstitution[pktmodCnt].buffSize + 4;

	pDemux->stPktSubstitution[pktmodCnt].hPacketSub = NEXUS_PacketSub_Open(pktmodCnt, &packetSubOpenSettings);
	if(pDemux->stPktSubstitution[pktmodCnt].hPacketSub == NULL)
	{
		VK_SEM_Get(pktsubLock);
		pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;
		VK_SEM_Release(pktsubLock);
		PrintError(" failed NEXUS_PacketSub_Open\n");
		return DRV_ERR;
	}

	NEXUS_PacketSub_GetSettings(pDemux->stPktSubstitution[pktmodCnt].hPacketSub, &packetSubSettings);
	packetSubSettings.pidChannel = pDemux->pidChannel[ulPIDChannel].pidHandle;
	packetSubSettings.loop = TRUE;
	packetSubSettings.forcedInsertion = FALSE;
	packetSubSettings.fullRateOutput = FALSE;

	if(pDemux->stPktSubstitution[pktmodCnt].ulIntervalMS == 0)
	{
		pDemux->stPktSubstitution[pktmodCnt].ulIntervalMS = P_DEMUX_PACKET_INSERT_MAX_INTERVAL;
	}

	packetSubSettings.outputRate = (188*8*1000) / pDemux->stPktSubstitution[pktmodCnt].ulIntervalMS;

	rc = NEXUS_PacketSub_SetSettings(pDemux->stPktSubstitution[pktmodCnt].hPacketSub, &packetSubSettings);
	if(rc != NEXUS_SUCCESS)
	{
		VK_SEM_Get(pktsubLock);
		pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;
		VK_SEM_Release(pktsubLock);
		PrintError(" failed NEXUS_PacketSub_SetSettings. rc=0x%x\n", rc);
		return DRV_ERR;
	}

	rc = NEXUS_PacketSub_GetBuffer(pDemux->stPktSubstitution[pktmodCnt].hPacketSub, (void **)&pPacket, &pktSize);
	if(rc != NEXUS_SUCCESS || pktSize < pDemux->stPktSubstitution[pktmodCnt].buffSize)
	{
		VK_SEM_Get(pktsubLock);
		pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;
		VK_SEM_Release(pktsubLock);
		PrintError(" failed NEXUS_PacketSub_GetBuffer. rc=0x%x, pktSize=%d\n", rc, pktSize);
		return DRV_ERR;
	}

	VK_memcpy(pPacket, pDemux->stPktSubstitution[pktmodCnt].pBuffer, pDemux->stPktSubstitution[pktmodCnt].buffSize);

	rc = NEXUS_PacketSub_ReadComplete(pDemux->stPktSubstitution[pktmodCnt].hPacketSub, pDemux->stPktSubstitution[pktmodCnt].buffSize);
	if(rc != NEXUS_SUCCESS)
	{
		VK_SEM_Get(pktsubLock);
		pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;
		VK_SEM_Release(pktsubLock);
		PrintError(" failed NEXUS_PacketSub_ReadComplete. rc=0x%x\n", rc);
		return DRV_ERR;
	}
	rc = NEXUS_PacketSub_Start(pDemux->stPktSubstitution[pktmodCnt].hPacketSub);
	if(rc != NEXUS_SUCCESS)
	{
		VK_SEM_Get(pktsubLock);
		pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;
		VK_SEM_Release(pktsubLock);
		PrintError(" failed NEXUS_PacketSub_Start. rc=0x%x\n", rc);
		return DRV_ERR;
	}
	return DRV_OK;
}
#else
static DRV_Error P_DEMUX_StartPktSub(HUINT32 ulDemuxId, HUINT32 ulPIDChannel)
{
	P_DEMUX_Module *pDemux;
	NEXUS_Error rc;
	NEXUS_PacketSubOpenSettings packetSubOpenSettings;
	NEXUS_PacketSubSettings packetSubSettings;
	HUINT8  *pPacket;
	HUINT32  pktSize;
	int pktmodCnt;

	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(pktsubLock);
	for(pktmodCnt=0; pktmodCnt<MAX_PKTSUB_MODULE_NUM; pktmodCnt++)
	{
		if(   pDemux->stPktSubstitution[pktmodCnt].Status == DRV_DEMUX_RESOURCE_ALLOCATED &&
			pDemux->stPktSubstitution[pktmodCnt].ulDemuxId == ulDemuxId &&
			((pDemux->stPktSubstitution[pktmodCnt].usPid == pDemux->pidChannel[ulPIDChannel].pid) ||
			(pDemux->stPktSubstitution[pktmodCnt].usPid == P_DEMUX_PAT_PID && pDemux->pidChannel[ulPIDChannel].pid == P_DEMUX_NULLPKT_PID)))
		{
			//Found matched one. stop looping.
			break;
		}
	}
	if(pktmodCnt >= MAX_PKTSUB_MODULE_NUM)
	{
		//No match found.
		VK_SEM_Release(pktsubLock);
		return DRV_ERR;
	}
	pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ENABLED;
	VK_SEM_Release(pktsubLock);

	NEXUS_PacketSub_GetDefaultOpenSettings(&packetSubOpenSettings);
	packetSubOpenSettings.fifoSize = 1024;

	pDemux->stPktSubstitution[pktmodCnt].hPacketSub = NEXUS_PacketSub_Open(pktmodCnt, &packetSubOpenSettings);

	NEXUS_PacketSub_GetSettings(pDemux->stPktSubstitution[pktmodCnt].hPacketSub, &packetSubSettings);
	packetSubSettings.pidChannel = pDemux->pidChannel[ulPIDChannel].pidHandle;
	packetSubSettings.loop = true;
	//packetSubSettings.fullRateOutput = true;
	//packetSubSettings.forcedOutput = true;
	packetSubSettings.outputRate = 1648*10;
	rc = NEXUS_PacketSub_SetSettings(pDemux->stPktSubstitution[pktmodCnt].hPacketSub, &packetSubSettings);
	BDBG_ASSERT(!rc);

	rc = NEXUS_PacketSub_GetBuffer(pDemux->stPktSubstitution[pktmodCnt].hPacketSub, (void **)&pPacket, &pktSize);
	if(rc != 0 || pktSize < pDemux->stPktSubstitution[pktmodCnt].buffSize)
	{
		VK_SEM_Get(pktsubLock);
		pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;
		VK_SEM_Release(pktsubLock);
		PrintError(" failed starting pkt substitution.(st 1)\n");
		return DRV_ERR;
	}
	VK_memcpy(pPacket, pDemux->stPktSubstitution[pktmodCnt].pBuffer, pDemux->stPktSubstitution[pktmodCnt].buffSize);
	rc = NEXUS_PacketSub_ReadComplete(pDemux->stPktSubstitution[pktmodCnt].hPacketSub, pDemux->stPktSubstitution[pktmodCnt].buffSize);
	if(rc != 0)
	{
		VK_SEM_Get(pktsubLock);
		pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;
		VK_SEM_Release(pktsubLock);
		PrintError(" failed starting pkt substitution.(st 2)\n");
		return DRV_ERR;
	}
	rc = NEXUS_PacketSub_Start(pDemux->stPktSubstitution[pktmodCnt].hPacketSub);
	if(rc != 0)
	{
		VK_SEM_Get(pktsubLock);
		pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;
		VK_SEM_Release(pktsubLock);
		PrintError(" failed starting pkt substitution.(st 3)\n");
		return DRV_ERR;
	}
	return DRV_OK;
}
#endif

static DRV_Error P_DEMUX_StopPktSub(HUINT32 ulPIDChannel)
{
	P_DEMUX_Module *pDemux;
	int pktmodCnt;
#if (NEXUS_VERSION >= 1402)
	NEXUS_PacketSubSettings packetSubSettings;
#endif

	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(pktsubLock);
	for(pktmodCnt=0; pktmodCnt<MAX_PKTSUB_MODULE_NUM; pktmodCnt++)
	{
		if(   pDemux->stPktSubstitution[pktmodCnt].Status == DRV_DEMUX_RESOURCE_ENABLED &&
			pDemux->stPktSubstitution[pktmodCnt].ulDemuxId == pDemux->pidChannel[ulPIDChannel].demuxId &&
			((pDemux->stPktSubstitution[pktmodCnt].usPid == pDemux->pidChannel[ulPIDChannel].pid) ||
			(pDemux->stPktSubstitution[pktmodCnt].usPid == P_DEMUX_PAT_PID && pDemux->pidChannel[ulPIDChannel].pid == P_DEMUX_NULLPKT_PID)))
		{
			//Found matched one. stop looping.
			break;
		}
	}
	if(pktmodCnt >= MAX_PKTSUB_MODULE_NUM)
	{
		//No match found.
		VK_SEM_Release(pktsubLock);
		return DRV_OK; /*This func. calls at every stop of recording channel regardless of using psub.*/
	}

#if (NEXUS_VERSION >= 1402)
	NEXUS_PacketSub_GetSettings(pDemux->stPktSubstitution[pktmodCnt].hPacketSub, &packetSubSettings);
	packetSubSettings.outputRate = (188*8*500);/*The PSUB will insert 500 packets per second.*/
	NEXUS_PacketSub_SetSettings(pDemux->stPktSubstitution[pktmodCnt].hPacketSub, &packetSubSettings);
#endif

	pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;
	NEXUS_PacketSub_Stop(pDemux->stPktSubstitution[pktmodCnt].hPacketSub);
	NEXUS_PacketSub_Close(pDemux->stPktSubstitution[pktmodCnt].hPacketSub);
	pDemux->stPktSubstitution[pktmodCnt].hPacketSub = NULL;


	VK_SEM_Release(pktsubLock);
	return DRV_OK;
}

static DRV_Error P_DEMUX_ReleaseFilter(void)
{
	P_DEMUX_Module *pDemux;
	HUINT8 filter_index;

	pDemux = P_DEMUX_GetInstance();
	for(filter_index = 0; filter_index < DRV_DMX_NUM_PID_CHANNEL; filter_index++)
	{
		if(pDemux->msgFilterBuff[filter_index] != NULL)
		{
			NEXUS_Memory_Free(pDemux->msgFilterBuff[filter_index]);
		}
	}
	return DRV_OK;
}

static DRV_Error P_DEMUX_InitFilter(void)
{
	P_DEMUX_Module *pDemux;
	HUINT8 filter_index;
	NEXUS_Error nexusError;
	NEXUS_MemoryAllocationSettings memAllocationSettings;

	pDemux = P_DEMUX_GetInstance();

	for(filter_index = 0; filter_index < DRV_DMX_NUM_PID_CHANNEL; filter_index++)
	{
		NEXUS_Memory_GetDefaultAllocationSettings(&memAllocationSettings);
		memAllocationSettings.alignment = 1024;
		nexusError = NEXUS_Memory_Allocate(DI_MANAGED_FILTER_SIZE, &memAllocationSettings, &pDemux->msgFilterBuff[filter_index]);
		if(nexusError != BERR_SUCCESS)
		{
			PrintError(" Failed allocating from Nexus Heap. (%d)\n", nexusError);
			goto InitFilter_ReleaseResources;
		}
	}

	return DRV_OK;

InitFilter_ReleaseResources:

	P_DEMUX_ReleaseFilter();

	return DRV_ERR;
}

static DRV_Error P_DEMUX_AllocateFilter(P_DEMUX_FilterType eFilterType, DI_DEMUX_PESTYPE ePesType, HUINT8* filterIndex, HUINT32 requestId)
{
	P_DEMUX_Module *pDemux;
	HUINT8 filter_index;
	struct msgfilterlist *filtElem;

	if(filterIndex == NULL)
	{
		PrintError(" Invalid parameter\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	if(eFilterType >= P_DEMUX_FilterType_MAX)
	{
		PrintError(" Unknown filter type. (%d)\n", eFilterType);
		*filterIndex = 0xFF;
		return DRV_ERR;
	}

	VK_SEM_Get(sqLock);
	if(BLST_SQ_EMPTY(&gMsgHead))
	{
		VK_SEM_Release(sqLock);
		PrintError(" no more available filter\n");
		*filterIndex = 0xff;
		return DRV_ERR;
	}
	filtElem = BLST_SQ_FIRST(&gMsgHead);
	BLST_SQ_REMOVE_HEAD(&gMsgHead, link);
	filter_index = filtElem->filterIndex;
	VK_SEM_Release(sqLock);

	if(pDemux->msgFilter[filter_index].status != DRV_DEMUX_RESOURCE_FREE)
	{

		PrintError(" filter is not in free state\n");
		*filterIndex = 0xff;
		return DRV_ERR;
	}
	pDemux->msgFilter[filter_index].status = DRV_DEMUX_RESOURCE_ALLOCATED;
	*filterIndex = filter_index;

	//DI_UART_Print("P_DEMUX_AllocateFilter: filter %d is allocated\n",filter_index);
	pDemux->msgFilter[filter_index].requestId = requestId;
	pDemux->msgFilter[filter_index].filterType = eFilterType;
	pDemux->msgFilter[filter_index].etPesType = ePesType;

	return DRV_OK;
}

static DRV_Error P_DEMUX_DeallocateFilter(P_DEMUX_FilterType eFilterType, HUINT8 filterIndex)
{
	P_DEMUX_Module *pDemux;

	if((eFilterType >= P_DEMUX_FilterType_MAX)||(filterIndex >= MAX_SECTION_FILTER_NUM))
	{
		PrintError(" Unknown filter type. (%d)\n", eFilterType);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();

	if(pDemux->msgFilter[filterIndex].status != DRV_DEMUX_RESOURCE_ALLOCATED)
	{
		PrintError(" filter %d is free or enabled\n", filterIndex);
		return DRV_ERR;
	}
	pDemux->msgFilter[filterIndex].status = DRV_DEMUX_RESOURCE_FREE;
	VK_SEM_Get(sqLock);
	BLST_SQ_INSERT_TAIL(&gMsgHead, &gMsgFilter[filterIndex], link);
	VK_SEM_Release(sqLock);

	return DRV_OK;
}

static DRV_Error P_DEMUX_EnableFilter(P_DEMUX_FilterType eFilterType, HUINT8 filterIndex, HUINT8 channelIndex,
										HUINT8 *filterValue, HUINT8 *filterMask, HUINT8 *filterExclusion,
										HUINT32 filterLen, HBOOL filterMode, HBOOL bCheckCRC, HUINT32 ulRequestID)
{
	P_DEMUX_Module *pDemux;
	NEXUS_MessageSettings openSettings;
	NEXUS_MessageStartSettings startSettings;
	HUINT32 i;
	HUINT32 buffIndex;

#if !defined(CONFIG_DEBUG)
	BSTD_UNUSED(filterExclusion);
#endif

	if((eFilterType >= P_DEMUX_FilterType_MAX)||(filterIndex >= MAX_SECTION_FILTER_NUM))
	{
		PrintError(" Unknown filter type. (%d)\n", eFilterType);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();

	pDemux->msgFilter[filterIndex].channelIndex = channelIndex;
#if defined(CONFIG_MSG_PID2BUF)
	pDemux->msgFilter[filterIndex].physicalChannelIndex = filterIndex;
#else
	pDemux->msgFilter[filterIndex].physicalChannelIndex = P_DEMUX_GetPhysicalPidChannelIndex(pDemux->pidChannel[pDemux->msgFilter[filterIndex].channelIndex].pidHandle);
#endif
	VK_SEM_Get(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);

	if(pDemux->msgFilter[filterIndex].status != DRV_DEMUX_RESOURCE_ALLOCATED)
	{
		VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
		PrintError(" filter %d is free or enabled\n", filterIndex);
		return DRV_ERR;
	}
	pDemux->msgFilter[filterIndex].status = DRV_DEMUX_RESOURCE_ENABLED;

#if defined(MESSAGE_FILTER_START_STOP_ONLY)
	NEXUS_Message_GetSettings(pDemux->msgFilter[filterIndex].msgHandle, &openSettings);
	openSettings.dataReady.callback = P_DEMUX_NEXUS_NotifyCB;
	openSettings.dataReady.context =  (void *)MAKE_FILTERPAIRVALUE(eFilterType ,filterIndex);
	openSettings.dataReady.param = ulRequestID;
	openSettings.overflow.callback = P_DEMUX_NEXUS_OverflowCB;
	openSettings.overflow.context = (void *)MAKE_FILTERPAIRVALUE(eFilterType ,filterIndex);
	openSettings.overflow.param = ulRequestID;
	openSettings.bufferSize =0;
#if defined(CONFIG_COMPLETE_SECTION_IN_DI)
	openSettings.maxContiguousMessageSize = 0;
#endif
	NEXUS_Message_SetSettings(pDemux->msgFilter[filterIndex].msgHandle, &openSettings);
#else
	NEXUS_Message_GetDefaultSettings(&openSettings);

	openSettings.dataReady.callback = P_DEMUX_NEXUS_NotifyCB;
	openSettings.dataReady.context =  (void *)MAKE_FILTERPAIRVALUE(eFilterType ,filterIndex);
	openSettings.dataReady.param = ulRequestID;
	openSettings.overflow.callback = P_DEMUX_NEXUS_OverflowCB;
	openSettings.overflow.context = (void *)MAKE_FILTERPAIRVALUE(eFilterType ,filterIndex);
	openSettings.overflow.param = ulRequestID;
	openSettings.bufferSize =0;
#if defined(CONFIG_COMPLETE_SECTION_IN_DI)
	openSettings.maxContiguousMessageSize = 0;
#endif

	pDemux->msgFilter[filterIndex].msgHandle = NEXUS_Message_Open(&openSettings);
	if(pDemux->msgFilter[filterIndex].msgHandle == NULL)
	{
		VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
		pDemux->msgFilter[filterIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
		PrintError(" NEXUS_Message_Open() return error\n");
		return DRV_ERR;
	}
#endif
	NEXUS_Message_GetDefaultStartSettings(pDemux->msgFilter[filterIndex].msgHandle,&startSettings);

	startSettings.pidChannel = pDemux->pidChannel[channelIndex].pidHandle;
	startSettings.format = P_DEMUX_GetNexusMessageFormat(eFilterType);
#if defined(CONFIG_MSG_PID2BUF)
	startSettings.filterGroup = false;
#else
	startSettings.filterGroup = P_DEMUX_CheckIfGroupingRequired(eFilterType);
#endif
	startSettings.bufferMode = (filterMode == TRUE ? NEXUS_MessageBufferMode_eOneMessage : NEXUS_MessageBufferMode_eContinuous);
	startSettings.bank = -1;
	startSettings.psiCrcDisabled = !bCheckCRC;
	startSettings.psfCrcDisabled = !bCheckCRC;

	buffIndex = P_DEMUX_GetPhysicalPidChannelIndex(pDemux->pidChannel[channelIndex].pidHandle);
#if defined(CONFIG_MSG_PID2BUF)
	if(buffIndex == 255)
#else
	if(buffIndex >= DRV_DMX_NUM_PID_CHANNEL)
#endif
	{
#if defined(MESSAGE_FILTER_START_STOP_ONLY)
		/*Don't close filter*/
#else
		NEXUS_Message_Close(pDemux->msgFilter[filterIndex].msgHandle);
#endif
		VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
		pDemux->msgFilter[filterIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
		PrintError("This pid Channel can't be used for Message filtering!. [pidchannel = %d]\n",buffIndex);
		return DRV_ERR;

	}
	//DI_UART_Print("BUFFINDEX = %d filterIndex = %d\n",buffIndex, filterIndex);
	if(eFilterType == P_DEMUX_PesFilter)
	{
		NEXUS_Error nexusError;
		NEXUS_MemoryAllocationSettings memAllocationSettings;
		NEXUS_Memory_GetDefaultAllocationSettings(&memAllocationSettings);
		memAllocationSettings.alignment = 1024;
		nexusError = NEXUS_Memory_Allocate(DI_MANAGED_PES_FILTER_SIZE, &memAllocationSettings, &PesFilterBuff[filterIndex]);
		if(nexusError != BERR_SUCCESS)
		{
			while(1)
			{
				PrintError(" Failed allocating from Nexus Heap. (%d)\n", nexusError);
			}
		}
		startSettings.buffer = PesFilterBuff[filterIndex];
		startSettings.bufferSize = DI_MANAGED_PES_FILTER_SIZE;
	}
	else
	{
#if defined(CONFIG_MSG_PID2BUF)
		startSettings.buffer = pDemux->msgFilterBuff[filterIndex];
#else
		startSettings.buffer = pDemux->msgFilterBuff[buffIndex];
#endif
#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
		if(eFilterType == P_DEMUX_TsFilter)
			startSettings.bufferSize = 1024;
		else
#endif
		startSettings.bufferSize = DI_MANAGED_FILTER_SIZE;
	}
#if defined(SECTION_FILTER_DEBUGGING)
	if(eFilterType == P_DEMUX_PsiFilter || eFilterType == P_DEMUX_DEBUG_PsiFilter)
#else
	if(eFilterType == P_DEMUX_PsiFilter)
#endif
	{
		PrintDebug("[SEC] value : ");
		for(i=0; i<filterLen; i++)
			PrintDebug("%02x ", filterValue[i]);
		PrintDebug("\n");

		PrintDebug("[SEC] mask : ");
		for(i=0; i<filterLen; i++)
			PrintDebug("%02x ", filterMask[i]);
		PrintDebug("\n");

		PrintDebug("[SEC] exclu : ");
		for(i=0; i<filterLen; i++)
			PrintDebug("%02x ", filterExclusion[i]);
		PrintDebug("\n");

		VK_memset(startSettings.filter.coefficient, 0x00, 16);
		VK_memset(startSettings.filter.exclusion, 0xff, 16);
		VK_memset(startSettings.filter.mask, 0xff, 16);

		startSettings.filter.coefficient[0] = filterValue[0];
		startSettings.filter.mask[0] = ~filterMask[0];

		for(i=1; i<filterLen; i++)
		{
			startSettings.filter.coefficient[i+1] = filterValue[i];
			startSettings.filter.mask[i+1] = ~filterMask[i];
		}
	}

	if(NEXUS_Message_Start(pDemux->msgFilter[filterIndex].msgHandle, &startSettings) != NEXUS_SUCCESS)
	{
#if defined(MESSAGE_FILTER_START_STOP_ONLY)
		/*Don't close filter*/
#else
		NEXUS_Message_Close(pDemux->msgFilter[filterIndex].msgHandle);
#endif
		VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
		pDemux->msgFilter[filterIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;
		PrintError(" NEXUS_Message_Start() return error\n");
		return DRV_ERR;
	}
	VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);

	return DRV_OK;
}

static DRV_Error P_DEMUX_DisableFilter(P_DEMUX_FilterType eFilterType, HUINT8 filterIndex)
{
	P_DEMUX_Module *pDemux;
	HUINT8 tempChannelIndex;

	if((eFilterType >= P_DEMUX_FilterType_MAX)||(filterIndex >= MAX_SECTION_FILTER_NUM))
	{
		PrintError(" Unknown filter type. (%d)\n", eFilterType);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);

	if(pDemux->msgFilter[filterIndex].status != DRV_DEMUX_RESOURCE_ENABLED)
	{
		VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
		PrintError(" filter %d is not enabled\n", filterIndex);
		return DRV_ERR;
	}
	pDemux->msgFilter[filterIndex].status = DRV_DEMUX_RESOURCE_ALLOCATED;

	NEXUS_Message_Stop(pDemux->msgFilter[filterIndex].msgHandle);
#if defined(MESSAGE_FILTER_START_STOP_ONLY)
		/*Don't close filter*/
#else
	NEXUS_Message_Close(pDemux->msgFilter[filterIndex].msgHandle);
	pDemux->msgFilter[filterIndex].msgHandle = NULL;
#endif
	tempChannelIndex = pDemux->msgFilter[filterIndex].channelIndex;
	pDemux->msgFilter[filterIndex].channelIndex = DI_DEMUX_INVALID_CHANNEL_ID;
	pDemux->msgFilter[filterIndex].requestId = 0;
	if(eFilterType == P_DEMUX_PesFilter)
	{
		NEXUS_Memory_Free(PesFilterBuff[filterIndex]);
		PesFilterBuff[filterIndex] = NULL;
	}

	VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);

	return DRV_OK;
}


#if defined(CONFIG_CI)
//Should be called only in ciLock semaphore protection.
typedef enum _CamManagementMsg
{
	Explicit_Connection_Req,
	Explicit_Disconnection_Req,
	Implicit_Connection_Req,
	Implicit_Disconnection_Req,
	CAM_Connected_Sts,
	CAM_Disconnected_Sts,
	CAM_Msg_Max
}CamManagementMsg;

typedef struct _CamCurrentStatus
{
	unsigned int cmdReceived;
	unsigned int phyStatus;
	unsigned int cmdExecuted;
}CamCurrentStatus;

typedef struct{
	HBOOL bIsValid;
	HBOOL bIsPlayback;
	NEXUS_InputBand inputBand;
	NEXUS_ParserBand parserBand;
}CamPathInfo;

//CAM_PATH_INFO camPathInfo[NUM_CAM];

typedef struct{
	CamPathInfo	pathInfo;
	CamCurrentStatus status;
}CamManageStruct;

CamManageStruct	stCamManage[CAM_MAXSLOTID];

static DRV_Error P_DEMUX_UnrouteUnusedCamToDefaultPath(HINT32 slotId)
{
	P_DEMUX_Module *pDemux;
	NEXUS_RemuxSettings rmx_settings;

	if((slotId < 0)||(slotId >= CAM_MAXSLOTID))
	{
		PrintError("Invalid parameter. (slotId = %d)\n", slotId);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();

	if(pDemux->ciInfo[slotId].bShadowOn == TRUE)
	{
		NEXUS_Remux_Stop(pDemux->rmxHandle);
		NEXUS_Remux_Close(pDemux->rmxHandle);
		NEXUS_Remux_GetDefaultSettings(&rmx_settings);
		rmx_settings.parallelOutput = false;
		NEXUS_Remux_Close(NEXUS_Remux_Open(1, &rmx_settings));
		pDemux->ciInfo[slotId].bShadowOn = FALSE;
	}

	return DRV_OK;
}

//Should be called only in ciLock semaphore protection.
static DRV_Error P_DEMUX_RouteCamToRequestedPath(HINT32 slotId, HBOOL bIsPlayback, NEXUS_InputBand inputBand, NEXUS_ParserBand parserBand, HUINT32 ulDemuxId)
{
	NEXUS_Error rc;
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBandSettings pb_settings;
	NEXUS_RemuxSettings rmx_settings;
	NEXUS_PidChannelHandle	playbackPidChannel = NULL;
	DRV_Error drvErr = DRV_ERR;

	if((slotId < 0)||(slotId >= CAM_MAXSLOTID))
	{
		PrintError("Invalid parameter. (slotId = %d)\n", slotId);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();

	if(pDemux->ciInfo[slotId].bShadowOn == TRUE)
	{
		NEXUS_Remux_Stop(pDemux->rmxHandle);
		NEXUS_Remux_Close(pDemux->rmxHandle);
		pDemux->ciInfo[slotId].bShadowOn = FALSE;
	}

	if(bIsPlayback == FALSE) /*Live*/
	{
		// Remux setting for externel route
		NEXUS_Remux_GetDefaultSettings(&rmx_settings);

		rmx_settings.parallelOutput = TRUE;

#if defined(CONFIG_REMUX_EXT_CLOCK)
		rmx_settings.outputClock = NEXUS_RemuxClock_eInputBand;
#if defined(CONFIG_USE_RMX_EXT_CLOCK_INSTANCE)
		rmx_settings.remuxClockIBSrc = s_ExtClockInputBand;
#else
		rmx_settings.remuxClockIBSrc = NEXUS_InputBand_e5;
#endif
#else
		rmx_settings.outputClock = NEXUS_RemuxClock_e54Mhz;
#endif

		rmx_settings.allPass = TRUE;
		rmx_settings.bypass= false;
	 	rmx_settings.maxDataRate = 108000000;

		pDemux->rmxHandle = NEXUS_Remux_Open(1, &rmx_settings);
		if(pDemux->rmxHandle == NULL)
		{
			return DRV_ERR;
		}
#if defined(CONFIG_BCM_MTSIF)
		/* Using MTSIF, set-up parseband is updated from P_DEMUX_SetMTSIFPath API */
#else
		NEXUS_ParserBand_GetSettings(parserBand, &pb_settings);
		pb_settings.allPass = TRUE;
		pb_settings.acceptNullPackets = FALSE;
		pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
		pb_settings.sourceTypeSettings.inputBand = inputBand;
		pb_settings.transportType = NEXUS_TransportType_eTs;
		rc = NEXUS_ParserBand_SetSettings(parserBand, &pb_settings);
		if(rc != NEXUS_SUCCESS)
		{
			NEXUS_Remux_Close(pDemux->rmxHandle);
			PrintError("Failed to set ParserBand for CI (%d)\n", rc);
			return DRV_ERR;
		}
#endif
		rc = NEXUS_Remux_AddPidChannel(pDemux->rmxHandle, pDemux->pidChannel[RESERVED_CHANNEL_LIVE1_LOGICAL].pidHandle);
		if(rc != NEXUS_SUCCESS)
		{
			NEXUS_Remux_Close(pDemux->rmxHandle);
			PrintError("Failed to add PID channel to Remux module.(%d)\n", rc);
			return DRV_ERR;
		}
		rc = NEXUS_Remux_Start(pDemux->rmxHandle);
		if(rc != NEXUS_SUCCESS)
		{
			NEXUS_Remux_RemovePidChannel(pDemux->rmxHandle, pDemux->pidChannel[RESERVED_CHANNEL_LIVE1_LOGICAL].pidHandle);
			NEXUS_Remux_Close(pDemux->rmxHandle);
			PrintError("Failed to start Remux module.(%d)\n", rc);
			return DRV_ERR;
		}
	}
	else /*Playback*/
	{
		drvErr = DRV_PVR_RequestAllpassChannel(ulDemuxId, &playbackPidChannel);
		if((drvErr != DRV_OK)||(playbackPidChannel == NULL))
		{
			PrintError("Failed to DRV_PVR_RequestAllpassChannel (drvErr:%d, playbackPidChannel:0x%x).\n", drvErr, playbackPidChannel);
			return DRV_ERR;
		}

		// Remux setting for externel route
		NEXUS_Remux_GetDefaultSettings(&rmx_settings);
		rmx_settings.parallelOutput = TRUE;
		rmx_settings.outputClock = NEXUS_RemuxClock_e27Mhz_VCXO_A;
		rmx_settings.allPass = TRUE;
		rmx_settings.bypass= false;

		pDemux->rmxHandle = NEXUS_Remux_Open(1, &rmx_settings);
		if(pDemux->rmxHandle == NULL)
		{
			return DRV_ERR;
		}

		if(playbackPidChannel != NULL)
		{
			rc = NEXUS_Remux_AddPidChannel(pDemux->rmxHandle, playbackPidChannel);
			if(rc != NEXUS_SUCCESS)
			{
				NEXUS_Remux_Close(pDemux->rmxHandle);
				PrintError("Failed to add PID channel to Remux module.(%d)\n", rc);
				return DRV_ERR;
			}
			rc = NEXUS_Remux_Start(pDemux->rmxHandle);
			if(rc != NEXUS_SUCCESS)
			{
				NEXUS_Remux_RemovePidChannel(pDemux->rmxHandle, playbackPidChannel);
				NEXUS_Remux_Close(pDemux->rmxHandle);

				PrintError("Failed to start Remux module.(%d)\n", rc);
				return DRV_ERR;
			}
		}
		else
		{
			NEXUS_Remux_Close(pDemux->rmxHandle);
			PrintError("Failed to add PID channel to Remux module.\n");
			return DRV_ERR;
		}
	}
	return DRV_OK;
}

/************************************************************************************************************************************************************/
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
DRV_Error	P_DEMUX_InitRemuxInstance(void)
{
	HUINT32	index = 0;
	HCHAR 	ucSemName[20] = {0,};
	HINT32	nRet = 0;

	for(index = 0; index < REMUX_MAX_NUM; index++)
	{
		remuxInstance[index].used = 0;
		remuxInstance[index].status = 0;
		remuxInstance[index].playback = 0;
		remuxInstance[index].rmxHandle = NULL;
		remuxInstance[index].rmxPlayPidChannel = NULL;
		VK_MEM_Memset(ucSemName, 0x00, 20);
		snprintf(ucSemName, 18, "di_rmx%d", index);
		nRet = VK_SEM_Create(&(remuxInstance[index].rmxSemId), ucSemName, DEFAULT_SUSPEND_TYPE);
		if( nRet != VK_OK )
		{
			break;
			return DRV_ERR_EVENT_INITIALIZATION;
		}
	}

	return DRV_OK;
}

P_REMUX_Module	*P_DEMUX_GetRemuxInstance(HUINT32 ulRemuxIndex)
{
	if(ulRemuxIndex >= REMUX_MAX_NUM)
	{
		return NULL;
	}

	return &(remuxInstance[ulRemuxIndex]);
}

DRV_Error	P_DEMUX_GetRemuxIndex(HUINT32 *gRemuxIndex)
{
	HUINT32	index = 0;
	P_REMUX_Module	*pRemux = NULL;

	for(index = 0; index < REMUX_MAX_NUM; index++)
	{
		pRemux = P_DEMUX_GetRemuxInstance(index);
		if(pRemux != NULL)
		{
			VK_SEM_Get(pRemux->rmxSemId);
			if(pRemux->used == 0)
			{
				pRemux->used = 1;
				VK_SEM_Release(pRemux->rmxSemId);
				break;
			}
			VK_SEM_Release(pRemux->rmxSemId);
		}
	}


	if(index < REMUX_MAX_NUM)
	{
		*gRemuxIndex = index;
		return DRV_OK;
	}

	return DRV_ERR;

}

DRV_Error	P_DEMUX_GetCountUsedRemux(HUINT32 *gRemuxCount)
{
	HUINT32	index = 0,count = 0;
	P_REMUX_Module	*pRemux = NULL;

	for(index = 0; index < REMUX_MAX_NUM; index++)
	{
		pRemux = P_DEMUX_GetRemuxInstance(index);
		if(pRemux != NULL)
		{
			VK_SEM_Get(pRemux->rmxSemId);
			if( pRemux->used == 1)
			{
				count++;
			}
			VK_SEM_Release(pRemux->rmxSemId);
		}
	}

	*gRemuxCount = count;

	return DRV_OK;
}

DRV_Error	P_DEMUX_CreateRemux( HUINT32 *pGetIndex )
{
	DRV_Error	drvErr = 0;
	HUINT32	index = 0;
	NEXUS_RemuxSettings rmx_settings;
	P_REMUX_Module	*pRemux = NULL;

	drvErr = P_DEMUX_GetRemuxIndex(&index);
	if((drvErr == DRV_OK)&&(index < REMUX_MAX_NUM ))
	{
		pRemux = P_DEMUX_GetRemuxInstance(index);
		if(pRemux != NULL)
		{
			VK_SEM_Get(pRemux->rmxSemId);
			NEXUS_Remux_GetDefaultSettings(&rmx_settings);
#if 0 /* org */
			rmx_settings.outputClock = NEXUS_RemuxClock_e54Mhz;
#else /* modify for 특정 CAM에서 IDLE 구간이 긴 stream이 들어올 경우, CIMAX FIFO 가 죽는 경우 자주 발생하여,  김희영 차장님 테스트 결과로 수정함 */
			rmx_settings.outputClock = NEXUS_RemuxClock_e27Mhz_VCXO_A;
#endif
			rmx_settings.allPass = TRUE;
			rmx_settings.bypass= false;
			pRemux->rmxHandle = NEXUS_Remux_Open(index, &rmx_settings);
			if(pRemux->rmxHandle == NULL)
			{
				pRemux->used = 0;
					VK_SEM_Release(pRemux->rmxSemId);
				return DRV_ERR;
			}

			*pGetIndex = index;

			VK_SEM_Release(pRemux->rmxSemId);
		}
		return DRV_OK;
	}

	return DRV_ERR;

}


DRV_Error	P_DEMUX_ConnectPidChanneltoRemux( HUINT32 ulIndex, HUINT32 ulPbDemuxId, HUINT32	ulPlayback )
{
	DRV_Error	drvErr = 0;
	P_REMUX_Module	*pRemux = NULL;

	NEXUS_Error rc = 0;

	NEXUS_ParserBandSettings pb_settings;
	NEXUS_ParserBand parserBand = 0;

	P_DEMUX_Module *pDemux = NULL;
	pDemux = P_DEMUX_GetInstance();
	pRemux = P_DEMUX_GetRemuxInstance(ulIndex);

	if(pRemux == NULL)
	{
		return DRV_ERR;
	}

	VK_SEM_Get(pRemux->rmxSemId);
	if((pRemux->rmxHandle == NULL)||(pRemux->used == 0))
	{
		VK_SEM_Release(pRemux->rmxSemId);
		return DRV_ERR;
	}

	if(ulPlayback)
	{
		drvErr = DRV_PVR_RequestAllpassChannel(ulPbDemuxId, &(pRemux->rmxPlayPidChannel));
		if((drvErr != 0)||(pRemux->rmxPlayPidChannel == NULL))
		{
			pRemux->playback = 0;
			goto FAIL;
		}

		pRemux->playback = 1;
	}
	else
	{
		parserBand = (MAX_IBPARSER_BAND_NUM-1);

		NEXUS_ParserBand_GetSettings(parserBand, &pb_settings);
		pb_settings.allPass = TRUE;
		pb_settings.acceptNullPackets = FALSE;
		pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
		pb_settings.sourceTypeSettings.inputBand = pDemux->dmxDevice[ulPbDemuxId].pathInfo.inputBand;
		pb_settings.transportType = NEXUS_TransportType_eTs;

		rc = NEXUS_ParserBand_SetSettings(parserBand, &pb_settings);
		pRemux->rmxPlayPidChannel = pDemux->pidChannel[RESERVED_CHANNEL_LIVE1_LOGICAL].pidHandle;
		pRemux->playback = 0;
	}

	if(pRemux->rmxPlayPidChannel != NULL)
	{
		rc = NEXUS_Remux_AddPidChannel(pRemux->rmxHandle,pRemux->rmxPlayPidChannel);
		if(rc != NEXUS_SUCCESS)
		{
			pRemux->rmxPlayPidChannel = NULL;
			PrintError("Failed to add PID channel to Remux module.(%d)\n", rc);
			goto FAIL;

		}

		rc = NEXUS_Remux_Start(pRemux->rmxHandle);
		if(rc != NEXUS_SUCCESS)
		{
			NEXUS_Remux_RemovePidChannel(pRemux->rmxHandle, pRemux->rmxPlayPidChannel);
			pRemux->rmxPlayPidChannel = NULL;
			PrintError("Failed to start Remux module.(%d)\n", rc);
			goto FAIL;
		}

		pRemux->status = 1;

		VK_SEM_Release(pRemux->rmxSemId);
		return DRV_OK;
	}

FAIL:
	NEXUS_Remux_Close(pRemux->rmxHandle);
	pRemux->rmxHandle = NULL;
	VK_SEM_Release(pRemux->rmxSemId);
	PrintError("== PB Failed to add PID channel to Remux module.(%d)\n", rc);

	return DRV_ERR;

}


DRV_Error	P_DEMUX_StopRemux( HUINT32 ulIndex )
{
	P_REMUX_Module	*pRemux = NULL;

	pRemux = P_DEMUX_GetRemuxInstance(ulIndex);

	if(pRemux == NULL)
	{
		return DRV_ERR;
	}

	VK_SEM_Get(pRemux->rmxSemId);
	if((pRemux->rmxHandle == NULL)||(pRemux->used == 0))
	{
		VK_SEM_Release(pRemux->rmxSemId);
		return DRV_ERR;
	}

	NEXUS_Remux_Stop(pRemux->rmxHandle);
	pRemux->status = 0;
	VK_SEM_Release(pRemux->rmxSemId);

	return DRV_OK;
}

DRV_Error	P_DEMUX_RestartRemux( HUINT32 ulIndex )
{
	P_REMUX_Module	*pRemux = NULL;
	NEXUS_Error rc = 0;

	pRemux = P_DEMUX_GetRemuxInstance(ulIndex);

	if(pRemux == NULL)
	{
		return DRV_ERR;
	}

	VK_SEM_Get(pRemux->rmxSemId);
	if((pRemux->rmxHandle == NULL)||(pRemux->used == 0))
	{
		VK_SEM_Release(pRemux->rmxSemId);
			return DRV_ERR;
		}

	rc = NEXUS_Remux_Start(pRemux->rmxHandle);
		if(rc != NEXUS_SUCCESS)
		{
		PrintError("Failed to stop Remux module.(%d)\n", rc);
		pRemux->status = 0;
		VK_SEM_Release(pRemux->rmxSemId);
		return DRV_ERR;
	}

	pRemux->status = 1;
	VK_SEM_Release(pRemux->rmxSemId);
	return DRV_OK;

}

DRV_Error	P_DEMUX_DestroyRemux( HUINT32 ulIndex )
{
	P_REMUX_Module	*pRemux = NULL;

	pRemux = P_DEMUX_GetRemuxInstance(ulIndex);

	if(pRemux == NULL)
	{
		return DRV_ERR;
	}

	VK_SEM_Get(pRemux->rmxSemId);
	if((pRemux->rmxHandle == NULL)||(pRemux->used == 0))
	{
		VK_SEM_Release(pRemux->rmxSemId);
		return DRV_ERR;
	}

	if(pRemux->status != 0)
	{
		NEXUS_Remux_Stop(pRemux->rmxHandle);
	}

	NEXUS_Remux_RemovePidChannel(pRemux->rmxHandle, pRemux->rmxPlayPidChannel);
	NEXUS_Remux_Close(pRemux->rmxHandle);
	pRemux->used = 0;
	pRemux->status = 0;
	pRemux->playback = 0;
	pRemux->rmxHandle = NULL;
	pRemux->rmxPlayPidChannel = NULL;
	VK_SEM_Release(pRemux->rmxSemId);
	return DRV_OK;

}

DRV_Error P_DEMUX_RouteRemuxToRequestedPath( HUINT32 ulDemuxId, HUINT32 playback )
{
	DRV_Error drvErr = 0;
	HUINT32 usedCountOfRemux = 0,remuxIndex = 0;
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();

	drvErr = P_DEMUX_GetCountUsedRemux(&usedCountOfRemux);
	if(usedCountOfRemux == REMUX_MAX_NUM)
	{
		PrintError("RemuxHandle Full \r\n");
		return DRV_ERR;
	}

	drvErr = P_DEMUX_CreateRemux(&remuxIndex);
	if((drvErr != 0)||(remuxIndex >= REMUX_MAX_NUM))
	{
		PrintError("Can't Create RemuxHandle(%d:%d)\r\n",drvErr,remuxIndex);
		return DRV_ERR;
	}

	drvErr = P_DEMUX_ConnectPidChanneltoRemux(remuxIndex,ulDemuxId,playback);
	if(drvErr != 0)
	{
		PrintError("Can't Connect Playback to Remux(%d:%d)\r\n",drvErr,remuxIndex);
		return DRV_ERR;
	}

	pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId = remuxIndex;

	return DRV_OK;

}

DRV_Error P_DEMUX_RouteTempRemuxToRequestedPath( HUINT32 ulDemuxId )
{
	DRV_Error drvErr = 0;
	HUINT32 usedCountOfRemux = 0,remuxIndex = 0;
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();
	drvErr = P_DEMUX_GetCountUsedRemux(&usedCountOfRemux);
	if(usedCountOfRemux == REMUX_MAX_NUM)
	{
		PrintError("RemuxHandle Full \r\n");
		return DRV_ERR;
	}

	drvErr = P_DEMUX_CreateRemux(&remuxIndex);
	if((drvErr != 0)||(remuxIndex >= REMUX_MAX_NUM))
	{
		PrintError("Can't Create RemuxHandle(%d:%d)\r\n",drvErr,remuxIndex);
		return DRV_ERR;
	}

	drvErr = P_DEMUX_ConnectPidChanneltoRemux(remuxIndex,ulDemuxId,1);
	if(drvErr != 0)
	{
		PrintError("Can't Connect Playback to Remux(%d:%d)\r\n",drvErr,remuxIndex);
		return DRV_ERR;
	}

	pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId = remuxIndex;

	return DRV_OK;

}

void P_DEMUX_InitConnectCI( void )
{

	VK_memset(CI_CONNECTED_TUNER,0,sizeof(CI_CONNECTED_TUNER));
	return;
}

DRV_Error P_DEMUX_SetConnectCI( HUINT32 ulInputDevice_id, HUINT32 ulincrease)
{
	if(ulInputDevice_id >= TUNER_MAX_NUM)
	{
		return DRV_ERR;
	}

	if( ulincrease == 0 )
	{
		if(	CI_CONNECTED_TUNER[ulInputDevice_id] >= 1)
		{
			CI_CONNECTED_TUNER[ulInputDevice_id] -= 1;
		}
	}
	else
	{
		CI_CONNECTED_TUNER[ulInputDevice_id]++;
	}

	return DRV_OK;
}

DRV_Error P_DEMUX_GetConnectCI( HUINT32 ulInputDevice_id, HUINT32 *pStatus)
{
	if((ulInputDevice_id >= TUNER_MAX_NUM)||(pStatus == NULL))
	{
		return DRV_ERR;
	}

	*pStatus = CI_CONNECTED_TUNER[ulInputDevice_id];


	return DRV_OK;
}

/*************************************************************************************************************************************/

#endif


static DRV_Error P_DEMUX_RouteUnusedCamToDefaultPath_new(HINT32 slotId)
{
	NEXUS_Error rc;
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBandSettings pb_settings;
	NEXUS_RemuxSettings rmx_settings;
	NEXUS_ParserBand co_parser_band;

	pDemux = P_DEMUX_GetInstance();

	if((slotId < 0)||(slotId >= CAM_MAXSLOTID))
	{
		PrintError("Invalid parameter. (slotId = %d)\n", slotId);
		return DRV_ERR;
	}

	if(pDemux->ciInfo[slotId].ciUsageCount == 0 && pDemux->ciInfo[slotId].bShadowOn == FALSE && pDemux->shadowPBPath.bEnabled == FALSE ) /*if currently not used*/
	{
		// Remux setting for externel route
		NEXUS_Remux_GetDefaultSettings(&rmx_settings);
		rmx_settings.parallelOutput = TRUE;
#if defined(CONFIG_REMUX_EXT_CLOCK)
		rmx_settings.outputClock = NEXUS_RemuxClock_eInputBand;
#if defined(CONFIG_USE_RMX_EXT_CLOCK_INSTANCE)
		rmx_settings.remuxClockIBSrc = s_ExtClockInputBand;
#else
		rmx_settings.remuxClockIBSrc = NEXUS_InputBand_e5;
#endif
#else
		rmx_settings.outputClock = NEXUS_RemuxClock_e54Mhz;
#endif
		rmx_settings.allPass = TRUE;
		rmx_settings.bypass= false;
	 	rmx_settings.maxDataRate = 108000000;

		pDemux->rmxHandle = NEXUS_Remux_Open(1, &rmx_settings);
		co_parser_band = P_DEMUX_GetCoParserBandForCI(0);

		NEXUS_ParserBand_GetSettings(co_parser_band, &pb_settings);
		pb_settings.allPass = TRUE;
		pb_settings.acceptNullPackets = FALSE;
		pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
		pb_settings.sourceTypeSettings.inputBand = P_DEMUX_GetInputBand(0);
		pb_settings.transportType = NEXUS_TransportType_eTs;
		rc = NEXUS_ParserBand_SetSettings(co_parser_band, &pb_settings);
		if(rc != NEXUS_SUCCESS)
		{
			NEXUS_Remux_Close(pDemux->rmxHandle);
			PrintError("Failed to set ParserBand for CI (%d)\n", rc);
			return DRV_ERR;
		}

		rc = NEXUS_Remux_AddPidChannel(pDemux->rmxHandle, pDemux->pidChannel[RESERVED_CHANNEL_LIVE1_LOGICAL].pidHandle);
		if(rc != NEXUS_SUCCESS)
		{
			NEXUS_Remux_Close(pDemux->rmxHandle);

			PrintError("Failed to add PID channel to Remux module.(%d)\n", rc);
			return DRV_ERR;
		}
		rc = NEXUS_Remux_Start(pDemux->rmxHandle);
		if(rc != NEXUS_SUCCESS)
		{
			NEXUS_Remux_RemovePidChannel(pDemux->rmxHandle, pDemux->pidChannel[RESERVED_CHANNEL_LIVE1_LOGICAL].pidHandle);
			NEXUS_Remux_Close(pDemux->rmxHandle);
			PrintError("Failed to start Remux module.(%d)\n", rc);
			return DRV_ERR;
		}
		pDemux->ciInfo[slotId].bShadowOn = TRUE;
	}


	return DRV_OK;
}

static DRV_Error P_DEMUX_UnrouteCamFromRequestedPath(HINT32 slotId)
{
	P_DEMUX_Module *pDemux;
	NEXUS_RemuxSettings rmx_settings;

	if((slotId < 0)||(slotId >= CAM_MAXSLOTID))
	{
		PrintError("Invalid parameter. (slotId = %d)\n", slotId);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();

	if(pDemux->ciInfo[slotId].bShadowOn != TRUE)
	{
		NEXUS_Remux_Stop(pDemux->rmxHandle);
		NEXUS_Remux_Close(pDemux->rmxHandle);
		NEXUS_Remux_GetDefaultSettings(&rmx_settings);
		rmx_settings.parallelOutput = false;
		NEXUS_Remux_Close(NEXUS_Remux_Open(1, &rmx_settings));
	}

	return DRV_OK;
}

DRV_Error P_DEMUX_TakeCAMAction(HUINT32 ulDemuxId)
{
	int i;

	for(i=0; i< CAM_MAXSLOTID; i++)
	{
		if(stCamManage[i].status.phyStatus == CAM_Connected_Sts)
		//if(CAM_CONN_STATUS[i][HW_STATUS_PRESENT] == CAM_IN)
		{
			if((stCamManage[i].status.cmdReceived== Explicit_Connection_Req || stCamManage[i].status.cmdReceived== Implicit_Connection_Req)
				&& (stCamManage[i].status.cmdReceived != stCamManage[i].status.cmdExecuted))
			{
				if(stCamManage[i].status.cmdReceived== Explicit_Connection_Req)
				{
					P_DEMUX_RouteCamToRequestedPath(i, stCamManage[i].pathInfo.bIsPlayback, stCamManage[i].pathInfo.inputBand, stCamManage[i].pathInfo.parserBand, ulDemuxId);
					stCamManage[i].status.cmdExecuted = Explicit_Connection_Req;
				}
				else
				{
					P_DEMUX_RouteUnusedCamToDefaultPath_new(i);
					stCamManage[i].status.cmdExecuted = Implicit_Connection_Req;
				}
			}
		}
		else //CAM_OUT
		{
			if((stCamManage[i].status.cmdReceived== Explicit_Connection_Req || stCamManage[i].status.cmdReceived== Implicit_Connection_Req)
				&& (stCamManage[i].status.cmdExecuted!=Explicit_Disconnection_Req && stCamManage[i].status.cmdExecuted!=Implicit_Disconnection_Req))
			{
				if(stCamManage[i].status.cmdReceived== Explicit_Connection_Req)
				{
					P_DEMUX_UnrouteCamFromRequestedPath(i);
					stCamManage[i].status.cmdExecuted = Explicit_Disconnection_Req;
				}
				else
				{
					P_DEMUX_UnrouteUnusedCamToDefaultPath(i);
					stCamManage[i].status.cmdExecuted = Implicit_Disconnection_Req;
				}
			}

		}
	}
	return DRV_OK;
}


DRV_Error P_DEMUX_UpdateCAMPath(CamManagementMsg Cmd, HINT32 slotId, HBOOL bIsPlayback, NEXUS_InputBand inputBand, NEXUS_ParserBand parserBand, HUINT32 ulDemuxId)
{

	if((slotId < 0)||(slotId >= CAM_MAXSLOTID))
	{
		PrintError("Invalid parameter. (slotId = %d)\n", slotId);
		return DRV_ERR;
	}

	if(Cmd == Implicit_Connection_Req && stCamManage[slotId].status.cmdReceived == Explicit_Connection_Req)
		return DRV_OK;

	if(Cmd == Explicit_Connection_Req)
	{
		stCamManage[slotId].pathInfo.bIsPlayback = bIsPlayback;
		stCamManage[slotId].pathInfo.inputBand = inputBand;
		stCamManage[slotId].pathInfo.parserBand = parserBand;
		stCamManage[slotId].pathInfo.bIsValid = TRUE;
	}
	else if(Cmd == Explicit_Disconnection_Req)
	{
		stCamManage[slotId].pathInfo.bIsValid = FALSE;
	}

	if(Cmd < CAM_Connected_Sts)
	{
		stCamManage[slotId].status.cmdReceived = Cmd;
	}
	else if(Cmd < CAM_Msg_Max)
	{
		stCamManage[slotId].status.phyStatus = Cmd;
	}
	else
	{
		PrintError("ERROR: Unknown MSG!\n");
		return DRV_ERR;
	}
	P_DEMUX_TakeCAMAction(ulDemuxId);
	return DRV_OK;
}

static void P_DEMUX_NotifyInsertCamForDefaultPath(unsigned short usSlotId, unsigned short flag)
{
	if(usSlotId >= CAM_MAXSLOTID)
	{
		PrintError("Invalid parameter. (slotId = %d)\n", usSlotId);
		return;
	}

	VK_SEM_Get(ciLock);
	if(flag == DI_CI_ENABLED)
	{
		P_DEMUX_UpdateCAMPath(CAM_Connected_Sts, usSlotId, 0,0,0,0);
	}
	else if(flag == DI_CI_EXTRACTED)
	{
		P_DEMUX_UpdateCAMPath(CAM_Disconnected_Sts, usSlotId, 0,0,0,0);

	}
	VK_SEM_Release(ciLock);
	return;
}
#endif

#if defined(CONFIG_CI)
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
static DRV_Error P_DEMUX_OpenShadowPlaybackPath(HUINT32 ulDemuxId)
{
	NEXUS_Error	rc;
	P_DEMUX_Module *pDemux;
	NEXUS_RemuxSettings rmx_settings;
	NEXUS_PidChannelHandle	playbackPidChannel = NULL;
	DRV_Error drvErr = DRV_ERR;

	pDemux = P_DEMUX_GetInstance();

	if(pDemux->shadowPBPath.bEnabled == FALSE)
	{
		NEXUS_Remux_GetDefaultSettings(&rmx_settings);
		rmx_settings.parallelOutput = FALSE;
		rmx_settings.outputClock = NEXUS_RemuxClock_e27Mhz_VCXO_A;
		rmx_settings.maxDataRate = 108000000;
		rmx_settings.allPass = TRUE;
		rmx_settings.bypass= false;

		pDemux->shadowPBPath.hRmx = NEXUS_Remux_Open(0, &rmx_settings);
		if(pDemux->shadowPBPath.hRmx == NULL)
		{
			PrintError("Failed to NEXUS_Remux_Open.\n");
			return DRV_ERR;
		}

		drvErr = DRV_PVR_RequestAllpassChannel(ulDemuxId, &playbackPidChannel);
		if((drvErr != DRV_OK)||(playbackPidChannel == NULL))
		{
			PrintError("Failed to DRV_PVR_RequestAllpassChannel (drvErr:%d, playbackPidChannel:0x%x).\n", drvErr, playbackPidChannel);
			return DRV_ERR;
		}

		if(playbackPidChannel != NULL)
		{
			rc = NEXUS_Remux_AddPidChannel(pDemux->shadowPBPath.hRmx, playbackPidChannel);
			if(rc != NEXUS_SUCCESS)
			{
				NEXUS_Remux_Close(pDemux->shadowPBPath.hRmx);
				PrintError("Failed to add PID channel to Remux module.(%d)\n", rc);
				return DRV_ERR;
			}
			rc = NEXUS_Remux_Start(pDemux->shadowPBPath.hRmx);
			if(rc != NEXUS_SUCCESS)
			{
				NEXUS_Remux_RemovePidChannel(pDemux->shadowPBPath.hRmx, playbackPidChannel);
				NEXUS_Remux_Close(pDemux->shadowPBPath.hRmx);
				PrintError("Failed to start Remux module.(%d)\n", rc);
				return DRV_ERR;
			}
		}
		else
		{
			NEXUS_Remux_Close(pDemux->shadowPBPath.hRmx);
			PrintError("Failed to add PID channel to Remux module.\n");
			return DRV_ERR;
		}

		pDemux->shadowPBPath.bEnabled = TRUE;
	}
	else if(pDemux->shadowPBPath.bEnabled == TRUE && pDemux->shadowPBPath.ulCount < 2)
	{
		pDemux->shadowPBPath.ulCount++;
	}

	return DRV_OK;

}

//Should be called only in ciLock semaphore protection.
static DRV_Error P_DEMUX_CloseShadowPlaybackPath(void)
{
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();
	if(pDemux->shadowPBPath.bEnabled != FALSE)
	{
		if(pDemux->shadowPBPath.ulCount > 0)
		{
			pDemux->shadowPBPath.ulCount--;
		}
		else
		{
			pDemux->shadowPBPath.bEnabled = FALSE;
			NEXUS_Remux_Stop(pDemux->shadowPBPath.hRmx);
			NEXUS_Remux_Close(pDemux->shadowPBPath.hRmx);
		}
	}
	return DRV_OK;
}
#endif
#endif

#if defined(CONFIG_BCM_MTSIF)
static DRV_Error P_DEMUX_SetMTSIFPath(DI_DEMUX_INPUTDEVICETYPE eProducer, HUINT32 ulInputDeviceId, NEXUS_ParserBand eParserBand, HBOOL bPathEnable, HBOOL bAllPathEnable)
{
	NEXUS_PlatformConfiguration tPlatformConfig;
	NEXUS_FrontendHandle pvFrontendHandle = NULL;
	NEXUS_FrontendUserParameters tUserParams;
	NEXUS_ParserBandSettings tParserBandSetting;
	NEXUS_Error eNexusRet;
#if (NEXUS_VERSION >= 1290)
#if defined(CONFIG_DEMOD_TSMF)
	NEXUS_TsmfHandle pvTsmfHandle = NULL;
#endif
#endif

	if(ulInputDeviceId >= NEXUS_MAX_FRONTENDS)
	{
		PrintError("Invalid frontend index (ulInputDeviceId=%d)\n", ulInputDeviceId);
		return DRV_ERR;
	}

	NEXUS_Platform_GetConfiguration(&tPlatformConfig);
	pvFrontendHandle = tPlatformConfig.frontend[ulInputDeviceId];
	if(pvFrontendHandle == NULL)
	{
		PrintError("Invalid frontend handle (pvFrontendHandle=%p)\n", pvFrontendHandle);
		return DRV_ERR;
	}

	NEXUS_Frontend_GetUserParameters(pvFrontendHandle, &tUserParams);
	NEXUS_ParserBand_GetSettings(eParserBand, &tParserBandSetting);
	if(tUserParams.isMtsif)
	{
		tParserBandSetting.sourceTypeSettings.inputBand =  P_DEMUX_GetUnusedInputBand(); /* Disable inputband */
		tParserBandSetting.acceptNullPackets = FALSE;

		if(bPathEnable == TRUE)
		{
			if(eProducer == DI_DEMUX_INPUT_NONE)
			{
				tParserBandSetting.sourceType = NEXUS_ParserBandSourceType_eMtsif;
				tParserBandSetting.sourceTypeSettings.mtsif = NULL;
			}
			else
			{
				if(bAllPathEnable == TRUE)
					tParserBandSetting.allPass = TRUE;
				else
					tParserBandSetting.allPass = FALSE;

#if (NEXUS_VERSION >= 1290)
#if defined(CONFIG_DEMOD_TSMF)
				tParserBandSetting.sourceType = NEXUS_ParserBandSourceType_eTsmf;
				DRV_CH_TSMF_GetHandle(ulInputDeviceId, (HUINT32*)&pvTsmfHandle);
				tParserBandSetting.sourceTypeSettings.tsmf = pvTsmfHandle;
#else
				tParserBandSetting.sourceType = NEXUS_ParserBandSourceType_eMtsif;
				tParserBandSetting.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(pvFrontendHandle);
				tParserBandSetting.sourceTypeSettings.remux = NULL;
#endif
#else
				tParserBandSetting.sourceType = NEXUS_ParserBandSourceType_eMtsif;
				tParserBandSetting.sourceTypeSettings.mtsif = pvFrontendHandle;
#endif
			}
		}
		else
		{
			tParserBandSetting.sourceType = NEXUS_ParserBandSourceType_eMtsif;
			tParserBandSetting.sourceTypeSettings.mtsif = NULL;
		}
	}
	else
	{
		PrintError("Frontend can't support MTSIF\n");
		return DRV_ERR;
	}

	tParserBandSetting.transportType = NEXUS_TransportType_eTs;

	eNexusRet = NEXUS_ParserBand_SetSettings(eParserBand, &tParserBandSetting);
	if(eNexusRet != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_ParserBand_SetSettings fail (ret=%d)\n", eNexusRet);
		return DRV_ERR;
	}

	eNexusRet = NEXUS_Frontend_ReapplyTransportSettings(pvFrontendHandle);
	if(eNexusRet != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Frontend_ReapplyTransportSettings fail (ret=%d)\n", eNexusRet);
		return DRV_ERR;
	}

	return DRV_OK;
}
#endif

NEXUS_Error DRV_DEMUX_GetScramblingStatus( NEXUS_PidChannelHandle ulPidChannelHandle, NEXUS_PidChannelScramblingStatus *pStatus )
{
#if 0 //defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	int i;
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(gTscCheck.Lock);

	for(i=0; i<NUM_TSC_CHECK_RES; i++)
	{
		if(gTscCheck.stResource[i].bEnabled == TRUE && gTscCheck.stResource[i].pidHandle == ulPidChannelHandle)
		{
			break;
		}
	}
	if(i >= NUM_TSC_CHECK_RES)
	{
		VK_SEM_Release(gTscCheck.Lock);
		PrintError("No slot found.\n");
		return -1;
	}

	if(gTscCheck.stResource[i].TSCValue == 0x00)
	{
		DI_UART_Print("[0x%lx] Clear\n",ulPidChannelHandle);
		pStatus->pidExists = true;
		pStatus->mode = NEXUS_PidChannelScramblingMode_eFree;
		pStatus->type = NEXUS_PidChannelScramblingType_eNone;
	}
	else if(gTscCheck.stResource[i].TSCValue == 0xFF)
	{
		DI_UART_Print("[0x%lx] Unknown\n",ulPidChannelHandle);
		pStatus->pidExists = false;
		pStatus->mode = NEXUS_PidChannelScramblingMode_eFree;
		pStatus->type = NEXUS_PidChannelScramblingType_eNone;
	}
	else
	{
		DI_UART_Print("[0x%lx] Scrambled\n",ulPidChannelHandle);
		pStatus->pidExists = true;
		pStatus->mode = NEXUS_PidChannelScramblingMode_eTsScrambled;
		pStatus->type = NEXUS_PidChannelScramblingType_eOdd;
	}
	VK_SEM_Release(gTscCheck.Lock);
	return 0;
#else
	return NEXUS_PidChannel_GetScramblingStatus(ulPidChannelHandle, pStatus);
#endif
}


DRV_Error DRV_DEMUX_SetSectionDbgMode(HUINT32 SectionType, HBOOL bOn)
{
	if(SectionType >= 32)
		return DRV_ERR;
	if(bOn == 1)
	{
		SectionDbgMode = SectionDbgMode | (1<<SectionType);
	}
	else
	{
		SectionDbgMode = SectionDbgMode & ~(1<<SectionType);
	}
	return DRV_OK;
}

DRV_Error DRV_DEMUX_BackupCallback(HBOOL backup)
{
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();
	if(backup)
		gBackup_callback = pDemux->callback;
	else
		pDemux->callback = gBackup_callback;

	return DRV_OK;
}


int DRV_DEMUX_GetFirstPlaybackParserIndex(void)
{
	return LIVE_DEMUX_NUM;
}

HBOOL DRV_DEMUX_IsConnected(HUINT32 unDeviceId)
{
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();

	return pDemux->dmxDevice[unDeviceId].bIsUsing;
}

DRV_Error DRV_DEMUX_PrintFilterInfo(HUINT32 ulDemuxId)
{
	P_DEMUX_Module *pDemux;
	HUINT32 filterIndex;
#if defined(SAVE_FILTER_INFO)
	HUINT32 i;
#endif

	pDemux = P_DEMUX_GetInstance();

	DI_UART_Print("----<Enabled Message Filter List----------\n");
	for(filterIndex=0; filterIndex< MAX_SECTION_FILTER_NUM; filterIndex++)
	{
		VK_SEM_Get(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
		if(pDemux->msgFilter[filterIndex].status == DRV_DEMUX_RESOURCE_ENABLED && pDemux->pidChannel[pDemux->msgFilter[filterIndex].channelIndex].demuxId == ulDemuxId)
		{
#if defined(SAVE_FILTER_INFO)
			DI_UART_Print("Filter[%03d] TYPE=%s  Pid=0x%04x(PidChannel=%03d)  FilterLength=%02d requestId=0x%x\n",filterIndex, GETFILTERTYPE(pDemux->msgFilter[filterIndex].filterType),
				                            filterDbgInfo[filterIndex].usPid, pDemux->msgFilter[filterIndex].channelIndex, filterDbgInfo[filterIndex].ulFilterLen, filterDbgInfo[filterIndex].ulRequestId);
#if defined(SECTION_FILTER_DEBUGGING)
			if(pDemux->msgFilter[filterIndex].filterType == P_DEMUX_PsiFilter
				||pDemux->msgFilter[filterIndex].filterType == P_DEMUX_DEBUG_PsiFilter )
#else
			if(pDemux->msgFilter[filterIndex].filterType == P_DEMUX_PsiFilter)
#endif
			{
				DI_UART_Print("		Value: ");
				for(i=0;i<filterDbgInfo[filterIndex].ulFilterLen; i++)
					DI_UART_Print("[%02d]=%02x ",i,filterDbgInfo[filterIndex].ucValue[i]);
				DI_UART_Print("\n");
				DI_UART_Print("		Mask : ");
				for(i=0;i<filterDbgInfo[filterIndex].ulFilterLen; i++)
					DI_UART_Print("[%02d]=%02x ",i,filterDbgInfo[filterIndex].ucMask[i]);
				DI_UART_Print("\n");
				DI_UART_Print("		Excl : ");
				for(i=0;i<filterDbgInfo[filterIndex].ulFilterLen; i++)
					DI_UART_Print("[%02d]=%02x ",i,filterDbgInfo[filterIndex].ucExclusion[i]);
				DI_UART_Print("\n");
			}
#else
			DI_UART_Print("Filter[%03d] TYPE=%s  PidChannel=%03d\n",filterIndex, GETFILTERTYPE(pDemux->msgFilter[filterIndex].filterType),
				                            pDemux->msgFilter[filterIndex].channelIndex);
#endif
			DI_UART_Print("---------------------------------------\n");
		}
		VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
	}
	return DRV_OK;
}

DRV_Error DRV_DEMUX_PrintInfo(HUINT32 demux_id)
{
	P_DEMUX_Module *pDemux;
	HINT32 target = 0;
	HINT32 i;
	char* contents = "NULL";
	HINT32 audio_pidchannel_index=DRV_DMX_NUM_PID_CHANNEL;
	HINT32 video_pidchannel_index=DRV_DMX_NUM_PID_CHANNEL;
	HINT32 pcr_pidchannel_index=DRV_DMX_NUM_PID_CHANNEL;
	HINT32 audioDesc_pidchannel_index=DRV_DMX_NUM_PID_CHANNEL;
	HUINT32 recording_pidchannel_num=0;
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	HUINT32 ulVideoPrimerIndex=0xff;
#endif
	char* strPidchannelStatus[3] = {"FREE","ALLOCATED","ENABLED"};

	HAPPY(demux_id);
	HAPPY(strInputDev);
	HAPPY(strOutputDev);
	HAPPY(strPidchannelStatus);
	HAPPY(target);

	pDemux = P_DEMUX_GetInstance();

	/* Getting channel index */
	for(i=0; i<DRV_DMX_NUM_PID_CHANNEL;i++)
	{
		if(pDemux->pidChannel[i].pidHandle == s_hAudioPIDChannel &&
			pDemux->pidChannel[i].channelType <=DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF &&
			pDemux->pidChannel[i].channelType >=DI_DEMUX_CHANNELTYPE_AUDIO )
		{
			audio_pidchannel_index = i;
		}

		if(pDemux->pidChannel[i].pidHandle == s_hVideoPIDChannel &&
			pDemux->pidChannel[i].channelType== DI_DEMUX_CHANNELTYPE_VIDEO)
		{
			video_pidchannel_index = i;
		}

		if(pDemux->pidChannel[i].pidHandle == s_hPcrPIDChannel &&
			pDemux->pidChannel[i].channelType== DI_DEMUX_CHANNELTYPE_PCR)
		{
			pcr_pidchannel_index = i;
		}

		if(pDemux->pidChannel[i].pidHandle == s_hDescriptionPIDChannel &&
			pDemux->pidChannel[i].channelType== DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION)
		{
			audioDesc_pidchannel_index = i;
		}

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
		if(DRV_VIDEO_IsPrimerStarted())
		{
			DRV_VIDEO_GetPrimerIndex(0, &ulVideoPrimerIndex);
			if(ulVideoPrimerIndex != 0xff)
			{
				video_pidchannel_index = s_stDemuxPrimerPidChannelInfo[ulVideoPrimerIndex].ulVideoPrimerPIDChannel;
				pcr_pidchannel_index = s_stDemuxPrimerPidChannelInfo[ulVideoPrimerIndex].ulPcrPrimerPIDChannel;
			}
		}
#endif
 		DI_UART_Print("PID Channel(%3d)(%3d)-status(%d)-DemuxId(%2d): PID(0x%04x) handle(0x%8x) channelType(%2d) subinfo(%d)\n",
 						i,
 						pDemux->pidChannel[i].pidHandle?P_DEMUX_GetPhysicalPidChannelIndex(pDemux->pidChannel[i].pidHandle):0,
 						pDemux->pidChannel[i].status,
 						pDemux->pidChannel[i].demuxId,
 						pDemux->pidChannel[i].pid,
 						pDemux->pidChannel[i].pidHandle,
 						pDemux->pidChannel[i].channelType,
 						pDemux->pidChannel[i].channelsubInfo);

	}

	DI_UART_Print("===============Demux path information================\n");
	DI_UART_Print("Number of DEMUX : [%d]\n\n",MAX_DEMUX_NUM);
	for(i=0; i<MAX_DEMUX_NUM; i++)
	{
		if(pDemux->dmxDevice[i].bIsUsing == TRUE)
		{
			/* dmxDevice path info */
			DI_UART_Print("DEMUX %d :\t%s[%d]->Parserband[%d]->%s\n",i,strInputDev[pDemux->dmxDevice[i].pathInfo.etProducer],pDemux->dmxDevice[i].pathInfo.ulInputDevice_id,pDemux->dmxDevice[i].pathInfo.parserBand,strOutputDev[pDemux->dmxDevice[i].pathInfo.etConsumer]);
		}
		else
		{
			DI_UART_Print("DEMUX %d :\tis not being used now\n",i);
		}
	}

	DI_UART_Print("\n");

	/* video pid channel information */
	DI_UART_Print("\n============Video pid channel information============\n");
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	DI_UART_Print("NOTE :\n\tCurrently FCC primer is %s.\n\tRefer to FCC Primer information section.\n\n",DRV_VIDEO_IsPrimerStarted() ? "STARTED":"NOT started");
#endif
	if(video_pidchannel_index != DRV_DMX_NUM_PID_CHANNEL)
	{
		DI_UART_Print("Video PID:		0x%04x\n", pDemux->pidChannel[video_pidchannel_index].pid);
		DI_UART_Print("Video PID Channel:	%d\n", video_pidchannel_index);
		DI_UART_Print("Channel Type:		%d\n", pDemux->pidChannel[video_pidchannel_index].channelType);
		DI_UART_Print("Resource Status:	%s\n", strPidchannelStatus[pDemux->pidChannel[video_pidchannel_index].status]);
	}
	else
	{
		DI_UART_Print("\nResource Status:	Not allocated Video pid channel\n");
	}


	DI_UART_Print("\n============Audio pid channel information============\n");

	DI_UART_Print("MAIN Audio \n\n");
	
	if(audio_pidchannel_index != DRV_DMX_NUM_PID_CHANNEL)
	{
		DI_UART_Print("Audio PID:		0x%04x\n",pDemux->pidChannel[audio_pidchannel_index].pid);
		DI_UART_Print("Audio PID Channel:	%d\n", audio_pidchannel_index);
		DI_UART_Print("Channel Type:		%d\n", pDemux->pidChannel[audio_pidchannel_index].channelType);
		DI_UART_Print("Resource Status:	%s\n", strPidchannelStatus[pDemux->pidChannel[audio_pidchannel_index].status]);
	}
	else
	{
		DI_UART_Print("Resource Status:	Not allocated Audio pid channel\n");
	}

	DI_UART_Print("\nSUB AUDIO ( Audio Description )\n\n");
	if(audioDesc_pidchannel_index != DRV_DMX_NUM_PID_CHANNEL)
	{
		DI_UART_Print("Audio Desc PID:		0x%04x\n",pDemux->pidChannel[audioDesc_pidchannel_index].pid);
		DI_UART_Print("Audio Desc PID Channel:	%d\n", audioDesc_pidchannel_index);
		DI_UART_Print("Channel Type:		%d\n", pDemux->pidChannel[audioDesc_pidchannel_index].channelType);
		DI_UART_Print("Resource Status:	%s\n", strPidchannelStatus[pDemux->pidChannel[audioDesc_pidchannel_index].status]);
	}
	else
	{
		DI_UART_Print("Resource Status:	Not allocated Audio Desciption pid channel\n");
	}

	DI_UART_Print("\n=============PCR pid channel information=============\n");
	if(pcr_pidchannel_index != DRV_DMX_NUM_PID_CHANNEL)
	{
		DI_UART_Print("Pcr PID:		0x%04x\n", pDemux->pidChannel[pcr_pidchannel_index].pid);
		DI_UART_Print("Pcr PID Channel:	%d\n",  pcr_pidchannel_index);
		DI_UART_Print("Channel Type:		%d\n", pDemux->pidChannel[pcr_pidchannel_index].channelType);
		DI_UART_Print("Resource Status:	%s\n", strPidchannelStatus[pDemux->pidChannel[pcr_pidchannel_index].status]);
	}
	else
	{
		DI_UART_Print("\nResource Status:	Not allocated PCR pid channel\n");
	}

	DI_UART_Print("\n=============Recording pid channel information=======\n");

	for(i=0; i<DRV_DMX_NUM_PID_CHANNEL; i++)
	{
		if(pDemux->pidChannel[i].channelType == DI_DEMUX_CHANNELTYPE_RECORD)
		{
			DI_UART_Print("Recording DemuxId: [%d], PID Channel Index : [%d], PID :[0x%04x], parserBand [%d]\n",
						pDemux->pidChannel[i].demuxId,
						i,
						pDemux->pidChannel[i].pid,
						pDemux->dmxDevice[pDemux->pidChannel[i].demuxId].pathInfo.parserBand);
		}
	}

	if(recording_pidchannel_num == 0)
	{
		DI_UART_Print("Now It's not recording\n");
	}

	DI_UART_Print("\n===============Capability information===============\n");
	DI_UART_Print("Capability(Logical):	0x%x\n", pDemux->dmxDevice[target].stCapability.ulCapability);
	DI_UART_Print("Num of PES filter: %d\n",  pDemux->dmxDevice[target].stCapability.NumOfPESFilter);
	DI_UART_Print("Num of Section filter: %d\n",  pDemux->dmxDevice[target].stCapability.NumOfSectoinFilter);
	DI_UART_Print("Num of Primer: %d\n",  pDemux->dmxDevice[target].stCapability.NumOfPrimer);
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	DI_UART_Print("\n===============FCC Primer information===============\n");
	DI_UART_Print("Num of NumOfAvailableStcChannel: %d\n",  pDemux->dmxDevice[target].stCapability.NumOfAvailableStcChannel);

	for(i=0;i<DEMUX_PRIMER_MAX_COUNT;i++)
	{
		DI_UART_Print("PrimerPIDChannel[%d][%s]%s	Demux ID : %d,  Video PID : 0x%x, Audio PID : 0x%x, PCR PID : 0x%x\n",
				i,
				s_stDemuxPrimerPidChannelInfo[i].bVideoPrimerIsEnable ? "ENABLE" : "DISABLE",
				(i==ulVideoPrimerIndex) ? "[DECODING]":"[        ]",
				s_stDemuxPrimerPidChannelInfo[i].ulDemuxId,
				s_stDemuxPrimerPidChannelInfo[i].usPid[0],
				s_stDemuxPrimerPidChannelInfo[i].usPid[1],
				s_stDemuxPrimerPidChannelInfo[i].usPid[2]);
	}

#endif
	DI_UART_Print("\n================Filter information==================\n");
	DI_UART_Print("channelIndex || physicalChannelIndex || Filter Type \n");
	for(i=0; i<MAX_SECTION_FILTER_NUM; i++)
	{
		if(pDemux->msgFilter[i].status == DRV_DEMUX_RESOURCE_ENABLED)
		{
			DI_UART_Print("%3d", pDemux->msgFilter[i].channelIndex);
			DI_UART_Print("%15d", pDemux->msgFilter[i].physicalChannelIndex);
			if(pDemux->msgFilter[i].filterType == P_DEMUX_PesFilter)
				contents = "PesFilter";
			else if(pDemux->msgFilter[i].filterType == P_DEMUX_PsiFilter)
				contents = "PsiFilter";
			//else if (pDemux->msgFilter[i].filterType == P_DEMUX_TsFilter)
			//	contents = "TsFilter";
			DI_UART_Print("%32s\n", contents);
		}
	}

		DI_UART_Print("\n================Video PidChannel information==================\n");
		DI_UART_Print("Index | pidHandle || PID || pidCHIndex || parserBand || transportType || originalTransportType ||remappedPid || enabled || playback || playbackIndex || continuityCountErrors \n");

		/* Getting Pidchannel information */
		for(i=0; i<DRV_DMX_NUM_PID_CHANNEL;i++)
		{
			/* CC is based on paserband CC or playpump CC */
			if(pDemux->pidChannel[i].status == DRV_DEMUX_RESOURCE_ENABLED)
			{
				if((pDemux->pidChannel[i].channelType == DI_DEMUX_CHANNELTYPE_VIDEO) || (pDemux->pidChannel[i].channelType == DI_DEMUX_CHANNELTYPE_RECORD) )
				{
					NEXUS_PidChannelStatus pidChannelStatus;
					NEXUS_Error errNexusCode = NEXUS_SUCCESS;

					errNexusCode = NEXUS_PidChannel_GetStatus(pDemux->pidChannel[i].pidHandle, &pidChannelStatus);
					if(errNexusCode != NEXUS_SUCCESS )
					{
						PrintError("NEXUS_PidChannel_GetStatus() Error, errCode = %d\n", errNexusCode);
					}
					else
					{
						DI_UART_Print("%5d || 0x%8x || 0x%04x || 0x%x || 0x%x || 0x%x || 0x%x || 0x%04x || 0x%x ||  0x%x || 0x%04x || %d \n",
							i, pDemux->pidChannel[i].pidHandle,
							pidChannelStatus.pid, pidChannelStatus.pidChannelIndex,
							pidChannelStatus.parserBand, pidChannelStatus.transportType, pidChannelStatus.originalTransportType,
							pidChannelStatus.remappedPid, pidChannelStatus.enabled, pidChannelStatus.playback, pidChannelStatus.playbackIndex,
							pidChannelStatus.continuityCountErrors);
					}
				}
			}
		}
	return DRV_OK;
}

NEXUS_PidChannelHandle DRV_DEMUX_GetVideoPidChannel(HUINT32 unDeviceId)
{
#if !defined(CONFIG_PIP)
	BSTD_UNUSED(unDeviceId);
	return s_hVideoPIDChannel;
#else
	if(unDeviceId >= MAX_DECORD_NUM)
		return NULL;

	if(unDeviceId == 0)
		return s_hVideoPIDChannel;
	else
		return s_hPIPVideoPIDChannel;
#endif
}

NEXUS_PidChannelHandle DRV_DEMUX_GetAudioPidChannel(HUINT32 unDeviceId)
{
	if(unDeviceId == 1)
	{
		return s_hDescriptionPIDChannel;
	}
	return s_hAudioPIDChannel;
}

NEXUS_PidChannelHandle DRV_DEMUX_GetPcrPidChannel(void)
{
	return s_hPcrPIDChannel;
}

#if defined(CONFIG_PIP)
NEXUS_PidChannelHandle DRV_DEMUX_GetPIPPcrPidChannel(void)
{
	return s_hPIPPcrPIDChannel;
}
#endif


#if defined(CONFIG_TRANSCODER)
NEXUS_PidChannelHandle DRV_DEMUX_GetPidChannelForTranscode(HUINT32 ulDemuxId,DRV_DEMUX_CHANNELTYPE drvChannelType)
{
	P_DEMUX_Module *pDemux;
	HUINT8 channel_index;
	NEXUS_PidChannelHandle getPidChannel = NULL;

	DI_DEMUX_CHANNELTYPE channelType;
	DRV_Error	drvRet = 0;

	drvRet = P_DEMUX_ConvertDRVtoDIChannelType(drvChannelType,&channelType);
	if(drvRet != DRV_OK)
	{
		return NULL;
	}

	pDemux = P_DEMUX_GetInstance();

	for(channel_index = 0; channel_index < DRV_DMX_NUM_PID_CHANNEL; channel_index++)
	{
		VK_SEM_Get(pDemux->pidChannel[channel_index].lock);
		if(pDemux->pidChannel[channel_index].status == DRV_DEMUX_RESOURCE_ENABLED &&
			pDemux->pidChannel[channel_index].demuxId == ulDemuxId &&
			pDemux->pidChannel[channel_index].channelType == channelType)
		{
			getPidChannel = pDemux->pidChannel[channel_index].pidHandle;
			VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
			break;
		}
		VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
	}
	return getPidChannel;
}

NEXUS_PidChannelHandle DRV_DEMUX_GetPidChannelWithPid(HUINT32 ulDemuxId,DRV_DEMUX_CHANNELTYPE drvChannelType, HUINT16 pid)
{
	P_DEMUX_Module *pDemux;
	HUINT8 channel_index;
	NEXUS_PidChannelHandle getPidChannel = NULL;

	DI_DEMUX_CHANNELTYPE channelType;
	DRV_Error	drvRet = 0;

	drvRet = P_DEMUX_ConvertDRVtoDIChannelType(drvChannelType,&channelType);
	if(drvRet != DRV_OK)
	{
		return NULL;
	}

	pDemux = P_DEMUX_GetInstance();

	for(channel_index = 0; channel_index < DRV_DMX_NUM_PID_CHANNEL; channel_index++)
	{
		VK_SEM_Get(pDemux->pidChannel[channel_index].lock);
		if(pDemux->pidChannel[channel_index].status == DRV_DEMUX_RESOURCE_ENABLED &&
			pDemux->pidChannel[channel_index].demuxId == ulDemuxId &&
			pDemux->pidChannel[channel_index].channelType == channelType &&
			pDemux->pidChannel[channel_index].pid == pid)
		{
			getPidChannel = pDemux->pidChannel[channel_index].pidHandle;
			VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
			break;
		}
		VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
	}
	return getPidChannel;
}
#endif


NEXUS_PidChannelHandle DRV_DEMUX_GetRecordPidChannel(HUINT32 channelIndex)
{
	P_DEMUX_Module *pDemux;

	if(channelIndex >= DRV_DMX_NUM_PID_CHANNEL)
	{
		PrintError(" channelIndex %d is invalid\n", channelIndex);
		return NULL;
	}

	pDemux = P_DEMUX_GetInstance();

	if((pDemux->pidChannel[channelIndex].channelType != DI_DEMUX_CHANNELTYPE_RECORD) && (pDemux->pidChannel[channelIndex].channelType != DI_DEMUX_CHANNELTYPE_ALLPASS))
	{
		PrintError(" channel type %d is invalid\n", pDemux->pidChannel[channelIndex].channelType);
		return NULL;
	}

	return pDemux->pidChannel[channelIndex].pidHandle;
}

DRV_Error DRV_DEMUX_GetRecordPidChannelInfo(HUINT32 channelIndex, HUINT32 *pulDemuxId, HUINT16 *pusPid)
{
	P_DEMUX_Module *pDemux;

	if(channelIndex >= DRV_DMX_NUM_PID_CHANNEL)
	{
		PrintError("[%s] channelIndex %d is invalid!\n", __func__, channelIndex);
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(pulDemuxId == NULL)
	{
		PrintError("[%s] Invalid Parameter: pulDemuxId is NULL!\n", __func__);
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(pusPid == NULL)
	{
		PrintError("[%s] Invalid Parameter: pusPid is NULL!\n", __func__);
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(pDemux->pidChannel[channelIndex].lock);
	if(pDemux->pidChannel[channelIndex].channelType != DI_DEMUX_CHANNELTYPE_RECORD)
	{
		PrintError("[%s] channel type %d is invalid\n", __func__, pDemux->pidChannel[channelIndex].channelType);
		VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
		return DRV_ERR;
	}

	*pulDemuxId = pDemux->pidChannel[channelIndex].demuxId;
	*pusPid = pDemux->pidChannel[channelIndex].pid;

	PrintDebug("[%s] channelindex(%d) : demuxId(%d), pid(0x%x)\n", __func__, channelIndex, pDemux->pidChannel[channelIndex].demuxId, pDemux->pidChannel[channelIndex].pid);

	VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);

	return DRV_OK;
}

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
DRV_Error DRV_DEMUX_GetVideoPrimerPidChannel(HUINT32 ulPrimerIndex, NEXUS_PidChannelHandle *hPidChannel)
{
	P_DEMUX_Module *pDemux;
	HUINT32	ulPrimerPidChannelIndex = 0;

	if(ulPrimerIndex >= DEMUX_PRIMER_MAX_COUNT)
	{
		PrintError("ERROR : channelIndex %d is invalid\n", ulPrimerIndex);
		return DRV_ERR;
	}

	if(hPidChannel == NULL)
	{
		PrintError("[%s] ERROR : hPidChannel is NULL \n", __func__);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();
	if(pDemux == NULL)
	{
		PrintError(" pDemux is NULL!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	ulPrimerPidChannelIndex = s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulVideoPrimerPIDChannel;

	VK_SEM_Get(pDemux->pidChannel[ulPrimerPidChannelIndex].lock);
	if(pDemux->pidChannel[ulPrimerPidChannelIndex].channelType != DI_DEMUX_CHANNELTYPE_VIDEO_PRIMER)
	{
		VK_SEM_Release(pDemux->pidChannel[ulPrimerPidChannelIndex].lock);
		PrintError("ERROR : channel type %d is invalid\n", pDemux->pidChannel[ulPrimerPidChannelIndex].channelType);
		return DRV_ERR;
	}

	*hPidChannel =  pDemux->pidChannel[ulPrimerPidChannelIndex].pidHandle;
	VK_SEM_Release(pDemux->pidChannel[ulPrimerPidChannelIndex].lock);

	return DRV_OK;
}
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
DRV_Error DRV_DEMUX_GetAudioPrimerPidChannel(HUINT32 ulPrimerIndex, NEXUS_PidChannelHandle *hPidChannel)
{
	P_DEMUX_Module *pDemux;
	HUINT32	ulPrimerPidChannelIndex = 0;

	if(ulPrimerIndex >= DEMUX_PRIMER_MAX_COUNT)
	{
		PrintError("ERROR : channelIndex %d is invalid\n", ulPrimerIndex);
		return DRV_ERR;
	}

	if(hPidChannel == NULL)
	{
		PrintError("[%s] ERROR : hPidChannel is NULL \n", __func__);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();
	if(pDemux == NULL)
	{
		PrintError(" pDemux is NULL!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	ulPrimerPidChannelIndex = s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulAudioPrimerPIDChannel;

	VK_SEM_Get(pDemux->pidChannel[ulPrimerPidChannelIndex].lock);
	if(pDemux->pidChannel[ulPrimerPidChannelIndex].channelType != DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER)
	{
		VK_SEM_Release(pDemux->pidChannel[ulPrimerPidChannelIndex].lock);
		PrintError("ERROR : channel type %d is invalid\n", pDemux->pidChannel[ulPrimerPidChannelIndex].channelType);
		return DRV_ERR;
	}

	*hPidChannel =  pDemux->pidChannel[ulPrimerPidChannelIndex].pidHandle;
	VK_SEM_Release(pDemux->pidChannel[ulPrimerPidChannelIndex].lock);

	return DRV_OK;
}
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
NEXUS_StcChannelHandle DRV_DEMUX_GetPrimerStcChannelHandle(HUINT32 ulPrimerIndex)
{
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();
	if(pDemux == NULL)
	{
		PrintError(" pDemux is NULL!\n");
		return NULL;
	}

	if(primerStcChannel[ulPrimerIndex] == NULL)
	{
		PrintError(" STC channel is NULL, primer index(%d)\n", ulPrimerIndex);
		return NULL;
	}

	return primerStcChannel[ulPrimerIndex];
}

DRV_Error DRV_DEMUX_GetStcChannelIndex(NEXUS_StcChannelHandle hStcHandle, HUINT32 *pStcChannelIndex)
{
	DRV_Error drvErr = DRV_ERR;

	PrintEnter();

	if(hStcHandle == NULL)
	{
		PrintError("[%s] Invalid Parameter: hStcHandle is NULL!\n", __func__);
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(pStcChannelIndex == NULL)
	{
		PrintError("[%s] Invalid Parameter: pStcChannelIndex is NULL!\n", __func__);
		return DRV_ERR_INVALID_PARAMETER;
	}

	drvErr= P_DEMUX_GetStcChannelIndex(hStcHandle, pStcChannelIndex);
	if(drvErr != DRV_OK)
	{
		PrintError(" get STC channel index is failed!\n");
		return DRV_ERR;
	}

	PrintExit();
	return DRV_OK;
}
#endif

NEXUS_StcChannelHandle DRV_DEMUX_GetStcChannelHandle(void)
{
	return shstcChannel;
}

#if defined(CONFIG_PIP)
NEXUS_StcChannelHandle DRV_DEMUX_GetPIPStcChannelHandle(void)
{
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	if(shstcChannel_o == NULL)
	{
		PrintError(" STC channel is NULL!\n");
		return NULL;
	}
#endif
	return shstcChannel_o;
}
#endif

HUINT32	DRV_DEMUX_GetSyncCount(HUINT32 ulIBIndex)
{
	NEXUS_InputBand inputBand;
	NEXUS_InputBandStatus inputBandStatus;

	inputBand = P_DEMUX_GetInputBand(ulIBIndex);
	NEXUS_InputBand_GetStatus(inputBand, &inputBandStatus);
	return inputBandStatus.syncCount;
}

HUINT32 DRV_DEMUX_GetPIDChannel(HUINT32 ulDemuxId, HUINT16 usPid)
{
	P_DEMUX_Module *pDemux;
	HUINT8 channel_index;
	unsigned long physicalChannelIndex = INVALID_32BITS;

	pDemux = P_DEMUX_GetInstance();

	for(channel_index = 0; channel_index < DRV_DMX_NUM_PID_CHANNEL; channel_index++)
	{
		VK_SEM_Get(pDemux->pidChannel[channel_index].lock);
		if(pDemux->pidChannel[channel_index].status == DRV_DEMUX_RESOURCE_ENABLED &&
			pDemux->pidChannel[channel_index].demuxId == ulDemuxId &&
			pDemux->pidChannel[channel_index].pid == usPid)
		{
			physicalChannelIndex = P_DEMUX_GetPhysicalPidChannelIndex(pDemux->pidChannel[channel_index].pidHandle);
			VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
			break;
		}
		VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
	}
	return physicalChannelIndex;
}

HUINT32 DRV_DEMUX_GetPIDChannelForDSC(HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 *pulPidChannel)
{
	P_DEMUX_Module *pDemux;
	HUINT8 channel_index;
	unsigned long physicalChannelIndex = INVALID_32BITS;
	HUINT32	ulNumChannel = 0;

	pDemux = P_DEMUX_GetInstance();

	for(channel_index = 0; channel_index < DRV_DMX_NUM_PID_CHANNEL; channel_index++)
	{
		VK_SEM_Get(pDemux->pidChannel[channel_index].lock);
		if(pDemux->pidChannel[channel_index].status == DRV_DEMUX_RESOURCE_ENABLED &&
			pDemux->pidChannel[channel_index].pidHandle != NULL &&
			pDemux->pidChannel[channel_index].channelType == DI_DEMUX_CHANNELTYPE_PRIVATE &&
			pDemux->pidChannel[channel_index].demuxId == ulDemuxId &&
			pDemux->pidChannel[channel_index].pid == usPid &&
			pDemux->pidChannel[channel_index].channelsubInfo == P_DEMUX_CHSUBT_eDSC)
		{
			if(physicalChannelIndex == INVALID_32BITS)
			{
				physicalChannelIndex = P_DEMUX_GetPhysicalPidChannelIndex(pDemux->pidChannel[channel_index].pidHandle);
			}
			ulNumChannel++;
		}
		VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
	}
	*pulPidChannel = physicalChannelIndex;

	return ulNumChannel;
}

DRV_Error DRV_DEMUX_ConnectPIDChannelToDSC(HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 *pulPidChannel)
{
	P_DEMUX_Module *pDemux;
	DRV_Error rc;
	HUINT32 logicalPidChannel;

	PrintEnter();

	if(ulDemuxId >= MAX_DEMUX_NUM)
	{
		PrintError(" DemuxId %d is out of range!\n", ulDemuxId);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();

	rc = P_DEMUX_AllocateChannel(DI_DEMUX_CHANNELTYPE_PRIVATE, usPid, &logicalPidChannel);
	if(rc != DRV_OK || logicalPidChannel == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError(" Failed: P_DEMUX_AllocateChannel() return error %d \n", rc);
		return DRV_ERR;
	}

	rc = P_DEMUX_EnableChannel(DI_DEMUX_CHANNELTYPE_PRIVATE, ulDemuxId, logicalPidChannel, P_DEMUX_CHSUBT_eDSC);
	if(rc != DRV_OK)
	{
		PrintError(" Failed: P_DEMUX_EnableChannel() return error %d \n", rc);
		P_DEMUX_DeallocateChannel(logicalPidChannel);
		return DRV_ERR;
	}
	*pulPidChannel = P_DEMUX_GetPhysicalPidChannelIndex(pDemux->pidChannel[logicalPidChannel].pidHandle);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_DEMUX_DisconnectPIDChannelFromDSC(HUINT32 ulPidChannel)
{
	P_DEMUX_Module *pDemux;
	DRV_Error drvError;
	HUINT32 logicalPidChannel;
	HUINT32 physicalChannelIndex = INVALID_32BITS;

	PrintEnter();

	pDemux = P_DEMUX_GetInstance();

	for(logicalPidChannel = 0; logicalPidChannel < DRV_DMX_NUM_PID_CHANNEL; logicalPidChannel++)
	{
		VK_SEM_Get(pDemux->pidChannel[logicalPidChannel].lock);

		if(pDemux->pidChannel[logicalPidChannel].status == DRV_DEMUX_RESOURCE_ENABLED &&
			pDemux->pidChannel[logicalPidChannel].pidHandle != NULL &&
			pDemux->pidChannel[logicalPidChannel].channelType == DI_DEMUX_CHANNELTYPE_PRIVATE &&
			pDemux->pidChannel[logicalPidChannel].channelsubInfo == P_DEMUX_CHSUBT_eDSC)
		{
			physicalChannelIndex = P_DEMUX_GetPhysicalPidChannelIndex(pDemux->pidChannel[logicalPidChannel].pidHandle);
			if(physicalChannelIndex == ulPidChannel)
			{
				VK_SEM_Release(pDemux->pidChannel[logicalPidChannel].lock);
				break;
			}
		}
		VK_SEM_Release(pDemux->pidChannel[logicalPidChannel].lock);
	}
	if(logicalPidChannel >= DRV_DMX_NUM_PID_CHANNEL)
	{
		PrintError("Can't find the channel connected to dsc.");
		return DRV_ERR;
	}

	drvError = P_DEMUX_DisableChannel(logicalPidChannel);
	if(drvError != DRV_OK){
		PrintError(" Failed: DRV_DEMUX_DisableChannel() return error %d \n", drvError);
		return DRV_ERR;
	}

	drvError = P_DEMUX_DeallocateChannel(logicalPidChannel);
	if(drvError != DRV_OK){
		PrintError("Failed: DRV_DEMUX_DeallocateChannel() return error %d \n", drvError);
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_DEMUX_GetPlaybackOutputPath (HUINT32 ulPbDeviceId, HBOOL *pbExternal)
{
	P_DEMUX_Module *pDemux;
	HUINT32 ulDemuxId;
	HINT32 nCI_Id;
	HBOOL bShadowPath = FALSE;

	ulDemuxId = ulPbDeviceId + LIVE_DEMUX_NUM;
	if(ulDemuxId < LIVE_DEMUX_NUM || ulDemuxId >= MAX_DEMUX_NUM || pbExternal == NULL)
	{
		PrintError(" Invalid input parameters.(PB chan : %d, pbExternal = 0x%lx)\n\r", ulPbDeviceId, pbExternal);
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();
	VK_SEM_Get(ciLock);
	VK_SEM_Get(pDemux->dmxDevice[ulDemuxId].lock);
	nCI_Id	= pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id;
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	if(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId != 0xff)
	{
		bShadowPath = TRUE;
	}
#else
	bShadowPath = pDemux->shadowPBPath.bEnabled;
#endif
	VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
	VK_SEM_Release(ciLock);

	if(nCI_Id >= 0 || bShadowPath != FALSE)
	{
		*pbExternal = TRUE;
	}
	else
	{
		*pbExternal = FALSE;
	}

	return DRV_OK;
}


DRV_Error DRV_DEMUX_StartRecordPID(HUINT32 ulRecDeviceId, HUINT16 usPid, HUINT32 *pulPIDChannel, HBOOL bFullTs)
{
	DRV_Error rc;
	HUINT32 ulDemuxId;
	HBOOL bPsubRequired = FALSE;
	DI_DEMUX_CHANNELTYPE channelType;
	PrintEnter();

	if(ulRecDeviceId >= MAX_REC_CHANNEL)
	{
		PrintError(" Recorder ID is out of range!\n");
		return DRV_ERR;
	}

	ulDemuxId = gaRecorder2DemuxMapper[ulRecDeviceId];
	if(bFullTs == TRUE)
	{
		channelType = DI_DEMUX_CHANNELTYPE_ALLPASS;
	}
	else
	{
		channelType = DI_DEMUX_CHANNELTYPE_RECORD;
	}

	if(bFullTs == FALSE)
	{
		bPsubRequired = P_DEMUX_CheckPktSubIsRequired(ulDemuxId, usPid);
		if(bPsubRequired == TRUE && usPid == P_DEMUX_PAT_PID)
	  	{
			usPid = P_DEMUX_NULLPKT_PID;
	  	}
	}


	rc = P_DEMUX_AllocateChannel(channelType, usPid, pulPIDChannel);
	if(rc != DRV_OK || *pulPIDChannel == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError(" Failed: P_DEMUX_AllocateChannel() return error %d \n", rc);
		return DRV_ERR;
	}

	rc = P_DEMUX_EnableChannel(channelType, ulDemuxId, *pulPIDChannel, P_DEMUX_CHSUBT_eNone);
	if(rc != DRV_OK)
	{
		P_DEMUX_DeallocateChannel(*pulPIDChannel);
		PrintError(" Failed: P_DEMUX_EnableChannel() return error %d \n", rc);
		return DRV_ERR;
	}

	if(bFullTs == FALSE)
	{
		P_DEMUX_StartPktSub(ulDemuxId, *pulPIDChannel);
	}

	PrintExit();

	return DRV_OK;


}


DRV_Error DRV_DEMUX_StopRecordPID(HUINT32 ulPIDChannel, HBOOL bFullTs)
{
	DRV_Error drvError;

	PrintEnter();

	if(bFullTs == FALSE)
	{
 		P_DEMUX_StopPktSub(ulPIDChannel);
	}

	drvError = P_DEMUX_DisableChannel(ulPIDChannel);
	if(drvError != DRV_OK)
	{
		PrintError(" failed: DRV_DEMUX_DisableChannel() return error %d \n", drvError);
		return DRV_ERR;
	}

	drvError = P_DEMUX_DeallocateChannel(ulPIDChannel);
	if(drvError != DRV_OK)
	{
		PrintError(" failed: DRV_DEMUX_DeallocateChannel() return error %d \n", drvError);
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_DEMUX_StartRecordTSC(HUINT32 ulPIDChannel)
{
	DRV_Error rc;
	rc = P_DEMUX_StartTSCMonitoring(ulPIDChannel, 1);
	return rc;
}

HBOOL DRV_DEMUX_IsPlaybackPidChannel(NEXUS_PidChannelHandle ulPidChannelHandle, DRV_DEMUX_PLAYBACK_MODE *ePlayMode)
{
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBand parserBand;
	HUINT32 channelIndex;
	HUINT32 demuxId = INVALID_32BITS;
	HBOOL	bIsPlayback = FALSE;

	pDemux = P_DEMUX_GetInstance();

	*ePlayMode = DRV_DEMUX_PLAYBACK_UNKNOWN;

	for(channelIndex = 0; channelIndex < DRV_DMX_NUM_PID_CHANNEL; channelIndex++)
	{
		VK_SEM_Get(pDemux->pidChannel[channelIndex].lock);
		if(pDemux->pidChannel[channelIndex].status == DRV_DEMUX_RESOURCE_ENABLED &&
			pDemux->pidChannel[channelIndex].pidHandle == ulPidChannelHandle)
		{
			demuxId = pDemux->pidChannel[channelIndex].demuxId;
			VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
			break;
		}
		VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
	}

	if(demuxId != INVALID_32BITS)
	{
		parserBand = P_DEMUX_GetParserBandByDemuxId(demuxId);
		if(parserBand >= NEXUS_ParserBand_ePb0)
		{
			*ePlayMode = DRV_DEMUX_PLAYBACK_INTERNAL;
			bIsPlayback = TRUE;
		}
#if defined(CONFIG_PVR)&&defined(CONFIG_CI)
		else if(parserBand == P_DEMUX_GetIBParserForScrambledFilePlayback() || parserBand == P_DEMUX_GetIBParserForScrambledFileRecording())
		{
			*ePlayMode = DRV_DEMUX_PLAYBACK_EXTERNAL;
			bIsPlayback = TRUE;
		}
#endif
	}
	return bIsPlayback;

}

DRV_Error DRV_DEMUX_GetCAMBypassStatus(unsigned short usSlotId, HUINT32 *pciUsageCount, HBOOL *pbPassthrough )
{
#if !defined(CONFIG_ZAPPING_ONLY)
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();
	VK_SEM_Get(ciLock);
	*pciUsageCount = pDemux->ciInfo[usSlotId].ciUsageCount;
	*pbPassthrough = pDemux->ciInfo[usSlotId].bPassthrough;
	VK_SEM_Release(ciLock);
	return DRV_OK;
#else
	return DRV_OK;
#endif
}

DRV_Error DRV_DEMUX_CamBypass(unsigned short usSlotId)
{
#if !defined(CONFIG_ZAPPING_ONLY)
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBandSettings pb_settings;
	NEXUS_ParserBand parser_band;
	NEXUS_Error rc;
	DRV_Error drvError = DRV_OK;
	int i;

	if(usSlotId >= CAM_MAXSLOTID)
	{
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();
	VK_SEM_Get(ciLock);

	if(pDemux->ciInfo[usSlotId].ciUsageCount == 0)
	{
		VK_SEM_Release(ciLock);
		return DRV_ERR;
	}

	for(i=0; i<MAX_DEMUX_NUM; i++)
	{
		VK_SEM_Get(pDemux->dmxDevice[i].lock);
		if(pDemux->dmxDevice[i].pathInfo.nCI_Id == usSlotId)
		{
			parser_band = P_DEMUX_GetParserBandByDemuxId(i);

			if(i< LIVE_DEMUX_NUM)
			{
#if defined(CONFIG_BCM_MTSIF)
				drvError = P_DEMUX_SetMTSIFPath(pDemux->dmxDevice[i].pathInfo.etProducer, pDemux->dmxDevice[i].pathInfo.ulInputDevice_id, parser_band, TRUE, FALSE);
				if(drvError != DRV_OK)
				{
					PrintError(" Failed to set P_DEMUX_SetMTSIFPath@@@@@@@ (%d)\n", drvError);
					drvError = DRV_ERR;
				}
#else
				NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
				pb_settings.allPass = FALSE;
				pb_settings.acceptNullPackets = FALSE;
				pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
				if(pDemux->dmxDevice[i].pathInfo.etProducer == DI_DEMUX_INPUT_NONE)
				{
					pb_settings.sourceTypeSettings.inputBand = P_DEMUX_GetUnusedInputBand(); //P_DEMUX_INBAND_NOT_USED;
				}
				else
				{
					pb_settings.sourceTypeSettings.inputBand = P_DEMUX_GetInputBand(pDemux->dmxDevice[i].pathInfo.ulInputDevice_id);
				}
				pb_settings.transportType = NEXUS_TransportType_eTs;
				rc = NEXUS_ParserBand_SetSettings(parser_band, &pb_settings);
				if(rc != NEXUS_SUCCESS)
				{
					PrintError(" Failed to set main ParserBand (%d)\n", rc);
					drvError = DRV_ERR;
				}
#endif
			}
			else
			{
				NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
				pb_settings.allPass = FALSE;
				pb_settings.acceptNullPackets = FALSE;
				pb_settings.sourceType = NEXUS_ParserBandSourceType_eRemux;
				pb_settings.sourceTypeSettings.remux = pDemux->rmxHandle;
				pb_settings.transportType = NEXUS_TransportType_eTs;
				rc = NEXUS_ParserBand_SetSettings(parser_band, &pb_settings);
				if(rc != NEXUS_SUCCESS)
				{
					PrintError(" Failed to set main ParserBand (%d)\n", rc);
					drvError = DRV_ERR;
				}
			}
		}
		VK_SEM_Release(pDemux->dmxDevice[i].lock);
	}
	pDemux->ciInfo[usSlotId].bPassthrough = FALSE;
	VK_SEM_Release(ciLock);
	return drvError;
#else
	return DRV_OK;
#endif
}

DRV_Error DRV_DEMUX_CamPassthrough(unsigned short usSlotId)
{
#if !defined(CONFIG_ZAPPING_ONLY)
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBandSettings pb_settings;
	NEXUS_ParserBand parser_band;
	NEXUS_Error rc;
	DRV_Error drvError = DRV_OK;
	int i;

	NEXUS_InputBand inputBand;

	if(usSlotId >= CAM_MAXSLOTID)
	{
		return DRV_ERR;
	}

	pDemux = P_DEMUX_GetInstance();
	VK_SEM_Get(ciLock);

	if(pDemux->ciInfo[usSlotId].ciUsageCount == 0)
	{
		VK_SEM_Release(ciLock);
		return DRV_ERR;
	}

	for(i=0; i<MAX_DEMUX_NUM; i++)
	{
		VK_SEM_Get(pDemux->dmxDevice[i].lock);
		if(pDemux->dmxDevice[i].pathInfo.nCI_Id == usSlotId)
		{
			parser_band = P_DEMUX_GetParserBandByDemuxId(i);
#if defined(CONFIG_BCM_MTSIF)
			NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
			if(pb_settings.sourceTypeSettings.inputBand == DRV_CIRemux_PATH_GetInbandID(0)
				&& pb_settings.sourceType == NEXUS_ParserBandSourceType_eInputBand )
			{
				/* It is already using CI */
			}
			else
			{
				/* If MTSIF for CI, need set-up parseband for REMUX */
				drvError = P_DEMUX_SetMTSIFPath(pDemux->dmxDevice[i].pathInfo.etProducer, pDemux->dmxDevice[i].pathInfo.ulInputDevice_id, parser_band, FALSE, FALSE);
				if(drvError != DRV_OK)
				{
				    VK_SEM_Release(pDemux->dmxDevice[i].lock);
				    VK_SEM_Release(ciLock);
				    PrintError(" Failed to set Remux ParserBand -P_DEMUX_SetMTSIFPath@@@@@@@ (%d)\n", drvError);
				    return DI_ERR_ERROR;
				}
			}
#endif
			NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
			if(pDemux->dmxDevice[i].pathInfo.etProducer == DI_DEMUX_INPUT_NONE)
			{
				inputBand = P_DEMUX_GetUnusedInputBand();
			}
			else
			{
				inputBand = DRV_CIRemux_PATH_GetInbandID(0);
			}

			if(pb_settings.sourceTypeSettings.inputBand == inputBand
			   && pb_settings.sourceType== NEXUS_ParserBandSourceType_eInputBand)
			{
				/* It is already using CI */
			}
			else
			{
				pb_settings.allPass = FALSE;
				pb_settings.acceptNullPackets = FALSE;
				pb_settings.transportType = NEXUS_TransportType_eTs;
				pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
				pb_settings.sourceTypeSettings.inputBand = inputBand;
			    rc = NEXUS_ParserBand_SetSettings(parser_band, &pb_settings);
			    if(rc != NEXUS_SUCCESS)
			    {
				    PrintError(" Failed to set main ParserBand (%d)\n", rc);
				    drvError = DRV_ERR;
			    }
			}
		}
		VK_SEM_Release(pDemux->dmxDevice[i].lock);
	}

	if(drvError == DRV_OK)
	{
		pDemux->ciInfo[usSlotId].bPassthrough = TRUE;
	}

	VK_SEM_Release(ciLock);
	return drvError;
#else
	return DRV_OK;
#endif
}

NEXUS_StcChannelHandle DRV_DEMUX_GetSTCChannel( HUINT32 unDeviceId )
{
#if defined(CONFIG_TRANSCODER)
	HUINT32 index = 0;
#endif
	NEXUS_StcChannelHandle	pStcChannel = NULL;

	if(unDeviceId > 2)
		return NULL;

	if(unDeviceId == 0)
	{
		pStcChannel = shstcChannel;
	}
#if defined(CONFIG_TRANSCODER)
	else
	{
		index = unDeviceId;
		if(index > 0)
			index -= 1;

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
		if(s_subhstcChannel[index] == NULL)
		{
			PrintError(" STC channel is NULL, index(%d).\n", index);
			return NULL;
		}
#endif
		if(index < NUM_TRANSCODE_STC)
		{
			pStcChannel = s_subhstcChannel[index];
		}
		else
		{
			PrintError(" Out of range : index(%d), max(%d).\n", index, NUM_TRANSCODE_STC);
			return NULL;
		}
	}
#endif

	return pStcChannel;
}

#if defined(CONFIG_TRANSCODER)
NEXUS_StcChannelHandle DRV_DEMUX_GetSTCChannelTranscode( HUINT32 unDeviceId )
{
	NEXUS_StcChannelHandle	pStcChannelTranscode = NULL;

	if(unDeviceId > 1)
		return NULL;

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	if(s_stcChannelTranscode[unDeviceId] == NULL)
	{
		PrintError(" STC channel is NULL, unDeviceId(%d).\n", unDeviceId);
		return NULL;
	}
#endif

	if(unDeviceId < NUM_TRANSCODE_STC)
	{
		pStcChannelTranscode = s_stcChannelTranscode[unDeviceId];
	}
	else
	{
		PrintError(" Out of range : unDeviceId(%d), max(%d).\n", unDeviceId, NUM_TRANSCODE_STC);
		return NULL;
	}

	return pStcChannelTranscode;
}
#endif


DRV_Error P_DEMUX_StartPID(HUINT32 ulDemuxId, DI_DEMUX_CHANNELTYPE eChannelType, HUINT16 usPid)
{
	DRV_Error rc;

	PrintEnter();

#if defined(CONFIG_PIP)
#if defined(CONFIG_TRANSCODER)
if((( eChannelType > DI_DEMUX_CHANNELTYPE_PCR )&&(eChannelType < DI_DEMUX_CHANNELTYPE_VIDEO_SUB))|| eChannelType >= DI_DEMUX_CHANNELTYPE_MAX|| usPid >= DI_DEMUX_INVALID_PID)
#else
if((( eChannelType > DI_DEMUX_CHANNELTYPE_PCR )&&(eChannelType < DI_DEMUX_CHANNELTYPE_VIDEO_SUB))|| eChannelType > DI_DEMUX_CHANNELTYPE_PCR_SUB|| usPid >= DI_DEMUX_INVALID_PID)
#endif
#else
#if defined(CONFIG_TRANSCODER)
	if(((eChannelType > DI_DEMUX_CHANNELTYPE_PCR )&&(eChannelType < DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_1))|| eChannelType >= DI_DEMUX_CHANNELTYPE_MAX|| usPid >= DI_DEMUX_INVALID_PID)
#else
	if(eChannelType > DI_DEMUX_CHANNELTYPE_PCR || usPid >= DI_DEMUX_INVALID_PID)
#endif
#endif
	{
		PrintError(" Failed: invalid parameter  eChannelType=%d\n",eChannelType);
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(PidChannelInfo[ulDemuxId][eChannelType] != DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError("[%s] Channel is already enabled. stop it first.", PidType_Info[eChannelType]);
		return DRV_ERR;
	}

	rc = P_DEMUX_AllocateChannel(eChannelType, usPid,&PidChannelInfo[ulDemuxId][eChannelType]);
	if(rc != DRV_OK || PidChannelInfo[ulDemuxId][eChannelType] == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError(" Failed: P_DEMUX_AllocateChannel() return error %d \n", rc);
		return DRV_ERR;
	}

	rc = P_DEMUX_EnableChannel(eChannelType, ulDemuxId, PidChannelInfo[ulDemuxId][eChannelType], P_DEMUX_CHSUBT_eNone);
	if(rc != DRV_OK)
	{
		PrintError(" Failed: P_DEMUX_EnableChannel() return error %d \n", rc);
		P_DEMUX_DeallocateChannel(PidChannelInfo[ulDemuxId][eChannelType]);
		PidChannelInfo[ulDemuxId][eChannelType] = DI_DEMUX_INVALID_CHANNEL_ID;
		return DRV_ERR;
	}

	PrintDebug("P_DEMUX_StartPID(), channel index is %d channel type =%d pid=%ld\n", PidChannelInfo[ulDemuxId][eChannelType], eChannelType, usPid);

	PrintExit();

	return DRV_OK;
}

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
static DRV_Error P_DEMUX_StartPrimerPID(HUINT32 ulDemuxId, DI_DEMUX_CHANNELTYPE eChannelType, HUINT16 usPid, HUINT32 ulPrimerIndex)
{
	DRV_Error rc;

	PrintEnter();

	if(eChannelType > DI_DEMUX_CHANNELTYPE_PCR || usPid >= DI_DEMUX_INVALID_PID)
	{
		PrintError(" Failed: invalid parameter  eChannelType=%d\n",eChannelType);
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(PidChannelInfo[ulDemuxId][eChannelType] != DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError("[%s] Channel is already enabled. stop it first.", PidType_Info[eChannelType]);
		return DRV_ERR;
	}

	rc = P_DEMUX_AllocateChannel(eChannelType, usPid,&PidChannelInfo[ulDemuxId][eChannelType]);
	if(rc != DRV_OK || PidChannelInfo[ulDemuxId][eChannelType] == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError(" Failed: P_DEMUX_AllocateChannel() return error %d \n", rc);
		return DRV_ERR;
	}

	rc = P_DEMUX_EnablePrimerChannel(eChannelType, ulDemuxId, PidChannelInfo[ulDemuxId][eChannelType], ulPrimerIndex);
	if(rc != DRV_OK)
	{
		PrintError(" Failed: P_DEMUX_EnablePrimerChannel() return error %d \n", rc);
		P_DEMUX_DeallocateChannel(PidChannelInfo[ulDemuxId][eChannelType]);
		PidChannelInfo[ulDemuxId][eChannelType] = DI_DEMUX_INVALID_CHANNEL_ID;
		return DRV_ERR;
	}

	PrintDebug("P_DEMUX_StartPrimerPID(), channel index is %d channel type =%d pid=%ld\n", PidChannelInfo[ulDemuxId][eChannelType], eChannelType, usPid);

	PrintExit();

	return DRV_OK;
}
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
static DRV_Error P_DEMUX_StopVideoPrimerPID(HUINT32 ulDemuxId, HUINT32 ulPrimerIndex)
{
	HUINT32 ulPIDChannelIndex;
	DRV_Error eDrvRet = DRV_OK;

	PrintEnter();

	if((s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].bVideoPrimerIsEnable == TRUE) &&
		(s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId == ulDemuxId))
	{
		ulPIDChannelIndex = s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulVideoPrimerPIDChannel;

		eDrvRet = P_DEMUX_StopPrimerPID(ulPIDChannelIndex, ulPrimerIndex);
		if(eDrvRet != DRV_OK)
		{
			PrintError("ERROR : P_DEMUX_StopPrimerPID() returns error %d \n", eDrvRet);
			return DRV_ERR;
		}

		PidChannelInfo[ulDemuxId][DI_DEMUX_CHANNELTYPE_VIDEO_PRIMER] = DI_DEMUX_INVALID_CHANNEL_ID;

	}
	else
	{
		PrintError("ERROR : Video primer PID channel is not allocated or invalid demux id, bVideoPrimerIsEnable=%d, CurrentDemuxId=%d, TargetDemuxId\n",
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].bVideoPrimerIsEnable, s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId, ulDemuxId);
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

static DRV_Error P_DEMUX_StartVideoPrimerPID(HUINT32 ulDemuxId, PrimerPid_Info *pPrimerPidInfo)
{
	HUINT32 ulLocalPrimerIndex;
	DRV_Error eDrvRet = DRV_OK;

	PrintEnter();

	if(pPrimerPidInfo == NULL)
	{
		PrintError("ERROR : Invalid parameter, ppidInfo is null\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	ulLocalPrimerIndex = pPrimerPidInfo->ulPrimerIndex;

	if(pPrimerPidInfo->ulPidBitmask & PID_BITMASK_VIDEO_PRIMER)
	{
		if(s_stDemuxPrimerPidChannelInfo[ulLocalPrimerIndex].bVideoPrimerIsEnable == TRUE)
		{
			PrintError("ERROR : The allocation of video primer pidchannel is failed. resource is busy\n");
			return DRV_ERR;
		}

		if(ulLocalPrimerIndex != DEMUX_PRIMER_MAX_COUNT)
		{
			eDrvRet = P_DEMUX_StartPrimerPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_VIDEO_PRIMER, pPrimerPidInfo->usVideoPid, ulLocalPrimerIndex);
			if(eDrvRet != DRV_OK)
			{
				P_DEMUX_StopVideoPrimerPID(ulDemuxId, ulLocalPrimerIndex);
				PrintError("ERROR : The allocation of video primer pidchannels are failed.\n");
				return DRV_ERR;
			}
		}
	}
	else
	{
		PrintError("ERROR : Invalid parameter, pidBitmask(0x%x) is not valid\n", pPrimerPidInfo->ulPidBitmask);
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

static DRV_Error P_DEMUX_StopPcrPrimerPID(HUINT32 ulDemuxId, HUINT32 ulPrimerIndex)
{
	HUINT32 ulPIDChannelIndex;
	DRV_Error eDrvRet = DRV_OK;

	PrintEnter();

	if(s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId == ulDemuxId)
	{
		ulPIDChannelIndex = s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulPcrPrimerPIDChannel;

		eDrvRet = P_DEMUX_StopPrimerPID(ulPIDChannelIndex, ulPrimerIndex);
		if(eDrvRet != DRV_OK)
		{
			PrintError("ERROR : P_DEMUX_StopPrimerPID() returns error %d \n", eDrvRet);
			return DRV_ERR;
		}

		PidChannelInfo[ulDemuxId][DI_DEMUX_CHANNELTYPE_PCR_PRIMER] = DI_DEMUX_INVALID_CHANNEL_ID;
	}
	else
	{
		PrintError("ERROR : Video primer PID channel is not allocated or invalid demux id, bVideoPrimerIsEnable=%d, CurrentDemuxId=%d, TargetDemuxId\n",
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].bVideoPrimerIsEnable, s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId, ulDemuxId);
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

static DRV_Error P_DEMUX_StartPcrPrimerPID(HUINT32 ulDemuxId, PrimerPid_Info *pPrimerPidInfo)
{
	HUINT32 ulLocalPrimerIndex;
	DRV_Error eDrvRet = DRV_OK;

	PrintEnter();

	if(pPrimerPidInfo == NULL)
	{
		PrintError("ERROR : Invalid parameter, ppidInfo is null\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	ulLocalPrimerIndex = pPrimerPidInfo->ulPrimerIndex;

	if(pPrimerPidInfo->ulPidBitmask & PID_BITMASK_PCR_PRIMER)
	{
		if(ulLocalPrimerIndex != DEMUX_PRIMER_MAX_COUNT)
		{
			eDrvRet = P_DEMUX_StartPrimerPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_PCR_PRIMER, pPrimerPidInfo->usPcrPid, ulLocalPrimerIndex);
			if(eDrvRet != DRV_OK)
			{
				P_DEMUX_StopPcrPrimerPID(ulDemuxId, ulLocalPrimerIndex);
				PrintError("ERROR : The allocation of pcr primer pidchannels are failed.\n");
				return DRV_ERR;
			}
		}
	}
	else
	{
		PrintError("ERROR : Invalid parameter, pidBitmask(0x%x) is not valid\n", pPrimerPidInfo->ulPidBitmask);
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
static DRV_Error P_DEMUX_StopAudioPrimerPID(HUINT32 ulDemuxId, HUINT32 ulPrimerIndex)
{
	HUINT32 ulPIDChannelIndex;
	DRV_Error eDrvRet = DRV_OK;

	PrintEnter();

	if((s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].bAudioPrimerIsEnable == TRUE) &&
		(s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId == ulDemuxId))
	{
		ulPIDChannelIndex = s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulAudioPrimerPIDChannel;

		eDrvRet = P_DEMUX_StopPrimerPID(ulPIDChannelIndex, ulPrimerIndex);
		if(eDrvRet != DRV_OK)
		{
			PrintError("ERROR : P_DEMUX_StopPrimerPID() returns error %d \n", eDrvRet);
				return DRV_ERR;
		}

		PidChannelInfo[ulDemuxId][DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER] = DI_DEMUX_INVALID_CHANNEL_ID;
	}
	else
	{
		PrintError("ERROR : Audio primer pidchannel is not allocated or invalid demux id, bAudioPrimerIsEnable=%d, CurrentDemuxId=%d, TargetDemuxId\n",
			s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].bAudioPrimerIsEnable, s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId, ulDemuxId);
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

static DRV_Error P_DEMUX_StartAudioPrimerPID(HUINT32 ulDemuxId, PrimerPid_Info *pPrimerPidInfo)
{
	HUINT32 ulLocalPrimerIndex;
	DRV_Error eDrvRet = DRV_OK;

	PrintEnter();

	if(pPrimerPidInfo == NULL)
	{
		PrintError("ERROR : Invalid parameter, ppidInfo is null\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	ulLocalPrimerIndex = pPrimerPidInfo->ulPrimerIndex;

	if(pPrimerPidInfo->ulPidBitmask & PID_BITMASK_AUDIO_PRIMER)
	{
		if(s_stDemuxPrimerPidChannelInfo[ulLocalPrimerIndex].bAudioPrimerIsEnable == TRUE)
		{
			PrintError("ERROR : The allocation of audio primer pidchannel is failed. resource is busy\n");
			return DRV_ERR;
		}

		if(ulLocalPrimerIndex != DEMUX_PRIMER_MAX_COUNT)
		{
			eDrvRet = P_DEMUX_StartPrimerPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER, pPrimerPidInfo->usAudioPid, ulLocalPrimerIndex);
			if(eDrvRet != DRV_OK)
			{
				P_DEMUX_StopAudioPrimerPID(ulDemuxId, ulLocalPrimerIndex);
				PrintError("ERROR : The allocation of audio primer pidchannels are failed.\n");
				return DRV_ERR;
			}
		}
	}
	else
	{
		PrintError("ERROR : Invalid parameter, pidBitmask(0x%x) is not valid\n", pPrimerPidInfo->ulPidBitmask);
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}
#endif

DRV_Error P_DEMUX_StopPID(HUINT32 ulDemuxId, DI_DEMUX_CHANNELTYPE eChannelType)
{
	DRV_Error drvError;

	PrintEnter();

	if(PidChannelInfo[ulDemuxId][eChannelType] == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError("[%s] Channel is not enabled.\r\n", PidType_Info[eChannelType]);
		return DRV_ERR;
	}

	drvError = P_DEMUX_DisableChannel(PidChannelInfo[ulDemuxId][eChannelType]);
	if(drvError != DRV_OK){
		PrintError(" failed: DRV_DEMUX_DisableChannel() return error %d \n", drvError);
		return DRV_ERR;
	}

	drvError= P_DEMUX_DeallocateChannel(PidChannelInfo[ulDemuxId][eChannelType]);
	if(drvError != DRV_OK){
		PrintError(" failed: DRV_DEMUX_DeallocateChannel() return error %d \n", drvError);
		return DRV_ERR;
	}

	PidChannelInfo[ulDemuxId][eChannelType] = DI_DEMUX_INVALID_CHANNEL_ID;
	PrintExit();

	return DRV_OK;
}

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
static DRV_Error P_DEMUX_StopPrimerPID(HUINT32 ulChannelIndex, HUINT32 ulPrimerIndex)
{
	DRV_Error drvError;

	PrintEnter();

	if(ulChannelIndex == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError("[%d] Channel is not enabled.\r\n", ulChannelIndex);
		return DRV_ERR;
	}

	drvError = P_DEMUX_DisablePrimerChannel(ulChannelIndex, ulPrimerIndex);
	if(drvError != DRV_OK){
		PrintError(" failed: DRV_DEMUX_DisableChannel() return error %d \n", drvError);
		return DRV_ERR;
	}

	drvError= P_DEMUX_DeallocateChannel(ulChannelIndex);
	if(drvError != DRV_OK){
		PrintError(" failed: DRV_DEMUX_DeallocateChannel() return error %d \n", drvError);
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}
#endif

DRV_Error P_DEMUX_StopAllPIDS(HUINT32 ulDemuxId, HUINT32 pidBitmask)
{
	DRV_Error error;
	DRV_Error errorSaved = DRV_OK;
#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
	HUINT32 ulPrimerIndex = 0;
#endif

	if(pidBitmask & PID_BITMASK_AUDIO_SPDIF)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF);
		if(error != DRV_OK)
		{
			errorSaved = DRV_ERR;
		}
	}
	if(pidBitmask & PID_BITMASK_AUDIO_SUB)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_SUB);
		if(error != DRV_OK)
		{
			errorSaved = DRV_ERR;
		}
	}
	if(pidBitmask & PID_BITMASK_AUDIO_MAIN)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_MAIN);
		if(error != DRV_OK)
		{
			errorSaved = DRV_ERR;
		}
	}
	if(pidBitmask & PID_BITMASK_VIDEO)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_VIDEO);
		if(error != DRV_OK)
		{
			errorSaved = DRV_ERR;
		}
	}
	if(pidBitmask & PID_BITMASK_PCR)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_PCR);
		if(error != DRV_OK)
		{
			errorSaved = DRV_ERR;
		}
	}
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	if(pidBitmask & PID_BITMASK_PCR_PRIMER)
	{
		for(ulPrimerIndex=0 ; ulPrimerIndex<DEMUX_PRIMER_MAX_COUNT ; ulPrimerIndex++)
		{
			error = P_DEMUX_StopPcrPrimerPID(s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId, ulPrimerIndex);
			if(error != DRV_OK)
			{
				errorSaved = DRV_ERR;
			}
		}
	}

	if(pidBitmask & PID_BITMASK_VIDEO_PRIMER)
	{
		for(ulPrimerIndex=0 ; ulPrimerIndex<DEMUX_PRIMER_MAX_COUNT ; ulPrimerIndex++)
		{
			error = P_DEMUX_StopVideoPrimerPID(s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId, ulPrimerIndex);
		if(error != DRV_OK)
		{
			errorSaved = DRV_ERR;
		}
	}
	}
#endif
#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
	if(pidBitmask & PID_BITMASK_AUDIO_PRIMER)
	{
		for(ulPrimerIndex=0 ; ulPrimerIndex<DEMUX_PRIMER_MAX_COUNT ; ulPrimerIndex++)
		{
			error = P_DEMUX_StopAudioPrimerPID(s_stDemuxPrimerPidChannelInfo[ulPrimerIndex].ulDemuxId, ulPrimerIndex);
			if(error != DRV_OK)
			{
				errorSaved = DRV_ERR;
			}
		}
	}
#endif
	return errorSaved;
}


/* Check parserband sharing status */
static HUINT32 P_DEMUX_GetSharingParserBandCnt(HUINT32 ulDemuxId)
{
	HUINT32 sharingParserBandCnt = 0;

	HAPPY(ulDemuxId);

#if defined(CONFIG_PARSERBAND_OPTIMIZE)
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBand parserBand;
	HUINT32 i;

	pDemux = P_DEMUX_GetInstance();
	parserBand = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);

	for(i=0;i<LIVE_DEMUX_NUM;i++)
	{
		if( i == ulDemuxId )
			continue;

		if( parserBand == pDemux->dmxDevice[i].pathInfo.parserBand )
		{
			sharingParserBandCnt++;
		}
	}
#endif
	return sharingParserBandCnt;
}

DRV_Error DRV_DEMUX_Init(void)
{
	P_DEMUX_Module *pDemux = NULL;
	HUINT32 i = 0, j = 0;
	HUINT32 err = 0;
#if defined(CONFIG_TS_MATRIX) || defined(CONFIG_FAST_CHANNEL_CHANGE)
	DRV_Error nRet = 0;
#endif
#if defined(CONFIG_TS_MATRIX)
	HUINT32 ulTs0_0 = 0, ulTs0_1 = 0, ulTs1_0 = 0, ulTs1_1 = 0, ulTs1_2 = 0, ulTs1_3 = 0;
#endif
	NEXUS_InputBandSettings inputBandSettings;
	NEXUS_ParserBandSettings pb_settings;
	NEXUS_PidChannelSettings pidSettings;
	NEXUS_StcChannelSettings stcSettings;
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	HUINT32 stcChannelIndex = 0;
	NEXUS_Error rc;
#endif
#if defined(MESSAGE_FILTER_START_STOP_ONLY)
	NEXUS_MessageSettings openSettings;
#endif
	NEXUS_InputBand inputbandIndex;
	HUINT32 ulTSMaxDev = 0;

	VK_memset(&inputBandSettings, 0x00, sizeof(NEXUS_InputBandSettings));
	VK_memset(&pb_settings, 0x00, sizeof(NEXUS_ParserBandSettings));
	VK_memset(&pidSettings, 0x00, sizeof(NEXUS_PidChannelSettings));
	VK_memset(&stcSettings, 0x00, sizeof(NEXUS_StcChannelSettings));

	pDemux = P_DEMUX_GetInstance();
	for(i = 0; i < MAX_DEMUX_NUM; i++)
	{
		if(i < LIVE_DEMUX_NUM)
		{
			pDemux->dmxDevice[i].ulDemuxID = i;
			pDemux->dmxDevice[i].stCapability.ulCapability = DEMUX_FOR_LIVE_CAPABILITY;
			pDemux->dmxDevice[i].stCapability.NumOfPESFilter = 1;
			pDemux->dmxDevice[i].stCapability.NumOfSectoinFilter = MAX_SECTION_FILTER_NUM;
			pDemux->dmxDevice[i].stCapability.NumOfSectoinFilterPerPid = 32;	/* 일단 32개로 제한 한다 */
		}
		else if(i>= LIVE_DEMUX_NUM && i<MAX_DEMUX_NUM)
		{
			pDemux->dmxDevice[i].ulDemuxID = i;
			pDemux->dmxDevice[i].stCapability.ulCapability = DEMUX_FOR_PLAYBACK_CAPABILITY;
			pDemux->dmxDevice[i].stCapability.NumOfPESFilter = 1;
			pDemux->dmxDevice[i].stCapability.NumOfSectoinFilter = MAX_SECTION_FILTER_NUM;
			pDemux->dmxDevice[i].stCapability.NumOfSectoinFilterPerPid = 32;	/* 일단 32개로 제한 한다 */
		}
		pDemux->dmxDevice[i].bIsUsing = FALSE;
		pDemux->dmxDevice[i].stCapability.NumOfMaximumFilterLength = 15;
		pDemux->dmxDevice[i].stCapability.NumOfPrimer = DEMUX_PRIMER_MAX_COUNT;
		pDemux->dmxDevice[i].stCapability.NumOfAvailableStcChannel = MAX_TIMEBASE_NUM;
		pDemux->dmxDevice[i].pathInfo.nCI_Id = -1; /*default value: -1 (CI not used)*/
		pDemux->dmxDevice[i].pathInfo.etProducer = DI_DEMUX_INPUT_NONE;
		pDemux->dmxDevice[i].pathInfo.ulInputDevice_id = 0xff;
		pDemux->dmxDevice[i].pathInfo.etConsumer = DI_DEMUX_OUTPUT_NONE;
		pDemux->dmxDevice[i].pathInfo.ulConsumerGroupId = 0;
		pDemux->dmxDevice[i].pathInfo.inputBand = 0xff; /*default value: 255 (not used)*/
		pDemux->dmxDevice[i].pathInfo.ulRemuxId = 0xff; /*default value: 255 (not used)*/
		pDemux->dmxDevice[i].nHiddenParserIndex = -1;
		pDemux->dmxDevice[i].pathInfo.ulRequestedTunerId = TUNER_NONE;
		pDemux->dmxDevice[i].pathInfo.parserBand = NEXUS_ParserBand_eInvalid;
		err = VK_SEM_Create(&pDemux->dmxDevice[i].lock, "DEMUX_SEM", VK_SUSPENDTYPE_FIFO);
		if(err){
			PrintError("VK_SEM_Create() error, DRV_DEMUX_Init(), demux(%d)\n", i);
			return DI_ERR_ERROR;
		}
	}

	for(i = 0; i < DRV_DMX_NUM_PID_CHANNEL; i++){
		if(NUM_OF_RESERVED_CHANNEL>0 && i<NUM_OF_RESERVED_PIDCHANNEL)
		{
			pidSettings.pidChannelIndex = P_DEMUX_Logical2Physical(i);
			pidSettings.requireMessageBuffer = FALSE;
			pDemux->pidChannel[i].pid = 0x1fff;
			pDemux->pidChannel[i].pidHandle = NEXUS_PidChannel_Open(P_DEMUX_GetReservedParserBand(i), pDemux->pidChannel[i].pid, &pidSettings);
			if(pDemux->pidChannel[i].pidHandle == NULL)
			{
				PrintError(" PATH_LIVE_SCARMBLED, NEXUS_PidChannel_Open() returns NULL\n");
				return DI_ERR_ERROR;
			}
			pDemux->pidChannel[i].status = DRV_DEMUX_RESOURCE_ALLOCATED;
			pDemux->pidChannel[i].channelType = DI_DEMUX_CHANNELTYPE_ALLPASS;
		}
		else
		{
			pidSettings.pidChannelIndex = -1;
			pidSettings.requireMessageBuffer = FALSE;
			pDemux->pidChannel[i].pidHandle = NULL;
			pDemux->pidChannel[i].status = DRV_DEMUX_RESOURCE_FREE;
			pDemux->pidChannel[i].pid = 0xffff;
			pDemux->pidChannel[i].channelType = DI_DEMUX_CHANNELTYPE_MAX;
		}
		pDemux->pidChannel[i].parserBand = NEXUS_ParserBand_eInvalid;
		pDemux->pidChannel[i].bTSCMonitornigOn = FALSE; /*TSC monitor sets false by default.*/
#if defined(CONFIG_SI_PARSING_FOR_MEDIA_PLAYBACK)
		pDemux->pidChannel[i].bFromMedia = FALSE;
#endif
		err = VK_SEM_Create(&pDemux->pidChannel[i].lock, "PID_CHANNEL_SEM", VK_SUSPENDTYPE_FIFO);
		if(err){
			PrintError("VK_SEM_Create() error, DRV_DEMUX_Init(), pid channel(%d)\n", i);
			return DI_ERR_ERROR;
		}
	}

#if defined(CONFIG_TS_MATRIX)
	nRet = DRV_InitTsMatrix();
	if(nRet != DRV_OK)
	{
		PrintError("DRV_InitTsMatrix() Error\n");
	}

	/* Default Setting */
	ulTs0_0 = 0x00;		/* TS0 = 0, TS1 = 1, TS2 = 2, TS3 = 3 */
	ulTs0_1 = 0x01; 	/* TS0 = 0, TS1 = 1, TS2 = 2, TS3 = 3 */

	ulTs1_0 = 0x02;		/* CI_TS0 = 0, CI_TS1 = 1, TS0 = 2 */
	ulTs1_1 = 0x02;		/* CI_TS0 = 0, CI_TS1 = 1, TS1 = 2 */
	ulTs1_2 = 0x02; 	/* CI_TS0 = 0, CI_TS1 = 1, TS2 = 2 */
	ulTs1_3 = 0x02;		/* CI_TS0 = 0, CI_TS1 = 1, TS3 = 2 */

	nRet = DRV_SetTsMatrix(ulTs0_0, ulTs0_1, ulTs1_0, ulTs1_1, ulTs1_2, ulTs1_3);
	if(nRet != DRV_OK)
	{
		PrintError("DRV_SetTsMatrix() Error\n");
	}
#endif

	P_DEMUX_StartInbandMonitoring();

	for(i=0; i<DRV_DMX_NUM_PID_CHANNEL; i++)
	{
		err = VK_SEM_Create(&chLock[i], "GPIDCH_SEM", VK_SUSPENDTYPE_FIFO);
		if(err){
			PrintError("VK_SEM_Create() error, GPIDCH_SEM\n");
			return DRV_ERR;
		}

	}

	for(i=0; i<CAM_MAXSLOTID; i++)
	{
		pDemux->ciInfo[i].ciUsageCount = 0;
		pDemux->ciInfo[i].bPassthrough = FALSE;
		pDemux->ciInfo[i].bShadowOn = FALSE;
		pDemux->ciInfo[i].OutputDevice_id = 0xff;
		pDemux->ciInfo[i].ulDemuxId = 0xff;
	}

	err = VK_SEM_Create(&ciLock, "CIINFO_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		PrintError("VK_SEM_Create() error, CIINFO_SEM\n");
		return DRV_ERR;
	}

	err = VK_SEM_Create(&cbLockSection, "CBSEC_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		PrintError("VK_SEM_Create() error, CBSEC_SEM\n");
		return DRV_ERR;
	}
	err = VK_SEM_Create(&cbLockPES, "CBPES_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		PrintError("VK_SEM_Create() error, CBPES_SEM\n");
		return DRV_ERR;
	}
	err = VK_SEM_Create(&cbLockTS, "CBTS_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		PrintError("VK_SEM_Create() error, CBTS_SEM\n");
		return DRV_ERR;
	}
	err = VK_SEM_Create(&cbLockInbandChk, "CBIBCHK_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		PrintError("VK_SEM_Create() error, CBIBCHK_SEM\n");
		return DRV_ERR;
	}

	pDemux->callback.pfnAllocBuffer = NULL;
	pDemux->callback.pfnFreeBuffer = NULL;
	pDemux->callback.pfnSectionNotify = NULL;

	for(i=0; i<DI_DEMUX_PESTYPE_MAX; i++)
	{
		pDemux->callback.pfnPesAllocBuffer[i] = NULL;
		pDemux->callback.pfnPesFreeBuffer[i] = NULL;
		pDemux->callback.pfnPesNotify[i] = NULL;
	}

	pDemux->callback.pfnTSStateNotify = NULL;

	P_DEMUX_PrepareSectionHandling();
	P_DEMUX_PreparePESHandling();
#if defined(SECTION_FILTER_DEBUGGING)
	P_DEMUX_DEBUG_PrepareSectionHandling();
#endif
#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	P_DEMUX_PrepareTSHandling();
#endif

	P_DEMUX_InitFilter();

#if defined(MESSAGE_FILTER_START_STOP_ONLY)
	for(i = 0; i < MAX_SECTION_FILTER_NUM; i++)
	{
		NEXUS_Message_GetDefaultSettings(&openSettings);
		openSettings.dataReady.callback = P_DEMUX_NEXUS_NotifyCB;
		openSettings.dataReady.context =  (void *)MAKE_FILTERPAIRVALUE(0 ,0);
		openSettings.dataReady.param = 0;
		openSettings.overflow.callback = P_DEMUX_NEXUS_OverflowCB;
		openSettings.overflow.context = (void *)MAKE_FILTERPAIRVALUE(0 ,0);
		openSettings.overflow.param = 0;
		openSettings.bufferSize =0;
#if defined(CONFIG_COMPLETE_SECTION_IN_DI)
		openSettings.maxContiguousMessageSize = 0;
#endif

		pDemux->msgFilter[i].msgHandle = NEXUS_Message_Open(&openSettings);
		if(pDemux->msgFilter[i].msgHandle == NULL)
		{
			PrintError(" NEXUS_Message_Open() return error\n");
			return DRV_ERR;
		}
	}

#endif

	inter_buffer = DD_MEM_Alloc(INTER_BUFFER_SIZE);
	if(inter_buffer == NULL)
	{
		PrintError("Failed allocation intermediate buffer for section processing.\n");
		return DRV_ERR;
	}
	pes_inter_buffer = DD_MEM_Alloc(PES_INTER_BUFFER_SIZE);
	if(pes_inter_buffer == NULL)
	{
		PrintError("Failed allocation intermediate buffer for section processing.\n");
		return DRV_ERR;
	}

	for(j = 0; j < MAX_DEMUX_NUM; j++)
	{
		for( i=0; i < DI_DEMUX_CHANNELTYPE_MAX; i++)
		{
			PidChannelInfo[j][i] = DI_DEMUX_INVALID_CHANNEL_ID;
		}
	}

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
	VK_memset(&s_stDemuxPrimerPidChannelInfo, 0x00, sizeof(s_stDemuxPrimerPidChannelInfo));
#endif

	//Initialize Packet Substitution structure.
	err = VK_SEM_Create(&pktsubLock, "PKTSUB_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		PrintError("VK_SEM_Create() error, PKTSUB_SEM\n");
		return DRV_ERR;
	}
	P_DEMUX_InitPktSubModule();

	//Initialize TSC Monitor data structure.
	err = VK_SEM_Create(&tscmonLock, "TSCMON_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		PrintError("VK_SEM_Create() error, TSCMON_SEM\n");
		return DRV_ERR;
	}

#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	P_DEMUX_InitTscMonModule_2nd();
#endif

	P_DEMUX_InitTscMonModule();

	//temporary code.
	ulTSMaxDev = DRV_CFG_GetTSMaxDevice();
	for ( i = 0; i < ulTSMaxDev ; i++ )
	{
		inputbandIndex = P_DEMUX_GetInputBand(i);
		if(inputbandIndex != NEXUS_InputBand_eMax)
		{
			NEXUS_InputBand_GetSettings(inputbandIndex, &inputBandSettings);
#if defined(CONFIG_DEMUX_IGNORE_INPUT_VALID_SIGNAL)
			inputBandSettings.validEnabled = false;
#else
			inputBandSettings.validEnabled = true;
#endif
			inputBandSettings.useInternalSync = true;
			if(inputbandIndex == 4)
			{
				inputBandSettings.parallelInput = true;
			}
			NEXUS_InputBand_SetSettings(inputbandIndex, &inputBandSettings);
		}
	}

#if 0 /* 40nm 에서는 이렇게 설정하면, 설정이 되지 않아서, 막아놓음 */
	/* Change Parser's bitrate limit to 36Mbps*/
	for ( i = 0; i<MAX_IBPARSER_BAND_NUM; i++ )
	{
		NEXUS_ParserBand_GetSettings(i, &pb_settings);
		pb_settings.maxDataRate= 108000000;
		NEXUS_ParserBand_SetSettings(i, &pb_settings);
	}
#endif

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	P_DEMUX_InitRemuxInstance();
	P_DEMUX_InitConnectCI();
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	for(i=0; i<MAX_TIMEBASE_NUM; i++)
	{
		NEXUS_StcChannel_GetDefaultSettings(i, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0 + i;
		pDemux->stcChannel[i].stcHandle = NEXUS_StcChannel_Open(i, &stcSettings);
		pDemux->stcChannel[i].bStcChannelIsAllocated = FALSE;

		err = VK_SEM_Create(&pDemux->stcChannel[i].lock, "STC_CHANNEL_SEM", VK_SUSPENDTYPE_FIFO);
		if(err){
			PrintError("VK_SEM_Create() error, DRV_DEMUX_Init(), STC channel(%d)\n", i);
			return DI_ERR_ERROR;
		}
	}

	if(shstcChannel == NULL)
	{
		nRet = P_DEMUX_AllocateStcChannel(&stcChannelIndex);
		if(nRet != DRV_OK)
		{
			PrintError(" stc channel allocation is failed!\n");
			return DI_ERR_ERROR;
		}

		shstcChannel = pDemux->stcChannel[stcChannelIndex].stcHandle;

		NEXUS_StcChannel_GetDefaultSettings(stcChannelIndex, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0 + stcChannelIndex;
		rc = NEXUS_StcChannel_SetSettings(shstcChannel, &stcSettings);
		if(rc != 0)
		{
			PrintError(" NEXUS_StcChannel_SetSettings failed!\n");
			return DI_ERR_ERROR;
		}
	}

#else
	if(shstcChannel == NULL)
	{
		NEXUS_StcChannel_GetDefaultSettings(DEF_STC_CHANNEL_ID, &stcSettings);
		shstcChannel = NEXUS_StcChannel_Open(DEF_STC_CHANNEL_ID, &stcSettings);
		if(shstcChannel == NULL)
		{
			PrintError(" stc channel open fail\n");
			return DI_ERR_ERROR;
		}
	}
#endif

#if defined(CONFIG_PIP)
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
#else
	if(shstcChannel_o == NULL)
	{
 		NEXUS_StcChannel_GetDefaultSettings(DEF_STC_CHANNEL_ID + 1, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e1;
		/*stcSettings.mode = NEXUS_StcChannelMode_ePcr;*/ /* Don't set mode(NEXUS_StcChannelMode_ePcr) value: valid pidChannel shall be configured first.*/
		shstcChannel_o = NEXUS_StcChannel_Open(DEF_STC_CHANNEL_ID + 1, &stcSettings);
		if(shstcChannel_o == NULL)
		{
			PrintError(" stc channel other open fail\n");
			return DI_ERR_ERROR;
		}
	}
#endif
#endif

#if defined(CONFIG_CI)
	DRV_CI_RegisterCallback (P_DEMUX_NotifyInsertCamForDefaultPath);
	for(i=0; i< CAM_MAXSLOTID; i++)
	{
		stCamManage[i].pathInfo.bIsValid = FALSE;
		stCamManage[i].status.cmdReceived = Explicit_Disconnection_Req;
		stCamManage[i].status.phyStatus = CAM_Disconnected_Sts;
		stCamManage[i].status.cmdExecuted = Explicit_Disconnection_Req;
	}
#endif

#if defined(CONFIG_TRANSCODER)
	/***********************************************************************************************************************
													add for transcoding
	***********************************************************************************************************************/
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
#else
	unsigned int index = 0;
	/* encoders/mux require different STC broadcast mode from decoder */
	for(index = 0; index < NUM_TRANSCODE_STC; index++)
	{
#if !defined(CONFIG_PIP)
		NEXUS_StcChannel_GetDefaultSettings( index*2 + 1, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0 + index*2 + 1;
		stcSettings.mode = NEXUS_StcChannelMode_eAuto;
		s_subhstcChannel[index] = NEXUS_StcChannel_Open( 2*index + 1, &stcSettings);
		if(s_subhstcChannel[index] == NULL)
		{
			PrintError(" trans stc channel open fail\n");
			return DI_ERR_ERROR;
		}

		NEXUS_StcChannel_GetDefaultSettings( index*2 + 2, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0 + index*2 + 2;/* should be the same timebase for end-to-end locking */
		stcSettings.mode = NEXUS_StcChannelMode_eAuto;
		stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
		s_stcChannelTranscode[index] = NEXUS_StcChannel_Open(2 + index*2, &stcSettings);
		if(s_stcChannelTranscode[index] == NULL)
		{
			PrintError(" trans stc channel other open fail\n");
			return DI_ERR_ERROR;
		}
#else
 		NEXUS_StcChannel_GetDefaultSettings( index*2 + 2, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0 + index*2 + 2;
		stcSettings.mode = NEXUS_StcChannelMode_eAuto;
		s_subhstcChannel[index] = NEXUS_StcChannel_Open( 2*index + 2, &stcSettings);
		if(s_subhstcChannel[index] == NULL)
		{
			PrintError(" trans stc channel open fail\n");
			return DI_ERR_ERROR;
		}

		NEXUS_StcChannel_GetDefaultSettings( index*2 + 3, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0 + index*2 + 3;/* should be the same timebase for end-to-end locking */
		stcSettings.mode = NEXUS_StcChannelMode_eAuto;
		stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
		s_stcChannelTranscode[index] = NEXUS_StcChannel_Open(3 + index*2, &stcSettings);
		if(s_stcChannelTranscode[index] == NULL)
		{
			PrintError(" trans stc channel other open fail\n");
			return DI_ERR_ERROR;
		}
#endif
	}
#endif
	/***********************************************************************************************************************/
#endif

	return DI_ERR_OK;
}

DRV_Error DRV_DEMUX_UnInit(void)
{
	P_DEMUX_Module *pDemux = NULL;
	HUINT8 i;
	HUINT8 filter_index, channel_index;
	HUINT32 err;

	pDemux = P_DEMUX_GetInstance();

	if(pDemux == NULL)
	{
		PrintError(" DRV_DEMUX_UnInit() returns error.\n");
		return DI_ERR_ERROR;
	}

	for(filter_index = 0; filter_index < MAX_SECTION_FILTER_NUM; filter_index++)
	{

		if(pDemux->msgFilter[filter_index].filterType != P_DEMUX_FilterType_MAX
			&& pDemux->msgFilter[filter_index].status == DRV_DEMUX_RESOURCE_ENABLED)
		{
			channel_index = pDemux->msgFilter[filter_index].channelIndex;

			if(P_DEMUX_DisableFilter(pDemux->msgFilter[filter_index].filterType, filter_index) != DRV_OK)
			{
				PrintError("DRV_DEMUX_UnInit() P_DEMUX_DisableFilter() returns error.\n");
				return DI_ERR_ERROR;
			}

			if(P_DEMUX_DeallocateFilter(pDemux->msgFilter[filter_index].filterType, filter_index) != DRV_OK)
			{
				PrintError(" DRV_DEMUX_UnInit() P_DEMUX_DeallocateFilter() returns error.\n");
				return DI_ERR_ERROR;
			}

			/* disable & deallocate channel */
			if(P_DEMUX_DisableChannel(channel_index) != DRV_OK)
			{
				PrintError(" DRV_DEMUX_UnInit() P_DEMUX_DisableChannel() returns error.\n");
				return DI_ERR_ERROR;
			}

			if(P_DEMUX_DeallocateChannel(channel_index) != DRV_OK)
			{
				PrintError(" DRV_DEMUX_UnInit() P_DEMUX_DeallocateChannel() returns error.\n");
				return DI_ERR_ERROR;
			}

		}

	}

#if defined(MESSAGE_FILTER_START_STOP_ONLY)
	for(i = 0; i < MAX_SECTION_FILTER_NUM; i++)
	{
		NEXUS_Message_Close(pDemux->msgFilter[i].msgHandle);
		pDemux->msgFilter[i].msgHandle = NULL;
	}
#endif

	for(i = 0; i < DRV_DMX_NUM_PID_CHANNEL; i++)
	{
		pDemux->pidChannel[i].pidHandle = NULL;
		pDemux->pidChannel[i].status = DRV_DEMUX_RESOURCE_FREE;
		pDemux->pidChannel[i].pid = 0xffff;
		pDemux->pidChannel[i].channelType = DI_DEMUX_CHANNELTYPE_MAX;

		err = VK_SEM_Destroy(pDemux->pidChannel[i].lock);
		if(err)
		{
			PrintError("DRV_DEMUX_UnInit() VK_SEM_Destroy() error, DRV_DEMUX_UnInit(), pid channel(%d)\n", i);
			return DI_ERR_ERROR;
		}
	}

#if defined(CONFIG_TS_MATRIX)
	DRV_UnInitTsMatrix();
#endif

	P_DEMUX_StopInbandMonitoring();

	P_DEMUX_DestorySectionHandling();

	P_DEMUX_DestroyPESHandling();

#if defined(SECTION_FILTER_DEBUGGING)
	P_DEMUX_DEBUG_DestroySectionHandling();
#endif

#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	P_DEMUX_DestroyTSHandling();
#endif

	P_DEMUX_ReleaseFilter();

	err = VK_SEM_Destroy(ciLock);
	if(err)
	{
		PrintError(" DRV_DEMUX_UnInit() VK_SEM_Destroy() error, CIINFO_SEM\n");
		return DRV_ERR;
	}

	err = VK_SEM_Destroy(cbLockSection);
	if(err)
	{
		PrintError("DRV_DEMUX_UnInit() VK_SEM_Destroy() error, CBSEC_SEM\n");
		return DRV_ERR;
	}

	err = VK_SEM_Destroy(cbLockPES);
	if(err)
	{
		PrintError("DRV_DEMUX_UnInit() VK_SEM_Destroy() error, CBPES_SEM\n");
		return DRV_ERR;
	}

	err = VK_SEM_Destroy(cbLockTS);
	if(err)
	{
		PrintError("DRV_DEMUX_UnInit() VK_SEM_Destroy() error, CBTS_SEM\n");
		return DRV_ERR;
	}

	err = VK_SEM_Destroy(cbLockInbandChk);
	if(err)
	{
		PrintError("DRV_DEMUX_UnInit() VK_SEM_Destroy() error, CBIBCHK_SEM\n");
		return DRV_ERR;
	}


	DD_MEM_Free(inter_buffer);
	DD_MEM_Free(pes_inter_buffer);

	err = VK_SEM_Destroy(pktsubLock);
	if(err)
	{
		PrintError("DRV_DEMUX_UnInit() VK_SEM_Destroy() error, PKTSUB_SEM\n");
		return DRV_ERR;
	}

	//Initialize TSC Monitor data structure.
	err = VK_SEM_Destroy(tscmonLock);
	if(err)
	{
		PrintError("DRV_DEMUX_UnInit() VK_SEM_Destroy() error, TSCMON_SEM\n");
		return DRV_ERR;
	}

#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	err = VK_SEM_Destroy(gTscCheck.Lock);
	if(err)
	{
		PrintError("DRV_DEMUX_UnInit() VK_SEM_Destroy() error, gTscCheck.Lock\n");
		return DRV_ERR;
	}
#endif

#if 0 /* 우선 막아 놓음  */
	if(shstcChannel != NULL)
	{
		NEXUS_StcChannel_Close(shstcChannel);
	}
#endif
	return DI_ERR_OK;

}


#if defined(CONFIG_DEMOD_TSMF)
DRV_Error DRV_DEMUX_GetLiveInputBand(HINT32 nSearchIdx, HUINT32* punDemuxId, HUINT32* punInputBand)
{
	P_DEMUX_Module *pDemux;
	DRV_Error eRet=DRV_OK;
	HUINT32 ulDemuxId;
	HUINT32 ulLiveDemuxId=0;
	HUINT32 unInputBand=0;

	pDemux = P_DEMUX_GetInstance();

	if((punDemuxId == NULL) || (punInputBand == NULL))
	{
		PrintError("Invalid param : punDemuxId(0x%x), punInputBand(0x%x)\n", punDemuxId, punInputBand);
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(nSearchIdx == -1)
	{
		nSearchIdx = 0;
	}

	for(ulDemuxId = nSearchIdx; ulDemuxId < MAX_DEMUX_NUM; ulDemuxId++)
	{
		VK_SEM_Get(pDemux->dmxDevice[ulDemuxId].lock);
		if((pDemux->dmxDevice[ulDemuxId].bIsUsing == TRUE) &&
			(pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer == DI_DEMUX_INPUT_TUNER) &&
			(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_DECODER) &&
			(DRV_CH_GetTunerType(pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id) == DI_CH_TYPE_CAB))
		{
			unInputBand = pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id;
			ulLiveDemuxId = ulDemuxId;
		//	PrintError("\n\n\nFind Live inputBand(%d) : demux(%d)\n", unInputBand, ulDemuxId);

			VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
			break;
		}
		else
		{
			VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
			continue;
		}
	}

	if(ulDemuxId >= MAX_DEMUX_NUM)
	{
		//PrintError("\nFail to Find Live inputband\n");
		eRet = DRV_ERR;
	}
	else
	{
		*punDemuxId = ulLiveDemuxId;
		*punInputBand = unInputBand;
		eRet = DRV_OK;
	}

	return eRet;
}
NEXUS_InputBand DRV_DEMUX_GetUnusedInputBand(void)
{
	return P_DEMUX_GetUnusedInputBand();
}
#endif

HBOOL DRV_DEMUX_CheckIfDecoderInputIsMediaPlayer(void)
{
	P_DEMUX_Module *pDemux;
	HUINT32 ulDemuxId;

	pDemux = P_DEMUX_GetInstance();

	for(ulDemuxId = 0; ulDemuxId < MAX_DEMUX_NUM; ulDemuxId++)
	{
		VK_SEM_Get(pDemux->dmxDevice[ulDemuxId].lock);
		if((pDemux->dmxDevice[ulDemuxId].bIsUsing == TRUE) &&
			(pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer == DI_DEMUX_INPUT_MEDIA_PLAYER) &&
			(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_DECODER))
		{
			PrintDebug("\n\n\nFind demux using decoder(%d) : inputdevicetype(%d)\n", ulDemuxId, DI_DEMUX_INPUT_MEDIA_PLAYER);
			VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
			break;
		}
		else
		{
			VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
			continue;
		}
	}

	if(ulDemuxId >= MAX_DEMUX_NUM)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

#if defined(CONFIG_TS_MATRIX)
static DRV_Error DRV_InitTsMatrix(void)
{
	VK_memset(s_pTSMatrixValue, 0x00,sizeof(s_pTSMatrixValue));

	return DRV_OK;
}

static DRV_Error DRV_UnInitTsMatrix(void)
{
	return DRV_OK;
}

DRV_Error P_DEMUX_SelectCIMAXInput(int InputCh, HUINT32 ulSource)
{
	DRV_Error dResult;
	int switchIndex = GPIO_ID_TS_SEL0;

	PrintDebug("P_DEMUX_SelectCIMAXInput inputCh=%d\n", InputCh);
	switchIndex += (InputCh*2);

	dResult = DRV_GPIO_Write(switchIndex, ulSource & 0x01);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}
	dResult = DRV_GPIO_Write(switchIndex+1, (ulSource>>1) & 0x01);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}

	return DRV_OK;

}

DRV_Error P_DEMUX_SelectInbandInput(int Inband, HUINT32 ulSource)
{
	DRV_Error dResult;
	int switchIndex = GPIO_ID_TS_SEL10;

	PrintDebug("P_DEMUX_SelectInbandInput Inband=%d\n", Inband);
	switchIndex += (Inband*2);

	dResult = DRV_GPIO_Write(switchIndex, ulSource & 0x01);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}
	dResult = DRV_GPIO_Write(switchIndex+1, (ulSource>>1) & 0x01);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}

	return DRV_OK;

}

DRV_Error DRV_SetTsMatrix(HUINT32 ulTs0_0, HUINT32 ulTs0_1, HUINT32 ulTs1_0, HUINT32 ulTs1_1, HUINT32 ulTs1_2, HUINT32 ulTs1_3)
{
	DRV_Error dResult;

	dResult = P_DEMUX_SelectCIMAXInput(0,ulTs0_0);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}
	dResult = P_DEMUX_SelectCIMAXInput(1,ulTs0_1);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}
	dResult = P_DEMUX_SelectInbandInput(0, ulTs1_0);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}
	dResult = P_DEMUX_SelectInbandInput(1, ulTs1_1);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}
	dResult = P_DEMUX_SelectInbandInput(2, ulTs1_2);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}
	dResult = P_DEMUX_SelectInbandInput(3, ulTs1_3);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}

	s_pTSMatrixValue[0] = ulTs0_0;
	s_pTSMatrixValue[1] = ulTs0_1;
	s_pTSMatrixValue[2] = ulTs1_0;
	s_pTSMatrixValue[3] = ulTs1_1;
	s_pTSMatrixValue[4] = ulTs1_2;
	s_pTSMatrixValue[5] = ulTs1_3;

	return DRV_OK;
}

DRV_Error DRV_SetTs2CIInput( HUINT32 ulTunerId,HUINT32 ulCiInput )
{
	DRV_Error dResult;

	if(	s_pTSMatrixValue[ulCiInput] != ulTunerId)
	{
		dResult = P_DEMUX_SelectCIMAXInput(ulCiInput, ulTunerId);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_GPIO_Write failed!\n");
			return DRV_ERR;
		}
		s_pTSMatrixValue[ulCiInput] = ulTunerId;
	}

	return DRV_OK;
}

DRV_Error DRV_SetCIOutput2InputBand( HUINT32 ulCiOutput, HUINT32 ulTunerId )
{
	DRV_Error dResult;

	if( s_pTSMatrixValue[ulTunerId + 2] != ulCiOutput)
	{
		dResult = P_DEMUX_SelectInbandInput(ulTunerId,ulCiOutput);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_GPIO_Write failed!\n");
			return DRV_ERR;
		}
		s_pTSMatrixValue[ulTunerId + 2] = ulCiOutput;
	}

	return DRV_OK;

}

DRV_Error DRV_SetTS2CIandInputBand(  HUINT32 ulTunerId, HUINT32 ulCiInput )
{
	HUINT32	directpass = 2;
	DRV_Error dResult;

	if(	s_pTSMatrixValue[ulCiInput] != ulTunerId)
	{
		dResult = P_DEMUX_SelectCIMAXInput(ulCiInput, ulCiInput);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_GPIO_Write failed!\n");
			return DRV_ERR;
		}
		s_pTSMatrixValue[ulCiInput] = ulTunerId;

	}

	if( s_pTSMatrixValue[ulTunerId + 2] != directpass)
	{
		dResult = P_DEMUX_SelectInbandInput(ulTunerId,directpass);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_GPIO_Write failed!\n");
			return DRV_ERR;
		}
		s_pTSMatrixValue[ulTunerId + 2] = directpass;
	}

	return DRV_OK;

}

DRV_Error DRV_SetTS2InputBand(  HUINT32 ulTunerId )
{
	HUINT32	directpass = 2;
	DRV_Error dResult;

	if( s_pTSMatrixValue[ulTunerId + 2] != directpass)
	{
		dResult = P_DEMUX_SelectInbandInput(ulTunerId,directpass);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_GPIO_Write failed!\n");
			return DRV_ERR;
		}
		s_pTSMatrixValue[ulTunerId + 2] = directpass;
	}

	return DRV_OK;

}



void DRV_GetTsMatrix(HUINT32 *gTs)
{
	if(gTs != NULL)
	{
		VK_memcpy((void *)gTs,(void *)s_pTSMatrixValue,sizeof(s_pTSMatrixValue));
	}

	return;
}

DRV_Error DRV_DefaultTsMatrix(void)
{
	HUINT32 ulTs0_0 = 0,ulTs0_1 = 0, ulTs1_0 = 0, ulTs1_1 = 0,ulTs1_2 = 0, ulTs1_3 = 0;
	DRV_Error result = 0;

	ulTs0_0 = 0x00; 	/* TS0 = 0, TS1 = 1, TS2 = 2, TS3 = 3 */
	ulTs0_1 = 0x01; 	/* TS0 = 0, TS1 = 1, TS2 = 2, TS3 = 3 */

	ulTs1_0 = 0x02; 	/* CI_TS0 = 0, CI_TS1 = 1, TS0 = 2 */
	ulTs1_1 = 0x02; 	/* CI_TS0 = 0, CI_TS1 = 1, TS1 = 2 */
	ulTs1_2 = 0x02; 	/* CI_TS0 = 0, CI_TS1 = 1, TS2 = 2 */
	ulTs1_3 = 0x02; 	/* CI_TS0 = 0, CI_TS1 = 1, TS3 = 2 */

	result = DRV_SetTsMatrix(ulTs0_0, ulTs0_1, ulTs1_0, ulTs1_1, ulTs1_2, ulTs1_3);

	return result;
}
#endif

#if defined(CONFIG_TRANSCODER)
DRV_Error DRV_DEMUX_StartPIDForTranscode(HUINT32 ulDemuxId, Pid_Info *ppidInfo)
{

	DRV_Error error = 0;
	HUINT32 localPidBitMask = 0;
	P_DEMUX_Module *pDemux = NULL;

	if(ulDemuxId >= MAX_DEMUX_NUM )
	{
		PrintError(" failed: invalid parameter \n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
	{
		if(ppidInfo->pidBitmask & PID_BITMASK_PCR_TRANS_1)
		{
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_PCR_TRANS_1, ppidInfo->usPCRPid);
			if(error == DRV_OK)
			{
				localPidBitMask |= PID_BITMASK_PCR_TRANS_1;
			}
			else
			{
				P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
				ppidInfo->pidBitmask= 0;
				return DRV_ERR;
			}
		}
		if(ppidInfo->pidBitmask & PID_BITMASK_VIDEO_TRANS_1)
		{
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_1, ppidInfo->usVideoPid);
			if(error == DRV_OK)
			{
				localPidBitMask |= PID_BITMASK_VIDEO_TRANS_1;
			}
			else
			{
				P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
				ppidInfo->pidBitmask= 0;
				return DRV_ERR;
			}
		}
		if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_TRANS_1)
		{
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_1, ppidInfo->usAudioMainPid);
			if(error == DRV_OK)
			{
				localPidBitMask |= PID_BITMASK_AUDIO_TRANS_1;
			}
			else
			{
				P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
				ppidInfo->pidBitmask= 0;
				return DRV_ERR;
			}
		}
		if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_SUB_TRANS_1)
		{
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1, ppidInfo->usAudioSubPid);
			if(error == DRV_OK)
			{
				localPidBitMask |= PID_BITMASK_AUDIO_SUB_TRANS_1;
			}
			else
			{
				P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
				ppidInfo->pidBitmask= 0;
				return DRV_ERR;
			}
		}

		if(ppidInfo->pidBitmask & PID_BITMASK_PCR_TRANS_2)
		{
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_PCR_TRANS_2, ppidInfo->usPCRPid);
			if(error == DRV_OK)
			{
				localPidBitMask |= PID_BITMASK_PCR_TRANS_2;
			}
			else
			{
				P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
				ppidInfo->pidBitmask= 0;
				return DRV_ERR;
			}
		}
		if(ppidInfo->pidBitmask & PID_BITMASK_VIDEO_TRANS_2)
		{
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_2, ppidInfo->usVideoPid);
			if(error == DRV_OK)
			{
				localPidBitMask |= PID_BITMASK_VIDEO_TRANS_2;
			}
			else
			{
				P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
				ppidInfo->pidBitmask= 0;
				return DRV_ERR;
			}
		}
		if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_TRANS_2)
		{
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_2, ppidInfo->usAudioMainPid);
			if(error == DRV_OK)
			{
				localPidBitMask |= PID_BITMASK_AUDIO_TRANS_2;
			}
			else
			{
				P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
				ppidInfo->pidBitmask= 0;
				return DRV_ERR;
			}
		}
		if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_SUB_TRANS_2)
		{
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2, ppidInfo->usAudioSubPid);
			if(error == DRV_OK)
			{
				localPidBitMask |= PID_BITMASK_AUDIO_SUB_TRANS_2;
			}
			else
			{
				P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
				ppidInfo->pidBitmask= 0;
				return DRV_ERR;
			}
		}

		ppidInfo->pidBitmask= localPidBitMask;
	}
	else
	{
		PrintError(" failed: did not initialize path \n");
		return DRV_ERR_INITIALIZATION;
	}

	return DRV_OK;

}

DRV_Error DRV_DEMUX_StopPIDForTranscode(HUINT32 ulDemuxId, HUINT32 pidBitmask)
{
	DRV_Error error = 0;
	DRV_Error drvError = 0;
	P_DEMUX_Module *pDemux = NULL;

	if(ulDemuxId >= MAX_DEMUX_NUM )
	{
		PrintError(" failed: invalid parameter \n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
	{

		if(pidBitmask & PID_BITMASK_AUDIO_TRANS_1)
		{
			error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_1);
			if(error != DRV_OK)
			{
				drvError = DRV_ERR;
			}
		}
		if(pidBitmask & PID_BITMASK_AUDIO_SUB_TRANS_1)
		{
			error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1);
			if(error != DRV_OK)
			{
				drvError = DRV_ERR;
			}
		}
		if(pidBitmask & PID_BITMASK_VIDEO_TRANS_1)
		{
			error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_1);
			if(error != DRV_OK)
			{
				drvError = DRV_ERR;
			}
		}
		if(pidBitmask & PID_BITMASK_PCR_TRANS_1)
		{
			error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_PCR_TRANS_1);
			if(error != DRV_OK)
			{
				drvError = DRV_ERR;
			}
		}

		if(pidBitmask & PID_BITMASK_AUDIO_TRANS_2)
		{
			error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_2);
			if(error != DRV_OK)
			{
				drvError = DRV_ERR;
			}
		}

		if(pidBitmask & PID_BITMASK_AUDIO_SUB_TRANS_2)
		{
			error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2);
			if(error != DRV_OK)
			{
				drvError = DRV_ERR;
			}
		}

		if(pidBitmask & PID_BITMASK_VIDEO_TRANS_2)
		{
			error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_2);
			if(error != DRV_OK)
			{
				drvError = DRV_ERR;
			}
		}
		if(pidBitmask & PID_BITMASK_PCR_TRANS_2)
		{
			error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_PCR_TRANS_2);
			if(error != DRV_OK)
			{
				drvError = DRV_ERR;
			}
		}
	}
	else
	{
		drvError = DRV_ERR;
	}

	return drvError;
}
#endif

DRV_Error DRV_DEMUX_GetOutPutDeviceType(HUINT32 ulDemuxId , DI_DEMUX_OUTPUTDEVICETYPE * etConsumer)
{
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();
	if (etConsumer == NULL)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(pDemux->dmxDevice[ulDemuxId].bIsUsing == FALSE)
	{
		return DRV_ERR;
	}

	*etConsumer = pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer;

	return DRV_OK;
}

DRV_Error DRV_DEMUX_GetInputDevice(HUINT32 ulDemuxId , DI_DEMUX_INPUTDEVICETYPE * etProducer, HUINT32 * ulDeviceId)
{
	P_DEMUX_Module *pDemux;

	pDemux = P_DEMUX_GetInstance();
	if (etProducer == NULL)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(pDemux->dmxDevice[ulDemuxId].bIsUsing == FALSE)
	{
		return DRV_ERR;
	}

	*etProducer = pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer;
	*ulDeviceId = pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id;

	return DRV_OK;
}

DI_ERR_CODE DI_DEMUX_GetNumOfDemux(HUINT32 * pulNumOfDemux)
{
	PrintEnter();

	if(pulNumOfDemux == NULL)
	{
		return DI_ERR_INVALID_PARAM;
	}

	*pulNumOfDemux = (HUINT32)MAX_DEMUX_NUM;

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_GetTotalCapability( DI_DEMUX_TOTALCAPABILITY * ptotalstCapability )
{
	PrintEnter();

	if(ptotalstCapability == NULL)
	{
		return DI_ERR_INVALID_PARAM;
	}

	ptotalstCapability->NumOfDemux = (HUINT32)MAX_DEMUX_NUM;
	ptotalstCapability->NumOfTotalSectoinFilter = (HUINT32)MAX_SECTION_FILTER_NUM;

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_GetCapability(HUINT32 ulDemuxID, DI_DEMUX_CAPABILITY * pstCapability )
{
	P_DEMUX_Module *pDemux;
	PrintEnter();

	if(pstCapability == NULL || ulDemuxID >= MAX_DEMUX_NUM)
	{
		return DI_ERR_INVALID_PARAM;
	}

	pDemux = P_DEMUX_GetInstance();

	VK_memcpy(pstCapability, &pDemux->dmxDevice[ulDemuxID].stCapability, sizeof(DI_DEMUX_CAPABILITY));

	PrintExit();

	return DI_ERR_OK;
}

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
#define	FIRST_TUNER_ID	0
#define	LAST_TUNER_ID	3
#define	MAX_TUNER_NUM	4

DRV_Error P_DEMUX_GetInputDeviceId(HUINT32 ulDemuxId , HUINT32 *pInputDeviceID)
{
	P_DEMUX_Module *pDemux;

	HUINT32 demuxIndex = 0,count = 0,index = 0,findflag = 0,findId = 0;
	HUINT32 tmp[MAX_DEMUX_NUM] = {0,};
	HUINT32	tunerId[MAX_TUNER_NUM] = {0,};
	HINT32 deviceId = 0;

	PrintEnter();

	if(pInputDeviceID == NULL )
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	VK_memset(tmp,0xff,sizeof(tmp));

	for(demuxIndex = 0; demuxIndex < MAX_DEMUX_NUM; demuxIndex++)
	{
		if(ulDemuxId != demuxIndex)
		{
			if((pDemux->dmxDevice[demuxIndex].bIsUsing == 1)&&(pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer != 0)&&(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer != 0))
			{
				if( pDemux->dmxDevice[demuxIndex].pathInfo.ulInputDevice_id != 0xff )
				{
					tmp[count] = pDemux->dmxDevice[demuxIndex].pathInfo.ulInputDevice_id;
					count++;
				}
			}
		}
	}

	if(count == 0)
	{
		*pInputDeviceID = LAST_TUNER_ID;
	}
	else
	{

		for(index = 0; index < count; index++)
		{
			findId = tmp[index];
			tunerId[findId]++;
		}

		for(deviceId = LAST_TUNER_ID; deviceId >= FIRST_TUNER_ID ; deviceId--)
		{
			if(tunerId[deviceId] == 0)
			{
				findflag = 1;
				*pInputDeviceID = deviceId;
				break;
			}
		}

		if(findflag == 0)
		{
			return DRV_ERR;
		}

	}

	PrintExit();

	return DRV_OK;
}
#endif

DI_ERR_CODE DI_DEMUX_SetInputSource(HUINT32 ulDemuxId, DI_DEMUX_INPUTDEVICETYPE etProducer, HUINT32 ulInputDevice_id)
{
	P_DEMUX_Module *pDemux;
	pDemux = P_DEMUX_GetInstance();

	if(ulDemuxId >= MAX_DEMUX_NUM)
	{
		PrintError("Invalid demux id (%d)\n", ulDemuxId);
		return DI_ERR_INVALID_PARAM;
	}

	switch(etProducer)
	{
		case DI_DEMUX_INPUT_TUNER :
			VK_SEM_Get(pDemux->dmxDevice[ulDemuxId].lock);
			pDemux->dmxDevice[ulDemuxId].pathInfo.ulRequestedTunerId= ulInputDevice_id;
			VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
			break;
		default :
			break;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_SetPath(HUINT32 ulDemuxId,
								   DI_DEMUX_INPUTDEVICETYPE etProducer, HUINT32 ulInputDevice_id,
								   DI_DEMUX_OUTPUTDEVICETYPE etConsumer, HUINT32 ulConsumerGroupId,
								   HINT32 nCI_Id)

{
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBand parser_band = NEXUS_ParserBand_eInvalid;
	NEXUS_ParserBandSettings pb_settings;
	NEXUS_Error rc;
#if defined(CONFIG_CI) || defined(CONFIG_BCM_MTSIF)
	DRV_Error drvError = 0;
#endif
#if defined(CONFIG_BCM_MTSIF)
	HBOOL bIsTSModeMtsif = FALSE;
#endif
	HINT32	ciSlotId = -1;
	NEXUS_InputBand inputBand = 0;
	/* just use for live limux */
	HUINT32 OutputDevice_id = 3;

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	P_REMUX_Module	*pRemux = NULL;
	NEXUS_ParserBand oparser_band;
	DRV_CI_PATH_STATUS_t tmpCiPathStatus;
	HUINT32 CIConnectStatus = 0;

	VK_MEM_Memset((void *)&(tmpCiPathStatus),0,sizeof(DRV_CI_PATH_STATUS_t));
#endif

	PrintEnter("ulDemuxId %d, etProducer %x, ulInputDevice_id %x, etConsumer %x, ulConsumerGroupId %x, CI_Id %d \n",
			ulDemuxId, etProducer, ulInputDevice_id, etConsumer, ulConsumerGroupId, nCI_Id);
	DI_UART_Print("[SetPath] ulDemuxId %d, etProducer %x, ulInputDevice_id %x, etConsumer %x, ulConsumerGroupId %x, CI_Id %d \n",
			ulDemuxId, etProducer, ulInputDevice_id, etConsumer, ulConsumerGroupId, nCI_Id);
	pDemux = P_DEMUX_GetInstance();

	if(ulDemuxId >= MAX_DEMUX_NUM)
	{
		PrintError("Invalid demux id (%d)\n", ulDemuxId);
		return DI_ERR_INVALID_PARAM;
	}

	VK_SEM_Get(ciLock);
	VK_SEM_Get(pDemux->dmxDevice[ulDemuxId].lock);

	if(pDemux->dmxDevice[ulDemuxId].bIsUsing == FALSE && (etProducer ==DI_DEMUX_INPUT_CURRENT|| etConsumer == DI_DEMUX_OUTPUT_CURRENT))
	{
		VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
		VK_SEM_Release(ciLock);
		PrintError("Currently disabled Demux can't be configured with INPUT_CURRENT(%ld) or OUTPUT_CURRENT(%ld)\n", etProducer, etConsumer);
		return DI_ERR_INVALID_PARAM;
	}

	/*What should be done when both producer and consumer are not changed.(that is CURRENT)*/
	if(etProducer == DI_DEMUX_INPUT_CURRENT && etConsumer == DI_DEMUX_OUTPUT_CURRENT
			&& pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id == nCI_Id)
	{
		VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
		VK_SEM_Release(ciLock);
		return DI_ERR_OK;
	}

	if(etProducer == DI_DEMUX_INPUT_CURRENT)
	{
		etProducer = pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer;
		ulInputDevice_id = pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id;
	}

	if(etConsumer == DI_DEMUX_OUTPUT_CURRENT)
	{
		etConsumer = pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer;
		ulConsumerGroupId = pDemux->dmxDevice[ulDemuxId].pathInfo.ulConsumerGroupId;
	}

#if 0 //Stop checking
	if(pDemux->dmxDevice[ulDemuxId].bIsUsing){
		/*Check If reconfiguration is possible without releasing.*/
		if((pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer == etProducer &&
			pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id == ulInputDevice_id &&
			pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id == nCI_Id) ||
			(pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer == etProducer &&
			pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id == ulInputDevice_id &&
			pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == etConsumer &&
			pDemux->dmxDevice[ulDemuxId].pathInfo.ulConsumerGroupId == ulConsumerGroupId &&
			pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id != nCI_Id))
		{
			PrintDebug("DI_DEMUX_SetPath(): Reconfiguring Demux %d's path.\n",ulDemuxId);
		}
		else
		{
			/*Reconfiguration is impossible*/
			VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
			VK_SEM_Release(ciLock);
			PrintError(" Demux %d is in use now. release it first.\n",ulDemuxId);
			return DI_ERR_NO_RESOURCE;
		}
	}
#endif

	ciSlotId = nCI_Id;

	//Check if CI-redirection is required
	if(ciSlotId >= 0)
	{
		if(pDemux->ciInfo[ciSlotId].ciUsageCount  >= NUM_CI_SLOT_CONCURRENT)
		{
			//Check if CI-path sharing is possible.
			if(pDemux->ciInfo[ciSlotId].etProducer == etProducer &&
				pDemux->ciInfo[ciSlotId].ulInputDevice_id == ulInputDevice_id)
			{
				PrintDebug("DI_DEMUX_SetPath(): Sharing CI-Path\n");
			}
			else
			{
				VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
				VK_SEM_Release(ciLock);
				PrintError(" CI is alrealy used and can't be shared.\n");
				return DI_ERR_NO_RESOURCE;
			}
		}
	}


	/*Set Input Path*/
	if(P_DEMUX_IsLiveParserBand(ulDemuxId,etProducer)==TRUE) /*Set path for Live decoding*/
	{
		parser_band = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);

		if(ciSlotId >= 0)
		{

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
			P_DEMUX_SetConnectCI(ulInputDevice_id,TRUE);
#endif

			if(pDemux->ciInfo[ciSlotId].ciUsageCount == 0)
			{
#if defined(CONFIG_CI)
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
				DRV_CIMAX_ResetFIFOBySynchro( ciSlotId );

				VK_memset(&tmpCiPathStatus,0,sizeof(DRV_CI_PATH_STATUS_t));
				drvError = DRV_CI_GetPathStatus(ciSlotId ,&tmpCiPathStatus);
				if(drvError != DRV_OK)
				{
					VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
					VK_SEM_Release(ciLock);
					PrintError("Failed routing path to CAM (for LIVE).(drvError = %ld)\n", drvError);
					return DI_ERR_ERROR;
				}

				if((tmpCiPathStatus.used == TRUE)&&(tmpCiPathStatus.connectStatus == TRUE))
				{
					if(tmpCiPathStatus.outputId != 0xff)
					{
						if((tmpCiPathStatus.inputSrc == CI_INPUT_LIVE0)||(tmpCiPathStatus.inputSrc == CI_INPUT_LIVE1))
						{
							drvError = DRV_SetCIOutput2InputBand(tmpCiPathStatus.outputId,ulInputDevice_id );
							if(drvError != DRV_OK)
							{
								PrintError("Failed routing path (CAM output to Inputband )(drvError = %ld)\n", drvError);
								VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
								VK_SEM_Release(ciLock);
								PrintError("Failed routing path to CAM (for LIVE).(drvError = %ld)\n", drvError);
								return DI_ERR_ERROR;
							}

							pDemux->ciInfo[ciSlotId].bPassthrough = TRUE;

						}
						else if((tmpCiPathStatus.inputSrc == CI_INPUT_REMUX0)||(tmpCiPathStatus.inputSrc == CI_INPUT_REMUX1))
						{
							drvError = P_DEMUX_RouteRemuxToRequestedPath(ulDemuxId,CI_LIVE_REMUX);
							if(drvError != DRV_OK)
							{
								PrintError("Failed routing path (CAM output to Inputband )(drvError = %ld)\n", drvError);
								VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
								VK_SEM_Release(ciLock);
								PrintError("Failed routing path to CAM (for LIVE).(drvError = %ld)\n", drvError);
								return DI_ERR_ERROR;
							}

							/* OutputDevice_id ==> 남는 output band를 가져오는 코드가 필요한데, 내가 알아서 하는 것은 안되고, 위에서, 인자가 필요함 .. 우선은 마지막으로 fix 시켜 놓음 */
							drvError = DRV_SetCIOutput2InputBand(tmpCiPathStatus.outputId,OutputDevice_id );
							if(drvError != DRV_OK)
							{
								PrintError("Failed routing path (CAM output to Inputband )(drvError = %ld)\n", drvError);
								P_DEMUX_DestroyRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
								VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
								VK_SEM_Release(ciLock);
								PrintError("Failed routing path to CAM (for LIVE).(drvError = %ld)\n", drvError);
								return DI_ERR_ERROR;
							}
						}
					}
				}

#else
#if defined(CONFIG_BCM_MTSIF)
				/* If MTSIF for CI, need set-up parseband for REMUX */
				drvError = P_DEMUX_SetMTSIFPath(etProducer, ulInputDevice_id, P_DEMUX_GetCoParserBandForCI(parser_band), TRUE, TRUE);
				if(drvError != DRV_OK)
				{
					VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
					VK_SEM_Release(ciLock);
					PrintError(" Failed to set Remux ParserBand (%d)\n", drvError);
					return DI_ERR_ERROR;
				}

#endif
				drvError = P_DEMUX_UpdateCAMPath(Explicit_Connection_Req, ciSlotId,FALSE,P_DEMUX_GetInputBand(ulInputDevice_id),P_DEMUX_GetCoParserBandForCI(parser_band),ulDemuxId);
				if(drvError != DRV_OK)
				{
					VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
					VK_SEM_Release(ciLock);
					PrintError("Failed routing path to CAM (for LIVE).(drvError = %ld)\n", drvError);
					return DI_ERR_ERROR;
				}
#endif
#endif
				pDemux->ciInfo[ciSlotId].etProducer = etProducer;
				pDemux->ciInfo[ciSlotId].ulInputDevice_id = ulInputDevice_id;
			}

			pDemux->dmxDevice[ulDemuxId].nHiddenParserIndex = P_DEMUX_GetAvailableHiddenParserIndex(ulDemuxId);

			if(P_DEMUX_GetSharingParserBandCnt(ulDemuxId) == NOT_USED)
			{
#if defined(CONFIG_BCM_MTSIF)
				if(DRV_CH_GetTSOutputMode(ulInputDevice_id) == CH_TS_MODE_MTSIF)
				{
					bIsTSModeMtsif = TRUE;
				}
				else
				{
					bIsTSModeMtsif = FALSE;
				}

				if(!bIsTSModeMtsif) /* If using MTSIF, no need set-up parseband */
				{
#endif
					NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
					pb_settings.allPass = FALSE;
					pb_settings.acceptNullPackets = FALSE;
					pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
					if(etProducer == DI_DEMUX_INPUT_NONE)
					{
						pb_settings.sourceTypeSettings.inputBand =	P_DEMUX_GetUnusedInputBand(); //P_DEMUX_INBAND_NOT_USED;
					}
					else
					{
						pb_settings.sourceTypeSettings.inputBand = P_DEMUX_GetInputBand(ulInputDevice_id); /* connec to input band at first, whether ci is used on not.NEXUS_InputBand_e4;*/
					}
					pb_settings.transportType = NEXUS_TransportType_eTs;

					inputBand = pb_settings.sourceTypeSettings.inputBand ;
					rc = NEXUS_ParserBand_SetSettings(parser_band, &pb_settings);
					if(rc != NEXUS_SUCCESS)
					{
						VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
						VK_SEM_Release(ciLock);
						PrintError("DRV_DEMUX_SetPath(), Failed to set main ParserBand (%d)\n", rc);
						return DI_ERR_ERROR;
					}
#if defined (CONFIG_BCM_MTSIF)
				}
				else
				{
					drvError = P_DEMUX_SetMTSIFPath(etProducer, ulInputDevice_id, parser_band, TRUE, FALSE);
					if(drvError != DRV_OK)
					{
						VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
						VK_SEM_Release(ciLock);
						PrintError(" Failed to set set main ParserBand (%d)\n", drvError);
						return DI_ERR_ERROR;
					}
				}
#endif
			}

			pDemux->ciInfo[ciSlotId].OutputDevice_id = OutputDevice_id;

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
			if(pDemux->ciInfo[ciSlotId].ciUsageCount == 0)
			{

				VK_memset(&tmpCiPathStatus,0,sizeof(DRV_CI_PATH_STATUS_t));
				drvError = DRV_CI_GetPathStatus(ciSlotId ,&tmpCiPathStatus);

				if((tmpCiPathStatus.used == TRUE)&&(tmpCiPathStatus.connectStatus == TRUE))
				{
					if(tmpCiPathStatus.outputId != 0xff)
					{
						if((tmpCiPathStatus.inputSrc == CI_INPUT_REMUX0)||(tmpCiPathStatus.inputSrc == CI_INPUT_REMUX1))
						{

							oparser_band = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);
							NEXUS_ParserBand_GetSettings(oparser_band, &pb_settings);
							pb_settings.allPass = FALSE;
							pb_settings.acceptNullPackets = FALSE;
							pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;

							if(pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer == DI_DEMUX_INPUT_NONE)
							{
								pb_settings.sourceTypeSettings.inputBand = P_DEMUX_GetUnusedInputBand(); //P_DEMUX_INBAND_NOT_USED;
							}
							else
							{
								pb_settings.sourceTypeSettings.inputBand = P_DEMUX_GetInputBand(OutputDevice_id);

							}
							pb_settings.transportType = NEXUS_TransportType_eTs;

							rc = NEXUS_ParserBand_SetSettings(oparser_band, &pb_settings);

							if(rc != NEXUS_SUCCESS)
							{
								PrintError(" Failed to set main ParserBand (%d)\n", rc);
								drvError = DRV_ERR;
							}
							else
							{
								pDemux->ciInfo[ciSlotId].bPassthrough = TRUE;
							}
						}
					}
				}

			}

#endif
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id != ciSlotId)
			{
				pDemux->ciInfo[ciSlotId].ciUsageCount++;
			}

		}
		else
		{
			/*Check if CI is used before*/
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id >= 0)
			{
				/*Decrease CI Usage count */
				//Release CI route path
				if(pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].ciUsageCount  > 0)
				{
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
					P_DEMUX_SetConnectCI(ulInputDevice_id,FALSE);
					P_DEMUX_GetConnectCI(ulInputDevice_id,&CIConnectStatus);
#endif

					pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].ciUsageCount --;
#if defined(CONFIG_CI)
					if(pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].ciUsageCount  == 0)
					{
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
						if(CIConnectStatus == 0)
						{
							drvError = DRV_SetTS2InputBand(ulInputDevice_id);

							VK_memset(&tmpCiPathStatus,0,sizeof(DRV_CI_PATH_STATUS_t));
							drvError = DRV_CI_GetPathStatus(pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id ,&tmpCiPathStatus);
							if((tmpCiPathStatus.used == TRUE)&&(tmpCiPathStatus.connectStatus == TRUE))
							{
								if(tmpCiPathStatus.outputId != 0xff)
								{
									if((tmpCiPathStatus.inputSrc == CI_INPUT_REMUX0)||(tmpCiPathStatus.inputSrc == CI_INPUT_REMUX1))
									{
										drvError = P_DEMUX_DestroyRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
										pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId = 0xff;
										pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].bPassthrough = FALSE;
										drvError = DRV_SetTS2InputBand(pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].OutputDevice_id);
									}
								}
							}
						}
#else
#if defined(CONFIG_BCM_MTSIF)
						/* If MTSIF for CI, need set-up parseband for REMUX */
						drvError = P_DEMUX_SetMTSIFPath(etProducer, ulInputDevice_id, P_DEMUX_GetCoParserBandForCI(parser_band), FALSE, FALSE);
						if(drvError != DRV_OK)
						{
							VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
							VK_SEM_Release(ciLock);
							PrintError(" Failed to set Remux ParserBand (%d)\n", drvError);
							return DI_ERR_ERROR;
						}
#endif
						P_DEMUX_UpdateCAMPath(Explicit_Disconnection_Req, pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id,0,0,0,ulDemuxId);
#if !defined(CONFIG_ZAPPING_ONLY)
						P_DEMUX_UpdateCAMPath(Implicit_Connection_Req, pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id,0,0,0,ulDemuxId);
#endif

#endif
					}
#endif
				}
				else
				{
					/*somethings wrong.*/
				}
			}
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
			else
			{
				CIConnectStatus = 0;
				P_DEMUX_GetConnectCI(ulInputDevice_id,&CIConnectStatus);

				if(CIConnectStatus == 0)
				{
					drvError = DRV_SetTS2InputBand(ulInputDevice_id);
				}
			}
#endif

			pDemux->dmxDevice[ulDemuxId].nHiddenParserIndex = -1; /*No hidden parser used.*/

			if(P_DEMUX_GetSharingParserBandCnt(ulDemuxId) == NOT_USED)
			{
#if defined(CONFIG_PARSERBAND_OPTIMIZE)
				if(pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id >= 0 && P_DEMUX_GetSharingParserBandCnt(ulDemuxId) != NOT_USED)
				{
					/* for debug */
					PrintError("currently connect Remux IB. Change path to Tuner IB.");
				}
#endif
#if defined(CONFIG_BCM_MTSIF)
				if(DRV_CH_GetTSOutputMode(ulInputDevice_id) == CH_TS_MODE_MTSIF)
				{
					bIsTSModeMtsif = TRUE;
				}
				else
				{
					bIsTSModeMtsif = FALSE;
				}

				if(!bIsTSModeMtsif)
				{
#endif /* CONFIG_BCM_MTSIF */
					NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
					pb_settings.allPass = FALSE;
					pb_settings.acceptNullPackets = FALSE;
					pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
					if(etProducer == DI_DEMUX_INPUT_NONE)
					{
						pb_settings.sourceTypeSettings.inputBand =  P_DEMUX_GetUnusedInputBand(); //P_DEMUX_INBAND_NOT_USED;
					}
					else
					{
						pb_settings.sourceTypeSettings.inputBand = P_DEMUX_GetInputBand(ulInputDevice_id);
					}
					pb_settings.transportType = NEXUS_TransportType_eTs;

					inputBand = pb_settings.sourceTypeSettings.inputBand ;
					rc = NEXUS_ParserBand_SetSettings(parser_band, &pb_settings);
					if(rc != NEXUS_SUCCESS)
					{
						VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
						VK_SEM_Release(ciLock);
						PrintError(" Failed to set main ParserBand (%d)\n", rc);
						return DI_ERR_ERROR;
					}
#if defined(CONFIG_BCM_MTSIF)
				}
				else
				{
					drvError = P_DEMUX_SetMTSIFPath(etProducer, ulInputDevice_id, parser_band, TRUE, FALSE);
					if(drvError != DRV_OK)
					{
						VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
						VK_SEM_Release(ciLock);
						PrintError(" Failed to set main ParserBand (%d)\n", drvError);
						return DI_ERR_ERROR;
					}
				}
#endif /* CONFIG_BCM_MTSIF */
			} // end of if(P_DEMUX_GetSharingParserBandCnt(ulDemuxId))

		}

	}
	else if(P_DEMUX_IsPBParserBand(ulDemuxId,etProducer)==TRUE)
	{
		if(ciSlotId >= 0)
		{
#if !defined(CONFIG_ZAPPING_ONLY)

#if defined(CONFIG_CI)
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
			P_DEMUX_CloseShadowPlaybackPath();
#else
			drvError = P_DEMUX_GetInputDeviceId(ulDemuxId,&OutputDevice_id);
			if(drvError != DRV_OK)
			{
				P_DEMUX_DestroyRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
				pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId = 0xff;
				VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
				VK_SEM_Release(ciLock);
				PrintError("Failed routing path to CAM (for Playback).(drvError = %ld)\n", drvError);
				return DI_ERR_ERROR;
			}

			P_DEMUX_SetConnectCI(OutputDevice_id,TRUE);

			P_DEMUX_StopRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
#endif
#endif
			pDemux->ciInfo[ciSlotId].ulDemuxId = ulDemuxId;

			//Use playback parser as all-pass mode.
			if(pDemux->ciInfo[ciSlotId].ciUsageCount == 0)
			{
#if defined(CONFIG_CI)
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
				VK_memset(&tmpCiPathStatus,0,sizeof(DRV_CI_PATH_STATUS_t));
				drvError = DRV_CI_GetPathStatus(ciSlotId ,&tmpCiPathStatus);
				if(drvError != DRV_OK)
				{
					P_DEMUX_DestroyRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
					pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId = 0xff;
					VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
					VK_SEM_Release(ciLock);
					PrintError("Failed routing path to CAM (for LIVE).(drvError = %ld)\n", drvError);
					return DI_ERR_ERROR;
				}

				if((tmpCiPathStatus.used == TRUE)&&(tmpCiPathStatus.connectStatus == TRUE))
				{
					if((tmpCiPathStatus.inputSrc == CI_INPUT_REMUX0)||(tmpCiPathStatus.inputSrc == CI_INPUT_REMUX1))
					{
						if(tmpCiPathStatus.outputId != 0xff)
						{
							drvError = P_DEMUX_RestartRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
							if(drvError != DRV_OK)
							{
								P_DEMUX_DestroyRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
								pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId = 0xff;
								VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
								VK_SEM_Release(ciLock);
								PrintError("Failed routing path to Remux (for Playback).(drvError = %ld)\n", drvError);
								return DI_ERR_ERROR;
							}

							drvError = DRV_SetCIOutput2InputBand(tmpCiPathStatus.outputId,OutputDevice_id );

							oparser_band = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);
							NEXUS_ParserBand_GetSettings(oparser_band, &pb_settings);
							pb_settings.allPass = FALSE;
							pb_settings.acceptNullPackets = FALSE;
							pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
							if(pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer == DI_DEMUX_INPUT_NONE)
							{
								pb_settings.sourceTypeSettings.inputBand = P_DEMUX_GetUnusedInputBand(); //P_DEMUX_INBAND_NOT_USED;
							}
							else
							{
								pb_settings.sourceTypeSettings.inputBand = P_DEMUX_GetInputBand(OutputDevice_id);

							}
							pb_settings.transportType = NEXUS_TransportType_eTs;

							rc = NEXUS_ParserBand_SetSettings(oparser_band, &pb_settings);
							if(rc != NEXUS_SUCCESS)
							{
								PrintError(" Failed to set main ParserBand (%d)\n", rc);
								drvError = DRV_ERR;
							}
							else
							{
								pDemux->ciInfo[ciSlotId].bPassthrough = TRUE;
							}

						}
					}
				}

				pDemux->dmxDevice[ulDemuxId].nHiddenParserIndex = 0;
				pDemux->ciInfo[ciSlotId].OutputDevice_id = OutputDevice_id;

#else
				// Remux setting for externel route
				drvError = P_DEMUX_UpdateCAMPath(Explicit_Connection_Req, ciSlotId,TRUE,0,0,ulDemuxId);
				if(drvError != DRV_OK)
				{
					VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
					VK_SEM_Release(ciLock);
					PrintError("Failed routing path to CAM (for Playback).(drvError = %ld)\n", drvError);
					return DI_ERR_ERROR;
				}

				pDemux->dmxDevice[ulDemuxId].nHiddenParserIndex = P_DEMUX_GetAvailableHiddenParserIndex(ulDemuxId);
				parser_band = P_DEMUX_GetParserBandByDemuxId(ulDemuxId); /*NEXUS_ParserBand_e6; */
				NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
				pb_settings.allPass = FALSE;
				pb_settings.acceptNullPackets = FALSE;
				//pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
				//pb_settings.sourceTypeSettings.inputBand = INPUTBAND_FROM_REMUX_OUTPUT;
				pb_settings.sourceType = NEXUS_ParserBandSourceType_eRemux;
				pb_settings.sourceTypeSettings.remux = pDemux->rmxHandle;
				pb_settings.transportType = NEXUS_TransportType_eTs;

				inputBand = pb_settings.sourceTypeSettings.inputBand ;
				rc = NEXUS_ParserBand_SetSettings(parser_band, &pb_settings);
				if(rc != NEXUS_SUCCESS)
				{
					VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
					VK_SEM_Release(ciLock);
					PrintError(" Failed to set main ParserBand (%d)\n", rc);
					return DI_ERR_ERROR;
				}
#endif
#endif

				pDemux->ciInfo[ciSlotId].etProducer = etProducer;
				pDemux->ciInfo[ciSlotId].ulInputDevice_id = ulInputDevice_id;


			}

			if(pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id != ciSlotId)
			{
				pDemux->ciInfo[ciSlotId].ciUsageCount++;
			}
#endif
		}
		else
		{
			/*Check if CI is used before*/
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id >= 0)
			{
				/*Decrease CI Usage count */
				//Release CI route path
				if(pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].ciUsageCount  > 0)
				{
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
					OutputDevice_id = pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].OutputDevice_id;
					P_DEMUX_SetConnectCI(OutputDevice_id,FALSE);
					P_DEMUX_GetConnectCI(OutputDevice_id,&CIConnectStatus);
#endif

					pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].ciUsageCount --;

#if defined(CONFIG_CI)
					if(pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].ciUsageCount  == 0)
					{
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
						drvError = P_DEMUX_DestroyRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
						pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId = 0xff;
						drvError = DRV_SetTS2InputBand(OutputDevice_id);
						pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].bPassthrough = FALSE;
						pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].OutputDevice_id = 0xff;


#else
						P_DEMUX_UpdateCAMPath(Explicit_Disconnection_Req, pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id,0,0,0,ulDemuxId);
#if !defined(CONFIG_ZAPPING_ONLY)
						P_DEMUX_UpdateCAMPath(Implicit_Connection_Req, pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id,0,0,0,ulDemuxId);
#endif
#endif
					}
#endif
				}
				else
				{
					/*somethings wrong.*/
				}
			}

#if defined(CONFIG_CI)
			/*In case of scrambled file player, routing thru remux is required regardless of CI Id.*/
			if(etProducer == DI_DEMUX_INPUT_SCRAMBLED_FILE_PLAYER || etProducer == DI_DEMUX_INPUT_SCRAMBLED_FILE_RECORDER )
			{
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
				drvError = P_DEMUX_OpenShadowPlaybackPath(ulDemuxId);
				if(drvError != DRV_OK)
				{
					VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
					VK_SEM_Release(ciLock);
					PrintError("Failed creating shadow path (for Playback).(drvError = %ld)\n", drvError);
					return DI_ERR_ERROR;
				}
				pDemux->dmxDevice[ulDemuxId].nHiddenParserIndex = P_DEMUX_GetAvailableHiddenParserIndex(ulDemuxId);
				parser_band = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);
				NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
				pb_settings.allPass = FALSE;
				pb_settings.acceptNullPackets = FALSE;
				pb_settings.sourceType = NEXUS_ParserBandSourceType_eRemux;
				pb_settings.sourceTypeSettings.remux = pDemux->shadowPBPath.hRmx;
				pb_settings.transportType = NEXUS_TransportType_eTs;

				rc = NEXUS_ParserBand_SetSettings(parser_band, &pb_settings);
				if(rc != NEXUS_SUCCESS)
				{
					VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
					VK_SEM_Release(ciLock);
					PrintError(" Failed to set main ParserBand (%d)\n", rc);
					return DI_ERR_ERROR;
				}
#else
				pDemux->dmxDevice[ulDemuxId].nHiddenParserIndex = 0;

				drvError = P_DEMUX_RouteRemuxToRequestedPath(ulDemuxId,PLAYBACK_PLAY);
				if(drvError != DRV_OK)
				{
					P_DEMUX_DestroyRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
					pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId = 0xff;

					VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
					VK_SEM_Release(ciLock);
					PrintError("Failed creating shadow path (for Playback).(drvError = %ld)\n", drvError);
					return DRV_ERR;
				}

				pRemux = P_DEMUX_GetRemuxInstance(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
				parser_band = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);

				NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
				pb_settings.allPass = FALSE;
				pb_settings.acceptNullPackets = FALSE;
				pb_settings.sourceType = NEXUS_ParserBandSourceType_eRemux;
				pb_settings.sourceTypeSettings.remux = pRemux->rmxHandle;
				pb_settings.transportType = NEXUS_TransportType_eTs;

				rc = NEXUS_ParserBand_SetSettings(parser_band, &pb_settings);
				if(rc != NEXUS_SUCCESS)
				{
					VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
					VK_SEM_Release(ciLock);
					PrintError(" Failed to set main ParserBand (%d)\n", rc);
					return DRV_ERR;
				}
#endif

			}
			else
			{
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
				P_DEMUX_CloseShadowPlaybackPath(); /*Close shadow path if it's opened.*/
#endif
				pDemux->dmxDevice[ulDemuxId].nHiddenParserIndex = -1; /*No hidden parser used.*/
			}
#endif
		}

#if defined(CONFIG_PVR)
		if(etProducer == DI_DEMUX_INPUT_RECORDER || etProducer == DI_DEMUX_INPUT_SCRAMBLED_FILE_RECORDER )
		{
#if defined(CONFIG_TRANSCODER) /*FIXIT: compatilbility issue*/
			DRV_PVR_PLAY_SetRecordPath(ulDemuxId, ulInputDevice_id);
#else
			DRV_PVR_PLAY_SetRecordPath(DEF_PB_DEV_ID, ulInputDevice_id);
#endif
			/* only store PB parserBand information for debug */
			parser_band = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);
		}
#endif
	}
	else
	{
		VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
		VK_SEM_Release(ciLock);
		PrintError("Unacceptable input path...\n");
		return DI_ERR_ERROR;
	}

#if defined(CONFIG_CI)
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
#if !defined(CONFIG_ZAPPING_ONLY)
	if((ciSlotId >= 0)&&(ciSlotId < CAM_MAXSLOTID))
	{
		P_DEMUX_UpdateCAMPath(Implicit_Connection_Req, ciSlotId,0,0,0,ulDemuxId);
	}
#endif
#endif
#endif

	pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer = etProducer;
	pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id = ulInputDevice_id;
	pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer = etConsumer;
	pDemux->dmxDevice[ulDemuxId].pathInfo.ulConsumerGroupId = ulConsumerGroupId;

	pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id = ciSlotId;
	pDemux->dmxDevice[ulDemuxId].pathInfo.inputBand = inputBand;
	if(etProducer != DI_DEMUX_INPUT_NONE)
	{
		pDemux->dmxDevice[ulDemuxId].pathInfo.ulRequestedTunerId = TUNER_NONE;
	}
	pDemux->dmxDevice[ulDemuxId].pathInfo.parserBand = parser_band;

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	pDemux->dmxDevice[ulDemuxId].bIsUsing++;
#else
	pDemux->dmxDevice[ulDemuxId].bIsUsing = TRUE;
#endif
	if(etConsumer == DI_DEMUX_OUTPUT_RECORDER)
	{
		if(ulConsumerGroupId < MAX_REC_CHANNEL)
			gaRecorder2DemuxMapper[ulConsumerGroupId] = ulDemuxId;
	}

	VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
	VK_SEM_Release(ciLock);

	/*Set Output Path*/

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_ReleasePath(HUINT32 ulDemuxId)
{
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBand parser_band;
	NEXUS_ParserBandSettings pb_settings;
#if defined(CONFIG_BCM_MTSIF)
	DRV_Error eDrvError;
	HBOOL bIsTSModeMtsif = FALSE;
#endif
	NEXUS_Error rc = 0;

	PrintEnter();
	DI_UART_Print("[RelPath] ulDemuxId = %ld\n", ulDemuxId);
	pDemux = P_DEMUX_GetInstance();
	if(ulDemuxId >= MAX_DEMUX_NUM)
	{
		PrintError(" failed: Demux %d is invalid \n",ulDemuxId);
		return DI_ERR_INVALID_PARAM;
	}

	VK_SEM_Get(ciLock);
	VK_SEM_Get(pDemux->dmxDevice[ulDemuxId].lock);
	if(pDemux->dmxDevice[ulDemuxId].bIsUsing == FALSE){
		VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
		VK_SEM_Release(ciLock);
		PrintError(" failed: Demux %d is  already released \n",ulDemuxId);
		return DI_ERR_OK;
	}

	if(ulDemuxId >=LIVE_DEMUX_NUM)
	{
		pDemux->dmxDevice[ulDemuxId].nHiddenParserIndex = -1; /*No hidden parser used.*/
#if defined(CONFIG_CI)
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
		P_DEMUX_CloseShadowPlaybackPath();
#else
		P_DEMUX_DestroyRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
		pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId = 0xff;
#endif
#endif
	}

	//sem-protection required.
	if(pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id != -1)
	{
		//Release CI route path
		if(pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].ciUsageCount  > 0)
		{
			pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].ciUsageCount --;
#if defined(CONFIG_CI)
			if(pDemux->ciInfo[pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id].ciUsageCount  == 0)
			{
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
				P_DEMUX_DestroyRemux(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId);
				pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId = 0xff;
#else
				P_DEMUX_UpdateCAMPath(Explicit_Disconnection_Req, pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id,0,0,0,ulDemuxId);
#if !defined(CONFIG_ZAPPING_ONLY)
				P_DEMUX_UpdateCAMPath(Implicit_Connection_Req, pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id,0,0,0,ulDemuxId);
#endif
#endif

			}
#endif
		}
		else
		{
			/*somethings wrong.*/
		}
		pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id = -1;
	}

	if(ulDemuxId < LIVE_DEMUX_NUM)
	{
		if(P_DEMUX_GetSharingParserBandCnt(ulDemuxId) == NOT_USED)
		{
		parser_band = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);
#if defined(CONFIG_BCM_MTSIF)
		if(DRV_CH_GetTSOutputMode(pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id) == CH_TS_MODE_MTSIF)
		{
			bIsTSModeMtsif = TRUE;
		}
		else
		{
			bIsTSModeMtsif = FALSE;
		}

		if(!bIsTSModeMtsif)
		{
#endif
		NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
		pb_settings.allPass = FALSE;
		pb_settings.acceptNullPackets = FALSE;
		pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
		pb_settings.sourceTypeSettings.inputBand =  P_DEMUX_GetUnusedInputBand(); //P_DEMUX_INBAND_NOT_USED;
		pb_settings.transportType = NEXUS_TransportType_eTs;
		rc = NEXUS_ParserBand_SetSettings(parser_band, &pb_settings);
		if(rc != NEXUS_SUCCESS)
		{
			VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
			VK_SEM_Release(ciLock);
			PrintError(" Failed to set main ParserBand (%d)\n", rc);
			return DI_ERR_ERROR;
		}
#if defined(CONFIG_BCM_MTSIF)
		}
		else
		{
		eDrvError = P_DEMUX_SetMTSIFPath(pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer, pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id, parser_band, FALSE, FALSE);
		if(eDrvError != DRV_OK)
		{
			VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
			VK_SEM_Release(ciLock);
			PrintError(" Failed to set main ParserBand (%d)\n", eDrvError);
			return DI_ERR_ERROR;
		}
		}
#endif
	}
	}
	pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer = DI_DEMUX_INPUT_NONE;
	pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id = 0xff;
	pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer = DI_DEMUX_OUTPUT_NONE;
	pDemux->dmxDevice[ulDemuxId].pathInfo.ulConsumerGroupId = 0;
	pDemux->dmxDevice[ulDemuxId].pathInfo.ulRequestedTunerId= TUNER_NONE;
	pDemux->dmxDevice[ulDemuxId].pathInfo.parserBand = NEXUS_ParserBand_eInvalid;

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	pDemux->dmxDevice[ulDemuxId].bIsUsing--;
#else
	pDemux->dmxDevice[ulDemuxId].bIsUsing = FALSE;
#endif
	if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_RECORDER)
	{
			gaRecorder2DemuxMapper[pDemux->dmxDevice[ulDemuxId].pathInfo.ulConsumerGroupId] = INVALID_32BITS;
	}

	VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);
	VK_SEM_Release(ciLock);

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_GetPath(HUINT32 ulDemuxId ,
								   DI_DEMUX_INPUTDEVICETYPE * etProducer, HUINT32 * ulInputDevice_id,
								   DI_DEMUX_OUTPUTDEVICETYPE * etConsumer, HUINT32  * ulConsumerGroupId,
								   HINT32 * nCI_Id)

{
	DI_ERR_CODE diErr = DI_ERR_OK;
	int ret;
	P_DEMUX_Module *pDemux;

	PrintEnter();

	pDemux = P_DEMUX_GetInstance();
	if ((etProducer == NULL) || (ulInputDevice_id == NULL) || (etConsumer == NULL)
		                                 || (ulConsumerGroupId == NULL) ||  (nCI_Id == NULL))
	{
		return DI_ERR_INVALID_PARAM;
	}

	ret = VK_SEM_Get(pDemux->dmxDevice[ulDemuxId].lock);
	*etProducer				= pDemux->dmxDevice[ulDemuxId].pathInfo.etProducer;
	*ulInputDevice_id		= pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id;
	*etConsumer			= pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer;
	*ulConsumerGroupId		= pDemux->dmxDevice[ulDemuxId].pathInfo.ulConsumerGroupId;
	*nCI_Id				= pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id;
	if(pDemux->dmxDevice[ulDemuxId].bIsUsing == FALSE)
	{
		diErr = DI_ERR_ERROR;
	}
	VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);

	PrintExit();

	return diErr;
}

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
DI_ERR_CODE DI_DEMUX_GetPrimerSTC (HUINT32 *pulHighBitSTC, HUINT32 *pulLowBitSTC)
{
	DRV_Error nDrvErr = DRV_OK;
	HUINT32 ultempSTC;
	HUINT32 ulPrimerIndex=0;

	PrintEnter();

	if(DRV_VIDEO_IsPrimerStarted())
	{
		nDrvErr = DRV_VIDEO_GetPrimerIndex(DEF_VIDEO_DEV_ID, &ulPrimerIndex);
		if(nDrvErr != DRV_OK)
		{
			PrintError("DRV_VIDEO_GetPrimerIndex be failed!\n");
	        return DI_ERR_ERROR;
		}

		PrintDebug("[%s] ulPrimerIndex : %d\n", __FUNCTION__, ulPrimerIndex);

		if(primerStcChannel[ulPrimerIndex] == NULL)
		{
			PrintError(" Error Primer[%d] STC channel is NULL.\n", ulPrimerIndex);
	        return DI_ERR_ERROR;
		}

		NEXUS_StcChannel_GetStc(primerStcChannel[ulPrimerIndex], pulLowBitSTC);
	}
	else
	{
		PrintDebug("[%s] Get data from the default STC channel (for playback's subtitle, or non-primer).\n", __FUNCTION__);
		if(shstcChannel == NULL)
		{
			PrintError(" Error STC channel is NULL.\n");
			return DI_ERR_ERROR;
		}

		NEXUS_StcChannel_GetStc(shstcChannel, pulLowBitSTC);
	}

	ultempSTC = *pulLowBitSTC;
	if(ultempSTC & 0x80000000)
	{
		*pulHighBitSTC = 1;
	}
	else
	{
		*pulHighBitSTC = 0;
	}

	*pulLowBitSTC = (ultempSTC & 0x7fffffff) << 1;

	PrintExit();

	return DI_ERR_OK;
}
#endif

DI_ERR_CODE DI_DEMUX_GetSTC (HUINT32 ulDemuxId,HUINT32 *pulHighBitSTC, HUINT32 *pulLowBitSTC)
{
	HUINT32 tempSTC;

	BSTD_UNUSED(ulDemuxId);

	PrintEnter();
	if(shstcChannel == NULL)
		return DI_ERR_ERROR;
#if 0
	NEXUS_StcChannel_GetStc(shstcChannel, pulHighBitSTC);
	*pulLowBitSTC = 0;
#else
	NEXUS_StcChannel_GetStc(shstcChannel, pulLowBitSTC);
	//DI_UART_Print("DI STC = %lx\n", *pulLowBitSTC);
	tempSTC = *pulLowBitSTC;
	if(tempSTC & 0x80000000)
		*pulHighBitSTC = 1;
	else
		*pulHighBitSTC = 0;
	*pulLowBitSTC =  (tempSTC & 0x7fffffff) << 1;
#endif
	PrintExit();

	return DI_ERR_OK;
}

//DUMMY
void DI_DEMUX_GetTsState(HUINT32 ulDmxId, HUINT16 usPid, DI_DEMUX_TsState_t *petTSState)
{
	BSTD_UNUSED(ulDmxId);
	BSTD_UNUSED(usPid);
	BSTD_UNUSED(petTSState);

	PrintEnter();

	PrintExit();

	return;
}

DI_ERR_CODE DI_DEMUX_RegisterTSStateCallback (HUINT32 ulDemuxId, pfnDI_DEMUX_TSStateNotify pfnNotify)
{
	P_DEMUX_Module *pDemux;
	BSTD_UNUSED(ulDemuxId);
	BSTD_UNUSED(pfnNotify);

	PrintEnter();
	pDemux = P_DEMUX_GetInstance();
	VK_SEM_Get(cbLockTS);
	pDemux->callback.pfnTSStateNotify = pfnNotify;
	VK_SEM_Release(cbLockTS);
	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_StartPID(HUINT32 ulDemuxId, Pid_Info *ppidInfo)
{
	DRV_Error error = DRV_OK;
	HUINT32 localPidBitMask = 0;

#if defined(CONFIG_TRANSCODER)
	P_DEMUX_Module *pDemux = NULL;
#endif

	if(ulDemuxId >= MAX_DEMUX_NUM )
	{
		PrintError(" failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}

#if defined(CONFIG_TRANSCODER)
	pDemux = P_DEMUX_GetInstance();
#endif

	if(ppidInfo->pidBitmask & PID_BITMASK_PCR)
	{
		localPidBitMask |= PID_BITMASK_PCR;

		error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_PCR, ppidInfo->usPCRPid);
		if(error != DRV_OK)
		{
			PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_PCR,0x%x) Error \n",ulDemuxId,ppidInfo->usPCRPid);
			goto pid_error;
		}
	}
	if(ppidInfo->pidBitmask & PID_BITMASK_VIDEO)
	{
		localPidBitMask |= PID_BITMASK_VIDEO;

		error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_VIDEO, ppidInfo->usVideoPid);
		if(error != DRV_OK)
		{
			PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_VIDEO,0x%x) Error \n",ulDemuxId,ppidInfo->usVideoPid);
			goto pid_error;
		}
	}
	if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_MAIN)
	{
		localPidBitMask |= PID_BITMASK_AUDIO_MAIN;
		error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_MAIN, ppidInfo->usAudioMainPid);
		if(error != DRV_OK)
		{
			PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_AUDIO_MAIN,0x%x) Error \n",ulDemuxId,ppidInfo->usAudioMainPid);
			goto pid_error;

		}
	}
	if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_SUB)
	{
		localPidBitMask |= PID_BITMASK_AUDIO_SUB;
		error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_SUB, ppidInfo->usAudioSubPid);
		if(error != DRV_OK)
		{
			PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_AUDIO_SUB,0x%x) Error \n",ulDemuxId,ppidInfo->usAudioSubPid);
			goto pid_error;
		}
	}
	if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_SPDIF)
	{
		localPidBitMask |= PID_BITMASK_AUDIO_SPDIF;
		error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF, ppidInfo->usAudioSpdifPid);
		if(error != DRV_OK)
		{
			PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF,0x%x) Error \n",ulDemuxId,ppidInfo->usAudioSpdifPid);
			goto pid_error;
		}
	}

#if defined(CONFIG_PIP)
	if(ppidInfo->pidBitmask & PID_BITMASK_PCR_SUB)
	{
		localPidBitMask |= PID_BITMASK_PCR_SUB;
		error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_PCR_SUB, ppidInfo->usPCRPid);
		if(error != DRV_OK)
		{
			PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_PCR_SUB,0x%x) Error \n",ulDemuxId,ppidInfo->usPCRPid);
			goto pid_error;
		}
	}

	if(ppidInfo->pidBitmask & PID_BITMASK_VIDEO_SUB)
	{
		localPidBitMask |= PID_BITMASK_VIDEO_SUB;
		error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_VIDEO_SUB, ppidInfo->usVideoPid);
		if(error != DRV_OK)
		{

			PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_VIDEO_SUB,0x%x) Error \n",ulDemuxId,ppidInfo->usVideoPid);
			goto pid_error;
		}
	}
#endif

#if defined(CONFIG_TRANSCODER)
	if(ppidInfo->pidBitmask & PID_BITMASK_PCR_TRANS_1)
	{
		localPidBitMask |= PID_BITMASK_PCR_TRANS_1;
		if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
		{
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_PCR_TRANS_1, ppidInfo->usPCRPid);
			if(error != DRV_OK)
			{
				PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_PCR_TRANS_1,0x%x) Error \n",ulDemuxId,ppidInfo->usPCRPid);
				goto pid_error;
			}
		}
		else
		{
			PrintError("pDemux->dmxDevice[%d].pathInfo.etConsumer = %d Error \n",ulDemuxId,pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer);
			goto pid_error;
		}
	}
	if(ppidInfo->pidBitmask & PID_BITMASK_VIDEO_TRANS_1)
	{
		if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
		{
			localPidBitMask |= PID_BITMASK_VIDEO_TRANS_1;
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_1, ppidInfo->usVideoPid);
			if(error != DRV_OK)
			{
				PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_1,0x%x) Error \n",ulDemuxId,ppidInfo->usVideoPid);
				goto pid_error;
			}
		}
		else
		{
			ppidInfo->pidBitmask = 0;
			PrintError("pDemux->dmxDevice[%d].pathInfo.etConsumer = %d Error \n",ulDemuxId,pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer);
			return DI_ERR_ERROR;
		}
	}
	if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_TRANS_1)
	{
		if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
		{
			localPidBitMask |= PID_BITMASK_AUDIO_TRANS_1;
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_1, ppidInfo->usAudioMainPid);
			if(error != DRV_OK)
			{
				PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_1,0x%x) Error \n",ulDemuxId,ppidInfo->usAudioMainPid);
				goto pid_error;
			}
		}
		else
		{
			ppidInfo->pidBitmask = 0;
			PrintError("pDemux->dmxDevice[%d].pathInfo.etConsumer = %d Error \n",ulDemuxId,pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer);
			return DI_ERR_ERROR;
		}
	}
	if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_SUB_TRANS_1)
	{
		if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
		{
			localPidBitMask |= PID_BITMASK_AUDIO_SUB_TRANS_1;
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1, ppidInfo->usAudioSubPid);
			if(error != DRV_OK)
			{
				PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_1,0x%x) Error \n",ulDemuxId,ppidInfo->usAudioSubPid);
				goto pid_error;
			}
		}
		else
		{
			ppidInfo->pidBitmask = 0;
			PrintError("pDemux->dmxDevice[%d].pathInfo.etConsumer = %d Error \n",ulDemuxId,pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer);
			return DI_ERR_ERROR;
			}
		}

	if(ppidInfo->pidBitmask & PID_BITMASK_PCR_TRANS_2)
	{
		if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
		{
			localPidBitMask |= PID_BITMASK_PCR_TRANS_2;
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_PCR_TRANS_2, ppidInfo->usPCRPid);
			if(error != DRV_OK)
			{
				PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_PCR_TRANS_2,0x%x) Error \n",ulDemuxId,ppidInfo->usPCRPid);
				goto pid_error;
			}
		}
		else
		{
			ppidInfo->pidBitmask = 0;
			PrintError("pDemux->dmxDevice[%d].pathInfo.etConsumer = %d Error \n",ulDemuxId,pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer);
			return DI_ERR_ERROR;
		}

	}
	if(ppidInfo->pidBitmask & PID_BITMASK_VIDEO_TRANS_2)
	{
		if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
		{
			localPidBitMask |= PID_BITMASK_VIDEO_TRANS_2;
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_2, ppidInfo->usVideoPid);
			if(error != DRV_OK)
			{
				PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_2,0x%x) Error \n",ulDemuxId,ppidInfo->usVideoPid);
				goto pid_error;
			}
		}
		else
		{
			ppidInfo->pidBitmask = 0;
			PrintError("pDemux->dmxDevice[%d].pathInfo.etConsumer = %d Error \n",ulDemuxId,pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer);
			return DI_ERR_ERROR;
		}
	}
	if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_TRANS_2)
	{
		if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
		{
			localPidBitMask |= PID_BITMASK_AUDIO_TRANS_2;
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_2, ppidInfo->usAudioMainPid);
			if(error != DRV_OK)
			{
				PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_2,0x%x) Error \n",ulDemuxId,ppidInfo->usAudioMainPid);
				goto pid_error;
			}
		}
		else
		{
			ppidInfo->pidBitmask = 0;
			PrintError("pDemux->dmxDevice[%d].pathInfo.etConsumer = %d Error \n",ulDemuxId,pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer);
			return DI_ERR_ERROR;
		}
	}
	if(ppidInfo->pidBitmask & PID_BITMASK_AUDIO_SUB_TRANS_2)
	{
		if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
		{
			localPidBitMask |= PID_BITMASK_AUDIO_SUB_TRANS_2;
			error = P_DEMUX_StartPID(ulDemuxId, DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2, ppidInfo->usAudioSubPid);
			if(error != DRV_OK)
			{
				PrintError("P_DEMUX_StartPID(0x%x,DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2,0x%x) Error \n",ulDemuxId,ppidInfo->usAudioSubPid);
				goto pid_error;
			}
		}
		else
		{
			ppidInfo->pidBitmask = 0;
			PrintError("pDemux->dmxDevice[%d].pathInfo.etConsumer = %d Error \n",ulDemuxId,pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer);
			return DI_ERR_ERROR;
		}
	}
#endif

	ppidInfo->pidBitmask = localPidBitMask;
	return error;

pid_error:
	PrintError("P_DEMUX_StartPID(DMX ID : %d) Error..... Stopping all PIDs [%08x]\n",ulDemuxId, localPidBitMask);
	P_DEMUX_StopAllPIDS(ulDemuxId, localPidBitMask);
	ppidInfo->pidBitmask = 0;
	return DI_ERR_ERROR;


}

DI_ERR_CODE DI_DEMUX_StopPID(HUINT32 ulDemuxId, HUINT32 pidBitmask)
{
	DI_ERR_CODE diError = DI_ERR_OK;
	DRV_Error error;

#if defined(CONFIG_TRANSCODER)
	P_DEMUX_Module *pDemux = NULL;
#endif

	if(ulDemuxId >= MAX_DEMUX_NUM )
	{
		PrintError(" failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}

#if defined(CONFIG_TRANSCODER)
	pDemux = P_DEMUX_GetInstance();
#endif

	if(pidBitmask & PID_BITMASK_AUDIO_SPDIF)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF);
		if(error != DRV_OK)
		{
			diError = DI_ERR_ERROR;
		}
	}
	if(pidBitmask & PID_BITMASK_AUDIO_SUB)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_SUB);
		if(error != DRV_OK)
		{
			diError = DI_ERR_ERROR;
		}
	}
	if(pidBitmask & PID_BITMASK_AUDIO_MAIN)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_MAIN);
		if(error != DRV_OK)
		{
			diError = DI_ERR_ERROR;
		}
	}
	if(pidBitmask & PID_BITMASK_VIDEO)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_VIDEO);
		if(error != DRV_OK)
		{
			diError = DI_ERR_ERROR;
		}
	}
	if(pidBitmask & PID_BITMASK_PCR)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_PCR);
		if(error != DRV_OK)
		{
			diError = DI_ERR_ERROR;
		}
	}

#if defined(CONFIG_PIP)
	if(pidBitmask & PID_BITMASK_VIDEO_SUB)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_VIDEO_SUB);
		if(error != DRV_OK)
		{
			diError = DI_ERR_ERROR;
		}
	}
	if(pidBitmask & PID_BITMASK_PCR_SUB)
	{
		error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_PCR_SUB);
		if(error != DRV_OK)
		{
			diError = DI_ERR_ERROR;
		}
	}
#endif

#if defined(CONFIG_TRANSCODER)
		if(pidBitmask & PID_BITMASK_AUDIO_TRANS_1)
		{
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
			{
				error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_1);
				if(error != DRV_OK)
				{
					diError = DI_ERR_ERROR;
				}
			}
			else
			{
				diError = DI_ERR_ERROR;
			}
		}
		if(pidBitmask & PID_BITMASK_AUDIO_SUB_TRANS_1)
		{
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
			{
				error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1);
				if(error != DRV_OK)
				{
					diError = DI_ERR_ERROR;
				}
			}
			else
			{
				diError = DI_ERR_ERROR;
			}
		}

		if(pidBitmask & PID_BITMASK_VIDEO_TRANS_1)
		{
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
			{
				error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_1);
				if(error != DRV_OK)
				{
					diError = DI_ERR_ERROR;
				}
			}
			else
			{
				diError = DI_ERR_ERROR;
			}
		}
		if(pidBitmask & PID_BITMASK_PCR_TRANS_1)
		{
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
			{
				error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_PCR_TRANS_1);
				if(error != DRV_OK)
				{
					diError = DI_ERR_ERROR;
				}
			}
			else
			{
				diError = DI_ERR_ERROR;
			}
		}

		if(pidBitmask & PID_BITMASK_AUDIO_TRANS_2)
		{
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
			{
				error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_2);
				if(error != DRV_OK)
				{
					diError = DI_ERR_ERROR;
				}
			}
			else
			{
				diError = DI_ERR_ERROR;
			}
		}

		if(pidBitmask & PID_BITMASK_AUDIO_SUB_TRANS_2)
		{
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
			{
				error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2);
				if(error != DRV_OK)
				{
					diError = DI_ERR_ERROR;
				}
			}
			else
			{
				diError = DI_ERR_ERROR;
			}
		}

		if(pidBitmask & PID_BITMASK_VIDEO_TRANS_2)
		{
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
			{
				error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_2);
				if(error != DRV_OK)
				{
					diError = DI_ERR_ERROR;
				}
			}
			else
			{
				diError = DI_ERR_ERROR;
			}
		}
		if(pidBitmask & PID_BITMASK_PCR_TRANS_2)
		{
			if(pDemux->dmxDevice[ulDemuxId].pathInfo.etConsumer == DI_DEMUX_OUTPUT_TRANSCODER)
			{
				error = P_DEMUX_StopPID(ulDemuxId,DI_DEMUX_CHANNELTYPE_PCR_TRANS_2);
				if(error != DRV_OK)
				{
					diError = DI_ERR_ERROR;
				}
			}
			else
			{
				diError = DI_ERR_ERROR;
			}
		}
#endif

	return diError;
}

DI_ERR_CODE DI_DEMUX_StartPrimerPID(HUINT32 ulDemuxId, PrimerPid_Info *pPrimerPidInfo)
{
	DI_ERR_CODE diError = DI_ERR_OK;
	DRV_Error drvErr = DRV_ERR;
	HUINT32 localPidBitMask = 0;

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
	if(pPrimerPidInfo == NULL)
	{
		PrintError("[%s] ERROR : pPrimerPidInfo is NULL \n", __func__);
		return DI_ERR_INVALID_PARAM;
	}

	if((ulDemuxId >= MAX_DEMUX_NUM) || (pPrimerPidInfo->ulPrimerIndex >= DEMUX_PRIMER_MAX_COUNT))
	{
		PrintError(" failed: invalid parameter, ulDemuxId:%d, ulPrimerIndex:%d\n", ulDemuxId, pPrimerPidInfo->ulPrimerIndex);
		return DI_ERR_INVALID_PARAM;
	}

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	if(pPrimerPidInfo->ulPidBitmask & PID_BITMASK_PCR_PRIMER)
	{
		drvErr = P_DEMUX_StartPcrPrimerPID(ulDemuxId, pPrimerPidInfo);
		if(drvErr == DRV_OK)
		{
			localPidBitMask |= PID_BITMASK_PCR_PRIMER;
		}
		else
		{
			P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
			pPrimerPidInfo->ulPidBitmask = 0;
			diError = DI_ERR_ERROR;
		}
	}

	if(pPrimerPidInfo->ulPidBitmask & PID_BITMASK_VIDEO_PRIMER)
	{
		drvErr = P_DEMUX_StartVideoPrimerPID(ulDemuxId, pPrimerPidInfo);
		if(drvErr == DRV_OK)
		{
			localPidBitMask |= PID_BITMASK_VIDEO_PRIMER;
		}
		else
		{
			P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
			pPrimerPidInfo->ulPidBitmask = 0;
			diError = DI_ERR_ERROR;
		}
	}
#endif

	if(pPrimerPidInfo->ulPidBitmask & PID_BITMASK_AUDIO_PRIMER)
	{
		drvErr = P_DEMUX_StartAudioPrimerPID(ulDemuxId, pPrimerPidInfo);
		if(drvErr == DRV_OK)
		{
			localPidBitMask |= PID_BITMASK_AUDIO_PRIMER;
		}
		else
		{
			P_DEMUX_StopAllPIDS(ulDemuxId,localPidBitMask);
			pPrimerPidInfo->ulPidBitmask = 0;
			diError = DI_ERR_ERROR;
		}
	}
	pPrimerPidInfo->ulPidBitmask = localPidBitMask;

#else
	BSTD_UNUSED(ulDemuxId);
	BSTD_UNUSED(pPrimerPidInfo);
	BSTD_UNUSED(drvErr);
	BSTD_UNUSED(localPidBitMask);
#endif

	return diError;
}

DI_ERR_CODE DI_DEMUX_StopPrimerPID(HUINT32 ulDemuxId, PrimerPid_Info *pPrimerPidInfo)
{
	DI_ERR_CODE diError = DI_ERR_OK;
	DRV_Error drvErr = DRV_ERR;

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
	if(pPrimerPidInfo == NULL)
	{
		PrintError("[%s] ERROR : pPrimerPidInfo is NULL \n", __func__);
		return DI_ERR_INVALID_PARAM;
	}

	if((ulDemuxId >= MAX_DEMUX_NUM) || (pPrimerPidInfo->ulPrimerIndex >= DEMUX_PRIMER_MAX_COUNT))
	{
		PrintError(" failed: invalid parameter, ulDemuxId:%d, ulPrimerIndex:%d\n", ulDemuxId, pPrimerPidInfo->ulPrimerIndex);
		return DI_ERR_INVALID_PARAM;
	}

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	if(pPrimerPidInfo->ulPidBitmask & PID_BITMASK_PCR_PRIMER)
	{
		drvErr = P_DEMUX_StopPcrPrimerPID(ulDemuxId, pPrimerPidInfo->ulPrimerIndex);
		if(drvErr != DRV_OK)
		{
			diError = DI_ERR_ERROR;
		}
	}

	if(pPrimerPidInfo->ulPidBitmask & PID_BITMASK_VIDEO_PRIMER)
	{
		drvErr = P_DEMUX_StopVideoPrimerPID(ulDemuxId, pPrimerPidInfo->ulPrimerIndex);
		if(drvErr != DRV_OK)
		{
			diError = DI_ERR_ERROR;
		}
	}
#endif

	if(pPrimerPidInfo->ulPidBitmask & PID_BITMASK_AUDIO_PRIMER)
	{
		drvErr = P_DEMUX_StopAudioPrimerPID(ulDemuxId, pPrimerPidInfo->ulPrimerIndex);
		if(drvErr != DRV_OK)
		{
			diError = DI_ERR_ERROR;
		}
	}
#else
	BSTD_UNUSED(ulDemuxId);
	BSTD_UNUSED(pPrimerPidInfo);
	BSTD_UNUSED(drvErr);
#endif

	return diError;
}

/*****************************************************************
 * Function		: DI_CI_GetPlaybackId
 * Description	:
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DI_DEMUX_GetPlaybackId( HUINT32 ulDemuxId ,HUINT32 *pPlayBackId)
{
	P_DEMUX_Module *pDemux = NULL;
	DI_ERR_CODE		diErr = DI_ERR_OK;
	HINT32 ret = 0;

	pDemux = P_DEMUX_GetInstance();

	ret = VK_SEM_Get(pDemux->dmxDevice[ulDemuxId].lock);
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	if((pDemux->dmxDevice[ulDemuxId].bIsUsing == FALSE)||(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId == 0xff)||(pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId >= REMUX_MAX_NUM))
	{
		diErr = DI_ERR_ERROR;
	}
	else
	{
		*pPlayBackId = pDemux->dmxDevice[ulDemuxId].pathInfo.ulRemuxId;
	}
#else
	if(pDemux->dmxDevice[ulDemuxId].bIsUsing == FALSE)
	{
		diErr = DI_ERR_ERROR;
	}
	else
	{
		*pPlayBackId = 1;
	}
#endif
	VK_SEM_Release(pDemux->dmxDevice[ulDemuxId].lock);


	return diErr;
}

DI_ERR_CODE DI_DEMUX_StartPID_EXT(HUINT32 ulDemuxId, Pid_Info *ppidInfo)
{
	return DI_DEMUX_StartPID(ulDemuxId, ppidInfo);
}

DI_ERR_CODE DI_DEMUX_StopPID_EXT(HUINT32 ulDemuxId, HUINT32 pidBitmask)
{
	return DI_DEMUX_StopPID(ulDemuxId, pidBitmask);
}

DI_ERR_CODE DI_DEMUX_RegisterPESCallback (HUINT32 ulDemuxId, DI_DEMUX_PESTYPE etPesType, pfnDI_DEMUX_PES_Notify pfnNotify,	pfnDI_DEMUX_AllocBuffer pfnAllocBuffer, pfnDI_DEMUX_FreeBufferForError pfnErrorFreeBuffer)
{
	P_DEMUX_Module *pDemux;
	BSTD_UNUSED(ulDemuxId);

	PrintEnter();
	pDemux = P_DEMUX_GetInstance();

	if(ulDemuxId >= MAX_DEMUX_NUM || etPesType >= DI_DEMUX_PESTYPE_MAX){
		PrintError(" failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}
	VK_SEM_Get(cbLockPES);
	pDemux->callback.pfnPesNotify[etPesType] = pfnNotify;
	pDemux->callback.pfnPesAllocBuffer[etPesType] = pfnAllocBuffer;
	pDemux->callback.pfnPesFreeBuffer[etPesType] = pfnErrorFreeBuffer;
	VK_SEM_Release(cbLockPES);
	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_StartPESFilter (HUINT32 ulDemuxId,HUINT16 usPid, DI_DEMUX_PESTYPE etPESType, HUINT32 ulRequestID)
{
	HUINT32 channel_index;
	HUINT8 filter_index;
	DRV_Error rc;
	HUINT8 pucValue[8];
	HUINT8 pucMask[8];
	HUINT8 pucExclusion[8];
	HUINT32 ulFilterLen = 8;
	HBOOL bIsOnce = FALSE;
	HBOOL bCheckCRC = FALSE;

	PrintEnter("ulDemuxId %d, usPid=0x%x, ulRequestID 0x%lx\n", ulDemuxId, usPid, ulRequestID);

	if(ulDemuxId >= MAX_DEMUX_NUM || usPid >= DI_DEMUX_INVALID_PID){
		PrintError(" failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}

	if(P_DEMUX_HasCapability(ulDemuxId, DI_DEMUX_CAP_PESFILTER) == FALSE){
		PrintError(" demux %d has no capability SECTION_FILTER\n");
		return DI_ERR_ERROR;
	}

	/* allocate & enable pid */
	rc = P_DEMUX_AllocateChannel(DI_DEMUX_CHANNELTYPE_PES, usPid,  &channel_index);
	if(rc != DRV_OK || channel_index == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError(" failed: P_DEMUX_AllocateChannel() return error %d \n", rc);
		return DI_ERR_ERROR;
	}

	rc = P_DEMUX_EnableChannel(DI_DEMUX_CHANNELTYPE_PES, ulDemuxId, channel_index, P_DEMUX_CHSUBT_eNone);
	if(rc != DRV_OK)
	{
		PrintError(" failed: P_DEMUX_EnableChannel() return error %d \n", rc);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}
	/********************/
	if(P_DEMUX_AllocateFilter(P_DEMUX_PesFilter, etPESType, &filter_index, ulRequestID) != DRV_OK)
	{
		PrintError(" DRV_DEMUX_AllocateFilter() return error\n");
		P_DEMUX_DisableChannel(channel_index);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}
	//DI_UART_Print("DI_DEMUX_StartPESFilter(): channel_index = %ld filter_index = %ld\n",channel_index, filter_index);
	if(P_DEMUX_EnableFilter(P_DEMUX_PesFilter, filter_index, channel_index, pucValue, pucMask, pucExclusion, ulFilterLen, bIsOnce, bCheckCRC, ulRequestID) != DRV_OK)
	{
		PrintError(" DRV_DEMUX_EnableFilter() return error\n");
		P_DEMUX_DeallocateFilter(P_DEMUX_PesFilter, filter_index);
		P_DEMUX_DisableChannel(channel_index);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}
#if defined(SAVE_FILTER_INFO)
	filterDbgInfo[filter_index].usPid = usPid;
#endif
	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_StopPESFilter (HUINT32 ulDemuxId, HUINT32 ulRequestID)
{
	P_DEMUX_Module *pDemux;
	HUINT8 filter_index, channel_index;

	BSTD_UNUSED(ulDemuxId);

	PrintEnter();

	pDemux = P_DEMUX_GetInstance();

	for(filter_index = 0; filter_index < MAX_SECTION_FILTER_NUM; filter_index++){
		if(pDemux->msgFilter[filter_index].requestId == ulRequestID  && pDemux->msgFilter[filter_index].filterType == P_DEMUX_PesFilter
			&& pDemux->msgFilter[filter_index].status == DRV_DEMUX_RESOURCE_ENABLED
			&& pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId == ulDemuxId){
			break;
		}
	}

	if(filter_index == MAX_SECTION_FILTER_NUM){
		PrintError(" no filter to release with RequestID(0x%lx)\n", ulRequestID);
		return DI_ERR_ERROR;
	}

	channel_index = pDemux->msgFilter[filter_index].channelIndex;

	if(P_DEMUX_DisableFilter(P_DEMUX_PesFilter, filter_index) != DRV_OK){
		PrintError(" P_DEMUX_DisableFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_DeallocateFilter(P_DEMUX_PesFilter, filter_index) != DRV_OK){
		PrintError(" P_DEMUX_DeallocateFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	/* disable & deallocate channel */
	if(P_DEMUX_DisableChannel(channel_index) != DRV_OK){
		PrintError(" P_DEMUX_DisableChannel() returns error.\n");
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_DeallocateChannel(channel_index) != DRV_OK){
		PrintError(" P_DEMUX_DeallocateChannel() returns error.\n");
		return DI_ERR_ERROR;
	}
	/**************************/

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_RegisterSectionCallback (HUINT32 ulDemuxId, pfnDI_DEMUX_Notify pfnNotify, pfnDI_DEMUX_AllocBuffer pfnAllocBuffer)
{
	P_DEMUX_Module *pDemux;
	BSTD_UNUSED(ulDemuxId);

	PrintEnter();
	pDemux = P_DEMUX_GetInstance();
	if(ulDemuxId >= MAX_DEMUX_NUM ){
		PrintError(" failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}

	VK_SEM_Get(cbLockSection);
	pDemux->callback.pfnSectionNotify = pfnNotify;
	pDemux->callback.pfnAllocBuffer = pfnAllocBuffer;
	VK_SEM_Release(cbLockSection);
	PrintExit();

	return DI_ERR_OK;

}

DI_ERR_CODE DI_DEMUX_SetSectionFilter (HUINT32 ulDemuxId,HUINT16 usPid,HUINT32 ulRequestID, HBOOL bIsOnce,
											HUINT32 ulFilterLen, HUINT8 * pucValue,HUINT8 * pucMask,HUINT8 * pucExclusion,
											HBOOL bCheckCRC)
{
	HUINT32 channel_index;
	HUINT8 filter_index;
	DRV_Error rc;


	PrintEnter("ulDemuxId %d, usPid=0x%x, ulRequestID 0x%lx, bIsOnce %d\n", ulDemuxId, usPid, ulRequestID, bIsOnce);

	if(ulDemuxId >= MAX_DEMUX_NUM || usPid >= DI_DEMUX_INVALID_PID || pucValue == NULL || pucMask == NULL || pucExclusion == NULL){
		PrintError(" Failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}

	if(ulFilterLen > SECTION_FILTER_LENGTH){
		PrintError(" Failed: not supported filter length %d \n", ulFilterLen);
		return DI_ERR_INVALID_PARAM;
	}

	if(P_DEMUX_HasCapability(ulDemuxId, DI_DEMUX_CAP_SECTIONFILTER) == FALSE){
		PrintError(" demux %d has no capability SECTION_FILTER\n", ulDemuxId);
		return DI_ERR_ERROR;
	}

	/* allocate & enable pid */
	rc = P_DEMUX_AllocateChannel(DI_DEMUX_CHANNELTYPE_SECTION, usPid, &channel_index);
	if(rc != DRV_OK || channel_index == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError(" Failed: P_DEMUX_AllocateChannel() return error %d \n", rc);
		return DI_ERR_ERROR;
	}

	rc = P_DEMUX_EnableChannel(DI_DEMUX_CHANNELTYPE_SECTION, ulDemuxId, channel_index, P_DEMUX_CHSUBT_eNone);
	if(rc != DRV_OK)
	{
		PrintError(" Failed: P_DEMUX_EnableChannel() return error %d \n", rc);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_AllocateFilter(P_DEMUX_PsiFilter, 0, &filter_index, ulRequestID) != DRV_OK)
	{
		PrintError(" DRV_DEMUX_AllocateFilter() return error\n");
		P_DEMUX_DisableChannel(channel_index);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_EnableFilter(P_DEMUX_PsiFilter, filter_index, channel_index, pucValue, pucMask, pucExclusion, ulFilterLen, bIsOnce, bCheckCRC, ulRequestID) != DRV_OK)
	{
		PrintError(" DRV_DEMUX_EnableFilter() return error\n");
		P_DEMUX_DeallocateFilter(P_DEMUX_PsiFilter, filter_index);
		P_DEMUX_DisableChannel(channel_index);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}

#if defined(SAVE_FILTER_INFO)
	filterDbgInfo[filter_index].usPid = usPid;
	filterDbgInfo[filter_index].ulFilterLen = ulFilterLen;
	filterDbgInfo[filter_index].ulRequestId = ulRequestID;
	VK_MEM_Memcpy(filterDbgInfo[filter_index].ucValue, pucValue, ulFilterLen);
	VK_MEM_Memcpy(filterDbgInfo[filter_index].ucMask, pucMask, ulFilterLen);
	VK_MEM_Memcpy(filterDbgInfo[filter_index].ucExclusion, pucExclusion, ulFilterLen);
#endif
#if defined(PRINT_KNOWN_SECTION_INFO)
	if(SectionDbgMode & 0x80000000 && pucValue[0]==0x02)
	{
		DI_UART_Print("[0x%08x] [SET FILTER]  PID=0x%04x FiltLen=%02d Value 0x%02x:0x%02x:0x%02x:0x%02x    Mask 0x%02x:0x%02x:0x%02x:0x%02x (filterindex = %d, reqid=0x%lx)\n"
	                 ,VK_TIMER_GetSystemTick() ,usPid, ulFilterLen, pucValue[0],pucValue[1], pucValue[2],pucValue[3],pucMask[0],pucMask[1],pucMask[2],pucMask[3],filter_index,ulRequestID);
	}
#endif

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_ReleaseSectionFilter (HUINT32 ulDemuxId, HUINT32	ulRequestID)
{
	P_DEMUX_Module *pDemux;
	HUINT8 filter_index, channel_index;

	BSTD_UNUSED(ulDemuxId);
	pDemux = P_DEMUX_GetInstance();
	PrintEnter("ulDemuxId %d, ulRequestID 0x%lx\n", ulDemuxId, ulRequestID);

	for(filter_index = 0; filter_index < MAX_SECTION_FILTER_NUM; filter_index++){
		if(pDemux->msgFilter[filter_index].requestId == ulRequestID && pDemux->msgFilter[filter_index].filterType == P_DEMUX_PsiFilter
			&& pDemux->msgFilter[filter_index].status == DRV_DEMUX_RESOURCE_ENABLED
			&& pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId == ulDemuxId){
			break;
		}
	}

	if(filter_index == MAX_SECTION_FILTER_NUM){
		PrintError(" no filter to release with RequestID(0x%lx)\n", ulRequestID);
		return DI_ERR_ERROR;
	}
#if defined(PRINT_KNOWN_SECTION_INFO)
	if(SectionDbgMode & 0x80000000 && filterDbgInfo[filter_index].ucValue[0]==0x02)
	{
		DI_UART_Print("[0x%08x] [REL FILTER] filterindex = %d reqid=%ld\n",VK_TIMER_GetSystemTick(),filter_index, ulRequestID);
	}
#endif
	channel_index = pDemux->msgFilter[filter_index].channelIndex;

	if(P_DEMUX_DisableFilter(P_DEMUX_PsiFilter, filter_index) != DRV_OK){
		PrintError(" P_DEMUX_DisableFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_DeallocateFilter(P_DEMUX_PsiFilter, filter_index) != DRV_OK){
		PrintError(" P_DEMUX_DeallocateFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	/* disable & deallocate channel */
	if(P_DEMUX_DisableChannel(channel_index) != DRV_OK){
		PrintError(" P_DEMUX_DisableChannel() returns error.\n");
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_DeallocateChannel(channel_index) != DRV_OK){
		PrintError(" P_DEMUX_DeallocateChannel() returns error.\n");
		return DI_ERR_ERROR;
	}
	/**************************/
	PrintExit();

	return DI_ERR_OK;
}

#if defined(CONFIG_DI20)
DI_ERR_CODE DI_DEMUX_InsertPacketModeOn(HUINT32 ulDemuxId, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize, HUINT32 ulInterval, HUINT32 *pulHandle)
{
	DRV_Error eDrvRet = DRV_OK;

	eDrvRet = P_DEMUX_InsertPacketModeOn(ulDemuxId, usPid, usPid, pucSection, ulSectionSize, pulHandle, ulInterval);
	if(eDrvRet == DRV_ERR_INVALID_PARAMETER)
	{
		return DI_ERR_INVALID_PARAM;
	}
	else if(eDrvRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_ReplacePacketModeOn(HUINT32 ulDemuxId, HUINT16 usVirtualPid, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize, HUINT32 *pulHandle)
{
	DRV_Error eDrvRet = DRV_OK;

	eDrvRet = P_DEMUX_InsertPacketModeOn(ulDemuxId, usVirtualPid, usPid, pucSection, ulSectionSize, pulHandle, P_DEMUX_PACKET_REPLACE_INTERVAL);
	if(eDrvRet == DRV_ERR_INVALID_PARAMETER)
	{
		return DI_ERR_INVALID_PARAM;
	}
	else if(eDrvRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_ReplacePacketModeOff(HUINT32 ulHandle)
{
	DRV_Error eDrvRet = DRV_OK;

	eDrvRet = P_DEMUX_InsertPacketModeOff(ulHandle);
	if(eDrvRet == DRV_ERR_INVALID_PARAMETER)
	{
		return DI_ERR_INVALID_PARAM;
	}
	else if(eDrvRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_InsertPacketModeOff(HUINT32 ulHandle)
{
	DRV_Error eDrvRet = DRV_OK;

	eDrvRet = P_DEMUX_InsertPacketModeOff(ulHandle);
	if(eDrvRet == DRV_ERR_INVALID_PARAMETER)
	{
		return DI_ERR_INVALID_PARAM;
	}
	else if(eDrvRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#else
DI_ERR_CODE DI_DEMUX_ReplacePacketModeOn(HUINT32 ulDemuxId, HUINT16 usPid, HUINT8 *pucReplaced, HUINT32 packetSize, HUINT32 *pulHandle)
{
	P_DEMUX_Module *pDemux;
	HUINT8 *pucPacketBuffer = NULL;
	int pktmodCnt;

	pDemux = P_DEMUX_GetInstance();

	if(ulDemuxId >= MAX_DEMUX_NUM || usPid >= DI_DEMUX_INVALID_PID || pucReplaced == NULL)
	{
		PrintError("DI_DEMUX_SetPacketReplaced() failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}
	VK_SEM_Get(pktsubLock);
	for(pktmodCnt=0; pktmodCnt<MAX_PKTSUB_MODULE_NUM; pktmodCnt++)
	{
		if(   pDemux->stPktSubstitution[pktmodCnt].Status == DRV_DEMUX_RESOURCE_ALLOCATED &&
			pDemux->stPktSubstitution[pktmodCnt].ulDemuxId == ulDemuxId &&
			pDemux->stPktSubstitution[pktmodCnt].usPid == usPid)
		{
			//packets registered already.
			break;
		}
	}
	if(pktmodCnt < MAX_PKTSUB_MODULE_NUM)
	{
		//packets registered already. just return error.
		VK_SEM_Release(pktsubLock);
		return DI_ERR_ERROR;
	}
	for(pktmodCnt=0; pktmodCnt<MAX_PKTSUB_MODULE_NUM; pktmodCnt++)
	{
		if(pDemux->stPktSubstitution[pktmodCnt].Status == DRV_DEMUX_RESOURCE_FREE)
		{
			//no more slot.
			break;
		}
	}
	if(pktmodCnt >= MAX_PKTSUB_MODULE_NUM)
	{
		//all slots in use.
		VK_SEM_Release(pktsubLock);
		return DI_ERR_NO_RESOURCE;
	}
	pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;
	VK_SEM_Release(pktsubLock);


	pucPacketBuffer = DD_MEM_Alloc(PSUB_PACKET_SIZE);
	if(pucPacketBuffer == NULL)
	{
		VK_SEM_Get(pktsubLock);
		pDemux->stPktSubstitution[pktmodCnt].Status = DRV_DEMUX_RESOURCE_FREE;
		VK_SEM_Release(pktsubLock);
		PrintError("DI_DEMUX_SetPacketReplaced() failed: no memory to save packet to be replaced.\n");
		return DI_ERR_ERROR;
	}

	VK_memset(pucPacketBuffer, 0xFF, PSUB_PACKET_SIZE);
	pucPacketBuffer[0] =  0x47;
	pucPacketBuffer[1] =  0x40;
	pucPacketBuffer[2] =  0x00;
	pucPacketBuffer[3] =  0x15;
	pucPacketBuffer[4] =  0x00;
	VK_memcpy(&pucPacketBuffer[5], pucReplaced, packetSize);

	pDemux->stPktSubstitution[pktmodCnt].ulDemuxId = ulDemuxId;
	pDemux->stPktSubstitution[pktmodCnt].usPid = usPid;
	pDemux->stPktSubstitution[pktmodCnt].pBuffer = pucPacketBuffer;
	pDemux->stPktSubstitution[pktmodCnt].buffSize = PSUB_PACKET_SIZE;
	*pulHandle = pktmodCnt;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_ReplacePacketModeOff(HUINT32 ulHandle)
{
	P_DEMUX_Module *pDemux;
	HUINT8 *pBuffbeFreed = NULL;

	if(ulHandle >= MAX_PKTSUB_MODULE_NUM)
	{
		PrintError("%s() failed: invalid parameter \n", __FUNCTION__);
		return DI_ERR_INVALID_PARAM;
	}

	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(pktsubLock);
	if(pDemux->stPktSubstitution[ulHandle].Status != DRV_DEMUX_RESOURCE_ALLOCATED)
	{
		VK_SEM_Release(pktsubLock);
		PrintError("%s() failed: slot is not in allocated status.\n",__FUNCTION__);
		return DI_ERR_ERROR;
	}
	pBuffbeFreed = pDemux->stPktSubstitution[ulHandle].pBuffer;
	pDemux->stPktSubstitution[ulHandle].Status = DRV_DEMUX_RESOURCE_FREE;
	VK_SEM_Release(pktsubLock);

	if(pBuffbeFreed != NULL)
	{
		DD_MEM_Free(pBuffbeFreed);
	}
	return DI_ERR_OK;
}
#endif

DI_ERR_CODE DI_DEMUX_StartTSCMonitor(HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 *pulHandle)
{
#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	int i;
	DI_ERR_CODE diError = DI_ERR_OK;

	VK_SEM_Get(gTscCheck.Lock);
	for(i=0; i<NUM_TSC_CHECK_RES; i++)
	{
		if(gTscCheck.stResource[i].bEnabled == FALSE)
		{
			gTscCheck.stResource[i].bEnabled = TRUE;
			gTscCheck.stResource[i].ulDemuxId = ulDemuxId;
			gTscCheck.stResource[i].usPid = usPid;
			gTscCheck.stResource[i].footPrint = (i<<24 |gFootPrint<<16);
			gFootPrint++;
			break;
		}
	}
	if(i==NUM_TSC_CHECK_RES)
	{
		diError = DI_ERR_ERROR;
		PrintError("No more resources\n");
	}
	else
	{
		if(gTscCheck.stResource[i].bFilterOn == FALSE)
		{
			P_DEMUX_StartTSFilter(i);
			gTscCheck.stResource[i].bFilterOn = TRUE;
		}
		*pulHandle = i;
	}

	VK_SEM_Release(gTscCheck.Lock);

	return diError;
#else
	DRV_Error rc;
	HUINT32 logicalPidChannel;

	PrintEnter();

	if(ulDemuxId >= MAX_DEMUX_NUM || usPid >= DI_DEMUX_INVALID_PID || pulHandle == NULL)
	{
		PrintError(" Invalid parameter (dmxId=%ld usPid=0x%lx pHandle = 0x%lx)\n", ulDemuxId, usPid, pulHandle);
		return DI_ERR_INVALID_PARAM;
	}

	rc = P_DEMUX_AllocateChannel(DI_DEMUX_CHANNELTYPE_PRIVATE, usPid, &logicalPidChannel);
	if(rc != DRV_OK || logicalPidChannel == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError("Failed: P_DEMUX_AllocateChannel() return error %d \n", rc);
		return DI_ERR_ERROR;
	}

	rc = P_DEMUX_EnableChannel(DI_DEMUX_CHANNELTYPE_PRIVATE, ulDemuxId, logicalPidChannel, P_DEMUX_CHSUBT_eTSC);
	if(rc != DRV_OK)
	{
		PrintError(" Failed: P_DEMUX_EnableChannel() return error %d \n", rc);
		P_DEMUX_DeallocateChannel(logicalPidChannel);
		return DI_ERR_ERROR;
	}

	rc = P_DEMUX_StartTSCMonitoring(logicalPidChannel, 0);
	if(rc != DRV_OK)
	{
		P_DEMUX_DisableChannel(logicalPidChannel);
		P_DEMUX_DeallocateChannel(logicalPidChannel);
		PrintError("Failed starting TSC monitor.\n");
		return DI_ERR_ERROR;
	}

	*pulHandle = logicalPidChannel;

	PrintExit();

	return DI_ERR_OK;
#endif

}

DI_ERR_CODE DI_DEMUX_StopTSCMonitor(HUINT32 ulHandle)
{
#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	if(ulHandle >= NUM_TSC_CHECK_RES)
	{
		PrintError("Handle is invalid. (%ld)\n", ulHandle);
		return DI_ERR_ERROR;
	}
	VK_SEM_Get(gTscCheck.Lock);

	if(gTscCheck.stResource[ulHandle].bEnabled == TRUE)
	{
		if(gTscCheck.stResource[ulHandle].timerId != 0)
		{
			VK_TIMER_Cancel(gTscCheck.stResource[ulHandle].timerId);
			gTscCheck.stResource[ulHandle].timerId = 0;
		}
		gTscCheck.stResource[ulHandle].bEnabled = FALSE;
		gTscCheck.stResource[ulHandle].pidChannel = 0xFF;
		gTscCheck.stResource[ulHandle].TSCValue = 0xFF;
		if(gTscCheck.stResource[ulHandle].bFilterOn != FALSE)
		{
			P_DEMUX_StopTSFilter(ulHandle);
			gTscCheck.stResource[ulHandle].bFilterOn = FALSE;
		}
	}
	VK_SEM_Release(gTscCheck.Lock);
#else
	P_DEMUX_StopTSCMonitoring(ulHandle);
	P_DEMUX_DisableChannel(ulHandle);
	P_DEMUX_DeallocateChannel(ulHandle);
#endif

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_GetTSCStatus(HUINT32 ulHandle, DI_DEMUX_TsState_t *peTsState)
{
#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	if(ulHandle >= NUM_TSC_CHECK_RES)
	{
		PrintError("Handle is invalid. (%ld)\n", ulHandle);
		return DI_ERR_ERROR;
	}
	VK_SEM_Get(gTscCheck.Lock);
	if(gTscCheck.stResource[ulHandle].bEnabled == TRUE)
	{
		if(gTscCheck.stResource[ulHandle].TSCValue == 0x00)
		{
			DI_UART_Print("[%ld] Clear\n",ulHandle);
			*peTsState = eDI_DMX_TS_Clean;
		}
		else if(gTscCheck.stResource[ulHandle].TSCValue == 0xFF)
		{
			DI_UART_Print("[%ld] Unknown\n",ulHandle);
			*peTsState = eDI_DMX_TS_UnKnown;
		}
		else
		{
			DI_UART_Print("[%ld]Scrambled\n",ulHandle);
			*peTsState = eDI_DMX_TS_Scramble;
		}
	}
	else
	{
		DI_UART_Print("[%ld] Not enabled\n",ulHandle);
		*peTsState = eDI_DMX_TS_UnKnown;
	}
	VK_SEM_Release(gTscCheck.Lock);
	return 0;
#else
	DRV_Error drvError;

	drvError = P_DEMUX_GetTSCStatus(ulHandle, peTsState);
	if(drvError != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
#endif
}

DI_ERR_CODE DI_DEMUX_BindChannel(HUINT32 tuner_id, DI_CH_Type_e delivery_type)
{
	BSTD_UNUSED(tuner_id);
	BSTD_UNUSED(delivery_type);
	return DI_ERR_OK;
}

#if defined(CONFIG_CI)
DRV_Error DRV_DEMUX_RouteToInternalRmxFeedback(HUINT32 ulDemuxId)
{
#if defined(CONFIG_ZAPPING_ONLY)
	BSTD_UNUSED(ulDemuxId);
#else
	P_DEMUX_Module *pDemux;
	NEXUS_ParserBand parser_band;
	NEXUS_ParserBand co_parser_band;
	NEXUS_ParserBandSettings pb_settings;
	NEXUS_RemuxSettings rmx_settings;
	NEXUS_Error rc;

	pDemux = P_DEMUX_GetInstance();
	parser_band = P_DEMUX_GetParserBandByDemuxId(ulDemuxId);

	if(pDemux->dmxDevice[ulDemuxId].pathInfo.nCI_Id == -1)
	{
		// Remux setting for externel route
		NEXUS_Remux_GetDefaultSettings(&rmx_settings);
		rmx_settings.parallelOutput = TRUE;
		//rmx_settings.outputClock = NEXUS_RemuxClock_eInputBand;
		//rmx_settings.allPass = TRUE;
		//rmx_settings.remuxClockIBSrc = P_DEMUX_GetInputBand(pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id);
		rmx_settings.outputClock = NEXUS_RemuxClock_e54Mhz; //NEXUS_RemuxClock_eInputBand;
		rmx_settings.allPass = TRUE;
		rmx_settings.bypass= false;
	 	rmx_settings.maxDataRate = 108000000;
		pDemux->rmxHandle = NEXUS_Remux_Open(1, &rmx_settings);

		co_parser_band = P_DEMUX_GetCoParserBandForCI(parser_band);
		NEXUS_ParserBand_GetSettings(co_parser_band, &pb_settings);
		pb_settings.allPass = TRUE;
		pb_settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
		pb_settings.sourceTypeSettings.inputBand = P_DEMUX_GetInputBand(pDemux->dmxDevice[ulDemuxId].pathInfo.ulInputDevice_id);
		pb_settings.transportType = NEXUS_TransportType_eTs;
		rc = NEXUS_ParserBand_SetSettings(co_parser_band, &pb_settings);
		if(rc != NEXUS_SUCCESS)
		{
			PrintError(" Failed to set ParserBand for RMX (%d)\n", rc);
			return DRV_ERR;
		}
		rc = NEXUS_Remux_AddPidChannel(pDemux->rmxHandle, pDemux->pidChannel[RESERVED_CHANNEL_LIVE1_LOGICAL+ulDemuxId].pidHandle);
		if(rc != NEXUS_SUCCESS)
		{
			PrintError("DI_DEMUX_SetPath(), Failed to add PID channel to Remux module.(%d)\n", rc);
			return DRV_ERR;
		}
		rc = NEXUS_Remux_Start(pDemux->rmxHandle);
		if(rc != NEXUS_SUCCESS)
		{
			PrintError("DI_DEMUX_SetPath(), Failed to start Remux module.(%d)\n", rc);
			return DRV_ERR;
		}
	}

	NEXUS_ParserBand_GetSettings(parser_band, &pb_settings);
	pb_settings.allPass = FALSE;
	pb_settings.sourceType = NEXUS_ParserBandSourceType_eRemux;
	pb_settings.sourceTypeSettings.remux = pDemux->rmxHandle;
	pb_settings.transportType = NEXUS_TransportType_eTs;
	rc = NEXUS_ParserBand_SetSettings(parser_band, &pb_settings);
#endif
	return DRV_OK;
}
#else
DRV_Error DRV_DEMUX_RouteToInternalRmxFeedback(HUINT32 ulDemuxId)
{
	BSTD_UNUSED(ulDemuxId);
	return DRV_OK;
}
#endif

DRV_Error DRV_DEMUX_StartGettingTSCStatus(HUINT32 ulMainSubId)
{
	NEXUS_Error rc;
	NEXUS_PidChannelScramblingSettings scSettings;

	NEXUS_PidChannel_GetDefaultScramblingSettings(&scSettings);

#if !defined(CONFIG_PIP)
	BSTD_UNUSED(ulMainSubId);
	rc = NEXUS_PidChannel_StartScramblingCheck(s_hVideoPIDChannel, &scSettings);
#else
	if(ulMainSubId >= MAX_DECORD_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	if(ulMainSubId == MAIN_ID)
		rc = NEXUS_PidChannel_StartScramblingCheck(s_hVideoPIDChannel, &scSettings);
	else
		rc = NEXUS_PidChannel_StartScramblingCheck(s_hPIPVideoPIDChannel, &scSettings);
#endif
	if(rc != 0)
	{
		DI_UART_Print("[%s] failed.(%d)\n",__FUNCTION__, rc);
		return DRV_ERR;
	}
	return DRV_OK;
}

DRV_Error DRV_DEMUX_StopGettingTSCStatus(HUINT32 ulMainSubId)
{

#if !defined(CONFIG_PIP)
	BSTD_UNUSED(ulMainSubId);
	NEXUS_PidChannel_StopScramblingCheck(s_hVideoPIDChannel);
#else
	if(ulMainSubId >= MAX_DECORD_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	if(ulMainSubId == MAIN_ID)
		NEXUS_PidChannel_StopScramblingCheck(s_hVideoPIDChannel);
	else
		NEXUS_PidChannel_StopScramblingCheck(s_hPIPVideoPIDChannel);
#endif

	return DRV_OK;
}

DRV_Error DRV_DEMUX_GetTscStatus(HUINT32 ulMainSubId)
{
	NEXUS_Error rc;
	NEXUS_PidChannelScramblingStatus scStatus;
#if !defined(CONFIG_PIP)
	BSTD_UNUSED(ulMainSubId);
	rc = NEXUS_PidChannel_GetScramblingStatus(s_hVideoPIDChannel, &scStatus);
#else
	if(ulMainSubId >= MAX_DECORD_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	if(ulMainSubId == MAIN_ID)
		rc = NEXUS_PidChannel_GetScramblingStatus(s_hVideoPIDChannel, &scStatus);
	else
		rc = NEXUS_PidChannel_GetScramblingStatus(s_hPIPVideoPIDChannel, &scStatus);

#endif
	if(rc != 0)
	{
		DI_UART_Print("[%s] failed.(%d)\n",__FUNCTION__, rc);
		if(scStatus.scrambled == true)
		{
#if defined(CONFIG_PIP)
			if(ulMainSubId == MAIN_ID)
			NEXUS_PidChannel_ClearScramblingStatus(s_hVideoPIDChannel);
			else
				NEXUS_PidChannel_ClearScramblingStatus(s_hPIPVideoPIDChannel);
#else
			NEXUS_PidChannel_ClearScramblingStatus(s_hVideoPIDChannel);
#endif
		}
		return DRV_ERR;
	}
	else
	{
		if(scStatus.scrambled == true)
		{
#if defined(CONFIG_PIP)
			if(ulMainSubId == MAIN_ID)
				NEXUS_PidChannel_ClearScramblingStatus(s_hVideoPIDChannel);
			else
				NEXUS_PidChannel_ClearScramblingStatus(s_hPIPVideoPIDChannel);
#else
			NEXUS_PidChannel_ClearScramblingStatus(s_hVideoPIDChannel);
#endif
		}
		DI_UART_Print("pidExist=%d scrambled=%d\n", scStatus.pidExists, scStatus.scrambled);
	}
	return DRV_OK;
}

/*Codes used for Debug only. */
#if defined(SECTION_FILTER_DEBUGGING)
static void P_DEMUX_DEBUG_SectionEventTask(void* param)
{
	P_DEMUX_Module *pDemux;
	HUINT32 message_length;
	HULONG  filter_index;
	HUINT8 *buffer, *tmp_buf;
	size_t size;
	NEXUS_Error rc;
	int i;
	P_DEMUX_MSG_Struct msg;
	unsigned long requestId;
	int vk_ret;
	HBOOL bSectionOverflow=FALSE;

	BSTD_UNUSED(param);
	pDemux = P_DEMUX_GetInstance();

	while(TRUE)
	{
		vk_ret = VK_MSG_Receive(pDemux->eventID[P_DEMUX_DEBUG_PsiFilter], &msg, sizeof(P_DEMUX_MSG_Struct));
		if(vk_ret != VK_OK)
		{
			PrintError("VK_MSG_Receive returns error. (err=%ld)\n", vk_ret);
			continue;
		}
		filter_index = msg.ulFilterIDX;
		requestId = msg.ulReqID;
		bSectionOverflow = msg.bSectionOverflow;

		PrintDebug("P_DEMUX_SectionEventTask(), filter_index = %d  requestId = 0x%lx\n", filter_index, requestId);

		//Check if filter is still alive.
		VK_SEM_Get(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
		if(pDemux->msgFilter[filter_index].status== DRV_DEMUX_RESOURCE_ENABLED &&
			requestId == pDemux->msgFilter[filter_index].requestId)
		{
			if((rc=NEXUS_Message_GetBuffer(pDemux->msgFilter[filter_index].msgHandle, (const void **)&buffer, &size)))
			{
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				PrintDebug("P_DEMUX_SectionEventTask(), NEXUS_Message_GetBuffer() return error (%ld)[filter_index=%d]\n",rc, filter_index);
				continue;
			}
			if(!size)
			{
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				/*size can be zero in normal condition. PrintError("P_DEMUX_SectionEventTask(), NEXUS_Message_GetBuffer() read size 0\n");*/
				continue;
			}
			message_length = (((buffer[1]&0xF)<<8)|(buffer[2]))+3;
			message_length = P_DEMUX_GetSizeAligned(message_length);
			if(message_length == size && size <= INTER_BUFFER_SIZE)
			{
				VK_memcpy(inter_buffer, buffer, message_length);
				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, message_length);
			}
			else
			{
				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, size);
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				PrintError("P_DEMUX_SectionEventTask() message_length(%ld) != size(%ld) or section is too big  [filter_index=%d]\n", message_length, size, filter_index);
				while(1);
			}

#if defined(CONFIG_DEBUG)
			PrintDebug("[recv] : ");
			for(i=0; i<6; i++)
				PrintDebug("%02x ", inter_buffer[i]);
			PrintDebug("\n");
#if defined(PRINT_KNOWN_SECTION_INFO)
			if(inter_buffer[0] == 0x00)
			{
				if(SectionDbgMode & 0x00000001)
				{
					DI_UART_Print("PAT arrived (0x%02x) filter=%d reqid=0x%lx\n", inter_buffer[0], filter_index, pDemux->msgFilter[filter_index].requestId);
				}
			}
			else if(inter_buffer[0] == 0x02)
			{
				if(SectionDbgMode & 0x00000002)
				{
					DI_UART_Print("[0x%08x] PMT arrived (0x%02x) filter=%d reqid=0x%lx\n",VK_TIMER_GetSystemTick(), inter_buffer[0], filter_index, pDemux->msgFilter[filter_index].requestId);
				}
			}
			else if(inter_buffer[0] == 0x42 || inter_buffer[0] == 0x46)
			{
				if(SectionDbgMode & 0x00000004)
				{
					DI_UART_Print("SDT arrived (0x%02x) filter=%d reqid=0x%lx\n", inter_buffer[0], filter_index, pDemux->msgFilter[filter_index].requestId);
				}
			}
			else if(inter_buffer[0] == 0x4e || inter_buffer[0] == 0x4f)
			{
				if(SectionDbgMode & 0x00000008)
				{
					DI_UART_Print("EIT arrived (0x%02x) filter=%d reqid=0x%lx\n", inter_buffer[0], filter_index, pDemux->msgFilter[filter_index].requestId);
				}
			}
#endif
#endif
			if( pDemux->callback.pfnAllocBuffer != NULL  && pDemux->callback.pfnSectionNotify!= NULL && bSectionOverflow == FALSE)
			{
				if(pDemux->callback.pfnAllocBuffer(message_length, &tmp_buf) != DI_ERR_OK)
				{
					VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
					PrintError("pfnAllocBuffer() return error\n");
					continue;
				}
				VK_memcpy(tmp_buf, inter_buffer, message_length);
				if(pDemux->callback.pfnSectionNotify(pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId \
												, tmp_buf, pDemux->msgFilter[filter_index].requestId) != DI_ERR_OK)
				{
					PrintError("pfnSectionNotify() return error\n");
				}
			}
			VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);

		}
		else
		{
			VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
			PrintDebug("%s, Message Filter (filterIdx: %ld) is already stopped.\n",__FUNCTION__, filter_index);\
			continue;
		}
	}
}

DI_ERR_CODE DI_DEMUX_DEBUG_SetSectionFilter (HUINT32 ulDemuxId,HUINT16 usPid,HUINT32 ulRequestID, HBOOL bIsOnce,
											HUINT32 ulFilterLen, HUINT8 * pucValue,HUINT8 * pucMask,HUINT8 * pucExclusion,
											HBOOL bCheckCRC)
{
	HUINT32 channel_index;
	HUINT8 filter_index;
	DRV_Error rc;


	PrintEnter("ulDemuxId %d, usPid=0x%x, ulRequestID 0x%lx, bIsOnce %d\n", ulDemuxId, usPid, ulRequestID, bIsOnce);

	if(ulDemuxId >= MAX_DEMUX_NUM || usPid >= DI_DEMUX_INVALID_PID || pucValue == NULL || pucMask == NULL || pucExclusion == NULL)
	{
		PrintError(" failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}

	if(ulFilterLen > SECTION_FILTER_LENGTH)
	{
		PrintError(" failed: not supported filter length %d \n", ulFilterLen);
		return DI_ERR_INVALID_PARAM;
	}

	if(P_DEMUX_HasCapability(ulDemuxId, DI_DEMUX_CAP_SECTIONFILTER) == FALSE)
	{
		PrintError(" demux %d has no capability SECTION_FILTER\n");
		return DI_ERR_ERROR;
	}

	/* allocate & enable pid */
	rc = P_DEMUX_AllocateChannel(DI_DEMUX_CHANNELTYPE_SECTION, usPid, &channel_index);
	if(rc != DRV_OK || channel_index == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError(" failed: P_DEMUX_AllocateChannel() return error %d \n", rc);
		return DI_ERR_ERROR;
	}

	rc = P_DEMUX_EnableChannel(DI_DEMUX_CHANNELTYPE_SECTION, ulDemuxId, channel_index, P_DEMUX_CHSUBT_eNone);
	if(rc != DRV_OK)
	{
		PrintError(" failed: P_DEMUX_EnableChannel() return error %d \n", rc);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_AllocateFilter(P_DEMUX_DEBUG_PsiFilter, 0, &filter_index, ulRequestID) != DRV_OK)
	{
		PrintError(" DRV_DEMUX_AllocateFilter() return error\n");
		P_DEMUX_DisableChannel(channel_index);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_EnableFilter(P_DEMUX_DEBUG_PsiFilter, filter_index, channel_index, pucValue, pucMask, pucExclusion, ulFilterLen, bIsOnce, bCheckCRC, ulRequestID) != DRV_OK)
	{
		PrintError(" DRV_DEMUX_EnableFilter() return error\n");
		P_DEMUX_DeallocateFilter(P_DEMUX_DEBUG_PsiFilter, filter_index);
		P_DEMUX_DisableChannel(channel_index);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}

#if defined(SAVE_FILTER_INFO)
	filterDbgInfo[filter_index].usPid = usPid;
	filterDbgInfo[filter_index].ulFilterLen = ulFilterLen;
	VK_MEM_Memcpy(filterDbgInfo[filter_index].ucValue, pucValue, ulFilterLen);
	VK_MEM_Memcpy(filterDbgInfo[filter_index].ucMask, pucMask, ulFilterLen);
	VK_MEM_Memcpy(filterDbgInfo[filter_index].ucExclusion, pucExclusion, ulFilterLen);
#endif
#if defined(PRINT_KNOWN_SECTION_INFO)
	if(SectionDbgMode & 0x80000000 && pucValue[0]==0x02)
	{
		DI_UART_Print("[0x%08x] [SET FILTER]  PID=0x%04x FiltLen=%02d Value 0x%02x:0x%02x:0x%02x:0x%02x    Mask 0x%02x:0x%02x:0x%02x:0x%02x (filterindex = %d, reqid=0x%lx)\n"
	                 ,VK_TIMER_GetSystemTick() ,usPid, ulFilterLen, pucValue[0],pucValue[1], pucValue[2],pucValue[3],pucMask[0],pucMask[1],pucMask[2],pucMask[3],filter_index,ulRequestID);
	}
#endif
	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_DEBUG_ReleaseSectionFilter (HUINT32 ulDemuxId, HUINT32	ulRequestID)
{
	P_DEMUX_Module *pDemux;
	HUINT8 filter_index, channel_index;

	BSTD_UNUSED(ulDemuxId);
	pDemux = P_DEMUX_GetInstance();
	PrintEnter("ulDemuxId %d, ulRequestID 0x%lx\n", ulDemuxId, ulRequestID);

	for(filter_index = 0; filter_index < MAX_SECTION_FILTER_NUM; filter_index++)
	{
		if(pDemux->msgFilter[filter_index].requestId == ulRequestID && pDemux->msgFilter[filter_index].filterType == P_DEMUX_DEBUG_PsiFilter
			&& pDemux->msgFilter[filter_index].status == DRV_DEMUX_RESOURCE_ENABLED
			&& pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId == ulDemuxId)
		{
			break;
		}
	}

	if(filter_index == MAX_SECTION_FILTER_NUM){
		PrintError(" no filter to release with RequestID(0x%lx)\n", ulRequestID);
		return DI_ERR_ERROR;
	}
#if defined(PRINT_KNOWN_SECTION_INFO)
	if(SectionDbgMode & 0x80000000 && filterDbgInfo[filter_index].ucValue[0]==0x02)
	{
		DI_UART_Print("[0x%08x] [REL FILTER] filterindex = %d reqid=0x%lx\n",VK_TIMER_GetSystemTick(),filter_index, ulRequestID);
	}
#endif
	channel_index = pDemux->msgFilter[filter_index].channelIndex;

	if(P_DEMUX_DisableFilter(P_DEMUX_DEBUG_PsiFilter, filter_index) != DRV_OK)
	{
		PrintError(" P_DEMUX_DisableFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_DeallocateFilter(P_DEMUX_DEBUG_PsiFilter, filter_index) != DRV_OK)
	{
		PrintError(" P_DEMUX_DeallocateFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	/* disable & deallocate channel */
	if(P_DEMUX_DisableChannel(channel_index) != DRV_OK)
	{
		PrintError(" P_DEMUX_DisableChannel() returns error.\n");
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_DeallocateChannel(channel_index) != DRV_OK)
	{
		PrintError(" P_DEMUX_DeallocateChannel() returns error.\n");
		return DI_ERR_ERROR;
	}
	/**************************/

	PrintExit();

	return DI_ERR_OK;
}

static DRV_Error P_DEMUX_DEBUG_PrepareSectionHandling(void)
{
	P_DEMUX_Module *pDemux;
	HUINT32 err;

	pDemux = P_DEMUX_GetInstance();

	if(VK_MSG_Create(DMX_MSGQ_COUNT, sizeof(P_DEMUX_MSG_Struct),"PsiMsgDbg",&pDemux->eventID[P_DEMUX_DEBUG_PsiFilter],VK_SUSPENDTYPE_FIFO))
	{
		PrintError("DRV_DEMUX_Init(), VK_MSG_Create error\n");
		return DRV_ERR;
	}
	err = VK_TASK_Create(P_DEMUX_DEBUG_SectionEventTask,
						DRV_DEMUX_SEF_HANDLER_TASK_PRIORITY,
						DRV_DEMUX_SEF_HANDLER_TASK_STACK_SIZE,
						"SecTaskDbg",
						NULL,
						&dbgsectiontaskId, FALSE);

	if( err != VK_OK )
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_DEBUG_PsiFilter]);
		PrintError("DRV_DEMUX_Init() Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Start(dbgsectiontaskId);
	if(err != VK_OK)
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_DEBUG_PsiFilter]);
		VK_TASK_Destroy(dbgsectiontaskId);
		PrintError("DRV_DEMUX_Init() Error(0x%x) in VK_TASK_Start()\n\r", err);
		return DRV_ERR;
	}
	return DRV_OK;
}

static DRV_Error P_DEMUX_DEBUG_DestroySectionHandling(void)
{
	P_DEMUX_Module *pDemux;
	HUINT32 err;

	pDemux = P_DEMUX_GetInstance();

	err = VK_TASK_Start(dbgsectiontaskId);

	err = VK_TASK_Destroy(dbgsectiontaskId);

	err = VK_MSG_Destroy(pDemux->eventID[P_DEMUX_DEBUG_PsiFilter]);

	return DRV_OK;
}

#endif

DRV_Error DRV_DEMUX_DebugSectionFilter(void)
{
#if defined(SECTION_FILTER_DEBUGGING)
	HUINT8 ucValue[4];
	HUINT8 ucMask[4];
	HUINT8 ucExcl[4];
	DI_ERR_CODE eRet;

	ucValue[0] = 0x02;
	ucValue[1] = 0x6f;
	ucValue[2] = 0x76;
	ucValue[3] = 0x00;
	ucMask[0]= 0xFF;
	ucMask[1]= 0xFF;
	ucMask[2]= 0xFF;
	ucMask[3]= 0x00;
	ucExcl[0] = 0x00;
	ucExcl[1] = 0x00;
	ucExcl[2] = 0x00;
	ucExcl[3] = 0x00;
 	eRet = DI_DEMUX_SetPath(1,DI_DEMUX_INPUT_TUNER,0,DI_DEMUX_OUTPUT_NONE,0,-1);
 	if(eRet != DI_ERR_OK)
 	{
		PrintError("DI_DEMUX_SetPath() error\n");
 	}
	eRet = DI_DEMUX_DEBUG_SetSectionFilter(1,0x22,500, 0,4,ucValue, ucMask, ucExcl, 0);
 	if(eRet != DI_ERR_OK)
 	{
		PrintError("DI_DEMUX_DEBUG_SetSectionFilter() error\n");
 	}
#endif
	return DRV_OK;
}

NEXUS_ParserBand DRV_DEMUX_GetParserBandByDemuxId(HUINT32 demuxId)
{
	 return P_DEMUX_GetParserBandByDemuxId(demuxId);
}


#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)

P_DEMUX_TSC_MSG aTscMsg[10];
HULONG	tscLock;

static DRV_Error P_DEMUX_PrepareTSHandling(void)
{
	P_DEMUX_Module *pDemux;
	HUINT32 err;
	HULONG taskId;
	HULONG taskIdm;

	pDemux = P_DEMUX_GetInstance();

	if(VK_MSG_Create(20, sizeof(P_DEMUX_MSG_Struct),"TsMsg",&pDemux->eventID[P_DEMUX_TsFilter],VK_SUSPENDTYPE_FIFO))
	{
		PrintError("DRV_DEMUX_Init(), VK_MSG_Create error\n");
		return DRV_ERR;
	}

	if(VK_MSG_Create(20, sizeof(P_DEMUX_TSC_MSG),"TsTsc",&pDemux->tscID, VK_SUSPENDTYPE_FIFO))
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_TsFilter]);
		PrintError("DRV_DEMUX_Init(), VK_MSG_Create error\n");
		return DRV_ERR;
	}

	err = VK_SEM_Create(&tscLock, "TSC_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_TsFilter]);
		VK_MSG_Destroy(pDemux->tscID);
		PrintError("VK_SEM_Create() error, TSC_SEM\n");
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_DEMUX_TSCMonTask,
						DRV_DEMUX_PES_HANDLER_TASK_PRIORITY,
						DRV_DEMUX_PES_HANDLER_TASK_STACK_SIZE,
						"TscMonTask",
						NULL,
						&tsmontaskId, FALSE);
	if( err != VK_OK )
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_TsFilter]);
		VK_MSG_Destroy(pDemux->tscID);
		VK_SEM_Destroy(tscLock);
		PrintError("P_DEMUX_PrepareTSHandling() Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Start(tsmontaskId);
	if(err != VK_OK)
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_TsFilter]);
		VK_MSG_Destroy(pDemux->tscID);
		VK_SEM_Destroy(tscLock);
		VK_TASK_Destroy(tsmontaskId);
		PrintError("P_DEMUX_PrepareTSHandling() Error(0x%x) in VK_TASK_Start()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_DEMUX_TsEventTask,
						DRV_DEMUX_PES_HANDLER_TASK_PRIORITY,
						DRV_DEMUX_PES_HANDLER_TASK_STACK_SIZE,
						"TsFiltTask",
						NULL,
						&tseventtaskId, FALSE);
	if( err != VK_OK )
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_TsFilter]);
		VK_MSG_Destroy(pDemux->tscID);
		VK_SEM_Destroy(tscLock);
		VK_TASK_Destroy(tsmontaskId);
		PrintError("P_DEMUX_PrepareTSHandling() Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Start(tseventtaskId);
	if(err != VK_OK)
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_TsFilter]);
		VK_MSG_Destroy(pDemux->tscID);
		VK_SEM_Destroy(tscLock);
		VK_TASK_Destroy(tsmontaskId);
		VK_TASK_Destroy(tseventtaskId);
		PrintError("P_DEMUX_PrepareTSHandling() Error(0x%x) in VK_TASK_Start()\n\r", err);
		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_DEMUX_DestroyTSHandling(void)
{
	P_DEMUX_Module *pDemux;
	HUINT32 err;

	pDemux = P_DEMUX_GetInstance();

	err = VK_TASK_Stop(tsmontaskId);
	if( err != VK_OK )
	{
		PrintError("P_DEMUX_DestroyTSHandling() Error(0x%x) in VK_TASK_Stop()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Stop(tseventtaskId);
	if( err != VK_OK )
	{
		PrintError("P_DEMUX_DestroyTSHandling() Error(0x%x) in VK_TASK_Stop()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Destroy(tsmontaskId);
	if( err != VK_OK )
	{
		PrintError("P_DEMUX_DestroyTSHandling() Error(0x%x) in VK_TASK_Destroy()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Destroy(tseventtaskId);
	if( err != VK_OK )
	{
		PrintError("P_DEMUX_DestroyTSHandling() Error(0x%x) in VK_TASK_Destroy()\n\r", err);
		return DRV_ERR;
	}

	err = VK_MSG_Destroy(pDemux->eventID[P_DEMUX_TsFilter]);
	if( err != VK_OK )
	{
		PrintError("P_DEMUX_DestroyTSHandling() Error(0x%x) in VK_MSG_Destroy()\n\r", err);
		return DRV_ERR;
	}

	err = VK_MSG_Destroy(pDemux->tscID);
	if( err != VK_OK )
	{
		PrintError("P_DEMUX_DestroyTSHandling() Error(0x%x) in VK_MSG_Destroy()\n\r", err);
		return DRV_ERR;
	}

	return DRV_OK;
}

HUINT32 gChIndex;
HUINT32 gFiltIndex;
HUINT32 tsRequestID= 0x0;

#if 1
DI_ERR_CODE P_DEMUX_StartTSFilter(int RscIdx)
{
	P_DEMUX_Module *pDemux;
	HUINT32 channel_index;
	HUINT8 filter_index;
	DI_ERR_CODE rc;
	HUINT8 pucValue[8];
	HUINT8 pucMask[8];
	HUINT8 pucExclusion[8];
	HUINT32 ulFilterLen = 8;
	HUINT32 ulDemuxId;
	HUINT16 usPid;
	HBOOL bIsOnce = TRUE;
	HBOOL bCheckCRC = FALSE;
	HUINT32 requestID;

	pDemux = P_DEMUX_GetInstance();
	channel_index = gTscCheck.stResource[RscIdx].pidChannel;

#if 0
	VK_SEM_Get(pDemux->pidChannel[channel_index].lock);
	if(pDemux->pidChannel[channel_index].status == DRV_DEMUX_RESOURCE_ENABLED)
	{
		ulDemuxId = 	pDemux->pidChannel[channel_index].demuxId;
		usPid = pDemux->pidChannel[channel_index].pid;
		bInfoFound = TRUE;
	}
	VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
	if(bInfoFound == FALSE)
	{
		PrintError(" Failed: Can't find demuxId & pid\n");
		return DI_ERR_ERROR;
	}
#else
	ulDemuxId = 	gTscCheck.stResource[RscIdx].ulDemuxId;
	usPid = gTscCheck.stResource[RscIdx].usPid;
#endif

	requestID = (gTscCheck.stResource[RscIdx].footPrint | gTSCREQID++);

	if(ulDemuxId >= MAX_DEMUX_NUM || usPid >= DI_DEMUX_INVALID_PID)
	{
		PrintError(" failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}

	if(P_DEMUX_HasCapability(ulDemuxId, DI_DEMUX_CAP_PESFILTER) == FALSE)
	{
		PrintError(" demux %d has no capability SECTION_FILTER\n");
		return DI_ERR_ERROR;
	}

	/* allocate & enable pid */
	rc = P_DEMUX_AllocateChannel(DI_DEMUX_CHANNELTYPE_SECTION, usPid, &channel_index);
	if(rc != DRV_OK || channel_index == DI_DEMUX_INVALID_CHANNEL_ID)
	{
		PrintError(" Failed: P_DEMUX_AllocateChannel() return error %d \n", rc);
		return DI_ERR_ERROR;
	}
	rc = P_DEMUX_EnableChannel(DI_DEMUX_CHANNELTYPE_SECTION, ulDemuxId, channel_index, P_DEMUX_CHSUBT_eNone);
	if(rc != DRV_OK)
	{
		PrintError(" Failed: P_DEMUX_EnableChannel() return error %d \n", rc);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}
	if(P_DEMUX_AllocateFilter(P_DEMUX_TsFilter, DI_DEMUX_PESTYPE_USERDEFINE, &filter_index, requestID) != DRV_OK)
	{
		PrintError(" DRV_DEMUX_AllocateFilter() return error\n");
		P_DEMUX_DisableChannel(channel_index);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}
	//gTscCheck.stResource[RscIdx].pidHandle = pDemux->pidChannel[channel_index].pidHandle;
	gTscCheck.stResource[RscIdx].myPidChannel = channel_index;
	gTscCheck.stResource[RscIdx].filterIndex = filter_index;
	if(P_DEMUX_EnableFilter(P_DEMUX_TsFilter, filter_index, channel_index, pucValue, pucMask, pucExclusion, ulFilterLen, bIsOnce, bCheckCRC, requestID) != DRV_OK)
	{
		PrintError(" DRV_DEMUX_EnableFilter() return error\n");
		P_DEMUX_DeallocateFilter(P_DEMUX_TsFilter, filter_index);
		P_DEMUX_DisableChannel(channel_index);
		P_DEMUX_DeallocateChannel(channel_index);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE P_DEMUX_StopTSFilter(int RscIdx)
{
	HUINT8 filter_index, channel_index;

	filter_index = gTscCheck.stResource[RscIdx].filterIndex;
	channel_index = gTscCheck.stResource[RscIdx].myPidChannel;

	if(P_DEMUX_DisableFilter(P_DEMUX_TsFilter, filter_index) != DRV_OK){
		PrintError(" P_DEMUX_DisableFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_DeallocateFilter(P_DEMUX_TsFilter, filter_index) != DRV_OK){
		PrintError(" P_DEMUX_DeallocateFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	/* disable & deallocate channel */
	if(P_DEMUX_DisableChannel(channel_index) != DRV_OK){
		PrintError(" P_DEMUX_DisableChannel() returns error.\n");
		return DI_ERR_ERROR;
	}

	if(P_DEMUX_DeallocateChannel(channel_index) != DRV_OK){
		PrintError(" P_DEMUX_DeallocateChannel() returns error.\n");
		return DI_ERR_ERROR;
	}
	/**************************/

	return DI_ERR_OK;
}
#else
DI_ERR_CODE P_DEMUX_StartTSFilter (int RscIdx)
{
	/* HUINT32 channel_index;*/
	HUINT32 channel_index;
	HUINT8 filter_index;
	HUINT32 requestID;
	DI_ERR_CODE rc;
	HUINT8 pucValue[8];
	HUINT8 pucMask[8];
	HUINT8 pucExclusion[8];
	HUINT32 ulFilterLen = 8;
	HBOOL bIsOnce = TRUE;
	HBOOL bCheckCRC = FALSE;

	channel_index = gTscCheck.stResource[RscIdx].pidChannel;
	requestID = (gTscCheck.stResource[RscIdx].footPrint | gTSCREQID++);

	if(P_DEMUX_AllocateFilter(P_DEMUX_TsFilter, DI_DEMUX_PESTYPE_USERDEFINE, &filter_index, requestID) != DRV_OK)
	{
		PrintError(" DRV_DEMUX_AllocateFilter() return error\n");
		return DI_ERR_ERROR;
	}
	gTscCheck.stResource[RscIdx].filterIndex = filter_index;
	gPIDHAHAHA = 0;
	if(P_DEMUX_EnableFilter(P_DEMUX_TsFilter, filter_index, channel_index, pucValue, pucMask, pucExclusion, ulFilterLen, bIsOnce, bCheckCRC, requestID) != DRV_OK)
	{
		gPIDHAHAHA = 0x2000;
		PrintError(" DRV_DEMUX_EnableFilter() return error\n");
		return DI_ERR_ERROR;
	}
	gPIDHAHAHA = 0x2000;
	TSC_PRINT("CH_IDX = %ld FILT_IDX=%ld reqID = 0x%lx\n", channel_index,filter_index, requestID);

	PrintExit();

	return DI_ERR_OK;
}


DI_ERR_CODE P_DEMUX_StopTSFilter (int RscIdx)
{
	P_DEMUX_Module *pDemux;
	HUINT8 filter_index;

	filter_index = gTscCheck.stResource[RscIdx].filterIndex;

	TSC_PRINT("StopTSFilter: filt_index = %ld\n", filter_index);
	gPIDHAHAHA = 0;
	if(P_DEMUX_DisableFilter(P_DEMUX_TsFilter, filter_index) != DRV_OK){
		gPIDHAHAHA = 0x2000;
		PrintError(" P_DEMUX_DisableFilter() returns error.\n");
		return DI_ERR_ERROR;
	}
	gPIDHAHAHA = 0x2000;
	TSC_PRINT("HAHA02\n");
	if(P_DEMUX_DeallocateFilter(P_DEMUX_TsFilter, filter_index) != DRV_OK){
		PrintError(" P_DEMUX_DeallocateFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#endif

static void P_DEMUX_TSCTimerCallback(unsigned long size, void *p)
{
	P_DEMUX_Module *pDemux;
	HUINT32 ulSlotId;
	HUINT32 footPrint;
	P_DEMUX_TSC_MSG tscMsg;

	BSTD_UNUSED(size);
	footPrint = *(unsigned long *)p;
	ulSlotId = ((footPrint & 0xFF000000)>>24);
	//VK_SEM_Get(gTscCheck.Lock);
	//gTscCheck.stResource[ulSlotId].timerId = 0;
	//VK_SEM_Release(gTscCheck.Lock);
	pDemux = P_DEMUX_GetInstance();
	tscMsg.ulMsg = TSC_MSG_RESTART_FILTER;
	tscMsg.footPrint= footPrint;
	VK_MSG_SendTimeout(pDemux->tscID, &tscMsg, sizeof(P_DEMUX_TSC_MSG), 0);
}

static void P_DEMUX_TSCMonTask(void *param)
{
	P_DEMUX_Module *pDemux;
	P_DEMUX_TSC_MSG tscMsg;
	HUINT32 ulSlotId, footPrint;
	int vk_ret;

	BSTD_UNUSED(param);
	pDemux = P_DEMUX_GetInstance();
	while(TRUE)
	{
		vk_ret = VK_MSG_Receive(pDemux->tscID, &tscMsg, sizeof(P_DEMUX_TSC_MSG));
		if(vk_ret != VK_OK)
		{
			PrintError("VK_MSG_Receive returns error. (err=%ld)\n", vk_ret);
			continue;
		}

		switch(tscMsg.ulMsg)
		{
			case TSC_MSG_TS_ARRIVED:
				ulSlotId = ((tscMsg.footPrint & 0xFF000000)>>24);
				VK_SEM_Get(gTscCheck.Lock);
				gTscCheck.stResource[ulSlotId].TSCValue = tscMsg.ucTSCValue;
				VK_SEM_Release(gTscCheck.Lock);
				//VK_TASK_Sleep(500);
				VK_SEM_Get(gTscCheck.Lock);
				//DI_UART_Print("[TSC] SlotID=%ld Value=%lx\n", ulSlotId, tscMsg.ucTSCValue);
				footPrint = (tscMsg.footPrint & 0xFFFF0000);
				VK_SEM_Release(gTscCheck.Lock);
				VK_TIMER_EventAfter(500, P_DEMUX_TSCTimerCallback, &footPrint, 4, &gTscCheck.stResource[ulSlotId].timerId);
				break;

			case TSC_MSG_RESTART_FILTER:
				ulSlotId = ((tscMsg.footPrint & 0xFF000000)>>24);
				VK_SEM_Get(gTscCheck.Lock);
				if(gTscCheck.stResource[ulSlotId].bEnabled == TRUE && ((gTscCheck.stResource[ulSlotId].footPrint&0xFFFF0000) == (tscMsg.footPrint&0xFFFF0000)))
				{
					if(gTscCheck.stResource[ulSlotId].bFilterOn == FALSE)
					{
						P_DEMUX_StartTSFilter(ulSlotId); //(0,0x6e);
						gTscCheck.stResource[ulSlotId].bFilterOn = TRUE;
					}
				}
				VK_SEM_Release(gTscCheck.Lock);
				break;
		}

	}
}

static void P_DEMUX_TsEventTask(void* param)
{
	P_DEMUX_Module *pDemux;
	HUINT32 message_length;
	HULONG  filter_index;
	HUINT8 *buffer;
	size_t size;
	NEXUS_Error rc;
	int i;
	int vk_ret;
	P_DEMUX_MSG_Struct msg;
	P_DEMUX_TSC_MSG tscMsg;
	unsigned long requestId;
	HUINT8 TSC_INFO;

	BSTD_UNUSED(param);

	pDemux = P_DEMUX_GetInstance();
	while(TRUE)
	{

		vk_ret = VK_MSG_Receive(pDemux->eventID[P_DEMUX_TsFilter], &msg, sizeof(P_DEMUX_MSG_Struct));
		if(vk_ret != VK_OK)
		{
			PrintError("VK_MSG_Receive returns error. (err=%ld)\n", vk_ret);
			continue;
		}
		filter_index = msg.ulFilterIDX;
		requestId = msg.ulReqID;

#if defined(TS_STATE_TEST_PRINT_FORCE)
		DI_UART_Print("P_DEMUX_TsEventTask(), filter_index = %d\n", filter_index);
#else
		PrintDebug("P_DEMUX_TsEventTask(), filter_index = %d\n", filter_index);
#endif

		message_length = 0;
		VK_SEM_Get(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
		if(pDemux->msgFilter[filter_index].status== DRV_DEMUX_RESOURCE_ENABLED &&
			requestId == pDemux->msgFilter[filter_index].requestId)
		{
			if((rc=NEXUS_Message_GetBuffer(pDemux->msgFilter[filter_index].msgHandle, (const void **)&buffer, &size)))
			{
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				PrintError("P_DEMUX_TsEventTask(), NEXUS_Message_GetBuffer() return error (%ld)\n",rc);
				continue;
			}

			if(!size)
			{
				/*size can be zero in normal condition. PrintError("P_DEMUX_SectionEventTask(), NEXUS_Message_GetBuffer() read size 0\n");*/
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				continue;
			}

			message_length = 188; /*hardcoded*/
			message_length = P_DEMUX_GetSizeAligned(message_length);

			if(message_length > size)
			{
				PrintError("P_DEMUX_TsEventTask(), message length and read size are different %d-%d\n", message_length, size);
				//continue;
			}

#if defined(TS_STATE_TEST_PRINT_FORCE)
			DI_UART_Print("[ts_recv] : ");
			for(i=0; i<6; i++)
				DI_UART_Print("%02x ", buffer[i]);
			DI_UART_Print("\n");
#else
			PrintDebug("[ts_recv] : ");
			for(i=0; i<6; i++)
				PrintDebug("%02x ", buffer[i]);
			PrintDebug("\n");
#endif
			TSC_INFO = buffer[3];
			NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, size);
			VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
			VK_SEM_Get(gTscCheck.Lock);
			if(gTscCheck.stResource[(requestId & 0xFF000000)>>24].bEnabled == TRUE && gTscCheck.stResource[(requestId & 0xFF000000)>>24].bFilterOn == TRUE)
			{
				P_DEMUX_StopTSFilter ((requestId & 0xFF000000)>>24);
				gTscCheck.stResource[(requestId & 0xFF000000)>>24].bFilterOn = FALSE;
			}
			VK_SEM_Release(gTscCheck.Lock);
			tscMsg.ulMsg = TSC_MSG_TS_ARRIVED;
			tscMsg.footPrint= requestId;
			tscMsg.ucTSCValue = ((TSC_INFO&0xC0)>>6);
			VK_MSG_SendTimeout(pDemux->tscID, &tscMsg, sizeof(P_DEMUX_TSC_MSG), 0);
		}
		else
		{
			VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
		}
	}
}
#endif

#if 0
uint32_t DI_DEMUX_DBG_ReadRegister(uint32_t regAddr)
{
	return NEXUS_DEMUX_DBG_ReadRegister(regAddr);
}

void DI_DEMUX_DBG_WriteRegister(uint32_t regAddr, uint32_t regVal)
{
	NEXUS_DEMUX_DBG_WriteRegister(regAddr, regVal);
}

uint8_t DI_CI_ReadRegister(uint32_t regAddr)
{
	return NEXUS_CI_ReadRegister(regAddr);
}

void DI_CI_WriteRegister(uint32_t regAddr, uint8_t regVal)
{
	NEXUS_CI_WriteRegister(regAddr,regVal);
}
#endif

#if defined(CONFIG_DI20)
static DRV_Error P_DEMUX_InsertPacketModeOn(HUINT32 ulDemuxId, HUINT16 usVirtualPid, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize,
									HUINT32 *pulHandle, HUINT32 ulIntervalMS)
{
	P_DEMUX_Module *pDemux = NULL;
	HUINT8 *pucPacketBuffer = NULL;
	HUINT32 ulPktmodCnt = 0;
	HUINT32 ulTsPacketNum = 0;
	HUINT32 ulIndex = 0;
	HUINT8 *pucTempPacketBuffer = NULL;
	HUINT8 *pucTempReplaced = NULL;

	PrintEnter("ulDemuxId=%d, usPid=0x%lx, ulSectionSize=%d, ulPacketInsertInterval=%d\n",
		ulDemuxId, usPid, ulSectionSize, ulIntervalMS);

	pDemux = P_DEMUX_GetInstance();

	if(ulDemuxId >= MAX_DEMUX_NUM || usVirtualPid >= DI_DEMUX_INVALID_PID || usPid >= DI_DEMUX_INVALID_PID  || pucSection == NULL || pulHandle == NULL)
	{
		PrintError("DI_DEMUX_SetPacketReplaced() failed: invalid parameter \n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_SEM_Get(pktsubLock);

	for(ulPktmodCnt = 0; ulPktmodCnt < MAX_PKTSUB_MODULE_NUM; ulPktmodCnt++)
	{
		if(pDemux->stPktSubstitution[ulPktmodCnt].Status == DRV_DEMUX_RESOURCE_ALLOCATED &&
		  pDemux->stPktSubstitution[ulPktmodCnt].ulDemuxId == ulDemuxId &&
		  pDemux->stPktSubstitution[ulPktmodCnt].usPid == usVirtualPid)
		{
			/* packets registered already. */
			break;
		}
	}

	if(ulPktmodCnt < MAX_PKTSUB_MODULE_NUM)
	{
		/* packets registered already. just return error. */
		VK_SEM_Release(pktsubLock);
		return DRV_ERR;
	}

	for(ulPktmodCnt = 0; ulPktmodCnt < MAX_PKTSUB_MODULE_NUM; ulPktmodCnt++)
	{
		if(pDemux->stPktSubstitution[ulPktmodCnt].Status == DRV_DEMUX_RESOURCE_FREE)
		{
			break;
		}
	}

	if(ulPktmodCnt >= MAX_PKTSUB_MODULE_NUM)
	{
		/* all slots in use. */
		VK_SEM_Release(pktsubLock);
		return DRV_ERR;
	}

	pDemux->stPktSubstitution[ulPktmodCnt].Status = DRV_DEMUX_RESOURCE_ALLOCATED;

	VK_SEM_Release(pktsubLock);

	ulTsPacketNum = 1;

	if (ulSectionSize>183)
	{
		ulTsPacketNum += (ulSectionSize - 183) / P_DEMUX_PAYLOAD_SIZE;
		if ((ulSectionSize-183) % P_DEMUX_PAYLOAD_SIZE)
		{
			ulTsPacketNum++;
		}
	}

	if (ulTsPacketNum == 1)
	{
		pucPacketBuffer = DD_MEM_Alloc(2 * P_DEMUX_TS_PKT_SIZE);
	}
	else
	{
		pucPacketBuffer = DD_MEM_Alloc(ulTsPacketNum * P_DEMUX_TS_PKT_SIZE);
	}

	if(pucPacketBuffer == NULL)
	{
		VK_SEM_Get(pktsubLock);
		pDemux->stPktSubstitution[ulPktmodCnt].Status = DRV_DEMUX_RESOURCE_FREE;
		VK_SEM_Release(pktsubLock);
		PrintError("DI_DEMUX_SetPacketReplaced() failed: no memory to save packet to be replaced.\n");
		return DRV_ERR;
	}

	VK_memset(pucPacketBuffer, 0xFF, ulTsPacketNum * P_DEMUX_TS_PKT_SIZE);

	pucTempReplaced = pucSection;

	for (ulIndex = 0; ulIndex < ulTsPacketNum; ulIndex++)
	{
		pucTempPacketBuffer = pucPacketBuffer+ulIndex * P_DEMUX_TS_PKT_SIZE;

		if (ulIndex == 0) /* first TS packet */
		{
			pucTempPacketBuffer[0] = 0x47;
			pucTempPacketBuffer[1] = 0x40 | ((usPid >> 8) & 0x1f);
			pucTempPacketBuffer[2] = usPid & 0xff; /* PID */
			pucTempPacketBuffer[3] = 0x10; /* "Not Scrambled", "No Adaptation field", "Continuity Counter 0000" */
			pucTempPacketBuffer[4] = 0x00;

			if (ulTsPacketNum == 1)
			{
				VK_memcpy(&pucTempPacketBuffer[5], pucTempReplaced, ulSectionSize);
				VK_memcpy(&pucTempPacketBuffer[188], &pucTempPacketBuffer[0], 188);
				pucTempPacketBuffer[188+3] += 1; /* Increase Coninuity Counter */
			}
			else
			{
				VK_memcpy(&pucTempPacketBuffer[5], pucTempReplaced, 183);
				pucTempReplaced += 183;
			}
		}
		else
		{
			pucTempPacketBuffer[0] = 0x47;
			pucTempPacketBuffer[1] = ((usPid >> 8) & 0x1f);
			pucTempPacketBuffer[2] = usPid & 0xff; /* PID */
			pucTempPacketBuffer[3] = 0x10 | (ulIndex & 0x0F); /* "Not Scrambled", "No Adaptation field", "Continuity Counter" */

			if (ulIndex == ulTsPacketNum - 1) /* last TS packet */
			{
				VK_memcpy(&pucTempPacketBuffer[4], pucTempReplaced, ulSectionSize - (183 + (ulTsPacketNum - 2) * P_DEMUX_PAYLOAD_SIZE));
			}
			else
			{
				VK_memcpy(&pucTempPacketBuffer[4], pucTempReplaced, P_DEMUX_PAYLOAD_SIZE);
			}

			pucTempReplaced += P_DEMUX_PAYLOAD_SIZE;
		}
	}

	pDemux->stPktSubstitution[ulPktmodCnt].ulDemuxId = ulDemuxId;
	pDemux->stPktSubstitution[ulPktmodCnt].usPid = usVirtualPid;
	pDemux->stPktSubstitution[ulPktmodCnt].pBuffer = pucPacketBuffer;
	pDemux->stPktSubstitution[ulPktmodCnt].ulIntervalMS = ulIntervalMS;

	if (ulTsPacketNum == 1)
	{
		pDemux->stPktSubstitution[ulPktmodCnt].buffSize = 2 * P_DEMUX_TS_PKT_SIZE;
	}
	else
	{
		pDemux->stPktSubstitution[ulPktmodCnt].buffSize = ulTsPacketNum * P_DEMUX_TS_PKT_SIZE;
	}

	/* DI_UART_Dump(pDemux->stPktSubstitution[ulPktmodCnt].pBuffer, pDemux->stPktSubstitution[ulPktmodCnt].buffSize, P_DEMUX_TS_PKT_SIZE); */

	*pulHandle = ulPktmodCnt;

	PrintExit();

	return DRV_OK;
}

static DRV_Error P_DEMUX_InsertPacketModeOff(HUINT32 ulHandle)
{
	P_DEMUX_Module *pDemux;
	HUINT8 *pBuffbeFreed = NULL;

	if(ulHandle >= MAX_PKTSUB_MODULE_NUM)
	{
		PrintError("%s() failed: invalid parameter \n", __FUNCTION__);
		return DRV_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_GetInstance();

	VK_SEM_Get(pktsubLock);

	if(pDemux->stPktSubstitution[ulHandle].Status != DRV_DEMUX_RESOURCE_ALLOCATED)
	{
		VK_SEM_Release(pktsubLock);
		PrintError("%s() failed: slot is not in allocated status.\n",__FUNCTION__);
		return DRV_ERR;
	}

	pBuffbeFreed = pDemux->stPktSubstitution[ulHandle].pBuffer;

	pDemux->stPktSubstitution[ulHandle].Status = DRV_DEMUX_RESOURCE_FREE;

	VK_SEM_Release(pktsubLock);

	if(pBuffbeFreed != NULL)
	{
		DD_MEM_Free(pBuffbeFreed);
	}

	return DRV_OK;
}
#endif

DI_ERR_CODE DI_DEMUX_GetInfo(HUINT32 demux_id,HUINT16 *videoPid, HUINT16 *audioPid)
{

	P_DEMUX_Module *pDemux;
	HINT32 demux_amount=0;
	HINT32 target = 0;
	HINT32 i;
	HINT32 audio_pidchannel_index=0;
	HINT32 video_pidchannel_index=0;
	HINT32 pcr_pidchannel_index=0;

	pDemux = P_DEMUX_GetInstance();
	for(i=0; i<MAX_DEMUX_NUM; i++)
	{
		if(pDemux->dmxDevice[i].bIsUsing == TRUE)
		{
			demux_amount++;
		}
		if(pDemux->dmxDevice[i].ulDemuxID == demux_id)
		{
			target = i;
			if(pDemux->dmxDevice[target].bIsUsing != TRUE)
				DI_UART_Print("This demux is not being used now\n");

		}
	}
	/* Getting channel index */
	for(i=0; i<DRV_DMX_NUM_PID_CHANNEL;i++)
	{
		if(pDemux->pidChannel[i].pidHandle == s_hAudioPIDChannel &&
				pDemux->pidChannel[i].channelType <=DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF &&
				pDemux->pidChannel[i].channelType >=DI_DEMUX_CHANNELTYPE_AUDIO )
		{

			audio_pidchannel_index = i;
		}

		if(pDemux->pidChannel[i].pidHandle == s_hVideoPIDChannel &&
				pDemux->pidChannel[i].channelType== DI_DEMUX_CHANNELTYPE_VIDEO)
			video_pidchannel_index = i;
		if(pDemux->pidChannel[i].pidHandle == s_hPcrPIDChannel &&
				pDemux->pidChannel[i].channelType== DI_DEMUX_CHANNELTYPE_PCR)
			pcr_pidchannel_index = i;
	}

	/* video pid channel information */
	*videoPid =  pDemux->pidChannel[video_pidchannel_index].pid;

	*audioPid = pDemux->pidChannel[audio_pidchannel_index].pid;

	return DI_ERR_OK;

}

#if defined(CONFIG_DI20)
DI_ERR_CODE DI_DEMUX_ChangeSTCRate(HUINT32 unDeviceId, unsigned int unIncrement, unsigned int unPrescale)
{
	NEXUS_Error nexusError;
	NEXUS_StcChannelHandle hStcHandle;

	PrintEnter();
	hStcHandle = DRV_DEMUX_GetSTCChannel(unDeviceId);
	if(hStcHandle == NULL)
	{
		return DI_ERR_NO_RESOURCE;
	}

	/* stcRate_changed = (stcRate_normal)*unIncrement/(1+unPrescale)*/
	nexusError = NEXUS_StcChannel_SetRate(hStcHandle, unIncrement, unPrescale );
	if(nexusError != NEXUS_SUCCESS)
	{
		return DI_ERR_ERROR;
	}
	PrintExit();

	return DI_ERR_OK;
}
#endif

DI_ERR_CODE DI_DEMUX_Init(void)
{
	DRV_Error drvError = 0;

	drvError = DRV_DEMUX_Init();
	if(drvError != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;

}

DI_ERR_CODE DI_DEMUX_UnInit(void)
{
	DRV_Error drvError = 0;

	drvError = DRV_DEMUX_UnInit();
	if(drvError != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}



