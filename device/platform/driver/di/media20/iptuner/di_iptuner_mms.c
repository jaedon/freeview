/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_mms.c  $
 * Version:		2.0 (for Media 2.0 with 40nm)
 * Original Author:	Jin Lee
 * Current Author:	$Author: jlee@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     MMS - VFIO Controller
 * Module:
 * Remarks:
 */

/**
 * @defgroup MMS_VFIO_CONTROLLER
 * @brief MMS VFIO Controller. This is interlocked with Nexus playback.
 */

/**
 * @author Jin Lee (jlee@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @brief MMS VFIO Controller. This is interlocked with Nexus playback.
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
 * @file di_iptuner_mms.c
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

#include "di_iptuner20.h"
#include "di_iptuner_dnp.h"
#include "di_iptuner_mms.h"
#include "di_media20.h"
#include "di_uart.h"


#include "vfio20.h"
#include "stream_mms.h"
#include "mms_tcpudp.h"
#include "mms_http.h"

/* RTSP header */
#include "stream_global.h"
#include "mms_asf.h"
#include "rtsp_lib.h"
#include "util_url.h"

#include "stream_rtsp.h"


/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define DI_IPTUNER_MMS_DEBUG

#ifdef IPTUNER_ALL_DEBUG

#ifdef DI_IPTUNER_MMS_DEBUG
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

/* Semaphore를 이용하여 Read함수와 Close함수간에 침범하지 않도록 하지 않는 경우 Rebooting문제가 발생하여 처리함 */
/* 현재는 Seek에 대해서는 Semaphore를 하지 않는데 차후 고려가 필요함. */
#define SUPPORT_MMS_SEM 0

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
static STREAM_ACCESS_t * stlastpMmsAccess ; /* for Control and Probe */

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

/**
 * @brief IPTUNER 를 초기화한다.
 * @param 	None
 * @return 	HAAL Error Code
 * @date 	2009/05/24
 * @author 	junsy@humaxdigital.com
 * @remark 	IPTUNER Index별로 DI_IPTUNER_t 정보를 저장할 배열을 초기화한다.
 * @see 	DI_IPTUNER_t
 */

void DI_STREAM_MMS_RegisterCallback (const char *pUrl)
{
#if 0 /* for CANVAS */
#if SUPPORT_MMS_SEM
	HINT32 nRet = DI_IPT_SUCCESS;
#endif
#endif
	VFIO_Callback_t stCBFunction;

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB) DI_STREAM_MMS_Open;
	stCBFunction.Close			= (VFIOCloseCB) DI_STREAM_MMS_Close;
	stCBFunction.Read			= (VFIOReadCB) DI_STREAM_MMS_Read;
	stCBFunction.Seek			= (VFIOSeekCB) DI_STREAM_MMS_Seek;
	stCBFunction.Tell			= (VFIOTellCB) DI_STREAM_MMS_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) DI_STREAM_MMS_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB) DI_STREAM_MMS_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) DI_STREAM_MMS_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) DI_STREAM_MMS_BoundsForNexus;
	(void) VFIO_RegisterCallback (pUrl, stCBFunction);

#if 0 /* for CANVAS */
#if SUPPORT_MMS_SEM
	if (s_ulMmsSemId == 0)
	{
		PrintData ("%s(%d) : sem create\n",__FUNCTION__,__LINE__);
		nRet = VK_SEM_Create(&s_ulMmsSemId,"MmsSema", DEFAULT_SUSPEND_TYPE);
		if( nRet != VK_OK )
		{
			PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
			s_ulMmsSemId = 0;
		}
	}
#endif
#endif

	return;
}

void DI_STREAM_MMS_UnregisterCallback (const char *pUrl)
{
	PrintEnter;

#if 0 /* for CANVAS */
#if SUPPORT_MMS_SEM
	if (s_ulMmsSemId != 0)
	{
		(void)VK_SEM_Get(s_ulMmsSemId);

		(void)VK_SEM_Release (s_ulMmsSemId);

		PrintDebug ("%s(%d) : close sem \n",__FUNCTION__,__LINE__);
	 	(void)VK_SEM_Destroy(s_ulMmsSemId);
	 	s_ulMmsSemId = 0;
	}
#endif
#endif

	(void)VFIO_UnregisterCallback (pUrl);

	PrintExit;
	return;
}

