/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_adaptive.c  $
 * Version:			$Revision:   $
 * Original Author:	Young Ki Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	     Adaptive Streaming Tuner
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2011 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

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
#include "vfio20.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_media20.h"
#include "di_uart.h"
#include "drv_media20.h"

#include "util_url.h"
#include "di_iptuner20.h"
#include "di_iptuner_http.h"
#include "di_iptuner_adaptive.h"

#ifdef CONFIG_MEDIA_SSTR
#include "di_iptuner_sstr.h" /* for SSTR */
#endif
#include "di_iptuner_hls.h" /* for HLS */
#ifdef CONFIG_MEDIA_DASH
#include "di_iptuner_dash.h" /* for DASH */
#endif
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */

//#define DI_IPTUNER_ADAPTIVE_DEBUG

#ifdef IPTUNER_ALL_DEBUG

#ifdef DI_IPTUNER_ADAPTIVE_DEBUG
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
#define MAX_FILE_NAME_SIZE	(1024)	/** bytes */
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

#if __________p__________
#endif

static void P_AdaptiveBufferingTask (void *pParam)
{
	DI_IPTUNER_Msg_t s_Msg;
	HINT32 nVKRet;
	DI_IPTUNER_t *pHTuner = (DI_IPTUNER_t*) pParam;
	PrintEnter;
	if (pHTuner == NULL)
	{
		PrintDebug ("%s (%d) : P_AdaptiveBufferingTask unavailable (pHTuner handle is null)!!!\n",__FUNCTION__,__LINE__);
		return;
	}
	pHTuner->bAbrTaskFinished = FALSE;
	while ( pHTuner->bAbrTaskQuit == FALSE)
	{
		nVKRet = VK_MSG_Receive (pHTuner->ulAbrTaskMsgQueue, &s_Msg, sizeof(DI_IPTUNER_Msg_t));
		if ( nVKRet != VK_OK )
		{
			continue;
		}
		/* else { continue } */

//		nTunerIndex = s_Msg.unParam1;

#ifdef CONFIG_MEDIA_SSTR
		if ( s_Msg.unMsg == TUNER_ADAPTIVE_MSG_SSTR_BUFFERING ) /* for SSTR */
		{
			PrintDebug ("%s(%d) : TUNER_ADAPTIVE_SSTR_START Received (Start Buffering %d )\n",__FUNCTION__,__LINE__,pHTuner->bAbrTaskQuit);
			/* for test */
			/* TODO :: */
			(void) VK_SEM_Get (pHTuner->ulAbrTaskSema);

			/** Buffering Start */
			DI_STREAM_SSTR_PreBuffering (pHTuner);

			(void) VK_SEM_Release (pHTuner->ulAbrTaskSema );
		}
#endif
		if ( s_Msg.unMsg == TUNER_ADAPTIVE_MSG_HLS_BUFFERING ) /* for HLS */
		{
			PrintDebug ("%s(%d) : TUNER_ADAPTIVE_HLS_START Received (Start Buffering %d )\n",__FUNCTION__,__LINE__,pHTuner->bAbrTaskQuit);
			/* for test */
			/* TODO :: */
			(void) VK_SEM_Get (pHTuner->ulAbrTaskSema);

			/** Buffering Start */
			DI_STREAM_HLS_PreBuffering (pHTuner);

			(void) VK_SEM_Release (pHTuner->ulAbrTaskSema );
		}
		if ( s_Msg.unMsg == TUNER_ADAPTIVE_MSG_STOP_BUFFERING )
		{
			PrintDebug ("%s(%d) : TUNER_ADAPTIVE_MSG_STOP_BUFFERING Received (stop Buffering %d )\n",__FUNCTION__,__LINE__,pHTuner->bAbrTaskQuit);
			break;
		}
		/* else { continue } */
	}
	pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_COMPLETE;
	pHTuner->bAbrTaskFinished = TRUE;
	PrintExit;
	return;
}

