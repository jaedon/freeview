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

#include <sys/types.h>
#include <sys/socket.h>

#include "di_uart.h"
#include "rtsp_lib.h"
#include "mms_asf.h"

#include "stream_global.h"
#include "di_iptuner.h"
#include "util_url.h"
#include "stream_rtsp.h"

#include "util_b64_encode.h"
#include "rtsp_sdp_ms.h"
#include "rtsp_ms.h"

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
static const char STRING_WMP_USERAGENT[] =
"User-Agent: WMPlayer/9.0.0.2833 guid/B64345F5-8C45-4818-8A1A-4775F0923FAC"; /* by MS-RTSP Spec */
	/*"User-Agent: WMPlayer/11.0.5721.5145 guid/3300AD50-2C39-46C0-AE0A-E1D79F0FD2D8";*/

static const char STRING_WMP_SUPPORTED[] = "Supported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.eosmsg, com.microsoft.wm.fastcache, com.microsoft.wm.packetpairssrc, com.microsoft.wm.startupprofile";

static const char STRING_WMP_AUTH[]= "X-Accept-Authentication: Negotiate, NTLM, Digest, Basic";
static const char STRING_WMP_TRANSPORT[] = "Transport: RTP/AVP/TCP;unicast;interleaved=0-1;ssrc=75afe1e1;mode=PLAY";



static HINT32 P_RTSP_MS_ProcessRtpPacketAndFillChannelWithAnAsfPacket(struct RTSP_CHANNEL_t *Channel, HINT32 interleaved_packet_size);
static HINT32 P_RTSP_MS_ParseSdpAndAsfHeader(struct RTSP_ACCESS_SYS_s *stream,char *sdpstr,
						 ASF_HEADER_t **pAsfHeaderInfo_ret, struct RTSP_MS_SdpMsCtx_s **sdpwms_ret);
static ASF_HEADER_t *P_RTSP_MS_AnnounceContinue(struct RTSP_ACCESS_SYS_s *stream);


/*
 * DESCRIBE Msg.
 * RETURN values
 * 	-1: failed
 * 	 1: success
 * 	 0: retry with other protocol
 */
static HINT32 P_RTSP_MS_Describe(struct RTSP_ACCESS_SYS_s *stream,char **description_ret)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->protocol.rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *description = NULL;
	HINT32 ret = 0;
	char *pField = 0;
	HINT32 len = 0;

	PrintEnter;

	/* send DESCRIBE request */
	rtsp_hdr = STREAM_RTSP_NewRtspHeaderAndFillBasicFields(rtsp_ctrl);

	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr, STRING_WMP_USERAGENT);
	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr, STRING_WMP_SUPPORTED);
	//rtsp_request_describe(rtsp_hdr,rtsp_ctrl->mrl);
	STREAM_RTSP_SetProtoclCmdForRtspHeader(rtsp_hdr,STREAM_RTSP_CMD_DESCRIBE);
	STREAM_RTSP_SetUriForRtspHeader(rtsp_hdr,rtsp_ctrl->mrl);
	STREAM_RTSP_SerializeRtspHeader(rtsp_hdr);
	STREAM_RTSP_SendSerializedRtspRequestAndFree(stream,rtsp_hdr);

	/* receive reply for DESCRIBE */
	rtsp_hdr = STREAM_RTSP_NewRtspProtocolHeader();
	ret  = STREAM_RTSP_ReadRtspHeaderAndParseBasic(stream,rtsp_hdr);
	if(ret < 0)
	{
	STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);
		goto retry_by_failure;
	}

	/* NOT OK */
	if(!STREAM_RTSP_CheckRtspResponseOk(rtsp_hdr->status_code)) {
	PrintCriticalDebug("DESCRIBE request returned: %d (%s)\n",
		rtsp_hdr->status_code,rtsp_hdr->reason_phrase);

		pField = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Alert");
		if(pField) {
			while(*pField == ' ') pField++;
			PrintCriticalDebug("message from server --> %s\n",pField);
	}
	STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);
		goto describe_failed;
	}


	len = 0;

	/* Content-length must be present */
	if((pField = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Content-Length")) != NULL) {
		while(*pField == ' ') pField++;
		len = VK_atoi(pField);
	}
	else { /* no Content-length */
	PrintCriticalDebug("warning: No Content-Length in fields!!\n");
	}

	if((/*pField = */STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"ETag")) == NULL) {
	PrintDebug("warning: No ETag!!\n"); /* in MS-RTSP Etag is not necessary */
	}

	STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);

	/*
	  copy description (sdp)
	*/
	description = (char *)DD_MEM_Alloc(len + 1);
	VK_memset(description ,0, len + 1);
	len = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,(HUINT8 *)description,len);
	description[len] = '\0';

	PrintDebug("=SDP data =================\n%s\n=(%d bytes)========desc=\n",
		description,(int)VK_strlen(description));
	*description_ret = description;

	PrintExit;
	return 1;

describe_failed:
	if(description)
	{
		DD_MEM_Free(description);
		description = NULL;
	}
	*description_ret = NULL;
	PrintExit;
	return -1;

