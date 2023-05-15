/**
 * di_rec.c
*/

/**
 * @defgroup		DI_PVR DI_REC : record module
 * @author		Dong-joon Kim
 * @note			DI_REC APIs
 * @brief			Define DI_REC APIs
 * @file 			di_rec.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "drv_pvr.h"
#include "di_video.h"
#include "drv_video.h"
#include "drv_demux.h"
#include "drv_crypt.h"
#include "drv_pm.h"
#include "di_uart.h"

#if defined(CONFIG_TRANSCODER)
#include "drv_stream_mux.h"
#endif
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
#include "drv_dsc.h"
#endif
#endif
#include "nexus_pid_channel_scrambling.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if	defined(CONFIG_DELAYED_RECORD)
#include <errno.h>
#endif

#if	defined(CONFIG_TEST_COPY_TASK)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "vkernel.h"
#include "taskdef.h"

/*private header files*/
#include "p_rec.h"

#if defined(REC_DYNAMIC_DMA_ALLOC)
#include "nexus_dma.h"
#endif
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DI_PVR

#if	defined(CONFIG_DELAYED_RECORD)
#define ENABLE_RECORD_CHUNK_FILE
#define RECORD_CHUNK_SIZE	CONFIG_CHUNK_FILE_SIZE /*(1024*47*48*4*10)*/
#endif

#define	MAX_REC_PID_NUM			DRV_DMX_NUM_PID_CHANNEL
#define	INITIAL_TIMECODE_NOTIFY_INTERVAL		(1000) /*1 sec */

#define	TSR_USED_100_PERCENT	(10000)
#define	DATA_ALIGN   DRV_PVR_ATOM_SIZE

#if	defined(CONFIG_DELAYED_RECORD)
#define DELAYED_COPY_STATUS_TIMER 100 //1000
#define DELAYED_COPY_END_TIMER 100 //5000

#define DELAYED_RECORD_TASK_QUEUE_SIZE 5
#define DELAYED_COPY_BUFFER_SIZE				(4096*256) // 1MB
#endif
#define RECORD_TIMER_TASK_QUEUE_SIZE MAX_REC_INST_NUM*32

#if defined(CONFIG_TRANSCODER)
/*Do not used VK_INTR (temp)*/
#else
#define USE_VK_INTR_TO_CALLBACK
#endif

#if (ENCRYPTION_KEY_LEN != DI_PVR_CRYPTO_KEY_LEN)
#error ENCRYPTION_KEY_LEN is different from DI_PVR_CRYPTO_KEY_LEN
#endif

#if defined(CONFIG_USB_FLASH_RECORD)
#define DIRTY_RATIO_VALUE_FOR_HDD 5
#define DIRTY_BACKGROUND_RATIO_VALUE_FOR_HDD 2
#define DIRTY_WRITEBACK_CENTISECS_VALUE_FOR_HDD 1
#define DIRTY_EXPIRE_CENTISECS_VALUE_FOR_HDD 1

#define DIRTY_RATIO_VALUE_FOR_USB_FLASH 10
#define DIRTY_BACKGROUND_RATIO_VALUE_FOR_USB_FLASH 5
#define DIRTY_WRITEBACK_CENTISECS_VALUE_FOR_USB_FLASH 15
#define DIRTY_EXPIRE_CENTISECS_VALUE_FOR_USB_FLASH 15

#define DIRTY_RATIO_FILE_NAME "/proc/sys/vm/dirty_ratio"
#define DIRTY_BACKGROUND_RATIO_FILE_NAME "/proc/sys/vm/dirty_background_ratio"
#define DIRTY_WRITEBACK_CENTISECS_FILE_NAME "/proc/sys/vm/dirty_writeback_centisecs"
#define DIRTY_EXPIRE_CENTISECS_FILE_NAME "/proc/sys/vm/dirty_expire_centisecs"
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum {
#if	defined(CONFIG_DELAYED_RECORD)
	P_REC_MSG_DELAYED_COPY_START,
	P_REC_MSG_DELAYED_COPY_STATUS,
	P_REC_MSG_DELAYED_COPY_END,
	P_REC_MSG_DELAYED_COPY_ERROR,
#endif
	P_REC_MSG_TIME_NOTIFY
} P_REC_MSG_TYPE;

typedef struct stP_REC_TASK_MSG
{
	unsigned long	deviceId;
	P_REC_MSG_TYPE msgId;
} P_REC_TASK_MSG;

typedef struct stP_REC_PID_CHANNEL
{
	NEXUS_PidChannelHandle recPidChHandle;
	unsigned long refCnt;
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	/* HW test에서는 각각 다른 record device가 동일한 PID channel(-> 동일 playback parser)을
	사용하는 경우가 있어서 추가 */
	unsigned long refCntInDevice[MAX_REC_INST_NUM];
#endif
} P_REC_PID_CHANNEL;

#if	defined(CONFIG_DELAYED_RECORD)
typedef struct stP_REC_DELAYED_RECORD_INFO
{
	unsigned char used;
	unsigned long semId;
	unsigned long delayRecRequestId;
	NEXUS_ChunkedFifoRecordHandle fifoRecHandle;
	NEXUS_KeySlotHandle encKeyHandle;
	DI_PVR_Event_Callback delayRecStatusCallbackFunc;
	unsigned long delayedCopyStatusTimerId;
	unsigned long delayedCopyEndTimerId;
	unsigned long delayedCopyStatus;
} P_REC_DELAYED_RECORD_INFO;
#endif

#if defined(USE_VK_INTR_TO_CALLBACK)
typedef struct stP_REC_CALLBACK_ID
{
	unsigned long intId;
	HBOOL	valid;
}P_REC_CALLBACK_ID;
#endif

#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
typedef struct stP_REC_DSC_KEYSLOT_INFO
{
	NEXUS_KeySlotHandle		dscKeyHandle;
	NEXUS_PidChannelHandle	pesPidChHandle;
}P_REC_DSC_KEYSLOT_INFO;

#define P_REC_MAX_PID_PER_REC_INT	6
#endif

typedef struct	stP_REC_INSTANCE
{
	DI_PVR_STATUS	status;
	NEXUS_RecordHandle	recordHandle;
	NEXUS_RecpumpHandle recPumpHandle;
	NEXUS_FileRecordHandle fileRecHandle;
#if 	defined(REC_DYNAMIC_DMA_ALLOC)
	NEXUS_DmaHandle dmaHandle;
#endif
#if	defined(CONFIG_DELAYED_RECORD)
	NEXUS_ChunkedFifoRecordHandle fifoRecHandle;
#else
	NEXUS_FifoRecordHandle fifoRecHandle;
#endif
	NEXUS_KeySlotHandle		encKeyHandle;
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
	P_REC_DSC_KEYSLOT_INFO	dscKeyInfo[P_REC_MAX_PID_PER_REC_INT];
#endif
	DI_PVR_Event_Callback	recEvtCbFunc[DI_PVR_EVENT_MAX_NUMBER];
#if defined(USE_VK_INTR_TO_CALLBACK)
	P_REC_CALLBACK_ID	recEvtCbId[DI_PVR_EVENT_MAX_NUMBER];
#endif
	unsigned char		requestRealStart;
	NEXUS_PidChannelHandle	idxPidChHandle;
	DI_PVR_REC_SETUP_t 				setup;
	unsigned long					timeCodeNotifyTimerId;
	unsigned long					timeCodeNotifyInterval; 	/*(ms)*/
#if defined(CONFIG_FULL_TS_RECORDING)
	unsigned long				fullTsRecordTime;
#endif
	unsigned long				diSemId;
	unsigned char			lastScrambleStatus; /* true : scrambled, false : clear */
	unsigned char			recordSuspend;
	unsigned long			delayedTimeOffset; /*(ms)*/
#if	defined(CONFIG_DELAYED_RECORD)
	int			switchedfromTsrToLinear;
#endif
#if	defined(CONFIG_REC_MEMIO) || defined(CONFIG_TRANSCODER_MEMIO)
	HBOOL	bMemIoOn;
#endif
	HBOOL	bTsRec;
	DI_PVR_INST_TYPE	instType;
	HBOOL				bEncrpytionSetted;
	HBOOL				bFlush;	
}	P_REC_INSTANCE;

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
extern NEXUS_DmaHandle g_ulDmaHandle;

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static P_REC_INSTANCE	g_astPvrRecInst[MAX_REC_INST_NUM];
static P_REC_PID_CHANNEL	g_astRecPidChInfo[MAX_REC_PID_NUM];
static unsigned long	g_ulRecTimeTaskMsgQueueId;
static unsigned long 	g_ulRecTimeTaskId;
#if	defined(CONFIG_DELAYED_RECORD)
static P_REC_DELAYED_RECORD_INFO s_stDelayedRecInfo;
static unsigned long		g_ulDelayedRecTaskMsgQueueId;
static unsigned long		g_ulDelayedRecTaskId;
#endif

unsigned long g_ulSemgRecPid;

#if defined(CONFIG_TRANSCODER)
static NEXUS_PidChannelHandle	   g_pidChannelTranscodeVideo[MAX_MUX_NUM];
static NEXUS_PidChannelHandle	   g_pidChannelTranscodeAudio[MAX_MUX_NUM];
static NEXUS_PidChannelHandle	   g_pidChannelTranscodeSubAudio[MAX_MUX_NUM];
static NEXUS_PidChannelHandle	   g_pidChannelTranscodePcr[MAX_MUX_NUM];
static NEXUS_PidChannelHandle	   g_pidChannelTranscodePsi[MAX_MUX_NUM][MAX_NUM_PSI];
static NEXUS_PidChannelHandle	   g_pidChannelTranscodePes[MAX_MUX_NUM][MAX_NUM_PES];
#endif

#if defined(CONFIG_USB_FLASH_RECORD)
HBOOL g_bTunePdflushForUsbFlash = FALSE;
#endif

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
static DRV_Error P_REC_Start(unsigned long ulDeviceId);
#if	defined(CONFIG_DELAYED_RECORD)
static void P_REC_DelayedCopyStartTimerCallback(unsigned long tid, void *arg);
static void P_REC_DelayedCopyStatusTimerCallback(unsigned long tid, void *arg);
static void P_REC_DelayedCopyErrorCallBack(unsigned long tid, void *arg);
static void P_REC_DelayedCopyEndCallBack(unsigned long tid, void *arg);
static void P_REC_DelayedRecordTask(void *param);
#endif

#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
static DRV_Error P_REC_LoadEncryptionKey(NEXUS_KeySlotHandle ulEncKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings, HBOOL btimestampOn);
static DRV_Error P_REC_SetEncryption(NEXUS_RecordHandle ulRecordHandle, NEXUS_KeySlotHandle ulEncKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings, HBOOL btimestampOn);
#else
static DRV_Error P_REC_LoadEncryptionKey(NEXUS_KeySlotHandle ulEncKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings);
static DRV_Error P_REC_SetEncryption(NEXUS_RecordHandle ulRecordHandle, NEXUS_KeySlotHandle ulEncKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings);
#endif
//static NEXUS_VideoCodec P_REC_ConvertVideoCodec2Nexus(DI_PVR_VIDEOTYPE_t videoCodec);
static void P_REC_BufferOverflowCallback(void *context, int param);
static void P_REC_WriteErrorCallback(void *context, int param);
static void P_REC_TimeNotifyCallback(unsigned long tid, void *arg);
static void P_REC_TimeTask(void *param);
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO) && !defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
static NEXUS_KeySlotHandle P_REC_GetKeySlotHandleFromDSC(HUINT32 unChannelIndex);
#endif
#endif

#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
static DRV_Error P_REC_AddDscKeyslot(HUINT32 ulDeviceId, NEXUS_KeySlotHandle keyHandle, NEXUS_PidChannelHandle pidChHandle);
static DRV_Error P_REC_RemoveDscKeyslot(HUINT32 ulDeviceId, NEXUS_PidChannelHandle pidChHandle);
static void P_REC_ClearDscKeyslot(HUINT32 ulDeviceId); 
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
void DRV_PVR_REC_Uninit(void)
{
	int i;

	for(i=0; i<MAX_REC_INST_NUM; i++)
	{
		if(g_astPvrRecInst[i].encKeyHandle != NULL)
		{
			NEXUS_Security_FreeKeySlot(g_astPvrRecInst[i].encKeyHandle);
			g_astPvrRecInst[i].encKeyHandle = NULL;
		}
	}
#if	defined(CONFIG_DELAYED_RECORD)
	if(s_stDelayedRecInfo.encKeyHandle != NULL)
	{
		NEXUS_Security_FreeKeySlot(s_stDelayedRecInfo.encKeyHandle);
		s_stDelayedRecInfo.encKeyHandle = NULL;
	}
#endif
}

void DRV_PVR_REC_Reinit(void)
{
	int i;
	NEXUS_SecurityKeySlotSettings keySettings;

	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eCaCp;
#else
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
#endif
	for(i=0; i<MAX_REC_INST_NUM; i++)
	{
		g_astPvrRecInst[i].encKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	}
#if	defined(CONFIG_DELAYED_RECORD)
	s_stDelayedRecInfo.encKeyHandle =  NEXUS_Security_AllocateKeySlot(&keySettings);
#endif
}

#endif

DRV_Error DRV_PVR_REC_Init(void)
{
	int err;
	unsigned long i;
	NEXUS_RecpumpOpenSettings openSettings;
	NEXUS_RecpumpHandle recPumpHandle = NULL;
	NEXUS_RecordHandle	recHandle = NULL;
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
	NEXUS_SecurityKeySlotSettings	keySettings;
#endif
	NEXUS_KeySlotHandle		encKeyHandle = NULL;
#if	defined(CONFIG_DELAYED_RECORD)
	unsigned long dlyRecSemId;
#endif
#if defined(USE_VK_INTR_TO_CALLBACK)
	unsigned long j;
#endif

	unsigned long diSemId;

	PrintEnter("DRV_PVR_REC_Init");

	VK_memset(g_astPvrRecInst, 0x00, sizeof(P_REC_INSTANCE)*MAX_REC_INST_NUM);
	VK_memset(g_astRecPidChInfo, 0x00, sizeof(P_REC_PID_CHANNEL)*MAX_REC_PID_NUM);
#if	defined(CONFIG_DELAYED_RECORD)
	VK_memset(&s_stDelayedRecInfo, 0x00, sizeof(P_REC_DELAYED_RECORD_INFO));
#endif

	VK_MEM_Memset(&openSettings, 0x0, sizeof(NEXUS_RecpumpOpenSettings)); /*redundant line to avoid prevent defect.*/
	for( i=0; i<MAX_REC_INST_NUM; i++ )
	{
		NEXUS_Recpump_GetDefaultOpenSettings(&openSettings);
#if defined(CONFIG_JAPAN_SPEC)
		openSettings.data.bufferSize = DRV_PVR_REC_BUFFER_LARGE + DRV_PVR_REC_BUFFER_ADD;
#else
		openSettings.data.bufferSize = DRV_PVR_REC_BUFFER + DRV_PVR_REC_BUFFER_ADD;
#endif
		openSettings.data.dataReadyThreshold = (188/4)*4096*4; /*brcm_recommendation: set multiple of LCM(188,4k)*/ /*openSettings.data.bufferSize / 5;*/ /* 20% */
		openSettings.data.alignment = 12; /* 4K alignment */
#if defined(CONFIG_JAPAN_SPEC)
	    /* it is temporary. it need performance check... ing... */
		openSettings.data.atomSize = 128*1024;
#endif

		if(i < MAX_REC_INST_FOR_REC)
		{
			g_astPvrRecInst[i].instType = DI_PVR_INST_TYPE_REC;
#if defined(CONFIG_PVR_REC_BUFFER_2X)
			openSettings.data.bufferSize = DRV_PVR_REC_BUFFER_LARGE + DRV_PVR_REC_BUFFER_ADD;
#endif
		}
		else if(i < (MAX_REC_INST_FOR_REC+MAX_PVR_INST_FOR_TRANS))
		{
			g_astPvrRecInst[i].instType = DI_PVR_INST_TYPE_TRANS;
		}
		else if(i < (MAX_REC_INST_FOR_REC+MAX_PVR_INST_FOR_TRANS+MAX_PVR_INST_FOR_SATIP))
		{
			g_astPvrRecInst[i].instType = DI_PVR_INST_TYPE_SATIP;
		}
		else
		{
			g_astPvrRecInst[i].instType = DI_PVR_INST_TYPE_NONE;
		}

#if defined(CONFIG_SATIP)
	    if(g_astPvrRecInst[i].instType == DI_PVR_INST_TYPE_SATIP)
	    {
			openSettings.data.atomSize = 188*2;
#if ((NEXUS_VERSION >= 1401) && (NEXUS_VERSION <= 1501))
#else
			openSettings.data.bCheckAlign = false;
#endif
	    }
	    else
	    {
#if ((NEXUS_VERSION >= 1401) && (NEXUS_VERSION <= 1501))
#else
			openSettings.data.bCheckAlign = true;
#endif
	    }
#endif

	    openSettings.index.bufferSize = (ITB_BUFFER_CHUNK_SIZE*ITB_BUFFER_CHUNK_COUNT); /* 6*4 is a size of the single 6-word SCT entry, 16 is a number of SCT entries per descriptor */
	    openSettings.index.alignment = 0; /* unused */
	    openSettings.index.dataReadyThreshold = (ITB_BUFFER_CHUNK_SIZE*(ITB_BUFFER_CHUNK_COUNT/5)); /*brcm_recommendation: set as multiple of 48 */ /*openSettings.index.bufferSize / 5; *//* 20% */

		recPumpHandle = NEXUS_Recpump_Open(i, &openSettings);
		if(!recPumpHandle)
		{
			PrintError("[DRV_PVR_REC_Init] Error in NEXUS_Recpump_Open()\n\r");
			return DRV_ERR;
		}
		recHandle = NEXUS_Record_Create();
	    if (!recHandle)
		{
	        PrintError("[DRV_PVR_REC_Init] Error in NEXUS_Record_Create()\n\r");
			return DRV_ERR;
	    }
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
		/* Allocate encryption keyslot */
	    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
		keySettings.keySlotEngine = NEXUS_SecurityEngine_eCaCp;
#else
	    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
#endif
	    encKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
		if(encKeyHandle == NULL)
		{
	        PrintError("[DRV_PVR_REC_Init] Error in NEXUS_Security_AllocateKeySlot(), instance: %d\n\r", i);
			return DRV_ERR;
	    }
#endif

		/* Create DI semaphore */
		err = VK_SEM_Create(&diSemId, "di_rec", VK_SUSPENDTYPE_FIFO);
		if( err != VK_OK )
		{
			PrintError("[DRV_PVR_REC_Init] Error(0x%x) in VK_SEM_Create()\n\r", err);
			return DRV_ERR;
		}

		g_astPvrRecInst[i].status = DI_PVR_ALLOCATED;
		g_astPvrRecInst[i].timeCodeNotifyInterval = INITIAL_TIMECODE_NOTIFY_INTERVAL;
		g_astPvrRecInst[i].recPumpHandle = recPumpHandle;
		g_astPvrRecInst[i].recordHandle = recHandle;
		g_astPvrRecInst[i].encKeyHandle = encKeyHandle;
		g_astPvrRecInst[i].diSemId = diSemId;
#if defined(REC_DYNAMIC_DMA_ALLOC)
		g_astPvrRecInst[i].dmaHandle=NULL;
#endif
		g_astPvrRecInst[i].bFlush = FALSE;
#if defined(CONFIG_REC_MEMIO)
		g_astPvrRecInst[i].bMemIoOn = FALSE;
#endif
#if defined(USE_VK_INTR_TO_CALLBACK)
		for( j=0; j<DI_PVR_EVENT_MAX_NUMBER; j++ )
		{
			g_astPvrRecInst[i].recEvtCbId[j].valid = FALSE;
		}
#endif
	}

	/* Create semaphore for g_astRecPidChInfo */
	err = VK_SEM_Create(&g_ulSemgRecPid, "RECPID", VK_SUSPENDTYPE_FIFO);
	if( err != VK_OK )
	{
		PrintError("[DRV_PVR_REC_Init] Error(0x%x) in VK_SEM_Create()\n\r", err);
		return DRV_ERR;
	}

#if defined(CONFIG_DELAYED_RECORD)
	/* Allocate encryption keyslot for delayed copy */
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eCaCp;
#else
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
#endif
	encKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	if(encKeyHandle == NULL)
	{
		PrintError("[DRV_PVR_REC_Init] Error in NEXUS_Security_AllocateKeySlot()\n\r");
		return DRV_ERR;
	}
#endif
	/* Create semaphore for delayed recording */
	err = VK_SEM_Create(&dlyRecSemId, "dlySem", VK_SUSPENDTYPE_FIFO);
	if( err != VK_OK )
	{
		PrintError("[DRV_PVR_REC_Init] Error(0x%x) in VK_SEM_Create()\n\r", err);
		return DRV_ERR;
	}
#endif

	/* Time TASK 설정 */
	err = VK_MSG_Create(RECORD_TIMER_TASK_QUEUE_SIZE,
						sizeof(P_REC_TASK_MSG),
						"rTimeMsg",
						&g_ulRecTimeTaskMsgQueueId,
						VK_SUSPENDTYPE_FIFO);
	if(err != VK_OK)
	{
#if	defined(CONFIG_DELAYED_RECORD)
		VK_SEM_Destroy(dlyRecSemId);
#endif
		PrintError("[DRV_PVR_REC_Init] Error(0x%x) in VK_MSG_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_REC_TimeTask,
						RECORD_TIME_TASK_PRIORITY,
						RECORD_TIME_TASK_STACK_SIZE,
						"rTime",
						NULL,
						&g_ulRecTimeTaskId,
						false);
	if(err != VK_OK)
	{
		VK_MSG_Destroy(g_ulRecTimeTaskMsgQueueId);
#if	defined(CONFIG_DELAYED_RECORD)
		VK_SEM_Destroy(dlyRecSemId);
#endif
		PrintError("[DRV_PVR_REC_Init] Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}
	VK_TASK_Start(g_ulRecTimeTaskId);

#if	defined(CONFIG_DELAYED_RECORD)
	/* delayed recording 을 위한 TASK 설정 */
	err = VK_MSG_Create(DELAYED_RECORD_TASK_QUEUE_SIZE,
						sizeof(P_REC_TASK_MSG),
						"dlyMsg",
						&g_ulDelayedRecTaskMsgQueueId,
						VK_SUSPENDTYPE_FIFO);
	if(err != VK_OK)
	{
		VK_TASK_Destroy(g_ulRecTimeTaskId);
		VK_MSG_Destroy(g_ulRecTimeTaskMsgQueueId);
		VK_SEM_Destroy(dlyRecSemId);
		PrintError("[DRV_PVR_REC_Init] Error(0x%x) in VK_MSG_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_REC_DelayedRecordTask,
						DELAYED_RECORD_TASK_PRIORITY,
						DELAYED_RECORD_TASK_STACK_SIZE,
						"dlyrec",
						NULL,
						&g_ulDelayedRecTaskId,
						false);
	if(err != VK_OK)
	{
		VK_MSG_Destroy(g_ulDelayedRecTaskMsgQueueId);
		VK_TASK_Destroy(g_ulRecTimeTaskId);
		VK_MSG_Destroy(g_ulRecTimeTaskMsgQueueId);
		VK_SEM_Destroy(dlyRecSemId);
		PrintError("[DRV_PVR_REC_Init] Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}
	VK_TASK_Start(g_ulDelayedRecTaskId);

	s_stDelayedRecInfo.semId = dlyRecSemId;
	s_stDelayedRecInfo.encKeyHandle = encKeyHandle;
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		DRV_PM_RegisterCallback(DRV_PVR_REC_Uninit, DRV_PVR_REC_Reinit);
#endif

	PrintExit("DRV_PVR_REC_Init");

	return DRV_OK;
}

DI_ERR_CODE DI_PVR_REC_GetCapability(HUINT32 *pulNumOfDeviceId)
{
	PrintEnter("DI_PVR_REC_GetCapability");
	*pulNumOfDeviceId = MAX_REC_INST_NUM;
    PrintExit("DI_PVR_REC_GetCapability");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_REC_GetDeviceType(HUINT32 ulDeviceId, DI_PVR_INST_TYPE *eInstType)
{
	PrintEnter("DI_PVR_REC_DeviceType");
	*eInstType = g_astPvrRecInst[ulDeviceId].instType;
	PrintExit("DI_PVR_REC_DeviceType");

	return DI_ERR_OK;
}

DRV_Error DRV_PVR_REC_GetDeviceIDForTranscoding(HUINT32 ulTranscoderDeviceId, HUINT32 *pulRecDeviceId)
{
#if (MAX_PVR_INST_FOR_TRANS > 0)
	if(ulTranscoderDeviceId >= MAX_PVR_INST_FOR_TRANS)
	{
		PrintError("Transcoder device id %lu is out of range. (device id shall be less than %lu)\n", ulTranscoderDeviceId, MAX_PVR_INST_FOR_TRANS);
		return DRV_ERR;
	}
	*pulRecDeviceId = (MAX_REC_INST_FOR_REC + ulTranscoderDeviceId);
	return DRV_OK;
#else
	BSTD_UNUSED(ulTranscoderDeviceId);
	BSTD_UNUSED(pulRecDeviceId);
	PrintError("Transcoding not supported.\n");
	return DRV_ERR;
#endif
}

#if defined(USE_VK_INTR_TO_CALLBACK)
DI_ERR_CODE DI_PVR_REC_RegisterEventCallback(HUINT32 ulDeviceId, DI_PVR_EVENT_t eEvent, DI_PVR_Event_Callback pCallbackFunc)
{
	int returnValue;
	DI_ERR_CODE	diError = DI_ERR_OK;
	PrintEnter("DI_PVR_REC_RegisterEventCallback");

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_RegisterEventCallback] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status != DI_PVR_FREE)
		{
			switch(eEvent)
			{
				case DI_PVR_EVENT_DISK_FULL:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_DISK_FULL] = pCallbackFunc;
					returnValue = VK_INTR_Register(pCallbackFunc, "rec", sizeof(DI_PVR_MSG_t), &g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_DISK_FULL].intId, VK_INTR_TASK1);
					if(returnValue == VK_OK)
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_DISK_FULL].valid = TRUE;
					}
					else
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_DISK_FULL].valid = FALSE;
						diError = DI_ERR_ERROR;
					}
					break;
				case DI_PVR_EVENT_TIMECODE_CHANGED:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED] = pCallbackFunc;
					returnValue = VK_INTR_Register(pCallbackFunc, "rec", sizeof(DI_PVR_MSG_t), &g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_TIMECODE_CHANGED].intId, VK_INTR_TASK1);
					if(returnValue == VK_OK)
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_TIMECODE_CHANGED].valid = TRUE;
					}
					else
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_TIMECODE_CHANGED].valid = FALSE;
						diError = DI_ERR_ERROR;
					}
					break;
				case DI_PVR_EVENT_STREAM_SCRAMBLED:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_STREAM_SCRAMBLED] = pCallbackFunc;
					returnValue = VK_INTR_Register(pCallbackFunc, "rec", sizeof(DI_PVR_MSG_t), &g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_STREAM_SCRAMBLED].intId, VK_INTR_TASK1);
					if(returnValue == VK_OK)
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_STREAM_SCRAMBLED].valid = TRUE;
					}
					else
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_STREAM_SCRAMBLED].valid = FALSE;
						diError = DI_ERR_ERROR;
					}
					break;
				case DI_PVR_EVENT_STREAM_UNSCRAMBLED:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_STREAM_UNSCRAMBLED] = pCallbackFunc;
					returnValue = VK_INTR_Register(pCallbackFunc, "rec", sizeof(DI_PVR_MSG_t), &g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_STREAM_UNSCRAMBLED].intId, VK_INTR_TASK1);
					if(returnValue == VK_OK)
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_STREAM_UNSCRAMBLED].valid = TRUE;
					}
					else
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_STREAM_UNSCRAMBLED].valid = FALSE;
						diError = DI_ERR_ERROR;
					}
					break;
				case DI_PVR_EVENT_DELAYED_RECORD_STATUS_CHANGED:
