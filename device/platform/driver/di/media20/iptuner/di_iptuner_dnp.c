/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_dnp.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Mon Apr 20 10:20:48 KST 2009 $
 * File Description:	Download & Play Tuner Body
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/


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
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <math.h>

#include "bstd.h"
#include "vfio20.h"

#include "vkernel.h"
#include "taskdef.h"
#include "drv_err.h"

#include "di_media20.h"
#include "di_uart.h"

#include "util_url.h"
#include "util_sys.h"
#include "di_iptuner20.h"
#include "di_iptuner_config.h"
#include "di_iptuner_dnp.h"
#include "di_iptuner_mms.h"
#include "di_iptuner_pool.h"
#include "dnp_olt.h"
#include "util_index_parser.h"
#include "util_clock.h"
#include "dnp_olt.h"
#include "adp_buffermgr.h"
#include "stream_http.h"

#include "drv_drm.h"
#include "bfile_types.h"

/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */

//#define DI_IPTUNER_DNP_DEBUG
#ifdef IPTUNER_ALL_DEBUG
#ifdef DI_IPTUNER_DNP_DEBUG
//#define DEBUG_IO_FUNCTION 				/* warning : heavy log messages */
//#define VFIO_TYPE_DEBUG

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
#endif

#define UNUSED_PARAM(x)		((void)x)

#define	TUNER_DNP_STACK_SIZE					(256*1024)	/* 256Kbytes */
#define	TUNER_DNP_TASK_PRIORITY					(10)
#define	TUNER_DNP_MSG_QUEUE_SIZE				(10)
#define	TUNER_DNP_WAITEVTTIME					(60000)		/* 3000msec = 3sec */
#define	TUNER_DNP_CALLBACK_WAITCOUNT			(100000)		/* 20 * 100msec = 2sec */
#define TUNER_DNP_PROBE_SEEK_CALLBACK_WAITCOUNT (100000)	/* 100000 times X TUNER_DNP_CALLBACK_WAITTIME */
#define	TUNER_DNP_SEEK_CALLBACK_WAITCOUNT		(100000)	/* 100000 times X TUNER_DNP_CALLBACK_WAITTIME */
#define	TUNER_DNP_CALLBACK_WAITTIME			(100)		/* 100msec = 0.1sec */
#define	TUNER_DNP_MSGQ_TIMEOUT				(500)


#define MAX_IP_SIZE			(128)	/** bytes */
#define MAX_PORT_SIZE		(16)	/** bytes */
#define MAX_FILE_NAME_SIZE	(2048)	/** bytes */ /** FIXME : will be extended for URL size */

/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */

/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/

#if __________p__________
#endif
/** VFIO (for DNP) Open Function */
static int P_IPTUNER_DnpOpen (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *pVfioHandle)
{
	UNUSED_PARAM (path);
	UNUSED_PARAM (mode);
	UNUSED_PARAM (pVfioHandle);

	return VFIO_ERROR_SUCCESS;
}

/** VFIO (for DNP) Close Function */
static int P_IPTUNER_DnpClose (VFIO_Handle_t tHandle)
{
	tHandle.pTunerHandle = NULL;
	return VFIO_ERROR_SUCCESS;
}

/** VFIO (for DNP) Read Function */
static int P_IPTUNER_DnpRead (VFIO_Handle_t tHandle, void *pBuf, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	off_t cur=0;
	ssize_t nRead=0;
	HUINT32 uiRetryCount=0;
	HBOOL bResult, bReadReady=FALSE;
	DI_IPTUNER_t *pHTuner=NULL;
	size_t unRequestSize = (size*nmemb);
	HUINT32 uiTryCount=TUNER_DNP_CALLBACK_WAITCOUNT;
	HUINT32 uiTryInterval=TUNER_DNP_CALLBACK_WAITTIME;

/*************************
*	1. NULL check
*************************/
	if(pBuf == NULL)
	{
		PrintError("Invalid parameter, pBuf\n");
		*ptReadSize = -1;
		return VFIO_ERROR_IN_DEVICE;
	}

	pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;
	if(pHTuner == NULL)
	{
		PrintError ("Invalid parameter, pTunerHandle\n");
		*ptReadSize = -1;
		return VFIO_ERROR_IN_DEVICE;
	}

/*************************
*	2. Get the current position
*************************/
	cur = DNP_OLT_Seek(pHTuner->pOLT, 0, SEEK_CUR, DNP_OLT_SEEK_FOR_READ);
	if (cur == (off_t)-1)
	{
		PrintError ("lseek failed!\n");
		*ptReadSize = -1;
		return VFIO_ERROR_IN_DEVICE;
	}

	if ( cur >= (off_t)pHTuner->ulDNPFileLength )
	{
		PrintError ("playback requests invalid range %"PRId64"!!\n", cur);
		*ptReadSize = -1;
		return VFIO_ERROR_BAD_PARAMETER;
	}
	PrintDebug("[+] VFIO Read (0x%llX)(%s)\n", cur, (tHandle.eOpenType == VFIO_OPEN_DATA)?"DATA":"INDEX");

/*************************
*	3. Keep the read offset
*************************/
	if(tHandle.eOpenType == VFIO_OPEN_DATA)
	{
		bResult = DNP_OLT_SetDataReadOffset(pHTuner->pOLT, cur);
		if(bResult != TRUE)
		{
			PrintError("DNP_OLT_SetDataReadOffset failed!\n");
			*ptReadSize = -1;
			return VFIO_ERROR_IN_DEVICE;
		}
	}

/*************************
*	4. main loop
*************************/
	if((pHTuner->tMediaInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_MP3)&&
		(pHTuner->tMediaInfo.video_codec == DI_VIDEO_FORMAT_NONE))
	{
		uiTryCount = 1;
		uiTryInterval = 0;
	}

	while(uiRetryCount++ < uiTryCount)
	{
		if(pHTuner->bErrorExit == TRUE)
		{
			PrintError("bErrorExit flag is ON!\n");
			*ptReadSize = -1;
			return VFIO_ERROR_SUCCESS;
		}

/*************************
*	Break out if the session is closed!
*************************/
		if(tHandle.eOpenType == VFIO_OPEN_DATA)
		{
			if(pHTuner->bExitRead == TRUE)
			{
				PrintError("Forced Read Exit!!!\n\n\n\n");
				pHTuner->bExitRead=FALSE;
				break;
			}
		}
		else if(tHandle.eOpenType == VFIO_OPEN_INDEX)
		{
			if(pHTuner->bExitIndexRead == TRUE)
			{
				PrintError("Forced Index Read Exit!!!\n\n\n\n");
				pHTuner->bExitIndexRead=FALSE;
				break;
			}
		}

		pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_START;
		DNP_OLT_SetJumpRequestPosition(pHTuner->pOLT, cur);

		if ( FALSE == DNP_OLT_IsFilled (pHTuner->pOLT, cur, (HINT64)unRequestSize) )
		{
			bReadReady = FALSE;
		}
		else
		{
			bReadReady = TRUE;
			break;
		}
		(void) VK_TASK_Sleep (uiTryInterval);
	}

/*************************
*	5. Data read
*************************/
	if(bReadReady == TRUE)
	{
		nRead = DNP_OLT_Read(pHTuner->pOLT, pBuf, unRequestSize);
		if ( nRead > 0)
		{
			*ptReadSize = nRead;
		}
	}
	else
	{
		PrintError("VFIO tryied %d times to read but failed!\n", TUNER_DNP_CALLBACK_WAITCOUNT);
		*ptReadSize = BFILE_ERROR_NO_DATA;
	}
	PrintDebug("[-] VFIO Read (0x%llX)(%s)\n", cur, (tHandle.eOpenType == VFIO_OPEN_DATA)?"DATA":"INDEX");

	return VFIO_ERROR_SUCCESS;
}

/**
 * VFIO (for DNP) Seek Function
 * call by : DI_MEDIA_Probe
 *
 * Probe를 시도할 때 bmedia player단에서 요구하는 Probe Point까지
 * Download 받지 못한 경우 Seek을 통해 받을때까지 Check 하는 Function
 *
 * 해당 offset까지 Seek 이 성공하는 경우 bmedia_player는 비로소 Probe를 시도한다.
 */
static int P_IPTUNER_DnpSeek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	off_t	curOffset;
	off_t			cur=0;
	DI_IPTUNER_t 	*pHTuner=NULL;

