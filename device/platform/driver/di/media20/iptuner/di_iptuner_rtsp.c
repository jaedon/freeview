/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_rtsp.c  $
 * Version:		2.0 (for Media 2.0 with 40nm)
 * Original Author:	Jin Lee
 * Current Author:	$Author: jlee@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     RTSP - VFIO Controller
 * Module:
 * Remarks:
 */

/**
 * @defgroup RTSP_VFIO_CONTROLLER
 * @brief RTSP VFIO Controller. This is interlocked with Nexus playback.
 */

/**
 * @author Jin Lee (jlee@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @brief RTSP VFIO Controller. This is interlocked with Nexus playback.
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
 * @file di_iptuner_rtsp.c
 */

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
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"
#include "di_media20.h"
#include "vfio20.h"
#include "di_iptuner20.h"
#include "di_iptuner_pool.h"
/*#include "di_iptuner_dnp.h"*/
#include "di_iptuner_rtsp.h"
#include "stream_rtsp.h"
#include "stream_global.h"
#include "rtsp_lib.h"
#include "util_url.h"
#include "util_sys.h"
#if 0
#include "mms_asf.h"
#endif
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define DI_IPTUNER_RTSP_DEBUG

#ifdef IPTUNER_ALL_DEBUG

#ifdef DI_IPTUNER_RTSP_DEBUG
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
#define SUPPORT_RTSP_SEM 0
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
static STREAM_ACCESS_t * s_ptLastRtspAccess ; /* for Control and Probe */
/* End static variable */

/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
#if __________p__________
#endif

#if __________task__________
#endif

#if __________drv__________
#endif

#if __________di__________
#endif