DI_ERR_CODE DI_STREAM_MMS_Probe (const HINT8 *path,  DI_MEDIA_INFO_t *pstMediaInfo)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	VFIO_Handle_t stHandle;
	MMSTU_ACCESS_SYS_t *pstMmsSys=NULL;
	MMSH_ACCESS_SYS_t *pstMmsHSys=NULL;
	STREAM_ACCESS_t *pMmsAccess = NULL;
	HINT32	nASFMediaListCount = 0;
	HINT32	nVideoIndex = 0;
	HINT32	nAudioIndex = 0;
	HBOOL	bWMS = TRUE; /* Adjust AV pes_id if MS-WMS is used; Change this if Maxdome */

	PrintEnter;

	PrintDebug("%s (%d) REMOTE PROBE >>>> path=(%s)\n",__FUNCTION__,__LINE__,path);
	if( VFIO_ERROR_SUCCESS != DI_STREAM_MMS_Open (path,NULL, &stHandle))
	{
		PrintError("%s (%d) Error> DI_STREAM_MMS_Open fail.\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		return nRet;
	}
	else
	{
		pMmsAccess = stlastpMmsAccess;
		if(pMmsAccess == NULL)
		{
			return DI_ERR_ERROR;
		}

		if(pMmsAccess->proto == PROTOCOL_RTSP)
		{
			/* Fixed warning: initialization from incompatible pointer type */
			struct RTSP_ACCESS_SYS_s *p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pMmsAccess->p_sys;

			pstMediaInfo->stream_type = DI_MEDIA_MPEG_TYPE_ASF;
			pstMediaInfo->max_bitrate = p_rtsp_stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo->i_max_bitrate;    /* maximum stream bitreate, bps or 0 if unknown  */
			pstMediaInfo->duration = p_rtsp_stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo->i_play_duration/10000;       /* duration of stream, in milliseconds or 0 if unknown */
			pstMediaInfo->nprograms =1;      /* total number of programs */
			pstMediaInfo->ntracks = 2;        /* total number of tracks */
			pstMediaInfo->video_pes_id = 224;
			pstMediaInfo->video_stream_id = 2;    /* or Video PID */
			if(p_rtsp_stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo->eVidCodec == MMS_ASF_CODEC_SIMPLE)
			{
				pstMediaInfo->video_codec = DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain;
			}
			else
			{
				pstMediaInfo->video_codec = DI_VIDEO_FORMAT_MEDIA_VC1;
			}
			pstMediaInfo->audio_pes_id = 192;
			pstMediaInfo->audio_stream_id = 1;    /* or Audio PID */
			pstMediaInfo->audio_codec = DI_AUDFORMAT_WMA_STD;
			pstMediaInfo->support_trick = 1;
			pstMediaInfo->support_seek = 1;
			pstMediaInfo->pcr_pid = 8191;        /* for TS stream */
#if 0 /* for CANVAS : dragon */
			pstMediaInfo->stream_size = pMmsAccess->info.i_size;        /* File Size */
#endif
#if SUPPORT_MSDRM_PD
			pstMediaInfo->is_drm			= p_rtsp_stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo->i_encrypted;
#endif /* SUPPORT_MSDRM_PD */

			if (bWMS == TRUE)
			{
				pstMediaInfo->ucTotalVideoTrack = 1;
				pstMediaInfo->stVideoTrack[0].video_pes_id = pstMediaInfo->video_pes_id;
				pstMediaInfo->stVideoTrack[0].video_stream_id = pstMediaInfo->video_stream_id;
				pstMediaInfo->stVideoTrack[0].video_codec = pstMediaInfo->video_codec;

				pstMediaInfo->ucTotalAudioTrack = 1;
				pstMediaInfo->stAudioTrack[0].audio_pes_id = pstMediaInfo->audio_pes_id;
				pstMediaInfo->stAudioTrack[0].audio_stream_id = pstMediaInfo->audio_stream_id;
				pstMediaInfo->stAudioTrack[0].audio_codec = pstMediaInfo->audio_codec;
			}
		}
		else if(pMmsAccess->proto == PROTOCOL_MMSTU)
		{
			pstMmsSys = (MMSTU_ACCESS_SYS_t *)pMmsAccess->p_sys;

			PrintData ("%s(%d) : info.i_update      =          %d \n",__FUNCTION__,__LINE__,pMmsAccess->info.i_update);
			PrintData ("%s(%d) : info.i_size        =          %"PRId64"\n",__FUNCTION__,__LINE__,pMmsAccess->info.i_size);
			PrintData ("%s(%d) : info.i_pos         =          %"PRId64"\n",__FUNCTION__,__LINE__,pMmsAccess->info.i_pos);
			PrintData ("%s(%d) : info.i_title       =          %d \n",__FUNCTION__,__LINE__,pMmsAccess->info.i_title);
			PrintData ("%s(%d) : info.i_seekpoint   =          %d \n",__FUNCTION__,__LINE__,pMmsAccess->info.i_seekpoint);
			PrintData ("%s(%d) : info.b_prebuffered =          %d \n",__FUNCTION__,__LINE__,pMmsAccess->info.b_prebuffered);

			PrintData ("%s(%d) : i_proto =                     %d \n",__FUNCTION__,__LINE__,pstMmsSys->i_proto);
			PrintData ("%s(%d) : i_handle_tcp =                %d \n",__FUNCTION__,__LINE__,pstMmsSys->i_handle_tcp);
			PrintData ("%s(%d) : i_handle_udp =                %d \n",__FUNCTION__,__LINE__,pstMmsSys->i_handle_udp);

			PrintData ("%s(%d) : asfh.i_file_size =            %"PRId64" \n",__FUNCTION__,__LINE__,pstMmsSys->asfh.i_file_size);
			PrintData ("%s(%d) : asfh.i_data_packets_count =   %"PRId64" \n",__FUNCTION__,__LINE__,pstMmsSys->asfh.i_data_packets_count);
			PrintData ("%s(%d) : asfh.i_min_data_packet_size = %d \n",__FUNCTION__,__LINE__,pstMmsSys->asfh.i_min_data_packet_size);
			PrintData ("%s(%d) : asfh.stream[0].i_cat =        %d \n",__FUNCTION__,__LINE__,pstMmsSys->asfh.stream[0].i_cat);
			PrintData ("%s(%d) : asfh.stream[0].i_bitrate =    %d \n",__FUNCTION__,__LINE__,pstMmsSys->asfh.stream[0].i_bitrate);
			PrintData ("%s(%d) : asfh.stream[0].i_selected =   %d \n",__FUNCTION__,__LINE__,pstMmsSys->asfh.stream[0].i_selected);
			PrintData ("%s(%d) : asfh.stream[1].i_cat =        %d \n",__FUNCTION__,__LINE__,pstMmsSys->asfh.stream[1].i_cat);
			PrintData ("%s(%d) : asfh.stream[1].i_bitrate =    %d \n",__FUNCTION__,__LINE__,pstMmsSys->asfh.stream[1].i_bitrate);
			PrintData ("%s(%d) : asfh.stream[1].i_selected =   %d \n",__FUNCTION__,__LINE__,pstMmsSys->asfh.stream[1].i_selected);

			PrintData ("%s(%d) : i_flags_broadcast =           %d \n",__FUNCTION__,__LINE__,pstMmsSys->i_flags_broadcast);
			PrintData ("%s(%d) : i_media_length =              %d secs \n",__FUNCTION__,__LINE__,pstMmsSys->i_media_length);
			PrintData ("%s(%d) : i_packet_length =             %d bytes\n",__FUNCTION__,__LINE__,pstMmsSys->i_packet_length);
			PrintData ("%s(%d) : i_packet_count =              %d \n",__FUNCTION__,__LINE__,pstMmsSys->i_packet_count);
			PrintData ("%s(%d) : i_max_bit_rate =              %d bps\n",__FUNCTION__,__LINE__,pstMmsSys->i_max_bit_rate);
			PrintData ("%s(%d) : i_header_size =               %d \n",__FUNCTION__,__LINE__,pstMmsSys->i_header_size);
			PrintData ("%s(%d) : b_seekable =                  %d \n",__FUNCTION__,__LINE__,pstMmsSys->b_seekable);
#if SUPPORT_MSDRM_PD
			PrintData ("%s(%d) : is_drm =                      %d \n",__FUNCTION__,__LINE__,pstMmsSys->asfh.i_encrypted);
#endif /* SUPPORT_MSDRM_PD */

			pstMediaInfo->stream_type 		= DI_MEDIA_MPEG_TYPE_ASF;
			pstMediaInfo->max_bitrate 		= pstMmsSys->i_max_bit_rate;
			pstMediaInfo->duration 			= pstMmsSys->i_media_length * 1000 ;//(1000 * 8 * pMmsAccess->info.i_size) / pstMediaInfo->max_bitrate; /* duration of stream, in milliseconds or 0 if unknown */
			pstMediaInfo->nprograms 		= 1;      /* total number of programs */
			pstMediaInfo->ntracks 			= 2;        /* total number of tracks */
			if (bWMS == TRUE)
			{
				pstMediaInfo->video_pes_id 	= 224;
			}
			else
			{
				pstMediaInfo->video_pes_id 	= 0;
			}
			pstMediaInfo->video_stream_id 	= 2;    /* or Video PID */
			pstMediaInfo->video_codec 		= DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain;
			if (bWMS == TRUE)
			{
				pstMediaInfo->audio_pes_id 	= 192;
			}
			else
			{
				pstMediaInfo->audio_pes_id 	= 0;
			}
			pstMediaInfo->audio_stream_id 	= 1;    /* or Audio PID */
			pstMediaInfo->audio_codec		= DI_AUDFORMAT_WMA_STD;
			pstMediaInfo->support_trick 	= 1;
			pstMediaInfo->support_seek 		= pstMmsSys->b_seekable;
			pstMediaInfo->pcr_pid 			= 0;        /* for TS stream */
#if 0 /* for CANVAS : dragon */
			pstMediaInfo->stream_size = pMmsAccess->info.i_size;        /* File Size */
#endif
#if SUPPORT_MSDRM_PD
			pstMediaInfo->is_drm			= pstMmsSys->asfh.i_encrypted;
#endif /* SUPPORT_MSDRM_PD */

			if (bWMS == TRUE)
			{
				pstMediaInfo->ucTotalVideoTrack = 1;
				pstMediaInfo->stVideoTrack[0].video_pes_id = pstMediaInfo->video_pes_id;
				pstMediaInfo->stVideoTrack[0].video_stream_id = pstMediaInfo->video_stream_id;
				pstMediaInfo->stVideoTrack[0].video_codec = pstMediaInfo->video_codec;

				pstMediaInfo->ucTotalAudioTrack = 1;
				pstMediaInfo->stAudioTrack[0].audio_pes_id = pstMediaInfo->audio_pes_id;
				pstMediaInfo->stAudioTrack[0].audio_stream_id = pstMediaInfo->audio_stream_id;
				pstMediaInfo->stAudioTrack[0].audio_codec = pstMediaInfo->audio_codec;
			}
		}
		else if (pMmsAccess->proto == PROTOCOL_MMSH)
		{
			pstMmsHSys = (MMSH_ACCESS_SYS_t *)pMmsAccess->p_sys;
			PrintData ("%s(%d) : info.i_update      =          %d \n",__FUNCTION__,__LINE__,pMmsAccess->info.i_update);
			PrintData ("%s(%d) : info.i_size        =          %"PRId64"\n",__FUNCTION__,__LINE__,pMmsAccess->info.i_size);
			PrintData ("%s(%d) : info.i_pos         =          %"PRId64"\n",__FUNCTION__,__LINE__,pMmsAccess->info.i_pos);
			PrintData ("%s(%d) : info.i_title       =          %d \n",__FUNCTION__,__LINE__,pMmsAccess->info.i_title);
			PrintData ("%s(%d) : info.i_seekpoint   =          %d \n",__FUNCTION__,__LINE__,pMmsAccess->info.i_seekpoint);
			PrintData ("%s(%d) : info.b_prebuffered =          %d \n",__FUNCTION__,__LINE__,pMmsAccess->info.b_prebuffered);

			PrintData ("%s(%d) : i_request_context =           %d \n",__FUNCTION__,__LINE__,pstMmsHSys->i_request_context);
			PrintData ("%s(%d) : b_broadcast =                 %d \n",__FUNCTION__,__LINE__,pstMmsHSys->b_broadcast);

			PrintData ("%s(%d) : asfh.i_file_size =            %"PRId64"\n",__FUNCTION__,__LINE__,pstMmsHSys->asfh.i_file_size);
			PrintData ("%s(%d) : asfh.i_data_packets_count =   %"PRId64"\n",__FUNCTION__,__LINE__,pstMmsHSys->asfh.i_data_packets_count);
			PrintData ("%s(%d) : asfh.i_min_data_packet_size = %d \n",__FUNCTION__,__LINE__,pstMmsHSys->asfh.i_min_data_packet_size);
			PrintData ("%s(%d) : asfh.stream[0].i_cat =        %d \n",__FUNCTION__,__LINE__,pstMmsHSys->asfh.stream[0].i_cat);
			PrintData ("%s(%d) : asfh.stream[0].i_bitrate =    %d \n",__FUNCTION__,__LINE__,pstMmsHSys->asfh.stream[0].i_bitrate);
			PrintData ("%s(%d) : asfh.stream[0].i_selected =   %d \n",__FUNCTION__,__LINE__,pstMmsHSys->asfh.stream[0].i_selected);
			PrintData ("%s(%d) : asfh.stream[1].i_cat =        %d \n",__FUNCTION__,__LINE__,pstMmsHSys->asfh.stream[1].i_cat);
			PrintData ("%s(%d) : asfh.stream[1].i_bitrate =    %d \n",__FUNCTION__,__LINE__,pstMmsHSys->asfh.stream[1].i_bitrate);
			PrintData ("%s(%d) : asfh.stream[1].i_selected =   %d \n",__FUNCTION__,__LINE__,pstMmsHSys->asfh.stream[1].i_selected);
#if SUPPORT_MSDRM_PD
			PrintData ("%s(%d) : is_drm =                      %d \n",__FUNCTION__,__LINE__,pstMmsHSys->asfh.i_encrypted);
#endif /* SUPPORT_MSDRM_PD */

			for(nASFMediaListCount =0; nASFMediaListCount<128;nASFMediaListCount++)
			{
				if(pstMmsHSys->asfh.stream[nASFMediaListCount].i_cat == MMS_ASF_STREAM_VIDEO)
				{
					PrintData ("%s(%d) : [Video] asfh.stream[%d].i_bitrate =    %d \n",__FUNCTION__,__LINE__, nASFMediaListCount, pstMmsHSys->asfh.stream[nASFMediaListCount].i_bitrate);
					PrintData ("%s(%d) : [Video] asfh.stream[%d].i_selected =   %d \n",__FUNCTION__,__LINE__, nASFMediaListCount, pstMmsHSys->asfh.stream[nASFMediaListCount].i_selected);
					nVideoIndex = nASFMediaListCount;
				}
				else if(pstMmsHSys->asfh.stream[nASFMediaListCount].i_cat == MMS_ASF_STREAM_AUDIO)
				{
					PrintData ("%s(%d) : [Audio] asfh.stream[%d].i_bitrate =    %d \n",__FUNCTION__,__LINE__, nASFMediaListCount, pstMmsHSys->asfh.stream[nASFMediaListCount].i_bitrate);
					PrintData ("%s(%d) : [Audio] asfh.stream[%d].i_selected =   %d \n",__FUNCTION__,__LINE__, nASFMediaListCount, pstMmsHSys->asfh.stream[nASFMediaListCount].i_selected);
					nAudioIndex = nASFMediaListCount;
				}

				if((nVideoIndex != 0) && (nAudioIndex != 0))
				{
					break;
				}
			}

			pstMediaInfo->stream_type 		= DI_MEDIA_MPEG_TYPE_ASF;
			pstMediaInfo->max_bitrate 		= pstMmsHSys->asfh.stream[nVideoIndex].i_bitrate;    /* maximum stream bitreate, bps or 0 if unknown  */

			PrintData ("%s(%d) : pstMediaInfo->max_bitrate = %d\n",__FUNCTION__,__LINE__, pstMediaInfo->max_bitrate);

			if((pMmsAccess->info.i_size == 0) || (pstMediaInfo->max_bitrate == 0))
			{
				PrintError("%s(%d) : info.i_size = %"PRId64", pstMediaInfo->max_bitrate = %u\n",__FUNCTION__,__LINE__,pMmsAccess->info.i_size, pstMediaInfo->max_bitrate);
				pstMediaInfo->duration 			= 0;   /* duration of stream, in milliseconds or 0 if unknown */
			}
			else
			{
			pstMediaInfo->duration 			= (1000*8*pMmsAccess->info.i_size) / pstMediaInfo->max_bitrate;       /* duration of stream, in milliseconds or 0 if unknown */
			}

			pstMediaInfo->nprograms 		= 1;      /* total number of programs */
			pstMediaInfo->ntracks 			= 2;        /* total number of tracks */
			if (bWMS == TRUE)
			{
				pstMediaInfo->video_pes_id 	= 224;
			}
			else
			{
				pstMediaInfo->video_pes_id 	= 0;
			}
			pstMediaInfo->video_stream_id 	= 2;    /* or Video PID */
			pstMediaInfo->video_codec		= DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain;
			if (bWMS == TRUE)
			{
				pstMediaInfo->audio_pes_id 	= 192;
			}
			else
			{
				pstMediaInfo->audio_pes_id 	= 0;
			}
			pstMediaInfo->audio_stream_id 	= 1;    /* or Audio PID */
			pstMediaInfo->audio_codec		= DI_AUDFORMAT_WMA_STD;
			pstMediaInfo->support_trick 	= 0;
			pstMediaInfo->support_seek 		= 1;
			pstMediaInfo->pcr_pid 			= 8191;        /* for TS stream */
#if 0 /* for CANVAS */
			pstMediaInfo->stream_size = pMmsAccess->info.i_size;        /* File Size */
#endif
#if SUPPORT_MSDRM_PD
			pstMediaInfo->is_drm			= pstMmsHSys->asfh.i_encrypted;
#endif /* SUPPORT_MSDRM_PD */

			if (bWMS == TRUE)
			{
				pstMediaInfo->ucTotalVideoTrack = 1;
				pstMediaInfo->stVideoTrack[0].video_pes_id = pstMediaInfo->video_pes_id;
				pstMediaInfo->stVideoTrack[0].video_stream_id = pstMediaInfo->video_stream_id;
				pstMediaInfo->stVideoTrack[0].video_codec = pstMediaInfo->video_codec;

				pstMediaInfo->ucTotalAudioTrack = 1;
				pstMediaInfo->stAudioTrack[0].audio_pes_id = pstMediaInfo->audio_pes_id;
				pstMediaInfo->stAudioTrack[0].audio_stream_id = pstMediaInfo->audio_stream_id;
				pstMediaInfo->stAudioTrack[0].audio_codec = pstMediaInfo->audio_codec;
			}
		}
	}

	DI_STREAM_MMS_Close (stHandle);

	PrintExit;
	return nRet;
}


