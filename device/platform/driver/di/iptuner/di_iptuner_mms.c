/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_mms.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:	     MMS DNP Receiver & VFIO Functions for Streaming
 * Module:
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
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "htype.h"

#include "vkernel.h"
#include "di_uart.h"
#include "taskdef.h"

#include "di_iptuner.h"
#include "di_iptuner_dnp.h"
#include "di_iptuner_mms.h"
#include "di_media.h"

#if STREAM_BUFFERING_MMS
#include "di_iptuner_stream.h"
#endif

#include "vfio.h"
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
//#define DI_IPTUNER_MMS_CRITICAL_DEBUG

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
#define SUPPORT_MMS_SEM 1

//#define FILE_TEST
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
#if STREAM_BUFFERING_MMS
static HUINT8 	*g_MmsBuffer;
static HINT32	LOWEST_MMS_BUFFERING_THRESHOLD;
static HINT32	HIGHEST_MMS_BUFFERING_THRESHOLD;
static HINT32 	MMS_BUF_START;
static HINT32 	MMS_BUF_END;
static HUINT8 	*g_mmsptrW=NULL;
static HUINT8 	*g_mmsptrR=NULL;
static HINT32	g_MMS_BUFFERING_SIZE;

static HBOOL	s_isProbeComplete	= FALSE;
static HBOOL	s_isSeeking			= FALSE;
static HBOOL	s_isTricking		= FALSE;
static HBOOL	s_isPlaying			= FALSE;
static HBOOL	s_preBufferTaskWakeUp 			= FALSE;

#define MMS_READ_BLOCK_SIZE (64*1024) // 64Kbytes
#endif

#ifdef FILE_TEST
static HINT32 	MainHanlde=0;
static int	s_TempFd;
#endif

//static STREAM_ACCESS_t stMmsAccess ; /* remove for multi handling */
static STREAM_ACCESS_t * stlastpMmsAccess ; /* for Control and Probe */

#if SUPPORT_MMS_SEM
static unsigned long	s_ulMmsSemId=0;
#endif

static HINT32 s_nIsEventSend=0;
static HINT64 nSumLength=0;
static HINT32 s_nMmsStopReceiving=FALSE;
static HINT32 s_nMmsPauseReceiving=FALSE;
#if STREAM_BUFFERING_MMS
static DI_IPTUNER_PlayCallback_t stMmsBufferingCallback;
#endif


static int s_nWfd = -1;
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

#define MAX_MMS_RECV_BUFF_SIZE (4096*3)	/** 6KB */

void DI_STREAM_MMS_DnpStartReceiving (void)
{
	PrintEnter;
	s_nMmsStopReceiving = FALSE;
	s_nMmsPauseReceiving = FALSE;
	PrintExit;
	return;
}
void DI_STREAM_MMS_DnpStopReceiving (void)
{
	PrintEnter;
	s_nMmsStopReceiving = TRUE;
	PrintExit;
	return;
}

void DI_STREAM_MMS_DnpPauseReceiving (void)
{
	PrintEnter;
	s_nMmsPauseReceiving = TRUE;
	PrintExit;
	return;
}
void DI_STREAM_MMS_DnpResumeReceiving (void)
{
	PrintEnter;
	s_nMmsPauseReceiving = FALSE;
	PrintExit;
	return;
}

