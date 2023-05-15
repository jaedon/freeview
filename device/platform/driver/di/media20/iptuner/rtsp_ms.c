/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   rtsp_ms.c  $
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
#include <sys/types.h>
#include <sys/socket.h>

#include "di_uart.h"
#include "mms_asf.h"
#include "stream_global.h"
#include "di_iptuner20.h"
#include "util_url.h"
#include "stream_rtsp.h"
#include "util_b64_encode.h"
#include "rtsp_lib.h"
#include "rtsp_sdp_ms.h"
#include "rtsp_ms.h"
#include "di_iptuner_config.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define RTSP_MS_DEBUG
//#define RTSP_MS_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG

#ifdef RTSP_MS_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#define PrintError		DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter		while (0) ((int (*)(char *, ...)) 0)
#define PrintExit		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* END DEBUG DEFINE */


#ifdef RTSP_MS_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((HINT32 (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)


#define INTERLEAVED_PACK_HEADER_SIZE 4
#define RTP_HEADER_FIXED_SIZE 12
#define RTP_PAYLOAD_HEADER_FIXED_SIZE 4

/***************************************************************
 * RTSP Strings for the interaction between player and server
 ***************************************************************/
static const char STRING_WMP_USERAGENT[] = /* WMS Player UserAgent for WMS */
"User-Agent: WMPlayer/9.0.0.2833 guid/B64345F5-8C45-4818-8A1A-4775F0923FAC"; /* by MS-RTSP Spec */
	/*"User-Agent: WMPlayer/11.0.5721.5145 guid/3300AD50-2C39-46C0-AE0A-E1D79F0FD2D8";*/

static const char STRING_WMP_SUPPORTED[] = "Supported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.eosmsg, com.microsoft.wm.fastcache, com.microsoft.wm.packetpairssrc, com.microsoft.wm.startupprofile";
static const char STRING_WMP_AUTH[]= "X-Accept-Authentication: Negotiate, NTLM, Digest, Basic";
static const char STRING_WMP_TRANSPORT[] = "Transport: RTP/AVP/TCP;unicast;interleaved=0-1;ssrc=75afe1e1;mode=PLAY";


static HINT32 P_RTSP_MS_ProcessRtpPacketAndFillChannelWithAnAsfPacket(struct RTSP_CHANNEL_t *pChannel, HINT32 interleaved_packet_size);
static HINT32 P_RTSP_MS_ParseSdpAndAsfHeader(struct RTSP_ACCESS_SYS_s *stream,char *sdpstr,
						 ASF_HEADER_t **pAsfHeaderInfo_ret, struct RTSP_MS_SdpMsCtx_s **sdpwms_ret);
/* P_RTSP_MS_AnnounceContinue(): Return boolean result, not (ASF_HEADER_t*); and free used (ASF_HEADER_t*) */
static HBOOL P_RTSP_MS_AnnounceContinue(struct RTSP_ACCESS_SYS_s *stream);
static HINT32 P_RTSP_MS_ProcessResponseMessage(struct RTSP_ACCESS_SYS_s *stream);
static HINT32 P_RTSP_MS_RequestPlay(struct RTSP_ACCESS_SYS_s *stream);


/*
 * DESCRIBE Msg.
 * RETURN values
 * 	-1: failed
 * 	 1: success
 * 	 0: retry with other protocol
 */
static HINT32 P_RTSP_MS_RequestDescribe(struct RTSP_ACCESS_SYS_s *stream, char **description_ret)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *description = NULL;
	HINT32 ret = 0;
	char *pField = 0;
	HINT32 i_len = 0;

	PrintEnter;

	/* send DESCRIBE request */
	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get new basic message\n", __FUNCTION__, __LINE__);
		goto retry_by_failure;
	}

	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, STRING_WMP_USERAGENT);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, STRING_WMP_SUPPORTED);
	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_DESCRIBE);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,rtsp_ctrl->p_mrl);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);
	STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);

	/* receive reply for DESCRIBE */
	/* rtsp_hdr is already cleared at above RequestMessageAndFree() */
	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get new message header\n", __FUNCTION__, __LINE__);
		goto retry_by_failure;
	}

	ret  = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);
	if (ret < 0)
	{
		PrintError("%s(%d) Error> Can't read response(%d)\n", __FUNCTION__, __LINE__, ret);
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
		goto retry_by_failure;
	}

	/* NOT OK */
	if (!RTSP_LIB_CheckMessageResponseOK(rtsp_hdr->i_statusCode))
	{
		PrintError("%s(%d) Error> DESCRIBE request failed: %d (%s)\n",
				__FUNCTION__, __LINE__,
				rtsp_hdr->i_statusCode,rtsp_hdr->p_reasonPhrase);

		pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Alert");
		if (pField)
		{
			while(*pField == ' ')
			{
				pField++;
			}
			PrintDebug("%s(%d) message from server --> %s\n",__FUNCTION__, __LINE__, pField);
		}
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
		goto describe_failed;
	}


	i_len = 0;

	/* Content-length must be present */
	if ((pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Content-Length")) != NULL)
	{
		while(*pField == ' ')
		{
			pField++;
		}
		i_len = VK_atoi(pField);
	}
	else
	{
		/* no Content-length */
		PrintError("%s(%d) Error(warning)> No Content-Length in fields!!\n", __FUNCTION__, __LINE__);
	}

	/* in MS-RTSP Etag is not necessary */
	/*
	 * if ((pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"ETag")) == NULL)
	 * {
	 * 	PrintDebug("%s(%d) warning: No ETag!!\n", __FUNCTION__, __LINE__);
	 * }
	 */

	STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	rtsp_hdr = NULL;

	/* copy description (sdp) */
	description = (char *)DD_MEM_Alloc(i_len + 1);
	if (description == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto describe_failed;
	}
	VK_memset(description ,0, i_len + 1);
	i_len = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,(HUINT8 *)description,i_len);
	description[i_len] = '\0';

	PrintDebug("=SDP data =================\n%s\n=(%d bytes)========desc=\n",
			description,(int)VK_strlen(description));
	*description_ret = description;

	PrintExit;
	return 1;

describe_failed:
	if (description)
	{
		DD_MEM_Free(description);
		description = NULL;
	}
	*description_ret = NULL;
	PrintExit;
	return -1;

retry_by_failure:
	if (description)
	{
		DD_MEM_Free(description);
		description = NULL;
	}
	*description_ret = NULL;
	PrintExit;
	return 0;
}


/*
 * SETUP request
 * RETURN Value
 * -1: failed
 *  last status code: SETUP request sent
 */