#if	defined(CONFIG_DELAYED_RECORD)
					VK_SEM_Get(s_stDelayedRecInfo.semId);
					s_stDelayedRecInfo.delayRecStatusCallbackFunc = pCallbackFunc;
					VK_SEM_Release(s_stDelayedRecInfo.semId);
#endif
					break;
				case DI_PVR_EVENT_REC_BUFFER_OVERFLOW:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_REC_BUFFER_OVERFLOW] = pCallbackFunc;
					returnValue = VK_INTR_Register(pCallbackFunc, "rec", sizeof(DI_PVR_MSG_t), &g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_REC_BUFFER_OVERFLOW].intId, VK_INTR_TASK1);
					if(returnValue == VK_OK)
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_REC_BUFFER_OVERFLOW].valid = TRUE;
					}
					else
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_REC_BUFFER_OVERFLOW].valid = FALSE;
						diError = DI_ERR_ERROR;
					}
					break;
				case DI_PVR_EVENT_REC_WRITE_ERROR:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_REC_WRITE_ERROR] = pCallbackFunc;
					returnValue = VK_INTR_Register(pCallbackFunc, "rec", sizeof(DI_PVR_MSG_t), &g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_REC_WRITE_ERROR].intId, VK_INTR_TASK1);
					if(returnValue == VK_OK)
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_REC_WRITE_ERROR].valid = TRUE;
					}
					else
					{
						g_astPvrRecInst[ulDeviceId].recEvtCbId[DI_PVR_EVENT_REC_WRITE_ERROR].valid = FALSE;
						diError = DI_ERR_ERROR;
					}
					break;
				default:
					PrintError("[DI_PVR_REC_RegisterEventCallback] Invalid event! (eEvent : %d)\n\r", eEvent);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
					break;
			}
		}
		else
		{
			PrintError("[DI_PVR_REC_RegisterEventCallback] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_RegisterEventCallback");

	return diError;
}
#else
DI_ERR_CODE DI_PVR_REC_RegisterEventCallback(HUINT32 ulDeviceId, DI_PVR_EVENT_t eEvent, DI_PVR_Event_Callback pCallbackFunc)
{
	PrintEnter("DI_PVR_REC_RegisterEventCallback");

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_RegisterEventCallback] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status != DI_PVR_FREE)
		{
			switch(eEvent)
			{
				case DI_PVR_EVENT_DISK_FULL:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_DISK_FULL] = pCallbackFunc;
					break;
				case DI_PVR_EVENT_TIMECODE_CHANGED:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED] = pCallbackFunc;
					break;
				case DI_PVR_EVENT_STREAM_SCRAMBLED:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_STREAM_SCRAMBLED] = pCallbackFunc;
					break;
				case DI_PVR_EVENT_STREAM_UNSCRAMBLED:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_STREAM_UNSCRAMBLED] = pCallbackFunc;
					break;
				case DI_PVR_EVENT_DELAYED_RECORD_STATUS_CHANGED:
#if	defined(CONFIG_DELAYED_RECORD)
					VK_SEM_Get(s_stDelayedRecInfo.semId);
					s_stDelayedRecInfo.delayRecStatusCallbackFunc = pCallbackFunc;
					VK_SEM_Release(s_stDelayedRecInfo.semId);
#endif
					break;
				case DI_PVR_EVENT_REC_BUFFER_OVERFLOW:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_REC_BUFFER_OVERFLOW] = pCallbackFunc;
					break;
				case DI_PVR_EVENT_REC_WRITE_ERROR:
					g_astPvrRecInst[ulDeviceId].recEvtCbFunc[DI_PVR_EVENT_REC_WRITE_ERROR] = pCallbackFunc;
					break;
				default:
					PrintError("[DI_PVR_REC_RegisterEventCallback] Invalid event! (eEvent : %d)\n\r", eEvent);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
					break;
			}
		}
		else
		{
			PrintError("[DI_PVR_REC_RegisterEventCallback] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_RegisterEventCallback");

	return DI_ERR_OK;
}
#endif

#if defined(CONFIG_DI20)
#if defined(CONFIG_REC_MEMIO)
DI_PVR_Rec_DataRdy_Callback cbRecordDataReady = NULL;

int P_REC_MEMIO_DATA_CB(int deviceId, const void *buf, size_t length)
{
	if((g_astPvrRecInst[deviceId].recordSuspend == TRUE)&&(g_astPvrRecInst[deviceId].bFlush == TRUE))
	{
		return length;
	}

	if(cbRecordDataReady != NULL)
	{
		cbRecordDataReady(deviceId, buf, length);
	}
	return length;
}

DI_ERR_CODE DI_PVR_REC_RegisterMemIOCallback(HUINT32 ulDeviceId, DI_PVR_Rec_DataRdy_Callback pCallbackFunc)
{
	PrintEnter("DI_PVR_REC_RegisterMemIOCallback");

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_RegisterMemIOCallback] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status != DI_PVR_FREE)
		{
			cbRecordDataReady = pCallbackFunc;
		}
		else
		{
			PrintError("[DI_PVR_REC_RegisterMemIOCallback] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_RegisterMemIOCallback");

	return DI_ERR_OK;
}
#else
DI_ERR_CODE DI_PVR_REC_RegisterMemIOCallback(HUINT32 ulDeviceId, DI_PVR_Rec_DataRdy_Callback pCallbackFunc)
{
	BSTD_UNUSED(ulDeviceId);
	BSTD_UNUSED(pCallbackFunc);
	PrintError("[DI_PVR_REC_RegisterMemIOCallback] Not supported.\n\r");

	return DI_ERR_NOT_SUPPORTED;
}
#endif
#endif
DI_ERR_CODE DI_PVR_REC_Setup(HUINT32 ulDeviceId, DI_PVR_REC_SETUP_t *pstSetup )
{
	NEXUS_Error	neResult;
	DRV_Error drvErr;
	char *avFileName = NULL;
	char *indexFileName = NULL;
	NEXUS_FileRecordHandle	fileRecHandle = NULL;
#if defined(ENABLE_RECORD_CHUNK_FILE)
	NEXUS_ChunkedFifoRecordHandle 	fifoRecHandle = NULL;
	NEXUS_ChunkedFifoRecordSettings fifoRecordSettings;
#else
	NEXUS_FifoRecordHandle 	fifoRecHandle = NULL;
	NEXUS_FifoRecordSettings	fifoRecordCfg;
#endif
 	NEXUS_RecordSettings	recordCfg;

	PrintEnter("DI_PVR_REC_Setup (devId=%d)", ulDeviceId);

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_Setup] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
#if defined(CONFIG_DELAYED_RECORD)
		g_astPvrRecInst[ulDeviceId].switchedfromTsrToLinear =0;
#endif
		g_astPvrRecInst[ulDeviceId].bTsRec = FALSE;

#if defined(CONFIG_REC_MEMIO)
		if(VK_memcmp((void *)pstSetup->aucFileName, (void *)"memio",5) == 0)
		{
			g_astPvrRecInst[ulDeviceId].bMemIoOn = TRUE;
			pstSetup->eFileType = eDI_PVR_FileType_Linear;
		}
		else
		{
			g_astPvrRecInst[ulDeviceId].bMemIoOn = FALSE;
		}
#elif defined(CONFIG_TRANSCODER_MEMIO)
		g_astPvrRecInst[ulDeviceId].bMemIoOn = FALSE;
