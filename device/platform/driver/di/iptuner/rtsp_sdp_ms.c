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

#include "di_uart.h"
#include "rtsp_lib.h"

#include "util_b64_encode.h"
#include "rtsp_sdp_ms.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define RTSP_SDP_MS_DEBUG
//#define RTSP_SDP_MS_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef RTSP_SDP_MS_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
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
#endif /* The END of DEBUG DEFINE */

#ifdef RTSP_SDP_MS_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((HINT32 (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)



static void P_RTSP_SDP_MS_FreeMediaDesc(struct RTSP_SDP_MS_MediaDesc_s *p);


struct RTSP_MS_SdpMsCtx_s *RTSP_SDP_MS_NewSdpMsCtx(void)
{
	struct RTSP_MS_SdpMsCtx_s *p;
	PrintEnter;
	p = DD_MEM_Alloc(sizeof(struct RTSP_MS_SdpMsCtx_s));
	VK_memset(p ,0, sizeof(struct RTSP_MS_SdpMsCtx_s));
	PrintExit;
	return p;
}



void RTSP_SDP_MS_FreeSdpMsCtx(struct RTSP_MS_SdpMsCtx_s *p)
{
	PrintEnter;
	DD_MEM_Free(p->pRawAsfHeader);
	p->pRawAsfHeader = NULL;
	RTSP_LIB_FreeLikedList(p->streams,(void (*)(void *))P_RTSP_SDP_MS_FreeMediaDesc);

	DD_MEM_Free(p);
	p = NULL;
	PrintExit;
}





static void P_RTSP_SDP_MS_FreeMediaDesc(struct RTSP_SDP_MS_MediaDesc_s *p)
{
	PrintEnter;
	DD_MEM_Free(p->id);
	p->id = NULL;
	DD_MEM_Free(p->control);
	p->control = NULL;
	DD_MEM_Free(p);
	p = NULL;
	PrintExit;
}



struct RTSP_SDP_MS_MediaDesc_s *RTSP_SDP_MS_ParseMediaDesc(char **data)
{
	struct RTSP_SDP_MS_MediaDesc_s *stream;
	char *p = *data;
	char *buf = NULL;
	HINT32 buf_len = 0; /* don't touch it!! after here */
	char *decoded = NULL;
	HINT32 decoded_len = 0; /* don't touch it after here*/
	decoded_len = 0; /* shut up compiler !! */

	PrintEnter;

	stream = DD_MEM_Alloc(sizeof(struct RTSP_SDP_MS_MediaDesc_s));
	VK_memset(stream ,0, sizeof(struct RTSP_SDP_MS_MediaDesc_s));

	if(RTSP_LIB_ThisLineHasKeyword(p,"m=",&buf,&buf_len)) {
	stream->id = VK_StrDup(buf);
	PrintDebug("stream: %s\n",stream->id);
	p = RTSP_LIB_GetNextLine(p);
	}
	else {
	PrintCriticalDebug("no 'm=' line\n");
	P_RTSP_SDP_MS_FreeMediaDesc(stream);
	return NULL;
	}

	for( ; p && *p && (p[0] != 'm'); ) {

	if(RTSP_LIB_ThisLineHasKeyword(p,"a=control:",&buf,&buf_len)) {
		stream->control = VK_StrDup(buf);
		p = RTSP_LIB_GetNextLine(p);
	}
	else if(RTSP_LIB_ThisLineHasKeyword(p,"a=stream:",&buf,&buf_len)) {
		stream->streamnum = VK_atoi(buf);
		p = RTSP_LIB_GetNextLine(p);
	}
	else {
		p = RTSP_LIB_GetNextLine(p);
	}
	}

	DD_MEM_Free(buf);
	if(decoded)
	{
		DD_MEM_Free(decoded);
		decoded = NULL;
	}

	*data = p;

	PrintExit;

	return stream;
}



/*
 * parse sdp. compared to real-rtsp, it doesn't do much here
 * because asf_header will have all stream information.
 * thus it only does simplest stuff, extract asf header.
 */
struct RTSP_MS_SdpMsCtx_s *RTSP_SDP_MS_ParseSdp(char *data)
{
	char *p;					  /* always points first char of line in 'data' */
	char *buf = NULL;
	HINT32 buf_len = 0;			  /* don't touch it after here !!*/
	char *decoded = NULL;
	HINT32 decoded_len = 0;		  /* don't touch it after here   */
	struct RTSP_MS_SdpMsCtx_s *desc = NULL;
	struct RTSP_SDP_MS_MediaDesc_s *stream = NULL;

	PrintEnter;

	desc = RTSP_SDP_MS_NewSdpMsCtx();

	for(p = data ; p && *p ; ) {

	if(RTSP_LIB_ThisLineHasKeyword(p,"a=pgmpu:data:application/vnd.ms.wms-hdr.asfv1;base64,",&buf,&buf_len)) {
		RTSP_LIB_ShrinkBufferIfPossible(&decoded,&decoded_len,VK_strlen(buf));
		desc->nRawAsfHeaderLen = base64_decode(buf,decoded,decoded_len);
		if(desc->nRawAsfHeaderLen <= -1)
		{
			PrintError("base64_decode error\n");
			goto failed;
		}
		desc->pRawAsfHeader = DD_MEM_Alloc(desc->nRawAsfHeaderLen);
		if(desc->pRawAsfHeader == NULL)
		{
			PrintError("mem alloc error\n");
			goto failed;
		}

		VK_memset(desc->pRawAsfHeader ,0, desc->nRawAsfHeaderLen);
		VK_memcpy(desc->pRawAsfHeader,decoded,desc->nRawAsfHeaderLen);
		p = RTSP_LIB_GetNextLine(p);
	}
	else if(RTSP_LIB_ThisLineHasKeyword(p,"m=",&buf,&buf_len)) { /* stream description */
		stream = RTSP_SDP_MS_ParseMediaDesc(&p);	/* p is at the end of stream desc */
		RTSP_LIB_AppendToLinkedListHdr(&desc->streams,stream);
	}
	else {
		p = RTSP_LIB_GetNextLine(p);
	}
	}

failed:
	if(buf)
	{
		DD_MEM_Free(buf);
		buf = NULL;
	}

	if(decoded)
	{
		DD_MEM_Free(decoded);
		decoded = NULL;
	}
  PrintExit;
	return desc;
}


