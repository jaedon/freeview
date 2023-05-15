/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_hls.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		Yang Hyun Uk $
 * Current Author:	$Author: huyang@humaxdigital.com $
 * Date:			$Date: 2011.08.06
 * File Description:	HTTP Live Streaming
 * Module:
 * Remarks:
 */

/**
 * @defgroup HLS_IPTUNER HLS IPTUNER
 * @ingroup HLS
 *
 */

/**
 * @author Hyunuk Yang(huyang@humaxdigital.com)
 * @date 6 august 2011
 */

/**
 * @note
 * Copyright (C) 2011 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file di_iptuner_http_live_streaming.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>
#include <fcntl.h>
#include "htype.h"
#include "vkernel.h"
#include "di_iptuner.h"
#include "di_iptuner_config.h"
#include "di_iptuner_common.h"
#include "di_iptuner_hls.h"
#include "di_iptuner_stream.h"
#include "di_uart.h"

#include "stream_http.h"
#include "taskdef.h"

#include "sstr_buffermgr.h"

#include "hls_controller.h"
#include "hls_m3u8_parser.h" /* remove xxx */

#include "drv_crypt.h"
#include "nexus_dma.h"
#include "nexus_memory.h"
#include "nexus_security.h"
#include "di_video.h"
#include "di_audio.h"


/* End Including Headers */
// typedef void* HLS_HANDLE;

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
//#define DI_IPTUNER_HLS_DEBUG
//#define USE_HLS_FAKEKEYS_FOR_TEST
//#define HLS_CONFIG_WRITE_SEGMENT_FOR_TEST

#define USE_HLS_SEEK_IMPROVEMENT // 주석 제거 시, seek 개선 동작으로 변경 가능

#ifdef IPTUNER_ALL_DEBUG
#ifdef DI_IPTUNER_HLS_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#define PrintError		DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* END DEBUG DEFINE */

#ifdef DI_IPTUNER_HLS_CRITICAL_DEBUG
#define PrintCriticalDebug      DI_UART_Print
#else
#define PrintCriticalDebug      while (0) ((int (*)(char *, ...)) 0)
#endif


#define UNUSED_PARAM(x)  ((void)x)
#define HLS_CONST_MAX_TIMEGAP_FOR_CHECK_LIVE 60

#define MINIMUM_PARTIAL_SEGMENT_SIZE 3008
#define MINIMUB_DOWNLOAD_COUNT 70 // 이전 값 30.... // 15 : 고화질( 1080i ) play 불가.
#define PARTIAL_SEGMENT_SIZE MINIMUM_PARTIAL_SEGMENT_SIZE * MINIMUB_DOWNLOAD_COUNT // jwji : 16의 배수 & 188의 배수. 3008이 최소 다운로드 사이즈 // x50은 느림 그래서 x150으로
#define ENCRYPTED_SEGMENT_BLOCK 16

/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

typedef enum
{
	HLS_IO_TYPE_NONE,
	HLS_IO_TYPE_PROBE,
	HLS_IO_TYPE_PLAYBACK,
} HLS_IO_TYPE_e;

typedef struct HLS_MEM_FILE_s {
	HBOOL bIsOpened;
    HUINT8 *buf;
    size_t buf_len;
    size_t length;
    size_t offset;
} HLS_MEM_FILE_t;

/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
/* Start static function */
#ifdef USE_HLS_SEEK_IMPROVEMENT
static HBOOL HexString2Binary(HINT8 *pszHexStr, HUINT8* pucBinStr, HINT32 nBinSize);
#endif
int P_STREAM_HLS_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle );


static size_t mem_file_write(HLS_MEM_FILE_t *file,const void *ptr, size_t size)
{
/*    BDBG_ASSERT(file->offset + size<= file->buf_len); */
	if(file == NULL)
	{
		return -1;
	}
	if(!file->bIsOpened)
	{
		return -1;
	}
	if(file->offset + size > file->buf_len)
	{
		return -1;
	}
    VK_MEM_Memcpy(file->buf + file->offset, (void *)ptr, size);
    file->offset += size;
    if(file->length<file->offset) {
        file->length = file->offset;
    }
    return size;
}

static size_t mem_file_read(HLS_MEM_FILE_t *file, void *ptr, size_t size)
{
	if(file == NULL)
	{
		return -1;
	}
	if(!file->bIsOpened)
	{
		return -1;
	}
    if(file->offset + size > file->buf_len)
	{
		return -1;
	}
    VK_MEM_Memcpy(ptr,file->buf + file->offset, size);
    file->offset += size;
    return size;
}

static size_t mem_file_tell(HLS_MEM_FILE_t *file)
{
	if(file == NULL)
	{
		return -1;
	}
	if(!file->bIsOpened)
	{
		return -1;
	}
    return file->offset;
}

static size_t mem_file_seek(HLS_MEM_FILE_t *file, size_t offset)
{
	if(file == NULL)
	{
		return -1;
	}
	if(!file->bIsOpened)
	{
		return -1;
	}
	if(offset > file->buf_len)
	{
		return -1;
	}
    file->offset = offset;
    return offset;
}

static HLS_MEM_FILE_t* mem_file_open(size_t nLen)
{
	HLS_MEM_FILE_t* file = NULL;
	file = (HLS_MEM_FILE_t*)DD_MEM_Alloc(sizeof(HLS_MEM_FILE_t));
	if(file == NULL)
	{
		return NULL;
	}
	VK_MEM_Memset(file,0x00,sizeof(HLS_MEM_FILE_t));
	file->buf = DD_MEM_Alloc(nLen + HLS_CONST_PADDING);
	if(file->buf == NULL)
	{
		if(file != NULL)
		{
			DD_MEM_Free(file);
			file = NULL;
		}
		return NULL;
	}
	VK_MEM_Memset(file->buf,0x00,nLen + HLS_CONST_PADDING);
	file->length = 0;
	file->buf_len = nLen + HLS_CONST_PADDING;
	file->bIsOpened = TRUE;
	return file;
}

static HBOOL mem_file_close(HLS_MEM_FILE_t *file)
{
	if(file == NULL)
	{
		return FALSE;
	}
	if(!file->bIsOpened)
	{
		return FALSE;
	}
	if(file->buf != NULL)
	{
		DD_MEM_Free(file->buf);
		file->buf = NULL;
	}
	VK_MEM_Memset(file,0x00,sizeof(HLS_MEM_FILE_t));
	file->bIsOpened = FALSE;
	DD_MEM_Free(file);
	file = NULL;
	return TRUE;
}


static size_t mem_file_bound(HLS_MEM_FILE_t *file)
{
    return file->length;
}

static void _ultohex(HUINT64 ullDec, HUINT8 *pHex, HUINT32 unLen)
{
	HUINT32 unTemp = 0;
	for(HUINT32 i = unLen - 1 ; i > 0; i--)
	{
		unTemp = ullDec % 16;
		pHex[i] = unTemp;
		ullDec /= 16;
		if(ullDec <= 0)
		{
			break;
		}
		unTemp = ullDec % 16;
		pHex[i] |= unTemp << 4;
		ullDec /= 16;
		if(ullDec <= 0)
		{
			break;
		}
	}
#if 0
	PrintDebug("IV = ");
	for(HUINT32 i = 0; i < unLen ; i++)
	{
		PrintDebug("%x ,",pHex[i]);
	}
	PrintDebug("\n");
#endif
}

typedef struct HLS_s
{
#ifdef HLS_CONFIG_WRITE_SEGMENT_FOR_TEST
		/* for Test */
		HINT32 fd;
#endif

	HLS_CTRL hCtrl;

	HINT8 *pInitSegUrl;
	HINT8 *pInitSegKeyUrl;
	HUINT8 *pSegBuff; /* Segment Buffer */
	HUINT32 ulBuffSize;
//	HINT8 *pDecInBuff; /* Decryption In Buffer (for Encrypted Stream) */
//	HINT8 *pDecOutBuff; /* Decryption Out Buffer (for Decrypted Stream) */
//	HUINT32 ulDecBuffSize;
	size_t sz_SegSize;

	HINT8 *pCurKeyUrl;
	HUINT8 ucCurKey[512];
	HUINT8 *pCurIV;

	m3u8Method_e eEncMethod;
	m3u8EncPadding_e eEncPadding;

	HUINT64 ullTimeForSeek;
	HBOOL	bRequestedSeek;
	unsigned long ulSema;
	unsigned long ulLiveSema; /**< Live Streaming 지원을 위한 Semaphore */

	HBOOL bImmediatelyExit;
	off_t ullVirtureOffset;

	/* Live Update */
	HBOOL bLiveUpdateTaskInit; /**< Live Update Task ID */
	unsigned long ulLiveUpdateTaskId; /**< Live Update Task ID */
	HBOOL bIsPrebuffering; /**< Prebuffering Task가 시작한 것을 체크, 시작 되었으면 Live Update Task 시작함. */

	HBOOL	bTriggerUpdate;
	HBOOL	bForcedTriggerUpdate;
	HUINT32 unProgramIdxForUpdate;
	HUINT32 unStreamIdxForUpdate;
	HUINT32 unDelayForUpdate;
	HBOOL  bNeedLiveRecovery; /**< Recovery needed in LiveStreaming */
#ifdef CONFIG_HLS_LIVE_RESTART
	HBOOL  bOnLiveRestart; /**< Live Restart 상태: LiveStreaming에서 현재 segment 보다 뒤의 것을 play하는 상태  */
	HUINT32 unProbingBandwidth; /**< Prob시 사용된 stream의 Bandwidth */
	HUINT32 unProbingStreamIndex; /**< Prob시 사용된 stream의 Index  */
	HUINT64 unProbingSegmentID; /**< Prob시 사용된 segment의  id  */
#endif

	DI_IPTUNER_HLS_LIVE_UPDATE_e eLiveUpdateStatus; /**< Live Update Status */

	/* Probing */
	HLS_IO_TYPE_e eIO_Type;
	HLS_MEM_FILE_t *pMemFd;

	unsigned long ulDownloadingSema;	// segment를 다운로드 하는 중 list update를 막기 위함. 기존 duration을 이용하여 list update를 막고있었으나 download 속도가 느리다면 segment 다운로드중 list update가 발생할 때가 있다.
#if 1 // __SUPPORT_BYTERANGE__
	STREAM_ACCESS_t *pSegmentAccess;					/**< 하나의 segment 에 대한 stream access */
#endif
} HLS_t;
/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
extern DI_IPTUNER_CustomCallback_t g_tCustomCallbacks;
//extern NEXUS_DmaHandle g_ulDmaHandle;
HBOOL g_isChangedTs = FALSE;
HUINT32 g_uDeviceId = 0;
DI_VIDEO_FORMAT g_eVideoFormat = 0;
DI_AUDIO_FORMAT g_eAudioFormat = 0;

/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */

static HLS_HANDLE g_hHLS = NULL;
static DI_IPTUNER_HLS_BUFFERING_TASK_STATE_e g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_DONE;
static DI_IPTUNER_HLS_BUFFERING_TASK_STATE_e g_ePreBufferingTaskChangeState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_DONE;