/*************************
*	1. NULL check
*************************/
	pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;
	if(pHTuner == NULL)
	{
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

/*************************
*	2. get the current offset
*************************/
	cur = DNP_OLT_Seek(pHTuner->pOLT, 0, SEEK_CUR, DNP_OLT_SEEK_FOR_READ);
	if ( (off_t)-1 == cur )
	{
		PrintError ("%s (%d) Error>  DNP_OLT_Seek failed!\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	if( SEEK_CUR == whence )
	{
		if(cur+offset >= (off_t)pHTuner->ulDNPFileLength)
		{
			PrintDebug("Seek Request is beyond the file length\n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else if ( SEEK_SET == whence )
	{
		if(offset >= (off_t)pHTuner->ulDNPFileLength)
		{
			PrintDebug("Seek Request is beyond the file length\n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else if( SEEK_END == whence )
	{
		offset = pHTuner->ulDNPFileLength - 1; /* last offset */
		whence = SEEK_SET;
	}

/*************************
*	3. OLT Seek
*************************/
	curOffset = DNP_OLT_Seek(pHTuner->pOLT, offset, whence, DNP_OLT_SEEK_FOR_READ);
	if ( (off_t)-1 == curOffset )
	{
		PrintError ("%s (%d) Error>  DNP_OLT_Seek failed!\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	return VFIO_ERROR_SUCCESS;
}

/** VFIO (for DNP) Tell Function */
static int P_IPTUNER_DnpTell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	int		ret=VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t 	*pHTuner=NULL;

/*************************
*	1. NULL check
*************************/
	pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;
	if(pHTuner == NULL)
	{
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		ret = VFIO_ERROR_IN_DEVICE;
		return ret;
	}

/*************************
*	2. OLT Seek with SEEK_CUR
*************************/
	*plCurOffset = DNP_OLT_Seek(pHTuner->pOLT, 0, SEEK_CUR, DNP_OLT_SEEK_FOR_READ);
	if ( (off_t)-1 == *plCurOffset )
	{
		PrintError ("%s (%d) Error> DNP_OLT_Seek failed!\n",__FUNCTION__,__LINE__);
		ret = -1;
	}

	return ret;
}

/** VFIO (for DNP) IsEOF Function */
static int P_IPTUNER_DnpIsEOF (VFIO_Handle_t tHandle)
{
	off_t		cur=0;
	int ret=0;
	DI_IPTUNER_t *pHTuner=NULL;

	pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;
	if(pHTuner == NULL)
	{
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	cur = DNP_OLT_Seek(pHTuner->pOLT, 0, SEEK_CUR, DNP_OLT_SEEK_FOR_READ);
	if(cur == (off_t)-1)
	{
		PrintError("DNP_OLT_Seek failed!\n");
		return VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		if(cur == (off_t)(pHTuner->ulDNPFileLength - 1))
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
		}

	return ret;
}

/** VFIO (for DNP) OpenFromNexus Function */
static int P_IPTUNER_DnpOpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *pVfioHandle)
{
	DI_IPTUNER_t *pHTuner=NULL;
	UNUSED_PARAM (flags);

	if (pVfioHandle == NULL || path == NULL)
	{
		PrintError("pVfioHandle is NULL\n");
		return VFIO_ERROR_BAD_PARAMETER;
	}

/*************************
*	Tie up the VFIO Handle and Tuner handle
*************************/
	pHTuner = DI_IPTUNER_POOL_FindBySaveFileName ((HINT8 *)path);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) Error> pHTuner is NULL\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	if(pHTuner->bVFIOTypeSet == FALSE)
	{
		pVfioHandle->eOpenType = VFIO_OPEN_DATA;
		pHTuner->bVFIOTypeSet = TRUE;
	}
	else
	{
		pVfioHandle->eOpenType = VFIO_OPEN_INDEX;
	}
	pVfioHandle->pTunerHandle = pHTuner;
	pVfioHandle->ucAvailable = 1;

	return VFIO_ERROR_SUCCESS;
}

/** VFIO (for DNP) SeekFromNexus Function */
static off_t P_IPTUNER_DnpSeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	off_t			curOffset=(off_t)-1;
	off_t			cur=0;
	DI_IPTUNER_t	*pHTuner=NULL;

	pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;
	if(pHTuner == NULL)
	{
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return -1;
	}

	cur = DNP_OLT_Seek(pHTuner->pOLT, 0, SEEK_CUR, DNP_OLT_SEEK_FOR_READ);
	if ( (off_t)-1 == cur )
	{
		PrintError ("%s (%d) DNP_OLT_Seek failed!\n",__FUNCTION__,__LINE__);
		curOffset = (off_t)-1;
	}
	else
	{
		curOffset = DNP_OLT_Seek(pHTuner->pOLT, offset, whence, DNP_OLT_SEEK_FOR_READ);
	}

	return curOffset;
}

/** VFIO (for DNP) BoundsFromNexus Function */
static int P_IPTUNER_DnpBoundsForNexus(VFIO_Handle_t tHandle, off_t *file_size)
{
	int		ret=VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pHTuner = NULL;

	/* Get IPTUNER handle from VFIO Handle. It is bound at VFIO_Open or VFIO_OpenFromNexus */
	pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;
	if(pHTuner == NULL)
	{
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		ret = VFIO_ERROR_IN_DEVICE;
		return ret;
	}

	*file_size = pHTuner->ulDNPFileLength;

	return ret;
}

DRV_Error P_IPTUNER_DnpUpdateBufferingPosition(DI_IPTUNER_t *pHTuner, HINT64 llOffset)
{
	if(pHTuner == NULL)
	{
		PrintError("Invalid parameter!\n");
		return DRV_ERR;
	}

	DNP_OLT_SetJumpRequestPosition(pHTuner->pOLT, llOffset);

	return DRV_OK;
}


DI_ERR_CODE P_IPTUNER_DnpConvertTimeToByteOffset(DI_IPTUNER_t *pHTuner, HUINT32 ulTime , HUINT64 *pEndByte)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HUINT64	ulOffset=0;
	HUINT32 unTarget = 0;

	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		return nRet;
	}

	if (pHTuner->tMediaInfo.duration == 0)
	{
		PrintError ("%s(%d) Error> Not Support ConvertTimeToByteOffset(Contents duration=0)\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		return nRet;
	}

	if(ulTime <= pHTuner->tMediaInfo.duration)
		unTarget = ulTime;
	else
	{
		*pEndByte = pHTuner->ulDNPFileLength;
		return nRet;
	}

	if ( pHTuner->eTimeLine == DI_IPTUNER_TIMELINE_INDEX_FILE )
	{
		if ( UTIL_Index_GetOffsetByPosition (pHTuner->pIndexRoom, ulTime, &ulOffset) < 0)
		{
			PrintError ("%s(%d) Warning> There is a wrong X-IndexFile Header. Switch to StreamItSelf TimeLine \n",__FUNCTION__,__LINE__);
			if(pHTuner->tMediaInfo.duration == 0)
			{
				PrintError("duration is 0!!\n");
				return DI_ERR_ERROR;
			}
			else
			{
				ulOffset = (HUINT64) (ulTime * pHTuner->ulDNPFileLength/pHTuner->tMediaInfo.duration);
			}
		}
	}
	else if (pHTuner->eTimeLine == DI_IPTUNER_TIMELINE_BYTESPERSECOND)
	{
		ulOffset = (HUINT64) ( ulTime * pHTuner->nTimeLineBPS );
		if(ulOffset == 0)
		{
			PrintError ("%s(%d) Warning> There is a wrong X-BytePerSecond Header. Switch to StreamItSelf TimeLine \n",__FUNCTION__,__LINE__);
			if(pHTuner->tMediaInfo.duration == 0)
			{
				PrintError("duration is 0!!\n");
				return DI_ERR_ERROR;
			}
			else
			{
				ulOffset = (HUINT64) (ulTime * pHTuner->ulDNPFileLength/pHTuner->tMediaInfo.duration);
			}
		}
	}
	else /* default DI_IPTUNER_TIMELINE_STREAM base.*/
	{
		/* This method can't calculate the byteOffset precisely. */
		if(pHTuner->tMediaInfo.duration == 0)
		{
			PrintError("duration is 0!!\n");
			return DI_ERR_ERROR;
		}
		else
		{
			ulOffset = (HUINT64) (ulTime * pHTuner->ulDNPFileLength/pHTuner->tMediaInfo.duration);
		}
	}

	*pEndByte = ulOffset;
	return nRet;
}

HBOOL P_IPTUNER_DnpCheckUnderflow(DI_IPTUNER_t *pHTuner)
{
	DNP_OLT_t *pOLT = NULL;
	HBOOL bPassStartThreshold=FALSE;
	HBOOL bPassStartConstant=FALSE;
	HBOOL bUnderFlow=FALSE;
	HUINT64 ullBufferedData=0;
	HULONG ulCurrentClock=0;
	HULONG ulClockDifference=0;
	HUINT32	unBufferedMilliseconds=0;
	HUINT64 ullLeftData=0;
	HUINT64 ullLeftRunningTime=0;
	HINT64 llNextEmptyOffset=0;
	HINT64 llReadOffset=0;
	HUINT32 uiVideoBufferDepth=0, uiVideoFifoSize =0;
	HUINT32 uiPumpBufferDepth=0, uiPumpFifoSize =0;
	HUINT32 uiDownloadSpeed=0;
	HUINT32 uiContentsBitrate=0;
	HUINT64 ullDownloadedBytes=0;
	HUINT32 uiDownloadedChunks=0;
	HUINT32 uiTotalChunks=0;
	HUINT32 uiCurrentChunk=0;

/*************************
*	1. validate parameters
*************************/
	if((pHTuner == NULL)||(pHTuner->pOLT == NULL))
	{
		PrintError("Invalid parameter, pHTuner is null!\n");
		return FALSE;
	}

	pOLT = (DNP_OLT_t *)pHTuner->pOLT;
	if(pOLT->bComplete == TRUE)
	{
		return FALSE;
	}

	if(pHTuner->eUnderflowMode == DI_IPTUNER_UNDERFLOW_AUTO)
	{
		uiDownloadSpeed = DNP_OLT_GetBufferingRate(pOLT);
		uiContentsBitrate = pHTuner->tMediaInfo.max_bitrate/8;
//		VK_printf("DownSpeed: %u KBps ContentsBitrate: %u KBps\n",uiDownloadSpeed, uiContentsBitrate);

		if(uiDownloadSpeed > uiContentsBitrate)
		{
			bUnderFlow = FALSE;
			pHTuner->uiChunksToBuffer = 0;
		}
		else
		{
/*************************
*	Update ChunksToBuffer
*************************/
			uiCurrentChunk = pHTuner->ullPauseCheckOffset >> pOLT->nOLTShiftCount;
			uiTotalChunks = pOLT->llTotalDataBytes >> pOLT->nOLTShiftCount;
			uiTotalChunks -= uiCurrentChunk;

			ullDownloadedBytes = DNP_OLT_GetDownloadedBytes(pOLT, pHTuner->ullPauseCheckOffset);
			pHTuner->uiChunksToBuffer = uiTotalChunks - (uiTotalChunks * ((double)uiDownloadSpeed / uiContentsBitrate));

			ullDownloadedBytes = DNP_OLT_GetDownloadedBytes(pOLT, pHTuner->ullPauseCheckOffset);
			uiDownloadedChunks = ullDownloadedBytes >> pOLT->nOLTShiftCount;

			if(uiDownloadedChunks >= pHTuner->uiChunksToBuffer)
			{
				bUnderFlow = FALSE;
			}
			else
			{
				bUnderFlow = TRUE;
			}
//			VK_printf("uiCurrent: %u Downloaded: %u BufferOffset: %u\n", uiCurrentChunk, uiDownloadedChunks, uiCurrentChunk+pHTuner->uiChunksToBuffer);
		}
	}
	else if(pHTuner->eUnderflowMode == DI_IPTUNER_UNDERFLOW_THRESHOLD)
	{
/*************************
*	1. check StartThreshold
*************************/
		if(pHTuner->tVideoBufferCallback != NULL)
		{
			pHTuner->tVideoBufferCallback((HUINT32)pHTuner, &uiVideoBufferDepth, &uiVideoFifoSize);
			ullBufferedData += uiVideoBufferDepth;
		}

		if(pHTuner->tPumpBufferCallback != NULL)
		{
			pHTuner->tPumpBufferCallback((HUINT32)pHTuner, &uiPumpBufferDepth, &uiPumpFifoSize);
			ullBufferedData += uiPumpBufferDepth;
		}

		ullBufferedData += DNP_OLT_GetBufferedDataSize(pHTuner->pOLT);
		if (ullBufferedData && pHTuner->ulDNPFileLength && pHTuner->tMediaInfo.duration)
		{
			unBufferedMilliseconds = ullBufferedData / (pHTuner->ulDNPFileLength / pHTuner->tMediaInfo.duration);
		}

		if(pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_RESUME)
		{
			if(unBufferedMilliseconds < (pHTuner->ulStartThreshold/10))
			{
				bPassStartThreshold = FALSE;
			}
			else
			{
				bPassStartThreshold = TRUE;
			}
		}
		else if(pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_PAUSE)
		{
			if(unBufferedMilliseconds < pHTuner->ulStartThreshold)
			{
				bPassStartThreshold = FALSE;
			}
			else
			{
				bPassStartThreshold = TRUE;
			}
		}

/*************************
*	3. check StartConstant
*************************/
		ulCurrentClock = UTIL_GetClockMsec ();
		if(ulCurrentClock > pOLT->ulDelayedStartClock)
		{
			ulClockDifference = ulCurrentClock - pOLT->ulDelayedStartClock;
		}
		else
		{
			ulClockDifference = 0;
		}

		if (ulClockDifference>0)
		{
			if((HULONG)unBufferedMilliseconds >(HULONG)(ulClockDifference + pHTuner->ulStartConstant))
			{
				bPassStartConstant = TRUE;
			}
			else
			{
				bPassStartConstant = FALSE;
			}
		}

		if(bPassStartThreshold == FALSE)
		{
//			PrintDebug("Buffer: [%d mesc]\n",unBufferedMilliseconds);
		}

/*************************
*	Do not pause if the left amount is less than threshold.
*************************/
		llReadOffset = (HUINT64)DNP_OLT_GetDataReadOffset(pHTuner->pOLT);
		ullLeftData = pHTuner->ulDNPFileLength - llReadOffset;
		if(ullLeftData && pHTuner->ulDNPFileLength && pHTuner->tMediaInfo.duration)
		{
			ullLeftRunningTime = ullLeftData / (pHTuner->ulDNPFileLength / pHTuner->tMediaInfo.duration);
		}

		if(ullLeftRunningTime <= pHTuner->ulStartThreshold)
		{
			bPassStartThreshold = TRUE;
		}

		llNextEmptyOffset = DNP_OLT_GetNextEmptyPosition(pHTuner->pOLT, llReadOffset);
		if(llNextEmptyOffset == -2)
		{
			bPassStartThreshold = TRUE;
		}

		bUnderFlow = (!bPassStartThreshold) && (!bPassStartConstant);
	}

	return bUnderFlow;
}

HBOOL P_IPTUNER_DnpCheckOverflow(DI_IPTUNER_t *pHTuner)
{
	HUINT32 uiContentBPS = 0;
	HUINT64 ullThresholdInBytes = 0;
	HINT64 llBufferedDataSize = 0;
	HINT32 nBufSnapBPS = 0;

	if(pHTuner == NULL)
	{
		PrintError("Invalid parameter!\n");
		return FALSE;
	}

/******************************
*	Buffering Mode Check
******************************/
	if(pHTuner->eBufferingMode == DI_IPTUNER_MODE_UNLIMITED)
	{/* Just Go */
		return FALSE;
	}
	else
	{
		if(pHTuner->tMediaInfo.duration != 0)
		{
			uiContentBPS = (HUINT32)(pHTuner->ulDNPFileLength*1000 / pHTuner->tMediaInfo.duration);
		}
		else
		{
			uiContentBPS = pHTuner->tMediaInfo.max_bitrate;
		}
		ullThresholdInBytes = (pHTuner->nBufferThreshold)/1000 * uiContentBPS;
		llBufferedDataSize = DNP_OLT_GetBufferedDataSize(pHTuner->pOLT);

		if(llBufferedDataSize <= (HINT64)ullThresholdInBytes)
		{
/******************************
*	Fast Filling Condition
******************************/
			if(pHTuner->bFastFillIsLimited)
			{
				nBufSnapBPS = DNP_OLT_GetBufferingRate(pHTuner->pOLT);
				if(nBufSnapBPS > 0)
				{
					if (nBufSnapBPS > (HINT32)(uiContentBPS * pHTuner->dFastFillLimitMultiple))
					{
						return TRUE;
					}
				}
			}
		}
		else
		{
/******************************
*	Managed Filling Condition
******************************/
			if(pHTuner->eBufferingMode == DI_IPTUNER_MODE_FIXED)
			{
				return TRUE;
			}
			else if(pHTuner->eBufferingMode == DI_IPTUNER_MODE_THRESHOLD)
			{
				nBufSnapBPS = DNP_OLT_GetBufferingRate(pHTuner->pOLT);
				if(nBufSnapBPS > (HINT32) (uiContentBPS * pHTuner->dManagedFillLimitMultiple)) /* warning: comparison between signed and unsigned */
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

int P_DNP_STREAM_HTTP_DnpReceive(DI_IPTUNER_t *pHTuner)
{
	HINT32 iResult=0;
	HUINT32 uiChunkSize=0;
	HUINT32 uiReadSize=0;
	HINT8* pChunk=NULL;
	HINT64 llDownloadOffset=0;
	HINT64 llJumpRequestOffset=0;
	HINT64 llNextEmptyOffset=0;
	HBOOL bResult=FALSE;
	HUINT32 uiRetryCount = 0;
	size_t offset=0;
	size_t readsize=0;
	ssize_t readresult=0;
	HUINT32 nRetryStreamReadCount = 0;
	HINT64 seekResult=0;
	STREAM_ACCESS_t *pAccess=NULL;
	RT_STREAMING_INSTANCE_t * pInstance = NULL;

	if(pHTuner == NULL)
	{
		PrintError("invalid parameter!\n");
		return -1;
	}

/******************************
*	Alloc memory
******************************/
	uiChunkSize = DNP_OLT_GetOneChunkSize(pHTuner->pOLT);
	pChunk = (HINT8*)VK_MEM_Alloc(uiChunkSize);
	if(pChunk == NULL)
	{
		PrintError("Out of Memory!\n");
		return -1;
	}

	pAccess = (STREAM_ACCESS_t*)VK_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		pHTuner->bErrorExit = TRUE;
		PrintError("Out of memory!\n");
		VK_MEM_Free(pChunk);
		return -1;
	}
	VK_MEM_Memset(pChunk,0,uiChunkSize);
	VK_MEM_Memset(pAccess,0,sizeof(STREAM_ACCESS_t));

/******************************
*	Open Connection (Redirection check first)
******************************/
	pInstance = (RT_STREAMING_INSTANCE_t*)pHTuner->pRealTimeInstance;
	if(pInstance == NULL)
	{
		pHTuner->bErrorExit = TRUE;
		PrintError("pHTuner->pRealTimeInstance is null!\n");
		VK_MEM_Free(pChunk);
		VK_MEM_Free(pAccess);
		return -1;
	}

	if(pInstance->pUrl != NULL)
	{
		pAccess->psz_path = VK_StrDup((void *)pInstance->pUrl);
	}
	else
	{
		pAccess->psz_path = VK_StrDup((void *)UTIL_GetOriginalURL((const char *)pHTuner->pUrl));
	}

	iResult = STREAM_HTTP_Open(pAccess);
	if(iResult != DI_IPT_SUCCESS)
	{
		pHTuner->bErrorExit = TRUE;
		VK_MEM_Free(pChunk);
		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}
		VK_MEM_Free(pAccess);
		return -1;
	}
	pHTuner->ulDNPFileLength = pAccess->info.i_size;

	(void) DNP_OLT_InitSpeedGun (pHTuner->pOLT);

/******************************
*	pass through cached area
******************************/
	llDownloadOffset = DNP_OLT_GetBufferingPosition(pHTuner->pOLT);
	if(llDownloadOffset != 0x0)
	{
		iResult = STREAM_HTTP_Seek (pAccess ,llDownloadOffset);
		if(iResult != DI_IPT_SUCCESS)
		{
			PrintError("DI_STREAM_HTTP_Seek to (0x%llx) failed!... Retrying %d times\n", llDownloadOffset, uiRetryCount);
			if( TRUE == UTIL_SYS_GetLinkStatus())
			{
				if(pHTuner->tErrorEventCallback !=NULL)
				{
					pHTuner->tErrorEventCallback ( (HUINT32) pHTuner, eMEDIA_ERROR_NETWORK, eMEDIA_ERROR_R_NETWORK_SEEK_TIMEOUT );
				}
			}
			else
			{
				if(pHTuner->tErrorEventCallback !=NULL)
				{
					pHTuner->tErrorEventCallback ( (HUINT32) pHTuner, eMEDIA_ERROR_NETWORK, eMEDIA_ERROR_R_NETWORK_LINK_DOWN );
				}
			}
			STREAM_HTTP_Close(pAccess);
			if(pAccess->psz_path != NULL)
			{
				VK_MEM_Free(pAccess->psz_path);
				pAccess->psz_path = NULL;
			}
			VK_MEM_Free(pAccess);
			VK_MEM_Free(pChunk);
			pAccess = NULL;
			pChunk = NULL;
			pHTuner->bErrorExit = TRUE;
			return 0;
		}
	}

	while(1)
	{
		if(pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_START)
		{
			DNP_OLT_UpdateSpeedGun (pHTuner->pOLT);
		}

/******************************
*	Pause or Exit condition check
******************************/
		if(pHTuner->bExitDownload == TRUE)
		{
			break;
		}

		if((pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_PAUSE)
			||(pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_COMPLETE))
		{
			VK_printf("Buffering Task is PAUSED!!\n");
			(void) VK_TASK_Sleep(100);
			continue;
		}

/******************************
*	Check Overflow
******************************/
		if(pHTuner->bOverFlow == TRUE)
		{
			continue;
		}

/*******************************
*	Seek to the target position
*******************************/
		VK_SEM_Get(pHTuner->ulDNPWriteSema);
		llDownloadOffset = DNP_OLT_GetBufferingPosition(pHTuner->pOLT);

		bResult = DNP_OLT_CheckForJumping(pHTuner->pOLT);
		if(bResult == TRUE)
		{
			llJumpRequestOffset = DNP_OLT_GetJumpRequestPosition(pHTuner->pOLT);
			llNextEmptyOffset = DNP_OLT_GetNextEmptyPosition(pHTuner->pOLT, llJumpRequestOffset);
			if(llDownloadOffset != llNextEmptyOffset)
			{
//				VK_printf("Jump From 0x%llx to 0x%llx\n", llDownloadOffset, llNextEmptyOffset);
				bResult = DNP_OLT_SetBufferingPosition(pHTuner->pOLT, llJumpRequestOffset);
				if(bResult != TRUE)
				{
					PrintError("DNP_OLT_SetBufferingPosition failed!\n");
					VK_SEM_Release(pHTuner->ulDNPWriteSema);
					(void) VK_TASK_Sleep (0); /* for context switching */
					continue;
				}

				llDownloadOffset = DNP_OLT_GetBufferingPosition(pHTuner->pOLT);

				iResult = DI_IPT_EGENERIC;
				while(iResult != DI_IPT_SUCCESS)
				{
					if(pHTuner->bExitDownload == TRUE)
					{
						PrintError("user called exit!\n");
						break;
					}

					iResult = STREAM_HTTP_Seek (pAccess ,llDownloadOffset);
					if(iResult != DI_IPT_SUCCESS)
					{
						PrintError("DI_STREAM_HTTP_Seek to (0x%llx) failed!... Retrying %d times\n", llDownloadOffset, uiRetryCount);
						if( TRUE == UTIL_SYS_GetLinkStatus())
						{
							if(pHTuner->tErrorEventCallback !=NULL)
							{
								pHTuner->tErrorEventCallback ( (HUINT32) pHTuner, eMEDIA_ERROR_NETWORK, eMEDIA_ERROR_R_NETWORK_SEEK_TIMEOUT );
							}
						}
						else
						{
							if(pHTuner->tErrorEventCallback !=NULL)
							{
								pHTuner->tErrorEventCallback ( (HUINT32) pHTuner, eMEDIA_ERROR_NETWORK, eMEDIA_ERROR_R_NETWORK_LINK_DOWN );
							}
						}
						uiRetryCount++;
						(void) VK_TASK_Sleep (0); /* for context switching */
					}
				}

/*******************************
*	If iResult is not success, it means "user exit" while seek keeps to fail
*******************************/
				if(iResult != VFIO_ERROR_SUCCESS)
				{
					break;
				}
			}
		}

/*******************************
*	Check EOF
*	THRESHOLD mode: ullPauseCheckOffset must be 0x0
*	AUTO mode: ullPauseCheckOffset must be the last seek position
*******************************/
		bResult = DNP_OLT_CheckReachedEOF(pHTuner->pOLT);
		if(bResult == TRUE)
		{
			llNextEmptyOffset = DNP_OLT_GetNextEmptyPosition (pHTuner->pOLT, pHTuner->ullPauseCheckOffset);
			if(llNextEmptyOffset == -1)
			{
				pHTuner->bErrorExit = TRUE;
				PrintError("Invalid parameter!\n");
				VK_SEM_Release(pHTuner->ulDNPWriteSema);
				(void) VK_TASK_Sleep (0); /* for context switching */
				break;
			}
			else if(llNextEmptyOffset == -2)
			{
				llNextEmptyOffset = DNP_OLT_GetNextEmptyPosition (pHTuner->pOLT, 0);
				if(llNextEmptyOffset == -2)
				{
					VK_printf("File download is over!\n");
					pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_COMPLETE;
					if(pHTuner->pBufferCallbacks != NULL)
					{
						if(pHTuner->pBufferCallbacks->cbComplete != NULL)
						{
							pHTuner->pBufferCallbacks->cbComplete ((HUINT32)pHTuner);
						}
					}
					VK_SEM_Release(pHTuner->ulDNPWriteSema);
					(void) VK_TASK_Sleep (0); /* for context switching */
					break;
				}
			}

			VK_printf("File download is not over, Let's find the empty space to download!\n");
			DNP_OLT_SetJumpRequestPosition(pHTuner->pOLT, llNextEmptyOffset);
			VK_SEM_Release(pHTuner->ulDNPWriteSema);
			(void) VK_TASK_Sleep (0); /* for context switching */
			continue;
		}

/********************************
*	Read from the remote file
********************************/
		offset = 0;
		nRetryStreamReadCount = 0;
		iResult = VFIO_ERROR_SUCCESS;

		if((llDownloadOffset + uiChunkSize) > (HINT64)pHTuner->ulDNPFileLength)
		{
			readsize = pHTuner->ulDNPFileLength - llDownloadOffset;
		}
		else
		{
			readsize = uiChunkSize;
		}

		while(readsize > offset)
		{
			readresult = STREAM_HTTP_Read(pAccess, (HUINT8*)pChunk+offset, readsize-offset);
			if(readresult == 0 || readresult == DI_IPT_ETIMEOUT)
			{
				if(pHTuner->bExitDownload == TRUE)
				{
					break;
				}

				if(pAccess->info.b_eof == TRUE) /* if chunked-TE and last-chunk, */
				{
					break;
				}
				else /* Something happened in STREAM Controller , Let's try again */
				{
					if(nRetryStreamReadCount++ > 100)
					{
						iResult = VFIO_ERROR_IN_DEVICE;
						break;
					}
				}
			}
			else if(readresult < 0)
			{
				iResult = VFIO_ERROR_IN_DEVICE;
				break;
			}
			else
			{
				offset += readresult;
			}
		}
		uiReadSize = offset;

		if(iResult != VFIO_ERROR_SUCCESS)
		{
			pHTuner->bErrorExit = TRUE;
			if( TRUE == UTIL_SYS_GetLinkStatus())
			{
				if(pHTuner->tErrorEventCallback !=NULL)
				{
					pHTuner->tErrorEventCallback ( (HUINT32) pHTuner, eMEDIA_ERROR_NETWORK, eMEDIA_ERROR_R_NETWORK_SEEK_TIMEOUT );
				}
			}
			else
			{
				if(pHTuner->tErrorEventCallback !=NULL)
				{
					pHTuner->tErrorEventCallback ( (HUINT32) pHTuner, eMEDIA_ERROR_NETWORK, eMEDIA_ERROR_R_NETWORK_LINK_DOWN );
				}
			}
			VK_SEM_Release(pHTuner->ulDNPWriteSema);
			break;
		}

/********************************
*	OLT Write
********************************/
		seekResult = DNP_OLT_Seek(pHTuner->pOLT, llDownloadOffset, SEEK_SET, DNP_OLT_SEEK_FOR_WRITE);
		if(seekResult < 0)
		{
			PrintError("DNP_OLT_Seek failed!\n");
			pHTuner->bErrorExit = TRUE;

			if(pHTuner->tErrorEventCallback !=NULL)
			{
				pHTuner->tErrorEventCallback ( (HUINT32) pHTuner, eMEDIA_ERROR_IO,	eMEDIA_ERROR_R_IO_SEEK_FAIL );
			}

			VK_SEM_Release(pHTuner->ulDNPWriteSema);
			break;
		}

		if(FALSE == DNP_OLT_Write (pHTuner->pOLT, pChunk, uiReadSize))
		{
			PrintError("DNP_OLT_Write failed!! \n");
			pHTuner->bErrorExit = TRUE;

			if(pHTuner->tErrorEventCallback !=NULL)
			{
				pHTuner->tErrorEventCallback ( (HUINT32) pHTuner, eMEDIA_ERROR_IO, eMEDIA_ERROR_R_IO_WRITE_FAIL );
			}

			VK_SEM_Release(pHTuner->ulDNPWriteSema);
			break;
		}
		VK_SEM_Release(pHTuner->ulDNPWriteSema);
		(void) VK_TASK_Sleep (0); /* for context switching */
	} /* end of while */

	STREAM_HTTP_Close(pAccess);
	if(pAccess->psz_path != NULL)
	{
		VK_MEM_Free(pAccess->psz_path);
		pAccess->psz_path = NULL;
	}
	VK_MEM_Free(pAccess);
	VK_MEM_Free(pChunk);
	pAccess = NULL;
	pChunk = NULL;
	return iResult;
}

HUINT32 P_IPTUNER_GetTimeFromOffset(DI_IPTUNER_t* pHTuner, HUINT64 ullOffset)
{
	HUINT32 uiIndex=0;
	HUINT32 uiPktNum=0;
	HUINT32 uiTime=0;

	if(pHTuner->tMediaInfo.pucIndex == NULL)
	{
		PrintError("index is NULL!\n");
		return 0;
	}

/*************************
*	1. Find packet number
*************************/
	if(pHTuner->tMediaInfo.uiAsfPacketSize != 0)
	{
		uiPktNum = (ullOffset - pHTuner->tMediaInfo.ullAsfDataOffset)/pHTuner->tMediaInfo.uiAsfPacketSize;
	}
	else
	{
		PrintError("Invalid pHTuner->tMediaInfo.uiAsfPacketSize! %u\n", pHTuner->tMediaInfo.uiAsfPacketSize);
		return 0;
	}

/*************************
*	2. Find index number with packet number
*************************/
	for(uiIndex=0; uiIndex < pHTuner->tMediaInfo.uiIndexEntriesCount; uiIndex++)
	{
		if(uiPktNum == pHTuner->tMediaInfo.pucIndex[uiIndex])
		{
			break;
		}
	}

	if(uiIndex == pHTuner->tMediaInfo.uiIndexEntriesCount)
	{
		PrintError("Invalid PktNum!\n");
		return 0;
	}

/*************************
*	3. Get the Time value
*************************/
	uiTime = (uiIndex*pHTuner->tMediaInfo.ullAsfIndexTimeInterval) - pHTuner->tMediaInfo.ullAsfPreroll;

	return uiTime;
}

DI_ERR_CODE P_IPTUNER_DnpGetBufferedInfo(DI_IPTUNER_t *pHTuner, HUINT32 *uiChunks, HINT32 **iStartOffsets, HINT32 **iEndOffsets)
{
	DNP_OLT_t* pOLT;
	HINT8* pTable;
	HUINT64 ullDownloadedData=0;
	HUINT32 uiCurrentTime=0;
	HUINT32 uiLeftTime=0;
	HUINT32 uiBufferedTime=0;

/*************************
*	1. parameter validation
*************************/
	if((pHTuner == NULL) || (uiChunks == NULL) || (iStartOffsets == NULL) || (iEndOffsets == NULL))
	{
        PrintError("Invalid parameter!\n");
        return DI_ERR_ERROR;
	}

	if((pHTuner->ulDNPFileLength == 0) || (pHTuner->tMediaInfo.duration == 0))
	{
		PrintError("pHTuner->ulDNPFileLength or pHTuner->tMediaInfo.duration is zero!\n");
		return DI_ERR_ERROR;
	}

	pOLT = (DNP_OLT_t *)(pHTuner->pOLT);
	if(pOLT == NULL)
	{
		PrintError("pOLT is NULL, call StartBuffering first\n");
		return DI_ERR_ERROR;
	}
	pTable = pOLT->pTable;

/*************************
*	Get the percentage of downloaded amount
*************************/
	if(*iStartOffsets != NULL)
	{
		VK_MEM_Free(*iStartOffsets);
		*iStartOffsets = NULL;
	}
	*iStartOffsets = VK_MEM_Alloc(sizeof(HINT32));
	if(*iStartOffsets == NULL)
	{
		PrintError("Out of memory!\n");
		return DI_ERR_ERROR;
	}

	if(*iEndOffsets != NULL)
	{
		VK_MEM_Free(*iEndOffsets);
		*iEndOffsets = NULL;
	}
	*iEndOffsets = VK_MEM_Alloc(sizeof(HINT32));
	if(*iEndOffsets == NULL)
	{
		PrintError("Out of memory!\n");
		return DI_ERR_ERROR;
	}

	ullDownloadedData = DNP_OLT_GetDownloadedBytes(pOLT, pHTuner->ullPauseCheckOffset);

	if(pHTuner->ulDNPFileLength != 0)
	{
		uiCurrentTime = (double)pHTuner->ullPauseCheckOffset * pHTuner->tMediaInfo.duration / (double)pHTuner->ulDNPFileLength ;
		uiLeftTime = pHTuner->tMediaInfo.duration - uiCurrentTime;
		uiBufferedTime = (double)pHTuner->tMediaInfo.duration / (double)pHTuner->ulDNPFileLength * ullDownloadedData;

		*uiChunks = 1;
		(*iStartOffsets)[0] = 0;
		(*iEndOffsets)[0] = uiCurrentTime+uiBufferedTime;
	}
	else
	{
		*uiChunks=0;
	}
	return DI_ERR_OK;
}

DI_ERR_CODE P_IPTUNER_DnpWriteCachedData(DI_IPTUNER_t *pHTuner)
{
	HBOOL bResult;
	HUINT32 uiChunkSize=0;
	HUINT64 ullSeekResult=0;
	HUINT8 *pucBuf = NULL;
	ADP_BUFFER_ERR ret=ADP_BUFFER_OK;
	ADP_BUFFERMGR_t* pBufferMgr = NULL;
	ADP_BUFFERMGR_BlockSnapshot_t tSnapshot;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;

/*************************
*	parameter check
*************************/
	VK_MEM_Memset(&tSnapshot,0x00,sizeof(ADP_BUFFERMGR_BlockSnapshot_t));

	pInstance = pHTuner->pRealTimeInstance;
	if(pInstance == NULL)
	{
		PrintError ("%s (%d) Error> RealTime Instance is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	pBufferMgr = pInstance->pBufferMgr;
	if(pBufferMgr == NULL)
	{
		PrintError("BufferMgr is NULL!\n");
		return DI_ERR_ERROR;
	}

	uiChunkSize = pHTuner->uiBlockSize;

	pucBuf = VK_MEM_Alloc(uiChunkSize);
	if(pucBuf == NULL)
	{
		PrintError("Out of memory!\n");
		return DI_ERR_ERROR;
	}

	while(1)
	{
/*************************
*	Check aparted or not
*************************/
		ret = ADP_BUFFERMGR_NextSnapshot(pBufferMgr,&tSnapshot);
		if(ret == ADP_BUFFER_NOMOREDATA)
		{
			break;
		}
		else if(ret != ADP_BUFFER_OK)
		{
			PrintError("ADP_BUFFERMGR_NextSnapshot failed!\n");
			VK_MEM_Free(pucBuf);
			return DI_ERR_ERROR;
		}
		else if(tSnapshot.ulReadableSize < uiChunkSize)
		{
			VK_printf("Throwing away a block (size: 0x%x)\n", tSnapshot.ulReadableSize);
			continue;
		}

/*************************
*	move pointer to the offset
*************************/
		ullSeekResult = DNP_OLT_Seek(pHTuner->pOLT, tSnapshot.ullStartOffset, SEEK_SET, DNP_OLT_SEEK_FOR_WRITE);
		if(ullSeekResult != tSnapshot.ullStartOffset)
		{
			PrintError("DNP_OLT_Seek failed!\n");
			VK_MEM_Free(pucBuf);
			return DI_ERR_ERROR;
		}

		bResult = DNP_OLT_SetBufferingPosition(pHTuner->pOLT, tSnapshot.ullStartOffset);
		if(bResult != TRUE)
		{
			PrintError("DNP_OLT_SetBufferingPosition failed!\n");
			VK_MEM_Free(pucBuf);
			return DI_ERR_ERROR;
		}

/*************************
*	write to OLT
*************************/
		bResult = DNP_OLT_Write(pHTuner->pOLT, (HINT8*)tSnapshot.pBuffer, uiChunkSize);
		if(bResult != TRUE)
		{
			PrintError("DNP_OLT_Write failed!\n");
			VK_MEM_Free(pucBuf);
			return DI_ERR_ERROR;
		}
	}

	VK_MEM_Free(pucBuf);
	return DI_ERR_OK;
}

#if __________task__________
#endif
static void P_IPTUNER_DnpDownloadingTask(void *pParam)
{
	HINT32 vResult, iResult;
	DI_IPTUNER_Msg_t stMsg;
	DI_IPTUNER_t *pHTuner = (DI_IPTUNER_t*) pParam;
	DI_IPTUNER_BufferCallback_t *cbBufferStatus = NULL;

	PrintEnter;

	if (pHTuner == NULL)
	{
		PrintDebug ("%s (%d) : IPTUNER_DnpTask unavailable (pHTuner handle is null)!!!\n",__FUNCTION__,__LINE__);
		return;
	}

	cbBufferStatus = pHTuner->pBufferCallbacks;
	if (cbBufferStatus == NULL)
	{
		PrintError ("%s (%d) Error>  Not installed buffering status callbacks. Task ID (%lu)\n",__FUNCTION__,__LINE__,pHTuner->ulDNPTaskId);
		return;
	}

/*************************
*	Task finished flag is off
*************************/
	pHTuner->bDNPTaskFinished = FALSE;

/*************************
*	Download
*************************/
	while(pHTuner->bDNPTaskQuit != TRUE)
	{
		vResult = VK_MSG_ReceiveTimeout (pHTuner->ulDNPMsgQId, &stMsg, sizeof(DI_IPTUNER_Msg_t), TUNER_DNP_MSGQ_TIMEOUT);
		if ( vResult != VK_OK )
		{
			continue;
		}

		if ( stMsg.unMsg == IPTUNER_DOWNLOAD_TRIGGER_HTTP )
		{
			iResult = P_DNP_STREAM_HTTP_DnpReceive (pHTuner);
			if(iResult != 0)
			{
				pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_STOP;
				pHTuner->bDNPTaskFinished = TRUE;

				if(cbBufferStatus->cbFailure != NULL)
				{
					cbBufferStatus->cbFailure((HUINT32)pHTuner);
				}
				return;
			}
			break;
		}
		else
		{
			continue;
		}
	}

/*************************
*	Task finished flag is on
*************************/
	pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_COMPLETE;
	pHTuner->bDNPTaskFinished = TRUE;

	PrintExit;
	return;
}

#define DI_IPTUNER_DNP_PAUSE_GAUGE_MIN 0
#define DI_IPTUNER_DNP_PAUSE_GAUGE_MAX 10

static void P_IPTUNER_DnpDelayedStartTask(void* param)
{
	DI_ERR_CODE dResult;
	DI_IPTUNER_t* pHTuner = (DI_IPTUNER_t*)param;
	HUINT64 ullReadOffset=0;
	DI_IPTUNER_BufferCallback_t *cbBufferStatus = NULL;
	HBOOL bUnderflow = FALSE;

	PrintEnter;

	pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_INIT;
	pHTuner->bDelayedStartTaskFinished = FALSE;
	cbBufferStatus = pHTuner->pBufferCallbacks;
	if(cbBufferStatus == NULL)
	{
		PrintError("%s(%d) Error> cbBufferStatus is NULL!\n",__FUNCTION__,__LINE__);
		return;
	}

	while(1)
	{
		if((pHTuner->bDNPTaskQuit == TRUE) || (pHTuner->bErrorExit == TRUE))
		{
			pHTuner->bDelayedStartTaskFinished = TRUE;
			break;
		}

		if(pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_INIT)
		{
			if(cbBufferStatus->cbCanPlay != NULL)
			{
				VK_printf("========================================FROM Init TO Canplay===============================================\n");
				cbBufferStatus->cbCanPlay((HUINT32)pHTuner);
				pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_CANPLAY;
			}
		}
		else if(pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_CANPLAY)
		{
			if(pHTuner->bDelayedStart == TRUE)
			{
				if(pHTuner->tDelayedStartCallback != NULL)
				{
					VK_printf("========================================FROM Canplay TO Resume===============================================\n");
					pHTuner->tDelayedStartCallback ((HUINT32)pHTuner);
					pHTuner->bDelayedStart = FALSE;
						pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_RESUME;
						pHTuner->uiPauseGauge = DI_IPTUNER_DNP_PAUSE_GAUGE_MIN;
					}
			}
		}
		else
		{
			pHTuner->bOverFlow = P_IPTUNER_DnpCheckOverflow(pHTuner);
			bUnderflow = P_IPTUNER_DnpCheckUnderflow(pHTuner);
			if(bUnderflow == FALSE)
			{
/*************************
*	1. Resume event
*************************/
				if(pHTuner->uiPauseGauge > DI_IPTUNER_DNP_PAUSE_GAUGE_MIN)
				{
					pHTuner->uiPauseGauge--;
				}
				else
				{
					if(pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_PAUSE)
					{
						if(cbBufferStatus->cbResume != NULL)
						{
							VK_printf("========================================FROM Pause TO Resume===============================================\n");
							pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_RESUME;
							cbBufferStatus->cbResume((HUINT32)pHTuner);
						}
					}
				}
			}
			else
			{
/*************************
*	2. Pause event
*************************/
				if(pHTuner->uiPauseGauge < DI_IPTUNER_DNP_PAUSE_GAUGE_MAX)
				{
					pHTuner->uiPauseGauge++;
				}
				else
				{
					if(pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_RESUME)
					{
						if(cbBufferStatus->cbPause != NULL)
						{
							VK_printf("========================================FROM Resume TO Pause===============================================\n");
							pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_PAUSE;
							cbBufferStatus->cbPause((HUINT32)pHTuner);
						}

						ullReadOffset = (HUINT64)DNP_OLT_GetDataReadOffset(pHTuner->pOLT);
						dResult = DI_IPTUNER_DnpUpdateBufferingPosition(pHTuner, ullReadOffset);
						if(dResult != DI_ERR_OK)
						{
							PrintError("DI_IPTUNER_DnpUpdateBufferingPosition failed!\n");
							return;
						}
						pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_START;
					}
				}
			}
		}

		VK_TASK_Sleep(100);
	}
}

#if __________di__________
#endif
/*******************************************************************/
/************************ public funtions **************************/
/*******************************************************************/
DI_ERR_CODE DI_IPTUNER_DnpOpen (DI_IPTUNER_t *pHTuner)
{
	VFIO_Callback_t	stCBFunction;
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid IPTUNER Handle !!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	pHTuner->bExitRead = FALSE;
	pHTuner->bExitIndexRead = FALSE;
	/*
	 * 1. Register Callback
	 * 	아래 function들은 download 된 buffer 로 부터 file operation 을 수행한다.
	 * 	VFIO install 된 후 playback 이 start 전까지는 아래 function 은 동작하지 않는다.
	 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));
	stCBFunction.Open 			=(VFIOOpenCB)  P_IPTUNER_DnpOpen;
	stCBFunction.Close			=(VFIOCloseCB) P_IPTUNER_DnpClose;
	stCBFunction.Read			=(VFIOReadCB)  P_IPTUNER_DnpRead;
	stCBFunction.Seek			=(VFIOSeekCB)  P_IPTUNER_DnpSeek;
	stCBFunction.Tell			=(VFIOTellCB)  P_IPTUNER_DnpTell;
	stCBFunction.IsEOF			=(VFIOIsEOFCB) P_IPTUNER_DnpIsEOF;
	stCBFunction.OpenNexus		=(VFIOOpenFromNexusCB) P_IPTUNER_DnpOpenFromNexus;
	stCBFunction.SeekNexus		=(VFIOSeekForNexusCB) P_IPTUNER_DnpSeekForNexus;
	stCBFunction.BoundsNexus	=(VFIOBoundsForNexusCB) P_IPTUNER_DnpBoundsForNexus;

	(void) VFIO_RegisterCallback ((char *)pHTuner->pSaveFileName, stCBFunction);

	PrintExit;
	return nRet;
}

/*
 * 1. create dnp thread (stop state)
 * 2. protocol controller open & create vfio handle
 */
DI_ERR_CODE DI_IPTUNER_DnpCreate (DI_IPTUNER_t *pHTuner)
{
	HINT32 vResult;
	DI_IPTUNER_Msg_t s_Msg;
	DI_ERR_CODE dResult;

	PrintEnter;

/*************************
*	Initialize Status
*************************/
	pHTuner->bExitDownload = FALSE;
	pHTuner->bPauseDownload = FALSE;
	pHTuner->bDNPTaskQuit = FALSE;

/*************************
*	Initialize OLT
*************************/
	pHTuner->pOLT = (void*)DNP_OLT_Create((log(pHTuner->uiBlockSize)/log(2))-10,
											pHTuner->ulDNPFileLength,
											pHTuner->pSaveFileName);

	if (pHTuner->pOLT == NULL)
	{
		PrintError("DNP_OLT_Create failed!\n");
		return DI_ERR_ERROR;
	}

	DI_IPTUNER_DnpSetOLTEndTime (pHTuner, pHTuner->ulEndByte);
	DNP_OLT_PrintInformation (pHTuner->pOLT);

/*************************
*	Create Semaphores & Msg
*************************/
	vResult = VK_SEM_Create(&pHTuner->ulDNPWriteSema, "DNPWriteSema", VK_SUSPENDTYPE_PRIORITY);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Create failed!\n");
		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Create(&pHTuner->ulDNPSema, "DNPTaskSema", VK_SUSPENDTYPE_PRIORITY);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Create failed!\n");
		return DI_ERR_ERROR;
	}

	vResult = VK_MSG_Create(TUNER_DNP_MSG_QUEUE_SIZE, sizeof(s_Msg), "DNPMsg", &pHTuner->ulDNPMsgQId, VK_SUSPENDTYPE_FIFO);
	if(vResult != VK_OK)
	{
		PrintError("VK_MSG_Create failed!\n");
		return DI_ERR_ERROR;
	}

/*************************
*	Merge the cached data into OLT
*************************/
	dResult = P_IPTUNER_DnpWriteCachedData(pHTuner);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_IPTUNER_DnpWriteCachedData failed!\n");
		return DI_ERR_ERROR;
	}

/*************************
*	Create Tasks
*************************/
	vResult = VK_TASK_Create(P_IPTUNER_DnpDownloadingTask
					, DOWNLOAD_TASK_PRIORITY
					, TUNER_DNP_STACK_SIZE
					, "Download"
					, (void*)pHTuner
					, &pHTuner->ulDNPTaskId
					, TRUE);
	if(vResult != VK_OK)
	{
		PrintError("VK_TASK_Create failed!\n");
		return DI_ERR_ERROR;
	}

	vResult = VK_TASK_Create(P_IPTUNER_DnpDelayedStartTask
					, DOWNLOAD_TASK_PRIORITY
					, TUNER_DNP_STACK_SIZE
					, "DelayedStart"
					, (void*)pHTuner
					, &pHTuner->ulDelayedStartTaskId
					, FALSE);
	if(vResult != VK_OK)
	{
		PrintError("VK_TASK_Create failed!\n");
		return DI_ERR_ERROR;
	}

	VK_TASK_Start(pHTuner->ulDNPTaskId);
	VK_TASK_Start(pHTuner->ulDelayedStartTaskId);

/*************************
*	Start Downloading
*************************/
	if(pHTuner->eProtocolType == DI_IPTUNER_PROTOCOL_HTTP)
	{
		s_Msg.unMsg = IPTUNER_DOWNLOAD_TRIGGER_HTTP;
	}
	else
	{
		PrintError("Downloading is supported in HTTP only!\n");
	}
	VK_MSG_Send(pHTuner->ulDNPMsgQId, (HUINT32 *)&s_Msg, sizeof(DI_IPTUNER_Msg_t));

	PrintExit;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_DnpDestroy (DI_IPTUNER_t *pHTuner)
{
	HINT32	nRet = DI_ERR_OK;
	HBOOL bDNPKilled=FALSE;
	HBOOL bDelayedStartKilled=FALSE;

	PrintEnter;
	/*******************************************
	 * Check param  & tuner state
	 *******************************************/
	if (pHTuner == NULL)
	{
		PrintError ("%s (%d) Error>  Invalid Tuner Handle !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		PrintExit;
		return nRet;
	}
	/* else - { continue } */

	if (pHTuner->ulDNPTaskId == 0)
	{
		PrintError ("%s (%d) Error>  A Download Task doesn't exist. \n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		PrintExit;
		return nRet;
	}
	/* else - { continue } */

	nRet = DI_IPTUNER_DnpStop (pHTuner);
	if ( DI_ERR_OK != nRet )
	{
		PrintError ("%s (%d) Error> DI_IPTUNER_DnpStop fail\n",__FUNCTION__,__LINE__);
		return nRet;
	}

	pHTuner->bDNPTaskQuit = TRUE;

	/* instead task join, spin wait */
	while (1)
	{
		PrintDebug ("%s (%d) Waiting> until Task finishes.\n",__FUNCTION__,__LINE__);
		if((pHTuner->ulDNPTaskId == 0)||
			((pHTuner->ulDNPTaskId != 0) && (pHTuner->bDNPTaskFinished == TRUE)))
		{
			bDNPKilled = TRUE;
		}

		if((pHTuner->ulDelayedStartTaskId == 0)||
			((pHTuner->ulDelayedStartTaskId != 0) && (pHTuner->bDelayedStartTaskFinished == TRUE)))
		{
			bDelayedStartKilled = TRUE;
		}

		if((bDNPKilled == TRUE) && (bDelayedStartKilled == TRUE))
		{
			break;
		}
		VK_TASK_Sleep(100);
	}

	/* Terminate OLT Manager */
	(void) DNP_OLT_Destroy (pHTuner->pOLT);

	switch (pHTuner->eProtocolType)
	{
		case DI_IPTUNER_PROTOCOL_HTTP	:
			break;
		case DI_IPTUNER_PROTOCOL_MMS		:
			if( VFIO_ERROR_SUCCESS != DI_STREAM_MMS_Close (*(VFIO_Handle_t*)pHTuner->pVfioHandle) )
			{
				PrintError ("%s (%d) Error>  DI_STREAM_MMS_Close Fail\n",__FUNCTION__,__LINE__);
				nRet = -1;
			}

			break;
		case DI_IPTUNER_PROTOCOL_RTSP	:

		case DI_IPTUNER_PROTOCOL_FTP		:

		default :
			PrintError  ("%s (%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;

	}

	VK_TASK_Stop (pHTuner->ulDNPTaskId);
	VK_TASK_Destroy (pHTuner->ulDNPTaskId);
	pHTuner->ulDNPTaskId = 0;

	VK_TASK_Stop(pHTuner->ulDelayedStartTaskId);
	VK_TASK_Destroy(pHTuner->ulDelayedStartTaskId);
	pHTuner->ulDelayedStartTaskId = 0;

	VK_SEM_Destroy (pHTuner->ulDNPSema);
	VK_SEM_Destroy (pHTuner->ulDNPWriteSema);
	VK_MSG_Destroy(pHTuner->ulDNPMsgQId);

	pHTuner->ulDNPTaskId = 0;
	pHTuner->ulDNPSema = 0;
	pHTuner->bErrorExit = FALSE;

	PrintExit;

	return nRet;
}


DI_ERR_CODE DI_IPTUNER_DnpClose (DI_IPTUNER_t *pHTuner)
{
	HINT32	nRet = DI_ERR_OK;

	PrintEnter;

	if(NULL == pHTuner)
	{
		PrintError ("%s(%d) Error> invalid tuner handle (NULL)\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	(void) VFIO_UnregisterCallback ((char *)pHTuner->pSaveFileName);

	PrintExit;

	return nRet;
}


/* Just exit Receving Loop using exit-flag*/
DI_ERR_CODE DI_IPTUNER_DnpStop (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet=DI_ERR_OK;

	PrintEnter;

	if(NULL == pHTuner)
	{
		PrintError ("%s(%d) Error> invalid tuner handle (NULL)\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if(pHTuner->bExitDownload == FALSE )
	{
		pHTuner->bExitDownload = TRUE;
	}

	PrintExit;

	return nRet;
}

/** Download Task 를 Pause 한다. */
DI_ERR_CODE DI_IPTUNER_DnpPause (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	PrintEnter;

	if(NULL == pHTuner)
	{
		PrintError ("%s(%d) Error> invalid tuner handle (NULL)\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if(pHTuner->bPauseDownload == FALSE)
	{
		pHTuner->bPauseDownload = TRUE;
	}
	PrintExit;
	return nRet;
}

/** Download 가 Stop 된 경우 Resume의 용도로 사용한다. */
DI_ERR_CODE DI_IPTUNER_DnpResume (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	PrintEnter;

	if(NULL == pHTuner)
	{
		PrintError ("%s(%d) Error> invalid tuner handle (NULL)\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if(pHTuner->bPauseDownload == TRUE)
	{
		pHTuner->bPauseDownload = FALSE;
	}
	PrintExit;

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_DnpGetInfo (DI_IPTUNER_t *pHTuner, DI_IPTUNER_Info_t *pInfo)
{
	PrintEnter;

	if((NULL == pHTuner) || (pInfo == NULL))
	{
		PrintError ("%s(%d) Error> invalid tuner handle (NULL)\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	PrintExit;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_DnpControl (DI_IPTUNER_t *pHTuner,  DI_IPTUNER_CMD_e cmd, HUINT64 val)
{
	/* TODO : junsy */
	UNUSED_PARAM (pHTuner);
	switch (cmd)
	{
		case CMD_CONTROL_SPEED :
			PrintDebug("%s (%d) CMD_CONTROL_SPEED (val=%d)\n",__FUNCTION__,__LINE__,(HINT32 )val);
			break; /* Removed prevent-defect-CID-117603 MISSING_BREAK */
#if 1 /* Added becuase of warning: enumeration value 'xxxx' not handled in switch */
		case CMD_CONTROL_PAUSE  :
		case CMD_CONTROL_RESUME :
		case CMD_CONTROL_SEEK   :
		default :
			PrintError ("%s(%d) Error>  -  Unsupported cmd(%d)\n",__FUNCTION__,__LINE__, cmd);
			break;
#endif
	}
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_DnpGetBufferStatus (DI_IPTUNER_t *pHTuner,HUINT32 nHiddenPumpBuf, HUINT32 nHiddenDecBuf,  DI_IPTUNER_Buffer_t *pStatus)
{
	DNP_OLT_t *pOLT = NULL;
	HUINT64 ullDownloadedBytes=0;
	HUINT32 uiDownloadedChunks=0;
	HINT32 iDownloadSpeed=0;
	HUINT64 ullLeftBytes=0;

	if(pHTuner==NULL)
	{
		PrintError ("%s (%d) Error> invalid IPTuner Handle \n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if(pStatus==NULL)
	{
		PrintError ("%s (%d) Error> pStatus is NULL pointer.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	/* Get OLT Handler */
	pOLT = (DNP_OLT_t*) pHTuner->pOLT;
	if(pOLT==NULL)
	{
		if(pHTuner->bIsProbeDone == TRUE)
		{
			pStatus->llBufferedBytes         =  0;
			pStatus->llTotalBytesRemaining   = (HINT64) pHTuner->ulDNPFileLength;
			pStatus->llTotalStreamBytes      = (HINT64) pHTuner->ulDNPFileLength;
			pStatus->nArrivalBytedPerSecond  =  0;
			pStatus->nStreamBytesPerSecond   = -1;
			pStatus->nBufferedMilliseconds   =  0;
			pStatus->uiBufferPercentage=0;
			pStatus->uiRemainBufferSec=0;
			PrintData ("%s (%d) Info> This tuner doesn't enter the buffering condition yet. But it was probed.!! \n",__FUNCTION__,__LINE__);
		}
		else
		{
			/* default values in youview system*/
			pStatus->llBufferedBytes         = 0;
			pStatus->llTotalBytesRemaining   = 0;
			pStatus->llTotalStreamBytes      = 0;
			pStatus->nArrivalBytedPerSecond  =  0;
			pStatus->nStreamBytesPerSecond   = -1;
			pStatus->nBufferedMilliseconds   =  0;
			pStatus->uiBufferPercentage=0;
			pStatus->uiRemainBufferSec=0;
			PrintData ("%s (%d) Info> This tuner does not enter Download status.!! \n",__FUNCTION__,__LINE__);
		}
	}
	else
	{
		if(pOLT->pTable == NULL)
		{
			PrintError("[%s][%d] pTable is NULL!\n", __FUNCTION__,__LINE__);
			return DI_ERR_ERROR;
		}

/*************************
*	1. Buffered Data in bytes & msec
*************************/
		pStatus->llBufferedBytes = DNP_OLT_GetBufferedDataSize(pOLT);
		pStatus->llBufferedBytes += nHiddenPumpBuf;
		pStatus->llBufferedBytes += nHiddenDecBuf;

		if(pStatus->llBufferedBytes && pHTuner->ulDNPFileLength && pHTuner->tMediaInfo.duration)
		{
			pStatus->nBufferedMilliseconds = pStatus->llBufferedBytes / (HINT64)(pHTuner->ulDNPFileLength / pHTuner->tMediaInfo.duration);
		}
		else
		{
			pStatus->nBufferedMilliseconds = 0;
		}

/*************************
*	2. TotalBytesRemaining
*************************/
		if(!pOLT->ulEndOffset)
		{
			/* sometimes nexus playback request invalid offset which is over than content length */
			if((HINT64)pOLT->ullDataReadOffset > pOLT->llTotalDataBytes)
				pOLT->ullDataReadOffset = pOLT->llTotalDataBytes;
			pStatus->llTotalBytesRemaining = pOLT->llTotalDataBytes - pOLT->ullDataReadOffset + nHiddenPumpBuf + nHiddenDecBuf;
		}
		else if (pOLT->ulEndOffset >= pOLT->ullDataReadOffset)
		{
			pStatus->llTotalBytesRemaining = pOLT->ulEndOffset - pOLT->ullDataReadOffset + nHiddenPumpBuf + nHiddenDecBuf;
		}
		else
		{
			pStatus->llTotalBytesRemaining = nHiddenPumpBuf + nHiddenDecBuf;
		}

/*************************
*	3. TotalStreamBytes
*************************/
		pStatus->llTotalStreamBytes = (HINT64)pOLT->ulEndOffset;

/*************************
*	4. Speed Parameters
*************************/
		pStatus->nArrivalBytedPerSecond = DNP_OLT_GetBufferingRate (pOLT);
		pStatus->nStreamBytesPerSecond  = -1; 		/* TODO : support progressive download only. -1 */

/*************************
*	5. Buffering Percentage
*************************/
		if(pHTuner->eUnderflowMode == DI_IPTUNER_UNDERFLOW_AUTO)
		{
			ullDownloadedBytes = DNP_OLT_GetDownloadedBytes(pOLT, pHTuner->ullPauseCheckOffset);
			uiDownloadedChunks = ullDownloadedBytes >> pOLT->nOLTShiftCount;
//			VK_printf("From %llu, [%u/%u] Chunks are downloaded\n", pHTuner->ullPauseCheckOffset>>pOLT->nOLTShiftCount, uiDownloadedChunks, pHTuner->uiChunksToBuffer);
			if(pHTuner->uiChunksToBuffer != 0)
			{
				if(uiDownloadedChunks < pHTuner->uiChunksToBuffer)
				{
					pStatus->uiBufferPercentage = uiDownloadedChunks * 10000 / pHTuner->uiChunksToBuffer;
				}
				else
				{
					pStatus->uiBufferPercentage = 10000;
				}
			}
			else
			{
				pStatus->uiBufferPercentage = 0;
			}

/*************************
*	6. Remain Buffering Sec
*************************/
			iDownloadSpeed = DNP_OLT_GetBufferingRate(pOLT);
			if(uiDownloadedChunks < pHTuner->uiChunksToBuffer)
			{
				ullLeftBytes = (pHTuner->uiChunksToBuffer - uiDownloadedChunks) << pOLT->nOLTShiftCount;
				if(iDownloadSpeed > 0)
				{
					pStatus->uiRemainBufferSec = ullLeftBytes / iDownloadSpeed;
				}
				else
				{
					pStatus->uiRemainBufferSec = 0;
				}
			}
			else
			{
				pStatus->uiRemainBufferSec = 0;
			}

//			VK_printf("[%u%%] uiRemainBufferSec: %u sec\n", pStatus->uiBufferPercentage, pStatus->uiRemainBufferSec);
		}
	}

#ifdef DI_IPTUNER_DNP_DEBUG
	PrintData("\n");
	PrintData("%s(%d) : ----------------------------------------------------\n",__FUNCTION__,__LINE__);
	PrintData("%s(%d) : BUFFERING STATUS Information \n",__FUNCTION__,__LINE__);
	if(pHTuner->eBufferingMode ==  DI_IPTUNER_MODE_THRESHOLD)
	{
		PrintData("%s(%d) : BUFFERING Mode [THRESHOLD]\n",__FUNCTION__,__LINE__);
		PrintData("%s(%d) : [T] nBufferThreshold          = %u msec\n",__FUNCTION__,__LINE__,pHTuner->nBufferThreshold);
		PrintData("%s(%d) : [T] bFastFillIsLimited        = %s \n",__FUNCTION__,__LINE__, pHTuner->bFastFillIsLimited?"TRUE":"FALSE");
		PrintData("%s(%d) : [T] dFastFillLimitMultiple    = %llf\n",__FUNCTION__,__LINE__,pHTuner->dFastFillLimitMultiple);
		PrintData("%s(%d) : [T] dManagedFillLimitMultiple = %llf\n",__FUNCTION__,__LINE__,pHTuner->dManagedFillLimitMultiple);
	}
	else if(pHTuner->eBufferingMode ==  DI_IPTUNER_MODE_FIXED)
	{
		PrintData("%s(%d) : BUFFERING Mode [FIXED] \n",__FUNCTION__,__LINE__);
		PrintData("%s(%d) : [F] nBufferThreshold          = %u msec\n",__FUNCTION__,__LINE__,pHTuner->nBufferThreshold);
	}
	else
	{
		PrintData("%s(%d) : BUFFERING Mode [UNLIMITED] \n",__FUNCTION__,__LINE__);
	}

	PrintData("%s(%d) : llBufferedBytes         =%"PRId64" bytes\n",__FUNCTION__,__LINE__,pStatus->llBufferedBytes      );
	PrintData("%s(%d) : internal Total          =%u bytes\n",__FUNCTION__,__LINE__, nHiddenPumpBuf+nHiddenDecBuf);
	PrintData("%s(%d) :      playpump           =%u bytes\n",__FUNCTION__,__LINE__, nHiddenPumpBuf);
	PrintData("%s(%d) :      v/a dec            =%u bytes\n",__FUNCTION__,__LINE__, nHiddenDecBuf);
	PrintData("%s(%d) : llTotalBytesRemaining   =%"PRId64" bytes\n",__FUNCTION__,__LINE__,pStatus->llTotalBytesRemaining);
	PrintData("%s(%d) : llTotalStreamBytes      =%"PRId64" bytes\n",__FUNCTION__,__LINE__,pStatus->llTotalStreamBytes   );
	PrintData("%s(%d) : nArrivalBytedPerSecond  =%d bytes\n"       ,__FUNCTION__,__LINE__,pStatus->nArrivalBytedPerSecond);
	PrintData("%s(%d) : nStreamBytesPerSecond   =%d bytes\n"       ,__FUNCTION__,__LINE__,pStatus->nStreamBytesPerSecond );
	PrintData("%s(%d) : nBufferedMilliseconds   =%d millisecs\n"   ,__FUNCTION__,__LINE__,pStatus->nBufferedMilliseconds );
	PrintData("%s(%d) : ----------------------------------------------------",__FUNCTION__,__LINE__);
	PrintData("\n");
#endif

	return DI_ERR_OK;
}

HBOOL DI_IPTUNER_DnpSetOLTEndTime (DI_IPTUNER_t *pHTuner, HUINT64 ulOffset)
{
	PrintDebug("%s(%d) Set EndOffset in OLT Table (%llu) bytes\n",__FUNCTION__,__LINE__,ulOffset);

	return DNP_OLT_SetEndOffset (pHTuner->pOLT, ulOffset);
}

HBOOL DI_IPTUNER_DnpCheckBufferRegion (DI_IPTUNER_t *pHTuner, HUINT64 ullStart, HUINT64 ullEnd)
{
	return  DNP_OLT_IsFilled (pHTuner->pOLT, (HINT64)ullStart, (HINT64)(ullEnd-ullStart));
}

DI_ERR_CODE DI_IPTUNER_DnpCancelBlockedRead  (DI_IPTUNER_t *pHTuner)
{

	pHTuner->bExitRead = TRUE;
	pHTuner->bExitIndexRead = TRUE;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_DnpUpdateBufferingPosition(DI_IPTUNER_t *pHTuner, HINT64 llOffset)
{
	DRV_Error dResult;

	dResult = P_IPTUNER_DnpUpdateBufferingPosition(pHTuner, llOffset);
	if(dResult != DRV_OK)
	{
		PrintError("P_IPTUNER_DnpUpdateBufferingPosition failed!\n");
		return DI_ERR_ERROR;
	}
	return DI_ERR_OK;
}

void DI_IPTUNER_DnpRegisterStartTimeMsec (DI_IPTUNER_t *pHTuner)
{
	PrintEnter;
	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> invalid tuner handle (NULL)\n",__FUNCTION__,__LINE__);
		return;
	}

	if (pHTuner->pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid OLT Handle (NULL)\n",__FUNCTION__,__LINE__);
		return;
	}

	(void) DNP_OLT_RegisterStartTimeMsec (pHTuner->pOLT);

	PrintExit;
	return;
}

void DI_IPTUNER_DnpUnRegisterStartTimeMsec (DI_IPTUNER_t *pHTuner)
{
	PrintEnter;
	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> invalid tuner handle (NULL)\n",__FUNCTION__,__LINE__);
		return;
	}

	if (pHTuner->pOLT == NULL)
	{
		PrintError ("%s(%d) Error> invalid OLT Handle (NULL)\n",__FUNCTION__,__LINE__);
		return;
	}

	(void) DNP_OLT_UnRegisterStartTimeMsec (pHTuner->pOLT);

	PrintExit;
	return;
}

DI_ERR_CODE DI_IPTUNER_DnpGetBufferInfo(DI_IPTUNER_t *pHTuner, HUINT32 *uiChunks, HINT32 **iStartOffsets, HINT32 **iEndOffsets)
{
	DI_ERR_CODE dResult;
	dResult = P_IPTUNER_DnpGetBufferedInfo(pHTuner, uiChunks, iStartOffsets, iEndOffsets);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_IPTUNER_DnpGetBufferedInfo failed!\n");
		return DI_ERR_ERROR;
	}
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_DnpConvertTimeToByteOffset(DI_IPTUNER_t *pHTuner, HUINT32 ulTime , HUINT64 *pEndByte)
{
	DI_ERR_CODE dResult;

	dResult = P_IPTUNER_DnpConvertTimeToByteOffset(pHTuner, ulTime, pEndByte);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_IPTUNER_DnpConvertTimeToByteOffset failed!\n");
		return DI_ERR_ERROR;
	}
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_DnpSetCurrentPlayTime(DI_IPTUNER_t *pHTuner, HUINT32 uiTime)
{
	HUINT64 ullOffset=0;
	DI_ERR_CODE dResult;

	if(pHTuner == NULL)
	{
		PrintError("Invalid parameter! pHTuner is NULL!\n");
		return DI_ERR_ERROR;
	}

	dResult = P_IPTUNER_DnpConvertTimeToByteOffset(pHTuner, uiTime, &ullOffset);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_IPTUNER_DnpConvertTimeToByteOffset failed!\n");
		return DI_ERR_ERROR;
	}
	pHTuner->ullPauseCheckOffset = ullOffset;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_DnpCheckUnderrun(DI_IPTUNER_t *pHTuner)
{
	if(pHTuner == NULL)
	{
		PrintError("Invalid parameter! pHTuner is NULL!\n");
		return DI_ERR_ERROR;
	}

	pHTuner->uiPauseGauge = 0;
	pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_RESUME;
	return DI_ERR_OK;
}
#if 0 /* not ready */
#define MAX_MMS_RECV_BUFF_SIZE (4096*3)	/** 6KB */

int P_DNP_STREAM_MMS_DnpReceive (DI_IPTUNER_t *pHTuner)
{
	HINT8 *pBuffer =NULL;
	HINT32 nRecvLength=0;
	HINT64 llContentLength ;
	HINT32 flags = O_CREAT|O_TRUNC|O_WRONLY;
	HINT32 nRc = -1;
	STREAM_ACCESS_t * pMmsAccess = NULL;
#if 1 /* for CANVAS : dragon */
	int saveFd = -1;
	HINT64 llSumLength=0;
#endif

	PrintEnter;

	VFIO_Handle_t *pVfio = (VFIO_Handle_t *)pHTuner->pVfioHandle;
	if(pVfio == NULL)
	{
		PrintError("Error > %s(%d) Vfio Handle is NULL \n", __FUNCTION__, __LINE__);
		return -1;
	}

	if(pHTuner->pSaveFileName == NULL)
	{
		PrintError("Error > %s(%d) SaveFileName is NULL \n", __FUNCTION__, __LINE__);
		return -1;
	}

	pMmsAccess = (STREAM_ACCESS_t *)pVfio->pDnpUserData;
	if(pMmsAccess == NULL)
	{
		PrintError("pMmsAccess is NULL\n");
		return -1;
	}

	llContentLength =pMmsAccess->info.i_size;

	/* 1. Open DNP FilePath */
	saveFd = open (pHTuner->pSaveFileName, flags, 0666);
	if(saveFd<0)
	{
		PrintError ("%s(%d) :  Error> %s open fail (MMS_DNP) \n",__FUNCTION__,__LINE__,pHTuner->pSaveFileName);
		return -1;
	}

	PrintDebug ("%s(%d): Open %s.\n",__FUNCTION__,__LINE__,pHTuner->pSaveFileName);
	PrintDebug ("%s(%d): llContentLength (%"PRId64")bytes \n",__FUNCTION__,__LINE__,llContentLength);

	/* heap allocation */
	pBuffer = (HINT8*) DD_MEM_Alloc (MAX_MMS_RECV_BUFF_SIZE);
	if(pBuffer==NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,MAX_MMS_RECV_BUFF_SIZE);
		goto exit_stop;
	}
	pHTuner->ulDNPFileLength = llContentLength;

	do
	{
		if ( TRUE == pHTuner->bExitDownload)
		{
			PrintDebug("%s(%d) : User Stoped Receiving !!\n",__FUNCTION__,__LINE__);
			goto exit_stop;
		}

		if ( TRUE == pHTuner->bPauseDownload)
		{
			PrintDebug("%s(%d) : User Paused Receiving !!\n",__FUNCTION__,__LINE__);
			VK_TASK_Sleep(100);
			continue;
		}

		nRecvLength = 0;
		VK_memset  (pBuffer,0,MAX_MMS_RECV_BUFF_SIZE);

		/*XXX: Error check */
		(void) DI_STREAM_MMS_Read (*pVfio, pBuffer, MAX_MMS_RECV_BUFF_SIZE, 0, (size_t *)&nRecvLength);

#if 0
		PrintData ("%s (%d) : total(%"PRId64") recv(%d)\n",__FUNCTION__,__LINE__,llContentLength, nRecvLength);
#endif

		if(nRecvLength <= 0)
		{
#if 0
			if (pMmsAccess->info.b_eof == TRUE )
			{
				DI_IPTUNER_DnpSendEvent();

			}
			/* check network status */
			if (TRUE == DI_IPTUNER_GetNetworkStatus())
			{
				PrintError ("%s(%d) : Error> Socket Error in Network Device \n",__FUNCTION__,__LINE__);
				return -1;
			}
#endif
#if 0 /* for CANVAS */
			pDnpUserData->bDNPNetworkError = TRUE;
#endif
			continue;
		}
		/* normal case */
		else
		{
			nRc = write (saveFd, pBuffer, nRecvLength);
			if(nRc<0)
			{
				PrintError ("%s(%d) : Error > Write Error Occured !! \n",__FUNCTION__,__LINE__);
			}

			llSumLength += nRecvLength;
			pHTuner->stBufferStatus.nBufferedBytes += nRecvLength;
#if 0 /* CANVAS TODO */
			/* Send Event for Probing (64Kbytes) */
			if ( (s_nIsEventSend == 0) && (llSumLength > (2048*64)) ) //(1024*64)) )
			{
				s_nIsEventSend = 1;
				DI_IPTUNER_DnpSendEvent ();
			}
			/* else { continue; } */

			PrintData ("%s (%d) : sum(%"PRId64") remained(%"PRId64")\n",__FUNCTION__,__LINE__,llSumLength, llContentLength-llSumLength);
#endif
			usleep(0);
		}
	} while ( llSumLength < llContentLength );

exit_stop:
	if(pBuffer != NULL)
	{
		DD_MEM_Free (pBuffer);
		pBuffer = NULL;
	}
	close(saveFd);
	saveFd = -1;

	PrintExit;
	return 0;
}
#endif
