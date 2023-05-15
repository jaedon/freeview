/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   sstr_controller.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		YoungKi Kim $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2012.02.28
 * File Description:	SSTR Controller
 * Module:
 * Remarks:
 */

/**
 * @defgroup SSTR_CONTROLLER
 * @brief SSTR Controller 이며 Manifest 운용을 위한 역할을 한다.
 */

/**
 * @author YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 28 Feb 2012
 */

/**
 * @brief SSTR Controller 이며 Manifest 운용을 위한 역할을 한다.
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
 * @file sstr_controller.c
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

#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "di_iptuner_config.h"
#include "sstr_controller.h"
#include "sstr_buffermgr.h"
#include "sstr_heuristics.h"
#include "htype.h"
#include "di_uart.h"

#include "vkernel.h"
#include "taskdef.h"
#include "util_url.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define SSTR_CONTROLLER_DEBUG
//#define DI_IPTUNER_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef SSTR_CONTROLLER_DEBUG
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
#endif

#ifdef DI_IPTUNER_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)

/*----------------------------------------------------------------------------------------------
 *	MACRO
 *---------------------------------------------------------------------------------------------*/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/**
 * SSTR CONTROLLER Handle Structure
 */
typedef struct SSTR_CONTROLLER_s
{
	/* Current Info */
	SSTR_STREAM_TYPE_e eCurType; /**< 현재 Download 중인/할 Fragment 의 TYPE (VIDEO/AUDIO) */

	/* Stream Index(It is not Real Index in Manifest. it is just address in Stream Array) */
	HINT32 nCurVideoIndex; /**< 현재 Download 중인/할 Video의 Stream Index (Array Index) */
	HINT32 nCurAudioIndex; /**< 현재 Download 중인/할 Audio의 Stream Index (Array Index) */

	/* Track Index(It is not Real Index in Manifest. it is just address in Track Array)*/
//	HINT32 nCurVideoTrack; /**< 현재 Download 중인/할 Video의 Track Index (Array Index) */
//	HINT32 nCurAudioTrack; /**< 현재 Download 중인/할 Audio의 Track Index (Array Index) */

	HULONG ulSema; /**< Semaphore */

	SSTR_HEURISTICS_t *hHeuristics; /**< Heuristics Handle */

	/* for SSTR Control */
	HINT8 *pBaseUrl; /**< Manifest Url (Fragment Download 시 Base URL로 사용)*/
	sstrParser_t *parser; /**< Manifest Parser Handle */
	HBOOL bTargetChanged; /**< not supported yet */
	HUINT64 ullMaxBitrate; /**< not supported yet */
	HUINT64 ullMinBitrate; /**< not supported yet */

	SSTR_BUFFERMGR_t *hBufferMgr; /**< BufferManager Handle */

	SSTR_MANIFEST_Info_t *pManifest; /**< not supported yet */
} SSTR_CONTROLLER_t;

/* End typedef */

#if _________static________
#endif

/*----------------------------------------------------------------------------------------------
 *	static functions
 *---------------------------------------------------------------------------------------------*/
#if __________util_________
#endif

static inline sstrMedia_t *__sstrMedia(void *media) { return (sstrMedia_t*)media; }
static inline SSTR_CONTROLLER_t *__controller(SSTR_CTRL *hCtrl) { return (SSTR_CONTROLLER_t*)hCtrl; }

static void strreverse(char* begin, char* end) {
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}

#if 0 /* avoid compile warning */
static void itoa(int value, char* str, int base) {
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	int sign;
	div_t res;

	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }

	// Take care of sign
	if ((sign=value) < 0) value = -value;
	// Conversion. Number is reversed.
	do {
		res = div(value,base);
		*wstr++ = num[res.rem];
	}while(value=res.quot);
	if(sign<0) *wstr++='-';	*wstr='\0';

	// Reverse string
	strreverse(str,wstr-1);
}
#endif

static void uitoa(unsigned int value, char* str, int base) {
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
//	unsigned int sign;
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
//	if(sign<0) *wstr++='-';	*wstr='\0';

	// Reverse string
	strreverse(str,wstr-1);
}

static void uitoa64(unsigned long long value, char* str, int base) {
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
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

#if 0 /* xxx ykkim */
static void uitoa64(unsigned long long value, char* str, int base) {
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	unsigned long long sign;
	ldiv_t res;

	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }

	// Take care of sign
//	if ((sign=value) < 0) value = -value;
	// Conversion. Number is reversed.
	do {
		res = ldiv(value,base);
		*wstr++ = num[res.rem];
	}while(value=res.quot);

//	if(sign<0) *wstr++='-';
	*wstr='\0';

	// Reverse string
	strreverse(str,wstr-1);
}
#endif

#if __________p__________
#endif

/**
 * @b Function @b Description <br>
 * SSTR CONTROLLER 에 대한 Lock을 설정한다. <br>
 *
 * @param[in] hCtrl	:  SSTR CONTROLLER HANDLE
 *
 * @return  #  SSTR_ERR <br>
 */