int DI_STREAM_MMS_DnpReceive (VFIO_Handle_t tHandle)
{
	HINT8 *pBuffer =NULL;
	HINT32 nRecvLength=0;
	HINT64 nContentLength ;
	HINT32 flags = O_CREAT|O_TRUNC|O_WRONLY;
	HINT32 nRc = -1;
	STREAM_ACCESS_t * pMmsAccess = NULL;
	nSumLength = 0;

	PrintEnter;



	pMmsAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	if(pMmsAccess == NULL)
	{
		PrintError("pMmsAccess is NULL\n");
		return -1;
	}

	nContentLength =pMmsAccess->info.i_size;

	/* 1. Open DNP FilePath */
	s_nWfd = open (DI_IPTUNER_DNP_FILE_PATH, flags, 0666);
	if(s_nWfd<0)
	{
		PrintError ("%s(%d) :  Error> DI_IPTUNER_DNP_FILE_PATH open fail (MMS_DNP) \n",__FUNCTION__,__LINE__);
		return -1;
	}

	PrintDebug ("%s(%d): Open %s.\n",__FUNCTION__,__LINE__,DI_IPTUNER_DNP_FILE_PATH);
	PrintDebug ("%s(%d): nContentLength (%"PRId64")bytes \n",__FUNCTION__,__LINE__,nContentLength);

	/* heap allocation */
	pBuffer = (HINT8*) DD_MEM_Alloc (MAX_MMS_RECV_BUFF_SIZE);
	if(pBuffer==NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,MAX_MMS_RECV_BUFF_SIZE);
		goto exit_stop;
	}
	(void) DI_IPTUNER_DnpPutFileLength (nContentLength);

	do
	{
		if ( TRUE == s_nMmsStopReceiving)
		{
			PrintDebug("%s(%d) : User Stoped Receiving !!\n",__FUNCTION__,__LINE__);
			goto exit_stop;
		}

		if ( TRUE == s_nMmsPauseReceiving)
		{
			PrintDebug("%s(%d) : User Paused Receiving !!\n",__FUNCTION__,__LINE__);
			VK_TASK_Sleep(100);
			continue;
		}

		nRecvLength = 0;
		VK_memset  (pBuffer,0,MAX_MMS_RECV_BUFF_SIZE);

		/*XXX: Error check */
		(void) DI_STREAM_MMS_Read (tHandle, pBuffer, MAX_MMS_RECV_BUFF_SIZE, 0, (size_t *)&nRecvLength);

		PrintData ("%s (%d) : total(%"PRId64") recv(%d)\n",__FUNCTION__,__LINE__,nContentLength, nRecvLength);

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
			continue;
		}
		/* normal case */
		else
		{
			nRc = write (s_nWfd, pBuffer, nRecvLength);
			if(nRc<0)
			{
				PrintError ("%s(%d) : Error > Write Error Occured !! \n",__FUNCTION__,__LINE__);
			}

			/* Send Event for Probing (64Kbytes) */
			if ( (s_nIsEventSend == 0) && (nSumLength > (2048*64)) ) //(1024*64)) )
			{
				s_nIsEventSend = 1;
				DI_IPTUNER_DnpSendEvent ();
			}
			/* else { continue; } */

			nSumLength += nRecvLength;
			PrintData ("%s (%d) : sum(%"PRId64") remained(%"PRId64")\n",__FUNCTION__,__LINE__,nSumLength, nContentLength-nSumLength);
			usleep(0);
		}
	} while ( nSumLength < nContentLength );

exit_stop:
	DD_MEM_Free (pBuffer);
	close(s_nWfd);
	s_nWfd = -1;

	PrintExit;
	return 0;
}

void DI_STREAM_MMS_RegisterCallback (void)
{
	HINT32 nRet = DI_IPT_SUCCESS;
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
	(void) VFIO_RegisterCallback (VFIO_CALLBACK_TYPE_STREAMING_MMS, stCBFunction);

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

	return;
}