static HINT32 P_IPTUNER_InitAdaptiveThread (DI_IPTUNER_t* pHTuner)
{
	HUINT8 strName[32] = {0};
	DI_IPTUNER_Msg_t s_Msg;
	HINT32 nVKRet;

	PrintEnter;
	if(pHTuner == NULL)
	{
		PrintError	("%s (%d) Error>  pHTuner is NULL !\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if ( TRUE != pHTuner->bAbrTaskInit )
	{
		/* create & init message que */
#if 1
		VK_snprintf((char*)strName,VK_strlen("ABR_TASKMSGQ_")+10, "ABR_TASKMSGQ_%p", (void*)pHTuner);
		nVKRet = VK_MSG_Create(TUNER_ADAPTIVE_MSG_QUEUE_SIZE, sizeof(s_Msg), (const char*)strName, &pHTuner->ulAbrTaskMsgQueue, VK_SUSPENDTYPE_FIFO);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> STREAM Task msgQ creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}	/* Semaphore for shared data between main_task and timer_task */
		VK_MEM_Memset(strName,0x00,32);
		VK_snprintf((char*)strName,VK_strlen("ABR_TASKSEM_")+10, "ABR_TASKSEM_%p", (void*)pHTuner);
		nVKRet = VK_SEM_Create(&pHTuner->ulAbrTaskSema, (const char*)strName, VK_SUSPENDTYPE_PRIORITY);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> STREAM Timer sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}
#endif
		VK_MEM_Memset(strName,0x00,32);
		VK_snprintf((char*)strName,VK_strlen("ABR_TASK_")+10, "ABR_TASK_%p", (void*)pHTuner);
		/** Create Download Task */
		nVKRet = VK_TASK_Create ( P_AdaptiveBufferingTask
							, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
							, TUNER_ADAPTIVE_STACK_SIZE
							, (const char*)strName
							, (void*)pHTuner
							, &pHTuner->ulAbrTaskId
							, 0);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> STREAM Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
			return -1;
		}
		/** Start Download Task */
		pHTuner->bAbrTaskQuit = FALSE;
		pHTuner->bAbrTaskInit = TRUE;
		VK_TASK_Start(pHTuner->ulAbrTaskId);
		PrintError ("%s (%d) Debug Task START!\n",__FUNCTION__,__LINE__);
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

DI_ERR_CODE DI_IPTUNER_AdaptiveSetSettings (DI_IPTUNER_t *pHTuner, DI_IPTUNER_AdaptiveSettings_t *pAdaptiveSettings)
{
	DI_ERR_CODE	nRet = DI_ERR_OK;
	PrintEnter;

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError ("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* check Tuner url */
	if ( NULL == pHTuner->hAbr)
	{
		PrintError ("%s(%d) Error> Adaptive Handle is NULL\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* 1. Register Stream VFIO */
	switch (pHTuner->eProtocolType)
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_PROTOCOL_SSTR	: /* for SSTR  */
			if(DI_STREAM_SSTR_SetSettings(pHTuner->hAbr, pAdaptiveSettings) !=  SSTR_ERR_OK)
			{
				PrintError  ("%s(%d) Error> Adaptive Setting Failed !\n",__FUNCTION__,__LINE__);
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
#endif
		case DI_IPTUNER_PROTOCOL_HLS : /* for HLS  */
			if(DI_STREAM_HLS_SetSettings(pHTuner->hAbr, pAdaptiveSettings) !=  HLS_OK)
			{
				PrintError  ("%s(%d) Error> Adaptive Setting Failed !\n",__FUNCTION__,__LINE__);
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
		default :
			PrintError  ("%s(%d) Error> not supported protocol\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}
	VK_MEM_Memcpy(&pHTuner->tAdaptiveSettings,pAdaptiveSettings,sizeof(DI_IPTUNER_AdaptiveSettings_t));
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_AdaptiveGetSettings (DI_IPTUNER_t *pHTuner, DI_IPTUNER_AdaptiveSettings_t *pAdaptiveSettings)
{
	DI_ERR_CODE	nRet = DI_ERR_OK;
	PrintEnter;

#ifndef CONFIG_MEDIA_SSTR
	UNUSED_PARAM(pAdaptiveSettings);
#endif

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError ("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* check Tuner url */
	if ( NULL == pHTuner->hAbr)
	{
		PrintError ("%s(%d) Error> Adaptive Handle is NULL\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* 1. Register Stream VFIO */
	switch (pHTuner->eProtocolType)
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_PROTOCOL_SSTR	: /* for SSTR  */
			if(DI_STREAM_SSTR_GetSettings(pHTuner->hAbr, pAdaptiveSettings) !=  SSTR_ERR_OK)
			{
				PrintError  ("%s(%d) Error> Adaptive Setting Failed !\n",__FUNCTION__,__LINE__);
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
#endif
		case DI_IPTUNER_PROTOCOL_HLS : /* for HLS  */
			if(DI_STREAM_HLS_GetSettings(pHTuner->hAbr, pAdaptiveSettings) !=  HLS_OK)
			{
				PrintError  ("%s(%d) Error> Adaptive Setting Failed !\n",__FUNCTION__,__LINE__);
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Protocol !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_AdaptiveProbe (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE	nRet = DI_ERR_OK;
#ifdef CONFIG_MEDIA_DASH
	DASH_SEGMENT_FORMAT_e format;
#endif
	PrintEnter;

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError ("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* check Tuner url */
	if ( NULL == pHTuner->pUrl)
	{
		PrintError ("%s(%d) Error> Tuner Handle has invalid url (pUrl is NULL)\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	PrintData ("%s(%d) TunerHandle=[%p] URL=[%s]\n"
		,__FUNCTION__
		,__LINE__
		, pHTuner
		, pHTuner->pUrl);

	/* 1. Register Stream VFIO */
	switch (pHTuner->eProtocolType)
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_PROTOCOL_SSTR	: /* for SSTR  */
			if (NULL!= VK_strstr((char *)pHTuner->pUrl,".ism/Manifest")
				|| NULL!= VK_strstr((char *)pHTuner->pUrl,".ism/manifest")
				|| NULL!= VK_strstr((char *)pHTuner->pUrl,".isml/Manifest")
				|| NULL!= VK_strstr((char *)pHTuner->pUrl,".isml/manifest")
				|| NULL!= VK_strstr((char *)pHTuner->pUrl,".smil/Manifest")
				|| NULL!= VK_strstr((char *)pHTuner->pUrl,".smil/manifest"))
			{
				if(pHTuner->hAbr != NULL)
				{
					PrintError ("%s(%d) Error> SSTR Controller Already Opened !!\n",__FUNCTION__,__LINE__);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				pHTuner->hAbr = DI_STREAM_SSTR_OpenController(pHTuner->pUrl);
				if (pHTuner->hAbr == NULL)
				{
					PrintError ("%s(%d) Error> Controller Open Fail !!\n",__FUNCTION__,__LINE__);
					(void)DI_STREAM_SSTR_CloseController(pHTuner->hAbr);
					pHTuner->hAbr = NULL;
					/* get httpErrorCode */
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				nRet = DI_STREAM_SSTR_GetMediaInfo(pHTuner->hAbr,&pHTuner->tMediaInfo);
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
					(void)DI_STREAM_SSTR_CloseController(pHTuner->hAbr);
					pHTuner->hAbr = NULL;
					/* get httpErrorCode */
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
//				(void) DRV_MEDIA_PrintProbeInfo (&pHTuner->tMediaInfo);
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
		case DI_IPTUNER_PROTOCOL_HLS : /* for HLS  */
			if(NULL!= VK_strstr((char *)pHTuner->pUrl,".m3u8"))
			{
				if(pHTuner->hAbr != NULL)
				{
					PrintError ("%s(%d) Error> SSTR Controller Already Opened !!\n",__FUNCTION__,__LINE__);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				pHTuner->hAbr = DI_STREAM_HLS_OpenController(pHTuner->pUrl);
				if (pHTuner->hAbr == NULL)
				{
					PrintError ("%s(%d) Error> Controller Open Fail !!\n",__FUNCTION__,__LINE__);
					/* get httpErrorCode */
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				(void) DI_STREAM_HLS_RegisterCallback ((const char*)pHTuner->pUrl);
				// DI_STREAM_HLS_SetSettings(pHTuner->hAbr,&pHTuner->tAdaptiveSettings); /* for Heuristics */
				nRet = DRV_MEDIA_Probe (pHTuner, (HUINT8 *)pHTuner->pUrl,&pHTuner->tMediaInfo);
				(void) DI_STREAM_HLS_UnregisterCallback ((const char*)pHTuner->pUrl);
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
					(void)DI_STREAM_HLS_CloseController(pHTuner->hAbr);
					pHTuner->hAbr = NULL;
					/* get httpErrorCode */
					//(void) DI_STREAM_HTTP_GetProtoStatusCode (&pHTuner->tMediaInfo.protoStatus);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				else
				{
					DI_STREAM_HLS_GetMediaInfo(pHTuner->hAbr,&pHTuner->tMediaInfo);
				}
//				(void) DRV_MEDIA_PrintProbeInfo(&pHTuner->tMediaInfo);
				nRet = DI_ERR_OK;
			}
			else
			{
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
#ifdef CONFIG_MEDIA_DASH
		case DI_IPTUNER_PROTOCOL_DASH:
			if(NULL!= VK_strstr((char *)pHTuner->pUrl,".mpd") || NULL!= VK_strstr((const char*)pHTuner->pUrl,".xml"))
			{
				if(pHTuner->hAbr != NULL)
				{
					PrintError ("%s(%d) Error> DASH Controller Already Opened !!\n",__FUNCTION__,__LINE__);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}

				/* for DASH,MPD processing branch : MPD Task processing first*/

				/* waiting MPD processing */
				/* get type of mpd */
				/* if else processing */

				/* another open, close, destroy, control is different */

				/* Do basic initialization */
				pHTuner->hAbr = DI_STREAM_DASH_OpenController(pHTuner->pUrl);
				if (pHTuner->hAbr == NULL)
				{
					PrintError ("%s(%d) Error> Controller Open Fail !!\n",__FUNCTION__,__LINE__);
					/* get httpErrorCode */
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}

				/* Create scheduler and MPD task */
				nRet = DI_STREAM_DASH_CreateSchedulerTask(pHTuner);
				if(nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Schedulertask create failed !!\n",__FUNCTION__,__LINE__);
					(void) DI_STREAM_DASH_CloseController(pHTuner->hAbr);
					pHTuner->hAbr = NULL;
					PrintExit;
					return nRet;
				}
				nRet = DI_STREAM_DASH_CreateMPDTask(pHTuner);
				if(nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> MPD task create failed !!\n",__FUNCTION__,__LINE__);
					(void) DI_STREAM_DASH_CloseController(pHTuner->hAbr);
					pHTuner->hAbr = NULL;
					PrintExit;
					return nRet;
				}

				/* waiting for a scheduler state "READY"
				   (initially, scheduler state "READY" is set when MPD task state is changed to "READY")
				   in case of live streaming, MPD task state is changed to "READY" when after availablstarttime */

				nRet = DI_STREAM_DASH_WaitState(pHTuner->hAbr, DASH_STATE_READY);
				if(nRet == DI_ERR_ERROR)
				{
					PrintError ("%s(%d) Error> There are some trouble while task running !!\n",__FUNCTION__,__LINE__);
					(void) DI_STREAM_DASH_CloseController(pHTuner->hAbr);
					pHTuner->hAbr = NULL;
					PrintExit;
					return nRet;
				}
				nRet = DI_STREAM_DASH_GetMediaFormat(pHTuner->hAbr, &format);
				if(nRet == DI_ERR_ERROR)
				{
					PrintError ("%s(%d) Error> Cannot get media format !!\n",__FUNCTION__,__LINE__);
					(void) DI_STREAM_DASH_CloseController(pHTuner->hAbr);
					pHTuner->hAbr = NULL;
					PrintExit;
					return nRet;
				}
				if(format == DASH_SEGMENT_TS)
				{
					/* probing for TS : VFIO_OPEN and READ will be called internally */
					(void) DI_STREAM_DASH_RegisterCallback ((const char*)pHTuner->pUrl);
					nRet = DRV_MEDIA_Probe (pHTuner, (HUINT8 *)pHTuner->pUrl,&pHTuner->tMediaInfo);
					(void) DI_STREAM_DASH_UnregisterCallback ((const char*)pHTuner->pUrl);
					if (nRet != DI_ERR_OK)
					{
						PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
						(void)DI_STREAM_DASH_CloseController(pHTuner->hAbr);
						pHTuner->hAbr = NULL;
						nRet = DI_ERR_ERROR;
						PrintExit;
						return nRet;
					}
					else
					{
						DI_STREAM_DASH_GetMediaInfo(pHTuner->hAbr,&pHTuner->tMediaInfo);
					}
				}
				else if(format == DASH_SEGMENT_MP4)
				{
					DI_STREAM_DASH_GetMediaInfo(pHTuner->hAbr,&pHTuner->tMediaInfo);
				}

				(void) DRV_MEDIA_PrintProbeInfo(&pHTuner->tMediaInfo);
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
#ifdef USE_IPTUNER_ABR
		case DI_IPTUNER_PROTOCOL_ABR :

			if(NULL!= VK_strstr((const char*)pHTuner->pUrl,".mpd") || NULL!= VK_strstr((const char*)pHTuner->pUrl,".xml"))
			{
				(void) DI_STREAM_ABR_RegisterProbeCallback ((const char*)pHTuner->pUrl);
				nRet = DRV_MEDIA_Probe (pHTuner, pHTuner->pUrl, &pHTuner->tMediaInfo );
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
					/* get httpErrorCode */
#if 0
					(void) DI_STREAM_HTTP_GetProtoStatusCode (&pHTuner->tMediaInfo.protoStatus);
#endif
					(void) DI_STREAM_ABR_UnregisterProbeCallback ((const char*)pHTuner->pUrl);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				(void) DI_STREAM_ABR_UnregisterProbeCallback ((const char*)pHTuner->pUrl);
				pHTuner->tMediaInfo.max_bitrate = ABR_MPD_GetMaxBitrate (pHTuner->hAbr
																		, ABR_MPD_GetCurrentPeriodId(pHTuner->hAbr));
				ABR_MPD_SetAutoBitrate(pHTuner->hAbr);
				pHTuner->tMediaInfo.duration = ABR_MPD_GetMediaDuration (pHTuner->hAbr);
			}
			else
			{
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
#endif
		default :
			PrintError  ("%s(%d) Error> invalid Protocol !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}

	PrintExit;

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_AdaptiveOpen (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE	nRet = DI_ERR_OK;
	PrintEnter;

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError ("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* check Tuner url */
	if ( NULL == pHTuner->pUrl)
	{
		PrintError ("%s(%d) Error> Tuner Handle has invalid url (pUrl is NULL)\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	PrintData ("%s(%d) TunerHandle=[%p] URL=[%s]\n"
		,__FUNCTION__
		,__LINE__
		, pHTuner
		, pHTuner->pUrl);

	/* 1. Register Stream VFIO */
	switch (pHTuner->eProtocolType)
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_PROTOCOL_SSTR	: /* for SSTR  */
			if (NULL!= VK_strstr((char *)pHTuner->pUrl,".ism/Manifest")
				|| NULL!= VK_strstr((char *)pHTuner->pUrl,".ism/manifest")
				|| NULL!= VK_strstr((char *)pHTuner->pUrl,".isml/Manifest")
				|| NULL!= VK_strstr((char *)pHTuner->pUrl,".isml/manifest"))
			{
				if (pHTuner->hAbr == NULL)
				{
					PrintError ("%s(%d) Error> Controller is NULL !!\n",__FUNCTION__,__LINE__);
					/* get httpErrorCode */
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				else
				{
					DI_STREAM_SSTR_SetSettings(pHTuner->hAbr,&pHTuner->tAdaptiveSettings);
					PrintError ("%s(%d) Enter PreBuffering Mode. \n",__FUNCTION__,__LINE__);
#if defined(DI_IPTUNER_SSTR_CONFIG_TEXTSTREAM)
					DI_STREAM_SSTR_CreateTextStreamTask(pHTuner);
#endif
					DI_STREAM_SSTR_CreateLiveUpdateTask(pHTuner);

					/** Create Buffering Thread */
					(void) P_IPTUNER_InitAdaptiveThread (pHTuner);
					//(void) DI_STREAM_SSTR_CreateTextStreamTask(nTunerIndex); /* not supported yet */
				}
				(void) DI_STREAM_SSTR_RegisterCallback ((const char*)pHTuner->pUrl);
				/* for debugging */
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
		case DI_IPTUNER_PROTOCOL_HLS : /* for HLS  */
			if(NULL!= VK_strstr((char *)pHTuner->pUrl,".m3u8"))
			{
				if (pHTuner->hAbr == NULL)
				{
					PrintError ("%s(%d) Error> Controller is NULL !!\n",__FUNCTION__,__LINE__);
					/* get httpErrorCode */
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				else
				{
					DI_STREAM_HLS_SetSettings(pHTuner->hAbr,&pHTuner->tAdaptiveSettings);
					PrintError ("%s(%d) Enter PreBuffering Mode. \n",__FUNCTION__,__LINE__);
					DI_STREAM_HLS_CreateLiveUpdateTask(pHTuner);
					/** Create Buffering Thread */
					(void) P_IPTUNER_InitAdaptiveThread (pHTuner);
					//(void) DI_STREAM_SSTR_InitTextStreamTask(nTunerIndex); /* not supported yet */
				}
				(void) DI_STREAM_HLS_RegisterCallback ((const char*)pHTuner->pUrl);
				/* for debugging */
				nRet = DI_ERR_OK;
			}
			else
			{
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
#ifdef CONFIG_MEDIA_DASH
		case DI_IPTUNER_PROTOCOL_DASH:
			if(NULL!= VK_strstr((char *)pHTuner->pUrl,".mpd") || NULL!= VK_strstr((char*)pHTuner->pUrl, ".xml"))
			{
				if (pHTuner->hAbr == NULL)
				{
					PrintError ("%s(%d) Error> Controller is NULL !!\n",__FUNCTION__,__LINE__);
					/* get httpErrorCode */
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}

				/* waiting for DASH ready - Probing completion */
				if(DI_STREAM_DASH_WaitState(pHTuner->hAbr, DASH_STATE_READY) == DASH_ERR_ERROR)
				{
					PrintError ("%s(%d) Error> There are some trouble while task running !!\n",__FUNCTION__,__LINE__);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}

				if(DI_STREAM_DASH_SetState(pHTuner->hAbr, DASH_STATE_PREBUFFERING) == DASH_ERR_ERROR)
				{
					PrintError ("%s(%d) Error> There are some trouble while prebuffering !!\n",__FUNCTION__,__LINE__);
									nRet = DI_ERR_ERROR;
									PrintExit;
									return nRet;
				}

				/* waiting for DASH ready - After prebuffering compeletion or suspended session */
				if(DI_STREAM_DASH_WaitState(pHTuner->hAbr, DASH_STATE_READY) == DASH_ERR_ERROR)
				{
					PrintError ("%s(%d) Error> There are some trouble while task running !!\n",__FUNCTION__,__LINE__);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}

				(void) DI_STREAM_DASH_RegisterCallback ((const char*)pHTuner->pUrl);

				/* if suspended by multi-session processing, switch on buffering state */
				if(((MEDIA_DASH_HANDLE_t)pHTuner->hAbr)->IOmode == DASH_IO_TYPE_PLAYBACK)
				{
					//DI_STREAM_DASH_RestoreState(pHTuner->hAbr);
					DI_STREAM_DASH_SetState(pHTuner->hAbr, DASH_STATE_BUFFERING);
				}

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
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Protocol !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}
	PrintExit;
	return nRet;
}


DI_ERR_CODE DI_IPTUNER_AdaptiveClose (DI_IPTUNER_t *pHTuner)
{
	HINT32	nRet = DI_ERR_OK;
	PrintEnter;
	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError ("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* check Tuner url */
	if ( NULL == pHTuner->pUrl)
	{
		PrintError ("%s(%d) Error> Tuner Handle has invalid url (pUrl is NULL)\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	PrintData ("%s(%d) TunerHandle=[%p] URL=[%s]\n"
		,__FUNCTION__
		,__LINE__
		, pHTuner
		, pHTuner->pUrl);

	switch (pHTuner->eProtocolType)
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_PROTOCOL_SSTR : /* for SSTR */
			DI_STREAM_SSTR_UnregisterCallback((const char*)pHTuner->pUrl);
			break;
#endif
		case DI_IPTUNER_PROTOCOL_HLS : /* for HLS  */
			DI_STREAM_HLS_UnregisterCallback((const char*)pHTuner->pUrl);
			break;
#ifdef CONFIG_MEDIA_DASH
		case DI_IPTUNER_PROTOCOL_DASH : /* for HLS  */
			DI_STREAM_DASH_UnregisterCallback((const char*)pHTuner->pUrl);
			if(DI_STREAM_DASH_GetState(pHTuner->hAbr) != DASH_STATE_FINALIZEOK
				&& DI_STREAM_DASH_GetState(pHTuner->hAbr) != DASH_STATE_ERROR
				&& DI_STREAM_DASH_GetState(pHTuner->hAbr) != DASH_STATE_FINALIZE)
			{
				DI_STREAM_DASH_SetState(pHTuner->hAbr, DASH_STATE_RESET);
			}
			//DI_STREAM_DASH_WaitState(pHTuner->hAbr, DASH_STATE_READY);
			break;
#endif
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_AdaptiveDestroy (DI_IPTUNER_t *pHTuner)
{
	HINT32	nRet = DI_ERR_OK;
	PrintEnter;
	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError ("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* check Tuner url */
	if ( NULL == pHTuner->pUrl)
	{
		PrintError ("%s(%d) Error> Tuner Handle has invalid url (pUrl is NULL)\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	PrintData ("%s(%d) TunerHandle=[%p] URL=[%s]\n"
		,__FUNCTION__
		,__LINE__
		, pHTuner
		, pHTuner->pUrl);

	switch (pHTuner->eProtocolType)
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_PROTOCOL_SSTR : /* for SSTR */
			if(pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_COMPLETE
			|| pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_STOP )
			{
				pHTuner->bExitDownload = TRUE;
			}
			while(pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_COMPLETE
			&& pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_STOP )
			{
				VK_TASK_Sleep(100);
			}
			if(pHTuner->hAbr != NULL)
			{
				(void)DI_STREAM_SSTR_DestroyLiveUpdateTask(pHTuner); /* Warning Blocking */
#if defined(DI_IPTUNER_SSTR_CONFIG_TEXTSTREAM)
				(void)DI_STREAM_SSTR_DestroyTextStreamTask(pHTuner);
#endif
				(void)DI_STREAM_SSTR_CloseController(pHTuner->hAbr);
				pHTuner->hAbr = NULL;
			}
			break;
#endif
		case DI_IPTUNER_PROTOCOL_HLS : /* for HLS  */
			if(pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_COMPLETE
			|| pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_STOP )
			{
				pHTuner->bExitDownload = TRUE;
			}
			while(pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_COMPLETE
			&& pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_STOP )
			{
				VK_TASK_Sleep(100);
			}
			if(pHTuner->hAbr != NULL)
			{
				(void)DI_STREAM_HLS_DestroyLiveUpdateTask(pHTuner); /* Warning Blocking */
				(void)DI_STREAM_HLS_CloseController(pHTuner->hAbr);
				pHTuner->hAbr = NULL;
			}
			break;
#ifdef CONFIG_MEDIA_DASH
		case DI_IPTUNER_PROTOCOL_DASH :
			if(pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_COMPLETE
			|| pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_STOP )
			{
				pHTuner->bExitDownload = TRUE;
			}
			while(pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_COMPLETE
			&& pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_STOP )
			{
				VK_TASK_Sleep(100);
			}
			if(pHTuner->hAbr != NULL)
			{
				(void)DI_STREAM_DASH_CloseController(pHTuner->hAbr);
				pHTuner->hAbr = NULL;
			}
			break;
#endif
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}

	pHTuner->bAbrTaskQuit = TRUE;

	if (pHTuner->bAbrTaskInit == TRUE)
	{
		{
			DI_IPTUNER_Msg_t s_Msg;
			PrintData ("%s (%d) Buffering Stop from OpenNexusCallback\n",__FUNCTION__,__LINE__);
			(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));
			s_Msg.unMsg = TUNER_ADAPTIVE_MSG_STOP_BUFFERING;
//			s_Msg.unParam1 = pHTuner;
			(void) VK_MSG_Send ( pHTuner->ulAbrTaskMsgQueue , (HUINT32 *)&s_Msg, sizeof(DI_IPTUNER_Msg_t) ) ;
		}

		while (pHTuner->bAbrTaskFinished != TRUE)
		{
			VK_TASK_Sleep (100);
		}

		(void) VK_TASK_Destroy (pHTuner->ulAbrTaskId);

		pHTuner->bAbrTaskInit = FALSE;

		(void) VK_SEM_Destroy (pHTuner->ulAbrTaskSema);

		(void) VK_MSG_Destroy (pHTuner->ulAbrTaskMsgQueue);
	}
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_AdaptiveGetInfo (DI_IPTUNER_t* pHTuner, DI_IPTUNER_Info_t *pInfo)
{
	HINT32	nRet = DI_ERR_OK;

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError ("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* check Tuner url */
	if ( NULL == pHTuner->pUrl)
	{
		PrintError ("%s(%d) Error> Tuner Handle has invalid url (pUrl is NULL)\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	PrintData ("%s(%d) TunerHandle=[%p] URL=[%s]\n"
		,__FUNCTION__
		,__LINE__
		, pHTuner
		, pHTuner->pUrl);

	switch (pHTuner->eProtocolType)
	{
#if 0
#ifdef CONFIG_MEDIA_SSTR
			case DI_IPTUNER_PROTOCOL_SSTR :
				pInfo->adaptiveInfo.pSSTRInfo = DI_STREAM_SSTR_GetManifestInfo(pHTuner->hAbr);
				if(pInfo->adaptiveInfo.pSSTRInfo == NULL)
				{
					return DI_ERR_ERROR;
				}
				break;
#endif
#endif
			default  :
				/* Caution to warning: unused parameter 'pInfo' */
				/* Use UNUSED_PARAM() macro carefully */
				UNUSED_PARAM (pInfo);
				PrintError ("%s(%d) Error>	-  Not Supported Tuner(%d)\n",__FUNCTION__,__LINE__,pHTuner->eProtocolType);
				break;
		}
		PrintExit;

	return DI_ERR_OK;
}

/** Download 가 Stop 된 경우 Resume의 용도로 사용한다. */
DI_ERR_CODE DI_IPTUNER_AdaptiveControl (DI_IPTUNER_t* pHTuner, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError ("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* check Tuner url */
	if ( NULL == pHTuner->pUrl)
	{
		PrintError ("%s(%d) Error> Tuner Handle has invalid url (pUrl is NULL)\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	PrintData ("%s(%d) TunerHandle=[%p] URL=[%s]\n"
		,__FUNCTION__
		,__LINE__
		, pHTuner
		, pHTuner->pUrl);

	switch (pHTuner->eProtocolType)
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_PROTOCOL_SSTR :
			nRet = DI_STREAM_SSTR_Control (pHTuner->hAbr,cmd, val, val2);
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_HLS_Control Fail \n",__FUNCTION__,__LINE__);
			}
			/* else { continue; } */
			break;
#endif
		case DI_IPTUNER_PROTOCOL_HLS : /* for HLS  */
			nRet = DI_STREAM_HLS_Control (pHTuner->hAbr,cmd, val, val2);
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_HLS_Control Fail \n",__FUNCTION__,__LINE__);
			}
			/* else { continue; } */
			break;
#ifdef CONFIG_MEDIA_DASH
		case DI_IPTUNER_PROTOCOL_DASH:
			nRet = DI_STREAM_DASH_Control (pHTuner->hAbr, cmd, val, val2);
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_DASH_Control Fail \n",__FUNCTION__,__LINE__);
			}
			break;
#endif
		default  :
			UNUSED_PARAM(cmd);
			UNUSED_PARAM(val);
			UNUSED_PARAM(val2);
			nRet = DI_ERR_INVALID_PARAM;
			PrintError ("%s(%d) Error>  -  Not Supported Tuner(%d)\n",__FUNCTION__,__LINE__,pHTuner->eProtocolType);
			break;
	}
	PrintExit;

	return nRet;
}
