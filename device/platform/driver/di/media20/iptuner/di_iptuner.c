/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:	     IP tuner Main API
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2010 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
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

#include "di_iptuner_config.h"
#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_media20.h"
#include "di_uart.h"
#include "di_iptuner_dnp.h"
#include "di_iptuner_stream.h"
#include "di_iptuner_inetradio.h"
#ifdef USE_IPTUNER_ABR /* Do not use this define, "ABR" is not supported yet. MUST remove */
#include "di_iptuner_abr.h"
#endif
#include "di_iptuner_adaptive.h"
#include "di_iptuner_http.h"
#include "di_iptuner_multicast.h"
#include "di_iptuner20.h"
#include "di_iptuner_pool.h"
#include "di_iptuner_cls_monitor.h"
#include "drv_media20.h"

#include "adp_buffermgr.h"

#include "util_url.h"
#include "vfio20.h"
#include "util_index_parser.h"
#include "util_getaddrinfo.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
#define DI_IPTUNER_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef DI_IPTUNER_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
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

#define UNUSED_PARAM(x)  ((void)x)
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
DI_IPTUNER_CONFIG_t g_Configuration;

#if __________p__________
#endif

/******************************* NEW Implementation for CANVAS ********************/

void P_IPTUNER_Configuration (DI_IPTUNER_CONFIG_t *pConfig)
{
	/* install configuration(DI_IPTUNER_CONFIG_t) the iptuner which is created. */
	/* TODO : Error Checking */
	if(pConfig==NULL)
	{
		/* If pConfig is null we use default values. */
		g_Configuration.bUseStorage = TRUE;
		g_Configuration.nStreamBufferingSecond = 5;
		g_Configuration.nChunkBaseExponent = 3;
	}
	else
	{
		g_Configuration.bUseStorage = pConfig->bUseStorage;
		g_Configuration.nStreamBufferingSecond = pConfig->nStreamBufferingSecond;
		g_Configuration.nChunkBaseExponent = pConfig->nChunkBaseExponent;
	}

	g_Configuration.bInit = FALSE;
	return;
}

void P_IPTUNER_ClearConfiguration (void)
{
	g_Configuration.bInit = FALSE;
	g_Configuration.bUseStorage = 0;
	g_Configuration.nStreamBufferingSecond = 0;
	g_Configuration.nChunkBaseExponent = 0;

	return;
}

/*
 * url string 을 입력받아 iptuner protocol type을 return 한다.
 */
static DI_IPTUNER_ProtocolType_e P_IPTUNER_GetProtocolType (HINT8 *pUrl)
{
	DI_IPTUNER_ProtocolType_e nProtocolType = DI_IPTUNER_PROTOCOL_UNKNOWN;
	URL_t *pTmpUrl=NULL;

	/** Check URI */
	PrintEnter;

	if (pUrl == NULL)
	{
		PrintError ("%s(%d) Error> pUrl is null pointer.\n",__FUNCTION__,__LINE__);
		return nProtocolType;
	}
	if (!VK_strlen(pUrl))
	{
		PrintError ("%s(%d) Error> pUrl don't have any string.\n",__FUNCTION__,__LINE__);
		return nProtocolType;
	}

	pTmpUrl = (URL_t*) VK_MEM_Alloc (sizeof(URL_t));
	if(pTmpUrl == NULL)
	{
		PrintError ("%s(%d) Error> can't allocate URL_t. \n",__FUNCTION__,__LINE__);
		return nProtocolType;
	}

	VK_memset(pTmpUrl, 0 ,sizeof(URL_t));

	UTIL_UrlParse (pTmpUrl, (const char*)pUrl, 0);

	if (!VK_strcmp(pTmpUrl->psz_protocol,"https"))
	{
		nProtocolType = DI_IPTUNER_PROTOCOL_HTTPS;
	}
	else if (!VK_strcmp(pTmpUrl->psz_protocol,"http"))
	{
		nProtocolType = DI_IPTUNER_PROTOCOL_HTTP;
	}
	else if (!VK_strcmp(pTmpUrl->psz_protocol,"mms"))
	{
		nProtocolType = DI_IPTUNER_PROTOCOL_MMS;
	}
	else if (!VK_strcmp(pTmpUrl->psz_protocol,"rtsp"))
	{
		nProtocolType = DI_IPTUNER_PROTOCOL_RTSP;
	}
	else if (!VK_strcmp(pTmpUrl->psz_protocol,"ftp"))
	{
		nProtocolType = DI_IPTUNER_PROTOCOL_FTP;
	}
	else if (!VK_strcmp(pTmpUrl->psz_protocol,"udp"))
	{
		/* Check whether this IP has Class-D range or not */
		if (net_HostAddrIsMulticast(pTmpUrl->psz_host) == TRUE)
		{
			nProtocolType = DI_IPTUNER_PROTOCOL_UDP_MCAST;
		}
		else
		{
			nProtocolType = DI_IPTUNER_PROTOCOL_UDP; /* Currently, unsupported */
		}
	}
	else if (!VK_strcmp(pTmpUrl->psz_protocol,"rtp"))
	{
		/* Check whether this IP has Class-D range or not */
		if (net_HostAddrIsMulticast(pTmpUrl->psz_host) == TRUE)
		{
			nProtocolType = DI_IPTUNER_PROTOCOL_RTP_MCAST;
		}
		else
		{
			nProtocolType = DI_IPTUNER_PROTOCOL_RTP; /* Currently, unsupported */
		}
	}
	else if (!VK_strcmp(pTmpUrl->psz_protocol,"file"))
	{
		nProtocolType = DI_IPTUNER_PROTOCOL_FILE;
	}
	else if (!VK_strcmp(pTmpUrl->psz_protocol,"dvr"))
	{
		nProtocolType = DI_IPTUNER_PROTOCOL_FILE;
	}

	if (nProtocolType == DI_IPTUNER_PROTOCOL_HTTP || nProtocolType == DI_IPTUNER_PROTOCOL_HTTPS)
	{
		if(pTmpUrl->psz_path != NULL)
		{
			if(NULL != VK_strstr(pTmpUrl->psz_path,".mpd") || NULL != VK_strstr(pTmpUrl->psz_path,".xml"))
			{
				//nProtocolType = DI_IPTUNER_PROTOCOL_ABR;
				nProtocolType = DI_IPTUNER_PROTOCOL_DASH;
			}
			else if(NULL != VK_strstr(pTmpUrl->psz_path,".m3u8"))
			{
				nProtocolType = DI_IPTUNER_PROTOCOL_HLS;
			}
			else if(NULL != VK_strstr(pTmpUrl->psz_path,".ism/manifest") || NULL != VK_strstr(pTmpUrl->psz_path,".ism/Manifest")
			|| NULL != VK_strstr(pTmpUrl->psz_path,".isml/Manifest") || NULL != VK_strstr(pTmpUrl->psz_path,".isml/manifest")
			|| NULL != VK_strstr(pTmpUrl->psz_path,".smil/Manifest") || NULL != VK_strstr(pTmpUrl->psz_path,".smil/manifest"))
			{
				nProtocolType = DI_IPTUNER_PROTOCOL_SSTR;
			}
		}
	}

	UTIL_UrlClean (pTmpUrl);

	if(pTmpUrl!=NULL)
	{
		VK_MEM_Free(pTmpUrl);
		pTmpUrl = NULL;
	}

	return nProtocolType ;
}

DI_ERR_CODE P_IPTUNER_ConvertTimeToByteOffset (DI_IPTUNER_t *pHTuner, HUINT32 ulTime , HUINT64 *pEndByte )
{
	return DI_IPTUNER_DnpConvertTimeToByteOffset(pHTuner, ulTime, pEndByte);
}

DI_ERR_CODE P_IPTUNER_UpdateEndTime (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HBOOL bRet=FALSE;
	HUINT32 unMarginTime = 5000; /* msec */
	PrintEnter;

	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) Invalid Tuner Handle .\n",__FUNCTION__,__LINE__,pHTuner);
		nRet = DI_ERR_ERROR;
	}
	else
	{
		pHTuner->ulEndByte = 0;
		/* If this tuner has been probed, The TimeLine would be determined.*/
		if ( pHTuner->bIsProbeDone == TRUE)
		{
#if 1 /* mp3 workaround : 오디오만 play 하는 경우 margin 값이 10초이상 커야함.(측정치)
			설정하기 원하는 endtime 보다 10초 이상의 data 가 (eof제외) file 에 buffering 되어 있지 않으면
			AUDIO_UNDERRUN 으로 인해 bufcomplete 일지라도 제시간 까지 play하지 않음 in BRCM audio playback
			추후 AUDIO or bmedia parser 단에서 필요 data 만큼만 처리하도록 보완처리되어야 함.
			현재 margin 값으로 인해 bufferstatus 조회시 endtime 에 해당 하는 data 보다 margin 만큼 많이 버퍼링함.*/
			if (pHTuner->tMediaInfo.video_pes_id == 0
				|| pHTuner->tMediaInfo.audio_pes_id )
				unMarginTime += 10000;
#endif
			if(pHTuner->ulEndTime == 0)
				nRet = P_IPTUNER_ConvertTimeToByteOffset (pHTuner, pHTuner->tMediaInfo.duration + unMarginTime , &pHTuner->ulEndByte);
			else
				nRet = P_IPTUNER_ConvertTimeToByteOffset (pHTuner, pHTuner->ulEndTime + unMarginTime , &pHTuner->ulEndByte);

			if(nRet != DI_ERR_OK)
			{
				PrintError ("%s(%d) Error> Session(0x%x) Can't set endtime to iptuner \n",__FUNCTION__,__LINE__,pHTuner);
			}

			bRet = DI_IPTUNER_DnpSetOLTEndTime (pHTuner,pHTuner->ulEndByte);
			if (bRet != TRUE)
			{
				PrintError ("%s(%d) Error> Session(0x%x) Can't set endoffset to OLT table. \n",__FUNCTION__,__LINE__,pHTuner);
			}
		}
		/* If not probed, the endbyte should be updated and calculated after probing. */
		else
		{
			PrintDebug ("%s(%d) Info> Session(0x%x) This Media doesn't probed yet, So The endTime will be updated after probing.\n",__FUNCTION__,__LINE__,pHTuner);
		}
	}

	PrintExit;
	return nRet;
}

