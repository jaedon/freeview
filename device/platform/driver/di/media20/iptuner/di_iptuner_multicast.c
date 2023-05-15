/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_multicast.c  $
 * Version:		2.0 (for Media 2.0 with 40nm)
 * Original Author:	Jin Lee
 * Current Author:	$Author: jlee@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     MULTICAST - VFIO Controller
 * Module:
 * Remarks:
 */

/**
 * @defgroup MULTICAST_IPTUNER_CONTROLLER
 * @brief MULTICAST IPTuner Controller and non-VFIO based
 */

/**
 * @author Jin Lee (jlee@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @brief MULTICAST VFIO Controller.
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file di_iptuner_multicast.c
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
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_media20.h"
#include "di_uart.h"
#include "di_iptuner20.h"
#include "di_iptuner_config.h"
#include "di_iptuner_multicast.h"
#include "stream_global.h"
#include "stream_multicast.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define DI_IPTUNER_MULTICAST_DEBUG

//#ifdef IPTUNER_ALL_DEBUG
#if 1
	#ifdef DI_IPTUNER_MULTICAST_DEBUG
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

#define UNUSED_PARAM(x)  ((void)x)

#if 0
  #define __MULTICAST_DEBUG_SAVE__
#else
  #ifdef __MULTICAST_DEBUG_SAVE__
  #undef __MULTICAST_DEBUG_SAVE__
  #endif
#endif

#if 0
  #define __MULTICAST_CADRM_DECRYPT__ /* Set if CA/DRM decryption here */
#endif

#if 0
#define _DEF_MULTICAST_TASK_PRIORITY   (10)
#else
#define	_DEF_MULTICAST_TASK_PRIORITY   AUDIO_CLIP_TASK_PRIORITY
#endif
#define	_DEF_MULTICAST_STACK_SIZE      (256*1024)	/* 256Kbytes */
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

#if __________task__________
#endif

#if __________drv__________
#endif

#if __________di__________
#endif