retry_by_failure:
   if(description)
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
static HINT32 P_RTSP_MS_Setup(struct RTSP_ACCESS_SYS_s *stream,
				   ASF_HEADER_t *pAsfHeaderInfo,struct RTSP_MS_SdpMsCtx_s *sdpwms)
{
	struct rtsp_ctrl_t *rtsp_ctrl;
	char *buffer = NULL;
	HINT32 ret = 0;
	HINT32 i = 0;
	struct RTSP_PROT_HEADER_s *rtsp_hdr;
	PrintEnter;

	if(!stream || !pAsfHeaderInfo || !sdpwms) { /* NULL check */
	return -1;
	}

	if(stream->stream_ctrl == NULL)
	{
		PrintError("stream_ctrl is NULL\n");
		return -1;
	}
	rtsp_ctrl = stream->stream_ctrl->protocol.rtsp_ctrl;

	if(!(pAsfHeaderInfo->streams->n_audio + pAsfHeaderInfo->streams->n_video)) {
	return -1;
	}

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	VK_memset(buffer ,0, BUFSIZE_1K);

	for(i = 0; i < 2; i++)
	{ 	/* 0 --> audio, 1 --> video */
		HINT32 n  = 0;
		HINT32 id = 0;
		char *default_control = NULL;
		if(i == 0) {
			n  = pAsfHeaderInfo->streams->n_audio;
			id = pAsfHeaderInfo->streams->audio_id;
			default_control = "audio";
		}
		else {
			n  = pAsfHeaderInfo->streams->n_video;
			id = pAsfHeaderInfo->streams->video_id;
			default_control = "video";
		}

		if(n)
		{
			struct RTSP_LinkedListHdr_s *p = NULL;
			char *controlstr = NULL;

			rtsp_hdr = STREAM_RTSP_NewRtspHeaderAndFillBasicFields(rtsp_ctrl);

			STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,STRING_WMP_SUPPORTED);

			if(stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_TCP)
				STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,STRING_WMP_TRANSPORT);
			else
				{
				if(i == 0)
					STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,"Transport: RTP/AVP/UDP;unicast;client_port=7002;ssrc=75afe1e1;mode=PLAY");
				else
					STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,"Transport: RTP/AVP/UDP;unicast;client_port=7002;ssrc=75afe1e1;mode=PLAY");

				}

			/* find control for audio_id */
			for (p = sdpwms->streams ; p ; p = p->next)
			{
			struct RTSP_SDP_MS_MediaDesc_s *sdpwmsstream = p->p;
				if(sdpwmsstream->streamnum == id)
				{
				controlstr = sdpwmsstream->control;
				break;
			}
			}

			if(!controlstr)
			{
			controlstr = default_control; /* default */
			}

			if(VK_strstr(controlstr,"://"))
			{
			VK_snprintf(buffer,BUFSIZE_1K - 1,"%s",controlstr);
			}
			else
			{
			VK_snprintf(buffer,BUFSIZE_1K - 1,"%s/%s",rtsp_ctrl->mrl,controlstr);
			}

			//STREAM_RTSP_RequestSetup (rtsp_hdr,buffer);
			STREAM_RTSP_SetProtoclCmdForRtspHeader(rtsp_hdr,STREAM_RTSP_CMD_SETUP);
			STREAM_RTSP_SetUriForRtspHeader(rtsp_hdr,buffer);
			STREAM_RTSP_SerializeRtspHeader(rtsp_hdr);
			STREAM_RTSP_SendSerializedRtspRequestAndFree(stream,rtsp_hdr);

			/* receive message for SETUP */
			//ret = STREAM_RTSP_ReadRtspResponseAndFree(stream);
			{
				char* pField; /* temportal pointer */
				rtsp_hdr = STREAM_RTSP_NewRtspProtocolHeader();
				ret = STREAM_RTSP_ReadRtspHeaderAndParseBasic(stream,rtsp_hdr);
				if(ret < 0)
				{
					PrintError("STREAM_RTSP_ReadRtspHeaderAndParseBasic Error\n");
				}

				if((pField = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Transport")) != NULL)
				{
					if(!VK_strstr(pField,"RTP"))
					{
						PrintError("Server does not support RTP\n");
						ret = -1;
					}
					if(!VK_strstr(pField,"TCP"))
					{
						PrintError("Server does not support TCP\n");
						ret = -1;
					}
					if(!VK_strstr(pField,"unicast"))
					{
						PrintError("Server does not support unicast\n");
						ret = -1;
					}
					if(!VK_strstr(pField,"interleaved=0-1"))
					{
						PrintError("Server does not support interleaved=0-1 mode\n");
						ret = -1;
					}
					PrintDebug("Transport is supported\n");
				}

				if(ret < 0)
				{
					PrintDebug("Setup is failed\n");
					goto setup_failed;
				}

				STREAM_RTSP_ReadAndFreeBody(stream, rtsp_hdr);

				STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);
				rtsp_hdr = NULL;

				PrintDebug("Setup Success\n");
			}
		}
	}

setup_failed:
	if(rtsp_hdr)
	{
		STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);
	}

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return ret;
}

#if 0 /* Commented with warning: 'P_RTSP_MS_SetParameter' defined but not used */
/*
 * send SET_PARAMETER request
 * return value:  -1:failure   status code: success
 */
static HINT32 P_RTSP_MS_SetParameter(struct RTSP_ACCESS_SYS_s *stream)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->protocol.rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	 HINT32 ret = 0;
	char *buffer;
	PrintEnter;
	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	VK_memset(buffer ,0, BUFSIZE_1K);

	rtsp_hdr = STREAM_RTSP_NewRtspHeaderAndFillBasicFields(rtsp_ctrl);
	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,STRING_WMP_USERAGENT);
	//rtsp_request_set_parameter(rtsp_hdr,rtsp_ctrl->mrl);
	STREAM_RTSP_SetProtoclCmdForRtspHeader(rtsp_hdr,STREAM_RTSP_CMD_SET_PARAMETER);
	STREAM_RTSP_SetUriForRtspHeader(rtsp_hdr,rtsp_ctrl->mrl);
	STREAM_RTSP_SerializeRtspHeader(rtsp_hdr);


	STREAM_RTSP_SendSerializedRtspRequestAndFree(stream,rtsp_hdr);

	/*
	  receive message for SET_PARAMETER
	  if something needs to be done to message, use STREAM_RTSP_ReadRtspHeaderAndParseBasic
	*/
	ret = STREAM_RTSP_ReadRtspResponseAndFree(stream);

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return ret;
}
#endif /* Commented with warning: 'P_RTSP_MS_SetParameter' defined but not used */

#if 0 /* Commented with warning: 'P_RTSP_MS_Pause' defined but not used */
/*
 * send PAUSE request
 * return value:  -1:failure   status code: success
 */
