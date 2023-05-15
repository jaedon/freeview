/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_stream.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	Real-Time Streaming Tuner Body
 * Module: 			    IPTUNER Stream Tuner
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

#include "di_iptuner_config.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "bstd.h"
#include "vfio.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"

#include "di_media.h"

#include "util_url.h"
#include "di_iptuner.h"
#include "di_iptuner_stream.h"
#include "di_iptuner_http.h"
#include "di_iptuner_http_chnk.h"
#include "di_iptuner_mms.h"
#ifdef CONFIG_MEDIA_SSTR
#include "di_iptuner_sstr.h" /* for SSTR */
#endif
#include "di_iptuner_hls.h" /* for HLS */
#include "di_iptuner_underflow.h"

/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */

//#define DI_IPTUNER_STREAM_DEBUG
//#define DI_IPTUNER_STREAM_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG

#ifdef DI_IPTUNER_STREAM_DEBUG
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

#define UNUSED_PARAM(x)	 ((void)x)


#define MAX_IP_SIZE			(128)	/** bytes */
#define MAX_PORT_SIZE		(16)	/** bytes */
#define MAX_FILE_NAME_SIZE	(IPTUNER_CONST_URL_LENGTH)	/** bytes */
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
HULONG s_ulStreamMsgQId;
HULONG s_ulBufSema = -1;
HUINT32 g_nDeviceId;
/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
static URL_t					stURL;

/* STREAM_BUFFERING || STREAM_BUFFERING_MMS || STREAM_BUFFERING_HLS || STREAM_BUFFERING_SSTR */
static HULONG s_ulStreamTaskId=0;
static HUINT8 s_ucStreamIsInitialized = FALSE;
static HUINT8 s_ucStreamQuit = FALSE;
static HBOOL  s_bStreamTaskFinished = TRUE;
static HULONG s_ulStreamSema;
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
#if __________task__________
#endif

#if __________p__________
#endif