static void P_IPTUNER_SendTuningEvent(DI_IPTUNER_t *pHTuner,
				DI_MEDIA_ChannelTuningEvent_e eTuningEvent)
{
	PrintEnter;
	PrintDebug("%s(%d) [tuning_event:%d]\n", __FUNCTION__, __LINE__, eTuningEvent);

	if (pHTuner == NULL)
	{
		PrintError("%s(%d) Error> Invalid IPTUNER Handle!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	if(pHTuner->pChannelCallbacks == NULL)
	{
		PrintError("%s(%d) Error> Unregistered channel callback!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	if(pHTuner->pChannelCallbacks->cbTuningEvent == NULL)
	{
		PrintError("%s(%d) Error> Unregistered channel tuning callback!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	pHTuner->pChannelCallbacks->cbTuningEvent((HUINT32)pHTuner, eTuningEvent);

func_error:
	PrintExit;
	return;
}

#if 0 /* defined but not used */
static void P_IPTUNER_SendNotifyEvent(DI_IPTUNER_t *pHTuner,
				DI_MEDIA_ChannelNotifyEvent_e eNotifyEvent)
{
	PrintEnter;
	PrintDebug("%s(%d) [notify_event:%d]\n", __FUNCTION__, __LINE__, eNotifyEvent);

	if (pHTuner == NULL)
	{
		PrintError("%s(%d) Error> Invalid IPTUNER Handle!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	if(pHTuner->pChannelCallbacks == NULL)
	{
		PrintError("%s(%d) Error> Unregistered channel callback!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	if(pHTuner->pChannelCallbacks->cbNotifyEvent == NULL)
	{
		PrintError("%s(%d) Error> Unregistered channel notify callback!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	pHTuner->pChannelCallbacks->cbNotifyEvent((HUINT32)pHTuner, eNotifyEvent);

func_error:
	PrintExit;
	return;
}
#endif

#ifdef __CONTINUOUS_CNT_CHK__
static void P_IPTUNER_MULTICAST_CheckContinuityCnt (MULTICAST_ACCESS_SYS_t *pMulticastSys,
							HUINT8 *pucPacket,
							HUINT32 unLen,
							HUINT8 ucPidHi,
							HUINT8 ucPidLow)
{
	HUINT8  *pucTsbuf = NULL;
	HUINT32 unSyncBytePos = 0;
	HUINT32 i = 0;

	if (pMulticastSys == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL pMulticastSys\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	while(unSyncBytePos + TS_PACKET_SIZE <= unLen)
	{
		if(pucPacket[unSyncBytePos] == TS_SYNC_BYTE)
		{
			pucTsbuf = &pucPacket[unSyncBytePos];
			/* set PID */
			/*if (((pucTsbuf[1] & 0x0F) == 0x03) && (pucTsbuf[2] == 0xE9))  */
			/*if (((pucTsbuf[1] & 0x0F) == 0x00) && (pucTsbuf[2] == 0x21)) */
			/*if (((pucTsbuf[1] & 0x0F) == 0x03) && (pucTsbuf[2] == 0xEE))  */
			if (((pucTsbuf[1] & 0x0F) == ucPidHi)
				&& (pucTsbuf[2] == ucPidLow))
			{
				if((pucTsbuf[i+2+1] & 0x30) != 0x20)
				{
					if (pMulticastSys->ucTSCount == 0x0F)
					{
						if ((0x0F & pucTsbuf[i+2+1]) != 0x00)
						{
							pMulticastSys->uTSErrorCount++;
							PrintError("[%02x%02x] error !! - last[%02x], current[%02x] -- [%u]\n",
										pucTsbuf[1], pucTsbuf[2],
										pMulticastSys->ucTSCount,
										(0x0F & pucTsbuf[i+2+1]), pMulticastSys->uTSErrorCount);
						}
						/* else - OK */
					}
					else
					{
						if ((pMulticastSys->ucTSCount+1) != (0x0F & pucTsbuf[i+2+1]))
						{
							pMulticastSys->uTSErrorCount++;
							PrintError("[%02x%02x] error !! - last[%02x], current[%02x] -- [%u]\n",
										pucTsbuf[1], pucTsbuf[2],
										pMulticastSys->ucTSCount,
										(0x0F & pucTsbuf[i+2+1]),
										pMulticastSys->uTSErrorCount);
						}
						/* else - OK */
					}
				}
				pMulticastSys->ucTSCount = (0x0F & pucTsbuf[i + 2 + 1]);
			}
			unSyncBytePos += TS_PACKET_SIZE;
		}
		else
		{
			PrintError("%s(%d) Error> ts sync char\n", __FUNCTION__, __LINE__);
			unSyncBytePos++;
		}
	}
func_error:
	return;
}
#endif /* __CONTINUOUS_CNT_CHK__ */

static void P_IPTUNER_MULTICAST_ReceiveThread(void *pParam)
{
	MULTICAST_ACCESS_SYS_t *pstMulticastSys = NULL;
	DI_IPTUNER_t *pHTuner = NULL;
	DI_IPTUNER_TSInjectorCallback_t *pTSInjectorCallbacks = NULL;
	HUINT8 *pBuffer = NULL;
	size_t iReqLen = 0; /* (ssize_t) is inappropriate because of small packet size */
  	size_t iRecvLen = 0;
#if defined(__MULTICAST_CADRM_DECRYPT__)
	unsigned long ulDrmErr = 0;
#endif

	/* Even if there was socket or stream error,
	   we have to continue looping and wait stop() call by M/W */
	HBOOL bWaitStreamError = TRUE;

	/* But, send "UNLOCKED" event callback for long time stream error */
	struct timeval tvStartTime  = {0,0};
	struct timeval tvCurrentTime  = {0,0};
	HUINT32 uStreamInterval = 0; /* normal interval between prev recv() and current recv() */
	HUINT32 uErrorInterval = 0; /* stream error interval time (usec) */

	PrintEnter;

	if (pParam == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL params\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	/* (void)memcpy(&pstMulticastSys, pParam, sizeof(MULTICAST_ACCESS_SYS_t)); */
	pstMulticastSys = (MULTICAST_ACCESS_SYS_t *) pParam;
	pHTuner = pstMulticastSys->pTuner;
	if (pHTuner == NULL)
	{
		PrintDebug("%s(%d) : (Tuner handle is null)!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (pstMulticastSys->bThreadRunning == FALSE)
	{
		pstMulticastSys->bThreadRunning = TRUE;
	}

	pTSInjectorCallbacks = pHTuner->pTSInjectorCallbacks;
	if (pTSInjectorCallbacks == NULL)
	{
		PrintError("%s(%d) Error> Uninstalled injection callbacks!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	if (pTSInjectorCallbacks->cbDataReceived == NULL)
	{
		PrintError("%s(%d) Error> Uninstalled injection receive callback!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	/* Change buffer size which it has MTU packet size */
	/* Use enough buffer size about xx (Kbytes) */
	if (!pstMulticastSys->iReadSize)
	{
		pstMulticastSys->iReadSize = MULTICAST_BUFFER_SIZE;
	}

	//	lock( pstMulticastSys );
	pstMulticastSys->bThreadQuit = FALSE;
	while (TRUE != pstMulticastSys->bThreadQuit)
	{
#if 0
		VK_memset(pstMulticastSys->pReadBuffer, 0, MULTICAST_BUFFER_SIZE);
		pBuffer = pstMulticastSys->pReadBuffer;
#else
		VK_memset(pstMulticastSys->readBuffer, 0, MULTICAST_BUFFER_SIZE);
		pBuffer = pstMulticastSys->readBuffer;
#endif

		if (pstMulticastSys->iReadSize)
		{
			iReqLen = pstMulticastSys->iReadSize;
		}
		else
		{
#if 1
			iReqLen = _MTU_PKT_SIZE;
#else
			iReqLen = _RTP_PKT_SIZE;
#endif
		}

		/* Record recv() time */
		UTIL_GetCurrentTimeval(&tvStartTime);
		UTIL_GetCurrentTimeval(&tvCurrentTime);


		/* Receive function will be blocked
		   until multicast thread doesn't return abnormal error or user stop event */
		PrintDebug("%s(%d) --> +STREAM_MULTICAST_Read(req:%d)\n", __FUNCTION__, __LINE__, iReqLen);
		iRecvLen = STREAM_MULTICAST_Read(pstMulticastSys, pBuffer, iReqLen);
		PrintDebug("%s(%d) --> -STREAM_MULTICAST_Read(recv:%d)\n", __FUNCTION__, __LINE__, iRecvLen);
		if (iRecvLen > 0)
		{
			pstMulticastSys->tvPrevRecvErrorTime.tv_usec = 0;
			pstMulticastSys->tvFirstRecvErrorTime.tv_usec = 0;
			pstMulticastSys->bRecvErrorOccurred = FALSE;

			UTIL_GetCurrentTimeval(&tvCurrentTime);
			uStreamInterval = UTIL_GetTimevalDiff(&tvCurrentTime, &tvStartTime);

#ifdef __CONTINUOUS_CNT_CHK__
			P_IPTUNER_MULTICAST_CheckContinuityCnt(pstMulticastSys,
						pBuffer, iRecvLen,
						0x00, 0x21);	/* hdforum.mpg */ /* PidHi, PidLow */
#endif

			/* if CA/DRM decrypt is processed here... */
			/* or process this at di_media:TSIjector  */
#if defined(__MULTICAST_CADRM_DECRYPT__)
			ulDrmErr = SOME_CADRM_Decrypt((unsigned char *)pBuffer, iRecvLen);
			if (ulDrmErr != 0)
			{
				// CA/DRM error...
				switch(ulDrmErr) {
				case SOME_CADRM_ERR_INCORRECT_TS_PACKET_SIZE :
					break;
				case SOME_CADRM_ERR_INCORRECT_TS_PACKET :
					break;
				case SOME_CADRM_ERR_SCRAMBLED_TS_IN_BUFF :
					break;
				default:
					break;
				}
			}
#endif

			/* Callback to TSInjector in di_media */
			if (pTSInjectorCallbacks->cbDataReceived != NULL)
			{
				//pTSInjectorCallbacks->cbDataReceived(pHTuner->uTSInjID, pBuffer, iRecvLen, (void *)pHTuner);
				pTSInjectorCallbacks->cbDataReceived(pHTuner->uTSInjID, pBuffer, iRecvLen);
			}

#if defined(__MULTICAST_DEBUG_SAVE__)
			if ((pstMulticastSys->bDbgSave == TRUE)
				&& (pstMulticastSys->fpDbgSave != NULL))
			{
				PrintDebug("%s(%d) --> +fwrite(recv:%d)\n", __FUNCTION__, __LINE__, iRecvLen);
				fwrite(pBuffer, 1, iRecvLen, pstMulticastSys->fpDbgSave);
				PrintDebug("%s(%d) --> -fwrite(recv:%d)\n", __FUNCTION__, __LINE__, iRecvLen);
			}
#endif
		}
		else
		{
			PrintError("%s(%d) Error> Cant get stream buffer(nrecv:%d)!\n", __FUNCTION__, __LINE__, iRecvLen);
			/* Dont exit and wait streaming; Just send notify event to M/W */

			/* Check whether this stream error could be process with "LOCKED" event */
			if (pstMulticastSys->tvPrevRecvErrorTime.tv_usec == 0)
			{
				/* first time */
				pstMulticastSys->tvFirstRecvErrorTime = tvCurrentTime;
				pstMulticastSys->tvPrevRecvErrorTime = tvCurrentTime;
			}
			else
			{
#if 0
				uErrorInterval = UTIL_GetTimevalDiff(&tvCurrentTime, &pstMulticastSys->tvPrevRecvErrorTime);
#else
				uErrorInterval = UTIL_GetTimevalDiff(&tvCurrentTime, &pstMulticastSys->tvFirstRecvErrorTime);
#endif
				PrintError("%s(%d) Error> stream error interval:%u\n", __FUNCTION__, __LINE__, uErrorInterval);

				if (uErrorInterval >= 3000)
				{
					//if (pMulticastSys->bAVStream == TRUE)
					//{
						pstMulticastSys->bRecvErrorOccurred = TRUE;

						PrintDebug("%s(%d) Error> stream timeout --> +SendTuningEvent(UNLOCKED), errorInterval:%d\n", __FUNCTION__, __LINE__, uErrorInterval);
						P_IPTUNER_SendTuningEvent(pHTuner, DI_MEDIA_CHANNEL_TUNING_EVENT_UNLOCKED);
						PrintDebug("%s(%d) Error> stream timeout --> -SendTuningEvent(UNLOCKED)\n", __FUNCTION__, __LINE__);

#if 0
						goto func_error;
#endif
					//}
				}
				pstMulticastSys->tvPrevRecvErrorTime = tvCurrentTime;
			}

			/* Even if there was socket or stream error,
			   we have to continue looping and wait stop() call by M/W */
			if (bWaitStreamError == TRUE)
			{
				/* Retry again;
				   Is enough or too large sleep time..? */
				VK_TASK_Sleep(10);
				continue;
			}
			else
			{
				goto func_error;
			}
		}
	}
	pstMulticastSys->bThreadQuit = FALSE;
	pstMulticastSys->bThreadRunning = FALSE;
	//unlock(pstMulticastSys );
	PrintExit;
	return;

func_error:
	if (pstMulticastSys != NULL)
	{
		pstMulticastSys->bThreadQuit = FALSE;
		pstMulticastSys->bThreadRunning = FALSE;
		//unlock ( pstMulticastSys );
	}
	PrintExit;
	return;
}


#if __________di__________
#endif
/*******************************************************************/
/************************ public funtions **************************/
/*******************************************************************/

/*****************************************************************************
 * DI_IPTUNER_MulticastOpen:
 *   - Allocate/Initialize pAccess
 *   - Start packet receive thread
 *   - IGMP join
 *****************************************************************************/
DI_ERR_CODE DI_IPTUNER_MulticastOpen (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	HINT32 nVKRet = 0;
	HINT32 nIptRet = DI_IPT_EGENERIC;
	HBOOL bValidateStreamAccess = TRUE;
	HBOOL bValidateMulticastSys = TRUE;
	STREAM_ACCESS_t * pAccess = NULL;
	MULTICAST_ACCESS_SYS_t *pstMulticastSys = NULL;
#ifdef __MULTICAST_DEBUG_SAVE__
	/* const HINT8 szDbgPath[] = "/mnt/nfs/multicast_stream.mpg"; */
	HINT8 szDbgPath[256];
#endif

	PrintEnter;

	if (pHTuner == NULL)
	{
		PrintError("%s(%d) Error> Invalid IPTUNER Handle!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	PrintDebug("%s(%d) --> +SendTuningEvent(TUNED)\n", __FUNCTION__, __LINE__);
	P_IPTUNER_SendTuningEvent(pHTuner, DI_MEDIA_CHANNEL_TUNING_EVENT_TUNED);
	PrintDebug("%s(%d) --> -SendTuningEvent(TUNED)\n", __FUNCTION__, __LINE__);

	pAccess = DD_MEM_Alloc(sizeof (STREAM_ACCESS_t));
	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto func_error;
	}
	VK_memset (pAccess, 0, sizeof (STREAM_ACCESS_t));

	/*
	 * We must store a STREAM Handler in iptuner handle .
	 * If not so, we can't know correct stream handle in vfio function set.(VFIO_Read/VFIO_xxx etc)
	 *
	 * iptuner_handler --  VFIO Handler (include file and socket descriptor)
	 *                 --  STREAM Handler ( include protocol controller)
	 */
	//ptHandle->pTunerHandle = (void*) pHTuner;
	pHTuner->pHStream = (void*) pAccess;

	/* psz_path must be free */
	pAccess->psz_path = VK_StrDup (pHTuner->pUrl);
	if (pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto func_error;
	}
	pAccess->b_openfromnexus = FALSE;

	/*
	 * 1. Initialize STREAM_ACCESS_t handle
	 * 2. Allocate/Initialize MULTICAST_ACCESS_SYS_t handle
	 * 3. IGMP Join
	 */
	nIptRet = STREAM_MULTICAST_Open (pAccess);
	if (nIptRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) Error> STREAM_MULTICAST_Open failed!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto func_error;
	}
	PrintDebug("%s(%d) --> +SendTuningEvent(LOCKED)\n", __FUNCTION__, __LINE__);
	P_IPTUNER_SendTuningEvent(pHTuner, DI_MEDIA_CHANNEL_TUNING_EVENT_LOCKED);
	PrintDebug("%s(%d) --> -SendTuningEvent(LOCKED)\n", __FUNCTION__, __LINE__);

	if (bValidateStreamAccess == TRUE)
	{
		if ((pAccess->proto != PROTOCOL_UDP) // PROTOCOL_UDP_MCAST
			&& (pAccess->proto != PROTOCOL_RTP)) // PROTOCOL_RTP_MCAST
		{
			PrintError("%s(%d) Error> Unsupported access protocol(%d)!\n", __FUNCTION__, __LINE__, pAccess->proto);
			eDiErr = DI_ERR_ERROR;
			goto func_error;
		}
	}

	pstMulticastSys = (MULTICAST_ACCESS_SYS_t *)pAccess->p_sys;
	if (bValidateMulticastSys == TRUE)
	{
		if (pstMulticastSys == NULL)
		{
			PrintError("%s(%d) Error> Invalid multicast stream handle!\n", __FUNCTION__, __LINE__);
			eDiErr = DI_ERR_ERROR;
			goto func_error;
		}
		if ((pstMulticastSys->eStreamingType !=  MULTICAST_STREAMING_RTP)
			&& (pstMulticastSys->eStreamingType !=  MULTICAST_STREAMING_UDP))
		{
			PrintError("%s(%d) Error> Unsupported streaming proto(%d)!\n",
						__FUNCTION__, __LINE__,
						pstMulticastSys->eStreamingType);
			eDiErr = DI_ERR_ERROR;
			goto func_error;
		}
	}

#ifdef __MULTICAST_DEBUG_SAVE__
	pstMulticastSys->bDbgSave = TRUE;

	if (pstMulticastSys->bDbgSave == TRUE)
	{
		VK_memset(szDbgPath, 0, 256);
		VK_sprintf(szDbgPath, "/mnt/nfs/_di_iptuner_multicast_stream_%s_%d.ts", pstMulticastSys->tUrl.psz_host, pstMulticastSys->tUrl.i_port);

		pstMulticastSys->pDbgSavePath = (HUINT8*)VK_StrDup(szDbgPath);
		if (pstMulticastSys->pDbgSavePath == NULL)
		{
			PrintError("%s(%d) Error> Cant open debug path(%s)\n",__FUNCTION__,__LINE__, szDbgPath);
			pstMulticastSys->fpDbgSave = NULL;
		}
		else
		{
			PrintError("%s(%d) Open debug path(%s)\n",__FUNCTION__,__LINE__, szDbgPath);
			pstMulticastSys->fpDbgSave = fopen((char*)pstMulticastSys->pDbgSavePath, "wb");
			if (pstMulticastSys->fpDbgSave == NULL)
			{
				PrintError("%s(%d) Error> Cant open save debug stream(%s)!\n",__FUNCTION__,__LINE__, szDbgPath);
				if (pstMulticastSys->pDbgSavePath != NULL)
				{
					VK_MEM_Free(pstMulticastSys->pDbgSavePath);
					pstMulticastSys->pDbgSavePath = NULL;
				}
			}
		}
	}
#endif

	pHTuner->bExitRead = FALSE;
	if (pHTuner->eChannelType == DI_MEDIA_CHANNEL_TUNER_TYPE_SI)
	{
		pstMulticastSys->bAVStream = FALSE;
	}
	else
	{
		pstMulticastSys->bAVStream = TRUE;
	}

	pstMulticastSys->pTuner = pHTuner;
	pstMulticastSys->bThreadError = FALSE;
	pstMulticastSys->bThreadQuit = FALSE;
	pstMulticastSys->bThreadRunning = FALSE;
	nVKRet = VK_TASK_Create(P_IPTUNER_MULTICAST_ReceiveThread,
					_DEF_MULTICAST_TASK_PRIORITY,
					_DEF_MULTICAST_STACK_SIZE,
					"MULTICAST_PacketReceiveTask",
					(void *)pstMulticastSys,
					&pstMulticastSys->iThreadID,
#if 1
					0); /* force thread detaching */
#else
					1); /* joinable */
#endif
	if (VK_OK != nVKRet)
	{
		PrintError("%s(%d) Error> ReceiveThread task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		pstMulticastSys->bThreadError = TRUE;
		pstMulticastSys->bThreadRunning = FALSE;
		return DI_ERR_ERROR;
	}

	/* Start Multicast Receive Task */
	(void)VK_TASK_Start(pstMulticastSys->iThreadID);
	if (pstMulticastSys->bThreadRunning == FALSE)
	{
		pstMulticastSys->bThreadRunning = TRUE;
	}

	PrintExit;
	return DI_ERR_OK;

func_error:
	PrintError("%s(%d) Error> ReceiveThread task failed\n",__FUNCTION__,__LINE__);

	if (pHTuner != NULL)
	{
		PrintDebug("%s(%d) --> +SendTuningEvent(UNLOCKED)\n", __FUNCTION__, __LINE__);
		P_IPTUNER_SendTuningEvent(pHTuner, DI_MEDIA_CHANNEL_TUNING_EVENT_UNLOCKED);
		PrintDebug("%s(%d) --> -SendTuningEvent(UNLOCKED)\n", __FUNCTION__, __LINE__);
	}

	if (pAccess != NULL)
	{
		if (pAccess->psz_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}

		if (pAccess->p_sys != NULL)
		{
			DD_MEM_Free(pAccess->p_sys);
			pAccess->p_sys = NULL;
		}

		DD_MEM_Free(pAccess);
		pAccess = NULL;
	}
	PrintExit;
	return eDiErr;
}

/*****************************************************************************
 * DI_IPTUNER_MulticastClose:
 *   - Deallocate pAccess
 *   - Stop packet receive thread
 *   - IGMP leave
 *****************************************************************************/
DI_ERR_CODE DI_IPTUNER_MulticastClose (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	MULTICAST_ACCESS_SYS_t *pstMulticastSys =NULL;
	STREAM_ACCESS_t * pAccess = NULL;
	HUINT32 uMaxLoopCnt = 20;
	HUINT32 uLoopCnt = 0;

	PrintEnter;

	if (NULL == pHTuner)
	{
		PrintError("%s(%d) Error> invalid tuner handle (NULL)\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_INVALID_PARAM;
		goto func_error;
	}

	if ((pHTuner->eProtocolType != DI_IPTUNER_PROTOCOL_UDP_MCAST)
		&& (pHTuner->eProtocolType != DI_IPTUNER_PROTOCOL_RTP_MCAST))
	{
		PrintError("%s(%d) Error> Unsupported protocol(%d)\n", __FUNCTION__, __LINE__, pHTuner->eProtocolType);
		eDiErr = DI_ERR_INVALID_PARAM;
		goto func_error;
	}

	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> pAccess is NULL\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto func_error;
	}

	pstMulticastSys = (MULTICAST_ACCESS_SYS_t *)pAccess->p_sys;
	if (pstMulticastSys == NULL)
	{
		PrintError("%s(%d) Error> MulticastSys is NULL\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
		goto func_error;
	}

	/* instead task join, spin wait */
	if (pstMulticastSys->bThreadQuit == TRUE)
	{
		while (pstMulticastSys->bThreadQuit == FALSE)
		{
			uLoopCnt++;
			if (uMaxLoopCnt && (uLoopCnt > uMaxLoopCnt))
			{
				pstMulticastSys->bThreadQuit = TRUE;
				break;
			}
			VK_TASK_Sleep(10);
		}
	}
	pstMulticastSys->bThreadQuit = TRUE;

	/* VK_TASK_Join may induce bus error. let destroy handle pthread join */
	VK_TASK_Destroy(pstMulticastSys->iThreadID);

#ifdef __MULTICAST_DEBUG_SAVE__
	if (pstMulticastSys->bDbgSave == TRUE)
	{
		if (pstMulticastSys->fpDbgSave != NULL)
		{
			fclose(pstMulticastSys->fpDbgSave);
			pstMulticastSys->fpDbgSave = NULL;
		}

		if (pstMulticastSys->pDbgSavePath != NULL)
		{
			VK_MEM_Free(pstMulticastSys->pDbgSavePath);
			pstMulticastSys->pDbgSavePath = NULL;
		}
	}
#endif

	/* 1. Destroy MULTICAST_ACCESS_SYS_t */
	/* 2. IGMP Leave */
	STREAM_MULTICAST_Close(pAccess);

	PrintDebug("%s(%d) --> +SendTuningEvent(UNLOCKED)\n", __FUNCTION__, __LINE__);
	P_IPTUNER_SendTuningEvent(pHTuner, DI_MEDIA_CHANNEL_TUNING_EVENT_UNLOCKED);
	PrintDebug("%s(%d) --> -SendTuningEvent(UNLOCKED)\n", __FUNCTION__, __LINE__);

	pAccess->p_sys = NULL;
	if (pAccess->psz_path != NULL)
	{
		DD_MEM_Free(pAccess->psz_path);
		pAccess->psz_path = NULL;
	}

	if (pHTuner->pHStream != NULL)
	{
		/* Destroy STREAM_ACCESS_t */
		DD_MEM_Free(pHTuner->pHStream);
		pHTuner->pHStream = NULL;
	}
	PrintExit;
	return eDiErr;

func_error:
	PrintError("%s(%d) Error> failed\n", __FUNCTION__, __LINE__);

	if ((NULL != pHTuner) && (pHTuner->pHStream != NULL))
	{
		pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;

		/* Destroy multicast sys */
		pstMulticastSys = (MULTICAST_ACCESS_SYS_t *)pAccess->p_sys;
		if (pstMulticastSys == NULL)
		{
			DD_MEM_Free(pstMulticastSys);
			pstMulticastSys = NULL;
		}
		pAccess->p_sys = NULL;

		/* Destroy stream access */
		if (pAccess->psz_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}

		DD_MEM_Free(pHTuner->pHStream);
		pHTuner->pHStream = NULL;
	}
	PrintExit;
	return eDiErr;
}


