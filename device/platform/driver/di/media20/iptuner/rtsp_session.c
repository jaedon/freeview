/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   rtsp_session.c  $
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
#include <errno.h>
#include "di_iptuner_config.h"
#include "di_uart.h"
#include "stream_global.h"
#include "di_iptuner20.h"
#include "util_url.h"
#include "stream_rtsp.h"
#include "util_b64_encode.h"
#include "rtsp_lib.h"
#include "rtsp_session.h"
#include "rtsp_sdp.h"
#include "transport_udp.h"
#include "transport_rtp.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define RTSP_SESSION_DEBUG
//#define RTSP_SESSION_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
  #ifdef RTSP_SESSION_DEBUG
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

#ifdef RTSP_SESSION_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((HINT32 (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)


#define _INTERLEAVED_PACK_HEADER_SIZE 4
#define _RTP_HEADER_FIXED_SIZE 12
#define _RTP_PAYLOAD_HEADER_FIXED_SIZE 4

#define _DEF_RTSP_HDR_ACCEPT_SDP "Accept: application/sdp"
#define _DEF_RTSP_HDR_CONTENT_LENGTH "Content-length"
#define _DEF_RTSP_HDR_CONTENT_TYPE "Content-Type"
#define _DEF_RTSP_HDR_APPLICATION_SDP "application/sdp"
#define _DEF_RTSP_HDR_RANGE "Range: "
#define _DEF_RTSP_HDR_NPT_NOW "npt=now-"
#define _DEF_RTSP_HDR_MEDIA_CONTAINER_MPEG_TS "33"

/* Transport field after SETUP */
/* Transport: RTP/AVP;unicast;destination=10.0.13.119;source=10.0.13.142;client_port=7002-7003;server_port=6970-6971 */
#define _DEF_RTSP_HDR_TRANSPORT              "Transport"
#define _DEF_RTSP_HDR_SESSION                "Session"
#define _DEF_RTSP_HDR_TRANSPORT_REQUEST      "Transport: RTP/AVP;%s;%s%i-%i;mode=\"PLAY\""
#define _DEF_RTSP_HDR_TRANSPORT_MULTICAST    "multicast"
#define _DEF_RTSP_HDR_TRANSPORT_UNICAST      "unicast"
#define _DEF_RTSP_HDR_MULTICAST_PORT         "port="
#define _DEF_RTSP_HDR_UNICAST_CLIENT_PORT    "client_port="
#define _DEF_RTSP_HDR_UNICAST_SERVER_PORT    "server_port="
#define _DEF_RTSP_HDR_TRANSPORT_DESTINATION  "destination="
#define _DEF_RTSP_HDR_TRANSPORT_SOURCE       "source="

/* RTP-Info field after PLAY */
/* RTP-Info: url=rtsp://10.0.13.142:8554/pub/_spiderman.ts/track1;seq=58343;rtptime=4035134626 */
#define _DEF_RTSP_HDR_RTPINFO                "RTP-Info"
#define _DEF_RTSP_HDR_RTPINFO_URL            "url="
#define _DEF_RTSP_HDR_RTPINFO_SEQ            "seq="
#define _DEF_RTSP_HDR_RTPINFO_RTPTIME        "rtptime="

/***************************************************************
 * RTSP Strings for the interaction between player and server
 ***************************************************************/
static const char s_szRTSPUserAgent[] = "User-Agent: RTSP-Client/9.0.0.2833 guid/B64345F5-8C45-4818-8A1A-4775F0923FAC";

/* transport-protocol = "RTP" | "MP2T" */
/* profile = "AVP" | "H2221" | "DVBC" */
/* lower-transport = "TCP" | "UDP" | "AAL5-PVC" | "AAL5-SVC" | "ASI" | "QAM" */
static const char s_szRTSPTransportInterleaved[]  = "Transport: RTP/AVP/TCP;unicast;interleaved=0-1;ssrc=75afe1e1;mode=PLAY";
static const char s_szRTSPTransportRtp[]          = "Transport: RTP/AVP/UDP;unicast;client_port=%d";
static const char s_szRTSPTransportRtpRange[]     = "Transport: RTP/AVP/UDP;unicast;client_port=%d-%d";

/*
 * There is no single defined standard for how to request raw UDP streaming using RTSP;
 * the IETF standard media streaming protocol is RTP.
 * However, clients that wish to request raw UDP streaming typically do so in one of two ways:
 * 1/ By specifying a protocol of "RAW/RAW/UDP" (or perhaps "MP2T/H2221/UDP") in the "Transport:" RTSP header, or
 * 2/ Specifying only a single client port in the "client_port=" part of the "Transport:" header.
 */
static const char s_szRTSPTransportRawUdp[]       = "Transport: RAW/RAW/UDP;unicast;client_port=%d";
static const char s_szRTSPTransportMp2tH2221Udp[] = "Transport: MP2T/H2221/UDP;unicast";
static const char s_szRTSPTransportMp2tAvpUdp[]   = "Transport: MP2T/AVP/UDP;unicast";


static HBOOL P_RTSP_SESSION_ParseSDP(char *pSdpString, struct RTSP_SDP_Context_s **ppSdpCtx_out);
static HBOOL P_RTSP_SESSION_AnnounceContinue(struct RTSP_ACCESS_SYS_s *stream);
static HINT32 P_RTSP_SESSION_ProcessMessageResponse(struct RTSP_ACCESS_SYS_s *stream);
static HINT32 P_RTSP_SESSION_RequestPlay(struct RTSP_ACCESS_SYS_s *stream);

/*
 * DESCRIBE Msg.
 * RETURN values
 * 	-1: failed
 * 	 1: success
 * 	 0: retry with other protocol
 */