#endif
		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ALLOCATED)
		{
			avFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(avFileName == NULL)
			{
				PrintError("[DI_PVR_REC_Setup] Error in DD_MEM_Alloc()\n\r");
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			indexFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(indexFileName == NULL)
			{
				PrintError("[DI_PVR_REC_Setup] Error in DD_MEM_Alloc()\n\r");
				DD_MEM_Free(avFileName);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			VK_memcpy((void *)avFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
#if defined(CONFIG_USE_PRIVATE_FILE_EXTENTION_HTS)
			VK_strncat(avFileName, ".hts\0", 4);
#else
			VK_strncat(avFileName, ".ts\0", 3); // Add  fileExtension
#endif
			VK_memcpy((void *)indexFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
			VK_strncat(indexFileName, ".nts\0", 4); // Add  fileExtension
			if(pstSetup->eFileType == eDI_PVR_FileType_Linear)
			{
#if 0 /*defined(ENABLE_RECORD_CHUNK_FILE)*//*Linear recording files may not be chunked.*/
				mkdir(avFileName); /* directory must already exist */
				fileRecHandle = NEXUS_ChunkedFileRecord_Open(avFileName, indexFileName, NULL);
#else
#if defined(CONFIG_REC_MEMIO)
				if(g_astPvrRecInst[ulDeviceId].bMemIoOn == TRUE)
				{
					fileRecHandle = NEXUS_FileTranscode_OpenPosix(avFileName, indexFileName, P_REC_MEMIO_DATA_CB, ulDeviceId );
				}
				else
				{
					fileRecHandle = NEXUS_FileRecord_OpenPosix(avFileName, indexFileName);
				}
#else
#if defined(CONFIG_USB_FLASH_RECORD)
				if(pstSetup->stUsbFlash.bIsUsbFlashRecord == TRUE)
				{
					fileRecHandle = NEXUS_FileRecord_OpenPosixForUsbFlash(avFileName, indexFileName);
				}
				else
#endif
				{
					fileRecHandle = NEXUS_FileRecord_OpenPosix(avFileName, indexFileName);
				}
#endif
#endif

			    if (!fileRecHandle)
				{
			        PrintError("[DI_PVR_REC_Setup] Error in NEXUS_FileRecord_OpenPosix()\n\r");
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }
			}
			else if(pstSetup->eFileType == eDI_PVR_FileType_Circular)
			{
#if defined(ENABLE_RECORD_CHUNK_FILE)
				fifoRecHandle = NEXUS_ChunkedFifoRecord_Create(avFileName, indexFileName);
				if(fifoRecHandle == NULL)
				{
					PrintError("[DI_PVR_REC_Setup] Error in NEXUS_ChunkedFifoRecord_Create(%s,%s)\n\r", avFileName,indexFileName);
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
				NEXUS_ChunkedFifoRecord_GetSettings(fifoRecHandle, &fifoRecordSettings);
#if defined(CONFIG_USB_FLASH_RECORD)
				if (pstSetup->stUsbFlash.bIsUsbFlashRecord == TRUE)
				{
					fifoRecordSettings.interval = ((pstSetup->stUsbFlash.ulLimitTimeSecond * 90) / 100); /* 10% margin */
				}
				else
#endif
				{
					fifoRecordSettings.interval = TSR_TIME_LIMIT;
				}
				fifoRecordSettings.data.chunkSize = RECORD_CHUNK_SIZE;
				neResult = NEXUS_ChunkedFifoRecord_SetSettings(fifoRecHandle, &fifoRecordSettings);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[DI_PVR_REC_Setup] Error(0x%x) in NEXUS_FifoRecord_SetSettings()\n\r", neResult);
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
				fileRecHandle = NEXUS_ChunkedFifoRecord_GetFile(fifoRecHandle);
				if (!fileRecHandle)
				{
					PrintError("[DI_PVR_REC_Setup] Error in NEXUS_FifoRecord_GetFile()\n\r");
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
#else
#if defined(CONFIG_USB_FLASH_RECORD)
				if(pstSetup->stUsbFlash.bIsUsbFlashRecord == TRUE)
				{
					fifoRecHandle = NEXUS_FifoRecord_CreateForUsbFlash(avFileName, indexFileName);
				}
				else
#endif
				{
					fifoRecHandle = NEXUS_FifoRecord_Create(avFileName, indexFileName);
				}
				if (!fifoRecHandle)
				{
			        PrintError("[DI_PVR_REC_Setup] Error in NEXUS_FifoRecord_Create()\n\r");
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }
				NEXUS_FifoRecord_GetSettings(fifoRecHandle, &fifoRecordCfg);
#if defined(CONFIG_USB_FLASH_RECORD)
				if(pstSetup->stUsbFlash.bIsUsbFlashRecord == TRUE)
				{
					fifoRecordCfg.interval = ((pstSetup->stUsbFlash.ulLimitTimeSecond * 90) / 100); /* 10% margin */
					fifoRecordCfg.data.soft = ((pstSetup->stUsbFlash.ullLimitDataSize * 90) / 100); /* 10% margin */
					fifoRecordCfg.data.hard = pstSetup->stUsbFlash.ullLimitDataSize;
					fifoRecordCfg.data.soft = DATA_ALIGN*(fifoRecordCfg.data.soft/DATA_ALIGN);
					fifoRecordCfg.data.hard = DATA_ALIGN*(fifoRecordCfg.data.hard/DATA_ALIGN);
					fifoRecordCfg.index.soft = ((pstSetup->stUsbFlash.ullLimitIndexSize * 90) / 100); /* 10% margin */
					fifoRecordCfg.index.hard = pstSetup->stUsbFlash.ullLimitIndexSize;
				}
				else
#endif
				{
					fifoRecordCfg.interval = TSR_TIME_LIMIT;
					fifoRecordCfg.data.soft = TSR_DATA_SOFT_LIMIT;
					fifoRecordCfg.data.hard = TSR_DATA_HARD_LIMIT;
					fifoRecordCfg.data.soft = DATA_ALIGN*(fifoRecordCfg.data.soft/DATA_ALIGN);
					fifoRecordCfg.data.hard = DATA_ALIGN*(fifoRecordCfg.data.hard/DATA_ALIGN);
					fifoRecordCfg.index.soft = TSR_INDEX_SOFT_LIMIT;
					fifoRecordCfg.index.hard = TSR_INDEX_HARD_LIMIT;
				}
				neResult = NEXUS_FifoRecord_SetSettings(fifoRecHandle, &fifoRecordCfg);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[DI_PVR_REC_Setup] Error(0x%x) in NEXUS_FifoRecord_SetSettings()\n\r", neResult);
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }
    			fileRecHandle = NEXUS_FifoRecord_GetFile(fifoRecHandle);
				if (!fileRecHandle)
				{
			        PrintError("[DI_PVR_REC_Setup] Error in NEXUS_FifoRecord_GetFile()\n\r");
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }
#endif
			}
			DD_MEM_Free(avFileName);
			DD_MEM_Free(indexFileName);

			NEXUS_Record_GetDefaultSettings(&recordCfg);
		    recordCfg.indexFormat = NEXUS_RecordIndexType_eBcm;
#if defined(CONFIG_SATIP)
			if(g_astPvrRecInst[ulDeviceId].instType == DI_PVR_INST_TYPE_SATIP)
			{
				recordCfg.pollingTimer = MAX_PVR_REC_SATIP_TIMEOUT; 
			}
#endif
#if	defined(CONFIG_DELAYED_RECORD)
			recordCfg.fifoRecord = pstSetup->eFileType == eDI_PVR_FileType_Circular ? true : false;
#endif
		    recordCfg.recpump = g_astPvrRecInst[ulDeviceId].recPumpHandle;

			recordCfg.recpumpSettings.tpit.storePcrMsb = true;
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
			if(pstSetup->ulPacketSize != 188)
			{
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
				recordCfg.recpumpSettings.timestampType = NEXUS_TransportTimestampType_e32_Binary;
#else /*CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY*/
				recordCfg.recpumpSettings.timestampType = NEXUS_TransportTimestampType_eMod300;
#endif /*CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY*/
			}
			else
			{
				recordCfg.recpumpSettings.timestampType =  NEXUS_TransportTimestampType_eNone;
			}

#else /*CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH*/

#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
			recordCfg.recpumpSettings.timestampType = NEXUS_TransportTimestampType_e32_Binary;
#else /*CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY*/
			recordCfg.recpumpSettings.timestampType = NEXUS_TransportTimestampType_eMod300;
#endif /*CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY*/
#endif /*CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD*/

#endif /*CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH*/

			recordCfg.overflowCallback.callback = P_REC_BufferOverflowCallback;
			recordCfg.overflowCallback.context = (void *)&g_astPvrRecInst[ulDeviceId];
			recordCfg.overflowCallback.param = (int)ulDeviceId;
			recordCfg.errorCallback.callback = P_REC_WriteErrorCallback;
			recordCfg.errorCallback.context = (void *)&g_astPvrRecInst[ulDeviceId];
			recordCfg.errorCallback.param = (int)ulDeviceId;
			if(pstSetup->stEncryptSettings.eEncType == DI_PVR_ENCRYPTION_TYPE_NONE)
			{
				recordCfg.recpumpSettings.securityDma = NULL;
				recordCfg.recpumpSettings.securityContext = NULL;
			}
#if (NEXUS_VERSION >= 1502) 
#if	defined(CONFIG_REC_MEMIO) || defined(CONFIG_TRANSCODER_MEMIO)
			if(g_astPvrRecInst[ulDeviceId].bMemIoOn == TRUE)
			{
				recordCfg.memio = true;
			}
			else
			{
				recordCfg.memio = false;
			}
#else
			recordCfg.memio = false;
#endif
#endif
			
		    neResult = NEXUS_Record_SetSettings(g_astPvrRecInst[ulDeviceId].recordHandle, &recordCfg);
			if(neResult != NEXUS_SUCCESS)
			{
				NEXUS_FileRecord_Close(fileRecHandle);
				PrintError("[DI_PVR_REC_Setup] Error(0x%x) in NEXUS_Record_SetSettings()\n\r", neResult);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
		    }

			if(pstSetup->stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
			{
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
				drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].encKeyHandle, &(pstSetup->stEncryptSettings), (pstSetup->ulPacketSize!=188?TRUE:FALSE));
#else
				drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].encKeyHandle, &(pstSetup->stEncryptSettings));
#endif
				if (drvErr != DRV_OK)
				{
					NEXUS_FileRecord_Close(fileRecHandle);
					PrintError("[DI_PVR_REC_Setup] Error(0x%x) in P_REC_SetEncryption()\n\r", drvErr);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }
				g_astPvrRecInst[ulDeviceId].bEncrpytionSetted = true;
			}

			g_astPvrRecInst[ulDeviceId].setup = *pstSetup;
			g_astPvrRecInst[ulDeviceId].fileRecHandle = fileRecHandle;
			g_astPvrRecInst[ulDeviceId].fifoRecHandle = fifoRecHandle;
			g_astPvrRecInst[ulDeviceId].lastScrambleStatus = false; /* default clear */
			g_astPvrRecInst[ulDeviceId].recordSuspend = false;
		}
		else
		{
			PrintError("[DI_PVR_REC_Setup] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
#if defined(CONFIG_USB_FLASH_RECORD)
		if(pstSetup->stUsbFlash.bIsUsbFlashRecord == TRUE)
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
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_Setup");

	return DI_ERR_OK;
}

#if defined(CONFIG_TRANSCODER)
#if defined(CONFIG_TRANSCODER_MEMIO)
DI_TRANCODE_DataRdy_Callback cbTranscodeDataReady = NULL;

int P_TRANSCODE_DATA_CB(int deviceId, const void *buf, size_t length)
{
	if(cbTranscodeDataReady != NULL)
	{
		cbTranscodeDataReady(deviceId, buf, length);
	}
	return length;
}

DI_ERR_CODE DI_TRANSCODE_RegisterCallback(HUINT32 ulDeviceId, DI_TRANCODE_DataRdy_Callback pCallbackFunc)
{
	PrintEnter("DI_TRANSCODE_RegisterCallback");

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_RegisterEventCallback] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status != DI_PVR_FREE)
		{
			cbTranscodeDataReady = pCallbackFunc;
		}
		else
		{
			PrintError("[DI_TRANSCODE_RegisterCallback] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_TRANSCODE_RegisterCallback");

	return DI_ERR_OK;
}
#endif

NEXUS_VideoCodec P_TRANSCODE_Convert2NexusCodec(DI_VIDEO_FORMAT VideoCodec)
{
	NEXUS_VideoCodec nexusVideoCodec;

	switch(VideoCodec)
	{
		case DI_VIDEO_FORMAT_MPEG1:
		case DI_VIDEO_FORMAT_MEDIA_MPEG1:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg1;
			break;
		case DI_VIDEO_FORMAT_MPEG2:
		case DI_VIDEO_FORMAT_MEDIA_MPEG2:
		case DI_VIDEO_FORMAT_STILL_MPEG2:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg2;
			break;
		case DI_VIDEO_FORMAT_MPEG4Part2:
		case DI_VIDEO_FORMAT_MEDIA_MPEG4Part2:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg4Part2;
			break;
		case DI_VIDEO_FORMAT_H261:	//divx test
		case DI_VIDEO_FORMAT_MEDIA_DIVX_311:
			// nexusVideoCodec = NEXUS_VideoCodec_eUnknown; /*seemed to be unsupported in Nexus platform, but can be modified.*/
			nexusVideoCodec = NEXUS_VideoCodec_eDivx311; /*seemed to be unsupported in Nexus platform, but can be modified.*/
			break;
		case DI_VIDEO_FORMAT_H263:
		case DI_VIDEO_FORMAT_MEDIA_H263:
			nexusVideoCodec = NEXUS_VideoCodec_eH263;
			break;
		case DI_VIDEO_FORMAT_H264:
		case DI_VIDEO_FORMAT_MEDIA_H264:
		case DI_VIDEO_FORMAT_STILL_H264:
			nexusVideoCodec = NEXUS_VideoCodec_eH264;
			break;
		case DI_VIDEO_FORMAT_VC1SimpleMain:
		case DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain:
			nexusVideoCodec = NEXUS_VideoCodec_eVc1SimpleMain;
			break;
		case DI_VIDEO_FORMAT_VC1:
		case DI_VIDEO_FORMAT_MEDIA_VC1:
		case DI_VIDEO_FORMAT_STILL_VC1:
			nexusVideoCodec = NEXUS_VideoCodec_eVc1;
			break;
		case DI_VIDEO_FORMAT_AVS:
		case DI_VIDEO_FORMAT_MEDIA_AVS:
			nexusVideoCodec = NEXUS_VideoCodec_eAvs;
			break;
		default:
			nexusVideoCodec = NEXUS_VideoCodec_eUnknown;
			break;
	}
	return nexusVideoCodec;
}

DI_ERR_CODE DI_TRANSCODE_REC_Setup(HUINT32 ulDeviceId, HUINT32 ulMuxId, DI_PVR_REC_SETUP_t *pstSetup )
{
	NEXUS_Error	neResult;
	DRV_Error drvErr = 0,dErr = 0;
	char *avFileName = NULL;
	char *indexFileName = NULL;
	NEXUS_FileRecordHandle	fileRecHandle = NULL;
#if defined(ENABLE_RECORD_CHUNK_FILE)
	NEXUS_ChunkedFifoRecordHandle 	fifoRecHandle = NULL;
	NEXUS_ChunkedFifoRecordSettings fifoRecordSettings;
#else
	NEXUS_FifoRecordHandle 	fifoRecHandle = NULL;
	NEXUS_FifoRecordSettings	fifoRecordCfg;
#endif
 	NEXUS_RecordSettings	recordCfg;
	NEXUS_RecordPidChannelSettings		recordPidSettings;
	HUINT32	psiNumber = 0,index = 0;

	PrintEnter("DI_TRANSCODE_REC_Setup (devId=%d)", ulDeviceId);

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_TRANSCODE_REC_Setup] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
#if defined(CONFIG_DELAYED_RECORD)
		g_astPvrRecInst[ulDeviceId].switchedfromTsrToLinear =0;
#endif
#if defined(CONFIG_TRANSCODER_MEMIO)
		if(VK_memcmp((void *)pstSetup->aucFileName, (void *)"memio",5) == 0)
		{
			g_astPvrRecInst[ulDeviceId].bMemIoOn = TRUE;
			pstSetup->eFileType = eDI_PVR_FileType_Linear;
		}
		else
		{
			g_astPvrRecInst[ulDeviceId].bMemIoOn = FALSE;
		}
#elif defined(CONFIG_REC_MEMIO)
		g_astPvrRecInst[ulDeviceId].bMemIoOn = FALSE;
#endif
		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ALLOCATED)
		{
			avFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(avFileName == NULL)
			{
				PrintError("[DI_TRANSCODE_REC_Setup] Error in DD_MEM_Alloc()\n\r");
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			indexFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(indexFileName == NULL)
			{
				PrintError("[DI_PVR_REC_Setup] Error in DD_MEM_Alloc()\n\r");
				DD_MEM_Free(avFileName);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			VK_memcpy((void *)avFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
			VK_strncat(avFileName, ".ts\0", 3); // Add  fileExtension
			VK_memcpy((void *)indexFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
			VK_strncat(indexFileName, ".nts\0", 4); // Add  fileExtension

			if(pstSetup->eFileType == eDI_PVR_FileType_Linear)
			{
#if 0 /*defined(ENABLE_RECORD_CHUNK_FILE)*//*Linear recording files may not be chunked.*/
				mkdir(avFileName); /* directory must already exist */
				fileRecHandle = NEXUS_ChunkedFileRecord_Open(avFileName, indexFileName, NULL);
#else
#if defined(CONFIG_TRANSCODER_MEMIO)
				if(g_astPvrRecInst[ulDeviceId].bMemIoOn == TRUE)
				{
					fileRecHandle = NEXUS_FileTranscode_OpenPosix(avFileName, indexFileName, P_TRANSCODE_DATA_CB, ulDeviceId );
				}
				else
				{
					fileRecHandle = NEXUS_FileRecord_OpenPosix(avFileName, indexFileName);
				}
#else
				fileRecHandle = NEXUS_FileRecord_OpenPosix(avFileName, indexFileName);
#endif
#endif
			    if (!fileRecHandle)
				{
			        PrintError("[DI_TRANSCODE_REC_Setup] Error in NEXUS_FileRecord_OpenPosix()\n\r");
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }
			}
			else if(pstSetup->eFileType == eDI_PVR_FileType_Circular)
			{
#if defined(ENABLE_RECORD_CHUNK_FILE)
				fifoRecHandle = NEXUS_ChunkedFifoRecord_Create(avFileName, indexFileName);
				NEXUS_ChunkedFifoRecord_GetSettings(fifoRecHandle, &fifoRecordSettings);
				fifoRecordSettings.interval = TSR_TIME_LIMIT;
				fifoRecordSettings.data.chunkSize = RECORD_CHUNK_SIZE;
				neResult = NEXUS_ChunkedFifoRecord_SetSettings(fifoRecHandle, &fifoRecordSettings);

				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_FifoRecord_SetSettings()\n\r", neResult);
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
				fileRecHandle = NEXUS_ChunkedFifoRecord_GetFile(fifoRecHandle);
				if (!fileRecHandle)
				{
					PrintError("[DI_TRANSCODE_REC_Setup] Error in NEXUS_FifoRecord_GetFile()\n\r");
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
#else
				fifoRecHandle = NEXUS_FifoRecord_Create(avFileName, indexFileName);
				if (!fifoRecHandle)
				{
			        PrintError("[DI_TRANSCODE_REC_Setup] Error in NEXUS_FifoRecord_Create()\n\r");
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }

				NEXUS_FifoRecord_GetSettings(fifoRecHandle, &fifoRecordCfg);
				fifoRecordCfg.interval = TSR_TIME_LIMIT;
				fifoRecordCfg.data.soft = TSR_DATA_SOFT_LIMIT;
				fifoRecordCfg.data.hard = TSR_DATA_HARD_LIMIT;
				fifoRecordCfg.data.soft = DATA_ALIGN*(fifoRecordCfg.data.soft/DATA_ALIGN);
				fifoRecordCfg.data.hard = DATA_ALIGN*(fifoRecordCfg.data.hard/DATA_ALIGN);
				fifoRecordCfg.index.soft = TSR_INDEX_SOFT_LIMIT;
				fifoRecordCfg.index.hard = TSR_INDEX_HARD_LIMIT;
				neResult = NEXUS_FifoRecord_SetSettings(fifoRecHandle, &fifoRecordCfg);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_FifoRecord_SetSettings()\n\r", neResult);
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }
    			fileRecHandle = NEXUS_FifoRecord_GetFile(fifoRecHandle);
				if (!fileRecHandle)
				{
			        PrintError("[DI_TRANSCODE_REC_Setup] Error in NEXUS_FifoRecord_GetFile()\n\r");
					DD_MEM_Free(avFileName);
					DD_MEM_Free(indexFileName);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }
#endif
			}
			DD_MEM_Free(avFileName);
			DD_MEM_Free(indexFileName);

			NEXUS_Record_GetDefaultSettings(&recordCfg);
		    recordCfg.indexFormat = NEXUS_RecordIndexType_eBcm;
#if	defined(CONFIG_DELAYED_RECORD)
			recordCfg.fifoRecord = pstSetup->eFileType == eDI_PVR_FileType_Circular ? true : false;
#endif
		    recordCfg.recpump = g_astPvrRecInst[ulDeviceId].recPumpHandle;

#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
			if(pstSetup->ulPacketSize != 188)
			{
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
				DRV_STREAM_MUX_GetTranscodePCR(ulMuxId,&(g_pidChannelTranscodePcr[ulMuxId]));

				recordCfg.recpumpSettings.timestampType =  NEXUS_TransportTimestampType_e32_Binary;
				recordCfg.recpumpSettings.localTimestamp = true;
				recordCfg.recpumpSettings.adjustTimestampUsingPcrs = true;
				recordCfg.recpumpSettings.pcrPidChannel = g_pidChannelTranscodePcr[ulMuxId];
				recordCfg.recpumpSettings.dropBtpPackets = true;
#else /*CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY*/
				recordCfg.recpumpSettings.timestampType = NEXUS_TransportTimestampType_eMod300;
#endif /*CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY*/
			}
			else
			{
				recordCfg.recpumpSettings.timestampType =  NEXUS_TransportTimestampType_eNone;
			}
#else /*CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH*/

#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
			DRV_STREAM_MUX_GetTranscodePCR(ulMuxId,&(g_pidChannelTranscodePcr[ulMuxId]));
			recordCfg.recpumpSettings.timestampType =  NEXUS_TransportTimestampType_e32_Binary;
			recordCfg.recpumpSettings.localTimestamp = true;
			recordCfg.recpumpSettings.adjustTimestampUsingPcrs = true;
			recordCfg.recpumpSettings.pcrPidChannel = g_pidChannelTranscodePcr[ulMuxId];
			recordCfg.recpumpSettings.dropBtpPackets = true;
#else /*CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY*/
			recordCfg.recpumpSettings.timestampType = NEXUS_TransportTimestampType_eMod300;
#endif /*CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY*/
#endif /*CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD*/

#endif /*CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH*/

			recordCfg.overflowCallback.callback = P_REC_BufferOverflowCallback;
			recordCfg.overflowCallback.context = (void *)&g_astPvrRecInst[ulDeviceId];
			recordCfg.overflowCallback.param = (int)ulDeviceId;
			recordCfg.errorCallback.callback = P_REC_WriteErrorCallback;
			recordCfg.errorCallback.context = (void *)&g_astPvrRecInst[ulDeviceId];
			recordCfg.errorCallback.param = (int)ulDeviceId;
			if(pstSetup->stEncryptSettings.eEncType == DI_PVR_ENCRYPTION_TYPE_NONE)
			{
				recordCfg.recpumpSettings.securityDma = NULL;
				recordCfg.recpumpSettings.securityContext = NULL;
			}

#if (NEXUS_VERSION >= 1502) 
#if	defined(CONFIG_REC_MEMIO) || defined(CONFIG_TRANSCODER_MEMIO)
			if(g_astPvrRecInst[ulDeviceId].bMemIoOn == TRUE)
			{
				recordCfg.memio = true;
			}
			else
			{
				recordCfg.memio = false;
			}
#else
			recordCfg.memio = false;
#endif
#endif
		    neResult = NEXUS_Record_SetSettings(g_astPvrRecInst[ulDeviceId].recordHandle, &recordCfg);
			if(neResult != NEXUS_SUCCESS)
			{
				NEXUS_FileRecord_Close(fileRecHandle);
				PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_Record_SetSettings()\n\r", neResult);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
		    }

			if(pstSetup->stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
			{
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
				drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].encKeyHandle, &(pstSetup->stEncryptSettings), (pstSetup->ulPacketSize!=188?TRUE:FALSE));
#else
				drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].encKeyHandle, &(pstSetup->stEncryptSettings));
#endif
				if (drvErr != DRV_OK)
				{
					NEXUS_FileRecord_Close(fileRecHandle);
					PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in P_REC_SetEncryption()\n\r", drvErr);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }
			}

			NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
			recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
			recordPidSettings.recpumpSettings.pidTypeSettings.video.index = true;
			recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = P_TRANSCODE_Convert2NexusCodec(pstSetup->eVideoType);

			DRV_STREAM_MUX_GetTranscodeVideo(ulMuxId,&(g_pidChannelTranscodeVideo[ulMuxId]));

			if(g_pidChannelTranscodeVideo[ulMuxId] != NULL)
			{
				/* add multiplex data to the same record */
				neResult = NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodeVideo[ulMuxId], &recordPidSettings);
				if(neResult != NEXUS_SUCCESS)
				{
					NEXUS_FileRecord_Close(fileRecHandle);
					PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", neResult);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
				if(pstSetup->stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
				{
					neResult = NEXUS_KeySlot_AddPidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, g_pidChannelTranscodeVideo[ulMuxId]);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_KeySlot_AddPidChannel()\n\r", neResult);
					}
				}
#endif
			}

			DRV_STREAM_MUX_GetTranscodeAudio(ulMuxId,&(g_pidChannelTranscodeAudio[ulMuxId]),&(g_pidChannelTranscodeSubAudio[ulMuxId]));
			if(g_pidChannelTranscodeAudio[ulMuxId] != NULL)
			{
				neResult= NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodeAudio[ulMuxId], NULL);
				if(neResult != NEXUS_SUCCESS)
				{
					NEXUS_FileRecord_Close(fileRecHandle);
					PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", neResult);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
				if(pstSetup->stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
				{
					neResult = NEXUS_KeySlot_AddPidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, g_pidChannelTranscodeAudio[ulMuxId]);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_KeySlot_AddPidChannel()\n\r", neResult);
					}
				}
#endif
			}
			if(g_pidChannelTranscodeSubAudio[ulMuxId] != NULL)
			{
				neResult= NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodeSubAudio[ulMuxId], NULL);
				if(neResult != NEXUS_SUCCESS)
				{
					NEXUS_FileRecord_Close(fileRecHandle);
					PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", neResult);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
				if(pstSetup->stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
				{
					neResult = NEXUS_KeySlot_AddPidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, g_pidChannelTranscodeSubAudio[ulMuxId]);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_KeySlot_AddPidChannel()\n\r", neResult);
					}
				}
#endif
			}


			DRV_STREAM_MUX_GetTranscodePCR(ulMuxId,&(g_pidChannelTranscodePcr[ulMuxId]));
			if(g_pidChannelTranscodePcr[ulMuxId] != NULL)
			{
				neResult = NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodePcr[ulMuxId], NULL);
				if(neResult != NEXUS_SUCCESS)
				{
					NEXUS_FileRecord_Close(fileRecHandle);
					PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", neResult);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
			}

			drvErr = DRV_STREAM_MUX_GetUsedPSI(ulMuxId,&psiNumber);
			if((drvErr == 0)&&(psiNumber > 0))
			{
				for(index = 0; index < psiNumber; index++)
				{
					dErr = DRV_STREAM_MUX_GetTranscodePSI(ulMuxId,index,&(g_pidChannelTranscodePsi[ulMuxId][index]));
					if((dErr == 0)&&(g_pidChannelTranscodePsi[ulMuxId][index] != NULL))
					{
						neResult = NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodePsi[ulMuxId][index], NULL);
						if(neResult != NEXUS_SUCCESS)
						{
							NEXUS_FileRecord_Close(fileRecHandle);
							PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", neResult);
							VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
							return DI_ERR_ERROR;
						}
					}
				}
			}

			drvErr = DRV_STREAM_MUX_GetUsedPES(ulMuxId,&psiNumber);
			if((drvErr == 0)&&(psiNumber > 0))
			{
				for(index = 0; index < psiNumber; index++)
				{
					dErr = DRV_STREAM_MUX_GetTranscodePES(ulMuxId,index,&(g_pidChannelTranscodePes[ulMuxId][index]));
					if((dErr == 0)&&(g_pidChannelTranscodePes[ulMuxId][index] != NULL))
					{
						neResult = NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodePes[ulMuxId][index], NULL);
						if(neResult != NEXUS_SUCCESS)
						{
							NEXUS_FileRecord_Close(fileRecHandle);
							PrintError("[DI_TRANSCODE_REC_Setup] Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", neResult);
							VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
							return DI_ERR_ERROR;
						}
					}
				}
			}

			g_astPvrRecInst[ulDeviceId].setup = *pstSetup;
			g_astPvrRecInst[ulDeviceId].fileRecHandle = fileRecHandle;
			g_astPvrRecInst[ulDeviceId].fifoRecHandle = fifoRecHandle;
			g_astPvrRecInst[ulDeviceId].lastScrambleStatus = false; /* default clear */
			g_astPvrRecInst[ulDeviceId].recordSuspend = false;
		}
		else
		{
			PrintError("[DI_TRANSCODE_REC_Setup] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_TRANSCODE_REC_Setup");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_TRANSCODE_REC_Start(HUINT32 ulDeviceId )
{
	DRV_Error drvErr;

	PrintEnter("DI_TRANSCODE_REC_Start (devId=%d)", ulDeviceId);

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_Start] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ALLOCATED)
		{
			drvErr = P_REC_Start(ulDeviceId);
			if(drvErr != DRV_OK)
			{
				PrintError("[DI_TRANSCODE_REC_Start] Error(0x%x) in P_REC_Start()\n\r", drvErr);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			g_astPvrRecInst[ulDeviceId].status = DI_PVR_ENABLED;
			g_astPvrRecInst[ulDeviceId].delayedTimeOffset = 0;

		}
		else
		{
			PrintError("[DI_TRANSCODE_REC_Start] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_TRANSCODE_REC_Start");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_TRANSCODE_REC_Stop(HUINT32 ulDeviceId )
{
	int err = 0;

	PrintEnter("DI_TRANSCODE_REC_Stop (devId=%d)", ulDeviceId);

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_TRANSCODE_REC_Stop] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
#if defined(CONFIG_DELAYED_RECORD)
		VK_SEM_Get(s_stDelayedRecInfo.semId);
		if( g_astPvrRecInst[ulDeviceId].fifoRecHandle != NULL && s_stDelayedRecInfo.fifoRecHandle != NULL
			&& g_astPvrRecInst[ulDeviceId].fifoRecHandle == s_stDelayedRecInfo.fifoRecHandle
			&& s_stDelayedRecInfo.used == true)
		{
			s_stDelayedRecInfo.used = false;
		}
		VK_SEM_Release(s_stDelayedRecInfo.semId);
#endif
		if(!(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPvrRecInst[ulDeviceId].fileRecHandle))
		{
			PrintError("[DI_TRANSCODE_REC_Stop] Invalid device status! (status : %d, fileRecHandle : 0x%x)\n\r",
				g_astPvrRecInst[ulDeviceId].status, (unsigned long)g_astPvrRecInst[ulDeviceId].fileRecHandle);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}

		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED)
		{
			if (g_astPvrRecInst[ulDeviceId].timeCodeNotifyTimerId != VK_TIMERID_NULL)
			{
				err = VK_TIMER_Cancel(g_astPvrRecInst[ulDeviceId].timeCodeNotifyTimerId);
				if(err != VK_OK )
				{
					PrintError("[DI_TRANSCODE_REC_Stop] Error(0x%x) in VK_TIMER_Cancel\n", err);
				}
				g_astPvrRecInst[ulDeviceId].timeCodeNotifyTimerId = VK_TIMERID_NULL;
			}
			NEXUS_Record_Stop(g_astPvrRecInst[ulDeviceId].recordHandle);
		}
#if defined(REC_DYNAMIC_DMA_ALLOC)
		if(g_astPvrRecInst[ulDeviceId].dmaHandle != NULL)
		{
			NEXUS_Dma_Close(g_astPvrRecInst[ulDeviceId].dmaHandle);
			g_astPvrRecInst[ulDeviceId].dmaHandle=NULL;
		}
#endif
		NEXUS_FileRecord_Close(g_astPvrRecInst[ulDeviceId].fileRecHandle);

		g_astPvrRecInst[ulDeviceId].status = DI_PVR_ALLOCATED;
		g_astPvrRecInst[ulDeviceId].requestRealStart = false;
		g_astPvrRecInst[ulDeviceId].fileRecHandle = NULL;
		g_astPvrRecInst[ulDeviceId].fifoRecHandle = NULL;
#if defined(CONFIG_REC_MEMIO) || defined(CONFIG_TRANSCODER_MEMIO)
		g_astPvrRecInst[ulDeviceId].bMemIoOn = FALSE;
#endif
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_TRANSCODE_REC_Stop");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_TRANSCODE_REC_Unset(HUINT32 ulDeviceId, HUINT32 ulMuxId )
{

	HUINT32 index = 0,psiNumber = 0;
	DRV_Error drvErr = 0;

	PrintEnter("DI_TRANSCODE_REC_Unset")

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_TRANSCODE_REC_Unset] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPvrRecInst[ulDeviceId].fileRecHandle)
		{
			PrintError("[DI_TRANSCODE_REC_Unset] Invalid device status! (status : %d, fileRecHandle : 0x%x)\n\r",
				g_astPvrRecInst[ulDeviceId].status, (unsigned long)g_astPvrRecInst[ulDeviceId].fileRecHandle);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}

		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ALLOCATED)
		{
			if(g_pidChannelTranscodeVideo[ulMuxId] != NULL)
			{
				/* add multiplex data to the same record */
				NEXUS_Record_RemovePidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodeVideo[ulMuxId]);

#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
				if(g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
				{
					 NEXUS_KeySlot_RemovePidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, g_pidChannelTranscodeVideo[ulMuxId]);
				}
#endif
				g_pidChannelTranscodeVideo[ulMuxId] = NULL;
			}

			if(g_pidChannelTranscodeAudio[ulMuxId] != NULL)
			{
				NEXUS_Record_RemovePidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodeAudio[ulMuxId]);
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
				if(g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
				{
					NEXUS_KeySlot_RemovePidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, g_pidChannelTranscodeAudio[ulMuxId]);
				}
#endif
				g_pidChannelTranscodeAudio[ulMuxId] = NULL;
			}

			if(g_pidChannelTranscodeSubAudio[ulMuxId] != NULL)
			{
				NEXUS_Record_RemovePidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodeSubAudio[ulMuxId]);
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
				if(g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
				{
					NEXUS_KeySlot_RemovePidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, g_pidChannelTranscodeSubAudio[ulMuxId]);
				}
#endif
				g_pidChannelTranscodeSubAudio[ulMuxId] = NULL;
			}

			if(g_pidChannelTranscodePcr[ulMuxId] != NULL)
			{
				NEXUS_Record_RemovePidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodePcr[ulMuxId]);
				g_pidChannelTranscodePcr[ulMuxId] = NULL;
			}

			drvErr = DRV_STREAM_MUX_GetUsedPSI(ulMuxId,&psiNumber);
			if((drvErr == 0)&&(psiNumber > 0))
			{
				for(index = 0; index < psiNumber; index++)
				{
					drvErr = DRV_STREAM_MUX_GetTranscodePSI(ulMuxId,index,&(g_pidChannelTranscodePsi[ulMuxId][index]));
					if((drvErr == 0)&&(g_pidChannelTranscodePsi[ulMuxId][index] != NULL))
					{
						NEXUS_Record_RemovePidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodePsi[ulMuxId][index]);
					}
				}
			}

			drvErr = DRV_STREAM_MUX_GetUsedPES(ulMuxId,&psiNumber);
			if((drvErr == 0)&&(psiNumber > 0))
			{
				for(index = 0; index < psiNumber; index++)
				{
					drvErr = DRV_STREAM_MUX_GetTranscodePES(ulMuxId,index,&(g_pidChannelTranscodePes[ulMuxId][index]));
					if((drvErr == 0)&&(g_pidChannelTranscodePes[ulMuxId][index] != NULL))
					{
						NEXUS_Record_RemovePidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, g_pidChannelTranscodePes[ulMuxId][index]);
					}
				}
			}
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
			g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
#endif

		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_TRANSCODE_REC_Unset")

	return DI_ERR_OK;

}
#endif