static HINT32 P_RTSP_MS_RequestSetup(struct RTSP_ACCESS_SYS_s *stream,
				   ASF_HEADER_t *pAsfHeaderInfo,struct RTSP_MS_SdpMsCtx_s *sdpwms)
{
	struct rtsp_ctrl_t *rtsp_ctrl;
	char *buffer = NULL;
	HINT32 ret = 0;
	HINT32 i = 0;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL; /* CID #46229 */
	PrintEnter;

	if (!stream || !pAsfHeaderInfo || !sdpwms) { /* NULL check */
		return -1;
	}

	if (stream->stream_ctrl == NULL)
	{
		PrintError("%s(%d) Error> stream_ctrl is NULL\n", __FUNCTION__, __LINE__);
		return -1;
	}
	rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;

	if (!(pAsfHeaderInfo->streams->n_audio + pAsfHeaderInfo->streams->n_video)) {
		return -1;
	}

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	VK_memset(buffer ,0, BUFSIZE_1K);

	for(i = 0; i < 2; i++)
	{ 	/* 0 --> audio, 1 --> video */
		HINT32 n  = 0;
		HINT32 id = 0;
		char *default_control = NULL;
		if (i == 0) {
			n  = pAsfHeaderInfo->streams->n_audio;
			id = pAsfHeaderInfo->streams->audio_id;
			default_control = "audio";
		}
		else {
			n  = pAsfHeaderInfo->streams->n_video;
			id = pAsfHeaderInfo->streams->video_id;
			default_control = "video";
		}

		if (n)
		{
			struct RTSP_LinkedListHdr_s *p = NULL;
			char *controlstr = NULL;

			// SETUP rtsp://192.168.0.7:554/incoming/gameoftruth.wmv/audio RTSP/1.0
			// --> Transport: RTP/AVP/TCP;unicast;interleaved=0-1;ssrc=1d79b32a;mode=PLAY
			// SETUP rtsp://192.168.0.7:554/incoming/gameoftruth.wmv/video RTSP/1.0
			// --> Transport: RTP/AVP/TCP;unicast;interleaved=0-1;ssrc=75afe1e1;mode=PLAY
			rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);

			STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,STRING_WMP_SUPPORTED);

			if (stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_TCP)
			{
				/* "Transport: RTP/AVP/TCP;unicast;interleaved=0-1;ssrc=75afe1e1;mode=PLAY"; */
				STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,STRING_WMP_TRANSPORT);
			}
			else
			{
				if (i == 0)
				{
					STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,"Transport: RTP/AVP/UDP;unicast;client_port=7002;ssrc=75afe1e1;mode=PLAY");
				}
				else
				{
					STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,"Transport: RTP/AVP/UDP;unicast;client_port=7002;ssrc=75afe1e1;mode=PLAY");
				}
			}

			/* find control for audio_id */
			for (p = sdpwms->streams ; p ; p = p->next)
			{
				struct RTSP_SDP_MS_MediaDesc_s *sdpwmsstream = p->p;
				if (sdpwmsstream->i_stream_num == id)
				{
					controlstr = sdpwmsstream->p_control;
					break;
				}
			}

			if (!controlstr)
			{
				controlstr = default_control; /* default */
			}

			if (VK_strstr(controlstr,"://"))
			{
				VK_snprintf(buffer,BUFSIZE_1K - 1,"%s",controlstr);
			}
			else
			{
				VK_snprintf(buffer,BUFSIZE_1K - 1,"%s/%s",rtsp_ctrl->p_mrl,controlstr);
			}

			//STREAM_RTSP_RequestSetup (rtsp_hdr,buffer);
			STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_SETUP);
			STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,buffer);
			STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);
			STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);
			rtsp_hdr = NULL;

			/* receive message for SETUP */
			//ret = STREAM_RTSP_ReadRtspResponseAndFree(stream);
			{
				char* pField; /* temportal pointer */
				rtsp_hdr = STREAM_RTSP_NewMessageHeader();
				ret = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);
				if (ret < 0)
				{
					PrintError("%s(%d) Error> STREAM_RTSP_ReadMessageAndParseBasic Error\n", __FUNCTION__, __LINE__);
				}

				if ((pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Transport")) != NULL)
				{
					if (!VK_strstr(pField,"RTP"))
					{
						PrintError("%s(%d) Error> Server does not support RTP\n", __FUNCTION__, __LINE__);
						ret = -1;
					}
					if (!VK_strstr(pField,"TCP"))
					{
						PrintError("%s(%d) Error> Server does not support TCP\n", __FUNCTION__, __LINE__);
						ret = -1;
					}
					if (!VK_strstr(pField,"unicast"))
					{
						PrintError("%s(%d) Error> Server does not support unicast\n", __FUNCTION__, __LINE__);
						ret = -1;
					}

					 /* Check interleaved */
					if (!VK_strstr(pField,"interleaved=0-1"))
					{
						PrintError("%s(%d) Error> Server does not support interleaved=0-1 mode\n", __FUNCTION__, __LINE__);
						ret = -1;
					}
					PrintDebug("%s(%d) Transport is supported\n", __FUNCTION__, __LINE__);
				}

				if (ret < 0)
				{
					PrintError("%s(%d) Error> Setup is failed\n", __FUNCTION__, __LINE__);
					goto setup_failed;
				}

				STREAM_RTSP_ReadMessageAndFreeBody(stream, rtsp_hdr);

				STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
				rtsp_hdr = NULL;

				PrintDebug("%s(%d) Setup Success\n", __FUNCTION__, __LINE__);
			}
		}
	}

setup_failed:
	if (rtsp_hdr) /* CID #46229 */
	{
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
	}

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return ret;
}

#if 0 /* unused, so commented */
/*
 * send SET_PARAMETER request
 * return value:  -1:failure   status code: success
 */
static HINT32 P_RTSP_MS_SetParameter(struct RTSP_ACCESS_SYS_s *stream)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	HINT32 ret = 0;
	char *buffer;
	PrintEnter;
	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	VK_memset(buffer ,0, BUFSIZE_1K);

	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,STRING_WMP_USERAGENT);
	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_SET_PARAMETER);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,rtsp_ctrl->p_mrl);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);


	STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);

	/*
	   receive message for SET_PARAMETER
	   if something needs to be done to message, use STREAM_RTSP_ReadMessageAndParseBasic
	   */
	ret = STREAM_RTSP_ReadRtspResponseAndFree(stream);

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return ret;
}

/*
 * send PAUSE request
 * return value:  -1:failure   status code: success
 */
static HINT32 P_RTSP_MS_Pause(struct RTSP_ACCESS_SYS_s *stream)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *buffer;
	HINT32 ret = 0;
	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	VK_memset(buffer ,0, BUFSIZE_1K);

	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,STRING_WMP_USERAGENT);
	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_PAUSE);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,rtsp_ctrl->p_mrl);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);

	STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);

	/*
	   receive message for SET_PARAMETER
	   if something needs to be done to message, use STREAM_RTSP_ReadMessageAndParseBasic
	   */
	ret = STREAM_RTSP_ReadRtspResponseAndFree(stream);

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return ret;
}
#endif /* unused, so commented */