DI_ERR_CODE DI_STREAM_RTSP_Probe (const HINT8 *pURL,  DI_MEDIA_INFO_t *pstMediaInfo)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	VFIO_Handle_t stHandle;
	struct RTSP_ACCESS_SYS_s *pstRtspSys = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	HINT32	idx = 0;

	PrintEnter;

	PrintDebug("%s(%d) REMOTE PROBE >>>> pURL=(%s)\n",__FUNCTION__,__LINE__,pURL);
	nRet = DI_STREAM_RTSP_OpenForProbe (pURL,NULL, &stHandle);
	if ( VFIO_ERROR_SUCCESS != nRet)
	{
		PrintError("%s(%d) Error> DI_STREAM_RTSP_OpenForProbe fail.\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		return nRet;
	}

	pAccess = s_ptLastRtspAccess;
	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> NULL last RTSP access\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	/* Fixed warning: initialization from incompatible pointer type */
	pstRtspSys = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;
	if (pstRtspSys == NULL)
	{
		PrintError("%s(%d) Error> NULL RTSP sys\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}


	/*
	 * RTSP/1.0 200 OK
	 * Server: VLC/2.0.2
	 * Date: Wed, 12 Sep 2012 08:38:10 GMT
	 * Content-Type: application/sdp
	 * Content-Base: rtsp://192.168.0.2:8554/
	 * Content-Length: 306
	 * Cache-Control: no-cache
	 * Cseq: 1
	 *
	 * v=0
	 * o=- 15274741479123855949 15274741479123855949 IN IP4 jlee
	 * s=Unnamed
	 * i=N/A
	 * c=IN IP4 0.0.0.0
	 * t=0 0
	 * a=tool:vlc 2.0.2
	 * a=recvonly
	 * a=type:broadcast
	 * a=charset:UTF-8
	 * a=control:rtsp://192.168.0.2:8554/
	 * m=video 0 RTP/AVP 32
	 * b=RR:0
	 * a=rtpmap:32 MPV/90000
	 * a=control:rtsp://192.168.0.2:8554/trackID=0
	 */
	pstMediaInfo->stream_type = DI_MEDIA_MPEG_TYPE_TS;
	pstMediaInfo->max_bitrate = 3697711;
	pstMediaInfo->duration = 257996;
	pstMediaInfo->nprograms =1;      /* total number of programs */
	pstMediaInfo->ntracks = 3;        /* total number of tracks */
	pstMediaInfo->video_pes_id = 224;
	pstMediaInfo->video_stream_id = 2005;    /* or Video PID */
	pstMediaInfo->video_codec = DI_VIDEO_FORMAT_MEDIA_H264; /* 0x10000080; */
	pstMediaInfo->usVideoWidth = 0;
	pstMediaInfo->usVideoHeight = 0;
	pstMediaInfo->audio_pes_id = 192;
	pstMediaInfo->audio_stream_id = 2006;    /* 0x3EA */
	pstMediaInfo->audio_codec = DI_AUDFORMAT_AAC;
	pstMediaInfo->support_trick = 1;
	pstMediaInfo->support_seek = 1; // <--
	pstMediaInfo->eStartCondition = 0;
	pstMediaInfo->ulStartTime = 0;
	pstMediaInfo->demuxId = 0;
	pstMediaInfo->pcr_pid = 2005;
	if (pAccess->info.i_size == 0)
	{
		pAccess->info.i_size = 0xFFFFFF;
		/* pstMediaInfo->stream_size = pAccess->info.i_size; 	*/
		PrintError("%s(%d) Error> pAccess->info.i_size:%lld\n", __FUNCTION__, __LINE__, pAccess->info.i_size);
	}
	pstMediaInfo->ucTotalVideoTrack = 1;
	pstMediaInfo->stVideoTrack[0].video_pes_id = pstMediaInfo->video_pes_id;
	pstMediaInfo->stVideoTrack[0].video_stream_id = pstMediaInfo->video_stream_id;
	pstMediaInfo->stVideoTrack[0].video_codec = pstMediaInfo->video_codec;
	pstMediaInfo->ucTotalAudioTrack = 1;
	pstMediaInfo->stAudioTrack[0].audio_pes_id = pstMediaInfo->audio_pes_id;
	pstMediaInfo->stAudioTrack[0].audio_stream_id = pstMediaInfo->audio_stream_id;
	pstMediaInfo->stAudioTrack[0].audio_codec = pstMediaInfo->audio_codec;

	if (1)
	{
		PrintData ("stream_type             (%d)\n",pstMediaInfo->stream_type);
		PrintData ("max_bitrate             (%u)\n",pstMediaInfo->max_bitrate);
		PrintData ("duration                (%u)\n",pstMediaInfo->duration);
		PrintData ("nprograms               (%u)\n",pstMediaInfo->nprograms);
		PrintData ("ntracks                 (%u)\n",pstMediaInfo->ntracks);
		PrintData ("video_pes_id            (%u,0x%X)\n",pstMediaInfo->video_pes_id, pstMediaInfo->video_pes_id);
		PrintData ("video_stream_id         (%u,0x%X)\n",pstMediaInfo->video_stream_id, pstMediaInfo->video_stream_id);
		PrintData ("video_codec             (%u,0x%X)\n",pstMediaInfo->video_codec, pstMediaInfo->video_codec);
		PrintData ("audio_pes_id            (%u,0x%X)\n",pstMediaInfo->audio_pes_id, pstMediaInfo->audio_pes_id);
		PrintData ("audio_stream_id         (%u,0x%X)\n",pstMediaInfo->audio_stream_id, pstMediaInfo->audio_stream_id);
		PrintData ("audio_codec             (%u,0x%X)\n",pstMediaInfo->audio_codec, pstMediaInfo->audio_codec);
		PrintData ("support_trick           (%u)\n",pstMediaInfo->support_trick);
		PrintData ("support_seek            (%u)\n",pstMediaInfo->support_seek);
		PrintData ("audiodecodertype        (%u,0x%X)\n",pstMediaInfo->audiodecodertype, pstMediaInfo->audiodecodertype);
		PrintData ("videodecodertype        (%u,0x%X)\n",pstMediaInfo->videodecodertype, pstMediaInfo->videodecodertype);
		PrintData ("eStartCondition         (%u)\n",pstMediaInfo->eStartCondition);
		PrintData ("ulStartTime             (%u)\n",pstMediaInfo->ulStartTime);
		PrintData ("demuxId                 (%u)\n",pstMediaInfo->demuxId);
		PrintData ("pcr_pid                 (%u,0x%X)\n",pstMediaInfo->pcr_pid, pstMediaInfo->pcr_pid);
		PrintData ("Total Video Track       (%u)\n",pstMediaInfo->ucTotalVideoTrack);
		for(idx=0;idx<pstMediaInfo->ucTotalVideoTrack;idx++)
		{
			PrintData ("------------------------------------------------\n");
			PrintData ("Video Track Num     (%u)\n",idx);
			PrintData ("video_pes_id        (%u)\n",pstMediaInfo->stVideoTrack[idx].video_pes_id);
			PrintData ("video_stream_id     (%u)\n",pstMediaInfo->stVideoTrack[idx].video_stream_id);
			PrintData ("video_codec         (%u)\n",pstMediaInfo->stVideoTrack[idx].video_codec);
		}

		PrintData ("Total audio track       (%u)\n",pstMediaInfo->ucTotalAudioTrack);
		for(idx=0;idx<pstMediaInfo->ucTotalAudioTrack;idx++)
		{
			PrintData ("------------------------------------------------\n");
			PrintData ("audio track Num     (%u)\n",idx);
			PrintData ("audio_pes_id        (%u)\n",pstMediaInfo->stAudioTrack[idx].audio_pes_id);
			PrintData ("audio_stream_id     (%u)\n",pstMediaInfo->stAudioTrack[idx].audio_stream_id);
			PrintData ("audio_codec	        (%u)\n",pstMediaInfo->stAudioTrack[idx].audio_codec);
			PrintData ("audio_language      [%s]\n",pstMediaInfo->stAudioTrack[idx].uacAudioLang);
		}

		PrintData ("------------------------------------------------\n");
		PrintData ("Total Xsub track        (%u)\n",pstMediaInfo->ucTotalXsubTrack);
		for(idx=0;idx<pstMediaInfo->ucTotalXsubTrack;idx++)
		{
			PrintData ("------------------------------------------------\n");
			PrintData ("Divx Subtitle Track (%u)\n",idx);
			PrintData ("Xsub Type           (%u)\n",pstMediaInfo->stXsubTrack[idx].eXsubType);
			PrintData ("Xsub Language       (%s)\n",pstMediaInfo->stXsubTrack[idx].uacXsubLang);
		}
	}

	DI_STREAM_RTSP_Close (stHandle);
	PrintExit;
	return nRet;
}


/** Open From User Application and bmedia (Probe)*/
int DI_STREAM_RTSP_OpenForProbe  (const HINT8 *pURL, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HINT32 nRet = DI_IPT_SUCCESS;
	struct RTSP_ACCESS_SYS_s * pstRtspSys = NULL;
	STREAM_ACCESS_t * pAccess = NULL;

	PrintEnter;

	UNUSED_PARAM (mode);

	if (pURL==NULL)
	{
		PrintError("%s(%d) Error> invalid URL\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Make Stream Core Handler */
	pAccess = (STREAM_ACCESS_t*)DD_MEM_Alloc(sizeof (STREAM_ACCESS_t));
	if ( pAccess == NULL )
	{
		PrintError("%s(%d) Error> Mem Alloc Failed.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		return VFIO_ERROR_IN_DEVICE;
	}
	VK_memset (pAccess, 0, sizeof (STREAM_ACCESS_t));
	ptHandle->pDnpUserData = pAccess;
	s_ptLastRtspAccess = pAccess;

	pAccess->psz_path = VK_StrDup ((void *)UTIL_GetOriginalURL ((char *)pURL) );
	if ( pAccess->psz_path == NULL )
	{
		PrintError("%s(%d) Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		if ( pAccess != NULL )
		{
			DD_MEM_Free(pAccess);
			pAccess = NULL;
		}
		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		s_ptLastRtspAccess = NULL;
		return VFIO_ERROR_IN_DEVICE;
	}
	pAccess->b_openfromnexus = FALSE;

#if SUPPORT_RTSP_SEM
	PrintData("%s(%d) sem create\n",__FUNCTION__,__LINE__);
	vRet = VK_SEM_Create(&s_ptLastRtspAccess->ulStreamSema,"RtspSema", DEFAULT_SUSPEND_TYPE);
	if ( vRet != VK_OK )
	{
		PrintError("%s(%d) Error(0x%x)> can't create SemaPhore \n\r",__FUNCTION__,__LINE__, vRet);
		s_ptLastRtspAccess->ulStreamSema = 0;
		return VFIO_ERROR_IN_DEVICE;
	}
	(void)VK_SEM_Get(s_ptLastRtspAccess->ulStreamSema);
#endif

	nRet = STREAM_RTSP_Open ( pAccess);
	if (nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) Error> STREAM_RTSP_Open Fail.\n",__FUNCTION__,__LINE__);
		if ( pAccess->psz_path != NULL )
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}

		if ( pAccess != NULL )
		{
			DD_MEM_Free(pAccess);
			pAccess = NULL;
		}

		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		s_ptLastRtspAccess = NULL;

#if SUPPORT_RTSP_SEM
		(void)VK_SEM_Release (s_ptLastRtspAccess->ulStreamSema);
#endif
		return VFIO_ERROR_IN_DEVICE;
	}

	/******************
	 * Normal Case
	 ******************/
	pstRtspSys = (struct RTSP_ACCESS_SYS_s *)(pAccess->p_sys);
	ptHandle->ucAvailable = 1;
	ptHandle->tFile.callback.unHandle = 0;
	//ptHandle->tFile.callback.unHandle = (HUINT32) pstRtspSys->netsock->sock; /** rtsp socket descriptor */
	PrintDebug("%s(%d) callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);

#if SUPPORT_RTSP_SEM
	(void)VK_SEM_Release (s_ptLastRtspAccess->ulStreamSema);
#endif

	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/** */
int DI_STREAM_RTSP_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t * pAccess = NULL;

	PrintEnter;

	pAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if (pAccess == NULL)
	{
		PrintError("pAccess is NULL\n");
		return VFIO_ERROR_SUCCESS;
	}

#if SUPPORT_RTSP_SEM
	(void)VK_SEM_Get(s_ptLastRtspAccess->ulStreamSema);
#endif

	STREAM_RTSP_Close ( pAccess);
	pAccess->p_sys = NULL;

	if (pAccess->psz_path)
	{
		DD_MEM_Free(pAccess->psz_path);
		pAccess->psz_path = NULL;
	}

	if (tHandle.pDnpUserData)
	{
		DD_MEM_Free(tHandle.pDnpUserData);
		tHandle.pDnpUserData = NULL; /* original is not seted */
	}

#if SUPPORT_RTSP_SEM
	(void)VK_SEM_Release (s_ptLastRtspAccess->ulStreamSema);
	(void)VK_SEM_Destroy(s_ptLastRtspAccess->ulStreamSema);
#endif

	PrintExit;
	return vRet;
}

/** */
int DI_STREAM_RTSP_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	STREAM_ACCESS_t * pAccess = NULL;
	size_t i_total = 0;
	size_t i_request = size * nmemb;
	int nret = VFIO_ERROR_SUCCESS;
	HINT32 i_read = 0;
	HINT32	i_retryCnt =0;

	PrintEnter;

	*ptReadSize = 0;

	pAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> pAccess is NULL\n", __FUNCTION__, __LINE__);
		return VFIO_ERROR_SUCCESS;
	}
	PrintDebug("%s(%d) ++Read Size (req:%d)\n",__FUNCTION__,__LINE__,size*nmemb);

#if SUPPORT_RTSP_SEM
	(void)VK_SEM_Get(s_ptLastRtspAccess->ulStreamSema);
#endif

	/*
	if (pAccess->info.i_size < pAccess->info.i_pos + i_request)
	{
		i_request = pAccess->info.i_size - pAccess->info.i_pos;
	}

	if (i_request > size*nmemb)
	{
		PrintDebug("######### [%s(%d)] Error> Size Invalid(%u, %"PRId64",%"PRId64") #######\n", __FUNCTION__,__LINE__, i_request, pAccess->info.i_size, pAccess->info.i_pos);
		i_request = size*nmemb;
	}
	*/

	i_retryCnt = 0;
	do
	{
func_retry:
		i_retryCnt++;

		i_read = STREAM_RTSP_Read ( pAccess, (HUINT8 *)ptr+i_total, (i_request-i_total));
		if (i_read == 0 || i_read == DI_IPT_ETIMEOUT) /* Read Error : should be exit immediately */
		{
			/* If an application requests destruction of http session like DI_IPTUNER_StopBuffering ,
			 *  this function should be returned immediately.
			 * If not so, the session may be blocked by retrying logic. Be careful.
			 */
			if (pAccess->b_immediately_exit == TRUE)
			{
				pAccess->b_retrying = FALSE;
				pAccess->b_alive = FALSE;
				nret = VFIO_ERROR_IN_DEVICE;

				PrintError("%s(%d) Error(info)> access->exit!\n", __FUNCTION__, __LINE__);
				goto func_error;
			}

			if (pAccess->info.b_eof == TRUE) /* if chunked-TE and last-chunk, */
			{
				PrintError("%s(%d) Error> EndOfStream Occured in stream RTSP controller!!\n", __FUNCTION__, __LINE__);
				break;
			}
			else
			{
				if (i_retryCnt > 100)
				{
					*ptReadSize = i_total;
					i_retryCnt = 0;
					PrintError("%s(%d) Error> Retry Time Over (%d)!!\n", __FUNCTION__, __LINE__, i_retryCnt );
					goto func_error;
				}

				PrintError("%s(%d) Error> (RetryCount=%d)...\n", __FUNCTION__, __LINE__, i_retryCnt );
				goto func_retry;
			}

		}
		else if (i_read < 0)
		{
			PrintError("%s(%d) Error> STREAM_RTSP_Read Fail(i_read:%d, i_total:%d)\n", __FUNCTION__, __LINE__, i_read, i_total);
			*ptReadSize = i_total;
			nret = VFIO_ERROR_IN_DEVICE;
			goto func_error;
		}
		/* EOF or Error */
		i_total += i_read;
	}while (i_total < i_request);

	*ptReadSize = i_total;
	PrintData("%s(%d) --tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, size, *ptReadSize);
#if SUPPORT_RTSP_SEM
	(void)VK_SEM_Release (s_ptLastRtspAccess->ulStreamSema);
#endif
	/*
	if (i_request != i_total)
	{
		PrintError("%s(%d) Error> Unmatched read size(%d != req:%d, diff:%d)\n", __FUNCTION__, __LINE__, i_total, i_request, (i_request-i_total));
		;
	}
	*/

	PrintExit;
	return VFIO_ERROR_SUCCESS;

func_error:
	PrintError("%s(%d) Error> function failed(nret:%d): Unmatched read size(%d != req:%d, diff:%d)\n", __FUNCTION__, __LINE__, nret, i_total, i_request, (i_request-i_total));
	if (i_request != i_total)
	{
		PrintError("%s(%d) Error> function failed(nret:%d): Unmatched read size(%d != req:%d, diff:%d)\n", __FUNCTION__, __LINE__, nret, i_total, i_request, (i_request-i_total));
		;
	}
#if SUPPORT_RTSP_SEM
	(void)VK_SEM_Release (s_ptLastRtspAccess->ulStreamSema);
#endif
	PrintExit;
	return nret;
}

/** */
int DI_STREAM_RTSP_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;

	STREAM_ACCESS_t * pAccess = NULL;

	PrintEnter;

	UNUSED_PARAM(whence);

	pAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> pAccess is NULL\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_SUCCESS;
	}

	nRet = STREAM_RTSP_Seek ( pAccess, (HINT64)offset );
	if (nRet != DI_IPT_SUCCESS)
	{
		nRet = VFIO_ERROR_IN_DEVICE;
		PrintError("%s(%d) Error> STREAM_RTSP_Seek fail \n",__FUNCTION__,__LINE__);
	}
	PrintExit;
	return offset;
}

/** */
int DI_STREAM_RTSP_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;

	UNUSED_PARAM(tHandle);
	UNUSED_PARAM(plCurOffset);
	PrintEnter;
	PrintError("%s (%d) Tell is Not implemented\n",__FUNCTION__,__LINE__);
	PrintExit;
	return nRet;
}

/** */
int DI_STREAM_RTSP_IsEOF (VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;

	STREAM_ACCESS_t * pAccess = NULL;

	PrintEnter;

	pAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if (pAccess == NULL)
	{
		PrintError("pAccess is NULL\n");
		return VFIO_ERROR_SUCCESS;
	}

    if (FALSE == pAccess->info.b_eof)
	{
		nRet = -1; /* must be -1 like feof*/
	}
	PrintExit;
	return nRet;
}

/* Open From Nexus */
int DI_STREAM_RTSP_OpenFromNexus (const HINT8 *pURL, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	STREAM_ACCESS_t * pAccess = NULL;
	HINT32 nRet = DI_IPT_SUCCESS;

	PrintEnter;

	UNUSED_PARAM (flags);

	if (pURL==NULL)
	{
		PrintError("%s(%d) Error> invalid pURL(%s)\n",__FUNCTION__,__LINE__,pURL);
		ptHandle->ucAvailable = 0;
		return VFIO_ERROR_IN_DEVICE;
	}

	/** psz_path must be free */
	pAccess = (STREAM_ACCESS_t*) DD_MEM_Alloc(sizeof (STREAM_ACCESS_t));
	if ( pAccess == NULL )
	{
		PrintError("%s(%d) Error> STREAM_RTSP_Open Fail.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		return VFIO_ERROR_IN_DEVICE;
	}
	VK_memset (pAccess, 0, sizeof (STREAM_ACCESS_t));
	ptHandle->pDnpUserData = pAccess;
	s_ptLastRtspAccess = pAccess;

	pAccess->psz_path = VK_StrDup((void *)UTIL_GetOriginalURL((char *)pURL) );
	if ( pAccess->psz_path == NULL )
	{
		PrintError("%s(%d) Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		if ( pAccess != NULL )
		{
			DD_MEM_Free(pAccess);
			pAccess = NULL;
		}
		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		s_ptLastRtspAccess = NULL;
		return VFIO_ERROR_IN_DEVICE;
	}
	pAccess->b_openfromnexus = TRUE;

#if SUPPORT_RTSP_SEM
	(void)VK_SEM_Get(s_ptLastRtspAccess->ulStreamSema);
#endif

	nRet = STREAM_RTSP_Open (pAccess);
	if (nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) Error> STREAM_RTSP_Open Fail.\n",__FUNCTION__,__LINE__);

		if ( pAccess->psz_path != NULL )
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}

		if ( pAccess != NULL )
		{
			DD_MEM_Free(pAccess);
			pAccess = NULL;
		}

		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		s_ptLastRtspAccess = NULL;

#if SUPPORT_RTSP_SEM
		(void)VK_SEM_Release (s_ptLastRtspAccess->ulStreamSema);
#endif
		return VFIO_ERROR_IN_DEVICE;
	}

	PrintDebug("%s(%d) pAccess->info.i_size:%lld\n", __FUNCTION__, __LINE__, pAccess->info.i_size);
	if (pAccess->info.i_size == 0)
	{
		pAccess->info.i_size = 0xFFFFFF;
		PrintError("%s(%d) Error> -pAccess->info.i_size:%lld\n", __FUNCTION__, __LINE__, pAccess->info.i_size);
	}


#if SUPPORT_RTSP_SEM
	(void)VK_SEM_Release (s_ptLastRtspAccess->ulStreamSema);
#endif

	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/* Seek From Nexus */
off_t DI_STREAM_RTSP_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	STREAM_ACCESS_t * pAccess = NULL;

	PrintEnter;

	pAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> pAccess is NULL\n", __FUNCTION__, __LINE__);
		return VFIO_ERROR_SUCCESS;
	}
	PrintData("##### %s(%d) : ++[Current i_pos=%"PRId64", offset=%"PRId64"] #####\n",__FUNCTION__,__LINE__, pAccess->info.i_pos, offset);
	PrintData("%s (%d) Action:Nothing.. offset=%"PRId64", whence=%d\n",__FUNCTION__,__LINE__, offset, whence);

	if (1)
	{
		if (whence == SEEK_END)
		{
			offset = (off_t)pAccess->info.i_size-offset;
		}
	}

#if 0
	nRet = STREAM_RTSP_Seek ( pAccess, (HINT64)offset ); /* for feed asf header when the offset is 0 */
	if (nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) Error> STREAM_RTSP_Seek fail \n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}
	else
	{
		PrintExit;
		return pAccess->info.i_pos;
	}
