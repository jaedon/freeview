/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   rtsp_sdp.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	JungWook Chai
 * Current Author:	$Author: juchae@humaxdigital.com $
 * Date:			$Date: Thur Oct 29 10:20:48 KST 2009 $
 * File Description:	Real-Time STREAMMING IP Tuner Header File
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
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
#include "rtsp_sdp.h"
#include "di_iptuner_config.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define RTSP_SDP_DEBUG
//#define RTSP_SDP_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef RTSP_SDP_DEBUG
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

#ifdef RTSP_SDP_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((HINT32 (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)


static void P_RTSP_SDP_FreeMediaDesc(struct RTSP_SDP_MediaDesc_s *ptr)
{
	PrintEnter;

	if (ptr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL mediaDesc!\n", __FUNCTION__, __LINE__);
		return;
	}

	if (ptr->p_id != NULL)
	{
		DD_MEM_Free(ptr->p_id);
		ptr->p_id = NULL;
	}

	if (ptr->p_control != NULL)
	{
		DD_MEM_Free(ptr->p_control);
		ptr->p_control = NULL;
	}
	DD_MEM_Free(ptr);
	ptr = NULL;
	PrintExit;
}

struct RTSP_SDP_Context_s *RTSP_SDP_NewContext(void)
{
	struct RTSP_SDP_Context_s *ptr = NULL;

	PrintEnter;