static HINT32 P_RTSP_MS_Pause(struct RTSP_ACCESS_SYS_s *stream)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->protocol.rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *buffer;
	HINT32 ret = 0;
	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	VK_memset(buffer ,0, BUFSIZE_1K);

	rtsp_hdr = STREAM_RTSP_NewRtspHeaderAndFillBasicFields(rtsp_ctrl);
	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,STRING_WMP_USERAGENT);
	//rtsp_request_set_parameter(rtsp_hdr,rtsp_ctrl->mrl);
	STREAM_RTSP_SetProtoclCmdForRtspHeader(rtsp_hdr,STREAM_RTSP_CMD_PAUSE);
	STREAM_RTSP_SetUriForRtspHeader(rtsp_hdr,rtsp_ctrl->mrl);
	STREAM_RTSP_SerializeRtspHeader(rtsp_hdr);

	STREAM_RTSP_SendSerializedRtspRequestAndFree(stream,rtsp_hdr);

	/*
	  receive message for SET_PARAMETER
	  if something needs to be done to message, use STREAM_RTSP_ReadRtspHeaderAndParseBasic
	*/
	ret = STREAM_RTSP_ReadRtspResponseAndFree(stream);

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return ret;
}
#endif /* Commented with warning: 'P_RTSP_MS_Pause' defined but not used */