/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
#if __HLS_STATIC_FUNC__
#endif
static inline HLS_t* __getHLS(HLS_HANDLE hHls) { return (HLS_t*)hHls; }
#if 0
static HLS_ERROR_e P_STREAM_HLS_SetHeuristics(HLS_HANDLE hHls)
{
	HLS_t* pHLS = NULL;
	SSTR_HEURISTICS_Config_t tHeurConfig;
	SSTR_HEURISTICS_t *pHeuristics = NULL;
	SSTR_BUFFERMGR_t *pBufferMgr = NULL;
	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHls);
	pBufferMgr = HLS_CONTROLLER_GetBufferMgr(pHLS->hCtrl);
	if(pBufferMgr == NULL)
	{
		PrintError ("%s(%d) Fail! GetBufferMgr was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHeuristics = HLS_CONTROLLER_GetHeuristics(pHLS->hCtrl);
	if(pHeuristics == NULL)
	{
		PrintError ("%s(%d) Fail! GetHeuristicsHandle was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(SSTR_HEURISTICS_GetDefaultConfig(pHeuristics,SSTR_HEUR_CONFIG_PRESET_HLS,&tHeurConfig))
	{
		PrintError ("%s(%d) Fail! GetDefaultConfig was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	tHeurConfig.hCtrl = pHLS->hCtrl;
	tHeurConfig.hBufferMgr = pBufferMgr;
	tHeurConfig.cbCurBitrate = HLS_CONTROLLER_GetCurBitrate;
	tHeurConfig.cbCurDuration = HLS_CONTROLLER_GetCurSegmentDuration;
	tHeurConfig.cbIsProtected = HLS_CONTROLLER_IsCurStreamEncrypted;
	tHeurConfig.cbGearUp = HLS_CONTROLLER_GearUp;
	tHeurConfig.cbGearDown = HLS_CONTROLLER_GearDown;
	tHeurConfig.cbGearSet = HLS_CONTROLLER_SetClosestBandwidth;
	tHeurConfig.cbGearLowest = HLS_CONTROLLER_GearLowest;
	tHeurConfig.cbGearHighest = HLS_CONTROLLER_GearHighest;

//	tHeurConfig.eStartup = SSTR_HEUR_STARTUP_LOWEST;
//	tHeurConfig.eDecision = SSTR_HEUR_DECISION_AUTO;

#if 0
	tHeurConfig.ulDefaultDuration = 10;
	tHeurConfig.ulDelayValue = 100;
	tHeurConfig.ulDecValue = 100;
	tHeurConfig.ulDelayLimitPerc = 120;
	tHeurConfig.ulBufferPositivePerc = 60;
	tHeurConfig.ulDelayPositivePerc = 90;
	tHeurConfig.ulEnterGreedyBufferPerc = 30;
	tHeurConfig.ulEnterPanicBufferPerc = 10;
	tHeurConfig.ulEnterPanicDelayPerc = 85;
	tHeurConfig.ulExitPanicBufferPerc = 20;
	tHeurConfig.ulExitPanicDelayPerc = 60;
#endif

	if(SSTR_HEURISTICS_Init(pHeuristics, &tHeurConfig))
	{
		PrintError ("%s(%d) Fail! Heuristics Init was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	PrintExit;
	return HLS_OK;
}
#endif

//#ifdef USE_HLS_SEEK_IMPROVEMENT
#if 0 // __SUPPORT_BYTERANGE__
/* Used for error recovery mainly */
static int int_hls_seek(STREAM_ACCESS_t *pAccess, off_t offset)
{
	static const int retry_count = 3;
	int ret = DI_IPT_SUCCESS;
	HINT32 nSeekRetryCount = 0;
	if( pAccess == NULL)
	{
		PrintError("######### [%s:%d] Critical Section: Handle is NULL\n",__FUNCTION__,__LINE__);
		ret = (-1);
		goto seek_done;
	}
	if( pAccess->info.i_size < offset )
	{
		ret = (-1);
		goto seek_done;
	}

	while( DI_IPT_SUCCESS != STREAM_HTTP_Seek ( pAccess , offset ))
	{
		nSeekRetryCount++;
		PrintError("%s(%d) Error> STREAM_HTTP_Seek fail... Retrying seek request (%d) times..\n"
		,__FUNCTION__
		,__LINE__
		,nSeekRetryCount);

#if 0 // Not supported in DI 1.0
		if (pHTuner->bProbeCancel == TRUE)
		{
			PrintError("%s (%d) Debug> PROBE Cancel !!\n",__FUNCTION__,__LINE__);
			ret = (-1);
			goto seek_done;
		}
		if(pHTuner->bExitRead == TRUE)
		{
			PrintError("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
			goto seek_done;
		}
		if(pAccess->b_immediately_exit == TRUE)
		{
			PrintError("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
			goto seek_done;
		}
#endif
		if(nSeekRetryCount > retry_count)
		{
			PrintError("%s(%d) Error> STREAM_HTTP_Seek Retrying fail. Last Retrying count(%d) ...\n"
				,__FUNCTION__
				,__LINE__
				,nSeekRetryCount);
			ret = (-1);
			goto seek_done;
		}
	}
seek_done:
	return ret;
}
#endif


#if 1 // __SUPPORT_BYTERANGE
static HLS_ERROR_e int_hls_open(HLS_t* pHLS ,STREAM_ACCESS_t *pSegmentAccess, HINT8 *pUrl);
static HLS_ERROR_e int_hls_seek(HLS_t* pHLS ,STREAM_ACCESS_t *pAccess, off_t offset, HINT32 max_attempt);
#endif

static HUINT64 P_STREAM_HLS_GetTimeForSeek(HLS_HANDLE hHls)
{
	HLS_t* pHLS = NULL;
	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHls);
	PrintExit;
	return pHLS->ullTimeForSeek;
}

static HLS_ERROR_e P_STREAM_HLS_SetTimeForSeek(HLS_HANDLE hHls, HUINT64 ullTime)
{
	HLS_t* pHLS = NULL;
	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHls);
	if(pHLS->bRequestedSeek == TRUE)
	{
		PrintExit;
		return HLS_FAIL;
	}
	VK_SEM_Get(pHLS->ulSema);
	pHLS->ullTimeForSeek = ullTime;
	VK_SEM_Release(pHLS->ulSema);
	PrintExit;
	return HLS_OK;
}


static HBOOL P_STREAM_HLS_IsSeekRequested(HLS_HANDLE hHls)
{
	HLS_t* pHLS = NULL;
//	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHls);
//	PrintError("%s(%d) Seek Request : %d\n",__FUNCTION__,__LINE__,pHLS->bRequestedSeek);
//	PrintExit;
	return pHLS->bRequestedSeek;
}

static HLS_ERROR_e P_STREAM_HLS_SetRequestSeek(HLS_HANDLE hHls, HBOOL bRequest)
{
	HLS_t* pHLS = NULL;
	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHls);
	VK_SEM_Get(pHLS->ulSema);
	pHLS->bRequestedSeek = bRequest;
	VK_SEM_Release(pHLS->ulSema);
	PrintExit;
	return HLS_OK;
}

static HBOOL P_STREAM_HLS_IsValidTimeForSeek(HLS_HANDLE hHls, HUINT64 ullTime)
{
	HLS_t* pHLS = NULL;
	HFLOAT32 fStartTime = 0;
	HFLOAT32 fEndTime = 0;
//	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHls);
	fStartTime = HLS_CONTROLLER_GetCurStreamStartTime(pHLS->hCtrl);
	fEndTime = HLS_CONTROLLER_GetCurStreamEndTime(pHLS->hCtrl);
	PrintDebug("%s(%d) Avail Range %lf ~ %lf (Request %"PRId64")\n",__FUNCTION__,__LINE__,fStartTime, fEndTime, ullTime);
	if(fStartTime > ullTime || ullTime > fEndTime)
	{
		return FALSE;
	}
//	P_SSTR_CONTROLLER_Unlock(hSstr);
//	PrintExit;
	return TRUE;
}

#ifdef CONFIG_HLS_LIVE_RESTART
static HBOOL P_STREAM_HLS_CheckLiveRestart(HLS_HANDLE hHls, HUINT64 ullTime)
{
	HLS_t* pHLS = NULL;
	HFLOAT32 fEndTime = 0;
	HFLOAT32 fTimeGap = 0;
	HUINT32 unThreeTargetDuration = 0;
//	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHls);
	fEndTime = HLS_CONTROLLER_GetCurStreamEndTime(pHLS->hCtrl);
	fTimeGap = fEndTime - ullTime;
	unThreeTargetDuration = 3 * HLS_CONTROLLER_GetCurStreamTargetDuration(pHLS->hCtrl);

	PrintDebug("%s(%d) TimeGap(EndTime-SeekTime) %lf\t Three Target Duration %d\n",__FUNCTION__,__LINE__,fTimeGap, unThreeTargetDuration);

	if (unThreeTargetDuration == 0)
	{
		PrintError("%s(%d) Three Target Duration is Zero \n",__FUNCTION__,__LINE__);
		pHLS->bOnLiveRestart = FALSE;
//		PrintExit;
		return FALSE;
	}

	if(fTimeGap >= 0 && fTimeGap <= unThreeTargetDuration)
	{
		pHLS->bOnLiveRestart = FALSE;
		PrintDebug("%s(%d) OnLiveRestart ON\n",__FUNCTION__,__LINE__);
	}
	else
	{
		pHLS->bOnLiveRestart = TRUE;
		PrintDebug("%s(%d) OnLiveRestart OFF\n",__FUNCTION__,__LINE__);
	}

//	PrintExit;
	return TRUE;
}
#endif

static HINT32  P_STREAM_HLS_GetM3U8(HINT8 *pUrl, HINT8 **pM3U8Buff, HINT8 * pHostUrl)
{
	HINT64 	nSumLength=0;
	HINT32	nPos = 0;
	HINT32	nRet = DI_ERR_OK;
	HINT64  nContentLength = 0;
	HINT8 	*pBuffer=NULL;
	HINT32	nRetryStreamReadCount = 0;
	HINT32 	nRecvLength=0;
#if !HLS_CONFIG_MEMORY_PARSE
	HINT32 	nRc = -1;
	HINT32	flags = O_CREAT|O_TRUNC|O_RDWR|O_SYNC;
	HINT32	nFd = -1;
#endif
	STREAM_ACCESS_t tAccess;
	HUINT64 strSize = 0;
#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
	HINT32	nChunkFd = -1;
	HBOOL	isChunked = FALSE;

	/*******************************************
	 * Open Protocol Controller
	 * 	: After the pVfioHandle created,
	 * 			IPTUNER handle has to link with it
	 *******************************************/
	if(NULL == VK_strstr((const char*)pUrl,".m3u8"))
	{
		PrintError ("%s (%d) Error>  Invalid Url for HLS \n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		/* TODO : Resource check */
		return 0;
	}
	else
	{
		VK_memset(&tAccess,0,sizeof(STREAM_ACCESS_t));

#if SUPPORT_HUMAX_COOKIE
		VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
		UTIL_UrlParse(&tUrl, (const char *)pUrl, 0);
		if(tUrl.psz_cookie != NULL)
		{
			psCookie = VK_StrDup(tUrl.psz_cookie);
		}
		UTIL_UrlClean(&tUrl);
#endif

		tAccess.psz_path = VK_StrDup ((const char*)pUrl);
#if SUPPORT_HUMAX_COOKIE
		if(psCookie != NULL)
		{
			tAccess.psz_cookie = psCookie;
		}
#endif
		if(tAccess.psz_path == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);

			if(tAccess.psz_path != NULL)
			{
				DD_MEM_Free(tAccess.psz_path);
				tAccess.psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
			if(tAccess.psz_cookie != NULL)
			{
				DD_MEM_Free(tAccess.psz_cookie);
				tAccess.psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			PrintExit;
			return 0;
		}

		/* Try to open the STREAM */
		if (DI_IPT_SUCCESS != STREAM_HTTP_Open (&tAccess) )
		{
			if(tAccess.psz_path != NULL)
			{
				DD_MEM_Free(tAccess.psz_path);
				tAccess.psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
			if(tAccess.psz_cookie != NULL)
			{
				DD_MEM_Free(tAccess.psz_cookie);
				tAccess.psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			if(tAccess.psz_path != NULL)
			{
				DD_MEM_Free  (tAccess.psz_path);
				tAccess.psz_path =NULL;
			}
			/* else { continue; } */

			PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
			PrintExit;
			return 0;
		}
		if(pHostUrl != NULL)
		{
			strSize = VK_strlen(((HTTP_ACCESS_SYS_t*)tAccess.p_sys)->url.psz_host);
			VK_strncpy(pHostUrl, ((HTTP_ACCESS_SYS_t*)tAccess.p_sys)->url.psz_host, strSize);
			pHostUrl[strSize] = 0;
			PrintError("%s(%d) Host Url : %s\n",__FUNCTION__,__LINE__, pHostUrl);
		}
	}
	nContentLength = tAccess.info.i_size;

	if((HTTP_ACCESS_SYS_t*)tAccess.p_sys == NULL)
	{
		PrintError ("%s(%d) : Error> Invaild tAccess.p_sys \n",__FUNCTION__,__LINE__);
		return 0;
	}
	isChunked = ((HTTP_ACCESS_SYS_t*)tAccess.p_sys)->b_chunked;

	PrintDebug ("%s(%d): Open %s\n",__FUNCTION__,__LINE__,pUrl);
	PrintDebug ("%s(%d): nContentLength (%lld) chunked(%s)\n",__FUNCTION__,__LINE__,nContentLength, isChunked? "TRUE":"FALSE");

	if((nContentLength <= 0 || nContentLength == 0xFFFFFFFFFFFFFF)&& !isChunked)
	{
		PrintError ("%s(%d) : Error> STREAM_HTTP_Open Fail : Invalid contentLendth(%d) chunked(%d)\n",__FUNCTION__,__LINE__, nContentLength, isChunked? "TRUE":"FALSE");
		nContentLength = -1;
		return nContentLength;
	}
#if HLS_CONFIG_MEMORY_PARSE
	if(isChunked)
	{
		nChunkFd = open (HLS_CONFIG_TEMP_PATH, (O_CREAT|O_TRUNC|O_RDWR|O_SYNC), 0666);
		if(nChunkFd <0)
		{
			PrintError ("%s(%d) : Error> Manifest Cache File Open Fail %s\n",__FUNCTION__,__LINE__,HLS_CONFIG_TEMP_CACHE);
			return 0;
		}

		nContentLength = 0;
	}
	else
	{
		*pM3U8Buff = (HINT8*) DD_MEM_Alloc (nContentLength+HLS_CONST_PADDING);
		if(*pM3U8Buff==NULL)
		{
			PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%"PRId64") !!\n",__FUNCTION__,__LINE__,nContentLength+HLS_CONST_PADDING);
			return 0;
		}
		VK_memset(*pM3U8Buff,0x00,nContentLength+HLS_CONST_PADDING);
	}
#else
	nFd = open (HLS_CONFIG_TEMP_CACHE, flags, 0666);
	if(nFd <0)
	{
		PrintError ("%s(%d) : Error> Manifest Cache File Open Fail %s\n",__FUNCTION__,__LINE__,HLS_CONFIG_TEMP_CACHE);
		return 0;
	}
#endif
	/* heap allocation */
	pBuffer = (HINT8*) DD_MEM_Alloc (HLS_CONST_HTTP_RECV_BUFFSIZE);
	if(pBuffer==NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,HLS_CONST_HTTP_RECV_BUFFSIZE);
		return 0;
	}

	do
	{
		nRecvLength = 0;
		nRecvLength = STREAM_HTTP_Read ( &tAccess, (HUINT8 *)pBuffer, HLS_CONST_HTTP_RECV_BUFFSIZE );
		if(isChunked && tAccess.info.b_eof)
		{
#if HLS_CONFIG_MEMORY_PARSE
			*pM3U8Buff = (HINT8*) DD_MEM_Alloc (nSumLength+HLS_CONST_PADDING);
			VK_memset(*pM3U8Buff,0x00,nSumLength+HLS_CONST_PADDING);

			lseek(nChunkFd, 0, 0);
			nRecvLength = read(nChunkFd, (void*)(*pM3U8Buff), nSumLength);
			if(nRecvLength < 0)
			{
				PrintError ("%s(%d) : Error > read failed (%d, %d)  !! \n",__FUNCTION__,__LINE__, nRecvLength, nSumLength);
				close(nChunkFd);
				return 0;
			}
#else
			/* Nothing to do */
#endif
			break;
		}

		if(nRecvLength <= 0)
		{
			if(nRecvLength == 0 && tAccess.info.b_eof == TRUE)
			{
				nContentLength = nSumLength;
				break;
			}
			else
			{
				if(nRetryStreamReadCount < 3)
				{
					nRetryStreamReadCount++;
					PrintError("%s(%d) STREAM_HTTP_Read Fail!!\n",__FUNCTION__,__LINE__);
					continue;
				}
				else
				{
					nRetryStreamReadCount = 0;
					if(STREAM_HTTP_Seek(&tAccess,tAccess.info.i_pos) != 0)
					{
						nSumLength = HLS_FAIL;
						PrintError("%s(%d) Reconnection Fail!!\n",__FUNCTION__,__LINE__);
						goto __download_error;
					}
					else
					{
						PrintDebug("%s(%d) Reconnectedl!!\n",__FUNCTION__,__LINE__);
						continue;
					}
				}
			}
		}
		/* normal case */
		else
		{
#if HLS_CONFIG_MEMORY_PARSE
			if(isChunked)
			{
				nRet = write(nChunkFd, pBuffer, nRecvLength);
				if(nRet <= 0)
				{
					PrintError ("%s(%d) : Error > write failed (%d, %d)  !! \n",__FUNCTION__,__LINE__, nRecvLength, nSumLength);
					close(nChunkFd);
					return 0;
				}
				nSumLength+=nRecvLength;
			}
			else
			{
				if(nContentLength < nPos + nRecvLength)
				{
					PrintError ("%s(%d) : Error > BufferOverFlow !!Content : %lld, Pos : %d, RecvLength : %d \n",__FUNCTION__,__LINE__,nContentLength,nPos,nRecvLength);
					VK_MEM_Memcpy(*pM3U8Buff+nPos, pBuffer, (nPos+nRecvLength) - nContentLength);
					break;
				}
				else
				{
					VK_MEM_Memcpy(*pM3U8Buff+nPos, pBuffer, nRecvLength);
					nPos+=nRecvLength;
					nSumLength+=nRecvLength;
				}
			}
#else
			nRc = write (nFd, pBuffer, nRecvLength);
			nPos+=nRecvLength;
			nSumLength+=nRecvLength;
#endif
			usleep(0);
		}
	}
	while (nSumLength < nContentLength || isChunked);

__download_error:

	if(pBuffer != NULL)
	{
		VK_MEM_Free(pBuffer);
		pBuffer = NULL;
	}

	if(nChunkFd > 0)
	{
		close(nChunkFd);
	}

#if HLS_CONFIG_MEMORY_PARSE
#else
	close(nFd);
#endif
//	PrintDebug ("%s(%d) : Content : %lld, Pos : %d, RecvLength : %d \n",__FUNCTION__,__LINE__,nContentLength,nPos,nSumLength);
	(void) STREAM_HTTP_Close ( &tAccess );
	tAccess.b_alive = FALSE;
	if(tAccess.psz_path != NULL)
	{
		DD_MEM_Free  (tAccess.psz_path);
		tAccess.psz_path =NULL;
	}
	/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
	if(tAccess.psz_cookie != NULL)
	{
		DD_MEM_Free(tAccess.psz_cookie);
		tAccess.psz_cookie =NULL;
	}
	/* else { continue; } */
#endif
	nContentLength = nSumLength;
	return nContentLength;
}

#if 1 /* disable DECRYPTION based on IPTUNER */
static HLS_ERROR_e P_STREAM_HLS_ConfigureKeys(NEXUS_KeySlotHandle KeyHandle,
						  const HUINT8 * pKey, const HUINT8 * pIv, m3u8Method_e eMethod)
{
	HINT32 nKeySize = 0;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;

	if(pKey == NULL)
	{
		return HLS_FAIL;
	}

	/* Configure Algorithm */
    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);

    switch(eMethod)
    {
    	case M3U8_METHOD_UNKNOWN:
    	case M3U8_METHOD_AESCBC128:
    		nKeySize = 16;
    		algConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
    		algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
#if defined(CONFIG_SES_HDPLUS_SMARTTV)
		case M3U8_METHOD_NONE:
			return HLS_FAIL;
#endif //CONFIG_SES_HDPLUS_SMARTTV
    }
    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
	algConfig.keyDestEntryType 	= NEXUS_SecurityKeyType_eEven;
    algConfig.operation = NEXUS_SecurityOperation_eDecrypt;
    if ( NEXUS_Security_ConfigAlgorithm(KeyHandle, &algConfig)!=0 )
	{
		PrintError("\nConfigure Algorithn failed for EVEN key failed \n");
    }

	/* Load Key and IV  for EVEN */
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = nKeySize;
    VK_memcpy(key.keyData, pKey, key.keySize);
    if (NEXUS_Security_LoadClearKey(KeyHandle, &key) != 0)
	{
		PrintError("\nLoad EVEN key failed \n");
    }
    if(pIv != NULL)
    {
    	key.keyEntryType = NEXUS_SecurityKeyType_eIv;
	    key.keySize = nKeySize;
    	VK_memcpy(key.keyData, pIv, key.keySize);
		if (NEXUS_Security_LoadClearKey(KeyHandle, &key) != 0)
		{
			PrintError("\nLoad EVEN IV failed \n");
		}
	}
	return HLS_OK;
}

#if 0 /* for Test */
static HLS_ERROR_e P_STREAM_HLS_ConfigureKeysForEnc(NEXUS_KeySlotHandle KeyHandle,
						  HUINT8 * pKey, HUINT8 * pIv, m3u8Method_e eMethod)
{
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;
	HINT32 nKeySize = 0;

	if(pKey == NULL)
	{
		return HLS_FAIL;
	}
	/* Configure Algorithm */
    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
      switch(eMethod)
    {
    	case M3U8_METHOD_UNKNOWN:
    	case M3U8_METHOD_AESCBC128:
    		nKeySize = 16;
    		algConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
    		algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
    }
	algConfig.keyDestEntryType 	= NEXUS_SecurityKeyType_eEven;
//    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
	algConfig.operation = NEXUS_SecurityOperation_eEncrypt;
	if ( NEXUS_Security_ConfigAlgorithm(KeyHandle, &algConfig)!=0 )
	{
		PrintError("\nConfigure Algorithn failed for EVEN key failed \n");
	}

	/* Load Key and IV  for EVEN */
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	key.keySize = nKeySize;
	BKNI_Memcpy(key.keyData, pKey, key.keySize);
	if (NEXUS_Security_LoadClearKey(KeyHandle, &key) != 0)
	{
		PrintError("\nLoad Key failed \n");
	}
    if(pIv != NULL)
    {
    	key.keyEntryType = NEXUS_SecurityKeyType_eIv;
	    key.keySize = nKeySize;
    	BKNI_Memcpy(key.keyData, pIv, key.keySize);
		if (NEXUS_Security_LoadClearKey(KeyHandle, &key) != 0)
		{
			PrintError("\nLoad IV failed \n");
		}
    }
	return HLS_OK;
}
#endif

static HLS_ERROR_e P_STREAM_HLS_DMATransfer (HUINT8 * pIn, HUINT8 * pOut, HUINT32 size, NEXUS_DmaHandle hDma, NEXUS_KeySlotHandle hKey,HBOOL bResetCrypto)
{
	NEXUS_DmaJobSettings  dmaJobSetting;
	NEXUS_DmaSettings dmaSettings;
	NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_DmaJobHandle hDmaJob;
    NEXUS_DmaJobStatus jobStatus;

	dmaSettings.endianMode = NEXUS_DmaEndianMode_eLittle;
	dmaSettings.swapMode = NEXUS_DmaSwapMode_eNone;
	NEXUS_Dma_SetSettings (hDma, &dmaSettings);

	NEXUS_DmaJob_GetDefaultSettings (&dmaJobSetting);
	dmaJobSetting.numBlocks = 1;
	dmaJobSetting.completionCallback.callback = NULL;
	dmaJobSetting.dataFormat = NEXUS_DmaDataFormat_eBlock;
	dmaJobSetting.keySlot = hKey;
	if ( (hDmaJob = NEXUS_DmaJob_Create (hDma, &dmaJobSetting)) == NULL )
	{
		PrintError("%s(%d) Err> NEXUS_DmaJob_Create failed \n",__FUNCTION__,__LINE__);
		return HLS_FAIL;
	}

	NEXUS_DmaJob_GetDefaultBlockSettings (&blockSettings);
    blockSettings.pSrcAddr = pIn;
    blockSettings.pDestAddr = pOut;
    blockSettings.blockSize = size;
    blockSettings.resetCrypto = bResetCrypto;
    blockSettings.scatterGatherCryptoStart = true;
    blockSettings.scatterGatherCryptoEnd = true;

	NEXUS_DmaJob_SetBlockSettings(hDmaJob, 0, &blockSettings);

//	rc = NEXUS_DmaJob_Start(hDmaJob);

	NEXUS_DmaJob_ProcessBlocks(hDmaJob, &blockSettings, 1);

    jobStatus.currentState = NEXUS_DmaJobState_eUnknown;
    while (jobStatus.currentState != NEXUS_DmaJobState_eComplete)
    {
		/*BDBG_ERR(("Waiting... status=%d\n", jobStatus.currentState));*/
       NEXUS_DmaJob_GetStatus(hDmaJob, &jobStatus);
	   if ( jobStatus.currentState == NEXUS_DmaJobState_eComplete )
		   break;
		usleep(1);
    }
//	PrintDebug("P_STREAM_HLS_DMATransfer End\n");
	NEXUS_DmaJob_Destroy (hDmaJob);
	return HLS_OK;
}
#endif

#ifdef USE_HLS_SEEK_IMPROVEMENT
// !Caution! P_STREAM_HLS_Decryption() 함수는 drr_bcm, drr_bcm2 마다  다르다.
static HLS_ERROR_e P_STREAM_HLS_Decryption(HUINT8* pBuf, HINT32 nBufLen, HUINT8* pKey, HUINT8* pIv, m3u8Method_e eMethod, m3u8EncPadding_e eEncPadding, void *pDstBlock, HUINT8 *pSrcBlock, HINT32 nBlockLen)
{
	DI_CRYPT_CryptMode eCryptMode = DI_CRYPT_AES;
	HUINT32 unKeySize = 16;
	HUINT32 ulDecSize = 0;
	DRV_Error errCode = DRV_OK;
	NEXUS_Error nexusError;
//	HUINT8 *pSrcBlock = NULL;
//	HUINT8 *pDstBlock = NULL;
//	HINT32 nBlockLen = 0;
	switch(eMethod)
	{
		case M3U8_METHOD_UNKNOWN:
		case M3U8_METHOD_AESCBC128:
			unKeySize = 16;
			eCryptMode = DI_CRYPT_AES;
			if(eEncPadding == M3U8_ENCPADDING_UNKNOWN)
			{
				eEncPadding = M3U8_ENCPADDING_PKCS7;
			}
			break;
#if defined(CONFIG_SES_HDPLUS_SMARTTV)
		case M3U8_METHOD_NONE:
			NEXUS_Security_FreeKeySlot(KeyHandle);
			return 0;
#endif //CONFIG_SES_HDPLUS_SMARTTV

	}

	PrintDebug("%s(%d) Debug> In/Out Buffer Block Allocated %d * 2\n",__FUNCTION__,__LINE__,nBlockLen);
	PrintDebug("%s(%d) Debug> Enc Data Size %d\n",__FUNCTION__,__LINE__,nBufLen);
	if(pSrcBlock != NULL && pDstBlock != NULL)
	{
		HUINT32 unPadding = 0;
		HUINT32 unStartTick = 0;
		HUINT32 unEndTick = 0;
		unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		size_t szRemain = nBufLen;
		HINT32 nLen = nBlockLen;
		HINT32 nPos = 0;
		while(szRemain > 0)
		{
			if(szRemain < (HUINT32)nBlockLen)
			{
				nLen = szRemain;
			}
			VK_MEM_Memcpy(pSrcBlock, pBuf+nPos,nLen);
			//eErrRet = P_STREAM_HLS_DMATransfer(pSrcBlock,pDstBlock,nLen,g_ulDmaHandle,KeyHandle,bResetCrypto);
			errCode = DRV_CRYPT_EndecryptMedia(eCryptMode, pSrcBlock, nLen, pDstBlock, pKey, unKeySize, pIv, unKeySize, FALSE);
			if(errCode != DRV_OK)
			{
				PrintError("%s(%d) Err> Decryption failed \n",__FUNCTION__,__LINE__);
				break;
			}
			szRemain-=nLen;
			if(szRemain > 0 && eMethod == M3U8_METHOD_AESCBC128)
			{
				if(pIv != NULL)
				{
					VK_MEM_Memcpy(pIv,pBuf+nPos+nLen-unKeySize,unKeySize);
				}
			}
			VK_MEM_Memcpy(pBuf+nPos,pDstBlock,nLen);
			VK_MEM_Memset(pSrcBlock,0x00,nLen);
			VK_MEM_Memset(pDstBlock,0x00,nLen);
			nPos+=nLen;
			ulDecSize+=nLen;
			// bResetCrypto = FALSE;
		}
		if(eEncPadding == M3U8_ENCPADDING_PKCS7 && unKeySize > 0)
		{
			unPadding = ulDecSize % unKeySize;
			if(unPadding == 0)
			{
				unPadding = unKeySize;
			}
			if((ulDecSize - unPadding) > 0 && (HINT32)ulDecSize <= nBufLen)
			{
				for(HUINT32 i = ulDecSize - unPadding ; i < ulDecSize ; i++)
				{

					if(pBuf[i] != unPadding)
					{
						unPadding = 0;
						break;
					}
				}
			}
			if(unPadding != 0)
			{
				ulDecSize-=unPadding;
//				PrintDebug("[Padding! %u] %d -> %u \n",unPadding,nBufLen,ulDecSize); // REMOVE
			}
			else
			{
//				PrintDebug("[Did not Padding! %u] %d -> %u \n",unPadding,nBufLen,ulDecSize); // REMOVE
			}
		}
		unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		PrintDebug("%s(%d) Debug> Decryption %d Bytes Complete (Delay : %u)\n",__FUNCTION__,__LINE__,nPos,unEndTick - unStartTick);
	}
	else
		return HLS_DECRYPTION_FAIL;

	return HLS_OK;
}
#endif


#ifdef USE_HLS_SEEK_IMPROVEMENT
#else

#if 1 // __SUPPORT_BYTERANGE__
static HLS_ERROR_e P_HLS_DownloadCurSegment(HLS_t* pHLS, HUINT32 ulLimitDelay)
{
	HINT32 nRet = HLS_OK;
	HUINT64 ullSegment = 0;
	HINT8 *pUrl = NULL;
	HUINT32 unRetry=0;
//	STREAM_ACCESS_t tAccess;
	size_t sz_RemainSize=0;
	size_t sz_Real=0;
	size_t sz_RealTotal=0;
	size_t sz_Try = 0;
	HINT32 n_ReturnedValue = 0;
	HUINT32 unStartTick = 0;
	HUINT32 unTick = 0;
	STREAM_ACCESS_t *pSegmentAccess = NULL;

	HBOOL bByteRangeSupport = 0;
	HUINT64 ullByteRangeLength = 0;
	HUINT64 ullByteRangeOffset = 0;
	HUINT64 ullByteRangeIndex = 0; // For Debug

//	HUINT32 unEndTick = 0;
	PrintEnter;
	if(pHLS == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pHLS->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> HLS Controller is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
//	VK_memset(&tAccess,0x00,sizeof(STREAM_ACCESS_t));
	/* playqueue DATA 정리*/
	pUrl = HLS_CONTROLLER_GetCurSegmentUrl(pHLS->hCtrl);
	if( pUrl == NULL )
	{
		PrintData("%s(%d) HLS_NO_URL_TO_DOWNLOAD! it may be the end\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_NO_URL_TO_DOWNLOAD;
	}
	HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl,&ullSegment);

	bByteRangeSupport = HLS_CONTROLLER_GetCurSegmentByteRangeSupport(pHLS->hCtrl);
	ullByteRangeLength = HLS_CONTROLLER_GetCurSegmentByteRangeLength(pHLS->hCtrl);
	ullByteRangeOffset = HLS_CONTROLLER_GetCurSegmentByteRangeOffset(pHLS->hCtrl);
	ullByteRangeIndex = HLS_CONTROLLER_GetCurSegmentByteRangeIndex(pHLS->hCtrl);

	PrintDebug("%s(%d) : Download Segment (%"PRId64")  \n",__FUNCTION__,__LINE__,ullSegment);

	/* 	하나의 segment access 를 사용하여 의미없는 open 막음 */
	pSegmentAccess = pHLS->pSegmentAccess;

	if(pSegmentAccess == NULL)
	{
		pSegmentAccess = (STREAM_ACCESS_t*)DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
		pHLS->pSegmentAccess = pSegmentAccess;

		PrintError("%s(%d) Segment Access open for probe\n",__FUNCTION__,__LINE__);
		PrintError("%s(%d) : Segment pUrl \n\t\t[ %s ]\n",__FUNCTION__,__LINE__, pUrl);
		while((nRet = int_hls_open(pHLS, pSegmentAccess, pUrl)) != HLS_OK)
		{
			if(nRet != HLS_OK)
			{
				PrintError("STREAM OPEN error. \n");
				PrintExit;
				goto exit_stop;
			}
			PrintError("Unknown return error. So, Retry??\n");
		}
	}
	else
	{
		/* 이전 segment 와 현재 segment 의 url 이 다르거나, (ByteRange 인 경우)position 이 다른 경우, url 에 해당하는 Stream Http Open */
		if(VK_strcmp(pSegmentAccess->psz_path, pUrl) || pSegmentAccess->info.i_pos != ullByteRangeOffset)
		{
			PrintError("%s(%d) Old seg url [ %s ]\n",__FUNCTION__,__LINE__, pSegmentAccess->psz_path);
			PrintError("%s(%d) New seg url [ %s ]\n",__FUNCTION__,__LINE__, pUrl);
			PrintError("%s(%d) Old seg pos [ %llu ]\n",__FUNCTION__,__LINE__, pSegmentAccess->info.i_pos);
			PrintError("%s(%d) New seg pos [ %llu ]\n",__FUNCTION__,__LINE__, ullByteRangeOffset);

			if(VK_strcmp(pSegmentAccess->psz_path, pUrl))
			{
				PrintError("%s(%d) Segment URL is different from each other\n",__FUNCTION__,__LINE__);
			}
			if(pSegmentAccess->info.i_pos != ullByteRangeOffset)
			{
				PrintError("%s(%d) Segment Stream Access Position is different from each other\n",__FUNCTION__,__LINE__);
			}
			(void)STREAM_HTTP_Close (pSegmentAccess);

			if ( pSegmentAccess->psz_path != NULL )
			{
				DD_MEM_Free(pSegmentAccess->psz_path);
				pSegmentAccess->psz_path = NULL;
			}

			PrintError("%s(%d) SegmentAccess open\n",__FUNCTION__,__LINE__);
			while((nRet = int_hls_open(pHLS, pSegmentAccess, pUrl)) != HLS_OK)
			{
				if(nRet != HLS_OK)
				{
					PrintError("STREAM OPEN error. \n");
					PrintExit;
					goto exit_stop;
				}
				PrintError("Unknown return error. So, Retry??\n");
			}
		}
	}

	if(bByteRangeSupport == TRUE)	sz_RemainSize = ullByteRangeLength;
	else							sz_RemainSize = pSegmentAccess->info.i_size;

	/* pSegmentAccess 의 pos 변경  */
	if(bByteRangeSupport == TRUE)
	{
		/* Sequence 처리 시, 이전 segment 의 url & position 이 같다면, seek 하지 않는다.*/
		if(VK_strcmp(pSegmentAccess->psz_path, pUrl) || pSegmentAccess->info.i_pos != (ullByteRangeOffset))
		{
			nRet = int_hls_seek(pHLS, pSegmentAccess, ullByteRangeOffset, 0);
			if(HLS_OK != nRet)
			{
				PrintError("Changing position is fail\n");
				PrintExit;
				goto exit_stop;
			}
		}
	}

	if( sz_RemainSize > pHLS->ulBuffSize )
	{
		PrintError("%s(%d) : Error> Memory Reallocation.\n",__FUNCTION__,__LINE__);
		if(pHLS->pSegBuff != NULL)
		{
			DD_MEM_Free(pHLS->pSegBuff);
			pHLS->pSegBuff = NULL;
		}
		pHLS->pSegBuff = DD_MEM_Alloc(sz_RemainSize + HLS_CONST_PADDING);
		if(pHLS->pSegBuff == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			PrintExit;
			return HLS_FAIL;
		}
		pHLS->ulBuffSize = sz_RemainSize + HLS_CONST_PADDING;
	}
	VK_MEM_Memset(pHLS->pSegBuff, 0x00, pHLS->ulBuffSize);
	/* Download 시작*/

	do
	{
		/* 60KB 단위로 다운로드*/
		if(sz_RemainSize > HLS_CONST_RECV_BLOCKSIZE)
		{
			sz_Try = HLS_CONST_RECV_BLOCKSIZE;
		}
		else
		{
			sz_Try = sz_RemainSize;
		}

		sz_Real=0;

		do
		{
			if(P_STREAM_HLS_IsSeekRequested(pHLS))
			{
				nRet = HLS_DOWNLOAD_STOP_FORCE;
				break;
			}
			if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
			{
				PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
				nRet = HLS_DOWNLOAD_STOP_FORCE;
				break;
			}
			n_ReturnedValue = STREAM_HTTP_Read ( pSegmentAccess, pHLS->pSegBuff+sz_RealTotal, sz_Try - sz_Real );
			if(n_ReturnedValue <= 0)
			{
				if(unRetry >= HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
				{
					PrintError ("%s(%d) Stop!! STREAM_HTTP_Read Fail !!!\n",__FUNCTION__,__LINE__);
					nRet = HLS_FAIL;
					break;
				}
				PrintError ("%s(%d) STREAM_HTTP_Read Fail Retry...!!! \n",__FUNCTION__,__LINE__);
				unRetry++;
				VK_TASK_Sleep(HLS_CONST_WAITING);
				continue;
			}
			sz_Real += n_ReturnedValue;
			sz_RealTotal += n_ReturnedValue;
			unTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
			if( (unTick - unStartTick) >= ulLimitDelay)
			{
				if(HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) != HLS_CONTROLLER_GetCurMinBitrate(pHLS->hCtrl))
				{
					SSTR_HEURISTICS_Run(HLS_CONTROLLER_GetHeuristics(pHLS->hCtrl),sz_RealTotal,unTick - unStartTick);
					if(HLS_CONTROLLER_CheckHeuristicsPanic(pHLS->hCtrl,unTick - unStartTick))
					{
						PrintDebug("%s(%d) Exit Download (Network Bandwidth Panic)\n",__FUNCTION__,__LINE__);
						nRet = HLS_PANIC_DETECTED;
						break;
					}
					else
					{
						PrintError("%s(%d) Exit Download (Time Out)\n",__FUNCTION__,__LINE__);
						nRet = HLS_TIMEOUT;
						break;
					}
				}
			}
		}
		while (sz_Real < sz_Try);
		sz_RemainSize = sz_RemainSize - sz_Real;
		if( nRet != HLS_OK )
		{
			break;
		}
	}
	while(sz_RemainSize > 0);

exit_stop:
	if(sz_RemainSize == 0)
		pHLS->sz_SegSize = sz_RealTotal;

	PrintExit;
	return nRet;
}
#else
static HLS_ERROR_e P_HLS_DownloadCurSegment(HLS_t* pHLS, HUINT32 ulLimitDelay)
{
	HINT32 nRet = HLS_OK;
	HUINT64 ullSegment = 0;
	HINT8 *pUrl = NULL;
	HUINT32 unRetry=0;
	STREAM_ACCESS_t tAccess;
	size_t sz_RemainSize=0;
	size_t sz_Real=0;
	size_t sz_RealTotal=0;
	size_t sz_Try = 0;
	HINT32 n_ReturnedValue = 0;
	HUINT32 unStartTick = 0;
	HUINT32 unTick = 0;
//	HUINT32 unEndTick = 0;
	PrintEnter;
	if(pHLS == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pHLS->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> HLS Controller is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	VK_memset(&tAccess,0x00,sizeof(STREAM_ACCESS_t));
	/* playqueue DATA 정리*/
	pUrl = HLS_CONTROLLER_GetCurSegmentUrl(pHLS->hCtrl);
	if( pUrl == NULL )
	{
		PrintData("%s(%d) HLS_NO_URL_TO_DOWNLOAD! it may be the end\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_NO_URL_TO_DOWNLOAD;
	}
	HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl,&ullSegment);
	PrintDebug("%s(%d) : Download Segment (%"PRId64")  \n",__FUNCTION__,__LINE__,ullSegment);
	tAccess.psz_path = VK_StrDup((const char*)pUrl);
	if(tAccess.psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	/* */
	nRet = STREAM_HTTP_Open (&tAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	else
	{
		if( tAccess.info.i_size > pHLS->ulBuffSize )
		{
			PrintError("%s(%d) : Error> Memory Reallocation.\n",__FUNCTION__,__LINE__);
			if(pHLS->pSegBuff != NULL)
			{
				DD_MEM_Free(pHLS->pSegBuff);
				pHLS->pSegBuff = NULL;
			}
			pHLS->pSegBuff = DD_MEM_Alloc(tAccess.info.i_size + HLS_CONST_PADDING);
			if(pHLS->pSegBuff == NULL)
			{
				PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
				PrintExit;
				return HLS_FAIL;
			}
			pHLS->ulBuffSize = tAccess.info.i_size + HLS_CONST_PADDING;
		}
		VK_MEM_Memset(pHLS->pSegBuff, 0x00, pHLS->ulBuffSize);
		/* Download 시작*/

		/* 다운로드 받을 남은 사이즈를 입력*/
		sz_RemainSize = tAccess.info.i_size;

		do
		{
			/* 60KB 단위로 다운로드*/
			if(sz_RemainSize > HLS_CONST_RECV_BLOCKSIZE)
			{
				sz_Try = HLS_CONST_RECV_BLOCKSIZE;
			}
			else
			{
				sz_Try = sz_RemainSize;
			}

			sz_Real=0;

			do
			{
				if(P_STREAM_HLS_IsSeekRequested(pHLS))
				{
					nRet = HLS_DOWNLOAD_STOP_FORCE;
					break;
				}
				if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
					nRet = HLS_DOWNLOAD_STOP_FORCE;
					break;
				}
				n_ReturnedValue = STREAM_HTTP_Read ( &tAccess, pHLS->pSegBuff+sz_RealTotal, sz_Try - sz_Real );
				if(n_ReturnedValue <= 0)
				{
					if(unRetry >= HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
					{
						PrintError ("%s(%d) Stop!! STREAM_HTTP_Read Fail !!!\n",__FUNCTION__,__LINE__);
						nRet = HLS_FAIL;
						break;
					}
					PrintError ("%s(%d) STREAM_HTTP_Read Fail Retry...!!! \n",__FUNCTION__,__LINE__);
					unRetry++;
					VK_TASK_Sleep(HLS_CONST_WAITING);
					continue;
				}
				sz_Real += n_ReturnedValue;
				sz_RealTotal += n_ReturnedValue;
				unTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
				if( (unTick - unStartTick) >= ulLimitDelay)
				{
					if(HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) != HLS_CONTROLLER_GetCurMinBitrate(pHLS->hCtrl))
					{
						SSTR_HEURISTICS_Run(HLS_CONTROLLER_GetHeuristics(pHLS->hCtrl),sz_RealTotal,unTick - unStartTick);
						if(HLS_CONTROLLER_CheckHeuristicsPanic(pHLS->hCtrl,unTick - unStartTick))
						{
							PrintDebug("%s(%d) Exit Download (Network Bandwidth Panic)\n",__FUNCTION__,__LINE__);
							nRet = HLS_PANIC_DETECTED;
							break;
						}
						else
						{
							PrintError("%s(%d) Exit Download (Time Out)\n",__FUNCTION__,__LINE__);
							nRet = HLS_TIMEOUT;
							break;
						}
					}
				}
			}
			while (sz_Real < sz_Try);
			sz_RemainSize = sz_RemainSize - sz_Real;
			if( nRet != HLS_OK )
			{
				break;
			}
		}
		while(sz_RemainSize > 0);
	}
	pHLS->sz_SegSize = sz_RealTotal;
	(void)STREAM_HTTP_Close (&tAccess);
	if ( tAccess.psz_path != NULL )
	{
		VK_MEM_Free(tAccess.psz_path);
		tAccess.psz_path = NULL;
	}
	PrintExit;
	return nRet;
}
#endif

#endif

#if 1
HLS_ERROR_e P_HLS_ReadDataRequestSize(HLS_t* pHLS /* Could be NULL */,STREAM_ACCESS_t *pSegmentAccess, HUINT8 *p_buffer, size_t nSize)
{
// Download segment
	HINT32 nRet = HLS_OK;
	HUINT64 ullSegment = 0;
	HINT8 *pUrl = NULL;
	size_t sz_Read = 0;
	off_t cur = 0;
	size_t sz_RemainSize=0;
	size_t sz_Real=0;
	size_t sz_RealTotal=0;
	size_t sz_Try = 0;
	HUINT32 unStartTick = 0;
	HUINT32 unTick = 0;
	HUINT32 unEndTick = 0;
	HUINT64 ullRTD = 0;
	HUINT64 ullRTDCount = 0;
	HUINT32 ulBuffSize = 0;
	size_t sz_RealTotalForPlay = 0;
	m3u8_t* m3u8 = NULL;
	off_t offsetForSeek = 0;
	HUINT32 nTemp = 0;
	off_t partial_offsetForSeek = 0;
	HUINT32 ulRemainSize = 0;
	HUINT32 ulPartailSegSize = 0;
	HUINT32 ulLimitPartialDelayTemp = 0;
	HUINT32 ulLimitPartialDelay = 0;

// Network Connecting
	HINT32 n_ReturnedValue = 0;
	HINT32 nRetryStreamReadCount = 0;
	HINT32 nReConnectionTry = 0;

	if( pHLS == NULL )
	{
		PrintError("%s(%d) : Error> HLS Handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	sz_Try = nSize;

	do
	{
		if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
			goto exit_stop;
		}
		if(P_STREAM_HLS_IsSeekRequested(pHLS))
		{
			nRet = HLS_SEEK_REQUESTE;
			PrintExit;
			goto exit_stop;
		}

		n_ReturnedValue = STREAM_HTTP_Read ( pSegmentAccess, p_buffer+sz_RealTotal, sz_Try - sz_Real );

		/* Read Error : should be exit immediately */
		if (n_ReturnedValue == 0)
		{
			if(nRetryStreamReadCount < 3)
			{
				nRetryStreamReadCount++;
				PrintError("STREAM_HTTP_Read Fail!!\n");
				continue;
			}
			else
			{
				nRetryStreamReadCount = 0;
				nRet = int_hls_seek(pHLS, pSegmentAccess, pSegmentAccess->info.i_pos, 0);
				if(HLS_OK != nRet)
				{
					PrintError("Changing position is fail\n");
					PrintExit;
					goto exit_stop;
				}
				PrintError( "Position is changed\n");
				continue;
			}
		}
		else if (n_ReturnedValue < 0)
		{
			PrintError("STREAM_HTTP_Read Fail!!\n");
			nRet = int_hls_seek(pHLS, pSegmentAccess, pSegmentAccess->info.i_pos, 0);
			if(HLS_OK != nRet)
			{
				PrintError("Reconnection fail\n");
				PrintExit;
				goto exit_stop;
			}
			PrintError( "Reconnected!!\n");
			continue;
		}
		sz_Real += n_ReturnedValue;
		sz_RealTotal += n_ReturnedValue;

	}
	while (sz_Real < sz_Try);

	exit_stop:

	return nRet;
}
#endif

static HLS_ERROR_e P_HLS_DownloadKey(HINT8 *pUrl, HUINT8 *pRetrvKey, HUINT32 unKeyBufferSize, HINT32 *pnKeySize)
{
	HINT32 nRet = HLS_OK;
	HUINT32 unRetry=0;
	STREAM_ACCESS_t tAccess;
	size_t sz_RemainSize=0;
	size_t sz_Real=0;
	size_t sz_RealTotal=0;
	size_t sz_Try = 0;
	HINT32 n_ReturnedValue = 0;
	PrintEnter;
	if(pUrl == NULL)
	{
		PrintError("%s(%d) Error> Key URL is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pRetrvKey == NULL)
	{
		PrintError("%s(%d) Error> Key Buffer is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}

	VK_memset(&tAccess,0x00,sizeof(STREAM_ACCESS_t));
	tAccess.psz_path = VK_StrDup((const char*)pUrl);
	if(tAccess.psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	/* */
	nRet = STREAM_HTTP_Open (&tAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	else
	{
		/* Download 시작*/
		if( tAccess.info.i_size < unKeyBufferSize)
		{
			/* 다운로드 받을 남은 사이즈를 입력*/
			sz_RemainSize = tAccess.info.i_size;
			*pnKeySize = tAccess.info.i_size;
			do
			{
				/* 60KB 단위로 다운로드*/
				if(sz_RemainSize > HLS_CONST_RECV_BLOCKSIZE)
				{
					sz_Try = HLS_CONST_RECV_BLOCKSIZE;
				}
				else
				{
					sz_Try = sz_RemainSize;
				}

				sz_Real=0;

				do
				{
					n_ReturnedValue = STREAM_HTTP_Read ( &tAccess, pRetrvKey+sz_RealTotal, sz_Try - sz_Real );
					if(n_ReturnedValue <= 0)
					{
						if(unRetry >= HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
						{
							PrintError ("%s(%d) Stop!! STREAM_HTTP_Read Fail !!!\n",__FUNCTION__,__LINE__);
							nRet = HLS_FAIL;
							break;
						}
						PrintError ("%s(%d) STREAM_HTTP_Read Fail Retry...!!! \n",__FUNCTION__,__LINE__);
						unRetry++;
						VK_TASK_Sleep(HLS_CONST_WAITING);
						continue;
					}
					sz_Real += n_ReturnedValue;
					sz_RealTotal += n_ReturnedValue;
				}
				while (sz_Real < sz_Try);
				sz_RemainSize = sz_RemainSize - sz_Real;
				if( nRet != HLS_OK )
				{
					break;
				}
			}
			while(sz_RemainSize > 0);
		}
	}
	(void)STREAM_HTTP_Close (&tAccess);
	if ( tAccess.psz_path != NULL )
	{
		VK_MEM_Free(tAccess.psz_path);
		tAccess.psz_path = NULL;
	}
#if 0
	for(HUINT32 i = 0 ; i < *pnKeySize ; i++)
	{
		PrintDebug("0x%x",pRetrvKey+i);
	}
	PrintDebug("\n");
#endif
	PrintExit;
	return nRet;
}

#ifdef USE_NAGRAPRM
static HLS_ERROR_e P_HLS_GetKey_NAGRAPRM(HLS_HANDLE hHLS)
{
	HINT32 nRetry = 0;
	HINT32 nKeySize = 0;
	HINT8 *pKeyUrl = NULL;
	HLS_t* pHLS = NULL;
	DI_ERR_CODE errCode = DI_ERR_ERROR;


	if(hHLS == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHLS);
	pHLS->eEncMethod = HLS_CONTROLLER_GetCurKeyMethod(pHLS->hCtrl);
	pHLS->pCurIV = HLS_CONTROLLER_GetCurIV(pHLS->hCtrl);
	pHLS->eEncPadding = HLS_CONTROLLER_GetCurPaddingMethod(pHLS->hCtrl);
	pKeyUrl = HLS_CONTROLLER_GetCurKeyUrl(pHLS->hCtrl);
	if(g_tCustomCallbacks.eType == DI_IPTUNER_CUSTOM_NAGRAPRM && g_tCustomCallbacks.cbNAGRAPRMKeyRetrv != NULL)
	{
		do
		{
			g_tCustomCallbacks.cbNAGRAPRMKeyRetrv(pKeyUrl,pHLS->ucCurKey,&nKeySize,&errCode);
			if(errCode != DI_ERR_OK)
			{
				if(P_STREAM_HLS_IsSeekRequested(pHLS))
				{
					return HLS_DOWNLOAD_STOP_FORCE;
				}
				if(g_ePreBufferingTaskState >= DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
					return HLS_DOWNLOAD_STOP_FORCE;
				}
				if(nRetry >= HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
				{
					PrintError("%s(%d) Retrieve Key Fail. Break Download Task!!\n",__FUNCTION__,__LINE__);
					return HLS_RETRIEVE_KEY_FAIL;
				}
//				PrintError ("%s(%d) Retrieve Key Fail. Retry...%u !!! \n",__FUNCTION__,__LINE__,nRetry);
//				nRetry++;
//				VK_TASK_Sleep(HLS_CONST_WAITING);
			}

		}while(errCode != DI_ERR_OK);
	}

	return HLS_OK;
}
#endif

static HLS_ERROR_e P_HLS_GetKey(HLS_HANDLE hHLS)
{
	HINT32 nRetry = 0;
	HINT32 nKeySize = 0;
	HINT8 *pKeyUrl = NULL;
	HLS_t* pHLS = NULL;
	DI_ERR_CODE errCode = DI_ERR_ERROR;
	HLS_ERROR_e errHLS = HLS_FAIL;

	if(hHLS == NULL)
	{
		PrintError("%s(%d) Error> SSTR HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHLS);
	pHLS->eEncMethod = HLS_CONTROLLER_GetCurKeyMethod(pHLS->hCtrl);
	pHLS->pCurIV = HLS_CONTROLLER_GetCurIV(pHLS->hCtrl);
	pHLS->eEncPadding = HLS_CONTROLLER_GetCurPaddingMethod(pHLS->hCtrl);
	pKeyUrl = HLS_CONTROLLER_GetCurKeyUrl(pHLS->hCtrl);
	if(pHLS->pCurKeyUrl != NULL)
	{
		if(!VK_strcmp(pKeyUrl, pHLS->pCurKeyUrl))
		{
			return HLS_OK;
		}
	}
#ifdef USE_HLS_FAKEKEYS_FOR_TEST /* for TEST (MUST remove xxx) */
	return HLS_OK;
#endif
	if(g_tCustomCallbacks.eType == DI_IPTUNER_CUSTOM_VMX && g_tCustomCallbacks.cbVmxKeyRetrv != NULL)
	{
		while( errCode != DI_ERR_OK )
		{
			g_tCustomCallbacks.cbVmxKeyRetrv(pKeyUrl,pHLS->ucCurKey,&nKeySize,&errCode);
			if(errCode != DI_ERR_OK)
			{
				if(P_STREAM_HLS_IsSeekRequested(pHLS))
				{
					return HLS_DOWNLOAD_STOP_FORCE;
				}
				if(g_ePreBufferingTaskState >= DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
					return HLS_DOWNLOAD_STOP_FORCE;
				}
				if(nRetry >= HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
				{
					PrintError("%s(%d) Retrieve Key Fail. Break Download Task!!\n",__FUNCTION__,__LINE__);
					return HLS_RETRIEVE_KEY_FAIL;
				}
				PrintError ("%s(%d) Retrieve Key Fail. Retry...%u !!! \n",__FUNCTION__,__LINE__,nRetry);
				nRetry++;
				VK_TASK_Sleep(HLS_CONST_WAITING);
			}
			else
			{
				errCode = DI_ERR_OK;
				break;
			}
		}
	}
#ifdef USE_NAGRAPRM
	else if(g_tCustomCallbacks.eType == DI_IPTUNER_CUSTOM_NAGRAPRM && g_tCustomCallbacks.cbNAGRAPRMKeyRetrv!= NULL)
	{
			return P_HLS_GetKey_NAGRAPRM(hHLS);
	}
#endif //USE_NAGRAPRM
	else
	{
		while( errHLS != HLS_OK )
		{
			errHLS = P_HLS_DownloadKey(pKeyUrl,pHLS->ucCurKey,sizeof(pHLS->ucCurKey),&nKeySize);
			if( errHLS != HLS_OK)
			{
				if(P_STREAM_HLS_IsSeekRequested(pHLS))
				{
					return HLS_DOWNLOAD_STOP_FORCE;
				}
				if(g_ePreBufferingTaskChangeState >= DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
					return HLS_DOWNLOAD_STOP_FORCE;
				}

				if(nRetry >= HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
				{
					PrintError("%s(%d) Retrieve Key Fail. Break Download Task!!\n",__FUNCTION__,__LINE__);

					#if 0 // Because there is no procedure to cover error routine.
					(void)mem_file_close(pHLS->pMemFd);
					pHLS->pMemFd = NULL;
					#endif
					return HLS_RETRIEVE_KEY_FAIL;
				}

				PrintError ("%s(%d) Retrieve Key Fail. Retry...%u !!! \n",__FUNCTION__,__LINE__,nRetry);
				nRetry++;
				VK_TASK_Sleep(HLS_CONST_WAITING);
			}
		}
	}
#if 0
	for(HINT32 i = 0 ; i < 16 ; i++)
	{
		PrintDebug("%x ",pHLS->ucCurKey[i]);
	}
	PrintDebug("\n\n");
	for(HINT32 i = 0 ; i < 16 ; i++)
	{
		PrintDebug("%c ",pHLS->ucCurKey[i]);
	}
	PrintDebug("\n\n");
#endif
	pHLS->pCurKeyUrl = pKeyUrl;
	return HLS_OK;
}

#ifdef USE_HLS_SEEK_IMPROVEMENT
#if 1 // __SUPPORT_BYTERANGE
static HLS_ERROR_e P_HLS_DownloadWithPlay(off_t offsetForSeek, HUINT32 ulTimeForSeek, void *hDec, HUINT8 *pSrcBlock, HINT32 nBlockLen )
{
	HINT32 nWriteSize=0;
	HINT32 nRet = HLS_OK;
	HUINT64 ullSegment = 0;
	HINT8 *pUrl = NULL;
	size_t sz_Read = 0;
	off_t cur = 0;
	STREAM_ACCESS_t *pSegmentAccess = NULL;
	size_t sz_RemainSize=0;
	size_t sz_Real=0;
	size_t sz_RealTotal=0;
	size_t sz_Try = 0;
	HINT32 n_ReturnedValue = 0;
	HUINT32 unStartTick = 0;
	HUINT32 unTick = 0;
	HUINT32 unEndTick = 0;
	HUINT64 ullRTD = 0;
	HUINT64 ullRTDCount = 0;
	HUINT32 unStartDecryptionFuncTick = 0;
	HUINT32 unEndDecryptionFuncTick = 0;
	SSTR_BUFFERMGR_t* pBufferMgr = NULL;
	SSTR_HEURISTICS_t* pHeuristics = NULL;
	HLS_t* pHLS = NULL;

	HUINT32 nTemp = 0;
	off_t partial_offsetForSeek = 0;

	HUINT8 *pSegBuff = NULL;
	HUINT8 *pEncBuff = NULL;
	HUINT32 ulBuffSize = 0;
	size_t sz_RealTotalForProbe = 0;

	HUINT64 ullSegSize = 0;

	HINT32 nPartialConut = 0;

	HUINT32 ulDuration = 0;

	HUINT32 ulRemainSize = 0;
	HUINT32 ulPartailSegSize = 0;
	HUINT32 ulLimitPartialDelayTemp = 0;
	HUINT32 ulLimitPartialDelay = 0;


#ifdef USE_HLS_FAKEKEYS_FOR_TEST /* for TEST (MUST remove xxx) */
	HUINT8 fakekeys[16] = "0123456789abcdef";
#endif
	HUINT8 emptyiv[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	HUINT32 unDecSize = 0;


#if 0
	static const HUINT32 HLS_RETAIN_THRESHOLD = 80;		 /* percentage of total buffer size*/
	static const HUINT32 HLS_RETAIN_BLOCK_AT_ONCE = 30; /* percentage of the number of blocks */
#endif
	HUINT8 lastEncryptedData[ ENCRYPTED_SEGMENT_BLOCK ] = { 0, };
	HUINT8 encryptedData[ ENCRYPTED_SEGMENT_BLOCK ] = { 0, };
	HUINT8 firstEncryptedData[ ENCRYPTED_SEGMENT_BLOCK ] = { 0, };
	HUINT32 unUsingFirstEncryptedData = 0;
	HBOOL bEncryptedSegment = FALSE;
	HBOOL bDownloadASegment = FALSE;
	HBOOL bDownloadFirstPartialSegment = TRUE;

// ByteRangeSupport
	HBOOL bByteRangeSupport = 0;
	HUINT64 ullByteRangeLength = 0;
	HUINT64 ullByteRangeOffset = 0;
	HUINT64 ullByteRangeIndex = 0; // For Debug

	PrintEnter;

	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return HLS_FAIL;
	}
	pHLS = __getHLS(g_hHLS);
	pBufferMgr = HLS_CONTROLLER_GetBufferMgr(pHLS->hCtrl);
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> pBufferMgr is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pHLS->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> HLS Controller is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHeuristics = HLS_CONTROLLER_GetHeuristics(pHLS->hCtrl);
	if(pHeuristics == NULL)
	{
		PrintError("%s(%d) Error> pHeuristics is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}

#if 0
	ADP_BUFFERMGR_STATUS_t pStatus;
	if(ADP_BUFFERMGR_GetStatus(pBufferMgr, &pStatus) != ADP_BUFFER_OK)
	{
		PrintError("%s(%d) Can not get buffer status\n", __FUNCTION__, __LINE__);
		//pHTuner->bErrorExit = TRUE;
		PrintExit;
		return HLS_FAIL;
	}
	if(pStatus.ulTotalBlocks * ((double)HLS_RETAIN_THRESHOLD / (double)100) < pStatus.ulUsedBlocks)
	{
		ADP_BUFFERMGR_SimpleRetainBlock(pBufferMgr, pStatus.ulTotalBlocks * (double)HLS_RETAIN_BLOCK_AT_ONCE / (double)100);
		//cso_log(CSO_LOG_DEBUG,"%s(%d) Retain blocks! remaining size : %"PRId64", readable : %"PRId64"\n", __FUNCTION__, __LINE__,pStatus.ullFreedSize, pStatus.ullReadableSize);
	}
	/* again, to get newer buffer status */
	if(ADP_BUFFERMGR_GetStatus(pBufferMgr, &pStatus) != ADP_BUFFER_OK)
	{
		PrintError("%s(%d) Can not get buffer status\n", __FUNCTION__, __LINE__);
		//pHTuner->bErrorExit = TRUE;
		return HLS_FAIL;
	}
#endif

	while(TRUE)
	{
		if((SSTR_BUFFERMGR_GetBufferSize(pBufferMgr) * HLS_CONST_HIGHEST_BUFFERING_THRESHOLD)
		> SSTR_BUFFERMGR_GetReadableSize(pBufferMgr) /*&& SSTR_BUFFERMGR_GetWritableSize(pBufferMgr) != 0*/)
		{
			break;
		}
		else
		{
			VK_TASK_Sleep(HLS_CONST_WAITING);
	//			PrintDebug ("%s(%d) BUFFER IS ALMOST FULL( %d percent over )\n",__FUNCTION__,__LINE__,SSTR_CONST_HIGHEST_BUFFERING_THRESHOLD * 100);
			continue;
		}
	}


	/***********************************************************************
	 *  1. Download Key
	 *     Encrypt 되어 있다면 Key를 가져온다.
	 *     Segment 내에서의 Seek Position(offset) 을 Bitrate 를 이용하여 측정
	 ***********************************************************************/

	if(HLS_CONTROLLER_IsCurStreamEncrypted(pHLS->hCtrl) == TRUE)
	{
		if(P_HLS_GetKey(pHLS) == HLS_OK)
		{
			if(pHLS->pCurIV == NULL)
			{
				(void)HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &ullSegment);
				_ultohex(ullSegment,emptyiv,16);
				pHLS->pCurIV = emptyiv;
			}
			else
			{
				if(HexString2Binary(pHLS->pCurIV, emptyiv, 16) == TRUE)
				{
					pHLS->pCurIV = emptyiv;
				}
			}
		}
		bEncryptedSegment = TRUE;
	}


	/***********************************************************************
	 *  2. Download Partial Segment
	 *	   Segment Buffer에 Current Segment를 Download 받는다.
	 *	   ( Bufferfullness 90% 이하인 경우 )
	 ***********************************************************************/
	/* playqueue DATA 정리*/
	pUrl = HLS_CONTROLLER_GetCurSegmentUrl(pHLS->hCtrl);
	PrintError("%s(%d) : Download Segment pUrl \n\t\t[ %s ]\n",__FUNCTION__,__LINE__,pUrl);

	bByteRangeSupport = HLS_CONTROLLER_GetCurSegmentByteRangeSupport(pHLS->hCtrl);
	ullByteRangeLength = HLS_CONTROLLER_GetCurSegmentByteRangeLength(pHLS->hCtrl);
	ullByteRangeOffset = HLS_CONTROLLER_GetCurSegmentByteRangeOffset(pHLS->hCtrl);
	ullByteRangeIndex = HLS_CONTROLLER_GetCurSegmentByteRangeIndex(pHLS->hCtrl);


	if( pUrl == NULL )
	{
		PrintError("%s(%d) HLS_NO_URL_TO_DOWNLOAD! it may be the end\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_NO_URL_TO_DOWNLOAD;
	}
	HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl,&ullSegment);


	/* 	하나의 segment access 를 사용하여 의미없는 open 막음 */
	pSegmentAccess = pHLS->pSegmentAccess;

	if(pSegmentAccess == NULL)
	{
		pSegmentAccess = (STREAM_ACCESS_t*)DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
		pHLS->pSegmentAccess = pSegmentAccess;

		PrintError("%s(%d) Segment Access open for probe\n",__FUNCTION__,__LINE__);
		PrintError("%s(%d) : Segment pUrl \n\t\t[ %s ]\n",__FUNCTION__,__LINE__, pUrl);
		while((nRet = int_hls_open(pHLS, pSegmentAccess, pUrl)) != HLS_OK)
		{
			if(nRet != HLS_OK)
			{
				PrintError("STREAM OPEN error. \n");
				PrintExit;
				goto exit_stop;
			}
			PrintError("Unknown return error. So, Retry??\n");
		}
	}
	else
	{
		/* 이전 segment 와 현재 segment 의 url 이 다르거나, (ByteRange 인 경우)position 이 다른 경우, url 에 해당하는 Stream Http Open */
		if(VK_strcmp(pSegmentAccess->psz_path, pUrl) || pSegmentAccess->info.i_pos != ullByteRangeOffset)
		{
			PrintError("%s(%d) Old seg url [ %s ]\n",__FUNCTION__,__LINE__, pSegmentAccess->psz_path);
			PrintError("%s(%d) New seg url [ %s ]\n",__FUNCTION__,__LINE__, pUrl);
			PrintError("%s(%d) Old seg pos [ %llu ]\n",__FUNCTION__,__LINE__, pSegmentAccess->info.i_pos);
			PrintError("%s(%d) New seg pos [ %llu ]\n",__FUNCTION__,__LINE__, ullByteRangeOffset);

			if(VK_strcmp(pSegmentAccess->psz_path, pUrl))
			{
				PrintError("%s(%d) Segment URL is different from each other\n",__FUNCTION__,__LINE__);
			}
			if(pSegmentAccess->info.i_pos != ullByteRangeOffset)
			{
				PrintError("%s(%d) Segment Stream Access Position is different from each other\n",__FUNCTION__,__LINE__);
			}
			(void)STREAM_HTTP_Close (pSegmentAccess);

			if ( pSegmentAccess->psz_path != NULL )
			{
				DD_MEM_Free(pSegmentAccess->psz_path);
				pSegmentAccess->psz_path = NULL;
			}

			PrintError("%s(%d) SegmentAccess open\n",__FUNCTION__,__LINE__);
			while((nRet = int_hls_open(pHLS, pSegmentAccess, pUrl)) != HLS_OK)
			{
				if(nRet != HLS_OK)
				{
					PrintError("STREAM OPEN error. \n");
					PrintExit;
					goto exit_stop;
				}
				PrintError("Unknown return error. So, Retry??\n");
			}
		}
	}

	if(bByteRangeSupport == TRUE)	sz_RemainSize = ullByteRangeLength;
	else							sz_RemainSize = pSegmentAccess->info.i_size;


	/* Probe 동작 시, M3U8에 기재된 첫 번째 Segment의 크기가 ADP 버퍼 사이즈보다 클 때 ADP Resize를 실행.*/
	if( offsetForSeek == -1 )
	{
		if( SSTR_BUFFERMGR_GetBufferSize(pBufferMgr) < sz_RemainSize )
		{
			PrintError("%s(%d) The first segment is larger than SSTR Buffer!!!\n",__FUNCTION__,__LINE__);
			nRet = HLS_FAIL;
			PrintExit;
			goto exit_stop; // free( pSegBuff )
		}
		pHLS->ulBuffSize = sz_RemainSize + HLS_CONST_PADDING;
		if(pHLS->pSegBuff != NULL)
		{
			VK_MEM_Free(pHLS->pSegBuff);
			pHLS->pSegBuff = NULL;
			pHLS->pSegBuff = VK_MEM_Alloc(sz_RemainSize + HLS_CONST_PADDING);
		}
		VK_MEM_Memset(pHLS->pSegBuff, 0x00, pHLS->ulBuffSize);
	}


	/* 세그먼트의 양만큼 버퍼를 할당하고 다운로드 받을 남은 사이즈를 입력*/
	pSegBuff = VK_MEM_Alloc( PARTIAL_SEGMENT_SIZE + HLS_CONST_PADDING);
	pEncBuff = VK_MEM_Alloc( ENCRYPTED_SEGMENT_BLOCK + HLS_CONST_PADDING);
	ulBuffSize = PARTIAL_SEGMENT_SIZE + HLS_CONST_PADDING;
	pHLS->sz_SegSize = sz_RemainSize;

	//PrintData("%s(%d) : Download Segment Size [ %lld ]\n",__FUNCTION__,__LINE__, tAccess.info.i_size);
	ullSegSize = (HUINT64)sz_RemainSize;


	/* LimitDelay = */
	ulRemainSize = sz_RemainSize;
	ulPartailSegSize = PARTIAL_SEGMENT_SIZE;
	if( ulRemainSize > ulPartailSegSize )
	{
		ulLimitPartialDelayTemp = (HUINT32) ( ulRemainSize / ulPartailSegSize );
		ulDuration = HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl);
		ulLimitPartialDelay = (ulDuration * (HUINT32)HLS_CONST_MILLISEC / ulLimitPartialDelayTemp);
	}
	else
	{
		ulLimitPartialDelay = 3000000; // 0 으로 해서 아래 if 조건 줘야 겟다.
	}


	/* offset을 새롭게 계산, partial_offsetForSeek의 시작 위치는 이전 블록을 포함한 위치이다. ( 그래서 ENCRYPTED_SEGMENT_BLOCK 을 뺀다) */
	if( ulTimeForSeek > 0 )
	{
		offsetForSeek = ulTimeForSeek * (HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) / 8);
		if(sz_RemainSize <= offsetForSeek)
		{
			offsetForSeek = ((HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl) - (ulTimeForSeek - HLS_CONTROLLER_GetCurSegmentStartTime(pHLS->hCtrl))) * (HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) / 8));
			offsetForSeek = sz_RemainSize > offsetForSeek ? sz_RemainSize - offsetForSeek : sz_RemainSize - (HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) / 8);
			PrintData("%s(%d) <<< SEEK 0 >>> offsetForSeek = %ld\n",__FUNCTION__,__LINE__
								, offsetForSeek);
		}
		if( offsetForSeek == 0 )
		{
			//pass
		}
		else
		{
			nTemp = offsetForSeek / MINIMUM_PARTIAL_SEGMENT_SIZE;


			partial_offsetForSeek = ( nTemp * MINIMUM_PARTIAL_SEGMENT_SIZE );
			sz_RemainSize = sz_RemainSize - partial_offsetForSeek;
			if( bEncryptedSegment == TRUE )
			{
				partial_offsetForSeek -= ENCRYPTED_SEGMENT_BLOCK;
			}
			ullSegSize = sz_RemainSize;
			bDownloadFirstPartialSegment = FALSE;
		}
	}


#if 0 // to be removed, old code from calypso
	VK_memset(&tUserData, 0x0, sizeof(ADP_BUFFER_UserData_t));
	tUserData.ullStartTime = HLS_CONTROLLER_GetCurSegmentStartTime(pHLS->hCtrl) + ulTimeForSeek;
	tUserData.ullDuration = HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl) - ulTimeForSeek;
	HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &tUserData.ulSegId);
	tUserData.ullSegmentSize = tAccess.info.i_size;

	ADP_BUFFERMGR_Open(pBufferMgr, &tUserData);
#endif


	VK_MEM_Memset(lastEncryptedData, 0x00, ENCRYPTED_SEGMENT_BLOCK );
	VK_MEM_Memset(encryptedData, 0x00, ENCRYPTED_SEGMENT_BLOCK );



	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */

	if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
	{
		PrintError("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
		nRet = HLS_DOWNLOAD_STOP_FORCE;
		PrintExit;
		goto exit_stop;
	}

	/* ByteRangeSupport 지원 시, 시작 위치 조정 */
	if(bByteRangeSupport == TRUE)
	{
		/* Seek 처리 */
		if( ulTimeForSeek > 0 )
		{
			partial_offsetForSeek = partial_offsetForSeek + ullByteRangeOffset;// + unPlayReadyEnvelopedHeaderSize;
			if( partial_offsetForSeek != ullByteRangeOffset )
			{
				bDownloadFirstPartialSegment = FALSE;
			}
		}
		/* Sequence 처리 시, 이전 segment 의 url & position 이 같다면, seek 하지 않는다.*/
		else
		{
			if(VK_strcmp(pSegmentAccess->psz_path, pUrl) || pSegmentAccess->info.i_pos != (ullByteRangeOffset))
			{
				partial_offsetForSeek = ullByteRangeOffset;
			}
		}
		//cso_log(CSO_LOG_INFO,"%s(%d) offsetForSeek		= %ld\n",__FUNCTION__,__LINE__, partial_offsetForSeek);
	}

	while( ( bDownloadASegment == FALSE ) && ( sz_RemainSize > 0 ) )
	{


		VK_MEM_Memset(pSegBuff, 0x00, ulBuffSize);

		/* Download 시작전, Seek 이벤트 발생시에 Seek 위치 이동 */
		if(sz_RemainSize > PARTIAL_SEGMENT_SIZE)
		{
			sz_Try = PARTIAL_SEGMENT_SIZE;
		}
		else
		{
			sz_Try = sz_RemainSize;
		}

		if( partial_offsetForSeek > 0 )
		{
			PrintError("%s(%d) before seg pos [ %llu ]\n",__FUNCTION__,__LINE__, pSegmentAccess->info.i_pos);

			PrintError("%s(%d) : partial_offsetForSeek [ %lu ]\n",__FUNCTION__,__LINE__, partial_offsetForSeek);
			nRet = int_hls_seek(pHLS, pSegmentAccess, partial_offsetForSeek, 0);
			if(HLS_OK != nRet)
			{
				PrintError("Changing position is fail\n");
				PrintExit;
				goto exit_stop;
			}

			/* Read 16 byte( encryption data ) for chiper block  */
			if( bEncryptedSegment == TRUE && bDownloadFirstPartialSegment == FALSE )
			{
				nRet = P_HLS_ReadDataRequestSize(pHLS, pSegmentAccess, pEncBuff, ENCRYPTED_SEGMENT_BLOCK);
				if(HLS_OK != nRet)
				{
					PrintError("Changing position is fail\n");
					PrintExit;
					goto exit_stop;
				}
				VK_MEM_Memcpy(encryptedData, pEncBuff, ENCRYPTED_SEGMENT_BLOCK);
			}
			PrintError("%s(%d) after seg pos [ %llu ], bEncryptedSegment [ %d ]\n",__FUNCTION__,__LINE__, pSegmentAccess->info.i_pos, bEncryptedSegment);

			/* SEEK 처리때는 이전 블록이 필요  */
			partial_offsetForSeek = 0;
			//unUsingFirstEncryptedData = ENCRYPTED_SEGMENT_BLOCK;
		}

		sz_Real=0;
		sz_RealTotal = 0;
		unUsingFirstEncryptedData = 0;

		/* Download 시작*/
		/* PARTIAL_SEGMENT_SIZE 단위로 다운로드*/
		do
		{
			if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
			{
				PrintError("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
				nRet = HLS_DOWNLOAD_STOP_FORCE;
				PrintExit;
				goto exit_stop;
			}
			if(P_STREAM_HLS_IsSeekRequested(pHLS))
			{
				nRet = HLS_SEEK_REQUESTE;
				PrintExit;
				goto seek_stop;
			}
			n_ReturnedValue = STREAM_HTTP_Read ( pSegmentAccess, pSegBuff+sz_RealTotal, sz_Try - sz_Real );

			/* Read Error : should be exit immediately */
			if (n_ReturnedValue <= 0)
			{
				PrintError("STREAM_HTTP_Read Fail!!\n");
				if(DI_IPT_SUCCESS != int_hls_seek(pHLS, pSegmentAccess, pSegmentAccess->info.i_pos, 0))
				{
					PrintError("Reconnection fail!!\n");
					nRet = HLS_FAIL;
					goto seek_stop;
				}
				else
				{
					PrintError("Reconnected!!\n");
					continue;
				}
			}
			sz_Real += n_ReturnedValue;
			sz_RealTotal += n_ReturnedValue;

		}
		while (sz_Real < sz_Try);



		/* Delay Check */
		/* 세그먼트 크기를 Partial Segment Size 로 나누면 Partial Segment Size 를 해당 초 안에 받지 못하는 지 검사.  */
		unTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */

		sz_RemainSize = sz_RemainSize - sz_Real;
		//PrintData("%s(%d) sz_RemainSize = %lu, sz_Real = %d \n",__FUNCTION__,__LINE__, sz_RemainSize, sz_Real);


		if(P_STREAM_HLS_IsSeekRequested(pHLS))
		{
			nRet = HLS_SEEK_REQUESTE;
			PrintExit;
			goto seek_stop;
		}


		/***********************************************************************
		 *	3. Decrypt Partial Segment
		 *	   Encrypted Segment를 복호화한다.
		 *
		 ***********************************************************************/
#if 0
		/* SEEK 처리때는 이전 블록이 필요  */
		if( partial_offsetForSeek > 0 )
		{
			VK_MEM_Memcpy(firstEncryptedData, pSegBuff, ENCRYPTED_SEGMENT_BLOCK);
			partial_offsetForSeek = 0;
			bDownloadFirstPartialSegment = FALSE;
			unUsingFirstEncryptedData = ENCRYPTED_SEGMENT_BLOCK;
			VK_MEM_Memcpy(encryptedData, firstEncryptedData, ENCRYPTED_SEGMENT_BLOCK);
		}
#endif

		VK_MEM_Memcpy(lastEncryptedData, pSegBuff + sz_Try - ENCRYPTED_SEGMENT_BLOCK, ENCRYPTED_SEGMENT_BLOCK);

#if 1
		/*	처음으로 세그먼트를 받는다면.  lastEncrryptedDate_Old는 디크립션시 사용.*/
		if( ( bDownloadFirstPartialSegment == TRUE ) && ( bEncryptedSegment == TRUE ) )
		{
			PrintError("%s(%d) P_STREAM_HLS_Decryption 1st ENTER\n",__FUNCTION__,__LINE__);
			#if defined(USE_HLS_FAKEKEYS_FOR_TEST)  // for TEST (MUST remove xxx)
			if(P_STREAM_HLS_Decryption(pSegBuff, sz_Try,
							fakekeys, emptyiv, pHLS->eEncMethod, pHLS->eEncPadding )!=HLS_OK)
			{
				nRet = HLS_DECRYPTION_FAIL;
				PrintExit;
				goto seek_stop;
			}
			#else
			if(P_STREAM_HLS_Decryption(pSegBuff, sz_Try,
							pHLS->ucCurKey, pHLS->pCurIV, pHLS->eEncMethod, pHLS->eEncPadding, hDec, pSrcBlock, nBlockLen)!=HLS_OK)
			{
				nRet = HLS_DECRYPTION_FAIL;
				PrintExit;
				goto seek_stop;
			}
			#endif

			bDownloadFirstPartialSegment = FALSE;
		}
		else if( ( bDownloadFirstPartialSegment == FALSE ) && ( bEncryptedSegment == TRUE ) )
		{
			PrintError("%s(%d) P_STREAM_HLS_Decryption other ENTER\n",__FUNCTION__,__LINE__);

			#if defined(USE_HLS_FAKEKEYS_FOR_TEST) // for TEST (MUST remove xxx)
			if(P_STREAM_HLS_Decryption(pSegBuff, sz_Try,
										fakekeys, emptyiv, pHLS->eEncMethod, pHLS->eEncPadding )!=HLS_OK)
			{
				nRet = HLS_DECRYPTION_FAIL;
				PrintExit;
				goto seek_stop;
			}
			#else
			//if(P_STREAM_HLS_Decryption(pHTuner,pSegBuff + unUsingFirstEncryptedData, sz_Try - unUsingFirstEncryptedData,
			//				pHLS->ucCurKey, encryptedData, pHLS->eEncMethod, pHLS->eEncPadding, hDec, pSrcBlock, nBlockLen)!=HLS_OK)
			if(P_STREAM_HLS_Decryption(pSegBuff, sz_Try,
							pHLS->ucCurKey, encryptedData, pHLS->eEncMethod, pHLS->eEncPadding, hDec, pSrcBlock, nBlockLen)!=HLS_OK)
			{
				nRet = HLS_DECRYPTION_FAIL;
				PrintExit;
				goto seek_stop;
			}
			#endif
		}

#else

		/*  처음으로 세그먼트를 받는다면.  lastEncrryptedDate_Old는 디크립션시 사용.*/
		if( ( bDownloadFirstPartialSegment == TRUE ) && ( bEncryptedSegment == TRUE ) )
		{
			//PrintData("%s(%d) P_STREAM_HLS_Decryption 1st ENTER\n",__FUNCTION__,__LINE__);
			#if defined(USE_HLS_FAKEKEYS_FOR_TEST)  /* for TEST (MUST remove xxx) */
			unDecSize = P_STREAM_HLS_Decryption(pSegBuff, sz_Try,
							fakekeys, emptyiv, pHLS->eEncMethod, pHLS->eEncPadding );
			#else
			unDecSize = P_STREAM_HLS_Decryption(pSegBuff, sz_Try,
							pHLS->ucCurKey, pHLS->pCurIV, pHLS->eEncMethod, pHLS->eEncPadding, hDec, pSrcBlock, nBlockLen);
			#endif

			bDownloadFirstPartialSegment = FALSE;
		}
		else if( ( bDownloadFirstPartialSegment == FALSE ) && ( bEncryptedSegment == TRUE ) )
		{
			//PrintError("%s(%d) P_STREAM_HLS_Decryption other ENTER\n",__FUNCTION__,__LINE__);
			unStartDecryptionFuncTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */

			#if defined(USE_HLS_FAKEKEYS_FOR_TEST) /* for TEST (MUST remove xxx) */
			unDecSize = P_STREAM_HLS_Decryption(pSegBuff + unUsingFirstEncryptedData, sz_Try - unUsingFirstEncryptedData,
										fakekeys, emptyiv, pHLS->eEncMethod, pHLS->eEncPadding );
			#else
			unDecSize = P_STREAM_HLS_Decryption(pSegBuff + unUsingFirstEncryptedData, sz_Try - unUsingFirstEncryptedData,
							pHLS->ucCurKey, /*pHLS->pCurIV*/encryptedData, pHLS->eEncMethod, pHLS->eEncPadding, hDec, pSrcBlock, nBlockLen);
			#endif

			unEndDecryptionFuncTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		}
#endif
#ifdef USE_NAGRAPRM
		else if(HLS_CONTROLLER_IsCurStreamEncrypted(pHLS->hCtrl) == FALSE)  // [02013.01.18.][khyeo][EXT-X-KEY] :
		{
			if(P_HLS_GetKey_NAGRAPRM(pHLS) != HLS_OK)
			{
				(void)mem_file_close(pHLS->pMemFd);
				pHLS->pMemFd = NULL;
				nRet = HLS_FAIL;

				goto seek_stop;
			}
		}
#endif //USE_NAGRAPRM
		else
		{
			//PrintData("%s(%d) No Encrypted Data\n",__FUNCTION__,__LINE__);
		}

		/* lastEncrryptedDate_Old는 다음 세그먼트 디크립션시 사용. */
		VK_MEM_Memcpy(encryptedData, lastEncryptedData, ENCRYPTED_SEGMENT_BLOCK);


		if( offsetForSeek == -1 )
		{
			//PrintData("<<< sz_RealTotalForProbe >>>\n");
			VK_MEM_Memcpy(pHLS->pSegBuff + sz_RealTotalForProbe, pSegBuff, sz_Try);
			sz_RealTotalForProbe += sz_Try;
		}

		/***********************************************************************
		 *	4. Buffering Decrypted Data
		 *	   Download된 Current Patial Segment를 Buffer에 Writing 한다.
		 *	   Seek이 요청된경우 Segment내의 Seek Position(Offset) 부터 Buffer에 Writing한다.
		 ***********************************************************************/

		/* 세그먼트 전체를 받는 시간을 구한다. */
		unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		ullRTDCount = 0;
		ullRTDCount = unEndTick - unStartTick;
		ullRTD = ullRTD + ullRTDCount;


		cur = 0;
		sz_Read = sz_Try - unUsingFirstEncryptedData;

		while(sz_Read > 0)
		{
			if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
			{
				PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
				goto exit_stop;
			}
			if(P_STREAM_HLS_IsSeekRequested(pHLS))
			{
				nRet = HLS_SEEK_REQUESTE;
				PrintExit;
				goto seek_stop;
			}
			nWriteSize = SSTR_BUFFERMGR_Write(pBufferMgr, pSegBuff + cur + unUsingFirstEncryptedData, sz_Read); // 다운로드한 데이터를 여기에 넣는것! 이거 전에 Decryption을 해야 한다!!
			if(nWriteSize == 0)
			{
				VK_TASK_Sleep(HLS_CONST_WAITING);
				continue;
			}
			cur += nWriteSize;
			sz_Read -= nWriteSize;
		}

		nPartialConut++;

		// 세그먼트를 다운로드 하는 중 속도가 느려지면 비트레이트를 낮춰야한다.
		// 하지만 Writer 가 가르키는 ADP 버퍼에 Reader 가 포함 되어 있는 경우, 비트레이트를 바꾼다면 Cliping 현상 ( 이전에 넣은 dts 가 기준점이 되어 새로 넣은 프레임을 버리게 되는 현상 ) 이 발생한다.
		// 그래서 비트레이트를 바꾸지 않는다. 물론 다운로드 속도가 느려 화면이 버벅거릴 수 있겠지만, 해당상황은 특수한 상황이므로 감수한다.
		// < 중요 > 기존 hls 는 하나의 세그먼트에 대한 딜레이 체크등을 햇다. 딜레이가 발생했을 경우 페닉이나 타임아웃으로 리턴해 최소 비트레이트를 선택햇다.
		//	하지만 개선은 부분 세그먼트에 대한 딜레이 체크로 받는 동시에 체크가 가능하다. 어뎁티브 스트림의 장점을 살리기 위해 최소 비트레이트를 선택하지 않고 한단계 낮은 비트레이트를 선택하도록 햇다.
		if( (unTick - unStartTick) >= ulLimitPartialDelay)
		{
			// 현재 비트레이트가 최소 비트레이트가 아닐 경우
			if(HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) != HLS_CONTROLLER_GetCurMinBitrate(pHLS->hCtrl))
			{
				// SSTR/ADP Buffer Manager 차이로 인해, 아래 사항을 적용 하지 못한다.
#if 0
				// 1. Writer 가 가르키는 ADP 버퍼에 Reader 가 포함된 경우,
				// 1 - 1. 현재 비트레이트의 세그먼트를 받을 때 딜레이가 중간에 발생한 경우, 클립핑현상을 방지하기 위해 비트레이트 낮추지 않는다( 변경하지 않는다. )
				if( ADP_BUFFERMGR_IsReaderContained(pBufferMgr) )
				{
					// Pass
				}
				// 2. Writer 가 가르키는 ADP 버퍼에 Reader 가 포함된 경우
				else
				{
					ADP_BUFFERMGR_FlushWriteBlock(pBufferMgr);
					nRet = HLS_TIMEOUT;
					PrintExit;
					goto seek_stop;
				}
#endif
			}
		}


		if( sz_RemainSize == 0 )
		{
			if( HLS_CONTROLLER_IsCurSegmentEndList( pHLS->hCtrl) )
			{
				PrintError("%s(%d) Exit HLS_CONTROLLER_SetIsEndOfStream\n",__FUNCTION__,__LINE__);
//				HLS_CONTROLLER_SetIsEndOfStream( pHLS->hCtrl, TRUE );
			}
			if( offsetForSeek == -1 )
			{
				pHLS->sz_SegSize = sz_RealTotalForProbe;
			}
			bDownloadASegment = TRUE;
			break;
		}
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	}


exit_stop:


	/***********************************************************************
	 *	5. Bitrate Change
	 *	   Heuristics Module을 이용하여 Bitrate Changing 수행
	 ***********************************************************************/

	if(ullRTD == 0) /* avoid divide zero */
	{
		ullRTD = 10;
	}

	pHLS->pCurIV = NULL;


	SSTR_HEURISTICS_Run(pHeuristics, ullSegSize, ullRTD);

seek_stop:

	//ADP_BUFFERMGR_Close(pBufferMgr);
	if ( pSegBuff != NULL )
	{
		VK_MEM_Free(pSegBuff);
	}
	if ( pEncBuff != NULL )
	{
		VK_MEM_Free(pEncBuff);
	}
	PrintExit;

	return nRet;


}
#else
static HLS_ERROR_e P_HLS_DownloadWithPlay(off_t offsetForSeek, HUINT32 ulTimeForSeek, void *hDec, HUINT8 *pSrcBlock, HINT32 nBlockLen )
{
	HINT32 nWriteSize=0;
	HINT32 nRet = HLS_OK;
	HUINT64 ullSegment = 0;
	HINT8 *pUrl = NULL;
	size_t sz_Read = 0;
	off_t cur = 0;
	STREAM_ACCESS_t tAccess;
	size_t sz_RemainSize=0;
	size_t sz_Real=0;
	size_t sz_RealTotal=0;
	size_t sz_Try = 0;
	HINT32 n_ReturnedValue = 0;
	HUINT32 unStartTick = 0;
	HUINT32 unTick = 0;
	HUINT32 unEndTick = 0;
	HUINT64 ullRTD = 0;
	HUINT64 ullRTDCount = 0;
	HUINT32 unStartDecryptionFuncTick = 0;
	HUINT32 unEndDecryptionFuncTick = 0;
	SSTR_BUFFERMGR_t* pBufferMgr = NULL;
	SSTR_HEURISTICS_t* pHeuristics = NULL;
	HLS_t* pHLS = NULL;

	HUINT32 nTemp = 0;
	off_t partial_offsetForSeek = 0;

	HUINT8 *pSegBuff = NULL;
	HUINT32 ulBuffSize = 0;
	size_t sz_RealTotalForProbe = 0;

	HUINT64 ullSegSize = 0;

	HINT32 nPartialConut = 0;

	HUINT32 ulDuration = 0;

	HUINT32 ulRemainSize = 0;
	HUINT32 ulPartailSegSize = 0;
	HUINT32 ulLimitPartialDelayTemp = 0;
	HUINT32 ulLimitPartialDelay = 0;


#ifdef USE_HLS_FAKEKEYS_FOR_TEST /* for TEST (MUST remove xxx) */
	HUINT8 fakekeys[16] = "0123456789abcdef";
#endif
	HUINT8 emptyiv[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	HUINT32 unDecSize = 0;


#if 0
	static const HUINT32 HLS_RETAIN_THRESHOLD = 80;		 /* percentage of total buffer size*/
	static const HUINT32 HLS_RETAIN_BLOCK_AT_ONCE = 30; /* percentage of the number of blocks */
#endif
	HUINT8 lastEncryptedData[ ENCRYPTED_SEGMENT_BLOCK ] = { 0, };
	HUINT8 encryptedData[ ENCRYPTED_SEGMENT_BLOCK ] = { 0, };
	HUINT8 firstEncryptedData[ ENCRYPTED_SEGMENT_BLOCK ] = { 0, };
	HUINT32 unUsingFirstEncryptedData = 0;
	HBOOL bEncryptedSegment = FALSE;
	HBOOL bDownloadASegment = FALSE;
	HBOOL bDownloadFirstPartialSegment = TRUE;

	PrintEnter;

	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return HLS_FAIL;
	}
	pHLS = __getHLS(g_hHLS);
	pBufferMgr = HLS_CONTROLLER_GetBufferMgr(pHLS->hCtrl);
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> pBufferMgr is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pHLS->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> HLS Controller is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHeuristics = HLS_CONTROLLER_GetHeuristics(pHLS->hCtrl);
	if(pHeuristics == NULL)
	{
		PrintError("%s(%d) Error> pHeuristics is NULL\n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	VK_memset(&tAccess,0x00,sizeof(STREAM_ACCESS_t));

#if 0
	ADP_BUFFERMGR_STATUS_t pStatus;
	if(ADP_BUFFERMGR_GetStatus(pBufferMgr, &pStatus) != ADP_BUFFER_OK)
	{
		PrintError("%s(%d) Can not get buffer status\n", __FUNCTION__, __LINE__);
		//pHTuner->bErrorExit = TRUE;
		PrintExit;
		return HLS_FAIL;
	}
	if(pStatus.ulTotalBlocks * ((double)HLS_RETAIN_THRESHOLD / (double)100) < pStatus.ulUsedBlocks)
	{
		ADP_BUFFERMGR_SimpleRetainBlock(pBufferMgr, pStatus.ulTotalBlocks * (double)HLS_RETAIN_BLOCK_AT_ONCE / (double)100);
		//cso_log(CSO_LOG_DEBUG,"%s(%d) Retain blocks! remaining size : %"PRId64", readable : %"PRId64"\n", __FUNCTION__, __LINE__,pStatus.ullFreedSize, pStatus.ullReadableSize);
	}
	/* again, to get newer buffer status */
	if(ADP_BUFFERMGR_GetStatus(pBufferMgr, &pStatus) != ADP_BUFFER_OK)
	{
		PrintError("%s(%d) Can not get buffer status\n", __FUNCTION__, __LINE__);
		//pHTuner->bErrorExit = TRUE;
		return HLS_FAIL;
	}
#endif

	while(TRUE)
	{
		if((SSTR_BUFFERMGR_GetBufferSize(pBufferMgr) * HLS_CONST_HIGHEST_BUFFERING_THRESHOLD)
		> SSTR_BUFFERMGR_GetReadableSize(pBufferMgr) /*&& SSTR_BUFFERMGR_GetWritableSize(pBufferMgr) != 0*/)
		{
			break;
		}
		else
		{
			VK_TASK_Sleep(HLS_CONST_WAITING);
	//			PrintDebug ("%s(%d) BUFFER IS ALMOST FULL( %d percent over )\n",__FUNCTION__,__LINE__,SSTR_CONST_HIGHEST_BUFFERING_THRESHOLD * 100);
			continue;
		}
	}


	/***********************************************************************
	 *  1. Download Key
	 *     Encrypt 되어 있다면 Key를 가져온다.
	 *     Segment 내에서의 Seek Position(offset) 을 Bitrate 를 이용하여 측정
	 ***********************************************************************/

	if(HLS_CONTROLLER_IsCurStreamEncrypted(pHLS->hCtrl) == TRUE)
	{
		if(P_HLS_GetKey(pHLS) == HLS_OK)
		{
			if(pHLS->pCurIV == NULL)
			{
				(void)HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &ullSegment);
				_ultohex(ullSegment,emptyiv,16);
				pHLS->pCurIV = emptyiv;
			}
			else
			{
				if(HexString2Binary(pHLS->pCurIV, emptyiv, 16) == TRUE)
				{
					pHLS->pCurIV = emptyiv;
				}
			}
		}
		bEncryptedSegment = TRUE;
	}


	/***********************************************************************
	 *  2. Download Partial Segment
	 *	   Segment Buffer에 Current Segment를 Download 받는다.
	 *	   ( Bufferfullness 90% 이하인 경우 )
	 ***********************************************************************/
	/* playqueue DATA 정리*/
	pUrl = HLS_CONTROLLER_GetCurSegmentUrl(pHLS->hCtrl);
	PrintError("%s(%d) : Download Segment pUrl \n\t\t[ %s ]\n",__FUNCTION__,__LINE__,pUrl);


	if( pUrl == NULL )
	{
		PrintError("%s(%d) HLS_NO_URL_TO_DOWNLOAD! it may be the end\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_NO_URL_TO_DOWNLOAD;
	}
	HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl,&ullSegment);
	tAccess.psz_path = VK_StrDup(pUrl);
	if(tAccess.psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}

	nRet = STREAM_HTTP_Open (&tAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		//PrintData("%s(%d) : STREAM_HTTP_Open nRet [ %ld ]\n",__FUNCTION__,__LINE__, nRet);
		PrintError("%s(%d) : Error> HTTP_Open Fail.nRet [ %ld ]\n",__FUNCTION__,__LINE__, nRet);
		(void)STREAM_HTTP_Close (&tAccess);
		if ( tAccess.psz_path != NULL )
		{
			VK_MEM_Free(tAccess.psz_path);
			tAccess.psz_path = NULL;
		}
		PrintExit;
		return HLS_FAIL;
	}


	/* Probe 동작 시, M3U8에 기재된 첫 번째 Segment의 크기가 ADP 버퍼 사이즈보다 클 때 ADP Resize를 실행.*/
	if( offsetForSeek == -1 )
	{
		if( SSTR_BUFFERMGR_GetBufferSize(pBufferMgr) < tAccess.info.i_size )
		{
			PrintError("%s(%d) The first segment is larger than SSTR Buffer!!!\n",__FUNCTION__,__LINE__);
			nRet = HLS_FAIL;
			PrintExit;
			goto exit_stop; // free( pSegBuff )
		}
		pHLS->ulBuffSize = tAccess.info.i_size + HLS_CONST_PADDING;
		if(pHLS->pSegBuff != NULL)
		{
			VK_MEM_Free(pHLS->pSegBuff);
			pHLS->pSegBuff = NULL;
			pHLS->pSegBuff = VK_MEM_Alloc(tAccess.info.i_size + HLS_CONST_PADDING);
		}
		VK_MEM_Memset(pHLS->pSegBuff, 0x00, pHLS->ulBuffSize);
	}


	/* 세그먼트의 양만큼 버퍼를 할당하고 다운로드 받을 남은 사이즈를 입력*/
	pSegBuff = VK_MEM_Alloc( PARTIAL_SEGMENT_SIZE + HLS_CONST_PADDING);
	ulBuffSize = PARTIAL_SEGMENT_SIZE + HLS_CONST_PADDING;
	sz_RemainSize = tAccess.info.i_size;
	pHLS->sz_SegSize = sz_RemainSize;

	//PrintData("%s(%d) : Download Segment Size [ %lld ]\n",__FUNCTION__,__LINE__, tAccess.info.i_size);
	ullSegSize = (HUINT64)tAccess.info.i_size;


	/* LimitDelay = */
	ulRemainSize = sz_RemainSize;
	ulPartailSegSize = PARTIAL_SEGMENT_SIZE;
	if( ulRemainSize > ulPartailSegSize )
	{
		ulLimitPartialDelayTemp = (HUINT32) ( ulRemainSize / ulPartailSegSize );
		ulDuration = HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl);
		ulLimitPartialDelay = (ulDuration * (HUINT32)HLS_CONST_MILLISEC / ulLimitPartialDelayTemp);
	}
	else
	{
		ulLimitPartialDelay = 3000000; // 0 으로 해서 아래 if 조건 줘야 겟다.
	}


	/* offset을 새롭게 계산, partial_offsetForSeek의 시작 위치는 이전 블록을 포함한 위치이다. ( 그래서 ENCRYPTED_SEGMENT_BLOCK 을 뺀다) */
	if( ulTimeForSeek > 0 )
	{
		offsetForSeek = ulTimeForSeek * (HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) / 8);
		if(sz_RemainSize <= offsetForSeek)
		{
			offsetForSeek = ((HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl) - (ulTimeForSeek - HLS_CONTROLLER_GetCurSegmentStartTime(pHLS->hCtrl))) * (HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) / 8));
			offsetForSeek = sz_RemainSize > offsetForSeek ? sz_RemainSize - offsetForSeek : sz_RemainSize - (HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) / 8);
			PrintData("%s(%d) <<< SEEK 0 >>> offsetForSeek = %ld\n",__FUNCTION__,__LINE__
								, offsetForSeek);
		}
		if( offsetForSeek == 0 )
		{
			//pass
		}
		else
		{
			nTemp = offsetForSeek / MINIMUM_PARTIAL_SEGMENT_SIZE;
			partial_offsetForSeek = ( nTemp * MINIMUM_PARTIAL_SEGMENT_SIZE ) - ENCRYPTED_SEGMENT_BLOCK;
			sz_RemainSize = sz_RemainSize - partial_offsetForSeek;
			ullSegSize = sz_RemainSize;
		}
	}


#if 0 // to be removed, old code from calypso
	VK_memset(&tUserData, 0x0, sizeof(ADP_BUFFER_UserData_t));
	tUserData.ullStartTime = HLS_CONTROLLER_GetCurSegmentStartTime(pHLS->hCtrl) + ulTimeForSeek;
	tUserData.ullDuration = HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl) - ulTimeForSeek;
	HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &tUserData.ulSegId);
	tUserData.ullSegmentSize = tAccess.info.i_size;

	ADP_BUFFERMGR_Open(pBufferMgr, &tUserData);
#endif


	VK_MEM_Memset(lastEncryptedData, 0x00, ENCRYPTED_SEGMENT_BLOCK );
	VK_MEM_Memset(encryptedData, 0x00, ENCRYPTED_SEGMENT_BLOCK );



	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */

	if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
	{
		PrintError("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
		nRet = HLS_DOWNLOAD_STOP_FORCE;
		PrintExit;
		goto exit_stop;
	}

	while( ( bDownloadASegment == FALSE ) && ( sz_RemainSize > 0 ) )
	{


		VK_MEM_Memset(pSegBuff, 0x00, ulBuffSize);

		/* Download 시작전, Seek 이벤트 발생시에 Seek 위치 이동 */
		if(sz_RemainSize > PARTIAL_SEGMENT_SIZE)
		{
			sz_Try = PARTIAL_SEGMENT_SIZE;
		}
		else
		{
			sz_Try = sz_RemainSize;
		}

		if( partial_offsetForSeek > 0 )
		{
			//PrintData("%s(%d) : <<< Test >>> partial_offsetForSeek [ %lu ]\n",__FUNCTION__,__LINE__, partial_offsetForSeek);
			if(DI_IPT_SUCCESS != STREAM_HTTP_Seek( &tAccess, partial_offsetForSeek))
			{
				PrintData("%s(%d) : <<< Test >>> partial_offsetForSeek [ %lu ]\n",__FUNCTION__,__LINE__, partial_offsetForSeek);
				PrintDebug("%s(%d) STREAM_HTTP_Seek FAIL\n\n",__FUNCTION__,__LINE__);

				// ADP 버퍼 Flush 를 여기서 사용 하는 이유 : Seek / Trick 에 대한 처리 요청이 왔는데 Flush 를 안하면, ADP 버퍼가 가득 풀로 되면 무한루프에 진입한다.
				// 그러므로 여기서 아래의 Flush 를 해준다.
//				ADP_BUFFERMGR_FlushWriteBlock(pBufferMgr);
//				ADP_BUFFERMGR_Flush(pBufferMgr);
				(void)SSTR_BUFFERMGR_Clear(pBufferMgr); // ADP Buffer대신 SSTR 버퍼를 사용.
				nRet = HLS_FAIL;
				PrintExit;
				goto exit_stop;
			}
		}

		sz_Real=0;
		sz_RealTotal = 0;
		unUsingFirstEncryptedData = 0;

		/* Download 시작*/
		/* PARTIAL_SEGMENT_SIZE 단위로 다운로드*/
		do
		{
			if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
			{
				PrintError("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
				nRet = HLS_DOWNLOAD_STOP_FORCE;
				PrintExit;
				goto exit_stop;
			}
			if(P_STREAM_HLS_IsSeekRequested(pHLS))
			{
				nRet = HLS_SEEK_REQUESTE;
				PrintExit;
				goto seek_stop;
			}
			n_ReturnedValue = STREAM_HTTP_Read ( &tAccess, pSegBuff+sz_RealTotal, sz_Try - sz_Real );

			/* Read Error : should be exit immediately */
			if (n_ReturnedValue <= 0)
			{
				PrintError("STREAM_HTTP_Read Fail!!\n");
				if(DI_IPT_SUCCESS != int_hls_seek(&tAccess, tAccess.info.i_pos))
				{
					PrintError("Reconnection fail!!\n");
					nRet = HLS_FAIL;
					goto seek_stop;
				}
				else
				{
					PrintError("Reconnected!!\n");
					continue;
				}
			}
			sz_Real += n_ReturnedValue;
			sz_RealTotal += n_ReturnedValue;

		}
		while (sz_Real < sz_Try);



		/* Delay Check */
		/* 세그먼트 크기를 Partial Segment Size 로 나누면 Partial Segment Size 를 해당 초 안에 받지 못하는 지 검사.  */
		unTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */

		sz_RemainSize = sz_RemainSize - sz_Real;
		//PrintData("%s(%d) sz_RemainSize = %lu, sz_Real = %d \n",__FUNCTION__,__LINE__, sz_RemainSize, sz_Real);


		if(P_STREAM_HLS_IsSeekRequested(pHLS))
		{
			nRet = HLS_SEEK_REQUESTE;
			PrintExit;
			goto seek_stop;
		}


		/***********************************************************************
		 *	3. Decrypt Partial Segment
		 *	   Encrypted Segment를 복호화한다.
		 *
		 ***********************************************************************/
		/* SEEK 처리때는 이전 블록이 필요  */
		if( partial_offsetForSeek > 0 )
		{
			VK_MEM_Memcpy(firstEncryptedData, pSegBuff, ENCRYPTED_SEGMENT_BLOCK);
			partial_offsetForSeek = 0;
			bDownloadFirstPartialSegment = FALSE;
			unUsingFirstEncryptedData = ENCRYPTED_SEGMENT_BLOCK;
			VK_MEM_Memcpy(encryptedData, firstEncryptedData, ENCRYPTED_SEGMENT_BLOCK);
		}

		VK_MEM_Memcpy(lastEncryptedData, pSegBuff + sz_Try - ENCRYPTED_SEGMENT_BLOCK, ENCRYPTED_SEGMENT_BLOCK);

		/*  처음으로 세그먼트를 받는다면.  lastEncrryptedDate_Old는 디크립션시 사용.*/
		if( ( bDownloadFirstPartialSegment == TRUE ) && ( bEncryptedSegment == TRUE ) )
		{
			//PrintData("%s(%d) P_STREAM_HLS_Decryption 1st ENTER\n",__FUNCTION__,__LINE__);
			#if defined(USE_HLS_FAKEKEYS_FOR_TEST)  /* for TEST (MUST remove xxx) */
			unDecSize = P_STREAM_HLS_Decryption(pSegBuff, sz_Try,
							fakekeys, emptyiv, pHLS->eEncMethod, pHLS->eEncPadding );
			#else
			unDecSize = P_STREAM_HLS_Decryption(pSegBuff, sz_Try,
							pHLS->ucCurKey, pHLS->pCurIV, pHLS->eEncMethod, pHLS->eEncPadding, hDec, pSrcBlock, nBlockLen);
			#endif

			bDownloadFirstPartialSegment = FALSE;
		}
		else if( ( bDownloadFirstPartialSegment == FALSE ) && ( bEncryptedSegment == TRUE ) )
		{
			//PrintError("%s(%d) P_STREAM_HLS_Decryption other ENTER\n",__FUNCTION__,__LINE__);
			unStartDecryptionFuncTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */

			#if defined(USE_HLS_FAKEKEYS_FOR_TEST) /* for TEST (MUST remove xxx) */
			unDecSize = P_STREAM_HLS_Decryption(pSegBuff + unUsingFirstEncryptedData, sz_Try - unUsingFirstEncryptedData,
										fakekeys, emptyiv, pHLS->eEncMethod, pHLS->eEncPadding );
			#else
			unDecSize = P_STREAM_HLS_Decryption(pSegBuff + unUsingFirstEncryptedData, sz_Try - unUsingFirstEncryptedData,
							pHLS->ucCurKey, /*pHLS->pCurIV*/encryptedData, pHLS->eEncMethod, pHLS->eEncPadding, hDec, pSrcBlock, nBlockLen);
			#endif

			unEndDecryptionFuncTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		}
#ifdef USE_NAGRAPRM
		else if(HLS_CONTROLLER_IsCurStreamEncrypted(pHLS->hCtrl) == FALSE)  // [02013.01.18.][khyeo][EXT-X-KEY] :
		{
			if(P_HLS_GetKey_NAGRAPRM(pHLS) != HLS_OK)
			{
				(void)mem_file_close(pHLS->pMemFd);
				pHLS->pMemFd = NULL;
				nRet = HLS_FAIL;

				goto seek_stop;
			}
		}
#endif //USE_NAGRAPRM
		else
		{
			//PrintData("%s(%d) No Encrypted Data\n",__FUNCTION__,__LINE__);
		}

		/* lastEncrryptedDate_Old는 다음 세그먼트 디크립션시 사용. */
		VK_MEM_Memcpy(encryptedData, lastEncryptedData, ENCRYPTED_SEGMENT_BLOCK);


		if( offsetForSeek == -1 )
		{
			//PrintData("<<< sz_RealTotalForProbe >>>\n");
			VK_MEM_Memcpy(pHLS->pSegBuff + sz_RealTotalForProbe, pSegBuff, sz_Try);
			sz_RealTotalForProbe += sz_Try;
		}

		/***********************************************************************
		 *	4. Buffering Decrypted Data
		 *	   Download된 Current Patial Segment를 Buffer에 Writing 한다.
		 *	   Seek이 요청된경우 Segment내의 Seek Position(Offset) 부터 Buffer에 Writing한다.
		 ***********************************************************************/

		/* 세그먼트 전체를 받는 시간을 구한다. */
		unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		ullRTDCount = 0;
		ullRTDCount = unEndTick - unStartTick;
		ullRTD = ullRTD + ullRTDCount;


		cur = 0;
		sz_Read = sz_Try - unUsingFirstEncryptedData;

		while(sz_Read > 0)
		{
			if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
			{
				PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
				goto exit_stop;
			}
			if(P_STREAM_HLS_IsSeekRequested(pHLS))
			{
				nRet = HLS_SEEK_REQUESTE;
				PrintExit;
				goto seek_stop;
			}
			nWriteSize = SSTR_BUFFERMGR_Write(pBufferMgr, pSegBuff + cur + unUsingFirstEncryptedData, sz_Read); // 다운로드한 데이터를 여기에 넣는것! 이거 전에 Decryption을 해야 한다!!
			if(nWriteSize == 0)
			{
				VK_TASK_Sleep(HLS_CONST_WAITING);
				continue;
			}
			cur += nWriteSize;
			sz_Read -= nWriteSize;
		}

		nPartialConut++;

		// 세그먼트를 다운로드 하는 중 속도가 느려지면 비트레이트를 낮춰야한다.
		// 하지만 Writer 가 가르키는 ADP 버퍼에 Reader 가 포함 되어 있는 경우, 비트레이트를 바꾼다면 Cliping 현상 ( 이전에 넣은 dts 가 기준점이 되어 새로 넣은 프레임을 버리게 되는 현상 ) 이 발생한다.
		// 그래서 비트레이트를 바꾸지 않는다. 물론 다운로드 속도가 느려 화면이 버벅거릴 수 있겠지만, 해당상황은 특수한 상황이므로 감수한다.
		// < 중요 > 기존 hls 는 하나의 세그먼트에 대한 딜레이 체크등을 햇다. 딜레이가 발생했을 경우 페닉이나 타임아웃으로 리턴해 최소 비트레이트를 선택햇다.
		//	하지만 개선은 부분 세그먼트에 대한 딜레이 체크로 받는 동시에 체크가 가능하다. 어뎁티브 스트림의 장점을 살리기 위해 최소 비트레이트를 선택하지 않고 한단계 낮은 비트레이트를 선택하도록 햇다.
		if( (unTick - unStartTick) >= ulLimitPartialDelay)
		{
			// 현재 비트레이트가 최소 비트레이트가 아닐 경우
			if(HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) != HLS_CONTROLLER_GetCurMinBitrate(pHLS->hCtrl))
			{
				// SSTR/ADP Buffer Manager 차이로 인해, 아래 사항을 적용 하지 못한다.
#if 0
				// 1. Writer 가 가르키는 ADP 버퍼에 Reader 가 포함된 경우,
				// 1 - 1. 현재 비트레이트의 세그먼트를 받을 때 딜레이가 중간에 발생한 경우, 클립핑현상을 방지하기 위해 비트레이트 낮추지 않는다( 변경하지 않는다. )
				if( ADP_BUFFERMGR_IsReaderContained(pBufferMgr) )
				{
					// Pass
				}
				// 2. Writer 가 가르키는 ADP 버퍼에 Reader 가 포함된 경우
				else
				{
					ADP_BUFFERMGR_FlushWriteBlock(pBufferMgr);
					nRet = HLS_TIMEOUT;
					PrintExit;
					goto seek_stop;
				}
#endif
			}
		}


		if( sz_RemainSize == 0 )
		{
			if( HLS_CONTROLLER_IsCurSegmentEndList( pHLS->hCtrl) )
			{
				PrintError("%s(%d) Exit HLS_CONTROLLER_SetIsEndOfStream\n",__FUNCTION__,__LINE__);
//				HLS_CONTROLLER_SetIsEndOfStream( pHLS->hCtrl, TRUE );
			}
			if( offsetForSeek == -1 )
			{
				pHLS->sz_SegSize = sz_RealTotalForProbe;
			}
			bDownloadASegment = TRUE;
			break;
		}
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	}


exit_stop:


	/***********************************************************************
	 *	5. Bitrate Change
	 *	   Heuristics Module을 이용하여 Bitrate Changing 수행
	 ***********************************************************************/

	if(ullRTD == 0) /* avoid divide zero */
	{
		ullRTD = 10;
	}

	pHLS->pCurIV = NULL;


	SSTR_HEURISTICS_Run(pHeuristics, ullSegSize, ullRTD);

seek_stop:

	//ADP_BUFFERMGR_Close(pBufferMgr);
	(void)STREAM_HTTP_Close (&tAccess);

	if ( tAccess.psz_path != NULL )
	{
		VK_MEM_Free(tAccess.psz_path);
		tAccess.psz_path = NULL;
	}
	if ( pSegBuff != NULL )
	{
		VK_MEM_Free(pSegBuff);
	}
	PrintExit;

	return nRet;


}
#endif

#endif


#if 0
static HLS_ERROR_e P_HLS_RequestDownloadQueue(HLS_PlayQueue_t *playqueue, HLS_t* pHLS)
{
	HINT32 nRet = 0;
	HINT8 *pUrl = NULL;
//	HUINT32 bandwidth;
	HUINT32 unSegmentId;
//	HUINT32 retry_count_to_download=0;
	STREAM_ACCESS_t tAccess;
	size_t sz_RemainSize=0;
	size_t sz_Real=0;
	size_t sz_RealTotal=0;
	size_t sz_Try = 0;
	HINT32 n_ReturnedValue = 0;
	PrintEnter;
	if(pHLS == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(playqueue == NULL)
	{
		PrintError("%s(%d) Error> PlayQueue is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pHLS->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> HLS Controller is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	VK_memset(&tAccess,0x00,sizeof(STREAM_ACCESS_t));
	/* playqueue DATA 정리*/
	playqueue->offset_present_position = 0;
	playqueue->segmentSize =0;

#if 0
	/*이전 Download가 존재한다면 Free 함. */
	if (playqueue->buffer_of_segment != NULL )
	{
		(void)VK_MEM_Free(playqueue->buffer_of_segment);
	}
	playqueue->buffer_of_segment = NULL;
#endif

//	bandwidth = HLS_GetHeuristicBitrates();

	/* topm3u8 , bandwidth 는 Heuristic 으로 부터, segment number 는 Controller 로부터 결국 Segment 의 URL 을 얻어 온다.*/
	if((HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl,&unSegmentId) != HLS_OK)
	|| (unSegmentId != playqueue->segmentId))
	{
		PrintData("%s(%d) Segment (%u) is incorrect\n",__FUNCTION__,__LINE__,playqueue->segmentId);
		PrintExit;
		return HLS_FAIL;
	}
	pUrl = playqueue->pUrl;
	if( pUrl == NULL )
	{
		PrintData("%s(%d) HLS_NO_URL_TO_DOWNLOAD! it may be the end\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_NO_URL_TO_DOWNLOAD;
	}
	tAccess.psz_path = VK_StrDup((const char*)pUrl);
	if(tAccess.psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	/* */
	nRet = STREAM_HTTP_Open (&tAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	else
	{
	#if 0
		PrintData ("%s(%d) : i_update       = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_update);
		PrintData ("%s(%d) : i_size         = %"PRId64"\n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_size);
		PrintData ("%s(%d) : i_pos          = %"PRId64"\n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_pos);
		PrintData ("%s(%d) : b_eof          = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.b_eof);
		PrintData ("%s(%d) : i_title        = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_title);
		PrintData ("%s(%d) : i_seekpoint    = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_seekpoint);
		PrintData ("%s(%d) : b_prebuffered  = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.b_prebuffered);
	#endif
		if( tAccess.info.i_size > pHLS->ulBuffSize )
		{
			if(pHLS->pSegBuff != NULL)
			{
				DD_MEM_Free(pHLS->pSegBuff);
			}
			pHLS->pSegBuff = DD_MEM_Alloc(tAccess.info.i_size + HLS_CONST_PADDING);
			if(pHLS->pSegBuff == NULL)
			{
				PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
				PrintExit;
				return HLS_FAIL;
			}
			pHLS->ulBuffSize = tAccess.info.i_size + HLS_CONST_PADDING;
		}
		VK_MEM_Memset(pHLS->pSegBuff, 0x00, pHLS->ulBuffSize);
		/* Download 시작*/

		/* 다운로드 받을 남은 사이즈를 입력*/
		sz_RemainSize = tAccess.info.i_size;

		do
		{
			/* 60KB 단위로 다운로드*/
			if(sz_RemainSize > HLS_CONST_RECV_BLOCKSIZE)
			{
				sz_Try = HLS_CONST_RECV_BLOCKSIZE;
			}
			else
			{
				sz_Try = sz_RemainSize;
			}

			sz_Real=0;

			do
			{
				if( TRUE == pHLS->bImmediatelyExit )
				{
					PrintError ("%s(%d) Stop Download Right Now!!! \n",__FUNCTION__,__LINE__);
					PrintExit;
					return HLS_FAIL;
				}
				n_ReturnedValue = STREAM_HTTP_Read ( &tAccess, (HUINT8 *)pHLS->pSegBuff+sz_RealTotal, sz_Try - sz_Real );

				sz_Real += n_ReturnedValue;
				sz_RealTotal += n_ReturnedValue;
			}
			while (sz_Real < sz_Try);

			sz_RemainSize = sz_RemainSize-sz_Real;
		}
		while(sz_RemainSize > 0);
	}
//	pHLS->ulQueueId = playqueue->queueId;
	playqueue->segmentSize = tAccess.info.i_size;

	(void)STREAM_HTTP_Close (&tAccess);
	if ( tAccess.psz_path != NULL )
	{
		VK_MEM_Free(tAccess.psz_path);
	}

#if 0
	/* 다운로드 시도를 총 3번 하도록 한다. */
	while( playqueue->buffer_of_segment == NULL  )
	{
		playqueue->buffer_of_segment = HLS_DownloadURL( url, &playqueue->size_of_segment );

		if ( s_ucStopDownloadRightNow == TRUE )
		{
			PrintData("%s(%d) HLS_DOWNLOAD_STOP_FORCE \n",__FUNCTION__,__LINE__);
			return HLS_DOWNLOAD_STOP_FORCE;
		}

		if ( retry_count_to_download > 5 )
		{
			PrintError("%s(%d) : Error> HLS_DownloadURL \n",__FUNCTION__,__LINE__);
			return HLS_DOWNLOAD_FAIL;
		}
		retry_count_to_download++;
	}
	playqueue->number_of_segment = segment_number;
#endif
	PrintExit;
	return HLS_OK;
}
#endif
#if 0 /* Test */

/**
 * @b Function @b Description <br>
 * HLS 의 VFIO Open 함수.
 *
 * @param[in] path	: Open 할 PATH.
 * @param[in] mode	: Open Mode.
 * @param[in] ptHandle	: VFIO_Handle_t 핸들.
 *
 * @see  VFIOOpenCB 에 typecasting 된다.
 */
int DI_STREAM_HLS_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	PrintEnter;
	HLS_ERROR_e nRet;

	/*1. Controller Init  : 필수 */
	nRet = HLS_Open(path);
	if( nRet !=  HLS_OK)
	{
		PrintError ("%s(%d) Error> HLS_Open \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}
	/*2. Heuristic Mode 결정 : 옵션 default 는 Heuristic On 임. */

	PrintExit;
	return DI_IPT_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * HLS 의 VFIO Close 함수.
 *
 * @param[in] tHandle	: VFIO_Handle_t 핸들.
 *
 * @see  VFIOCloseCB 에 typecasting 된다.
 */
int DI_STREAM_HLS_Close (VFIO_Handle_t tHandle)
{
	PrintEnter;
	HLS_ERROR_e nRet;

	nRet =  HLS_Close();
	if( nRet !=  HLS_OK)
	{
		PrintError ("%s(%d) Error> HLS_Close \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	PrintExit;
	return DI_IPT_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * HLS 의 VFIO Read 함수.
 *
 * @param[in] tHandle	: VFIO_Handle_t 핸들.
 * @param[in] ptr		: Read 를 위한 Pointer.
 * @param[in] size		: Read 할 size.
 * @param[in] nmemb		: Read 할 nmemb.
 * @param[in] ptReadSize	: Read 한 사이즈.
 *
 * @see  VFIOReadCB 에 typecasting 된다.
 */
int DI_STREAM_HLS_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize)
{
	PrintEnter;
	HLS_ERROR_e nRet;
	size_t total_size;
	HUINT8 * read;

	total_size = size * nmemb;

	PrintError ("%s(%d) Requested size = %d \n",__FUNCTION__,__LINE__, total_size);

	nRet =  HLS_Read(ptr, total_size, ptReadSize);
	if( nRet !=  HLS_OK)
	{
		PrintError ("%s(%d) Error> HLS_Read \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	read = (HUINT8 *)ptr;

	PrintError ("%s(%d) Read size = %d \n",__FUNCTION__,__LINE__, *ptReadSize);
	PrintError ("%s(%d) Read data = %x,%x \n",__FUNCTION__,__LINE__, read[0], read[1]);

	PrintExit;
	return DI_IPT_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * HLS 의 VFIO Seek 함수. offset seek 은 하지 않는다.
 *
 * @param[in] tHandle	: VFIO_Handle_t 핸들.
 * @param[in] offset	: Seek Offset.
 * @param[in] whence	: Seek whence.
 *
 * @see  VFIOSeekCB 에 typecasting 된다.
 */
int DI_STREAM_HLS_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	PrintEnter;
	HLS_ERROR_e nRet;

#if 0
	nRet =  HLS_Seek(offset);
	if( nRet !=  HLS_OK)
	{
		PrintError ("%s(%d) Error> HLS_Seek \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}
#endif
	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * HLS 의 VFIO Tell 함수.
 *
 * @param[in] tHandle	: VFIO_Handle_t 핸들.
 * @param[in] plCurOffset	: Current Offset.
 *
 * @see  VFIOTellCB 에 typecasting 된다.
 */
int DI_STREAM_HLS_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	PrintEnter;
	HLS_ERROR_e nRet;

#if 0
	nRet =  HLS_Tell(plCurOffset);
	if( nRet !=  HLS_OK)
	{
		PrintError ("%s(%d) Error> HLS_Seek \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}
#endif

	PrintData ("%s(%d) Tell = %x \n",__FUNCTION__,__LINE__, *plCurOffset);


	//PrintError ("%s(%d) Error> HLS_Tell is not implemented! \n",__FUNCTION__,__LINE__);
	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * HLS 의 VFIO ISEoF 함수.
 *
 * @param[in] tHandle	: VFIO_Handle_t 핸들.
 *
 * @see  VFIOIsEOFCB 에 typecasting 된다.
 */
int DI_STREAM_HLS_IsEOF (VFIO_Handle_t tHandle)
{
	PrintEnter;
	HLS_ERROR_e nRet;

	nRet =  HLS_IsEOF();
	if( nRet ==  HLS_OK)
	{
		PrintError ("%s(%d) Error> HLS_IsEOF \n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}

	PrintExit;
	return DI_IPT_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * HLS 의 VFIO Open for Nexus 함수.
 *
 * @param[in] path	: Open 할 PATH.
 * @param[in] flags	: Nexus Open Flag.
 * @param[in] ptHandle	: VFIO_Handle_t 핸들.
 *
 * @see  VFIOOpenFromNexusCB 에 typecasting 된다.
 */
int DI_STREAM_HLS_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	PrintEnter;
	HLS_ERROR_e nRet;

	/*1. Controller Init  : 필수 */
	nRet = HLS_Open(path);
	if( nRet !=  HLS_OK)
	{
		PrintError ("%s(%d) Error> HLS_Open \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}
	/*2. Heuristic Mode 결정 : 옵션 default 는 Heuristic On 임. */

	PrintExit;
	return DI_IPT_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * HLS 의 VFIO Seek For Nexus 함수.
 *
 * @param[in] tHandle	: VFIO_Handle_t 핸들.
 * @param[in] offset	: Seek Offset.
 * @param[in] whence	: Seek whence.
 *
 * @see  VFIOSeekForNexusCB 에 typecasting 된다.
 */
off_t DI_STREAM_HLS_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	PrintEnter;
	HLS_ERROR_e nRet;
	off_t rt_offset;

	PrintDebug ("%s(%d) Requested offset = %d \n",__FUNCTION__,__LINE__,offset );

	rt_offset =offset;
#if 0
	rt_offset =  HLS_SeekForNexus(offset);

	if( nRet !=  HLS_OK)
	{
		PrintError ("%s(%d) Error> HLS_SeekForNexus \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}
#endif
	PrintExit;
	return rt_offset;
}

/**
 * @b Function @b Description <br>
 * HLS 의 VFIO Bounds for Nexus 함수.
 *
 * @param[in] tHandle	: VFIO_Handle_t 핸들.
 * @param[in] file_size	: 파일사이즈.
 *
 * @see  VFIOBoundsForNexusCB 에 typecasting 된다.
 *
 * @todo 전체 파일사이즈를 어떻게 처리하여 리턴해 줄 것인가? 0만 리턴하지 않으면 괜찮을 것 같다.
 */
int DI_STREAM_HLS_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	PrintEnter;
	HLS_ERROR_e nRet;

	/*1. Controller Init  : 필수 */
	nRet = HLS_BoundsForNexus(file_size);
#if 0
	if( nRet !=  HLS_OK)
	{
		PrintError ("%s(%d) Error> HLS_BoundsForNexus \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}
#endif
	PrintExit;
	return DI_IPT_SUCCESS;
}
#endif

/**
 * @b Function @b Description <br>
 * PlayList 를 Update <br>
 * 현재 미완성 <br>
 *
 * @param[in] pParam : Session Handle
 * @param[in] unProgram : 대상 Program Index
 * @param[in] unStream :  대상 Stream Index
 * @todo 추가 작업 필요
 *
 * @return	# HLS_ERROR_e <br>
 */
HLS_ERROR_e P_STREAM_HLS_UpdatePlayList(HLS_HANDLE hHLS, HUINT32 unProgram , HUINT32 unStream)
{
	HINT32 nContentLength = 0;
	HINT8 *pM3U8Buff = NULL;
	HFLOAT32 flTimeGap = 0;
	m3u8_t* m3u8 = NULL;
	m3u8Node_t* playlistNode = NULL;
	m3u8Node_t* programNode = NULL;
	m3u8Node_t* streamNode = NULL;
	m3u8Playlist_t* pPlaylist = NULL;
	m3u8Program_t* pProgram = NULL;
	m3u8Stream_t* pStream = NULL;
	HUINT64 ullSegment = 0;
	HUINT64 ullCurSegment = 0;

	HINT32 nRet = DI_ERR_OK;
	HLS_t *pHLS = NULL;
	HBOOL bUpdated = FALSE;
	PrintEnter;
	if( hHLS == NULL )
	{
		PrintError("%s(%d) : Error> HLS Handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHLS);
	if( pHLS == NULL)
	{
		PrintError("%s(%d) : Error> HLS Handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if( pHLS->hCtrl == NULL )
	{
		PrintError("%s(%d) : Error> HLS Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	m3u8 = HLS_CONTROLLER_GetM3U8Handle(pHLS->hCtrl);
	if(m3u8 == NULL)
	{
		PrintError("%s(%d) : Error> M3U8 Handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	programNode = UTIL_DLLIST_Get_n_Node(&m3u8->programSet, unProgram);
	if(programNode != NULL && programNode->pData != NULL)
	{
		pProgram = (m3u8Program_t*) programNode->pData;
	}
	else
	{
		PrintError("%s(%d) : Error> Program(%u) is NULL\n",__FUNCTION__,__LINE__,unProgram);
		PrintExit;
		return HLS_FAIL;
	}
	streamNode = UTIL_DLLIST_Get_n_Node(&pProgram->streamSet , unStream);
	if(streamNode != NULL && streamNode->pData != NULL)
	{
		pStream = (m3u8Stream_t*) streamNode->pData;
	}
	else
	{
		PrintError("%s(%d) : Error> Stream(%u) is NULL\n",__FUNCTION__,__LINE__,unStream);
		PrintExit;
		return HLS_FAIL;
	}
	playlistNode = UTIL_DLLIST_GetHeadNode(&m3u8->playlistSet);
	while(playlistNode != NULL)
	{
		nContentLength = 0;
		pPlaylist = (m3u8Playlist_t*)playlistNode->pData;
		if(pPlaylist != NULL)
		{
			if(pPlaylist->nProgramId == (HINT32)pProgram->nProgramId && pPlaylist->pUrl != NULL)
			{
				if(pPlaylist->pStream == pStream)
				{
					while((nContentLength = P_STREAM_HLS_GetM3U8((HINT8 *)pPlaylist->pUrl ,&pM3U8Buff, NULL)) <= 0)
					{
						PrintError("%s(%d) : Error> P_STREAM_HLS_GetM3U8 Fail Retrying...\n",__FUNCTION__,__LINE__);
						#if 0
						if(unRetry >= HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
						{
							PrintError("%s(%d) : Error> P_STREAM_HLS_GetM3U8 Fail Exit...\n",__FUNCTION__,__LINE__);
							goto _updatectrl_error;
						}
						else
						{
							unRetry++;
							nContentLength = 0;
							if(pM3U8Buff != NULL)
							{
								DD_MEM_Free(pM3U8Buff);
								pM3U8Buff = NULL;
							}
						}
						#else
						goto _updatectrl_error;
						#endif
					}
					nRet = HLS_M3U8_Update(m3u8,pPlaylist,pM3U8Buff,nContentLength);
					if(nRet != DI_ERR_OK)
					{
				 	 	PrintError("%s(%d) : Error> HLS_M3U8_Update Fail ret : %d\n",__FUNCTION__,__LINE__,nRet);
				 	 	goto _updatectrl_error;
	 			 	}
	 			 	(void)HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &ullSegment);
					ullCurSegment = ullSegment;
	 			 	if(pPlaylist->ullSequenceNum < ullSegment)
	 			 	{
						ullSegment = pPlaylist->ullSequenceNum;
					}

//					if(ullSegment != pStream->ullSequenceNum && ullSegment > 0)
					{
#if 0
						if(HLS_M3U8_DiscardSegmentsAndKeys(pStream,ullSegment - 1) != M3U8_ERR_OK)
#else
#ifdef CONFIG_HLS_LIVE_RESTART
						if(!HLS_CONTROLLER_DiscardSegmentsAndKeysUntilPlayListSequenceNum(pHLS->hCtrl, pPlaylist->ullSequenceNum))
#else
						if(!HLS_CONTROLLER_DiscardSegmentsAndKeysUntilCurrent(pHLS->hCtrl))
#endif
#endif
					{
							PrintError("%s(%d) : Error> HLS_M3U8_Update(Discard) Fail\n",__FUNCTION__,__LINE__);
						}
						else
						{
							PrintDebug("%s(%d) : Debug> HLS_M3U8_Update(Discard)\n",__FUNCTION__,__LINE__);
//							pStream->ullSequenceNum = ullSegment;
							bUpdated = TRUE;
							HLS_M3U8_Print(m3u8);
						}
					}

#ifdef CONFIG_HLS_LIVE_RESTART
					if(!pStream->bEndList && !pHLS->bOnLiveRestart)
#else
					if(!pStream->bEndList)
#endif
					{
						flTimeGap = HLS_CONTROLLER_GetTimeGapBetweenCurAndLast(pHLS->hCtrl);
//						PrintError("%s(%d) Live Streaming --> CurSeg(%llu) LiveSeg(%llu) Time gap (%f)\n", __FUNCTION__, __LINE__, ullSegment, pPlaylist->ullSequenceNum, flTimeGap);
						PrintError("%s(%d) Live Gap %f\n", __FUNCTION__, __LINE__, flTimeGap);
						if(flTimeGap > HLS_CONST_MAX_TIMEGAP_FOR_CHECK_LIVE /* && ullSegment < pPlaylist->ullSequenceNum */)
						{
#ifdef CONFIG_HLS_LIVE_RESTART
// go to three target duration from the end time.
							PrintError("%s(%d) Jump three target duration from the end of Playlist for LIVE\n", __FUNCTION__, __LINE__);
							HLS_CONTROLLER_SetSegmentToLive(pHLS->hCtrl);
#else
							PrintError("%s(%d) Jump first segment in new update list for LIVE\n", __FUNCTION__, __LINE__);
							HLS_CONTROLLER_SetSegmentUntilDiscontinued(pHLS->hCtrl, pPlaylist->ullSequenceNum);
//							HLS_CONTROLLER_SetSegment(pHLS->hCtrl, pPlaylist->ullSequenceNum);
							ullSegment = pPlaylist->ullSequenceNum;
#endif
						}
#ifdef CONFIG_HLS_LIVE_RESTART
						if(pHLS->bNeedLiveRecovery && (flTimeGap > 3*HLS_CONTROLLER_GetCurStreamTargetDuration(pHLS->hCtrl)) )
						{

							PrintError("%s(%d) Jump three target duration from the end of Playlist for LIVE\n", __FUNCTION__, __LINE__);
							HLS_CONTROLLER_SetSegmentToLive(pHLS->hCtrl);
						}
#else
						if(pHLS->bNeedLiveRecovery && ullCurSegment != pPlaylist->ullSequenceNum )
						{

							PrintError("%s(%d) Jump first segment in new update list for LIVE\n", __FUNCTION__, __LINE__);
							HLS_CONTROLLER_SetSegmentUntilDiscontinued(pHLS->hCtrl, pPlaylist->ullSequenceNum);
							// HLS_CONTROLLER_SetSegment(pHLS->hCtrl, pPlaylist->ullSequenceNum);
							ullSegment = pPlaylist->ullSequenceNum;
						}
#endif
					}

	 			 	if(pPlaylist->bUpdated)
	 			 	{
						bUpdated = TRUE;
	 			 	}
				}
			}
		}
		else
		{
			PrintError("%s(%d) : Error> playlistNode is NULL\n",__FUNCTION__,__LINE__);
			return HLS_FAIL;
		}
		if(pM3U8Buff != NULL)
		{
			DD_MEM_Free(pM3U8Buff);
			pM3U8Buff = NULL;
		}
		playlistNode = UTIL_DLLIST_NextNode(playlistNode);
	}
	if(bUpdated != TRUE)
	{
		return HLS_M3U8_NOT_CHANGED;
	}
	return HLS_OK;
_updatectrl_error:
	if(pM3U8Buff != NULL)
	{
		DD_MEM_Free(pM3U8Buff);
		pM3U8Buff = NULL;
	}
	return HLS_FAIL;
}


/**
 * @b Function @b Description <br>
 * Live M3U8 Update를 위한 Task <br>
 * 현재 미완성 <br>
 *
 * @param[in] pParam : Session Handle
 * @todo 추가 작업 필요
 *
 * @return	# void <br>
 */
void P_STREAM_HLS_LiveUpdate_Task(void *pParam)
{
	HLS_ERROR_e eErr = HLS_FAIL;
	HLS_t* pHLS = NULL;
	HUINT32 unTime = 0;
	HUINT32 unMaxDelay = 0;
	UNUSED_PARAM(pParam);
	PrintEnter;
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return;
	}
	pHLS = __getHLS(g_hHLS);
	if(pHLS->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> HLS CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return;
	}
	pHLS->eLiveUpdateStatus = DI_IPTUNER_HLS_LIVE_UPDATE_STARTED;
	pHLS->unDelayForUpdate = HLS_CONST_DEFAULT_UPDATEDELAY;
	pHLS->bIsPrebuffering = FALSE;

	/* prebuffering task 및 DestroyLiveUpdateTask 가 호출 될 때가지 live update task가 진행 안되도록 잡고 있다. */
	while(pHLS->bIsPrebuffering==FALSE)
	{
		VK_TASK_Sleep(HLS_CONST_WAITING / 2);
	}

	/* Task Started Flag */
	do
	{
		if( pHLS->bTriggerUpdate != TRUE )
		{
			VK_TASK_Sleep(HLS_CONST_WAITING / 2);
			unTime+=(HLS_CONST_WAITING / 2);
			continue;
		}
		else
		{
			pHLS->eLiveUpdateStatus = DI_IPTUNER_HLS_LIVE_UPDATE_TRIGGERED;
			PrintDebug("%s(%d) Debug> Triggered !! %u:%u(Delay %u ms) \n",__FUNCTION__,__LINE__,pHLS->unProgramIdxForUpdate,pHLS->unStreamIdxForUpdate,pHLS->unDelayForUpdate);
		}

		if( pHLS->unDelayForUpdate > unTime && pHLS->bForcedTriggerUpdate == FALSE )
		{
			VK_TASK_Sleep(HLS_CONST_WAITING);
			unTime+=(HLS_CONST_WAITING);
		}
		else
		{
			unTime = 0;
			VK_SEM_Get(pHLS->ulDownloadingSema);	// caution : Should be blocking download sema before blocked LiveSema.
			VK_SEM_Get(pHLS->ulLiveSema);
			pHLS->eLiveUpdateStatus = DI_IPTUNER_HLS_LIVE_UPDATE_UPDATING;
			eErr = P_STREAM_HLS_UpdatePlayList(pHLS,pHLS->unProgramIdxForUpdate,pHLS->unStreamIdxForUpdate);
			if( eErr == HLS_M3U8_NOT_CHANGED )
			{
				VK_TASK_Sleep(HLS_CONST_WAITING);
				if(pHLS->unDelayForUpdate <= HLS_CONST_DEFAULT_RETRY_UPDATEDELAY)
				{
					pHLS->unDelayForUpdate = HLS_CONST_DEFAULT_RETRY_UPDATEDELAY;
				}
				else
				{
					pHLS->unDelayForUpdate /= 2;
				}
				PrintDebug("%s(%d) Debug> NOT Changed !!\n",__FUNCTION__,__LINE__);
			}
			else if( eErr == HLS_OK )
			{
				pHLS->eLiveUpdateStatus = DI_IPTUNER_HLS_LIVE_UPDATE_STARTED;
				if(pHLS->bForcedTriggerUpdate == TRUE)
				{
					pHLS->bForcedTriggerUpdate = FALSE;
				}
//				pHLS->unDelayForUpdate = 0;
//				pHLS->bTriggerUpdate = FALSE;
//				unMaxDelay = 0;
				PrintDebug("%s(%d) Debug> Updated !!\n",__FUNCTION__,__LINE__);
				HLS_M3U8_Print(HLS_CONTROLLER_GetM3U8Handle(pHLS->hCtrl));
			}
			else
			{
				unTime = 0;
			}
			if(pHLS->unDelayForUpdate == 0)
			{
				pHLS->unDelayForUpdate = HLS_CONST_DEFAULT_UPDATEDELAY;
			}
			VK_SEM_Release(pHLS->ulLiveSema);
			VK_SEM_Release(pHLS->ulDownloadingSema);
		}
	}
	while (g_ePreBufferingTaskChangeState < DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING);
	/* Task Terminate Flag */
	pHLS->eLiveUpdateStatus = DI_IPTUNER_HLS_LIVE_UPDATE_TERMINATED;
	PrintExit;
}

void P_STREAM_HLS_RequestUpdate(HLS_HANDLE hHls, HUINT32 unProgramId, HUINT32 unStreamId, HUINT32 unDelay, HBOOL bForce)
{

	HLS_t* pHLS = NULL;
	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}
	pHLS = __getHLS(hHls);
	VK_SEM_Get(pHLS->ulLiveSema);
	pHLS->bForcedTriggerUpdate = bForce;
	pHLS->bTriggerUpdate = TRUE;
	pHLS->unProgramIdxForUpdate = unProgramId;
	pHLS->unStreamIdxForUpdate = unStreamId;
	pHLS->unDelayForUpdate = unDelay;
	PrintDebug("%s(%d) Debug> Live Update Request %u:%u(Delay %u ms) \n",__FUNCTION__,__LINE__,unProgramId,unStreamId,unDelay);
	VK_SEM_Release(pHLS->ulLiveSema);
	PrintExit;
	return;
}

#if __HLS_VFIO_FUNC__
#endif

/**
 * @brief       "OpenFromNexus"
 * @param	path : path of file
 * @param       flags :
 * @param	ptHandle : VFIO Handle
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/* Open From Nexus */
int P_STREAM_HLS_OpenFromNexus (const HINT8 *pUrl, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	HINT32 nRet = DI_ERR_OK;
	UNUSED_PARAM (flags);
	UNUSED_PARAM (pUrl);
	UNUSED_PARAM (ptHandle);
//	HINT8 tmpString[256]; /* for naming */
	DI_IPTUNER_Msg_t s_Msg;

#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
	HLS_t *pHLS = NULL;
	//	PrintEnter;
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_SUCCESS;
	}
	pHLS = __getHLS(g_hHLS);
	PrintData ("%s (%d) Buffering Start from OpenNexusCallback\n",__FUNCTION__,__LINE__);
	pHLS->eIO_Type = HLS_IO_TYPE_PLAYBACK;

	if(HLS_CONTROLLER_NextSegment(pHLS->hCtrl) == HLS_FAIL)
	{
		g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING;
	}
	else
	{
		g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_READY;
	}
	g_ePreBufferingTaskChangeState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_READY;
	(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));
	s_Msg.unMsg = TUNER_STREAM_MSG_HLS_BUFFERING;
	s_Msg.unParam1 = DI_IPTUNER_STREAM_HLS;
	(void) VK_MSG_Send ( s_ulStreamMsgQId, (HUINT32 *)&s_Msg, sizeof(DI_IPTUNER_Msg_t) ) ;
	PrintExit;
	return nRet;
}

/**
 * @brief       "SeekFromNexus"
 * @param	tHandle : VFIO Handle
 * @param       offset : offset for seeking
 * @param	whence : whence
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/* Seek From Nexus : Long jump 인 경우 어느정도는 block 되어도 된다.
 * 단 Read 함수에서는 block 되면 waiting IO error, 0 return 시 endofStream, -1 return시 errorofstream 이 뜬다.*/
off_t P_STREAM_HLS_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HLS_t *pHLS = NULL;
//	HINT64 nStreamLength = 0;
	// HINT32 nSegment = 0;
//	HINT32 nQueueRet = DI_ERR_OK;
//	HINT32 nCurSegment = 0;
	// HUINT64 ullBitrate = 0;
//	STREAM_ACCESS_t *pAccess = NULL;
//	off_t ret = 0;
//	off_t readPos = 0;
//	off_t writePos = 0;
//	off_t realOffset = 0;
	UNUSED_PARAM(whence);
	UNUSED_PARAM(tHandle);
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> HLS Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_FAIL;
	}
//	PrintEnter;
	pHLS = __getHLS(g_hHLS);
	pHLS->ullVirtureOffset = offset;

	/* not supported */
//	PrintExit;
	return offset;
}

/**
 * @brief       "BoundsFromNexus"
 * @param	tHandle : VFIO Handle
 * @param       file_size : size of content
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/* Bounds From Nexus */
int P_STREAM_HLS_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
//	HUINT32 ulDuration = 0;
//	HUINT32 ulMaxBitrate = 0;
//	DI_IPTUNER_t *pTunerHandle = NULL;
//	PrintEnter;
	UNUSED_PARAM(tHandle);
#if 0
	*file_size = SSTR_CONTROLLER_GetStreamLength(hSstr);
	if(*file_size <= 0)
	{
		*file_size = 0xFFFFFFFFFFFFFF;
	}
#else
	HLS_t *pHLS = NULL;
//	PrintEnter;
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_SUCCESS;
	}
	pHLS = __getHLS(g_hHLS);
	if(pHLS->eIO_Type == HLS_IO_TYPE_PROBE)
	{
		if(pHLS->pMemFd != NULL)
		{
			*file_size = mem_file_bound(pHLS->pMemFd);
		}
	}
	else
		*file_size = 0xFFFFFFFFFFFFFF;
#endif
//	PrintExit;
	return nRet;
}
static HBOOL HexString2Char(HINT8 const* szHex, HUINT8* r,HINT32 len)
{
	HUINT8 rch = *r;
	if(len <= 0)
	{
		return FALSE;
	}
	if(len > 1)
	{
		if(*szHex >= '0' && *szHex <= '9')
		{
			rch = *szHex - '0';
		}
		else if(*szHex >= 'A' && *szHex <= 'F')
		{
			rch = *szHex - 55; //-'A' + 10
		}
		else if(*szHex >= 'a' && *szHex <= 'f')
		{
			rch = *szHex - 87; //-'a' + 10
		}
		else
		{
			return FALSE;
		}
		szHex++;
	}
	if(*szHex >= '0' && *szHex <= '9')
	{
		rch = (rch << 4) + (*szHex - '0');
	}
	else if(*szHex >= 'A' && *szHex <= 'F')
	{
		rch = (rch << 4) + (*szHex - 55); //-'A' + 10;
	}
	else if(*szHex >= 'a' && *szHex <= 'f')
	{
		rch = (rch << 4) + (*szHex - 87); //-'a' + 10;
	}
	else
	{
		return FALSE;
	}
	*r = rch;
	return TRUE;
}


static HBOOL HexString2Binary(HINT8 *pszHexStr, HUINT8* pucBinStr, HINT32 nBinSize)
{
	HINT32 i = 0;
	HINT32 j = 0;
	HUINT8 ch = 0;
	HINT8 *p = NULL;
	HINT32 n = 0;
	if(pszHexStr == NULL || pucBinStr == NULL)
	{
		return FALSE;
	}

	if((p = (HINT8 *)VK_strstr((char *)pszHexStr,"0x")) != NULL || (p = (HINT8 *)VK_strstr((char *)pszHexStr,"0X")) != NULL )
	{
		p += 2;
	}
	else
	{
		p = (HINT8 *)pszHexStr;
	}
	n = VK_strlen(p);
	if(n <= 0)
	{
		return FALSE;
	}
	for(i=0 ; i < nBinSize ; i++)
	{
		if(HexString2Char(p + j, &ch,n - j) == FALSE)
		{
			return FALSE;
		}
		*(pucBinStr + i) = ch;
		j += 2;
		if(j >= n)
		{
			break;
		}
	}
	return TRUE;
}

#ifdef USE_HLS_SEEK_IMPROVEMENT
int P_STREAM_HLS_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HUINT32 unSegSize = 0;
	size_t nSize = 0;
	HINT32  vRet = HLS_OK;
	HLS_t *pHLS = NULL;
	HUINT32 unStartTick = 0;
	HUINT32 unEndTick = 0;
	HUINT32 ulBitrate = 0;
	HINT8 *pUrl = NULL;

	STREAM_ACCESS_t *pSegmentAccess = NULL;

	HUINT8 *pSrcBlock = NULL;
	void *pDstBlock = NULL;
	HINT32 nBlockLen = 0;

	NEXUS_Error nexusError;

	PrintEnter;
	UNUSED_PARAM(path);
	UNUSED_PARAM(mode);
	UNUSED_PARAM(ptHandle);
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pHLS = __getHLS(g_hHLS);
	(void)mem_file_close(pHLS->pMemFd);

	ulBitrate = HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl);
	HLS_CONTROLLER_GearHighest(pHLS->hCtrl);
	pUrl = HLS_CONTROLLER_GetCurSegmentUrl(pHLS->hCtrl);
	PrintDebug("%s(%d) Initialize Url : %s\n",__FUNCTION__,__LINE__,pUrl);
	if(pUrl == NULL)
	{
		return HLS_FAIL;
	}
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	HLS_ERROR_e eDownloadErr = HLS_FAIL;
	HUINT32 bTimeout = FALSE;

	nBlockLen = HLS_CONST_DEFAULT_DMABLOCK;
	nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pSrcBlock);
	if(nexusError != NEXUS_SUCCESS)
	{
		if(nBlockLen > 0)
		{
			nBlockLen = nBlockLen / 2;
		}
		else
		{
			PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
			return HLS_FAIL;
		}
		nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pSrcBlock);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n", __FUNCTION__,__LINE__,nexusError);
			return 0;
		}
	}
	nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pDstBlock);
	if(nexusError != NEXUS_SUCCESS)
	{
		if(pSrcBlock != NULL)
		{
			NEXUS_Memory_Free(pSrcBlock);
			if(nBlockLen > 0)
			{
				nBlockLen = nBlockLen / 2;
			}
			else
			{
				PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
				return 0;
			}
			nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pSrcBlock);
			if(nexusError != NEXUS_SUCCESS)
			{
				PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
				return 0;
			}
			nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pDstBlock);
			if(nexusError != NEXUS_SUCCESS)
			{
				if(pSrcBlock != NULL)
				{
					NEXUS_Memory_Free(pSrcBlock);
				}
				PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
				return 0;
			}
		}
	}
#ifdef CONFIG_HLS_LIVE_RESTART
		HLS_CONTROLLER_SetSegmentByTime(pHLS->hCtrl, 0); // probing은 처음 segment로 한다.
#endif

#if 1 // __SUPPORT_BYTERANGE__
	pSegmentAccess = pHLS->pSegmentAccess;

	if(pSegmentAccess != NULL)
	{
		(void)STREAM_HTTP_Close (pSegmentAccess);

		if ( pSegmentAccess->psz_path != NULL )
		{
			DD_MEM_Free(pSegmentAccess->psz_path);
			pSegmentAccess->psz_path = NULL;
		}
		DD_MEM_Free(pSegmentAccess);
		pSegmentAccess = NULL;
	}
#endif

	while((eDownloadErr = P_HLS_DownloadWithPlay( -1, 0, pDstBlock, pSrcBlock, nBlockLen)) != HLS_OK)
	{
		if(g_ePreBufferingTaskState >= DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
		{
			PrintError("%s(%d) User Exit. So Break Buffering\n",__FUNCTION__,__LINE__);
			PrintExit;
			return HLS_FAIL;
		}
		if(eDownloadErr == HLS_PANIC_DETECTED
		|| HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) != HLS_CONTROLLER_GetCurMinBitrate(pHLS->hCtrl))
		{
			HLS_CONTROLLER_GearLowest(pHLS->hCtrl);
		}
		else if(eDownloadErr == HLS_TIMEOUT)
		{
			if(bTimeout == TRUE)
			{
#ifdef CONFIG_HLS_LIVE_RESTART
				HLS_CONTROLLER_Setup(pHLS->hCtrl,0,0,0); // lowest로 안바뀌는 상황이 있어, 낮은 비트레이트 스트림의 첫번째 segment로 강제 설정
#else
				HLS_CONTROLLER_GearLowest(pHLS->hCtrl);
#endif
			}
			else
			{
				bTimeout = TRUE;
			}
		}
		else
		{
			PrintError("%s(%d) Infinity Try\n",__FUNCTION__,__LINE__);
		}
	}

#ifdef CONFIG_HLS_LIVE_RESTART
	pHLS->unProbingBandwidth = HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) ;
	HLS_CONTROLLER_GetCurStreamIndex(pHLS->hCtrl, &(pHLS->unProbingStreamIndex));
	HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &(pHLS->unProbingSegmentID));
#endif


	pHLS->pMemFd = mem_file_open(pHLS->sz_SegSize);
	if(pHLS->pMemFd == NULL)
	{
		(void)mem_file_close(pHLS->pMemFd);
		pHLS->pMemFd = NULL;
		PrintExit;
		return HLS_FAIL;
	}
	nSize = mem_file_write(pHLS->pMemFd,pHLS->pSegBuff,pHLS->sz_SegSize);
	if(nSize != pHLS->sz_SegSize)
	{
		(void)mem_file_close(pHLS->pMemFd);
		pHLS->pMemFd = NULL;
		PrintExit;
		return HLS_FAIL;
	}
	mem_file_seek(pHLS->pMemFd,0);
	unSegSize = mem_file_bound(pHLS->pMemFd);

	unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */

	if(eDownloadErr == HLS_OK){
		pHLS->eIO_Type = HLS_IO_TYPE_PROBE;
	} else {
		pHLS->eIO_Type = HLS_IO_TYPE_NONE;
	}
#ifdef HLS_CONFIG_WRITE_SEGMENT_FOR_TEST
	{
//		HUINT32 unSegmentNum = 0;
		HUINT8 strBuf[32] = {0};
		VK_sprintf(strBuf,"/home/segment_init");
		pHLS->fd=open(strBuf,O_CREAT|O_TRUNC|O_WRONLY|O_SYNC,0666);
		write(pHLS->fd,pHLS->pMemFd->buf, unSegSize);
		close(pHLS->fd);
	}
#endif

	if(pSrcBlock != NULL)
	{
		NEXUS_Memory_Free(pSrcBlock);
		pSrcBlock = NULL;
	}
	if(pDstBlock != NULL)
	{
		NEXUS_Memory_Free(pDstBlock);
		pDstBlock = NULL;
	}

	PrintExit;
	return vRet;
}
#endif

/** */
int P_STREAM_HLS_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
//	STREAM_ACCESS_t *pAccess = NULL;
	UNUSED_PARAM(tHandle);
	HLS_t *pHLS = NULL;
	PrintEnter;
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pHLS = __getHLS(g_hHLS);
	(void)mem_file_close(pHLS->pMemFd);
	pHLS->pMemFd = NULL;
	/* Now we just get the Stream handler */
	PrintExit;
	return vRet;
}

/**
 * @brief       "Read" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @param       ptr : pointer of buffer
 * @param	size : size of memory for read
 * @param	nmemb : size of memory for read
 * @param	ptReadSize : size of byte
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int P_STREAM_HLS_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	UNUSED_PARAM(tHandle);
	HBOOL bIsEOS = FALSE;
	size_t sz_Try = size*nmemb;
	size_t cur=0;
	size_t sz_Read=0;
	size_t sz_Temp=0;
	int nRead=0;
	int ret = VFIO_ERROR_SUCCESS;
//	int nRet = VFIO_ERROR_SUCCESS;
	SSTR_BUFFERMGR_t* pBufferMgr = NULL;
	HLS_t *pHLS = NULL;
	PrintEnter;
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_FAIL;
	}
	pHLS = __getHLS(g_hHLS);
	if(pHLS->eIO_Type == HLS_IO_TYPE_PROBE)
	{
		if(pHLS->pMemFd != NULL)
		{
			*ptReadSize = mem_file_read(pHLS->pMemFd, ptr, sz_Try);
//			PrintExit;
			return ret;
		}
	}

	/* Now we just get the Stream handler */
	// PrintDebug("%s(%d) : Debug> Try to Read %d \n",__FUNCTION__,__LINE__,sz_Try);
	pBufferMgr = HLS_CONTROLLER_GetBufferMgr(pHLS->hCtrl);
	sz_Read = sz_Try;
	VK_MEM_Memset(ptr,0x00,sz_Read);
/*
	if(pHLS->bImmediatelyExit)
	{
		pHLS->bImmediatelyExit = FALSE;
	}
*/
	while(sz_Read > 0)
	{
		if(pHLS->bImmediatelyExit)
		{
			pHLS->bImmediatelyExit = FALSE;
			break;
		}

		if(P_STREAM_HLS_IsSeekRequested(pHLS))
		{
		#if 0
			cur = 0;
		#else
			cur = sz_Try;
			VK_MEM_Memset(ptr,0xff,sz_Try);
			break;
		#endif
		}
		if(SSTR_BUFFERMGR_GetReadableSize(pBufferMgr) < sz_Read)
		{
			if(g_ePreBufferingTaskState > DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED) /* || SSTR_BUFFERMGR_GetWritableSize(pBufferMgr) <= 0)*/
			{
				sz_Temp = SSTR_BUFFERMGR_GetReadableSize(pBufferMgr);
				if(sz_Temp == 0)
				{
					if(g_ePreBufferingTaskState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING)
					{
						if(HLS_CONTROLLER_IsCurSegmentEndList(pHLS->hCtrl) == TRUE)
						{
							break;
						}
						else
						{
							VK_TASK_Sleep(10);
							continue;
						}
					}
 					else if(g_ePreBufferingTaskState >= DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
					{
						break;
					}
				}
				if(sz_Temp < sz_Read)
				{
					sz_Read = sz_Temp;
					bIsEOS = TRUE;
				}
			}
			else
			{
				VK_TASK_Sleep(10);
//				PrintDebug("%s(%d) : Debug> BufferUnderrun : %"PRId64"\n",__FUNCTION__,__LINE__,SSTR_BUFFERMGR_GetReadableSize(pBufferMgr) );
				continue;
			}
		}
		nRead = SSTR_BUFFERMGR_Read(pBufferMgr, (HUINT8 *)ptr + cur, sz_Read);
		if(nRead == 0)
		{
			if(g_ePreBufferingTaskState > DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED)
			{
				break;
			}
			else
			{
				//VK_TASK_Sleep(10);
				continue;
			}
		}
		cur+=nRead;
		sz_Read-=nRead;
		// TODO: must add Break Condition ex) End of Stream, Stop to play and so on
	}
	if(!bIsEOS)
	{
		*ptReadSize = cur;
	}
	else
	{
		*ptReadSize = sz_Try;
	}
//	PrintDebug("%s(%d) : Debug> Readsize : %d \n",__FUNCTION__,__LINE__,cur);
//	stream_read_error:
//	PrintExit;
	return ret;
}

/**
 * @brief       "Seek" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @param       offset : offset for seeking
 * @param	whence : whence
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/** This Function called by bmedia probe only.*/
int P_STREAM_HLS_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	UNUSED_PARAM(tHandle);
	UNUSED_PARAM(offset);
	UNUSED_PARAM(whence);
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	HLS_t *pHLS = NULL;
//	PrintEnter;
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_SUCCESS;
	}
	pHLS = __getHLS(g_hHLS);
	if(whence == SEEK_END)
	{
		offset = mem_file_bound(pHLS->pMemFd);
	}

	if(pHLS->eIO_Type == HLS_IO_TYPE_PROBE)
	{
		if(pHLS->pMemFd != NULL)
		{
			mem_file_seek(pHLS->pMemFd, offset);
		}
	}
	return nRet;
}

/**
 * @brief       "Tell" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @param       plCurOffset : current offset
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int P_STREAM_HLS_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	UNUSED_PARAM(tHandle);
	PrintEnter;
	HLS_t *pHLS = NULL;
	PrintEnter;
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> HLS Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_FAIL;
	}
	pHLS = __getHLS(g_hHLS);

	if(pHLS->eIO_Type == HLS_IO_TYPE_PROBE)
	{
		if(pHLS->pMemFd != NULL)
		{
			*plCurOffset = mem_file_tell(pHLS->pMemFd);
		}
	}
	else
		*plCurOffset = pHLS->ullVirtureOffset;
	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/**
 * @brief       "IsEOF" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @return     	bool
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

/** if EOF is true, must be return -1 like feof*/
int P_STREAM_HLS_IsEOF (VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
//	PrintEnter;
	if(g_hHLS == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError("%s(%d) : Error> HLS Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	/* Now we just get the Stream handler */
#if 0
	pAccess = &stHttpAccessForSstr;
    if(TRUE == pAccess->info.b_eof)
	{
		nRet = -1; /* must be -1 like feof*/
	}
#endif
//	PrintExit;
	return nRet;
}


#if __HLS_GLOBAL_FUNC_BEGIN__
#endif

HINT8* DI_STREAM_HLS_InitSegUrl()
{
	HLS_t *pHLS = __getHLS(g_hHLS);
	if(pHLS == NULL)
	{
		return NULL;
	}
	return pHLS->pInitSegUrl;
}

#if 0
HINT8* DI_STREAM_HLS_InitSegKeyUrl()
{
	HLS_t *pHLS = __getHLS(hHLS);
	if(pHLS == NULL)
	{
		return NULL;
	}
	return pHLS->pInitSegKeyUrl;
}
#endif

HUINT32 DI_STREAM_HLS_GetBufferingState()
{
	return g_ePreBufferingTaskState;
}

void DI_STREAM_HLS_SetBufferingState(HUINT32 nState)
{
	HLS_t *pHLS = __getHLS(g_hHLS);
	if(pHLS == NULL)
	{
		return;
	}
	if(nState ==DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
	{
		pHLS->bImmediatelyExit = TRUE;
	}
	g_ePreBufferingTaskState = nState;
	return;
}

void DI_STREAM_HLS_StopBufferingTask()
{
	HLS_t *pHLS = __getHLS(g_hHLS);
	if(pHLS == NULL)
	{
		return;
	}
	pHLS->bImmediatelyExit = TRUE;
	g_ePreBufferingTaskChangeState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING;
	return;
}

HLS_ERROR_e	DI_STREAM_HLS_GetMediaInfo (DI_MEDIA_INFO_t *pstMediaInfo)
{
	HUINT32 duration = 0;
	HUINT32 max_bitrate = 0;
	HLS_t *pHLS = __getHLS(g_hHLS);
#ifdef CONFIG_HLS_UNDERFLOW_MONITORING_FOR_BBC_CATAL
	HUINT32 min_bitrate = 0;
#endif
	if(pHLS == NULL)
	{
		return HLS_FAIL;
	}
	if(pHLS->hCtrl == NULL)
	{
		return HLS_FAIL;
	}
	if( HLS_CONTROLLER_IsCurStreamEndList(pHLS->hCtrl) == TRUE )
	{
		duration = HLS_CONTROLLER_GetCurStreamDuration(pHLS->hCtrl);
		if(duration > 0)
		{
			pstMediaInfo->duration = duration * 1000; /* To Millisec */
		}
	}
	else
	{
		pstMediaInfo->duration = 0xFFFFFFFF;
	}
	max_bitrate = HLS_CONTROLLER_GetCurMaxBitrate(pHLS->hCtrl);
	if(max_bitrate > 0)
	{
		pstMediaInfo->max_bitrate = max_bitrate;
	}
#ifdef CONFIG_HLS_UNDERFLOW_MONITORING_FOR_BBC_CATAL
	min_bitrate = HLS_CONTROLLER_GetCurMinBitrate(pHLS->hCtrl);
	if(min_bitrate > 0)
	{
		pstMediaInfo->min_bitrate = min_bitrate;
	}
#endif
	return HLS_OK;
}


/**
 * @brief       register callback
 * @return      void
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
void DI_STREAM_HLS_RegisterCallback ()
{
	VFIO_Callback_t stCBFunction;

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB)	P_STREAM_HLS_Open;
	stCBFunction.Close			= (VFIOCloseCB)	P_STREAM_HLS_Close;
	stCBFunction.Read			= (VFIOReadCB)	P_STREAM_HLS_Read;
	stCBFunction.Seek			= (VFIOSeekCB) 	P_STREAM_HLS_Seek;
	stCBFunction.Tell			= (VFIOTellCB) 	P_STREAM_HLS_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) P_STREAM_HLS_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 P_STREAM_HLS_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 P_STREAM_HLS_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) P_STREAM_HLS_BoundsForNexus;

	(void) VFIO_RegisterCallback (VFIO_CALLBACK_TYPE_STREAMING_HLS, stCBFunction);

	PrintExit;
	return;
}

/**
 * @brief       unregister callback
 * @return      void
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
void DI_STREAM_HLS_UnregisterCallback ()
{
	PrintEnter;
	(void)VFIO_UnregisterCallback (VFIO_CALLBACK_TYPE_STREAMING_HLS);
	PrintExit;
	return;
}

DI_ERR_CODE DI_STREAM_HLS_Control ( DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 )
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	SSTR_BUFFERMGR_t* pBufferMgr = NULL;
	HLS_t* pHLS = NULL;
	PrintEnter;
	UNUSED_PARAM(val2);
	if ( cmd == CMD_CONTROL_SEEK )
	{
		if(P_STREAM_HLS_IsValidTimeForSeek(g_hHLS, val))
		{
			pHLS = __getHLS(g_hHLS);
			pBufferMgr = HLS_CONTROLLER_GetBufferMgr(pHLS->hCtrl);
			if(pBufferMgr == NULL)
			{
				PrintError("%s(%d) Error> pBufferMgr is NULL\n", __FUNCTION__, __LINE__);
				return DI_ERR_ERROR;
			}
			SSTR_BUFFERMGR_SetReadLock(pBufferMgr, TRUE);
			nRet = P_STREAM_HLS_SetTimeForSeek(g_hHLS, val);
			if(nRet != DI_ERR_OK)
			{
				return nRet;
			}
			nRet = P_STREAM_HLS_SetRequestSeek(g_hHLS, TRUE);
		}
		else
		{
			return DI_ERR_ERROR;
		}
	}
	else if ( cmd == CMD_CONTROL_SPEED )
	{
		/* not supported yet */
	}
	PrintExit;

	return nRet;
}

#if defined(CONFIG_SES_HDPLUS_SMARTTV)

DI_ERR_CODE DI_STREAM_HLS_GetTimeBySegment(const HUINT64 pullSegmentId, HUINT64 * ullTime)
{

	HLS_ERROR_e nRet=HLS_OK;
	HLS_t *pHLS = __getHLS(g_hHLS);

	PrintEnter;
	if(pHLS == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
	nRet = HLS_CONTROLLER_GetTimeBySegment(pHLS->hCtrl, pullSegmentId, ullTime);

	if(nRet != HLS_OK)
	{
		PrintError("%s(%d) Error> Get ime by segment error in DI_STREAM_HLS_Control\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;

}
#endif //CONFIG_SES_HDPLUS_SMARTTV
int DI_STREAM_HLS_OpenController(HINT8 * pUrl)
{
	HUINT32 unBufferSize = 0;
	HINT32 nContentLength = 0;
	HINT8 *pM3U8Buff = NULL;
	HINT8 sem_name[128]={0};
	HUINT32 unRetry = 0;
	m3u8_t* m3u8 = NULL;
	m3u8Node_t* playlistNode = NULL;
	m3u8Playlist_t* pPlaylist = NULL;
	HINT32 nRet = DI_ERR_OK;
	HLS_t *pHLS = NULL;
	HINT8 * pHostUrl = NULL;
	HUINT64 strSize = 0;
	PrintEnter;
	if(pUrl == NULL)
	{
		PrintError("%s(%d) : Error> invalid pUrl\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if( g_hHLS != NULL )
	{
		PrintError("%s(%d) : Error> HLS HANDLE is already opened\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	strSize = VK_strlen(pUrl);
	pHostUrl = VK_MEM_Alloc(strSize + 1);
	if(pHostUrl == NULL)
	{
		PrintError("%s(%d) Error> Heap Memory Allocation Fail. size (%"PRId64") !!\n",__FUNCTION__,__LINE__,strSize + 1);
		goto _openctrl_error;
	}
	VK_memset(pHostUrl ,0x00, strSize + 1);
	while((nContentLength = P_STREAM_HLS_GetM3U8(pUrl,&pM3U8Buff, pHostUrl)) <= 0)
	{
		PrintError("%s(%d) : Error> P_STREAM_HLS_GetM3U8 Fail Retrying...\n",__FUNCTION__,__LINE__);
		if(unRetry >= HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
		{
			PrintError("%s(%d) : Error> P_STREAM_HLS_GetM3U8 Fail Exit...\n",__FUNCTION__,__LINE__);
			goto _openctrl_error;
		}
		else
		{
			unRetry++;
			nContentLength = 0;
			if(pM3U8Buff != NULL)
			{
				DD_MEM_Free(pM3U8Buff);
				pM3U8Buff = NULL;
			}
		}
	}

	PrintError("%s(%d) : Create M3U8 : pHostUrl : %s \n",__FUNCTION__,__LINE__, pHostUrl);

	m3u8 = HLS_M3U8_Create(pM3U8Buff, nContentLength, pUrl, pHostUrl);
	if(m3u8 == NULL)
	{
		PrintError("%s(%d) : Error> m3u8 open fail\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}
	playlistNode = UTIL_DLLIST_GetHeadNode(&m3u8->playlistSet);
	while(playlistNode != NULL)
	{
		nContentLength = 0;
		pPlaylist = (m3u8Playlist_t*)playlistNode->pData;
		if(pPlaylist->pUrl != NULL && pPlaylist->bMasterPlaylist == FALSE)
		{
			if(pM3U8Buff != NULL)
			{
				DD_MEM_Free(pM3U8Buff);
				pM3U8Buff = NULL;
			}
#if 0
			nContentLength = P_STREAM_HLS_GetM3U8(pPlaylist->pUrl,&pM3U8Buff);
			if(nContentLength <= 0)
			{
				PrintError("%s(%d) : Error> P_STREAM_HLS_GetM3U8 Fail Exit...\n",__FUNCTION__,__LINE__);
	 			goto _openctrl_error;
			}
#else
			while((nContentLength = P_STREAM_HLS_GetM3U8(pPlaylist->pUrl,&pM3U8Buff, NULL)) <= 0)
	 		{
	 			PrintError("%s(%d) : Error> P_STREAM_HLS_GetM3U8 Fail Retrying...\n",__FUNCTION__,__LINE__);
	 			if(unRetry >= HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
	 			{
	 				PrintError("%s(%d) : Error> P_STREAM_HLS_GetM3U8 Fail Exit...\n",__FUNCTION__,__LINE__);
	 				goto _openctrl_error;
	 			}
	 			else
	 			{
	 				unRetry++;
	 				nContentLength = 0;
	 				if(pM3U8Buff != NULL)
					{
						DD_MEM_Free(pM3U8Buff);
						pM3U8Buff = NULL;
					}
	 			}
	 		}
#endif
		 	nRet = HLS_M3U8_Update(m3u8,pPlaylist,pM3U8Buff,nContentLength);
		 	if(nRet != DI_ERR_OK)
		 	{
		 	 	PrintError("%s(%d) : Error> HLS_M3U8_Update Fail ret : %d\n",__FUNCTION__,__LINE__,nRet);
	 	 	}
		}
		 	playlistNode = UTIL_DLLIST_NextNode(playlistNode);
		}
//	HLS_M3U8_Print(m3u8);
	pHLS = (HLS_t*)DD_MEM_Alloc(sizeof(HLS_t));
	if(pHLS == NULL)
{
		PrintError("%s(%d) : Error> HLS HANDLE Allocation Fail\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}
	else
	{
		VK_memset(pHLS,0x00,sizeof(HLS_t));
	}
	pHLS->hCtrl = HLS_CONTROLLER_Open(m3u8);
	if(pHLS->hCtrl == NULL)
	{
		PrintError("%s(%d) : Error> HLS_CONTROLLER_Open Fail\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}
	VK_snprintf((char*)sem_name,VK_strlen("HLS_SEM_")+10, "HLS_SEM_%p", (void*)pHLS); /* + 10 is AddressSize */
//	PrintData ("%s(%d) : sem name = %s \n",__FUNCTION__,__LINE__, &sem_name);
	nRet = VK_SEM_Create(&pHLS->ulSema, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
		goto _openctrl_error;
	}
	VK_snprintf((char*)sem_name,VK_strlen("HLS_LiveSema_")+10, "HLS_LiveSema_%p", (void*)pHLS);
	//	PrintData ("%s(%d) : sem name = %s \n",__FUNCTION__,__LINE__, &sem_name);
	nRet = VK_SEM_Create(&pHLS->ulLiveSema, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
		goto _openctrl_error;
	}

	VK_snprintf((char*)sem_name,VK_strlen("HLS_DownloadSema_")+10, "DownloadSema_%p", (void*)pHLS);
	nRet = VK_SEM_Create(&pHLS->ulDownloadingSema, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
		goto _openctrl_error;
	}

	while(pHLS->pSegBuff == NULL)
	{
	pHLS->ulBuffSize = HLS_CONTROLLER_GetCurMaxBitrate(pHLS->hCtrl);
	if( pHLS->ulBuffSize <= 0 )
	{
		pHLS->ulBuffSize = HLS_CONST_DEFAULT_SEGBUFF;
			pHLS->pSegBuff = DD_MEM_Alloc(pHLS->ulBuffSize);
			if(pHLS->pSegBuff == NULL)
			{
				PrintError("%s(%d) : Error> Segment Buffer Allocation Fail\n",__FUNCTION__,__LINE__);
				goto _openctrl_error;
			}
			else
				break;
	}
	else
	{
		pHLS->ulBuffSize = pHLS->ulBuffSize / 8 * (HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl) + HLS_CONST_PADDING);
		if( pHLS->ulBuffSize <= 0 )
		{
			pHLS->ulBuffSize = HLS_CONST_DEFAULT_SEGBUFF;
			pHLS->pSegBuff = DD_MEM_Alloc(pHLS->ulBuffSize);
			if(pHLS->pSegBuff == NULL)
			{
				PrintError("%s(%d) : Error> Segment Buffer Allocation Fail\n",__FUNCTION__,__LINE__);
				goto _openctrl_error;
			}
			else
				break;
		}
	}
	PrintDebug("%s(%d) : Debug> Segment Buffer : %u\n",__FUNCTION__,__LINE__,pHLS->ulBuffSize );
	pHLS->pSegBuff = DD_MEM_Alloc(pHLS->ulBuffSize);
	if(pHLS->pSegBuff == NULL)
	{
			PrintError("%s(%d) : Error> Segment Buffer Allocation Fail\n",__FUNCTION__,__LINE__);
			if(HLS_CONTROLLER_DiscardCurHighestStream(pHLS->hCtrl) == FALSE)
			{
				PrintError("%s(%d) : Error> Segment Buffer Allocation Fail\n",__FUNCTION__,__LINE__);
				goto _openctrl_error;
			}
		}
	}
	if(pHLS->pSegBuff == NULL)
	{
		PrintError("%s(%d) : Error> Segment Buffer Allocation Fail\n",__FUNCTION__,__LINE__);
		goto _openctrl_error;
	}
#if 0
	pHLS->pDecBuff = DD_MEM_Alloc(pHLS->ulBuffSize);
	if(pHLS->pDecBuff == NULL)
	{
		PrintError("%s(%d) : Error> Decryption Buffer Allocation Fail. Change To Default BLOCK SIZE\n",__FUNCTION__,__LINE__);
		pHLS->pDecBuff = DD_MEM_Alloc(HLS_CONST_DEFAULT_DMABLOCK);
		if(pHLS->pDecBuff == NULL)
		{
			PrintError("%s(%d) : Error> Decryption Buffer Allocation Fail\n",__FUNCTION__,__LINE__);
			goto _openctrl_error;
		}
		else
		{
			pHLS->ulDecBuffSize = HLS_CONST_DEFAULT_DMABLOCK;
		}
	}
	else
	{
		pHLS->ulDecBuffSize = pHLS->ulBuffSize;
	}
#endif
/*
	HLS_CONTROLLER_GearHighest(pHLS->hCtrl);
	pHLS->pInitSegUrl = HLS_CONTROLLER_GetCurSegmentUrl(pHLS->hCtrl);
	pHLS->pInitSegKeyUrl = HLS_CONTROLLER_GetCurKeyUrl(pHLS->hCtrl);
*/
	HLS_CONTROLLER_GearLowest(pHLS->hCtrl);
	g_hHLS = pHLS;
	unBufferSize = HLS_CONTROLLER_GetCurMaxBitrate(pHLS->hCtrl);
	if(unBufferSize <= 0)
	{
		unBufferSize = HLS_CONFIG_MAX_BUFFER;
	}
	else
	{
		// unBufferSize = (unBufferSize / 8) * HLS_CONFIG_BUFFERING_SECS;
		unBufferSize = (unBufferSize / 8) * (HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl) * HLS_CONFIG_BUFFERING_SEGMENTS);
	}
	if(unBufferSize > HLS_CONFIG_MAX_BUFFER )
	{
		unBufferSize = HLS_CONFIG_MAX_BUFFER;
	}
	else if( unBufferSize < HLS_CONFIG_MIN_BUFFER)
	{
		unBufferSize = HLS_CONFIG_MIN_BUFFER;
	}
	nRet = HLS_CONTROLLER_OpenBuffer(pHLS->hCtrl,unBufferSize, HLS_CONST_MEMORY_BUFFER);
	if (nRet != DI_ERR_OK)
	{
		(void) SSTR_BUFFERMGR_Close(HLS_CONTROLLER_GetBufferMgr(pHLS->hCtrl));
		PrintError ("%s(%d) Fail! CreateBuffer was failed. \n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		goto _openctrl_error;
	}
	(void)HLS_CONTROLLER_RegisterRequestUpdateCallback(pHLS->hCtrl, pHLS, P_STREAM_HLS_RequestUpdate);
	if(pM3U8Buff != NULL)
	{
		DD_MEM_Free(pM3U8Buff);
		pM3U8Buff = NULL;
	}
	/**
	 * Heuristics Setting
	**/
	if(DI_STREAM_HLS_SetSettings(pHLS,NULL) != HLS_OK)
	{
		PrintError ("%s(%d) Fail! Default Setting(Heuristics Initialize)\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		goto _openctrl_error;
	}
	if(pHostUrl != NULL)
	{
		DD_MEM_Free(pHostUrl);
		pHostUrl = NULL;
	}
	#if 0
	if(P_STREAM_HLS_SetHeuristics(pHLS))
	{
		PrintError ("%s(%d) Fail! Set Heuristics was failed. \n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		goto _openctrl_error;
	}
	#endif
//	HLS_M3U8_Destroy(m3u8);
//	m3u8 = NULL;
	return DI_ERR_OK;

_openctrl_error:
	if(pHostUrl != NULL)
	{
		DD_MEM_Free(pHostUrl);
		pHostUrl = NULL;
	}
	if(pM3U8Buff != NULL)
	{
		DD_MEM_Free(pM3U8Buff);
		pM3U8Buff = NULL;
	}
	if(m3u8 != NULL)
	{
		playlistNode = UTIL_DLLIST_GetTailNode(&m3u8->playlistSet);
		while(playlistNode != NULL)
		{
			pPlaylist= (m3u8Playlist_t*)playlistNode->pData;
			if(pPlaylist == NULL)
	        {
					PrintError("%s(%d) Playlist Data is not exist\n",__FUNCTION__,__LINE__);
					PrintExit;
					return M3U8_ERR_FAIL;
	        }
			if(pPlaylist->pUrl != NULL)
			{
				DD_MEM_Free(pPlaylist->pUrl);
				pPlaylist->pUrl = NULL;
			}
			if(pPlaylist->pBaseUrl != NULL)
			{
				DD_MEM_Free(pPlaylist->pBaseUrl);
				pPlaylist->pBaseUrl = NULL;
			}
			if(pPlaylist != NULL)
			{
				DD_MEM_Free(pPlaylist);
				playlistNode->pData = NULL;
				pPlaylist = NULL;
			}
			playlistNode = UTIL_DLLIST_PrevNode(playlistNode);
		}
	}
	if(pHLS != NULL)
	{
		if(pHLS->pSegBuff != NULL)
		{
			DD_MEM_Free(pHLS->pSegBuff);
			pHLS->pSegBuff = NULL;
		}
		if(pHLS->hCtrl != NULL)
		{
			HLS_CONTROLLER_Close(pHLS->hCtrl);
			pHLS->hCtrl = NULL;
		}
	}
	if(m3u8 != NULL)
	{
		UTIL_DLLIST_Reset(&m3u8->playlistSet);
		HLS_M3U8_Destroy(m3u8);
	}
	if(pHLS != NULL)
	{
		DD_MEM_Free(pHLS);
		pHLS = NULL;
	}
	g_hHLS = NULL;
	return DI_ERR_ERROR;
}

int DI_STREAM_HLS_CloseController()
{
	m3u8_t* m3u8 = NULL;
	HINT32 nRet = DI_ERR_OK;
	HLS_t* pHLS = NULL;
	PrintEnter;
	if( g_hHLS == NULL )
	{
		PrintError("%s(%d) : Error> HLS Handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pHLS = __getHLS(g_hHLS);
	if( pHLS == NULL)
	{
		PrintError("%s(%d) : Error> HLS Handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	if( pHLS->hCtrl == NULL )
	{
		PrintError("%s(%d) : Error> HLS Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
return DI_ERR_ERROR;
	}
	m3u8 = HLS_CONTROLLER_GetM3U8Handle(pHLS->hCtrl);
	if(m3u8 == NULL)
	{
		PrintError("%s(%d) : Error> M3U8 Handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
return DI_ERR_ERROR;
	}
	nRet = HLS_M3U8_Destroy(m3u8);
	if(nRet != DI_ERR_OK)
	{
		PrintError("%s(%d) : Error> M3U8 Destroy Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
return DI_ERR_ERROR;
	}
	HLS_CONTROLLER_CloseBuffer(pHLS->hCtrl);
	nRet = HLS_CONTROLLER_Close(pHLS->hCtrl);
	if(nRet != DI_ERR_OK)
	{
		PrintError("%s(%d) : Error> HLS Close Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	(void)VK_SEM_Destroy(pHLS->ulSema);
	(void)VK_SEM_Destroy(pHLS->ulLiveSema);
	(void)VK_SEM_Destroy(pHLS->ulDownloadingSema);
	pHLS->ulBuffSize = 0;
	if(pHLS->pSegBuff != NULL)
	{
		DD_MEM_Free(pHLS->pSegBuff);
		pHLS->pSegBuff = NULL;
	}
	if(pHLS != NULL)
	{
		DD_MEM_Free(pHLS);
		pHLS = NULL;
	}
	g_hHLS = NULL;
	PrintExit;
	return DI_ERR_OK;
}

int DI_STREAM_HLS_CreateLiveUpdateTask ()
{
	HLS_t* pHLS;
	VK_ERR_CODE nVKRet = VK_OK;
	PrintEnter;
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return -1;
	}
	pHLS = __getHLS(g_hHLS);
	if(pHLS->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> HLS CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return -1;
	}
	if(pHLS->bLiveUpdateTaskInit == TRUE)
	{
		return 0;
	}
/*
	if(HLS_CONTROLLER_IsLive (pHLS->hCtrl) != TRUE)
	{
		return -1;
	}
*/
	nVKRet = VK_TASK_Create ( P_STREAM_HLS_LiveUpdate_Task
									, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
									, TUNER_STREAM_STACK_SIZE
									, (char*)"HLS_LiveUpdateTask"
									, NULL
									, &pHLS->ulLiveUpdateTaskId
									, 0);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> STREAM Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		return -1;
	}
	pHLS->bLiveUpdateTaskInit = TRUE;
	VK_TASK_Start(pHLS->ulLiveUpdateTaskId);
	PrintExit;
	return 0;
}

int DI_STREAM_HLS_DestroyLiveUpdateTask ()
{
	HLS_t* pHLS;
	PrintEnter;
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return -1;
	}
	pHLS = __getHLS(g_hHLS);
	if(pHLS->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> HLS CONTROLLER is NULL. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return -1;
	}
	/* 만일 Live Update Task가 holding 상태 이면 풀어 줌. */
	pHLS->bIsPrebuffering = TRUE;
/*
	if(SSTR_CONTROLLER_IsLive (pSstr->hCtrl) != TRUE)
	{
		return -1;
	}
*/
	while(pHLS->eLiveUpdateStatus != DI_IPTUNER_HLS_LIVE_UPDATE_TERMINATED)
	{
		VK_TASK_Sleep(100);
	}
	(void) VK_TASK_Destroy (pHLS->ulLiveUpdateTaskId);
	pHLS->bLiveUpdateTaskInit = FALSE;
	PrintExit;
	return 0;
}


HLS_ERROR_e DI_STREAM_HLS_SetSettings(HLS_HANDLE hHls, DI_IPTUNER_AdaptiveSettings_t *pSettings)
{
	HLS_t* pHLS = NULL;
	SSTR_HEURISTICS_Config_t tHeurConfig;
	SSTR_HEURISTICS_t *pHeuristics = NULL;
	SSTR_BUFFERMGR_t *pBufferMgr = NULL;
	HUINT32 unProgramNum = 0;
	HUINT64 ullSegment = 0;
	HUINT64 ullSequenceNum = 0;
	DI_IPTUNER_ADAPTIVE_CONTROL_e eControl = DI_IPTUNER_ADAPTIVE_NONE;
	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHls);
	pBufferMgr = HLS_CONTROLLER_GetBufferMgr(pHLS->hCtrl);
	if(pBufferMgr == NULL)
	{
		PrintError ("%s(%d) Fail! GetBufferMgr was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHeuristics = HLS_CONTROLLER_GetHeuristics(pHLS->hCtrl);
	if(pHeuristics == NULL)
	{
		PrintError ("%s(%d) Fail! GetHeuristicsHandle was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pSettings != NULL)
	{
		eControl = pSettings->eControl;
	}
	else
	{
		PrintDebug ("%s(%d) Warning> Adaptive Setting is NULL. Change to Initialize\n",__FUNCTION__,__LINE__);
		eControl = DI_IPTUNER_ADAPTIVE_INIT;
	}
	switch(eControl)
	{
		case DI_IPTUNER_ADAPTIVE_NONE :
			break;
		case DI_IPTUNER_ADAPTIVE_INIT :
			if(pHeuristics->bInit != TRUE)
			{
				if(SSTR_HEURISTICS_GetDefaultConfig(pHeuristics, SSTR_HEUR_CONFIG_PRESET_HLS, &tHeurConfig))
				{
					PrintError ("%s(%d) Fail! GetDefaultConfig was failed. \n",__FUNCTION__,__LINE__);
					PrintExit;
					return HLS_FAIL;
				}
				tHeurConfig.hCtrl = pHLS->hCtrl;
				tHeurConfig.hBufferMgr = pBufferMgr;
				tHeurConfig.cbCurBitrate = HLS_CONTROLLER_GetCurBitrate;
				tHeurConfig.cbCurDuration = HLS_CONTROLLER_GetCurSegmentDuration;
				tHeurConfig.cbIsProtected = HLS_CONTROLLER_IsCurStreamEncrypted;
				tHeurConfig.cbGearUp = HLS_CONTROLLER_GearUp;
				tHeurConfig.cbGearDown = HLS_CONTROLLER_GearDown;
				tHeurConfig.cbGearSet = HLS_CONTROLLER_SetClosestBandwidth;
				tHeurConfig.cbGearLowest = HLS_CONTROLLER_GearLowest;
				tHeurConfig.cbGearHighest = HLS_CONTROLLER_GearHighest;
				tHeurConfig.eStartup = DI_IPTUNER_ADAPTIVE_STARTUP_LOWEST;
				tHeurConfig.eDecision= DI_IPTUNER_ADAPTIVE_DECISION_AUTO;

				// HLS_CONTROLLER_Setup(pHLS->hCtrl,pSettings->ulProgramIndex,pSettings->ulStreamIndex,0);

				if(SSTR_HEURISTICS_Init(pHeuristics, &tHeurConfig))
				{
					PrintError ("%s(%d) Fail! Heuristics Init was failed. \n",__FUNCTION__,__LINE__);
					PrintExit;
					return HLS_FAIL;
				}
			}
			break;
		case DI_IPTUNER_ADAPTIVE_HLS_ALL :
			SSTR_HEURISTICS_GetConfig(pHeuristics, &tHeurConfig);
			tHeurConfig.eStartup = pSettings->eStartup;
			tHeurConfig.eDecision= pSettings->eDecision;
			if(HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &ullSegment))
			{
				PrintError ("%s(%d) Fail! HLS_CONTROLLER_GetCurSegmentId was failed. \n",__FUNCTION__,__LINE__);
				PrintExit;
				return HLS_FAIL;
			}
			ullSequenceNum = HLS_CONTROLLER_GetCurStreamSequenceNum(pHLS->hCtrl);
			HLS_CONTROLLER_Setup(pHLS->hCtrl,pSettings->ulProgramIndex,pSettings->ulStreamIndex,ullSegment - ullSequenceNum);
			if(SSTR_HEURISTICS_SetConfig(pHeuristics, &tHeurConfig))
			{
				PrintError ("%s(%d) Fail! Heuristics Init was failed. \n",__FUNCTION__,__LINE__);
				PrintExit;
				return HLS_FAIL;
			}
			break;
		case DI_IPTUNER_ADAPTIVE_HLS_PROGRAM:
			HLS_CONTROLLER_Setup(pHLS->hCtrl,pSettings->ulProgramIndex,0,0);
			break;
		case DI_IPTUNER_ADAPTIVE_HLS_STREAM:
			if(HLS_CONTROLLER_GetCurProgramIndex(pHLS->hCtrl, &unProgramNum))
			{
				PrintError ("%s(%d) Fail! HLS_CONTROLLER_GetCurSegmentId was failed. \n",__FUNCTION__,__LINE__);
				PrintExit;
				return HLS_FAIL;
			}
			if(HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &ullSegment))
			{
				PrintError ("%s(%d) Fail! HLS_CONTROLLER_GetCurSegmentId was failed. \n",__FUNCTION__,__LINE__);
				PrintExit;
				return HLS_FAIL;
			}
			ullSequenceNum = HLS_CONTROLLER_GetCurStreamSequenceNum(pHLS->hCtrl);
			HLS_CONTROLLER_Setup(pHLS->hCtrl,unProgramNum,pSettings->ulStreamIndex,ullSegment - ullSequenceNum);
			break;
		default :
			PrintError ("%s(%d) UNKNOWN ADAPTIVE CONTROL (%d) \n",__FUNCTION__,__LINE__,pSettings->eControl);
			PrintExit;
			return HLS_FAIL;

	}
	PrintExit;
	return HLS_OK;
}

HLS_ERROR_e DI_STREAM_HLS_GetSettings(HLS_HANDLE hHls, DI_IPTUNER_AdaptiveSettings_t *pSettings)
{
	HLS_t* pHLS = NULL;
	SSTR_HEURISTICS_Config_t tHeurConfig;
	SSTR_HEURISTICS_t *pHeuristics = NULL;
	HUINT32 unProgramNum = 0;
	HUINT32 unStreamNum = 0;
	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> SSTR HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHls);
	pHeuristics = HLS_CONTROLLER_GetHeuristics(pHLS->hCtrl);
	if(pHeuristics == NULL)
	{
		PrintError ("%s(%d) Fail! GetHeuristicsHandle was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
#if 1
	SSTR_HEURISTICS_GetConfig(pHeuristics, &tHeurConfig);
	pSettings->eStartup = tHeurConfig.eStartup;
	pSettings->eDecision = tHeurConfig.eDecision;
	if(HLS_CONTROLLER_GetCurProgramIndex(pHLS->hCtrl, &unProgramNum))
	{
		PrintError ("%s(%d) Fail! HLS_CONTROLLER_GetCurProgramIndex was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(HLS_CONTROLLER_GetCurStreamIndex(pHLS->hCtrl, &unStreamNum))
	{
		PrintError ("%s(%d) Fail! HLS_CONTROLLER_GetCurStreamIndex was failed. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pSettings->ulProgramIndex = unProgramNum;
	pSettings->ulStreamIndex = unStreamNum;
#else
	switch(pSettings->eControl)
	{
		case DI_IPTUNER_ADAPTIVE_NONE :
			if(SSTR_HEURISTICS_GetDefaultConfig(pHeuristics, SSTR_HEUR_CONFIG_PRESET_SSTR, &tHeurConfig))
			{
				PrintError ("%s(%d) Fail! GetDefaultConfig was failed. \n",__FUNCTION__,__LINE__);
				PrintExit;
				return SSTR_ERR_FAIL;
			}
			pSettings->eStartup = tHeurConfig.eStartup;
			pSettings->eDecision = tHeurConfig.eDecision;
			pSettings->ulVideoStreamIndex = SSTR_CONTROLLER_GetCurAVStreamIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO);
			pSettings->ulAudioStreamIndex = SSTR_CONTROLLER_GetCurAVStreamIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO);
			pSettings->ulVideoTrackIndex = SSTR_CONTROLLER_GetCurAVTrackIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO);
			pSettings->ulAudioTrackIndex = SSTR_CONTROLLER_GetCurAVTrackIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO);
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_ALL :
			SSTR_HEURISTICS_GetConfig(pHeuristics, &tHeurConfig);
			pSettings->eStartup = tHeurConfig.eStartup;
			pSettings->eDecision = tHeurConfig.eDecision;
			pSettings->ulVideoStreamIndex = SSTR_CONTROLLER_GetCurAVStreamIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO);
			pSettings->ulAudioStreamIndex = SSTR_CONTROLLER_GetCurAVStreamIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO);
			pSettings->ulVideoTrackIndex = SSTR_CONTROLLER_GetCurAVTrackIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO);
			pSettings->ulAudioTrackIndex = SSTR_CONTROLLER_GetCurAVTrackIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO);
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_VIDEO_STREAM :
			pSettings->ulVideoStreamIndex = SSTR_CONTROLLER_GetCurAVStreamIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO);
		case DI_IPTUNER_ADAPTIVE_SSTR_AUDIO_STREAM :
			pSettings->ulAudioStreamIndex = SSTR_CONTROLLER_GetCurAVStreamIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO);
		case DI_IPTUNER_ADAPTIVE_SSTR_VIDEO_TRACK :
			pSettings->ulVideoTrackIndex = SSTR_CONTROLLER_GetCurAVTrackIndex(pSstr->hCtrl , SSTR_TYPE_VIDEO);
		case DI_IPTUNER_ADAPTIVE_SSTR_AUDIO_TRACK :
			pSettings->ulAudioTrackIndex = SSTR_CONTROLLER_GetCurAVTrackIndex(pSstr->hCtrl , SSTR_TYPE_AUDIO);
			break;
		case DI_IPTUNER_ADAPTIVE_SSTR_TEXT_STREAM :
		case DI_IPTUNER_ADAPTIVE_SSTR_TEXT_TRACK :
		default :
			PrintError ("%s(%d) UNKNOWN ADAPTIVE CONTROL (%d) \n",__FUNCTION__,__LINE__,pSettings->eControl);
			PrintExit;
			return SSTR_ERR_FAIL;

	}
#endif
	PrintExit;
	return HLS_OK;
}

HBOOL DI_STREAM_HLS_isChangedTs()
{
#if 0
	if(g_isChangedTs)
	{
		g_isChangedTs = FALSE;
		return TRUE;
	}
#endif
	return FALSE;
}

HLS_ERROR_e	DI_STREAM_HLS_SetDecoderInfo(HUINT32 uDeviceId,  DI_VIDEO_FORMAT eVideoFormat, DI_AUDIO_FORMAT eAudioFormat)
{
	g_uDeviceId = uDeviceId;
	g_eVideoFormat = eVideoFormat;
	g_eAudioFormat = eAudioFormat;

//	PrintDebug("Set Decoder Info :DeviceID(%d), V(%x), A(%x)", g_uDeviceId, g_eVideoFormat, g_eAudioFormat);
	return HLS_OK;
}

HBOOL DI_STREAM_HLS_ProcessDiscontinuity(HINT32 nDuration, SSTR_BUFFERMGR_t* pBufferMgr)
{
	int nRet = HLS_OK;
	HINT32 nRetryCount = nDuration * 100;
	HUINT32 uVideoBufferDepth = 0;
	HUINT32 uAudioBufferDepth = 0;
	HUINT32	uPumpBufferDepth = 0;
	HUINT32 uBufferDepth = 0;
	HUINT32 uPrvBufferDepth = 0;
	HINT32 nBreakCount = 0;

	HUINT8 pBuf[1024];
	HINT32 nWriteSize  = 0;
	HINT32 nCount = 512;

	// input dummy data to playpump (size : 1024 * 1000  = 1,024,000)
	VK_memset(pBuf, 0x00, 1024);
	while(nCount-- > 0)
	{
		nWriteSize = SSTR_BUFFERMGR_Write(pBufferMgr,pBuf, 1024);
		if(nWriteSize == 0)
		{
			VK_TASK_Sleep(HLS_CONST_WAITING);
			continue;
		}
	}

	// wait for decoder buffer empty status
	do
	{
		nRet = DI_MEDIA_GetVideoBufferDepth(&uVideoBufferDepth);
		if(nRet != DI_ERR_OK)
		{
			PrintError("[%s:%d] Getting VideoBufferDepth is Error(=%d)\n", __func__, __LINE__, nRet);
			return HLS_FAIL;
		}

		nRet = DI_MEDIA_GetAudioBufferDepth(&uAudioBufferDepth);
		if(nRet != DI_ERR_OK)
		{
			PrintError("[%s:%d] Getting unAudioBufferDepth is Error(=%d)\n", __func__, __LINE__, nRet);
			return HLS_FAIL;
		}

	 	nRet = DI_MEDIA_GetPumpBufferDepth(&uPumpBufferDepth);
		if(nRet != DI_ERR_OK)
		{
			PrintError("[%s:%d] Getting uPumpBufferDepth is Error(=%d)\n", __func__, __LINE__, nRet);
			return HLS_FAIL;
		}
		uBufferDepth = uVideoBufferDepth + uAudioBufferDepth;
		PrintDebug("\t\033[01m\033[33m Wait for decoder buffer empty..V(%d) A(%d) P(%d)\033[0m\n", uVideoBufferDepth, uAudioBufferDepth, uPumpBufferDepth);

		if(uBufferDepth == uPrvBufferDepth)
			nBreakCount++;
		else
		{
			uPrvBufferDepth = uBufferDepth;
			nBreakCount = 0;
		}

		if(nBreakCount > 2)
		{
			PrintError("[%s:%d] Decoder buffer empty(=%d)\n", __func__, __LINE__, nBreakCount);
			break;
		}
		if(uPumpBufferDepth > 10)
			nRetryCount++;

		VK_TASK_Sleep(HLS_CONST_WAITING);
	}while(nRetryCount-- > 0);


	// video/audio restart
	PrintDebug("\n\n\t\033[01m\033[32m [%s:%d] Stop decoder for DISCONTINUITY tag\033[0m\n\n", __FUNCTION__, __LINE__);

	DI_VIDEO_Show(g_uDeviceId, FALSE);
	DI_AUDIO_Stop(g_uDeviceId);
	DI_VIDEO_Stop(g_uDeviceId);
#if defined(CONFIG_SES_HDPLUS_SMARTTV)
	DI_HDMI_EnableOutput(TRUE);
#endif
	VK_TASK_Sleep(10);

	PrintDebug("\n\n\t\033[01m\033[32m [%s:%d] Sdtart decoder for DISCONTINUITY tag\033[0m\n\n", __FUNCTION__, __LINE__);
	DI_AUDIO_Start(g_uDeviceId,  g_eAudioFormat);
	DI_VIDEO_Start(g_uDeviceId, g_eVideoFormat);
	DI_VIDEO_Show(g_uDeviceId,TRUE);

	return HLS_OK;
}

#ifdef USE_HLS_SEEK_IMPROVEMENT
int DI_STREAM_HLS_PreBuffering (DI_IPTUNER_Index_e nTunerIndex)
{
	HUINT32 unRetry=0;
	off_t offsetForSeek = 0;
	HBOOL bWaitingForUpdate = FALSE;
	HUINT32 unProgramIdxForUpdate = 0;
	HUINT32 unStreamIdxForUpdate = 0;
	HUINT32 unDelayForUpdate = 0;
	HUINT64 ullRequestedSeekPoint;
	HUINT32 ulTimeForSeek = 0;
	HUINT32 unStartTick = 0;
	HUINT32 unEndTick = 0;
	HUINT32 uSegmentDuration = 0;

	SSTR_BUFFERMGR_t* pBufferMgr = NULL;
	SSTR_HEURISTICS_t* pHeuristics = NULL;
	HLS_t* pHLS = NULL;

	HUINT8 *pSrcBlock = NULL;
	void *pDstBlock = NULL;
	HINT32 nBlockLen = 0;

	NEXUS_Error nexusError;

	PrintEnter;
	if(nTunerIndex != DI_IPTUNER_STREAM_HLS)
	{
		PrintError("%s(%d) : Error> invalid Tuner Type(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
		return DI_ERR_ERROR;
	}
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pHLS = __getHLS(g_hHLS);
	pBufferMgr = HLS_CONTROLLER_GetBufferMgr(pHLS->hCtrl);
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> pBufferMgr is NULL\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pHeuristics = HLS_CONTROLLER_GetHeuristics(pHLS->hCtrl);
	if(pHeuristics == NULL)
	{
		PrintError("%s(%d) Error> pHeuristics is NULL\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	nBlockLen = HLS_CONST_DEFAULT_DMABLOCK;
	nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pSrcBlock);
	if(nexusError != NEXUS_SUCCESS)
	{
		if(nBlockLen > 0)
		{
			nBlockLen = nBlockLen / 2;
		}
		else
		{
			PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
			return HLS_FAIL;
		}
		nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pSrcBlock);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n", __FUNCTION__,__LINE__,nexusError);
			return 0;
		}
	}
	nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pDstBlock);
	if(nexusError != NEXUS_SUCCESS)
	{
		if(pSrcBlock != NULL)
		{
			NEXUS_Memory_Free(pSrcBlock);
			if(nBlockLen > 0)
			{
				nBlockLen = nBlockLen / 2;
			}
			else
			{
				PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
				return 0;
			}
			nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pSrcBlock);
			if(nexusError != NEXUS_SUCCESS)
			{
				PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
				return 0;
			}
			nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pDstBlock);
			if(nexusError != NEXUS_SUCCESS)
			{
				if(pSrcBlock != NULL)
				{
					NEXUS_Memory_Free(pSrcBlock);
				}
				PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
				return 0;
			}
		}
	}

	if(g_ePreBufferingTaskState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_READY)
	{
		g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED;
	}
//	g_ePreBufferingTaskChangeState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED;
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	// start live update task
	pHLS->bIsPrebuffering = TRUE;
#ifdef CONFIG_HLS_LIVE_RESTART
	// Initialize LiveRestart
	pHLS->bOnLiveRestart = FALSE;
	if (HLS_CONTROLLER_IsCurStreamEndList(pHLS->hCtrl) == FALSE)
	{
		HLS_CONTROLLER_Setup(pHLS->hCtrl, 0, pHLS->unProbingStreamIndex, 0); // probing stream에서 재생하도록 ...
		HLS_CONTROLLER_SetSegmentToLive(pHLS->hCtrl);
		(void)SSTR_BUFFERMGR_Clear(pBufferMgr);
	}
#endif
	do
	{
		if(pBufferMgr == NULL)
		{
			PrintError("pBufferMgr is NULL\n");
			goto exit_stop;
		}
		if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
			goto exit_stop;
		}
		if(HLS_CONTROLLER_IsCurStreamEndList(pHLS->hCtrl) == FALSE)
		{
			if(pHLS->eLiveUpdateStatus == DI_IPTUNER_HLS_LIVE_UPDATE_STARTED)
			{
				if( HLS_CONTROLLER_GetCurProgramIndex(pHLS->hCtrl, &unProgramIdxForUpdate) == HLS_OK
				 && HLS_CONTROLLER_GetCurStreamIndex(pHLS->hCtrl, &unStreamIdxForUpdate) == HLS_OK
				 && HLS_CONTROLLER_GetCurUpdateDelay(pHLS->hCtrl, &unDelayForUpdate) == HLS_OK )
				{
					P_STREAM_HLS_RequestUpdate(pHLS, unProgramIdxForUpdate, unStreamIdxForUpdate, (unDelayForUpdate * HLS_CONST_WAITING * 10) / 2, FALSE);
				}
			}
		}
		if(g_ePreBufferingTaskState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING
		&& bWaitingForUpdate == TRUE)
		{
			if(HLS_CONTROLLER_NextSegment(pHLS->hCtrl) == HLS_OK)
			{
				bWaitingForUpdate = FALSE;
				g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED;
			}
		}

		/***********************************************************************
		 *  1. Seek 요청에 대한 처리
		 *     Seek Time이 포함된 Segment를 찾아서 Current Segment로 세팅
		 *     Segment 내에서의 Seek Position(offset) 을 Bitrate 를 이용하여 측정
		 ***********************************************************************/
		if(P_STREAM_HLS_IsSeekRequested(pHLS))
		{
			seek_operation:
			PrintDebug("%s(%d) P_STREAM_HLS_IsSeekRequested\n",__FUNCTION__,__LINE__);
			VK_SEM_Get(pHLS->ulDownloadingSema);
			ullRequestedSeekPoint = P_STREAM_HLS_GetTimeForSeek(pHLS);
			if(P_STREAM_HLS_IsValidTimeForSeek(pHLS,ullRequestedSeekPoint))
			{
#ifdef CONFIG_HLS_LIVE_RESTART
				P_STREAM_HLS_CheckLiveRestart(pHLS,ullRequestedSeekPoint);
#endif
				ulTimeForSeek = ullRequestedSeekPoint;
				HLS_CONTROLLER_SetSegmentByTime(pHLS->hCtrl, ulTimeForSeek);
				ulTimeForSeek = (ulTimeForSeek - HLS_CONTROLLER_GetCurSegmentStartTime(pHLS->hCtrl)); // during seek improvement porting
//			reset_buffer:
				SSTR_HEURISTICS_SetHeuristicsState(pHeuristics,SSTR_HEUR_STATE_TIMID);
				(void)SSTR_BUFFERMGR_Clear(pBufferMgr);
				SSTR_HEURISTICS_Run(pHeuristics,0,0);
				HLS_CONTROLLER_GearDown(pHLS->hCtrl);
				g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED;
				bWaitingForUpdate = FALSE;
				if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
					VK_SEM_Release(pHLS->ulDownloadingSema);
					goto exit_stop;
				}
			}
			P_STREAM_HLS_SetRequestSeek(pHLS,FALSE);
			SSTR_BUFFERMGR_SetReadLock(pBufferMgr, FALSE);
			VK_SEM_Release(pHLS->ulDownloadingSema);
		}
		if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
			goto exit_stop;
		}
		if(g_ePreBufferingTaskState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING)
		{
			VK_TASK_Sleep(HLS_CONST_WAITING);
			continue;
		}

		if((SSTR_BUFFERMGR_GetBufferSize(pBufferMgr) * HLS_CONST_HIGHEST_BUFFERING_THRESHOLD)
		> SSTR_BUFFERMGR_GetReadableSize(pBufferMgr) /*&& SSTR_BUFFERMGR_GetWritableSize(pBufferMgr) != 0*/)
		{
			HLS_ERROR_e eHLSError = HLS_OK;
			/***********************************************************************
			 *  2. Download Current Segment
			 *     Segment Buffer에 Current Segment를 Download 받는다.
			 *     ( Bufferfullness 90% 이하인 경우 )
			 ***********************************************************************/
			VK_SEM_Get(pHLS->ulDownloadingSema);
			eHLSError = P_HLS_DownloadWithPlay( offsetForSeek, ulTimeForSeek, pDstBlock, pSrcBlock, nBlockLen);

			offsetForSeek = 0;
			ulTimeForSeek = 0;

			if(eHLSError != HLS_OK)
			{
				if(eHLSError == HLS_PANIC_DETECTED)
				{
					VK_SEM_Release(pHLS->ulDownloadingSema);
					continue;
				}
				if( eHLSError == HLS_SEEK_REQUESTE )
				{
//					unEndFuncTick = SYS_TIMER_GetSystemTick();
					//PrintError("%s(%d) Function Complete Time : [ %lu ]\n",__FUNCTION__,__LINE__, unEndFuncTick - unStartFuncTick);
//					unStartFuncTick = SYS_TIMER_GetSystemTick();
					VK_SEM_Release(pHLS->ulDownloadingSema);
					continue;
				}
				if( eHLSError == HLS_TIMEOUT )
				{
					VK_SEM_Release(pHLS->ulDownloadingSema);
					SSTR_HEURISTICS_Reset(pHeuristics);
					SSTR_HEURISTICS_SetHeuristicsState(pHeuristics, SSTR_HEUR_STATE_PANIC);
					HLS_CONTROLLER_GearLowest(pHLS->hCtrl); /* set lowest bitrate */
					continue;
				}
				if(P_STREAM_HLS_IsSeekRequested(pHLS))
				{
					VK_SEM_Release(pHLS->ulDownloadingSema);
					goto seek_operation;
				}
				if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
					VK_SEM_Release(pHLS->ulDownloadingSema);
					goto exit_stop;
				}
				unRetry++;
				if(unRetry > HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
				{
					pHLS->bNeedLiveRecovery = TRUE;
				}
				PrintError ("%s(%d) Download Segment Fail Retry...%u !!! \n",__FUNCTION__,__LINE__,unRetry);
				VK_SEM_Release(pHLS->ulDownloadingSema);
				VK_TASK_Sleep(HLS_CONST_WAITING);
				continue;
			}
			else
			{
				pHLS->bNeedLiveRecovery = FALSE;
				unRetry = 0;
			}

			VK_SEM_Release(pHLS->ulDownloadingSema);

			if(HLS_CONTROLLER_IsCurSegmentDiscontinuity(pHLS->hCtrl) == TRUE)
			{
				uSegmentDuration = HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl);
				DI_STREAM_HLS_ProcessDiscontinuity(uSegmentDuration, pBufferMgr);
			}

			unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		}
		else
		{
			VK_TASK_Sleep(HLS_CONST_WAITING);
//			PrintDebug ("%s(%d) BUFFER IS ALMOST FULL( %d percent over )\n",__FUNCTION__,__LINE__,SSTR_CONST_HIGHEST_BUFFERING_THRESHOLD * 100);
			continue;
		}
#if 0
		if(stAdaptiveCallback.cbAVStreamCallback != NULL)
		{
			stAdaptiveCallback.cbAVStreamCallback(nTunerIndex,SSTR_CONTROLLER_GetCurStreamType(hSstr), SSTR_CONTROLLER_GetCurrentBitrate(hSstr),SSTR_CONTROLLER_GetCurrentFragmentId(hSstr));
		}
#endif
#if defined(CONFIG_SES_HDPLUS_SMARTTV)
		/***********************************************************************
		 * This sequence are only used to RTLNow app with SMARTTV.
		 * 4-1. Check the current segment whether it is the first changed segment of advertisement.
		 *	   So, if it is the first changed segment of advertisement,
		 *	   the isChangeAD flag would be changed from FALSE to TRUE.
		 ***********************************************************************/
		if(HLS_CONTROLLER_IsCurSegmentChangedTs(pHLS->hCtrl) == TRUE)
		{
			g_isChangedTs = TRUE;
		}
#endif
		/***********************************************************************
		 *	5. Setting Next Segment
		 *	   다음 Segment를 Current Segment로 지정한다.
		 *	   Next Segment가 없을 경우 EOS 처리 한다.
		 ***********************************************************************/
		 VK_SEM_Get(pHLS->ulDownloadingSema);
		if(HLS_CONTROLLER_NextSegment(pHLS->hCtrl) != HLS_OK)
		{
			g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING;
			if(HLS_CONTROLLER_IsCurStreamEndList(pHLS->hCtrl) == FALSE)
			{
				bWaitingForUpdate = TRUE;
				if( HLS_CONTROLLER_GetCurProgramIndex(pHLS->hCtrl, &unProgramIdxForUpdate) == HLS_OK
					 && HLS_CONTROLLER_GetCurStreamIndex(pHLS->hCtrl, &unStreamIdxForUpdate) == HLS_OK
					 && HLS_CONTROLLER_GetCurUpdateDelay(pHLS->hCtrl, &unDelayForUpdate) == HLS_OK )
				{
					P_STREAM_HLS_RequestUpdate(pHLS, unProgramIdxForUpdate, unStreamIdxForUpdate, 0, TRUE);
				}
			}
			VK_SEM_Release(pHLS->ulDownloadingSema);
			VK_TASK_Sleep(HLS_CONST_WAITING);
			continue;
		}
		VK_SEM_Release(pHLS->ulDownloadingSema);

		if(HLS_CONTROLLER_IsCurSegmentDiscontinuity(pHLS->hCtrl) == TRUE)
		{
#if 0	// is not ready to process the DISCONTINUITY tag
			g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING;
			// TODO: Send Event to MW/PAL with HLS_CONTROLLER_GetCurSegmentDateTime(pHLS->hCtrl);
			VK_TASK_Sleep(HLS_CONST_WAITING);
			continue;
#endif
		}
	}while (g_ePreBufferingTaskChangeState != DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING); /* end while */

	//VK_SEM_Release(s_ulPreBufferingLock); 	//--------------------------------	Critical Section
exit_stop:
	if(pSrcBlock != NULL)
	{
		NEXUS_Memory_Free(pSrcBlock);
		pSrcBlock = NULL;
	}
	if(pDstBlock != NULL)
	{
		NEXUS_Memory_Free(pDstBlock);
		pDstBlock = NULL;
	}

	SSTR_BUFFERMGR_SetReadLock(pBufferMgr, FALSE);
	g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_DONE;
	g_ePreBufferingTaskChangeState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_DONE;
	PrintExit;
	return 0;
}
#endif

HLS_ERROR_e DI_STREAM_HLS_SetTimeForSeek(HLS_HANDLE hHls, HUINT64 ullTime)
{
	HLS_t* pHLS = NULL;
	PrintEnter;
	if(hHls == NULL)
	{
		PrintError("%s(%d) Err> HLS HANDLE is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pHLS = __getHLS(hHls);
	if(pHLS->bRequestedSeek == TRUE)
	{
		PrintExit;
		return HLS_FAIL;
	}
	VK_SEM_Get(pHLS->ulSema);
	pHLS->ullTimeForSeek = ullTime;
	pHLS->bRequestedSeek = TRUE;
	VK_SEM_Release(pHLS->ulSema);
	PrintExit;
	return HLS_OK;
}


#ifdef USE_HLS_SEEK_IMPROVEMENT
#else
// HLS Seek 성능 개선전 코드 보관용.
// !Caution! P_STREAM_HLS_Decryption() 함수는 drr_bcm, drr_bcm2 마다  다르다.
static HLS_ERROR_e P_STREAM_HLS_Decryption(HUINT8* pBuf, HINT32 nBufLen, HUINT8* pKey, HUINT8* pIv, m3u8Method_e eMethod, m3u8EncPadding_e eEncPadding)
{
	DI_CRYPT_CryptMode eCryptMode = DI_CRYPT_AES;
	HUINT32 unKeySize = 16;
	HUINT32 ulDecSize = 0;
	DRV_Error errCode = DRV_OK;
	NEXUS_Error nexusError;
	HUINT8 *pSrcBlock = NULL;
	HUINT8 *pDstBlock = NULL;
	HINT32 nBlockLen = 0;
	switch(eMethod)
	{
		case M3U8_METHOD_UNKNOWN:
		case M3U8_METHOD_AESCBC128:
			unKeySize = 16;
			eCryptMode = DI_CRYPT_AES;
			if(eEncPadding == M3U8_ENCPADDING_UNKNOWN)
			{
				eEncPadding = M3U8_ENCPADDING_PKCS7;
			}
			break;
#if defined(CONFIG_SES_HDPLUS_SMARTTV)
		case M3U8_METHOD_NONE:
			NEXUS_Security_FreeKeySlot(KeyHandle);
			return 0;
#endif //CONFIG_SES_HDPLUS_SMARTTV

	}
	nBlockLen = HLS_CONST_DEFAULT_DMABLOCK;
	nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pSrcBlock);
	if(nexusError != NEXUS_SUCCESS)
	{
		if(nBlockLen > 0)
		{
			nBlockLen = nBlockLen / 2;
		}
		else
		{
			PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
			return 0;
		}
		nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pSrcBlock);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n", __FUNCTION__,__LINE__,nexusError);
			return 0;
		}
	}
	nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pDstBlock);
	if(nexusError != NEXUS_SUCCESS)
	{
		if(pSrcBlock != NULL)
		{
			NEXUS_Memory_Free(pSrcBlock);
			if(nBlockLen > 0)
			{
				nBlockLen = nBlockLen / 2;
			}
			else
			{
				PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
				return 0;
			}
			nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pSrcBlock);
			if(nexusError != NEXUS_SUCCESS)
			{
				PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
				return 0;
			}
			nexusError = NEXUS_Memory_Allocate(nBlockLen, NULL, (void *)&pDstBlock);
			if(nexusError != NEXUS_SUCCESS)
			{
				if(pSrcBlock != NULL)
				{
					NEXUS_Memory_Free(pSrcBlock);
				}
				PrintError("%s(%d) Err> Failed allocating from Nexus Heap. (%d)\n",  __FUNCTION__,__LINE__,nexusError);
				return 0;
			}
		}
	}
	PrintDebug("%s(%d) Debug> In/Out Buffer Block Allocated %d * 2\n",__FUNCTION__,__LINE__,nBlockLen);
	PrintDebug("%s(%d) Debug> Enc Data Size %d\n",__FUNCTION__,__LINE__,nBufLen);
	if(pSrcBlock != NULL && pDstBlock != NULL)
	{
		HUINT32 unPadding = 0;
		HUINT32 unStartTick = 0;
		HUINT32 unEndTick = 0;
		unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		size_t szRemain = nBufLen;
		HINT32 nLen = nBlockLen;
		HINT32 nPos = 0;
		while(szRemain > 0)
		{
			if(szRemain < (HUINT32)nBlockLen)
			{
				nLen = szRemain;
			}
			VK_MEM_Memcpy(pSrcBlock, pBuf+nPos,nLen);
			//eErrRet = P_STREAM_HLS_DMATransfer(pSrcBlock,pDstBlock,nLen,g_ulDmaHandle,KeyHandle,bResetCrypto);
			errCode = DRV_CRYPT_EndecryptMedia(eCryptMode, pSrcBlock, nLen, pDstBlock, pKey, unKeySize, pIv, unKeySize, FALSE);
			if(errCode != DRV_OK)
			{
				PrintError("%s(%d) Err> Decryption failed \n",__FUNCTION__,__LINE__);
				break;
			}
			szRemain-=nLen;
			if(szRemain > 0 && eMethod == M3U8_METHOD_AESCBC128)
			{
				if(pIv != NULL)
				{
					VK_MEM_Memcpy(pIv,pBuf+nPos+nLen-unKeySize,unKeySize);
				}
			}
			VK_MEM_Memcpy(pBuf+nPos,pDstBlock,nLen);
			VK_MEM_Memset(pSrcBlock,0x00,nLen);
			VK_MEM_Memset(pDstBlock,0x00,nLen);
			nPos+=nLen;
			ulDecSize+=nLen;
			// bResetCrypto = FALSE;
		}
		if(eEncPadding == M3U8_ENCPADDING_PKCS7 && unKeySize > 0)
		{
			unPadding = ulDecSize % unKeySize;
			if(unPadding == 0)
			{
				unPadding = unKeySize;
			}
			if((ulDecSize - unPadding) > 0 && (HINT32)ulDecSize <= nBufLen)
			{
				for(HUINT32 i = ulDecSize - unPadding ; i < ulDecSize ; i++)
				{

					if(pBuf[i] != unPadding)
					{
						unPadding = 0;
						break;
					}
				}
			}
			if(unPadding != 0)
			{
				ulDecSize-=unPadding;
//				PrintDebug("[Padding! %u] %d -> %u \n",unPadding,nBufLen,ulDecSize); // REMOVE
			}
			else
			{
//				PrintDebug("[Did not Padding! %u] %d -> %u \n",unPadding,nBufLen,ulDecSize); // REMOVE
			}
		}
		unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		PrintDebug("%s(%d) Debug> Decryption %d Bytes Complete (Delay : %u)\n",__FUNCTION__,__LINE__,nPos,unEndTick - unStartTick);
	}
	if(pSrcBlock != NULL)
	{
		NEXUS_Memory_Free(pSrcBlock);
	}
	if(pDstBlock != NULL)
	{
		NEXUS_Memory_Free(pDstBlock);
	}
	return HLS_OK;
}


int P_STREAM_HLS_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	SSTR_BUFFERMGR_t* pBufferMgr = NULL;
	HUINT32 unSegSize = 0;
	size_t nSize = 0;
	HINT32  vRet = HLS_OK;
	HLS_t *pHLS = NULL;
	HUINT32 unStartTick = 0;
	HUINT32 unEndTick = 0;
	HUINT64 ullRTD = 0;
	SSTR_HEURISTICS_t* pHeuristics = NULL;
	HUINT32 ulBitrate = 0;
	HINT8 *pUrl = NULL;
	STREAM_ACCESS_t *pSegmentAccess = NULL;

	PrintEnter;
	UNUSED_PARAM(path);
	UNUSED_PARAM(mode);
	UNUSED_PARAM(ptHandle);
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) : Error> SSTR Controller is NULL.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pHLS = __getHLS(g_hHLS);
	(void)mem_file_close(pHLS->pMemFd);

	ulBitrate = HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl);
#ifdef CONFIG_HLS_LIVE_RESTART
	/* BBC CATAL에서는 낮은 bitrate으로 시작을 해서 높은 비트렛으로 Play해야 한다. TC205, TC235 */
    /* 코드가 검증되면 ifdef로 묶어 둘 필요 없다. */
	HLS_CONTROLLER_SetStreamToProbe(pHLS->hCtrl);
#else

	HLS_CONTROLLER_GearHighest(pHLS->hCtrl);
#endif
	pUrl = HLS_CONTROLLER_GetCurSegmentUrl(pHLS->hCtrl);
	PrintDebug("%s(%d) Initialize Url : %s\n",__FUNCTION__,__LINE__,pUrl);
	if(pUrl == NULL)
	{
		return HLS_FAIL;
	}
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	HUINT32 ulLimitDelay = (HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl) * HLS_CONST_MILLISEC);
	HLS_ERROR_e eDownloadErr = HLS_FAIL;
	HUINT32 bTimeout = FALSE;
#ifdef CONFIG_HLS_LIVE_RESTART
	HLS_CONTROLLER_SetSegmentByTime(pHLS->hCtrl, 0); // probing은 처음 segment로 한다.
#endif
#if 1 // __SUPPORT_BYTERANGE__
	pSegmentAccess = pHLS->pSegmentAccess;

	if(pSegmentAccess != NULL)
	{
		(void)STREAM_HTTP_Close (pSegmentAccess);

		if ( pSegmentAccess->psz_path != NULL )
		{
			DD_MEM_Free(pSegmentAccess->psz_path);
			pSegmentAccess->psz_path = NULL;
		}
		DD_MEM_Free(pSegmentAccess);
		pSegmentAccess = NULL;
	}
#endif

	while((eDownloadErr = P_HLS_DownloadCurSegment(pHLS,ulLimitDelay)) != HLS_OK)
	{
		if(eDownloadErr == HLS_FAIL)
		{
			pHLS->eIO_Type = HLS_IO_TYPE_NONE;
			return HLS_FAIL;
		}
		else
		{
			if(eDownloadErr == HLS_PANIC_DETECTED
			|| HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) == HLS_CONTROLLER_GetCurMinBitrate(pHLS->hCtrl))
			{
				ulLimitDelay = 0;
			}
			else if(eDownloadErr == HLS_TIMEOUT)
			{
				if(bTimeout == TRUE)
				{
#ifdef CONFIG_HLS_LIVE_RESTART
					/* lowest stream에는 audio만 있고 다른 stream에는 video도 있는 경우, audio만 있는 stream으로 probing 하면 안된다. */
					HLS_CONTROLLER_SetStreamToProbe(pHLS->hCtrl);
#else
					HLS_CONTROLLER_GearLowest(pHLS->hCtrl);
#endif
				}
				else
				{
					bTimeout = TRUE;
				}
			}
			else
			{
				pHLS->eIO_Type = HLS_IO_TYPE_NONE;
				return HLS_FAIL;
			}
		}
	}
#ifdef CONFIG_HLS_LIVE_RESTART
		pHLS->unProbingBandwidth = HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) ;
		HLS_CONTROLLER_GetCurStreamIndex(pHLS->hCtrl, &(pHLS->unProbingStreamIndex));
		HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &(pHLS->unProbingSegmentID));
#endif
	pHLS->pMemFd = mem_file_open(pHLS->sz_SegSize);
	if(pHLS->pMemFd == NULL)
	{
		(void)mem_file_close(pHLS->pMemFd);
		pHLS->pMemFd = NULL;
		return HLS_FAIL;
	}
	nSize = mem_file_write(pHLS->pMemFd,pHLS->pSegBuff,pHLS->sz_SegSize);
	if(nSize != pHLS->sz_SegSize)
	{
		(void)mem_file_close(pHLS->pMemFd);
		pHLS->pMemFd = NULL;
		return HLS_FAIL;
	}
	mem_file_seek(pHLS->pMemFd,0);
	unSegSize = mem_file_bound(pHLS->pMemFd);
	if(HLS_CONTROLLER_IsCurStreamEncrypted(pHLS->hCtrl) == TRUE)
	{
#ifdef USE_HLS_FAKEKEYS_FOR_TEST /* for TEST (MUST remove xxx) */
		HUINT8 fakekeys[16] = { 0x66, 0xB3, 0x01, 0xA6, 0xE9, 0x09, 0xF9, 0x3D,0x16, 0x73, 0xD9, 0x80, 0xF6, 0xBE, 0x1C, 0x47};
#endif
		HUINT8 emptyiv[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		HUINT32 unDecSize = 0;
		HUINT64 ullSegment = 0;
		if(P_HLS_GetKey(pHLS) == HLS_OK)
		{
			if(pHLS->pCurIV == NULL)
			{
				(void)HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &ullSegment);
				_ultohex(ullSegment,emptyiv,16);
				pHLS->pCurIV = emptyiv;
			}
			else
			{
				if(HexString2Binary((HINT8 *)pHLS->pCurIV, emptyiv, 16) == TRUE)
				{
					pHLS->pCurIV = emptyiv;
				}
			}

#ifdef USE_HLS_FAKEKEYS_FOR_TEST /* for TEST (MUST remove xxx) */
			unDecSize = P_STREAM_HLS_Decryption(pHLS->pMemFd->buf, mem_file_bound(pHLS->pMemFd), fakekeys, emptyiv, pHLS->eEncMethod, pHLS->eEncPadding );
#else
			unDecSize = P_STREAM_HLS_Decryption(pHLS->pMemFd->buf, mem_file_bound(pHLS->pMemFd), pHLS->ucCurKey, pHLS->pCurIV, pHLS->eEncMethod, pHLS->eEncPadding );
#endif
			if( unDecSize > 0 && unDecSize < pHLS->sz_SegSize )
			{
				unSegSize = unDecSize;
			}
			pHLS->pCurIV = NULL;
		}
	}
#ifdef USE_NAGRAPRM
	else if(HLS_CONTROLLER_IsCurStreamEncrypted(pHLS->hCtrl) == FALSE)  // [02013.01.18.][khyeo][EXT-X-KEY] :
	{
		if(P_HLS_GetKey_NAGRAPRM(pHLS) != HLS_OK)
		{
			(void)mem_file_close(pHLS->pMemFd);
			pHLS->pMemFd = NULL;

			return HLS_FAIL;
		}
	}
#endif //USE_NAGRAPRM
	unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	ullRTD = unEndTick - unStartTick;
	if(ullRTD == 0) /* avoid divide zero */
	{
		ullRTD = 10;
	}
	(void)HLS_CONTROLLER_SetClosestBandwidth(pHLS->hCtrl, ulBitrate);
	pHeuristics = HLS_CONTROLLER_GetHeuristics(pHLS->hCtrl);
	if(pHeuristics != NULL)
	{
		SSTR_HEURISTICS_Run(pHeuristics,unSegSize,ullRTD);
	}
	pHLS->eIO_Type = HLS_IO_TYPE_PROBE;
	pBufferMgr = HLS_CONTROLLER_GetBufferMgr(pHLS->hCtrl);
	if(pBufferMgr != NULL && SSTR_BUFFERMGR_GetBufferSize(pBufferMgr) >= unSegSize)
	{
		if(SSTR_BUFFERMGR_Write(pBufferMgr, pHLS->pMemFd->buf, unSegSize ) != unSegSize)
		{
			pHLS->eIO_Type = HLS_IO_TYPE_NONE;
			return HLS_FAIL;
		}
	}
#ifdef HLS_CONFIG_WRITE_SEGMENT_FOR_TEST
	{
//		HUINT32 unSegmentNum = 0;
		HUINT8 strBuf[32] = {0};
		VK_sprintf(strBuf,"/home/segment_init");
		pHLS->fd=open(strBuf,O_CREAT|O_TRUNC|O_WRONLY|O_SYNC,0666);
		write(pHLS->fd,pHLS->pMemFd->buf, unSegSize);
		close(pHLS->fd);
	}
#endif
	PrintExit;
	return vRet;
}


int DI_STREAM_HLS_PreBuffering (DI_IPTUNER_Index_e nTunerIndex)
{
	HINT32 nWriteSize=0;
	HUINT32 unRetry=0;
	size_t sz_Read = 0;
	off_t offsetForSeek = 0;
	off_t cur = 0;
	HBOOL bWaitingForUpdate = FALSE;
	HUINT32 unProgramIdxForUpdate = 0;
	HUINT32 unStreamIdxForUpdate = 0;
	HUINT32 unDelayForUpdate = 0;
	HUINT64 ullRequestedSeekPoint;
	HUINT32 ulTimeForSeek = 0;
	HUINT32 unStartTick = 0;
	HUINT32 unEndTick = 0;
	HUINT64 ullRTD = 0;
	HUINT32 uSegmentDuration = 0;
#ifdef CONFIG_HLS_LIVE_RESTART
	HBOOL   bSkippedFirstSeekZeroForLive = FALSE; // BBC Catal 에서 재생 시작전 seek 0가 오는 오류 수정을 위해
#endif

	SSTR_BUFFERMGR_t* pBufferMgr = NULL;
	SSTR_HEURISTICS_t* pHeuristics = NULL;
	HLS_t* pHLS = NULL;
	PrintEnter;
	if(nTunerIndex != DI_IPTUNER_STREAM_HLS)
	{
		PrintError("%s(%d) : Error> invalid Tuner Type(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
		return DI_ERR_ERROR;
	}
	if(g_hHLS == NULL)
	{
		PrintError("%s(%d) Error> HLS HANDLE is NULL \n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pHLS = __getHLS(g_hHLS);
	pBufferMgr = HLS_CONTROLLER_GetBufferMgr(pHLS->hCtrl);
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> pBufferMgr is NULL\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}
	pHeuristics = HLS_CONTROLLER_GetHeuristics(pHLS->hCtrl);
	if(pHeuristics == NULL)
	{
		PrintError("%s(%d) Error> pHeuristics is NULL\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	if(g_ePreBufferingTaskState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_READY)
	{
		g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED;
	}
//	g_ePreBufferingTaskChangeState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED;
	unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
	// start live update task
	pHLS->bIsPrebuffering = TRUE;
#ifdef CONFIG_HLS_LIVE_RESTART
	// Initialize LiveRestart
	pHLS->bOnLiveRestart = FALSE;
	if (HLS_CONTROLLER_IsCurStreamEndList(pHLS->hCtrl) == FALSE)
	{
		HLS_CONTROLLER_Setup(pHLS->hCtrl, 0, pHLS->unProbingStreamIndex, 0); // probing stream에서 재생하도록 ...
		HLS_CONTROLLER_SetSegmentToLive(pHLS->hCtrl);
		(void)SSTR_BUFFERMGR_Clear(pBufferMgr);
	}
#endif
	do
	{
		if(pBufferMgr == NULL)
		{
			PrintError("pBufferMgr is NULL\n");
			goto exit_stop;
		}
		if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
			goto exit_stop;
		}
		if(HLS_CONTROLLER_IsCurStreamEndList(pHLS->hCtrl) == FALSE)
		{
			if(pHLS->eLiveUpdateStatus == DI_IPTUNER_HLS_LIVE_UPDATE_STARTED)
			{
				if( HLS_CONTROLLER_GetCurProgramIndex(pHLS->hCtrl, &unProgramIdxForUpdate) == HLS_OK
				 && HLS_CONTROLLER_GetCurStreamIndex(pHLS->hCtrl, &unStreamIdxForUpdate) == HLS_OK
				 && HLS_CONTROLLER_GetCurUpdateDelay(pHLS->hCtrl, &unDelayForUpdate) == HLS_OK )
				{
					P_STREAM_HLS_RequestUpdate(pHLS, unProgramIdxForUpdate, unStreamIdxForUpdate, (unDelayForUpdate * HLS_CONST_WAITING * 10) / 2, FALSE);
				}
			}
		}
		if(g_ePreBufferingTaskState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING
		&& bWaitingForUpdate == TRUE)
		{
			if(HLS_CONTROLLER_NextSegment(pHLS->hCtrl) == HLS_OK)
			{
				bWaitingForUpdate = FALSE;
				g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED;
			}
		}

		/***********************************************************************
		 *  1. Seek 요청에 대한 처리
		 *     Seek Time이 포함된 Segment를 찾아서 Current Segment로 세팅
		 *     Segment 내에서의 Seek Position(offset) 을 Bitrate 를 이용하여 측정
		 ***********************************************************************/
		if(P_STREAM_HLS_IsSeekRequested(pHLS))
		{
			seek_operation:
#ifdef CONFIG_HLS_LIVE_RESTART
			if((HLS_CONTROLLER_IsCurStreamEndList(pHLS->hCtrl) == FALSE)&&(bSkippedFirstSeekZeroForLive==FALSE)&&(0==P_STREAM_HLS_GetTimeForSeek(pHLS)))
			{
				bSkippedFirstSeekZeroForLive = TRUE;
				/* BBC iPlayer의 시작전 seek 0를 무시 하기 위해 */
				P_STREAM_HLS_SetRequestSeek(pHLS,FALSE);
				SSTR_BUFFERMGR_SetReadLock(pBufferMgr, FALSE);
				goto skip_seek_zero_done;
			}
#endif
			PrintDebug("%s(%d) P_STREAM_HLS_IsSeekRequested\n",__FUNCTION__,__LINE__);
			VK_SEM_Get(pHLS->ulDownloadingSema);
			ullRequestedSeekPoint = P_STREAM_HLS_GetTimeForSeek(pHLS);
			if(P_STREAM_HLS_IsValidTimeForSeek(pHLS,ullRequestedSeekPoint))
			{
#ifdef CONFIG_HLS_LIVE_RESTART
				P_STREAM_HLS_CheckLiveRestart(pHLS,ullRequestedSeekPoint);
#endif
				ulTimeForSeek = ullRequestedSeekPoint;
				HLS_CONTROLLER_SetSegmentByTime(pHLS->hCtrl, ulTimeForSeek);
				offsetForSeek = (ulTimeForSeek - HLS_CONTROLLER_GetCurSegmentStartTime(pHLS->hCtrl)) * (HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) / 8);
//			reset_buffer:
				SSTR_HEURISTICS_SetHeuristicsState(pHeuristics,SSTR_HEUR_STATE_TIMID);
				(void)SSTR_BUFFERMGR_Clear(pBufferMgr);
				SSTR_HEURISTICS_Run(pHeuristics,0,0);
				HLS_CONTROLLER_GearDown(pHLS->hCtrl);
				g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED;
				bWaitingForUpdate = FALSE;
				if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
					VK_SEM_Release(pHLS->ulDownloadingSema);
					goto exit_stop;
				}
			}
			P_STREAM_HLS_SetRequestSeek(pHLS,FALSE);
			SSTR_BUFFERMGR_SetReadLock(pBufferMgr, FALSE);
			VK_SEM_Release(pHLS->ulDownloadingSema);
		}
skip_seek_zero_done:
		if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
			goto exit_stop;
		}
		if(g_ePreBufferingTaskState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING)
		{
			VK_TASK_Sleep(HLS_CONST_WAITING);
			continue;
		}

		if((SSTR_BUFFERMGR_GetBufferSize(pBufferMgr) * HLS_CONST_HIGHEST_BUFFERING_THRESHOLD)
		> SSTR_BUFFERMGR_GetReadableSize(pBufferMgr) /*&& SSTR_BUFFERMGR_GetWritableSize(pBufferMgr) != 0*/)
		{
			HLS_ERROR_e eHLSError = HLS_OK;
			/***********************************************************************
			 *  2. Download Current Segment
			 *     Segment Buffer에 Current Segment를 Download 받는다.
			 *     ( Bufferfullness 90% 이하인 경우 )
			 ***********************************************************************/
			VK_SEM_Get(pHLS->ulDownloadingSema);
			eHLSError = P_HLS_DownloadCurSegment(pHLS, (HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl) * HLS_CONST_MILLISEC * 2));
			if(eHLSError != HLS_OK)
			{
				if(eHLSError == HLS_PANIC_DETECTED)
				{
					VK_SEM_Release(pHLS->ulDownloadingSema);
					continue;
				}
				if( eHLSError == HLS_TIMEOUT )
				{
					VK_SEM_Release(pHLS->ulDownloadingSema);
					SSTR_HEURISTICS_Reset(pHeuristics);
					SSTR_HEURISTICS_SetHeuristicsState(pHeuristics, SSTR_HEUR_STATE_PANIC);
					HLS_CONTROLLER_GearLowest(pHLS->hCtrl); /* set lowest bitrate */
					continue;
				}
				if(P_STREAM_HLS_IsSeekRequested(pHLS))
				{
					VK_SEM_Release(pHLS->ulDownloadingSema);
					goto seek_operation;
				}
				if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
					VK_SEM_Release(pHLS->ulDownloadingSema);
					goto exit_stop;
				}
				unRetry++;
				if(unRetry > HLS_CONST_RETRY_COUNT_ON_NET_ERROR)
				{
					pHLS->bNeedLiveRecovery = TRUE;
				}
				PrintError ("%s(%d) Download Segment Fail Retry...%u !!! \n",__FUNCTION__,__LINE__,unRetry);
				VK_SEM_Release(pHLS->ulDownloadingSema);
				VK_TASK_Sleep(HLS_CONST_WAITING);
				continue;
			}
			else
			{
				pHLS->bNeedLiveRecovery = FALSE;
				unRetry = 0;
			}
			if(HLS_CONTROLLER_IsCurStreamEncrypted(pHLS->hCtrl) == TRUE)
			{
#ifdef USE_HLS_FAKEKEYS_FOR_TEST /* for TEST (MUST remove xxx) */
				HUINT8 fakekeys[16] = { 0x66, 0xB3, 0x01, 0xA6, 0xE9, 0x09, 0xF9, 0x3D,0x16, 0x73, 0xD9, 0x80, 0xF6, 0xBE, 0x1C, 0x47};
#endif
				HUINT8 emptyiv[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
				HUINT32 unDecSize = 0;
				HUINT64 ullSegment = 0;
				if(P_HLS_GetKey(pHLS) == HLS_OK)
				{
					if(pHLS->pCurIV == NULL)
					{
						(void)HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &ullSegment);
						_ultohex(ullSegment,emptyiv,16);
						pHLS->pCurIV = emptyiv;
					}
					else
					{
						if(HexString2Binary((HINT8 *)pHLS->pCurIV, emptyiv, 16) == TRUE)
						{
							pHLS->pCurIV = emptyiv;
						}
					}

#ifdef USE_HLS_FAKEKEYS_FOR_TEST /* for TEST (MUST remove xxx) */
					unDecSize = P_STREAM_HLS_Decryption(pHLS->pSegBuff, pHLS->sz_SegSize , fakekeys, emptyiv, pHLS->eEncMethod, pHLS->eEncPadding );
#else
					unDecSize = P_STREAM_HLS_Decryption(pHLS->pSegBuff, pHLS->sz_SegSize ,pHLS->ucCurKey, pHLS->pCurIV, pHLS->eEncMethod, pHLS->eEncPadding);
#endif
					if( unDecSize > 0 && unDecSize < pHLS->sz_SegSize )
					{
						pHLS->sz_SegSize = unDecSize;
					}

					pHLS->pCurIV = NULL;
				}
			}
#ifdef USE_NAGRAPRM
			else if(HLS_CONTROLLER_IsCurStreamEncrypted(pHLS->hCtrl) == FALSE)  // [02013.01.18.][khyeo][EXT-X-KEY] :
			{
				if(P_HLS_GetKey_NAGRAPRM(pHLS) != HLS_OK)
				{
					VK_SEM_Release(pHLS->ulDownloadingSema);
					return HLS_FAIL;
				}
			}
#endif //USE_NAGRAPRM
#ifdef HLS_CONFIG_WRITE_SEGMENT_FOR_TEST
			{
				HUINT64 ullSegmentNum = 0;
				HUINT8 strBuf[32] = {0};
				(void)HLS_CONTROLLER_GetCurSegmentId(pHLS->hCtrl, &ullSegmentNum);
				VK_sprintf(strBuf,"/home/segment_%u",ullSegmentNum);
				pHLS->fd =open(strBuf,O_CREAT|O_TRUNC|O_WRONLY|O_SYNC,0666);
				write(pHLS->fd,pHLS->pSegBuff, pHLS->sz_SegSize);
				close(pHLS->fd);
			}
#endif
			VK_SEM_Release(pHLS->ulDownloadingSema);

			if(HLS_CONTROLLER_IsCurSegmentDiscontinuity(pHLS->hCtrl) == TRUE)
			{
				uSegmentDuration = HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl);
				DI_STREAM_HLS_ProcessDiscontinuity(uSegmentDuration, pBufferMgr);
			}

			unEndTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
			/***********************************************************************
			 *  3. Buffering
			 *     Download된 Current Segment를 Buffer에 Writing 한다.
			 *     Seek이 요청된경우 Segment내의 Seek Position(Offset) 부터 Buffer에 Writing한다.
			 ***********************************************************************/
			if(pHLS->sz_SegSize <= offsetForSeek)
			{
				offsetForSeek = ((HLS_CONTROLLER_GetCurSegmentDuration(pHLS->hCtrl) - (ulTimeForSeek - HLS_CONTROLLER_GetCurSegmentStartTime(pHLS->hCtrl))) * (HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) / 8));
				offsetForSeek = pHLS->sz_SegSize > offsetForSeek ? pHLS->sz_SegSize - offsetForSeek : pHLS->sz_SegSize - (HLS_CONTROLLER_GetCurBitrate(pHLS->hCtrl) / 8);
			}
			cur = offsetForSeek;
			sz_Read = pHLS->sz_SegSize - cur;
			offsetForSeek = 0;
			ulTimeForSeek = 0;
			while(sz_Read > 0)
			{
				if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
				{
					PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
					goto exit_stop;
				}
				if(P_STREAM_HLS_IsSeekRequested(pHLS))
				{
					goto seek_operation;
				}
				nWriteSize = SSTR_BUFFERMGR_Write(pBufferMgr,pHLS->pSegBuff + cur, sz_Read);
				if(nWriteSize == 0)
				{
					VK_TASK_Sleep(HLS_CONST_WAITING);
					continue;
				}
				cur += nWriteSize;
				sz_Read -= nWriteSize;
			}

			/***********************************************************************
			 *	4. Bitrate Change
			 *	   Heuristics Module을 이용하여 Bitrate Changing 수행
			 ***********************************************************************/
			ullRTD = unEndTick - unStartTick;
			if(ullRTD == 0) /* avoid divide zero */
			{
				ullRTD = 10;
			}
//			PrintDebug("%s(%d) : Debug> RTD : %"PRId64"\n",__FUNCTION__,__LINE__,ullRTD);
//			PrintDebug("%s(%d) : Debug> SegmentSize : %u\n",__FUNCTION__,__LINE__,pHLS->sz_SegSize );
			SSTR_HEURISTICS_Run(pHeuristics,pHLS->sz_SegSize,ullRTD);
			unStartTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
		}
		else
		{
			VK_TASK_Sleep(HLS_CONST_WAITING);
//			PrintDebug ("%s(%d) BUFFER IS ALMOST FULL( %d percent over )\n",__FUNCTION__,__LINE__,SSTR_CONST_HIGHEST_BUFFERING_THRESHOLD * 100);
			continue;
		}
#if 0
		if(stAdaptiveCallback.cbAVStreamCallback != NULL)
		{
			stAdaptiveCallback.cbAVStreamCallback(nTunerIndex,SSTR_CONTROLLER_GetCurStreamType(hSstr), SSTR_CONTROLLER_GetCurrentBitrate(hSstr),SSTR_CONTROLLER_GetCurrentFragmentId(hSstr));
		}
#endif
#if defined(CONFIG_SES_HDPLUS_SMARTTV)
		/***********************************************************************
		 * This sequence are only used to RTLNow app with SMARTTV.
		 * 4-1. Check the current segment whether it is the first changed segment of advertisement.
		 *     So, if it is the first changed segment of advertisement,
		 *     the isChangeAD flag would be changed from FALSE to TRUE.
		 ***********************************************************************/
		if(HLS_CONTROLLER_IsCurSegmentChangedTs(pHLS->hCtrl) == TRUE)
		{
			g_isChangedTs = TRUE;
		}
#endif
		/***********************************************************************
		 *	5. Setting Next Segment
		 *	   다음 Segment를 Current Segment로 지정한다.
		 *	   Next Segment가 없을 경우 EOS 처리 한다.
		 ***********************************************************************/
		 VK_SEM_Get(pHLS->ulDownloadingSema);
		if(HLS_CONTROLLER_NextSegment(pHLS->hCtrl) != HLS_OK)
		{
			g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING;
			if(HLS_CONTROLLER_IsCurStreamEndList(pHLS->hCtrl) == FALSE)
			{
				bWaitingForUpdate = TRUE;
				if( HLS_CONTROLLER_GetCurProgramIndex(pHLS->hCtrl, &unProgramIdxForUpdate) == HLS_OK
					 && HLS_CONTROLLER_GetCurStreamIndex(pHLS->hCtrl, &unStreamIdxForUpdate) == HLS_OK
					 && HLS_CONTROLLER_GetCurUpdateDelay(pHLS->hCtrl, &unDelayForUpdate) == HLS_OK )
				{
					P_STREAM_HLS_RequestUpdate(pHLS, unProgramIdxForUpdate, unStreamIdxForUpdate, 0, TRUE);
				}
			}
			VK_SEM_Release(pHLS->ulDownloadingSema);
			VK_TASK_Sleep(HLS_CONST_WAITING);
			continue;
		}
		VK_SEM_Release(pHLS->ulDownloadingSema);

		if(HLS_CONTROLLER_IsCurSegmentDiscontinuity(pHLS->hCtrl) == TRUE)
		{
#if 0	// is not ready to process the DISCONTINUITY tag
			g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING;
			// TODO: Send Event to MW/PAL with HLS_CONTROLLER_GetCurSegmentDateTime(pHLS->hCtrl);
			VK_TASK_Sleep(HLS_CONST_WAITING);
			continue;
#endif
		}
	}while (g_ePreBufferingTaskChangeState != DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING); /* end while */

	//VK_SEM_Release(s_ulPreBufferingLock); 	//--------------------------------	Critical Section
exit_stop:
	SSTR_BUFFERMGR_SetReadLock(pBufferMgr, FALSE);
	g_ePreBufferingTaskState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_DONE;
	g_ePreBufferingTaskChangeState = DI_IPTUNER_HLS_BUFFERING_TASK_STATE_DONE;
	PrintExit;
	return 0;
}
#endif


void DI_STREAM_HlsUnderflowMonitorAwake(void)
{
#ifdef CONFIG_HLS_UNDERFLOW_MONITORING_FOR_BBC_CATAL
	DI_IPTUNER_UnderflowHlsMonitorAwakeThread();
#endif
}


void DI_STREAM_HlsUnderflowMonitorSleep(void)
{
#ifdef CONFIG_HLS_UNDERFLOW_MONITORING_FOR_BBC_CATAL
	DI_IPTUNER_UnderflowHlsMonitorSleepThread();
#endif
}

#if 1 // __SUPPORT_BYTERANGE
static HLS_ERROR_e int_hls_open(HLS_t* pHLS ,STREAM_ACCESS_t *pSegmentAccess, HINT8 *pUrl)
{
	HINT32 nReConnectionTry = 0;
	HINT32 nRet = HLS_OK;

	if(pHLS->hCtrl == NULL)
	{
		PrintError("%s(%d) Error> HLS Controller is NULL \n", __FUNCTION__, __LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pSegmentAccess == NULL)
	{
		PrintError("%s(%d) : Error> Segment Access is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if( pUrl == NULL )
	{
		PrintError("%s(%d) : Error> HLS_NO_URL_TO_DOWNLOAD! it may be the end\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}

	VK_MEM_Memset(pSegmentAccess,0x00,sizeof(STREAM_ACCESS_t));
	pSegmentAccess->psz_path = VK_StrDup(pUrl);
	if(pSegmentAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		nRet = HLS_FAIL;
		PrintExit;
		goto open_done;
	}

	while(TRUE)
	{
		if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
			goto open_done;
		}
		if(P_STREAM_HLS_IsSeekRequested(pHLS))
		{
			PrintError("%s(%d) : Seek Request\n",__FUNCTION__,__LINE__);
			nRet = HLS_FAIL;
			PrintExit;
			goto open_done;
		}
		nRet = STREAM_HTTP_Open(pSegmentAccess);
		if(nRet == DI_IPT_SUCCESS)
		{
			nRet = HLS_OK;
			break;
		}
#if 0
		else if(nRet == RETURN_ENETUNREACH || nRet == RETURN_ECONNFAIL1 || nRet == RETURN_ECONNFAIL2 || nRet == RETURN_EREQFAIL5xx || nRet == RETURN_EREQFAIL)
		{
			if( nReConnectionTry < HLS_CONST_RETRY_COUNT_ON_NET_ERROR )
			{
				PrintError( "Retry to open. Just 3 times. nReConnectionTry [ %d ]\n", nReConnectionTry);
				nReConnectionTry++;
				continue;
			}

			PrintError( "Connect failed?? (If It's VOD, Infinity Retry!\n");
			if( FALSE == HLS_CONTROLLER_IsCurStreamEndList( pHLS->hCtrl) )
			{
				PrintError( "%s(%d) Select Next Segment for Live & Probe\n",__FUNCTION__,__LINE__);
				nRet = HLS_DOWNLOAD_FAIL;
				PrintExit;
				goto open_done;
			}
			else
			{
				VK_TASK_Sleep(HLS_CONST_WAITING);
				continue;
			}

		}
#endif
		//else if(nRet == RETURN_ERROR || nRet == RETURN_EREQFAIL4xx)
		else if(nRet == DI_IPT_EGENERIC)
		{
			PrintError( "ERROR> STREAM_HTTP_Open Unknown Error!\n");
			nRet = HLS_FAIL;
			PrintExit;
			goto open_done;
		}
	}

open_done:
#if 0
	if(nRet != HLS_OK)
	{
		(void)STREAM_HTTP_Close (pSegmentAccess);
		if ( pSegmentAccess->psz_path != NULL )
		{
			DD_MEM_Free(pSegmentAccess->psz_path);
			pSegmentAccess->psz_path = NULL;
		}
		if ( pSegmentAccess->psz_user_agent != NULL )
		{
			DD_MEM_Free(pSegmentAccess->psz_user_agent);
			pSegmentAccess->psz_user_agent = NULL;
		}
	}
#endif
	return nRet;
}

static HLS_ERROR_e int_hls_seek(HLS_t* pHLS /* Could be NULL */,STREAM_ACCESS_t *pAccess, off_t offset, HINT32 max_attempt /* 0 is unlimited */)
{
	HINT32 retry_count = max_attempt;
	//int ret = RETURN_SUCCESS;
	HINT32 nSeekRetryCount = 0;
	HINT32 nRet = 0;
	HINT32 nReConnectionTry = 0;

	if(pHLS != NULL)
	{
		if(pHLS->hCtrl == NULL)
		{
			PrintError("%s(%d) Error> HLS Controller is NULL \n", __FUNCTION__, __LINE__);
			PrintExit;
			return HLS_FAIL;
		}
	}

	if( pAccess == NULL)
	{
		PrintError("######### [%s:%d] Critical Section: Handle is NULL\n",__FUNCTION__,__LINE__);
		nRet = HLS_FAIL;
		PrintExit;
		goto seek_done;
	}
	if( pAccess->info.i_size < offset )
	{
		nRet = HLS_FAIL;
		PrintExit;
		goto seek_done;
	}

	do
	{
		nRet = STREAM_HTTP_Seek(pAccess, offset);
		if(nRet != DI_IPT_SUCCESS)
		{
			nSeekRetryCount++;
			PrintError("%s(%d) Error> STREAM_HTTP_Seek fail... Retrying seek request...\n"
			,__FUNCTION__
			,__LINE__
			,nSeekRetryCount);

			if(g_ePreBufferingTaskChangeState == DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING)
			{
				PrintDebug("%s(%d) Break Buffering\n",__FUNCTION__,__LINE__);
				goto seek_done;
			}

			if(P_STREAM_HLS_IsSeekRequested(pHLS))
			{
				nRet = HLS_FAIL;
				PrintExit;
				goto seek_done;
			}
#if 0
			if(nRet == RETURN_ENETUNREACH || nRet == RETURN_ECONNFAIL1 || nRet == RETURN_ECONNFAIL2 || nRet == RETURN_EREQFAIL5xx || nRet == RETURN_EREQFAIL)
			{
				if( nReConnectionTry < HLS_CONST_RETRY_COUNT_ON_NET_ERROR )
				{
					PrintError( "%s(%d) Retry to open. Just 3 times. nReConnectionTry [ %d ]\n",__FUNCTION__,__LINE__, nReConnectionTry);
					nReConnectionTry++;
					continue;
				}

				PrintError( "Connect failed?? (If It's VOD, Infinity Retry!\n");
				if(pHLS && FALSE == HLS_CONTROLLER_IsCurStreamEndList( pHLS->hCtrl) ) // for Live
				{
					PrintError( "%s(%d) Select Next Segment for Live\n",__FUNCTION__,__LINE__);
					nRet = HLS_DOWNLOAD_FAIL;
					PrintExit;
					goto seek_done;
				}
				else
				{
					SYS_TASK_Sleep(HLS_CONST_WAITING);
					continue;
				}
			}
#endif
			//else if(nRet == RETURN_ERROR || nRet == RETURN_EREQFAIL4xx)
			if(nRet == DI_IPT_EGENERIC)
			{
				PrintError( "ERROR> STREAM_HTTP_Open Unknown Error!\n");
				nRet = HLS_FAIL;
				PrintExit;
				goto seek_done;
			}
			if(retry_count != 0 && nSeekRetryCount > retry_count)
			{
				PrintError("%s(%d) Error> STREAM_HTTP_Seek Retrying fail. Last Retrying count(%d) ...\n"
					,__FUNCTION__
					,__LINE__
					,nSeekRetryCount);
				nRet = HLS_FAIL;
				PrintExit;
				goto seek_done;
			}
		}
		else
		{
			nRet = HLS_OK;
			PrintExit;
			break;
		}
	}
	while(TRUE);

seek_done:
	return nRet;
}

#endif


#if __HLS_GLOBAL_FUNC_END__
#endif