DI_ERR_CODE DI_PVR_REC_Start(HUINT32 ulDeviceId )
{
	DRV_Error drvErr;

	PrintEnter("DI_PVR_REC_Start (devId=%d)", ulDeviceId);

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_Start] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ALLOCATED)
		{
#if defined(CONFIG_SATIP)
			if(g_astPvrRecInst[ulDeviceId].idxPidChHandle || g_astPvrRecInst[ulDeviceId].setup.bIsAllPidChannel) //index PID가 결정되어야 NEXUS_Record_Start를 실행할 수 있다.
#else
			if(g_astPvrRecInst[ulDeviceId].idxPidChHandle)
#endif
			{
				drvErr = P_REC_Start(ulDeviceId);
				if(drvErr != DRV_OK)
				{
					PrintError("[DI_PVR_REC_Start] Error(0x%x) in P_REC_Start()\n\r", drvErr);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
				g_astPvrRecInst[ulDeviceId].status = DI_PVR_ENABLED;
				g_astPvrRecInst[ulDeviceId].delayedTimeOffset = 0;
				g_astPvrRecInst[ulDeviceId].bFlush = FALSE;				
			}
			else
			{
				g_astPvrRecInst[ulDeviceId].requestRealStart = true; // 나중에 index PID를 add할 때 start 실행하도록 함
			}
		}
		else
		{
			PrintError("[DI_PVR_REC_Start] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_Start");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_REC_Stop(HUINT32 ulDeviceId )
{
	int err;

	PrintEnter("DI_PVR_REC_Stop (devId=%d)", ulDeviceId);

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_Stop] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
#if defined(CONFIG_DELAYED_RECORD)
		VK_SEM_Get(s_stDelayedRecInfo.semId);
		if( g_astPvrRecInst[ulDeviceId].fifoRecHandle != NULL && s_stDelayedRecInfo.fifoRecHandle != NULL
			&& g_astPvrRecInst[ulDeviceId].fifoRecHandle == s_stDelayedRecInfo.fifoRecHandle
			&& s_stDelayedRecInfo.used == true)
		{
			s_stDelayedRecInfo.used = false;
		}
		VK_SEM_Release(s_stDelayedRecInfo.semId);
#endif
		if(!(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED || g_astPvrRecInst[ulDeviceId].fileRecHandle))
		{
			PrintError("[DI_PVR_REC_Stop] Invalid device status! (status : %d, fileRecHandle : 0x%x)\n\r",
				g_astPvrRecInst[ulDeviceId].status, (unsigned long)g_astPvrRecInst[ulDeviceId].fileRecHandle);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}

		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED)
		{
			if (g_astPvrRecInst[ulDeviceId].timeCodeNotifyTimerId != VK_TIMERID_NULL)
			{
				err = VK_TIMER_Cancel(g_astPvrRecInst[ulDeviceId].timeCodeNotifyTimerId);
				if(err != VK_OK )
				{
					PrintError("[DI_PVR_REC_Stop] Error(0x%x) in VK_TIMER_Cancel\n", err);
				}
				g_astPvrRecInst[ulDeviceId].timeCodeNotifyTimerId = VK_TIMERID_NULL;
			}
			NEXUS_Record_Stop(g_astPvrRecInst[ulDeviceId].recordHandle);
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
			P_REC_ClearDscKeyslot(ulDeviceId);
#endif
		}

#if defined(REC_DYNAMIC_DMA_ALLOC)
		if(g_astPvrRecInst[ulDeviceId].dmaHandle != NULL)
		{
			NEXUS_Dma_Close(g_astPvrRecInst[ulDeviceId].dmaHandle);
			g_astPvrRecInst[ulDeviceId].dmaHandle=NULL;
		}
#endif
		NEXUS_FileRecord_Close(g_astPvrRecInst[ulDeviceId].fileRecHandle);

		g_astPvrRecInst[ulDeviceId].status = DI_PVR_ALLOCATED;
		g_astPvrRecInst[ulDeviceId].requestRealStart = false;
		g_astPvrRecInst[ulDeviceId].bEncrpytionSetted = false;
		g_astPvrRecInst[ulDeviceId].fileRecHandle = NULL;
		g_astPvrRecInst[ulDeviceId].fifoRecHandle = NULL;
#if defined(CONFIG_REC_MEMIO) || defined(CONFIG_TRANSCODER_MEMIO)
		g_astPvrRecInst[ulDeviceId].bMemIoOn = FALSE;
#endif
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_Stop");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_REC_SetDelayRecording(HUINT32 ulDeviceId, DI_PVR_REC_DELAYRECORD_SETUP_t *pstSetup)
{
#if	defined(CONFIG_DELAYED_RECORD)
	int err;
	char *srcAvFileName = NULL;
	char *srcIndexFileName = NULL;
	unsigned long orgStartTime;
	NEXUS_ChunkedDelayedRecordInfo	delayedRecordInfo;
	NEXUS_Error	neResult;
	NEXUS_RecordStatus	recStatus;

	PrintEnter("DI_PVR_REC_SetDelayRecording");

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_SetDelayRecording] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	PrintError("[DI_PVR_REC_SetDelayRecording] pstSetup->ulStartTime : %ld)\n\r", pstSetup->ulStartTime);
	orgStartTime = pstSetup->ulStartTime;
	neResult = NEXUS_Record_GetStatus(g_astPvrRecInst[ulDeviceId].recordHandle, &recStatus);
	if(neResult == NEXUS_SUCCESS)
	{
		if((recStatus.lastTimestamp/1000) >= TSR_TIME_LIMIT)
		{
			if((recStatus.lastTimestamp/1000 - pstSetup->ulStartTime) >= TSR_TIME_LIMIT)
			{
				pstSetup->ulStartTime = 1;
			}
			else
			{
				pstSetup->ulStartTime = pstSetup->ulStartTime - ((recStatus.lastTimestamp/1000) - TSR_TIME_LIMIT);
			}
		}

	}
	else
	{
		pstSetup->ulStartTime = 1;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		VK_SEM_Get(s_stDelayedRecInfo.semId);

		if((g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED) && (g_astPvrRecInst[ulDeviceId].setup.eFileType == eDI_PVR_FileType_Circular)
			&& !s_stDelayedRecInfo.used)
		{
			srcAvFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(srcAvFileName == NULL)
			{
				PrintError("[DI_PVR_REC_SetDelayRecording] Error in DD_MEM_Alloc()\n\r");
				VK_SEM_Release(s_stDelayedRecInfo.semId);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			srcIndexFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(srcIndexFileName == NULL)
			{
				PrintError("[DI_PVR_REC_SetDelayRecording] Error in DD_MEM_Alloc()\n\r");
				DD_MEM_Free(srcAvFileName);
				VK_SEM_Release(s_stDelayedRecInfo.semId);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}

			delayedRecordInfo.delayedRecDestDir = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(delayedRecordInfo.delayedRecDestDir == NULL)
			{
				PrintError("[DI_PVR_REC_SetDelayRecording] Error in DD_MEM_Alloc()\n\r");
				DD_MEM_Free(srcAvFileName);
				DD_MEM_Free(srcIndexFileName);
				VK_SEM_Release(s_stDelayedRecInfo.semId);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			delayedRecordInfo.delayedRecDestDataFile = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(delayedRecordInfo.delayedRecDestDataFile == NULL)
			{
				PrintError("[DI_PVR_REC_SetDelayRecording] Error in DD_MEM_Alloc()\n\r");
				DD_MEM_Free(srcAvFileName);
				DD_MEM_Free(srcIndexFileName);
				DD_MEM_Free(delayedRecordInfo.delayedRecDestDir);
				VK_SEM_Release(s_stDelayedRecInfo.semId);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}

			delayedRecordInfo.delayedRecDestIndexFile = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
			if(delayedRecordInfo.delayedRecDestIndexFile == NULL)
			{
				PrintError("[DI_PVR_REC_SetDelayRecording] Error in DD_MEM_Alloc()\n\r");
				DD_MEM_Free(srcAvFileName);
				DD_MEM_Free(srcIndexFileName);
				DD_MEM_Free(delayedRecordInfo.delayedRecDestDir);
				DD_MEM_Free(delayedRecordInfo.delayedRecDestDataFile);
				VK_SEM_Release(s_stDelayedRecInfo.semId);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}

			VK_memcpy((void *)srcAvFileName, (void *)(pstSetup->aucTempFileName), VK_strlen((char *)pstSetup->aucTempFileName)+1 ); //include terminating 0
#if defined(CONFIG_USE_PRIVATE_FILE_EXTENTION_HTS)
			VK_strncat(srcAvFileName, ".hts\0", 4);
#else
			VK_strncat(srcAvFileName, ".ts\0", 3); // Add  fileExtension
#endif
			VK_memcpy((void *)srcIndexFileName, (void *)(pstSetup->aucTempFileName), VK_strlen((char *)pstSetup->aucTempFileName)+1 ); //include terminating 0
			VK_strncat(srcIndexFileName, ".nts\0", 4); // Add  fileExtension
			VK_memcpy((void *)delayedRecordInfo.delayedRecDestDir, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
#if defined(CONFIG_USE_PRIVATE_FILE_EXTENTION_HTS)
			VK_strncat(delayedRecordInfo.delayedRecDestDir, ".hts\0", 4); // Add fileExtension
#else
			VK_strncat(delayedRecordInfo.delayedRecDestDir, ".ts\0", 3); // Add fileExtension
#endif
			VK_memcpy((void *)delayedRecordInfo.delayedRecDestDataFile, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
#if defined(CONFIG_USE_PRIVATE_FILE_EXTENTION_HTS)
			VK_strncat(delayedRecordInfo.delayedRecDestDataFile, ".hts/chunk\0", 10);
#else
			VK_strncat(delayedRecordInfo.delayedRecDestDataFile, ".ts/chunk\0", 9); // Add fileExtension
#endif
			VK_memcpy((void *)delayedRecordInfo.delayedRecDestIndexFile, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
			VK_strncat(delayedRecordInfo.delayedRecDestIndexFile, ".nts\0", 4); // Add fileExtension
			delayedRecordInfo.timeinsec = pstSetup->ulStartTime;

			NEXUS_Record_Switch2Linear(g_astPvrRecInst[ulDeviceId].recordHandle);
			g_astPvrRecInst[ulDeviceId].switchedfromTsrToLinear =1;
			NEXUS_ChunkedFifoRecord_StopTrimming(g_astPvrRecInst[ulDeviceId].fifoRecHandle, &delayedRecordInfo);

			DD_MEM_Free(srcAvFileName);
			DD_MEM_Free(srcIndexFileName);
			DD_MEM_Free(delayedRecordInfo.delayedRecDestDir);
			DD_MEM_Free(delayedRecordInfo.delayedRecDestDataFile);
			DD_MEM_Free(delayedRecordInfo.delayedRecDestIndexFile);

			s_stDelayedRecInfo.fifoRecHandle = g_astPvrRecInst[ulDeviceId].fifoRecHandle;
			s_stDelayedRecInfo.used = true;
			s_stDelayedRecInfo.delayRecRequestId = pstSetup->ulRequestId;
			s_stDelayedRecInfo.delayedCopyStatus = 0;
			g_astPvrRecInst[ulDeviceId].delayedTimeOffset = orgStartTime*1000; //(pstSetup->ulStartTime)*1000;

			if(s_stDelayedRecInfo.delayedCopyStatusTimerId != VK_TIMERID_NULL)
			{
				err = VK_TIMER_Cancel(s_stDelayedRecInfo.delayedCopyStatusTimerId);
				if(err != VK_OK )
				{
					PrintError("[DI_PVR_REC_SetDelayRecording] Error(0x%x) in VK_TIMER_Cancel()\n\r", err);
					VK_SEM_Release(s_stDelayedRecInfo.semId);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
			}
			err = VK_TIMER_EventAfter(DELAYED_COPY_STATUS_TIMER,
									P_REC_DelayedCopyStartTimerCallback,
									NULL,
									0,
									&(s_stDelayedRecInfo.delayedCopyStatusTimerId));
			if(err != VK_OK)
			{
				PrintError("[DI_PVR_REC_SetDelayRecording] Error(0x%x) in VK_TIMER_EventAfter()\n\r", err);
				VK_SEM_Release(s_stDelayedRecInfo.semId);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
		}
		else
		{
			PrintError("[DI_PVR_REC_SetDelayRecording] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(s_stDelayedRecInfo.semId);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(s_stDelayedRecInfo.semId);
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_SetDelayRecording");
#else
	BSTD_UNUSED(ulDeviceId);
	BSTD_UNUSED(pstSetup);
#endif

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_REC_StartDelaySplit(DI_PVR_REC_DELAYSPLIT_SETUP_t *pstSetup)
{
#if	0 //defined(CONFIG_DELAYED_RECORD)
	DRV_Error drvErr;
	int err;
	char *srcAvFileName = NULL;
	char *destAvFileName = NULL;
	char *srcIndexFileName = NULL;
	char *destIndexFileName = NULL;
	NEXUS_FileDelayedCopyPositionInfo fileDlyCopyPosInfo;
	NEXUS_FileDelayedCopyHandle fileDlyCopyHandle = NULL;
	NEXUS_FileDelayedCopyOpenSettings fileDlycopyOpenSettings;
	NEXUS_RecordDelayedCopyHandle recDlyCopyHandle = NULL;
	NEXUS_RecordDelayedCopyOpenSettings recDlyCopyOpenSettings;

	PrintEnter("DI_PVR_REC_StartDelaySplit");

	VK_SEM_Get(s_stDelayedRecInfo.semId);

	if(!s_stDelayedRecInfo.used)
	{
		srcAvFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
		if(srcAvFileName == NULL)
		{
			PrintError("[DI_PVR_REC_StartDelaySplit] Error in DD_MEM_Alloc()\n\r");
			VK_SEM_Release(s_stDelayedRecInfo.semId);
			return DI_ERR_ERROR;
		}
		srcIndexFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
		if(srcIndexFileName == NULL)
		{
			PrintError("[DI_PVR_REC_StartDelaySplit] Error in DD_MEM_Alloc()\n\r");
			DD_MEM_Free(srcAvFileName);
			VK_SEM_Release(s_stDelayedRecInfo.semId);
			return DI_ERR_ERROR;
		}
		destAvFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
		if(destAvFileName == NULL)
		{
			PrintError("[DI_PVR_REC_StartDelaySplit] Error in DD_MEM_Alloc()\n\r");
			DD_MEM_Free(srcAvFileName);
			DD_MEM_Free(srcIndexFileName);
			VK_SEM_Release(s_stDelayedRecInfo.semId);
			return DI_ERR_ERROR;
		}
		destIndexFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
		if(destIndexFileName == NULL)
		{
			PrintError("[DI_PVR_REC_StartDelaySplit] Error in DD_MEM_Alloc()\n\r");
			DD_MEM_Free(srcAvFileName);
			DD_MEM_Free(srcIndexFileName);
			DD_MEM_Free(destAvFileName);
			VK_SEM_Release(s_stDelayedRecInfo.semId);
			return DI_ERR_ERROR;
		}
		VK_memcpy((void *)srcAvFileName, (void *)(pstSetup->aucTempFileName), VK_strlen((char *)pstSetup->aucTempFileName)+1 ); //include terminating 0
		VK_strcat(srcAvFileName, ".ts\0"); // Add fileExtension
		VK_memcpy((void *)srcIndexFileName, (void *)(pstSetup->aucTempFileName), VK_strlen((char *)pstSetup->aucTempFileName)+1 ); //include terminating 0
		VK_strcat(srcIndexFileName, ".nts\0"); // Add fileExtension
		VK_memcpy((void *)destAvFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
		VK_strcat(destAvFileName, ".ts\0"); // Add fileExtension
		VK_memcpy((void *)destIndexFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
		VK_strcat(destIndexFileName, ".nts\0"); // Add fileExtension

		fileDlycopyOpenSettings.isSrcFileFifo = true;
		fileDlycopyOpenSettings.filesName.data.srcFileName = srcAvFileName;
		fileDlycopyOpenSettings.filesName.data.destFileName = destAvFileName;
		fileDlycopyOpenSettings.filesName.index.srcFileName = srcIndexFileName;
		fileDlycopyOpenSettings.filesName.index.destFileName = destIndexFileName;
		fileDlycopyOpenSettings.useTimeOffset = true;
		fileDlycopyOpenSettings.dataCutAllign =DATA_ALIGN;
		fileDlycopyOpenSettings.copyStartTimeOffset = (pstSetup->ulStartTime)*1000; /* sec -> ms */
		fileDlycopyOpenSettings.copyEndTimeOffset = (pstSetup->ulEndTime)*1000; /* sec -> ms */

		fileDlyCopyHandle = NEXUS_FileRecord_OpenDelayedCopy(&fileDlycopyOpenSettings);
		if(!fileDlyCopyHandle)
		{
			PrintError("[DI_PVR_REC_StartDelaySplit] Error in NEXUS_FileRecord_OpenDelayedCopy()\n\r");
			DD_MEM_Free(srcAvFileName);
			DD_MEM_Free(srcIndexFileName);
			DD_MEM_Free(destAvFileName);
			DD_MEM_Free(destIndexFileName);
			VK_SEM_Release(s_stDelayedRecInfo.semId);
			return DI_ERR_ERROR;
		}
		DD_MEM_Free(srcAvFileName);
		DD_MEM_Free(srcIndexFileName);
		DD_MEM_Free(destAvFileName);
		DD_MEM_Free(destIndexFileName);

		if((pstSetup->stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_CURRENT)
			&& (pstSetup->stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE))
		{
			drvErr = P_REC_LoadEncryptionKey(s_stDelayedRecInfo.encKeyHandle, &(pstSetup->stEncryptSettings));
			if (drvErr != DRV_OK)
			{
				PrintError("[DI_PVR_REC_StartDelaySplit] Error(0x%x) in P_REC_LoadEncryptionKey()\n\r", drvErr);
				VK_SEM_Release(s_stDelayedRecInfo.semId);
				return DI_ERR_ERROR;
			}
			recDlyCopyOpenSettings.securityDma = g_ulDmaHandle;
			recDlyCopyOpenSettings.securityContext = s_stDelayedRecInfo.encKeyHandle;
		}
		else
		{
			recDlyCopyOpenSettings.securityDma = NULL;
			recDlyCopyOpenSettings.securityContext = NULL;
		}
		recDlyCopyOpenSettings.bufferSize = DELAYED_COPY_BUFFER_SIZE;
		recDlyCopyOpenSettings.alignment = 12; /* 4K alignment */
		recDlyCopyOpenSettings.error.callback = P_REC_DelayedCopyErrorCallBack;
		recDlyCopyOpenSettings.copyEnd.callback = P_REC_DelayedCopyEndCallBack;

		recDlyCopyHandle = NEXUS_Record_StartDelayedCopy(&recDlyCopyOpenSettings, fileDlyCopyHandle);
		if(!recDlyCopyHandle)
		{
			PrintError("[DI_PVR_REC_StartDelaySplit] Error in NEXUS_Record_StartDelayedCopy()\n\r");
			VK_SEM_Release(s_stDelayedRecInfo.semId);
			return DI_ERR_ERROR;
		}
		s_stDelayedRecInfo.used = true;
		s_stDelayedRecInfo.delayRecRequestId = pstSetup->ulRequestId;
		s_stDelayedRecInfo.delayedCopyStatus = 0;
		s_stDelayedRecInfo.recDelayedCopyHandle = recDlyCopyHandle;

		if(s_stDelayedRecInfo.delayedCopyStatusTimerId != VK_TIMERID_NULL)
		{
			err = VK_TIMER_Cancel(s_stDelayedRecInfo.delayedCopyStatusTimerId);
			if(err != VK_OK )
			{
				PrintError("[DI_PVR_REC_StartDelaySplit] Error(0x%x) in VK_TIMER_Cancel()\n\r", err);
				VK_SEM_Release(s_stDelayedRecInfo.semId);
				return DI_ERR_ERROR;
			}
		}
		err = VK_TIMER_EventAfter(DELAYED_COPY_STATUS_TIMER,
								P_REC_DelayedCopyStatusTimerCallback,
								NULL,
								0,
								&(s_stDelayedRecInfo.delayedCopyStatusTimerId));
		if(err != VK_OK)
		{
			PrintError("[DI_PVR_REC_StartDelaySplit] Error(0x%x) in VK_TIMER_EventAfter()\n\r", err);
			VK_SEM_Release(s_stDelayedRecInfo.semId);
			return DI_ERR_ERROR;
		}
	}
	else
	{
		PrintError("[DI_PVR_REC_StartDelaySplit] Invalid status! (used : %d)\n\r", s_stDelayedRecInfo.used);
		VK_SEM_Release(s_stDelayedRecInfo.semId);
		return DI_ERR_ERROR;
	}
	VK_SEM_Release(s_stDelayedRecInfo.semId);

	PrintExit("DI_PVR_REC_StartDelaySplit");
#else
	BSTD_UNUSED(pstSetup);
#endif

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_REC_GetDelayRecordingStatus(HUINT32 ulRequestId, HUINT32 *pulStatus)
{
#if	0 //defined(CONFIG_DELAYED_RECORD)
	PrintEnter("DI_PVR_REC_GetDelayRecordingStatus");

	VK_SEM_Get(s_stDelayedRecInfo.semId);

	if(s_stDelayedRecInfo.delayRecRequestId == ulRequestId)
	{
		*pulStatus = (HUINT32)s_stDelayedRecInfo.delayedCopyStatus;
	}
	else
	{
		PrintError("[DI_PVR_REC_GetDelayRecordingStatus] Invalid request ID! (ulRequestId : %d)\n\r", ulRequestId);
		VK_SEM_Release(s_stDelayedRecInfo.semId);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_stDelayedRecInfo.semId);

	PrintExit("DI_PVR_REC_GetDelayRecordingStatus");
#else
	BSTD_UNUSED(ulRequestId);
	BSTD_UNUSED(pulStatus);
#endif

	return DI_ERR_OK;
}


DI_ERR_CODE DI_PVR_REC_AddPidChannel(HUINT32 ulDeviceId, HUINT16 usPid, DI_PVR_REC_PID_PES_TYPE ePesType, HUINT32 *pulRequestId)
{
	NEXUS_Error	neResult;
	DRV_Error drvErr;
	NEXUS_PidChannelHandle recPidChannelHandle;
	NEXUS_RecordPidChannelSettings recPidChannelSettings;
	HBOOL bFullTs;
#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD) && defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
	NEXUS_RecpumpSettings recPumpSettings;
#else
#if (BCHP_CHIP == 7425 || BCHP_CHIP == 7356 || BCHP_CHIP == 7346 || BCHP_CHIP == 7358 || BCHP_CHIP == 7445)
	NEXUS_RecpumpSettings recPumpSettings;
#endif
#endif
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	NEXUS_KeySlotHandle keyHandleFromDSC;
#endif
#endif
	unsigned long i;
	unsigned long recPidChannelIndex = MAX_REC_PID_NUM;

	PrintEnter("DI_PVR_REC_AddPidChannel (devId=%d, pesType=%d)", ulDeviceId, ePesType);

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_AddPidChannel] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(ePesType == DI_PVR_REC_PID_PES_TYPE_ALL)
	{
		bFullTs = TRUE;
	}
	else
	{
		bFullTs = FALSE;
	}

	drvErr = DRV_DEMUX_StartRecordPID(ulDeviceId, usPid, pulRequestId, bFullTs);
	if(drvErr != DRV_OK)
	{
		PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in DRV_DEMUX_StartRecordPID()\n\r", drvErr);
		return DI_ERR_ERROR;
	}

	recPidChannelHandle = DRV_DEMUX_GetRecordPidChannel(*pulRequestId);
	if(recPidChannelHandle == NULL)
	{
		PrintError("[DI_PVR_REC_AddPidChannel] Invalid PID channel! (ulPidChannel : %d)\n\r", *pulRequestId);
		goto exit_after_release_resource;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(VK_SEM_Get(g_ulSemgRecPid) != VK_OK)
		{
			PrintError("[DI_PVR_REC_AddPidChannel] VK_SEM_GET(g_ulSemRecPid) ERROR\n\r");
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			goto exit_after_release_resource;
		}

		if(g_astPvrRecInst[ulDeviceId].status != DI_PVR_FREE)
		{
			for(i=0; i<MAX_REC_PID_NUM; i++)
			{
				if(g_astRecPidChInfo[i].refCnt == 0 && recPidChannelIndex == MAX_REC_PID_NUM)
				{
					recPidChannelIndex = i;
				}
				else if(g_astRecPidChInfo[i].refCnt && (g_astRecPidChInfo[i].recPidChHandle == recPidChannelHandle))
				{
					recPidChannelIndex = i;
					break;
				}
			}
			if(recPidChannelIndex == MAX_REC_PID_NUM)
			{
				PrintError("[DI_PVR_REC_AddPidChannel] No record PID channel info slot!\n\r");
				VK_SEM_Release(g_ulSemgRecPid);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				goto exit_after_release_resource;
			}

			if(g_astPvrRecInst[ulDeviceId].setup.eStreamType == eDI_PVR_Stream_Video && ePesType == DI_PVR_REC_PID_PES_TYPE_VIDEO) // record setup이 보장되어야 함
			{
				if(g_astPvrRecInst[ulDeviceId].idxPidChHandle != NULL)
				{
					PrintError("[DI_PVR_REC_AddPidChannel] Index PID channel handle(0x%x) is already exist!\n\r", g_astPvrRecInst[ulDeviceId].idxPidChHandle);
					VK_SEM_Release(g_ulSemgRecPid);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					goto exit_after_release_resource;
				}

				/* ex) PCR PID == video PID 인데 PCR PID로 먼저 add된 경우 다시 remove -> add 필요 */
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
				if(g_astRecPidChInfo[recPidChannelIndex].refCntInDevice[ulDeviceId])
#else
				if(g_astRecPidChInfo[recPidChannelIndex].refCnt)
#endif
				{
					neResult = NEXUS_Record_RemovePidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, recPidChannelHandle);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_Record_RemovePidChannel()\n\r", neResult);
						VK_SEM_Release(g_ulSemgRecPid);
						VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
						goto exit_after_release_resource;
					}
				}
				NEXUS_Record_GetDefaultPidChannelSettings(&recPidChannelSettings);
				recPidChannelSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
				recPidChannelSettings.recpumpSettings.pidTypeSettings.video.codec = DRV_VIDEO_GetCodecType(0, g_astPvrRecInst[ulDeviceId].setup.eVideoType);
#if defined(CONFIG_REC_MEMIO) || defined(CONFIG_TRANSCODER_MEMIO)
				if(g_astPvrRecInst[ulDeviceId].bMemIoOn == TRUE)
				{
					recPidChannelSettings.recpumpSettings.pidTypeSettings.video.index = false;
				}
				else
				{
					recPidChannelSettings.recpumpSettings.pidTypeSettings.video.index = true;
				}
#else
				recPidChannelSettings.recpumpSettings.pidTypeSettings.video.index = true;
#endif
				recPidChannelSettings.recpumpSettings.pidTypeSettings.video.pid = usPid;
				neResult = NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, recPidChannelHandle, &recPidChannelSettings);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", neResult);
					VK_SEM_Release(g_ulSemgRecPid);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					goto exit_after_release_resource;
				}
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
				if(g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
				{
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
					drvErr = DRV_DSC_OpenKeySlotForRecord(*pulRequestId, &keyHandleFromDSC);
					if ((drvErr == DRV_OK) && (keyHandleFromDSC != NULL))
#else
					keyHandleFromDSC = P_REC_GetKeySlotHandleFromDSC(*pulRequestId);
					if(keyHandleFromDSC != NULL)
#endif
					{
						PrintDebug("[DI_PVR_REC_AddPidChannel] Using DSC keyhandle(0x%x)\n\r", keyHandleFromDSC);
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
						drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, keyHandleFromDSC, &g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings, (g_astPvrRecInst[ulDeviceId].setup.ulPacketSize!=188?TRUE:FALSE));
#else
						drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, keyHandleFromDSC, &g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings);
#endif
						if (drvErr != DRV_OK)
						{
							PrintError("[DI_PVR_REC_AddPidChannel] Error in P_REC_SetEncryption()\n\r");
							VK_SEM_Release(g_ulSemgRecPid);
							VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
							goto exit_after_release_resource;
						}
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
						drvErr = P_REC_AddDscKeyslot(ulDeviceId, keyHandleFromDSC, recPidChannelHandle);
						if (drvErr != DRV_OK)
						{
							PrintError("[%s] Error, P_REC_AddDscKeyslot(%d, , )\n\r", __FUNCTION__, ulDeviceId);
							VK_SEM_Release(g_ulSemgRecPid);
							VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
							goto exit_after_release_resource;
						}
#endif
					}
					else
					{
						PrintDebug("[DI_PVR_REC_AddPidChannel] Using REC keyhandle(0x%x)\n\r", g_astPvrRecInst[ulDeviceId].encKeyHandle);
						neResult = NEXUS_KeySlot_AddPidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, recPidChannelHandle);
						if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_KeySlot_AddPidChannel()\n\r", neResult);
							VK_SEM_Release(g_ulSemgRecPid);
							VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
							goto exit_after_release_resource;
						}
					}
				}
#endif
#endif
				drvErr = DRV_DEMUX_StartRecordTSC(*pulRequestId);
				if(drvErr != DRV_OK)
				{
					PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in DRV_DEMUX_StartRecordTSC()\n\r", drvErr);
					VK_SEM_Release(g_ulSemgRecPid);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					goto exit_after_release_resource;
				}

				if(g_astPvrRecInst[ulDeviceId].requestRealStart)
				{
					drvErr = P_REC_Start(ulDeviceId);
					if(drvErr != DRV_OK)
					{
						PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in P_REC_Start()\n\r", drvErr);
						VK_SEM_Release(g_ulSemgRecPid);
						VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
						goto exit_after_release_resource;
					}
					g_astPvrRecInst[ulDeviceId].status = DI_PVR_ENABLED;
					g_astPvrRecInst[ulDeviceId].requestRealStart = false;
					g_astPvrRecInst[ulDeviceId].delayedTimeOffset = 0;
				}
				g_astPvrRecInst[ulDeviceId].idxPidChHandle = recPidChannelHandle;
				g_astRecPidChInfo[recPidChannelIndex].recPidChHandle = recPidChannelHandle;
				g_astRecPidChInfo[recPidChannelIndex].refCnt++;
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
				g_astRecPidChInfo[recPidChannelIndex].refCntInDevice[ulDeviceId]++;
#endif
			}
			else if(g_astPvrRecInst[ulDeviceId].setup.eStreamType == eDI_PVR_Stream_Audio && ePesType == DI_PVR_REC_PID_PES_TYPE_AUDIO)
			{
				if(g_astPvrRecInst[ulDeviceId].idxPidChHandle == NULL)
				{
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
					if(g_astRecPidChInfo[recPidChannelIndex].refCntInDevice[ulDeviceId])
#else
					if(g_astRecPidChInfo[recPidChannelIndex].refCnt)
#endif
					{
						neResult = NEXUS_Record_RemovePidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, recPidChannelHandle);
						if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_Record_RemovePidChannel()\n\r", neResult);
							VK_SEM_Release(g_ulSemgRecPid);
							VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
							goto exit_after_release_resource;
						}
					}
					NEXUS_Record_GetDefaultPidChannelSettings(&recPidChannelSettings);
					recPidChannelSettings.recpumpSettings.pidType = NEXUS_PidType_eAudio; //NEXUS_PidType_eOther;
#if defined(CONFIG_REC_MEMIO) || defined(CONFIG_TRANSCODER_MEMIO)
					if(g_astPvrRecInst[ulDeviceId].bMemIoOn == TRUE)
					{
						recPidChannelSettings.recpumpSettings.pidTypeSettings.other.index = false;
					}
					else
					{
						recPidChannelSettings.recpumpSettings.pidTypeSettings.other.index = true;
					}
#else
					recPidChannelSettings.recpumpSettings.pidTypeSettings.other.index = true;
#endif
					neResult = NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, recPidChannelHandle, &recPidChannelSettings);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", neResult);
						VK_SEM_Release(g_ulSemgRecPid);
						VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
						goto exit_after_release_resource;
					}
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
					if(g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
					{
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
						drvErr = DRV_DSC_OpenKeySlotForRecord(*pulRequestId, &keyHandleFromDSC);
						if ((drvErr == DRV_OK) && (keyHandleFromDSC != NULL))
#else
						keyHandleFromDSC = P_REC_GetKeySlotHandleFromDSC(*pulRequestId);
						if(keyHandleFromDSC != NULL)
#endif
						{
							PrintDebug("[DI_PVR_REC_AddPidChannel] Using DSC keyhandle(0x%x)\n\r", keyHandleFromDSC);
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
							drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, keyHandleFromDSC, &g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings, (g_astPvrRecInst[ulDeviceId].setup.ulPacketSize!=188?TRUE:FALSE));
#else
							drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, keyHandleFromDSC, &g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings);
#endif
							if (drvErr != DRV_OK)
							{
								PrintError("[DI_PVR_REC_AddPidChannel] Error in P_REC_SetEncryption()\n\r");
								VK_SEM_Release(g_ulSemgRecPid);
								VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
								goto exit_after_release_resource;
							}
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
							drvErr = P_REC_AddDscKeyslot(ulDeviceId, keyHandleFromDSC, recPidChannelHandle);
							if (drvErr != DRV_OK)
							{
								PrintError("[%s] Error, P_REC_AddDscKeyslot(%d, , )\n\r", __FUNCTION__, ulDeviceId);
								VK_SEM_Release(g_ulSemgRecPid);
								VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
								goto exit_after_release_resource;
							}
#endif
						}
						else
						{
							PrintDebug("[DI_PVR_REC_AddPidChannel] Using REC keyhandle(0x%x)\n\r", g_astPvrRecInst[ulDeviceId].encKeyHandle);
							neResult = NEXUS_KeySlot_AddPidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, recPidChannelHandle);
							if(neResult != NEXUS_SUCCESS)
							{
								PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_KeySlot_AddPidChannel()\n\r", neResult);
								VK_SEM_Release(g_ulSemgRecPid);
								VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
								goto exit_after_release_resource;
							}
						}
					}
#endif
#endif
					drvErr = DRV_DEMUX_StartRecordTSC(*pulRequestId);
					if(drvErr != DRV_OK)
					{
						PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in DRV_DEMUX_StartRecordTSC()\n\r", drvErr);
						VK_SEM_Release(g_ulSemgRecPid);
						VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
						goto exit_after_release_resource;
					}

					if(g_astPvrRecInst[ulDeviceId].requestRealStart)
					{
						drvErr = P_REC_Start(ulDeviceId);
						if(drvErr != DRV_OK)
						{
							PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in P_REC_Start()\n\r", drvErr);
							VK_SEM_Release(g_ulSemgRecPid);
							VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
							goto exit_after_release_resource;
						}
						g_astPvrRecInst[ulDeviceId].status = DI_PVR_ENABLED;
						g_astPvrRecInst[ulDeviceId].requestRealStart = false;
						g_astPvrRecInst[ulDeviceId].delayedTimeOffset = 0;
					}
					g_astPvrRecInst[ulDeviceId].idxPidChHandle = recPidChannelHandle;
				}
				else
				{
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
					if(g_astRecPidChInfo[recPidChannelIndex].refCntInDevice[ulDeviceId] == 0)
#else
					if(g_astRecPidChInfo[recPidChannelIndex].refCnt == 0)
#endif
					{
						neResult = NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, recPidChannelHandle, NULL);
						if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", neResult);
							VK_SEM_Release(g_ulSemgRecPid);
							VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
							goto exit_after_release_resource;
						}
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
						if(g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
						{
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
							drvErr = DRV_DSC_OpenKeySlotForRecord(*pulRequestId, &keyHandleFromDSC);
							if ((drvErr == DRV_OK) && (keyHandleFromDSC != NULL))
#else
							keyHandleFromDSC = P_REC_GetKeySlotHandleFromDSC(*pulRequestId);
							if(keyHandleFromDSC != NULL)
#endif
							{
								PrintDebug("[DI_PVR_REC_AddPidChannel] Using DSC keyhandle(0x%x)\n\r", keyHandleFromDSC);
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
								drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, keyHandleFromDSC, &g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings, (g_astPvrRecInst[ulDeviceId].setup.ulPacketSize!=188?TRUE:FALSE));
#else
								drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, keyHandleFromDSC, &g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings);
#endif
								if (drvErr != DRV_OK)
								{
									PrintError("[DI_PVR_REC_AddPidChannel] Error in P_REC_SetEncryption()\n\r");
									VK_SEM_Release(g_ulSemgRecPid);
									VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
									goto exit_after_release_resource;
								}
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
								drvErr = P_REC_AddDscKeyslot(ulDeviceId, keyHandleFromDSC, recPidChannelHandle);
								if (drvErr != DRV_OK)
								{
									PrintError("[%s] Error, P_REC_AddDscKeyslot(%d, , )\n\r", __FUNCTION__, ulDeviceId);
									VK_SEM_Release(g_ulSemgRecPid);
									VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
									goto exit_after_release_resource;
								}
#endif
							}
							else
							{
								PrintDebug("[DI_PVR_REC_AddPidChannel] Using REC keyhandle(0x%x)\n\r", g_astPvrRecInst[ulDeviceId].encKeyHandle);
								neResult = NEXUS_KeySlot_AddPidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, recPidChannelHandle);
								if(neResult != NEXUS_SUCCESS)
								{
									PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_KeySlot_AddPidChannel()\n\r", neResult);
									VK_SEM_Release(g_ulSemgRecPid);
									VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
									goto exit_after_release_resource;
								}
							}
						}
#endif
#endif
					}
				}
				g_astRecPidChInfo[recPidChannelIndex].recPidChHandle = recPidChannelHandle;
				g_astRecPidChInfo[recPidChannelIndex].refCnt++;
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
				g_astRecPidChInfo[recPidChannelIndex].refCntInDevice[ulDeviceId]++;