/** Open From User Application and bmedia*/
int DI_STREAM_MMS_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HINT32 nRet = DI_IPT_SUCCESS;
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	MMSTU_ACCESS_SYS_t *pstMmsSys =NULL;
	MMSH_ACCESS_SYS_t *pstMmsHSys =NULL;
	struct stream_rtsp_t * pstMmsRSys = NULL;
	STREAM_ACCESS_t * pMmsAccess = NULL;

	PrintEnter;
	UNUSED_PARAM (mode);

	/** psz_path must be free */
	pMmsAccess = DD_MEM_Alloc(sizeof (STREAM_ACCESS_t));
	if ( pMmsAccess == NULL )
	{
		PrintError("%s(%d) : Error> STREAM_MMS_Open: Mem Alloc Failed.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		vRet = VFIO_ERROR_IN_DEVICE;
		return vRet;
	}

	VK_memset (pMmsAccess, 0, sizeof (STREAM_ACCESS_t));
	ptHandle->pDnpUserData = pMmsAccess;
	stlastpMmsAccess = pMmsAccess;

	pMmsAccess->psz_path = VK_StrDup((void *)UTIL_GetOriginalURL ((char *)path) );
	if ( pMmsAccess->psz_path == NULL )
	{
		if ( pMmsAccess != NULL )
		{
			DD_MEM_Free(pMmsAccess);
			pMmsAccess = NULL;
		}
		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		stlastpMmsAccess = NULL;
		vRet = VFIO_ERROR_IN_DEVICE;
		return vRet;
	}

	pMmsAccess->b_openfromnexus = FALSE;


#if SUPPORT_MMS_SEM /* for CANVAS : dragon */
	PrintData ("%s(%d) : sem create\n",__FUNCTION__,__LINE__);
	vRet = VK_SEM_Create(&stlastpMmsAccess->ulStreamSema,"MmsSema", DEFAULT_SUSPEND_TYPE);
	if( vRet != VK_OK )
	{
		PrintError("%s(%d) Error(0x%x)> can't create SemaPhore \n\r",__FUNCTION__,__LINE__, vRet);
		stlastpMmsAccess->ulStreamSema = 0;
		return VFIO_ERROR_IN_DEVICE;
	}
	(void)VK_SEM_Get(stlastpMmsAccess->ulStreamSema);
#endif

	nRet = STREAM_MMS_Open ( pMmsAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) : Error> STREAM_MMS_Open Fail.\n",__FUNCTION__,__LINE__);
		if ( pMmsAccess->psz_path != NULL )
		{
			DD_MEM_Free(pMmsAccess->psz_path);
			pMmsAccess->psz_path = NULL;
		}

		if ( pMmsAccess != NULL )
		{
			DD_MEM_Free(pMmsAccess);
			pMmsAccess = NULL;
		}

		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		stlastpMmsAccess = NULL;
		vRet = VFIO_ERROR_IN_DEVICE;

#if SUPPORT_MMS_SEM
		(void)VK_SEM_Release (stlastpMmsAccess->ulStreamSema);
#endif
		return vRet;
	}
	else
	{
		if(pMmsAccess->proto == PROTOCOL_MMSTU)
		{
			pstMmsSys = (MMSTU_ACCESS_SYS_t *)pMmsAccess->p_sys;
			ptHandle->ucAvailable = 1;
			if(pstMmsSys->i_proto ==  MMS_PROTO_TCP)
			{
				ptHandle->tFile.callback.unHandle = (HUINT32) pstMmsSys->i_handle_tcp; /** TCP socket descriptor */
			}
			else if (pstMmsSys->i_proto ==  MMS_PROTO_UDP)
			{
				ptHandle->tFile.callback.unHandle = (HUINT32) pstMmsSys->i_handle_udp; /** UDP socket descriptor */
			}
		}
		else if (pMmsAccess->proto == PROTOCOL_MMSH)
		{
			pstMmsHSys = (MMSH_ACCESS_SYS_t *)pMmsAccess->p_sys;
			ptHandle->ucAvailable = 1;
			ptHandle->tFile.callback.unHandle = (HUINT32) pstMmsHSys->fd; /** http socket descriptor */
		}
		else if (pMmsAccess->proto == PROTOCOL_RTSP)
		{
			pstMmsRSys = (struct stream_rtsp_t *)(pMmsAccess->p_sys);
			ptHandle->ucAvailable = 1;
			ptHandle->tFile.callback.unHandle = 0;
			//ptHandle->tFile.callback.unHandle = (HUINT32) pstMmsRSys->netsock->sock; /** rtsp socket descriptor */
		}

		PrintDebug ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
	}

