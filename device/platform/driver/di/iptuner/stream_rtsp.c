/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_rtsp.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	JungWook Chai
 * Current Author:	$Author: juchae@humaxdigital.com $
 * Date:			$Date: Thur Oct 29 10:20:48 KST 2009 $
 * File Description:	Real-Time STREAMMING IP Tuner Header File
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.							  */
/* All rights reserved.											*/
/*******************************************************************/




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




#include "htype.h"
#include "taskdef.h"
#include "di_uart.h"
#include "stream_global.h"

#include "di_iptuner_config.h"
#include "di_err.h"
#include "di_media.h"
#include "di_iptuner.h"
#include "di_iptuner_common.h"

#include "di_iptuner_mms.h"
//#include "di_iptuner_rtsp.h"

// RTSP Header
#include "rtsp_lib.h"
#include "mms_asf.h"


#include "util_url.h"

#include "stream_rtsp.h"
#include "rtsp_ms.h"

// network
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>



/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define STREAM_RTSP_DEBUG
//#define STREAM_RTSP_CRITICAL_DEBUG
//#define IPTUNER_ALL_DEBUG
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

#define PRE_DETECT_SERVER_TYPE 0
#define STREAM_RECORD_FOR_DEBUG 0

const char default_file_name_for_empty[] = "debugfile";
const char rtsp_protocol_version[] = "RTSP/1.0";


#define RTSP_STACK_SIZE							(32*1024)
#define RTSP_TASK_START_WATING_TIME				(300)

#define RTSP_PROTOCOL_TYPE_PARM					("SET_PARAMETER")
#define RTSP_PROTOCOL_TYPE_OPTN					("OPTIONS")
#define RTSP_PROTOCOL_TYPE_ANNC					("ANNOUNCE")
#define RTSP_HEADER_SEPARATOR_LEN				(4)
#define RTSP_HEADER_LEN						(4)

//#define USE_TRANSPORT_UDP
static void P_STREAM_RTSP_KeepAliveTask ( void *pParam );

#if STREAM_RECORD_FOR_DEBUG
static int g_reenter_counter = 0;
static int g_fd = 0;
#endif

/* defined in real.c */
const char stream_rtsp_real_clientid[] = "ClientID: Linux_2.6_10.0.0.0_play32_RN01_EN_586";
const char stream_rtsp_real_useragent[] = "User-Agent: RealMedia Player Version 10.0.0.0 (linux-2.6-libc6-gcc4.1-i586)";
const char stream_rtsp_real_companyid[] = "CompanyID: X160hPo6JmsD6Vger24oAg==";
const char stream_rtsp_real_clientchallenge[] = "ClientChallenge: cd7cb1ac431e8d1ad4d2fadb8cf762d1";
const char stream_rtsp_real_playerstarttime[] = "PlayerStarttime: [19/03/2007:21:42:56 00:00]";
const char stream_rtsp_real_transport[] = "Transport: x-pn-tng/tcp;mode=play,rtp/avp/tcp;unicast;mode=play";
static const char wms_useragent[] = "User-Agent: WMPlayer/9.0.0.2833 guid/B64345F5-8C45-4818-8A1A-4775F0923FAC"; /* from MS-RTSP Spec example */




static struct rtsp_ctrl_t *STREAM_RTSP_NewRtspCtrl(void);
static void STREAM_RTSP_FreeRtspCtrl(struct rtsp_ctrl_t *ctrlt);
static HINT32 STREAM_RTSP_ReadRtspHeader(struct RTSP_ACCESS_SYS_s *stream, struct RTSP_PROT_HEADER_s *rtsp_hdr);
HINT32 STREAM_RTSP_ParseBasicFieldsInRtspHeader(struct rtsp_ctrl_t *rtsp_ctrl,
			struct RTSP_PROT_HEADER_s *rtsp_header);
#if 0 /* Commented with warning: 'P_STREAM_RTSP_ReadAsfStream' declared 'static' but never defined */
static HINT32 P_STREAM_RTSP_ReadAsfStream(struct RTSP_ACCESS_SYS_s *stream,
			HUINT8 *buffer, size_t buffer_size);
#endif /* Commented with warning: 'P_STREAM_RTSP_ReadAsfStream' declared 'static' but never defined */

/* keep alive 와 seek command는 send header 에서 thread safe. */
static HINT32 P_STREAM_RTSP_SendKeepAliveCommand(struct RTSP_ACCESS_SYS_s *stream)
{
	HINT32	nRet = -1;
	struct RTSP_PROT_HEADER_s *rtsp_hdr;
	struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;
	HUINT32 nCurrentSeqNum = 0;
	HINT32 nRtspStatusCode = 0;

	PrintEnter;

	rtsp_hdr = STREAM_RTSP_NewRtspHeaderAndFillBasicFields(stream_ctrl->protocol.rtsp_ctrl);
	STREAM_RTSP_SetProtoclCmdForRtspHeader(rtsp_hdr,STREAM_RTSP_CMD_GET_PARAMETER);
	STREAM_RTSP_SetUriForRtspHeader(rtsp_hdr,stream_ctrl->protocol.rtsp_ctrl->mrl);
	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,wms_useragent);
	STREAM_RTSP_SerializeRtspHeader(rtsp_hdr);

	nRet = STREAM_RTSP_SendSerializedRtspRequestAndFree(stream,rtsp_hdr);
	if ( nRet < 0 )
	{
		return nRet;
	}

	nCurrentSeqNum = stream_ctrl->protocol.rtsp_ctrl->cseq-1;

#if 1
	if(stream->b_continuous_send == TRUE) /* need sock read lock (rival with MMSR_Read) */
	{
		VK_SEM_Get(stream->lock_read);
		while(TRUE)
		{
			/* stream buffer will be flushed after feed asf header first */
			nRtspStatusCode = RTSP_MS_ProcessInterleavedPacket(stream);
			if(stream_ctrl->protocol.rtsp_ctrl->uiLastAnswerCSeq >= nCurrentSeqNum) /* wait last sent cmd's response seq */
			{
				break;
			}

			if(stream->bInterleavedDataLefted == TRUE) /* read and trash garbage packets */
			{
				char* read_buffer;

				read_buffer = DD_MEM_Alloc(BUFSIZE_1K * 500);
				PrintDebug("[read and trash buffered packets]\n");
				RTSP_MS_ProcessRtpPacket(stream, (HUINT8 *)read_buffer , BUFSIZE_1K * 500);
				DD_MEM_Free(read_buffer);
			}
		}
		VK_SEM_Release(stream->lock_read);
		stream->b_continuous_send = FALSE;
	}
	else
	{
		stream->b_continuous_send = TRUE;
	}
#else
	//STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr); /* this cause double free */

	//STREAM_RTSP_ReadRtspResponseAndFree(stream);
	/* response RTSP OK... is deffered read by mmsr_read */
	/* many send only operation causes sock write error */

	/* for pause status keep alive */
	if(stream->b_continuous_send == TRUE) /* need sock read lock (rival with MMSR_Read) */
	{	/* recv header와 MMSR_read는 thread safe */
		PrintDebug("Double KeepAlive Command.. read response..\n");
		//VK_SEM_Get(stream->lock_read);

		//VK_TASK_Sleep(1000);

		/* TODO need peek logic */
		//RTSP_MS_ProcessRtspResponse(stream);
		//STREAM_RTSP_ReadRtspResponseAndFree(stream);
		//VK_SEM_Release(stream->lock_read);
		stream->b_continuous_send = FALSE;
	}
	else
	{
		stream->b_continuous_send = TRUE;
	}
#endif

	PrintExit;
	return 0;
}

static HINT32 P_STREAM_RTSP_SendSeekWithTrickCommand(struct RTSP_ACCESS_SYS_s *stream, HUINT64 npt, HINT32 nScale)
{
		struct RTSP_PROT_HEADER_s *rtsp_hdr;
		struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;
		char* buffer;
		HUINT32 nCurrentSeqNum = 0;
		HINT32 nRtspStatusCode = 0;
		HINT32 ret = DI_IPT_SUCCESS;
		PrintEnter;

		if(stream->bDuringSeeking == TRUE || stream->b_feed_asfheader_first == TRUE)
		{
			PrintDebug("Re Entering Send Seek Command\n");
			ret = DI_IPT_EGENERIC;
			goto failed;
		}

		/* stop MMSTU_Read */
		stream->bDuringSeeking = TRUE;

		buffer = DD_MEM_Alloc(BUFSIZE_1K);
		VK_memset(buffer ,0, BUFSIZE_1K);

		rtsp_hdr = STREAM_RTSP_NewRtspHeaderAndFillBasicFields(stream_ctrl->protocol.rtsp_ctrl);
		//rtsp_request_pause(rtsp_hdr,stream_ctrl->protocol.rtsp_ctrl->mrl);
		STREAM_RTSP_SetProtoclCmdForRtspHeader(rtsp_hdr,STREAM_RTSP_CMD_PAUSE);
		STREAM_RTSP_SetUriForRtspHeader(rtsp_hdr,stream_ctrl->protocol.rtsp_ctrl->mrl);
		STREAM_RTSP_SerializeRtspHeader(rtsp_hdr);

		STREAM_RTSP_SendSerializedRtspRequestAndFree(stream,rtsp_hdr);

		/*STREAM_RTSP_ReadRtspResponseAndFree(stream);*/
		/* response RTSP OK... is deffered read by mmsr_read */

		if(stream->dlopts->range)
		{
			DD_MEM_Free(stream->dlopts->range);
			stream->dlopts->range = NULL;
		}

		VK_snprintf(buffer, BUFSIZE_1K, "%.3f-", (double)npt);
		PrintDebug( "Seek cmd NPT string = [%s]\n", buffer);
		stream->dlopts->range = VK_StrDup(buffer);
		DD_MEM_Free(buffer);
		buffer = NULL;

		/* if WMS -> WMS IMPL */
		nCurrentSeqNum = stream_ctrl->protocol.rtsp_ctrl->cseq;
		RTSP_MS_AsyncPlay(stream, (float)nScale);

		PrintDebug("Current CSEQ = %u\n", nCurrentSeqNum);

		/*STREAM_RTSP_ReadRtspResponseAndFree(stream);*/
		/* response RTSP OK... is deffered read by mmsr_read */

		VK_SEM_Get(stream->lock_read);
		{
			char* read_buffer;
			read_buffer = DD_MEM_Alloc(BUFSIZE_1K * 500);
			while(TRUE)
			{
				/* stream buffer will be flushed after feed asf header first */
				nRtspStatusCode = RTSP_MS_ProcessInterleavedPacket(stream);
				PrintDebug("answer seq num = %u, status code = %d\n", stream_ctrl->protocol.rtsp_ctrl->uiLastAnswerCSeq, nRtspStatusCode);
				if(stream_ctrl->protocol.rtsp_ctrl->uiLastAnswerCSeq >= nCurrentSeqNum) /* wait last sent cmd's response seq */
				{
					break;
				}
				if(stream->bInterleavedDataLefted == TRUE) /* read and trash garbage packets */
				{
					PrintDebug("[read and trash buffered packets]\n");
					RTSP_MS_ProcessRtpPacket(stream, (HUINT8 *)read_buffer , BUFSIZE_1K * 500);/*read*/
				}

			}
			DD_MEM_Free(read_buffer);
		}
		VK_SEM_Release(stream->lock_read);


		if(nRtspStatusCode != 200) /* Expect RTSP 200 OK */
		{
			PrintDebug("STATUS CODE %d\n",nRtspStatusCode);
			PrintDebug("Denied Server Side Seek or Trick\n");
			ret = DI_IPT_EGENERIC;
			goto failed;
		}

/* success: */

		/* this must moved to seekfornexus 0 */
		stream->b_feed_asfheader_first = TRUE;
		PrintExit;
		return ret;

failed:
	PrintExit;
	return ret;


}