void DI_STREAM_MMS_UnregisterCallback (void)
{
	PrintEnter;

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

	(void)VFIO_UnregisterCallback (VFIO_CALLBACK_TYPE_STREAMING_MMS);

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
			pstMediaInfo->max_bitrate = p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo->i_max_bitrate;    /* maximum stream bitreate, bps or 0 if unknown  */
			pstMediaInfo->duration = p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo->i_play_duration/10000;       /* duration of stream, in milliseconds or 0 if unknown */
			pstMediaInfo->nprograms =1;      /* total number of programs */
			pstMediaInfo->ntracks = 2;        /* total number of tracks */
			pstMediaInfo->video_pes_id = 224;
			pstMediaInfo->video_stream_id = 2;    /* or Video PID */
			if(p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo->eVidCodec == MMS_ASF_CODEC_SIMPLE)
			{
				pstMediaInfo->video_codec = 9; //DI_MEDIA_VIDEO_FORMAT_VC1SimpleMain
			}
			else
			{
				pstMediaInfo->video_codec = 10; //DI_MEDIA_VIDEO_FORMAT_VC1
			}
			pstMediaInfo->audio_pes_id = 192;
			pstMediaInfo->audio_stream_id = 1;    /* or Audio PID */
			pstMediaInfo->audio_codec = 14;
			pstMediaInfo->support_trick = 1;
			pstMediaInfo->support_seek = 1;
			pstMediaInfo->pcr_pid = 8191;        /* for TS stream */
#if SUPPORT_MSDRM_PD
			pstMediaInfo->is_drm			= p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo->i_encrypted;
#endif /* SUPPORT_MSDRM_PD */
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
			pstMediaInfo->video_pes_id 		= 0;
			pstMediaInfo->video_stream_id 	= 2;    /* or Video PID */
			pstMediaInfo->video_codec 		= 9; //DI_MEDIA_VIDEO_FORMAT_VC1;
			pstMediaInfo->audio_pes_id 		= 0;
			pstMediaInfo->audio_stream_id 	= 1;    /* or Audio PID */
			pstMediaInfo->audio_codec 		= 14;
			pstMediaInfo->support_trick 	= 1;
			pstMediaInfo->support_seek 		= pstMmsSys->b_seekable;
			pstMediaInfo->pcr_pid 			= 0;        /* for TS stream */
#if SUPPORT_MSDRM_PD
			pstMediaInfo->is_drm			= pstMmsSys->asfh.i_encrypted;
#endif /* SUPPORT_MSDRM_PD */
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
			pstMediaInfo->video_pes_id 		= 0;
			pstMediaInfo->video_stream_id 	= 2;    /* or Video PID */
			pstMediaInfo->video_codec 		= 9; //DI_MEDIA_VIDEO_FORMAT_VC1;
			pstMediaInfo->audio_pes_id 		= 0;
			pstMediaInfo->audio_stream_id 	= 1;    /* or Audio PID */
			pstMediaInfo->audio_codec 		= 14;
			pstMediaInfo->support_trick 	= 0;
			pstMediaInfo->support_seek 		= 1;
			pstMediaInfo->pcr_pid 			= 8191;        /* for TS stream */
#if SUPPORT_MSDRM_PD
			pstMediaInfo->is_drm			= pstMmsHSys->asfh.i_encrypted;
#endif /* SUPPORT_MSDRM_PD */
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

	pMmsAccess->psz_path = VK_StrDup((void*)path);

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

#if SUPPORT_MMS_SEM
	if (s_ulMmsSemId != 0)
	{
		(void)VK_SEM_Get(s_ulMmsSemId);
	}
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
		if (s_ulMmsSemId != 0)
		{
			(void)VK_SEM_Release (s_ulMmsSemId);
		}
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
		if (s_ulMmsSemId != 0)
		{
			(void)VK_SEM_Release (s_ulMmsSemId);
		}
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
	if (s_ulMmsSemId != 0)
	{
		(void)VK_SEM_Get(s_ulMmsSemId);
	}
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
	if (s_ulMmsSemId != 0)
	{
		(void)VK_SEM_Release (s_ulMmsSemId);
	}
#endif

#if STREAM_BUFFERING_MMS
s_isProbeComplete = FALSE;
#endif

#ifdef FILE_TEST
	if(MainHanlde == tHandle.tFile.callback.unHandle)
	{
		close (s_TempFd);
	}

#endif

	PrintExit;
	return vRet;
}



#if STREAM_BUFFERING_MMS
static int P_STREAM_MMS_GetBufferSize (void)
{
	int nRet=0;

	if((int)g_mmsptrW > (int)g_mmsptrR)
	{
		nRet = (int)g_mmsptrW - (int)g_mmsptrR;
	}
	else if ((int)g_mmsptrW == (int)g_mmsptrR)
	{
		/** 초기 state를 제외하고는 g_mmsptrW 와 ptrR이 동일할 수 없다. */
		nRet = 0;
	}
	else
	{
		nRet = g_MMS_BUFFERING_SIZE - ((int)g_mmsptrR - (int)g_mmsptrW);
	}

	return nRet;
}