#if SUPPORT_MMS_SEM
	(void)VK_SEM_Release (stlastpMmsAccess->ulStreamSema);
#endif

	PrintExit;
	return vRet;
}

/** */
int DI_STREAM_MMS_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t * pMmsAccess = NULL;

	PrintEnter;

	pMmsAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if(pMmsAccess == NULL)
	{
		PrintError("pMmsAccess is NULL\n");
		return VFIO_ERROR_SUCCESS;
	}

#if SUPPORT_MMS_SEM
	(void)VK_SEM_Get(stlastpMmsAccess->ulStreamSema);
#endif

	STREAM_MMS_Close ( pMmsAccess);
	pMmsAccess->p_sys = NULL;

	if(pMmsAccess->psz_path)
	{
		DD_MEM_Free(pMmsAccess->psz_path);
		pMmsAccess->psz_path = NULL;
	}

	if(tHandle.pDnpUserData)
	{
		DD_MEM_Free(tHandle.pDnpUserData);
		tHandle.pDnpUserData = NULL; /* original is not seted */
	}

#if SUPPORT_MMS_SEM
	(void)VK_SEM_Release (stlastpMmsAccess->ulStreamSema);
 	(void)VK_SEM_Destroy(stlastpMmsAccess->ulStreamSema);
#endif

	PrintExit;
	return vRet;
}