static void P_STREAM_RTSP_KeepAliveTask ( void *pParam )
{
	struct rtsp_keepalive_thread_t *p_thread = (struct rtsp_keepalive_thread_t *) pParam;
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = p_thread->p_stream;



//	lock( p_thread );
	PrintEnter;

	while ( true) //object alive ??( p_thread) )
	{
		if(p_rtsp_stream->Channel->is_started == FALSE)
		{
			VK_TASK_Sleep (1000);
			PrintDebug("Keep Alive Task is idle \n");
			continue;
		}
		(void)VK_TASK_Sleep (p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->keepalive_interval * 667); /* interval is lesser than SETUPed interval's 2/3 */
		//(void)VK_TASK_Sleep (20000); /* for test */
		PrintDebug( "Send keep alive CMD \n");

		(void)P_STREAM_RTSP_SendKeepAliveCommand(p_rtsp_stream);
	}


	PrintExit;
//	unlock ( p_thread );
	return ;
}





/*
 * read rtsp stream. filles buffer, and buffer size is 'size'
 *
 *		read cached data from stream->Channel->buffer
 *		check for to see if network access is necessary
 *		get chunk(media packet) from network.
 *
 *  return value: bytes written to buffer. -1 if error.
 *										  0 if buffering
 */
static HINT32 P_STREAM_RTSP_ReadFromStreamBuffer(struct RTSP_ACCESS_SYS_s *stream,
			HUINT8 *buffer, size_t buffer_size)
{
	struct RTSP_CTRL_s *stream_ctrl = NULL; //stream->stream_ctrl;
#if 0 /* warning: unused variable */
	HINT32 ret = 0;
#endif

	//PrintEnter;

	/* data channel 을 고른다. */
	switch(stream->dlopts->data_transport_protocol )
	{
	case RTSP_TRANSPORT_TCP:
		stream_ctrl = stream->stream_ctrl;
		break;
	case RTSP_TRANSPORT_UDP_UNICAST:
		stream_ctrl = stream->stream_ctrl;
		break;
	default:
		PrintError("un supported data_transport_protocol\n");
		return -1;
	}


	if(buffer_size == 0) {
	PrintCriticalDebug("buffer_size must be bigger than 0\n");
	return -1;
	}

	if(stream_ctrl->write_data_len)/* write_buffer에 대기중인 data 존재 */
	{

		if(buffer_size <= stream_ctrl->write_data_len) /* buffer size is large enough */
		{
			VK_memcpy(buffer,stream_ctrl->write_buffer + stream_ctrl->write_pos, buffer_size);

			stream_ctrl->write_data_len -= buffer_size;
			stream_ctrl->write_pos += buffer_size;

			if((stream_ctrl->last_packet_status == LAST_PACKET_ARRIVED || stream_ctrl->status == RTSP_STREAMING_FINISHED) && stream_ctrl->write_data_len == 0)
			{
					stream_ctrl->last_packet_status = LAST_PACKET_READ;
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
			if((stream_ctrl->last_packet_status == LAST_PACKET_ARRIVED || stream_ctrl->status == RTSP_STREAMING_FINISHED) && stream_ctrl->write_data_len == 0)
			{
					stream_ctrl->last_packet_status = LAST_PACKET_READ;

			}
			/* partial 패킷 이후 buffer에 추가 패킷을 넣는 것을 막는다. */
			return pos;
			}
			PrintCriticalDebug( "you should not see this line %s %d\n",__FUNCTION__, __LINE__);
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
HINT32 STREAM_RTSP_ReadAndFreeBody(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	char *field = NULL;
	HINT32 body_left = 0;

	PrintEnter;

	if((field = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Content-Length")) != NULL) {
	HUINT8 *ignorebuf = NULL;

	while(*field == ' ') field++;
	body_left = VK_atoi(field);
	if(body_left) {
		ignorebuf = DD_MEM_Alloc(body_left);
		VK_memset(ignorebuf ,0, body_left);
		RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,ignorebuf,body_left);
		DD_MEM_Free(ignorebuf);
		ignorebuf = NULL;
	}
	}

	PrintExit;
	return body_left;
}


HINT32 STREAM_RTSP_CheckRtspResponseOk(HINT32 status_code)
{
	PrintEnter;
	return ((200 <= status_code) && (status_code <= 299)) ? 1 : 0;
}


/*
 * receive rtsp header but ignore its message, etc..
 * this function is purely for making code more readable.
 *
 *	   return value:  what STREAM_RTSP_ReadRtspHeaderAndParseBasic returned
 */
HINT32 STREAM_RTSP_ReadRtspResponseAndFree(struct RTSP_ACCESS_SYS_s *stream)
{
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	HINT32 ret = 0;

	PrintEnter;

	rtsp_hdr = STREAM_RTSP_NewRtspProtocolHeader();
	ret = STREAM_RTSP_ReadRtspHeaderAndParseBasic(stream,rtsp_hdr);

	/* if content-length field, get all following data */
	STREAM_RTSP_ReadAndFreeBody(stream,rtsp_hdr);

	STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);
	PrintExit;
	return ret;
}



/*
 * alloc new RTSP_PROT_HEADER_s and set standard fields for send.
 * this function is purely for making code more readable.
 *
 * return value:  what STREAM_RTSP_ReadRtspHeaderAndParseBasic returned (status code: success  -1: failure)
 */
struct RTSP_PROT_HEADER_s *STREAM_RTSP_NewRtspHeaderAndFillBasicFields(struct rtsp_ctrl_t *rtsp_ctrl)
{
	struct RTSP_PROT_HEADER_s *rtsp_hdr;

	PrintEnter;

	rtsp_hdr = STREAM_RTSP_NewRtspProtocolHeader();
	STREAM_RTSP_SetRtspHeaderWithBasicFields(rtsp_ctrl,rtsp_hdr);
	PrintExit;
	return rtsp_hdr;
}


/*
 * send rtsp_request and free it.
 * this function is purely for making code more readable.
 *
 * return value:  what rtsp_send request returned. (status code: success  -1: failure)
 */
HINT32 STREAM_RTSP_SendSerializedRtspRequestAndFree(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	HINT32 ret;
	PrintEnter;

	ret = STREAM_RTSP_SendSerializedRtspRequest(stream,rtsp_hdr);
	STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);

	PrintExit;

	return ret;
}



/*
 * set Range: field to rtsp_hdr.
 */
void STREAM_RTSP_SetRangeField(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *rangestr)
{
	char *buffer;

	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	VK_memset(buffer ,0, BUFSIZE_1K);

	/* range specified */
	if(rangestr) {
		VK_snprintf(buffer,BUFSIZE_1K - 1,"Range: npt=%s",rangestr);
	}
	else { /* default range, from beginning */
	VK_strncpy(buffer,"Range: npt=0.000-",BUFSIZE_1K - 1);
	}
	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,buffer);

	DD_MEM_Free(buffer);
	buffer = NULL;

	PrintExit;
}


/*
 * set Speed: field to rtsp_hdr.
 */
void STREAM_RTSP_SetSpeedField(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *speedstr)
{
	char *buffer;

	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	VK_memset(buffer ,0, BUFSIZE_1K);

	/* speed specified */
	if(speedstr) {
		VK_snprintf(buffer,BUFSIZE_1K - 1,"Speed: %s",speedstr);
	}
	else { /* set default speed*/
	VK_strncpy(buffer,"Speed: 1.000",BUFSIZE_1K - 1);
	}
	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,buffer);

	DD_MEM_Free(buffer);
	buffer = NULL;

	PrintExit;
}


/*
 * receive rtsp header and set rtsp_ctrl
 *			  return status code : success
 *							   -1 : failure
 */
HINT32 STREAM_RTSP_ReadRtspHeaderAndParseBasic(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	HINT32 ret = 0;

	PrintEnter;

	ret = STREAM_RTSP_ReadRtspHeader(stream,rtsp_hdr);
	if(ret < 0) {
		PrintCriticalDebug( "STREAM_RTSP_ReadRtspHeader ERROR \n");
		PrintExit;
	return -1;
	}

	ret = STREAM_RTSP_ParseBasicFieldsInRtspHeader(stream->stream_ctrl->protocol.rtsp_ctrl,rtsp_hdr);
	if(ret < 0) {
		PrintCriticalDebug( "STREAM_RTSP_ParseBasicFieldsInRtspHeader ERROR \n");
		PrintExit;
	return -1;
	}

	PrintExit;
	return rtsp_hdr->status_code;
}



/*
 * receive rtsp message.
 */
static HINT32 STREAM_RTSP_ReadRtspHeader(struct RTSP_ACCESS_SYS_s *stream, struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	HINT32 ret = 0,n = 0,total = 0;

  PrintEnter;

	rtsp_hdr->buffer_len = 0;
	rtsp_hdr->buffer = NULL;
	total = 0;

	do { /* get rtsp reply */
	rtsp_hdr->buffer_len += BUFSIZE_1K;
	rtsp_hdr->buffer = (char *)RTSP_LIB_Realloc(rtsp_hdr->buffer,
						rtsp_hdr->buffer_len + 1);

		n = RTSP_LIB_ReadFromCtrlChannelWithNoWait(stream,rtsp_hdr->buffer + total,BUFSIZE_1K);
		if(n <= 0) {
			PrintCriticalDebug("STREAM_RTSP_ReadRtspHeaderAndParseBasic error: RTSP_LIB_ReadFromCtrlChannelWithNoWait() returned %d\n",n);
		goto failed;
	}
		total += n;
		rtsp_hdr->buffer[total] = '\0'; /* EOS is for STREAM_RTSP_IsEndOfRtspHeader */
	} while(!STREAM_RTSP_IsEndOfRtspHeader(rtsp_hdr));

	rtsp_hdr->buffer_len = total;
	rtsp_hdr->buffer[total] = '\0';

	ret = STREAM_RTSP_DeserializeRtspResponse(rtsp_hdr);
	if(ret < 0) {
	PrintCriticalDebug("response RTSP header parse failed\n");
	goto failed;
	}

	if(rtsp_hdr->body_len)
	{
	RTSP_LIB_PutDataForLaterRead(stream,rtsp_hdr->body,rtsp_hdr->body_len);
	}

	/* case of "rtsp_hdr->buffer" only hvae contains header */
	VK_memset(rtsp_hdr->body,0,rtsp_hdr->body_len);

	/* dbg */
	PrintDebug("STREAM_RTSP_ReadRtspHeader : RTSP HEADER (%d) ----------------------\n", VK_strlen(rtsp_hdr->buffer));
	PrintDebug("%s\n", rtsp_hdr->buffer);
	PrintDebug("--------------------------------------------------------------------\n");

   /* success */
   PrintExit;
	return rtsp_hdr->status_code;

  failed:
	if(rtsp_hdr->buffer)
	{
		DD_MEM_Free(rtsp_hdr->buffer);
		rtsp_hdr->buffer = NULL;
	}
	PrintExit;
	return -1;
}