static int P_STREAM_MMS_WriteBuffer ( HUINT8 *pBuffer, size_t len)
{
	size_t space =0;
	int size1=0;
	int size2=0;

	PrintEnter;

	space = (g_MMS_BUFFERING_SIZE - P_STREAM_MMS_GetBufferSize()) - 1;

	if(space < len)
	{
		while ((space = (g_MMS_BUFFERING_SIZE - P_STREAM_MMS_GetBufferSize()) -1) < len )
		{
			PrintDebug("%s(%d) MMS_WRITE Waiting IO \n",__FUNCTION__,__LINE__);
			sleep(1);
		}
	}

	(void) VK_SEM_Get (s_ulBufSema);

	if ( ((int)g_mmsptrW + (int)len) > (int)MMS_BUF_END)
	{
		size1 = (int)MMS_BUF_END - (int)g_mmsptrW;
		VK_memcpy (g_mmsptrW,pBuffer,size1);

		size2 = (int)len-(int)size1;
		VK_memcpy((void*)MMS_BUF_START, pBuffer+size1,size2);

		g_mmsptrW = (HUINT8*)(MMS_BUF_START + size2);
	}
	else
	{
		VK_memcpy(g_mmsptrW,pBuffer,len);
		g_mmsptrW += len;
	}
	(void) VK_SEM_Release (s_ulBufSema);

#if 0
	{
		HUINT8 *p=MMS_BUF_START;
		PrintDebug ("\n");
		for(;p<MMS_BUF_END;p++)
		{
			PrintDebug ("0x%x ",p[0]);
			if(!(p-MMS_BUF_START)%32) PrintDebug ("\n");
		}
		PrintDebug ("\n");
	}
#endif

	PrintExit;
	return len;
}

static int P_STREAM_MMS_ClearBuffer (HINT64 off_start)
{
	int nRet = 0;
	PrintEnter;

	(void) VK_SEM_Get (s_ulBufSema);
	if (NULL != g_MmsBuffer)
	{
		//VK_memset (g_HttpBuffer, 0, sizeof(g_HTTP_BUFFERING_SIZE));
		g_mmsptrW = MMS_BUF_START;
		g_mmsptrR = MMS_BUF_START;
	}
	else
	{
		nRet = -1;
	}

	MMS_BUF_START = (HINT32)g_MmsBuffer;
	MMS_BUF_END = (HINT32) g_MmsBuffer + g_MMS_BUFFERING_SIZE - 1 ;

	PrintExit;
	(void) VK_SEM_Release (s_ulBufSema);
	return nRet;
}

