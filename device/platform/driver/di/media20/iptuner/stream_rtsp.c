/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   stream_rtsp.c  $
 * Version:		2.0 (for Media 2.0 with 40nm)
 * Original Author:	Jin Lee
 * Current Author:	$Author: jlee@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     RTSP - STREAM Controller
 * Module:
 * Remarks:
 */

/**
 * @defgroup RTSP_STREAM_CONTROLLER
 * @brief RTSP STREAM Controller.
 */

/**
 * @author Jin Lee (jlee@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @brief RTSP STREAM Controller.
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
 * @file stream_rtsp.c
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
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include "htype.h"
#include "di_uart.h"
#include "taskdef.h"
#include "stream_global.h"
#include "di_iptuner_config.h"
#include "di_err.h"
#include "di_media.h"
#include "di_iptuner.h"
#include "di_iptuner_common.h"
#include "di_iptuner_mms.h"
#include "di_iptuner_rtsp.h"
#include "mms_asf.h"
#include "util_url.h"
#include "stream_rtsp.h"
#include "rtsp_lib.h"
#include "rtsp_ms.h"
#include "rtsp_session.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define STREAM_RTSP_DEBUG
//#define STREAM_RTSP_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef STREAM_RTSP_DEBUG
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
#endif /* The END of DEBUG DEFINE */


#ifdef STREAM_RTSP_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((HINT32 (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)

#define STREAM_RECORD_FOR_DEBUG 0
//#define USE_TRANSPORT_UDP

#define RTSP_STACK_SIZE						(32*1024)
#define RTSP_TASK_START_WATING_TIME				(300)

#define RTSP_PROTOCOL_TYPE_PARM					("SET_PARAMETER")
#define RTSP_PROTOCOL_TYPE_OPTN					("OPTIONS")
#define RTSP_PROTOCOL_TYPE_ANNC					("ANNOUNCE")
#define RTSP_HEADER_SEPARATOR_LEN				(4)
#define RTSP_HEADER_LEN						(4)
#define RTSP_RTP_RECV_TIMEOUT               (3000000) /* (usec, 1,000,000) */

#define _UDP_RMEM_MAX                  (1048560) /* 1,048,560 or 204,800(1024*200) */


#if STREAM_RECORD_FOR_DEBUG
static int g_reenter_counter = 0;
static int g_fd = 0;
#endif


#ifdef __RTSP_THIS_IS_UNUSED_VARS__
const char default_file_name_for_empty[] = "debugfile";
#endif
const char s_szRtspProtocolVersion[] = "RTSP/1.0";

/* For probing RTSP server type */
static const char s_szRtspRealClientID[] = "ClientID: Linux_2.6_10.0.0.0_play32_RN01_EN_586";
static const char s_szRtspRealUserAgent[] = "User-Agent: RealMedia Player Version 10.0.0.0 (linux-2.6-libc6-gcc4.1-i586)";
static const char s_szRtspRealCompanyID[] = "CompanyID: X160hPo6JmsD6Vger24oAg==";
static const char s_szRtspRealClientChallenge[] = "ClientChallenge: cd7cb1ac431e8d1ad4d2fadb8cf762d1";
static const char s_szRtspRealPlayerStarttime[] = "PlayerStarttime: [19/03/2007:21:42:56 00:00]";
static const char s_szRtspRealTransport[] = "Transport: x-pn-tng/tcp;mode=play,rtp/avp/tcp;unicast;mode=play";
static const char s_szWMSUserAgent[] = "User-Agent: WMPlayer/9.0.0.2833 guid/B64345F5-8C45-4818-8A1A-4775F0923FAC"; /* from WMS-RTSP Spec example */
static const char s_szRTSPUserAgent[] =  "User-Agent: RTSP-Client/9.0.0.2833 guid/B64345F5-8C45-4818-8A1A-4775F0923FAC";

static struct rtsp_ctrl_t *P_STREAM_RTSP_NewRtspCtrl(void);
static void P_STREAM_RTSP_FreeRtspCtrl(struct rtsp_ctrl_t *rtsp_ctrl);
void STREAM_RTSP_SetMessageHeaderMethod(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *method);
static HINT32 P_STREAM_RTSP_ReadMessageHeader(struct RTSP_ACCESS_SYS_s *stream, struct RTSP_PROT_HEADER_s *rtsp_hdr);
static HINT32 P_STREAM_RTSP_ParseMessageBasicHeader(struct rtsp_ctrl_t *rtsp_ctrl, struct RTSP_PROT_HEADER_s *rtsp_header);
static HINT32 P_STREAM_RTSP_DeserializeMessageResponse(struct RTSP_PROT_HEADER_s *rtsp_hdr);
static void P_STREAM_RTSP_FreeRtspOption(struct download_opts_t *dlopts);

/* keep alive 와 seek command는 send header 에서 thread safe. */
static HINT32 P_STREAM_RTSP_SendKeepAliveCommand(struct RTSP_ACCESS_SYS_s *stream, HBOOL isWMS, HBOOL isInterleaved)
{
	HINT32	nret = -1;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	struct RTSP_CTRL_s *stream_ctrl = NULL;
	HUINT32 i_currentCSeq = 0;
	HINT32 i_statusCode = 0;
	char* read_buffer = NULL; /* for processing interleaved packet if there is remained data */

	PrintEnter;

	UNUSED_PARAM(isInterleaved);

	if ((stream == NULL) || (stream->stream_ctrl == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		return (-1);
	}
	stream_ctrl = stream->stream_ctrl;
	if (stream_ctrl->rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp_ctrl\n", __FUNCTION__, __LINE__);
		return (-1);
	}

	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(stream_ctrl->rtsp_ctrl);
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get rtsp_hdr\n", __FUNCTION__, __LINE__);
		return (-1);
	}

	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_GET_PARAMETER);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,stream_ctrl->rtsp_ctrl->p_mrl);
	if (isWMS == TRUE)
	{
		STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,s_szWMSUserAgent);
	}
	else
	{
		STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,s_szRTSPUserAgent);
	}
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);

	nret = STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);
	if ( nret < 0 )
	{
		PrintError("%s(%d) Error> RTSP requesting failed(%d)\n", __FUNCTION__, __LINE__, nret);
		return nret;
	}
	i_currentCSeq = stream_ctrl->rtsp_ctrl->i_cseq-1;

	if (0) /* Don't care because standard-RTSP don't use interleaved transfer */
	{
		if (stream->b_continuous_send == TRUE) /* need sock read lock (rival with MMSR_Read) */
		{
			VK_SEM_Get(stream->lock_read);
			while(TRUE)
			{
				/* stream buffer will be flushed after feed asf header first */
				if (isInterleaved == TRUE)
				{
					if (isWMS == TRUE)
					{
						i_statusCode = RTSP_MS_ProcessInterleavedPacket(stream);
					}
					/*
					else
					{
						i_statusCode = RTSP_SESSION_ProcessInterleavedPacket(stream);
					}
					*/
				}

				if (stream_ctrl->rtsp_ctrl->i_lastAnswerCSeq >= i_currentCSeq) /* wait last sent cmd's response seq */
				{
					PrintError("%s(%d) Error> break[answ_CSeq:%d, curr_CSeq:%d]\n", __FUNCTION__, __LINE__, stream_ctrl->rtsp_ctrl->i_lastAnswerCSeq, i_currentCSeq);
					break;
				}

				if ((isInterleaved == TRUE) && (stream->bInterleavedDataLefted == TRUE)) /* read and trash garbage packets */
				{
					read_buffer = DD_MEM_Alloc(BUFSIZE_1K * 500);
					if (read_buffer == NULL)
					{
						PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
						VK_SEM_Release(stream->lock_read);
						return (-1);
					}
					PrintError("%s(%d) Error> Unexpected [read and trash buffered packets]\n", __FUNCTION__, __LINE__);

					if (isWMS == TRUE)
					{
						RTSP_MS_ProcessRtpPacket(stream, (HUINT8 *)read_buffer , BUFSIZE_1K * 500);
					}

					if (read_buffer != NULL)
					{
						DD_MEM_Free(read_buffer);
						read_buffer = NULL;
					}
				}
			}
			VK_SEM_Release(stream->lock_read);
			stream->b_continuous_send = FALSE;
		}
		else
		{
			stream->b_continuous_send = TRUE; /* Enabled for next-time sending */
		}
	}

	PrintExit;
	return 0;
}

static HINT32 P_STREAM_RTSP_SendSeekWithTrickCommand(struct RTSP_ACCESS_SYS_s *stream, HUINT64 npt, HINT32 nScale)
{
	HBOOL isWMS = FALSE;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	struct RTSP_CTRL_s *stream_ctrl = NULL;
	char* buffer = NULL;
	HUINT32 i_currentCSeq = 0;
	HINT32 i_statusCode = 0;
	HINT32 nret = DI_IPT_SUCCESS;

	PrintEnter;

	if (stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid RTSP handle!", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	stream_ctrl = stream->stream_ctrl;
	if (stream_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid RTSP controller!", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	if (stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_WMS)
	{
		isWMS = TRUE;
	}
	else
	{
		isWMS = FALSE;
	}

	if (stream->bDuringSeeking == TRUE || stream->b_feed_asfheader_first == TRUE)
	{
		PrintError("%s(%d) Error> Re-Entering Send Seek Command(seeking:%d, first:%d)\n",
				__FUNCTION__, __LINE__, stream->bDuringSeeking, stream->b_feed_asfheader_first);
		return DI_IPT_EGENERIC;
	}

	/* stop MMSTU_Read */
	stream->bDuringSeeking = TRUE;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	if (buffer == NULL)
	{
		PrintError("%s(%d) Error> Can't Alloc Memmory!", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	VK_memset(buffer ,0, BUFSIZE_1K);

	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(stream_ctrl->rtsp_ctrl);
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get new basic message\n",__FUNCTION__, __LINE__);
		if (buffer != NULL)
		{
			DD_MEM_Free(buffer);
			buffer = NULL;
		}
		goto failed;
	}
	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_PAUSE);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,stream_ctrl->rtsp_ctrl->p_mrl);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);
	STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);

	/* STREAM_RTSP_ReadRtspResponseAndFree(stream); */
	/* response RTSP OK... is deffered read by mmsr_read */

	if (stream->dlopts->p_range)
	{
		DD_MEM_Free(stream->dlopts->p_range);
		stream->dlopts->p_range = NULL;
	}

	VK_snprintf(buffer, BUFSIZE_1K, "%.3f-", (double)npt);
	PrintDebug("%s(%d) Seek cmd NPT string = [%s]\n", __FUNCTION__, __LINE__, buffer);
	stream->dlopts->p_range = VK_StrDup(buffer);
	if (stream->dlopts->p_range == NULL)
	{
		PrintError("%s(%d) Error> Can't allocate range\n", __FUNCTION__, __LINE__);
		if (buffer != NULL)
		{
			DD_MEM_Free(buffer);
			buffer = NULL;
		}
		goto failed;
	}

	if (buffer != NULL)
	{
		DD_MEM_Free(buffer);
		buffer = NULL;
	}

	/* if WMS -> WMS IMPL */
	i_currentCSeq = stream_ctrl->rtsp_ctrl->i_cseq;
	if (isWMS == TRUE)
	{
		RTSP_MS_RequestAsyncPlay(stream, (float)nScale);
	}
	else
	{
		RTSP_SESSION_RequestAsyncPlay(stream, (float)nScale);
	}

	PrintDebug("Current CSEQ = %u\n", i_currentCSeq);

	/* STREAM_RTSP_ReadRtspResponseAndFree(stream); */
	/* response RTSP OK... is deffered read by mmsr_read */

	VK_SEM_Get(stream->lock_read);
	{
		char* read_buffer;
		read_buffer = DD_MEM_Alloc(BUFSIZE_1K * 500);
		while(TRUE)
		{
			/* if (WMS && use asf), stream buffer will be flushed after feed asf header first */
			if (isWMS == TRUE)
			{
				i_statusCode = RTSP_MS_ProcessInterleavedPacket(stream);
			}
			/* Dont care because current standard-RTSP don't use interleaved transfer */
			/*
			else
			{
				i_statusCode = RTSP_SESSION_ProcessInterleavedChannel(stream);
			}
			*/

			if (stream_ctrl->rtsp_ctrl->i_lastAnswerCSeq >= i_currentCSeq) /* wait last sent cmd's response seq */
			{
				break;
			}

			if (stream->bInterleavedDataLefted == TRUE) /* read and trash garbage packets */
			{
				PrintDebug("[read and trash buffered packets]\n");
				if (isWMS == TRUE)
				{
					RTSP_MS_ProcessRtpPacket(stream, (HUINT8 *)read_buffer , BUFSIZE_1K * 500);
				}
				/* Dont care because current standard-RTSP don't use interleaved transfer */
				/*
				else
				{
					RTSP_SESSION_ProcessRtpPacket(stream, (HUINT8 *)read_buffer , BUFSIZE_1K * 500);
				}
				*/
			}
		}
		DD_MEM_Free(read_buffer);
	}
	VK_SEM_Release(stream->lock_read);

	if (i_statusCode != 200) /* Expect RTSP 200 OK */
	{
		PrintDebug("STATUS CODE %d\n",i_statusCode);
		PrintDebug("Denied Server Side Seek or Trick\n");
		nret = DI_IPT_EGENERIC;
		goto failed;
	}

/* success: */
	/* this must moved to seekfornexus 0 */
	stream->b_feed_asfheader_first = TRUE;
	PrintExit;
	return nret;

failed:
	if (buffer != NULL) /* CID #46214 */
	{
		DD_MEM_Free(buffer);
		buffer = NULL;
	}
	PrintExit;
	return nret;
}