#if 1 /* jhchoi2 : fixed maxdome rtsp issue */
static void strreverse(char* begin, char* end) {
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}

static void uitoa32(unsigned int value, char* str, int base) {
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

	//	if(sign<0) *wstr++='-';
	*wstr='\0';

	// Reverse string
	strreverse(str,wstr-1);
}


#endif

/*
 * set fields which are common in all rtsp packet.
 * CSeq and Session field should be present in
 * every rtsp packet.
 */
HINT32 STREAM_RTSP_SetRtspHeaderWithBasicFields(struct rtsp_ctrl_t *rtsp_ctrl,
				 struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	char buf[20];
	char *buf2=NULL;
	char len=0;

	PrintEnter;

	VK_memset(buf,0,sizeof(buf));

	/* jhchoi2 : fixed maxdome rtsp issue */
	{
		char tmp_buf[20] = {0,};
		VK_memset(tmp_buf, 0, sizeof(tmp_buf));
		(void)uitoa32(rtsp_ctrl->cseq, tmp_buf, 10); //use local static function
		len = VK_strlen(tmp_buf);
	}

	(void)VK_snprintf(buf,len+7,"Cseq: %u",rtsp_ctrl->cseq);

	(void)STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,buf);

	if(rtsp_ctrl->session)
	{
		buf2 = (char *)DD_MEM_Alloc(VK_strlen(rtsp_ctrl->session) + 15);
		VK_memset(buf2 ,0, VK_strlen(rtsp_ctrl->session) + 15);
		len = VK_strlen(rtsp_ctrl->session);
		(void)VK_snprintf(buf2,len+10,"Session: %s",rtsp_ctrl->session);
		STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,buf2);
		DD_MEM_Free(buf2);
		buf2 = NULL;
	}

	PrintExit;
	return 0;
}


/*
 * send rtsp header (rtsp_hdr->buffer)
 */
HINT32 STREAM_RTSP_SendSerializedRtspRequest(struct RTSP_ACCESS_SYS_s *stream,
			  struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	HINT32 ret;

	PrintEnter;

	/* dbg */
	PrintDebug("SEND RTSP REQUEST ------------\n"
		"%s\n--(%d bytes)---------------\n",
		rtsp_hdr->buffer,(int)rtsp_hdr->buffer_len);



	VK_SEM_Get(stream->lock_write);
	 fcntl(stream->Channel->sock, F_SETFL, fcntl(stream->Channel->sock,F_GETFL) & ~O_NONBLOCK);
	ret = RTSP_LIB_SockWrite(stream->Channel->sock,rtsp_hdr->buffer,rtsp_hdr->buffer_len);
	fcntl(stream->Channel->sock,F_SETFL,fcntl(stream->Channel->sock,F_GETFL) | O_NONBLOCK);
	VK_SEM_Release(stream->lock_write);

	if(ret < 1)
	{
		PrintError("Network Write Error %d", errno);
	}



	/* OK change stream->stream_ctrl->protocol.rtsp_ctrl->cseq!! */
	stream->stream_ctrl->protocol.rtsp_ctrl->cseq++;

	PrintExit;

	return ret;
}



/*
 * set basic stuff and send OPTIONS request.
 * this can be used to determine  which protocol to use,
 * depending on its server.
 *			  return value	sock : success
 *								-1 : failure
 */
HINT32 STREAM_RTSP_Connect(struct RTSP_ACCESS_SYS_s *stream)
{
	HINT32 sock;
	struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;
	struct rtsp_ctrl_t *rtsp_ctrl = stream_ctrl->protocol.rtsp_ctrl;
	URL_t *url = stream->url;
	PrintEnter;

	if(url->psz_host == NULL)
		PrintDebug( "hostname is null");
	if(url->i_port == 0)
		PrintDebug( "port is 0");

	RTSP_LIB_SetServerInfo(stream->serverinfo,url->psz_host,url->i_port,NULL,0,RTSP_PORT);

	rtsp_ctrl->server = NULL;
	rtsp_ctrl->cseq = 1;
	rtsp_ctrl->server_state = 0;
	rtsp_ctrl->server_caps  = 0;
	rtsp_ctrl->session = NULL;
	rtsp_ctrl->get_media_packet = NULL;
	/* make mrl */
	STREAM_RTSP_NewRefinedUrl(rtsp_ctrl,stream->serverinfo->host,stream->serverinfo->port,url->psz_path);


	sock = RTSP_LIB_NetConnectTCP(stream->serverinfo->connect_host,stream->serverinfo->connect_port);
	if(sock < 0) { /* couldn't connect for some reason. (port closed maybe) */
		PrintError("Cannot Connect Socket to %s(%d)\n", stream->serverinfo->connect_host, stream->serverinfo->connect_port);
		goto failed;
	}


	stream->Channel->sock = sock; /* save socket id */
	PrintExit;
	return sock;

failed:
	PrintExit;
	return -1;
}




/*
 * make mrl from url and set to rtsp_ctrl->mrl
 * mrl is "rtsp://server.com:port/path/to/file.foo"
 * --IMPORTANT--
 * request URI must be complete according to RFC
 */
HINT32 STREAM_RTSP_NewRefinedUrl(struct rtsp_ctrl_t *rtsp_ctrl,char *host,HINT32 port,char *filepath)
{
	HINT32 len = 0;

	PrintEnter;

	len = VK_strlen(host) + VK_strlen(filepath) + 32; /* 32 is enough */

	rtsp_ctrl->mrl = (char *)DD_MEM_Alloc(len);
	VK_memset(rtsp_ctrl->mrl ,0, len);
	VK_snprintf(rtsp_ctrl->mrl,len,"rtsp://%s:%i%s",
		 host,port,filepath);
	PrintDebug("%s", rtsp_ctrl->mrl);
	PrintExit;
	return 1;
}



/*
 * parse rtsp response
 * return value:   1 ... success
 *				-1 ... parse error
 */
