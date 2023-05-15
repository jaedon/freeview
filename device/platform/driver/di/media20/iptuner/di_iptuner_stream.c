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
#include "vfio20.h"

#include "vkernel.h"
#include "taskdef.h"

#include "drv_media20.h"
#include "di_media20.h"
#include "di_uart.h"

#include "util_url.h"
#include "di_iptuner20.h"
#include "di_iptuner_stream.h"
#include "di_iptuner_http.h"
#include "di_iptuner_mms.h"
#include "di_iptuner_adaptive.h"
#include "di_iptuner_rtsp.h"
#include "di_iptuner_inetradio.h"
#include "di_iptuner_cls_monitor.h"
#ifdef USE_IPTUNER_ABR /* Do not use this define, "ABR" is not supported yet. MUST remove */
#include "di_iptuner_abr.h"
#endif


/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */

//#define DI_IPTUNER_STREAM_DEBUG

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
HULONG s_ulStreamMsgQId;
HULONG s_ulBufSema = -1;
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

#if __________di__________
#endif
DI_ERR_CODE DI_IPTUNER_StreamProbe (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE	nRet = DI_ERR_OK;
	/* HBOOL bIsIcy = FALSE; */
	/* HUINT64 uStreamSize = 0; */

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
		case DI_IPTUNER_PROTOCOL_HTTP	:
		case DI_IPTUNER_PROTOCOL_HTTPS	:
			{
				/* Common RealTime Mode */
				(void) DI_STREAM_HTTP_RegisterCallback ((char*)pHTuner->pUrl);
				nRet = DRV_MEDIA_Probe (pHTuner, (HUINT8 *)pHTuner->pUrl, &pHTuner->tMediaInfo );
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
#if 0
					/* get httpErrorCode */
					(void) DI_STREAM_HTTP_GetProtoStatusCode (&pHTuner->tMediaInfo.protoStatus);
#endif
					(void) DI_STREAM_HTTP_UnregisterCallback ((const char*)pHTuner->pUrl);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}

				(void) DI_STREAM_HTTP_UnregisterCallback ((const char*)pHTuner->pUrl);
#if defined (CONFIG_PLAYREADY)
/*************************
*	Parse & get the WRM Header
************************/
				if(pHTuner->tMediaInfo.stream_type == DI_MEDIA_MPEG_TYPE_ASF)
				{
					(void)DI_STREAM_HTTP_GetASFWrmHeader(pHTuner);
					(void)DI_STREAM_HTTP_GetSimpleIndex(pHTuner);
				}
#endif
			}
			/* else { continue; } */
			break;
		case DI_IPTUNER_PROTOCOL_SSTR	:
		case DI_IPTUNER_PROTOCOL_HLS	:
		case DI_IPTUNER_PROTOCOL_DASH	:
			nRet = DI_IPTUNER_AdaptiveProbe (pHTuner);
			if (nRet != DI_ERR_OK)
			{
				DI_IPTUNER_AdaptiveClose(pHTuner);
				PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;

		case DI_IPTUNER_PROTOCOL_ABR :
#ifdef USE_IPTUNER_ABR
			if(NULL!= VK_strstr((const char*)pHTuner->pUrl,".mpd") || NULL!= VK_strstr((const char*)pHTuner->pUrl,".xml"))
			{
				(void) DI_STREAM_ABR_RegisterProbeCallback ((const char*)pHTuner->pUrl);
				nRet = DRV_MEDIA_Probe (pHTuner, pHTuner->pUrl, &pHTuner->tMediaInfo );
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
#if 0
					/* get httpErrorCode */
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
#endif
			{
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
		case DI_IPTUNER_PROTOCOL_MMS		:
			nRet = DI_STREAM_MMS_Probe (pHTuner->pUrl,&pHTuner->tMediaInfo);
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
		case DI_IPTUNER_PROTOCOL_RTSP	:
			/* Use static probeInfo because currently media_probe is unavailable thru RTSP */
			if (1)
			{
				nRet = DI_STREAM_RTSP_Probe (pHTuner->pUrl,&pHTuner->tMediaInfo);
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
					return nRet;
				}
			}
			else
			{
				(void) DI_STREAM_RTSP_RegisterCallback ((char*)pHTuner->pUrl);
				nRet = DRV_MEDIA_Probe (pHTuner, (HUINT8 *)pHTuner->pUrl, &pHTuner->tMediaInfo );
				if (nRet != DI_ERR_OK)
				{
					PrintError ("%s(%d) Error> Probing Fail !!\n",__FUNCTION__,__LINE__);
					(void) DI_STREAM_RTSP_UnregisterCallback ((const char*)pHTuner->pUrl);
					nRet = DI_ERR_ERROR;
					PrintExit;
					return nRet;
				}
				(void) DI_STREAM_RTSP_UnregisterCallback ((const char*)pHTuner->pUrl);
			}
			break;
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}

	PrintExit;

	return nRet;
}