SSTR_ERR P_SSTR_CONTROLLER_Lock(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	VK_SEM_Get(pController->ulSema);
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * SSTR CONTROLLER 에 대한 Lock을 해제한다. <br>
 *
 * @param[in] hCtrl	:  SSTR CONTROLLER HANDLE
 *
 * @return  #  SSTR_ERR <br>
 */
SSTR_ERR P_SSTR_CONTROLLER_Unlock(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	VK_SEM_Release(pController->ulSema);
	return SSTR_ERR_OK;
}


static SSTR_ERR P_SSTR_CONTROLLER_ResizeUrlBuffer(SSTR_URL_t *pUrl, HUINT32 unResize)
{
	HINT8 *pNewBuffer = NULL;
	HINT8 *pNewSwapBuffer = NULL;
	HUINT32 unNewBufSize = 0;
	if(pUrl == NULL)
	{
		PrintError("%s(%d) Err> URL_t is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	unNewBufSize = unResize + SSTR_CONST_URL_PADDING;
	pNewBuffer = VK_MEM_Alloc(unNewBufSize);
	if(pNewBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer Overflow\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pNewSwapBuffer = VK_MEM_Alloc(unNewBufSize);
	if(pNewSwapBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer Overflow\n",__FUNCTION__,__LINE__);
		if(pNewBuffer != NULL)
		{
			VK_MEM_Free(pNewBuffer);
			pNewBuffer = NULL;
		}
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	VK_MEM_Memset(pNewBuffer,0x00,unNewBufSize);
	VK_MEM_Memset(pNewSwapBuffer,0x00,unNewBufSize);
	if(pUrl->unBufSize > 0)
	{
		if(pUrl->url != NULL)
		{
			VK_memcpy(pNewBuffer,pUrl->url,pUrl->unBufSize);
		}
		if(pUrl->swap != NULL)
		{
			VK_memcpy(pNewSwapBuffer,pUrl->swap,pUrl->unBufSize);
		}
	}
	if(pUrl->url != NULL)
	{
		VK_MEM_Free(pUrl->url);
	}
	if(pUrl->swap != NULL)
	{
		VK_MEM_Free(pUrl->swap);
	}
	PrintError("%s(%d) URL Buffer Resize %u -> %u\n",__FUNCTION__,__LINE__,pUrl->unBufSize,unNewBufSize);
	pUrl->url = pNewBuffer;
	pUrl->swap = pNewSwapBuffer;
	pUrl->unBufSize = unNewBufSize;
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Fragment Request URL 을 생성한다. <br>
 *
 * @param[in] hCtrl	:  SSTR CONTROLLER HANDLE
 * @param[in] ulStreamIndex :  Stream Index (Array Index)
 * @param[in] ulTrackId :  Track Index (Array Index)
 * @param[in] ulFragmentId	:  Fragment Index (Array Index)
 * @param[out] szBuf	:  생성된 Fragment Request Url 을 저장하기 위한 Buffer
 * @param[in] nBufSize	:  Buffer 크기
 *
 * @return  #  SSTR_ERR <br>
 */
static SSTR_ERR P_SSTR_CONTROLLER_BuildUrl(SSTR_CONTROLLER_t *pController, HUINT32 ulStreamIndex, HUINT32 ulTrackId, HUINT32 ulFragmentId, SSTR_URL_t *pUrl)
{
	HINT32 nIdx = 0;
	HINT32 nPos = 0;
	HINT32 nLen = 0;
	HINT32 nFieldLen = 0;
	HINT32 nUrlLen = 0;
	HINT8 *pUrlPattern = NULL;
	HINT8 *pTemp = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrNode_t *pTrackContentNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	sstrFragment_t *pFragment = NULL;
	sstrTrackContent_t *pTrackContent = NULL;
	HINT8 szValue[SSTR_CONST_VALUEBUFFER_LENGTH] = {0};
	PrintEnter;
	if(pController == NULL || pController->parser == NULL
		|| pController->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pMedia = __sstrMedia(pController->parser->media);

	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, ulStreamIndex);
	if(pStreamNode == NULL)
	{
		PrintError("%s(%d) Err> StreamIndex(%d) is not exist\n",__FUNCTION__,__LINE__,ulStreamIndex);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pStream = (sstrStream_t*)pStreamNode->pData;
	if(pStream == NULL)
	{
		PrintError("%s(%d) Err> StreamIndex(%u) is not exist\n",__FUNCTION__,__LINE__,ulStreamIndex);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if((pStream->pUrlPattern == NULL || pStream->nUrlLen <= 0))
	{
		PrintError("%s(%d) Err> URL is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pUrlPattern = (HINT8*)pStream->pUrlPattern;
#if 0
	if(pStream->eType == SSTR_TYPE_VIDEO)
	{
		pTrack = pStream->trackSet + pController->nCurVideoTrack;
	}
	else if(pStream->eType == SSTR_TYPE_AUDIO)
	{
		pTrack = pStream->trackSet + pController->nCurAudioTrack;
	}
	else
	{
		PrintError("%s(%d) Err> Stream Type(%d) is not support\n",__FUNCTION__,__LINE__,pStream->eType);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
#else
	pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, ulTrackId);
	if(pTrackNode == NULL)
	{
		PrintError("%s(%d) Err> Track(%d) is NULL\n",__FUNCTION__,__LINE__,ulTrackId);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pTrack = (sstrTrack_t*)pTrackNode->pData;
	if(pTrack == NULL)
	{
		PrintError("%s(%d) Err> Track(%d) is NULL\n",__FUNCTION__,__LINE__,ulTrackId);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
#endif
	pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, ulFragmentId);
	if(pFragNode == NULL)
	{
		PrintError("%s(%d) Err> Fragment(%d) is NULL\n",__FUNCTION__,__LINE__,ulFragmentId);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pFragment = (sstrFragment_t*)pFragNode->pData;
	if(pFragment == NULL)
	{
		PrintError("%s(%d) Err> Fragment(%d) is NULL\n",__FUNCTION__,__LINE__,ulFragmentId);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pController->pBaseUrl == NULL)
	{
		PrintError("%s(%d) Err> Base URL(Manifest URL) is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pUrl == NULL)
	{
		PrintError("%s(%d) Err> Url Buffer is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pUrl->url == NULL || pUrl->swap == NULL)
	{
		if(P_SSTR_CONTROLLER_ResizeUrlBuffer(pUrl,SSTR_CONST_URL_LENGTH) != SSTR_ERR_OK)
		{
			PrintError("%s(%d) Err> Url Buffer Allocation\n",__FUNCTION__,__LINE__);
			PrintExit;
			return SSTR_ERR_FAIL;
		}
	}

	VK_MEM_Memset(pUrl->url,0,pUrl->unBufSize);
	VK_MEM_Memset(pUrl->swap,0,pUrl->unBufSize);

	pTemp = (HINT8*)VK_strstr((char*)pController->pBaseUrl,SSTR_TEMPLATE_UPPERMANIFEST);
	if(pTemp == NULL)
	{
		pTemp = (HINT8*)VK_strstr((char*)pController->pBaseUrl,SSTR_TEMPLATE_MANIFEST);
		if(pTemp == NULL)
		{
			PrintError("%s(%d) Err> Base URL(Manifest URL : %s) is INVALID\n",__FUNCTION__,__LINE__,pController->pBaseUrl);
			PrintExit;
			return SSTR_ERR_FAIL;
		}
	}

	/* Base Url */
	nLen = pTemp - pController->pBaseUrl;
	if( nPos + nLen >= (HINT32)pUrl->unBufSize)
	{
		if(P_SSTR_CONTROLLER_ResizeUrlBuffer(pUrl, nPos + nLen) != SSTR_ERR_OK)
		{
			PrintError("%s(%d) Err> Buffer Overflow\n",__FUNCTION__,__LINE__);
			PrintExit;
			return SSTR_ERR_FAIL;
		}
	}
	VK_MEM_Memcpy(pUrl->url + nPos, pController->pBaseUrl, nLen);
	nPos += nLen;

	nLen = VK_strlen(pUrlPattern);
	if( nPos + nLen >= (HINT32)pUrl->unBufSize)
	{
		if(P_SSTR_CONTROLLER_ResizeUrlBuffer(pUrl, nPos + nLen) != SSTR_ERR_OK)
		{
			PrintError("%s(%d) Err> Buffer Overflow\n",__FUNCTION__,__LINE__);
			PrintExit;
			return SSTR_ERR_FAIL;
		}
	}
	VK_MEM_Memcpy(pUrl->url + nPos , pUrlPattern, nLen);
	nUrlLen = nPos + nLen;

	/* Bitrate */
	pTemp = (HINT8*)VK_strstr((char*)pUrl->url,SSTR_TEMPLATE_BITRATE);
	if(pTemp == NULL)
	{
		pTemp = (HINT8*)VK_strstr((char*)pUrl->url,SSTR_TEMPLATE_UPPERBITRATE);
		if(pTemp == NULL)
		{
			PrintError("%s(%d) Err> SSTR_TEMPLATE_BITRATE is NULL\n",__FUNCTION__,__LINE__);
//			PrintExit;
//			return SSTR_ERR_FAIL;
		}
	}
	if(pTemp != NULL)
	{
		VK_MEM_Memset(pUrl->swap,0x00,pUrl->unBufSize);
		VK_MEM_Memset(szValue,0x00,SSTR_CONST_VALUEBUFFER_LENGTH);
		uitoa(pTrack->ulBitrate,(char*)szValue,10);
		nLen = VK_strlen(szValue);
		nFieldLen = VK_strlen(SSTR_TEMPLATE_BITRATE);
	//	if(nLen != nFieldLen)
		{
			nPos = pTemp - pUrl->url;
			if( ((nUrlLen + nLen) - nFieldLen ) >= (HINT32)pUrl->unBufSize)
			{
				if(P_SSTR_CONTROLLER_ResizeUrlBuffer(pUrl, ((nUrlLen + nLen) - nFieldLen)) != SSTR_ERR_OK)
				{
					PrintError("%s(%d) Err> Buffer Overflow\n",__FUNCTION__,__LINE__);
					PrintExit;
					return SSTR_ERR_FAIL;
				}
			}
	//		VK_MEM_Memcpy(szBuf + nPos + nLen, pTemp + nFieldLen,VK_strlen(pTemp) - nFieldLen);
			if( VK_strlen(pTemp) - nFieldLen >= pUrl->unBufSize )
			{
				if(P_SSTR_CONTROLLER_ResizeUrlBuffer(pUrl, VK_strlen(pTemp) - nFieldLen) != SSTR_ERR_OK)
				{
					PrintError("%s(%d) Err> Buffer Overflow\n",__FUNCTION__,__LINE__);
					PrintExit;
					return SSTR_ERR_FAIL;
				}
			}
			VK_MEM_Memcpy(pUrl->swap, pTemp + nFieldLen,VK_strlen(pTemp) - nFieldLen);
		}
		VK_MEM_Memcpy(pUrl->url + nPos, szValue, nLen);
		VK_MEM_Memcpy(pUrl->url + nPos + nLen, pUrl->swap,VK_strlen(pTemp) - nFieldLen);

		nUrlLen += (nLen - nFieldLen);
	}
	/* Start Time */
	pTemp = (HINT8*)VK_strstr((char*)pUrl->url,SSTR_TEMPLATE_STARTTIME);
	if(pTemp == NULL)
	{
		pTemp = (HINT8*)VK_strstr((char*)pUrl->url,SSTR_TEMPLATE_START_TIME);
		if(pTemp == NULL)
		{
			PrintError("%s(%d) Err> SSTR_TEMPLATE_BITRATE is NULL\n",__FUNCTION__,__LINE__);
//			PrintExit;
//			return SSTR_ERR_FAIL;
		}
		else
		{
			nFieldLen = VK_strlen(SSTR_TEMPLATE_START_TIME);
		}
	}
	else
	{
		nFieldLen = VK_strlen(SSTR_TEMPLATE_STARTTIME);
	}
	if(pTemp != NULL)
	{
		VK_MEM_Memset(pUrl->swap,0x00,pUrl->unBufSize);
		VK_MEM_Memset(szValue,0x00,SSTR_CONST_VALUEBUFFER_LENGTH);
		uitoa64(pFragment->ullStartTime,(char*)szValue,10);
		nLen = VK_strlen(szValue);
		if(nLen <= 0)
		{
			PrintError("%s(%d) Err> Invalid Start Time : %"PRId64"\n",__FUNCTION__,__LINE__,pFragment->ullStartTime);
			PrintExit;
			return SSTR_ERR_FAIL;
		}

		{
			nPos = pTemp - pUrl->url;
			if( ((nUrlLen + nLen) - nFieldLen) >= (HINT32)pUrl->unBufSize)
			{
				if(P_SSTR_CONTROLLER_ResizeUrlBuffer(pUrl, ((nUrlLen + nLen) - nFieldLen)) != SSTR_ERR_OK)
				{
					PrintError("%s(%d) Err> Buffer Overflow\n",__FUNCTION__,__LINE__);
					PrintExit;
					return SSTR_ERR_FAIL;
				}
			}
			if( VK_strlen(pTemp) - nFieldLen >=  pUrl->unBufSize )
			{
				if(P_SSTR_CONTROLLER_ResizeUrlBuffer(pUrl, VK_strlen(pTemp) - nFieldLen) != SSTR_ERR_OK)
				{
					PrintError("%s(%d) Err> Buffer Overflow\n",__FUNCTION__,__LINE__);
					PrintExit;
					return SSTR_ERR_FAIL;
				}
				PrintError("%s(%d) Err> Buffer Overflow : Url is too long \n",__FUNCTION__,__LINE__);
				PrintExit;
				return SSTR_ERR_FAIL;
			}
			VK_MEM_Memcpy(pUrl->swap, pTemp + nFieldLen,VK_strlen(pTemp) - nFieldLen);
		}
		VK_MEM_Memcpy(pUrl->url + nPos, szValue, nLen);
		VK_MEM_Memcpy(pUrl->url + nPos + nLen, pUrl->swap,VK_strlen(pTemp) - nFieldLen);
		nUrlLen += (nLen - nFieldLen);
	}

	/* Custom Attribute */
	pTemp = (HINT8*)VK_strstr((char*)pUrl->url,SSTR_TEMPLATE_CUSTOMATTR);
	if(pTemp != NULL)
	{
		nFieldLen = VK_strlen(SSTR_TEMPLATE_CUSTOMATTR);
		nPos = pTemp - pUrl->url;
		VK_MEM_Memset(pUrl->swap,0x00,pUrl->unBufSize);
		VK_MEM_Memcpy(pUrl->swap, pTemp + nFieldLen,VK_strlen(pTemp) - nFieldLen);
		VK_MEM_Memcpy(pUrl->url + nPos, pUrl->swap,VK_strlen(pTemp) - nFieldLen);
		nUrlLen -= nFieldLen;
		pUrl->url[nUrlLen] = '\0';
		pTrackContentNode = UTIL_DLLIST_GetHeadNode(&pTrack->trackContentSet);
		while(pTrackContentNode != NULL)
		{
			pTrackContent = (sstrTrackContent_t*)pTrackContentNode->pData;
			if(pTrackContent != NULL)
			{
				HINT32 nNameLen = 0;
				HINT32 nValueLen = 0;
				VK_MEM_Memset(pUrl->swap,0x00,pUrl->unBufSize);
				nNameLen = pTrackContent->nNameLen;
				nValueLen = pTrackContent->nValueLen;
				if( nUrlLen + nNameLen + nValueLen + 2 >= (HINT32)pUrl->unBufSize) /* '+1' is '=' */
				{
					if(P_SSTR_CONTROLLER_ResizeUrlBuffer(pUrl, nUrlLen + nNameLen + nValueLen + 2) != SSTR_ERR_OK)
					{
						PrintError("%s(%d) Err> Buffer Overflow\n",__FUNCTION__,__LINE__);
						PrintExit;
						return SSTR_ERR_FAIL;
					}
				}
				if( nUrlLen - nPos >=  (HINT32)pUrl->unBufSize )
				{
					if(P_SSTR_CONTROLLER_ResizeUrlBuffer(pUrl, nUrlLen - nPos) != SSTR_ERR_OK)
					{
						PrintError("%s(%d) Err> Buffer Overflow\n",__FUNCTION__,__LINE__);
						PrintExit;
						return SSTR_ERR_FAIL;
					}
				}
				VK_MEM_Memcpy(pUrl->swap,pUrl->url + nPos,nUrlLen - nPos);
				VK_MEM_Memcpy(pUrl->url + nPos,pTrackContent->pName, nNameLen);
				nPos+=nNameLen;
				pUrl->url[nPos++] = '=';
				VK_MEM_Memcpy(pUrl->url + nPos,pTrackContent->pValue, nValueLen);
				nPos+=nValueLen;
				if((HUINT32)(nIdx+1) < UTIL_DLLIST_Size(&pTrack->trackContentSet))
				{
					pUrl->url[nPos++] = ',';
					nUrlLen += 1;
				}
				nUrlLen += (nNameLen + nValueLen + 1);
				VK_MEM_Memcpy(pUrl->url + nPos,pUrl->swap, VK_strlen(pUrl->swap));
			}
			pTrackContentNode = UTIL_DLLIST_NextNode(pTrackContentNode);
			nIdx++;
		}
	}
	pUrl->url[nUrlLen] = '\0';
//	PrintDebug ("Url Build : %s\n",szBuf);
	PrintExit;
	return SSTR_ERR_OK;
}
#if 0
static SSTR_ERR P_SSTR_CONTROLLER_CreateInfo(SSTR_CONTROLLER_t *pController)
{
	sstrMedia_t *pMedia = NULL;
	SSTR_MANIFEST_Info_t *pManifest = NULL;
	PrintEnter;
	if(pController == NULL || pController->parser == NULL
		|| pController->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pMedia = __sstrMedia(pController->parser->media);
	pController->pManifest = (SSTR_MANIFEST_Info_t*)VK_MEM_Alloc(sizeof(SSTR_MANIFEST_Info_t));
	if( pController->pManifest == NULL )
	{
		PrintError("%s(%d) Err> Manifest Info Memory Allocation Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pManifest = pController->pManifest;
	pManifest->nStreamCnt = pMedia->nStreamCnt;
	if(pManifest->nStreamCnt > 0)
	{
		pManifest->pStream = (SSTR_STREAM_Info_t*)VK_MEM_Alloc(sizeof(SSTR_STREAM_Info_t)*pManifest->nStreamCnt);
		if( pManifest->pStream == NULL )
		{
			PrintError("%s(%d) Err> Manifest Info Memory Allocation Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return SSTR_ERR_FAIL;
		}
		for(HINT32 i = 0 ; i < pManifest->nStreamCnt ; i++)
		{
			(pManifest->pStream + i)->eType = (pMedia->streamSet + i)->eType;
			(pManifest->pStream + i)->ulIndex = (pMedia->streamSet + i)->ulIndex;
			(pManifest->pStream + i)->nNameLen = (pMedia->streamSet + i)->nNameLen;
			(pManifest->pStream + i)->pName = (HINT8*)(pMedia->streamSet + i)->pName;
			(pManifest->pStream + i)->nLangLen = (pMedia->streamSet + i)->nLangLen;
			(pManifest->pStream + i)->pLanguage = (HINT8*)(pMedia->streamSet + i)->pLanguage;
			(pManifest->pStream + i)->nTrackCnt = (pMedia->streamSet + i)->ulQualityLevels;
			if((pManifest->pStream + i)->nTrackCnt > 0)
			{
				(pManifest->pStream + i)->pTrack = (SSTR_TRACK_Info_t*)VK_MEM_Alloc(sizeof(SSTR_TRACK_Info_t) * (pMedia->streamSet + i)->ulQualityLevels);
				if( (pManifest->pStream + i)->pTrack == NULL )
				{
					PrintError("%s(%d) Err> Manifest Info Memory Allocation Fail\n",__FUNCTION__,__LINE__);
					PrintExit;
					return SSTR_ERR_FAIL;
				}
				for(HINT32 j = 0 ; j < (pManifest->pStream + i)->nTrackCnt ; j++)
				{
					((pManifest->pStream + i)->pTrack+j)->ulIndex = ((pMedia->streamSet + i)->trackSet + j)->ulIndex;
					((pManifest->pStream + i)->pTrack+j)->ulBitrate = ((pMedia->streamSet + i)->trackSet + j)->ulBitrate;
					((pManifest->pStream + i)->pTrack+j)->pFourCC = (HINT8*)&(((pMedia->streamSet + i)->trackSet + j)->FourCC);
					((pManifest->pStream + i)->pTrack+j)->nCustomAttrCnt = ((pMedia->streamSet + i)->trackSet + j)->ulTrackContents;
					if(((pManifest->pStream + i)->pTrack+j)->nCustomAttrCnt > 0)
					{
						((pManifest->pStream + i)->pTrack+j)->pCustomAttr = (SSTR_CUSTOMATTR_Info_t*)VK_MEM_Alloc(sizeof(SSTR_CUSTOMATTR_Info_t) * ((pMedia->streamSet + i)->trackSet + j)->ulTrackContents);
						if( ((pManifest->pStream + i)->pTrack+j)->pCustomAttr == NULL )
						{
							PrintError("%s(%d) Err> Manifest Info Memory Allocation Fail\n",__FUNCTION__,__LINE__);
							PrintExit;
							return SSTR_ERR_FAIL;
						}
						for(HINT32 k = 0 ; k < ((pManifest->pStream + i)->pTrack+j)->nCustomAttrCnt ; k++)
						{
							(((pManifest->pStream + i)->pTrack+j)->pCustomAttr+k)->nNameLen = (((pMedia->streamSet + i)->trackSet + j)->trackContentSet+k)->nNameLen;
							(((pManifest->pStream + i)->pTrack+j)->pCustomAttr+k)->pName = (HINT8*)(((pMedia->streamSet + i)->trackSet + j)->trackContentSet+k)->pName;
							(((pManifest->pStream + i)->pTrack+j)->pCustomAttr+k)->nValueLen = (((pMedia->streamSet + i)->trackSet + j)->trackContentSet+k)->nValueLen;
							(((pManifest->pStream + i)->pTrack+j)->pCustomAttr+k)->pValue = (HINT8*)(((pMedia->streamSet + i)->trackSet + j)->trackContentSet+k)->pValue;
						}
					}
					else
					{
						((pManifest->pStream + i)->pTrack+j)->pCustomAttr = NULL;
					}
				}
			}
			else
			{
				(pManifest->pStream + i)->pTrack = NULL;
			}
		}
	}
	else
	{
		pManifest->pStream = NULL;
	}
	PrintExit;
	return SSTR_ERR_OK;
}

static SSTR_ERR P_SSTR_CONTROLLER_DestroyInfo(SSTR_CONTROLLER_t *pController)
{
	SSTR_MANIFEST_Info_t *pManifest = NULL;
	PrintEnter;
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
//	pController->pManifest = (SSTR_MANIFEST_Info_t*)VK_MEM_Alloc(sizeof(SSTR_MANIFEST_Info_t));
	pManifest = pController->pManifest;
	for(HINT32 i = 0 ; i < pManifest->nStreamCnt ; i++)
	{
		for(HINT32 j = 0 ; j < (pManifest->pStream + i)->nTrackCnt ; j++)
		{
			if(((pManifest->pStream + i)->pTrack+j)->pCustomAttr != NULL)
			{
				VK_MEM_Free(((pManifest->pStream + i)->pTrack+j)->pCustomAttr);
			}
		}
		if((pManifest->pStream + i)->pTrack != NULL)
		{
			VK_MEM_Free((pManifest->pStream + i)->pTrack);
		}

	}
	if(pManifest->pStream != NULL)
	{
		VK_MEM_Free(pManifest->pStream);
	}

	PrintExit;
	return SSTR_ERR_OK;
}
#endif



/**
 * @b Function @b Description <br>
 * SSTR Controller 를 Memory 상 Manifest를 통해 Open 한다. <br>
 *
 * @param[in] szBuf : Manifest Data
 * @param[in] nBufSize : Manifest 크기
 * @param[in] pGC : Garbage Collector (Optional)
 *
 * @return	# SSTR CONTROLLER HANDLE : 성공 <br>
 * 			# NULL : 실패 <br>
 */
SSTR_CTRL	P_SSTR_CONTROLLER_MemOpen (HINT8 *szBuf, HINT64 nBufSize, sstrParserGC_t *pGC)
{
	sstrErr_e err = 0;
	sstrParser_t* parser = NULL;
	SSTR_CONTROLLER_t *controller = NULL;
	// ABR_HANDLE hMpd = NULL;
	PrintEnter;
	parser = (sstrParser_t*)SSTR_ManifestParser_MemOpen((char*)szBuf,nBufSize);
	if(parser == NULL)
	{
		PrintError("%s(%d) Err> Parser is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	err = SSTR_ManifestParser_Parse(parser,pGC);
	if(err != SSTR_ERR_OK)
	{
		PrintError("%s(%d) Err> Parse Fail (error code : %d)\n",__FUNCTION__,__LINE__,err);
		SSTR_ManifestParser_Close(parser,pGC);
		PrintExit;
		return NULL;
	}
	controller = (SSTR_CONTROLLER_t*)VK_MEM_Alloc(sizeof(SSTR_CONTROLLER_t));
	if(controller == NULL)
	{
		PrintError("%s(%d) Err> controller Memory Alloc Fail\n",__FUNCTION__,__LINE__);
		SSTR_ManifestParser_Close(parser,pGC);
		PrintExit;
		return NULL;
	}
	VK_MEM_Memset(controller,0x00,sizeof(SSTR_CONTROLLER_t));
	controller->hBufferMgr = NULL;
	controller->parser = parser;
	PrintExit;
	return (SSTR_CTRL)controller;
}

/**
 * @b Function @b Description <br>
 * SSTR Controller 를 Manifest File을 통해 Open 한다. <br>
 *
 * @param[in] szPath : Manifest File Path
 * @param[in] pGC : Garbage Collector (Optional)
 *
 * @return	# SSTR CONTROLLER HANDLE : 성공 <br>
 * 			# NULL : 실패 <br>
 */
SSTR_CTRL	P_SSTR_CONTROLLER_FileOpen (HINT8 *szPath, sstrParserGC_t *pGC)
{
	sstrErr_e err = 0;
	sstrParser_t* parser = NULL;
	SSTR_CONTROLLER_t *controller = NULL;
	// ABR_HANDLE hMpd = NULL;
	PrintEnter;
	controller = (SSTR_CONTROLLER_t*)VK_MEM_Alloc(sizeof(SSTR_CONTROLLER_t));
	if(controller == NULL)
	{
		PrintError("%s(%d) Err> Controller Memory Alloc Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	VK_MEM_Memset(controller,0x00,sizeof(SSTR_CONTROLLER_t));
	parser = (sstrParser_t*)SSTR_ManifestParser_FileOpen((char*)szPath);
	if(parser == NULL)
	{
		PrintError("%s(%d) Err> Parser is NULL\n",__FUNCTION__,__LINE__);
		if(controller != NULL)
		{
			DD_MEM_Free(controller);
		}
		PrintExit;
		return NULL;
	}
	err = SSTR_ManifestParser_Parse(parser,pGC);
	if(err != SSTR_ERR_OK)
	{
		PrintError("%s(%d) Err> Parse Fail (error code : %d)\n",__FUNCTION__,__LINE__,err);
		SSTR_ManifestParser_Close(parser,pGC);
		if(controller != NULL)
		{
			DD_MEM_Free(controller);
		}
		PrintExit;
		return NULL;
	}
	controller->hBufferMgr = NULL;
	controller->parser = parser;
	PrintExit;
	return (SSTR_CTRL)controller;
}

/*----------------------------------------------------------------------------------------------
 *	Task
 *---------------------------------------------------------------------------------------------*/

#if __________task__________
#endif

#if __________di__________
#endif

/*----------------------------------------------------------------------------------------------
 *	External API
 *---------------------------------------------------------------------------------------------*/

SSTR_CTRL 	SSTR_CONTROLLER_Open (HINT8	*pManifest, HINT64 nContentLength)
{
	HINT32 nRet;
	HINT8 tmpString[256]; /* for naming each tasks */
	SSTR_CTRL hCtrl = NULL;
	SSTR_CONTROLLER_t *pController = NULL;
	PrintEnter;
	/*******************************************
	 * Check param  & tuner state
	 *******************************************/
	/*if ( NULL == pHTuner->pUrl || pstMediaInfo == NULL)
	{
		PrintError ("%s (%d) Error>  pHTuner->pUrl or pstMediaInfo is NULL !\n",__FUNCTION__,__LINE__);
		nRet = ABR_INVALID_PARAM;
		return NULL;
	}*/

//	PrintData ("%s (%d)  pHTuner->pUrl=%s, pHTuner=%p VFIO Handle : %p pTuner in VFIO : %p\n",__FUNCTION__,__LINE__, pHTuner->pUrl, pHTuner,pVfio,pVfio->pTunerHandle);

#if SSTR_CONFIG_MEMORY_PARSE
	hCtrl = P_SSTR_CONTROLLER_MemOpen(pManifest, nContentLength+SSTR_CONST_PADDING,NULL);
#else
	hCtrl = P_SSTR_CONTROLLER_FileOpen(SSTR_CONFIG_TEMP_CACHE,NULL);
#endif

	if( hCtrl == NULL )
	{
		PrintError("%s(%d) Error> SSTR Manifest Open Fail\n",__FUNCTION__,__LINE__);
		// nRet = ABR_FAIL;
		return NULL;
	}
#if 0
	if(P_SSTR_CONTROLLER_QueueOpen(hCtrl))
	{
		PrintError("%s(%d) Error> SSTR Queue Open Fail\n",__FUNCTION__,__LINE__);
		// nRet = ABR_FAIL;
		return NULL;
	}
#endif
	pController = __controller(hCtrl);
	VK_memset (tmpString, 0, sizeof(tmpString));
	VK_snprintf ((char*)tmpString,VK_strlen("SSTR_CONTROLLER_SEMA_")+10,"SSTR_CONTROLLER_SEMA_%p",hCtrl);
	nRet = VK_SEM_Create(&pController->ulSema,(char*)tmpString, VK_SUSPENDTYPE_PRIORITY);
	if ( VK_OK != nRet )
	{
		PrintError ("%s(%d) Error> SSTR Controller Sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nRet );
		// nRet = ABR_FAIL;
		SSTR_CONTROLLER_Close(hCtrl);
		return NULL;
	}
	pController->hHeuristics = (SSTR_HEURISTICS_t*)VK_MEM_Alloc(sizeof(SSTR_HEURISTICS_t));
	if(pController->hHeuristics == NULL)
	{
		SSTR_CONTROLLER_Close(hCtrl);
		return NULL;
	}
	VK_MEM_Memset(pController->hHeuristics,0x00,sizeof(SSTR_HEURISTICS_t));
	pController->hHeuristics->eState = SSTR_HEUR_STATE_TIMID;
//	pController->hHeuristics->nCursor = 0;
//	pController->hHeuristics->nSize = 0;
//	pController->hHeuristics->ullAvgBandwidth = 0;
//	pController->hHeuristics->ullPairBandwidth = 0;
//	(void)P_SSTR_CONTROLLER_CreateInfo(pController);
	PrintExit;
	return hCtrl;
}

SSTR_ERR SSTR_CONTROLLER_Update(SSTR_CTRL hDstCtrl, HINT8 *pManifest, HINT64 nContentLength)
{
	SSTR_CTRL hSrcCtrl = NULL;
	SSTR_CONTROLLER_t *pSrcController = NULL;
	SSTR_CONTROLLER_t *pDstController = NULL;
	PrintEnter;
	if(hDstCtrl == NULL || ((SSTR_CONTROLLER_t*)hDstCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hDstCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pDstController = __controller(hDstCtrl);
#if SSTR_CONFIG_MEMORY_PARSE
	hSrcCtrl = P_SSTR_CONTROLLER_MemOpen(pManifest, nContentLength+SSTR_CONST_PADDING,&pDstController->parser->gc);
#else
	hSrcCtrl = P_SSTR_CONTROLLER_FileOpen(SSTR_CONFIG_TEMP_CACHE,&pDstController->parser->gc);
#endif
	if(hSrcCtrl == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pSrcController = __controller(hSrcCtrl);
	if(SSTR_ManifestParser_Merge(pDstController->parser, pSrcController->parser) != SSTR_ERR_OK)
	{
		PrintError("%s(%d) Err> Merge Fail\n",__FUNCTION__,__LINE__);
	}
	if(pSrcController->pBaseUrl != NULL)
	{
		VK_MEM_Free(pSrcController->pBaseUrl);
	}
#if defined(SSTR_MANIFESTPARSER_CONFIG_GC_SUPPORT)
	SSTR_ManifestParser_Close(pSrcController->parser,&pDstController->parser->gc);
#else
	SSTR_ManifestParser_Close(pSrcController->parser,NULL);
#endif
	VK_MEM_Free(pSrcController);
	PrintExit;
	return SSTR_ERR_OK;
}

SSTR_ERR SSTR_CONTROLLER_AddFragment(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullStartTime,HUINT64 ullDuration)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrFragment_t *pFrag = NULL;
	sstrFragment_t *pNewFrag = NULL;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		pFragNode = UTIL_DLLIST_GetTailNode(&pStream->fragmentSet);
		if(pFragNode != NULL)
		{
			pFrag = (sstrFragment_t*) pFragNode->pData;
			if(pFrag->ullStartTime < ullStartTime)
			{
				pNewFrag = DD_MEM_Alloc(sizeof(sstrFragment_t));
				if(pNewFrag != NULL)
				{
					VK_MEM_Memset(pNewFrag,0x00,sizeof(sstrFragment_t));
					pNewFrag->ullStartTime = pNewFrag->ullTime = ullStartTime;
					pNewFrag->ullDuration = ullDuration;
					if(UTIL_DLLIST_Append(&pStream->fragmentSet, pNewFrag) == DLL_ERR_OK)
					{
						P_SSTR_CONTROLLER_Unlock(hCtrl);
						PrintExit;
						return SSTR_ERR_OK;
					}
					else
					{
						DD_MEM_Free(pNewFrag);
					}
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

SSTR_ERR SSTR_CONTROLLER_DiscardFragments(SSTR_CTRL hCtrl, HUINT32 unTargetStream, HUINT32 unFragment)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pExtractedNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	HUINT32 unDiscardedNode = 0;
#if !defined(SSTR_MANIFESTPARSER_CONFIG_GC_SUPPORT)
	sstrFragment_t *pFragment = NULL;
#endif
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,unTargetStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pExtractedNode = UTIL_DLLIST_Extract_toStart(&pStream->fragmentSet, unFragment);
			pFragNode = pExtractedNode;
#if defined(SSTR_MANIFESTPARSER_CONFIG_GC_SUPPORT)
			if(pExtractedNode != NULL)
			{
				if(UTIL_DLLIST_AppendNode(&pController->parser->gc.fragmentGC, pExtractedNode) == DLL_ERR_OK)
				{
					pStream->ulFragmentSeqNum -= (unFragment+1);
				}
			}
#else
			while(pFragNode != NULL)
			{
				unDiscardedNode++;
				pFragment = (sstrFragment_t*) pFragNode->pData;
				if(pFragment != NULL)
				{
					if(pFragment->pulIndex != NULL)
					{
						DD_MEM_Free(pFragment->pulIndex);
					}
					if(pFragment->pExtensionTrackData != NULL)
					{
						DD_MEM_Free(pFragment->pExtensionTrackData);
					}
					DD_MEM_Free(pFragNode->pData);
				}
				pFragNode->pData = NULL;
				pFragNode = UTIL_DLLIST_NextNode(pFragNode);
			}
			UTIL_DLLIST_DeleteNodes(pExtractedNode);
			pStream->ulFragmentSeqNum -= (unDiscardedNode);
#endif

		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}

#if 0
SSTR_ERR	SSTR_CONTROLLER_GetMediaInfo (SSTR_CTRL hCtrl, DI_MEDIA_INFO_t *pstMediaInfo)
{
	HINT32 nVideoIndex = 0;
	HINT32 nAudioIndex = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	HUINT16 usMaxWidth = 0;
	HUINT16 usMaxHeight = 0;
	PrintEnter;
	//P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}

	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pstMediaInfo->max_bitrate = SSTR_CONTROLLER_GetMaxBitrate(hCtrl);
	pstMediaInfo->duration	= SSTR_CONTROLLER_GetMediaDuration(hCtrl);
	pstMediaInfo->nprograms	= 1;  /* Check */
	pstMediaInfo->ntracks = pMedia->nStreamCnt;
//	pTunerHandle->tMediaInfo.ntracks	= pMedia->nStreamCnt; /* SSTR_CONTROLLER_GetTotalTrack(hCtrl);*/
	pstMediaInfo->stream_type = DI_MEDIA_MPEG_TYPE_MP4FRAGMENT;
	pstMediaInfo->pcr_pid	= 0x1fff; /* NULL_PID;*/
	for(int i = 0; i < pMedia->nStreamCnt ; i++)
	{
		if((pMedia->streamSet + i)->eType == SSTR_TYPE_VIDEO)
		{
			if(usMaxWidth < (pMedia->streamSet + i)->ulMaxWidth)
			{
				usMaxWidth = (pMedia->streamSet + i)->ulMaxWidth;
			}
			if(usMaxHeight < (pMedia->streamSet + i)->ulMaxHeight)
			{
				usMaxHeight = (pMedia->streamSet + i)->ulMaxHeight;
			}
			if(nVideoIndex < DI_MEDIA_DIVX_MAX_VIDEO_TRACK )
			{
				pstMediaInfo->stVideoTrack[nVideoIndex].video_stream_id = SSTR_CONST_VIDEO_STREAM_ID;
				pstMediaInfo->stVideoTrack[nVideoIndex].video_pes_id = DI_MEDIA_VIDEO_PES_ID;
				pstMediaInfo->stVideoTrack[nVideoIndex].video_codec = P_SSTR_CONTROLLER_FourCCToDICodec(((pMedia->streamSet + i)->trackSet)->FourCC);
				nVideoIndex++;
			}
/*
			for(int j = 0; j < (pMedia->streamSet + i)->ulQualityLevels || ; j++)
			{
				pstMediaInfo->stVideoTrack[nVideoIndex].video_stream_id = SSTR_CONST_VIDEO_STREAM_ID;
				pstMediaInfo->stVideoTrack[nVideoIndex].video_pes_id = DI_MEDIA_VIDEO_PES_ID;
				pstMediaInfo->stVideoTrack[nVideoIndex].video_codec = P_SSTR_CONTROLLER_FourCCToDICodec(((pMedia->streamSet + i)->trackSet + j)->FourCC);
				nVideoIndex++;
			}
*/
		}
		else if((pMedia->streamSet + i)->eType == SSTR_TYPE_AUDIO)
		{
			if(nAudioIndex < DI_MEDIA_DIVX_MAX_AUDIO_TRACK )
			{
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_stream_id = SSTR_CONST_AUDIO_STREAM_ID;
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_pes_id = DI_MEDIA_AUDIO_PES_ID;
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec = P_SSTR_CONTROLLER_FourCCToDICodec(((pMedia->streamSet + i)->trackSet)->FourCC);
				nAudioIndex++;
			}
/*
			for(int j = 0; j < (pMedia->streamSet + i)->ulQualityLevels ; j++)
			{
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_stream_id = SSTR_CONST_AUDIO_STREAM_ID;
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_pes_id = DI_MEDIA_AUDIO_PES_ID;
				pstMediaInfo->stAudioTrack[nAudioIndex].audio_codec = P_SSTR_CONTROLLER_FourCCToDICodec(((pMedia->streamSet + i)->trackSet + j)->FourCC);
				nAudioIndex++;
			}
*/
		}
	}
	pstMediaInfo->usVideoWidth = usMaxWidth;
	pstMediaInfo->usVideoHeight = usMaxHeight;
	pstMediaInfo->audio_codec = pstMediaInfo->stAudioTrack[0].audio_codec;
	pstMediaInfo->video_codec = pstMediaInfo->stVideoTrack[0].video_codec;
	pstMediaInfo->audiodecodertype = DI_MEDIA_AUDIO_DECODER_MAIN;
	pstMediaInfo->videodecodertype = DI_MEDIA_VIDEO_DECODER_MAIN;
	pstMediaInfo->audio_stream_id = pstMediaInfo->stAudioTrack[0].audio_stream_id;
	pstMediaInfo->video_stream_id = pstMediaInfo->stVideoTrack[0].video_stream_id;
	pstMediaInfo->audio_pes_id = pstMediaInfo->stAudioTrack[0].audio_pes_id;
	pstMediaInfo->video_pes_id = pstMediaInfo->stVideoTrack[0].video_pes_id;
	pstMediaInfo->ucTotalAudioTrack = nAudioIndex;
	pstMediaInfo->ucTotalVideoTrack = nVideoIndex;
	pstMediaInfo->ntracks = nVideoIndex + nAudioIndex;
	pstMediaInfo->support_seek = 1;
	pstMediaInfo->support_trick = 1;
	PrintExit;
	return SSTR_ERR_OK;
}

#endif

SSTR_MANIFEST_Info_t*	SSTR_CONTROLLER_GetManifestInfo (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}

	pController = __controller(hCtrl);
	PrintExit;
	return pController->pManifest;
}

SSTR_ERR	SSTR_CONTROLLER_Close (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t *pController = NULL;
	PrintEnter;
	// ABR_HANDLE hMpd = NULL;
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		return SSTR_ERR_FAIL;
	}
#if 0 /* DO NOT USE TASK by ykkim */
	pMpd->bStopABRTask = TRUE;
	while(pMpd->bABRTaskQuit != TRUE)
	{
		usleep(100);
	}
#endif
//	P_SSTR_CONTROLLER_QueueClose(pController);
//	SSTR_BUFFERMGR_Close(SSTR_CONTROLLER_GetBuffer(hCtrl));
	if(pController->hHeuristics != NULL)
	{
		VK_MEM_Free(pController->hHeuristics);
	}
	VK_SEM_Destroy(pController->ulSema);
#if 0 /* DO NOT USE TASK by ykkim */
	VK_TASK_Destroy(pMpd->ulTaskId);
#endif
	if(pController->pBaseUrl != NULL)
	{
		VK_MEM_Free(pController->pBaseUrl);
	}
	SSTR_ManifestParser_Close(((SSTR_CONTROLLER_t*)pController)->parser,NULL);
//	(void)P_SSTR_CONTROLLER_DestroyInfo(pController);
	VK_MEM_Free(pController);
	PrintExit;
	return SSTR_ERR_OK;
}
#if 0
/**
 * @brief       open buffer
 * @param       hCtrl : SSTR Handle, ullBufferSize : buffer size
 * @return      sstr error code
 * @date        2011/05/02
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
SSTR_ERR	SSTR_CONTROLLER_OpenBuffer (SSTR_CTRL hCtrl, HUINT64 ullBufferSize)
{
	SSTR_CONTROLLER_t *pController = NULL;
	SSTR_CONTROLLER_BUFFER_t *pBufferMgr = NULL;
	HINT8 tmpString[256]; /* for naming each tasks */
	HINT32 nRet = 0;
	PrintEnter;
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		return SSTR_ERR_FAIL;
	}
	pBufferMgr = &pController->BufferMgr;
	pBufferMgr->ullBufferSize = ullBufferSize;
	if(pBufferMgr->ullBufferSize <= 0)
	{
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pBufferMgr->pBuffer = (HINT8 *)DD_MEM_Alloc(ullBufferSize);
	pBufferMgr->ullReadPos = 0;
	pBufferMgr->ullWritePos = 0;
	pBufferMgr->bWriteLock = 0;
	VK_memset (tmpString, 0, sizeof(tmpString));
	VK_sprintf (tmpString,"SSTR_BUFFER_SEMA_%p",pBufferMgr);
	nRet = VK_SEM_Create(&pBufferMgr->ulSema,  tmpString, VK_SUSPENDTYPE_PRIORITY);
	if ( VK_OK != nRet )
	{
		PrintError ("%s(%d) Error> SSTR Controller Buffer Sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nRet );
		// nRet = ABR_FAIL;
		return SSTR_ERR_FAIL;
	}
	PrintExit;
	return SSTR_ERR_OK;
}

/**
 * @brief       close buffer
 * @param       hCtrl : SSTR Handle, ullBufferSize : buffer size
 * @return      sstr error code
 * @date        2011/05/02
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
SSTR_ERR	SSTR_CONTROLLER_CloseBuffer (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t *pController = NULL;
	SSTR_CONTROLLER_BUFFER_t *pBufferMgr = NULL;
	PrintEnter;
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		return SSTR_ERR_FAIL;
	}
	pBufferMgr = &pController->BufferMgr;
	if(pBufferMgr->pBuffer != NULL)
	{
			DD_MEM_Free(pBufferMgr->pBuffer);
	}
	pBufferMgr->pBuffer = NULL;
	pBufferMgr->ullBufferSize = 0;
	pBufferMgr->ullReadPos = 0;
	pBufferMgr->ullWritePos = 0;
	pBufferMgr->bWriteLock = 0;
	VK_SEM_Destroy(pBufferMgr->ulSema);
	PrintExit;
	return SSTR_ERR_OK;
}

/**
 * @brief       Write to buffer
 * @param       hCtrl : SSTR Handle, pData : data , ullSize : data size
 * @return      sstr error code
 * @date        2011/05/02
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HUINT64	SSTR_CONTROLLER_WriteBuffer (SSTR_CTRL hCtrl, HUINT8 *pData, HUINT64 ullSize)
{
	SSTR_CONTROLLER_t *pController = NULL;
	SSTR_CONTROLLER_BUFFER_t *pBufferMgr = NULL;
	HUINT64 ullSubSize = 0;
//	PrintEnter;
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pBufferMgr = &pController->BufferMgr;
	if(pBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(pBufferMgr->ullWritePos == pBufferMgr->ullReadPos && pBufferMgr->bWriteLock == TRUE)
	{
//		PrintError("%s(%d) Debug> didn't Write(close to ReadPos :%"PRId64") %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,pBufferMgr->ullReadPos,ullSize,pBufferMgr->ullWritePos);
//		VK_SEM_Release(pBufferMgr->ulSema);
		return 0;
	}
	VK_SEM_Get(pBufferMgr->ulSema);

	if(pBufferMgr->ullWritePos + ullSize > pBufferMgr->ullBufferSize)
	{
		ullSubSize = (pBufferMgr->ullWritePos + ullSize) - pBufferMgr->ullBufferSize;
		ullSize = pBufferMgr->ullBufferSize - pBufferMgr->ullWritePos;
	}
#if 1
	if( pBufferMgr->ullWritePos < pBufferMgr->ullReadPos )
	{
		if( (pBufferMgr->ullWritePos + ullSize) >= pBufferMgr->ullReadPos)
		{
			ullSize = pBufferMgr->ullReadPos - pBufferMgr->ullWritePos;
			pBufferMgr->bWriteLock = TRUE;
			VK_memcpy(pBufferMgr->pBuffer + pBufferMgr->ullWritePos,pData,ullSize);
	//		PrintError("%s(%d) Debug> didn't Write(close to ReadPos :%"PRId64") %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,pBufferMgr->ullReadPos,ullSize,pBufferMgr->ullWritePos);
			pBufferMgr->ullWritePos += ullSize;
			VK_SEM_Release(pBufferMgr->ulSema);
			return ullSize;
		}
	}
//	PrintError("%s(%d) Debug> Write %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,ullSize,pBufferMgr->ullWritePos);
	VK_memcpy(pBufferMgr->pBuffer + pBufferMgr->ullWritePos,pData,ullSize);
	if(ullSubSize > 0)
	{
		if( ullSubSize >= pBufferMgr->ullReadPos )
		{
			ullSubSize = pBufferMgr->ullReadPos;
			pBufferMgr->bWriteLock = TRUE;
		}
		// pBufferMgr->ullWritePos	= 0;
		PrintError("%s(%d) Debug> Write(Sub) %"PRId64"(%"PRId64") from %"PRId64"(%"PRId64") \n",__FUNCTION__,__LINE__,ullSize,ullSubSize,pBufferMgr->ullWritePos,pBufferMgr->ullReadPos);
		VK_memcpy(pBufferMgr->pBuffer,pData + ullSize,ullSubSize);
		pBufferMgr->ullWritePos = ullSubSize;
		ullSize += ullSubSize;
	}
	else
	{
		pBufferMgr->ullWritePos += ullSize;
	}
#else
	if( pBufferMgr->ullWritePos < pBufferMgr->ullReadPos )
	{
		if( (pBufferMgr->ullWritePos + ullSize) > pBufferMgr->ullReadPos)
		{
			ullSize = (pBufferMgr->ullWritePos) - pBufferMgr->ullReadPos;
		}
	}
	VK_memcpy(pBufferMgr->pBuffer + pBufferMgr->ullWritePos,pData,ullSize);

	pBufferMgr->ullWritePos += ullSize;
#endif
//	PrintExit;
	VK_SEM_Release(pBufferMgr->ulSema);
	return ullSize;
}

/**
 * @brief       Read from buffer
 * @param       hCtrl : SSTR Handle, pData : data , ullSize : data size
 * @return      sstr error code
 * @date        2011/05/02
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HUINT64	SSTR_CONTROLLER_ReadBuffer (SSTR_CTRL hCtrl, HUINT8 *pData, HUINT64 ullSize)
{
	SSTR_CONTROLLER_t *pController = NULL;
	SSTR_CONTROLLER_BUFFER_t *pBufferMgr = NULL;
	HUINT64 ullSubSize = 0;
//	PrintEnter;
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pBufferMgr = &pController->BufferMgr;
	if(pBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	VK_SEM_Get(pBufferMgr->ulSema);
	if(pBufferMgr->ullReadPos == pBufferMgr->ullWritePos && pBufferMgr->bWriteLock == FALSE)
	{
		PrintError("%s(%d) Debug> didn't Read(close to WritePos : %"PRId64") %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,pBufferMgr->ullWritePos,ullSize,pBufferMgr->ullReadPos);
		VK_SEM_Release(pBufferMgr->ulSema);
		return 0;
	}
	if(pBufferMgr->ullReadPos + ullSize > pBufferMgr->ullBufferSize)
	{
		ullSubSize = (pBufferMgr->ullReadPos + ullSize) - pBufferMgr->ullBufferSize;
		ullSize = pBufferMgr->ullBufferSize - pBufferMgr->ullReadPos;
	}

	if( pBufferMgr->ullReadPos < pBufferMgr->ullWritePos )
	{
		if( (pBufferMgr->ullReadPos + ullSize) >= pBufferMgr->ullWritePos)
		{
			ullSize = pBufferMgr->ullWritePos - pBufferMgr->ullReadPos;
			VK_memcpy(pData,pBufferMgr->pBuffer + pBufferMgr->ullReadPos,ullSize);
//			PrintError("%s(%d) Debug> didn't Read(close to WritePos : %"PRId64") %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,pBufferMgr->ullWritePos,ullSize,pBufferMgr->ullReadPos);
			pBufferMgr->ullReadPos += ullSize;
			if(pBufferMgr->bWriteLock == TRUE)
			{
				pBufferMgr->bWriteLock = FALSE;
			}
			VK_SEM_Release(pBufferMgr->ulSema);
			return ullSize;
		}
	}
	VK_memcpy(pData,pBufferMgr->pBuffer + pBufferMgr->ullReadPos,ullSize);
//	PrintError("%s(%d) Debug> Read  %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,ullSize,pBufferMgr->ullReadPos);
	if(ullSubSize > 0)
	{
		if( ullSubSize > pBufferMgr->ullWritePos )
		{
			ullSubSize = pBufferMgr->ullWritePos;
//			return SSTR_ERR_FAIL_BUFFER_SIZE;
		}
		// pBufferMgr->ullWritePos	= 0;
		VK_memcpy(pData + ullSize,pBufferMgr->pBuffer,ullSubSize);
		PrintError("%s(%d) Debug> Read(Sub) %"PRId64"(%"PRId64") from %"PRId64"(%"PRId64") \n",__FUNCTION__,__LINE__,ullSize,ullSubSize,pBufferMgr->ullReadPos,pBufferMgr->ullWritePos);
		pBufferMgr->ullReadPos = ullSubSize;
		ullSize += ullSubSize;
	}
	else
	{
		pBufferMgr->ullReadPos += ullSize;
	}
	PrintExit;
	if(pBufferMgr->bWriteLock == TRUE)
	{
		pBufferMgr->bWriteLock = FALSE;
	}
	VK_SEM_Release(pBufferMgr->ulSema);
	return ullSize;
}

/**
 * @brief
 * @param       hCtrl : SSTR Handle, pData : data , ullSize : data size
 * @return      sstr error code
 * @date        2011/05/02
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HUINT64	SSTR_CONTROLLER_GetWritePos (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t *pController = NULL;
	SSTR_CONTROLLER_BUFFER_t *pBufferMgr = NULL;
	HUINT64 ullSubSize = 0;
//	PrintEnter;
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pBufferMgr = &pController->BufferMgr;
	if(pBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	return pBufferMgr->ullWritePos;
}

/**
 * @brief       Read from buffer
 * @param       hCtrl : SSTR Handle, pData : data , ullSize : data size
 * @return      sstr error code
 * @date        2011/05/02
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HUINT64	SSTR_CONTROLLER_GetReadPos (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t *pController = NULL;
	SSTR_CONTROLLER_BUFFER_t *pBufferMgr = NULL;
	HUINT64 ullSubSize = 0;
//	PrintEnter;
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pBufferMgr = &pController->BufferMgr;
	if(pBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	return pBufferMgr->ullReadPos;
}

/**
 * @brief
 * @param       hCtrl : SSTR Handle, pData : data , ullSize : data size
 * @return      sstr error code
 * @date        2011/05/02
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HUINT64	SSTR_CONTROLLER_GetReadableSize (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t *pController = NULL;
	SSTR_CONTROLLER_BUFFER_t *pBufferMgr = NULL;
	HUINT64 ullSubSize = 0;
	HUINT64 ullSize = 0;
//	PrintEnter;
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pBufferMgr = &pController->BufferMgr;
	if(pBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(pBufferMgr->ullReadPos == pBufferMgr->ullWritePos && pBufferMgr->bWriteLock == FALSE)
	{
		return 0;
	}
	if(pBufferMgr->ullReadPos < pBufferMgr->ullWritePos)
	{
		ullSize = pBufferMgr->ullWritePos - pBufferMgr->ullReadPos;
	}
	else
	{
		ullSize = (pBufferMgr->ullBufferSize - pBufferMgr->ullReadPos) + pBufferMgr->ullWritePos;
	}
	return ullSize;
}

/**
 * @brief
 * @param       hCtrl : SSTR Handle, pData : data , ullSize : data size
 * @return      sstr error code
 * @date        2011/05/02
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HUINT64	SSTR_CONTROLLER_GetWritableSize (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t *pController = NULL;
	SSTR_CONTROLLER_BUFFER_t *pBufferMgr = NULL;
	HUINT64 ullSubSize = 0;
	HUINT64 ullSize = 0;
//	PrintEnter;
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pBufferMgr = &pController->BufferMgr;
	if(pBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(pBufferMgr->ullReadPos == pBufferMgr->ullWritePos && pBufferMgr->bWriteLock == TRUE)
	{
		return 0;
	}
	if(pBufferMgr->ullReadPos > pBufferMgr->ullWritePos)
	{
		ullSize = pBufferMgr->ullReadPos - pBufferMgr->ullWritePos;
	}
	else
	{
		ullSize = (pBufferMgr->ullBufferSize - pBufferMgr->ullWritePos) + pBufferMgr->ullReadPos;
	}
	return ullSize;
}


/**
 * @brief
 * @param       hCtrl : SSTR Handle, pData : data , ullSize : data size
 * @return      sstr error code
 * @date        2011/05/02
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HUINT64	SSTR_CONTROLLER_GetBufferSize (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t *pController = NULL;
	SSTR_CONTROLLER_BUFFER_t *pBufferMgr = NULL;
	HUINT64 ullSubSize = 0;
	HUINT64 ullSize = 0;
//	PrintEnter;
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	pBufferMgr = &pController->BufferMgr;
	if(pBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	return pBufferMgr->ullBufferSize;
}
#endif

SSTR_ERR 	SSTR_CONTROLLER_SetBaseUrl (SSTR_CTRL hCtrl, HINT8* pBaseUrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
//	mpdData_t *mpdData = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
#if 1
	if(pBaseUrl == NULL)
	{
		PrintError("%s(%d) Err> Base Url is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pController->pBaseUrl= VK_StrDup(pBaseUrl);
#endif
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}

SSTR_ERR	SSTR_CONTROLLER_SetBitrate (SSTR_CTRL hCtrl, HUINT32 ulBitrate)
{
	HINT32 nIdx = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurVideoIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
//			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pController->nCurVideoTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pStream->ulCurruntTrackNum);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					if(pTrack->ulBitrate == ulBitrate)
					{
						P_SSTR_CONTROLLER_Unlock(hCtrl);
						PrintExit;
						return SSTR_ERR_OK;
					}
					else
					{
						pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
						while(pTrackNode != NULL)
						{
							pTrack = (sstrTrack_t*)pTrackNode->pData;
							if(pTrack != NULL)
							{
								if(pTrack->ulBitrate == ulBitrate)
								{
//									pController->nCurVideoTrack = nIdx;
									pStream->ulCurruntTrackNum = nIdx;
									PrintDebug("%s(%d) Debug> Set Bitrate(%u)\n",__FUNCTION__,__LINE__,ulBitrate);
									P_SSTR_CONTROLLER_Unlock(hCtrl);
									PrintExit;
									return SSTR_ERR_OK;
								}
							}
							pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
							nIdx++;
						}
					}
				}
			}
		}
	}
	PrintError("%s(%d) Err> Bitrate(%u) is INVALID\n",__FUNCTION__,__LINE__,ulBitrate);
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

HBOOL	SSTR_CONTROLLER_SetLowestBitrate (SSTR_CTRL hCtrl)
{
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	SSTR_CONTROLLER_SetBitrate (hCtrl,	SSTR_CONTROLLER_GetMinBitrate(hCtrl));
	PrintExit;
	return TRUE;
}

HBOOL	SSTR_CONTROLLER_SetHighestBitrate (SSTR_CTRL hCtrl)
{
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	SSTR_CONTROLLER_SetBitrate (hCtrl,	SSTR_CONTROLLER_GetMaxBitrate(hCtrl));
	PrintExit;
	return TRUE;
}

HBOOL	SSTR_CONTROLLER_BitrateUp (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	HUINT32 ulBitrate = 0;
	HUINT32 ulTempBitrate = 0;
	HINT32 nIndex = -1;
	HINT32 i = 0;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurVideoIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
//			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet,pController->nCurVideoTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet,pStream->ulCurruntTrackNum);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*) pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulBitrate = pTrack->ulBitrate;
				}
			}
			ulTempBitrate = SSTR_CONTROLLER_GetMaxBitrate(hCtrl);
			if(ulTempBitrate == ulBitrate)
			{
				P_SSTR_CONTROLLER_Unlock(hCtrl);
				return TRUE;
			}
			pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
			while(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*) pTrackNode->pData;
				if(pTrack != NULL)
				{
					if(pTrack->ulBitrate > ulBitrate && ulTempBitrate >= pTrack->ulBitrate)
					{
						ulTempBitrate = pTrack->ulBitrate;
						nIndex = i;
					}
				}
				pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
				i++;
			}
		}
	}
	if(nIndex >= 0)
	{
//		pController->nCurVideoTrack = nIndex;
		pStream->ulCurruntTrackNum = nIndex;
		PrintDebug("%s(%d) Debug> Set Bitrate Up(%u)\n",__FUNCTION__,__LINE__,ulTempBitrate);
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return TRUE;
}

HBOOL	SSTR_CONTROLLER_BitrateDown (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	HUINT32 ulBitrate = 0;
	HUINT32 ulTempBitrate = 0;
	HINT32 nIndex = -1;
	HINT32 i = 0;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurVideoIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			//pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet,pController->nCurVideoTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet,pStream->ulCurruntTrackNum);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*) pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulBitrate = pTrack->ulBitrate;
				}
			}
			ulTempBitrate = SSTR_CONTROLLER_GetMinBitrate(hCtrl);
			if(ulTempBitrate == ulBitrate)
			{
				P_SSTR_CONTROLLER_Unlock(hCtrl);
				return TRUE;
			}
			pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
			while(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*) pTrackNode->pData;
				if(pTrack != NULL)
				{
					if(pTrack->ulBitrate < ulBitrate && ulTempBitrate <= pTrack->ulBitrate)
					{
						ulTempBitrate = pTrack->ulBitrate;
						nIndex = i;
					}
				}
				pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
				i++;
			}
		}
	}
	if(nIndex >= 0)
	{
//		pController->nCurVideoTrack = nIndex;
		pStream->ulCurruntTrackNum = nIndex;
		PrintDebug("%s(%d) Debug> Set Bitrate Down(%u)\n",__FUNCTION__,__LINE__,ulTempBitrate);
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return TRUE;
}

HBOOL	SSTR_CONTROLLER_SetClosestBitrate (SSTR_CTRL hCtrl, HUINT32 ulBitrate)
{
	HUINT32 ulLowerBitrate = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurVideoIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
			while(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					if(pTrack->ulBitrate < ulBitrate && pTrack->ulBitrate > ulLowerBitrate)
					{
						ulLowerBitrate = pTrack->ulBitrate;
					}
				}
				pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
			}
		}
	}
	if(ulLowerBitrate > 0)
	{
		SSTR_CONTROLLER_SetBitrate (hCtrl, ulLowerBitrate);
	}
	else
	{
		SSTR_CONTROLLER_SetBitrate (hCtrl,	SSTR_CONTROLLER_GetMinBitrate(hCtrl));
		// SSTR_CONTROLLER_SetBitrate (hCtrl, ullLowestBitrate);
	}
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return TRUE;
}

HUINT64	SSTR_CONTROLLER_GetMediaDuration (SSTR_CTRL hCtrl)
{
	HUINT64 ullDuration = 0;

	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	ullDuration = pMedia->ullDuration;
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ullDuration;
}

HUINT64	SSTR_CONTROLLER_GetMediaStartTime (SSTR_CTRL hCtrl)
{
	HUINT64 ullStartTime = 0;

	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	ullStartTime = pMedia->ullStartTime;
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ullStartTime;
}


HINT32	SSTR_CONTROLLER_GetTotalStreams (SSTR_CTRL hCtrl)
{
//	HINT32 nStreamCnt = 0;

	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	PrintExit;
	return UTIL_DLLIST_Size(&pMedia->streamSet);
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
}

HINT32	SSTR_CONTROLLER_GetTotalTracks (SSTR_CTRL hCtrl, HUINT32 unStream)
{
//	HINT32 nStreamCnt = 0;

	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return -1;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			return UTIL_DLLIST_Size(&pStream->trackSet);
		}
	}
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return -1;
}

SSTR_ERR	SSTR_CONTROLLER_GetFragmentUrl (SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, SSTR_URL_t *pUrl)
{
	HINT32 nIndex = 0;
	HUINT32 unFragment = 0;
//	HUINT32 unTrack = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);

//	pMedia = __sstrMedia(pController->parser->media);
	if(eType == SSTR_TYPE_VIDEO)
	{
		nIndex = pController->nCurVideoIndex;
		// unTrack = pController->nCurVideoTrack;
	}
	else if(eType == SSTR_TYPE_AUDIO)
	{
		nIndex = pController->nCurAudioIndex;
//		unTrack = pController->nCurAudioTrack;
	}
	else
	{
		PrintError("%s(%d) Err> Stream Type %d is not support\n",__FUNCTION__,__LINE__,eType );
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,(HUINT32)nIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			unFragment = pStream->ulFragmentSeqNum;
		}
		else
		{
			PrintError("%s(%d) Err> Stream Data is NULL\n",__FUNCTION__,__LINE__);
			P_SSTR_CONTROLLER_Unlock(hCtrl);
			PrintExit;
			return SSTR_ERR_FAIL;
		}
	}
	if(pStream == NULL)
	{
		PrintError("%s(%d) Err> Stream Data is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(P_SSTR_CONTROLLER_BuildUrl(pController,nIndex,pStream->ulCurruntTrackNum,unFragment,pUrl))
	{
		PrintError("%s(%d) Err> URL Build Fail\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	PrintDebug("%s(%d) Debug> Build URL : %s\n",__FUNCTION__,__LINE__,pUrl->url);
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}

SSTR_ERR	SSTR_CONTROLLER_Get_N_FragmentUrl (SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack, HUINT32 unFragment, SSTR_URL_t *pUrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController = __controller(hCtrl);
//	pMedia = __sstrMedia(pController->parser->media);
#if 1
	if(P_SSTR_CONTROLLER_BuildUrl(pController,unStream,unTrack,unFragment,pUrl))
	{
		PrintError("%s(%d) Err> URL Build Fail\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
#else
	int nLen = VK_strlen("http://59.12.191.102/stream/smth/Ctine/Ctine2005BluRay720px264DTS-WiKi.ism/QualityLevels(1644000)/Fragments(video=0)\0");
	VK_memcpy(szBuf,"http://59.12.191.102/stream/smth/Ctine/Ctine2005BluRay720px264DTS-WiKi.ism/QualityLevels(1644000)/Fragments(video=0)\0",
	nLen);
#endif

//	PrintError("%s(%d) Debug> URL : %s\n",__FUNCTION__,__LINE__,szBuf);
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}

SSTR_ERR SSTR_CONTROLLER_GetFragmentStartTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unFragment, HUINT64 *pullStartTime)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrFragment_t *pFragment = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pullStartTime == NULL)
	{
		PrintError("%s(%d) Err> pullStartTime is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, unFragment);
			if(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					*pullStartTime = pFragment->ullStartTime;
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return SSTR_ERR_OK;
				}
			}
		}
	}
	PrintError("%s(%d) Err> Fragment(%u) is INVALID\n",__FUNCTION__,__LINE__,unFragment);
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

SSTR_ERR SSTR_CONTROLLER_GetFragmentEndTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unFragment, HUINT64 *pullEndTime)
{
	HUINT64 ullStartTime=0;
	HUINT64 ullEndTime=0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrFragment_t *pFragment = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pullEndTime == NULL)
	{
		PrintError("%s(%d) Err> pullStartTime is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, unFragment);
			if(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullStartTime = pFragment->ullStartTime;
					ullEndTime = pFragment->ullDuration;
				}
			}
		}
	}
	if(	ullEndTime <= 0 )
	{
		if((unFragment + 1) < SSTR_CONTROLLER_GetTotalFragment(hCtrl,unStream))
		{
			pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, unFragment+1);
			if(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullEndTime = pFragment->ullStartTime;
				}
			}
		}
		else
		{
			PrintError("%s(%d) Err> Can't measure endtime\n",__FUNCTION__,__LINE__);
			P_SSTR_CONTROLLER_Unlock(hCtrl);
			PrintExit;
			return SSTR_ERR_FAIL;
		}
	}
	else
	{
		ullEndTime+=ullStartTime;
	}
	*pullEndTime = ullEndTime;
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}

SSTR_ERR SSTR_CONTROLLER_GetSubType(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex, HINT8* pFourCC)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pFourCC == NULL)
	{
		PrintError("%s(%d) Err> FourCC is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStreamIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			VK_MEM_Memcpy(pFourCC, pStream->subType, 4);
			PrintExit;
			return SSTR_ERR_OK;
		}
	}
	PrintExit;
	return SSTR_ERR_FAIL;
}

HUINT32 SSTR_CONTROLLER_GetCurAVStream(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	if(eType == SSTR_TYPE_VIDEO)
	{
		PrintExit;
		return pController->nCurVideoIndex;
	}
	else if(eType == SSTR_TYPE_AUDIO)
	{
		PrintExit;
		return pController->nCurAudioIndex;
	}
	PrintExit;
	return 0;
}

HUINT32 SSTR_CONTROLLER_GetCurAVTrack(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType)
{
	SSTR_CONTROLLER_t* pController = NULL;
	HUINT32 unStream = 0;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
#if 1
	if(eType == SSTR_TYPE_VIDEO)
	{
		unStream = pController->nCurVideoIndex;
	}
	else if(eType == SSTR_TYPE_AUDIO)
	{
		unStream = pController->nCurAudioIndex;
	}
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			PrintExit;
			return pStream->ulCurruntTrackNum;
		}
	}
#else
	if(eType == SSTR_TYPE_VIDEO)
	{
		PrintExit;
		return pController->nCurVideoTrack;
	}
	else if(eType == SSTR_TYPE_AUDIO)
	{
		PrintExit;
		return pController->nCurAudioTrack;
	}
#endif
	PrintExit;
	return 0;
}

HUINT32 SSTR_CONTROLLER_GetTrack(SSTR_CTRL hCtrl, HUINT32 unStream)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			PrintExit;
			return pStream->ulCurruntTrackNum;
		}
	}
	PrintExit;
	return 0;
}

SSTR_STREAM_TYPE_e SSTR_CONTROLLER_GetStreamType(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_TYPE_UNKNOWN;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStreamIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			PrintExit;
			return pStream->eType;
		}
	}
	PrintExit;
	return SSTR_TYPE_UNKNOWN;
}

HUINT32 SSTR_CONTROLLER_GetStreamIndex(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStreamIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			PrintExit;
			return pStream->ulIndex;
		}
	}
	PrintExit;
	return 0;
}

HINT8* SSTR_CONTROLLER_GetStreamName(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStreamIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			PrintExit;
			return (HINT8*)pStream->pName;
		}
	}
	PrintExit;
	return NULL;
}

HINT32 SSTR_CONTROLLER_GetStreamNameLen(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStreamIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			PrintExit;
			return pStream->nNameLen;
		}
	}
	PrintExit;
	return 0;
}

HINT8* SSTR_CONTROLLER_GetStreamLang(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStreamIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			PrintExit;
			return (HINT8*)pStream->pLanguage;
		}
	}
	PrintExit;
	return NULL;
}

HINT32 SSTR_CONTROLLER_GetStreamLangLen(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStreamIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			PrintExit;
			return pStream->nLangLen;
		}
	}
	PrintExit;
	return 0;
}

SSTR_STREAM_TYPE_e SSTR_CONTROLLER_GetCurStreamType(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}
	pController = __controller(hCtrl);
//	pMedia = __sstrMedia(pController->parser->media);
	return pController->eCurType;
}

SSTR_ERR SSTR_CONTROLLER_SetCurStreamType(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType)
{
	SSTR_CONTROLLER_t* pController = NULL;
	//sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	//pMedia = __sstrMedia(pController->parser->media);
	pController->eCurType = eType;
	return SSTR_ERR_OK;
}

HUINT32 	SSTR_CONTROLLER_GetMaxBitrate (SSTR_CTRL hCtrl)
{
	HUINT64 ullMaxBitrate = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurVideoIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
			while(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					if(pTrack->ulBitrate > ullMaxBitrate)
					{
						ullMaxBitrate = pTrack->ulBitrate;
					}
				}
				pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
			}
		}
	}
	PrintExit;
	return ullMaxBitrate;
}

HUINT32 	SSTR_CONTROLLER_GetMinBitrate (SSTR_CTRL hCtrl)
{
	HUINT64 ullMinBitrate = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	ullMinBitrate = SSTR_CONTROLLER_GetMaxBitrate (hCtrl);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurVideoIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
			while(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					if(pTrack->ulBitrate < ullMinBitrate)
					{
						ullMinBitrate = pTrack->ulBitrate;
					}
				}
				pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
			}
		}
	}
//	PrintExit;
	return ullMinBitrate;
}

HUINT32 	SSTR_CONTROLLER_GetMaxBitrateFromStream (SSTR_CTRL hCtrl, HUINT32 unStream)
{
	HUINT64 ullMaxBitrate = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
			while(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					if(pTrack->ulBitrate > ullMaxBitrate)
					{
						ullMaxBitrate = pTrack->ulBitrate;
					}
				}
				pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
			}
		}
	}
	PrintExit;
	return ullMaxBitrate;
}

HUINT32 	SSTR_CONTROLLER_GetMinBitrateFromStream(SSTR_CTRL hCtrl, HUINT32 unStream)
{
	HUINT64 ullMinBitrate = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	ullMinBitrate = SSTR_CONTROLLER_GetMaxBitrateFromStream (hCtrl,unStream);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
			while(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					if(pTrack->ulBitrate < ullMinBitrate)
					{
						ullMinBitrate = pTrack->ulBitrate;
					}
				}
				pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
			}
		}
	}
//	PrintExit;
	return ullMinBitrate;
}

HUINT32 	SSTR_CONTROLLER_GetMinBitrateTrack(SSTR_CTRL hCtrl, HUINT32 unStream)
{
	HUINT64 ullMinBitrate = 0;
	HUINT32 unTrack = 0;
	HUINT32 unIdx = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	ullMinBitrate = SSTR_CONTROLLER_GetMaxBitrateFromStream (hCtrl,unStream);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
			while(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					if(pTrack->ulBitrate < ullMinBitrate)
					{
						ullMinBitrate = pTrack->ulBitrate;
						unTrack = unIdx;
					}
				}
				unIdx++;
				pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
			}
		}
	}
//	PrintExit;
	return unTrack;
}

HBOOL SSTR_CONTROLLER_IsProtected (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	PrintExit;
	return pMedia->protection.bIsProtected;
}

HBOOL SSTR_CONTROLLER_IsLive (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	PrintExit;
	return pMedia->bIsLive;
}

HUINT64 SSTR_CONTROLLER_GetDVRWindowLength (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	PrintExit;
	return pMedia->ullDVRWindowLength;
}

HUINT8* SSTR_CONTROLLER_GetProtectionHeader (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(pMedia->protection.bIsProtected != TRUE)
	{
		PrintExit;
		return NULL;
	}
	PrintExit;
	return (HUINT8*)pMedia->protection.pHeaderContent;
}

HUINT8* SSTR_CONTROLLER_GetDecodedProtectionHeader (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(pMedia->protection.bIsProtected != TRUE)
	{
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pMedia->protection.pDecodedHeaderContent;
}


HINT32 SSTR_CONTROLLER_GetProtectionHeaderLen (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(pMedia->protection.bIsProtected != TRUE)
	{
		PrintExit;
		return 0;
	}
	PrintExit;
	return pMedia->protection.nHeaderContentLen;
}

HINT32 SSTR_CONTROLLER_GetDecodedProtectionHeaderLen (SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(pMedia->protection.bIsProtected != TRUE)
	{
		PrintExit;
		return 0;
	}
	PrintExit;
	return pMedia->protection.nDecodedHeaderContentLen;
}

#if 0
HBOOL SSTR_CONTROLLER_GetDecision(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return TRUE;
	}
	pController = __controller(hCtrl);
	return	pController->bDecision;
}

SSTR_ERR SSTR_CONTROLLER_SetDecision(SSTR_CTRL hCtrl, HBOOL bDecision)
{
	SSTR_CONTROLLER_t* pController = NULL;
	//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pController->bDecision = bDecision;
	return SSTR_ERR_OK;
}
#endif

SSTR_ERR	SSTR_CONTROLLER_OpenBuffer(SSTR_CTRL hCtrl, HUINT64 ullBufferSize, HINT32 nBufferType )
{
	SSTR_CONTROLLER_t* pController = NULL;
	//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);

	if(pController->hBufferMgr != NULL)
	{
		PrintError("%s(%d) Err> hBufferMgr is exist\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController->hBufferMgr = SSTR_BUFFERMGR_Open (ullBufferSize, nBufferType);
	if(pController->hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> hBufferMgr Open Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	return SSTR_ERR_OK;
}

SSTR_ERR	SSTR_CONTROLLER_CloseBuffer(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	(void)SSTR_BUFFERMGR_Close (pController->hBufferMgr);
	if(pController->hBufferMgr != NULL)
	{
		VK_MEM_Free(pController->hBufferMgr);
		pController->hBufferMgr = NULL;
	}
	PrintExit;
	return SSTR_ERR_OK;
}


/*
SSTR_ERR	SSTR_CONTROLLER_SetBuffer(SSTR_CTRL hCtrl, SSTR_BUFFERMGR_t* hBufferMgr)
{
	SSTR_CONTROLLER_t* pController = NULL;
	//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	if(pController->hBufferMgr != NULL)
	{
		PrintError("%s(%d) Err> hBufferMgr is exist\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController->hBufferMgr = hBufferMgr;
	return SSTR_ERR_OK;
}
*/
#if 0
SSTR_CONFIG_t	SSTR_CONTROLLER_GetConfig(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	SSTR_CONFIG_t stConfig;
	sstrMedia_t *pMedia = NULL;
//	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return stConfig;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
//	pController->stConfig.ulAudioStreamIndex = (pMedia->streamSet + pController->nCurAudioIndex)->ulIndex;
//	pController->stConfig.ulVideoStreamIndex = (pMedia->streamSet + pController->nCurVideoIndex)->ulIndex;
//	pController->stConfig.ulAudioTrackIndex = ((pMedia->streamSet + pController->nCurAudioIndex)->trackSet + pController->nCurAudioTrack)->ulIndex;
//	pController->stConfig.ulVideoTrackIndex = ((pMedia->streamSet + pController->nCurVideoIndex)->trackSet + pController->nCurVideoTrack)->ulIndex;
	return pController->stConfig;
}

SSTR_ERR SSTR_CONTROLLER_SetConfig(SSTR_CTRL hCtrl, SSTR_CONFIG_t stConfig)
{
	SSTR_CONTROLLER_t* pController = NULL;
	SSTR_ERR nRet = SSTR_ERR_OK;
	//	sstrMedia_t *pMedia = NULL;
//	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);

	SSTR_CONTROLLER_SetStreamByIndex(hCtrl, SSTR_TYPE_VIDEO, stConfig.ulVideoStreamIndex);
	SSTR_CONTROLLER_SetStreamByIndex(hCtrl, SSTR_TYPE_AUDIO, stConfig.ulAudioStreamIndex);
	SSTR_CONTROLLER_SetTrackByIndex(hCtrl, SSTR_TYPE_VIDEO, stConfig.ulVideoTrackIndex);
	SSTR_CONTROLLER_SetTrackByIndex(hCtrl, SSTR_TYPE_AUDIO, stConfig.ulAudioTrackIndex);

	if(stConfig.eDecision == SSTR_DECISION_AUTO)
	{
		switch(stConfig.eStartup)
		{
			default:
				PrintError ("%s(%d) STARTUP Mode is UNKNOWN -> Set LOWEST Mode \n",__FUNCTION__,__LINE__);
				stConfig.eStartup = SSTR_STARTUP_LOWEST;
			case SSTR_STARTUP_LOWEST :
				PrintDebug ("%s(%d) Enter SSTR_STARTUP_LOWEST Mode. \n",__FUNCTION__,__LINE__);
				nRet = SSTR_CONTROLLER_SetClosestBitrate(hCtrl, SSTR_CONTROLLER_GetMinBitrate(hCtrl)+1);
				break;
			case SSTR_STARTUP_HIGHEST :
				PrintDebug ("%s(%d) Enter SSTR_STARTUP_HIGHEST Mode. \n",__FUNCTION__,__LINE__);
				nRet = SSTR_CONTROLLER_SetClosestBitrate(hCtrl, SSTR_CONTROLLER_GetMaxBitrate(hCtrl)+1);
				break;
			case SSTR_STARTUP_AUTO :
				PrintDebug ("%s(%d) Enter SSTR_STARTUP_AUTO Mode. \n",__FUNCTION__,__LINE__);
				nRet = SSTR_CONTROLLER_SetClosestBitrate(hCtrl, SSTR_CONTROLLER_GetMinBitrate(hCtrl)+1);
				break;
		}
	}
	pController->stConfig = stConfig;
#if 0
	if(pController->stConfig.eStartup == SSTR_STARTUP_UNKNOWN)
	{
		switch(stConfig.eStartup)
		{
			default:
				PrintError ("%s(%d) STARTUP Mode is UNKNOWN -> Set LOWEST Mode \n",__FUNCTION__,__LINE__);
				stConfig.eStartup = SSTR_STARTUP_LOWEST;
			case SSTR_STARTUP_LOWEST :
				PrintDebug ("%s(%d) Enter SSTR_STARTUP_LOWEST Mode. \n",__FUNCTION__,__LINE__);
				nRet = SSTR_CONTROLLER_SetClosestBitrate(DI_STREAM_SSTR_GetHandle(), SSTR_CONTROLLER_GetMinBitrate(DI_STREAM_SSTR_GetHandle())+1);
				break;
			case SSTR_STARTUP_HIGHEST :
				PrintDebug ("%s(%d) Enter SSTR_STARTUP_HIGHEST Mode. \n",__FUNCTION__,__LINE__);
				nRet = SSTR_CONTROLLER_SetClosestBitrate(DI_STREAM_SSTR_GetHandle(), SSTR_CONTROLLER_GetMaxBitrate(DI_STREAM_SSTR_GetHandle())+1);
				break;
			case SSTR_STARTUP_AUTO :
				PrintDebug ("%s(%d) Enter SSTR_STARTUP_AUTO Mode. \n",__FUNCTION__,__LINE__);
				nRet = SSTR_CONTROLLER_SetClosestBitrate(DI_STREAM_SSTR_GetHandle(), SSTR_CONTROLLER_GetMinBitrate(DI_STREAM_SSTR_GetHandle())+1);
				break;
		}
		pController->stConfig.eStartup = stConfig.eStartup;
	}

	if(pController->stConfig.eDecision != stConfig.eDecision)
	{
		pController->stConfig.eDecision = stConfig.eDecision;
	}
	if(pController->stConfig.ulVideoStreamIndex != stConfig.ulVideoStreamIndex)
	{
		SSTR_CONTROLLER_SetStreamByIndex(DI_STREAM_SSTR_GetHandle(), SSTR_TYPE_VIDEO, stConfig.ulVideoStreamIndex);
	}
	if(pController->stConfig.ulAudioStreamIndex != stConfig.ulAudioStreamIndex)
	{
		SSTR_CONTROLLER_SetStreamByIndex(DI_STREAM_SSTR_GetHandle(), SSTR_TYPE_AUDIO, stConfig.ulAudioStreamIndex);
	}
	if(pController->stConfig.ulVideoTrackIndex != stConfig.ulVideoTrackIndex)
	{
		SSTR_CONTROLLER_SetTrackByIndex(DI_STREAM_SSTR_GetHandle(), SSTR_TYPE_VIDEO, stConfig.ulVideoTrackIndex);
	}
	if(pController->stConfig.ulAudioTrackIndex != stConfig.ulAudioTrackIndex)
	{
		SSTR_CONTROLLER_SetTrackByIndex(DI_STREAM_SSTR_GetHandle(), SSTR_TYPE_AUDIO, stConfig.ulAudioTrackIndex);
	}
#endif
	return nRet;
}
#endif

SSTR_BUFFERMGR_t*	SSTR_CONTROLLER_GetBuffer(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	//	sstrMedia_t *pMedia = NULL;
//	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	if(pController->hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> hBufferMgr is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return NULL;
	}
	return pController->hBufferMgr;
}

HBOOL	SSTR_CONTROLLER_CheckHeuristicsPanic(SSTR_CTRL hCtrl, HUINT64 ullMs)
{
	SSTR_CONTROLLER_t* pController = NULL;
	HUINT64 ullBufferSize = 0;
	HUINT64 ullReadableSize = 0;
	HUINT64 ullBandwidth = 0;
	//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	if(pController->hHeuristics == NULL)
	{
		PrintError("%s(%d) Err> hHeuristics is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	ullBufferSize = SSTR_BUFFERMGR_GetBufferSize(pController->hBufferMgr);
	ullReadableSize = SSTR_BUFFERMGR_GetReadableSize(pController->hBufferMgr);
	PrintDebug("%s(%d) Debug> Bufferfullness : %"PRId64"\n",__FUNCTION__,__LINE__,ullReadableSize);
	PrintDebug("%s(%d) Debug> Time : %"PRId64"\n",__FUNCTION__,__LINE__,ullMs);
	if((ullBufferSize * SSTR_HEURISTICS_CONFIG_PANIC_BUFFERFULLNESS / 100) >= ullReadableSize
	&& (ullMs >= ( ( SSTR_CONST_DEFAULT_DURATION * SSTR_CONST_MILLISEC  * 2))))
	{
		SSTR_HEURISTICS_SetHeuristicsState(pController->hHeuristics, SSTR_HEUR_STATE_PANIC);
		ullBandwidth = SSTR_CONTROLLER_GetMinBitrate(hCtrl);
		SSTR_CONTROLLER_SetBitrate(hCtrl,ullBandwidth); /* set lowest bitrate */
		PrintError("%s(%d) Debug> Heuristics PANIC Detected !! \n",__FUNCTION__,__LINE__);
		PrintError("%s(%d) Debug> Change to PANIC : %"PRId64"\n",__FUNCTION__,__LINE__,SSTR_BUFFERMGR_GetReadableSize(pController->hBufferMgr));
		return TRUE;
	}
	PrintExit;
	return FALSE;
}

SSTR_HEURISTICS_t*	SSTR_CONTROLLER_GetHeuristics(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	if(pController->hHeuristics == NULL)
	{
		PrintError("%s(%d) Err> hHeuristics is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pController->hHeuristics;
}

void	SSTR_CONTROLLER_ResetHeuristics(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}
	pController = __controller(hCtrl);
	if(pController->hHeuristics == NULL)
	{
		PrintError("%s(%d) Err> hHeuristics is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}
	#if 0
	(void)SSTR_HEURISTICS_Reset(pController->hHeuristics);
	#else
	(void)SSTR_HEURISTICS_SetHeuristicsState(pController->hHeuristics,SSTR_HEUR_STATE_TIMID);
	#endif
	PrintExit;
	return;
}

SSTR_ERR	SSTR_CONTROLLER_SetTrackByIndex(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HUINT32 nTrackIndex)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
//	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(eType == SSTR_TYPE_VIDEO)
	{
		pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,pController->nCurVideoIndex);
		if(pStreamNode != NULL)
		{
			pStream = (sstrStream_t*)pStreamNode->pData;
			if(pStream != NULL)
			{
				if( UTIL_DLLIST_Size(&pStream->trackSet) <= nTrackIndex)
				{
					PrintError("%s(%d) Err> Track Index is INVALID\n",__FUNCTION__,__LINE__);
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return SSTR_ERR_FAIL;
				}
			}
		}
		if(pStream == NULL)
		{
			PrintError("%s(%d) Err> Stream Data is NULL\n",__FUNCTION__,__LINE__);
			P_SSTR_CONTROLLER_Unlock(hCtrl);
			PrintExit;
			return SSTR_ERR_FAIL;
		}
		//pController->nCurVideoTrack = nTrackIndex;
		pStream->ulCurruntTrackNum = nTrackIndex;
	}
	else if(eType == SSTR_TYPE_AUDIO)
	{
		pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,pController->nCurAudioIndex);
		if(pStreamNode != NULL)
		{
			pStream = (sstrStream_t*)pStreamNode->pData;
			if(pStream != NULL)
			{
				if( UTIL_DLLIST_Size(&pStream->trackSet) <= nTrackIndex)
				{
					PrintError("%s(%d) Err> Track Index is INVALID\n",__FUNCTION__,__LINE__);
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return SSTR_ERR_FAIL;
				}
			}
			else
			{
				PrintError("%s(%d) Err> Stream Data is NULL\n",__FUNCTION__,__LINE__);
				P_SSTR_CONTROLLER_Unlock(hCtrl);
				PrintExit;
				return SSTR_ERR_FAIL;
			}
		}
		// pController->nCurAudioTrack = nTrackIndex;
		if(pStream == NULL)
		{
			PrintError("%s(%d) Err> Stream Data is NULL\n",__FUNCTION__,__LINE__);
			P_SSTR_CONTROLLER_Unlock(hCtrl);
			PrintExit;
			return SSTR_ERR_FAIL;
		}
		pStream->ulCurruntTrackNum = nTrackIndex;
	}
	else
	{
		PrintError("%s(%d) Err> Stream Type %d is not support\n",__FUNCTION__,__LINE__,eType );
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}

SSTR_CONTENTS_TYPE_e	SSTR_CONTROLLER_SetDefaultAVStream(SSTR_CTRL hCtrl)
{
	HINT32 nIdx = 0;
	HBOOL bAudioDetected = FALSE;
	HBOOL bVideoDetected = FALSE;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	SSTR_CONTENTS_TYPE_e eAVType = SSTR_CONTENTS_NONE;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_GetHeadNode(&pMedia->streamSet);
	while(pStreamNode != NULL)
	{
		if(bVideoDetected == TRUE && bAudioDetected == TRUE)
		{
			break;
		}
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			if(pStream->eType == SSTR_TYPE_VIDEO && bVideoDetected == FALSE)
			{
				pController->nCurVideoIndex = nIdx;
				eAVType += SSTR_CONTENTS_VIDEO;
				bVideoDetected = TRUE;

			}
			else if(pStream->eType == SSTR_TYPE_AUDIO && bAudioDetected == FALSE)
			{
				pController->nCurAudioIndex = nIdx;
				eAVType += SSTR_CONTENTS_AUDIO;
				bAudioDetected = TRUE;
			}
		}
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
		nIdx++;
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return eAVType;
}

SSTR_ERR	SSTR_CONTROLLER_SetStreamByIndex(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HUINT32 nStreamIndex)
{
	HINT32 nIdx = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_GetHeadNode(&pMedia->streamSet);
	while(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			if(pStream->eType == eType && nStreamIndex == pStream->ulIndex)
			{
				if(eType == SSTR_TYPE_VIDEO)
				{
					pController->nCurVideoIndex = nIdx;
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return SSTR_ERR_OK;
				}
				else if(eType == SSTR_TYPE_AUDIO)
				{
					pController->nCurAudioIndex = nIdx;
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return SSTR_ERR_OK;
				}
			}
		}
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
		nIdx++;
	}
	PrintError("%s(%d) Err> Stream(%d) is not exist\n",__FUNCTION__,__LINE__,nStreamIndex);
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

SSTR_ERR	SSTR_CONTROLLER_SetStreamByArrayIndex(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HUINT32 nStreamIndex)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,nStreamIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			if(pStream->eType == eType)
			{
				if(eType == SSTR_TYPE_VIDEO)
				{
					pController->nCurVideoIndex = nStreamIndex;
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return SSTR_ERR_OK;
				}
				else if(eType == SSTR_TYPE_AUDIO)
				{
					pController->nCurAudioIndex = nStreamIndex;
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return SSTR_ERR_OK;
				}
			}
		}
	}
	PrintError("%s(%d) Err> Stream(%d) is not exist\n",__FUNCTION__,__LINE__,nStreamIndex);
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

SSTR_ERR	SSTR_CONTROLLER_SetStreamByName(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HINT8 *pStreamName)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	HINT32 nIdx = 0;
	HINT8 *pName = NULL;
	HINT32 nNameLen = 0;
	HINT32 nStreamNameLen = 0;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(pStreamName == NULL)
	{
		PrintError("%s(%d) Err> Stream Name is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	nStreamNameLen = VK_strlen(pStreamName);
	pStreamNode = UTIL_DLLIST_GetHeadNode(&pMedia->streamSet);
	while(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pName = (HINT8*)pStream->pName;
			nNameLen = pStream->nNameLen;
			if(nStreamNameLen < nNameLen)
			{
				nNameLen = nStreamNameLen;
			}
			if( !VK_strncmp(pStreamName,pName,nNameLen) && pStream->eType == eType )
			{
				if(eType == SSTR_TYPE_VIDEO)
				{
					pController->nCurVideoIndex = nIdx;
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return SSTR_ERR_OK;
				}
				else if(eType == SSTR_TYPE_AUDIO)
				{
					pController->nCurAudioIndex = nIdx;
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return SSTR_ERR_OK;
				}
			}
		}
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
		nIdx++;
	}
	PrintError("%s(%d) Err> Stream(%s) is not exist\n",__FUNCTION__,__LINE__,pStreamName);
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

SSTR_ERR	SSTR_CONTROLLER_GetFragmentByTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullTime, HUINT32 *punFragment)
{
	HUINT32 unIdx = 0;
	HUINT64 ullStartTime = 0;
	HUINT64 ullLastStartTime = 0;
	HUINT64 ullDuration =  0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrFragment_t *pFragment = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(punFragment == NULL)
	{
		PrintError("%s(%d) Err> punFragment is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
//	ullTime *= 10000; /* micro seconds? */
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
#if 0
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_GetHeadNode(&pStream->fragmentSet);
			while(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullStartTime = pFragment->ullStartTime;
					ullDuration = pFragment->ullDuration;
					if( ullStartTime <= ullTime && (ullDuration + ullStartTime) > ullTime )
					{
						//P_SSTR_CONTROLLER_Unlock(hCtrl);
						*punFragment = unIdx;
						PrintExit;
						return SSTR_ERR_OK;
					}
				}
				pFragNode = UTIL_DLLIST_NextNode(pFragNode);
				unIdx++;
			}
		}
#else
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_GetTailNode(&pStream->fragmentSet);
			while(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullStartTime = pFragment->ullStartTime;
					ullDuration = pFragment->ullDuration;
					if(ullDuration == 0)
					{
						ullDuration = ullLastStartTime - ullStartTime;
					}
					if( ullStartTime <= ullTime && (ullDuration + ullStartTime) > ullTime )
					{
						//P_SSTR_CONTROLLER_Unlock(hCtrl);
						*punFragment = SSTR_CONTROLLER_GetTotalFragment(hCtrl,unStream) - (unIdx + 1);
						VK_printf("*punFragment %u\n",*punFragment);
						PrintExit;
						return SSTR_ERR_OK;
					}
				}
				ullLastStartTime = ullStartTime;
				pFragNode = UTIL_DLLIST_PrevNode(pFragNode);
				unIdx++;
			}
		}

#endif
	}
	PrintError("%s(%d) Err> ullTime(%"PRId64") is INVALID\n",__FUNCTION__,__LINE__,ullTime);
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

SSTR_ERR	SSTR_CONTROLLER_GetFragmentByStartTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullStartTime, HUINT32 *punFragment)
{
	HUINT32 unIdx = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrFragment_t *pFragment = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(punFragment == NULL)
	{
		PrintError("%s(%d) Err> punFragment is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
//	ullTime *= 10000; /* micro seconds? */
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_GetHeadNode(&pStream->fragmentSet);
			while(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					if( pFragment->ullStartTime == ullStartTime)
					{
						//P_SSTR_CONTROLLER_Unlock(hCtrl);
						*punFragment = unIdx;
						PrintExit;
						return SSTR_ERR_OK;
					}
				}
				pFragNode = UTIL_DLLIST_NextNode(pFragNode);
				unIdx++;
			}
		}
	}
	PrintError("%s(%d) Err> ullTime(%"PRId64") is INVALID\n",__FUNCTION__,__LINE__,ullStartTime);
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

SSTR_ERR SSTR_CONTROLLER_GetFragment(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullTime, HUINT32 *punFragment)
{
	HUINT32 unIdx = 0;
	HUINT64 ullStartTime = 0;
	HUINT64 ullDuration =  0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrFragment_t *pFragment = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	if(punFragment == NULL)
	{
		PrintError("%s(%d) Err> punFragment is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
//	ullTime *= 10000; /* micro seconds? */
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_GetHeadNode(&pStream->fragmentSet);
			while(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullStartTime = pFragment->ullStartTime;
					ullDuration = pFragment->ullDuration;
					if( ullStartTime <= ullTime && (ullDuration + ullStartTime) > ullTime )
					{
			//			P_SSTR_CONTROLLER_Unlock(hCtrl);
						*punFragment = unIdx;
						PrintExit;
						return SSTR_ERR_OK;
					}
				}
				pFragNode = UTIL_DLLIST_NextNode(pFragNode);
				unIdx++;
			}
		}
	}
	PrintError("%s(%d) Err> ullTime(%"PRId64") is INVALID\n",__FUNCTION__,__LINE__,ullTime);
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

SSTR_ERR	SSTR_CONTROLLER_SetFragment (SSTR_CTRL hCtrl, HUINT32 ulStream, HUINT32 ulFragment)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			if(UTIL_DLLIST_Size(&pStream->fragmentSet) > ulFragment)
			{
				pStream->ulFragmentSeqNum = ulFragment;
				P_SSTR_CONTROLLER_Unlock(hCtrl);
				PrintExit;
				return SSTR_ERR_OK;
			}
		}
	}
	PrintError("%s(%d) Err> Fragment(%d) is INVALID\n",__FUNCTION__,__LINE__,ulFragment);
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

SSTR_ERR 	SSTR_CONTROLLER_NextFragment (SSTR_CTRL hCtrl, HUINT32 ulStream)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			if(UTIL_DLLIST_Size(&pStream->fragmentSet) > pStream->ulFragmentSeqNum + 1)
			{
				pStream->ulFragmentSeqNum++;
				P_SSTR_CONTROLLER_Unlock(hCtrl);
				PrintExit;
				return SSTR_ERR_OK;
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_FAIL;
}

SSTR_ERR 	SSTR_CONTROLLER_PrevFragment (SSTR_CTRL hCtrl, HUINT32 ulStream)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			if(pStream->ulFragmentSeqNum == 0)
			{
				PrintError("%s(%d) Err> Current Fragment is a First Fragment\n",__FUNCTION__,__LINE__);
				P_SSTR_CONTROLLER_Unlock(hCtrl);
				PrintExit;
				return SSTR_ERR_FAIL;
			}
			pStream->ulFragmentSeqNum--;
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}

HUINT32	SSTR_CONTROLLER_GetTotalFragment (SSTR_CTRL hCtrl, HUINT32 unStream)
{
	HUINT32	ulChunks = 0;

	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return -1;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			ulChunks = UTIL_DLLIST_Size(&pStream->fragmentSet);
		}
	}
//	ulChunks = (pMedia->streamSet+pController->nCurVideoIndex)->ulChunks;
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulChunks;
}

HUINT32 SSTR_CONTROLLER_GetStreamMaxWidth(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex)
{
	HUINT32 ulMaxWidth = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
//	ullTime *= 10000; /* micro seconds? */
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStreamIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			if(pStream->eType == SSTR_TYPE_VIDEO)
			{
				ulMaxWidth = pStream->ulMaxWidth;
			}
			if(ulMaxWidth == 0)
			{
				pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
				while(pTrackNode != NULL)
				{
					pTrack = (sstrTrack_t*)pTrackNode->pData;
					if(pTrack != NULL)
					{
						if(pTrack->ulMaxWidth > ulMaxWidth)
						{
							ulMaxWidth = pTrack->ulMaxWidth;
						}
					}
					pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
				}
			}
		}
	}
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulMaxWidth;
}

HUINT32 SSTR_CONTROLLER_GetStreamMaxHeight(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex)
{
	HUINT32 ulMaxHeight = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
//	ullTime *= 10000; /* micro seconds? */
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStreamIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			if(pStream->eType == SSTR_TYPE_VIDEO)
			{
				ulMaxHeight = pStream->ulMaxHeight;
			}
			if(ulMaxHeight == 0)
			{
				pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
				while(pTrackNode != NULL)
				{
					pTrack = (sstrTrack_t*)pTrackNode->pData;
					if(pTrack != NULL)
					{
						if(pTrack->ulMaxHeight > ulMaxHeight)
						{
							ulMaxHeight = pTrack->ulMaxHeight;
						}
					}
					pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
				}
			}
		}
	}
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulMaxHeight;
}

HUINT32 SSTR_CONTROLLER_GetMaxWidth(SSTR_CTRL hCtrl)
{
	HUINT32 ulMaxWidth = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
//	ullTime *= 10000; /* micro seconds? */
	pStreamNode = UTIL_DLLIST_GetHeadNode(&pMedia->streamSet);
	while(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			if(pStream->eType == SSTR_TYPE_VIDEO && ulMaxWidth < pStream->ulMaxWidth)
			{
				ulMaxWidth = pStream->ulMaxWidth;
			}
		}
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
	}
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulMaxWidth;
}

HUINT32 SSTR_CONTROLLER_GetMaxHeight(SSTR_CTRL hCtrl)
{
	HUINT32 ulMaxHeight = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
//	ullTime *= 10000; /* micro seconds? */
	pStreamNode = UTIL_DLLIST_GetHeadNode(&pMedia->streamSet);
	while(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			if(pStream->eType == SSTR_TYPE_VIDEO && ulMaxHeight < pStream->ulMaxHeight)
			{
				ulMaxHeight = pStream->ulMaxHeight;
			}
		}
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
	}
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulMaxHeight;
}

HINT8* SSTR_CONTROLLER_GetTrackFourCC(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	HINT8 *pFourCC = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, unTrack);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					pFourCC = (HINT8*)pTrack->FourCC;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return pFourCC;
}


HINT8* SSTR_CONTROLLER_GetCurTrackFourCC(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType)
{
	HINT32 nIndex = 0, nTrack = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	HINT8 *pFourCC = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(eType == SSTR_TYPE_VIDEO)
	{
		nIndex = pController->nCurVideoIndex;
		// nTrack = pController->nCurVideoTrack;
	}
	else if(eType == SSTR_TYPE_AUDIO)
	{
		nIndex = pController->nCurAudioIndex;
		// nTrack = pController->nCurAudioTrack;
	}
	else
	{
		PrintError("%s(%d) Err> INVALID Type(%d)\n",__FUNCTION__,__LINE__,eType);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, nIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, nTrack);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					pFourCC = (HINT8*)pTrack->FourCC;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return pFourCC;
}

HINT8* SSTR_CONTROLLER_GetStreamSubtype(SSTR_CTRL hCtrl, HUINT32 unStream)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	HINT8 *pSubType = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pSubType = (HINT8*)pStream->subType;
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return pSubType;
}


HINT8* SSTR_CONTROLLER_GetCurStreamSubtype(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType)
{
	HINT32 nIndex = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	HINT8 *pSubType = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(eType == SSTR_TYPE_VIDEO)
	{
		nIndex = pController->nCurVideoIndex;
	}
	else if(eType == SSTR_TYPE_AUDIO)
	{
		nIndex = pController->nCurAudioIndex;
	}
	else
	{
		PrintError("%s(%d) Err> INVALID Type(%d)\n",__FUNCTION__,__LINE__,eType);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, nIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pSubType = (HINT8*)pStream->subType;
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return pSubType;
}

HUINT32 SSTR_CONTROLLER_GetTrackIndex(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	HUINT32 ulIndex = 0;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, unTrack);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulIndex = pTrack->ulIndex;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulIndex;
}

HUINT32 SSTR_CONTROLLER_GetTrackBitrate(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	HUINT32 ulBitrate = 0;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, unTrack);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulBitrate = pTrack->ulBitrate;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulBitrate;
}

HINT8* SSTR_CONTROLLER_GetTrackCodecPrivate(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	HINT8 *pCodecPrivate = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);

	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, unStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
//			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, nTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet,unTrack);

			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					pCodecPrivate = (HINT8*)pTrack->pCodecPrivateData;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return pCodecPrivate;
}

HINT8* SSTR_CONTROLLER_GetCurTrackCodecPrivate(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType)
{
	HINT32 nIndex = 0;
//	HINT32 nTrack = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	HINT8 *pCodecPrivate = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(eType == SSTR_TYPE_VIDEO)
	{
		nIndex = pController->nCurVideoIndex;
//		nTrack = pController->nCurVideoTrack;
	}
	else if(eType == SSTR_TYPE_AUDIO)
	{
		nIndex = pController->nCurAudioIndex;
//		nTrack = pController->nCurAudioTrack;
	}
	else
	{
		PrintError("%s(%d) Err> INVALID Type(%d)\n",__FUNCTION__,__LINE__,eType);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, nIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
//			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, nTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet,pStream->ulCurruntTrackNum);

			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					pCodecPrivate = (HINT8*)pTrack->pCodecPrivateData;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return pCodecPrivate;
}

HUINT32 SSTR_CONTROLLER_GetCurAudioPacketSize(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	HUINT32 ulPacketSize = 0;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurAudioIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			// pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pController->nCurAudioTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pStream->ulCurruntTrackNum);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulPacketSize = pTrack->ulPacketSize;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulPacketSize;
}

HUINT32 SSTR_CONTROLLER_GetCurAudioBitrate(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	HUINT32 ulBitrate = 0;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurAudioIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			// pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pController->nCurAudioTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pStream->ulCurruntTrackNum);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulBitrate = pTrack->ulBitrate;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulBitrate;
}


HUINT32 SSTR_CONTROLLER_GetCurAudioSamplingRate(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	HUINT32 ulSamplingRate = 0;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurAudioIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			// pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pController->nCurAudioTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pStream->ulCurruntTrackNum);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulSamplingRate = pTrack->ulSamplingRate;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulSamplingRate;
}

HUINT32 SSTR_CONTROLLER_GetCurAudioChannels(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	HUINT32 ulChannels = 0;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurAudioIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			//pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pController->nCurAudioTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pStream->ulCurruntTrackNum);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulChannels = pTrack->ulChannels;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulChannels;
}

HUINT32 SSTR_CONTROLLER_GetCurAudioBitsPerSample(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	HUINT32 ulBitsPerSample = 0;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurAudioIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			//pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pController->nCurAudioTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pStream->ulCurruntTrackNum);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulBitsPerSample = pTrack->ulBitsPerSample;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulBitsPerSample;
}

HUINT32 SSTR_CONTROLLER_GetCurAudioTag(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	HUINT32 ulAudioTag = 0;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurAudioIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			//pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pController->nCurAudioTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pStream->ulCurruntTrackNum);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulAudioTag = pTrack->ulAudioTag;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulAudioTag;
}

HUINT32 SSTR_CONTROLLER_GetCurFragment(SSTR_CTRL hCtrl, HUINT32 ulStream)
{
	HUINT32 ulFragmentId = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrStream_t *pStream = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet,ulStream);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			ulFragmentId = pStream->ulFragmentSeqNum;
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulFragmentId;
}

SSTR_ERR SSTR_CONTROLLER_GetSummary(SSTR_CTRL hCtrl, SSTR_SUMMARY_t *pSummary)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
//	pSummary->ulCurFragment = pController->ulCurFragment;
	pSummary->eCurDownloadType = pController->eCurType;
	pSummary->nCurVideoIndex = pController->nCurVideoIndex;
	pSummary->nCurAudioIndex = pController->nCurAudioIndex;
//	pSummary->nCurVideoTrack = pController->nCurVideoTrack;
//	pSummary->nCurAudioTrack = pController->nCurAudioTrack;
	pSummary->ulCurVideoBitrate = SSTR_CONTROLLER_GetCurBitrate(hCtrl);
	pSummary->ulCurAudioBitrate = 0; /* not supported yet */
	pSummary->ulBandwidth = SSTR_HEURISTICS_GetBandwidth(pController->hHeuristics);
	pSummary->eHeurState = SSTR_HEURISTICS_GetHeuristicsState(pController->hHeuristics);
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}

HUINT64 SSTR_CONTROLLER_GetCurFragmentStartTime(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType)
{
	HINT32 nIndex = 0;
	HUINT64 ullStartTime = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrFragment_t *pFragment = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(eType == SSTR_TYPE_VIDEO)
	{
		nIndex = pController->nCurVideoIndex;
	}
	else if(eType == SSTR_TYPE_AUDIO)
	{
		nIndex = pController->nCurAudioIndex;
	}
	else
	{
		PrintError("%s(%d) Err> INVALID Type(%d)\n",__FUNCTION__,__LINE__,eType);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}

	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, nIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, pStream->ulFragmentSeqNum);
			if(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullStartTime = pFragment->ullStartTime;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ullStartTime;
}

HUINT64 SSTR_CONTROLLER_GetCurFragmentEndTime(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType)
{
	HINT32 nIndex = 0;
	HUINT64 ullStartTime = 0;
	HUINT64 ullEndTime = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrFragment_t *pFragment = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(eType == SSTR_TYPE_VIDEO)
	{
		nIndex = pController->nCurVideoIndex;
	}
	else if (eType == SSTR_TYPE_AUDIO)
	{
		nIndex = pController->nCurAudioIndex;
	}
	else
	{
		PrintError("%s(%d) Err> INVALID Type(%d)\n",__FUNCTION__,__LINE__,eType);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, nIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, pStream->ulFragmentSeqNum);
			if(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullStartTime = pFragment->ullStartTime;
					ullEndTime = pFragment->ullDuration;
				}
				else
				{
					PrintError("%s(%d) Err> Fragment Data is NULL\n",__FUNCTION__,__LINE__);
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return 0;
				}
			}
		}
		else
		{
			PrintError("%s(%d) Err> Stream Data is NULL\n",__FUNCTION__,__LINE__);
			P_SSTR_CONTROLLER_Unlock(hCtrl);
			PrintExit;
			return 0;
			}
		}
	if(pStream == NULL)
	{
		PrintError("%s(%d) Err> Stream Data is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	if(	ullEndTime <= 0 )
	{
		if( pStream->ulFragmentSeqNum + 1 < SSTR_CONTROLLER_GetTotalFragment(hCtrl,(HUINT32)nIndex))
		{
			pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, pStream->ulFragmentSeqNum+1);
			if(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullEndTime = pFragment->ullStartTime;
				}
				else
				{
					PrintError("%s(%d) Err> Fragment Data is NULL\n",__FUNCTION__,__LINE__);
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return 0;
				}
			}
		}
		else
		{
			PrintError("%s(%d) Err> Can't measure endtime\n",__FUNCTION__,__LINE__);
			P_SSTR_CONTROLLER_Unlock(hCtrl);
			PrintExit;
			return 0;
		}
	}
	else
	{
		ullEndTime+=ullStartTime;
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ullEndTime;

}

HUINT64 SSTR_CONTROLLER_GetCurFragmentDuration(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType)
{
	HINT32 nIndex = 0;
	HUINT64 ullStartTime = 0;
	HUINT64 ullEndTime = 0;
	HUINT64 ullDuration = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrFragment_t *pFragment = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if(eType == SSTR_TYPE_VIDEO)
	{
		nIndex = pController->nCurVideoIndex;
	}
	else if (eType == SSTR_TYPE_AUDIO)
	{
		nIndex = pController->nCurAudioIndex;
	}
	else
	{
		PrintError("%s(%d) Err> INVALID Type(%d)\n",__FUNCTION__,__LINE__,eType);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, nIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, pStream->ulFragmentSeqNum);
			if(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullDuration	= pFragment->ullDuration;
				}
				else
				{
					PrintError("%s(%d) Err> Fragment Data is NULL\n",__FUNCTION__,__LINE__);
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return 0;
				}
			}
		}
	}
	if(pFragment == NULL)
	{
		PrintError("%s(%d) Err> Fragment Data is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	if( ullDuration == 0)
	{
		ullStartTime = pFragment->ullStartTime;
		if( pStream->ulFragmentSeqNum + 1 < SSTR_CONTROLLER_GetTotalFragment(hCtrl,(HUINT32)nIndex))
		{
			pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, pStream->ulFragmentSeqNum+1);
			if(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullEndTime = pFragment->ullStartTime;
				}
			}
		}
		else
		{
			PrintError("%s(%d) Err> Can't measure endtime\n",__FUNCTION__,__LINE__);
			P_SSTR_CONTROLLER_Unlock(hCtrl);
			PrintExit;
			return 0;
		}
		ullDuration = ullEndTime - ullStartTime;
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ullDuration;
}

HUINT32 SSTR_CONTROLLER_GetCurFragmentDurationForHeur(SSTR_CTRL hCtrl)
{
	HINT32 nIndex = 0;
	HUINT64 ullStartTime = 0;
	HUINT64 ullEndTime = 0;
	HUINT64 ullDuration = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrFragment_t *pFragment = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	nIndex = pController->nCurVideoIndex;
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, nIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, pStream->ulFragmentSeqNum);
			if(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullDuration	= pFragment->ullDuration;
				}
				else
				{
					PrintError("%s(%d) Err> Fragment Data is NULL\n",__FUNCTION__,__LINE__);
					P_SSTR_CONTROLLER_Unlock(hCtrl);
					PrintExit;
					return 0;
				}
			}
		}
	}
	if(pFragment == NULL)
	{
		PrintError("%s(%d) Err> Fragment Data is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return 0;
	}
	if( ullDuration == 0)
	{
		ullStartTime = pFragment->ullStartTime;
		if( pStream->ulFragmentSeqNum + 1 < SSTR_CONTROLLER_GetTotalFragment(hCtrl,(HUINT32)nIndex))
		{
			pFragNode = UTIL_DLLIST_Get_n_Node(&pStream->fragmentSet, pStream->ulFragmentSeqNum+1);
			if(pFragNode != NULL)
			{
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					ullEndTime = pFragment->ullStartTime;
				}
			}
		}
		else
		{
			PrintError("%s(%d) Err> Can't measure endtime\n",__FUNCTION__,__LINE__);
			P_SSTR_CONTROLLER_Unlock(hCtrl);
			PrintExit;
			return 0;
		}
		ullDuration = ullEndTime - ullStartTime;
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ullDuration / SSTR_CONST_MICROSEC;
}


HUINT32	SSTR_CONTROLLER_GetCurBitrate (SSTR_CTRL hCtrl)
{
	HUINT32 ulCurrentBitrate = 0;
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	PrintEnter;
	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		P_SSTR_CONTROLLER_Unlock(hCtrl);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pMedia->streamSet, pController->nCurVideoIndex);
	if(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*)pStreamNode->pData;
		if(pStream != NULL)
		{
//			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pController->nCurVideoTrack);
			pTrackNode = UTIL_DLLIST_Get_n_Node(&pStream->trackSet, pStream->ulCurruntTrackNum);
			if(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*)pTrackNode->pData;
				if(pTrack != NULL)
				{
					ulCurrentBitrate = pTrack->ulBitrate;
				}
			}
		}
	}
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return ulCurrentBitrate;
}