/* System 시작시 한번 호출되며 IPTUNER Pool 을 생성한다. */
DI_ERR_CODE DI_IPTUNER_Init ( DI_IPTUNER_CONFIG_t *pConfig )
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if (g_Configuration.bInit == TRUE)
	{
		PrintError ("%s(%d) Error> IPTUNER already initialized. Please check this!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	/* Create ip pool.
	 * This pool must be created only one-time in this system until uninitialized.*/
	nRet = DI_IPTUNER_POOL_Init ();
	if (DI_ERR_OK == nRet)
	{
		/* configure tuner */
		(void) P_IPTUNER_Configuration (pConfig);
	}
	else
	{
		PrintError ("%s(%d) Error> DI_IPTUNER_POLL_Init Faild.\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return nRet;
}

/* System 종료시 한번 호출되어 생성되어 있는 모든 IPTUNER list 을 삭제한다. */
DI_ERR_CODE DI_IPTUNER_Uninit (void)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;
	nRet = DI_IPTUNER_POOL_Uninit ();
	if (DI_ERR_OK == nRet)
	{
		P_IPTUNER_ClearConfiguration ();
	}
	else
	{
		PrintError ("%s(%d) Error> DI_IPTUNER_POLL_Uninit Faild.\n",__FUNCTION__,__LINE__);
	}

	PrintExit;
	return nRet;
}

/* 하나의 IPTUNER 를 생성한다.
 * 생성된 IPTUNER 는 고유의 handle 을 가지며 IPTUNER Pool 에 등록된다.
 * 입력되는 URL 을 통해 IPTUNER 의 type이 결정된다. (default : streaming 방식)
 *
 * [in] HINT8 *url
 * [out] DI_IPTUNER_t *pHandle
 *
 * malloc members.
 * 	- pHTuner->pUrl 할당. ( pHtuner <- vfioHandle <- dnpUserData 순으로 종속된다.)
 * 	- pHTuner->pVfioHandle 할당.
 *
 * example url : mms://username:password@xxx.xxx.xxx.xxx:port_number/PublishingPoint1/test.asf
 *
 * pHTuner->member 초기화 및 type 결정
 *
 * 2011.08.18 junsy
 *   : URL 이 더이상 Unique 한 값이 되지 않음. (by YouView)
 *     URL 을 Key 로 iptuner handle 을 찾을 수 없게 됨. (같은 URL 을 동시에 Probing 불가능)
 *     따라서 IPTUNER 가 가지는 pUrl member 는 URL 값 앞에 "{TID=tunerId}"값을 지정하여 Unique 한 URL String 으로
 *     간주한다. 이 URL은 Stream Handler 로 전달시에 반드시 삭제되어야 하는 String 으로 사용한다.
 *
 *     VFIO Function 에서 (특히 Open) string 으로만 전달되는 URL(modified) 로
 *     해당 TUNER 를 찾을수 있어야만 동작이 가능하다.
 *
 *     ex) http://a 를 probing 도중에 다른 tuner 가 만들어져 같은 url 로 probing 하는경우에
 *         VFIO Open 에는 동일한 URL 이 전달되어 iptuner pool 또는 vfio 의 pool 에서
 *         원하는 tuner handler 을 찾을 수 없다. 즉 동일 url 이 여러개 저장되어 있기 때문.
 *
 *     따라서 다음과 같은 URL form 을 사용하도록 한다.
 *
 *     http://sample.url/a.ts => {TID=15716536}http://sample.url/a.ts
 *     이때 사용되는 TID 의 value 는 tuner handler pointer 의 unsigned int 값을 사용한다.
 *
 *     URL 을 DI_IPTUNER 의 main controller 단에서 사용 시 그대로 사용가능하지만
 *     Stream Core 로 전달시에는 삭제하고 전달해야 한다. (include UTIL_Url_Parser 포함)
 *
 */
DI_ERR_CODE DI_IPTUNER_Create (HINT8 *pUrl, DI_IPTUNER_t ** pHTuner)
{
	DI_IPTUNER_t *pTempTuner = NULL;
	DI_IPTUNER_ProtocolType_e eProto = DI_IPTUNER_PROTOCOL_UNKNOWN;

	HINT32 nVKRet;

	PrintEnter;

	PrintDebug ("%s(%d) : IPTUER Create URL[%s] \n",__FUNCTION__,__LINE__,pUrl);
	if(pUrl == NULL)
	{
		PrintError ("%s(%d) Error> Invalid URL Pointer. \n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	/* 1. Allocate iptuner */
	pTempTuner = DI_IPTUNER_POOL_AllocTuner ();
	if ( NULL == pTempTuner)
	{
		PrintError ("%s(%d) Error> DI_IPTUNER_POLL_AllocTuner Faild.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	/* 2. setting parameters
	 *		default download type = streaming
	 *		taskid	0
	 *		filepath null
	 */
	pTempTuner->pUrl = (HINT8*) UTIL_AllocUniqueURL ((unsigned int) pTempTuner ,(const char *) pUrl );
	if(pTempTuner->pUrl == NULL)
	{
		PrintError ("%s(%d) Error> UTIL_AllocUniqueURL Faild.\n",__FUNCTION__,__LINE__);
		(void) DI_IPTUNER_POOL_FreeTuner (pTempTuner);
		return DI_ERR_ERROR;
	}

	/* decision protocol type */
	eProto = P_IPTUNER_GetProtocolType (pTempTuner->pUrl);

	/* Check validation of url. */
	if(eProto == DI_IPTUNER_PROTOCOL_UNKNOWN)
	{
		(void) DI_IPTUNER_POOL_FreeTuner (pTempTuner);
		PrintError ("%s(%d) Error> URL[%s] is not supported by IPTUNER \n",__FUNCTION__,__LINE__,pUrl);
		return DI_ERR_ERROR;
	}
	else if (eProto == DI_IPTUNER_PROTOCOL_FILE)
	{
		pTempTuner->eDownloadType = DI_IPTUNER_FILE;
	}
	else if ((eProto == DI_IPTUNER_PROTOCOL_UDP)
		|| (eProto == DI_IPTUNER_PROTOCOL_RTP))
	{
		/* UDP/unicast or RTP/unicast is unsupported */
		(void) DI_IPTUNER_POOL_FreeTuner (pTempTuner);
		PrintError ("%s(%d) Error> URL[%s] is not supported by IPTUNER (proto:%d) \n",__FUNCTION__,__LINE__,pUrl, eProto);
		return DI_ERR_ERROR;
	}
	else if ((eProto == DI_IPTUNER_PROTOCOL_UDP_MCAST)
		|| (eProto == DI_IPTUNER_PROTOCOL_RTP_MCAST))
	{
		/* Checking class-D address already done */
		pTempTuner->eDownloadType = DI_IPTUNER_MULTICAST;
	}
	else
	{
		pTempTuner->eDownloadType = DI_IPTUNER_STREAMING;
	}

	pTempTuner->eProtocolType 		= eProto;
	pTempTuner->eProbeStatus 		= DI_IPTUNER_PROBE_INIT;
	pTempTuner->eBufferingStatus  	= DI_IPTUNER_BUFFERING_STOP;
	pTempTuner->eBufferingMode    = DI_IPTUNER_MODE_THRESHOLD;
	pTempTuner->bOverFlow = FALSE;
	pTempTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_INIT;
	pTempTuner->eUnderflowMode = DI_IPTUNER_UNDERFLOW_THRESHOLD;

	/* initialization VFIO Parameter */
	if (pTempTuner->eDownloadType != DI_IPTUNER_MULTICAST)
	{
		pTempTuner->bUseVfio     = TRUE;
		pTempTuner->pVfioHandle  = (void *)VK_MEM_Alloc(sizeof(VFIO_Handle_t));
		VK_memset (pTempTuner->pVfioHandle, 0, sizeof(VFIO_Handle_t));
		pTempTuner->bIsProbeNeeded   = TRUE;
		pTempTuner->bIsProbeDone     = FALSE;
	}
	else
	{
		/* Multicast don't use VFIO and media_probe */
		pTempTuner->bUseVfio     = FALSE;
		pTempTuner->pVfioHandle  = NULL;
		pTempTuner->bIsProbeNeeded   = FALSE;
		pTempTuner->bIsProbeDone     = FALSE ;
	}

	pTempTuner->ulIPTUNERSema      = 0 ;

	/* initialization DNP Parameter */
	pTempTuner->ulDNPTaskId      = 0 ;
	pTempTuner->ulDelayedStartTaskId = 0;
	pTempTuner->ulDNPSema        = 0 ;
	pTempTuner->ulDNPWriteSema   = 0 ;
	pTempTuner->ulDNPMsgQId      = 0 ;
	pTempTuner->bDNPTaskFinished = FALSE ;
	pTempTuner->bDNPTaskQuit     = FALSE ;
	pTempTuner->ulDNPFileLength  = 0 ;			/* Content-Length or Total Content File size */
	pTempTuner->bIsPlaybackStop  = 0 ;
	pTempTuner->bUnderflow       = FALSE;
	pTempTuner->uiChunksToBuffer = 0;
	pTempTuner->ullPauseCheckOffset = 0;
	pTempTuner->tPumpBufferCallback = NULL;
	pTempTuner->tVideoBufferCallback = NULL;
	pTempTuner->tAudioBufferCallback = NULL;

#if 1 /* dragon test */
	pTempTuner->bEscapeUnderflow = FALSE;
	pTempTuner->bDelayedSeek     = FALSE;
#endif

	/* Defaut buffering parameters */
	pTempTuner->ulStartThreshold			= 10000 ;	/* default : 10 secs = 10000 msecs */
	pTempTuner->ulStartConstant            = 250 ;		/* default : 250 msecs */
	pTempTuner->nBufferThreshold           = 10000 ;	/* default : 10 secs = 10000 msecs */
	pTempTuner->bFastFillIsLimited         = FALSE;		/* default : false (unlimited) */
	pTempTuner->dFastFillLimitMultiple     = 3.0f;
	pTempTuner->dManagedFillLimitMultiple  = 1.2f;

	pTempTuner->pBufferCallbacks           = NULL;
	pTempTuner->pPlayerControlCallbacks    = NULL;
	pTempTuner->pProbeCallbacks            = NULL;
	pTempTuner->tDelayedStartCallback      = NULL;
	pTempTuner->tDelayedSeekCallback       = NULL;
	pTempTuner->tErrorEventCallback        = NULL;

	pTempTuner->bForceUpdateBP = FALSE;

	pTempTuner->bErrorExit       = FALSE ;
	pTempTuner->bIsProbeDone     = FALSE ;
	pTempTuner->nData            = 0;
	pTempTuner->nProbe           = 0;
	pTempTuner->nIndex           = 0;
	pTempTuner->bVFIOTypeSet	 = FALSE;
	pTempTuner->eTimeLine        = DI_IPTUNER_TIMELINE_STREAM ;
	pTempTuner->pIndexTimeLineUrl= NULL;
	pTempTuner->pIndexRoom       = NULL;
	pTempTuner->nIndexRoomSize   = 0;
	pTempTuner->pUserAgentString = NULL;
	pTempTuner->nTimeLineBPS     = 0;
	pTempTuner->ulEndTime	     = 0;
	pTempTuner->ulEndByte	     = 0;
#if 0
	pTempTuner->eExitReason      = eIPTUNER_DNP_REASON_INIT;
#endif

	pTempTuner->unFirstPts = 0;
	pTempTuner->bPtsInstalled = FALSE;

	pTempTuner->pBufferCallbacks = NULL;
	pTempTuner->pPlayerControlCallbacks = NULL;
	pTempTuner->pProbeCallbacks  = NULL;

	pTempTuner->bHttpChunked             = FALSE;
	pTempTuner->bRangeAccepted           = TRUE;
	pTempTuner->bUseClsMonitor           = FALSE;
	pTempTuner->ulClsMonitorQid          = 0x0;
	pTempTuner->ulClsMonitorTid          = 0x0;
	pTempTuner->eClsBufferState          = TRUE;
	pTempTuner->eClsMonitorStatus        = DI_IPTUNER_CLS_MONITOR_STOPPED;
	pTempTuner->tLiveBufferStateCallback = NULL;

	VK_memset (&pTempTuner->tMediaInfo,0,sizeof(DI_MEDIA_INFO_t));

	pTempTuner->pSaveFileName = NULL;
	pTempTuner->pOLT = NULL;

	/* Multicast and TS Injector related... */
	/* Change to union by protocol */
	pTempTuner->pChannelCallbacks       = NULL;
	pTempTuner->bUseTSInjInstance       = FALSE;
	pTempTuner->bUseTSInjTask           = FALSE;
	pTempTuner->uTSInjID                = 0;
	pTempTuner->ulTSInjTaskId           = 0;
	pTempTuner->eChannelType            = DI_MEDIA_CHANNEL_TUNER_TYPE_AV;
	pTempTuner->pTSInjectorCallbacks    = NULL;
	pTempTuner->eInjectorStatus         = DI_IPTUNER_TSINJECTOR_STATUS_STOP;
	pTempTuner->eMulticastJoinStatus    = DI_IPTUNER_MULTICAST_STATUS_LEAVE;

/*************************
*	Default buffer size
*************************/
	pTempTuner->uiBlockSize = DEFAULT_DATA_BLOCK_SIZE;
	pTempTuner->uiRambufferSize = DEFAULT_RAM_BUFFER_SIZE;
	pTempTuner->uiProbebufferSize = DEFAULT_PROBE_BUFFER_SIZE;

	/* Create Semaphore : ProbeCancel/DestroySession */
	nVKRet = VK_SEM_Create (&pTempTuner->ulIPTUNERSema, "IPTUNER_SEMA", VK_SUSPENDTYPE_PRIORITY);
	if ( VK_OK != nVKRet )
	{
		(void) DI_IPTUNER_POOL_FreeTuner (pTempTuner);
		PrintError ("%s(%d) Error> Closing semaphore creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
		return DI_ERR_ERROR;
	}

	*pHTuner = pTempTuner;

	/* link with VfioHandle of Tuner Handle */
	PrintExit;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_Probe (DI_IPTUNER_t *pHTuner )
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	DI_IPTUNER_DownloadType_e eDown;

	PrintEnter;
	/*******************************************
	 * Check param  & tuner state
	 *******************************************/
	if ( NULL == pHTuner )
	{
		PrintError ("%s (%d) Error> invalid tuner handle (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
	if(	VK_SEM_Get (pHTuner->ulIPTUNERSema) != VK_OK)
	{
		PrintError ("%s (%d) Error> Get IPTuner Semaphore Fail !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
	if(pHTuner->bIsProbeNeeded == FALSE)
	{
		PrintError ("%s (%d) Error> Session(0x%x) Probe is unsupported!! \n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_INVALID_PARAM;
	}

	if(pHTuner->bIsProbeDone == TRUE)
	{
		PrintError ("%s (%d) Error> Session(0x%x) This tuner is already probed!! \n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_INVALID_PARAM;
	}

	if ( NULL == pHTuner->pUrl )
	{
		PrintError ("%s (%d) Error> Session(0x%x)  pHTuner->pUrl is NULL !\n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_INVALID_PARAM;
	}

	pHTuner->eProbeStatus = DI_IPTUNER_PROBE_START;
	pHTuner->bProbeCancel = FALSE;
	eDown = pHTuner->eDownloadType;


	switch (eDown)
	{
		case 	DI_IPTUNER_DOWNLOADING :
				/* Downloading tuner also use realtime probing. */
		case 	DI_IPTUNER_STREAMING :
			nRet = DI_IPTUNER_StreamProbe (pHTuner);
			break;
		case 	DI_IPTUNER_FILE :
			nRet = DRV_MEDIA_Probe (pHTuner, (HUINT8 *)pHTuner->pUrl,&pHTuner->tMediaInfo);
			break;
		default	:
			PrintError ("%s(%d) Error> Session(0x%x) %d downloadtype %d protocol is not supported. \n"
				,__FUNCTION__
				,__LINE__
				,pHTuner
				,pHTuner->eDownloadType
				,pHTuner->eProtocolType);
			nRet = DI_ERR_ERROR;
			break;
	}


	/*
	 * OK!!
	 * Anyway we tried to probe the url is delivered.
	 * We have to check a validation weather it probed correctly or not.
	 * After check the validation, the StartBuffering should emit the signal to upper layer.
	 */
#if 1
	if ( (nRet != DI_ERR_OK)
		|| (pHTuner->bProbeCancel == TRUE))
#else
	if ( nRet != DI_ERR_OK)
#endif
	{
		/* Update tuner status */
		pHTuner->bIsProbeDone = FALSE;

		/* Send Probing Failure Event */
		if(pHTuner->pProbeCallbacks != NULL)
		{
			if(pHTuner->pProbeCallbacks->cbProbeFailure != NULL)
			{
				pHTuner->pProbeCallbacks->cbProbeFailure ((HUINT32)pHTuner);
			}
		}
		else
		{
			PrintError("%s (%d) Warning> Session(0x%x) Probe Failure!! But doesn't send an Event because we don't have any callbacks !\n"
			,__FUNCTION__,__LINE__,pHTuner);
		}
		/* we should return the error value to upper layer */
		nRet= DI_ERR_ERROR;
	}
	else
	{
		/*************************************************************
		 * YouView Timeline : StreamItSelf, IndexFile, BytesPerSecond
		 *************************************************************/
		if(pHTuner->eTimeLine == DI_IPTUNER_TIMELINE_INDEX_FILE)
		{
			/* TODO : parsing index file and store as structure in iptuner handle.
			 *
			 * After probing the media file (ts),
			 * The Tuner should prepare a structure parsed by index-file format.
			 *
			 * A Media Player must be able to get the byte-offset asap in indexfile timeline mode.
			 */
			if ( DI_IPT_SUCCESS != DI_STREAM_HTTP_GetSmallFile ( pHTuner->pIndexTimeLineUrl
																, pHTuner->pUserAgentString
				                                                , &pHTuner->pIndexRoom
																, &pHTuner->nIndexRoomSize) )
			{
				/* switchover to DI_IPTUNER_TIMELINE_STREAM */
				if(pHTuner->pIndexTimeLineUrl != NULL)
				{
					VK_MEM_Free (pHTuner->pIndexTimeLineUrl);
					pHTuner->pIndexTimeLineUrl = NULL;
				}

				if(pHTuner->pIndexRoom != NULL)
				{
					DD_MEM_Free (pHTuner->pIndexTimeLineUrl);
					pHTuner->pIndexTimeLineUrl = NULL;
				}

				pHTuner->eTimeLine == DI_IPTUNER_TIMELINE_STREAM;
				/* TODO : Emit index file error */
			}
		}

		/* Update tuner status */
		pHTuner->bIsProbeDone = TRUE;
		/* We don't need to check the error about below update */
		nRet = P_IPTUNER_UpdateEndTime (pHTuner);
		if(nRet != DI_ERR_OK)
		{
			PrintError ("%s(%d) Error> Session(0x%x) can't update EndTime !! \n",__FUNCTION__,__LINE__,pHTuner);
			nRet = DI_ERR_OK;
		}

		(void) DRV_MEDIA_PrintProbeInfo (&pHTuner->tMediaInfo);

		/* Send Probing Complete Event */
		if(pHTuner->pProbeCallbacks != NULL)
		{
			if(pHTuner->pProbeCallbacks->cbProbeComplete != NULL)
			{
				pHTuner->pProbeCallbacks->cbProbeComplete ((HUINT32)pHTuner);
			}
		}
		else
		{
			PrintError("%s (%d) Warning> Session(0x%x) Probe Completed!! But doesn't send an Event because we don't have any callbacks !\n",__FUNCTION__,__LINE__,pHTuner);
		}
	}
	pHTuner->eProbeStatus = DI_IPTUNER_PROBE_END;
	VK_SEM_Release (pHTuner->ulIPTUNERSema);
	return nRet;
}
#ifdef CONFIG_DLNA_CERTIFICATION
DI_ERR_CODE DI_IPTUNER_ConnectForResume (DI_IPTUNER_t *pHTuner )
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if ( NULL == pHTuner )
	{
		PrintError ("%s (%d) Error>  invalid tuner handle (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	(void) VK_SEM_Get (pHTuner->ulIPTUNERSema);

	switch(pHTuner->eProtocolType)
	{
		case DI_IPTUNER_PROTOCOL_HTTP :
			DI_IPTUNER_HTTP_ConnectForResume(pHTuner);
			break;
		default:
			break;
	}

	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_DisconnectForPause (DI_IPTUNER_t *pHTuner )
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if ( NULL == pHTuner )
	{
		PrintError ("%s (%d) Error>  invalid tuner handle (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	(void) VK_SEM_Get (pHTuner->ulIPTUNERSema);

	switch(pHTuner->eProtocolType)
	{
		case DI_IPTUNER_PROTOCOL_HTTP :
			DI_IPTUNER_HTTP_DisconnectForPause(pHTuner);
			break;
		default:
			break;
	}

	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
	PrintExit;
	return nRet;
}
#endif

DI_ERR_CODE DI_IPTUNER_ProbeCancel (DI_IPTUNER_t *pHTuner )
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if ( NULL == pHTuner )
	{
		PrintError ("%s (%d) Error>  invalid tuner handle (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

#if 0
	(void) VK_SEM_Get (pHTuner->ulIPTUNERSema);
#endif
	/*******************************************
	 * Check param  & tuner state
	 *******************************************/
	if (pHTuner->bIsProbeNeeded == FALSE)
	{
		PrintError ("%s (%d) Error> Session(0x%x) Probe is not needed!! \n",__FUNCTION__,__LINE__,pHTuner);
#if 0
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
#endif
		return DI_ERR_INVALID_PARAM;
	}

	if ( NULL == pHTuner->pUrl )
	{
		PrintError ("%s (%d) Error> Session(0x%x) pHTuner->pUrl is NULL !\n",__FUNCTION__,__LINE__,pHTuner);
#if 0
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
#endif
		return DI_ERR_INVALID_PARAM;
	}

	if (pHTuner->eProbeStatus == DI_IPTUNER_PROBE_INIT)
	{
		PrintError ("%s (%d) pHTuner->eProbeStatus == DI_IPTUNER_PROBE_INIT\n",__FUNCTION__,__LINE__);
		return DI_ERR_OK;
	}

	/* prevent to duplicated probing. */
	if (pHTuner->bProbeCancel == TRUE)
	{
		PrintError ("%s (%d) Error> Session(0x%x) This Session already canceled probing. !\n",__FUNCTION__,__LINE__,pHTuner);
#if 0
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
#endif
		return DI_ERR_OK;
	}
	/* Back-Off Algo. */
	(void)DI_STREAM_HTTP_ForcedExit ( pHTuner->pHStream );
	pHTuner->bProbeCancel = TRUE;

	/* we need to wait until probing function released.*/
#if 1
	while (pHTuner->eProbeStatus == DI_IPTUNER_PROBE_START)
	{
		PrintError ("%s (%d) Debug> Session(0x%x) Waiting......until probing released.!\n",__FUNCTION__,__LINE__,pHTuner);
		VK_TASK_Sleep(50);
	}
#endif

	pHTuner->eProbeStatus = DI_IPTUNER_PROBE_END;

	/* Update tuner status */
	pHTuner->bIsProbeDone = FALSE;
#if 0
	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
#endif

	return nRet;
}


/* 생성된 IPTUNER handle을 입력받아 해당 IPTUNER를 삭제한 후 IPTUNER Pool 에서 삭제한다.*/
DI_ERR_CODE DI_IPTUNER_Destroy (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	RT_STREAMING_INSTANCE_t *pInstance = NULL;
	ADP_BUFFERMGR_t *pBufferMgr=NULL;
	PrintEnter;

	(void) VK_SEM_Get (pHTuner->ulIPTUNERSema);

	switch(pHTuner->eProtocolType)
	{
		case DI_IPTUNER_PROTOCOL_HTTP :
		case DI_IPTUNER_PROTOCOL_HTTPS :
			pInstance = pHTuner->pRealTimeInstance;
			if(pInstance != NULL)
			{
				if(pInstance->pUrl != NULL)
				{
					VK_MEM_Free(pInstance->pUrl);
					pInstance->pUrl = NULL;
				}
				pBufferMgr = pInstance->pBufferMgr;
				if(pBufferMgr != NULL)
				{
					ADP_BUFFERMGR_Destroy(pBufferMgr);
				}
				if(pHTuner->pRealTimeInstance != NULL)
				{
					VK_MEM_Free(pHTuner->pRealTimeInstance);
					pHTuner->pRealTimeInstance = NULL;
				}
			}
			break;
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_PROTOCOL_SSTR : /* for SSTR */
#endif
		case DI_IPTUNER_PROTOCOL_HLS : /* for HLS  */
#ifdef CONFIG_MEDIA_DASH
		case DI_IPTUNER_PROTOCOL_DASH : /* for DASH */
#endif

			if(DI_IPTUNER_AdaptiveDestroy(pHTuner) != DI_ERR_OK)
			{
				PrintError ("%s(%d) Error> Session(0x%x) DI_IPTUNER_AdaptiveDestroy failed. \n",__FUNCTION__,__LINE__,pHTuner);
			}
			break;
		default :
			break;
	}

	if(pHTuner->pTls_ca_name != NULL)
	{
		DD_MEM_Free(pHTuner->pTls_ca_name);
		pHTuner->pTls_ca_name = NULL;
	}

	if(pHTuner->pTls_ca_path != NULL)
	{
		DD_MEM_Free(pHTuner->pTls_ca_path);
		pHTuner->pTls_ca_path = NULL;
	}

	if(pHTuner->pTls_certi_path != NULL)
	{
		DD_MEM_Free(pHTuner->pTls_certi_path);
		pHTuner->pTls_certi_path = NULL;
	}

	if(pHTuner->pTls_prikey_path != NULL)
	{
		DD_MEM_Free(pHTuner->pTls_prikey_path);
		pHTuner->pTls_prikey_path = NULL;
	}

	if(pHTuner->pTls_pripw != NULL)
	{
		DD_MEM_Free(pHTuner->pTls_pripw);
		pHTuner->pTls_pripw = NULL;
	}

	if(pHTuner->cbTlsCertificate.cbVerify != NULL)
	{
		pHTuner->cbTlsCertificate.cbVerify = NULL;
	}

	if(pHTuner->tDlnaSetting.bDlna == TRUE)
	{
		if(pHTuner->tDlnaSetting.pcProfile != NULL)
		{
			DD_MEM_Free(pHTuner->tDlnaSetting.pcProfile);
			pHTuner->tDlnaSetting.pcProfile = NULL;
		}
	}

	if(pHTuner->pProtocolinfo != NULL)
	{
		DD_MEM_Free(pHTuner->pProtocolinfo);
		pHTuner->pProtocolinfo = NULL;
	}

	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);

	(void) VK_SEM_Destroy (pHTuner->ulIPTUNERSema);

	nRet = DI_IPTUNER_POOL_FreeTuner (pHTuner);
	if (nRet != DI_ERR_OK)
	{
		PrintError ("%s(%d) Error> DI_IPTUNER_Destroy failed. \n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return nRet;
}

/*
 * 생성된 IPTUNER handle 을 입력받아 Tuner의 type 에 맞게
 * Playback 아래에 VFIO 를 Install 한다.
 * Install 되는 VFIO 는 Playback 이 전용으로 사용하게 되며
 * index 파일까지 같이 Open 되는 경우에 대해 고려되어야 한다.
 * DNP 의 경우 startBuffering 이후에 호출된다.
 * Multicast는 VFIO install 대신 TSInjector Thread 구동 및 IGMP Join을 수행한다.
 *
 */
DI_ERR_CODE DI_IPTUNER_Open (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	DI_IPTUNER_DownloadType_e eDown;

	PrintEnter;
	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> invalid Tuner Handle!!\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		return nRet;
	}
	eDown = pHTuner->eDownloadType;
	if(	VK_SEM_Get (pHTuner->ulIPTUNERSema) != VK_OK)
	{
		PrintError ("%s (%d) Error> Get IPTuner Semaphore Fail !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
	if ((pHTuner->bIsProbeNeeded == TRUE)
    	    	&& ( pHTuner->bIsProbeDone != TRUE))
	{
		PrintError ("%s(%d) Error> This media doesn't probed yet. DI_IPTUNER_Probe first.!!\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return nRet;
	}

	switch (eDown)
	{
		case 	DI_IPTUNER_DOWNLOADING :
			nRet = DI_IPTUNER_DnpOpen (pHTuner);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> Session(0x%x) Download Tuner Open Error.\n",__FUNCTION__,__LINE__,pHTuner);
				(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
				return DI_ERR_ERROR;
			}
			break;
		case 	DI_IPTUNER_STREAMING :
			nRet = DI_IPTUNER_StreamOpen (pHTuner);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> Session(0x%x) STREAM Tuner Open Error.\n",__FUNCTION__,__LINE__,pHTuner);
				(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
				return DI_ERR_ERROR;
			}
			break;
		case 	DI_IPTUNER_MULTICAST :
			nRet = DI_IPTUNER_MulticastOpen (pHTuner);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> Session(0x%x) MULTICAST Tuner Open Error.\n",__FUNCTION__,__LINE__,pHTuner);
				(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
				return DI_ERR_ERROR;
			}
			break;
		case 	DI_IPTUNER_FILE :
			PrintDebug ("%s(%d) Info > Session(0x%x) This is just file tuner \n",__FUNCTION__,__LINE__,pHTuner);
			nRet = DI_ERR_OK;
			break;
		default	:
			PrintError ("%s(%d) Error> Session(0x%x) %d downloadtype %d protocol is not supported. \n"
				,__FUNCTION__
				,__LINE__
				,pHTuner
				,pHTuner->eDownloadType
				, pHTuner->eProtocolType);
		break;
	}
	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
	PrintExit;
	return DI_ERR_OK;
}

/*
 * 생성된 IPTUNER handle 을 입력받아 해당 tuner 를 Close 한다.
 * 이때 Install 되었던 VFIO Set 이 삭제되어야 한다.
 * Multicast는 VFIO uninstall 대신 Thread 종료 및 IGMP Leave을 수행한다.
 */
DI_ERR_CODE DI_IPTUNER_Close (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	DI_IPTUNER_DownloadType_e eDown;

	PrintEnter;
	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> invalid Tuner Handle!!\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		return nRet;
	}
	eDown = pHTuner->eDownloadType;
	(void) VK_SEM_Get (pHTuner->ulIPTUNERSema);
	switch (eDown)
	{
		case 	DI_IPTUNER_DOWNLOADING :
			/* Buffering should be stopped before closing. */
			/* Buffering should be stopped when IPTUNER_Destory called*/
#if 0
			if (pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_START)
			{
				PrintError ("%s(%d) Error> To close this tuner, DI_IPTUNER_StopBuffering should be called first.\n",__FUNCTION__,__LINE__);
				return DI_ERR_ERROR;
			}
#endif

#if 0
			if (pHTuner->eBufferingStatus != DI_IPTUNER_BUFFERING_STOP)
			{
				nRet = DI_IPTUNER_StopBuffering (pHTuner);
				if (DI_ERR_OK != nRet)
				{
					PrintError ("%s(%d) Error> To close this tuner, DI_IPTUNER_StopBuffering should be called first.\n",__FUNCTION__,__LINE__);
					return DI_ERR_ERROR;
				}
			}
#endif

			nRet =DI_IPTUNER_DnpClose (pHTuner);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> Session(0x%x) STREAM Tuner Open Error.\n",__FUNCTION__,__LINE__,pHTuner);
				(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
				return DI_ERR_ERROR;
			}
			break;

		case 	DI_IPTUNER_STREAMING :
			nRet = DI_IPTUNER_StreamClose (pHTuner);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> Session(0x%x) STREAM Tuner Open Error.\n",__FUNCTION__,__LINE__,pHTuner);
				(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
				return DI_ERR_ERROR;
			}
			break;
		case 	DI_IPTUNER_MULTICAST :
			nRet = DI_IPTUNER_MulticastClose (pHTuner);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> Session(0x%x) MULTICAST Tuner Open Error.\n",__FUNCTION__,__LINE__,pHTuner);
				PrintExit;
				(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
				return DI_ERR_ERROR;
			}
			break;
		case 	DI_IPTUNER_FILE :
			PrintDebug ("%s(%d) Info > Session(0x%x) This is just file tuner \n",__FUNCTION__,__LINE__,pHTuner);
			nRet = DI_ERR_OK;
			break;
		default	:
			PrintError ("%s(%d) Error> Session(0x%x) %d downloadtype %d protocol is not supported. \n"
				,__FUNCTION__
				,__LINE__
				,pHTuner
				,pHTuner->eDownloadType
				, pHTuner->eProtocolType);
		break;
	}

	if(pHTuner->tMediaInfo.pucIndex != NULL)
	{
		VK_MEM_Free(pHTuner->tMediaInfo.pucIndex);
	}

	if(pHTuner->tMediaInfo.pucWrmHeader != NULL)
	{
		VK_MEM_Free(pHTuner->tMediaInfo.pucWrmHeader);
	}
	PrintExit;
	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);

	return DI_ERR_OK;
}

/**
 * Control IP Tuner
 *
 * @brief 		IPTUNER 의 세부동작을 제어한다.
 * @param[in] 	IPTUNER Index (IPTUNER Handler)
 * @param[out] 	none
 * @return 		DI_ERR_CODE
 * @date 		2009/05/24
 * @author 		junsy@humaxdigital.com
 * @remark 		IP Tuner Index를 통해 Tuner의 세부적인 기능을 제어하며
 * 				Remote Trick, Pause, Resume 등의 기능을 Tuner Type에 따라 제어한다.
 * @see
 */
DI_ERR_CODE DI_IPTUNER_Control  (DI_IPTUNER_t *pHTuner, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if(	VK_SEM_Get (pHTuner->ulIPTUNERSema) != VK_OK)
	{
		PrintError ("%s (%d) Error> Get IPTuner Semaphore Fail !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
	switch ( pHTuner->eProtocolType )
	{

#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_PROTOCOL_SSTR : /* for SSTR */
#endif
		case DI_IPTUNER_PROTOCOL_HLS :
		case DI_IPTUNER_PROTOCOL_DASH :
			nRet = DI_IPTUNER_StreamControl (pHTuner, cmd, val, val2);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> Session(0x%x) STREAM Tuner Control Error.\n",__FUNCTION__,__LINE__,pHTuner);
				(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
				return nRet;
			}
			break;
		case DI_IPTUNER_PROTOCOL_HTTP :
		case DI_IPTUNER_PROTOCOL_HTTPS :
		case DI_IPTUNER_PROTOCOL_MMS :
		case DI_IPTUNER_PROTOCOL_FTP :
		case DI_IPTUNER_PROTOCOL_RTSP :
		case DI_IPTUNER_PROTOCOL_UDP :
		case DI_IPTUNER_PROTOCOL_UDP_MCAST :
		case DI_IPTUNER_PROTOCOL_RTP :
		case DI_IPTUNER_PROTOCOL_RTP_MCAST :
		case DI_IPTUNER_PROTOCOL_SMB :
		case DI_IPTUNER_PROTOCOL_ABR :
		default  :
			PrintError ("[%s] [%d] Protocol %d is not supported\n",__FUNCTION__,__LINE__, pHTuner->eProtocolType);
			break;
	}
	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
	PrintExit;
	return DI_ERR_OK;
}


#if 0
/*
 * FALSE : hole exists.
 * TRUE  : downloaded
 */
HBOOL DI_IPTUNER_CheckBufferRegion (DI_IPTUNER_t *pHTuner, HUINT32 unSeekTime)
{
	HBOOL bFilled=TRUE;
	DI_ERR_CODE nRet = DI_ERR_OK;
	DI_IPTUNER_DownloadType_e eDown = pHTuner->eDownloadType;
	HINT64 llByteOffset = 0;

	PrintEnter;

	nRet = P_IPTUNER_ConvertTimeToByteOffset(pHTuner, unSeekTime, &llByteOffset);
	if ( DI_ERR_OK != nRet)
	{
		PrintError ("%s(%d) Error> Fail P_IPTUNER_ConvertTimeToByteOffset \n",__FUNCTION__,__LINE__);
	}
	else
	{
		switch (eDown)
		{
			case 	DI_IPTUNER_DOWNLOADING :
				bFilled = DI_IPTUNER_DnpCheckBufferRegion (pHTuner,llByteOffset, pHTuner->ulDNPFileLength);
				break;
			case 	DI_IPTUNER_STREAMING :
			case 	DI_IPTUNER_FILE :
			default	:
				PrintError ("%s(%d) Error> %d downloadtype %d protocol is not supported. \n"
						,__FUNCTION__
						,__LINE__
						,pHTuner->eDownloadType
						, pHTuner->eProtocolType);
				break;
		}
		PrintDebug ("%s(%d) Debug> Check Buffering Region from %lld to %llu bytes [%s]\n"
				,__FUNCTION__
				,__LINE__
				,llByteOffset
				,pHTuner->ulDNPFileLength
				, bFilled?"FILLED !!!":"HOLE !!!");
	}

	PrintExit;

	return bFilled;
}
#endif
/* TEST */
void DI_IPTUNER_EscapeUnderflow (DI_IPTUNER_t *pHTuner)
{
	PrintEnter;
	/* check params */
	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return;
	}
	/* TODO : this code show be re-implemented in dnp layer */
	if(pHTuner->bUnderflow == TRUE )
	{
		if(pHTuner->pPlayerControlCallbacks != NULL)
		{
			pHTuner->pPlayerControlCallbacks->cbResume ((HUINT32)pHTuner);
			pHTuner->bUnderflow = FALSE;
			pHTuner->bEscapeUnderflow = TRUE;
			VK_TASK_Sleep (2000);
		}
	}
	else
	{
		PrintError ("%s(%d) Debug> Session(0x%x) not underflow status.\n",__FUNCTION__,__LINE__,pHTuner);
	}
	PrintExit;
	return;
}

/* unInterval 0 means that the end of the original content */
HBOOL DI_IPTUNER_CheckBufferRegion (DI_IPTUNER_t *pHTuner, HUINT32 unSeekTime, HUINT32 unEndTime)
{
	HBOOL bFilled=TRUE;
	DI_ERR_CODE nStartRet = DI_ERR_OK;
	DI_ERR_CODE nEndRet = DI_ERR_OK;
	DI_IPTUNER_DownloadType_e eDown;
	HUINT64 ulStartByteOffset = 0;
	HUINT64 ulEndByteOffset = 0;

	PrintEnter;
	/* check params */
	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return FALSE;
	}

	eDown = pHTuner->eDownloadType;

	nStartRet = P_IPTUNER_ConvertTimeToByteOffset(pHTuner, unSeekTime, &ulStartByteOffset);
	if(unEndTime == 0)
		ulEndByteOffset = (HINT64) pHTuner->ulDNPFileLength;
	else
	{
		if( (pHTuner->tMediaInfo.duration < unEndTime))
			nEndRet = P_IPTUNER_ConvertTimeToByteOffset (pHTuner, pHTuner->tMediaInfo.duration , &ulEndByteOffset);
		else
			nEndRet = P_IPTUNER_ConvertTimeToByteOffset (pHTuner, unEndTime , &ulEndByteOffset);
	}

	if( (nStartRet == DI_ERR_OK)
		&& (nEndRet == DI_ERR_OK))
	{
		switch (eDown)
		{
			case 	DI_IPTUNER_DOWNLOADING :
				bFilled = DI_IPTUNER_DnpCheckBufferRegion (pHTuner,ulStartByteOffset, ulEndByteOffset);
				break;
			case 	DI_IPTUNER_STREAMING :
			case 	DI_IPTUNER_FILE :
			default	:
				PrintError ("%s(%d) Error> Session(0x%x) %d downloadtype %d protocol is not supported. \n"
						,__FUNCTION__
						,__LINE__
						,pHTuner
						,pHTuner->eDownloadType
						, pHTuner->eProtocolType);
				break;
		}
		PrintDebug ("%s(%d) Debug> Session(0x%x) Check Buffering Region from %llu to %llu bytes [%s]\n"
				,__FUNCTION__
				,__LINE__
				,pHTuner
				,ulStartByteOffset
				,ulEndByteOffset
				,bFilled?"FILLED !!!":"HOLE !!!");
	}
	else PrintError ("%s(%d) Error> Session(0x%x) P_IPTUNER_ConvertTimeToByteOffset Failed !!\n" ,__FUNCTION__ ,__LINE__,pHTuner);

	PrintExit;

	return bFilled;
}
/*
 * 생성된 IPTUNER handle 을 입력받아 Seek 기능을 수행한다.
 * 이 Seek 기능은 Playback Seek 이 아닌 Server 단 Seek 기능을 위해 사용한다.
 * 따라서 이 함수는 Streaming Content 에 전용으로 할당된다.
 */
DI_ERR_CODE DI_IPTUNER_Seek (DI_IPTUNER_t *pHTuner, HUINT64 nPosition)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	/* TODO : will be implemented. */
	UNUSED_PARAM(pHTuner);
	UNUSED_PARAM(nRet);
	UNUSED_PARAM(nPosition);
	return nRet;
}
/*
 * 생성된 IPTUNER handle 을 입력받아 Seek 기능을 수행한다.
 * 이 SetSpeed 기능은 Playback Speed 설정이 아닌 Server 단 Trick 기능을 위해 사용한다.
 * 따라서 이 함수는 Streaming Content 에 전용으로 할당된다.
 */
DI_ERR_CODE DI_IPTUNER_SetSpeed (DI_IPTUNER_t *pHTuner, HINT32 speed )
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	/* TODO : will be implemented. */
	UNUSED_PARAM(pHTuner);
	UNUSED_PARAM(nRet);
	UNUSED_PARAM(speed);
	return nRet;
}

/*
 * 생성된 IPTUNER handle 을 입력받아 Tuner Structure 를 전달해준다.
 */
DI_ERR_CODE DI_IPTUNER_GetTunerInfo (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	/* TODO : will be implemented. */
	UNUSED_PARAM(pHTuner);
	UNUSED_PARAM(nRet);
	return nRet;
}

/*
 * 생성된 IPTUNER 의 list 를 모두 제공한다.
 */
DI_ERR_CODE DI_IPTUNER_GetTunerList (DI_IPTUNER_SET_t *pTunerList)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	/* TODO : will be implemented. */
	UNUSED_PARAM(nRet);
	UNUSED_PARAM(pTunerList);
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_SetUserAgentString (DI_IPTUNER_t *pHTuner, HINT8 *pUserAgent, HINT32 nSize)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	/* check params */
	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	/* check params */
	if ( (pUserAgent == NULL) || (nSize <= 0 ))
	{
		PrintError ("%s(%d) Error> Session(0x%x) Invalid input parameters .\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}
	if(pHTuner->pUserAgentString != NULL)
	{
		VK_MEM_Free(pHTuner->pUserAgentString);
		pHTuner->pUserAgentString = NULL;
	}
	pHTuner->pUserAgentString = (HINT8*) VK_MEM_Alloc (nSize + 1); /* Padding */
	if(pHTuner->pUserAgentString == NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) can't VK_malloc the heap to store UA String..\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}
	VK_MEM_Memset(pHTuner->pUserAgentString,0x0,nSize + 1); /* Padding */
	(void) VK_strncpy (pHTuner->pUserAgentString, pUserAgent, nSize);
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_SetAdaptiveSettings (DI_IPTUNER_t *pHTuner, const DI_MEDIA_AdaptiveSettings_t *pAdaptiveSettings)
{
	DI_IPTUNER_AdaptiveSettings_t tSettings;
	DI_ERR_CODE	nRet = DI_ERR_OK;

	PrintEnter;
	/* valid checking */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	tSettings.eControl = pAdaptiveSettings->eControl;
	tSettings.eDecision = pAdaptiveSettings->eDecision;
	tSettings.eStartup = pAdaptiveSettings->eStartup;
	tSettings.ulVideoStreamIndex = pAdaptiveSettings->ulVideoStreamIndex;
	tSettings.ulAudioStreamIndex = pAdaptiveSettings->ulAudioStreamIndex;
	tSettings.ulTextStreamIndex = pAdaptiveSettings->ulTextStreamIndex;
	tSettings.ulVideoTrackIndex = pAdaptiveSettings->ulVideoTrackIndex;
	tSettings.ulAudioTrackIndex = pAdaptiveSettings->ulAudioTrackIndex;
	tSettings.ulTextTrackIndex = pAdaptiveSettings->ulTextTrackIndex;
	tSettings.ulProgramIndex = pAdaptiveSettings->ulProgramIndex;
	tSettings.ulStreamIndex = pAdaptiveSettings->ulStreamIndex;
	nRet = DI_IPTUNER_AdaptiveSetSettings(pHTuner,&tSettings);
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_GetAdaptiveSettings (DI_IPTUNER_t *pHTuner, DI_MEDIA_AdaptiveSettings_t *pAdaptiveSettings)
{
	DI_IPTUNER_AdaptiveSettings_t tSettings;

	PrintEnter;
	/* valid checking */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	VK_MEM_Memset(&tSettings,0x00,sizeof(DI_IPTUNER_AdaptiveSettings_t));
	if(DI_IPTUNER_AdaptiveGetSettings(pHTuner,&tSettings) == DI_ERR_OK)
	{
		pAdaptiveSettings->eControl = tSettings.eControl;
		pAdaptiveSettings->eDecision = tSettings.eDecision;
		pAdaptiveSettings->eStartup = tSettings.eStartup;

		pAdaptiveSettings->ulVideoStreamIndex = tSettings.ulVideoStreamIndex;
		pAdaptiveSettings->ulAudioStreamIndex = tSettings.ulAudioStreamIndex;
		pAdaptiveSettings->ulTextStreamIndex = tSettings.ulTextStreamIndex;
		pAdaptiveSettings->ulVideoTrackIndex = tSettings.ulVideoTrackIndex;
		pAdaptiveSettings->ulAudioTrackIndex = tSettings.ulAudioTrackIndex;
		pAdaptiveSettings->ulTextTrackIndex = tSettings.ulTextTrackIndex;
		pAdaptiveSettings->ulProgramIndex = tSettings.ulProgramIndex;
		pAdaptiveSettings->ulStreamIndex = tSettings.ulStreamIndex;
	}
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_SetBufferingMode (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferingMode_e eConstraint)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	/* valid checking */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if( /*(eConstraint >= DI_IPTUNER_MODE_UNLIMITED) && */ /* warning: comparison of unsigned expression >= 0 is always true */
		(eConstraint <= DI_IPTUNER_MODE_FIXED))
	{
		if(pHTuner->eBufferingMode != eConstraint)
		{
			pHTuner->eBufferingMode = eConstraint;
#ifdef DI_IPTUNER_DEBUG
			if (eConstraint == DI_IPTUNER_MODE_UNLIMITED)
			{
				PrintDebug ("%s(%d) Info > Session(0x%x) DI_IPTUNER_BufferingMode_e changes to DI_IPTUNER_MODE_UNLIMITED \n"
						,__FUNCTION__,__LINE__,pHTuner);
			}
			else if (eConstraint == DI_IPTUNER_MODE_THRESHOLD)
			{
				PrintDebug ("%s(%d) Info > Session(0x%x) DI_IPTUNER_BufferingMode_e changes to DI_IPTUNER_MODE_THRESHOLD \n"
						,__FUNCTION__,__LINE__,pHTuner);
			}
			else if (eConstraint == DI_IPTUNER_MODE_FIXED)
			{
				PrintDebug ("%s(%d) Info > Session(0x%x) DI_IPTUNER_BufferingMode_e changes DI_IPTUNER_MODE_FIXED \n"
						,__FUNCTION__,__LINE__,pHTuner);
			}
#endif
		}
		else
		{
			PrintDebug ("%s(%d) Info > Session(0x%x) DI_IPTUNER_BufferingMode_e dosen't change because it is the same value.. \n"
				,__FUNCTION__,__LINE__,pHTuner);
		}
	}
	else
	{
		PrintError ("%s(%d) Error> Session(0x%x) DI_IPTUNER_BufferingMode_e is unknown types(%d).\n",__FUNCTION__,__LINE__,pHTuner,eConstraint);
		nRet = DI_ERR_ERROR;
	}

	/* TODO */


	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_SetBufferingModeParams (DI_IPTUNER_t *pHTuner, HUINT32 nBufferThreshold, HBOOL bFastFillIsLimited, HFLOAT64 dFastFillLimitMultiple, HFLOAT64 dManagedFillLimitMultiple)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	/* valid checking */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if(pHTuner->nBufferThreshold != nBufferThreshold)
	{
		pHTuner->nBufferThreshold = nBufferThreshold;
		PrintDebug ("%s(%d) Info > Session(0x%x) BufferingModeParam (nBufferThreshold) is changed to %u msecs\n"
				,__FUNCTION__,__LINE__,pHTuner,nBufferThreshold);
	}

	if(pHTuner->bFastFillIsLimited != bFastFillIsLimited)
	{
		pHTuner->bFastFillIsLimited = bFastFillIsLimited;
		PrintDebug ("%s(%d) Info > Session(0x%x) BufferingModeParam (bFastFillisLimitedMultiple) is changed to %s\n"
				,__FUNCTION__,__LINE__,pHTuner,bFastFillIsLimited ? "TRUE":"FALSE" );
	}

	if(pHTuner->dFastFillLimitMultiple != dFastFillLimitMultiple)
	{
		pHTuner->dFastFillLimitMultiple = dFastFillLimitMultiple;
		PrintDebug ("%s(%d) Info > Session(0x%x) BufferingModeParam (dFastFillLimitMultiple) is changed to %f\n"
				,__FUNCTION__,__LINE__,pHTuner,dFastFillLimitMultiple);
	}

	if(pHTuner->dManagedFillLimitMultiple != dManagedFillLimitMultiple)
	{
		pHTuner->dManagedFillLimitMultiple = dManagedFillLimitMultiple;
		PrintDebug ("%s(%d) Info > Session(0x%x) BufferingModeParam (dManagedFillLimitMultiple) is changed to %f\n"
				,__FUNCTION__,__LINE__,pHTuner,dManagedFillLimitMultiple);
	}
#if 1 /* dragon */
	PrintDebug ("%s(%d) Info > Session(0x%x)\n" ,__FUNCTION__,__LINE__, pHTuner);
	PrintDebug ("%s(%d) Info > (nBufferThreshold) %u msecs\n" ,__FUNCTION__,__LINE__, nBufferThreshold);
	PrintDebug ("%s(%d) Info > (bFastFillisLimitedMultiple)  %s \n" ,__FUNCTION__,__LINE__, bFastFillIsLimited ? "TRUE":"FALSE" );
	PrintDebug ("%s(%d) Info > (dFastFillLimitMultiple)  %f \n" ,__FUNCTION__,__LINE__, dFastFillLimitMultiple);
	PrintDebug ("%s(%d) Info > (dManagedFillLimitMultiple)  %f \n" ,__FUNCTION__,__LINE__, dManagedFillLimitMultiple);
#endif

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_SetPlaybackStartParams (DI_IPTUNER_t *pHTuner, HUINT32 ulStartThreshold, HUINT32 ulStartConstant)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	/* valid checking */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if(pHTuner->ulStartThreshold != ulStartThreshold)
	{
		pHTuner->ulStartThreshold 	= ulStartThreshold;
		PrintDebug ("%s(%d) Info> Session(0x%x) Starting Threshold is changed to %ud.\n",__FUNCTION__,__LINE__,pHTuner,pHTuner->ulStartThreshold);
	}

	if(pHTuner->ulStartConstant != ulStartConstant)
	{
		pHTuner->ulStartConstant 	= ulStartConstant;
		PrintDebug ("%s(%d) Info> Session(0x%x) Starting Constant is changed to %ud.\n",__FUNCTION__,__LINE__,pHTuner,pHTuner->ulStartThreshold);
	}

	/* Print Information */
#ifdef DI_IPTUNER_DEBUG
	PrintDebug ("%s(%d) Info> Playback Starting Parameters \n",__FUNCTION__,__LINE__);
	PrintDebug ("%s(%d) pHTuner(%u)\n",__FUNCTION__,__LINE__,(unsigned int)pHTuner);
	PrintDebug ("%s(%d) ulStartThreshold (%u)\n",__FUNCTION__,__LINE__, ulStartThreshold);
	PrintDebug ("%s(%d) ulStartConstant  (%u)\n",__FUNCTION__,__LINE__, ulStartConstant);
#endif
	/* TODO : how to update changed params in download task */

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_GetBufferingMode (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferingMode_e *peConstraint)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	/* valid checking */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	*peConstraint = pHTuner->eBufferingMode;
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_GetBufferingModeParams (DI_IPTUNER_t *pHTuner, HUINT32 *punBufferThreshold, HBOOL *pbFastFillIsLimited, HFLOAT64 *pdFastFillLimitMultiple, HFLOAT64 *pdManagedFillLimitMultiple)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	/* valid checking */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	*punBufferThreshold = pHTuner->nBufferThreshold;
	*pbFastFillIsLimited = pHTuner->bFastFillIsLimited;
	*pdFastFillLimitMultiple = pHTuner->dFastFillLimitMultiple;
	*pdManagedFillLimitMultiple = pHTuner->dManagedFillLimitMultiple;
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_GetPlaybackStartParams (DI_IPTUNER_t *pHTuner, HUINT32 *pulStartThreshold, HUINT32 *pulStartConstant)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	/* valid checking */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	*pulStartThreshold = pHTuner->ulStartThreshold;
	*pulStartConstant = pHTuner->ulStartConstant;
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_GetDuration (DI_IPTUNER_t *pHTuner, HUINT32 *punDuration)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	/* valid checking #1 */
	if(pHTuner == NULL || punDuration == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	/* valid checking #2 */
	if( FALSE == DI_IPTUNER_POOL_IsValidHandle (pHTuner) )
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle couldn't be found within current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	if (pHTuner->bIsProbeDone == TRUE)
	{
		*punDuration = pHTuner->tMediaInfo.duration;
	}
	else
	{
		PrintError ("%s(%d) Error> Session(0x%x) not probed yet.\n",__FUNCTION__,__LINE__,pHTuner);
		*punDuration = 0;
	}

	PrintExit;
	return nRet;
}

/*
 * This function returns a structure of current buffer information at any time.
 * See DI_IPTUNER_Buffer_t.
 */
DI_ERR_CODE DI_IPTUNER_GetBufferStatus (DI_IPTUNER_t *pHTuner,HUINT32 nHiddenPumpBuf, HUINT32 nHiddenDecBuf , DI_IPTUNER_Buffer_t *pStatus)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	/* valid checking #1 */
	if(pHTuner == NULL || pStatus == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	/* valid checking #2 */
	if( FALSE == DI_IPTUNER_POOL_IsValidHandle (pHTuner) )
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle couldn't be found within current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	nRet = DI_IPTUNER_DnpGetBufferStatus (pHTuner,nHiddenPumpBuf,nHiddenDecBuf, pStatus);
	if(nRet != DI_ERR_OK)
	{
		PrintError ("%s(%d) Error> Session(0x%x) DI_IPTUNER_DnpGetBufStatus failed.\n",__FUNCTION__,__LINE__,pHTuner);
	}

	PrintExit;
	return nRet;
}


/*
 *  생성된 IPTUNER handle 을 입력받아 Open된 iptuner 의 Buffering Position 관리를 위해
 *  EndTime 을 설정한다.
 */
DI_ERR_CODE DI_IPTUNER_SetEndTime (DI_IPTUNER_t *pHTuner, HUINT32 ulTime )
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HBOOL bRet = FALSE;
	HUINT32 unMarginTime = 5000; /* msecs */
	PrintEnter;

	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
	}
	else
	{
		pHTuner->ulEndTime  = ulTime;
		pHTuner->ulEndByte = 0;

		/* If this tuner has been probed, The TimeLine would be determined.*/
		if ( pHTuner->bIsProbeDone == TRUE)
		{
#if 1 /* mp3 workaround : 오디오만 play 하는 경우 margin 값이 10초이상 커야함.(측정치)
			설정하기 원하는 endtime 보다 10초 이상의 data 가 (eof제외) file 에 buffering 되어 있지 않으면
			AUDIO_UNDERRUN 으로 인해 bufcomplete 일지라도 제시간 까지 play하지 않음 in BRCM audio playback
			추후 AUDIO or bmedia parser 단에서 필요 data 만큼만 처리하도록 보완처리되어야 함.
			현재 margin 값으로 인해 bufferstatus 조회시 endtime 에 해당 하는 data 보다 margin 만큼 많이 버퍼링함.*/
			if (pHTuner->tMediaInfo.video_pes_id == 0
				|| pHTuner->tMediaInfo.audio_pes_id )
				unMarginTime += 10000;
#endif
			if(pHTuner->ulEndTime == 0)
				pHTuner->ulEndByte = pHTuner->ulDNPFileLength;
			else
			{
				nRet = P_IPTUNER_ConvertTimeToByteOffset (pHTuner, ulTime + unMarginTime , &pHTuner->ulEndByte);
			if(nRet != DI_ERR_OK)
			{
				PrintError ("%s(%d) Error> Can't set endtime to iptuner \n",__FUNCTION__,__LINE__);
			}
		}

			bRet = DI_IPTUNER_DnpSetOLTEndTime (pHTuner, pHTuner->ulEndByte);
			if (bRet != TRUE)
			{
				PrintError ("%s(%d) Error> Can't set endoffset to OLT table. \n",__FUNCTION__,__LINE__);
			}
		}
		/* If not probed, the endbyte should be updated and calculated after probing. */
		else
		{
			PrintDebug ("%s(%d) Info> Session(0x%x) This Media doesn't probed yet, So The endTime will be updated after probing.\n",__FUNCTION__,__LINE__,pHTuner);
		}
	}

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_GetEndTime (DI_IPTUNER_t *pHTuner, HUINT32 *pulTime )
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	*pulTime = pHTuner->ulEndTime;
	PrintExit;
	return nRet;
}

/*
 * 생성된 IPTUNER의 handle 을 입력받아 Buffering 을 Start 하게 된다.
 * BufferType 에는 RAMBUFFERING, HDDBuffering 두가지가 있으며
 * HTTP,RTSP 등 모든 protocol controller 에 대해 동일한 기능을 수행한다.
 */
DI_ERR_CODE DI_IPTUNER_StartBuffering (DI_IPTUNER_t *pHTuner, HINT8 *pSaveFileName)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	/* 1. check the validation about input parameters */
	if(pHTuner == NULL || pSaveFileName == NULL)
	{
		PrintError ("%s(%d) Error> Please check pHTuner (%p) or pSaveFileName(%p).\n"
		,__FUNCTION__
		,__LINE__
		,(void*)pHTuner
		,(void*)pSaveFileName);

		return DI_ERR_ERROR;
	}

	(void) VK_SEM_Get (pHTuner->ulIPTUNERSema);

	/* 2. Check Probing Information */
    if ( pHTuner->bIsProbeDone != TRUE )
	{
		PrintError ("%s(%d) Error> Session(0x%x) This media doesn't probed yet. DI_IPTUNER_Probe first.!!\n",__FUNCTION__,__LINE__,pHTuner);
		nRet = DI_ERR_ERROR;
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return nRet;
	}

	/* 3. check buffering status */
	if(pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_START)
	{
		PrintError ("%s(%d) Error> Session(0x%x) This tuner is already being buffered. \n",__FUNCTION__,__LINE__,pHTuner);
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	/* 4. check the validation about TunerHandle */
	if( FALSE == DI_IPTUNER_POOL_IsValidHandle (pHTuner) )
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle doesn't exist in current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	/* 5. Now, This tuner should be switchover to Downloading type */
	pHTuner->eDownloadType = DI_IPTUNER_DOWNLOADING;

	/*
	 * 6. We need to make the binding between a file and this tuner handle to be stored
	 *  before download task is started.
	 */
	pHTuner->pSaveFileName = (HINT8*) VK_StrDup (pSaveFileName);
	if(pHTuner->pSaveFileName == NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) VK_StrDup Failed. \n",__FUNCTION__,__LINE__,pHTuner);
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	/* 7. A Download TASK will be created. */
	nRet = DI_IPTUNER_DnpCreate (pHTuner);
	if(nRet != DI_ERR_OK)
	{
		/*
		 * If DI_IPTUNER_DnpCreate return error,
		 * we should investigate whether the thread is already terminated or not.
		 */
		PrintError ("%s(%d) Debug> Session(0x%x) pSaveFileName : %s(%d)\n",__FUNCTION__,__LINE__,pHTuner,pHTuner->pSaveFileName,(HUINT32)pHTuner->pSaveFileName);
		PrintError ("%s(%d) Error> Session(0x%x) Download tuner was broken. Check this !\n",__FUNCTION__,__LINE__,pHTuner);
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return nRet;
	}

	/* 8. The Emition of BUFFER_STARTE Event. */
	if(pHTuner->pBufferCallbacks)
	{
		if(pHTuner->pBufferCallbacks->cbStart != NULL)
		{
			pHTuner->pBufferCallbacks->cbStart((HUINT32)pHTuner);
		}
	}

	/* 9. Entering BUFFERING status . */
	pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_START;
	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);

	PrintExit;
	return nRet;
}

/*
 * 생성된 IPTUNER 의 handle 을 입력받아 Buffering Task 를 정지시킨다.
 * 정지된 Task 는 더이상 Stream 을 buffering 하지 않게 되며
 * 이에 대한 정보는 Event 를 통해 알아내야한다.
 */
DI_ERR_CODE DI_IPTUNER_PauseBuffering (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if( VK_SEM_Get (pHTuner->ulIPTUNERSema) != VK_OK)
	{
		PrintError ("%s (%d) Error> Get IPTuner Semaphore Fail !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	if(pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_PAUSE
		|| pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_STOP )
	{
		PrintError ("%s(%d) Error> Session(0x%x) Already paused or stopped. \n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	/* valid checking #2 */
	if( FALSE == DI_IPTUNER_POOL_IsValidHandle (pHTuner) )
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle couldn't be found within current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	nRet = DI_IPTUNER_DnpPause (pHTuner);
	if(nRet != DI_ERR_OK)
	{
		/* If dnpcreate return error , we know that the thread already is terminated. */
		PrintError ("%s(%d) Error> Session(0x%x) Download tuner cann't be paused. \n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_PAUSE;
	VK_SEM_Release (pHTuner->ulIPTUNERSema);
	PrintExit;
	return nRet;
}

/***********************************
 * YouView Specific Function
 ***********************************/
DI_ERR_CODE DI_IPTUNER_ResumeBufferingBySeek (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if( VK_SEM_Get (pHTuner->ulIPTUNERSema) != VK_OK)
	{
		PrintError ("%s (%d) Error> Get IPTuner Semaphore Fail !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
	/* valid checking #2 */
	if( FALSE == DI_IPTUNER_POOL_IsValidHandle (pHTuner) )
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle couldn't be found within current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	if(pHTuner->bPauseDownload == FALSE )
	{
		PrintError ("%s(%d) Error > Session(0x%x) Already Download State \n",__FUNCTION__,__LINE__,pHTuner);\
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	nRet = DI_IPTUNER_DnpResume (pHTuner);
	if(nRet != DI_ERR_OK)
	{
		PrintError ("%s(%d) Error> Session(0x%x) Download tuner cann't be resumed. \n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	/* emit BUFFERING START EVENT */
	if (pHTuner->pBufferCallbacks != NULL)
	{
		if(pHTuner->pBufferCallbacks->cbStart != NULL)
		{
			pHTuner->pBufferCallbacks->cbStart ((HUINT32)pHTuner);
		}
	}

	pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_START;
	VK_SEM_Release (pHTuner->ulIPTUNERSema);
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_ResumeBuffering (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if( VK_SEM_Get (pHTuner->ulIPTUNERSema) != VK_OK)
	{
		PrintError ("%s (%d) Error> Get IPTuner Semaphore Fail !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	if (pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_START
		|| pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_COMPLETE
		|| pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_STOP )
	{
		PrintError ("%s(%d) Error> Session(0x%x) Already started or stopped and complete. \n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	/* valid checking #2 */
	if( FALSE == DI_IPTUNER_POOL_IsValidHandle (pHTuner) )
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle couldn't be found within current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	nRet = DI_IPTUNER_DnpResume (pHTuner);
	if(nRet != DI_ERR_OK)
	{
		PrintError ("%s(%d) Error> Session(0x%x) Download tuner cann't be resumed. \n",__FUNCTION__,__LINE__,pHTuner);
		VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	/* emit BUFFERING START EVENT */
	if (pHTuner->pBufferCallbacks != NULL)
	{
		if(pHTuner->pBufferCallbacks->cbStart != NULL)
		{
			pHTuner->pBufferCallbacks->cbStart ((HUINT32)pHTuner);
		}
	}

	pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_START;
	VK_SEM_Release (pHTuner->ulIPTUNERSema);
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_OnExitReadFlag(DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	/* valid checking #2 */
	if( FALSE == DI_IPTUNER_POOL_IsValidHandle (pHTuner) )
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle couldn't be found within current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	if( VK_SEM_Get (pHTuner->ulIPTUNERSema) != VK_OK)
	{
		PrintError ("%s (%d) Error> Get IPTuner Semaphore Fail !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	pHTuner->bExitRead = TRUE;
	pHTuner->bExitIndexRead = TRUE;

	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_OffExitReadFlag(DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	/* valid checking #1 */
	if(pHTuner == NULL)
{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	/* valid checking #2 */
	if( FALSE == DI_IPTUNER_POOL_IsValidHandle (pHTuner) )
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle couldn't be found within current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	if( VK_SEM_Get (pHTuner->ulIPTUNERSema) != VK_OK)
	{
		PrintError ("%s (%d) Error> Get IPTuner Semaphore Fail !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	pHTuner->bExitRead = FALSE;
	pHTuner->bExitIndexRead = FALSE;

	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);

	PrintExit;
	return nRet;
}

/*
 * 생성된 IPTUNER 의 handle 을 입력받아 Buffering Task 를 종료한다.
 * 종료되지 않은 content 일지라도 강제로 Buffering 을 Stop 시키며
 * Buffering 이 종료되면 Playback 에 install 된 VFIO 를 통해서도 EOF 발생시기를 조절해야한다.
 */
DI_ERR_CODE DI_IPTUNER_StopBuffering (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	(void) VK_SEM_Get (pHTuner->ulIPTUNERSema);

	/* valid checking #2 */
	if (pHTuner->eBufferingStatus == DI_IPTUNER_BUFFERING_STOP)
	{
		/* TODO :  XXX XXX XXX XXX XXX XXX XXX XXX
		 * If the probe is progressing , we have to cancel in time.
		 * Is it possible . DnpDestroy ??
		 */
		PrintError ("%s(%d) Error> Session(0x%x) Already started or stopped. \n",__FUNCTION__,__LINE__,pHTuner);
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	/* valid checking #2 */
	if( FALSE == DI_IPTUNER_POOL_IsValidHandle (pHTuner) )
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle couldn't be found within current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);
		return DI_ERR_ERROR;
	}

	/* Back-Off Algo. */
	(void)DI_STREAM_HTTP_ForcedExit ( pHTuner->pHStream );

	nRet = DI_IPTUNER_DnpDestroy (pHTuner);
	if(nRet != DI_ERR_OK)
	{
		/* If dnpcreate return error , we know that the thread already is terminated. */
		PrintError ("%s(%d) Error> Session(0x%x) Download tuner can't be destroied. \n",__FUNCTION__,__LINE__,pHTuner);
	}

	pHTuner->eBufferingStatus = DI_IPTUNER_BUFFERING_STOP;
	(void) VK_SEM_Release (pHTuner->ulIPTUNERSema);

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_RegisterDelayedSeekCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_DelayedSeekCallback Callback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if( pHTuner->tDelayedSeekCallback != NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) DelayedSeekCallback is already registered.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}
	pHTuner->tDelayedSeekCallback = Callback;

	return nRet;
}
DI_ERR_CODE DI_IPTUNER_UnregisterDelayedSeekCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if( pHTuner->tDelayedSeekCallback != NULL)
	{
		pHTuner->tDelayedSeekCallback = NULL;
	}

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_RegisterDelayedStartCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_DelayedStartCallback Callback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	/* valid checking #1 */
	PrintEnter;
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if( pHTuner->tDelayedStartCallback != NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) DelayedStartCallback is already registered.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}
	pHTuner->tDelayedStartCallback = Callback;

	DI_IPTUNER_DnpRegisterStartTimeMsec (pHTuner);

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_UnregisterDelayedStartCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	pHTuner->tDelayedStartCallback = NULL;

	DI_IPTUNER_DnpUnRegisterStartTimeMsec (pHTuner);

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_RegisterProbeEventCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_ProbeCallback_t *pCallbacks)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if( pHTuner->pProbeCallbacks != NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) ProbeCallback is already registered.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}
	pHTuner->pProbeCallbacks = (DI_IPTUNER_ProbeCallback_t*)VK_MEM_Alloc(sizeof(DI_IPTUNER_ProbeCallback_t));
	if( pHTuner->pProbeCallbacks == NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) Memory Alloc Fail.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memcpy (pHTuner->pProbeCallbacks,pCallbacks, sizeof(DI_IPTUNER_ProbeCallback_t));

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_UnregisterProbeEventCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if(pHTuner->pProbeCallbacks != NULL)
	{
		VK_MEM_Free(pHTuner->pProbeCallbacks);
		pHTuner->pProbeCallbacks = NULL;
	}
	else
		PrintError ("%s(%d) Error> Session(0x%x) ProbeCallback is already unregistered.\n",__FUNCTION__,__LINE__,pHTuner);
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_RegisterBufEventCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferCallback_t *pCallbacks)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if( pHTuner->pBufferCallbacks != NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) BufferEventCallback is already registered.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	pHTuner->pBufferCallbacks = (DI_IPTUNER_BufferCallback_t*)VK_MEM_Alloc(sizeof(DI_IPTUNER_BufferCallback_t));
	if( pHTuner->pBufferCallbacks == NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) Memory Alloc Fail.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}
	VK_MEM_Memcpy(pHTuner->pBufferCallbacks,pCallbacks, sizeof(DI_IPTUNER_BufferCallback_t));

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_UnregisterBufEventCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if (pHTuner->pBufferCallbacks != NULL)
	{
		VK_MEM_Free(pHTuner->pBufferCallbacks);
		pHTuner->pBufferCallbacks = NULL;
	}
	else
	{
		PrintError ("%s(%d) Error> Session(0x%x) BufferEventCallback is already unregistered.\n",__FUNCTION__,__LINE__,pHTuner);
	}

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_RegisterPlayerControlCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_PlayerControlCallback_t *pCallbacks)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	pHTuner->pPlayerControlCallbacks = (DI_IPTUNER_PlayerControlCallback_t*)VK_MEM_Alloc(sizeof(DI_IPTUNER_PlayerControlCallback_t));
	if( pHTuner->pPlayerControlCallbacks == NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) Memory Alloc Fail.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}
	VK_MEM_Memcpy (pHTuner->pPlayerControlCallbacks,pCallbacks, sizeof(DI_IPTUNER_PlayerControlCallback_t));

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_UnregisterPlayerControlCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if (pHTuner->pPlayerControlCallbacks != NULL)
	{
		VK_MEM_Free(pHTuner->pPlayerControlCallbacks);
		pHTuner->pPlayerControlCallbacks = NULL;
	}
	else
	{
		PrintError ("%s(%d) Error> Session(0x%x) PlayerControlCallback is already unregistered.\n",__FUNCTION__,__LINE__,pHTuner);
	}

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_RegisterErrorEventCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_ErrorEventCallback pCallback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if( pHTuner->tErrorEventCallback != NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) ErrorEventCallback is already registered.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	pHTuner->tErrorEventCallback = pCallback;

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_UnregisterErrorEventCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if( pHTuner->tErrorEventCallback != NULL)
	{
		pHTuner->tErrorEventCallback = NULL;
	}

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_RegisterTlsCertificateCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_VerifyExternalCallback_t *pCallback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if(pCallback != NULL)
	{
		pHTuner->cbTlsCertificate.cbVerify = pCallback->cbVerify;
		pHTuner->cbTlsCertificate.pUserdata = pCallback->pUserdata;
	}
	else
	{
		PrintError ("%s(%d) Error> pCallback is null .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_UnregisterTlsCertificateCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if( pHTuner->cbTlsCertificate.cbVerify != NULL)
	{
		pHTuner->cbTlsCertificate.cbVerify = NULL;
		pHTuner->cbTlsCertificate.pUserdata = NULL;
	}

	return nRet;
}

/* TEST */
#if 0
DI_ERR_CODE DI_IPTUNER_DelayedSeekTrigger (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

//	pHTuner->bDelayedSeek = TRUE;

	return nRet;
}
#endif


/*
 * TS injector is used at multicast injection between di_media20 and di_multicast
 * Multicast
 *   - Internal callback between di_media and di_iptuner_multicast
 *   - Forward received packet data to TSInjector:pumpingData
 */
DI_ERR_CODE DI_IPTUNER_RegisterTSInjectorCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_TSInjectorCallback_t *pCallbacks)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	if( pHTuner->pTSInjectorCallbacks != NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) TSInjectorCallbacks is already registered.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	pHTuner->pTSInjectorCallbacks = (DI_IPTUNER_TSInjectorCallback_t*)VK_MEM_Alloc(sizeof(DI_IPTUNER_TSInjectorCallback_t));
	if( pHTuner->pTSInjectorCallbacks == NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) Memory Alloc Fail.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}
	VK_MEM_Memcpy (pHTuner->pTSInjectorCallbacks, pCallbacks, sizeof(DI_IPTUNER_TSInjectorCallback_t));
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_UnregisterTSInjectorCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	/* valid checking #1 */
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if (pHTuner->pTSInjectorCallbacks != NULL)
	{
		VK_MEM_Free(pHTuner->pTSInjectorCallbacks);
		pHTuner->pTSInjectorCallbacks = NULL;
	}
	else
	{
		PrintError ("%s(%d) Error> Session(0x%x) TSInjectorCallback is already unregistered.\n",__FUNCTION__,__LINE__,pHTuner);
	}
	return nRet;
}

/*
 * Multicast
 *   - Allocate channel event like (Tuning, Error, ...)
 *   - Multicast live channel only use this event callback
 */
DI_ERR_CODE DI_IPTUNER_RegisterChannelEventCB (DI_IPTUNER_t *pHTuner,
				DI_MEDIA_ChannelTunerType_e eTunerType,
				DI_MEDIA_ChannelEventCallback_t *pCallbacks)

{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;
	PrintDebug("%s(%d) [tuner_type:%d]\n", __FUNCTION__, __LINE__, eTunerType);

	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	pHTuner->eChannelType = eTunerType;
	if( pHTuner->pChannelCallbacks != NULL)
	{
		PrintError ("%s(%d) Error> Session(0x%x) ChannelEventCallback is already registered.\n",__FUNCTION__,__LINE__,pHTuner);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pHTuner->pChannelCallbacks = (DI_MEDIA_ChannelEventCallback_t*)VK_MEM_Alloc(sizeof(DI_MEDIA_ChannelEventCallback_t));
	if (pHTuner->pChannelCallbacks == NULL) {
		PrintError ("%s(%d) Error> Session(0x%x) Out of memory!!\n",__FUNCTION__,__LINE__,pHTuner);
		PrintExit;
		return DI_ERR_ERROR;
	}
	else {
		VK_MEM_Memcpy(pHTuner->pChannelCallbacks, pCallbacks, sizeof(DI_MEDIA_ChannelEventCallback_t));
	}
	PrintExit;
	return nRet;
}

/*
 * Release channel event callback like (Tuning, Error, ...)
 * Multicast live channel only use this event callback
 */
DI_ERR_CODE DI_IPTUNER_UnregisterChannelEventCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	if(pHTuner->pChannelCallbacks != NULL)
	{
		VK_MEM_Free(pHTuner->pChannelCallbacks);
		pHTuner->pChannelCallbacks = NULL;
	}
	PrintExit;
	return nRet;
}


DI_ERR_CODE DI_IPTUNER_INETRADIO_Probe(DI_IPTUNER_t *pHTuner, DI_MEDIA_INFO_t *stMediaInfo, HBOOL bNeedHttpConnect)
{
	return DI_STREAM_INETRADIO_Probe(pHTuner, stMediaInfo, bNeedHttpConnect);
}

DI_ERR_CODE DI_IPTUNER_RegisterInetRadioChangeTitleCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_InetRadioChangeTitleCallback callback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;
	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if( pHTuner->tInetRadioChangeTitleCallback != NULL)
	{
		PrintError ("%s(%d) Error> InetRadioChangeTitleCallback is already registered.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pHTuner->tInetRadioChangeTitleCallback = callback;

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_UnregisterInetRadioChangeTitleCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if(pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	if(pHTuner->tInetRadioChangeTitleCallback != NULL)
	{
		pHTuner->tInetRadioChangeTitleCallback = NULL;
	}
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_RegisterVideoBufferCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferStatusCallback_t callback)
{
	if(pHTuner == NULL)
	{
		PrintError("[%s][%d] Invalid parameter!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pHTuner->tVideoBufferCallback = callback;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_UnregisterVideoBufferCB (DI_IPTUNER_t *pHTuner)
{
	if(pHTuner == NULL)
	{
		PrintError("[%s][%d] Invalid parameter!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pHTuner->tVideoBufferCallback = NULL;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_RegisterAudioBufferCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferStatusCallback_t callback)
{
	if(pHTuner == NULL)
	{
		PrintError("[%s][%d] Invalid parameter!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pHTuner->tAudioBufferCallback = callback;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_UnregisterAudioBufferCB (DI_IPTUNER_t *pHTuner)
{
	if(pHTuner == NULL)
	{
		PrintError("[%s][%d] Invalid parameter!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pHTuner->tAudioBufferCallback = NULL;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_RegisterPumpBufferCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferStatusCallback_t callback)
{
	if(pHTuner == NULL)
	{
		PrintError("[%s][%d] Invalid parameter!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pHTuner->tPumpBufferCallback = callback;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_UnregisterPumpBufferCB (DI_IPTUNER_t *pHTuner)
{
	if(pHTuner == NULL)
	{
		PrintError("[%s][%d] Invalid parameter!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	pHTuner->tPumpBufferCallback = NULL;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_ForceMoveBufferingPosition (DI_IPTUNER_t *pHTuner)
{
	PrintEnter;

	if(pHTuner == NULL)
	{
		PrintError ("Invalid Parameter! pHTuner is NULL!\n");
		return DI_ERR_ERROR;
	}
	pHTuner->bForceUpdateBP = TRUE;

	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_GetBufferInfo(DI_IPTUNER_t *pHTuner, HUINT32 *uiChunks, HINT32 **iStartOffsets, HINT32 **iEndOffsets)
{
	DI_ERR_CODE dResult;

	PrintEnter;

	if((pHTuner == NULL) || (uiChunks == NULL))
	{
		PrintError("Invalid parameter!\n");
		return DI_ERR_ERROR;
	}

	dResult = DI_IPTUNER_DnpGetBufferInfo(pHTuner, uiChunks, iStartOffsets, iEndOffsets);
	if(dResult != DI_ERR_OK)
	{
		PrintError("%s(%d) Error> Session(0x%x) DI_IPTUNER_DnpGetBufferedInfo failed!\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	PrintExit;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_SetCurrentPlayTime(DI_IPTUNER_t *pHTuner, HUINT32 uiTime)
{
	DI_ERR_CODE dResult;

	if(pHTuner == NULL)
	{
		PrintError("Invalid Parameter, pHTuner is NULL!\n");
		return DI_ERR_ERROR;
	}

	dResult = DI_IPTUNER_DnpSetCurrentPlayTime(pHTuner, uiTime);
	if(dResult != DI_ERR_OK)
	{
		PrintError("DI_IPTUNER_DnpSetCurrentOffset failed!\n");
		return DI_ERR_ERROR;
	}
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_CheckUnderrun(DI_IPTUNER_t* pHTuner)
{
	DI_ERR_CODE dResult;

	if(pHTuner == NULL)
	{
		PrintError("Invalid Parameter, pHTuner is NULL!\n");
		return DI_ERR_ERROR;
	}

	dResult = DI_IPTUNER_DnpCheckUnderrun(pHTuner);
	if(dResult != DI_ERR_OK)
	{
		PrintError("DI_IPTUNER_DnpCheckUnderrun failed!\n");
		return DI_ERR_ERROR;
	}
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_RegisterLiveBufferEventCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_LiveBufferStateCallback callback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if (pHTuner->tLiveBufferStateCallback != NULL)
	{
		PrintError ("%s(%d) Error> InetRadioChangeTitleCallback is already registered.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pHTuner->tLiveBufferStateCallback = callback;
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_UnregisterLiveBufferEventCB (DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid Tuner Handle .\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if (pHTuner->tLiveBufferStateCallback != NULL)
	{
		pHTuner->tLiveBufferStateCallback = NULL;
	}
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_GetLiveBufferState (DI_IPTUNER_t *pHTuner, DI_IPTUNER_CLS_BUFSTATE_e *pnLiveBufferState)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if (FALSE == DI_IPTUNER_POOL_IsValidHandle(pHTuner))
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle couldn't be found within current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	if (pHTuner->bIsProbeDone == TRUE)
	{
		*pnLiveBufferState = pHTuner->eClsBufferState;
	}
	else
	{
		PrintError ("%s(%d) Error> Session(0x%x) not probed yet.\n",__FUNCTION__,__LINE__,pHTuner);
		*pnLiveBufferState = DI_IPTUNER_CLS_BUFSTATE_INVALID;
	}

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_SetLiveBufferState (DI_IPTUNER_t *pHTuner, DI_IPTUNER_CLS_BUFSTATE_e eLiveBufferState)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	if (pHTuner == NULL)
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if (FALSE == DI_IPTUNER_POOL_IsValidHandle(pHTuner))
	{
		PrintError ("%s(%d) Error> Session(0x%x) This handle couldn't be found within current ip pool. \n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	if (pHTuner->bIsProbeDone == TRUE)
	{
		DI_IPTUNER_CLS_SetLiveBufferState(pHTuner, eLiveBufferState);
	}
	else
	{
		PrintError ("%s(%d) Error> Session(0x%x) not probed yet.\n",__FUNCTION__,__LINE__,pHTuner);
		return DI_ERR_ERROR;
	}

	PrintExit;
	return nRet;
}