int DI_STREAM_MMS_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	STREAM_ACCESS_t * pMmsAccess = NULL;
	int nReadSizet = 0;

	PrintEnter;

	pMmsAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if(pMmsAccess == NULL)
	{
		PrintError("pMmsAccess is NULL\n");
		return VFIO_ERROR_SUCCESS;
	}

	//PrintEnter;
	PrintDebug("%s (%d) Read Size (%d)\n",__FUNCTION__,__LINE__,size*nmemb);

#if SUPPORT_MMS_SEM
	(void)VK_SEM_Get(stlastpMmsAccess->ulStreamSema);
#endif

	nReadSizet = STREAM_MMS_Read ( pMmsAccess, ptr, size*nmemb );
	if(nReadSizet >= 0)
	{
		*ptReadSize = nReadSizet;
	}
	else
	{
		*ptReadSize = 0;
	}


#if SUPPORT_MMS_SEM
	(void)VK_SEM_Release (stlastpMmsAccess->ulStreamSema);
#endif

	if(nReadSizet < 0)
	{
		PrintExit;
		return VFIO_ERROR_FAIL;
	}

	PrintData("%s(%d) : tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__,size, *ptReadSize);
	PrintExit;

	return VFIO_ERROR_SUCCESS;
}

/** */
int DI_STREAM_MMS_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;

	STREAM_ACCESS_t * pMmsAccess = NULL;

	PrintEnter;

	UNUSED_PARAM(whence);

	pMmsAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if(pMmsAccess == NULL)
	{
		PrintError ("%s(%d) Error> pMmsAccess is NULL\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_SUCCESS;
	}

	nRet = STREAM_MMS_Seek ( pMmsAccess, (HINT64)offset );
	if(nRet != DI_IPT_SUCCESS)
	{
		nRet = VFIO_ERROR_IN_DEVICE;
		PrintError ("%s(%d) Error> STREAM_MMS_Seek fail \n",__FUNCTION__,__LINE__);
	}
	PrintExit;
	return offset;
}

