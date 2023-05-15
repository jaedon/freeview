#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include "htype.h"
#include "drv_err.h"
#include "taskdef.h"
#include "vkernel.h"
#include "di_bt.h"

#define BUILDCFG
#include "bt_target.h"
#include "bsa_api.h"

#if defined(CONFIG_SHARED_COMPILE)
#include "nexus_platform_client.h"
#include "nexus_audio_capture.h"
#else
#include "di_uart.h"
#include "di_audio.h"
#include "drv_audio.h"
#endif


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
//#define DI_DEBUG
#if defined(CONFIG_SHARED_COMPILE)
#ifdef DI_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#endif
#else
#ifdef DI_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#endif
#endif

#define UNUSED(x) (void)(x)

/* Number of simultaneous A2DP connections supported */
#define BT_AV_DEVICE_MAX 1		/* Available upto 2  */


//#define HUMAX_AUDIO_RECORD
#ifdef HUMAX_AUDIO_RECORD
#define HUMAX_AUDIO_RECORD_FILE "/tmp/bt_audio_dump.pcm"
FILE* audio_fd;
#endif
#define BT_AV_QUEUE_SIZE 4
typedef struct
{
	HBOOL bInUse;
	HBOOL bIsStreamActive;
	tBSA_AV_HNDL handle;
	DI_BT_DEV_INFO_t stAvDevInfo;
}BT_AV_DEVICE_t;

typedef struct
{
	BT_AV_DEVICE_t stAvTable[BT_AV_DEVICE_MAX];
	tUIPC_CH_ID stUipcChannel;
	HUINT32 ulAutoPairTimer;
}BT_AV_t;

typedef struct
{
	HUINT32		ulTask;
	HUINT32		ulSema;
} DI_AV_INSTANCE_t;

typedef struct
{
	void* pvData;	/* PCM raw data */
	HUINT32 ulDataLen;	/* data length */
}BT_AV_PCM_t;

typedef struct
{
	BT_AV_PCM_t stQueue[BT_AV_QUEUE_SIZE];
	HUINT32 ulFront;
	HUINT32 ulRear;
}BT_AV_QUEUE;

static NEXUS_AudioCaptureHandle    hCapture = NULL;

static DI_AV_INSTANCE_t s_stAvInstance;
static BT_AV_QUEUE s_stBtAvQueue;
extern BT_AV_t s_stBtAvDevInfo;

/*******************************************************************/
/********************** Local Functions ****************************/
/*******************************************************************/
static DRV_Error P_BT_AV_PushAudioBuffer(void *pvBuffer, HUINT32 ulLength)
{
	DRV_Error drvResult = DRV_OK;
	HINT32		vResult = 0;

	vResult = VK_SEM_Get(s_stAvInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		drvResult = DRV_ERR;
		goto ret;
	}
	if( s_stBtAvQueue.ulFront == (s_stBtAvQueue.ulRear + 1) % BT_AV_QUEUE_SIZE )
	{
		PrintDebug("[%s] Audio Buffer Queue Overflow!\n", __FUNCTION__);
		VK_MEM_Memset(&s_stBtAvQueue, 0x0, sizeof(BT_AV_QUEUE));
		drvResult = DRV_OK;
		goto ret;
	}
	s_stBtAvQueue.ulRear = (s_stBtAvQueue.ulRear + 1) % BT_AV_QUEUE_SIZE;
	s_stBtAvQueue.stQueue[s_stBtAvQueue.ulRear].pvData = pvBuffer;
	s_stBtAvQueue.stQueue[s_stBtAvQueue.ulRear].ulDataLen = ulLength;

ret:
	VK_SEM_Release(s_stAvInstance.ulSema);
	return drvResult;
}

static DRV_Error P_BT_AV_PopAudioBuffer(BT_AV_PCM_t *pstAvPCM)
{
	DRV_Error drvResult = DRV_OK;
	HINT32		vResult = 0;

	vResult = VK_SEM_Get(s_stAvInstance.ulSema);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		drvResult = DRV_ERR;
		goto ret;
	}
	if( s_stBtAvQueue.ulFront == s_stBtAvQueue.ulRear )
	{
		PrintDebug("[%s] Audio Buffer Queue Underflow!\n", __FUNCTION__);
		drvResult = DRV_OK;
		goto ret;
	}
	s_stBtAvQueue.ulFront = (s_stBtAvQueue.ulFront + 1) % BT_AV_QUEUE_SIZE;
	VK_MEM_Memcpy(pstAvPCM , &s_stBtAvQueue.stQueue[s_stBtAvQueue.ulFront], sizeof(BT_AV_PCM_t));
	VK_MEM_Memset(&s_stBtAvQueue.stQueue[s_stBtAvQueue.ulFront], 0x0, sizeof(BT_AV_PCM_t));