#endif
			}
			else
			{
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
				if(g_astRecPidChInfo[recPidChannelIndex].refCntInDevice[ulDeviceId] == 0)
#else
				if(g_astRecPidChInfo[recPidChannelIndex].refCnt == 0)
#endif
				{
					if(ePesType == DI_PVR_REC_PID_PES_TYPE_ALL)
					{
						NEXUS_Record_GetDefaultPidChannelSettings(&recPidChannelSettings);
						recPidChannelSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
						recPidChannelSettings.recpumpSettings.pidTypeSettings.video.codec = DRV_VIDEO_GetCodecType(0, g_astPvrRecInst[ulDeviceId].setup.eVideoType);
						recPidChannelSettings.recpumpSettings.pidTypeSettings.video.index = true;
						recPidChannelSettings.recpumpSettings.pidTypeSettings.video.pid = 0x00;
						neResult = NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, recPidChannelHandle,  &recPidChannelSettings);
					}
					else
					{
						neResult = NEXUS_Record_AddPidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, recPidChannelHandle, NULL);
					}

					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", neResult);
						VK_SEM_Release(g_ulSemgRecPid);
						VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
						goto exit_after_release_resource;
				    }
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
					if(((ePesType == DI_PVR_REC_PID_PES_TYPE_AUDIO))&&(g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE))
					{
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
						drvErr = DRV_DSC_OpenKeySlotForRecord(*pulRequestId, &keyHandleFromDSC);
						if ((drvErr == DRV_OK) && (keyHandleFromDSC != NULL))
#else
						keyHandleFromDSC = P_REC_GetKeySlotHandleFromDSC(*pulRequestId);
						if(keyHandleFromDSC != NULL)
#endif
						{
							PrintDebug("[DI_PVR_REC_AddPidChannel] Using DSC keyhandle(0x%x)\n\r", keyHandleFromDSC);
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
							drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, keyHandleFromDSC, &g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings, (g_astPvrRecInst[ulDeviceId].setup.ulPacketSize!=188?TRUE:FALSE));
#else
							drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, keyHandleFromDSC, &g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings);
#endif
							if (drvErr != DRV_OK)
							{
								PrintError("[DI_PVR_REC_AddPidChannel] Error in P_REC_SetEncryption()\n\r");
								VK_SEM_Release(g_ulSemgRecPid);
								VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
								goto exit_after_release_resource;
							}
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
							drvErr = P_REC_AddDscKeyslot(ulDeviceId, keyHandleFromDSC, recPidChannelHandle);
							if (drvErr != DRV_OK)
							{
								PrintError("[%s] Error, P_REC_AddDscKeyslot(%d, , )\n\r", __FUNCTION__, ulDeviceId);
								VK_SEM_Release(g_ulSemgRecPid);
								VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
								goto exit_after_release_resource;
							}
#endif
						}
						else
						{
							PrintDebug("[DI_PVR_REC_AddPidChannel] Using REC keyhandle(0x%x)\n\r", g_astPvrRecInst[ulDeviceId].encKeyHandle);
							neResult = NEXUS_KeySlot_AddPidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, recPidChannelHandle);
							if(neResult != NEXUS_SUCCESS)
							{
								PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_KeySlot_AddPidChannel()\n\r", neResult);
								VK_SEM_Release(g_ulSemgRecPid);
								VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
								goto exit_after_release_resource;
							}
						}
					}
#endif
#endif
					if(ePesType == DI_PVR_REC_PID_PES_TYPE_ALL)
					{
						if(g_astPvrRecInst[ulDeviceId].requestRealStart)
						{
							drvErr = P_REC_Start(ulDeviceId);
							if(drvErr != DRV_OK)
							{
								PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in P_REC_Start()\n\r", drvErr);
								VK_SEM_Release(g_ulSemgRecPid);
								VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
								return DI_ERR_ERROR;
							}
							g_astPvrRecInst[ulDeviceId].status = DI_PVR_ENABLED;
							g_astPvrRecInst[ulDeviceId].requestRealStart = false;
							g_astPvrRecInst[ulDeviceId].delayedTimeOffset = 0;
						}
						g_astPvrRecInst[ulDeviceId].idxPidChHandle = recPidChannelHandle;
						g_astPvrRecInst[ulDeviceId].bTsRec = TRUE;
					}
				}

#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD) && defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
				if(ePesType == DI_PVR_REC_PID_PES_TYPE_PCR)
				{
					NEXUS_Recpump_GetSettings(g_astPvrRecInst[ulDeviceId].recPumpHandle, &recPumpSettings);
					recPumpSettings.adjustTimestampUsingPcrs = TRUE;
					recPumpSettings.pcrPidChannel = recPidChannelHandle;
					neResult = NEXUS_Recpump_SetSettings(g_astPvrRecInst[ulDeviceId].recPumpHandle, &recPumpSettings);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_Recpump_SetSettings()\n\r", neResult);
						VK_SEM_Release(g_ulSemgRecPid);
						VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
						goto exit_after_release_resource;
					}
				}
#else
#if (BCHP_CHIP == 7425 || BCHP_CHIP == 7356 || BCHP_CHIP == 7346 || BCHP_CHIP == 7358 || BCHP_CHIP == 7445)
				if(ePesType == DI_PVR_REC_PID_PES_TYPE_PCR)
				{
					NEXUS_Recpump_GetSettings(g_astPvrRecInst[ulDeviceId].recPumpHandle, &recPumpSettings);
					recPumpSettings.adjustTimestampUsingPcrs = FALSE;
					recPumpSettings.pcrPidChannel = recPidChannelHandle;
					neResult = NEXUS_Recpump_SetSettings(g_astPvrRecInst[ulDeviceId].recPumpHandle, &recPumpSettings);
					if(neResult != NEXUS_SUCCESS)
					{
						PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in NEXUS_Recpump_SetSettings()\n\r", neResult);
						VK_SEM_Release(g_ulSemgRecPid);
						VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
						goto exit_after_release_resource;
					}
				}
#endif
#endif
				g_astRecPidChInfo[recPidChannelIndex].recPidChHandle = recPidChannelHandle;
				g_astRecPidChInfo[recPidChannelIndex].refCnt++;
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
				g_astRecPidChInfo[recPidChannelIndex].refCntInDevice[ulDeviceId]++;
#endif
			}
		}
		else
		{
			PrintError("[DI_PVR_REC_AddPidChannel] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_ulSemgRecPid);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			goto exit_after_release_resource;
		}
		VK_SEM_Release(g_ulSemgRecPid);
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_AddPidChannel");

	return DI_ERR_OK;

exit_after_release_resource:

	drvErr = DRV_DEMUX_StopRecordPID(*pulRequestId, g_astPvrRecInst[ulDeviceId].bTsRec);
	if(drvErr != DRV_OK)
	{
		PrintError("[DI_PVR_REC_AddPidChannel] Error(0x%x) in DRV_DEMUX_StartRecordPID()\n\r", drvErr);
	}

	return DI_ERR_ERROR;
}

DI_ERR_CODE DI_PVR_REC_RemovePidChannel(HUINT32 ulDeviceId, HUINT32 ulRequestId)
{
	NEXUS_Error	neResult;
	DRV_Error drvErr;
	NEXUS_PidChannelHandle recPidChannelHandle;
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	NEXUS_KeySlotHandle keyHandleFromDSC;
#endif
#endif
	unsigned long i;
	unsigned long recPidChannelIndex = MAX_REC_PID_NUM;

	PrintEnter("DI_PVR_REC_RemovePidChannel (devId=%d)", ulDeviceId);

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_RemovePidChannel] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	recPidChannelHandle = DRV_DEMUX_GetRecordPidChannel(ulRequestId);
	if(recPidChannelHandle == NULL)
	{
		PrintError("[DI_PVR_REC_RemovePidChannel] Invalid PID channel! (ulPidChannel : %d)\n\r", ulRequestId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{		if(VK_SEM_Get(g_ulSemgRecPid) != VK_OK)
		{
			PrintError("[DI_PVR_REC_RemovePidChannel] VK_SEM_GET(g_ulSemRecPid) ERROR\n\r");
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		if(g_astPvrRecInst[ulDeviceId].status != DI_PVR_FREE)
		{
			for(i=0; i<MAX_REC_PID_NUM; i++)
			{
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
				if(g_astRecPidChInfo[i].refCntInDevice[ulDeviceId] && (g_astRecPidChInfo[i].recPidChHandle == recPidChannelHandle))
#else
				if(g_astRecPidChInfo[i].refCnt && (g_astRecPidChInfo[i].recPidChHandle == recPidChannelHandle))
#endif
				{
					recPidChannelIndex = i;
					break;
				}
			}
			if(recPidChannelIndex == MAX_REC_PID_NUM)
			{
				PrintError("[DI_PVR_REC_RemovePidChannel] No record PID channel to remove!\n\r");
				VK_SEM_Release(g_ulSemgRecPid);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
			if(g_astRecPidChInfo[recPidChannelIndex].refCntInDevice[ulDeviceId] == 1)
#else
			if(g_astRecPidChInfo[recPidChannelIndex].refCnt == 1)
#endif
			{
				neResult = NEXUS_Record_RemovePidChannel(g_astPvrRecInst[ulDeviceId].recordHandle, recPidChannelHandle);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[DI_PVR_REC_RemovePidChannel] Error(0x%x) in NEXUS_Record_RemovePidChannel()\n\r", neResult);
					VK_SEM_Release(g_ulSemgRecPid);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
			    }

#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
				if(g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
				{
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
					drvErr = DRV_DSC_CloseKeySlotForRecord(ulRequestId, &keyHandleFromDSC);
					if ((drvErr == DRV_OK) || (keyHandleFromDSC != NULL))
#else
					keyHandleFromDSC = P_REC_GetKeySlotHandleFromDSC(ulRequestId);
					if(keyHandleFromDSC == NULL)
#endif
					{
						PrintDebug("[DI_PVR_REC_RemovePidChannel] REMOVE PID channel(%d) from REC keyslot\n\r", ulRequestId);
						NEXUS_KeySlot_RemovePidChannel(g_astPvrRecInst[ulDeviceId].encKeyHandle, recPidChannelHandle);
					}
					else
					{
						PrintDebug("[DI_PVR_REC_RemovePidChannel] REMOVE PID channel(%d) from DSC keyslot\n\r", ulRequestId);
					}
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
					drvErr = P_REC_RemoveDscKeyslot(ulDeviceId, recPidChannelHandle);
					if (drvErr != DRV_OK)
					{
						PrintError("[%s] Error, P_REC_RemoveDscKeyslot(%d, )\n\r", __FUNCTION__, ulRequestId);
					}
#endif
				}
#endif
#endif
				if(g_astPvrRecInst[ulDeviceId].idxPidChHandle == recPidChannelHandle)
				{
					g_astPvrRecInst[ulDeviceId].idxPidChHandle = NULL;
				}
			}
			g_astRecPidChInfo[recPidChannelIndex].refCnt--;
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
			g_astRecPidChInfo[recPidChannelIndex].refCntInDevice[ulDeviceId]--;
#endif
		}
		else
		{
			PrintError("[DI_PVR_REC_RemovePidChannel] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_ulSemgRecPid);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_ulSemgRecPid);
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	drvErr = DRV_DEMUX_StopRecordPID(ulRequestId, g_astPvrRecInst[ulDeviceId].bTsRec);
	if(drvErr != DRV_OK)
	{
		PrintError("[DI_PVR_REC_RemovePidChannel] Error(0x%x) in DRV_DEMUX_StartRecordPID()\n\r", drvErr);
		return DI_ERR_ERROR;
	}

	g_astPvrRecInst[ulDeviceId].bTsRec = FALSE;

	PrintExit("DI_PVR_REC_RemovePidChannel");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_REC_SetEncryption(HUINT32 ulDeviceId, DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings)
{
	DRV_Error drvErr;
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO) && defined(CONFIG_CAS_CX)
	HUINT32 i;
#endif
	PrintEnter("DI_PVR_REC_SetEncryption");

	if(pstEncryptSettings == NULL)
	{
		PrintError("[DI_PVR_REC_SetEncryption] pstEncryptSettings == NULL\n\r");
		return DI_ERR_INVALID_PARAM;
	}

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_SetEncryption] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ALLOCATED)
		{
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
			drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].encKeyHandle, pstEncryptSettings, (g_astPvrRecInst[ulDeviceId].setup.ulPacketSize!=188?TRUE:FALSE));
#else
			drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].encKeyHandle, pstEncryptSettings);
#endif
			if (drvErr != DRV_OK)
			{
				PrintError("[DI_PVR_REC_SetEncryption] Error(0x%x) in P_REC_SetEncryption()\n\r", drvErr);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
		    }
			g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings = *pstEncryptSettings;
			g_astPvrRecInst[ulDeviceId].bEncrpytionSetted		= true;
		}
		else if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED && g_astPvrRecInst[ulDeviceId].bEncrpytionSetted == true)
		{
			if (VK_memcmp(g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings.ucKey, pstEncryptSettings->ucKey, DI_PVR_CRYPTO_KEY_LEN) != 0)
			{
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
				/* find alloced dsckeyslot*/
				for(i=0; i<P_REC_MAX_PID_PER_REC_INT; i++)
				{
					if(g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].dscKeyHandle != NULL)
					{
						PrintError("[%s]Set new key at keyslot: %x\n", __FUNCTION__, (HUINT32)g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].dscKeyHandle);
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
						drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].dscKeyHandle, pstEncryptSettings, (g_astPvrRecInst[ulDeviceId].setup.ulPacketSize!=188?TRUE:FALSE));
#else
						drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].dscKeyHandle, pstEncryptSettings);
#endif
						if (drvErr != DRV_OK)
						{
							PrintError("[DI_PVR_REC_SetEncryption] Error(0x%x) in P_REC_SetEncryption()\n\r", drvErr);
							VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
							return DI_ERR_ERROR;
						}
						g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings = *pstEncryptSettings;
						g_astPvrRecInst[ulDeviceId].bEncrpytionSetted		= true;

					}
				}
#else
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
				drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].encKeyHandle, pstEncryptSettings, (g_astPvrRecInst[ulDeviceId].setup.ulPacketSize!=188?TRUE:FALSE));
#else
				drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].encKeyHandle, pstEncryptSettings);
#endif
				if (drvErr != DRV_OK)
				{
					PrintError("[DI_PVR_REC_SetEncryption] Error(0x%x) in P_REC_SetEncryption()\n\r", drvErr);
					VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
					return DI_ERR_ERROR;
				}
				g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings = *pstEncryptSettings;
				g_astPvrRecInst[ulDeviceId].bEncrpytionSetted		= true;
#endif
			}
			else
			{
				PrintError("[DI_PVR_REC_SetEncryption] same key already setted \n\r");
			}
		}
		else
		{
			PrintError("[DI_PVR_REC_SetEncryption] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_SetEncryption");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_REC_Pause(HUINT32 ulDeviceId )
{
//	NEXUS_Error	neResult;
//	NEXUS_RecordSettings	recordCfg;

	PrintEnter("DI_PVR_REC_Pause");

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_Pause] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status != DI_PVR_FREE && g_astPvrRecInst[ulDeviceId].recordSuspend == false)
		{
			NEXUS_Error neResult;
			NEXUS_RecordSettings	recordCfg;

			NEXUS_Record_GetSettings(g_astPvrRecInst[ulDeviceId].recordHandle, &recordCfg);
#if (NEXUS_VERSION <= 900)
#else
			recordCfg.recpumpSettings.recpumpSuspend = true;
#endif
			neResult = NEXUS_Record_SetSettings(g_astPvrRecInst[ulDeviceId].recordHandle, &recordCfg);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[DI_PVR_REC_Pause] Error(0x%x) in NEXUS_Record_SetSettings()\n\r", neResult);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			g_astPvrRecInst[ulDeviceId].recordSuspend = true;
		}
		else
		{
			PrintError("[DI_PVR_REC_Pause] Invalid device status! (status : %d, recordSuspend : %d)\n\r",
				g_astPvrRecInst[ulDeviceId].status, g_astPvrRecInst[ulDeviceId].recordSuspend);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_Pause");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_REC_Resume(HUINT32 ulDeviceId )
{
//	NEXUS_Error	neResult;
//	NEXUS_RecordSettings	recordCfg;

	PrintEnter("DI_PVR_REC_Resume");

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_Resume] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	g_astPvrRecInst[ulDeviceId].bFlush = FALSE;

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status != DI_PVR_FREE && g_astPvrRecInst[ulDeviceId].recordSuspend == true)
		{
			NEXUS_Error neResult;
			NEXUS_RecordSettings	recordCfg;

			NEXUS_Record_GetSettings(g_astPvrRecInst[ulDeviceId].recordHandle, &recordCfg);
#if (NEXUS_VERSION <= 900)
#else
			recordCfg.recpumpSettings.recpumpSuspend = false;
#endif
			neResult = NEXUS_Record_SetSettings(g_astPvrRecInst[ulDeviceId].recordHandle, &recordCfg);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[DI_PVR_REC_Resume] Error(0x%x) in NEXUS_Record_SetSettings()\n\r", neResult);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			g_astPvrRecInst[ulDeviceId].recordSuspend = false;
		}
		else
		{
			PrintError("[DI_PVR_REC_Resume] Invalid device status! (status : %d, recordSuspend : %d)\n\r",
				g_astPvrRecInst[ulDeviceId].status, g_astPvrRecInst[ulDeviceId].recordSuspend);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_Resume");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_REC_GetDuration(HUINT32 ulDeviceId, HUINT32 *pulTime )
{
	NEXUS_Error	neResult;
	NEXUS_RecordStatus	recStatus;

	PrintEnter("DI_PVR_REC_GetDuration");

	if(pulTime == NULL)
	{
		PrintError("[DI_PVR_REC_GetDuration] pulTime == NULL\n\r");
		return DI_ERR_INVALID_PARAM;
	}

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_GetDuration] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED)
		{
			neResult = NEXUS_Record_GetStatus(g_astPvrRecInst[ulDeviceId].recordHandle, &recStatus);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[DI_PVR_REC_GetDuration] Error(0x%x) in NEXUS_Record_GetStatus()\n\r", neResult);
				VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
				return DI_ERR_ERROR;
			}
			*pulTime = (HUINT32)(recStatus.lastTimestamp/1000); /* ms -> sec */
		}
		else
		{
			PrintError("[DI_PVR_REC_GetDuration] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_GetDuration");

	return DI_ERR_OK;
}

DI_ERR_CODE  DI_PVR_REC_ConvertTimeToPosition(HUINT32 ulDeviceId, HUINT32 ulTime, HUINT64 *pulPosition)
{
	PrintEnter("DI_PVR_REC_ConvertTimeToPosition");

	if(pulPosition == NULL)
	{
		PrintError("[DI_PVR_REC_ConvertTimeToPosition] pulPosition == NULL\n\r");
		return DI_ERR_INVALID_PARAM;
	}

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_ConvertTimeToPosition] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED)
		{
			BSTD_UNUSED(ulTime); // for clean compile
			BSTD_UNUSED(pulPosition); // for clean compile
		}
		else
		{
			PrintError("[DI_PVR_REC_ConvertTimeToPosition] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_ConvertTimeToPosition");

	return DI_ERR_OK;
}