HINT32 STREAM_RTSP_ParseBasicFieldsInRtspHeader(struct rtsp_ctrl_t *rtsp_ctrl,
			struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	char *field;
	HINT32 answer_cseq;

	PrintEnter;

	if((field = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Alert")) != NULL) {
		PrintCriticalDebug("*** Alert ***\n%s\n",field);
	}


	if((field = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Cseq")) != NULL) {
		VK_sscanf(field,"%d",&answer_cseq);
		if(answer_cseq != (HINT32)rtsp_ctrl->cseq - 1)
		{
			/* -1 because cseq is incremented in build_request */
			PrintDebug("warning: Cseq mismatch, got %u, expected %u\n",
					answer_cseq,rtsp_ctrl->cseq - 1);
		}
		rtsp_ctrl->uiLastAnswerCSeq = answer_cseq;
	}

	if((field = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Server")) != NULL) {
		while(*field == ' ') field++;
		if(rtsp_ctrl->server) { /* already has it */
			if(VK_strcmp(rtsp_ctrl->server,field)) { /* different server */
				PrintDebug("warning: got different server\n"
						"	old: %s\n"
						"	new: %s\n",rtsp_ctrl->server,field);
				DD_MEM_Free(rtsp_ctrl->server);
				rtsp_ctrl->server = NULL;
				rtsp_ctrl->server = VK_StrDup(field);
			}
		}
		else { /* not set yet */
			rtsp_ctrl->server = VK_StrDup(field);
		}
	}

	if((field = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Session")) != NULL) {
		while(*field == ' ') field++;
		if(rtsp_ctrl->session) {
			if(VK_strncmp(rtsp_ctrl->session,field,VK_strlen(rtsp_ctrl->session))) { /* different session */
				PrintDebug("warning: got different session\n"
						"	old: [%s]\n"
						"	new: [%s]\n",rtsp_ctrl->session,field);
				DD_MEM_Free(rtsp_ctrl->session);
				rtsp_ctrl->session = NULL;
				rtsp_ctrl->session = VK_StrDup(field);
			}
		}
		else
		{
			HUINT32 keepalive = 10; /* MS-RTSP SPEC : min keepalive interval */
			char *sub_ptr = VK_strstr(field,";timeout=");
			if(sub_ptr)
			{
				rtsp_ctrl->session = DD_MEM_Alloc(sub_ptr - field + 1);
				VK_memset(rtsp_ctrl->session ,0, sub_ptr - field + 1);
				VK_strncpy(rtsp_ctrl->session, field, sub_ptr - field);

				VK_sscanf(VK_strstr(sub_ptr, "=") + 1, "%u", &keepalive);
				PrintDebug("parsed keep alive time = %u\n", keepalive);
			}
			else
			{
				rtsp_ctrl->session = VK_StrDup(field);
			}

			rtsp_ctrl->keepalive_interval = keepalive;
		}
	}

	/* real only, ignore in other protocol... */
	if((field = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"RealChallenge1")) != NULL) {
		while(*field == ' ') field++;
		if(rtsp_ctrl->challenge) {
			if(VK_strcmp(rtsp_ctrl->challenge,field)) { /* different server */
				PrintDebug("warning: got different client challenge\n"
						"	old: %s\n"
						"	new: %s\n",rtsp_ctrl->challenge,field);
				DD_MEM_Free(rtsp_ctrl->challenge);
				rtsp_ctrl->challenge = NULL;
				rtsp_ctrl->challenge = VK_StrDup(field);
			}
		}
		else {
			rtsp_ctrl->challenge = VK_StrDup(field);
		}
	}

	PrintExit;
	return 1;
}



/*
 * parsing rtsp response header
 */
HINT32 STREAM_RTSP_DeserializeRtspResponse(struct RTSP_PROT_HEADER_s *rtsp_hdr)
{

	char *field = NULL;
	char *hdr_ptr		= NULL;
	char *str_ptr		= NULL;

	HINT32 nPosHeaderSep, nHdrSepLen;
	size_t length;

	PrintEnter;

	if(rtsp_hdr == NULL)
	{
		return -1;
	}
	if(rtsp_hdr->is_parsed)
	{
		return 0;
	}

	hdr_ptr = VK_strstr(rtsp_hdr->buffer, " ");

	if(hdr_ptr == NULL)
	{
		PrintCriticalDebug("STREAM_RTSP_DeserializeRtspResponse Malformed answer : No %20 separator\n");
		return -1;
	}

	length = hdr_ptr - rtsp_hdr->buffer;
	rtsp_hdr->protocol = DD_MEM_Alloc(length + 1);
	VK_memset(rtsp_hdr->protocol ,0, length + 1);
	VK_strncpy(rtsp_hdr->protocol,rtsp_hdr->buffer,length);
	rtsp_hdr->protocol[length] = '\0';

	rtsp_hdr->method = VK_StrDup(rtsp_hdr->protocol);

	if(!VK_strncasecmp(rtsp_hdr->protocol,"RTSP",4)) { /* RTSP message*/

		/* RTSP status code */
#if 0 /* format '%d' expects type 'int *', but argument 3 has type 'HUINT32 *' */
		if(VK_sscanf(++hdr_ptr,"%d",&(rtsp_hdr->status_code)) != 1)
#else
		if(VK_sscanf(++hdr_ptr,"%d",(int*)&(rtsp_hdr->status_code)) != 1)  // <--
#endif
		{
			PrintCriticalDebug("STREAM_RTSP_DeserializeRtspResponse Malformed answer : No rtsp status code!!\n");
			return -1;
		}

		hdr_ptr += RTSP_HEADER_LEN;

		str_ptr = VK_strstr(hdr_ptr,"\n");
		if(str_ptr == NULL)
		{
			PrintCriticalDebug("STREAM_RTSP_DeserializeRtspResponse Malformed answer : unable to get reason_phrase\n");
			return -1;
		}

		length = str_ptr - hdr_ptr;
		rtsp_hdr->reason_phrase = DD_MEM_Alloc(length + 1);
		VK_memset(rtsp_hdr->reason_phrase ,0, length + 1);
		VK_strncpy(rtsp_hdr->reason_phrase,hdr_ptr,length);
		if(rtsp_hdr->reason_phrase[length - 1] == '\r')
		{
			length --;
		}
		rtsp_hdr->reason_phrase[length] = '\0';
	}
	else if(!VK_strncasecmp(rtsp_hdr->protocol, RTSP_PROTOCOL_TYPE_ANNC, VK_strlen(RTSP_PROTOCOL_TYPE_ANNC)))
	{
		rtsp_hdr->status_code = RTSP_STATUS_ANNOUNCE;
	}
	else if(!VK_strncasecmp(rtsp_hdr->protocol, RTSP_PROTOCOL_TYPE_OPTN, VK_strlen(RTSP_PROTOCOL_TYPE_OPTN)))
	{

		rtsp_hdr->status_code = RTSP_STATUS_OPTIONS;
	}
	else if(!VK_strncasecmp(rtsp_hdr->protocol, RTSP_PROTOCOL_TYPE_PARM, VK_strlen(RTSP_PROTOCOL_TYPE_PARM)))
	{
		rtsp_hdr->status_code = RTSP_STATUS_SET_PARAMETER;
	}
	else
	{
		PrintCriticalDebug("STREAM_RTSP_DeserializeRtspResponse error : Protocol not RTSP '%s'",rtsp_hdr->protocol);
		return -1;
	}

	nHdrSepLen = RTSP_HEADER_SEPARATOR_LEN; /* header separator length */
	str_ptr = VK_strstr(rtsp_hdr->buffer,"\r\n\r\n");
	if(!str_ptr)
	{
		str_ptr = VK_strstr(rtsp_hdr->buffer,"\n\n");
		if(!str_ptr)
		{
			PrintCriticalDebug("STREAM_RTSP_DeserializeRtspResponse error : Header Incompleted\n");
			return -1;
		}
		nHdrSepLen = 2;
	}
	nPosHeaderSep = str_ptr - rtsp_hdr->buffer;
	hdr_ptr = VK_strstr(rtsp_hdr->buffer,"\n") + 1;

	do {
		str_ptr = hdr_ptr;
		while(*str_ptr != '\r' && *str_ptr != '\n') str_ptr++;
		length  = str_ptr - hdr_ptr;

		if(length == 0) break;

		field = (char *)RTSP_LIB_Realloc(field,length + 1);
		VK_strncpy(field,hdr_ptr,length);
		field[length] = '\0';


		STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,field);
		hdr_ptr = str_ptr + ((*str_ptr == '\r') ? 2 : 1);
	} while(hdr_ptr < (rtsp_hdr->buffer + nPosHeaderSep));

	if(field)
	{
		DD_MEM_Free(field);
		field = NULL;
	}

	if(nPosHeaderSep + nHdrSepLen < (HINT32)rtsp_hdr->buffer_len) {
		rtsp_hdr->body = rtsp_hdr->buffer + (nPosHeaderSep + nHdrSepLen);
		rtsp_hdr->body_len = rtsp_hdr->buffer_len - (nPosHeaderSep + nHdrSepLen);
	}

	/* mark parsed */
	rtsp_hdr->is_parsed = 1;

	PrintExit;
	return 1;
}



#if PRE_DETECT_SERVER_TYPE
/*
 * send OPTIONS request, this is used as very first trial to detect server type is MS-media server or Real(Helix)-media server
 * we will need rtsp_hdr later, so return that.
 * return value:   -1: failure   status_code: success, rtsp_hdr_ret(malloc)
 */
static int P_STREAM_RTSP_DetectServerType(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s **rtsp_hdr_ret)
{
    int ret = 0;
    struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->protocol.rtsp_ctrl;
    struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
    char *options_uri = NULL;
    int options_uri_len = 0;

    /* default is rtsp-real (becasue OPTIONS req is supported) */
    rtsp_hdr = STREAM_RTSP_NewRtspHeaderAndFillBasicFields(rtsp_ctrl);

    STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,stream_rtsp_real_useragent);
    STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,"GUID: 00000000-0000-0000-0000-000000000000");
    STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,stream_rtsp_real_clientid);
    STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,"Pragma: initiate-session");
    STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,"RegionData: 0");
    STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,stream_rtsp_real_clientchallenge);
    STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,stream_rtsp_real_companyid);
    STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,stream_rtsp_real_playerstarttime);


    options_uri_len = VK_strlen(stream->serverinfo->host) + 256;
    options_uri = (char *)DD_MEM_Alloc(options_uri_len);
    if(options_uri == NULL)
    {
    	PrintError(" mem alloc error \n");
    	goto failed;
    }
	VK_memset(options_uri, 0, options_uri_len);
    VK_snprintf(options_uri,options_uri_len,"rtsp://%s:%i",
	     stream->serverinfo->host,stream->serverinfo->port);

    //rtsp_request_options(rtsp_hdr,options_uri);
    STREAM_RTSP_SetProtoclCmdForRtspHeader(rtsp_hdr,STREAM_RTSP_CMD_OPTION);
	STREAM_RTSP_SetUriForRtspHeader(rtsp_hdr,rtsp_ctrl->mrl);
	STREAM_RTSP_SerializeRtspHeader(rtsp_hdr);

    STREAM_RTSP_SendSerializedRtspRequestAndFree(stream,rtsp_hdr);

    rtsp_hdr = STREAM_RTSP_NewRtspProtocolHeader();
    if(rtsp_hdr == NULL)
    {
    	PrintError(" STREAM_RTSP_NewRtspProtocolHeader failed\n");
    	goto failed;
    }
    ret = STREAM_RTSP_ReadRtspHeaderAndParseBasic(stream,rtsp_hdr);
    if(ret < 0) {
	goto failed;
    }

    *rtsp_hdr_ret = rtsp_hdr;

    if(options_uri) VK_free(options_uri);
    return ret;

  failed:
    if(rtsp_hdr)    STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);
    if(options_uri) VK_free(options_uri);
    *rtsp_hdr_ret = NULL;
    return -1;
}
#endif /* PRE_DETECT_SERVER_TYPE */


/*
 * start rtsp streaming.
 *
 *	return value :   negative or 0  ... error
 *								 1  ... success
 */