static HINT32 P_RTSP_SESSION_RequestDescribe(struct RTSP_ACCESS_SYS_s *stream,char **ppDescription)
{
	struct rtsp_ctrl_t *rtsp_ctrl = NULL;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *p_sdpString = NULL;
	HINT32 nret = 0;
	char *pField = 0;
	HINT32 i_content_length = 0;
	HINT32 i_read = 0;

	PrintEnter;

	if ((stream == NULL) || (stream->stream_ctrl == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n",__FUNCTION__, __LINE__);
		goto func_retry_failed;
	}

	rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl; /* <-- */
	if (rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp_ctrl!\n",__FUNCTION__, __LINE__);
		goto func_retry_failed;
	}

	/* send DESCRIBE request */
	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get new basic message\n", __FUNCTION__, __LINE__);
		goto func_retry_failed;
	}

	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, s_szRTSPUserAgent);
	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_DESCRIBE);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,rtsp_ctrl->p_mrl);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);
	STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr); /* Also, free() */

	/* receive reply for DESCRIBE */
	/* rtsp_hdr is already cleared at above RequestMessageAndFree() */
	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get new message header\n", __FUNCTION__, __LINE__);
		goto func_retry_failed;
	}

	nret  = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);
	if (nret < 0)
	{
		PrintError("%s(%d) Error> Can't read response header(%d)\n", __FUNCTION__, __LINE__, nret);
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		goto func_retry_failed;
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
			PrintError("%s(%d) message from server --> field:%s\n", __FUNCTION__, __LINE__, pField);
		}
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		goto func_error;
	}

	pField = STREAM_RTSP_FindMessageHeaderField (rtsp_hdr, _DEF_RTSP_HDR_CONTENT_TYPE);
	if (!pField)
	{
		PrintError("%s(%d) Error> Can't get application/sdp field\n", __FUNCTION__, __LINE__, nret);
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		goto func_error;
	}
	if (!VK_strstr (pField, _DEF_RTSP_HDR_APPLICATION_SDP))
	{
		PrintError("%s(%d) Error> Unexpected application/sdp field(%s)\n", __FUNCTION__, __LINE__, pField);
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		goto func_error;
	}

	/* Content-length must be present */
	i_content_length = 0;
	if ((pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Content-Length")) != NULL)
	{
		while(*pField == ' ')
		{
			pField++;
		}
		i_content_length = VK_atoi(pField);
	}
	else  /* no Content-length */
	{
		PrintError("%s(%d) Error(warning)> No Content-Length in fields!!\n", __FUNCTION__, __LINE__);
	}

	/* in STD-RTSP Etag is not necessary */
	/*
	if ((pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"ETag")) == NULL)
	{
		PrintDebug("%s(%d) warning: No ETag!!\n", __FUNCTION__, __LINE__);
	}
	*/

	STREAM_RTSP_FreeMessageHeader(rtsp_hdr);

	p_sdpString = (char *)DD_MEM_Alloc(i_content_length + 1);
	if (p_sdpString == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(p_sdpString ,0, i_content_length + 1);
	i_read = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,(HUINT8 *)p_sdpString,i_content_length);
	p_sdpString[i_read] = '\0';

	PrintDebug("%s(%d) SDP data =================\n%s\n=(%d bytes)========desc=\n",
			__FUNCTION__, __LINE__,
			p_sdpString,(int)VK_strlen(p_sdpString));
	*ppDescription = p_sdpString;
	PrintExit;
	return 1;

func_error:
	if (p_sdpString != NULL)
	{
		DD_MEM_Free(p_sdpString);
		p_sdpString = NULL;
	}
	*ppDescription = NULL;
	PrintExit;
	return -1;

func_retry_failed:
	if (p_sdpString != NULL)
	{
		DD_MEM_Free(p_sdpString);
		p_sdpString = NULL;
	}
	*ppDescription = NULL;
	PrintExit;
	return 0;
}


/* client_port and server_port */
static HINT32 P_RTSP_SESSION_ParseTransportPort (const char *p_responseLine, const char *p_targetField, int *p_uPort, int *p_uRtcpPort)
{
	/* _DEF_RTSP_HDR_UNICAST_CLIENT_PORT "client_port=" */
	/* _DEF_RTSP_HDR_UNICAST_SERVER_PORT "server_port=" */
	char *ptr_1st = NULL;
	char *ptr_2nd = NULL;
	char *ptr_3rd = NULL;
	char *p_lineCopy = NULL;
	HBOOL isRanged = TRUE; /* Assumed with non-single port format */

	*p_uPort = 0;
	*p_uRtcpPort = 0;

	if ((p_responseLine == NULL) || (p_targetField == NULL))
	{
		goto func_error;
	}
	p_lineCopy = VK_StrDup ((char *)p_responseLine);
	if (p_lineCopy == NULL)
	{
		goto func_error;
	}

	/*
	 *      C->M: SETUP rtsp://foo/twister/audio RTSP/1.0
	 *            CSeq: 2
	 *            Transport: RTP/AVP;unicast;client_port=8000-8001
	 *
	 *      M->C: RTSP/1.0 200 OK
	 *            CSeq: 2
	 *            Transport: RTP/AVP;unicast;client_port=8000-8001;
	 *                       server_port=9000-9001
	 *            Session: 12345678
	 */

	ptr_1st = VK_strstr (p_lineCopy, p_targetField);
	if (ptr_1st == NULL)
	{
		goto func_error;
	}

	ptr_2nd = VK_strstr (ptr_1st, "-");
	if (ptr_2nd == NULL)
	{
		PrintDebug("%s(%d) Error(info)> no range formatted port(1st:%s)\n", __FUNCTION__, __LINE__, ptr_1st);
		isRanged = FALSE;

		ptr_2nd = VK_strstr (ptr_1st, ";");
		if (ptr_2nd)
		{
			ptr_2nd[0] = 0;
		}
		ptr_1st[0] = 0;
		*p_uPort = VK_atoi (ptr_1st + VK_strlen (p_targetField));
		*p_uRtcpPort = 0;
		goto func_done;
	}

	ptr_3rd = VK_strstr (ptr_2nd, ";");
	if (ptr_3rd)
	{
		ptr_3rd[0] = 0;
	}
	ptr_2nd[0] = 0;

	*p_uPort = VK_atoi (ptr_1st + VK_strlen (p_targetField));
	*p_uRtcpPort = VK_atoi (ptr_2nd + 1);

func_done:
	if (p_lineCopy != NULL)
	{
		VK_free (p_lineCopy);
		p_lineCopy = NULL;
	}
	return TRUE;

func_error:
	if (p_lineCopy != NULL)
	{
		VK_free (p_lineCopy);
		p_lineCopy = NULL;
	}
	return FALSE;
}

static char *P_RTSP_SESSION_ParseTransportDestination (const char *p_responseLine)
{
	/* _DEF_RTSP_HDR_TRANSPORT_DESTINATION "destination=" */
	char *ptr_1st = NULL;
	char *ptr_2nd = NULL;
	char *p_transDest = NULL;
	char *p_lineCopy = NULL;
	int i_destLen;

	if (p_responseLine == NULL)
	{
		goto func_error;
	}
	p_lineCopy = VK_StrDup ((char *)p_responseLine);
	if (!p_lineCopy)
	{
		goto func_error;
	}

	/*
	 *      C->M: SETUP rtsp://foo/twister/audio RTSP/1.0
	 *            CSeq: 2
	 *            Transport: RTP/AVP;unicast;client_port=8000-8001
	 *
	 *      M->C: RTSP/1.0 200 OK
	 *            CSeq: 2
	 *            Transport: RTP/AVP;unicast;client_port=8000-8001;
	 *                       server_port=9000-9001
	 *            Session: 12345678
	 */

	ptr_1st = VK_strstr (p_lineCopy, _DEF_RTSP_HDR_TRANSPORT_DESTINATION);
	if (!ptr_1st)
	{
		goto func_error;
	}

	ptr_2nd = VK_strstr (ptr_1st, ";");
	if (!ptr_2nd)
	{
		goto func_error;
	}

	i_destLen = VK_strlen (ptr_1st) - VK_strlen (ptr_2nd) - VK_strlen (_DEF_RTSP_HDR_TRANSPORT_DESTINATION) + 1;
	p_transDest = (char *) DD_MEM_Alloc (i_destLen + 1);
	VK_snprintf (p_transDest, i_destLen, ptr_1st + VK_strlen (_DEF_RTSP_HDR_TRANSPORT_DESTINATION));
	DD_MEM_Free (p_lineCopy);
	return p_transDest;

func_error:
	return NULL;
}

static char *P_RTSP_SESSION_ParseTransportSource (const char *p_responseLine)
{
	/* _DEF_RTSP_HDR_TRANSPORT_SOURCE "source=" */
	char *ptr_1st = NULL;
	char *ptr_2nd = NULL;
	char *p_transportSource = NULL;
	char *p_lineCopy = NULL;
	int i_srcLen;

	if (p_responseLine == NULL)
	{
		goto func_error;
	}
	p_lineCopy = VK_StrDup ((char *)p_responseLine);
	if (!p_lineCopy)
	{
		goto func_error;
	}

	/*
	 *      C->M: SETUP rtsp://foo/twister/audio RTSP/1.0
	 *            CSeq: 2
	 *            Transport: RTP/AVP;unicast;client_port=8000-8001
	 *
	 *      M->C: RTSP/1.0 200 OK
	 *            CSeq: 2
	 *            Transport: RTP/AVP;unicast;client_port=8000-8001;
	 *                       server_port=9000-9001
	 *            Session: 12345678
	 */

	ptr_1st = VK_strstr (p_lineCopy, _DEF_RTSP_HDR_TRANSPORT_SOURCE);
	if (!ptr_1st)
	{
		goto func_error;
	}

	ptr_2nd = VK_strstr (ptr_1st, ";");
	if (!ptr_2nd)
	{
		goto func_error;
	}

	i_srcLen = VK_strlen (ptr_1st) - VK_strlen (ptr_2nd) - VK_strlen (_DEF_RTSP_HDR_TRANSPORT_SOURCE) + 1;
	p_transportSource = (char *) DD_MEM_Alloc (i_srcLen + 1);
	VK_snprintf (p_transportSource, i_srcLen, ptr_1st + VK_strlen (_DEF_RTSP_HDR_TRANSPORT_SOURCE));
	DD_MEM_Free (p_lineCopy);
	return p_transportSource;

func_error:
	return NULL;
}

static char *P_RTSP_SESSION_ParseRTPInfoURL (const char *p_responseLine) /* Response for PLAY */
{
	/* _DEF_RTSP_HDR_RTPINFO_URL "url=" */
	char *ptr_1st = NULL;
	char *ptr_2nd = NULL;
	char *p_rtpinfo_url = NULL;
	char *p_lineCopy = NULL;
	int i_urlLen;

	if (p_responseLine == NULL)
	{
		goto func_error;
	}
	p_lineCopy = VK_StrDup ((char *)p_responseLine);
	if (!p_lineCopy)
	{
		goto func_error;
	}


	ptr_1st = VK_strstr (p_lineCopy, _DEF_RTSP_HDR_RTPINFO_URL);
	if (!ptr_1st)
	{
		goto func_error;
	}

	ptr_2nd = VK_strstr (ptr_1st, ";");
	if (!ptr_2nd)
	{
		goto func_error;
	}

	i_urlLen = VK_strlen (ptr_1st) - VK_strlen (ptr_2nd) - VK_strlen (_DEF_RTSP_HDR_RTPINFO_URL) + 1;
	p_rtpinfo_url = (char *) DD_MEM_Alloc (i_urlLen + 1);
	VK_snprintf (p_rtpinfo_url, i_urlLen, ptr_1st + VK_strlen (_DEF_RTSP_HDR_RTPINFO_URL));
	DD_MEM_Free (p_lineCopy);
	return p_rtpinfo_url;

func_error:
	return NULL;
}

/*
 * SETUP request
 * RETURN Value
 * -1: failed
 *  last status code: SETUP request sent
 */
static HINT32 P_RTSP_SESSION_RequestSetup(struct RTSP_ACCESS_SYS_s *stream, struct RTSP_SDP_Context_s *pSdpContext) /* (ASF_HEADER_t *pAsfHeaderInfo) */
{
	struct rtsp_ctrl_t *rtsp_ctrl = NULL;
	char *p_mediaURL = NULL;
	char* pField = NULL; /* temportal pointer */
	HBOOL bret = TRUE;
	HINT32 nret = 0;
	HINT32 i_portNb = 0;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char transportBuf[200];
	char *p_transDest = NULL;
	char *p_transSrc = NULL;
	HINT32 i_rtp_client_port = 0;
	HINT32 i_rtp_server_port = 0;
	HINT32 i_rtcp_client_port = 0;
	HINT32 i_rtcp_server_port = 0;
	HBOOL bGotRawTransport = FALSE;

	PrintEnter;

	if (!stream || !pSdpContext)
	{
		PrintError("%s(%d) Error> Invalid NULL args\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (stream->stream_ctrl == NULL)
	{
		PrintError("%s(%d) Error> stream_ctrl is NULL\n", __FUNCTION__, __LINE__);
		return -1;
	}
	rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;

	p_mediaURL = DD_MEM_Alloc(BUFSIZE_1K);
	if (p_mediaURL == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_memset(p_mediaURL ,0, BUFSIZE_1K);

	/*
	 * =====================================================================
	 * http://rfc-ref.org/RFC-TEXTS/2326/chapter14.html
	 * =====================================================================
	 * 14.2. Streaming of a Container file
	 *
	 *      C->M: DESCRIBE rtsp://foo/twister RTSP/1.0
	 *            CSeq: 1
	 *
	 *      M->C: RTSP/1.0 200 OK
	 *            CSeq: 1
	 *            Content-Type: application/sdp
	 *            Content-Length: 164
	 *
	 *            v=0
	 *            o=- 2890844256 2890842807 IN IP4 172.16.2.93
	 *            s=RTSP Session
	 *            i=An Example of RTSP Session Usage
	 *            a=control:rtsp://foo/twister
	 *            t=0 0
	 *            m=audio 0 RTP/AVP 0
	 *            a=control:rtsp://foo/twister/audio
	 *            m=video 0 RTP/AVP 26
	 *            a=control:rtsp://foo/twister/video
	 *
	 *      C->M: SETUP rtsp://foo/twister/audio RTSP/1.0
	 *            CSeq: 2
	 *            Transport: RTP/AVP;unicast;client_port=8000-8001
	 *
	 *      M->C: RTSP/1.0 200 OK
	 *            CSeq: 2
	 *            Transport: RTP/AVP;unicast;client_port=8000-8001;
	 *                       server_port=9000-9001
	 *            Session: 12345678
	 */
	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);

	if (stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_TCP) // <-- RTSP_TRANSPORT_TYPE_e transport_type; 으로 바꿀 것
	{
		STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,s_szRTSPTransportInterleaved);
	}
	else
	{
		if (stream->dlopts->i_oobPort == 0)
		{
			stream->dlopts->i_oobPort = _RTSP_OOB_CHANNEL_PORT;
			PrintError("%s(%d) Error> Invalid data channel port\n", __FUNCTION__, __LINE__);
		}

		VK_memset(transportBuf, 0, 200);
		if (stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_RTP_UNICAST)
		{
			/* CID #46222 */
			/* VK_sprintf(transportBuf, s_szRTSPTransportRtp, stream->dlopts->i_oobPort); */
			VK_snprintf(transportBuf, sizeof(transportBuf)-1, s_szRTSPTransportRtp, stream->dlopts->i_oobPort);
		}
		else
		{
			/* CID #46222 */
			/* VK_sprintf(transportBuf, s_szRTSPTransportRawUdp, stream->dlopts->i_oobPort); */
			VK_snprintf(transportBuf, sizeof(transportBuf)-1, s_szRTSPTransportRawUdp, stream->dlopts->i_oobPort);
		}
		STREAM_RTSP_SetMessageHeaderField(rtsp_hdr, transportBuf);
	}

	/* Check URL/MRL */
	if (rtsp_ctrl->p_mrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL MRL\n", __FUNCTION__, __LINE__);
	}
	else /* CID #46184 */
	{
		if (VK_strncmp(rtsp_ctrl->p_mrl, "rtsp://", VK_strlen("rtsp://")) != 0)
		{
			PrintError("%s(%d) Error> Invalid NULL URL string(%s)\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_mrl);
		}
		VK_snprintf(p_mediaURL,BUFSIZE_1K - 1,"%s",rtsp_ctrl->p_mrl);
	}
	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_SETUP);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,p_mediaURL);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);
	STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);

	/* receive message for SETUP */
	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	nret = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);
	if (nret < 0)
	{
		PrintError("%s(%d) Error> ReadMessageAndParseBasic failed\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	/* Parse the Transport fields */
	pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"Transport");
	if (pField == NULL)
	{
		PrintError("%s(%d) Error> No transport string in SETUP response\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	else
	{
		PrintDebug("%s(%d) Transport field:%s\n", __FUNCTION__, __LINE__, pField);

		if (VK_strstr (pField, _DEF_RTSP_HDR_TRANSPORT_MULTICAST))
		{
			PrintError("%s(%d) Error> Unsuported multicast transport\n", __FUNCTION__, __LINE__);
			goto func_error;
		}

		if (stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_TCP)
		{
			/* Transport: RTP/AVP;unicast;destination=10.0.13.151;source=10.0.13.142;client_port=7002-7003;server_port=6970-6971 */
			if (!VK_strstr(pField,"RTP"))
			{
				PrintError("%s(%d) Error(info)> Server does not support RTP\n", __FUNCTION__, __LINE__);
			}
			if (!VK_strstr(pField,"UDP"))
			{
				PrintError("%s(%d) Error(info)> Server does not support UDP\n", __FUNCTION__, __LINE__);
			}
			if (!VK_strstr(pField,"TCP")) /* Unsupported TCP field: TCP or TCP/RTP connection */
			{
				PrintError("%s(%d) Error(info)> Server does not support TCP\n", __FUNCTION__, __LINE__);
			}

			if (VK_strstr(pField,"RAW/RAW/UDP") != NULL) /* Unsupported TCP field: TCP or TCP/RTP connection */
			{
				PrintError("%s(%d) Error(info)> Got RAW/RAW/UDP\n", __FUNCTION__, __LINE__);
				bGotRawTransport = TRUE;
			}

			if (!VK_strstr(pField,"unicast")) /* Unsupported multicast field */
			{
				PrintError("%s(%d) Error(info)> Server does not support unicast\n", __FUNCTION__, __LINE__);
			}
			 /* Check interleaved */
			if (!VK_strstr(pField,"interleaved=0-1"))
			{
				PrintError("%s(%d) Error(info)> Server does not support interleaved=0-1 mode\n", __FUNCTION__, __LINE__);
			}

			if (VK_strstr (pField, _DEF_RTSP_HDR_TRANSPORT_MULTICAST))
			{
				PrintError("%s(%d) Error(info)> Unsupported multicast Transport\n", __FUNCTION__, __LINE__);
				goto func_error;
			}
		}

		/* Store transport each field */
		/* check for RTP and RTCP ports to bind according to how request was done */
		i_portNb = P_RTSP_SESSION_ParseTransportPort (pField, _DEF_RTSP_HDR_UNICAST_CLIENT_PORT, &i_rtp_client_port, &i_rtcp_client_port);
		if (i_portNb == 0)
		{
			PrintError("%s(%d) Error> Transport:client_port field failed\n", __FUNCTION__, __LINE__);
		}

		bret = P_RTSP_SESSION_ParseTransportPort (pField, _DEF_RTSP_HDR_UNICAST_SERVER_PORT, &i_rtp_server_port, &i_rtcp_server_port);
		if (bret == FALSE)
		{
			PrintError("%s(%d) Error> Transport:server_port field failed\n", __FUNCTION__, __LINE__);
		}

		/*
		 * RTSP/1.0 200 OK
		 * CSeq: 2
		 * Date: Wed, Sep 12 2012 13:07:18 GMT
		 * Transport: RTP/AVP;unicast;destination=10.0.13.151;source=10.0.13.142;client_port=7002-7003;server_port=6970-6971
		 * Session: 38B2B26C
		 */
		/* now check network settings as determined by server */
		/* Transport, destination= field */
		p_transDest = P_RTSP_SESSION_ParseTransportDestination (pField);
		if (!p_transDest)
		{
			PrintError("%s(%d) Error> Transport:destination field failed\n", __FUNCTION__, __LINE__);
			/*p_transDest = VK_StrDup (SDP_server_addr);*/
			if (rtsp_ctrl->p_transDest != NULL)
			{
				PrintError("%s(%d) Error> Remove prev Transport:destination(%s)\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_transDest);
				DD_MEM_Free(rtsp_ctrl->p_transDest);
				rtsp_ctrl->p_transDest = NULL;
			}
		}
		else
		{
			PrintError("%s(%d) Error(info)> Got Transport:destination(%s)\n", __FUNCTION__, __LINE__, p_transDest);

			if (rtsp_ctrl->p_transDest != NULL)
			{
				PrintError("%s(%d) Error> Remove prev Transport:destination(%s)\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_transDest);
				DD_MEM_Free(rtsp_ctrl->p_transDest);
				rtsp_ctrl->p_transDest = NULL;
			}
			rtsp_ctrl->p_transDest = VK_StrDup(p_transDest);
		}

		/* Transport, source= field */
		p_transSrc = P_RTSP_SESSION_ParseTransportSource (pField);
		if (!p_transSrc)
		{
			PrintError("%s(%d) Error> Transport:source field failed\n", __FUNCTION__, __LINE__);
			/*p_transSrc = VK_StrDup (SDP_server_addr);*/

			if (rtsp_ctrl->p_transSrc != NULL)
			{
				PrintError("%s(%d) Error> Remove prev Transport:source(%s)\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_transSrc);
				DD_MEM_Free(rtsp_ctrl->p_transSrc);
				rtsp_ctrl->p_transSrc = NULL;
			}
		}
		else
		{
			PrintError("%s(%d) Error(info)> Got Transport:source(%s)\n", __FUNCTION__, __LINE__, p_transSrc);

			if (rtsp_ctrl->p_transSrc != NULL)
			{
				PrintError("%s(%d) Error> Remove prev Transport:source(%s)\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_transSrc);
				DD_MEM_Free(rtsp_ctrl->p_transSrc);
				rtsp_ctrl->p_transSrc = NULL;
			}
			rtsp_ctrl->p_transSrc = VK_StrDup(p_transSrc);
		}

		rtsp_ctrl->i_rtp_client_port  = i_rtp_client_port;
		rtsp_ctrl->i_rtcp_client_port = i_rtcp_client_port;
		rtsp_ctrl->i_rtp_server_port  = i_rtp_server_port;
		rtsp_ctrl->i_rtcp_server_port = i_rtcp_server_port;

		PrintDebug("%s(%d) RTSP Transport: \n", __FUNCTION__, __LINE__);
		PrintDebug("%s(%d) + destination: %s \n", __FUNCTION__, __LINE__, rtsp_ctrl->p_transDest);
		PrintDebug("%s(%d) + client     : %d-%d \n", __FUNCTION__, __LINE__, rtsp_ctrl->i_rtp_client_port, rtsp_ctrl->i_rtcp_client_port);
		PrintDebug("%s(%d) + server     : %d-%d \n", __FUNCTION__, __LINE__, rtsp_ctrl->i_rtp_server_port, rtsp_ctrl->i_rtcp_server_port);

		/* Store transport field */
		if (rtsp_ctrl->p_transport)
		{
			DD_MEM_Free(rtsp_ctrl->p_transport);
			rtsp_ctrl->p_transport = NULL;

			while(*pField == ' ') pField++;
			rtsp_ctrl->p_transport = VK_StrDup(pField);
		}
	}

	if (nret < 0)
	{
		PrintError("%s(%d) Error> SETUP request is failed\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	STREAM_RTSP_ReadMessageAndFreeBody(stream, rtsp_hdr);
	STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	rtsp_hdr = NULL;
	PrintDebug("%s(%d) Setup Success\n", __FUNCTION__, __LINE__);

func_error:
	if( p_transDest != NULL)
	{
		DD_MEM_Free(p_transDest);
		p_transDest = NULL;
	}
	if( p_transSrc != NULL)
	{
		DD_MEM_Free(p_transSrc);
		p_transSrc = NULL;
	}
	if (rtsp_hdr != NULL)
	{
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
	}

	if (p_mediaURL != NULL)
	{
		DD_MEM_Free(p_mediaURL);
		p_mediaURL = NULL;
	}
	PrintExit;
	return nret;
}

#if 0 /* Commented */
/*
 * send SET_PARAMETER request
 * return value:  -1:failure   status code: success
 */
static HINT32 P_RTSP_SESSION_RequestSetParameter(struct RTSP_ACCESS_SYS_s *stream)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	HINT32 nret = 0;
	char *buffer;

	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	if (buffer == NULL)
	{
		PrintError("%s(%d) Error> Out of memory \n", __FUNCTION__, __LINE__);
		return (-1);
	}
	VK_memset(buffer ,0, BUFSIZE_1K);

	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,s_szRTSPUserAgent);
	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_SET_PARAMETER);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,rtsp_ctrl->p_mrl);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);


	STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);

	/*
	   receive message for SET_PARAMETER
	   if something needs to be done to message, use STREAM_RTSP_ReadMessageAndParseBasic
	   */
	nret = STREAM_RTSP_ReadRtspResponseAndFree(stream);

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return nret;
}
#endif /* Commented */

#if 0 /* Commented */
/*
 * send PAUSE request
 * return value:  -1:failure   status code: success
 */
static HINT32 P_RTSP_SESSION_RequestPause(struct RTSP_ACCESS_SYS_s *stream)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *buffer;
	HINT32 nret = 0;

	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	if (buffer == NULL)
	{
		PrintError("%s(%d) Error> Out of memory \n", __FUNCTION__, __LINE__);
		return (-1);
	}
	VK_memset(buffer ,0, BUFSIZE_1K);

	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);

	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,s_szRTSPUserAgent);
	//rtsp_request_set_parameter(rtsp_hdr,rtsp_ctrl->p_mrl);
	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_PAUSE);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,rtsp_ctrl->p_mrl);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);

	STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);

	/*
	   receive message for SET_PARAMETER
	   if something needs to be done to message, use STREAM_RTSP_ReadMessageAndParseBasic
	   */
	nret = STREAM_RTSP_ReadRtspResponseAndFree(stream);

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return nret;
}
#endif /* Commented */