DI_ERR_CODE  DI_PVR_REC_ConvertPositionToTime(HUINT32 ulDeviceId, HUINT64 ulPosition, HUINT32 *pulTime)
{
	PrintEnter("DI_PVR_REC_ConvertPositionToTime");

	if(pulTime == NULL)
	{
		PrintError("[DI_PVR_REC_ConvertPositionToTime] pulTime == NULL\n\r");
		return DI_ERR_INVALID_PARAM;
	}

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_ConvertPositionToTime] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED)
		{
			BSTD_UNUSED(ulPosition); // for clean compile
			BSTD_UNUSED(pulTime); // for clean compile
		}
		else
		{
			PrintError("[DI_PVR_REC_ConvertPositionToTime] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DI_ERR_ERROR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_ConvertPositionToTime");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_REC_Flush(HUINT32 ulDeviceId)
{				
	const void *buffer;
	size_t size;

	PrintEnter("DI_PVR_REC_Flush");

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DI_PVR_REC_Flush] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status != DI_PVR_FREE && g_astPvrRecInst[ulDeviceId].recordSuspend == true)
		{
			g_astPvrRecInst[ulDeviceId].bFlush = TRUE;		
			NEXUS_Recpump_GetDataBuffer(g_astPvrRecInst[ulDeviceId].recPumpHandle, &buffer, &size);
			NEXUS_Recpump_DataReadComplete(g_astPvrRecInst[ulDeviceId].recPumpHandle,size);
		}

		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DI_PVR_REC_Flush");

	return DI_ERR_OK;

}

DRV_Error DRV_PVR_REC_GetDeviceInfo(HUINT32 ulDeviceId, DRV_PVR_REC_DEVICE_INFO *pstRecDevInfo)
{
	PrintEnter("DRV_PVR_REC_GetDeviceInfo");

	if(pstRecDevInfo == NULL)
	{
		PrintError("[DRV_PVR_REC_GetDeviceInfo] pstRecDevInfo == NULL\n\r");
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(ulDeviceId >= MAX_REC_INST_NUM)
	{
		PrintError("[DRV_PVR_REC_GetDeviceInfo] Invalid device ID! (ulDeviceId : %d)\n\r", ulDeviceId);
		return DRV_ERR;
	}

	if(VK_SEM_Get(g_astPvrRecInst[ulDeviceId].diSemId) == VK_OK)
	{
		if(g_astPvrRecInst[ulDeviceId].status == DI_PVR_ENABLED)
		{
			pstRecDevInfo->deviceId = ulDeviceId;
			pstRecDevInfo->recordHandle = g_astPvrRecInst[ulDeviceId].recordHandle;
			pstRecDevInfo->fifoRecHandle = g_astPvrRecInst[ulDeviceId].fifoRecHandle;
#if defined(CONFIG_DELAYED_RECORD)
			pstRecDevInfo->delayedRecordOn = g_astPvrRecInst[ulDeviceId].switchedfromTsrToLinear;
#endif
		}
		else
		{
			PrintError("[DRV_PVR_REC_GetDeviceInfo] Invalid device status! (status : %d)\n\r", g_astPvrRecInst[ulDeviceId].status);
			VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
			return DRV_ERR;
		}
		VK_SEM_Release(g_astPvrRecInst[ulDeviceId].diSemId);
	}

	PrintExit("DRV_PVR_REC_GetDeviceInfo");

	return DRV_OK;
}

static DRV_Error P_REC_Start(unsigned long ulDeviceId)
{
	NEXUS_Error	neResult;
	int err;

	neResult = NEXUS_Record_Start(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].fileRecHandle);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_REC_Start] Error(0x%x) in NEXUS_Record_Start()\n\r", neResult);
		return DRV_ERR;
    }

	err = VK_TIMER_EventEvery(g_astPvrRecInst[ulDeviceId].timeCodeNotifyInterval,
							P_REC_TimeNotifyCallback,
							NULL,
							0,
							&(g_astPvrRecInst[ulDeviceId].timeCodeNotifyTimerId));
	if(err != VK_OK)
	{
		NEXUS_Record_Stop(g_astPvrRecInst[ulDeviceId].recordHandle);
		PrintError("[P_REC_Start] Error(0x%x) in VK_TIMER_EventEvery()\n\r", err);
		return DRV_ERR;
	}

	return DRV_OK;
}

#if	0 //defined(CONFIG_DELAYED_RECORD)
static DRV_Error P_REC_RestartForDelayRecording(
	unsigned long ulDeviceId,
	DI_PVR_REC_DELAYRECORD_SETUP_t *pstSetup,
	char *pucRetOldAvFileName,
	char *pucRetOldIndexFileName,
	NEXUS_FileDelayedCopyPositionInfo *pstRetFileDelayedCopyPosInfo)
{
	NEXUS_Error	neResult;
	DRV_Error drvErr;
	unsigned long msTime;
	NEXUS_FifoRecordDelayedPositionInfo fifoDlyPosInfo;
	char *avFileName = NULL;
	char *indexFileName = NULL;
	NEXUS_FileRecordHandle	fileRecHandle = NULL;

	/* Record buffer에 남아있는 data를 write */
	NEXUS_Record_StopForFileChange(g_astPvrRecInst[ulDeviceId].recordHandle);

	msTime = (pstSetup->ulStartTime)*1000; /* sec -> ms */
	neResult = NEXUS_FifoRecord_CloseForDelayRecording(g_astPvrRecInst[ulDeviceId].fifoRecHandle, msTime, DATA_ALIGN, &fifoDlyPosInfo);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_REC_RestartForDelayRecording] Error(0x%x) in NEXUS_FifoRecord_CloseForDelayRecording()\n\r", neResult);
		return DRV_ERR;
    }

	avFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
	if(avFileName == NULL)
	{
		PrintError("[P_REC_RestartForDelayRecording] Error in DD_MEM_Alloc()\n\r");
		return DRV_ERR;
	}
	indexFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
	if(indexFileName == NULL)
	{
		PrintError("[P_REC_RestartForDelayRecording] Error in DD_MEM_Alloc()\n\r");
		DD_MEM_Free(avFileName);
		return DRV_ERR;
	}
	VK_memcpy((void *)avFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
	VK_strcat(avFileName, ".ts\0"); // Add  fileExtension
	VK_memcpy((void *)indexFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)pstSetup->aucFileName)+1 ); //include terminating 0
	VK_strcat(indexFileName, ".nts\0"); // Add  fileExtension
	fileRecHandle = NEXUS_FileRecord_OpenPosixForDelayRecording(avFileName, indexFileName, fifoDlyPosInfo.data.delayedSize, fifoDlyPosInfo.index.delayedSize);
	if (!fileRecHandle)
	{
		PrintError("[P_REC_RestartForDelayRecording] Error in NEXUS_FileRecord_OpenPosixForDelayRecording()\n\r");
		DD_MEM_Free(avFileName);
		DD_MEM_Free(indexFileName);
		return DRV_ERR;
	}
	DD_MEM_Free(avFileName);
	DD_MEM_Free(indexFileName);

	if(pstSetup->stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_CURRENT)
	{
		drvErr = P_REC_SetEncryption(g_astPvrRecInst[ulDeviceId].recordHandle, g_astPvrRecInst[ulDeviceId].encKeyHandle, &(pstSetup->stEncryptSettings));
		if (drvErr != DRV_OK)
		{
			PrintError("[P_REC_RestartForDelayRecording] Error(0x%x) in P_REC_SetEncryption()\n\r", drvErr);
			return DRV_ERR;
		}
	}

	neResult = NEXUS_Record_StartFileChange(g_astPvrRecInst[ulDeviceId].recordHandle, fileRecHandle);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_REC_RestartForDelayRecording] Error(0x%x) in NEXUS_Record_StartFileChange()\n\r", neResult);
		return DRV_ERR;
	}

	pstRetFileDelayedCopyPosInfo->data.copyStartOffset = fifoDlyPosInfo.data.delayedStartOffset;
	pstRetFileDelayedCopyPosInfo->data.copyEndOffset = fifoDlyPosInfo.data.delayedStartOffset + (off_t)fifoDlyPosInfo.data.delayedSize;
	pstRetFileDelayedCopyPosInfo->index.copyStartOffset = fifoDlyPosInfo.index.delayedStartOffset;
	pstRetFileDelayedCopyPosInfo->index.copyEndOffset = fifoDlyPosInfo.index.delayedStartOffset + (off_t)fifoDlyPosInfo.index.delayedSize;
	VK_memcpy((void *)pucRetOldIndexFileName, (void *)g_astPvrRecInst[ulDeviceId].setup.aucFileName, VK_strlen((char *)g_astPvrRecInst[ulDeviceId].setup.aucFileName)+1 ); //include terminating 0
	VK_strcat(pucRetOldIndexFileName, ".nts\0"); // Add  fileExtension
	VK_memcpy((void *)pucRetOldAvFileName, (void *)g_astPvrRecInst[ulDeviceId].setup.aucFileName, VK_strlen((char *)g_astPvrRecInst[ulDeviceId].setup.aucFileName)+1 ); //include terminating 0
	VK_strcat(pucRetOldAvFileName, ".ts\0"); // Add  fileExtension

	VK_memcpy((void *)g_astPvrRecInst[ulDeviceId].setup.aucFileName, (void *)(pstSetup->aucFileName), VK_strlen((char *)(pstSetup->aucFileName))+1 ); //include terminating 0
	if(pstSetup->stEncryptSettings.eEncType != DI_PVR_ENCRYPTION_TYPE_CURRENT)
	{
		g_astPvrRecInst[ulDeviceId].setup.stEncryptSettings = pstSetup->stEncryptSettings;
	}
	g_astPvrRecInst[ulDeviceId].setup.eFileType = eDI_PVR_FileType_Linear;
	g_astPvrRecInst[ulDeviceId].fileRecHandle = fileRecHandle;
	g_astPvrRecInst[ulDeviceId].fifoRecHandle = NULL;
	g_astPvrRecInst[ulDeviceId].delayedTimeOffset = msTime; //ms

	return DRV_OK;
}
#else

#endif

#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
static DRV_Error P_REC_LoadEncryptionKey(NEXUS_KeySlotHandle ulEncKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings, HBOOL btimestampOn)
#else
static DRV_Error P_REC_LoadEncryptionKey(NEXUS_KeySlotHandle ulEncKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings)
#endif
{
#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
	NEXUS_Error	neResult;
	NEXUS_SecurityAlgorithmSettings	algConfig;
	NEXUS_SecurityClearKey			key;
	NEXUS_SecurityEncryptedSessionKey encSessionKey;
	NEXUS_SecurityEncryptedControlWord encCw;
	HUINT8 	encKey[ENCRYPTION_KEY_LEN];
#if (BCHP_CHIP == 7584) && defined(CONFIG_CAS_CX)
	const HUINT8						klrcKey[ENCRYPTION_KEY_LEN] = {0x70, 0x72, 0x69, 0x65, 0x74, 0x61, 0x72, 0x79, 0x43, 0x6f, 0x6e, 0x61, 0x78, 0x50, 0x72, 0x6f};
#endif

	PrintEnter("P_REC_LoadEncryptionKey\r");
	DI_UART_Print("\033[01;32m[%s,%d] eEncType: %d\033[00m\n", __FUNCTION__, __LINE__, pstEncryptSettings->eEncType);

	if(pstEncryptSettings == NULL)
	{
		PrintError("[P_REC_LoadEncryptionKey] pstEncryptSettings == NULL\n\r");
		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_MEM_Memcpy(encKey, pstEncryptSettings->ucKey, ENCRYPTION_KEY_LEN);

	if(pstEncryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_3DES)
	{
		DRV_CRYPT_ExpandKeyLength(encKey,encKey);
	}

	NEXUS_Security_GetDefaultClearKey(&key);
	NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);

#if defined(CONFIG_CAS_ASKM)
	algConfig.ivMode 			= NEXUS_SecurityIVMode_eRegular;
	algConfig.solitarySelect 		= NEXUS_SecuritySolitarySelect_eClear;
	algConfig.caVendorID 		= 0xEAF6;
	algConfig.askmModuleID 	= NEXUS_SecurityAskmModuleID_eModuleID_8;
	algConfig.otpId 			= NEXUS_SecurityOtpId_eOtpVal;
	algConfig.key2Select 		= NEXUS_SecurityKey2Select_eRealKey;
#endif

	switch(pstEncryptSettings->eEncType)
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
#if defined(CONFIG_DI20)
			algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
#else
			algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
#endif
			algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		default :
			PrintError("[P_REC_LoadEncryptionKey] Not supported encryption type! (eEncType : %d)\n\r", pstEncryptSettings->eEncType);
			return DRV_ERR_INVALID_PARAMETER;
	}
	algConfig.operation = NEXUS_SecurityOperation_eEncrypt;
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	algConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps;
	algConfig.bRestrictEnable = false;
	algConfig.bEncryptBeforeRave = false;
	algConfig.bGlobalEnable = true;
	algConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
	algConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal]     = true;
	algConfig.keyDestEntryType = NEXUS_SecurityKeyType_eClear;
#endif

#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
	if(btimestampOn == TRUE)
	{
		algConfig.enableTimestamps = true;
	}
#else
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
	algConfig.enableTimestamps = true; /* enable timestamps */
#endif
#endif

#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	algConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eEven;
	algConfig.scValue[NEXUS_SecurityPacketType_eGlobal] = NEXUS_SecurityAlgorithmScPolarity_eEven;
    neResult = NEXUS_Security_ConfigAlgorithm(ulEncKeyHandle, &algConfig);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in NEXUS_Security_ConfigAlgorithm(eEven)\n\r", neResult);
		return DRV_ERR;
    }

	algConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eOdd;
	algConfig.scValue[NEXUS_SecurityPacketType_eGlobal] = NEXUS_SecurityAlgorithmScPolarity_eOdd;
    neResult = NEXUS_Security_ConfigAlgorithm(ulEncKeyHandle, &algConfig);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in NEXUS_Security_ConfigAlgorithm(eOdd)\n\r", neResult);
		return DRV_ERR;
    }
#else
    neResult = NEXUS_Security_ConfigAlgorithm(ulEncKeyHandle, &algConfig);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in NEXUS_Security_ConfigAlgorithm()\n\r", neResult);
		return DRV_ERR;
    }
#endif
    /* Load AV keys */
	switch(pstEncryptSettings->eEncType)
	{
		case DI_PVR_ENCRYPTION_TYPE_3DES :
		case DI_PVR_ENCRYPTION_TYPE_AES :
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
			key.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps;
			key.keyEntryType = NEXUS_SecurityKeyType_eClear;
#else
			key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
#endif
		    key.keySize = sizeof(encKey);
		    VK_memcpy(key.keyData, encKey, sizeof(encKey));
		    neResult = NEXUS_Security_LoadClearKey(ulEncKeyHandle, &key);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in NEXUS_Security_LoadClearKey()\n\r", neResult);
				return DRV_ERR;
		    }
#if defined(CONFIG_DI20)
			if(pstEncryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_AES)
			{
				/* Load IV for AES_CBC*/
				NEXUS_Security_GetDefaultClearKey(&key);
#if 0	/*Use this conventional-way to be compatible with DI_CRYPT*/
				key.keyEntryType = NEXUS_SecurityKeyType_eIv;
#else   /*To apply this, DI_CRYPT shall also be modified.*/
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
			key.keyEntryType = NEXUS_SecurityKeyType_eClear;
#else
			key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
#endif
				key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
#endif
				key.keySize = sizeof(pstEncryptSettings->ucIv);
			    VK_memcpy(key.keyData,pstEncryptSettings->ucIv,key.keySize);
				neResult = NEXUS_Security_LoadClearKey (ulEncKeyHandle, &key);
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
			encSessionKey.keyladderID   = NEXUS_SecurityKeyladderID_eA;
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
			encSessionKey.cusKeyVarH    = 0xFF;
#endif
			encSessionKey.rootKeySrc    = NEXUS_SecurityRootKeySrc_eOtpKeyA;
		 	encSessionKey.bRouteKey     = false;
		 	encSessionKey.operation     = NEXUS_SecurityOperation_eDecrypt;
			encSessionKey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
		 	encSessionKey.keyEntryType  = NEXUS_SecurityKeyType_eOdd;

			encSessionKey.custSubMode	 		= NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
#if (BCHP_CHIP == 7584) && defined(CONFIG_CAS_CX)
			encSessionKey.virtualKeyLadderID 	= NEXUS_SecurityVirtualKeyladderID_eVKL1;
#else
			encSessionKey.virtualKeyLadderID 	= NEXUS_SecurityVirtualKeyladderID_eVKL5;
#endif
			encSessionKey.keyMode		 		= NEXUS_SecurityKeyMode_eRegular;

#if defined(CONFIG_SECURITY_EXTENSION)
			encSessionKey.keyGenCmdID			= NEXUS_SecurityKeyGenCmdID_eKeyGen;
			encSessionKey.sessionKeyOp 		= NEXUS_SecuritySessionKeyOp_eNoProcess;
#if defined(CONFIG_CAS_ASKM)
			encSessionKey.bASKMMode	 		= true;
#else
			encSessionKey.bASKMMode	 		= false;
#endif
			encSessionKey.bSwapAESKey	 		= false;
			encSessionKey.keyDestIVType 		= NEXUS_SecurityKeyIVType_eNoIV;
#endif
#if (BCHP_CHIP == 7584) && defined(CONFIG_CAS_CX)
			VK_memcpy((void *)encSessionKey.keyData, (void *)klrcKey, sizeof(pstEncryptSettings->ucProtectionKey));
#else
			VK_memcpy((void *)encSessionKey.keyData, (void *)pstEncryptSettings->ucProtectionKey, sizeof(pstEncryptSettings->ucProtectionKey));
#endif
			neResult = NEXUS_Security_GenerateSessionKey(ulEncKeyHandle, &encSessionKey);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in NEXUS_Security_GenerateSessionKey()\n\r", neResult);
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
			encCw.keySize = sizeof(encKey);
			VK_memcpy((void *)encCw.keyData, (void *)encKey, sizeof(encKey));
			neResult = NEXUS_Security_GenerateControlWord(ulEncKeyHandle , &encCw);
			if(neResult != NEXUS_SUCCESS)
			{
				PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in NEXUS_Security_GenerateControlWord()\n\r", neResult);
				return DRV_ERR;
		    }
#if defined(CONFIG_DI20)
			if(pstEncryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY)
			{
				/* Load IV for AES_CBC*/
				NEXUS_Security_GetDefaultClearKey(&key);
#if 1	/*Use this conventional-way to be compatible with DI_CRYPT*/
				key.keyEntryType = NEXUS_SecurityKeyType_eIv;
#else   /*To apply this, DI_CRYPT shall also be modified.*/
			    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
				key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
#endif
				key.keySize = sizeof(pstEncryptSettings->ucIv);
			    VK_memcpy(key.keyData,pstEncryptSettings->ucIv,key.keySize);
				neResult = NEXUS_Security_LoadClearKey (ulEncKeyHandle, &key);
				if(neResult != NEXUS_SUCCESS)
				{
					PrintError("[P_REC_LoadEncryptionKey] Error(0x%x) in Loading IV by NEXUS_Security_LoadClearKey()\n\r", neResult);
					return DRV_ERR;
				}
			}
#endif
			break;
		default :
			PrintError("[P_REC_LoadEncryptionKey] Not supported encryption type! (eEncType : %d)\n\r", pstEncryptSettings->eEncType);
			return DRV_ERR_INVALID_PARAMETER;
	}

	PrintExit("P_REC_LoadEncryptionKey");
#endif

	return DRV_OK;
}
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
static DRV_Error P_REC_SetEncryption(NEXUS_RecordHandle ulRecordHandle, NEXUS_KeySlotHandle ulEncKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings, HBOOL btimestampOn)
#else
static DRV_Error P_REC_SetEncryption(NEXUS_RecordHandle ulRecordHandle, NEXUS_KeySlotHandle ulEncKeyHandle, DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings)
#endif
{
	NEXUS_Error	neResult;
	DRV_Error	drvErr;
	NEXUS_RecordSettings	recordCfg;
#if defined(REC_DYNAMIC_DMA_ALLOC)
	HUINT32 ulRecIdx = 0;
#endif

	PrintEnter("P_REC_SetEncryption\r");

	if(pstEncryptSettings == NULL)
	{
		PrintError("[P_REC_SetEncryption] pstEncryptSettings == NULL\n\r");
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(pstEncryptSettings->eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
	{
#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
		drvErr = P_REC_LoadEncryptionKey(ulEncKeyHandle, pstEncryptSettings, btimestampOn);
#else
		drvErr = P_REC_LoadEncryptionKey(ulEncKeyHandle, pstEncryptSettings);
#endif
		if (drvErr != DRV_OK)
		{
			PrintError("[P_REC_SetEncryption] Error(0x%x) in P_REC_LoadEncryptionKey()\n\r", drvErr);
			return DRV_ERR;
	    }
	}

	NEXUS_Record_GetSettings(ulRecordHandle, &recordCfg);
#if !defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	recordCfg.recpumpSettings.securityDma = pstEncryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_NONE ? NULL : g_ulDmaHandle;
	recordCfg.recpumpSettings.securityContext = pstEncryptSettings->eEncType == DI_PVR_ENCRYPTION_TYPE_NONE ? NULL : ulEncKeyHandle;
#endif
#if defined(REC_DYNAMIC_DMA_ALLOC)
	if(pstEncryptSettings->eEncType != DI_PVR_ENCRYPTION_TYPE_NONE)
	{
		recordCfg.recpumpSettings.securityDma = NEXUS_Dma_Open(NEXUS_ANY_ID,NULL);
		if(recordCfg.recpumpSettings.securityDma != NULL)
		{
			for(ulRecIdx=0; ulRecIdx < MAX_REC_INST_NUM; ulRecIdx++)
			{
				if(g_astPvrRecInst[ulRecIdx].recordHandle == ulRecordHandle)
				{
					g_astPvrRecInst[ulRecIdx].dmaHandle = recordCfg.recpumpSettings.securityDma;
					break;
				}
			}
		}
		else
		{
			PrintError("[P_REC_SetDecryption] NEXUS_Dma_Open Error!!\n");
		}
	}
#endif
	neResult = NEXUS_Record_SetSettings(ulRecordHandle, &recordCfg);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_REC_SetEncryption] Error(0x%x) in NEXUS_Record_SetSettings()\n\r", neResult);
#if defined(REC_DYNAMIC_DMA_ALLOC)
		if(recordCfg.recpumpSettings.securityDma != NULL)
		{
			NEXUS_Dma_Close(recordCfg.recpumpSettings.securityDma);
			if(ulRecIdx < MAX_REC_INST_NUM)
			{
				if(g_astPvrRecInst[ulRecIdx].dmaHandle != NULL)
					g_astPvrRecInst[ulRecIdx].dmaHandle = NULL;
			}
		}
#endif
		return DRV_ERR;
    	}

	PrintExit("P_REC_SetEncryption");

	return DRV_OK;
}

#if 0
static NEXUS_VideoCodec P_REC_ConvertVideoCodec2Nexus(DI_PVR_VIDEOTYPE_t videoCodec)
{
	NEXUS_VideoCodec nexusVideoCodec;

	switch(videoCodec)
	{
		case eDI_PVR_VideoType_Mpeg1:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg1;
			break;
		case eDI_PVR_VideoType_Mpeg2:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg2;
			break;
		case eDI_PVR_VideoType_Mpeg2Dtv:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg2;
			/*more codes required related to set transport type*/
			break;
		case eDI_PVR_VideoType_Mpeg2Dtv_Pes:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg2;
			/*more codes required related to set transport type*/
			break;
		case eDI_PVR_VideoType_Mpeg4_Part2:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg4Part2;
			break;
		case eDI_PVR_VideoType_H261:	//divx test
//			nexusVideoCodec = NEXUS_VideoCodec_eUnknown; /*seemed to be unsupported in Nexus platform, but can be modified.*/
			nexusVideoCodec = NEXUS_VideoCodec_eDivx311; /*seemed to be unsupported in Nexus platform, but can be modified.*/
			break;
		case eDI_PVR_VideoType_H263:
			nexusVideoCodec = NEXUS_VideoCodec_eH263;
			break;
		case eDI_PVR_VideoType_H264:
			nexusVideoCodec = NEXUS_VideoCodec_eH264;
			break;
		case eDI_PVR_VideoType_Vc1_Sm:
			nexusVideoCodec = NEXUS_VideoCodec_eVc1SimpleMain;
			break;
		case eDI_PVR_VideoType_Vc1:
			nexusVideoCodec = NEXUS_VideoCodec_eVc1;
			break;
		case eDI_PVR_VideoType_Avs:
			nexusVideoCodec = NEXUS_VideoCodec_eAvs;
			break;
		case eDI_PVR_VideoType_Divx_311:
			nexusVideoCodec = NEXUS_VideoCodec_eDivx311;
			break;
		default:
			nexusVideoCodec = NEXUS_VideoCodec_eUnknown;
			break;
	}
	return nexusVideoCodec;
}
#endif