#endif

	//if ( pAccess->info.i_pos != offset)
	//{
		PrintData("##### %s(%d) : --[Current i_pos=%"PRId64", offset=%"PRId64"] #####\n",__FUNCTION__,__LINE__, pAccess->info.i_pos, offset);
	//}

	if ( pAccess->info.i_pos != offset)
	{
		PrintError("%s(%d) Error> i_pos=%"PRId64", req.offset=%"PRId64"] #####\n",__FUNCTION__,__LINE__, pAccess->info.i_pos, offset);
	}


	PrintExit;
	return offset;
}

/* Bounds From Nexus */
int DI_STREAM_RTSP_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t * pAccess = NULL;

	PrintEnter;

	*file_size = 0;
	pAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> pAccess is NULL\n", __FUNCTION__, __LINE__);
		return VFIO_ERROR_SUCCESS;
	}

	if (pAccess->info.i_size == 0)
	{
		PrintError("%s(%d) Error> Invalid stream size!\n", __FUNCTION__, __LINE__);
		;
	}

	*file_size = pAccess->info.i_size;
	PrintData("%s(%d) i_size=%"PRId64"\n",__FUNCTION__,__LINE__, pAccess->info.i_size);
	PrintExit;
	return nRet;
}

/**
 * @brief IPTUNER 를 초기화한다.
 * @param 	None
 * @return 	HAAL Error Code
 * @date 	2009/05/24
 * @author 	junsy@humaxdigital.com
 * @remark 	IPTUNER Index별로 DI_IPTUNER_t 정보를 저장할 배열을 초기화한다.
 * @see 	DI_IPTUNER_t
 */