/** */
int DI_STREAM_MMS_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;

	UNUSED_PARAM(tHandle);
	UNUSED_PARAM(plCurOffset);
	PrintEnter;
	PrintError ("%s (%d) Tell is Not implemented\n",__FUNCTION__,__LINE__);
	PrintExit;
	return nRet;
}

/** */
int DI_STREAM_MMS_IsEOF (VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;

	STREAM_ACCESS_t * pMmsAccess = NULL;

	PrintEnter;

	pMmsAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if(pMmsAccess == NULL)
	{
		PrintError("pMmsAccess is NULL\n");
		return VFIO_ERROR_SUCCESS;
	}

    if(FALSE == pMmsAccess->info.b_eof)
	{
		nRet = -1; /* must be -1 like feof*/
	}
	PrintExit;
	return nRet;
}

/* Open From Nexus */
int DI_STREAM_MMS_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	HINT32 nRet = DI_IPT_SUCCESS;
	MMSTU_ACCESS_SYS_t *pstMmsSys=NULL;
	MMSH_ACCESS_SYS_t *pstMmsHSys=NULL;
	STREAM_ACCESS_t * pMmsAccess = NULL;

	UNUSED_PARAM (flags);
	PrintEnter;

	/** psz_path must be free */
	pMmsAccess = DD_MEM_Alloc(sizeof (STREAM_ACCESS_t));
	if ( pMmsAccess == NULL )
	{
		PrintError("%s (%d) : Error> STREAM_MMS_Open Fail.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		return VFIO_ERROR_IN_DEVICE;
	}

	VK_memset (pMmsAccess, 0, sizeof (STREAM_ACCESS_t));
	ptHandle->pDnpUserData = pMmsAccess;
	stlastpMmsAccess = pMmsAccess;

	pMmsAccess->psz_path = VK_StrDup((void *)UTIL_GetOriginalURL((char *)path) );
	if ( pMmsAccess->psz_path == NULL )
	{
		if ( pMmsAccess != NULL )
		{
			DD_MEM_Free(pMmsAccess);
			pMmsAccess = NULL;
		}
		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		stlastpMmsAccess = NULL;
		return VFIO_ERROR_IN_DEVICE;
	}

	pMmsAccess->b_openfromnexus = TRUE;

#if SUPPORT_MMS_SEM
	(void)VK_SEM_Get(stlastpMmsAccess->ulStreamSema);
#endif

	nRet = STREAM_MMS_Open (pMmsAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s (%d) : Error> STREAM_MMS_Open Fail.\n",__FUNCTION__,__LINE__);

		if ( pMmsAccess->psz_path != NULL )
		{
			DD_MEM_Free(pMmsAccess->psz_path);
			pMmsAccess->psz_path = NULL;
		}

		if ( pMmsAccess != NULL )
		{
			DD_MEM_Free(pMmsAccess);
			pMmsAccess = NULL;
		}

		ptHandle->ucAvailable = 0;
		ptHandle->pDnpUserData = NULL;
		stlastpMmsAccess = NULL;

#if SUPPORT_MMS_SEM
		(void)VK_SEM_Release (stlastpMmsAccess->ulStreamSema);
#endif
		return VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		if(pMmsAccess->proto == PROTOCOL_MMSTU)
		{
			pstMmsSys = (MMSTU_ACCESS_SYS_t *)pMmsAccess->p_sys;
			ptHandle->ucAvailable = 1;
			if(pstMmsSys->i_proto ==  MMS_PROTO_TCP)
			{
				ptHandle->tFile.callback.unHandle = (HUINT32) pstMmsSys->i_handle_tcp; /** TCP socket descriptor */
			}
			else if (pstMmsSys->i_proto ==  MMS_PROTO_UDP)
			{
				ptHandle->tFile.callback.unHandle = (HUINT32) pstMmsSys->i_handle_udp; /** UDP socket descriptor */
			}
		}
		else if (pMmsAccess->proto == PROTOCOL_MMSH)
		{
			pstMmsHSys = (MMSH_ACCESS_SYS_t *)pMmsAccess->p_sys;
			ptHandle->ucAvailable = 1;
			ptHandle->tFile.callback.unHandle = (HUINT32) pstMmsHSys->fd; /** http socket descriptor */
		}

		PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);

	}