/** Buffering Function */
void STREAM_MMS_PreBuffering (DI_IPTUNER_Index_e nTunerIndex)
{
	HUINT8 pBuffer [MMS_READ_BLOCK_SIZE];
	int nRead=0;
	HINT32 szBuf=0;
	STREAM_ACCESS_t * pMmsAccess = stlastpMmsAccess;

	PrintEnter;

	while (1)
	{
		VK_memset  (pBuffer,0,MMS_READ_BLOCK_SIZE);

		if(pMmsAccess->b_alive == FALSE)
		{
			PrintError ("%s(%d) Closed STREAM TUNER . Finish  Buffering\n",__FUNCTION__,__LINE__);
			//	(void) VK_SEM_Release (s_ulBufSema);
			return;
		}

		if ( s_isSeeking == TRUE || s_isTricking == TRUE )
		{
			if ( s_preBufferTaskWakeUp != TRUE )
			{
				usleep(10);
				continue;
			}
		}

		nRead = STREAM_MMS_Read ( pMmsAccess, pBuffer, MMS_READ_BLOCK_SIZE );
		if(nRead<=0)
		{
			if(pMmsAccess->info.b_eof == TRUE)
			{
				PrintError ("%s(%d) Finish Buffering (End of Stream) (%d)\n",__FUNCTION__,__LINE__, nRead);
				break;
			}
			else
			{
				PrintError ("%s(%d) Read <=0 (%d)\n",__FUNCTION__,__LINE__,nRead);
			}
		}
		else
		{
			(void) P_STREAM_MMS_WriteBuffer (pBuffer,nRead);
		}

		if ( P_STREAM_MMS_GetBufferSize() < LOWEST_MMS_BUFFERING_THRESHOLD)
		{
#if 0
			if ( NULL != stMmsBufferingCallback.cbPauseCallback)
			{
				stMmsBufferingCallback.cbPauseCallback (nTunerIndex);
			}
			/* else { continue; } */
#endif

			//(void) VK_SEM_Get (s_ulBufSema);
			PrintError ("%s(%d) Start Buffering (buffered size(%d))\n",__FUNCTION__,__LINE__,P_STREAM_MMS_GetBufferSize());
			while ( (szBuf = P_STREAM_MMS_GetBufferSize()) < HIGHEST_MMS_BUFFERING_THRESHOLD )
			{
				if(pMmsAccess->b_alive == FALSE)
				{
					PrintError ("%s(%d) Closed STREAM TUNER . Finish  Buffering\n",__FUNCTION__,__LINE__);
				//	(void) VK_SEM_Release (s_ulBufSema);
					return;
				}

				VK_memset  (pBuffer,0,MMS_READ_BLOCK_SIZE);
				nRead = STREAM_MMS_Read ( pMmsAccess, pBuffer, MMS_READ_BLOCK_SIZE);
				if(nRead<=0)
				{
					if(pMmsAccess->info.b_eof == TRUE)
					{
						PrintError ("%s(%d) Finish Buffering (End of Stream) (%d)\n",__FUNCTION__,__LINE__,nRead);
					}
					break;
				}
				else
				{
					P_STREAM_MMS_WriteBuffer (pBuffer,nRead);

					if(pMmsAccess->info.b_eof == TRUE)
					{
						PrintError ("%s(%d) Finish Buffering (End of Stream) (%d)\n",__FUNCTION__,__LINE__,nRead);
						break;
					}
				}
			}
			//(void) VK_SEM_Release (s_ulBufSema);

#if 0
			if (NULL != stMmsBufferingCallback.cbPlayCallback)
			{
				if(pMmsAccess->info.b_eof == TRUE)
				{
					PrintError ("%s (%d) End of Stream ( Call Play Callback )\n",__FUNCTION__,__LINE__);
				}
				/* else { continue; } */
				stMmsBufferingCallback.cbPlayCallback (nTunerIndex);
			}
#else
			if (NULL != stMmsBufferingCallback.cbBufferCallback && P_STREAM_MMS_GetBufferSize() >= HIGHEST_MMS_BUFFERING_THRESHOLD)
			{
				if(pMmsAccess->info.b_eof == TRUE)
				{
					PrintError ("%s (%d) End of Stream ( Call Play Callback )\n",__FUNCTION__,__LINE__);
				}
				/* else { continue; } */

				if ( s_isSeeking == TRUE )
				{
					stMmsBufferingCallback.cbBufferCallback (nTunerIndex, PLAYER_BUFFERING_CBTYPE_SEEK);
					s_isSeeking = FALSE;
					s_preBufferTaskWakeUp = FALSE;
				}
				else if ( s_isTricking == TRUE )
				{
					stMmsBufferingCallback.cbBufferCallback (nTunerIndex, PLAYER_BUFFERING_CBTYPE_TRICK);
					s_isTricking = FALSE;
					s_preBufferTaskWakeUp = FALSE;
				}
				else if ( s_isPlaying == FALSE )
				{
					stMmsBufferingCallback.cbBufferCallback (nTunerIndex, PLAYER_BUFFERING_CBTYPE_PLAY);
					s_preBufferTaskWakeUp = FALSE;
					s_isPlaying = TRUE;
				}
				else
				{
				}
			}
#endif
			/* else { continue; } */
		}
		/* else { continue; } */
	} /* end while */

	PrintExit;
	return;
}