HINT32 STREAM_RTSP_OpenRtspConnection(struct RTSP_ACCESS_SYS_s *stream)
{
#if 0 /* warning: unused variable */
	HINT32 header_size;
#endif
	char *server_type_name = NULL;
	char *redirected = NULL;

	struct RTSP_PROT_HEADER_s *rtsp_server_query_answer = NULL;

	PrintEnter;

	stream->stream_ctrl->status = RTSP_STREAMING_HANDSHAKING;
	stream->stream_ctrl->last_packet_status = LAST_PACKET_NOTARRIVED;


 	{

	/* try rtsp-wms */
	ASF_HEADER_t *pAsfHeaderInfo = NULL;
	HINT32 ret = 0;




#if PRE_DETECT_SERVER_TYPE
	/* re-establish rtsp connection for WMServer */
	stream->Channel->sock = STREAM_RTSP_Connect(stream);
	if(stream->Channel->sock < 0) {
		/*
		  couldn't connect for some reason.
		  (rtsp port probably closed)
		 */
		PrintCriticalDebug("rtsp-wms connection not established\n");
		PrintCriticalDebug("server probably does not accept rtsp retry using mmst protocol\n");

		stream->stream_ctrl->status = STREAMING_OTHER_PROTOCOL;
		goto failed;
	}

	{
		char *answer = NULL; /* temporary pointer */
		/* detect server type */
		P_STREAM_RTSP_DetectServerType(stream, &rtsp_server_query_answer);

		if((answer = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_server_query_answer,"Server")) != NULL)
		{
	    		while(*answer == ' ') answer++;  /* skip space */
	    		server_type_name = strdup(answer);
		}
		else
		{
	    		if((STREAM_RTSP_FindFieldFromRtspHeader(rtsp_server_query_answer,"RealChallenge1")) != NULL)
			{
				server_type_name = strdup("Real");
	    		}
	    		else
			{
				server_type_name = strdup("Unknown Server");
	    		}
		}

		if(server_type_name == NULL)
		{
			PrintError("programming error server_type_name must not be null\n");
			goto failed;
		}
		PrintDebug("Current Server Type is %s\n", server_type_name);

		if(!VK_strstr(server_type_name,"WMServer"))
		{
			PrintError("Not yet supported server type (%s)   !!!\n", server_type_name);
			goto failed;
		}
	}

	/* Clean up first. */
	if(stream->Channel->sock) {
		close(stream->Channel->sock);
	}

	/* free rtsp_ctrl_t */
	STREAM_RTSP_FreeRtspCtrl(stream->stream_ctrl->protocol.rtsp_ctrl);
	stream->stream_ctrl->protocol.rtsp_ctrl = STREAM_RTSP_NewRtspCtrl();

	/* free RTSP_SERVER_INFO_s */
	RTSP_LIB_FreeServerInfo(stream->serverinfo);
	stream->serverinfo = RTSP_LIB_NewServerInfo();

	/* DO NOT MESS WITH
	 * RTSP_ACCESS_SYS_s->Channel,
	 * RTSP_ACCESS_SYS_s->url,
	 * RTSP_ACCESS_SYS_s->dlopts,
	 * these are still valid
	 */
#endif

	/* re-establish rtsp connection for WMServer */
	stream->Channel->sock = STREAM_RTSP_Connect(stream);
	if(stream->Channel->sock < 0) {
		/*
		  couldn't connect for some reason.
		  (rtsp port probably closed)
		 */
		PrintCriticalDebug("rtsp-wms connection not established\n");
		PrintCriticalDebug("server probably does not accept rtsp retry using mmst protocol\n");

		stream->stream_ctrl->status = RTSP_STREAMING_OTHER_PROTOCOL;
		goto failed;
	}



	/* Describe, Parse Session Description(SDP), SETUP */
	ret = P_RTSP_MS_Setup_and_get_header(stream,&pAsfHeaderInfo);
	if(ret == 0) { /* retry	*/
		/* probably non-wmserver */
		PrintCriticalDebug("server probably does not accept rtsp\n"
			"retry using mmst protocol\n");
		stream->stream_ctrl->status = RTSP_STREAMING_OTHER_PROTOCOL;
		goto failed;
	}
	else if((!pAsfHeaderInfo) || (ret < 0)) {	   /* no retry */
		PrintCriticalDebug("rtsp setup failed\n");
		goto failed;
	}


	/* set asf info from asf_header */
	stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo = pAsfHeaderInfo;
	stream->stream_ctrl->packet_length = pAsfHeaderInfo->i_max_data_packet_size;
	stream->stream_ctrl->file_size = pAsfHeaderInfo->i_file_size;
	stream->stream_ctrl->total_packets = pAsfHeaderInfo->i_data_packets_count;

	stream->stream_ctrl->protocol.rtsp_ctrl->rtsp_protocol = RTSP_WMS_PROTOCOL;
	}


	if(server_type_name)
	{
		DD_MEM_Free(server_type_name);
		server_type_name = NULL;
	}
	if(rtsp_server_query_answer)
	{
		STREAM_RTSP_FreeRtspProtoclHeader(rtsp_server_query_answer);
	}
	if(redirected)
	{
		DD_MEM_Free(redirected);
		redirected = NULL;
	}

	//stream->stream_ctrl->status = STREAMING_DOWNLOADING;
	PrintExit;
	return 1; /* success */

  failed:
	 if(server_type_name)
	{
		DD_MEM_Free(server_type_name);
		server_type_name = NULL;
	}
	if(rtsp_server_query_answer)
	{
		STREAM_RTSP_FreeRtspProtoclHeader(rtsp_server_query_answer);
	}
	if(redirected)
	{
		DD_MEM_Free(redirected);
		redirected = NULL;
	}

	PrintExit;
	return -1;
}




/*
 * STREAM_RTSP_NewRtspProtocolHeader : create new RTSP header
 */
struct RTSP_PROT_HEADER_s *STREAM_RTSP_NewRtspProtocolHeader(void)
{
	struct RTSP_PROT_HEADER_s *hdr;
	PrintEnter;
	hdr = (struct RTSP_PROT_HEADER_s *)DD_MEM_Alloc(sizeof(struct RTSP_PROT_HEADER_s));
	VK_memset(hdr ,0, sizeof(struct RTSP_PROT_HEADER_s));
	PrintExit;
	return hdr;
}



/*
 * free RTSP header
 */
void STREAM_RTSP_FreeRtspProtoclHeader(struct RTSP_PROT_HEADER_s *h)
{
	struct rtsp_field_t *field, *prev;

	PrintEnter;

	if(h == NULL) return; // rtsp_hdr is not malloc()ed yet.

	if(h->protocol)
	{
		DD_MEM_Free(h->protocol);
		h->protocol = NULL;
	}
	if(h->method)
	{
		DD_MEM_Free(h->method);
		h->method = NULL;
	}
	if(h->uri)
	{
		DD_MEM_Free(h->uri);
		h->uri = NULL;
	}
	if(h->reason_phrase)
	{
		DD_MEM_Free(h->reason_phrase);
		h->reason_phrase = NULL;
	}
	if(h->field_search)
	{
		DD_MEM_Free(h->field_search);
		h->field_search = NULL;
	}
	if(h->buffer)
	{
		DD_MEM_Free(h->buffer);
		h->buffer = NULL;
	}

	field = h->first_field;
	for(field = h->first_field; field ; ) {

	if(field->field_name)
	{
		DD_MEM_Free(field->field_name);
		field->field_name = NULL;
	}

	prev = field;
	field = field->next;
	DD_MEM_Free(prev);
	prev = NULL;
	}

	DD_MEM_Free(h);
	h = NULL;

	PrintExit;
}



/*
 * returns first filed with 'field name'.
 * STREAM_RTSP_GetNextField will get next field with 'filed_name'
 */
char *STREAM_RTSP_FindFieldFromRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *pszQuery)
{
	char 	*str_ptr;
	struct 	rtsp_field_t *rtsp_field;

	PrintEnter;

	if(!rtsp_hdr || !pszQuery) return NULL;

	rtsp_hdr->field_search_pos = rtsp_hdr->first_field;
	/* warning: cast discards qualifiers from pointer target type */
	/* VK_strlen() -> VK_strlen() */
	rtsp_hdr->field_search = (char *)RTSP_LIB_Realloc(rtsp_hdr->field_search, VK_strlen(pszQuery) + 1);
	/* copy field name first. */
	VK_strncpy(rtsp_hdr->field_search, pszQuery , VK_strlen(pszQuery)); /* jhchoi2 : fixed maxdome rtsp issue */

	rtsp_field = rtsp_hdr->field_search_pos;

	while(rtsp_field)
	{
		str_ptr = VK_strstr(rtsp_field->field_name,":");
		if(str_ptr == NULL)
		{
			return NULL;
		}
		if(!VK_strncasecmp(rtsp_field->field_name,rtsp_hdr->field_search,
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
	PrintExit;
	return NULL; // NOT FOUND
}



/*
 * STREAM_RTSP_SetFieldForRtspHeader : make new field and attach it to last_field->next.
 */
void STREAM_RTSP_SetFieldForRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *field_name)
{
	struct rtsp_field_t *new_field;

	PrintEnter;

	if(rtsp_hdr == NULL || field_name == NULL) return;

	new_field = DD_MEM_Alloc(sizeof(struct rtsp_field_t));
	VK_memset(new_field ,0, sizeof(struct rtsp_field_t));

	new_field->next = NULL;
	/* warning: cast discards qualifiers from pointer target type */
	/* VK_strlen() -> VK_strlen() */
	new_field->field_name = DD_MEM_Alloc (VK_strlen(field_name) + 1);
	VK_memset(new_field->field_name ,0, VK_strlen(field_name) + 1);
	VK_strncpy(new_field->field_name, field_name, VK_strlen(field_name)); /* jhchoi2 : fixed maxdome rtsp issue */

	if(rtsp_hdr->last_field == NULL) {
		rtsp_hdr->first_field = new_field; // this was first filed!!
	}
	else {
		rtsp_hdr->last_field->next = new_field; // attach to last.
	}
	rtsp_hdr->last_field = new_field;
	rtsp_hdr->field_nb++;
	PrintExit;
}



void STREAM_RTSP_SetUriForRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *uri)
{
	PrintEnter;
	if(rtsp_hdr == NULL || uri == NULL) return;

	/* warning: cast discards qualifiers from pointer target type */
	/* VK_strlen() -> VK_strlen() */
	rtsp_hdr->uri = DD_MEM_Alloc(VK_strlen(uri) + 1);
	VK_memset(rtsp_hdr->uri ,0, VK_strlen(uri) + 1);
	VK_strncpy(rtsp_hdr->uri, uri,VK_strlen(uri)); /* jhchoi2 : fixed maxdome rtsp issue */
	PrintExit;
}



void STREAM_RTSP_SetProtoclCmdForRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *method)
{
	PrintEnter;
	if(rtsp_hdr == NULL || method == NULL) return;

	/* warning: cast discards qualifiers from pointer target type */
	/* VK_strlen() -> VK_strlen() */
	rtsp_hdr->method = DD_MEM_Alloc(VK_strlen(method) + 1);
	VK_memset(rtsp_hdr->method, 0, VK_strlen(method) + 1);
	VK_strncpy(rtsp_hdr->method, method, VK_strlen(method)); /* jhchoi2 : fixed maxdome rtsp issue */
	PrintExit;
}



/*
 * wrappers for each request
 */
HINT32 STREAM_RTSP_SendRtspOk(struct RTSP_ACCESS_SYS_s *stream,HINT32 cseq,char *session)
{
	HINT32 ret;
	HINT32 buflen = 100 + VK_strlen((char*)&rtsp_protocol_version) + VK_strlen(session);
	char *buf;
	PrintEnter;

	buf = DD_MEM_Alloc(buflen + 1);
	VK_memset(buf ,0, buflen + 1);

	VK_snprintf(buf,buflen,
		 "%s 200 OK\r\n"
		 "Cseq: %u\r\n"
		 "Session: %s\r\n"
		 "\r\n",
		 rtsp_protocol_version,
		 cseq,
		 session);

	ret = RTSP_LIB_SockWrite(stream->Channel->sock,buf,VK_strlen(buf));
	PrintDebug("=send 200 OK============\n%s\n=================\n",buf);

	DD_MEM_Free(buf);
	buf = NULL;

	PrintExit;
	return ret;
}





#if 0
char *rtsp_request_options(struct RTSP_PROT_HEADER_s *rtsp_hdr,char *request_uri)
{
	STREAM_RTSP_SetProtoclCmdForRtspHeader(rtsp_hdr,STREAM_RTSP_CMD_OPTION);
	STREAM_RTSP_SetUriForRtspHeader(rtsp_hdr,request_uri);
	return (STREAM_RTSP_SerializeRtspHeader(rtsp_hdr));
}
#endif











/*
 * make complete RTSP_PROT_HEADER_s.
 * firstline is first line, such as OPTIONS, DESCRIBE, etc... (see rtsp)
 * the request string goes to rtsp_hdr->buffer
 */