HINT32 RTSP_MS_ProcessRtpPacket(struct RTSP_ACCESS_SYS_s *stream, HUINT8 *buffer,size_t max_size)
{
	HINT32 ret = 0;
	HINT32 nReadCount = 0;
	HINT32 asf_packet_size = 0;
	struct RTSP_CHANNEL_t *pChannel = NULL;
	struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;

	PrintEnter;

	switch(stream->dlopts->data_transport_protocol ) /* select data channel */
	{
	case RTSP_TRANSPORT_TCP:
		pChannel = stream->ctrlChannel;
		break;
	case RTSP_TRANSPORT_UDP_UNICAST:
	case RTSP_TRANSPORT_RTP_UNICAST:
		pChannel = stream->dataChannel[2];
		break;
	case RTSP_TRANSPORT_UDP_MULTICAST:
	case RTSP_TRANSPORT_RTP_MULTICAST:
	default:
		PrintError("%s(%d) Error> Unsupported data transport protocol\n", __FUNCTION__, __LINE__);
		goto process_failed;
		/*break;*/
	}

	if (pChannel == NULL)
	{
		PrintError("%s(%d) Error> Can't get channel\n", __FUNCTION__, __LINE__);
		goto process_failed;
	}


	/* interleaved_data_packet_size 는 rtp packet size와 같아야 한다. */

	/* 한개의 asf packet 을 읽어 온다. UDP인경우 한번에 읽어 Channel Buffer에 넣어둔다. */
	asf_packet_size = P_RTSP_MS_ProcessRtpPacketAndFillChannelWithAnAsfPacket(pChannel, stream_ctrl->rtsp_ctrl->interleaved_data_packet_size);

	//PrintDebug( "pure Read asf_packet_size(except padding byte) = %d \n", asf_packet_size);

	/* bound check */
	if (asf_packet_size < 0 || asf_packet_size > stream_ctrl->packet_length)
	{
		PrintError("%s(%d) Error> Invalid asf_packet_size=[%d] expect less than [%d]\n", __FUNCTION__, __LINE__, asf_packet_size, stream_ctrl->packet_length);
		goto process_failed;
	}
	stream_ctrl->packet_count++;


	/* RTP 에서 last packet이 오면 이곳에서 LAST_PACKET_RECEIVED 를 한다. */


	/*read_data 총합은 asf_packet_size여야한다.*/
	if ((HINT32)max_size >= stream_ctrl->packet_length) /*just go to buffer*/
	{
		/* asf packet은 마지막 0들을 보내지 않는다. */
		ret = RTSP_LIB_ReadFromChannel(pChannel,buffer, asf_packet_size);
		if (ret <= 0)
		{
			goto process_failed;
		}
		/* 서버가 보내지 않은 0들을 여기서 채운다. padding */
		VK_memset(buffer+asf_packet_size,0,stream_ctrl->packet_length - asf_packet_size);
		//PrintExit;
		nReadCount = stream_ctrl->packet_length;
		goto process_success;
	}
	/* buffer가 작아 패킷의 나머지는 write_buffer에 기록해둠 */
	else if ((HINT32)max_size >= asf_packet_size) /* && (packet_length > max_size)   case : EOF */
	{
		ret = RTSP_LIB_ReadFromChannel(pChannel,buffer, asf_packet_size);
		if (ret <= 0)
		{
			PrintError("%s(%d) Error> failed\n", __FUNCTION__, __LINE__);
			goto process_failed;
		}

		VK_memset(buffer+asf_packet_size, 0, max_size - asf_packet_size);

		VK_memset(stream_ctrl->write_buffer,0, stream_ctrl->packet_length - max_size);
		stream_ctrl->write_data_len = stream_ctrl->packet_length - max_size;
		stream_ctrl->write_pos = 0;
		nReadCount = max_size;

		goto process_success;
	}
	else /* (asf_packet_size > max_size) && (packet_length > max_size)   case : reamin partial packet go to write_buffer */
	{
		/*PrintError("%s(%d) Error> remain partial packet max size = %u, remain part = %u\n", __FUNCTION__, __LINE__, max_size, asf_packet_size - max_size);*/
		ret = RTSP_LIB_ReadFromChannel(pChannel,buffer, max_size);
		if (ret <= 0)
		{
			PrintError("%s(%d) Error> failed\n", __FUNCTION__, __LINE__);
			goto process_failed;
		}
		ret = RTSP_LIB_ReadFromChannel(pChannel,stream_ctrl->write_buffer, asf_packet_size - max_size);
		if (ret <= 0)
		{
			PrintError("%s(%d) Error> failed\n", __FUNCTION__, __LINE__);
			goto process_failed;
		}

		VK_memset(stream_ctrl->write_buffer + asf_packet_size - max_size, 0, stream_ctrl->packet_length - asf_packet_size);
		stream_ctrl->write_data_len = stream_ctrl->packet_length - max_size;
		stream_ctrl->write_pos = 0;

		nReadCount = max_size;
		goto process_success;
	}


process_failed:
  	PrintExit;
	return -1;

process_success:
	stream->bInterleavedDataLefted = FALSE;
	PrintExit;
	return nReadCount;
}


/*
 * send SETUP,SEND_REQUEST,PLAY requests.
 *
 *	  return value				1 : success
 *								  0 : error, retry via mms
 *								 -1 : error.
 */