ssize_t P_STREAM_MMS_ReadBuffer (STREAM_ACCESS_t *pAccess, HUINT8 *pBuffer, size_t i_len)
{
	int size1=0;
	int size2=0;

	PrintEnter;

	if( (P_STREAM_MMS_GetBufferSize()) < (int)i_len )
	{
		if(pAccess->info.b_eof == TRUE)
		{
			i_len = P_STREAM_MMS_GetBufferSize();
		}
		else
		{
			i_len = P_STREAM_MMS_GetBufferSize();
		}
	}

	(void) VK_SEM_Get (s_ulBufSema);
	if ( (int)(g_mmsptrR+i_len) > (int)MMS_BUF_END)
	{
		size1 = (int)MMS_BUF_END - (int)g_mmsptrR;
		VK_memcpy (pBuffer, g_mmsptrR, size1);
		VK_memset  (g_mmsptrR,0,size1);
		size2 = (i_len-size1);

		VK_memcpy ((pBuffer+size1), (const void*)MMS_BUF_START, size2);
		VK_memset  ((void*)MMS_BUF_START,0,size2);

		g_mmsptrR = (HUINT8 *)(MMS_BUF_START+size2);
	}
	else
	{
		VK_memcpy (pBuffer, g_mmsptrR, i_len);
		VK_memset  (g_mmsptrR,0,i_len);

		g_mmsptrR += i_len;
	}

#if 0
	{
		int i=0;
		for(i=1;i<(i_len+1);i++)
		{
			PrintDebug ("0x%x ",pBuffer[i-1]);
			if(!(i%32)) PrintDebug ("\n");
		}
	}
#endif
	(void) VK_SEM_Release (s_ulBufSema);
	PrintExit;

	usleep(1000);

	return i_len;
}

DI_ERR_CODE DI_STREAM_MMS_CreateBuffer (DI_MEDIA_INFO_t *pstMediaInfo)
{
	DI_ERR_CODE nRet= DI_ERR_OK;
	HUINT32 nBufSize = 0;
	STREAM_ACCESS_t * pMmsAccess = stlastpMmsAccess;
	HUINT32 nTotalSize = pMmsAccess->info.i_size;
	HUINT32 nDuration = pstMediaInfo->duration;

	PrintEnter;

#if 0
	nBufSize = 50*1024*1024;
#else
#if 0
	nBufSize = STREAM_BUFFERING_SECS * (HUINT32)(nTotalSize/(nDuration/1000));
	if ( nBufSize > 10*1024*1024 )
	{
		nBufSize = 10*1024*1024;
	}
#else
	nBufSize = 10*1024*1024;
#endif

	s_isPlaying	= FALSE;
#endif

#if 0
	/* 1.5Mbyte Under .... */
	if(nBufSize < (HUINT32)(1.5*1024*1024))
	{
		PrintError ("%s (%d) Error> Buffering Size is Too Small!!! ========> (%d)bytes \n",__FUNCTION__,__LINE__, nBufSize);
		return DI_ERR_ERROR;
	}
	/* 20 Mbyte Under .... */
	else if (nBufSize > (HUINT32)20*1024*1024)
	{
		PrintError ("%s (%d) Error> Buffering Size is Too BIG!!! ==========> (%d)bytes \n",__FUNCTION__,__LINE__, nBufSize);
		return DI_ERR_ERROR;
	}
#endif

	g_MmsBuffer =(HUINT8 *)DD_MEM_Alloc ((size_t)nBufSize);
	if(g_MmsBuffer == NULL)
	{
		PrintError ("%s (%d) Error> BUFFER Malloc Fail. Not enough Memory !!!(%d)\n",__FUNCTION__,__LINE__, nBufSize);
		return DI_ERR_ERROR;
	}
	/* else { continue; } */

	/** Assign Pointer & Thresholds value*/
	g_MMS_BUFFERING_SIZE = nBufSize;
	g_mmsptrW = g_MmsBuffer;
	g_mmsptrR = g_MmsBuffer;
	MMS_BUF_START = (HINT32)g_MmsBuffer;
	MMS_BUF_END = (HINT32) g_MmsBuffer + g_MMS_BUFFERING_SIZE - 1 ;
	LOWEST_MMS_BUFFERING_THRESHOLD = g_MMS_BUFFERING_SIZE * 10 / 100;
//	HIGHEST_MMS_BUFFERING_THRESHOLD = g_MMS_BUFFERING_SIZE * 90 / 100;
	HIGHEST_MMS_BUFFERING_THRESHOLD = g_MMS_BUFFERING_SIZE * 40 / 100;

#if 0
	PrintData ("--------------------------------------------------\n");
	PrintData ("STREAM_BUFFERING_SECS            %d secs\n",STREAM_BUFFERING_SECS);
	PrintData ("STREAMING_BUFFERING              %d bytes\n",g_MMS_BUFFERING_SIZE);
	PrintData ("g_mmsptrW                        %p\n",g_mmsptrW);
	PrintData ("g_mmsptrW                        %p\n",g_mmsptrR);
	PrintData ("MMS_BUF_START                    0x%x\n",MMS_BUF_START);
	PrintData ("MMS_BUF_END                      0x%x\n",MMS_BUF_END);
	PrintData ("LOWEST_MMS_BUFFERING_THRESHOLD    %d bytes\n",LOWEST_MMS_BUFFERING_THRESHOLD);
	PrintData ("HIGHEST_MMS_BUFFERING_THRESHOLD   %d bytes\n",HIGHEST_MMS_BUFFERING_THRESHOLD);
	PrintData ("--------------------------------------------------\n");
#else
	PrintError ("--------------------------------------------------\n");
	PrintError ("STREAM_BUFFERING_SECS            %d secs\n",STREAM_BUFFERING_SECS);
	PrintError ("STREAMING_BUFFERING              %d bytes\n",g_MMS_BUFFERING_SIZE);
	PrintError ("g_mmsptrW                        %p\n",g_mmsptrW);
	PrintError ("g_mmsptrR                        %p\n",g_mmsptrR);
	PrintError ("MMS_BUF_START                    0x%x\n",MMS_BUF_START);
	PrintError ("MMS_BUF_END                      0x%x\n",MMS_BUF_END);
	PrintError ("LOWEST_MMS_BUFFERING_THRESHOLD    %d bytes\n",LOWEST_MMS_BUFFERING_THRESHOLD);
	PrintError ("HIGHEST_MMS_BUFFERING_THRESHOLD   %d bytes\n",HIGHEST_MMS_BUFFERING_THRESHOLD);
	PrintError ("--------------------------------------------------\n");
#endif


	PrintExit;


	return nRet;
}