static void P_REC_BufferOverflowCallback(void *context, int param)
{
	P_REC_INSTANCE *pvrRecInst;
	HUINT32 deviceId;
	DI_PVR_MSG_t stCallbackMsg;

	deviceId = param;
	pvrRecInst	= (P_REC_INSTANCE *)context;

	stCallbackMsg.ulSender = DI_PVR_RECORDER;
	stCallbackMsg.ulInstance = deviceId;
	stCallbackMsg.ulEvent = DI_PVR_EVENT_REC_BUFFER_OVERFLOW;
	stCallbackMsg.ulParam1 = 0;
	stCallbackMsg.ulParam2 = 0;
	stCallbackMsg.ulParam3 = 0;

	PrintError("[P_REC_BufferOverflowCallback] deviceId(%d) : Buffer Overflow!!\n\r", deviceId);
#if defined(USE_VK_INTR_TO_CALLBACK)
	if(g_astPvrRecInst[deviceId].recEvtCbId[DI_PVR_EVENT_REC_BUFFER_OVERFLOW].valid != FALSE)
	{
		VK_INTR_Event(g_astPvrRecInst[deviceId].recEvtCbId[DI_PVR_EVENT_REC_BUFFER_OVERFLOW].intId,(void *)&stCallbackMsg);
	}
#else
	if(g_astPvrRecInst[deviceId].recEvtCbFunc[DI_PVR_EVENT_REC_BUFFER_OVERFLOW] != NULL)
	{
		g_astPvrRecInst[deviceId].recEvtCbFunc[DI_PVR_EVENT_REC_BUFFER_OVERFLOW]((void *)&stCallbackMsg);
	}
#endif
}

static void P_REC_WriteErrorCallback(void *context, int param)
{
	P_REC_INSTANCE *pvrRecInst;
	HUINT32 deviceId;
	DI_PVR_MSG_t stCallbackMsg;
	NEXUS_RecordStatus recStatus;
	NEXUS_Error neResult;

	deviceId = param;
	pvrRecInst	= (P_REC_INSTANCE *)context;

	stCallbackMsg.ulSender = DI_PVR_RECORDER;
	stCallbackMsg.ulInstance = deviceId;
	stCallbackMsg.ulEvent = DI_PVR_EVENT_REC_WRITE_ERROR;
	stCallbackMsg.ulParam1 = 0;
	stCallbackMsg.ulParam2 = 0;
	stCallbackMsg.ulParam3 = 0;

	neResult = NEXUS_Record_GetStatus(pvrRecInst->recordHandle, &recStatus);
	if(neResult != NEXUS_SUCCESS)
	{
		PrintError("[P_REC_WriteErrorCallback] Error(0x%x) in NEXUS_Record_GetStatus()\n\r", neResult);
	}
	else
	{
		switch(recStatus.error)
		{
			case NEXUS_RecordErrorType_eNone:
				PrintError("[P_REC_WriteErrorCallback] deviceId(%d) : Write Error(None)!!\n\r", deviceId);
				break;
			case NEXUS_RecordErrorType_eDiskFull:
				PrintError("[P_REC_WriteErrorCallback] deviceId(%d) : Write Error(DiskFull)!!\n\r", deviceId);
				break;
			case NEXUS_RecordErrorType_eMaxFileSize:
				PrintError("[P_REC_WriteErrorCallback] deviceId(%d) : Write Error(MaxFileSize)!!\n\r", deviceId);
				break;
			case NEXUS_RecordErrorType_eUnknown:
				PrintError("[P_REC_WriteErrorCallback] deviceId(%d) : Write Error(Unknown)!!\n\r", deviceId);
				break;
		}
	}

#if defined(USE_VK_INTR_TO_CALLBACK)
	if(g_astPvrRecInst[deviceId].recEvtCbId[DI_PVR_EVENT_REC_WRITE_ERROR].valid != FALSE)
	{
		VK_INTR_Event(g_astPvrRecInst[deviceId].recEvtCbId[DI_PVR_EVENT_REC_WRITE_ERROR].intId,(void *)&stCallbackMsg);
	}
#else
	if(g_astPvrRecInst[deviceId].recEvtCbFunc[DI_PVR_EVENT_REC_WRITE_ERROR] != NULL)
	{
		g_astPvrRecInst[deviceId].recEvtCbFunc[DI_PVR_EVENT_REC_WRITE_ERROR]((void *)&stCallbackMsg);
	}
#endif
}

static void P_REC_TimeNotifyCallback(unsigned long tid, void *arg)
{
	int err;
	unsigned long i;
	P_REC_TASK_MSG sendMsg;

	BSTD_UNUSED(arg);

	VK_MEM_Memset(&sendMsg, 0x0, sizeof(P_REC_TASK_MSG));

	for(i=0; i<MAX_REC_INST_NUM; i++)
	{
		if(tid == g_astPvrRecInst[i].timeCodeNotifyTimerId)
		{
			break;
		}
	}
	if(i == MAX_REC_INST_NUM)
	{
		PrintError("[P_REC_TimeNotifyCallback] Invalid timer ID! (tid : 0x%x)\n\r", tid);
		return;
	}

	sendMsg.deviceId = i;
	sendMsg.msgId = P_REC_MSG_TIME_NOTIFY;
	err = VK_MSG_SendTimeout(g_ulRecTimeTaskMsgQueueId, &sendMsg, sizeof(P_REC_TASK_MSG), 0);
	if(err != VK_OK)
	{
		PrintError("[P_REC_TimeNotifyCallback] Error(0x%x) in VK_MSG_SendTimeout()\n\r", err);
	}
}

#if	defined(CONFIG_TEST_COPY_TASK)
DRV_Error DRV_PVR_REC_StartCopyTask(void)
{
#if 0
	P_REC_TASK_MSG sendMsg;
	int err;
	char srcFileName[22] = "/mnt/hd2/copytest.ori";
	char destFileName[22] = "/mnt/hd2/copytest.cpy";
	long long	compareSize= (long long)(1024*1024)*(long long)(100*9); //900M

	NEXUS_FileDelayedCopyHandle fileDlyCopyHandle = NULL;
	NEXUS_FileDelayedCopyOpenSettings fileDlycopyOpenSettings;
	NEXUS_RecordDelayedCopyHandle recDlyCopyHandle = NULL;
	NEXUS_RecordDelayedCopyOpenSettings recDlyCopyOpenSettings;


	fileDlycopyOpenSettings.isSrcFileFifo = FALSE;
	fileDlycopyOpenSettings.filesName.data.srcFileName	= srcFileName;
	fileDlycopyOpenSettings.filesName.data.destFileName	= destFileName;
	fileDlycopyOpenSettings.filesName.index.srcFileName = NULL;
	fileDlycopyOpenSettings.filesName.index.destFileName	= NULL;

	fileDlycopyOpenSettings.useTimeOffset = false;
	fileDlycopyOpenSettings.dataCutAllign =DATA_ALLIGN;


	fileDlycopyOpenSettings.positionInfo.data.copyStartOffset		= 0;
	fileDlycopyOpenSettings.positionInfo.data.copyCurrentOffset 	= 0;
	fileDlycopyOpenSettings.positionInfo.data.copyEndOffset 		= compareSize;
	fileDlycopyOpenSettings.positionInfo.index.copyStartOffset		= 0;
	fileDlycopyOpenSettings.positionInfo.index.copyCurrentOffset	= 0;
	fileDlycopyOpenSettings.positionInfo.index.copyEndOffset		= 0;

	fileDlyCopyHandle = NEXUS_FileRecord_OpenDelayedCopy(&fileDlycopyOpenSettings);
	if(!fileDlyCopyHandle)
	{
		PrintError("[DRV_PVR_REC_StartCopyTask] Error in NEXUS_FileRecord_OpenTestCopy()\n\r");
		return DI_ERR_ERROR;
	}

	recDlyCopyOpenSettings.bufferSize = DELAYED_COPY_BUFFER_SIZE;
	recDlyCopyOpenSettings.alignment = 12; /* 4K alignment */
	recDlyCopyOpenSettings.copyEnd.callback=P_REC_DelayedCopyEndCallBack;
	recDlyCopyOpenSettings.securityDma = NULL;
	recDlyCopyOpenSettings.securityContext = NULL;
	recDlyCopyHandle = NEXUS_Record_StartDelayedCopy(&recDlyCopyOpenSettings, fileDlyCopyHandle);
	if(!recDlyCopyHandle)
	{
		PrintError("[DRV_PVR_REC_StartCopyTask] Error in NEXUS_Record_OpenDelayedCopy()\n\r");
		return DI_ERR_ERROR;
	}

	s_stDelayedRecInfo.used = true;
	s_stDelayedRecInfo.delayRecRequestId = 0;
	s_stDelayedRecInfo.delayedCopyStatus = 0;
	s_stDelayedRecInfo.recDelayedCopyHandle = recDlyCopyHandle;

	sendMsg.msgId = P_REC_MSG_DELAYED_COPY_STATUS;
	err = VK_MSG_SendTimeout(g_ulDelayedRecTaskMsgQueueId, &sendMsg, sizeof(P_REC_TASK_MSG), 0);
	if(err != VK_OK)
	{
		PrintError("[DI_PVR_REC_StartCopyTask] Error(0x%x) in VK_MSG_SendTimeout()\n\r", err);
		return DRV_ERR;
	}
#endif
	return DRV_OK;
}
#endif

#if	defined(CONFIG_DELAYED_RECORD)
static void P_REC_DelayedCopyStartTimerCallback(unsigned long tid, void *arg)
{
	int err;
	P_REC_TASK_MSG sendMsg;

	BSTD_UNUSED(arg);
	VK_MEM_Memset(&sendMsg, 0x0, sizeof(P_REC_TASK_MSG));

	if(tid != s_stDelayedRecInfo.delayedCopyStatusTimerId)
	{
		PrintError("[P_REC_DelayedCopyStatusTimerCallback] Invalid timer ID! (tid : 0x%x)\n\r", tid);
		return;
	}
	s_stDelayedRecInfo.delayedCopyStatusTimerId = VK_TIMERID_NULL;

	sendMsg.msgId = P_REC_MSG_DELAYED_COPY_START;
	err = VK_MSG_SendTimeout(g_ulDelayedRecTaskMsgQueueId, &sendMsg, sizeof(P_REC_TASK_MSG), 0);
	if(err != VK_OK)
	{
		PrintError("[P_REC_DelayedCopyStatusTimerCallback] Error(0x%x) in VK_MSG_SendTimeout()\n\r", err);
	}
}

static void P_REC_DelayedCopyStatusTimerCallback(unsigned long tid, void *arg)
{
	int err;
	P_REC_TASK_MSG sendMsg;

	BSTD_UNUSED(arg);
	VK_MEM_Memset(&sendMsg, 0x0, sizeof(P_REC_TASK_MSG));

	if(tid != s_stDelayedRecInfo.delayedCopyStatusTimerId)
	{
		PrintError("[P_REC_DelayedCopyStatusTimerCallback] Invalid timer ID! (tid : 0x%x)\n\r", tid);
		return;
	}
	s_stDelayedRecInfo.delayedCopyStatusTimerId = VK_TIMERID_NULL;

	sendMsg.msgId = P_REC_MSG_DELAYED_COPY_STATUS;
	err = VK_MSG_SendTimeout(g_ulDelayedRecTaskMsgQueueId, &sendMsg, sizeof(P_REC_TASK_MSG), 0);
	if(err != VK_OK)
	{
		PrintError("[P_REC_DelayedCopyStatusTimerCallback] Error(0x%x) in VK_MSG_SendTimeout()\n\r", err);
	}
}

static void P_REC_DelayedCopyErrorCallBack(unsigned long tid, void *arg)
{
	int err;
	P_REC_TASK_MSG sendMsg;

	BSTD_UNUSED(arg);
	VK_MEM_Memset(&sendMsg, 0x0, sizeof(P_REC_TASK_MSG));

	if(tid != s_stDelayedRecInfo.delayedCopyStatusTimerId)
	{
		PrintError("[P_REC_DelayedCopyStatusTimerCallback] Invalid timer ID! (tid : 0x%x)\n\r", tid);
		return;
	}
	s_stDelayedRecInfo.delayedCopyStatusTimerId = VK_TIMERID_NULL;

	sendMsg.msgId = P_REC_MSG_DELAYED_COPY_ERROR;
	err = VK_MSG_SendTimeout(g_ulDelayedRecTaskMsgQueueId, &sendMsg, sizeof(P_REC_TASK_MSG), 0);
	if(err != VK_OK)
	{
		PrintError("[P_REC_DelayedCopyStatusCallBack] Error(0x%x) in VK_MSG_SendTimeout()\n\r", err);
	}
}

static void P_REC_DelayedCopyEndCallBack(unsigned long tid, void *arg)
{
	int err;
	P_REC_TASK_MSG sendMsg;

	BSTD_UNUSED(arg);
	VK_MEM_Memset(&sendMsg, 0x0, sizeof(P_REC_TASK_MSG));

	if(tid != s_stDelayedRecInfo.delayedCopyStatusTimerId)
	{
		PrintError("[P_REC_DelayedCopyStatusTimerCallback] Invalid timer ID! (tid : 0x%x)\n\r", tid);
		return;
	}
	s_stDelayedRecInfo.delayedCopyStatusTimerId = VK_TIMERID_NULL;

	sendMsg.msgId = P_REC_MSG_DELAYED_COPY_END;
	err = VK_MSG_SendTimeout(g_ulDelayedRecTaskMsgQueueId, &sendMsg, sizeof(P_REC_TASK_MSG), 0);
	if(err != VK_OK)
	{
		PrintError("[P_REC_DelayedCopyEndTimerCallBack] Error(0x%x) in VK_MSG_SendTimeout()\n\r", err);
	}
}

static void P_REC_DelayedRecordTask(void *param)
{
	int err;
	P_REC_TASK_MSG rcvMsg;
	DI_PVR_MSG_t stCallbackMsg;
	NEXUS_Error neResult;
	int delCopyStatus = 0;
	int subCopyStatus;
	BSTD_UNUSED(param);

	while(1)
	{
		err = VK_MSG_Receive(g_ulDelayedRecTaskMsgQueueId, &rcvMsg, sizeof(P_REC_TASK_MSG));
		if(err != VK_OK)
		{
			PrintError("[P_REC_DelayedRecordTask] Error(0x%x) in VK_MSG_Receive()\n\r", err);
			continue;
		}

		VK_MEM_Memset(&stCallbackMsg, 0x0, sizeof(DI_PVR_MSG_t));
		switch(rcvMsg.msgId)
		{
			case P_REC_MSG_DELAYED_COPY_START:
				VK_SEM_Get(s_stDelayedRecInfo.semId);
				if(s_stDelayedRecInfo.used)
				{
					neResult = NEXUS_ChunkedFifoRecord_DelayedIndexCopy_Init(s_stDelayedRecInfo.fifoRecHandle);
					if(neResult == NEXUS_NOT_AVAILABLE)
					{
						err = VK_TIMER_EventAfter(DELAYED_COPY_STATUS_TIMER,
												P_REC_DelayedCopyStartTimerCallback,
												NULL,
												0,
												&(s_stDelayedRecInfo.delayedCopyStatusTimerId));
						if(err != VK_OK)
						{
							PrintError("[P_REC_DelayedRecordTask] Error(0x%x) in VK_TIMER_EventAfter()\n\r", err);
							VK_SEM_Release(s_stDelayedRecInfo.semId);
							break;
						}
					}
					else if(neResult == NEXUS_SUCCESS)
					{
						err = VK_TIMER_EventAfter(DELAYED_COPY_STATUS_TIMER,
												P_REC_DelayedCopyStatusTimerCallback,
												NULL,
												0,
												&(s_stDelayedRecInfo.delayedCopyStatusTimerId));
						if(err != VK_OK)
						{
							PrintError("[P_REC_DelayedRecordTask] Error(0x%x) in VK_TIMER_EventAfter()\n\r", err);
							VK_SEM_Release(s_stDelayedRecInfo.semId);
							break;
						}
					}
					else
					{
						err = VK_TIMER_EventAfter(DELAYED_COPY_STATUS_TIMER,
												P_REC_DelayedCopyErrorCallBack,
												NULL,
												0,
												&(s_stDelayedRecInfo.delayedCopyStatusTimerId));
						if(err != VK_OK)
						{
							PrintError("[P_REC_DelayedRecordTask] Error(0x%x) in VK_TIMER_EventAfter()\n\r", err);
							VK_SEM_Release(s_stDelayedRecInfo.semId);
							break;
						}
					}
				}
				VK_SEM_Release(s_stDelayedRecInfo.semId);
				break;
			case P_REC_MSG_DELAYED_COPY_STATUS :
				VK_SEM_Get(s_stDelayedRecInfo.semId);

				if(s_stDelayedRecInfo.used)
				{
					delCopyStatus = 10;
					subCopyStatus = NEXUS_ChunkedFifoRecord_CopyData(s_stDelayedRecInfo.fifoRecHandle);
					if(subCopyStatus != 0)
					{
#if 0 //hee
						if(s_stDelayedRecInfo.delayRecStatusCallbackFunc != NULL)
						{
							stCallbackMsg.ulSender = DI_PVR_RECORDER;
							stCallbackMsg.ulInstance = s_stDelayedRecInfo.delayRecRequestId;
							stCallbackMsg.ulEvent = DI_PVR_EVENT_DELAYED_RECORD_STATUS_CHANGED;
							/* 여기서는 99% 까지만 notify한다. 100%는 copy 동작 완전히 종료할 때 */
							stCallbackMsg.ulParam1 = delCopyStatus >= 100 ? 99 : delCopyStatus;
							stCallbackMsg.ulParam2 = (HUINT32)0;
							stCallbackMsg.ulParam3 = (HUINT32)0;
							VK_SEM_Release(s_stDelayedRecInfo.semId);
							s_stDelayedRecInfo.delayRecStatusCallbackFunc((void *)&stCallbackMsg);
							VK_SEM_Get(s_stDelayedRecInfo.semId);
						}
#endif

						/* 100%는 copy 동작 완전히 종료할 때 */
						s_stDelayedRecInfo.delayedCopyStatus = delCopyStatus >= 100 ? 99 : delCopyStatus;

						err = VK_TIMER_EventAfter(10,
												P_REC_DelayedCopyStatusTimerCallback,
												NULL,
												0,
												&(s_stDelayedRecInfo.delayedCopyStatusTimerId));
						if(err != VK_OK)
						{
							PrintError("[P_REC_DelayedRecordTask] Error(0x%x) in VK_TIMER_EventAfter()\n\r", err);
							VK_SEM_Release(s_stDelayedRecInfo.semId);
							break;
						}
					}
					else
					{
						err = VK_TIMER_EventAfter(10,
												P_REC_DelayedCopyEndCallBack,
												NULL,
												0,
												&(s_stDelayedRecInfo.delayedCopyStatusTimerId));
						if(err != VK_OK)
						{
							PrintError("[P_REC_DelayedRecordTask] Error(0x%x) in VK_TIMER_EventAfter()\n\r", err);
							VK_SEM_Release(s_stDelayedRecInfo.semId);
							break;
						}
					}
				}

				VK_SEM_Release(s_stDelayedRecInfo.semId);
				break;

			case P_REC_MSG_DELAYED_COPY_ERROR :
				VK_SEM_Get(s_stDelayedRecInfo.semId);

				if(s_stDelayedRecInfo.used)
				{
					if(s_stDelayedRecInfo.delayRecStatusCallbackFunc != NULL)
					{
						stCallbackMsg.ulSender = DI_PVR_RECORDER;
						stCallbackMsg.ulInstance = s_stDelayedRecInfo.delayRecRequestId;
						stCallbackMsg.ulEvent = DI_PVR_EVENT_DELAYED_RECORD_STATUS_CHANGED;
						stCallbackMsg.ulParam1= 200; /* copy fail */
						stCallbackMsg.ulParam2 = 0;
						stCallbackMsg.ulParam3 = 0;
						VK_SEM_Release(s_stDelayedRecInfo.semId);
						s_stDelayedRecInfo.delayRecStatusCallbackFunc((void *)&stCallbackMsg);
						VK_SEM_Get(s_stDelayedRecInfo.semId);
					}
					s_stDelayedRecInfo.delayedCopyStatusTimerId = VK_TIMERID_NULL;
					s_stDelayedRecInfo.delayedCopyStatus = 200; /* copy fail */
				}

				PrintError("[P_REC_DelayedRecordTask] P_REC_MSG_DELAYED_COPY_ERROR\n\r");
				VK_SEM_Release(s_stDelayedRecInfo.semId);
				break;

			case P_REC_MSG_DELAYED_COPY_END :
				VK_SEM_Get(s_stDelayedRecInfo.semId);

				if(s_stDelayedRecInfo.used)
				{
					NEXUS_ChunkedFifoRecord_DelayedIndexCopy_Uninit(s_stDelayedRecInfo.fifoRecHandle);

					if(s_stDelayedRecInfo.delayRecStatusCallbackFunc != NULL)
					{
						stCallbackMsg.ulSender = DI_PVR_RECORDER;
						stCallbackMsg.ulInstance = s_stDelayedRecInfo.delayRecRequestId;
						stCallbackMsg.ulEvent = DI_PVR_EVENT_DELAYED_RECORD_STATUS_CHANGED;
						stCallbackMsg.ulParam1 = 100; /* copy done. progress 100% */
						stCallbackMsg.ulParam2 = 0;
						stCallbackMsg.ulParam3 = 0;
						VK_SEM_Release(s_stDelayedRecInfo.semId);
						s_stDelayedRecInfo.delayRecStatusCallbackFunc((void *)&stCallbackMsg);
						VK_SEM_Get(s_stDelayedRecInfo.semId);
					}
					PrintError("[P_REC_DelayedRecordTask] Delayed Copy completed.\n\r");
					s_stDelayedRecInfo.used = false;
					s_stDelayedRecInfo.delayedCopyStatusTimerId = VK_TIMERID_NULL;
					s_stDelayedRecInfo.delayedCopyStatus = 100; /* copy done. progress 100% */
					s_stDelayedRecInfo.fifoRecHandle = NULL;
				}

				VK_SEM_Release(s_stDelayedRecInfo.semId);
				break;

			default :
				PrintError("[P_REC_DelayedRecordTask] Invalid message! (msgId : %d)\n\r", rcvMsg.msgId);
				break;
		}
	}
}
#endif