/* STREAM_BUFFERING || STREAM_BUFFERING_MMS || STREAM_BUFFERING_HLS || STREAM_BUFFERING_SSTR */
static void IPTUNER_StreamTask (void *pParam)
{
	DI_IPTUNER_Msg_t s_Msg;
	HINT32 nVKRet;
	DI_IPTUNER_Index_e nTunerIndex; // = *(DI_IPTUNER_Index_e*)pParam;

#if 1 /* warning: unused parameter */
	UNUSED_PARAM(pParam);
#endif

	PrintEnter;

	while ( TRUE != s_ucStreamQuit )
	{
		nVKRet = VK_MSG_Receive (s_ulStreamMsgQId, &s_Msg, sizeof(DI_IPTUNER_Msg_t));
		if ( nVKRet != VK_OK )
		{
			continue;
		}
		/* else { continue } */

		nTunerIndex = s_Msg.unParam1;

#if STREAM_BUFFERING_MMS
		if ( s_Msg.unMsg == TUNER_STREAM_MSG_MMS_BUFFERING )
		{
			PrintDebug ("%s(%d) : TUNER_STREAM_MMS_START Received (Start Buffering %d) \n",__FUNCTION__,__LINE__,nTunerIndex);
			/* for test */
			/* TODO :: */
//			(void) VK_SEM_Get (s_ulStreamSema);

			/** Buffering Start */
			STREAM_MMS_PreBuffering (nTunerIndex);

//			(void) VK_SEM_Release (s_ulStreamSema );
		}
#endif

#if STREAM_BUFFERING
		if ( s_Msg.unMsg == TUNER_STREAM_MSG_HTTP_BUFFERING )
		{
			PrintDebug ("%s(%d) : TUNER_STREAM_HTTP_START Received (Start Buffering %d )\n",__FUNCTION__,__LINE__,nTunerIndex);
			/* for test */
			/* TODO :: */
			(void) VK_SEM_Get (s_ulStreamSema);

			/** Buffering Start */
			STREAM_HTTP_PreBuffering (nTunerIndex);

			(void) VK_SEM_Release (s_ulStreamSema );
		}
#endif
#if STREAM_BUFFERING_SSTR
		if ( s_Msg.unMsg == TUNER_STREAM_MSG_SSTR_BUFFERING ) /* for SSTR */
		{
			PrintDebug ("%s(%d) : TUNER_STREAM_SSTR_START Received (Start Buffering %d )\n",__FUNCTION__,__LINE__,nTunerIndex);
			/* for test */
			/* TODO :: */
			(void) VK_SEM_Get (s_ulStreamSema);

			/** Buffering Start */
			DI_STREAM_SSTR_PreBuffering (nTunerIndex);

			(void) VK_SEM_Release (s_ulStreamSema );
		}
#endif
#if STREAM_BUFFERING_HLS
		if ( s_Msg.unMsg == TUNER_STREAM_MSG_HLS_BUFFERING ) /* for HLS */
		{
			PrintDebug ("%s(%d) : TUNER_STREAM_HLS_START Received (Start Buffering %d )\n",__FUNCTION__,__LINE__,nTunerIndex);
			/* for test */
			/* TODO :: */
			(void) VK_SEM_Get (s_ulStreamSema);

			/** Buffering Start */
			DI_STREAM_HLS_PreBuffering (nTunerIndex);

			(void) VK_SEM_Release (s_ulStreamSema );
		}
#endif
		if ( s_Msg.unMsg == TUNER_STREAM_MSG_HTTP_CHNK_BUFFERING )
		{
			PrintDebug ("%s(%d) : TUNER_STREAM_HTTP_START Received (Start Buffering %d )\n",__FUNCTION__,__LINE__,nTunerIndex);
			(void) VK_SEM_Get (s_ulStreamSema);

			/** Buffering Start */
			DI_STREAM_HTTP_CHNK_PreBuffering (nTunerIndex);

			(void) VK_SEM_Release (s_ulStreamSema );
		}

		if ( s_Msg.unMsg == TUNER_STREAM_MSG_STOP_BUFFERING )
		{
			PrintDebug ("%s(%d) : TUNER_STREAM_MSG_STOP_BUFFERING Received (stop Buffering %d )\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
		}
		/* else { continue } */
	}

	s_ucStreamQuit = TRUE;
	s_bStreamTaskFinished = TRUE;

	PrintExit;
	return;
}

static HINT32 P_IPTUNER_StreamInitThread (DI_IPTUNER_Index_e nTunerIndex)
{
	DI_IPTUNER_Msg_t s_Msg;
	HINT32 nVKRet;

	PrintEnter;

	if ( TRUE != s_ucStreamIsInitialized )
	{
		/* create & init message que */
		nVKRet = VK_MSG_Create(TUNER_STREAM_MSG_QUEUE_SIZE, sizeof(s_Msg), "STREAM_MSGQ", &s_ulStreamMsgQId, VK_SUSPENDTYPE_FIFO);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> STREAM Task msgQ creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}	/* Semaphore for shared data between main_task and timer_task */

		nVKRet = VK_SEM_Create(&s_ulStreamSema, "StreamTunerSema", VK_SUSPENDTYPE_PRIORITY);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> STREAM Timer sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}
		nVKRet = VK_SEM_Create (&s_ulBufSema, "BufSema", VK_SUSPENDTYPE_PRIORITY);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> Buffer R/W sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}

		s_ucStreamQuit = FALSE;
		s_bStreamTaskFinished = FALSE;

		/** Create Download Task */
		nVKRet = VK_TASK_Create ( IPTUNER_StreamTask
							, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
							, TUNER_STREAM_STACK_SIZE
							, "STREAM_Tuner"
							, (void*)&nTunerIndex
							, &s_ulStreamTaskId
							, 0);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> STREAM Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
			return -1;
		}
		else
		{
			nVKRet = VK_EVENT_Create (&s_ulStreamTunerEventID, "StreamTunerEvent");
			if ( VK_OK != nVKRet )
			{
				PrintError("%s (%d) Error> Error(0x%x) in VK_EVENT_Create()\n\r",__FUNCTION__,__LINE__,nVKRet);
				return -1;
			}
			else
			{
				s_ucStreamIsInitialized = TRUE;
			}
		}

		/** Start Download Task */
		VK_TASK_Start(s_ulStreamTaskId);
	}
	else
	{
		PrintError  ("%s (%d) Error>  Already initialized !\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return 0;
}



#if __________di__________
#endif
/*******************************************************************/
/************************ public funtions **************************/
/*******************************************************************/
DI_ERR_CODE DI_IPTUNER_StreamOpen (DI_IPTUNER_Index_e nTunerIndex, HINT8 *pUrl, DI_MEDIA_INFO_t *pstMediaInfo)
{
	HINT32	nRet = DI_ERR_OK;
	HBOOL	bIsIcy = FALSE;
#if 0 /* unused vars */
	HUINT64 uStreamSize = 0;
#endif

	HINT64 llFileLength = 0;

	PrintEnter;

	VK_memset (&stURL,0,sizeof(URL_t));

	if ( NULL == pUrl )
	{
		PrintError ("%s(%d) Error> - pUrl is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		PrintExit;
		return nRet;
	}
	else if (pstMediaInfo == NULL)
	{
		PrintError ("%s(%d) Error> - pstMediaInfo is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		PrintExit;
		return nRet;
	}	/* else - { continue } */

	PrintData ("%s(%d) pUrl=%s, nTunerIndex=%d\n",__FUNCTION__,__LINE__, pUrl, nTunerIndex);

	(void) UTIL_UrlParse (&stURL,(const char*)pUrl,  0);

	/* 1. Register Stream VFIO */
	switch (nTunerIndex)
	{
		case DI_IPTUNER_STREAM_HTTP	:
#if !defined(HD5500T_PATCH)
			if(NULL == VK_strstr((char *)pUrl, "https://"))
			{
				/* ICY Protocol인지 판단을 위해 Probe(Open)를 먼저 수행함 단, 이 동작으로 현재 Server에 ICY가 아닌 경우 세번 접근하게 됨. */
				nRet = DI_STREAM_HTTP_ICYProbe(pUrl,pstMediaInfo, &bIsIcy);
				if(nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
					/* get httpErrorCode */
					(void) DI_STREAM_HTTP_GetProtoStatusCode (&pstMediaInfo->protoStatus);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
			}
			else
			{
				bIsIcy = FALSE;
			}
#endif

			/* ICY Protocol이 아니면 DI Media Probe를 수행하며 VFIO를 위한 RegisterCallback은 서로 다른 것을 사용함. */
			if(bIsIcy == TRUE)
			{
				(void) DI_STREAM_HTTP_IcyVfioRegisterCallback ();
			}
			else
			{
				(void) DI_STREAM_HTTP_RegisterCallback ();
				/* Removed warning: pointer targets in passing argument 1 of 'DI_MEDIA_Probe' differ in signedness */
				nRet = (HINT32)DI_MEDIA_Probe ((HUINT8 *)pUrl,pstMediaInfo);
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
					/* get httpErrorCode */
					(void) DI_STREAM_HTTP_GetProtoStatusCode (&pstMediaInfo->protoStatus);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				else
				{
#if REAL_TIME_MULTISOCKET_SUPPORT
					/* Multi Socket사용시 Drm Handle을 생성하여하는지 구분하기 위함. */
					if(pstMediaInfo->is_drm)
					{
						(void)DI_STREAM_HTTP_SetContentFormat(pstMediaInfo->stream_type);
						(void)DI_STREAM_HTTP_SetDrmMode(TRUE);
					}
					else
					{
						(void)DI_STREAM_HTTP_SetContentFormat(pstMediaInfo->stream_type);
						(void)DI_STREAM_HTTP_SetDrmMode(FALSE);
					}
#endif
				}
			}

			(void) DI_MEDIA_PrintProbeInfo(pstMediaInfo);

#if STREAM_BUFFERING
			if(bIsIcy != TRUE)
			{
				nRet = DI_STREAM_HTTP_CreateBuffer (pstMediaInfo);
				if (nRet != DI_ERR_OK)
				{
					(void) DI_STREAM_HTTP_FreeBuffer ();

					PrintError ("%s(%d) Fail! DI_STREAM_CreateBuffer \n",__FUNCTION__,__LINE__);
					PrintError ("%s(%d) Enter Real-Time Mode. \n",__FUNCTION__,__LINE__);
					nRet = DI_ERR_OK;
					break;
				}
				else
				{
					PrintError ("%s(%d) Enter PreBuffering Mode. \n",__FUNCTION__,__LINE__);
					/** Create Buffering Thread */
					(void) P_IPTUNER_StreamInitThread (nTunerIndex);
				}
			}
#endif
			llFileLength = DI_STREAM_HTTP_GetStreamSize();
			DI_IPTUNER_UnderflowMonitorInitThread(nTunerIndex, pstMediaInfo, llFileLength);
			/* else { continue; } */
			break;
		case DI_IPTUNER_STREAM_HTTP_CHNK	:
			if (pstMediaInfo->isHttpChunked != TRUE)
			{
				PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}

			(void) DI_STREAM_HTTP_CHNK_RegisterCallback ();

			nRet = (HINT32)DI_MEDIA_Probe ((HUINT8 *)pUrl,pstMediaInfo);
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
				/* get httpErrorCode */
				(void) DI_STREAM_HTTP_GetProtoStatusCode (&pstMediaInfo->protoStatus);
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}

			(void) DI_MEDIA_PrintProbeInfo(pstMediaInfo);
			if (pstMediaInfo->isHttpChunked != TRUE)
			{
				/*
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
				*/
				pstMediaInfo->isHttpChunked = TRUE;
				PrintError ("%s(%d) Error> Invalid [isHttpChunked:%d]\n",__FUNCTION__,__LINE__, pstMediaInfo->isHttpChunked);
			}

			if (pstMediaInfo->isHttpChunked == TRUE)
			{
				nRet = DI_STREAM_HTTP_CHNK_CreateBuffer (pstMediaInfo);
				if (nRet != DI_ERR_OK)
				{
					(void) DI_STREAM_HTTP_CHNK_FreeBuffer ();

					PrintError ("%s(%d) Fail! DI_STREAM_CreateBuffer \n",__FUNCTION__,__LINE__);
					PrintError ("%s(%d) Enter Real-Time Mode. \n",__FUNCTION__,__LINE__);
					nRet = DI_ERR_OK;
					break;
				}
				else
				{
					PrintError ("%s(%d) Enter PreBuffering Mode. \n",__FUNCTION__,__LINE__);
					/** Create Buffering Thread */
					(void) P_IPTUNER_StreamInitThread (nTunerIndex);
				}
#ifdef	CONFIG_IPTUNER_XTREMEHSS_LIVE	/* To support UK Freeview MHEG-ICS live streaming contents - C.G. Kang */
				if (pstMediaInfo->max_bitrate == 0)
				{
					pstMediaInfo->max_bitrate = 2000000;	/* 2Mbps */
					PrintError ("%s(%d) Error> Unknown max_bitrate! Assume %d bps...\n",__FUNCTION__,__LINE__, pstMediaInfo->max_bitrate);
				}
				if (pstMediaInfo->duration == 0)
				{
					pstMediaInfo->duration = 0xFFFFFFFF;
					PrintError ("%s(%d) Error> Unknown duration! Assume %d sec...\n",__FUNCTION__,__LINE__, pstMediaInfo->duration / 1000);
				}
				DI_IPTUNER_UnderflowMonitorInitThread(nTunerIndex, pstMediaInfo, 0xFFFFFFFFFFFFFF);
#endif
			}
			break;
		case DI_IPTUNER_STREAM_MMS		:
			nRet = DI_STREAM_MMS_Probe (pUrl,pstMediaInfo);
			if (nRet != DI_ERR_OK)
			{
				return nRet;
			}

			DI_STREAM_MMS_RegisterCallback ();

#if STREAM_BUFFERING_MMS
			nRet = DI_STREAM_MMS_CreateBuffer (pstMediaInfo);
			if (nRet != DI_ERR_OK)
			{
				(void) DI_STREAM_MMS_FreeBuffer ();

				PrintError ("%s(%d) Error> Fail DI_STREAM_CreateBuffer \n",__FUNCTION__,__LINE__);
				PrintError ("%s(%d) Error> Enter Real-Time Mode. \n",__FUNCTION__,__LINE__);

				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			else
			{
				PrintError ("%s(%d) Error> Enter PreBuffering Mode. \n",__FUNCTION__,__LINE__);
				/** Create Buffering Thread */
				(void) P_IPTUNER_StreamInitThread (nTunerIndex);
			}
#endif
			/* else { continue; } */
			break;
#if STREAM_BUFFERING_SSTR
		case DI_IPTUNER_STREAM_SSTR	: /* for SSTR  */
			if (NULL!= VK_strstr((char *)pUrl,".ism/Manifest")
				|| NULL!= VK_strstr((char *)pUrl,".ism/manifest")
				|| NULL!= VK_strstr((char *)pUrl,".isml/Manifest")
				|| NULL!= VK_strstr((char *)pUrl,".isml/manifest")
				|| NULL!= VK_strstr((char *)pUrl,".smil/Manifest")
				|| NULL!= VK_strstr((char *)pUrl,".smil/manifest"))
			{
				nRet = DI_STREAM_SSTR_OpenController(pUrl);
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Controller Open Fail !!\n",__FUNCTION__,__LINE__);
					/* get httpErrorCode */
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}

				nRet = DI_STREAM_SSTR_GetMediaInfo(pstMediaInfo);
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
					/* get httpErrorCode */
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				else
				{
					PrintError ("%s(%d) Enter PreBuffering Mode. \n",__FUNCTION__,__LINE__);
#if defined(DI_IPTUNER_SSTR_CONFIG_TEXTSTREAM)
					DI_STREAM_SSTR_CreateTextStreamTask();
#endif
					DI_STREAM_SSTR_CreateLiveUpdateTask();
					/** Create Buffering Thread */
					(void) P_IPTUNER_StreamInitThread (nTunerIndex);
					//(void) DI_STREAM_SSTR_InitTextStreamTask(nTunerIndex); /* not supported yet */
				}
				(void) DI_STREAM_SSTR_RegisterCallback ();
				/* for debugging */
				(void) DI_MEDIA_PrintProbeInfo (pstMediaInfo);
				nRet = DI_ERR_OK;
			}
			else
			{
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
#endif
#if STREAM_BUFFERING_HLS
		case DI_IPTUNER_STREAM_HLS : /* for HLS  */
			if(NULL!= VK_strstr((char *)pUrl,".m3u8"))
			{
				nRet = DI_STREAM_HLS_OpenController(pUrl);
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Controller Open Fail !!\n",__FUNCTION__,__LINE__);
					(void)DI_STREAM_HLS_CloseController();
					/* get httpErrorCode */
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				// pUrl = DI_STREAM_HLS_InitSegUrl();
				// (void) DI_STREAM_HTTP_RegisterCallback ();
				(void) DI_STREAM_HLS_RegisterCallback ();
				nRet = DI_MEDIA_Probe ((HUINT8 *)pUrl,pstMediaInfo);
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
					(void)DI_STREAM_HLS_CloseController();
					/* get httpErrorCode */
					(void) DI_STREAM_HTTP_GetProtoStatusCode (&pstMediaInfo->protoStatus);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				else
				{
					DI_STREAM_HLS_GetMediaInfo(pstMediaInfo);
				}
				(void) DI_MEDIA_PrintProbeInfo(pstMediaInfo);
				(void) DI_STREAM_HLS_CreateLiveUpdateTask();
				//(void) DI_STREAM_HTTP_UnregisterCallback ();
				{
					PrintError ("%s(%d) Enter PreBuffering Mode. \n",__FUNCTION__,__LINE__);
					/** Create Buffering Thread */
					(void) P_IPTUNER_StreamInitThread (nTunerIndex);
					/** Blocked by ykkim3  */
				}
				//(void) DI_STREAM_HLS_RegisterCallback ();
				/* for debugging */
//				(void) DI_MEDIA_PrintProbeInfo (pstMediaInfo);
#ifdef CONFIG_HLS_UNDERFLOW_MONITORING_FOR_BBC_CATAL
				DI_IPTUNER_UnderflowHlsMonitorInitThread(nTunerIndex, pstMediaInfo, llFileLength);
#endif
				nRet = DI_ERR_OK;
			}
			else
			{
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
		break;
#endif
		case DI_IPTUNER_STREAM_RTSP	:
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			UTIL_UrlClean (&stURL);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}

	PrintExit;

	return nRet;
}


DI_ERR_CODE DI_IPTUNER_StreamClose (DI_IPTUNER_Index_e nTunerIndex)
{
	HINT32	nRet = DI_ERR_OK;
	PrintEnter;

	UTIL_UrlClean (&stURL);

	switch (nTunerIndex)
	{
		case DI_IPTUNER_STREAM_HTTP	:
			DI_STREAM_HTTP_UnregisterCallback ();
#if STREAM_BUFFERING
			(void) DI_STREAM_HTTP_FreeBuffer ();
#endif
			break;
		case DI_IPTUNER_STREAM_HTTP_CHNK	:
			(void) DI_STREAM_HTTP_CHNK_FreeBuffer ();
			DI_STREAM_HTTP_CHNK_UnregisterCallback ();
			break;
		case DI_IPTUNER_STREAM_MMS		:
#if STREAM_BUFFERING_MMS
			s_ucStreamQuit = TRUE;
			{
#if 0
				PrintError ("%s(%d) TEMP CLOSE : FIXME !!!!!!!!!!!!!!!!!!!!!!!!!!\n",__FUNCTION__,__LINE__);
				VFIO_Handle_t handle;
				DI_STREAM_MMS_Close (handle);
#endif
			}
#endif
			DI_STREAM_MMS_UnregisterCallback ();
#if STREAM_BUFFERING_MMS
			(void) DI_STREAM_MMS_FreeBuffer ();
#endif
			break;
#if STREAM_BUFFERING_SSTR
		case DI_IPTUNER_STREAM_SSTR : /* for SSTR */
			DI_STREAM_SSTR_UnregisterCallback();
			if(DI_STREAM_SSTR_GetBufferingState() != DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_DONE)
			{
//				DI_STREAM_SSTR_SetBufferingState(DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING);
				DI_STREAM_SSTR_StopBufferingTask();

			}
			while(DI_STREAM_SSTR_GetBufferingState() != DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_DONE)
			{
				VK_TASK_Sleep(100);
			}

			(void)DI_STREAM_SSTR_DestroyLiveUpdateTask(); /* Warning Blocking */
#if defined(DI_IPTUNER_SSTR_CONFIG_TEXTSTREAM)
			(void)DI_STREAM_SSTR_DestroyTextStreamTask();
#endif
//			SSTR_BUFFERMGR_Close(SSTR_CONTROLLER_GetBuffer(DI_STREAM_SSTR_GetHandle()));
			(void)DI_STREAM_SSTR_CloseController();
			//DI_STREAM_SSTR_DestroyTextStreamTask(nTunerIndex); /* not supported yet */
			//SSTR_CONTROLLER_Close(DI_STREAM_SSTR_GetHandle());
			break;
#endif
#if STREAM_BUFFERING_HLS
		case DI_IPTUNER_STREAM_HLS : /* for HLS  */
			DI_STREAM_HLS_UnregisterCallback();
			if(DI_STREAM_HLS_GetBufferingState() != DI_IPTUNER_HLS_BUFFERING_TASK_STATE_DONE)
			{
//				DI_STREAM_HLS_SetBufferingState(DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING);
				DI_STREAM_HLS_StopBufferingTask();

			}
			while(DI_STREAM_HLS_GetBufferingState() != DI_IPTUNER_HLS_BUFFERING_TASK_STATE_DONE)
			{
				VK_TASK_Sleep(100);
			}
			(void)DI_STREAM_HLS_DestroyLiveUpdateTask();
			(void)DI_STREAM_HLS_CloseController();
			break;
#endif

		case DI_IPTUNER_STREAM_RTSP	:
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}

	s_ucStreamQuit = TRUE;

	if (s_ucStreamIsInitialized == TRUE)
	{
		{
			DI_IPTUNER_Msg_t s_Msg;
			PrintData ("%s (%d) Buffering Stop from OpenNexusCallback\n",__FUNCTION__,__LINE__);
			(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));
			s_Msg.unMsg = TUNER_STREAM_MSG_STOP_BUFFERING;
			s_Msg.unParam1 = nTunerIndex;
			(void) VK_MSG_Send ( s_ulStreamMsgQId, (HUINT32 *)&s_Msg, sizeof(DI_IPTUNER_Msg_t) ) ;
		}

		while (s_bStreamTaskFinished != TRUE)
		{
			VK_TASK_Sleep (100);
		}

		(void) VK_TASK_Destroy (s_ulStreamTaskId);

		s_ucStreamIsInitialized = FALSE;

		(void) VK_SEM_Destroy (s_ulStreamSema);

		(void) VK_SEM_Destroy (s_ulBufSema);

		(void) VK_EVENT_Destroy (s_ulStreamTunerEventID);

		(void) VK_MSG_Destroy (s_ulStreamMsgQId);
	}
	DI_IPTUNER_UnderflowMonitorTerminateThread();
#ifdef CONFIG_HLS_UNDERFLOW_MONITORING_FOR_BBC_CATAL
	if (nTunerIndex == DI_IPTUNER_STREAM_HLS)
	{
		DI_IPTUNER_UnderflowHlsMonitorTerminateThread();
	}
#endif

	PrintExit;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_StreamGetInfo (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_Info_t *pInfo)
{
#ifdef CONFIG_MEDIA_SSTR	/* not supported yet Blocked By ykkim */
//	SSTR_MANIFEST_Info_t *pManifest = NULL;
//	DI_IPTUNER_SSTR_Info_t *pSSTRInfo = NULL;
#endif
	/* TODO : junsy */
//	UNUSED_PARAM (pInfo);
//	UNUSED_PARAM (nTunerIndex);
	switch (nTunerIndex)
	{
			case DI_IPTUNER_STREAM_MMS	:
				/* not supported yet */
				break;
#if 0
			case DI_IPTUNER_STREAM_RTSP :
				/* not supported yet */
				break;
#endif
			case DI_IPTUNER_STREAM_HTTP :
			case DI_IPTUNER_STREAM_HTTP_CHNK :
				/* not supported yet */
				break;
#ifdef CONFIG_MEDIA_SSTR
			case DI_IPTUNER_STREAM_SSTR :
				pInfo->adaptiveInfo.pSSTRInfo = DI_STREAM_SSTR_GetManifestInfo();
				if(pInfo->adaptiveInfo.pSSTRInfo == NULL)
				{
					return DI_ERR_ERROR;
				}
				break;
#endif
			default  :
				/* Caution to warning: unused parameter 'pInfo' */
				/* Use UNUSED_PARAM() macro carefully */
				UNUSED_PARAM (pInfo);
				PrintError ("%s(%d) Error>	-  Not Supported Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
				break;
		}
		PrintExit;

	return DI_ERR_OK;
}

/** Download 가 Stop 된 경우 Resume의 용도로 사용한다. */
DI_ERR_CODE DI_IPTUNER_StreamControl (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2)
{
	DI_ERR_CODE nRet=DI_ERR_OK;

	PrintEnter;

	switch (nTunerIndex)
	{
		case DI_IPTUNER_STREAM_MMS 	:
			nRet = DI_STREAM_MMS_Control ( cmd, val, val2 );
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_MMS_SetSpeed Fail \n",__FUNCTION__,__LINE__);
			}
			/* else { continue; } */
			break;
#if 0
		case DI_IPTUNER_STREAM_RTSP :
			nRet = DI_STREAM_RTSP_Control ();
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_RTSP_SetSpeed Fail \n",__FUNCTION__,__LINE__);
			}
			/* else { continue; } */
			break;
#endif
		case DI_IPTUNER_STREAM_HTTP :
			/* XXX : DI_STREAM_HTTP_Control is a Dummy function */
			nRet = DI_STREAM_HTTP_Control (cmd, val, val2);
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_STREAM_SetSpeed Fail \n",__FUNCTION__,__LINE__);
			}
			/* else { continue; } */
			break;
#if STREAM_BUFFERING_SSTR
		case DI_IPTUNER_STREAM_SSTR :
			nRet = DI_STREAM_SSTR_Control (cmd, val, val2);
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_STREAM_SetSpeed Fail \n",__FUNCTION__,__LINE__);
			}
			/* else { continue; } */
			break;
#endif
#if STREAM_BUFFERING_HLS
		case DI_IPTUNER_STREAM_HLS : /* for HLS  */
			nRet = DI_STREAM_HLS_Control (cmd, val, val2);
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_STREAM_SetSpeed Fail \n",__FUNCTION__,__LINE__);
			}
			/* else { continue; } */
			break;
#endif
		default  :
			nRet = DI_ERR_INVALID_PARAM;
			PrintError ("%s(%d) Error>  -  Not Supported Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
	}
	PrintExit;

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_StreamSendEvent (void)
{
	PrintEnter;

	(void) VK_EVENT_Send (s_ulStreamTunerEventID, (unsigned long)0);

	PrintExit;
	return DI_ERR_OK;
}

#ifdef SUPPORT_TITLE
DI_ERR_CODE DI_IPTUNER_ICY_SreamRegisterEventCallback (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_EventCallback_t *pCallback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	UNUSED_PARAM (nTunerIndex);

	PrintEnter;

	nRet = DI_STREAM_HTTP_ICY_RegisterEventCallback (pCallback);
	if(nRet != DI_ERR_OK)
	{
		PrintError("%s (%d) Error> DI_STREAM_HTTP_RegisterEventCallback Fail\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return nRet;
}

void DI_IPTUNER_ICY_StreamUnregisterEventCallback (void)
{
	PrintEnter;

	DI_STREAM_HTTP_ICY_UnregisterEventCallback();

	PrintExit;
	return;
}
#endif

DI_ERR_CODE DI_IPTUNER_StreamRegisterCallback (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_PlayCallback_t *pCallback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	switch ( nTunerIndex )
	{
		case DI_IPTUNER_STREAM_HTTP	:
#if STREAM_BUFFERING
			nRet = DI_STREAM_HTTP_RegisterBufferingCallback (pCallback);
#endif
			break;
		case DI_IPTUNER_STREAM_MMS :
#if STREAM_BUFFERING_MMS
			nRet = DI_STREAM_MMS_RegisterBufferingCallback (pCallback);
#endif
			break;
		case DI_IPTUNER_STREAM_HTTP_CHNK	:
			nRet = DI_STREAM_HTTP_CHNK_RegisterBufferingCallback (pCallback);
			break;
		case DI_IPTUNER_STREAM_RTSP	:
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			break;
	}

	if(nRet != DI_ERR_OK)
	{
		PrintError("%s (%d) Error> DI_STREAM_MMS_RegisterBufferingCallback Fail\n",__FUNCTION__,__LINE__);
	}

	PrintExit;
	return nRet;
}
void DI_IPTUNER_StreamUnregisterCallback (DI_IPTUNER_Index_e nTunerIndex)
{
	PrintEnter;
	switch ( nTunerIndex )
	{
		case DI_IPTUNER_STREAM_HTTP	:
#if STREAM_BUFFERING
			DI_STREAM_HTTP_UnregisterBufferingCallback ();
#endif
			break;
		case DI_IPTUNER_STREAM_MMS :
#if STREAM_BUFFERING_MMS
			DI_STREAM_MMS_UnregisterBufferingCallback ();
#endif
			break;
		case DI_IPTUNER_STREAM_HTTP_CHNK	: /* HTTP Chunked Live Streaming */
			DI_STREAM_HTTP_CHNK_UnregisterBufferingCallback ();
			break;
		case DI_IPTUNER_STREAM_RTSP	:
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			break;
	}
	PrintExit;
	return;
}