HINT32 RTSP_MS_RequestDescribeAndGetHeader(struct RTSP_ACCESS_SYS_s *stream,
				  ASF_HEADER_t **pAsfHeaderInfo_ret)
{
	struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;
	struct rtsp_ctrl_t *rtsp_ctrl = stream_ctrl->rtsp_ctrl;
	ASF_HEADER_t *pAsfHeaderInfo = NULL;
	struct RTSP_MS_SdpMsCtx_s *sdpwms = NULL;

	char *description = NULL;
	HINT32 ret = 0;

	PrintEnter;

	if (stream->dlopts->i_bandwidth)
	{
		stream_ctrl->bandwidth = stream->dlopts->i_bandwidth;
	}
	else
	{
		stream_ctrl->bandwidth = INT_MAX_BANDWIDTH;
	}

	rtsp_ctrl->i_cseq = 1;

	/* GET SDP description*/
	ret = P_RTSP_MS_RequestDescribe(stream,&description);
	if (ret == 0)
	{
		PrintError("%s(%d) Error> Describe() failed\n", __FUNCTION__, __LINE__);
		goto failed_retry;
	}
	else if (ret < 0)
	{
		PrintError("%s(%d) Error> Describe() failed\n", __FUNCTION__, __LINE__);
		goto failed;
	}


	/* parse SDP decription and ASF header*/
	if (!P_RTSP_MS_ParseSdpAndAsfHeader(stream,description,&pAsfHeaderInfo,&sdpwms))
	{
		PrintError("%s(%d) Error> P_RTSP_MS_ParseSdpAndAsfHeader() failed\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	stream_ctrl->pWmsSdpCtx = sdpwms;
	if (stream_ctrl->pWmsSdpCtx == 0)
	{
		PrintError("%s(%d) Error> Invalid NULL WMS-SDP\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	/* SETUP requests (for audio and video) */
	//if (P_RTSP_MS_RequestSetup(stream,pAsfHeaderInfo,sdpwms) < 0) {
	//	goto failed;
	//}

	/* simulate MS MP client like action with no XML data */
	//if (P_RTSP_MS_SetParameter(stream) < 0) {
	//	goto failed;
	//}

	/* buffering it for Read function */
	stream->b_feed_asfheader_first = TRUE;
	//stream_ctrl->write_pos = 0;
	//VK_memcpy(stream_ctrl->write_buffer, sdpwms->pRawAsfHeader, sdpwms->nRawAsfHeaderLen);
	//stream_ctrl->write_data_len = sdpwms->nRawAsfHeaderLen;

	//RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);
	DD_MEM_Free(description);
	description = NULL;
	*pAsfHeaderInfo_ret = pAsfHeaderInfo;
	return 1;

failed:
	if (pAsfHeaderInfo)
	{
		free_pAsfHeaderInfo_t(pAsfHeaderInfo);
		pAsfHeaderInfo = NULL;
	}
	//if (sdpwms)
	//{
	//	RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);
	//	sdpwms = NULL;
	//}
	if (description)
	{
		DD_MEM_Free(description);
		description = NULL;
	}

	*pAsfHeaderInfo_ret = NULL;
	PrintExit;
	return -1;

  failed_retry: /* retry with other protocol */
	if (pAsfHeaderInfo)
	{
		free_pAsfHeaderInfo_t(pAsfHeaderInfo);
		pAsfHeaderInfo = NULL;
	}
	//if (sdpwms)
	//{
	//	RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);
	//	sdpwms = NULL;
	//}
	if (description)
	{
		DD_MEM_Free(description);
		description = NULL;
	}

	*pAsfHeaderInfo_ret = NULL;
	PrintExit;
	return 0;
}


/*
 * 주어진 channel에서 RTP 를 처리해 ASF Packet 1개를 channel buffer에 넣어준다.
 * input : channel
 * return value:   bytes written in channel buffer,
 *								   -1 ... error
 */
static HINT32 P_RTSP_MS_ProcessRtpPacketAndFillChannelWithAnAsfPacket(struct RTSP_CHANNEL_t *pChannel, HINT32 interleaved_packet_size)
 {
	HINT32 i = 0;
	HINT32 ret = 0;
#if 0 /* warning: unused variable */
	HINT32 is_partial_asf_data = 0;
#endif
	HINT32 i_totalAsfSize = 0;/* size of pure ASF packet */
	HINT32 i_currentAsfSize   = 0;
	HINT32 i_rtpHdr_OptionSize = 0;	/* how many bytes for optional field */

	/* Data Structure for RTP HEADER */
	HUINT8 rtp_header[RTP_HEADER_FIXED_SIZE]; /* RTP fixed header size */
	HUINT8 flags1 = 0;
	HINT32 i_rtpHdr_version = 0; /* 2bit ; 현재 항상 2 */
	HINT32 i_rtpHdr_padding = 0; /* 1bit ; 헤더에 padding 여부 */
	HINT32 i_rtpHdr_extension = 0; /* 1bit ; 고정 헤더 외에 확장 여부 */
	HINT32 i_rtpHdr_csrc_count = 0;		   /* 4bit; number of csrc */

	HUINT8 flags2 = 0;
	/*This bit MUST be set to 1 if the RTP packet contains the last fragment of an ASF data packet, */
	/* or one or more complete ASF data packets. Otherwise, it MUST be set to 0.*/
	HINT32 i_rtpHdr_marker = 0; /* 1bit */
	HINT32 i_rtpHdr_payload_type = 0; /* 7bit; 31 = H.261 */

	HUINT16 i_rtpHdr_seqnum = 0;
	HUINT32 i_rtpHdr_timestamp  = 0;
	HUINT32 i_rtpHdr_SSRC_syncsrcid = 0;
	HUINT32 i_rtpHdr_CSRC_contributingsrcid = 0;


	/* Data Structure for RTP PAYLOAD HEADER */
	HUINT8 rtp_payload_header[RTP_PAYLOAD_HEADER_FIXED_SIZE];
	HINT32 i_rtpPHdr_S_flag_keyframe = 0;
	HINT32 i_rtpPHdr_L_flag_length_or_offset= 0;
	HINT32 i_rtpPHdr_R_flag_relative_timestamp  = 0;
	HINT32 i_rtpPHdr_D_flag_duration = 0;
	HINT32 i_rtpPHdr_I_flag_location_id = 0;
	HINT32 i_rtpPHdr_length_or_offset = 0;

	HUINT32 i_rtpPHdr_relative_timestamp = 0;
	HUINT32 i_rtpPHdr_duration = 0;
	HUINT32 i_rtpPHdr_location_id = 0;


	HUINT8* p_udpMediaBuffer = NULL;
	HINT32 b_flushUdpBuffer = 0;

	PrintEnter;

	if ((pChannel->transport_type == RTSP_TRANSPORT_UDP_UNICAST) ||
		(pChannel->transport_type == RTSP_TRANSPORT_RTP_UNICAST))
	{
		p_udpMediaBuffer = (HUINT8 *)DD_MEM_Alloc(20000);/* twice LARGER than MAX ASF PACKET SIZE */
		VK_memset(p_udpMediaBuffer ,0, 20000);
	}


	while(TRUE) /* break with i_rtpHdr_marker  <- UDP모드에서 packet이 조각으로 들어올대 complete flag */
	{
		i_rtpHdr_OptionSize = 0;

		if ((pChannel->transport_type == RTSP_TRANSPORT_UDP_UNICAST) ||
			(pChannel->transport_type == RTSP_TRANSPORT_RTP_UNICAST))
		{
			RTSP_LIB_ReadADgramAndFillChannelBuffer(pChannel);
		}
		i_currentAsfSize = 0;

		ret = RTSP_LIB_ReadFromChannel(pChannel, rtp_header, RTP_HEADER_FIXED_SIZE);
		if (ret <= 0)
		{
			PrintError("%s(%d) Error> Cannot read RTP data from sock\n", __FUNCTION__, __LINE__);
		}

		//RTSP_LIB_DumpByteBuffer(rtp_header, RTP_HEADER_FIXED_SIZE);



		/* Parse RTP Header */
		flags1 = rtp_header[0];
		flags2 = rtp_header[1];

		i_rtpHdr_version = ((flags1 & 0xC0) >> 6); 	/* 11000000 */
		i_rtpHdr_padding = ((flags1 & 0x20) >> 5); 	/* 00100000 */
		i_rtpHdr_extension = ((flags1 & 0x10) >> 4); /* 00010000 */
		i_rtpHdr_csrc_count = (flags1 & 0x0f); 			/* 00001111 */

		i_rtpHdr_marker = ((flags2 & 0x80) >> 7); 	/* 10000000 */
		i_rtpHdr_payload_type = (flags2 & 0x7f); 	/* 01111111 */

		i_rtpHdr_seqnum = get16_be((HUINT8 *)rtp_header+2);
		i_rtpHdr_timestamp = get32_be((HUINT8 *)rtp_header+4);
		i_rtpHdr_SSRC_syncsrcid = get32_be((HUINT8 *)rtp_header+8);

		i_currentAsfSize -= RTP_HEADER_FIXED_SIZE;

		PrintDebug( "RTP HEADER Ver=%d Pad=%d Ext=%d CSRC=%d M=%d PaylType=%d SeqN = %hu, TimeS=%u, SSRC = %u\n",i_rtpHdr_version,	i_rtpHdr_padding,
				i_rtpHdr_extension,i_rtpHdr_csrc_count, i_rtpHdr_marker, i_rtpHdr_payload_type, i_rtpHdr_seqnum, i_rtpHdr_timestamp, i_rtpHdr_SSRC_syncsrcid);


		if (pChannel->i_prevRtpSeq != i_rtpHdr_seqnum - 1 && pChannel->i_prevRtpSeq != 0)
		{
			PrintError("%s(%d) Error> seq mismatch !!!%d, expect %d\n",
						__FUNCTION__, __LINE__,
						(int)i_rtpHdr_seqnum, pChannel->i_prevRtpSeq + 1);
			PrintError("%s(%d) Error> RTP HEADER Ver=%d Pad=%d Ext=%d CSRC=%d M=%d PaylType=%d SeqN = %hu, TimeS=%u, SSRC = %u\n",
						__FUNCTION__, __LINE__,
						i_rtpHdr_version, i_rtpHdr_padding,
						i_rtpHdr_extension,i_rtpHdr_csrc_count, i_rtpHdr_marker, i_rtpHdr_payload_type, i_rtpHdr_seqnum, i_rtpHdr_timestamp, i_rtpHdr_SSRC_syncsrcid);
			b_flushUdpBuffer = 1;
		}

		pChannel->i_prevRtpSeq = i_rtpHdr_seqnum;


		/* redundant CSCR field */
		for(i = 0 ; i < i_rtpHdr_csrc_count ; i++ ) {

			RTSP_LIB_ReadFromChannel(pChannel,(HUINT8 *)&i_rtpHdr_CSRC_contributingsrcid,4);
			i_rtpHdr_CSRC_contributingsrcid = be2me_32(i_rtpHdr_CSRC_contributingsrcid);
			PrintDebug( "CSCR = %u\n", i_rtpHdr_CSRC_contributingsrcid);
			i_currentAsfSize -= 4;
		}


		/* FUTURE: Parse FEC Header */




		/* Parse RTP Payload Header */
		RTSP_LIB_ReadFromChannel(pChannel,rtp_payload_header,RTP_PAYLOAD_HEADER_FIXED_SIZE); /* get RTP Payload Format Header */
		//RTSP_LIB_DumpByteBuffer(rtp_payload_header, RTP_PAYLOAD_HEADER_FIXED_SIZE);

		if (rtp_payload_header[0] & 0x80) { /* S bit : ASF Key Frame *//* 10000000 */
			i_rtpPHdr_S_flag_keyframe = 1;
		}
		if (rtp_payload_header[0] & 0x40) { /* L bit : 0->offset		 1->Length*//* 01000000 */
			i_rtpPHdr_L_flag_length_or_offset= 1;
		}
		i_rtpPHdr_length_or_offset = (rtp_payload_header[1] << 16) + (rtp_payload_header[2] << 8) + rtp_payload_header[3];




		if (rtp_payload_header[0] & 0x20) { /* R : Relative Timestamp (optional) present *//* 00100000 */
			i_rtpPHdr_R_flag_relative_timestamp  = 1;
			i_rtpHdr_OptionSize += 4;
		}
		if (rtp_payload_header[0] & 0x10) { /* D : Duration (optional) present   */ /* 00010000 */
			i_rtpPHdr_D_flag_duration = 1;
			i_rtpHdr_OptionSize += 4;
		}
		if (rtp_payload_header[0] & 0x08) { /* I : LocationId (optional) present *//* 00001000 */
			i_rtpPHdr_I_flag_location_id = 1;
			i_rtpHdr_OptionSize += 4;
		}


		PrintDebug( "RTP Payload Hdr S=%d, L=%d, R=%d, D=%d, I=%d Length or Offset = %d\n", i_rtpPHdr_S_flag_keyframe, i_rtpPHdr_L_flag_length_or_offset,
			i_rtpPHdr_R_flag_relative_timestamp, i_rtpPHdr_D_flag_duration, i_rtpPHdr_I_flag_location_id, i_rtpPHdr_length_or_offset);


		/* Parse (Optional) RTP Payload Header */
		if (i_rtpHdr_OptionSize > 0)
		{
			char optional_fields[16]; /* maximum 12 bytes */
			HINT32 option_offset = 0;

			RTSP_LIB_ReadFromChannel(pChannel,optional_fields,i_rtpHdr_OptionSize);


			if (i_rtpPHdr_R_flag_relative_timestamp)
			{
				i_rtpPHdr_relative_timestamp = get32_be((HUINT8 *)optional_fields+option_offset);
				PrintDebug( "Relative Timestamp = %u\n", i_rtpPHdr_relative_timestamp);
				option_offset += 4;
			}
			if (i_rtpPHdr_D_flag_duration)
			{
				i_rtpPHdr_duration = get32_be((HUINT8 *)optional_fields+option_offset);
				PrintDebug( "Duration = %u\n", i_rtpPHdr_duration);
				option_offset += 4;
			}
			if (i_rtpPHdr_I_flag_location_id)
			{
				i_rtpPHdr_location_id = get32_be((HUINT8 *)optional_fields+option_offset);
				PrintDebug( "Location_id = %u\n", i_rtpPHdr_location_id);
			}

		}
		i_currentAsfSize -= RTP_PAYLOAD_HEADER_FIXED_SIZE + i_rtpHdr_OptionSize;


		if (i_rtpPHdr_L_flag_length_or_offset == 1) /* length */
		{
			interleaved_packet_size += i_currentAsfSize;

			/* FUTURE identify why -4(fixed payload header size)? in the spec */
			i_currentAsfSize = i_rtpPHdr_length_or_offset - 4 - i_rtpHdr_OptionSize;

			/* error check */
			if (interleaved_packet_size != i_currentAsfSize)
			{
				PrintError("%s(%d) Error> RTP size %d and Interleved size %d mismatch\n", __FUNCTION__, __LINE__, i_currentAsfSize, interleaved_packet_size);
				PrintError("%s(%d) Error> RTP Payload Hdr S=%d, L=%d, R=%d, D=%d, I=%d Length or Offset = %d\n", __FUNCTION__, __LINE__, i_rtpPHdr_S_flag_keyframe, i_rtpPHdr_L_flag_length_or_offset,
				i_rtpPHdr_R_flag_relative_timestamp, i_rtpPHdr_D_flag_duration, i_rtpPHdr_I_flag_location_id, i_rtpPHdr_length_or_offset);
			}
		}
		else /* offset */
		{
			i_currentAsfSize = pChannel->i_dgramRead + i_currentAsfSize;
		}



		if ((pChannel->transport_type == RTSP_TRANSPORT_UDP_UNICAST) ||
			(pChannel->transport_type == RTSP_TRANSPORT_RTP_UNICAST))
		{

			/* FUTURE : Forward Error Correction Routine */
			/* input : FEC pasring info, data chunk   output : p_udpMediaBuffer */

			/* FUTURE : clear me */
			RTSP_LIB_ReadFromChannel(pChannel, (p_udpMediaBuffer + i_totalAsfSize), i_currentAsfSize); /* just add up */
		}

		i_totalAsfSize += i_currentAsfSize;

		if (i_rtpHdr_marker) /* EOF asf complete packet ? */
		{
			if ((pChannel->transport_type == RTSP_TRANSPORT_UDP_UNICAST) ||
				(pChannel->transport_type == RTSP_TRANSPORT_RTP_UNICAST))
			{
				/* FUTURE : GET FEC Result */
				/* param [out] : pChannel buffer, sizeof buffer result : data_len
				code */
				if (b_flushUdpBuffer == 0)
				{
					VK_memcpy(pChannel->p_buffer, p_udpMediaBuffer, i_totalAsfSize);
					pChannel->i_dataLen = i_totalAsfSize;
					pChannel->i_bufferPos = 0;
				}
				else
				{ /* flush invalid packet */
					pChannel->i_dataLen = 0;
					pChannel->i_bufferPos = 0;
					i_totalAsfSize = 0;
				}
			}

			break;
		}
	}

	if ((pChannel->transport_type == RTSP_TRANSPORT_UDP_UNICAST) ||
		(pChannel->transport_type == RTSP_TRANSPORT_RTP_UNICAST))
	{
		if (p_udpMediaBuffer)
		{
			DD_MEM_Free(p_udpMediaBuffer);/* twice LARGER than MAX ASF PACKET SIZE */
			p_udpMediaBuffer = NULL;
		}
	}

	ret = i_totalAsfSize;

	PrintExit;
	return ret;
  }


/*
 * get pAsfHeaderInfo and sdpwms by sdpstr, and stream->stream_ctrl->bandwidth
 * return value: 0 : failed	   1: success
 */
static HINT32 P_RTSP_MS_ParseSdpAndAsfHeader(struct RTSP_ACCESS_SYS_s *stream,char *sdpstr,
						 ASF_HEADER_t **pAsfHeaderInfo_ret, struct RTSP_MS_SdpMsCtx_s **sdpwms_ret)
{
	ASF_HEADER_t *pAsfHeaderInfo = NULL;
	struct RTSP_MS_SdpMsCtx_s *sdpwms = NULL;

	PrintEnter;

	if (!stream || !sdpstr) { /* stream could not be NULL */
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	/* parse sdp and get information about file to download */
	sdpwms = RTSP_SDP_MS_ParseSdp(sdpstr);
	if (sdpwms->nRawAsfHeaderLen <= (HINT32)sizeof(struct asf_header_t)) {
		PrintError("%s(%d) Error> asf header smaller than asf_header_t\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	pAsfHeaderInfo = new_pAsfHeaderInfo_t();
	asf_interpret_header(pAsfHeaderInfo,stream->stream_ctrl->bandwidth,
			 sdpwms->pRawAsfHeader,sdpwms->nRawAsfHeaderLen);


	/* set asf header len */
	pAsfHeaderInfo->nRawAsfHeaderLen = sdpwms->nRawAsfHeaderLen;

	/* copy raw header */
	pAsfHeaderInfo->pRawAsfHeader = DD_MEM_Alloc(sdpwms->nRawAsfHeaderLen);
	VK_memset(pAsfHeaderInfo->pRawAsfHeader ,0, sdpwms->nRawAsfHeaderLen);
	VK_memcpy(pAsfHeaderInfo->pRawAsfHeader,sdpwms->pRawAsfHeader,sdpwms->nRawAsfHeaderLen);


	/* return structures */
	*pAsfHeaderInfo_ret = pAsfHeaderInfo;
	*sdpwms_ret		 = sdpwms;

	return 1;

failed:
	PrintError("%s(%d) Error> failed\n", __FUNCTION__, __LINE__);
	if (pAsfHeaderInfo) free_pAsfHeaderInfo_t(pAsfHeaderInfo);
	if (sdpwms) RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);
	*pAsfHeaderInfo_ret = NULL;
	*sdpwms_ret = NULL;
	PrintExit;
	return 0;
}



HINT32 RTSP_MS_ProcessInterleavedPacket(struct RTSP_ACCESS_SYS_s *stream)
{
	HUINT8 interleaved_p_hdr[INTERLEAVED_PACK_HEADER_SIZE];
	struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;
	HINT32 ret = 0;
	HINT32 nStatusCode = 0;

	PrintEnter;

	if (stream->bInterleavedDataLefted == TRUE)
	{
		goto success;
	}

	ret = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,interleaved_p_hdr,INTERLEAVED_PACK_HEADER_SIZE);
	if (ret < INTERLEAVED_PACK_HEADER_SIZE) {
		PrintError("%s(%d) Error> failed(ret:%d)\n", __FUNCTION__, __LINE__, ret);
		goto failed;
	}

	if (interleaved_p_hdr[0] == 0x24)
	{
		goto interleaved_packet;
	}

	if (!VK_strncasecmp((char *)interleaved_p_hdr,"SET_",INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"GET_",INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"OPTI",INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"RTSP",INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"ANNO",INTERLEAVED_PACK_HEADER_SIZE))
	{
		RTSP_LIB_PutMessageForLaterRead(stream,interleaved_p_hdr,INTERLEAVED_PACK_HEADER_SIZE); /* push back interleaved_p_hdr */

		ret = P_RTSP_MS_ProcessResponseMessage(stream);
		if (ret < 0) {
			PrintError("%s(%d) Error> failed (ret:%d)\n", __FUNCTION__, __LINE__, ret);
			goto failed;
		}
		nStatusCode = ret;

		if (ret == 0 && stream_ctrl->session_status == RTSP_SESSION_STATUS_FINISHED)
		{
			/* streaming finished (at least it says) */

			/*
			  wait for 2 seconds.. this is necessary becasue
			  otherwise it would block when waiting "ANNOUNCE"
			*/
			if (RTSP_LIB_SelectCtrlChannel(stream,2) > 0)
			{
				/* Prevent CID#11976 */
				/* ASF_HEADER_t *pAsfHeaderInfo = NULL; */
				if (P_RTSP_MS_AnnounceContinue(stream) != FALSE)
				{
					PrintDebug("%s(%d) streaming still continues!!\n", __FUNCTION__, __LINE__);
					stream->b_feed_asfheader_first = TRUE;

					/*free_pAsfHeaderInfo_t(pAsfHeaderInfo); this is done in STREAM_RTSP_FreeRtspCtrl */
					stream_ctrl->session_status = RTSP_SESSION_STATUS_REWIND;
					return 0; /* write asf header */
				}
			}

			/* if no ANNOUCNE, finish streaming */
			PrintDebug("%s(%d) finish rtsp streaming\n", __FUNCTION__, __LINE__);
			return 0; /* streaming really finished */
		}
#if 0
		else
		{
			/* streaming still continues */
			ret = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,interleaved_p_hdr,INTERLEAVED_PACK_HEADER_SIZE);
			if (ret < INTERLEAVED_PACK_HEADER_SIZE)
			{
				goto failed;
			}
		}
#endif
	}
	goto success;

interleaved_packet:
	/* RTSP interleaved frame */
	/* 32 bit is interleved frame's interleaved_p_hdr */
	if (interleaved_p_hdr[0] != 0x24) /* $ */
	{
		PrintError("%s(%d) Error> wrong rdt/rtp magic : [0x%02x]\n", __FUNCTION__, __LINE__, interleaved_p_hdr[0]);
		/* dump interleaved_p_hdr*/
		RTSP_LIB_DumpByteBuffer(interleaved_p_hdr,INTERLEAVED_PACK_HEADER_SIZE);
		PrintDebug("\n");
		goto failed;
	}

	/*
	 *  0                   1                   2                   3
	 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * | "$" = 24      | Channel ID    | Length in bytes               |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * : Length number of bytes of binary data                         :
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	stream_ctrl->rtsp_ctrl->interleaved_data_packet_size = (interleaved_p_hdr[1] << 16) + (interleaved_p_hdr[2] << 8) + interleaved_p_hdr[3];
	/*PrintDebug( "interleaved_data_packet_size = %d\n", stream_ctrl->rtsp_ctrl->interleaved_data_packet_size);*/
	stream->bInterleavedDataLefted = TRUE;

success:
	PrintExit;
	return nStatusCode;

failed:
	PrintExit;
	return -1;
}


/*
 * After reveiving EOF packet, ANNOUNCE may come when
 * streaming still continues. (this is most likely to be a download protection)
 *
 *
 *
 */
static HBOOL P_RTSP_MS_AnnounceContinue(struct RTSP_ACCESS_SYS_s *stream) /* Prevent CID#11976 */
{
	/* Return boolean result, not (ASF_HEADER_t*); and free used (ASF_HEADER_t*) */
	struct RTSP_CTRL_s *stream_ctrl = NULL;
	struct rtsp_ctrl_t *rtsp_ctrl = NULL;

	HINT32 i = 0;
	char checkbuf[10];			  		/* check whether the message is ANNOUCNE */
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	HINT32 i_cseq = 0;			   	/* cseq receivedi */
	HINT32 i_statusCode = 0;				/* rtsp_hdr status value */
	char *pField = NULL;			 	/* filed pointer for rtsp_hdr   */
	HINT32 i_len = 0;						/* length of description (content length) */
	char *description = NULL;	   		/* description pointer, data of rtsp-packet	*/
	char *buffer = NULL;			/* temp buffer for building req */

	struct RTSP_MS_SdpMsCtx_s *sdpwms = NULL;
	ASF_HEADER_t* pAsfHeaderInfo = NULL;

	PrintEnter;

	if ((stream == NULL) || (stream->stream_ctrl == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		goto announce_failed;
	}
	stream_ctrl = stream->stream_ctrl;

	if (stream_ctrl->rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp_ctrl\n", __FUNCTION__, __LINE__);
		goto announce_failed;
	}
	rtsp_ctrl = stream_ctrl->rtsp_ctrl;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	if (buffer == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto announce_failed;
	}
	VK_memset(buffer ,0, BUFSIZE_1K);

	i = RTSP_LIB_ReadFromCtrlChannelWithNoWait(stream,checkbuf,8);
	if (i < 8 || VK_memcmp(checkbuf,"ANNOUNCE",8))
	{
		PrintError("%s(%d) Error> Got invalid message(i:%d, %s)\n", __FUNCTION__, __LINE__, i, checkbuf);
		goto announce_failed;
	}

	RTSP_LIB_PutMessageForLaterRead(stream,checkbuf,i); /* push back "ANNOUNCE" */

	rtsp_hdr = STREAM_RTSP_NewMessageHeader(); /* CID #46208 */
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get new mesage\n", __FUNCTION__, __LINE__);
		goto announce_failed;
	}

	i_statusCode = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);
	if (i_statusCode < 0)
	{
		PrintError("%s(%d) Error> Cant read message\n", __FUNCTION__, __LINE__);
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
		goto announce_failed;
	}

	/* to parse Content-Length Field */
	if ((pField = STREAM_RTSP_FindMessageHeaderField (rtsp_hdr,"Content-Length")) != NULL)
	{
		while(*pField == ' ')
		{
			pField++;
		}
		i_len = VK_atoi (pField);
	}

	/* to parse Cseq Field */
	if ((pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Cseq")) != NULL)
	{
		while(*pField == ' ')
		{
			pField++;
		}
		i_cseq = VK_atoi(pField);
	}

	/* copy sdp fields */
	description = DD_MEM_Alloc(i_len + 1);
	if (description == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto announce_failed;
	}
	VK_memset(description ,0, i_len + 1);

	i_len = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,description,i_len);
	description[i_len] = '\0';

	STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	rtsp_hdr = NULL;

	if (i_statusCode != RTSP_STATUS_ANNOUNCE)
	{
		PrintError("%s(%d) Error> Unmatched status code(%d)\n", __FUNCTION__, __LINE__, i_statusCode);
		goto announce_failed;
	}

	/* got annouce packet, and data is in description */
	if (rtsp_ctrl->p_sessionNum == NULL)
	{
		PrintError("%s(%d) Error> Invalid session number\n", __FUNCTION__, __LINE__);
		/*goto announce_failed; */
	}
	else /* CID #46179 */
	{
  		STREAM_RTSP_RequestMessageOK(stream,i_cseq,rtsp_ctrl->p_sessionNum); /* OK (required, according to RFC )*/
	}

	(void) P_RTSP_MS_ParseSdpAndAsfHeader (stream,description,&pAsfHeaderInfo,&sdpwms);

	/* Be careful with Cseq: */

	/* SETUP */
	if (P_RTSP_MS_RequestSetup(stream,pAsfHeaderInfo,sdpwms) < 0)
	{
		goto announce_failed;
	}

	/* send SET_PARAMETER request to download stream */
#if 0
	if (P_RTSP_MS_SetParameter(stream) < 0)
	{
		goto announce_failed;
	}
#endif

	/*  send PLAY request to download stream */
	if (P_RTSP_MS_RequestPlay(stream) < 0)
	{
		goto announce_failed;
	}

	/*
	 * here do NOT need to write pAsfHeaderInfo or those stuff
	 * those are already done in very first protocol initiation.
	 */

	RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);

	if (description != NULL)
	{
		DD_MEM_Free(description);
		description = NULL;
	}

	if (buffer != NULL)
	{
		DD_MEM_Free(buffer);
		buffer = NULL;
	}

	if (pAsfHeaderInfo != NULL)
	{
		free_pAsfHeaderInfo_t(pAsfHeaderInfo);
		pAsfHeaderInfo = NULL;
	}

	PrintExit;
	return TRUE; /*pAsfHeaderInfo;*/


announce_failed:
	if (description)
	{
		DD_MEM_Free(description);
		description = NULL;
	}
	if (sdpwms)
	{
		RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);
	}

	if (rtsp_hdr != NULL) /* CID #46208 */
	{
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
	}

	if (pAsfHeaderInfo)
	{
		free_pAsfHeaderInfo_t(pAsfHeaderInfo);
	}
	if (buffer)
	{
		DD_MEM_Free(buffer);
		buffer = NULL;
	}
	PrintExit;
	return FALSE;
}



HINT32 RTSP_MS_RequestAsyncPlay(struct RTSP_ACCESS_SYS_s *stream, float scale)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *buffer;
	HINT32 ret = 0;
	HINT32 bandwidth_to_send = 0;

	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	VK_memset(buffer ,0, BUFSIZE_1K);

	/* Sending part */
	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,STRING_WMP_USERAGENT);


#if 0
	if (stream->dlopts->resume_download) {
	RTSP_MS_prepare_resuming(stream);
	}
#endif


	STREAM_RTSP_SetRangeField(rtsp_hdr,stream->dlopts->p_range);
	STREAM_RTSP_SetSpeedField(rtsp_hdr,stream->dlopts->p_speed);

	/* bandwidth specified */
	bandwidth_to_send =
	(stream->dlopts->i_bandwidth) ? (stream->dlopts->i_bandwidth) : INT_MAX_BANDWIDTH;

	VK_snprintf(buffer,BUFSIZE_1K - 1,"Bandwidth: %d",bandwidth_to_send);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,buffer);

	//if (!(scale > 0.9 && scale < 1.1))
	//{
	VK_snprintf(buffer,BUFSIZE_1K - 1,"Scale: %.3f", scale);
	PrintDebug("Scale String is %s\n", buffer);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,buffer);
	//}
	VK_snprintf(buffer,BUFSIZE_1K - 1,
		 "X-Accelerate-Streaming: AccelDuration = 18000;AccelBandwidth=%d",bandwidth_to_send);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,buffer);
	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_PLAY);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,rtsp_ctrl->p_mrl);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);

	STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return ret;