#if SUPPORT_MMS_SEM
	(void)VK_SEM_Release (stlastpMmsAccess->ulStreamSema);
#endif

	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/* Seek From Nexus */
off_t DI_STREAM_MMS_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	STREAM_ACCESS_t * pMmsAccess = NULL;
	PrintEnter;
	pMmsAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if(pMmsAccess == NULL)
	{
		PrintError ("%s(%d) Error> pMmsAccess is NULL\n", __FUNCTION__, __LINE__);
		return VFIO_ERROR_SUCCESS;
	}
	PrintData ("%s (%d) Action:Nothing.. offset=%"PRId64", whence=%d\n",__FUNCTION__,__LINE__, offset, whence);

	if (pMmsAccess->proto == PROTOCOL_RTSP)
	{
		HINT32 nRet = DI_IPT_SUCCESS;

		nRet = STREAM_MMS_Seek ( pMmsAccess, (HINT64)offset ); /* for feed asf header when the offset is 0 */
		if(nRet != DI_IPT_SUCCESS)
		{
			PrintError ("%s(%d) Error> STREAM_MMS_Seek fail \n",__FUNCTION__,__LINE__);
			PrintExit;
			return -1;
		}
		else
		{
			PrintExit;

			return pMmsAccess->info.i_pos;
		}
	}
	else
	{
		if( pMmsAccess->info.i_pos != offset)
		{
			PrintData ("##### %s(%d) : Current offset=%"PRId64", offset=%"PRId64" #####\n",__FUNCTION__,__LINE__, pMmsAccess->info.i_pos, offset);
		}

		PrintExit;
		return offset;
	}
}

/* Bounds From Nexus */
int DI_STREAM_MMS_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;

	STREAM_ACCESS_t * pMmsAccess = NULL;

	//PrintEnter;

	pMmsAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if(pMmsAccess == NULL)
	{
		PrintError("pMmsAccess is NULL\n");
		return VFIO_ERROR_SUCCESS;
	}

//	PrintEnter;

	*file_size = pMmsAccess->info.i_size;

//	PrintExit;
	return nRet;
}


DI_ERR_CODE DI_STREAM_MMS_Control ( DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 )
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	PrintEnter;

	if(stlastpMmsAccess == NULL)
	{
		return DI_ERR_ERROR;
	}


	nRet = STREAM_MMS_Control (stlastpMmsAccess, cmd , val, val2);
	if(nRet != DI_ERR_OK)
	{
		PrintError("%s (%d) Error> STREAM_MMS_Control Fail \n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return nRet;
}