static void P_STREAM_RTSP_WMSKeepAliveTask ( void *pParam )
{
	HBOOL isWMS = FALSE;
	HBOOL isInterleaved = FALSE;
	struct rtsp_keepalive_thread_t *p_thread = NULL;
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;

	PrintEnter;

	if (pParam == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		return;
	}

//	lock( p_thread );

	p_thread = (struct rtsp_keepalive_thread_t *) pParam;
	p_rtsp_stream = p_thread->p_stream;
	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL RTSP handle\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if ((p_rtsp_stream->stream_ctrl == NULL) || (p_rtsp_stream->stream_ctrl->rtsp_ctrl == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL RTSP sub handle\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (p_rtsp_stream->ctrlChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL ctrl channel\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	switch(p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type) {
	case RTSP_SERVER_WMS :
		isWMS = TRUE;
		break;
	case RTSP_SERVER_Standard :
		isWMS = FALSE;
		break;
	case RTSP_SERVER_Real :
	case RTSP_SERVER_Unknown :
	default :
		/* Don't return failure and go ahead */
		PrintError("%s(%d) Error> Unexpected rtsp protocol type(%d)\n", __FUNCTION__, __LINE__, p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type);
		isWMS = FALSE;
		break;
	}

	isInterleaved = TRUE;
	if (p_rtsp_stream->bUseDataChannel == TRUE)
	{
		isInterleaved = FALSE;
	}

	while (true)
	{
		if (p_rtsp_stream->ctrlChannel == NULL)
		{
			PrintError("%s(%d) Error> Invalid NULL ctrl channel\n", __FUNCTION__, __LINE__);
			goto func_error;
		}

		if (p_rtsp_stream->ctrlChannel->b_started == FALSE)
		{
			VK_TASK_Sleep (1000);
			PrintDebug("%s(%d) Error(info)> Keep Alive Task is idle(connected:%d, started:%d)\n",
						__FUNCTION__, __LINE__,
						p_rtsp_stream->ctrlChannel->b_connected,
						p_rtsp_stream->ctrlChannel->b_started);
			continue;
		}
		(void)VK_TASK_Sleep (p_rtsp_stream->stream_ctrl->rtsp_ctrl->i_keepaliveInterval * 667); /* interval is lesser than SETUPed interval's 2/3 */
		//(void)VK_TASK_Sleep (20000); /* for test */
		PrintDebug("%s(%d) Send keep alive CMD (WMS:%d, interleaved:%d) \n", __FUNCTION__, __LINE__, isWMS, isInterleaved);
		(void)P_STREAM_RTSP_SendKeepAliveCommand(p_rtsp_stream, isWMS, isInterleaved);
	}

func_error:
//	unlock ( p_thread );
	PrintExit;
	return ;
}


static void P_STREAM_RTSP_CommonKeepAliveTask ( void *pParam )
{
	HBOOL isWMS = FALSE;
	HBOOL isInterleaved = FALSE;
	struct rtsp_keepalive_thread_t *p_thread = NULL;
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;

	PrintEnter;

	if (pParam == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		return;
	}

//	lock( p_thread );

	p_thread = (struct rtsp_keepalive_thread_t *) pParam;
	p_rtsp_stream = p_thread->p_stream;
	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL RTSP handle\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (p_rtsp_stream->ctrlChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL ctrl channel\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	switch(p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type) {
	case RTSP_SERVER_WMS :
		isWMS = TRUE;
		break;
	case RTSP_SERVER_Standard :
		isWMS = FALSE;
		break;
	case RTSP_SERVER_Real :
	case RTSP_SERVER_Unknown :
	default :
		/* Don't return failure and go ahead */
		PrintError("%s(%d) Error> Unexpected rtsp protocol type(%d)\n", __FUNCTION__, __LINE__, p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type);
		isWMS = FALSE;
		break;
	}

	isInterleaved = TRUE;
	if (p_rtsp_stream->bUseDataChannel == TRUE)
	{
		isInterleaved = FALSE;
	}
	PrintDebug("%s(%d) Error(info)> [WMS:%d, interleave:%d, protocol:%d]\n",
				__FUNCTION__, __LINE__,
				isWMS,
				isInterleaved,
				p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type);

	while (true)
	{
		if (p_rtsp_stream->ctrlChannel == NULL)
		{
			PrintError("%s(%d) Error> Invalid NULL ctrl channel\n", __FUNCTION__, __LINE__);
			goto func_error;
		}

		if (p_rtsp_stream->ctrlChannel->b_started == FALSE)
		{
			VK_TASK_Sleep (1000);
			PrintDebug("%s(%d) Error(info)> Keep Alive Task is idle(connected:%d, started:%d, session_status:%d)\n",
						__FUNCTION__, __LINE__,
						p_rtsp_stream->ctrlChannel->b_connected,
						p_rtsp_stream->ctrlChannel->b_started,
						p_rtsp_stream->stream_ctrl->session_status);
			continue;
		}
		(void)VK_TASK_Sleep (p_rtsp_stream->stream_ctrl->rtsp_ctrl->i_keepaliveInterval * 667); /* interval is lesser than SETUPed interval's 2/3 */
		PrintDebug("%s(%d) Send keep alive CMD (WMS:%d, interleaved:%d) \n", __FUNCTION__, __LINE__, isWMS, isInterleaved);
		(void)P_STREAM_RTSP_SendKeepAliveCommand(p_rtsp_stream, isWMS, isInterleaved);
	}

func_error:
//	unlock ( p_thread );
	PrintExit;
	return ;
}


/*
 * read rtsp stream. filles buffer, and buffer size is 'size'
 *
 *		read cached data from stream->ctrlChannel->buffer
 *		check for to see if network access is necessary
 *		get chunk(media packet) from network.
 *
 *  return value: bytes written to buffer. -1 if error.
 *										  0 if buffering
 */
static HINT32 P_STREAM_RTSP_ReadFromStreamBuffer(struct RTSP_ACCESS_SYS_s *stream,
			HUINT8 *buffer, size_t buffer_size)
{
	struct RTSP_CTRL_s *stream_ctrl = NULL;

	//PrintEnter;

	/* data channel 을 고른다. */
	switch(stream->dlopts->data_transport_protocol ) {
	case RTSP_TRANSPORT_TCP:
		stream_ctrl = stream->stream_ctrl;
		break;
	case RTSP_TRANSPORT_UDP_UNICAST:
		stream_ctrl = stream->stream_ctrl;
		break;
	case RTSP_TRANSPORT_RTP_UNICAST:
		stream_ctrl = stream->stream_ctrl;
		break;
	case RTSP_TRANSPORT_UDP_MULTICAST:
	case RTSP_TRANSPORT_RTP_MULTICAST:
	default:
		PrintError("%s(%d) Error> un supported data_transport_protocol\n",__FUNCTION__, __LINE__);
		return (-1);
	}

	if (buffer_size == 0)
	{
		PrintError("%s(%d) Error> buffer_size must be bigger than 0\n",__FUNCTION__, __LINE__);
		return (-1);
	}

	if (stream_ctrl->write_data_len)/* write_buffer에 대기중인 data 존재 */
	{

		if (buffer_size <= stream_ctrl->write_data_len) /* buffer size is large enough */
		{
			VK_memcpy(buffer,stream_ctrl->write_buffer + stream_ctrl->write_pos, buffer_size);

			stream_ctrl->write_data_len -= buffer_size;
			stream_ctrl->write_pos += buffer_size;

			if ((stream_ctrl->last_packet_status == LAST_PACKET_STATUS_ARRIVED
					|| stream_ctrl->session_status == RTSP_SESSION_STATUS_FINISHED)
				&& stream_ctrl->write_data_len == 0)
			{
					stream_ctrl->last_packet_status = LAST_PACKET_STATUS_READ;
			}
			return buffer_size;
		}
		else  /* copy buffer size and left others  */
		{
			size_t pos = 0; // how many bytes are in 'buffer'

			VK_memcpy(buffer,stream_ctrl->write_buffer + stream_ctrl->write_pos,
			   stream_ctrl->write_data_len);
			pos = stream_ctrl->write_data_len;
			/* and stream_ctrl->write_buffer is empty. */
			stream_ctrl->write_data_len = 0;
			stream_ctrl->write_pos = 0;

			/* 마지막 조각패킷 이후에도 더 읽는 것을 막는다.*/
			if ((stream_ctrl->last_packet_status == LAST_PACKET_STATUS_ARRIVED || stream_ctrl->session_status == RTSP_SESSION_STATUS_FINISHED) && stream_ctrl->write_data_len == 0)
			{
				stream_ctrl->last_packet_status = LAST_PACKET_STATUS_READ;
			}
			/* partial 패킷 이후 buffer에 추가 패킷을 넣는 것을 막는다. */
			return pos;
		}
		PrintError("%s(%d) Error> you should not see this line\n",__FUNCTION__, __LINE__);
	}

	/* reset buffer for future */
	stream_ctrl->write_data_len = 0;
	stream_ctrl->write_pos = 0;
	return 0;
}


/*
 * find Content-Length field in rtsp_hdr, and read (and trash) following data
 * return value:   bytes received and discarded
 */
HINT32 STREAM_RTSP_ReadMessageAndFreeBody(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	char *field = NULL;
	HINT32 body_left = 0;
	HUINT8 *ignorebuf = NULL;

	PrintEnter;

	if ((field = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Content-Length")) != NULL)
	{
		while(*field == ' ') field++;
		body_left = VK_atoi(field);
		if (body_left)
		{
			ignorebuf = DD_MEM_Alloc(body_left);
			if (ignorebuf == NULL)
			{
				PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
				goto func_error;
			}

			VK_memset(ignorebuf ,0, body_left);
			RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,ignorebuf,body_left);
			DD_MEM_Free(ignorebuf);
			ignorebuf = NULL;
		}
	}

	PrintExit;
	return body_left;

func_error:
	PrintExit;
	return (-1);
}


#if 0
/*
 * receive rtsp header but ignore its message, etc..
 * this function is purely for making code more readable.
 *
 *	   return value:  what STREAM_RTSP_ReadMessageAndParseBasic returned
 */
HINT32 STREAM_RTSP_ReadRtspResponseAndFree(struct RTSP_ACCESS_SYS_s *stream)
{
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	HINT32 nret = 0;

	PrintEnter;

	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get new message header\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	nret = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);
	if (nret < 0)
	{
		PrintError("%s(%d) Error> can't read message\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	/* if content-length field, get all following data */
	STREAM_RTSP_ReadMessageAndFreeBody(stream,rtsp_hdr);

	STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	PrintExit;
	return nret;

func_error:
	PrintError("%s(%d) Error> function failed\n", __FUNCTION__, __LINE__);
	if (rtsp_hdr != NULL)
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	}
	PrintExit;
	return (-1);
}
#endif


static void P_strreverse(char* begin, char* end)  /* jhchoi2 : fixed maxdome rtsp issue */
{
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}

static void P_uitoa32(unsigned int value, char* str, int base) /* jhchoi2 : fixed maxdome rtsp issue */
{
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
#if 0 /* warning: unused variable */
	unsigned int sign;
#endif
	int res;

	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }

	// Take care of sign
	//	if ((sign=value) < 0) value = -value;
	// Conversion. Number is reversed.
	do {
		res = value % base;
		value /= base;
		*wstr++ = num[res];
	}while(value!=0);

	//	if (sign<0) *wstr++='-';
	*wstr='\0';

	// Reverse string
	P_strreverse(str,wstr-1);
}

/*
 * set fields which are common in all rtsp packet.
 * CSeq and Session field should be present in
 * every rtsp packet.
 */
HINT32 P_STREAM_RTSP_SetMessageHeaderWithBasicFields(struct rtsp_ctrl_t *rtsp_ctrl,
				 struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	char buf[20];
	char *buf2=NULL;
	char len=0;
	char tmp_buf[20] = {0,};

	//PrintEnter;

	VK_memset(buf,0,sizeof(buf));

	/* jhchoi2 : fixed maxdome rtsp issue */
	{
		VK_memset(tmp_buf, 0, sizeof(tmp_buf));
		(void)P_uitoa32(rtsp_ctrl->i_cseq, tmp_buf, 10); //use local static function
		len = VK_strlen(tmp_buf);
	}

	(void)VK_snprintf(buf,len+7,"Cseq: %u",rtsp_ctrl->i_cseq);

	(void)STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,buf);

	if (rtsp_ctrl->p_sessionNum)
	{
		buf2 = (char *)DD_MEM_Alloc(VK_strlen(rtsp_ctrl->p_sessionNum) + 15);
		VK_memset(buf2 ,0, VK_strlen(rtsp_ctrl->p_sessionNum) + 15);
		len = VK_strlen(rtsp_ctrl->p_sessionNum);
		(void)VK_snprintf(buf2,len+10,"Session: %s",rtsp_ctrl->p_sessionNum);
		STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,buf2);
		DD_MEM_Free(buf2);
		buf2 = NULL;
	}

	//PrintExit;
	return 0;
}


/*
 * alloc new RTSP_PROT_HEADER_s and set standard fields for send.
 * this function is purely for making code more readable.
 *
 * return value:  status code: success  -1: failure
 */
struct RTSP_PROT_HEADER_s *STREAM_RTSP_NewMessageHeaderAndFillBasicFields(struct rtsp_ctrl_t *rtsp_ctrl)
{
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;

	PrintEnter;

	if (rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get RTSP header\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	P_STREAM_RTSP_SetMessageHeaderWithBasicFields(rtsp_ctrl,rtsp_hdr);

	PrintExit;
	return rtsp_hdr;

func_error:
	PrintExit;
	return NULL;
}


/*
 * send rtsp header (rtsp_hdr->buffer)
 */
static HINT32 P_STREAM_RTSP_RequestMessage(struct RTSP_ACCESS_SYS_s *stream, struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	HINT32 nret = 0;

	//PrintEnter;

	if ((stream == NULL) || (rtsp_hdr == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		return (-1);
	}

	if ((stream->ctrlChannel == NULL) || (stream->ctrlChannel->i_sock < 0))
	{
		PrintError("%s(%d) Error> Invalid control channel status\n", __FUNCTION__, __LINE__);
		return (-1);
	}

	/* dbg */
	PrintDebug("SEND RTSP REQUEST ------------\n"
			"%s\n--(%d bytes)---------------\n",
			rtsp_hdr->buffer,(int)rtsp_hdr->buffer_len);

	VK_SEM_Get(stream->lock_write);
	nret = fcntl(stream->ctrlChannel->i_sock, F_SETFL, fcntl(stream->ctrlChannel->i_sock,F_GETFL) & ~O_NONBLOCK);
	if(nret < 0)
	{
		PrintError("%s(%d) Error> can't disable non-blocking flag to socket (%d, err:%d)\n", __FUNCTION__, __LINE__, nret, errno);
	}
	nret = RTSP_LIB_SockWrite(stream->ctrlChannel->i_sock,rtsp_hdr->buffer,rtsp_hdr->buffer_len);
	if (nret < 1)
	{
		PrintError("%s(%d) Error> Network Write Error (%d, err:%d)\n", __FUNCTION__, __LINE__, nret, errno);
	}
	nret = fcntl(stream->ctrlChannel->i_sock,F_SETFL,fcntl(stream->ctrlChannel->i_sock,F_GETFL) | O_NONBLOCK);
	if(nret < 0)
	{
		PrintError("%s(%d) Error> can't enable non-blocking flag to socket (%d, err:%d)\n", __FUNCTION__, __LINE__, nret, errno);
	}
	VK_SEM_Release(stream->lock_write);


	/* OK change stream->stream_ctrl->rtsp_ctrl->i_cseq!! */
	stream->stream_ctrl->rtsp_ctrl->i_cseq++;
	//PrintExit;
	return nret;
}


/*
 * send rtsp_request and free it.
 * this function is purely for making code more readable.
 *
 * return value:  what rtsp_send request returned. (status code: success  -1: failure)
 */
HINT32 STREAM_RTSP_RequestMessageAndFree(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	HINT32 nret = 0;

	//PrintEnter;

	if ((stream == NULL) || (rtsp_hdr == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		return (-1);
	}

	nret = P_STREAM_RTSP_RequestMessage(stream,rtsp_hdr);
	STREAM_RTSP_FreeMessageHeader(rtsp_hdr);

	//PrintExit;
	return nret;
}

/*
 * set Range: field to rtsp_hdr.
 */
void STREAM_RTSP_SetRangeField(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *rangestr)
{
	char *buffer = NULL;

	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	if (buffer == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		PrintExit;
		return;
	}
	VK_memset(buffer ,0, BUFSIZE_1K);

	/* range specified */
	if (rangestr)
	{
		/* Reference 3.6 NPT (http://www.ietf.org/rfc/rfc2326.txt) */
		VK_snprintf(buffer,BUFSIZE_1K - 1,"Range: npt=%s",rangestr);
	}
	else
	{
		/* default range, from beginning */
		VK_strncpy(buffer,"Range: npt=0.000-",BUFSIZE_1K - 1);
	}
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,buffer);

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
}


/*
 * set Speed: field to rtsp_hdr.
 */
void STREAM_RTSP_SetSpeedField(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *speedstr)
{
	char *buffer = NULL;
	/*char *reason = NULL;*/
	/*HINT32  guessed_speed = 0;*/

	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	if (buffer == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		PrintExit;
		return;
	}
	VK_memset(buffer ,0, BUFSIZE_1K);

	/* speed specified */
	if (speedstr)
	{
		VK_snprintf(buffer,BUFSIZE_1K - 1,"Speed: %s",speedstr);
	}
	else
	{
		/* set default speed*/
		VK_strncpy(buffer,"Speed: 1.000",BUFSIZE_1K - 1);
	}
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,buffer);

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return;
}


/*
 * receive rtsp header and set rtsp_ctrl
 *			  return status code : success
 *							   -1 : failure
 */
HINT32 STREAM_RTSP_ReadMessageAndParseBasic(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	HINT32 nret = 0;

	PrintEnter;

	nret = P_STREAM_RTSP_ReadMessageHeader(stream,rtsp_hdr);
	if (nret < 0)
	{
		PrintError("%s(%d) Error> RTSP header read failed!\n", __FUNCTION__, __LINE__);
		PrintExit;
		return (-1);
	}

	nret = P_STREAM_RTSP_ParseMessageBasicHeader(stream->stream_ctrl->rtsp_ctrl,rtsp_hdr);
	if (nret < 0)
	{
		PrintError("%s(%d) Error> RTSP header parsing failed!\n", __FUNCTION__, __LINE__);
		PrintExit;
		return (-1);
	}

	PrintExit;
	return (HINT32)rtsp_hdr->i_statusCode; /* CID #46166: Unchecked return value */
}


/*
 * judges if rtsp_hdr is complete RTSP header, or still needs some parts.
 *	return value :	   0 ... NOT complete
 *						 1 ... COMPLETE!!
 */
static HINT32 P_STREAM_RTSP_IsEndOfRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	PrintEnter;
	if (rtsp_hdr == NULL) return 0; /* unlikely, but error */

	if (rtsp_hdr->buffer == NULL) return 0;   /* nothing received. */

	if (VK_strstr(rtsp_hdr->buffer,"\r\n\r\n") ||
	   VK_strstr(rtsp_hdr->buffer,"\n\n"))
	   {
	   PrintExit;
	return 1;
	}

  	PrintExit;
	return 0;
}

/*
 * receive rtsp message.
 */
static HINT32 P_STREAM_RTSP_ReadMessageHeader(struct RTSP_ACCESS_SYS_s *stream, struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	HINT32 nret = 0;
	HINT32 nread = 0;
	HINT32 total = 0;

  	PrintEnter;

	rtsp_hdr->buffer_len = 0;
	rtsp_hdr->buffer = NULL;
	total = 0;

	do { /* get rtsp reply */
		rtsp_hdr->buffer_len += BUFSIZE_1K;
		rtsp_hdr->buffer = (char *)RTSP_LIB_Realloc(rtsp_hdr->buffer, rtsp_hdr->buffer_len + 1);
		if (rtsp_hdr->buffer == NULL)
		{
			PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
			return (-1);
		}

		nread = RTSP_LIB_ReadFromCtrlChannelWithNoWait(stream,rtsp_hdr->buffer + total,BUFSIZE_1K);
		if (nread <= 0)
		{
			PrintError("%s(%d) Error> read error:%d\n", __FUNCTION__, __LINE__, nread);
			goto failed;
		}
		total += nread;
		rtsp_hdr->buffer[total] = '\0'; /* EOS is for P_STREAM_RTSP_IsEndOfRtspHeader */
	} while(!P_STREAM_RTSP_IsEndOfRtspHeader(rtsp_hdr));

	rtsp_hdr->buffer_len = total;
	rtsp_hdr->buffer[total] = '\0';

	nret = P_STREAM_RTSP_DeserializeMessageResponse(rtsp_hdr);
	if (nret < 0)
	{
		PrintError("%s(%d) Error> response RTSP header parse failed\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	if (rtsp_hdr->body_len)
	{
		RTSP_LIB_PutMessageForLaterRead(stream,rtsp_hdr->body,rtsp_hdr->body_len);
	}

	/* case of "rtsp_hdr->buffer" only hvae contains header */
	VK_memset(rtsp_hdr->body,0,rtsp_hdr->body_len);

	/* dbg */
	PrintDebug("%s\n", rtsp_hdr->buffer);
	PrintDebug("--------------------------------------------------------------------\n");

   	/* success */
   	PrintExit;
	return rtsp_hdr->i_statusCode;

failed:
	if (rtsp_hdr->buffer)
	{
		DD_MEM_Free(rtsp_hdr->buffer);
		rtsp_hdr->buffer = NULL;
	}
	PrintExit;
	return (-1);
}


/*
 * make mrl from url and set to rtsp_ctrl->p_mrl
 * mrl is "rtsp://server.com:port/path/to/file.foo"
 * --IMPORTANT--
 * request URI must be complete according to RFC
 */
static HINT32 P_STREAM_RTSP_NewRefinedUrl(struct rtsp_ctrl_t *rtsp_ctrl,char *host,HINT32 port,char *filepath)
{
	HINT32 len = 0;

	PrintEnter;

	if ((rtsp_ctrl == NULL) || (host == NULL) || (port == 0) || (filepath == NULL))
	{
		PrintError("%s(%d) Error> Invalid args!\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	len = VK_strlen(host) + VK_strlen(filepath) + 32; /* 32 is enough */

	if (rtsp_ctrl->p_mrl != NULL)
	{
		PrintError("%s(%d) Error> Invalid non-NULL MRL\n", __FUNCTION__, __LINE__);
		/*goto failed;*/
	}
	rtsp_ctrl->p_mrl = (char *)DD_MEM_Alloc(len);
	if (rtsp_ctrl->p_mrl == NULL)
	{
		PrintError("%s(%d) Error> Out of memroy\n", __FUNCTION__, __LINE__);
		goto failed;
	}
	VK_memset(rtsp_ctrl->p_mrl, 0, len);
	VK_snprintf(rtsp_ctrl->p_mrl,len,"rtsp://%s:%i%s", host, port, filepath);
	PrintDebug("%s(%d) [MRL:%s]\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_mrl);
	PrintExit;
	return 1;

failed:
	PrintExit;
	return (-1);
}


/*
 * set basic stuff and send OPTIONS request.
 * this can be used to determine  which protocol to use,
 * depending on its server.
 *			  return value	sock : success
 *								-1 : failure
 */
static HINT32 P_STREAM_RTSP_ConnectServer(struct RTSP_ACCESS_SYS_s *stream)
{
	HINT32 sock = -1;
	HINT32 nret = 0;
	struct RTSP_CTRL_s *stream_ctrl = NULL;
	struct rtsp_ctrl_t *rtsp_ctrl = NULL;
	URL_t *pURL = NULL;

	PrintEnter;

	if (stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	stream_ctrl = stream->stream_ctrl;
	if (stream_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL stream_ctrl\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	rtsp_ctrl = stream_ctrl->rtsp_ctrl;
	if (rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp_ctrl\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	pURL = stream->p_url;
	if (pURL == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL tURL\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	if (pURL->psz_host == NULL)
	{
		PrintError("%s(%d) Error> hostname is null\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	if (pURL->i_port == 0)
	{
		PrintError("%s(%d) Error> Invalid port\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	/* server_type_probing is already done */
	stream->p_host = VK_StrDup(pURL->psz_host);
	if (stream->p_host == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto failed;
	}
	stream->i_port = (pURL->i_port) ? pURL->i_port : _RTSP_DEFAULT_PORT;
	stream->p_connectHost = stream->p_host;
	stream->i_connectPort = stream->i_port;
	stream->p_proxyHost = NULL;
	stream->i_proxyPort = 0;

	rtsp_ctrl->p_serverName = NULL;
	rtsp_ctrl->i_cseq = 1;
	rtsp_ctrl->p_sessionNum = NULL;
	rtsp_ctrl->p_etag = NULL;
	rtsp_ctrl->p_challenge = NULL;
	rtsp_ctrl->p_transport = NULL;
	rtsp_ctrl->p_transDest = NULL;
	rtsp_ctrl->p_transSrc  = NULL;
	rtsp_ctrl->i_rtp_client_port  = 0; /* Transport field */
	rtsp_ctrl->i_rtp_server_port  = 0; /* Transport field */
	rtsp_ctrl->i_rtcp_client_port = 0; /* Transport field */
	rtsp_ctrl->i_rtcp_server_port = 0; /* Transport field */
	rtsp_ctrl->p_rtpinfo = NULL;
	rtsp_ctrl->p_rtpinfo_url = NULL;
#ifdef __RTSP_THIS_IS_UNUSED_VARS__
	rtsp_ctrl->server_state = 0;
	rtsp_ctrl->server_caps  = 0;
	rtsp_ctrl->get_media_packet = NULL;
#endif

	/* make mrl */
	nret = P_STREAM_RTSP_NewRefinedUrl(rtsp_ctrl, stream->p_host, stream->i_port, pURL->psz_path);
	if (nret < 0)
	{
		PrintError("%s(%d) Error> Can't make MRL\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	sock = RTSP_LIB_ConnectTCPChannel(stream->p_connectHost, stream->i_connectPort);
	if (sock < 0)  /* couldn't connect for some reason. (port closed maybe) */
	{
		PrintError("%s(%d) Error> Cannot connect to %s(%d)\n", __FUNCTION__, __LINE__, stream->p_connectHost, stream->i_connectPort);
		goto failed;
	}
	stream->ctrlChannel->i_sock = sock; /* save socket id */
	PrintExit;
	return sock;

failed:
	PrintExit;
	return (-1);
}


/*
 * parse rtsp response
 * return value:   1 ... success
 *				-1 ... parse error
 */
static HINT32 P_STREAM_RTSP_ParseMessageBasicHeader(struct rtsp_ctrl_t *rtsp_ctrl, struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	char *field = NULL;
	HINT32 i_cseq;
	HUINT32 i_keepaliveInterval = 10; /* MS-RTSP SPEC : min keepalive interval */
	char *ptr = NULL;

	PrintEnter;

	if ((rtsp_ctrl == NULL) || (rtsp_hdr == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		PrintExit;
		return (-1);
	}

	if ((field = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Alert")) != NULL)
	{
		PrintError("%s(%d) Error> *** Alert ***\n%s\n", __FUNCTION__, __LINE__, field);
		;
	}

	if ((field = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Cseq")) != NULL)
	{
		/* Prevent CID #12178 */
		/* VK_sscanf(field,"%d",&i_cseq); */
		i_cseq = strtol(field, (char **)NULL, 10);
		if (i_cseq != (HINT32)rtsp_ctrl->i_cseq - 1)
		{
			/* -1 because cseq is incremented in build_request */
			PrintError("%s(%d) Error(warning) Cseq mismatch, got %u, but expected %u\n",
					__FUNCTION__, __LINE__, i_cseq, rtsp_ctrl->i_cseq - 1);
		}
		rtsp_ctrl->i_lastAnswerCSeq = i_cseq;
	}

	if ((field = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Server")) != NULL)
	{
		while(*field == ' ') {
			field++;
		}

		if (rtsp_ctrl->p_serverName) { /* already has it */
			if (VK_strcmp(rtsp_ctrl->p_serverName,field)) { /* different server */
				PrintError("%s(%d) Error(warning) got different server\n"
						"	old: %s\n"
						"	new: %s\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_serverName,field);
				DD_MEM_Free(rtsp_ctrl->p_serverName);
				rtsp_ctrl->p_serverName = NULL;
				rtsp_ctrl->p_serverName = VK_StrDup(field);
				if (rtsp_ctrl->p_serverName == NULL)
				{
					PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
					goto func_error;
				}
			}
		}
		else { /* not set yet */
			rtsp_ctrl->p_serverName = VK_StrDup(field);
			if (rtsp_ctrl->p_serverName == NULL)
			{
				PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
				goto func_error;
			}
		}
	}

	if ((field = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Session")) != NULL)
	{
		while(*field == ' ') field++;
		if (rtsp_ctrl->p_sessionNum)
		{
			if (VK_strncmp(rtsp_ctrl->p_sessionNum,field,VK_strlen(rtsp_ctrl->p_sessionNum))) { /* different session */
				PrintError("%s(%d) Error(warning) got different session\n"
						"	old: [%s]\n"
						"	new: [%s]\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_sessionNum,field);
				DD_MEM_Free(rtsp_ctrl->p_sessionNum);
				rtsp_ctrl->p_sessionNum = NULL;
				rtsp_ctrl->p_sessionNum = VK_StrDup(field);
			}
		}
		else
		{
			char *sub_ptr = VK_strstr(field,";timeout=");
			if (sub_ptr)
			{
				rtsp_ctrl->p_sessionNum = DD_MEM_Alloc(sub_ptr - field + 1);
				if (rtsp_ctrl->p_sessionNum == NULL)
				{
					PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
					goto func_error;
				}
				VK_memset(rtsp_ctrl->p_sessionNum ,0, sub_ptr - field + 1);
				VK_strncpy(rtsp_ctrl->p_sessionNum, field, sub_ptr - field);

				/* CID #46223 */
				/* VK_sscanf(VK_strstr(sub_ptr, "=") + 1, "%u", &i_keepaliveInterval); */
				ptr = VK_strstr(sub_ptr, "=");
				if (ptr)
				{
					i_keepaliveInterval = strtol(ptr+1, (char **)NULL, 10);
				}
				/* if else routine, the keepalive interval has default value */

				PrintDebug("%s(%d) parsed keep alive time = %u\n", __FUNCTION__, __LINE__, i_keepaliveInterval);
			}
			else
			{
				rtsp_ctrl->p_sessionNum = VK_StrDup(field);
				if (rtsp_ctrl->p_sessionNum == NULL)
				{
					PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
					goto func_error;
				}
			}

			rtsp_ctrl->i_keepaliveInterval = i_keepaliveInterval;
		}
	}

	/* real only, ignore in other protocol... */
	if ((field = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"RealChallenge1")) != NULL)
	{
		while(*field == ' ') field++;
		if (rtsp_ctrl->p_challenge)
		{
			if (VK_strcmp(rtsp_ctrl->p_challenge,field)) { /* different server */
				PrintError("%s(%d) Error(warning) got different client challenge\n"
						"	old: %s\n"
						"	new: %s\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_challenge,field);

				DD_MEM_Free(rtsp_ctrl->p_challenge);
				rtsp_ctrl->p_challenge = NULL;
				rtsp_ctrl->p_challenge = VK_StrDup(field);
				if (rtsp_ctrl->p_challenge == NULL)
				{
					PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
					goto func_error;
				}
			}
		}
		else
		{
			rtsp_ctrl->p_challenge = VK_StrDup(field);
			if (rtsp_ctrl->p_challenge == NULL)
			{
				PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
				goto func_error;
			}
		}
	}

	PrintExit;
	return 1;

func_error:
	PrintExit;
	return (-1);
}


/*
 * parsing rtsp response header
 */
static HINT32 P_STREAM_RTSP_DeserializeMessageResponse(struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	char *field 		= NULL;
	char *hdr_ptr		= NULL;
	char *str_ptr		= NULL;
	HINT32 nPosHeaderSep, nHdrSepLen;
	size_t length;

	PrintEnter;

	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL arg!\n", __FUNCTION__, __LINE__);
		return (-1);
	}
	if (rtsp_hdr->is_parsed)
	{
		PrintError("%s(%d) Error> Already pared response\n", __FUNCTION__, __LINE__);
		return 0;
	}

	hdr_ptr = VK_strstr(rtsp_hdr->buffer, " ");
	if (hdr_ptr == NULL)
	{
		PrintError("%s(%d) Error> Malformed p_answer : No %20 separator\n", __FUNCTION__, __LINE__);
		return (-1);
	}

	length = hdr_ptr - rtsp_hdr->buffer;
	rtsp_hdr->p_protocol = DD_MEM_Alloc(length + 1);
	if (rtsp_hdr->p_protocol == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(rtsp_hdr->p_protocol ,0, length + 1);
	VK_strncpy(rtsp_hdr->p_protocol,rtsp_hdr->buffer,length);
	rtsp_hdr->p_protocol[length] = '\0';

	rtsp_hdr->p_method = VK_StrDup(rtsp_hdr->p_protocol);
	if (rtsp_hdr->p_method == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (!VK_strncasecmp(rtsp_hdr->p_protocol,"RTSP",4))
	{
		/* wait RTSP status code */
		/* Prevent CID #12177 */
		/* if(VK_sscanf(++hdr_ptr,"%d",&(rtsp_hdr->status_code)) != 1) */
		rtsp_hdr->i_statusCode = strtol(++hdr_ptr, (char **)NULL, 10);

		hdr_ptr += RTSP_HEADER_LEN;

		str_ptr = VK_strstr(hdr_ptr,"\n"); /* Getting reason string */
		if (str_ptr == NULL)
		{
			PrintError("%s(%d) Error> Wrong answer : unable to get reason_phrase\n", __FUNCTION__, __LINE__);
			return (-1);
		}

		length = str_ptr - hdr_ptr;  /* Getting reason string */
		rtsp_hdr->p_reasonPhrase = DD_MEM_Alloc(length + 1);
		if (rtsp_hdr->p_reasonPhrase == NULL)
		{
			PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
			return (-1);
		}
		VK_memset(rtsp_hdr->p_reasonPhrase ,0, length + 1);
		VK_strncpy(rtsp_hdr->p_reasonPhrase,hdr_ptr,length);

		/* M$ */
		if (rtsp_hdr->p_reasonPhrase[length - 1] == '\r')
		{
			/* move \r */
			length --;
		}

		rtsp_hdr->p_reasonPhrase[length] = '\0';  /* terminate */
	}
	else if (!VK_strncasecmp(rtsp_hdr->p_protocol, RTSP_PROTOCOL_TYPE_OPTN, VK_strlen(RTSP_PROTOCOL_TYPE_OPTN)))
	{
		rtsp_hdr->i_statusCode = RTSP_STATUS_OPTIONS;
	}
	else if (!VK_strncasecmp(rtsp_hdr->p_protocol, RTSP_PROTOCOL_TYPE_ANNC, VK_strlen(RTSP_PROTOCOL_TYPE_ANNC)))
	{
		rtsp_hdr->i_statusCode = RTSP_STATUS_ANNOUNCE;
	}
	else if (!VK_strncasecmp(rtsp_hdr->p_protocol, RTSP_PROTOCOL_TYPE_PARM, VK_strlen(RTSP_PROTOCOL_TYPE_PARM)))
	{
		rtsp_hdr->i_statusCode = RTSP_STATUS_SET_PARAMETER;
	}
	else
	{
		PrintError("%s(%d) Error> Protocol not RTSP (%s)\n", __FUNCTION__, __LINE__, rtsp_hdr->p_protocol);
		goto func_error;
	}

	nHdrSepLen = RTSP_HEADER_SEPARATOR_LEN; /* header separator length */
	str_ptr = VK_strstr(rtsp_hdr->buffer,"\r\n\r\n");
	if (!str_ptr)
	{
		str_ptr = VK_strstr(rtsp_hdr->buffer,"\n\n");
		if (!str_ptr)
		{
			PrintError("%s(%d) Error> Header Incompleted\n", __FUNCTION__, __LINE__);
			goto func_error;
		}
		nHdrSepLen = 2;
	}
	nPosHeaderSep = str_ptr - rtsp_hdr->buffer;
  	hdr_ptr = VK_strstr(rtsp_hdr->buffer,"\n") + 1; /* points to first line after method line. */

	do
	{
		str_ptr = hdr_ptr; /* get all fields. they are separated by '\n' */
		while(*str_ptr != '\r' && *str_ptr != '\n') str_ptr++;
		length  = str_ptr - hdr_ptr;

		if (length == 0)
		{
			/* met \n\n --> end of header! */
			break;
		}

		field = (char *)RTSP_LIB_Realloc(field,length + 1); // +1 for '\0'
		if (field == NULL)
		{
			PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
			goto func_error;
		}
		VK_strncpy(field,hdr_ptr,length);
		field[length] = '\0';


		STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,field);
		hdr_ptr = str_ptr + ((*str_ptr == '\r') ? 2 : 1);  /* next line */
	} while(hdr_ptr < (rtsp_hdr->buffer + nPosHeaderSep));

	if (field)
	{
		DD_MEM_Free(field);
		field = NULL;
	}

	if (nPosHeaderSep + nHdrSepLen < (HINT32)rtsp_hdr->buffer_len)
	{
		rtsp_hdr->body = rtsp_hdr->buffer + (nPosHeaderSep + nHdrSepLen); // rtsp_hdr->buffer_len is length of header
		rtsp_hdr->body_len = rtsp_hdr->buffer_len - (nPosHeaderSep + nHdrSepLen);
	}

	/* mark parsed */
	rtsp_hdr->is_parsed = 1;
	PrintExit;
	return 1;

func_error:
	PrintExit;
	return (-1);
}


/*
 * send OPTIONS request, this is used as very first trial to detect server type is MS-media server or Real(Helix)-media server
 * we will need rtsp_hdr later, so return that.
 * return value:   -1: failure   status_code: success, ppRtspHdr_out(malloc)
 */
static int P_STREAM_RTSP_DetectServerType(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s **ppRtspHdr_out) /* ____PRE_DETECT_SERVER_TYPE____ */
{
	int nret = 0;
	struct rtsp_ctrl_t *rtsp_ctrl = NULL;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *options_uri = NULL;
	int options_uri_len = 0;
	int i_loopcnt = 0;

	PrintEnter;

	*ppRtspHdr_out = NULL;

	if ((stream == NULL) || (stream->stream_ctrl == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n",__FUNCTION__, __LINE__);
		goto failed;
	}

	rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;
	if (rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp_ctrl\n",__FUNCTION__, __LINE__);
		goto failed;
	}

	/* default is rtsp-real (becasue OPTIONS req is supported) */
	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get basic message\n",__FUNCTION__, __LINE__);
		goto failed;
	}

	/*
	 * OPTIONS rtsp://192.168.0.2:8554/ RTSP/1.0
	 * Cseq: 1
	 * User-Agent: RealMedia Player Version 10.0.0.0 (linux-2.6-libc6-gcc4.1-i586)
	 * GUID: 00000000-0000-0000-0000-000000000000
	 * ClientID: Linux_2.6_10.0.0.0_play32_RN01_EN_586
	 * Pragma: initiate-session
	 * RegionData: 0
	 * ClientChallenge: cd7cb1ac431e8d1ad4d2fadb8cf762d1
	 * CompanyID: X160hPo6JmsD6Vger24oAg==
	 * PlayerStarttime: [19/03/2007:21:42:56 00:00]
	 */
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, s_szRtspRealUserAgent);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, "GUID: 00000000-0000-0000-0000-000000000000");
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, s_szRtspRealClientID);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, "Pragma: initiate-session");
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, "RegionData: 0");
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, s_szRtspRealClientChallenge);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, s_szRtspRealCompanyID);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, s_szRtspRealPlayerStarttime);

	options_uri_len = VK_strlen(stream->p_host) + 256;
	options_uri = (char *)DD_MEM_Alloc(options_uri_len);
	if (options_uri == NULL)
	{
		PrintError("%s(%d) Error> Out of memory \n",__FUNCTION__, __LINE__);
		goto failed;
	}
	VK_memset(options_uri, 0, options_uri_len);
	VK_snprintf(options_uri,options_uri_len,"rtsp://%s:%i", stream->p_host,stream->i_port);

	i_loopcnt = 0;
func_retry:
	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_OPTION);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,rtsp_ctrl->p_mrl);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);

	PrintDebug("%s(%d) --> +RequestMessageAndFree()\n",__FUNCTION__, __LINE__);
	nret = STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);
	PrintDebug("%s(%d) --> -RequestMessageAndFree()\n",__FUNCTION__, __LINE__);
	i_loopcnt++;
	if (nret < 0)
	{
		PrintError("%s(%d) Error> message request failed\n",__FUNCTION__, __LINE__);

		/* Maybe socket error...                                               */
		if (0)
		{
			/* But message_hdr is already cleared at above RequestMessageAndFree() */
			/* So, we can't use message_hdr handle anymore; or don't free it         */
			goto func_retry;
		}
		else
		{
			goto failed;
		}
	}

	/* message_hdr is already cleared at above RequestMessageAndFree() */
	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> can't get new message header\n",__FUNCTION__, __LINE__);
		goto failed;
	}
	nret = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr); // rtsp_recv_header
	if (nret < 0) {
		PrintError("%s(%d) Error> can't read message\n",__FUNCTION__, __LINE__);
		goto failed;
	}
	*ppRtspHdr_out = rtsp_hdr;

	if (options_uri)
	{
		DD_MEM_Free(options_uri);
	}
	PrintExit;
	return nret;

failed:
	if (rtsp_hdr)
	{
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	}
	if (options_uri)
	{
		DD_MEM_Free(options_uri);
	}
	*ppRtspHdr_out = NULL;
	PrintExit;
	return (-1);
} /* ____PRE_DETECT_SERVER_TYPE____ */

/*
 * start rtsp streaming.
 *
 *	return value :   negative or 0  ... error
 *								 1  ... success
 */
static HINT32 P_STREAM_RTSP_Open(struct RTSP_ACCESS_SYS_s *stream, HBOOL bForceWMS)
{
	HBOOL bDetectNeeded = TRUE;
	HBOOL bAvailableRTSP = FALSE;
	HBOOL bAlreadyConnected = FALSE;
	HBOOL isRTP = TRUE;
	HINT32 nret = 0;
	char *p_serverName = NULL;
	char *p_answer = NULL; /* temporary pointer */
	struct RTSP_PROT_HEADER_s *p_responseHdr = NULL;
	ASF_HEADER_t *pAsfHeaderInfo = NULL;

	PrintEnter;

	if (stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		PrintExit;
		return (-1);
	}

	if ((stream->stream_ctrl == NULL) || (stream->dlopts == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL internal handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return (-1);
	}

	if (stream->stream_ctrl->rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp_ctrl\n", __FUNCTION__, __LINE__);
		PrintExit;
		return (-1);
	}

	stream->stream_ctrl->session_status = RTSP_SESSION_STATUS_HANDSHAKING;
	stream->stream_ctrl->last_packet_status = LAST_PACKET_STATUS_NOTARRIVED;

	/* Skip server_type_probing if WMS */
	if ((bForceWMS == TRUE) && (stream->dlopts->expected_server_type != RTSP_SERVER_WMS))
	{
		stream->dlopts->expected_server_type = RTSP_SERVER_WMS;
	}

	switch (stream->dlopts->expected_server_type) {
	case RTSP_SERVER_WMS :
		bAvailableRTSP = TRUE;
		bDetectNeeded = FALSE;
		p_serverName = VK_StrDup("WMServer");
		if (p_serverName == NULL)
		{
			PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
			goto failed;
		}

		/* Set server and protocol type; Skip server_type_probing */
		stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type = RTSP_PROTOCOL_WMS;
		goto func_connect;
		break;
	case RTSP_SERVER_Unknown :
	case RTSP_SERVER_Standard :
		bAvailableRTSP = TRUE;
		bDetectNeeded = TRUE;
		/* server_type_probing will be done */
		break;
	case RTSP_SERVER_Real :
	default:
		PrintError("%s(%d) Error> Unsupported RTSP server\n", __FUNCTION__, __LINE__);
		bAvailableRTSP = FALSE;
		bDetectNeeded = TRUE;
		PrintExit;
		return (-1);
	}

	bAlreadyConnected = FALSE;
	if ((bDetectNeeded == TRUE) || (p_serverName == NULL))
	{
		/* server_type_probing */
		if (stream->ctrlChannel->i_sock > 0)
		{
			PrintError("%s(%d) Error> Invalid socket fd(%d)\n", __FUNCTION__, __LINE__, stream->ctrlChannel->i_sock);
			goto failed;
		}
		stream->ctrlChannel->i_sock = P_STREAM_RTSP_ConnectServer(stream);
		if (stream->ctrlChannel->i_sock < 0)
		{
			PrintError("%s(%d) Error> Can't connect to server\n", __FUNCTION__, __LINE__);
#if 0
			stream->stream_ctrl->session_status = STREAMING_OTHER_PROTOCOL;
#endif
			goto failed;
		}
		bAlreadyConnected = TRUE;

		/* detect server type */
		nret = P_STREAM_RTSP_DetectServerType(stream, &p_responseHdr); // real_rtsp_options
		if (nret < 0)
		{
			PrintError("%s(%d) Error> server detection failed\n", __FUNCTION__, __LINE__);
			goto failed;
		}

		if (p_responseHdr == NULL)
		{
			PrintError("%s(%d) Error> Can't get response header\n", __FUNCTION__, __LINE__);
			goto failed;
		}

		if ((p_answer = STREAM_RTSP_FindMessageHeaderField(p_responseHdr,"Server")) != NULL)
		{
	    	while(*p_answer == ' ')
	    	{
	    		p_answer++;  /* skip space */
	    	}
	    	p_serverName = VK_StrDup(p_answer);
	    	if (p_serverName == NULL)
			{
				PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
				goto failed;
			}
			PrintDebug("%s(%d) server:(%s)\n", __FUNCTION__, __LINE__, p_serverName);
		}
		else
		{
			if ((STREAM_RTSP_FindMessageHeaderField(p_responseHdr,"RealChallenge1")) != NULL)
			{
				p_serverName = VK_StrDup("Real");
				if (p_serverName == NULL)
				{
					PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
					goto failed;
				}
	    	}
	    	else
			{
				p_serverName = VK_StrDup("Unknown Server");
				if (p_serverName == NULL)
				{
					PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
					goto failed;
				}
			}
		}

		if (p_serverName == NULL)
		{
			PrintError("%s(%d) Error> Can't get serverName and serverType\n", __FUNCTION__, __LINE__);
			goto failed;
		}
		PrintDebug("%s(%d) serverName(%s)\n", __FUNCTION__, __LINE__, p_serverName);

		if (VK_strstr(p_serverName,"Real") || VK_strstr(p_serverName,"Helix"))
		{
			PrintError("%s(%d) Error> unsupported server type(%s)!\n", __FUNCTION__, __LINE__, p_serverName);
			goto failed;
		}
		else
		{
			/* WMS or Standard */
			if (VK_strstr(p_serverName,"WMServer"))
			{
				stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type = RTSP_PROTOCOL_WMS;
			}
			else
			{
				/* Assumed with RTSP/1.0 based server */
				stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type = RTSP_PROTOCOL_STANDARD;
			}
		}
	}
	else
	{
		/* In normal case, this routine flow can't followed here */
		/* because server_type_probing will be skipped if WMS */
		PrintError("%s(%d) Error> This is unexpected routine flow\n", __FUNCTION__, __LINE__);

		if ((stream->stream_ctrl == NULL) || (stream->dlopts == NULL))
		{
			PrintError("%s(%d) Error> Invalid NULL internal handle\n", __FUNCTION__, __LINE__);
			PrintExit;
			return (-1);
		}

		if (stream->stream_ctrl->rtsp_ctrl == NULL)
		{
			PrintError("%s(%d) Error> Can't get rtsp_ctrl\n", __FUNCTION__, __LINE__);
			PrintExit;
			return (-1);
		}

		stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type = RTSP_PROTOCOL_STANDARD;
	}

	if (p_serverName)
	{
		DD_MEM_Free(p_serverName);
		p_serverName = NULL;
	}
	if (p_responseHdr)
	{
		STREAM_RTSP_FreeMessageHeader(p_responseHdr);
	}


func_connect:
	/* Check server_type again */
	if (1)
	{
		if ((stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type != RTSP_PROTOCOL_WMS)
			&& (stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type != RTSP_PROTOCOL_STANDARD))
		{
			PrintError("%s(%d) Error> Unsupported server_type(%d)\n", __FUNCTION__, __LINE__, stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type);
			goto failed;
		}

		if (stream->stream_ctrl->rtsp_ctrl == NULL)
		{
			PrintError("%s(%d) Error> Can't get rtsp_ctrl\n", __FUNCTION__, __LINE__);
			PrintExit;
			return (-1);
		}
	}

	/* connect or re-connect with RTSP server */
	/* Come here directly if WMS server */
	if (bAlreadyConnected == FALSE)
	{
		if (stream->ctrlChannel->i_sock > 0)
		{
			PrintError("%s(%d) Error> Invalid socket fd(%d)\n", __FUNCTION__, __LINE__, stream->ctrlChannel->i_sock);
			goto failed;
		}
		stream->ctrlChannel->i_sock = P_STREAM_RTSP_ConnectServer(stream);
		if (stream->ctrlChannel->i_sock < 0)
		{
			/* couldn't connect for some reason(rtsp port probably closed) */
			PrintError("%s(%d) Error> RTSP connection not established\n", __FUNCTION__, __LINE__);
			PrintError("%s(%d) Error> server probably does not accept rtsp retry using mmst protocol\n", __FUNCTION__, __LINE__);

			stream->stream_ctrl->session_status = RTSP_SESSION_STATUS_SESSION_ERROR;
			// stream->stream_ctrl->retry_protocol = MMST;...
			goto failed;
		}
	}

	/* Describe, Parse Session Description(SDP), SETUP */
	do
	{
		if (stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_WMS)
		{
			/* Send DESCRIBE request and get/parse the SDP */
			nret = RTSP_MS_RequestDescribeAndGetHeader(stream,&pAsfHeaderInfo);
			if (nret == 0)  /* retry	*/
			{
				/* probably non-wmserver */
				PrintError("%s(%d) Error> server probably does not accept rtsp\n", __FUNCTION__, __LINE__);
				stream->stream_ctrl->session_status = RTSP_SESSION_STATUS_SESSION_ERROR;
				goto failed;
			}
			else if ((!pAsfHeaderInfo) || (nret < 0))    /* no retry */
			{
				PrintError("%s(%d) Error> WMS-RTSP DESCRIBE failed\n", __FUNCTION__, __LINE__);
				goto failed;
			}

			/* set asf info from asf_header */
			stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo = pAsfHeaderInfo;
			stream->stream_ctrl->packet_length = pAsfHeaderInfo->i_max_data_packet_size;
			stream->stream_ctrl->file_size = pAsfHeaderInfo->i_file_size;
			stream->stream_ctrl->total_packets = pAsfHeaderInfo->i_data_packets_count;
		}
		else
		{
			/* Send DESCRIBE request and get/parse the SDP */
			nret = RTSP_SESSION_RequestDescribeAndGetSDP(stream);
			if (nret == 0)  /* retry	*/
			{
				/* probably non-wmserver */
				PrintError("%s(%d) Error> server probably does not accept rtsp\n", __FUNCTION__, __LINE__);
				stream->stream_ctrl->session_status = RTSP_SESSION_STATUS_SESSION_ERROR;
				goto failed;
			}
			else if (nret < 0)    /* no retry */
			{
				PrintError("%s(%d) Error> RTSP DESCRIBE failed\n", __FUNCTION__, __LINE__);
				goto failed;
			}

			/* set asf info from asf_header */
			stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo = NULL;
			stream->stream_ctrl->packet_length = 0;
			stream->stream_ctrl->file_size = 0;
			stream->stream_ctrl->total_packets = 0;
		}
	} while(0);

	/* Send SETUP and PLAY */
	do
	{
		if (stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_WMS)
		{
			nret = RTSP_MS_RequestSetup(stream);
		}
		else
		{
			nret = RTSP_SESSION_RequestSetup(stream);
		}

		if (nret < 0)
		{
			PrintError("%s(%d) Error> RTSP SETUP,PLAY failed\n", __FUNCTION__, __LINE__);
			goto failed;
		}
	} while(0);

	/* Open out_of_band (data) channel if UDP or RTP is used here */
	if ((stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_UDP_UNICAST) ||
		(stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_RTP_UNICAST))
	{
		if (stream->dataChannel[2] == NULL)
		{
			PrintError("%s(%d) Error> Invalid NULL data channel\n", __FUNCTION__, __LINE__);
			goto failed;
		}

		if (stream->stream_ctrl->rtsp_ctrl->i_rtp_client_port == 0)
		{
			PrintError("%s(%d) Error> Invalid client_port\n", __FUNCTION__, __LINE__);
			goto failed;
		}

		isRTP = (stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_RTP_UNICAST) ? TRUE : FALSE;
		if (stream->stream_ctrl->rtsp_ctrl->p_transDest != NULL) /* destination field */
		{
			stream->dataChannel[2]->i_sock = RTSP_LIB_ConnectRTPChannel(stream->dataChannel[2],
							stream->stream_ctrl->rtsp_ctrl->p_transDest,
							stream->stream_ctrl->rtsp_ctrl->i_rtp_client_port,
							isRTP);
		}
		else
		{
			PrintError("%s(%d) Error> Invalid destination\n", __FUNCTION__, __LINE__);
			if (stream->p_connectHost == NULL)
			{
				PrintError("%s(%d) Error> Invalid connectHost\n", __FUNCTION__, __LINE__);
				goto failed;
			}
			stream->dataChannel[2]->i_sock = RTSP_LIB_ConnectRTPChannel(stream->dataChannel[2],
							stream->p_connectHost,
							stream->stream_ctrl->rtsp_ctrl->i_rtp_client_port,
							isRTP);
		}

		if (stream->dataChannel[2]->i_sock < 0)
		{
			PrintError("%s(%d) Error> Can't connect server\n", __FUNCTION__, __LINE__);
			goto failed;
		}
		stream->dataChannel[2]->b_connected = TRUE;
	}


	/* Send SETUP and PLAY */
	do
	{
		if (stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_WMS)
		{
			nret = RTSP_MS_RequestPlay(stream);
		}
		else
		{
			nret = RTSP_SESSION_RequestPlay(stream);
		}

		if (nret < 0)
		{
			PrintError("%s(%d) Error> RTSP SETUP,PLAY failed\n", __FUNCTION__, __LINE__);
			goto failed;
		}
	} while(0);

	if (stream->stream_ctrl->session_status != RTSP_SESSION_STATUS_STARTED)
	{
		stream->stream_ctrl->session_status = RTSP_SESSION_STATUS_STARTED;
	}

	PrintExit;
	return 1; /* success */

failed:
	PrintError("%s(%d) Error> Function failed\n", __FUNCTION__, __LINE__);
	if (p_serverName)
	{
		DD_MEM_Free(p_serverName);
		p_serverName = NULL;
	}

	if (p_responseHdr)
	{
		STREAM_RTSP_FreeMessageHeader(p_responseHdr);
	}

	PrintExit;
	return (-1);
}


/*
 * STREAM_RTSP_NewMessageHeader : create new RTSP header
 */
struct RTSP_PROT_HEADER_s *STREAM_RTSP_NewMessageHeader(void)
{
	struct RTSP_PROT_HEADER_s *hdr = NULL;

	//PrintEnter;

	hdr = (struct RTSP_PROT_HEADER_s *)DD_MEM_Alloc(sizeof(struct RTSP_PROT_HEADER_s));
	if (hdr == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(hdr ,0, sizeof(struct RTSP_PROT_HEADER_s));
	//PrintExit;
	return hdr;

func_error:
	PrintError("%s(%d) Error> function failed\n", __FUNCTION__, __LINE__);
	if (hdr != NULL) /* Impossible, but... */
	{
		STREAM_RTSP_FreeMessageHeader(hdr);
	}
	//PrintExit;
	return NULL;
}

void STREAM_RTSP_FreeMessageHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	struct rtsp_field_t *field = NULL;
	struct rtsp_field_t *prev = NULL;

	//PrintEnter;

	if (rtsp_hdr == NULL) return; // rtsp_hdr is not malloc()ed yet.

	if (rtsp_hdr->p_protocol)
	{
		DD_MEM_Free(rtsp_hdr->p_protocol);
		rtsp_hdr->p_protocol = NULL;
	}
	if (rtsp_hdr->p_method)
	{
		DD_MEM_Free(rtsp_hdr->p_method);
		rtsp_hdr->p_method = NULL;
	}
	if (rtsp_hdr->p_uri)
	{
		DD_MEM_Free(rtsp_hdr->p_uri);
		rtsp_hdr->p_uri = NULL;
	}
	if (rtsp_hdr->p_reasonPhrase)
	{
		DD_MEM_Free(rtsp_hdr->p_reasonPhrase);
		rtsp_hdr->p_reasonPhrase = NULL;
	}
	if (rtsp_hdr->field_search)
	{
		DD_MEM_Free(rtsp_hdr->field_search);
		rtsp_hdr->field_search = NULL;
	}
	if (rtsp_hdr->buffer)
	{
		DD_MEM_Free(rtsp_hdr->buffer);
		rtsp_hdr->buffer = NULL;
	}

	field = rtsp_hdr->first_field;
	for(field = rtsp_hdr->first_field; field ; )
	{
		if (field->field_name)
		{
			DD_MEM_Free(field->field_name);
			field->field_name = NULL;
		}

		prev = field;
		field = field->next;
		DD_MEM_Free(prev);
		prev = NULL;
	}

	DD_MEM_Free(rtsp_hdr);
	rtsp_hdr = NULL;
	//PrintExit;
}

/*
 * returns first filed with 'field name'.
 * STREAM_RTSP_GetNextField will get next field with 'filed_name'
 */
char *STREAM_RTSP_FindMessageHeaderField(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *pszQuery)
{
	char 	*str_ptr;
	struct 	rtsp_field_t *rtsp_field;

	//PrintEnter;

	if (!rtsp_hdr || !pszQuery) return NULL;

	rtsp_hdr->field_search_pos = rtsp_hdr->first_field;
	/* warning: cast discards qualifiers from pointer target type */
	/* VK_strlen() -> VK_strlen() */
	rtsp_hdr->field_search = (char *)RTSP_LIB_Realloc(rtsp_hdr->field_search, VK_strlen(pszQuery) + 1);
	if (rtsp_hdr->field_search == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	/* copy field name first. */
	VK_strncpy(rtsp_hdr->field_search, pszQuery , VK_strlen(pszQuery)); /* jhchoi2 : fixed maxdome rtsp issue */

	rtsp_field = rtsp_hdr->field_search_pos;

	while(rtsp_field)
	{
		str_ptr = VK_strstr(rtsp_field->field_name,":");
		if (str_ptr == NULL)
		{
			/* header is not valid */
			return NULL;
		}

		if (!VK_strncasecmp(rtsp_field->field_name,rtsp_hdr->field_search,
					str_ptr - rtsp_field->field_name))
		{
			str_ptr++;
			while(*str_ptr == ' ')
			{
 				str_ptr++;
			}
			rtsp_hdr->field_search_pos = rtsp_field->next;
			PrintExit;
			return str_ptr;
		}
		rtsp_field = rtsp_field->next;
	}
	//PrintExit;
	return NULL; // NOT FOUND
}


/*
 * STREAM_RTSP_SetMessageHeaderField : make new field and attach it to last_field->next.
 */
void STREAM_RTSP_SetMessageHeaderField(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *field_name)
{
	struct rtsp_field_t *new_field = NULL;

	//PrintEnter;

	if (rtsp_hdr == NULL || field_name == NULL) return;

	PrintDebug("%s(%d) [field:%s]\n", __FUNCTION__, __LINE__, field_name);

	new_field = DD_MEM_Alloc(sizeof(struct rtsp_field_t));
	if (new_field == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(new_field ,0, sizeof(struct rtsp_field_t));

	new_field->next = NULL;
	/* warning: cast discards qualifiers from pointer target type */
	/* VK_strlen() -> VK_strlen() */
	new_field->field_name = DD_MEM_Alloc (VK_strlen(field_name) + 1);
	if (new_field->field_name == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(new_field->field_name ,0, VK_strlen(field_name) + 1);
	VK_strncpy(new_field->field_name, field_name, VK_strlen(field_name)); /* jhchoi2 : fixed maxdome rtsp issue */

	if (rtsp_hdr->last_field == NULL) {
		rtsp_hdr->first_field = new_field; // this was first filed!!
	}
	else {
		rtsp_hdr->last_field->next = new_field; // attach to last.
	}
	rtsp_hdr->last_field = new_field;
	rtsp_hdr->i_field_nb++;
	//PrintExit;
	return;

func_error:
	PrintError("%s(%d) Error> Function failed\n", __FUNCTION__, __LINE__);
	if (new_field != NULL) /* CID #46216 */
	{
		DD_MEM_Free(new_field);
		new_field = NULL;
	}
	//PrintExit;
	return;
}


void STREAM_RTSP_SetMessageHeaderURI(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *uri)
{
	//PrintEnter;
	if (rtsp_hdr == NULL || uri == NULL) return;

	rtsp_hdr->p_uri = DD_MEM_Alloc(VK_strlen(uri) + 1);
	if (rtsp_hdr->p_uri == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(rtsp_hdr->p_uri ,0, VK_strlen(uri) + 1);
	VK_strncpy(rtsp_hdr->p_uri, uri,VK_strlen(uri)); /* jhchoi2 : fixed maxdome rtsp issue */
	//PrintExit;
	return;

func_error:
	//PrintExit;
	return;
}

void STREAM_RTSP_SetMessageHeaderMethod(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *method)
{
	//PrintEnter;

	if (rtsp_hdr == NULL || method == NULL) return;

	rtsp_hdr->p_method = DD_MEM_Alloc(VK_strlen(method) + 1);
	if (rtsp_hdr->p_method == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(rtsp_hdr->p_method, 0, VK_strlen(method) + 1);
	VK_strncpy(rtsp_hdr->p_method, method, VK_strlen(method)); /* jhchoi2 : fixed maxdome rtsp issue */
	//PrintExit;
	return;

func_error:
	//PrintExit;
	return;
}

/*
 * wrappers for each request
 */
HINT32 STREAM_RTSP_RequestMessageOK(struct RTSP_ACCESS_SYS_s *stream,HINT32 cseq,char *session)
{
	HINT32 nret = 0;
#if 0
	HINT32 buflen = 100 + VK_strlen(s_szRtspProtocolVersion) + VK_strlen(session);
#else
	HINT32 buflen = 100 + VK_strlen((char*)&s_szRtspProtocolVersion) + VK_strlen(session);
#endif
	char *buf;

	PrintEnter;

	buf = DD_MEM_Alloc(buflen + 1);
	if (buf == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		nret = (-1);
		goto func_error;
	}
	VK_memset(buf ,0, buflen + 1);

	VK_snprintf(buf,buflen,
		 "%s 200 OK\r\n"
		 "Cseq: %u\r\n"
		 "Session: %s\r\n"
		 "\r\n",
		 s_szRtspProtocolVersion,
		 cseq,
		 session);

	nret = RTSP_LIB_SockWrite(stream->ctrlChannel->i_sock,buf,VK_strlen(buf));
	PrintDebug("=send 200 OK============\n%s\n=================\n",buf);

func_error:
	if (buf != NULL)
	{
		DD_MEM_Free(buf);
		buf = NULL;
	}
	PrintExit;
	return nret;
}


/*
 * make complete RTSP_PROT_HEADER_s.
 * firstline is first line, such as OPTIONS, DESCRIBE, etc... (see rtsp)
 * the request string goes to rtsp_hdr->buffer
 */
char *STREAM_RTSP_SerializeMessageHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	char *ptr;
	HINT32 length,length2 = 0;
	struct rtsp_field_t *field;

	//PrintEnter;

	if (!rtsp_hdr)
	{
	 	return NULL;
	}
	if (!rtsp_hdr->p_method)
	{
		return NULL;
	}
	if (!rtsp_hdr->p_uri)
	{
		return NULL;
	}

	length = VK_strlen(rtsp_hdr->p_method) + VK_strlen(rtsp_hdr->p_uri) +
				VK_strlen((char*)&s_szRtspProtocolVersion) + 2 + 2 ;
	field = rtsp_hdr->first_field;

	while(field)
	{
		length += VK_strlen(field->field_name) + 2;
		field = field->next;
	}

	length += 2;

	if (rtsp_hdr->body)
	{
		length += rtsp_hdr->body_len;
	}

	if (rtsp_hdr->buffer)
	{
		DD_MEM_Free(rtsp_hdr->buffer);
		rtsp_hdr->buffer = NULL;
	}

	if (rtsp_hdr->body_len)
	{
		length += rtsp_hdr->body_len;
	}

	rtsp_hdr->buffer_len = length;
	rtsp_hdr->buffer = DD_MEM_Alloc(length + 1);
	if (rtsp_hdr->buffer == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(rtsp_hdr->buffer ,0, length + 1);

	ptr = rtsp_hdr->buffer;
#if 0 /* warning: cast discards qualifiers from pointer target type */
	len2 = VK_strlen(rtsp_hdr->p_method)+VK_strlen(rtsp_hdr->p_uri)+VK_strlen(s_szRtspProtocolVersion);
#else
	length2 = VK_strlen(rtsp_hdr->p_method)+VK_strlen(rtsp_hdr->p_uri)+VK_strlen((char*)&s_szRtspProtocolVersion);
#endif
	ptr += VK_snprintf(ptr,length2+5,"%s %s %s\r\n",rtsp_hdr->p_method,rtsp_hdr->p_uri, s_szRtspProtocolVersion);

	field = rtsp_hdr->first_field;

	while(field != NULL)
	{
		length2 = VK_strlen(field->field_name);
		ptr += VK_snprintf(ptr,length2+3,"%s\r\n",field->field_name);
		field = field->next;
	}
	ptr += VK_snprintf(ptr,3,"\r\n");

	if (rtsp_hdr->body != NULL) {
		VK_memcpy(ptr,rtsp_hdr->body,rtsp_hdr->body_len);
	}

	//PrintExit;
	return rtsp_hdr->buffer;

func_error:
	//PrintExit;
	return NULL;
}


static struct rtsp_ctrl_t *P_STREAM_RTSP_NewRtspCtrl(void)
{
	struct rtsp_ctrl_t *rtsp_ctrl = NULL;

	PrintEnter;

	rtsp_ctrl = (struct rtsp_ctrl_t *)DD_MEM_Alloc(sizeof(struct rtsp_ctrl_t));
	if (rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(rtsp_ctrl ,0, sizeof(struct rtsp_ctrl_t));

#ifdef __RTSP_THIS_IS_UNUSED_VARS__
	rtsp_ctrl->prev_ts = -1;
	rtsp_ctrl->prev_stream_number = -1;
#endif

	/* allocate other protocol controllers in
	 *  each protocol initiation functions,
	 *  such as rmff_header or pAsfHeaderInfo...
	 */
	rtsp_ctrl->rtsp_protocol_type = RTSP_PROTOCOL_UNKNOWN;
	PrintExit;
	return rtsp_ctrl;

func_error:
	PrintExit;
	return rtsp_ctrl;
}


static void P_STREAM_RTSP_FreeRtspCtrl(struct rtsp_ctrl_t *rtsp_ctrl)
{
	PrintEnter;

	if (rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULLL rtsp_ctrl\n", __FUNCTION__, __LINE__);
		return;
	}

	if (rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_REAL) {
		//if (rtsp_ctrl->rmff_header) {
			;
			//FUTURE REAL-RTSP
			//free_rmff_header_t(rtsp_ctrl->rmff_header);
		//}
	}
	else if (rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_WMS) {
		if (rtsp_ctrl->pAsfHeaderInfo) {
			free_pAsfHeaderInfo_t(rtsp_ctrl->pAsfHeaderInfo);
		}
	}

	if (rtsp_ctrl->p_mrl)
	{
		DD_MEM_Free(rtsp_ctrl->p_mrl);
		rtsp_ctrl->p_mrl = NULL;
	}
	if (rtsp_ctrl->p_serverName)
	{
		DD_MEM_Free(rtsp_ctrl->p_serverName);
		rtsp_ctrl->p_serverName = NULL;
	}
	if (rtsp_ctrl->p_sessionNum)
	{
		DD_MEM_Free(rtsp_ctrl->p_sessionNum);
		rtsp_ctrl->p_sessionNum = NULL;
	}
#ifdef __RTSP_THIS_IS_UNUSED_VARS__
	if (rtsp_ctrl->p_etag)
	{
		DD_MEM_Free(rtsp_ctrl->p_etag);
		rtsp_ctrl->p_etag = NULL;
	}
#endif
	if (rtsp_ctrl->p_challenge)
	{
		DD_MEM_Free(rtsp_ctrl->p_challenge);
		rtsp_ctrl->p_challenge = NULL;
	}

	/* Transport */
	if (rtsp_ctrl->p_transport)
	{
		DD_MEM_Free(rtsp_ctrl->p_transport);
		rtsp_ctrl->p_transport = NULL;
	}

	if (rtsp_ctrl->p_transDest)
	{
		DD_MEM_Free(rtsp_ctrl->p_transDest);
		rtsp_ctrl->p_transDest = NULL;
	}

	if (rtsp_ctrl->p_transSrc)
	{
		DD_MEM_Free(rtsp_ctrl->p_transSrc);
		rtsp_ctrl->p_transSrc = NULL;
	}

	/* RTP-INFO */
	if (rtsp_ctrl->p_rtpinfo)
	{
		DD_MEM_Free(rtsp_ctrl->p_rtpinfo);
		rtsp_ctrl->p_rtpinfo = NULL;
	}
	if (rtsp_ctrl->p_rtpinfo_url)
	{
		DD_MEM_Free(rtsp_ctrl->p_rtpinfo_url);
		rtsp_ctrl->p_rtpinfo_url = NULL;
	}

	DD_MEM_Free(rtsp_ctrl);
	rtsp_ctrl = NULL;
	PrintExit;
}

static struct RTSP_ACCESS_SYS_s *P_STREAM_RTSP_NewRtspAccessSysAndRtspCtrl(void)
{
	struct RTSP_ACCESS_SYS_s *stream = NULL;

	PrintEnter;

	stream = RTSP_LIB_InitRtspAccessSys();
	if (stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null RTSP handle\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	if (stream->stream_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null RTSP stream_ctrl\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	stream->stream_ctrl->rtsp_ctrl = P_STREAM_RTSP_NewRtspCtrl();
	if (stream->stream_ctrl->rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Can't get rtsp_ctrl\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	PrintExit;
	return stream;

func_error:
	PrintExit;
	return stream;
}


static void P_STREAM_RTSP_SendTeardownAndFreeRtspAccessSys(struct RTSP_ACCESS_SYS_s *stream)
{
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	HINT32 nret = 0;

	PrintEnter;

	if (stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null arg\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	if (stream->stream_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null arg\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	if (stream->stream_ctrl->rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null arg\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	/* TEARDOWN */
	if (stream->stream_ctrl->session_status == RTSP_SESSION_STATUS_STARTED ||
	   stream->stream_ctrl->session_status == RTSP_SESSION_STATUS_FINISHED)
	{
		/* send teardown request to make server happier. */
		rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(stream->stream_ctrl->rtsp_ctrl);
		if (rtsp_hdr == NULL)
		{
			PrintError("%s(%d) Error> Can't get new basic message\n",__FUNCTION__, __LINE__);
			goto failed;
		}
		STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_TEARDOWN);
		STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,stream->stream_ctrl->rtsp_ctrl->p_mrl);
		STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);
		nret = STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);
	}

	if ((stream->ctrlChannel != NULL) && (stream->ctrlChannel->i_sock > 0))
	{
		net_Close(stream->ctrlChannel->i_sock);
		stream->ctrlChannel->i_sock = (-1);
	}

	if ((stream->dataChannel[2] != NULL) && (stream->dataChannel[2]->i_sock > 0))
	{
		net_Close(stream->dataChannel[2]->i_sock);
		stream->dataChannel[2]->i_sock = (-1);
	}

	if (stream->stream_ctrl->rtsp_ctrl)
	{
		P_STREAM_RTSP_FreeRtspCtrl(stream->stream_ctrl->rtsp_ctrl);
		stream->stream_ctrl->rtsp_ctrl = NULL;
	}

	if (stream->p_url)
	{
		UTIL_UrlClean( stream->p_url );
		DD_MEM_Free(stream->p_url);
		stream->p_url = NULL;
	}

	if (stream->dlopts)
	{
		P_STREAM_RTSP_FreeRtspOption(stream->dlopts);
		stream->dlopts = NULL;
	}

	RTSP_LIB_FreeRtspAccessSys(stream);
	stream = NULL;

failed:
	PrintExit;
}

static struct download_opts_t *P_STREAM_RTSP_NewRtspOption(void)
{
	struct download_opts_t *dlopts = NULL;

	PrintEnter;

	dlopts = DD_MEM_Alloc(sizeof(struct download_opts_t));
	if (dlopts == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_memset(dlopts,0,sizeof(struct download_opts_t));

	dlopts->expected_server_type = RTSP_SERVER_Unknown;
	dlopts->i_bandwidth = 0;
	dlopts->p_speed = NULL;
	dlopts->p_range = NULL;
	dlopts->p_byterange = NULL;
	dlopts->i_oobPort = 0;
	dlopts->data_transport_protocol = RTSP_TRANSPORT_UDP_UNICAST; /* Undefined */
#ifdef __RTSP_THIS_IS_UNUSED_VARS__
	dlopts->p_username = NULL;
	dlopts->p_password = NULL;
#endif

	PrintExit;
	return dlopts;
}


static void P_STREAM_RTSP_FreeRtspOption(struct download_opts_t *dlopts)
{
	PrintEnter;
	if (!dlopts) {
		return;
	}

	if (dlopts->p_speed)
	{
		DD_MEM_Free(dlopts->p_speed);
		dlopts->p_speed = NULL;
	}
	if (dlopts->p_range)
	{
		DD_MEM_Free(dlopts->p_range);
		dlopts->p_range = NULL;
	}
	if (dlopts->p_byterange)
	{
		DD_MEM_Free(dlopts->p_byterange);
		dlopts->p_byterange = NULL;
	}
#ifdef __RTSP_THIS_IS_UNUSED_VARS__
	if (dlopts->p_username)
	{
		DD_MEM_Free(dlopts->p_username);
		dlopts->p_username = NULL;
	}
	if (dlopts->p_password)
	{
		DD_MEM_Free(dlopts->p_password);
		dlopts->p_password = NULL;
	}
#endif

	DD_MEM_Free(dlopts);
	dlopts = NULL;
	PrintExit;
}


static void P_STREAM_RTSP_Close(struct RTSP_ACCESS_SYS_s *p_rtsp_stream)
{
	HULONG lock_read;
	HULONG lock_write;
	HULONG lock_sync;

	PrintEnter;

	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> NULL arg\n", __FUNCTION__, __LINE__);
		return;
	}

	lock_read = p_rtsp_stream->lock_read;
	lock_write = p_rtsp_stream->lock_write;
	lock_sync = p_rtsp_stream->lock_sync;

	/* release resource */
	if ( p_rtsp_stream->p_keepalive_thread)
	{
		PrintDebug("==========STOP KEEPALIVE THREAD================\n");
		(void)VK_TASK_Sleep (RTSP_TASK_START_WATING_TIME); /* wait for starting the task */

		VK_TASK_Stop(p_rtsp_stream->p_keepalive_thread->thread_id);
		VK_TASK_Destroy(p_rtsp_stream->p_keepalive_thread->thread_id);
		DD_MEM_Free(p_rtsp_stream->p_keepalive_thread);
		p_rtsp_stream->p_keepalive_thread = NULL;
	}


	/* terminate and release protocol internal resource */
	if (p_rtsp_stream)
	{
		P_STREAM_RTSP_SendTeardownAndFreeRtspAccessSys(p_rtsp_stream); /* FUTURE integrate P_STREAM_RTSP_Close and rtsp_streming_close */
		p_rtsp_stream = NULL;
	}

	/* release resource */
	/* STREAM_RTSP_SendTeardownAndFreeRtspAccessSys에서도 lock을 사용 */
	VK_SEM_Destroy (lock_read);
	VK_SEM_Destroy (lock_write);
	VK_SEM_Destroy (lock_sync);

	PrintExit;
	return;
}


HINT32  MMSR_Open ( STREAM_ACCESS_t *pAccess )
{
	URL_t *ptURL = NULL;
	struct download_opts_t *dlopts = NULL;
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;
	HINT32 nret = 0;
	HINT32 nVKRet = 0; /* for virtaul kernal */
	HBOOL bForceWMS = TRUE;

	PrintEnter;

	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null access\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	/* Set up pAccess */
	pAccess->info.i_update = 0;
	pAccess->info.i_size = 0;
	pAccess->info.i_pos = 0;
	pAccess->info.b_eof = FALSE;
	pAccess->info.i_title = 0;
	pAccess->info.i_seekpoint = 0;

#if STREAM_RECORD_FOR_DEBUG
	if (pAccess->b_openfromnexus == TRUE)
	{
		g_reenter_counter = 0;

		g_fd = open ( "/backtrick.asf" , O_CREAT|O_TRUNC|O_WRONLY , 0666);
		if (g_fd == NULL)
		{
			PrintDebug("=== open failed ===\n");
		}
		else
		{
			PrintDebug("=== open success ===\n");
		}
	}
#endif

	/* parse path and make url and setup downloadopt */
	ptURL = DD_MEM_Alloc(sizeof(URL_t));
	if (ptURL == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto failed;
	}
	VK_memset(ptURL ,0, sizeof(URL_t));
	UTIL_UrlParse(ptURL, pAccess->psz_path, 0 );

	/* check if target url is valid */
	if (ptURL == NULL) {
		PrintError("%s(%d) Error> failed \"%s\"\n", __FUNCTION__, __LINE__, pAccess->psz_path);
		goto failed;
	}
	if (!ptURL->psz_host || !VK_strcmp(ptURL->psz_host,"")) {
		PrintError("%s(%d) Error> failed \"%s\" (no host information)\n", __FUNCTION__, __LINE__, pAccess->psz_path);
		goto failed;
	}
	if (!ptURL->psz_path || !VK_strcmp(ptURL->psz_path,"")) {
		PrintError("%s(%d) Error> failed: \"%s\" (no filepath)\n", __FUNCTION__, __LINE__, pAccess->psz_path);
		goto failed;
	}


	/* set download options */
	dlopts = P_STREAM_RTSP_NewRtspOption();
	if (dlopts == NULL)
	{
		PrintError("%s(%d) Error> Can't get dl option\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	/* Set it to WMS type to prevent overlapped server probing */
	dlopts->expected_server_type = RTSP_SERVER_WMS; /* Fix it with WMS type when called though MMSR_Func() */
	dlopts->p_speed = VK_StrDup("8.252"); /* for variable rate video and support trick speed */
	dlopts->i_oobPort = _RTSP_OOB_PORT_FOR_WMS;
#ifdef USE_TRANSPORT_UDP /* MMS */
	dlopts->data_transport_protocol = RTSP_TRANSPORT_UDP_UNICAST;
#else
	dlopts->data_transport_protocol = RTSP_TRANSPORT_TCP;
#endif

	/* alloc RTSP_ACCESS_SYS_s */
	p_rtsp_stream = P_STREAM_RTSP_NewRtspAccessSysAndRtspCtrl(); /*stream->close(stream);*/
	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> Can't get dl option\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	p_rtsp_stream->p_keepalive_thread = NULL;
	p_rtsp_stream->current_trick_speed = 1;

	/* setup rtsp_stream */
	p_rtsp_stream->p_url = ptURL;
	ptURL = NULL;
	p_rtsp_stream->dlopts = dlopts;
#ifdef __RTSP_THIS_IS_UNUSED_VARS__
	p_rtsp_stream->localfile = VK_StrDup((char*)&default_file_name_for_empty);
#endif

	nret = VK_SEM_Create(&p_rtsp_stream->lock_read, "rtspread", DEFAULT_SUSPEND_TYPE);
	if ( nret != VK_OK )
	{
		PrintError("%s(%d) Error> Can't create SEM(0x%x) \n\r",__FUNCTION__,__LINE__, nret);
		goto failed;
	}
	nret = VK_SEM_Create(&p_rtsp_stream->lock_write, "rtspwrite", DEFAULT_SUSPEND_TYPE);
	if ( nret != VK_OK )
	{
		PrintError("%s(%d) Error> Can't create SEM(0x%x) \n\r",__FUNCTION__,__LINE__, nret);
		goto failed;
	}
	nret = VK_SEM_Create(&p_rtsp_stream->lock_sync, "rtspsync", DEFAULT_SUSPEND_TYPE);
	if ( nret != VK_OK )
	{
		PrintError("%s(%d) Error> Can't create SEM(0x%x) \n\r",__FUNCTION__,__LINE__, nret);
		goto failed;
	}

	/* Describe, process SDP, SETUP */
	nret = P_STREAM_RTSP_Open(p_rtsp_stream, bForceWMS);
	if (nret <= 0) {
		PrintError("%s(%d) Error> cannot open RTSP connection\n",__FUNCTION__,__LINE__);
		goto failed;
	}

	/* setup pAccess */
	pAccess->p_sys = p_rtsp_stream;
	pAccess->info.i_size = p_rtsp_stream->stream_ctrl->file_size; /* from asf header */

	/* Control channel is TCP */
	p_rtsp_stream->ctrlChannel->transport_type = RTSP_TRANSPORT_TCP;

#ifdef USE_TRANSPORT_UDP /* MMS */
	p_rtsp_stream->dataChannel[2] = RTSP_LIB_NewChannel();
	p_rtsp_stream->dataChannel[2]->b_connected = FALSE; /* Trigged at Connect() */
	p_rtsp_stream->dataChannel[2]->b_started = FALSE; /* Trigged at StartDataChannelIfNotStarted() */
	p_rtsp_stream->dataChannel[2]->i_port = _RTSP_OOB_PORT_FOR_WMS;
	p_rtsp_stream->dataChannel[2]->transport_type = RTSP_TRANSPORT_RTP_UNICAST;
	p_rtsp_stream->dataChannel[2]->i_prevRtpSeq = 0;

	/* Don't connect out_of_band channel here */
	/* Connect to data channal after checking the Transport field of SETUP/PLAY response */
#endif

	if (pAccess->b_openfromnexus == TRUE)
	{
		/* Setup keep alive thread */
		p_rtsp_stream->p_keepalive_thread = (struct rtsp_keepalive_thread_t *)DD_MEM_Alloc(sizeof(struct rtsp_keepalive_thread_t));
		if (p_rtsp_stream->p_keepalive_thread == NULL)
		{
			PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
			goto failed;
		}
		VK_memset(p_rtsp_stream->p_keepalive_thread ,0, sizeof(struct rtsp_keepalive_thread_t));

		p_rtsp_stream->p_keepalive_thread->p_stream = p_rtsp_stream;
		p_rtsp_stream->p_keepalive_thread->b_paused = FALSE;
		p_rtsp_stream->p_keepalive_thread->b_thread_error = FALSE;
		p_rtsp_stream->p_keepalive_thread->b_quit = FALSE;

		nVKRet = VK_TASK_Create ( P_STREAM_RTSP_WMSKeepAliveTask
					, AUDIO_CLIP_TASK_PRIORITY
					, RTSP_STACK_SIZE
					, "P_STREAM_RTSP_WMSKeepAliveTask"
					, (void *)p_rtsp_stream->p_keepalive_thread
					, &p_rtsp_stream->p_keepalive_thread->thread_id
					, FALSE); /* TRUE means JOINABLE */

		if ( VK_OK != nVKRet )
		{
			PrintError("%s(%d) Error> KeepAliveThread task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
			p_rtsp_stream->p_keepalive_thread->b_thread_error = TRUE;
			return DI_IPT_ETHREAD;
		}
		(void)VK_TASK_Start(p_rtsp_stream->p_keepalive_thread->thread_id);
	}

	PrintDebug( "Rtsp open success!! %s (%d)   \n",__FUNCTION__,__LINE__);

	PrintExit;
	return DI_IPT_SUCCESS;


failed:
	P_STREAM_RTSP_Close(p_rtsp_stream);
	if (ptURL != NULL)
	{
		UTIL_UrlClean( ptURL );
		DD_MEM_Free(ptURL);
		ptURL = NULL;
	}

	if (dlopts != NULL) /* CID #46213 */
	{
		DD_MEM_Free(dlopts);
		dlopts = NULL;
	}

	PrintExit;
	return DI_IPT_EGENERIC;
}


ssize_t MMSR_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len )
{
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;
	size_t	  i_data = 0;
	size_t requested_size = i_len; /* 보통 192KB */
	size_t pos = 0;
	HINT32 inner_ret = 0;
	ssize_t nret = DI_IPT_SUCCESS;

	//PrintEnter;

	/* null check */
	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null arg\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	PrintDebug("current Start of Read Point is %"PRId64"\n", pAccess->info.i_pos);

	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;
	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null arg\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	/* MMSR_Func() could be assumed with WMS-RTSP based protocol */
	/* So, probing or checking for server_type is unnecessary */

	if (p_rtsp_stream->stream_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null stream_ctrl\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	if (p_rtsp_stream->stream_ctrl->rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null rtsp_ctrl\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	if (p_rtsp_stream->dlopts == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null dlopts\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	if (pAccess->b_openfromnexus == FALSE && pAccess->info.i_pos == 0)
	{
		//inner_ret = P_STREAM_RTSP_ReadFromStreamBuffer(p_rtsp_stream, p_buffer + pos, requested_size - pos);
		VK_memcpy(p_buffer, p_rtsp_stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo->pRawAsfHeader, p_rtsp_stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo->nRawAsfHeaderLen);
		pos += p_rtsp_stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo->nRawAsfHeaderLen;
		if (inner_ret < 0)
		{
			PrintDebug("P_STREAM_RTSP_ReadFromStreamBuffer failed\n");
			goto func_read_error;
		}

		pos += inner_ret;

		PrintDebug("sizeof asf hdr pos = %d\n", (HINT32)pos);
		goto success;
	}

	/* feed ASF HDR if nessary */
	if (p_rtsp_stream->b_feed_asfheader_first == TRUE && pAccess->info.i_pos == 0)
	{
		PrintDebug("FEED ASF HEADER FIRST \n");
		VK_memcpy(p_buffer, p_rtsp_stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo->pRawAsfHeader, p_rtsp_stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo->nRawAsfHeaderLen);
		pos += p_rtsp_stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo->nRawAsfHeaderLen;

		/* flush patial packet */
		p_rtsp_stream->stream_ctrl->write_data_len = 0;
		p_rtsp_stream->stream_ctrl->write_pos = 0;

		p_rtsp_stream->b_feed_asfheader_first = FALSE;
		p_rtsp_stream->bDuringSeeking = FALSE;

#if STREAM_RECORD_FOR_DEBUG
		g_reenter_counter ++;
		if (g_reenter_counter == 3)
		{
			PrintDebug ("========\n============\nnow recording\n");
		}
#endif
	}

	/* Send Play if not started */
	inner_ret = RTSP_MS_StartDataChannelIfNotStarted(p_rtsp_stream);
	if (inner_ret < 0)
	{
		PrintError("%s(%d) Error> dataChannel starting failed\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	while(requested_size > pos) /* requested_size is nexus aligned size */
	{
		size_t recved_size = 0;

		if (p_rtsp_stream->bDuringSeeking == TRUE) /* wait for thread canceled by nexus seek(0) */
		{
			nret = DI_IPT_SUCCESS;
			goto success;
		}

		VK_SEM_Get(p_rtsp_stream->lock_read);

		/* 1st, read from buffer */
		inner_ret = P_STREAM_RTSP_ReadFromStreamBuffer(p_rtsp_stream, p_buffer + pos, requested_size - pos);
		if (inner_ret < 0)
		{
			PrintError("%s(%d) Error> P_STREAM_RTSP_ReadFromStreamBuffer failed\n", __FUNCTION__, __LINE__);
			goto func_read_error;
		}

		recved_size += inner_ret;

		/* 2nd, parse CMD packet */
		if (p_rtsp_stream->stream_ctrl->session_status != RTSP_SESSION_STATUS_FINISHED &&  nret == 0)
		{
			if (p_rtsp_stream->dlopts->data_transport_protocol  ==  RTSP_TRANSPORT_TCP) /* Maybe interleaved */
			{
				/* interleaved mode에서 control channel recevier */
				inner_ret = RTSP_MS_ProcessInterleavedPacket(p_rtsp_stream);
				if (inner_ret < 0)
				{
					PrintError("%s(%d) Error> ProcessInterleavedPacket failed\n", __FUNCTION__, __LINE__);
					goto func_read_error;
				}
			}
		}

		/* 3rd, read an asf packet from socket. if the buffer size is lower than an asf packet size then leave rest part to stream buffer */
		if (p_rtsp_stream->stream_ctrl->session_status != RTSP_SESSION_STATUS_FINISHED
			&& p_rtsp_stream->bInterleavedDataLefted == TRUE
			&& recved_size == 0)
		{
			inner_ret = RTSP_MS_ProcessRtpPacket(p_rtsp_stream, p_buffer + pos, requested_size - pos);
			if (inner_ret < 0)
			{
				PrintError("%s(%d) Error> ProcessRtpPacket failed\n", __FUNCTION__, __LINE__);
				goto func_read_error;
			}
			recved_size += inner_ret;
		}


func_read_error:
		VK_SEM_Release(p_rtsp_stream->lock_read);
		pos += recved_size;

		/* check end of stream */
		if ((p_rtsp_stream->stream_ctrl->last_packet_status == LAST_PACKET_STATUS_ARRIVED ||
			p_rtsp_stream->stream_ctrl->session_status == RTSP_SESSION_STATUS_FINISHED) &&
			p_rtsp_stream->stream_ctrl->write_data_len == 0)
		{
			p_rtsp_stream->stream_ctrl->last_packet_status = LAST_PACKET_STATUS_READ;
			pAccess->info.b_eof = TRUE;
			nret = DI_IPT_SUCCESS;
			goto success;
		}
		p_rtsp_stream->b_continuous_send = FALSE;

		if (inner_ret < 0) /* read failed */
		{
			nret = DI_IPT_EGENERIC;
			goto genericfailed;
		}
	}

success:
	;

#if STREAM_RECORD_FOR_DEBUG
	if (g_fd)
	{
		if (g_reenter_counter == 3)
		{
			PrintDebug ("[x]");
			write (g_fd, p_buffer, pos);
		}
	}
#endif

genericfailed:
	i_data = pos;

	PrintDebug("%s(%d) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__,requested_size, pos);
	//PrintExit;

	if (nret == DI_IPT_SUCCESS)
	{
		return i_data;
	}

	return DI_IPT_EGENERIC;
}

HINT32 MMSR_Seek ( STREAM_ACCESS_t * pAccess, HINT64 i_pos )
{
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;

	PrintEnter;

	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null access\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;
	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL RTSP handle\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	//PrintDebug("current Seek Point is %"PRId64"\n", i_pos);

	pAccess->info.i_pos = i_pos;
	pAccess->info.b_eof = false;

	PrintExit;
	return DI_IPT_SUCCESS;
}

HINT32 MMSR_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2)
{
	static HINT32 i_trick_scale = 1;
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;
	HINT32 nret = DI_IPT_SUCCESS;

	PrintEnter;

	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null access\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;
	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL RTSP handle\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	switch (cmd) {
	case CMD_CONTROL_PAUSE :
		PrintError("%s(%d) CMD_CONTROL_PAUSE \n",__FUNCTION__,__LINE__);
		/* In Interleaved mode, pause can be passive */
		break;

	case CMD_CONTROL_RESUME :
		PrintError("%s(%d) CMD_CONTROL_RESUME \n",__FUNCTION__,__LINE__);
		/* In Interleaved mode, resume can be passive */
		break;

	case CMD_CONTROL_SPEED :
		PrintError("%s(%d) CMD_CONTROL_SPEED (val=%d)\n",__FUNCTION__,__LINE__,(HINT32 )val);

		/* convert DI_MEDIA_TrickSpeed_e to decimal rate)*/
		switch(val) {
		case DI_MEDIA_TRICK_PAUSE :
			PrintError("%s(%d) Error> In TCP RTSP PASUE is handled by playback\n", __FUNCTION__, __LINE__);
			i_trick_scale = 0;
			//P_STREAM_RTSP_PAUSE(p_rtsp_stream);
			nret = DI_IPT_SUCCESS; /* DI_IPT_EGENERIC */
			goto success;
			//goto failed;
			//break;
#if 0
		case DI_MEDIA_TRICK_1RWSTEP :
			i_trick_scale = -1;
			break;
		case DI_MEDIA_TRICK_2RWSTEP :
			i_trick_scale = -2;
			break;
		case DI_MEDIA_TRICK_3RWSTEP :
			i_trick_scale = -3;
			break;
		case DI_MEDIA_TRICK_4RWSTEP :
			i_trick_scale = -4;
			break;
		case DI_MEDIA_TRICK_5RWSTEP :
			i_trick_scale = -5;
			break;
		case DI_MEDIA_TRICK_6RWSTEP :
			i_trick_scale = -6;
			break;
		case DI_MEDIA_TRICK_7RWSTEP :
			i_trick_scale = -7;
			break;
		case DI_MEDIA_TRICK_8RWSTEP :
			i_trick_scale = -8;
			break;
		case DI_MEDIA_TRICK_9RWSTEP :
			i_trick_scale = -9;
			break;
		case DI_MEDIA_TRICK_10RWSTEP :
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_11RWSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_12RWSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_13RWSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_14RWSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_15RWSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_16RWSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
#endif
		case DI_MEDIA_TRICK_1FFSTEP :
			i_trick_scale = 1;
			break;
		case DI_MEDIA_TRICK_2FFSTEP :
			i_trick_scale = 2;
			break;
		case DI_MEDIA_TRICK_3FFSTEP :
			i_trick_scale = 3;
			break;
		case DI_MEDIA_TRICK_4FFSTEP :
			i_trick_scale = 4;
			break;
		case DI_MEDIA_TRICK_5FFSTEP :
			i_trick_scale = 5;
			break;
		case DI_MEDIA_TRICK_6FFSTEP :
			i_trick_scale = 6;
			break;
		case DI_MEDIA_TRICK_7FFSTEP :
			i_trick_scale = 7;
			break;
		case DI_MEDIA_TRICK_8FFSTEP :
			i_trick_scale = 8;
			break;
		case DI_MEDIA_TRICK_9FFSTEP :
			i_trick_scale = 9;
			break;
		case DI_MEDIA_TRICK_10FFSTEP :
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_11FFSTEP : /* isMaxdome */
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_12FFSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_13FFSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_14FFSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_15FFSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_16FFSTEP :
			PrintError("%s(%d) Error> MAXDOME WMS does not support this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;

		default:
			PrintError("%s(%d) Error> this trick speed is not supported currently\n", __FUNCTION__, __LINE__);
			nret = DI_ERR_INVALID_PARAM;
			goto failed;
		}
		if (p_rtsp_stream->current_trick_speed == i_trick_scale) /* skip same trick request */
		{
			PrintDebug("Ignore same speed of trick...\n");
			nret = DI_ERR_INVALID_PARAM;
			goto failed;
		}

		PrintDebug("%s(%d) RTSP Play Scale also known as speed multiplier (X %d)\n",__FUNCTION__,__LINE__,i_trick_scale);

		if (val2 != 0)
		{
			val2 += 5;
		}
		nret = P_STREAM_RTSP_SendSeekWithTrickCommand(p_rtsp_stream, (HUINT64)val2, i_trick_scale);
		if (nret != DI_IPT_SUCCESS)
		{
			PrintError("%s(%d) Error> failed trick command(%d). \n",__FUNCTION__,__LINE__,cmd);
			goto failed;
		}
		p_rtsp_stream->current_trick_speed = i_trick_scale;
		break;

	case CMD_CONTROL_SEEK : /* Seek() by User */
		PrintDebug("%s(%d) CMD_CONTROL_SEEK (val(position)=%lu) is supported !! \n",__FUNCTION__,__LINE__,(unsigned long)val);

		if (val != 0)
		{
			val += 5;
		}

		nret = P_STREAM_RTSP_SendSeekWithTrickCommand(p_rtsp_stream, val , 1);
		if (nret != DI_IPT_SUCCESS)
		{
			PrintError("%s(%d) Error> failed seek command(%d). \n",__FUNCTION__,__LINE__,cmd);
			goto failed;
		}
		p_rtsp_stream->current_trick_speed = 1;
		break;

	default :
		PrintError("%s(%d) Error> Unknown Command.(%d). \n",__FUNCTION__,__LINE__,cmd);
		nret =  DI_IPT_EGENERIC;
		goto failed;
		break;
	}

success:
;

failed:
	PrintExit;
	return nret;
}


void MMSR_Close ( STREAM_ACCESS_t *pAccess )
{
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;

	PrintEnter;

	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null arg\n", __FUNCTION__, __LINE__);
		return;
	}

#if STREAM_RECORD_FOR_DEBUG
	if (pAccess->b_openfromnexus == TRUE)
	{
		if (g_fd != NULL)
		{
			fclose ( g_fd);
			PrintDebug("=== close success ===\n");
		}
	}
#endif

	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;
	if (p_rtsp_stream)
	{
		P_STREAM_RTSP_Close(p_rtsp_stream);
		pAccess->p_sys = NULL;
	}

	PrintExit;
	return;
}


HINT32  STREAM_RTSP_Open ( STREAM_ACCESS_t *pAccess )
{
	URL_t *ptURL = NULL;
	struct download_opts_t *dlopts = NULL;
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;
	HINT32 nret = 0;
	HINT32 nVKRet = 0; /* for virtaul kernal */
	HBOOL bForceWMS = FALSE; /* server_type_probing will be done */

	PrintEnter;

	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null access\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	PrintDebug("%s(%d) [b_openfromnexus:%d]\n", __FUNCTION__, __LINE__, pAccess->b_openfromnexus);

	/* Set up pAccess */
	pAccess->info.i_update = 0;
	pAccess->info.i_size = 0;
	pAccess->info.i_pos = 0;
	pAccess->info.b_eof = FALSE;
	pAccess->info.i_title = 0;
	pAccess->info.i_seekpoint = 0;

#if STREAM_RECORD_FOR_DEBUG
	if (pAccess->b_openfromnexus == TRUE)
	{
		g_reenter_counter = 0;

		g_fd = open ( "/backtrick.asf" , O_CREAT|O_TRUNC|O_WRONLY , 0666);
		if (g_fd == NULL)
		{
			PrintDebug("=== open failed ===\n");
		}
		else
		{
			PrintDebug("=== open success ===\n");
		}
	}
#endif

	/* parse path and make url and setup downloadopt */
	ptURL = DD_MEM_Alloc(sizeof(URL_t));
	if (ptURL == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto failed;
	}
	VK_memset(ptURL ,0, sizeof(URL_t));
	UTIL_UrlParse(ptURL, pAccess->psz_path, 0 );

	/* check whether target url is valid */
	if (ptURL == NULL)
	{
		PrintError("%s(%d) Error> Invalid URL info(%s)\n", __FUNCTION__, __LINE__, pAccess->psz_path);
		goto failed;
	}
	if (!ptURL->psz_host || !VK_strcmp(ptURL->psz_host,""))
	{
		PrintError("%s(%d) Error> Invalid host(%s)\n", __FUNCTION__, __LINE__, pAccess->psz_path);
		goto failed;
	}
	if (!ptURL->psz_path || !VK_strcmp(ptURL->psz_path,""))
	{
		PrintError("%s(%d) Error> Invalid path(%s)\n", __FUNCTION__, __LINE__, pAccess->psz_path);
		goto failed;
	}


	/* set download options */
	dlopts = P_STREAM_RTSP_NewRtspOption();
	if (dlopts == NULL)
	{
		PrintError("%s(%d) Error> Can't get dl option\n", __FUNCTION__, __LINE__);
		goto failed;
	}
	dlopts->expected_server_type = RTSP_SERVER_Unknown; /* server_type_probing will be done */
	dlopts->p_speed = NULL;
	dlopts->i_oobPort = _RTSP_OOB_CHANNEL_PORT;

	if (1) /* ___USE_TRANSPORT_UDP___ */
	{
		dlopts->data_transport_protocol = RTSP_TRANSPORT_RTP_UNICAST;
	}
	else
	{
		dlopts->data_transport_protocol = RTSP_TRANSPORT_TCP;
	}

	/* alloc RTSP_ACCESS_SYS_s */
	p_rtsp_stream = P_STREAM_RTSP_NewRtspAccessSysAndRtspCtrl();
	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> Can't get RTSP handle\n", __FUNCTION__, __LINE__);
		goto failed;
	}
	p_rtsp_stream->p_keepalive_thread = NULL;
	p_rtsp_stream->current_trick_speed = 1;

	/* setup rtsp_stream */
	p_rtsp_stream->p_url = ptURL;
	if (1)
	{
		ptURL = NULL; /* Dont free ptURL and just set NULL because we will use p_rtsp_stream->p_url */
	}
	p_rtsp_stream->dlopts = dlopts;
#ifdef __RTSP_THIS_IS_UNUSED_VARS__
	p_rtsp_stream->localfile = VK_StrDup((char*)&default_file_name_for_empty);
#endif

	nret = VK_SEM_Create(&p_rtsp_stream->lock_read, "rtspread", DEFAULT_SUSPEND_TYPE);
	if ( nret != VK_OK )
	{
		PrintError("%s(%d) Error> Can't create SEM(0x%x) \n\r",__FUNCTION__,__LINE__, nret);
		goto failed;
	}
	nret = VK_SEM_Create(&p_rtsp_stream->lock_write, "rtspwrite", DEFAULT_SUSPEND_TYPE);
	if ( nret != VK_OK )
	{
		PrintError("%s(%d) Error> Can't create SEM(0x%x) \n\r",__FUNCTION__,__LINE__, nret);
		goto failed;
	}
	nret = VK_SEM_Create(&p_rtsp_stream->lock_sync, "rtspsync", DEFAULT_SUSPEND_TYPE);
	if ( nret != VK_OK )
	{
		PrintError("%s(%d) Error> Can't create SEM(0x%x) \n\r",__FUNCTION__,__LINE__, nret);
		goto failed;
	}

	/* Control channel is TCP */
	p_rtsp_stream->ctrlChannel->transport_type = RTSP_TRANSPORT_TCP;

	/* Open data channel */
	p_rtsp_stream->bUseDataChannel = TRUE;
	p_rtsp_stream->bDataChannelError = FALSE;
	p_rtsp_stream->bUseRTPChannel = TRUE;

	if (pAccess->b_openfromnexus == FALSE) /* probe */
	{
		PrintError("%s(%d) Error> Skip the RTSP SETUP for probe\n",__FUNCTION__,__LINE__);
		;
	}
	else
	{
		/* Describe, process SDP, SETUP */
		nret = P_STREAM_RTSP_Open(p_rtsp_stream, bForceWMS);
		if (nret <= 0)
		{
			PrintError("%s(%d) Error> cannot open RTSP connection\n",__FUNCTION__,__LINE__);
			goto failed;
		}
	}

	/* setup pAccess */
	pAccess->p_sys = p_rtsp_stream;
	if (p_rtsp_stream->stream_ctrl == NULL)
	{
		PrintError("%s(%d) Error> NULL stream_ctrl\n",__FUNCTION__,__LINE__);
		goto failed;
	}

	pAccess->info.i_size = p_rtsp_stream->stream_ctrl->file_size;
	if (pAccess->info.i_size == 0)
	{
		PrintError("%s(%d) Error> Invalid file size(%lld)\n",__FUNCTION__,__LINE__, pAccess->info.i_size);
		;
	}

	if (pAccess->b_openfromnexus == TRUE)
	{
		/* Setup keep alive thread */
		p_rtsp_stream->p_keepalive_thread = (struct rtsp_keepalive_thread_t *)DD_MEM_Alloc(sizeof(struct rtsp_keepalive_thread_t));
		if (p_rtsp_stream->p_keepalive_thread == NULL)
		{
			PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
			goto failed;
		}
		VK_memset(p_rtsp_stream->p_keepalive_thread ,0, sizeof(struct rtsp_keepalive_thread_t));

		p_rtsp_stream->p_keepalive_thread->p_stream = p_rtsp_stream;
		p_rtsp_stream->p_keepalive_thread->b_paused = FALSE;
		p_rtsp_stream->p_keepalive_thread->b_thread_error = FALSE;
		p_rtsp_stream->p_keepalive_thread->b_quit = FALSE;

		nVKRet = VK_TASK_Create ( P_STREAM_RTSP_CommonKeepAliveTask
					, AUDIO_CLIP_TASK_PRIORITY
					, RTSP_STACK_SIZE
					, "P_STREAM_RTSP_CommonKeepAliveTask"
					, (void *)p_rtsp_stream->p_keepalive_thread
					, &p_rtsp_stream->p_keepalive_thread->thread_id
					, FALSE); /* TRUE means JOINABLE */

		if ( VK_OK != nVKRet )
		{
			PrintError("%s(%d) Error> KeepAliveThread task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
			p_rtsp_stream->p_keepalive_thread->b_thread_error = TRUE;
			return DI_IPT_ETHREAD;
		}
		(void)VK_TASK_Start(p_rtsp_stream->p_keepalive_thread->thread_id);
	}
	PrintDebug("%s(%d) Rtsp open success!\n",__FUNCTION__,__LINE__);

	PrintExit;
	return DI_IPT_SUCCESS;


failed:
	PrintError("%s(%d) Error> Function failed\n",__FUNCTION__,__LINE__);
	P_STREAM_RTSP_Close(p_rtsp_stream);

	if (ptURL != NULL)
	{
		UTIL_UrlClean( ptURL );
		DD_MEM_Free(ptURL);
		ptURL = NULL;
	}

	if (dlopts != NULL) /* CID #46215 */
	{
		DD_MEM_Free(dlopts);
		dlopts = NULL;
	}

	PrintExit;
	return DI_IPT_EGENERIC;
}

#if 0
/* <------------------------------- hexa debug -------------------------------> */
#undef TRACE_HEX
#define TRACE_HEX(x) VK_printf x;

/* P_STREAM_RTSP_DumpHex("hexdump|", p_buffer, 1024); */
static unsigned long P_STREAM_RTSP_DumpHex(char *string, HUINT8 *pBuf, HUINT32 length)
{
	static const char *_gHexChars = "0123456789abcdef";

	/* Check the length */
	if (length > 0)
	{
		/* Sanitise the input */
		if (length > 64*1024)
		{
			TRACE_HEX(("Size of Dumping data is too much! %s, %d\n", __FILE__, __LINE__));
			/* k_fatal(0, K_LOCAL); */
		}
		else
		{
			HUINT8	buf [90];
			unsigned long	i;

			TRACE_HEX(("################################################################################## \n"));
			TRACE_HEX(("# Hex Dump of \"%s\". %ld (0x%0lx) bytes\n",string, length, length));
			TRACE_HEX(("################################################################################## \n"));

			/* Hex dump of the message body */
			for (i = 0; i < length; i += 16)
			{
				unsigned long	j;
				int		len;

				/* Print the byte u64Offset */
			#if 0
				len = VK_sprintf ((char *)buf,  "%05lx: ", i);   /* Byte address */
			#else
				len = VK_sprintf ((char *)buf,  "hex| %05lx: ", i);
			#endif
				/* Print hex values */
				for (j = 0; j < 16; j++)
				{
					if (i+j < length)
					{
						buf[len++] = _gHexChars [((pBuf[i+j] >> 4) & 0xF)];
						buf[len++] = _gHexChars [(pBuf[i+j] & 0xF)];
					}
					else
					{
						buf[len++] = '.';
						buf[len++] = '.';
					}
					buf[len++] = ' ';
				}

				/* Print the ASCII equivelents */
				for (j = 0; j < 16; j++)
				{
					if (i+j < length)
						buf[len++] = VK_isprint(pBuf[i+j]) ? pBuf[i+j] : '.';
					else
						buf[len++] = ' ';
				}
				buf[len] = '\0';

				TRACE_HEX (("%s\n", buf));
			}
		}
	}

	return length;
}
/* <------------------------------- hexa debug -------------------------------> */
#endif

ssize_t STREAM_RTSP_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *pBuffer, size_t i_request)
{
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;
	struct RTSP_CHANNEL_t *pOobChannel = NULL;
	HBOOL isProbeRead = FALSE;
	HBOOL isWMS = TRUE;
	size_t i_total = 0;
	HINT32 i_read = 0;

	PrintEnter;

	if ((pAccess == NULL) || (pAccess->p_sys == NULL))
	{
		PrintError("%s(%d) Error> Invalid Null arg\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	isProbeRead = (pAccess->b_openfromnexus == FALSE) ? TRUE : FALSE;
	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;
	if ((p_rtsp_stream->stream_ctrl == NULL) || (p_rtsp_stream->stream_ctrl->rtsp_ctrl == NULL))
	{
		PrintError("%s(%d) Error> Invalid Null RTSP sys\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	PrintDebug("%s(%d) [acc.i_pos:%"PRId64", len:%d, probe:%d, protocol:%d\n", __FUNCTION__, __LINE__, pAccess->info.i_pos, i_request, isProbeRead, p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type);

	if (p_rtsp_stream->dlopts == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null dlopts\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	isWMS = FALSE;
	if (p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_WMS)
	{
		isWMS = TRUE;
		return MMSR_Read (pAccess, pBuffer, i_request);
	}
	else if (p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_REAL)
	{
		PrintError("%s(%d) Error> Unexpected server type(%d)\n", __FUNCTION__, __LINE__, p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type);
		return DI_IPT_EGENERIC;
	}

	if ((p_rtsp_stream->dlopts->data_transport_protocol != RTSP_TRANSPORT_UDP_UNICAST) &&
		(p_rtsp_stream->dlopts->data_transport_protocol != RTSP_TRANSPORT_RTP_UNICAST))
	{
		PrintError("%s(%d) Error> Unsupported interleaved(%d)\n", __FUNCTION__, __LINE__, p_rtsp_stream->dlopts->data_transport_protocol);
		goto func_error;
	}

	pOobChannel = p_rtsp_stream->dataChannel[2];
	if ((pOobChannel == NULL) || (pOobChannel->i_sock < 0))
	{
		PrintError("%s(%d) Error> Can't get the channel(%d)\n", __FUNCTION__, __LINE__, p_rtsp_stream->dlopts->data_transport_protocol);
		goto func_error;
	}

	i_total = 0;
	if (pOobChannel->i_dataLen > 0)
	{
		PrintError("%s(%d) Error> There is small prev remained buffer[uncopied:%d, req:%d]\n", __FUNCTION__, __LINE__, pOobChannel->i_dataLen, i_request);

		if (pOobChannel->p_buffer == NULL)
		{
			PrintError("%s(%d) Error> Invalid NULL remain buffer[uncopied:%d, req:%d]\n", __FUNCTION__, __LINE__, pOobChannel->i_dataLen, i_request);
			goto func_error;
		}

		/* The remained size has sery small value */
		if (pOobChannel->i_dataLen <= i_request)
		{
			VK_memcpy(pBuffer, (HUINT8*)pOobChannel->p_buffer, pOobChannel->i_dataLen);
			i_total += pOobChannel->i_dataLen;

			/* Update position */
			pAccess->info.i_pos += i_total;

			pOobChannel->i_dataLen = 0;
			(void)RTSP_LIB_ClearChannelBuffer(pOobChannel);
		}
		else
		{
			PrintError("%s(%d) Error> Unexpected[uncopied:%d, req:%d]\n", __FUNCTION__, __LINE__, pOobChannel->i_dataLen, i_request);
			;
		}

		if (i_total >= i_request)
		{
			PrintError("%s(%d) Error> function done[i_total:%d, i_request:%d]\n", __FUNCTION__, __LINE__, i_total, i_request);
			goto func_done;
		}
	}

	i_read = 0;
	if (RTSP_TRANSPORT_RTP_UNICAST == p_rtsp_stream->dlopts->data_transport_protocol)
	{
		i_read = RTSP_SESSION_ProcessOobRTPChannel(pOobChannel, pBuffer+i_total, (i_request-i_total));
	}
	else
	{
		i_read = RTSP_SESSION_ProcessOobUDPChannel(pOobChannel, pBuffer+i_total, (i_request-i_total));
	}

	if ( i_read > 0 )
	{
		PrintDebug("%s(%d) +[read:%d, req:%d, acc.i_pos:%"PRId64"]\n", __FUNCTION__, __LINE__, i_read, i_request, pAccess->info.i_pos);
		i_total += i_read;
		pAccess->info.i_pos += i_read; /* previous remained buffer is already added when checking above routine */
	}
	else
	{
		PrintError("%s(%d) Error> recv() failed[total:%d, read:%d, req:%d, acc.i_pos:%"PRId64"]\n", __FUNCTION__, __LINE__, i_total, i_read, i_request, pAccess->info.i_pos);

		if ( i_read == 0 )
		{
			PrintError("%s(%d) Error> read:%d, acc.i_pos(%"PRId64"): \n", __FUNCTION__,__LINE__, i_read, pAccess->info.i_pos);
			pAccess->info.b_eof = TRUE;
		}
		else
		{
			PrintError("%s(%d) Error> read:%d, errno:%d(%s), acc.i_pos(%"PRId64"): \n"
					,__FUNCTION__,__LINE__, i_read, errno, strerror(errno), pAccess->info.i_pos);
			;
		}
	}


func_done:
	PrintDebug("%s(%d) --[total:%d, read:%d, req:%d, acc.i_pos:%"PRId64"]\n", __FUNCTION__, __LINE__, i_total, i_read, i_request, pAccess->info.i_pos);
	return i_total; /* not i_total; because of remaining small piece */

func_error:
	PrintError("%s(%d) Error> function failed--[total:%d, read:%d, req:%d, acc.i_pos:%"PRId64"]\n", __FUNCTION__, __LINE__, i_total, i_read, i_request, pAccess->info.i_pos);
	return DI_IPT_EGENERIC;
}

HINT32 STREAM_RTSP_Seek (STREAM_ACCESS_t * pAccess, HINT64 i_pos)
{
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;

	PrintEnter;

	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null arg\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;
	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL RTSP handle\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	if (1) /* by server_type */
	{
		if (p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_WMS)
		{
			return MMSR_Seek(pAccess, i_pos);
		}
		else if (p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_REAL)
		{
			PrintError("%s(%d) Error> Unexpected server type(%d)\n", __FUNCTION__, __LINE__, p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type);
			return DI_IPT_EGENERIC;
		}
	}

	PrintError("%s(%d) Error(info)> Unsupported; current Seek Point is %"PRId64"\n", __FUNCTION__, __LINE__, i_pos);

	pAccess->info.i_pos = i_pos;
	pAccess->info.b_eof = false;

	PrintExit;
	return DI_IPT_SUCCESS;
}

HINT32 STREAM_RTSP_Control (STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2)
{
	static HINT32 i_trick_scale = 1;
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;
	HINT32 nret = DI_IPT_SUCCESS;

	PrintEnter;

	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null arg\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;
	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL RTSP handle\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}

	if (1) /* function branch by server_type */
	{
		if (p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_WMS)
		{
			return MMSR_Control(pAccess, cmd, val, val2);
		}
		else if (p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_REAL)
		{
			PrintError("%s(%d) Error> Unexpected server type(%d)\n", __FUNCTION__, __LINE__, p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type);
			return DI_IPT_EGENERIC;
		}
	}

	switch (cmd) {
	case CMD_CONTROL_PAUSE :
		PrintError("%s(%d) CMD_CONTROL_PAUSE \n",__FUNCTION__,__LINE__);
		/* In Interleaved mode, pause can be passive */
		break;

	case CMD_CONTROL_RESUME :
		PrintError("%s(%d) CMD_CONTROL_RESUME \n",__FUNCTION__,__LINE__);
		/* In Interleaved mode, resume can be passive */
		break;

	case CMD_CONTROL_SPEED :
		PrintError("%s(%d) CMD_CONTROL_SPEED (val=%d)\n",__FUNCTION__,__LINE__,(HINT32 )val);

		/* convert DI_MEDIA_TrickSpeed_e to decimal rate)*/
		switch(val) {
		case DI_MEDIA_TRICK_PAUSE :
			PrintError("%s(%d) Error> In TCP RTSP PASUE is handled by playback\n", __FUNCTION__, __LINE__);
			i_trick_scale = 0;
			//P_STREAM_RTSP_PAUSE(p_rtsp_stream);
			nret = DI_IPT_SUCCESS; /* DI_IPT_EGENERIC */
			goto success;
#if 0
		case DI_MEDIA_TRICK_1RWSTEP :
			i_trick_scale = -1;
			break;
		case DI_MEDIA_TRICK_2RWSTEP :
			i_trick_scale = -2;
			break;
		case DI_MEDIA_TRICK_3RWSTEP :
			i_trick_scale = -3;
			break;
		case DI_MEDIA_TRICK_4RWSTEP :
			i_trick_scale = -4;
			break;
		case DI_MEDIA_TRICK_5RWSTEP :
			i_trick_scale = -5;
			break;
		case DI_MEDIA_TRICK_6RWSTEP :
			i_trick_scale = -6;
			break;
		case DI_MEDIA_TRICK_7RWSTEP :
			i_trick_scale = -7;
			break;
		case DI_MEDIA_TRICK_8RWSTEP :
			i_trick_scale = -8;
			break;
		case DI_MEDIA_TRICK_9RWSTEP :
			i_trick_scale = -9;
			break;
		case DI_MEDIA_TRICK_10RWSTEP :
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_11RWSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_12RWSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_13RWSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_14RWSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_15RWSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
		case DI_MEDIA_TRICK_16RWSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10RW instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = -10;
			break;
#endif
		case DI_MEDIA_TRICK_1FFSTEP :
			i_trick_scale = 1;
			break;
		case DI_MEDIA_TRICK_2FFSTEP :
			i_trick_scale = 2;
			break;
		case DI_MEDIA_TRICK_3FFSTEP :
			i_trick_scale = 3;
			break;
		case DI_MEDIA_TRICK_4FFSTEP :
			i_trick_scale = 4;
			break;
		case DI_MEDIA_TRICK_5FFSTEP :
			i_trick_scale = 5;
			break;
		case DI_MEDIA_TRICK_6FFSTEP :
			i_trick_scale = 6;
			break;
		case DI_MEDIA_TRICK_7FFSTEP :
			i_trick_scale = 7;
			break;
		case DI_MEDIA_TRICK_8FFSTEP :
			i_trick_scale = 8;
			break;
		case DI_MEDIA_TRICK_9FFSTEP :
			i_trick_scale = 9;
			break;
		case DI_MEDIA_TRICK_10FFSTEP :
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_11FFSTEP : /* isMaxdome */
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_12FFSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_13FFSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_14FFSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_15FFSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;
		case DI_MEDIA_TRICK_16FFSTEP :
			PrintError("%s(%d) Error> Unsupported this trick speed. using 10FF instead\n", __FUNCTION__, __LINE__);
			i_trick_scale = 10;
			break;

		default:
			PrintError("%s(%d) Error> this trick speed is not supported currently\n", __FUNCTION__, __LINE__);
			nret = DI_ERR_INVALID_PARAM;
			goto failed;
		}

		if (p_rtsp_stream->current_trick_speed == i_trick_scale) /* skip same trick request */
		{
			PrintDebug("Ignore same speed of trick...\n");
			nret = DI_ERR_INVALID_PARAM;
			goto failed;
		}

		PrintDebug("%s(%d) RTSP Play Scale also known as speed multiplier (X %d)\n",__FUNCTION__,__LINE__,i_trick_scale);

		if (val2 != 0)
		{
			val2 += 5;
		}

		nret = P_STREAM_RTSP_SendSeekWithTrickCommand(p_rtsp_stream, (HUINT64)val2, i_trick_scale);
		if (nret != DI_IPT_SUCCESS)
		{
			PrintError("%s(%d) Error> failed trick command(%d). \n",__FUNCTION__,__LINE__,cmd);
			goto failed;
		}
		p_rtsp_stream->current_trick_speed = i_trick_scale;
		break;

	case CMD_CONTROL_SEEK : /* Seek() by User */
		PrintDebug("%s(%d) CMD_CONTROL_SEEK (val(position)=%lu) is supported !! \n",__FUNCTION__,__LINE__,(unsigned long)val);

		if (val != 0)
		{
			val += 5;
		}

		nret = P_STREAM_RTSP_SendSeekWithTrickCommand(p_rtsp_stream, val , 1);
		if (nret != DI_IPT_SUCCESS)
		{
			PrintError("%s(%d) Error> failed seek command(%d). \n",__FUNCTION__,__LINE__,cmd);
			goto failed;
		}
		p_rtsp_stream->current_trick_speed = 1;
		break;

	default :
		PrintError("%s(%d) Error> Unknown Command.(%d). \n",__FUNCTION__,__LINE__,cmd);
		nret =  DI_IPT_EGENERIC;
		goto failed;
		break;
	}

success:
	;

failed:
	PrintExit;
	return nret;
}


void STREAM_RTSP_Close ( STREAM_ACCESS_t *pAccess )
{
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;

	PrintEnter;

	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null arg\n", __FUNCTION__, __LINE__);
		return;
	}

#if STREAM_RECORD_FOR_DEBUG
	if (pAccess->b_openfromnexus == TRUE)
	{
		if (g_fd != NULL)
		{
			fclose ( g_fd);
			PrintDebug("=== close success ===\n");
		}
	}
#endif

	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;
	if (p_rtsp_stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid Null RTSP sys\n", __FUNCTION__, __LINE__);
		return;
	}

	if (p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_WMS)
	{
		MMSR_Close(pAccess);
		return;
	}
	else if (p_rtsp_stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type == RTSP_PROTOCOL_REAL)
	{
		PrintError("%s(%d) Error> Unexpected real/helix server\n", __FUNCTION__, __LINE__);
		return;
	}

	P_STREAM_RTSP_Close(p_rtsp_stream);
	pAccess->p_sys = NULL;

	PrintExit;
	return;
}