#if 0
SSTR_ERR SSTR_CONTROLLER_SetChangeFragment(SSTR_CTRL hCtrl,HBOOL bRequest)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;

	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController->bChangeFragment = bRequest;
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}


SSTR_ERR SSTR_CONTROLLER_SetRequestSeek(SSTR_CTRL hCtrl,HBOOL bRequest)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;

	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController->bRequestedSeek = bRequest;
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}

SSTR_ERR SSTR_CONTROLLER_SetTimeForSeek(SSTR_CTRL hCtrl, HUINT64 ullTime)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;

	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	if(pController->bRequestedSeek == TRUE)
	{
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	P_SSTR_CONTROLLER_Lock(hCtrl);
	pController->ullTimeForSeek = ullTime;
//	pController->bRequestedSeek = TRUE;
	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return SSTR_ERR_OK;
}

HUINT64 SSTR_CONTROLLER_GetTimeForSeek(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		return 0;
	}
	pController = __controller(hCtrl);
//	pController->bRequestedSeek = FALSE;
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
	PrintExit;
	return pController->ullTimeForSeek;
}

HBOOL SSTR_CONTROLLER_IsSeekRequested(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
//	PrintEnter;
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		return 0;
	}
	pController = __controller(hCtrl);
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
//	PrintExit;
	return pController->bRequestedSeek;
}