DI_ERR_CODE DI_STREAM_MMS_FreeBuffer (void)
{
	DI_ERR_CODE nRet= DI_ERR_OK;

	if(g_MmsBuffer != NULL)
	{
		P_STREAM_MMS_ClearBuffer(0);
		DD_MEM_Free (g_MmsBuffer);
		g_MmsBuffer = NULL;
	}

	return nRet;
}
#endif /* STREAM_BUFFERING_MMS */

int DI_STREAM_MMS_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize)
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

#if STREAM_BUFFERING_MMS
	/* if DNP Tuner */
	if(tHandle.eCBtype == VFIO_CALLBACK_TYPE_DNP)
	{
		*ptReadSize = STREAM_MMS_Read ( pMmsAccess, ptr, size*nmemb );
	}
	else
	{
		if ( s_isProbeComplete == FALSE )
		{
			*ptReadSize = STREAM_MMS_Read ( pMmsAccess, ptr, size*nmemb );
		}
		else
		{
			*ptReadSize = P_STREAM_MMS_ReadBuffer ( pMmsAccess, ptr, size * nmemb );
		}
//		*ptReadSize = P_STREAM_MMS_ReadBuffer ( pMmsAccess, ptr, size * nmemb );
	}
#else
#if SUPPORT_MMS_SEM
	if (s_ulMmsSemId != 0)
	{
		(void)VK_SEM_Get(s_ulMmsSemId);
	}
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

#ifdef FILE_TEST
	if(MainHanlde == tHandle.tFile.callback.unHandle)
	{
		write(s_TempFd, ptr, nReadSizet);
	}
#endif

#if SUPPORT_MMS_SEM
	if (s_ulMmsSemId != 0)
	{
		(void)VK_SEM_Release (s_ulMmsSemId);
	}