/*******************************************************************/
/************************ public funtions **************************/
/*******************************************************************/
DI_ERR_CODE DI_IPTUNER_StreamOpen (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE	nRet = DI_ERR_OK;
	/* HBOOL bIsIcy = FALSE; */
	/* HUINT64 uStreamSize = 0;*/

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
		case DI_IPTUNER_PROTOCOL_HTTP	:
		case DI_IPTUNER_PROTOCOL_HTTPS	:
			switch(pHTuner->eInetRadioType)
			{
				case DI_IPTUNER_INETRADIO_TYPE_ICY:
					DI_STREAM_INETRADIO_ICY_RegisterCallback ((char *)pHTuner->pUrl);
					break;
				case DI_IPTUNER_INETRADIO_TYPE_GENERIC:
					DI_STREAM_INETRADIO_Generic_RegisterCallback ((char *)pHTuner->pUrl);
					break;
				default:
					/* Common RealTime Mode */
					(void) DI_STREAM_HTTP_RegisterCallback ((char *)pHTuner->pUrl);
					break;
			}

			if ((pHTuner->bHttpChunked == TRUE) && (pHTuner->bRangeAccepted == FALSE) && (!pHTuner->tDlnaSetting.bDlna))
			{
				pHTuner->bUseClsMonitor = TRUE;
				DI_IPTUNER_CLS_MonitorStart(pHTuner);
			}
			else
			{
				pHTuner->bUseClsMonitor = FALSE;
			}

			break;
		case DI_IPTUNER_PROTOCOL_SSTR	:
		case DI_IPTUNER_PROTOCOL_HLS	:
		case DI_IPTUNER_PROTOCOL_DASH	:
			nRet = DI_IPTUNER_AdaptiveOpen (pHTuner);
			break;
		case DI_IPTUNER_PROTOCOL_ABR :
#ifdef USE_IPTUNER_ABR
			if(NULL!= VK_strstr(pHTuner->pUrl,".mpd") || NULL!= VK_strstr(pHTuner->pUrl,".xml"))
			{
				(void) DI_STREAM_ABR_RegisterCallback (pHTuner->pUrl);
				nRet = DI_ERR_OK;
			}
			else
#endif
			{
				nRet = DI_ERR_ERROR;
				PrintExit;
				return nRet;
			}
			break;
		case DI_IPTUNER_PROTOCOL_MMS		:
			DI_STREAM_MMS_RegisterCallback ((char *)pHTuner->pUrl);
			/* else { continue; } */
			break;

		case DI_IPTUNER_PROTOCOL_RTSP	:
			(void) DI_STREAM_RTSP_RegisterCallback ((char *)pHTuner->pUrl);
			break;
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}

	PrintExit;

	return nRet;
}