HBOOL SSTR_CONTROLLER_IsValidTimeForSeek(SSTR_CTRL hCtrl, HUINT64 ullTime)
{
	SSTR_CONTROLLER_t* pController = NULL;
	sstrMedia_t *pMedia = NULL;
//	PrintEnter;
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		return 0;
	}
	pController = __controller(hCtrl);
	pMedia = __sstrMedia(pController->parser->media);
	if( pMedia->ullDuration < ullTime )
	{
		return FALSE;
	}
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
//	PrintExit;
	return TRUE;
}

HBOOL SSTR_CONTROLLER_IsChangeFragment(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
//	PrintEnter;
//	P_SSTR_CONTROLLER_Lock(hCtrl);
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
//		P_SSTR_CONTROLLER_Unlock(hCtrl);
		return 0;
	}
	pController = __controller(hCtrl);
//	P_SSTR_CONTROLLER_Unlock(hCtrl);
//	PrintExit;
	return pController->bChangeFragment;
}



SSTR_ERR SSTR_CONTROLLER_SetStreamLength(SSTR_CTRL hCtrl, HINT64 nLength)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return SSTR_ERR_FAIL;
	}
	pController = __controller(hCtrl);
	pController->nStreamLength = nLength;
	PrintExit;
	return SSTR_ERR_OK;
}
HINT64 SSTR_CONTROLLER_GetStreamLength(SSTR_CTRL hCtrl)
{
	SSTR_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(hCtrl == NULL || ((SSTR_CONTROLLER_t*)hCtrl)->parser == NULL
		|| ((SSTR_CONTROLLER_t*)hCtrl)->parser->media == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	PrintExit;
	return pController->nStreamLength;
}
#endif

/*@}*/