char *STREAM_RTSP_SerializeRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	char *ptr;
	HINT32 length,length2 = 0;
	struct rtsp_field_t *field;

	PrintEnter;

	if(!rtsp_hdr)
	{
	 	return NULL;
	}
	if(!rtsp_hdr->method)
	{
		return NULL;
	}
	if(!rtsp_hdr->uri)
	{
		return NULL;
	}

	length = VK_strlen(rtsp_hdr->method) + VK_strlen(rtsp_hdr->uri) +
				VK_strlen((char*)&rtsp_protocol_version) + 2 + 2 ;
	field = rtsp_hdr->first_field;

	while(field)
	{
		length += VK_strlen(field->field_name) + 2;
		field = field->next;
	}

	length += 2;

	if(rtsp_hdr->body)
	{
		length += rtsp_hdr->body_len;
	}

	if(rtsp_hdr->buffer)
	{
		DD_MEM_Free(rtsp_hdr->buffer);
		rtsp_hdr->buffer = NULL;
	}

	if(rtsp_hdr->body_len)
	{
		length += rtsp_hdr->body_len;
	}

	rtsp_hdr->buffer_len = length;
	rtsp_hdr->buffer = DD_MEM_Alloc(length + 1);
	VK_memset(rtsp_hdr->buffer ,0, length + 1);

	ptr = rtsp_hdr->buffer;
#if 0 /* warning: cast discards qualifiers from pointer target type */
	len2 = VK_strlen(rtsp_hdr->method)+VK_strlen(rtsp_hdr->uri)+VK_strlen(rtsp_protocol_version);
#else
	length2 = VK_strlen(rtsp_hdr->method)+VK_strlen(rtsp_hdr->uri)+VK_strlen((char*)&rtsp_protocol_version);
#endif
	ptr += VK_snprintf(ptr,length2+5,"%s %s %s\r\n",rtsp_hdr->method,rtsp_hdr->uri, rtsp_protocol_version);

	field = rtsp_hdr->first_field;

	while(field != NULL)
	{
		length2 = VK_strlen(field->field_name);
		ptr += VK_snprintf(ptr,length2+3,"%s\r\n",field->field_name);
		field = field->next;
	}
	ptr += VK_snprintf(ptr,3,"\r\n");

	if(rtsp_hdr->body != NULL) {
		VK_memcpy(ptr,rtsp_hdr->body,rtsp_hdr->body_len);
	}

	PrintExit;
	return rtsp_hdr->buffer;
}



/*
 * judges if rtsp_hdr is complete RTSP header, or still needs some parts.
 *	return value :	   0 ... NOT complete
 *						 1 ... COMPLETE!!
 */
HINT32 STREAM_RTSP_IsEndOfRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr)
{
	PrintEnter;
	if(rtsp_hdr == NULL) return 0; /* unlikely, but error */

	if(rtsp_hdr->buffer == NULL) return 0;   /* nothing received. */

	if(VK_strstr(rtsp_hdr->buffer,"\r\n\r\n") ||
	   VK_strstr(rtsp_hdr->buffer,"\n\n"))
	   {
	   PrintExit;
	return 1;
	}

  	PrintExit;
	return 0;
}


static struct rtsp_ctrl_t *STREAM_RTSP_NewRtspCtrl(void)
{
	struct rtsp_ctrl_t *ctrlt =
	(struct rtsp_ctrl_t *)DD_MEM_Alloc(sizeof(struct rtsp_ctrl_t));
	PrintEnter;
	VK_memset(ctrlt ,0, sizeof(struct rtsp_ctrl_t));

	/* allocate other protocol controllers in
	   each protocol initiation functions,
	   such as rmff_header or pAsfHeaderInfo...
	*/

	ctrlt->prev_ts = -1;
	ctrlt->prev_stream_number = -1;
	ctrlt->rtsp_protocol = RTSP_UNKNOWN_PROTOCOL; /* off course unknown by default */

	PrintExit;
	return ctrlt;
}


static void STREAM_RTSP_FreeRtspCtrl(struct rtsp_ctrl_t *ctrlt)
{
	PrintEnter;

	if(ctrlt == NULL)
	{
		return;
	}

	if(ctrlt->rtsp_protocol == RTSP_REAL_PROTOCOL) {
		//if(ctrlt->rmff_header) {
			;//FUTURE REAL-RTSP //free_rmff_header_t(ctrlt->rmff_header);
		//}
	}
	else if(ctrlt->rtsp_protocol == RTSP_WMS_PROTOCOL) {
		if(ctrlt->asf.pAsfHeaderInfo) {
			free_pAsfHeaderInfo_t(ctrlt->asf.pAsfHeaderInfo);
		}
	}

	if(ctrlt->mrl)
	{
		DD_MEM_Free(ctrlt->mrl);
		ctrlt->mrl = NULL;
	}
	if(ctrlt->server)
	{
		DD_MEM_Free(ctrlt->server);
		ctrlt->server = NULL;
	}
	if(ctrlt->session)
	{
		DD_MEM_Free(ctrlt->session);
		ctrlt->session = NULL;
	}
	if(ctrlt->etag)
	{
		DD_MEM_Free(ctrlt->etag);
		ctrlt->etag = NULL;
	}
	if(ctrlt->challenge)
	{
		DD_MEM_Free(ctrlt->challenge);
		ctrlt->challenge = NULL;
	}

	DD_MEM_Free(ctrlt);
	ctrlt = NULL;

	PrintExit;
}



struct RTSP_ACCESS_SYS_s *STREAM_RTSP_NewRtspAccessSysAndRtspCtrl(void)
{
	struct RTSP_ACCESS_SYS_s *stream = NULL;


	PrintEnter;

	stream = RTSP_LIB_InitRtspAccessSys();
	stream->stream_ctrl->protocol.rtsp_ctrl = STREAM_RTSP_NewRtspCtrl();

	PrintExit;

	return stream;
}