#if defined(USE_VK_INTR_TO_CALLBACK)
static void P_REC_TimeTask(void *param)
{
	NEXUS_Error	neResult;
	int err;
	unsigned long deviceId;
	P_REC_TASK_MSG rcvMsg;
	NEXUS_RecordStatus	recStatus;
	NEXUS_PidChannelScramblingStatus recScrStatus;
	unsigned char scrStatusChanged;
	DI_PVR_MSG_t stCallbackMsg;
	NEXUS_FilePosition firstPos, lastPos;
#if defined(ENABLE_RECORD_CHUNK_FILE)
	NEXUS_ChunkedFifoRecordSettings fifoRecordCfg;
#else
	NEXUS_FifoRecordSettings fifoRecordCfg;
#endif
	unsigned long timeLimit;
	unsigned long unUsage=0;

	BSTD_UNUSED(param);

	while(1)
	{
		err = VK_MSG_Receive(g_ulRecTimeTaskMsgQueueId, &rcvMsg, sizeof(P_REC_TASK_MSG));
		if(err != VK_OK)
		{
			PrintError("[P_REC_TimeTask] Error(0x%x) in VK_MSG_Receive()\n\r", err);
			continue;
		}

		deviceId = rcvMsg.deviceId;
		VK_MEM_Memset(&stCallbackMsg, 0x0, sizeof(DI_PVR_MSG_t));

		switch(rcvMsg.msgId)
		{
			case P_REC_MSG_TIME_NOTIFY :
				if(VK_SEM_Get(g_astPvrRecInst[deviceId].diSemId) == VK_OK)
				{
					if(g_astPvrRecInst[deviceId].status == DI_PVR_ENABLED)
					{
						neResult = NEXUS_Record_GetStatus(g_astPvrRecInst[deviceId].recordHandle, &recStatus);
						if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[P_REC_TimeTask] Error(0x%x) in NEXUS_Record_GetStatus()\n\r", neResult);
							VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
							break;
						}

						scrStatusChanged = false;
#if 0
						neResult = NEXUS_Record_TpitIndexerEnable(g_astPvrRecInst[deviceId].recordHandle, true);
						if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[P_REC_TimeTask] Error(0x%x) in NEXUS_Record_TpitIndexerEnable()\n\r", neResult);
						}
#endif
						if(g_astPvrRecInst[deviceId].idxPidChHandle != NULL)
						{
							if (g_astPvrRecInst[deviceId].bTsRec == FALSE)
							{
#if 0
								scrStatusChanged = false;
								g_astPvrRecInst[deviceId].lastScrambleStatus =  false;
#else
								neResult = NEXUS_PidChannel_GetScramblingStatus(g_astPvrRecInst[deviceId].idxPidChHandle, &recScrStatus);
								if(neResult != NEXUS_SUCCESS)
								{
									PrintError("[P_REC_TimeTask] Error(0x%x) in NEXUS_PidChannel_GetScramblingStatus()\n\r", neResult);
									if(recScrStatus.scrambled == true)
									{
										NEXUS_PidChannel_ClearScramblingStatus(g_astPvrRecInst[deviceId].idxPidChHandle);
									}
									VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
									break;
								}
								if(recScrStatus.scrambled == true)
								{
									NEXUS_PidChannel_ClearScramblingStatus(g_astPvrRecInst[deviceId].idxPidChHandle);
								}
								PrintDebug("[P_REC_TimeTask] recScrStatus.pidExists = 0x%x, recScrStatus.scrambled=0x%x\n",recScrStatus.pidExists, recScrStatus.scrambled);
#if 0 /*HEE*/
								recScrStatus.scrambled = false; /*set true forcibly for test.*/
#endif
#endif
								if(recScrStatus.pidExists && (g_astPvrRecInst[deviceId].lastScrambleStatus != (!recScrStatus.scrambled ? false : true)))
								{
									scrStatusChanged = true;
									g_astPvrRecInst[deviceId].lastScrambleStatus = (!recScrStatus.scrambled) ? false : true;

									if(g_astPvrRecInst[deviceId].setup.eStreamType == eDI_PVR_Stream_Video) /* audio stream인 경우 항상 TPIT 사용 */
									{
										neResult = NEXUS_Record_TpitIndexerEnable(g_astPvrRecInst[deviceId].recordHandle, (bool)g_astPvrRecInst[deviceId].lastScrambleStatus);
										if(neResult != NEXUS_SUCCESS)
										{
											PrintError("[P_REC_TimeTask] Error(0x%x) in NEXUS_Record_TpitIndexerEnable()\n\r", neResult);
											VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
											break;
										}
									}
								}
							}
							else
							{
								recScrStatus.scrambled = false;
							}

						}

						if(g_astPvrRecInst[deviceId].recEvtCbId[DI_PVR_EVENT_TIMECODE_CHANGED].valid != FALSE)
						{
							//Converting ms value to sec.
							if(recStatus.lastTimestamp > g_astPvrRecInst[deviceId].delayedTimeOffset)
							{
								stCallbackMsg.ulParam1 = (HUINT32)((recStatus.lastTimestamp - g_astPvrRecInst[deviceId].delayedTimeOffset)/1000); /* ms -> sec */
							}
							else
							{
								stCallbackMsg.ulParam1 = 0;
							}
							stCallbackMsg.ulSender = DI_PVR_RECORDER;
							stCallbackMsg.ulInstance = rcvMsg.deviceId;
							stCallbackMsg.ulEvent = DI_PVR_EVENT_TIMECODE_CHANGED;
							stCallbackMsg.ulParam2 = (HUINT32)0;
							stCallbackMsg.ulParam3 = (HUINT32)0;

							PrintDebug("[P_REC_TimeTask] deviceId : %d, time : %d, CDB depth : %d, ITB depth : %d, paused : %d, encryption type : %d\n",
								deviceId, stCallbackMsg.ulParam1, recStatus.recpumpStatus.data.fifoDepth, recStatus.recpumpStatus.index.fifoDepth,
								g_astPvrRecInst[deviceId].recordSuspend, g_astPvrRecInst[deviceId].setup.stEncryptSettings.eEncType);
#if defined(CONFIG_DELAYED_RECORD)
							if(g_astPvrRecInst[deviceId].setup.eFileType == eDI_PVR_FileType_Circular && g_astPvrRecInst[deviceId].switchedfromTsrToLinear == 0)
#else
							if(g_astPvrRecInst[deviceId].setup.eFileType == eDI_PVR_FileType_Circular)
#endif
							{
								if(g_astPvrRecInst[deviceId].fifoRecHandle)
								{
#if defined(ENABLE_RECORD_CHUNK_FILE)
									neResult = NEXUS_ChunkedFifoRecord_GetPosition(g_astPvrRecInst[deviceId].fifoRecHandle, &firstPos, &lastPos);
#else
									neResult = NEXUS_FifoRecord_GetPosition(g_astPvrRecInst[deviceId].fifoRecHandle, &firstPos, &lastPos);
#endif
									if(neResult != NEXUS_SUCCESS)
									{
										PrintError("[P_REC_TimeTask] Error(0x%x) in NEXUS_FifoRecord_GetPosition()\n\r", neResult);
										VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
										break;
									}
#if defined(ENABLE_RECORD_CHUNK_FILE)
									NEXUS_ChunkedFifoRecord_GetSettings(g_astPvrRecInst[deviceId].fifoRecHandle, &fifoRecordCfg);
#else
									NEXUS_FifoRecord_GetSettings(g_astPvrRecInst[deviceId].fifoRecHandle, &fifoRecordCfg);
#endif
									timeLimit = fifoRecordCfg.interval*1000;
									unUsage = (unsigned long)(((unsigned long long)TSR_USED_100_PERCENT*(lastPos.timestamp-firstPos.timestamp))/timeLimit);
	 								if(unUsage > (unsigned long)TSR_USED_100_PERCENT)
									{
										unUsage = (unsigned long)TSR_USED_100_PERCENT;
									}
									stCallbackMsg.ulParam2 = (HUINT32)unUsage;
									stCallbackMsg.ulParam3 = (HUINT32)(firstPos.timestamp/1000);
								}
							}
							/* callback에서 어떤 함수를 호출할지 알 수 없으므로 일단 semaphore release 해준다. */
							VK_INTR_Event(g_astPvrRecInst[deviceId].recEvtCbId[DI_PVR_EVENT_TIMECODE_CHANGED].intId,(void *)&stCallbackMsg);
						}

						/*if(scrStatusChanged && (g_astPvrRecInst[deviceId].recEvtCbFunc[DI_PVR_EVENT_STREAM_UNSCRAMBLED] != NULL )
							&& (g_astPvrRecInst[deviceId].recEvtCbFunc[DI_PVR_EVENT_STREAM_SCRAMBLED] != NULL))*/
						if(scrStatusChanged && (g_astPvrRecInst[deviceId].recEvtCbId[DI_PVR_EVENT_STREAM_UNSCRAMBLED].valid != FALSE )
							&& (g_astPvrRecInst[deviceId].recEvtCbId[DI_PVR_EVENT_STREAM_SCRAMBLED].valid != FALSE))
						{
							stCallbackMsg.ulSender = DI_PVR_RECORDER;
							stCallbackMsg.ulInstance = rcvMsg.deviceId;
							stCallbackMsg.ulEvent = g_astPvrRecInst[deviceId].lastScrambleStatus ? DI_PVR_EVENT_STREAM_SCRAMBLED : DI_PVR_EVENT_STREAM_UNSCRAMBLED;
							stCallbackMsg.ulParam1 = (HUINT32)0;
							stCallbackMsg.ulParam2 = (HUINT32)0;
							stCallbackMsg.ulParam3 = (HUINT32)0;
							/* callback에서 어떤 함수를 호출할지 알 수 없으므로 일단 semaphore release 해준다. */
							VK_INTR_Event(g_astPvrRecInst[deviceId].recEvtCbId[stCallbackMsg.ulEvent].intId, (void *)&stCallbackMsg);
						}
					}
					VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
				}
				break;
			default :
				PrintError("[P_REC_TimeTask] Invalid message! (msgId : %d)\n\r", rcvMsg.msgId);
				break;
		}
	}
}

#else
static void P_REC_TimeTask(void *param)
{
	NEXUS_Error	neResult;
	int err;
	unsigned long deviceId;
	P_REC_TASK_MSG rcvMsg;
	NEXUS_RecordStatus	recStatus;
	NEXUS_PidChannelScramblingStatus recScrStatus;
	unsigned char scrStatusChanged;
	DI_PVR_MSG_t stCallbackMsg;
	NEXUS_FilePosition firstPos, lastPos;
#if defined(ENABLE_RECORD_CHUNK_FILE)
	NEXUS_ChunkedFifoRecordSettings fifoRecordCfg;
#else
	NEXUS_FifoRecordSettings fifoRecordCfg;
#endif
	unsigned long timeLimit;
	unsigned long unUsage=0;

	BSTD_UNUSED(param);

	while(1)
	{
		err = VK_MSG_Receive(g_ulRecTimeTaskMsgQueueId, &rcvMsg, sizeof(P_REC_TASK_MSG));
		if(err != VK_OK)
		{
			PrintError("[P_REC_TimeTask] Error(0x%x) in VK_MSG_Receive()\n\r", err);
			continue;
		}

		deviceId = rcvMsg.deviceId;

		switch(rcvMsg.msgId)
		{
			case P_REC_MSG_TIME_NOTIFY :
				if(VK_SEM_Get(g_astPvrRecInst[deviceId].diSemId) == VK_OK)
				{
					if(g_astPvrRecInst[deviceId].status == DI_PVR_ENABLED)
					{
						neResult = NEXUS_Record_GetStatus(g_astPvrRecInst[deviceId].recordHandle, &recStatus);
						if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[P_REC_TimeTask] Error(0x%x) in NEXUS_Record_GetStatus()\n\r", neResult);
							VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
							break;
						}

						scrStatusChanged = false;
#if 0
						neResult = NEXUS_Record_TpitIndexerEnable(g_astPvrRecInst[deviceId].recordHandle, true);
						if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[P_REC_TimeTask] Error(0x%x) in NEXUS_Record_TpitIndexerEnable()\n\r", neResult);
						}
#endif
						if(g_astPvrRecInst[deviceId].idxPidChHandle != NULL)
						{
#if 0
								scrStatusChanged = false;
								g_astPvrRecInst[deviceId].lastScrambleStatus =  false;
#else
							neResult = NEXUS_PidChannel_GetScramblingStatus(g_astPvrRecInst[deviceId].idxPidChHandle, &recScrStatus);
							if(neResult != NEXUS_SUCCESS)
							{
								PrintError("[P_REC_TimeTask] Error(0x%x) in NEXUS_PidChannel_GetScramblingStatus()\n\r", neResult);
								if(recScrStatus.scrambled == true)
								{
									NEXUS_PidChannel_ClearScramblingStatus(g_astPvrRecInst[deviceId].idxPidChHandle);
								}
								VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
								break;
							}
							if(recScrStatus.scrambled == true)
							{
								NEXUS_PidChannel_ClearScramblingStatus(g_astPvrRecInst[deviceId].idxPidChHandle);
							}
#if 0 /*HEE*/
							recScrStatus.scrambled = false; /*set true forcibly for test.*/
#endif
							if(recScrStatus.pidExists && (g_astPvrRecInst[deviceId].lastScrambleStatus != (!recScrStatus.scrambled ? false : true)))
							{
								scrStatusChanged = true;
								g_astPvrRecInst[deviceId].lastScrambleStatus = (!recScrStatus.scrambled) ? false : true;

								if(g_astPvrRecInst[deviceId].setup.eStreamType == eDI_PVR_Stream_Video) /* audio stream인 경우 항상 TPIT 사용 */
								{
									neResult = NEXUS_Record_TpitIndexerEnable(g_astPvrRecInst[deviceId].recordHandle, (bool)g_astPvrRecInst[deviceId].lastScrambleStatus);
									if(neResult != NEXUS_SUCCESS)
									{
										PrintError("[P_REC_TimeTask] Error(0x%x) in NEXUS_Record_TpitIndexerEnable()\n\r", neResult);
										VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
										break;
									}
								}
							}
#endif
						}

						if(g_astPvrRecInst[deviceId].recEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED] != NULL)
						{
							//Converting ms value to sec.
							if(recStatus.lastTimestamp > g_astPvrRecInst[deviceId].delayedTimeOffset)
							{
								stCallbackMsg.ulParam1 = (HUINT32)((recStatus.lastTimestamp - g_astPvrRecInst[deviceId].delayedTimeOffset)/1000); /* ms -> sec */
							}
							else
							{
								stCallbackMsg.ulParam1 = 0;
							}
							stCallbackMsg.ulSender = DI_PVR_RECORDER;
							stCallbackMsg.ulInstance = rcvMsg.deviceId;
							stCallbackMsg.ulEvent = DI_PVR_EVENT_TIMECODE_CHANGED;
							stCallbackMsg.ulParam2 = (HUINT32)0;
							stCallbackMsg.ulParam3 = (HUINT32)0;

							PrintDebug("[P_REC_TimeTask] deviceId : %d, time : %d, CDB depth : %d, ITB depth : %d, paused : %d, encryption type : %d  scramble: %d\n",
								deviceId, stCallbackMsg.ulParam1, recStatus.recpumpStatus.data.fifoDepth, recStatus.recpumpStatus.index.fifoDepth,
								g_astPvrRecInst[deviceId].recordSuspend, g_astPvrRecInst[deviceId].setup.stEncryptSettings.eEncType, g_astPvrRecInst[deviceId].lastScrambleStatus);
#if defined(CONFIG_DELAYED_RECORD)
							if(g_astPvrRecInst[deviceId].setup.eFileType == eDI_PVR_FileType_Circular && g_astPvrRecInst[deviceId].switchedfromTsrToLinear == 0)
#else
							if(g_astPvrRecInst[deviceId].setup.eFileType == eDI_PVR_FileType_Circular)
#endif
							{
								if(g_astPvrRecInst[deviceId].fifoRecHandle)
								{
#if defined(ENABLE_RECORD_CHUNK_FILE)
									neResult = NEXUS_ChunkedFifoRecord_GetPosition(g_astPvrRecInst[deviceId].fifoRecHandle, &firstPos, &lastPos);
#else
									neResult = NEXUS_FifoRecord_GetPosition(g_astPvrRecInst[deviceId].fifoRecHandle, &firstPos, &lastPos);
#endif
									if(neResult != NEXUS_SUCCESS)
									{
										PrintError("[P_REC_TimeTask] Error(0x%x) in NEXUS_FifoRecord_GetPosition()\n\r", neResult);
										VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
										break;
									}
#if defined(ENABLE_RECORD_CHUNK_FILE)
									NEXUS_ChunkedFifoRecord_GetSettings(g_astPvrRecInst[deviceId].fifoRecHandle, &fifoRecordCfg);
#else
									NEXUS_FifoRecord_GetSettings(g_astPvrRecInst[deviceId].fifoRecHandle, &fifoRecordCfg);
#endif
									timeLimit = fifoRecordCfg.interval*1000;
									unUsage = (unsigned long)(((unsigned long long)TSR_USED_100_PERCENT*(lastPos.timestamp-firstPos.timestamp))/timeLimit);
	 								if(unUsage > (unsigned long)TSR_USED_100_PERCENT)
									{
										unUsage = (unsigned long)TSR_USED_100_PERCENT;
									}
									stCallbackMsg.ulParam2 = (HUINT32)unUsage;
									stCallbackMsg.ulParam3 = (HUINT32)(firstPos.timestamp/1000);
								}
							}
							/* callback에서 어떤 함수를 호출할지 알 수 없으므로 일단 semaphore release 해준다. */
							VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
							g_astPvrRecInst[deviceId].recEvtCbFunc[DI_PVR_EVENT_TIMECODE_CHANGED]((void *)&stCallbackMsg);
							VK_SEM_Get(g_astPvrRecInst[deviceId].diSemId);
						}

						if(scrStatusChanged && (g_astPvrRecInst[deviceId].recEvtCbFunc[DI_PVR_EVENT_STREAM_UNSCRAMBLED] != NULL )
							&& (g_astPvrRecInst[deviceId].recEvtCbFunc[DI_PVR_EVENT_STREAM_SCRAMBLED] != NULL))
						{
							stCallbackMsg.ulSender = DI_PVR_RECORDER;
							stCallbackMsg.ulInstance = rcvMsg.deviceId;
							stCallbackMsg.ulEvent = g_astPvrRecInst[deviceId].lastScrambleStatus ? DI_PVR_EVENT_STREAM_SCRAMBLED : DI_PVR_EVENT_STREAM_UNSCRAMBLED;
							stCallbackMsg.ulParam1 = (HUINT32)0;
							stCallbackMsg.ulParam2 = (HUINT32)0;
							stCallbackMsg.ulParam3 = (HUINT32)0;
							/* callback에서 어떤 함수를 호출할지 알 수 없으므로 일단 semaphore release 해준다. */
							VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
							g_astPvrRecInst[deviceId].recEvtCbFunc[stCallbackMsg.ulEvent]((void *)&stCallbackMsg);
							VK_SEM_Get(g_astPvrRecInst[deviceId].diSemId);
						}
					}
					VK_SEM_Release(g_astPvrRecInst[deviceId].diSemId);
				}
				break;
			default :
				PrintError("[P_REC_TimeTask] Invalid message! (msgId : %d)\n\r", rcvMsg.msgId);
				break;
		}
	}
}
#endif

#if defined(CONFIG_DSC) || defined(CONFIG_HDCP) || defined(CONFIG_CRYPTO)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO) && !defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
static NEXUS_KeySlotHandle P_REC_GetKeySlotHandleFromDSC(HUINT32 unChannelIndex)
{
	DRV_Error	drv_Err;
	HUINT32 unDemuxId;
	HUINT16 usPid;
	NEXUS_KeySlotHandle dsckeyHandle = NULL;

	drv_Err = DRV_DEMUX_GetRecordPidChannelInfo(unChannelIndex, &unDemuxId, &usPid);
	if(drv_Err != DRV_OK)
	{
		PrintError("[%s] DRV_DEMUX_GetRecordPidChannelInfo() : unChannelIndex(%d)\n\r", __func__, unChannelIndex);
		return NULL;
	}
	else
	{
		dsckeyHandle = DRV_DSC_GetKeySlotHandle(unDemuxId, usPid);
		PrintDebug("[%s] DRV_DSC_GetKeySlotHandle() : unChannelIndex(%d), keyHandle(%p)\n\r", __func__, unChannelIndex, dsckeyHandle);
		return dsckeyHandle;
	}
}
#endif
#endif

/* below RECHandle used on CMD */
DRV_Error DRV_PVR_REC_GetRECHandle(HUINT32 unDeviceId, NEXUS_RecordHandle *ppstRecordHandle)
{
	if((ppstRecordHandle == NULL) || (unDeviceId >= MAX_REC_INST_NUM))
	{
		PrintError("[DRV_PVR_REC_GetPBHandle] Error : Invalid parameter\n\r");
		return DRV_ERR;
	}

	if(g_astPvrRecInst[unDeviceId].status != DI_PVR_FREE)
	{
		*ppstRecordHandle = g_astPvrRecInst[unDeviceId].recordHandle;
	}
	else
	{
		PrintError("[%s %d] Error : Invalid Status\n",__func__,__LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_PVR_REC_GetIDXPidHandle(HUINT32 unDeviceId, NEXUS_PidChannelHandle *ppstPidChHandle)
{
	if((ppstPidChHandle == NULL) || (unDeviceId >= MAX_REC_INST_NUM))
	{
		PrintError("[DRV_PVR_REC_GetPBHandle] Error : Invalid parameter\n\r");
		return DRV_ERR;
	}

	if(g_astPvrRecInst[unDeviceId].status != DI_PVR_FREE)
	{
		*ppstPidChHandle = g_astPvrRecInst[unDeviceId].idxPidChHandle;

	}
	else
	{
		PrintError("[%s %d] Error : Invalid Status\n",__func__,__LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

#if defined(CONFIG_USB_FLASH_RECORD)
void DRV_PVR_TunePdflush(HBOOL bTunePdflushForUsbFlash)
{
	FILE *fpDirtyRatio, *fpDirtyBackgroundRatio, *fpDirtyWritebackCentisecs, *fpDirtyExpireCentisecs;
	HUINT32 unDirtyRatio, unDirtyBackgroundRatio, unDirtyWritebackCentisecs, unDirtyExpireCentisecs;

	if(bTunePdflushForUsbFlash == TRUE)
	{
		unDirtyRatio = DIRTY_RATIO_VALUE_FOR_USB_FLASH;
		unDirtyBackgroundRatio = DIRTY_BACKGROUND_RATIO_VALUE_FOR_USB_FLASH;
		unDirtyWritebackCentisecs = DIRTY_WRITEBACK_CENTISECS_VALUE_FOR_USB_FLASH;
		unDirtyExpireCentisecs = DIRTY_EXPIRE_CENTISECS_VALUE_FOR_USB_FLASH;
	}
	else
	{
		unDirtyRatio = DIRTY_RATIO_VALUE_FOR_HDD;
		unDirtyBackgroundRatio = DIRTY_BACKGROUND_RATIO_VALUE_FOR_HDD;
		unDirtyWritebackCentisecs = DIRTY_WRITEBACK_CENTISECS_VALUE_FOR_HDD;
		unDirtyExpireCentisecs = DIRTY_EXPIRE_CENTISECS_VALUE_FOR_HDD;
	}

	fpDirtyRatio = fopen(DIRTY_RATIO_FILE_NAME, "wb+");
	if (fpDirtyRatio)
	{
		VK_fprintf(fpDirtyRatio,"%d" , unDirtyRatio);
		fclose(fpDirtyRatio);
		PrintDebug("[DRV_PVR_TunePdflush] %s file %d write\n", DIRTY_RATIO_FILE_NAME, unDirtyRatio);
	}
	else
	{
		PrintError("[DRV_PVR_TunePdflush] %s file open fail\n", DIRTY_RATIO_FILE_NAME);
	}

	fpDirtyBackgroundRatio = fopen(DIRTY_BACKGROUND_RATIO_FILE_NAME, "wb+");
	if (fpDirtyBackgroundRatio)
	{
		VK_fprintf(fpDirtyBackgroundRatio,"%d" , unDirtyBackgroundRatio);
		fclose(fpDirtyBackgroundRatio);
		PrintDebug("[DRV_PVR_TunePdflush] %s file %d write\n", DIRTY_BACKGROUND_RATIO_FILE_NAME, unDirtyBackgroundRatio);
	}
	else
	{
		PrintError("[DRV_PVR_TunePdflush] %s file open fail\n", DIRTY_BACKGROUND_RATIO_FILE_NAME);
	}

	fpDirtyWritebackCentisecs = fopen(DIRTY_WRITEBACK_CENTISECS_FILE_NAME, "wb+");
	if (fpDirtyWritebackCentisecs)
	{
		VK_fprintf(fpDirtyWritebackCentisecs,"%d" , unDirtyWritebackCentisecs);
		fclose(fpDirtyWritebackCentisecs);
		PrintDebug("[DRV_PVR_TunePdflush] %s file %d write\n", DIRTY_WRITEBACK_CENTISECS_FILE_NAME, unDirtyWritebackCentisecs);
	}
	else
	{
		PrintError("[DRV_PVR_TunePdflush] %s file open fail\n", DIRTY_WRITEBACK_CENTISECS_FILE_NAME);
	}

	fpDirtyExpireCentisecs = fopen(DIRTY_EXPIRE_CENTISECS_FILE_NAME, "wb+");
	if (fpDirtyExpireCentisecs)
	{
		VK_fprintf(fpDirtyExpireCentisecs,"%d" , unDirtyExpireCentisecs);
		fclose(fpDirtyExpireCentisecs);
		PrintDebug("[DRV_PVR_TunePdflush] %s file %d write\n", DIRTY_EXPIRE_CENTISECS_FILE_NAME, unDirtyExpireCentisecs);
	}
	else
	{
		PrintError("[DRV_PVR_TunePdflush] %s file open fail\n", DIRTY_WRITEBACK_CENTISECS_FILE_NAME);
	}
}
#endif


#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)

DRV_Error P_REC_AddDscKeyslot(HUINT32 ulDeviceId, NEXUS_KeySlotHandle keyHandle, NEXUS_PidChannelHandle pidChHandle)
{
	HUINT32 i;

	PrintDebug("[%s,%d] ulDeviceId: %d, keyHandle: %x, pidChHandle: %x\n", __FUNCTION__, __LINE__, ulDeviceId, keyHandle, pidChHandle);
	if((keyHandle == NULL) || (pidChHandle == NULL))
	{
		PrintError("[%s] Error : Invalid parameter\n\r", __FUNCTION__);
		return DRV_ERR;
	}
	
	/* find matched pidchhadle */
	for(i=0; i<P_REC_MAX_PID_PER_REC_INT; i++)
	{
		if(g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].pesPidChHandle == pidChHandle)
		{
			PrintDebug("[%s]pidchannel is found at %d\n", __FUNCTION__, i);
			break;
		}
	}

	/* find null pidchannel */
	if(i == P_REC_MAX_PID_PER_REC_INT)
	{
		for(i=0; i<P_REC_MAX_PID_PER_REC_INT; i++)
		{
			if(g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].pesPidChHandle == NULL)
			{
				PrintDebug("NULL pidchannel is found at %d\n", i);
				break;
			}
		}

		if(i < P_REC_MAX_PID_PER_REC_INT)
		{
			PrintDebug("[%s] add pidChHandle: %x, keyHandle: %x at %d.\n", __FUNCTION__, pidChHandle, keyHandle, i);
			g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].pesPidChHandle = pidChHandle;
			g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].dscKeyHandle = keyHandle;
		}
		else
		{
			PrintError("[%s]There are not empty dscKeyInfo instance!\n", __FUNCTION__);
			return DRV_ERR;
		}
	}

	/* if found matched pidchannel, update dsckeyhandle */
	if(g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].dscKeyHandle != keyHandle)
	{
		PrintDebug("[%s]dscKeyHandle is changed to %x at %d\n", __FUNCTION__, keyHandle, i);
		g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].dscKeyHandle = keyHandle;
	}

	return DRV_OK;
}


DRV_Error P_REC_RemoveDscKeyslot(HUINT32 ulDeviceId, NEXUS_PidChannelHandle pidChHandle)
{
	HUINT32 i;

	PrintDebug("[%s,%d] ulDeviceId: %d, pidChHandle: %x\n", __FUNCTION__, __LINE__, ulDeviceId, pidChHandle);
	if(pidChHandle == NULL)
	{
		PrintError("[%s] Error : Invalid parameter\n\r", __FUNCTION__);
		return DRV_ERR;
	}
	
	/* find matched pidchhadle */
	for(i=0; i<P_REC_MAX_PID_PER_REC_INT; i++)
	{
		if(g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].pesPidChHandle == pidChHandle)
		{
			PrintDebug("[%s]pidchannel is found at %d\n", __FUNCTION__, i);
			break;
		}
	}

	/* find null pidchannel */
	if(i == P_REC_MAX_PID_PER_REC_INT)
	{
		PrintError("[%s]There is not matched pidchannel with previous info.\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* if found matched pidchannel, remove dsckeyhandle */
	PrintDebug("[%s]Remove pidChHandle: %x at %d\n", __FUNCTION__, pidChHandle, i);
	g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].pesPidChHandle = NULL;
	g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].dscKeyHandle = NULL;

	return DRV_OK;
}


void P_REC_ClearDscKeyslot(HUINT32 ulDeviceId)
{
	HUINT32 i;

	PrintDebug("[%s,%d] ulDeviceId: %d\n", __FUNCTION__, __LINE__, ulDeviceId);
	for(i=0; i<P_REC_MAX_PID_PER_REC_INT; i++)
	{
		g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].pesPidChHandle = NULL;
		g_astPvrRecInst[ulDeviceId].dscKeyInfo[i].dscKeyHandle = NULL;
	}
}
#endif