HINT32 RTSP_MS_ProcessRtpPacket(struct RTSP_ACCESS_SYS_s *stream, HUINT8 *buffer,size_t max_size)
{

	HINT32 ret = 0;
	HINT32 nReadCount = 0;
	HINT32 asf_packet_size = 0;
	struct RTSP_CHANNEL_t *Channel = NULL;
	struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;

	PrintEnter;

	switch(stream->dlopts->data_transport_protocol ) /* select data channel */
	{
	case RTSP_TRANSPORT_TCP:
		Channel = stream->Channel;
		break;
	case RTSP_TRANSPORT_UDP_UNICAST:
		Channel = stream->datasocket[2];
		break;
	default:
		PrintError("Un supported data transport protocol\n");
			goto process_failed;
		/*break;*/
	}



	/* interleaved_data_packet_size 는 rtp packet size와 같아야 한다. */

	/* 한개의 asf packet 을 읽어 온다. UDP인경우 한번에 읽어 Channel Buffer에 넣어둔다. */
	asf_packet_size = P_RTSP_MS_ProcessRtpPacketAndFillChannelWithAnAsfPacket(Channel, stream_ctrl->protocol.rtsp_ctrl->interleaved_data_packet_size);

	//PrintDebug( "pure Read asf_packet_size(except padding byte) = %d \n", asf_packet_size);

	/* bound check */
	if(asf_packet_size < 0 || asf_packet_size > stream_ctrl->packet_length)
	{
		PrintCriticalDebug( "Invalid asf_packet_size=[%d] expect less than [%d]\n", asf_packet_size, stream_ctrl->packet_length);
		goto process_failed;
	}
	stream_ctrl->packet_count++;


	/* RTP 에서 last packet이 오면 이곳에서 LAST_PACKET_RECEIVED 를 한다. */




	/*read_data 총합은 asf_packet_size여야한다.*/
	if((HINT32)max_size >= stream_ctrl->packet_length) /*just go to buffer*/
	{
		/* asf packet은 마지막 0들을 보내지 않는다. */
		ret = RTSP_LIB_ReadFromChannel(Channel,buffer, asf_packet_size);
		if(ret <= 0)
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
	else if((HINT32)max_size >= asf_packet_size) /* && (packet_length > max_size)   case : EOF */
	{
		ret = RTSP_LIB_ReadFromChannel(Channel,buffer, asf_packet_size);
		if(ret <= 0)
		{
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
		/*PrintCriticalDebug( " remain partial packet max size = %u, remain part = %u\n", max_size, asf_packet_size - max_size);*/
		ret = RTSP_LIB_ReadFromChannel(Channel,buffer, max_size);
		if(ret <= 0)
		{
			goto process_failed;
		}
		ret = RTSP_LIB_ReadFromChannel(Channel,stream_ctrl->write_buffer, asf_packet_size - max_size);
		if(ret <= 0)
		{
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
HINT32 P_RTSP_MS_Setup_and_get_header(struct RTSP_ACCESS_SYS_s *stream,
				  ASF_HEADER_t **pAsfHeaderInfo_ret)
{
	struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;
	struct rtsp_ctrl_t *rtsp_ctrl = stream_ctrl->protocol.rtsp_ctrl;

	ASF_HEADER_t *pAsfHeaderInfo = NULL;
	struct RTSP_MS_SdpMsCtx_s *sdpwms = NULL;

	char *description = NULL;
	HINT32 ret = 0;

	PrintEnter;

	if(stream->dlopts->bandwidth)
	{
	stream_ctrl->bandwidth = stream->dlopts->bandwidth;
	}
	else
	{
	stream_ctrl->bandwidth = INT_MAX_BANDWIDTH;
	}

	rtsp_ctrl->cseq = 1;

	/* GET SDP description*/
	ret = P_RTSP_MS_Describe(stream,&description);
	if(ret == 0)
	{
	goto failed_retry;
	}
	else if(ret < 0)
	{
	goto failed;
	}


	/* parse SDP decription and ASF header*/
	if(!P_RTSP_MS_ParseSdpAndAsfHeader(stream,description,&pAsfHeaderInfo,&sdpwms))
	{
	goto failed;
	}

	stream_ctrl->pSdpCtx = sdpwms;

	/* SETUP requests (for audio and video) */
	//if(P_RTSP_MS_Setup(stream,pAsfHeaderInfo,sdpwms) < 0) {
	//goto failed;
	//}

	/* simulate MS MP client like action with no XML data */
	//if(P_RTSP_MS_SetParameter(stream) < 0) {
	//goto failed;
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
	if(pAsfHeaderInfo)
	{
		free_pAsfHeaderInfo_t(pAsfHeaderInfo);
		pAsfHeaderInfo = NULL;
	}
	//if(sdpwms)
	//{
	//	RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);
	//	sdpwms = NULL;
	//}
	if(description)
	{
		DD_MEM_Free(description);
		description = NULL;
	}

	*pAsfHeaderInfo_ret = NULL;
	PrintExit;
	return -1;

  failed_retry: /* retry with other protocol */
	if(pAsfHeaderInfo)
	{
		free_pAsfHeaderInfo_t(pAsfHeaderInfo);
		pAsfHeaderInfo = NULL;
	}
	//if(sdpwms)
	//{
	//	RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);
	//	sdpwms = NULL;
	//}
	if(description)
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



 static HINT32 P_RTSP_MS_ProcessRtpPacketAndFillChannelWithAnAsfPacket(struct RTSP_CHANNEL_t *Channel, HINT32 interleaved_packet_size)
 {
	HINT32 i = 0;
	HINT32 ret = 0;
#if 0 /* warning: unused variable */
	HINT32 is_partial_asf_data = 0;
#endif
	HINT32 asf_data_total = 0;/* size of pure ASF packet */
	HINT32 current_asf_data_size   = 0;
	HINT32  rtp_payl_hdr_opt_size = 0;	/* how many bytes for optional field */




	/* Data Structure for RTP HEADER */
	HUINT8 rtp_header[RTP_HEADER_FIXED_SIZE]; /* RTP fixed header size */
	HUINT8 flags1 = 0;
	HINT32 rtp_h_version = 0; /* 2bit ; 현재 항상 2 */
	HINT32 rtp_h_padding = 0; /* 1bit ; 헤더에 padding 여부 */
	HINT32 rtp_h_extension = 0; /* 1bit ; 고정 헤더 외에 확장 여부 */
	HINT32 rtp_h_csrc_count = 0;		   /* 4bit; number of csrc */

	HUINT8 flags2 = 0;
	/*This bit MUST be set to 1 if the RTP packet contains the last fragment of an ASF data packet, */
	/* or one or more complete ASF data packets. Otherwise, it MUST be set to 0.*/
	HINT32 rtp_h_marker = 0; /* 1bit */
	HINT32 rtp_h_payload_type = 0; /* 7bit; 31 = H.261 */

	HUINT16 rtp_h_seqnum = 0;
	HUINT32 rtp_h_timestamp  = 0;
	HUINT32 rtp_h_SSRC_syncsrcid = 0;
	HUINT32 rtp_h_CSRC_contributingsrcid = 0;


	/* Data Structure for RTP PAYLOAD HEADER */
	HUINT8  rtp_payload_header[RTP_PAYLOAD_HEADER_FIXED_SIZE];
	HINT32 rtp_payload_h_S_flag_keyframe = 0;
	HINT32 rtp_payload_h_L_flag_length_or_offset= 0;
	HINT32 rtp_payload_h_R_flag_relative_timestamp  = 0;
	HINT32 rtp_payload_h_D_flag_duration = 0;
	HINT32 rtp_payload_h_I_flag_location_id = 0;
	HINT32 rtp_payload_h_length_or_offset = 0;

	HUINT32 rtp_payload_h_relative_timestamp = 0;
	HUINT32 rtp_payload_h_duration = 0;
	HUINT32 rtp_payload_h_location_id = 0;


	HUINT8* completed_asf_packet_from_udp = NULL;
	HINT32 flush_this_packet = 0;

	PrintEnter;

	if(Channel->is_udp){
		completed_asf_packet_from_udp = (HUINT8 *)DD_MEM_Alloc(20000);/* twice LARGER than MAX ASF PACKET SIZE */
		VK_memset(completed_asf_packet_from_udp ,0, 20000);
	}


	while(TRUE) /* break with rtp_h_marker  <- UDP모드에서 packet이 조각으로 들어올대 complete flag */
	{
		rtp_payl_hdr_opt_size = 0;

		if(Channel->is_udp)
		{
			RTSP_LIB_ReadADgramAndFillChannelBuffer(Channel);
		}
		current_asf_data_size = 0;

		ret = RTSP_LIB_ReadFromChannel(Channel, rtp_header, RTP_HEADER_FIXED_SIZE);
		if (ret <= 0){PrintCriticalDebug( "Cannot read RTP data from sock\n");}

		//RTSP_LIB_DumpByteBuffer(rtp_header, RTP_HEADER_FIXED_SIZE);



		/* Parse RTP Header */
		flags1 = rtp_header[0];
		flags2 = rtp_header[1];

		rtp_h_version = ((flags1 & 0xC0) >> 6); 	/* 11000000 */
		rtp_h_padding = ((flags1 & 0x20) >> 5); 	/* 00100000 */
		rtp_h_extension = ((flags1 & 0x10) >> 4); /* 00010000 */
		rtp_h_csrc_count = (flags1 & 0x0f); 			/* 00001111 */

		rtp_h_marker = ((flags2 & 0x80) >> 7); 	/* 10000000 */
		rtp_h_payload_type = (flags2 & 0x7f); 	/* 01111111 */

		rtp_h_seqnum = get16_be((HUINT8 *)rtp_header+2);
		rtp_h_timestamp = get32_be((HUINT8 *)rtp_header+4);
		rtp_h_SSRC_syncsrcid = get32_be((HUINT8 *)rtp_header+8);

		current_asf_data_size -= RTP_HEADER_FIXED_SIZE;

		PrintDebug( "RTP HEADER Ver=%d Pad=%d Ext=%d CSRC=%d M=%d PaylType=%d SeqN = %hu, TimeS=%u, SSRC = %u\n",rtp_h_version,	rtp_h_padding,
				rtp_h_extension,rtp_h_csrc_count, rtp_h_marker, rtp_h_payload_type, rtp_h_seqnum, rtp_h_timestamp, rtp_h_SSRC_syncsrcid);


		if(Channel->prev_rtp_seq_num != rtp_h_seqnum - 1 && Channel->prev_rtp_seq_num != 0)
		{
				PrintCriticalDebug( " ERROR !!! seq mismatch !!!%d, expect %d\n", (int)rtp_h_seqnum, Channel->prev_rtp_seq_num + 1);
				flush_this_packet = 1;

				PrintCriticalDebug( "RTP HEADER Ver=%d Pad=%d Ext=%d CSRC=%d M=%d PaylType=%d SeqN = %hu, TimeS=%u, SSRC = %u\n",rtp_h_version,	rtp_h_padding,
				rtp_h_extension,rtp_h_csrc_count, rtp_h_marker, rtp_h_payload_type, rtp_h_seqnum, rtp_h_timestamp, rtp_h_SSRC_syncsrcid);
		}

		Channel->prev_rtp_seq_num = rtp_h_seqnum;


		/* redundant CSCR field */
		for(i = 0 ; i < rtp_h_csrc_count ; i++ ) {

			RTSP_LIB_ReadFromChannel(Channel,(HUINT8 *)&rtp_h_CSRC_contributingsrcid,4);
			rtp_h_CSRC_contributingsrcid = be2me_32(rtp_h_CSRC_contributingsrcid);
			PrintDebug( "CSCR = %u\n", rtp_h_CSRC_contributingsrcid);
			current_asf_data_size -= 4;
		}


		/* FUTURE: Parse FEC Header */




		/* Parse RTP Payload Header */
		RTSP_LIB_ReadFromChannel(Channel,rtp_payload_header,RTP_PAYLOAD_HEADER_FIXED_SIZE); /* get RTP Payload Format Header */
		//RTSP_LIB_DumpByteBuffer(rtp_payload_header, RTP_PAYLOAD_HEADER_FIXED_SIZE);

		if(rtp_payload_header[0] & 0x80) { /* S bit : ASF Key Frame *//* 10000000 */
			rtp_payload_h_S_flag_keyframe = 1;
		}
		if(rtp_payload_header[0] & 0x40) { /* L bit : 0->offset		 1->Length*//* 01000000 */
			rtp_payload_h_L_flag_length_or_offset= 1;
		}
		rtp_payload_h_length_or_offset = (rtp_payload_header[1] << 16) + (rtp_payload_header[2] << 8) + rtp_payload_header[3];




		if(rtp_payload_header[0] & 0x20) { /* R : Relative Timestamp (optional) present *//* 00100000 */
			rtp_payload_h_R_flag_relative_timestamp  = 1;
			rtp_payl_hdr_opt_size += 4;
		}
		if(rtp_payload_header[0] & 0x10) { /* D : Duration (optional) present   */ /* 00010000 */
			rtp_payload_h_D_flag_duration = 1;
			rtp_payl_hdr_opt_size += 4;
		}
		if(rtp_payload_header[0] & 0x08) { /* I : LocationId (optional) present *//* 00001000 */
			rtp_payload_h_I_flag_location_id = 1;
			rtp_payl_hdr_opt_size += 4;
		}


		PrintDebug( "RTP Payload Hdr S=%d, L=%d, R=%d, D=%d, I=%d Length or Offset = %d\n", rtp_payload_h_S_flag_keyframe, rtp_payload_h_L_flag_length_or_offset,
			rtp_payload_h_R_flag_relative_timestamp, rtp_payload_h_D_flag_duration, rtp_payload_h_I_flag_location_id, rtp_payload_h_length_or_offset);


		/* Parse (Optional) RTP Payload Header */
		if(rtp_payl_hdr_opt_size > 0) {
			char optional_fields[16]; /* maximum 12 bytes */
			HINT32 option_offset = 0;

			RTSP_LIB_ReadFromChannel(Channel,optional_fields,rtp_payl_hdr_opt_size);


			if(rtp_payload_h_R_flag_relative_timestamp)
			{
				rtp_payload_h_relative_timestamp = get32_be((HUINT8
				*)optional_fields+option_offset);
				PrintDebug( "Relative Timestamp = %u\n", rtp_payload_h_relative_timestamp);
				option_offset += 4;
			}
			if(rtp_payload_h_D_flag_duration)
			{
				rtp_payload_h_duration = get32_be((HUINT8
				*)optional_fields+option_offset);
				PrintDebug( "Duration = %u\n", rtp_payload_h_duration);
				option_offset += 4;
			}
			if(rtp_payload_h_I_flag_location_id)
			{
				rtp_payload_h_location_id = get32_be((HUINT8
				*)optional_fields+option_offset);
				PrintDebug( "Location_id = %u\n", rtp_payload_h_location_id);
			}

		}
		current_asf_data_size -= RTP_PAYLOAD_HEADER_FIXED_SIZE + rtp_payl_hdr_opt_size;


		if(rtp_payload_h_L_flag_length_or_offset == 1) /* length */
		{
			interleaved_packet_size += current_asf_data_size;

			/* FUTURE identify why -4(fixed payload header size)? in the spec */
			current_asf_data_size = rtp_payload_h_length_or_offset - 4 - rtp_payl_hdr_opt_size;

			/* error check */
			if(interleaved_packet_size != current_asf_data_size)
			{
				PrintCriticalDebug( "RTP size %d and Interleved size %d mismatch\n", current_asf_data_size, interleaved_packet_size);

				PrintCriticalDebug( "RTP Payload Hdr S=%d, L=%d, R=%d, D=%d, I=%d Length or Offset = %d\n", rtp_payload_h_S_flag_keyframe, rtp_payload_h_L_flag_length_or_offset,
			rtp_payload_h_R_flag_relative_timestamp, rtp_payload_h_D_flag_duration, rtp_payload_h_I_flag_location_id, rtp_payload_h_length_or_offset);
			}
		}
		else /* offset */
		{
			current_asf_data_size = Channel->dgram_size + current_asf_data_size;
		}



		if(Channel->is_udp)
		{

			/* FUTURE : Forward Error Correction Routine */
			/* input : FEC pasring info, data chunk   output :
			completed_asf_packet_from_udp */

			/* FUTURE : clear me */
			RTSP_LIB_ReadFromChannel(Channel,completed_asf_packet_from_udp +
			asf_data_total, current_asf_data_size); /* just add up */


		}

		asf_data_total += current_asf_data_size;

		if(rtp_h_marker) /* EOF asf complete packet ? */
		{
			if(Channel->is_udp)
			{
				/* FUTURE : GET FEC Result */
				/* param [out] : Channel buffer, sizeof buffer result : data_len
				code */
				if(flush_this_packet == 0)
				{
				VK_memcpy(Channel->buffer, completed_asf_packet_from_udp, asf_data_total);
				Channel->data_len = asf_data_total;
				Channel->buffer_pos = 0;
				}
				else
				{ /* flush invalid packet */
					Channel->data_len = 0;
					Channel->buffer_pos = 0;
					asf_data_total = 0;
				}
			}

			break;
		}


	}

	if(Channel->is_udp){
		if(completed_asf_packet_from_udp)
		{
			DD_MEM_Free(completed_asf_packet_from_udp);/* twice LARGER than MAX ASF PACKET SIZE */
			completed_asf_packet_from_udp = NULL;
		}
	}

	ret = asf_data_total;

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

	if(!stream || !sdpstr) { /* stream could not be NULL */
	goto failed;
	}

	/* parse sdp and get information about file to download */
	sdpwms = RTSP_SDP_MS_ParseSdp(sdpstr);
	if(sdpwms->nRawAsfHeaderLen <= (HINT32)sizeof(struct asf_header_t)) {
	PrintCriticalDebug("asf header smaller than asf_header_t\n");
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
	if(pAsfHeaderInfo) free_pAsfHeaderInfo_t(pAsfHeaderInfo);
	if(sdpwms) RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);
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

	if(stream->bInterleavedDataLefted == TRUE)
	{
		goto success;
	}

	ret = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,interleaved_p_hdr,INTERLEAVED_PACK_HEADER_SIZE);

	if(ret < INTERLEAVED_PACK_HEADER_SIZE) {
	goto failed;
	}

	if(interleaved_p_hdr[0] == 0x24)
		goto interleaved_packet;

	if(!VK_strncasecmp((char *)interleaved_p_hdr,"SET_",INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"GET_",INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"OPTI",INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"RTSP",INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"ANNO",INTERLEAVED_PACK_HEADER_SIZE))
	  {

	RTSP_LIB_PutDataForLaterRead(stream,interleaved_p_hdr,INTERLEAVED_PACK_HEADER_SIZE); /* push back interleaved_p_hdr */

	ret = RTSP_MS_ProcessRtspResponse(stream);
	if(ret < 0) {
		goto failed;
	}
	nStatusCode = ret;

	if(ret == 0 && stream_ctrl->status == RTSP_STREAMING_FINISHED) {
		/* streaming finished (at least it says) */

		/*
		  wait for 2 seconds.. this is necessary becasue
		  otherwise it would block when waiting "ANNOUNCE"
		*/
		if(RTSP_LIB_SelectCtrlChSocket(stream,2) > 0)
		{
			/* Fixed prevent-CID-15513: Resource leak (RESOURCE_LEAK) */
			/* Removed unused pAsfHeaderInfo pointer */
			/* http://10.0.0.92:5467/sourcebrowser.htm?projectId=10014#mergedDefectId=15513&defectInstanceId=269909&fileInstanceId=533878 */
			/* ASF_HEADER_t *pAsfHeaderInfo = NULL; */

			if ((/*pAsfHeaderInfo = */ P_RTSP_MS_AnnounceContinue(stream)) != NULL)
			{
				PrintDebug("streaming still continues!!\n");

				stream->b_feed_asfheader_first = TRUE;

				/*free_pAsfHeaderInfo_t(pAsfHeaderInfo); this is done in STREAM_RTSP_FreeRtspCtrl */
				stream_ctrl->status = RTSP_STREAMING_REWIND;
				return 0; /* write asf header */
			}
		}

		/* if no ANNOUCNE, finish streaming */
		PrintDebug("finish rtsp streaming\n");
		return 0; /* streaming really finished */
	}
#if 0
	else
	{
		/* streaming still continues */
		ret = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,interleaved_p_hdr,INTERLEAVED_PACK_HEADER_SIZE);
		if(ret < INTERLEAVED_PACK_HEADER_SIZE) {
		goto failed;
		}
	}
#endif
	}
	goto success;

	interleaved_packet:

	/* RTSP interleaved frame */ /* 32 bit is interleved frame's interleaved_p_hdr */
		if(interleaved_p_hdr[0] != 0x24) {
		PrintCriticalDebug("wrong rdt/rtp magic : [0x%02x]\n",interleaved_p_hdr[0]);
		/* dump interleaved_p_hdr*/
		RTSP_LIB_DumpByteBuffer(interleaved_p_hdr,INTERLEAVED_PACK_HEADER_SIZE);
		PrintDebug("\n");
		goto failed;
		}
		stream_ctrl->protocol.rtsp_ctrl->interleaved_data_packet_size = (interleaved_p_hdr[1] << 16) + (interleaved_p_hdr[2] << 8) + interleaved_p_hdr[3];
		/*PrintDebug( "interleaved_data_packet_size = %d\n", stream_ctrl->protocol.rtsp_ctrl->interleaved_data_packet_size);*/

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
static ASF_HEADER_t *P_RTSP_MS_AnnounceContinue(struct RTSP_ACCESS_SYS_s *stream)
{
	struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;
	struct rtsp_ctrl_t *rtsp_ctrl = stream_ctrl->protocol.rtsp_ctrl;

	HINT32 i = 0;
	char checkbuf[10];			  		/* check whether the message is ANNOUCNE */
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	HINT32 got_cseq = 0;			   	/* cseq receivedi */
	HINT32 status_code = 0;				/* rtsp_hdr status value */
	char *pField = NULL;			 	/* filed pointer for rtsp_hdr   */
	HINT32 len = 0;						/* length of description (content length) */
	char *description = NULL;	   		/* description pointer, data of rtsp-packet	*/
	char *buffer = NULL;			/* temp buffer for building req */

	struct RTSP_MS_SdpMsCtx_s *sdpwms = NULL;
	ASF_HEADER_t* pAsfHeaderInfo = NULL;

	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	if(buffer == NULL)
	{
		PrintError("mem alloc error \n");
		goto announce_failed;
	}

	VK_memset(buffer ,0, BUFSIZE_1K);

	i = RTSP_LIB_ReadFromCtrlChannelWithNoWait(stream,checkbuf,8);

	if (i < 8 || VK_memcmp(checkbuf,"ANNOUNCE",8))
	{
		goto announce_failed;
	}

	RTSP_LIB_PutDataForLaterRead(stream,checkbuf,i); /* push back "ANNOUNCE" */

	rtsp_hdr = STREAM_RTSP_NewRtspProtocolHeader();

	STREAM_RTSP_ReadRtspHeaderAndParseBasic(stream,rtsp_hdr);

	status_code = rtsp_hdr->status_code;

	/* to parse Content-Length Field */
	if ((pField = STREAM_RTSP_FindFieldFromRtspHeader (rtsp_hdr,"Content-Length")) != NULL)
	{
		while(*pField == ' ')
		{
			pField++;
	}

		len = VK_atoi (pField);
	}

	/* to parse Cseq Field */
	if ((pField = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Cseq")) != NULL)
	{
		while(*pField == ' ')
		{
			pField++;
		}
		got_cseq = VK_atoi(pField);
	}

	/* copy sdp fields */
	description = DD_MEM_Alloc(len + 1);

	VK_memset(description ,0, len + 1);

	len = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,description,len);
	description[len] = '\0';

	STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);

	if(status_code != RTSP_STATUS_ANNOUNCE)
	{
		goto announce_failed;
	}

	/* got annouce packet, and data is in description */
	STREAM_RTSP_SendRtspOk(stream,got_cseq,rtsp_ctrl->session); /* OK (required, according to RFC )*/


	(void) P_RTSP_MS_ParseSdpAndAsfHeader (stream,description,&pAsfHeaderInfo,&sdpwms);

	/* Be careful with Cseq: */

	/* SETUP */
	if(P_RTSP_MS_Setup(stream,pAsfHeaderInfo,sdpwms) < 0)
	{
		goto announce_failed;
	}

	/* send SET_PARAMETER request to download stream */
#if 0
	if(P_RTSP_MS_SetParameter(stream) < 0)
	{
		goto announce_failed;
	}
#endif

	/*  send PLAY request to download stream */
	if(RTSP_MS_Play(stream) < 0)
	{
		goto announce_failed;
	}

	/*
	 * here do NOT need to write pAsfHeaderInfo or those stuff
	 * those are already done in very first protocol initiation.
	 */


	RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);
	DD_MEM_Free(description);
	description = NULL;
	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return pAsfHeaderInfo;


  announce_failed:
	if(description)
	{
		DD_MEM_Free(description);
		description = NULL;
	}
	if(sdpwms)
	{
		RTSP_SDP_MS_FreeSdpMsCtx(sdpwms);
	}
	if(pAsfHeaderInfo)
	{
		free_pAsfHeaderInfo_t(pAsfHeaderInfo);
	}
	if(buffer)
	{
		DD_MEM_Free(buffer);
		buffer = NULL;
	}
	PrintExit;
	return NULL;

}



HINT32 RTSP_MS_AsyncPlay(struct RTSP_ACCESS_SYS_s *stream, float scale)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->protocol.rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *buffer;
	HINT32 ret = 0;
	HINT32 bandwidth_to_send = 0;

	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	VK_memset(buffer ,0, BUFSIZE_1K);

	/* Sending part */
	rtsp_hdr = STREAM_RTSP_NewRtspHeaderAndFillBasicFields(rtsp_ctrl);
	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,STRING_WMP_USERAGENT);


#if 0
	if(stream->dlopts->resume_download) {
	RTSP_MS_prepare_resuming(stream);
	}
#endif


	STREAM_RTSP_SetRangeField(rtsp_hdr,stream->dlopts->range);
	STREAM_RTSP_SetSpeedField(rtsp_hdr,stream->dlopts->speed);

	/* bandwidth specified */
	bandwidth_to_send =
	(stream->dlopts->bandwidth) ? (stream->dlopts->bandwidth) : INT_MAX_BANDWIDTH;

	VK_snprintf(buffer,BUFSIZE_1K - 1,"Bandwidth: %d",bandwidth_to_send);
	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,buffer);

	//if(!(scale > 0.9 && scale < 1.1))
	//{
	VK_snprintf(buffer,BUFSIZE_1K - 1,"Scale: %.3f", scale);
	PrintDebug("Scale String is %s\n", buffer);
	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,buffer);
	//}
	VK_snprintf(buffer,BUFSIZE_1K - 1,
		 "X-Accelerate-Streaming: AccelDuration = 18000;AccelBandwidth=%d",bandwidth_to_send);
	STREAM_RTSP_SetFieldForRtspHeader(rtsp_hdr,buffer);

	//rtsp_request_play(rtsp_hdr,rtsp_ctrl->mrl);
	STREAM_RTSP_SetProtoclCmdForRtspHeader(rtsp_hdr,STREAM_RTSP_CMD_PLAY);
	STREAM_RTSP_SetUriForRtspHeader(rtsp_hdr,rtsp_ctrl->mrl);
	STREAM_RTSP_SerializeRtspHeader(rtsp_hdr);

	STREAM_RTSP_SendSerializedRtspRequestAndFree(stream,rtsp_hdr);

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
HINT32 RTSP_MS_Play(struct RTSP_ACCESS_SYS_s *stream)
{
		struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *pField = NULL;
	HINT32 ret = 0;

	PrintEnter;

	/* Sending part */
	RTSP_MS_AsyncPlay(stream, 1.000);

	/* Receiving part */

	/* receive message for PLAY request */
	rtsp_hdr = STREAM_RTSP_NewRtspProtocolHeader();
	ret = STREAM_RTSP_ReadRtspHeaderAndParseBasic(stream,rtsp_hdr);

	if(!STREAM_RTSP_CheckRtspResponseOk(ret))
	{
	PrintCriticalDebug("PLAY request returned: %d (%s)\n",
		rtsp_hdr->status_code,rtsp_hdr->reason_phrase);

		pField = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Alert");
		if(pField)
		{
			while(*pField == ' ')
			{
				pField++;
			}
			PrintCriticalDebug("message from server --> %s\n",pField);
	}
	STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);
	goto failed;
	}

	/* display real speed (might differ from user requested) */
	if((pField = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Speed")) != NULL)
	{
		if(stream->dlopts->speed)
		{
			while(*pField == ' ')
			{
				pField++;
			}
			PrintData("Speed: %s\n",pField);
	}
	}

	if((pField = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Range")) != NULL)
	{
		if(stream->dlopts->range)
		{
			while(*pField == ' ')
			{
				pField++;
			}
			PrintData("Range: %s\n",pField);
	}
	}

	/* skip content-length bytes from network */
	STREAM_RTSP_ReadAndFreeBody(stream,rtsp_hdr);
	STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);

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
HINT32 RTSP_MS_StartDataChannelIfNotStarted(struct RTSP_ACCESS_SYS_s *stream)
{
	HINT32 ret = 0;
	struct RTSP_CHANNEL_t * pDataChannel;
	PrintEnter;


	/* select data channel */
	if(stream->dlopts->data_transport_protocol  ==  RTSP_TRANSPORT_TCP)
	{
		pDataChannel = stream->Channel;
	}
	else
	{
		pDataChannel = stream->datasocket[2];
	}

	/* start the channel */
	if(pDataChannel->is_started == FALSE)
	{
		if(P_RTSP_MS_Setup(stream,stream->stream_ctrl->protocol.rtsp_ctrl->asf.pAsfHeaderInfo, stream->stream_ctrl->pSdpCtx) < 0)
		{
			goto failed;
		}

		if(RTSP_MS_Play(stream) < 0)
		{
			goto failed;
		}
		pDataChannel->is_started = TRUE;
		stream->stream_ctrl->status = RTSP_STREAMING_DOWNLOADING;
	}

	PrintExit;
	return ret;

  failed:
  	PrintExit;
	return -1;

}

/*
 * process rtsp header coming among rtsp wmserver media packet
 *
 * return value:	   status code ... normal proceed case
 *							   0 ... EOF packet
 *							  -1 ... error
 */
HINT32 RTSP_MS_ProcessRtspResponse(struct RTSP_ACCESS_SYS_s *stream)
{
	struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;
	struct rtsp_ctrl_t *rtsp_ctrl = stream_ctrl->protocol.rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	HINT32 status_code = 0;
	size_t bodylen = 0;
	HUINT8 *body = NULL;
	char *pField = NULL;
	HINT32 got_cseq = 0;

	PrintEnter;

	/* push back 4 bytes */

	rtsp_hdr = STREAM_RTSP_NewRtspProtocolHeader();
	if(rtsp_hdr == NULL)
	{
		PrintError("STREAM_RTSP_NewRtspProtocolHeader error\n");
		goto failed;
	}
	status_code = STREAM_RTSP_ReadRtspHeaderAndParseBasic(stream,rtsp_hdr);
	if(status_code < 0) {
	goto failed;
	}

	/* get content length data */
	if((pField = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Content-Length")) != NULL)
	{
		while (*pField == ' ')
		{
			pField++;
		}
		bodylen = VK_atoi(pField);
	body = DD_MEM_Alloc(bodylen);
	VK_memset(body ,0, bodylen);

	bodylen = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,body,bodylen); /* read rest of data */
	}

	if((pField = STREAM_RTSP_FindFieldFromRtspHeader(rtsp_hdr,"Cseq")) != NULL) {
		while(*pField == ' ') pField++;
		got_cseq = VK_atoi(pField);
	}

	if(!VK_strncasecmp(rtsp_hdr->protocol,"RTSP",4)) {
	/* RTSP/1.0 200 OK ... ignore this */

	if(got_cseq != (HINT32)rtsp_ctrl->cseq - 1) { /* rtsp_ctrl->cseq increment in send */
		PrintDebug("CSeq mismatch: expected %d, got %d",
			rtsp_ctrl->cseq - 1,got_cseq);
	}
	/* DO NOT send OK back to this */
	}
	else if(!VK_strncasecmp(rtsp_hdr->protocol,"SET_PARAMETER",13)) { /* SET_PARAMETER */
	char *p;

	/* EOF 는 SET_PARAMETER로 온다. 이후 서버측에서 소켓을 닫는다. */
	/* 닫힌 소켓을 사용하면 131 sock error, 비정상 종료 현상이 나타날 수 잇다. */

	/* MS-RTSP X-Notice(with End-of-Stream Reached), X-RTP-Info(with last RTP seqN), */
	/* X-Playlist-Gen-Id 가 오면 Body를 보고 세부 판단을 한다. */
	if(body && (p = VK_strstr((char *)body,"EOF:"))) { // body must exist <<- fix 1.2.2
		p += 4;
		while(*p == ' ') p++;
		if((!VK_strncasecmp(p,"true",4))) { /* EOF */

				stream_ctrl->status = RTSP_STREAMING_FINISHED;

				PrintDebug("=======================RTSP_STREAMING_FINISHED=========================\n");
		if(stream->p_keepalive_thread)
		{
			VK_TASK_Stop(stream->p_keepalive_thread->thread_id);
			VK_TASK_Destroy(stream->p_keepalive_thread->thread_id);
			DD_MEM_Free(stream->p_keepalive_thread);
			stream->p_keepalive_thread = NULL;
		}

		DD_MEM_Free(body);
		body = NULL;
		STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);
		return 0;
		}
	}
	STREAM_RTSP_SendRtspOk(stream,got_cseq,rtsp_ctrl->session);
	}
	else {
	/* everything else ... just send OK */
	STREAM_RTSP_SendRtspOk(stream,got_cseq,rtsp_ctrl->session);
	}


	if(body)
	{
		DD_MEM_Free(body);
		body = NULL;
	}
	if(rtsp_hdr)
	{
		STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);
	}
	PrintExit;
	return status_code;

  failed:
	if(body)
	{
		DD_MEM_Free(body);
		body = NULL;
	}
	if(rtsp_hdr)
	{
		STREAM_RTSP_FreeRtspProtoclHeader(rtsp_hdr);
	}
	PrintExit;
	return -1;
}