ret:
	VK_SEM_Release(s_stAvInstance.ulSema);
	return drvResult;
}

static void P_BT_AV_UipcTxTask(void *pvParam)
{
	UNUSED(pvParam);
	HUINT32 index;
	BT_AV_PCM_t stParams_pcm;
	DRV_Error drvResult = DRV_OK;

	while (1)
	{
		VK_MEM_Memset(&stParams_pcm, 0x0, sizeof(BT_AV_PCM_t));
		drvResult = P_BT_AV_PopAudioBuffer(&stParams_pcm);
		if (drvResult != DRV_OK)
		{
			PrintError("[%s] P_BT_AV_PopAudioBuffer failed:%d\n", __FUNCTION__, drvResult);
			continue;
		}
		if(stParams_pcm.pvData == NULL || stParams_pcm.ulDataLen == 0)
		{
			/* PrintDebug("[%s] Invaild Audio Buffer\n", __FUNCTION__); */
			VK_TASK_Sleep(10);
			continue;
		}
		for (index = 0; index < BT_AV_DEVICE_MAX; index++)
		{
			if(s_stBtAvDevInfo.stAvTable[index].bInUse && s_stBtAvDevInfo.stAvTable[index].bIsStreamActive)
			{
				P_BT_AV_SendAudioBuffer(s_stBtAvDevInfo.stUipcChannel, stParams_pcm.pvData, stParams_pcm.ulDataLen);
			}
		}
	}
}

static void P_BT_AV_Capture_Callback(void *pParam, int param)
{
	NEXUS_AudioCaptureHandle capture = pParam;
	UNUSED(param);
	NEXUS_Error errCode;
	DRV_Error drvResult = DRV_OK;

	for(;;)
	{
		void *pBuffer;
		size_t bufferSize;
		/* Check available buffer space */
		errCode = NEXUS_AudioCapture_GetBuffer(capture, (void **)&pBuffer, &bufferSize);
		if ( errCode )
		{
			PrintError("[%s]Error getting capture buffer\n",__FUNCTION__);
			NEXUS_AudioCapture_Stop(capture);
			return;
		}

		if ( bufferSize > 0 )
		{
#ifdef HUMAX_AUDIO_RECORD
			size_t writeSize = 0;

			writeSize = fwrite(pBuffer, bufferSize, 1, audio_fd);
			if(writeSize <= 0)
			{
				PrintError("[%s] fwrite error!!\n",__FUNCTION__);
			}
#endif
			drvResult = P_BT_AV_PushAudioBuffer(pBuffer, bufferSize);
			if (drvResult != DRV_OK)
			{
				PrintError("[%s] P_BT_AV_PushAudioBuffer failed\n", __FUNCTION__);
				return;
			}

			/*PrintError("[%s]Data callback - Wrote %d bytes\n",__FUNCTION__, (int)bufferSize); */
			errCode = NEXUS_AudioCapture_ReadComplete(capture, bufferSize);
			if ( errCode )
			{
				PrintError("[%s] Error committing capture buffer\n", __FUNCTION__);
				NEXUS_AudioCapture_Stop(capture);
				return;
			}
		}
		else
		{
			break;
		}
	}
}

static void P_BT_AV_SampleRate_Callback(void *context, int param)
{
	NEXUS_AudioCaptureHandle capture = context;
	UNUSED(param);
	DRV_Error drvResult = DRV_OK;
	PrintDebug("[%s] entered\n", __FUNCTION__);

	NEXUS_AudioCaptureStatus captureStatus;
	NEXUS_AudioCapture_GetStatus(capture,&captureStatus);

	PrintDebug("[%s] sampling frequency%d\n", __FUNCTION__,captureStatus.sampleRate);

	drvResult = P_BT_AV_Stop();
	if (drvResult == DRV_OK)
	{
		PrintDebug("[%s] P_BT_AV_STOP Completed\n", __FUNCTION__);
	}

	VK_TASK_Sleep(100);

	drvResult = P_BT_AV_Start();
	if (drvResult != DRV_OK)
	{
		PrintError("[%s] P_BT_AV_Start failed:%d \n", __FUNCTION__, drvResult);
	}

}