DI_ERR_CODE DI_IPTUNER_StreamClose (DI_IPTUNER_t *pHTuner)
{
	HINT32	nRet = DI_ERR_OK;
	PrintEnter;

	switch (pHTuner->eProtocolType)
	{
		case DI_IPTUNER_PROTOCOL_HTTP	:
		case DI_IPTUNER_PROTOCOL_HTTPS	:
			switch(pHTuner->eInetRadioType)
			{
				case DI_IPTUNER_INETRADIO_TYPE_ICY:
					DI_STREAM_INETRADIO_ICY_UnregisterCallback ((char *)pHTuner->pUrl);
					break;
				case DI_IPTUNER_INETRADIO_TYPE_GENERIC:
					DI_STREAM_INETRADIO_Generic_UnregisterCallback ((char *)pHTuner->pUrl);
					break;
				default:
					(void) DI_STREAM_HTTP_UnregisterCallback ((char *)pHTuner->pUrl);
					break;
			}

			if (pHTuner->bUseClsMonitor == TRUE)
			{
				DI_IPTUNER_CLS_MonitorStop(pHTuner);
			}
			break;
		case DI_IPTUNER_PROTOCOL_ABR :
#ifdef USE_IPTUNER_ABR
			DI_STREAM_ABR_UnregisterCallback (pHTuner->pUrl);
			ABR_MPD_Close(pHTuner->hAbr);
#endif
		case DI_IPTUNER_PROTOCOL_SSTR	:
		case DI_IPTUNER_PROTOCOL_HLS	:
		case DI_IPTUNER_PROTOCOL_DASH	:
			DI_IPTUNER_AdaptiveClose (pHTuner);
			break;
		case DI_IPTUNER_PROTOCOL_MMS		:
			DI_STREAM_MMS_UnregisterCallback ((char *)pHTuner->pUrl);
			break;
		case DI_IPTUNER_PROTOCOL_RTSP	:
			DI_STREAM_RTSP_UnregisterCallback ((char *)pHTuner->pUrl);
			break;
		default :
			PrintError  ("%s(%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			PrintExit;
			return nRet;
	}
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_StreamGetInfo (DI_IPTUNER_t *pHTuner,DI_IPTUNER_Info_t *pInfo)
{
	/* TODO : junsy */
	UNUSED_PARAM (pInfo);
	UNUSED_PARAM (pHTuner);
	return DI_ERR_OK;
}

/** Download 가 Stop 된 경우 Resume의 용도로 사용한다. */
DI_ERR_CODE DI_IPTUNER_StreamControl (DI_IPTUNER_t *pHTuner, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2)
{
	DI_ERR_CODE nRet=DI_ERR_OK;

	PrintEnter;

	switch (pHTuner->eProtocolType)
	{
		case DI_IPTUNER_PROTOCOL_MMS :
			nRet = DI_STREAM_MMS_Control ( cmd, val, val2 );
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_MMS_Control(cmd:%d, val:%llu, val2:%llu) Fail \n",__FUNCTION__,__LINE__, cmd, val, val2);
			}
			/* else { continue; } */
			break;
		case DI_IPTUNER_PROTOCOL_RTSP :
			nRet = DI_STREAM_RTSP_Control ( cmd, val, val2 );
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_RTSP_Control Fail \n",__FUNCTION__,__LINE__);
			}
			/* else { continue; } */
			break;
		case DI_IPTUNER_PROTOCOL_HTTP :
		case DI_IPTUNER_PROTOCOL_HTTPS	:
			/* XXX : DI_STREAM_HTTP_Control is a Dummy function */
			nRet = DI_STREAM_HTTP_Control (cmd, val, val2);
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_STREAM_STREAM_SetSpeed Fail \n",__FUNCTION__,__LINE__);
			}
			/* else { continue; } */
			break;
		case DI_IPTUNER_PROTOCOL_SSTR	:
		case DI_IPTUNER_PROTOCOL_HLS	:
		case DI_IPTUNER_PROTOCOL_DASH	:
			nRet = DI_IPTUNER_AdaptiveControl (pHTuner, cmd, val, val2 );
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> Adaptive Control Fail \n",__FUNCTION__,__LINE__);
			}
			break;
		default  :
			nRet = DI_ERR_INVALID_PARAM;
			PrintError ("%s(%d) Error> Unknown protocol type. \n",__FUNCTION__,__LINE__);
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

#if 0//#ifdef SUPPORT_TITLE
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