void STREAM_RTSP_SendTeardownAndFreeRtspAccessSys(struct RTSP_ACCESS_SYS_s *stream)
{
	PrintEnter;
	if(stream == NULL)
	{
		PrintError("Arg is Null %s(%d)\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	if(stream->stream_ctrl == NULL)
	{
		PrintError("Arg is Null %s(%d)\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	if(stream->stream_ctrl->protocol.rtsp_ctrl == NULL)
	{
		PrintError("Arg is Null %s(%d)\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	/* TEARDOWN */
	if(stream->stream_ctrl->status == RTSP_STREAMING_DOWNLOADING ||
	   stream->stream_ctrl->status == RTSP_STREAMING_FINISHED)
	{
		struct RTSP_PROT_HEADER_s *rtsp_hdr;
		HINT32 ret;
		/* send teardown request to make server happier. */
		rtsp_hdr = STREAM_RTSP_NewRtspHeaderAndFillBasicFields(stream->stream_ctrl->protocol.rtsp_ctrl);
		//rtsp_request_teardown(rtsp_hdr,stream->stream_ctrl->protocol.rtsp_ctrl->mrl);
		STREAM_RTSP_SetProtoclCmdForRtspHeader(rtsp_hdr,STREAM_RTSP_CMD_TEARDOWN);
		STREAM_RTSP_SetUriForRtspHeader(rtsp_hdr,stream->stream_ctrl->protocol.rtsp_ctrl->mrl);
		STREAM_RTSP_SerializeRtspHeader(rtsp_hdr);

		ret = STREAM_RTSP_SendSerializedRtspRequestAndFree(stream,rtsp_hdr);
	}

	if(stream->Channel)
	{
		if(stream->Channel->sock > 0)
		{ /* valid socket --> close */
			close(stream->Channel->sock);
			stream->Channel->sock = 0; /*NULL;*/
		}
	}

	if(stream->datasocket[2])
	{
		if(stream->datasocket[2]->sock > 0)
		{
			close(stream->datasocket[2]->sock);
			stream->datasocket[2]->sock = 0; /*NULL;*/
		}
	}

	if(stream->stream_ctrl->protocol.rtsp_ctrl)
	{
		STREAM_RTSP_FreeRtspCtrl(stream->stream_ctrl->protocol.rtsp_ctrl);
		stream->stream_ctrl->protocol.rtsp_ctrl = NULL;
	}

	if(stream->url)
	{
			UTIL_UrlClean( stream->url );
			DD_MEM_Free(stream->url);
			stream->url = NULL;
	}
	if(stream->dlopts)
	{
		STREAM_RTSP_FreeRtspOption(stream->dlopts);
		stream->dlopts = NULL;
	}

	RTSP_LIB_FreeRtspAccessSys(stream);
	stream = NULL;
failed:
;
	PrintExit;
}






struct download_opts_t *STREAM_RTSP_NewRtspOption(void)
{
	struct download_opts_t *dlopts;
	PrintEnter;

	dlopts = DD_MEM_Alloc(sizeof(struct download_opts_t));
	VK_memset(dlopts,0,sizeof(struct download_opts_t));

	PrintExit;
	return dlopts;
}



void STREAM_RTSP_FreeRtspOption(struct download_opts_t *dlopts)
{
	PrintEnter;
	if(!dlopts) {
	return;
	}

	if(dlopts->speed)
	{
		DD_MEM_Free(dlopts->speed);
		dlopts->speed = NULL;
	}
	if(dlopts->range)
	{
		DD_MEM_Free(dlopts->range);
		dlopts->range = NULL;
	}
	if(dlopts->byterange)
	{
		DD_MEM_Free(dlopts->byterange);
		dlopts->byterange = NULL;
	}
	if(dlopts->username)
	{
		DD_MEM_Free(dlopts->username);
		dlopts->username = NULL;
	}
	if(dlopts->password)
	{
		DD_MEM_Free(dlopts->password);
		dlopts->password = NULL;
	}

	DD_MEM_Free(dlopts);
	dlopts = NULL;

	PrintExit;
}



void STREAM_RTSP_CloseRtspConnection(struct RTSP_ACCESS_SYS_s *p_rtsp_stream)
{
	HULONG lock_read;
	HULONG lock_write;
	HULONG lock_sync;

	PrintEnter;
	if(p_rtsp_stream == NULL)
	{
		PrintError("NULL arg\n");
		return;
	}

	lock_read = p_rtsp_stream->lock_read;
	lock_write = p_rtsp_stream->lock_write;
	lock_sync = p_rtsp_stream->lock_sync;

	/* release resource */
	if( p_rtsp_stream->p_keepalive_thread)
	{
		PrintDebug("==========STOP KEEPALIVE THREAD================\n");
		(void)VK_TASK_Sleep (RTSP_TASK_START_WATING_TIME); /* wait for starting the task */

		VK_TASK_Stop(p_rtsp_stream->p_keepalive_thread->thread_id);
		VK_TASK_Destroy(p_rtsp_stream->p_keepalive_thread->thread_id);
		DD_MEM_Free(p_rtsp_stream->p_keepalive_thread);
		p_rtsp_stream->p_keepalive_thread = NULL;
	}


	/* terminate and release protocol internal resource */
	if(p_rtsp_stream)
	{
			STREAM_RTSP_SendTeardownAndFreeRtspAccessSys(p_rtsp_stream); /* FUTURE integrate STREAM_RTSP_CloseRtspConnection and rtsp_streming_close */
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
	/* RTSP Data */
	URL_t *url = NULL;
	struct download_opts_t *dlopts = NULL;
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream = NULL;

	/* ret val */
	HINT32 rtsp_ret = 0;
	HINT32 nVKRet = 0; /* for virtaul kernal */

	PrintEnter;

	if(pAccess == NULL)
	{
		PrintError("Null Ptr Error %s(%d)\n", __FUNCTION__, __LINE__);
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
	if(pAccess->b_openfromnexus == TRUE)
	{
		g_reenter_counter = 0;

		g_fd = open ( "/backtrick.asf" , O_CREAT|O_TRUNC|O_WRONLY , 0666);
		if(g_fd == NULL)
		{
			PrintDebug ("=== open failed ===\n");
		}
		else
		{
			PrintDebug ("=== open success ===\n");
		}
	}
#endif



	/* parse path and make url and setup downloadopt */
	url = DD_MEM_Alloc(sizeof(URL_t));
	if(url == NULL)
	{
		PrintError("mem alloc err\n");
		goto failed;
	}
	VK_memset(url ,0, sizeof(URL_t));
	UTIL_UrlParse(url, pAccess->psz_path, 0 );

	/* check if target url is valid */
	if(url == NULL) {
		PrintError("error \"%s\"\n", pAccess->psz_path);
		goto failed;
	}
	if(!url->psz_host || !VK_strcmp(url->psz_host,"")) {
		PrintError("error \"%s\" (no host information)\n", pAccess->psz_path);
		goto failed;
	}
	if(!url->psz_path || !VK_strcmp(url->psz_path,"")) {
		PrintError("error: \"%s\" (no filepath)\n", pAccess->psz_path);
		goto failed;
	}


	/* set download options */
	dlopts = STREAM_RTSP_NewRtspOption();
	dlopts->dl_protocol = RTSP;
	dlopts->speed = VK_StrDup("8.252"); /* for variable rate video and support trick speed */
	dlopts->udp_port = 7002;
#ifdef USE_TRANSPORT_UDP
	dlopts->data_transport_protocol = RTSP_TRANSPORT_UDP_UNICAST;
#else
	dlopts->data_transport_protocol = RTSP_TRANSPORT_TCP;
#endif



	/* alloc RTSP_ACCESS_SYS_s */
	p_rtsp_stream = STREAM_RTSP_NewRtspAccessSysAndRtspCtrl(); /*stream->close(stream);*/
	p_rtsp_stream->p_keepalive_thread = NULL;
	p_rtsp_stream->current_trick_speed = 1;

	/* setup rtsp_stream */
	p_rtsp_stream->url = url;
	url = NULL;
		p_rtsp_stream->dlopts = dlopts;
#if 0 /* warning: passing argument 1 of 'VK_StrDup' discards qualifiers from pointer target type */
	p_rtsp_stream->localfile = VK_StrDup(default_file_name_for_empty);
#else
	p_rtsp_stream->localfile = VK_StrDup((char*)&default_file_name_for_empty);
#endif


	rtsp_ret = VK_SEM_Create(&p_rtsp_stream->lock_read, "rtspread", DEFAULT_SUSPEND_TYPE);
	if( rtsp_ret != VK_OK )
	{
		PrintError("Can't create SEM [%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, rtsp_ret);
		goto failed;
	}
	rtsp_ret = VK_SEM_Create(&p_rtsp_stream->lock_write, "rtspwrite", DEFAULT_SUSPEND_TYPE);
	if( rtsp_ret != VK_OK )
	{
		PrintError("Can't create SEM [%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, rtsp_ret);
		goto failed;
	}
	rtsp_ret = VK_SEM_Create(&p_rtsp_stream->lock_sync, "rtspsync", DEFAULT_SUSPEND_TYPE);
	if( rtsp_ret != VK_OK )
	{
		PrintError("Can't create SEM [%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, rtsp_ret);
		goto failed;
	}


	/* Describe, process SDP, SETUP */
	rtsp_ret = STREAM_RTSP_OpenRtspConnection(p_rtsp_stream);
	if(rtsp_ret <= 0) {
		goto failed;
	}

	/* setup pAccess */
	pAccess->p_sys = p_rtsp_stream;
	pAccess->info.i_size = p_rtsp_stream->stream_ctrl->file_size; /* from asf header */

	/* Control channel is TCP */
	p_rtsp_stream->Channel->is_udp= 0;



#ifdef USE_TRANSPORT_UDP
	 p_rtsp_stream->datasocket[2] = RTSP_LIB_NewChannel();
	 p_rtsp_stream->datasocket[2]->port = 7002;
	p_rtsp_stream->datasocket[2]->is_udp= 1;
	p_rtsp_stream->datasocket[2]->is_started = 0;
	p_rtsp_stream->datasocket[2]->prev_rtp_seq_num = 0;
	RTSP_LIB_ConnectUDPChannel(p_rtsp_stream->datasocket[2]);
	if (p_rtsp_stream->datasocket[2]->sock == -1)
	{
		   PrintDebug( "%s (%d) Error> cannot create socket \n",__FUNCTION__,__LINE__); /* %m ?? ISO C does not support the '%m' VK_printf format */
		goto failed;
	}
#endif


	if(pAccess->b_openfromnexus == TRUE)
	{
		/* Setup keep alive thread */
		p_rtsp_stream->p_keepalive_thread = (struct rtsp_keepalive_thread_t *)DD_MEM_Alloc(sizeof(struct rtsp_keepalive_thread_t));
		VK_memset(p_rtsp_stream->p_keepalive_thread ,0, sizeof(struct rtsp_keepalive_thread_t));

		p_rtsp_stream->p_keepalive_thread->p_stream = p_rtsp_stream;
		p_rtsp_stream->p_keepalive_thread->b_paused = FALSE;
		p_rtsp_stream->p_keepalive_thread->b_thread_error = FALSE;
		p_rtsp_stream->p_keepalive_thread->b_quit = FALSE;

		nVKRet = VK_TASK_Create ( P_STREAM_RTSP_KeepAliveTask
					, AUDIO_CLIP_TASK_PRIORITY
					, RTSP_STACK_SIZE
					, "P_STREAM_RTSP_KeepAliveTask"
					, (void *)p_rtsp_stream->p_keepalive_thread
					, &p_rtsp_stream->p_keepalive_thread->thread_id
					, FALSE); /* TRUE means JOINABLE */

		if ( VK_OK != nVKRet )
		{
			PrintCriticalDebug( "%s(%d) Error> KeepAliveThread task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
			p_rtsp_stream->p_keepalive_thread->b_thread_error = TRUE;
			return DI_IPT_ETHREAD;
		}

		(void)VK_TASK_Start(p_rtsp_stream->p_keepalive_thread->thread_id);
	}

	PrintDebug( "Rtsp open success!! %s (%d)   \n",__FUNCTION__,__LINE__);

	PrintExit;
	return DI_IPT_SUCCESS;


  failed:

	STREAM_RTSP_CloseRtspConnection(p_rtsp_stream);

	if(url)
	{
		UTIL_UrlClean( url );
		DD_MEM_Free(url);
		url = NULL;
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
	ssize_t ret = DI_IPT_SUCCESS;

	//PrintEnter;

	/* null check */
	if(pAccess == NULL)
	{
		PrintError("Null Ptr Error %s(%d)\n", __FUNCTION__, __LINE__);
		ret = DI_IPT_EGENERIC;
		goto genericfailed;
	}

	PrintDebug("current Start of Read Point is %"PRId64"\n", pAccess->info.i_pos);

	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;
	if(p_rtsp_stream == NULL)
	{
		PrintError("Null Ptr Error %s(%d)\n", __FUNCTION__, __LINE__);
		ret = DI_IPT_EGENERIC;
		goto genericfailed;
	}
	if(p_rtsp_stream->stream_ctrl == NULL)
	{
		PrintError("Null Ptr Error %s(%d)\n", __FUNCTION__, __LINE__);
		ret = DI_IPT_EGENERIC;
		goto genericfailed;
	}
	if(p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl == NULL)
	{
		PrintError("Null Ptr Error %s(%d)\n", __FUNCTION__, __LINE__);
		ret = DI_IPT_EGENERIC;
		goto genericfailed;
	}

	if(p_rtsp_stream->dlopts == NULL)
	{
		PrintError("Null Ptr Error %s(%d)\n", __FUNCTION__, __LINE__);
		ret = DI_IPT_EGENERIC;
		goto genericfailed;
	}

	if(pAccess->b_openfromnexus == FALSE && pAccess->info.i_pos == 0)
	{
		//inner_ret = P_STREAM_RTSP_ReadFromStreamBuffer(p_rtsp_stream, p_buffer + pos, requested_size - pos);

		VK_memcpy(p_buffer, p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo->pRawAsfHeader, p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo->nRawAsfHeaderLen);
		pos += p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo->nRawAsfHeaderLen;
		if(inner_ret < 0)
		{
			PrintDebug("P_STREAM_RTSP_ReadFromStreamBuffer failed\n");
			goto readfailed;
		}

		pos += inner_ret;

		PrintDebug("sizeof asf hdr pos = %d\n", (HINT32)pos);
		goto success;
	}

	/* feed ASF HDR if nessary */
	if(p_rtsp_stream->b_feed_asfheader_first == TRUE && pAccess->info.i_pos == 0)
	{
		PrintDebug("FEED ASF HEADER FIRST \n");
		VK_memcpy(p_buffer, p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo->pRawAsfHeader, p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo->nRawAsfHeaderLen);
		pos += p_rtsp_stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo->nRawAsfHeaderLen;

		/* flush patial packet */
		p_rtsp_stream->stream_ctrl->write_data_len = 0;
		p_rtsp_stream->stream_ctrl->write_pos = 0;

		p_rtsp_stream->b_feed_asfheader_first = FALSE;
		p_rtsp_stream->bDuringSeeking = FALSE;

#if STREAM_RECORD_FOR_DEBUG
		g_reenter_counter ++;
		if(g_reenter_counter == 3)
		{
			PrintDebug ("========\n============\nnow recording\n");
		}
#endif
	}

	/* Send Play if not started */
	inner_ret = RTSP_MS_StartDataChannelIfNotStarted(p_rtsp_stream);
	if(inner_ret < 0)
	{
		ret = DI_IPT_EGENERIC;
		goto genericfailed;
	}

	while(requested_size > pos) /* requested_size is nexus aligned size */
	{
		size_t recved_size = 0;

		if(p_rtsp_stream->bDuringSeeking == TRUE) /* wait for thread canceled by nexus seek(0) */
		{
			ret = DI_IPT_SUCCESS;
			goto success;
		}

		VK_SEM_Get(p_rtsp_stream->lock_read);

		/* 1st, read from buffer */
		inner_ret = P_STREAM_RTSP_ReadFromStreamBuffer(p_rtsp_stream, p_buffer + pos, requested_size - pos);
		if(inner_ret < 0)
		{
			PrintDebug("P_STREAM_RTSP_ReadFromStreamBuffer failed\n");
			goto readfailed;
		}

		recved_size += inner_ret;

		/* 2nd, parse CMD packet */
		if(p_rtsp_stream->stream_ctrl->status != RTSP_STREAMING_FINISHED &&  ret == 0)
		{
				if(p_rtsp_stream->dlopts->data_transport_protocol  ==  RTSP_TRANSPORT_TCP)
				{
					/* interleaved mode에서 control channel recevier */
					inner_ret = RTSP_MS_ProcessInterleavedPacket(p_rtsp_stream);
					if(inner_ret < 0)
					{
						PrintDebug("RTSP_MS_ProcessInterleavedPacket failed\n");
						goto readfailed;
					}
				}
		}

		/* 3rd, read an asf packet from socket. if the buffer size is lower than an asf packet size then leave rest part to stream buffer */
		if(p_rtsp_stream->stream_ctrl->status != RTSP_STREAMING_FINISHED && p_rtsp_stream->bInterleavedDataLefted == TRUE && recved_size == 0)
		{
			inner_ret = RTSP_MS_ProcessRtpPacket(p_rtsp_stream, p_buffer + pos, requested_size - pos);
				if(inner_ret < 0)
				{
					PrintDebug("RTSP_MS_ProcessRtpPacket failed\n");
					goto readfailed;
				}
				recved_size += inner_ret;
		}


readfailed:
		VK_SEM_Release(p_rtsp_stream->lock_read);
		pos += recved_size;

		/* check end of stream */
		if((p_rtsp_stream->stream_ctrl->last_packet_status == LAST_PACKET_ARRIVED || p_rtsp_stream->stream_ctrl->status == RTSP_STREAMING_FINISHED) && p_rtsp_stream->stream_ctrl->write_data_len == 0)
		{
				p_rtsp_stream->stream_ctrl->last_packet_status = LAST_PACKET_READ;
				pAccess->info.b_eof = TRUE;
				ret = DI_IPT_SUCCESS;
				goto success;
		}
		p_rtsp_stream->b_continuous_send = FALSE;

		if(inner_ret < 0) /* read failed */
		{
			ret = DI_IPT_EGENERIC;
			goto genericfailed;
		}
	}

success:
;

#if STREAM_RECORD_FOR_DEBUG
	if(g_fd)
	{
		if(g_reenter_counter == 3)
		{
			PrintDebug ("[x]");
			write (g_fd, p_buffer, pos);
		}
	}
#endif

genericfailed:
	i_data = pos;

	PrintDebug("%s(%d) : tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__,requested_size, pos);
	//PrintExit;

	if(ret == DI_IPT_SUCCESS)
	{
		return i_data;
	}
	else
	{
		return DI_IPT_EGENERIC;
	}
}

HINT32 MMSR_Seek ( STREAM_ACCESS_t * pAccess, HINT64 i_pos ){
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream;

	PrintEnter;
	if(pAccess == NULL)
	{
		PrintError("Null Ptr Error %s(%d)\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;

	//PrintDebug("current Seek Point is %"PRId64"\n", i_pos);

	pAccess->info.i_pos = i_pos;
	pAccess->info.b_eof = false;

	PrintExit;
	return DI_IPT_SUCCESS;
}

HINT32 MMSR_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2)
{
	static HINT32 rtsp_scale = 1;
	struct RTSP_ACCESS_SYS_s *p_rtsp_stream;
	HINT32 ret = DI_IPT_SUCCESS;
	PrintEnter;


	if(pAccess == NULL)
	{
		PrintError("Null Ptr Error %s(%d)\n", __FUNCTION__, __LINE__);
		return DI_IPT_EGENERIC;
	}
	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;

	switch (cmd)
	{
		case CMD_CONTROL_PAUSE :
			PrintCriticalDebug( "%s (%d) CMD_CONTROL_PAUSE \n",__FUNCTION__,__LINE__);
			/* In Interleaved mode, pause can be passive */
			break;

		case CMD_CONTROL_RESUME :
			PrintCriticalDebug("%s (%d) CMD_CONTROL_RESUME \n",__FUNCTION__,__LINE__);
			/* In Interleaved mode, resume can be passive */
			break;

		case CMD_CONTROL_SPEED :
			PrintCriticalDebug("%s (%d) CMD_CONTROL_SPEED (val=%d)\n",__FUNCTION__,__LINE__,(HINT32 )val);
			{

				/* convert DI_MEDIA_TrickSpeed_e to decimal rate)*/
				switch(val)
				{
					case DI_MEDIA_TRICK_PAUSE :
						PrintError("In TCP RTSP PASUE is handled by playback\n");
						rtsp_scale = 0;
						//P_STREAM_RTSP_PAUSE(p_rtsp_stream);
						//ret = DI_IPT_EGENERIC;
						ret = DI_IPT_SUCCESS;
						goto success;
						//goto failed;
						//break;
#if 0
					case DI_MEDIA_TRICK_1RWSTEP :
						rtsp_scale = -1;
						break;
					case DI_MEDIA_TRICK_2RWSTEP :
						rtsp_scale = -2;
						break;
					case DI_MEDIA_TRICK_3RWSTEP :
						rtsp_scale = -3;
						break;
					case DI_MEDIA_TRICK_4RWSTEP :
						rtsp_scale = -4;
						break;
					case DI_MEDIA_TRICK_5RWSTEP :
						rtsp_scale = -5;
						break;
					case DI_MEDIA_TRICK_6RWSTEP :
						rtsp_scale = -6;
						break;
					case DI_MEDIA_TRICK_7RWSTEP :
						rtsp_scale = -7;
						break;
					case DI_MEDIA_TRICK_8RWSTEP :
						rtsp_scale = -8;
						break;
					case DI_MEDIA_TRICK_9RWSTEP :
						rtsp_scale = -9;
						break;
					case DI_MEDIA_TRICK_10RWSTEP :
						rtsp_scale = -10;
						break;
					case DI_MEDIA_TRICK_11RWSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10RW instead\n");
						rtsp_scale = -10;
						break;
					case DI_MEDIA_TRICK_12RWSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10RW instead\n");
						rtsp_scale = -10;
						break;
					case DI_MEDIA_TRICK_13RWSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10RW instead\n");
						rtsp_scale = -10;
						break;
					case DI_MEDIA_TRICK_14RWSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10RW instead\n");
						rtsp_scale = -10;
						break;
					case DI_MEDIA_TRICK_15RWSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10RW instead\n");
						rtsp_scale = -10;
						break;
					case DI_MEDIA_TRICK_16RWSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10RW instead\n");
						rtsp_scale = -10;
						break;
#endif
					case DI_MEDIA_TRICK_1FFSTEP :
						rtsp_scale = 1;
						break;
					case DI_MEDIA_TRICK_2FFSTEP :
						rtsp_scale = 2;
						break;
					case DI_MEDIA_TRICK_3FFSTEP :
						rtsp_scale = 3;
						break;
					case DI_MEDIA_TRICK_4FFSTEP :
						rtsp_scale = 4;
						break;
					case DI_MEDIA_TRICK_5FFSTEP :
						rtsp_scale = 5;
						break;
					case DI_MEDIA_TRICK_6FFSTEP :
						rtsp_scale = 6;
						break;
					case DI_MEDIA_TRICK_7FFSTEP :
						rtsp_scale = 7;
						break;
					case DI_MEDIA_TRICK_8FFSTEP :
						rtsp_scale = 8;
						break;
					case DI_MEDIA_TRICK_9FFSTEP :
						rtsp_scale = 9;
						break;
					case DI_MEDIA_TRICK_10FFSTEP :
						rtsp_scale = 10;
						break;
					case DI_MEDIA_TRICK_11FFSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10FF instead\n");
						rtsp_scale = 10;
						break;
					case DI_MEDIA_TRICK_12FFSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10FF instead\n");
						rtsp_scale = 10;
						break;
					case DI_MEDIA_TRICK_13FFSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10FF instead\n");
						rtsp_scale = 10;
						break;
					case DI_MEDIA_TRICK_14FFSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10FF instead\n");
						rtsp_scale = 10;
						break;
					case DI_MEDIA_TRICK_15FFSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10FF instead\n");
						rtsp_scale = 10;
						break;
					case DI_MEDIA_TRICK_16FFSTEP :
						PrintError("MAXDOME WMS does not support this trick speed. using 10FF instead\n");
						rtsp_scale = 10;
						break;

					default:
						PrintError("this trick speed is not supported currently\n");
						ret = DI_ERR_INVALID_PARAM;
						goto failed;
				}
				if(p_rtsp_stream->current_trick_speed == rtsp_scale) /* skip same trick request */
				{
					PrintDebug("Ignore same speed of trick...\n");
					ret = DI_ERR_INVALID_PARAM;
					goto failed;
				}

				PrintDebug( "%s (%d) RTSP Play Scale also known as speed multiplier (X %d)\n",__FUNCTION__,__LINE__,rtsp_scale);

				if(val2 != 0)
				{
					val2 += 5;
				}
				ret = P_STREAM_RTSP_SendSeekWithTrickCommand(p_rtsp_stream, (HUINT64)val2, rtsp_scale);
				if(ret != DI_IPT_SUCCESS)
				{
					goto failed;
				}
				p_rtsp_stream->current_trick_speed = rtsp_scale;
			}
			break;

		case CMD_CONTROL_SEEK :
			PrintDebug("%s (%d) CMD_CONTROL_SEEK (val(position)=%lu) is supported !! \n",__FUNCTION__,__LINE__,(unsigned long)val);

			if(val != 0)
			{
				val += 5;
			}

			ret = P_STREAM_RTSP_SendSeekWithTrickCommand(p_rtsp_stream, val , 1);
			if(ret != DI_IPT_SUCCESS)
			{
				goto failed;
			}
			p_rtsp_stream->current_trick_speed = 1;
			break;

		default :
			PrintDebug( "%s (%d) Error> Unknown Command.(%d). \n",__FUNCTION__,__LINE__,cmd);
			ret =  DI_IPT_EGENERIC;
			goto failed;
			break;
	}

success:
;

failed:

	PrintExit;

	return ret;

}


void MMSR_Close ( STREAM_ACCESS_t *pAccess )
{

	struct RTSP_ACCESS_SYS_s * p_rtsp_stream;
	PrintEnter;
	if(pAccess == NULL)
	{
		PrintError("Null Ptr Error %s(%d)\n", __FUNCTION__, __LINE__);
		return;
	}

#if STREAM_RECORD_FOR_DEBUG
	if(pAccess->b_openfromnexus == TRUE)
	{
		if(g_fd != NULL)
		{
			fclose ( g_fd);
			PrintDebug ("=== close success ===\n");
		}
	}
#endif

	p_rtsp_stream = (struct RTSP_ACCESS_SYS_s *)pAccess->p_sys;

	if(p_rtsp_stream)
	{
		STREAM_RTSP_CloseRtspConnection(p_rtsp_stream);
		pAccess->p_sys = NULL;
	}

	//DD_MEM_PrintList();


	PrintExit;
	return;
}