HINT32 RTSP_SESSION_ProcessOobRTPChannel(struct RTSP_CHANNEL_t *pRtpChannel, HUINT8 *p_buffer, size_t i_request)
{
	HUINT32 i_total = 0;
	HINT32 i_read = 0;
	HUINT32 i_recvCnt = 0;
	HUINT32 i_errLoopCnt = 0;

	PrintEnter;

	if ((pRtpChannel == NULL) || (pRtpChannel->transport_type != RTSP_TRANSPORT_RTP_UNICAST))
	{
		PrintError("%s(%d) Error> Invalid arg\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (pRtpChannel->i_sock < 0)
	{
		PrintError("%s(%d) Error> Invalid socket(%d)!\n", __FUNCTION__, __LINE__, pRtpChannel->i_sock);
		goto func_error;
	}

	if (pRtpChannel->i_dataLen > 0)
	{
		PrintError("%s(%d) Error> Unexpected prev remained buffer[uncopied:%d, req:%d]\n", __FUNCTION__, __LINE__, pRtpChannel->i_dataLen, i_request);
		;
	}

	i_total = i_read = 0;
	do
	{
		if (pRtpChannel->i_sock < 0)
		{
			PrintError("%s(%d) Error> Invalid socket(%d)!\n", __FUNCTION__, __LINE__, pRtpChannel->i_sock);
			goto func_error;
		}

		i_recvCnt++;

		i_read = net_RTPReadChannel (pRtpChannel, (HUINT8 *)p_buffer+i_total, i_request - i_total );
		if (i_read < 0)
		{
			if (i_total > 0)
			{
				goto func_done;
			}

			goto func_error;
		}
		else
		{
			i_total += i_read;
		}

		if ((i_errLoopCnt == 10) && (i_total == 0))
		{
			PrintError("%s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, i_errLoopCnt);
			i_errLoopCnt = 0;
			goto func_error;
		}
		i_errLoopCnt++;
	} while (i_total < i_request);


func_done:
	if (i_total != i_request)
	{
		PrintError("%s(%d) Error> Can't read all of pkts(total:%d, max:%d)\n", __FUNCTION__, __LINE__, i_total, i_request);
		;
	}
	PrintError("%s(%d) --(total:%d, max:%d), last(read:%d)\n", __FUNCTION__, __LINE__, i_total, i_request, i_read);
	PrintExit;
	return i_total;

func_error:
	PrintError("%s(%d) Error> failed(total:%d, max:%d)\n", __FUNCTION__, __LINE__, i_total, i_request);
	PrintExit;
	return (-1);
}

static HINT32 P_RTSP_SESSION_ReadSmallUdpPacket(struct RTSP_CHANNEL_t *pUdpChannel, HUINT8 *pBuffer, size_t i_request) /* Under 1500 bytes */
{
	HINT32 i_read = 0;
	HUINT8 m_packetBuffer[_MTU_PKT_SIZE];
	HUINT8 *ptr = NULL; /* For getting payload only, skipping RTP header */
	int	fromlen;
	struct sockaddr *from; // struct sockaddr_in from;

	PrintEnter;

	fromlen = sizeof(from);

	if (pUdpChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid arg\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (pUdpChannel->transport_type != RTSP_TRANSPORT_UDP_UNICAST)
	{
		PrintError("%s(%d) Error> Invalid non-UDP protocol(%d)\n", __FUNCTION__, __LINE__, pUdpChannel->transport_type);
		goto func_error;
	}

	if (pUdpChannel->i_sock < 0)
	{
		PrintError("%s(%d) Error> Invalid fd(%d)!\n", __FUNCTION__, __LINE__, pUdpChannel->i_sock);
		goto func_error;
	}

	if (i_request > _MTU_PKT_SIZE)
	{
		PrintError("%s(%d) Error> Too large request buffer[req:%d]\n", __FUNCTION__, __LINE__, i_request);
		goto func_error;
	}

	/* Read socket until given p_buffer is full */
	/* Socket timeout value have 1 (sec) */
	VK_memset(m_packetBuffer, 0, _MTU_PKT_SIZE);
	ptr = m_packetBuffer;
	i_read = recvfrom(pUdpChannel->i_sock, (void *)m_packetBuffer, i_request, //
							0, /* consider MSG_DONTWAIT usage */
							(struct sockaddr *) &from,
							(socklen_t*) &fromlen);
	if (i_read <= 0)
	{
		PrintError("%s(%d) Error> recv() failed(read:%d/size:%d)\n", __FUNCTION__, __LINE__, i_read, i_request);
		goto func_error;
	}

	/* Moved the pointer */
	if (i_read == (_RTP_HEADER_SIZE + _UDP_PKT_SIZE)) /* RTP packet size */
	{
		i_read -= _RTP_HEADER_SIZE;
		ptr += _RTP_HEADER_SIZE;
	}
	else if (i_read == _UDP_PKT_SIZE)  /* UDP packet size */
	{
		;
	}

	/* Copy TS payload only to pumping */
	if (i_read <= (HINT32)i_request)
	{
		VK_memcpy((HUINT8*)((unsigned long)pBuffer), ptr, i_read);
	}
	else
	{
		PrintError("%s(%d) Error> Unexpected size(read:%d/size:%d)\n", __FUNCTION__, __LINE__, i_read, i_request);
	}

	PrintError("%s(%d) --(read:%d, req:%d, diff:%d)\n", __FUNCTION__, __LINE__, i_read, i_request, (i_request-i_read));
	PrintExit;
	return i_read;

func_error:
	PrintError("%s(%d) Error> failed(read:%d, req:%d, diff:%d)\n", __FUNCTION__, __LINE__, i_read, i_request);
	PrintExit;
	return 0;
}

static HINT32 P_RTSP_SESSION_ReadUDPPacket(struct RTSP_CHANNEL_t *pUdpChannel, HUINT8 *pBuffer, size_t i_request)
{
	HINT32 nret = 0;
	HINT32 i_read = 0;
	HUINT32 i_total = 0;
	HUINT32 i_recvCnt = 0;
	HUINT32 i_errLoopCnt = 0;
	HUINT32 i_errLoopMax = 100;
	fd_set t_fds;
	struct timeval 	i_sockTimeout;
	struct sockaddr *from = NULL;
	socklen_t	from_len = 0;

	HUINT8 m_packetBuffer[_MTU_PKT_SIZE];
	HUINT8 *ptr = NULL; /* For getting payload only, skipping RTP header */

	HBOOL b_copyPartially = FALSE;
	HINT32 i_remain = 0;
	HINT32 i_uncopied = 0;

	PrintEnter;

	if ((pUdpChannel == NULL) || (pUdpChannel->transport_type != RTSP_TRANSPORT_UDP_UNICAST))
	{
		PrintError("%s(%d) Error> Invalid arg\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (pUdpChannel->i_sock < 0)
	{
		PrintError("%s(%d) Error> Invalid fd(%d)!\n", __FUNCTION__, __LINE__, pUdpChannel->i_sock);
		goto func_error;
	}

	i_total = 0;
	if (i_request < _MTU_PKT_SIZE)
	{
		VK_memset(m_packetBuffer, 0, _MTU_PKT_SIZE);
		ptr = m_packetBuffer;

		PrintError("%s(%d) Error> ++Too small buffer[req:%d]\n", __FUNCTION__, __LINE__, i_request);
		i_read = P_RTSP_SESSION_ReadSmallUdpPacket(pUdpChannel, m_packetBuffer, i_request); /* Under 1500 bytes */
		if (i_read != (HINT32)i_request)
		{
			PrintError("%s(%d) Error> Unmatched small buffer[small:%d != req:%d]\n", __FUNCTION__, __LINE__, i_read, i_request);
			;
		}

		if (i_read > 0)
		{
			VK_memcpy((HUINT8*)((unsigned long)pBuffer+i_total), ptr, i_read);
			i_total += i_read;
		}
		PrintError("%s(%d) Error> --Too small buffer[req:%d, i_total:%u]\n", __FUNCTION__, __LINE__, i_request, i_total);
		return i_total;
	}

	if (pUdpChannel->i_dataLen > 0)
	{
		PrintError("%s(%d) Error> There is small prev remained buffer[req:%d]\n", __FUNCTION__, __LINE__, pUdpChannel->i_dataLen);
		goto func_error;
	}

	/* Read socket until given p_buffer is full */
	/* Socket timeout value have 1 (sec) */
	while (1)
	{
		//PrintDebug("%s(%d) Received Bytes(err:%d, total:%u, i_request:%u)!\n", __FUNCTION__, __LINE__, i_errLoopCnt, i_total, i_request);
		i_recvCnt++;

		if (i_recvCnt > 1000) /* 1316*X */
		{
			PrintError("%s(%d) Error> Too many receive[err:%d, total:%u, req:%u:last:%d]\n", __FUNCTION__, __LINE__, i_recvCnt, i_errLoopCnt, i_total, i_request, i_read);
			goto func_error;
		}

		if (pUdpChannel->i_sock < 0)
		{
			PrintError("%s(%d) Error> Invalid fd(%d)!\n", __FUNCTION__, __LINE__, pUdpChannel->i_sock);
			goto func_error;
		}

		/* The request length must have 1500 or above value */
		if (i_total >= i_request)
		{
			PrintDebug("%s(%d) Read stream successfully(total:%u, req:%u, loop(%d, err:%d)!\n", __FUNCTION__, __LINE__, i_total, i_request, i_recvCnt, i_errLoopCnt);
			if (i_total > i_request)
			{
				PrintError("%s(%d) Error> Invalid receive size(total:%u > req:%u, loop(%d, err:%d)!\n", __FUNCTION__, __LINE__, i_total, i_request, i_recvCnt, i_errLoopCnt);
				;
			}
			goto func_done;
		}

		b_copyPartially = FALSE;
		if ((i_total+_UDP_PKT_SIZE) >= i_request)
		{
			PrintError("%s(%d) Error(info) Read exeed size(cnt:%d, total:%u, req:%u, diff:%d, loop:%d,%d)!\n", __FUNCTION__, __LINE__, i_recvCnt, i_total, i_request, (i_request-i_total), i_recvCnt, i_errLoopCnt);

			b_copyPartially = TRUE;
			i_remain = i_request - i_total;
			if (i_remain%188 != 0)
			{
				PrintError("%s(%d) Error> Not 188x small size[remain:%d, req:%d, total:%d]\n", __FUNCTION__, __LINE__,i_remain, i_request, i_total);
			}
			VK_memset(m_packetBuffer, 0, _MTU_PKT_SIZE);
			ptr = m_packetBuffer;

			PrintError("%s(%d) Error> ++Too small remained size[remain:%d, req:%d, total:%d, cnt:%d]\n", __FUNCTION__, __LINE__, i_remain, i_request, i_total, i_recvCnt);
			i_read = P_RTSP_SESSION_ReadSmallUdpPacket(pUdpChannel, m_packetBuffer, i_remain); /* Under 1500 bytes */
			PrintError("%s(%d) Error> --Too small remained size[read:%d, remain:%d, total:%d]\n", __FUNCTION__, __LINE__,i_read, i_remain, i_total);
			if (i_read != i_remain)
			{
				PrintError("%s(%d) Error> Unmatched small buffer[small:%d != remain:%d]\n", __FUNCTION__, __LINE__, i_read, i_remain);
			}

			if (i_read > 0)
			{
				if (i_read <= i_remain)
				{
					VK_memcpy((HUINT8*)((unsigned long)pBuffer+i_total), ptr, i_read);
					i_total += i_read;
					PrintError("%s(%d) Error(info)> copy small size[read:%d <= remain:%d, total:%d]\n", __FUNCTION__, __LINE__,i_read, i_remain, i_total);
				}
				else
				{
					/* Copy remain size */
					PrintError("%s(%d) Error> More bigger size[read:%d > remain:%d, total:%d]\n", __FUNCTION__, __LINE__,i_read, i_remain, i_total);
					VK_memcpy((HUINT8*)((unsigned long)pBuffer+i_total), ptr, i_remain);
					i_total += i_remain;

					/* If uncopied data is remain, move it to channel's udp buffer */
					i_uncopied = (i_read - i_remain);
					if (i_uncopied > 0)
					{
						(void)RTSP_LIB_ClearChannelBuffer(pUdpChannel);

						pUdpChannel->i_dataLen = i_uncopied;
						VK_memcpy((HUINT8*)pUdpChannel->p_buffer, ptr+i_remain, i_uncopied);
					}
				}
			}
			if (i_total != i_request)
			{
				PrintError("%s(%d) Error> Can't read all of pkts(total:%d, max:%d, diff:%d)\n", __FUNCTION__, __LINE__, i_total, i_request, (i_request-i_total));
			}
			goto func_done;
		}

/*func_read:*/
		if (1) /* Check again because of above smaller packet(under MTU size) */
		{
			if (i_total >= i_request)
			{
				PrintDebug("%s(%d) Read stream successfully(total:%u, req:%u, loop(%d, err:%d)!\n", __FUNCTION__, __LINE__, i_total, i_request, i_recvCnt, i_errLoopCnt);
				if (i_total > i_request)
				{
					PrintError("%s(%d) Error> Invalid receive size(total:%u > req:%u, loop(%d, err:%d)!\n", __FUNCTION__, __LINE__, i_total, i_request, i_recvCnt, i_errLoopCnt);
					;
				}
				goto func_done;
			}
		}
		VK_memset(m_packetBuffer, 0, _MTU_PKT_SIZE);
		ptr = m_packetBuffer;

		/*
		 * Receive payload; extract RTP header if RTP streaming
		 * ssize_t net_Recvfrom(int nfd, const struct virtual_socket_t *vs,
		 *                      HUINT8 *restrict p_buf, size_t i_buflen,
		 *                      HBOOL waitall)
		 */
		i_read = recvfrom(pUdpChannel->i_sock,  //
							  m_packetBuffer,
							  _MTU_PKT_SIZE,
							  0,
							  from,
							  &from_len);

		if (i_read > 0)
		{
			/* Copy TS payload only to pumping */
			VK_memcpy((HUINT8*)((unsigned long)pBuffer+i_total), ptr, i_read);
			i_total += i_read;
			continue;
		}
		else if (i_read==0)
		{
			PrintError("%s(%d) Error> connection closed(recv:%d, total:%d, max:%d)\n", __FUNCTION__, __LINE__, i_read, i_total, i_request);
			goto func_wait;
		}
		else
		{
			i_errLoopCnt++;
			PrintError("%s(%d) Error> read func_error(%d, total:%d, errloop:%d,%d)\n", __FUNCTION__, __LINE__, i_read, i_total, i_errLoopCnt, i_errLoopMax);

			if (i_errLoopCnt < i_errLoopMax)
			{
				PrintError("%s(%d) Error> Too many retries(%d>max:%d, fd:%d) --> err:%d,%s\n", __FUNCTION__, __LINE__, i_errLoopCnt, i_errLoopMax, pUdpChannel->i_sock, errno, strerror(errno));
				goto func_error;
			}

			if (errno == EINTR)
			{
				continue;
			}
			else if (errno != EAGAIN)
			{
				PrintError("%s(%d) Error> recvfrom error(%d,%s)\n", __FUNCTION__, __LINE__, errno, strerror(errno));
				goto func_error;
			}
		}


func_wait:
		FD_ZERO(&t_fds);
		FD_SET(pUdpChannel->i_sock, &t_fds);
		i_sockTimeout.tv_sec = 0;
		i_sockTimeout.tv_usec = _RTP_RECV_TIMEOUT;

		nret = select(pUdpChannel->i_sock+1, &t_fds, NULL, NULL, &i_sockTimeout);
		if (nret == 0)
		{
			PrintError("%s(%d) Error> Receive timeout\n", __FUNCTION__, __LINE__);
			continue;
		}
		else if (nret<0 && errno != EINTR)
		{
			PrintError("%s(%d) Error> select error(%d,%s)\n", __FUNCTION__, __LINE__, errno, strerror(errno));
			goto func_error;
		}
	}

func_done:
	if (i_total != i_request)
	{
		PrintError("%s(%d) Error> Can't read all of pkts(total:%d, max:%d, diff:%d)\n", __FUNCTION__, __LINE__, i_total, i_request, (i_request-i_total));
		;
	}
	PrintError("%s(%d) --(total:%d, max:%d, diff:%d), last(read:%d, cnt:%d,%d)\n", __FUNCTION__, __LINE__, i_total, i_request, (i_request-i_total), i_read, i_recvCnt, i_errLoopCnt);
	PrintExit;
	return i_total;

func_error:
	PrintError("%s(%d) Error> failed(total:%d, max:%d)\n", __FUNCTION__, __LINE__, i_total, i_request);
	if (i_total != 0)
	{
		PrintExit;
		return i_total;
	}

	PrintExit;
	return (-1);
}

HINT32 RTSP_SESSION_ProcessOobUDPChannel(struct RTSP_CHANNEL_t *pUdpChannel, HUINT8 *p_buffer, size_t i_request)
{
	HUINT32 i_total = 0;
	HINT32 i_read = 0;
	HUINT32 i_recvCnt = 0;
	HUINT32 i_errLoopCnt = 0;

	PrintEnter;

	if ((pUdpChannel == NULL) || (pUdpChannel->transport_type != RTSP_TRANSPORT_UDP_UNICAST))
	{
		PrintError("%s(%d) Error> Invalid arg\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (pUdpChannel->i_sock < 0)
	{
		PrintError("%s(%d) Error> Invalid socket(%d)!\n", __FUNCTION__, __LINE__, pUdpChannel->i_sock);
		goto func_error;
	}

	if (pUdpChannel->i_dataLen > 0)
	{
		PrintError("%s(%d) Error> Unexpected prev remained buffer[uncopied:%d, req:%d]\n", __FUNCTION__, __LINE__, pUdpChannel->i_dataLen, i_request);
	}

	i_total = i_read = 0;
	do
	{
		if (pUdpChannel->i_sock < 0)
		{
			PrintError("%s(%d) Error> Invalid socket(%d)!\n", __FUNCTION__, __LINE__, pUdpChannel->i_sock);
			goto func_error;
		}

		i_recvCnt++;
		i_read = P_RTSP_SESSION_ReadUDPPacket (pUdpChannel, (HUINT8 *)p_buffer+i_total, i_request - i_total );
		if (i_read < 0)
		{
			if (i_total > 0)
			{
				goto func_done;
			}

			goto func_error;
		}
		else
		{
			i_total += i_read;
		}

		if ((i_errLoopCnt == 10) && (i_total == 0))
		{
			PrintError("%s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, i_errLoopCnt);
			i_errLoopCnt = 0;
			goto func_error;
		}
		i_errLoopCnt++;
	} while (i_total < i_request); // <--


func_done:
	if (i_total != i_request)
	{
		PrintError("%s(%d) Error> Can't read all of pkts(total:%d, max:%d)\n", __FUNCTION__, __LINE__, i_total, i_request);
	}
	PrintError("%s(%d) --(total:%d, max:%d), last(read:%d)\n", __FUNCTION__, __LINE__, i_total, i_request, i_read);
	PrintExit;
	return i_total;

func_error:
	PrintError("%s(%d) Error> failed(total:%d, max:%d)\n", __FUNCTION__, __LINE__, i_total, i_request);
	PrintExit;
	return (-1);
}

/*
 * send SETUP,SEND_REQUEST,PLAY requests.
 *
 *	  return value				1 : success
 *								  0 : error, retry via mms
 *								 -1 : error.
 */
HINT32 RTSP_SESSION_RequestDescribeAndGetSDP(struct RTSP_ACCESS_SYS_s *stream)
{
	struct RTSP_CTRL_s *stream_ctrl = NULL;
	struct rtsp_ctrl_t *rtsp_ctrl = NULL;
	struct RTSP_SDP_Context_s *pSdpContext = NULL;
	char *p_description = NULL;
	HBOOL bret = FALSE;
	HINT32 nret = 0;

	PrintEnter;

	if (stream == NULL)
	{
		PrintError("%s(%d) Error>> Invalid NULL stream\n", __FUNCTION__, __LINE__);
		goto func_retry;
	}
	stream_ctrl = stream->stream_ctrl;
	if (stream_ctrl == NULL)
	{
		PrintError("%s(%d) Error>> Invalid NULL stream_ctrl\n", __FUNCTION__, __LINE__);
		goto func_retry;
	}
	rtsp_ctrl = stream_ctrl->rtsp_ctrl;
	if (rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error>> Invalid NULL rtsp_ctrl\n", __FUNCTION__, __LINE__);
		goto func_retry;
	}

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
	nret = P_RTSP_SESSION_RequestDescribe(stream, &p_description);
	if (nret == 0)
	{
		PrintError("%s(%d) Error>> RTSP DESCRIBE failed --> retry\n", __FUNCTION__, __LINE__);
		goto func_retry;
	}
	else if (nret < 0)
	{
		PrintError("%s(%d) Error>> RTSP DESCRIBE failed\n", __FUNCTION__, __LINE__);
		goto func_error;
	}


	/* parse SDP decription and header*/
	bret = P_RTSP_SESSION_ParseSDP(p_description, &pSdpContext);
	if (bret == FALSE)
	{
		PrintError("%s(%d) Error>> Getting SDP failed\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	stream_ctrl->pSdpContext = pSdpContext;

	/* SETUP requests (for audio and video) */
	if (0)
	{
		nret = P_RTSP_SESSION_RequestSetup(stream, pSdpContext);
		if (nret < 0)
		{
			PrintError("%s(%d) Error>> RTSP SETUP failed\n", __FUNCTION__, __LINE__);
			goto func_error;
		}
	}

	/* buffering it for Read function */
	if (stream->b_feed_asfheader_first == TRUE)
	{
		stream->b_feed_asfheader_first = FALSE;
	}

	//RTSP_SDP_FreeContext(pSdpContext);
	DD_MEM_Free(p_description);
	p_description = NULL;
	PrintExit;
	return 1;

func_error:
	//if (pSdpContext)
	//{
	//	RTSP_SDP_FreeContext(pSdpContext);
	//	pSdpContext = NULL;
	//}
	if (p_description)
	{
		DD_MEM_Free(p_description);
		p_description = NULL;
	}

	PrintExit;
	return (-1);

func_retry: /* retry with other protocol */
	//if (pSdpContext)
	//{
	//	RTSP_SDP_FreeContext(pSdpContext);
	//	pSdpContext = NULL;
	//}
	if (p_description)
	{
		DD_MEM_Free(p_description);
		p_description = NULL;
	}

	PrintExit;
	return 0;
}

/*
 * get SDP context by string, and stream->stream_ctrl->bandwidth
 * return value: 0 : failed	   1: success
 */
static HBOOL P_RTSP_SESSION_ParseSDP(char *pSdpString, struct RTSP_SDP_Context_s **ppSdpCtx_out)
{
	struct RTSP_SDP_Context_s *pSdpContext = NULL;

	PrintEnter;

	if (!pSdpString)
	{
		/* rtsp stream could not be NULL */
		PrintError("%s(%d) Error> Invalid NULL args\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	/* parse sdp and get information about file to download */
	pSdpContext = RTSP_SDP_ParseDescription(pSdpString);
	if (pSdpContext == NULL)
	{
		PrintError("%s(%d) Error> Can't get SDP context\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	*ppSdpCtx_out		 = pSdpContext;
	return TRUE;

func_error:
	PrintError("%s(%d) Error> function failed\n", __FUNCTION__, __LINE__);

	if (pSdpContext)
	{
		RTSP_SDP_FreeContext(pSdpContext);
	}
	*ppSdpCtx_out = NULL;
	PrintExit;
	return FALSE;
}


HINT32 RTSP_SESSION_ProcessInterleavedChannel(struct RTSP_ACCESS_SYS_s *stream)
{
	HUINT8 interleaved_p_hdr[_INTERLEAVED_PACK_HEADER_SIZE];
	struct RTSP_CTRL_s *stream_ctrl = stream->stream_ctrl;
	HINT32 nret = 0;
	HINT32 nStatusCode = 0;

	PrintEnter;
	PrintError("%s(%d) Error> Current, don't permitted function\n", __FUNCTION__, __LINE__);

	if (stream->bInterleavedDataLefted == TRUE)
	{
		goto success;
	}

	nret = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,interleaved_p_hdr,_INTERLEAVED_PACK_HEADER_SIZE);
	if (nret < _INTERLEAVED_PACK_HEADER_SIZE) {
		PrintError("%s(%d) Error> Reading data channel failed(nret:%d)\n", __FUNCTION__, __LINE__, nret);
		goto func_error;
	}

	if (interleaved_p_hdr[0] == 0x24)
	{
		goto interleaved_packet;
	}

	if (!VK_strncasecmp((char *)interleaved_p_hdr,"SET_",_INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"GET_",_INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"OPTI",_INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"RTSP",_INTERLEAVED_PACK_HEADER_SIZE) ||
	  !VK_strncasecmp((char *)interleaved_p_hdr,"ANNO",_INTERLEAVED_PACK_HEADER_SIZE))
	{
		RTSP_LIB_PutMessageForLaterRead(stream,interleaved_p_hdr,_INTERLEAVED_PACK_HEADER_SIZE); /* push back interleaved_p_hdr */

		nret = P_RTSP_SESSION_ProcessMessageResponse(stream);
		if (nret < 0) {
			PrintError("%s(%d) Error> RTSP response failed (nret:%d)\n", __FUNCTION__, __LINE__, nret);
			goto func_error;
		}
		nStatusCode = nret;

		if (nret == 0 && stream_ctrl->session_status == RTSP_SESSION_STATUS_FINISHED)
		{
			/* streaming finished (at least it says) */

			/*
			  wait for 2 seconds.. this is necessary becasue
			  otherwise it would block when waiting "ANNOUNCE"
			*/
			if (RTSP_LIB_SelectCtrlChannel(stream,2) > 0)
			{
				if (P_RTSP_SESSION_AnnounceContinue(stream) != FALSE)
				{
					PrintDebug("%s(%d) streaming still continues!!\n", __FUNCTION__, __LINE__);
#if 0 /* no meaning at standard RTSP */
					stream->b_feed_asfheader_first = TRUE;
#endif
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
			nret = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,interleaved_p_hdr,_INTERLEAVED_PACK_HEADER_SIZE);
			if (nret < _INTERLEAVED_PACK_HEADER_SIZE)
			{
				PrintError("%s(%d) Error> Invalid header size[%d]\n", __FUNCTION__, __LINE__, nret);
				goto func_error;
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
		RTSP_LIB_DumpByteBuffer(interleaved_p_hdr,_INTERLEAVED_PACK_HEADER_SIZE);
		goto func_error;
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
	/*PrintDebug("%s(%d) interleaved_data_packet_size = %d\n", __FUNCTION__, __LINE__, stream_ctrl->rtsp_ctrl->interleaved_data_packet_size);*/

	stream->bInterleavedDataLefted = TRUE;

success:
	PrintExit;
	return nStatusCode;

func_error:
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
static HBOOL P_RTSP_SESSION_AnnounceContinue(struct RTSP_ACCESS_SYS_s *stream)
{
	struct RTSP_CTRL_s *stream_ctrl = NULL;
	struct rtsp_ctrl_t *rtsp_ctrl = NULL;
  	HINT32 i = 0;
	HBOOL bret = FALSE;
	HINT32 nret = 0;
	char checkbuf[10];			  		/* check whether the message is ANNOUCNE */
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	HINT32 i_cseq = 0;			   	/* cseq receivedi */
	HINT32 i_statusCode = 0;				/* rtsp_hdr status value */
	char *pField = NULL;			 	/* filed pointer for rtsp_hdr   */
	HINT32 i_len = 0;						/* length of description (content length) */
	char *p_description = NULL;	   		/* description pointer, data of rtsp-packet	*/
	char *buffer = NULL;			/* temp buffer for building req */
	struct RTSP_SDP_Context_s *pSdpContext = NULL;

	PrintEnter;

	if ((stream == NULL) || (stream->stream_ctrl == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	stream_ctrl = stream->stream_ctrl;

	if (stream_ctrl->rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp_ctrl\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	rtsp_ctrl = stream_ctrl->rtsp_ctrl;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	if (buffer == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(buffer ,0, BUFSIZE_1K);

	i = RTSP_LIB_ReadFromCtrlChannelWithNoWait(stream,checkbuf,8);
	if (i < 8 || VK_memcmp(checkbuf,"ANNOUNCE",8))
	{
		PrintError("%s(%d) Error> Got invalid message(i:%d, %s)\n", __FUNCTION__, __LINE__, i, checkbuf);
		goto func_error;
	}

	RTSP_LIB_PutMessageForLaterRead(stream,checkbuf,i); /* push back "ANNOUNCE" */

	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get new mesage\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	nret = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);
	if (nret < 0)
	{
		PrintError("%s(%d) Error> ReadMessage failed\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	i_statusCode = rtsp_hdr->i_statusCode;

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
	p_description = DD_MEM_Alloc(i_len + 1);
	if (p_description == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(p_description ,0, i_len + 1);

	i_len = RTSP_LIB_ReadFromCtrlChannelWithWaitAll(stream,p_description,i_len);
	p_description[i_len] = '\0';

	STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	rtsp_hdr = NULL;

	if (i_statusCode != RTSP_STATUS_ANNOUNCE)
	{
		goto func_error;
	}

	/* got annouce packet, and data is in description */
	if (rtsp_ctrl->p_sessionNum == NULL)
	{
		PrintError("%s(%d) Error> Invalid session number\n", __FUNCTION__, __LINE__);
		/*goto func_error; */
	}
	else /* CID #46181 */
	{
		STREAM_RTSP_RequestMessageOK(stream,i_cseq,rtsp_ctrl->p_sessionNum); /* OK (required, according to RFC )*/
	}

	bret = P_RTSP_SESSION_ParseSDP (p_description, &pSdpContext);
	if (bret == FALSE)
	{
		PrintError("%s(%d) Error> SDP parsing failed\n", __FUNCTION__, __LINE__);
		/*goto func_error;*/
	}

	/* Be careful with Cseq: */

	/* SETUP */
	if (0)
	{
		nret = P_RTSP_SESSION_RequestSetup(stream, pSdpContext);
		if (nret < 0)
		{
			PrintError("%s(%d) Error> RTSP Setup failed\n", __FUNCTION__, __LINE__);
			goto func_error;
		}
	}

	/*  send PLAY request to download stream */
	if (P_RTSP_SESSION_RequestPlay(stream) < 0)
	{
		PrintError("%s(%d) Error> RTSP Play failed\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	RTSP_SDP_FreeContext(pSdpContext);
	DD_MEM_Free(p_description);
	p_description = NULL;
	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return TRUE;

func_error:
	PrintError("%s(%d) Error> func failed\n", __FUNCTION__, __LINE__);
	if (p_description != NULL)
	{
		DD_MEM_Free(p_description);
		p_description = NULL;
	}
	if (pSdpContext != NULL)
	{
		RTSP_SDP_FreeContext(pSdpContext);
	}
	if (rtsp_hdr != NULL) /* CID #46211 */
	{
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
	}
	if (buffer)
	{
		DD_MEM_Free(buffer);
		buffer = NULL;
	}
	PrintExit;
	return FALSE;
}


HINT32 RTSP_SESSION_RequestAsyncPlay(struct RTSP_ACCESS_SYS_s *stream, float scale)
{
	struct rtsp_ctrl_t *rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	char *buffer = NULL;
	HINT32 nret = 0;
	HBOOL b_includBandwidth = FALSE;
	HINT32 bandwidth_to_send = 0;
	/*char szNpt[256];*/

	PrintEnter;

	buffer = DD_MEM_Alloc(BUFSIZE_1K);
	if (buffer == NULL)
	{
		PrintError("%s(%d) Error> Out of memory!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(buffer ,0, BUFSIZE_1K);

	/* Sending part */
	rtsp_hdr = STREAM_RTSP_NewMessageHeaderAndFillBasicFields(rtsp_ctrl);
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get RTSP hdr\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	/*
	 * PLAY rtsp://10.0.13.142:8554/pub/_spiderman.ts RTSP/1.0
	 * Cseq: 3
	 * Session: F8072DE1
	 * User-Agent: RTSP-Client/9.0.0.2833 guid/B64345F5-8C45-4818-8A1A-4775F0923FAC
	 * Range: npt=0.000-
	 * Speed: 8.252
	 * Bandwidth: 2147483647
	 * Scale: 1.000
	 * Scale: 1.000
	 */
	STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,s_szRTSPUserAgent);

	/*
	VK_sprintf(szNpt, _DEF_RTSP_HDR_RANGE);
	VK_strcat(szNpt, _DEF_RTSP_HDR_NPT_NOW);
	*/

	if (1)
	{
		STREAM_RTSP_SetRangeField(rtsp_hdr,stream->dlopts->p_range);
	}

	if (1)
	{
		STREAM_RTSP_SetSpeedField(rtsp_hdr,stream->dlopts->p_speed);
	}

	/* bandwidth specified */
	if (0)
	{
		bandwidth_to_send = (stream->dlopts->i_bandwidth) ? (stream->dlopts->i_bandwidth) : INT_MAX_BANDWIDTH;

		VK_snprintf(buffer,BUFSIZE_1K - 1,"Bandwidth: %d",bandwidth_to_send);
		STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,buffer);
	}

	if (0)
	{
		//if (!(scale > 0.9 && scale < 1.1))
		//{
			VK_snprintf(buffer,BUFSIZE_1K - 1,"Scale: %.3f", scale);
			PrintDebug("%s(%d) Scale String is %s\n", __FUNCTION__, __LINE__, buffer);
			STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,buffer);
		//}
    }

	if (b_includBandwidth == TRUE)
	{
		VK_snprintf(buffer,BUFSIZE_1K - 1, "X-Accelerate-Streaming: AccelDuration = 18000;AccelBandwidth=%d",bandwidth_to_send);
		STREAM_RTSP_SetMessageHeaderField(rtsp_hdr,buffer);
	}

	STREAM_RTSP_SetMessageHeaderMethod(rtsp_hdr,STREAM_RTSP_CMD_PLAY);
	STREAM_RTSP_SetMessageHeaderURI(rtsp_hdr,rtsp_ctrl->p_mrl);
	STREAM_RTSP_SerializeMessageHeader(rtsp_hdr);
	STREAM_RTSP_RequestMessageAndFree(stream,rtsp_hdr);

	DD_MEM_Free(buffer);
	buffer = NULL;
	PrintExit;
	return nret;

func_error:
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
static HINT32 P_RTSP_SESSION_RequestPlay(struct RTSP_ACCESS_SYS_s *stream) /* Unused, so commented */
{
	struct RTSP_PROT_HEADER_s *rtsp_hdr = NULL;
	struct rtsp_ctrl_t *rtsp_ctrl = NULL;
	char *pField = NULL;
	char *p_rtpinfo_url = NULL;

	HINT32 nret = 0;

	PrintEnter;

	if ((stream == NULL) || (stream->stream_ctrl == NULL))
	{
		PrintError("%s(%d) Error> stream_ctrl is NULL\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	rtsp_ctrl = stream->stream_ctrl->rtsp_ctrl;


	/* Sending part */
	nret = RTSP_SESSION_RequestAsyncPlay(stream, 1.000);
	if (nret < 0)
	{
		PrintError("%s(%d) Error> RequestAsyncPlay failed(%d)\n", __FUNCTION__, __LINE__, nret);
		goto func_error;
	}

	/* Receiving part */

	/* receive message for PLAY request */
	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Can't get new msg header\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	nret = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);
	if (nret < 0)
	{
		PrintError("%s(%d) Error> ReadMessage failed(%d)\n", __FUNCTION__, __LINE__, nret);
		goto func_error;
	}

	if (!RTSP_LIB_CheckMessageResponseOK(nret))
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
		goto func_error;
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
			PrintData("%s(%d) [Speed: %s]\n", __FUNCTION__, __LINE__, pField);
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
			PrintData("%s(%d) [Range: %s]\n", __FUNCTION__, __LINE__, pField);
		}
	}

	/*
	 * RTSP/1.0 200 OK
	 * CSeq: 3
	 * Date: Wed, Sep 12 2012 14:25:00 GMT
	 * Scale: 1.000000
	 * Range: npt=0.000-
	 * Session: B7C4B833
	 * RTP-Info: url=rtsp://10.0.13.142:8554/pub/_spiderman.ts/track1;seq=62238;rtptime=3659094097
	 */
	/* Parse the RTP-Info fields */
	pField = STREAM_RTSP_FindMessageHeaderField(rtsp_hdr,"RTP-Info");
	if (pField == NULL)
	{
		PrintError("%s(%d) Error> No RTP-Info string in PLAY response\n", __FUNCTION__, __LINE__);

		/* Free rtp-info related params */
		if (rtsp_ctrl->p_rtpinfo)
		{
			DD_MEM_Free(rtsp_ctrl->p_rtpinfo);
			rtsp_ctrl->p_rtpinfo = NULL;

			/* CID #46183 */
			/*
			while(*pField == ' ') pField++;
			rtsp_ctrl->p_rtpinfo = VK_StrDup(pField);
			*/
		}

		if (rtsp_ctrl->p_rtpinfo_url != NULL)
		{
			PrintError("%s(%d) Error> Remove prev RTP-Info:url(%s)\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_rtpinfo_url);
			DD_MEM_Free(rtsp_ctrl->p_rtpinfo_url);
			rtsp_ctrl->p_rtpinfo_url = NULL;
		}
	}
	else
	{
		PrintDebug("%s(%d) RTP-Info field:%s\n", __FUNCTION__, __LINE__, pField);

		/* RTP-Info: url=rtsp://10.0.13.142:8554/pub/_spiderman.ts/track1;seq=58343;rtptime=4035134626 */
		p_rtpinfo_url = P_RTSP_SESSION_ParseRTPInfoURL (pField);
		if (!p_rtpinfo_url)
		{
			PrintError("%s(%d) Error> RTP-Info:url field failed\n", __FUNCTION__, __LINE__);
			/*p_rtpinfo_url = VK_StrDup (SDP_server_addr);*/

			if (rtsp_ctrl->p_rtpinfo_url != NULL)
			{
				PrintError("%s(%d) Error> Remove prev RTP-Info:url(%s)\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_rtpinfo_url);
				DD_MEM_Free(rtsp_ctrl->p_rtpinfo_url);
				rtsp_ctrl->p_rtpinfo_url = NULL;
			}
		}
		else
		{
			PrintError("%s(%d) Error(info)> Got RTP-Info:url(%s)\n", __FUNCTION__, __LINE__, p_rtpinfo_url);

			if (rtsp_ctrl->p_rtpinfo_url != NULL)
			{
				PrintError("%s(%d) Error> Remove prev RTP-Info:url(%s)\n", __FUNCTION__, __LINE__, rtsp_ctrl->p_rtpinfo_url);
				DD_MEM_Free(rtsp_ctrl->p_rtpinfo_url);
				rtsp_ctrl->p_rtpinfo_url = NULL;
			}
			rtsp_ctrl->p_rtpinfo_url = VK_StrDup(p_rtpinfo_url);
			if (rtsp_ctrl->p_rtpinfo == NULL)
			{
				PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
				goto func_error;
			}
		}

		PrintDebug("%s(%d) RTSP RTP-Info: \n", __FUNCTION__, __LINE__);
		PrintDebug("%s(%d) + p_rtpinfo_url:%s \n", __FUNCTION__, __LINE__, p_rtpinfo_url);

		/* Store rtpinfo field */
		if (rtsp_ctrl->p_rtpinfo)
		{
			DD_MEM_Free(rtsp_ctrl->p_rtpinfo);
			rtsp_ctrl->p_rtpinfo = NULL;

			while(*pField == ' ') pField++;
			rtsp_ctrl->p_rtpinfo = VK_StrDup(pField);
			if (rtsp_ctrl->p_rtpinfo == NULL)
			{
				PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
				goto func_error;
			}
		}
	}

	/* skip content-length bytes from network */
	STREAM_RTSP_ReadMessageAndFreeBody(stream,rtsp_hdr);
	STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	rtsp_hdr = NULL;

	if (p_rtpinfo_url != NULL) /* CID #46212 */
	{
		DD_MEM_Free(p_rtpinfo_url);
		p_rtpinfo_url = NULL;
	}

	PrintExit;
	return nret;

func_error:
	PrintError("%s(%d) Error> function failed\n", __FUNCTION__, __LINE__);
	if (rtsp_hdr != NULL)
	{
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
		rtsp_hdr = NULL;
	}

	if (p_rtpinfo_url != NULL)
	{
		DD_MEM_Free(p_rtpinfo_url);
		p_rtpinfo_url = NULL;
	}
	PrintExit;
	return (-1);
}


/*
 * read from media stream. (rdp packet)
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
HINT32 RTSP_SESSION_RequestSetup(struct RTSP_ACCESS_SYS_s *stream)
{
	HINT32 nret = 0;
	struct RTSP_CHANNEL_t *pDataChannel = NULL;
	HBOOL bUseOobChannel = FALSE;

	PrintEnter;

	if ((stream == NULL) || (stream->dlopts == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if ((stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_UDP_UNICAST) ||
		(stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_RTP_UNICAST))
	{
		/* For UDP or RTP channel, check channel validation because it maybe has un-alloced status */
		if (stream->dataChannel[2] != NULL)
		{
			PrintError("%s(%d) Error(info)> Already allocated data channel\n", __FUNCTION__, __LINE__);
			goto func_error;
		}

		stream->dataChannel[2] = RTSP_LIB_NewChannel();
		if (stream->dataChannel[2] == NULL)
		{
			PrintError("%s(%d) Error> cannot create data channel\n",__FUNCTION__,__LINE__);
			goto func_error;
		}

		if (stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_UDP_UNICAST)
		{
			stream->dataChannel[2]->transport_type = RTSP_TRANSPORT_UDP_UNICAST;
		}
		else if (stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_RTP_UNICAST)
		{
			stream->dataChannel[2]->transport_type = RTSP_TRANSPORT_RTP_UNICAST;
		}

		stream->dataChannel[2]->b_connected = FALSE;
		stream->dataChannel[2]->b_started = FALSE;
		stream->dataChannel[2]->i_port = _RTSP_OOB_CHANNEL_PORT;
		stream->dataChannel[2]->i_prevRtpSeq = 0;
	}


	/* select data channel */
	if (stream->dlopts->data_transport_protocol ==  RTSP_TRANSPORT_TCP)
	{
		pDataChannel = stream->ctrlChannel;
		bUseOobChannel = FALSE;
	}
	else
	{
		pDataChannel = stream->dataChannel[2];
		bUseOobChannel = TRUE;
	}

	if (pDataChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL data channel \n",__FUNCTION__,__LINE__);
		goto func_error;
	}

	/* start the channel */
	PrintDebug("%s(%d) Error(info)> current channel: ctrl[connected:%d, started:%d], session_status:%d], oob[connected:%d, started:%d]\n",
				__FUNCTION__, __LINE__,
				stream->ctrlChannel->b_connected, stream->ctrlChannel->b_started,
				stream->stream_ctrl->session_status,
				pDataChannel->b_connected, pDataChannel->b_started);

	if (pDataChannel->b_started == FALSE)
	{
		PrintError("%s(%d) Error(into)> data channel status(data_transport_protocol:%d)\n",
					__FUNCTION__, __LINE__,
					stream->dlopts->data_transport_protocol);

		nret = P_RTSP_SESSION_RequestSetup(stream, stream->stream_ctrl->pSdpContext);
		if (nret < 0)
		{
			PrintError("%s(%d) Error> SETUP request failed\n", __FUNCTION__, __LINE__);
			goto func_error;
		}
	}

	PrintDebug("%s(%d) Error(info)> --ctrl[connected:%d, started:%d], session_status:%d], oob[connected:%d, started:%d]\n",
				__FUNCTION__, __LINE__,
				stream->ctrlChannel->b_connected, stream->ctrlChannel->b_started,
				stream->stream_ctrl->session_status,
				pDataChannel->b_connected, pDataChannel->b_started);
  	PrintExit;
	return nret;

func_error:
	PrintDebug("%s(%d) Error(info)> --ctrl[connected:%d, started:%d], session_status:%d], oob[connected:%d, started:%d]\n",
				__FUNCTION__, __LINE__,
				stream->ctrlChannel->b_connected, stream->ctrlChannel->b_started,
				stream->stream_ctrl->session_status,
				pDataChannel->b_connected, pDataChannel->b_started);
  	PrintExit;
	return -1;
}

HINT32 RTSP_SESSION_RequestPlay(struct RTSP_ACCESS_SYS_s *stream)
{
	HINT32 nret = 0;
	struct RTSP_CHANNEL_t *pDataChannel = NULL;
	HBOOL bUseOobChannel = FALSE;

	PrintEnter;

	if ((stream == NULL) || (stream->dlopts == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if ((stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_UDP_UNICAST) ||
		(stream->dlopts->data_transport_protocol == RTSP_TRANSPORT_RTP_UNICAST))
	{
		/* For UDP or RTP channel, check channel validation because it maybe has un-alloced status */
		if (stream->dataChannel[2] == NULL)
		{
			PrintError("%s(%d) Error(info)> Non-allocated data channel\n", __FUNCTION__, __LINE__);
			goto func_error;
		}
	}

	/* select data channel */
	if (stream->dlopts->data_transport_protocol ==  RTSP_TRANSPORT_TCP)
	{
		pDataChannel = stream->ctrlChannel;
		bUseOobChannel = FALSE;
	}
	else
	{
		pDataChannel = stream->dataChannel[2];
		bUseOobChannel = TRUE;
	}

	if (pDataChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL data channel \n",__FUNCTION__,__LINE__);
		goto func_error;
	}

	/* start the channel */
	PrintDebug("%s(%d) Error(info)> current channel: ctrl[connected:%d, started:%d], session_status:%d], oob[connected:%d, started:%d]\n",
				__FUNCTION__, __LINE__,
				stream->ctrlChannel->b_connected, stream->ctrlChannel->b_started,
				stream->stream_ctrl->session_status,
				pDataChannel->b_connected, pDataChannel->b_started);

	if (pDataChannel->b_started == FALSE)
	{
		PrintError("%s(%d) Error(into)> data channel status(data_transport_protocol:%d)\n",
					__FUNCTION__, __LINE__,
					stream->dlopts->data_transport_protocol);

		nret = P_RTSP_SESSION_RequestPlay(stream);
		if (nret < 0)
		{
			PrintError("%s(%d) Error> PLAY request failed\n", __FUNCTION__, __LINE__);
			goto func_error;
		}

		/* Updata control channel status */
		if (stream->ctrlChannel->b_connected == FALSE)
		{
			PrintError("%s(%d) Error> Invalid control channel status(connected:%d)\n",
						__FUNCTION__, __LINE__, stream->ctrlChannel->b_connected);
			stream->ctrlChannel->b_connected = TRUE;
		}
		stream->ctrlChannel->b_started = TRUE;

		/* Update data channel status */
		pDataChannel->b_started = TRUE;
		if (stream->stream_ctrl->session_status != RTSP_SESSION_STATUS_STARTED)
		{
			stream->stream_ctrl->session_status = RTSP_SESSION_STATUS_STARTED;
		}
	}

	PrintDebug("%s(%d) Error(info)> --ctrl[connected:%d, started:%d], session_status:%d], oob[connected:%d, started:%d]\n",
				__FUNCTION__, __LINE__,
				stream->ctrlChannel->b_connected, stream->ctrlChannel->b_started,
				stream->stream_ctrl->session_status,
				pDataChannel->b_connected, pDataChannel->b_started);
  	PrintExit;
	return nret;

func_error:
	PrintDebug("%s(%d) Error(info)> --ctrl[connected:%d, started:%d], session_status:%d], oob[connected:%d, started:%d]\n",
				__FUNCTION__, __LINE__,
				stream->ctrlChannel->b_connected, stream->ctrlChannel->b_started,
				stream->stream_ctrl->session_status,
				pDataChannel->b_connected, pDataChannel->b_started);
  	PrintExit;
	return -1;
}

#if 0 /* Currently, unused at standard RTSP (OOB channel) */
HINT32 RTSP_SESSION_StartDataChannelIfNotStarted(struct RTSP_ACCESS_SYS_s *stream)
{
	return RTSP_SESSION_RequestPlay(stream);
}
#endif

/*
 * process rtsp header coming among rtsp server media packet
 *
 * return value:	   status code ... normal proceed case
 *							   0 ... EOF packet
 *							  -1 ... error
 */
static HINT32 P_RTSP_SESSION_ProcessMessageResponse(struct RTSP_ACCESS_SYS_s *stream)
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
		goto func_error;
	}
	stream_ctrl = stream->stream_ctrl;

	if (stream_ctrl->rtsp_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp_ctrl\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	rtsp_ctrl = stream_ctrl->rtsp_ctrl;

	/* push back 4 bytes */
	rtsp_hdr = STREAM_RTSP_NewMessageHeader();
	if (rtsp_hdr == NULL)
	{
		PrintError("%s(%d) Error> Cant get RTSP header!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	i_statusCode = STREAM_RTSP_ReadMessageAndParseBasic(stream,rtsp_hdr);
	if (i_statusCode < 0)
	{
		PrintError("%s(%d) Error> Invalid status code(%d)\n", __FUNCTION__, __LINE__, i_statusCode);
		goto func_error;
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
			goto func_error;
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
		if (i_cseq != (HINT32)rtsp_ctrl->i_cseq - 1) /* rtsp_ctrl->i_cseq increment in send */
		{
			PrintError("%s(%d) Error> CSeq mismatch: expected %d, got %d",
					__FUNCTION__, __LINE__, rtsp_ctrl->i_cseq - 1,i_cseq);
		}
		/* DO NOT send OK back to this */
	}
	else if (!VK_strncasecmp(rtsp_hdr->p_protocol,"SET_PARAMETER",13)) /* SET_PARAMETER */
	{
#if 0
		char *p;

		/* EOF 는 SET_PARAMETER로 온다. 이후 서버측에서 소켓을 닫는다. */
		/* 닫힌 소켓을 사용하면 131 sock error, 비정상 종료 현상이 나타날 수 잇다. */

		/* MS-RTSP X-Notice(with End-of-Stream Reached), X-RTP-Info(with last RTP seqN), */
		/* X-Playlist-Gen-Id 가 오면 Body를 보고 세부 판단을 한다. */
		if (body && (p = VK_strstr((char *)body,"EOF:")))  // body must exist <<- fix 1.2.2
		{
			p += 4;
			while(*p == ' ') p++;

			if ((!VK_strncasecmp(p,"true",4))) /* EOF */
			{
				stream_ctrl->session_status = RTSP_SESSION_STATUS_FINISHED;
    				PrintDebug("%s(%d) RTSP_SESSION_STATUS_FINISHED \n", __FUNCTION__, __LINE__);
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
				return 0;
			}
		}
#endif
		if (rtsp_ctrl->p_sessionNum == NULL)
		{
			PrintError("%s(%d) Error> Invalid session number\n", __FUNCTION__, __LINE__);
			/*goto func_error; */
		}
		else /* CID #46182 */
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
			/*goto func_error; */
		}
		else /* CID #46182 */
		{
			STREAM_RTSP_RequestMessageOK(stream,i_cseq,rtsp_ctrl->p_sessionNum);
		}
	}

	if (body)
	{
		DD_MEM_Free(body);
		body = NULL;
	}

	if (rtsp_hdr != NULL)
	{
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	}
	PrintExit;
	return i_statusCode;


func_error:
	if (body)
	{
		DD_MEM_Free(body);
		body = NULL;
	}

	if (rtsp_hdr != NULL)
	{
		STREAM_RTSP_FreeMessageHeader(rtsp_hdr);
	}
	PrintExit;
	return -1;
}