/*******************************************************************/
/********************** External Functions *************************/
/*******************************************************************/
DRV_Error P_BT_Audio_Init(void)
{
	HINT32	vResult = DRV_OK;
	VK_MEM_Memset(&s_stAvInstance, 0x00, sizeof(s_stAvInstance));

	vResult = VK_SEM_Create((unsigned long*)&s_stAvInstance.ulSema, "BT_AV", VK_SUSPENDTYPE_FIFO);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Create failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Create(P_BT_AV_UipcTxTask, BT_TASK_PRIORITY, BT_TASK_STACK_SIZE, "BtAvUipcTxTask", (void *)0, (unsigned long*)&s_stAvInstance.ulTask, FALSE);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_TASK_Create failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Start(s_stAvInstance.ulTask);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_TASK_Start failed!\n",__FUNCTION__);
		return DRV_ERR;
	}
	return vResult;
}

void P_BT_NEXUS_Init(void)
{
	/*
	* Only necessary for Multi Process to join Nexus Server
	*/
	/* TODO */
	(void)NEXUS_Platform_Join();

#if defined(DI_DEBUG)
	const char *modestr[] = {"untrusted", "protected", "unprotected", "invalid"};
	NEXUS_ClientConfiguration clientConfig;
	NEXUS_Platform_GetClientConfiguration(&clientConfig);
	PrintDebug("[%s]%s unauthenticated client joined\n",__FUNCTION__, modestr[clientConfig.mode]);
	/* print client heaps */
	for (int i=0;i<NEXUS_MAX_HEAPS;i++)
	{
		NEXUS_MemoryStatus status;
		NEXUS_HeapHandle heap;
		heap = clientConfig.heap[i];
		if (!heap) break;
		eNError = NEXUS_Heap_GetStatus(heap, &status);
		PrintDebug("client heap[%d]: MEMC %d, offset %#x, addr %p, size %d (%#x), alignment %d\n",
			i, status.memcIndex, status.offset, status.addr, status.size, status.size, status.alignment);
	}
#endif
}

DRV_Error P_BT_NEXUS_AudioCapture_GetHandle(HUINT32 ulDevId)
{
	if( NULL == hCapture)
	{
#if defined(CONFIG_SHARED_COMPILE)
		hCapture = NEXUS_AudioCapture_GetHandle(ulDevId);
#else
		DRV_AUDIO_GetCaptureHandle(ulDevId, &hCapture);
#endif
		if ( NULL == hCapture)
		{
			PrintError("[%s] Unable to Get Capture Handle\n", __FUNCTION__);
			return DRV_ERR;
		}
	}
	else
	{
		PrintDebug("[%s] AudioCapture %p is already open\n", __FUNCTION__, hCapture);
	}

	return DRV_OK;
}


void P_BT_NEXUS_AudioCapture_Start(void)
{
	NEXUS_AudioCaptureStartSettings captureSettings;
	NEXUS_Error rc;

	/************* AUDIO CAPTURE CODE *********************/
	NEXUS_AudioCapture_GetDefaultStartSettings(&captureSettings);
	captureSettings.dataCallback.callback = P_BT_AV_Capture_Callback;
	captureSettings.dataCallback.context = hCapture;
	captureSettings.sampleRateCallback.callback = P_BT_AV_SampleRate_Callback;
	captureSettings.sampleRateCallback.context = hCapture;

	rc = NEXUS_AudioCapture_Start(hCapture, &captureSettings);
	if ( rc != NEXUS_SUCCESS)
	{
		PrintError("[%s]AudioCapture cannot start\n", __FUNCTION__);
	}
	else
	{
		PrintDebug("[%s]AudioCapture start\n", __FUNCTION__);
	}
	
	VK_MEM_Memset(&s_stBtAvQueue, 0x0, sizeof(BT_AV_QUEUE));

	return;
}

void P_BT_NEXUS_AudioCapture_Stop(void)
{
	NEXUS_AudioCapture_Stop(hCapture);
	VK_MEM_Memset(&s_stBtAvQueue, 0x0, sizeof(BT_AV_QUEUE));

	return;
}

HUINT32 P_BT_NEXUS_AudioCapture_GetStatusSampleRate(void)
{
	NEXUS_AudioCaptureStatus captureStatus;
	NEXUS_AudioCapture_GetStatus(hCapture,&captureStatus);

	return captureStatus.sampleRate;
}