void DI_STREAM_RTSP_RegisterCallback (const char *pURL)
{
	VFIO_Callback_t stCBFunction;

	PrintEnter;

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB) DI_STREAM_RTSP_OpenForProbe;
	stCBFunction.Close			= (VFIOCloseCB) DI_STREAM_RTSP_Close;
	stCBFunction.Read			= (VFIOReadCB) DI_STREAM_RTSP_Read;
	stCBFunction.Seek			= (VFIOSeekCB) DI_STREAM_RTSP_Seek;
	stCBFunction.Tell			= (VFIOTellCB) DI_STREAM_RTSP_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) DI_STREAM_RTSP_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB) DI_STREAM_RTSP_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) DI_STREAM_RTSP_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) DI_STREAM_RTSP_BoundsForNexus;

	(void) VFIO_RegisterCallback (pURL, stCBFunction);
	PrintExit;
	return;
}

void DI_STREAM_RTSP_UnregisterCallback (const char *pURL)
{
	PrintEnter;

#if 0 /* for CANVAS */
#if SUPPORT_RTSP_SEM
	if (s_ulRtspSemId != 0)
	{
		(void)VK_SEM_Get(s_ulRtspSemId);
		(void)VK_SEM_Release (s_ulRtspSemId);

		PrintDebug("%s(%d) close sem \n",__FUNCTION__,__LINE__);
	 	(void)VK_SEM_Destroy(s_ulRtspSemId);
	 	s_ulRtspSemId = 0;
	}
#endif
#endif

	(void)VFIO_UnregisterCallback (pURL);

	PrintExit;
	return;
}

DI_ERR_CODE DI_STREAM_RTSP_Control ( DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 )
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	PrintEnter;

	if (s_ptLastRtspAccess == NULL)
	{
		PrintError("%s(%d) Error> NULL prev.rtspAccess\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	nRet = STREAM_RTSP_Control (s_ptLastRtspAccess, cmd , val, val2);
	if (nRet != DI_ERR_OK)
	{
		PrintError("%s(%d) Error> STREAM_RTSP_Control Fail \n",__FUNCTION__,__LINE__);
	}

	PrintExit;
	return nRet;
}