	ptr = DD_MEM_Alloc(sizeof(struct RTSP_SDP_Context_s));
	if (ptr == NULL)
	{
		PrintError("%s(%d) Error> Out of memory!\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_memset(ptr ,0, sizeof(struct RTSP_SDP_Context_s));
	PrintExit;
	return ptr;
}


void RTSP_SDP_FreeContext(struct RTSP_SDP_Context_s *ptr)
{
	PrintEnter;

	if (ptr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL arg\n", __FUNCTION__, __LINE__);
		return;
	}

	RTSP_LIB_FreeLikedList(ptr->streams,(void (*)(void *))P_RTSP_SDP_FreeMediaDesc);

	DD_MEM_Free(ptr);
	ptr = NULL;
	PrintExit;
}

static struct RTSP_SDP_MediaDesc_s *P_RTSP_SDP_ParseMediaDesc(char **ppData)
{
	struct RTSP_SDP_MediaDesc_s *p_mediaDesc = NULL;
	char *ptr = *ppData;
	char *buf = NULL;
	HINT32 buf_len = 0; /* don't touch it!! after here */
	char *decoded = NULL;
	HINT32 decoded_len = 0; /* don't touch it after here*/
	decoded_len = 0; /* shut up compiler !! */

	PrintEnter;

	if (ptr == NULL)
	{
		PrintError("%s(%d) Error> Invalid arg!\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	p_mediaDesc = DD_MEM_Alloc(sizeof(struct RTSP_SDP_MediaDesc_s));
	if (p_mediaDesc == NULL)
	{
		PrintError("%s(%d) Error> Out of memory!\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_memset(p_mediaDesc ,0, sizeof(struct RTSP_SDP_MediaDesc_s));

	if (RTSP_LIB_ThisLineHasKeyword(ptr,"m=",&buf,&buf_len))
	{
		p_mediaDesc->p_id = VK_StrDup(buf);
		PrintDebug("%s(%d) stream id: %s\n", __FUNCTION__, __LINE__, p_mediaDesc->p_id);
		ptr = RTSP_LIB_GetNextLine(ptr);
	}
	else
	{
		PrintError("s(%d) Error> no 'm=' line\n", __FUNCTION__, __LINE__);
		P_RTSP_SDP_FreeMediaDesc(p_mediaDesc);
		return NULL;
	}

	for( ; ptr && *ptr && (ptr[0] != 'm'); )
	{
		if (RTSP_LIB_ThisLineHasKeyword(ptr,"a=control:",&buf,&buf_len))
		{
			p_mediaDesc->p_control = VK_StrDup(buf);
			if (p_mediaDesc->p_control == NULL)
			{
				PrintError("%s(%d) Error> Out of memory!\n", __FUNCTION__, __LINE__);
				if (p_mediaDesc) /* CID #46209 */
				{
					DD_MEM_Free(p_mediaDesc);
					p_mediaDesc = NULL;
				}
				return NULL;
			}
			ptr = RTSP_LIB_GetNextLine(ptr);
		}
		else if (RTSP_LIB_ThisLineHasKeyword(ptr,"a=stream:",&buf,&buf_len))
		{
			p_mediaDesc->i_stream_num = VK_atoi(buf);
			ptr = RTSP_LIB_GetNextLine(ptr);
		}
		else
		{
			ptr = RTSP_LIB_GetNextLine(ptr);
		}
	}

	DD_MEM_Free(buf);
	if(decoded)
	{
		DD_MEM_Free(decoded);
		decoded = NULL;
	}
	*ppData = ptr;

	PrintExit;
	return p_mediaDesc;
}


/*
 * parse sdp. compared to real-rtsp, it doesn't do much here
 * because asf header will have all stream information.
 * thus it only does simplest stuff, extract asf header.
 */
struct RTSP_SDP_Context_s *RTSP_SDP_ParseDescription(char *pData)
{
	char *ptr = NULL;             /* always points first char of line in 'data' */
	char *buf = NULL;
	HINT32 buf_len = 0;			  /* don't touch it after here !!*/
	char *decoded = NULL;
	struct RTSP_SDP_Context_s *p_sdpContext = NULL;
	struct RTSP_SDP_MediaDesc_s *p_mediaDesc = NULL;

	PrintEnter;

	p_sdpContext = RTSP_SDP_NewContext();
	if (p_sdpContext == NULL)
	{
		PrintError("%s(%d) Error> Can't get new ctx!\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	for(ptr = pData ; ptr && *ptr ; )
	{
#if 0
		if (RTSP_LIB_ThisLineHasKeyword(ptr,"a=pgmpu:data:application/vnd.ms.wms-hdr.asfv1;base64,",&buf,&buf_len))
		{
			RTSP_LIB_ShrinkBufferIfPossible(&decoded,&decoded_len,VK_strlen(buf));
			p_sdpContext->nRawAsfHeaderLen = base64_decode(buf,decoded,decoded_len);
			if(p_sdpContext->nRawAsfHeaderLen <= -1)
			{
				PrintError("%s(%d) Error> base64_decode failed\n", __FUNCTION__, __LINE__);
				goto func_error;
			}
			p_sdpContext->pRawAsfHeader = DD_MEM_Alloc(p_sdpContext->nRawAsfHeaderLen);
			if(p_sdpContext->pRawAsfHeader == NULL)
			{
				PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
				goto func_error;
			}

			VK_memset(p_sdpContext->pRawAsfHeader ,0, p_sdpContext->nRawAsfHeaderLen);
			VK_memcpy(p_sdpContext->pRawAsfHeader,decoded,p_sdpContext->nRawAsfHeaderLen);
			ptr = RTSP_LIB_GetNextLine(ptr);
		}
		else
#endif
		if(RTSP_LIB_ThisLineHasKeyword(ptr,"m=",&buf,&buf_len)) { /* stream description */
			p_mediaDesc = P_RTSP_SDP_ParseMediaDesc(&ptr);	/* ptr is at the end of stream desc */
			if (p_mediaDesc == NULL)
			{
				PrintError("%s(%d) Error> Can't get media desc\n", __FUNCTION__, __LINE__);
				/*goto func_error;*/
			}
			RTSP_LIB_AppendToLinkedListHdr(&p_sdpContext->streams,p_mediaDesc);
		}
		else
		{
			ptr = RTSP_LIB_GetNextLine(ptr);
		}
	}

/*func_error:*/
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
	return p_sdpContext;
}