/* warning: label 'failed' defined but not used */
/* failed: */
  	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return -1;
}

/*
 * PLAY request
 * Return VALUE
 * -1:failure
 *  status code: success
 */
static HINT32 P_RTSP_MS_RequestPlay(struct RTSP_ACCESS_SYS_s *stream) /* Unused, so commented */
{
		struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *pField = NULL;
	HINT32 ret = 0;

	PrintEnter;

	/* Sending part */
	RTSP_MS_RequestAsyncPlay(stream, 1.000);

	/* Receiving part */

	/* receive message for PLAY request */
	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	ret = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);

	if (!RTSP_LIB_CheckMessageResponseOK(ret))
	{
		PrintError("%s(%d) Error> PLAY request returned: %d (%s)\n", __FUNCTION__, __LINE__,
				rtsp_hdr->i_statusCode,rtsp_hdr->p_reasonPhrase);

		pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Alert");
		if (pField)
		{
			while(*pField == ' ')
			{
				pField++;
			}
			PrintError("%s(%d) Error> message from server --> %s\n", __FUNCTION__, __LINE__, pField);
		}
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
		goto failed;
	}

	/* display real speed (might differ from user requested) */
	if ((pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Speed")) != NULL)
	{
		if (stream->dlopts->p_speed)
		{
			while(*pField == ' ')
			{
				pField++;
			}
			PrintData("Speed: %s\n",pField);
		}
	}

	if ((pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Range")) != NULL)
	{
		if (stream->dlopts->p_range)
		{
			while(*pField == ' ')
			{
				pField++;
			}
			PrintData("Range: %s\n",pField);
		}
	}

	/* skip content-length bytes from network */
	STREAM_RTSP_ReadMessageAndFreeBody(stream,rtsp_hdr);
	STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	rtsp_hdr = NULL;
	PrintExit;
	return ret;

failed:
	PrintExit;
	return -1;
}


/*
 * read from media stream. (wmserver rdp packet)
 *
 * fill 'buffer' with data, and buffer size is 'max_size'.
 * receive one chunk from stream->sock, and fills 'max_size' bytes to buffer.
 * and if there are more bytes in the chunk, the data is stored to
 * stream->Channel->buffer.
 *
 *	return value: >0 ... written bytes.
 *				  0	 ... eof
 *				 -1	 ... error
 */
HINT32 RTSP_MS_RequestSetup(struct RTSP_ACCESS_SYS_s *stream)
{
	HINT32 ret = 0;
	struct RTSP_CHANNEL_t * pDataChannel = NULL;
	PrintEnter;

	if ((stream == NULL) || (stream->stream_ctrl == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL args\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	PrintDebug("%s(%d) rtsp_protocol_type:%d\n", __FUNCTION__, __LINE__, stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type);


	/* select data channel */
	if (stream->dlopts->data_transport_protocol  ==  RTSP_TRANSPORT_TCP)
	{
		pDataChannel = stream->ctrlChannel;
	}
	else
	{
		pDataChannel = stream->dataChannel[2];
	}

	if (pDataChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL channel(dlopts->data_transport_protocol:%d)\n", __FUNCTION__, __LINE__, stream->dlopts->data_transport_protocol);
		goto func_error;
	}


	/* start the channel */
	if(pDataChannel->b_started == TRUE)
	{
		PrintError("%s(%d) Error> Already started channel(%d)\n",
					__FUNCTION__, __LINE__,
					stream->dlopts->data_transport_protocol);
		goto func_done;
	}

	/* This is error case: */
	if (P_RTSP_MS_RequestSetup(stream, stream->stream_ctrl->rtsp_ctrl->pAsfHeaderInfo,
			stream->stream_ctrl->pWmsSdpCtx) < 0)
	{
		PrintError("%s(%d) Error> MS_Setup() failed\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

func_done:
	PrintExit;
	return ret;

func_error:
  	PrintExit;
	return -1;
}

HINT32 RTSP_MS_RequestPlay(struct RTSP_ACCESS_SYS_s *stream)
{
	HINT32 ret = 0;
	struct RTSP_CHANNEL_t * pDataChannel = NULL;
	PrintEnter;

	if ((stream == NULL) || (stream->stream_ctrl == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL args\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	PrintDebug("%s(%d) rtsp_protocol_type:%d\n", __FUNCTION__, __LINE__, stream->stream_ctrl->rtsp_ctrl->rtsp_protocol_type);


	/* select data channel */
	if (stream->dlopts->data_transport_protocol  ==  RTSP_TRANSPORT_TCP)
	{
		pDataChannel = stream->ctrlChannel;
	}
	else
	{
		pDataChannel = stream->dataChannel[2];
	}

	if (pDataChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL channel(dlopts->data_transport_protocol:%d)\n", __FUNCTION__, __LINE__, stream->dlopts->data_transport_protocol);
		goto func_error;
	}


	/* start the channel */
	if(pDataChannel->b_started == TRUE)
	{
		PrintError("%s(%d) Error> Already started channel(%d)\n",
					__FUNCTION__, __LINE__,
					stream->dlopts->data_transport_protocol);
		goto func_done;
	}

	/* This is error case: */
	PrintError("%s(%d) Error> Invalid data channel status(data_transport_protocol:%d)\n",
				__FUNCTION__, __LINE__,
				stream->dlopts->data_transport_protocol);

	if (P_RTSP_MS_RequestPlay(stream) < 0)
	{
		PrintError("%s(%d) Error> MS_Play() failed\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	pDataChannel->b_started = TRUE;
	stream->stream_ctrl->session_status = RTSP_SESSION_STATUS_STARTED;

func_done:
	PrintExit;
	return ret;

func_error:
  	PrintExit;
	return -1;
}

HINT32 RTSP_MS_StartDataChannelIfNotStarted(struct RTSP_ACCESS_SYS_s *stream)
{
	return RTSP_MS_RequestPlay(stream);
}

/*
 * process rtsp header coming among rtsp wmserver media packet
 *
 * return value:	   status code ... normal proceed case
 *							   0 ... EOF packet
 *							  -1 ... error
 */
static HINT32 P_RTSP_MS_ProcessResponseMessage(struct RTSP_ACCESS_SYS_s *stream)
{
	struct RTSP_CTRL_s *stream_ctrl = NULL;
	struct rtsp_ctrl_t *rtsp_ctrl = NULL;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	HINT32 i_statusCode = 0;
	size_t bodylen = 0;
	HUINT8 *body = NULL;
	char *pField = NULL;
	HINT32 i_cseq = 0;

	PrintEnter;

	if ((stream == NULL) || (stream->stream_ctrl == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		goto failed;
	}
	stream_ctrl = stream->stream_ctrl;

	if (stream_ctrl->rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp_ctrl\n", __FUNCTION__, __LINE__);
		goto failed;
	}
	rtsp_ctrl = stream_ctrl->rtsp_ctrl;

	/* push back 4 bytes */
	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get RTSP header\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	i_statusCode = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);
	if (i_statusCode < 0)
	{
		PrintError("%s(%d) Error> Can't read RTSP header\n", __FUNCTION__, __LINE__);
		goto failed;
	}

	/* get content length data */
	if ((pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Content-Length")) != NULL)
	{
		while (*pField == ' ')
		{
			pField++;
		}
		bodylen = VK_atoi(pField);
		body = DD_MEM_Alloc(bodylen);
		if (body == NULL)
		{
			PrintError("%s(%d) Error> Out of memory!\n", __FUNCTION__, __LINE__);
			goto failed;
		}
		VK_memset(body ,0, bodylen);
		bodylen = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,body,bodylen); /* read rest of data */
	}

	if ((pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Cseq")) != NULL)
	{
		while(*pField == ' ') pField++;
		i_cseq = VK_atoi(pField);
	}

	if (!VK_strncasecmp(rtsp_hdr->p_protocol,"RTSP",4))
	{
		/* RTSP/1.0 200 OK ... ignore this */
		if (i_cseq != (HINT32)rtsp_ctrl->i_cseq - 1)  /* rtsp_ctrl->i_cseq increment in send */
		{
			PrintError("%s(%d) Error> CSeq mismatch: expected %d, got %d", __FUNCTION__, __LINE__, rtsp_ctrl->i_cseq - 1,i_cseq);
		}
		/* DO NOT send OK back to this */
	}
	else if (!VK_strncasecmp(rtsp_hdr->p_protocol,"SET_PARAMETER",13)) /* SET_PARAMETER */
	{
		char *p;

		/* EOF 는 SET_PARAMETER로 온다. 이후 서버측에서 소켓을 닫는다. */
		/* 닫힌 소켓을 사용하면 131 sock error, 비정상 종료 현상이 나타날 수 잇다. */
		/* MS-RTSP X-Notice(with End-of-Stream Reached), X-RTP-Info(with last RTP seqN), */
		/* X-Playlist-Gen-Id 가 오면 Body를 보고 세부 판단을 한다. */
		if (body && (p = VK_strstr((char *)body,"EOF:")))  // body must exist <<- fix 1.2.2
		{
			p += 4;
			while(*p == ' ') p++;

			if((!VK_strncasecmp(p,"true",4)))  /* EOF */
			{
				stream_ctrl->session_status = RTSP_SESSION_STATUS_FINISHED;
				PrintDebug("=======================RTSP_SESSION_STATUS_FINISHED=========================\n");

				if (stream->p_keepalive_thread)
				{
					VK_TASK_Stop(stream->p_keepalive_thread->thread_id);
					VK_TASK_Destroy(stream->p_keepalive_thread->thread_id);
					DD_MEM_Free(stream->p_keepalive_thread);
					stream->p_keepalive_thread = NULL;
				}

				DD_MEM_Free(body);
				body = NULL;
				STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
				rtsp_hdr = NULL;
				return 0;
			}
		}

		if (rtsp_ctrl->p_sessionNum == NULL)
		{
			PrintError("%s(%d) Error> Invalid session number\n", __FUNCTION__, __LINE__);
			/*goto announce_failed; */
		}
		else /* CID #46180 */
		{
			STREAM_RTSP_RequestMessageOK(stream,i_cseq,rtsp_ctrl->p_sessionNum);
		}
	}
	else
	{
		/* everything else ... just send OK */
		if (rtsp_ctrl->p_sessionNum == NULL)
		{
			PrintError("%s(%d) Error> Invalid session number\n", __FUNCTION__, __LINE__);
			/*goto announce_failed; */
		}
		else /* CID #46180 */
		{
			STREAM_RTSP_RequestMessageOK(stream,i_cseq,rtsp_ctrl->p_sessionNum);
		}
	}


	if (body)
	{
		DD_MEM_Free(body);
		body = NULL;
	}
	if (rtsp_hdr)
	{
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
	}
	PrintExit;
	return i_statusCode;

failed:
	if (body)
	{
		DD_MEM_Free(body);
		body = NULL;
	}
	if (rtsp_hdr)
	{
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
	}
	PrintExit;
	return -1;
}