#endif
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
		PrintError("pMmsAccess is NULL\n");
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
#if STREAM_BUFFERING_MMS
	DI_IPTUNER_Msg_t s_Msg;
#endif
	MMSTU_ACCESS_SYS_t *pstMmsSys=NULL;
	MMSH_ACCESS_SYS_t *pstMmsHSys=NULL;
	STREAM_ACCESS_t * pMmsAccess = NULL;

	UNUSED_PARAM (flags);
	PrintEnter;

#if STREAM_BUFFERING_MMS
s_isProbeComplete = FALSE;
#endif

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

	pMmsAccess->psz_path = VK_StrDup ((void*)path);

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
	if (s_ulMmsSemId != 0)
	{
		(void)VK_SEM_Get(s_ulMmsSemId);
	}
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
		if (s_ulMmsSemId != 0)
		{
			(void)VK_SEM_Release (s_ulMmsSemId);
		}
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
#ifdef FILE_TEST
			MainHanlde = ptHandle->tFile.callback.unHandle;
			s_TempFd = open ( "/mnt/hd2/Test.wmv" , O_RDWR|O_CREAT, 0666);
#endif
		}

		PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);

#if STREAM_BUFFERING_MMS
		if(g_MmsBuffer != NULL)
		{
			PrintData ("%s (%d) Buffering Start from OpenNexusCallback\n",__FUNCTION__,__LINE__);
			(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));
			s_Msg.unMsg = TUNER_STREAM_MSG_MMS_BUFFERING;
			s_Msg.unParam1 = DI_IPTUNER_STREAM_MMS;
			(void) VK_MSG_Send ( s_ulStreamMsgQId, (HUINT32 *)&s_Msg, sizeof(DI_IPTUNER_Msg_t) ) ;

			s_isProbeComplete = TRUE;
		}
#endif
	}

#if SUPPORT_MMS_SEM
	if (s_ulMmsSemId != 0)
	{
		(void)VK_SEM_Release (s_ulMmsSemId);
	}
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
		PrintError("pMmsAccess is NULL\n");
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

#if STREAM_BUFFERING_MMS
	if ( cmd == CMD_CONTROL_SEEK )
	{
		stlastpMmsAccess->info.i_pos = 0;
		s_isSeeking = TRUE;
	}
	else if ( cmd == CMD_CONTROL_SPEED )
	{
		stlastpMmsAccess->info.i_pos = 0;
		s_isTricking = TRUE;
	}
#endif

	nRet = STREAM_MMS_Control (stlastpMmsAccess, cmd , val, val2);
	if(nRet != DI_ERR_OK)
	{
		PrintError("%s (%d) Error> STREAM_MMS_Control Fail \n",__FUNCTION__,__LINE__);
	}
#if STREAM_BUFFERING_MMS
	else
	{
		if ( cmd == CMD_CONTROL_SEEK )
		{
			stlastpMmsAccess->info.i_pos = 0;
			P_STREAM_MMS_ClearBuffer(0);
		}
		else if ( cmd == CMD_CONTROL_SPEED )
		{
			stlastpMmsAccess->info.i_pos = 0;
			P_STREAM_MMS_ClearBuffer(0);
		}
		s_preBufferTaskWakeUp = TRUE;
	}
#endif

	PrintExit;

	return nRet;
}

#if STREAM_BUFFERING_MMS
DI_ERR_CODE DI_STREAM_MMS_RegisterBufferingCallback (DI_IPTUNER_PlayCallback_t *pCallback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;
	if(pCallback!=NULL)
	{
		VK_memcpy (&stMmsBufferingCallback, pCallback, sizeof(DI_IPTUNER_PlayCallback_t));
	}
	else
	{
		PrintError("%s (%d) Error> pCallback pointer not allocated. value is NULL\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
	}

	PrintExit;
	return nRet;
}

void DI_STREAM_MMS_UnregisterBufferingCallback (void)
{
	PrintEnter;

	VK_memset  (&stMmsBufferingCallback, 0x0, sizeof(DI_IPTUNER_PlayCallback_t));

	PrintExit;

	return;
}
#endif
